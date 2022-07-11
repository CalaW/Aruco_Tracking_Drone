#ifndef __MENUL3_ITEM_H
#define __MENUL3_ITEM_H
#include <stdint.h>
#include "gui_menu.h"

/********************************************************************************	 
 * 本程序只供学习使用，未经作者许可，不得用于其它任何用途
 * ALIENTEK MiniFly_Remotor
 * 二级菜单驱动代码		
 * 正点原子@ALIENTEK
 * 技术论坛:www.openedv.com
 * 创建日期:2018/6/1
 * 版本：V1.0
 * 版权所有，盗版必究。
 * Copyright(C) 广州市星翼电子科技有限公司 2014-2024
 * All rights reserved
********************************************************************************/

extern MenuItem_Typedef  ledringMenu[];
extern MenuItem_Typedef  cameraMenu[];
extern MenuItem_Typedef  opflowMenu[];

void ledringMenuInit(void);
void cameraMenuInit(void);
void vl53lxxMenuInit(void);

#endif /*__MENUL3_ITEM_H*/

