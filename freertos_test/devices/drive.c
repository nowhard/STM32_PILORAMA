#include "drive.h"
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_pwr.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

struct drive drv;

void Drive_Init(void)
{
	drv.bkp_reg=(struct dev_registers *) BKPSRAM_BASE;
	drv.move_type_flag=MOVE_TYPE_NONE;
	drv.error_flag=DRIVE_OK;
}

uint8_t Drive_Start(uint8_t move_type,int16_t move_val)
{
	switch(move_type)
	{
		case MOVE_TYPE_NONE:
		{
			return DRIVE_OK;
		}
		break;

		case MOVE_TYPE_RELATIVE_UP:
		{
			return DRIVE_OK;
		}
		break;

		case MOVE_TYPE_RELATIVE_DOWN:
		{
			return DRIVE_OK;
		}
		break;

		case MOVE_TYPE_ABSOLUTE_UP:
		{
			return DRIVE_OK;
		}
		break;

		case MOVE_TYPE_ABSOLUTE_DOWN:
		{
			return DRIVE_OK;
		}
		break;

		default:
		{
			return DRIVE_ERROR;
		}
	}
	return DRIVE_ERROR;
}

uint8_t Drive_Set_Speed(uint8_t val_speed)
{
	switch(val_speed)
	{
		case DRIVE_SPEED_LOW:
		{
			return DRIVE_OK;
		}
		break;

		case DRIVE_SPEED_HI:
		{
			return DRIVE_OK;
		}
		break;

		default:
		{
			return DRIVE_ERROR;
		}
	}
	return DRIVE_ERROR;
}

uint8_t Drive_Stop(uint8_t stop_type)
{

}


#define MAGIC_IMPULSE 	240000
#define MAGIC_MM		785


uint32_t Drive_MM_To_Impulse(uint16_t val_mm)
{
	return ((uint32_t)val_mm*MAGIC_IMPULSE/MAGIC_MM);
}

uint16_t Drive_Impulse_To_MM(uint32_t val_impulse)
{
	return (uint16_t)((uint64_t)val_impulse*MAGIC_MM/MAGIC_IMPULSE);
}
