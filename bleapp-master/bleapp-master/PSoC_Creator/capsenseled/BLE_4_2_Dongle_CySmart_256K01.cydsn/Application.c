/******************************************************************************
* Project Name      : BLE_Dongle_CySmart
* File Name         : Application.c
* Description       : This module handles state machine for custom client operation.
*                          Scan and connects to fixed CapSense client device, update the brightness as per slider position.
* Version           : 1.2
* Software Used     : PSoC Creator 3.3 CP2
* Compiler          : ARM GCC 4.9.3, ARM MDK Generic
* Related Hardware  : CY5677 CySmart BLE 4.2 USB Dongle
*
********************************************************************************
* Copyright (2016), Cypress Semiconductor Corporation. All Rights Reserved.
********************************************************************************
* This software is owned by Cypress Semiconductor Corporation (Cypress)
* and is protected by and subject to worldwide patent protection (United
* States and foreign), United States copyright laws and international treaty
* provisions. Cypress hereby grants to licensee a personal, non-exclusive,
* non-transferable license to copy, use, modify, create derivative works of,
* and compile the Cypress Source Code and derivative works for the sole
* purpose of creating custom software in support of licensee product to be
* used only in conjunction with a Cypress integrated circuit as specified in
* the applicable agreement. Any reproduction, modification, translation,
* compilation, or representation of this software except as specified above 
* is prohibited without the express written permission of Cypress.
*
* Disclaimer: CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH 
* REGARD TO THIS MATERIAL, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* Cypress reserves the right to make changes without further notice to the 
* materials described herein. Cypress does not assume any liability arising out 
* of the application or use of any product or circuit described herein. Cypress 
* does not authorize its products for use as critical components in life-support 
* systems where a malfunction or failure may reasonably be expected to result in 
* significant injury to the user. The inclusion of Cypress' product in a life-
* support systems application implies that the manufacturer assumes all risk of 
* such use and in doing so indemnifies Cypress against all charges. 
*
* Use of this Software may be limited by and subject to the applicable Cypress
* software license agreement. 
*******************************************************************************/

#include "Application.h"
#include "CySmt_TransportLayer.h"


/* Dongle state machine for CapSense custom client */
typedef enum
{
    /* Default state until peripheral device is discovered */
    DONGLE_BLE_STATE_NOT_READY = 0,

    /* BLE is ready for any commands with peripheral */
    DONGLE_BLE_STATE_ACTIVE,

    /* BLE module is processing current command */
    DONGLE_BLE_STATE_BUSY
}DONGLE_BLE_STATES_T;

#define CAPSENSE_SLIDER_MAX_VALUE       (0x64u)

/* CapSense slider example server configurations */
#define CAPSENSE_SLIDER_ATT_UUID        (0xCAA2u)

/* Initialize flag for button trigger */
volatile static bool isButtonTriggered = false;
/* Flag to check if Advertisement packet occurred from CapSense Slider device */
static bool isCapSenseDeviceFound = false;
/* Flag for device which is waiting for scan */
static bool isScanRspOccured = false;

/* Initialize state machine.*/
static DONGLE_BLE_STATES_T DongleBleState = DONGLE_BLE_STATE_NOT_READY;

/* Fixed CapSense example BD address */
static CYBLE_GAP_BD_ADDR_T detectedServerAddr = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0};

/*****************************************************************************
* Function Name: DongleBleAssert()
******************************************************************************
* Summary:
* This functions asserts if any unexpected errors occur in BLE API calls
*
* Parameters:
* None
*
* Return:
* None
*
* Theory:
* None
* 
* Side Effects:
* Code flow will be halted if ENABLE_ASSERT is enabled.
* 
*****************************************************************************/
static void DongleBleAssert(void)
{
    /* Should not come here, check the failure reasons */
    Led_Stop();
#ifdef ENABLE_ASSERT
    while(1);
#endif /* ENABLE_ASSERT */
}

