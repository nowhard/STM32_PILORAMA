#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"

#include <misc.h>
#include "system_stm32f4xx.h"

//FreeRTOS:

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"


#include "menu.h"
#include "spi_bus.h"
//#include "tablo.h"

#include "keyboard.h"
#include "buzzer.h"
//#include "power.h"
#include "watchdog.h"
//#include "relay.h"
#include "power_detector.h"
#include "encoder.h"
#include "external_events.h"
#include "backup_sram.h"
#include "drive.h"


int main(void)
{
	SystemInit();
	Power_Detector_Init();
	Backup_SRAM_Init();
	Drive_Init();
	Encoder_Init();
	External_Events_Init();

	tablo_devices_init();
	Keyboard_Init();
	buzzer_init();
	Menu_Init();
	//Watchdog_Init();


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
