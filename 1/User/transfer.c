#include "transfer.h"
		
char payload[128];
int clude =0;

u16 USART2_RX_STA=0;       //接收状态标记	  
u8 USART2_RX_BUF[200];     //接收缓冲,最大USART_REC_LEN个字节.
char Rec_Data[255]=""; 

void usart2_send(unsigned char *data,unsigned char len)
{
		unsigned char t;
		for(t=0;t<len;t++)
        {	
		    while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);
            USART_SendData(USART2,data[t]); 				
        }
}
void USART2_IRQHandler(void)                	//串口2中断服务程序
{
	u8 Res,t;

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res =USART_ReceiveData(USART2);	//读取接收到的数据	
		   if(Res=='a')
				{
					TIM_SetCompare1(TIM1, 0);
					clude=0;
				}				
					if(Res=='b')
				{
					TIM_SetCompare1(TIM1, 100);
					clude=1;
				}				
		if((USART2_RX_STA&0x8000)==0)//接收未完成
			{
			if(USART2_RX_STA&0x4000)//接收到了0x0d
				{
				if(Res!=0x0a)USART2_RX_STA=0;//接收错误,重新开始
				else USART2_RX_STA|=0x8000;	//接收完成了 
				}
			else //还没收到0X0D
				{	
				if(Res==0x0d)USART2_RX_STA|=0x4000;
				else
					{
					USART2_RX_BUF[USART2_RX_STA&0X3FFF]=Res ;
					USART2_RX_STA++;
					if(USART2_RX_STA>(200-1))USART2_RX_STA=0;//接收数据错误,重新开始接收	  
					}		 
				}
			}   		 
     } 
}
void receive_usart2_handle()
{	
	if(USART2_RX_STA&0x8000)
	{					   

		memset(USART2_RX_BUF,0,sizeof(USART2_RX_BUF));//清除buffer
		USART2_RX_STA=0;
	}
}

void heartbat1(){
		usart2_send((uint8_t*)"73YXL7ULZ53KPXLA",16);
		}
		
void heartbat2(){		
		usart2_send((uint8_t*)"#21,211,30#",11);
		}