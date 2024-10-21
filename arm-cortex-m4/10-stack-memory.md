# Stack Memory

## Introduction

* Stack memory is parth of the main memoory reserved for temporary storage of data.
* Mainly used during function, interrupt/exception handling
* Stack memory is accessed in last in first out (LIFO) order.
* The stack can be accessed using PUSH and POP instructions or using any memory manipulation instructions (LD, ST, etc).
* The stack is traced using a stack pointer (SP, R13) register. PUSH and POP instructions affect the stack pointer.

Why uses:
* temporary storage of processor register values
* temporary storage of local variables of the function
* during system exception or interrupt, stack memory will be used to save the context (some general-purpose registers, processor status register, return address) of the currently executing code

```
E.g. SRAM (128KB)

|-------| RAM_START
|       |
|       | Part of RAM serverd for global data
|       |
|-------| 
|       |
|       | Reserved for heap memory
|       |
|-------|
|       |
|       | Stack memory
|       |
|-------| RAM_END
```

* Reserved for global data: Will be utilized when the program contains global data and static local variables. Even you can store instructions here and execute it.
* Reserved for heap memory: Will be utilized when the program contains dynamic memory allocation.
* Stack memory: Will be utilized when the program contains function calls, interrupts, and exceptions.

How big each segment is decided by the programmer.

## Stack Operation Model

ARM Cortex Mx processor stach consumption model: Full Descending (FD) stack model.

Few consumption model:
* Full Ascending (FA): Stack grows from lower memory address to higher memory address. SP will be incremented before storing the data.
* Full Descending (FD): Stack grows from higher memory address to lower memory address. SP will be decremented before storing the data.
* Empty Ascending (EA): Stack grows from lower memory address to higher memory address. SP will be incremented after storing the data.
* Empty Descending (ED): Stack grows from higher memory address to lower memory address. SP will be decremented after storing the data.

## Stack Placement

Stack placement is decided by the linker script. It define the start and end of the stack memory. If the SP goes beyond the end of the stack memory, it will cause a stack overflow. We need to detect the stack overflow and handle it properly.

* Type 1: stack memory is placed after the heap memory.
* Type 2: stack memory is placed start at the end of the memory. (generally we use this)

Sample of type 2:

```
|-------| RAM_END
|       |
|       | Stack. It grows higher to lower
|       |
|-------| 
|       |
|       | Unused space
|       |
|-------|
|       |
|       | Heap space (grows lower to higher)
|       |
|-------| 
|       |
|       | Data
|       |
|-------| RAM_START
```

## Banked stack pointer registers of ARM Cortex Mx

1. Cortex M Processor physically has 3 stack pointers SP(R13), MSP, PSP.
    * MSP: Main stack pointer. Used to store the context of the main program.
    * PSP: Process stack pointer. Used to store the context of the process.

    Refer to core register section on the documentation for more details.

1. After processor reset, by default, MSP will be used as the stack pointer. That means, SP copy the value of MSP.

1. Thread mode can change the current stack pointer to PSP by configuring the CONTROL register's SPSEL bit. 
    * SPSEL = 0: MSP is the current stack pointer.
    * SPSEL = 1: PSP is the current stack pointer. 

1. Handler mode code execution will always use MSP as the stack pointer. That also means that, changing the value of SPSEL in handler mode will not change the current stack pointer. This will be ignored.

1. MSP will be initialized automatically by the processor after reset by reading the content of the address 0x00000000. 

1. If you want to use the PSP then make sure that you initialize the PSP to valid memory location before switching to the PSP.

## Stack Exercise

How the stack is set to the highest memory? 

```s
g_pfnVectors:
    .word _estack
```
`_estack` is the initial value of the stack pointer. It is the highest memory address of the stack memory. it is a linker symbol.

The size of heap, stack, and data memory is defined in the linker script. 


### Use case: Change SP to PSP for thread mode

* In thread mode, we will change the SP to PSP.
* In handler mode, SP is always MSP.

```
Stack space total 1KB

|-------| STACK_MSP_START
|       |
|       | MSP stack space (512B)
|       |
|-------| STACK_MSP_END = STACK_MSP_START 
|       |
|       | PSP stack space (512B)
|       |
|-------| STACK_PSP_END
```

