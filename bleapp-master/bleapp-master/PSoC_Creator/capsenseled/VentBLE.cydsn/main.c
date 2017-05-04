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

CYBLE_CONN_HANDLE_T connectionHandle;

void Stack_Handler( uint32 eventCode, void * eventParam)
{
    
    CYBLE_GATTS_WRITE_CMD_REQ_PARAM_T* wrReq;
    
    switch (eventCode)
    {
        case CYBLE_EVT_STACK_ON:
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
            CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            LED_Scan_Write(1);
            break;
        case CYBLE_EVT_GATT_CONNECT_IND:
            connectionHandle = *(CYBLE_CONN_HANDLE_T *)eventParam;
            LED_Scan_Write(0);
            break;
        case CYBLE_EVT_GATTS_WRITE_REQ:
            wrReq = (CYBLE_GATTS_WRITE_REQ_PARAM_T*)eventParam;
            if (wrReq->handleValPair.attrHandle == CYBLE_VENTSERVICE_LED_CHAR_HANDLE)
            {
                CyBle_GattsWriteAttributeValue (&wrReq->handleValPair, 0, &connectionHandle, CYBLE_GATT_DB_LOCALLY_INITIATED);
                LED_Conf_Write(wrReq->handleValPair.value.val[0]);
            }
            
            CyBle_GattsWriteRsp(connectionHandle);
            break;
        
        
    }
}

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

    CyBle_Start( Stack_Handler );
    LED_Conf_Write(0);
    LED_Scan_Write(1);
    
    for(;;)
    {
        CyBle_ProcessEvents();
    }
}

/* [] END OF FILE */
