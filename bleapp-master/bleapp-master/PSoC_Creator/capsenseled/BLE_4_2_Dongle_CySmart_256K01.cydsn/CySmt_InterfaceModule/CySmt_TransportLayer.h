/******************************************************************************
* File Name         : CySmt_TransportLayer.h
* Description       : This file contains the modules to abstract serial interface communication for CySmart tool.
* Version           : 1.2
* Software Used     : PSoC Creator 3.3 CP2
* Compiler          : ARM GCC 4.9.3, ARM MDK Generic
*
********************************************************************************
* Copyright (2016), Cypress Semiconductor Corporation. All Rights Reserved.
********************************************************************************
* This software is owned by Cypress Semiconductor Corporation (Cypress)
* and is protected by and subject to worldwide patent protection (United
* States and foreign), United States copyright laws and international treaty
* provisions. Cypress hereby grants to licensee a personal, non-exclusive,
* non-transferable license to copy, use, modify, create derivative works of,
* and compile the Cypress Source Code and derivative works for the sole
* purpose of creating custom software in support of licensee product to be
* used only in conjunction with a Cypress integrated circuit as specified in
* the applicable agreement. Any reproduction, modification, translation,
* compilation, or representation of this software except as specified above 
* is prohibited without the express written permission of Cypress.
*
* Disclaimer: CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH 
* REGARD TO THIS MATERIAL, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* Cypress reserves the right to make changes without further notice to the 
* materials described herein. Cypress does not assume any liability arising out 
* of the application or use of any product or circuit described herein. Cypress 
* does not authorize its products for use as critical components in life-support 
* systems where a malfunction or failure may reasonably be expected to result in 
* significant injury to the user. The inclusion of Cypress' product in a life-
* support systems application implies that the manufacturer assumes all risk of 
* such use and in doing so indemnifies Cypress against all charges. 
*
* Use of this Software may be limited by and subject to the applicable Cypress
* software license agreement. 
*******************************************************************************/

#ifndef _CYSMT_TRANSPORT_LAYER_H_
#define _CYSMT_TRANSPORT_LAYER_H_

#include "main.h"
/* Size of command packet header */
#define COMMAND_HEADER_SIZE                  (0x04u)

/* Set maximum supported payload as per RAM availability */
/* Both UART RX & TX buffers are set as per limits of GATT/L2CAP */
#define MAX_PAYLOAD_SIZE                     (UART_UART_TX_BUFFER_SIZE)

/* Error code if RX command cant be handled */
#define CYS_FW_ERR_INSUFFICIENT_RESOURCES    (0xFD00u)

/* Primary command receive buffer size */
#define PRIMARY_CMD_BUFF_SIZE                (MAX_PAYLOAD_SIZE + COMMAND_HEADER_SIZE)

/* Secondary command payload maximum derived by CMD_PAIRING_PASSKEY_RESPONSE, all other commands are smaller */
#define SECONDARY_CMD_BUFF_SIZE              (8u + COMMAND_HEADER_SIZE)

/* State machine implementation for decoding received packet
        over UART.
        There are 3 states in this implementation. 
        1. Receive commands
        2. Receive packet length
        3. Receive payload */
typedef enum _CY_RX_CMD_STATE
{
    STATE_HEADER,
    STATE_COMMAND,
    STATE_LENGTH,
    STATE_PAYLOAD,     
}CY_RX_CMD_STATE;


#define ClearTXBuffer                        (UART_SpiUartClearTxBuffer)
#define TransmitAdditionalData               (UART_SpiUartPutArray)

/* ISR callback for UART RX */
CY_ISR_PROTO(Transport_RX_ISR);

/* ISR callback for 10ms timer used in RX state machine */
CY_ISR_PROTO(Transport_Timer_ISR);

/* Application buffer to cache primary command packet */
extern uint8 primaryCmdRxBuf[PRIMARY_CMD_BUFF_SIZE];

/* Application buffer to cache secondary command packet */
extern uint8 secondaryCmdRxBuf[SECONDARY_CMD_BUFF_SIZE];

/* Flag to indicate one complete command packet is detected */
extern volatile bool newCmdRxDoneFlag; 

#endif /* _CYSMT_TRANSPORT_LAYER_H_ */
/* [] END OF FILE */
