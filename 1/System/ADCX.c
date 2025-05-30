#include "adcx.h"

/**
  * @brief  ADC初始化程序
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
	while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET); //等待转换完成
	return ADC_GetConversionValue(ADC1);
}

void _ADCX_Init(void)
{
	//开启ADC时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	
	//ADC频率进行6分频
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	//配置ADC结构体
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//独立模式
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//数据右对齐
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//软件触发
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//单次转换
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//非扫描模式
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//总通道数
	ADC_Init(ADC1, &ADC_InitStructure);	//初始化ADC1
	
	//开启ADC1
	ADC_Cmd(ADC1, ENABLE);
	
	//进行ADC校准
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1) == SET);
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1) == SET);	
}

/**
  * @brief  获取ADC转换后的数据
  * @param  ADC_Channel 	选择需要采集的ADC通道
  * @param  ADC_SampleTime  选择需要采样时间
  * @retval 返回转换后的模拟信号数值
  */
u16 _ADC_GetValue(uint8_t ADC_Channel,uint8_t ADC_SampleTime)
{
	//配置ADC通道
	ADC_RegularChannelConfig(ADC1, ADC_Channel, 1, ADC_SampleTime);
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE); //软件触发ADC转换
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET); //读取ADC转换完成标志位
	return ADC_GetConversionValue(ADC1);
}
