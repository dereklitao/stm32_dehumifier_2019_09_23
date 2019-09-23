#include "mb_config.h"

modbus_master master_cps;
static osSemaphoreId uart_idle_sem;
static osSemaphoreId write_cps_sem;
static osMutexId uart_source_mut;
static osTimerId write_cps_tim;

void master_cps_set_tx(void)
{
    HAL_GPIO_WritePin(UEn3_GPIO_Port, UEn3_Pin, GPIO_PIN_SET);
}

void master_cps_set_rx(void)
{
    HAL_GPIO_WritePin(UEn3_GPIO_Port, UEn3_Pin, GPIO_PIN_RESET);
}

void master_cps_uart_idle(void)
{
    if (((READ_REG(master_cps.uart->Instance->SR) & USART_SR_IDLE) != RESET) && ((READ_REG(master_cps.uart->Instance->CR1) & USART_CR1_IDLEIE) != RESET))
    {
        __HAL_UART_CLEAR_IDLEFLAG(master_cps.uart);
        master_cps.rx_len = MODBUS_BUFFER_LENGTH - master_cps.uart->hdmarx->Instance->CNDTR;
        osSemaphoreRelease(uart_idle_sem);
    }
}

uint8_t master_cps_send_receive(uint16_t timeout)
{
    master_cps.buff_len = master_cps.tx_len;
    master_crc16(&master_cps, master_cps.tx_buf);
    master_cps.tx_buf[master_cps.tx_len++] = master_cps.crc_hi;
    master_cps.tx_buf[master_cps.tx_len++] = master_cps.crc_lo;
    osSemaphoreWait(uart_idle_sem, 0);
    HAL_UART_Receive_DMA(master_cps.uart, master_cps.rx_buf, MODBUS_BUFFER_LENGTH);
    master_cps.master_set_tx();
    HAL_UART_Transmit_DMA(master_cps.uart, master_cps.tx_buf, master_cps.tx_len);
    if (osSemaphoreWait(uart_idle_sem, timeout) == osOK)
    {
        master_cps.status = 1;
        sys_regs.discs[0x0B] = 1;
    }
    else
    {
        master_cps.status = 0;
        sys_regs.discs[0x0B] = 0;
    }
    HAL_UART_DMAStop(master_cps.uart);
    return master_cps.status;
}

void master_write_cps_tim_callback(void const *argument)
{
    static uint16_t count = 0;
    sys_regs.holding_flags[0x51] = 1;
    sys_regs.holdings[0x51] = sys_regs.inputs[0x04];
    sys_regs.holdings[0x52] = sys_regs.inputs[0x03];
    sys_regs.holdings[0x53] = count++;
    osSemaphoreRelease(write_cps_sem);
}

void master_cps_trigger_write(void)
{
    osSemaphoreRelease(write_cps_sem);
}

void csro_master_cps_init(UART_HandleTypeDef *uart)
{
    osSemaphoreDef(uart_idle_semaphore);
    uart_idle_sem = osSemaphoreCreate(osSemaphore(uart_idle_semaphore), 1);

    osSemaphoreDef(write_cps_semaphore);
    write_cps_sem = osSemaphoreCreate(osSemaphore(write_cps_semaphore), 1);

    osMutexDef(uart_source_mutex);
    uart_source_mut = osMutexCreate(osMutex(uart_source_mutex));

    osTimerDef(write_cps_timer, master_write_cps_tim_callback);
    write_cps_tim = osTimerCreate(osTimer(write_cps_timer), osTimerPeriodic, NULL);

    osTimerStart(write_cps_tim, 2500);

    master_cps.uart = uart;
    master_cps.slave_id = 0x01;
    master_cps.master_set_tx = master_cps_set_tx;
    master_cps.master_set_rx = master_cps_set_rx;
    master_cps.master_uart_idle = master_cps_uart_idle;
    master_cps.master_send_receive = master_cps_send_receive;
    master_cps.master_trigger_write = master_cps_trigger_write;

    __HAL_UART_ENABLE_IT(master_cps.uart, UART_IT_IDLE);
}

void csro_master_cps_write_task(void)
{
    if (osSemaphoreWait(write_cps_sem, osWaitForever) == osOK)
    {
        if (osMutexWait(uart_source_mut, osWaitForever) == osOK)
        {
            for (uint8_t i = 0; i < 4; i++)
            {
                if (sys_regs.holding_flags[0x1E + i] != 0)
                {
                    master_cps.write_addr = 0x1E + i;
                    master_cps.write_qty = 1;
                    master_write_single_holding_reg(&master_cps, &sys_regs.holdings[0x1E + i]);
                    sys_regs.holding_flags[0x1E + i] = 0;
                }
            }
            for (uint8_t i = 0; i < 4; i++)
            {
                if (sys_regs.holding_flags[0x36 + i] != 0)
                {
                    master_cps.write_addr = 0x36 + i;
                    master_cps.write_qty = 1;
                    master_write_single_holding_reg(&master_cps, &sys_regs.holdings[0x36 + i]);
                    sys_regs.holding_flags[0x36 + i] = 0;
                }
            }
            if (sys_regs.holdings[0x51] != 0)
            {
                master_cps.write_addr = 0x51;
                master_cps.write_qty = 3;
                master_write_multi_holding_regs(&master_cps, &sys_regs.holdings[0x51]);
                sys_regs.holding_flags[0x51] = 0;
            }
            osMutexRelease(uart_source_mut);
        }
    }
}

void csro_master_cps_read_task(void)
{
    if (osMutexWait(uart_source_mut, osWaitForever) == osOK)
    {
        uint16_t result[35];
        master_cps.read_addr = 0x00;
        master_cps.read_qty = 30;
        if (master_read_holding_regs(&master_cps, result) == 1)
        {
            for (uint8_t i = 0; i < 30; i++)
            {
                sys_regs.inputs[16 + i] = result[i];
            }
        }
        osMutexRelease(uart_source_mut);
    }

    if (osMutexWait(uart_source_mut, osWaitForever) == osOK)
    {
        uint16_t result[5];
        master_cps.read_addr = 0x1E;
        master_cps.read_qty = 4;
        if (master_read_holding_regs(&master_cps, result) == 1)
        {
            for (uint8_t i = 0; i < 4; i++)
            {
                if (sys_regs.holding_flags[30 + i] == 0)
                {
                    sys_regs.holdings[30 + i] = result[i];
                }
            }
        }
        osMutexRelease(uart_source_mut);
    }

    if (osMutexWait(uart_source_mut, osWaitForever) == osOK)
    {
        uint16_t result[5];
        master_cps.read_addr = 0x36;
        master_cps.read_qty = 4;
        if (master_read_holding_regs(&master_cps, result) == 1)
        {
            for (uint8_t i = 0; i < 4; i++)
            {
                if (sys_regs.holding_flags[54 + i] == 0)
                {
                    sys_regs.holdings[54 + i] = result[i];
                }
            }
        }
        osMutexRelease(uart_source_mut);
    }
}
