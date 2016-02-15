/**CopyRight (c) 2015 - 2016 ChenZiping
**My Library start at 2015.12
**Design for STM32F10x MCU
**Github:ChenZiping
**/
#ifndef _CHENZIPING_H_
#define _CHENZIPING_H_

/****** The include file ******/
#include "stm32f10x.h"//STM32F10x
#include "stdio.h"//Standard input & output stream
/****** The include file ******/

/****** Some useful typedef ******/
typedef unsigned long long u64;
typedef signed   long long s64;
/****** Some useful typedef ******/

/****** Some useful define ******/
#define uc unsigned char
#define ui unsigned int
/****** Some useful define ******/

/****** Bit band define ******/
#define BITBAND(addr, bitnum) ((addr & 0xF0000000) + 0x2000000 + ((addr & 0xFFFFF) << 5) + (bitnum << 2))//(PERIPH_BASE + ((addr - PERIPH_BASE) * 8 + bitnum) * 4)
#define MEM_ADDR(addr) *((volatile unsigned long *)(addr))
#define BIT_ADDR(addr, bitnum) MEM_ADDR(BITBAND(addr, bitnum))
/****** Bit band define ******/

/****** The Flash size & Unique device ID register define ******/
#define FLASH_SIZE_BASE (0x1FFFF7E0)
#define UID_0_15_BASE (FLASH_SIZE_BASE + 0x08)
#define UID_16_31_BASE (FLASH_SIZE_BASE + 0x0A)
#define UID_32_63_BASE (FLASH_SIZE_BASE + 0x0C)
#define UID_64_95_BASE (FLASH_SIZE_BASE + 0x0F)
#define FLASH_SIZE MEM_ADDR(FLASH_SIZE_BASE)
#define UID_0_15 MEM_ADDR(UID_0_15_BASE)
#define UID_16_31 MEM_ADDR(UID_16_31_BASE)
#define UID_32_63 MEM_ADDR(UID_32_63_BASE)
#define UID_64_95 MEM_ADDR(UID_64_95_BASE)
/****** The Flash size & Unique device ID register define ******/

/****** RCC register define ******/
#define RCC_CR_BASE (RCC_BASE + 0x00)/*0x000**(undefined)83*/
#define RCC_CFGR_BASE (RCC_BASE + 0x04)/*0x00000000*/
#define RCC_CIR_BASE (RCC_BASE + 0x08)/*0x00000000*/
#define RCC_APB2RSTR_BASE (RCC_BASE + 0x0C)/*0x00000000*/
#define RCC_APB1RSTR_BASE (RCC_BASE + 0x10)/*0x00000000*/
#define RCC_AHBENR_BASE (RCC_BASE + 0x14)/*0x00000014*/
#define RCC_APB2ENR_BASE (RCC_BASE + 0x18)/*0x00000000*/
#define RCC_APB1ENR_BASE (RCC_BASE + 0x1C)/*0x00000000*/
#define RCC_BDCR_BASE (RCC_BASE + 0x20)/*0x00000000*/
#define RCC_CSR_BASE (RCC_BASE + 0x24)/*0x00000000*/
#define RCC_AHBRSTR_BASE (RCC_BASE + 0x28)/*0x00000000*/
#define RCC_CFGR2_BASE (RCC_BASE + 0x2C)/*0x00000000*/

#define APB2ENR_IOA BIT_ADDR(RCC_APB2ENR_BASE,2)
#define APB2ENR_IOB BIT_ADDR(RCC_APB2ENR_BASE,3)
#define APB2ENR_IOC BIT_ADDR(RCC_APB2ENR_BASE,4)
#define APB2ENR_IOD BIT_ADDR(RCC_APB2ENR_BASE,5)
#define APB2ENR_IOE BIT_ADDR(RCC_APB2ENR_BASE,6)
#define APB2ENR_IOF BIT_ADDR(RCC_APB2ENR_BASE,7)
#define APB2ENR_IOG BIT_ADDR(RCC_APB2ENR_BASE,8)
/****** RCC register define ******/

