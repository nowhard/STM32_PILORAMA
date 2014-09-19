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
void Menu_Input_Field(uint8_t current_key);

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
						Menu_Input_Field(current_key);
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
					}
					break;

					default:
					{
						Menu_Input_Field(current_key);
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
					}
					break;

					default:
					{
						Menu_Input_Field(current_key);
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
					}
					break;
					default:
					{
						Menu_Input_Field(current_key);
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
					}
					break;

					default:
					{
						Menu_Input_Field(current_key);
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
						//return;
					}
					break;

					default:
					{
						Menu_Input_Field(current_key);
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
						dispMenu();
						//return;
					}
					break;

					default:
					{
						Menu_Input_Field(current_key);
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

#define INPUT_BUF_LEN	8
static	uint8_t input_buf[INPUT_BUF_LEN]="";

void Menu_Input_Field(uint8_t current_key)
{
	uint8_t current_buf_len=0;
	current_buf_len=strlen(input_buf);

	if(current_buf_len>(INPUT_BUF_LEN-1))
	{
		return;
	}

	switch(current_key)
	{
		case KEY_0:
		{
			input_buf[current_buf_len]='0';
		}
		break;

		case KEY_1:
		{
			input_buf[current_buf_len]='1';
		}
		break;

		case KEY_2:
		{
			input_buf[current_buf_len]='2';
		}
		break;

		case KEY_3:
		{
			input_buf[current_buf_len]='3';
		}
		break;

		case KEY_4:
		{
			input_buf[current_buf_len]='4';
		}
		break;

		case KEY_5:
		{
			input_buf[current_buf_len]='5';
		}
		break;

		case KEY_6:
		{
			input_buf[current_buf_len]='6';
		}
		break;

		case KEY_7:
		{
			input_buf[current_buf_len]='7';
		}
		break;

		case KEY_8:
		{
			input_buf[current_buf_len]='8';
		}
		break;

		case KEY_9:
		{
			input_buf[current_buf_len]='9';
		}
		break;

		case KEY_STAR://-
		{

		}
		break;

		case KEY_SHARP://+
		{

		}
		break;

		case KEY_C://backspace
		{

		}
		break;

		default:
		{

		}
		break;
	}
	input_buf[current_buf_len+1]=0x0;
	str_to_ind(IND_2,input_buf);
}
