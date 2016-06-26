#ifndef __KEY_H
#define __KEY_H
#include "sys.h"


/*通过直接操作库函数方式读取IO*/
#define KEY_R 		PEin(4) //PE4
#define KEY_D 		PEin(3)	//PE3 
#define KEY_L 		PEin(2) //PE2
#define KEY_U 	  PAin(0)	//PA0

#define KEY_R_PRESS   1
#define KEY_D_PRESS   2
#define KEY_L_PRESS   3
#define KEY_U_PRESS   4

#define KEY2_PRES   KEY_R_PRESS
#define KEY1_PRES   KEY_D_PRESS
#define KEY0_PRES   KEY_L_PRESS
#define WKUP_PRES   KEY_U_PRESS


void KEY_Init(void);    //IO init 
u8 Key_Scan(u8);        //KeyScan


#endif




//#ifndef __KEY_H
//#define __KEY_H	 
//#include "sys.h" 
////////////////////////////////////////////////////////////////////////////////////	 
////本程序只供学习使用，未经作者许可，不得用于其它任何用途
////ALIENTEK STM32F407开发板
////按键输入驱动代码	   
////正点原子@ALIENTEK
////技术论坛:www.openedv.com
////创建日期:2014/5/3
////版本：V1.0
////版权所有，盗版必究。
////Copyright(C) 广州市星翼电子科技有限公司 2014-2024
////All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////// 	 

///*下面的方式是通过直接操作库函数方式读取IO*/
//#define KEY_R 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4) //PE4
//#define KEY_D 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)	//PE3 
//#define KEY_L 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2) //PE2
//#define KEY_U 	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)	//PA0


///*下面方式是通过位带操作方式读取IO*/
///*
//#define KEY_R 		PEin(4)   	//PE4
//#define KEY_D 		PEin(3)		//PE3 
//#define KEY_L 		PEin(2)		//P32
//#define KEY_U 	PAin(0)		//PA0
//*/


//#define KEY_R_PRESS 	1
//#define KEY_D_PRESS	2
//#define KEY_L_PRESS	3
//#define KEY_U_PRESS   4

//void KEY_Init(void);	//IO初始化
//u8 Key_Scan(u8);  		//按键扫描函数	

//#endif
