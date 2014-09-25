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
	struct backup_registers *bkp_reg;
	uint8_t move_type_flag;
	uint8_t error_flag;
};

void Drive_Init(void);


#endif
