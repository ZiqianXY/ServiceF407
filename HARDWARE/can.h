#ifndef __CAN_H
#define __CAN_H	 

#include <stdbool.h>
#include "sys.h"

#define CAN_IT_ENABLE	        DISABLE		// CAN����RX0�ж�ʹ��			
#define LEN_DataCAN             8	        // CANĬ�����ݳ���
#define CAN_Bandrate_500        (0x00)
#define CAN_Bandrate_250        (0x01)


bool CAN_GetProtocal(void);// ��ȡCAN�Ĳ����ʺͱ�ʶ��λ��������������䷶Χ�򷵻�false

void CAN_SetProtocal(u8 bandrate, u8 idType);

//u8 CAN_GetBandrate(void);

//u8 CAN_GetIDE(void);

bool CAN_ModeInit(u16 boundrate, u8 mode);//CAN��ʼ�����Զ���
 
bool CAN_SendMsg(u32 can_id , u8* msg);	//�������ݣ��Զ���
    
CanRxMsg CAN_RecMsg(void);//�������ݣ��Զ���

bool CAN_HasData(void); // ���CAN�Ƿ��н��յ�����

void CAN_IT_SET(FunctionalState enable); // ����CAN�ж�

u8 CAN_GetBandrate(void);

#endif
