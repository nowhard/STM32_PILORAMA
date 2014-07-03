#include "power.h"
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "spi_bus.h"
#include "tablo.h"

extern struct tablo tab;//

void Power_Init(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1|GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_WriteBit(GPIOB, GPIO_Pin_7, Bit_SET);
    GPIO_WriteBit(GPIOA, GPIO_Pin_1, Bit_SET);
    GPIO_WriteBit(GPIOA, GPIO_Pin_12,Bit_SET);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0|GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

 	if(Power_Channel_1_OK())
 	{
 		Power_On_Channel_1();
 		tab.buses[BUS_SPI_3].error=BUS_ERROR_NONE;
 	}
 	else
 	{
 		tab.buses[BUS_SPI_3].error=BUS_ERROR_POWER;
	}

 	if(Power_Channel_2_OK())
 	{
 		Power_On_Channel_2();
 		tab.buses[BUS_SPI_2].error=BUS_ERROR_NONE;
 	}
 	else
 	{
 		tab.buses[BUS_SPI_2].error=BUS_ERROR_POWER;
	}

 	if(Power_Channel_3_OK())
 	{
 		Power_On_Channel_3();
 		tab.buses[BUS_SPI_1].error=BUS_ERROR_NONE;
 	}
 	else
 	{
 		tab.buses[BUS_SPI_1].error=BUS_ERROR_POWER;
 	}
}

void Power_On_Channel_1(void)
{
	GPIO_WriteBit(GPIOB, GPIO_Pin_7, Bit_RESET);
}
void Power_On_Channel_2(void)
{
	GPIO_WriteBit(GPIOA, GPIO_Pin_12, Bit_RESET);
}
void Power_On_Channel_3(void)
{
	GPIO_WriteBit(GPIOA, GPIO_Pin_1, Bit_RESET);
}

void Power_Off_Channel_1(void)
{
	GPIO_WriteBit(GPIOB, GPIO_Pin_7, Bit_SET);
}
void Power_Off_Channel_2(void)
{
	GPIO_WriteBit(GPIOA, GPIO_Pin_12, Bit_SET);
}

void Power_Off_Channel_3(void)
{
	GPIO_WriteBit(GPIOA, GPIO_Pin_1, Bit_SET);
}

uint8_t Power_Channel_1_OK(void)
{
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8)==Bit_SET)
	{
		return Bit_RESET;
	}
	else
	{
		return Bit_SET;
	}
}
uint8_t Power_Channel_2_OK(void)
{
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_15)==Bit_SET)
	{
		return Bit_RESET;
	}
	else
	{
		return Bit_SET;
	}
}
uint8_t Power_Channel_3_OK(void)
{
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)==Bit_SET)
	{
		return Bit_RESET;
	}
	else
	{
		return Bit_SET;
	}
}
