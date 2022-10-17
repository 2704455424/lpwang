#include "AliIoT.h"
#include "tim3.h"
#include "wifi.h"
#include "mqtt.h"
#include "usart1.h"
#include "monitor.h"

u8 flagPingTX = 0;							//PING发送标志
u8 PingCmd = 0;									//发送PING报文的使能命令
u16 PingCnt = 0;								//Ping的1s计数
u16 PingTime = 0;								//Ping报文的时间间隔
u16 PingRxTime = 0;						  //Ping等待接收时间


 /* 处理阿里发布的报文命令 */
void AliIoT_DealCmd(void) 
{
	/* 在这里添加相应的内容即可 */
	SwitchIoTCmd();														//处理阿里云开关命令
}

/* 发布给阿里云数据  */
void AliIoT_SubData(void)
{
	/* 在这里添加相应的内容即可 */
	
}

/*  网络断开连接时,设置断开连接控制标志  */
void SetDisConnect(void)
{
	flag1s = 0;
	cntTime = 0;
}

/* 定时PING报文的发送时间x秒 */
void SetPingTime(u16 timeout)
{
	PingTime = timeout*1000;			//PING报文的时间间隔
	PingCnt = 0;									//Ping1s计数清0
	PingCmd = 1;									//发送PING报文的使能命令
	
	u1_printf("PING包上传周期: %ds\r\n",timeout);	
}

/* Ping计时，利用定时器中断计时 */
void PingTimeCount(u16 ms)
{
	if(PingCmd)											//允许PING定时发送
	{
		PingCnt += ms;								//PING进行定时计数
		if(PingCnt>=PingTime){				//PING包定时时间到
			PingCnt = 0;								//计数清零
			flagPingTX = 1;							//PING发送标志置1
		}
	}
	else{
		PingCnt = 0;									//停止PING进行计数清零
	}
}

/* PING接收监控 */
void PingRxMonitor(u8 ms)
{
	if(Ping_flag != 0)
	{
		PingRxTime += ms;
	}
}

/* 阿里保活PING命令监控 */
void PingMonitor(void)
{
	if(flagPingTX || (PingRxTime>2000)){											 //定时时间到,发送标志置1准备构建PING报文
		flagPingTX = 0;										 //发送标志		
		PingRxTime = 0;										 //接收计时清0
		switch(Ping_flag){                 //判断Ping_flag的状态
				case 0:												 //如果Ping_flag等于0，表示正常状态，发送Ping报文  
						MQTT_PingREQ(); 					 //添加Ping报文到发送缓冲区  
						break;
				case 1:												 //如果Ping_flag等于1，说明上一次发送到的ping报文，没有收到服务器回复，所以1没有被清除为0，可能是连接异常，我们要启动快速ping模式
						SetPingTime(2); 					 //我们将定时器6设置为2s定时,快速发送Ping报文
						MQTT_PingREQ();  					 //添加Ping报文到发送缓冲区  
						break;
				case 2:										 		 //如果Ping_flag等于2，说明还没有收到服务器回复
				case 3:				                 //如果Ping_flag等于3，说明还没有收到服务器回复
				case 4:				                 //如果Ping_flag等于4，说明还没有收到服务器回复	
						MQTT_PingREQ();  					 //添加Ping报文到发送缓冲区 
						break;
				case 5:												 //如果Ping_flag等于5，说明我们发送了多次ping，均无回复，应该是连接有问题，我们重启连接
						Connect_flag = 0;          //连接状态置0，表示断开，没连上服务器
						PingCmd = 0;;              //关TIM3 				
						break;			
		}
		Ping_flag++;   
	}
}



