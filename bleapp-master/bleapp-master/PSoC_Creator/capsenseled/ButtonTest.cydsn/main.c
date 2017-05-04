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

int led_state;


CY_ISR(Button_Left_Handler)
{
    if (led_state > 0)
    {
        led_state--;
    }    
    Button_Left_ClearInterrupt(); 
}

CY_ISR(Button_Right_Handler)
{
    if (led_state < 5)
    {
        led_state++;
    }
    Button_Right_ClearInterrupt();
}

void set_leds(led_state)
{
    LED_Green_1_Write(led_state >= 0);
    LED_Green_2_Write(led_state >= 1);
    LED_Yellow_1_Write(led_state >= 2);
    LED_Yellow_2_Write(led_state >= 3);
    LED_Red_1_Write(led_state >= 4);
    LED_Red_2_Write(led_state == 5);
}

int main(void)
{
    led_state = 0;
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    Button_Left_Int_StartEx(Button_Left_Handler);
    Button_Right_Int_StartEx(Button_Right_Handler);

    for(;;)
    {
        set_leds(led_state);
        /* Place your application code here. */
    }
}

/* [] END OF FILE */