/*******************************************************************************
* Function Name: Dongle_Ble_State_Handler
********************************************************************************
*
* Summary:
*  Handles state machine for BLE CapSense Custom client
*
* Parameters:
* None
*
* Return:
* None
*
* Theory:
* None
*
* Side Effects:
* None
*
*******************************************************************************/
void Dongle_Ble_State_Handler(void)
{    
    if (CYBLE_STACK_STATE_BUSY == CyBle_GattGetBusStatus())
    {
        /* If stack is busy wait for ready signal */
        return;
    }

    switch(DongleBleState)
    {
        /* BLE component will be in this state until the GATT discovery is complete on connected peripheral */
        case DONGLE_BLE_STATE_NOT_READY:
            if(CYBLE_STATE_DISCONNECTED == CyBle_GetState())
            {
                if(isScanRspOccured)
                {
                    isScanRspOccured = false;
                    if(CYBLE_ERROR_OK != CyBle_GapcConnectDevice(&detectedServerAddr))
                    {
                        DongleBleAssert();
                    }
                }
                else if(isButtonTriggered)
                {
                    /* Start scanning, if button is pressed */
                    (void)CyBle_GapcStartScan(CYBLE_SCANNING_CUSTOM);
                }
            }
            else if(CYBLE_STATE_SCANNING == CyBle_GetState())
            {
                if(isButtonTriggered)
                {
                    isButtonTriggered = false;
                }
            }
            else if(CYBLE_CLIENT_STATE_CONNECTED == CyBle_GetClientState())
            {
                /* Discover all server DB items */
                if(CYBLE_ERROR_OK != CyBle_GattcStartDiscovery(cyBle_connHandle))
                {
                    /* Assert if unable to start discovery */
                    DongleBleAssert();
                }
            }
            else if(CYBLE_CLIENT_STATE_DISCOVERED == CyBle_GetClientState())
            {
                /* Enable notification on server device */
                uint16 attrValue = CYBLE_CCCD_NOTIFICATION;
                CYBLE_API_RESULT_T status;
                CYBLE_GATTC_WRITE_REQ_T writeReqParam;

                /* CapSense slider Descriptor handle to set notifications */
                writeReqParam.attrHandle = cyBle_customCServ[0].customServChar[0].customServCharDesc[0].descHandle;

                writeReqParam.value.len = sizeof(attrValue);
                writeReqParam.value.val = (uint8*)&attrValue;

                status = CyBle_GattcWriteCharacteristicDescriptors(
                                    cyBle_connHandle, &writeReqParam);

                if(CYBLE_ERROR_OK == status)
                {
                    DongleBleState = DONGLE_BLE_STATE_BUSY;
                }
                else
                {
                    /* Disconnect if GATT write fails */
                    (void)CyBle_GapDisconnect(cyBle_connHandle.bdHandle);
                }
            }
            break;

        case DONGLE_BLE_STATE_ACTIVE:
            /* Reset the application state, if BLE is not in discovered state */
            if(CYBLE_CLIENT_STATE_DISCOVERED != CyBle_GetClientState())
            {
                DongleBleState = DONGLE_BLE_STATE_NOT_READY;
            }

            /* Enter into low power mode while waiting for notifications */
            {
                CYBLE_BLESS_STATE_T blessState = CYBLE_BLESS_STATE_ACTIVE;
                CYBLE_LP_MODE_T pwrState = CyBle_EnterLPM(CYBLE_BLESS_SLEEP);

                CyGlobalIntDisable;
                blessState = CyBle_GetBleSsState();
                if(pwrState == CYBLE_BLESS_SLEEP)
                {
                    if(blessState != CYBLE_BLESS_STATE_EVENT_CLOSE)
                    {
                        CySysPmSleep();
                    }
                }
                CyGlobalIntEnable;
            }
            break;

        case DONGLE_BLE_STATE_BUSY:
            /* BLE component is processing current command */

            /* Reset the application state, if BLE is not in discovered state */
            if(CYBLE_CLIENT_STATE_DISCOVERED != CyBle_GetClientState())
            {
                DongleBleState = DONGLE_BLE_STATE_NOT_READY;
            }
            break;
    }
    
    /* On button press disconnect if connection exist and re-start scanning without white-list */
    if(isButtonTriggered)
    {
        if(CYBLE_STATE_CONNECTED == CyBle_GetState())
        {
            /* Issue disconnect for already connected device */
            if(CYBLE_ERROR_OK != CyBle_GapDisconnect(cyBle_connHandle.bdHandle))
            {
                DongleBleAssert();
            }
        }
        else if(CYBLE_STATE_CONNECTING == CyBle_GetState())
        {
            /* Cancel the connection, if it is in progress */
            if(CYBLE_ERROR_OK != CyBle_GapcCancelDeviceConnection())
            {
                DongleBleAssert();
            }
        }
        else if(CYBLE_STATE_SCANNING == CyBle_GetState())
        {
            /* Restart the scanning */
            CyBle_GapcStopScan();
        }
    }
}

