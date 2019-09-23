#include "csro_common.h"
#include "gpio.h"

void Csro_Gpio_Read_Write_Pin(void)
{
    sys_regs.discs[0] = HAL_GPIO_ReadPin(SW1_GPIO_Port, SW1_Pin);
    sys_regs.discs[1] = HAL_GPIO_ReadPin(SW2_GPIO_Port, SW2_Pin);
    sys_regs.discs[2] = HAL_GPIO_ReadPin(SW3_GPIO_Port, SW3_Pin);
    sys_regs.discs[3] = HAL_GPIO_ReadPin(SW4_GPIO_Port, SW4_Pin);
    sys_regs.discs[4] = HAL_GPIO_ReadPin(SW5_GPIO_Port, SW5_Pin);
    sys_regs.discs[5] = HAL_GPIO_ReadPin(SW6_GPIO_Port, SW6_Pin);
    HAL_GPIO_WritePin(SR1_GPIO_Port, SR1_Pin, sys_regs.coils[0] == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(SR2_GPIO_Port, SR2_Pin, sys_regs.coils[1] == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(SR3_GPIO_Port, SR3_Pin, sys_regs.coils[2] == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(SR4_GPIO_Port, SR4_Pin, sys_regs.coils[3] == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(DR1_GPIO_Port, DR1_Pin, sys_regs.coils[4] == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(DR2_GPIO_Port, DR2_Pin, sys_regs.coils[5] == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(DR3_GPIO_Port, DR3_Pin, sys_regs.coils[6] == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(DR4_GPIO_Port, DR4_Pin, sys_regs.coils[7] == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
}
