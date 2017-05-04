/*****************************************************************************
* File Name: timer.h
*
* Version: 1.1
*
* Description:
* This file contains timer API functions for the protocol.This can be used by the 
* application logic also
*
* Note:
* Define DISABLE_TIMER in platform.h to disable timer module
* 
* Owner: MRAO
*
* Related Document:
* System Reference Guide
*
* Hardware Dependency:
* PRoC BLE Family of Devices
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
#ifndef _TIMER_H_
#define _TIMER_H_

#include "main.h"

/* Function prototype for the application to register */
typedef void(*TimerCBK)(void);

/* Variable that is used enabling and disabling the callback function */
extern bool disableCallBack;

/* Ensure corresponding system block is enabled (WDT/TCPWM block from the customizer) while enabling the timer module source */
#define ENABLE_WDT_TIMER // ENABLE_TIMER_COMPONENT //

#ifdef ENABLE_TIMER_COMPONENT
#define TIMER_CLOCK         32
#define Timer_Active()      Timer_Set_Period((TIMER_CLOCK*DEVICE_TIMER_TICK_ACTIVE_PERIOD))
#define Timer_Idle()        Timer_Set_Period((TIMER_CLOCK*DEVICE_TIMER_TICK_IDLE_PERIOD))
#define Timer_Sleep()       Timer_Set_Period((TIMER_CLOCK*DEVICE_TIMER_TICK_SLEEP_PERIOD))
#define Timer_Deep_Sleep()  Timer_Set_Period((TIMER_CLOCK*DEVICE_TIMER_TICK_DEEP_SLEEP_PERIOD))
#define Timer_Stop()        Timer_HW_Stop();Timer_HW_Clock_Stop()
#elif defined(ENABLE_WDT_TIMER)
#define TIMER_CLOCK         32
#define Timer_Active()      Timer_Set_Period(TIMER_CLOCK*DEVICE_TIMER_TICK_ACTIVE_PERIOD)
#define Timer_Idle()        Timer_Set_Period(TIMER_CLOCK*DEVICE_TIMER_TICK_IDLE_PERIOD)
#define Timer_Sleep()       Timer_Set_Period(TIMER_CLOCK*DEVICE_TIMER_TICK_SLEEP_PERIOD)
#define Timer_Deep_Sleep()  Timer_Set_Period(TIMER_CLOCK*DEVICE_TIMER_TICK_DEEP_SLEEP_PERIOD)
#define Timer_Stop()        CySysWdtUnlock(); CySysWdtDisable(CY_SYS_WDT_COUNTER1_MASK);CySysWdtLock();
#define TIMER_DEFAULT_VALUE (31)
#define WATCH_DOG_TIMER_INT (8)
#else
#error "ENABLE_TIMER_COMPONENT or ENABLE_WDT_TIMER should be defined"
#endif

/* Macros for enabling and disabling timer call back function */
#define Timer_CallBack_Disable()    (disableCallBack = true)
#define Timer_CallBack_Enable()     (disableCallBack = false)

#ifndef DISABLE_TIMER
/*****************************************************************************
* Function Name: Timer_Init()
******************************************************************************
* Summary:
* This functions is used to initializes the timer module
*
* Parameters:
* timercbk cbk - Register the call back function when the timer fires
*
* Return:
* None
*
*****************************************************************************/
extern void Timer_Init(TimerCBK cbk);

/*****************************************************************************
* Function Name: Timer_Get_Time_Stamp()
******************************************************************************
* Summary:
* This functions is used to get the timestamp at that movement
*
* Parameters:
* None
*
* Return:
* uint32 - Time stamp at that movement
*
*****************************************************************************/
extern uint32 Timer_Get_Time_Stamp(void);

/*****************************************************************************
* Function Name: Timer_Time_Elapsed()
******************************************************************************
* Summary:
* This functions is used to check whether a certain time has elapsed or not 
*
* Parameters:
* None
*
* Return:
* true - if current time is greater than the time_stamp+interval
* false - otherwise
*****************************************************************************/
extern bool Timer_Time_Elapsed(uint32 time_stamp, uint32 interval);

/*****************************************************************************
* Function Name: Timer_Set_Period()
******************************************************************************
* Summary:
* This functions is used to set the period for the interrupt to be fired 
*
* Parameters:
* period - period to be set
*
* Return:
* None
*
* Note:
* Period must be multiple of 32
*****************************************************************************/
extern void Timer_Set_Period(uint32 period);

#else
#define Timer_Init(cbk)
#define Timer_Get_Time_Stamp()                  (0)
#define Timer_Time_Elapsed(time_stamp,interval) (false)
#define Timer_Set_Period(period)
#endif /* DISABLE_TIMER */

#endif /* _TIMER_H_ */
