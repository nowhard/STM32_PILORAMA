#include "indicator.h"
#include "tablo.h"
#include "tablo_parser.h"
extern struct tablo tab;
uint8_t indicators_init(void)//
{
	uint8_t error=0;


	return error;
}

void Indicator_Blink_Handler(uint8_t bus)
{
	uint8_t i=0;
	static uint8_t counter;//2Hz blink
	static uint8_t blink_state;
	counter++;
	if(counter>=10)
	{
		blink_state^=1;
		counter=0;
	}



	for(i=0;i<IND_ALL_NUM;i++)
	{
		if((tab.indicators[i].bus==bus)&&(tab.indicators[i].type==IND_TYPE_LINE))
		{
			if(tab.indicators[i].blink==BLINK_TRUE)
			{
				if((i==18)||(i==19))//мигаем всей круговой секцией
				{
					tab.indicators[18].blink=BLINK_TRUE;
					tab.indicators[19].blink=BLINK_TRUE;
				}
				ln_redraw(&tab.indicators[i],blink_state);
			}
			else
			{
				ln_redraw(&tab.indicators[i],0);
			}
		}
	}
}
