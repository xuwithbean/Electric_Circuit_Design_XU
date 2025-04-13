#include "lcd.h"
#include <intrins.h>

//constant definition
uchar code AC_TABLE[]=
{
    0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,      //��һ�к���λ��
    0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,      //�ڶ��к���λ��
    0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,      //�����к���λ��
    0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,      //�����к���λ��
};

sbit SCLK=P1^3;
sbit SID=P1^4;
sbit CS=P1^5;
void Lcd_Port_Init(void)
{
	//set P1[5:3] to push_pull model
    P1MDOUT |= 0x38;
	
	//enable crossbar
    XBR2 |= 0x40;
}


void LcdInit( void )
{
    Lcd_Port_Init();

    WriteCommand(0x30);      //8BitMCU
    WriteCommand(0x03);      //AC?0,���ı�DDRAMֵ
    WriteCommand(0x0C);      //��ʾON,�α�OFF,�α�λ����OFF
    WriteCommand(0x01);      //����,AC��0
    WriteCommand(0x06);      //д��ʱ,�α����ƶ�

    LcdClear();
}


void Delay_ms(uint times)
{
    uint i,j;
    for (i=0; i<times; i++)
        for (j=0; j<1300; j++);
}

void Delay(uint k)
{
    do
    {
    }
    while( --k );
}


void SendByte(uchar Dbyte)
{
    uchar i;
    for(i=0; i<8; i++)
    {
        SCLK = 0;
        Dbyte=Dbyte<<1;      //����һλ
        SID = CY;            //�Ƴ���λ��SID
        SCLK = 1;
        SCLK = 0;
    }
}

uchar ReceiveByte(void)
{
    uchar i, temp1, temp2, temp3;
    temp1 = 0;
    temp2 = 0;
    for(i=0; i<8; i++)
    {
        temp1=temp1<<1;
        SCLK = 0;
        SCLK = 1;
        SCLK = 0;
        if(SID) temp1++;
    }
    for(i=0; i<8; i++)
    {
        temp2=temp2<<1;
        SCLK = 0;
        SCLK = 1;
        SCLK = 0;
        if(SID) temp2++;
    }
	temp3 = (0xf0&temp1)+(0x0f&temp2);
    return (temp3);
}

void CheckBusy( void )
{
    do   SendByte(0xfc);      //11111,RW(1),RS(0),0
    while(0x80&ReceiveByte());      //BF(.7)=1 Busy
}

void WriteCommand(unsigned char Cbyte )
{
    CS = 1;
    CheckBusy();
    SendByte(0xf8);            //11111,RW(0),RS(0),0
    SendByte(0xf0&Cbyte);      //����λ
    SendByte(0xf0&Cbyte<<4);//����λ
    CS = 0;
}

void WriteData(uchar Dbyte )
{
    CS = 1;
    CheckBusy();
    SendByte(0xfa);            //11111,RW(0),RS(1),0
    SendByte(0xf0&Dbyte);      //����λ
    SendByte(0xf0&Dbyte<<4);//����λ(��ִ��<<)
    CS = 0;
}


void LineClear(uchar line)
{

    uchar i;
    WriteCommand(0x30);      //8BitMCU
    WriteCommand(AC_TABLE[8*line]);      //��ʼλ��

    for(i=0; i<16; i++)
    {
        WriteData(0x20);//�ո�
    }
}


void LcdClear( void )
{
    uchar i;
    for(i=0; i<4; i++)
    {
        LineClear(i);
    }
}

void WriteChar(uchar row,uchar col,uchar ch)
{
	WriteCommand(0x30);      //8BitMCU
	WriteCommand(AC_TABLE[8*row+col]);      //��ʼλ��
	WriteData(ch);
}

void WriteStr(uchar row,uchar col,uchar *puts)
{
    WriteCommand(0x30);      //8BitMCU
    WriteCommand(AC_TABLE[8*row+col]);      //��ʼλ��
    while(*puts !='\0')      //�ж��ַ����Ƿ���ʾ���
    {
        if(col==8)            //�жϻ���
        {
            //�����ж�,���Զ��ӵ�һ�е�������
            col=0;
            row++;
        }
        if(row==4) row=0;      //һ����ʾ��,�ص������Ͻ�
        WriteCommand(AC_TABLE[8*row+col]);
        WriteData(*puts);      //һ������Ҫд����
        puts++;
        if(*puts == '\0')
        {
            return;
        }
        WriteData(*puts);
        puts++;
        col++;
    }
}

