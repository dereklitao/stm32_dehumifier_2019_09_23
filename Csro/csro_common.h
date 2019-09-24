
#ifndef __CSRO_COMMON_H
#define __CSRO_COMMON_H

#include "stm32f1xx_hal.h"
#include "mb_config.h"

float Csro_Calculate_ntc3950_Temperature_from_Resvalue(float res_value);
float Csro_Calculate_ntc3380_Temperature_from_Resvalue(float res_value);

void Csro_Adc_Init(void);
void Csro_Adc_Calculate_Value(void);

void Csro_Dac_Init(void);

void Csro_Gpio_Read_Write_Pin(void);

void Csro_Pwm_Init(void);
void Csro_Pwm_Set_Duty(void);

void Csro_Stepper_Set_Position(uint16_t target);

#endif
