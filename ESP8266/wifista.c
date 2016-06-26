#include "common.h"
#include "stdlib.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//ATK-ESP8266 WIFI模块 WIFI STA驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2015/4/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 

//ATK-ESP8266 WIFI STA测试
//用于测试TCP/UDP连接
//返回值:0,正常
//    其他,错误代码

u8 netpro=0x01;	//网络模式为 wifi-sta

u8 atk_8266_wifista_test(void)
{
	//u8 netpro=0;	//网络模式
	u8 key;
	u8 timex=0; 
	u8 ipbuf[16]; 	//IP缓存
	u8 *p;
	u16 t=999;		//加速第一次获取链接状态
	u8 res=0;
	u16 rlen=0;
	u8 constate=0;	//连接状态
	p=mymalloc(SRAMIN,32);							//申请32字节内存
	atk_8266_send_cmd("AT+CWMODE=1","OK",50);		//设置WIFI STA模式
	atk_8266_send_cmd("AT+RST","OK",20);		    //DHCP服务器关闭(仅AP模式有效) 
	
  printf("设置WIFI STA模式,重启...\r\n");
  
  delay_ms(3000);         //延时3S等待重启成功

	//设置连接到的WIFI网络名称/加密方式/密码,这几个参数需要根据您自己的路由器设置进行修改!! 
	sprintf((char*)p,"AT+CWJAP=\"%s\",\"%s\"",wifista_ssid,wifista_password);//设置无线参数:ssid,密码
  
  printf("\r\n无线参数设置：%s",p);
	
  while(atk_8266_send_cmd(p,"WIFI GOT IP",300));					//连接目标路由器,并且获得IP


    //if(atk_8266_ip_set("WIFI-STA 远端IP设置",(u8*)ATK_ESP8266_WORKMODE_TBL[netpro],(u8*)portnum,(u8*)ipnum)) goto PRESTA;	//IP设置
    
    atk_8266_send_cmd("AT+CIPMUX=0","OK",20);   //0：单连接，1：多连接
    sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",%s",(u8*)ipnum,(u8*)portnum);    //配置目标TCP服务器
    
    printf("\r\n===正在配置ATK-ESP模块,请稍等...\r\n");
    printf("配置为单连接，并访问服务器：%s\r\n",p);
    
    while(atk_8266_send_cmd(p,"OK",200))
    {
        printf("访问指定地址失败，重试...\r\n");
    }	
    
    printf("\r\n===设置为透传\r\n");
		atk_8266_send_cmd("AT+CIPMODE=1","OK",200);      //传输模式为：透传			

    printf("\r\n===获取连接信息\r\n");
    atk_8266_get_wanip(ipbuf);//服务器模式,获取WAN IP
    sprintf((char*)p,"连接状态:%s IP地址:%s 端口:%s",(u8*)ATK_ESP8266_WORKMODE_TBL[netpro],ipbuf,(u8*)portnum);
    printf("\r\n%s\r\n",p);

    //atk_8266_wificonf_show(30,180,"无线参数为:",(u8*)wifista_ssid,(u8*)wifista_encryption,(u8*)wifista_password);
    printf("无线参数为:%s,%s,%s\r\n",(u8*)wifista_ssid,(u8*)wifista_encryption,(u8*)wifista_password);
    
     
    printf("\r\n====WK_UP:退出测试,KEY_R:发送数据\r\n");
    
    USART3_RX_STA=0;
    while(1)
    {
      key=Key_Scan(0);
      if(key==KEY_U_PRESS)			//KEY_U 退出测试		 
      { 
        printf("\r\n===退出测试\r\n");
        res=0;					
        atk_8266_quit_trans();	//退出透传
        atk_8266_send_cmd("AT+CIPMODE=0","OK",20);   //关闭透传模式
        break;												 
      }
      else if(key==KEY_R_PRESS)	//KEY_R 发送数据 
      {
        //atk_8266_quit_trans();
        atk_8266_send_cmd("AT+CIPSEND","OK",20);         //开始透传           
        sprintf((char*)p,"ATK-8266%s测试%d\r\n",ATK_ESP8266_WORKMODE_TBL[netpro],t/10);//测试数据
        
        u3_printf("%s",p);
        printf("[u3-T]%s",p);
        timex=100;
        
      }
    
      if(timex)timex--;
      t++;
      delay_ms(10);
      
      if(USART3_RX_STA&0X8000)		//接收到一次数据了
      { 
        rlen=USART3_RX_STA&0X7FFF;	//得到本次接收到的数据长度
        USART3_RX_BUF[rlen]=0;		//添加结束符 
        sprintf((char*)p,"[收到%d字节数据]%s",rlen,USART3_RX_BUF);//接收到的字节数 
        printf("\r\n%s\r\n",USART3_RX_BUF);	//发送到串口 
        
        USART3_RX_STA=0;
        if(constate!='+')t=1000;		//状态为还未连接,立即更新连接状态
        else t=0;                   //状态为已经连接了,10秒后再检查
      }  
      if(t==1000)//连续10秒钟没有收到任何数据,检查连接是不是还存在.
      {
        printf("检查状态：");
        constate=atk_8266_consta_check();//得到连接状态
        if(constate=='+')
          printf("连接持续\r\n");
        else 
          printf("连接断开\r\n");
        t=0;
      }
      
      if((t%20)==0)LED_Blin_Green();
      
      atk_8266_at_response(1);
    }
    
      
	myfree(SRAMIN,p);		//释放内存 
	return res;		
} 




























