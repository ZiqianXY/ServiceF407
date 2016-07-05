#ifndef __OBD_H
#define __OBD_H

#include <stdbool.h>
#include "sys.h"


/**************************************************
* define targetAddress & sourceAddress            *
***************************************************/

#define Req_11_Fun 		0x7DF   

#define Req_11_Phy_0  	0x7E0
#define Req_11_Phy_1 	0x7E1
#define Req_11_Phy_2 	0x7E2
#define Req_11_Phy_3  	0x7E3
#define Req_11_Phy_4  	0x7E4
#define Req_11_Phy_5  	0x7E5
#define Req_11_Phy_6  	0x7E6
#define Req_11_Phy_7  	0x7E7

#define Rsp_11_Phy_0 	0x7E8
#define Rsp_11_Phy_1 	0x7E9
#define Rsp_11_Phy_2 	0x7EA
#define Rsp_11_Phy_3 	0x7EB
#define Rsp_11_Phy_4 	0x7EC
#define Rsp_11_Phy_5 	0x7ED
#define Rsp_11_Phy_6 	0x7EE
#define Rsp_11_Phy_7 	0x7EF

#define Req_29_Fun 		0x18DB33F1
#define Req_29_Phy(x) 	0x18DA<<16+(x)<<8+F1
#define Rsp_29_Phy(x) 	0x18DA<<16+F1<<8+(x)
#define Rsp_29_phy      0x18DA33F1

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


/**********************************************
**
** 定义查询数据以及接收数据的结构体
**
***********************************************/
#define NUM_COMMAND     14  // 待查询数据指令数量
#define NUM_MAX_DATA    40  // 接收CAN数据的最大长度

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
    u32 PID_SID;
    char* Min;
    char* Max;
    char* Per;
    char* Unit;
    char* Desc;
}OBD_CmdTypeDef;

typedef struct {
    OBD_CmdTypeDef* cmd;
    bool indication_Request;
    bool indication_FF;
    bool indication_RX;
    u8 Data[NUM_MAX_DATA];
    u16 lengthCurrent;
    u16 lengthToRec;
    u32 id_CAN;
    N_Result result;
}OBD_DataTypeDef;


extern OBD_CmdTypeDef OBD_CmdList[NUM_COMMAND];    // 待读取数据指令列表
extern OBD_DataTypeDef OBD_DATA_TMP;

/**********************************************
**
**  OBD公开方法
**
***********************************************/

bool OBD_SendCommand( OBD_CmdTypeDef* obd_command);
void OBD_ReceiveData(void);
void OBD_CAN_RecHandler(CanRxMsg msgCanRx);
void OBD_ErrorHandler(u8 error);
void OBD_StartService(void);
void OBD_DeInit(void);
void OBD_StartTimerRx(void);
void OBD_print_CmdList(void);
void OBD_TEST(void);
#endif
