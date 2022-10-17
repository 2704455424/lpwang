#ifndef __CAT1_H
#define __CAT1_H
#include "stm32f10x.h"
#include "usart2.h"

#define CAT1_PORT								GPIOA											//����CAT1�˿���
#define CAT1_RST_Pin						GPIO_Pin_1								//����RST����
#define CAT1_GPIO_CLOCK				  RCC_APB2Periph_GPIOA			//����˿�

#define CAT1_RstSet(x)    			GPIO_WriteBit(CAT1_PORT, CAT1_RST_Pin, (BitAction)x)  //PA1����CAT1�ĸ�λ


#define CAT1_printf       			u2_printf           			//����2���� CAT1
#define CAT1_RxCounter    			Usart2_RxCounter    			//����2���� CAT1
#define CAT1_RX_BUF       			Usart2_RxBuff       			//����2���� CAT1
#define CAT1_RXBUFF_SIZE  			USART2_RXBUFF_SIZE  			//����2���� CAT1

#define SERVERIP								"a1XsfLUnjwy.iot-as-mqtt.cn-shanghai.aliyuncs.com"
#define SERVERPORT							1883

#define PRODUCTKEY           		"a1XsfLUnjwy"                                 						//��ƷID
#define DEVICENAME           		"ZHDP001"                                        					//�豸��  
#define DEVICESECRE          		"014dbe1ac3570b3d90c172021ac08e16"            						//�豸��Կ 
#define S_TOPIC_NAME         		"/sys/a1XsfLUnjwy/ZHDP001/thing/service/property/set"   	//��Ҫ���ĵ�����  
#define P_TOPIC_NAME         		"/sys/a1XsfLUnjwy/ZHDP001/thing/event/property/post"    	//��Ҫ���������� 

#define R_NUM               		5     										//���ջ���������
#define RBUFF_UNIT          		500   										//���ջ���������

#define T_NUM               		5     										//���ͻ���������  
#define TBUFF_UNIT          		500   										//���ͻ���������


extern char Connect_flag;  																//�ⲿ����������ͬ����������״̬  0����û�����ӷ�����  1�������Ϸ�������
extern char Send_flag;     																//1����������

extern unsigned char  RxDataBuf[R_NUM][RBUFF_UNIT];   		//�������ݻ�������10��
extern unsigned char *RxDataInPtr;            						//�ⲿ����������ָ�򻺳���������ݵ�λ��
extern unsigned char *RxDataOutPtr;           						//�ⲿ����������ָ�򻺳�����ȡ���ݵ�λ��
extern unsigned char *RxDataEndPtr;           						//�ⲿ����������ָ�򻺳���������λ��

extern unsigned char  TxDataBuf[T_NUM][TBUFF_UNIT];   		//�������ݻ�������10��
extern unsigned char *TxDataInPtr;            						//�ⲿ����������ָ�򻺳���������ݵ�λ��
extern unsigned char *TxDataOutPtr;           						//�ⲿ����������ָ�򻺳�����ȡ���ݵ�λ��
extern unsigned char *TxDataEndPtr;           						//�ⲿ����������ָ�򻺳���������λ��

void CAT1_DataBuff_Init(void);														//���ݻ�������ʼ��
void CAT1_Init(void);																			//CAT1��ʼ��
void CAT1_SubData(void);																	//����Topic
void CAT1_ResetIO_Init(void);															//CAT1��λ���ų�ʼ��
char CAT1_Publish(u8 *data, int timeout);									//CAT1��������
void CAT1_PushData(char *topic, char *data);							//����Ҫ���͵Ļ�����װ��������
char CAT1_SendCmd(char *cmd, int timeout);								//CAT1��������
char CAT1_ConnectServer(void);														//CAT1���ӷ�����
char CAT1_CAT1_CPIN_Check(void);													//SIM��������
char CAT1_CGATT_Check(void);															//���總�ż��
char CAT1_CIFSR_Check(void);															//����ȡ��IP
int CAT1_GetCSQ(void);																		//��ȡ�ź�����

#endif



