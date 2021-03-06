/***************************
** LCD12864并行驱动程序   **
** by IMA-陈子平 2016-2-2 **
** Github:ChenZiping      **
****************************/
#include "lcd12864.h"

void initLCD12864()
{
	RCC->APB2ENR |= 0x28; // 使能PB、PD时钟
	GPIOB->CRL &= 0xFF000FFF; 
	GPIOB->CRL |= 0x00777000; // PB3-5 开漏输出
	GPIOD->CRL &= 0x00000000;
	GPIOD->CRL |= 0x77777777; // PD0-7 开漏输出
	LCD_OPEN;
	delayUs_(100);
	LCD_CURSOR_OPEN;
	delayUs_(100);
	writeLCD12864(0x36); //扩展指令操作，绘图开
	LCD_CLEAR;
	delayUs_(100);
	LCD_RETURN;
	delayUs_(100);
}

//////////////////////////////////////
//函数名称：waitLCD12864()
//参数：无
//功能：等待LCD变为空闲状态
//////////////////////////////////////
void waitLCD12864()
{
	u16 flag = 0;
	RSout(0);
	RWout(1);
	do
	{
		ENout(0);
		delayUs_(5); //延时时间需要根据不同MCU设置
		ENout(1);
		delayUs_(5); //延时时间需要根据不同MCU设置
		flag = BUSY_STATE;
	}while(flag);
}

///////////////////////////////////////////
//函数名称：writeLCD12864(data)
//参数：data 指令或者为一个一字节数据
//功能：写指令或者一个一字节数据到LCD12864，
//也可以用来显示一个字符
///////////////////////////////////////////
void writeLCD12864(u8 data)
{
	waitLCD12864();
	RSout(0);
	RWout(0);
	ENout(1);
	DATA_OUT |= data;
	delayUs_(600); //延时时间需要根据不同MCU设置
	ENout(0);
	RWout(1);
	RSout(1);//
}

//////////////////////////////////////
//函数名称：setCursor(row, col)
//参数：row 行1-4, col 列1-8
//功能：设置LCD12864的光标位置
//说明：	
//LCD12864屏幕：
//第一行：0x80 0x81 ---- 0x86 0x87 
//第二行：0x90 0x91 ---- 0x96 0x97	
//第三行：0x88 0x89 ---- 0x8E 0x8F	
//第四行：0x98 0x99 ---- 0x9E 0x9F 
//////////////////////////////////////
void setCursor(u8 row, u8 col)
{
	u8 pos = 0x00;
	if(row > 2) pos = 0x08;
	if(row & 1) pos |= 0x80;
	else pos |= 0x90;
	pos += col - 1;
	writeLCD12864(pos);
}

//////////////////////////////////////
//函数名称：dispChar(row, col, ch)
//参数：row 行1-4, col 列1-8, ch 字符
//功能：LCD12864显示一个字符 
//////////////////////////////////////
void dispChar(u8 row, u8 col, char ch)
{
	setCursor(row, col);
	writeLCD12864(ch);
}

//////////////////////////////////////////
//函数名称：dispStr(row, col, str[])
//参数：row 行1-4, col 列1-8, str[] 字符串
//功能：LCD12864显示一个字符串
//////////////////////////////////////////
void dispStr(u8 row, u8 col, char str[])
{
	u16 i;
	setCursor(row, col);
	for(i = 0; str[i] != '\0'; i ++) writeLCD12864(str[i]);
}

//////////////////////////////////////////
//函数名称：dispPic(picture[])
//参数：picture[] 图片数组
//功能：LCD12864显示一张128*64的图片
//说明：
//12864液晶分为上半屏和下半屏。当你想要点亮某个地方时，
//必须先写这个地方的水平地址紧接着写入它的垂直地址，
//水平地址液晶可以自动加1，而垂直地址不会。每个垂直地
//址只确定一排，所以水平和垂直地址不能确定某个点的位置，
//只能确定某个两字节的位置，通过写进2字节数据确定点亮
//某个点或几个点。
//////////////////////////////////////////
void dispPic(const unsigned char picture[])
{
	u8 i, j;
	LCD_EXT;
	delayMs_(20);
	for(i = 0;i < 32;i ++)
	{
		writeLCD12864(0x80 + i);
		delayUs_(150);
		writeLCD12864(0x80);
		delayUs_(150);
		for(j = 0;j < 16;j ++)
		{
			writeLCD12864(*picture);
			picture ++;
			delayUs_(150);
		}
		delayUs_(150);
	}
	for(i = 0;i < 32;i ++)
	{
		writeLCD12864(0x80 + i);
		delayUs_(150);
		writeLCD12864(0x88);
		delayUs_(150);
		for(j = 0;j < 16;j ++)
		{
			writeLCD12864(*picture);
			picture ++;
			delayUs_(150);
		}
		delayUs_(150);
	}
	writeLCD12864(0x36); //写完数据,开图形显示
	delayMs_(50);
}

//////////////////////////////////////////
//函数名称：flickerScreen()
//参数：无
//功能：LCD12864屏幕闪烁
//////////////////////////////////////////
void flickerScreen()
{
	LCD_CLOSE;
	delayMs_(500);
	LCD_OPEN;
	delayMs_(500);
}
