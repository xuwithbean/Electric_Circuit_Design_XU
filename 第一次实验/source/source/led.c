#include "led.h"
/*select�Լ�display���������е�һ����ѡ������ܣ����ϵ��´�����1-12���ڶ�������ʾ����*/

extern int xdata v[10];
extern int xdata u[10];
extern long ADC0_result[2];
extern int w1;
extern int x1;
extern int y1;
extern int z1;
extern int w2;
extern int x2;
extern int y2;
extern int z2;
extern int result;
extern long xdata result2;
extern int w3;
extern int x3;
extern int y3;
extern int z3;
extern volatile unsigned char idata samplepos;
extern volatile long idata slidesum;

void display(int a)  // ��ʾ����
{
	switch(a)
	{
		case 0:P7=0xC0;break;
		case 1:P7=0xF9;break;
		case 2:P7=0xA4;break;
		case 3:P7=0xB0;break;
		case 4:P7=0x99;break;
		case 5:P7=0x92;break;
		case 6:P7=0x82;break;
		case 7:P7=0xF8;break;
		case 8:P7=0x80;break;
		case 9:P7=0x90;break;
		case 10:P7=0x88;break;//A
		case 11:P7=0x83;break;
		case 12:P7=0xC6;break;
		case 13:P7=0xA1;break;
		case 14:P7=0x86;break;
		case 15:P7=0x8e;break;
		default:P7=0xc0;break;
	}
}

void select(int b)  // ѡ�������
{
	switch(b)
	{			
		case 1: P5 = 0XFE;P6 = 0XFF;break;
		case 2: P5 = 0XFD;P6 = 0XFF;break;
		case 3: P5 = 0XFB;P6 = 0XFF;break;
		case 4: P5 = 0XF7;P6 = 0XFF;break;
		case 5: P5 = 0XEF;P6 = 0XFF;break;
		case 6: P5 = 0XDF;P6 = 0XFF;break;
		case 7: P5 = 0XBF;P6 = 0XFF;break;
		case 8: P5 = 0X77;P6 = 0XFF;break;
		case 9: P6 = 0XFE;P5 = 0XFF;break;
		case 10:P6 = 0XFD;P5 = 0XFF;break;
		case 11:P6 = 0XFB;P5 = 0XFF;break;
		case 12:P6 = 0XF7;P5 = 0XFF;break;
	}
}

void Led_Port_Init()
{
    //set to push_pull model
    P74OUT &= 0xff;

    //enable crossbar
    XBR2 |= 0x40;
}

void LedInit()
{
	Led_Port_Init();
	P5 |= 0xff;
	P6 |= 0x0f;
	P7 = 0xff;
}

void LedDispNum()  //�������ʾ����
{
	uchar temp[4];
	
	temp[0] = Time_num%10;// ��ĸ�λ
	temp[1] = Time_num%100/10;
	temp[2] = Time_num%1000/100;  
	temp[3] = Time_num/1000;

	// ���ϱ�һ����ʾ
	select(4);display(mima1[3]); Delay(500); P7 = 0xff;
	select(3);display(mima1[2]); Delay(500); P7 = 0xff;
	select(2);display(mima1[1]); Delay(500); P7 = 0xff;            //��С����
	select(1);display(mima1[0]); Delay(500); P7 = 0xff; //����

	// �м�һ��
	select(8);display(5); Delay(500); P7 = 0xff;
	select(7);display(2); Delay(500); P7 = 0xff;
	select(6);display(0); Delay(500); P7 = 0xff;
	select(5);display(2); Delay(500); P7 = 0xff;  

	// �±�һ��
	select(12);display(4); Delay(500); P7 = 0xff;
	select(11);display(2); Delay(500); P7 = 0xff;
	select(10);display(2); Delay(500); P7 = 0xff;
	select(9) ;display(0); Delay(500); P7 = 0xff;  
}




void show_f_row(void)
{
	select(1);display(w1);P7 = P7 & ~0x80;Delay(500); P7 = 0xff;
	select(2);display(x1);Delay(500); P7 = 0xff;
	select(3);display(y1);Delay(500); P7 = 0xff;
	select(4);display(z1);Delay(500); P7 = 0xff;
}
void show_s_row(void)
{
	select(5);display(w2);P7 = P7 & ~0x80;Delay(500); P7 = 0xff;
	select(6);display(x2);Delay(500); P7 = 0xff;
	select(7);display(y2);Delay(500); P7 = 0xff;
	select(8);display(z2);Delay(500); P7 = 0xff;
}
void show_t_row(void)
{
	select(9);display(w3);P7 = P7 & ~0x80;Delay(500); P7 = 0xff;
	select(10);display(x3);Delay(500); P7 = 0xff;
	select(11);display(y3);Delay(500); P7 = 0xff;
	select(12);display(z3);Delay(500); P7 = 0xff;
}
void show_mima_row(int * codenum1234)
{
	select(1);display(codenum1234[0]);Delay(500); P7 = 0xff;
	select(2);display(codenum1234[1]);Delay(500); P7 = 0xff;
	select(3);display(codenum1234[2]);Delay(500); P7 = 0xff;
	select(4);display(codenum1234[3]);Delay(500); P7 = 0xff;
}