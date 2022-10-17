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
EnvirData envirData;								//环境数据结构体初始化
EnvirThreshold envThreshold;				//环境阈值结构体初始化


/********************************************************************************************************
数据处理部分
*********************************************************************************************************/
/*  环境数据状态  */
void EnvirState(void)
{
	char temp[400];
	
	//构建上传服务器的环境数据
	sprintf(temp, "{\\22method\\22:\\22thing.event.property.post\\22,\\22id\\22:\\223232323200\\22,\\22params\\22:\
	{\\22EnvironmentTemperature\\22:%.1f,\\22EnvironmentHumidity\\22:%.1f,\\22SoilMoisture\\22:%.1f,\\22LightLux\\22:%d,\\22CO2\\22:%d},\\22version\\22:\\221.0.0\\22}",\
	envirData.temp, envirData.humi, envirData.soil, envirData.illumi, envirData.carbon);
	
	//将数据放入发送缓冲区
	CAT1_PushData(P_TOPIC_NAME, temp);
}

/*  开关数据状态  */
void SwitchState(void)
{
	char temp[400];
	
	//构建上传服务器的开关数据
	sprintf(temp, "{\\22method\\22:\\22thing.event.property.post\\22,\\22id\\22:\\223232323201\\22,\\22params\\22:\
	{\\22HeatSwitch\\22:%d,\\22Humidified\\22:%d,\\22WaterOutletSwitch\\22:%d,\\22WotkSwitch\\22:%d,\\22PowerSwitch\\22:%d,\\22SwitchMode\\22:%d},\\22version\\22:\\221.0.0\\22}",\
	TempSwitchState, HumiSwitchState, SoilSwitchState, LightSwitchState, CarbonSwitchState, SwitchMode);
	
	//将数据放入发送缓冲区
	CAT1_PushData(P_TOPIC_NAME, temp);
}


/*  系统数据1状态  */
void SysdatState(void)
{
	u8 TxData[400];
		
	//构建上传服务器的开关数据
	sprintf((char*)TxData, "AT+MPUB=\"%s\",1,0,\"{\\22method\\22:\\22thing.event.property.post\\22,\
	\\22params\\22:{\\22EnvTemp_Low_Threshold\\22:%d,\\22EnvHumidity_Low_Threshold\\22:%d,\\22SoilHumidity_Low_Threshold\\22:%d,\\22LightLux_Max_Threshold\\22:%d,\\22CO2_Max_Threshold\\22:%d}}\"\r\n", \
	P_TOPIC_NAME, envThreshold.Temp_Low_Threshold, envThreshold.Humi_Low_Threshold, envThreshold.Soil_Low_Threshold,envThreshold.Light_Max_Threshold, envThreshold.Carbon_Max_Threshold);		
	
	u1_printf("发布的数据:\r\n%s\r\n", TxData);
	CAT1_Publish(TxData, 50);
}

/*  系统状态回应  */
void SysdatStateAck(char *mark, u16	data)
{
	u8 temp[200];
	
	sprintf((char*)temp, "AT+MPUB=\"%s\",1,0,\"{\\22method\\22:\\22thing.event.property.post\\22,\\22params\\22:{\\22%s\\22:%d}}\"\r\n",\
	P_TOPIC_NAME, mark, data);
	
	CAT1_Publish(temp, 50);
}

/*  初始化数据  */
void DATA_Init(void)
{
	u8 temp;
	
	temp = AT24CXX_ReadOneByte(CHECK_ADDR);		//避免每次开机都写AT24CXX	
	if(temp != 0x5A)
	{
		AT24CXX_WriteOneByte(CHECK_ADDR, 0x5A);
		//初始化相关阈值
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
		u1_printf("第一次启动！\r\n");
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
		u1_printf("非第一次启动！\r\n");
	}

	u1_printf("系统初始化数据:\r\n模式:%d\r\n周期:%d\r\n温度:%d\r\n湿度:%d\r\n土壤:%d\r\n二氧化碳:%d\r\n光照强度:%d\r\n",\
	SwitchMode,UploadTime, envThreshold.Temp_Low_Threshold, envThreshold.Humi_Low_Threshold, envThreshold.Soil_Low_Threshold,\
	envThreshold.Carbon_Max_Threshold, envThreshold.Light_Max_Threshold);
}

/*  发布初始化数据  */
void PubInitData(void)
{
	SysdatState();					//系统数据
}

