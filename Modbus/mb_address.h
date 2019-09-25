#ifndef __MB_ADDRESS_H
#define __MB_ADDRESS_H

//Disc address
#define DISC_SW1 0
#define DISC_SW2 1
#define DISC_SW3 2
#define DISC_SW4 3
#define DISC_SW5 4
#define DISC_SW6 5
#define DISC_SRLY1 6
#define DISC_SRLY2 7
#define DISC_SRLY3 8
#define DISC_SRLY4 9
#define DISC_DRLY1 10
#define DISC_DRLY2 11
#define DISC_DRLY3 12
#define DISC_DRLY4 13
#define DISC_AQI_COM 14
#define DISC_CPS_COM 15
#define DISC_ERR_NTC1 16
#define DISC_ERR_NTC2 17
#define DISC_ERR_NTC3 18
#define DISC_ERR_NTC4 19
#define DISC_ERR_SEN1 20
#define DISC_ERR_SEN2 21

//Coil address
#define COIL_SRLY1 0
#define COIL_SRLY2 1
#define COIL_SRLY3 2
#define COIL_SRLY4 3
#define COIL_DRLY1 4
#define COIL_DRLY2 5
#define COIL_DRLY3 6
#define COIL_DRLY4 7

//Inpit address
#define INPUT_NTC1 0
#define INPUT_NTC2 1
#define INPUT_NTC3 2
#define INPUT_NTC4 3
#define INPUT_S1T 4
#define INPUT_S1H 5
#define INPUT_S2T 6
#define INPUT_S2H 7
#define INPUT_AQIT 8
#define INPUT_AQIH 9
#define INPUT_AQIP 10
#define INPUT_AQIC 11
#define INPUT_AQIV 12
#define INPUT_EC1 13
#define INPUT_EC1 14
#define INPUT_EC1 15
#define INPUT_EC1 16
#define INPUT_STEPPER 17
#define INPUT_FAN_REASON 18

#define INPUT_CPS_STATUS_START 20
#define INPUT_CPS_STATUS_LENGTH 30

//Holding address
#define HOLDING_UPOWER 0
#define HOLDING_UMODE 1
#define HOLDING_UTEMP 2
#define HOLDING_UFAN 3

#define HOLDING_EC1 10
#define HOLDING_EC2 11
#define HOLDING_EC3 12
#define HOLDING_EC4 13
#define HOLDING_CPS_CONTROL 14
#define HOLDING_CPS_MODE 15
#define HOLDING_CPS_ROOMFAN 16
#define HOLDING_CPS_COLD_TEMP 17
#define HOLDING_CPS_COLD_INTERVAL 18
#define HOLDING_CPS_HOT_TEMP 19
#define HOLDING_CPS_HOT_INTERVAL 20

#endif
