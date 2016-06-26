#include "can.h"
#include "usart.h"

//----������������--------------------------------------------------//
u8 mCAN_BANDRATE;

//----��������--------------------------------------------------//
bool CAN1_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode);// CAN��ʼ��
void Translate_Can_RxMsg( CanRxMsg messageRx);// CAN��Ϣ����
void Translate_Can_TxMsg( CanTxMsg messageTx);// CAN��Ϣ����
//-------------------------------------------------------------//

u8 CAN_GetBandrate(void){
    printf("%d[0-500,1-250]",mCAN_BANDRATE);
    return mCAN_BANDRATE;
}

// CAN��ʼ�����Զ���
bool CAN_ModeInit(u16 boundrate, u8 mode){
    // �����ڲ�����������
    mCAN_BANDRATE=boundrate;
    
    if(mode==CAN_Mode_Normal)
        printf("[Nom]");
    else 
        printf("[Lop]");
    
    if(mCAN_BANDRATE==CAN_Bandrate_250){
        printf("[250k-29b-E]");
        CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,12,mode);//250kbps????????????????????????????????????????????????????????????????????????????
        return true;
    }else if(mCAN_BANDRATE==CAN_Bandrate_500){
        printf("[500k-11b-S]");
        CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,mode);//500kbps
        return true;
    }
    else{
        printf("[Error: failed to init can bandrate!]");
        return false;
    }
}


////////////////////////////////////////////////////////////////////////////////// 	 
//CAN��ʼ����explore��
//tsjw:����ͬ����Ծʱ�䵥Ԫ.��Χ:CAN_SJW_1tq~ CAN_SJW_4tq
//tbs2:ʱ���2��ʱ�䵥Ԫ.   ��Χ:CAN_BS2_1tq~CAN_BS2_8tq;
//tbs1:ʱ���1��ʱ�䵥Ԫ.   ��Χ:CAN_BS1_1tq ~CAN_BS1_16tq
//brp :�����ʷ�Ƶ��.��Χ:1~1024; tq=(brp)*tpclk1
//������=Fpclk1/((<tbs1+1>+<tbs2+1>+1)*brp);
//mode:CAN_Mode_Normal,��ͨģʽ;CAN_Mode_LoopBack,�ػ�ģʽ;
//Fpclk1��ʱ���ڳ�ʼ����ʱ������Ϊ42M,�������CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,CAN_Mode_LoopBack);
//������Ϊ:42M/((6+7+1)*6)=500Kbps
//����ֵ:0,��ʼ��OK;
//    ����,��ʼ��ʧ��; 
////////////////////////////////////////////////////////////////////////////////// 	
void CAN_FilterIdSet(){
    CAN_FilterInitTypeDef   CAN_FilterInitStructure;
    
    CAN_FilterInitStructure.CAN_FilterNumber=0;	  //������0
  	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
    
    if(mCAN_BANDRATE==CAN_Bandrate_250){
        printf("[250k-29bit]");
        CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32λ 
        CAN_FilterInitStructure.CAN_FilterIdHigh=0x18D<<7;////32λID
        CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
        CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x1ff<<7;//32λMASK
        CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
    }else if(mCAN_BANDRATE==CAN_Bandrate_500){
        printf("[500k-11bit]");
        CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_16bit; //16λ 
        // 7df��7e0-8,ʹ��0x700����
        CAN_FilterInitStructure.CAN_FilterIdHigh=0x0700<<5;////16λID
        CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0700<<5;//16λMASK
        // ��ʹ����һ�飬��Ϊ0000ȫƥ��
        CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
        CAN_FilterInitStructure.CAN_FilterMaskIdLow=0xffff;
    }else{
        printf("[this is a wrong filter set process.]");
    }
    
   	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//������0������FIFO0
  	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //���������0
  	CAN_FilterInit(&CAN_FilterInitStructure);//�˲�����ʼ��
}


bool CAN1_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode){

  	GPIO_InitTypeDef        GPIO_InitStructure; 
	CAN_InitTypeDef         CAN_InitStructure;
  	

    //ʹ�����ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��PORTAʱ��	                   											 
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//ʹ��CAN1ʱ��	
	
    //��ʼ��GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11| GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
    GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��PA11,PA12
	
	//���Ÿ���ӳ������
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource11,GPIO_AF_CAN1); //GPIOA11����ΪCAN1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource12,GPIO_AF_CAN1); //GPIOA12����ΪCAN1
	  
  	//CAN��Ԫ����
   	CAN_InitStructure.CAN_TTCM=DISABLE;	//��ʱ�䴥��ͨ��ģʽ   
  	CAN_InitStructure.CAN_ABOM=DISABLE;	//����Զ����߹���	  
  	CAN_InitStructure.CAN_AWUM=DISABLE;//˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)
  	CAN_InitStructure.CAN_NART=ENABLE;	//��ֹ�����Զ����� 
  	CAN_InitStructure.CAN_RFLM=DISABLE;	//���Ĳ�����,�µĸ��Ǿɵ�  
  	CAN_InitStructure.CAN_TXFP=DISABLE;	//���ȼ��ɱ��ı�ʶ������ 
  	CAN_InitStructure.CAN_Mode= mode;	 //ģʽ���� 
  	CAN_InitStructure.CAN_SJW=tsjw;	//����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ CAN_SJW_1tq~CAN_SJW_4tq
  	CAN_InitStructure.CAN_BS1=tbs1; //Tbs1��ΧCAN_BS1_1tq ~CAN_BS1_16tq
  	CAN_InitStructure.CAN_BS2=tbs2;//Tbs2��ΧCAN_BS2_1tq ~	CAN_BS2_8tq
  	CAN_InitStructure.CAN_Prescaler=brp;  //��Ƶϵ��(Fdiv)Ϊbrp+1	
  	CAN_Init(CAN1, &CAN_InitStructure);   // ��ʼ��CAN1 
    
    //���ù�����   
 	CAN_FilterIdSet();
    
    // �ж�����
	CAN_IT_SET(CAN_IT_ENABLE);
	return true;
}   
 