/*  处理IoT开关命令 */
char SwitchIoTCmd(void)
{
	char status = 0;
	
	if(strstr((char *)RxDataOutPtr+2,"\"params\":{\"HeatSwitch\":1}")){										//检测到加热开关为 1       	
		status++;																																						//标志位加一
		SwitchMode = 1;								                  																		//标志位置1
		Switch_SetState(SwitchTemp, ON);                      															//打开加热开关                                   
	}	else if(strstr((char *)RxDataOutPtr+2,"\"params\":{\"HeatSwitch\":0}")){ 						//检测到加热开关为 0    
		status++;																																						//标志位加一
		SwitchMode = 1;								                  																		//标志位置1
		Switch_SetState(SwitchTemp, OFF);    																								//关闭加热开关 	
	}
	
	if(strstr((char *)RxDataOutPtr+2,"\"params\":{\"Humidified\":1}")){										//检测到加湿开关为 1       	
		status++;																																						//标志位加一
		SwitchMode = 1;								                  																		//标志位置1
		Switch_SetState(SwitchHumi, ON);                      															//打开加湿开关                                   
	}	else if(strstr((char *)RxDataOutPtr+2,"\"params\":{\"Humidified\":0}")){ 						//检测到加湿开关为 0    
		status++;																																						//标志位加一
		SwitchMode = 1;								                  																		//标志位置1
		Switch_SetState(SwitchHumi, OFF);    																								//关闭加湿开关                                                          
	}
	
	if(strstr((char *)RxDataOutPtr+2,"\"params\":{\"WaterOutletSwitch\":1}")){						//检测到浇水开关为 1       	
		status++;																																						//标志位加一
		SwitchMode = 1;								                  																		//标志位置1
		Switch_SetState(SwitchSoil, ON);                      															//打开浇水开关                                   
	}	else if(strstr((char *)RxDataOutPtr+2,"\"params\":{\"WaterOutletSwitch\":0}")){ 		//检测到浇水开关为 0    
		status++;																																						//标志位加一
		SwitchMode = 1;								                  																		//标志位置1
		Switch_SetState(SwitchSoil, OFF);    																								//关闭浇水开关                                                          
	}
		
	if(strstr((char *)RxDataOutPtr+2,"\"params\":{\"PowerSwitch\":1}")){									//检测到通风开关为 1       	
		status++;																																						//标志位加一
		SwitchMode = 1;								                  																		//标志位置1 
		Switch_SetState(SwitchCarbon, ON);                      														//打开通风开关                                   
	}	else if(strstr((char *)RxDataOutPtr+2,"\"params\":{\"PowerSwitch\":0}")){ 					//检测到通风开关为 0    
		status++;																																						//标志位加一
		SwitchMode = 1;								                  																		//标志位置1 
		Switch_SetState(SwitchCarbon, OFF);    																							//关闭通风开关                                                          
	}
	
	if(strstr((char *)RxDataOutPtr+2,"\"params\":{\"WotkSwitch\":1}")){										//检测到遮阳开关为 1       	
		status++;																																						//标志位加一
		LightSwitchState = 1;								                  															//打开遮阳开关  
		SwitchMode = 1;								                  																		//标志位置1 		
		StartMotor(360*10);																																	//电机正转
	}	else if(strstr((char *)RxDataOutPtr+2,"\"params\":{\"WotkSwitch\":0}")){ 						//检测到遮阳开关为 0    
		status++;																																						//标志位加一
		StartMotor(-360*10);																																//电机反转
		SwitchMode = 1;								                  																		//标志位置1 
		LightSwitchState = 0;    																														//关闭遮阳开关                                                          
	}
	
	if(strstr((char *)RxDataOutPtr+2,"\"params\":{\"SwitchMode\":1}")){										//开关模式为 1       	
		status++;																																						//标志位加一
		SwitchMode = 1;								                  																		//标志位置1 
		AT24CXX_WriteOneByte(SwitchMode_ADDR, SwitchMode);																	//保存数据

	}	else if(strstr((char *)RxDataOutPtr+2,"\"params\":{\"SwitchMode\":0}")){ 						//开关模式为 0    
		status++;																																						//标志位加一
		SwitchMode = 0;    																																	//标志位置0
		AT24CXX_WriteOneByte(SwitchMode_ADDR, SwitchMode);																	//保存数据
	}
	return status;
}

