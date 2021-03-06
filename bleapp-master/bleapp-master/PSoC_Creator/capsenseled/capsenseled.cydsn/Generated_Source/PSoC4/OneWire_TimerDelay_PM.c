/*******************************************************************************
* File Name: OneWire_TimerDelay_PM.c
* Version 2.70
*
*  Description:
*     This file provides the power management source code to API for the
*     Timer.
*
*   Note:
*     None
*
*******************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#include "OneWire_TimerDelay.h"

static OneWire_TimerDelay_backupStruct OneWire_TimerDelay_backup;


/*******************************************************************************
* Function Name: OneWire_TimerDelay_SaveConfig
********************************************************************************
*
* Summary:
*     Save the current user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  OneWire_TimerDelay_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void OneWire_TimerDelay_SaveConfig(void) 
{
    #if (!OneWire_TimerDelay_UsingFixedFunction)
        OneWire_TimerDelay_backup.TimerUdb = OneWire_TimerDelay_ReadCounter();
        OneWire_TimerDelay_backup.InterruptMaskValue = OneWire_TimerDelay_STATUS_MASK;
        #if (OneWire_TimerDelay_UsingHWCaptureCounter)
            OneWire_TimerDelay_backup.TimerCaptureCounter = OneWire_TimerDelay_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!OneWire_TimerDelay_UDB_CONTROL_REG_REMOVED)
            OneWire_TimerDelay_backup.TimerControlRegister = OneWire_TimerDelay_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: OneWire_TimerDelay_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  OneWire_TimerDelay_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void OneWire_TimerDelay_RestoreConfig(void) 
{   
    #if (!OneWire_TimerDelay_UsingFixedFunction)

        OneWire_TimerDelay_WriteCounter(OneWire_TimerDelay_backup.TimerUdb);
        OneWire_TimerDelay_STATUS_MASK =OneWire_TimerDelay_backup.InterruptMaskValue;
        #if (OneWire_TimerDelay_UsingHWCaptureCounter)
            OneWire_TimerDelay_SetCaptureCount(OneWire_TimerDelay_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!OneWire_TimerDelay_UDB_CONTROL_REG_REMOVED)
            OneWire_TimerDelay_WriteControlRegister(OneWire_TimerDelay_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: OneWire_TimerDelay_Sleep
********************************************************************************
*
* Summary:
*     Stop and Save the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  OneWire_TimerDelay_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void OneWire_TimerDelay_Sleep(void) 
{
    #if(!OneWire_TimerDelay_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(OneWire_TimerDelay_CTRL_ENABLE == (OneWire_TimerDelay_CONTROL & OneWire_TimerDelay_CTRL_ENABLE))
        {
            /* Timer is enabled */
            OneWire_TimerDelay_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            OneWire_TimerDelay_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    OneWire_TimerDelay_Stop();
    OneWire_TimerDelay_SaveConfig();
}


/*******************************************************************************
* Function Name: OneWire_TimerDelay_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  OneWire_TimerDelay_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void OneWire_TimerDelay_Wakeup(void) 
{
    OneWire_TimerDelay_RestoreConfig();
    #if(!OneWire_TimerDelay_UDB_CONTROL_REG_REMOVED)
        if(OneWire_TimerDelay_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                OneWire_TimerDelay_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */
