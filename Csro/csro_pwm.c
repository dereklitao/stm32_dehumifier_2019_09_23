#include "csro_common.h"
#include "tim.h"

void Csro_Pwm_Init(void)
{
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
}

void Csro_Pwm_Set_Duty(void)
{
    TIM2->CCR1 = sys_regs.holdings[3];
    TIM2->CCR2 = sys_regs.holdings[2];
    TIM2->CCR3 = sys_regs.holdings[1];
    TIM2->CCR4 = sys_regs.holdings[0];
}