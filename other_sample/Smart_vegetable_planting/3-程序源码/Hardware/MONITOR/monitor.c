#include "monitor.h"
#include "adc.h"
#include "led.h"
#include "tim3.h"
#include "cat1.h"
#include "motor.h"
#include "aht10.h"
#include "24cxx.h"
#include "usart1.h"
#include "switch.h"
#include "bh1750.h"

u8 UploadTime;
u8 SwitchMode;
EnvirData envirData;								//�������ݽṹ���ʼ��
EnvirThreshold envThreshold;				//������ֵ�ṹ���ʼ��


/********************************************************************************************************
���ݴ�����
*********************************************************************************************************/
/*  ��������״̬  */
void EnvirState(void)
{
	char temp[400];
	
	//�����ϴ��������Ļ�������
	sprintf(temp, "{\\22method\\22:\\22thing.event.property.post\\22,\\22id\\22:\\223232323200\\22,\\22params\\22:\
	{\\22EnvironmentTemperature\\22:%.1f,\\22EnvironmentHumidity\\22:%.1f,\\22SoilMoisture\\22:%.1f,\\22LightLux\\22:%d,\\22CO2\\22:%d},\\22version\\22:\\221.0.0\\22}",\
	envirData.temp, envirData.humi, envirData.soil, envirData.illumi, envirData.carbon);
	
	//�����ݷ��뷢�ͻ�����
	CAT1_PushData(P_TOPIC_NAME, temp);
}

/*  ��������״̬  */
void SwitchState(void)
{
	char temp[400];
	
	//�����ϴ��������Ŀ�������
	sprintf(temp, "{\\22method\\22:\\22thing.event.property.post\\22,\\22id\\22:\\223232323201\\22,\\22params\\22:\
	{\\22HeatSwitch\\22:%d,\\22Humidified\\22:%d,\\22WaterOutletSwitch\\22:%d,\\22WotkSwitch\\22:%d,\\22PowerSwitch\\22:%d,\\22SwitchMode\\22:%d},\\22version\\22:\\221.0.0\\22}",\
	TempSwitchState, HumiSwitchState, SoilSwitchState, LightSwitchState, CarbonSwitchState, SwitchMode);
	
	//�����ݷ��뷢�ͻ�����
	CAT1_PushData(P_TOPIC_NAME, temp);
}


/*  ϵͳ����1״̬  */
void SysdatState(void)
{
	u8 TxData[400];
		
	//�����ϴ��������Ŀ�������
	sprintf((char*)TxData, "AT+MPUB=\"%s\",1,0,\"{\\22method\\22:\\22thing.event.property.post\\22,\
	\\22params\\22:{\\22EnvTemp_Low_Threshold\\22:%d,\\22EnvHumidity_Low_Threshold\\22:%d,\\22SoilHumidity_Low_Threshold\\22:%d,\\22LightLux_Max_Threshold\\22:%d,\\22CO2_Max_Threshold\\22:%d}}\"\r\n", \
	P_TOPIC_NAME, envThreshold.Temp_Low_Threshold, envThreshold.Humi_Low_Threshold, envThreshold.Soil_Low_Threshold,envThreshold.Light_Max_Threshold, envThreshold.Carbon_Max_Threshold);		
	
	u1_printf("����������:\r\n%s\r\n", TxData);
	CAT1_Publish(TxData, 50);
}

/*  ϵͳ״̬��Ӧ  */
void SysdatStateAck(char *mark, u16	data)
{
	u8 temp[200];
	
	sprintf((char*)temp, "AT+MPUB=\"%s\",1,0,\"{\\22method\\22:\\22thing.event.property.post\\22,\\22params\\22:{\\22%s\\22:%d}}\"\r\n",\
	P_TOPIC_NAME, mark, data);
	
	CAT1_Publish(temp, 50);
}

