#include <stdlib.h>
#include "joystick.h"
#include "adc.h"
#include "config_param.h"

/********************************************************************************	 
 * 本程序只供学习使用，未经作者许可，不得用于其它任何用途
 * ALIENTEK MiniFly_Remotor
 * 摇杆驱动代码	
 * 正点原子@ALIENTEK
 * 技术论坛:www.openedv.com
 * 创建日期:2018/6/1
 * 版本：V1.0
 * 版权所有，盗版必究。
 * Copyright(C) 广州市星翼电子科技有限公司 2014-2024
 * All rights reserved
********************************************************************************/

//摇杆中间软件死区值（ADC值）
#define MID_DB_THRUST		150	
#define MID_DB_YAW			300	
#define MID_DB_PITCH		150
#define MID_DB_ROLL			150

//摇杆上下量程死区值（ADC值）
#define DB_RANGE			10

//获取摇杆方向时定义在中间的范围值（ADC值）
#define DIR_MID_THRUST		800
#define DIR_MID_YAW			800
#define DIR_MID_PITCH		800
#define DIR_MID_ROLL		800


static bool isInit;
static joystickParam_t* jsParam;

/*去除死区函数*/
int deadband(int value, const int threshold)
{
	if (abs(value) < threshold)
	{
		value = 0;
	}
	else if (value > 0)
	{
		value -= threshold;
	}
	else if (value < 0)
	{
		value += threshold;
	}
	return value;
}

/*摇杆初始化*/
void joystickInit(void)
{
	if(isInit) return;
	Adc_Init();
	jsParam = &configParam.jsParam;
	isInit = true;
}

/*获取摇杆ADC值*/
void getFlyDataADCValue(joystickFlyui16_t *adcValue)
{
	adcValue->thrust = getAdcValue(ADC_THRUST);
	adcValue->roll = getAdcValue(ADC_ROLL);
	adcValue->pitch = getAdcValue(ADC_PITCH);
	adcValue->yaw = getAdcValue(ADC_YAW);
}

/*ADC值转换成飞控数据百分比*/
void ADCtoFlyDataPercent(joystickFlyf_t *percent)
{
	s16 adcValue;
	
	//THRUST
	adcValue = getAdcValue(ADC_THRUST) - jsParam->thrust.mid;
	adcValue = deadband(adcValue,MID_DB_THRUST);
	if(adcValue>=0)
		percent->thrust = (float)adcValue/(jsParam->thrust.range_pos-MID_DB_THRUST-DB_RANGE);
	else
		percent->thrust = (float)adcValue/(jsParam->thrust.range_neg-MID_DB_THRUST-DB_RANGE);
	
	//ROLL
	adcValue = getAdcValue(ADC_ROLL) - jsParam->roll.mid;
	adcValue = deadband(adcValue, MID_DB_ROLL);
	if(adcValue >= 0)
		percent->roll = (float)adcValue/(jsParam->roll.range_pos-MID_DB_ROLL-DB_RANGE);
	else
		percent->roll = (float)adcValue/(jsParam->roll.range_neg-MID_DB_ROLL-DB_RANGE);
	
	//PITCH
	adcValue = getAdcValue(ADC_PITCH) - jsParam->pitch.mid;
	adcValue = deadband(adcValue, MID_DB_PITCH);
	if(adcValue >= 0)
		percent->pitch = (float)adcValue/(jsParam->pitch.range_pos-MID_DB_PITCH-DB_RANGE);
	else
		percent->pitch = (float)adcValue/(jsParam->pitch.range_neg-MID_DB_PITCH-DB_RANGE);
	
	//YAW
	adcValue = getAdcValue(ADC_YAW) - jsParam->yaw.mid;
	adcValue = deadband(adcValue, MID_DB_YAW);
	if(adcValue >= 0)
		percent->yaw = (float)adcValue/(jsParam->yaw.range_pos-MID_DB_YAW-DB_RANGE);
	else
		percent->yaw = (float)adcValue/(jsParam->yaw.range_neg-MID_DB_YAW-DB_RANGE);
}

