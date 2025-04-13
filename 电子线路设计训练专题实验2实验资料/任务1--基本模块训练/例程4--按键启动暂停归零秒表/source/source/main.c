 #include <stdio.h>

#include "c8051f020.h"
#include "led.h"


#define SYSCLK    11059200

#define TIMER_PRESCALER            12  // Based on Timer CKCON settings

#define LED_TOGGLE_RATE            100 // LED toggle rate in milliseconds    1ms
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

sbit INT1 = P0^3; //旧板子P0.2端口

unsigned int Time_num; //全局计时用


void Port_IO_Init()
{
	//set P4[1] to push_pull model
    P74OUT |= 0x01;

    //close the alam P4.1
    P4 &= 0xfd;
	
	//uart1,int1
	XBR1 |= 0x14;//旧板子0x10
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

void Timer0_Init(void)
{
	TH0 = TIMER0_RELOAD_HIGH;           // Reinit Timer0 High register
	TL0 = TIMER0_RELOAD_LOW;
	ET0 = 1;                            // Timer0 interrupt enabled
	TMOD = 0x01;                        // 16-bit Mode Timer0
	//TCON |= 0x10;                        // Timer0 ON
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
	Timer0_Init();
	Interrupt_Init();
	LedInit();
}


//-----------------------------------------------------------------------------
// 主程序
//-----------------------------------------------------------------------------

void main()
{	
  Init_Device();
	
	INT1 = 1;

    while(1)
    {
		LedDispNum();   //显示三行数据
    }
}

void INT1_ISR(void) interrupt 2
{
	Delay(1);
	switch(P5)
	{
		//sub
			case 0xfb:   //左键
			  Time_num = 0; //计时清零	
			break;	
		
		 case 0xfd:   //中间键
				TR0 = 1;  // 开始计时
			break;
		
		 case 0xfe:    //右键
        TR0 = 0;  // 关闭计时
			break;
	
	}
}

void Timer0_ISR (void) interrupt 1
{
	TH0 = TIMER0_RELOAD_HIGH;           // Reinit Timer0 High register
	TL0 = TIMER0_RELOAD_LOW;
	
	Time_num++;
	
	if(Time_num >= 6000) Time_num = 0;
}




