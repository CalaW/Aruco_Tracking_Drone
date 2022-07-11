#include "24l01.h"
/********************************************************************************	 
 * 本程序只供学习使用，未经作者许可，不得用于其它任何用途
 * ALIENTEK MiniFly_Remotor
 * nrf24l01驱动代码	
 * 正点原子@ALIENTEK
 * 技术论坛:www.openedv.com
 * 创建日期:2018/6/1
 * 版本：V1.0
 * 版权所有，盗版必究。
 * Copyright(C) 广州市星翼电子科技有限公司 2014-2024
 * All rights reserved
********************************************************************************/

/* 用户配置参数（默认） */
#define DEFAULT_ADDR			0x123456789AULL
#define DEFAULT_CHANNEL 	 	2
#define DEFAULT_DATARATE  		DATA_RATE_250K
#define DEFAULT_POWR 			POWER_0dBm

uint64_t nrfAddress = DEFAULT_ADDR;
static void (*interruptCb)(void) = 0;

/***************************NRF24L01+驱动函数***********************************/

/* NRF初始化，使用STM32的SPI1 */
static void NRF_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE); 

	/* 配置SPI1的SCK(PA5),MISO(PA6),MOSI(PA7)引脚 */ 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6| GPIO_Pin_7; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* 配置NRF的CE(PA3),CSN(PA4)引脚 */ 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* 配置NRF的IRQ引脚(PC13) */ 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	/* 配置外IRQ外部中断 */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource13);
	EXTI_InitStructure.EXTI_Line = EXTI_Line13;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
  
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  /* 设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工 */
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		/* 设置SPI工作模式:设置为主SPI */
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;	/* 设置SPI的数据大小:SPI发送接收8位帧结构 */
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;			/* 选择了串行时钟的稳态:时钟悬空低 */
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;		/* 数据捕获于第一个时钟沿 */
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;			/* NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制 */
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;	/*定义波特率预分频的值:波特率预分频值为4 */
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	/* 指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始 */
	SPI_InitStructure.SPI_CRCPolynomial = 7;			/* CRC值计算的多项式 */
	SPI_Init(SPI1, &SPI_InitStructure);  				/* 根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器 */
 
	SPI_Cmd(SPI1, ENABLE);	/*使能SPI外设*/
	
	SPI2_CSN_H();
	NRF_CE_L();
}

static u8 SPI_RWByte(SPI_TypeDef* SPIx , u8 TxData)
{			
	/* 通过外设SPIx发送一个数据 */
	SPI_I2S_SendData(SPIx, TxData);
	/* 检查指定的SPI标志位设置与否:发送缓存空标志位*/
	while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET);
	
	/* 检查指定的SPI标志位设置与否:接受缓存非空标志位 */
	while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET);
	
	/* 返回通过SPIx最近接收的数据 */
	return SPI_I2S_ReceiveData(SPIx); 	
}

/* 写寄存器 */
static u8 writeReg(u8 reg,u8 value)
{
	u8 status;	
	SPI2_CSN_L();                	 	
	status=SPI_RWByte(NRF_SPI,reg|CMD_W_REG);
	SPI_RWByte(NRF_SPI , value); 	
	SPI2_CSN_H();                 	  
	return status;       					
}

/* 读寄存器 */ 
static u8 readReg(u8 reg)
{
	u8 reg_val;	    
 	SPI2_CSN_L();         		 			
	SPI_RWByte(NRF_SPI,reg|CMD_R_REG);
	reg_val=SPI_RWByte(NRF_SPI,0xA5);
	SPI2_CSN_H();     								    
	return reg_val;    						
}	

/* 读缓冲区 */
static u8 readBuf(u8 cmd,u8 *pBuf,u8 len)
{
	u8 status,i;	       
	SPI2_CSN_L();            
	status=SPI_RWByte(NRF_SPI,cmd);   	   
	for(i=0;i<len;i++)
		pBuf[i]=SPI_RWByte(NRF_SPI,0XFF);
	SPI2_CSN_H();
	return status;
}

/* 写缓冲区 */
static u8 writeBuf(u8 cmd,u8 *pBuf,u8 len)
{
	u8 status,i;	    
	SPI2_CSN_L();          
	status=SPI_RWByte(NRF_SPI,cmd);
	for(i=0;i<len;i++)
		SPI_RWByte(NRF_SPI,*pBuf++);  
	SPI2_CSN_H();  			 	
	return status;  
}

