#include "tim4.h"
#include "motor.h"

//����TIM4�ж�����
void TIM4_NVIC_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//������ռ�������ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

//��ʱ��ʱ����㹫ʽTout = ((��װ��ֵ+1)*(Ԥ��Ƶϵ��+1))/ʱ��Ƶ��;
//���磺1�붨ʱ����װ��ֵ=9999��Ԥ��Ƶϵ��=7199
void TIM4_Init(u16 arr,u16 psc)																		//TIM4 ��ʼ�� arr��װ��ֵ pscԤ��Ƶϵ��
{  
	TIM_TimeBaseInitTypeDef	TIM_TimeBaseInitStrue;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);							//ʹ��TIM4
					
	TIM_TimeBaseInitStrue.TIM_Period=arr;								 					 	//�����Զ���װ��ֵ
	TIM_TimeBaseInitStrue.TIM_Prescaler=psc; 												//Ԥ��Ƶϵ��
	TIM_TimeBaseInitStrue.TIM_CounterMode=TIM_CounterMode_Up; 			//�������������
	TIM_TimeBaseInitStrue.TIM_ClockDivision=TIM_CKD_DIV1; 					//ʱ�ӵķ�Ƶ���ӣ�����һ������ʱ���ã�һ����ΪTIM_CKD_DIV1
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStrue);									//TIM4��ʼ������
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);											//ʹ��TIM4�ж�    
	TIM_Cmd(TIM4,DISABLE); 																					//��ʹ��TIM4

	TIM4_NVIC_Init (); 																							//����TIM4�ж�����
}


/*   TIM4�жϴ�����   */
void TIM4_IRQHandler(void)																	
{	
	if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET){	
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);							//���TIM4����жϱ�־
		
		TurnMotor();
	}		
}


