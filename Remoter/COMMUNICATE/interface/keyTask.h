#ifndef __KEY_TASK_H
#define __KEY_TASK_H	 
#include "sys.h"
#include "key.h"

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

//按键状态
#define KEY_L_SHORT_PRESS	1	
#define KEY_L_LONG_PRESS	2
#define KEY_R_SHORT_PRESS	3	
#define KEY_R_LONG_PRESS	4	
#define KEY_J1_SHORT_PRESS	5	
#define KEY_J1_LONG_PRESS	6
#define KEY_J2_SHORT_PRESS	7
#define KEY_J2_LONG_PRESS	8

void keyTask(void* param);
u8 getKeyState(void);

#endif



