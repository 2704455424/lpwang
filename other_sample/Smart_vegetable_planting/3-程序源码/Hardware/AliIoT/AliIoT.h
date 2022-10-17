#ifndef _ALIIOT_H
#define _ALIIOT_H
#include "stm32f10x.h"


void AliIoT_Link(void);							//���Ӱ�����
void AliIoTDriver(void);						//����������
void AliIoT_DealCmd(void);					//������ı�������
void AliIoT_SubData(void);					//������һ�η�������
void PingMonitor(void);			  			//PING���ļ��
void PingRxMonitor(u8 ms);					//Ping���ռ�ʱ���
void PingTimeCount(u16 ms);					//PING����
void SetPingTime(u16 timeout);			//PING��ʱx��

extern u8 PingCmd;


#endif

