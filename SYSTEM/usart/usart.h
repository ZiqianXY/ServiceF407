#ifndef __USART_H
#define __USART_H

#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 


//有效数据格式：  $:*********0D0A

#define USART_REC_LEN  			4096  	//定义最大接收字节数 1000(<2^12=1024*4=4096)
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收

#define STA_RECEIVE_END         0x8000  //接收结束标记位       
#define STA_RECEIVE_END_ING     0x4000  //准备结束前标记位     
#define STA_RECEIVE_START       0x2000  //开始标记位           
#define STA_RECEIVE_START_ING   0x1000  //准备开始标记位       

#define REC_END         0x0A  //接收结束标记位       0A
#define REC_END_ING     0x0D  //准备结束前标记位     0D
#define REC_START       0x3A  //开始标记位           :
#define REC_START_ING   0x24  //准备开始标记位       $
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;         		//接收状态标记	

//如果想串口中断接收，请不要注释以下宏定义
void usart_init(u32 bound);
void usart_reset(void);
u8 usart_hasData(void);
u16 usart_getDataLength(void);

#endif

