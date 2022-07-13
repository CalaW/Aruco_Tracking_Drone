#include "string.h"
#include <math.h>
#include "sensfusion6.h"
#include "remoter_ctrl.h"
#include "config_param.h"
#include "commander.h"
#include "flip.h"
#include "radiolink.h"
#include "sensors.h"
#include "pm.h"
#include "stabilizer.h"
#include "module_mgt.h"
#include "ledring12.h"
#include "vl53lxx.h"
#include "system.h"

/*FreeRTOS���ͷ�ļ�*/
#include "FreeRTOS.h"
#include "task.h"

/********************************************************************************	 
 * ������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
 * ALIENTEK MiniFly
 * ң����������������	
 * ����ԭ��@ALIENTEK
 * ������̳:www.openedv.com
 * ��������:2017/5/12
 * �汾��V1.3
 * ��Ȩ���У�����ؾ���
 * Copyright(C) �������������ӿƼ����޹�˾ 2014-2024
 * All rights reserved
 *
 * �޸�˵��:
 * �汾V1.3 �����ϵ�У׼ͨ�����ϴ�΢����Ϣ��
********************************************************************************/

ctrlVal_t remoterCtrl;/*���͵�commander��̬��������*/
static MiniFlyMsg_t msg;
static u8 reSendTimes = 3;	/*΢���ط�����*/

/*����������Ϣ*/
void sendMsgACK(void)
{
	msg.version = configParam.version;
	msg.mpu_selfTest = getIsMPU9250Present();
	msg.baro_slfTest = getIsBaroPresent();
	msg.isCanFly = getIsCalibrated();
	if(msg.isCanFly == true)	/*У׼ͨ��֮����΢��ֵ*/
	{
		if(reSendTimes > 0) /*΢���ط�����*/
		{
			reSendTimes--;
			msg.trimPitch = configParam.trimP;
			msg.trimRoll = configParam.trimR;
		}
	}
	msg.isLowpower = getIsLowpower();
	msg.moduleID = getModuleID();
	
	atkp_t p;
	p.msgID = UP_REMOTER;
	p.dataLen = sizeof(msg)+1;
	p.data[0] = ACK_MSG;
	memcpy(p.data+1, &msg, sizeof(msg));
	radiolinkSendPacketBlocking(&p);	
}

/*ң�����ݽ��մ���*/
void remoterCtrlProcess(atkp_t* pk)
{	
	if(pk->data[0] == REMOTER_CMD)
	{
		switch(pk->data[1])
		{
			case CMD_FLIGHT_LAND:
				if(getCommanderKeyFlight() != true)
				{
					setCommanderKeyFlight(true);
					setCommanderKeyland(false);
				}
				else
				{
					setCommanderKeyFlight(false);
					setCommanderKeyland(true);
				}
				break;

			case CMD_EMER_STOP:
				setCommanderKeyFlight(false);
				setCommanderKeyland(false);
				break;
			
			case CMD_FLIP:
				setFlipDir(pk->data[2]);
				break;
			
			case CMD_GET_MSG:
				sendMsgACK();
				break;
			
			case CMD_POWER_MODULE:
				expModulePower(pk->data[2]);
				break;
			
			case CMD_LEDRING_EFFECT:
//				expModulePower(true);
				setLedringEffect(pk->data[2]);
				break;
			
			case CMD_POWER_VL53LXX:
				setVl53lxxState(pk->data[2]);
				break;
		}
	}
	else if(pk->data[0] == REMOTER_DATA)//����x,y,depth
	{
		remoterData_t remoterData = *(remoterData_t*)(pk->data+1);
		
		remoterCtrl.roll = remoterData.roll;
		remoterCtrl.pitch = remoterData.pitch;
		remoterCtrl.yaw = remoterData.yaw;
		remoterCtrl.thrust = remoterData.thrust * 655.35f;
		remoterCtrl.trimPitch = remoterData.trimPitch;
		remoterCtrl.trimRoll = remoterData.trimRoll;

		remoterCtrl.x = remoterData.x;
		remoterCtrl.y = remoterData.y;
		remoterCtrl.depth = remoterData.depth;
		remoterCtrl.aruco_id = remoterData.aruco_id;
		
		setCommanderCtrlMode(remoterData.ctrlMode);
		setCommanderFlightmode(remoterData.flightMode);
		flightCtrldataCache(ATK_REMOTER, remoterCtrl);
	}
}

