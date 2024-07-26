#ifndef MENU_H
#define MENU_H
#include "main.h"
#include "stdint.h"
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

typedef struct _MENU_OptionTypeDef
{
    char *String;
		char* item[3];
    char *(*funcdis)(volatile struct Oscilloscope *value,uint8_t item);
		void (*funchand)(volatile struct Oscilloscope *value,uint8_t key); 	
} MENU_OptionTypeDef;

void MENU_SEL_HANDLER(volatile struct Oscilloscope *value,uint8_t key);
void MENU_HANDLER_PWM(volatile struct Oscilloscope *value,uint8_t key);
void MENU_HANDLER_MAIN(volatile struct Oscilloscope *value,uint8_t key);
char* MENU_DISPLAY_PWM(volatile struct Oscilloscope *value,uint8_t item);
char* MENU_DISPLAY_MAIN(volatile struct Oscilloscope *value,uint8_t item);
void drawMenu(volatile struct Oscilloscope *value);
void drawItem(volatile struct Oscilloscope *value);
void resetMenuFlag(volatile struct Oscilloscope *value);
char* MENU_DISPLAY_SET(volatile struct Oscilloscope *value,uint8_t item);
void MENU_HANDLER_SET(volatile struct Oscilloscope *value,uint8_t key);
char* MENU_DISPLAY_TOOL(volatile struct Oscilloscope *value,uint8_t item);
void MENU_HANDLER_TOOL(volatile struct Oscilloscope *value,uint8_t key);
#endif