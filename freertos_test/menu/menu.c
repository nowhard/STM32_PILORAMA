#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"

#include <stdio.h>
#include "keyboard.h"
#include "menu.h"
#include "tablo_parser.h"
#include "backup_sram.h"
#include "drive.h"
#include "buzzer.h"
#include "watchdog.h"

//������� �� FreeRTOS:

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

typedef struct {
	void       *Next;
	void       *Previous;
	void       *Parent;
	void       *Child;
	uint8_t     Select;
	const char  Text[];
} menuItem;

menuItem* selectedMenuItem; // ������� ����� ����

menuItem* menuStack[10];
uint8_t display_buf[20];
volatile unsigned char menuStackTop;
void MenuHandler( void *pvParameters );
void Menu_Handle_Key(menuItem* currentMenuItem,uint8_t current_key);

#define INPUT_BUF_LEN		8
#define INPUT_STRING_MAX	4

#define MENU_ABS_MAX_VAL	 6000
#define MENU_ABS_MIN_VAL		0

#define MENU_ROOT_MAX_VAL	 6000
#define MENU_ROOT_MIN_VAL	-6000

#define MENU_ROOT_ABS_MAX_VAL	 6000
#define MENU_ROOT_ABS_MIN_VAL		1

#define MENU_F01_MAX_VAL	 6000
#define MENU_F01_MIN_VAL	-6000

#define MENU_F02_MAX_VAL	 6000
#define MENU_F02_MIN_VAL	-6000

#define MENU_F03_MAX_VAL	 6000
#define MENU_F03_MIN_VAL		0

#define MENU_F04_MAX_VAL	 6000
#define MENU_F04_MIN_VAL	    0

#define MENU_F05_MAX_VAL	 6000
#define MENU_F05_MIN_VAL		0

#define MENU_F06_MAX_VAL	 6000
#define MENU_F06_MIN_VAL		0

#define MENU_F07_MAX_VAL	 6000
#define MENU_F07_MIN_VAL		0

#define MENU_F08_MAX_VAL	 6000
#define MENU_F08_MIN_VAL	-6000

#define MENU_F09_MAX_VAL	 6000
#define MENU_F09_MIN_VAL	-6000

#define MENU_F10_MAX_VAL	 6000
#define MENU_F10_MIN_VAL	-6000

struct input_buffer
{
	uint8_t buf[INPUT_BUF_LEN];
	uint8_t counter;
	uint8_t sign;
}input_buf, abs_buf;

void 	Menu_Input_Field(uint8_t current_key,uint8_t attributes,struct input_buffer *inp_buf,int16_t min_value, int16_t max_value);
void 	Menu_Input_Field_Clear(struct input_buffer *inp_buf);
uint8_t Menu_Input_Buf_To_Int(struct input_buffer *inp_buf,int16_t *val,int16_t min_value, int16_t max_value);
uint8_t Menu_Input_Int_To_Buf(int16_t val,struct input_buffer *inp_buf,int16_t min_value, int16_t max_value);
uint8_t Menu_Input_Buf_To_Indicator(struct input_buffer *inp_buf,uint8_t indicator);

extern xQueueHandle xKeyQueue;//������� ����������

#define MAKE_MENU(Name, Next, Previous, Parent, Child, Select, Text) \
    extern menuItem Next;     \
	extern menuItem Previous; \
	extern menuItem Parent;   \
	extern menuItem Child;  \
	menuItem Name = {(void*)&Next, (void*)&Previous, (void*)&Parent, (void*)&Child, (uint8_t)Select, { Text }}

#define PREVIOUS   selectedMenuItem->Previous
#define NEXT       selectedMenuItem->Next
#define PARENT     selectedMenuItem->Parent
#define CHILD      selectedMenuItem->Child
#define SELECT	   selectedMenuItem->Select

char strNULL[]  = "";

#define NULL_ENTRY Null_Menu
menuItem        Null_Menu = {(void*)0, (void*)0, (void*)0, (void*)0, 0, {0x00}};

//                 NEXT,      PREVIOUS     PARENT,     CHILD
MAKE_MENU(m_s0i1,  NULL_ENTRY,NULL_ENTRY,  NULL_ENTRY, m_s1i1	 ,  MENU_ROOT,	""    );

MAKE_MENU(m_s1i1,  m_s1i2,    NULL_ENTRY,  m_s0i1, 	   NULL_ENTRY,  MENU_F_01,	"F-01");
MAKE_MENU(m_s1i2,  m_s1i3,	  m_s1i1,      m_s0i1, 	   NULL_ENTRY,  MENU_F_02,	"F-02");
MAKE_MENU(m_s1i3,  m_s1i4,	  m_s1i2,      m_s0i1, 	   NULL_ENTRY,  MENU_F_03,  "F-03");
MAKE_MENU(m_s1i4,  m_s1i5,    m_s1i3,      m_s0i1, 	   NULL_ENTRY,  MENU_F_04,  "F-04");
MAKE_MENU(m_s1i5,  m_s1i6,    m_s1i4,      m_s0i1, 	   NULL_ENTRY,  MENU_F_05,  "F-05");
MAKE_MENU(m_s1i6,  m_s1i7,	  m_s1i5,      m_s0i1, 	   NULL_ENTRY,  MENU_F_06,  "F-06");
MAKE_MENU(m_s1i7,  m_s1i8,	  m_s1i6,      m_s0i1, 	   NULL_ENTRY,  MENU_F_07,  "F-07");
MAKE_MENU(m_s1i8,  m_s1i9,	  m_s1i7,      m_s0i1, 	   NULL_ENTRY,  MENU_F_08,  "F-08");
MAKE_MENU(m_s1i9,  m_s1i10,	  m_s1i8,      m_s0i1, 	   NULL_ENTRY,  MENU_F_09,  "F-09");
MAKE_MENU(m_s1i10, NULL_ENTRY,m_s1i9,      m_s0i1, 	   NULL_ENTRY,  MENU_F_10,  "F-10");

extern struct drive drv;
extern struct task_watch task_watches[];
xQueueHandle xClrIndicatorQueue;//������� ����������

uint8_t const_tickness_counter=0;

void menuChange(menuItem* NewMenu)
{
	if ((void*)NewMenu == (void*)&NULL_ENTRY)
	{
	  return;
	}

	selectedMenuItem = NewMenu;
}

