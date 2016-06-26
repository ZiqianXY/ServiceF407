#include "common.h"
#include "stdlib.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//ATK-ESP8266 WIFIģ�� WIFI STA��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2015/4/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 

//ATK-ESP8266 WIFI STA����
//���ڲ���TCP/UDP����
//����ֵ:0,����
//    ����,�������

u8 netpro=0x01;	//����ģʽΪ wifi-sta

u8 atk_8266_wifista_test(void)
{
	//u8 netpro=0;	//����ģʽ
	u8 key;
	u8 timex=0; 
	u8 ipbuf[16]; 	//IP����
	u8 *p;
	u16 t=999;		//���ٵ�һ�λ�ȡ����״̬
	u8 res=0;
	u16 rlen=0;
	u8 constate=0;	//����״̬
	p=mymalloc(SRAMIN,32);							//����32�ֽ��ڴ�
	atk_8266_send_cmd("AT+CWMODE=1","OK",50);		//����WIFI STAģʽ
	atk_8266_send_cmd("AT+RST","OK",20);		    //DHCP�������ر�(��APģʽ��Ч) 
	
  printf("����WIFI STAģʽ,����...\r\n");
  
  delay_ms(3000);         //��ʱ3S�ȴ������ɹ�

	//�������ӵ���WIFI��������/���ܷ�ʽ/����,�⼸��������Ҫ�������Լ���·�������ý����޸�!! 
	sprintf((char*)p,"AT+CWJAP=\"%s\",\"%s\"",wifista_ssid,wifista_password);//�������߲���:ssid,����
  
  printf("\r\n���߲������ã�%s",p);
	
  while(atk_8266_send_cmd(p,"WIFI GOT IP",300));					//����Ŀ��·����,���һ��IP


    //if(atk_8266_ip_set("WIFI-STA Զ��IP����",(u8*)ATK_ESP8266_WORKMODE_TBL[netpro],(u8*)portnum,(u8*)ipnum)) goto PRESTA;	//IP����
    
    atk_8266_send_cmd("AT+CIPMUX=0","OK",20);   //0�������ӣ�1��������
    sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",%s",(u8*)ipnum,(u8*)portnum);    //����Ŀ��TCP������
    
    printf("\r\n===��������ATK-ESPģ��,���Ե�...\r\n");
    printf("����Ϊ�����ӣ������ʷ�������%s\r\n",p);
    
    while(atk_8266_send_cmd(p,"OK",200))
    {
        printf("����ָ����ַʧ�ܣ�����...\r\n");
    }	
    
    printf("\r\n===����Ϊ͸��\r\n");
		atk_8266_send_cmd("AT+CIPMODE=1","OK",200);      //����ģʽΪ��͸��			

    printf("\r\n===��ȡ������Ϣ\r\n");
    atk_8266_get_wanip(ipbuf);//������ģʽ,��ȡWAN IP
    sprintf((char*)p,"����״̬:%s IP��ַ:%s �˿�:%s",(u8*)ATK_ESP8266_WORKMODE_TBL[netpro],ipbuf,(u8*)portnum);
    printf("\r\n%s\r\n",p);

    //atk_8266_wificonf_show(30,180,"���߲���Ϊ:",(u8*)wifista_ssid,(u8*)wifista_encryption,(u8*)wifista_password);
    printf("���߲���Ϊ:%s,%s,%s\r\n",(u8*)wifista_ssid,(u8*)wifista_encryption,(u8*)wifista_password);
    
     
    printf("\r\n====WK_UP:�˳�����,KEY_R:��������\r\n");
    
    USART3_RX_STA=0;
    while(1)
    {
      key=Key_Scan(0);
      if(key==KEY_U_PRESS)			//KEY_U �˳�����		 
      { 
        printf("\r\n===�˳�����\r\n");
        res=0;					
        atk_8266_quit_trans();	//�˳�͸��
        atk_8266_send_cmd("AT+CIPMODE=0","OK",20);   //�ر�͸��ģʽ
        break;												 
      }
      else if(key==KEY_R_PRESS)	//KEY_R �������� 
      {
        //atk_8266_quit_trans();
        atk_8266_send_cmd("AT+CIPSEND","OK",20);         //��ʼ͸��           
        sprintf((char*)p,"ATK-8266%s����%d\r\n",ATK_ESP8266_WORKMODE_TBL[netpro],t/10);//��������
        
        u3_printf("%s",p);
        printf("[u3-T]%s",p);
        timex=100;
        
      }
    
      if(timex)timex--;
      t++;
      delay_ms(10);
      
      if(USART3_RX_STA&0X8000)		//���յ�һ��������
      { 
        rlen=USART3_RX_STA&0X7FFF;	//�õ����ν��յ������ݳ���
        USART3_RX_BUF[rlen]=0;		//��ӽ����� 
        sprintf((char*)p,"[�յ�%d�ֽ�����]%s",rlen,USART3_RX_BUF);//���յ����ֽ��� 
        printf("\r\n%s\r\n",USART3_RX_BUF);	//���͵����� 
        
        USART3_RX_STA=0;
        if(constate!='+')t=1000;		//״̬Ϊ��δ����,������������״̬
        else t=0;                   //״̬Ϊ�Ѿ�������,10����ټ��
      }  
      if(t==1000)//����10����û���յ��κ�����,��������ǲ��ǻ�����.
      {
        printf("���״̬��");
        constate=atk_8266_consta_check();//�õ�����״̬
        if(constate=='+')
          printf("���ӳ���\r\n");
        else 
          printf("���ӶϿ�\r\n");
        t=0;
      }
      
      if((t%20)==0)LED_Blin_Green();
      
      atk_8266_at_response(1);
    }
    
      
	myfree(SRAMIN,p);		//�ͷ��ڴ� 
	return res;		
} 




























