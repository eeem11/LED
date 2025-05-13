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

#define FLASH_START_ADDR	0x0801f000	//写入的起始地址

volatile uint8_t rx_buffer[256];
volatile uint16_t rx_index = 0;
volatile uint8_t response_received = 0;


uint8_t systemModel = 0;				//存储系统当前模式
uint8_t menu = 1;					//显示菜单变量



DHT11_Data_TypeDef DHT11_Data;	//声明传感器数据结构体变量
SensorModules sensorData;	//声明传感器数据结构体变量
SensorThresholdValue Sensorthreshold;	//声明传感器阈值结构体变量
//上云
//u16 USART2_RX_STA=0;       //接收状态标记	  
//u8 USART2_RX_BUF[200];     //接收缓冲,最大USART_REC_LEN个字节.
//char Rec_Data[255]=""; 

enum 
{
	display_page = 1,
	settingsPage,
	timeSettingsPage
	
}MenuPages;

//已完成菜单
void OLED_Menu(void)
{
	
	OLED_ShowCHinese(0*16,2, 0);
	OLED_ShowCHinese(1*16,2, 1);

	//显示系统当前模式 手动模式 or 自动模式
		if (systemModel)
		{
			//显示“自动”
			OLED_ShowCHinese(0*8, 0*8, 9);
			OLED_ShowCHinese(2*8, 0*8, 10);					
		}
		else
		{
			//显示“手动”
			OLED_ShowCHinese(0*8, 0*8, 11);
			OLED_ShowCHinese(2*8, 0*8, 12);			
		}
	//显示“温度：”
	OLED_ShowCHinese(0*16,2, 0);
	OLED_ShowCHinese(1*16,2, 1);
	
	//显示“湿度：”
	OLED_ShowCHinese(4*16,2, 2);
	OLED_ShowCHinese(5*16,2, 1);
		
	
	//显示“光强：”
	OLED_ShowCHinese(0*16,4, 4);
	OLED_ShowCHinese(1*16,4, 5);
	
	//显示“距离：”
	OLED_ShowCHinese(4*16,4, 17);
	OLED_ShowCHinese(5*16,4, 18);
	
		
	//显示“亮度：”
	OLED_ShowCHinese(0*16,6, 6);
	OLED_ShowCHinese(1*16,6, 1);
	
}

///**
//  * @brief  显示主页面的传感器数据等信息
//  * @param  无
//  * @retval 无
//  */
void OLED_Menu_SensorData(void)
{
	
		//显示温度数据
		OLED_ShowNum(2*16, 2, DHT11_Data.temperature, 3,16);
		OLED_ShowChar(7*8, 2, 'C');	

		//显示湿度数据
		OLED_ShowNum(6*16, 2, DHT11_Data.humidity, 3,16);	
		OLED_ShowChar(15*8, 2, '%');	

		//显示光强数据
		OLED_ShowNum(2*16, 4, sensorData.lux, 3,16);	

		//显示距离数据
		OLED_ShowNum(6*16, 4, sensorData.distance, 3,16);

		//显示亮度等级
		OLED_ShowNum(2*16, 6, ledDutyRatio, 3,16);
		OLED_ShowChar(7*8, 6, '%');	
	

		
}
/**
  * @brief  显示系统设置界面
  * @param  无
  * @retval 无
  */
void OLED_SetInterfacevoid(void)
{
	//显示“系统设置界面”
	OLED_ShowCHinese(1*16, 0*16, 20);
	OLED_ShowCHinese(2*16, 0*16, 21);
	OLED_ShowCHinese(3*16, 0*16, 22);
	OLED_ShowCHinese(4*16, 0*16, 23);
	OLED_ShowCHinese(5*16, 0*16, 24);
	OLED_ShowCHinese(6*16, 0*16, 25);	
	
	
	//显示“光照阈值”
	OLED_ShowCHinese(1*16, 4, 4);
	OLED_ShowCHinese(2*16, 4, 5);	
	OLED_ShowCHinese(3*16, 4, 26);
	OLED_ShowCHinese(4*16, 4, 27);	
	

	//显示”距离阈值“
	OLED_ShowCHinese(1*16, 6, 17);
	OLED_ShowCHinese(2*16, 6, 18);	
	OLED_ShowCHinese(3*16, 6, 26);
	OLED_ShowCHinese(4*16, 6, 27);	
	
	
	//显示光照阈值数值
	OLED_ShowNum(5*16, 4, Sensorthreshold.Illumination_threshold, 3,16);

	//显示距离阈值数值
	OLED_ShowNum(5*16, 6, Sensorthreshold.Distance_threshold, 3,16);
}