/* 发送数据包(PTX模式) */
void nrf_txPacket(u8 *tx_buf,u8 len)
{	
	NRF_CE_L();	
	writeBuf(CMD_W_TX_PAYLOAD,tx_buf,len);
	NRF_CE_H();		 
}

/* 发送NO_ACK数据包(PTX模式) */
void nrf_txPacketNoACK(u8 *tx_buf,u8 len)
{	
	NRF_CE_L();		 
	writeBuf(CMD_W_TX_PAYLOAD_NO_ACK,tx_buf,len);
	NRF_CE_H();		
}

/* 发送ACK数据包，设置0通道(PRX模式) */
void nrf_txPacket_AP(u8 *tx_buf,u8 len)
{	
	NRF_CE_L();		 	
	writeBuf(CMD_W_ACK_PAYLOAD(0),tx_buf,len);
	NRF_CE_H();		 
}
/* 发送NO_ACK数据包(PTX模式) */
void nrf_sendPacketNoACK(u8 *sendBuf,u8 len)
{	
	while((readReg(REG_STATUS)&0x01)!=0);	/* 等待TX_FIFO不为full */
	nrf_txPacketNoACK(sendBuf,len);			/* 发送NO_ACK数据包 */
}

/**************************NRF24L01+配置函数***********************************/
/* 设置发射接收地址，这里收发地址一致 */
void nrf_setAddress(uint64_t address)
{
	writeBuf(CMD_W_REG |REG_RX_ADDR_P0,(u8*)&address,5);//接收使用P0节点
	writeBuf(CMD_W_REG |REG_TX_ADDR,(u8*)&address,5); 		
}

/* 设置频率通道,channel:0~125 */
void nrf_setChannel(u8 channel)
{
	if(channel<=125) 
		writeReg(REG_RF_CH, channel);
}

/* 设置传输速率，dr:0->250KHz、1->1MHz、2->2MHz。 */
void nrf_setDataRate(enum nrfRate dataRate)
{
	u8 reg_rf = readReg(REG_RF_SETUP);
	reg_rf &= ~((1<<5)|(1<<3));/* 清除原设速率 */
	switch(dataRate)
	{
		case DATA_RATE_250K:
			reg_rf |= 0x20;
			break;
		case DATA_RATE_1M:
			reg_rf |= 0x00;
			break;
		case DATA_RATE_2M:
			reg_rf |= 0x08;
			break;	
	}
	writeReg(REG_RF_SETUP,reg_rf); 	
}

/* 设置发射功率,power: 0->-18dB  1->-12dB  2->-6dB  3->0dB */
void nrf_setPower(enum nrfPower power)
{
	u8 reg_rf = readReg(REG_RF_SETUP);
	reg_rf &= 0xF8;/* 清除原设功率 */
	switch(power)
	{
		case POWER_M18dBm:
			reg_rf |= 0x00;
			break;
		case POWER_M12dBm:
			reg_rf |= 0x02;
			break;
		case POWER_M6dBm:
			reg_rf |= 0x04;
			break;
		case POWER_0dBm:
			reg_rf |= 0x07;
			break;	
	}
	writeReg(REG_RF_SETUP,reg_rf);
}

/* 设置重发时间间隔，根据速率及收发字节大小设置 */
/* 详细说明参考nrf24l01.datasheet的P34. */
void nrf_setArd(void)
{
	u8 reg_rf,reg_retr;
	reg_rf=readReg(REG_RF_SETUP);
	reg_retr=readReg(REG_SETUP_RETR);
	if(!(reg_rf&0x20))	/* 速率不是250K(寄存器0x20) */
		reg_retr|= 1<<4;/* (1+1)*250=500us,在接收32字节时 */
	else
		reg_retr|= 5<<4;/* (5+1)*250=1500us,在接收32字节时 */
	writeReg(REG_SETUP_RETR,reg_retr);
}

/* 设置重发次数，arc:0~15 */
void nrf_setArc(u8 arc)
{
	u8 reg_retr;
	if(arc>15)
		return;
	reg_retr=readReg(REG_SETUP_RETR);
	reg_retr|= arc;
	writeReg(REG_SETUP_RETR,reg_retr);
}

/* 获取接收功率检测 */
u8 nrf_getRpd(void)
{
   return readReg(REG_RPD);
}

/* 获取重发失败次数 */
u8 nrf_getTxRetry(void)
{
   return readReg(REG_OBSERVE_TX)&0x0F;
}


