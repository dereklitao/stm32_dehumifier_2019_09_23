#include "mb_config.h"

modbus_slave slave_hmi;
modbus_regs sys_regs;
static osSemaphoreId uart_msg_sem;

void slave_hmi_set_tx(void)
{
    HAL_GPIO_WritePin(slave_hmi.txrx_port, slave_hmi.txrx_pin_num, GPIO_PIN_SET);
}

void slave_hmi_set_rx(void)
{
    HAL_GPIO_WritePin(slave_hmi.txrx_port, slave_hmi.txrx_pin_num, GPIO_PIN_RESET);
}

void slave_hmi_uart_idle(void)
{
    if (((READ_REG(slave_hmi.uart->Instance->SR) & USART_SR_IDLE) != RESET) && ((READ_REG(slave_hmi.uart->Instance->CR1) & USART_CR1_IDLEIE) != RESET))
    {
        __HAL_UART_CLEAR_IDLEFLAG(slave_hmi.uart);
        slave_hmi.rx_len = MODBUS_BUFFER_LENGTH - slave_hmi.uart->hdmarx->Instance->CNDTR;
        osSemaphoreRelease(uart_msg_sem);
    }
}

void slave_hmi_reply(void)
{
    slave_hmi.buff_len = slave_hmi.tx_len;
    slave_crc16(&slave_hmi, slave_hmi.tx_buf);
    slave_hmi.tx_buf[slave_hmi.tx_len++] = slave_hmi.crc_hi;
    slave_hmi.tx_buf[slave_hmi.tx_len++] = slave_hmi.crc_lo;
    slave_hmi.slave_set_tx();
    HAL_UART_Transmit_DMA(slave_hmi.uart, slave_hmi.tx_buf, slave_hmi.tx_len);
}

void slave_hmi_write_coils_callback(void)
{
    if (slave_hmi.func_code == MODBUS_FC_WRITE_SINGLE_COIL)
    {
        if (slave_hmi.write_addr <= 7)
        {
            if (slave_hmi.rx_buf[4] == 0xFF && slave_hmi.rx_buf[5] == 0x00)
            {
                slave_hmi.regs->coils[slave_hmi.write_addr] = 1;
            }
            else if (slave_hmi.rx_buf[4] == 0x00 && slave_hmi.rx_buf[5] == 0x00)
            {
                slave_hmi.regs->coils[slave_hmi.write_addr] = 0;
            }
        }
    }
    else if (slave_hmi.func_code == MODBUS_FC_WRITE_MULTIPLE_COILS)
    {
        for (slave_hmi.i = 0; slave_hmi.i < slave_hmi.write_qty; slave_hmi.i++)
        {
            if ((slave_hmi.write_addr + slave_hmi.i) <= 7)
            {
                slave_hmi.regs->coils[slave_hmi.write_addr + slave_hmi.i] = (slave_hmi.rx_buf[7 + slave_hmi.i / 8] >> (slave_hmi.i % 8)) & 0x01;
            }
        }
    }
}

