#include "stm32f10x.h"                  // Device header
#include "adcx.h"
#include "ldr.h"
#include "oled.h"
#include "dht11.h"
#include "pwm.h"
#include "led.h"
#include "key.h"
#include "tim2.h"   
#include "tim3.h"   
//#include "hc_sr501.h"
#include "hc_sr04.h"
#include "sensormodules.h"
//#include "usart.h"
#include "gizwits_product.h"
//#include "myrtc.h"
#include "flash.h"
#include "iwdg.h"
#include "delay.h"
#include <stdio.h>
#include <string.h>
#include "transfer.h"



#define KEY_Long1	11

#define KEY_1	1
#define KEY_2	2
#define KEY_3	3
#define KEY_4	4

#define FLASH_START_ADDR	0x0801f000	//д�����ʼ��ַ

volatile uint8_t rx_buffer[256];
volatile uint16_t rx_index = 0;
volatile uint8_t response_received = 0;


uint8_t systemModel = 0;				//�洢ϵͳ��ǰģʽ
uint8_t menu = 1;					//��ʾ�˵�����



DHT11_Data_TypeDef DHT11_Data;	//�������������ݽṹ�����
SensorModules sensorData;	//�������������ݽṹ�����
SensorThresholdValue Sensorthreshold;	//������������ֵ�ṹ�����
//����
//u16 USART2_RX_STA=0;       //����״̬���	  
//u8 USART2_RX_BUF[200];     //���ջ���,���USART_REC_LEN���ֽ�.
//char Rec_Data[255]=""; 

enum 
{
	display_page = 1,
	settingsPage,
	timeSettingsPage
	
}MenuPages;

//����ɲ˵�
void OLED_Menu(void)
{
	
	OLED_ShowCHinese(0*16,2, 0);
	OLED_ShowCHinese(1*16,2, 1);

	//��ʾϵͳ��ǰģʽ �ֶ�ģʽ or �Զ�ģʽ
		if (systemModel)
		{
			//��ʾ���Զ���
			OLED_ShowCHinese(0*8, 0*8, 9);
			OLED_ShowCHinese(2*8, 0*8, 10);					
		}
		else
		{
			//��ʾ���ֶ���
			OLED_ShowCHinese(0*8, 0*8, 11);
			OLED_ShowCHinese(2*8, 0*8, 12);			
		}
	//��ʾ���¶ȣ���
	OLED_ShowCHinese(0*16,2, 0);
	OLED_ShowCHinese(1*16,2, 1);
	
	//��ʾ��ʪ�ȣ���
	OLED_ShowCHinese(4*16,2, 2);
	OLED_ShowCHinese(5*16,2, 1);
		
	
	//��ʾ����ǿ����
	OLED_ShowCHinese(0*16,4, 4);
	OLED_ShowCHinese(1*16,4, 5);
	
	//��ʾ�����룺��
	OLED_ShowCHinese(4*16,4, 17);
	OLED_ShowCHinese(5*16,4, 18);
	
		
	//��ʾ�����ȣ���
	OLED_ShowCHinese(0*16,6, 6);
	OLED_ShowCHinese(1*16,6, 1);
	
}

///**
//  * @brief  ��ʾ��ҳ��Ĵ��������ݵ���Ϣ
//  * @param  ��
//  * @retval ��
//  */
void OLED_Menu_SensorData(void)
{
	
		//��ʾ�¶�����
		OLED_ShowNum(2*16, 2, DHT11_Data.temperature, 3,16);
		OLED_ShowChar(7*8, 2, 'C');	

		//��ʾʪ������
		OLED_ShowNum(6*16, 2, DHT11_Data.humidity, 3,16);	
		OLED_ShowChar(15*8, 2, '%');	

		//��ʾ��ǿ����
		OLED_ShowNum(2*16, 4, sensorData.lux, 3,16);	

		//��ʾ��������
		OLED_ShowNum(6*16, 4, sensorData.distance, 3,16);

		//��ʾ���ȵȼ�
		OLED_ShowNum(2*16, 6, ledDutyRatio, 3,16);
		OLED_ShowChar(7*8, 6, '%');	
	

		
}
/**
  * @brief  ��ʾϵͳ���ý���
  * @param  ��
  * @retval ��
  */
void OLED_SetInterfacevoid(void)
{
	//��ʾ��ϵͳ���ý��桱
	OLED_ShowCHinese(1*16, 0*16, 20);
	OLED_ShowCHinese(2*16, 0*16, 21);
	OLED_ShowCHinese(3*16, 0*16, 22);
	OLED_ShowCHinese(4*16, 0*16, 23);
	OLED_ShowCHinese(5*16, 0*16, 24);
	OLED_ShowCHinese(6*16, 0*16, 25);	
	
	
	//��ʾ��������ֵ��
	OLED_ShowCHinese(1*16, 4, 4);
	OLED_ShowCHinese(2*16, 4, 5);	
	OLED_ShowCHinese(3*16, 4, 26);
	OLED_ShowCHinese(4*16, 4, 27);	
	

	//��ʾ��������ֵ��
	OLED_ShowCHinese(1*16, 6, 17);
	OLED_ShowCHinese(2*16, 6, 18);	
	OLED_ShowCHinese(3*16, 6, 26);
	OLED_ShowCHinese(4*16, 6, 27);	
	
	
	//��ʾ������ֵ��ֵ
	OLED_ShowNum(5*16, 4, Sensorthreshold.Illumination_threshold, 3,16);

	//��ʾ������ֵ��ֵ
	OLED_ShowNum(5*16, 6, Sensorthreshold.Distance_threshold, 3,16);
}

