#include "led.h"
#include "transfer.h"
uint8_t ledDutyRatio = 0;				//�洢��ǰ��LED������ֵ




//��ʱ��1ʱ������
void RCC_Configuration(void)
{
  /* TIM1 clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
}
//��ʱ��1 ͨ��1 PWM�������
void TIM1_Config_Init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

  /* ---------------------------------------------------------------
    72M��Ƶ��72��Ƶ����1us��������1000����1ms�ж�һ��
  --------------------------------------------------------------- */
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 999;//������1000�������ж�һ��
  TIM_TimeBaseStructure.TIM_Prescaler = 71;//72��Ƶ
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
  /* TIM IT enable */
  TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 500;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

  TIM_OC1Init(TIM1, &TIM_OCInitStructure);
  TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
  TIM_ARRPreloadConfig(TIM1, ENABLE);
  TIM_Cmd(TIM1, ENABLE);
	TIM_CtrlPWMOutputs(TIM1,ENABLE); //ȷ����TIM1���PWM

}
//PA8�������ã�TIM1 ͨ��1
void LED_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	RCC_Configuration();
	TIM1_Config_Init();
}
void _LED_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    // GPIO��ʼ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
 

	  
    // TIM1��ʼ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    TIM_TimeBaseStructure.TIM_Period = 100-1;
    TIM_TimeBaseStructure.TIM_Prescaler = 72-1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    // PWMͨ������
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);

    TIM_CtrlPWMOutputs(TIM1, ENABLE);
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM1, ENABLE);
    TIM_Cmd(TIM1, ENABLE);
}

void LED_PWM_KEY(void)
{
	if (KeyNum == 2)
	{
		KeyNum = 0;
		ledDutyRatio ++;
		if (ledDutyRatio >= LED_PWM_UPPER_LIMIT)
		{
			ledDutyRatio = 0;
		}
		currentDataPoint.valueLamp_brightness = ledDutyRatio;
	}
	if (KeyNum == 3)
	{
		KeyNum = 0;
		ledDutyRatio --;	
		if (ledDutyRatio >= LED_PWM_UPPER_LIMIT)
		{
			ledDutyRatio = LED_PWM_UPPER_LIMIT;
		}
		currentDataPoint.valueLamp_brightness = ledDutyRatio;
		clude=0;
	}
	if (KeyNum == 4)
	{
		KeyNum = 0;
		ledDutyRatio = 0;
		currentDataPoint.valueLamp_brightness = ledDutyRatio;
		clude=0;
	}
	if (!clude){
		TIM_SetCompare1(TIM1, ledDutyRatio);
	}
}


//void LED_APP_Command(void)
//{
//	if (!systemModel)
//	{
//		if (currentDataPoint.valueLamp_brightness < ledDutyRatio)
//		{
//			ledDutyRatio--;
//		}
//		else if (currentDataPoint.valueLamp_brightness > ledDutyRatio)
//		{
//			ledDutyRatio++;
//		}
//		TIM_SetCompare1(TIM1, ledDutyRatio);
//	}
//	else if (systemModel && sensorData.people)
//	{
//		uint16_t tempData;
//		if (currentDataPoint.valueIllumination_threshold > sensorData.lux)
//		{
//			tempData = currentDataPoint.valueIllumination_threshold - sensorData.lux;
//			if (tempData > ledDutyRatio)
//			{
//				ledDutyRatio++;	
//				if (ledDutyRatio >= LED_PWM_UPPER_LIMIT)
//				{
//					ledDutyRatio = 100;
//				}	
//			}
//			else if (tempData < ledDutyRatio)
//			{
//				ledDutyRatio--;	
//				if (ledDutyRatio >= LED_PWM_UPPER_LIMIT)
//				{
//					ledDutyRatio = 0;
//				}				
//			}
//		}
//		else if (currentDataPoint.valueIllumination_threshold < sensorData.lux) 	
//		{
//			ledDutyRatio--;	
//			if (ledDutyRatio >= LED_PWM_UPPER_LIMIT)
//			{
//				ledDutyRatio = 0;
//			}		
//		}
//		currentDataPoint.valueLamp_brightness = ledDutyRatio;
//		TIM_SetCompare1(TIM1, ledDutyRatio);
//	}
//	else if (systemModel && sensorData.people == 0)
//	{
//		ledDutyRatio = 0;
//		currentDataPoint.valueLamp_brightness = ledDutyRatio;
//		TIM_SetCompare1(TIM1, ledDutyRatio);
//	}
//		
//}


