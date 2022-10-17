#include "cat1.h"
#include "delay.h"
#include "stdlib.h"
#include "usart1.h"
#include "utils_hmac.h"
#include "utils_md5.h"
#include "utils_sha1.h"

char Connect_flag = 0;  											//外部变量声明，同服务器连接状态  0：还没有连接服务器  1：连接上服务器了
char Send_flag = 0;     											//1：发送数据 0：非发送数据

unsigned char  RxDataBuf[R_NUM][RBUFF_UNIT];  //数据缓冲区
unsigned char *RxDataInPtr;            				//指向缓冲区存放数据的位置
unsigned char *RxDataOutPtr;           				//指向缓冲区读取数据的位置
unsigned char *RxDataEndPtr;           				//指向缓冲区结束的位置

unsigned char  TxDataBuf[T_NUM][TBUFF_UNIT];  //数据缓冲区
unsigned char *TxDataInPtr;            				//指向缓冲区存放数据的位置
unsigned char *TxDataOutPtr;           				//指向缓冲区读取数据的位置
unsigned char *TxDataEndPtr;           				//指向缓冲区结束的位置

/**
  * @brief  初始化CAT1的复位引脚
  * @param  None
  * @retval None
  */
void CAT1_ResetIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;                      //定义一个设置IO端口参数的结构体
	RCC_APB2PeriphClockCmd(CAT1_GPIO_CLOCK, ENABLE);   				//使能PA端口时钟
	
	GPIO_InitStructure.GPIO_Pin = CAT1_RST_Pin;               //准备设置复位引脚
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;         //速率50Mhz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   		  	//推免输出方式
	GPIO_Init(GPIOA, &GPIO_InitStructure);            		  	//设置PA1
}

/**
  * @brief  初始化CAT1模块Air724UG 
  * @param  None
  * @retval None
  */
void CAT1_Init(void)
{
	s8 timeout = 20;
	
	u1_printf("复位Air724UG模块\r\n");    //串口提示数据
	CAT1_ResetIO_Init();							//初始化端口	
	
	CAT1_RST:
	CAT1_RstSet(1);
	delay_ms(500);
	CAT1_RstSet(0);
		
	u1_printf("等待Air724UG初始化完成\r\n");                //串口提示数据	
	while(timeout--)
	{	
		if(strstr(CAT1_RX_BUF,"SMS READY")!=NULL){
			u1_printf("\r\n接收数据:\r\n%s", CAT1_RX_BUF);			//由于Air202模块复位后,稳定比较慢,需要等待
			break;
		}
		u1_printf("%d ", timeout);														//串口提示数据,超时时间
		delay_ms(1000);
	}
	
	if(timeout<=0){																					//超时的话，重新复位
		timeout = 20;																					//重置超时时间
		goto CAT1_RST;
	}
	CAT1_DataBuff_Init();																		//初始化缓冲区
	
	u1_printf("Air724UG初始化完成\r\n");                		//串口提示数据	

}

/**
	* @brief  初始化数据缓冲区
	* @param  None
	* @retval None
  */
void CAT1_DataBuff_Init(void)
{
	RxDataInPtr = RxDataBuf[0];               					//指向接收缓冲区存放数据的指针归位
	RxDataOutPtr = RxDataInPtr;               					//指向接收缓冲区读取数据的指针归位
  RxDataEndPtr = RxDataBuf[R_NUM-1];          				//指向接收缓冲区结束的指针归位
	
	TxDataInPtr = TxDataBuf[0];               					//指向发送缓冲区存放数据的指针归位
	TxDataOutPtr = TxDataInPtr;               					//指向发送缓冲区读取数据的指针归位
  TxDataEndPtr = TxDataBuf[T_NUM-1];          				//指向发送缓冲区结束的指针归位
}

/**
	* @brief  获取阿里云MQTT初始化报文
	* @param  存放整理好的数据指针
	* @retval None
  */
