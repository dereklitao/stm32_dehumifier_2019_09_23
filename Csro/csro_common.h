
#ifndef __CSRO_COMMON_H
#define __CSRO_COMMON_H

#include "stm32f1xx_hal.h"
#include "mb_config.h"

float Csro_Calculate_ntc3950_Temperature_from_Resvalue(float res_value);
float Csro_Calculate_ntc3380_Temperature_from_Resvalue(float res_value);

#endif
