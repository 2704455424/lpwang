#include "cat1.h"
#include "delay.h"
#include "stdlib.h"
#include "usart1.h"
#include "utils_hmac.h"
#include "utils_md5.h"
#include "utils_sha1.h"

char Connect_flag = 0;  											//�ⲿ����������ͬ����������״̬  0����û�����ӷ�����  1�������Ϸ�������
char Send_flag = 0;     											//1���������� 0���Ƿ�������

unsigned char  RxDataBuf[R_NUM][RBUFF_UNIT];  //���ݻ�����
unsigned char *RxDataInPtr;            				//ָ�򻺳���������ݵ�λ��
unsigned char *RxDataOutPtr;           				//ָ�򻺳�����ȡ���ݵ�λ��
unsigned char *RxDataEndPtr;           				//ָ�򻺳���������λ��

unsigned char  TxDataBuf[T_NUM][TBUFF_UNIT];  //���ݻ�����
unsigned char *TxDataInPtr;            				//ָ�򻺳���������ݵ�λ��
unsigned char *TxDataOutPtr;           				//ָ�򻺳�����ȡ���ݵ�λ��
unsigned char *TxDataEndPtr;           				//ָ�򻺳���������λ��

/**
  * @brief  ��ʼ��CAT1�ĸ�λ����
  * @param  None
  * @retval None
  */
void CAT1_ResetIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;                      //����һ������IO�˿ڲ����Ľṹ��
	RCC_APB2PeriphClockCmd(CAT1_GPIO_CLOCK, ENABLE);   				//ʹ��PA�˿�ʱ��
	
	GPIO_InitStructure.GPIO_Pin = CAT1_RST_Pin;               //׼�����ø�λ����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;         //����50Mhz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   		  	//���������ʽ
	GPIO_Init(GPIOA, &GPIO_InitStructure);            		  	//����PA1
}

/**
  * @brief  ��ʼ��CAT1ģ��Air724UG 
  * @param  None
  * @retval None
  */
void CAT1_Init(void)
{
	s8 timeout = 20;
	
	u1_printf("��λAir724UGģ��\r\n");    //������ʾ����
	CAT1_ResetIO_Init();							//��ʼ���˿�	
	
	CAT1_RST:
	CAT1_RstSet(1);
	delay_ms(500);
	CAT1_RstSet(0);
		
	u1_printf("�ȴ�Air724UG��ʼ�����\r\n");                //������ʾ����	
	while(timeout--)
	{	
		if(strstr(CAT1_RX_BUF,"SMS READY")!=NULL){
			u1_printf("\r\n��������:\r\n%s", CAT1_RX_BUF);			//����Air202ģ�鸴λ��,�ȶ��Ƚ���,��Ҫ�ȴ�
			break;
		}
		u1_printf("%d ", timeout);														//������ʾ����,��ʱʱ��
		delay_ms(1000);
	}
	
	if(timeout<=0){																					//��ʱ�Ļ������¸�λ
		timeout = 20;																					//���ó�ʱʱ��
		goto CAT1_RST;
	}
	CAT1_DataBuff_Init();																		//��ʼ��������
	
	u1_printf("Air724UG��ʼ�����\r\n");                		//������ʾ����	

}

/**
	* @brief  ��ʼ�����ݻ�����
	* @param  None
	* @retval None
  */
void CAT1_DataBuff_Init(void)
{
	RxDataInPtr = RxDataBuf[0];               					//ָ����ջ�����������ݵ�ָ���λ
	RxDataOutPtr = RxDataInPtr;               					//ָ����ջ�������ȡ���ݵ�ָ���λ
  RxDataEndPtr = RxDataBuf[R_NUM-1];          				//ָ����ջ�����������ָ���λ
	
	TxDataInPtr = TxDataBuf[0];               					//ָ���ͻ�����������ݵ�ָ���λ
	TxDataOutPtr = TxDataInPtr;               					//ָ���ͻ�������ȡ���ݵ�ָ���λ
  TxDataEndPtr = TxDataBuf[T_NUM-1];          				//ָ���ͻ�����������ָ���λ
}

/**
	* @brief  ��ȡ������MQTT��ʼ������
	* @param  �������õ�����ָ��
	* @retval None
  */
