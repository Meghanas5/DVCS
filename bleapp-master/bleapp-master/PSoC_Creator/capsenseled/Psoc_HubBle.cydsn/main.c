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
#include <project.h>
#include <stdio.h>

#define CYBLE_MAX_ADV_DEVICES       10u

/* BLE State Macros used for LED status updates*/
#define BLE_DISCONNECTED				0x01
#define BLE_SCANNING					0x02
#define BLE_SERVICE_DISCOVERY			0x03
#define BLE_CONNECTED					0x04
#define BLE_SERVICE_FOUND               0x05
#define BLE_READING                     0x06
#define BLE_WRITING                     0x07

CYBLE_GATTC_READ_BY_TYPE_REQ_T          readByTypeReqParam;
CYBLE_GATTC_READ_BY_TYPE_RSP_PARAM_T*    readResponse;
CYBLE_GATTC_READ_BY_GRP_RSP_PARAM_T     readGroupResponse;

CYBLE_GATTC_FIND_BY_TYPE_RSP_PARAM_T*   findResponse;

CYBLE_GATTC_READ_BY_GRP_RSP_PARAM_T     readGroupArray[10];

CYBLE_GATTC_READ_REQ_T                  readRequest;
CYBLE_GATT_ATTR_HANDLE_RANGE_T          range;
CYBLE_UUID_T                            uuid;

uint16 ledCharHandle = 0;

CYBLE_GATTC_ERR_RSP_PARAM_T error;

int lock = 1;

// reverse!!!!
/*
const uint8 VentUnitServiceUuid[16]    = {
                                            0x27, 0xA2, 0xBA, 0x12, 0xD1, 0x65, 0x4D, 0xB8, \
                                            0x8D, 0x01, 0x57, 0xC3, 0xEC, 0xDA, 0x67, 0xE7 \
                                        };
const uint8 ledcharUuid[16]            = {
                                            0x44, 0x9B, 0xF4, 0xC8, 0x3D, 0x01, 0x49, 0xC1, \
                                            0x9F, 0x5F, 0xB1, 0x12, 0x81, 0xFD, 0xC3, 0x9B \
                                        };
*/
const uint8 VentUnitServiceUuid[2] = { (uint8)0x12, (uint8)0xBA};

CYBLE_UUID_T LedCharUuid;

const CYBLE_GATT_VALUE_T    VentUnitUuidInfo = { 
                                                        (uint8 *) VentUnitServiceUuid, \
                                                        CYBLE_GATT_16_BIT_UUID_SIZE,\
                                                        CYBLE_GATT_16_BIT_UUID_SIZE \
                                                      };

uint8 ble_state = BLE_DISCONNECTED;


uint8 deviceConnected = 0;

CYBLE_GAP_BD_ADDR_T connectPeriphDevice;

uint8 periphAddress[6];
uint8 periphFound = 0;

uint8 addedDevices = 0;

uint8 address_store[10][6];


uint8 restartScanning = 0;

CYBLE_EVENT_T a;

char str_buf[50];

CYBLE_GAPC_ADV_REPORT_T 	list_of_devices[CYBLE_MAX_ADV_DEVICES];

/* BD Address Length*/
#define ADV_ADDR_LEN				0x06
	
/* Public Address indication used for Scanning */
#define PUBLIC_ADDR_TYPE			0x00
	
/* Random Address indication used for Scanning */
#define RANDOM_ADDR_TYPE			0x01

CYBLE_CONN_HANDLE_T			connectionHandle;

void HandleScanDevices(CYBLE_GAPC_ADV_REPORT_T* scanReport);


