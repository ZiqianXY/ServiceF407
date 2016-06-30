#include "common.h"
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


u8 atk_8266_wifista_test(void){
	u8 key;
	u8 ipbuf[16]; 	//IP����
	u8 *p;
	u16 t=999;		//���ٵ�һ�λ�ȡ����״̬
	u8 res=0;

	p=mymalloc(SRAMIN,32);							//����32�ֽ��ڴ�

    printf("\r\n\r\n=====ATK-ESP8266 WIFIģ�����=====\r\n");
    
    atk_8266_init();
    
    atk_8266_connect_to_wifi();
    
    atk_8266_get_ip(ipbuf);
    
    atk_8266_connect_to_server();
    
    printf("\r\n====��ʾ===\r\nWK_UP:�˳�����,KEY_R:��������\r\n");
    
    USART3_RX_STA=0;
    while(1)
    {
      key=Key_Scan(0);
      if(key==KEY_U_PRESS)			//KEY_U �˳�����		 
      { 
        printf("\r\n===�˳�����\r\n");
        res=0;					
        atk_8266_quit_trans();	//�˳�͸��
        break;												 
      }
      else if(key==KEY_R_PRESS)	//KEY_R �������� 
      {
        atk_8266_start_trans();
        
        sprintf((char*)p,"ATK-8266%s����%d\r\n",ATK_ESP8266_WORKMODE_TBL[netpro],t);//��������
        //u3_printf("%s",p);
        atk_8266_send_data(p,0,0);
        printf("[u3-T]%s",p);
      }
      delay_ms(10);
      t++;
      if((t%20)==0)LED_Blin_Green();
    }
      
	myfree(SRAMIN,p);		//�ͷ��ڴ� 
	return res;		
} 




























