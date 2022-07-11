#include "menu_ui.h"
#include "windows.h"
#include "scrollbar.h"
#include "joystick.h"
#include "keyTask.h"
#include "display.h"
#include "menuL1_item.h"
#include "menuL2_item.h"

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

//菜单的位置
#define   MEMU_POSX_1    20
#define   MEMU_POSY_1    19

MenuItem_Typedef*  CurMenu = mainMenu;
MenuItem_Typedef*  CurItem;

static int selected;
static int cur_sequence; //某一级菜单选中项目的序号
static bool isChangeMenu = true;
const u8* defaultTitle = "ATK MiniFly";

//窗体
WINDOWS MenuWindow={
.x = 0,
.y = 0,	
.width = 128,
.height = 64,
.itemsperpage = 3,
.topitem = 0,
.title = "ATK MiniFly",
};

//主窗体滚动条
Scrollbar_Typedef MenuScrollbar={
.x = 118,
.y = 14,
.width = 10,
.height = 50,
.itemsperpage = 3,
.topitem = 0,
.scbbarlen = 0,
};

u8 getMenuSelectitem(MenuItem_Typedef menu[])
{
	int i;
	for(i=0; i< menu->menuItemCount; i++)
	{
		if(menu[i].isSelect == true) 
			break;
	}
	return i;
}

void exitMenu(void)
{
	CurMenu = mainMenu;
	MenuWindow.title = defaultTitle;
	setShow_ui(MAIN_UI);
	isChangeMenu = true;
}

void gotoNextMenu(void)	
{
	if(CurItem->childrenMenu != NULL)
	{
		CurMenu = CurItem->childrenMenu;
		MenuWindow.title = CurItem->title +2;
		isChangeMenu = true;
	}
	else if(CurItem->Function!=NULL && CurItem->parentMenu==NULL)
	{
		CurItem->Function();
	}
}

void gotoLastMenu(void)
{
	if(CurItem->parentMenu != NULL)
	{
		//恢复默认选中
		CurItem->isSelect = false;
		CurItem = CurMenu + selected;
		CurItem->isSelect = true;
		
		CurMenu = CurItem->parentMenu;
		if(CurMenu == mainMenu)
			MenuWindow.title = defaultTitle;
		else
			MenuWindow.title = CurItem->title +2;
		isChangeMenu = true;
	}
	else
	{
		exitMenu();
	}
}

/*显示菜单*/
void DisplayMenuInit(MenuItem_Typedef* menu)
{
	u8 topitem;
	u8 showItems;
	
	if(isChangeMenu == false) return;
	
	selected = getMenuSelectitem(menu);
	cur_sequence = selected;
	if(selected < menu->cursorPosition)
		menu->cursorPosition = 0;
	topitem = selected - menu->cursorPosition;
	if(menu->menuItemCount <= MenuWindow.itemsperpage)
		topitem = 0;

	MenuWindow.topitem = topitem;
	GUI_WindowsDraw(&MenuWindow);
	
	MenuScrollbar.topitem = topitem;
	MenuScrollbar.totalitems = menu->menuItemCount;
	GUI_ScrollbarDraw(&MenuScrollbar);
	
	showItems = MenuWindow.itemsperpage;
	if(menu->menuItemCount < MenuWindow.itemsperpage)
		showItems = menu->menuItemCount;
	for(int i=0; i<showItems; i++)
	{
		MenuItem_Typedef* Item = &menu[topitem+i];
		GUI_MenuItemDraw(MEMU_POSX_1, MEMU_POSY_1+i*15, Item);
	}
	CurItem = menu + cur_sequence;
	CurItem->isSelect = true;
	
	GUI_Refresh();//刷新屏幕
	
	isChangeMenu = false;
}

