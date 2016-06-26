#include "key.h"
#include "sys.h"
#include "delay.h"

void KEY_Init(void){
    
    GPIO_InitTypeDef GPIO_InitTypeStructure;
    //enable the clock
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOE,ENABLE);
    
    //init GPIOE,pin2,3,4  GPIOA, pin0
    
    GPIO_InitTypeStructure.GPIO_Mode=GPIO_Mode_IN;
    GPIO_InitTypeStructure.GPIO_Speed=GPIO_Speed_100MHz;
    
    GPIO_InitTypeStructure.GPIO_Pin=GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;
    GPIO_InitTypeStructure.GPIO_PuPd=GPIO_PuPd_UP;
    GPIO_Init(GPIOE,&GPIO_InitTypeStructure);
    
    GPIO_InitTypeStructure.GPIO_Pin=GPIO_Pin_0;
    GPIO_InitTypeStructure.GPIO_PuPd=GPIO_PuPd_DOWN;
    GPIO_Init(GPIOA,&GPIO_InitTypeStructure);
    
}


//  scan the input of the keys
//  ScanMode=0,not long press
//  ScanMode=1,allow long press
//
u8 Key_Scan(u8 ScanMode){

    static u8 key_up = 1;
    if(ScanMode)   key_up=1;
    
    if(KEY_R==1&&KEY_D==1&&KEY_L==1&&KEY_U==0){
        key_up=1;
        return 0;
    }
    delay_ms(10);    
    
    if(key_up==1){
        key_up=0;
        if(KEY_R==0) return KEY_R_PRESS;
        if(KEY_D==0) return KEY_D_PRESS;
        if(KEY_L==0) return KEY_L_PRESS;
        if(KEY_U==1) return KEY_U_PRESS; 
    }
    
    return 0;
}


//#include "key.h"
//#include "delay.h" 

////按键初始化函数
//void KEY_Init(void)
//{
//	
//	GPIO_InitTypeDef  GPIO_InitStructure;

//  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOE, ENABLE);//使能GPIOA,GPIOE时钟
// 
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4; //KEY_R KEY_D KEY_L对应引脚
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
//  GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化GPIOE2,3,4
//	
//	 
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//KEY_U对应引脚PA0
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN ;//下拉
//  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA0
// 
//} 
////按键处理函数
////返回按键值
////mode:0,不支持连续按;1,支持连续按;
////0，没有任何按键按下
////1，KEY_R按下
////2，KEY_D按下
////3，KEY_L按下 
////4，WKUP按下 KEY_U
////注意此函数有响应优先级,KEY_R>KEY_D>KEY_L>KEY_U!!
//u8 Key_Scan(u8 mode)
//{	 
//	static u8 key_up=1;//按键按松开标志
//	if(mode)key_up=1;  //支持连按		  
//	if(key_up&&(KEY_R==0||KEY_D==0||KEY_L==0||KEY_U==1))
//	{
//		delay_ms(10);//去抖动 
//		key_up=0;
//		if(KEY_R==0)return 1;
//		else if(KEY_D==0)return 2;
//		else if(KEY_L==0)return 3;
//		else if(KEY_U==1)return 4;
//	}else if(KEY_R==1&&KEY_D==1&&KEY_L==1&&KEY_U==0)key_up=1; 	    
// 	return 0;// 无按键按下
//}




















