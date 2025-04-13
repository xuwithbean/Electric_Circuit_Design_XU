/************************************************
designed by wjj
wjj12138@stu.xjtu.edu.cn
************************************************/
#include "sysclk.h"
#include "time.h"
//11059200
#define TIMER0_RELOAD_HIGH 0xff// Timer0高位重载值
#define TIMER0_RELOAD_LOW 0x47   // Timer0低位重载值
//0.2ms
sbit pwm = P0^6;//pwm输出脚为p0.6

//timer0初始化
void Timer0_Init()
{
	TH0 = TIMER0_RELOAD_HIGH;           // 设置高8位
	TL0 = TIMER0_RELOAD_LOW;
	TMOD = 0x01;                        // 设置为16位模式
	ET0 = 1;                            // 使能定时器0
	TR0 = 1;
}

//timer0中断函数
void Timer0_ISR (void) interrupt 1
{
	TH0 = TIMER0_RELOAD_HIGH;           // Timer0高位重载值
	TL0 = TIMER0_RELOAD_LOW;            // Timer0低位重载值

}

//timer3中断函数
void timer3_isr() interrupt 14
{
	DAC0L = temp & (0x00ff);//取出低8位
	DAC0H = temp>>8;//取出高8位
	TMR3CN &= ~(0X80);//手动清0
}