#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"

#include <stdio.h>
#include "keyboard.h"
#include "menu.h"
#include "tablo_parser.h"
#include "backup_sram.h"

//Инклуды от FreeRTOS:

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

menuItem* selectedMenuItem; // текущий пункт меню

menuItem* menuStack[10];
uint8_t display_buf[20];
volatile unsigned char menuStackTop;
void MenuHandler( void *pvParameters );
void Menu_Handle_Key(menuItem* currentMenuItem,uint8_t current_key);

#define INPUT_BUF_LEN		8
#define INPUT_STRING_MAX	5
#define INPUT_MAX_VALUE		6000//600.0

struct input_buffer
{
	uint8_t buf[INPUT_BUF_LEN];
	uint8_t counter;
}input_buf;
void 	Menu_Input_Field(uint8_t current_key,uint8_t attributes,struct input_buffer *inp_buf);
void 	Menu_Input_Field_Clear(struct input_buffer *inp_buf);
void 	Menu_Input_Field_Shift(uint8_t direction,struct input_buffer *inp_buf);
int16_t Menu_Input_Buf_To_Int(struct input_buffer *inp_buf);
void 	Menu_Input_Int_To_Buf(int16_t val,struct input_buffer *inp_buf);

extern xQueueHandle xKeyQueue;//очередь клавиатуры

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
MAKE_MENU(m_s1i6,  NULL_ENTRY,m_s1i5,      m_s0i1, 	   NULL_ENTRY,  MENU_F_06,  "F-06");


extern  struct dev_registers *dev_reg;

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
	{ // мы на верхнем уровне
		str_to_ind(IND_1,"UP");
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
				//Menu_Input_Int_To_Buf(dev_reg->F_01_cal_up,&input_buf);
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
				//Menu_Input_Field_Clear(&input_buf);
				Menu_Input_Int_To_Buf(dev_reg->F_04_current_position,&input_buf);
			}
			break;

			case MENU_F_05:
			{
				//Menu_Input_Field_Clear(&input_buf);
				Menu_Input_Int_To_Buf(dev_reg->F_05_cal_speed_down,&input_buf);
			}
			break;

			case MENU_F_06:
			{
				//Menu_Input_Field_Clear(&input_buf);
				Menu_Input_Int_To_Buf(dev_reg->F_06_cal_stop,&input_buf);
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
	//Menu_Input_Field_Clear(&input_buf);
	dispMenu();

	return (0);
}

void Menu_Init(void)
{
	 xTaskCreate(MenuHandler,(signed char*)"Menu",128,NULL, tskIDLE_PRIORITY + 1, NULL);
	 //startMenu();
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
		switch(currentMenuItem->Select)
		{
			case MENU_ROOT:
			{
				switch(current_key)
				{
					case KEY_A_LONG:
					{
						Menu_Child();
						//Menu_Input_Field_Clear(&input_buf);
					}
					break;

					case KEY_POINT://ввод значения
					{

					}
					break;

					case KEY_B://функция BACK
					{

					}
					break;

					case KEY_STAR_LONG://движение вниз
					{

					}
					break;

					case KEY_SHARP_LONG://движение вверх
					{

					}
					break;

					case KEY_STAR_LONG_RELEASE://стоп движение вниз
					{

					}
					break;

					case KEY_SHARP_LONG_RELEASE://стоп движение вверх
					{

					}
					break;

					default:
					{
						Menu_Input_Field(current_key,INPUT_WITH_POINT|INPUT_WITH_SIGN,&input_buf);
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
						//Menu_Input_Field_Clear(&input_buf);
						Menu_Parent();
					}
					break;

					case KEY_A:
					{
						Menu_Next();
						//Menu_Input_Field_Clear(&input_buf);
					}
					break;

					case KEY_POINT_LONG://запомним значение
					{
						Backup_SRAM_Write_Reg(&dev_reg->F_01_cal_up,Menu_Input_Buf_To_Int(&input_buf));
					}
					break;

					default:
					{
						Menu_Input_Field(current_key,INPUT_WITH_POINT|INPUT_WITH_SIGN,&input_buf);
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
						//Menu_Input_Field_Clear(&input_buf);
						Menu_Parent();
					}
					break;

					case KEY_A:
					{
						Menu_Next();
						//Menu_Input_Field_Clear(&input_buf);
					}
					break;

					case KEY_POINT_LONG://запомним значение
					{
						Backup_SRAM_Write_Reg(&dev_reg->F_02_cal_down,Menu_Input_Buf_To_Int(&input_buf));
					}
					break;

					default:
					{
						Menu_Input_Field(current_key,INPUT_WITH_POINT|INPUT_WITH_SIGN,&input_buf);
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
						//Menu_Input_Field_Clear(&input_buf);
						Menu_Parent();
					}
					break;

					case KEY_A:
					{
						Menu_Next();
						//Menu_Input_Field_Clear(&input_buf);
					}
					break;

					case KEY_POINT_LONG://запомним значение
					{
						Backup_SRAM_Write_Reg(&dev_reg->F_03_cal_syncro,Menu_Input_Buf_To_Int(&input_buf));
					}
					break;

					default:
					{
						Menu_Input_Field(current_key,INPUT_WITH_POINT,&input_buf);
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
						//Menu_Input_Field_Clear(&input_buf);
						Menu_Parent();
					}
					break;

					case KEY_A:
					{
						Menu_Next();
						//Menu_Input_Field_Clear(&input_buf);
					}
					break;

					case KEY_POINT_LONG://запомним значение
					{
						Backup_SRAM_Write_Reg(&dev_reg->F_04_current_position,Menu_Input_Buf_To_Int(&input_buf));
					}
					break;

					default:
					{
						Menu_Input_Field(current_key,INPUT_WITH_POINT,&input_buf);
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
						//Menu_Input_Field_Clear(&input_buf);
						Menu_Parent();
					}
					break;

					case KEY_A:
					{
						Menu_Next();
						//Menu_Input_Field_Clear(&input_buf);
						//return;
					}
					break;

					case KEY_POINT_LONG://запомним значение
					{
						Backup_SRAM_Write_Reg(&dev_reg->F_05_cal_speed_down,Menu_Input_Buf_To_Int(&input_buf));
					}
					break;

					default:
					{
						Menu_Input_Field(current_key,INPUT_WITH_POINT,&input_buf);
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
						//Menu_Input_Field_Clear(&input_buf);
						Menu_Parent();
					}
					break;

					case KEY_A:
					{
						//Menu_Next();
						selectedMenuItem = (menuItem*)&m_s1i1;
						//Menu_Input_Field_Clear(&input_buf);
						dispMenu();
						//return;
					}
					break;

					case KEY_POINT_LONG://запомним значение
					{
						Backup_SRAM_Write_Reg(&dev_reg->F_06_cal_stop,Menu_Input_Buf_To_Int(&input_buf));
					}
					break;

					default:
					{
						Menu_Input_Field(current_key,INPUT_WITH_POINT,&input_buf);
					}
					break;
				}
			}
			break;

		}
}

