# ARM GCC inline assembly coding



Use inline assembly coding in C/C++ to access processor register.

## Basic inline assembly code

Assembly instruction: 
```asm
MOV  RO, R1
```

Inline assembly code in C/:
```c
__asm volatile("MOV R0, R1");
```

Why:
* Move the content of C variable data to ARM register
* Move the content of the CONTROL register to C variable data

General form of inline assembly code:
```
__asm volatile(code : output operand list : input operand list : clobber list);
```

* volatile: Prevent the compiler from optimizing the code
* code: Assembly code
* output: Output operands
* input: Input operands
* clobber: Clobbered registers

Example:
```c
__asm volatile("MOV R0, R1" : : :);
```


Load 2 values from memory, add them and store the result back to the memory using inline assembly.

```c
#include <stdint.h>

int main(void)
{
    // initialize R1 and R2 with memory addresses
	__asm volatile("LDR R1, =0x20001000");
	__asm volatile("LDR R2, =0x20001004");
    
    // in debugger we are going to set the value in the memory address 0x20001000 to 6 and 0x20001004 to 7 (for instance)

    // load the value from memory address 0x20001000 to R0
	__asm volatile("LDR R0, [R1]");
    // load the value from memory address 0x20001004 to R1
	__asm volatile("LDR R1, [R2]");
    // add the values in R0 and R1 and store the result back to register R0
	__asm volatile("ADD R0, R0, R1");
    // store the result in R0 to memory address 0x20001000
	__asm volatile("STR R0, [R2]");
	while(1);
}
```

## Constraint string

Each operand in the input and output operand list is described by a constraint string followed by a C expression in parentheses.

```
"<Constraint string>" (<C expression>)
``` 

> Constraint string = constraint character + constraint modifier

## Input Operand 

Move the content of C variable data to ARM register R0

* Instruction => MOV
* Source => C variable `val` (INPUT)
* Destination => ARM register `R0`

    ```c
    __asm volatile("MOV R0, %0" : : "r" (val));
    ```

    * there is no output operand
    * input operand is `val`. It is described by the constraint string `"r"` followed by the C expression `(val)`
    * `%0` is the placeholder for the input operand

```c
int main(void)
{
	int val = 50;
	__asm volatile("MOV R0, %0":  :"r"(val));
}
```

`r` constraint string tells compiler to use Any general-purpose register. It will move the content of C variable `val` to any general purpose arm register from the stack first and then move it to R0 using `MOV`.


```c
int main(void)
{
	int val = 50;

    // this translate to: mov.w   r0, #80 @ 0x50
	__asm volatile("MOV R0, %0":  :"i"(0x50));
}
```

`i` constraint string tells compiler to use an immediate value. It will move the immediate value `0x50` to R0 using `MOV`. If we use `"r"(0x50)`, it will move the content of the memory address `0x50` to general purpose register first before moving it to R0.

## Output Operand

Move the content of `CONTROL` register to C variable `control_reg`

> CONTROL register is a special register in ARM Cortex-M processor that holds the control bits. To read the content of the CONTROL register, we need to use MRS instruction.

* Instruction => MRS
* Source => CONTROL register (OUTPUT)
* Destination => C variable `control_reg`

```c
__asm volatile("MRS %0, CONTROL" : "=r" (control_reg));
//	080002fa: ef f3 14 83   mrs     r3, CONTROL
//	080002fe: 7b 60         str     r3, [r7, #4]
```

Two instructions are generated:
* `MRS R3, CONTROL` - Move the content of CONTROL register to R3
* `STR R3, [R7, #4]` - Store the content of R3 to memory address `[R7, #4]`

* output operand is `control_reg`. It is described by the constraint string `"=r"` followed by the C expression `(control_reg)`
* `=` constraint modifier tells the compiler that the operand is an output operand. It is write only operand.
* `%0` is the placeholder for the output operand

## Input/Output Operand

### Copy the content of C variable `var1` to `var2`

* Instruction: MOV
* Source: C variable `var1` (INPUT)
* Destination: C variable `var2` (OUTPUT)

```c
int var1 = 10;
int var2;
__asm volatile("MOV %0, %1" : "=r" (var2) : "r" (var1));
// 080002fa: 7b 68         ldr     r3, [r7, #4]
// 080002fc: 1b 46         mov     r3, r3
// 080002fe: 3b 60         str     r3, [r7, #0]
```

### Copy the contents of a pointer to another variable

* Instruction: LDR => LDR is used to do memory read operation
* Source: Memory address pointed by `ptr1` (INPUT)
* Destination: C variable `var2` (OUTPUT)

```c
int p1, *p2;
p2 = (int *)0x20001000;
__asm volatile("LDR %0, [%1]" : "=r" (p1) : "r" (p2)); //p1 = *p2
// 080002fa: 7b 68         ldr     r3, [r7, #4]
// 080002fc: 1b 68         ldr     r3, [r3, #0]   => r3 is dereferenced here. it reads the value from the memory address pointed by p2
// 080002fe: 3b 60         str     r3, [r7, #0]

```

* `[%1]` is the memory address pointed by `p2`

