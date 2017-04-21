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

#define NEUTRAL 4450
#define CW90 4250
#define CCW90 4650

volatile int flag = 1;
volatile int x = NEUTRAL;


CY_ISR( Timer_Int_Handler) {
    CyDelay(1000);
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
            if (x == NEUTRAL)
                x = CW90;
            else if (x == CW90)
                x = 4650;
            else
                x = NEUTRAL;
            Timer_WritePeriod(TIME_INTERVAL_360);
            Timer_Start();
            flag = 0;
        }
        //if (x>=4540){x=4320;}
        
        /* Place your application code here. */
    }
}

/* [] END OF FILE */
