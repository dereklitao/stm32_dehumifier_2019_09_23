#include "csro_common.h"

void Csro_Cps_Auto_Control(void)
{
    if (sys_regs.holdings[HOLDING_UPOWER] == 0) // airsys power off
    {
        csro_cps.ctrl = 0;
    }
    else // airsys power on
    {
        if (sys_regs.holdings[HOLDING_UMODE] == AUTO) //auto
        {
            csro_cps.ctrl = 1;
            csro_cps.mode = AUTO;
        }
        else if (sys_regs.holdings[HOLDING_UMODE] == COLD) // cold
        {
            csro_cps.ctrl = 1;
            csro_cps.mode = COLD;
        }
        else if (sys_regs.holdings[HOLDING_UMODE] == DEHUMI) //dehumi
        {
            csro_cps.ctrl = 1;
            csro_cps.mode = DEHUMI;
        }
        else if (sys_regs.holdings[HOLDING_UMODE] == VENTI) //venti
        {
            csro_cps.ctrl = 0;
        }
        else if (sys_regs.holdings[HOLDING_UMODE] == HOT) //hot
        {
            csro_cps.ctrl = 1;
            csro_cps.mode = HOT;
        }
    }
    csro_cps.fan = (TIM2->CCR4 <= 238) ? 1 : (TIM2->CCR4 <= 576) ? 2 : 3;
    csro_cps.cold_temp = sys_regs.holdings[HOLDING_UTEMP];
    csro_cps.hot_temp = sys_regs.holdings[HOLDING_UTEMP];
    csro_cps.cold_interval = 20;
    csro_cps.hot_interval = 20;
    csro_cps.room_temp = sys_regs.inputs[INPUT_AQIT];
    csro_cps.pipe_temp = sys_regs.inputs[INPUT_NTC2];
    csro_cps.error_code = 0;
}

static fan_speed_mode fan_speed_temp(void)
{
    if (sys_regs.holdings[HOLDING_UMODE] == COLD)
    {
        float temp_diff = (sys_regs.inputs[INPUT_AQIT] - sys_regs.holdings[HOLDING_UTEMP]) / 10.0;
        return (temp_diff >= 8) ? FAN_SUPER : (temp_diff >= 5) ? FAN_HIGH : (temp_diff >= 2) ? FAN_MID : FAN_LOW;
    }
    else if (sys_regs.holdings[HOLDING_UMODE] == HOT)
    {
        float temp_diff = (sys_regs.holdings[HOLDING_UTEMP] - sys_regs.inputs[INPUT_AQIT]) / 10.0;
        return (temp_diff >= 8) ? FAN_SUPER : (temp_diff >= 5) ? FAN_HIGH : (temp_diff >= 2) ? FAN_MID : FAN_LOW;
    }
    return FAN_LOW;
}

static fan_speed_mode fan_speed_humi(void)
{
    float humi = sys_regs.inputs[INPUT_AQIH] / 10.0;
    return (humi >= 90) ? FAN_SUPER : (humi >= 75) ? FAN_HIGH : (humi >= 60) ? FAN_MID : FAN_LOW;
}

static fan_speed_mode fan_speed_pm(void)
{
    float pm = sys_regs.inputs[INPUT_AQIP];
    return (pm >= 150) ? FAN_SUPER : (pm >= 100) ? FAN_HIGH : (pm >= 50) ? FAN_MID : FAN_LOW;
}

static fan_speed_mode fan_speed_co2(void)
{
    float co2 = sys_regs.inputs[INPUT_AQIC];
    return (co2 >= 1800) ? FAN_SUPER : (co2 >= 1500) ? FAN_HIGH : (co2 >= 1200) ? FAN_MID : FAN_LOW;
}

void Csro_Fan_Auto_Control(void)
{
    if (sys_regs.holdings[HOLDING_UFAN] == AUTO)
    {
        fan_speed_mode speed = fan_speed_temp();
        fan_speed_mode humi_spd = fan_speed_humi();
        fan_speed_mode co2_spd = fan_speed_co2();
        fan_speed_mode pm_spd = fan_speed_pm();
        sys_regs.inputs[INPUT_FAN_REASON] = 1;

        if (humi_spd > speed)
        {
            speed = humi_spd;
            sys_regs.inputs[INPUT_FAN_REASON] = 2;
        }
        if (co2_spd > speed)
        {
            speed = co2_spd;
            sys_regs.inputs[INPUT_FAN_REASON] = 3;
        }
        if (pm_spd > speed)
        {
            speed = pm_spd;
            sys_regs.inputs[INPUT_FAN_REASON] = 4;
        }
    }
    else
    {
        TIM2->CCR4 = (sys_regs.holdings[HOLDING_UFAN] * 238) % 1000;
        sys_regs.inputs[INPUT_FAN_REASON] = 0;
    }
}

void Csro_NewAir_Auto_Control(void)
{
}

void Csro_Heater_Auto_Control(void)
{
}

void Csro_Valve_Auto_Control(void)
{
}
