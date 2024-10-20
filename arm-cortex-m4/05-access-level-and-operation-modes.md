# Access level and operation modes of the processor

## Operatonal mode 

1. Thread mode
    * All application code will execute under thread more of the processor. also called User mode.
    * Processor always start with the thread mode
    * can be executed with previlledge and unprevilledge access level

1. Handler mode
    * All the exceptions handlers or interrupt handler will run under the handler mode of the processor.
    * Whenever the code meeets the system exception or any external interrupt, the code will change its mode to handler mode to service the ISR asscoiated with that system
    * can only be executed with previledge access level

How do we know it is executed under thread or handler mode?
    * There is register that we can use to check the status 
        * Interrupt Program Status Register (IPSR). 
        * There is field called `ISR_NUMBER`. If it is 0, then it is thread mode.
        * Otherwise handler mode
        * From cube ide, it is `xpsr` register. Check the first 9 bit.


## Access level

Processor offers two access level:

* Previledged access level (PAL)
    * If your code is running under PAL, you can access all the resources and restricted registers of the processor.
    * By default your code will run under PAL

* Non-previledged access level (NPAL)
    * If your code is running under NPAL, your code may not have access to some of the restricted registers of the processors

When the processor is in thread mode, its possible to move processor in to NPAL. Once the processor is in NPAL, it is not possible to move back to PAL, unless you change the mode processor operational mode to handler mode.

Handler mode always runs under PAL.

Use the CONTROL register to switch between the access level of the processor.

## Core Registers

Refer to Generic User Guide "Core Registers" section for more details.

1. R0 to R12 registers are for general purpose.

1. All the core registers are 32 bit wide.

1. R13 is also called as Stack Pointer (SP). It is used to point to the stack memory.
    * PSP (Process Stack Pointer) - used in thread mode
    * MSP (Main Stack Pointer) - used in handler mode

1. R14 is also called as Link Register (LR). It is used to store the return address of the subroutine.
    
    > The Link Register (LR) is register R14. It stores the return information for subroutines, function calls, and exceptions. 

    Imagine you have a function `A` and it calls another function `B`. When function `B` is called, the return address of function `A` is stored in the LR register. When function `B` is done, it will return to the address stored in the LR register by setting the PC to the value stored in the LR register.

1. R15 is also called as Program Counter (PC). it holds the address of the next instruction to be executed.

    > On reset, the processor loads the PC with the value of the reset vector, which is at the initial value of the Vector Table Offset Register (VTOR) plus 0x00000004. Bit[0] of the value is loaded into the EPSR T-bit at reset and must be 1. 

1. Program Status Register (PSR) is a 32 bit register that contains the current status of the processor.
    * It contains the Application Program Status Register (APSR), Interrupt Program Status Register (IPSR), and Execution Program Status Register (EPSR).
    * APSR - Application Program Status Register
        * It contains the current state of of the condition flags from previous instruction execution.
        * It uses to check negative flag, zero flag, carry flag, overflow flag, etc.
    * IPSR - Interrupt Program Status Register
        * It contains the exception number of the current interrupt.
        * It is used to check the current interrupt number.
    * EPSR - Execution Program Status Register
        * It contains the T-bit, which indicates the current mode of the processor.
            * If T-bit is 0, processor thinks that the next instruction is ARM instruction. Otherwise, it is thumb instruction.
        * It is used to check the current mode of the processor.

These registers are non-memory mapped registers. They are not accessible by the memory. They are only accessible by using assembly language instructions.

Memory mapped registers are the registers that are accessible by the memory. They are used to configure the peripherals of the microcontroller. NVIC, MPU, SCB, etc are the processor specific peripherals. While GPIO, SPI, I2C, etc are the external or microcontroller peripherals.