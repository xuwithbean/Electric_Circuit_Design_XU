/************************************************
designed by wjj
wjj12138@stu.xjtu.edu.cn
************************************************/
#include "queue.h"
#include "string.h"
typedef   unsigned   char        uchar; 
void InintQueue(Queue *Q)
{
	memset(Q->base,0,129*sizeof(uchar));
    Q->front = Q->rear = 0;
	Q->number=0;
}
void EnQueue(Queue *Q, unsigned char rows)
{
    if ((Q->rear + 1) % 129 == Q->front)
    {
        Q->front = (Q->front + 1) % 129;
		Q->number-=1;
    }
    Q->base[Q->rear] = rows;
    Q->rear = (Q->rear + 1) % 129;
	Q->number+=1;
}

void ClearQueue(Queue *Q)
{
    Q->front = Q->rear = 0;
	Q->number=0;
}


