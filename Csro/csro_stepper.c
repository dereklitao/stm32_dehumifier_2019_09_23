#include "csro_common.h"

int16_t position = 0;
uint8_t last_phase = 0;
uint8_t phase = 0;

static void one_pulse(uint8_t dir)
{
    if (phase == 0 || phase == 1 || phase == 7)
    {
        HAL_GPIO_WritePin(STEP1_GPIO_Port, STEP1_Pin, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(STEP1_GPIO_Port, STEP1_Pin, GPIO_PIN_RESET);
    }
    if (phase == 1 || phase == 2 || phase == 3)
    {
        HAL_GPIO_WritePin(STEP2_GPIO_Port, STEP2_Pin, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(STEP2_GPIO_Port, STEP2_Pin, GPIO_PIN_RESET);
    }

    if (phase == 3 || phase == 4 || phase == 5)
    {
        HAL_GPIO_WritePin(STEP3_GPIO_Port, STEP3_Pin, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(STEP3_GPIO_Port, STEP3_Pin, GPIO_PIN_RESET);
    }

    if (phase == 5 || phase == 6 || phase == 7)
    {
        HAL_GPIO_WritePin(STEP4_GPIO_Port, STEP4_Pin, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(STEP4_GPIO_Port, STEP4_Pin, GPIO_PIN_RESET);
    }
    osDelay(10);
    HAL_GPIO_WritePin(STEP1_GPIO_Port, STEP1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(STEP2_GPIO_Port, STEP2_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(STEP3_GPIO_Port, STEP3_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(STEP4_GPIO_Port, STEP4_Pin, GPIO_PIN_RESET);
    last_phase = phase;
    if (dir == 1)
    {
        phase = (phase + 7) % 8;
        position = position + 1;
    }
    else
    {
        phase = (phase + 1) % 8;
        position = position - 1;
        if (position <= 0)
        {
            position = 0;
        }
    }
    sys_regs.holdings[5] = position;
}

static void prepare_pulse(uint16_t holdms)
{
    if (last_phase == 0 || last_phase == 1 || last_phase == 7)
    {
        HAL_GPIO_WritePin(STEP1_GPIO_Port, STEP1_Pin, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(STEP1_GPIO_Port, STEP1_Pin, GPIO_PIN_RESET);
    }
    if (last_phase == 1 || last_phase == 2 || last_phase == 3)
    {
        HAL_GPIO_WritePin(STEP2_GPIO_Port, STEP2_Pin, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(STEP2_GPIO_Port, STEP2_Pin, GPIO_PIN_RESET);
    }

    if (last_phase == 3 || last_phase == 4 || last_phase == 5)
    {
        HAL_GPIO_WritePin(STEP3_GPIO_Port, STEP3_Pin, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(STEP3_GPIO_Port, STEP3_Pin, GPIO_PIN_RESET);
    }

    if (last_phase == 5 || last_phase == 6 || last_phase == 7)
    {
        HAL_GPIO_WritePin(STEP4_GPIO_Port, STEP4_Pin, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(STEP4_GPIO_Port, STEP4_Pin, GPIO_PIN_RESET);
    }
    osDelay(holdms);
}

void Csro_Stepper_Init(void)
{
    for (size_t i = 0; i < 600; i++)
    {
        one_pulse(0);
    }
    position = 0;
}

void Csro_Stepper_Set_Position(uint16_t target)
{
    static uint8_t firstpulse = 1;

    if (target > position)
    {
        while (target > position)
        {
            if (firstpulse == 1)
            {
                prepare_pulse(100);
                firstpulse = 0;
            }
            one_pulse(1);
        }
    }
    else if (target < position)
    {
        while (target < position)
        {
            if (firstpulse == 1)
            {
                prepare_pulse(100);
                firstpulse = 0;
            }
            one_pulse(0);
        }
    }
    firstpulse = 1;
}
