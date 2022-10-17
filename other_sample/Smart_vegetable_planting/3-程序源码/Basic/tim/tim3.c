#include "tim3.h"
#include "led.h"

u8 flag10s = 0;								//10s��ʱ��־
u8 flag1min = 0;							//1min��ʱ��־

//����TIM3�ж�����
void TIM3_NVIC_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//������ռ�������ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

//��ʱ��ʱ����㹫ʽTout = ((��װ��ֵ+1)*(Ԥ��Ƶϵ��+1))/ʱ��Ƶ��;
//���磺1�붨ʱ����װ��ֵ=9999��Ԥ��Ƶϵ��=7199

void TIM3_Init(u16 arr,u16 psc)														//TIM3 ��ʼ�� arr��װ��ֵ pscԤ��Ƶϵ��
{  
    TIM_TimeBaseInitTypeDef     TIM_TimeBaseInitStrue;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);		//ʹ��TIM3
    	      
    TIM_TimeBaseInitStrue.TIM_Period=arr;								 					 	//�����Զ���װ��ֵ
    TIM_TimeBaseInitStrue.TIM_Prescaler=psc; 												//Ԥ��Ƶϵ��
    TIM_TimeBaseInitStrue.TIM_CounterMode=TIM_CounterMode_Up; 			//�������������
    TIM_TimeBaseInitStrue.TIM_ClockDivision=TIM_CKD_DIV1; 					//ʱ�ӵķ�Ƶ���ӣ�����һ������ʱ���ã�һ����ΪTIM_CKD_DIV1
    TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStrue);									//TIM3��ʼ������
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);											//ʹ��TIM3�ж�    
    TIM_Cmd(TIM3,ENABLE); 																					//ʹ��TIM3
	
		TIM3_NVIC_Init (); 																							//����TIM3�ж�����
}


/*   TIM3�жϴ�����   */
void TIM3_IRQHandler(void)																	
{	
	static u8 cnt = 0;
	static u8 cnt10s = 0;
	
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET){	
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);							//���TIM3����жϱ�־
		
		LED_TOGGLE();
		
		cnt++;
		if(cnt>=10){
			cnt = 1;
			flag10s = 1;
		
			cnt10s++;
			if(cnt10s >= 6){
				cnt10s = 0;
				flag1min = 1;
			}
		}
	}		
}


