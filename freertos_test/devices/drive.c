#include "drive.h"
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_pwr.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "backup_sram.h"
#include "buzzer.h"
#include "menu.h"

struct drive drv;
extern xQueueHandle xClrIndicatorQueue;//������� ����������

void Drive_Init(void)
{
	RCC_AHB1PeriphClockCmd(/*DRIVE_ERROR_PORT_RCC|*/DRIVE_CONTROL_PORT_RCC, ENABLE);

	GPIO_InitTypeDef init_pin;
	init_pin.GPIO_Pin  = DRIVE_FORWARD | DRIVE_BACKWARD | DRIVE_RESET | DRIVE_SPEED;
	init_pin.GPIO_Speed = GPIO_Speed_2MHz;
	init_pin.GPIO_Mode  = GPIO_Mode_OUT;
	init_pin.GPIO_OType = GPIO_OType_PP;
	init_pin.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init (DRIVE_CONTROL_PORT, &init_pin);

	DRIVE_CONTROL_PORT->BSRRH=(DRIVE_FORWARD | DRIVE_BACKWARD | DRIVE_RESET | DRIVE_SPEED);

	drv.bkp_reg=(struct dev_registers *) BKPSRAM_BASE;


	if(drv.bkp_reg->backup_magic != BACKUP_MAGIC)//��������� �������? ������� ��� �� �� ���������
	{
		struct backup_registers default_reg;

		default_reg.F_01_cal_down.imp = 1930660355;
		default_reg.F_01_cal_down.mm = 470;

		default_reg.F_02_cal_up.imp = 1930700172;
		default_reg.F_02_cal_up.mm = 4070;

		default_reg.F_03_cal_syncro.imp = 1930699398;
		default_reg.F_03_cal_syncro.mm = 4070;


		default_reg.F_04_function_back = 4000;
		default_reg.F_05_cal_speed_down = 120;
		default_reg.F_06_cal_stop_down = 1;
		default_reg.F_07_cal_stop_up = 1;
		default_reg.F_08_const_tickness = -280;
		default_reg.F_09_const_tickness = -450;
		default_reg.F_10_const_tickness = -530;

		default_reg.backup_current_position = 1930658818;

		default_reg.backup_magic = BACKUP_MAGIC;

		Backup_SRAM_Write_Reg(drv.bkp_reg, &default_reg, sizeof(struct backup_registers));
	}

	drv.move_type_flag=MOVE_TYPE_NONE;
	drv.stop_type=STOP_NONE;
	drv.error_flag=DRIVE_OK;
	drv.current_position=drv.bkp_reg->backup_current_position;
	drv.function_back_flag=DRIVE_BACK_POS_DOWN;
	drv.function_back_temp_position=0x0;
	drv.limitation_flag=DRIVE_LIMITATION_NONE;
	//��������� ������?
}

uint8_t Drive_Set_Position_Imp_Absolute(uint32_t move_val_imp)
{
	Drive_Set_Speed(DRIVE_SPEED_LOW);

	if((drv.move_type_flag==MOVE_TYPE_NONE)&&(drv.error_flag==DRIVE_OK))
	{
		int32_t temp=move_val_imp-drv.current_position;

		if(temp>0)
		{
			if(Drive_Impulse_To_MM(temp)<=drv.bkp_reg->F_07_cal_stop_up)
			{
				return DRIVE_ERR;
			}
		}
		else
		{
			if(Drive_Impulse_To_MM(temp)<=drv.bkp_reg->F_06_cal_stop_down)
			{
				return DRIVE_ERR;
			}
		}

		if(temp<0)
		{
			temp=-temp;
		}

//		if(Drive_Impulse_To_MM(temp)<=drv.bkp_reg->F_06_cal_stop)
//		{
//			return DRIVE_ERR;
//		}

		if(Drive_Impulse_To_MM(temp)>drv.bkp_reg->F_05_cal_speed_down)
		{
			Drive_Set_Speed(DRIVE_SPEED_HI);
		}
		else
		{
			Drive_Set_Speed(DRIVE_SPEED_LOW);
		}

		if(move_val_imp<drv.current_position)
		{
			if(drv.limitation_flag==DRIVE_LIMITATION_ONLY_DOWN)
			{
				return DRIVE_ERR;
			}

			drv.dest_position=move_val_imp;
			drv.stop_position=move_val_imp+Drive_MM_To_Impulse(drv.bkp_reg->F_06_cal_stop_down);
			drv.min_speed_position=move_val_imp+Drive_MM_To_Impulse(drv.bkp_reg->F_05_cal_speed_down);
			Drive_Start(DRIVE_DIRECTION_DOWN);
		}
		else
		{
			if(drv.limitation_flag==DRIVE_LIMITATION_ONLY_UP)
			{
				return DRIVE_ERR;
			}

			drv.dest_position=move_val_imp;
			drv.stop_position=move_val_imp-Drive_MM_To_Impulse(drv.bkp_reg->F_07_cal_stop_up);
			drv.min_speed_position=move_val_imp-Drive_MM_To_Impulse(drv.bkp_reg->F_05_cal_speed_down);
			Drive_Start(DRIVE_DIRECTION_UP);
		}

		drv.stop_type=STOP_NONE;
		drv.move_type_flag=MOVE_TYPE_ABSOLUTE;
		return DRIVE_OK;
	}
	else
	{
		return DRIVE_BUSY;
	}
}

