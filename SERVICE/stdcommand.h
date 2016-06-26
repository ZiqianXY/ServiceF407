#ifndef __STANDARD_COMMAND_H
#define __STANDARD_COMMAND_H

#include "sys.h"


/**************************************************
* define targetAddress & sourceAddress            *
***************************************************/

#define Req_11_Fun 		0x7DF   

#define Req_11_Phy_0  	0x7E0
#define Req_11_Phy_1 	0x7E1
#define Req_11_Phy_2 	0x7E2
#define Req_11_Phy_3  	0x7E3
#define Req_11_Phy_4  	0x7E4
#define Req_11_Phy_5  	0x7E5
#define Req_11_Phy_6  	0x7E6
#define Req_11_Phy_7  	0x7E7

#define Rsp_11_Phy_0 	0x7E8
#define Rsp_11_Phy_1 	0x7E9
#define Rsp_11_Phy_2 	0x7EA
#define Rsp_11_Phy_3 	0x7EB
#define Rsp_11_Phy_4 	0x7EC
#define Rsp_11_Phy_5 	0x7ED
#define Rsp_11_Phy_6 	0x7EE
#define Rsp_11_Phy_7 	0x7EF

#define Req_29_Fun 		0x18DB33F1
#define Req_29_Phy(x) 	0x18DA<<16+(x)<<8+F1
#define Rsp_29_Phy(x) 	0x18DA<<16+F1<<8+(x)
#define Rsp_29_phy      0x18DA33F1

/**************************************************
* define OBD mode / SID                           *
***************************************************/

#define SID_01	0x01
#define SID_02	0x02
#define SID_03	0x03
#define SID_04	0x04
#define SID_05	0x05
#define SID_06	0x06
#define SID_07	0x07
#define SID_08	0x08
#define SID_09	0x09


// define SID & PID
#define SID_PID_0100 	0x0100
#define SID_PID_0101 	0x0101
#define SID_PID_0102 	0x0102
#define SID_PID_0103 	0x0103
#define SID_PID_0104 	0x0104
#define SID_PID_0105 	0x0105

typedef struct {
    u8 SID;
    u8 PID;
    u8* description;
}OBD_CommandTypeDef;

typedef struct{
    u16 command;
    float minimum;
    float maximum;
    float precision;
    char* unit; 
    char* description;
}CmdStandardTypeDef;


static const CmdStandardTypeDef mCommandList[]={
    // define SID & PID  -->Engine
    {0x0104,    0.0,    100,    1.0,    "N.m",  "CMD_EngineLoad_0104"},
    {0x010C,    0.0,    100,    1.0,    "Rpm",  "CMD_EngineRPM_010C"},
    {0x011F,    0.0,    100,    1.0,    "min",  "CMD_EngineRuntime_011F"},
    {0x0110,    0.0,    100,    1.0,    "L",    "CMD_MassAirFlow_0110"},
    {0x0111,    0.0,    100,    1.0,    "mm",   "CMD_ThrottlePosition_0111"},
    // define SID & PID  -->Fuel
    {0x1051,    0.0,    100,    1.0,    "Type",  "CMD_FindFuelType_1051"},
    {0x015E,    0.0,    100,    1.0,    "Cons",  "CMD_FuelConsumption_015E"},
    {0x012F,    0.0,    100,    1.0,    "Level",  "CMD_FuelLevel_012F"},
    // define SID & PID  -->Pressure
    {0x0133,    0.0,    100,    1.0,    "Mpa",  "CMD_BarometricPressure_0x33"},
    {0x010A,    0.0,    100,    1.0,    "Mpa",  "CMD_FuelPressure_010A"},
    {0x010B,    0.0,    100,    1.0,    "Mpa",  "CMD_IntakeManifoldPressure_010B"},
    // define SID & PID  -->Temperature
    {0x010F,    0.0,    100,    1.0,    "C.",  "CMD_AirIntakeTemperature_010F"},
    {0x0146,    0.0,    100,    1.0,    "C.",  "CMD_AmbientAirTemperature_0146"},
    {0x0105,    0.0,    100,    1.0,    "C.",  "CMD_EngineCoolantTemperature_0105"},
};


#endif
