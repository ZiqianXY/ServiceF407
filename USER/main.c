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
	
u8 mode = CAN_Mode_LoopBack;//CAN工作模式;CAN_Mode_Normal(0)：普通模式，CAN_Mode_LoopBack(1)：环回模式
u8 bandrate = CAN_Bandrate_250;

u8 chars[]="作1CN";
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
        atk_8266_wifista_test();		//进入ATK_ESP8266测试
	}
}


void sytemInit(void){
	
	// 【特别说明】要先初始化串口，再去用printf;
	usart_init(USART_BPS);	printf("...串口初始化:%dbps\r\n",USART_BPS); 
  usart3_init(115200);  //初始化串口3波特率为115200
	delay_init(SYS_CLK);    printf("...延时函数初始化\r\n"); 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
                            printf("...中断优先级设置，优先级为2\r\n");//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	LED_Init();	            printf("...LED初始化\r\n"); 
	KEY_Init();	            printf("...KEY初始化\r\n"); 
	TIMER3_Init();          printf("...TIM3初始化\r\n"); 
	CAN_ModeInit(bandrate, mode);
	DB_Init();              printf("...文件系统初始化\r\n"); 
	My_RTC_Init();		 			printf("...时钟初始化RTC\r\n"); 
	RTC_Set_WakeUp(RTC_WakeUpClock_CK_SPRE_16bits,0);		//配置WAKE UP中断,1秒钟中断一次
	
	printf("\r\n---help------------------\
					\r\n左键：循环发送OBD查询指令\
          \r\n右键：单次发送OBD查询指令\
          \r\n上键：切换CAN工作模式\
          \r\n下键：切换CAN波特率\
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
        lengthUsart=usart_getDataLength();//得到此次接收到的数据长度
        LED_ON_Green();   //点亮绿灯提示
        printf("\r\n[$:]");
        for(indexUsartTmp=0;indexUsartTmp<lengthUsart;indexUsartTmp++)
            printf("%02x ",USART_RX_BUF[indexUsartTmp]);
        printf("\t"); 
        //从can发送出去
        CAN_SendMsg( Req_29_Fun,USART_RX_BUF);
        usart_reset();     //重置状态标记 
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
             
    LED_OUT_Green();  //熄灭绿灯提示    
    t_led++; 
    delay_ms(10);     
    if(t_led==50){
        LED_Blin_Red();//提示系统正在运行	
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

