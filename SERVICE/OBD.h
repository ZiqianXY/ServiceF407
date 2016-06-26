#ifndef __OBD_H
#define __OBD_H

#include <stdbool.h>
#include "sys.h"
#include "stdcommand.h"


//#define Req_Fun_Default Req_29_Fun
#define Req_Fun_Default Req_11_Fun
#define Rsp_Fhy_Default Rsp_29_phy

#define Length_Default 8
#define startByte_SF 1
#define startByte_FF 1
#define startByte_CF 2

/**************************************************
*
* define Time Parameter                           *
*
***************************************************/

#define TIME_Rx_OVER        2000
#define TIME_InRequest      50
#define TIME_NextRequest    300

// define PCItype parameter
#define PCItype_SF      0x00
#define PCItype_FF      0x10
#define PCItype_CF      0x20
#define PCItype_FCtrl   0x30
#define FC_BS           0
#define FC_STmin        0


#define NUM_COMMAND     10  // 待查询数据指令数量
#define NUM_MAX_DATA    40  // 待查询数据指令数量

typedef enum {
    N_OK=           0,
    N_TIMEOUT_RX=   1,
    N_TIMEOUT_TX=   2,
    N_WRONG_FF=     3,
    N_WRONG_SF=     4,
    N_WRONG_CF=     5,
    N_WRONG_FC=     6,
    N_WRING_DLC=    7,
    N_UNEXPECTED=   8,
    N_WRONG_PCItype=9,
    N_ERROR=        10,
    N_NA=           100,
}N_Result;


typedef struct {
    bool indication_Request;
    bool indication_FF;
    bool indication_RX;
    OBD_CommandTypeDef command;
    u8 Data[NUM_MAX_DATA];
    u16 lengthCurrent;
    u16 lengthToRec;
    u32 id_CAN;
    N_Result result;
}OBD_USDataTypeDef;


/**********************************************
**
**
***********************************************/

bool OBD_SendCommand( OBD_CommandTypeDef obd_command);
void OBD_ReceiveData(void);
//void OBD_CHECK(void);
void OBD_CAN_RecHandler(CanRxMsg msgCanRx);
void OBD_ErrorHandler(u8 error);
void OBD_StartService(void);
void OBD_DeInit(void);
void OBD_StartTimerRx(void);
void OBD_ReadCommandList(void);
void OBD_TEST(void);
#endif
