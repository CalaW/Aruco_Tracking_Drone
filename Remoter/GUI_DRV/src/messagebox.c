#include "messagebox.h"
#include "gui_basic.h"
#include "string.h"
#include "button.h"
#include "delay.h"
#include "text.h"

//得到字符串的长度
uint16_t getMessageBoxStringlen(uint8_t *str)
{
	uint16_t strlenth=0;
	strlenth=strlen((const char*)str);//字节数
	strlenth*=6;//一个字节水平6个点(12x12汉字)
	return strlenth;
}

Button_Typedef btn_ok_messagebox={
20,
20,
48,
15,
"OK",
1
};

Button_Typedef btn_cancel_messagebox={
20,
20,
48,
15,
"cancel",
1
};


//画消息框
uint8_t GUI_MessageBoxDraw(MessageBox_Typedef* messagebox)
{
	/* 参数过滤，若窗口起出范围，则返回0 */
	if( ( (messagebox->with)<20 ) || ( (messagebox->hight)<20 ) ) return(0);// 宽度、高度检查，限制最小窗口
	if( (messagebox->x + messagebox->with ) > GUI_LCM_XMAX ) return(0);		// 窗口宽度是否溢出
	if( (messagebox->y + messagebox->hight ) > GUI_LCM_YMAX ) return(0);	// 窗口高度是否溢出

	/* 开始画窗口 */
	GUI_RectangleFill(messagebox->x, messagebox->y, messagebox->x + messagebox->with - 1, messagebox->y + messagebox->hight - 1, 0);//清屏
	GUI_Rectangle(messagebox->x, messagebox->y, messagebox->x + messagebox->with - 1, messagebox->y + messagebox->hight - 1, 1);// 画窗口(边框)
	GUI_HLine(messagebox->x, messagebox->y + 14, messagebox->x + messagebox->with - 1, disp_color);		// 画标题下边框	 

	//填充标题栏
	GUI_RectangleFill(messagebox->x, messagebox->y, messagebox->x + messagebox->with - 1, messagebox->y + 14-1, 1);
	if(getMessageBoxStringlen(messagebox->title)>(messagebox->with-2))
	{  	
		return 0;//显示的标题超过了窗体的宽度
	} 
	//显示标题
	show_str_mid(messagebox->x, messagebox->y+1,messagebox->title,12,12,0,messagebox->with);


	//确定按钮
	btn_ok_messagebox.state = 0;
	btn_ok_messagebox.title = messagebox->ok;
	btn_ok_messagebox.x = messagebox->x+5;
	btn_ok_messagebox.y = messagebox->y+messagebox->hight-1 -btn_ok_messagebox.height -3;
	GUI_DrawButton(&btn_ok_messagebox);

	//取消按钮	  
	btn_cancel_messagebox.state = 1;
	btn_cancel_messagebox.title = messagebox->cancel;
	btn_cancel_messagebox.x = messagebox->x+ messagebox->with -btn_cancel_messagebox.width-1-5;
	btn_cancel_messagebox.y = messagebox->y+messagebox->hight-1 -btn_cancel_messagebox.height-3;	 
	GUI_DrawButton(&btn_cancel_messagebox); 

	//显示消息
	show_str(messagebox->x+6, messagebox->y+1+16,messagebox->msg, 12,12,1);
	return(1);
}

//设置按钮状态
void GUI_MessageBoxButtonStatus(MessageBox_Typedef* messagebox, uint8_t status)
{	
	uint8_t ok_status;
	uint8_t cancel_status;

	if(status)
	{
		ok_status =1;
		cancel_status  =0;			
	}else
	{
		ok_status =0;
		cancel_status  =1;	
	}
	//确定按钮
	GUI_Button_Selected(&btn_ok_messagebox,ok_status);

	//取消按钮	  
	GUI_Button_Selected(&btn_cancel_messagebox,cancel_status);

}

//得到当前选择结果
uint8_t GUI_MessageBoxResult(MessageBox_Typedef* messagebox)
{
	return btn_ok_messagebox.state;	
}

//删除消息框
uint8_t GUI_MessageBoxDelete(MessageBox_Typedef* messagebox)
{	
   if( ( (messagebox->with)<20 ) || ( (messagebox->hight)<20 ) ) return(0);		// 宽度、高度检查，限制最小窗口
   if( (messagebox->x + messagebox->with ) > GUI_LCM_XMAX ) return(0);			// 窗口宽度是否溢出
   if( (messagebox->y + messagebox->hight ) > GUI_LCM_YMAX ) return(0);			// 窗口高度是否溢出
    
   /* 消隐窗口 */
   GUI_RectangleFill(messagebox->x, messagebox->y, messagebox->x + messagebox->with - 1, messagebox->y + messagebox->hight - 1, back_color);
   return(1);
}

//MessageBox_Typedef MessageBox={
//0,
//0,	
//128,
//64,
//"reset",	
//"system will be resetni hao a1234?",
//"OK",
//"Cancel"
//};


////测试消息框
//void test_MessageBox(void)
//{
// GUI_MessageBoxDraw(&MessageBox);
// GUI_Refresh();
// delay_ms(2000);
// GUI_MessageBoxButtonStatus(&MessageBox, 1);	
// GUI_Refresh();
// delay_ms(2000);
// GUI_MessageBoxButtonStatus(&MessageBox, 0);		
// GUI_Refresh();
// delay_ms(2000);	
//GUI_MessageBoxDelete(&MessageBox);	
//	
//	
//}




