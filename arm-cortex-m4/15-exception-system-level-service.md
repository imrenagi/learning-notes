# Exception for System Level Service

## SVC Exception

* ARM cortex Mx processor supports 2 important system-level service exceptions:
  * SVC (Supervisor Call) exception
  * PendSV (Pending Service) exception

* Supervisory calls are typically used to request privileged operations or access to system resources from an operating system

* SVC exception is maily used in an OS environment. For example, A less privileged user task can trigger SVC exception to get system-level services (like accessing device drivers, peripherals) from the kernel of the OS.

* PendSV is mainly used in an OS environment to carry out context switching between 2 or more tasks when no ther exceptions are active in the system.

### SVC instruction

* `SVC` is a thumb ISA instruction which causes SVC exception
* In an RTOS scenario, user tasks can execute SVC instruction with associated argument to make supervisory calls to see privileged resources from the kernel of the OS.
* Unprivileged user tasks use the SVC instruction to change the processor mode to privileged mode to access privileged resources like peripherals.
* SVC instruction is always used along with a number, which can be used to identify the request type by the kernel code
* The svc handler executes right after the SVC instruction (no delay, unless a higher priority exception arrives at the same time)

User level code:
```
User task
SVC #1
Access level: un-privileged
Processor mode: Thread mode
```

Kernel level code:
```
__SVC_request_handler(int svc_number)
(
    case 1:
        __write();
    case 2:
        __read();
)
Access level: privileged
Processor mode: Handler mode
```

* User task executes SVC instruction with a number 1
* The kernel code has a handler for SVC number 1, which is `__write()`
* The kernel code executes `__write()` to perform the write operation to the restricted resource e.g. peripheral

User tasks can not directly access the peripherals, they need to make a request to the kernel to access the peripherals. The kernel code has the handler for the request and it can perform the operation on behalf of the user task.

If the user task tries to access the peripheral directly, the processor will raise a memory manage fault exception.

#### Methods to trigger SVC exception

There are two methods to trigger SVC exception:
* Direct execution of SVC instruction with an immediate value. Example: `SVC #0x04` in assembly 
(using SVC instruction is  very efficient in terms of latency)
* Setting the exception pending bit in "System Handler Control and State Register" (SHCSR) register. This is uncommon.

### Extracing SVC number from the exception

* The SVC instruction has a number embedded within it, often referred to as the SVC number.
* In the SVC handler, you should fetch the opcode of the SVC instruction to extract the SVC number.
* To fetch the opcode of SVC instruction from program memory, we should have the value of PC(return address) where the user code had interrupted while triggering the SVC exception
* The value of PC(return address) where the user code had interrupted is stored in the stack as a part of exception entry sequence by the processor.

Illustration:

* When SVC is triggered, the stack frame looks like this:
    | Stack |
    | --- |
    | Used stack space |
    | Last stack item |
    | xPSR |
    | PC(return address) |
    | LR |
    | R12 |
    | R3 |
    | R2 |
    | R1 |
    | R0 ==> MSP | 

* Get the next insert address after svc, which is the PC.
    ```
    next_insert_address = MSP[6]
    ```

* Get the SVC number. It is the value at the next insert address - 2.
    ```
    SVC_number = *(next_insert_address - 2)
    ```

### Exercise 1

Write a program to execute an SVC instruction from thread mode, implement the svc handler to print the SVC number used. Also, increment the SVC number by 4 and return it to the thread mode code and print if.

Hints:
1. Write a main() function where you should execute the SVC instruction with an argument. Let's say `SVC #0x05`
1. Implement the SVC handler
1. In the SVC handler, extract the SVC number and print it using printf
1. Increment the SVC number by 4 and return it to the thread mode code

```c
#include <stdint.h>
#include <stdio.h>

int main(void)
{
    __asm volatile ("SVC #5");

    // tell the compiler to use register to store the value
    // and associate this with r0
    register uint32_t returned_value __asm ("r0");

    // as alternative, we can use inline assembly code
    // uint32_t returned_value;
    // __asm volatile("MOV %0,R0" : "=r"(returned_value) ::);

	for(;;);
}

__attribute__ ((naked)) void SVC_Handler(void)
{
	__asm ("MRS r0, MSP");
	__asm ("B SVC_Handler_c");
}

void SVC_Handler_c(uint32_t *pBaseStackFrame)
{
	printf("svc handler \n");

	// pBaseStackFrame[6] takes the PC
	// then it is type casted to uint8_t
	uint8_t *pReturn_address = (uint8_t *)pBaseStackFrame[6];

	// 2. decrement the return address  by 2 to point to
	// opcode of the svc instruction in the program memory
	pReturn_address--;
	pReturn_address--;

	// 3. extract the svc number (LSB of the opcode)
	uint8_t svc_number = *pReturn_address;

	// 4. increment it by 4 and return it to thread mode
	// here we set the R0 to the new value 	
	pBaseStackFrame[0] = svc_number + 4;
}
```

### Exercise 2

Write a program to add, substract, multiply, and divide 2 operands using SVC handler and return the result to the thread mode code
and print the result. Thread mode could pass 2 operands via the stack frame.

Use the SVC number to decide the operation:

| SVC number | Operation |
| --- | --- |
| 36 | Add |
| 37 | Substract |
| 38 | Multiply |
| 39 | Divide |