On the example above, when application run in thread mode, we will use PSP as the stack pointer. When the application run in handler mode, we will use MSP as the stack pointer. This typically is done when we want to run the application in user space (thread mode) and kernel space (handler mode).

```c
#include <stdint.h>

int add(int a, int b, int c, int d) {
	return a+b+c+d;
}

int main(void)
{
	int ret;
	// at this point SP and MSP are the same.
	// PSP is not initialize
	ret=add(1,2,3,4);
	for(;;);
}
```

```c
#include <stdint.h>
#include <stdio.h>

int add(int a, int b, int c, int d) {
	return a+b+c+d;
}

__attribute__((naked)) void change_sp_to_psp(void) {
	__asm volatile(".equ SRAM_END, (0x20000000 + (128 * 1024))");
	__asm volatile(".equ PSP_START, (SRAM_END - 512)");

	// initializing R0 with PSP start
	__asm volatile("LDR R0, =PSP_START");
	// setting PSP to value from R0
	__asm volatile("MSR PSP, R0");

	// set the control register to use PSP
	__asm volatile("MOV R0, #0x02");
	__asm volatile("MSR CONTROL, R0");

	// at this point, sp SP using PSP

	// return instruction
	// branch indirect to LR
	__asm volatile("BX LR");
}

void generate_exception(void) {
	// SVC is used to trigger the handler mode.
	// it usually used to trigger call to the kernel layer
	__asm volatile("SVC #0x0");
}

int main(void)
{

	change_sp_to_psp();

	int ret = 0;
	ret=add(1,2,3,4);

	printf("result");
	generate_exception();

	for(;;);
}

void SVC_Handler(void) {
    // ideally when this is executed, the MSP is used as the stack pointer automatically
	printf("triggering SVC_Handler\n");
}

void HardFault_Handler(void) {
	printf("triggering HardFault_Handler\n");
}
```

## Function call and AAPCS (ARM Architecture Procedure Call Standard)

```c
void fun_y(int a, int b, int c, int d) {
    int x = a+b+c+d;
}

void fun_x(void) {
    int ret;
    ret = fun_y(1,2,3,4);
}
```

* According to AAPCS, a C function can modify the registers R0, R1, R2, R3, R14(LR), and PSR and it's not the responsibility of the function to save these registers before any modification. The caller function should save these registers before calling the callee function.
    * The caller should PUSH the registers to the stack before calling the callee function. and POP the registers after the callee function returns.

* If a function wants to make use of R4 to R11 registers, then it's the responsibility of the function to save its previous contents before using them and restore them before returning.
    * The callee function should PUSH the registers to the stack before using them and POP the registers after using them.

* R0,R1,R2,R3,R14(LR) registers are called "caller saved registers", it's the responsibility of the caller to save these registers before calling the callee function if those values will still be needed after the function call and retrieve it back once the called function returns. Register values that are not required after the function call don't have to be saved.

* R4-R11 are called "callee saved registers", it's the responsibility of the callee function to save these registers before using them and restore them before returning.

* Caller function users R0, R1, R2, R3 registers to send input arguments to the callee function.
    * If the input arguments are more than 4, then the caller function should PUSH the remaining arguments to the stack before calling the callee function.

* The callee function users register R0 and R1 to send the result back to the caller function.

## Stack activities during interrupt and exception

* To allow a C function to be used as an exception/interrupt handler, the exception mechanism needs to save R0 to R3, R12, LR and XPSR at exception entrace automatically and restore them at exception exit under the control of the processor hardware.
    * The processor hardware will save the R0 to R3, R12, LR and XPSR registers to the stack automatically when an exception occurs. => This is called stack frame.

* In this way, when returned to the interrupted program, all the registers would have the same value as when the interrupt entry sequence started.

### Stack Initialization

Tips:
* Evaluate the targeted application. Decide the amount of stack that would be needed for the worst-case scenario of your application run time
* Know the stack consumption model (FA, FD, EA, ED) of the processor you are using.
* Decide the stack placement in the memory. (start and end of the stack memory)
* In many applications, there may be second stage stack init. For example, if you want to allocate stack in external SDRAM, then first start with internal RAM, in the main or startup code initialize the SDRAM and then change the stack pointer to point to SDRAM.
* If you are using ARM Cortex Mx, make sure that the first location of vector table contains the initial stack address (MSP) 
* In RTOS scenario, the kernel code may use MSP to trace its own stack and configure PSP for user task's stack.