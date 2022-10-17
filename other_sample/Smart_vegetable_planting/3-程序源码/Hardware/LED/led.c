#include "led.h"

  
//初始化PB5和PE5为输出口.并使能这两个口的时钟		    
//LED IO初始化
void LED_Init(void)
{
 	GPIO_InitTypeDef  GPIO_InitStructure; 	
	RCC_APB2PeriphClockCmd(LED_GPIO_CLOCK, ENABLE);	 //使能PB,PE端口时钟
	
	GPIO_InitStructure.GPIO_Pin = LED_PB5;				 //LED0-->PB.5 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	    		 //LED1-->PE.5 端口配置, 推挽输出
	GPIO_Init(GPIOE, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz

	LED_OFF(0);
	LED_OFF(1);
}
 

void LED_ON(u8 Ledx)
{
	switch(Ledx)
	{
		case 0: GPIO_ResetBits(LED_PORT_B, LED_PB5); break;
		case 1: GPIO_ResetBits(LED_PORT_E, LED_PE5); break;
		default: break;
	}
}

void LED_OFF(u8 Ledx)
{
	switch(Ledx)
	{
		case 0: GPIO_SetBits(LED_PORT_B, LED_PB5); break;
		case 1: GPIO_SetBits(LED_PORT_E, LED_PE5); break;
		default: break;
	}
}