void AliIoT_MQTT_DataInit(char *Buff)
{
	char temp[TBUFF_UNIT];                                              	//������ܵ�ʱ����ʱʹ�õĻ�����
	char ClientID[128];                                          					//��ſͻ���ID�Ļ�����
	char Username[128];                                          					//����û����Ļ�����
	char Passward[128];                                          					//�������Ļ�����
	
	memset(ClientID,0,128);                                               //�ͻ���ID�Ļ�����ȫ������
	sprintf(ClientID,"%s|securemode=3,signmethod=hmacsha1|",DEVICENAME);  //�����ͻ���ID�������뻺����
	
	memset(Username,0,128);                                               //�û����Ļ�����ȫ������
	sprintf(Username,"%s&%s",DEVICENAME,PRODUCTKEY);                      //�����û����������뻺����
	
	memset(temp,0,128);                                                                      //��ʱ������ȫ������
	sprintf(temp,"clientId%sdeviceName%sproductKey%s",DEVICENAME,DEVICENAME,PRODUCTKEY);     //��������ʱ������   
	utils_hmac_sha1(temp,strlen(temp),Passward,DEVICESECRE,strlen(DEVICESECRE));             //��DeviceSecretΪ��Կ��temp�е����ģ�����hmacsha1���ܣ�����������룬�����浽��������
	
	sprintf(Buff, "AT+MCONFIG=\"%s\",\"%s\",\"%s\"", ClientID, Username, Passward);					 //������õ����ݷ���Buff����
	u1_printf("�ͻ���ID��%s\r\n",ClientID);               								//�������������Ϣ
	u1_printf("�� �� ����%s\r\n",Username);               								//�������������Ϣ
	u1_printf("��    �룺%s\r\n",Passward);               								//�������������Ϣ
}

/**
	* @brief  CAT1��������
	* @param  ��Ҫ���͵�����
	* @param  ��ʱʱ��, timeout*100ms
	* @retval ���ݷ������� 0:��ȷ, 1:����
  */
char CAT1_SendCmd(char *cmd, int timeout)
{
	CAT1_RxCounter=0;                           //CAT1������������������                        
	memset(CAT1_RX_BUF,0,CAT1_RXBUFF_SIZE);     //���CAT1���ջ����� 
	CAT1_printf("%s\r\n",cmd);                  //����ָ��
	u1_printf("\r\n��������:\r\n%s\r\n",cmd);   //����ָ��
	while(timeout--){                           //�ȴ���ʱʱ�䵽0
		delay_ms(100);                          	//��ʱ100ms
		if(strstr(CAT1_RX_BUF,"OK")){           	//������յ�OK��ʾָ��ɹ�
			break;       														//��������whileѭ��
		}
		if(strstr(CAT1_RX_BUF,"ERROR")){          //������յ�ERROR��ʾָ��ʧ��
			return 1;       												//��������whileѭ��
		}
		u1_printf("%d ",timeout);              	 	//����������ڵĳ�ʱʱ��
	}                    
	u1_printf("\r\n��������:\r\n%s", CAT1_RX_BUF);	//���������Ϣ
	
	if(timeout<=0) return 2;                    //���timeout<=0��˵����ʱʱ�䵽�ˣ�Ҳû���յ�OK������1
	else return 0;		         									//��֮����ʾ��ȷ��˵���յ�OK��ͨ��break��������while
}

/**
	* @brief  CAT1��������
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
	
	u1_printf("����Topic\r\n");
}
/**
	* @brief  ���CAT1�Ĳ忨״̬
	* @param  None
	* @retval ���ݷ������� 0:SIM�����, ����:����
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
	* @brief  ���CAT1�Ĳ忨״̬
	* @param  None
	* @retval ���ݷ������� 0:�ɹ���������, 1:δ��������,SIM�������û���Ƿ��, -1�����ݶ�ȡ������
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
	* @brief  IP��ȡ������
	* @param  None
	* @retval ���ݷ������� 0:�ɹ���������, 1:δ��������,SIM�������û���Ƿ��, -1�����ݶ�ȡ������
  */
char CAT1_CIFSR_Check(void)
{
	int buf[4];
	u8 timeout = 50;

	CAT1_RxCounter=0;                           	//CAT1������������������                        
	memset(CAT1_RX_BUF,0,CAT1_RXBUFF_SIZE);     	//���CAT1���ջ����� 
	CAT1_printf("AT+CIFSR\r\n");                	//����ָ��
	while(timeout--){
		delay_ms(100);
		if(strstr(CAT1_RX_BUF,"AT+CIFSR")){        	//������û��OK��������յ�AT+CIFSR��ʾ���յ�����
			u1_printf("%s", CAT1_RX_BUF);			
			if(strstr(CAT1_RX_BUF,"ERROR")) return 1;
			sscanf(CAT1_RX_BUF,"%*[^\n]%d.%d.%d.%d", &buf[0],&buf[1],&buf[2],&buf[3]);		//�����IP��ַ
			u1_printf("\r\nCAT1��ȡ��IP:%d.%d.%d.%d\r\n", buf[0],buf[1],buf[2],buf[3]);		//���ڴ�ӡIP����
			break;       															//��������whileѭ��
		}
		u1_printf("%d ",timeout);              	 		//����������ڵĳ�ʱʱ��
	}
	
	if(timeout<=0) return 1;
	else return 0;
}