/* 初始化NRF24L01配置 */
/* model: PTX_MODE、PRX_MODE */
void nrfInit(enum nrfMode model)
{
	NRF_Init();
	nrf_setAddress(nrfAddress);
	nrf_setChannel(DEFAULT_CHANNEL);
	nrf_setDataRate(DEFAULT_DATARATE);
	nrf_setPower(DEFAULT_POWR);
	nrf_setArd();
	nrf_setArc(3);
	if(model==PRX_MODE)
	{
		writeReg(REG_CONFIG, 0x0f);   		/* IRQ收发完成中断开启,16位CRC,PRX */
		writeReg(REG_DYNPD,0x01);			/* 使能RX_P0动态长度PLAYLOAD */
		writeReg(REG_FEATURE,0x06);			/* 使能动态长度PLAYLOAD、发送ACK PLAYLOAD */
		
		writeReg(REG_EN_AA,0x01); 			/* 使能通道0的自动应答 */	
		
		writeReg(CMD_FLUSH_TX,0xff);		/* 冲洗TX_FIFO */
		writeReg(CMD_FLUSH_RX,0xff);
	}
	else							 	
	{
		writeReg(REG_CONFIG, 0x0e);   		/* IRQ收发完成中断开启,16位CRC,PTX */
		writeReg(REG_DYNPD,0x01);			/* 使能RX_P0动态长度PLAYLOAD */
		writeReg(REG_FEATURE,0x07);			/* 使能动态长度、ACK PLAYLOAD发送、W_TX_PAYLOAD_NOACK */
		
		writeReg(CMD_FLUSH_TX,0xff);		/* 冲洗TX_FIFO */
		writeReg(CMD_FLUSH_RX,0xff);
	}
}

/* 检查MCU与24l01是否通讯正常 */
/* 方法：写入读出地址是否一致 */
ErrorStatus nrf_check(void)
{ 
	uint64_t addr = 0;
	
	NRF_Init();
	writeBuf(CMD_W_REG |REG_TX_ADDR,(u8*)&nrfAddress,5); 
	readBuf(CMD_R_REG|REG_TX_ADDR,(u8*)&addr,5); 
	if(nrfAddress==addr)
		return SUCCESS;
	else
		return ERROR;
}

/* 接收数据包，返回包长度len */
u8 nrf_rxPacket(u8 *rx_buf)
{	
	u8 rx_len = readReg(CMD_RX_PL_WID);
	if(rx_len>0 && rx_len<33)
	{
		NRF_CE_L();	
		readBuf(CMD_R_RX_PAYLOAD,rx_buf,rx_len);
		NRF_CE_H();
	}
	else 
		rx_len = 0;
	writeReg(CMD_FLUSH_RX,0xff);/* 冲洗RX_FIFO */
	return rx_len;		
}

/* 查询事件并接收数据包 */
nrfEvent_e nrf_checkEventandRxPacket(u8 *ackBuf, u8 *acklen)
{
	nrfEvent_e nrfEvent = IDLE;
	*acklen = 0;
	u8 status = readReg(REG_STATUS);/*读事件标志寄存器*/
	if(status&BIT_MAX_RT)/*重发失败*/
	{
		writeReg(CMD_FLUSH_TX,0xff);
		nrfEvent =  MAX_RT;
	}
	else if(status&BIT_RX_DR)/*接收数据到RX_FIFO*/
	{
		*acklen =  nrf_rxPacket(ackBuf);
		nrfEvent = RX_DR;
	}
	else if(status&BIT_TX_DS)/*发送数据至TX_FIFO成功*/
	{
		nrfEvent = TX_DS;
	}
	writeReg(REG_STATUS,0x70);/*清除标志*/
	u8 status1 = readReg(REG_STATUS);/*读事件标志寄存器*/
	status1 = status1;
	return nrfEvent;
}

/* 发送数据包，并等待接收ACK(PTX模式) */
/* 返回值：1成功、0失败*/
u8 nrf_sendPacketWaitACK(u8 *sendBuf, u8 len, u8 *ackBuf, u8 *acklen)
{ 
	if(len==0) return 0;
	nrf_txPacket(sendBuf,len);
	while((readReg(REG_STATUS)&0x70) == 0);/* 等待事件 */
	nrfEvent_e nrfEvent = nrf_checkEventandRxPacket(ackBuf, acklen);
	if(nrfEvent == MAX_RT)
		return 0;
	return 1;
}

/*设置nrf中断回调函数*/
void nrf_setIterruptCallback(void(*cb)(void))
{
	interruptCb = cb;
}

/*外部中断服务函数*/
void EXTI15_10_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line13)==SET)
	{
		if(interruptCb)
		{
			interruptCb();
		}
		EXTI_ClearITPendingBit(EXTI_Line13);
	}
}
