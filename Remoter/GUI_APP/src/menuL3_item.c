#include "menuL3_item.h"
#include "menuL2_item.h"
#include "menu_ui.h"
#include "windows.h"
#include "scrollbar.h"
#include "joystick.h"
#include "keyTask.h"
#include "display.h"
#include "OLED.h"
#include "config_param.h"
#include "24l01.h"
#include "menuL1_item.h"
#include "atkp.h"
#include "remoter_ctrl.h"

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

/*菜单项数*/
#define  LED_RING_MENU_NUM   	10
#define  WIFI_CAMERA_MENU_NUM  	2
#define  OPTICAL_FLOW_MENU_NUM  2

MenuItem_Typedef  ledringMenu[LED_RING_MENU_NUM];
MenuItem_Typedef  cameraMenu[WIFI_CAMERA_MENU_NUM];
MenuItem_Typedef  opflowMenu[OPTICAL_FLOW_MENU_NUM];

const u8* ledringLanguage[3][LED_RING_MENU_NUM]={
{
"关闭",
"颜色测试",
"姿态感应",
"陀螺感应",
"闪烁警报",
"手电筒",
"呼吸灯",
"流水灯1",
"流水灯2",
"流水灯3",
},
{
"Turn off",
"Color Test",
"Attitude induce",
"Gyro induce",
"Blink LED",
"Flashlight",
"Breathing LED",
"Spin Effect1",
"Spin Effect2",
"Spin Effect3",
},
{
"關閉",
"顏色測試",
"姿態感應",
"陀螺感應",
"閃爍警報",
"手電筒",
"呼吸燈",
"流水燈1",
"流水燈2",
"流水燈3",
},
};
const u8* cameraLanguage[3][WIFI_CAMERA_MENU_NUM]={
{
"开启",
"关闭",
},
{
"Turn On",
"Turn Off",
},
{
"開啟",
"關閉",
}	
};
const u8* opflowLanguage[3][OPTICAL_FLOW_MENU_NUM]={
{
"开启激光",
"关闭激光",
},
{
"Turn On Vl53lxx",
"Turn Off Vl53lxx",
},
{
"開啟激光",
"關閉激光",
}	
};


static void setLedringEffect0(void)
{
	exitMenu();
	sendRmotorCmd(CMD_LEDRING_EFFECT, 0);
}

static void setLedringEffect1(void)
{
	exitMenu();
	sendRmotorCmd(CMD_LEDRING_EFFECT, 1);
}

static void setLedringEffect2(void)
{
	exitMenu();
	sendRmotorCmd(CMD_LEDRING_EFFECT, 2);
}

static void setLedringEffect3(void)
{
	exitMenu();
	sendRmotorCmd(CMD_LEDRING_EFFECT, 3);
}

static void setLedringEffect4(void)
{
	exitMenu();
	sendRmotorCmd(CMD_LEDRING_EFFECT, 4);
}

static void setLedringEffect5(void)
{
	exitMenu();
	sendRmotorCmd(CMD_LEDRING_EFFECT, 5);
}

static void setLedringEffect6(void)
{
	exitMenu();
	sendRmotorCmd(CMD_LEDRING_EFFECT, 6);
}


static void setLedringEffect7(void)
{
	exitMenu();
	sendRmotorCmd(CMD_LEDRING_EFFECT, 7);
}

static void setLedringEffect8(void)
{
	exitMenu();
	sendRmotorCmd(CMD_LEDRING_EFFECT, 8);
}

static void setLedringEffect9(void)
{
	exitMenu();
	sendRmotorCmd(CMD_LEDRING_EFFECT, 9);
}

/*初始化ledring菜单*/
void ledringMenuInit(void)
{	
	for(int i=0; i<LED_RING_MENU_NUM; i++)
	{
		ledringMenu[i].menuItemCount = LED_RING_MENU_NUM;
		ledringMenu[i].isSelect = false;
		ledringMenu[i].icoSelected = radio_selected_img;
		ledringMenu[i].icoUnselected = radio_unselected_img;
		ledringMenu[i].title = ledringLanguage[configParam.language][i];
		ledringMenu[i].parentMenu = &expModuleMenu[1];
		ledringMenu[i].childrenMenu = NULL;
	}
	
	ledringMenu[0].isSelect = true;
	ledringMenu[0].Function = setLedringEffect0;
	ledringMenu[1].Function = setLedringEffect1;
	ledringMenu[2].Function = setLedringEffect2;
	ledringMenu[3].Function = setLedringEffect3;
	ledringMenu[4].Function = setLedringEffect4;
	ledringMenu[5].Function = setLedringEffect5;
	ledringMenu[6].Function = setLedringEffect6;
	ledringMenu[7].Function = setLedringEffect7;
	ledringMenu[8].Function = setLedringEffect8;
	ledringMenu[9].Function = setLedringEffect9;
}

static void turnOnExpModule(void)
{
	exitMenu();
	sendRmotorCmd(CMD_POWER_MODULE, 1);
}

static void turnOffExpModule(void)
{
	exitMenu();
	sendRmotorCmd(CMD_POWER_MODULE, 0);
}
/*初始摄像头设置菜单*/
void cameraMenuInit(void)
{	
	for(int i=0; i<WIFI_CAMERA_MENU_NUM; i++)
	{
		cameraMenu[i].menuItemCount = WIFI_CAMERA_MENU_NUM;
		cameraMenu[i].isSelect = false;
		cameraMenu[i].icoSelected = radio_selected_img;
		cameraMenu[i].icoUnselected = radio_unselected_img;
		cameraMenu[i].title = cameraLanguage[configParam.language][i];
		cameraMenu[i].Function = NULL;
		cameraMenu[i].parentMenu = &expModuleMenu[2];
		cameraMenu[i].childrenMenu = NULL;
	}
	
	cameraMenu[0].isSelect = true;
	cameraMenu[0].Function = turnOnExpModule;
	cameraMenu[1].Function = turnOffExpModule;
}

static void turnOnVl53lxx(void)
{
	exitMenu();
	sendRmotorCmd(CMD_POWER_VL53LXX, 1);
}

static void turnOffVl53lxx(void)
{
	exitMenu();
	sendRmotorCmd(CMD_POWER_VL53LXX, 0);
}
/*初始摄像头设置菜单*/
void vl53lxxMenuInit(void)
{	
	for(int i=0; i<OPTICAL_FLOW_MENU_NUM; i++)
	{
		opflowMenu[i].menuItemCount = OPTICAL_FLOW_MENU_NUM;
		opflowMenu[i].isSelect = false;
		opflowMenu[i].icoSelected = radio_selected_img;
		opflowMenu[i].icoUnselected = radio_unselected_img;
		opflowMenu[i].title = opflowLanguage[configParam.language][i];
		opflowMenu[i].Function = NULL;
		opflowMenu[i].parentMenu = &expModuleMenu[3];
		opflowMenu[i].childrenMenu = NULL;
	}
	
	opflowMenu[0].isSelect = true;
	opflowMenu[0].Function = turnOnVl53lxx;
	opflowMenu[1].Function = turnOffVl53lxx;
}

