#ifndef __OLED_H
#define __OLED_H			  	 
#include "sys.h"

/********************************************************************************	 
 * 本程序只供学习使用，未经作者许可，不得用于其它任何用途
 * ALIENTEK MiniFly_Remotor
 * SSD1306 OLED驱动代码
 * 驱动方式:STM32硬件SPI 
 * 正点原子@ALIENTEK
 * 技术论坛:www.openedv.com
 * 创建日期:2018/6/1
 * 版本：V1.0
 * 版权所有，盗版必究。
 * Copyright(C) 广州市星翼电子科技有限公司 2014-2024
 * All rights reserved
********************************************************************************/

#define  OLED_SPI   SPI2				  
//#define OLED_CS  	PDout(6)
#define OLED_RST 	PBout(14) 	
#define OLED_RS  	PBout(12)

#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

//OLED控制用函数
void oled_wr_byte(u8 dat,u8 cmd);	    
void oled_displayON(void);
void oled_displayOFF(void);
void oled_refreshGram(void);  		    
void oledInit(void);
void oled_clear(void);
void oled_drawPoint(u8 x,u8 y,u8 t);
u8 oled_readPoint(u8 x,u8 y);
void oled_fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
void oled_showChar(u8 x,u8 y,u8 chr,u8 f_w,u8 f_h,u8 mode);
void oled_showNum(u8 x,u8 y,u32 num,u8 len,u8 f_w,u8 f_h);
void oled_showString(u8 x,u8 y,const u8 *p,u8 f_w,u8 f_h);	
void oled_showPicture(u8 x,u8 y,const u8 *p,u8 p_w,u8 p_h);
#endif  
	 







 

