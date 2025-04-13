 #include <stdio.h>
#include "c8051f020.h"
#include "lcd.h"
#define SYSCLK    11059200
uchar code str0[]="西安交通大学";
uchar code str1[]="电信学部";
uchar code str2[]="自动化学院";
uchar code str3[]="****";
void Oscillator_Init()
{
    int i = 0;
    OSCXCN    = 0x67;
    for (i = 0; i < 3000; i++);  // Wait 1ms for initialization
    while ((OSCXCN & 0x80) == 0);
    OSCICN    = 0x08;
}


void Init_Device(void)
{
    //disable watchdog
    EA = 0;
    WDTCN = 0xde;
    WDTCN = 0xad;
    EA = 1;
	
    Oscillator_Init();
    LcdInit();

}


//-----------------------------------------------------------------------------
// 主程序
//-----------------------------------------------------------------------------

void main()
{	
  Init_Device();
	WriteStr(0, 0, str0);
  WriteStr(1, 0, str1);
  WriteStr(2, 0, str2);
	WriteStr(3, 0, str3);
    while(1); 
}



