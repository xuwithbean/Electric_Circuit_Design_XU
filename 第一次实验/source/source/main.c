#include "stdio.h"
#include "c8051f020.h"
#include "intrins.h"
#include "lcd.h"
#include "led.h"

#define SYSCLK    11059200
#define SAMPLERATE0    50000
#define RI1  SCON1&0x01

#define	AMX0SL_AIN   8
#define INT_DEC      256
#define SAMPLE_RATE_DAC 100000L        // DAC sampling rate in Hz
#define PHASE_PRECISION 65536          // range of phase accumulator
#define FREQUENCY       1000           // Frequency of output waveform in Hz

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

#define uchar unsigned char
#define uint unsigned int

sfr16 TMR3     = 0x94;
sfr16 ADC0     = 0xbe;
extern void          _nop_     (void);
sbit INT1 = P0^2; //0��ʱ�򰴼�û��

unsigned int PHASE_ADD = FREQUENCY * PHASE_PRECISION / SAMPLE_RATE_DAC;
long ADC0_result[2];		//ADת������
int xdata v[10];
int xdata u[10];
unsigned char count=0,tcount=0;		//������ʱ
int w1,x1,y1,z1;//led��һ����ʾ����
int w2,x2,y2,z2;//led�ڶ�����ʾ����
int w3,x3,y3,z3;//led��������ʾ����
int setting=2300;//�趨���Ƶ�ѹֵ
int result;//������ͼʱ����תֵ��16���ƣ�
long xdata result2;//������ͼʱ����תֵ��10���ƣ�


float kp=1.5;//PID����
float ki=0.05;
float kd=0.45;
int error,derror;//����м���
int preerror=0;
int lasterror=0;
long sumerror=0;//������
int output;//���
int codenum;
int code1234[4]={0,0,0,0};
int mimaflag=0;
int mimacount=0;
int mima1[4]={1,2,3,4};
int mima2[4]={0,0,0,0};
unsigned int channel = 1;
//unsigned char xdata temp[12];//ram����
int SIN_temp,Square_temp,Triangle_temp;
unsigned char idata key_delay;
unsigned int idata lcd_delay;
volatile int xdata wavevalue[128]= {0};
unsigned char idata wavepos=0;
unsigned int idata filtertype=0;
unsigned int idata filtertype2=0;
int idata init=10;
volatile unsigned char idata samplepos=0;
volatile long idata slidesum=0;
unsigned int ledcount=0;//��ʱ��ʾ������
//������ʵ����õı���

unsigned int Time_num; //ȫ�ּ�ʱ��
unsigned int Time_num2; //ȫ�ּ�ʱ��
unsigned int Time_num3; //ȫ�ּ�ʱ��
unsigned int botflag=4;//������־ 0��1��2�ֱ��ʾ���С��ң����귵��4
unsigned int TRflag=0; //TRֵ�ı�־�����ڼ�ʱ����ͣ
unsigned int mainflag=0;//ҳ���־
unsigned int showflag1=0;//ʵ��һ��ʾ��־
unsigned int showflag2=0;//ʵ�����ʾ��־
unsigned int imgflag=0;//ʵ��һͼƬ��־
unsigned int GFlag=0;//ʵ��һgif����

int state = 0;//��־PID �����˵�����״̬�ı���
char s[15] = "               ";
//-----------------------------------------------------------------------------
// 16-bit SFR Definitions for 'F02x
//-----------------------------------------------------------------------------

sfr16 RCAP4    = 0xE4;                 // Timer4 capture/reload
sfr16 TMR4     = 0xF4;                 // Timer4
sfr16 DAC0     = 0xd2;                 // DAC0 data
uchar code SONG_TONE[]={212,212,190,212,159,169,212,212,190,212,142,159,
212,212,106,126,159,169,190,119,119,126,159,142,159,0};
uchar code SONG_LONG[]={9,3,12,12,12,24,9,3,12,12,12,24,
9,3,12,12,12,12,12,9,3,12,12,12,24,0};