```c
#include<stdint.h>
#include<stdio.h>

void SVC_Handle_c(uint32_t *pBaseStackFrame);

int32_t add_numbers(int32_t x , int32_t y )
{
    int32_t res;
    __asm volatile("SVC #36");
    __asm volatile ("MOV %0,R0": "=r"(res) ::);
    return res;
}

int32_t sub_numbers(int32_t x , int32_t y )
{
    int32_t res;
    __asm volatile("SVC #37");
    __asm volatile ("MOV %0,R0": "=r"(res) ::);
    return res;
}

int32_t mul_numbers(int32_t x , int32_t y )
{

    int32_t res;
    __asm volatile("SVC #38");
    __asm volatile ("MOV %0,R0": "=r"(res) ::);
    return res;
}

int32_t div_numbers(int32_t x , int32_t y )
{
    int32_t res;
    __asm volatile("SVC #39");
    __asm volatile ("MOV %0,R0": "=r"(res) ::);
    return res;
}


int main(void)
{
    int32_t res;

    res = add_numbers(40, -90);
    printf("Add result = %ld\n",res);

    res = sub_numbers(25,150);
    printf("Sub result = %ld\n",res);

    res = mul_numbers(374,890);
    printf("mul result = %ld\n", res);

    res = div_numbers(67,-3);
    printf("div result = %ld\n",res);

    for(;;);
}


__attribute__( ( naked ) ) void SVC_Handler( void )
{
    __asm ("MRS r0,MSP");
    __asm( "B SVC_Handler_c");
}


void SVC_Handler_c(uint32_t *pBaseOfStackFrame)
{
    printf("in SVC handler\n");

    uint8_t *pReturn_addr = (uint8_t*)pBaseOfStackFrame[6];

    //2. decrement the return address by 2 to point to
    //opcode of the SVC instruction in the program memory
    pReturn_addr-=2;


    //3. extract the SVC number (LSByte of the opcode)
    uint8_t svc_number = *pReturn_addr;

    printf("Svc number is : %d\n",svc_number);


/*SVC number signifies the type of operation , decode it */
    int32_t res;

    int32_t x, y;
    x = pBaseOfStackFrame[0]; // get x from R0
    y = pBaseOfStackFrame[1]; // get y from R1


    switch(svc_number)
    {
    case 36:
        res = x + y;
        break;
    case 37:
        res = x - y;
        break;
    case 38:
        res = x * y;
        break;
    case 39:
        res = x / y;
        break;
    default:
        printf("invalid svc code\n");

    }
    /*store the result back to stack frame at R0's position */
    pBaseOfStackFrame[0] = res;
}
```

## PendSV Exception

* It is an exception type 14 and has a programmable priority level.
* This exception is triggered by setting its pending status by writing to "Interrupt Control and State Register" (ICSR) register of processor.
* Triggering a pendSV system exception is a way of invoking the preemptive kernel to carry out the context switch in an OS environment.
* In an OS environment, PendSV handler is set to the lowest priority level, and the PendSV handler carries out the context switch operation.

Typical use of PendSV exception:
* Typically this exception is triggered inside a higher priority exception handler, and it gets executed when the higher priority handler finishes. 
* Using this characteristic, we can schedule the PendSV exception handler to be executed after all the other interrupt processing tasks are done.
* This is very useful for a context switching operation, which is a crucial operation in various OS design.
* Using PendSV in context switching will be more efficient in an interrupt noisy environment. Because in an interrupt noisy environment, we need to delay the context switching operation until all the other interrupt processing tasks are done.

### Context Switching

If there are task a and b, scheduler decides to switch from task a to task b. The context switching operation involves saving the context of task a and restoring the context of task b.

Schedules is usually implemented in a Systick handler, which is a periodic timer interrupt. The Systick handler is the highest priority interrupt in the system. The Systick handler is responsible for decrementing the time quantum of the current task and invoking the scheduler to decide the next task to run.

When the scheduler decides to switch from task a to task b, the scheduler triggers the PendSV exception. The PendSV exception handler is responsible for saving the context of task a and restoring the context of task b.

1. Task A is running
1. Systick handler is triggered and decides to switch from task A to task B.
1. Systick handler triggers the PendSV exception.
1. If there is no other interrupt in system, PendSV handler runs. PendSV handler saves the context of task A and restores the context of task B.
1. Task B starts running.
1. When task B is running, there is an interrupt (ISR) in the system. The interrupt handler runs.
1. When ISR is running, assuming systick priority is higher than the IRQ, systick will pre-empt the ISR and pend the PendSV exception.
1. WHen the scheduler exits, the ISR continues. Since PendSV is the lowest priority, it will run after all other interrupts are done.
1. PendSV handler saves the context of task B and restores the context of task A.

In scheduler, we cant attempt to transition to thread mode when there is unfinished ISR. This will cause usage fault exception.

### PendSV other use cases

* Offloading interrupt processing
* If a higher priority handler is doing time-consuming work, then the other lower priority interrupts will suffer, and systems responsiveness may reduce. This can be solved using a combination of ISR and pendSV handler.

Offloading interrupt processing using PendSV

Interrupts may be services in 2 halves:
1. The first half is the time critical part tha needs to be executed as part of ISR.
1. The second half is called bottom half, is basically delayed execution where rest of time-consuming work will be done.

So, PendSV can be used in this cases, to handle the second half execution by triggerring it in the first half.

1. Task A is running, then an IRQ #0 is triggered.
1. ISR of IRQ #0 is executed executes the time critical part of the ISR and exits by triggering the PendSV exception.
1. PendSV handler is executed and it executes the rest of the ISR, which is the time-consuming part.
1. If there is IRQ #1, it will pre-empt the PendSV handler and run. 
1. Once IRQ #1 is done, PendSV handler will run and finish the rest of the ISR.
1. Once PendSV handler is done, it will return to the task A.