#ifndef DRIVE_H
#define DRIVE_H
#include "stm32f4xx.h"

enum
{
	MOVE_TYPE_NONE		=0,
	MOVE_TYPE_RELATIVE_UP,
	MOVE_TYPE_RELATIVE_DOWN,
	MOVE_TYPE_ABSOLUTE,
};

enum
{
	DRIVE_OK=0,
	DRIVE_ERR,
};

enum
{
	STOP_NONE=0,
	STOP_USER,
	STOP_HI_SENSOR,
	STOP_LO_SENSOR,
	STOP_INVERTOR_ERROR,
	STOP_END_OF_OPERATION,
};

enum
{
	DRIVE_SPEED_LOW=0,
	DRIVE_SPEED_HI,
};

enum
{
	DRIVE_DIRECTION_UP=0,
	DRIVE_DIRECTION_DOWN,
};

struct mm_imp
{
	uint16_t mm;
	uint32_t imp;
};

struct backup_registers
{
	struct mm_imp F_01_cal_up;
	struct mm_imp F_02_cal_down;
	struct mm_imp F_03_cal_syncro;
	uint16_t F_04_function_back;
	uint16_t F_05_cal_speed_down;
	uint16_t F_06_cal_stop;
	uint32_t backup_current_position;
};

struct drive
{
	struct 		backup_registers *bkp_reg;
	uint32_t 	current_position;
	uint32_t 	dest_position;//��������� �������
	uint32_t	min_speed_position;

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
uint8_t Drive_Set_Position(uint8_t move_type,int16_t move_val);
uint8_t Drive_Set_Speed(uint8_t val_speed);
uint8_t Drive_Start(uint8_t direction);
uint8_t Drive_Stop(uint8_t stop_type);

uint32_t Drive_MM_To_Impulse(uint16_t val_mm);
uint16_t Drive_Impulse_To_MM(uint32_t val_impulse);


#endif
