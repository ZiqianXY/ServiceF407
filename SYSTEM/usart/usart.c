#include "sys.h"
#include "usart.h"	
#include "usart3.h" 
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
//V1.5�޸�˵��
//1,�����˶�UCOSII��֧��
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE { 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) { 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f){ 	
	USART1->DR = (u8) ch;  
    while((USART1->SR&0X40)==0);//ѭ������,ֱ���������       
	return ch;
}
#endif
//////////////////////////////////////////////////////////////////

 
#if EN_USART1_RX   //���ʹ���˽���	
	  	
u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
u16 USART_RX_STA;         		//����״̬���	

u16 lengthUsartBuffer=0;
u16 indexUsartBuffer=0;
/**
  ******************************************************************************
  * @brief  ���ڳ�ʼ������
  * @param  ������
  * @retval None
  ******************************************************************************
  */
void usart_init(u32 bound){
    
   //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//ʹ��USART1ʱ��
 
	//����1��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9����ΪUSART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10����ΪUSART1
	
	//USART1�˿�����
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9��GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	
    GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��PA9��PA10

   //USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
    
    USART_Init(USART1, &USART_InitStructure); //��ʼ������1
    USART_Cmd(USART1, ENABLE);  //ʹ�ܴ���1 
	
	//USART_ClearFlag(USART1, USART_FLAG_TC);
	
#if EN_USART1_RX	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//��������ж�

	//Usart1 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����

#endif
	
    USART_ClearFlag(USART1,USART_FLAG_TC);//�����ͱ�־λ���㣬�Ա����һ��ʹ�ô��ڶ�ʧ��һ������
}


void usart_reset(void){
  int i =0;
  USART_RX_STA=0;
  for(i=0;i<USART_REC_LEN;i++)
    USART_RX_BUF[i]=0;
} 

/**
  ******************************************************************************
  * @brief  ���ڽ�����ϱ�־,�ߵ�2λΪ��־λ    
  * @param  None
  * @retval 1�����յ���Ч���ݣ�
            0��δ���յ�
  ******************************************************************************
  */
u8 usart_hasData(void){
    return USART_RX_STA&0x8000;
}

/**
  ******************************************************************************
  * @brief  ��ȡ���ڽ������ݳ���,��12λΪ��Чλ    
  ******************************************************************************
  */
u16 usart_getDataLength(void){
    return USART_RX_STA&0x0fff;
}

void send2usart3(void){
    u16 lengthUsart;
    u16 indexUsartTmp;
    lengthUsart=usart_getDataLength();//�õ��˴ν��յ������ݳ���
//    printf("\r\n[$:]");
//    for(indexUsartTmp=0;indexUsartTmp<lengthUsart;indexUsartTmp++){
//      printf("%d ",USART_RX_BUF[indexUsartTmp]);
//      u3_printf("%d ",USART_RX_BUF[indexUsartTmp]);
//    }
    u3_printf("%s\r\n",USART_RX_BUF);	//��������
    printf("r\n[$:]%s[END]\r\n",USART_RX_BUF);
    usart_reset();     //����״̬��� 
}


/**
  ******************************************************************************
  * @brief  ����USART1�жϺ���     
  * @param  None
  * @retval None
  ******************************************************************************
  */
void USART1_IRQHandler(void)                	//����1�жϷ������
{
	u8 Receive;
    
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
    {     
		Receive =USART_ReceiveData(USART1);	//��ȡ���յ�������		

        if(USART_RX_STA&STA_RECEIVE_START)
        {      
            
            if((USART_RX_STA&STA_RECEIVE_END)==0)//����δ���
            {    
                if(USART_RX_STA&STA_RECEIVE_END_ING)//�ϴν��յ���0x0d
                {
                    if(Receive==REC_END){
                      USART_RX_STA|=STA_RECEIVE_END;	//���������
                      send2usart3();
                    }
                    else 
                        USART_RX_STA=0;//���մ���,���¿�ʼ
                }
                else //�ϴ�û�յ�0X0D
                {
                    if(Receive==REC_END_ING){   //��ν��յ���0x0d
                        USART_RX_STA|=STA_RECEIVE_END_ING;
                    }
                    else{   //�����Ȼû���յ�0x0d
                        USART_RX_STA++; //���ݳ��ȼ�1
                        lengthUsartBuffer=USART_RX_STA&0X0FFF;
                        USART_RX_BUF[lengthUsartBuffer-1]=Receive ; //����������������
                        if(lengthUsartBuffer==USART_REC_LEN){
                            USART_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
                        }
                    }		 
                } 
            } 
        }else{
            if(USART_RX_STA&STA_RECEIVE_START_ING) { //�ϴ��յ���׼����ʼ���
               //printf("4");
                if(Receive==REC_START)  // ���ܵ���ʼ���
                    USART_RX_STA|=STA_RECEIVE_START;    //��ʼ���λ��λ
                else    //û�յ���ʼ��ǣ���������
                    USART_RX_STA=0;
                
            }else { //�ϴ�û�յ�׼����ʼ���

                if(Receive==REC_START_ING)  //����յ�׼����ʼ���
                    USART_RX_STA|=STA_RECEIVE_START_ING;    //׼����ʼ���λ��λ
            }
        }    
    }        
} 




#endif	

