#ifndef RELAY_H
#define RELAY_H
#include "stm32f4xx.h"

void Relay_Init(void);
void Relay_Set_State(uint8_t state);

#define RELAY_SIGN2	GPIO_Pin_5
#define RELAY_SIGN3	GPIO_Pin_4
#define RELAY_SIGN4	GPIO_Pin_3

#define RELAY_PORT GPIOC

#endif
