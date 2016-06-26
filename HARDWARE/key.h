#ifndef __KEY_H
#define __KEY_H
#include "sys.h"


/*ͨ��ֱ�Ӳ����⺯����ʽ��ȡIO*/
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
////������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
////ALIENTEK STM32F407������
////����������������	   
////����ԭ��@ALIENTEK
////������̳:www.openedv.com
////��������:2014/5/3
////�汾��V1.0
////��Ȩ���У�����ؾ���
////Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
////All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////// 	 

///*����ķ�ʽ��ͨ��ֱ�Ӳ����⺯����ʽ��ȡIO*/
//#define KEY_R 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4) //PE4
//#define KEY_D 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)	//PE3 
//#define KEY_L 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2) //PE2
//#define KEY_U 	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)	//PA0


///*���淽ʽ��ͨ��λ��������ʽ��ȡIO*/
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

//void KEY_Init(void);	//IO��ʼ��
//u8 Key_Scan(u8);  		//����ɨ�躯��	

//#endif
