# Implementation of task scheduler

## Introduction 

* Implement a scheduler which schedules multiple user tasks in a round-robin fashion by carrying out the context switching operation.
* Round robin scheduling method is, time slices are assigned to each task in equal portions and in circular order, handling all tasks without priority.
* First will use systick handler to carry out the context switch operation between multiple tasks
* Later will we change the code using pendSV handler

### Task

* a piece of code, or a specific job when it is allowed tu run on the CPU
* a task has its own stack to create its local variable when it runs on the CPU. Also when scheduler decides to remove a task from CPU, scheduler first saves the context(state)  of the task in task's private stack
* In summary, a piece code or a function is called a task when it is schedulable and never loses its state unless it is deleted permanently.

## Stack Pointer Selection

* We have 4 user tasks, they run in thread mode in their own private stack and use PSP (Process Stack Pointer) as stack pointer.
* There is another scheduler task which runs in handler mode and uses MSP (Main Stack Pointer) as stack pointer. It gets triggered by Systick handler / PendSV handler. 

### Stack assessment

```
------ RAM_END

PRIVATE STACK T1 (1KB)

------

PRIVATE STACK T2 (1KB)

------

PRIVATE STACK T3 (1KB)

------

PRIVATE STACK T4 (1KB)

------

PRIVATE SCHEDULER STACK (1KB)

------


UNUSED

------ RAM START
```

## Task and Scheduling

### Scheduling Policy Selection

* Using round robin pre-emptive scheduling
* No task priority
* We will use systick timer to generate exception for every 1ms to run the scheduler code
* Every 1ms we will run different task

### What is scheduling?

Scheduling is an algorithm which takes the decision of pre-empting a running task from CPU and takes the decision about which task should run on the CPU next.

The decision could be based on many factors such as system load, priority, shared resources, etc.

### What is context switching?

Context switching is the procedure of switching out of the currently running task from the CPU after saving the tasks execution context or state and switching in the next task to run on the CPU by retrieving the past execution context or state of the task.

### What is state of a task?

State of a task is the set of values of the task's registers (general purpose register, status register, special register), program counter, stack pointer, etc. which are required to resume the task from the point where it was switched out of the CPU.

State is collection of: General purpose registers + some special registers + status registers.
* RO-R12 general purpose registers
* Program counter (PC) -> When scheduler was preempting or switching out a task, the PC will be holding the next instruction of that task handler so that when scheduler switches in the task again, the task can resume from the next instruction.
* LInk register (LR )
* PSP -> The current value or the current state of task private stack, it is stored in PSP. We should also preserved the PSP value of the task when we are switching out the task from the CPU.
* PSR -> Program status register, it holds the current state of the task, whether the task was running in thread mode or handler mode, whether the task was running in privileged mode or unprivileged mode, whether the task was running in thumb mode or ARM mode, etc.

When scheduler decides to remove a task from CPU, scheduler first saves the context(state)  of the task in task's private stack.

## Case study of context switching

### Case of t1 swiching out, t2 switching in

```
Running TI
    |
Save the context of T1 to T1's              |
private stack (PUSH)                        |
    |                                       | Context saving
Save the PSP value of T1                    |
    |
Get current PSP value of T2                 |
    |                                       | Context retrieval
Retrieve the context of T2                  |
from T2's private stack (POP)               |
    |
Run T2
```

#### Stacking and unstacking during exception

1. Task 1 running in thread mode using PSP
1. Systick exception occurs. Automatically thread mode pre-empted and handler mode runs using MSP.
    * When the exception occurs, the processor automatically stacks the current state of the task in the private stack of the task. It saves the context of task 1 in task 1's private stack.
    * To make task 2 run, we need to change the PSP value to point to the private stack of task 2 latest PSP. So when unstacking happened, the processor will unstack the context of task 2 from task 2's private stack and continue from the point where it was switched out.

Stack frame which get saved automatically by the processor when exception occurs:

| Stack Frame |
| ----- |
| xPSR    |
| Return address (PC) |
| LR      |
| R12     |
| R3      |
| R2      |
| R1      |
| R0      |

What needs to be saved by the scheduler manually is remaining registers from R4 to R11 and PSP.

## Configuring Systick Timer

Configure systick timer for every 1ms

Arm cortex m7
Processor clock = 16MHz ==> need to find this value
Systick timer clock = 16Mhz ==> need to fins this value
1ms is 1KHz in frequency domain

To bring down the frequency of the systick timer to 1KHz, we need to divide the processor clock by 1KHz. This is called (reload value)

For instance:
* Processor clock = 16MHz
* Systick timer clock = 16MHz
* Target frequency = 1KHz (desired exception frequency)
* Reload value = 16MHz / 1KHz = 16000

```
System Clock (16Mhz)  --> Processor Clock (16MHz) 
                      |
                      --> Systick Timer Clock (16MHz)
```

## Case study of context switching (contd)  

### Saving the context of the task

