#ifndef __MENUL2_ITEM_H
#define __MENUL2_ITEM_H
#include "sys.h"
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

extern MenuItem_Typedef  controlmodeMenu[];
extern MenuItem_Typedef  flymodeMenu[];
extern MenuItem_Typedef  flyspeedMenu[];
extern MenuItem_Typedef  flipEnableMenu[];
extern MenuItem_Typedef  languageMenu[];
extern MenuItem_Typedef  expModuleMenu[];

extern const u8 radio_selected_img[32];
extern const u8  radio_unselected_img[32];


void flymodeMenuInit(void);
void controlmodeMenuInit(void);
void languageMenuInit(void);
void flySpeedMenuInit(void);
void flipEnableMenuInit(void);
void expModuleMenuInit(void);

#endif /*__MENUL2_ITEM_H*/

