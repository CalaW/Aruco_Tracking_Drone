#include <math.h>
#include <stdio.h>
#include <string.h>
#include "trim_ui.h"
#include "display.h"
#include "keyTask.h"
#include "oled.h"
#include "joystick.h"
#include "remoter_ctrl.h"
#include "radiolink.h"
#include "beep.h"
#include "main_ui.h"
#include "config_param.h"

/********************************************************************************	 
 * 本程序只供学习使用，未经作者许可，不得用于其它任何用途
 * ALIENTEK MiniFly_Remotor
 * 微调界面代码	
 * 正点原子@ALIENTEK
 * 技术论坛:www.openedv.com
 * 创建日期:2018/6/1
 * 版本：V1.0
 * 版权所有，盗版必究。
 * Copyright(C) 广州市星翼电子科技有限公司 2014-2024
 * All rights reserved
********************************************************************************/

#define TRIM_SCALE 	5
#define PI			(3.14159265)
#define DEG2RAD		0.017453293f	/* 度转弧度 π/180 */

extern float plane_yaw;

double mx_sin(double rad)
{   
	double sine;
	if (rad < 0)
		sine = rad*(1.27323954f + 0.405284735f * rad);
	else
		sine = rad * (1.27323954f - 0.405284735f * rad);
	if (sine < 0)
		sine = sine*(-0.225f * (sine + 1) + 1);
	else
		sine = sine * (0.225f *( sine - 1) + 1);
	return sine;
}

double my_sin(double rad)
{
	s8 flag = 1;

	if (rad >= PI)
	{
		rad -= PI;
		flag = -1;
	}

	return mx_sin(rad) * flag;
}

float my_cos(double rad)
{
	s8 flag = 1;
	rad += PI / 2.0;

	if (rad >= PI)
	{
		flag = -1;
		rad -= PI;
	}
	return my_sin(rad)*flag;
}

void trim_ui(void)
{
	char str[22];
	static u8 timeOut;
	joystickFlyf_t  percent;
	
	oled_showString(22,0,(u8*)"TRIM",6,12);
	memset(str,0,22);
	sprintf(str,"TRIM_PIT:%5.2f",configParam.trim.pitch);
	oled_showString(20,20,(u8*)str,6,8);
	memset(str,0,22);
	sprintf(str,"TRIM_ROL:%5.2f",configParam.trim.roll);
	oled_showString(20,40,(u8*)str,6,8);

	ADCtoFlyDataPercent(&percent);
	
	if(configParam.flight.mode == HEAD_LESS)//无头模式
	{	
		float yawRad = plane_yaw * DEG2RAD;
		//float cosy = cosf(yawRad);
		//float siny = sinf(yawRad);
		float cosy = my_cos(yawRad);
		float siny = my_sin(yawRad);
		float originalPitch =  percent.pitch/TRIM_SCALE;
		float originalRoll = percent.roll/TRIM_SCALE;
		configParam.trim.roll += (originalRoll * cosy - originalPitch * siny);
		configParam.trim.pitch += (originalPitch * cosy + originalRoll * siny);
	}
	else//有头模式
	{
		configParam.trim.pitch += percent.pitch/TRIM_SCALE;
		configParam.trim.roll += percent.roll/TRIM_SCALE;
	}
	configParam.trim.pitch = limit(configParam.trim.pitch, -5.0, 5.0);
	configParam.trim.roll = limit(configParam.trim.roll, -5.0, 5.0);
	
	// 按键处理
	u8 keyState = getKeyState();
	if(keyState==KEY_J2_SHORT_PRESS || (++timeOut>60))//短按KEY_R或超时退出微调界面
	{
		timeOut = 0;
		setTrimFlag(false);/*清除微调标志*/
		stopBeepAcktion(TRIM_BEEP);/*关闭蜂鸣器*/
		setShow_ui(MAIN_UI);
	}
}

