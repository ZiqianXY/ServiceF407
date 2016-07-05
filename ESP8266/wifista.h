#ifndef __WIFI_STA_H__
#define __WIFI_STA_H__	 
#include "sys.h"

//////////////////////////////////////////////////////////////////////////
// wifista.c����ͷ�ļ�����
//////////////////////////////////////////////////////////////////////////

void WIFI_Init(void);
u8 WIFI_test(void);	//WIFI STA����
u8 WIFI_send_data(char *data);	//��������

u8 WIFI_connect2wifi(void);
u8 WIFI_connect2server(void);
u8 WIFI_trans_start(void);
u8 WIFI_trans_quit(void);

#endif
