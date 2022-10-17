#ifndef __SWITCH_H
#define __SWITCH_H
#include "stm32f10x.h"

#define ON										(BitAction)(1)									//���忪��״̬
#define OFF										(BitAction)(0)									//����ر�״̬

#define TempPin								GPIO_Pin_0											//�¶ȿ���
#define HumiPin								GPIO_Pin_1											//ʪ�ȿ���
#define SoilPin								GPIO_Pin_8											//��������
#define CarbonPin							GPIO_Pin_9											// CO2����
#define SWITCH_PORT						GPIOB
#define SWITCH_GPIO_CLOCK			RCC_APB2Periph_GPIOB

#define TempSwitchState				GPIO_ReadOutputDataBit(SWITCH_PORT, TempPin)
#define HumiSwitchState				GPIO_ReadOutputDataBit(SWITCH_PORT, HumiPin)
#define SoilSwitchState				GPIO_ReadOutputDataBit(SWITCH_PORT, SoilPin)
#define CarbonSwitchState			GPIO_ReadOutputDataBit(SWITCH_PORT, CarbonPin)

extern u8 LightSwitchState;

typedef enum{
	SwitchTemp,
	SwitchHumi,
	SwitchSoil,
	SwitchLight,
	SwitchCarbon	
}SwitchSelect;

void MotorCheck(void);
void Motor_Init(void);
void Switch_Init(void);
void Switch_SetState(SwitchSelect sw, BitAction state);

#endif
