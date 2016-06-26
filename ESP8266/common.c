#include "common.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//ATK-ESP8266 WIFI模块 公用驱动代码	   						  
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//用户配置区

//连接端口号:8086,可自行修改为其他端口.
const u8* ipnum="192.168.1.115";	
const u8* portnum="8888";		 

//WIFI STA模式,设置要去连接的路由器无线参数,请根据你自己的路由器设置,自行修改.
//const u8* wifista_ssid="nao";			//路由器SSID号
//const u8* wifista_encryption="wpawpa2_aes";	//wpa/wpa2 aes加密方式
//const u8* wifista_password="naonaonao"; 	//连接密码

const u8* wifista_ssid="VM_1001_2.4";		//路由器SSID号
const u8* wifista_encryption="wpawpa2_aes";	//wpa/wpa2 aes加密方式
const u8* wifista_password="1001VM1005"; 	//连接密码

/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//4个网络模式
const u8 *ATK_ESP8266_CWMODE_TBL[3]={"STA模式 ","AP模式 ","AP&STA模式 "};	//ATK-ESP8266,3种网络模式,默认为路由器(ROUTER)模式 
//4种工作模式
const u8 *ATK_ESP8266_WORKMODE_TBL[3]={"TCP服务器","TCP客户端"," UDP 模式"};	//ATK-ESP8266,4种工作模式
//5种加密方式
const u8 *ATK_ESP8266_ECN_TBL[5]={"OPEN","WEP","WPA_PSK","WPA2_PSK","WPA_WAP2_PSK"};
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 

//usmart支持部分
//将收到的AT指令应答数据返回给电脑串口
//mode:0,不清零USART3_RX_STA;
//     1,清零USART3_RX_STA;
void atk_8266_at_response(u8 mode)
{
	if(USART3_RX_STA&0X8000)		//接收到一次数据了
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//添加结束符
		printf("\r\n[u3-R]%s",USART3_RX_BUF);	//发送到串口
		if(mode)USART3_RX_STA=0;
	} 
}

//ATK-ESP8266发送命令后,检测接收到的应答
//str:期待的应答结果
//返回值:0,没有得到期待的应答结果
//    其他,期待应答结果的位置(str的位置)
u8* atk_8266_check_cmd(u8 *str)
{
	
	char *strx=0;
	if(USART3_RX_STA&0X8000)		//接收到一次数据了
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//添加结束符
		strx=strstr((const char*)USART3_RX_BUF,(const char*)str);
	} 
    printf("[%d]\r\n[_get]%s\r\n[want]%s\r\n",strx,USART3_RX_BUF,str);
	return (u8*)strx;
}

//向ATK-ESP8266发送命令
//cmd:发送的命令字符串
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:1ms)(默认单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
u8 atk_8266_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART3_RX_STA=0;
    //waittime = waittime;     // 系统默认时间单位是10ms
    printf("\r\n[cmd:%s]",cmd);	//发送命令
  
	u3_printf("%s\r\n",cmd);	//发送命令
	if(ack&&waittime)		//需要等待应答
	{
		while(waittime)	//等待倒计时
		{
			delay_ms(10);
            waittime-=10;
            printf("<-%d->",waittime);
			if(USART3_RX_STA&0X8000)//接收到期待的应答结果
			{
				if(atk_8266_check_cmd(ack)) break;//得到有效数据 
				USART3_RX_STA=0;
			} 
		}
		if(waittime==0){
          res=1; 
          printf("[%s-TimeOver<%d>]\r\n",(u8*)ack,waittime);
        }
	}

	return res;
} 