/**
	* @brief  ��ȡCAT1���ź�����
	* @param  None
	* @retval ���ݷ������� -1:��ȡ������, 0~31:�ź�����
  */
int CAT1_GetCSQ(void)
{
	int num;
	
	if(CAT1_SendCmd("AT+CSQ", 20) == 0){
		sscanf(CAT1_RX_BUF,"%*[^ ]%d", &num);							//����� +CSQ: X,0\r\nOK\r\n �е�X, ���ź�ǿ��
		return num;																				//���ַ���ת����Ϊ�����󷵻�
	}
	else{
		return -1;
	}
}


/**
	* @brief  CAT1���ӷ�����
	* @param  None
	* @retval ���ݷ������� 0:�������˷�����, ����:�޷����ӷ�����
  */
char CAT1_ConnectServer(void)
{
	int cnt;
	char temp[TBUFF_UNIT];
	
	CAT1_SendCmd("AT", 5);																//���Ͳ�������
	
	if(CAT1_SendCmd("AT+CIPSHUT", 5) == 0){								//����ʱ,Ϊȷ���Ͽ�����
		u1_printf("�ر��ƶ������ɹ�\r\n");									//������ʾ����
	}
	else{
		u1_printf("�ر��ƶ�����ʧ��\r\n");									//������ʾ����
		return 1;
	}
	
	if(CAT1_CPIN_Check() != 0){
		u1_printf("SIM��δ��������������\r\n");
		return 2;																						//SIM�����������⣬ֱ�ӷ���
	}
	else{
		u1_printf("SIM���Ѳ��\r\n");
	}
	
	cnt = CAT1_GetCSQ();																	//��ȡ�ź�����
	u1_printf("��ǰ�ź�������%d\r\n", cnt);
	if(cnt>=0 && cnt<=16){
		u1_printf("�ź���������,�豸���ܷ���������\r\n");		//������ʾ����
	}
	else if(cnt>16 && cnt<=31){
		u1_printf("�ź���������\r\n");											//������ʾ����
	}
	else{
		u1_printf("��ȡ�ź���������\r\n");									//������ʾ����
		return 3;
	}


	if(CAT1_CGATT_Check() == 0){																//�鿴�Ƿ�ɹ���������
		u1_printf("�ѳɹ���������\r\n");										//������ʾ����
	}
	else{
		u1_printf("SIM�������û���Ƿ��\r\n");								//������ʾ����
		return 4;
	}

	
	if(CAT1_SendCmd("AT+CSTT=\"\",\"\",\"\"", 20) == 0){	//����APN
		u1_printf("����APN�ɹ�\r\n");
	}else{
		u1_printf("����APNʧ��\r\n");
		return 5;
	}

	if((CAT1_SendCmd("AT+CIICR",50)==0)){									//�����ƶ�����
		u1_printf("�����ƶ������ɹ�\r\n");
	}else{
		u1_printf("�����ƶ�����ʧ��\r\n");
		return 6;
	}
		
	if(CAT1_CIFSR_Check() != 0){													//����ȡ��IP���
		u1_printf("�޷���ȡIP\r\n");
		return 7;
	}
	
	AliIoT_MQTT_DataInit(temp);														//��ʼ���������Ӳ���							
	if(CAT1_SendCmd(temp, 50) == 0){
		u1_printf("���Ӳ������óɹ�\r\n");
	}else{
		u1_printf("���Ӳ�������ʧ��\r\n");
		return 8;
	}
	
	//����TCP����
	sprintf(temp, "AT+MIPSTART=\"%s\",%d",SERVERIP,SERVERPORT);
	if(CAT1_SendCmd(temp, 50) == 0){
		cnt = 50;
		while(cnt--)
		{
			delay_ms(100);
		  if(strstr(CAT1_RX_BUF,"CONNECT")){								//CONNECT����ɹ�����
				u1_printf("\r\n����TCP�ɹ�\r\n");
				break;				
			}		
			if(strstr(CAT1_RX_BUF,"CLOSED")){									//CLOSED�������ӱ��Ͽ�
				u1_printf("\r\n����TCP�ر�\r\n");
				return 9;
			}
			u1_printf("%d ", cnt);
		}
		if(cnt == 0){
			u1_printf("\r\n����TCP��ʱ\r\n");									//���ӳ�ʱ
			return 9;
		}
	}
	else{
		u1_printf("����TCPʧ��\r\n");												//ERROR����ʧ��
		return 9;
	}

	if(CAT1_SendCmd("AT+MCONNECT=0,600", 50) == 0){				//���Ӱ����������������600s������ʱ��
		cnt = 50;
		while(cnt--)
		{
			delay_ms(100);
			if(strstr(CAT1_RX_BUF,"CONNACK")){								//CONNACK�������ӳɹ�
				u1_printf("\r\n������������ӳɹ�\r\n");
				break;				
			}
			if(strstr(CAT1_RX_BUF,"CLOSED")){									//CLOSED�������ӱ��Ͽ�
				u1_printf("\r\n������������ӹر�\r\n");
				return 10;
			}
			u1_printf("%d ", cnt);
		}
		if(cnt <= 0){
			u1_printf("\r\n������������������Ӧ\r\n");		//���ӳ�ʱ
			return 10;
		}	
	}
	else{
		u1_printf("�������������ʧ��\r\n");								//ERROR����ʧ��
		return 10;
	}
	
	Connect_flag = 1;																			//���ӱ�־��1
	CAT1_RxCounter=0;                           					//������������������                        
	memset(CAT1_RX_BUF,0,CAT1_RXBUFF_SIZE);     					//��ս��ջ����� 
	
	return 0;																							//�ɹ�����0
}