void PlayMusic()
{
uint i=0,j,k;
while(SONG_LONG[i]!=0||SONG_TONE[i]!=0){
    for(j=0;j<SONG_LONG[i]*20;j++){
    if(P4==0x00) P4=0xff;
		else P4=0x00;
    for(k=0;k<SONG_TONE[i]/3;k++);
		}
    Delay_ms(10);
    i++;
}
}
void showlock()
{
	ImageShow1(gImage_lock1);
	Delay_ms(1000);
	ImageShow1(gImage_lock2);
	Delay_ms(1000);
	ImageShow1(gImage_lock3);
	Delay_ms(1000);
	ImageShow1(gImage_lock4);
	Delay_ms(1000);
}
void SYSCLK_Init (void)//ϵͳʱ�ӳ�ʼ��
{
    int i;                      // ��ʱ������
    OSCXCN = 0x67;              // ʹ���ⲿ22.1184MHz����
    for (i=0; i<256; i++);      // �ȴ�1ms
    while (!(OSCXCN & 0x80));   // �ȴ��ⲿ�����ȶ�
    OSCICN = 0x88;              // �л����ⲿ����

}
void PORT_Init (void)//�˿ڳ�ʼ��
{
    XBR0     = 0X00;         //����XBR0
    XBR1     = 0X14;         //����XBR1
    XBR2     = 0X44;         //�򿪽��濪�غ�������
    P74OUT  &= 0xcc;
    P2MDOUT |= 0x7F;
    P0MDOUT|= 0x03;
    P1MDOUT=0xFF;			 //�˿�1����Ϊ�������
    P2MDOUT=0xFF;			 //�˿�2����Ϊ�������
    P3MDOUT=0xFF;			 //�˿�3����Ϊ�������
}
void Port_IO_Init()//IO�ڳ�ʼ��
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

void Oscillator_Init()//�����ʼ��
{
    int i = 0;
    OSCXCN    = 0x67;
    for (i = 0; i < 3000; i++);  // Wait 1ms for initialization
    while ((OSCXCN & 0x80) == 0);
    OSCICN    = 0x08;
}

void Timer0_Init(void)//��ʱ��0��ʼ��
{
    TH0 = TIMER0_RELOAD_HIGH;           // Reinit Timer0 High register
    TL0 = TIMER0_RELOAD_LOW;
    ET0 = 1;                            // Timer0 interrupt enabled
    TMOD = 0x01;                        // 16-bit Mode Timer0
    //TCON |= 0x10;                        // Timer0 ON
}

void Timer3_Init (int counts)//��ʱ��3��ʼ��
{
    TMR3CN = 0x02;                      // ֹͣTimer3; ���TF3;
    TMR3RLL=-counts;                   // ��ʼ��װ��ֵ
    TMR3=0xffff;                 // ��������װ��
    EIE2&=0xFE;                     // ��ֹ��ʱ��Timer3�ж�
    TMR3CN|= 0x04;                     // ����Timer3
}
void Timer2_Init (void)//��ʱ��2��ʼ�� ΪDA0ת���ṩ
{
    RCAP2H=0x05;
    RCAP2L=0x00;
    TH2=0x05;
    TL2=0x00;
    T2CON=0x04;
    IE|= 0x20;
}

void Interrupt_Init()//�жϳ�ʼ��
{
    //INT1
    //low level triggle
    IT1 = 0;
    //enable INT1
    EX1 = 1;
    //enable all interrupt
    EA = 1;
}

void Init_Device(void)//���ֳ�ʼ������
{
    //�رտ��Ź�
    EA = 0;
    WDTCN = 0xde;
    WDTCN = 0xad;
    EA = 1;

    Oscillator_Init();
    Port_IO_Init();
    Timer0_Init();
    Interrupt_Init();
    SYSCLK_Init ();//
    PORT_Init();//
    Timer3_Init(SYSCLK/SAMPLERATE0);//
    //TIMER4_Init(SYSCLK/SAMPLE_RATE_DAC);
    Timer2_Init ();//
    INT1 = 1;
    TR0=1;
    EX1=1;
    AD0EN=1;
    EA=1;

    LcdInit();
    LedInit();
}