unsigned char dispMenu(void)
{
	menuItem* tempMenu;
	tempMenu=selectedMenuItem->Parent;

	if ((void*)tempMenu == (void*)&NULL_ENTRY)
	{ // �� �� ������� ������
		Menu_Input_Field_Clear(&input_buf);
	}
	else
	{
		str_to_ind(IND_1,(char *)selectedMenuItem->Text);

		switch(selectedMenuItem->Select)
		{
			case MENU_F_01:
			{
				Menu_Input_Field_Clear(&input_buf);
			}
			break;

			case MENU_F_02:
			{
				Menu_Input_Field_Clear(&input_buf);
			}
			break;

			case MENU_F_03:
			{
				Menu_Input_Field_Clear(&input_buf);
			}
			break;

			case MENU_F_04:
			{
				if(Menu_Input_Int_To_Buf(drv.bkp_reg->F_04_function_back,&input_buf,MENU_F04_MIN_VAL,MENU_F04_MAX_VAL)==INPUT_ERR)
				{
					Menu_Input_Field_Clear(&input_buf);
					Indicator_Blink_Set(IND_2,0xFF,2);
					buzzer_set_buzz(BUZZER_EFFECT_3_BEEP,BUZZER_ON,FROM_TASK);
					Indicator_Blink_Set(IND_2,0xFF,2);
					//error
				}
				else
				{
					Menu_Input_Buf_To_Indicator(&input_buf,IND_2);
				}
			}
			break;

			case MENU_F_05:
			{
				if(Menu_Input_Int_To_Buf(drv.bkp_reg->F_05_cal_speed_down,&input_buf,MENU_F05_MIN_VAL,MENU_F05_MAX_VAL)==INPUT_ERR)
				{
					Menu_Input_Field_Clear(&input_buf);
					Indicator_Blink_Set(IND_2,0xFF,2);
					buzzer_set_buzz(BUZZER_EFFECT_3_BEEP,BUZZER_ON,FROM_TASK);
					Indicator_Blink_Set(IND_2,0xFF,2);
					//error
				}
				else
				{
					Menu_Input_Buf_To_Indicator(&input_buf,IND_2);
				}
			}
			break;

			case MENU_F_06:
			{
				if(Menu_Input_Int_To_Buf(drv.bkp_reg->F_06_cal_stop_down,&input_buf,MENU_F06_MIN_VAL,MENU_F06_MAX_VAL)==INPUT_ERR)
				{
					Menu_Input_Field_Clear(&input_buf);
					Indicator_Blink_Set(IND_2,0xFF,2);
					buzzer_set_buzz(BUZZER_EFFECT_3_BEEP,BUZZER_ON,FROM_TASK);
					Indicator_Blink_Set(IND_2,0xFF,2);
					//error
				}
				else
				{
					Menu_Input_Buf_To_Indicator(&input_buf,IND_2);
				}
			}
			break;

			case MENU_F_07:
			{
				if(Menu_Input_Int_To_Buf(drv.bkp_reg->F_07_cal_stop_up,&input_buf,MENU_F07_MIN_VAL,MENU_F07_MAX_VAL)==INPUT_ERR)
				{
					Menu_Input_Field_Clear(&input_buf);
					Indicator_Blink_Set(IND_2,0xFF,2);
					buzzer_set_buzz(BUZZER_EFFECT_3_BEEP,BUZZER_ON,FROM_TASK);
					Indicator_Blink_Set(IND_2,0xFF,2);
					//error
				}
				else
				{
					Menu_Input_Buf_To_Indicator(&input_buf,IND_2);
				}
			}
			break;

			//--------
			case MENU_F_08:
			{
				if(Menu_Input_Int_To_Buf(drv.bkp_reg->F_08_const_tickness,&input_buf,MENU_F08_MIN_VAL,MENU_F08_MAX_VAL)==INPUT_ERR)
				{
					Menu_Input_Field_Clear(&input_buf);
					Indicator_Blink_Set(IND_2,0xFF,2);
					buzzer_set_buzz(BUZZER_EFFECT_3_BEEP,BUZZER_ON,FROM_TASK);
					Indicator_Blink_Set(IND_2,0xFF,2);
					//error
				}
				else
				{
					Menu_Input_Buf_To_Indicator(&input_buf,IND_2);
				}
			}
			break;

			case MENU_F_09:
			{
				if(Menu_Input_Int_To_Buf(drv.bkp_reg->F_09_const_tickness,&input_buf,MENU_F09_MIN_VAL,MENU_F09_MAX_VAL)==INPUT_ERR)
				{
					Menu_Input_Field_Clear(&input_buf);
					Indicator_Blink_Set(IND_2,0xFF,2);
					buzzer_set_buzz(BUZZER_EFFECT_3_BEEP,BUZZER_ON,FROM_TASK);
					Indicator_Blink_Set(IND_2,0xFF,2);
					//error
				}
				else
				{
					Menu_Input_Buf_To_Indicator(&input_buf,IND_2);
				}
			}
			break;

			case MENU_F_10:
			{
				if(Menu_Input_Int_To_Buf(drv.bkp_reg->F_10_const_tickness,&input_buf,MENU_F10_MIN_VAL,MENU_F10_MAX_VAL)==INPUT_ERR)
				{
					Menu_Input_Field_Clear(&input_buf);
					Indicator_Blink_Set(IND_2,0xFF,2);
					buzzer_set_buzz(BUZZER_EFFECT_3_BEEP,BUZZER_ON,FROM_TASK);
					Indicator_Blink_Set(IND_2,0xFF,2);
					//error
				}
				else
				{
					Menu_Input_Buf_To_Indicator(&input_buf,IND_2);
				}
			}
			break;

			default:
			{

			}
			break;
		}
	}

	return (0);
}


unsigned char startMenu(void) {
	selectedMenuItem = (menuItem*)&m_s0i1;
	input_buf.counter=0;
	input_buf.sign=' ';
	dispMenu();
	return (0);
}

void Menu_Init(void)
{
	xClrIndicatorQueue = xQueueCreate( 2, sizeof( uint8_t ) );
	xTaskCreate(MenuHandler,(signed char*)"Menu",128,NULL, tskIDLE_PRIORITY + 1, NULL);
	task_watches[MENU_TASK].task_status=TASK_IDLE;
}

void Menu_Previous(void)
{
	menuChange(PREVIOUS);
	dispMenu();
}
void Menu_Parent(void)
{
	menuChange(PARENT);
	dispMenu();
}
void Menu_Child(void)
{
	menuChange(CHILD);
	dispMenu();
}
void Menu_Select(void)
{
	menuChange(SELECT);
	dispMenu();
}
void Menu_Next(void)
{
	menuChange(NEXT);
	dispMenu();
}

