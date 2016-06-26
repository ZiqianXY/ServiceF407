#ifndef __TIMER__
#define __TIMER__
#include "sys.h"

void initTimer(void);

void startTimer(int ms);
    
void isTimerOver(void);

void TIMER3_Init(void);

void TIM7_Int_Init(u16 arr,u16 psc);

#endif

