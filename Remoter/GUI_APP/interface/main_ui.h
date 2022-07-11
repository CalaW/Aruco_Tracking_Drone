#ifndef __MAIN_UI_H
#define __MAIN_UI_H
#include <stdint.h>
#include <stdbool.h>
#include "atkp.h"
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

void main_ui(void);
void miniFlyMsgACKProcess(atkp_t* p);
	
void setRCLock(bool set);
bool getRCLock(void);

void setTrimFlag(bool set);
bool getTrimFlag(void);

void setMFLowPower(bool set);
bool getMFLowPower(void);

void setIsMFCanFly(bool set);
bool getIsMFCanFly(void);
#endif /*__MAIN_UI_H*/
