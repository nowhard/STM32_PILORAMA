#ifndef POWER_H
#define POWER_H
#include "stm32f4xx.h"
void Power_Init(void);

void Power_On_Channel_1(void);
void Power_On_Channel_2(void);
void Power_On_Channel_3(void);

void Power_Off_Channel_1(void);
void Power_Off_Channel_2(void);
void Power_Off_Channel_3(void);

uint8_t Power_Channel_1_OK(void);
uint8_t Power_Channel_2_OK(void);
uint8_t Power_Channel_3_OK(void);

#endif
