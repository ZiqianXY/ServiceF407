#ifndef _Z_STRING_
#define _Z_STRING_

//È¥³ý×Ö·û´®ÖÐËùÓÐ¿Õ¸ñ 
void Z_StrTrim(char*pStr); 
  
//È¥³ý×Ö·û´®ÓÒ±ß¿Õ¸ñ
void Z_StrRTrim(char *pStr);
 
//È¥³ý×Ö·û´®×ó±ß¿Õ¸ñ
void Z_StrLTrim(char *pStr);

void Z_StrTrimHE(char *pStr);

void Z_StrETrim(char *pStr);
	
int Z_Hex2i(char s[]) ;
	
#endif
