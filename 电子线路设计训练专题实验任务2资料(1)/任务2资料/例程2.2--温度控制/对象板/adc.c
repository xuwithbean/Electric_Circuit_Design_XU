/************************************************
designed by wjj
wjj12138@stu.xjtu.edu.cn
************************************************/
#include "adc.h"
#include "dight.h"

//adc0��ʼ��
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

//timer3��ʼ��
void timer3_init()
{
	TMR3CN = 0x00;//�ֶ���������־λ
	TMR3RLL = 0xdc;//100hz
	//TMR3RLL = 0xfc;
	TMR3RLH = 0Xff;
	TMR3L = 0Xff;//
	TMR3H = 0Xff;//ֱ�����
	EIE2 |= 0x01;//����timer3�ж�
	TMR3CN |= 0x04;//����ʱ��3
}

//adc0����жϷ�����
//ADC0�ж��ͨ����ÿ��ͨ���Ͽ������Ӳ�ͬ�����ݣ�ͬһʱ��ֻ��һ��ͨ���ᱻ�򿪣�֮���ȡ���ݣ�ͨ��������Ҫʱ��
void ADC0_ISR () interrupt 15
{
		/*int adc_re = ADC0H;
		adc_re = (adc_re <<8)|(ADC0L);
		zkb = adc_re;
		zkb = (zkb-770)/31;*/
	
		static int ad_e = 0;
		int adc_re = ADC0H;
		adc_re = (adc_re <<8)|(ADC0L);//����adc0����ֵ
		if(ad_e == 0)
		{
			//AMX0SL=0x00;//�����������ʾ1
			zkb = adc_re;
			//zkb = (zkb-770)/31;//��ʾ0ͨ������
			zkb = zkb / 33;
			up_dight(zkb);
			AMX0SL=0x00;//�ı�ͨ��
			ad_e = 1;
		}else
		{
			//AMX0SL=0x01;//�����������ʾ0
			temp = adc_re;//��ʾ1ͨ������
			meind_dight(temp);
			AMX0SL=0x01;//�ı�ͨ��
			ad_e = 0;
		}
}