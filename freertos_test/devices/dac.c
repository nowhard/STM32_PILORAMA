#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_dac.h"
#include <misc.h>

#include "dac.h"

void DAC_Current_Init(void)
{
	  GPIO_InitTypeDef GPIO_InitStructure;
	  DAC_InitTypeDef DAC_InitStructure;

	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1 | RCC_AHB1Periph_GPIOA, ENABLE);

	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	  GPIO_Init(GPIOA, &GPIO_InitStructure);

	  DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
	  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
	  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
	  DAC_Init(DAC_Channel_2, &DAC_InitStructure);

	  DAC_Cmd(DAC_Channel_2, ENABLE);
}
