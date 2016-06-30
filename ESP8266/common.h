#ifndef __COMMON_H__
#define __COMMON_H__	 
#include "sys.h"
#include "usart.h"		
#include "delay.h"	
#include "led.h"   	 
#include "key.h"	 	 	 	 	 
#include "lcd.h"   
#include "w25qxx.h" 	 
#include "touch.h" 	 
#include "malloc.h"
#include "string.h"    
#include "text.h"		
#include "usart3.h" 
#include "ff.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//ATK-ESP8266 WIFI模块 公用驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2014/4/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
	
#define showLogInfo    1   //1,详细信息打印至串口，0，不打印
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
void atk_8266_init(void);
void atk_8266_version_msg_show(void);

void atk_8266_at_response(u8 mode);

u8  atk_8266_send_data(u8 *data,u8 *ack,u16 waittime);
u8  atk_8266_send_cmd(u8 *cmd,u8 *ack,u16 waittime);
u8* atk_8266_check_cmd(u8 *str);

u8  atk_8266_start_trans(void);
u8  atk_8266_quit_trans(void);

void atk_8266_get_ip(u8* ipbuf);
u8   atk_8266_connect_wifi_check(void);
u8   atk_8266_connect_server_check(void);

u8 atk_8266_connect_to_wifi(void);
u8 atk_8266_connect_to_server(void);
// wifista.c函数头文件定义
u8 atk_8266_wifista_test(void);	//WIFI STA测试



//用户配置参数
extern const u8* ipnum;			//连接ip
extern const u8* portnum;			//连接端口
 
extern const u8* wifista_ssid;		//WIFI STA SSID
extern const u8* wifista_encryption;//WIFI STA 加密方式
extern const u8* wifista_password; 	//WIFI STA 密码

extern u8* kbd_fn_tbl[2];
extern const u8* ATK_ESP8266_CWMODE_TBL[3];
extern const u8* ATK_ESP8266_WORKMODE_TBL[3];
extern const u8 netpro; // 网络传输协议
extern const u8* ATK_ESP8266_ECN_TBL[5];
#endif





