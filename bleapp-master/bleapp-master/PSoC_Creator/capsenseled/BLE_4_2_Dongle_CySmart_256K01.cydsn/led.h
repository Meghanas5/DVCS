/*****************************************************************************
* File Name: led.h
*
* Version: 1.0
*
* Description:
* This file contains API for breathing LED effect
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
#ifndef _LED_H_
#define _LED_H_

#include "main.h"
#include "timer.h"

#define LED_CONTINOUS_BREATHING_EFFECT                      (0xFF)
#define LED_MAX_BRIGHTNESS                                  (DEVICE_LED_MAX_INTENSITY)

#ifndef DISABLE_LED
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
*****************************************************************************/
extern void Led_Init(void);

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
* Note:
* This function need to be called every millisecond
*****************************************************************************/
extern void Led_Update(void);

/*****************************************************************************
* Function Name: Led_IsComplete()
******************************************************************************
* Summary:
* This functions is checks the status of the LED effect
*
* Parameters:
* None
*
* Return:
* true if it is complete
*
* Note:
* If LedOn behaviour is set, it is application responsibility to stop it.
*
*****************************************************************************/
extern bool Led_IsComplete(void);

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
* Note:
* Application need to make sure that previous LED effect is complete before initiliating an other
* Led_Update function need to be called every millisecond
*****************************************************************************/
extern void Led_Blinking(uint16 riseTime, uint16 highTime, uint16 fallTime, uint16 lowTime, uint8 repeat);

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
* Note:
* Intensity should be set as less than the Period mention in the Prism_Led 
* component
*****************************************************************************/
extern void Led_On(uint32 intensity);

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
*****************************************************************************/
extern void Led_Stop(void);

#else
#define Led_Init()
#define Led_Update()
#define Led_Blinking(riseTime,highTime,fallTime,lowTime,repeat)
#define Led_IsComplete()               true
#define Led_On(intensity)
#define Led_Stop()
#endif /* DISABLE_LED */

#endif /* _LED_H_ */
