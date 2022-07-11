#include "config_param.h"
#include "stmflash.h"
#include "joystick.h"
/* FreeRtos includes */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
/********************************************************************************	 
 * 本程序只供学习使用，未经作者许可，不得用于其它任何用途
 * ALIENTEK MiniFly
 * 配置参数驱动代码	
 * 正点原子@ALIENTEK
 * 技术论坛:www.openedv.com
 * 创建日期:2018/6/1
 * 版本：V1.0
 * 版权所有，盗版必究。
 * Copyright(C) 广州市星翼电子科技有限公司 2014-2024
 * All rights reserved
********************************************************************************/


configParam_t configParam;/*配置参数全局变量*/

static bool isInit;
static bool isConfigParamOK;

static configParam_t configParamDefault=
{
	.version = VERSION,
	.language = DISPLAY_LANGUAGE,
	
	.radio.channel = RADIO_CHANNEL,
	.radio.dataRate = RADIO_DATARATE,
	.radio.addressHigh = ((uint64_t)RADIO_ADDRESS >> 32),
	.radio.addressLow = (RADIO_ADDRESS & 0xFFFFFFFFULL),
	
	.flight.ctrl = FLIGHT_CTRL_MODE,
	.flight.mode = FLIGHT_MODE,
	.flight.speed = FLIGHT_SPEED,
	.flight.flip = FLIP_SET,
	
	.jsParam.pitch.mid = 2000,
	.jsParam.pitch.range_neg = 2000,
	.jsParam.pitch.range_pos = 2000,
	
	.jsParam.roll.mid = 2000,
	.jsParam.roll.range_neg = 2000,
	.jsParam.roll.range_pos =2000,
	
	.jsParam.yaw.mid = 2000,
	.jsParam.yaw.range_neg = 2000,
	.jsParam.yaw.range_pos =2000,
	
	.jsParam.thrust.mid = 2000,
	.jsParam.thrust.range_neg = 2000,
	.jsParam.thrust.range_pos =2000,
	
	.trim.pitch = 0.0,
    .trim.roll = 0.0,
};

/* 计算校验值 */
static u8 calculate_cksum(configParam_t* data)
{ 
	u8 cksum=0;	
	u8* c = (u8*)data;
	
	for (int i=0; i<sizeof(configParam_t); i++)
		cksum += *(c++);
	cksum -= data->cksum;
	
	return cksum;
}

/*配置参数初始化*/
void configParamInit(void)
{
	if(isInit) return;
	
	/* 读取配置参数 */
	STMFLASH_Read(CONFIG_PARAM_ADDR,(u16*)&configParam,sizeof(configParam)/2);
	
	if(configParam.version == VERSION)/*版本正确*/
	{
		u8 cksum = calculate_cksum(&configParam);
		if(cksum == configParam.cksum)/*校验正确*/
			isConfigParamOK = true;
		else
			isConfigParamOK = false;
	}
	else/*版本更新*/
	{
		isConfigParamOK = false;
	}
	
	/* 配置参数错误，写入默认参数 */
	if(isConfigParamOK == false)	
	{
		configParam = configParamDefault;
		writeConfigParamToFlash();
		isConfigParamOK=true;
	}
}

/*配置参数任务*/
void configParamTask(void* param)
{
	u8 cksum = 0;
	static u8 count = 0;
	while(1) 
	{	
		vTaskDelay(1000);
		cksum = calculate_cksum(&configParam);
		if(configParam.cksum != cksum)	
		{
			configParam.cksum = cksum;
			count = 1;
		}
		if(count)
		{
			count ++;
		}
		if(count > 6)/*参数有改变之后6S内不再变则写入flash*/
		{
			count = 0;
			STMFLASH_Write(CONFIG_PARAM_ADDR,(u16*)&configParam,sizeof(configParam)/2);
		}
	}
}

/*保存配置参数*/
void writeConfigParamToFlash(void)
{
	u8 cksum = calculate_cksum(&configParam);
	configParam.cksum = cksum;
	STMFLASH_Write(CONFIG_PARAM_ADDR,(u16*)&configParam,sizeof(configParam)/2);
}

/*配置参数复位为默认(摇杆校准参数不恢复)*/
void configParamReset(void)
{
	joystickParam_t jsParam;
	jsParam = configParam.jsParam;
	configParam = configParamDefault;
	configParam.jsParam = jsParam;
	writeConfigParamToFlash();
}
