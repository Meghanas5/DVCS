/*******************************************************************************
* File Name: LCD_Seg_1_PM.c
* Version 1.30
*
* Description:
*  This file provides the API source code for Power Management of the Segment
*  LCD component.
*
* Note:
*  None
*
********************************************************************************
* Copyright 2013-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "LCD_Seg_1.h"


/*******************************************************************************
* Function Name: LCD_Seg_1_SaveConfig
********************************************************************************
*
* Summary:
*  Saves user data. This API is called by LCD_Seg_1_Sleep() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void LCD_Seg_1_SaveConfig(void)
{
    /* Nothing to save for current version of the component */
}


/*******************************************************************************
* Function Name: LCD_Seg_1_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores user data. This API is called by LCD_Seg_1_Wakeup() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void LCD_Seg_1_RestoreConfig(void)
{
    /* Nothing to restore for current version of the component */
}


/*******************************************************************************
* Function Name: LCD_Seg_1_Sleep
********************************************************************************
*
* Summary:
*  Prepares the component to enter Sleep mode.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void LCD_Seg_1_Sleep(void)
{
    LCD_Seg_1_SaveConfig();
}


/*******************************************************************************
* Function Name: LCD_Seg_1_Wakeup
********************************************************************************
*
* Summary:
*  Restores component after exiting Sleep mode.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void LCD_Seg_1_Wakeup(void)
{
    LCD_Seg_1_RestoreConfig();
}


/* [] END OF FILE */
