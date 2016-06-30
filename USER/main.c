#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "beep.h"
#include "key.h"
#include "can.h"
#include "OBD.h"
#include "rtc.h"
#include "timer.h"
#include "database.h"

#include "usart3.h"
#include "common.h" 

// constant define here
#define SYS_CLK         168
#define USART_BPS       115200

// control info define below
#define EnableCheckCAN  0

// declaration of some functions in this file.
void CAN_KEY_U(void);
void CAN_KEY_R(void);
void CAN_KEY_D(void);
void CAN_KEY_L(void);
void sytemInit(void);
void checkCAN(void);
void checkUsart(void);
void checkKey(void);
void blinLed(void);
	
u8 mode = CAN_Mode_LoopBack;//CAN����ģʽ;CAN_Mode_Normal(0)����ͨģʽ��CAN_Mode_LoopBack(1)������ģʽ
u8 bandrate = CAN_Bandrate_250;

u8 chars[]="��1CN";
u8 key;
u8 i=0,t_led=0;
u8 cnt=0;
u8 canbuf[8];


u16 time_can_receive_IT=0;
u16 time_can_receive_Loop=0;
u16 time_can_transmit=0;


int main(void){ 
	sytemInit();
	//printf("\r\n===read file test===");
	DB_TEST();
	//printf("\r\n===read done===");
    
	while(1){
		//if(EnableCheckCAN)  checkCAN();
		//checkUsart();
		//checkKey();
        atk_8266_wifista_test();		//����ATK_ESP8266����
	}
}


void sytemInit(void){
	
	// ���ر�˵����Ҫ�ȳ�ʼ�����ڣ���ȥ��printf;
	usart_init(USART_BPS);	printf("...���ڳ�ʼ��:%dbps\r\n",USART_BPS); 
  usart3_init(115200);  //��ʼ������3������Ϊ115200
	delay_init(SYS_CLK);    printf("...��ʱ������ʼ��\r\n"); 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
                            printf("...�ж����ȼ����ã����ȼ�Ϊ2\r\n");//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	LED_Init();	            printf("...LED��ʼ��\r\n"); 
	KEY_Init();	            printf("...KEY��ʼ��\r\n"); 
	TIMER3_Init();          printf("...TIM3��ʼ��\r\n"); 
	CAN_ModeInit(bandrate, mode);
	DB_Init();              printf("...�ļ�ϵͳ��ʼ��\r\n"); 
	My_RTC_Init();		 			printf("...ʱ�ӳ�ʼ��RTC\r\n"); 
	RTC_Set_WakeUp(RTC_WakeUpClock_CK_SPRE_16bits,0);		//����WAKE UP�ж�,1�����ж�һ��
	
	printf("\r\n---help------------------\
					\r\n�����ѭ������OBD��ѯָ��\
          \r\n�Ҽ������η���OBD��ѯָ��\
          \r\n�ϼ����л�CAN����ģʽ\
          \r\n�¼����л�CAN������\
          \r\n-------------------------\r\n");   
}

void checkCAN(void){
	printf("\r\n[CAN_Check]");
	if(CAN_HasData()){			
			CAN_RecMsg();
	}else{
			printf("[noData]");
	}
}

void checkUsart(void){
    u16 lengthUsart;
    u16 indexUsartTmp;
    if(usart_hasData()){	
        lengthUsart=usart_getDataLength();//�õ��˴ν��յ������ݳ���
        LED_ON_Green();   //�����̵���ʾ
        printf("\r\n[$:]");
        for(indexUsartTmp=0;indexUsartTmp<lengthUsart;indexUsartTmp++)
            printf("%02x ",USART_RX_BUF[indexUsartTmp]);
        printf("\t"); 
        //��can���ͳ�ȥ
        CAN_SendMsg( Req_29_Fun,USART_RX_BUF);
        usart_reset();     //����״̬��� 
    }
}

void checkKey(void){
    key=Key_Scan(0);
		if(key==KEY_R_PRESS)    CAN_KEY_R();
        if(key==KEY_U_PRESS)    CAN_KEY_U();
        if(key==KEY_D_PRESS)    CAN_KEY_D();
        if(key==KEY_L_PRESS)    CAN_KEY_L();
}

void blinLed(void){
             
    LED_OUT_Green();  //Ϩ���̵���ʾ    
    t_led++; 
    delay_ms(10);     
    if(t_led==50){
        LED_Blin_Red();//��ʾϵͳ��������	
        t_led=0;
    }	      
}

void CAN_KEY_U(void){   printf("\r\n\r\n####[IN]KEY_U...change CAN_Mode:");
    mode=!mode;
    CAN_ModeInit(bandrate, mode);
}

void CAN_KEY_D(void){   printf("\r\n\r\n####[IN]KEY_D...change CAN_Band:");
    bandrate=!bandrate;
    CAN_ModeInit(bandrate, mode);
}

void CAN_KEY_R(void){
    OBD_TEST();
}

void CAN_KEY_L(void){
    OBD_StartService();
}