void MenuHandler( void *pvParameters )
{
	uint16_t key;
    startMenu();
    while(1)
    {
		if( (xKeyQueue != 0)&&(uxQueueMessagesWaiting(xKeyQueue)) )
		 {
			 if( xQueueReceive( xKeyQueue, &( key ), ( portTickType ) 10 ) )
			 {
				 Menu_Handle_Key(selectedMenuItem,key);
			 }
		 }
    	vTaskDelay(10);
    }
}



void Menu_Input_Field_Clear(struct input_buffer *inp_buf)
{
	input_buf.counter=0;
	sprintf(inp_buf->buf," 000.0");
	str_to_ind(IND_2,inp_buf->buf);
}

enum
{
	DIRECTION_LEFT=0,
	DIRECTION_RIGHT
};

void Menu_Input_Field_Shift(uint8_t direction,struct input_buffer *inp_buf)
{
	uint8_t i=0;
	if(direction==DIRECTION_LEFT)
	{
		for(i=1;i<(INPUT_STRING_MAX-2);i++)
		{
			inp_buf->buf[i]=inp_buf->buf[i+1];
		}
		inp_buf->buf[INPUT_STRING_MAX-2]=inp_buf->buf[INPUT_STRING_MAX];
		inp_buf->buf[INPUT_STRING_MAX]='0';
		inp_buf->counter++;
	}
	else
	{
		if(direction==DIRECTION_RIGHT)
		{
			inp_buf->buf[INPUT_STRING_MAX]=inp_buf->buf[INPUT_STRING_MAX-2];

			for(i=(INPUT_STRING_MAX-2);i>1;i--)
			{
				inp_buf->buf[i]=inp_buf->buf[i-1];
			}
			inp_buf->buf[1]='0';
			inp_buf->counter--;
		}
	}
}

int16_t Menu_Input_Buf_To_Int(struct input_buffer *inp_buf)
{
	int16_t result=0;
	uint8_t i=0,counter=0,temp_val=0;

	result=((inp_buf->buf[1]-0x30)*1000)+((inp_buf->buf[2]-0x30)*100)+((inp_buf->buf[3]-0x30)*10)+((inp_buf->buf[5]-0x30));

	if(inp_buf->buf[0]=='-')
	{
		result=-result;
	}

	return result;
}

