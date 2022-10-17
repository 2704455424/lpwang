#ifndef __LED_H
#define __LED_H	 
#include "stm32f10x.h"

#define LED_PB5								GPIO_Pin_5
#define LED_PE5								GPIO_Pin_5
#define LED_PORT_B						GPIOB
#define LED_PORT_E						GPIOE
#define LED_GPIO_CLOCK				RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOE

#define LED_TOGGLE()					GPIO_WriteBit(LED_PORT_B, LED_PB5, (BitAction)(1-GPIO_ReadOutputDataBit(LED_PORT_B, LED_PB5)))


void LED_Init(void);									//初始化
void LED_ON(u8 Ledx);									//打开LEDx
void LED_OFF(u8 Ledx);								//关闭LEDx
void SetNetLed(void);	
		 				    
#endif
