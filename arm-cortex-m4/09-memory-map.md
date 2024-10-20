# Memory map and bus interfaces of ARM Cortex Mx processor

## Memory map

* Memory map explains mapping of different peripherral registers and memories ithe processor addressable memory location range.
* The processor, addressable memory location range, depends upon the size of the address bus.
*the mapping od different regions in the addressable memory location range is called `memory map`.

Microcontroller connect with peripheral using system bus (AHB). System bus has 32 bit address bus and 32 bit data bus. 

To copy data from peripheral, processor will send the address of the peripheral register to the peripheral using the address bus. The peripheral will send the data to the processor using the data bus. Then processor store the data in the memory.

In summary:
* Load from peripheral
* Store to memory

### Memory map of ARM Cortex Mx processor

* ARM Cortex Mx processor has 32 bit address bus.
* The addressable memory location range is 4GB.
* The memory map of ARM Cortex Mx processor is divided into different regions.
* The memory map of ARM Cortex Mx processor is divided into few regions.  The regions are:
    * Code region
    * SRAM region
    * Peripheral region
    * External memory region
    * External device region
    * Private peripheral bus region
    * Vendor specific region
* To access the peripheral registers, we need to know the address of the peripheral register.

### Code region

* 512MB
* The code region is the region where the program is stored.
* Processor fetches the instruction from the code region.

### SRAM region

* Next 512MB of the addressable memory location range.
* Mainly used to connect to on-chip SRAM.

### Peripheral region

* Peripheral memory region also has the size of 512MB.
* The peripheral memory region is used to connect to the on-chip peripheral.
*the first 1MB of the peripheral region is bit addressable if the optional bit banding feature is enabled.
* Execute never region.
* This for MCU vendor peripheral registers e.g.  GPIO, UART, SPI, I2C, etc.

### External memory region

* For external memory like SDRAM, DDRAM, etc.

### Private peripheral bus region

* This region include NVIC, System Control Block, System timer, etc.
* Execute never region.

## Bus Protocol and bus interfaces

* ARM Cortex Mx processor has 3 bus interfaces.
    * System bus (AHB)
    * Peripheral bus (APB)
    * Private peripheral bus (PPB)

* AHB lite bus is maily used for the main bus interfaces
* APB bus is used to connect to the on-chip peripheral.
* AHB majorly used for high speed communication with peripherals that demand high operation speed.
* APB bus is used for low speed communication with peripherals that demand low operation speed.

In arm context interfaces there are 4 AHB interfaces:
* System bus => to connect to various memories and peripherals SRAM, Peripheral, External memory, External device
* Instruction bus => access instruction from code region
* Data bus => access data from code region 
* Private peripheral bus (PPB) => Communicate with private peripheral bus region.

## Bit Banding

* Bit banding is a feature that allows you to access the bit of a memory location using a single instruction.

If we have 1KB of memory, we will have 1024 memory locations. Each memory location will have 8 bits. If we want to access the 5th bit of the 10th memory location, we need to read the 10th memory location and then mask the 5th bit.

```c
__asm volatile ("LDR R0, =0x20000000");
// byte addressing (1 byte = 8 bits)
__asm volatile ("LDRB R1, [R0, #0]");

// half word addressing (1 half word = 16 bits)
__asm volatile ("LDRH R1, [R0, #0]");

// word addressing (1 word = 32 bits)
__asm volatile ("LDR R1, [R0, #0]");
```

Bit banding feature allows you to access the 5th bit of the 10th memory location using a single instruction.

Let's say we want to set the 0th bit of the 0th memory location to 1. In normal method we gonna do:

```c
__asm volatile ("LDR R0, =0x20000000");
// read
__asm volatile ("LDRB R1, [R0, #0]");
// modify
__asm volatile ("ORR R1, R1, #0x01");
// write
__asm volatile ("STRB R1, [R0, #0]");
```

In bit banding, we can just use LDRB to the address dedicated to the 0th bit of the 0th memory location without affecting the other bits.

```c
// =0x22000000 is the alias address of the 0th bit in address 0x20000000
__asm volatile ("LDR R0, =0x22000000");
__asm volatile ("LDRB R1, [R0, #0]");
```

Bit band is only available in SRAM and peripheral region. And only the first 1MB of the peripheral region is bit addressable. This 1MB bit band region have 32MB bit band alias region.

|-----|------|
| Address | Alias Address |
|-----|------|
| 0x20000000 bit[0] | 0x22000000 |
| 0x20000000 bit[1] | 0x22000004 |
| 0x20000000 bit[2] | 0x22000008 |

### Exercise

Modify the content of the memory location 0x2000_0200 using usual and bit banding method and analyze the difference
* First store the value of 0xff into the memory localtion 0x2000_0200
* Make the 7th bit position of the value to 0

Calculation of bit band alias address

Formula:
```
alias_address = alias_base + (32 * (bit_band_memory_addr - bit_band_base)) + (bit_number * 4)

alias address = 0x22000000 + (32 * (0x2000_0200 - 0x2000_0000)) + (7 * 4)
```

```c
#include <stdint.h>

#define BITBAND_ALIAS_BASE 	0x22000000U
#define BITBAND_BASE 		0x20000000U


int main(void)
{
	uint8_t *ptr = (uint8_t *)0x20000200;
	*ptr = 0xFF;
	// clearing
	*ptr &= ~(1 << 7);

	//bit band
	*ptr = 0xFF;
	uint8_t *alias_address = (uint8_t *) (BITBAND_ALIAS_BASE + (32 * (0x20000200U - BITBAND_BASE)) + (7*4));
	*alias_address = 0;

	for(;;);
}
```

> ARM cortext M7 does not support bit banding.