void Stack_Handler(uint32 eventCode, void* eventParam)
{
    CYBLE_API_RESULT_T 			apiResult;
    
    CYBLE_GAPC_ADV_REPORT_T scanReport;
    
    uint16 i = 0;
    
    switch(eventCode)
    {
        case CYBLE_EVT_STACK_ON:
            restartScanning = 1;
            break;
        case CYBLE_EVT_GAPC_SCAN_START_STOP:
            if (CyBle_GetState() == CYBLE_STATE_DISCONNECTED)
            {
                ble_state = BLE_DISCONNECTED;
                if (!periphFound)
                {
                    restartScanning = 1;
                }
            }
            else
                ble_state = BLE_SCANNING;
            break;
        case CYBLE_EVT_GAPC_SCAN_PROGRESS_RESULT:
            scanReport = *(CYBLE_GAPC_ADV_REPORT_T*) eventParam;
            
            ble_state = BLE_SCANNING;
            break;
        case CYBLE_EVT_GATT_CONNECT_IND:
            connectionHandle = *(CYBLE_CONN_HANDLE_T*)eventParam;
            break;
        case CYBLE_EVT_GAP_ENHANCE_CONN_COMPLETE:
            a = eventCode;
            break;
        case CYBLE_EVT_GATT_DISCONNECT_IND:
            break;
        case CYBLE_EVT_GAP_DEVICE_CONNECTED:
            CyBle_GattcStartDiscovery(connectionHandle);
            ble_state = BLE_CONNECTED;
            break;
        case CYBLE_EVT_GATTC_DISCOVERY_COMPLETE:
            range.startHandle   = 0x0001;
            range.endHandle     = 0x0030;
            uuid.uuid16         = (uint16)0x5EB7; 
            
            readByTypeReqParam.range = range;
            readByTypeReqParam.uuid = uuid;
            readByTypeReqParam.uuidFormat = 0x01;
            apiResult = CyBle_GattcReadUsingCharacteristicUuid(cyBle_connHandle,&readByTypeReqParam);
            
            
            
            //if (CYBLE_ERROR_OK != (apiResult = CyBle_GattcReadCharacteristicValue(connectionHandle, readRequest)))
            //if (CYBLE_ERROR_OK != (apiResult = CyBle_GattcDiscoverAllPrimaryServices(connectionHandle)))
            //if (CYBLE_ERROR_OK != (apiResult = CyBle_GattcDiscoverPrimaryServiceByUuid(connectionHandle,VentUnitUuidInfo)))
            {
                a = eventCode;
            }
            break;
        case CYBLE_EVT_GATTC_READ_BY_GROUP_TYPE_RSP:
            readGroupResponse = *(CYBLE_GATTC_READ_BY_GRP_RSP_PARAM_T*)eventParam;
          
            ble_state = BLE_CONNECTED;
            break;
            
        case CYBLE_EVT_GATTC_READ_BY_TYPE_RSP:
            readResponse = (CYBLE_GATTC_READ_BY_TYPE_RSP_PARAM_T*)eventParam;
            sprintf(str_buf, "\r\nLength of Characteristic is: %d\r\n",readResponse->attrData.length);
            UART_UartPutString(str_buf);
            sprintf(str_buf, "Characteristic is: %s\r\n",readResponse->attrData.attrValue);
            UART_UartPutString(str_buf);
            ledCharHandle = readResponse->attrData.attrValue[3];
            ledCharHandle |= (readResponse->attrData.attrValue[4] << 8);
            
            ble_state = BLE_READING;
            break;
        case CYBLE_EVT_GATTC_READ_RSP:
            readRequest = *(CYBLE_GATTC_READ_REQ_T*)eventParam;
            a = eventCode;
            ble_state = BLE_CONNECTED;
            
        case CYBLE_EVT_GATTC_FIND_BY_TYPE_VALUE_RSP:
            findResponse            = (CYBLE_GATTC_FIND_BY_TYPE_RSP_PARAM_T *) eventParam;
            //CYBLE_GATTC_FIND_BY_TYPE_RSP_PARAM_T* temp1 = findResponse;
            readByTypeReqParam.range.startHandle = findResponse->range->startHandle;
            readByTypeReqParam.range.endHandle = findResponse->range->endHandle;
            a = eventCode;
            CyBle_GattcStopCmd();
            ble_state = BLE_SERVICE_FOUND;
            break;
        case CYBLE_EVT_GATTC_WRITE_RSP:
            a = eventCode;
            break;
            
        case CYBLE_EVT_GATTC_ERROR_RSP:
            error = *(CYBLE_GATTC_ERR_RSP_PARAM_T*)eventParam;
            a = eventCode;
            break;
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
            for (i = 0; i < addedDevices; i++)
            {
                list_of_devices[i].peerBdAddr[0] = 0;
                list_of_devices[i].peerBdAddr[1] = 0;
                list_of_devices[i].peerBdAddr[2] = 0;
                list_of_devices[i].peerBdAddr[3] = 0;
                list_of_devices[i].peerBdAddr[4] = 0;
                list_of_devices[i].peerBdAddr[5] = 0;
            }
            
            addedDevices = 0;
            periphFound = 0;
            deviceConnected = 0;
            ble_state = BLE_DISCONNECTED;
            
            restartScanning = 1;
            
            if (apiResult != CYBLE_ERROR_OK)
            {
            }
            
            break;
        
            
        default:
            deviceConnected = 0;
            break;
    }

}

