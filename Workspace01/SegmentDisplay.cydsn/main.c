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

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    LCD_Seg_1_Start();
    LCD_Seg_1_ClearDisplay();
    

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    int i = 0;
    for(;;)
    {
        if (++i == 10)
            i = 0;
        LCD_Seg_1_Write7SegDigit_0(i, 0);
        CyDelay(1000);
        LCD_Seg_1_ClearDisplay();
        CyDelay(300);
        /* Place your application code here. */
    }
}

/* [] END OF FILE */
