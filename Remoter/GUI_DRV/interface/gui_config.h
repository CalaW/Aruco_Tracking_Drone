
#ifndef  GUI_CONFIG_H
#define  GUI_CONFIG_H

#include <stdint.h>
#include <stdbool.h>

#ifndef TRUE
	#define TRUE  1
#endif

#ifndef FALSE
	#define FALSE 0
#endif

#ifndef NULL
	#define NULL 0
#endif


extern uint8_t const  DCB2HEX_TAB[8];


/* 定义颜色数据类型(可以是数据结构) */
#define  TCOLOR	  uint8_t 					//自行修改，原来是	uint8_t_t 

/* 定义LCM像素数宏 */
#define  GUI_LCM_XMAX		128				/* 定义液晶x轴的像素数 */
#define  GUI_LCM_YMAX		64				/* 定义液晶y轴的像素数 */

/* 定义前景色及背景色变量，用于ASCII码、汉字、窗口、单色位图显示 */
extern TCOLOR	disp_color;
extern TCOLOR	back_color;

/****************************************************************************
* 名称：GUI_CmpColor()
* 功能：判断颜色值是否一致。
* 入口参数：color1		颜色值1
*		   color2		颜色值2
* 出口参数：返回1表示相同，返回0表示不相同。
* 说明：由于颜色类型TCOLOR可以是结构类型，所以需要用户编写比较函数。
****************************************************************************/
#define  GUI_CmpColor(color1, color2)	( (color1&0x01) == (color2&0x01) )

/****************************************************************************
* 名称：GUI_CopyColor()
* 功能：颜色值复制。
* 入口参数：color1		目标颜色变量
*		   color2		源颜色变量
* 出口参数：无
* 说明：由于颜色类型TCOLOR可以是结构类型，所以需要用户编写复制函数。
****************************************************************************/
#define  GUI_CopyColor(color1, color2) 	*color1 = color2



/* 有宽度的直线函数 */
#define   GUI_LineWith_EN		0
/* 画圆函数 */
#define   GUI_CircleX_EN		1
/* 椭圆函数 */
#define   GUI_EllipseX_EN		0
/* 填充函数 */
#define   GUI_FloodFill_EN		1
/* 向上及向下节点个数(填充函数) */
#define   DOWNP_N				20
#define   UPP_N					20
/* 圆弧函数 */
#define   GUI_ArcX_EN			1
/* 扇形函数 */
#define   GUI_Pieslice_EN		0



/* 窗口管理 */
#define   GUI_WINDOW_EN			1

/* 图标菜单 */
#define  GUI_MenuIco_EN			0	

/* 下拉菜单 */
#define  GUI_MenuDown_EN		0



/* 5*7字体 */
#define  FONT5x7_EN				0

/* 6*12字体*/
#define  FONT6x12_EN     		0

/* 8*8字体 */
#define  FONT8x8_EN				0

/* 8*16字体 */
#define  FONT8x16_EN			0

/* 12*12字体*/
#define  FONT12x12_EN    		0

/* 12*24字体*/
#define  FONT12x24_EN     		0

/* 16*32字体 */
#define  FONT16x32_EN			0

/* 14*56字体*/
#define  FONT14x40_EN     		0

/* 24*32字体 */
#define  FONT24x24_EN			0

/* 24*48字体 */
#define  FONT24x48_EN			0

/* 24*54字体*/
#define  FONT24x54_EN     		0

/* 汉字显示函数 */
#define  GUI_PutHZ_EN			0


/* 单色图形显示函数 */
#define	 GUI_LoadPic_EN			0

/* 颜色转换 */
#define  CONVERTCOLOR_EN		0



#endif