static void UpdateLEDBrightness(uint8 CapSensePos)
{
    /* Calculate the value from CapSense level to LED level */
    Led_Stop();
    Led_On(((CAPSENSE_SLIDER_MAX_VALUE - CapSensePos) * Prism_Led_PWM_PERIOD_VALUE)
                /(CAPSENSE_SLIDER_MAX_VALUE));
}

/*****************************************************************************
* Function Name: IsCapSenseSliderSupported()
******************************************************************************
* Summary:
* This function checks if the advt report contains CapSense service and attribute UUID.
*
* Parameters:
* CYBLE_GAPC_ADV_REPORT_T*
*
* Return:
* bool - returns True if the report packet contains CapSense service UUID
*
* Theory:
* None
* 
* Side Effects:
* None
* 
*****************************************************************************/
static bool IsCapSenseSliderSupported(CYBLE_GAPC_ADV_REPORT_T* scanReport)
{
    uint16 byteindex;

    for(byteindex = 0; byteindex <  scanReport->dataLen; )
    {
        /* Each advt data element contains: Length + Type + Data */
        uint8 advStructureSize = scanReport->data[byteindex];
        if(advStructureSize)
        {
            /* Check the type of Adv Data */
            uint8 advStructureType =
                           scanReport->data[byteindex + sizeof(advStructureSize)];
            /* Slider UUID can be available in service data structure */
            if(CYBLE_GAP_ADV_SRVC_DATA_128UUID == advStructureType)
            {                  
                /* Point to Adv structure data */
                uint8 UuidIndex = byteindex + sizeof(advStructureSize) + sizeof(advStructureType);
                /* Parse the list of UUIDs and check for CapSense service */
                while(UuidIndex < (byteindex + advStructureSize))
                {
                    if(memcmp(cyBle_customCServ[0].uuid, &scanReport->data[UuidIndex], CYBLE_GATT_128_BIT_UUID_SIZE) == 0u)
                    {
                        /* Check if adv structure contains the service data */
                        UuidIndex += CYBLE_GATT_128_BIT_UUID_SIZE;
                        if(UuidIndex < (byteindex + advStructureSize))
                        {
                            /* Return success, if service data matches to slider ATT UUID */
                            if(CAPSENSE_SLIDER_ATT_UUID == CyBle_Get16ByPtr(&scanReport->data[UuidIndex]))
                            {
                                return true;
                            }
                        }
                    }
                    /* Check next UUID */
                    UuidIndex += CYBLE_GATT_16_BIT_UUID_SIZE;
                }
            }
            /* Move to next AD structure */
            byteindex += (advStructureSize + sizeof(uint8));
        }
        else
        {
            return false;
        }
    }
    return false;
}

