#include "channels.h"
struct Channel  channels[CHANNEL_NUMBER];
//-----------------------------------
void ChannelsInit(void) //
{
		channels[0].number=0;		  //
		channels[0].settings.set.type=CHNL_MEMORY;
		channels[0].settings.set.modific=0xF;
		channels[0].settings.set.state_byte_1=0x40;
		channels[0].settings.set.state_byte_2=0x06;
		channels[0].channel_data=0;
		channels[0].channel_data_calibrate=11;
		channels[0].calibrate.cal.calibrate=0;

		channels[1].number=1;		  //
		channels[1].settings.set.type=CHNL_DEV_STATE;
		channels[1].settings.set.modific=CHNL_DEV_STATE_SET;
		channels[1].settings.set.state_byte_1=0x40;
		channels[1].settings.set.state_byte_2=0x06;
		channels[1].channel_data=0;
		channels[1].channel_data_calibrate=11;
		channels[1].calibrate.cal.calibrate=0;
	return;
}

