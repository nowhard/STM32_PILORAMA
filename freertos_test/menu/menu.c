#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_spi.h"


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
void vMeasureOutput( void *pvParameters );
void Menu_Handle_Key(menuItem* currentMenuItem,uint16_t current_key);

extern xQueueHandle xKeyQueue;//очередь клавиатуры
extern xSemaphoreHandle xKeySemaphore;

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
MAKE_MENU(m_s1i1,  m_s1i2,    NULL_ENTRY,  NULL_ENTRY, m_s3i1,  				0,				 "MEASURE");
MAKE_MENU(m_s1i2,  m_s1i3,	  m_s1i1,      NULL_ENTRY, m_s2i1,      			0,				 "SETTINGS");
MAKE_MENU(m_s1i3,  NULL_ENTRY,m_s1i2,      NULL_ENTRY, m_s5i1,      			0,  			 "CALIBRATE");
//MAKE_MENU(m_s1i3,  NULL_ENTRY,m_s1i2,      NULL_ENTRY, NULL_ENTRY,   MENU_RESET, "Reset");

// подменю Настройка
MAKE_MENU(m_s2i1,  m_s2i2,    NULL_ENTRY,  m_s1i2,     m_s4i1	 ,   0						, "Pulse time");
MAKE_MENU(m_s2i2,  m_s2i3,    m_s2i1,      m_s1i2,     m_s4i2	 ,   0				 		, "Dead time");
MAKE_MENU(m_s2i3,  m_s2i4,    m_s2i2,      m_s1i2,     m_s4i3	 ,   0						, "Measure time");
MAKE_MENU(m_s2i4,  NULL_ENTRY,m_s2i3,      m_s1i2,     m_s4i4  	 ,   0						, "ADC convert time");
//// подменю Измерения
//MAKE_MENU(m_s3i1,  NULL_ENTRY,NULL_ENTRY,  m_s1i1,     NULL_ENTRY,   MENU_MEASURE			, "Measure");


MAKE_MENU(m_s4i1,  NULL_ENTRY,    NULL_ENTRY,      m_s2i1,     NULL_ENTRY,   MENU_SET_NULL		, "");
MAKE_MENU(m_s4i2,  NULL_ENTRY,    NULL_ENTRY,      m_s2i2,     NULL_ENTRY,   MENU_STEP_START_STOP 		, "");
//MAKE_MENU(m_s4i3,  NULL_ENTRY,    NULL_ENTRY,      m_s2i3,     NULL_ENTRY,   MENU_MEASURE_TIME		, "");
//MAKE_MENU(m_s4i4,  NULL_ENTRY,    NULL_ENTRY,      m_s2i4,     NULL_ENTRY,   MENU_ADC_CONVERT_TIME	, "");


//MAKE_MENU(m_s5i1,  NULL_ENTRY,NULL_ENTRY,  m_s1i3,     NULL_ENTRY,   MENU_CALIBRATE			, "Calibrate");


void menuChange(menuItem* NewMenu)
{
	if ((void*)NewMenu == (void*)&NULL_ENTRY)
	  return;

	selectedMenuItem = NewMenu;
}

unsigned char dispMenu(void) {
	menuItem* tempMenu,*tempMenu2;


//	Lcd_Write_Cmd(LCD_CMD_CLEAR);//clrscr
	// первая строка - заголовок. Или пункт меню верхнего уровня

	tempMenu=selectedMenuItem->Parent;

//	Lcd_Goto (0,0);


	if ((void*)tempMenu == (void*)&NULL_ENTRY)
	{ // мы на верхнем уровне
//		Lcd_Write_Str ("MENU:");
	}
	else
	{
		 //Lcd_Write_Str ((char *)tempMenu->Text);
		//Lcd_Write_Str ("MENU CHILD:");
	}

//	//lcd_clrscr(2);
//	//
//	Lcd_Goto (1,0);
	tempMenu2=selectedMenuItem->Previous;
//	Lcd_Write_Str ((char *)tempMenu2->Text);


	tempMenu2=selectedMenuItem;
//	Lcd_Goto (2,0);
//	Lcd_Write_Str (">");
//	Lcd_Goto (2,1);
//	Lcd_Write_Str ((char *)selectedMenuItem->Text);
//
//
//	Lcd_Goto (3,0);
	tempMenu2=selectedMenuItem->Next;
//	Lcd_Write_Str ((char *)tempMenu2->Text);

	return (0);
}



unsigned char startMenu(void) {
	selectedMenuItem = (menuItem*)&m_s1i1;

	dispMenu();

	return (0);
}

void Menu_Init(void)
{
	 xTaskCreate(MenuHandler,(signed char*)"Menu",128,NULL, tskIDLE_PRIORITY + 1, NULL);
	 xTaskCreate(vMeasureOutput,(signed char*)"OutMeasure",96,NULL, tskIDLE_PRIORITY + 1, NULL);

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

	menuItem* currentMenuItemChild;
	uint8_t i=0;
	currentMenuItemChild=currentMenuItem->Child;

	if(current_key==KEY_7)
	{
		Menu_Parent();
	}
	else
	{
		if((void*)currentMenuItem->Child!=(void*)&NULL_ENTRY)//если есть вложенные меню у потомков
		{
			switch(current_key)
			{
				case KEY_6:
				{
					Menu_Previous();
				}
				break;

				case KEY_8:
				{
					Menu_Next();
				}
				break;

				case KEY_0:
				{
					Menu_Child();
					if((void*)currentMenuItem->Child==(void*)&NULL_ENTRY)
					{
						//Lcd_Write_Cmd(LCD_CMD_CLEAR);
						//Lcd_Goto (0,0);
						switch(currentMenuItem->Select)
						{

							case MENU_SET_NULL:
							{

							}
							break;

							case MENU_STEP_START_STOP:
							{
								//Lcd_Write_Str ("Set Pulse Time:");
							}
							break;
						}
					}
				}
				break;
			}
		}
		else
		{
			switch(currentMenuItem->Select)
			{
				case MENU_SET_NULL:
				{
					switch(current_key)
					{
						case KEY_6:
						{

						}
						break;

						case KEY_8:
						{

						}
						break;

						case KEY_0:
						{

						}
						break;
					}
				}
				break;

				case MENU_STEP_START_STOP:
				{
					switch(current_key)
					{
					case KEY_6:
					{

					}
					break;

					case KEY_8:
					{

					}
					break;

						case KEY_0:
						{

						}
						break;
					}
				}
				break;
			}
		}
	}
}

void MenuHandler( void *pvParameters )
{
	uint16_t key;
	uint8_t str[20]="";
    startMenu();
    while(1)
    {
        if( xKeySemaphore != NULL )
        {

            if( xSemaphoreTake( xKeySemaphore, ( portTickType ) 10 ) == pdTRUE )
            {
            	if( xKeyQueue != 0 )
            	 {
					 if( xQueueReceive( xKeyQueue, &( key ), ( portTickType ) 10 ) )
					 {
						 Menu_Handle_Key(selectedMenuItem,key);
					 }
            	 }
            }
            else
            {

            }
        }

    	vTaskDelay(10);
    }
}


void vMeasureOutput( void *pvParameters )
{
	while(1)
	{
			vTaskDelay(300);
	}
}