/* 连接阿里云 */
void AliIoT_Link(void)
{
	u1_printf("需要连接服务器\r\n");                 //串口输出信息
	TIM_Cmd(TIM2,DISABLE);                           //关闭TIM2 
	PingCmd = 0;;                          					 //关闭TIM3   
	WiFi_RxCounter=0;                                //WiFi接收数据量变量清零                        
	memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);          //清空WiFi接收缓冲区 
	if(WiFi_Connect_IoTServer()==0){   			    		 //如果WiFi连接云服务器函数返回0，表示正确，进入if
		u1_printf("建立TCP连接成功\r\n");            	 //串口输出信息
		Connect_flag = 1;                              //Connect_flag置1，表示连接成功	
		WiFi_RxCounter=0;                              //WiFi接收数据量变量清零                        
		memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);        //清空WiFi接收缓冲区 
		MQTT_Buff_Init();  
	}
}




/* 阿里云驱动 */
void  AliIoTDriver(void)
{
		/*--------------------------------------------------------------------*/
		/*   Connect_flag=1同服务器建立了连接,我们可以发布数据和接收推送了    */
		/*--------------------------------------------------------------------*/
		if(Connect_flag==1){     
			/*-------------------------------------------------------------*/
			/*                     处理发送缓冲区数据                      */
			/*-------------------------------------------------------------*/
				if(MQTT_TxDataOutPtr != MQTT_TxDataInPtr){                //if成立的话，说明发送缓冲区有数据了
				//3种情况可进入if
				//第1种：0x10 连接报文
				//第2种：0x82 订阅报文，且ConnectPack_flag置位，表示连接报文成功
				//第3种：SubcribePack_flag置位，说明连接和订阅均成功，其他报文可发
				if((MQTT_TxDataOutPtr[2]==0x10)||((MQTT_TxDataOutPtr[2]==0x82)&&(ConnectPack_flag==1))||(SubcribePack_flag==1)){    
					u1_printf("发送数据:0x%x\r\n",MQTT_TxDataOutPtr[2]);   //串口提示信息
					MQTT_TxData(MQTT_TxDataOutPtr);                        //发送数据
					MQTT_TxDataOutPtr += TBUFF_UNIT;                       //指针下移
					if(MQTT_TxDataOutPtr==MQTT_TxDataEndPtr)               //如果指针到缓冲区尾部了
						MQTT_TxDataOutPtr = MQTT_TxDataBuf[0];               //指针归位到缓冲区开头
				} 				
			}//处理发送缓冲区数据的else if分支结尾
			
			/*-------------------------------------------------------------*/
			/*                     处理接收缓冲区数据                      */
			/*-------------------------------------------------------------*/
			if(MQTT_RxDataOutPtr != MQTT_RxDataInPtr){  //if成立的话，说明接收缓冲区有数据了														
				u1_printf("接收到数据:");
				/*-----------------------------------------------------*/
				/*                    处理CONNACK报文                  */
				/*-----------------------------------------------------*/				
				//if判断，如果第一个字节是0x20，表示收到的是CONNACK报文
				//接着我们要判断第4个字节，看看CONNECT报文是否成功
				if(MQTT_RxDataOutPtr[2]==0x20){             			
				    switch(MQTT_RxDataOutPtr[5]){					
						case 0x00 : u1_printf("CONNECT报文成功\r\n");                            //串口输出信息	
								    ConnectPack_flag = 1;                                        //CONNECT报文成功，订阅报文可发
									break;                                                       //跳出分支case 0x00                                              
						case 0x01 : u1_printf("连接已拒绝，不支持的协议版本，准备重启\r\n");     //串口输出信息
									Connect_flag = 0;                                            //Connect_flag置零，重启连接
									break;                                                       //跳出分支case 0x01   
						case 0x02 : u1_printf("连接已拒绝，不合格的客户端标识符，准备重启\r\n"); //串口输出信息
									Connect_flag = 0;                                            //Connect_flag置零，重启连接
									break;                                                       //跳出分支case 0x02 
						case 0x03 : u1_printf("连接已拒绝，服务端不可用，准备重启\r\n");         //串口输出信息
									Connect_flag = 0;                                            //Connect_flag置零，重启连接
									break;                                                       //跳出分支case 0x03
						case 0x04 : u1_printf("连接已拒绝，无效的用户名或密码，准备重启\r\n");   //串口输出信息
									Connect_flag = 0;                                            //Connect_flag置零，重启连接						
									break;                                                       //跳出分支case 0x04
						case 0x05 : u1_printf("连接已拒绝，未授权，准备重启\r\n");               //串口输出信息
									Connect_flag = 0;                                            //Connect_flag置零，重启连接						
									break;                                                       //跳出分支case 0x05 		
						default   : u1_printf("连接已拒绝，未知状态，准备重启\r\n");             //串口输出信息 
									Connect_flag = 0;                                            //Connect_flag置零，重启连接					
									break;                                                       //跳出分支case default 								
					}				
				}			
				//if判断，第一个字节是0x90，表示收到的是SUBACK报文
				//接着我们要判断订阅回复，看看是不是成功
				else if(MQTT_RxDataOutPtr[2]==0x90){ 
						switch(MQTT_RxDataOutPtr[6]){					
						case 0x00:
						case 0x01:
									u1_printf("订阅成功\r\n");            		//串口输出信息
							    SubcribePack_flag = 1;                		//SubcribePack_flag置1，表示订阅报文成功，其他报文可发送
									Ping_flag = 0;                        		//Ping_flag清零
   							  SetPingTime(15);                    			//启动30s的PING定时器
									AliIoT_SubData();													//第一次上传数据
									break;                                		//跳出分支                                             
						default: u1_printf("订阅失败，准备重启\r\n"); 	//串口输出信息 
									Connect_flag = 0;                     		//Connect_flag置零，重启连接
									break;                                		//跳出分支 								
					}					
				}
				//if判断，第一个字节是0xD0，表示收到的是PINGRESP报文
				else if(MQTT_RxDataOutPtr[2]==0xD0){ 
					u1_printf("PING报文回复\r\n"); 		  	//串口输出信息 
					if(Ping_flag==1){                     //如果Ping_flag=1，表示第一次发送
						 Ping_flag = 0;    				  				//要清除Ping_flag标志
					}else if(Ping_flag>1){ 				  			//如果Ping_flag>1，表示是多次发送了，而且是2s间隔的快速发送
						Ping_flag = 0;     				  				//要清除Ping_flag标志
						SetPingTime(15); 				  					//PING定时器重回30s的时间
					}				
				}	
				//if判断，如果第一个字节是0x30，表示收到的是服务器发来的推送数据
				//我们要提取控制命令
				else if((MQTT_RxDataOutPtr[2]==0x30)){ 
					u1_printf("服务器等级0推送\r\n"); 		     //串口输出信息 
					MQTT_DealPushdata_Qs0(MQTT_RxDataOutPtr);  //处理等级0推送数据
				}				
								
				MQTT_RxDataOutPtr += RBUFF_UNIT;                     //指针下移
				if(MQTT_RxDataOutPtr==MQTT_RxDataEndPtr)             //如果指针到缓冲区尾部了
					MQTT_RxDataOutPtr = MQTT_RxDataBuf[0];             //指针归位到缓冲区开头                        
			}//处理接收缓冲区数据的else if分支结尾
			
			/*-------------------------------------------------------------*/
			/*                     处理命令缓冲区数据                      */
			/*-------------------------------------------------------------*/
			if(MQTT_CMDOutPtr != MQTT_CMDInPtr){                           //if成立的话，说明命令缓冲区有数据了			       
				u1_printf("命令:%s\r\n",&MQTT_CMDOutPtr[2]);                 //串口输出信息
				AliIoT_DealCmd();																						 //处理阿里的数据
				MQTT_CMDOutPtr += CBUFF_UNIT;                             	 //指针下移
				if(MQTT_CMDOutPtr==MQTT_CMDEndPtr)           	               //如果指针到缓冲区尾部了
					MQTT_CMDOutPtr = MQTT_CMDBuf[0];          	               //指针归位到缓冲区开头				
			}//处理命令缓冲区数据的else if分支结尾	
		}//Connect_flag=1的if分支的结尾
		
		/*--------------------------------------------------------------------*/
		/*      Connect_flag=0同服务器断开了连接,我们要重启连接服务器         */
		/*--------------------------------------------------------------------*/
		else{
			AliIoT_Link();																								 //进行重连
		}
}

