#ifndef __MONITOR_H
#define __MONITOR_H
#include "stm32f10x.h"


#define CarbonValue				  2000 										//定义最大采集的二氧化碳浓度为 2000 ppm
#define SoilValue						99											//定义最大采集的二氧化碳浓度为   99 %

#define DEF_SwitchMode			0												//定义开关模式，0：自动 1：手动
#define DEF_UploadTime 			1										  	//定义数据上传周期，单位：分钟
#define DEF_MaxLight				5000										//默认光照最大报警阈值
#define DEF_MaxCarbon				1000									  //默认 CO2最大报警阈值
#define DEF_MinTemp					10											//默认温度最小报警阈值
#define DEF_MinHumi					40											//默认湿度最小报警阈值
#define DEF_MinSoil					35											//默认土壤最小报警阈值

#define CHECK_ADDR					255											//是否第一次启动地址检测
#define MinTemp_ADDR				0												//最小温度存储地址
#define MinHumi_ADDR				4												//最小湿度存储地址
#define MinSoil_ADDR				8												//最小土壤存储地址
#define MaxLight_ADDR				12											//最大光照存储地址
#define MaxCarbon_ADDR			16											//最大 CO2存储地址
#define UploadTime_ADDR			20											//上传时间存储地址
#define SwitchMode_ADDR     24											//开关模式地址


typedef struct 																		//定义环境数据结构体，存放环境数据
{	
	volatile float temp;
	volatile float humi;
	volatile float soil;
	volatile u16 carbon;
	volatile u16 illumi;
}EnvirData;


typedef struct 														//定义环境数据阈值结构体，存放环境数据
{	
  volatile u8 Temp_Low_Threshold;
  volatile u8 Humi_Low_Threshold;
	volatile u8 Soil_Low_Threshold;
	volatile u16 Light_Max_Threshold;
	volatile u16 Carbon_Max_Threshold;
}EnvirThreshold;


void DATA_Init(void);											//初始化相关系统数据
void PubInitData(void);										//发布初始化数据
void SecMonitor(void);										//秒监控
void DhtxxMonitor(void);									//DHTxx监控
void SoilMonitor(void);										//土壤湿度监控
void CarbonMonitor(void);									//二氧化碳监控
void illumiMonitor(void);									//光照强度监控
void SwitchMonitor(void);									//开关监控

#endif