uint8_t Drive_Set_Position(uint8_t move_type,int16_t move_val)
{
	Drive_Set_Speed(DRIVE_SPEED_LOW);

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
				if(drv.limitation_flag==DRIVE_LIMITATION_ONLY_DOWN)
				{
					return DRIVE_ERR;
				}

				uint16_t tmp_val=move_val+Drive_Impulse_To_MM_Absolute(drv.current_position);

				drv.dest_position=Drive_MM_To_Impulse_Absolute(move_val+Drive_Impulse_To_MM_Absolute(drv.current_position));
				drv.stop_position=drv.dest_position-Drive_MM_To_Impulse(drv.bkp_reg->F_07_cal_stop_up);
				drv.min_speed_position=drv.dest_position-Drive_MM_To_Impulse(drv.bkp_reg->F_05_cal_speed_down);
				drv.stop_type=STOP_NONE;
				drv.move_type_flag=MOVE_TYPE_RELATIVE_UP;

				if(move_val>drv.bkp_reg->F_05_cal_speed_down)
				{
					Drive_Set_Speed(DRIVE_SPEED_HI);
				}
				else
				{
					Drive_Set_Speed(DRIVE_SPEED_LOW);
				}

				Drive_Start(DRIVE_DIRECTION_UP);
				return DRIVE_OK;
			}
			break;

			case MOVE_TYPE_RELATIVE_DOWN:
			{
				if(drv.limitation_flag==DRIVE_LIMITATION_ONLY_UP)
				{
					return DRIVE_ERR;
				}

				if(move_val<0)
				{
					move_val=-move_val;
				}

				drv.dest_position=Drive_MM_To_Impulse_Absolute(Drive_Impulse_To_MM_Absolute(drv.current_position)-move_val);
				drv.stop_position=drv.dest_position+Drive_MM_To_Impulse(drv.bkp_reg->F_06_cal_stop_down);
				drv.min_speed_position=drv.dest_position+Drive_MM_To_Impulse(drv.bkp_reg->F_05_cal_speed_down);
				drv.stop_type=STOP_NONE;
				drv.move_type_flag=MOVE_TYPE_RELATIVE_DOWN;

				if(move_val>drv.bkp_reg->F_05_cal_speed_down)
				{
					Drive_Set_Speed(DRIVE_SPEED_HI);
				}
				else
				{
					Drive_Set_Speed(DRIVE_SPEED_LOW);
				}

				Drive_Start(DRIVE_DIRECTION_DOWN);
				return DRIVE_OK;
			}
			break;

			case MOVE_TYPE_ABSOLUTE:
			{
				int16_t temp=move_val-Drive_Impulse_To_MM_Absolute(drv.current_position);

				if(temp>=0)
				{
					if(drv.limitation_flag==DRIVE_LIMITATION_ONLY_DOWN)
					{
						return DRIVE_ERR;
					}

					drv.dest_position=Drive_MM_To_Impulse_Absolute(move_val);
					drv.stop_position=drv.dest_position-Drive_MM_To_Impulse(drv.bkp_reg->F_07_cal_stop_up);
					drv.min_speed_position=drv.dest_position-Drive_MM_To_Impulse(drv.bkp_reg->F_05_cal_speed_down);

					if(temp>drv.bkp_reg->F_05_cal_speed_down)
					{
						Drive_Set_Speed(DRIVE_SPEED_HI);
					}
					else
					{
						Drive_Set_Speed(DRIVE_SPEED_LOW);
					}

					Drive_Start(DRIVE_DIRECTION_UP);
				}
				else
				{
					if(drv.limitation_flag==DRIVE_LIMITATION_ONLY_UP)
					{
						return DRIVE_ERR;
					}

					drv.dest_position=Drive_MM_To_Impulse_Absolute(move_val);
					drv.stop_position=drv.dest_position+Drive_MM_To_Impulse(drv.bkp_reg->F_06_cal_stop_down);
					drv.min_speed_position=drv.dest_position+Drive_MM_To_Impulse(drv.bkp_reg->F_05_cal_speed_down);

					if((-temp)>drv.bkp_reg->F_05_cal_speed_down)
					{
						Drive_Set_Speed(DRIVE_SPEED_HI);
					}
					else
					{
						Drive_Set_Speed(DRIVE_SPEED_LOW);
					}

					Drive_Start(DRIVE_DIRECTION_DOWN);
				}
				drv.stop_type=STOP_NONE;
				drv.move_type_flag=MOVE_TYPE_ABSOLUTE;
				return DRIVE_OK;
			}
			break;

			default:
			{
				return DRIVE_ERR;
			}
		}
	}
	else//������ �����
	{
		return DRIVE_BUSY;
	}
	return DRIVE_ERR;
}

