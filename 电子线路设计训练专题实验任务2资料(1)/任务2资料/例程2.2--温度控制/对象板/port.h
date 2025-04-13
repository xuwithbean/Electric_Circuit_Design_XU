#ifndef PORT_H
#define PORT_H

void Port_IO_Init(void);
void Interrupt_Init(void);
void INT1_ISR(void);

extern int page;
extern int pict_page;

#endif