/*********************************************************************************************
*
* �������ƣ�	������ʪ�ȣ�4G�汾��
* ��д���ߣ�  ������
* ��дʱ�䣺	2020��2��8��
* Ӳ��֧�֣�	STM32F103C8   �ⲿ����8MHz RCC����������Ƶ72MHz��
* ��    ����  v1.0.0
* ��ע��Ϣ������
*
*
*********************************************************************************************/

#include "stm32f10x.h"							        //������Ҫ��ͷ�ļ�
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
	RCC_Configuration();								//RCCʱ�ӳ�ʼ��
	NVIC_Configuration();								//NVIC�жϹ����ʼ��
	ADC_Configuration();								//ADC��ʼ��
	JTAG_Configuration(0);							//JTAGʹ�ܹر�

	
	Usart1_Init(115200);								//����1��ʼ��
	Usart2_Init(115200);								//����2��ʼ��	
	TIM2_Init(500,7199);							  //��ʱ��2��ʼ��, ��ʱ50ms
	TIM3_Init(10000,7199);							//��ʱ��3��ʼ��, ��ʱ1s
	TIM4_Init(2000,71);									//��ʱ��4��ʼ��, ��ʱ2ms
	Switch_Init();											//���س�ʼ��
	IIC_Init();													//��ʼ��IIC�ӿ�
	Motor_Init();												//�����ʼ��
	Motor_Check();											//�����ʼ�����
	EXTIX_Init();												//�ⲿ�жϳ�ʼ��
	AHT10_Init();												//��ʼ����ʪ�ȴ�����AHT10
	BH1750_Init();											//��ʼ������ǿ�ȴ�����BH1750		
	CAT1_Init();												//��ʼ��CAT1ģ��
	while(CAT1_ConnectServer());				//CA1���ӷ�����
	DATA_Init();												//��ʼ���������
	PubInitData();											//������ʼ������
	
	while(1)
	{
		AliDriver();											//����������	
		SecMonitor();											//����		
	}
}

