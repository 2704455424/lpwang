#include "alidata.h"
#include "cat1.h"
#include "delay.h"
#include "usart1.h"
#include "switch.h"


/**
	* @brief  CAT1���մ���������������
	* @param  None
	* @retval None
  */
//void DealAliCmd(void)
//{
//	
//}



/**
	* @brief  CAT1���Ӱ����Ƶ����ݽ��շ�������
	* @param  None
	* @retval None
  */
void AliDriver(void)
{
	
	if(RxDataOutPtr != RxDataInPtr){       											//if�����Ļ���˵�����ջ�������������														
		u1_printf("���յ�����:\r\n%s\r\n",RxDataOutPtr+2);        //������ʾ����	
		delay_ms(200);                                      			//��ʱ		
		DealAliCmd();

		RxDataOutPtr += RBUFF_UNIT;               								//ָ������
		if(RxDataOutPtr==RxDataEndPtr)     												//���ָ�뵽������β����
			RxDataOutPtr = RxDataBuf[0];   													//ָ���λ����������ͷ  
	}
	
	if(TxDataOutPtr != TxDataInPtr){       											//if�����Ļ���˵�����ջ�������������	
		u1_printf("����������:\r\n%s\r\n",TxDataOutPtr);        	//������ʾ����			                          			
		if(CAT1_Publish(TxDataOutPtr, 50) != 0) Connect_flag = 0;	//����ʧ�ܣ���������
		TxDataOutPtr += TBUFF_UNIT;               								//ָ������
		if(TxDataOutPtr==TxDataEndPtr)     												//���ָ�뵽������β����
			TxDataOutPtr = TxDataBuf[0];   													//ָ���λ����������ͷ  
		delay_ms(200);    																				//��ʱ		    
	}
}

