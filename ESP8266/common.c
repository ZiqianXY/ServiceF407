#include "common.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//ATK-ESP8266 WIFIģ�� ������������	   						  
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//�û�������


//���Ӷ˿ں�:8086,�������޸�Ϊ�����˿�.
const u8* ipnum="192.168.1.115";	
const u8* portnum="8888";		 

//WIFI STAģʽ,����Ҫȥ���ӵ�·�������߲���,��������Լ���·��������,�����޸�.
//const u8* wifista_ssid="nao";			//·����SSID��
//const u8* wifista_encryption="wpawpa2_aes";	//wpa/wpa2 aes���ܷ�ʽ
//const u8* wifista_password="naonaonao"; 	//��������

const u8* wifista_ssid="VM_1001_2.4";		//·����SSID��
const u8* wifista_encryption="wpawpa2_aes";	//wpa/wpa2 aes���ܷ�ʽ
const u8* wifista_password="1001VM1005"; 	//��������

/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
const u8 netpro=0x01;	//����ģʽΪ wifi-sta
//4������ģʽ
const u8 *ATK_ESP8266_CWMODE_TBL[3]={"STAģʽ ","APģʽ ","AP&STAģʽ "};	//ATK-ESP8266,3������ģʽ,Ĭ��Ϊ·����(ROUTER)ģʽ 
//4�ֹ���ģʽ
const u8 *ATK_ESP8266_WORKMODE_TBL[3]={"TCP������","TCP�ͻ���"," UDP ģʽ"};	//ATK-ESP8266,4�ֹ���ģʽ
//5�ּ��ܷ�ʽ
const u8 *ATK_ESP8266_ECN_TBL[5]={"OPEN","WEP","WPA_PSK","WPA2_PSK","WPA_WAP2_PSK"};
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 

//usmart֧�ֲ���
//���յ���ATָ��Ӧ�����ݷ��ظ����Դ���
//mode:0,������USART3_RX_STA;
//     1,����USART3_RX_STA;
void atk_8266_at_response(u8 mode)
{
	if(USART3_RX_STA&0X8000)		//���յ�һ��������
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//��ӽ�����
		if(showLogInfo) printf("\r\n[u3-R]%s",USART3_RX_BUF);	//���͵�����
		if(mode)USART3_RX_STA=0;
	} 
}

//ATK-ESP8266���������,�����յ���Ӧ��
//str:�ڴ���Ӧ����
//����ֵ:0,û�еõ��ڴ���Ӧ����
//    ����,�ڴ�Ӧ������λ��(str��λ��)
u8* atk_8266_check_cmd(u8 *str){
	char *strx=0;
	if(USART3_RX_STA&0X8000)		//���յ�һ��������
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//��ӽ�����
		strx=strstr((const char*)USART3_RX_BUF,(const char*)str);
	} 
    if(showLogInfo) if(strx==0) printf("[%d]\r\n[_get]%s\r\n[want]%s\r\n",strx,USART3_RX_BUF,str);
	return (u8*)strx;
}

//��ATK-ESP8266��������
//cmd:���͵������ַ���
//ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
//waittime:�ȴ�ʱ��(��λ:1ms)(Ĭ�ϵ�λ:10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)
//       1,����ʧ��
u8 atk_8266_send_cmd(u8 *cmd,u8 *ack,u16 waittime){
	u8 res=0; 
	USART3_RX_STA=0;
    //waittime = waittime;     // ϵͳĬ��ʱ�䵥λ��10ms
    //printf("[cmd:%s]\r\n",cmd);	//��������
  
	u3_printf("%s\r\n",cmd);	//��������
	if(ack&&waittime)		//��Ҫ�ȴ�Ӧ��
	{
		while(waittime)	//�ȴ�����ʱ
		{
			delay_ms(10);
            waittime-=10;
            //printf("<-%d->",waittime);
			if(USART3_RX_STA&0X8000)//���յ��ڴ���Ӧ����
			{
				if(atk_8266_check_cmd(ack)) break;//�õ���Ч���� 
			} 
		}
		if(waittime==0){
          res=1; 
          USART3_RX_STA=0;
          if(showLogInfo) printf("[cmd:%s]\r\n",cmd);	//��������
          if(showLogInfo) printf("[%s-TimeOver<%d>]\r\n",(u8*)ack,waittime);
        }
	}
	return res;
} 


