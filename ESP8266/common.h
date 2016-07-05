#ifndef __COMMON_H__
#define __COMMON_H__	 
#include "sys.h"
#include "usart.h"		
#include "delay.h"	
#include "led.h"   	 
#include "key.h"	 	 	 	 	 
#include "w25qxx.h" 	 
#include "malloc.h"
#include "string.h"    
#include "usart3.h" 
#include "ff.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//ATK-ESP8266 WIFIģ�� ������������	   						  
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
	
#define showLogInfo    0   //1,��ϸ��Ϣ��ӡ�����ڣ�0������ӡ
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
void atk_8266_init(void);
void atk_8266_version_msg_show(void);

void atk_8266_at_response(u8 mode);

u8  atk_8266_send_data(u8 *data,u8 *ack,u16 waittime);
u8  atk_8266_send_cmd(u8 *cmd,u8 *ack,u16 waittime);
u8* atk_8266_check_cmd(u8 *str);

void atk_8266_get_ip(u8* ipbuf);
u8   atk_8266_connect_wifi_check(void);
u8   atk_8266_connect_server_check(void);

u8 atk_8266_connect_to_wifi(void);
u8 atk_8266_connect_to_server(void);
u8  atk_8266_start_trans(void);
u8  atk_8266_quit_trans(void);

//�û����ò���
extern const u8* ipnum;			//����ip
extern const u8* portnum;			//���Ӷ˿�
 
extern const u8* wifista_ssid;		//WIFI STA SSID
extern const u8* wifista_encryption;//WIFI STA ���ܷ�ʽ
extern const u8* wifista_password; 	//WIFI STA ����

extern u8* kbd_fn_tbl[2];
extern const u8* ATK_ESP8266_CWMODE_TBL[3];
extern const u8* ATK_ESP8266_WORKMODE_TBL[3];
extern const u8 netpro; // ���紫��Э��
extern const u8* ATK_ESP8266_ECN_TBL[5];
#endif
