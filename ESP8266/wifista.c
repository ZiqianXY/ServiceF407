#include "common.h"
#include "wifista.h"
#include "stdlib.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//ATK-ESP8266 WIFIģ�� WIFI STA��������	   								  
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 

//ATK-ESP8266 WIFI STA����
//���ڲ���TCP/UDP����
//����ֵ:0,����
//    ����,�������

u8 WIFI_test(void){
	u8 key;
	u8 ipbuf[16]; 	//IP����
	u8 *p;
	u16 t=999;		//���ٵ�һ�λ�ȡ����״̬
	u8 res=0;
    static u8 FIRST=1;
	p=mymalloc(SRAMIN,32);							//����32�ֽ��ڴ�

    printf("\r\n\r\n=====ATK-ESP8266 WIFIģ�����=====\r\n");
    
    atk_8266_init();
    atk_8266_connect_to_wifi();
    atk_8266_get_ip(ipbuf);
    atk_8266_connect_to_server();
    
    printf("\r\n====��ʾ===WK_UP:�˳�����,KEY_R:��������\r\n");
    USART3_RX_STA=0;
    while(1)
    {
      key=Key_Scan(0);
      if(key==KEY_U_PRESS)			//KEY_U �˳�����		 
      { 
        printf("\r\n===WK_UP,�˳�����\r\n");
        res=0;					
        atk_8266_quit_trans();	//�˳�͸��
        break;												 
      }
      else if(key==KEY_R_PRESS)	//KEY_R �������� 
      {
        printf("\r\n===KEY_R,�˳�����\r\n");
        if(FIRST){
            atk_8266_start_trans();
            FIRST = 0;
        }
        sprintf((char*)p,"ATK-8266%s����%d\r\n",ATK_ESP8266_WORKMODE_TBL[netpro],t);//��������
        res = WIFI_send_data(p);
      }
      delay_ms(10);
      t++;
      if((t%20)==0)LED_Blin_Green();
    }
	myfree(SRAMIN,p);		//�ͷ��ڴ� 
	return res;		
} 


void WIFI_Init(void){
    atk_8266_init();
}

u8 WIFI_send_data(char *data)	//��������
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
