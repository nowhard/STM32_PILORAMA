#include "rtc.h"
#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_rtc.h"

void RTC_config(void)
{
	RTC_TimeTypeDef RTC_TimeStruct;
	RTC_DateTypeDef RTC_DateStruct;
	RTC_InitTypeDef RTC_InitStruct;
	/* Enable the PWR clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	/* Allow access to RTC */
	RTC_WriteProtectionCmd(DISABLE);
	PWR_BackupAccessCmd(ENABLE);

	RCC_LSEConfig(RCC_LSE_ON);

	/* Wait till LSE is ready */
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
	{
	}

	/* Select the RTC Clock Source */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	RCC_RTCCLKCmd(ENABLE);
	RTC_WaitForSynchro();

	 RTC_WriteBackupRegister(RTC_BKP_DR0, FIRST_DATA);

	RTC_StructInit(&RTC_InitStruct);

	RTC_TimeStruct.RTC_Hours = 16;
	RTC_TimeStruct.RTC_Minutes = 30;
	RTC_TimeStruct.RTC_Seconds = 5;
	RTC_SetTime(RTC_Format_BCD,&RTC_TimeStruct);

	RTC_DateStruct.RTC_WeekDay = RTC_Weekday_Thursday;
	RTC_DateStruct.RTC_Date = 14;
	RTC_DateStruct.RTC_Month = RTC_Month_March;
	RTC_DateStruct.RTC_Year = 13;
	RTC_SetDate(RTC_Format_BCD,&RTC_DateStruct);
}
