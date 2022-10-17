#include "dht22.h"


void DHT22_IO_OUT (void)
{
	//端口变为输出
	GPIO_InitTypeDef  GPIO_InitStructure; 	
  GPIO_InitStructure.GPIO_Pin = DHT22_IO; //选择端口号（0~15或all）                        
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //选择IO接口工作方式       
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //设置IO接口速度（2/10/50MHz）    
	GPIO_Init(DHT22PORT, &GPIO_InitStructure);
}

void DHT22_IO_IN (void)
{
	//端口变为输入
	GPIO_InitTypeDef  GPIO_InitStructure; 	
  GPIO_InitStructure.GPIO_Pin = DHT22_IO; //选择端口号（0~15或all）                        
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //选择IO接口工作方式       
	GPIO_Init(DHT22PORT, &GPIO_InitStructure);
}

void DHT22_RST (void)
{
	//DHT22端口复位，发出起始信号（IO发送）
	DHT22_IO_OUT();
	GPIO_ResetBits(DHT22PORT,DHT22_IO); //	
	delay_ms(20); //拉低至少18ms					
	GPIO_SetBits(DHT22PORT,DHT22_IO); //							
	delay_us(30); //主机拉高20~40us
}

u8 Dht22_Check(void)
{ 
	//等待DHT22回应，返回1:未检测到DHT22，返回0:成功（IO接收）	   
  u8 retry=0;
  
	DHT22_IO_IN();//IO到输入状态	 
  while (GPIO_ReadInputDataBit(DHT22PORT,DHT22_IO)&&retry<100){//DHT22会拉低40~80us
    retry++;
    delay_us(1);
  }	 
  if(retry>=100)return 1; else retry=0;
  while (!GPIO_ReadInputDataBit(DHT22PORT,DHT22_IO)&&retry<100){//DHT22拉低后会再次拉高40~80us
    retry++;
    delay_us(1);
  }
  if(retry>=100)return 1;	    
  return 0;
}

u8 Dht22_ReadBit(void)
{
	//从DHT22读取一个位 返回值：1/0
  u8 retry=0;
  while(GPIO_ReadInputDataBit(DHT22PORT,DHT22_IO)&&retry<100){//等待变为低电平
    retry++;
    delay_us(1);
  }
  retry=0;
  while(!GPIO_ReadInputDataBit(DHT22PORT,DHT22_IO)&&retry<100){//等待变高电平
    retry++;
    delay_us(1);
  }
  delay_us(40);//等待40us	//用于判断高低电平，即数据1或0
  if(GPIO_ReadInputDataBit(DHT22PORT,DHT22_IO))return 1; 
	else return 0;		   
}

u8 Dht22_ReadByte(void)
{
  //从DHT22读取一个字节  返回值：读到的数据
  u8 i,dat;
    
	dat=0;
  for (i=0;i<8;i++){ 
    dat<<=1; 
    dat|=Dht22_ReadBit();
  }						    
  return dat;
}

u8 DHT22_Init(void)
{
	//DHT22初始化
	RCC_APB2PeriphClockCmd(DHT22_GPIO_CLOCK, ENABLE); //APB2外设时钟使能      
	DHT22_RST();																			//DHT22端口复位，发出起始信号
	return Dht22_Check(); 														//等待DHT22回应
}

u8 DHT22_GetData(u16 *temp, u16 *humi)
{
	//读取一次数据//湿度值(十进制，范围:20%~90%) ，温度值(十进制，范围:0~50°)，返回值：0,正常;1,失败 
  u8 buf[5];
  u8 i;
  
	DHT22_RST();//DHT22端口复位，发出起始信号
  if(Dht22_Check()==0)
	{ 
		//等待DHT22回应
		for(i=0;i<5;i++){//读取5位数据
      buf[i]=Dht22_ReadByte(); //读出数据
    }
    if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4]){	//数据校验
			*humi = ((u16)buf[0]<<8)|buf[1]; //将湿度值放入指针1
      *temp = ((u16)buf[2]<<8)|buf[3]; //将温度值放入指针2
    }
  }
	else{
		return 1;
	}
    
	return 0;	    
}


u8 DHT11_GetData(u8 *temp, u8 *humi)
{
	//读取一次数据//湿度值(十进制，范围:20%~90%) ，温度值(十进制，范围:0~50°)，返回值：0,正常;1,失败 
  u8 buf[5];
  u8 i;
  
	DHT22_RST();//DHT22端口复位，发出起始信号
  if(Dht22_Check()==0)
	{ 
		//等待DHT22回应
		for(i=0;i<5;i++){//读取5位数据
      buf[i]=Dht22_ReadByte(); //读出数据
    }
    if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4]){	//数据校验
			*humi = buf[0]; //将湿度值放入指针1
      *temp = buf[2]; //将温度值放入指针2
    }
  }
	else{
		return 1;
	}
    
	return 0;	    
}

