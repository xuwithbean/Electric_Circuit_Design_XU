#include <stdio.h>
#include <string.h>

#include "c8051f020.h"
#include "lcd.h"
//#include "led.h"
#include "xjtu_image.h"
//#include "111.h"
//#include "222.h"
//#include "xjtu_image.h"

#define SYSCLK    11059200

#define TIMER_PRESCALER            12  // Based on Timer CKCON settings

#define LED_TOGGLE_RATE            100 // LED toggle rate in milliseconds
                                       // if LED_TOGGLE_RATE = 1, the LED will
                                       // be on for 1 millisecond and off for
                                       // 1 millisecond

// There are SYSCLK/TIMER_PRESCALER timer ticks per second, so
// SYSCLK/TIMER_PRESCALER/1000 timer ticks per millisecond.
#define TIMER_TICKS_PER_MS  SYSCLK/TIMER_PRESCALER/1000

// Note: LED_TOGGLE_RATE*TIMER_TICKS_PER_MS should not exceed 65535 (0xFFFF)
// for the 16-bit timer

#define AUX1     TIMER_TICKS_PER_MS*LED_TOGGLE_RATE
#define AUX2     -AUX1
#define AUX3     AUX2&0x00FF
#define AUX4     ((AUX2&0xFF00)>>8)

#define TIMER0_RELOAD_HIGH       AUX4  // Reload value for Timer0 high byte
#define TIMER0_RELOAD_LOW        AUX3  // Reload value for Timer0 low byte


sbit INT1 = P0^3;
uchar code str0[]="西安交通大学";
uchar code str1[]="左键Triangular";
uchar code str2[]="中间键方波";
uchar code str3[]="右键sin";
//global variable
//char buffer[17];
void Timer2_Init (void);//定时器2初始化
void DAC0_Init(void);//DA初始化
long  DAC0_result[1];
void Delay_us(unsigned int times)
{
	unsigned int i,j;
	
	for (i = 0; i< times; i++)
	{
		for (j = 0; j < 11; j++)
		{
			_nop_();
    }
  }
}
void Port_IO_Init()
{
	//set P4[1] to push_pull model
    P74OUT |= 0x01;
	
	//enable crossbar
    XBR2 |= 0x40;

    //close the alam P4.1
    P4 &= 0xfd;
	
	//uart1,int1
	XBR1 |= 0x14;
	XBR2 |= 0x44;
}

void Oscillator_Init()
{
    int i = 0;
    OSCXCN    = 0x67;
    for (i = 0; i < 3000; i++);  // Wait 1ms for initialization
    while ((OSCXCN & 0x80) == 0);
    OSCICN    = 0x08;
}
void Interrupt_Init()
{
	//INT1
	//low level triggle
	IT1 = 0;
	//enable INT1
	EX1 = 1;
	//enable all interrupt
	EA = 1;
}

void Init_Device(void)
{
    //disable watchdog
    EA = 0;
    WDTCN = 0xde;
    WDTCN = 0xad;
    EA = 1;
	
    Oscillator_Init();
    Port_IO_Init();
//	Timer0_Init();
	
	Interrupt_Init();
	
    LcdInit();
//	LedInit();
}


//-----------------------------------------------------------------------------
// 主程序
//-----------------------------------------------------------------------------
uchar  waveflag=0;
unsigned int vvalue=0;
code unsigned int sinn[64]={2048,2252,2454,2653,2845,3029,3204,3367,
	                          3517,3652,3771,3873,3956,4021,4066,4090,
	                          4095,4078,4042,3986,3910,3815,3703,3575,
	                          3431,3273,3103,2923,2734,2537,2337,2133,
	                          1929,1725,1525,1330,1142,963,795,640,499,
	                          373,264,172,100,47,13,1,8,36,85,152,239,
                            344,466,604,756,921,1097,1283,1476,1675,
                            1878,2082};
bit ifstart=0;

uchar column=0;
uchar Position=0;
void main()
{	
    Init_Device(); 
  Timer2_Init ();
  DAC0_Init();
//  EX1=1;
//  EA=1;
  INT1=1;
  DAC0_result[0]=0;
  ClearAll();
	WriteStr(0, 0, str0);
  WriteStr(1, 0, str1);
  WriteStr(2, 0, str2);
	WriteStr(3, 0, str3);
    while(1)
    {
	   	P5 = 0xFB;Delay_us(5);
			P5 = 0xFD;Delay_us(5);
			P5 = 0xFE;Delay_us(5);
 }
}

void INT1_ISR(void) interrupt 2
{
	switch (P5)
		{case 0xfb:
			 LcdClear();
		    waveflag=1;
		    break;
    		case 0xfd:
				 LcdClear();	
			DAC0_result[0]=0;
		    waveflag=2;
    		break;
    		case 0xfe:
				 LcdClear();	
		    waveflag=3;
    		break;
			default:break;
		}
}

//---------------------------------------------
//DAC0初始化
//---------------------------------------------

void DAC0_Init(void)
{
    REF0CN = 0x03;                   // 使用内部VREF发生器
    DAC0CN = 0x98;
	DAC0L=0x00;
	DAC0H=0x00;                   // 打开DAC0，右对齐模式
}

//---------------------------------------------
//定时器2初始化
//为DA0转换提供
//---------------------------------------------

void Timer2_Init (void)
{
    RCAP2H=0x10;
    RCAP2L=0x00;
    TH2=0x10;
    TL2=0x00;
    T2CON=0x04;
    IE|= 0x20;
}
//----------------------------------------------
// 定时器2中断服务
//----------------------------------------------

int placelast=0;

void Time2_ISR(void) interrupt 5 
{
    unsigned int xdata temp;
    vvalue=(vvalue+1)%64;
	switch(waveflag)
	{
	case 1:DAC0_result[0]=vvalue*64;break;
	case 2:if(vvalue==0)DAC0_result[0]=4095-DAC0_result[0];break;
	case 3:DAC0_result[0]=sinn[vvalue];break;
	default:DAC0_result[0]=0;break; 
	}

    TF2=0;
    temp=DAC0_result[0];
    temp&=0x00ff;
    DAC0L=temp;
    temp=DAC0_result[0];
    temp>>=8;
    DAC0H=temp;

	if(waveflag)
	{
	    int i,place=DAC0_result[0]/128;
	    int dir=(place>placelast)?1:-1;
	    if(column==0&&Position==0)ClearAll();
		for(i=placelast;i!=place;i+=dir){
		   if(column<8){
		   toshow[62-i*2]|=(0x80>>column);
		   }
		   else	
		     {toshow[62-i*2+1]|=(0x80>>(column-8));}
		        WriteCommand(0x34);
				WriteCommand(0x80+31-i);//row :0~32
				WriteCommand(0x80+Position);//column	:80~87uphalf;88~8fdownhalf
				WriteCommand(0x30);
				WriteData(toshow[62-i*2]);
				WriteData(toshow[62-i*2+1]);
		}
		if(column==0)memset(toshow,0,sizeof(uchar)*64);
		if(column<8){toshow[62-place*2]|=(0x80>>column);}
		else {toshow[62-place*2+1]|=(0x80>>(column-8));}
		        

				WriteCommand(0x32);
	            WriteCommand(0x36);
		column++;
		if(column==16){column=0;Position=(Position+1)%8;}
		placelast=place;
	}
}