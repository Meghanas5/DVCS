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

#define CYBLE_MAX_ADV_DEVICES        10u

/* BLE State Macros used for LED status updates*/
#define BLE_DISCONNECTED				0x01
#define BLE_SCANNING					0x02
#define BLE_SERVICE_DISCOVERY			0x03
#define BLE_CONNECTED					0x04


uint8 ble_state = BLE_DISCONNECTED;


uint8 deviceConnected = 0;

CYBLE_GAP_BD_ADDR_T connectPeriphDevice;

uint8 periphAddress[6];
uint8 periphFound = 0;

uint8 addedDevices = 0;

uint8 address_store[10][6];


uint8 restartScanning = 0;


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
            //if (CYBLE_STATE_CONNECTED == CyBle_GetState())
            //{
            scanReport = *(CYBLE_GAPC_ADV_REPORT_T*) eventParam;
            HandleScanDevices(&scanReport);
            //}
            break;
        case CYBLE_EVT_GATT_CONNECT_IND:
            connectionHandle = *(CYBLE_CONN_HANDLE_T*)eventParam;
            break;
        case CYBLE_EVT_GAP_DEVICE_CONNECTED:
            apiResult = CyBle_GattcStartDiscovery(connectionHandle);
            ble_state = BLE_SERVICE_DISCOVERY;
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
            
            break;
        case CYBLE_EVT_GATTC_DISCOVERY_COMPLETE:
            deviceConnected = 1;
            ble_state = BLE_CONNECTED;
            break;
            
        default:
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
    //periphAddress = 0x00A050CC2313
    periphAddress[5] = 0x00;
    periphAddress[4] = 0xA0;
    periphAddress[3] = 0x50;
    periphAddress[2] = 0xCC;
    periphAddress[1] = 0x23;
    periphAddress[0] = 0x13;
    
    
    for(;;)
    {
        /* Place your application code here. */
        CyBle_ProcessEvents();
        if (periphFound)
        {
            CyBle_GapcConnectDevice(&connectPeriphDevice);
            
            CyBle_ProcessEvents();
            periphFound = 0;
        }
        if (deviceConnected)
        {
            
        }
        if (restartScanning)
        {
            restartScanning = 0;
            
            CyBle_GapcStartScan(CYBLE_SCANNING_FAST);
        }
    }
}

/* [] END OF FILE */
