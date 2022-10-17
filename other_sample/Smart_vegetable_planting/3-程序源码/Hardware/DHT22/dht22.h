#ifndef __DHT22_H
#define __DHT22_H	 
#include "stm32f10x.h"
#include "delay.h"


#define DHT22_IO										GPIO_Pin_8								//定义IO接口
#define DHT22PORT										GPIOB											//定义IO接口
#define DHT22_GPIO_CLOCK						RCC_APB2Periph_GPIOB			//定义IO时钟


void DHT22_IO_OUT(void);
void DHT22_IO_IN(void);
void DHT22_RST(void);
u8 Dht22_Check(void); 	   
u8 Dht22_ReadBit(void); 
u8 Dht22_ReadByte(void); 
u8 DHT22_Init (void);
u8 DHT22_GetData(u16 *temp, u16 *humi);
u8 DHT11_GetData(u8 *temp, u8 *humi);

#endif