void Menu_Handle_Key(menuItem* currentMenuItem,uint8_t current_key)
{
	struct mm_imp temp_mm_imp;
	int16_t temp_val;

	if(drv.move_type_flag!=MOVE_TYPE_NONE)//��������� ��������� ������� ��� ��������
	{
		if((current_key==KEY_STOP)||(current_key==KEY_STOP_LONG))
		{
			Drive_Stop(STOP_USER,FROM_TASK);
			buzzer_set_buzz(BUZZER_EFFECT_LONG_BEEP,BUZZER_ON,FROM_TASK);
		}
		return;
	}

	if(drv.error_flag==DRIVE_ERR)
	{
			if(current_key==KEY_C_LONG) //������� ����
			{
				Menu_Input_Field_Clear(&input_buf);

					switch(drv.stop_type)
					{
						case STOP_HI_SENSOR:
						{

						}
						break;

						case STOP_LO_SENSOR:
						{

						}
						break;

						case STOP_INVERTOR_ERROR:
						{
							Drive_Reset();
						}
						break;

						case STOP_CONTROLLER_FAULT:
						{
							//???
						}
						break;

						default:
						{

						}
						break;
					}
				drv.error_flag=DRIVE_OK;
				buzzer_set_buzz(BUZZER_EFFECT_1_BEEP,BUZZER_ON,FROM_TASK);
			}
			return;
		}

		if((drv.function_back_flag==DRIVE_BACK_POS_UP)||(drv.function_back_flag==DRIVE_BACK_GET_UP)||(drv.function_back_flag==DRIVE_BACK_GET_DOWN))
		{
			if(current_key==KEY_ROLLBACK) //������� ����
			{
				drv.function_back_flag=DRIVE_BACK_GET_DOWN;
				if(Drive_Set_Position(MOVE_TYPE_ABSOLUTE,drv.function_back_temp_position)!=DRIVE_OK)
				{
					buzzer_set_buzz(BUZZER_EFFECT_3_BEEP,BUZZER_ON,FROM_TASK);
				}
				else
				{
					buzzer_set_buzz(BUZZER_EFFECT_1_BEEP,BUZZER_ON,FROM_TASK);
				}
			}

			if(current_key==KEY_C_LONG) //������� ����
			{
				Menu_Input_Field_Clear(&input_buf);
				drv.error_flag=DRIVE_OK;
				drv.function_back_flag=DRIVE_BACK_POS_DOWN;
				buzzer_set_buzz(BUZZER_EFFECT_2_BEEP,BUZZER_ON,FROM_TASK);
			}

			return;
		}

		switch(currentMenuItem->Select)
		{
			case MENU_ROOT:
			{
				switch(current_key)
				{
					case KEY_STEP://������� ����������������� ������
					{
						if(const_tickness_counter>2)
						{
							const_tickness_counter=0;
						}

						if(const_tickness_counter==0)
						{
							Menu_Input_Int_To_Buf(drv.bkp_reg->F_08_const_tickness,&input_buf,MENU_F08_MIN_VAL,MENU_F08_MAX_VAL);
							if(drv.bkp_reg->F_08_const_tickness>0)
							{
								input_buf.sign='+';
							}
							else
							{
								input_buf.sign='-';
							}
						}
						else if(const_tickness_counter==1)
						{
							Menu_Input_Int_To_Buf(drv.bkp_reg->F_09_const_tickness,&input_buf,MENU_F09_MIN_VAL,MENU_F09_MAX_VAL);
							if(drv.bkp_reg->F_09_const_tickness>0)
							{
								input_buf.sign='+';
							}
							else
							{
								input_buf.sign='-';
							}
						}
						else
						{
							Menu_Input_Int_To_Buf(drv.bkp_reg->F_10_const_tickness,&input_buf,MENU_F10_MIN_VAL,MENU_F10_MAX_VAL);
							if(drv.bkp_reg->F_10_const_tickness>0)
							{
								input_buf.sign='+';
							}
							else
							{
								input_buf.sign='-';
							}
						}

						Menu_Input_Buf_To_Indicator(&input_buf,IND_2);

						buzzer_set_buzz(BUZZER_EFFECT_1_BEEP,BUZZER_ON,FROM_TASK);
						const_tickness_counter++;
					}
					break;

					case KEY_A_LONG:
					{
						Menu_Child();
						buzzer_set_buzz(BUZZER_EFFECT_LONG_BEEP,BUZZER_ON,FROM_TASK);
					}
					break;

					case KEY_C_LONG://������� ����
					{
						Menu_Input_Field_Clear(&input_buf);
						if(drv.error_flag==DRIVE_ERR)
						{
							switch(drv.stop_type)
							{
								case STOP_HI_SENSOR:
								{

								}
								break;

								case STOP_LO_SENSOR:
								{

								}
								break;

								case STOP_INVERTOR_ERROR:
								{
									Drive_Reset();
								}
								break;

								case STOP_CONTROLLER_FAULT:
								{
									//???
								}
								break;

								default:
								{

								}
								break;
							}
						}
						drv.error_flag=DRIVE_OK;
						buzzer_set_buzz(BUZZER_EFFECT_1_BEEP,BUZZER_ON,FROM_TASK);
					}
					break;

					case KEY_START://���� ��������
					{
						int16_t move_val=0;
						const_tickness_counter=0;
						switch(input_buf.sign)
						{
							case '+':
							{
								if(Menu_Input_Buf_To_Int(&input_buf,&move_val,MENU_ROOT_MIN_VAL,MENU_ROOT_MAX_VAL)==INPUT_OK)
								{

									//��������� ���� �������� ������ ��������� � ������ ��������

									if((Drive_Impulse_To_MM_Absolute(drv.current_position)+move_val)>MENU_ABS_MAX_VAL)
									{
										buzzer_set_buzz(BUZZER_EFFECT_3_BEEP,BUZZER_ON,FROM_TASK);
										Indicator_Blink_Set(IND_2,0xFF,2);
										break;
									}

									//test move val for correct
									if(drv.bkp_reg->F_07_cal_stop_up>=move_val)
									{
										buzzer_set_buzz(BUZZER_EFFECT_3_BEEP,BUZZER_ON,FROM_TASK);
										Indicator_Blink_Set(IND_2,0xFF,2);
										break;
									}

									if(Drive_Set_Position(MOVE_TYPE_RELATIVE_UP, move_val)!=DRIVE_OK)
									{
										Drive_Stop(STOP_CONTROLLER_FAULT,FROM_TASK);
										buzzer_set_buzz(BUZZER_EFFECT_3_BEEP,BUZZER_ON,FROM_TASK);
									}
									else
									{
										buzzer_set_buzz(BUZZER_EFFECT_1_BEEP,BUZZER_ON,FROM_TASK);
									}
								}
								else
								{
									buzzer_set_buzz(BUZZER_EFFECT_3_BEEP,BUZZER_ON,FROM_TASK);
								}
							}
							break;

							case '-':
							{
								if(Menu_Input_Buf_To_Int(&input_buf,&move_val,MENU_ROOT_MIN_VAL,MENU_ROOT_MAX_VAL)==INPUT_OK)
								{
									//��������� ���� �������� ������ ��������� � ������ ��������

									if(((int16_t)Drive_Impulse_To_MM_Absolute(drv.current_position)+move_val)<MENU_ABS_MIN_VAL)
									{
										buzzer_set_buzz(BUZZER_EFFECT_3_BEEP,BUZZER_ON,FROM_TASK);
										Indicator_Blink_Set(IND_2,0xFF,2);
										break;
									}

									//test move val for correct

									if(drv.bkp_reg->F_06_cal_stop_down>=(-move_val))
									{
										buzzer_set_buzz(BUZZER_EFFECT_3_BEEP,BUZZER_ON,FROM_TASK);
										Indicator_Blink_Set(IND_2,0xFF,2);
										break;
									}

									if(Drive_Set_Position(MOVE_TYPE_RELATIVE_DOWN, move_val)!=DRIVE_OK)
									{
										Drive_Stop(STOP_CONTROLLER_FAULT,FROM_TASK);
									}
									else
									{
										buzzer_set_buzz(BUZZER_EFFECT_1_BEEP,BUZZER_ON,FROM_TASK);
									}
								}
								else
								{
									buzzer_set_buzz(BUZZER_EFFECT_3_BEEP,BUZZER_ON,FROM_TASK);
								}
							}
							break;

							case ' ':
							{
								if(Menu_Input_Buf_To_Int(&input_buf,&move_val,MENU_ROOT_ABS_MIN_VAL,MENU_ROOT_ABS_MAX_VAL)==INPUT_OK)
								{

											int16_t temp=move_val-Drive_Impulse_To_MM_Absolute(drv.current_position);

											if(temp>=0)
											{
												if(drv.bkp_reg->F_07_cal_stop_up>=temp)
												{
													buzzer_set_buzz(BUZZER_EFFECT_3_BEEP,BUZZER_ON,FROM_TASK);
													Indicator_Blink_Set(IND_2,0xFF,2);
													break;
												}
											}
											else
											{
												if(drv.bkp_reg->F_06_cal_stop_down>=(-temp))
												{
													buzzer_set_buzz(BUZZER_EFFECT_3_BEEP,BUZZER_ON,FROM_TASK);
													Indicator_Blink_Set(IND_2,0xFF,2);
													break;
												}
											}


											if(Drive_Set_Position(MOVE_TYPE_ABSOLUTE, move_val)!=DRIVE_OK)
											{
												Drive_Stop(STOP_CONTROLLER_FAULT,FROM_TASK);
											}
											else
											{
												buzzer_set_buzz(BUZZER_EFFECT_1_BEEP,BUZZER_ON,FROM_TASK);
											}
								}
								else
								{
									buzzer_set_buzz(BUZZER_EFFECT_3_BEEP,BUZZER_ON,FROM_TASK);
								}
							}
							break;

							default :
							{
								//sign incorrect
							}
							break;
						}
					}
					break;

					case KEY_ROLLBACK://������� BACK
					{
						switch(drv.function_back_flag)
						{
							case DRIVE_BACK_POS_DOWN:
							{
								uint32_t temp_imp_pos=Drive_MM_To_Impulse_Absolute(drv.bkp_reg->F_04_function_back);
								if(temp_imp_pos<=drv.current_position)
								{
									buzzer_set_buzz(BUZZER_EFFECT_3_BEEP,BUZZER_ON,FROM_TASK);
								}
								else
								{
									drv.function_back_flag=DRIVE_BACK_GET_UP;
									drv.function_back_temp_position=Drive_Impulse_To_MM_Absolute(drv.current_position);
									//go to UP  position
									if(Drive_Set_Position_Imp_Absolute(temp_imp_pos)!=DRIVE_OK)
									{
										buzzer_set_buzz(BUZZER_EFFECT_3_BEEP,BUZZER_ON,FROM_TASK);
										drv.function_back_flag=DRIVE_BACK_POS_DOWN;
									}
									else
									{
										buzzer_set_buzz(BUZZER_EFFECT_1_BEEP,BUZZER_ON,FROM_TASK);
									}
								}
							}
							break;

//							case DRIVE_BACK_POS_UP:
//							{
//								drv.function_back_flag=DRIVE_BACK_POS_DOWN;
//								if(Drive_Set_Position_Imp_Absolute(drv.function_back_temp_position)!=DRIVE_OK)
//								{
//									buzzer_set_buzz(BUZZER_EFFECT_3_BEEP,BUZZER_ON,FROM_TASK);
//								}
//								else
//								{
//									buzzer_set_buzz(BUZZER_EFFECT_1_BEEP,BUZZER_ON,FROM_TASK);
//								}
//							}
//							break;

							default:
							{
								//drv.function_back_flag=DRIVE_BACK_POS_DOWN;
							}
							break;
						}

					}
					break;

					case KEY_STAR_LONG://�������� ����
					{
						if(drv.limitation_flag!=DRIVE_LIMITATION_ONLY_UP)
						{
							Drive_Set_Speed(DRIVE_SPEED_HI);
							Drive_Start(DRIVE_DIRECTION_DOWN);
							buzzer_set_buzz(BUZZER_EFFECT_1_BEEP,BUZZER_ON,FROM_TASK);
						}
					}
					break;

					case KEY_SHARP_LONG://�������� �����
					{
						if(drv.limitation_flag!=DRIVE_LIMITATION_ONLY_DOWN)
						{
							Drive_Set_Speed(DRIVE_SPEED_HI);
							Drive_Start(DRIVE_DIRECTION_UP);
							buzzer_set_buzz(BUZZER_EFFECT_1_BEEP,BUZZER_ON,FROM_TASK);
						}
					}
					break;

					case KEY_STAR_LONG_RELEASE://���� �������� ����
					{
						Drive_Stop(STOP_MANUAL_CONTROL,FROM_TASK);
			    		uint32_t temp_position=drv.current_position;
			    		Backup_SRAM_Write_Reg(&drv.bkp_reg->backup_current_position,&temp_position,sizeof(uint32_t));
			    		vTaskDelay(500);
						buzzer_set_buzz(BUZZER_EFFECT_2_BEEP,BUZZER_ON,FROM_TASK);
					}
					break;

					case KEY_SHARP_LONG_RELEASE://���� �������� �����
					{
						Drive_Stop(STOP_MANUAL_CONTROL,FROM_TASK);
			    		uint32_t temp_position=drv.current_position;
			    		Backup_SRAM_Write_Reg(&drv.bkp_reg->backup_current_position,&temp_position,sizeof(uint32_t));
			    		vTaskDelay(500);
						buzzer_set_buzz(BUZZER_EFFECT_2_BEEP,BUZZER_ON,FROM_TASK);
					}
					break;

					case KEY_B://������� back �� ��������� ��������
					{
						uint16_t temp_back=0x0;
						if(Menu_Input_Buf_To_Int(&input_buf,&temp_back,MENU_F04_MIN_VAL,MENU_F04_MAX_VAL)==INPUT_OK)
						{
							Backup_SRAM_Write_Reg(&drv.bkp_reg->F_04_function_back,&temp_back,sizeof(uint16_t));
							buzzer_set_buzz(BUZZER_EFFECT_2_BEEP,BUZZER_ON,FROM_TASK);
						}
						else
						{
							buzzer_set_buzz(BUZZER_EFFECT_3_BEEP,BUZZER_ON,FROM_TASK);
							Indicator_Blink_Set(IND_2,0xFF,2);
							//input error
						}

					}
					break;

					case KEY_B_LONG://������� BACK ��������� ��������
					{
						uint16_t temp=Drive_Impulse_To_MM_Absolute(drv.current_position);
						if((temp>=MENU_F04_MIN_VAL)&&(temp<=MENU_F04_MAX_VAL))
						{
							Backup_SRAM_Write_Reg(&drv.bkp_reg->F_04_function_back,&temp,sizeof(uint16_t));
							buzzer_set_buzz(BUZZER_EFFECT_2_BEEP,BUZZER_ON,FROM_TASK);
						}
						else
						{
							buzzer_set_buzz(BUZZER_EFFECT_3_BEEP,BUZZER_ON,FROM_TASK);
							Indicator_Blink_Set(IND_2,0xFF,2);
							//input error
						}
					}
					break;

					default:
					{
						Menu_Input_Field(current_key,INPUT_WITH_POINT|INPUT_WITH_SIGN,&input_buf,MENU_ROOT_MIN_VAL,MENU_ROOT_MAX_VAL);
					}
					break;
				}
			}
			break;

			case MENU_F_01:
			{
				switch(current_key)
				{
					case KEY_C_LONG:
					{
						Menu_Parent();
						buzzer_set_buzz(BUZZER_EFFECT_LONG_BEEP,BUZZER_ON,FROM_TASK);
					}
					break;

					case KEY_A:
					{
						Menu_Next();
						buzzer_set_buzz(BUZZER_EFFECT_1_BEEP,BUZZER_ON,FROM_TASK);
					}
					break;

					case KEY_POINT_LONG://�������� ��������
					{
						//int16_t temp_val;
						if(Menu_Input_Buf_To_Int(&input_buf,&temp_mm_imp.mm,MENU_F01_MIN_VAL,MENU_F01_MAX_VAL)==INPUT_OK)
						{
							temp_mm_imp.imp=drv.current_position;
							Backup_SRAM_Write_Reg(&drv.bkp_reg->F_01_cal_down,&temp_mm_imp,sizeof(struct mm_imp));
							buzzer_set_buzz(BUZZER_EFFECT_2_BEEP,BUZZER_ON,FROM_TASK);
						}
						else
						{
							buzzer_set_buzz(BUZZER_EFFECT_3_BEEP,BUZZER_ON,FROM_TASK);
							Indicator_Blink_Set(IND_2,0xFF,2);
						}
					}
					break;

					default:
					{
						Menu_Input_Field(current_key,INPUT_WITH_POINT,&input_buf,MENU_F01_MIN_VAL,MENU_F01_MAX_VAL);
					}
					break;
				}
			}
			break;

			case MENU_F_02:
			{
				switch(current_key)
				{
					case KEY_C_LONG:
					{
						Menu_Parent();
						buzzer_set_buzz(BUZZER_EFFECT_LONG_BEEP,BUZZER_ON,FROM_TASK);
					}
					break;

					case KEY_A:
					{
						Menu_Next();
						buzzer_set_buzz(BUZZER_EFFECT_1_BEEP,BUZZER_ON,FROM_TASK);
					}
					break;

					case KEY_POINT_LONG://�������� ��������
					{
						int16_t temp_val;
						if(Menu_Input_Buf_To_Int(&input_buf,&temp_mm_imp.mm,MENU_F02_MIN_VAL,MENU_F02_MAX_VAL)==INPUT_OK)
						{
							temp_mm_imp.imp=drv.current_position;
							Backup_SRAM_Write_Reg(&drv.bkp_reg->F_02_cal_up,&temp_mm_imp,sizeof(struct mm_imp));
							buzzer_set_buzz(BUZZER_EFFECT_2_BEEP,BUZZER_ON,FROM_TASK);
						}
						else
						{
							buzzer_set_buzz(BUZZER_EFFECT_3_BEEP,BUZZER_ON,FROM_TASK);
							Indicator_Blink_Set(IND_2,0xFF,2);
							//input error
						}
					}
					break;

					default:
					{
						Menu_Input_Field(current_key,INPUT_WITH_POINT,&input_buf,MENU_F02_MIN_VAL,MENU_F02_MAX_VAL);
					}
					break;
				}
			}
			break;

			case MENU_F_03:
			{
				switch(current_key)
				{
					case KEY_C_LONG:
					{
						Menu_Parent();
						buzzer_set_buzz(BUZZER_EFFECT_LONG_BEEP,BUZZER_ON,FROM_TASK);
					}
					break;

					case KEY_A:
					{
						Menu_Next();
						buzzer_set_buzz(BUZZER_EFFECT_1_BEEP,BUZZER_ON,FROM_TASK);
					}
					break;

					case KEY_POINT_LONG://�������� ��������
					{
						if(Menu_Input_Buf_To_Int(&input_buf,&temp_mm_imp.mm,MENU_F03_MIN_VAL,MENU_F03_MAX_VAL)==INPUT_OK)
						{
							temp_mm_imp.imp=drv.current_position;
							Backup_SRAM_Write_Reg(&drv.bkp_reg->F_03_cal_syncro,&temp_mm_imp,sizeof(struct mm_imp));
							buzzer_set_buzz(BUZZER_EFFECT_2_BEEP,BUZZER_ON,FROM_TASK);
						}
						else
						{
							buzzer_set_buzz(BUZZER_EFFECT_3_BEEP,BUZZER_ON,FROM_TASK);
							Indicator_Blink_Set(IND_2,0xFF,2);
							//input error
						}
					}
					break;

					default:
					{
						Menu_Input_Field(current_key,INPUT_WITH_POINT,&input_buf,MENU_F03_MIN_VAL,MENU_F03_MAX_VAL);
					}
					break;
				}
			}
			break;

			case MENU_F_04:
			{
				switch(current_key)
				{
					case KEY_C_LONG:
					{
						Menu_Parent();
						buzzer_set_buzz(BUZZER_EFFECT_LONG_BEEP,BUZZER_ON,FROM_TASK);
					}
					break;

					case KEY_A:
					{
						Menu_Next();
						buzzer_set_buzz(BUZZER_EFFECT_1_BEEP,BUZZER_ON,FROM_TASK);
					}
					break;

					case KEY_POINT_LONG://�������� ��������
					{
						if(Menu_Input_Buf_To_Int(&input_buf,&temp_val,MENU_F04_MIN_VAL,MENU_F04_MAX_VAL)==INPUT_OK)
						{
							Backup_SRAM_Write_Reg(&drv.bkp_reg->F_04_function_back,&temp_val,sizeof(uint16_t));
							buzzer_set_buzz(BUZZER_EFFECT_2_BEEP,BUZZER_ON,FROM_TASK);
						}
						else
						{
							buzzer_set_buzz(BUZZER_EFFECT_3_BEEP,BUZZER_ON,FROM_TASK);
							Indicator_Blink_Set(IND_2,0xFF,2);
							//input error
						}
					}
					break;

					default:
					{
						Menu_Input_Field(current_key,INPUT_WITH_POINT,&input_buf,MENU_F04_MIN_VAL,MENU_F04_MAX_VAL);
					}
					break;
				}
			}
			break;

			case MENU_F_05:
			{
				switch(current_key)
				{
					case KEY_C_LONG:
					{
						Menu_Parent();
						buzzer_set_buzz(BUZZER_EFFECT_LONG_BEEP,BUZZER_ON,FROM_TASK);
					}
					break;

					case KEY_A:
					{
						Menu_Next();
						buzzer_set_buzz(BUZZER_EFFECT_1_BEEP,BUZZER_ON,FROM_TASK);
					}
					break;

					case KEY_POINT_LONG://�������� ��������
					{
						if(Menu_Input_Buf_To_Int(&input_buf,&temp_val,MENU_F05_MIN_VAL,MENU_F05_MAX_VAL)==INPUT_OK)
						{
							Backup_SRAM_Write_Reg(&drv.bkp_reg->F_05_cal_speed_down,&temp_val,sizeof(uint16_t));
							buzzer_set_buzz(BUZZER_EFFECT_2_BEEP,BUZZER_ON,FROM_TASK);
						}
						else
						{
							buzzer_set_buzz(BUZZER_EFFECT_3_BEEP,BUZZER_ON,FROM_TASK);
							Indicator_Blink_Set(IND_2,0xFF,2);
							//input error
						}
					}
					break;

					default:
					{
						Menu_Input_Field(current_key,INPUT_WITH_POINT,&input_buf,MENU_F05_MIN_VAL,MENU_F05_MAX_VAL);
					}
					break;
				}
			}
			break;

			case MENU_F_06:
			{
				switch(current_key)
				{
					case KEY_C_LONG:
					{
						Menu_Parent();
						buzzer_set_buzz(BUZZER_EFFECT_LONG_BEEP,BUZZER_ON,FROM_TASK);
					}
					break;

					case KEY_A:
					{
						Menu_Next();
						buzzer_set_buzz(BUZZER_EFFECT_1_BEEP,BUZZER_ON,FROM_TASK);
					}
					break;

					case KEY_POINT_LONG://�������� ��������
					{
						int16_t temp_val;
						if(Menu_Input_Buf_To_Int(&input_buf,&temp_val,MENU_F06_MIN_VAL,MENU_F06_MAX_VAL)==INPUT_OK)
						{
							Backup_SRAM_Write_Reg(&drv.bkp_reg->F_06_cal_stop_down,&temp_val,sizeof(uint16_t));
							buzzer_set_buzz(BUZZER_EFFECT_2_BEEP,BUZZER_ON,FROM_TASK);
						}
						else
						{
							buzzer_set_buzz(BUZZER_EFFECT_3_BEEP,BUZZER_ON,FROM_TASK);
							Indicator_Blink_Set(IND_2,0xFF,2);
							//input error
						}
					}
					break;

					default:
					{
						Menu_Input_Field(current_key,INPUT_WITH_POINT,&input_buf,MENU_F06_MIN_VAL,MENU_F06_MAX_VAL);
					}
					break;
				}
			}
			break;


			case MENU_F_07:
			{
				switch(current_key)
				{
					case KEY_C_LONG:
					{
						Menu_Parent();
						buzzer_set_buzz(BUZZER_EFFECT_LONG_BEEP,BUZZER_ON,FROM_TASK);
					}
					break;

					case KEY_A:
					{
						Menu_Next();
						dispMenu();
						buzzer_set_buzz(BUZZER_EFFECT_1_BEEP,BUZZER_ON,FROM_TASK);
					}
					break;

					case KEY_POINT_LONG://�������� ��������
					{
						int16_t temp_val;
						if(Menu_Input_Buf_To_Int(&input_buf,&temp_val,MENU_F06_MIN_VAL,MENU_F06_MAX_VAL)==INPUT_OK)
						{
							Backup_SRAM_Write_Reg(&drv.bkp_reg->F_07_cal_stop_up,&temp_val,sizeof(uint16_t));
							buzzer_set_buzz(BUZZER_EFFECT_2_BEEP,BUZZER_ON,FROM_TASK);
						}
						else
						{
							buzzer_set_buzz(BUZZER_EFFECT_3_BEEP,BUZZER_ON,FROM_TASK);
							Indicator_Blink_Set(IND_2,0xFF,2);
							//input error
						}
					}
					break;

					default:
					{
						Menu_Input_Field(current_key,INPUT_WITH_POINT,&input_buf,MENU_F07_MIN_VAL,MENU_F07_MAX_VAL);
					}
					break;
				}
			}
			break;

			//----------

			case MENU_F_08:
			{
				switch(current_key)
				{
					case KEY_C_LONG:
					{
						Menu_Parent();
						buzzer_set_buzz(BUZZER_EFFECT_LONG_BEEP,BUZZER_ON,FROM_TASK);
					}
					break;

					case KEY_A:
					{
						Menu_Next();
						dispMenu();
						buzzer_set_buzz(BUZZER_EFFECT_1_BEEP,BUZZER_ON,FROM_TASK);
					}
					break;

					case KEY_POINT_LONG://�������� ��������
					{
						int16_t temp_val;
						if(Menu_Input_Buf_To_Int(&input_buf,&temp_val,MENU_F08_MIN_VAL,MENU_F08_MAX_VAL)==INPUT_OK)
						{
							Backup_SRAM_Write_Reg(&drv.bkp_reg->F_08_const_tickness,&temp_val,sizeof(uint16_t));
							buzzer_set_buzz(BUZZER_EFFECT_2_BEEP,BUZZER_ON,FROM_TASK);
						}
						else
						{
							buzzer_set_buzz(BUZZER_EFFECT_3_BEEP,BUZZER_ON,FROM_TASK);
							Indicator_Blink_Set(IND_2,0xFF,2);
							//input error
						}
					}
					break;

					default:
					{
						Menu_Input_Field(current_key,INPUT_WITH_POINT|INPUT_WITH_SIGN,&input_buf,MENU_F08_MIN_VAL,MENU_F08_MAX_VAL);
					}
					break;
				}
			}
			break;


			case MENU_F_09:
			{
				switch(current_key)
				{
					case KEY_C_LONG:
					{
						Menu_Parent();
						buzzer_set_buzz(BUZZER_EFFECT_LONG_BEEP,BUZZER_ON,FROM_TASK);
					}
					break;

					case KEY_A:
					{
						Menu_Next();
						dispMenu();
						buzzer_set_buzz(BUZZER_EFFECT_1_BEEP,BUZZER_ON,FROM_TASK);
					}
					break;

					case KEY_POINT_LONG://�������� ��������
					{
						int16_t temp_val;
						if(Menu_Input_Buf_To_Int(&input_buf,&temp_val,MENU_F09_MIN_VAL,MENU_F09_MAX_VAL)==INPUT_OK)
						{
							Backup_SRAM_Write_Reg(&drv.bkp_reg->F_09_const_tickness,&temp_val,sizeof(uint16_t));
							buzzer_set_buzz(BUZZER_EFFECT_2_BEEP,BUZZER_ON,FROM_TASK);
						}
						else
						{
							buzzer_set_buzz(BUZZER_EFFECT_3_BEEP,BUZZER_ON,FROM_TASK);
							Indicator_Blink_Set(IND_2,0xFF,2);
							//input error
						}
					}
					break;

					default:
					{
						Menu_Input_Field(current_key,INPUT_WITH_POINT|INPUT_WITH_SIGN,&input_buf,MENU_F09_MIN_VAL,MENU_F09_MAX_VAL);
					}
					break;
				}
			}
			break;

			case MENU_F_10:
			{
				switch(current_key)
				{
					case KEY_C_LONG:
					{
						Menu_Parent();
						buzzer_set_buzz(BUZZER_EFFECT_LONG_BEEP,BUZZER_ON,FROM_TASK);
					}
					break;

					case KEY_A:
					{
						selectedMenuItem = (menuItem*)&m_s1i1;
						dispMenu();
						buzzer_set_buzz(BUZZER_EFFECT_1_BEEP,BUZZER_ON,FROM_TASK);
					}
					break;

					case KEY_POINT_LONG://�������� ��������
					{
						int16_t temp_val;
						if(Menu_Input_Buf_To_Int(&input_buf,&temp_val,MENU_F10_MIN_VAL,MENU_F10_MAX_VAL)==INPUT_OK)
						{
							Backup_SRAM_Write_Reg(&drv.bkp_reg->F_10_const_tickness,&temp_val,sizeof(uint16_t));
							buzzer_set_buzz(BUZZER_EFFECT_2_BEEP,BUZZER_ON,FROM_TASK);
						}
						else
						{
							buzzer_set_buzz(BUZZER_EFFECT_3_BEEP,BUZZER_ON,FROM_TASK);
							Indicator_Blink_Set(IND_2,0xFF,2);
							//input error
						}
					}
					break;

					default:
					{
						Menu_Input_Field(current_key,INPUT_WITH_POINT|INPUT_WITH_SIGN   ,&input_buf,MENU_F10_MIN_VAL,MENU_F10_MAX_VAL);
					}
					break;
				}
			}
			break;
		}
}


