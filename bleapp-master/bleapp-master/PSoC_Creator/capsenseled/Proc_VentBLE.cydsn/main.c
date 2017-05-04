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

CYBLE_EVENT_T a;

void Stack_Handler( uint32 eventCode, void * eventParam)
{
    
    CYBLE_GATTS_WRITE_CMD_REQ_PARAM_T* wrReq;
    
    switch (eventCode)
    {
        case CYBLE_EVT_STACK_ON:
            CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
		
		    break;
        case CYBLE_EVT_TIMEOUT:
    		/* Timeout has occured */
            if(CYBLE_GAP_ADV_MODE_TO == *(uint8 *) eventParam)
            {
                /* Advertisement timeout has occured - stop advertisement */
                CyBle_GappStopAdvertisement();
            }
	        break;
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
            CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            LED_Scan_Write(1);
            break;
        
        case CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP:
		/* Restart Advertisement if the state is disconnected */
		if(CyBle_GetState() == CYBLE_STATE_DISCONNECTED )
		{
			CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
		}

        break;
        case CYBLE_EVT_GATT_CONNECT_IND:
            connectionHandle = *(CYBLE_CONN_HANDLE_T *)eventParam;
            LED_Scan_Write(0);
            break;
        case CYBLE_EVT_GATTS_WRITE_REQ:
            wrReq = (CYBLE_GATTS_WRITE_REQ_PARAM_T*)eventParam;
            if (wrReq->handleValPair.attrHandle == CYBLE_VENTSERVICE_PRESSURE_CHAR_HANDLE)
            {
                CyBle_GattsWriteAttributeValue (&wrReq->handleValPair, 0, &connectionHandle, CYBLE_GATT_DB_LOCALLY_INITIATED);
                LED_Conf_Write(wrReq->handleValPair.value.val[0]);
            }
            if (wrReq->handleValPair.attrHandle == CYBLE_VENTSERVICE_SERVO_CHAR_HANDLE)
            {
                switch (wrReq->handleValPair.value.val[0]) {
                        case (0):
                            PWM_Servo_WriteCompare(4315);
                            //Timer_WritePeriod(1205);
                            //Timer_Start();
                        break;
                        case (1):
                            PWM_Servo_WriteCompare(4350);
                            //Timer_WritePeriod(540);
                            //Timer_Start();
                        break;
                        case (2):
                            PWM_Servo_WriteCompare(4375);
                            //Timer_WritePeriod(225);
                            //Timer_Start();
                        case (3):
                            PWM_Servo_WriteCompare(4425);
                        break;
                        case (4):
                            PWM_Servo_WriteCompare(4450);
                        break;
                        case (5):
                            PWM_Servo_WriteCompare(4475);
                        break;
                    default:
                            
                        break;
                }
            }
            CyBle_GattsWriteRsp(connectionHandle);
            break;
        default:
            a = eventCode;
            break;
        
    }
}

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

    PWM_Servo_Start();
    CyBle_Start( Stack_Handler );
    LED_Conf_Write(0);
    LED_Scan_Write(1);
    for(;;)
    {
        CyBle_ProcessEvents();
    }
}

/* [] END OF FILE */
