#include <string.h>
#include "text.h"	
#include "oled.h"
#include "font.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板V3
//汉字显示驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/1/20
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 
 
#define  LCD_WIDTH		128
#define  LCD_HIGH		64

 const char* font1212_table = {
"控制模式飞行速度翻滚使能摇杆校准\
匹配四轴语言选择重置退出扩展块飛\
滾搖桿準軸語選擇擴塊定高手动点動\
點无头有無頭低中开启关闭開啟關閉\
简体文繁未发现！灯环摄像光流發現\
燈環攝两个一圈兩個按下步保持回完\
成提示：已…通信失败请先遥和再。\
敗請遙功确恢复为默认设？確復為認\
設取消颜色测试姿态感应陀螺闪烁警\
报电筒呼吸水顏測試態應閃爍報電激"
};
 
 const char* font2424_table = {
"控四遥遙轴軸"
};

//显示一个指定大小的汉字
//x,y :汉字的坐标
//font:汉字GBK码
//f_w:字宽
//f_h:字高
//mode:0,正常显示,1,叠加显示
static void show_font(uint16_t x,uint16_t y,const uint8_t *font,uint8_t f_w,uint8_t f_h,uint8_t mode)
{
	const char* font_table;
	uint16_t fontSeq;
	uint8_t temp,t,t1;
	uint16_t y0=y; 
	uint8_t csize=(f_h/8+((f_h%8)?1:0))*f_w;//得到自由分辨字符所占的字节数 

	if(f_w==12 && f_h==12)
		font_table = font1212_table;
	else if(f_w==24 && f_h==24)
		font_table = font2424_table;
	else return;/*没有的字库*/
	for(fontSeq=0; fontSeq<strlen(font_table)/2; fontSeq++)/*计算font_table对应字库的数组下标*/
	{
		if(font_table[2*fontSeq]==font[0] && font_table[2*fontSeq+1]==font[1])
			break;
	}
	if(fontSeq >= strlen(font_table)/2) return;/*font_table中没有font该字*/

	for(t=0;t<csize;t++)
	{   												   
		if(f_w==12 && f_h==12)
			temp = font_1212[fontSeq][t];/*调用font_1212字库*/
		else if(f_w==24 && f_h==24)
			temp = font_2424[fontSeq][t];/*调用font_2424字库*/
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)oled_drawPoint(x,y,mode);
			else oled_drawPoint(x,y,!mode);
			temp<<=1;
			y++;
			if((y-y0)==f_h)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
	}  
}
//在指定位置开始显示一个字符串	    
//支持自动换行
//(x,y):起始坐标
//width,height:区域
//str  :字符串
//f_w:字宽
//f_h:字高
//mode:0,非叠加方式;1,叠加方式    	   		   
void show_str(uint16_t x,uint16_t y,const uint8_t*str,uint8_t f_w,uint8_t f_h,uint8_t mode)
{					
	uint16_t x0=x;
	uint16_t y0=y;							  	  
    uint8_t bHz=0;     //字符或者中文  	    				    				  	  
    while(*str!=0)//数据未结束
    { 
        if(!bHz)
        {
	        if(*str>0x80)bHz=1;//中文 
	        else               //字符
	        {      
                if(x>(x0+LCD_WIDTH-f_h/2))//换行
				{				   
					y+=f_h;
					x=x0;	   
				}							    
		        if(y>(y0+LCD_HIGH-f_h))break;//越界返回      
		        if(*str==13)//换行符号
		        {         
		            y+=f_h;
					x=x0;
		            str++; 
		        }  
		        else oled_showChar(x,y,*str,f_w/2,f_h,mode);//有效部分写入 
				str++; 
		        x+=f_h/2; //字符,为全字的一半 
	        }
        }else//中文 
        {     
            bHz=0;//有汉字库    
            if(x>(x0+LCD_WIDTH-f_h))//换行
			{	    
				y+=f_h+2;//2为行间距
				x=x0;		  
			}
	        if(y>(y0+LCD_HIGH-f_h))break;//越界返回  						     
	        show_font(x,y,str,f_w,f_h,mode); //显示这个汉字,空心显示 
	        str+=2; 
	        x+=f_h;//下一个汉字偏移	    
        }						 
    }   
}  			 		 
//在指定宽度的中间显示字符串
//如果字符长度超过了len,则用Show_Str显示
//len:指定要显示的宽度			  
void show_str_mid(uint16_t x,uint16_t y,const uint8_t*str,uint8_t f_w,uint8_t f_h,uint8_t mode,uint16_t len)
{
	uint16_t strlenth=0;
   	strlenth=strlen((char*)str);
	strlenth*=f_h/2;
	if(strlenth>len)show_str(x,y,str,f_w,f_h,mode);
	else
	{
		strlenth=(len-strlenth)/2;
	    show_str(strlenth+x,y,str,f_w,f_h,mode);
	}
}   

