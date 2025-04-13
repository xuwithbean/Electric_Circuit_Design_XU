#include <reg52.h> 
#include <intrins.h> 
typedef   unsigned   char        uint8;        
typedef   unsigned   int         uint16;   
typedef   unsigned   long int    uint32;                
/********************************* 
PID = Uk + KP*[E(k)-E(k-1)]+KI*E(k)+KD*[E(k)-2E(k-1)+E(k-2)];(???PID??) 
????: RK(???),CK(???),KP,KI,KD 
????: U(K) 
//PID???? 
********************************/ 
void     PIDOperation (void)   
{  
uint32       Temp[3];                                        //?????? 
uint32       PostSum;                                       //??? 
uint32       NegSum;                                       //??? 
Temp[0] = 0; 
Temp[1] = 0; 
Temp[2] = 0; 
PostSum = 0; 
NegSum  = 0; 

if( PID.RK_Uint16 > PID.RK_Uint16 )                    //?????????? 
{ 
 if( PID.RK_Uint16 - PID.RK_Uint16 >10 )            //????10?? 
 { 
  PID.Uk_Uint16 = 100;    //????10???????(????) 
 }                     
 else 
{ 
Temp[0] = PID.RK_Uint16 - PID.CK_Uint16;       //??<=10,??E(k) 
PID.EkFlag_Uint8[1]=0;                        //E(k)??? 
  //???? 
PID.Ek_Uint32[2] = PID.Ek_Uint32[1]; 
PID.Ek_Uint32[1] = PID.Ek_Uint32[0]; 
PID.Ek_Uint32[0] = Temp[0]; 
/****************************************/ 
if( PID.Ek_Uint32[0] >PID.Ek_Uint32[1] )                            //E(k)>E(k-1)?? 
{ 
Temp[0]=PID.Ek_Uint32[0] - PID.Ek_Uint32[1];           //E(k)>E(k-1) 
PID.EkFlag_Uint8[0]=0;                                       //E(k)-E(k-1)???
} 
else 
{ 
Temp[0]=PID.Ek_Uint32[0] - PID.Ek_Uint32[1];        //E(k)<E(k-1) 
 PID.EkFlag_Uint8[0]=1;                                                //E(k)-E(k-1)???
}
/****************************************/ 
Temp[2]=PID.Ek_Uint32[1]*2 ;                                             // 2E(k-1) 
if( (PID.Ek_Uint32[0]+ PID.Ek_Uint32[2])>Temp[2] )            //E(k-2)+E(k)>2E(k-1)?? 
{ 
Temp[2]=(PID.Ek_Uint32[0]+ PID.Ek_Uint32[2])-Temp[2];     //E(k-2)+E(k)>2E(k-1) 
PID.EkFlag_Uint8[2]=0;                                           //E(k-2)+E(k)-2E(k-1)??? 
} 
else 
{ 
Temp[2]=Temp[2]-(PID.Ek_Uint32[0]+ PID.Ek_Uint32[2]);  //E(k-2)+E(k)<2E(k-1) 
PID.EkFlag_Uint8[2]=1;                                      //E(k-2)+E(k)-2E(k-1)???
}  
/****************************************/        
Temp[0] = (uint32)PID.KP_Uint8 * Temp[0];                        // KP*[E(k)-E(k-1)] 
Temp[1] = (uint32)PID.KI_Uint8 * PID.Ek_Uint32[0];              // KI*E(k) 
Temp[2] = (uint32)PID.KD_Uint8 * Temp[2];                      // KD*[E(k-2)+E(k)-2E(k-1)] 




/*????????????????,?????*/      
/**********KP*[E(k)-E(k-1)]**********/ 
if(PID.EkFlag_Uint8[0]==0) 
PostSum += Temp[0];                                    //??? 
else                                               
NegSum += Temp[0];                                    //??? 
/********* KI*E(k)****************/  
if(PID.EkFlag_Uint8[1]==0)       
PostSum += Temp[1];                                 //??? 
else 
;                                                 //???,E(K)>0 
/****KD*[E(k-2)+E(k)-2E(k-1)]****/                            
if(PID.EkFlag_Uint8[2]==0) 
PostSum += Temp[2];                               //??? 
else 
NegSum += Temp[2];                             //??? 
/***************U(K)***************/                              
PostSum += (uint32)PID.Uk_Uint16;     
 
if(PostSum > NegSum )                         // ???????? 
{ 
Temp[0] = PostSum - NegSum; 
if( Temp[0] < 100 )                         //????????????? 
PID.Uk_Uint16 = (uint16)Temp[0]; 
else 
PID.Uk_Uint16 = 100;                     //????????? 
} 
else                                     //????????,???0(??????) 
PID.Uk_Uint16 = 0; 
} 
} 
else  
{ 
PID.Uk_Uint16 = 0;  
}  
} 

void     PIDOutput (void)   
{  
static  int i; 
i=PID.Uk_Uint16; 
if(i==0) 
out=1; 
else out=0; 
if((count++)==5)//??????40MS,40MS*5=0.2S(??????),????20S(100??) 
{              //?20S PID???? 
count=0; 
i--; 
} 
} 

