#include "adc.h"
#include "dight.h"
void ADC0_Init ()
{
	  ADC0CN = 0xC4;       // �ر�ADC0; ����׷��ģʽ
    REF0CN = 0x03;        // ���ڲ���׼��ѹ
    AMX0SL=0x00;     // ADC0ʹ��"0"ͨ��
	  AMX0CF=0x00;
    ADC0CF=31<<3;         // ADCת��ʱ��= 2.5MHz
    ADC0CF&=0xf8;          // PGA gain = 1
    EIE2|=0x02;        // ��ADC0�ж�
}
