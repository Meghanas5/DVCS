/******************************************************************************
* File Name         : CySmt_TransportLayer.c
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

#include "CySmt_TransportLayer.h"
#ifdef CYSMART_SUPPORT

/* RX buffers for primary and secondary command data */
uint8 primaryCmdRxBuf[PRIMARY_CMD_BUFF_SIZE] = {0};
uint8 secondaryCmdRxBuf[SECONDARY_CMD_BUFF_SIZE] = {0};

/* This Flag indicates if any new command is detected */
volatile bool newCmdRxDoneFlag = false; 

/* State machine for command detection */
volatile static CY_RX_CMD_STATE cmdRxState = STATE_HEADER;

volatile static uint32 rxCnt = 0;

/*******************************************************************************
* Function Name: Transport_RX_ISR
********************************************************************************
*
* Summary:
*  Handles the Interrupt Service Routine for the UART RX.
*  Contains cmdRxState machine to check for valid command receipt and triggers flag
*
* Parameters:
*  NONE
*
* Return:
*  NONE
*
* Theory:
*  NONE
*
* Side Effects:
*  Command buffers and newCmdRxDoneFlag will be modified
*
* Note:
*
*******************************************************************************/
CY_ISR(Transport_RX_ISR)
{
    volatile static uint16 expectedPktLength;
    uint8 *currentPktBuf = primaryCmdRxBuf;
        
    if(UART_CHECK_INTR_RX_MASKED(UART_INTR_RX_NOT_EMPTY))
    {    
        /* Use secondary buffer if primary command is in progress */
        if(primaryCmdInProgress)
        {
            currentPktBuf = secondaryCmdRxBuf;
        }

        /* Check if secondary command has been detected and ignore the new bytes until secondary command is processed */
        if((currentPktBuf == secondaryCmdRxBuf) && newCmdRxDoneFlag)
        {
            UART_ClearRxInterruptSource(UART_INTR_RX_NOT_EMPTY);
            return;
        }

        while(0 != UART_SpiUartGetRxBufferSize())
        {
            /* Get each input byte into buffer, before checking for cmdRxState transition */
            currentPktBuf[rxCnt++] = (uint8)UART_SpiUartReadRxData();

            /* restart the timer on each received byte */
            Timer_10ms_Stop();
            Timer_10ms_Start();

            /* State machine implementation for decoding received packet
                    over UART.
                    There are 3 states in this implementation. 
                    1. Receive commands
                    2. Receive packet length
                    3. Receive payload */
            switch(cmdRxState) 
            {
                case STATE_HEADER: 
                    /* Header is 2 byte value, wait until it is received */
                    expectedPktLength = 0;
                    
                    if(sizeof(uint16) <= rxCnt)
                    {
                        if((0x43 == currentPktBuf[0]) && (0x59 == currentPktBuf[1]))
                        {
                            cmdRxState = STATE_COMMAND;
                        }
                        /* Reset the byte counter if header is not detected */
                        rxCnt = 0;
                    }
                    break;

                /* cmdRxState to receive command code of incoming data */
                case STATE_COMMAND: 
                    /* command code is 2 byte value, wait until it is received */
                    expectedPktLength = 0;

                    if(sizeof(uint16)== rxCnt)
                    {
                        cmdRxState = STATE_LENGTH;
                    }
                    break;

                /* cmdRxState to receive payload length */
                case STATE_LENGTH:
                    if(COMMAND_HEADER_SIZE == rxCnt)
                    {
                        expectedPktLength = CyBle_Get16ByPtr(&currentPktBuf[sizeof(uint16)]);
                        if(expectedPktLength > MAX_PAYLOAD_SIZE)
                        {
                            /* If payload size is big, discard remaining packet data */
                            rxCnt = 0;
                            cmdRxState = STATE_HEADER;
                            /* Set the flag, to send out error response */
                            newCmdRxDoneFlag = true;

                            /* Stop the timer as complete packet is received */
                            Timer_10ms_Stop();
                            break;
                        }
                        else if(0 != expectedPktLength)
                        {
                            cmdRxState = STATE_PAYLOAD;
                        }
                        else
                        {
                            rxCnt = 0;
                            cmdRxState = STATE_HEADER;
                            /* Done flag to indicate that the RX packet decoding is complete */
                            newCmdRxDoneFlag = true;

                            /* Stop the timer as complete packet is received */
                            Timer_10ms_Stop();
                            break;
                        }
                    }
                    else
                    {
                        /* wait until 2 byte length value is received */
                        expectedPktLength = 0;
                    }
                    break;

                /* cmdRxState to receive payload data */
                case STATE_PAYLOAD: 
                    /* Decrement the payload length on each byte reception */
                    if(0 != expectedPktLength)
                    {
                        expectedPktLength--;
                    }

                    /* Set complete flag, If last byte occurs */
                    if(0 == expectedPktLength)
                    {
                        rxCnt = 0;
                        cmdRxState = STATE_HEADER;
                        /* Done flag to indicate that the RX packet decoding is complete */
                        newCmdRxDoneFlag = true;

                        /* Stop the timer as complete packet is received */
                        Timer_10ms_Stop();
                    }
                    break;

                default:
                    /* Do Nothing, invalid cmdRxState */
                    break;    
            }
        }
        UART_ClearRxInterruptSource(UART_INTR_RX_NOT_EMPTY);
    }
    if(UART_CHECK_INTR_RX_MASKED(UART_INTR_RX_BREAK_DETECT | UART_INTR_RX_OVERFLOW))
    {    
        /* Break can occur on start of each command, reset cmdRxState machine and RX buffers */
        cmdRxState = STATE_HEADER;
        rxCnt = 0;
        UART_ClearRxInterruptSource(UART_INTR_RX_BREAK_DETECT);
        UART_ClearRxInterruptSource(UART_INTR_RX_OVERFLOW);
    }
}

/*******************************************************************************
* Function Name: Transport_Timer_ISR
********************************************************************************
*
* Summary:
*  Handles the Interrupt Service Routine for the UART Timer.
*  Resets RX command state machine for every 10ms delay on RX byte
*  If RX packet has started, there should not be more than 10ms delay between each RX byte with-in the packet
*
* Parameters:
*  NONE
*
* Return:
*  NONE
*
* Theory:
*  NONE
*
* Side Effects:
*  State machine and other parameters will be reset
*
* Note:
*
*******************************************************************************/
CY_ISR(Transport_Timer_ISR)
{
    uint32 intrSrc = Timer_10ms_GetInterruptSource();

    if(Timer_10ms_INTR_MASK_TC == intrSrc)
    {
        /* Reset RX state machine */
        cmdRxState = STATE_HEADER;
        rxCnt = 0;
        Timer_10ms_ClearInterrupt(intrSrc);
    }
    else
    {
        /* Ignore other interrupt sources */
    }

    isr_10ms_ClearPending();
}
#endif /* CYSMART_SUPPORT */

/* [] END OF FILE */
