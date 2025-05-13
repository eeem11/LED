#ifndef _TRANSFER_H_
#define _TRANSFER_H_

#include "stm32f10x.h"                  // Device header
#include "Delay.h"


extern char payload[128];
extern int clude;//默认本地，1为云记录
void usart2_send(unsigned char *data,unsigned char len);
//void USART2_IRQHandler(void) ;
void receive_usart2_handle();

void heartbat1();
void heartbat2();

	
#endif