/*菜单运行*/
void Menu_Run(void)
{
	u8 keyState;
	enum dir_e joystick2;
	static int timeout;
	u8 showItems;
	
	DisplayMenuInit(CurMenu);
	
	joystick2 = getJoystick2Dir(0);
	if(joystick2 != CENTER)
	{
		timeout = 0;
	}
	switch(joystick2)
	{
		case BACK:	//PITCH向后
			//清除窗口内容
			GUI_RectangleFill(1, 18, 117, 62, 0);
			CurItem = CurMenu + cur_sequence;
			CurItem->isSelect = false;
		
			//菜单项目序号++
			cur_sequence++;
			if(cur_sequence >= CurMenu->menuItemCount-1)
				cur_sequence = CurMenu->menuItemCount-1;
			CurItem = CurMenu + cur_sequence;
			CurItem->isSelect = true;
			
			//光标位置++
			CurMenu->cursorPosition++;
			if(CurMenu->menuItemCount <= MenuWindow.itemsperpage)
			{
				showItems = CurMenu->menuItemCount;
				if(CurMenu->cursorPosition >= CurMenu->menuItemCount)
					CurMenu->cursorPosition = CurMenu->menuItemCount-1;
			}
			else
			{
				showItems = MenuWindow.itemsperpage;
				if(CurMenu->cursorPosition > MenuWindow.itemsperpage-1)
				{		
					CurMenu->cursorPosition= MenuWindow.itemsperpage-1;
					if(MenuWindow.topitem < CurMenu->menuItemCount-MenuWindow.itemsperpage)
						MenuWindow.topitem++;
				}
			}
			for(int i=0; i<showItems; i++)
			{
				MenuItem_Typedef* Item = CurMenu+MenuWindow.topitem+i;
				GUI_MenuItemDraw(MEMU_POSX_1, MEMU_POSY_1+i*15, Item);
			}
			MenuScrollbar.topitem = cur_sequence;
			GUI_Scrollbar_SetPos(&MenuScrollbar);				
			break;
					
		case FORWARD:	//PITCH向前
			//清除窗口内容
			GUI_RectangleFill(1, 18, 117, 62, 0);
			CurItem = CurMenu + cur_sequence;
			CurItem->isSelect = false;
		
			//菜单项目序号--
			cur_sequence--;
			if(cur_sequence <= 0)
				cur_sequence = 0;
			CurItem = CurMenu + cur_sequence;
			CurItem->isSelect = true;
			
			//光标位置--
			CurMenu->cursorPosition--;
			if(CurMenu->menuItemCount <= MenuWindow.itemsperpage)
			{	
				showItems = CurMenu->menuItemCount;
				if(CurMenu->cursorPosition < 0)
					CurMenu->cursorPosition = 0;
			}
			else
			{	
				showItems = MenuWindow.itemsperpage;				
				if(CurMenu->cursorPosition < 0)
				{		
					CurMenu->cursorPosition = 0;
					if(MenuWindow.topitem > 0)
						MenuWindow.topitem--;
				}
			}
			for(int i=0; i<showItems; i++)
			{
				MenuItem_Typedef* Item = CurMenu+MenuWindow.topitem+i;
				GUI_MenuItemDraw(MEMU_POSX_1, MEMU_POSY_1+i*15, Item);
			}
			MenuScrollbar.topitem = cur_sequence;
			GUI_Scrollbar_SetPos(&MenuScrollbar);      					
			break;
			
		case RIGHT:  //ROLL向右
			gotoNextMenu();//进入下一级菜单
			break;
			
		case LEFT:  //ROLL向左
			gotoLastMenu();//进入上一级菜单
			break;
		
		default :break;
	}
	
	keyState = getKeyState();
	/*按下摇杆键执行菜单对应的动作*/
	if(keyState == KEY_J2_SHORT_PRESS)
	{	
		if(CurItem->Function != NULL)
		{
			CurItem->Function();
		}
	}
	/*超时退出菜单*/
	if(timeout++ > 100)
	{
		timeout = 0;
		if(CurItem->parentMenu != NULL)
		{
			//恢复默认选中
			CurItem->isSelect = false;
			CurItem = CurMenu + selected;
			CurItem->isSelect = true;
		}
		exitMenu();
	}
}

