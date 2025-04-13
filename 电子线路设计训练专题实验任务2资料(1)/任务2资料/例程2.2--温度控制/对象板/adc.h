#ifndef ADC_H
#define ADC_H
#include "sysclk.h"

void ADC0_Init (void);
void ADC0_ISR (void);
void timer3_init( void);
extern int zkb;
extern int temp;
#endif