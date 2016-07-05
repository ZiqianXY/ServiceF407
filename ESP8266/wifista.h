#ifndef __WIFI_STA_H__
#define __WIFI_STA_H__	 
#include "sys.h"

//////////////////////////////////////////////////////////////////////////
// wifista.c函数头文件定义
//////////////////////////////////////////////////////////////////////////

void WIFI_Init(void);
u8 WIFI_test(void);	//WIFI STA测试
u8 WIFI_send_data(char *data);	//发送数据

u8 WIFI_connect2wifi(void);
u8 WIFI_connect2server(void);
u8 WIFI_trans_start(void);
u8 WIFI_trans_quit(void);

#endif
