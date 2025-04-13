#ifndef LED_H
#define LED_H

#include "c8051f020.h"
#include "lcd.h"

extern unsigned int Time_num;
extern unsigned int Time_num2;
extern unsigned int Time_num3;
//extern unsigned char xdata temp[12];
extern void display(int a);
extern void select(int b);
extern void Led_Port_Init();
extern void LedInit();
extern void LedDispNum();
extern int mima1[4];
void show_f_row(void);
void show_s_row(void);
void show_t_row(void);
void show_mima_row(int* codenum1234[4]);
extern void          _nop_     (void);
void Delay_us(unsigned int times);
#endif
