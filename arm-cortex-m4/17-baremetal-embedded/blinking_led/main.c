
#include <stdint.h>
// #include <stdio.h>
#include "led.h"

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

// 1. defining task handler
void task1_handler(void);
void task2_handler(void);
void task3_handler(void);
void task4_handler(void);

// 2. macros for stack location
#define SIZE_TASK_STACK		1024U
#define SIZE_SCHED_STACK	1024U
#define SRAM_START			0x20000000U
// setting this to 64 instead of 128 since
// size of SRAM that we can use is 64.
#define SIZE_SRAM			((64) * (1024))
#define SRAM_END			((SRAM_START) + (SIZE_SRAM))
#define T1_STACK_START		SRAM_END
#define T2_STACK_START		( (SRAM_END) - (1 * SIZE_TASK_STACK) )
#define T3_STACK_START		( (SRAM_END) - (2 * SIZE_TASK_STACK) )
#define T4_STACK_START		( (SRAM_END) - (3 * SIZE_TASK_STACK) )
#define IDLE_STACK_START	( (SRAM_END) - (4 * SIZE_TASK_STACK) )
#define SCHED_STACK_START	( (SRAM_END) - (5 * SIZE_TASK_STACK) )


// 3. defining init systick
#define TICK_HZ 	1000U //1kHz
#define HSI_CLK		64000000U
#define SYSTICK_TIM_CLK HSI_CLK
void init_systick_timer(uint32_t tick_hz);

// 5. initialize scheduler stack
__attribute__ ((naked)) void init_scheduler_stack(uint32_t sceduler_top_of_stack);
void init_tasks_stack(void);
#define MAX_TASKS 		5
#define DUMMY_XPSR 		0x01000000U
// global array that holds psp value of different task
// 6. enable processor fault handler
void enable_processor_faults(void);

// 7. stack pointer setup

__attribute__((naked)) void switch_sp_to_psp(void);
uint32_t get_psp_value(void);
void save_psp_value(uint32_t current_psp_value);
void update_next_task(void);

typedef struct {
	uint32_t psp_value;
	uint32_t block_count;
	uint32_t current_state;
	void (*task_handler)(void);
} TCB_t; // task control block

TCB_t user_tasks[MAX_TASKS];

#define TASK_READY_STATE 		0x00
#define TASK_BLOCKED_STATE		0xFF

// current task now is set to 1 since 0 is the idle task
uint32_t current_task = 1; // task 1 run first

// global tick counter
uint32_t g_tick_count = 0;

const uint32_t const_v_1 = 10;
const uint32_t const_v_2 = 20;	
const uint8_t const_v_3 = 30;

void update_global_tick_count(void);
void unblock_tasks(void);
void schedule(void);
void task_delay(uint32_t);

#define INTERRUPT_DISABLE()  do{__asm volatile ("MOV R0,#0x1"); asm volatile("MSR PRIMASK,R0"); } while(0)
#define INTERRUPT_ENABLE()  do{__asm volatile ("MOV R0,#0x0"); asm volatile("MSR PRIMASK,R0"); } while(0)

uint32_t ctr1 = 0;
uint32_t ctr2 = 0;
uint32_t ctr3 = 0;
uint32_t ctr4 = 0;

int main(void)
{
	enable_processor_faults();

	init_scheduler_stack(SCHED_STACK_START);

	init_tasks_stack();

	led_init_all();

	init_systick_timer(TICK_HZ);

	// above code is using MSP, then when executing task1 we need to switch to PSP
	switch_sp_to_psp();

	task1_handler();
    /* Loop forever */
	for(;;);
}

void idle_handler(void) {
	while(1);
}
// 1. defining task handler
void task1_handler(void) {
	while(1) {
		ctr1++;
		led_on(LED_GREEN);
		task_delay(1000);
		led_off(LED_GREEN);
		task_delay(1000);
	}
}

void task2_handler(void) {
	while(1) {
		ctr2++;
		led_on(LED_ORANGE);
		task_delay(500);
		led_off(LED_ORANGE);
		task_delay(500);
	}
}

void task3_handler(void) {
	while(1) {
		ctr3++;
		led_on(LED_RED);
		task_delay(250);
		led_off(LED_RED);
		task_delay(250);
	}
}

void task4_handler(void) {
	while(1) {
		ctr1++;
		task_delay(125);
	}
}

