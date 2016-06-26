#ifndef __DATABASE_H
#define __DATABASE_H	 

#include <stdbool.h>
#include "sys.h"

//#define CONVERT_ENABLE      1
//#define CONVERT_DISABLE     0

void DB_Init(void);

void DB_SaveData(char* data, int length);

void DB_ReadData(void);

bool DB_HasData(void);

void DB_TEST(void);

#endif
