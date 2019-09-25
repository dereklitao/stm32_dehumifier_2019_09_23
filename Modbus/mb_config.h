#ifndef __MB_CONFIG_H
#define __MB_CONFIG_H

#include "stm32f1xx_hal.h"
#include "mb_address.h"
#include "cmsis_os.h"
#include "gpio.h"
#include "usart.h"

#define MODBUS_FC_READ_COILS 0x01
#define MODBUS_FC_READ_DISCRETE_INPUTS 0x02
#define MODBUS_FC_READ_HOLDING_REGISTERS 0x03
#define MODBUS_FC_READ_INPUT_REGISTERS 0x04
#define MODBUS_FC_WRITE_SINGLE_COIL 0x05
#define MODBUS_FC_WRITE_SINGLE_REGISTER 0x06
#define MODBUS_FC_READ_EXCEPTION_STATUS 0x07
#define MODBUS_FC_WRITE_MULTIPLE_COILS 0x0F
#define MODBUS_FC_WRITE_MULTIPLE_REGISTERS 0x10
#define MODBUS_FC_REPORT_SLAVE_ID 0x11
#define MODBUS_FC_MASK_WRITE_REGISTER 0x16
#define MODBUS_FC_WRITE_AND_READ_REGISTERS 0x17

#define MODBUS_BUFFER_LENGTH 512
#define MODBUS_TIMEOUT 800

#define MODBUS_DISC_MAX 256
#define MODBUS_COIL_MAX 256
#define MODBUS_INPUT_MAX 256
#define MODBUS_HOLDING_MAX 256

typedef struct
{
    uint8_t discs[MODBUS_DISC_MAX];
    uint8_t coils[MODBUS_COIL_MAX];
    uint8_t coil_flags[MODBUS_COIL_MAX];
    uint16_t inputs[MODBUS_INPUT_MAX];
    uint16_t holdings[MODBUS_HOLDING_MAX];
    uint8_t holding_flags[MODBUS_HOLDING_MAX];
} modbus_regs;

typedef struct
{
    UART_HandleTypeDef *uart;
    GPIO_TypeDef *txrx_port;
    uint16_t txrx_pin_num;

    uint8_t slave_id;
    uint8_t func_code;

    uint8_t crc_hi;
    uint8_t crc_lo;
    uint16_t buff_len;
    uint16_t index;

    uint16_t read_addr;
    uint16_t read_qty;
    uint16_t write_addr;
    uint16_t write_qty;
    uint16_t i;

    uint8_t rx_buf[MODBUS_BUFFER_LENGTH];
    uint16_t rx_len;
    uint8_t tx_buf[MODBUS_BUFFER_LENGTH];
    uint16_t tx_len;
    uint8_t status;

    uint8_t (*master_send_receive)(uint16_t timeout);
    void (*master_uart_idle)(void);
    void (*master_set_tx)(void);
    void (*master_set_rx)(void);
    void (*master_trigger_write)(void);
} modbus_master;

typedef struct
{
    UART_HandleTypeDef *uart;
    GPIO_TypeDef *txrx_port;
    uint16_t txrx_pin_num;

    uint8_t slave_id;
    uint8_t func_code;

    uint8_t crc_hi;
    uint8_t crc_lo;
    uint16_t buff_len;
    uint16_t index;

    uint16_t read_addr;
    uint16_t read_qty;
    uint16_t write_addr;
    uint16_t write_qty;

    uint16_t byte_count;
    uint8_t data;
    uint16_t i;

    uint8_t rx_buf[MODBUS_BUFFER_LENGTH];
    uint16_t rx_len;
    uint8_t tx_buf[MODBUS_BUFFER_LENGTH];
    uint16_t tx_len;

    modbus_regs *regs;
    void (*slave_uart_idle)(void);
    void (*slave_set_tx)(void);
    void (*slave_set_rx)(void);
    void (*slave_reply)(void);
    void (*slave_write_coils_callback)(void);
    void (*slave_write_holdings_callback)(void);
} modbus_slave;

typedef struct
{
    int16_t ctrl;
    int16_t mode;
    int16_t fan;
    int16_t cold_temp;
    int16_t cold_interval;
    int16_t hot_temp;
    int16_t hot_interval;
    int16_t room_temp;
    int16_t pipe_temp;
    int16_t error_code;
} luko_cps;

extern modbus_master master_aqi;
extern modbus_master master_cps;
extern modbus_slave slave_hmi;
extern modbus_regs sys_regs;
extern luko_cps csro_cps;

uint16_t master_crc16(modbus_master *master, uint8_t *buffer);
uint16_t slave_crc16(modbus_slave *slave, uint8_t *buffer);

uint8_t master_read_discs(modbus_master *master, uint8_t *result);
uint8_t master_read_coils(modbus_master *master, uint8_t *result);
uint8_t master_read_input_regs(modbus_master *master, int16_t *result);
uint8_t master_read_holding_regs(modbus_master *master, int16_t *result);
uint8_t master_write_single_coil(modbus_master *master, uint8_t *value);
uint8_t master_write_single_holding_reg(modbus_master *master, int16_t *value);
uint8_t master_write_multi_holding_regs(modbus_master *master, int16_t *value);

void slave_handle_cmd(modbus_slave *slave);

void csro_master_aqi_init(UART_HandleTypeDef *Uart);
void csro_master_aqi_read_task(void);

void csro_master_cps_init(UART_HandleTypeDef *Uart);
void csro_master_cps_read_task(void);
void csro_master_cps_write_task(void);
void csro_master_cps_read_write_task(void);

void csro_slave_hmi_init(UART_HandleTypeDef *uart);
void csro_slave_hmi_wait_cmd(void);

#endif
