#ifndef LED_H
#define LED_H

#include "c8051f020.h"

extern unsigned int Time_num;
extern void Delay(uint k);
extern void display(int a);
extern void select(int b);
extern void Led_Port_Init();
extern void LedInit();
extern void LedDispNum();

#endif
