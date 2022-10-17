#include "tim2.h"
#include "cat1.h"


/*  定时器2初始化  */
void TIM2_Init(u16 arr, u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;            //定义一个设置定时器的变量
	NVIC_InitTypeDef NVIC_InitStructure;                          //定义一个设置中断的变量
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);               //设置中断向量分组：第2组 抢先优先级：0 1 2 3 子优先级：0 1 2 3		
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);           //使能TIM2时钟	
  TIM_TimeBaseInitStructure.TIM_Period = arr; 	                //设置自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;                  //设置定时器预分频数
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;     //1分频
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);            //设置TIM2
	
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);                    //清除溢出中断标志位
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);                      //使能TIM2溢出中断    
	TIM_Cmd(TIM2,DISABLE);                                        //先关闭TIM2                          
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn;                 //设置TIM2中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;       //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;              //子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;                 //中断通道使能
	NVIC_Init(&NVIC_InitStructure);                               //设置中断
}


void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET){           	//如果TIM_IT_Update置位，表示TIM2溢出中断，进入if	
		memcpy(&RxDataInPtr[2],Usart2_RxBuff,Usart2_RxCounter);  		//拷贝数据到接收缓冲区
		RxDataInPtr[0] = Usart2_RxCounter/256;                   		//记录数据长度高字节
		RxDataInPtr[1] = Usart2_RxCounter%256;                   		//记录数据长度低字节
		RxDataInPtr+=RBUFF_UNIT;                                    //指针下移
		if(RxDataInPtr == RxDataEndPtr)                            	//如果指针到缓冲区尾部了
			RxDataInPtr = RxDataBuf[0];                          			//指针归位到缓冲区开头
		Usart2_RxCounter = 0;                                    		//串口2接收数据量变量清零
		TIM_Cmd(TIM2, DISABLE);                        		     			//关闭TIM2定时器
		TIM_SetCounter(TIM2, 0);                        		 				//清零TIM2计数器
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);     		 				//清除TIM2溢出中断标志 	
	}
}


