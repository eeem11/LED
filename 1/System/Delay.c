#include "delay.h"
#include "stm32f10x.h"                  // Device header




//初始化延迟函数
//当使用OS的时候,此函数会初始化OS的时钟节拍
//配置等待时间单位为1ms
void delay_init(void)
{
	if(SysTick_Config(SystemCoreClock / 1000))//systick中断配置，1ms进入中断一次，stm32f10x_it.c 文件中的SysTick_Handler函数中调用计数
  { 	

  }
}								    


static __IO uint32_t TimingDelay;//时间计数值
//1ms延时
void delay_ms(int  nTime)
{	 		  	  
  TimingDelay = nTime;

  while(TimingDelay != 0);
 	    
} 

//1ms被调用一次
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}



void Delay_s(int s)
{
	int i=0;
	for(;i<s;i++){
		delay_ms(1000);
	};
};



//1us等待
void Delay_us(int nus)
{
  uint32_t tickStart, tickCur, tickCnt;
  uint32_t tickMax = SysTick->LOAD;
  uint32_t udelay_value = (SysTick->LOAD/1000)*nus;

  tickStart = SysTick->VAL;
  while(1)
  {
    tickCur = SysTick->VAL;
    tickCnt = (tickStart < tickCur) ? (tickMax+tickStart-tickCur) : (tickStart-tickCur);
    if (tickCnt > udelay_value)
      break;
  }
}


