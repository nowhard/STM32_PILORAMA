#include "rtc.h"
#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_rtc.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "tablo.h"
#include "tablo_parser.h"

extern struct tablo tab;//

static void RTC_Task(void *pvParameters);

uint32_t AsynchPrediv = 0, SynchPrediv = 0;
void RTC_Clock_Init(void)
{
	RTC_TimeTypeDef RTC_TimeStruct;
	RTC_DateTypeDef RTC_DateStruct;
	RTC_InitTypeDef RTC_InitStruct;
	/* Enable the PWR clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	/* Allow access to RTC */
	//RTC_WriteProtectionCmd(DISABLE);
	PWR_BackupAccessCmd(ENABLE);

	RCC_LSEConfig(RCC_LSE_ON);

	/* Wait till LSE is ready */
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
	{
	}

	/* Select the RTC Clock Source */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

    SynchPrediv = 0xFF;
    AsynchPrediv = 0x7F;

	RCC_RTCCLKCmd(ENABLE);
	RTC_WaitForSynchro();

	RTC_WriteBackupRegister(RTC_BKP_DR0, FIRST_DATA);

	RTC_StructInit(&RTC_InitStruct);

//	RTC_TimeStruct.RTC_Hours = (1<<4)|0;
//	RTC_TimeStruct.RTC_Minutes = (2<<4)|3;
//	RTC_TimeStruct.RTC_Seconds = 0;
//	RTC_SetTime(RTC_Format_BCD,&RTC_TimeStruct);
//
//	RTC_DateStruct.RTC_WeekDay = RTC_Weekday_Thursday;
//	RTC_DateStruct.RTC_Date = 14;
//	RTC_DateStruct.RTC_Month = RTC_Month_March;
//	RTC_DateStruct.RTC_Year = 13;
//	RTC_SetDate(RTC_Format_BCD,&RTC_DateStruct);

	RTC_InitStruct.RTC_AsynchPrediv = AsynchPrediv;
	RTC_InitStruct.RTC_SynchPrediv = SynchPrediv;
	RTC_InitStruct.RTC_HourFormat = RTC_HourFormat_24;
    RTC_Init(&RTC_InitStruct);

    xTaskCreate(RTC_Task,(signed char*)"RTC",128,NULL, tskIDLE_PRIORITY + 1, NULL);
}

static void RTC_Task(void *pvParameters)
{
    RTC_TimeTypeDef RTC_TimeStructure;
//    RTC_DateTypeDef RTC_DateStructure;
    uint8_t str_buf[8];
	while(1)
	{
        RTC_GetTime(RTC_Format_BCD, &RTC_TimeStructure);
        //RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);
        str_buf[0]=0x30+((RTC_TimeStructure.RTC_Minutes>>4)&0xF);
        str_buf[1]=0x30+(RTC_TimeStructure.RTC_Minutes&0xF);
        str_buf[2]='.';
        str_buf[3]=0x30+((RTC_TimeStructure.RTC_Seconds>>4)&0xF);
        str_buf[4]=0x30+(RTC_TimeStructure.RTC_Seconds&0xF);
        str_buf[5]=NULL;

     //   str_to_ind(&tab.indicators[0],str_buf);
        vTaskDelay(200);
	}
}