#define DISPLAY_DELAY	10
void MenuHandler( void *pvParameters )
{
	uint16_t key;
	uint8_t clr_indicator_msg, display_counter=0;
    startMenu();
    task_watches[MENU_TASK].task_status=TASK_ACTIVE;
    while(1)
    {
		if( (xKeyQueue != 0)&&(uxQueueMessagesWaiting(xKeyQueue)) )
		 {
			 if( xQueueReceive( xKeyQueue, &( key ), ( portTickType ) 10 ) )
			 {
				 Menu_Handle_Key(selectedMenuItem,key);
			 }
		 }

		if( (xClrIndicatorQueue != 0)&&(uxQueueMessagesWaiting(xClrIndicatorQueue)) )
		 {
			 if( xQueueReceive( xClrIndicatorQueue, &( clr_indicator_msg ), ( portTickType ) 10 ) )
			 {
				 if(clr_indicator_msg==MENU_MSG_CLR_INDICATOR)
				 {
					 Menu_Input_Field_Clear(&input_buf);
				 }
			 }
		 }
		display_counter++;
		if((selectedMenuItem->Select==MENU_ROOT)&&(display_counter>=DISPLAY_DELAY))
		{
			display_counter=0x0;
			if(drv.error_flag==DRIVE_OK)
			{
				if(Menu_Input_Int_To_Buf(Drive_Impulse_To_MM_Absolute(drv.current_position),&abs_buf, MENU_ABS_MIN_VAL,MENU_ABS_MAX_VAL)!=INPUT_ERR)
				{
					//Menu_Input_Buf_To_Indicator(&abs_buf,IND_1);
				}
				Menu_Input_Buf_To_Indicator(&abs_buf,IND_1);

				switch(drv.move_type_flag)
				{
					case MOVE_TYPE_RELATIVE_UP:
					{
						if(Menu_Input_Int_To_Buf(Drive_Impulse_To_MM(drv.dest_position-drv.current_position),&abs_buf, MENU_ABS_MIN_VAL,MENU_ABS_MAX_VAL)!=INPUT_ERR)
						{
//							Menu_Input_Buf_To_Indicator(&abs_buf,IND_2);
						}
						Menu_Input_Buf_To_Indicator(&abs_buf,IND_2);
					}
					break;

					case MOVE_TYPE_RELATIVE_DOWN:
					{
						if(Menu_Input_Int_To_Buf(Drive_Impulse_To_MM(drv.current_position-drv.dest_position),&abs_buf, MENU_ABS_MIN_VAL,MENU_ABS_MAX_VAL)!=INPUT_ERR)
						{
//							abs_buf.sign='-';
//							Menu_Input_Buf_To_Indicator(&abs_buf,IND_2);
						}
						abs_buf.sign='-';
						Menu_Input_Buf_To_Indicator(&abs_buf,IND_2);
					}
					break;

					case MOVE_TYPE_ABSOLUTE:
					{
							if(drv.dest_position>=drv.current_position)
							{
								if(Menu_Input_Int_To_Buf(Drive_Impulse_To_MM(drv.dest_position-drv.current_position),&abs_buf, MENU_ABS_MIN_VAL,MENU_ABS_MAX_VAL)!=INPUT_ERR)
								{
									//Menu_Input_Buf_To_Indicator(&abs_buf,IND_2);
								}
								Menu_Input_Buf_To_Indicator(&abs_buf,IND_2);
							}
							else
							{
								if(Menu_Input_Int_To_Buf(Drive_Impulse_To_MM(drv.current_position-drv.dest_position),&abs_buf, MENU_ABS_MIN_VAL,MENU_ABS_MAX_VAL)!=INPUT_ERR)
								{
	//								abs_buf.sign='-';
	//								Menu_Input_Buf_To_Indicator(&abs_buf,IND_2);
								}
								abs_buf.sign='-';
								Menu_Input_Buf_To_Indicator(&abs_buf,IND_2);
							}
					}
					break;

					default:
					{
						if(drv.function_back_flag==DRIVE_BACK_POS_UP)
						{
							str_to_ind(IND_2,"--b--");
						}
					}
				}
			}
			else
			{
				if(drv.error_flag==DRIVE_ERR)
				{
					if((drv.stop_type==STOP_HI_SENSOR)||(drv.stop_type==STOP_LO_SENSOR))
					{
						str_to_ind(IND_1," 5t0P");
					}
					else
					{
						str_to_ind(IND_1,"Err0r");
					}
				}
			}
		}
		task_watches[MENU_TASK].counter++;
    	vTaskDelay(10);
    }
}