void Menu_Input_Int_To_Buf(int16_t val,struct input_buffer *inp_buf)
{
	uint8_t tmp_val,i;

	if((val>INPUT_MAX_VALUE)||(val<(-INPUT_MAX_VALUE)))
	{
		sprintf(inp_buf->buf," 000.0");
		inp_buf->counter=0x0;
	}
	else
	{
		sprintf(inp_buf->buf," 000.0");
		if(val>=0)
		{
			inp_buf->buf[0]=' ';
		}
		else
		{
			inp_buf->buf[0]='-';
			val=-val;
		}


		tmp_val=val%10;
		inp_buf->buf[5]=tmp_val+0x30;
		val=val/10;

		tmp_val=val%10;
		inp_buf->buf[3]=tmp_val+0x30;
		val=val/10;

		tmp_val=val%10;
		inp_buf->buf[2]=tmp_val+0x30;
		val=val/10;

		tmp_val=val%10;
		inp_buf->buf[1]=tmp_val+0x30;
		val=val/10;



		for(i=1;i<=5;i++)
		{
			if((inp_buf->buf[i]!='0')&&(inp_buf->buf[i]!='.'))
			{
				break;
			}
		}

		switch(i)
		{
			case 1:
			{
				inp_buf->counter=4;
			}
			break;

			case 2:
			{
				inp_buf->counter=3;
			}
			break;

			case 3:
			{
				inp_buf->counter=2;
			}
			break;

			case 5:
			{
				inp_buf->counter=1;
			}
			break;

			default:
			{
				inp_buf->counter=0;
			}
			break;
		}
	}
	str_to_ind(IND_2,inp_buf->buf);
}

void Menu_Input_Field(uint8_t current_key,uint8_t attributes,struct input_buffer *inp_buf)
{
	if(current_key==KEY_C)//backspace
    {
		if(inp_buf->counter)
		{
			Menu_Input_Field_Shift(DIRECTION_RIGHT,inp_buf);
			str_to_ind(IND_2,inp_buf->buf);
		}

		return;
    }

	if(attributes&INPUT_WITH_SIGN)
	{
		switch(current_key)
		{
			case KEY_STAR://-
			{
				inp_buf->buf[0]='-';
				str_to_ind(IND_2,inp_buf->buf);
				return;
			}
			break;

			case KEY_SHARP://+
			{
				inp_buf->buf[0]=' ';
				str_to_ind(IND_2,inp_buf->buf);
				return;
			}
			break;
		}
	}

	if(inp_buf->counter>=(INPUT_STRING_MAX-1))
	{
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

			Menu_Input_Field_Shift(DIRECTION_LEFT,inp_buf);
			inp_buf->buf[INPUT_STRING_MAX]='0';
		}
		break;

		case KEY_1:
		{
			Menu_Input_Field_Shift(DIRECTION_LEFT,inp_buf);
			inp_buf->buf[INPUT_STRING_MAX]='1';
		}
		break;

		case KEY_2:
		{
			Menu_Input_Field_Shift(DIRECTION_LEFT,inp_buf);
			inp_buf->buf[INPUT_STRING_MAX]='2';
		}
		break;

		case KEY_3:
		{
			Menu_Input_Field_Shift(DIRECTION_LEFT,inp_buf);
			inp_buf->buf[INPUT_STRING_MAX]='3';
		}
		break;

		case KEY_4:
		{
			Menu_Input_Field_Shift(DIRECTION_LEFT,inp_buf);
			inp_buf->buf[INPUT_STRING_MAX]='4';
		}
		break;

		case KEY_5:
		{
			Menu_Input_Field_Shift(DIRECTION_LEFT,inp_buf);
			inp_buf->buf[INPUT_STRING_MAX]='5';
		}
		break;

		case KEY_6:
		{
			Menu_Input_Field_Shift(DIRECTION_LEFT,inp_buf);
			inp_buf->buf[INPUT_STRING_MAX]='6';
		}
		break;

		case KEY_7:
		{
			Menu_Input_Field_Shift(DIRECTION_LEFT,inp_buf);
			inp_buf->buf[INPUT_STRING_MAX]='7';
		}
		break;

		case KEY_8:
		{
			Menu_Input_Field_Shift(DIRECTION_LEFT,inp_buf);
			inp_buf->buf[INPUT_STRING_MAX]='8';
		}
		break;

		case KEY_9:
		{
			Menu_Input_Field_Shift(DIRECTION_LEFT,inp_buf);
			inp_buf->buf[INPUT_STRING_MAX]='9';
		}
		break;

		default:
		{

		}
		break;
	}

	if((Menu_Input_Buf_To_Int(inp_buf)>INPUT_MAX_VALUE)||(Menu_Input_Buf_To_Int(inp_buf)<(-INPUT_MAX_VALUE)))//выход за диапазон-удаляем последний введенный символ
	{
		Menu_Input_Field_Shift(DIRECTION_RIGHT,inp_buf);
	}

	str_to_ind(IND_2,inp_buf->buf);
}
