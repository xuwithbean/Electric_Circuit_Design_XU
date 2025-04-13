#include "sysclk.h"
#include "dight.h"

uchar code tab[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x7f};//0，1，2，3，4，5，6,7，8，9，.

void up_dight(int num)//显示最上面的数码管
{
	int t_num = num/1000;
	int h_num = (num - t_num * 1000)/100;
  int ten_num = (num - t_num*1000 - h_num*100)/10;
	num = num - t_num*1000 - h_num*100 - ten_num*10;
	P5 = 0XF7;//上一右一显示
	P6 = 0XFF;
	P7 = tab[num];
	Delay_us(500);
  P5 = 0XFB;//上一右二
	P6 = 0XFF;
	P7 = tab[ten_num];	      		
	Delay_us(500);
  P5 = 0XFD;//上一右三
	P6 = 0XFF;
	P7 = tab[h_num];
	Delay_us(500);	
	P5 = 0XFE;//上一右四
	P6 = 0XFF;
	P7 = tab[t_num];
	Delay_us(500);
}

void meind_dight(int num)//显示中间的数码管
{
	int t_num = num/1000;
	int h_num = (num - t_num * 1000)/100;
  int ten_num = (num - t_num*1000 - h_num*100)/10;
	num = num - t_num*1000 - h_num*100 - ten_num*10;
	P5 = 0X7F;//中右一
	P6 = 0XFF;
	P7 = tab[num];
	Delay_us(500);
	P5 = 0XBF;//中右二
	P6 = 0XFF;
	P7 = tab[ten_num];
	Delay_us(500);
	P5 = 0XDF;//中右三
	P6 = 0XFF;
	P7 = tab[h_num];
	Delay_us(500);
	P5 = 0XEF;//中右四
	P6 = 0XFF;
	P7 = tab[t_num];
	Delay_us(500);
}

void down_dight(int num)//显示最下面的数码管
{
	int t_num = num/1000;
	int h_num = (num - t_num * 1000)/100;
  int ten_num = (num - t_num*1000 - h_num*100)/10;
	num = num - t_num*1000 - h_num*100 - ten_num*10;
	P5 = 0XFF;//下左一
	P6 = 0XFE;
	P7 = tab[t_num];
	Delay_us(500);		
	P5 = 0XFF;//下左二
	P6 = 0XFD;
	P7 = tab[h_num];
	Delay_us(500);
	P5 = 0XFF;//下左三
	P6 = 0XFB;
	P7 = tab[ten_num];
	Delay_us(500);
	P5 = 0XFF;//下左四
	P6 = 0XF7;
	P7 = tab[num];
	Delay_us(500);
}
