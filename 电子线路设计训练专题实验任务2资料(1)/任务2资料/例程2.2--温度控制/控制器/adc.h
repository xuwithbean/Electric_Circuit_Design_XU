#ifndef ADC_H
#define ADC_H
#include "sysclk.h"

void ADC0_Init (void);
void ADC0_ISR (void);
extern int temp;
#endif