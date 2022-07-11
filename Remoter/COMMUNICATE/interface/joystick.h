#ifndef __JOYSTICK_H
#define __JOYSTICK_H
#include "sys.h"
#include <stdbool.h>

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

#define MAX(a,b) a>=b? a:b
#define MIN(a,b) a<=b? a:b

enum dir_e
{
	CENTER,
	FORWARD,
	BACK,
	LEFT,
	RIGHT,
	BACK_LEFT,
	BACK_RIGHT,
};

//摇杆单方向量程参数
struct range_s
{
	s16 range_pos;	//摇杆正量程
	u16 mid;
	s16 range_neg;	//摇杆负量程
};

//摇杆量程参数
typedef struct
{
	struct range_s thrust;
	struct range_s roll;
	struct range_s pitch;
	struct range_s yaw;
}joystickParam_t;

//飞控数据结构
typedef struct 
{
	float roll;
	float pitch;
	float yaw;
	float thrust;
}joystickFlyf_t;

//飞控数据结构
typedef struct 
{
	u16 roll;
	u16 pitch;
	u16 yaw;
	u16 thrust;
}joystickFlyui16_t;


void joystickInit(void);

void getFlyDataADCValue(joystickFlyui16_t *adcValue);

void ADCtoFlyDataPercent(joystickFlyf_t *flyDataPercent);

void joystcikParamSet(joystickParam_t set);

enum dir_e getJoystick1Dir(u8 mode);

enum dir_e getJoystick2Dir(u8 mode);


#endif /*__JOYSTICK_H*/
