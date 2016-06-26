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
/////////////////////////////////////////
/*******变量定义*****/
FIL fil;
FRESULT res=FR_OK;
UINT bww=0;
#define BUFFER_LEN  100		// 一次读取数据buffer长度
#define TEST_LEN 	 	50		// 最大测试数据长度
char buf[BUFFER_LEN];
char dataBuffer[TEST_LEN] = "Time,SID-PID,Min,Max,Per,Unit,Description\r\n";
char* root = "0:";
char* dirName = "_data";
char fileName[40] = "0:/_data/data.csv";
char* configName = "0:/config";
///////////////////////////////////////
u32 total_memory,free_memory;


void DB_Init(void){
    
	//////////////////////////////////////////////
	W25QXX_Init();				//初始化W25Q128
	my_mem_init(SRAMIN);		//初始化内部内存池 
	my_mem_init(SRAMCCM);		//初始化CCM内存池
    
  while(SD_Init()){//检测不到SD卡
		printf("\r\nNo SD Card, init Error!");
		delay_ms(500);					
		LED_Blin_Green();//DS0闪烁
	}
    
	exfuns_init();							//为fatfs相关变量申请内存
	
	res = f_mount(fs[0],"0:",1);    printf("\r\n挂载SdCard：%d",res);
	res=f_mkdir(dirName);	if(res)  printf("[error:creat dir][%d]",res);// 创建数据保存文件夹
	res = f_mount(fs[1],"1:",1);    printf("\r\n挂载Flash ：%d",res);
	if(res==0X0D){//FLASH磁盘,FAT文件系统错误,重新格式化FLASH
		printf("Flash Disk Formatting...");	//格式化FLASH
		res=f_mkfs("1:",1,4096);//格式化FLASH,1,盘符;1,不需要引导区,8个扇区为1个簇
		if(res==0){
			f_setlabel((const TCHAR *)"1:ALIENTEK");	//设置Flash磁盘的名字为：ALIENTEK
			printf("Flash Disk Format Finish");	//格式化完成
		}else 
			printf("Flash Disk Format Error ");	//格式化失败
		delay_ms(1000);
	}		
    
  while(exf_getfree("0:",&total_memory,&free_memory)){	//得到SD卡的总容量和剩余容量
		printf("SD Card Fatfs Error!");
		delay_ms(200);
    LED_Blin_Green();//DS0闪烁
	}	
  printf("\r\nSD总容量：%d,剩余：%d",total_memory>>10,free_memory>>10);
    
  while(exf_getfree("1:",&total_memory,&free_memory)){	//得到SD卡的总容量和剩余容量
		printf("SD Card Fatfs Error!");
		delay_ms(200);
    LED_Blin_Green();//DS0闪烁
	}	
  printf("\r\nfalsh总容量：%d,剩余：%d",total_memory>>10,free_memory>>10);
}


void DB_READ_CONFIG(){
	char * data ;  
	int len;
	
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
		result = strtok( data, delims ); Z_StrTrimHE(result); printf("\r\n[%s][%d][%04x]", result, Z_Hex2i(result), Z_Hex2i(result) );
		result = strtok( NULL, delims ); Z_StrTrimHE(result); printf("\t[min][%s]", result );
		result = strtok( NULL, delims ); Z_StrTrimHE(result); printf("\t[max][%s]", result );
		result = strtok( NULL, delims ); Z_StrTrimHE(result); printf("\t[per][%s]", result );
		result = strtok( NULL, delims ); Z_StrTrimHE(result); printf("\t[uni][%s]", result );
		result = strtok( NULL, delims ); Z_StrTrimHE(result); printf("\t[dsc][%s]", result );
		
		//printf("%d",Z_Hex2i("0xa4df"));
		//while( result != NULL ) {
		//	printf("[%s]", result );
		//	result = strtok( NULL, delims );
		//}
	}
	f_close(&fil);
  myfree(SRAMIN,data);
}


void DB_SetFileName(){
	char time[17];
	getTimeString((char*)time);
	sprintf(fileName,"%s/%s/%s.csv",root,dirName,time);
	printf("<%s><%s>\r\n",fileName,time);
}


void DB_TEST(void){
	DB_SetFileName();
	printf("<%s>\r\n",fileName);
	DB_SaveData(dataBuffer, TEST_LEN);
	printf("<%s>\r\n",fileName);
	DB_ReadData();
	printf("<%s>\r\n",fileName);
	DB_READ_CONFIG();
	printf("<%s>\r\n",fileName);
}


void DB_SaveData(char* data, int length){
	
	res=f_open (&fil,fileName, FA_OPEN_ALWAYS|FA_WRITE|FA_READ);	if(res)  printf("[error:open][%d]",res);
	/* Move to end of the file to append data */
	res = f_lseek(&fil, f_size(&fil));    if(res)  printf("[error:move-fir][%d]",res);
	printf("\r\n[DB_SaveData<%s>]",data);
	res=f_write (&fil, data, length, &bww); if(res) printf("[error:write][%d]",res);
	f_close(&fil);
}

void DB_ReadData(void){
	char* outStr=NULL;
	u32 size=0;
	res=f_open (&fil,fileName, FA_READ); 
	size = f_size(&fil);
	outStr = mymalloc(SRAMIN,size+1);
	f_read (&fil, outStr, size, &bww);
	f_close(&fil);
	printf("\r\n[DB_open_file]%s[size]%d\r\n[data]%s",fileName,size,outStr);
	myfree(SRAMIN,outStr);
}