/****** GPIO input & output define ******/
#define GPIOA_ODR_BASE (GPIOA_BASE + 0x0C) //0x4001080C
#define GPIOB_ODR_BASE (GPIOB_BASE + 0x0C) //0x40010C0C
#define GPIOC_ODR_BASE (GPIOC_BASE + 0x0C) //0x4001100C
#define GPIOD_ODR_BASE (GPIOD_BASE + 0x0C) //0x4001140C
#define GPIOE_ODR_BASE (GPIOE_BASE + 0x0C) //0x4001180C
#define GPIOF_ODR_BASE (GPIOF_BASE + 0x0C) //0x40011A0C
#define GPIOG_ODR_BASE (GPIOG_BASE + 0x0C) //0x40011E0C

#define GPIOA_IDR_BASE (GPIOA_BASE + 0x08) //0x40010808
#define GPIOB_IDR_BASE (GPIOB_BASE + 0x08) //0x40010C08
#define GPIOC_IDR_BASE (GPIOC_BASE + 0x08) //0x40011008
#define GPIOD_IDR_BASE (GPIOD_BASE + 0x08) //0x40011408
#define GPIOE_IDR_BASE (GPIOE_BASE + 0x08) //0x40011808
#define GPIOF_IDR_BASE (GPIOF_BASE + 0x08) //0x40011A08
#define GPIOG_IDR_BASE (GPIOG_BASE + 0x08) //0x40011E08

#define PAout(n) BIT_ADDR(GPIOA_ODR_BASE,n)
#define PAin(n)  BIT_ADDR(GPIOA_IDR_BASE,n)

#define PBout(n) BIT_ADDR(GPIOB_ODR_BASE,n)
#define PBin(n)  BIT_ADDR(GPIOB_IDR_BASE,n)

#define PCout(n) BIT_ADDR(GPIOC_ODR_BASE,n)
#define PCin(n)  BIT_ADDR(GPIOC_IDR_BASE,n)

#define PDout(n) BIT_ADDR(GPIOD_ODR_BASE,n)
#define PDin(n)  BIT_ADDR(GPIOD_IDR_BASE,n)

#define PEout(n) BIT_ADDR(GPIOE_ODR_BASE,n)
#define PEin(n)  BIT_ADDR(GPIOE_IDR_BASE,n)

#define PFout(n) BIT_ADDR(GPIOF_ODR_BASE,n)
#define PFin(n)  BIT_ADDR(GPIOF_IDR_BASE,n)

#define PGout(n) BIT_ADDR(GPIOG_ODR_BASE,n)
#define PGin(n)  BIT_ADDR(GPIOG_IDR_BASE,n)
/****** GPIO input & output define ******/

/****** System clock config ******/
void initRCC(void);

void setVectorTable(u32 NVIC_VectTab, u32 Offset);

void initClock(u8 PLL);
/****** System clock config ******/

/****** SysTick delay config ******/
void initSysTick(u8 SYSCLK);

void delayUs(u32 nus);

void delayMs(u16 nms);
/****** SysTick delay config ******/

/****** NVIC config ******/
typedef enum
{
	GPIO_A = 0,
	GPIO_B = 1,
	GPIO_C = 2,
	GPIO_D = 3,
	GPIO_E = 4,
	GPIO_F = 5,
	GPIO_G = 6
} GPIO_Type;

#define FTIR 1
#define RTIR 2

void configNVICGroup(u8 NVIC_Group);

void initNVIC(u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority,u8 NVIC_Channel,u8 NVIC_Group);

void configEXTI(GPIO_Type GPIOx,u8 BITx,u8 TRIM);

void TIM1_PWM_Init(u16 arr,u16 psc);

void TIM3_Int_Init(u16 arr,u16 psc);

void TIM3_IRQHandler(void);
/****** NVIC config ******/



#endif /*_CHENZIPING_H_*/
