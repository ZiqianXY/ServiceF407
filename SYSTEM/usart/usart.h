#ifndef __USART_H
#define __USART_H

#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 


//��Ч���ݸ�ʽ��  $:*********0D0A

#define USART_REC_LEN  			4096  	//�����������ֽ��� 1000(<2^12=1024*4=4096)
#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����

#define STA_RECEIVE_END         0x8000  //���ս������λ       
#define STA_RECEIVE_END_ING     0x4000  //׼������ǰ���λ     
#define STA_RECEIVE_START       0x2000  //��ʼ���λ           
#define STA_RECEIVE_START_ING   0x1000  //׼����ʼ���λ       

#define REC_END         0x0A  //���ս������λ       0A
#define REC_END_ING     0x0D  //׼������ǰ���λ     0D
#define REC_START       0x3A  //��ʼ���λ           :
#define REC_START_ING   0x24  //׼����ʼ���λ       $
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA;         		//����״̬���	

//����봮���жϽ��գ��벻Ҫע�����º궨��
void usart_init(u32 bound);
void usart_reset(void);
u8 usart_hasData(void);
u16 usart_getDataLength(void);

#endif

