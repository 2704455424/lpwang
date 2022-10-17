#include "bh1750.h"
#include "delay.h"


void Single_Write_BH1750(u8 REG_Address)
{
	IIC_Start();
	IIC_Send_Byte(SlaveAddress);
	IIC_Wait_Ack();
	IIC_Send_Byte(REG_Address);
	IIC_Wait_Ack();
	IIC_Stop();
}



u16 Multiple_Read_BH1750(void)
{  
	u16 HSB, LSB;	
	
  IIC_Start();                         //起始信号
  IIC_Send_Byte(SlaveAddress+1);       //发送设备地址+读信号
	IIC_Wait_Ack();
	HSB = IIC_Read_Byte(1);
	LSB = IIC_Read_Byte(0);
  IIC_Stop();                         //停止信号
  
	return ((HSB<<8)|LSB);
}

/*  BH1750初始化  */
void BH1750_Init(void)
{
	Single_Write_BH1750(0x01); 
}

/*  BH1750获取数据  */
u16 BH1750_ReadData(void)
{
	u16 dat;
	
	Single_Write_BH1750(0x01);   			// power on
  Single_Write_BH1750(0x10);   			// H- resolution mode
  delay_ms(180);               			//延时180ms
  dat = Multiple_Read_BH1750(); 		//连续读出数据，存储在BUF中
	
	return dat;
}


