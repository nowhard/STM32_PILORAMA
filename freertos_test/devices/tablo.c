#include "tablo.h"

struct tablo tab;//

#define BUS_MOD_1 		BUS_SPI_3
#define BUS_MOD_1_NUM	IND_SPI_BUS_3_NUM

#define BUS_MOD_2 		BUS_SPI_2
#define BUS_MOD_2_NUM	IND_SPI_BUS_2_NUM

#define BUS_MOD_3 		BUS_SPI_1
#define BUS_MOD_3_NUM	IND_SPI_BUS_1_NUM

void tablo_indicator_struct_init(void)//
{
	tab.buses[BUS_SPI_1].indicators_num=2; //
//---------------------------------------------------------------------------
	tab.indicators[0].bus=BUS_SPI_1;
	tab.indicators[0].number_in_bus=0;
	tab.indicators[0].type=IND_TYPE_SEGMENT;
	tab.indicators[0].character_num=4;
	tab.indicators[0].brightness=IND_BRIGHTNESS|0x9;
	tab.indicators[0].decode_mode=IND_DECODE|IND_DECODE_OFF;
	tab.indicators[0].scan_limit=IND_SCAN_LIMIT|(tab.indicators[0].character_num-1);
	tab.indicators[0].shutdown=IND_SHUTDOWN|IND_SHUTDOWN_OFF;
	tab.indicators[0].display_test=IND_DISPLAY_TEST|IND_DISPLAY_TEST_OFF;
	tab.indicators[0].blink=BLINK_FALSE;

	tab.indicators[1].bus=BUS_SPI_1;
	tab.indicators[1].number_in_bus=1;
	tab.indicators[1].type=IND_TYPE_SEGMENT;
	tab.indicators[1].character_num=4;
	tab.indicators[1].brightness=IND_BRIGHTNESS|0x9;
	tab.indicators[1].decode_mode=IND_DECODE|IND_DECODE_OFF;
	tab.indicators[1].scan_limit=IND_SCAN_LIMIT|(tab.indicators[1].character_num-1);
	tab.indicators[1].shutdown=IND_SHUTDOWN|IND_SHUTDOWN_OFF;
	tab.indicators[1].display_test=IND_DISPLAY_TEST|IND_DISPLAY_TEST_OFF;
	tab.indicators[1].blink=BLINK_FALSE;
}

uint8_t tablo_devices_init(void)//
{
	tablo_indicator_struct_init();
	spi_buses_init();
	return 0;
}
