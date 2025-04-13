/************************************************
designed by wjj
wjj12138@stu.xjtu.edu.cn
************************************************/
#include "sysclk.h"
#include "time.h"
//11059200
#define TIMER0_RELOAD_HIGH 0xff// Timer0��λ����ֵ
#define TIMER0_RELOAD_LOW 0x47   // Timer0��λ����ֵ
//0.2ms
sbit pwm = P0^6;//pwm�����Ϊp0.6

//timer0��ʼ��
void Timer0_Init()
{
	TH0 = TIMER0_RELOAD_HIGH;           // ���ø�8λ
	TL0 = TIMER0_RELOAD_LOW;
	TMOD = 0x01;                        // ����Ϊ16λģʽ
	ET0 = 1;                            // ʹ�ܶ�ʱ��0
	TR0 = 1;
}

//timer0�жϺ���
void Timer0_ISR (void) interrupt 1
{
	TH0 = TIMER0_RELOAD_HIGH;           // Timer0��λ����ֵ
	TL0 = TIMER0_RELOAD_LOW;            // Timer0��λ����ֵ

}

//timer3�жϺ���
void timer3_isr() interrupt 14
{
	DAC0L = temp & (0x00ff);//ȡ����8λ
	DAC0H = temp>>8;//ȡ����8λ
	TMR3CN &= ~(0X80);//�ֶ���0
}