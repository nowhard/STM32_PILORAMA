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
	BUZZER_EFFECT_0=0,
	BUZZER_EFFECT_1,
	BUZZER_EFFECT_2,
	BUZZER_EFFECT_3,
	BUZZER_EFFECT_4,
	BUZZER_EFFECT_5,
	BUZZER_EFFECT_6,
	BUZZER_EFFECT_7,
	BUZZER_EFFECT_NONE
};

#define BUZZER_PIN	GPIO_Pin_2
#define BUZZER_PORT GPIOB

struct buzzer{
	uint8_t buzzer_enable;//
	uint8_t buzzer_effect;//
};

void buzzer_init(void);
void buzzer_task(void *pvParameters );
void buzzer_set_buzz(uint8_t effect, uint8_t enable);


#endif
