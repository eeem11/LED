#include "tim2.h"                  // Device header
#include "transfer.h"


uint32_t delay20ms;
uint8_t keyDelay_50ms = 150;

void Timer2_Init(u16 Prescaler, u16 Period)	//0-65535
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = Period;
	TIM_TimeBaseInitStructure.TIM_Prescaler = Prescaler;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
	
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM2, ENABLE);
}

int times=0;//计数，用于发送标签标识心跳

void TIM2_IRQHandler(void)	//2ms定时中断
{
	times+=1;
	if (times==(5)*1000/2/2){   //(60-10)/2秒发送标签1
		 heartbat1();
	}
		if (times==(7)*1000/2){	//(60-10)秒发送标签2
		 heartbat2();
	}
	switch (times){
	case (10)*1000/2:{
		usart2_send((uint8_t*)payload, strlen(payload));
		break;
	}
		case (20)*1000/2:{
		usart2_send((uint8_t*)payload, strlen(payload));
		break;
	}
		case (30)*1000/2:{
		usart2_send((uint8_t*)payload, strlen(payload));
		break;
	}
		case (40)*1000/2:{
		usart2_send((uint8_t*)payload, strlen(payload));
		break;
	};
		case (50)*1000/2:{
		usart2_send((uint8_t*)payload, strlen(payload));
		break;
	};
		case (60)*1000/2:{
		usart2_send((uint8_t*)payload, strlen(payload));
		break;
	};
}
		
	if (times>=(60)*1000/2){
		times=0;
		}
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET ) 
	{
		Key_scan();	//按键扫描函数
		
		if (keyDelay_50ms)
		{
			keyDelay_50ms--;
			KeyNum = 0;	
		}
		
		//HC_SR04的20ms延迟
		if (HC_SR04_DelayStructure.delay20msStartFlag)
		{
			HC_SR04_DelayStructure.delay20ms++;
			if(HC_SR04_DelayStructure.delay20ms > 10)
			{
				HC_SR04_DelayStructure.delay20msStartFlag = 0;
				HC_SR04_DelayStructure.delay20msFinishFlag = 1;
			}	
		}
		//HC_SR04的100ms延迟
		if (HC_SR04_DelayStructure.delay100msStartFlag)
		{
			HC_SR04_DelayStructure.delay100ms++;
			
			if(HC_SR04_DelayStructure.delay100ms > 50)
			{
				HC_SR04_DelayStructure.delay100msStartFlag = 0;
				HC_SR04_DelayStructure.delay100msFinishFlag = 1;
			}	
		}		
		
		//机智云APP控制LED的延迟
		delay20ms++;
		if(delay20ms < 10)
		{
			delay20ms = 0;
//			LED_APP_Command();	
		}
		
//		//距离阈值报警
//		if ((currentDataPoint.valueDistance_threshold > sensorData.distance) && systemModel)
//		{
//			Buzzer_ON();
//		}
//		else
//		{
//			Buzzer_OFF();
//		}
//		
		//机智云配网按键，可以重新进行配网。	
		if((KeyNum == 44)) 
		{
			KeyNum = 0;
			gizwitsSetMode(2);
			Buzzer_ON();
			delay_ms(300);
			Buzzer_OFF();
		}		
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);				
	}	
}

