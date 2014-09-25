#include "backup_sram.h"
#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_pwr.h"



void Backup_SRAM_Init(void)
{
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_BKPSRAM, ENABLE);
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
}

void Backup_SRAM_Write_Reg(int16_t *reg, int16_t reg_val)
{
	  PWR_BackupAccessCmd(ENABLE);        // set PWR->CR.dbp = 1;
	  PWR_BackupRegulatorCmd(ENABLE);     // set PWR->CSR.bre = 1;

	  *reg=reg_val;
	  PWR_BackupAccessCmd(DISABLE);                     // reset PWR->CR.dbp = 0;
}

//uint16_t Backup_SRAM_Read_Reg(uint8_ reg_num)
//{
//
//}


int8_t Backup_SRAM_Write( int16_t *data, uint16_t bytes, uint16_t offset )
{
  const uint16_t backup_size = 0x1000;
  int16_t* base_addr = (int16_t *) BKPSRAM_BASE;
  uint16_t i;
  if( bytes + offset >= backup_size ) {
    /* ERROR : the last byte is outside the backup SRAM region */
    return -1;
  }

  PWR_BackupAccessCmd(ENABLE);        // set PWR->CR.dbp = 1;

  PWR_BackupRegulatorCmd(ENABLE);     // set PWR->CSR.bre = 1;
  for( i = 0; i < bytes; i++ ) {
    *(base_addr + offset + i) = *(data + i);
  }
  PWR_BackupAccessCmd(DISABLE);                     // reset PWR->CR.dbp = 0;
  return 0;
}

int8_t Backup_SRAM_Read( int16_t *data, uint16_t bytes, uint16_t offset )
{
  const uint16_t backup_size = 0x1000;
  int16_t* base_addr = (int16_t *) BKPSRAM_BASE;
  uint16_t i;
  if( bytes + offset >= backup_size ) {
    /* ERROR : the last byte is outside the backup SRAM region */
    return -1;
  }
 // RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_BKPSRAM, ENABLE);
  for( i = 0; i < bytes; i++ ) {
    *(data + i) = *(base_addr + offset + i);
  }
  return 0;
}
