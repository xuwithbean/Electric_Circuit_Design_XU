#include "sysclk.h"
#include "dight.h"

uchar code tab[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x7f};//0��1��2��3��4��5��6,7��8��9��.

void up_dight(int num)//��ʾ������������
{
	int t_num = num/1000;
	int h_num = (num - t_num * 1000)/100;
  int ten_num = (num - t_num*1000 - h_num*100)/10;
	num = num - t_num*1000 - h_num*100 - ten_num*10;
	P5 = 0XF7;//��һ��һ��ʾ
	P6 = 0XFF;
	P7 = tab[num];
	Delay_us(500);
  P5 = 0XFB;//��һ�Ҷ�
	P6 = 0XFF;
	P7 = tab[ten_num];	      		
	Delay_us(500);
  P5 = 0XFD;//��һ����
	P6 = 0XFF;
	P7 = tab[h_num];
	Delay_us(500);	
	P5 = 0XFE;//��һ����
	P6 = 0XFF;
	P7 = tab[t_num];
	Delay_us(500);
}

void meind_dight(int num)//��ʾ�м�������
{
	int t_num = num/1000;
	int h_num = (num - t_num * 1000)/100;
  int ten_num = (num - t_num*1000 - h_num*100)/10;
	num = num - t_num*1000 - h_num*100 - ten_num*10;
	P5 = 0X7F;//����һ
	P6 = 0XFF;
	P7 = tab[num];
	Delay_us(500);
	P5 = 0XBF;//���Ҷ�
	P6 = 0XFF;
	P7 = tab[ten_num];
	Delay_us(500);
	P5 = 0XDF;//������
	P6 = 0XFF;
	P7 = tab[h_num];
	Delay_us(500);
	P5 = 0XEF;//������
	P6 = 0XFF;
	P7 = tab[t_num];
	Delay_us(500);
}

void down_dight(int num)//��ʾ������������
{
	int t_num = num/1000;
	int h_num = (num - t_num * 1000)/100;
  int ten_num = (num - t_num*1000 - h_num*100)/10;
	num = num - t_num*1000 - h_num*100 - ten_num*10;
	P5 = 0XFF;//����һ
	P6 = 0XFE;
	P7 = tab[t_num];
	Delay_us(500);		
	P5 = 0XFF;//�����
	P6 = 0XFD;
	P7 = tab[h_num];
	Delay_us(500);
	P5 = 0XFF;//������
	P6 = 0XFB;
	P7 = tab[ten_num];
	Delay_us(500);
	P5 = 0XFF;//������
	P6 = 0XF7;
	P7 = tab[num];
	Delay_us(500);
}