//向ATK-ESP8266发送指定数据
//data:发送的数据(不需要添加回车了)
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)luojian
u8 atk_8266_send_data(u8 *data,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART3_RX_STA=0;
	u3_printf("%s",data);	//发送命令
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			delay_ms(10);
			if(USART3_RX_STA&0X8000)//接收到期待的应答结果
			{
				if(atk_8266_check_cmd(ack))break;//得到有效数据 
				USART3_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
}


//ATK-ESP8266退出透传模式
//返回值:0,退出成功;
//       1,退出失败
u8 atk_8266_quit_trans(void)
{
	while((USART3->SR&0X40)==0);	//等待发送空
	USART3->DR='+';      
	delay_ms(15);					//大于串口组帧时间(10ms)
	while((USART3->SR&0X40)==0);	//等待发送空
	USART3->DR='+';      
	delay_ms(15);					//大于串口组帧时间(10ms)
	while((USART3->SR&0X40)==0);	//等待发送空
	USART3->DR='+';      
	delay_ms(500);					//等待500ms
	return atk_8266_send_cmd("AT","OK",200);//退出透传判断.
}


//获取ATK-ESP8266模块的AP+STA连接状态
//返回值:0，未连接;1,连接成功
u8 atk_8266_apsta_check(void)
{
	if(atk_8266_quit_trans())return 0;			//退出透传 
	atk_8266_send_cmd("AT+CIPSTATUS",":",500);	//发送AT+CIPSTATUS指令,查询连接状态
	if(atk_8266_check_cmd("+CIPSTATUS:0")&&
		 atk_8266_check_cmd("+CIPSTATUS:1")&&
		 atk_8266_check_cmd("+CIPSTATUS:2")&&
		 atk_8266_check_cmd("+CIPSTATUS:4"))
		return 0;
	else return 1;
}
//获取ATK-ESP8266模块的连接状态
//返回值:0,未连接;1,连接成功.
u8 atk_8266_consta_check(void)
{
	u8 *p;
	u8 res;
	if(atk_8266_quit_trans())return 0;			//退出透传 
	atk_8266_send_cmd("AT+CIPSTATUS",":",500);	//发送AT+CIPSTATUS指令,查询连接状态
	p=atk_8266_check_cmd("+CIPSTATUS:"); 
	res=*p;									//得到连接状态	
	return res;
}



//获取Client ip地址
//ipbuf:ip地址输出缓存区
void atk_8266_get_ip(u8* ipbuf)
{
	u8 *p,*p1;
    if(atk_8266_send_cmd("AT+CIFSR","OK",500))//获取IP地址失败
    {
        ipbuf[0]=0;
        return;
    }		
    p=atk_8266_check_cmd("\"");
    p1=(u8*)strstr((const char*)(p+1),"\"");
    *p1=0;
    sprintf((char*)ipbuf,"%s",p+1);	
}


//ATK-ESP8266模块信息显示
//x,y:显示信息的起始坐标.
//wanip:0,全部更新显示;1,仅更新wanip.
void atk_8266_msg_show(u8 wanip)
{
	u8 *p,*p1,*p2;
	p=mymalloc(SRAMIN,32);							//申请32字节内存
	p1=mymalloc(SRAMIN,32);							//申请32字节内存
	p2=mymalloc(SRAMIN,32);							//申请32字节内存

	atk_8266_send_cmd("AT+CWMODE=2","OK",200);
	atk_8266_send_cmd("AT+RST","OK",200);
	delay_ms(2000);//延时2s等待模块重启

	if(wanip==0)//全更新
	{
		atk_8266_send_cmd("AT+GMR","OK",200);		//获取固件版本号
        p=atk_8266_check_cmd("SDK version:");
        printf("\r\n固件版本:%s\r\n",p);

        
		atk_8266_send_cmd("AT+CWMODE?","+CWMODE:",200);	//获取网络模式
		p=atk_8266_check_cmd(":");
        printf("网络模式:%s\r\n",(u8*)ATK_ESP8266_CWMODE_TBL[*(p+1)-'1']);


        atk_8266_send_cmd("AT+CWSAP?","+CWSAP:",200);	//获取wifi配置
        
        p=atk_8266_check_cmd("\"");
        p1=(u8*)strstr((const char*)(p+1),"\"");
        p2=p1;
        *p1=0;
        printf("SSID号:%s\r\n",p+1);
            
        p=(u8*)strstr((const char*)(p2+1),"\"");
        p1=(u8*)strstr((const char*)(p+1),"\"");
        p2=p1;
        *p1=0;		
        printf("密码:%s\r\n",p+1);

        p=(u8*)strstr((const char*)(p2+1),",");
        p1=(u8*)strstr((const char*)(p+1),",");
        *p1=0;
        printf("通道号:%s\r\n",p+1);
        printf("加密方式:%s\r\n",(u8*)ATK_ESP8266_ECN_TBL[*(p1+1)-'0']);

	}
	myfree(SRAMIN,p);		//释放内存 
	myfree(SRAMIN,p1);		//释放内存 
	myfree(SRAMIN,p2);		//释放内存 
}



//测试界面主UI
void atk_8266_mtest_ui()
{ 
  printf("\r\nATK_ESP8266 WIFI模块测试,请选择:\
        \r\n|--KEY0:WIFI STA+AP\
        \r\n|--KEY1:WIFI STA\
        \r\n|--WKUP:WIFI AP\r\n");
 	atk_8266_msg_show(0);
}


//ATK-ESP8266模块测试主函数
void atk_8266_test(void)
{
    //	u16 rlen=0;
    u8 key;
    u8 timex;

    printf("\r\n\r\n=====ATK-ESP8266 WIFI模块测试=====\r\n\r\n");

    printf("检查ATK-ESP8266是否在线\r\n");
    while(atk_8266_send_cmd("AT","OK",200))//检查WIFI模块是否在线
    {
        atk_8266_quit_trans();//退出透传
        atk_8266_send_cmd("AT+CIPMODE=0","OK",200);  //关闭透传模式	    
        delay_ms(800);
        printf("未检测到模块!!!\r\n");
        printf("退出透传模式，延时800ms，尝试连接模块...\r\n");
    } 
    printf(">关闭回显\r\n");
    while(atk_8266_send_cmd("ATE0","OK",200));//关闭回显

    printf(">按key_Down开始进入循环测试===\r\n");

    while(1)
    {
        delay_ms(10); 
        atk_8266_at_response(1);//检查ATK-ESP8266模块发送过来的数据,及时上传给电脑
        key=Key_Scan(0); 
        if(key==KEY_D_PRESS)
        {
            printf("[KEY_D_PRESS]WIFI-STA TEST\r\n");
            printf(">>正在配置ATK-ESP8266模块，请稍等...\r\n");
            atk_8266_wifista_test();//WIFI STA测试
            timex=0;
        } 	 
        if((timex%20)==0)LED_Blin_Green();//200ms闪烁 
        timex++;	 
    } 
}

