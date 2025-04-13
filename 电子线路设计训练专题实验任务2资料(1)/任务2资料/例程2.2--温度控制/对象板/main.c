/************************************************
designed by wjj
wjj12138@stu.xjtu.edu.cn
************************************************/
#include "sysclk.h"
#include "time.h"
#include "port.h"
#include "adc.h"
#include "dight.h"
#include "lcd.h"
#include <stdio.h>

uchar code str0[]="�Զ���Ǯ91�����";
uchar code str1[]="�¶ȿ���ϵͳ";
uchar code str4[]="���϶�";
uchar zkbtemp[10];
//uchar code str2[]="ѧ�ţ�2150504124";
//uchar code str3[]="0123456789ABCDEF";

unsigned int Time_num;
int zkb;//ռ�ձ�
int temp;//�¶�

//dac��ʼ��
void dac0_init(void)
{
	DAC0CN = 0x80;//ʹ��dac��dac������дdac0h�����ݸ�ʽΪ�������ݸ�ʽ
	REF0CN |= 0x03;//��ADC��dacƫѹ����������׼��ѹ��vref�ṩ
}

//pac��ʼ��
void PCA0_initial(void)
{
	PCA0CPM0=0xc3;//16λpwm������Ƚ�����ʹ��pwm�������ж�
	PCA0CPL0=0x80;
	PCA0CPH0=0x7f;//��׽�Ƚ�ģ�鶨��ֵ����д��λ
	PCA0L=0x00;
	PCA0H=0x00;   //��ʱ������ֵ
	PCA0MD=0x01;  //�����ж�����
	PCA0CN=0x41;  //����ʱ��ģ��
}

//���еĳ�ʼ������
void Init_Device(void)
{
	  EA = 0;//�ر����ж�
    WDTCN = 0xde;
    WDTCN = 0xad;//�رտ��Ź�
    //EA = 1;//�����ж�
	  SYSCLK_Init();//ʱ�ӳ�ʼ��
	  Timer0_Init();//��ʱ��0��ʼ��
		Port_IO_Init();//�˿ڳ�ʼ��
	  timer3_init();//��ʱ��3��ʼ��
		ADC0_Init();//ADC0��ʼ��
		dac0_init();//dac0��ʼ��
	  PCA0_initial();//PCA0��ʼ��
		LcdInit();
		CCF0=0;//�ֶ�����PAC0���λ
    //EA=1;//�����ж�
	  EIE1=0x08;//����PAC0�ж�����
}

void nub_lcd(int num)
{
	char s[3];
	s[0] = num/10 + 48;
	s[1] = num%10 + 48;
	s[3] = '\0';
	PutStr(2, 0, s);
	PutStr(2, 1, str4);
	//PutStr(2, 3, str4);
	//PutStr(2, 4, str4);
}

void main(void)
{
	Init_Device();//��ʼ��
	PutStr(0, 0, str0);
  PutStr(1, 0, str1);
  //PutStr(2, 0, str2);
	//PutStr(3, 0, str3);
	EA=1;
	while(1)
	{
	}
}

//pac0����жϺ���
void A(void) interrupt 9
{
	int j_zkb = zkb;						
	//down_dight(j_zkb);20
	nub_lcd(temp/77+2);			//ԭ��nub_lcd((temp/77-9))
	j_zkb = 65536-65536 * j_zkb/100;//����ռ�ձ� 
	sprintf(zkbtemp, "%d" , j_zkb);
	//PutStr(3,0,zkbtemp);
	if(j_zkb > -15000)
	{
		j_zkb = 100;
	}
	if(j_zkb < -19000)
	{
		j_zkb = 0;
	}
	PCA0CPL0 = j_zkb & (0x00ff);//
	PCA0CPH0 = j_zkb >> 8;//��׽�Ƚ�ģ��ֵ����д��λ
	CCF0=0;//�ֶ�����PAC0���λ
	CF=0;//�ֶ���0
}
