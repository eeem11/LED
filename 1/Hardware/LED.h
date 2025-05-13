#ifndef	_LED_H_
#define	_LED_H_

#include "stm32f10x.h"                  // Device header
#include "key.h" 
#include "gizwits_product.h"
#include "sensormodules.h"
#include "tim2.h"

#define LED				GPIO_Pin_8
#define LED_PROT		GPIOA

#define LED_PWM_UPPER_LIMIT			100		//LED������ֵ������Ϊ100%

extern uint8_t ledDutyRatio;	

void LED_Init(void);
void _LED_Init(void);
void LED_PWM_KEY(void);
//void LED_APP_Command(void);
extern void LED_PWM_JiZhiYun_Mode(void);
#endif
