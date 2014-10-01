#ifndef DRIVE_H
#define DRIVE_H
#include "stm32f4xx.h"

enum
{
	MOVE_TYPE_NONE		=0,
	MOVE_TYPE_RELATIVE_UP,
	MOVE_TYPE_RELATIVE_DOWN,
	MOVE_TYPE_ABSOLUTE_UP,
	MOVE_TYPE_ABSOLUTE_DOWN,
};

enum
{
	DRIVE_OK=0,
	DRIVE_ERROR,
};

enum
{
	STOP_USER=1,
	STOP_HI_SENSOR,
	STOP_LO_SENSOR,
};

enum
{
	DRIVE_SPEED_LOW=0,
	DRIVE_SPEED_HI,
};

struct backup_registers
{
	uint16_t F_01_cal_up;
	uint16_t F_02_cal_down;
	uint16_t F_03_cal_syncro;
	uint16_t F_04_current_position;
	uint16_t F_05_cal_speed_down;
	uint16_t F_06_cal_stop;
};

struct drive
{
	struct 		backup_registers *bkp_reg;
	uint32_t 	current_position;
	uint8_t 	move_type_flag;
	uint8_t 	error_flag;
	uint8_t 	stop_type;
};


#define DRIVE_ERROR_PORT    		GPIOA
#define DRIVE_ERROR_PORT_RCC 		RCC_AHB1Periph_GPIOA

#define DRIVE_ERROR 	GPIO_Pin_0

#define DRIVE_CONTROL_PORT    		GPIOC
#define DRIVE_CONTROL_PORT_RCC		RCC_AHB1Periph_GPIOC

#define DRIVE_FORWARD  	GPIO_Pin_0
#define DRIVE_BACKWARD 	GPIO_Pin_1
#define DRIVE_RESET		GPIO_Pin_2
#define DRIVE_SPEED		GPIO_Pin_3


void Drive_Init(void);
uint8_t Drive_Start(uint8_t move_type,int16_t move_val);
uint8_t Drive_Set_Speed(uint8_t val_speed);
uint8_t Drive_Stop(uint8_t stop_type);

uint32_t Drive_MM_To_Impulse(uint16_t val_mm);
uint16_t Drive_Impulse_To_MM(uint32_t val_impulse);


#endif
