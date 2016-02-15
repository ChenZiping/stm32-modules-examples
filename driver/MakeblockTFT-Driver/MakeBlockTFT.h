/***********************************
** MakeBlockTFT驱动程序头文件     **
** by GDUTROBOT-陈子平 2016-2-10 **
** Github:ChenZiping            **
**********************************/
#ifndef _CHENZIPING_MBTFT_H_
#define _CHENZIPING_MBTFT_H_

#include "stm32f10x.h"
#include "stdio.h"

/*********************************
 * 本程序测试芯片为STM32F103ZET6, *
 * 屏幕TX接PB11(USART3_RX)        *
 * 屏幕RX接PB10(USART3_TX)        *
 **********************************/
 
//---------  64 colors of c  ------------------//
// These 64 colors are 'c' value of command
// with this sample code, you can see the color in your LCD.


//---------  Screen coordinates  ---------------//
// The upper left corner is coordinate point(0,0)
// Screen resolution is 320 * 240
// transverse direction is the X direction, 
// the vertical direction is the Y direction;


//---------  Command Description --------------//
// Command                 Description                               sample code         
// CLS(c);                 clear screen with c color                 CLS(0);
// SCC(c,n)                customizing colors for c                  SCC(15,65535);
// SBC(c)                  Set Background Color                      SBC(1)
// PS(x,y,c)               draw a point at (x, y) with color c       PS(100,100,1);
// PL(x1,y1,x2,y2,c);      draw line from (x1,y1) to (x2,y2)         PL(0,0,100,0,2);
// BOX(x1,y1,x2,y2,c);     draw rectangle,Left corner(x1,y1)         BOX(0,0,100,100,2);
//                         Bottom right corner(x2,y2)    
// BOXF(x1,y1,x2,y2,c);    draw Solid rectangle,Left corner(x1,y1)   BOXF(0,0,100,100,2);
// PIC(x,y,n)              painted figure n at(x,y)                  PIC(0,0,1);
// DS12(x,y,'String',c);   display character 12 dot matrix characters
// DS16(x,y,'String',c);   display character 16 dot matrix characters
// DS24(x,y,'String',c);   display character 24 dot matrix characters
// DS32(x,y,'String',c);   display character 32 dot matrix characters
// DS64(x,y,'String',c);   display character 64 dot matrix characters
// DRn                     Set the screen orientation; n is (0-3), 
//                         corresponding to four directions screen,   DR0: horizontal screen display
//                         you can use this to adjust the horizontal  DR1: vertical screen display
//                         and vertical screen display; This command  DR2: horizontal screen upside down
//                         will not clear screen                      DR3: vertical screen upside down
// Others: see the doc   

u16 RGB565(u32 RGB888);
void initTFT(u32 pclk2,u32 bound);//pclk2:PCLK2时钟频率(Mhz),bound:波特率
void clearTFT(u16 color);
void customsizeColor(u16 color, u16 n);
void setBGC(u16 color);
void drawPoint(u16 x, u16 y, u16 color);
void drawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 color);
void drawBox(u16 x1, u16 y1, u16 x2, u16 y2, u16 color);
void drawSolidBox(u16 x1, u16 y1, u16 x2, u16 y2, u16 color);
void dispFigure(u16 x, u16 y, u8 n);
void dispInt24(u16 x, u16 y, u32 num, u16 color);
void dispChar12(u16 x, u16 y, char* str, u16 color);
void dispChar16(u16 x, u16 y, char* str, u16 color);
void dispChar24(u16 x, u16 y, char* str, u16 color);
void dispChar32(u16 x, u16 y, char* str, u16 color);
void dispChar64(u16 x, u16 y, char* str, u16 color);
void drawPic(const unsigned char* pic);
void setOrientation(u8 idx);

#define BLACK RGB565(0x000000)
#define GRAY RGB565(0xC0C0C0)
#define RED RGB565(0xFF0000)
#define PINK RGB565(0xFFC0CB)
#define YELLOW RGB565(0xFFFF00)
#define ORANGE RGB565(0xFF6100)
#define BROWN RGB565(0x734A12)
#define BLUE RGB565(0x0000FF)
#define CYAN RGB565(0x00FFFF)
#define GREEN RGB565(0x00FF00)
#define PURPLE RGB565(0xA020F0)
#define WHITE RGB565(0xFFFFFF)

#endif /* _CHENZIPING_MBTFT_H_ */
