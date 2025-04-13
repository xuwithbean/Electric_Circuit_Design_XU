 #include <stdio.h>
#include "led.h"

void Delay(uint k)
{
   do
    {
    }
    while( --k );
}
/*select�Լ�display���������е�һ����ѡ������ܣ����ϵ��´�����1-12���ڶ�������ʾ����*/

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
		case 1: P5 = 0XFE;P6 = 0XFF;break;//F5 11111110
		case 2: P5 = 0XFD;P6 = 0XFF;break;//F5 11111101
		case 3: P5 = 0XFB;P6 = 0XFF;break;//F5 11111011
		case 4: P5 = 0XF7;P6 = 0XFF;break;//F5 11110111
		case 5: P5 = 0XEF;P6 = 0XFF;break;//F5 11101111
		case 6: P5 = 0XDF;P6 = 0XFF;break;//F5 11011111
		case 7: P5 = 0XBF;P6 = 0XFF;break;//F5 10111111
		case 8: P5 = 0X7F;P6 = 0XFF;break;//F5 01111111
		
		case 9: P6 = 0XFE;P5 = 0XFF;break;//F6 11111110
		case 10:P6 = 0XFD;P5 = 0XFF;break;//F6 11111101
		case 11:P6 = 0XFB;P5 = 0XFF;break;//F6 11111011
		case 12:P6 = 0XF7;P5 = 0XFF;break;//F6 11110111
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
	
	temp[0] = Time_num%10;
	temp[1] = Time_num%100/10;
	temp[2] = Time_num%1000/100;  // ��ĸ�λ
	temp[3] = Time_num/1000;

	// ���ϱ�һ����ʾ
	select(4);display(temp[0]); Delay(500); P7 = 0xff;
	select(3);display(temp[1]); Delay(500); P7 = 0xff;
	select(2);display(temp[2]); P7 = P7 & ~0x80; Delay(500); P7 = 0xff;            //��С����
	select(1);display(temp[3]); if(temp[3] == 0) P7 = 0xff; Delay(500); P7 = 0xff; //����

	// �м�һ��
	select(8);display(4); Delay(500); P7 = 0xff;
	select(7);display(2); P7 = P7; Delay(500); P7 = 0xff;
	select(6);display(0); Delay(500); P7 = 0xff;
	select(5);display(2); Delay(500); P7 = 0xff; // Delay(500);

	// �±�һ��
	select(12);display(3); Delay(500); P7 = 0xff;
	select(11);display(0);  Delay(500); P7 = 0xff;
	select(10);display(4); Delay(500); P7 = 0xff;
	select(9) ;display(0); Delay(500); P7 = 0xff; // Delay(500);
}