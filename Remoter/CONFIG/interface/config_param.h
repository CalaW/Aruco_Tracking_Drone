#ifndef __CONFIG_PARAM_H
#define __CONFIG_PARAM_H
#include <stdbool.h>
#include "sys.h"
#include "24l01.h"
#include "remoter_ctrl.h"
#include "joystick.h"

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

#define BOOTLOADER_SIZE		(9*1024)		/*9K bootloader*/
#define CONFIG_PARAM_SIZE	(127*1024)		/*128K用于保存参数*/

#define CONFIG_PARAM_ADDR 	(FLASH_BASE + CONFIG_PARAM_SIZE)/*配置参数保存地址*/	
#define FIRMWARE_START_ADDR (FLASH_BASE + BOOTLOADER_SIZE)

/* 默认配置参数 */
#define  VERSION	13		/*表示版本为V1.3*/
#define  DISPLAY_LANGUAGE	SIMPLE_CHINESE

#define  RADIO_CHANNEL 		2				
#define  RADIO_DATARATE 	DATA_RATE_250K
#define  RADIO_ADDRESS 		0x123456789AULL

#define  FLIGHT_CTRL_MODE	ALTHOLD_MODE	
#define  FLIGHT_MODE		HEAD_LESS
#define  FLIGHT_SPEED		LOW_SPEED
#define  FLIP_SET			FLIP_DISABLE


enum ctrlMode
{
	ALTHOLD_MODE,
	MANUAL_MODE,
	THREEHOLD_MODE,
};

enum flightMode
{
	HEAD_LESS,
	X_MODE,
};

enum flightSpeed
{
	LOW_SPEED,
	MID_SPEED,
	HIGH_SPEED,
};

enum flipEnable
{
	FLIP_ENABLE,
	FLIP_DISABLE,
};

enum language
{
	SIMPLE_CHINESE,
	ENGLISH,
	COMPLEX_CHINESE,
};

/*飞行配置结构*/
typedef struct{
	enum ctrlMode ctrl;
	enum flightMode mode;
	enum flightSpeed speed;
	enum flipEnable flip;
}flightConfig_t;

/*无线配置结构*/
typedef struct{
	u8 channel;		
	enum nrfRate dataRate;
	u32 addressHigh;/*通信地址高4字节*/
	u32 addressLow;	/*通信地址低4字节*/
}radioConfig_t;

/*飞行微调结构*/
typedef __packed struct{
	float pitch;
	float roll;
}trim_t;

/*保存参数结构*/
typedef struct{
	u8 version;				/*软件版本号*/
	enum language language;	/*显示语言*/
	radioConfig_t radio;	/*无线配置参数*/
	flightConfig_t flight;	/*飞行配置参数*/
	joystickParam_t jsParam;/*摇杆校准参数*/
	trim_t trim;			/*姿态微调参数*/
	u8 cksum;				/*校验*/
} configParam_t;


extern configParam_t configParam;


void configParamInit(void);
void configParamTask(void* param);
void writeConfigParamToFlash(void);
void configParamReset(void);

#endif

