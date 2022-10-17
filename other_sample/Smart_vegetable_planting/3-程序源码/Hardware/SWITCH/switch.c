#include "switch.h"
#include "monitor.h"
	
u8 LightSwitchState;
/*  ���س�ʼ��  */
void Switch_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure; 	
	RCC_APB2PeriphClockCmd(SWITCH_GPIO_CLOCK, ENABLE);	 
		
	GPIO_InitStructure.GPIO_Pin = TempPin|HumiPin|SoilPin|CarbonPin;				//���� Switch �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 										//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 										//IO���ٶ�Ϊ50MHz
	GPIO_Init(SWITCH_PORT, &GPIO_InitStructure);					

	GPIO_ResetBits(SWITCH_PORT, TempPin|HumiPin|SoilPin|CarbonPin);					//���п�����Ϊ�͵�ƽ	
}

/*  ���ÿ���״̬  */
void Switch_SetState(SwitchSelect sw, BitAction state)
{
	switch(sw)
	{
		case SwitchTemp: 		GPIO_WriteBit(SWITCH_PORT, TempPin, state); 	break;
		case SwitchHumi: 		GPIO_WriteBit(SWITCH_PORT, HumiPin, state); 	break;
		case SwitchSoil: 		GPIO_WriteBit(SWITCH_PORT, SoilPin, state); 	break;
		case SwitchCarbon: 	GPIO_WriteBit(SWITCH_PORT, CarbonPin, state); break;
		default: break;
	}
}

