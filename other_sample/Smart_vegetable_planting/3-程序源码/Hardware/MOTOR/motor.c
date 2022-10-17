#include "motor.h"
#include "switch.h"

s32 beats = 0;  										//���ת����������

/*  ������ų�ʼ��  */
void CHECK_GPIO_Init(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure; 
 	RCC_APB2PeriphClockCmd(CHECK_GPIO_CLOCK,ENABLE);					//ʹ��ʱ��

	GPIO_InitStructure.GPIO_Pin  = CHECK_PIN_1|CHECK_PIN_2;		//����������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 						//���ó���������
 	GPIO_Init(CHECK_PORT, &GPIO_InitStructure);								//���ó�ʼ��
}



/*  ����Լ�  */
void Motor_Check(void)
{
	LightSwitchState = 0;							//Ĭ�Ϲر�������
	
	CHECK_GPIO_Init();								//��ʼ������	
	if(MOTOR_STAT_STATUS != 0){				//����ʼ���Ų�Ϊ0�����Ĭ�ϴ�״̬
		StartMotor(-360);
	}

}

/*  �����ʼ��  */
void Motor_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure; 	
	RCC_APB2PeriphClockCmd(MOTOR_GPIO_CLOCK, ENABLE);	 		//ʹ�ܶ˿�ʱ��	
	
	GPIO_InitStructure.GPIO_Pin = MOTOR_PIN_1|MOTOR_PIN_2|MOTOR_PIN_3|MOTOR_PIN_4;				 //�˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 	//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 	//IO���ٶ�Ϊ50MHz
	GPIO_Init(MOTOR_PORT, &GPIO_InitStructure);	  		 		//������� ��IO���ٶ�Ϊ50MHz
}

/* �����������������angle-��ת���ĽǶ� */
void StartMotor(s32 angle)
{
  beats = (angle * 4076) / 360; 												//ʵ��Ϊ4076��ת��һȦ
	
	TIM_Cmd(MOTOR_TIM, ENABLE);                        		//�رն�ʱ��
	TIM_SetCounter(MOTOR_TIM, 0);                        	//���������
}


/* �������ֹͣ���� */
void StopMotor(void)
{
	beats = 0;
	
	MOTOR_OFF();																					//ֹͣ���
	TIM_Cmd(MOTOR_TIM, DISABLE);                        	//�رն�ʱ��
	TIM_SetCounter(MOTOR_TIM, 0);                        	//���������
}


/* ���ת�����ƺ��� */
void TurnMotor(void)
{
	static u8 index = 0;  		//�����������
  u8 BeatCode[8] = {  			//����������Ķ�Ӧ��IO���ƴ���
    0xE, 0xC, 0xD, 0x9, 0xB, 0x3, 0x7, 0x6
  };
    
  if (beats != 0)  															//��������Ϊ0�����һ����������
  {
		if (beats > 0)  														//����������0ʱ��ת
    {
      index++;               										//��תʱ���������������
      index = index & 0x07;  										//��&����ʵ�ֵ�8����
      beats--;               										//��תʱ���ļ����ݼ�
    }
    else            														//������С��0ʱ��ת
    {
      index--;               										//��תʱ������������ݼ�
      index = index & 0x07;  										//��&����ͬ������ʵ�ֵ�-1ʱ��7
      beats++;               										//��תʱ���ļ�������
    }	

		MOTOR_OUT1((BeatCode[index])&0x1);					//���ö�Ӧ��GPIO���
		MOTOR_OUT2((BeatCode[index]>>1)&0x1);
		MOTOR_OUT3((BeatCode[index]>>2)&0x1);
		MOTOR_OUT4((BeatCode[index]>>3)&0x1);
  }
  else  																				//������Ϊ0��رյ�����е���
  {
    MOTOR_OFF();																//ֹͣ���
  }
}




