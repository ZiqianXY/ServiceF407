#ifndef __CAN_H
#define __CAN_H	 

#include <stdbool.h>
#include "sys.h"

#define CAN_IT_ENABLE	        DISABLE		// CAN接收RX0中断使能			
#define LEN_DataCAN             8	        // CAN默认数据长度
#define CAN_Bandrate_500        (0x00)
#define CAN_Bandrate_250        (0x01)


bool CAN_GetProtocal(void);// 获取CAN的波特率和标识符位数，如果不在适配范围则返回false

void CAN_SetProtocal(u8 bandrate, u8 idType);

//u8 CAN_GetBandrate(void);

//u8 CAN_GetIDE(void);

bool CAN_ModeInit(u16 boundrate, u8 mode);//CAN初始化，自定义
 
bool CAN_SendMsg(u32 can_id , u8* msg);	//发送数据，自定义
    
CanRxMsg CAN_RecMsg(void);//接收数据，自定义

bool CAN_HasData(void); // 检查CAN是否有接收到数据

void CAN_IT_SET(FunctionalState enable); // 设置CAN中断

u8 CAN_GetBandrate(void);

#endif
