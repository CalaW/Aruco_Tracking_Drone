#include "button.h"
#include "gui_basic.h"
#include "string.h"
#include "delay.h"
#include "text.h"

#define BUTTON_RADIUS  8

//得到字符串的长度（字符为6*12）
static uint16_t getBtnStringlen(uint8_t *str)
{
	uint16_t strlenth=0;
	strlenth=strlen((const char*)str);//字节数
	strlenth*=6;//一个字节水平6个点(12x12汉字)
	return strlenth;
}

//设置按钮的状态
void GUI_Button_Selected(Button_Typedef *btn,uint8_t state)
{
	if(btn->height<14) return;
	if(btn->width<30)  return;
	if(getBtnStringlen(btn->title)>btn->width) return;

	btn->state = state;
	if(btn->state)
	{
		//填充按钮
		GUI_FloodFill(btn->x + BUTTON_RADIUS,btn->y + BUTTON_RADIUS, 1);
		show_str(btn->x+(btn->width-getBtnStringlen(btn->title))/2, btn->y+(btn->height-12)/2, btn->title, 12,12,0);
	}
	else
	{
		GUI_FloodFill(btn->x + BUTTON_RADIUS,btn->y + BUTTON_RADIUS, 0);

		GUI_Arc4(btn->x+BUTTON_RADIUS, btn->y+BUTTON_RADIUS, BUTTON_RADIUS,3, 1);
		GUI_Line(btn->x+BUTTON_RADIUS, btn->y, btn->x+BUTTON_RADIUS+btn->width-1-2*BUTTON_RADIUS, btn->y,1);

		GUI_Arc4(btn->x+btn->width-1-BUTTON_RADIUS, btn->y+BUTTON_RADIUS, BUTTON_RADIUS,4, 1);	
		GUI_Line(btn->x+BUTTON_RADIUS, btn->y+btn->height-1, btn->x+BUTTON_RADIUS+btn->width-1-2*BUTTON_RADIUS, btn->y+btn->height-1,1);	

		GUI_Arc4(btn->x+btn->width-1-BUTTON_RADIUS, btn->y+ btn->height-1-BUTTON_RADIUS, BUTTON_RADIUS,1, 1);	
		GUI_Line(btn->x, btn->y+BUTTON_RADIUS, btn->x, btn->y+btn->height-1-BUTTON_RADIUS,1);	

		GUI_Arc4(btn->x+BUTTON_RADIUS, btn->y+ btn->height-1-BUTTON_RADIUS, BUTTON_RADIUS,2, 1);	
		GUI_Line(btn->x+btn->width-1, btn->y+BUTTON_RADIUS, btn->x+btn->width-1,  btn->y+btn->height-1-BUTTON_RADIUS,1);			 

		//显示字符
		show_str(btn->x+(btn->width-getBtnStringlen(btn->title))/2, btn->y+(btn->height-12)/2, btn->title, 12,12,1);
	}
}




//画一个按钮
void GUI_DrawButton(Button_Typedef *btn)
{
	if(btn->height<14) return;
	if(btn->width<30)  return;
	if(getBtnStringlen(btn->title)>btn->width) return;

	GUI_Arc4(btn->x+BUTTON_RADIUS, btn->y+BUTTON_RADIUS, BUTTON_RADIUS,3, 1);
	GUI_Line(btn->x+BUTTON_RADIUS, btn->y, btn->x+BUTTON_RADIUS+btn->width-1-2*BUTTON_RADIUS, btn->y,1);

	GUI_Arc4(btn->x+btn->width-1-BUTTON_RADIUS, btn->y+BUTTON_RADIUS, BUTTON_RADIUS,4, 1);	
	GUI_Line(btn->x+BUTTON_RADIUS, btn->y+btn->height-1, btn->x+BUTTON_RADIUS+btn->width-1-2*BUTTON_RADIUS, btn->y+btn->height-1,1);	

	GUI_Arc4(btn->x+btn->width-1-BUTTON_RADIUS, btn->y+ btn->height-1-BUTTON_RADIUS, BUTTON_RADIUS,1, 1);	
	GUI_Line(btn->x, btn->y+BUTTON_RADIUS, btn->x, btn->y+btn->height-1-BUTTON_RADIUS,1);	

	GUI_Arc4(btn->x+BUTTON_RADIUS, btn->y+ btn->height-1-BUTTON_RADIUS, BUTTON_RADIUS,2, 1);	
	GUI_Line(btn->x+btn->width-1, btn->y+BUTTON_RADIUS, btn->x+btn->width-1,  btn->y+btn->height-1-BUTTON_RADIUS,1);	


	if(btn->state)
	{
		//填充按钮
		GUI_FloodFill(btn->x + BUTTON_RADIUS,btn->y + BUTTON_RADIUS, 1);
		show_str(btn->x + (btn->width-getBtnStringlen(btn->title))/2, btn->y + (btn->height-12)/2,	btn->title, 12,12,0);
	}
	else
	{
		show_str(btn->x + (btn->width-getBtnStringlen(btn->title))/2, btn->y + (btn->height-12)/2,	btn->title, 12,12,1);
	}
};

//删除按钮
void GUI_ButtonDelete(Button_Typedef *btn)
{
	GUI_RectangleFill(btn->x, btn->y, btn->x+btn->width-1,  btn->y +btn->height-1, 0);
}

Button_Typedef button={
10,
10,
40,
14,
"OK",
1
};

//测试按钮
void test_Button(void)
{
	GUI_DrawButton(&button);
	button.title = "set";
	button.x = 60;
	GUI_DrawButton(&button);
	GUI_Refresh();								//刷新屏幕
	delay_ms(2000);

	GUI_Button_Selected(&button,0);
	GUI_Refresh();								//刷新屏幕
	delay_ms(2000);	
	GUI_ButtonDelete(&button);
}






