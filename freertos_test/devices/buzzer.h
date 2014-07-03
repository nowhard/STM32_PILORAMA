#ifndef BUZZER_H
#define BUZZER_H

#include "stm32f4xx.h"

//#define BUZZER_ON	0x1
//#define BUZZER_OFF	0x0

enum
{
	BUZZER_OFF=0,
	BUZZER_ON=1
};

//#define BUZZER_EFFECT_0	0x0
//#define BUZZER_EFFECT_1	0x1
//#define BUZZER_EFFECT_2	0x2
//#define BUZZER_EFFECT_3	0x3
//#define BUZZER_EFFECT_4	0x4
//#define BUZZER_EFFECT_5	0x5
//#define BUZZER_EFFECT_6	0x6
//#define BUZZER_EFFECT_7	0x7

//#define BUZZER_EFFECT_NONE	0xF

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

#define BUZZER_PIN	GPIO_Pin_6
#define BUZZER_PORT GPIOC

struct buzzer{
	uint8_t buzzer_enable;//
	uint8_t buzzer_effect;//
};

void buzzer_init(void);
void buzzer_task(void *pvParameters );
void buzzer_set_buzz(uint8_t effect, uint8_t enable);


#endif
