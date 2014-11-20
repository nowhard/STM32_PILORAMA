#include "buzzer.h"

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include <misc.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "watchdog.h"


struct buzzer buz;
extern struct task_watch task_watches[];
xTaskHandle xBuzzer_Handle;
xSemaphoreHandle xBuzzerSemaphore;

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

    BUZZER_PORT->BSRRH=BUZZER_PIN;

    vSemaphoreCreateBinary( xBuzzerSemaphore );
    xTaskCreate(buzzer_task,(signed char*)"BUZZER",128,NULL, tskIDLE_PRIORITY + 1, &xBuzzer_Handle);
   // vTaskSuspend (xBuzzer_Handle);


    task_watches[BUZZER_TASK].task_status=TASK_IDLE;
}

void buzzer_task(void *pvParameters )
{
	while(1)
	{
		xSemaphoreTake( xBuzzerSemaphore, portMAX_DELAY );

		if( buz.buzzer_enable==BUZZER_ON)
		{
			switch( buz.buzzer_effect)
			{
				case BUZZER_EFFECT_1_BEEP:
				{
					BUZZER_PORT->BSRRL=BUZZER_PIN;
					vTaskDelay(200);
					BUZZER_PORT->BSRRH=BUZZER_PIN;
					buz.buzzer_enable=BUZZER_OFF;
				}
				break;

				case BUZZER_EFFECT_2_BEEP:
				{
					BUZZER_PORT->BSRRL=BUZZER_PIN;
					vTaskDelay(100);
					BUZZER_PORT->BSRRH=BUZZER_PIN;
					vTaskDelay(100);
					BUZZER_PORT->BSRRL=BUZZER_PIN;
					vTaskDelay(100);
					BUZZER_PORT->BSRRH=BUZZER_PIN;
					buz.buzzer_enable=BUZZER_OFF;
				}
				break;

				case BUZZER_EFFECT_3_BEEP:
				{
					BUZZER_PORT->BSRRL=BUZZER_PIN;
					vTaskDelay(50);
					BUZZER_PORT->BSRRH=BUZZER_PIN;
					vTaskDelay(50);
					BUZZER_PORT->BSRRL=BUZZER_PIN;
					vTaskDelay(50);
					BUZZER_PORT->BSRRH=BUZZER_PIN;
					vTaskDelay(50);
					BUZZER_PORT->BSRRL=BUZZER_PIN;
					vTaskDelay(50);
					BUZZER_PORT->BSRRH=BUZZER_PIN;
					buz.buzzer_enable=BUZZER_OFF;
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
					buz.buzzer_enable=BUZZER_OFF;
				}
				break;

				default:
				{
					vTaskDelay(500);
				}
			}
		}
		task_watches[BUZZER_TASK].counter++;
		//vTaskSuspend (xBuzzer_Handle);
		task_watches[BUZZER_TASK].task_status=TASK_IDLE;
	}
}



void buzzer_set_buzz(uint8_t effect, uint8_t enable,uint8_t function_start_type)
{
	if(enable&0x1)
	{
		if( buz.buzzer_enable==BUZZER_OFF)
		{
			if(function_start_type==FROM_TASK)
			{
				//vTaskResume(xBuzzer_Handle);
				xSemaphoreGive(xBuzzerSemaphore);
			}
			else
			{
				//xTaskResumeFromISR(xBuzzer_Handle);
				  static portBASE_TYPE xHigherPriorityTaskWoken;
				  xHigherPriorityTaskWoken = pdFALSE;
				  xSemaphoreGiveFromISR( xBuzzerSemaphore, &xHigherPriorityTaskWoken );
				  if( xHigherPriorityTaskWoken == pdTRUE )
				  {
					  portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
					  //taskYIELD();
				  }
			}
			 buz.buzzer_enable=BUZZER_ON;
		}
		//task_watches[BUZZER_TASK].task_status=TASK_ACTIVE;
	}
	else
	{
		if( buz.buzzer_enable==BUZZER_ON)
		{
			// vTaskSuspend (xBuzzer_Handle);
			 buz.buzzer_enable=BUZZER_OFF;
		}
		task_watches[BUZZER_TASK].task_status=TASK_IDLE;
		BUZZER_PORT->BSRRH=BUZZER_PIN;
	}
	 buz.buzzer_effect=effect&0x7;
}
