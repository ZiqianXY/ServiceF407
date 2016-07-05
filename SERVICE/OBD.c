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
u8 num = 0;
u8 TimerOver_Rx;  // 发送超时标志
u8 TimerOver_Tx;  // 接收超时标志
u8 ReSendTimes;     // 重新发送次数
u8 tmpTimer=0;      // timer临时变量
u32 OBD_CAN_ID;

OBD_CmdTypeDef OBD_CmdList[NUM_COMMAND];    // 待读取数据指令列表
OBD_DataTypeDef OBD_DATA_TMP;



// 发送OBD数据请求服务，SID-PID
bool OBD_SendCommand( OBD_CmdTypeDef* obd_command){
    u8 command_data[Length_Default]={0};
    bool sendFlag=false;
    command_data[0]=0x02;
    command_data[1]=obd_command->PID_SID>>8; 
    command_data[2]=obd_command->PID_SID & 0x00ff;
    sendFlag=CAN_SendMsg(OBD_CAN_ID,command_data);
    OBD_DATA_TMP.cmd = obd_command;
    if (sendFlag){
        //printf("-请求置位-");
        OBD_DATA_TMP.indication_Request=true;
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
    return CAN_SendMsg(OBD_DATA_TMP.id_CAN,command_data);
}


// 添加数据到缓存区
void AppendData(u8* msg, u8 start){
    u8 i=0;
    u8 position=0;
    for(i=start;i<8;i++){
        position=OBD_DATA_TMP.lengthCurrent;
        OBD_DATA_TMP.Data[position]=msg[i];
        // 数据接受完成
        OBD_DATA_TMP.lengthCurrent++;
        if(OBD_DATA_TMP.lengthCurrent==OBD_DATA_TMP.lengthToRec){
            //printf("-RxEnd-");
            OBD_DATA_TMP.indication_RX=true;
            break;
        }
    }
}


// CAN中断接收数据跳转函数
void OBD_CAN_RecHandler(CanRxMsg msgCanRx){
    
    u8 bytePCI=msgCanRx.Data[0]&0xF0;
    
    //  错误处理--尚未开启本次服务请求
    if(!OBD_DATA_TMP.indication_Request){
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
            OBD_DATA_TMP.lengthToRec=msgCanRx.Data[0]&0x0F;   // 保存待接受数据长度
            AppendData(msgCanRx.Data,1);        //  缓存接收数据
            OBD_DATA_TMP.indication_RX =true;
            //OBD_DeInit();
            break;
        
        case PCItype_FF:
            if(OBD_DATA_TMP.indication_FF){
                OBD_ErrorHandler(N_WRONG_FF);
                return;
            }
            OBD_DATA_TMP.lengthToRec=((msgCanRx.Data[0]&0x0F)<<8)+msgCanRx.Data[1];   // 保存待接受数据长度
            AppendData(msgCanRx.Data,2);        //  缓存接收数据
            
            OBD_DATA_TMP.id_CAN=msgCanRx.StdId;
            printf("-FF-");
            OBD_DATA_TMP.indication_FF=true;  //  首帧标志
            OBD_SendCommand_CF();               //  发送流量控制帧
            break;

        case PCItype_CF:
            if(!OBD_DATA_TMP.indication_FF){
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
    OBD_DATA_TMP.id_CAN = OBD_CAN_ID;
    OBD_DATA_TMP.cmd = NULL;
    OBD_DATA_TMP.indication_FF=false;
    OBD_DATA_TMP.indication_Request=false;
    OBD_DATA_TMP.indication_RX=false;
    OBD_DATA_TMP.result=N_NA;
    OBD_DATA_TMP.lengthCurrent=0;
    OBD_DATA_TMP.lengthToRec=0;
    TimerOver_Rx=false;
    TimerOver_Tx=false;
}

// OBD正确接收到对应请求的后续处理
void obd_saveData(){
    u16 i=0;
	char msg[150];
	u8 offset=0;
    char time[17];
    getTimeString((char*)time);
    offset+= sprintf(msg+offset,"%s,0x%08x,0x%04x,%d,",time,OBD_DATA_TMP.id_CAN,OBD_DATA_TMP.cmd->PID_SID,OBD_DATA_TMP.lengthToRec);
    for(i=0;i<OBD_DATA_TMP.lengthToRec;i++)
        offset+= sprintf(msg+offset,"%02x ",OBD_DATA_TMP.Data[i]);
    offset+= sprintf(msg+offset,",%s,%s,%s,%s,%s",OBD_DATA_TMP.cmd->Min,OBD_DATA_TMP.cmd->Max,OBD_DATA_TMP.cmd->Per,OBD_DATA_TMP.cmd->Unit,OBD_DATA_TMP.cmd->Desc);
    offset+= sprintf(msg+offset,"\r\n");	// 为数据添加结束换行标志
    DB_SaveData(msg,offset);
    printf("\r\n[obd_save]%s",msg);
}


// 打印读取到的命令配置
void OBD_print_CmdList(){
    int num;
    printf("\r\n===Print CMD List data...\r\n");
    for(num=0;num<NUM_COMMAND;num++){
        printf("[%02d]0x%04x,%4s,%4s,%4s,%5s,%s\r\n",num,OBD_CmdList[num].PID_SID,OBD_CmdList[num].Min,OBD_CmdList[num].Max,OBD_CmdList[num].Per,OBD_CmdList[num].Unit,OBD_CmdList[num].Desc);
    }
}


void OBD_CHECK(void){
    printf("[data resolve]:");
    if(OBD_DATA_TMP.indication_RX){
        printf("[got Data]");
    }else{
        printf("[something wrong]");
    }
    printf("\r\n");
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
    
    // 获取can命令行ID
    getCanId();
    printf("\r\n\r\n==OBD_RequestCommandList==\r\n");
    for(num=0;num<NUM_COMMAND;num++){
        printf("[%02d]",num);
        // 初始化公共缓存区
        OBD_DeInit(); 
        // blin the led to indicate the running
        LED_Blin_Syn();
        // 发送查询命令
        OBD_SendCommand(&(OBD_CmdList[num]));
        // 等待接收完成
        OBD_ReceiveData();
        // 对结果进行处理
        //OBD_CHECK();
        // 延时等待下一条数据查询
        delay_ms(TIME_NextRequest);
    }
}

// 循环查询直到得到结果或者超时
void OBD_ReceiveData(){
    tmpTimer=0;
    TimerOver_Rx=false;        
    //printf("[A1]");
    while(1){
        //printf("[A2]");
        // 临时处理的方式
        if((TIME_InRequest*tmpTimer++)>TIME_Rx_OVER){
            TimerOver_Rx=true;
        }
        if(TimerOver_Rx){
            OBD_DATA_TMP.result=N_TIMEOUT_RX;
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
            if(OBD_DATA_TMP.indication_RX){
                printf("[%02d次]",tmpTimer);
                //printf("[A3]");
                obd_saveData();
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
    printf("\r\n==OBD_startTimer==");
}