//��ATK-ESP8266����ָ������
//data:���͵�����(����Ҫ��ӻس���)
//ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
//waittime:�ȴ�ʱ��(��λ:10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)luojian
u8 atk_8266_send_data(u8 *data,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART3_RX_STA=0;
	u3_printf("%s",data);	//��������
	if(ack&&waittime)		//��Ҫ�ȴ�Ӧ��
	{
		while(waittime)	//�ȴ�����ʱ
		{
			delay_ms(10);
            waittime-=10;
			if(USART3_RX_STA&0X8000)//���յ��ڴ���Ӧ����
			{
				if(atk_8266_check_cmd(ack))break;//�õ���Ч���� 
				USART3_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
}

u8 atk_8266_start_trans(){
    atk_8266_quit_trans();
    printf("\r\n===����Ϊ͸��ģʽ\r\n");
	atk_8266_send_cmd("AT+CIPMODE=1","OK",200);      //����ģʽΪ��͸��			
    return (0==atk_8266_send_cmd("AT+CIPSEND","OK",200));         //��ʼ͸�� 
}

//ATK-ESP8266�˳�͸��ģʽ
//����ֵ:0,�˳��ɹ�;
//       1,�˳�ʧ��
u8 atk_8266_quit_trans(void){
    printf("===�˳�͸��ģʽ\r\n");
	while((USART3->SR&0X40)==0);	//�ȴ����Ϳ�
	USART3->DR='+';    
    delay_ms(15);					//���ڴ�����֡ʱ��(10ms)
	while((USART3->SR&0X40)==0);	//�ȴ����Ϳ�
	USART3->DR='+';      
	delay_ms(15);					//���ڴ�����֡ʱ��(10ms)
	while((USART3->SR&0X40)==0);	//�ȴ����Ϳ�
	USART3->DR='+';      
	delay_ms(500);					//�ȴ�500ms
    //atk_8266_send_cmd("AT+CIPMODE=0","OK",200);   //�ر�͸��ģʽ
	return atk_8266_send_cmd("AT","OK",200);//�˳�͸���ж�.
}

// ������wifi
// ����1�����ӳɹ���0������ʧ��
u8 atk_8266_connect_to_wifi(void){
    u8 *p;
    u8 retry = 3;
    p=mymalloc(SRAMIN,32);	
    printf("===connect to wifi\r\n");
    printf("���߲���:%s,%s,%s\r\n",(u8*)wifista_ssid,(u8*)wifista_encryption,(u8*)wifista_password);    //����32�ֽ��ڴ�
    if(atk_8266_connect_wifi_check()) {
        printf("[������]");
        myfree(SRAMIN,p);		//�ͷ��ڴ� 
        return 1;
    }
    //�������ӵ���WIFI��������/���ܷ�ʽ/����,�⼸��������Ҫ�������Լ���·�������ý����޸�!! 
	
    sprintf((char*)p,"AT+CWJAP=\"%s\",\"%s\"",wifista_ssid,wifista_password);//�������߲���:ssid,����
    while(retry && atk_8266_send_cmd(p,"WIFI GOT IP",5000)){
        retry--;
        printf(">>>reConnect(%d)...\r\n",3-retry);
    }
    myfree(SRAMIN,p);		//�ͷ��ڴ� 
    return retry;
}

// ������������
// ����1�����ӳɹ���0������ʧ��
u8 atk_8266_connect_to_server(void){
    u8 *p;
    u8 retry = 3;
    p=mymalloc(SRAMIN,32);							//����32�ֽ��ڴ�
    if(atk_8266_connect_server_check()) {
        printf("[�����ӵ�������]");
        myfree(SRAMIN,p);		//�ͷ��ڴ� 
        return 1;
    }
    printf("===connect to sever\r\n");
    printf(">����ATK-ESPģ��Ϊ������...\r\n");
    atk_8266_send_cmd("AT+CIPMUX=0","OK",500);   //0�������ӣ�1��������
    
    sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",%s",(u8*)ipnum,(u8*)portnum);    //����Ŀ��TCP������
    printf(">���ʷ�������%s\r\n",p+12);
    
    //?????????????????????????????????????????????????????????//
    while(retry && atk_8266_send_cmd(p,"ALREADY CONNECTED",2000) && atk_8266_send_cmd(p,"OK",2000)){ 
        retry--;
        printf(">>>����ָ����ַʧ�ܣ�����(%d)...\r\n",3-retry);
        //���յ�ERROR CLOSED��ʾ�������˲����ߣ�ֱ���˳�����
        if(atk_8266_send_cmd(p,"CLOSED",2000)==0) return 0; 
    }
    if(retry) printf(">>>����ָ����ַ�ɹ�\r\n");
    myfree(SRAMIN,p);		//�ͷ��ڴ� 
    return retry;
}

//��ȡATK-ESP8266ģ���wifi����״̬
//����ֵ:0��δ����;1,���ӳɹ�
u8 atk_8266_connect_wifi_check(void)
{
	if(atk_8266_quit_trans())return 0;			//�˳�͸�� 
	atk_8266_send_cmd("AT+CIPSTATUS",":",500);	//����AT+CIPSTATUSָ��,��ѯ����״̬
	if(atk_8266_check_cmd("+CIPSTATUS:0")&&
		 atk_8266_check_cmd("+CIPSTATUS:1")&&
		 atk_8266_check_cmd("+CIPSTATUS:2")&&
		 atk_8266_check_cmd("+CIPSTATUS:4"))
		return 0;
	else return 1;
}

//��ȡATK-ESP8266ģ�������״̬
//����ֵ:0,δ����;1,���ӳɹ�.
u8 atk_8266_connect_server_check(void)
{
	if(atk_8266_quit_trans())return 0;			//�˳�͸�� 
	atk_8266_send_cmd("AT+CIPSTATUS",":",500);	//����AT+CIPSTATUSָ��,��ѯ����״̬
	return (u8)atk_8266_check_cmd("STATUS:3"); 
}


//��ȡClient ip��ַ
//ipbuf:ip��ַ���������
void atk_8266_get_ip(u8* ipbuf)
{
	u8 *p,*p1;
    if(atk_8266_send_cmd("AT+CIFSR","OK",500))//��ȡIP��ַʧ��
    {
        ipbuf[0]=0;
        return;
    }		
    p=atk_8266_check_cmd("\"");
    p1=(u8*)strstr((const char*)(p+1),"\"");
    *p1=0;
    sprintf((char*)ipbuf,"%s",p+1);	
    printf("===get ip\r\n");
    printf("local-ip:%s\r\n",ipbuf);
}


//ATK-ESP8266ģ����Ϣ��ʾ
//wanip:0,ȫ��������ʾ;1,������wanip.
void atk_8266_version_msg_show()
{
	u8 *p,*p1,*p2;
	p=mymalloc(SRAMIN,32);							//����32�ֽ��ڴ�
	p1=mymalloc(SRAMIN,32);							//����32�ֽ��ڴ�
	p2=mymalloc(SRAMIN,32);							//����32�ֽ��ڴ�

	atk_8266_send_cmd("AT+CWMODE=2","OK",200);
	atk_8266_send_cmd("AT+RST","OK",200);
	delay_ms(2000);//��ʱ2s�ȴ�ģ������


    atk_8266_send_cmd("AT+GMR","OK",200);		//��ȡ�̼��汾��
    p=atk_8266_check_cmd("SDK version:");
    printf("\r\n�̼��汾:%s\r\n",p);

    
    atk_8266_send_cmd("AT+CWMODE?","+CWMODE:",200);	//��ȡ����ģʽ
    p=atk_8266_check_cmd(":");
    printf("����ģʽ:%s\r\n",(u8*)ATK_ESP8266_CWMODE_TBL[*(p+1)-'1']);


    atk_8266_send_cmd("AT+CWSAP?","+CWSAP:",200);	//��ȡwifi����
    
    p=atk_8266_check_cmd("\"");
    p1=(u8*)strstr((const char*)(p+1),"\"");
    p2=p1;
    *p1=0;
    printf("SSID��:%s\r\n",p+1);
        
    p=(u8*)strstr((const char*)(p2+1),"\"");
    p1=(u8*)strstr((const char*)(p+1),"\"");
    p2=p1;
    *p1=0;		
    printf("����:%s\r\n",p+1);

    p=(u8*)strstr((const char*)(p2+1),",");
    p1=(u8*)strstr((const char*)(p+1),",");
    *p1=0;
    printf("ͨ����:%s\r\n",p+1);
    printf("���ܷ�ʽ:%s\r\n",(u8*)ATK_ESP8266_ECN_TBL[*(p1+1)-'0']);

	
	myfree(SRAMIN,p);		//�ͷ��ڴ� 
	myfree(SRAMIN,p1);		//�ͷ��ڴ� 
	myfree(SRAMIN,p2);		//�ͷ��ڴ� 
}



void atk_8266_init(void){
    printf("\r\n\r\n=====ATK-ESP8266 WIFIģ���ʼ��=====\r\n");

    printf("===���ATK-ESP8266�Ƿ�����\r\n");
    while(atk_8266_send_cmd("AT","OK",200))//���WIFIģ���Ƿ�����
    {
        atk_8266_quit_trans();//�˳�͸��
        atk_8266_send_cmd("AT+CIPMODE=0","OK",200);  //�ر�͸��ģʽ	    
        delay_ms(1000);
        printf(">>>δ��⵽ģ��!!!\r\n");
        printf(">>>�˳�͸��ģʽ����ʱ1s����������ģ��...\r\n");
    } 
    printf("===�رջ���\r\n");
    while(atk_8266_send_cmd("ATE0","OK",200));//�رջ���
    
    printf("===����WIFIΪSTAģʽ,����...\r\n");
	
    atk_8266_send_cmd("AT+CWMODE=1","OK",500);		//����WIFI STAģʽ
    atk_8266_send_cmd("AT+RST","OK",500);
    delay_ms(3000);         //��ʱ3S�ȴ������ɹ�
}

