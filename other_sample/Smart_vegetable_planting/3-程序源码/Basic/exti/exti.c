#include "exti.h"
#include "motor.h"
#include "switch.h"
#include "usart1.h"

/*  外部中断初始化  */
void EXTIX_Init(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure; 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);								//使能复用功能时钟

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource11);				//中断线以及中断初始化配置   下降沿触发
	EXTI_InitStructure.EXTI_Line = EXTI_Line11;	
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	 																	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource12);				//中断线以及中断初始化配置   下降沿触发
	EXTI_InitStructure.EXTI_Line = EXTI_Line12;	
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	 																	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;									//使能外部中断通道
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;					//抢占优先级1 
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;								//子优先级0 
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;										//使能外部中断通道
  NVIC_Init(&NVIC_InitStructure);  	  															//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
}


/*  外部中断处理函数  */
void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line11) != RESET)	{
		EXTI_ClearITPendingBit(EXTI_Line11);
		
		if(LightSwitchState == 0)	{
			StopMotor();
		}
		u1_printf("起始点检测引脚\r\n");
	}
	
	if(EXTI_GetITStatus(EXTI_Line12) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line12);
		
		if(LightSwitchState != 0)	{
			StopMotor();
		}
		u1_printf("停止点检测引脚\r\n");
	}
}
