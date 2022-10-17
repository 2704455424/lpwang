#include "motor.h"
#include "switch.h"

s32 beats = 0;  										//电机转动节拍总数

/*  检测引脚初始化  */
void CHECK_GPIO_Init(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure; 
 	RCC_APB2PeriphClockCmd(CHECK_GPIO_CLOCK,ENABLE);					//使能时钟

	GPIO_InitStructure.GPIO_Pin  = CHECK_PIN_1|CHECK_PIN_2;		//电机检测引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 						//设置成上拉输入
 	GPIO_Init(CHECK_PORT, &GPIO_InitStructure);								//设置初始化
}



/*  电机自检  */
void Motor_Check(void)
{
	LightSwitchState = 0;							//默认关闭遮阳棚
	
	CHECK_GPIO_Init();								//初始化引脚	
	if(MOTOR_STAT_STATUS != 0){				//若开始引脚不为0，电机默认打开状态
		StartMotor(-360);
	}

}

/*  电机初始化  */
void Motor_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure; 	
	RCC_APB2PeriphClockCmd(MOTOR_GPIO_CLOCK, ENABLE);	 		//使能端口时钟	
	
	GPIO_InitStructure.GPIO_Pin = MOTOR_PIN_1|MOTOR_PIN_2|MOTOR_PIN_3|MOTOR_PIN_4;				 //端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 	//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 	//IO口速度为50MHz
	GPIO_Init(MOTOR_PORT, &GPIO_InitStructure);	  		 		//推挽输出 ，IO口速度为50MHz
}

/* 步进电机启动函数，angle-需转过的角度 */
void StartMotor(s32 angle)
{
  beats = (angle * 4076) / 360; 												//实测为4076拍转动一圈
	
	TIM_Cmd(MOTOR_TIM, ENABLE);                        		//关闭定时器
	TIM_SetCounter(MOTOR_TIM, 0);                        	//清零计数器
}


/* 步进电机停止函数 */
void StopMotor(void)
{
	beats = 0;
	
	MOTOR_OFF();																					//停止电机
	TIM_Cmd(MOTOR_TIM, DISABLE);                        	//关闭定时器
	TIM_SetCounter(MOTOR_TIM, 0);                        	//清零计数器
}


/* 电机转动控制函数 */
void TurnMotor(void)
{
	static u8 index = 0;  		//节拍输出索引
  u8 BeatCode[8] = {  			//步进电机节拍对应的IO控制代码
    0xE, 0xC, 0xD, 0x9, 0xB, 0x3, 0x7, 0x6
  };
    
  if (beats != 0)  															//节拍数不为0则产生一个驱动节拍
  {
		if (beats > 0)  														//节拍数大于0时正转
    {
      index++;               										//正转时节拍输出索引递增
      index = index & 0x07;  										//用&操作实现到8归零
      beats--;               										//正转时节拍计数递减
    }
    else            														//节拍数小于0时反转
    {
      index--;               										//反转时节拍输出索引递减
      index = index & 0x07;  										//用&操作同样可以实现到-1时归7
      beats++;               										//反转时节拍计数递增
    }	

		MOTOR_OUT1((BeatCode[index])&0x1);					//设置对应的GPIO输出
		MOTOR_OUT2((BeatCode[index]>>1)&0x1);
		MOTOR_OUT3((BeatCode[index]>>2)&0x1);
		MOTOR_OUT4((BeatCode[index]>>3)&0x1);
  }
  else  																				//节拍数为0则关闭电机所有的相
  {
    MOTOR_OFF();																//停止电机
  }
}




