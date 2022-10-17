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
	
  IIC_Start();                         //��ʼ�ź�
  IIC_Send_Byte(SlaveAddress+1);       //�����豸��ַ+���ź�
	IIC_Wait_Ack();
	HSB = IIC_Read_Byte(1);
	LSB = IIC_Read_Byte(0);
  IIC_Stop();                         //ֹͣ�ź�
  
	return ((HSB<<8)|LSB);
}

/*  BH1750��ʼ��  */
void BH1750_Init(void)
{
	Single_Write_BH1750(0x01); 
}

/*  BH1750��ȡ����  */
u16 BH1750_ReadData(void)
{
	u16 dat;
	
	Single_Write_BH1750(0x01);   			// power on
  Single_Write_BH1750(0x10);   			// H- resolution mode
  delay_ms(180);               			//��ʱ180ms
  dat = Multiple_Read_BH1750(); 		//�����������ݣ��洢��BUF��
	
	return dat;
}


