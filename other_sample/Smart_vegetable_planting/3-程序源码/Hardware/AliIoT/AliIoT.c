#include "AliIoT.h"
#include "tim3.h"
#include "wifi.h"
#include "mqtt.h"
#include "usart1.h"
#include "monitor.h"

u8 flagPingTX = 0;							//PING���ͱ�־
u8 PingCmd = 0;									//����PING���ĵ�ʹ������
u16 PingCnt = 0;								//Ping��1s����
u16 PingTime = 0;								//Ping���ĵ�ʱ����
u16 PingRxTime = 0;						  //Ping�ȴ�����ʱ��


 /* �����﷢���ı������� */
void AliIoT_DealCmd(void) 
{
	/* �����������Ӧ�����ݼ��� */
	SwitchIoTCmd();														//�������ƿ�������
}

/* ����������������  */
void AliIoT_SubData(void)
{
	/* �����������Ӧ�����ݼ��� */
	
}

/*  ����Ͽ�����ʱ,���öϿ����ӿ��Ʊ�־  */
void SetDisConnect(void)
{
	flag1s = 0;
	cntTime = 0;
}

/* ��ʱPING���ĵķ���ʱ��x�� */
void SetPingTime(u16 timeout)
{
	PingTime = timeout*1000;			//PING���ĵ�ʱ����
	PingCnt = 0;									//Ping1s������0
	PingCmd = 1;									//����PING���ĵ�ʹ������
	
	u1_printf("PING���ϴ�����: %ds\r\n",timeout);	
}

/* Ping��ʱ�����ö�ʱ���жϼ�ʱ */
void PingTimeCount(u16 ms)
{
	if(PingCmd)											//����PING��ʱ����
	{
		PingCnt += ms;								//PING���ж�ʱ����
		if(PingCnt>=PingTime){				//PING����ʱʱ�䵽
			PingCnt = 0;								//��������
			flagPingTX = 1;							//PING���ͱ�־��1
		}
	}
	else{
		PingCnt = 0;									//ֹͣPING���м�������
	}
}

/* PING���ռ�� */
void PingRxMonitor(u8 ms)
{
	if(Ping_flag != 0)
	{
		PingRxTime += ms;
	}
}

/* ���ﱣ��PING������ */
void PingMonitor(void)
{
	if(flagPingTX || (PingRxTime>2000)){											 //��ʱʱ�䵽,���ͱ�־��1׼������PING����
		flagPingTX = 0;										 //���ͱ�־		
		PingRxTime = 0;										 //���ռ�ʱ��0
		switch(Ping_flag){                 //�ж�Ping_flag��״̬
				case 0:												 //���Ping_flag����0����ʾ����״̬������Ping����  
						MQTT_PingREQ(); 					 //���Ping���ĵ����ͻ�����  
						break;
				case 1:												 //���Ping_flag����1��˵����һ�η��͵���ping���ģ�û���յ��������ظ�������1û�б����Ϊ0�������������쳣������Ҫ��������pingģʽ
						SetPingTime(2); 					 //���ǽ���ʱ��6����Ϊ2s��ʱ,���ٷ���Ping����
						MQTT_PingREQ();  					 //���Ping���ĵ����ͻ�����  
						break;
				case 2:										 		 //���Ping_flag����2��˵����û���յ��������ظ�
				case 3:				                 //���Ping_flag����3��˵����û���յ��������ظ�
				case 4:				                 //���Ping_flag����4��˵����û���յ��������ظ�	
						MQTT_PingREQ();  					 //���Ping���ĵ����ͻ����� 
						break;
				case 5:												 //���Ping_flag����5��˵�����Ƿ����˶��ping�����޻ظ���Ӧ�������������⣬������������
						Connect_flag = 0;          //����״̬��0����ʾ�Ͽ���û���Ϸ�����
						PingCmd = 0;;              //��TIM3 				
						break;			
		}
		Ping_flag++;   
	}
}



