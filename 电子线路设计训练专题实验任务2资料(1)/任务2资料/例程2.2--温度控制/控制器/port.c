#include "port.h"

void Port_IO_Init()
{
//    XBR0     = 0X00;         //设置XBR0
    XBR1     = 0X14;         //设置XBR1
    XBR2     = 0X44;         //打开交叉开关和弱上拉
	  P74OUT  &= 0xcc;
	  P2MDOUT |= 0x7F;
		P0MDOUT|= 0x03;
		P1MDOUT=0xFF;			 //端口1设置为推挽输出
		P2MDOUT=0xFF;			 //端口2设置为推挽输出
		P3MDOUT=0xFF;			 //端口3设置为推挽输出
}