void Menu_Input_Field_Clear(struct input_buffer *inp_buf)
{
	inp_buf->counter=0;
	inp_buf->sign=' ';
	Menu_Input_Buf_To_Indicator(inp_buf,IND_2);//��� �� ������
}

void Menu_Input_Field_Down_Clear(void)
{
	Menu_Input_Field_Clear(&input_buf);
}

#define POINT_POS	4
uint8_t Menu_Input_Buf_To_Indicator(struct input_buffer *inp_buf,uint8_t indicator)
{
	uint8_t temp_buf[INPUT_BUF_LEN]="   0.0";
	int8_t i=0,j=0;

	if(inp_buf->counter==0)
	{
		if(inp_buf->sign=='-')
		{
			str_to_ind(indicator,"-  0.0");
		}
		else
		{
			if(inp_buf->sign=='+')
			{
				str_to_ind(indicator,".  0.0");
			}
			else
			{
				str_to_ind(indicator,"   0.0");
			}

		}
		return INPUT_OK;
	}

	if(inp_buf->counter<=INPUT_STRING_MAX)
	{
		j=5;
		for(i=(inp_buf->counter-1);i>=0;i--)
		{
			temp_buf[j]=inp_buf->buf[i];
			j--;
			if(j==POINT_POS)
			{
				j--;
			}
		}

		if(inp_buf->sign=='-')
		{
			temp_buf[0]='-';
		}
		else
		{
			if(inp_buf->sign=='+')
			{
				temp_buf[0]='.';
			}
		}

		str_to_ind(indicator,temp_buf);
		return INPUT_OK;
	}
	else
	{
		return INPUT_ERR;
	}
}


