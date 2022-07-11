#include "scrollbar.h"
#include "gui_basic.h"
#include "delay.h"
#include "oled.h"

#define SCROLL_BTN_WIDTH  10  //按钮的宽度
#define SCROLL_BTN_HEIGHT 10	//按钮的高度

//上按钮
const unsigned char scroll_btn_up[8]={
0x02,0x0E,0x3E,0x7E,0x7E,0x3E,0x0E,0x02,
};

//下按钮
const unsigned char scroll_btn_down[8]={
0x40,0x70,0x7C,0x7E,0x7E,0x7C,0x70,0x40,
};

//设置滚动条的位置
void GUI_Scrollbar_SetPos(Scrollbar_Typedef *scbx)
{	
	uint16_t scbpos;  //滚动条偏移
	uint16_t scblen;  //滚动条可以滚动的长度
	uint16_t x,y,xlen,ylen;
//	uint8_t scbplen;
//	
//	scbplen = SCROLL_BTN_HEIGHT;  //得到按钮的高度
	
	scblen=scbx->height-2*SCROLL_BTN_HEIGHT-scbx->scbbarlen;//得到滚动条滚动空间的长度
	x=scbx->x+1;                  //滚动条起始坐标x
	y=scbx->y+SCROLL_BTN_HEIGHT;  //滚动条起始坐标y
	xlen=scbx->width-2-1;         //滚动条宽度 -1
	ylen=scblen+scbx->scbbarlen-1;//滚动条长度 -1
	
	//清除滚动条
	GUI_RectangleFill(x ,y,x+xlen,y+ylen, 0);
	if(scbx->totalitems <= scbx->itemsperpage)//总条目不够一页显示的
	{
		scbpos=0; //偏移0
	}
	else //不止一页
	{
		scbpos=(scbx->topitem*scblen)/(scbx->totalitems-1);//计算当前滚动条偏移的位置
	}
	
	//画出滚动条
	GUI_RectangleFill(x ,y+scbpos,x+xlen,y+scbpos+scbx->scbbarlen, 1);
	//GUI_Refresh();	//刷新屏幕	
}



//创建一个滚动条
void GUI_ScrollbarDraw(Scrollbar_Typedef *scbx)
{
	uint32_t scbactlen=0;//滚动条可以滚动的空间长度
	if(scbx==NULL) return;
	if(scbx->width<SCROLL_BTN_WIDTH) return;
	if(scbx->height<2*SCROLL_BTN_HEIGHT) return;
	
	//画外边框
	GUI_Rectangle(scbx->x, scbx->y, scbx->x+scbx->width-1, scbx->y+scbx->height-1, 1);
	//画上下两个按钮
	oled_showPicture(scbx->x+(scbx->width-8)/2, scbx->y+1, (uint8_t *)scroll_btn_up, 8, 8);
	oled_showPicture(scbx->x+(scbx->width-8)/2, scbx->y+scbx->height-9, (uint8_t *)scroll_btn_down, 8, 8);	
	//画按钮边框
	GUI_Line(scbx->x, scbx->y+9, scbx->x+scbx->width-1, scbx->y+9,1);
	GUI_Line(scbx->x, scbx->y+scbx->height-10, scbx->x+scbx->width-1, scbx->y+scbx->height-10,1);
	
	
	//可供滚动条滚动的空间范围
	scbactlen=scbx->height -2*SCROLL_BTN_HEIGHT;
	
	//每页滚动条目大于总条目数 滚动条目等于最大可滚动的控件
	if(scbx->itemsperpage >= scbx->totalitems)scbx->scbbarlen=scbactlen;
	//每页滚动条目小于总条目数 
	else scbx->scbbarlen=(scbactlen*scbx->itemsperpage)/scbx->totalitems;//得到滚动条的长度
	//设置滚动条的位置
	GUI_Scrollbar_SetPos(scbx);
}


//删除一个滚动条
void GUI_ScrollbarDelete(Scrollbar_Typedef *scbx)
{
  GUI_RectangleFill(scbx->x, scbx->y, scbx->x+scbx->width-1, scbx->y+scbx->height-1, 0);

}

Scrollbar_Typedef Scrollbar=
{
118,
14,
10,
50,
	
9,
3,
9,
0
};

//测试滚动条函数
void test_scrollbar(void){

	GUI_ScrollbarDraw(&Scrollbar);
	GUI_Refresh();								//刷新屏幕
	delay_ms(2000);
	Scrollbar.topitem = 1;
	GUI_Scrollbar_SetPos(&Scrollbar);
	GUI_Refresh();								//刷新屏幕
	delay_ms(2000);
//	GUI_ScrollbarDraw(&Scrollbar);
	
}




