#include <string.h>
#include <stdio.h>
#include "debug_ui.h"
#include "oled.h"
#include "keyTask.h"
#include "display.h"
#include "joystick.h"
#include "remoter_ctrl.h"
#include "radiolink.h"
#include "main_ui.h"

/********************************************************************************	 
 * 本程序只供学习使用，未经作者许可，不得用于其它任何用途
 * ALIENTEK MiniFly_Remotor
 * 调试界面代码	
 * 正点原子@ALIENTEK
 * 技术论坛:www.openedv.com
 * 创建日期:2018/6/1
 * 版本：V1.0
 * 版权所有，盗版必究。
 * Copyright(C) 广州市星翼电子科技有限公司 2014-2024
 * All rights reserved
********************************************************************************/

void debug_ui(void)
{
	extern float plane_yaw,plane_roll,plane_pitch;
	extern float plane_bat;
	extern u8 rssi;
	char str[22];
	joystickFlyf_t flyControl;
	
	flyControl = getFlyControlData();
	
	memset(str,0,22);
	sprintf(str," SEND --->  FNUM:%3d",radioinkFailRxcount());
	oled_showString(0,0,(u8*)str,6,12);
	memset(str,0,22);
	sprintf(str,"THR:%4.0f    PIT:%5.1f",flyControl.thrust,flyControl.pitch);
	oled_showString(0,13,(u8*)str,6,8);
	memset(str,0,22);
	sprintf(str,"YAW:%4.0f    ROL:%5.1f",flyControl.yaw,flyControl.roll);
	oled_showString(0,22,(u8*)str,6,8);
	
	memset(str,0,22);
	sprintf(str," BACK <---  RSSI:%3d",rssi);
	oled_showString(0,34,(u8*)str,6,12);
	memset(str,0,22);
	sprintf(str,"BAT:%4.2f    PIT:%5.1f",plane_bat,plane_pitch);
	if(strlen(str)>21) str[21]='\0';
	oled_showString(0,47,(u8*)str,6,8);
	memset(str,0,22);
	sprintf(str,"YAW:%4.0f    ROL:%5.1f",plane_yaw,plane_roll);
	if(strlen(str)>21) str[21]='\0';
	oled_showString(0,56,(u8*)str,6,8);
	
	/*按键处理*/
	u8 keyState = getKeyState();
	if(keyState == KEY_L_SHORT_PRESS)/*紧急停机*/
	{
		sendRmotorCmd(CMD_EMER_STOP, NULL);
	}
	else if(keyState==KEY_R_SHORT_PRESS)/*退出调试界面*/
	{
		setShow_ui(MAIN_UI);
	}
}

