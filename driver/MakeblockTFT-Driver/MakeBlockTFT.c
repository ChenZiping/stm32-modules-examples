/***********************************
** MakeBlockTFT驱动程序          **
** by GDUTROBOT-陈子平 2016-2-10 **
** Github:ChenZiping            **
**********************************/

#include "MakeBlockTFT.h"

/**半主机是这么一种机制，它使得在ARM目标上跑的代码，如果主机电脑运行了调试器，那么该代码可以使用该主机电脑的输入输出设备。
  这点非常重要，因为开发初期，可能开发者根本不知道该 ARM 器件上有什么输入输出设备，而半主基机制使得你不用知道ARM器件的外设，
	利用主机电脑的外设就可以实现输入输出调试。所以要利用目标 ARM器件的输入输出设备，首先要关掉半主机机制。然后再将输入输出
	重定向到 ARM 器件上，如 printf 和 scanf，你需要重写 fputc和 fgetc 函数。下面就是将 scanf 和 printf 重定向到 uart 的代码。 
**/
#pragma import(__use_no_semihosting) //确保没有从C库链接使用半主机的函数 

//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 

/* FILE is typedef’ d in stdio.h. */ 
FILE __stdout;  

//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 

//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART3->SR & 0x40) == 0); //循环发送,直到发送完毕   
	USART3->DR = (u8)ch;      
	return ch;
}

void initTFT(u32 pclk2,u32 bound)
{
	float tmp;
	u16 mantissa;
	u16 fraction;	   
	tmp = (float)(pclk2 * 1000000) / (bound * 16); //得到USARTDIV
	mantissa = tmp; //得到整数部分
	fraction = (tmp - mantissa) * 16; //得到小数部分	 
	mantissa <<= 4;
	mantissa += fraction;
	RCC->APB2ENR |= 1 << 3;  
	RCC->APB1ENR |= 1 << 18; //使能串口3时钟 
	GPIOB->CRH &= 0xFFFF00FF; //IO状态设置
	GPIOB->CRH |= 0x00008B00; //IO状态设置
		  
	RCC->APB1RSTR |= 1 << 18; //复位串口3
	RCC->APB1RSTR &= ~(1 << 18); //停止复位	   	   
	//波特率设置
 	USART3->BRR = mantissa; //波特率设置	 
	USART3->CR1 |= 0x200C; //1位停止,无校验位.  
}

u16 RGB565(u32 RGB888)
{
  u8 r, g, b;
  b = (RGB888 >> 3) & 0x1F; // 取B色的高5位
  g = (RGB888 >> 10) & 0x3F; // 取G色的高6位
  r = (RGB888 >> 19) & 0x1F; // 取R色的高5位
  return((r << 11) + (g << 5) + b);
}

void clearTFT(u16 color)
{
	customsizeColor(color, color);
	printf("CLS(%d);\r\n", color);
}

void customsizeColor(u16 color, u16 n)
{
	printf("SCC(%d,%d);\r\n", color, n);
}

void setBGC(u16 color)
{
	printf("SBC(%d);\r\n", color);
}

void drawPoint(u16 x, u16 y, u16 color)
{
	printf("PS(%d,%d,%d);\r\n", x, y, color);
}

void drawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 color)
{
	printf("PL(%d,%d,%d,%d,%d);\r\n", x1, y1, x2, y2, color);
}
	
void drawBox(u16 x1, u16 y1, u16 x2, u16 y2, u16 color)
{
	printf("BOX(%d,%d,%d,%d,%d);\r\n", x1, y1, x2, y2, color);
}

void drawSolidBox(u16 x1, u16 y1, u16 x2, u16 y2, u16 color)
{
	printf("BOXF(%d,%d,%d,%d,%d);\r\n", x1, y1, x2, y2, color);
}

void dispFigure(u16 x, u16 y, u8 n)
{
	printf("PIC(%d,%d,%d);\r\n", x, y, n);
}

void dispInt24(u16 x, u16 y, u32 num, u16 color)
{
	customsizeColor(color, color);
	printf("DS12(%d,%d,'%d',%d);\r\n", x, y, num, color);
}

void dispChar12(u16 x, u16 y, char* str, u16 color)
{
	printf("DS12(%d,%d,'%s',%d);\r\n", x, y, str, color);
}

void dispChar16(u16 x, u16 y, char* str, u16 color)
{
	printf("DS16(%d,%d,'%s',%d);\r\n", x, y, str, color);
}

void dispChar24(u16 x, u16 y, char* str, u16 color)
{
	printf("DS24(%d,%d,'%s',%d);\r\n", x, y, str, color);
}

void dispChar32(u16 x, u16 y, char* str, u16 color)
{
	printf("DS32(%d,%d,'%s',%d);\r\n", x, y, str, color);
}

void dispChar64(u16 x, u16 y, char* str, u16 color)
{
	printf("DS64(%d,%d,'%s',%d);\r\n", x, y, str, color);
}

void setOrientation(u8 idx)
{
	printf("DR%d;\r\n", idx);
}

void drawPic(const unsigned char* pic)
{
	u16 x, y, tmp = 0, color;
	for(y = 0;y < 120;y ++)
	{
		for(x = 0;x < 160;x ++)
		{
			color = (pic[tmp++] << 8) + pic[tmp++];
			if(color != 0xFFFF)
			{
				printf("SCC(%d,%d);\r\n", color, color);
				printf("PS(%d,%d,%d);\r\n", x, y, color);
			}
		}
	}
}