void AliIoT_MQTT_DataInit(char *Buff)
{
	char temp[TBUFF_UNIT];                                              	//计算加密的时候，临时使用的缓冲区
	char ClientID[128];                                          					//存放客户端ID的缓冲区
	char Username[128];                                          					//存放用户名的缓冲区
	char Passward[128];                                          					//存放密码的缓冲区
	
	memset(ClientID,0,128);                                               //客户端ID的缓冲区全部清零
	sprintf(ClientID,"%s|securemode=3,signmethod=hmacsha1|",DEVICENAME);  //构建客户端ID，并存入缓冲区
	
	memset(Username,0,128);                                               //用户名的缓冲区全部清零
	sprintf(Username,"%s&%s",DEVICENAME,PRODUCTKEY);                      //构建用户名，并存入缓冲区
	
	memset(temp,0,128);                                                                      //临时缓冲区全部清零
	sprintf(temp,"clientId%sdeviceName%sproductKey%s",DEVICENAME,DEVICENAME,PRODUCTKEY);     //构建加密时的明文   
	utils_hmac_sha1(temp,strlen(temp),Passward,DEVICESECRE,strlen(DEVICESECRE));             //以DeviceSecret为秘钥对temp中的明文，进行hmacsha1加密，结果就是密码，并保存到缓冲区中
	
	sprintf(Buff, "AT+MCONFIG=\"%s\",\"%s\",\"%s\"", ClientID, Username, Passward);					 //将整理好的数据放在Buff里面
	u1_printf("客户端ID：%s\r\n",ClientID);               								//串口输出调试信息
	u1_printf("用 户 名：%s\r\n",Username);               								//串口输出调试信息
	u1_printf("密    码：%s\r\n",Passward);               								//串口输出调试信息
}

/**
	* @brief  CAT1发送命令
	* @param  需要发送的命令
	* @param  超时时间, timeout*100ms
	* @retval 数据返回类型 0:正确, 1:错误
  */
char CAT1_SendCmd(char *cmd, int timeout)
{
	CAT1_RxCounter=0;                           //CAT1接收数据量变量清零                        
	memset(CAT1_RX_BUF,0,CAT1_RXBUFF_SIZE);     //清空CAT1接收缓冲区 
	CAT1_printf("%s\r\n",cmd);                  //发送指令
	u1_printf("\r\n发送命令:\r\n%s\r\n",cmd);   //发送指令
	while(timeout--){                           //等待超时时间到0
		delay_ms(100);                          	//延时100ms
		if(strstr(CAT1_RX_BUF,"OK")){           	//如果接收到OK表示指令成功
			break;       														//主动跳出while循环
		}
		if(strstr(CAT1_RX_BUF,"ERROR")){          //如果接收到ERROR表示指令失败
			return 1;       												//主动跳出while循环
		}
		u1_printf("%d ",timeout);              	 	//串口输出现在的超时时间
	}                    
	u1_printf("\r\n接收数据:\r\n%s", CAT1_RX_BUF);	//串口输出信息
	
	if(timeout<=0) return 2;                    //如果timeout<=0，说明超时时间到了，也没能收到OK，返回1
	else return 0;		         									//反之，表示正确，说明收到OK，通过break主动跳出while
}

/**
	* @brief  CAT1订阅主题
	* @param  None
	* @retval None
  */
void CAT1_SubData(void)
{
	char temp[128];
	
	sprintf(temp, "AT+MSUB=%s,1",S_TOPIC_NAME);
	CAT1_SendCmd(temp, 20);
	sprintf(temp, "AT+MSUB=%s,1",P_TOPIC_NAME);
	CAT1_SendCmd(temp, 20);
	
	u1_printf("订阅Topic\r\n");
}
/**
	* @brief  检查CAT1的插卡状态
	* @param  None
	* @retval 数据返回类型 0:SIM卡插好, 其他:错误
  */
char CAT1_CPIN_Check(void)
{
	if(CAT1_SendCmd("AT+CPIN?", 20) == 0)
	{
		if(strstr(CAT1_RX_BUF, "+CPIN: READY")){
			return 0;
		}
		else return 1;
	}
	
	return 2;
}

/**
	* @brief  检查CAT1的插卡状态
	* @param  None
	* @retval 数据返回类型 0:成功附着网络, 1:未附着网络,SIM卡不可用或者欠费, -1：数据读取出问题
  */
char CAT1_CGATT_Check(void)
{
	if(CAT1_SendCmd("AT+CGATT?", 50) == 0){
		if(strstr(CAT1_RX_BUF, "+CGATT: 1")){
			return 0;
		}
		else{
			return 1;
		}
	}
	else{
		return 2;
	}
}

/**
	* @brief  IP获取情况检测
	* @param  None
	* @retval 数据返回类型 0:成功附着网络, 1:未附着网络,SIM卡不可用或者欠费, -1：数据读取出问题
  */