/* ���Ӱ����� */
void AliIoT_Link(void)
{
	u1_printf("��Ҫ���ӷ�����\r\n");                 //���������Ϣ
	TIM_Cmd(TIM2,DISABLE);                           //�ر�TIM2 
	PingCmd = 0;;                          					 //�ر�TIM3   
	WiFi_RxCounter=0;                                //WiFi������������������                        
	memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);          //���WiFi���ջ����� 
	if(WiFi_Connect_IoTServer()==0){   			    		 //���WiFi�����Ʒ�������������0����ʾ��ȷ������if
		u1_printf("����TCP���ӳɹ�\r\n");            	 //���������Ϣ
		Connect_flag = 1;                              //Connect_flag��1����ʾ���ӳɹ�	
		WiFi_RxCounter=0;                              //WiFi������������������                        
		memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);        //���WiFi���ջ����� 
		MQTT_Buff_Init();  
	}
}




/* ���������� */
void  AliIoTDriver(void)
{
		/*--------------------------------------------------------------------*/
		/*   Connect_flag=1ͬ����������������,���ǿ��Է������ݺͽ���������    */
		/*--------------------------------------------------------------------*/
		if(Connect_flag==1){     
			/*-------------------------------------------------------------*/
			/*                     �����ͻ���������                      */
			/*-------------------------------------------------------------*/
				if(MQTT_TxDataOutPtr != MQTT_TxDataInPtr){                //if�����Ļ���˵�����ͻ�������������
				//3������ɽ���if
				//��1�֣�0x10 ���ӱ���
				//��2�֣�0x82 ���ı��ģ���ConnectPack_flag��λ����ʾ���ӱ��ĳɹ�
				//��3�֣�SubcribePack_flag��λ��˵�����ӺͶ��ľ��ɹ����������Ŀɷ�
				if((MQTT_TxDataOutPtr[2]==0x10)||((MQTT_TxDataOutPtr[2]==0x82)&&(ConnectPack_flag==1))||(SubcribePack_flag==1)){    
					u1_printf("��������:0x%x\r\n",MQTT_TxDataOutPtr[2]);   //������ʾ��Ϣ
					MQTT_TxData(MQTT_TxDataOutPtr);                        //��������
					MQTT_TxDataOutPtr += TBUFF_UNIT;                       //ָ������
					if(MQTT_TxDataOutPtr==MQTT_TxDataEndPtr)               //���ָ�뵽������β����
						MQTT_TxDataOutPtr = MQTT_TxDataBuf[0];               //ָ���λ����������ͷ
				} 				
			}//�����ͻ��������ݵ�else if��֧��β
			
			/*-------------------------------------------------------------*/
			/*                     ������ջ���������                      */
			/*-------------------------------------------------------------*/
			if(MQTT_RxDataOutPtr != MQTT_RxDataInPtr){  //if�����Ļ���˵�����ջ�������������														
				u1_printf("���յ�����:");
				/*-----------------------------------------------------*/
				/*                    ����CONNACK����                  */
				/*-----------------------------------------------------*/				
				//if�жϣ������һ���ֽ���0x20����ʾ�յ�����CONNACK����
				//��������Ҫ�жϵ�4���ֽڣ�����CONNECT�����Ƿ�ɹ�
				if(MQTT_RxDataOutPtr[2]==0x20){             			
				    switch(MQTT_RxDataOutPtr[5]){					
						case 0x00 : u1_printf("CONNECT���ĳɹ�\r\n");                            //���������Ϣ	
								    ConnectPack_flag = 1;                                        //CONNECT���ĳɹ������ı��Ŀɷ�
									break;                                                       //������֧case 0x00                                              
						case 0x01 : u1_printf("�����Ѿܾ�����֧�ֵ�Э��汾��׼������\r\n");     //���������Ϣ
									Connect_flag = 0;                                            //Connect_flag���㣬��������
									break;                                                       //������֧case 0x01   
						case 0x02 : u1_printf("�����Ѿܾ������ϸ�Ŀͻ��˱�ʶ����׼������\r\n"); //���������Ϣ
									Connect_flag = 0;                                            //Connect_flag���㣬��������
									break;                                                       //������֧case 0x02 
						case 0x03 : u1_printf("�����Ѿܾ�������˲����ã�׼������\r\n");         //���������Ϣ
									Connect_flag = 0;                                            //Connect_flag���㣬��������
									break;                                                       //������֧case 0x03
						case 0x04 : u1_printf("�����Ѿܾ�����Ч���û��������룬׼������\r\n");   //���������Ϣ
									Connect_flag = 0;                                            //Connect_flag���㣬��������						
									break;                                                       //������֧case 0x04
						case 0x05 : u1_printf("�����Ѿܾ���δ��Ȩ��׼������\r\n");               //���������Ϣ
									Connect_flag = 0;                                            //Connect_flag���㣬��������						
									break;                                                       //������֧case 0x05 		
						default   : u1_printf("�����Ѿܾ���δ֪״̬��׼������\r\n");             //���������Ϣ 
									Connect_flag = 0;                                            //Connect_flag���㣬��������					
									break;                                                       //������֧case default 								
					}				
				}			
				//if�жϣ���һ���ֽ���0x90����ʾ�յ�����SUBACK����
				//��������Ҫ�ж϶��Ļظ��������ǲ��ǳɹ�
				else if(MQTT_RxDataOutPtr[2]==0x90){ 
						switch(MQTT_RxDataOutPtr[6]){					
						case 0x00:
						case 0x01:
									u1_printf("���ĳɹ�\r\n");            		//���������Ϣ
							    SubcribePack_flag = 1;                		//SubcribePack_flag��1����ʾ���ı��ĳɹ����������Ŀɷ���
									Ping_flag = 0;                        		//Ping_flag����
   							  SetPingTime(15);                    			//����30s��PING��ʱ��
									AliIoT_SubData();													//��һ���ϴ�����
									break;                                		//������֧                                             
						default: u1_printf("����ʧ�ܣ�׼������\r\n"); 	//���������Ϣ 
									Connect_flag = 0;                     		//Connect_flag���㣬��������
									break;                                		//������֧ 								
					}					
				}
				//if�жϣ���һ���ֽ���0xD0����ʾ�յ�����PINGRESP����
				else if(MQTT_RxDataOutPtr[2]==0xD0){ 
					u1_printf("PING���Ļظ�\r\n"); 		  	//���������Ϣ 
					if(Ping_flag==1){                     //���Ping_flag=1����ʾ��һ�η���
						 Ping_flag = 0;    				  				//Ҫ���Ping_flag��־
					}else if(Ping_flag>1){ 				  			//���Ping_flag>1����ʾ�Ƕ�η����ˣ�������2s����Ŀ��ٷ���
						Ping_flag = 0;     				  				//Ҫ���Ping_flag��־
						SetPingTime(15); 				  					//PING��ʱ���ػ�30s��ʱ��
					}				
				}	
				//if�жϣ������һ���ֽ���0x30����ʾ�յ����Ƿ�������������������
				//����Ҫ��ȡ��������
				else if((MQTT_RxDataOutPtr[2]==0x30)){ 
					u1_printf("�������ȼ�0����\r\n"); 		     //���������Ϣ 
					MQTT_DealPushdata_Qs0(MQTT_RxDataOutPtr);  //����ȼ�0��������
				}				
								
				MQTT_RxDataOutPtr += RBUFF_UNIT;                     //ָ������
				if(MQTT_RxDataOutPtr==MQTT_RxDataEndPtr)             //���ָ�뵽������β����
					MQTT_RxDataOutPtr = MQTT_RxDataBuf[0];             //ָ���λ����������ͷ                        
			}//������ջ��������ݵ�else if��֧��β
			
			/*-------------------------------------------------------------*/
			/*                     ���������������                      */
			/*-------------------------------------------------------------*/
			if(MQTT_CMDOutPtr != MQTT_CMDInPtr){                           //if�����Ļ���˵�����������������			       
				u1_printf("����:%s\r\n",&MQTT_CMDOutPtr[2]);                 //���������Ϣ
				AliIoT_DealCmd();																						 //�����������
				MQTT_CMDOutPtr += CBUFF_UNIT;                             	 //ָ������
				if(MQTT_CMDOutPtr==MQTT_CMDEndPtr)           	               //���ָ�뵽������β����
					MQTT_CMDOutPtr = MQTT_CMDBuf[0];          	               //ָ���λ����������ͷ				
			}//��������������ݵ�else if��֧��β	
		}//Connect_flag=1��if��֧�Ľ�β
		
		/*--------------------------------------------------------------------*/
		/*      Connect_flag=0ͬ�������Ͽ�������,����Ҫ�������ӷ�����         */
		/*--------------------------------------------------------------------*/
		else{
			AliIoT_Link();																								 //��������
		}
}

