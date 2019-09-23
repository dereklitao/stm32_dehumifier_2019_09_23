#include "mb_config.h"

modbus_master master_aqi;
static osSemaphoreId uart_idle_sem;
static osMutexId uart_source_mut;

void master_aqi_set_tx(void)
{
    HAL_GPIO_WritePin(UEn4_GPIO_Port, UEn4_Pin, GPIO_PIN_SET);
}

void master_aqi_set_rx(void)
{
    HAL_GPIO_WritePin(UEn4_GPIO_Port, UEn4_Pin, GPIO_PIN_RESET);
}

void master_aqi_uart_idle(void)
{
    if (((READ_REG(master_aqi.uart->Instance->SR) & USART_SR_IDLE) != RESET) && ((READ_REG(master_aqi.uart->Instance->CR1) & USART_CR1_IDLEIE) != RESET))
    {
        __HAL_UART_CLEAR_IDLEFLAG(master_aqi.uart);
        master_aqi.rx_len = MODBUS_BUFFER_LENGTH - master_aqi.uart->hdmarx->Instance->CNDTR;
        osSemaphoreRelease(uart_idle_sem);
    }
}

uint8_t master_aqi_send_receive(uint16_t timeout)
{
    master_aqi.buff_len = master_aqi.tx_len;
    master_crc16(&master_aqi, master_aqi.tx_buf);
    master_aqi.tx_buf[master_aqi.tx_len++] = master_aqi.crc_hi;
    master_aqi.tx_buf[master_aqi.tx_len++] = master_aqi.crc_lo;
    osSemaphoreWait(uart_idle_sem, 0);
    HAL_UART_Receive_DMA(master_aqi.uart, master_aqi.rx_buf, MODBUS_BUFFER_LENGTH);
    master_aqi.master_set_tx();
    HAL_UART_Transmit_DMA(master_aqi.uart, master_aqi.tx_buf, master_aqi.tx_len);
    if (osSemaphoreWait(uart_idle_sem, timeout) == osOK)
    {
        master_aqi.status = 1;
        sys_regs.discs[0x0A] = 1;
    }
    else
    {
        master_aqi.status = 0;
        sys_regs.discs[0x0A] = 0;
    }
    HAL_UART_DMAStop(master_aqi.uart);
    return master_aqi.status;
}

void csro_master_aqi_init(UART_HandleTypeDef *uart)
{
    osSemaphoreDef(uart_idle_semaphore);
    uart_idle_sem = osSemaphoreCreate(osSemaphore(uart_idle_semaphore), 1);
    osMutexDef(uart_source_mutex);
    uart_source_mut = osMutexCreate(osMutex(uart_source_mutex));
    master_aqi.uart = uart;
    master_aqi.master_set_tx = master_aqi_set_tx;
    master_aqi.master_set_rx = master_aqi_set_rx;
    master_aqi.master_uart_idle = master_aqi_uart_idle;
    master_aqi.master_send_receive = master_aqi_send_receive;

    __HAL_UART_ENABLE_IT(master_aqi.uart, UART_IT_IDLE);
}

void csro_master_aqi_read_task(void)
{
    if (osMutexWait(uart_source_mut, osWaitForever) == osOK)
    {
        uint16_t result[10];
        master_aqi.slave_id = 0x10;
        master_aqi.read_addr = 0x10;
        master_aqi.read_qty = 8;
        if (master_read_holding_regs(&master_aqi, result) == 1)
        {
            sys_regs.inputs[8] = ((result[0] >> 8) * 10) + ((result[0] & 0x00FF) & 0x0F);
            sys_regs.inputs[9] = ((result[1] >> 8) * 10) + ((result[1] & 0x00FF) & 0x0F);
            sys_regs.inputs[10] = result[2];
            sys_regs.inputs[11] = result[4];
            sys_regs.inputs[12] = result[7];
        }
        osMutexRelease(uart_source_mut);
    }
}
