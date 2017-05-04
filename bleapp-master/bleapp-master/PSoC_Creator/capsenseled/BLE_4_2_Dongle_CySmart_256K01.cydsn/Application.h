/******************************************************************************
* Project Name      : BLE_Dongle_CySmart
* File Name         : Application.h
* Description       : This module handles state machine for custom client operation.
*                          Scan and connects to fixed CapSense client device, update the brightness as per slider position.
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

#ifndef _APPLICATION_H_
#define _APPLICATION_H_
#include "main.h"

/*****************************************************************************
* MACRO Definition
*****************************************************************************/    

/*****************************************************************************
* Data Type Definition
*****************************************************************************/


/*****************************************************************************
* Enumerated Data Definition
*****************************************************************************/


/*****************************************************************************
* Data Struct Definition
*****************************************************************************/


/*****************************************************************************
* Global Variable Declaration
*****************************************************************************/

/*****************************************************************************
* External Function Prototypes
*****************************************************************************/

/*******************************************************************************
* Function Name: BLE_Init
********************************************************************************
*
* Summary:
*  Initialize BLE component for CapSense client mode
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
extern void BLE_Init(void);

/*******************************************************************************
* Function Name: BLE_DeInit
********************************************************************************
*
* Summary:
* De-Initialize BLE component from CapSense client mode while entering other mode
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
extern void BLE_DeInit(void);

/*******************************************************************************
* Function Name: Dongle_Ble_State_Handler
********************************************************************************
*
* Summary:
*  Handles state machine for BLE CapSense Custom client
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
extern void Dongle_Ble_State_Handler(void);

/*****************************************************************************
* Function Name: Process_Suspend_Command()
******************************************************************************
* Summary:
* This function puts system into low power mode, on suspend command.
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
*****************************************************************************/
extern void Process_Suspend_Command(void);

/*******************************************************************************
* Function Name: Button_ISR
********************************************************************************
*
* Summary:
* ISR Callback function for user button pin, on interrupt this ISR will set button triggered flag
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
*  Modifies isButtonTriggered flag
*
*******************************************************************************/
CY_ISR_PROTO(Button_ISR);

#endif /* _APPLICATION_H_ */
/* [] END OF FILE */
