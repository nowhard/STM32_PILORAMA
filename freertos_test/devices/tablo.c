#include "tablo.h"

struct tablo tab;//

void tablo_indicator_struct_init(void)//
{
	tab.buses[BUS_SPI_1].indicators_num=2; //
//---------------------------------------------------------------------------
	tab.indicators[0].bus=BUS_SPI_1;
	tab.indicators[0].number_in_bus=0;
	tab.indicators[0].type=IND_TYPE_SEGMENT;
	tab.indicators[0].character_num=5;
	tab.indicators[0].brightness=IND_BRIGHTNESS|0x9;
	tab.indicators[0].decode_mode=IND_DECODE|IND_DECODE_OFF;
	tab.indicators[0].scan_limit=IND_SCAN_LIMIT|(tab.indicators[0].character_num-1);
	tab.indicators[0].shutdown=IND_SHUTDOWN|IND_SHUTDOWN_OFF;
	tab.indicators[0].display_test=IND_DISPLAY_TEST|IND_DISPLAY_TEST_OFF;
	tab.indicators[0].blink_mask=BLINK_FALSE;
	tab.indicators[0].blink_num=0;

	tab.indicators[1].bus=BUS_SPI_2;
	tab.indicators[1].number_in_bus=0;
	tab.indicators[1].type=IND_TYPE_SEGMENT;
	tab.indicators[1].character_num=5;
	tab.indicators[1].brightness=IND_BRIGHTNESS|0x9;
	tab.indicators[1].decode_mode=IND_DECODE|IND_DECODE_OFF;
	tab.indicators[1].scan_limit=IND_SCAN_LIMIT|(tab.indicators[1].character_num-1);
	tab.indicators[1].shutdown=IND_SHUTDOWN|IND_SHUTDOWN_OFF;
	tab.indicators[1].display_test=IND_DISPLAY_TEST|IND_DISPLAY_TEST_OFF;
	tab.indicators[1].blink_mask=BLINK_FALSE;
	tab.indicators[1].blink_num=0;
}

uint8_t tablo_devices_init(void)//
{
	tablo_indicator_struct_init();
	spi_buses_init();
	return 0;
}
