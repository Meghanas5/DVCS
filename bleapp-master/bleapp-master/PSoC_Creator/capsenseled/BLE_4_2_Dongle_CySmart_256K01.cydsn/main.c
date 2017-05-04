/******************************************************************************
* Project Name      : BLE_Dongle_CySmart
* File Name         : main.c
* Version           : 1.2
* Software Used     : PSoC Creator 3.3 CP2
* Compiler          : ARM GCC 4.9.3, ARM MDK Generic
* Related Hardware  : CY5677 CySmart BLE 4.2 USB Dongle
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

/******************************************************************************
*                           THEORY OF OPERATION
*******************************************************************************
* This project supports CySmart application using Dongle and also it show cases the 
* capability of PRoC BLE as a Custom Client Device.
* This firmware has two modes of operation:
* 1) CapSense Custom Client mode: In this mode the Dongle will scan for devices with address of
*        CapSense_Slider_LED example project, using whitelist filter. Once successful connection is made,
*        it will enable notifications and keep updating the Dongle's LED brightness 
*        as per the slider value reported by CapSense_Slider_LED peripheral.

* 2) CySmart Emulator mode: In this mode Dongle acts as custom central device which will 
*    emulate the actions triggered via CySmart PC tool. 
*    Dongle and CySmart PC tool uses custom protocol over UART to communicate the data.

*******************************************************************************
* Hardware connection required: None.
* All the required connections are hardwired on the Dongle.
******************************************************************************/

#include "main.h"
#include "Application.h"

/*****************************************************************************
* Global Variable Declarations
*****************************************************************************/
volatile bool dongleSuspend = false;
#ifdef CYSMART_SUPPORT
void* heapBuffer = NULL;
#endif /* CYSMART_SUPPORT */

/*******************************************************************************
* Function Name: Device_Timer_Callback
********************************************************************************
*
* Summary:
* Callback function to be called in LED timer ISR
*
* Parameters:
* None
*
* Return:
* None
*
* Theory:
* None
*
* Side Effects:
* None
*
*******************************************************************************/
void Device_Timer_Callback(void)
{
    Led_Update();
    if(Led_IsComplete())
    {
        Timer_CallBack_Disable();
    }
}

/*******************************************************************************
* Function Name: Suspend_Cmd_ISR
********************************************************************************
*
* Summary:
* ISR Callback function for suspend indication pin, on interrupt this ISR will set the suspend flag
*
* Parameters:
* None
*
* Return:
* None
*
* Theory:
* None
*
* Side Effects:
* Modifies dongleSuspend flag
*
*******************************************************************************/
CY_ISR(Suspend_Cmd_ISR)
{
    /* Toggle USB suspend state - this interrupt occurs for both suspend and resume commands */
    dongleSuspend = !dongleSuspend;
    Pin_SuspendIndicator_ClearInterrupt();
}

/*******************************************************************************
* Function Name: Device_Init
********************************************************************************
*
* Summary:
* Initialize all the system modules
*
* Parameters:
* None
*
* Return:
* None
*
* Theory:
* None
*
* Side Effects:
* None
*
*******************************************************************************/
static void Device_Init(void)
{ 
#ifdef CYSMART_SUPPORT
    UART_Start();
    UART_SetCustomInterruptHandler(Transport_RX_ISR);
    isr_10ms_StartEx(Transport_Timer_ISR);
#endif /* CYSMART_SUPPORT */

    Isr_Suspend_StartEx(Suspend_Cmd_ISR);
    Isr_Button_StartEx(Button_ISR);
    Timer_Init(Device_Timer_Callback);
    Led_Init();
    CyGlobalIntEnable;

    BLE_Init();
}

/*******************************************************************************
* Function Name: main
********************************************************************************
*
* Summary:
* Handles monitoring of suspend, UART commands and BLE state machine
*
* Parameters:
* None
*
* Return:
*  int - this is main loop and never returns
*
* Theory:
* None
*
* Side Effects:
* None
*
*******************************************************************************/
int main()
{
    Device_Init();

    for(;;)
    {
        CyBle_ProcessEvents();

        /* Check If suspend command has occurred */
        if(dongleSuspend)
        {
            /*Set to sleep mode */
            Process_Suspend_Command();
        }
        else
        {
#ifdef CYSMART_SUPPORT
            /* Free-up the Write request buffer on primary command completion, if allocated */
            if((!primaryCmdInProgress) && (NULL != heapBuffer))
            {
                free(heapBuffer);
            }

            /* Start command processing, If complete command packet is received */
            if(newCmdRxDoneFlag)
            {
                CySmt_ProcessCommands();
            }
            /* Process application state machine, if tool is not connected */
            else if(!isCySmtConnected)
#endif /* CYSMART_SUPPORT */
            {
                Dongle_Ble_State_Handler();
            }

            /* Wait until UART transmit is completed and no command is in progress */
#ifdef CYSMART_SUPPORT
            if((0 == (UART_SpiUartGetTxBufferSize() + UART_GET_TX_FIFO_SR_VALID)) && (!primaryCmdInProgress))
#else
            if(0 == (UART_SpiUartGetTxBufferSize() + UART_GET_TX_FIFO_SR_VALID))
#endif /* CYSMART_SUPPORT */
            {
                /* Check and perform flash write if it is pending */
                CyBle_StoreBondingData(0);
            }
        }
    }
}

/* [] END OF FILE */
