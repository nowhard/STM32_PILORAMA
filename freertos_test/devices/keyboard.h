#ifndef KEYBOARD_H
#define KEYBOARD_H

#define KEYB_PORT_IN    	GPIOC
#define INIT_KEYB_PORT_IN 	RCC_AHB1Periph_GPIOC

#define KEYB_PORT_OUT    	GPIOA
#define INIT_KEYB_PORT_OUT	RCC_AHB1Periph_GPIOA

#define KO_0 GPIO_Pin_8
#define KO_1 GPIO_Pin_9
#define KO_2 GPIO_Pin_10
#define KO_3 GPIO_Pin_11

#define KI_0 GPIO_Pin_10
#define KI_1 GPIO_Pin_11
#define KI_2 GPIO_Pin_12
#define KI_3 GPIO_Pin_15

#define KEY_0		4
#define KEY_1		4
#define KEY_2		4
#define KEY_3		4
#define KEY_4		4
#define KEY_5		8
#define KEY_6		9
#define KEY_7		10
#define KEY_8		11
#define KEY_9		1

#define KEY_A		4
#define KEY_B		4
#define KEY_C		4

#define KEY_STAR	3
#define KEY_POINT	0
#define KEY_SHARP	4

#define KEYB_ROW_NUM	4
#define KEYB_COLUMN_NUM	4

void Keyboard_Init(void);

#endif
