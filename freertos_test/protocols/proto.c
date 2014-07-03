#include "proto.h"
#include "channels.h"
#include "tablo.h"
#include "tablo_parser.h"
#include "watchdog.h"
#include "buzzer.h"

 extern struct Channel  channels[];
 extern struct tablo tab;

//-----------------------------------------------------------------------------------
uint8_t   DEV_NAME[DEVICE_NAME_LENGTH_SYM] ="<<TABLO>>";
uint8_t   NOTICE[DEVICE_DESC_MAX_LENGTH_SYM]="<-- GEOSPHERA_2013 -->";
uint8_t   VERSION[DEVICE_VER_LENGTH_SYM] ="\x30\x30\x30\x30\x31";

uint8_t   ADRESS_DEV=0xF;

uint8_t   dev_desc_len=20;
//--------------------------------global variable------------------------------------

uint8_t   	recieve_count;
uint8_t  	transf_count;
uint8_t  	buf_len;

uint8_t   crc_n_ERR;
uint8_t   COMMAND_ERR;

uint8_t   CUT_OUT_NULL;
uint8_t   PROTO_HAS_START;//была стартовая последовательность D7 29
uint8_t   frame_len=0;
//--------------------------------------------------------------------

uint8_t    *RecieveBuf;
uint8_t    *TransferBuf;

//--------------------------------------------------------------------
uint8_t    STATE_BYTE=0xC0;

uint16_t fr_err=0;

uint8_t  symbol=0xFF;

uint8_t	proto_type=PROTO_TYPE_NEW;

const uint8_t standby_frame[256]={0x3A,0x7C,0x5B,0x00,0x2E,0x2E,0x2E,0x5D,0x5B,0x01,0x2E,0x2E,0x2E,0x5D,0x5B,0x02,0x2E,0x2E,0x2E,0x5D,0x5B,0x03,0x2E,0x2E,0x2E,0x5D,0x5B,0x04,0x2E,0x2E,0x2E,0x5D,0x5B,0x05,0x2E,0x2E,0x2E,0x5D,0x5B,0x06,0x2E,0x2E,0x2E,0x5D,0x5B,0x07,0x2E,0x2E,0x2E,0x5D,0x5B,0x08,0x2E,0x2E,0x2E,0x2E,0x2E,0x5D,0x5B,0x0A,0x2E,0x2E,0x2E,0x2E,0x2E,0x5D,0x5B,0x0B,0x2E,0x2E,0x2E,0x5D,0x5B,0x0C,0x2E,0x2E,0x2E,0x2E,0x2E,0x5D,0x5B,0x0D,0x00,0x00,0x00,0xFF,0x5D,0x5B,0x0E,0x00,0x00,0x00,0xFF,0x5D,0x5B,0x0F,0x00,0x00,0x00,0xFF,0x5D,0x5B,0x10,0x00,0x00,0x00,0xFF,0x5D,0x5B,0x12,0x00,0x00,0x00,0xFF,0x5D,0x5B,0x13,0x00,0xFF,0xFF,0xFF,0x5D,0x5B,0x2A,0x07,0x5D};
const uint8_t standby_frame_2[256]={0x3A,0x7C,0x5B,0x00,' ',' ',' ',0x5D,0x5B,0x01,' ',' ',' ',0x5D,0x5B,0x02,' ',' ',' ',0x5D,0x5B,0x03,' ',' ',' ',0x5D,0x5B,0x04,' ',' ',' ',0x5D,0x5B,0x05,' ',' ',' ',0x5D,0x5B,0x06,' ',' ',' ',0x5D,0x5B,0x07,' ',' ',' ',0x5D,0x5B,0x08,' ',' ',' ',' ',' ',0x5D,0x5B,0x0A,' ',' ',' ',' ',' ',0x5D,0x5B,0x0B,' ',' ',' ',0x5D,0x5B,0x0C,' ',' ',' ',' ',' ',0x5D,0x5B,0x0D,0x00,0xFF,0xFF,0xFF,0x5D,0x5B,0x0E,0x00,0xFF,0xFF,0xFF,0x5D,0x5B,0x0F,0x00,0xFF,0xFF,0xFF,0x5D,0x5B,0x10,0x00,0xFF,0xFF,0xFF,0x5D,0x5B,0x12,0x00,0xFF,0xFF,0xFF,0x5D,0x5B,0x13,0x00,0xFF,0xFF,0xFF,0x5D,0x5B,0x2A,0x07,0x5D};
//-----------------------------------------------------------------------------------
union
{
	float result_float;
	uint8_t result_char[4];
}
sym_8_to_float;

