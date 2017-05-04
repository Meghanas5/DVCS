/*****************************************************************************
* File Name: led.c
*
* Version: 1.0
*
* Description:
* This file contains API for various LED effects
*
* Note:
*
*
* Owner: MRAO
*
* Related Document:
* TCPWM component datasheet
*
* Hardware Dependency:
* LED
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
#include "led.h"

#ifndef DISABLE_LED

/* Local variables for storing the different LED time variables */
static uint16 ledRiseTime = 0, ledHighTime = 0, ledFallTime = 0, ledLowTime = 0;
static uint8 ledRepeat = 0;
static uint32 ledTimeStamp = 0;
static uint16 riseCount = 0,fallCount = 0;
static uint32 compareValue = 0;
static bool isLedOnBehaviour = false;
/*****************************************************************************
* Function Name: Led_Update()
******************************************************************************
* Summary:
* This functions is a callback function to update the LED comparator values
*
* Parameters:
* None
*
* Return:
* None
*
* Theory:
* Updates the PrISM comparator values depending upon the pattern selected
*
* Side Effects:
* compareValue, ledTimeStamp is updated
* 
* Note:
* This function need to be called every millisecond
*****************************************************************************/
void Led_Update(void)
{
    if(!isLedOnBehaviour)
    {
        if(!Timer_Time_Elapsed(ledTimeStamp,ledRiseTime))
        {
            /*Increment compare value by rise count to get LED brightening effect*/
            compareValue += riseCount;
            Prism_Led_WriteCompare(compareValue);
        }
        else if(!Timer_Time_Elapsed(ledTimeStamp,ledRiseTime + ledHighTime))
        {
            /* Do Nothing */
            compareValue = compareValue;
        }
        else if(!Timer_Time_Elapsed(ledTimeStamp, ledRiseTime + ledHighTime + ledFallTime))
        {
            /*Decrement compare value to get LED dim effect*/
            if(compareValue > fallCount)
                compareValue -= fallCount;
            Prism_Led_WriteCompare(compareValue);
        }
        else if(!Timer_Time_Elapsed(ledTimeStamp, ledRiseTime + ledHighTime + ledFallTime + ledLowTime))
        {
            /*In LED low time make compare value 0*/
            compareValue = 0;
            Prism_Led_WriteCompare(compareValue);
        }
        else
        {
            /*If LED repeat is not set then stop the LED effect*/
            if(!ledRepeat)
            {
                Led_Stop();
            }
            else
            {
                /* if ledRepeat is set to 0xFF then LED Breathing will never stop */
                if(ledRepeat != LED_CONTINOUS_BREATHING_EFFECT)
                {
                    ledRepeat--;
                }
                ledTimeStamp = Timer_Get_Time_Stamp();
                compareValue = 0;
                Prism_Led_WriteCompare(compareValue);
            }
        }
    }
}

/*****************************************************************************
* Function Name: Led_Init()
******************************************************************************
* Summary:
* This functions is initializes the LED module
*
* Parameters:
* None
*
* Return:
* None
*
* Theory:
* Initializes PWM that is used controlling the LED behaviour
*
* Side Effects:
* None
* 
* Note:
* None
*****************************************************************************/
void Led_Init(void)
{
    Prism_Led_Init();
    Prism_Led_Clock_Stop();
}

