#include "buzzer.h"

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include <misc.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "tablo.h"
#include "watchdog.h"

extern struct tablo tab;
extern struct task_watch task_watches[];
xTaskHandle xBuzzer_Handle;

void buzzer_init(void)
{
    RCC_AHB1PeriphClockCmd(BUZZER_BUS, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;

    /* Configure port -------------------------------*/
    GPIO_InitStructure.GPIO_Pin   = BUZZER_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(BUZZER_PORT, &GPIO_InitStructure);


    //GPIO_WriteBit(BUZZER_PORT, BUZZER_PIN,0);
    BUZZER_PORT->BSRRH=BUZZER_PIN;

    xTaskCreate(buzzer_task,(signed char*)"BUZZER",64,NULL, tskIDLE_PRIORITY + 1, &xBuzzer_Handle);
    vTaskSuspend (xBuzzer_Handle);

    task_watches[BUZZER_TASK].task_status=TASK_IDLE;
}
void buzzer_task(void *pvParameters )
{
	while(1)
	{
		if(tab.buz.buzzer_enable==BUZZER_ON)
		{
			switch(tab.buz.buzzer_effect)
			{
				case BUZZER_EFFECT_1_BEEP:
				{
					BUZZER_PORT->BSRRL=BUZZER_PIN;
					vTaskDelay(200);
					BUZZER_PORT->BSRRH=BUZZER_PIN;
					//vTaskDelay(200);
					tab.buz.buzzer_enable=BUZZER_OFF;
				}
				break;

				case BUZZER_EFFECT_2_BEEP:
				{
					BUZZER_PORT->BSRRL=BUZZER_PIN;
					vTaskDelay(200);
					BUZZER_PORT->BSRRH=BUZZER_PIN;
					vTaskDelay(200);
					BUZZER_PORT->BSRRL=BUZZER_PIN;
					vTaskDelay(200);
					BUZZER_PORT->BSRRH=BUZZER_PIN;
					//vTaskDelay(200);
					tab.buz.buzzer_enable=BUZZER_OFF;
				}
				break;

				case BUZZER_EFFECT_3_BEEP:
				{
					BUZZER_PORT->BSRRL=BUZZER_PIN;
					vTaskDelay(200);
					BUZZER_PORT->BSRRH=BUZZER_PIN;
					vTaskDelay(200);
					BUZZER_PORT->BSRRL=BUZZER_PIN;
					vTaskDelay(200);
					BUZZER_PORT->BSRRH=BUZZER_PIN;
					vTaskDelay(200);
					BUZZER_PORT->BSRRL=BUZZER_PIN;
					vTaskDelay(200);
					BUZZER_PORT->BSRRH=BUZZER_PIN;
					//vTaskDelay(200);
					tab.buz.buzzer_enable=BUZZER_OFF;
				}
				break;

				case BUZZER_EFFECT_INFINITE_BEEP:
				{
					BUZZER_PORT->BSRRL=BUZZER_PIN;
				}
				break;

				case BUZZER_EFFECT_LONG_BEEP:
				{
					BUZZER_PORT->BSRRL=BUZZER_PIN;
					vTaskDelay(1000);
					BUZZER_PORT->BSRRH=BUZZER_PIN;
					//vTaskDelay(200);
					tab.buz.buzzer_enable=BUZZER_OFF;
				}
				break;

				default:
				{
					vTaskDelay(500);
				}
			}
		}
		task_watches[BUZZER_TASK].counter++;
		vTaskSuspend (xBuzzer_Handle);
		task_watches[BUZZER_TASK].task_status=TASK_IDLE;
	}
}


void buzzer_set_buzz(uint8_t effect, uint8_t enable)
{
	if(enable&0x1)
	{
		if(tab.buz.buzzer_enable==BUZZER_OFF)
		{
			vTaskResume(xBuzzer_Handle);
			tab.buz.buzzer_enable=BUZZER_ON;
		}
		task_watches[BUZZER_TASK].task_status=TASK_ACTIVE;
	}
	else
	{
		if(tab.buz.buzzer_enable==BUZZER_ON)
		{
			vTaskSuspend (xBuzzer_Handle);
			tab.buz.buzzer_enable=BUZZER_OFF;
		}
		task_watches[BUZZER_TASK].task_status=TASK_IDLE;
		//GPIO_WriteBit(BUZZER_PORT, BUZZER_PIN,0);
		BUZZER_PORT->BSRRH=BUZZER_PIN;
	}
	tab.buz.buzzer_effect=effect&0x7;
}
