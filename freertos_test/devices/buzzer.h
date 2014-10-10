#ifndef BUZZER_H
#define BUZZER_H

#include "stm32f4xx.h"

enum
{
	BUZZER_OFF=0,
	BUZZER_ON=1
};

enum
{
	BUZZER_EFFECT_1_BEEP=0,
	BUZZER_EFFECT_2_BEEP,
	BUZZER_EFFECT_3_BEEP,
	BUZZER_EFFECT_INFINITE_BEEP,
	BUZZER_EFFECT_LONG_BEEP,
	BUZZER_EFFECT_NONE
};

enum
{
	FROM_TASK=0,
	FROM_ISR,
};

#define BUZZER_BUS	RCC_AHB1Periph_GPIOB
#define BUZZER_PIN	GPIO_Pin_2
#define BUZZER_PORT GPIOB

struct buzzer{
	uint8_t buzzer_enable;//
	uint8_t buzzer_effect;//
};

void buzzer_init(void);
void buzzer_task(void *pvParameters );
void buzzer_set_buzz(uint8_t effect, uint8_t enable, uint8_t function_start_type);


#endif
