/******************************************************************************
* File Name         : CySmt_BleEventHandler.h
* Description       : BLE component Event handler module.
*                     Prepares response data to be sent for each command and handles Async BLE events.
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

#ifndef _HOST_EMULATOR_H_
#define _HOST_EMULATOR_H_
#include "main.h"

extern CYBLE_GATT_DB_ATTR_HANDLE_T cys_charEndHandle;
extern uint8 cysBdHandle;
extern CYBLE_GAP_IOCAP_T CySIoCap;

/*******************************************************************************
* Function Name: CyS_GenericEventHandler
********************************************************************************
*
* Summary:
*  This function provides handling of events from the CYBLE Component and sending responses
*
* Parameters:  
*  uint8 event:       Event from the CYBLE component
*  void* eventParams: A structure instance for corresponding event type. The 
*                     list of event structure is described in the component data sheet.
*
* Return: 
*  None
*
*******************************************************************************/
extern void CyS_GenericEventHandler(uint32 event, void *eventParam);
   
#endif /* _HOST_EMULATOR_H_ */
/* [] END OF FILE */
