#ifndef WATCHDOG_H
#define WATCHDOG_H
#include "stm32f4xx.h"

#define TASK_NUM	4//количество задач, кроме задачи ватчдога , в системе

enum
{
	SPI_TASK_1		=0,
	MENU_TASK		=1,
	KEYBOARD_TASK	=2,
	BUZZER_TASK		=3
};

enum
{
	TASK_ACTIVE	=0,
	TASK_IDLE	=1
};

struct task_watch
{
	uint32_t counter;
	uint8_t  task_status;
};

void Watchdog_Init(void);

#endif