/*  ��ʼ������  */
void DATA_Init(void)
{
	u8 temp;
	
	temp = AT24CXX_ReadOneByte(CHECK_ADDR);		//����ÿ�ο�����дAT24CXX	
	if(temp != 0x5A)
	{
		AT24CXX_WriteOneByte(CHECK_ADDR, 0x5A);
		//��ʼ�������ֵ
		SwitchMode = DEF_SwitchMode;
		UploadTime = DEF_UploadTime;
		envThreshold.Temp_Low_Threshold = DEF_MinTemp;
		envThreshold.Humi_Low_Threshold = DEF_MinHumi;
		envThreshold.Soil_Low_Threshold = DEF_MinSoil;
		envThreshold.Light_Max_Threshold =DEF_MaxLight;
		envThreshold.Carbon_Max_Threshold = DEF_MaxCarbon;
		
		AT24CXX_WriteOneByte(SwitchMode_ADDR, SwitchMode);
		AT24CXX_WriteOneByte(UploadTime_ADDR, UploadTime);
		AT24CXX_WriteOneByte(MinTemp_ADDR, envThreshold.Temp_Low_Threshold);
		AT24CXX_WriteOneByte(MinHumi_ADDR, envThreshold.Humi_Low_Threshold);
		AT24CXX_WriteOneByte(MinSoil_ADDR, envThreshold.Soil_Low_Threshold);
		AT24CXX_WriteLenByte(MaxLight_ADDR, envThreshold.Light_Max_Threshold, 2);
		AT24CXX_WriteLenByte(MaxCarbon_ADDR, envThreshold.Carbon_Max_Threshold, 2);
		u1_printf("��һ��������\r\n");
	}
	else
	{
		SwitchMode = AT24CXX_ReadOneByte(SwitchMode_ADDR);
		UploadTime = AT24CXX_ReadOneByte(UploadTime_ADDR);
		envThreshold.Temp_Low_Threshold = AT24CXX_ReadOneByte(MinTemp_ADDR);
		envThreshold.Humi_Low_Threshold = AT24CXX_ReadOneByte(MinHumi_ADDR);
		envThreshold.Soil_Low_Threshold = AT24CXX_ReadOneByte(MinSoil_ADDR);
		envThreshold.Light_Max_Threshold = AT24CXX_ReadLenByte(MaxLight_ADDR, 2);
		envThreshold.Carbon_Max_Threshold = AT24CXX_ReadLenByte(MaxCarbon_ADDR, 2);
		u1_printf("�ǵ�һ��������\r\n");
	}

	u1_printf("ϵͳ��ʼ������:\r\nģʽ:%d\r\n����:%d\r\n�¶�:%d\r\nʪ��:%d\r\n����:%d\r\n������̼:%d\r\n����ǿ��:%d\r\n",\
	SwitchMode,UploadTime, envThreshold.Temp_Low_Threshold, envThreshold.Humi_Low_Threshold, envThreshold.Soil_Low_Threshold,\
	envThreshold.Carbon_Max_Threshold, envThreshold.Light_Max_Threshold);
}

/*  ������ʼ������  */
void PubInitData(void)
{
	SysdatState();					//ϵͳ����
}

