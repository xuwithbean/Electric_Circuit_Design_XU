#include "sysclk.h"

void SYSCLK_Init (void)
{
   int i;                      
   OSCXCN = 0x67;              // ѡ�������������ؾ���Ϊ11.0592
   for (i=0; i<256; i++);      // ��ʱ�þ����ȶ�
   while (!(OSCXCN & 0x80));   // ��ȡ���λ���жϾ����Ƿ��ȶ�
   OSCICN = 0x88;              // ѡ���ⲿ������Ϊʱ��
}

void Delay_us(unsigned int times)//����һ����ʱ����
{
    unsigned int i;

    for (i=0; i<times; i++)
    {
        _nop_();
        _nop_();
        _nop_();
		    _nop_();
    }
}