void HandleScanDevices(CYBLE_GAPC_ADV_REPORT_T* scanReport)
{
	uint16 i;
	uint8 newDevice = 1;
	
	if((addedDevices < CYBLE_MAX_ADV_DEVICES))
	{
		for(i = 0; i < addedDevices; i++)
		{
			/* Initialize the peerBdAddr element of our list.*/
			list_of_devices[i].peerBdAddr = &address_store[i][0];
			
			/* In this for loop, compare the new device address with the existing addresses in the list to 
				determine if the address is new or not. If the address exists in the list, then the device 
				is not new.*/
			if(0 == memcmp(list_of_devices[i].peerBdAddr, scanReport->peerBdAddr, ADV_ADDR_LEN))
			{
				newDevice = 0;
				break;
			}
		}
		
		if(newDevice)
		{
			/* If the device address is new, then add the device to our existing list and compare the address
				with our expected address to see if the desired peripheral is advertising or not.*/
			
			list_of_devices[addedDevices].peerBdAddr = &address_store[addedDevices][0];
			
			/* Store the data*/
			list_of_devices[addedDevices].dataLen = scanReport->dataLen;
			
			list_of_devices[addedDevices].data = scanReport->data;
			
			list_of_devices[addedDevices].eventType = scanReport->eventType;
			/* Record the address type, Public or Random, of the advertising peripheral.*/
			list_of_devices[addedDevices].peerAddrType = scanReport->peerAddrType;
			
			/* Save the BD addresses */
			list_of_devices[addedDevices].peerBdAddr[0] = scanReport->peerBdAddr[0];
			list_of_devices[addedDevices].peerBdAddr[1] = scanReport->peerBdAddr[1];
			list_of_devices[addedDevices].peerBdAddr[2] = scanReport->peerBdAddr[2];
			list_of_devices[addedDevices].peerBdAddr[3] = scanReport->peerBdAddr[3];
			list_of_devices[addedDevices].peerBdAddr[4] = scanReport->peerBdAddr[4];
			list_of_devices[addedDevices].peerBdAddr[5] = scanReport->peerBdAddr[5];
			
			list_of_devices[addedDevices].rssi = scanReport->rssi;
			
			/* If the new BD address found matches the desired BD address, the Dongle has been found*/
			if(0 == memcmp(periphAddress, scanReport->peerBdAddr, ADV_ADDR_LEN))
			{
				/* Save the connected device BD Address and Type*/
				connectPeriphDevice.bdAddr[0] = scanReport->peerBdAddr[0];
				connectPeriphDevice.bdAddr[1] = scanReport->peerBdAddr[1];
				connectPeriphDevice.bdAddr[2] = scanReport->peerBdAddr[2];
				connectPeriphDevice.bdAddr[3] = scanReport->peerBdAddr[3];
				connectPeriphDevice.bdAddr[4] = scanReport->peerBdAddr[4];
				connectPeriphDevice.bdAddr[5] = scanReport->peerBdAddr[5];
				
				connectPeriphDevice.type = list_of_devices[addedDevices].peerAddrType;
					
				/* Set the flag to notify application of a connected peripheral device */
				periphFound = 1;	
				
				/* Stop existing BLE Scan */
				CyBle_GapcStopScan();
			}
			
			addedDevices++;
		}
		
		newDevice = 1;
	}
    
}
int main()
{
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

    CyGlobalIntEnable; /* Uncomment this line to enable global interrupts. */
    
    CyBle_Start(Stack_Handler);
    //periphAddress = 0x00A050CC2313 00A050-654325
    periphAddress[5] = 0x00;
    periphAddress[4] = 0xA0;
    periphAddress[3] = 0x50;
    periphAddress[2] = 0xCC;
    periphAddress[1] = 0x23;
    periphAddress[0] = 0x13;
    
    int flag = 0;
    LED_Conn_Write(1);
    UART_Start();
    
    for(;;)
    {
        
        CyBle_ProcessEvents();
        if (flag == 0)
        {
        connectPeriphDevice.bdAddr[0] = periphAddress[0];
				connectPeriphDevice.bdAddr[1] = periphAddress[1];
				connectPeriphDevice.bdAddr[2] = periphAddress[2];
				connectPeriphDevice.bdAddr[3] = periphAddress[3];
				connectPeriphDevice.bdAddr[4] = periphAddress[4];
				connectPeriphDevice.bdAddr[5] = periphAddress[5];
				
				//connectPeriphDevice.type = list_of_devices[addedDevices].peerAddrType;
                connectPeriphDevice.type = PUBLIC_ADDR_TYPE;
        /* Place your application code here. */
        CyBle_GapcConnectDevice(&connectPeriphDevice);
        flag = 1;
        }
        if (ble_state == BLE_CONNECTED)
        {   /*
            CYBLE_GATTC_WRITE_REQ_T tempHandle;
            uint8 byteVal[1];
            byteVal[0] = 1;
            tempHandle.value.val = (uint8*)byteVal;
            tempHandle.value.len = (uint16)1;
            tempHandle.attrHandle = (CYBLE_GATT_DB_ATTR_HANDLE_T)ledCharHandle;
            //CyBle_GattcWriteCharacteristicValue(connectionHandle, &tempHandle);
            //CyBle_GapDisconnect(cyBle_connHandle.bdHandle);
            ble_state = BLE_DISCONNECTED;
            */
        }
        if (ble_state == BLE_SERVICE_FOUND)
        {
            LedCharUuid.uuid16 = (CYBLE_UUID16)0xF4C8;
            readByTypeReqParam.uuid = LedCharUuid;
            readByTypeReqParam.uuidFormat = CYBLE_GATT_16_BIT_UUID_FORMAT;
            CyBle_GattcDiscoverCharacteristicByUuid(connectionHandle, &readByTypeReqParam);
        }
        if (ble_state == BLE_READING)
        {
            LedCharUuid.uuid16 = (CYBLE_UUID16)0xF4C8;
            readByTypeReqParam.uuid = LedCharUuid;
            readByTypeReqParam.uuidFormat = CYBLE_GATT_16_BIT_UUID_FORMAT;
            CyBle_GattcReadUsingCharacteristicUuid(connectionHandle, &readByTypeReqParam);
            CYBLE_GATTC_WRITE_REQ_T tempHandle;
            uint8 byteVal[1];
            byteVal[0] = 1;
            tempHandle.value.val = (uint8*)byteVal;
            tempHandle.value.len = (uint16)1;
            tempHandle.attrHandle = (CYBLE_GATT_DB_ATTR_HANDLE_T)ledCharHandle;
            CyBle_GattcWriteCharacteristicValue(connectionHandle, &tempHandle);
            readByTypeReqParam.uuid = uuid;
            CyBle_GattcReadUsingCharacteristicUuid(connectionHandle, &readByTypeReqParam);
            //CyBle_GattcWriteCharacteristicDescriptors(connectionHandle, &tempHandle);
            CyBle_ProcessEvents();
        }
    }
}

/* [] END OF FILE */
