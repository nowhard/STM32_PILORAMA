#ifndef PROTO_H
#define PROTO_H
#include "stm32f4xx.h"
enum
{
	PROTO_FIRST_INIT=0,
	PROTO_REINIT
};
void Proto_Init(uint8_t init_type);//


#define USART_RS485 USART3
#define GPIO_AF_USART_RS485 GPIO_AF_USART3
#define USART_RS485_IRQn	USART3_IRQn
#define RCC_USART_RS485 RCC_APB1Periph_USART3
#define USART_RS485_IRQHandler  USART3_IRQHandler

#define RCC_USART_RS485_GPIO 	RCC_AHB1Periph_GPIOB

#define USART_RS485_GPIO 	GPIOB

#define USART_RS485_TXD	GPIO_Pin_10
#define USART_RS485_RXD	GPIO_Pin_11

#define USART_RS485_DE	GPIO_Pin_1
#define USART_RS485_RE	GPIO_Pin_2

#define CHANNEL_NUMBER	1

#define DEVICE_NAME_LENGTH_SYM	20//
#define DEVICE_VER_LENGTH_SYM 0x8
#define DEVICE_DESC_MAX_LENGTH_SYM 40

//--------------------------------------------------------------------------------
#define MAX_LENGTH_REC_BUF 	256 //максимальная длина буфера приема
#define MIN_LENGTH_REC_BUF	5 //

#define MAX_LENGTH_TR_BUF  	256 //макс. длина буфера передачи
#define CRC_LEN				1 //длина CRC
//-------------------------��� ��������-------------------------------------------
#define  GET_DEV_INFO_REQ 				0x1 //�������� ���������� �� ����������	(��� �������)
#define  GET_DEV_INFO_RESP				0x2	//�������� ���������� �� ����������	(��� ������)

#define  NODE_FULL_INIT_REQ				0x3	//������ ������������� ����
#define  NODE_FULL_INIT_RESP			//������ ������������� ����-��� ������

#define  CHANNEL_LIST_INIT_REQ			0x5 //������������� ������ ������� ���� (��� ������ ������);
#define  CHANNEL_LIST_INIT_RESP			//������������� ������ ������� ���� (��� ������ ������); ��� ������

#define  CHANNEL_GET_DATA_REQ			0x7// ������ ������ �� �������, �������� ���������� ���������;
#define  CHANNEL_GET_DATA_RESP			0x8// ������ ������ �� �������, �������� ���������� ���������;

#define  CHANNEL_SET_PARAMETERS_REQ  	0x9	//���������� ��������� �� �������, �������� ���������� ���������;
#define  CHANNEL_SET_PARAMETERS_RESP  	//���������� ��������� �� �������, �������� ���������� ���������; ��� ������

#define  CHANNEL_SET_ORDER_QUERY_REQ   	0xA//������ ������������������ ������;
#define  CHANNEL_SET_ORDER_QUERY_RESP 	//������ ������������������ ������; ��� ������

#define  CHANNEL_GET_DATA_ORDER_REQ		0xC	//������ ������ �� �������, �������� ������������������ ������;
#define  CHANNEL_GET_DATA_ORDER_RESP	0xD	//������ ������ �� �������, �������� ������������������ ������;

#define  CHANNEL_SET_STATE_REQ		   	0xE//���������� ��������� �� �������, �������� ���������� ���������;
#define  CHANNEL_SET_STATE_RESP			   //���������� ��������� �� �������, �������� ���������� ���������; ��� ������

#define CHANNEL_GET_DATA_ORDER_M2_REQ	0x10 //������ ������ �� �������, �������� ������������������ ������;
#define CHANNEL_GET_DATA_ORDER_M2_RESP	0x11//������ ������ �� �������, �������� ������������������ ������;

#define CHANNEL_SET_RESET_STATE_FLAGS_REQ  0x12//���������/����� ������ ���������
#define CHANNEL_SET_RESET_STATE_FLAGS_RESP //���������/����� ������ ��������� -��� ������

