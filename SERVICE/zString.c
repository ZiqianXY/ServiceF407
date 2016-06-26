#include "zString.h"
#include "string.h"

//È¥³ý×Ö·û´®ÖÐËùÓÐ¿Õ¸ñ 
void Z_StrTrim(char*pStr)  
{  
    char *pTmp = pStr;  
      
    while (*pStr != 0)   
    {  
        if(*pTmp == ' '||*pTmp == '\t')   
        {  
            *pTmp++ = *pStr;  
        }  
        ++pStr;  
    }  
    *pTmp = 0;  
}  
  
//È¥³ý×Ö·û´®ÓÒ±ß¿Õ¸ñ
void Z_StrRTrim(char *pStr)  
{  
    char *pTmp = pStr + strlen(pStr)-1;  
      
    while (*pTmp == ' '||*pTmp == '\t') 
    {  
        *pTmp = 0;  
        pTmp--;  
    }  
}  
  
//È¥³ý×Ö·û´®Î²²¿»»ÐÐºÍ¿ÕÏ¶
void Z_StrETrim(char *pStr)  
{  
    char *pTmp = pStr + strlen(pStr)-1;  
      
    while (*pTmp == '\r'||*pTmp == '\n'||*pTmp == ' '||*pTmp == '\t') 
    {  
        *pTmp = 0;  
        pTmp--;  
    }  
}  
 
//È¥³ý×Ö·û´®×ó±ß¿Õ¸ñ
void Z_StrLTrim(char *pStr)  
{  
    char *pTmp = pStr;  
      
    while (*pTmp == ' '||*pTmp == '\t')   
    {  
        pTmp++;  
    }  
    while(*pTmp != 0)  
    {  
        *pStr = *pTmp;  
        pStr++;  
        pTmp++;  
    }  
    *pStr = 0;  
}

void Z_StrTrimHE(char *pStr){
	Z_StrLTrim(pStr);
	Z_StrRTrim(pStr);
}


int tolower(int c) { 
    if (c >= 'A' && c <= 'Z') 
        return c + 'a' - 'A'; 
    else 
        return c; 
} 

int Z_Hex2i(char s[]) { 
    int i; 
    int n = 0; 
    if (s[0] == '0' && (s[1]=='x' || s[1]=='X')) 
    { 
        i = 2; 
    } 
    else 
    { 
        i = 0; 
    } 
    for (; (s[i] >= '0' && s[i] <= '9') 
        || (s[i] >= 'a' && s[i] <= 'z') || (s[i] >='A' && s[i] <= 'Z');++i) 
    {         
        if (tolower(s[i]) > '9') 
        { 
            n = 16 * n + (10 + tolower(s[i]) - 'a'); 
        } 
        else 
        { 
            n = 16 * n + (tolower(s[i]) - '0'); 
        } 
    } 
    return n; 
} 