/**
  * @brief  记录阈值界面下按KEY1的次数
  * @param  无
  * @retval 返回次数
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
  * @brief  显示阈值界面的选择符号
  * @param  num 为显示的位置
  * @retval 无
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
  * @brief  对阈值界面的传感器阈值进行修改
  * @param  num 为当前用户需要更改的传感器阈值位置
  * @retval 无
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
  * @brief  获取传感器的数据
  * @param  无
  * @retval 无
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
		if(_distance<Sensorthreshold.Distance_threshold ){//低于设置值开启蜂鸣器
			GPIO_SetBits(Buzzer_PROT, Buzzer);}
		else{	
			GPIO_ResetBits(Buzzer_PROT, Buzzer);}	
}

void auto_led(){
	int i =sensorData.lux;		
	if (i>Sensorthreshold.Illumination_threshold){//光照高于设置值关闭光照
		TIM_SetCompare1(TIM1, 0);
		ledDutyRatio=0;
	}	else{
		if (100-i>=0){//光照低于100补偿,否则不处理
			int y=((100-i)/10)*10;
			TIM_SetCompare1(TIM1, y);
			ledDutyRatio=y;}
		else{//光照高于100不补偿
			TIM_SetCompare1(TIM1, 0);
			ledDutyRatio=0;
		}
	}		
}
//上云
void usart2_init(u32 bound)
{
  //GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//使能GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);	//使能USART2
	
	//USART2_TX   GPIOA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.2

	//USART1_RX	  GPIOA.3初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.3  

	//Usart2 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

	//USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART2, &USART_InitStructure); //初始化串口2	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启串口接受和发送中断
	USART_Cmd(USART2, ENABLE);                    //使能串口2 
}



void floatToString(float value, char *str, int precision) {
    sprintf(str, "%.*f", precision, value);
}


int main(void)
{

	SystemInit(); // 确保系统时钟初始化
	RCC_HSEConfig(RCC_HSE_ON); // 开启外部高速晶振
	while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET); // 等待晶振稳定
	RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9); // 配置PLL为9倍频
	RCC_PLLCmd(ENABLE); // 使能PLL
	while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET); // 等待PLL稳定
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); // 选择PLL作为系统时钟
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


	Sensorthreshold.Illumination_threshold = FLASH_R(FLASH_START_ADDR);	//从指定页的地址读FLASH
	Sensorthreshold.Distance_threshold = FLASH_R(FLASH_START_ADDR+2);	//从指定页的地址读FLASH
	
	GENERAL_TIM_Init();
	

OLED_Clear();
OLED_ShowString(1, 1, (uint8_t *)"System Iinit OK");
	

	int ii=0;
	
	usart2_init(115200);
	
	
	while (1)
	{
		

	
		IWDG_ReloadCounter(); //重新加载计数值 喂狗
		sensorScan();	//获取传感器数据
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
				OLED_Menu();	//显示主页面的固定内容
			  OLED_Menu_SensorData();	//显示主页面传感器数据、系统模式等内容
				if (!systemModel)
				{
					LED_PWM_KEY();	//按键控制LED的PWM			
				}else{	
					auto_led();	
					clude=0;					
				}
				//切换系统模式
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
				
				//判断是否进入阈值设置界面
				if (KeyNum == KEY_Long1)
				{
					KeyNum = 0;
					OLED_Clear();	//清屏
					menu = settingsPage;	//跳转到阈值设置界面
				}
				break;
			case settingsPage:
				OLED_SetInterfacevoid();	//显示阈值设置界面的固定内容
				OLED_Option(SetSelection());	//实现阈值设置页面的选择功能
				ThresholdModification(SetSelection());	//实现阈值调节功能	
			
				//判断是否退出阈值设置界面
				if (KeyNum == KEY_2)
				{
					KeyNum = 0;
					OLED_Clear();	//清屏
					menu = display_page;	//跳转到主界面

					//存储修改的传感器阈值至flash内				
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

  // 3. 发送数据
 

//  // 4. 统一延时
//  delay_ms(2000);
		
  
        	
	}
}