/*****************************************************************************
* Function Name: FilterScanResponsePackets()
******************************************************************************
* Summary:
* This function stops further scanning if a CapSense server is found.
*
* Parameters:
* CYBLE_GAPC_ADV_REPORT_T*
*
* Return:
* None
*
* Theory:
* None
*
* Side Effects:
* Updates the flag "isScanRspOccured" & "isCapSenseDeviceFound"
* 
*****************************************************************************/
static void FilterScanResponsePackets(CYBLE_GAPC_ADV_REPORT_T* scanReport)
{
    if(NULL == scanReport)
    {
        return;
    }

    /* Check if device contains CapSense service */
    if(IsCapSenseSliderSupported(scanReport))
    {
        /* Copy the detected BD addr and set the device found flag */
        memcpy(detectedServerAddr.bdAddr, scanReport->peerBdAddr,
               sizeof(detectedServerAddr.bdAddr));
        detectedServerAddr.type = scanReport->peerAddrType;
        isCapSenseDeviceFound = true;
        /* Wait for scan response */
        isScanRspOccured = false;
    }

    /* Check if the scan reponse occured for detected CapSense device */
    if(((CYBLE_GAPC_SCAN_RSP == scanReport->eventType) && 
         (!memcmp(detectedServerAddr.bdAddr, scanReport->peerBdAddr, sizeof(detectedServerAddr.bdAddr)))))
    {
        if(isCapSenseDeviceFound)
        {
            CyBle_GapcStopScan();
            isScanRspOccured = true;
            isCapSenseDeviceFound = false;
        }
    }

}

/*******************************************************************************
* Function Name: CapSenseClientEventHandler
********************************************************************************
*
* Summary:
*  This function handles events as required for CapSense client device
*
* Parameters:  
*  uint8 event:       Event from the CYBLE component
*  void* eventParams: A structure instance for corresponding event type. The 
*                     list of event structure is described in the component 
*                     data-sheet.
*
* Return: 
* None
*
*******************************************************************************/
static void CapSenseClientEventHandler(uint32 event, void *eventParam)
{
    switch(event)
    {
        case CYBLE_EVT_HARDWARE_ERROR:
        case CYBLE_EVT_HCI_STATUS:
            {
                DongleBleAssert();
            }
            break;

        case CYBLE_EVT_STACK_ON:
            {
                CYBLE_BLESS_CLK_CFG_PARAMS_T bleSSClkConfig;
                if(CYBLE_ERROR_OK == CyBle_GetBleClockCfgParam(&bleSSClkConfig))
                {
                    bleSSClkConfig.bleLlSca = CYBLE_LL_SCA_101_TO_150_PPM;
                    CyBle_SetBleClockCfgParam(&bleSSClkConfig);
                }
                /* In application mode reset the IO capability */
                CyBle_GapSetIoCap(CYBLE_GAP_IOCAP_NOINPUT_NOOUTPUT);
            }
            break;

        case CYBLE_EVT_TIMEOUT:
            /* Issue disconnect on any GATT timeout */
            if(CYBLE_GATT_RSP_TO == (CYBLE_TO_REASON_CODE_T)*(uint8 *)eventParam)
            {
                /* Close the connection for any time out errors */
                CyBle_GapDisconnect(cyBle_connHandle.bdHandle);
            }
            break;

        case CYBLE_EVT_GAPC_SCAN_PROGRESS_RESULT:
            {
                /* Check if device meets the dongle requirements */
                FilterScanResponsePackets((CYBLE_GAPC_ADV_REPORT_T *)eventParam);
            }
            break;

        case CYBLE_EVT_GATTC_ERROR_RSP:
            {
                /* This event indicates some failure on GATT operation */
                /* Disconnect the device and look for next device */
                CyBle_GapDisconnect(cyBle_connHandle.bdHandle);
            }
            break;
            
        case CYBLE_EVT_GAP_DEVICE_CONNECTED:
        case CYBLE_EVT_GAP_ENHANCE_CONN_COMPLETE:
            /* Power ON LED, for 3 seconds */
            Led_Stop();
            Device_Led_On_Timeout(3000);
            break;

        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
            Led_Stop();
            break;

        case CYBLE_EVT_GATTC_WRITE_RSP:
            DongleBleState = DONGLE_BLE_STATE_ACTIVE;
            break;

        case CYBLE_EVT_GATTC_HANDLE_VALUE_NTF:
            {
                /* CapSense value change notification occurred, Handle the LED brightness */
                CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *CapsenseValue = 
                    (CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *)eventParam;

                /* Accept only slider notifications to update the LED brightness */
                if(cyBle_customCServ[0].customServChar[0].customServCharHandle == CapsenseValue->handleValPair.attrHandle)
                {
                    /* Ignore 0xFF and update other values */
                    if((uint8)0xFF != (uint8)*(CapsenseValue->handleValPair.value.val))
                    {
                        UpdateLEDBrightness(*(CapsenseValue->handleValPair.value.val));
                    }
                }
            }
            break;

        case CYBLE_EVT_L2CAP_CONN_PARAM_UPDATE_REQ:
            {
                /* Reject connection parameter request from slider, to get faster notifications */
                 if(CYBLE_ERROR_OK != CyBle_L2capLeConnectionParamUpdateResponse(
                     cyBle_connHandle.bdHandle, CYBLE_L2CAP_CONN_PARAM_REJECTED))
                 {
                     DongleBleAssert();
                 }
            }
            break;
    }
}

