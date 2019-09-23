#include "csro_common.h"
#include "adc.h"

uint32_t adc_data[100];

void Csro_Adc_Init(void)
{
    HAL_ADCEx_Calibration_Start(&hadc1);
    HAL_ADC_Start_DMA(&hadc1, adc_data, 100);
}

void Csro_Adc_Calculate_Value(void)
{
    float sum[10] = {0};
    float voltage[10] = {0};
    float vadc = 0;
    for (uint8_t i = 0; i < 10; i++)
    {
        for (uint8_t j = 0; j < 10; j++)
        {
            sum[j] += adc_data[i * 10 + j];
        }
    }
    vadc = 1.225 * 4096.0 / (sum[9] / 10.0);
    for (uint8_t i = 0; i < 10; i++)
    {
        voltage[i] = (sum[i] / 10.0) * vadc / 4096.0;
    }
    for (uint8_t i = 0; i < 4; i++)
    {
        float ntc_res = voltage[3 - i] * 10.0 / (vadc - voltage[3 - i]);
        sys_regs.inputs[i] = (int16_t)(Csro_Calculate_ntc3950_Temperature_from_Resvalue(ntc_res) * 10.0);
    }
    for (uint8_t i = 0; i < 2; i++)
    {
        float rhi_temp_res = voltage[i * 2 + 5] * 10.0 / (vadc - voltage[i * 2 + 5]);
        sys_regs.inputs[i * 2 + 4] = (int16_t)(Csro_Calculate_ntc3380_Temperature_from_Resvalue(rhi_temp_res) * 10.0);
        sys_regs.inputs[i * 2 + 5] = (int16_t)(voltage[i * 2 + 4] / 3.3 * 100 * 10);
    }
}