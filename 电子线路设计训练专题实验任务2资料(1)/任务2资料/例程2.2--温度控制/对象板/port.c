/************************************************
designed by wjj
wjj12138@stu.xjtu.edu.cn
************************************************/
#include "sysclk.h"
#include "port.h"

void Port_IO_Init()
{
	 XBR0=0xCC;//cp0连到端口，pac0连到端口，cex0连到端口，rx0，tx0连到端口
	 XBR1=0x13;//int1连到端口，t0连到端口，cp1连到端口，这些功能只是用于交差开关的占位，并不用实际功能
	 XBR2=0x40;//使能交差开关
	 P0MDOUT=0xff;//p0端口推挽输出
   P74OUT |= 0x01;//将p4低4位设置为推挽输出
	 P4 &= 0xfd;//将p4.1为0
}
