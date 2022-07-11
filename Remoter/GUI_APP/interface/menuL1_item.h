#ifndef __MENUL1_ITEM_H
#define __MENUL1_ITEM_H
#include "sys.h"
#include "gui_menu.h"
#include "remoter_ctrl.h"

/********************************************************************************	 
 * 本程序只供学习使用，未经作者许可，不得用于其它任何用途
 * ALIENTEK MiniFly_Remotor
 * 一级菜单驱动代码	
 * 正点原子@ALIENTEK
 * 技术论坛:www.openedv.com
 * 创建日期:2018/6/1
 * 版本：V1.0
 * 版权所有，盗版必究。
 * Copyright(C) 广州市星翼电子科技有限公司 2014-2024
 * All rights reserved
********************************************************************************/

extern MenuItem_Typedef  mainMenu[];
extern const u8 finger_img[32];

void mainMenuInit(void);
void expModuleMenuSet(enum expModuleID id);

#endif /*__MENUL1_ITEM_H*/
