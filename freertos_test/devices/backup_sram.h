#ifndef BACKUP_SRAM_H
#define BACKUP_SRAM_H
#include "stm32f4xx.h"
void Backup_SRAM_Init(void);
int8_t Backup_SRAM_Write( int16_t *data, uint16_t bytes, uint16_t offset );
int8_t Backup_SRAM_Read ( int16_t *data, uint16_t bytes, uint16_t offset );

//void Backup_SRAM_Write_Reg(int16_t *reg, int16_t reg_val);
void Backup_SRAM_Write_Reg(void *backup_reg, void *source_reg,uint8_t reg_size);
#endif