void schedule(void)
{
	uint32_t *pICSR = (uint32_t*)0xE000ED04;
	*pICSR |= (1 << 28);
}

void task_delay(uint32_t tick_cnt)
{
	// disabling the interrupt here to prevent race condition when accessing g_tick_count and user_tasks
	INTERRUPT_DISABLE();
	if (current_task)
	{
		user_tasks[current_task].block_count = g_tick_count + tick_cnt;
		user_tasks[current_task].current_state = TASK_BLOCKED_STATE;
		// this triggers the PendSV here so that PendSV is switching to the next tasks.
		schedule();
	}
	INTERRUPT_ENABLE();
}

// 3. defining systick
void init_systick_timer(uint32_t tick_hz) {
	// count_value that we need to configure on the systick timer
	// this is minus 1 because the exception will be triggered when the value is reloaded
	// back to the CVR. 4 -> 3 -> 2 -> 1 -> 0 -> 4 (exception is triggered)
	uint32_t count_value = (SYSTICK_TIM_CLK/tick_hz) - 1;

	// systick reload value register (SRVR);
	// this is used to set reload value of SYST_CVR. Once CVR reaches 0, it will
	// reload its value by using value from SRVR
	uint32_t *pSYST_RVR = (uint32_t *)0xE000E014;

	// clear the value of SRVR
	*pSYST_RVR &= ~(0x00FFFFFF);

	// load the value to SRVR
	// the register is only 24 bit. Hence there should be any adjustment if we want to
	// use reload value that falls outside the 24bit
	*pSYST_RVR |= count_value;

	// do some settings for SYST_CSR
	uint32_t *pSYST_CSR = (uint32_t *)0xE000E010;

	*pSYST_CSR |= (1 << 1); // counting down to 0 assert systick exception
	*pSYST_CSR |= (1 << 2); // use processor clock as the source

	// enable the systick
	*pSYST_CSR |= (1 << 0); // enable the systick
}

// 4. initializing scheduler stack
__attribute__ ((naked)) void init_scheduler_stack(uint32_t sceduler_top_of_stack)
{
	// setting the MSP to the top of the stack
//	__asm volatile("MSR MSP,R0");
	// alternatively, we can use input output
	__asm volatile("MSR MSP,%0": : "r"(sceduler_top_of_stack) : );
	// we need to go back to main
	// BX is branch redirect
	__asm volatile("BX LR");
}

// 5. init task stack
void init_tasks_stack(void)
{

	user_tasks[0].psp_value = IDLE_STACK_START;
	user_tasks[1].psp_value = T1_STACK_START;
	user_tasks[2].psp_value = T2_STACK_START;
	user_tasks[3].psp_value = T3_STACK_START;
	user_tasks[4].psp_value = T4_STACK_START;

	user_tasks[0].current_state = TASK_READY_STATE;
	user_tasks[1].current_state = TASK_READY_STATE;
	user_tasks[2].current_state = TASK_READY_STATE;
	user_tasks[3].current_state = TASK_READY_STATE;
	user_tasks[4].current_state = TASK_READY_STATE;

	user_tasks[0].task_handler = idle_handler;
	user_tasks[1].task_handler = task1_handler;
	user_tasks[2].task_handler = task2_handler;
	user_tasks[3].task_handler = task3_handler;
	user_tasks[4].task_handler = task4_handler;

	uint32_t *pPSP;
	for(int i = 0 ; i < MAX_TASKS ;i++)
	{
		pPSP = (uint32_t *)user_tasks[i].psp_value;

		pPSP--; //xPSR
		*pPSP = DUMMY_XPSR;

		pPSP--; //PC
		*pPSP = (uint32_t) user_tasks[i].task_handler;

		pPSP--; //LR
		*pPSP = 0xFFFFFFFD;

		for (int j = 0; j < 13; j++)
		{
			pPSP--; // standard register
			*pPSP = 0;
		}
		// storing the PSP to the global variable so that we know where to back
		user_tasks[i].psp_value = (uint32_t) pPSP;
	}
}

