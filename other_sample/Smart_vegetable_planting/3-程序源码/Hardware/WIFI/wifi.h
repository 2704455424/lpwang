#ifndef __WIFI_H
#define __WIFI_H
#include "stm32f10x.h"
#include "usart2.h"	    //包含需要的头文件

#define RESET_IO(x)    				GPIO_WriteBit(GPIOA, GPIO_Pin_1, (BitAction)x)  //PA4控制WiFi的复位

#define WiFi_printf       		u2_printf           //串口2控制 WiFi
#define WiFi_RxCounter    		Usart2_RxCounter    //串口2控制 WiFi
#define WiFi_RX_BUF       		Usart2_RxBuff       //串口2控制 WiFi
#define WiFi_RXBUFF_SIZE  		USART2_RXBUFF_SIZE  //串口2控制 WiFi

#define SSID   "12#116"                    		 			//路由器SSID名称
#define PASS   "12dong116"               						//路由器密码

extern u16 WIFI_MODE;																//联网模式 0：SSID和密码写在程序里   1：Smartconfig方式用APP发送

void WiFi_ResetIO_Init(void);
char WiFi_SendCmd(char *cmd, int timeout);
char WiFi_Reset(int timeout);
char WiFi_JoinAP(int timeout);
char WiFi_Connect_Server(int timeout);
char WiFi_Smartconfig(int timeout);
char WiFi_WaitAP(int timeout);
char WiFi_Connect_IoTServer(void);


#endif


