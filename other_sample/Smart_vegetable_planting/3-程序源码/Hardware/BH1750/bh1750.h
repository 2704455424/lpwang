#ifndef __BH1750_H
#define __BH1750_H
#include "i2c.h"

#define	  SlaveAddress   0x46 			//����������IIC�����еĴӵ�ַ,����ALT  ADDRESS��ַ���Ų�ͬ�޸�
																		//ALT  ADDRESS���Žӵ�ʱ��ַΪ0xA6���ӵ�Դʱ��ַΪ0x3A

void BH1750_Init(void);
u16 BH1750_ReadData(void);


#endif

