# Access Level and T bit

## Access Level

To change the access level to unpriviledge, we need to use inline assembly code to change the `CONTROL` register. Ref: CONTROL register in the ARM Cortex-M4 Technical Reference Manual.

```c
void change_access_level(void)
{
    // read
    __asm volatile ("MRS R0, CONTROL");
    // set
    // ORR is instruction to do OR
    __asm volatile ("ORR R0, R0, #0x01");
    // write
    __asm volatile ("MSR CONTROL, R0");
}
```

When we call this function, the access level will be changed to unpriviledge. If we are trying to access any restricted control register, the processor will generate a hard fault exception.

This is used to protect the system from any malicious code that tries to change the system configuration. Kernel code will run in priviledge mode and user code will run in unpriviledge mode. Kernel will always change the access level to unpriviledge mode before executing the user code.

To change the access level back to priviledge mode, We need to change the mode to handler mode so that we can modify `CONTROL` register and set the `nPRIV` bit to 0.

## T bit

1. Various ARM processors support ARM-Thumb interworking, that means the ability to switch between ARM and Thumb state.

1. The processor must be in ARM state to execute instructions which are from ARM ISA and the processor must be in Thumb state to execute instructions which are from Thumb ISA.

1. If `T` bit of EPSR is set (1), processor thinks that the next instruction which it is about to execute is from Thumb ISA. 

1. If `T` bit is clear (0), processor thinks that the next instruction which it is about to execute is from ARM ISA.

1. The cortex-m4 processor does not support ARM-Thumb interworking. That means the processor can only execute instructions from Thumb ISA. The value of `T` bit is always 1.

1. The lsb(bit 0) of the program counter (PC) is linked to this `T` bit. When you load a value or an address in to PC the Bit[0] of the value is loaded into the T-bit.

1. Hence, any address you place in the PC must have its 0th bit as 1. This is usually taken care by the compilers and programmers do not have to worry about it.

1. This is the reason why we see all vector address are incremented by 1 in the vector table.


```c

//void call_function(void){
// 80002f0:	b480      	push	{r7}
// 80002f2:	af00      	add	r7, sp, #0
//
//}
void call_function(void){
    
}

int main(void)
{
    void (*fun_ptr)(void);

    // check the address of this function pointer has 1 in bit 0: 0x80002f1
    // even while the address of the function is 80002f0. 
    // this is changed by compiler.
    // so that we jump tp PC the bit 0 is maintained as 1.
    fun_ptr = call_function;

    fun_ptr();

	for(;;);
}
```

If you try to load an address which has 0 in bit 0, the processor will generate a hard fault exception.
