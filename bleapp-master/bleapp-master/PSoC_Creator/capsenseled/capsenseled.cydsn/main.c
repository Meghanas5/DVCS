#include <project.h>
#include <stdio.h>

uint16 fingerPos    = 0xFFFF;
uint16 fingerPosOld = 0xFFFF;

int capsenseNotify, tempNotify;

volatile uint16 Temp, TempOld;

int flag;

/***************************************************************
 * Function to update the Servo state in the GATT database
 **************************************************************/
void updateServo()
{
    CYBLE_GATTS_HANDLE_VALUE_NTF_T 	tempHandle;
   
    
    if(CyBle_GetState() != CYBLE_STATE_CONNECTED)
        return;
    
    tempHandle.attrHandle = CYBLE_LEDCAPSENSE_LED_CHAR_HANDLE;
    tempHandle.value.len = 1;
    CyBle_GattsWriteAttributeValue(&tempHandle,0,&cyBle_connHandle,CYBLE_GATT_DB_LOCALLY_INITIATED);  
}

/***************************************************************
 * Function to update the LED state in the GATT database
 **************************************************************/
void updateLed()
{
    CYBLE_GATTS_HANDLE_VALUE_NTF_T 	tempHandle;
   
    uint8 red_State = !red_Read();
    
    if(CyBle_GetState() != CYBLE_STATE_CONNECTED)
        return;
    
    tempHandle.attrHandle = CYBLE_LEDCAPSENSE_LED_CHAR_HANDLE;
  	tempHandle.value.val = (uint8 *) &red_State;
    tempHandle.value.len = 1;
    CyBle_GattsWriteAttributeValue(&tempHandle,0,&cyBle_connHandle,CYBLE_GATT_DB_LOCALLY_INITIATED);  
}

/***************************************************************
 * Function to update the CapSesnse state in the GATT database
 **************************************************************/
//void updateCapsense()
//{
//    if(CyBle_GetState() != CYBLE_STATE_CONNECTED)
//       return;
//    
//	CYBLE_GATTS_HANDLE_VALUE_NTF_T 	tempHandle;
//    
//    tempHandle.attrHandle = CYBLE_LEDCAPSENSE_CAPSENSE_CHAR_HANDLE;
//  	tempHandle.value.val = (uint8 *)&fingerPos;
//    tempHandle.value.len = 2; 
//    CyBle_GattsWriteAttributeValue(&tempHandle,0,&cyBle_connHandle,CYBLE_GATT_DB_LOCALLY_INITIATED );  
//    
//     send notification to client if notifications are enabled and finger location has changed */
//    if (capsenseNotify && (fingerPos != fingerPosOld) )
//        CyBle_GattsNotification(cyBle_connHandle,&tempHandle);
//        fingerPosOld = fingerPos;
//}

/***************************************************************
 * Function to update the CapSesnse state in the GATT database
 **************************************************************/
void updateTemp()
{
    if(CyBle_GetState() != CYBLE_STATE_CONNECTED)
        return;
    
	CYBLE_GATTS_HANDLE_VALUE_NTF_T 	tempHandle;
    
    tempHandle.attrHandle = CYBLE_LEDCAPSENSE_TEMP_CHAR_HANDLE;
  	//tempHandle.value.val = (uint8 *)&fingerPos;
    tempHandle.value.val = (uint8 *)&Temp;
    tempHandle.value.len = 2; 
    CyBle_GattsWriteAttributeValue(&tempHandle,0,&cyBle_connHandle,CYBLE_GATT_DB_LOCALLY_INITIATED );  
    
    /* send notification to client if notifications are enabled and finger location has changed */
    if (tempNotify && (Temp != TempOld) )
        CyBle_GattsNotification(cyBle_connHandle,&tempHandle);
        TempOld = Temp;
}

/***************************************************************
 * Function to handle the BLE stack
 **************************************************************/
