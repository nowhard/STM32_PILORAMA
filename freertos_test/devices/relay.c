#include "relay.h"
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
void Relay_Init(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;

    /* Configure port -------------------------------*/
    GPIO_InitStructure.GPIO_Pin   = RELAY_SIGN2|RELAY_SIGN3|RELAY_SIGN4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(RELAY_PORT, &GPIO_InitStructure);


    GPIO_WriteBit(RELAY_PORT, RELAY_SIGN2,0);
    GPIO_WriteBit(RELAY_PORT, RELAY_SIGN3,0);
    GPIO_WriteBit(RELAY_PORT, RELAY_SIGN4,0);
}

void Relay_Set_State(uint8_t state)
{
	if((state&0x1)==0)
	{
		GPIO_WriteBit(RELAY_PORT, RELAY_SIGN2,0);
	}
	else
	{
		GPIO_WriteBit(RELAY_PORT, RELAY_SIGN2,1);
	}

	if((state&0x2)==0)
	{
		GPIO_WriteBit(RELAY_PORT, RELAY_SIGN3,0);
	}
	else
	{
		GPIO_WriteBit(RELAY_PORT, RELAY_SIGN3,1);
	}

	if((state&0x4)==0)
	{
		GPIO_WriteBit(RELAY_PORT, RELAY_SIGN4,0);
	}
	else
	{
		GPIO_WriteBit(RELAY_PORT, RELAY_SIGN4,1);
	}
}
