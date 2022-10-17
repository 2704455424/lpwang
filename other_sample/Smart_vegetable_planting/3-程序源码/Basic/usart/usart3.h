#ifndef __USART3_H
#define __USART3_H

#include "stdio.h"      //������Ҫ��ͷ�ļ�
#include "stdarg.h"		//������Ҫ��ͷ�ļ� 
#include "string.h"     //������Ҫ��ͷ�ļ�

#define USART3_RX_ENABLE     1      //�Ƿ������չ���  1������  0���ر�
#define USART3_TXBUFF_SIZE   1024   //���崮��2 ���ͻ�������С 1024�ֽ�

#if  USART3_RX_ENABLE                          //���ʹ�ܽ��չ���
#define USART3_RXBUFF_SIZE   1024              //���崮��2 ���ջ�������С 1024�ֽ�
extern char Usart3_RxCompleted ;               //�ⲿ�����������ļ����Ե��øñ���
extern unsigned int Usart3_RxCounter;          //�ⲿ�����������ļ����Ե��øñ���
extern char Usart3_RxBuff[USART3_RXBUFF_SIZE]; //�ⲿ�����������ļ����Ե��øñ���
#endif

void Usart3_Init(unsigned int bound);       
void u3_printf(char*,...) ;          

#endif



