#include "time.h"

sfr16 RCAP4 = 0xE4;               
sfr16 TMR4 = 0xF4; 

//定时器3初始化
void timer3_init()
{
	TMR3CN = 0x00;//手动清除溢出标志位
	TMR3RLL = 0xdc;//100hz
	//TMR3RLL = 0xfc;
	TMR3RLH = 0Xef;
	TMR3L = 0Xff;
	TMR3H = 0Xff; //直接溢出
	EIE2 |= 0x01;//允许timer3中断
	TMR3CN |= 0x04;//允许定时器3
	EIE2&=0xFE;
}

//定时器4初始化
void TIMER4_Init (int counts)
{   
   T4CON = 0x00;    //手动清除溢出标志位
   CKCON |= 0x40;   // 定时器使用系统时钟
   RCAP4 = -counts; //设置重载值
   TMR4 = RCAP4;// 初始化定时器4
   EIE2 |= 0x04;  // 使能定时器4中断
   T4CON |= 0x04;  // 打开定时器4

}

