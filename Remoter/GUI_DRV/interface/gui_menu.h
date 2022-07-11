#ifndef  MENU_H
#define  MENU_H
#include <stdbool.h>
#include "gui_config.h"

//定义图标菜单32X32
typedef struct MenuItem
{  	
	int8_t cursorPosition;			//菜单显示时光标位置
	uint8_t menuItemCount;			//菜单包含项目数
	bool isSelect;					//显示菜单的状态 是否选中
	const uint8_t *icoSelected;		//菜单项目选中的ICO数据地址
	const uint8_t *icoUnselected;	//菜单项目没选中的ICO数据地址
	const uint8_t *title;			//菜单项目内容
	void (*Function)(void); 		//菜单对应的动作
	struct MenuItem* parentMenu;	//上一级菜单
	struct MenuItem* childrenMenu;	//下一级菜单
}MenuItem_Typedef;


uint8_t  GUI_MenuIcoDraw(MenuItem_Typedef *ico);
void GUI_MenuItem_Selected(uint32_t x, uint32_t y, MenuItem_Typedef *item, bool isSelect);
uint8_t  GUI_MenuItemDraw(uint32_t x, uint32_t y, MenuItem_Typedef *item);


#endif



