/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"
#include <stdio.h>

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    UART_Start();
    UART_UartPutString("Hello World!");
    char str[15];
    sprintf(str, "%d", !LED_Read());
    UART_UartPutString(str);
    LED_Write(!LED_Read());
    CyDelay(3000);
    
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

    for(;;)
    {
        /* Place your application code here. */
        uint32 key = UART_UartGetChar();
        if (key == 'a')
        {
            LED_Write(1);
            CyDelay(1000);
            UART_UartPutChar(key);
        }
            
        else
            LED_Write(0);
    }
}

/* [] END OF FILE */
