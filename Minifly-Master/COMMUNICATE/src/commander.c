#include <math.h>
#include "maths.h"
#include "commander.h"
#include "atkp.h"
#include "config_param.h"
#include "radiolink.h"
#include "remoter_ctrl.h"
#include "stabilizer.h"
#include "state_estimator.h"
#include "position_pid.h"
#include "module_mgt.h"
#include "optical_flow.h"

/*FreeRTOS���ͷ�ļ�*/
#include "FreeRTOS.h"
#include "task.h"

/********************************************************************************	 
 * ������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
 * ALIENTEK MiniFly
 * ��ȡң��������������
 * ����ԭ��@ALIENTEK
 * ������̳:www.openedv.com
 * ��������:2017/5/12
 * �汾��V1.3
 * ��Ȩ���У�����ؾ���
 * Copyright(C) �������������ӿƼ����޹�˾ 2014-2024
 * All rights reserved
********************************************************************************/

#define CLIMB_RATE			100.f
#define MAX_CLIMB_UP		100.f
#define MAX_CLIMB_DOWN		60.f

#define MIN_THRUST  	5000
#define MAX_THRUST  	60000

#define APPROXIMATE_CAM_DELAY 60

static bool isRCLocked;				/* ң������״̬ */
static ctrlValCache_t remoteCache;	/* ң�ػ������� */
static ctrlValCache_t wifiCache;	/* wifi�������� */
static ctrlValCache_t* nowCache = &remoteCache;/*Ĭ��Ϊң��*/
static ctrlVal_t ctrlValLpf = {0.f};/* �������ݵ�ͨ */
arucoData_t arucoData = {0.f}; //aruco data

static float minAccZ = 0.f; 
static float maxAccZ = 0.f; 

static YawModeType yawMode = XMODE;	/* Ĭ��ΪX����ģʽ */
static commanderBits_t commander;

static void commanderLevelRPY(void)
{
	ctrlValLpf.roll = 0;
	ctrlValLpf.pitch = 0;
	ctrlValLpf.yaw = 0;
}

static void commanderDropToGround(void)
{
	commanderLevelRPY();
	ctrlValLpf.thrust = 0;
	if(commander.keyFlight)	/*���й����У�ң�����źŶϿ���һ������*/
	{
		commander.keyLand = true;
		commander.keyFlight = false;
	}	
}
u32 timestamp = 0;
/********************************************************
 *ctrlDataUpdate()	���¿�������
 *ң������ ���ȼ�����wifi��������
*********************************************************/
static void ctrlDataUpdate(void)	
{
	static float lpfVal = 0.2f;
	u32 tickNow = getSysTickCnt();	

	timestamp = tickNow - wifiCache.timestamp;
	
	if ((tickNow - remoteCache.timestamp) < COMMANDER_WDT_TIMEOUT_STABILIZE) 
	{
		isRCLocked = false;			/*����*/
		nowCache = &remoteCache;	/* ң�ػ������� */
	}else 
	if ((tickNow - wifiCache.timestamp) < COMMANDER_WDT_TIMEOUT_STABILIZE) 
	{
		isRCLocked = false;			/*����*/
		nowCache = &wifiCache;		/* wifi�������� */
	}else 
	if ((tickNow - remoteCache.timestamp) < COMMANDER_WDT_TIMEOUT_SHUTDOWN) 
	{
		nowCache = &remoteCache;	/* ң�ػ������� */
		commanderLevelRPY();
	}else 
	if ((tickNow - wifiCache.timestamp) < COMMANDER_WDT_TIMEOUT_SHUTDOWN) 
	{
		nowCache = &wifiCache;		/* wifi�������� */
		commanderLevelRPY();
	} else 
	{
		isRCLocked = true;			/*����*/
		nowCache = &remoteCache;
		commanderDropToGround();
	}
	
	if(isRCLocked == false)	/*����״̬*/
	{
		ctrlVal_t ctrlVal =  nowCache->tarVal[nowCache->activeSide];	/*��ȡ����*///����x,y,d
		
//		ctrlValLpf.thrust = ctrlVal.thrust;
//		ctrlValLpf.pitch = ctrlVal.pitch;
//		ctrlValLpf.roll = ctrlVal.roll;
//		ctrlValLpf.yaw = ctrlVal.yaw;
		ctrlValLpf.thrust += (ctrlVal.thrust - ctrlValLpf.thrust) * lpfVal;
		ctrlValLpf.pitch += (ctrlVal.pitch - ctrlValLpf.pitch) * lpfVal;
		ctrlValLpf.roll += (ctrlVal.roll - ctrlValLpf.roll) * lpfVal;
		ctrlValLpf.yaw += (ctrlVal.yaw - ctrlValLpf.yaw) * lpfVal;

		// ctrlValLpf.x += (ctrlVal.x - ctrlValLpf.x) * lpfVal;
		// ctrlValLpf.y += (ctrlVal.y - ctrlValLpf.y) * lpfVal;
		// ctrlValLpf.depth += (ctrlVal.depth - ctrlValLpf.depth) * lpfVal;
		// ctrlValLpf.aruco_id = ctrlVal.aruco_id;
		
		configParam.trimP = ctrlVal.trimPitch;	/*����΢��ֵ*/
		configParam.trimR = ctrlVal.trimRoll;
		
		if (ctrlValLpf.thrust < MIN_THRUST)
			ctrlValLpf.thrust = 0;	
		else 		
			ctrlValLpf.thrust = (ctrlValLpf.thrust>=MAX_THRUST) ? MAX_THRUST:ctrlValLpf.thrust;
	}
}