xSemaphoreHandle xProtoSemaphore;
extern struct task_watch task_watches[];

#define RS_485_RECEIVE  GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_RESET); GPIO_WriteBit(GPIOA, GPIO_Pin_11, Bit_RESET);
#define RS_485_TRANSMIT GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_SET); GPIO_WriteBit(GPIOA, GPIO_Pin_11, Bit_SET);

void USART1_IRQHandler (void)
{
 	static portBASE_TYPE xHigherPriorityTaskWoken;
 	xHigherPriorityTaskWoken = pdFALSE;


 	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
   	{
 		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
   		symbol=USART_ReceiveData (USART1);

   		if(recieve_count>MAX_LENGTH_REC_BUF)
   		{
   			recieve_count=0x0;
   			return;
   		}

		switch(proto_type)
		{
			case PROTO_TYPE_OLD:
			{
				if(symbol==':')
				{
					recieve_count=0x0;
				}

				tab.tablo_proto_buf[recieve_count]=symbol;
				recieve_count++;

				if(recieve_count>1)
				{
					if(tab.tablo_proto_buf[1]==(recieve_count-2))//
					{
						 USART_ITConfig(USART1, USART_IT_RXNE , DISABLE);
						xSemaphoreGiveFromISR( xProtoSemaphore, &xHigherPriorityTaskWoken );

						 if( xHigherPriorityTaskWoken != pdFALSE )
						 {
							portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
						 }
					}
				}
			}
			break;

			case PROTO_TYPE_NEW:
			{
			//--------------------------

				switch(symbol)
				{
					case (char)(0xD7):
					{
						RecieveBuf[recieve_count]=symbol;
						recieve_count++;
						CUT_OUT_NULL=1;
					}
					break;

					case (char)(0x29):
					{
						if(CUT_OUT_NULL==1)
						{
							RecieveBuf[0]=0x0;
							RecieveBuf[1]=0xD7;
							RecieveBuf[2]=0x29;
							PROTO_HAS_START=1;
							recieve_count=0x3;
						}
						else
						{
							RecieveBuf[recieve_count]=symbol;
							recieve_count++;
						}
						CUT_OUT_NULL=0;
					}
					break;

					case (char)(0x0):
					{
						if(CUT_OUT_NULL==1)	  //если после 0xD7-пропускаем
						{
							CUT_OUT_NULL=0;
						}
						else
						{
							RecieveBuf[recieve_count]=symbol;
							recieve_count++;
						}
					}
					break;

					default :
					{
						if(PROTO_HAS_START)
						{
							RecieveBuf[recieve_count]=symbol;
							recieve_count++;
							CUT_OUT_NULL=0;
						}

					}
				}

			   if(recieve_count>6)
			   {
					  if(recieve_count==6+frame_len)	  // принимаем указанное в frame_len число байт
					  {
							 USART_ITConfig(USART1, USART_IT_RXNE , DISABLE);

							 xSemaphoreGiveFromISR( xProtoSemaphore, &xHigherPriorityTaskWoken );

							 if( xHigherPriorityTaskWoken != pdFALSE )
							 {
								portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
							 }
							 CUT_OUT_NULL=0;
							 PROTO_HAS_START=0;
					  }
			   }
			   else
			   {
					   if(recieve_count==6)
					   {
							frame_len=RecieveBuf[recieve_count-1];  // получаем длину данных после заголовка
					   }
			   }
			}
		}

   	}
   //-----------------------------------------------------------------------------------------------------------------
   	if(USART_GetITStatus(USART1, USART_IT_TC) != RESET)
   	{

   		USART_ClearITPendingBit(USART1, USART_IT_TC);

   		if(transf_count<buf_len)
   		{
   			if(transf_count<3)
   			{
   				//USART_SendData(USART1,TransferBuf[transf_count]);
   				USART1->DR =TransferBuf[transf_count];
   				transf_count++;
   			}
   			else
   			{
   					if(CUT_OUT_NULL==0)
   					{
   						if(TransferBuf[transf_count]==(uint8_t)0xD7)
   						{
   							CUT_OUT_NULL=0x1;
   						}
   						//USART_SendData(USART1,TransferBuf[transf_count]);
   						USART1->DR =TransferBuf[transf_count];
   						transf_count++;
   					}
   					else
   					{
   						//USART_SendData(USART1,(uint8_t)0x0);
   						USART1->DR =(uint8_t)0x0 ;
   						CUT_OUT_NULL=0;
   					}
   			}
   		}
   		else
   		{
   			transf_count=0;
   			recieve_count=0;

   			CUT_OUT_NULL=0;
   			USART_ITConfig(USART1, USART_IT_RXNE , ENABLE);
   			RS_485_RECEIVE;
   		}

   	}
   	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}
