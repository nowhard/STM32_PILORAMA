#include "indicator.h"
#include "tablo.h"
#include "tablo_parser.h"
extern struct tablo tab;
static uint8_t blink_flag=0;

#define BLINK_TIME_PERIOD 6

void Indicator_Blink_Handler(void)
{
	uint8_t indicator=0;
	uint8_t i=0;

	blink_flag++;

	if(blink_flag<(BLINK_TIME_PERIOD>>1))
	{
		for(indicator=0;indicator<IND_ALL_NUM;indicator++)
		{
			tab.indicators[indicator].blink_mask_flags=0x0;
		}
		return;
	}

	if(blink_flag>=BLINK_TIME_PERIOD)
	{
		blink_flag=0;
	}

	if(blink_flag==(BLINK_TIME_PERIOD>>1))
	{
		for(indicator=0;indicator<IND_ALL_NUM;indicator++)
		{
			if((tab.indicators[indicator].blink_mask==BLINK_FALSE)||(tab.indicators[indicator].blink_num==0))
			{
				tab.indicators[indicator].blink_num=0;
				tab.indicators[indicator].blink_mask_flags=0x0;
				//continue;
			}
			else
			{
				if(tab.indicators[indicator].blink_num<0xFF)//finite blink
				{
					tab.indicators[indicator].blink_num--;
				}
				tab.indicators[indicator].blink_mask_flags=tab.indicators[indicator].blink_mask;
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


