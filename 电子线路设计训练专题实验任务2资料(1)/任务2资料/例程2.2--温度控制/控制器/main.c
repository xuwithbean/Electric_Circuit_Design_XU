/************************************************
designed by wjj
wjj12138@stu.xjtu.edu.cn
************************************************/
#include "dight.h"
#include "sysclk.h"
#include "port.h"
#include "time.h"
#include "adc.h"
#include "lcd.h"
#include "string.h"

#define TIMER0_RELOAD_HIGH 0x4f//  Timer0��λ����ֵ
#define TIMER0_RELOAD_LOW 0x00   //  Timer0��λ����ֵ

typedef   unsigned   char        uint8;        
typedef   unsigned   int         uint16;   
typedef   unsigned   long int    uint32;  

uchar cure_data_flag;//����18650��������ʾ
uchar cure_data[2][1024];//

int pidflag = 0;

//pid����
typedef struct PIDValue 
{ 
uint32      Ek_Uint32[3];                  
uint8       EkFlag_Uint8[3];                 
uint8       KP_Uint8; 
uint8       KI_Uint8; 
uint8       KD_Uint8; 
uint16      Uk_Uint16;                 
uint16      RK_Uint16;                 
uint16      CK_Uint16;                
}PIDValueStr; 


PIDValueStr  PID; 
Queue points;//����һ�����д洢����
uint8 out ;                
uint8 count;
int lcd_x;
int temp;
int temp_uk;
//�����趨
int kp = 5;
int ki = 2;
int kd = 0;
int rk = 26;
int test = 100;
uint32 Temp_pid[3];                                       
uint32 PostSum;                                    
uint32 NegSum;  

//pid���ƺ���
void PIDOperation (void)   
{ 
	/*uint32 Temp[3];                                       
	uint32 PostSum;                                    
	uint32 NegSum; 
	Temp[0] = 0; 
	Temp[1] = 0; 
	Temp[2] = 0; 
	PostSum = 0; 
	NegSum  = 0; 
	PID.KP_Uint8 = kp;
	PID.KI_Uint8 = ki;
	PID.KD_Uint8 = kd;
	PID.RK_Uint16 = rk;*/
	PID.KP_Uint8 = kp;
  PID.KI_Uint8 = ki;
  PID.KD_Uint8 = kd;
  PID.RK_Uint16 = rk;
	PostSum = 0; 
  NegSum  = 0;
	Temp_pid[0] = 0; 
  Temp_pid[1] = 0; 
  Temp_pid[2] = 0; 
	if( PID.RK_Uint16 > PID.CK_Uint16 )                   
	{ 
	 if( PID.RK_Uint16 - PID.CK_Uint16 >10 )            
	 { 
		PID.Uk_Uint16 = 100;   
	 }                     
	 else 
	{ 
	Temp_pid[0] = PID.RK_Uint16 - PID.CK_Uint16;        
	PID.EkFlag_Uint8[1]=0;                         
	PID.Ek_Uint32[2] = PID.Ek_Uint32[1]; 
	PID.Ek_Uint32[1] = PID.Ek_Uint32[0]; 
	PID.Ek_Uint32[0] = Temp_pid[0]; 
	/****************************************/ 
	if( PID.Ek_Uint32[0] >PID.Ek_Uint32[1] )                           
	{ 
	Temp_pid[0]=PID.Ek_Uint32[0] - PID.Ek_Uint32[1];           
	PID.EkFlag_Uint8[0]=0;                                       
	} 
	else 
	{ 
	Temp_pid[0]=PID.Ek_Uint32[0] - PID.Ek_Uint32[1];        
	 PID.EkFlag_Uint8[0]=1;                                                
	}
	/****************************************/ 
	Temp_pid[2]=PID.Ek_Uint32[1]*2 ;                                             
	if( (PID.Ek_Uint32[0]+ PID.Ek_Uint32[2])>Temp_pid[2] )            
	{ 
	Temp_pid[2]=(PID.Ek_Uint32[0]+ PID.Ek_Uint32[2])-Temp_pid[2];     
	PID.EkFlag_Uint8[2]=0;                                           
	} 
	else 
	{ 
	Temp_pid[2]=Temp_pid[2]-(PID.Ek_Uint32[0]+ PID.Ek_Uint32[2]);  
	PID.EkFlag_Uint8[2]=1;                                      
	}  
	/****************************************/        
	Temp_pid[0] = (uint32)PID.KP_Uint8 * Temp_pid[0];                        
	Temp_pid[1] = (uint32)PID.KI_Uint8 * PID.Ek_Uint32[0];             
	Temp_pid[2] = (uint32)PID.KD_Uint8 * Temp_pid[2];                      
	/**********KP*[E(k)-E(k-1)]**********/ 
	if(PID.EkFlag_Uint8[0]==0) 
	PostSum += Temp_pid[0];                                   
	else                                               
	NegSum += Temp_pid[0];                                    
	/********* KI*E(k)****************/  
	if(PID.EkFlag_Uint8[1]==0)       
	PostSum += Temp_pid[1];                                
	else 
	;                                                 
	/****KD*[E(k-2)+E(k)-2E(k-1)]****/                            
	if(PID.EkFlag_Uint8[2]==0) 
	PostSum += Temp_pid[2];                               
	else 
	NegSum += Temp_pid[2];                             
	/***************U(K)***************/                              
	PostSum += (uint32)PID.Uk_Uint16;     
	 
	if(PostSum > NegSum )                         
	{ 
			Temp_pid[0] = PostSum - NegSum; 
		if( Temp_pid[0] < 100 )                         
			PID.Uk_Uint16 = Temp_pid[0]; 
		else 
			PID.Uk_Uint16 = 100;                    
	} 
	else                                    
		PID.Uk_Uint16 = 0; 
	} 
	} 
	else  
	{ 
		PID.Uk_Uint16 = 0;  
	}  
} 

