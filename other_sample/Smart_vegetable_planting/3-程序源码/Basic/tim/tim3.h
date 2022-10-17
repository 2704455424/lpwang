#ifndef  __TIM3_H
#define  __TIM3_H
#include "stm32f10x.h"

extern u8 flag10s;										//1s定时标志
extern u8 flag1min;										//1min定时标志	

void TIM3_NVIC_Init (void);						//定时器3中断初始化
void TIM3_Init(u16 arr,u16 psc);			//定时器3初始化


#endif
