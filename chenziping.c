/**CopyRight (c) 2015 - 2016 ChenZiping
**My Library start at 2015.12
**Design for STM32F10x MCU
**Github:ChenZiping
**/
#include "chenziping.h"

void initRCC()
{
  RCC -> APB1RSTR = 0x00000000;
  RCC -> APB2RSTR = 0x00000000;
  RCC -> AHBENR = 0x00000014; //睡眠模式闪存和SRAM时钟使能.其他关闭.
	RCC -> APB2ENR = 0x00000000; //外设时钟关闭.
	RCC -> APB1ENR = 0x00000000;
  RCC -> CR |= 0x00000001; //开启内部8MHz时钟
  RCC -> CFGR &= 0xF8FF0000; //复位SW[1:0],HPRE[3:0],PPRE1[2:0],PPRE2[2:0],ADCPRE[1:0],MCO[2:0]
	RCC -> CR &= 0xFEF6FFFF; //复位HSEON,CSSON,PLLON
	RCC -> CR &= 0xFFFBFFFF; //复位HSEBYP
	RCC -> CFGR &= 0xFF80FFFF; //复位PLLSRC, PLLXTPRE, PLLMUL[3:0] and USBPRE
	RCC -> CIR = 0x00000000; //关闭所有中断
	//setVectorTable(0x20000000, 0x0);
}

static u8  fac_us = 0;//us延时倍乘数			   
static u16 fac_ms = 0;//ms延时倍乘数,在ucos下,代表每个节拍的ms数

void setVectorTable(u32 NVIC_VectTab, u32 Offset)	 
{ 	   	 
	SCB -> VTOR = NVIC_VectTab|(Offset & (u32)0x1FFFFF80); //设置NVIC的向量表偏移寄存器
	//用于标识向量表是在CODE区还是在RAM区
}

void initClock(u8 PLL)//PLL:2-16
{
  uc tmp = 0;
  initRCC();
  RCC -> CR |= 1 << 16; //外部高速时钟使能HSEON
	while(!(RCC -> CR >> 17)); //等待外部时钟就绪
	RCC -> CFGR = 0x00000400; //0X00002000; //APB1=DIV2;APB2=DIV1;AHB=DIV1;
	PLL -= 2; //抵消2个单位
	RCC -> CFGR |= PLL << 18; //设置PLL值 2~16
	RCC -> CFGR |= 1 << 16; //PLLSRC ON
	//FLASH -> ACR |= 0x32; //FLASH 2个延时周期
	RCC -> CR |= 0x01000000; //PLLON
	while(!(RCC -> CR >> 25)); //等待PLL锁定
	RCC -> CFGR |= 0x00000002; //PLL作为系统时钟
	while(tmp != 0x02) //等待PLL作为系统时钟设置成功
	{
		tmp = RCC -> CFGR >> 2;
		tmp &= 0x03; //tmp读取SWS值(系统时钟切换状态)
	}
	
}

void initSysTick(u8 SYSCLK)
{
	SysTick -> CTRL &= ~(1 << 2);	//SYSTICK使用外部时钟源	 
	fac_us = SYSCLK / 8; //不论是否使用ucos,fac_us都需要使用
	fac_ms = (u16)fac_us * 1000;
}

//延时nus!=0
//nus为要延时的us数.		    								   
void delayUs(u32 nus)
{		
	u32 tmp;	    	 
	SysTick -> LOAD = nus * fac_us; //时间加载	  		 
	SysTick -> VAL = 0; //清空计数器
	SysTick -> CTRL = 0x01; //开始倒数 	 
	do
	{
		tmp = SysTick -> CTRL;
	}
	while((tmp & 0x01) && !(tmp & (1 << 16)));//等待时间到达   
	SysTick -> CTRL = 0; //关闭计数器
	SysTick -> VAL = 0; //清空计数器	 
}

//延时nms!=0
//注意nms的范围
//SysTick->LOAD为24位寄存器,所以,最大延时为:
//nms<=0xffffff*8*1000/SYSCLK
//SYSCLK单位为Hz,nms单位为ms
//对72M条件下,nms<=1864 
void delayMs(u16 nms)
{	 		  	  
	u32 tmp;		   
	SysTick -> LOAD = (u32) nms * fac_ms; //时间加载(SysTick->LOAD为24bit)
	SysTick -> VAL = 0; //清空计数器
	SysTick -> CTRL = 0x01; //开始倒数  
	do
	{
		tmp = SysTick -> CTRL;
	}
	while((tmp & 0x01) && !((tmp & (1 << 16)))); //等待时间到达   
	SysTick -> CTRL = 0; //关闭计数器
	SysTick -> VAL = 0; //清空计数器	  	    
} 

//设置NVIC分组
//NVIC_Group:NVIC分组 0~4 总共5组 		   
void configNVICGroup(u8 NVIC_Group)	 
{ 
	u32 tmp,tmp1;	  
	tmp1 = (~NVIC_Group) & 0x07; //取后三位
	tmp1 <<= 8;
	tmp = SCB -> AIRCR; //读取先前的设置
	tmp &= 0X0000F8FF; //清空先前分组
	tmp |= 0X05FA0000; //写入钥匙
	tmp |= tmp1;	   
	SCB -> AIRCR = tmp; //设置分组	    	  				   
}

