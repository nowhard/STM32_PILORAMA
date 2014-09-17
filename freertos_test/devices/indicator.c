#include "indicator.h"
#include "tablo.h"
#include "tablo_parser.h"
extern struct tablo tab;
static uint8_t blink_flag=0;

void Indicator_Blink_Handler(void)
{
	uint8_t indicator=0;
	uint8_t i=0;

	blink_flag++;

	if(blink_flag<5)
	{
		return;
	}

	if(blink_flag>=10)
	{
		blink_flag=0;
	}

	for(indicator=0;indicator<IND_ALL_NUM;indicator++)
	{
		if((tab.indicators[indicator].blink_mask==BLINK_FALSE)||(tab.indicators[indicator].blink_num==0))
		{
			tab.indicators[indicator].blink_num=0;
			continue;
		}
		else
		{
			if(tab.indicators[indicator].blink_num<0xFF)//finite blink
			{
				tab.indicators[indicator].blink_num--;
			}
		}



		for(i=0;i<8;i++)
		{
			if((tab.indicators[indicator].blink_mask>>i)&0x1)
			{
				tab.buses[tab.indicators[indicator].bus].bus_buf[tab.indicators[indicator].number_in_bus][5+i]=0x100*(i+1);
			}
		}

	}
	return;
}

void Indicator_Blink_Set(uint8_t indicator, uint8_t mask, uint8_t blink_num)
{
	tab.indicators[indicator].blink_mask=mask;
	tab.indicators[indicator].blink_num=blink_num;
}


