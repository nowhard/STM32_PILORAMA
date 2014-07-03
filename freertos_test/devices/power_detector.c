#include "power_detector.h"
#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_pwr.h"
#include "stm32f4xx_exti.h"
#include "misc.h"
void Power_Detector_Init(void)
{
	 RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

	  NVIC_InitTypeDef NVIC_InitStructure;

	  /* Configure one bit for preemption priority */
	  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	  /* Enable the PVD Interrupt */
	  NVIC_InitStructure.NVIC_IRQChannel = PVD_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	  NVIC_Init(&NVIC_InitStructure);

	  EXTI_InitTypeDef EXTI_InitStructure;

	  /* Configure EXTI Line16(PVD Output) to generate an interrupt on rising and
	     falling edges */
	  EXTI_ClearITPendingBit(EXTI_Line16);
	  EXTI_InitStructure.EXTI_Line = EXTI_Line16;
	  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	  EXTI_Init(&EXTI_InitStructure);
	    // Configure the PVD Level to 2.2V
	  PWR_PVDLevelConfig(PWR_PVDLevel_5);
	    // Enable the PVD Output
	  PWR_PVDCmd(ENABLE);
}

void PVD_IRQHandler(void) {
    // voltage dropping too low
    if (EXTI_GetITStatus(EXTI_Line16) != RESET) {
        // shut everything down
    	NVIC_SystemReset();
        EXTI_ClearITPendingBit(EXTI_Line16);
    }
}