//设置NVIC 
//NVIC_PreemptionPriority:抢占优先级
//NVIC_SubPriority       :响应优先级
//NVIC_Channel           :中断编号
//NVIC_Group             :中断分组 0~4
//注意优先级不能超过设定的组的范围!否则会有意想不到的错误
//组划分:
//组0:0位抢占优先级,4位响应优先级
//组1:1位抢占优先级,3位响应优先级
//组2:2位抢占优先级,2位响应优先级
//组3:3位抢占优先级,1位响应优先级
//组4:4位抢占优先级,0位响应优先级
//NVIC_SubPriority和NVIC_PreemptionPriority的原则是,数值越小,越优先	   
void initNVIC(u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority,u8 NVIC_Channel,u8 NVIC_Group)	 
{ 
	u32 tmp;	
	configNVICGroup(NVIC_Group); //设置分组
	tmp = NVIC_PreemptionPriority << (4 - NVIC_Group);	  
	tmp |= NVIC_SubPriority & (0x0F >> NVIC_Group);
	tmp &= 0xf; //取低四位  
	NVIC -> ISER[NVIC_Channel / 32] |= (1 << NVIC_Channel % 32); //使能中断位(要清除的话,相反操作就OK) 
	NVIC -> IP[NVIC_Channel] |= tmp << 4; //设置响应优先级和抢断优先级   	    	  				   
} 

//外部中断配置函数
//只针对GPIOA~G;不包括PVD,RTC和USB唤醒这三个
//参数:
//GPIOx:0~6,代表GPIOA~G
//BITx:需要使能的位;
//TRIM:触发模式,1,下升沿;2,上降沿;3，任意电平触发
//该函数一次只能配置1个IO口,多个IO口,需多次调用
//该函数会自动开启对应中断,以及屏蔽线   	    
void configEXTI(GPIO_Type GPIOx,u8 BITx,u8 TRIM) 
{
	u8 EXTADDR;
	u8 EXTOFFSET;
	EXTADDR = BITx / 4; //得到中断寄存器组的编号
	EXTOFFSET = (BITx % 4) * 4; 
	RCC -> APB2ENR |= 0x01; //使能io复用时钟			 
	AFIO -> EXTICR[EXTADDR] &= ~(0x000F << EXTOFFSET); //清除原来设置！！！
	AFIO -> EXTICR[EXTADDR] |= GPIOx << EXTOFFSET; //EXTI.BITx映射到GPIOx.BITx 
	//自动设置
	EXTI -> IMR |= 1 << BITx; //开启line BITx上的中断
	//EXTI->EMR|=1<<BITx;//不屏蔽line BITx上的事件 (如果不屏蔽这句,在硬件上是可以的,但是在软件仿真的时候无法进入中断!)
 	if(TRIM & 0x01) EXTI -> FTSR |= 1 << BITx; //line BITx上事件下降沿触发
	if(TRIM & 0x02) EXTI -> RTSR |= 1 << BITx; //line BITx上事件上升降沿触发
}

void initEXTI(void)
{
	configEXTI(GPIO_A, 0, RTIR); //上升沿触发
	configEXTI(GPIO_C, 5, FTIR); //下降沿触发
	configEXTI(GPIO_A, 15, FTIR); //下降沿触发

	initNVIC(2, 2, EXTI0_IRQn, 2); //抢占2，子优先级2，组2
	initNVIC(2, 1, EXTI9_5_IRQn, 2); //抢占2，子优先级1，组2
	initNVIC(2, 0, EXTI15_10_IRQn, 2);	//抢占2，子优先级0，组2	   
}

//TIM1_CH1 PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM1_PWM_Init(u16 arr,u16 psc)
{		 					 
	//此部分需手动修改IO口设置
	RCC -> APB2ENR |= 1<<11; //TIM1时钟使能    
	GPIOA -> CRH &= 0XFFFFFFF0;	//PA8清除之前的设置
	GPIOA -> CRH |= 0X0000000B;	//复用功能输出 
	
	TIM1 -> ARR = arr; //设定计数器自动重装值 
	TIM1 -> PSC = psc; //预分频器设置
  
	TIM1 -> CCMR1 |= 7 << 4; //CH1 PWM2模式		 
	TIM1 -> CCMR1 |= 1 << 3; //CH1 预装载使能	 
 	TIM1 -> CCER |= 1 << 0; //OC1 输出使能	   
	TIM1 -> BDTR |= 1 << 15; //MOE 主输出使能	   

	TIM1 -> CR1 = 0x80; //ARPE使能：TIM1_ARR寄存器被装入缓冲器 
	// TIM1 -> CR1 |= 1 << 4; //向下计数
	TIM1 -> CR1 |= 1; //使能计数器1 
}

void TIM3_Int_Init(u16 arr,u16 psc)
{
	RCC -> APB1ENR |= 1 << 1;	//TIM3时钟使能  
	GPIOA -> CRH &= 0XFFFFFFF0;	//PA8清除之前的设置
	GPIOA -> CRH |= 0X0000000B;	//复用功能输出 
  
 	TIM3 -> ARR = arr; //设定计数器自动重装值 
	TIM3 -> PSC = psc; //预分频器设置
	TIM3 -> DIER |= 1; //允许更新中断				
	TIM3 -> CR1 |= 1; //使能定时器3
  initNVIC(1, 3, TIM3_IRQn, 2); //抢占1，子优先级3，组2									 
}

void TIM3_IRQHandler(void)
{ 		    		  			    
	if(TIM3 -> SR & 1) //溢出中断
	{
			PCout(12) = ~PCout(12);
	}				   
	TIM3 -> SR &= ~(1 << 0); //清除中断标志位 	    
} 