uint8_t Drive_Set_Speed(uint8_t val_speed)
{
	switch(val_speed)
	{
		case DRIVE_SPEED_LOW:
		{
			DRIVE_CONTROL_PORT->BSRRL=DRIVE_SPEED;
			return DRIVE_OK;
		}
		break;

		case DRIVE_SPEED_HI:
		{
			DRIVE_CONTROL_PORT->BSRRH=DRIVE_SPEED;
			return DRIVE_OK;
		}
		break;

		default:
		{
			return DRIVE_ERR;
		}
	}
	return DRIVE_ERR;
}

uint8_t Drive_Start(uint8_t direction)
{
	switch(direction)
	{
		case DRIVE_DIRECTION_UP:
		{
			DRIVE_CONTROL_PORT->BSRRH=DRIVE_BACKWARD;
			DRIVE_CONTROL_PORT->BSRRL=DRIVE_FORWARD;
		}
		break;

		case DRIVE_DIRECTION_DOWN:
		{
			DRIVE_CONTROL_PORT->BSRRL=DRIVE_BACKWARD;
			DRIVE_CONTROL_PORT->BSRRH=DRIVE_FORWARD;
		}
		break;

		default:
		{

		}
		break;
	}
}

uint8_t Drive_Stop(uint8_t stop_type,uint8_t function_start_type)
{
	uint8_t clr_indicator_msg;

	switch(stop_type)
	{
		case STOP_END_OF_OPERATION:
		{
			DRIVE_CONTROL_PORT->BSRRH=(DRIVE_FORWARD | DRIVE_BACKWARD | DRIVE_SPEED);
			buzzer_set_buzz(BUZZER_EFFECT_3_BEEP,BUZZER_ON,function_start_type);
			drv.error_flag=DRIVE_OK;

			if(drv.function_back_flag==DRIVE_BACK_GET_UP)
			{
				drv.function_back_flag=DRIVE_BACK_POS_UP;
			}
			else if(drv.function_back_flag==DRIVE_BACK_GET_DOWN)
			{
				drv.function_back_flag=DRIVE_BACK_POS_DOWN;
			}
		}
		break;

		case STOP_HI_SENSOR:
		{
			DRIVE_CONTROL_PORT->BSRRH=(DRIVE_FORWARD | DRIVE_BACKWARD | DRIVE_SPEED);
			buzzer_set_buzz(BUZZER_EFFECT_LONG_BEEP,BUZZER_ON,function_start_type);
			drv.error_flag=DRIVE_ERR;
			drv.function_back_flag=DRIVE_BACK_POS_DOWN;
		}
		break;

		case STOP_LO_SENSOR:
		{
			DRIVE_CONTROL_PORT->BSRRH=(DRIVE_FORWARD | DRIVE_BACKWARD | DRIVE_SPEED);
			buzzer_set_buzz(BUZZER_EFFECT_LONG_BEEP,BUZZER_ON,function_start_type);
			drv.error_flag=DRIVE_ERR;
			drv.function_back_flag=DRIVE_BACK_POS_DOWN;
		}
		break;

		case STOP_INVERTOR_ERROR:
		{

			DRIVE_CONTROL_PORT->BSRRH=(DRIVE_FORWARD | DRIVE_BACKWARD | DRIVE_SPEED);
			buzzer_set_buzz(BUZZER_EFFECT_LONG_BEEP,BUZZER_ON,function_start_type);
			drv.error_flag=DRIVE_ERR;
			drv.function_back_flag=DRIVE_BACK_POS_DOWN;
		}
		break;

		case STOP_CONTROLLER_FAULT:
		{
			DRIVE_CONTROL_PORT->BSRRH=(DRIVE_FORWARD | DRIVE_BACKWARD | DRIVE_SPEED);
			buzzer_set_buzz(BUZZER_EFFECT_LONG_BEEP,BUZZER_ON,function_start_type);
			drv.error_flag=DRIVE_ERR;
			drv.function_back_flag=DRIVE_BACK_POS_DOWN;
		}
		break;

		case STOP_USER:
		{
			DRIVE_CONTROL_PORT->BSRRH=(DRIVE_FORWARD | DRIVE_BACKWARD | DRIVE_SPEED);
			buzzer_set_buzz(BUZZER_EFFECT_LONG_BEEP,BUZZER_ON,function_start_type);
			drv.error_flag=DRIVE_ERR;
			drv.function_back_flag=DRIVE_BACK_POS_DOWN;
		}
		break;

		case STOP_MANUAL_CONTROL:
		{
			DRIVE_CONTROL_PORT->BSRRH=(DRIVE_FORWARD | DRIVE_BACKWARD | DRIVE_SPEED);
			drv.error_flag=DRIVE_OK;
			drv.function_back_flag=DRIVE_BACK_POS_DOWN;
		}
		break;

		default:
		{
			DRIVE_CONTROL_PORT->BSRRH=(DRIVE_FORWARD | DRIVE_BACKWARD | DRIVE_SPEED);
			drv.error_flag=DRIVE_ERR;
			drv.function_back_flag=DRIVE_BACK_POS_DOWN;
		}
		break;
	}

	clr_indicator_msg=MENU_MSG_CLR_INDICATOR;
	if(function_start_type==FROM_TASK)
	{
		xQueueSend( xClrIndicatorQueue,  &clr_indicator_msg, ( portTickType ) 0 );
	}
	else
	{
		portBASE_TYPE *xHigherPriorityTaskWoken ;
		    /* We have not woken a task at the start of the ISR. */
		xHigherPriorityTaskWoken = pdFALSE;
		xQueueSendFromISR( xClrIndicatorQueue,  &clr_indicator_msg,/* ( portTickType ) 0*/&xHigherPriorityTaskWoken );
	}
	drv.stop_type=stop_type;
	drv.move_type_flag=MOVE_TYPE_NONE;
}

