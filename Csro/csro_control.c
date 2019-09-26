#include "csro_common.h"

static fan_speed_mode fan_speed_temp(void)
{
    if (sys_regs.holdings[HOLDING_UMODE] == COLD)
    {
        float temp_diff = (sys_regs.holdings[HOLDING_AQIT] - sys_regs.holdings[HOLDING_UTEMP]) / 10.0;
        return (temp_diff >= 8) ? FAN_SUPER : (temp_diff >= 5) ? FAN_HIGH : (temp_diff >= 2) ? FAN_MID : FAN_LOW;
    }
    else if (sys_regs.holdings[HOLDING_UMODE] == HOT)
    {
        float temp_diff = (sys_regs.holdings[HOLDING_UTEMP] - sys_regs.holdings[HOLDING_AQIT]) / 10.0;
        return (temp_diff >= 8) ? FAN_SUPER : (temp_diff >= 5) ? FAN_HIGH : (temp_diff >= 2) ? FAN_MID : FAN_LOW;
    }
    return FAN_LOW;
}

static fan_speed_mode fan_speed_humi(void)
{
    if (sys_regs.holdings[HOLDING_UMODE] == DEHUMI)
    {
        float humi = sys_regs.holdings[HOLDING_AQIH] / 10.0;
        return (humi >= 90) ? FAN_SUPER : (humi >= 75) ? FAN_HIGH : (humi >= 60) ? FAN_MID : FAN_LOW;
    }
    return FAN_LOW;
}

static fan_speed_mode fan_speed_pm(void)
{
    float pm = sys_regs.holdings[HOLDING_AQIP];
    return (pm >= 150) ? FAN_SUPER : (pm >= 100) ? FAN_HIGH : (pm >= 50) ? FAN_MID : FAN_LOW;
}

static fan_speed_mode fan_speed_co2(void)
{
    float co2 = sys_regs.holdings[HOLDING_AQIC];
    return (co2 >= 1800) ? FAN_SUPER : (co2 >= 1500) ? FAN_HIGH : (co2 >= 1200) ? FAN_MID : FAN_LOW;
}

static void csro_cps_auto_control(void)
{
    if (sys_regs.holdings[HOLDING_UPOWER] == 0) // airsys power off
    {
        sys_regs.holdings[HOLDING_CPS_CONTROL] = 0;
    }
    else // airsys power on
    {
        if (sys_regs.holdings[HOLDING_UMODE] == AUTO) //auto
        {
            sys_regs.holdings[HOLDING_CPS_CONTROL] = 1;
            sys_regs.holdings[HOLDING_CPS_MODE] = AUTO;
        }
        else if (sys_regs.holdings[HOLDING_UMODE] == COLD) // cold
        {
            sys_regs.holdings[HOLDING_CPS_CONTROL] = 1;
            sys_regs.holdings[HOLDING_CPS_MODE] = COLD;
        }
        else if (sys_regs.holdings[HOLDING_UMODE] == DEHUMI) //dehumi
        {
            sys_regs.holdings[HOLDING_CPS_CONTROL] = 1;
            sys_regs.holdings[HOLDING_CPS_MODE] = DEHUMI;
        }
        else if (sys_regs.holdings[HOLDING_UMODE] == VENTI) //venti
        {
            sys_regs.holdings[HOLDING_CPS_CONTROL] = 0;
        }
        else if (sys_regs.holdings[HOLDING_UMODE] == HOT) //hot
        {
            sys_regs.holdings[HOLDING_CPS_CONTROL] = 1;
            sys_regs.holdings[HOLDING_CPS_MODE] = HOT;
        }
    }
    sys_regs.holdings[HOLDING_CPS_ROOMFAN] = (sys_regs.holdings[HOLDING_EC1] <= 238) ? 1 : (sys_regs.holdings[HOLDING_EC1] <= 576) ? 2 : 3;
    sys_regs.holdings[HOLDING_CPS_COLD_TEMP] = sys_regs.holdings[HOLDING_UTEMP];
    sys_regs.holdings[HOLDING_CPS_HOT_TEMP] = sys_regs.holdings[HOLDING_UTEMP];
}

void csro_fan_auto_control(void)
{
    if (sys_regs.holdings[HOLDING_UPOWER] == 0) // airsys power off
    {
        sys_regs.holdings[HOLDING_EC1] = 0;
        return;
    }
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
        sys_regs.holdings[HOLDING_EC1] = (238 * speed) % 1000;
    }
    else
    {
        sys_regs.holdings[HOLDING_EC1] = (sys_regs.holdings[HOLDING_UFAN] * 238) % 1000;
        sys_regs.inputs[INPUT_FAN_REASON] = 0;
    }
}

static void csro_newair_auto_control(void)
{
    if (sys_regs.holdings[HOLDING_UPOWER] == 0) // airsys power off
    {
        sys_regs.coils[COIL_DRLY1] = 0;
        sys_regs.holdings[HOLDING_EC2] = 0;
        sys_regs.holdings[HOLDING_EC3] = 0;
        return;
    }
    float pm = sys_regs.holdings[HOLDING_AQIP];
    float co2 = sys_regs.holdings[HOLDING_AQIC];
    uint8_t newair_pm = (pm >= 150) ? 4 : (pm >= 100) ? 3 : (pm >= 50) ? 2 : (pm >= 35) ? 1 : 0;
    uint8_t newair_co2 = (co2 >= 1800) ? 4 : (co2 >= 1500) ? 3 : (co2 >= 1200) ? 2 : (co2 >= 800) ? 1 : 0;
    if (newair_pm == 0 && newair_pm == 0) // full room air circulation
    {
        sys_regs.coils[COIL_DRLY1] = 0;
        sys_regs.holdings[HOLDING_EC2] = 0;
        sys_regs.holdings[HOLDING_EC3] = 0;
    }
    else
    {
        sys_regs.coils[COIL_DRLY1] = 1;
        sys_regs.holdings[HOLDING_EC2] = ((newair_co2 > newair_pm ? newair_co2 : newair_pm) * 238) % 1000;
        sys_regs.holdings[HOLDING_EC3] = (uint16_t)(0.8 * sys_regs.holdings[HOLDING_EC2]);
    }
}

static void csro_heater_auto_control(void)
{
    if (sys_regs.holdings[HOLDING_UPOWER] == 0) // airsys power off
    {
        sys_regs.coils[COIL_SRLY1] = 0;
        return;
    }
    float out_temp = sys_regs.holdings[HOLDING_S2T] / 10.0;

    if ((out_temp + 10.0) < 0)
    {
        sys_regs.coils[COIL_SRLY1] = 1;
    }
    else if (out_temp + 10.0 > 0.5)
    {
        sys_regs.coils[COIL_SRLY1] = 0;
    }
}

static void csro_valve_auto_control(void)
{
}

void Csro_Smart_Control(void)
{
    csro_cps_auto_control();
    csro_fan_auto_control();
    csro_newair_auto_control();
    csro_heater_auto_control();
}
