#ifndef __MESSAGEBOX_H
#define __MESSAGEBOX_H

#include "gui_config.h"

/* 定义窗口数据结构 */
typedef  struct
{  
	uint32_t  x;		// 窗口位置(左上角的x坐标)
	uint32_t  y;		// 窗口位置(左上角的y坐标)

	uint32_t  with;		// 窗口宽度
	uint32_t  hight;	// 窗口高度

	uint8_t   *title;	// 定义标题栏指针 
	uint8_t   *msg;		//消息
	uint8_t   *ok;		//确定
	uint8_t   *cancel;	//取消
} MessageBox_Typedef;


uint8_t GUI_MessageBoxDelete(MessageBox_Typedef* messagebox);
uint8_t GUI_MessageBoxDraw(MessageBox_Typedef* messagebox);
void GUI_MessageBoxButtonStatus(MessageBox_Typedef* messagebox, uint8_t status);
uint8_t GUI_MessageBoxResult(MessageBox_Typedef* messagebox);
void test_MessageBox(void);



#endif



