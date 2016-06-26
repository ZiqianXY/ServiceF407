#include "can.h"
#include "usart.h"

//----公共变量声明--------------------------------------------------//
u8 mCAN_BANDRATE;

//----函数声明--------------------------------------------------//
bool CAN1_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode);// CAN初始化
void Translate_Can_RxMsg( CanRxMsg messageRx);// CAN消息解析
void Translate_Can_TxMsg( CanTxMsg messageTx);// CAN消息解析
//-------------------------------------------------------------//

u8 CAN_GetBandrate(void){
    printf("%d[0-500,1-250]",mCAN_BANDRATE);
    return mCAN_BANDRATE;
}

// CAN初始化，自定义
bool CAN_ModeInit(u16 boundrate, u8 mode){
    // 设置内部波特率属性
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
//CAN初始化【explore】
//tsjw:重新同步跳跃时间单元.范围:CAN_SJW_1tq~ CAN_SJW_4tq
//tbs2:时间段2的时间单元.   范围:CAN_BS2_1tq~CAN_BS2_8tq;
//tbs1:时间段1的时间单元.   范围:CAN_BS1_1tq ~CAN_BS1_16tq
//brp :波特率分频器.范围:1~1024; tq=(brp)*tpclk1
//波特率=Fpclk1/((<tbs1+1>+<tbs2+1>+1)*brp);
//mode:CAN_Mode_Normal,普通模式;CAN_Mode_LoopBack,回环模式;
//Fpclk1的时钟在初始化的时候设置为42M,如果设置CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,CAN_Mode_LoopBack);
//则波特率为:42M/((6+7+1)*6)=500Kbps
//返回值:0,初始化OK;
//    其他,初始化失败; 
////////////////////////////////////////////////////////////////////////////////// 	
void CAN_FilterIdSet(){
    CAN_FilterInitTypeDef   CAN_FilterInitStructure;
    
    CAN_FilterInitStructure.CAN_FilterNumber=0;	  //过滤器0
  	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
    
    if(mCAN_BANDRATE==CAN_Bandrate_250){
        printf("[250k-29bit]");
        CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32位 
        CAN_FilterInitStructure.CAN_FilterIdHigh=0x18D<<7;////32位ID
        CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
        CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x1ff<<7;//32位MASK
        CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
    }else if(mCAN_BANDRATE==CAN_Bandrate_500){
        printf("[500k-11bit]");
        CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_16bit; //16位 
        // 7df和7e0-8,使用0x700过滤
        CAN_FilterInitStructure.CAN_FilterIdHigh=0x0700<<5;////16位ID
        CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0700<<5;//16位MASK
        // 不使用这一组，设为0000全匹配
        CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
        CAN_FilterInitStructure.CAN_FilterMaskIdLow=0xffff;
    }else{
        printf("[this is a wrong filter set process.]");
    }
    
   	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//过滤器0关联到FIFO0
  	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //激活过滤器0
  	CAN_FilterInit(&CAN_FilterInitStructure);//滤波器初始化
}


bool CAN1_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode){

  	GPIO_InitTypeDef        GPIO_InitStructure; 
	CAN_InitTypeDef         CAN_InitStructure;
  	

    //使能相关时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能PORTA时钟	                   											 
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//使能CAN1时钟	
	
    //初始化GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11| GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化PA11,PA12
	
	//引脚复用映射配置
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource11,GPIO_AF_CAN1); //GPIOA11复用为CAN1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource12,GPIO_AF_CAN1); //GPIOA12复用为CAN1
	  
  	//CAN单元设置
   	CAN_InitStructure.CAN_TTCM=DISABLE;	//非时间触发通信模式   
  	CAN_InitStructure.CAN_ABOM=DISABLE;	//软件自动离线管理	  
  	CAN_InitStructure.CAN_AWUM=DISABLE;//睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
  	CAN_InitStructure.CAN_NART=ENABLE;	//禁止报文自动传送 
  	CAN_InitStructure.CAN_RFLM=DISABLE;	//报文不锁定,新的覆盖旧的  
  	CAN_InitStructure.CAN_TXFP=DISABLE;	//优先级由报文标识符决定 
  	CAN_InitStructure.CAN_Mode= mode;	 //模式设置 
  	CAN_InitStructure.CAN_SJW=tsjw;	//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位 CAN_SJW_1tq~CAN_SJW_4tq
  	CAN_InitStructure.CAN_BS1=tbs1; //Tbs1范围CAN_BS1_1tq ~CAN_BS1_16tq
  	CAN_InitStructure.CAN_BS2=tbs2;//Tbs2范围CAN_BS2_1tq ~	CAN_BS2_8tq
  	CAN_InitStructure.CAN_Prescaler=brp;  //分频系数(Fdiv)为brp+1	
  	CAN_Init(CAN1, &CAN_InitStructure);   // 初始化CAN1 
    
    //配置过滤器   
 	CAN_FilterIdSet();
    
    // 中断设置
	CAN_IT_SET(CAN_IT_ENABLE);
	return true;
}   
 

