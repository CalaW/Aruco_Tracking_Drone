#ifndef  __BUTTON_H
#define  __BUTTON_H
#include <stdint.h>
#include "gui_config.h"

typedef struct{
	uint32_t x;
	uint32_t y;
	uint32_t width;
	uint32_t height;
	uint8_t* title;
	uint8_t state;
}Button_Typedef;


void test_Button(void);
void GUI_Button_Selected(Button_Typedef *btn,uint8_t state);
void GUI_DrawButton(Button_Typedef *btn);
void GUI_ButtonDelete(Button_Typedef *btn);


#endif


