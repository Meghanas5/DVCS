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

#define TIME_INTERVAL_360 1325
#define TIME_INTERVAL_90  250

volatile int flag = 1;
volatile int x = 4520;


CY_ISR( Timer_Int_Handler) {
    PWM_WriteCompare(0);
    CyDelay(3000);
    PWM_WriteCompare(x);
    flag = 1;
    Timer_ClearInterrupt(Timer_INTR_MASK_TC);
}

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    //uint8 counter = 128;
    
    
    
    Timer_Int_StartEx( Timer_Int_Handler );
    PWM_Start();
    for(;;)
    {
       
        //printf("values = %d\n", values);
        //CyDelay(1000);
        if (flag) {
            /*
            if (x == 4530)
                x = 4350;
            else
                x = 4530;
            */
            Timer_WritePeriod(TIME_INTERVAL_360);
            Timer_Start();
            flag = 0;
        }
        //if (x>=4540){x=4320;}
        
        /* Place your application code here. */
    }
}

/* [] END OF FILE */
