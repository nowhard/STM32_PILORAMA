#ifndef KEYBOARD_H
#define KEYBOARD_H

#define KEYB_PORT    	GPIOB
#define INIT_KEYB_PORT 	RCC_AHB1Periph_GPIOB

#define KO_0 GPIO_Pin_5
#define KO_1 GPIO_Pin_6
#define KO_2 GPIO_Pin_7
#define KO_3 GPIO_Pin_1

#define KI_0 GPIO_Pin_0
#define KI_1 0//GPIO_Pin_2
#define KI_2 GPIO_Pin_9

#define KEY_5		8
#define KEY_6		9
#define KEY_7		10
#define KEY_8		11
#define KEY_SHIFT	0
#define KEY_9		1
#define KEY_0		2
#define KEY_ENTER	3

void Keyboard_Init(void);

#endif
