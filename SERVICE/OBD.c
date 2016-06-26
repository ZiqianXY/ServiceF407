/**********************************************
**
**  OBD协议相关的各项任务实现 ，by Ziqian      *
**
***********************************************/

#include "OBD.h"
#include "usart.h"
#include "delay.h"
#include "led.h"
#include "can.h"
#include "database.h"
#include "rtc.h"
// 服务数据项和标识量
static OBD_CommandTypeDef OBD_CommandList[NUM_COMMAND]={{0,0}};    // 待读取数据指令列表
static OBD_CommandTypeDef OBD_CommandStruct;   // 一条数据指令
static OBD_USDataTypeDef OBD_DataStruct;       // 数据缓存结构体
bool TimerOver_Rx;  // 发送超时标志
bool TimerOver_Tx;  // 接收超时标志
u8 ReSendTimes;     // 重新发送次数
u8 tmpTimer=0;      // timer临时变量
u32 OBD_CAN_ID;

// 发送OBD数据请求服务，SID-PID
bool OBD_SendCommand( OBD_CommandTypeDef obd_command){
    u8 command_data[Length_Default]={0};
    bool sendFlag=false;
    command_data[0]=0x02;
    command_data[1]=obd_command.SID;
    command_data[2]=obd_command.PID;
    
    sendFlag=CAN_SendMsg(OBD_CAN_ID,command_data);
    
    if (sendFlag){
			//printf("-请求置位-");
			OBD_DataStruct.indication_Request=true;
			OBD_DataStruct.command=obd_command;
			OBD_DataStruct.id_CAN=OBD_CAN_ID;
			OBD_StartTimerRx();
    }
    return sendFlag;
}


// 发送OBD控制帧
bool OBD_SendCommand_CF(void){
    
    u8 command_data[Length_Default]={0};
    command_data[0]=PCItype_FCtrl;
    command_data[1]=FC_BS;
    command_data[2]=FC_STmin;
    
    printf("-FC-");
    tmpTimer=0;
    TimerOver_Rx=false;
    return CAN_SendMsg(OBD_DataStruct.id_CAN,command_data);
}


// 添加数据到缓存区
void AppendData(u8* msg, u8 start){
    u8 i=0;
    u8 position=0;
    for(i=start;i<8;i++){
        position=OBD_DataStruct.lengthCurrent;
        OBD_DataStruct.Data[position]=msg[i];
        //printf("[%d][%d]",i,msg[i]);
        //printf("[%d][%d]",position,OBD_DataStruct.Data[position]);
        // 数据接受完成
        OBD_DataStruct.lengthCurrent++;
        if(OBD_DataStruct.lengthCurrent==OBD_DataStruct.lengthToRec){
            //printf("-RxEnd-");
            OBD_DataStruct.indication_RX=true;
            break;
        }
    }
}


// CAN中断接收数据跳转函数
void OBD_CAN_RecHandler(CanRxMsg msgCanRx){
    
    u8 bytePCI=msgCanRx.Data[0]&0xF0;
    
    //  错误处理--尚未开启本次服务请求
    if(!OBD_DataStruct.indication_Request){
        OBD_ErrorHandler(N_UNEXPECTED);
        return;
    }
    //  错误处理--CAN数据帧长度不是8
    if((msgCanRx.DLC)!=Length_Default){
        OBD_ErrorHandler(N_WRING_DLC);
        return;
    }
    
    // 根据PCItype处理数据
    switch (bytePCI){
                            
        case PCItype_SF:
            
            printf("-SF-");
            OBD_DataStruct.lengthToRec=msgCanRx.Data[0]&0x0F;   // 保存待接受数据长度
            AppendData(msgCanRx.Data,1);        //  缓存接收数据
            OBD_DataStruct.indication_RX =true;
            //OBD_DeInit();
            break;
        
        case PCItype_FF:
            if(OBD_DataStruct.indication_FF){
                OBD_ErrorHandler(N_WRONG_FF);
                return;
            }
            OBD_DataStruct.lengthToRec=((msgCanRx.Data[0]&0x0F)<<8)+msgCanRx.Data[1];   // 保存待接受数据长度
            AppendData(msgCanRx.Data,2);        //  缓存接收数据
            
            OBD_DataStruct.id_CAN=msgCanRx.StdId;
            printf("-FF-");
            OBD_DataStruct.indication_FF=true;  //  首帧标志
            OBD_SendCommand_CF();               //  发送流量控制帧
            break;

        case PCItype_CF:
            if(!OBD_DataStruct.indication_FF){
                OBD_ErrorHandler(N_WRONG_CF);
                return;
            }
            printf("-CF-%1x",msgCanRx.Data[0]&0x0F);
            AppendData(msgCanRx.Data,1);        //  缓存接收数据
            break;
        
        default :
            OBD_ErrorHandler(N_WRONG_PCItype);
            return;
    }

}

// 重置OBD服务的数据项
void OBD_DeInit(void){
    
    //OBD_DataStruct.command=OBD_CommandStruct;
    OBD_DataStruct.id_CAN=Rsp_Fhy_Default;
    OBD_DataStruct.indication_FF=false;
    OBD_DataStruct.indication_Request=false;
    OBD_DataStruct.indication_RX=false;
    OBD_DataStruct.result=N_NA;
    OBD_DataStruct.lengthCurrent=0;
    OBD_DataStruct.lengthToRec=0;
    
    TimerOver_Rx=false;
    TimerOver_Tx=false;
}

