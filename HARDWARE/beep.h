#ifndef __BEEP_H
#define __BEEP_H	 
#include "sys.h" 


//LED端口定义
#define BEEP PFout(8)	// 蜂鸣器控制IO 

#define ON_BEEP 1
#define OUT_BEEP 0

void BEEP_Init(void);//初始化	

#endif

