char CAT1_CIFSR_Check(void)
{
	int buf[4];
	u8 timeout = 50;

	CAT1_RxCounter=0;                           	//CAT1接收数据量变量清零                        
	memset(CAT1_RX_BUF,0,CAT1_RXBUFF_SIZE);     	//清空CAT1接收缓冲区 
	CAT1_printf("AT+CIFSR\r\n");                	//发送指令
	while(timeout--){
		delay_ms(100);
		if(strstr(CAT1_RX_BUF,"AT+CIFSR")){        	//该命令没有OK，如果接收到AT+CIFSR表示接收到数据
			u1_printf("%s", CAT1_RX_BUF);			
			if(strstr(CAT1_RX_BUF,"ERROR")) return 1;
			sscanf(CAT1_RX_BUF,"%*[^\n]%d.%d.%d.%d", &buf[0],&buf[1],&buf[2],&buf[3]);		//分离出IP地址
			u1_printf("\r\nCAT1获取的IP:%d.%d.%d.%d\r\n", buf[0],buf[1],buf[2],buf[3]);		//串口打印IP数据
			break;       															//主动跳出while循环
		}
		u1_printf("%d ",timeout);              	 		//串口输出现在的超时时间
	}
	
	if(timeout<=0) return 1;
	else return 0;
}


/**
	* @brief  获取CAT1的信号质量
	* @param  None
	* @retval 数据返回类型 -1:读取出问题, 0~31:信号质量
  */
int CAT1_GetCSQ(void)
{
	int num;
	
	if(CAT1_SendCmd("AT+CSQ", 20) == 0){
		sscanf(CAT1_RX_BUF,"%*[^ ]%d", &num);							//分离出 +CSQ: X,0\r\nOK\r\n 中的X, 即信号强度
		return num;																				//将字符串转换成为整数后返回
	}
	else{
		return -1;
	}
}


/**
	* @brief  CAT1连接服务器
	* @param  None
	* @retval 数据返回类型 0:连接上了服务器, 其他:无法连接服务器
  */
char CAT1_ConnectServer(void)
{
	int cnt;
	char temp[TBUFF_UNIT];
	
	CAT1_SendCmd("AT", 5);																//发送测试命令
	
	if(CAT1_SendCmd("AT+CIPSHUT", 5) == 0){								//重连时,为确保断开连接
		u1_printf("关闭移动场景成功\r\n");									//串口提示数据
	}
	else{
		u1_printf("关闭移动场景失败\r\n");									//串口提示数据
		return 1;
	}
	
	if(CAT1_CPIN_Check() != 0){
		u1_printf("SIM卡未插入或插入有问题\r\n");
		return 2;																						//SIM卡插入有问题，直接返回
	}
	else{
		u1_printf("SIM卡已插好\r\n");
	}
	
	cnt = CAT1_GetCSQ();																	//读取信号质量
	u1_printf("当前信号质量：%d\r\n", cnt);
	if(cnt>=0 && cnt<=16){
		u1_printf("信号质量较弱,设备可能非正常工作\r\n");		//串口提示数据
	}
	else if(cnt>16 && cnt<=31){
		u1_printf("信号质量良好\r\n");											//串口提示数据
	}
	else{
		u1_printf("读取信号质量出错\r\n");									//串口提示数据
		return 3;
	}


	if(CAT1_CGATT_Check() == 0){																//查看是否成功附着网络
		u1_printf("已成功附着网络\r\n");										//串口提示数据
	}
	else{
		u1_printf("SIM卡不可用或者欠费\r\n");								//串口提示数据
		return 4;
	}

	
	if(CAT1_SendCmd("AT+CSTT=\"\",\"\",\"\"", 20) == 0){	//设置APN
		u1_printf("设置APN成功\r\n");
	}else{
		u1_printf("设置APN失败\r\n");
		return 5;
	}

	if((CAT1_SendCmd("AT+CIICR",50)==0)){									//激活移动场景
		u1_printf("激活移动场景成功\r\n");
	}else{
		u1_printf("激活移动场景失败\r\n");
		return 6;
	}
		
	if(CAT1_CIFSR_Check() != 0){													//检查获取的IP情况
		u1_printf("无法获取IP\r\n");
		return 7;
	}
	
	AliIoT_MQTT_DataInit(temp);														//初始化阿里连接参数							
	if(CAT1_SendCmd(temp, 50) == 0){
		u1_printf("连接参数配置成功\r\n");
	}else{
		u1_printf("连接参数配置失败\r\n");
		return 8;
	}
	
	//建立TCP连接
	sprintf(temp, "AT+MIPSTART=\"%s\",%d",SERVERIP,SERVERPORT);
	if(CAT1_SendCmd(temp, 50) == 0){
		cnt = 50;
		while(cnt--)
		{
			delay_ms(100);
		  if(strstr(CAT1_RX_BUF,"CONNECT")){								//CONNECT代表成功连接
				u1_printf("\r\n建立TCP成功\r\n");
				break;				
			}		
			if(strstr(CAT1_RX_BUF,"CLOSED")){									//CLOSED代表连接被断开
				u1_printf("\r\n建立TCP关闭\r\n");
				return 9;
			}
			u1_printf("%d ", cnt);
		}
		if(cnt == 0){
			u1_printf("\r\n建立TCP超时\r\n");									//连接超时
			return 9;
		}
	}
	else{
		u1_printf("建立TCP失败\r\n");												//ERROR连接失败
		return 9;
	}

	if(CAT1_SendCmd("AT+MCONNECT=0,600", 50) == 0){				//连接阿里服务器，并设置600s的心跳时间
		cnt = 50;
		while(cnt--)
		{
			delay_ms(100);
			if(strstr(CAT1_RX_BUF,"CONNACK")){								//CONNACK代表连接成功
				u1_printf("\r\n阿里服务器连接成功\r\n");
				break;				
			}
			if(strstr(CAT1_RX_BUF,"CLOSED")){									//CLOSED代表连接被断开
				u1_printf("\r\n阿里服务器连接关闭\r\n");
				return 10;
			}
			u1_printf("%d ", cnt);
		}
		if(cnt <= 0){
			u1_printf("\r\n阿里服务器连接命令不响应\r\n");		//连接超时
			return 10;
		}	
	}
	else{
		u1_printf("阿里服务器连接失败\r\n");								//ERROR连接失败
		return 10;
	}
	
	Connect_flag = 1;																			//连接标志置1
	CAT1_RxCounter=0;                           					//接收数据量变量清零                        
	memset(CAT1_RX_BUF,0,CAT1_RXBUFF_SIZE);     					//清空接收缓冲区 
	
	return 0;																							//成功返回0
}





