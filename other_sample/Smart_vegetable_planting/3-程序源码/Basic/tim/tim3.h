#ifndef  __TIM3_H
#define  __TIM3_H
#include "stm32f10x.h"

extern u8 flag10s;										//1s��ʱ��־
extern u8 flag1min;										//1min��ʱ��־	

void TIM3_NVIC_Init (void);						//��ʱ��3�жϳ�ʼ��
void TIM3_Init(u16 arr,u16 psc);			//��ʱ��3��ʼ��


#endif
