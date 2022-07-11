#ifndef __RADIO_LINK_H
#define __RADIO_LINK_H
#include <stdbool.h>
#include "sys.h"
#include "atkp.h"
/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "task.h"

/*上行指令*/
#define  U_RADIO_RSSI		0x01
#define  U_RADIO_CONFIG		0x02	

/*下行指令*/
#define  D_RADIO_HEARTBEAT	0xFF
#define  D_RADIO_GET_CONFIG	0x01
#define  D_RADIO_SET_CONFIG	0x02

/********************************************************************************	 
 * 本程序只供学习使用，未经作者许可，不得用于其它任何用途
 * ALIENTEK MiniFly_Remotor
 * 无线通信驱动代码	
 * 正点原子@ALIENTEK
 * 技术论坛:www.openedv.com
 * 创建日期:2018/6/1
 * 版本：V1.0
 * 版权所有，盗版必究。
 * Copyright(C) 广州市星翼电子科技有限公司 2014-2024
 * All rights reserved
********************************************************************************/

extern xTaskHandle radiolinkTaskHandle;

void radiolinkInit(void);
bool radiolinkSendPacket(const atkp_t *p);
bool radiolinkSendPacketBlocking(const atkp_t *p);
bool radiolinkReceivePacket(atkp_t *p);
bool radiolinkReceivePacketBlocking(atkp_t *p);
void radiolinkTask(void* param);
u16 radioinkFailRxcount(void);
bool radioinkConnectStatus(void);
void radiolinkEnable(FunctionalState state);

#endif /*RADIO_LINK_H*/