//------------------------------------------------------------------------------
void Proto_Init(uint8_t init_type) //
{

	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 , ENABLE);
	 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);


	  GPIO_InitTypeDef GPIO_InitStructure;

	  // Tx on PD5 as alternate function push-pull
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; //
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; //
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_Init(GPIOA, &GPIO_InitStructure);

	  /* Rx on PD6 as input floating */
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; //
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; //
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_Init(GPIOA, &GPIO_InitStructure);

	  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

	  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8|GPIO_Pin_11;
	  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_Init(GPIOA, &GPIO_InitStructure);


	 RS_485_RECEIVE;

	USART_InitTypeDef USART_InitStructure;

	USART_DeInit(USART1);

	USART_InitStructure.USART_BaudRate = 57600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	USART_ClearFlag(USART1, USART_FLAG_CTS | USART_FLAG_LBD  | USART_FLAG_TC  | USART_FLAG_RXNE );

	USART_ITConfig(USART1, USART_IT_TC, ENABLE);
	USART_ITConfig(USART1, USART_IT_RXNE , ENABLE);

	USART_Cmd(USART1, ENABLE);

	  NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );

	  NVIC_InitTypeDef NVIC_InitStructure;


	   /* Enabling interrupt from USART1 */
	   NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 14;
	   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	   NVIC_Init(&NVIC_InitStructure);




	NVIC_EnableIRQ(USART1_IRQn);

	//------------------------

	crc_n_ERR=0x0;	//
	COMMAND_ERR=0x0;//


	TransferBuf=&tab.uart_buf;
	RecieveBuf= &tab.uart_buf;
	ChannelsInit();

	recieve_count=0x0;//
	transf_count=0x0;//
	buf_len=0x0;//

	CUT_OUT_NULL=0;
	PROTO_HAS_START=0;

	if(init_type==PROTO_FIRST_INIT)
	{
		xTaskCreate(ProtoProcess,(signed char*)"PROTO",128,NULL, tskIDLE_PRIORITY + 1, NULL);
		task_watches[PROTO_TASK].task_status=TASK_ACTIVE;
		vSemaphoreCreateBinary( xProtoSemaphore );
	}
	return;
}
//-----------------------------------------------------------------------------
uint8_t Send_Info(void) //
{
		uint8_t    i=0;

	   //
	   TransferBuf[0]=0x00;
	   TransferBuf[1]=0xD7;
	   TransferBuf[2]=0x29;
	   //------------------
	   TransferBuf[3]=ADRESS_DEV;  //
	   TransferBuf[4]=GET_DEV_INFO_RESP;  //
	   TransferBuf[6]=STATE_BYTE;

	   for(i=0;i<20;i++)
	   {				  //
			   if(i<DEVICE_NAME_LENGTH_SYM)
			   {
			     	TransferBuf[i+7]=DEV_NAME[i];
			   }
			   else
			   {
			   		TransferBuf[i+7]=0x00;
			   }
		}

	   for(i=0;i<5;i++)                   //
	   {
	       if(i<DEVICE_VER_LENGTH_SYM)
		   {
		    	 TransferBuf[i+27]=VERSION[i];
		   }
	   }

	   TransferBuf[32]=CHANNEL_NUMBER;

	   for(i=0;i<CHANNEL_NUMBER;i++)
       {
		  	TransferBuf[i*2+33]=((channels[i].settings.set.type)<<4)|channels[i].settings.set.modific; //
		  	TransferBuf[i*2+33+1]=0x00;
	   }
	   for(i=0;i<dev_desc_len;i++)
	   {
			 TransferBuf[i+33+CHANNEL_NUMBER*2]=NOTICE[i];
	   }

	   TransferBuf[5]=28+CHANNEL_NUMBER*2+dev_desc_len;
	   TransferBuf[33+CHANNEL_NUMBER*2+dev_desc_len]=CRC_Check(&TransferBuf[1],32+CHANNEL_NUMBER*2+dev_desc_len); //

	return (34+CHANNEL_NUMBER*2+dev_desc_len);
}
//-----------------------------------------------------------------------------
uint8_t Node_Full_Init(void) //
{
	return 0;
}
//-----------------------------------------------------------------------------
uint8_t Channel_List_Init(void) //
{
	return 0;
}
//-----------------------------------------------------------------------------
uint8_t Channel_Get_Data(void) //
{
	return 0;
}
//-----------------------------------------------------------------------------
uint8_t  Channel_Set_Parameters(void) //
{
	 uint8_t   index=0, store_data=0;
	 uint8_t len=0,i=0;

	   while(index<(RecieveBuf[5]-1))				   //
	      {
			  	if(RecieveBuf[6+index]<CHANNEL_NUMBER)
			    {
					switch((RecieveBuf[6+index+1]>>4)&0xF)
					{
//					 		case CHNL_ADC://Р В Р’В Р РЋРІР‚в„ўР В Р’В Р вЂ™Р’В¦Р В Р’В Р РЋРЎСџ
//							{
//								if((channels[RecieveBuf[6+index]].settings.set.modific!=RecieveBuf[6+index+1])||(channels[RecieveBuf[6+index]].settings.set.state_byte_1!=RecieveBuf[6+index+2]) || (channels[RecieveBuf[6+index]].settings.set.state_byte_2!=RecieveBuf[6+index+3]))
//								{
//									channels[RecieveBuf[6+index]].settings.set.state_byte_1=RecieveBuf[6+index+2];
//									channels[RecieveBuf[6+index]].settings.set.state_byte_2=RecieveBuf[6+index+3];
//									channels[RecieveBuf[6+index]].settings.set.modific	   =RecieveBuf[6+index+1]&0xF;
//									//debug----------------
//									//if(channels[RecieveBuf[6+index]].settings.set.modific==3)
////									{
////										GPIOC->ODR |= GPIO_Pin_8;
////									}
//									//-----------------
//									store_data=1;
//
//								}
//								index=index+1;
//							}
//							break;

//							case 0x2://Р В Р Р‹Р Р†Р вЂљР Р‹Р В Р’В Р вЂ™Р’В°Р В Р Р‹Р В РЎвЂњР В Р Р‹Р Р†Р вЂљРЎв„ўР В Р’В Р РЋРІР‚СћР В Р Р‹Р Р†Р вЂљРЎв„ўР В Р’В Р РЋРІР‚СћР В Р’В Р РЋРїС—Р…Р В Р’В Р вЂ™Р’ВµР В Р Р‹Р В РІР‚С™
//							{
//							/*   if(channels[RecieveBuf[6+index]].settings.set.state_byte_1!=RecieveBuf[6+index+2])
//							   {
//							   		channels[RecieveBuf[6+index]].settings.set.state_byte_1=RecieveBuf[6+index+2];
//									store_data=1;
//							   }*/
//							}
//							break;

							case CHNL_DEV_STATE://
							{
								switch((RecieveBuf[6+index+1])&0xF)
								{
									case CHNL_DEV_STATE_GET:
									{

									}
									break;

									case CHNL_DEV_STATE_SET:
									{
//										tab.buz.buzzer_effect=(RecieveBuf[8+index]>>1)&0x7;
//										tab.buz.buzzer_enable=RecieveBuf[8+index]&0x1;
										buzzer_set_buzz((RecieveBuf[8+index]>>1)&0x7,RecieveBuf[8+index]&0x1);
										Relay_Set_State((RecieveBuf[8+index]>>4)&0xF);
										index+=CHNL_DEV_STATE_SET_LEN;
									}
									break;

									case CHNL_DEV_STATE_GET_SET:
									{

									}
									break;
								}
							}
							break;

							case CHNL_MEMORY://
							{
								for(i=0;i<RecieveBuf[8+index];i++)
								{
									tab.tablo_proto_buf[i]=RecieveBuf[8+index+i+1];
								}

								tablo_proto_parser(&tab.tablo_proto_buf);
								index+=RecieveBuf[6+2+index]+3;
							}
					}
				}
				else
				{
					return Request_Error(FR_UNATTENDED_CHANNEL);
				}
		  }
	   if(store_data)
	   {
	   		//Store_Channels_Data();	//
		}
	   return Request_Error(FR_SUCCESFUL);
}
//-----------------------------------------------------------------------------
uint8_t Channel_Set_Order_Query(void) //
{
	return 0;
}
//-----------------------------------------------------------------------------
uint8_t Channel_Get_Data_Order(void) //
{
	return 0;
}
//-----------------------------------------------------------------------------
uint8_t Channel_Set_State(void) //
{
	return 0;
}
//-----------------------------------------------------------------------------
uint8_t  Channel_Get_Data_Order_M2(void) //
{
	return 0;
}
//-----------------------------------------------------------------------------
uint8_t Channel_Set_Reset_State_Flags(void) //
{
	STATE_BYTE=0x40;
	return	Request_Error(FR_SUCCESFUL);//
}
//-----------------------------------------------------------------------------
uint8_t Channel_All_Get_Data(void) //
{
	   uint8_t  index=0,i=0;


	   TransferBuf[0]=0x00;TransferBuf[1]=0xD7;TransferBuf[2]=0x29;
	   TransferBuf[3]=ADRESS_DEV;  //
	   TransferBuf[4]=CHANNEL_ALL_GET_DATA_RESP;  //
	   TransferBuf[6]=STATE_BYTE;
	    for(i=0;i<CHANNEL_NUMBER;i++)				   //
	    {
			  TransferBuf[index+7]=i;
			  index++;
			  TransferBuf[index+7]=((channels[i].settings.set.type)<<4)|channels[i].settings.set.modific; //
			  index++;
			  switch(channels[i].settings.set.type)
			    {
					 case 0:  //
					 {
						 switch(channels[i].settings.set.modific)
		                 {
							  case 0:
							  {
							  		if(channels[i].calibrate.cal.calibrate)//
									{
									//	 if(channels[i].settings.set.modific==0x00 || channels[i].settings.set.modific==0x01)
									//	 {
										 	TransferBuf[index+7]=((channels[i].channel_data_calibrate)&0x0000FF00)>>8;
										  	index++;
				    					  	TransferBuf[index+7]=((channels[i].channel_data_calibrate)&0x00FF0000)>>16;
										  	index++;
									//	 }
									}
									else
									{
										// if(channels[i].settings.set.modific==0x00 || channels[i].settings.set.modific==0x01)		   //
									//	 {
											TransferBuf[index+7]=((channels[i].channel_data)&0x0000FF00)>>8;
										  	index++;
				    					  	TransferBuf[index+7]=((channels[i].channel_data)&0x00FF0000)>>16;
										  	index++;
										// }
									}


									  TransferBuf[index+7]=channels[i].settings.set.state_byte_1;	 //
			                          index++;
			                          TransferBuf[index+7]=channels[i].settings.set.state_byte_2;	 //
				                      index++;
							  }
							  break;

							  case 1:
							  {
							  }
							  break;

				        	  case 2:
							  {
							  }
							  break;

							  case 3:
							  {
							        if(channels[i].calibrate.cal.calibrate)//
									{
							 			// if(channels[i].settings.set.modific==0x02 || channels[i].settings.set.modific==0x03)		   //
									//	 {
										  	TransferBuf[index+7]=((channels[i].channel_data_calibrate)&0x000000FF); //
								          	index++;
										  	TransferBuf[index+7]=((channels[i].channel_data_calibrate)&0x0000FF00)>>8;
										  	index++;
				    					  	TransferBuf[index+7]=((channels[i].channel_data_calibrate)&0x00FF0000)>>16;
										  	index++;
			  						  	// }
									}
									else
									{
									//	 if(channels[i].settings.set.modific==0x02 || channels[i].settings.set.modific==0x03)		   //
										// {
										  	TransferBuf[index+7]=((channels[i].channel_data)&0x000000FF); //
								          	index++;
										  	TransferBuf[index+7]=((channels[i].channel_data)&0x0000FF00)>>8;
										  	index++;
				    					  	TransferBuf[index+7]=((channels[i].channel_data)&0x00FF0000)>>16;
										  	index++;
									//	 }
									}


									  TransferBuf[index+7]=channels[i].settings.set.state_byte_1;	 //
			                          index++;
			                          TransferBuf[index+7]=channels[i].settings.set.state_byte_2;	 //
				                      index++;
							  }
							  break;
						  }
					  }
					  break;

				 	case 1:	 //
					{
						  switch(channels[i].settings.set.modific)
					      {
								  case 0:
								  {
								          TransferBuf[index+7]=((channels[i].channel_data)&0x000000FF); //
								          index++;

										  TransferBuf[index+7]=((channels[i].channel_data)&0x0000FF00)>>8;
										  index++;

								          TransferBuf[index+7]=((channels[i].channel_data)&0x00FF0000)>>16; //
								          index++;

										  TransferBuf[index+7]=((channels[i].channel_data)&0xFF000000)>>24;
										  index++;


										  TransferBuf[index+7]=channels[i].settings.set.state_byte_1;	 //
				                          index++;
								  }
								  break;
						   }
					}
					break;

					 case 2: //
					 {
						  switch(channels[i].settings.set.modific)
					      {
								  case 1:
								  {
								          TransferBuf[index+7]=((channels[i].channel_data)&0x000000FF); //
								          index++;
										  TransferBuf[index+7]=((channels[i].channel_data)&0x0000FF00)>>8;
										  index++;
										  TransferBuf[index+7]=channels[i].settings.set.state_byte_1;	 //
				                          index++;
								  }
								  break;
						   }
					  }
					  break;
			  }
		   }

		  TransferBuf[5]=index+2; 						 //
		  TransferBuf[index+7]=CRC_Check(&TransferBuf[1],(uint8_t)(index+7)-1); //
		  return (uint8_t)(7+index+1);
}

