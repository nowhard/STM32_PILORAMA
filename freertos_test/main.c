#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"

#include <misc.h>
#include "system_stm32f4xx.h"

#include <stdio.h>
#include <math.h>

//FreeRTOS:

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"


//#include "tablo_parser.h"
#include "spi_bus.h"
//#include "tablo.h"

#include "keyboard.h"
//#include "buzzer.h"
//#include "power.h"
#include "watchdog.h"
//#include "relay.h"
//#include "power_detector.h"
#include "encoder.h"



//static void Init_Task(void *pvParameters);//
//
//static void Init_Task(void *pvParameters)
//{
//
//	//Power_Detector_Init();
//	//vTaskDelay(200);
//
//
//
//	//Watchdog_Init();
////
//	spi1_config();
//	tablo_devices_init();
////
////    buzzer_init();
//	//Encoder_Init();
//	//Proto_Init(PROTO_FIRST_INIT);
//
//    vTaskDelete( NULL );
//}

int main(void)
{
	SystemInit();
	tablo_devices_init();
	Keyboard_Init();

    vTaskStartScheduler();

    while(1);
}
//---------------------------------------------------------------------------------------

void vApplicationTickHook( void )
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
}
/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
volatile size_t xFreeStackSpace;

	xFreeStackSpace = xPortGetFreeHeapSize();

	if( xFreeStackSpace > 100 )
	{
	}
}
