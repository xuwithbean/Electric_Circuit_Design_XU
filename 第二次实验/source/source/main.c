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
int setting=2440;//�趨���Ƶ�ѹֵ
int result;//������ͼʱ����תֵ��16���ƣ�
long xdata result2;//������ͼʱ����תֵ��10���ƣ�


float kp=2.0;//PID����
float ki=0.10;
float kd=0.05;
int error,derror;//����м���
int preerror=0;
int lasterror=0;
long sumerror=0;//������
int output;//���


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

void ADC0_Init (void)//ADC0��ʼ��
{
    ADC0CN = 0xC4;       // �ر�ADC0; ����׷��ģʽ
    REF0CN = 0x03;        // ���¶ȴ�����, on-chip VREF
    AMX0SL=0x01;     // ADC0ʹ��"0"ͨ��
    ADC0CF=31<<3;         // ADCת��ʱ��= 2.5MHz
    ADC0CF&=0xf8;          // PGA gain = 1
    EIE2|=0x02;        // ��ADC0�ж�
}

unsigned int ADC_Value (unsigned char Flag)//����ADC��ֵ
{
    unsigned int data temp,temp0,temp1;
    if(Flag)                    //ADC1
        temp0=ADC1;
    else                        //ADC0
    {
        temp0=ADC0L;
        temp1=ADC0H;
    }
    temp=temp1*256+temp0;
    return temp;                //ȡ��ADCת������
}
void DAC0_Init(void)//DAC0��ʼ��
{
    REF0CN = 0x03;                   // ʹ���ڲ�VREF������
    DAC0CN = 0x98;                   //��ʱ��2��000ģʽ
    DAC0L=0x00;
    DAC0H=0x00;                   // ��DAC0���Ҷ���ģʽ
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
    ADC0_Init ();//
    Timer2_Init ();//
    DAC0_Init();//
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
void pid(void)
{
    unsigned int i=0;
    LedDispNum();
    WriteStr(0, 1, "Kp:");
    WriteStr(1, 1, "Ki:");
    WriteStr(2, 1, "Kd:");
	  WriteStr(3, 1, "����");
    if(state==0) {
        if (botflag==0) {
            kp=kp+0.01;
            botflag=4;
        }
        if (botflag==2) {
            kp=kp-0.01;
            botflag=4;
        }
        sprintf(s, "Kp:%.3f ", kp);
        WriteStr(0, 1, s);
				WriteStr(0, 0, ">");
				WriteStr(1, 0, "  ");
				WriteStr(2, 0, "  ");
				WriteStr(3, 0, "  ");
    }
    else if(state ==1) {
        if (botflag==0) {
            ki=ki+0.001;
            botflag=4;
        }
        if (botflag==2) {
            ki=ki-0.001;
            botflag=4;
        }
        sprintf(s, "Ki:%.3f ", ki);
        WriteStr(1, 1, s);
				WriteStr(1, 0, ">");
				WriteStr(0, 0, "  ");
				WriteStr(2, 0, "  ");
				WriteStr(3, 0, "  ");
    }
    else if(state ==2) {
        if (botflag==0) {
            kd=kd+0.001;
            botflag=4;
        }
        if (botflag==2) {
            kd=kd-0.001;
            botflag=4;
        }
        sprintf(s, "Kd:%.3f ", kd);
        WriteStr(2, 1, s);
				WriteStr(2, 0, ">");
				WriteStr(1, 0, "  ");
				WriteStr(0, 0, "  ");
				WriteStr(3, 0, "  ");
    }
    else if(state ==3) {
				WriteStr(3, 0, ">");
				WriteStr(1, 0, "  ");
				WriteStr(2, 0, "  ");
				WriteStr(0, 0, "  ");
        if (botflag==0) {
            mainflag=0;
            LcdClear();
            botflag=4;
        }
    }


}
void system(void)
{
    int i;
    if (showflag2==0)//ʵ����Ӳ˵�
    {
        WriteStr(0, 0, "ϵͳ����");
        WriteStr(1, 0, "L:����");
        WriteStr(2, 0, "R:����");
        
        LedDispNum();
        if (botflag==2) {
            mainflag=0;
            LcdClear();
            botflag=4;
        }
        if (botflag==0) {
            showflag2=2;
            LcdInit();
            botflag=4;
        }
    }
    if (showflag2==2)//����ƽ����
    {
        DAC0CN = 0x98;
        filtertype2=0;//ѡ���㷨ƽ���˲�

        //v[samplepos++]=(int)(ADC0_result[1]-270)*1.22;
        v[samplepos++]=(int)(ADC0_result[1]-270)*1.101;
        samplepos%=10;
        if(init>0)
        {
            init--;
            //continue;
        }
        else if(init==0)//Ϊ������ֵ�������
        {
            for(i=1; i<10; i++)
            {
                slidesum+=v[i];
            }
            init=-1;
        }
        ledcount=30; //Խ����ʾ�����ٶ�Խ������������
        switch(filtertype2)
        {
        case 0:
            Average_filter();//�㷨ƽ��ֵ�˲�
            break;
        case 1:
            Median_average_filter();//��λֵƽ���˲�
            break;
        case 2:
            Sliding_filter();//������ֵ�˲�
            break;
        }
        //wavevalue[wavepos]=w1*1000+x1*100+y1*10+z1;
        wavevalue[wavepos]=(result-1900)*10.24;//ȫ����ʾ
        if(wavepos==0) {
            ImageShow1(gImage_white);
        }
        LcdShowPoint(wavepos++);
        wavepos%=128;
        
        while(ledcount)
        {
            show_f_row();
            show_s_row();
            show_t_row();
            ledcount--;
        }
        //�����㷨
        error=setting-result2;
        sumerror+=error;
        derror=lasterror-preerror;
        preerror=lasterror;
        lasterror=error;
        output=result2+kp*error+ki*sumerror+kd*derror;
				w2 =  setting/1000;//��ʾ�趨ֵ
        x2 = (setting - w2*1000)/100;
        y2 = (setting - w2*1000-x2*100)/10;
        z2 =  setting - w2*1000-x2*100-y2*10;
        w3 =  output/1000;//��ʾ������
        x3 = (output - w3*1000)/100;
        y3 = (output - w3*1000-x3*100)/10;
        z3 =  output - w3*1000-x3*100-y3*10;
        if (botflag==0) {
            setting=setting-10;
            botflag=4;
        }
        if (botflag==1) {
            setting=setting+10;
            botflag=4;
        }
        if (botflag==2) {
            showflag2=0;
            LcdInit();
            DAC0CN=0;
            botflag=4;
        } 
    }
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
            WriteStr(0, 0, "������Ϣ") ;
            WriteStr(1, 0, "ģ��ɻ�");
            WriteStr(2, 0, "PID �趨");
            WriteStr(3, 0, "����ϵͳ");
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
            pid();   //PID����
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

void ADC0_ISR (void) interrupt 15 //ADC0�жϷ���
{
    // ADC mux ?? (0-1)
    AD0INT=0;
    ADC0_result[channel]=ADC_Value(0);
    channel++;
    if (channel>=2)
        channel = 0;
    //AMX0SL = channel;
}
