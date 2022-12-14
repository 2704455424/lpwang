#include "tim3.h"
#include "led.h"

u8 flag10s = 0;								//10s定时标志
u8 flag1min = 0;							//1min定时标志

//开启TIM3中断向量
void TIM3_NVIC_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//设置抢占和子优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

//定时器时间计算公式Tout = ((重装载值+1)*(预分频系数+1))/时钟频率;
//例如：1秒定时，重装载值=9999，预分频系数=7199

void TIM3_Init(u16 arr,u16 psc)														//TIM3 初始化 arr重装载值 psc预分频系数
{  
    TIM_TimeBaseInitTypeDef     TIM_TimeBaseInitStrue;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);		//使能TIM3
    	      
    TIM_TimeBaseInitStrue.TIM_Period=arr;								 					 	//设置自动重装载值
    TIM_TimeBaseInitStrue.TIM_Prescaler=psc; 												//预分频系数
    TIM_TimeBaseInitStrue.TIM_CounterMode=TIM_CounterMode_Up; 			//计数器向上溢出
    TIM_TimeBaseInitStrue.TIM_ClockDivision=TIM_CKD_DIV1; 					//时钟的分频因子，起到了一点点的延时作用，一般设为TIM_CKD_DIV1
    TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStrue);									//TIM3初始化设置
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);											//使能TIM3中断    
    TIM_Cmd(TIM3,ENABLE); 																					//使能TIM3
	
		TIM3_NVIC_Init (); 																							//开启TIM3中断向量
}


/*   TIM3中断处理函数   */
void TIM3_IRQHandler(void)																	
{	
	static u8 cnt = 0;
	static u8 cnt10s = 0;
	
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET){	
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);							//清除TIM3溢出中断标志
		
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


