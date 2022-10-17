#include "switch.h"
#include "monitor.h"
	
u8 LightSwitchState;
/*  开关初始化  */
void Switch_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure; 	
	RCC_APB2PeriphClockCmd(SWITCH_GPIO_CLOCK, ENABLE);	 
		
	GPIO_InitStructure.GPIO_Pin = TempPin|HumiPin|SoilPin|CarbonPin;				//开关 Switch 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 										//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 										//IO口速度为50MHz
	GPIO_Init(SWITCH_PORT, &GPIO_InitStructure);					

	GPIO_ResetBits(SWITCH_PORT, TempPin|HumiPin|SoilPin|CarbonPin);					//所有开关置为低电平	
}

/*  设置开关状态  */
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

