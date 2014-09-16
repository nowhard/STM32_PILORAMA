#ifndef SPI_BUS_H
#define SPI_BUS_H

#include "stm32f4xx.h"
#include "indicator.h"

#define SPI1_GPIO_BUS 	RCC_AHB1Periph_GPIOB
#define SPI1_GPIO		GPIOB

#define SPI1_CS1	GPIO_Pin_6
#define SPI1_CS2	GPIO_Pin_7

#define SPI2_GPIO_BUS 	    RCC_AHB1Periph_GPIOB
#define SPI2_CS_GPIO_BUS 	RCC_AHB1Periph_GPIOC
#define SPI2_GPIO			GPIOB
#define SPI2_GPIO_CS		GPIOC

#define SPI2_CS1	GPIO_Pin_6
#define SPI2_CS2	GPIO_Pin_7

struct bus
{
	uint8_t indicators_num;
	uint16_t bus_buf[IND_SPI_BUS_1_NUM][IND_COMMAND_LEN];
	uint8_t error;
};

#define BUS_NUM	2


enum
{
	 BUS_SPI_1=0,
	 BUS_SPI_2=1,
};

enum
{
	BUS_ERROR_NONE	=0,
	BUS_ERROR_POWER=1
};

uint8_t spi_buses_init(void);

#endif
