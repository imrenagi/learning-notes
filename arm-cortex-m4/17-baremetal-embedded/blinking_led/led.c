#include<stdint.h>
#include "led.h"

#define GPIOB 1
#define GPIOD 3

#define PIN_7 7
#define PIN_10 10
#define PIN_13 13

void led_init_all(void)
{

	uint32_t volatile *const pRCC_AHB4_ENR = (uint32_t*)(0x58024400 + 0x140);
	uint32_t volatile *const pGPIOD_MODE_R = (uint32_t*)(0x58020C00 + 0x00);
	uint32_t volatile *const pGPIOB_MODE_R = (uint32_t*)(0x58020400 + 0x00);

	// enable RCC for GPIOD and GPIOB
	*pRCC_AHB4_ENR |= (1 << 1); //enable GPIO B
	*pRCC_AHB4_ENR |= (1 << 3); //enable GPIO D

	// setting mode to output
	// PB7 mode output
	*pGPIOB_MODE_R &= ~(0x03 << (2*PIN_7));
	*pGPIOB_MODE_R |= (0x1 << (2*PIN_7));

	// PD10 mode output
	*pGPIOD_MODE_R &= ~(0x03 << (2*PIN_10));
	*pGPIOD_MODE_R |= (0x1 << (2*PIN_10));

	// PD13 mode output
	*pGPIOD_MODE_R &= ~(0x03 << (2*PIN_13));
	*pGPIOD_MODE_R |= (0x1 << (2*PIN_13));

    led_off(LED_GREEN);
    led_off(LED_ORANGE);
    led_off(LED_RED);
}

void led_on(uint8_t led_no)
{
	uint32_t volatile *const pGPIOD_DATAOUT_R = (uint32_t*)(0x58020C00 + 0x14);
	uint32_t volatile *const pGPIOB_DATAOUT_R = (uint32_t*)(0x58020400 + 0x14);

	switch(led_no) {
	case LED_GREEN:
		*pGPIOD_DATAOUT_R |= (0x1 << PIN_10);
		break;
	case LED_ORANGE:
		*pGPIOD_DATAOUT_R |= (0x1 << PIN_13);
		break;
	case LED_RED:
		*pGPIOB_DATAOUT_R |= (0x1 << PIN_7);
		break;
	}
}

void led_off(uint8_t led_no)
{
	uint32_t volatile *const pGPIOD_DATAOUT_R = (uint32_t*)(0x58020C00 + 0x14);
	uint32_t volatile *const pGPIOB_DATAOUT_R = (uint32_t*)(0x58020400 + 0x14);

	switch(led_no) {
	case LED_GREEN:
		*pGPIOD_DATAOUT_R &= ~(1 << PIN_10);
		break;
	case LED_ORANGE:
		*pGPIOD_DATAOUT_R &= ~(1 << PIN_13);
		break;
	case LED_RED:
		*pGPIOB_DATAOUT_R &= ~(1 << PIN_7);
		break;
	}
}
