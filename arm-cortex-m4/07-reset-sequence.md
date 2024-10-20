# Reset Sequence of the ARM Cortex Processor

1. When you reset the processor, the PC is loaded with the value 0x00000000. 

1. The processor reads the value at 0x00000000 and loads it into the MSP (Main Stack Pointer) register. The 0x00000000 is belong to flash memory or ROM.

    MSP = value at 0x00000000
    MSP is a main stack pointer register. That means, processor first initializes the stack pointer before executing any code.

1. After that, processor reads the value at 0x00000004 and loads it into the PC (Program Counter) register. The 0x00000004 is the reset handler vector address.

    PC = value at 0x00000004
    PC is a program counter register. That means, processor knows where to start executing the code.

1. PC jumps to the reset handler address. The reset handler is the first function that gets executed after the reset.

1. A reset handler is just a C or assembly function that is defined in the startup code. The reset handler do any initialization that is required before the main function is called.

1. The reset handler calls the `main` function. The `main` function is the entry point of the C program.