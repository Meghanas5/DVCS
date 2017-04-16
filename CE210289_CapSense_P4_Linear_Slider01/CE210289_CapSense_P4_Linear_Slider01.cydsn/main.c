/*****************************************************************************
* File Name: main.c
*
* Version: 1.10
*
* Description:  This example code demonstrates the PSoC4 CapSense Linear Slider
* operation.
*
* Related Document: Code example CE210289.pdf 
*
* Hardware Dependency: See code example CE210289
*
******************************************************************************
* Copyright (2016), Cypress Semiconductor Corporation.
******************************************************************************
* This software is owned by Cypress Semiconductor Corporation (Cypress) and is
* protected by and subject to worldwide patent protection (United States and
* foreign), United States copyright laws and international treaty provisions.
* Cypress hereby grants to licensee a personal, non-exclusive, non-transferable
* license to copy, use, modify, create derivative works of, and compile the
* Cypress Source Code and derivative works for the sole purpose of creating
* custom software in support of licensee product to be used only in conjunction
* with a Cypress integrated circuit as specified in the applicable agreement.
* Any reproduction, modification, translation, compilation, or representation of
* this software except as specified above is prohibited without the express
* written permission of Cypress.
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
* such use and in doing so indemnifies Cypress against all charges. Use may be
* limited by and subject to the applicable Cypress software license agreement.
*****************************************************************************/

#include "project.h"

/*******************************************************************************
* Function Name: main
********************************************************************************
*
* Summary:
*  main() performs the following functions:
*  1. Initializes CapSense and SCB blocks
*  2. Scans & processes slider sensors
*
*******************************************************************************/
int main ()
{    
    /* Enable Global interrupts for CapSense operation */
    CyGlobalIntEnable;
    
    /* Start EZI2C block */
    EZI2C_Start();
    
    /* Set up communication data buffer to CapSense data structure to be exposed to 
    I2C master at primary slave address request. */
    EZI2C_EzI2CSetBuffer1(sizeof(CapSense_dsRam), sizeof(CapSense_dsRam), 
                         (uint8 *)&CapSense_dsRam);
 
    /* Start CapSense block - Initializes CapSense Data structure and 
    performs first scan to set up sensor baselines */
    CapSense_Start();
    
    /* Scan all widgets */
    CapSense_ScanAllWidgets();
    
    for(;;)
    {
        /* Do this only when a scan is done */
        if(CapSense_NOT_BUSY == CapSense_IsBusy())
        {
              /* Process all widgets */
              CapSense_ProcessAllWidgets();
              /* Include Tuner */ 
              CapSense_RunTuner(); 
              /* Scan result verification */
              if (CapSense_IsAnyWidgetActive())
              {
                   /* Add any required functionality
                      based on scanning result*/
              }
              /* Start next scan */
              CapSense_ScanAllWidgets();
        }
    }
}


/* [] END OF FILE */
