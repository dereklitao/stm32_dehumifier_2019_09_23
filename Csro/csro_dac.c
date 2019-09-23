#include "csro_common.h"
#include "dac.h"

void Csro_Dac_Init(void)
{
    HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 4095);
    HAL_DAC_Start(&hdac, DAC_CHANNEL_2);
    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, 4095);
}
