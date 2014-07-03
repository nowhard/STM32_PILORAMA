#ifndef CHANNELS_H
#define CHANNELS_H
#include <stm32f4xx.h>
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_usart.h"

//#include "preferences.h"
#include "misc.h"
//---------------------------------------
#define CHANNEL_NUMBER	2
//---------------------------------------
#define CHNL_ADC	0//
	#define CHNL_ADC_FIX_16	0
	#define CHNL_ADC_FIX_16_LEN	6

	#define CHNL_ADC_SW_16	1
	#define CHNL_ADC_SW_16_LEN	6

	#define CHNL_ADC_FIX_24	3
	#define CHNL_ADC_FIX_24_LEN	7

	#define CHNL_ADC_SW_24	2
	#define CHNL_ADC_SW_24_LEN	7

#define CHNL_DOL	1 //
	#define CHNL_DOL_ENC	0
	#define CHNL_DOL_ENC_LEN	7

	#define CHNL_DOL_FLOAT	1

#define CHNL_FREQ	2//
	#define	 CHNL_FREQ_COUNT_T	0
	#define	 CHNL_FREQ_COUNT_T_LEN	5

	#define	 CHNL_FREQ_256		1
	#define	 CHNL_FREQ_256_LEN		5

	#define  CHNL_FREQ_PERIOD	3
	#define  CHNL_FREQ_PERIOD_LEN	5

	#define	 CHNL_FREQ_COUNT	4
	#define	 CHNL_FREQ_COUNT_LEN	5

#define CHNL_DEV_STATE		4
	#define	 CHNL_DEV_STATE_GET		0
	#define	 CHNL_DEV_STATE_GET_LEN	3

	#define	 CHNL_DEV_STATE_SET		1
	#define	 CHNL_DEV_STATE_SET_LEN	3

	#define	 CHNL_DEV_STATE_GET_SET	2
	#define	 CHNL_DEV_STATE_GET_SET_LEN	3

#define CHNL_MEMORY		8
	#define CHNL_MEMORY_EEPROM	0
//---------------------------------------
struct Channel
{
	unsigned char number ;

	union
	{
		struct
		{
			unsigned char type;
			unsigned char modific;
			unsigned char state_byte_1;
			unsigned char state_byte_2;
		}set;

		unsigned long serialize;
	} settings;

	unsigned long channel_data;
	unsigned long channel_data_calibrate;

	union
	{
		struct
		{
			unsigned char calibrate;


			float K;//
			float C;//y=kx+c


		} cal;
		unsigned long serialize[3];
	} calibrate;
} ;
//---------------------------------------
void ChannelsInit(void);
//---------------------------------------
#endif
