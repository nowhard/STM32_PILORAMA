
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_dac.h"

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
#include "proto.h"
#include "keyboard.h"
//#include "buzzer.h"
//#include "power.h"
#include "watchdog.h"
//#include "relay.h"
//#include "power_detector.h"
#include "encoder.h"
#include "dac.h"
#include "rtc.h"


static void Init_Task(void *pvParameters);//

static void Init_Task(void *pvParameters)
{

	//Power_Detector_Init();
	//vTaskDelay(200);



	//Watchdog_Init();
//
	spi1_config();
	tablo_devices_init();
//




//    buzzer_init();
	//Encoder_Init();
	Proto_Init(PROTO_FIRST_INIT);
	DAC_Current_Init();

	DAC_SetChannel2Data(DAC_Align_12b_R, 0x7FF);
	RTC_Clock_Init();
    vTaskDelete( NULL );
}

int main(void)
{
	SystemInit();

    xTaskCreate(Init_Task,(signed char*)"INIT",128,NULL, tskIDLE_PRIORITY + 1, NULL);

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
