#ifndef __WIFI_H
#define __WIFI_H
#include "stm32f10x.h"
#include "usart2.h"	    //������Ҫ��ͷ�ļ�

#define RESET_IO(x)    				GPIO_WriteBit(GPIOA, GPIO_Pin_1, (BitAction)x)  //PA4����WiFi�ĸ�λ

#define WiFi_printf       		u2_printf           //����2���� WiFi
#define WiFi_RxCounter    		Usart2_RxCounter    //����2���� WiFi
#define WiFi_RX_BUF       		Usart2_RxBuff       //����2���� WiFi
#define WiFi_RXBUFF_SIZE  		USART2_RXBUFF_SIZE  //����2���� WiFi

#define SSID   "12#116"                    		 			//·����SSID����
#define PASS   "12dong116"               						//·��������

extern u16 WIFI_MODE;																//����ģʽ 0��SSID������д�ڳ�����   1��Smartconfig��ʽ��APP����

void WiFi_ResetIO_Init(void);
char WiFi_SendCmd(char *cmd, int timeout);
char WiFi_Reset(int timeout);
char WiFi_JoinAP(int timeout);
char WiFi_Connect_Server(int timeout);
char WiFi_Smartconfig(int timeout);
char WiFi_WaitAP(int timeout);
char WiFi_Connect_IoTServer(void);


#endif


