#include "time.h"

sfr16 RCAP4 = 0xE4;               
sfr16 TMR4 = 0xF4; 

//��ʱ��3��ʼ��
void timer3_init()
{
	TMR3CN = 0x00;//�ֶ���������־λ
	TMR3RLL = 0xdc;//100hz
	//TMR3RLL = 0xfc;
	TMR3RLH = 0Xef;
	TMR3L = 0Xff;
	TMR3H = 0Xff; //ֱ�����
	EIE2 |= 0x01;//����timer3�ж�
	TMR3CN |= 0x04;//����ʱ��3
	EIE2&=0xFE;
}

//��ʱ��4��ʼ��
void TIMER4_Init (int counts)
{   
   T4CON = 0x00;    //�ֶ���������־λ
   CKCON |= 0x40;   // ��ʱ��ʹ��ϵͳʱ��
   RCAP4 = -counts; //��������ֵ
   TMR4 = RCAP4;// ��ʼ����ʱ��4
   EIE2 |= 0x04;  // ʹ�ܶ�ʱ��4�ж�
   T4CON |= 0x04;  // �򿪶�ʱ��4

}

