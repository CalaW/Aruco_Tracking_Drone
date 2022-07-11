#ifndef __GUI_MENU_H
#define __GUI_MENU_H
#include <stdint.h>
#include "gui_basic.h"
#include "gui_menu.h"
#include "windows.h"
#include "scrollbar.h"

/********************************************************************************	 
 * 本程序只供学习使用，未经作者许可，不得用于其它任何用途
 * ALIENTEK MiniFly_Remotor
 * 菜单显示界面代码	
 * 正点原子@ALIENTEK
 * 技术论坛:www.openedv.com
 * 创建日期:2018/6/1
 * 版本：V1.0
 * 版权所有，盗版必究。
 * Copyright(C) 广州市星翼电子科技有限公司 2014-2024
 * All rights reserved
********************************************************************************/

extern WINDOWS MenuWindow;
extern Scrollbar_Typedef MenuScrollbar;

void menu_test(void);
void Menu_Run(void);//运行菜单
void MenuHookFun(uint8_t key);//钩子函数
void exitMenu(void);
void gotoNextMenu(void);
void gotoLastMenu(void);


#endif /*__GUI_MENU_H*/
