#include "tim4.h"
#include "motor.h"

//开启TIM4中断向量
void TIM4_NVIC_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//设置抢占和子优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

//定时器时间计算公式Tout = ((重装载值+1)*(预分频系数+1))/时钟频率;
//例如：1秒定时，重装载值=9999，预分频系数=7199
void TIM4_Init(u16 arr,u16 psc)																		//TIM4 初始化 arr重装载值 psc预分频系数
{  
	TIM_TimeBaseInitTypeDef	TIM_TimeBaseInitStrue;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);							//使能TIM4
					
	TIM_TimeBaseInitStrue.TIM_Period=arr;								 					 	//设置自动重装载值
	TIM_TimeBaseInitStrue.TIM_Prescaler=psc; 												//预分频系数
	TIM_TimeBaseInitStrue.TIM_CounterMode=TIM_CounterMode_Up; 			//计数器向上溢出
	TIM_TimeBaseInitStrue.TIM_ClockDivision=TIM_CKD_DIV1; 					//时钟的分频因子，起到了一点点的延时作用，一般设为TIM_CKD_DIV1
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStrue);									//TIM4初始化设置
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);											//使能TIM4中断    
	TIM_Cmd(TIM4,DISABLE); 																					//不使能TIM4

	TIM4_NVIC_Init (); 																							//开启TIM4中断向量
}


/*   TIM4中断处理函数   */
void TIM4_IRQHandler(void)																	
{	
	if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET){	
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);							//清除TIM4溢出中断标志
		
		TurnMotor();
	}		
}


