#include "csro_common.h"
#include "gpio.h"
#include "tim.h"
#include "adc.h"
#include "dac.h"

uint32_t adc_data[100];

void Csro_Control_init(void)
{
    HAL_ADCEx_Calibration_Start(&hadc1);
    HAL_ADC_Start_DMA(&hadc1, adc_data, 100);

    HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 4095);
    HAL_DAC_Start(&hdac, DAC_CHANNEL_2);
    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, 4095);

    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);

    sys_regs.holdings[HOLDING_UPOWER] = 0;
    sys_regs.holdings[HOLDING_UMODE] = AUTO;
    sys_regs.holdings[HOLDING_UTEMP] = 240;
    sys_regs.holdings[HOLDING_UFAN] = FAN_MID;

    sys_regs.holdings[HOLDING_CPS_CONTROL] = 0;
    sys_regs.holdings[HOLDING_CPS_MODE] = 0;
    sys_regs.holdings[HOLDING_CPS_ROOMFAN] = 2;
    sys_regs.holdings[HOLDING_CPS_COLD_TEMP] = 250;
    sys_regs.holdings[HOLDING_CPS_COLD_INTERVAL] = 20;
    sys_regs.holdings[HOLDING_CPS_HOT_TEMP] = 250;
    sys_regs.holdings[HOLDING_CPS_HOT_INTERVAL] = 20;
    sys_regs.holdings[HOLDING_AQIT] = 250;
    sys_regs.holdings[HOLDING_NTC2] = 180;
    sys_regs.holdings[HOLDING_CPS_ERROR_CODE] = 0;

    sys_regs.holdings[HOLDING_EC1] = 500;
    sys_regs.holdings[HOLDING_EC2] = 0;
    sys_regs.holdings[HOLDING_EC3] = 0;
    sys_regs.holdings[HOLDING_EC4] = 0;
}

static void csro_update_input(void)
{
    sys_regs.coils[COIL_SW1] = HAL_GPIO_ReadPin(SW1_GPIO_Port, SW1_Pin);
    sys_regs.coils[COIL_SW2] = HAL_GPIO_ReadPin(SW2_GPIO_Port, SW2_Pin);
    sys_regs.coils[COIL_SW3] = HAL_GPIO_ReadPin(SW3_GPIO_Port, SW3_Pin);
    sys_regs.coils[COIL_SW4] = HAL_GPIO_ReadPin(SW4_GPIO_Port, SW4_Pin);
    sys_regs.coils[COIL_SW5] = HAL_GPIO_ReadPin(SW5_GPIO_Port, SW5_Pin);
    sys_regs.coils[COIL_SW6] = HAL_GPIO_ReadPin(SW6_GPIO_Port, SW6_Pin);

    float sum[10] = {0};
    for (uint8_t i = 0; i < 10; i++)
    {
        for (uint8_t j = 0; j < 10; j++)
        {
            sum[j] += adc_data[i * 10 + j];
        }
    }

    float vadc = 1.225 * 4096.0 / (sum[9] / 10.0);
    float voltage[10] = {0};
    for (uint8_t i = 0; i < 10; i++)
    {
        voltage[i] = (sum[i] / 10.0) * vadc / 4096.0;
    }

    float ntc_res = voltage[3] * 10.0 / (vadc - voltage[3]);
    sys_regs.holdings[HOLDING_NTC1] = (int16_t)(Csro_Calculate_ntc3950_Temperature_from_Resvalue(ntc_res) * 10.0);
    ntc_res = voltage[2] * 10.0 / (vadc - voltage[2]);
    sys_regs.holdings[HOLDING_NTC2] = (int16_t)(Csro_Calculate_ntc3950_Temperature_from_Resvalue(ntc_res) * 10.0);
    ntc_res = voltage[1] * 10.0 / (vadc - voltage[1]);
    sys_regs.holdings[HOLDING_NTC3] = (int16_t)(Csro_Calculate_ntc3950_Temperature_from_Resvalue(ntc_res) * 10.0);
    ntc_res = voltage[0] * 10.0 / (vadc - voltage[0]);
    sys_regs.holdings[HOLDING_NTC4] = (int16_t)(Csro_Calculate_ntc3950_Temperature_from_Resvalue(ntc_res) * 10.0);

    float rhi_temp_res = voltage[5] * 10.0 / (vadc - voltage[5]);
    sys_regs.holdings[HOLDING_S1T] = (int16_t)(Csro_Calculate_ntc3380_Temperature_from_Resvalue(rhi_temp_res) * 10.0);
    sys_regs.holdings[HOLDING_S1H] = (int16_t)(voltage[4] / 3.3 * 100 * 10);
    rhi_temp_res = voltage[7] * 10.0 / (vadc - voltage[7]);
    sys_regs.holdings[HOLDING_S2T] = (int16_t)(Csro_Calculate_ntc3380_Temperature_from_Resvalue(rhi_temp_res) * 10.0);
    sys_regs.holdings[HOLDING_S2H] = (int16_t)(voltage[6] / 3.3 * 100 * 10);
}

static void csro_execute_output(void)
{
    HAL_GPIO_WritePin(SR1_GPIO_Port, SR1_Pin, sys_regs.coils[COIL_SRLY1] == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(SR2_GPIO_Port, SR2_Pin, sys_regs.coils[COIL_SRLY2] == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(SR3_GPIO_Port, SR3_Pin, sys_regs.coils[COIL_SRLY3] == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(SR4_GPIO_Port, SR4_Pin, sys_regs.coils[COIL_SRLY4] == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(DR1_GPIO_Port, DR1_Pin, sys_regs.coils[COIL_DRLY1] == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(DR2_GPIO_Port, DR2_Pin, sys_regs.coils[COIL_DRLY2] == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(DR3_GPIO_Port, DR3_Pin, sys_regs.coils[COIL_DRLY3] == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(DR4_GPIO_Port, DR4_Pin, sys_regs.coils[COIL_DRLY4] == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
    TIM2->CCR1 = sys_regs.holdings[HOLDING_EC4];
    TIM2->CCR2 = sys_regs.holdings[HOLDING_EC3];
    TIM2->CCR3 = sys_regs.holdings[HOLDING_EC2];
    TIM2->CCR4 = sys_regs.holdings[HOLDING_EC1];
}

void Csro_Auto_Run(void)
{
    //csro_update_input();
    Csro_Smart_Control();
    csro_execute_output();
}
