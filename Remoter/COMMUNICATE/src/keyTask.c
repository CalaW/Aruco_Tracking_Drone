#include <stdbool.h>
#include "keyTask.h"
/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "task.h"

/********************************************************************************	 
 * 本程序只供学习使用，未经作者许可，不得用于其它任何用途
 * ALIENTEK MiniFly_Remotor
 * 按键任务驱动代码	
 * 正点原子@ALIENTEK
 * 技术论坛:www.openedv.com
 * 创建日期:2018/6/1
 * 版本：V1.0
 * 版权所有，盗版必究。
 * Copyright(C) 广州市星翼电子科技有限公司 2014-2024
 * All rights reserved
********************************************************************************/

#define LONG_PRESS_COUNT 	500	/*判断为长按时间（ms）*/

#define PRESSED		0
#define RELEASED	1

static bool keyL_pressed;
static bool keyR_pressed;
static bool keyJ1_pressed;
static bool keyJ2_pressed;
static u8 keyState;
static u32 pressedTime;

void keyTask(void* param)
{
	while(1)
	{
		vTaskDelay(20);
		
		if(keyL_pressed==false && READ_KEY_L()==PRESSED)
		{
			keyL_pressed = true;
			pressedTime = xTaskGetTickCount();
		}
		if(keyR_pressed==false && READ_KEY_R()==PRESSED)
		{
			keyR_pressed = true;
			pressedTime = xTaskGetTickCount();
		}
		if(keyJ1_pressed==false && READ_KEY_J1()==PRESSED)
		{
			keyJ1_pressed = true;
			pressedTime = xTaskGetTickCount();
		}
		if(keyJ2_pressed==false && READ_KEY_J2()==PRESSED)
		{
			keyJ2_pressed = true;
			pressedTime = xTaskGetTickCount();
		}
		
		if(keyL_pressed==true)
		{
			if(READ_KEY_L()==RELEASED)
				keyL_pressed = false;
			if((xTaskGetTickCount() - pressedTime) > LONG_PRESS_COUNT)
				keyState = KEY_L_LONG_PRESS;
			else if(READ_KEY_L()==RELEASED)
				keyState = KEY_L_SHORT_PRESS;
		}
		if(keyR_pressed==true)
		{
			if(READ_KEY_R()==RELEASED)
				keyR_pressed = false;
			if((xTaskGetTickCount() - pressedTime) > LONG_PRESS_COUNT)
				keyState = KEY_R_LONG_PRESS;
			else if(READ_KEY_R()==RELEASED)
				keyState = KEY_R_SHORT_PRESS;
		}
		if(keyJ1_pressed==true)
		{
			if(READ_KEY_J1()==RELEASED)
				keyJ1_pressed = false;
			if((xTaskGetTickCount() - pressedTime) > LONG_PRESS_COUNT)
				keyState = KEY_J1_LONG_PRESS;
			else if(READ_KEY_J1()==RELEASED)
				keyState = KEY_J1_SHORT_PRESS;
		}
		if(keyJ2_pressed==true)
		{
			if(READ_KEY_J2()==RELEASED)
				keyJ2_pressed = false;
			if((xTaskGetTickCount() - pressedTime) > LONG_PRESS_COUNT)
				keyState = KEY_J2_LONG_PRESS;
			else if(READ_KEY_J2()==RELEASED)
				keyState = KEY_J2_SHORT_PRESS;
		}

	}
}

//读取按键状态
u8 getKeyState(void)
{
	u8 temp;
	temp = keyState;
	keyState = 0;//读取按键之后清零
	return temp;
}


















