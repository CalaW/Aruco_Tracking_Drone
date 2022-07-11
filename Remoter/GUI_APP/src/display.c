#include "display.h"
#include "main_ui.h"
#include "trim_ui.h"
#include "debug_ui.h"
#include "menu_ui.h"
#include "match_ui.h"
#include "joystickCalib_ui.h"
#include "menuL1_item.h"
#include "menuL2_item.h"
#include "menuL3_item.h"
#include "reset_ui.h"
/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "task.h"

/********************************************************************************	 
 * 本程序只供学习使用，未经作者许可，不得用于其它任何用途
 * ALIENTEK MiniFly_Remotor
 * 界面显示代码	
 * 正点原子@ALIENTEK
 * 技术论坛:www.openedv.com
 * 创建日期:2018/6/1
 * 版本：V1.0
 * 版权所有，盗版必究。
 * Copyright(C) 广州市星翼电子科技有限公司 2014-2024
 * All rights reserved
********************************************************************************/

static enum ui_e show_ui = MAIN_UI;

/*设置显示界面*/
void setShow_ui(enum ui_e ui)
{
	show_ui = ui;
	GUI_ClearSCR();
}

/*显示任务*/
void displayTask(void* param)
{
	while(1)
	{
		vTaskDelay(50);
		switch(show_ui)
		{
			case MAIN_UI:
				main_ui();
				break;
			case TRIM_UI:
				trim_ui();
				break;
			case MENU_UI:
				Menu_Run();
				break;
			case DEBUG_UI:
				debug_ui();
				break;
			case JOYSTICK_CALIB_UI:
				joystickCalib_ui();
				break;
			case MATCH_UI:
				match_ui();
				break;
			case RESET_UI:
				reset_ui();
				break;
			default:break;
		}
		GUI_Refresh();
	}
}

/*界面显示初始化*/
void displayInit(void)
{
	//菜单初始化
	mainMenuInit();
	flymodeMenuInit();
	controlmodeMenuInit();
	languageMenuInit();
	flySpeedMenuInit();
	flipEnableMenuInit();
	expModuleMenuInit();
	ledringMenuInit();
	cameraMenuInit();
	vl53lxxMenuInit();
}








