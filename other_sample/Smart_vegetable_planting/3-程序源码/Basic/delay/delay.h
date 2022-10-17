#ifndef __DELAY_H
#define __DELAY_H 			   
#include "sys.h"
 
#define AHB_INPUT  72  //请按RCC中设置的AHB时钟频率填写到这里（单位MHz）
 
void delay_s(u16 s);
void delay_ms(u16 ms);
void delay_us(u32 us);

#endif





