void BleCallBack(uint32 event, void* eventParam)
{
    CYBLE_GATTS_WRITE_REQ_PARAM_T *wrReqParam;

    switch(event)
    {
        /* if there is a disconnect or the stack just turned on from a reset then start the advertising and turn on the LED blinking */
        case CYBLE_EVT_STACK_ON:
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
            tempNotify = 0;
            capsenseNotify = 0;
            CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            blue_Write(0);
            //pwm_Start();
        break;
        
        /* when a connection is made, update the LED and Capsense states in the GATT database and stop blinking the LED */    
        case CYBLE_EVT_GATT_CONNECT_IND:
            updateServo();
            updateLed();
            updateTemp();
            blue_Write(1);
            //updateCapsense();  
            //pwm_Stop();
		break;

        /* handle a write request */
        case CYBLE_EVT_GATTS_WRITE_REQ:
            wrReqParam = (CYBLE_GATTS_WRITE_REQ_PARAM_T *) eventParam;
			
            /* request write the Servo value */
            if(wrReqParam->handleValPair.attrHandle == CYBLE_LEDCAPSENSE_SERVO_CHAR_HANDLE)
            {
                /* only update the value and write the response if the requested write is allowed */
                if(CYBLE_GATT_ERR_NONE == CyBle_GattsWriteAttributeValue(&wrReqParam->handleValPair, 0, &cyBle_connHandle, CYBLE_GATT_DB_PEER_INITIATED))
                {
                    switch (wrReqParam->handleValPair.value.val[0]) {
                        case (0):
                            PWM_Servo_WriteCompare(4250);
                            //Timer_WritePeriod(1205);
                            //Timer_Start();
                        break;
                        case (1):
                            PWM_Servo_WriteCompare(4350);
                            //Timer_WritePeriod(540);
                            //Timer_Start();
                        break;
                        case (2):
                            PWM_Servo_WriteCompare(4450);
                            //Timer_WritePeriod(225);
                            //Timer_Start();
                        case (3):
                            PWM_Servo_WriteCompare(4450);
                        break;
                        case (4):
                            PWM_Servo_WriteCompare(4550);
                        break;
                        case (5):
                            PWM_Servo_WriteCompare(4650);
                        break;
                    default:
                        break;
                    }
                    //red_Write(!wrReqParam->handleValPair.value.val[0]);
                    CyBle_GattsWriteRsp(cyBle_connHandle);
                }
            }
            
            /* request write the LED value */
            if(wrReqParam->handleValPair.attrHandle == CYBLE_LEDCAPSENSE_LED_CHAR_HANDLE)
            {
                /* only update the value and write the response if the requested write is allowed */
                if(CYBLE_GATT_ERR_NONE == CyBle_GattsWriteAttributeValue(&wrReqParam->handleValPair, 0, &cyBle_connHandle, CYBLE_GATT_DB_PEER_INITIATED))
                {
                    red_Write(!wrReqParam->handleValPair.value.val[0]);
                    CyBle_GattsWriteRsp(cyBle_connHandle);
                }
            }
            
            /* request to update the CapSense notification */
            //if(wrReqParam->handleValPair.attrHandle == CYBLE_LEDCAPSENSE_CAPSENSE_CAPSENSECCCD_DESC_HANDLE) 
            //{
                //CyBle_GattsWriteAttributeValue(&wrReqParam->handleValPair, 0, &cyBle_connHandle, CYBLE_GATT_DB_PEER_INITIATED);
                //capsenseNotify = wrReqParam->handleValPair.value.val[0] & 0x01;
                //CyBle_GattsWriteRsp(cyBle_connHandle);
            //}
            
            // request to update temp notification
            if(wrReqParam->handleValPair.attrHandle == CYBLE_LEDCAPSENSE_TEMP_TEMPCCCD_DESC_HANDLE)
            {
                CyBle_GattsWriteAttributeValue(&wrReqParam->handleValPair, 0, &cyBle_connHandle, CYBLE_GATT_DB_PEER_INITIATED);
                tempNotify = wrReqParam->handleValPair.value.val[0] & 0x01;
                CyBle_GattsWriteRsp(cyBle_connHandle);
            }
            
			break;  
        
        default:
            break;
    }
} 

CY_ISR(Timer_Int_Handler) {
    flag = 1;
    Timer_ClearInterrupt(Timer_INTR_MASK_TC);
}



/***************************************************************
 * Main
 **************************************************************/
int main()
{
    CyGlobalIntEnable; 
    
    //capsense_Start();
    //capsense_InitializeEnabledBaselines();
    
    timer_int_StartEx(Timer_Int_Handler);
    PWM_Servo_Start();
    UART_Start();
    OneWire_Start();
    flag = 1;
    Temp = 0;
    /* Start BLE stack and register the callback function */
    CyBle_Start(BleCallBack);
    
    // Turn off PWM
    PWM_Servo_WriteCompare(0);
    
    for(;;)
    {        
        /* if Capsense scan is done, read the value and start another scan */
        /*if(!capsense_IsBusy()
        {
            fingerPos=capsense_GetCentroidPos(capsense_LINEARSLIDER0__LS);
            capsense_UpdateEnabledBaselines();
            capsense_ScanEnabledWidgets();
            updateCapsense();
        }
        */
        
        if (flag == 1)
        {
            flag = 0;
            OneWire_SendTemperatureRequest();
        }
        if (OneWire_DataReady)
        {
            OneWire_ReadTemperature();
            Temp = (uint16) OneWire_GetTemperatureAsInt100(0);
            //char* strMsg;
            char buf[6];
            //strMsg = OneWire_GetTemperatureAsString(0);
            sprintf(buf, "%d\r\n", Temp);
            UART_UartPutString(buf);
            updateTemp();
            flag = 1;
            //Timer_WritePeriod(10000);
            //Timer_Start();
            //UART_UartPutString("!!!\r\n");
            //UART_UartPutString(strMsg);
            //UART_UartPutString("\r\n");
           
        }
        
        CyBle_ProcessEvents();
        CyBle_EnterLPM(CYBLE_BLESS_DEEPSLEEP);    
    }
}
