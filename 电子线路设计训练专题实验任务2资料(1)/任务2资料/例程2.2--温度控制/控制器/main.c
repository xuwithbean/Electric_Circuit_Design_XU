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

#define TIMER0_RELOAD_HIGH 0x4f//  Timer0高位重载值
#define TIMER0_RELOAD_LOW 0x00   //  Timer0低位重载值

typedef   unsigned   char        uint8;        
typedef   unsigned   int         uint16;   
typedef   unsigned   long int    uint32;  

uchar cure_data_flag;//用于18650的数据显示
uchar cure_data[2][1024];//

int pidflag = 0;

//pid控制
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
Queue points;//建立一个队列存储数据
uint8 out ;                
uint8 count;
int lcd_x;
int temp;
int temp_uk;
//参数设定
int kp = 5;
int ki = 2;
int kd = 0;
int rk = 26;
int test = 100;
uint32 Temp_pid[3];                                       
uint32 PostSum;                                    
uint32 NegSum;  

//pid控制函数
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

//dac0初始化
void dac0_init(void)
{
	DAC0CN = 0x80;//使能dac，dac更新在写dac0h，数据格式为正常数据格式
	REF0CN |= 0x03;//打开ADC，dac偏压发生器，基准电压由vref提供
}

//18650画曲线函数
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

//timer0初始化
void Timer0_Init()
{
	TH0 = TIMER0_RELOAD_HIGH;           // 设置高8位
	TL0 = TIMER0_RELOAD_LOW;
	TMOD = 0x01;                        // 设置为16位模式
	ET0 = 1;                            // 使能定时器0
	TR0 = 1;
	pidflag++;
	if (pidflag == 10)
	{
		PIDOperation();//pid控制
		pidflag=0;
  }
}

//timer0溢出中断服务函数
void Timer0_ISR (void) interrupt 1
{
	TH0 = TIMER0_RELOAD_HIGH;           // 重新装载定时器0
	TL0 = TIMER0_RELOAD_LOW;
	PID.CK_Uint16 = temp;
	PID.CK_Uint16 = PID.CK_Uint16/77;//获取温度数据
	//PIDOperation();
	temp_uk = PID.Uk_Uint16*33;//获得温度控制百分百
	meind_dight(PID.CK_Uint16);//显示
	down_dight(PID.Uk_Uint16);
	up_dight(PID.RK_Uint16);
	//meind_dight(ki);
	//down_dight(kd);
}

//总初始化
void Init_Device(void)
{
		EA = 0;//关闭总中断
    WDTCN = 0xde;
    WDTCN = 0xad;//关闭看门狗
    EA = 1;//打开总中断
	  SYSCLK_Init();//时钟初始化
	  XBR2 = 0x40;//使能交差开关
	  Port_IO_Init();//端口初始化
	  timer3_init();//定时器3初始化
	  ADC0_Init ();//adc0初始化
		TIMER4_Init (100);//定时器4初始化
		dac0_init();//dac0初始化
		//LcdInit();
		Timer0_Init();//定时器0初始化
}

//定时器4溢出中断服务函数
void TIMER4_ISR (void) interrupt 16
{
		T4CON &= ~0x80;// Clear Timer4 overflow flag 
		DAC0L = temp_uk & (0x00ff);//取出低8位
		DAC0H = temp_uk>>8;//取出高8位
}

//adc0中断服务函数
void ADC0_ISR () interrupt 15
{
		int adc_re = ADC0H;
		AD0INT=0;
		adc_re = (adc_re <<8)|(ADC0L);//获得adc0数据
		//up_dight(adc_re);//显示0通道数据
		//up_dight(kp);
		temp = adc_re;//存储温度数据
}

void main()
{
	Init_Device();//初始化
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
			EnQueue(&points,PID.CK_Uint16);//入队列0-63
			memset(cure_data[cure_data_flag],0,1024*sizeof(uchar));//刷新缓冲器，全部赋0
			draw_buffer(cure_data[cure_data_flag],&points);//将队列中的点绘制到缓冲区
			ScreenShow(cure_data[cure_data_flag],cure_data[!cure_data_flag],0);//画点，利用2个缓冲区	
			cure_data_flag=!cure_data_flag;//缓冲区互换角色
		}
	}
}