#include "radiolink.h"
#include "led.h"
#include "24l01.h"
#include "oled.h"
#include "config_param.h"
/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

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

/*发送和接收队列信息个数*/
#define  RADIOLINK_TX_QUEUE_SIZE  10
#define  RADIOLINK_RX_QUEUE_SIZE  10

xTaskHandle radiolinkTaskHandle;
static xQueueHandle  txQueue;
static xQueueHandle  rxQueue;
static xSemaphoreHandle nrfIT;
static bool isInit;
static bool connectStatus;
static atkp_t tx_p;
static u8 statusCount;
static u16 failRxCount;
static u16 failReceiveNum;
static TickType_t failRxcountTime;

/*nrf外部中断回调函数*/
static void nrf_interruptCallback(void)
{
	portBASE_TYPE  xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR(nrfIT, &xHigherPriorityTaskWoken);
}

/*无线配置初始化（地址、通道、速率）*/
static void radioInit(void)
{
	uint64_t addr = (uint64_t)configParam.radio.addressHigh<<32 | configParam.radio.addressLow;
	if(nrf_check() == SUCCESS)
	{
		nrfInit(PTX_MODE);
		nrf_setIterruptCallback(nrf_interruptCallback);
	}
	else
	{
		oledInit();
		oled_showString(0,0,(u8*)"NRF24L01 CHECK FAIL !",6,12);
		oled_refreshGram();
		while(1);
	}
	nrf_setAddress(addr);
	nrf_setChannel(configParam.radio.channel);
	nrf_setDataRate(configParam.radio.dataRate);
}

/*无线连接初始化*/
void radiolinkInit(void)
{
	if (isInit) return;
	radioInit();
	
	txQueue = xQueueCreate(RADIOLINK_TX_QUEUE_SIZE, sizeof(atkp_t));
	ASSERT(txQueue);
	rxQueue = xQueueCreate(RADIOLINK_RX_QUEUE_SIZE, sizeof(atkp_t));
	ASSERT(rxQueue);
	
	nrfIT = xSemaphoreCreateBinary();
	
	tx_p.msgID = DOWN_RADIO;
	tx_p.dataLen = 1;
	tx_p.data[0] = D_RADIO_HEARTBEAT;
	connectStatus = false;
	isInit = true;
}

/*无线发送atkpPacket*/
bool radiolinkSendPacket(const atkp_t *p)
{
	ASSERT(p);
	ASSERT(p->dataLen <= ATKP_MAX_DATA_SIZE);
	return xQueueSend(txQueue, p, 0);
}
bool radiolinkSendPacketBlocking(const atkp_t *p)
{
	ASSERT(p);
	ASSERT(p->dataLen <= ATKP_MAX_DATA_SIZE);
	return xQueueSend(txQueue, p, 100);//portMAX_DELAY
}

/*无线接收atkpPacket*/
bool radiolinkReceivePacket(atkp_t *p)
{
	ASSERT(p);
	return xQueueReceive(rxQueue, p, 0);
}
bool radiolinkReceivePacketBlocking(atkp_t *p)
{
	ASSERT(p);
	return xQueueReceive(rxQueue, p, portMAX_DELAY);
}

/*无线连接任务*/
void radiolinkTask(void* param)
{
	u8 rx_len;
	atkp_t rx_p;
	while(1)
	{
		nrf_txPacket((u8*)&tx_p, tx_p.dataLen+2);
		xSemaphoreTake(nrfIT, 1000);
		nrfEvent_e status = nrf_checkEventandRxPacket((u8*)&rx_p, &rx_len);
		if(status == RX_DR)//发送成功
		{	
			LED_BLUE = 0;
			LED_RED  = 1;
			statusCount = 0;
			connectStatus = true;
			if(rx_p.dataLen <= ATKP_MAX_DATA_SIZE)
			{
				xQueueSend(rxQueue, &rx_p, portMAX_DELAY);
			}
			if(xQueueReceive(txQueue, &tx_p, 0) == pdFALSE)
			{
				tx_p.msgID = DOWN_RADIO;
				tx_p.dataLen = 1;
				tx_p.data[0] = D_RADIO_HEARTBEAT;
			}
		}
		else if(status == MAX_RT)//发送失败
		{
			LED_BLUE = 1;
			LED_RED  = 0;
			failRxCount++;
			if(++statusCount > 10)//连续10次无应答则通讯失败
			{
				statusCount = 0;
				connectStatus = false;
			}
		}
		
		/*1000ms统计一次收发失败次数*/
		if(connectStatus==true && xTaskGetTickCount()>=failRxcountTime+1000)
		{
			failRxcountTime = xTaskGetTickCount();
			failReceiveNum = failRxCount;
			failRxCount = 0;
		}
		
	}
}

/*获取丢包个数*/
u16 radioinkFailRxcount(void)
{
	return failReceiveNum;
}

/*获取无线连接状态*/
bool radioinkConnectStatus(void)
{
	return connectStatus;
}

/*使能radiolink*/
void radiolinkEnable(FunctionalState state)
{
	if(state == ENABLE)
		vTaskResume(radiolinkTaskHandle);
	else
		vTaskSuspend(radiolinkTaskHandle);
}










