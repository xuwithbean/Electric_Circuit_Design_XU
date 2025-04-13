#ifndef PID_H
#define PID_H
#include "sysclk.h"

void PIDOutput (); 
void PIDOperation (); 
extern PIDValueStr PID;
extern uint8 out ;                
extern uint8 count;
#endif