#if CAN_IT_ENABLE	//ʹ��RX0�ж�
    
    //�жϷ�����			    
    void CAN1_RX0_IRQHandler(void){
        extern u16 time_can_receive_IT;
        time_can_receive_IT++;
        printf("[CAN_IT %03d]",time_can_receive_IT);
        CAN_RecMsg();
    }

#endif
    
void CAN_IT_SET(FunctionalState enable){    
    NVIC_InitTypeDef    NVIC_InitStructure;
    CAN_ITConfig(CAN1,CAN_IT_FMP0,enable);//FIFO0��Ϣ�Һ��ж�����.	
    NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // �����ȼ�Ϊ1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // �����ȼ�Ϊ0
    NVIC_InitStructure.NVIC_IRQChannelCmd = enable;
    NVIC_Init(&NVIC_InitStructure);
    //printf("[CAN_IT_Setting:%d,�����ȼ�1�������ȼ�0]",enable);
}

// ��ȡCAN�Ĳ����ʺͱ�ʶ��λ��������������䷶Χ�򷵻�false
bool CAN_GetProtocal(void){
    // TODO to get real bandrate. 
    // here for test juset set it to 250 for SAE-J1939.
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++TODO
    printf("[do something here to get the protocal type] \r\n");
    CAN_SetProtocal(CAN_Bandrate_250,CAN_ID_EXT);
    return true;
}

void CAN_SetProtocal(u8 bandrate, u8 idType){
    mCAN_BANDRATE = bandrate;
    CAN_FilterIdSet();
}


//can����һ������	
//len:���ݳ���(���Ϊ8)				     
//msg:����ָ��,���Ϊ8���ֽ�.
//����ֵ:0,�ɹ�;
//		 ����,ʧ��;

bool CAN_SendMsg(u32 can_id ,u8* msg){	
    
// CAN����һ������
	u8 mbox;
	u16 i=0;
    u8 len;
    u8 MAX_RESENT_TIMES = 10;
	CanTxMsg TxMessage;
	
    if(can_id<(0xffff>>5)){
        TxMessage.StdId=can_id;			// ��׼��ʶ�� 
        TxMessage.IDE=CAN_ID_STD; 	
    }else{
        TxMessage.ExtId=can_id;			// ��չ��ʶ�� 
        TxMessage.IDE=CAN_ID_EXT; 
    }
	TxMessage.RTR= CAN_RTR_DATA;		// ����֡
	TxMessage.DLC= 8;				// Ҫ���͵����ݳ���
    len=sizeof(msg);
    for(i=0;i<8;i++){
        if(i<len)
            TxMessage.Data[i]=msg[i];
        else
            TxMessage.Data[i]= 0;
    }
    // ��ӡ�����͵���Ϣ
    Translate_Can_TxMsg( TxMessage);
    
	mbox= CAN_Transmit(CAN1, &TxMessage);   
	i=0; 
	while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<MAX_RESENT_TIMES)){
        i++;	//�ȴ����ͽ���
        printf("\r\n....����ʧ��%d�Σ�����...",i);
    }
	if(i>=MAX_RESENT_TIMES){
        printf("\r\n...����ʧ��!\r\n");
        return false;
    }
	return true;	 
}



bool CAN_HasData(){
    return CAN_MessagePending(CAN1,CAN_FIFO0);
}

CanRxMsg CAN_RecMsg(){
    CanRxMsg RxMessage; 
    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);//��ȡ����	
    Translate_Can_RxMsg(RxMessage);// ��ӡ���յ�������
  
    return RxMessage;
}


void Translate_Can_RxMsg( CanRxMsg message){		   		   
 	u8 i;
    printf("\r\n--->[Rx]");
    if(message.IDE==CAN_Id_Standard)        
        printf(" [%03x]",message.StdId);
    else        
        printf(" [%08x]",message.ExtId);
    for(i=0;i<message.DLC;i++){
        printf("%02x ",message.Data[i]);
    }
}

void Translate_Can_TxMsg( CanTxMsg message){		   		   
  	u8 i;
    printf("\r\n--->[Tx]");
    if(message.IDE==CAN_Id_Standard)        
        printf(" [%03x]",message.StdId);
    else        
        printf(" [%08x]",message.ExtId);
    for(i=0;i<3;i++){
        printf("%02x ",message.Data[i]);
    }
}
