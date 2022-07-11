#ifndef  __SCROLLBAR_H
#define  __SCROLLBAR_H

#include "gui_config.h"

//定义一个滚动条类型
typedef  struct
{
 uint32_t x;             
 uint32_t y;
 uint32_t width;
 uint32_t height;

 uint32_t totalitems;  	  	//总条目数
 uint32_t itemsperpage;		//每页显示的条目数
 uint32_t topitem;			//最顶端的条目
 uint32_t scbbarlen;		//滚条长度	
	
}Scrollbar_Typedef;

void test_scrollbar(void);

void GUI_ScrollbarDelete(Scrollbar_Typedef *scbx);
void GUI_Scrollbar_SetPos(Scrollbar_Typedef *scbx);
void GUI_ScrollbarDraw(Scrollbar_Typedef *scbx);



#endif




