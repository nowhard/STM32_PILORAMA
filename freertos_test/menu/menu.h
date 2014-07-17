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
    MENU_SET_NULL=1,
    MENU_STEP_START_STOP,
};

#endif
