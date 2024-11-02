# Interrupt Priority

What is priority value: A measure of urgency. 

For instance:
* ADC (priority value 5)
* Timer (priority value 3)

In the above example, TIMER has higher priority than ADC.

The lesser the value, the higher the priority.

## Different priority levels

Priority levels are also called as priority levels.

How many priority levels are there in ARM Cortex M4 processor?
It depends on the implementation. STM32F4 has 16 priority levels. TI TM4C123 has 8 priority levels.

## Interrupt Priority Registers

This is part of the NVIC peripheral. There are 60 interrupt priority registers.

* Interrupt priority registers are 32 bit registers.
* Each register can store 4 priority values.
* Each priority value is 8 bits.

In IPR0 register, the first 8 bits are used to store the priority value of IRQ0 and so on.

In STM32 mcu, it uses 4 bits for each priority value. So, it can have 16 priority levels.

:::warn
If you want to configure priority for system exception, you need to configure on System Handler Priority Register (SHPR), not NVIC.
::: 

## Pre-emption and Sub-priority

What if two interrupts with the same priority level occurs at the same time?

The sub priority values of the interrupts will be checked to resolve the conflict. An interrupt with lower sub priority value will be given higher priority.

### Priority Grouping

**Pre-empt priority**: when the processor is running interrupt handler, and another interrupt appears, then the pre-empt priority value will be compared, and interrupt with higher pre-empt priority(lesser value) will be given higher priority.

**Sun priority**: This value is used only when two interrupts with the same pre-empt priority occur at the same time. In this case, the exception with higher sub-priority will be handled first.

Example:
* Priority group 0: 7 pre-empt priority bits, 1 sub-priority bit.
* Priority group 1: 6 pre-empt priority bits, 2 sub-priority bits. etc.

Priority grouping can be configured using the AIRCR register.

#### Case 1

When priority group = 0
* Pre-empty priority will be 7 bits, and sub-priority will be 1 bit.
* But only 4 bits are used for pre-empt priority and 0 bits for sub-priority.
* Sub-priority width = 1 bits. Bit 0 is not implemented, so no sub-priority levels

What is the pre-empt and sub-priority same? IRQ number will be used to resolve the conflict.

## Exercise

Generate the below peripheral interrupts using NVIC interrupt pending register and observe the execution of ISR when priorities are different

TIM2 global interrupt
I2C1 event interrupt

```c
#include <stdint.h>
#include <stdio.h>

uint32_t *pNVIC_IPRBase = (uint32_t *)0xE000E400;
uint32_t *pNVIC_ISPRBase = (uint32_t *)0XE000E200;
uint32_t *pNVIC_ISERBase = (uint32_t *)0xE000E100;

#define ISER0			0xE000E100U
#define TIM2_IRQNUM 	51
#define I2C1_EV_IRQNUM	76

void configure_priority(uint8_t irq_number, uint8_t priority_value)
{
	// configure priority for the peripheral
	// IRQ number is divided by 4 to get the IPR position
	uint8_t iprx = irq_number / 4;
	uint32_t *pIPR= pNVIC_IPRBase + iprx;

	// IRQ number is modulo by 4 and multiplied by 8 to get how much we have to shift
	uint8_t pos = (irq_number % 4) * 8;

	*pIPR &= ~(0xFF << pos); //clear
	*pIPR |= (priority_value << pos); //set
}

void set_pending(uint8_t irq_number)
{
	uint8_t ipsrx = (irq_number / 32);
	uint32_t *pISPR = pNVIC_ISPRBase + ipsrx;

	uint8_t pos = irq_number % 32;
	*pISPR |= (1 << pos);
}

void enable_interrupt(uint8_t irq_number)
{
	uint8_t iserx = (irq_number / 32);
	uint32_t *pISER = pNVIC_ISERBase + iserx;

	uint8_t pos = irq_number % 32;
	*pISER |= (1 << pos);
}

int main(void)
{
	configure_priority(TIM2_IRQNUM, 0x80);
	configure_priority(I2C1_EV_IRQNUM, 0x80);
	set_pending(TIM2_IRQNUM);
	enable_interrupt(TIM2_IRQNUM);
	enable_interrupt(I2C1_EV_IRQNUM);
	for(;;);
}

// in cortex m7 i2c has lower priority than tim2
void I2C1_EV_IRQHandler(void)
{
	printf("i2c");
}

void TIM2_IRQHandler(void)
{
	printf("tim2");
	// when the priority of tim2 and i2c1 is the same, the expectation is that this handler is never get intterupted.
	set_pending(I2C1_EV_IRQNUM);
	while(1);
}
```

The observation is that eventhough we pend the i2c interrupt, the tim2 interrupt is not interrupted. This is because the priority of tim2 is similar with i2c1.

```c
int main(void)
{
	configure_priority(TIM2_IRQNUM, 0x80);
	configure_priority(I2C1_EV_IRQNUM, 0x80);
	set_pending(TIM2_IRQNUM);
	enable_interrupt(TIM2_IRQNUM);
	enable_interrupt(I2C1_EV_IRQNUM);
	for(;;);
}
```

Now if we change the priority of i2c1 to 0x70, the tim2 interrupt will be interrupted by i2c1 interrupt.
