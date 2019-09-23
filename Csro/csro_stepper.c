#include "csro_common.h"

#define EXCITE_DURATION_MS 10;

typedef enum
{
    close = 0,
    open = 1,
} direction;

int16_t position = 0;
uint8_t pending_phase = 0;
uint8_t previous_phase = 0;

static void Excite_Current_Phase(direction dir)
{
    HAL_GPIO_WritePin(STEP1_GPIO_Port, STEP1_Pin, (pending_phase == 0 || pending_phase == 1 || pending_phase == 7) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(STEP2_GPIO_Port, STEP2_Pin, (pending_phase == 1 || pending_phase == 2 || pending_phase == 3) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(STEP3_GPIO_Port, STEP3_Pin, (pending_phase == 3 || pending_phase == 4 || pending_phase == 5) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(STEP4_GPIO_Port, STEP4_Pin, (pending_phase == 5 || pending_phase == 6 || pending_phase == 7) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    osDelay(EXCITE_DURATION_MS);

    HAL_GPIO_WritePin(STEP1_GPIO_Port, STEP1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(STEP2_GPIO_Port, STEP2_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(STEP3_GPIO_Port, STEP3_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(STEP4_GPIO_Port, STEP4_Pin, GPIO_PIN_RESET);

    previous_phase = pending_phase;
    if (dir)
}

static void One_Pulse(uint8_t dir)
{
    HAL_GPIO_WritePin(STEP1_GPIO_Port, STEP1_Pin, (pending_phase == 0 || pending_phase == 1 || pending_phase == 7) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(STEP2_GPIO_Port, STEP2_Pin, (pending_phase == 1 || pending_phase == 2 || pending_phase == 3) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(STEP3_GPIO_Port, STEP3_Pin, (pending_phase == 3 || pending_phase == 4 || pending_phase == 5) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(STEP4_GPIO_Port, STEP4_Pin, (pending_phase == 5 || pending_phase == 6 || pending_phase == 7) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    osDelay(EXCITE_DURATION_MS);
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

static void Prepare_Pulse(uint16_t holdms)
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
        One_Pulse(0);
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
                Prepare_Pulse(100);
                firstpulse = 0;
            }
            One_Pulse(1);
        }
    }
    else if (target < position)
    {
        while (target < position)
        {
            if (firstpulse == 1)
            {
                Prepare_Pulse(100);
                firstpulse = 0;
            }
            One_Pulse(0);
        }
    }
    firstpulse = 1;
}
