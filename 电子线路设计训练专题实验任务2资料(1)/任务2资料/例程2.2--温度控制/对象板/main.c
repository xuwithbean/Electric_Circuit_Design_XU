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

uchar code str0[]="自动化钱91徐彬琰";
uchar code str1[]="温度控制系统";
uchar code str4[]="摄氏度";
uchar zkbtemp[10];
//uchar code str2[]="学号：2150504124";
//uchar code str3[]="0123456789ABCDEF";

unsigned int Time_num;
int zkb;//占空比
int temp;//温度

//dac初始化
void dac0_init(void)
{
	DAC0CN = 0x80;//使能dac，dac更新在写dac0h，数据格式为正常数据格式
	REF0CN |= 0x03;//打开ADC，dac偏压发生器，基准电压由vref提供
}

//pac初始化
void PCA0_initial(void)
{
	PCA0CPM0=0xc3;//16位pwm，允许比较器，使能pwm，允许中断
	PCA0CPL0=0x80;
	PCA0CPH0=0x7f;//捕捉比较模块定初值，先写低位
	PCA0L=0x00;
	PCA0H=0x00;   //定时器定初值
	PCA0MD=0x01;  //允许中断请求
	PCA0CN=0x41;  //允许定时器模块
}

//所有的初始化函数
void Init_Device(void)
{
	  EA = 0;//关闭总中断
    WDTCN = 0xde;
    WDTCN = 0xad;//关闭看门狗
    //EA = 1;//打开总中断
	  SYSCLK_Init();//时钟初始化
	  Timer0_Init();//计时器0初始化
		Port_IO_Init();//端口初始化
	  timer3_init();//定时器3初始化
		ADC0_Init();//ADC0初始化
		dac0_init();//dac0初始化
	  PCA0_initial();//PCA0初始化
		LcdInit();
		CCF0=0;//手动清理PAC0溢出位
    //EA=1;//打开总中断
	  EIE1=0x08;//允许PAC0中断请求
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
	Init_Device();//初始化
	PutStr(0, 0, str0);
  PutStr(1, 0, str1);
  //PutStr(2, 0, str2);
	//PutStr(3, 0, str3);
	EA=1;
	while(1)
	{
	}
}

//pac0溢出中断函数
void A(void) interrupt 9
{
	int j_zkb = zkb;						
	//down_dight(j_zkb);20
	nub_lcd(temp/77+2);			//原：nub_lcd((temp/77-9))
	j_zkb = 65536-65536 * j_zkb/100;//计算占空比 
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
	PCA0CPH0 = j_zkb >> 8;//捕捉比较模块值，先写低位
	CCF0=0;//手动清理PAC0溢出位
	CF=0;//手动清0
}
