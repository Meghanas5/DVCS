/******************************************************************************
* Project Name      : BLE_Dongle_CySmart
* File Name         : main.h
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

#ifndef _MAIN_H_
#define _MAIN_H_
#include <project.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "led.h"
#include "timer.h"

/* CySmart module enables custom serial protocol which will be used to communicate with CySmart PC Tool */
/* Note: CySmart module uses Heap section for multiple write operation,
        so make sure 1K of heap area is allocated */
#define CYSMART_SUPPORT
#ifdef CYSMART_SUPPORT
#include "CySmt_protocol.h"

/* Definitions as required for CySmart tool */

/* Below versions will be used to track the firmware updates and also will be used by CySmart for compatibility checks */
#define FW_MAJOR_VER            (0x0001u)
#define FW_MINOR_VER            (0x0002u)
#define FW_PATCH_VER            (0x0002u)
#define FW_BUILD_NUM            (0x0020u)

/* CY5677 Dongle using 4.2 BLE part */
#define DEVICE_ID               (0x00000002u)
#define HW_MAJOR_VER            (0x0002u)
#define PRODUCT_STRING          ("CySmart BLE 4.2 USB Dongle")

#define HW_MINOR_VER            (0x0000u)
#define HW_PATCH_VER            (0x0000u)
#define HW_BUILD_NUM            (0x0000u)

/* Below version numbers should always be same as versions of CySmart tool tested with FW */
#define MIN_SUPPORTED_MAJOR_VER (0x0001u)
#define MIN_SUPPORTED_MINOR_VER (0x0002u)
#define RECOMM_TOOL_MAJOR_VER   (0x0001u)
#define RECOMM_TOOL_MINOR_VER   (0x0002u)

#define MANUFACTURER_STRING     ("Cypress Semiconductor")

extern void* heapBuffer;

#endif /* CYSMART_SUPPORT */

/* Wrapper functions for LED module */

/* Slow Breathing LED Timing */
#define DEVICE_LED_SLOW_RISE_TIME            1248
#define DEVICE_LED_SLOW_HIGH_STANDBY_TIME    500
#define DEVICE_LED_SLOW_FALL_TIME            624
#define DEVICE_LED_SLOW_LOW_STANDY_TIME      1000

/* Fast Breathing LED Timing */
#define DEVICE_LED_FAST_RISE_TIME            156
#define DEVICE_LED_FAST_HIGH_STANDBY_TIME    25
#define DEVICE_LED_FAST_FALL_TIME            156
#define DEVICE_LED_FAST_LOW_STANDY_TIME      200

/* LED On Intensity */
#define DEVICE_LED_ON_INTENSITY              16000u
#define DEVICE_LED_MAX_INTENSITY             32000u

#define LOGIC_LOW                            0

#define Device_Led_Fast_Blinking()           Timer_CallBack_Enable();\
                                             Led_Blinking(DEVICE_LED_FAST_RISE_TIME, \
                                                         DEVICE_LED_FAST_HIGH_STANDBY_TIME, \
                                                         DEVICE_LED_FAST_FALL_TIME, \
                                                         DEVICE_LED_FAST_LOW_STANDY_TIME,\
                                                         LED_CONTINOUS_BREATHING_EFFECT)

#define Device_Led_Slow_Blinking()          Timer_CallBack_Enable();\
                                            Led_Blinking(DEVICE_LED_SLOW_RISE_TIME, \
                                                         DEVICE_LED_SLOW_HIGH_STANDBY_TIME, \
                                                         DEVICE_LED_SLOW_FALL_TIME, \
                                                         DEVICE_LED_SLOW_LOW_STANDY_TIME,\
                                                         LED_CONTINOUS_BREATHING_EFFECT)

/* LED On for time-out */
#define Device_Led_On_Timeout(milliSec)     Timer_CallBack_Enable();\
                                            Led_Blinking(1, (milliSec), 1, 1, 0)

#define Device_Led_On_Continous()           Led_On(DEVICE_LED_ON_INTENSITY)

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
extern volatile bool dongleSuspend;

/*****************************************************************************
* External Function Prototypes
*****************************************************************************/
extern void Device_Timer_Callback(void);
CY_ISR_PROTO(Suspend_Cmd_ISR);

#endif /* _MAIN_H_ */
/* [] END OF FILE */
