#ifndef __MOTOR_H
#define __MOTOR_H
#include "stm32f10x.h"

#define MOTOR_TIM									TIM4								//������ʹ�ö�ʱ��

#define OneRound_RD								360									//����������ת��һȦ
#define OneRound_LD								-360								//����������ת��һȦ

#define MOTOR_PIN_1								GPIO_Pin_12
#define MOTOR_PIN_2								GPIO_Pin_13
#define MOTOR_PIN_3								GPIO_Pin_14
#define MOTOR_PIN_4								GPIO_Pin_15
#define MOTOR_PORT								GPIOB
#define MOTOR_GPIO_CLOCK					RCC_APB2Periph_GPIOB

#define CHECK_PIN_1								GPIO_Pin_11
#define CHECK_PIN_2								GPIO_Pin_12
#define CHECK_PORT								GPIOA
#define CHECK_GPIO_CLOCK					RCC_APB2Periph_GPIOA

#define MOTOR_OUT1(x)							GPIO_WriteBit(MOTOR_PORT, MOTOR_PIN_1, (BitAction)(x))
#define MOTOR_OUT2(x)							GPIO_WriteBit(MOTOR_PORT, MOTOR_PIN_2, (BitAction)(x))
#define MOTOR_OUT3(x)							GPIO_WriteBit(MOTOR_PORT, MOTOR_PIN_3, (BitAction)(x))
#define MOTOR_OUT4(x)							GPIO_WriteBit(MOTOR_PORT, MOTOR_PIN_4, (BitAction)(x))
#define MOTOR_OFF()								GPIO_SetBits(MOTOR_PORT, MOTOR_PIN_1|MOTOR_PIN_2|MOTOR_PIN_3|MOTOR_PIN_4)								

#define MOTOR_STAT_STATUS					GPIO_ReadInputDataBit(CHECK_PORT, CHECK_PIN_1)
#define MOTOR_STOP_STATUS					GPIO_ReadInputDataBit(CHECK_PORT, CHECK_PIN_2)


void Motor_Init(void);									//��ʼ�������������
void Motor_Check(void);									//�����ʼ�����
void TurnMotor(void);										//���ת�����ƺ���
void StopMotor(void);										//���ֹͣ����
void StartMotor(s32 angle);							//�����ʼ������������ת���Ƕ�

#endif