void Drive_Reset(void)
{
	uint32_t delay=1000000;
	DRIVE_CONTROL_PORT->BSRRL= DRIVE_RESET ;
	while(delay--);
	DRIVE_CONTROL_PORT->BSRRH= DRIVE_RESET ;
}

uint32_t Drive_MM_To_Impulse(uint16_t val_mm)
{
	return (uint32_t)((val_mm*((uint64_t)drv.bkp_reg->F_02_cal_up.imp-(uint64_t)drv.bkp_reg->F_01_cal_down.imp))/(drv.bkp_reg->F_02_cal_up.mm-drv.bkp_reg->F_01_cal_down.mm));
}

uint16_t Drive_Impulse_To_MM(uint32_t val_impulse)
{
	return (uint16_t)((val_impulse*((uint64_t)drv.bkp_reg->F_02_cal_up.mm-drv.bkp_reg->F_01_cal_down.mm))/(drv.bkp_reg->F_02_cal_up.imp-drv.bkp_reg->F_01_cal_down.imp));
}

uint16_t Drive_Impulse_To_MM_Absolute(uint32_t val_impulse)
{
	if(val_impulse>=drv.bkp_reg->F_03_cal_syncro.imp)
	{
		return drv.bkp_reg->F_03_cal_syncro.mm+Drive_Impulse_To_MM(val_impulse-drv.bkp_reg->F_03_cal_syncro.imp+1);
	}
	else
	{
		return drv.bkp_reg->F_03_cal_syncro.mm-Drive_Impulse_To_MM(drv.bkp_reg->F_03_cal_syncro.imp-val_impulse)-1;
	}
}

uint32_t Drive_MM_To_Impulse_Absolute(uint16_t val_mm)
{
	int16_t temp=(int16_t)val_mm-drv.bkp_reg->F_03_cal_syncro.mm;

	if(temp>=0)
	{
		return drv.bkp_reg->F_03_cal_syncro.imp+Drive_MM_To_Impulse((uint16_t)temp);
	}
	else
	{
		return drv.bkp_reg->F_03_cal_syncro.imp-Drive_MM_To_Impulse((uint16_t)(-temp));
	}
}
