#ifndef __USB_LINK_H
#define __USB_LINK_H
#include <stdint.h>
#include <stdbool.h>
#include "atkp.h"

/********************************************************************************	 
 * 本程序只供学习使用，未经作者许可，不得用于其它任何用途
 * ALIENTEK MiniFly_Remotor
 * USB通信驱动代码	
 * 正点原子@ALIENTEK
 * 技术论坛:www.openedv.com
 * 创建日期:2018/6/1
 * 版本：V1.0
 * 版权所有，盗版必究。
 * Copyright(C) 广州市星翼电子科技有限公司 2014-2024
 * All rights reserved
********************************************************************************/


void usblinkInit(void);
bool usblinkSendPacket(const atkp_t *p);
bool usblinkSendPacketBlocking(const atkp_t *p);
bool usblinkReceivePacket(atkp_t *p);
bool usblinkReceivePacketBlocking(atkp_t *p);
void usblinkTxTask(void* param);
void usblinkRxTask(void *param);


#endif /*usblink.h*/

