/*******************************************************************************
* File Name: LCD_Seg_1_Seg.h  
* Version 2.20
*
* Description:
*  This file contains the Alias definitions for Per-Pin APIs in cypins.h. 
*  Information on using these APIs can be found in the System Reference Guide.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_LCD_Seg_1_Seg_ALIASES_H) /* Pins LCD_Seg_1_Seg_ALIASES_H */
#define CY_PINS_LCD_Seg_1_Seg_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"


/***************************************
*              Constants        
***************************************/
#define LCD_Seg_1_Seg_0			(LCD_Seg_1_Seg__0__PC)
#define LCD_Seg_1_Seg_0_PS		(LCD_Seg_1_Seg__0__PS)
#define LCD_Seg_1_Seg_0_PC		(LCD_Seg_1_Seg__0__PC)
#define LCD_Seg_1_Seg_0_DR		(LCD_Seg_1_Seg__0__DR)
#define LCD_Seg_1_Seg_0_SHIFT	(LCD_Seg_1_Seg__0__SHIFT)
#define LCD_Seg_1_Seg_0_INTR	((uint16)((uint16)0x0003u << (LCD_Seg_1_Seg__0__SHIFT*2u)))

#define LCD_Seg_1_Seg_1			(LCD_Seg_1_Seg__1__PC)
#define LCD_Seg_1_Seg_1_PS		(LCD_Seg_1_Seg__1__PS)
#define LCD_Seg_1_Seg_1_PC		(LCD_Seg_1_Seg__1__PC)
#define LCD_Seg_1_Seg_1_DR		(LCD_Seg_1_Seg__1__DR)
#define LCD_Seg_1_Seg_1_SHIFT	(LCD_Seg_1_Seg__1__SHIFT)
#define LCD_Seg_1_Seg_1_INTR	((uint16)((uint16)0x0003u << (LCD_Seg_1_Seg__1__SHIFT*2u)))

#define LCD_Seg_1_Seg_2			(LCD_Seg_1_Seg__2__PC)
#define LCD_Seg_1_Seg_2_PS		(LCD_Seg_1_Seg__2__PS)
#define LCD_Seg_1_Seg_2_PC		(LCD_Seg_1_Seg__2__PC)
#define LCD_Seg_1_Seg_2_DR		(LCD_Seg_1_Seg__2__DR)
#define LCD_Seg_1_Seg_2_SHIFT	(LCD_Seg_1_Seg__2__SHIFT)
#define LCD_Seg_1_Seg_2_INTR	((uint16)((uint16)0x0003u << (LCD_Seg_1_Seg__2__SHIFT*2u)))

#define LCD_Seg_1_Seg_3			(LCD_Seg_1_Seg__3__PC)
#define LCD_Seg_1_Seg_3_PS		(LCD_Seg_1_Seg__3__PS)
#define LCD_Seg_1_Seg_3_PC		(LCD_Seg_1_Seg__3__PC)
#define LCD_Seg_1_Seg_3_DR		(LCD_Seg_1_Seg__3__DR)
#define LCD_Seg_1_Seg_3_SHIFT	(LCD_Seg_1_Seg__3__SHIFT)
#define LCD_Seg_1_Seg_3_INTR	((uint16)((uint16)0x0003u << (LCD_Seg_1_Seg__3__SHIFT*2u)))

#define LCD_Seg_1_Seg_4			(LCD_Seg_1_Seg__4__PC)
#define LCD_Seg_1_Seg_4_PS		(LCD_Seg_1_Seg__4__PS)
#define LCD_Seg_1_Seg_4_PC		(LCD_Seg_1_Seg__4__PC)
#define LCD_Seg_1_Seg_4_DR		(LCD_Seg_1_Seg__4__DR)
#define LCD_Seg_1_Seg_4_SHIFT	(LCD_Seg_1_Seg__4__SHIFT)
#define LCD_Seg_1_Seg_4_INTR	((uint16)((uint16)0x0003u << (LCD_Seg_1_Seg__4__SHIFT*2u)))

#define LCD_Seg_1_Seg_5			(LCD_Seg_1_Seg__5__PC)
#define LCD_Seg_1_Seg_5_PS		(LCD_Seg_1_Seg__5__PS)
#define LCD_Seg_1_Seg_5_PC		(LCD_Seg_1_Seg__5__PC)
#define LCD_Seg_1_Seg_5_DR		(LCD_Seg_1_Seg__5__DR)
#define LCD_Seg_1_Seg_5_SHIFT	(LCD_Seg_1_Seg__5__SHIFT)
#define LCD_Seg_1_Seg_5_INTR	((uint16)((uint16)0x0003u << (LCD_Seg_1_Seg__5__SHIFT*2u)))

#define LCD_Seg_1_Seg_6			(LCD_Seg_1_Seg__6__PC)
#define LCD_Seg_1_Seg_6_PS		(LCD_Seg_1_Seg__6__PS)
#define LCD_Seg_1_Seg_6_PC		(LCD_Seg_1_Seg__6__PC)
#define LCD_Seg_1_Seg_6_DR		(LCD_Seg_1_Seg__6__DR)
#define LCD_Seg_1_Seg_6_SHIFT	(LCD_Seg_1_Seg__6__SHIFT)
#define LCD_Seg_1_Seg_6_INTR	((uint16)((uint16)0x0003u << (LCD_Seg_1_Seg__6__SHIFT*2u)))

#define LCD_Seg_1_Seg_7			(LCD_Seg_1_Seg__7__PC)
#define LCD_Seg_1_Seg_7_PS		(LCD_Seg_1_Seg__7__PS)
#define LCD_Seg_1_Seg_7_PC		(LCD_Seg_1_Seg__7__PC)
#define LCD_Seg_1_Seg_7_DR		(LCD_Seg_1_Seg__7__DR)
#define LCD_Seg_1_Seg_7_SHIFT	(LCD_Seg_1_Seg__7__SHIFT)
#define LCD_Seg_1_Seg_7_INTR	((uint16)((uint16)0x0003u << (LCD_Seg_1_Seg__7__SHIFT*2u)))

#define LCD_Seg_1_Seg_INTR_ALL	 ((uint16)(LCD_Seg_1_Seg_0_INTR| LCD_Seg_1_Seg_1_INTR| LCD_Seg_1_Seg_2_INTR| LCD_Seg_1_Seg_3_INTR| LCD_Seg_1_Seg_4_INTR| LCD_Seg_1_Seg_5_INTR| LCD_Seg_1_Seg_6_INTR| LCD_Seg_1_Seg_7_INTR))


#endif /* End Pins LCD_Seg_1_Seg_ALIASES_H */


/* [] END OF FILE */
