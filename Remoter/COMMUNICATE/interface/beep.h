#ifndef __BEEP_H
#define __BEEP_H	 
#include "sys.h"
#include <stdbool.h>
/********************************************************************************	 
 * 本程序只供学习使用，未经作者许可，不得用于其它任何用途
 * ALIENTEK MiniFly_Remotor
 * 蜂鸣器驱动代码	
 * 正点原子@ALIENTEK
 * 技术论坛:www.openedv.com
 * 创建日期:2018/6/1
 * 版本：V1.0
 * 版权所有，盗版必究。
 * Copyright(C) 广州市星翼电子科技有限公司 2014-2024
 * All rights reserved
********************************************************************************/

#define  BEEP  PCout(14)


typedef struct 
{
	bool isActive;
	bool isLoop;
	u16 waitMS;
}beepSeq_t;


enum beepAction
{
	TRIM_BEEP = 0,
	FLIP_BEEP,
	LOWPOWER_BEEP,
	CANFLY_BEEP,
};


void beepInit(void);
void runBeepAcktion(enum beepAction runaction);
void stopBeepAcktion(enum beepAction runaction);
#endif