void information(void)
{
	  WriteStr(0, 0, "������Ϣ");
    WriteStr(1, 0, "�༶���Զ���2205");
    WriteStr(2, 0, "������Ф����");
    WriteStr(3, 0, "ѧ�ţ�2226114221");
    
    LedDispNum();
    if (botflag==2) {
        mainflag=0;
        LcdClear();
        botflag=4;
    }
}
void picture(void)
{
	LedDispNum();
	ImageShow1(xjtu_image1);
	Delay_ms(1000);
	LcdClear();
	ImageShow1(gImage_white);
	mainflag=0;
	
	
}
void system(void)
{
	if(mimacount==0)
	{
		ImageShow1(xjtu_image2);
		Delay_ms(1000);
		LedDispNum();
		mimacount=1;
		mimaflag=0;
	}
	
		if(mimaflag==0)
		{
			
				ImageShow1(gImage_white);
				WriteStr(0, 0, "������ϵͳ");
				WriteStr(1, 0, "�����밴���");
				WriteStr(2, 0, "�˳��밴�Ҽ�");
				WriteStr(3, 0, "�����밴�м�");
				if (botflag==0) 
			{
					mimaflag=1;
					codenum=0;
					LcdClear();
					ImageShow1(gImage_white);
					botflag=4;
			}
			if (botflag==1) 
			{
					mimaflag=4;
					codenum=0;
					LcdClear();
					ImageShow1(gImage_white);
					botflag=4;
			}
			if (botflag==2) 
			{
				mimaflag=2;
				LcdClear();
				ImageShow1(gImage_white);
				botflag=4;
			}
		}
		if(mimaflag==1) 
		{
				WriteStr(0, 0, "����������");
				WriteStr(1, 0, "���ѡλ���м���1 ���Ҽ�ȷ��");
				
				if(botflag==0)
				{
					codenum=codenum+1;
					if(codenum==4)
					{
						
						codenum=0;
					}
					LcdClear();
					ImageShow1(gImage_white);
					if(codenum==0)
					{
						WriteStr(3,0,"*");
					}
					if(codenum==1)
					{
						WriteStr(3,0,"* *");
					}
					if(codenum==2)
					{
						WriteStr(3,0,"* * *");
					}
					if(codenum==3)
					{
						WriteStr(3,0,"* * * *");
					}
					botflag=4;
				}
				if(botflag==1)
				{
					code1234[codenum]++;
					if(code1234[codenum]==10)
					{
						code1234[codenum]=0;
					}
					botflag=4;
				}
				if(botflag==2)
				{
					mimaflag=3;
					LcdClear();
					ImageShow1(gImage_white);
					botflag=4;
				}
		}
		if(mimaflag==3)
		{
			WriteStr(0, 0, "ȷ�����룿");
			WriteStr(1, 0, "���ȷ�����Ҽ����");
			if(botflag==0)
			{
			if(code1234[0]==mima1[0]&&code1234[1]==mima1[1]&&code1234[2]==mima1[2]&&code1234[3]==mima1[3])
			{
				LcdClear();
				ImageShow1(gImage_white);
				WriteStr(0, 0, "����right");
				PlayMusic();
				LcdClear();
				ImageShow1(gImage_white);
				showlock();
				mainflag=0;
			}
			if(code1234[0]!=mima1[0]||code1234[1]!=mima1[1]||code1234[2]!=mima1[2]||code1234[3]!=mima1[3])
			{
				LcdClear();
				ImageShow1(gImage_white);
				WriteStr(0, 0, "����");
				P4=0xff;
				Delay_ms(1000);
				P4=0x00;
				Delay_ms(1000);
				P4=0xff;
				Delay_ms(1000);
				P4=0x00;
				mimaflag=0;
			}
				botflag=4;
			}
			if(botflag==2)
			{
				code1234[0]=0;
				code1234[1]=0;
				code1234[2]=0;
				code1234[3]=0;
				codenum=0;
				botflag=4;
				mimaflag=1;
				
				
			}
		}
		if(mimaflag==4)
		{
			WriteStr(0, 0, "����������");
				WriteStr(1, 0, "���ѡλ���м���1 ���Ҽ�ȷ��");
				
				if(botflag==0)
				{
					codenum=codenum+1;
					if(codenum==4)
					{
						
						codenum=0;
					}
					LcdClear();
					ImageShow1(gImage_white);
					if(codenum==0)
					{
						WriteStr(3,0,"*");
					}
					if(codenum==1)
					{
						WriteStr(3,0,"* *");
					}
					if(codenum==2)
					{
						WriteStr(3,0,"* * *");
					}
					if(codenum==3)
					{
						WriteStr(3,0,"* * * *");
					}
					botflag=4;
				}
				if(botflag==1)
				{
					code1234[codenum]++;
					if(code1234[codenum]==10)
					{
						code1234[codenum]=0;
					}
					botflag=4;
				}
				if(botflag==2)
				{
					mimaflag=6;
					LcdClear();
					ImageShow1(gImage_white);
					botflag=4;
				}
		}
		if(mimaflag==6)
		{
			WriteStr(0, 0, "ȷ�����룿");
			WriteStr(1, 0, "���ȷ�����Ҽ����");
			if(botflag==0)
			{
			if(code1234[0]==mima1[0]&&code1234[1]==mima1[1]&&code1234[2]==mima1[2]&&code1234[3]==mima1[3])
			{
				LcdClear();
				ImageShow1(gImage_white);
				WriteStr(0, 0, "����right");
				PlayMusic();
				LcdClear();
				ImageShow1(gImage_white);
				showlock();
				mimaflag=5;
			}
			if(code1234[0]!=mima1[0]||code1234[1]!=mima1[1]||code1234[2]!=mima1[2]||code1234[3]!=mima1[3])
			{
				LcdClear();
				ImageShow1(gImage_white);
				WriteStr(0, 0, "����");
				P4=0x00;
				Delay_ms(1000);
				P4=0xff;
				Delay_ms(1000);
				P4=0x00;
				Delay_ms(1000);
				P4=0xff;
				mimaflag=4;
			}
				botflag=4;
		}
	}
		if(mimaflag==5)
		{
			WriteStr(0, 0, "����������");
				WriteStr(1, 0, "���ѡλ���м���1 ���Ҽ�ȷ��");
				
				if(botflag==0)
				{
					codenum=codenum+1;
					if(codenum==4)
					{
						
						codenum=0;
					}
					LcdClear();
					ImageShow1(gImage_white);
					if(codenum==0)
					{
						WriteStr(3,0,"*");
					}
					if(codenum==1)
					{
						WriteStr(3,0,"* *");
					}
					if(codenum==2)
					{
						WriteStr(3,0,"* * *");
					}
					if(codenum==3)
					{
						WriteStr(3,0,"* * * *");
					}
					botflag=4;
				}
				if(botflag==1)
				{
					code1234[codenum]++;
					if(code1234[codenum]==10)
					{
						code1234[codenum]=0;
					}
					botflag=4;
				}
				if(botflag==2)
				{
					mima1[0]=code1234[0];
					mima1[1]=code1234[1];
					mima1[2]=code1234[2];
					mima1[3]=code1234[3];
					LcdClear();
					ImageShow1(gImage_white);
					mainflag=0;
					
					botflag=4;
				}
		}
		if(mimaflag==2)
		{
			mainflag=0;
			LcdClear();
			ImageShow1(gImage_white);
			botflag=4;
		}
		show_mima_row(code1234);
}
//-----------------------------------------------------------------------------
// ������
//-----------------------------------------------------------------------------
void main()
{
	Init_Device();//���ֳ�ʼ��
    INT1 = 1;//�����ж�
    TR0=0;//��ʱ��ֹͣ

    while(1)
    {
        if(mainflag==0)//������
        {
					mimacount=0;
						ImageShow1(gImage_white);
            WriteStr(0, 0, "ʵ��һ") ;
            WriteStr(1, 0, "������Ϣ");
            WriteStr(2, 0, "ͼƬչʾ");
            WriteStr(3, 0, "������");
						output = 0;
            LedDispNum();
            if (botflag==0) {
                mainflag=1;
                LcdClear();
                botflag=4;
            }
            if (botflag==1) {
                mainflag=2;
                LcdClear();
                botflag=4;
            }
            if (botflag==2) {
                mainflag=3;
                LcdClear();
                botflag=4;
            }
        }
        if(mainflag==1) {
            information();   //������Ϣ����
        }
        if(mainflag==2) {
            picture();   //ͼƬ����
						mainflag==0;
        }
        if(mainflag==3) {
            system();   //ϵͳ����
        }
    }
}

