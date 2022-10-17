#include "tim2.h"
#include "cat1.h"


/*  ��ʱ��2��ʼ��  */
void TIM2_Init(u16 arr, u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;            //����һ�����ö�ʱ���ı���
	NVIC_InitTypeDef NVIC_InitStructure;                          //����һ�������жϵı���
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);               //�����ж��������飺��2�� �������ȼ���0 1 2 3 �����ȼ���0 1 2 3		
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);           //ʹ��TIM2ʱ��	
  TIM_TimeBaseInitStructure.TIM_Period = arr; 	                //�����Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;                  //���ö�ʱ��Ԥ��Ƶ��
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;     //1��Ƶ
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);            //����TIM2
	
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);                    //�������жϱ�־λ
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);                      //ʹ��TIM2����ж�    
	TIM_Cmd(TIM2,DISABLE);                                        //�ȹر�TIM2                          
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn;                 //����TIM2�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;       //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;              //�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;                 //�ж�ͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);                               //�����ж�
}


void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET){           	//���TIM_IT_Update��λ����ʾTIM2����жϣ�����if	
		memcpy(&RxDataInPtr[2],Usart2_RxBuff,Usart2_RxCounter);  		//�������ݵ����ջ�����
		RxDataInPtr[0] = Usart2_RxCounter/256;                   		//��¼���ݳ��ȸ��ֽ�
		RxDataInPtr[1] = Usart2_RxCounter%256;                   		//��¼���ݳ��ȵ��ֽ�
		RxDataInPtr+=RBUFF_UNIT;                                    //ָ������
		if(RxDataInPtr == RxDataEndPtr)                            	//���ָ�뵽������β����
			RxDataInPtr = RxDataBuf[0];                          			//ָ���λ����������ͷ
		Usart2_RxCounter = 0;                                    		//����2������������������
		TIM_Cmd(TIM2, DISABLE);                        		     			//�ر�TIM2��ʱ��
		TIM_SetCounter(TIM2, 0);                        		 				//����TIM2������
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);     		 				//���TIM2����жϱ�־ 	
	}
}