/*获取摇杆1方向*/
/*mode:0,不支持连续按;1,支持连续按*/
enum dir_e getJoystick1Dir(u8 mode)
{
	enum dir_e ret=CENTER;
	joystickFlyui16_t adcValue;
	static bool havebackToCenter = true;
	
	getFlyDataADCValue(&adcValue);
	if(mode) havebackToCenter = true;
	if(havebackToCenter == true)//摇杆回到过中间位置
	{
		if(adcValue.thrust > (jsParam->thrust.mid+DIR_MID_THRUST))
			ret = FORWARD;
		else if(adcValue.thrust < (jsParam->thrust.mid-DIR_MID_THRUST))
			ret = BACK;
		
		if(ret==BACK && adcValue.yaw>(jsParam->yaw.mid+DIR_MID_YAW))
			ret = BACK_RIGHT;
		else if(ret==BACK && adcValue.yaw<(jsParam->yaw.mid-DIR_MID_YAW))
			ret = BACK_LEFT;
		else if(adcValue.yaw > (jsParam->yaw.mid+DIR_MID_YAW))
			ret = RIGHT;
		else if(adcValue.yaw < (jsParam->yaw.mid-DIR_MID_YAW))
			ret = LEFT;
		
		havebackToCenter = false;//摇杆离开了中间位置
		if(ret == CENTER)//摇杆依然在中间位置
			havebackToCenter = true;
	}
	else if( adcValue.thrust >= (jsParam->thrust.mid-DIR_MID_THRUST) &&
			 adcValue.thrust <= (jsParam->thrust.mid+DIR_MID_THRUST) &&
			 adcValue.yaw >= (jsParam->yaw.mid-DIR_MID_YAW) &&
			 adcValue.yaw <= (jsParam->yaw.mid+DIR_MID_YAW) 
		   )//摇杆离开了中间位置，现在查询摇杆是否回中
	{
		havebackToCenter = true;
		ret = CENTER;
	}
	
	return ret;
}

/*获取摇杆2方向*/
/*mode:0,不支持连续按;1,支持连续按*/
enum dir_e getJoystick2Dir(u8 mode)
{
	enum dir_e ret = CENTER;
	joystickFlyui16_t adcValue;
	static bool havebackToCenter = true;
	
	getFlyDataADCValue(&adcValue);
	if(mode) havebackToCenter = true;
	if(havebackToCenter == true)//摇杆回到过中间位置
	{	
		if(adcValue.pitch > (jsParam->pitch.mid+DIR_MID_PITCH))
			ret = FORWARD;
		else if(adcValue.pitch < (jsParam->pitch.mid-DIR_MID_PITCH))
			ret = BACK;
		
		if(ret==BACK && adcValue.roll>(jsParam->roll.mid+DIR_MID_ROLL))
			ret = BACK_RIGHT;
		else if(ret==BACK && adcValue.roll<(jsParam->roll.mid-DIR_MID_ROLL))
			ret = BACK_LEFT;
		else if(adcValue.roll>(jsParam->roll.mid+DIR_MID_ROLL))
			ret = RIGHT;
		else if(adcValue.roll<(jsParam->roll.mid-DIR_MID_ROLL))
			ret = LEFT;

		havebackToCenter = false;//摇杆离开了中间位置
		if(ret == CENTER)//摇杆依然在中间位置
			havebackToCenter = true;
	}
	else if( adcValue.pitch >= (jsParam->pitch.mid-DIR_MID_PITCH) &&
			 adcValue.pitch <= (jsParam->pitch.mid+DIR_MID_PITCH) &&
			 adcValue.roll >= (jsParam->roll.mid-DIR_MID_ROLL) &&
			 adcValue.roll <= (jsParam->roll.mid+DIR_MID_ROLL) 
		   )//摇杆离开了中间位置，现在查询摇杆是否回中
	{
		havebackToCenter = true;
		ret = CENTER;
	}
	
	return ret;
}

