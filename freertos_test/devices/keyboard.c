#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

#include <misc.h>

#include "keyboard.h"
//»нклуды от FreeRTOS:

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

xQueueHandle xKeyQueue;//очередь клавиатуры
xSemaphoreHandle xKeySemaphore;

static void vKeyboardTask(void *pvParameters);

void Keyboard_Init(void)
{
	RCC_AHB1PeriphClockCmd(INIT_KEYB_PORT_IN|INIT_KEYB_PORT_OUT, ENABLE);

	GPIO_InitTypeDef init_pin;
	init_pin.GPIO_Pin  = KO_0 | KO_1 | KO_2 | KO_3;
	init_pin.GPIO_Mode  = GPIO_Mode_OUT;
	init_pin.GPIO_OType = GPIO_OType_PP;
	init_pin.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init (KEYB_PORT_OUT, &init_pin);

	init_pin.GPIO_Pin  = KI_0 | KI_1 | KI_2 | KI_3;//подт€гиваем вверх, дл€ уменьшени€ помех
	init_pin.GPIO_Speed = GPIO_Speed_2MHz;
	init_pin.GPIO_Mode  = GPIO_Mode_IN;
	init_pin.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init (KEYB_PORT_IN, &init_pin);

	KEYB_PORT_OUT->BSRRL=(KO_0 | KO_1 | KO_2 | KO_3);

	vSemaphoreCreateBinary( xKeySemaphore );
	xKeyQueue = xQueueCreate( 2, sizeof( uint16_t ) );
	xTaskCreate(vKeyboardTask,(signed char*)"Keyboard",64,NULL, tskIDLE_PRIORITY + 1, NULL);
}

enum
{
	ROW_0=0,
	ROW_1,
	ROW_2,
	ROW_3,
};
#define LONG_PRESS_KEY	100

static void vKeyboardTask(void *pvParameters)
{
	uint16_t key, last_key, key_port,key_port2;

	uint8_t key_counter=0;
	uint8_t read_cycle_counter=0;
	uint8_t tick_counter=0;
	uint16_t key_mask=0;

	key_mask=KI_0 | KI_1 | KI_2 | KI_3;
    while(1)
    {

    	for(key_counter=0;key_counter<KEYB_ROW_NUM;key_counter++)
		{
			KEYB_PORT_IN->BSRRL=(KO_0 | KO_1 | KO_2 | KO_3);
			switch(key_counter)
			{
				case ROW_0:
				{
					KEYB_PORT_IN->BSRRH=KO_0;
				}
				break;

				case ROW_1:
				{
					KEYB_PORT_IN->BSRRH=KO_1;
				}
				break;

				case ROW_2:
				{
					KEYB_PORT_IN->BSRRH=KO_2;
				}
				break;

				case ROW_3:
				{
					KEYB_PORT_IN->BSRRH=KO_3;
				}
				break;

			}
			key_port=GPIO_ReadInputData(KEYB_PORT_IN)&key_mask;
			if(key_port!=key_mask)
			{
				break;
			}
		}
		vTaskDelay(10);

    	for(key_counter=0;key_counter<KEYB_ROW_NUM;key_counter++)
		{
			KEYB_PORT_IN->BSRRL=(KO_0 | KO_1 | KO_2 | KO_3);
			switch(key_counter)
			{
				case ROW_0:
				{
					KEYB_PORT_IN->BSRRH=KO_0;
				}
				break;

				case ROW_1:
				{
					KEYB_PORT_IN->BSRRH=KO_1;
				}
				break;

				case ROW_2:
				{
					KEYB_PORT_IN->BSRRH=KO_2;
				}
				break;

				case ROW_3:
				{
					KEYB_PORT_IN->BSRRH=KO_3;
				}
				break;

			}
			key_port2=GPIO_ReadInputData(KEYB_PORT_IN)&key_mask;
			if(key_port2!=key_mask)
			{
				break;
			}
		}

    	if(key_port!=key_mask)
    	{
			if(key_port==key_port2)//нет дребезга
			{
				if(tick_counter<LONG_PRESS_KEY)
				{
					tick_counter++;
				}
				else
				{
					if(tick_counter==LONG_PRESS_KEY)
					{
						tick_counter++;
						//отправим в очередь код
					}
					else
					{

					}
				}

			}
    	}
    	else
    	{
    		if((tick_counter>0)&&(tick_counter<LONG_PRESS_KEY))
    		{
    			//отправим в очередь код
    		}
    		else
    		{
    			if(tick_counter==(LONG_PRESS_KEY+1))
    			{
    				//ничего не делаем
    			}
    		}
    		last_key=0xFF;
    		tick_counter=0;
    	}



			vTaskDelay(10);
			if(key_port==key_mask)
			{
				last_key=0xFFFF;
			}
			else
			{

				if(key_port==(GPIO_ReadInputData(KEYB_PORT_IN)&key_mask))
				{
					if((key_port&KI_0)==0)
					{
						key=key_counter;
					}

		//			if((key&KI_1)==0)
		//			{
		//				key=(key_counter|(1<<2));
		//			}

					if((key_port&KI_2)==0)
					{
						key=(key_counter|(2<<2));
					}

					if(key!=last_key)
					{
						 last_key=key;

						 xSemaphoreGive(xKeySemaphore);
						 if( xKeyQueue != 0 )
						 {
							 xQueueSend( xKeyQueue,  &key, ( portTickType ) 0 );
						 }
					}
			 }
		 }

	vTaskDelay(10);
    }
}
