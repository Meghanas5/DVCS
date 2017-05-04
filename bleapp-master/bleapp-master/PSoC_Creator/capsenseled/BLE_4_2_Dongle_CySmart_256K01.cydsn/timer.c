/*****************************************************************************
* File Name: timer.c
*
* Version: 1.1
*
* Description:
* This file contains timer API functions for the protocol. This can be used by the 
* application logic also
*
* Note:
* Defining DISABLE_TIMER in platform.h to disable timer module
*
* Owner: MRAO
*
* Related Document:
* System Reference Guide
*
* Hardware Dependency:
* Timer PSoC Creator Component
* System Reference Guide
*
* Code Tested With:
* 1. PSoC Creator 3.3
* 2. ARM GCC 4.9.3
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
#include "timer.h"

bool disableCallBack;

#ifndef DISABLE_TIMER
/* Variable to keep track of the time in millisecond */
static volatile uint32 tick;
/* Variable to increment the tick depending upon the timer configuration */
static uint8 tickIncrement;
/* Variable to store the timer callback function */
static TimerCBK timerCallBack = NULL;

/*****************************************************************************
* Function Name: Timer_CallBack()
******************************************************************************
* Summary:
* This function is a callback function called when timer interrupt fires
*
* Parameters:
* None
*
* Return:
* None
*
* Theory:
* Increments the tick timer. This function will call a second callback function if 
* the second callback was registered during initialization by the Timer_Init function 
* and if it the second callback has been enabled by the Timer_CallBack_Enable function. 
*
* Side Effects:
* tick variable value has changed
* 
* Note:
* None
*****************************************************************************/
static void Timer_CallBack(void)
{
#if defined(ENABLE_TIMER_COMPONENT)
    Timer_HW_ClearInterrupt(Timer_HW_INTR_MASK_TC);
#elif defined(ENABLE_WDT_TIMER)
    if(CySysWdtGetInterruptSource() | CY_SYS_WDT_COUNTER1_INT)
    {
        /* Clear WDT interrupt and system interrupt */
        CySysWdtClearInterrupt(CY_SYS_WDT_COUNTER1_INT);
    }
    else
    {
        /* Return for the callback function if the interrupt is from other counters */
        return;
    }
#else
    SysTick_Config(timerPeriod);
#endif
    tick+=tickIncrement;
    if(timerCallBack != NULL && !disableCallBack)
    {
        timerCallBack();
    }
}

/*****************************************************************************
* Function Name: Timer_Init()
******************************************************************************
* Summary:
* This function is used to initializes the timer module
*
* Parameters:
* timercbk cbk - Register the callback function when the timer fires
*
* Return:
* None
*
* Theory:
* Initialize the Timer_HW component if ENABLE_TIMER_COMPONENT macro is defined else 
* initialize watch dog timer 1 as the timer
*
* Side Effects:
* timerCallBack is updated
* 
* Note:
* Protocol Call this function. Application code is not expected to call this function
*****************************************************************************/
void Timer_Init(TimerCBK cbk)
{
    timerCallBack = cbk;
    Timer_CallBack_Disable();

#if defined(ENABLE_TIMER_COMPONENT)
    Timer_HW_Clock_Start();
    Timer_HW_Init();
    Timer_HW_Interrupt_StartEx(Timer_CallBack);
    Timer_HW_Start();
#elif defined(ENABLE_WDT_TIMER)
    CySysWdtUnlock();
    CyIntSetVector(WATCH_DOG_TIMER_INT, Timer_CallBack);
    CyIntEnable(WATCH_DOG_TIMER_INT);
    CySysWdtLock();
#endif /* ENABLE_TIMER_COMPONENT */
    tickIncrement = 1;
}

/*****************************************************************************
* Function Name: Timer_Get_Time_Stamp()
******************************************************************************
* Summary:
* This function is used to get the timestamp at that movement
*
* Parameters:
* None
*
* Return:
* uint32 - Time stamp at that movement
*
* Theory:
* Returns the tick value
*
* Side Effects:
* None
* 
* Note:
* None
*****************************************************************************/
uint32 Timer_Get_Time_Stamp(void)
{
    return tick;
}

/*****************************************************************************
* Function Name: Timer_Time_Elapsed()
******************************************************************************
* Summary:
* This function is used to check whether a certain time has elapsed or not 
*
* Parameters:
* None
*
* Return:
* None
*
* Theory:
* Checks whether a certain time has elapsed or not 
*
* Side Effects:
* None
* 
* Note:
* None
*****************************************************************************/
bool Timer_Time_Elapsed(uint32 time_stamp, uint32 interval)
{
    bool result = false;
    if((tick - time_stamp) >= interval)
    {
        result = true;
    }
    else if((time_stamp > tick) && ((time_stamp+interval) >= tick))
    {
        /* Checking the overflow condition here */
        result = true;
    }
    return result;
}

/*****************************************************************************
* Function Name: Timer_Set_Period()
******************************************************************************
* Summary:
* This function is used to set the period for the interrupt to be fired 
*
* Parameters:
* period - period to be set
*
* Return:
* None
*
* Theory:
* It calculates the tickIncrement depending upon the period 
*
* Side Effects:
* tickIncrement is updated
* 
* Note:
* period with value zero should not be passed to this API
*****************************************************************************/
void Timer_Set_Period(uint32 period)
{
#ifdef ENABLE_TIMER_COMPONENT
    Timer_HW_Interrupt_Disable();
    Timer_HW_Stop();
    tickIncrement = period/TIMER_CLOCK;
    Timer_HW_WriteCounter(0x0);
    Timer_HW_WritePeriod(period-1);
    Timer_HW_Interrupt_Enable();
    Timer_HW_Start();
#elif defined(ENABLE_WDT_TIMER)
    CySysWdtUnlock();
    CySysWdtDisable(CY_SYS_WDT_COUNTER1_MASK);
    CySysWdtResetCounters(CY_SYS_WDT_COUNTER1_RESET);
    tickIncrement = period/TIMER_CLOCK;
    CySysWdtWriteMatch(CY_SYS_WDT_COUNTER1, period);
    CySysWdtEnable(CY_SYS_WDT_COUNTER1_MASK);
    CySysWdtLock();
#endif /* ENABLE_TIMER_COMPONENT */
}

#endif /* DISABLE_TIMER */