/*****************************************************************************
* Function Name: Led_Blinking()
******************************************************************************
* Summary:
* This functions is used to create the pattern as mentioned below.
* <------REPEAT----------->
*     _________
*    /         \
*   /           \
*  /             \
* /               \____________
* <rT> <--ST--><-fT><---lT---->
*
* Parameters:
* riseTime - Time in millisecond for rise time
* highTime - Time in millisecond for high standby time
* fallTime - Time in millisecond for fall time
* lowTime - Time in millisecond for low standby time
* repeat - Repeating the above pattern
*
* Return:
* None
*
* Theory:
* Initializes the variable to create the pattern mentioned above
*
* Side Effects:
* ledRiseTime, ledHighTime, ledFallTime, ledLowTime, ledRepeat is updated
* 
* Note:
* Application need to make sure that previous LED effect is complete before initializing an other
* Led_Update function need to be called every millisecond
*****************************************************************************/
void Led_Blinking(uint16 riseTime, uint16 highTime, uint16 fallTime, uint16 lowTime, uint8 repeat)
{
    /*Check for LED completion*/
    if(Led_IsComplete())
    {
        ledRiseTime = riseTime;
        ledHighTime = highTime;
        ledFallTime = fallTime;
        ledLowTime = lowTime;
        ledRepeat = repeat;
        ledTimeStamp = Timer_Get_Time_Stamp();
        compareValue = 0;

        Prism_Led_WriteCompare(compareValue);
        /*Check if rise time update rise count*/
        if(riseTime)
        {
            riseCount = (uint16)((uint32)LED_MAX_BRIGHTNESS/(uint32)riseTime);
        }
        else
        {
            riseCount = (uint16)LED_MAX_BRIGHTNESS;
            Prism_Led_WriteCompare(LED_MAX_BRIGHTNESS);
        }

        /*If fall time update fall count*/
        if(fallTime)
        {
            fallCount = (uint16)((uint32)LED_MAX_BRIGHTNESS/(uint32)fallTime);
        }
        else
        {
            fallCount = (uint16)LED_MAX_BRIGHTNESS;
        }
        Prism_Led_Clock_Start();
        Prism_Led_Start();
    }
}

/*****************************************************************************
* Function Name: Led_On()
******************************************************************************
* Summary:
* This functions is a used to keep the LED at certain intensity
*
* Parameters:
* intensity - value of the intensity
*
* Return:
* None
*
* Theory:
* Turn on the PWM to provide a constant intensity
*
* Side Effects:
* isLedOnBehaviour is updated
* 
* Note:
* Intensity should be set as less than the Period mention in the Prism_Led 
* component
*****************************************************************************/
void Led_On(uint32 intensity)
{
    if(Led_IsComplete())
    {
        isLedOnBehaviour = true;
        Prism_Led_WriteCompare(intensity);
        Prism_Led_Clock_Start();
        Prism_Led_Start();
    }
}

/*****************************************************************************
* Function Name: Led_IsComplete()
******************************************************************************
* Summary:
* This functions is checks the status of the breathing LED effect
*
* Parameters:
* None
*
* Return:
* true if it is complete
*
* Theory:
* Checks the status of the breathing LED effect by checking the timestamp and 
* repeat value
*
* Side Effects:
* None
* 
* Note:
* If LedOn behaviour is set, it is application responsiblity to stop it.
*
*****************************************************************************/
bool Led_IsComplete(void)
{
    bool result = false;
    uint32 totalTime = ledRiseTime + ledHighTime + ledFallTime + ledLowTime;
    /*Check if not LED repeat functionality and LED ON behaviour then update the result 
    by calculating time elapsed*/
    if((ledRepeat == 0) && !isLedOnBehaviour)
    {
        if(totalTime > 0)
        {
            result = Timer_Time_Elapsed(ledTimeStamp, totalTime);
        }
        else
        {
            result = true;
        }
    }
    return result;
}

/*****************************************************************************
* Function Name: Led_Stop()
******************************************************************************
* Summary:
* This functions is stop of the LED module
*
* Parameters:
* None
*
* Return:
* None
*
* Theory:
* Stops the PWM and resets all the variables
*
* Side Effects:
* ledRiseTime, ledHighTime, ledFallTime, ledLowTime, ledRepeat and isLedOnBehaviour
* is updated
* 
* Note:
* None
*****************************************************************************/
void Led_Stop(void)
{
    Prism_Led_Stop();
    Prism_Led_Clock_Stop();
    ledRiseTime = 0;
    ledHighTime = 0;
    ledFallTime = 0;
    ledLowTime = 0;
    ledRepeat = 0;
    isLedOnBehaviour = false;
}

#endif /* DISABLE_LED */

