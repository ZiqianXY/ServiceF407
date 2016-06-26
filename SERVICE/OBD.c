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
static OBD_CommandTypeDef OBD_CommandList[NUM_COMMAND]={{0,0}};    // ����ȡ����ָ���б�
static OBD_CommandTypeDef OBD_CommandStruct;   // һ������ָ��
static OBD_USDataTypeDef OBD_DataStruct;       // ���ݻ���ṹ��
bool TimerOver_Rx;  // ���ͳ�ʱ��־
bool TimerOver_Tx;  // ���ճ�ʱ��־
u8 ReSendTimes;     // ���·��ʹ���
u8 tmpTimer=0;      // timer��ʱ����
u32 OBD_CAN_ID;

// ����OBD�����������SID-PID
bool OBD_SendCommand( OBD_CommandTypeDef obd_command){
    u8 command_data[Length_Default]={0};
    bool sendFlag=false;
    command_data[0]=0x02;
    command_data[1]=obd_command.SID;
    command_data[2]=obd_command.PID;
    
    sendFlag=CAN_SendMsg(OBD_CAN_ID,command_data);
    
    if (sendFlag){
			//printf("-������λ-");
			OBD_DataStruct.indication_Request=true;
			OBD_DataStruct.command=obd_command;
			OBD_DataStruct.id_CAN=OBD_CAN_ID;
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
    return CAN_SendMsg(OBD_DataStruct.id_CAN,command_data);
}


// ������ݵ�������
void AppendData(u8* msg, u8 start){
    u8 i=0;
    u8 position=0;
    for(i=start;i<8;i++){
        position=OBD_DataStruct.lengthCurrent;
        OBD_DataStruct.Data[position]=msg[i];
        //printf("[%d][%d]",i,msg[i]);
        //printf("[%d][%d]",position,OBD_DataStruct.Data[position]);
        // ���ݽ������
        OBD_DataStruct.lengthCurrent++;
        if(OBD_DataStruct.lengthCurrent==OBD_DataStruct.lengthToRec){
            //printf("-RxEnd-");
            OBD_DataStruct.indication_RX=true;
            break;
        }
    }
}


// CAN�жϽ���������ת����
void OBD_CAN_RecHandler(CanRxMsg msgCanRx){
    
    u8 bytePCI=msgCanRx.Data[0]&0xF0;
    
    //  ������--��δ�������η�������
    if(!OBD_DataStruct.indication_Request){
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
            OBD_DataStruct.lengthToRec=msgCanRx.Data[0]&0x0F;   // ������������ݳ���
            AppendData(msgCanRx.Data,1);        //  �����������
            OBD_DataStruct.indication_RX =true;
            //OBD_DeInit();
            break;
        
        case PCItype_FF:
            if(OBD_DataStruct.indication_FF){
                OBD_ErrorHandler(N_WRONG_FF);
                return;
            }
            OBD_DataStruct.lengthToRec=((msgCanRx.Data[0]&0x0F)<<8)+msgCanRx.Data[1];   // ������������ݳ���
            AppendData(msgCanRx.Data,2);        //  �����������
            
            OBD_DataStruct.id_CAN=msgCanRx.StdId;
            printf("-FF-");
            OBD_DataStruct.indication_FF=true;  //  ��֡��־
            OBD_SendCommand_CF();               //  ������������֡
            break;

        case PCItype_CF:
            if(!OBD_DataStruct.indication_FF){
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

// OBD��ȷ���յ���Ӧ����ĺ�������
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
		offset+= sprintf(msg+offset,",#\r\n");	// Ϊ������ӽ������б�־
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
    u16 i=0;
    // ��ȡcan������ID
    getCanId();
    // ��ȡ����ȡ����
    OBD_ReadCommandList();
    printf("\r\n\r\n==OBD_RequestCommandList==\r\n");
    
    for(i=0;i<NUM_COMMAND;i++){
        printf("[%02d]",i);
        // ��ʼ������������
        OBD_DeInit(); 
        // ��ȡһ������ָ��
        OBD_CommandStruct=OBD_CommandList[i];
        // blin the led to indicate the running
        LED_Blin_Syn();

        // ���Ͳ�ѯ����
        OBD_SendCommand(OBD_CommandStruct);
        // �ȴ��������
        OBD_ReceiveData();
        // �Խ�����д���
        OBD_CHECK();
        // ��ʱ�ȴ���һ�����ݲ�ѯ
        delay_ms(TIME_NextRequest);
    }
    
    OBD_WIFI();
    
}

/**
 * test
*/
void OBD_TEST(void){
    static u8 i=0;
    // ��ȡcan������ID
    getCanId();
    // ��ȡ����ȡ����
    OBD_ReadCommandList();
    
		printf("[%02d]",i);
		// ��ʼ������������
		OBD_DeInit(); 
		// ��ȡһ������ָ��
		OBD_CommandStruct=OBD_CommandList[i];
		// blin the led to indicate the running
		LED_Blin_Syn();

		// ���Ͳ�ѯ����
		OBD_SendCommand(OBD_CommandStruct);
		// �ȴ��������
		OBD_ReceiveData();
	
		// �Խ�����д���
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
        // ��ʱ����ķ�ʽ
        if((TIME_InRequest*tmpTimer++)>TIME_Rx_OVER){
            TimerOver_Rx=true;
        }
        
        if(TimerOver_Rx){
            OBD_DataStruct.result=N_TIMEOUT_RX;
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
            if(OBD_DataStruct.indication_RX){
                printf("[%02d��]",tmpTimer);
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
    static bool isCommandListRead=false;   // �����ô�����
    if(isCommandListRead)   return;
    isCommandListRead=true;
    
    printf("\r\n==OBD_ReadCommandList==\r\n");
    for(i=0;i<NUM_COMMAND;i++){
        OBD_CommandList[i]=TMP_CommandList[i];
        printf("[%02d][0x]%02x %02x\r\n",i,OBD_CommandList[i].SID,OBD_CommandList[i].PID);
    }
		printf("==========\r\n");
}
