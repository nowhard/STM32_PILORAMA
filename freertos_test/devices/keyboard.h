#ifndef KEYBOARD_H
#define KEYBOARD_H

#define KEYB_PORT_IN    	GPIOA
#define INIT_KEYB_PORT_IN 	RCC_AHB1Periph_GPIOA

#define KEYB_PORT_OUT    	GPIOC
#define INIT_KEYB_PORT_OUT	RCC_AHB1Periph_GPIOC

#define KO_0 GPIO_Pin_8
#define KO_1 GPIO_Pin_9
#define KO_2 GPIO_Pin_10
#define KO_3 GPIO_Pin_11

#define KI_0 GPIO_Pin_10
#define KI_1 GPIO_Pin_11
#define KI_2 GPIO_Pin_12
#define KI_3 GPIO_Pin_15

//#define KEY_0		4
//#define KEY_1		4
//#define KEY_2		4
//#define KEY_3		4
//#define KEY_4		4
//#define KEY_5		8
//#define KEY_6		9
//#define KEY_7		10
//#define KEY_8		11
//#define KEY_9		1
//
//#define KEY_A		4
//#define KEY_B		4
//#define KEY_C		4
//
//#define KEY_STAR	3
//#define KEY_POINT	0
//#define KEY_SHARP	4

enum
{
 KEY_0=0,
 KEY_1,
 KEY_2,
 KEY_3,
 KEY_4,
 KEY_5,
 KEY_6,
 KEY_7,
 KEY_8,
 KEY_9,

 KEY_A,
 KEY_B,
 KEY_C,

 KEY_STAR,
 KEY_POINT,
 KEY_SHARP,

 KEY_0_LONG=16,
 KEY_1_LONG,
 KEY_2_LONG,
 KEY_3_LONG,
 KEY_4_LONG,
 KEY_5_LONG,
 KEY_6_LONG,
 KEY_7_LONG,
 KEY_8_LONG,
 KEY_9_LONG,

 KEY_A_LONG,
 KEY_B_LONG,
 KEY_C_LONG,

 KEY_STAR_LONG,
 KEY_POINT_LONG,
 KEY_SHARP_LONG,
};

#define KEYB_ROW_NUM	4
#define KEYB_COLUMN_NUM	4

void Keyboard_Init(void);

#endif