void INT1_ISR(void) interrupt 2
{
    Delay_ms(1);
    switch(P5)
    {
    case 0xfb:   //����Ǹ���
        botflag=0;
        Delay_ms(50);
        break;

    case 0xfd:    //�м��Ǹ���
        botflag=1;
        Delay_ms(50);
        if(mainflag==2)state=(state+1)%4;
        break;

    case 0xfe:   //�ұ��Ǹ���
        botflag=2;
        Delay_ms(50);
        break;
    }
}

void Timer0_ISR (void) interrupt 1
{
    TH0 = TIMER0_RELOAD_HIGH;           // Reinit Timer0 High register
    TL0 = TIMER0_RELOAD_LOW;

    Time_num++;

    if(Time_num >= 6000) Time_num = 0;
    if(key_delay>0)
        key_delay++;
    if(key_delay==20) key_delay=0;

    if(lcd_delay>0)
        lcd_delay++;
    if(lcd_delay>100) lcd_delay=0;
}
void Timer1_ISR(void) interrupt 3//��ʱ��1�ж�
{
    TH1|=0xfc;
    TL1|=0x18;
    tcount++;
    if(tcount>=20)
    {
        tcount=0;
//			Timeout=1;
    }
    count++;
}
void Time2_ISR(void) interrupt 5 // ��ʱ��2�жϷ���
{
    TF2=0;
    //Set_DACs_forsine();//���ɺ�����
    DAC0=output;
}

