#include "tim.h"
#include "keyboard.h"

u8 flag200ms = 0;          // 200ms��־λ
u8 flag1s = 0;        	   // 200ms��־λ


//��ʱ��ʱ����㹫ʽTout = ((��װ��ֵ+1)*(Ԥ��Ƶϵ��+1))/ʱ��Ƶ��;
//���磺1�붨ʱ����װ��ֵ=9999��Ԥ��Ƶϵ��=7199

void TIM3_Init(u16 arr,u16 psc){  //TIM3 ��ʼ�� arr��װ��ֵ pscԤ��Ƶϵ��
    TIM_TimeBaseInitTypeDef     TIM_TimeBaseInitStrue;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);//ʹ��TIM3
    TIM3_NVIC_Init (); //����TIM3�ж�����
	      
    TIM_TimeBaseInitStrue.TIM_Period=arr; //�����Զ���װ��ֵ
    TIM_TimeBaseInitStrue.TIM_Prescaler=psc; //Ԥ��Ƶϵ��
    TIM_TimeBaseInitStrue.TIM_CounterMode=TIM_CounterMode_Up; //�������������
    TIM_TimeBaseInitStrue.TIM_ClockDivision=TIM_CKD_DIV1; //ʱ�ӵķ�Ƶ���ӣ�����һ������ʱ���ã�һ����ΪTIM_CKD_DIV1
    TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStrue); //TIM3��ʼ������
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);//ʹ��TIM3�ж�    
    TIM_Cmd(TIM3,ENABLE); //ʹ��TIM3
}

void TIM3_NVIC_Init (void){ //����TIM3�ж�����
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x3;	//������ռ�������ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void TIM3_IRQHandler(void){ //TIM3�жϴ�����
	static u8 tmr200ms = 0;     // 200ms����
	static u8 tmr1s = 0;
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET){	//�ж��Ƿ���TIM3�ж�
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
				
		KeyScan();							// ����ɨ��
		
		tmr200ms++;
		if(tmr200ms >= 200)			// ��ʱ200ms��, ��־λ��1
		{
			tmr200ms = 0;
			flag200ms = 1;
			
			tmr1s++;
			if(tmr1s >= 5)				// ��ʱ1s��, ��־λ��1, 200ms��һ��
			{
				tmr1s = 0;
				flag1s = 1;
			}
		}
		
  }
}


