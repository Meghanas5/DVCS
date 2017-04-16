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
    
    int flag = 1;
    
    for(;;)
    {
        
        if (flag == 1)
        {
            flag = 0;
            LED_Write(1);
            CyDelay(3000);
            OneWire_SendTemperatureRequest();
        }
        
        if (OneWire_DataReady)
        {
            OneWire_ReadTemperature();
            LED_Write(0);
            char strMsg[80]={};
            sprintf(strMsg,"%.2f\r\n", 
                    OneWire_GetTemperatureAsFloat(0)   
                    //25675 ticks
            );
            CyDelay(3000);
            flag = 1;
        }
        /* Place your application code here. */
    }
}

/* [] END OF FILE */