void slave_hmi_write_holdings_callback(void)
{
    uint8_t master_cps_need_write = 0;
    if (slave_hmi.func_code == MODBUS_FC_WRITE_SINGLE_REGISTER)
    {
        if (slave_hmi.write_addr <= 3)
        {
            uint16_t value = slave_hmi.rx_buf[4] * 256 + slave_hmi.rx_buf[5];
            if (value >= 300 && value <= 1000)
            {
                slave_hmi.regs->holdings[slave_hmi.write_addr] = value;
            }
        }
        else if (slave_hmi.write_addr == 4)
        {
            uint16_t value = slave_hmi.rx_buf[4] * 256 + slave_hmi.rx_buf[5];
            if (value >= 0 && value <= 500)
            {
                slave_hmi.regs->holdings[slave_hmi.write_addr] = value;
            }
        }
        else if ((slave_hmi.write_addr >= 0x1E) && (slave_hmi.write_addr <= 0x39))
        {
            slave_hmi.regs->holding_flags[slave_hmi.write_addr] = 1;
            slave_hmi.regs->holdings[slave_hmi.write_addr] = slave_hmi.rx_buf[4] * 256 + slave_hmi.rx_buf[5];
            master_cps_need_write = 1;
        }
    }
    else if (slave_hmi.func_code == MODBUS_FC_WRITE_MULTIPLE_REGISTERS)
    {
        for (slave_hmi.i = 0; slave_hmi.i < slave_hmi.write_qty; slave_hmi.i++)
        {
            if ((slave_hmi.write_addr + slave_hmi.i) <= 3)
            {
                uint16_t value = slave_hmi.rx_buf[7 + 2 * slave_hmi.i] * 256 + slave_hmi.rx_buf[8 + 2 * slave_hmi.i];
                if (value >= 300 && value <= 1000)
                {
                    slave_hmi.regs->holdings[slave_hmi.write_addr + slave_hmi.i] = value;
                }
            }
            else if ((slave_hmi.write_addr + slave_hmi.i) == 4)
            {
                uint16_t value = slave_hmi.rx_buf[7 + 2 * slave_hmi.i] * 256 + slave_hmi.rx_buf[8 + 2 * slave_hmi.i];
                if (value >= 0 && value <= 500)
                {
                    slave_hmi.regs->holdings[slave_hmi.write_addr + slave_hmi.i] = value;
                }
            }
            else if ((slave_hmi.write_addr + slave_hmi.i) >= 0x1E && (slave_hmi.write_addr + slave_hmi.i) <= 0x39)
            {
                slave_hmi.regs->holding_flags[slave_hmi.write_addr + slave_hmi.i] = 1;
                slave_hmi.regs->holdings[slave_hmi.write_addr + slave_hmi.i] = slave_hmi.rx_buf[7 + 2 * slave_hmi.i] * 256 + slave_hmi.rx_buf[8 + 2 * slave_hmi.i];
                master_cps_need_write = 1;
            }
        }
    }
    if (master_cps_need_write == 1)
    {
        master_cps.master_trigger_write();
    }
}

static void slave_hmi_init_regs(void)
{
    slave_hmi.regs->holdings[0] = 500;
    slave_hmi.regs->holdings[1] = 500;
    slave_hmi.regs->holdings[2] = 500;
    slave_hmi.regs->holdings[3] = 500;
    slave_hmi.regs->holdings[4] = 0;
}

void csro_slave_hmi_init(UART_HandleTypeDef *uart)
{

    osSemaphoreDef(uart_msg_semaphore);
    uart_msg_sem = osSemaphoreCreate(osSemaphore(uart_msg_semaphore), 1);
    slave_hmi.uart = uart;
    slave_hmi.txrx_port = UEn1_GPIO_Port;
    slave_hmi.txrx_pin_num = UEn1_Pin;
    slave_hmi.slave_id = 1;
    slave_hmi.regs = &sys_regs;
    slave_hmi.slave_set_tx = slave_hmi_set_tx;
    slave_hmi.slave_set_rx = slave_hmi_set_rx;
    slave_hmi.slave_uart_idle = slave_hmi_uart_idle;
    slave_hmi.slave_reply = slave_hmi_reply;
    slave_hmi.slave_write_coils_callback = slave_hmi_write_coils_callback;
    slave_hmi.slave_write_holdings_callback = slave_hmi_write_holdings_callback;
    slave_hmi_init_regs();

    __HAL_UART_ENABLE_IT(slave_hmi.uart, UART_IT_IDLE);
    slave_hmi.slave_set_rx();
    HAL_UART_Receive_DMA(slave_hmi.uart, slave_hmi.rx_buf, MODBUS_BUFFER_LENGTH);
}

void csro_slave_hmi_wait_cmd(void)
{
    if (osSemaphoreWait(uart_msg_sem, osWaitForever) == osOK)
    {
        HAL_UART_DMAStop(slave_hmi.uart);
        slave_handle_cmd(&slave_hmi);
        HAL_UART_Receive_DMA(slave_hmi.uart, slave_hmi.rx_buf, MODBUS_BUFFER_LENGTH);
    }
}
