#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"

#include <stdio.h>
#include "keyboard.h"
#include "menu.h"
#include "tablo_parser.h"



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
void Menu_Input_Field(uint8_t current_key,uint8_t attributes);
void Menu_Input_Field_Clear(void);
void Menu_Input_Field_Shift(uint8_t direction);

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

	}
	else
	{
		str_to_ind(IND_1,(char *)selectedMenuItem->Text);
	}

	return (0);
}



unsigned char startMenu(void) {
	selectedMenuItem = (menuItem*)&m_s0i1;

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
						Menu_Input_Field_Clear();
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
						Menu_Input_Field(current_key,INPUT_WITH_POINT|INPUT_WITH_SIGN);
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
					}
					break;

					case KEY_A:
					{
						Menu_Next();
						Menu_Input_Field_Clear();
					}
					break;

					default:
					{
						Menu_Input_Field(current_key,INPUT_WITH_POINT|INPUT_WITH_SIGN);
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
					}
					break;

					case KEY_A:
					{
						Menu_Next();
						Menu_Input_Field_Clear();
					}
					break;

					default:
					{
						Menu_Input_Field(current_key,INPUT_WITH_POINT|INPUT_WITH_SIGN);
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
					}
					break;

					case KEY_A:
					{
						Menu_Next();
						Menu_Input_Field_Clear();
					}
					break;
					default:
					{
						Menu_Input_Field(current_key,INPUT_WITH_POINT|INPUT_WITH_SIGN);
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
					}
					break;

					case KEY_A:
					{
						Menu_Next();
						Menu_Input_Field_Clear();
					}
					break;

					default:
					{
						Menu_Input_Field(current_key,INPUT_WITH_POINT|INPUT_WITH_SIGN);
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
					}
					break;

					case KEY_A:
					{
						Menu_Next();
						Menu_Input_Field_Clear();
						//return;
					}
					break;

					default:
					{
						Menu_Input_Field(current_key,INPUT_WITH_POINT|INPUT_WITH_SIGN);
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
					}
					break;

					case KEY_A:
					{
						//Menu_Next();
						selectedMenuItem = (menuItem*)&m_s1i1;
						Menu_Input_Field_Clear();
						dispMenu();
						//return;
					}
					break;

					default:
					{
						Menu_Input_Field(current_key,INPUT_WITH_POINT|INPUT_WITH_SIGN);
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

#define INPUT_BUF_LEN		8
#define INPUT_STRING_MAX	5
//static	uint8_t input_buf[INPUT_BUF_LEN]="";

struct input_buffer
{
	uint8_t buf[INPUT_BUF_LEN];
	uint8_t counter;
}input_buf;

void Menu_Input_Field_Clear(void)
{
	input_buf.counter=0;
	sprintf(&input_buf.buf," 000.0");
	str_to_ind(IND_2,input_buf.buf);
}

enum
{
	DIRECTION_LEFT=0,
	DIRECTION_RIGHT
};

void Menu_Input_Field_Shift(uint8_t direction)
{
	uint8_t i=0;
	if(direction==DIRECTION_LEFT)
	{
		for(i=1;i<(INPUT_STRING_MAX-2);i++)
		{
			input_buf.buf[i]=input_buf.buf[i+1];
		}
		input_buf.buf[INPUT_STRING_MAX-2]=input_buf.buf[INPUT_STRING_MAX];
		input_buf.buf[INPUT_STRING_MAX]='0';
		input_buf.counter++;
	}
	else
	{
		if(direction==DIRECTION_RIGHT)
		{
			input_buf.buf[INPUT_STRING_MAX]=input_buf.buf[INPUT_STRING_MAX-2];

			for(i=(INPUT_STRING_MAX-2);i>1;i--)
			{
				input_buf.buf[i]=input_buf.buf[i-1];
			}
			input_buf.buf[1]='0';
			input_buf.counter--;
		}
	}
}

void Menu_Input_Field(uint8_t current_key,uint8_t attributes)
{
	if(current_key==KEY_C)//backspace
    {
		if(input_buf.counter)
		{
			//input_buf.buf[input_buf.counter-1]=0x0;
			Menu_Input_Field_Shift(DIRECTION_RIGHT);
			//input_buf.counter--;
			str_to_ind(IND_2,input_buf.buf);
		}

		return;
    }

	if(attributes&INPUT_WITH_SIGN)
	{
		switch(current_key)
		{
			case KEY_STAR://-
			{
				input_buf.buf[0]='-';
				str_to_ind(IND_2,input_buf.buf);
				return;
			}
			break;

			case KEY_SHARP://+
			{
				input_buf.buf[0]=' ';
				str_to_ind(IND_2,input_buf.buf);
				return;
			}
			break;
		}
	}

	if(input_buf.counter>=(INPUT_STRING_MAX-1))
	{
		return;
	}

//	if(input_buf.counter==1)
//	{
//		input_buf.buf[4]='.';
//		input_buf.counter++;
//	}

	switch(current_key)
	{
		case KEY_0:
		{
//			input_buf.buf[input_buf.counter]='0';
//			input_buf.counter++;
			Menu_Input_Field_Shift(DIRECTION_LEFT);
			input_buf.buf[INPUT_STRING_MAX]='0';

		}
		break;

		case KEY_1:
		{
//			input_buf.buf[input_buf.counter]='1';
//			input_buf.counter++;
			Menu_Input_Field_Shift(DIRECTION_LEFT);
			input_buf.buf[INPUT_STRING_MAX]='1';

		}
		break;

		case KEY_2:
		{
//			input_buf.buf[input_buf.counter]='2';
//			input_buf.counter++;
			Menu_Input_Field_Shift(DIRECTION_LEFT);
			input_buf.buf[INPUT_STRING_MAX]='2';

		}
		break;

		case KEY_3:
		{
//			input_buf.buf[input_buf.counter]='3';
//			input_buf.counter++;
			Menu_Input_Field_Shift(DIRECTION_LEFT);
			input_buf.buf[INPUT_STRING_MAX]='3';

		}
		break;

		case KEY_4:
		{
//			input_buf.buf[input_buf.counter]='4';
//			input_buf.counter++;
			Menu_Input_Field_Shift(DIRECTION_LEFT);
			input_buf.buf[INPUT_STRING_MAX]='4';

		}
		break;

		case KEY_5:
		{
//			input_buf.buf[input_buf.counter]='5';
//			input_buf.counter++;
			Menu_Input_Field_Shift(DIRECTION_LEFT);
			input_buf.buf[INPUT_STRING_MAX]='5';

		}
		break;

		case KEY_6:
		{
//			input_buf.buf[input_buf.counter]='6';
//			input_buf.counter++;
			Menu_Input_Field_Shift(DIRECTION_LEFT);
			input_buf.buf[INPUT_STRING_MAX]='6';

		}
		break;

		case KEY_7:
		{
//			input_buf.buf[input_buf.counter]='7';
//			input_buf.counter++;
			Menu_Input_Field_Shift(DIRECTION_LEFT);
			input_buf.buf[INPUT_STRING_MAX]='7';

		}
		break;

		case KEY_8:
		{
//			input_buf.buf[input_buf.counter]='8';
//			input_buf.counter++;
			Menu_Input_Field_Shift(DIRECTION_LEFT);
			input_buf.buf[INPUT_STRING_MAX]='8';

		}
		break;

		case KEY_9:
		{
//			input_buf.buf[input_buf.counter]='9';
//			input_buf.counter++;
			Menu_Input_Field_Shift(DIRECTION_LEFT);
			input_buf.buf[INPUT_STRING_MAX]='9';

		}
		break;

		default:
		{

		}
		break;
	}
//	input_buf.buf[input_buf.counter+1]=0x0;
	str_to_ind(IND_2,input_buf.buf);
}
