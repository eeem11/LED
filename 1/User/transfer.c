#include "transfer.h"
		
char payload[128];
int clude =0;

u16 USART2_RX_STA=0;       //����״̬���	  
u8 USART2_RX_BUF[200];     //���ջ���,���USART_REC_LEN���ֽ�.
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
void USART2_IRQHandler(void)                	//����2�жϷ������
{
	u8 Res,t;

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		Res =USART_ReceiveData(USART2);	//��ȡ���յ�������	
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
		if((USART2_RX_STA&0x8000)==0)//����δ���
			{
			if(USART2_RX_STA&0x4000)//���յ���0x0d
				{
				if(Res!=0x0a)USART2_RX_STA=0;//���մ���,���¿�ʼ
				else USART2_RX_STA|=0x8000;	//��������� 
				}
			else //��û�յ�0X0D
				{	
				if(Res==0x0d)USART2_RX_STA|=0x4000;
				else
					{
					USART2_RX_BUF[USART2_RX_STA&0X3FFF]=Res ;
					USART2_RX_STA++;
					if(USART2_RX_STA>(200-1))USART2_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
					}		 
				}
			}   		 
     } 
}
void receive_usart2_handle()
{	
	if(USART2_RX_STA&0x8000)
	{					   

		memset(USART2_RX_BUF,0,sizeof(USART2_RX_BUF));//���buffer
		USART2_RX_STA=0;
	}
}

void heartbat1(){
		usart2_send((uint8_t*)"73YXL7ULZ53KPXLA",16);
		}
		
void heartbat2(){		
		usart2_send((uint8_t*)"#21,211,30#",11);
		}