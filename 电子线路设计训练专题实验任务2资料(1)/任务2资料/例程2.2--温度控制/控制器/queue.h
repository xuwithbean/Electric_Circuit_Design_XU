#ifndef QUEUE_H
#define QUEUE_H
#include "sysclk.h"
typedef struct
{
    unsigned char base[129];
    unsigned char front, rear;
	unsigned char number;

} Queue;

void InintQueue(Queue *Q);
void EnQueue(Queue *Q, unsigned char rows);
void ClearQueue(Queue *Q);


#endif