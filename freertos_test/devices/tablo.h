#ifndef TABLO_H
#define TABLO_H

#include "stm32f4xx.h"
#include "spi_bus.h"

struct tablo{
	struct bus buses[BUS_NUM];//
	struct indicator indicators[IND_ALL_NUM];//
};

void tablo_indicator_struct_init(void);//
uint8_t tablo_devices_init(void);//

#endif
