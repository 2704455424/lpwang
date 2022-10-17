#ifndef  __TIM_H
#define  __TIM_H
#include "sys.h"


extern u8 flag200ms;
extern u8 flag1s;

void TIM3_Init(u16 arr,u16 psc);
void TIM3_NVIC_Init (void);


#endif