/**
	* @brief  CAT1�������ݣ�����ȼ�Qs1
	* @param  ��Ҫ���͵�Topic����ָ��
	* @param  ��Ҫ���͵�����
	* @retval None
  */
void CAT1_PushData(char *topic, char *data)
{
	char temp[TBUFF_UNIT];
	
	sprintf(temp, "AT+MPUB=\"%s\",1,0,\"%s\"\r\n", topic, data);				//�������ݣ���Push��TxDataBuf
	memcpy(TxDataInPtr,temp,strlen(temp));      												//�������ݵ����ͻ�����	
	TxDataInPtr += TBUFF_UNIT;                 													//ָ������
	if(TxDataInPtr == TxDataEndPtr)      																	//���ָ�뵽������β����
		TxDataInPtr = TxDataBuf[0];    																		//ָ���λ����������ͷ
}


/**
	* @brief  CAT1�������ݣ�����ȼ�Qs1
	* @param  ��Ҫ���͵�����ָ��
	* @param  ��ʱʱ��
	* @retval ���ݷ������� 0:���ݳɹ�����, 1:����ʧ��, 2:���ͳ�ʱ
  */
char CAT1_Publish(u8 *data, int timeout)
{
	u8 error = 0;
	char *temp;
	
	Send_flag = 1;																												//��״̬��Ϊ����״̬��Ϊ�������ַ��������ͻ��ǻ�Ӧ
	CAT1_RxCounter=0;                           													//CAT1������������������                        
	memset(CAT1_RX_BUF,0,CAT1_RXBUFF_SIZE);     													//���CAT1���ջ����� 
	CAT1_printf((char*)data);          																		//����ָ��
	while(timeout--){
		delay_ms(10);
		if(strstr(CAT1_RX_BUF, "PUBACK")){																	//���յ�PUBACK�������ɹ�
			break;																														//����ѭ��
		}
		if(strstr(CAT1_RX_BUF, "ERROR")){																		//���յ�ERROR������ʧ��
			error = 1;																												//�����־��1������ʧ��
			break;																														//����ѭ��
		}		
		u1_printf("%d ", timeout);																					//��ӡ��Ϣ
	}
	
	if(timeout<=0) error = 2;																							//�����־��2������ʱ
	
	timeout = 20;																													//��Ϊ���ܵ�PUBACK�󻹻�������
	while(timeout--){																											//�ȴ�PUBACK������ݽ������
		delay_ms(10);
		temp = strstr(CAT1_RX_BUF, "PUBACK");																//��ȡPUBACK���ַ���
		if(strstr(temp, "+MSUB:"))	break;																	//��������� +MSUB: ˵�����ݽ������
	}
	delay_ms(100);
	
	u1_printf("\r\n���ܻ�Ӧ������:\r\n%s", CAT1_RX_BUF);									//������ʾ����
	if(error != 0){																												//������ʾ����
		u1_printf("\r\n���ݷ���ʧ��\r\nError����:%d\r\n",error);
	}
	else{
		u1_printf("\r\n���ݷ����ɹ�\r\n");
	}
	
	Send_flag = 0;																												//������ͱ�־
	CAT1_RxCounter=0;                           													//CAT1������������������                        
	memset(CAT1_RX_BUF,0,CAT1_RXBUFF_SIZE);     													//���CAT1���ջ����� 
	
	return error;																													//���ر�־
}