/**
  * @brief  ��¼��ֵ�����°�KEY1�Ĵ���
  * @param  ��
  * @retval ���ش���
  */
uint8_t SetSelection(void)
{
	static uint8_t count = 1;
	if(KeyNum == KEY_1)
	{
		KeyNum = 0;
		count++;
		if (count >= 4)
		{
			count = 1;
		}
	}
	return count;
}

/**
  * @brief  ��ʾ��ֵ�����ѡ�����
  * @param  num Ϊ��ʾ��λ��
  * @retval ��
  */
void OLED_Option(uint8_t num)
{
	switch(num)
	{
		case 1:	
			OLED_ShowChar(0*16,0,' ');
			OLED_ShowChar(0*16,2,'>');
			OLED_ShowChar(0*16,4,' ');
			OLED_ShowChar(0*16,6,' ');
			break;
		case 2:	
			OLED_ShowChar(0*16,0,' ');
			OLED_ShowChar(0*16,2,' ');
			OLED_ShowChar(0*16,4,'>');
			OLED_ShowChar(0*16,6,' ');
			break;
		case 3:	
			OLED_ShowChar(0*16,0,' ');
			OLED_ShowChar(0*16,2,' ');
			OLED_ShowChar(0*16,4,' ');
			OLED_ShowChar(0*16,6,'>');
			break;
		default: break;
	}
}


/**
  * @brief  ����ֵ����Ĵ�������ֵ�����޸�
  * @param  num Ϊ��ǰ�û���Ҫ���ĵĴ�������ֵλ��
  * @retval ��
  */
void ThresholdModification(uint8_t num)
{
	switch (num)
	{
		case 1:
			if (KeyNum == KEY_3)
			{
				KeyNum = 0;
				OLED_Clear();
				menu = timeSettingsPage;
					
			}
			else if (KeyNum == KEY_4)
			{
				KeyNum = 0;
				OLED_Clear();
				menu = timeSettingsPage;
				
			}			
			break;		

		case 2:
			if (KeyNum == KEY_3)
			{
				KeyNum = 0;
				Sensorthreshold.Illumination_threshold += 10;
				if (Sensorthreshold.Illumination_threshold > 999)
				{
					Sensorthreshold.Illumination_threshold = 1;
				}
			}
			else if (KeyNum == KEY_4)
			{
				KeyNum = 0;
				Sensorthreshold.Illumination_threshold -= 10;
				if (Sensorthreshold.Illumination_threshold < 1)
				{
					Sensorthreshold.Illumination_threshold = 999;
				}				
			}			
			break;
		case 3:
			if (KeyNum == KEY_3)
			{
				KeyNum = 0;
				Sensorthreshold.Distance_threshold++;
				if (Sensorthreshold.Distance_threshold > 99)
				{
					Sensorthreshold.Distance_threshold = 1;
				}
			}
			else if (KeyNum == KEY_4)
			{
				KeyNum = 0;
				Sensorthreshold.Distance_threshold--;
				if (Sensorthreshold.Distance_threshold < 1)
				{
					Sensorthreshold.Distance_threshold = 99;
				}				
			}
			break;
		default: break;		
	}
}



/**
  * @brief  ��ȡ������������
  * @param  ��
  * @retval ��
  */
void sensorScan(void)
{
		DHT11_Read_TempAndHumidity(&DHT11_Data);
		HC_SR04_Deboanle(&sensorData.distance);
		LDR_LuxData(&sensorData.lux);
//		HC_SR501_Input(&sensorData.people);		
}


int _last_Buzzer_flag=0;
int _Distance_list[10];
void auto_buzzer(){
	int sum=0;
	for (int i=0;i<10;i++){
		sum+=_Distance_list[i];
	}
	
	 int _distance=_distance=sum/10;
		if(_distance<Sensorthreshold.Distance_threshold ){//��������ֵ����������
			GPIO_SetBits(Buzzer_PROT, Buzzer);}
		else{	
			GPIO_ResetBits(Buzzer_PROT, Buzzer);}	
}

void auto_led(){
	int i =sensorData.lux;		
	if (i>Sensorthreshold.Illumination_threshold){//���ո�������ֵ�رչ���
		TIM_SetCompare1(TIM1, 0);
		ledDutyRatio=0;
	}	else{
		if (100-i>=0){//���յ���100����,���򲻴���
			int y=((100-i)/10)*10;
			TIM_SetCompare1(TIM1, y);
			ledDutyRatio=y;}
		else{//���ո���100������
			TIM_SetCompare1(TIM1, 0);
			ledDutyRatio=0;
		}
	}		
}
//����
void usart2_init(u32 bound)
{
  //GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);	//ʹ��USART2
	
	//USART2_TX   GPIOA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.2

	//USART1_RX	  GPIOA.3��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.3  

	//Usart2 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

	//USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART2, &USART_InitStructure); //��ʼ������2	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�������ڽ��ܺͷ����ж�
	USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ���2 
}



