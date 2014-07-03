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
	tab.buses[BUS_MOD_2].indicators_num=BUS_MOD_2_NUM; //
	tab.buses[BUS_MOD_1].indicators_num=BUS_MOD_1_NUM;
	tab.buses[BUS_MOD_3].indicators_num=BUS_MOD_3_NUM;
//---------------------------------------------------------------------------
	tab.indicators[0].bus=BUS_MOD_1;
	tab.indicators[0].number_in_bus=(BUS_MOD_1_NUM-1)-3;
	tab.indicators[0].type=IND_TYPE_SEGMENT;
	tab.indicators[0].character_num=3;
	tab.indicators[0].brightness=IND_BRIGHTNESS|0x9;
	tab.indicators[0].decode_mode=IND_DECODE|IND_DECODE_OFF;
	tab.indicators[0].scan_limit=IND_SCAN_LIMIT|(tab.indicators[0].character_num-1);
	tab.indicators[0].shutdown=IND_SHUTDOWN|IND_SHUTDOWN_OFF;
	tab.indicators[0].display_test=IND_DISPLAY_TEST|IND_DISPLAY_TEST_OFF;
	tab.indicators[0].blink=BLINK_FALSE;

	tab.indicators[1].bus=BUS_MOD_2;
	tab.indicators[1].number_in_bus=(BUS_MOD_2_NUM-1)-1;
	tab.indicators[1].type=IND_TYPE_SEGMENT;
	tab.indicators[1].character_num=3;
	tab.indicators[1].brightness=IND_BRIGHTNESS|0x9;
	tab.indicators[1].decode_mode=IND_DECODE|IND_DECODE_OFF;
	tab.indicators[1].scan_limit=IND_SCAN_LIMIT|(tab.indicators[1].character_num-1);
	tab.indicators[1].shutdown=IND_SHUTDOWN|IND_SHUTDOWN_OFF;
	tab.indicators[1].display_test=IND_DISPLAY_TEST|IND_DISPLAY_TEST_OFF;
	tab.indicators[1].blink=BLINK_FALSE;

	tab.indicators[2].bus=BUS_MOD_1;
	tab.indicators[2].number_in_bus=(BUS_MOD_1_NUM-1)-2;
	tab.indicators[2].type=IND_TYPE_SEGMENT;
	tab.indicators[2].character_num=3;
	tab.indicators[2].brightness=IND_BRIGHTNESS|0x9;
	tab.indicators[2].decode_mode=IND_DECODE|IND_DECODE_OFF;
	tab.indicators[2].scan_limit=IND_SCAN_LIMIT|(tab.indicators[2].character_num-1);
	tab.indicators[2].shutdown=IND_SHUTDOWN|IND_SHUTDOWN_OFF;
	tab.indicators[2].display_test=IND_DISPLAY_TEST|IND_DISPLAY_TEST_OFF;
	tab.indicators[2].blink=BLINK_FALSE;

	tab.indicators[3].bus=BUS_MOD_1;
	tab.indicators[3].number_in_bus=(BUS_MOD_1_NUM-1)-1;
	tab.indicators[3].type=IND_TYPE_SEGMENT;
	tab.indicators[3].character_num=3;
	tab.indicators[3].brightness=IND_BRIGHTNESS|0x9;
	tab.indicators[3].decode_mode=IND_DECODE|IND_DECODE_OFF;
	tab.indicators[3].scan_limit=IND_SCAN_LIMIT|(tab.indicators[3].character_num-1);
	tab.indicators[3].shutdown=IND_SHUTDOWN|IND_SHUTDOWN_OFF;
	tab.indicators[3].display_test=IND_DISPLAY_TEST|IND_DISPLAY_TEST_OFF;
	tab.indicators[3].blink=BLINK_FALSE;

	tab.indicators[4].bus=BUS_MOD_1;
	tab.indicators[4].number_in_bus=(BUS_MOD_1_NUM-1)-0;
	tab.indicators[4].type=IND_TYPE_SEGMENT;
	tab.indicators[4].character_num=3;
	tab.indicators[4].brightness=IND_BRIGHTNESS|0x9;
	tab.indicators[4].decode_mode=IND_DECODE|IND_DECODE_OFF;
	tab.indicators[4].scan_limit=IND_SCAN_LIMIT|(tab.indicators[4].character_num-1);
	tab.indicators[4].shutdown=IND_SHUTDOWN|IND_SHUTDOWN_OFF;
	tab.indicators[4].display_test=IND_DISPLAY_TEST|IND_DISPLAY_TEST_OFF;
	tab.indicators[4].blink=BLINK_FALSE;

	tab.indicators[5].bus=BUS_MOD_3;
	tab.indicators[5].number_in_bus=(BUS_MOD_3_NUM-1)-1;
	tab.indicators[5].type=IND_TYPE_SEGMENT;
	tab.indicators[5].character_num=3;
	tab.indicators[5].brightness=IND_BRIGHTNESS|0x9;
	tab.indicators[5].decode_mode=IND_DECODE|IND_DECODE_OFF;
	tab.indicators[5].scan_limit=IND_SCAN_LIMIT|(tab.indicators[5].character_num-1);
	tab.indicators[5].shutdown=IND_SHUTDOWN|IND_SHUTDOWN_OFF;
	tab.indicators[5].display_test=IND_DISPLAY_TEST|IND_DISPLAY_TEST_OFF;
	tab.indicators[5].blink=BLINK_FALSE;


	tab.indicators[6].bus=BUS_MOD_2;
	tab.indicators[6].number_in_bus=(BUS_MOD_2_NUM-1)-0;
	tab.indicators[6].type=IND_TYPE_SEGMENT;
	tab.indicators[6].character_num=3;
	tab.indicators[6].brightness=IND_BRIGHTNESS|0x9;
	tab.indicators[6].decode_mode=IND_DECODE|IND_DECODE_OFF;
	tab.indicators[6].scan_limit=IND_SCAN_LIMIT|(tab.indicators[6].character_num-1);
	tab.indicators[6].shutdown=IND_SHUTDOWN|IND_SHUTDOWN_OFF;
	tab.indicators[6].display_test=IND_DISPLAY_TEST|IND_DISPLAY_TEST_OFF;
	tab.indicators[6].blink=BLINK_FALSE;

	tab.indicators[7].bus=BUS_MOD_2;
	tab.indicators[7].number_in_bus=(BUS_MOD_2_NUM-1)-5;
	tab.indicators[7].type=IND_TYPE_SEGMENT;
	tab.indicators[7].character_num=3;
	tab.indicators[7].brightness=IND_BRIGHTNESS|0x9;
	tab.indicators[7].decode_mode=IND_DECODE|IND_DECODE_OFF;
	tab.indicators[7].scan_limit=IND_SCAN_LIMIT|(tab.indicators[7].character_num-1);
	tab.indicators[7].shutdown=IND_SHUTDOWN|IND_SHUTDOWN_OFF;
	tab.indicators[7].display_test=IND_DISPLAY_TEST|IND_DISPLAY_TEST_OFF;
	tab.indicators[7].blink=BLINK_FALSE;

	tab.indicators[8].bus=BUS_MOD_3;
	tab.indicators[8].number_in_bus=(BUS_MOD_3_NUM-1)-2;
	tab.indicators[8].type=IND_TYPE_SEGMENT;
	tab.indicators[8].character_num=5;
	tab.indicators[8].brightness=IND_BRIGHTNESS|0x9;
	tab.indicators[8].decode_mode=IND_DECODE|IND_DECODE_OFF;
	tab.indicators[8].scan_limit=IND_SCAN_LIMIT|(tab.indicators[8].character_num-1);
	tab.indicators[8].shutdown=IND_SHUTDOWN|IND_SHUTDOWN_OFF;
	tab.indicators[8].display_test=IND_DISPLAY_TEST|IND_DISPLAY_TEST_OFF;
	tab.indicators[8].blink=BLINK_FALSE;

	tab.indicators[9].bus=BUS_MOD_2;
	tab.indicators[9].number_in_bus=(BUS_MOD_2_NUM-1)-6;
	tab.indicators[9].type=IND_TYPE_SEGMENT;
	tab.indicators[9].character_num=5;
	tab.indicators[9].brightness=IND_BRIGHTNESS|0x9;
	tab.indicators[9].decode_mode=IND_DECODE|IND_DECODE_OFF;
	tab.indicators[9].scan_limit=IND_SCAN_LIMIT|(tab.indicators[9].character_num-1);
	tab.indicators[9].shutdown=IND_SHUTDOWN|IND_SHUTDOWN_OFF;
	tab.indicators[9].display_test=IND_DISPLAY_TEST|IND_DISPLAY_TEST_OFF;
	tab.indicators[9].blink=BLINK_FALSE;

	tab.indicators[10].bus=BUS_MOD_2;
	tab.indicators[10].number_in_bus=(BUS_MOD_2_NUM-1)-4;
	tab.indicators[10].type=IND_TYPE_SEGMENT;
	tab.indicators[10].character_num=4;
	tab.indicators[10].brightness=IND_BRIGHTNESS|0x9;
	tab.indicators[10].decode_mode=IND_DECODE|IND_DECODE_OFF;
	tab.indicators[10].scan_limit=IND_SCAN_LIMIT|(tab.indicators[10].character_num-1);
	tab.indicators[10].shutdown=IND_SHUTDOWN|IND_SHUTDOWN_OFF;
	tab.indicators[10].display_test=IND_DISPLAY_TEST|IND_DISPLAY_TEST_OFF;
	tab.indicators[10].blink=BLINK_FALSE;

	tab.indicators[11].bus=BUS_MOD_3;
	tab.indicators[11].number_in_bus=(BUS_MOD_3_NUM-1)-4;
	tab.indicators[11].type=IND_TYPE_SEGMENT;
	tab.indicators[11].character_num=3;
	tab.indicators[11].brightness=IND_BRIGHTNESS|0x9;
	tab.indicators[11].decode_mode=IND_DECODE|IND_DECODE_OFF;
	tab.indicators[11].scan_limit=IND_SCAN_LIMIT|(tab.indicators[11].character_num-1);
	tab.indicators[11].shutdown=IND_SHUTDOWN|IND_SHUTDOWN_OFF;
	tab.indicators[11].display_test=IND_DISPLAY_TEST|IND_DISPLAY_TEST_OFF;
	tab.indicators[11].blink=BLINK_FALSE;

	tab.indicators[12].bus=BUS_MOD_3;
	tab.indicators[12].number_in_bus=(BUS_MOD_3_NUM-1)-3;
	tab.indicators[12].type=IND_TYPE_SEGMENT;
	tab.indicators[12].character_num=4;
	tab.indicators[12].brightness=IND_BRIGHTNESS|0x9;
	tab.indicators[12].decode_mode=IND_DECODE|IND_DECODE_OFF;
	tab.indicators[12].scan_limit=IND_SCAN_LIMIT|(tab.indicators[12].character_num-1);
	tab.indicators[12].shutdown=IND_SHUTDOWN|IND_SHUTDOWN_OFF;
	tab.indicators[12].display_test=IND_DISPLAY_TEST|IND_DISPLAY_TEST_OFF;
	tab.indicators[12].blink=BLINK_FALSE;

	tab.indicators[13].bus=BUS_MOD_1;
	tab.indicators[13].number_in_bus=(BUS_MOD_1_NUM-1)-5;
	tab.indicators[13].type=IND_TYPE_LINE;
	tab.indicators[13].character_num=8;
	tab.indicators[13].brightness=IND_BRIGHTNESS|0x9;
	tab.indicators[13].decode_mode=IND_DECODE|IND_DECODE_OFF;
	tab.indicators[13].scan_limit=IND_SCAN_LIMIT|(tab.indicators[13].character_num-1);
	tab.indicators[13].shutdown=IND_SHUTDOWN|IND_SHUTDOWN_OFF;
	tab.indicators[13].display_test=IND_DISPLAY_TEST|IND_DISPLAY_TEST_OFF;
	tab.indicators[13].blink=BLINK_FALSE;

	tab.indicators[14].bus=BUS_MOD_3;
	tab.indicators[14].number_in_bus=(BUS_MOD_3_NUM-1)-0;
	tab.indicators[14].type=IND_TYPE_LINE;
	tab.indicators[14].character_num=8;
	tab.indicators[14].brightness=IND_BRIGHTNESS|0x9;
	tab.indicators[14].decode_mode=IND_DECODE|IND_DECODE_OFF;
	tab.indicators[14].scan_limit=IND_SCAN_LIMIT|(tab.indicators[14].character_num-1);
	tab.indicators[14].shutdown=IND_SHUTDOWN|IND_SHUTDOWN_OFF;
	tab.indicators[14].display_test=IND_DISPLAY_TEST|IND_DISPLAY_TEST_OFF;
	tab.indicators[14].blink=BLINK_FALSE;

	tab.indicators[15].bus=BUS_MOD_3;
	tab.indicators[15].number_in_bus=(BUS_MOD_3_NUM-1)-5;
	tab.indicators[15].type=IND_TYPE_LINE;
	tab.indicators[15].character_num=0x8;
	tab.indicators[15].brightness=IND_BRIGHTNESS|0x9;
	tab.indicators[15].decode_mode=IND_DECODE|IND_DECODE_OFF;
	tab.indicators[15].scan_limit=IND_SCAN_LIMIT|0xFF;
	tab.indicators[15].shutdown=IND_SHUTDOWN|IND_SHUTDOWN_OFF;
	tab.indicators[15].display_test=IND_DISPLAY_TEST|IND_DISPLAY_TEST_OFF;
	tab.indicators[15].blink=BLINK_FALSE;

	tab.indicators[16].bus=BUS_MOD_1;
	tab.indicators[16].number_in_bus=(BUS_MOD_1_NUM-1)-4;
	tab.indicators[16].type=IND_TYPE_LINE;
	tab.indicators[16].character_num=8;
	tab.indicators[16].brightness=IND_BRIGHTNESS|0x9;
	tab.indicators[16].decode_mode=IND_DECODE|IND_DECODE_OFF;
	tab.indicators[16].scan_limit=IND_SCAN_LIMIT|(tab.indicators[16].character_num-1);
	tab.indicators[16].shutdown=IND_SHUTDOWN|IND_SHUTDOWN_OFF;
	tab.indicators[16].display_test=IND_DISPLAY_TEST|IND_DISPLAY_TEST_OFF;
	tab.indicators[16].blink=BLINK_FALSE;

	tab.indicators[17].bus=BUS_MOD_1;
	tab.indicators[17].number_in_bus=(BUS_MOD_1_NUM-1)-6;
	tab.indicators[17].type=IND_TYPE_SEGMENT;
	tab.indicators[17].character_num=5;
	tab.indicators[17].brightness=IND_BRIGHTNESS|0x9;
	tab.indicators[17].decode_mode=IND_DECODE|IND_DECODE_OFF;
	tab.indicators[17].scan_limit=IND_SCAN_LIMIT|(tab.indicators[17].character_num-1);
	tab.indicators[17].shutdown=IND_SHUTDOWN|IND_SHUTDOWN_OFF;
	tab.indicators[17].display_test=IND_DISPLAY_TEST|IND_DISPLAY_TEST_OFF;
	tab.indicators[17].blink=BLINK_FALSE;

	tab.indicators[18].bus=BUS_MOD_2;
	tab.indicators[18].number_in_bus=(BUS_MOD_2_NUM-1)-2;
	tab.indicators[18].type=IND_TYPE_LINE;
	tab.indicators[18].character_num=8;
	tab.indicators[18].brightness=IND_BRIGHTNESS|0x9;
	tab.indicators[18].decode_mode=IND_DECODE|IND_DECODE_OFF;
	tab.indicators[18].scan_limit=IND_SCAN_LIMIT|(tab.indicators[18].character_num-1);
	tab.indicators[18].shutdown=IND_SHUTDOWN|IND_SHUTDOWN_OFF;
	tab.indicators[18].display_test=IND_DISPLAY_TEST|IND_DISPLAY_TEST_OFF;
	tab.indicators[18].blink=BLINK_FALSE;
//
	tab.indicators[19].bus=BUS_MOD_2;
	tab.indicators[19].number_in_bus=(BUS_MOD_2_NUM-1)-3;
	tab.indicators[19].type=IND_TYPE_LINE;
	tab.indicators[19].character_num=8;
	tab.indicators[19].brightness=IND_BRIGHTNESS|0x9;
	tab.indicators[19].decode_mode=IND_DECODE|IND_DECODE_OFF;
	tab.indicators[19].scan_limit=IND_SCAN_LIMIT|(tab.indicators[19].character_num-1);
	tab.indicators[19].shutdown=IND_SHUTDOWN|IND_SHUTDOWN_OFF;
	tab.indicators[19].display_test=IND_DISPLAY_TEST|IND_DISPLAY_TEST_OFF;
	tab.indicators[19].blink=BLINK_FALSE;
}

uint8_t tablo_devices_init(void)//
{
	uint8_t error=0;

	tablo_indicator_struct_init();
	spi_buses_init();

	return error;
}
