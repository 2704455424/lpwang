#include "exti.h"
#include "motor.h"
#include "switch.h"
#include "usart1.h"

/*  �ⲿ�жϳ�ʼ��  */
void EXTIX_Init(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure; 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);								//ʹ�ܸ��ù���ʱ��

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource11);				//�ж����Լ��жϳ�ʼ������   �½��ش���
	EXTI_InitStructure.EXTI_Line = EXTI_Line11;	
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	 																	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource12);				//�ж����Լ��жϳ�ʼ������   �½��ش���
	EXTI_InitStructure.EXTI_Line = EXTI_Line12;	
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	 																	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;									//ʹ���ⲿ�ж�ͨ��
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;					//��ռ���ȼ�1 
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;								//�����ȼ�0 
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;										//ʹ���ⲿ�ж�ͨ��
  NVIC_Init(&NVIC_InitStructure);  	  															//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
}


/*  �ⲿ�жϴ�����  */
void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line11) != RESET)	{
		EXTI_ClearITPendingBit(EXTI_Line11);
		
		if(LightSwitchState == 0)	{
			StopMotor();
		}
		u1_printf("��ʼ��������\r\n");
	}
	
	if(EXTI_GetITStatus(EXTI_Line12) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line12);
		
		if(LightSwitchState != 0)	{
			StopMotor();
		}
		u1_printf("ֹͣ��������\r\n");
	}
}