//dac0��ʼ��
void dac0_init(void)
{
	DAC0CN = 0x80;//ʹ��dac��dac������дdac0h�����ݸ�ʽΪ�������ݸ�ʽ
	REF0CN |= 0x03;//��ADC��dacƫѹ����������׼��ѹ��vref�ṩ
}

//18650�����ߺ���
void draw_buffer(uchar *buffer,Queue *Q)
{
	uchar num=Q->number,index=Q->rear,count=0,temp;
	while(num--!=0)
	{
		index=(index+129-1)%129;
		temp=(63-Q->base[index])*8+count>>4;
		buffer[(63-Q->base[index])*16+(count>>3)]|=0x80>>(count&0x07);
		++count;
	}
}

//timer0��ʼ��
void Timer0_Init()
{
	TH0 = TIMER0_RELOAD_HIGH;           // ���ø�8λ
	TL0 = TIMER0_RELOAD_LOW;
	TMOD = 0x01;                        // ����Ϊ16λģʽ
	ET0 = 1;                            // ʹ�ܶ�ʱ��0
	TR0 = 1;
	pidflag++;
	if (pidflag == 10)
	{
		PIDOperation();//pid����
		pidflag=0;
  }
}

//timer0����жϷ�����
void Timer0_ISR (void) interrupt 1
{
	TH0 = TIMER0_RELOAD_HIGH;           // ����װ�ض�ʱ��0
	TL0 = TIMER0_RELOAD_LOW;
	PID.CK_Uint16 = temp;
	PID.CK_Uint16 = PID.CK_Uint16/77;//��ȡ�¶�����
	//PIDOperation();
	temp_uk = PID.Uk_Uint16*33;//����¶ȿ��ưٷְ�
	meind_dight(PID.CK_Uint16);//��ʾ
	down_dight(PID.Uk_Uint16);
	up_dight(PID.RK_Uint16);
	//meind_dight(ki);
	//down_dight(kd);
}

//�ܳ�ʼ��
void Init_Device(void)
{
		EA = 0;//�ر����ж�
    WDTCN = 0xde;
    WDTCN = 0xad;//�رտ��Ź�
    EA = 1;//�����ж�
	  SYSCLK_Init();//ʱ�ӳ�ʼ��
	  XBR2 = 0x40;//ʹ�ܽ����
	  Port_IO_Init();//�˿ڳ�ʼ��
	  timer3_init();//��ʱ��3��ʼ��
	  ADC0_Init ();//adc0��ʼ��
		TIMER4_Init (100);//��ʱ��4��ʼ��
		dac0_init();//dac0��ʼ��
		//LcdInit();
		Timer0_Init();//��ʱ��0��ʼ��
}

//��ʱ��4����жϷ�����
void TIMER4_ISR (void) interrupt 16
{
		T4CON &= ~0x80;// Clear Timer4 overflow flag 
		DAC0L = temp_uk & (0x00ff);//ȡ����8λ
		DAC0H = temp_uk>>8;//ȡ����8λ
}

//adc0�жϷ�����
void ADC0_ISR () interrupt 15
{
		int adc_re = ADC0H;
		AD0INT=0;
		adc_re = (adc_re <<8)|(ADC0L);//���adc0����
		//up_dight(adc_re);//��ʾ0ͨ������
		//up_dight(kp);
		temp = adc_re;//�洢�¶�����
}

void main()
{
	Init_Device();//��ʼ��
	PID.Uk_Uint16 = 0;
	PID.Ek_Uint32[2] = 0; 
	PID.Ek_Uint32[1] = 0; 
	PID.Ek_Uint32[0] = 0;
	lcd_x = 0;
	while(1)
	{
		/*PID.CK_Uint16 = temp;
		PID.CK_Uint16 = PID.CK_Uint16/33;
		PIDOperation();
		temp_uk = PID.Uk_Uint16*33;*/

		lcd_x ++;
		if(lcd_x == 1000)
		{
			lcd_x = 0;
			EnQueue(&points,PID.CK_Uint16);//�����0-63
			memset(cure_data[cure_data_flag],0,1024*sizeof(uchar));//ˢ�»�������ȫ����0
			draw_buffer(cure_data[cure_data_flag],&points);//�������еĵ���Ƶ�������
			ScreenShow(cure_data[cure_data_flag],cure_data[!cure_data_flag],0);//���㣬����2��������	
			cure_data_flag=!cure_data_flag;//������������ɫ
		}
	}
}