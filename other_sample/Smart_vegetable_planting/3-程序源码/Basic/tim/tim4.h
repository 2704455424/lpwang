#ifndef  __TIM4_H
#define  __TIM4_H
#include "stm32f10x.h"


void TIM4_NVIC_Init (void);						//定时器4中断初始化
void TIM4_Init(u16 arr,u16 psc);			//定时器4初始化

#endif

