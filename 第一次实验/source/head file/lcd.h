#ifndef LCD_H
#define LCD_H

#include "c8051f020.h"
#include <intrins.h>
#include <stdio.h>

#define uchar unsigned char          
#define uint  unsigned int 

void Lcd_Port_Init(void);
void LcdInit(void);

void Delay_ms(uint t);
void Delay(uint k);

void SendByte(uchar Dbyte);
uchar ReceiveByte(void);
void CheckBusy(void);

void WriteData(uchar WDLCM);
void WriteCommand (uchar Cbyte);

void LineClear(uchar l);
void LcdClear( void );

void WriteChar(uchar row,uchar col,uchar ch);
void WriteStr(uchar row,uchar col,uchar *puts);

void ImageShow(uchar *imagePtr);
void ImageShow1(uchar *imagePtr);
//void ImageShow3(uchar *imagePtr);

extern volatile int xdata wavevalue[128];
extern const unsigned char code gImage_white[1024];
extern const unsigned char code axis[1024];
extern const unsigned char code xjtu_image1[1024];
extern const unsigned char code xjtu_image2[1024];
extern const unsigned char code xjtu_image3[512];
extern const unsigned char code gImage_lock1[1024];
extern const unsigned char code gImage_lock2[1024];
extern const unsigned char code gImage_lock3[1024];
extern const unsigned char code gImage_lock4[1024];
#endif
