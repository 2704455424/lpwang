#ifndef __MONITOR_H
#define __MONITOR_H
#include "stm32f10x.h"


#define CarbonValue				  2000 										//�������ɼ��Ķ�����̼Ũ��Ϊ 2000 ppm
#define SoilValue						99											//�������ɼ��Ķ�����̼Ũ��Ϊ   99 %

#define DEF_SwitchMode			0												//���忪��ģʽ��0���Զ� 1���ֶ�
#define DEF_UploadTime 			1										  	//���������ϴ����ڣ���λ������
#define DEF_MaxLight				5000										//Ĭ�Ϲ�����󱨾���ֵ
#define DEF_MaxCarbon				1000									  //Ĭ�� CO2��󱨾���ֵ
#define DEF_MinTemp					10											//Ĭ���¶���С������ֵ
#define DEF_MinHumi					40											//Ĭ��ʪ����С������ֵ
#define DEF_MinSoil					35											//Ĭ��������С������ֵ

#define CHECK_ADDR					255											//�Ƿ��һ��������ַ���
#define MinTemp_ADDR				0												//��С�¶ȴ洢��ַ
#define MinHumi_ADDR				4												//��Сʪ�ȴ洢��ַ
#define MinSoil_ADDR				8												//��С�����洢��ַ
#define MaxLight_ADDR				12											//�����մ洢��ַ
#define MaxCarbon_ADDR			16											//��� CO2�洢��ַ
#define UploadTime_ADDR			20											//�ϴ�ʱ��洢��ַ
#define SwitchMode_ADDR     24											//����ģʽ��ַ


typedef struct 																		//���廷�����ݽṹ�壬��Ż�������
{	
	volatile float temp;
	volatile float humi;
	volatile float soil;
	volatile u16 carbon;
	volatile u16 illumi;
}EnvirData;


typedef struct 														//���廷��������ֵ�ṹ�壬��Ż�������
{	
  volatile u8 Temp_Low_Threshold;
  volatile u8 Humi_Low_Threshold;
	volatile u8 Soil_Low_Threshold;
	volatile u16 Light_Max_Threshold;
	volatile u16 Carbon_Max_Threshold;
}EnvirThreshold;


void DATA_Init(void);											//��ʼ�����ϵͳ����
void PubInitData(void);										//������ʼ������
void SecMonitor(void);										//����
void DhtxxMonitor(void);									//DHTxx���
void SoilMonitor(void);										//����ʪ�ȼ��
void CarbonMonitor(void);									//������̼���
void illumiMonitor(void);									//����ǿ�ȼ��
void SwitchMonitor(void);									//���ؼ��

#endif
