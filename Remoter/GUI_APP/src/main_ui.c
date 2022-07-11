#include <stdio.h>
#include "main_ui.h"
#include "display.h"
#include "font.h"
#include "joystick.h"
#include "keyTask.h"
#include "beep.h"
#include "config_param.h"
#include "oled.h"
#include "atkp.h"
#include "radiolink.h"
#include "adc.h"
#include "menuL1_item.h"
#include "text.h"
#include "remoter_ctrl.h"
#include "gui_basic.h"
/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "task.h"
/********************************************************************************	 
 * 本程序只供学习使用，未经作者许可，不得用于其它任何用途
 * ALIENTEK MiniFly_Remotor
 * 主界面代码	
 * 正点原子@ALIENTEK
 * 技术论坛:www.openedv.com
 * 创建日期:2018/6/1
 * 版本：V1.0
 * 版权所有，盗版必究。
 * Copyright(C) 广州市星翼电子科技有限公司 2014-2024
 * All rights reserved
********************************************************************************/

#define  LOW_POWER  	3.25f
#define  FULL_POWER		4.15f

static bool RCLock = true;
static bool trimFlag = false;
static bool flipFlag = false;
static bool isMFSelfTestPass = true;

static bool RCLowPower = false;
static bool RCLowPowerOld = false;
static bool MFLowPower = false;
static bool MFLowPowerOld = false;
static bool isMFCanFly = false;
static bool isMFCanFlyOld = false;

static s8 batPercent;
static MiniFlyMsg_t msg;
static TickType_t dispalyBatTime;
static TickType_t getMFMsgTime;
static enum expModuleID moduleID = NO_MODULE;

extern float plane_bat;//四轴返回的电量
extern u8 rssi;//四轴返回的无线信号值


static void flight(void)
{
	static u8 flipBeepTimeOut;
	
	oled_fill(0,24,127,63,0);
	
	/*遥控器解锁、加锁*/
	if(getJoystick1Dir(1)==BACK_LEFT && getJoystick2Dir(1)==BACK_RIGHT)
	{
		RCLock = false;
	}
	if(getJoystick1Dir(1)==BACK_RIGHT && getJoystick2Dir(1)==BACK_LEFT)
	{
		RCLock = true;
	}
	
	/*显示控制模式、锁头、飞行模式*/
	GUI_Circle(20,43,16,1);
	GUI_Circle(20,43,17,1);
	if(configParam.flight.ctrl == ALTHOLD_MODE)
		oled_showChar(14,30,'H',12,24,1);
	else if(configParam.flight.ctrl == MANUAL_MODE)
		oled_showChar(14,30,'M',12,24,1);
	else 
		oled_showChar(14,30,'T',12,24,1);
	oled_showPicture(49,43-15,bmp_lock[RCLock],30,30);
	oled_showPicture(94,43-15,bmp_flightMode[configParam.flight.mode],30,30);
	GUI_Circle(94+15,43,16,1);
	GUI_Circle(94+15,43,17,1);
	
	/*按键处理*/
	u8 keyState = getKeyState();
	if(keyState == KEY_J1_SHORT_PRESS)//一键起飞、降落
	{
		if( RCLock==false && isMFCanFly==true &&
			(configParam.flight.ctrl == ALTHOLD_MODE || 
			configParam.flight.ctrl == THREEHOLD_MODE)
		  )
		{
			sendRmotorCmd(CMD_FLIGHT_LAND, NULL);
		}
	}
	else if(keyState == KEY_L_SHORT_PRESS)//紧急停机
	{
		sendRmotorCmd(CMD_EMER_STOP, NULL);
	}
	else if(keyState == KEY_J2_SHORT_PRESS)//进入微调界面
	{
		if( RCLock==false && isMFCanFly==true)
		{
			trimFlag = true;
			runBeepAcktion(TRIM_BEEP);
			setShow_ui(TRIM_UI);
		}
	}
	else if(keyState == KEY_R_LONG_PRESS)//调试界面
	{
		//if(RCLock == false)
		{
			setShow_ui(DEBUG_UI);
		}
	}
	else if(keyState == KEY_R_SHORT_PRESS)//一键翻滚
	{
		if( RCLock==false && isMFCanFly==true && 
			MFLowPower==false && configParam.flight.flip==FLIP_ENABLE  
			&& configParam.flight.ctrl != THREEHOLD_MODE) /*光流定点模式，关闭空翻*/
		{
			flipFlag = !flipFlag;
			flipBeepTimeOut = 0;
			if(flipFlag == true)
				runBeepAcktion(FLIP_BEEP);
			else
				stopBeepAcktion(FLIP_BEEP);
		}
	}
	else if(keyState == KEY_J2_LONG_PRESS)
	{
		if(RCLock == true)
		{
			setShow_ui(MENU_UI);
		}
	}
	
	/*事件处理*/
	if(flipFlag == true)
	{
		enum dir_e flipDir = getJoystick2Dir(0);
		
		if(flipDir != CENTER)
		{
			flipFlag = false;
			stopBeepAcktion(FLIP_BEEP);
			sendRmotorCmd(CMD_FLIP, flipDir);
		}
		else if(++flipBeepTimeOut>30)
		{
			flipFlag = false;
			stopBeepAcktion(FLIP_BEEP);
			flipBeepTimeOut = 0;
		}
	}
}

