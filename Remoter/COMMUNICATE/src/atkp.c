#include <string.h>
#include "atkp.h"
#include "radiolink.h"
#include "usblink.h"
#include "config_param.h"
#include "trim_ui.h"
#include "beep.h"
#include "main_ui.h"
#include "match_ui.h"
#include "remoter_ctrl.h"

/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "task.h"
/********************************************************************************	 
 * 本程序只供学习使用，未经作者许可，不得用于其它任何用途
 * ALIENTEK MiniFly_Remotor
 * 飞控通讯协议格式代码	
 * 正点原子@ALIENTEK
 * 技术论坛:www.openedv.com
 * 创建日期:2018/6/1
 * 版本：V1.0
 * 版权所有，盗版必究。
 * Copyright(C) 广州市星翼电子科技有限公司 2014-2024
 * All rights reserved
********************************************************************************/

float plane_yaw,plane_roll,plane_pitch;
float plane_bat;
u8 rssi;

/*atkp解析*/
static void atkpAnalyze(atkp_t *p)
{
	if(p->msgID == UP_STATUS)
	{
		plane_roll = (s16)(*(p->data+0)<<8)|*(p->data+1);
		plane_roll = plane_roll/100;
		plane_pitch = (s16)(*(p->data+2)<<8)|*(p->data+3);
		plane_pitch = plane_pitch/100;
		plane_yaw = (s16)(*(p->data+4)<<8)|*(p->data+5);
		plane_yaw = plane_yaw/100;
	}
	else if(p->msgID == UP_POWER)
	{
		plane_bat = (s16)(*(p->data+0)<<8)|*(p->data+1);
		plane_bat = plane_bat/100;
	}
	else if(p->msgID == UP_REMOTOR)	
	{
		switch(p->data[0])
		{
			case ACK_MSG:
				miniFlyMsgACKProcess(p);
				break;
		}
	}
	else if(p->msgID == UP_RADIO)
	{
		radioConfig_t radio;
		switch(p->data[0])
		{
			case U_RADIO_RSSI:
				rssi = p->data[1];
				break;
			
			case U_RADIO_CONFIG:
				memcpy(&radio, p->data+1, sizeof(radio));
				setMatchRadioConfig(&radio);
				break;
		}
	}
}

/*无线连接数据处理任务*/
void radiolinkDataProcessTask(void *param) 
{
	atkp_t p;
	while(1)
	{
		radiolinkReceivePacketBlocking(&p); /*接收四轴上传的数据，包括四轴数据和遥控器数据*/
		atkpAnalyze(&p);
		usblinkSendPacket(&p);	/*把接收到的四轴数据发送到上位机*/
		vTaskDelay(1);
	}
}

/*USB连接数据处理任务*/
void usblinkDataProcessTask(void *param)
{
	atkp_t p;
	while(1)
	{
		usblinkReceivePacketBlocking(&p);	/*接收上位机发送的数据*/
		atkpAnalyze(&p);
		radiolinkSendPacket(&p);
	}
}

