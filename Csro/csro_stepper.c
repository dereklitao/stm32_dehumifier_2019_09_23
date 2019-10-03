#include "csro_common.h"

#define PREPARE_DURATION_MS 100
#define EXCITE_DURATION_MS 10

uint16_t position = 600;
uint16_t phase = 0;

static void Excite_Phase(uint16_t duration)
{
    HAL_GPIO_WritePin(STEP1_GPIO_Port, STEP1_Pin, (phase == 0 || phase == 1 || phase == 7) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(STEP2_GPIO_Port, STEP2_Pin, (phase == 1 || phase == 2 || phase == 3) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(STEP3_GPIO_Port, STEP3_Pin, (phase == 3 || phase == 4 || phase == 5) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(STEP4_GPIO_Port, STEP4_Pin, (phase == 5 || phase == 6 || phase == 7) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    osDelay(duration);
    HAL_GPIO_WritePin(STEP1_GPIO_Port, STEP1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(STEP2_GPIO_Port, STEP2_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(STEP3_GPIO_Port, STEP3_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(STEP4_GPIO_Port, STEP4_Pin, GPIO_PIN_RESET);
}

void Csro_Stepper_Set_Position(uint16_t target)
{
    static uint8_t init_flag = 1;

    if (target > position)
    {
        while (target > position)
        {
            if (init_flag == 1)
            {
                Excite_Phase(PREPARE_DURATION_MS);
                init_flag = 0;
            }
            phase = (phase + 7) % 8;
            Excite_Phase(EXCITE_DURATION_MS);
            position = position + 1;
            sys_regs.inputs[INPUT_STEPPER] = position;
        }
    }
    else if (target < position)
    {
        while (target < position)
        {
            if (init_flag == 1)
            {
                Excite_Phase(PREPARE_DURATION_MS);
                init_flag = 0;
            }
            phase = (phase + 1) % 8;
            Excite_Phase(EXCITE_DURATION_MS);
            position = position - 1;
            sys_regs.inputs[INPUT_STEPPER] = position;
        }
    }
    init_flag = 1;
}
