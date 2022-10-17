#ifndef __CAT1_H
#define __CAT1_H
#include "stm32f10x.h"
#include "usart2.h"

#define CAT1_PORT								GPIOA											//定义CAT1端口组
#define CAT1_RST_Pin						GPIO_Pin_1								//定义RST引脚
#define CAT1_GPIO_CLOCK				  RCC_APB2Periph_GPIOA			//定义端口

#define CAT1_RstSet(x)    			GPIO_WriteBit(CAT1_PORT, CAT1_RST_Pin, (BitAction)x)  //PA1控制CAT1的复位


#define CAT1_printf       			u2_printf           			//串口2控制 CAT1
#define CAT1_RxCounter    			Usart2_RxCounter    			//串口2控制 CAT1
#define CAT1_RX_BUF       			Usart2_RxBuff       			//串口2控制 CAT1
#define CAT1_RXBUFF_SIZE  			USART2_RXBUFF_SIZE  			//串口2控制 CAT1

#define SERVERIP								"a1XsfLUnjwy.iot-as-mqtt.cn-shanghai.aliyuncs.com"
#define SERVERPORT							1883

#define PRODUCTKEY           		"a1XsfLUnjwy"                                 						//产品ID
#define DEVICENAME           		"ZHDP001"                                        					//设备名  
#define DEVICESECRE          		"014dbe1ac3570b3d90c172021ac08e16"            						//设备秘钥 
#define S_TOPIC_NAME         		"/sys/a1XsfLUnjwy/ZHDP001/thing/service/property/set"   	//需要订阅的主题  
#define P_TOPIC_NAME         		"/sys/a1XsfLUnjwy/ZHDP001/thing/event/property/post"    	//需要发布的主题 

#define R_NUM               		5     										//接收缓冲区个数
#define RBUFF_UNIT          		500   										//接收缓冲区长度

#define T_NUM               		5     										//发送缓冲区个数  
#define TBUFF_UNIT          		500   										//发送缓冲区长度


extern char Connect_flag;  																//外部变量声明，同服务器连接状态  0：还没有连接服务器  1：连接上服务器了
extern char Send_flag;     																//1：发送数据

extern unsigned char  RxDataBuf[R_NUM][RBUFF_UNIT];   		//接收数据缓冲区，10个
extern unsigned char *RxDataInPtr;            						//外部变量声明，指向缓冲区存放数据的位置
extern unsigned char *RxDataOutPtr;           						//外部变量声明，指向缓冲区读取数据的位置
extern unsigned char *RxDataEndPtr;           						//外部变量声明，指向缓冲区结束的位置

extern unsigned char  TxDataBuf[T_NUM][TBUFF_UNIT];   		//接收数据缓冲区，10个
extern unsigned char *TxDataInPtr;            						//外部变量声明，指向缓冲区存放数据的位置
extern unsigned char *TxDataOutPtr;           						//外部变量声明，指向缓冲区读取数据的位置
extern unsigned char *TxDataEndPtr;           						//外部变量声明，指向缓冲区结束的位置

void CAT1_DataBuff_Init(void);														//数据缓冲区初始化
void CAT1_Init(void);																			//CAT1初始化
void CAT1_SubData(void);																	//订阅Topic
void CAT1_ResetIO_Init(void);															//CAT1复位引脚初始化
char CAT1_Publish(u8 *data, int timeout);									//CAT1发布数据
void CAT1_PushData(char *topic, char *data);							//将需要发送的缓存区装进缓冲区
char CAT1_SendCmd(char *cmd, int timeout);								//CAT1发送命令
char CAT1_ConnectServer(void);														//CAT1连接服务器
char CAT1_CAT1_CPIN_Check(void);													//SIM卡插入检测
char CAT1_CGATT_Check(void);															//网络附着检测
char CAT1_CIFSR_Check(void);															//检测获取的IP
int CAT1_GetCSQ(void);																		//获取信号质量

#endif



