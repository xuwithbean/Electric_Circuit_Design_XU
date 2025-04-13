/************************************************
designed by wjj
wjj12138@stu.xjtu.edu.cn
************************************************/
#include "adc.h"
#include "dight.h"

//adc0初始化
void ADC0_Init ()
{
	  ADC0CN = 0xC4;       // 关闭ADC0; 低能追踪模式
    REF0CN = 0x03;        // 打开内部基准电压
    AMX0SL=0x00;     // ADC0使用"0"通道
	  AMX0CF=0x00;
    ADC0CF=31<<3;         // ADC转换时钟= 2.5MHz
    ADC0CF&=0xf8;          // PGA gain = 1
    EIE2|=0x02;        // 打开ADC0中断
}

//timer3初始化
void timer3_init()
{
	TMR3CN = 0x00;//手动清除溢出标志位
	TMR3RLL = 0xdc;//100hz
	//TMR3RLL = 0xfc;
	TMR3RLH = 0Xff;
	TMR3L = 0Xff;//
	TMR3H = 0Xff;//直接溢出
	EIE2 |= 0x01;//允许timer3中断
	TMR3CN |= 0x04;//允许定时器3
}

//adc0溢出中断服务函数
//ADC0有多个通道，每个通道上可以连接不同的数据，同一时间只有一个通道会被打开，之后读取数据，通道建立需要时间
void ADC0_ISR () interrupt 15
{
		/*int adc_re = ADC0H;
		adc_re = (adc_re <<8)|(ADC0L);
		zkb = adc_re;
		zkb = (zkb-770)/31;*/
	
		static int ad_e = 0;
		int adc_re = ADC0H;
		adc_re = (adc_re <<8)|(ADC0L);//接受adc0的数值
		if(ad_e == 0)
		{
			//AMX0SL=0x00;//这个先则是显示1
			zkb = adc_re;
			//zkb = (zkb-770)/31;//显示0通道数据
			zkb = zkb / 33;
			up_dight(zkb);
			AMX0SL=0x00;//改变通道
			ad_e = 1;
		}else
		{
			//AMX0SL=0x01;//这个先则是显示0
			temp = adc_re;//显示1通道数据
			meind_dight(temp);
			AMX0SL=0x01;//改变通道
			ad_e = 0;
		}
}