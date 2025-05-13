#include "adcx.h"

/**
  * @brief  ADC��ʼ������
  * @param  
  * @retval 
  */
	void AD_Init() {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
	
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent    ;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);
	
	ADC_Cmd(ADC1, ENABLE);
	
	ADC_ResetCalibration(ADC1);
	while (ADC_GetResetCalibrationStatus(ADC1) == SET);
	ADC_StartCalibration(ADC1);
	while (ADC_GetCalibrationStatus(ADC1) == SET);
}

uint16_t AD_GetValue(void) {
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET); //�ȴ�ת�����
	return ADC_GetConversionValue(ADC1);
}

void _ADCX_Init(void)
{
	//����ADCʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	
	//ADCƵ�ʽ���6��Ƶ
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	//����ADC�ṹ��
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//����ģʽ
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//�����Ҷ���
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//�������
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//����ת��
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//��ɨ��ģʽ
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//��ͨ����
	ADC_Init(ADC1, &ADC_InitStructure);	//��ʼ��ADC1
	
	//����ADC1
	ADC_Cmd(ADC1, ENABLE);
	
	//����ADCУ׼
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1) == SET);
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1) == SET);	
}

/**
  * @brief  ��ȡADCת���������
  * @param  ADC_Channel 	ѡ����Ҫ�ɼ���ADCͨ��
  * @param  ADC_SampleTime  ѡ����Ҫ����ʱ��
  * @retval ����ת�����ģ���ź���ֵ
  */
u16 _ADC_GetValue(uint8_t ADC_Channel,uint8_t ADC_SampleTime)
{
	//����ADCͨ��
	ADC_RegularChannelConfig(ADC1, ADC_Channel, 1, ADC_SampleTime);
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE); //�������ADCת��
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET); //��ȡADCת����ɱ�־λ
	return ADC_GetConversionValue(ADC1);
}
