#ifndef TIME_H
#define TIME_H

extern unsigned int Time_num;
extern int zkb;
extern int temp;
void Timer0_Init(void);
void Timer0_ISR (void);

#endif