void floatToString(float value, char *str, int precision) {
    sprintf(str, "%.*f", precision, value);
}


int main(void)
{

	SystemInit(); // ȷ��ϵͳʱ�ӳ�ʼ��
	RCC_HSEConfig(RCC_HSE_ON); // �����ⲿ���پ���
	while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET); // �ȴ������ȶ�
	RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9); // ����PLLΪ9��Ƶ
	RCC_PLLCmd(ENABLE); // ʹ��PLL
	while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET); // �ȴ�PLL�ȶ�
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); // ѡ��PLL��Ϊϵͳʱ��
	int a = 0;
	
	delay_init();
	OLED_Init();
	OLED_Clear();
  OLED_ShowString(1, 1, (uint8_t *)"OLED_Init OK");    
    

	Key_Init();
	OLED_Clear();
	OLED_ShowString(1, 1, (uint8_t *)"Key_Init OK");
	AD_Init();
	PWM_Init(100 - 1, 720 - 1);
	Timer2_Init(9,14398);

	
	
	LDR_Init();
	OLED_Clear();
		 OLED_ShowString(1, 1, (uint8_t *)"LDR_Init OK");
	
	
	DHT11_Init();
	OLED_Clear();
		 OLED_ShowString(1, 1, (uint8_t *)"DHT11_Init OK");
	
	_LED_Init();
	OLED_Clear();
	 OLED_ShowString(1, 1, (uint8_t *)"LED_Init OK");
	

	HC_SR04_Init();
	Buzzer_Init();


	Sensorthreshold.Illumination_threshold = FLASH_R(FLASH_START_ADDR);	//��ָ��ҳ�ĵ�ַ��FLASH
	Sensorthreshold.Distance_threshold = FLASH_R(FLASH_START_ADDR+2);	//��ָ��ҳ�ĵ�ַ��FLASH
	
	GENERAL_TIM_Init();
	

OLED_Clear();
OLED_ShowString(1, 1, (uint8_t *)"System Iinit OK");
	

	int ii=0;
	
	usart2_init(115200);
	
	
	while (1)
	{
		

	
		IWDG_ReloadCounter(); //���¼��ؼ���ֵ ι��
		sensorScan();	//��ȡ����������
		 _Distance_list[ii]=sensorData.distance;
		ii+=1;
		if (ii>=10){
			ii=0;
		};
		auto_buzzer();
		
		switch (menu)
		{
			case display_page:
				OLED_Clear();
				OLED_Menu();	//��ʾ��ҳ��Ĺ̶�����
			  OLED_Menu_SensorData();	//��ʾ��ҳ�洫�������ݡ�ϵͳģʽ������
				if (!systemModel)
				{
					LED_PWM_KEY();	//��������LED��PWM			
				}else{	
					auto_led();	
					clude=0;					
				}
				//�л�ϵͳģʽ
				if (KeyNum == KEY_1)
				{
					KeyNum = 0;
					systemModel = ~systemModel;
					if (systemModel)
					{
						currentDataPoint.valueModel = 1;
					}
					else
					{
						currentDataPoint.valueModel = 0;
					}
				}				
				
				//�ж��Ƿ������ֵ���ý���
				if (KeyNum == KEY_Long1)
				{
					KeyNum = 0;
					OLED_Clear();	//����
					menu = settingsPage;	//��ת����ֵ���ý���
				}
				break;
			case settingsPage:
				OLED_SetInterfacevoid();	//��ʾ��ֵ���ý���Ĺ̶�����
				OLED_Option(SetSelection());	//ʵ����ֵ����ҳ���ѡ����
				ThresholdModification(SetSelection());	//ʵ����ֵ���ڹ���	
			
				//�ж��Ƿ��˳���ֵ���ý���
				if (KeyNum == KEY_2)
				{
					KeyNum = 0;
					OLED_Clear();	//����
					menu = display_page;	//��ת��������

					//�洢�޸ĵĴ�������ֵ��flash��				
					FLASH_W(FLASH_START_ADDR, Sensorthreshold.Illumination_threshold, Sensorthreshold.Distance_threshold);
					currentDataPoint.valueIllumination_threshold = Sensorthreshold.Illumination_threshold;
					currentDataPoint.valueDistance_threshold = Sensorthreshold.Distance_threshold;
				}
				break;

		}	

					
  memset(payload, 0, sizeof(payload));
  sprintf(payload, "TEM=%.2f|HUM=%.2f|DIS=%.2f|LDR=%.2f|", 
          DHT11_Data.temperature, 
          DHT11_Data.humidity, 
          (float)sensorData.distance,
          (float)sensorData.lux);  

  // 3. ��������
 

//  // 4. ͳһ��ʱ
//  delay_ms(2000);
		
  
        	
	}
}

