/*******************************************************************************
* File Name: LED_Conf.c  
* Version 2.20
*
* Description:
*  This file contains APIs to set up the Pins component for low power modes.
*
* Note:
*
********************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "cytypes.h"
#include "LED_Conf.h"

static LED_Conf_BACKUP_STRUCT  LED_Conf_backup = {0u, 0u, 0u};


/*******************************************************************************
* Function Name: LED_Conf_Sleep
****************************************************************************//**
*
* \brief Stores the pin configuration and prepares the pin for entering chip 
*  deep-sleep/hibernate modes. This function must be called for SIO and USBIO
*  pins. It is not essential if using GPIO or GPIO_OVT pins.
*
* <b>Note</b> This function is available in PSoC 4 only.
*
* \return 
*  None 
*  
* \sideeffect
*  For SIO pins, this function configures the pin input threshold to CMOS and
*  drive level to Vddio. This is needed for SIO pins when in device 
*  deep-sleep/hibernate modes.
*
* \funcusage
*  \snippet LED_Conf_SUT.c usage_LED_Conf_Sleep_Wakeup
*******************************************************************************/
void LED_Conf_Sleep(void)
{
    #if defined(LED_Conf__PC)
        LED_Conf_backup.pcState = LED_Conf_PC;
    #else
        #if (CY_PSOC4_4200L)
            /* Save the regulator state and put the PHY into suspend mode */
            LED_Conf_backup.usbState = LED_Conf_CR1_REG;
            LED_Conf_USB_POWER_REG |= LED_Conf_USBIO_ENTER_SLEEP;
            LED_Conf_CR1_REG &= LED_Conf_USBIO_CR1_OFF;
        #endif
    #endif
    #if defined(CYIPBLOCK_m0s8ioss_VERSION) && defined(LED_Conf__SIO)
        LED_Conf_backup.sioState = LED_Conf_SIO_REG;
        /* SIO requires unregulated output buffer and single ended input buffer */
        LED_Conf_SIO_REG &= (uint32)(~LED_Conf_SIO_LPM_MASK);
    #endif  
}


/*******************************************************************************
* Function Name: LED_Conf_Wakeup
****************************************************************************//**
*
* \brief Restores the pin configuration that was saved during Pin_Sleep().
*
* For USBIO pins, the wakeup is only triggered for falling edge interrupts.
*
* <b>Note</b> This function is available in PSoC 4 only.
*
* \return 
*  None
*  
* \funcusage
*  Refer to LED_Conf_Sleep() for an example usage.
*******************************************************************************/
void LED_Conf_Wakeup(void)
{
    #if defined(LED_Conf__PC)
        LED_Conf_PC = LED_Conf_backup.pcState;
    #else
        #if (CY_PSOC4_4200L)
            /* Restore the regulator state and come out of suspend mode */
            LED_Conf_USB_POWER_REG &= LED_Conf_USBIO_EXIT_SLEEP_PH1;
            LED_Conf_CR1_REG = LED_Conf_backup.usbState;
            LED_Conf_USB_POWER_REG &= LED_Conf_USBIO_EXIT_SLEEP_PH2;
        #endif
    #endif
    #if defined(CYIPBLOCK_m0s8ioss_VERSION) && defined(LED_Conf__SIO)
        LED_Conf_SIO_REG = LED_Conf_backup.sioState;
    #endif
}


/* [] END OF FILE */