/*  ����IoT�������� */
char SwitchIoTCmd(void)
{
	char status = 0;
	
	if(strstr((char *)RxDataOutPtr+2,"\"params\":{\"HeatSwitch\":1}")){										//��⵽���ȿ���Ϊ 1       	
		status++;																																						//��־λ��һ
		SwitchMode = 1;								                  																		//��־λ��1
		Switch_SetState(SwitchTemp, ON);                      															//�򿪼��ȿ���                                   
	}	else if(strstr((char *)RxDataOutPtr+2,"\"params\":{\"HeatSwitch\":0}")){ 						//��⵽���ȿ���Ϊ 0    
		status++;																																						//��־λ��һ
		SwitchMode = 1;								                  																		//��־λ��1
		Switch_SetState(SwitchTemp, OFF);    																								//�رռ��ȿ��� 	
	}
	
	if(strstr((char *)RxDataOutPtr+2,"\"params\":{\"Humidified\":1}")){										//��⵽��ʪ����Ϊ 1       	
		status++;																																						//��־λ��һ
		SwitchMode = 1;								                  																		//��־λ��1
		Switch_SetState(SwitchHumi, ON);                      															//�򿪼�ʪ����                                   
	}	else if(strstr((char *)RxDataOutPtr+2,"\"params\":{\"Humidified\":0}")){ 						//��⵽��ʪ����Ϊ 0    
		status++;																																						//��־λ��һ
		SwitchMode = 1;								                  																		//��־λ��1
		Switch_SetState(SwitchHumi, OFF);    																								//�رռ�ʪ����                                                          
	}
	
	if(strstr((char *)RxDataOutPtr+2,"\"params\":{\"WaterOutletSwitch\":1}")){						//��⵽��ˮ����Ϊ 1       	
		status++;																																						//��־λ��һ
		SwitchMode = 1;								                  																		//��־λ��1
		Switch_SetState(SwitchSoil, ON);                      															//�򿪽�ˮ����                                   
	}	else if(strstr((char *)RxDataOutPtr+2,"\"params\":{\"WaterOutletSwitch\":0}")){ 		//��⵽��ˮ����Ϊ 0    
		status++;																																						//��־λ��һ
		SwitchMode = 1;								                  																		//��־λ��1
		Switch_SetState(SwitchSoil, OFF);    																								//�رս�ˮ����                                                          
	}
		
	if(strstr((char *)RxDataOutPtr+2,"\"params\":{\"PowerSwitch\":1}")){									//��⵽ͨ�翪��Ϊ 1       	
		status++;																																						//��־λ��һ
		SwitchMode = 1;								                  																		//��־λ��1 
		Switch_SetState(SwitchCarbon, ON);                      														//��ͨ�翪��                                   
	}	else if(strstr((char *)RxDataOutPtr+2,"\"params\":{\"PowerSwitch\":0}")){ 					//��⵽ͨ�翪��Ϊ 0    
		status++;																																						//��־λ��һ
		SwitchMode = 1;								                  																		//��־λ��1 
		Switch_SetState(SwitchCarbon, OFF);    																							//�ر�ͨ�翪��                                                          
	}
	
	if(strstr((char *)RxDataOutPtr+2,"\"params\":{\"WotkSwitch\":1}")){										//��⵽��������Ϊ 1       	
		status++;																																						//��־λ��һ
		LightSwitchState = 1;								                  															//����������  
		SwitchMode = 1;								                  																		//��־λ��1 		
		StartMotor(360*10);																																	//�����ת
	}	else if(strstr((char *)RxDataOutPtr+2,"\"params\":{\"WotkSwitch\":0}")){ 						//��⵽��������Ϊ 0    
		status++;																																						//��־λ��һ
		StartMotor(-360*10);																																//�����ת
		SwitchMode = 1;								                  																		//��־λ��1 
		LightSwitchState = 0;    																														//�ر���������                                                          
	}
	
	if(strstr((char *)RxDataOutPtr+2,"\"params\":{\"SwitchMode\":1}")){										//����ģʽΪ 1       	
		status++;																																						//��־λ��һ
		SwitchMode = 1;								                  																		//��־λ��1 
		AT24CXX_WriteOneByte(SwitchMode_ADDR, SwitchMode);																	//��������

	}	else if(strstr((char *)RxDataOutPtr+2,"\"params\":{\"SwitchMode\":0}")){ 						//����ģʽΪ 0    
		status++;																																						//��־λ��һ
		SwitchMode = 0;    																																	//��־λ��0
		AT24CXX_WriteOneByte(SwitchMode_ADDR, SwitchMode);																	//��������
	}
	return status;
}

