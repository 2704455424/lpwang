#include "tim.h"
#include "keyboard.h"

u8 flag200ms = 0;          // 200ms标志位
u8 flag1s = 0;        	   // 200ms标志位


//定时器时间计算公式Tout = ((重装载值+1)*(预分频系数+1))/时钟频率;
//例如：1秒定时，重装载值=9999，预分频系数=7199

void TIM3_Init(u16 arr,u16 psc){  //TIM3 初始化 arr重装载值 psc预分频系数
    TIM_TimeBaseInitTypeDef     TIM_TimeBaseInitStrue;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);//使能TIM3
    TIM3_NVIC_Init (); //开启TIM3中断向量
	      
    TIM_TimeBaseInitStrue.TIM_Period=arr; //设置自动重装载值
    TIM_TimeBaseInitStrue.TIM_Prescaler=psc; //预分频系数
    TIM_TimeBaseInitStrue.TIM_CounterMode=TIM_CounterMode_Up; //计数器向上溢出
    TIM_TimeBaseInitStrue.TIM_ClockDivision=TIM_CKD_DIV1; //时钟的分频因子，起到了一点点的延时作用，一般设为TIM_CKD_DIV1
    TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStrue); //TIM3初始化设置
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);//使能TIM3中断    
    TIM_Cmd(TIM3,ENABLE); //使能TIM3
}

void TIM3_NVIC_Init (void){ //开启TIM3中断向量
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x3;	//设置抢占和子优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void TIM3_IRQHandler(void){ //TIM3中断处理函数
	static u8 tmr200ms = 0;     // 200ms计数
	static u8 tmr1s = 0;
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET){	//判断是否是TIM3中断
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
				
		KeyScan();							// 按键扫描
		
		tmr200ms++;
		if(tmr200ms >= 200)			// 定时200ms到, 标志位置1
		{
			tmr200ms = 0;
			flag200ms = 1;
			
			tmr1s++;
			if(tmr1s >= 5)				// 定时1s到, 标志位置1, 200ms加一次
			{
				tmr1s = 0;
				flag1s = 1;
			}
		}
		
  }
}


