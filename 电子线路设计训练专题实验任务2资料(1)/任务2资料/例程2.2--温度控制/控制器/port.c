#include "port.h"

void Port_IO_Init()
{
//    XBR0     = 0X00;         //����XBR0
    XBR1     = 0X14;         //����XBR1
    XBR2     = 0X44;         //�򿪽��濪�غ�������
	  P74OUT  &= 0xcc;
	  P2MDOUT |= 0x7F;
		P0MDOUT|= 0x03;
		P1MDOUT=0xFF;			 //�˿�1����Ϊ�������
		P2MDOUT=0xFF;			 //�˿�2����Ϊ�������
		P3MDOUT=0xFF;			 //�˿�3����Ϊ�������
}
