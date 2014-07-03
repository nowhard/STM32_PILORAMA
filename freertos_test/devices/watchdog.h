#ifndef WATCHDOG_H
#define WATCHDOG_H
#include "stm32f4xx.h"

#define TASK_NUM	6//количество задач, кроме задачи ватчдога , в системе

enum
{
	SPI_TASK_1=0,
	SPI_TASK_2=1,
	SPI_TASK_3=2,
	PROTO_TASK=3,
	KEYBOARD_TASK=4,
	BUZZER_TASK=5
};

enum
{
	TASK_ACTIVE=0,
	TASK_IDLE=1
};

struct task_watch
{
	uint32_t counter;
	uint8_t  task_status;
};

void Watchdog_Init(void);

#endif
