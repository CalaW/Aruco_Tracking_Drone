#include <string.h>
#include <stdio.h>
#include "joystickCalib_ui.h"
#include "keyTask.h"
#include "display.h"
#include "joystick.h"
#include "text.h"
#include "config_param.h"
#include "oled.h"

/********************************************************************************	 
 * 本程序只供学习使用，未经作者许可，不得用于其它任何用途
 * ALIENTEK MiniFly_Remotor
 * 摇杆校准界面代码	
 * 正点原子@ALIENTEK
 * 技术论坛:www.openedv.com
 * 创建日期:2018/6/1
 * 版本：V1.0
 * 版权所有，盗版必究。
 * Copyright(C) 广州市星翼电子科技有限公司 2014-2024
 * All rights reserved
********************************************************************************/

const unsigned char* stepLanguage1[3]={
{"1.摇动两个摇杆一圈"},
{"1.Rock two joysticks   a circle"},
{"1.搖動兩個搖桿一圈"},
};

const unsigned char* stepLanguage2[3]={
{"2.按KEY_R下一步"},
{"2.Press KEY_R next"},
{"2.按KEY_R下一步"},
};

const unsigned char* stepLanguage3[3]={
{"3.保持摇杆回中"},
{"3.Keep joysticks on    center"},
{"3.保持搖桿回中"},
};

const unsigned char* stepLanguage4[3]={
{"4.按KEY_R完成"},
{"4.Press KEY_R finish"},
{"4.按KEY_R完成"},
};

/*摇杆校准界面*/
void joystickCalib_ui(void)
{
	static u8 steps;
	char str[30];
	static s16 max[4],min[4] = {1000,1000,1000,1000};
	u8 y;
	joystickParam_t jsParam;
	joystickFlyui16_t adcValue;
	
	getFlyDataADCValue(&adcValue);
	if(steps==0)
	{
		oled_fill(0,0,127,38,0);
		y = 14;
		if(configParam.language == ENGLISH) 
			y = 26;
		show_str(0,0,stepLanguage1[configParam.language],12,12,1);
		show_str(0,y,stepLanguage2[configParam.language],12,12,1);
					
		//roll
		max[0] = MAX(max[0],adcValue.roll);
		min[0] = MIN(min[0],adcValue.roll);	
		//pitch
		max[1] = MAX(max[1],adcValue.pitch);
		min[1] = MIN(min[1],adcValue.pitch);
		//yaw
		max[2] = MAX(max[2],adcValue.yaw);
		min[2] = MIN(min[2],adcValue.yaw);
		//thrust
		max[3] = MAX(max[3],adcValue.thrust);
		min[3] = MIN(min[3],adcValue.thrust);
	}
	if(steps==1)
	{
		oled_fill(0,0,127,38,0);
		y = 14;
		if(configParam.language == ENGLISH) 
			y = 26;
		show_str(0,0,stepLanguage3[configParam.language],12,12,1);
		show_str(0,y,stepLanguage4[configParam.language],12,12,1);
		
		jsParam.roll.mid = adcValue.roll;
		jsParam.pitch.mid = adcValue.pitch;
		jsParam.yaw.mid = adcValue.yaw;
		jsParam.thrust.mid = adcValue.thrust;
		
		jsParam.roll.range_pos = max[0] - jsParam.roll.mid;
		jsParam.roll.range_neg = jsParam.roll.mid - min[0];	
		
		jsParam.pitch.range_pos = max[1] - jsParam.pitch.mid;
		jsParam.pitch.range_neg = jsParam.pitch.mid - min[1];
		
		jsParam.yaw.range_pos = max[2] - jsParam.yaw.mid;
		jsParam.yaw.range_neg = jsParam.yaw.mid - min[2];
		
		jsParam.thrust.range_pos = max[3] - jsParam.thrust.mid;
		jsParam.thrust.range_neg = jsParam.thrust.mid - min[3];
	}
	
	memset(str,0,22);
	sprintf(str,"THR:%4d    PIT:%4d",adcValue.thrust,adcValue.pitch);
	oled_showString(0,44,(u8*)str,6,8);
	memset(str,0,22);
	sprintf(str,"YAW:%4d    ROL:%4d",adcValue.yaw,adcValue.roll);
	oled_showString(0,54,(u8*)str,6,8);	
	
	u8 keyState = getKeyState();
	if(keyState == KEY_R_SHORT_PRESS)/*短按KEY_R进入下一步*/
	{
		steps++;
		if(steps==2)
		{
			steps=0;
			configParam.jsParam = jsParam;
			writeConfigParamToFlash();
			for(u8 i=0; i<4; i++)
			{
				min[i] = 1000;
				max[i] = 1000;
			}
			setShow_ui(MAIN_UI);
		}
	}
	else if(keyState == KEY_L_SHORT_PRESS)/*短按KEY_L退出校准*/
	{
		steps=0;
		setShow_ui(MAIN_UI);
	}
}