/*****************************************************************************
* Function Name: Process_Suspend_Command()
******************************************************************************
* Summary:
* This function puts system into low power mode, on suspend command.
*
* Parameters:
* None
*
* Return:
* None
*
* Theory:
* None
*
* Side Effects:
* None
*
*****************************************************************************/
void Process_Suspend_Command(void)
{
    CYBLE_BLESS_STATE_T blessState = CYBLE_BLESS_STATE_ACTIVE;
    CYBLE_LP_MODE_T pwrState;

    /* Stop scanning before entering sleep mode */
    if(CYBLE_STATE_SCANNING == CyBle_GetState())
    {
        CyBle_GapcStopScan();
    }
    else if(CYBLE_STATE_CONNECTED == CyBle_GetState())
    {
        /* Issue disconnect for already connected device */
        if(CYBLE_ERROR_OK != CyBle_GapDisconnect(cyBle_connHandle.bdHandle))
        {
            DongleBleAssert();
        }
    }
    CyBle_ProcessEvents();

    /* Wait for disconnect state, before getting into deepsleep */
    if(CYBLE_STATE_DISCONNECTED == CyBle_GetState())
    {
        /* Switch OFF the LED, should be off when suspend occurs */
        Led_Stop();

        pwrState = CyBle_EnterLPM(CYBLE_BLESS_DEEPSLEEP);

        /* Enter into deep sleep mode, as per BLE component state */
        if(pwrState == CYBLE_BLESS_DEEPSLEEP)
        {
            CyGlobalIntDisable;
            blessState = CyBle_GetBleSsState();
            if(blessState == CYBLE_BLESS_STATE_ECO_ON || blessState == CYBLE_BLESS_STATE_DEEPSLEEP)
            {
                /* Send complete system components to deep sleep */
                UART_Sleep();
                Timer_10ms_Stop(); 
                Timer_Stop();
                Led_Stop();
                isr_10ms_Stop();
                /* Send the CPU to sleep */
                CySysPmDeepSleep();

                /* Restart the components after wakeup */
                isr_10ms_StartEx(Transport_Timer_ISR);
                Isr_Suspend_StartEx(Suspend_Cmd_ISR);
                Isr_Button_StartEx(Button_ISR);
                Timer_10ms_Start();
                Timer_Init(Device_Timer_Callback);
                UART_Wakeup();
            }
            CyGlobalIntEnable;
        }

        CyBle_ProcessEvents();
    }
}

