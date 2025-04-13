#ifndef SYSCLK_H
#define SYSCLK_H
#include "c8051f020.h"
#define SYSCLK 11059200
#define SAMPLERATE0 50000

void SYSCLK_Init (void);
void _nop_ (void);
void Delay_us(unsigned int times);

#endif