#include "led.h"

  
//��ʼ��PB5��PE5Ϊ�����.��ʹ���������ڵ�ʱ��		    
//LED IO��ʼ��
void LED_Init(void)
{
 	GPIO_InitTypeDef  GPIO_InitStructure; 	
	RCC_APB2PeriphClockCmd(LED_GPIO_CLOCK, ENABLE);	 //ʹ��PB,PE�˿�ʱ��
	
	GPIO_InitStructure.GPIO_Pin = LED_PB5;				 //LED0-->PB.5 �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	    		 //LED1-->PE.5 �˿�����, �������
	GPIO_Init(GPIOE, &GPIO_InitStructure);	  				 //������� ��IO���ٶ�Ϊ50MHz

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




