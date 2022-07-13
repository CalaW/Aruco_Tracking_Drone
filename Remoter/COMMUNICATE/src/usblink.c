#include <string.h>
#include "usblink.h"
#include "hw_config.h"
#include "usb_pwr.h"
#include "remoter_ctrl.h"
/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

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

#define USBLINK_TX_QUEUE_SIZE 16
#define USBLINK_RX_QUEUE_SIZE 16

static enum
{
	waitForStartByte1,
	waitForStartByte2,
	waitForMsgID,
	waitForDataLength,
	waitForData,
	waitForChksum1,
}rxState;

static bool isInit;
static atkp_t rxPacket;
static xQueueHandle  txQueue;
static xQueueHandle  rxQueue;


/*usb连接初始化*/
void usblinkInit(void)
{
	if (isInit) return;
	txQueue = xQueueCreate(USBLINK_TX_QUEUE_SIZE, sizeof(atkp_t));
	ASSERT(txQueue);
	rxQueue = xQueueCreate(USBLINK_RX_QUEUE_SIZE, sizeof(atkp_t));
	ASSERT(rxQueue);
	isInit = true;
}

/*usb连接发送atkpPacket*/
bool usblinkSendPacket(const atkp_t *p)
{
	ASSERT(p);
	ASSERT(p->dataLen <= ATKP_MAX_DATA_SIZE);
	return xQueueSend(txQueue, p, 0);
}
bool usblinkSendPacketBlocking(const atkp_t *p)
{
	ASSERT(p);
	ASSERT(p->dataLen <= ATKP_MAX_DATA_SIZE);
	return xQueueSend(txQueue, p, portMAX_DELAY);
}

/*usb连接接收atkpPacket*/
bool usblinkReceivePacket(atkp_t *p)
{
	ASSERT(p);
	return xQueueReceive(rxQueue, p, 0);
}
bool usblinkReceivePacketBlocking(atkp_t *p)
{
	ASSERT(p);
	return xQueueReceive(rxQueue, p, portMAX_DELAY);
}

/*usb连接发送任务*/
void usblinkTxTask(void* param)
{
	atkp_t p;
	u8 sendBuffer[64];
	u8 cksum;
	u8 dataLen;
	while(bDeviceState != CONFIGURED)//等usb配置成功
	{
		vTaskDelay(1000);
	}
	while(1)
	{
		xQueueReceive(txQueue, &p, portMAX_DELAY);
		if(p.msgID != UP_RADIO)/*NRF51822的数据包不上传*/
		{
			if(p.msgID == UP_PRINTF)/*打印数据包去掉帧头*/
			{
				memcpy(&sendBuffer, p.data, p.dataLen);
				dataLen = p.dataLen;
			}
			else
			{
				sendBuffer[0] = UP_BYTE1;
				sendBuffer[1] = UP_BYTE2;
				sendBuffer[2] = p.msgID;
				sendBuffer[3] = p.dataLen;
				memcpy(&sendBuffer[4], p.data, p.dataLen);
				cksum = 0;
				for (int i=0; i<p.dataLen+4; i++)
				{
					cksum += sendBuffer[i];
				}
				dataLen = p.dataLen+5;
				sendBuffer[dataLen - 1] = cksum;
			}
			usbsendData(sendBuffer, dataLen);
		}		
	}
}

/*usb连接接收任务*/
void usblinkRxTask(void *param)
{
	u8 c;
	u8 dataIndex = 0;
	u8 cksum = 0;
	rxState = waitForStartByte1;
	while(1)
	{
		if (usbGetDataWithTimout(&c))
		{
			switch(rxState)
			{
				case waitForStartByte1:
					rxState = (c == DOWN_BYTE1) ? waitForStartByte2 : waitForStartByte1;
					cksum = c;  
					break;
				case waitForStartByte2:
					rxState = (c == DOWN_BYTE2) ? waitForMsgID : waitForStartByte1;
					cksum += c;
					break;
				case waitForMsgID:
					rxPacket.msgID = c;
					rxState = waitForDataLength;
					cksum += c;
					break;
				case waitForDataLength:
					if (c <= ATKP_MAX_DATA_SIZE)
					{
						rxPacket.dataLen = c;
						dataIndex = 0;
						rxState = waitForData;
						cksum += c;
					} else 
					{
						rxState = waitForStartByte1;
					}
					break;
				case waitForData:
					rxPacket.data[dataIndex] = c;
					dataIndex++;
					cksum += c;
					if (dataIndex == rxPacket.dataLen)
					{
						rxState = waitForChksum1;
					}
					break;
				case waitForChksum1:
					if (cksum == c)/*所有校验正确*/
					{
						//upAnalyse(&rxPacket);
						xQueueSend(rxQueue, &rxPacket, 0);
					} 
					else
					{
						rxState = waitForStartByte1;
					}
					rxState = waitForStartByte1;
					break;
				default:
					break;
			}
		}
		else	/*超时处理*/
		{
			rxState = waitForStartByte1;
		}
	}
}

