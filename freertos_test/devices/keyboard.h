#ifndef KEYBOARD_H
#define KEYBOARD_H

#define KEY_X1 GPIO_Pin_2
#define KEY_X2 GPIO_Pin_1
#define KEY_X3 GPIO_Pin_0

#define PORT_X	GPIOC

#define KEY_Y1 GPIO_Pin_0
#define KEY_Y2 GPIO_Pin_1
#define KEY_Y3 GPIO_Pin_2

#define PORT_Y	GPIOB

void keyboard_init(void);
void keyboard_task(void *pvParameters );

#endif
