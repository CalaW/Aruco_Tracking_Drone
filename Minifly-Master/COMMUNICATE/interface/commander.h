#ifndef __COMMANDER_H
#define __COMMANDER_H
#include "atkp.h"
#include "config.h"
#include "stabilizer_types.h"

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

#define COMMANDER_WDT_TIMEOUT_STABILIZE  500
#define COMMANDER_WDT_TIMEOUT_SHUTDOWN   1000

typedef struct
{
	u8 ctrlMode		: 2;	/*bit0  1=����ģʽ 0=�ֶ�ģʽ   bit1  1=����ģʽ*/
	u8 keyFlight 	: 1;	/*bit2 һ�����*/
	u8 keyLand 		: 1;	/*bit3 һ������*/
	u8 emerStop 	: 1;	/*bit4 ����ͣ��*/
	u8 flightMode 	: 1;	/*bit5 ����ģʽ 1=��ͷ 0=��ͷ*/
	u8 reserved		: 2;	/*bit6~7 ����*/
}commanderBits_t;

/*�������ݽṹ��*/
typedef __packed struct
{
	float roll;       // deg
	float pitch;      // deg
	float yaw;        // deg
	float trimPitch;
	float trimRoll;
	u16 thrust;
	// float x;
	// float y;
	// float depth;
	// s16 aruco_id;
} ctrlVal_t;

typedef __packed struct
{
	float x;
	float y;
	float z;
	s16 id;
} arucoData_t;

/*���ݻ���ṹ��*/
typedef struct
{
	ctrlVal_t  tarVal[2];
	bool activeSide;
	u32 timestamp; 		/* FreeRTOS ʱ�ӽ���*/
} ctrlValCache_t;

typedef enum
{
	RATE    = 0,
	ANGLE   = 1,
} RPYType;

typedef enum
{
	XMODE     = 0, /*Xģʽ*/
	CAREFREE  = 1, /*��ͷģʽ*/
} YawModeType;

typedef enum
{
	ATK_REMOTER = 0,
	WIFI		= 1,
}ctrlSrc_e;
	
void commanderInit(void);
bool commanderTest(void);
void flightCtrldataCache(ctrlSrc_e ctrlSrc, ctrlVal_t pk);
void commanderGetSetpoint(setpoint_t *setpoint, state_t *state);
void flyerAutoLand(setpoint_t *setpoint,const state_t *state);

void getAndUpdateTrim(float* pitch, float* roll);

void setCommanderCtrlMode(u8 set);
u8 getCommanderCtrlMode(void);

void setCommanderKeyFlight(bool set);
bool getCommanderKeyFlight(void);

void setCommanderKeyland(bool set);
bool getCommanderKeyland(void);

void setCommanderFlightmode(bool set);
void setCommanderEmerStop(bool set);

extern arucoData_t arucoData;

#endif /* __COMMANDER_H */
