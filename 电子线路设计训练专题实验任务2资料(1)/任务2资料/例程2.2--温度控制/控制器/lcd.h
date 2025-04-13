/************************************************
designed by zlx
wjj12138@stu.xjtu.edu.cn
************************************************/
#ifndef LCD_H
#define LCD_H
#include "queue.h"
#define uint unsigned int
#define uchar unsigned char
void Lcd_Port_Init(void);
void LcdInit(void);

void Delay_ms(uint t);
void Delay(uint k);

void SendByte(uchar Dbyte);
uchar ReceiveByte(void);
void CheckBusy(void);

void WriteData(uchar WDLCM);
void WriteCommand(uchar Cbyte);

void LineClear(uchar l);
void LcdClear(void);

//void WriteChar(uchar row, uchar col, uchar ch);
void WriteStr(uchar row, uchar col, uchar *puts);

void ImageShow(uchar *imagePtr);
//void drawPoint(uchar x, uchar y, uchar color);
//void clearPoint(uchar x, uchar y, uchar color);


void drawPoint2(uchar x_index,uchar y,unsigned char tempH,unsigned char tempL, uchar color);
void LcdClearHalf(uchar direction);

void ScreenShow(uchar *imagePtr,uchar *reference,uchar direction);
#endif