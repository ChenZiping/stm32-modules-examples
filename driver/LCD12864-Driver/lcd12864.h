/******************************
** LCD12864并行驱动程序头文件 **
** by IMA-陈子平  2016-2-2    **
** Github:ChenZiping          **
********************************/
#ifndef _CHENZIPING_LCD12864_H_
#define _CHENZIPING_LCD12864_H_

#include "stm32f10x.h"
#include "delay.h"

//////////////////////
//LCD12864引脚接法：//
//RS Pin:PB3        //
//R/W Pin:PB4       //
//E Pin:PB5         //
//DB0-DB7:PD0-PD7   //
//////////////////////
#define RSout(i) ((i == 1) ? (GPIOB->BSRR |= (u16)(1 << 3)) : (GPIOB->BSRR |= (u16)(1 << 19))) 
#define RWout(i) ((i == 1) ? (GPIOB->BSRR |= (u16)(1 << 4)) : (GPIOB->BSRR |= (u16)(1 << 20)))
#define ENout(i) ((i == 1) ? (GPIOB->BSRR |= (u16)(1 << 5)) : (GPIOB->BSRR |= (u16)(1 << 21)))
#define DATA_IN GPIOD->IDR //数据输入
#define DATA_OUT GPIOD->ODR &= 0xFF00;GPIOD->ODR //数据输出
#define BUSY_STATE ((DATA_IN) & 0x80) //DB7:1忙,0空闲

#define LCD_CLEAR writeLCD12864(0x01) //清屏(待命模式)
#define LCD_RETURN writeLCD12864(0x02) //光标回到原点
#define LCD_OPPO_1_3 writeLCD12864(0x04) //反白1、3行
#define LCD_OPPO_2_4 writeLCD12864(0x05) //反白2、4行
#define LCD_CLOSE writeLCD12864(0x08) //关显示(进入睡眠模式)
#define LCD_OPEN writeLCD12864(0x0C) //开显示 & 关光标(退出睡眠模式)
#define LCD_CURSOR_OPEN writeLCD12864(0x0D) //开光标
#define LCD_CURSOR_LEFT writeLCD12864(0x10) //光标左移
#define LCD_CURSOR_RIGHT writeLCD12864(0x14) //光标右移
#define LCD_LEFT writeLCD12864(0x18) //屏幕内容左移，光标跟随（可用于滚动显示）
#define LCD_RIGHT writeLCD12864(0x1C) //屏幕内容右移，光标跟随（可用于滚动显示）
#define LCD_STD writeLCD12864(0x30) //进入基本指令模式
#define LCD_EXT writeLCD12864(0x34) //进入扩展功能模式，绘图显示关闭

void initLCD12864(void);
void waitLCD12864(void);
void writeLCD12864(u8 cmd);
void readLCD12864(void);
void setCursor(u8 row, u8 col);
void dispChar(u8 row, u8 col, char ch);
void dispStr(u8 row, u8 col, char str[]);
void dispPic(const unsigned char picture[]);
void flickerScreen(void);

#endif /* _CHENZIPING_LCD12864_H_ */