uint8_t Menu_Input_Buf_To_Int(struct input_buffer *inp_buf,int16_t *val,int16_t min_value, int16_t max_value)
{
	int16_t result=0;
	int8_t i=0;

	uint16_t dec_mult=1;

	if(inp_buf->counter==0x0)
	{
		*val=0x0;
		return INPUT_OK;
	}

	for(i=(inp_buf->counter-1);i>=0;i--)
	{
		if((inp_buf->buf[i]<'0')||(inp_buf->buf[i]>'9'))
		{
			return INPUT_ERR;
		}

		result+=(inp_buf->buf[i]-0x30)*dec_mult;
		dec_mult*=10;
	}

	if(inp_buf->sign=='-')
	{
		result=-result;
	}

	*val=result;

	if((result<min_value)||(result>max_value))
	{
		return INPUT_ERR;
	}

	return INPUT_OK;
}

uint8_t Menu_Input_Int_To_Buf(int16_t val,struct input_buffer *inp_buf,int16_t min_value, int16_t max_value)
{
	uint8_t tmp_val,error_flag=0;

	if((val>max_value)||(val<min_value))
	{
		//Menu_Input_Field_Clear(inp_buf);

		if(val>max_value)//��������� �����
		{
			val=max_value;
		}
		else
		{
			val=min_value;
		}

		//return INPUT_ERR;
		error_flag=0x1;
	}
	//else
//	{
		if(val<0)
		{
			inp_buf->sign='-';
			val=-val;
		}
		else
		{
			inp_buf->sign=' ';
		}

		inp_buf->counter=0;

		uint16_t divider=1000;
		uint8_t temp_val;
		uint8_t del_null_flag=0;

		while(divider!=0)
		{
			temp_val=val/divider;

			if(temp_val<10)
			{
				if((temp_val>0)||(del_null_flag))
				{
					inp_buf->buf[inp_buf->counter]=(temp_val+0x30);
					inp_buf->counter++;
					del_null_flag=0x1;
				}
			}

			val=val%divider;
			divider=divider/10;
		}
//	}
	//Menu_Input_Buf_To_Indicator(inp_buf,IND_2);
		if(error_flag)
		{
			return INPUT_ERR;
		}
		else
		{
			return INPUT_OK;
		}

}

