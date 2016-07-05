#include "common.h"
#include "wifista.h"
#include "stdlib.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//ATK-ESP8266 WIFI模块 WIFI STA驱动代码	   								  
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 

//ATK-ESP8266 WIFI STA测试
//用于测试TCP/UDP连接
//返回值:0,正常
//    其他,错误代码

u8 WIFI_test(void){
	u8 key;
	u8 ipbuf[16]; 	//IP缓存
	u8 *p;
	u16 t=999;		//加速第一次获取链接状态
	u8 res=0;
    static u8 FIRST=1;
	p=mymalloc(SRAMIN,32);							//申请32字节内存

    printf("\r\n\r\n=====ATK-ESP8266 WIFI模块测试=====\r\n");
    
    atk_8266_init();
    atk_8266_connect_to_wifi();
    atk_8266_get_ip(ipbuf);
    atk_8266_connect_to_server();
    
    printf("\r\n====提示===WK_UP:退出测试,KEY_R:发送数据\r\n");
    USART3_RX_STA=0;
    while(1)
    {
      key=Key_Scan(0);
      if(key==KEY_U_PRESS)			//KEY_U 退出测试		 
      { 
        printf("\r\n===WK_UP,退出测试\r\n");
        res=0;					
        atk_8266_quit_trans();	//退出透传
        break;												 
      }
      else if(key==KEY_R_PRESS)	//KEY_R 发送数据 
      {
        printf("\r\n===KEY_R,退出测试\r\n");
        if(FIRST){
            atk_8266_start_trans();
            FIRST = 0;
        }
        sprintf((char*)p,"ATK-8266%s测试%d\r\n",ATK_ESP8266_WORKMODE_TBL[netpro],t);//测试数据
        res = WIFI_send_data(p);
      }
      delay_ms(10);
      t++;
      if((t%20)==0)LED_Blin_Green();
    }
	myfree(SRAMIN,p);		//释放内存 
	return res;		
} 


void WIFI_Init(void){
    atk_8266_init();
}

u8 WIFI_send_data(char *data)	//发送数据
{
    u3_printf("%s",data);
    return 0;
}

u8 WIFI_connect2wifi(void){
    return atk_8266_connect_to_wifi();
}

u8 WIFI_connect2server(void){
    return atk_8266_connect_to_server();
}

u8 WIFI_trans_start(void){
    return atk_8266_start_trans();
}

u8 WIFI_trans_quit(void){
    return atk_8266_quit_trans();
}
