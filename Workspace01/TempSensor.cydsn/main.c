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

	OneWire_Start();
    
    UART_Start();
    
    int flag = 1;
    
    for(;;)
    {
        
        if (flag == 1)
        {
            flag = 0;
            LED_Write(1);
            OneWire_SendTemperatureRequest();
        }
        
        if (OneWire_DataReady)
        {
            OneWire_ReadTemperature();
            LED_Write(0);
            char* strMsg;
            strMsg = OneWire_GetTemperatureAsString(0);
            /*
            sprintf(strMsg,"%.2f\r\n", 
                    OneWire_GetTemperatureAsString(0)   
                    //25675 ticks
            );
            */
            UART_UartPutString(strMsg);
            UART_UartPutString("\r\n");
            flag = 1;
        }
        /* Place your application code here. */
    }
}

/* [] END OF FILE */
