/*********************************************************************************************
*
* 程序名称：	大棚温湿度（4G版本）
* 编写作者：  姬明杨
* 编写时间：	2020年2月8日
* 硬件支持：	STM32F103C8   外部晶振8MHz RCC函数设置主频72MHz　
* 版    本：  v1.0.0
* 备注信息：　　
*
*
*********************************************************************************************/

#include "stm32f10x.h"							        //加入需要的头文件
#include "sys.h"
#include "adc.h"
#include "led.h"
#include "exti.h"
#include "tim2.h"
#include "tim3.h"
#include "tim4.h"
#include "cat1.h"
#include "delay.h"
#include "aht10.h"
#include "motor.h"
#include "switch.h"
#include "bh1750.h"
#include "usart1.h"
#include "usart2.h"
#include "alidata.h"
#include "monitor.h"


int main(void)
{
	RCC_Configuration();								//RCC时钟初始化
	NVIC_Configuration();								//NVIC中断管理初始化
	ADC_Configuration();								//ADC初始化
	JTAG_Configuration(0);							//JTAG使能关闭

	
	Usart1_Init(115200);								//串口1初始化
	Usart2_Init(115200);								//串口2初始化	
	TIM2_Init(500,7199);							  //定时器2初始化, 定时50ms
	TIM3_Init(10000,7199);							//定时器3初始化, 定时1s
	TIM4_Init(2000,71);									//定时器4初始化, 定时2ms
	Switch_Init();											//开关初始化
	IIC_Init();													//初始化IIC接口
	Motor_Init();												//电机初始化
	Motor_Check();											//电机初始化检测
	EXTIX_Init();												//外部中断初始化
	AHT10_Init();												//初始化温湿度传感器AHT10
	BH1750_Init();											//初始化光照强度传感器BH1750		
	CAT1_Init();												//初始化CAT1模块
	while(CAT1_ConnectServer());				//CA1连接服务器
	DATA_Init();												//初始化相关数据
	PubInitData();											//发布初始化数据
	
	while(1)
	{
		AliDriver();											//阿里云驱动	
		SecMonitor();											//秒监控		
	}
}