#if CAN_IT_ENABLE	//使能RX0中断
    
    //中断服务函数			    
    void CAN1_RX0_IRQHandler(void){
        extern u16 time_can_receive_IT;
        time_can_receive_IT++;
        printf("[CAN_IT %03d]",time_can_receive_IT);
        CAN_RecMsg();
    }

#endif
    
void CAN_IT_SET(FunctionalState enable){    
    NVIC_InitTypeDef    NVIC_InitStructure;
    CAN_ITConfig(CAN1,CAN_IT_FMP0,enable);//FIFO0消息挂号中断允许.	
    NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // 主优先级为1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // 次优先级为0
    NVIC_InitStructure.NVIC_IRQChannelCmd = enable;
    NVIC_Init(&NVIC_InitStructure);
    //printf("[CAN_IT_Setting:%d,主优先级1，次优先级0]",enable);
}

// 获取CAN的波特率和标识符位数，如果不在适配范围则返回false
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


//can发送一组数据	
//len:数据长度(最大为8)				     
//msg:数据指针,最大为8个字节.
//返回值:0,成功;
//		 其他,失败;

bool CAN_SendMsg(u32 can_id ,u8* msg){	
    
// CAN接收一组数据
	u8 mbox;
	u16 i=0;
    u8 len;
    u8 MAX_RESENT_TIMES = 10;
	CanTxMsg TxMessage;
	
    if(can_id<(0xffff>>5)){
        TxMessage.StdId=can_id;			// 标准标识符 
        TxMessage.IDE=CAN_ID_STD; 	
    }else{
        TxMessage.ExtId=can_id;			// 扩展标识符 
        TxMessage.IDE=CAN_ID_EXT; 
    }
	TxMessage.RTR= CAN_RTR_DATA;		// 数据帧
	TxMessage.DLC= 8;				// 要发送的数据长度
    len=sizeof(msg);
    for(i=0;i<8;i++){
        if(i<len)
            TxMessage.Data[i]=msg[i];
        else
            TxMessage.Data[i]= 0;
    }
    // 打印待发送的消息
    Translate_Can_TxMsg( TxMessage);
    
	mbox= CAN_Transmit(CAN1, &TxMessage);   
	i=0; 
	while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<MAX_RESENT_TIMES)){
        i++;	//等待发送结束
        printf("\r\n....发送失败%d次，重试...",i);
    }
	if(i>=MAX_RESENT_TIMES){
        printf("\r\n...发送失败!\r\n");
        return false;
    }
	return true;	 
}



bool CAN_HasData(){
    return CAN_MessagePending(CAN1,CAN_FIFO0);
}

CanRxMsg CAN_RecMsg(){
    CanRxMsg RxMessage; 
    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);//读取数据	
    Translate_Can_RxMsg(RxMessage);// 打印接收到的数据
  
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
