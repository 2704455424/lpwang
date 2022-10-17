#ifndef __BH1750_H
#define __BH1750_H
#include "i2c.h"

#define	  SlaveAddress   0x46 			//定义器件在IIC总线中的从地址,根据ALT  ADDRESS地址引脚不同修改
																		//ALT  ADDRESS引脚接地时地址为0xA6，接电源时地址为0x3A

void BH1750_Init(void);
u16 BH1750_ReadData(void);


#endif