1. When the exception occurs, the processor automatically stacks the current state of the task in the private stack of the task. Then we need to save the remaining registers and PSP value of the task manually. It saves the context of task 1 in task 1's private stack.
1. Save PSP of task 1 so that we can retrieve it later when we want to run task 1 again. We can store it in a global variable. This is called context saving.
1. To make task 2 run, we need to change the PSP value to point to the private stack of task 2 latest PSP. So when unstacking happened, the processor will unstack the context of task 2 from task 2's private stack and continue from the point where it was switched out.

### Retrieving the context of the task

1. Program the PSP with the PSP value of the task 2
1. POP SF2. 
1. Exit the exception handler. The processor will automatically unstack the context of task 2 from task 2's private stack and continue from the point where it was switched out.

### Task stack area init and storing of dummy SF

What if task 2 is getting executed for the first time?

If task 2 is getting executed for the first time, it doesnt have any context. So programmer should store dummy SF1 and SF2 in task's stack area as a part of "task initialization" before launching the scheduler.

1. For SF1 we can keep: 
    * R0 to R3, R12 to 0.
    * xPSR  We need to keep T bit of EPSR 1 so that the task runs in thumb mode. The value of xPSR should be 0x01000000.
    * PC to the address of the task handler function. Make sure the LSB of the PC is 1 so that the task runs in thumb mode.
    * LR to 0xFFFFFFFD. This is the value of LR when the task is running for the first time. It tells the processor that when the task is done, it should return to the thread mode using the PSP value of the task.
1. For SF2 we can keep all of the registers to 0.
1. We also need to store the PSP value of the task in a global variable so that we can retrieve it later when we want to run the task again.

```c
#include <stdint.h>
#include <stdio.h>

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
#define SCHED_STACK_START	( (SRAM_END) - (4 * SIZE_TASK_STACK) )

// 3. defining init systick
#define TICK_HZ 	1000U //1kHz
#define HSI_CLK		64000000U
#define SYSTICK_TIM_CLK HSI_CLK
void init_systick_timer(uint32_t tick_hz);

// 5. initialize scheduler stack
__attribute__ ((naked)) void init_scheduler_stack(uint32_t sceduler_top_of_stack);
void init_tasks_stack(void);
#define MAX_TASKS 		4
#define DUMMY_XPSR 		0x01000000U
// global array that holds psp value of different task
uint32_t psp_of_tasks[MAX_TASKS] = {T1_STACK_START, T2_STACK_START, T3_STACK_START, T4_STACK_START};
uint32_t task_handlers[MAX_TASKS];

// 6. enable processor fault handler
void enable_processor_faults(void);

// 7. stack pointer setup
uint32_t current_task = 0; // task 1 run first
__attribute__((naked)) void switch_sp_to_psp(void);
uint32_t get_psp_value(void);
void save_psp_value(uint32_t current_psp_value);
void update_next_task(void);


uint32_t ctr1 = 0;
uint32_t ctr2 = 0;
uint32_t ctr3 = 0;
uint32_t ctr4 = 0;

int main(void)
{
	enable_processor_faults();

	init_scheduler_stack(SCHED_STACK_START);

	task_handlers[0] = (uint32_t)task1_handler;
	task_handlers[1] = (uint32_t)task2_handler;
	task_handlers[2] = (uint32_t)task3_handler;
	task_handlers[3] = (uint32_t)task4_handler;

	init_tasks_stack();

	init_systick_timer(TICK_HZ);

	// above code is using MSP, then when executing task1 we need to switch to PSP
	switch_sp_to_psp();

	task1_handler();
    /* Loop forever */
	for(;;);
}

// 1. defining task handler
void task1_handler(void) {
	while(1) {
		printf("task 1\n");
		ctr1++;
	}
}

void task2_handler(void) {
	while(1) {
		printf("task 1\n");
		ctr2++;
	}
}

void task3_handler(void) {
	while(1) {
		printf("task 1\n");
		ctr3++;
	}
}

void task4_handler(void) {
	while(1) {
		printf("task 1\n");
		ctr4++;
	}
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
	uint32_t *pPSP;
	for(int i = 0 ; i < MAX_TASKS ;i++)
	{
		pPSP = (uint32_t *)psp_of_tasks[i];

		pPSP--; //xPSR
		*pPSP = DUMMY_XPSR;

		pPSP--; //PC
		*pPSP = task_handlers[i];

		pPSP--; //LR
		*pPSP = 0xFFFFFFFD;

		for (int j = 0; j < 13; j++)
		{
			pPSP--; // standard register
			*pPSP = 0;
		}
		// storing the PSP to the global variable so that we know where to back
		psp_of_tasks[i] = (uint32_t) pPSP;
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
	printf("exception: hard fault \n");
}

void MemManage_Handler(void)
{
	printf("exception: mem manage fault \n");
}

void BusFault_Handler(void)
{
	printf("exception: bus fault \n");
}

void UsageFault_Handler(void)
{
	printf("exception: usage fault handler \n");
}

// 7. stack pointer initialization
uint32_t get_psp_value(void)
{
	return psp_of_tasks[current_task];
}

// 8. Implement context switch
void save_psp_value(uint32_t current_psp_value) {
	psp_of_tasks[current_task] = current_psp_value;
}

void update_next_task(void)
{
	current_task++;
	current_task %= MAX_TASKS;
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

__attribute__ ((naked)) void SysTick_Handler(void) {
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
```