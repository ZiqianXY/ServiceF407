#ifndef __DATABASE_H
#define __DATABASE_H	 

#include <stdbool.h>
#include "sys.h"

//#define CONVERT_ENABLE      1
//#define CONVERT_DISABLE     0

void DB_Init(void);

u8 DB_SaveData(char* data, int length);

void DB_ReadData(void);

bool DB_HasData(void);

u8 DB_WIFI_UPLOAD(void);

#endif