/*  ϵͳ��������  */
u8 SystemDataCmd(void)
{
	int num;
	u8 status = 0;	
	char *presult;	

	if(strstr((char *)RxDataOutPtr+2,"\"EnvTemp_Low_Threshold\":")){
		status=1;
		presult = strstr((char *)RxDataOutPtr+2,"\"EnvTemp_Low_Threshold\":");		
		sscanf(presult, "%*[^:]:%d", &num);	
		envThreshold.Temp_Low_Threshold = (u8)num;
		AT24CXX_WriteOneByte(MinTemp_ADDR, envThreshold.Temp_Low_Threshold);
		u1_printf("������ֵ: %d\r\n", envThreshold.Temp_Low_Threshold);
		SysdatStateAck("EnvTemp_Low_Threshold", envThreshold.Temp_Low_Threshold);
	}
	
	if(strstr((char *)RxDataOutPtr+2,"\"EnvHumidity_Low_Threshold\":")){
		status=2;
		presult = strstr((char *)RxDataOutPtr+2,"\"EnvHumidity_Low_Threshold\":");		
		sscanf(presult, "%*[^:]:%d", &num);	
		envThreshold.Humi_Low_Threshold = (u8)num;
		AT24CXX_WriteOneByte(MinHumi_ADDR, envThreshold.Humi_Low_Threshold);
		u1_printf("��ʪ��ֵ: %d\r\n", envThreshold.Humi_Low_Threshold);
		SysdatStateAck("EnvHumidity_Low_Threshold", envThreshold.Humi_Low_Threshold);
	}
		
	if(strstr((char *)RxDataOutPtr+2,"\"SoilHumidity_Low_Threshold\":")){
		status=3;
		presult = strstr((char *)RxDataOutPtr+2,"\"SoilHumidity_Low_Threshold\":");		
		sscanf(presult, "%*[^:]:%d", &num);	
		envThreshold.Soil_Low_Threshold = (u8)num;
		AT24CXX_WriteOneByte(MinSoil_ADDR, envThreshold.Soil_Low_Threshold);
		u1_printf("������ֵ: %d\r\n", envThreshold.Soil_Low_Threshold);
		SysdatStateAck("SoilHumidity_Low_Threshold", envThreshold.Soil_Low_Threshold);
	}
		
	if(strstr((char *)RxDataOutPtr+2,"\"LightLux_Max_Threshold\":")){
		status=4;
		presult = strstr((char *)RxDataOutPtr+2,"\"LightLux_Max_Threshold\":");		
		sscanf(presult, "%*[^:]:%d", &num);	
		envThreshold.Light_Max_Threshold = (u16)num;
		AT24CXX_WriteLenByte(MaxLight_ADDR, envThreshold.Light_Max_Threshold, 2);
		u1_printf("������ֵ: %d\r\n", envThreshold.Light_Max_Threshold);
		SysdatStateAck("LightLux_Max_Threshold", envThreshold.Light_Max_Threshold);
	}
		
	if(strstr((char *)RxDataOutPtr+2,"\"CO2_Max_Threshold\":")){
		status=5;
		presult = strstr((char *)RxDataOutPtr+2,"\"CO2_Max_Threshold\":");		
		sscanf(presult, "%*[^:]:%d", &num);	
		envThreshold.Carbon_Max_Threshold = (u16)num;
		AT24CXX_WriteLenByte(MaxCarbon_ADDR, envThreshold.Carbon_Max_Threshold, 2);
		u1_printf("CO2��ֵ: %d\r\n", envThreshold.Carbon_Max_Threshold);
		SysdatStateAck("CO2_Max_Threshold", envThreshold.Carbon_Max_Threshold);
	}
	
	return status;
}

/*  ������  */
void NetMonitor(void)
{
	if(strstr((char *)RxDataOutPtr+2,"CLOSED")){
		Connect_flag = 0;
	}
}

/*  �����������  */
void DealAliCmd(void)
{
	NetMonitor();													//������
	SystemDataCmd();											//ϵͳ���ݴ���

	if(SwitchIoTCmd() != 0) {							//˵�����յ���������
		SwitchState();											//�ظ�����������	
	}	
}

/********************************************************************************************************
���ݼ�ز���
*********************************************************************************************************/
//AHT10 ˢ�¼��
void Aht10Monitor(void)
{
	double tempdata,humidata;	
	
	AHT10_Data(&tempdata,&humidata);
	envirData.temp = (float)tempdata;
	envirData.humi = (float)humidata;
	u1_printf("\r\n�¶ȣ�%.1f'C\r\nʪ�ȣ�%.1f%%\r\n",envirData.temp,envirData.humi);
}

// ������̼Ũ�ȼ��
void CarbonMonitor(void)
{
	u16 temp;
	
	temp = ADC_DMA_IN[0];
	envirData.carbon = (temp*Verf/4096.0-0.4)*CarbonValue/1.6;				//���㹫ʽ ��V0-0.4V��*���̣�ppm��/��2.0V-0.4V��
	
	u1_printf("������̼��%dppm\r\n", envirData.carbon);	
}

// ����ʪ�ȼ��
void SoilMonitor(void)
{
	u16 temp;
	temp = ADC_DMA_IN[1];
	envirData.soil = SoilValue-(temp*SoilValue/4096.0)+30;
	
	u1_printf("����ʪ�ȣ�%.1f%%\r\n", envirData.soil);
}

// ����ǿ�ȼ��
void illumiMonitor(void)
{
	u16 dat;
	
	dat = BH1750_ReadData();
	envirData.illumi = dat/1.2;
	u1_printf("����ǿ��: %dlx\r\n", envirData.illumi);
}


