#include <stdio.h>
#include <string.h>
#include "match_ui.h"
#include "display.h"
#include "keyTask.h"
#include "atkp.h"
#include "oled.h"
#include "joystick.h"
#include "remoter_ctrl.h"
#include "radiolink.h"
#include "beep.h"
#include "main_ui.h"
#include "atkp.h"
#include "24l01.h"
#include "config_param.h"
#include "text.h"
#include "font.h"

/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "task.h"

/********************************************************************************	 
 * 本程序只供学习使用，未经作者许可，不得用于其它任何用途
 * ALIENTEK MiniFly_Remotor
 * 四轴匹配界面代码	
 * 正点原子@ALIENTEK
 * 技术论坛:www.openedv.com
 * 创建日期:2018/6/1
 * 版本：V1.0
 * 版权所有，盗版必究。
 * Copyright(C) 广州市星翼电子科技有限公司 2014-2024
 * All rights reserved
********************************************************************************/

static enum matchStep 
{
	CHECK_ADDR,
	GET_CONFIG_COMMANDER,
	SET_CONFIG_COMMANDER,
	SET_REMOTOR_CONFIG,
	CHECK_STATUS,
}step = CHECK_ADDR;

static bool isConfigRecv = false;
static radioConfig_t radio;


const u8* remoterLanguage[3] = 
{
"遥控",
"   R",
"遙控",
};

const u8* miniFlyLanguage[3] = 
{
"四轴",
"F",
"四軸",
};

const u8* noteMsgLanguage[3][4] = 
{
{
"提示：已匹配！",
"提示：匹配中……",
"  通信失败！请先重置遥控和四轴再匹配。",
},
{
"NOTE: Already matched!",
"NOTE: Matching.....",
"Connect fail!Please reset remoter and MiniFly before matching.",
},
{
"提示：已匹配！",
"提示：匹配中……",
"  通信失敗！請先重置遙控和四軸再匹配。",
},
};

const u8* resultLanguage[3][2] = 
{
{
"匹配成功！",
"匹配失败！",
},
{
"Match Success",
"Match Fail",
},
{
"匹配成功！",
"匹配失敗！",
},
};

/*获取无线配置指令*/
static void getRadioConfigCommander(void)
{
	atkp_t p;
	p.msgID = DOWN_RADIO;
	p.dataLen = 1;
	p.data[0] = D_RADIO_GET_CONFIG;
	radiolinkSendPacket(&p);
}
/*设置无线配置指令*/
static void setRadioConfigCommander(radioConfig_t* set)
{
	atkp_t p;
	p.msgID = DOWN_RADIO;
	p.dataLen = sizeof(radioConfig_t)+1;
	p.data[0] = D_RADIO_SET_CONFIG;
	memcpy(&p.data[1], set, sizeof(radioConfig_t));
	nrf_sendPacketNoACK((u8*)&p, p.dataLen+2);
}

bool isRadioAddrDefault(void)
{
	return(	configParam.radio.addressHigh == ((uint64_t)RADIO_ADDRESS>>32) &&
			configParam.radio.addressLow == (RADIO_ADDRESS & 0xFFFFFFFFULL));
}

void setMatchRadioConfig(radioConfig_t* set)
{
	isConfigRecv = true;
	radio = *set;
}

void match_ui(void)
{
	static bool startMatch = false;
	static u8 timeOut;
	uint64_t addr;
	
	show_str(0,0,remoterLanguage[configParam.language],24,24,1);
	oled_showPicture(48,0,bmp_remoter2fly,32,24);
	show_str(78,0,miniFlyLanguage[configParam.language],24,24,1);
	switch(step)
	{
		case CHECK_ADDR:
			if(radioinkConnectStatus() == true)
			{
				if(isRadioAddrDefault()==true)
				{
					show_str(0,28,noteMsgLanguage[configParam.language][1],12,12,1);
					step = GET_CONFIG_COMMANDER;
					startMatch = true;
				}
				else
				{
					show_str(0,28,noteMsgLanguage[configParam.language][0],12,12,1);
				}
			}
			else
			{
				show_str(0,28,noteMsgLanguage[configParam.language][2],12,12,1);
			}
			break;

		case GET_CONFIG_COMMANDER:
			if(isConfigRecv == true)
			{
				isConfigRecv = false;
				step = SET_CONFIG_COMMANDER;
			}
			else
			{
				getRadioConfigCommander();
			}
			break;
			
		case SET_CONFIG_COMMANDER:
			radiolinkEnable(DISABLE);
			setRadioConfigCommander(&radio);
			radiolinkEnable(ENABLE);
			step = SET_REMOTOR_CONFIG;
			break;
			
		case SET_REMOTOR_CONFIG:
			nrf_setChannel(radio.channel);
			nrf_setDataRate(radio.dataRate);
			addr = (uint64_t)radio.addressHigh<<32 | radio.addressLow;
			nrf_setAddress(addr);
			step = CHECK_STATUS;
			break;
		
		case CHECK_STATUS:
			if(radioinkConnectStatus()==true)
			{
				startMatch = false;
				configParam.radio = radio;
				writeConfigParamToFlash();
				show_str_mid(0,51,resultLanguage[configParam.language][0],12,12,1,128);
			}
			break;
	}
	
	timeOut++;
	
	if(startMatch==true && timeOut>10)
	{
		startMatch = false;
		show_str_mid(0,51,resultLanguage[configParam.language][1],12,12,1,128);
	}
	if(timeOut>30)
	{
		step = CHECK_ADDR;
		timeOut = 0;
		setShow_ui(MAIN_UI);
	}
}



