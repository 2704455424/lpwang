#ifndef __AHT10_H
#define __AHT10_H
#include "stm32f10x.h"  
#include "i2c.h" 

void AHT10_Reset(void);
void AHT10_ReadData(void);
char AHT10_ReadStatus(void);
char AHT10_Read_CalEnable(void); 
char AHT10_Init(void);
void AHT10_Data(double *temp, double *humi);	

#endif
