// OBD正确接收到对应请求的后续处理
void saveData(OBD_USDataTypeDef OBD_DataStruct){
    u16 i=0;
		char msg[100];
	  u8 offset=0;
		char time[17];
		getTimeString((char*)time);
//char dataBuffer[TEST_LEN] = "Time,SID-PID,Min,Max,Per,Unit,Description\r\n";
		offset+= sprintf(msg+offset,"%s%08x,%d,",time,OBD_DataStruct.id_CAN,OBD_DataStruct.lengthToRec);
		for(i=0;i<OBD_DataStruct.lengthToRec;i++){
				offset+= sprintf(msg+offset,"%02x ",OBD_DataStruct.Data[i]);
		}
		offset+= sprintf(msg+offset,",#\r\n");	// 为数据添加结束换行标志
    DB_SaveData(msg,offset);
    printf("\r\n%s",msg);
}

void printCommandList(void){
    int i=0;
    printf("\r\n==OBD_CommandList==\r\n");
    for(i=0;i<NUM_COMMAND;i++){
        printf("[%02d][0x]%02x %02x\r\n",i,OBD_CommandList[i].SID,OBD_CommandList[i].PID);
    }
}

void OBD_CHECK(void){
    printf("[data resolve]:");
    if(OBD_DataStruct.indication_RX){
        printf("[got Data]");
        
    }else{
        printf("[something wrong]");
    }
    printf("\r\n");

}

void OBD_WIFI(void){
    printf("\r\n\
	every cycle, check, if there are some trouble TMP_FILE, read and transmit through WIFI. \r\n");
}

void getCanId(void){
    // 根据CAN_ID类型发送不同的id
    if(CAN_GetBandrate()==CAN_Bandrate_250)
        OBD_CAN_ID = Req_29_Fun;
    else
        OBD_CAN_ID = Req_11_Fun;
}

/**
 * 开始OBD数据请求服务，轮询OBD_CommandList中的各项数据请求
*/
void OBD_StartService(void){
    u16 i=0;
    // 获取can命令行ID
    getCanId();
    // 读取待获取数据
    OBD_ReadCommandList();
    printf("\r\n\r\n==OBD_RequestCommandList==\r\n");
    
    for(i=0;i<NUM_COMMAND;i++){
        printf("[%02d]",i);
        // 初始化公共缓存区
        OBD_DeInit(); 
        // 获取一条数据指令
        OBD_CommandStruct=OBD_CommandList[i];
        // blin the led to indicate the running
        LED_Blin_Syn();

        // 发送查询命令
        OBD_SendCommand(OBD_CommandStruct);
        // 等待接收完成
        OBD_ReceiveData();
        // 对结果进行处理
        OBD_CHECK();
        // 延时等待下一条数据查询
        delay_ms(TIME_NextRequest);
    }
    
    OBD_WIFI();
    
}

/**
 * test
*/
void OBD_TEST(void){
    static u8 i=0;
    // 获取can命令行ID
    getCanId();
    // 读取待获取数据
    OBD_ReadCommandList();
    
		printf("[%02d]",i);
		// 初始化公共缓存区
		OBD_DeInit(); 
		// 获取一条数据指令
		OBD_CommandStruct=OBD_CommandList[i];
		// blin the led to indicate the running
		LED_Blin_Syn();

		// 发送查询命令
		OBD_SendCommand(OBD_CommandStruct);
		// 等待接收完成
		OBD_ReceiveData();
	
		// 对结果进行处理
		OBD_CHECK();   
    OBD_WIFI();
		
    i++;
    if(i==NUM_COMMAND)
        i=0;
}

void OBD_ReceiveData(){
    tmpTimer=0;
    TimerOver_Rx=false;        
    
    while(1){
        // 临时处理的方式
        if((TIME_InRequest*tmpTimer++)>TIME_Rx_OVER){
            TimerOver_Rx=true;
        }
        
        if(TimerOver_Rx){
            OBD_DataStruct.result=N_TIMEOUT_RX;
            OBD_ErrorHandler(N_TIMEOUT_RX);
            break;
        }
        // 定义多久查一次
        delay_ms(TIME_InRequest);
        // 强制查询收件箱
        if(CAN_HasData()){
            //-------------------------------------------------------OBD_CAN_RecHandler---------------------/    
            OBD_CAN_RecHandler(CAN_RecMsg());       
            //-------------------------------------------------------OBD_CAN_RecHandler---------------------/  
            if(OBD_DataStruct.indication_RX){
                printf("[%02d次]",tmpTimer);
                saveData(OBD_DataStruct);
                break;
            }
            tmpTimer=0;
            TimerOver_Rx=false;
        }
    }    
}


void OBD_ErrorHandler(u8 error){
    printf("-ObdErr:%d-\r\n",error);

};


void OBD_StartTimerRx(void){
    //printf("==OBD_startTimer==\r\n");
}

void OBD_ReadCommandList(void){
    u8 i=0;
    const OBD_CommandTypeDef TMP_CommandList[NUM_COMMAND]={
        {0x01,0x00},{0x02,0x00},{0x03,0x00},{0x04,0x00},{0x05,0x00},
        {0x06,0x00},{0x07,0x00},{0x08,0x00},{0x09,0x00},{0x10,0x00},
    };
    static bool isCommandListRead=false;   // 不重置此数据
    if(isCommandListRead)   return;
    isCommandListRead=true;
    
    printf("\r\n==OBD_ReadCommandList==\r\n");
    for(i=0;i<NUM_COMMAND;i++){
        OBD_CommandList[i]=TMP_CommandList[i];
        printf("[%02d][0x]%02x %02x\r\n",i,OBD_CommandList[i].SID,OBD_CommandList[i].PID);
    }
		printf("==========\r\n");
}
