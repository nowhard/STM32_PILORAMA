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
enum {
    MENU_ROOT=1,
	MENU_F_01,
    MENU_F_02,
    MENU_F_03,
    MENU_F_04,
    MENU_F_05,
    MENU_F_06
};

enum
{
	INPUT_WITHOUT_POINT_SIGN=0,
	INPUT_WITH_POINT=1,
	INPUT_WITH_SIGN=2
};


#endif