#define  CHANNEL_ALL_GET_DATA_REQ		0x14 //������ ���������� �� ���� ������� ���� (����������� �����);
#define  CHANNEL_ALL_GET_DATA_RESP		0x15 //������ ���������� �� ���� ������� ���� (����������� �����);

#define  CHANNEL_SET_ADDRESS_DESC		0xCD //���������� ����� ����� ����������, ���, ��������, ������ �������� � �����������

#define  CHANNEL_SET_CALIBRATE			0xCA//���������� ������� ��� ������ ����� ������������ ����������

#define  REQUEST_ERROR					0xFF//��������� ������/�����;

//-------------------------���� ������� ��������-------------------------------------------
#define	FR_SUCCESFUL 								0x0//��� ������ (������������ ��� �������������)
#define	FR_UNATTENDED_CHANNEL 						0x1//� ������� ����� �����, �� ������������� ������������� �������;
#define	FR_SET_UNATTENDED_CHANNEL					0x2//� �������� ������������������ ������ ����� �����, �� ������������� ��-����������� ������� (�������� � ����� �� �������� ������� 0Ah);
#define	FR_ORDER_NOT_SET							0x3//������������������ ������ �� ������ (�������� � ����� �� �������� ������� 0Ch);
#define	FR_CHNL_NOT_IN_ORDER_REQ					0x4//� ������� ������������ �����, �� �������� � ������������������ ������ (�������� � ����� �� �������� ������� 0Ch);
#define	FR_FALSE_TYPE_CHNL_DATA						0x5//���������� ���������� �� ������ � ������� ���� ������ ���������� �� ����� (�������� � ����� �� �������� ������� 09h ��� 0Eh);
#define	FR_COMMAND_NOT_EXIST						0x6//�������������� �������;
#define	FR_CHNL_TYPE_NOT_EXIST						0x7//�������������� ���/����������� ������;
#define	FR_COMMAND_NOT_SUPPORT						0x8//������� �� ��������������;
#define	FR_COMMAND_STRUCT_ERROR						0x9//������ � ��������� �������;
#define	FR_FRAME_LENGTH_ERROR						0xA//�������������� ����� ����� � ���������� �������;
#define	FR_CHANNEL_NUM_TOO_MUCH						0xC//������� ����� ������� � �����.
#define	FR_CHNL_TYPE_ERROR							0xD//�������������� ���� ������
#define	FR_CHNL_NOT_EXIST							0xF//������������� �����
//--------------------------------------------------------------------

//--------------------------------------------------------------------
#define PROTO_TYPE_OLD	0x0
#define PROTO_TYPE_NEW	0x1
//--------------------------------------------------------------------
#define PROTO_STANDBY_TIME	10000 //
//--------------------------------------------------------------------

//uint8_t Send_Info(void);
//uint8_t Node_Full_Init(void);
//uint8_t Channel_List_Init(void);
//uint8_t Channel_Get_Data(void);
//uint8_t Channel_Set_Parameters(void);
//uint8_t Channel_Set_Order_Query(void);
//uint8_t Channel_Get_Data_Order(void);
//uint8_t Channel_Set_State(void);
//uint8_t Channel_Get_Data_Order_M2(void);
//uint8_t Channel_Set_Reset_State_Flags(void);
//uint8_t Channel_All_Get_Data(void);
//uint8_t Channel_Set_Address_Desc(void);
//uint8_t Channel_Set_Calibrate(void);
//uint8_t Request_Error(uint8_t error_code);//
//
//
//void ProtoBufHandling(void);
//void Store_Dev_Address_Desc(void);
//void Restore_Dev_Address_Desc(void);
//
//uint8_t  CRC_Check( uint8_t *Spool,uint8_t Count);
 //------------------------------------------------------------------------------
#endif