/************************************************************************
* ����carefree(��ͷģʽ)���ο���������ϵ��������Χ��YAW��ת��
* ����ǰ����Ȼ���ֿ�ʼ�ķ������ģʽ�����ַǳ�ʵ��
************************************************************************/
static void rotateYawCarefree(setpoint_t *setpoint, const state_t *state)
{
	float yawRad = state->attitude.yaw * DEG2RAD;
	float cosy = cosf(yawRad);
	float siny = sinf(yawRad);
	
	if(setpoint->mode.x ==  modeDisable || setpoint->mode.y ==  modeDisable)	/*�ֶ��Ͷ���ģʽ*/
	{
		float originalRoll = setpoint->attitude.roll;
		float originalPitch = setpoint->attitude.pitch;

		setpoint->attitude.roll = originalRoll * cosy + originalPitch * siny;
		setpoint->attitude.pitch = originalPitch * cosy - originalRoll * siny;
	}
	else if(setpoint->mode.x ==  modeVelocity || setpoint->mode.y ==  modeVelocity)	/*����ģʽ*/
	{
		float originalVy = setpoint->velocity.y;
		float originalVx = setpoint->velocity.x;

		setpoint->velocity.y = originalVy * cosy + originalVx * siny;
		setpoint->velocity.x = originalVx * cosy - originalVy * siny;
	}
}

/*�ɿ����ݻ���*/
void flightCtrldataCache(ctrlSrc_e ctrlSrc, ctrlVal_t pk)
{
	switch(ctrlSrc)
	{
		case ATK_REMOTER:
			remoteCache.tarVal[!remoteCache.activeSide] = pk;
			remoteCache.activeSide = !remoteCache.activeSide;
			remoteCache.timestamp = getSysTickCnt();
			break;
		
		case WIFI:
			wifiCache.tarVal[!wifiCache.activeSide] = pk;
			wifiCache.activeSide = !wifiCache.activeSide;
			wifiCache.timestamp = getSysTickCnt();
			break;
		default :
			break;
	}
}

extern bool isExitFlip;			/*�Ƿ��˳��շ�*/
/********************************************************
* flyerAutoLand()
* �����Զ�����
*********************************************************/
void flyerAutoLand(setpoint_t *setpoint,const state_t *state)
{	
	static u8 lowThrustCnt = 0;
	static float stateVelLpf  = -30.f;
	
	setpoint->mode.z = modeVelocity;
	stateVelLpf += (state->velocity.z -  stateVelLpf) * 0.1f;	/*���ʵ�ͨ*/
	setpoint->velocity.z = -70.f - stateVelLpf;	/*�����ٶ� ��λcm/s*/

	if(getAltholdThrust() < 20000.f)	/*��������ֵ�ϵ�*/
	{
		lowThrustCnt++;
		if(lowThrustCnt > 10)
		{
			lowThrustCnt = 0;
			commander.keyLand = false;
			commander.keyFlight = false;
			estRstAll();	/*��λ����*/
		}	
	}else
	{
		lowThrustCnt = 0;
	}
	
	if(isExitFlip == true)	/*�˳��շ����ټ����ٶ�*/
	{
		float accZ = state->acc.z;
		if(minAccZ > accZ)
			minAccZ = accZ;
		if(maxAccZ < accZ)
			maxAccZ = accZ;
	}
	
	if (minAccZ < -80.f && maxAccZ > 320.f)
	{
		commander.keyLand = false;
		commander.keyFlight = false;
		estRstAll();	/*��λ����*/
	}	
}