/*****************************************************************************
* Function Name: BLE_Init()
******************************************************************************
* Summary:
* Starts BLE components and register callback functions for events.
*
* Parameters:
* None
*
* Return:
* None
*
* Theory:
* None
*
* Side Effects:
* None
*
*****************************************************************************/
void BLE_Init(void)
{
    isCapSenseDeviceFound = false;
    isScanRspOccured = false;

    /* scan in observation procedure, to get directed adv packets also */
    cyBle_discoveryInfo.discProcedure = CYBLE_GAPC_OBSER_PROCEDURE;
    cyBle_discoveryInfo.scanType = CYBLE_GAPC_ACTIVE_SCANNING;
    cyBle_discoveryInfo.scanIntv = CYBLE_FAST_SCAN_INTERVAL;
    cyBle_discoveryInfo.scanWindow = CYBLE_FAST_SCAN_WINDOW;
    cyBle_discoveryInfo.ownAddrType = CYBLE_GAP_ADDR_TYPE_PUBLIC;
    cyBle_discoveryInfo.scanFilterPolicy = CYBLE_GAPC_ADV_ACCEPT_ALL_PKT;
    cyBle_discoveryInfo.scanTo = CYBLE_FAST_SCAN_TIMEOUT;
    cyBle_discoveryInfo.filterDuplicates = CYBLE_GAPC_FILTER_DUP_ENABLE;

    cyBle_connectionParameters.scanIntv = cyBle_discoveryInfo.scanIntv;
    cyBle_connectionParameters.scanWindow = cyBle_discoveryInfo.scanWindow;
    cyBle_connectionParameters.ownAddrType = cyBle_discoveryInfo.ownAddrType;
    cyBle_connectionParameters.initiatorFilterPolicy = cyBle_discoveryInfo.scanFilterPolicy;
    cyBle_connectionParameters.connIntvMin = CYBLE_GAPC_CONNECTION_INTERVAL_MIN;
    cyBle_connectionParameters.connIntvMax = CYBLE_GAPC_CONNECTION_INTERVAL_MAX;
    cyBle_connectionParameters.connLatency = CYBLE_GAPC_CONNECTION_SLAVE_LATENCY;
    cyBle_connectionParameters.supervisionTO = CYBLE_GAPC_CONNECTION_TIME_OUT;
    cyBle_connectionParameters.minCeLength = 0x0000u;
    cyBle_connectionParameters.maxCeLength = 0xFFFFu;

    cyBle_authInfo.security = CYBLE_GAP_SEC_MODE_1 | CYBLE_GAP_SEC_LEVEL_1;
    cyBle_authInfo.bonding = CYBLE_GAP_BONDING_NONE;
    cyBle_authInfo.ekeySize = 0x10u;
    cyBle_authInfo.authErr = CYBLE_GAP_AUTH_ERROR_NONE;

    /* Disable automatic authentication procedures by component */
    cyBle_eventHandlerFlag |= CYBLE_DISABLE_AUTOMATIC_AUTH;

    DongleBleState = DONGLE_BLE_STATE_NOT_READY;

    /* Register callback to handle CapSense client mode */
    if(CYBLE_ERROR_OK != CyBle_Start(CapSenseClientEventHandler))
    {
        DongleBleAssert();
    }
}

/*****************************************************************************
* Function Name: BLE_DeInit()
******************************************************************************
* Summary:
* This functions stops the BLE component and sets call back functions to NULL.
* This is called just before switching to CySmart tool.
*
* Parameters:
* None
*
* Return:
* None
*
* Theory:
* None
* 
* Side Effects:
* None
* 
*****************************************************************************/
void BLE_DeInit(void)
{
    CyBle_Stop();
}

/*******************************************************************************
* Function Name: Button_ISR
********************************************************************************
*
* Summary:
* ISR Callback function for user button pin, on interrupt this ISR will set button triggered flag
*
* Parameters:
* None
*
* Return:
* None
*
* Theory:
* None
*
* Side Effects:
* Modifies isButtonTriggered flag
*
*******************************************************************************/
CY_ISR(Button_ISR)
{
    isButtonTriggered = true;
    Pin_UserButton_ClearInterrupt();
}

/* [] END OF FILE */
