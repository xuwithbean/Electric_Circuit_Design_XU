// 字符编码使用 UTF-8
#include "ADC.h"
#include "DAC.h"
#include "Delay.h"
#include "SYS_Init.h"
#include "Timer.h"
#include "lcd.h"
// #include "Handle_Menu.h"
#include "c8051f020.h"
#include "c8051f020.h"
uchar code str0[]="xjtu";
uchar code str1[]="dianxin";
uchar code str2[]="zidonghua";
uchar code str3[]="****";
// Timers value
unsigned int  timer0_value;
unsigned int  timer1_value;
unsigned int  timer2_value;
unsigned int  timer3_value;
unsigned int  timer4_value;

// ADC & DAC variables
unsigned char channel;   // ADC 通道转换
unsigned int  vref;      // VREF
unsigned int  vtarget;   // VTARGET
unsigned int  vadc;      // ADC 取值
unsigned int  vadc_dec;  // ADC 取值
unsigned int  vdac;      // DAC 输出
unsigned int  vdac_dec;  // DAC 输出

void Timer1_Init_Main(void) {
    Enable_Timer1;       // 设定 IE 标志位 1，允许 Timer1 溢出中断请求
    Timer1_Set_Method1;  // 设置定时器 1 为方式 1 定时器功能
}

void Timer1_ISR(void) interrupt 3 {
    timer1_value++;

    TH1 = 0x00; // Reinit Timer0 High register
    TL1 = 0x00;
}

void Device_Init(void) {
    SYS_Init();  // SYS 初始化

    Timer1_Init_Main();
    Timer3_Init_ADC0(SYSCLK / SAMPLERATE);  // TIMER3 初始化 

    INT_Init();  // INT 中断初始化

    ADC0_Init();     // ADC0 初始化
    ADC0_Enable(1);  // 使能 ADC0
    DAC0_Init();     // DAC 初始化
    LcdInit();
    Timer1_Start;  // 设定 TCON 中断标志位 6，定时器 1 开启

    timer1_value = 0;
    timer3_value = 0;

    channel  = 1;
    vref     = 5244;
    vtarget  = 2800;
    vadc     = 0x0000;
    vadc_dec = 0;
    vdac     = 0x0000;
    vdac_dec = 0;
}

void Do(void) {
    if ((timer1_value & 0x0007) == 0x0001) {     
        if (channel == 1) {
            // 从 ADC0 AIN1 取得 10 位 16 进制数 vadc
            vadc = ADC_ValueReturn(channel);
            // 将 vadc 转化为 10 进制数进行计算
            vadc_dec = (unsigned long int)vadc * (unsigned long int)vref / 4096;

            // 数据处理
            if (vadc_dec < vtarget) {
                vdac_dec = vdac_dec + 2;
            } else if (vadc_dec > vtarget + 20) {
                vdac_dec = vdac_dec - 2;
            } else {
                vdac_dec = vdac_dec;
            }

            // 将 10 进制数 vdac_dec 转化为 16 进制数
            vdac = (unsigned long int)vdac_dec * 4096 / (unsigned long int)vref;
            // 从 ADC0 输出 10 位 16 进制数 vdac
            DAC0_Output(vdac);
        }
    }
}

main(void) {
    Device_Init();
	      Device_Init();
	WriteStr(0, 0, str0);
  WriteStr(1, 0, str1);
  WriteStr(2, 0, str2);
	WriteStr(3, 0, str3);
    while (1) {
    Do();
    }
		WDTCN = 0xde;
    WDTCN = 0xad;//¹Ø±Õ¿´ÃÅ¹·
}