#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"

#include <stdio.h>
#include "keyboard.h"
#include "menu.h"
//#include "display.h"


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
//void vMeasureOutput( void *pvParameters );
void Menu_Handle_Key(menuItem* currentMenuItem,uint16_t current_key);

extern xQueueHandle xKeyQueue;//очередь клавиатуры
//extern xSemaphoreHandle xKeySemaphore;

//uint32_t delay=0;
//uint8_t flag_measure_out=0;

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
MAKE_MENU(m_s1i1,  m_s1i2,    NULL_ENTRY,  NULL_ENTRY, NULL_ENTRY,  MENU_F_01,	"F-01");
MAKE_MENU(m_s1i2,  m_s1i3,	  m_s1i1,      NULL_ENTRY, NULL_ENTRY,  MENU_F_02,	"F-02");
MAKE_MENU(m_s1i3,  m_s1i4,	  m_s1i2,      NULL_ENTRY, NULL_ENTRY,  MENU_F_03,  "F-03");
MAKE_MENU(m_s1i4,  m_s1i5,    m_s1i3,      NULL_ENTRY, NULL_ENTRY,  MENU_F_04,  "F-04");
MAKE_MENU(m_s1i5,  m_s1i6,    m_s1i4,      NULL_ENTRY, NULL_ENTRY,  MENU_F_05,  "F-05");
MAKE_MENU(m_s1i6,  NULL_ENTRY,m_s1i5,      NULL_ENTRY, NULL_ENTRY,  MENU_F_06,  "F-06");


void menuChange(menuItem* NewMenu)
{
	if ((void*)NewMenu == (void*)&NULL_ENTRY)
	  return;

	selectedMenuItem = NewMenu;
}

unsigned char dispMenu(void) {
	menuItem* tempMenu,*tempMenu2;



	tempMenu=selectedMenuItem->Parent;




	if ((void*)tempMenu == (void*)&NULL_ENTRY)
	{ // мы на верхнем уровне

	}
	else
	{
		 //Lcd_Write_Str ((char *)tempMenu->Text);
		//Lcd_Write_Str ("MENU CHILD:");
	}

	return (0);
}



unsigned char startMenu(void) {
	//selectedMenuItem = (menuItem*)&m_s1i1;

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

void Menu_Handle_Key(menuItem* currentMenuItem,uint16_t current_key)
{

//	menuItem* currentMenuItemChild;
	uint8_t i=0;
//	currentMenuItemChild=currentMenuItem->Child;

//	if(current_key==KEY_7)
//	{
//		Menu_Parent();
//	}
//	else
//	{
//		if((void*)currentMenuItem->Child!=(void*)&NULL_ENTRY)//если есть вложенные меню у потомков
//		{
//			switch(current_key)
//			{
//				case KEY_6:
//				{
//					Menu_Previous();
//				}
//				break;
//
//				case KEY_8:
//				{
//					Menu_Next();
//				}
//				break;
//
//				case KEY_0:
//				{
//					Menu_Child();
//					if((void*)currentMenuItem->Child==(void*)&NULL_ENTRY)
//					{
//						//Lcd_Write_Cmd(LCD_CMD_CLEAR);
//						//Lcd_Goto (0,0);
//						switch(currentMenuItem->Select)
//						{
//
//							case MENU_SET_NULL:
//							{
//
//							}
//							break;
//
//							case MENU_STEP_START_STOP:
//							{
//								//Lcd_Write_Str ("Set Pulse Time:");
//							}
//							break;
//						}
//					}
//				}
//				break;
//			}
//		}
//		else
//		{
//			switch(currentMenuItem->Select)
//			{
//				case MENU_SET_NULL:
//				{
//					switch(current_key)
//					{
//						case KEY_6:
//						{
//
//						}
//						break;
//
//						case KEY_8:
//						{
//
//						}
//						break;
//
//						case KEY_0:
//						{
//
//						}
//						break;
//					}
//				}
//				break;
//
//				case MENU_STEP_START_STOP:
//				{
//					switch(current_key)
//					{
//					case KEY_6:
//					{
//
//					}
//					break;
//
//					case KEY_8:
//					{
//
//					}
//					break;
//
//						case KEY_0:
//						{
//
//						}
//						break;
//					}
//				}
//				break;
//			}
//		}
//	}
}

void MenuHandler( void *pvParameters )
{
	uint16_t key;
	uint8_t str[20]="";
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

//
//void vMeasureOutput( void *pvParameters )
//{
//	while(1)
//	{
//			vTaskDelay(300);
//	}
//}
