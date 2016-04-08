#ifndef _MENU_H_
#define _MENU_H_

void Menu_Init(void);
void Menu_Previous(void);
void Menu_Parent(void);
void Menu_Child(void);
void Menu_Select(void);
void Menu_Next(void);
void MenuHandler( void *pvParameters );
unsigned char startMenu(void);

void Menu_Input_Field_Down_Clear(void);
enum {
    MENU_ROOT=1,
	MENU_F_01,
    MENU_F_02,
    MENU_F_03,
    MENU_F_04,
    MENU_F_05,
    MENU_F_06,
    MENU_F_07,
    MENU_F_08,
    MENU_F_09,
    MENU_F_10
};

enum
{
	INPUT_WITHOUT_POINT_SIGN=0,
	INPUT_WITH_POINT=1,
	INPUT_WITH_SIGN=2
};

enum
{
	INPUT_OK=0,
	INPUT_ERR,
};

enum
{
	MENU_MSG_NONE=0,
	MENU_MSG_CLR_INDICATOR,
};


#endif