/**
	* @brief  CAT1发布数据，服务等级Qs1
	* @param  需要发送的Topic数据指针
	* @param  需要发送的数据
	* @retval None
  */
void CAT1_PushData(char *topic, char *data)
{
	char temp[TBUFF_UNIT];
	
	sprintf(temp, "AT+MPUB=\"%s\",1,0,\"%s\"\r\n", topic, data);				//整理数据，并Push到TxDataBuf
	memcpy(TxDataInPtr,temp,strlen(temp));      												//拷贝数据到发送缓冲区	
	TxDataInPtr += TBUFF_UNIT;                 													//指针下移
	if(TxDataInPtr == TxDataEndPtr)      																	//如果指针到缓冲区尾部了
		TxDataInPtr = TxDataBuf[0];    																		//指针归位到缓冲区开头
}


/**
	* @brief  CAT1发布数据，服务等级Qs1
	* @param  需要发送的数据指针
	* @param  超时时间
	* @retval 数据返回类型 0:数据成功发布, 1:发送失败, 2:发送超时
  */
char CAT1_Publish(u8 *data, int timeout)
{
	u8 error = 0;
	char *temp;
	
	Send_flag = 1;																												//将状态变为发送状态，为的是区分服务器推送还是回应
	CAT1_RxCounter=0;                           													//CAT1接收数据量变量清零                        
	memset(CAT1_RX_BUF,0,CAT1_RXBUFF_SIZE);     													//清空CAT1接收缓冲区 
	CAT1_printf((char*)data);          																		//发送指令
	while(timeout--){
		delay_ms(10);
		if(strstr(CAT1_RX_BUF, "PUBACK")){																	//接收到PUBACK代表发布成功
			break;																														//结束循环
		}
		if(strstr(CAT1_RX_BUF, "ERROR")){																		//接收到ERROR代表发布失败
			error = 1;																												//错误标志置1，代表失败
			break;																														//结束循环
		}		
		u1_printf("%d ", timeout);																					//打印信息
	}
	
	if(timeout<=0) error = 2;																							//错误标志置2，代表超时
	
	timeout = 20;																													//因为接受到PUBACK后还会有数据
	while(timeout--){																											//等待PUBACK后的数据接受完成
		delay_ms(10);
		temp = strstr(CAT1_RX_BUF, "PUBACK");																//获取PUBACK的字符串
		if(strstr(temp, "+MSUB:"))	break;																	//如果搜索到 +MSUB: 说明数据接受完毕
	}
	delay_ms(100);
	
	u1_printf("\r\n接受回应的数据:\r\n%s", CAT1_RX_BUF);									//串口提示数据
	if(error != 0){																												//串口提示数据
		u1_printf("\r\n数据发布失败\r\nError类型:%d\r\n",error);
	}
	else{
		u1_printf("\r\n数据发布成功\r\n");
	}
	
	Send_flag = 0;																												//清除发送标志
	CAT1_RxCounter=0;                           													//CAT1接收数据量变量清零                        
	memset(CAT1_RX_BUF,0,CAT1_RXBUFF_SIZE);     													//清空CAT1接收缓冲区 
	
	return error;																													//返回标志
}


