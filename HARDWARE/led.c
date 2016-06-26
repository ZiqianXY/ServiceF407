#include "led.h"
#include "sys.h"

// resources: GPIO_F9,F10
#define LedRed    PFout(9)
#define LedGreen  PFout(10)
#define LED_ON   0
#define LED_OUT  1

void LED_Init(){

    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF ,ENABLE);
    
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9|GPIO_Pin_10;
    GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;
    
    GPIO_Init(GPIOF,&GPIO_InitStructure);
    
    LedRed=LED_OUT;
    LedGreen=LED_OUT;
}

void LED_Blin_Syn(void){
    LedRed=!LedRed;
    LedGreen=LedRed;
}

void LED_Blin_Asy(void){
    LedGreen=LedRed;
    LedRed=!LedRed;
}

void LED_Blin_Green(void){
    LedGreen=!LedGreen;
}

void LED_Blin_Red(void){
    LedRed=!LedRed;
}

void LED_ON_Green(void){
    LedGreen = LED_ON;
}

void LED_OUT_Green(void){
    LedGreen = LED_OUT;
}
void LED_ON_Red(void){
    LedRed = LED_ON;
}

void LED_OUT_Red(void){
    LedRed = LED_OUT;
}