// ����
void SecMonitor(void)
{
	static u8 cntMin = DEF_UploadTime;	//1���Ӽ���
		
	if(flag10s){												//��ʱ1sʱ�䵽��,���ж�ȡ�¶�
		flag10s = 0;											//��ʱ��0
		
		/*  ����������ݲɼ�  */
		Aht10Monitor();										//��ʪ�ȼ��
		SoilMonitor();										//����ʪ�ȼ��
		CarbonMonitor();									//������̼���
		illumiMonitor();									//����ǿ�ȼ��	
		SwitchMonitor();									//���ؼ��	
		
		
		if(Connect_flag == 0){
			__set_FAULTMASK(1);														//�ر������ж�
			NVIC_SystemReset();														//��λϵͳ
		}
	}
	
	if(flag1min){												//һ���Ӷ�ʱʱ�䵽
		flag1min = 0;											//������0
		cntMin++;
		
		if(cntMin >= UploadTime)					//��ʱ�ϴ�ʱ�䵽
		{
			cntMin = 0;											//��������
			
			if(Connect_flag){
				EnvirState();									//�����ϴ��������������ݱ���
			}
		}
	}
	
}




void SwitchMonitor(void)
{
//	SwitchTemp,					�¶ȿ���
//	SwitchHumi,					ʪ�ȿ���
//	SwitchSoil,					��������
//	SwitchLight,				���տ���
//	SwitchCarbon,				������̼����				
	static u8 backup[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	
	if(SwitchMode == 0)
	{
		//�¶Ȳ���		
		u1_printf("\r\n�¶���ֵ: %.1f - %d\r\n", envirData.temp, envThreshold.Temp_Low_Threshold);
		if(envirData.temp <= envThreshold.Temp_Low_Threshold){
			Switch_SetState(SwitchTemp, ON);
		}
		else{
			Switch_SetState(SwitchTemp, OFF);
		}
		
		//ʪ�Ȳ���
		u1_printf("ʪ����ֵ: %.1f - %d\r\n", envirData.humi, envThreshold.Humi_Low_Threshold);
		if(envirData.humi <= envThreshold.Humi_Low_Threshold){
			Switch_SetState(SwitchHumi, ON);
		}
		else{
			Switch_SetState(SwitchHumi, OFF);
		}
		
		//����ʪ�Ȳ���
		u1_printf("������ֵ: %.1f - %d\r\n", envirData.soil, envThreshold.Soil_Low_Threshold);
		if(envirData.soil <= envThreshold.Soil_Low_Threshold){
			Switch_SetState(SwitchSoil, ON);
		}
		else{
			Switch_SetState(SwitchSoil, OFF);
		}
		
		//������̼����
		u1_printf("������̼��ֵ: %d - %d\r\n", envirData.carbon, envThreshold.Carbon_Max_Threshold);
		if(envirData.carbon >= envThreshold.Carbon_Max_Threshold){
			Switch_SetState(SwitchCarbon, ON);
		}
		else{
			Switch_SetState(SwitchCarbon, OFF);
		}
		
		//����ǿ�Ȳ���
		u1_printf("����ǿ����ֵ: %d - %d\r\n", envirData.illumi, envThreshold.Light_Max_Threshold);
		if(envirData.illumi >= envThreshold.Light_Max_Threshold){
			if(LightSwitchState==0){
				LightSwitchState = 1;
				
				StartMotor(360*10);
			}
		}
		else{
			if(LightSwitchState){
				LightSwitchState = 0;
				
				StartMotor(-360*10);
			}
		}
	}
// TempSwitchState;				
// HumiSwitchState;				
// SoilSwitchState;				
// CarbonSwitchState;			
		
	//�Ƚϱ��ݵĿ���״̬
	if(TempSwitchState!=backup[0] \
		|| HumiSwitchState!=backup[1] \
		|| SoilSwitchState!=backup[2] \
		|| CarbonSwitchState!=backup[3]\
		|| LightSwitchState!=backup[4])
	{
		if(Connect_flag) SwitchState();					//�����������ݱ���
		
		backup[0] = TempSwitchState;						//���ݿ�������			
		backup[1] = HumiSwitchState;				
		backup[2] = SoilSwitchState;				
		backup[3] = CarbonSwitchState;	
		backup[4]	= LightSwitchState;	
	}
	
}