void Menu_Input_Field(uint8_t current_key,uint8_t attributes,struct input_buffer *inp_buf,int16_t min_value, int16_t max_value)
{

	if((current_key>KEY_9)&&(current_key!=KEY_C)&&(current_key!=KEY_STAR)&&(current_key!=KEY_SHARP))
	{
		return;
	}

	if(current_key==KEY_C)//backspace
    {
		if(inp_buf->counter)
		{
			if(inp_buf->counter>0)
			{
				inp_buf->counter--;
			}

			Menu_Input_Buf_To_Indicator(inp_buf,IND_2);
			buzzer_set_buzz(BUZZER_EFFECT_1_BEEP,BUZZER_ON,FROM_TASK);
		}

		return;
    }


	switch(current_key)
	{
		case KEY_STAR://-
		{
			if(attributes&INPUT_WITH_SIGN)
			{
				inp_buf->sign='-';
				Menu_Input_Buf_To_Indicator(inp_buf,IND_2);
				buzzer_set_buzz(BUZZER_EFFECT_1_BEEP,BUZZER_ON,FROM_TASK);
			}
			return;
		}
		break;

		case KEY_SHARP://+
		{
			if(attributes&INPUT_WITH_SIGN)
			{
				inp_buf->sign='+';
				Menu_Input_Buf_To_Indicator(inp_buf,IND_2);
				buzzer_set_buzz(BUZZER_EFFECT_1_BEEP,BUZZER_ON,FROM_TASK);
			}
			return;
		}
		break;
	}


	if(inp_buf->counter>=INPUT_STRING_MAX)
	{
		buzzer_set_buzz(BUZZER_EFFECT_3_BEEP,BUZZER_ON,FROM_TASK);
		Indicator_Blink_Set(IND_2,0xFF,2);
		return;
	}

	switch(current_key)
	{
		case KEY_0:
		{
			if(inp_buf->counter==0)
			{
				return;
			}

			inp_buf->buf[inp_buf->counter]='0';
			buzzer_set_buzz(BUZZER_EFFECT_1_BEEP,BUZZER_ON,FROM_TASK);
		}
		break;

		case KEY_1:
		{
			inp_buf->buf[inp_buf->counter]='1';
			buzzer_set_buzz(BUZZER_EFFECT_1_BEEP,BUZZER_ON,FROM_TASK);
		}
		break;

		case KEY_2:
		{
			inp_buf->buf[inp_buf->counter]='2';
			buzzer_set_buzz(BUZZER_EFFECT_1_BEEP,BUZZER_ON,FROM_TASK);
		}
		break;

		case KEY_3:
		{
			inp_buf->buf[inp_buf->counter]='3';
			buzzer_set_buzz(BUZZER_EFFECT_1_BEEP,BUZZER_ON,FROM_TASK);
		}
		break;

		case KEY_4:
		{
			inp_buf->buf[inp_buf->counter]='4';
			buzzer_set_buzz(BUZZER_EFFECT_1_BEEP,BUZZER_ON,FROM_TASK);
		}
		break;

		case KEY_5:
		{
			inp_buf->buf[inp_buf->counter]='5';
			buzzer_set_buzz(BUZZER_EFFECT_1_BEEP,BUZZER_ON,FROM_TASK);
		}
		break;

		case KEY_6:
		{
			inp_buf->buf[inp_buf->counter]='6';
			buzzer_set_buzz(BUZZER_EFFECT_1_BEEP,BUZZER_ON,FROM_TASK);
		}
		break;

		case KEY_7:
		{
			inp_buf->buf[inp_buf->counter]='7';
			buzzer_set_buzz(BUZZER_EFFECT_1_BEEP,BUZZER_ON,FROM_TASK);
		}
		break;

		case KEY_8:
		{
			inp_buf->buf[inp_buf->counter]='8';
			buzzer_set_buzz(BUZZER_EFFECT_1_BEEP,BUZZER_ON,FROM_TASK);
		}
		break;

		case KEY_9:
		{
			inp_buf->buf[inp_buf->counter]='9';
			buzzer_set_buzz(BUZZER_EFFECT_1_BEEP,BUZZER_ON,FROM_TASK);
		}
		break;

		default:
		{

		}
		break;
	}
	inp_buf->counter++;

	int16_t temp_val;

	if(Menu_Input_Buf_To_Int(inp_buf,&temp_val,min_value,max_value)==INPUT_ERR)//����� �� ��������-������� ��������� ��������� ������
	{
		inp_buf->counter--;
		buzzer_set_buzz(BUZZER_EFFECT_3_BEEP,BUZZER_ON,FROM_TASK);
		Indicator_Blink_Set(IND_2,0xFF,2);
	}

	Menu_Input_Buf_To_Indicator(inp_buf,IND_2);
}