void main_ui(void)
{
	u8 rssi_level;
	static float remoterBat;
	
	/*电量百分比显示*/
	if(xTaskGetTickCount() >= dispalyBatTime+1000)/*遥控器电量1000ms更新一次*/
	{
		dispalyBatTime = xTaskGetTickCount();
		remoterBat = ((float)getAdcValue(ADC_BAT)/4095.0)*3*2;
		if(remoterBat < LOW_POWER)
			RCLowPower = true;
		else 
			RCLowPower = false;	
	}
	if(RCLock == true)/*显示遥控器电量*/
		batPercent = (remoterBat-LOW_POWER)/(FULL_POWER-LOW_POWER)*100;
	else/*显示四轴电量*/
		batPercent = (plane_bat-LOW_POWER)/(FULL_POWER-LOW_POWER)*100;
	batPercent = limit(batPercent, 0, 99);
	
	/*信号显示*/
	if(rssi < 80) rssi_level = 5;/*信号值小80db,信号为5格*/
	else if(rssi < 85) rssi_level = 4;
	else if(rssi < 90) rssi_level = 3;
	else if(rssi < 95) rssi_level = 2;
	else if(rssi >= 95) rssi_level = 1;
	if(radioinkConnectStatus() == false) rssi_level = 0;
		
	/*显示图片*/
	oled_showPicture(0,0,bmp_rssi[rssi_level],24,22);
	oled_showPicture(24,0,bmp_logo,80,22);
	oled_showPicture(106,4,bmp_battery[batPercent*12/99],10,16);
	oled_showChar(119,13,'%',6,8,1);
	oled_showNum(116,4,batPercent,2,6,8);
	
	/*四轴自检*/
	if(isMFSelfTestPass == true)
	{
		flight();/*自检成功可飞行*/
	}
	else/*自检出错显示出错传感器*/
	{
		char str[22];
		oled_fill(0,24,127,63,0);
		if(msg.mpu_selfTest == true)
			oled_showString(0,24,(u8*)"MPU9250 CHECK [OK]",6,12);
		else
			oled_showString(0,24,(u8*)"MPU9250 CHECK [FAIL]",6,12);
			
		if(msg.baro_slfTest == true)
			oled_showString(0,38,(u8*)"BARO CHECK [OK]",6,12);
		else
			oled_showString(0,38,(u8*)"BARO CHECK [FAIL]",6,12);
		sprintf(str,"MINIFLY FIRMWARE V%1d.%1d",msg.version/10,msg.version%10);
		oled_showString(0,52,(u8*)str,6,12);
	}
	
	/*低电量报警*/
	if(RCLowPower!=RCLowPowerOld || MFLowPower!=MFLowPowerOld)
	{
		if(RCLowPower==true || MFLowPower==true)
			runBeepAcktion(LOWPOWER_BEEP);
		else 
			stopBeepAcktion(LOWPOWER_BEEP);
		RCLowPowerOld = RCLowPower;
		MFLowPowerOld = MFLowPower;
	}
	if(MFLowPower==true && radioinkConnectStatus()==false)
	{
		MFLowPower = false;
	}
	
	/*四轴可飞行提示报警*/
	if(isMFCanFly != isMFCanFlyOld)
	{
		if(isMFCanFly == true)
		{
			runBeepAcktion(CANFLY_BEEP);
		}
		isMFCanFlyOld = isMFCanFly;
	}
	/*500ms获取一次MFMsg*/
	if(xTaskGetTickCount() >= getMFMsgTime+500)
	{
		getMFMsgTime = xTaskGetTickCount();
		sendRmotorCmd(CMD_GET_MSG, NULL);
	}
	
	/*通信失败设置扩展模块菜单为NO_MODULE*/
	if(radioinkConnectStatus() == false)
	{
		expModuleMenuSet(NO_MODULE);
	}
}

static u8 reReceiveTimes = 3;	/*重接收次数*/

/*四轴MSG处理*/
void miniFlyMsgACKProcess(atkp_t* p)
{
	msg = *(MiniFlyMsg_t*)(p->data+1);
	if(msg.mpu_selfTest==false || msg.baro_slfTest==false) 
	{
		isMFSelfTestPass = false;
	}
	else
	{
		isMFSelfTestPass = true;
	}
	isMFCanFly = msg.isCanFly;
	MFLowPower = msg.isLowpower;
	moduleID = msg.moduleID;
	moduleID = moduleID;
	expModuleMenuSet(msg.moduleID);
	
	if(isMFCanFly == true)	/*校准通过之后接收微调值*/
	{
		if(reReceiveTimes > 0)
		{
			reReceiveTimes--;
			configParam.trim.roll = msg.trimRoll;	/*roll微调*/
			configParam.trim.pitch = msg.trimPitch;	/*pitch微调*/
		}
	}else
	{
		reReceiveTimes = 3;	/*重接收次数*/
	}	
}

/*RCLock属性*/
void setRCLock(bool set)
{
	RCLock = set;
}
bool getRCLock(void)
{
	return RCLock;
}

/*trimFlag属性*/
void setTrimFlag(bool set)
{
	trimFlag = set;
}
bool getTrimFlag(void)
{
	return trimFlag;
}

/*MFLowPower属性*/
void setMFLowPower(bool set)
{
	MFLowPower = set;
}
bool getMFLowPower(void)
{
	return MFLowPower;
}

/*isMFCanFly属性*/
void setIsMFCanFly(bool set)
{
	isMFCanFly = set;
}
bool getIsMFCanFly(void)
{
	return isMFCanFly;
}
