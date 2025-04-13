#include "adc.h"
#include "dight.h"
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