// 6. stack pointer setup
void enable_processor_faults(void)
{
	// 1. enable all configurable exception like usage fault, mem manage fault, and bus fault
	uint32_t *pSHCRS = (uint32_t *)0xE000ED24;
	// we are enabling bit 16, 17 and 18
	*pSHCRS |= (0x7 << 16);
	// 2. enable fault at div by 0
	uint32_t *pCCR = (uint32_t *)0xE000ED14;
	*pCCR |= (1 << 4);

}

void HardFault_Handler(void)
{
	// printf("exception: hard fault \n");
}

void MemManage_Handler(void)
{
	// printf("exception: mem manage fault \n");
}

void BusFault_Handler(void)
{
	// printf("exception: bus fault \n");
}

void UsageFault_Handler(void)
{
	// printf("exception: usage fault handler \n");
}

// 7. stack pointer initialization
uint32_t get_psp_value(void)
{
	return user_tasks[current_task].psp_value;
}

// 8. Implement context switch
void save_psp_value(uint32_t current_psp_value) {
	user_tasks[current_task].psp_value = current_psp_value;
}

void update_next_task(void)
{
	uint32_t state = TASK_BLOCKED_STATE;

	for (int i = 0; i < MAX_TASKS; i++) {
		current_task++;
		current_task %= MAX_TASKS;

		// this check whether the next task is blocked or not.
		// if blocked, it will skip to the next task
		state = user_tasks[current_task].current_state;
		if ((state == TASK_READY_STATE) && (current_task != 0)) {
			break;
		}
	}
	// if all of them are blocked, then just execute idle task
	if(state != TASK_READY_STATE) {
		current_task = 0;
	}
}

__attribute__((naked)) void switch_sp_to_psp(void)
{
	// 1. initialize PSP with task1 stack start address
	// preserve LR which connects back to main
	__asm volatile("PUSH {LR}");

	// get the value of PSP of current task
	// BL is branch with link because we need  to come back to this function
	__asm volatile("BL get_psp_value");
	__asm volatile("MSR PSP,  R0"); // this initialize PSP

	// at this point LR is broken after we moved to get_psp_value,
	// thus this function cant go back to main.
	// that is why we need to pop LR back
	__asm volatile("POP {LR}");

	// 2. change SP to PSP using CONTROL register
	__asm volatile("MOV R0, 0x02"); //second bit is set to use PRP
	__asm volatile("MSR CONTROL, R0"); //copy this to CONTROL register
	// go back to main
	// this LR is copied back to PR that enables us to go back to main
	__asm volatile("BX LR");

}

__attribute__ ((naked)) void PendSV_Handler(void)
{
	// Save the context of current running task
	// 1. Get the current running tasks PSP
	__asm volatile("MRS R0, PSP");
	// 2. Using that PSP value, store SF2 (R4 - R11)
	// STMDB store multilple registers, decrement before each access
	// this is to simulate push operation
	// ! is optional writeback suffix. If ! is present the final address,
	// that is loaded from or stored to, is written back to Rn
	__asm volatile("STMDB R0!, {R4-R11}");

	__asm volatile("PUSH {LR}");
	// 3. Save the current PSP value
	__asm volatile("BL save_psp_value");

	// Retrieve the context of the next task

	// 1. decide next task to run
	__asm volatile("BL update_next_task");
	// 2. get its past PSP value
	// when this function return R0 is set to PSP
	__asm volatile("BL get_psp_value");
	// 3. using that PSP value, retrieve SF2 (R4 - R11)
	// LDMIA load multiple register, increment after
	__asm volatile("LDMIA R0!, {R4-R11}");
	// 4. Update the PSP and exit
	// use MSR since we are storing to special register
	__asm volatile("MSR PSP, R0");
	// at this point PSP is already return address of the next task

	__asm volatile("POP {LR}");
	__asm volatile("BX LR");
}

void update_global_tick_count(void)
{
	g_tick_count++;
}

// this will unblock the task if the task counter target is already reached
void unblock_tasks(void)
{
	for (int i = 1; i< MAX_TASKS; i++)
	{
		if (user_tasks[i].current_state != TASK_READY_STATE)
		{
			if (user_tasks[i].block_count == g_tick_count)
			{
				user_tasks[i].current_state = TASK_READY_STATE;
			}
		}
	}
}

// Systick now gets triggerred every 1ms. Instead of handle the context switch,
// it only increase the count and trigger PendSV
void SysTick_Handler(void)
{
	update_global_tick_count();
	unblock_tasks();
	schedule();
}
