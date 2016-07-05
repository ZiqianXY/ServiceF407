//#include <stdlib.h>
#include "database.h"
#include "usart.h"
#include "led.h"
#include "delay.h"
#include "rtc.h"
#include "string.h"
#include "zString.h"
////////////////////////////////
// 卡文件相关的一堆
#include "sram.h"   
#include "malloc.h" 
#include "sdio_sdcard.h"    
#include "malloc.h" 
#include "w25qxx.h"    
#include "ff.h"  
#include "exfuns.h"    
////////////////////////////////////
#include "wifista.h"  
#include "OBD.h"
/////////////////////////////////////////
/*******变量定义*****/
FIL fil;
FRESULT res=FR_OK;
UINT bww=0;
#define BUFFER_LEN  100		// 一次读取数据buffer长度
char buf[BUFFER_LEN];
char *HeadData = "Time,CAN_ID,SID-PID,Len,Data,Min,Max,Per,Unit,Description\r\n";
char* Drive_SD = "0:";
char* Drive_Flash = "1:";
char* dirName = "_data";
char fileName[40] = "0:/_data/data.csv";
char* configName = "0:/config";
///////////////////////////////////////
u32 total_memory,free_memory;


void DB_get_memory_info(){
        
    while(exf_getfree(Drive_SD,&total_memory,&free_memory)){	//得到SD卡的总容量和剩余容量
		printf("SD Card Fatfs Error!");
		delay_ms(200);
        LED_Blin_Green();//DS0闪烁
	}	
    printf("\r\nSD总容量：%d,剩余：%d",total_memory>>10,free_memory>>10);
    
    while(exf_getfree(Drive_Flash,&total_memory,&free_memory)){	//得到Flash卡的总容量和剩余容量
		printf("SD Card Fatfs Error!");
		delay_ms(200);
        LED_Blin_Green();//DS0闪烁
    }	
    printf("\r\nFalsh总容量：%d,剩余：%d\r\n",total_memory>>10,free_memory>>10);
}

void DB_fatfs_init(){
    
    exfuns_init();							//为fatfs相关变量申请内存
	res = f_mount(fs[0],Drive_SD,1);
	if(res) printf("\r\n挂载SdCard：%d",res);
    //res = f_mkdir(dirName);
	//if(res)  printf("[error:creat _data dir][%d]",res);// 创建数据保存文件夹
	res = f_mount(fs[1],Drive_Flash,1);    
    if(res) printf("\r\n挂载Flash ：%d",res);
	if(res==0X0D){//FLASH磁盘,FAT文件系统错误,重新格式化FLASH
		printf("Flash Disk Formatting...");	//格式化FLASH
		res=f_mkfs(Drive_Flash,1,4096);//格式化FLASH,1,盘符;1,不需要引导区,8个扇区为1个簇
		if(res==0){
			f_setlabel((const TCHAR *)"1:ALIENTEK");	//设置Flash磁盘的名字为：ALIENTEK
			printf("Flash Disk Format Finish");	//格式化完成
		}else 
			printf("Flash Disk Format Error ");	//格式化失败
		delay_ms(1000);
	}	
}


// 设置文件名为当前系统时间
void db_set_file_name(){
	char time[17];
	getTimeString((char*)time);
	sprintf(fileName,"%s/%s/%s.csv",Drive_SD,dirName,time);
	printf(">SetFileName<%s>\r\n",fileName);
}


void DB_READ_CONFIG(){
	char * data ;  
	int len;
    int i=0;
	f_open (&fil,configName, FA_READ); 
	len=f_size(&fil); 
	data = mymalloc(SRAMIN,len+1);
	while(!f_eof(&fil)){
		char delims[] = ",";
		char *result = NULL;
		f_gets(data,len,&fil);
		Z_StrETrim(data);
		//printf("\r\n{%s}\r\n",data);
		// strtok(s,delims) 第一次使用需要对s给值，后续为Null
		result = strtok( data, delims ); Z_StrTrimHE(result); OBD_CmdList[i].PID_SID = Z_Hex2i(result);
		result = strtok( NULL, delims ); Z_StrTrimHE(result); OBD_CmdList[i].Min= mymalloc(SRAMIN,strlen(result)+1);    strcpy( OBD_CmdList[i].Min , result);
		result = strtok( NULL, delims ); Z_StrTrimHE(result); OBD_CmdList[i].Max= mymalloc(SRAMIN,strlen(result)+1);    strcpy( OBD_CmdList[i].Max , result);
		result = strtok( NULL, delims ); Z_StrTrimHE(result); OBD_CmdList[i].Per= mymalloc(SRAMIN,strlen(result)+1);    strcpy( OBD_CmdList[i].Per , result); 
		result = strtok( NULL, delims ); Z_StrTrimHE(result); OBD_CmdList[i].Unit= mymalloc(SRAMIN,strlen(result)+1);   strcpy( OBD_CmdList[i].Unit , result);
		result = strtok( NULL, delims ); Z_StrTrimHE(result); OBD_CmdList[i].Desc= mymalloc(SRAMIN,strlen(result)+1);   strcpy( OBD_CmdList[i].Desc , result);
		i++;
	}
	f_close(&fil);
    myfree(SRAMIN,data);
    
}

// 文件系统的初始化
void DB_PreConfig(void){
	db_set_file_name();
	DB_SaveData(HeadData, strlen(HeadData));
	DB_ReadData();
	DB_READ_CONFIG();
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// public functions

// 初始化
void DB_Init(void){
    
	W25QXX_Init();				//初始化W25Q128
	my_mem_init(SRAMIN);		//初始化内部内存池 
	my_mem_init(SRAMCCM);		//初始化CCM内存池
    
    while(SD_Init()){//检测不到SD卡
		printf("\r\nNo SD Card, init Error!");
		delay_ms(500);					
		LED_Blin_Green();//DS0闪烁
	}
    DB_fatfs_init();
    DB_get_memory_info();
    DB_PreConfig();
}


// 保存数据到文件中 
u8 DB_SaveData(char* data, int length){
	if(f_open (&fil,fileName, FA_OPEN_ALWAYS|FA_WRITE|FA_READ))  
        printf("[error:open][%d]",res);
	/* Move to end of the file to append data */
	if(f_lseek(&fil, f_size(&fil)))  
        printf("[error:move-fir][%d]",res);
	res=f_write (&fil, data, length, &bww); 
    if(res) 
        printf("[error:write][%d]",res);
	f_close(&fil);
    return res;
}

// 读取当前文件的全部数据
void DB_ReadData(void){
	char* outStr=NULL;
	u32 size=0;
	res=f_open (&fil,fileName, FA_READ); 
	size = f_size(&fil);
	outStr = mymalloc(SRAMIN,size+1);
	f_read (&fil, outStr, size, &bww);
	f_close(&fil);
	printf("[DB_ReadData]%s[%d B]\r\n[data]%s",fileName,size,outStr);
	myfree(SRAMIN,outStr);
}


u8 DB_WIFI_UPLOAD(){
	char * dataLine ;  
	int len=50;
    u8 res =0;
	
    WIFI_connect2wifi();
    WIFI_connect2server();
    WIFI_trans_start();
    
	f_open (&fil,fileName, FA_READ); 
	dataLine = mymalloc(SRAMIN,len+1);
	while(!f_eof(&fil)){
		f_gets(dataLine,len,&fil);
		res = WIFI_send_data(dataLine);
	}
	f_close(&fil);
    myfree(SRAMIN,dataLine);
    return res;
}
