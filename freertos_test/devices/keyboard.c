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

#include "buzzer.h"

xQueueHandle xKeyQueue;//очередь клавиатуры

static void vKeyboardTask(void *pvParameters);

static void delay(uint16_t time)
{
	while(time>0)
	{
		time--;
	}
}

void Keyboard_Init(void)
{
	RCC_AHB1PeriphClockCmd(INIT_KEYB_PORT_IN|INIT_KEYB_PORT_OUT, ENABLE);

	GPIO_InitTypeDef init_pin;
	init_pin.GPIO_Pin  = KO_0 | KO_1 | KO_2 | KO_3;
	init_pin.GPIO_Speed = GPIO_Speed_2MHz;
	init_pin.GPIO_Mode  = GPIO_Mode_OUT;
	init_pin.GPIO_OType = GPIO_OType_OD;
	init_pin.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init (KEYB_PORT_OUT, &init_pin);

	init_pin.GPIO_Pin  = KI_0 | KI_1 | KI_2 | KI_3;//подт€гиваем вверх, дл€ уменьшени€ помех
	init_pin.GPIO_Speed = GPIO_Speed_2MHz;
	init_pin.GPIO_Mode  = GPIO_Mode_IN;
	init_pin.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init (KEYB_PORT_IN, &init_pin);

	KEYB_PORT_OUT->BSRRL=(KO_0 | KO_1 | KO_2 | KO_3);

//	vSemaphoreCreateBinary( xKeySemaphore );
	xKeyQueue = xQueueCreate( 2, sizeof( uint8_t ) );
	xTaskCreate(vKeyboardTask,(signed char*)"Keyboard",64,NULL, tskIDLE_PRIORITY + 1, NULL);
}

enum
{
	ROW_0=0,
	ROW_1,
	ROW_2,
	ROW_3,
};


static uint8_t Keyboard_Scan_Matrix(void);

static void vKeyboardTask(void *pvParameters)
{
	uint8_t key,key2,key_temp, last_key;

	uint8_t key_counter=0;
	uint8_t read_cycle_counter=0;
	uint8_t tick_counter=0;
	uint16_t key_mask=0;

	key_mask=KI_0 | KI_1 | KI_2 | KI_3;
    while(1)
    {

    	key=Keyboard_Scan_Matrix();
		vTaskDelay(10);
		key2=Keyboard_Scan_Matrix();


    	if(key!=0xFF)//что то нажато
    	{
			if(key==key2)//нет дребезга
			{
				key_temp=key;
				if(tick_counter<LONG_PRESS_KEY)
				{
					tick_counter++;
				}
				else
				{
					if(tick_counter==LONG_PRESS_KEY)//долгое нажатие
					{
						 tick_counter=LONG_PRESS_KEY+1;
		    			//отправим в очередь код
						 key+=16;//дополнение кода при нажатии LONG
						// buzzer_set_buzz(BUZZER_EFFECT_LONG_BEEP,BUZZER_ON);//beep
						 if( xKeyQueue != 0 )
						 {
							 xQueueSend( xKeyQueue,  &key, ( portTickType ) 0 );
						 }
						// xSemaphoreGive(xKeySemaphore);
					}
					else
					{

					}
				}
			}
    	}
    	else//отпускание
    	{
    		if((tick_counter>0)&&(tick_counter<LONG_PRESS_KEY))
    		{
    			//отправим в очередь код
				 if( xKeyQueue != 0 )
				 {
					 xQueueSend( xKeyQueue,  &key_temp, ( portTickType ) 0 );
				 }
    		}
    		else
    		{
    			if(tick_counter==(LONG_PRESS_KEY+1))//отпускаем после длительного нажати€
    			{
    				key_temp+=32;//дополнение кода при нажатии LONG_RELEASE
					 if( xKeyQueue != 0 )
					 {
						 xQueueSend( xKeyQueue,  &key_temp, ( portTickType ) 0 );
					 }
    			}
    		}
    		last_key=0xFF;
    		tick_counter=0;
    	}
	vTaskDelay(50);
    }
}

static uint8_t Keyboard_Scan_Matrix(void)
{
	uint16_t   key_port;
	uint8_t key_counter=0;
	uint16_t key_mask=0;

	key_mask=KI_0 | KI_1 | KI_2 | KI_3;
	KEYB_PORT_OUT->BSRRL=(KO_0 | KO_1 | KO_2 | KO_3);

	for(key_counter=0;key_counter<KEYB_ROW_NUM;key_counter++)
	{
			//KEYB_PORT_OUT->BSRRL=(KO_0 | KO_1 | KO_2 | KO_3);
			switch(key_counter)
			{
				case ROW_0:
				{
					KEYB_PORT_OUT->BSRRH=KO_0;
					delay(10);
					key_port=KEYB_PORT_IN->IDR;//GPIO_ReadInputData(KEYB_PORT_IN);
					KEYB_PORT_OUT->BSRRL=(KO_0 | KO_1 | KO_2 | KO_3);

					switch((~key_port)&key_mask)
					{
						case KI_0:
						{
							return KEY_1;
						}
						break;

						case KI_1:
						{
							return KEY_2;
						}
						break;

						case KI_2:
						{
							return KEY_3;
						}
						break;

						case KI_3:
						{
							return KEY_A;
						}
						break;
					}
				}
				break;

				case ROW_1:
				{
					KEYB_PORT_OUT->BSRRH=KO_1;
					delay(10);
					key_port=KEYB_PORT_IN->IDR;//GPIO_ReadInputData(KEYB_PORT_IN);
					KEYB_PORT_OUT->BSRRL=(KO_0 | KO_1 | KO_2 | KO_3);

					switch((~key_port)&key_mask)
					{
						case KI_0:
						{
							return KEY_4;
						}
						break;

						case KI_1:
						{
							return KEY_5;
						}
						break;

						case KI_2:
						{
							return KEY_6;
						}
						break;

						case KI_3:
						{
							return KEY_B;
						}
						break;
					}
				}
				break;

				case ROW_2:
				{
					KEYB_PORT_OUT->BSRRH=KO_2;
					delay(10);
					key_port=KEYB_PORT_IN->IDR;//GPIO_ReadInputData(KEYB_PORT_IN);
					KEYB_PORT_OUT->BSRRL=(KO_0 | KO_1 | KO_2 | KO_3);

					switch((~key_port)&key_mask)
					{
						case KI_0:
						{
							return KEY_7;
						}
						break;

						case KI_1:
						{
							return KEY_8;
						}
						break;

						case KI_2:
						{
							return KEY_9;
						}
						break;

						case KI_3:
						{
							return KEY_C;
						}
						break;
					}
				}
				break;

				case ROW_3:
				{
					KEYB_PORT_OUT->BSRRH=KO_3;
					delay(10);
					key_port=KEYB_PORT_IN->IDR;//GPIO_ReadInputData(KEYB_PORT_IN);
					KEYB_PORT_OUT->BSRRL=(KO_0 | KO_1 | KO_2 | KO_3);

					switch((~key_port)&key_mask)
					{
						case KI_0:
						{
							return KEY_STAR;
						}
						break;

						case KI_1:
						{
							return KEY_0;
						}
						break;

						case KI_2:
						{
							return KEY_SHARP;
						}
						break;

						case KI_3:
						{
							return KEY_POINT;
						}
						break;
					}
				}
				break;
			}
	}
	return 0xFF;
}
