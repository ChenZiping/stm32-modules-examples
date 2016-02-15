/******************************
** LCD12864������������ͷ�ļ� **
** by IMA-����ƽ  2016-2-2    **
** Github:ChenZiping          **
********************************/
#ifndef _CHENZIPING_LCD12864_H_
#define _CHENZIPING_LCD12864_H_

#include "stm32f10x.h"
#include "delay.h"

//////////////////////
//LCD12864���Žӷ���//
//RS Pin:PB3        //
//R/W Pin:PB4       //
//E Pin:PB5         //
//DB0-DB7:PD0-PD7   //
//////////////////////
#define RSout(i) ((i == 1) ? (GPIOB->BSRR |= (u16)(1 << 3)) : (GPIOB->BSRR |= (u16)(1 << 19))) 
#define RWout(i) ((i == 1) ? (GPIOB->BSRR |= (u16)(1 << 4)) : (GPIOB->BSRR |= (u16)(1 << 20)))
#define ENout(i) ((i == 1) ? (GPIOB->BSRR |= (u16)(1 << 5)) : (GPIOB->BSRR |= (u16)(1 << 21)))
#define DATA_IN GPIOD->IDR //��������
#define DATA_OUT GPIOD->ODR &= 0xFF00;GPIOD->ODR //�������
#define BUSY_STATE ((DATA_IN) & 0x80) //DB7:1æ,0����

#define LCD_CLEAR writeLCD12864(0x01) //����(����ģʽ)
#define LCD_RETURN writeLCD12864(0x02) //���ص�ԭ��
#define LCD_OPPO_1_3 writeLCD12864(0x04) //����1��3��
#define LCD_OPPO_2_4 writeLCD12864(0x05) //����2��4��
#define LCD_CLOSE writeLCD12864(0x08) //����ʾ(����˯��ģʽ)
#define LCD_OPEN writeLCD12864(0x0C) //����ʾ & �ع��(�˳�˯��ģʽ)
#define LCD_CURSOR_OPEN writeLCD12864(0x0D) //�����
#define LCD_CURSOR_LEFT writeLCD12864(0x10) //�������
#define LCD_CURSOR_RIGHT writeLCD12864(0x14) //�������
#define LCD_LEFT writeLCD12864(0x18) //��Ļ�������ƣ������棨�����ڹ�����ʾ��
#define LCD_RIGHT writeLCD12864(0x1C) //��Ļ�������ƣ������棨�����ڹ�����ʾ��
#define LCD_STD writeLCD12864(0x30) //�������ָ��ģʽ
#define LCD_EXT writeLCD12864(0x34) //������չ����ģʽ����ͼ��ʾ�ر�

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
