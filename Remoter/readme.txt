
最新资料下载地址:
	http://www.openedv.com/docs/index.html
	
硬件资源：
	1,MCU:STM32F103C8T6 (FLAH:64K, RAM:20K, 系统运行时钟频率:72MHz) 
	2,无线通信NRF24L01+(CE:PA3, CSN:PA4, SCK:PA5, MISO:PA5, MOSI:PA6, IRQ:PC13) 
	3,油门摇杆（ADC_YAW:PB0, ADC_THRUST: PB1）
	4,方向摇杆（ADC_ROLL:PA1, ADC_PITCH: PA2）
	5,OLED(MOSI:PB15, RST:PB14, CLK:PB13, DC:PB12)
	6,按键(KEY_J1：PB10, KEY_J2：PA8, KEY_L:PB11, KEY_R:PC15)
	7,通信指示灯(RED_LED0:PB7, BLUE_LED1:PB3)
	8,蜂鸣器（PC14）

实验现象：
	遥控器开机后显示主界面，蜂鸣器“滴”一声后四轴可飞行。
	
	灯语：
	    RED_LED0亮：与四轴通信失败
	    BLUE_LED1亮：与四轴通信成功

注意事项:
	代码下载和调试前，请将下载器开关拨到STM32档。
	bootloader起始地址（BOOTLOADER_START_ADDR） : 0x8000000
	固件起始地址（FIRMWARE_START_ADDR） : 0x8002400

固件更新记录:
	V1.0 Release(硬件版本:V1.6, DATE:2017-06-30)
		
	V1.1 Release(硬件版本:V1.6, DATE:2017-10-18)
		1.修改config_param.h将配置参数保存起始地址由第63更改至127K
		2.GUI_APP增加menuL3_item.c，增加扩展模块的菜单项
	
	V1.2 Release(硬件版本:V1.7, DATE:2018-06-22)
		1.控制模式增加定点模式(需配合光流模块),此模式下，通过光电定点，激光(2m以内)定高的方式实现四轴稳定悬停，
			如果挂载光流模块，此模式作用相同于定高模式.注意：此模式关闭空翻功能.
	
	V1.2.1 Release(硬件版本:V1.7, DATE:2018-08-16)
		1.在发送遥控数据之前 ，增加MiniFly可飞行状态判断.
		
	V1.3 Release(硬件版本:V1.7, DATE:2018-11-11)
		1.搭载光流模块，用户可选择是否使用激光传感器定高，方便用户户外飞行;
		2.增加微调记录功能，同一个遥控器，控制另一台四轴，无需重新微调;
	
	V1.4 Release(硬件版本:V1.7, DATE:2021-04-20)
		1.修改oled.c SPI读写字节函数，防止出现OLED黑屏的情况;
		2.更改下载调试器为ATK Mini-CMSIS-DAP，同时需要注意的是，下载的时候需要USB给遥控器供电，否则可能导致下载出错;
	
	V1.4 Release(硬件版本:V1.7, DATE:2021-04-20)
		1. 配置FreeRTOSConfig.h 中configUSE_TICKLESS_IDLE 为0，关闭低功耗模式。如果开启了，使用DAP下载器
		   可能会报错。
					
					正点原子@ALIENTEK
					2018-06-22
					广州市星翼电子科技有限公司
					电话：020-38271790
					传真：020-36773971
					购买：http://shop62103354.taobao.com
					http://shop62057469.taobao.com
					公司网站：www.alientek.com
					技术论坛：www.openedv.com





