static bool initHigh = false;
static bool isAdjustingPosZ = false;/*Adjust Z Pos*/
static bool isAdjustingPosXY = true;/*Adjust XY Pos*/
static u8 adjustPosXYTime = 0;		/*XY Pos adjust time*/
static u8 adjustArucoXYTime = 0;	/*Aruco XY Pos adjust time*/
static float errorPosX = 0.f;		/*X error*/
static float errorPosY = 0.f;		/*Y error*/
static float errorPosZ = 0.f;		/*Z error*/
static point_t dronePosBeforeDelay = {0.f};


void commanderGetSetpoint(setpoint_t *setpoint, state_t *state)
{	
	static float maxAccZ = 0.f;
	
	ctrlDataUpdate(); /*Update contral values*/
	
	state->isRCLocked = isRCLocked;	/*Update remote locked status*/
	
	if(commander.ctrlMode & 0x01) /*Fixed height Controller*/
	{
		if(commander.keyLand) /*One-click landing*/
		{
			flyerAutoLand(setpoint, state);
		}
		else if(commander.keyFlight) /*One-click take off*/ 
		{	
			setpoint->thrust = 0;
			setpoint->mode.z = modeAbs;		
			
			if (initHigh == false)
			{
				initHigh = true;	
				isAdjustingPosXY = true;
				errorPosX = 0.f;
				errorPosY = 0.f;
				errorPosZ = 0.f;

				setFastAdjustPosParam(0, 1, 80.f);	/*Fly to 80cm*/															
			}		
				
			float climb = ((ctrlValLpf.thrust - 32767.f) / 32767.f);
			if(climb > 0.f) 
				climb *= MAX_CLIMB_UP;
			else
				climb *= MAX_CLIMB_DOWN;
			
			if (fabsf(climb) > 5.f)
			{
				isAdjustingPosZ = true;												
				setpoint->mode.z = modeVelocity;
				setpoint->velocity.z = climb;

				if(climb < -(CLIMB_RATE/5.f))	/*thrust too low*/
				{
					if(isExitFlip == true)		/*exit flipping, get accel again*/
					{
						if(maxAccZ < state->acc.z)
							maxAccZ = state->acc.z;
						if(maxAccZ > 250.f)		/*touch ground, shutdown*/
						{
							commander.keyFlight = false;
							estRstAll();	/*reset all estimation*/
						}
					}
				}else
				{
					maxAccZ = 0.f;
				}
			}
			else if(arucoData.x != 0 && arucoData.y != 0 && arucoData.z != 0) //aruco id decide height
			{
				isAdjustingPosZ = false;
				
				setpoint->mode.z = modeAbs;
				setpoint->position.z = arucoData.id * 3;
			}
			else if (isAdjustingPosZ == true)
			{
				isAdjustingPosZ = false;
			
				setpoint->mode.z = modeAbs;
				setpoint->position.z = state->position.z + errorPosZ; /*Adjust new pos*/									
			}
			else if(isAdjustingPosZ == false) /*Z error*/
			{
				errorPosZ = setpoint->position.z - state->position.z;
				errorPosZ = constrainf(errorPosZ, -10.f, 10.f); /*Z err constrain, unit is cm*/
			}
		}
		else /*landing*/
		{
			setpoint->mode.z = modeDisable;
			setpoint->thrust = 0;
			setpoint->velocity.z = 0;
			setpoint->position.z = 0;
			initHigh = false;
			isAdjustingPosZ = false;
		}
	}
	else /*Manual fly*/
	{
		setpoint->mode.z = modeDisable;
		setpoint->thrust = ctrlValLpf.thrust;
	}	
 	
	setpoint->attitude.roll = ctrlValLpf.roll;
	setpoint->attitude.pitch = ctrlValLpf.pitch;
	setpoint->attitude.yaw  = -ctrlValLpf.yaw; /*stick is opposite to yaw axis*/
	
	if(getOpDataState() && commander.ctrlMode == 0x03) /*Opflow is avaliable, fix point controller*/ 
	{
		setpoint->attitude.yaw *= 0.5f;	/*yaw adjust slow down*/
		
		/*adjust pos, vel mode*/
		if(fabsf(setpoint->attitude.roll) > 1.5f || fabsf(setpoint->attitude.pitch) > 1.5f)
		{
			adjustPosXYTime = 0;
			adjustArucoXYTime = 0;
			isAdjustingPosXY = true;
			setpoint->mode.x = modeVelocity;
			setpoint->mode.y = modeVelocity;
			setpoint->velocity.x = setpoint->attitude.pitch * 4.0f;
			setpoint->velocity.y = setpoint->attitude.roll * 4.0f;	
		}
		else if(arucoData.x == 0 || arucoData.y == 0 || arucoData.z == 0)
		{
			adjustArucoXYTime = 0;
		}
		else if(arucoData.x != 0 && arucoData.y != 0 && arucoData.z != 0 && adjustArucoXYTime == 100 - APPROXIMATE_CAM_DELAY) //aruco detected, get pos
		{
			adjustArucoXYTime++;
			dronePosBeforeDelay.x = state.x;
			dronePosBeforeDelay.y = state.y;
			dronePosBeforeDelay.z = state.z;
		}
		
		else if(arucoData.x != 0 && arucoData.y != 0 && arucoData.z != 0 && adjustArucoXYTime++ > 100) //aruco detected, get pos
		{
			adjustArucoXYTime = 0;
			isAdjustingPosXY = false;
			setpoint->mode.x = modeAbs;
			setpoint->mode.y = modeAbs;
			setpoint->position.x = dronePosBeforeDelay.x + arucoData.y;
			setpoint->position.y = dronePosBeforeDelay.y - arucoData.x;
		}
		else if(isAdjustingPosXY == true)
		{
			if(adjustPosXYTime++ > 100)
			{
				adjustPosXYTime = 0;
				isAdjustingPosXY = false;
			}		
			setpoint->mode.x = modeAbs;
			setpoint->mode.y = modeAbs;
			setpoint->position.x = state->position.x + errorPosX; //adjust new pos
			setpoint->position.y = state->position.y + errorPosY;
		}
		else if(isAdjustingPosXY == false) /*pos error*/
		{	
			errorPosX = setpoint->position.x - state->position.x;
			errorPosY = setpoint->position.y - state->position.y;
			errorPosX = constrainf(errorPosX, -30.0f, 30.0f); /*constraint, unit is cm*/
			errorPosY = constrainf(errorPosY, -30.0f, 30.0f);
		}
	}
	else /*Manual fly*/
	{
		setpoint->mode.x = modeDisable;
		setpoint->mode.y = modeDisable;		
	}
	
	setpoint->mode.roll = modeDisable;	
	setpoint->mode.pitch = modeDisable;	
	
	if(commander.flightMode) /*no heading mode*/
	{
		yawMode = CAREFREE;		
		rotateYawCarefree(setpoint, state);
	}		
	else /*x heading mode*/
	{
		yawMode = XMODE;
	}		
}

/* update trim */
void getAndUpdateTrim(float* pitch, float* roll)
{
	*pitch = nowCache->tarVal[nowCache->activeSide].trimPitch;
	*roll = nowCache->tarVal[nowCache->activeSide].trimRoll;
}

void setCommanderCtrlMode(u8 set)
{
	commander.ctrlMode = (set & 0x03);
}
u8 getCommanderCtrlMode(void)
{
	return (commander.ctrlMode & 0x03);
}

u8 getCommanderFlightMode(void)
{
	return (yawMode & 0x01);
}

void setCommanderKeyFlight(bool set)
{
	commander.keyFlight = set;
	if(set == true)	/*One-click take off, clear min and max*/
	{
		minAccZ = 0.f;
		maxAccZ = 0.f;
	}
}
bool getCommanderKeyFlight(void)
{
	return commander.keyFlight;
}

void setCommanderKeyland(bool set)
{
	commander.keyLand = set;
}
bool getCommanderKeyland(void)
{
	return commander.keyLand;
}

void setCommanderFlightmode(bool set)
{
	commander.flightMode = set;
}

void setCommanderEmerStop(bool set)
{
	commander.emerStop = set;
}


