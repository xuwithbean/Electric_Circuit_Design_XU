#include "sysclk.h"

void SYSCLK_Init (void)
{
   int i;                      
   OSCXCN = 0x67;              // 选择晶体振荡器，板载晶振为11.0592
   for (i=0; i<256; i++);      // 延时让晶振稳定
   while (!(OSCXCN & 0x80));   // 提取最高位，判断晶振是否稳定
   OSCICN = 0x88;              // 选择外部晶振作为时钟
}

void Delay_us(unsigned int times)//这是一个延时函数
{
    unsigned int i;

    for (i=0; i<times; i++)
    {
        _nop_();
        _nop_();
        _nop_();
		    _nop_();
    }
}
