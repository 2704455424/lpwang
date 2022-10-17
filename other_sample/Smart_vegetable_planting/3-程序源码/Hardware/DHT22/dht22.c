#include "dht22.h"


void DHT22_IO_OUT (void)
{
	//�˿ڱ�Ϊ���
	GPIO_InitTypeDef  GPIO_InitStructure; 	
  GPIO_InitStructure.GPIO_Pin = DHT22_IO; //ѡ��˿ںţ�0~15��all��                        
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //ѡ��IO�ӿڹ�����ʽ       
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //����IO�ӿ��ٶȣ�2/10/50MHz��    
	GPIO_Init(DHT22PORT, &GPIO_InitStructure);
}

void DHT22_IO_IN (void)
{
	//�˿ڱ�Ϊ����
	GPIO_InitTypeDef  GPIO_InitStructure; 	
  GPIO_InitStructure.GPIO_Pin = DHT22_IO; //ѡ��˿ںţ�0~15��all��                        
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //ѡ��IO�ӿڹ�����ʽ       
	GPIO_Init(DHT22PORT, &GPIO_InitStructure);
}

void DHT22_RST (void)
{
	//DHT22�˿ڸ�λ��������ʼ�źţ�IO���ͣ�
	DHT22_IO_OUT();
	GPIO_ResetBits(DHT22PORT,DHT22_IO); //	
	delay_ms(20); //��������18ms					
	GPIO_SetBits(DHT22PORT,DHT22_IO); //							
	delay_us(30); //��������20~40us
}

u8 Dht22_Check(void)
{ 
	//�ȴ�DHT22��Ӧ������1:δ��⵽DHT22������0:�ɹ���IO���գ�	   
  u8 retry=0;
  
	DHT22_IO_IN();//IO������״̬	 
  while (GPIO_ReadInputDataBit(DHT22PORT,DHT22_IO)&&retry<100){//DHT22������40~80us
    retry++;
    delay_us(1);
  }	 
  if(retry>=100)return 1; else retry=0;
  while (!GPIO_ReadInputDataBit(DHT22PORT,DHT22_IO)&&retry<100){//DHT22���ͺ���ٴ�����40~80us
    retry++;
    delay_us(1);
  }
  if(retry>=100)return 1;	    
  return 0;
}

u8 Dht22_ReadBit(void)
{
	//��DHT22��ȡһ��λ ����ֵ��1/0
  u8 retry=0;
  while(GPIO_ReadInputDataBit(DHT22PORT,DHT22_IO)&&retry<100){//�ȴ���Ϊ�͵�ƽ
    retry++;
    delay_us(1);
  }
  retry=0;
  while(!GPIO_ReadInputDataBit(DHT22PORT,DHT22_IO)&&retry<100){//�ȴ���ߵ�ƽ
    retry++;
    delay_us(1);
  }
  delay_us(40);//�ȴ�40us	//�����жϸߵ͵�ƽ��������1��0
  if(GPIO_ReadInputDataBit(DHT22PORT,DHT22_IO))return 1; 
	else return 0;		   
}

u8 Dht22_ReadByte(void)
{
  //��DHT22��ȡһ���ֽ�  ����ֵ������������
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
	//DHT22��ʼ��
	RCC_APB2PeriphClockCmd(DHT22_GPIO_CLOCK, ENABLE); //APB2����ʱ��ʹ��      
	DHT22_RST();																			//DHT22�˿ڸ�λ��������ʼ�ź�
	return Dht22_Check(); 														//�ȴ�DHT22��Ӧ
}

u8 DHT22_GetData(u16 *temp, u16 *humi)
{
	//��ȡһ������//ʪ��ֵ(ʮ���ƣ���Χ:20%~90%) ���¶�ֵ(ʮ���ƣ���Χ:0~50��)������ֵ��0,����;1,ʧ�� 
  u8 buf[5];
  u8 i;
  
	DHT22_RST();//DHT22�˿ڸ�λ��������ʼ�ź�
  if(Dht22_Check()==0)
	{ 
		//�ȴ�DHT22��Ӧ
		for(i=0;i<5;i++){//��ȡ5λ����
      buf[i]=Dht22_ReadByte(); //��������
    }
    if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4]){	//����У��
			*humi = ((u16)buf[0]<<8)|buf[1]; //��ʪ��ֵ����ָ��1
      *temp = ((u16)buf[2]<<8)|buf[3]; //���¶�ֵ����ָ��2
    }
  }
	else{
		return 1;
	}
    
	return 0;	    
}


u8 DHT11_GetData(u8 *temp, u8 *humi)
{
	//��ȡһ������//ʪ��ֵ(ʮ���ƣ���Χ:20%~90%) ���¶�ֵ(ʮ���ƣ���Χ:0~50��)������ֵ��0,����;1,ʧ�� 
  u8 buf[5];
  u8 i;
  
	DHT22_RST();//DHT22�˿ڸ�λ��������ʼ�ź�
  if(Dht22_Check()==0)
	{ 
		//�ȴ�DHT22��Ӧ
		for(i=0;i<5;i++){//��ȡ5λ����
      buf[i]=Dht22_ReadByte(); //��������
    }
    if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4]){	//����У��
			*humi = buf[0]; //��ʪ��ֵ����ָ��1
      *temp = buf[2]; //���¶�ֵ����ָ��2
    }
  }
	else{
		return 1;
	}
    
	return 0;	    
}