/*  系统数据命令  */
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
		u1_printf("低温阈值: %d\r\n", envThreshold.Temp_Low_Threshold);
		SysdatStateAck("EnvTemp_Low_Threshold", envThreshold.Temp_Low_Threshold);
	}
	
	if(strstr((char *)RxDataOutPtr+2,"\"EnvHumidity_Low_Threshold\":")){
		status=2;
		presult = strstr((char *)RxDataOutPtr+2,"\"EnvHumidity_Low_Threshold\":");		
		sscanf(presult, "%*[^:]:%d", &num);	
		envThreshold.Humi_Low_Threshold = (u8)num;
		AT24CXX_WriteOneByte(MinHumi_ADDR, envThreshold.Humi_Low_Threshold);
		u1_printf("低湿阈值: %d\r\n", envThreshold.Humi_Low_Threshold);
		SysdatStateAck("EnvHumidity_Low_Threshold", envThreshold.Humi_Low_Threshold);
	}
		
	if(strstr((char *)RxDataOutPtr+2,"\"SoilHumidity_Low_Threshold\":")){
		status=3;
		presult = strstr((char *)RxDataOutPtr+2,"\"SoilHumidity_Low_Threshold\":");		
		sscanf(presult, "%*[^:]:%d", &num);	
		envThreshold.Soil_Low_Threshold = (u8)num;
		AT24CXX_WriteOneByte(MinSoil_ADDR, envThreshold.Soil_Low_Threshold);
		u1_printf("土壤阈值: %d\r\n", envThreshold.Soil_Low_Threshold);
		SysdatStateAck("SoilHumidity_Low_Threshold", envThreshold.Soil_Low_Threshold);
	}
		
	if(strstr((char *)RxDataOutPtr+2,"\"LightLux_Max_Threshold\":")){
		status=4;
		presult = strstr((char *)RxDataOutPtr+2,"\"LightLux_Max_Threshold\":");		
		sscanf(presult, "%*[^:]:%d", &num);	
		envThreshold.Light_Max_Threshold = (u16)num;
		AT24CXX_WriteLenByte(MaxLight_ADDR, envThreshold.Light_Max_Threshold, 2);
		u1_printf("光照阈值: %d\r\n", envThreshold.Light_Max_Threshold);
		SysdatStateAck("LightLux_Max_Threshold", envThreshold.Light_Max_Threshold);
	}
		
	if(strstr((char *)RxDataOutPtr+2,"\"CO2_Max_Threshold\":")){
		status=5;
		presult = strstr((char *)RxDataOutPtr+2,"\"CO2_Max_Threshold\":");		
		sscanf(presult, "%*[^:]:%d", &num);	
		envThreshold.Carbon_Max_Threshold = (u16)num;
		AT24CXX_WriteLenByte(MaxCarbon_ADDR, envThreshold.Carbon_Max_Threshold, 2);
		u1_printf("CO2阈值: %d\r\n", envThreshold.Carbon_Max_Threshold);
		SysdatStateAck("CO2_Max_Threshold", envThreshold.Carbon_Max_Threshold);
	}
	
	return status;
}

/*  网络监控  */
void NetMonitor(void)
{
	if(strstr((char *)RxDataOutPtr+2,"CLOSED")){
		Connect_flag = 0;
	}
}

/*  处理阿里的命令  */
void DealAliCmd(void)
{
	NetMonitor();													//网络监控
	SystemDataCmd();											//系统数据处理

	if(SwitchIoTCmd() != 0) {							//说明接收到开关命令
		SwitchState();											//回复服务器数据	
	}	
}

/********************************************************************************************************
数据监控部分
*********************************************************************************************************/
//AHT10 刷新监控
void Aht10Monitor(void)
{
	double tempdata,humidata;	
	
	AHT10_Data(&tempdata,&humidata);
	envirData.temp = (float)tempdata;
	envirData.humi = (float)humidata;
	u1_printf("\r\n温度：%.1f'C\r\n湿度：%.1f%%\r\n",envirData.temp,envirData.humi);
}

// 二氧化碳浓度监控
void CarbonMonitor(void)
{
	u16 temp;
	
	temp = ADC_DMA_IN[0];
	envirData.carbon = (temp*Verf/4096.0-0.4)*CarbonValue/1.6;				//计算公式 （V0-0.4V）*量程（ppm）/（2.0V-0.4V）
	
	u1_printf("二氧化碳：%dppm\r\n", envirData.carbon);	
}

