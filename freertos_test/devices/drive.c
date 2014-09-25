#include "drive.h"
#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_pwr.h"

struct drive drv;

void Drive_Init(void)
{
	drv.bkp_reg=(struct dev_registers *) BKPSRAM_BASE;
	drv.move_type_flag=MOVE_TYPE_NONE;
	drv.error_flag=DRIVE_OK;
}
