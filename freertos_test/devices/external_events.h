#ifndef  EXTERNAL_EVENTS_H
#define  EXTERNAL_EVENTS_H
#include "stm32f4xx.h"

void External_Events_Init(void);

#define DRIVE_EXT_EVENTS_PORT    		GPIOA
#define DRIVE_EXT_EVENTS_PORT_RCC 		RCC_AHB1Periph_GPIOA

#define DRIVE_ERROR 		GPIO_Pin_0
#define DRIVE_LIMIT_UP 		GPIO_Pin_6
#define DRIVE_LIMIT_DOWN 	GPIO_Pin_7

#endif
