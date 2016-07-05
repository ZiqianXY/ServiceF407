/**********************************************
**
**  OBDЭ����صĸ�������ʵ�� ��by Ziqian      *
**
***********************************************/
#include "OBD.h"
#include "usart.h"
#include "delay.h"
#include "led.h"
#include "can.h"
#include "database.h"
#include "rtc.h"

// ����������ͱ�ʶ��
u8 num = 0;
u8 TimerOver_Rx;  // ���ͳ�ʱ��־
u8 TimerOver_Tx;  // ���ճ�ʱ��־
u8 ReSendTimes;     // ���·��ʹ���
u8 tmpTimer=0;      // timer��ʱ����
u32 OBD_CAN_ID;

OBD_CmdTypeDef OBD_CmdList[NUM_COMMAND];    // ����ȡ����ָ���б�
OBD_DataTypeDef OBD_DATA_TMP;



// ����OBD�����������SID-PID
bool OBD_SendCommand( OBD_CmdTypeDef* obd_command){
    u8 command_data[Length_Default]={0};
    bool sendFlag=false;
    command_data[0]=0x02;
    command_data[1]=obd_command->PID_SID>>8; 
    command_data[2]=obd_command->PID_SID & 0x00ff;
    sendFlag=CAN_SendMsg(OBD_CAN_ID,command_data);
    OBD_DATA_TMP.cmd = obd_command;
    if (sendFlag){
        //printf("-������λ-");
        OBD_DATA_TMP.indication_Request=true;
        OBD_StartTimerRx();
    }
    return sendFlag;
}


// ����OBD����֡
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


// ������ݵ�������
void AppendData(u8* msg, u8 start){
    u8 i=0;
    u8 position=0;
    for(i=start;i<8;i++){
        position=OBD_DATA_TMP.lengthCurrent;
        OBD_DATA_TMP.Data[position]=msg[i];
        // ���ݽ������
        OBD_DATA_TMP.lengthCurrent++;
        if(OBD_DATA_TMP.lengthCurrent==OBD_DATA_TMP.lengthToRec){
            //printf("-RxEnd-");
            OBD_DATA_TMP.indication_RX=true;
            break;
        }
    }
}


// CAN�жϽ���������ת����
void OBD_CAN_RecHandler(CanRxMsg msgCanRx){
    
    u8 bytePCI=msgCanRx.Data[0]&0xF0;
    
    //  ������--��δ�������η�������
    if(!OBD_DATA_TMP.indication_Request){
        OBD_ErrorHandler(N_UNEXPECTED);
        return;
    }
    //  ������--CAN����֡���Ȳ���8
    if((msgCanRx.DLC)!=Length_Default){
        OBD_ErrorHandler(N_WRING_DLC);
        return;
    }
    
    // ����PCItype��������
    switch (bytePCI){
                            
        case PCItype_SF:
            
            printf("-SF-");
            OBD_DATA_TMP.lengthToRec=msgCanRx.Data[0]&0x0F;   // ������������ݳ���
            AppendData(msgCanRx.Data,1);        //  �����������
            OBD_DATA_TMP.indication_RX =true;
            //OBD_DeInit();
            break;
        
        case PCItype_FF:
            if(OBD_DATA_TMP.indication_FF){
                OBD_ErrorHandler(N_WRONG_FF);
                return;
            }
            OBD_DATA_TMP.lengthToRec=((msgCanRx.Data[0]&0x0F)<<8)+msgCanRx.Data[1];   // ������������ݳ���
            AppendData(msgCanRx.Data,2);        //  �����������
            
            OBD_DATA_TMP.id_CAN=msgCanRx.StdId;
            printf("-FF-");
            OBD_DATA_TMP.indication_FF=true;  //  ��֡��־
            OBD_SendCommand_CF();               //  ������������֡
            break;

        case PCItype_CF:
            if(!OBD_DATA_TMP.indication_FF){
                OBD_ErrorHandler(N_WRONG_CF);
                return;
            }
            printf("-CF-%1x",msgCanRx.Data[0]&0x0F);
            AppendData(msgCanRx.Data,1);        //  �����������
            break;
        
        default :
            OBD_ErrorHandler(N_WRONG_PCItype);
            return;
    }

}

// ����OBD�����������
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

// OBD��ȷ���յ���Ӧ����ĺ�������
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
    offset+= sprintf(msg+offset,"\r\n");	// Ϊ������ӽ������б�־
    DB_SaveData(msg,offset);
    printf("\r\n[obd_save]%s",msg);
}


// ��ӡ��ȡ������������
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
    // ����CAN_ID���ͷ��Ͳ�ͬ��id
    if(CAN_GetBandrate()==CAN_Bandrate_250)
        OBD_CAN_ID = Req_29_Fun;
    else
        OBD_CAN_ID = Req_11_Fun;
}

/**
 * ��ʼOBD�������������ѯOBD_CommandList�еĸ�����������
*/
void OBD_StartService(void){
    
    // ��ȡcan������ID
    getCanId();
    printf("\r\n\r\n==OBD_RequestCommandList==\r\n");
    for(num=0;num<NUM_COMMAND;num++){
        printf("[%02d]",num);
        // ��ʼ������������
        OBD_DeInit(); 
        // blin the led to indicate the running
        LED_Blin_Syn();
        // ���Ͳ�ѯ����
        OBD_SendCommand(&(OBD_CmdList[num]));
        // �ȴ��������
        OBD_ReceiveData();
        // �Խ�����д���
        //OBD_CHECK();
        // ��ʱ�ȴ���һ�����ݲ�ѯ
        delay_ms(TIME_NextRequest);
    }
}

// ѭ����ѯֱ���õ�������߳�ʱ
void OBD_ReceiveData(){
    tmpTimer=0;
    TimerOver_Rx=false;        
    //printf("[A1]");
    while(1){
        //printf("[A2]");
        // ��ʱ����ķ�ʽ
        if((TIME_InRequest*tmpTimer++)>TIME_Rx_OVER){
            TimerOver_Rx=true;
        }
        if(TimerOver_Rx){
            OBD_DATA_TMP.result=N_TIMEOUT_RX;
            OBD_ErrorHandler(N_TIMEOUT_RX);
            break;
        }
        // �����ò�һ��
        delay_ms(TIME_InRequest);
        // ǿ�Ʋ�ѯ�ռ���
        if(CAN_HasData()){
            //-------------------------------------------------------OBD_CAN_RecHandler---------------------/    
            OBD_CAN_RecHandler(CAN_RecMsg());       
            //-------------------------------------------------------OBD_CAN_RecHandler---------------------/  
            if(OBD_DATA_TMP.indication_RX){
                printf("[%02d��]",tmpTimer);
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

