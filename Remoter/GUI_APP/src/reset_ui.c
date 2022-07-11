#include "reset_ui.h"
#include "windows.h"
#include "scrollbar.h"
#include "joystick.h"
#include "keyTask.h"
#include "display.h"
#include "messagebox.h"
#include "config_param.h"

/********************************************************************************	 
 * 本程序只供学习使用，未经作者许可，不得用于其它任何用途
 * ALIENTEK MiniFly_Remotor
 * 重置对话框界面代码	
 * 正点原子@ALIENTEK
 * 技术论坛:www.openedv.com
 * 创建日期:2018/6/1
 * 版本：V1.0
 * 版权所有，盗版必究。
 * Copyright(C) 广州市星翼电子科技有限公司 2014-2024
 * All rights reserved
********************************************************************************/

static bool isInit;

//重置对话框
MessageBox_Typedef ResetSystemMessageBox={
0,
0,	
128,
64,
"Reset",	
"System will be reset?",
"OK",
"Cancel"
};

const u8 *resetSystemLanguage[4][3]={
{
"重置",
"Reset",
"重置",
},
{
"确定恢复为默认设置？",
"Restore the default Settings?",
"確定恢復為默認設置？",
},
{
"确定",
"OK",
"確定",
},
{
"取消",
"Cancel",
"取消",
}
};

/*显示重置对话框*/
static void msgboxDispalyInit(void)
{
	if(isInit) return;
	
	ResetSystemMessageBox.title = (u8*)resetSystemLanguage[0][configParam.language];
	ResetSystemMessageBox.msg = (u8*)resetSystemLanguage[1][configParam.language];
	ResetSystemMessageBox.ok = (u8*)resetSystemLanguage[2][configParam.language];
	ResetSystemMessageBox.cancel = (u8*)resetSystemLanguage[3][configParam.language];
	GUI_MessageBoxDraw(&ResetSystemMessageBox);
	
	isInit = true;
}

/*重置界面*/
void reset_ui(void)
{
	enum dir_e joystick2;
	static int timeout;
	
	msgboxDispalyInit();
	joystick2 = getJoystick2Dir(0);
	if(joystick2 != CENTER)
	{
		timeout = 0;
	}
	switch(joystick2)
	{
		case RIGHT:  //ROLL向右
			GUI_MessageBoxButtonStatus(&ResetSystemMessageBox, 0);
			break;
			
		case LEFT:  //ROLL向左
			GUI_MessageBoxButtonStatus(&ResetSystemMessageBox, 1);
			break;
		default :break;
	}
	
	u8 keyState = getKeyState();
	if(keyState == KEY_J2_SHORT_PRESS)//按下摇杆键执行菜单对应的动作
	{	
		if(GUI_MessageBoxResult(&ResetSystemMessageBox) == 1)
		{
			configParamReset();
			INTX_DISABLE();/*禁止全局中断*/
			NVIC_SystemReset();	/*重启*/
		}
		isInit = false;
		setShow_ui(MAIN_UI);
	}
	if(timeout++ > 100)//超时退出菜单
	{
		timeout = 0;
		isInit = false;
		setShow_ui(MAIN_UI);
	}
}