// 土壤湿度监控
void SoilMonitor(void)
{
	u16 temp;
	temp = ADC_DMA_IN[1];
	envirData.soil = SoilValue-(temp*SoilValue/4096.0)+30;
	
	u1_printf("土壤湿度：%.1f%%\r\n", envirData.soil);
}

// 光照强度监控
void illumiMonitor(void)
{
	u16 dat;
	
	dat = BH1750_ReadData();
	envirData.illumi = dat/1.2;
	u1_printf("光照强度: %dlx\r\n", envirData.illumi);
}


// 秒监控
void SecMonitor(void)
{
	static u8 cntMin = DEF_UploadTime;	//1分钟计数
		
	if(flag10s){												//定时1s时间到了,进行读取温度
		flag10s = 0;											//定时清0
		
		/*  这里添加数据采集  */
		Aht10Monitor();										//温湿度监控
		SoilMonitor();										//土壤湿度监控
		CarbonMonitor();									//二氧化碳监控
		illumiMonitor();									//光照强度监控	
		SwitchMonitor();									//开关监控	
		
		
		if(Connect_flag == 0){
			__set_FAULTMASK(1);														//关闭所有中断
			NVIC_SystemReset();														//复位系统
		}
	}
	
	if(flag1min){												//一分钟定时时间到
		flag1min = 0;											//计数清0
		cntMin++;
		
		if(cntMin >= UploadTime)					//定时上传时间到
		{
			cntMin = 0;											//计数清零
			
			if(Connect_flag){
				EnvirState();									//构建上传给服务器的数据报文
			}
		}
	}
	
}




void SwitchMonitor(void)
{
//	SwitchTemp,					温度开关
//	SwitchHumi,					湿度开关
//	SwitchSoil,					土壤开关
//	SwitchLight,				光照开关
//	SwitchCarbon,				二氧化碳开关				
	static u8 backup[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	
	if(SwitchMode == 0)
	{
		//温度部分		
		u1_printf("\r\n温度阈值: %.1f - %d\r\n", envirData.temp, envThreshold.Temp_Low_Threshold);
		if(envirData.temp <= envThreshold.Temp_Low_Threshold){
			Switch_SetState(SwitchTemp, ON);
		}
		else{
			Switch_SetState(SwitchTemp, OFF);
		}
		
		//湿度部分
		u1_printf("湿度阈值: %.1f - %d\r\n", envirData.humi, envThreshold.Humi_Low_Threshold);
		if(envirData.humi <= envThreshold.Humi_Low_Threshold){
			Switch_SetState(SwitchHumi, ON);
		}
		else{
			Switch_SetState(SwitchHumi, OFF);
		}
		
		//土壤湿度部分
		u1_printf("土壤阈值: %.1f - %d\r\n", envirData.soil, envThreshold.Soil_Low_Threshold);
		if(envirData.soil <= envThreshold.Soil_Low_Threshold){
			Switch_SetState(SwitchSoil, ON);
		}
		else{
			Switch_SetState(SwitchSoil, OFF);
		}
		
		//二氧化碳部分
		u1_printf("二氧化碳阈值: %d - %d\r\n", envirData.carbon, envThreshold.Carbon_Max_Threshold);
		if(envirData.carbon >= envThreshold.Carbon_Max_Threshold){
			Switch_SetState(SwitchCarbon, ON);
		}
		else{
			Switch_SetState(SwitchCarbon, OFF);
		}
		
		//光照强度部分
		u1_printf("光照强度阈值: %d - %d\r\n", envirData.illumi, envThreshold.Light_Max_Threshold);
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
		
	//比较备份的开关状态
	if(TempSwitchState!=backup[0] \
		|| HumiSwitchState!=backup[1] \
		|| SoilSwitchState!=backup[2] \
		|| CarbonSwitchState!=backup[3]\
		|| LightSwitchState!=backup[4])
	{
		if(Connect_flag) SwitchState();					//构建开关数据报文
		
		backup[0] = TempSwitchState;						//备份开关数据			
		backup[1] = HumiSwitchState;				
		backup[2] = SoilSwitchState;				
		backup[3] = CarbonSwitchState;	
		backup[4]	= LightSwitchState;	
	}
	
}

