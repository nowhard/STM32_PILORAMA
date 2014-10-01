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
xQueueHandle xDriveCommandQueue;

struct DriveCommand
{
	uint8_t move_type;
	uint32_t move_val;// impulse
	uint8_t timeout;//seconds

};

void DriveHandler(void *pvParameters);

void Drive_Init(void)
{
	RCC_AHB1PeriphClockCmd(DRIVE_ERROR_PORT_RCC|DRIVE_CONTROL_PORT_RCC, ENABLE);

	GPIO_InitTypeDef init_pin;
	init_pin.GPIO_Pin  = DRIVE_FORWARD | DRIVE_BACKWARD | DRIVE_RESET | DRIVE_SPEED;
	init_pin.GPIO_Speed = GPIO_Speed_2MHz;
	init_pin.GPIO_Mode  = GPIO_Mode_OUT;
	init_pin.GPIO_OType = GPIO_OType_OD;
	init_pin.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init (DRIVE_CONTROL_PORT, &init_pin);

	init_pin.GPIO_Pin  = DRIVE_ERROR;//подтягиваем вверх, для уменьшения помех
	init_pin.GPIO_Speed = GPIO_Speed_2MHz;
	init_pin.GPIO_Mode  = GPIO_Mode_IN;
	init_pin.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init (DRIVE_ERROR_PORT, &init_pin);

	DRIVE_CONTROL_PORT->BSRRH=(DRIVE_FORWARD | DRIVE_BACKWARD | DRIVE_RESET | DRIVE_SPEED);

	drv.bkp_reg=(struct dev_registers *) BKPSRAM_BASE;
	drv.move_type_flag=MOVE_TYPE_NONE;
	drv.stop_type=STOP_NONE;
	drv.error_flag=DRIVE_OK;
	drv.current_position=0x80008000;

//	if((drv.bkp_reg->F_03_cal_syncro<0x90000000)&&(drv.bkp_reg->F_03_cal_syncro>0x70000000))
//	{
//		drv.current_position=drv.bkp_reg->F_03_cal_syncro;
//	}

	xDriveCommandQueue = xQueueCreate( 2, sizeof( struct DriveCommand ) );
//	xTaskCreate(xDriveCommandQueue,(signed char*)"Drive",256,NULL, tskIDLE_PRIORITY + 1, NULL);
}

uint8_t Drive_Start(uint8_t move_type,int16_t move_val)
{
	struct DriveCommand drvcmd;

	if((drv.move_type_flag==MOVE_TYPE_NONE)&&(drv.error_flag==DRIVE_OK))
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
				drvcmd.move_type=move_type;
				drvcmd.move_val= drv.current_position+Drive_MM_To_Impulse(move_val);
				xQueueSend( xDriveCommandQueue,  &(drvcmd), ( portTickType ) 0 );
				return DRIVE_OK;
			}
			break;

			case MOVE_TYPE_RELATIVE_DOWN:
			{
				drvcmd.move_type=move_type;
				drvcmd.move_val= drv.current_position-Drive_MM_To_Impulse(move_val);
				xQueueSend( xDriveCommandQueue,  &(drvcmd), ( portTickType ) 0 );
				return DRIVE_OK;
			}
			break;

			case MOVE_TYPE_ABSOLUTE_UP:
			{
				drvcmd.move_type=move_type;
				drvcmd.move_val= Drive_MM_To_Impulse(move_val);
				xQueueSend( xDriveCommandQueue,  &(drvcmd), ( portTickType ) 0 );
				return DRIVE_OK;
			}
			break;

			case MOVE_TYPE_ABSOLUTE_DOWN:
			{
				drvcmd.move_type=move_type;
				drvcmd.move_val= Drive_MM_To_Impulse(move_val);
				xQueueSend( xDriveCommandQueue,  &(drvcmd), ( portTickType ) 0 );
				return DRIVE_OK;
			}
			break;

			default:
			{
				return DRIVE_ERROR;
			}
		}
	}
	else//привод занят
	{

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
	DRIVE_CONTROL_PORT->BSRRH=(DRIVE_FORWARD | DRIVE_BACKWARD | DRIVE_SPEED);
	drv.stop_type=stop_type;
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

void DriveHandler( void *pvParameters )
{
	struct DriveCommand drvcmd;
    while(1)
    {
		if( (xDriveCommandQueue != 0)&&(uxQueueMessagesWaiting(xDriveCommandQueue)) )
		 {
			 if( xQueueReceive( xDriveCommandQueue, &( drvcmd ), ( portTickType ) 10 ) )
			 {
				 drv.move_type_flag=drvcmd.move_type;


				 while((drv.error_flag==DRIVE_OK)&&(drv.stop_type==STOP_NONE))//цикл установки привода
				 {
					int32_t  remain=drv.current_position-drvcmd.move_val;
					uint32_t carriage_sped_down=Drive_MM_To_Impulse(drv.bkp_reg->F_05_cal_speed_down);
					uint32_t carriage_stop=Drive_MM_To_Impulse(drv.bkp_reg->F_06_cal_stop);

					if(remain<0)
					{
						remain=-remain;
					}


					if(remain>carriage_sped_down)
					{
						Drive_Set_Speed(DRIVE_SPEED_HI);
					}
					else
					{
						if(remain>carriage_stop)
						{
							Drive_Set_Speed(DRIVE_SPEED_LOW);
						}
						else
						{
							Drive_Stop(STOP_END_OF_OPERATION);
						}
					}
				 }

				 if(drv.error_flag==DRIVE_OK)
				 {
					 drv.move_type_flag=MOVE_TYPE_NONE;//привод занял положение
				 }
				 else//ошибка привода
				 {

				 }
			 }
		 }
    	vTaskDelay(10);
    }
}
