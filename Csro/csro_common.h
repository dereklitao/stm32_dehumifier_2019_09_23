
#ifndef __CSRO_COMMON_H
#define __CSRO_COMMON_H

#include "stm32f1xx_hal.h"
#include "mb_config.h"

typedef enum
{
    AUTO = 0,
    COLD = 1,
    DEHUMI = 2,
    VENTI = 3,
    HOT = 4
} airsys_mode;

typedef enum
{
    FAN_AUTO = 0,
    FAN_LOW = 1,
    FAN_MID = 2,
    FAN_HIGH = 3,
    FAN_SUPER = 4
} fan_speed_mode;

float Csro_Calculate_ntc3950_Temperature_from_Resvalue(float res_value);
float Csro_Calculate_ntc3380_Temperature_from_Resvalue(float res_value);

void Csro_Control_init(void);
void Csro_Run_System_Manual(void);
void Csro_Run_System_Auto(void);

void Csro_Stepper_Set_Position(uint16_t target);

void Csro_Cps_Auto_Control(void);
void Csro_Fan_Auto_Control(void);
void Csro_NewAir_Auto_Control(void);
void Csro_Heater_Auto_Control(void);
void Csro_Valve_Auto_Control(void);

#endif