uint8_t Request_Error(uint8_t error_code) //
{
	TransferBuf[0]=0x00;TransferBuf[1]=0xD7;TransferBuf[2]=0x29;
    TransferBuf[3]=ADRESS_DEV;  //
    TransferBuf[7]=RecieveBuf[4]; //
    TransferBuf[4]=0xFF;  //

	TransferBuf[6]=STATE_BYTE; //
    TransferBuf[8]=error_code;
    TransferBuf[5]=0x04;	  //
    TransferBuf[9]=CRC_Check(TransferBuf,9);
	return 10;
}
//-----------------------------------------------------------------------------
void ProtoBufHandling(void) //
{
  switch(RecieveBuf[4])
  {
//---------------------------------------
  	case GET_DEV_INFO_REQ:
	{
		buf_len=Send_Info();
	}
	break;
//---------------------------------------
  	case NODE_FULL_INIT_REQ:
	{
		buf_len=Node_Full_Init();
	}
	break;
//---------------------------------------
  	case CHANNEL_LIST_INIT_REQ:
	{
		buf_len=Channel_List_Init();
	}
	break;
//---------------------------------------
	case CHANNEL_GET_DATA_REQ:
	{
		buf_len=Channel_Get_Data();
	}
	break;
	//-----------------------------------
	case CHANNEL_SET_PARAMETERS_REQ:
	{
		buf_len=Channel_Set_Parameters();
	}
	break;
	//-----------------------------------
	case CHANNEL_SET_ORDER_QUERY_REQ:
	{
		buf_len=Channel_Set_Order_Query();
	}
	break;
//----------------------------------------
	case CHANNEL_GET_DATA_ORDER_REQ:
	{
		 buf_len=Channel_Get_Data_Order();
	}
	break;
//----------------------------------------
	case CHANNEL_SET_STATE_REQ:
	{
		 buf_len=Channel_Set_State();
	}
	break;
//----------------------------------------
	case CHANNEL_GET_DATA_ORDER_M2_REQ:
	{
		 buf_len=Channel_Get_Data_Order_M2();
	}
	break;
//------------------------------------------
	case CHANNEL_SET_RESET_STATE_FLAGS_REQ:
	{
		buf_len=Channel_Set_Reset_State_Flags();
	}
	break;
//------------------------------------------
	case CHANNEL_ALL_GET_DATA_REQ:
	{
		 buf_len=Channel_All_Get_Data();
	}
	break;
//------------------------------------------
/*	case CHANNEL_SET_ADDRESS_DESC:
	{
		 buf_len=Channel_Set_Address_Desc();
	}
	break;
//------------------------------------------
	case CHANNEL_SET_CALIBRATE:
	{
		 buf_len=Channel_Set_Calibrate();
	}
	break;*/
//------------------------------------------
    default:
	{
       COMMAND_ERR=0x1;//
	   buf_len=Request_Error(FR_COMMAND_NOT_EXIST);
    }
  }

  return;
}
//-----------------------------------------------------------------------------------
void ProtoProcess( void *pvParameters )
{
	uint8_t   crc_n;
	task_watches[PROTO_TASK].task_status=TASK_IDLE;
	while(1)
	{
		task_watches[PROTO_TASK].task_status=TASK_IDLE;
		if( xProtoSemaphore != NULL )
		{

			if( xSemaphoreTake( xProtoSemaphore, ( portTickType ) PROTO_STANDBY_TIME ) == pdTRUE )
			{
				task_watches[PROTO_TASK].task_status=TASK_ACTIVE;
				switch(proto_type)
				{
					case PROTO_TYPE_OLD:
					{
						tablo_proto_parser(&tab.tablo_proto_buf);
						recieve_count=0x0;
						USART_ITConfig(USART1, USART_IT_RXNE , ENABLE);
					}
					break;

					case PROTO_TYPE_NEW:
					{
						crc_n=RecieveBuf[recieve_count-1];
						transf_count=0;
						if((CRC_Check(RecieveBuf,(recieve_count-CRC_LEN))==crc_n)&& (RecieveBuf[3]==ADRESS_DEV))
						{

							ProtoBufHandling();//

							transf_count=0;
							recieve_count=0;
							CUT_OUT_NULL=0;

							//USART_SendData(USART1,/*TransferBuf[transf_count]*/tab.uart_buf[transf_count]);
							RS_485_TRANSMIT;
							USART1->DR =TransferBuf[transf_count];
							transf_count++;//
						}
						else
						{
							crc_n_ERR=0x1;//
							USART_ITConfig(USART1, USART_IT_RXNE , ENABLE);
						}
					}
					break;
				}
				task_watches[PROTO_TASK].counter++;
			}
			else
			{
				//task_watches[PROTO_TASK].task_status=TASK_IDLE;
				tablo_proto_parser(&standby_frame_2);//
				vTaskDelay(200);
				tablo_proto_parser(&standby_frame);//
				//PROTO_HAS_START=0;
				 Proto_Init(PROTO_REINIT);
			}
		}
		task_watches[PROTO_TASK].task_status=TASK_IDLE;
	}
}
//-----------------------crc_n------------------------------------------------------------
uint8_t  CRC_Check( uint8_t  *Spool_pr,uint8_t Count_pr )
{
	uint8_t crc_n = 0;
	uint8_t  *Spool;
	uint8_t  Count ;

	Spool=Spool_pr;
	Count=Count_pr;

  		while(Count!=0x0)
        {
	        crc_n = crc_n ^ (*Spool++);//

	        crc_n = ((crc_n & 0x01) ? (uint8_t)0x80: (uint8_t)0x00) | (uint8_t)(crc_n >> 1);

	        if (crc_n & (uint8_t)0x80) crc_n = crc_n ^ (uint8_t)0x3C;
			Count--;
        }
    return crc_n;
}
 //-----------------------------------------------------------------------------------------------

