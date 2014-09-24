#ifndef BACKUP_SRAM_H
#define BACKUP_SRAM_H
#include "stm32f4xx.h"
void Backup_SRAM_Init(void);
int8_t Backup_SRAM_Write( int16_t *data, uint16_t bytes, uint16_t offset );
int8_t Backup_SRAM_Read ( int16_t *data, uint16_t bytes, uint16_t offset );

void Backup_SRAM_Write_Reg(int16_t *reg, int16_t reg_val);

struct dev_registers
{
	uint16_t F_01_cal_up;
	uint16_t F_02_cal_down;
	uint16_t F_03_cal_syncro;
	uint16_t F_04_current_position;
	uint16_t F_05_cal_speed_down;
	uint16_t F_06_cal_stop;
};

#endif
