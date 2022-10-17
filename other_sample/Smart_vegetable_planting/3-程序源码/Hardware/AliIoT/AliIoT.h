#ifndef _ALIIOT_H
#define _ALIIOT_H
#include "stm32f10x.h"


void AliIoT_Link(void);							//连接阿里云
void AliIoTDriver(void);						//阿里云驱动
void AliIoT_DealCmd(void);					//处理阿里的报文命令
void AliIoT_SubData(void);					//开机第一次发布数据
void PingMonitor(void);			  			//PING报文监控
void PingRxMonitor(u8 ms);					//Ping接收计时监控
void PingTimeCount(u16 ms);					//PING计数
void SetPingTime(u16 timeout);			//PING定时x秒

extern u8 PingCmd;


#endif

