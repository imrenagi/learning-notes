#include <stdint.h>

#define SRAM_START 0x20000000U
#define SRAM_SIZE  (64 * 1024U) // 128 KB
#define SRAM_END   ((SRAM_START) + (SRAM_SIZE))

#define STACK_START SRAM_END 

extern uint32_t _etext;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;

extern int main(void);

/*
if we give section attribute here, then the function will be moved to that section
instead of occupying the .text section

void Reset_Handler(void) __attribute__ ((section(".random_section")));

$ arm-none-eabi-objdump -d stm32_startup.o
stm32_startup.o:     file format elf32-littlearm
Disassembly of section .random_section:
00000000 <Reset_Handler>:
   0:   b480            push    {r7}
   2:   af00            add     r7, sp, #0
   4:   bf00            nop
   6:   46bd            mov     sp, r7
   8:   bc80            pop     {r7}
   a:   4770            bx      lr
*/
void Reset_Handler(void);


/* 
weak attribute is used to tell the compiler that this is a weak function
it lets programmer to override this function with the same name

alias attribute is used to give an alias name to the function.
so if we call NMI_Handler, it will call Default_Handler if NMI_Handler is not defined
https://gcc.gnu.org/onlinedocs/gcc-13.3.0/gcc/Common-Variable-Attributes.html
*/
void NMI_Handler                    (void) __attribute__ ((weak, alias("Default_Handler")));
void HardFault_Handler              (void) __attribute__ ((weak, alias("Default_Handler")));
void MemManage_Handler              (void) __attribute__ ((weak, alias("Default_Handler")));
void BusFault_Handler               (void) __attribute__ ((weak, alias("Default_Handler")));
void UsageFault_Handler             (void) __attribute__ ((weak, alias("Default_Handler")));
void SVC_Handler 					(void) __attribute__ ((weak, alias("Default_Handler")));
void DebugMon_Handler 				(void) __attribute__ ((weak, alias("Default_Handler")));
void PendSV_Handler   				(void) __attribute__ ((weak, alias("Default_Handler")));
void SysTick_Handler  				(void) __attribute__ ((weak, alias("Default_Handler")));
void PVD_AVD_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void DTS_Handler                    (void) __attribute__ ((weak, alias("Default_Handler")));
void IWDG_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void WWDG_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void RCC_Handler                    (void) __attribute__ ((weak, alias("Default_Handler")));
void LOOKUP_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void CACHE_ECC_Handler             (void) __attribute__ ((weak, alias("Default_Handler")));
void FLASH_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void ECC_FPU_Handler               (void) __attribute__ ((weak, alias("Default_Handler")));
void FPU_Handler                    (void) __attribute__ ((weak, alias("Default_Handler")));
void TAMP_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI0_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI1_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI2_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI3_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI4_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI5_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI6_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI7_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI8_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI9_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI10_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI11_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI12_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI13_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI14_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI15_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void RTC_Handler                    (void) __attribute__ ((weak, alias("Default_Handler")));
void SAES_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void AES_Handler                    (void) __attribute__ ((weak, alias("Default_Handler")));
void PKA_Handler                    (void) __attribute__ ((weak, alias("Default_Handler")));
void HASH_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void RNG_Handler                    (void) __attribute__ ((weak, alias("Default_Handler")));
void ADC1_2_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void GPDMA1_CH0_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void GPDMA1_CH1_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void GPDMA1_CH2_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void GPDMA1_CH3_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void GPDMA1_CH4_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void GPDMA1_CH5_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void GPDMA1_CH6_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void GPDMA1_CH7_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM1_BRK_Handler              (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM1_UP_Handler               (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM1_TRG_COM_Handler          (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM1_CC_Handler               (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM2_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM3_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM4_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM5_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM6_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM7_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM9_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI1_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI2_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI3_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI4_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI5_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI6_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void HPDMA1_CH0_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void HPDMA1_CH1_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void HPDMA1_CH2_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void HPDMA1_CH3_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void HPDMA1_CH4_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void HPDMA1_CH5_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void HPDMA1_CH6_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void HPDMA1_CH7_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void SAI1_A_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void SAI1_B_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void SAI2_A_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void SAI2_B_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C1_EV_Handler               (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C1_ER_Handler               (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C2_EV_Handler               (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C2_ER_Handler               (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C3_EV_Handler               (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C3_ER_Handler               (void) __attribute__ ((weak, alias("Default_Handler")));
void USART1_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void USART2_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void USART3_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void UART4_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void UART5_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void UART7_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void UART8_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void I3C1_EV_Handler               (void) __attribute__ ((weak, alias("Default_Handler")));
void I3C1_ER_Handler               (void) __attribute__ ((weak, alias("Default_Handler")));
void OTG_HS_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void ETH_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void CORDIC_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void GFXTIM_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void DCMIPP_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void LTDC_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void LTDC_ER_Handler               (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA2D_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void JPEG_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void GFXMMU_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void I3C1_WKUP_Handler             (void) __attribute__ ((weak, alias("Default_Handler")));
void MCE1_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void MCE2_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void MCE3_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void OSPI1_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void OSPI2_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void FMC_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void SDMMC1_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void SDMMC2_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void OTG_FS_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM12_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM13_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM14_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM15_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM16_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM17_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void LPTIM1_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void LPTIM2_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void LPTIM3_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void LPTIM4_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void LPTIM5_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void SPDIF_RX_Handler              (void) __attribute__ ((weak, alias("Default_Handler")));
void MDIOS_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void ADF1_FLT0_Handler             (void) __attribute__ ((weak, alias("Default_Handler")));
void CRS_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void UCPD1_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void CEC_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void PSSI_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void LPUART1_Handler               (void) __attribute__ ((weak, alias("Default_Handler")));
void WAKEUP_PIN_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void GPDMA1_CH8_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void GPDMA1_CH9_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void GPDMA1_CH10_Handler           (void) __attribute__ ((weak, alias("Default_Handler")));
void GPDMA1_CH11_Handler           (void) __attribute__ ((weak, alias("Default_Handler")));
void GPDMA1_CH12_Handler           (void) __attribute__ ((weak, alias("Default_Handler")));
void GPDMA1_CH13_Handler           (void) __attribute__ ((weak, alias("Default_Handler")));
void GPDMA1_CH14_Handler           (void) __attribute__ ((weak, alias("Default_Handler")));
void GPDMA1_CH15_Handler           (void) __attribute__ ((weak, alias("Default_Handler")));
void HPDMA1_CH8_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void HPDMA1_CH9_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void HPDMA1_CH10_Handler           (void) __attribute__ ((weak, alias("Default_Handler")));
void HPDMA1_CH11_Handler           (void) __attribute__ ((weak, alias("Default_Handler")));
void HPDMA1_CH12_Handler           (void) __attribute__ ((weak, alias("Default_Handler")));
void HPDMA1_CH13_Handler           (void) __attribute__ ((weak, alias("Default_Handler")));
void HPDMA1_CH14_Handler           (void) __attribute__ ((weak, alias("Default_Handler")));
void HPDMA1_CH15_Handler           (void) __attribute__ ((weak, alias("Default_Handler")));
void GPU2D_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void GPU2D_ER_Handler              (void) __attribute__ ((weak, alias("Default_Handler")));
void TCACHE_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void FDCAN1_IT0_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void FDCAN1_IT1_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void FDCAN2_IT0_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void FDCAN2_IT1_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));


// initialized data will go to data section. but we need to put this on user defined section
// to tell the compiler to move this to user defined section we need to use variable attribute
// https://gcc.gnu.org/onlinedocs/gcc-13.3.0/gcc/Common-Variable-Attributes.html 
// we can use section attribute to move the variable to user defined section
/*
$ arm-none-eabi-objdump -h stm32_startup.o 
stm32_startup.o:     file format elf32-littlearm

Sections:
Idx Name          Size      VMA       LMA       File off  Algn
  0 .text         0000000c  00000000  00000000  00000034  2**1
                  CONTENTS, ALLOC, LOAD, READONLY, CODE
  1 .data         00000000  00000000  00000000  00000040  2**0
                  CONTENTS, ALLOC, LOAD, DATA
  2 .bss          00000000  00000000  00000000  00000040  2**0
                  ALLOC
  3 .isr_vector   00000008  00000000  00000000  00000040  2**2
                  CONTENTS, ALLOC, LOAD, RELOC, DATA
  4 .comment      00000046  00000000  00000000  00000048  2**0
                  CONTENTS, READONLY
  5 .ARM.attributes 0000002e  00000000  00000000  0000008e  2**0
                  CONTENTS, READONLY

we can see there is .isr_vector section which is of size 8 bytes
*/
uint32_t vectors[] __attribute__ ((section (".isr_vector"))) = {
    STACK_START,                    // 0x0000 0000
    (uint32_t)&Reset_Handler,       // 0x0000 0004
    (uint32_t)&NMI_Handler,         // 0x0000 0008
    (uint32_t)&HardFault_Handler,   // 0x0000 000C
    (uint32_t)&MemManage_Handler,   // 0x0000 0010
    (uint32_t)&BusFault_Handler,    // 0x0000 0014
    (uint32_t)&UsageFault_Handler,  // 0x0000 0018
    (uint32_t)0,                    // 0x0000 001C
    (uint32_t)0,                    // 0x0000 0020
    (uint32_t)0,                    // 0x0000 0024
    (uint32_t)0,                    // 0x0000 0028
    (uint32_t)&SVC_Handler,         // 0x0000 002C
    (uint32_t)&DebugMon_Handler,    // 0x0000 0030
    (uint32_t)0,                    // 0x0000 0034
    (uint32_t)&PendSV_Handler,      // 0x0000 0038
    (uint32_t)&SysTick_Handler,     // 0x0000 003C
    
    // External Interrupts
    (uint32_t)&PVD_AVD_Handler,     // 0x0000 0040
    (uint32_t)0,                    // 0x0000 0044 Reserved
    (uint32_t)&DTS_Handler,         // 0x0000 0048
    (uint32_t)&IWDG_Handler,        // 0x0000 004C
    (uint32_t)&WWDG_Handler,        // 0x0000 0050
    (uint32_t)&RCC_Handler,         // 0x0000 0054
    (uint32_t)&LOOKUP_Handler,      // 0x0000 0058
    (uint32_t)&CACHE_ECC_Handler,   // 0x0000 005C
    (uint32_t)&FLASH_Handler,       // 0x0000 0060
    (uint32_t)&ECC_FPU_Handler,     // 0x0000 0064
    (uint32_t)&FPU_Handler,         // 0x0000 0068
    (uint32_t)0,                    // 0x0000 006C Reserved
    (uint32_t)0,                    // 0x0000 0070 Reserved
    (uint32_t)&TAMP_Handler,        // 0x0000 0074
    (uint32_t)0,                    // 0x0000 0078 Reserved (product secure)
    (uint32_t)0,                    // 0x0000 007C Reserved (product secure)
    (uint32_t)&EXTI0_Handler,       // 0x0000 0080
    (uint32_t)&EXTI1_Handler,       // 0x0000 0084
    (uint32_t)&EXTI2_Handler,       // 0x0000 0088
    (uint32_t)&EXTI3_Handler,       // 0x0000 008C
    (uint32_t)&EXTI4_Handler,       // 0x0000 0090
    (uint32_t)&EXTI5_Handler,       // 0x0000 0094
    (uint32_t)&EXTI6_Handler,       // 0x0000 0098
    (uint32_t)&EXTI7_Handler,       // 0x0000 009C
    (uint32_t)&EXTI8_Handler,       // 0x0000 00A0
    (uint32_t)&EXTI9_Handler,       // 0x0000 00A4
    (uint32_t)&EXTI10_Handler,      // 0x0000 00A8
    (uint32_t)&EXTI11_Handler,      // 0x0000 00AC
    (uint32_t)&EXTI12_Handler,      // 0x0000 00B0
    (uint32_t)&EXTI13_Handler,      // 0x0000 00B4
    (uint32_t)&EXTI14_Handler,      // 0x0000 00B8
    (uint32_t)&EXTI15_Handler,      // 0x0000 00BC
    (uint32_t)&RTC_Handler,         // 0x0000 00C0
    (uint32_t)&SAES_Handler,        // 0x0000 00C4
    (uint32_t)&AES_Handler,         // 0x0000 00C8
    (uint32_t)&PKA_Handler,         // 0x0000 00CC
    (uint32_t)&HASH_Handler,        // 0x0000 00D0
    (uint32_t)&RNG_Handler,         // 0x0000 00D4
    (uint32_t)&ADC1_2_Handler,      // 0x0000 00D8
    (uint32_t)&GPDMA1_CH0_Handler,  // 0x0000 00DC
    (uint32_t)&GPDMA1_CH1_Handler,  // 0x0000 00E0
    (uint32_t)&GPDMA1_CH2_Handler,  // 0x0000 00E4
    (uint32_t)&GPDMA1_CH3_Handler,  // 0x0000 00E8
    (uint32_t)&GPDMA1_CH4_Handler,  // 0x0000 00EC
    (uint32_t)&GPDMA1_CH5_Handler,  // 0x0000 00F0
    (uint32_t)&GPDMA1_CH6_Handler,  // 0x0000 00F4
    (uint32_t)&GPDMA1_CH7_Handler,  // 0x0000 00F8
    (uint32_t)&TIM1_BRK_Handler,    // 0x0000 00FC
    (uint32_t)&TIM1_UP_Handler,     // 0x0000 0100
    (uint32_t)&TIM1_TRG_COM_Handler,// 0x0000 0104
    (uint32_t)&TIM1_CC_Handler,     // 0x0000 0108
    (uint32_t)&TIM2_Handler,        // 0x0000 010C
    (uint32_t)&TIM3_Handler,        // 0x0000 0110
    (uint32_t)&TIM4_Handler,        // 0x0000 0114
    (uint32_t)&TIM5_Handler,        // 0x0000 0118
    (uint32_t)&TIM6_Handler,        // 0x0000 011C
    (uint32_t)&TIM7_Handler,        // 0x0000 0120
    (uint32_t)&TIM9_Handler,        // 0x0000 0124
    (uint32_t)&SPI1_Handler,        // 0x0000 0128
    (uint32_t)&SPI2_Handler,        // 0x0000 012C
    (uint32_t)&SPI3_Handler,        // 0x0000 0130
    (uint32_t)&SPI4_Handler,        // 0x0000 0134
    (uint32_t)&SPI5_Handler,        // 0x0000 0138
    (uint32_t)&SPI6_Handler,        // 0x0000 013C
    (uint32_t)&HPDMA1_CH0_Handler,  // 0x0000 0140
    (uint32_t)&HPDMA1_CH1_Handler,  // 0x0000 0144
    (uint32_t)&HPDMA1_CH2_Handler,  // 0x0000 0148
    (uint32_t)&HPDMA1_CH3_Handler,  // 0x0000 014C
    (uint32_t)&HPDMA1_CH4_Handler,  // 0x0000 0150
    (uint32_t)&HPDMA1_CH5_Handler,  // 0x0000 0154
    (uint32_t)&HPDMA1_CH6_Handler,  // 0x0000 0158
    (uint32_t)&HPDMA1_CH7_Handler,  // 0x0000 015C
    (uint32_t)&SAI1_A_Handler,      // 0x0000 0160
    (uint32_t)&SAI1_B_Handler,      // 0x0000 0164
    (uint32_t)&SAI2_A_Handler,      // 0x0000 0168
    (uint32_t)&SAI2_B_Handler,      // 0x0000 016C
    (uint32_t)&I2C1_EV_Handler,     // 0x0000 0170
    (uint32_t)&I2C1_ER_Handler,     // 0x0000 0174
    (uint32_t)&I2C2_EV_Handler,     // 0x0000 0178
    (uint32_t)&I2C2_ER_Handler,     // 0x0000 017C
    (uint32_t)&I2C3_EV_Handler,     // 0x0000 0180
    (uint32_t)&I2C3_ER_Handler,     // 0x0000 0184
    (uint32_t)&USART1_Handler,      // 0x0000 0188
    (uint32_t)&USART2_Handler,      // 0x0000 018C
    (uint32_t)&USART3_Handler,      // 0x0000 0190
    (uint32_t)&UART4_Handler,       // 0x0000 0194
    (uint32_t)&UART5_Handler,       // 0x0000 0198
    (uint32_t)&UART7_Handler,       // 0x0000 019C
    (uint32_t)&UART8_Handler,       // 0x0000 01A0
    (uint32_t)&I3C1_EV_Handler,     // 0x0000 01A4
    (uint32_t)&I3C1_ER_Handler,     // 0x0000 01A8
    (uint32_t)&OTG_HS_Handler,      // 0x0000 01AC
    (uint32_t)&ETH_Handler,         // 0x0000 01B0
    (uint32_t)&CORDIC_Handler,      // 0x0000 01B4
    (uint32_t)&GFXTIM_Handler,      // 0x0000 01B8
    (uint32_t)&DCMIPP_Handler,      // 0x0000 01BC
    (uint32_t)&LTDC_Handler,        // 0x0000 01C0
    (uint32_t)&LTDC_ER_Handler,     // 0x0000 01C4
    (uint32_t)&DMA2D_Handler,       // 0x0000 01C8
    (uint32_t)&JPEG_Handler,        // 0x0000 01CC
    (uint32_t)&GFXMMU_Handler,      // 0x0000 01D0
    (uint32_t)&I3C1_WKUP_Handler,   // 0x0000 01D4
    (uint32_t)&MCE1_Handler,        // 0x0000 01D8
    (uint32_t)&MCE2_Handler,        // 0x0000 01DC
    (uint32_t)&MCE3_Handler,        // 0x0000 01E0
    (uint32_t)&OSPI1_Handler,       // 0x0000 01E4
    (uint32_t)&OSPI2_Handler,       // 0x0000 01E8
    (uint32_t)&FMC_Handler,         // 0x0000 01EC
    (uint32_t)&SDMMC1_Handler,      // 0x0000 01F0
    (uint32_t)&SDMMC2_Handler,      // 0x0000 01F4
    (uint32_t)0,                    // 0x0000 01F8 Reserved
    (uint32_t)0,                    // 0x0000 01FC Reserved
    (uint32_t)&OTG_FS_Handler,      // 0x0000 0200
    (uint32_t)&TIM12_Handler,       // 0x0000 0204
    (uint32_t)&TIM13_Handler,       // 0x0000 0208
    (uint32_t)&TIM14_Handler,       // 0x0000 020C
    (uint32_t)&TIM15_Handler,       // 0x0000 0210
    (uint32_t)&TIM16_Handler,       // 0x0000 0214
    (uint32_t)&TIM17_Handler,       // 0x0000 0218
    (uint32_t)&LPTIM1_Handler,      // 0x0000 021C
    (uint32_t)&LPTIM2_Handler,      // 0x0000 0220
    (uint32_t)&LPTIM3_Handler,      // 0x0000 0224
    (uint32_t)&LPTIM4_Handler,      // 0x0000 0228
    (uint32_t)&LPTIM5_Handler,      // 0x0000 022C
    (uint32_t)&SPDIF_RX_Handler,    // 0x0000 0230
    (uint32_t)&MDIOS_Handler,       // 0x0000 0234
    (uint32_t)&ADF1_FLT0_Handler,   // 0x0000 0238
    (uint32_t)&CRS_Handler,         // 0x0000 023C
    (uint32_t)&UCPD1_Handler,       // 0x0000 0240
    (uint32_t)&CEC_Handler,         // 0x0000 0244
    (uint32_t)&PSSI_Handler,        // 0x0000 0248
    (uint32_t)&LPUART1_Handler,     // 0x0000 024C
    (uint32_t)&WAKEUP_PIN_Handler,  // 0x0000 0250
    (uint32_t)&GPDMA1_CH8_Handler,  // 0x0000 0254
    (uint32_t)&GPDMA1_CH9_Handler,  // 0x0000 0258
    (uint32_t)&GPDMA1_CH10_Handler, // 0x0000 025C
    (uint32_t)&GPDMA1_CH11_Handler, // 0x0000 0260
    (uint32_t)&GPDMA1_CH12_Handler, // 0x0000 0264
    (uint32_t)&GPDMA1_CH13_Handler, // 0x0000 0268
    (uint32_t)&GPDMA1_CH14_Handler, // 0x0000 026C
    (uint32_t)&GPDMA1_CH15_Handler, // 0x0000 0270
    (uint32_t)&HPDMA1_CH8_Handler,  // 0x0000 0274
    (uint32_t)&HPDMA1_CH9_Handler,  // 0x0000 0278
    (uint32_t)&HPDMA1_CH10_Handler, // 0x0000 027C
    (uint32_t)&HPDMA1_CH11_Handler, // 0x0000 0280
    (uint32_t)&HPDMA1_CH12_Handler, // 0x0000 0284
    (uint32_t)&HPDMA1_CH13_Handler, // 0x0000 0288
    (uint32_t)&HPDMA1_CH14_Handler, // 0x0000 028C
    (uint32_t)&HPDMA1_CH15_Handler, // 0x0000 0290
    (uint32_t)&GPU2D_Handler,       // 0x0000 0294
    (uint32_t)&GPU2D_ER_Handler,    // 0x0000 0298
    (uint32_t)&TCACHE_Handler,      // 0x0000 029C
    (uint32_t)&FDCAN1_IT0_Handler,  // 0x0000 02A0
    (uint32_t)&FDCAN1_IT1_Handler,  // 0x0000 02A4
    (uint32_t)&FDCAN2_IT0_Handler,  // 0x0000 02A8
    (uint32_t)&FDCAN2_IT1_Handler   // 0x0000 02AC
};

void Default_Handler(void) {
    while(1);
}

void Reset_Handler(void) { 
    /*
    // copy .data section to SRAM
    1. To copy data from flash to sram, we need to know the boundary of .data section
    boundary includes _edata, _sdata, _etext
    this will be exported by linker script
    */
    uint32_t size = &_edata - &_sdata;
    uint8_t *pDst = (uint8_t*)&_sdata; // sram address
    uint8_t *pSrc = (uint8_t*)&_etext; // flash address
    
    for (uint32_t i = 0; i < size; i++) {
        *pDst++ = *pSrc++;
    }

    // initialize .bss section to zero
    size = &_ebss - &_sbss;
    pDst = (uint8_t*)&_sbss;
    for (uint32_t i = 0; i < size; i++) {
        *pDst++ = 0; 
    }

    // call init function of standard library

    // call main
    main();
}