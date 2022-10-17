#include "alidata.h"
#include "cat1.h"
#include "delay.h"
#include "usart1.h"
#include "switch.h"


/**
	* @brief  CAT1接收处理阿里云数据命令
	* @param  None
	* @retval None
  */
//void DealAliCmd(void)
//{
//	
//}



/**
	* @brief  CAT1连接阿里云的数据接收发送驱动
	* @param  None
	* @retval None
  */
void AliDriver(void)
{
	
	if(RxDataOutPtr != RxDataInPtr){       											//if成立的话，说明接收缓冲区有数据了														
		u1_printf("接收的数据:\r\n%s\r\n",RxDataOutPtr+2);        //串口提示数据	
		delay_ms(200);                                      			//延时		
		DealAliCmd();

		RxDataOutPtr += RBUFF_UNIT;               								//指针下移
		if(RxDataOutPtr==RxDataEndPtr)     												//如果指针到缓冲区尾部了
			RxDataOutPtr = RxDataBuf[0];   													//指针归位到缓冲区开头  
	}
	
	if(TxDataOutPtr != TxDataInPtr){       											//if成立的话，说明接收缓冲区有数据了	
		u1_printf("发布的数据:\r\n%s\r\n",TxDataOutPtr);        	//串口提示数据			                          			
		if(CAT1_Publish(TxDataOutPtr, 50) != 0) Connect_flag = 0;	//发送失败，重连网络
		TxDataOutPtr += TBUFF_UNIT;               								//指针下移
		if(TxDataOutPtr==TxDataEndPtr)     												//如果指针到缓冲区尾部了
			TxDataOutPtr = TxDataBuf[0];   													//指针归位到缓冲区开头  
		delay_ms(200);    																				//延时		    
	}
}

