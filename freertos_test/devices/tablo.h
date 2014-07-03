#ifndef TABLO_H
#define TABLO_H

#include "stm32f4xx.h"
#include "spi_bus.h"
#include "buzzer.h"

#define UART_BUF_LEN			0xC0
#define TABLO_PROTO_BUF_LEN		0xC0


//#define DISCR_OUT_ON	0x1
//#define DISCR_OUT_OFF	0x0

enum
{
	DISCR_OUT_OFF=0x0,
	DISCR_OUT_ON=0x1
};

struct tablo{
	struct bus buses[BUS_NUM];//
	struct indicator indicators[IND_ALL_NUM];//
	struct buzzer buz;//
	uint8_t discr_outputs[4];//
	uint8_t uart_buf[UART_BUF_LEN];//
	uint8_t tablo_proto_buf[TABLO_PROTO_BUF_LEN];//
};

void tablo_indicator_struct_init(void);//
uint8_t tablo_devices_init(void);//

#endif
