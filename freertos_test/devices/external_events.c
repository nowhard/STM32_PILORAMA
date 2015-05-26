#include "external_events.h"
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"
#include <misc.h>

#include "tablo_parser.h"
#include "buzzer.h"

//»нклуды от FreeRTOS:

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "drive.h"

#include "watchdog.h"
extern struct task_watch task_watches[];

void ExtEventsHandler( void *pvParameters );
extern struct drive drv;


void External_Events_Init(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//тактируем портј
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);



	GPIO_InitTypeDef  GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = DRIVE_ERROR|DRIVE_LIMIT_UP|DRIVE_LIMIT_DOWN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(DRIVE_EXT_EVENTS_PORT, &GPIO_InitStructure);

	EXTI_InitTypeDef EXTI_InitStructure;

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, GPIO_PinSource0);
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, GPIO_PinSource6);
	EXTI_InitStructure.EXTI_Line = EXTI_Line6;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, GPIO_PinSource7);
	EXTI_InitStructure.EXTI_Line = EXTI_Line7;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );

	NVIC_InitTypeDef NVIC_InitStructure;

//	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 14;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//NVIC_EnableIRQ(EXTI9_5_IRQn);

	 xTaskCreate(ExtEventsHandler,(signed char*)"Ext_Events",128,NULL, tskIDLE_PRIORITY + 1, NULL);
}

#define DETECTION_ERROR_DELAY	50 //5 sec
void ExtEventsHandler( void *pvParameters )
{
	uint16_t detection_error_delay_counter=0;
	task_watches[EXT_EVENTS_TASK].task_status=TASK_ACTIVE;
	while(1)
	{
		if(detection_error_delay_counter<DETECTION_ERROR_DELAY)
		{
			detection_error_delay_counter++;
		}
		else
		{
			if(GPIO_ReadInputDataBit(DRIVE_EXT_EVENTS_PORT,DRIVE_ERROR)==Bit_RESET)
			{
				Drive_Stop(STOP_INVERTOR_ERROR,FROM_TASK);
			}
			else
			{
				EXTI->IMR |= EXTI_Line0;
			}
		}

		if(GPIO_ReadInputDataBit(DRIVE_EXT_EVENTS_PORT,DRIVE_LIMIT_UP)==Bit_RESET)
		{
			drv.limitation_flag=DRIVE_LIMITATION_ONLY_DOWN;
		}
		else
		{
			if(GPIO_ReadInputDataBit(DRIVE_EXT_EVENTS_PORT,DRIVE_LIMIT_DOWN)==Bit_RESET)
			{
				drv.limitation_flag=DRIVE_LIMITATION_ONLY_UP;
			}
			else
			{
				drv.limitation_flag=DRIVE_LIMITATION_NONE;
				EXTI->IMR |= (EXTI_Line6|EXTI_Line7);
			}
		}
		task_watches[EXT_EVENTS_TASK].counter++;
		vTaskDelay(100);
	}
}

//void EXTI0_IRQHandler(void)
//{
//    if(EXTI_GetITStatus(EXTI_Line0) != RESET)
//    {
//        EXTI_ClearITPendingBit(EXTI_Line0);
//       // Drive_Stop(STOP_INVERTOR_ERROR,FROM_ISR);
//        EXTI->IMR &= ~EXTI_Line0;
//    }
//}

void EXTI9_5_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line6) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line6);
        EXTI->IMR &= ~EXTI_Line6;
        Drive_Stop(STOP_HI_SENSOR,FROM_ISR);
    }

    if(EXTI_GetITStatus(EXTI_Line7) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line7);
        EXTI->IMR &= ~EXTI_Line7;
        Drive_Stop(STOP_LO_SENSOR,FROM_ISR);
    }
}
