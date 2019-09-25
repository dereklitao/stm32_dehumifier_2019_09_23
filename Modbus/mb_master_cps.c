#include "mb_config.h"

modbus_master master_cps;

static osSemaphoreId uart_idle_sem;
static osTimerId uart_idle_tim;

void master_cps_set_tx(void)
{
    HAL_GPIO_WritePin(master_cps.txrx_port, master_cps.txrx_pin_num, GPIO_PIN_SET);
}

void master_cps_set_rx(void)
{
    HAL_GPIO_WritePin(master_cps.txrx_port, master_cps.txrx_pin_num, GPIO_PIN_RESET);
}

void master_cps_uart_idle(void)
{
    if (((READ_REG(master_cps.uart->Instance->SR) & USART_SR_IDLE) != RESET) && ((READ_REG(master_cps.uart->Instance->CR1) & USART_CR1_IDLEIE) != RESET))
    {
        __HAL_UART_CLEAR_IDLEFLAG(master_cps.uart);
        master_cps.rx_len = MODBUS_BUFFER_LENGTH - master_cps.uart->hdmarx->Instance->CNDTR;
        osTimerStart(uart_idle_tim, 5);
    }
}

void master_cps_uart_idle_timeout_callback(void const *argument)
{
    if (master_cps.rx_len == (MODBUS_BUFFER_LENGTH - master_cps.uart->hdmarx->Instance->CNDTR))
    {
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
        sys_regs.coils[COIL_CPS_COM] = 1;
    }
    else
    {
        master_cps.status = 0;
        sys_regs.coils[COIL_CPS_COM] = 0;
    }
    HAL_UART_DMAStop(master_cps.uart);
    return master_cps.status;
}

void csro_master_cps_init(UART_HandleTypeDef *uart)
{
    osSemaphoreDef(uart_idle_semaphore);
    uart_idle_sem = osSemaphoreCreate(osSemaphore(uart_idle_semaphore), 1);

    osTimerDef(uart_idle_timer, master_cps_uart_idle_timeout_callback);
    uart_idle_tim = osTimerCreate(osTimer(uart_idle_timer), osTimerOnce, NULL);

    master_cps.uart = uart;
    master_cps.txrx_port = UEn3_GPIO_Port;
    master_cps.txrx_pin_num = UEn3_Pin;
    master_cps.slave_id = 0x01;
    master_cps.master_set_tx = master_cps_set_tx;
    master_cps.master_set_rx = master_cps_set_rx;
    master_cps.master_uart_idle = master_cps_uart_idle;
    master_cps.master_send_receive = master_cps_send_receive;

    __HAL_UART_ENABLE_IT(master_cps.uart, UART_IT_IDLE);
}

void csro_master_cps_read_write_task(void)
{
    int16_t result[50] = {0};
    master_cps.read_addr = 0x00;
    master_cps.read_qty = INPUT_CPS_STATUS_LENGTH;
    if (master_read_holding_regs(&master_cps, result) == 1)
    {
        for (uint8_t i = 0; i < INPUT_CPS_STATUS_LENGTH; i++)
        {
            sys_regs.inputs[INPUT_CPS_STATUS_START + i] = result[i];
        }
    }

    master_cps.write_addr = 0x1E;
    master_cps.write_qty = 4;
    int16_t value[5] = {0};
    value[0] = sys_regs.holdings[HOLDING_CPS_CONTROL];
    value[2] = sys_regs.holdings[HOLDING_CPS_MODE];
    value[3] = sys_regs.holdings[HOLDING_CPS_ROOMFAN];
    master_write_multi_holding_regs(&master_cps, value);

    master_cps.write_addr = 0x36;
    master_cps.write_qty = 4;
    value[0] = sys_regs.holdings[HOLDING_CPS_COLD_TEMP];
    value[1] = sys_regs.holdings[HOLDING_CPS_COLD_INTERVAL];
    value[2] = sys_regs.holdings[HOLDING_CPS_HOT_TEMP];
    value[3] = sys_regs.holdings[HOLDING_CPS_HOT_INTERVAL];
    master_write_multi_holding_regs(&master_cps, value);

    master_cps.write_addr = 0x51;
    master_cps.write_qty = 3;
    value[0] = sys_regs.holdings[HOLDING_AQIT];
    value[1] = sys_regs.holdings[HOLDING_NTC2];
    value[2] = sys_regs.holdings[HOLDING_CPS_ERROR_CODE];
    master_write_multi_holding_regs(&master_cps, value);
}
