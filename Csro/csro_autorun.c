#include "csro_common.h"
#include "gpio.h"
#include "tim.h"

uint32_t adc_data[100];

static void read_switch_status(void)
{
    sys_regs.discs[DISC_SW1] = HAL_GPIO_ReadPin(SW1_GPIO_Port, SW1_Pin);
    sys_regs.discs[DISC_SW2] = HAL_GPIO_ReadPin(SW2_GPIO_Port, SW2_Pin);
    sys_regs.discs[DISC_SW3] = HAL_GPIO_ReadPin(SW3_GPIO_Port, SW3_Pin);
    sys_regs.discs[DISC_SW4] = HAL_GPIO_ReadPin(SW4_GPIO_Port, SW4_Pin);
    sys_regs.discs[DISC_SW5] = HAL_GPIO_ReadPin(SW5_GPIO_Port, SW5_Pin);
    sys_regs.discs[DISC_SW6] = HAL_GPIO_ReadPin(SW6_GPIO_Port, SW6_Pin);
}

static void calculate_adc_Value(void)
{
    float sum[10] = {0};
    float voltage[10] = {0};
    for (uint8_t i = 0; i < 10; i++)
    {
        for (uint8_t j = 0; j < 10; j++)
        {
            sum[j] += adc_data[i * 10 + j];
        }
    }
    float vadc = 1.225 * 4096.0 / (sum[9] / 10.0);
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
}

void Csro_Run_System_Manual(void)
{
    //read input status
    read_switch_status();
    calculate_adc_Value();
    //write relay according to coil regs
    HAL_GPIO_WritePin(SR1_GPIO_Port, SR1_Pin, sys_regs.coils[COIL_SRLY1] == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(SR2_GPIO_Port, SR2_Pin, sys_regs.coils[COIL_SRLY2] == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(SR3_GPIO_Port, SR3_Pin, sys_regs.coils[COIL_SRLY3] == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(SR4_GPIO_Port, SR4_Pin, sys_regs.coils[COIL_SRLY4] == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(DR1_GPIO_Port, DR1_Pin, sys_regs.coils[COIL_DRLY1] == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(DR2_GPIO_Port, DR2_Pin, sys_regs.coils[COIL_DRLY2] == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(DR3_GPIO_Port, DR3_Pin, sys_regs.coils[COIL_DRLY3] == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(DR4_GPIO_Port, DR4_Pin, sys_regs.coils[COIL_DRLY4] == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
    //write EC speed according to holding regs
    TIM2->CCR1 = sys_regs.holdings[HOLDING_EC4];
    TIM2->CCR2 = sys_regs.holdings[HOLDING_EC3];
    TIM2->CCR3 = sys_regs.holdings[HOLDING_EC2];
    TIM2->CCR4 = sys_regs.holdings[HOLDING_EC1];
    //write cps reg according to holding regs
    csro_cps.ctrl = sys_regs.holdings[HOLDING_CPS_CONTROL];
    csro_cps.mode = sys_regs.holdings[HOLDING_CPS_MODE];
    csro_cps.fan = sys_regs.holdings[HOLDING_CPS_ROOMFAN];
    csro_cps.cold_temp = sys_regs.holdings[HOLDING_CPS_COLD_TEMP];
    csro_cps.cold_interval = sys_regs.holdings[HOLDING_CPS_COLD_INTERVAL];
    csro_cps.hot_temp = sys_regs.holdings[HOLDING_CPS_HOT_TEMP];
    csro_cps.hot_interval = sys_regs.holdings[HOLDING_CPS_HOT_INTERVAL];
    csro_cps.room_temp = sys_regs.inputs[INPUT_AQIT];
    csro_cps.pipe_temp = sys_regs.inputs[INPUT_NTC2];
    csro_cps.error_code = 0;
}

void Csro_Run_System_Auto(void)
{
}
