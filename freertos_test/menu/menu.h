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
    MENU_MEASURE=1,
    MENU_PULSE_TIME,
    MENU_DEAD_TIME,
    MENU_MEASURE_TIME,
    MENU_ADC_CONVERT_TIME,
    MENU_CALIBRATE
};

#endif
