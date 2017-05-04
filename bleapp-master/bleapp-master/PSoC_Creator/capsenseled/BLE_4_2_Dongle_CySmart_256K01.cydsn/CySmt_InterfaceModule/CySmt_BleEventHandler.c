/******************************************************************************
* File Name         : CySmt_BleEventHandler.c
* Description       : BLE component Event handler module.
*                     Prepares response data to be sent for each command and handles Async BLE events.
* Version           : 1.2
* Software Used     : PSoC Creator 3.3 CP2
* Compiler          : ARM GCC 4.9.3, ARM MDK Generic
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

#include "CySmt_protocol.h"
#include "CySmt_BleEventHandler.h"

#ifdef CYSMART_SUPPORT

/* Reason codes for Auth param info event */
#define AUTH_PARAM_NEGOTIATED (0u)
#define AUTH_COMPLETED (1u)

CYBLE_GATT_DB_ATTR_HANDLE_T cys_charEndHandle;
uint8 cysBdHandle;

CYBLE_GAP_IOCAP_T CySIoCap = CYBLE_IO_CAPABILITY;

/*******************************************************************************
* Function Name: Send_advt_report
********************************************************************************
*
* Summary:
* This function parses the advertisement report from stack and sends it to transport module
*
* Parameters:
*  CYBLE_GAPC_ADV_REPORT_T *scanResp: Pointer to scan response
*
* Return:
*  pass/fail status of Advertisement report parsing
*
* Theory:
*  NONE
*
* Side Effects:
*
* Note:
*
*******************************************************************************/
static CYBLE_API_RESULT_T Send_advt_report(CYBLE_GAPC_ADV_REPORT_T *scanResp)
{
    uint8 index = 0;
    uint8 advReportBuf[sizeof(CYBLE_GAPC_ADV_REPORT_T) + CYBLE_GAP_BD_ADDR_SIZE] = {0};

    advReportBuf[index++] = scanResp->eventType;
    
    /* Append BD address and size in response packet */
    memcpy(&advReportBuf[index], scanResp->peerBdAddr, CYBLE_GAP_BD_ADDR_SIZE);
    index += CYBLE_GAP_BD_ADDR_SIZE;

    advReportBuf[index++] = scanResp->peerAddrType;
    advReportBuf[index++] = scanResp->rssi;
    advReportBuf[index++] = scanResp->dataLen;

    CyS_SendEvent(EVT_SCAN_PROGRESS_RESULT, primaryCmd.opcode, scanResp->dataLen,
        index, advReportBuf);

    /* Send the packet raw data to TX buffer */
    if(0 != scanResp->dataLen)
    {
        TransmitAdditionalData(scanResp->data, scanResp->dataLen);
    }
    return CYBLE_ERROR_OK;
}

/*******************************************************************************
* Function Name: ReadByGroupEventHandler
********************************************************************************
*
* Summary:
* Parse and send responses to CMD_DISCOVER_ALL_PRIMARY_SERVICES
*
* Parameters:
* CYBLE_GATTC_READ_BY_GRP_RSP_PARAM_T *ReadByGrpResp
*
* Return:
*  true/false status for command completion
*
* Theory:
*  NONE
*
* Side Effects:
*
* Note: This function will be called repeatedly for each event occurrence.
*
*******************************************************************************/
static bool ReadByGroupEventHandler(CYBLE_GATTC_READ_BY_GRP_RSP_PARAM_T *ReadByGrpResp)
{
    /* Only CMD_DISCOVER_ALL_PRIMARY_SERVICES triggers this event */

    /* Each data element contains start and end handles along with UUID */
    uint8 UuidLen = ReadByGrpResp->attrData.length - sizeof(CYBLE_GATT_ATTR_HANDLE_RANGE_T);
    uint8 itemCount = (ReadByGrpResp->attrData.attrLen)/(ReadByGrpResp->attrData.length);
    CYBLE_DISC_SRVC_INFO_T* discSrvcInfo;
    uint8 UuidType = 0;
    uint8 count;

    switch(UuidLen)
    {
        case CYBLE_GATT_16_BIT_UUID_SIZE:
            UuidType = CYBLE_GATT_16_BIT_UUID_FORMAT;
            break;
        case CYBLE_GATT_128_BIT_UUID_SIZE:
            UuidType = CYBLE_GATT_128_BIT_UUID_FORMAT;
            break;
        default:
            /* No other types are expected */
            break;
    }

    /* After connection handle the list of items to be sent: Handle Range + UuidType + UUID Val */
    CyS_SendEvent(EVT_DISCOVER_ALL_PRIMARY_SERVICES_RESULT_PROGRESS, primaryCmd.opcode, 
                    (ReadByGrpResp->attrData.attrLen + (itemCount * sizeof(UuidType))),
                     sizeof(ReadByGrpResp->connHandle), (uint8 *)&ReadByGrpResp->connHandle);

    /* Send remaining responses in chunks */
    for(count = 0; count < itemCount; count++)
    {
        /* Send Start and End Handles */
        TransmitAdditionalData(&ReadByGrpResp->attrData.attrValue[
            count * (ReadByGrpResp->attrData.length)], sizeof(CYBLE_GATT_ATTR_HANDLE_RANGE_T));

        /* Insert UUID Type for each handle-value pair as per protocol */
        TransmitAdditionalData(&UuidType, sizeof(UuidType));

        /* Send UUID Value */
        TransmitAdditionalData(&ReadByGrpResp->attrData.attrValue[
            count * (ReadByGrpResp->attrData.length) + sizeof(CYBLE_GATT_ATTR_HANDLE_RANGE_T)],
            UuidLen);
    }

    /* Check if end handle occurs in last item to send-out command complete */
    discSrvcInfo = (CYBLE_DISC_SRVC_INFO_T*)&ReadByGrpResp->attrData.attrValue[
                            ReadByGrpResp->attrData.attrLen - ReadByGrpResp->attrData.length];

    /* Send command complete, if end-handle occurs */
    if(CYBLE_GATT_ATTR_HANDLE_END_RANGE == discSrvcInfo->range.endHandle)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*******************************************************************************
* Function Name: ReadUsingUUIDHandler
********************************************************************************
*
* Summary:
* Parse and send EVT_READ_USING_CHARACTERISTIC_UUID_RESPONSE events
*
* Parameters:
* CYBLE_GATTC_READ_BY_TYPE_RSP_PARAM_T *ReadByTypeResp
*
* Return:
*  NONE
*
* Theory:
*  NONE
*
* Side Effects:
*
* Note:
*
*******************************************************************************/
static void ReadUsingUUIDHandler(CYBLE_GATTC_READ_BY_TYPE_RSP_PARAM_T *ReadByTypeResp)
{
    /* Each data element contains Att (Attribute) handle along with UUID */
    uint16 ValueLen = ReadByTypeResp->attrData.length - sizeof(CYBLE_GATT_DB_ATTR_HANDLE_T);
    uint8 itemCount = (ReadByTypeResp->attrData.attrLen)/(ReadByTypeResp->attrData.length);
    uint8 count;

    /* After connection handle the list of items to be sent: Att Handle  + Val length + Val */
    CyS_SendEvent(EVT_READ_USING_CHARACTERISTIC_UUID_RESPONSE, primaryCmd.opcode, 
                    (ReadByTypeResp->attrData.attrLen + (itemCount * sizeof(ValueLen))), 
                    sizeof(ReadByTypeResp->connHandle), (uint8 *)&ReadByTypeResp->connHandle);

    /* Send remaining responses in chunks */
    for(count = 0; count < itemCount; count++)
    {
        /* Send Att Handle */
        TransmitAdditionalData(&ReadByTypeResp->attrData.attrValue[
            count * (ReadByTypeResp->attrData.length)], sizeof(CYBLE_GATT_DB_ATTR_HANDLE_T));

        /* Insert UUID Type as request by tool */
        TransmitAdditionalData((uint8*)&ValueLen, sizeof(ValueLen));

        /* Send UUID Value */
        TransmitAdditionalData(&ReadByTypeResp->attrData.attrValue[
            count * (ReadByTypeResp->attrData.length) + sizeof(CYBLE_GATT_DB_ATTR_HANDLE_T)],
            ValueLen);
    }
}

/*******************************************************************************
* Function Name: DiscoverAllCharacteristicsHandler
********************************************************************************
*
* Summary:
* Parse and send EVT_DISCOVER_ALL_CHARACTERISTICS_RESULT_PROGRESS events
*
* Parameters:
* CYBLE_GATTC_READ_BY_TYPE_RSP_PARAM_T *ReadByTypeResp
*
* Return:
*  true/false status for command completion
*
* Theory:
*  NONE
*
* Side Effects:
*
* Note: This function will be called repeatedly for each event occurrence.
*
*******************************************************************************/
static bool DiscoverAllCharacteristicsHandler(CYBLE_GATTC_READ_BY_TYPE_RSP_PARAM_T *ReadByTypeResp)
{
    /* Each data element contains Att handle along with UUID */
    uint8 itemCount = (ReadByTypeResp->attrData.attrLen)/(ReadByTypeResp->attrData.length);
    CYBLE_GATT_DB_ATTR_HANDLE_T valueHandle;
    uint8 UuidType = 0;
    uint8 UuidLen, ParamLen, count;

    /* Each item contains Attribute Handle + Characteristic Properties + 
                Characteristic Value Handle + Characteristic UUID */
    /* Calculate size of all parameters, except Characteristic UUID Value */
    ParamLen = sizeof(CYBLE_GATT_DB_ATTR_HANDLE_T) + sizeof(uint8) +
                sizeof(CYBLE_GATT_DB_ATTR_HANDLE_T);
    
    UuidLen = ReadByTypeResp->attrData.length - ParamLen;
    switch(UuidLen)
    {
        case CYBLE_GATT_16_BIT_UUID_SIZE:
            UuidType = CYBLE_GATT_16_BIT_UUID_FORMAT;
            break;
        case CYBLE_GATT_128_BIT_UUID_SIZE:
            UuidType = CYBLE_GATT_128_BIT_UUID_FORMAT;
            break;
        default:
            /* No other types are expected */
            break;
    }

    /* After connection handle the list of items to be sent: Att Handle  + UuidType + UUID Val */
    CyS_SendEvent(EVT_DISCOVER_ALL_CHARACTERISTICS_RESULT_PROGRESS, primaryCmd.opcode, 
                    (ReadByTypeResp->attrData.attrLen + (itemCount * sizeof(UuidType))),
                    sizeof(ReadByTypeResp->connHandle), (uint8 *)&ReadByTypeResp->connHandle);

    /* Send remaining responses in chunks */
    for(count = 0; count < itemCount; count++)
    {
        /* Send Attribute Handle + Characteristic Properties + Characteristic Value Handle */
        TransmitAdditionalData(&ReadByTypeResp->attrData.attrValue[
            count * (ReadByTypeResp->attrData.length)], ParamLen);

        /* Insert UUID Type as request by tool */
        TransmitAdditionalData(&UuidType, sizeof(UuidType));

        /* Send UUID Value */
        TransmitAdditionalData(&ReadByTypeResp->attrData.attrValue[
            count * (ReadByTypeResp->attrData.length) + ParamLen], UuidLen);
    }

    /* Get Att handle of last item in the list */
    valueHandle = (CYBLE_GATT_DB_ATTR_HANDLE_T)CyBle_Get16ByPtr(
                        &ReadByTypeResp->attrData.attrValue[
                            ReadByTypeResp->attrData.attrLen - ReadByTypeResp->attrData.length]);

    /* Check if last handle occurs in last item to send-out command complete */
    if(cys_charEndHandle == valueHandle)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*******************************************************************************
* Function Name: DiscoverCharacteristicsByUUIDHandler
********************************************************************************
*
* Summary:
* Parse and send EVT_DISCOVER_CHARACTERISTICS_BY_UUID_RESULT_PROGRESS events
*
* Parameters:
* CYBLE_GATTC_READ_BY_TYPE_RSP_PARAM_T *ReadByTypeResp
*
* Return:
*  true/false status for command completion
*
* Theory:
*  NONE
*
* Side Effects:
*
* Note: This function will be called repeatedly for each event occurrence.
*
*******************************************************************************/
static bool DiscoverCharacteristicsByUUIDHandler(CYBLE_GATTC_READ_BY_TYPE_RSP_PARAM_T *ReadByTypeResp)
{
    /* Each data element contains Att handle along with UUID */
    uint8 itemCount = (ReadByTypeResp->attrData.attrLen)/(ReadByTypeResp->attrData.length);
    uint8 UuidLen, ParamLen, count;

    /* Each item contains Attribute Handle + Characteristic Properties + 
            Characteristic Value Handle + Characteristic UUID */
    /* Calculate size of all parameters, except Characteristic UUID Value */
    ParamLen = sizeof(CYBLE_GATT_DB_ATTR_HANDLE_T) + sizeof(uint8) + 
                sizeof(CYBLE_GATT_DB_ATTR_HANDLE_T);

    UuidLen = ReadByTypeResp->attrData.length - ParamLen;

    /* UuidType + UUID Val are not request, send out all other parameters */
    CyS_SendEvent(EVT_DISCOVER_CHARACTERISTICS_BY_UUID_RESULT_PROGRESS, primaryCmd.opcode, 
                    (ReadByTypeResp->attrData.attrLen - (UuidLen * itemCount)),
                    sizeof(ReadByTypeResp->connHandle), (uint8 *)&ReadByTypeResp->connHandle);

    /* Send remaining responses in chunks */
    for(count = 0; count < itemCount; count++)
    {
        /* Send Attribute Handle + Characteristic Properties + Characteristic Value Handle */
        TransmitAdditionalData(&ReadByTypeResp->attrData.attrValue[
            count * (ReadByTypeResp->attrData.length)], ParamLen);
    }

    /* Check if last handle occurs in  item to send-out command complete */
    if(cys_charEndHandle == CyBle_Get16ByPtr(&ReadByTypeResp->attrData.attrValue[
                ReadByTypeResp->attrData.attrLen - ReadByTypeResp->attrData.length]))
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*******************************************************************************
* Function Name: FindIncludedServicesHandler
********************************************************************************
*
* Summary:
* Parse and send EVT_FIND_INDLUDED_SERVICES_RESULT_PROGRESS events
*
* Parameters:
* CYBLE_GATTC_READ_BY_TYPE_RSP_PARAM_T *ReadByTypeResp
*
* Return:
*  true/false status for command completion
*
* Theory:
*  NONE
*
* Side Effects:
*
* Note: This function will be called repeatedly for each event occurrence.
*
*******************************************************************************/
static bool FindIncludedServicesHandler(CYBLE_GATTC_READ_BY_TYPE_RSP_PARAM_T *ReadByTypeResp)
{
    /* Each data element contains Att handle along with UUID */
    uint8 itemCount = (ReadByTypeResp->attrData.attrLen)/(ReadByTypeResp->attrData.length);
    CYBLE_GATT_DB_ATTR_HANDLE_T valueHandle;
    uint8 UuidType = 0;
    uint8 ParamLen, index;

    /* UUID Type need to be inserted for each item */
    uint16 payloadSize = ReadByTypeResp->attrData.attrLen + (itemCount * sizeof(UuidType));

    /* Each item contains Attribute Handle + Start Handle + End Handle + optional UUID */
    /* Calculate size of all parameters, except UUID Value */
    ParamLen = sizeof(CYBLE_GATT_DB_ATTR_HANDLE_T) + sizeof(CYBLE_GATT_ATTR_HANDLE_RANGE_T);
    
    if(ParamLen == ReadByTypeResp->attrData.length)
    {
        /* Data element doesn't contain UUID, so UUID is of 128 bit */
        UuidType = CYBLE_GATT_128_BIT_UUID_FORMAT;
        /* Insert zeros as 128 bit UUID */
        payloadSize += itemCount * CYBLE_GATT_128_BIT_UUID_SIZE;
    }
    else
    {
        UuidType = CYBLE_GATT_16_BIT_UUID_FORMAT;
    }

    /* After connection handle the list of items to be sent: Att Handle  + UuidType + UUID Val */
    CyS_SendEvent(EVT_FIND_INDLUDED_SERVICES_RESULT_PROGRESS, primaryCmd.opcode, payloadSize,
                    sizeof(ReadByTypeResp->connHandle), (uint8 *)&ReadByTypeResp->connHandle);

    /* Send remaining responses in chunks */
    for(index = 0; index < itemCount; index++)
    {
        /* Send Attribute Handle + Characteristic Properties + Characteristic Value Handle */
        TransmitAdditionalData(&ReadByTypeResp->attrData.attrValue[
            index * (ReadByTypeResp->attrData.length)], ParamLen);

        /* Insert UUID Type as request by tool */
        TransmitAdditionalData(&UuidType, sizeof(UuidType));

        /* Send UUID only if it is 16 bit type, else send zeros */
        if(CYBLE_GATT_16_BIT_UUID_FORMAT == UuidType)
        {
            /* Send UUID Value */
            TransmitAdditionalData(&ReadByTypeResp->attrData.attrValue[
                index * (ReadByTypeResp->attrData.length) + ParamLen], CYBLE_GATT_16_BIT_UUID_SIZE);
        }
        else
        {
            /* Send zeros for 128 bit UUID */
            uint8 uuid[CYBLE_GATT_128_BIT_UUID_SIZE] = {0};
            TransmitAdditionalData(uuid, sizeof(uuid));
        }
    }

    /* Get Att handle of last item in the list */
    index = ReadByTypeResp->attrData.attrLen - ReadByTypeResp->attrData.length +
                sizeof(CYBLE_GATT_ATTR_HANDLE_RANGE_T);

    valueHandle = (CYBLE_GATT_DB_ATTR_HANDLE_T)CyBle_Get16ByPtr(
                    &ReadByTypeResp->attrData.attrValue[index]);

    /* Check if last handle occurs in  item to send-out command complete */
    if(cys_charEndHandle == valueHandle)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*******************************************************************************
* Function Name: ReadByTypeEventHandler
********************************************************************************
*
* Summary:
* Common handler function for all events with CYBLE_GATTC_READ_BY_TYPE_RSP_PARAM_T
*  Invokes corresponding function to process command responses
*
* Parameters:
* CYBLE_GATTC_READ_BY_TYPE_RSP_PARAM_T *ReadByTypeResp
*
* Return:
*  true/false status for command completion
*
* Theory:
*  NONE
*
* Side Effects:
*
* Note: This function will be called repeatedly for each event occurrence.
*
*******************************************************************************/
static bool ReadByTypeEventHandler(CYBLE_GATTC_READ_BY_TYPE_RSP_PARAM_T *ReadByTypeResp)
{
    bool retval = false;
    
    switch(primaryCmd.opcode)
    {
        case CMD_READ_USING_CHARACTERISTIC_UUID:
            ReadUsingUUIDHandler(ReadByTypeResp);
            retval = true;
            break;

        case CMD_DISCOVER_ALL_CHARACTERISTICS:
            retval = DiscoverAllCharacteristicsHandler(ReadByTypeResp);
            break;

        case CMD_DISCOVER_CHARACTERISTICS_BY_UUID:
            retval = DiscoverCharacteristicsByUUIDHandler(ReadByTypeResp);
            break;

        case CMD_FIND_INCLUDED_SERVICES:
            retval = FindIncludedServicesHandler(ReadByTypeResp);
            break;

        default:
            /* Not expected to come here */
            break;
    }
    return retval;
}

/*******************************************************************************
* Function Name: FindByTypeEventHandler
********************************************************************************
*
* Summary:
* Parse and send CMD_DISCOVER_PRIMARY_SERVICES_BY_UUID response events
*
* Parameters:
* CYBLE_GATTC_FIND_BY_TYPE_RSP_PARAM_T *FindByTypeResp
*
* Return:
*  true/false status for command completion
*
* Theory:
*  NONE
*
* Side Effects:
*
* Note: This function will be called repeatedly for each event occurrence.
*
*******************************************************************************/
static bool FindByTypeEventHandler(CYBLE_GATTC_FIND_BY_TYPE_RSP_PARAM_T *FindByTypeResp)
{
    /* Only CMD_DISCOVER_PRIMARY_SERVICES_BY_UUID triggers this event */
    
    /* Send Handle ranges as second packet */
    CyS_SendEvent(EVT_DISCOVER_PRIMARY_SERVICES_BY_UUID_RESULT_PROGRESS, primaryCmd.opcode,
                    ((FindByTypeResp->count) * sizeof(CYBLE_GATT_ATTR_HANDLE_RANGE_T)),
                    sizeof(FindByTypeResp->connHandle), (uint8 *)&FindByTypeResp->connHandle);

    /* Send the packet raw data to TX buffer */
    TransmitAdditionalData((uint8 *)FindByTypeResp->range, 
                            ((FindByTypeResp->count) * sizeof(CYBLE_GATT_ATTR_HANDLE_RANGE_T)));

    /* Send command complete if end handle occurs */
    if(CYBLE_GATT_ATTR_HANDLE_END_RANGE == FindByTypeResp->range[FindByTypeResp->count-1].endHandle)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*******************************************************************************
* Function Name: FindInfoEventHandler
********************************************************************************
*
* Summary:
* Parse and send CMD_DISCOVER_ALL_CHARACTERISTIC_DESCRIPTORS response events
*
* Parameters:
* CYBLE_GATTC_FIND_INFO_RSP_PARAM_T *FindInfoResp
*
* Return:
*  true/false status for command completion
*
* Theory:
*  NONE
*
* Side Effects:
*
* Note: This function will be called repeatedly for each event occurrence.
*
*******************************************************************************/
static bool FindInfoEventHandler(CYBLE_GATTC_FIND_INFO_RSP_PARAM_T *FindInfoResp)
{
    /* Only CMD_DISCOVER_ALL_CHARACTERISTIC_DESCRIPTORS triggers this event */
    uint16 valueLen = 0;
    uint16 pairCount, index;
    CYBLE_GATT_DB_ATTR_HANDLE_T* AttHandle;

    switch(FindInfoResp->uuidFormat)
    {
        case CYBLE_GATT_128_BIT_UUID_FORMAT:
            valueLen = CYBLE_GATT_128_BIT_UUID_SIZE;
            break;
        case CYBLE_GATT_16_BIT_UUID_FORMAT:
            valueLen = CYBLE_GATT_16_BIT_UUID_SIZE;
            break;
        default:
            /* No other types are expected */
            break;
    }

    /* Calculate total count of handle-value pair list */
    pairCount = (FindInfoResp->handleValueList.byteCount)/(sizeof(CYBLE_GATT_DB_ATTR_HANDLE_T) +
                    valueLen);

    /* After connection handle the list of items to be sent: Descriptor Handle + UuidType + UUID Val */
    CyS_SendEvent(EVT_DISCOVER_ALL_CHARACTERISTIC_DESCRIPTORS_RESULT_PROGRESS, primaryCmd.opcode, 
        (pairCount * (sizeof(CYBLE_GATT_DB_ATTR_HANDLE_T) + sizeof(FindInfoResp->uuidFormat) +
            valueLen)), sizeof(FindInfoResp->connHandle), (uint8 *)&FindInfoResp->connHandle);

    /* Send remaining responses in chunks */
    for(index = 0; index < pairCount; index++)
    {
        /* Send Handle */
        TransmitAdditionalData(&FindInfoResp->handleValueList.list[
            index * (sizeof(CYBLE_GATT_DB_ATTR_HANDLE_T) + valueLen)], sizeof(CYBLE_GATT_DB_ATTR_HANDLE_T));

        /* Insert UUID Type as request by tool */
        TransmitAdditionalData(&FindInfoResp->uuidFormat, sizeof(FindInfoResp->uuidFormat));
        /* Send UUID Value */
        TransmitAdditionalData(&FindInfoResp->handleValueList.list[
                (index * (sizeof(CYBLE_GATT_DB_ATTR_HANDLE_T) + valueLen)) +
                 sizeof(CYBLE_GATT_DB_ATTR_HANDLE_T)], valueLen);
    }

    /* Get Att handle of last item in the list */
    AttHandle = (CYBLE_GATT_DB_ATTR_HANDLE_T*)&FindInfoResp->handleValueList.list[
                    FindInfoResp->handleValueList.byteCount - sizeof(CYBLE_GATT_DB_ATTR_HANDLE_T) - valueLen];
    /* Send command complete if last requested handle occurs */
    if(cys_charEndHandle == *AttHandle)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*******************************************************************************
* Function Name: ReadRspEventHandler
********************************************************************************
*
* Summary:
* Common handler function for events with CYBLE_GATTC_READ_RSP_PARAM_T
*  Prepares and sends corresponding response as per the current command
*
* Parameters:
* CYBLE_GATTC_READ_RSP_PARAM_T *ReadResp
*
* Return:
*  NONE
*
* Theory:
*  NONE
*
* Side Effects:
*
* Note:
*
*******************************************************************************/
static void ReadRspEventHandler(CYBLE_GATTC_READ_RSP_PARAM_T *ReadResp)
{
    Event event = EVT_REPORT_STACK_MISC_STATUS;
    uint16 currentMtuSize = 1;
    bool sendComplete = false;
    
    /* Get current negotiated */
    (void)CyBle_GattGetMtuSize(&currentMtuSize);

    switch(primaryCmd.opcode)
    {
        case CMD_READ_CHARACTERISTIC_VALUE:
            event = EVT_READ_CHARACTERISTIC_VALUE_RESPONSE;
            sendComplete = true;
            break;

        case CMD_READ_LONG_CHARACTERISTIC_VALUES:
            event = EVT_READ_LONG_CHARACTERISTIC_VALUE_RESPONSE;
            /* For read long values and descriptors, send complete only if value size is less than MTU-1 */
            if(ReadResp->value.len < (currentMtuSize -1))
            {
                sendComplete = true;
            }
            break;
        
        case CMD_READ_MULTIPLE_CHARACTERISTIC_VALUES:
            event = EVT_READ_MULTIPLE_CHARACTERISTIC_VALUES_RESPONSE;
            sendComplete = true;
            break;

        case CMD_READ_CHARACTERISTIC_DESCRIPTOR:
            event = EVT_READ_CHARACTERISTIC_DESCRIPTOR_RESPONSE;
            sendComplete = true;
            break;

        case CMD_READ_LONG_CHARACTERISTIC_DESCRIPTOR:
            event = EVT_READ_LONG_CHARACTERISTIC_DESCRIPTOR_RESPONSE;
            /* For read long values and descriptors, send complete only if value size is less than MTU-1 */
            if(ReadResp->value.len < (currentMtuSize -1))
            {
                sendComplete = true;
            }
            break;
            
        default:
            /* Other commands are invalid for this event */
            return;
    }

    /* Send value length and value data as payload */
    CyS_SendEvent(event, primaryCmd.opcode, ReadResp->value.len + sizeof(ReadResp->value.len),
                    sizeof(CYBLE_CONN_HANDLE_T), (uint8 *)&ReadResp->connHandle);

    TransmitAdditionalData((uint8*)&ReadResp->value.len, sizeof(ReadResp->value.len));
    TransmitAdditionalData(ReadResp->value.val, ReadResp->value.len);

    /* For read long values and descriptors, send complete only if value size is less than MTU-1 */
    if(sendComplete)
    {
        CySmt_SendCommandComplete(primaryCmd.opcode, CYBLE_ERROR_OK);
    }
}

/*******************************************************************************
* Function Name: Gattc_NotificationHandler
********************************************************************************
*
* Summary:
* Parse and send notification responses
*
* Parameters:
* CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *NotifyParams
*
* Return:
*  NONE
*
* Theory:
*  NONE
*
* Side Effects:
*
* Note:
*
*******************************************************************************/
static void Gattc_NotificationHandler(CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *NotifyParams)
{
    uint16 payloadSize = sizeof(CYBLE_GATT_DB_ATTR_HANDLE_T) +
                        sizeof(NotifyParams->handleValPair.value.len) +
                        NotifyParams->handleValPair.value.len;

    CyS_SendEvent(EVT_CHARACTERISTIC_VALUE_NOTIFICATION, 0, payloadSize,
                    sizeof(CYBLE_CONN_HANDLE_T), (uint8 *)&NotifyParams->connHandle);

    TransmitAdditionalData((uint8*)&NotifyParams->handleValPair.attrHandle,
                            sizeof(NotifyParams->handleValPair.attrHandle));
    TransmitAdditionalData((uint8*)&NotifyParams->handleValPair.value.len,
                            sizeof(NotifyParams->handleValPair.value.len));
    TransmitAdditionalData(NotifyParams->handleValPair.value.val,
                            NotifyParams->handleValPair.value.len);
}

/*******************************************************************************
* Function Name: Gattc_IndicationHandler
********************************************************************************
*
* Summary:
* Parse and send Indication responses
*
* Parameters:
* CYBLE_GATTC_HANDLE_VALUE_IND_PARAM_T *IndParam
*
* Return:
*  NONE
*
* Theory:
*  NONE
*
* Side Effects:
*
* Note:
*
*******************************************************************************/
static void Gattc_IndicationHandler(CYBLE_GATTC_HANDLE_VALUE_IND_PARAM_T *IndParam)
{
     /* Send confirmation for indication event and pass-on the result */
    uint16 result = CyBle_GattcConfirmation(IndParam->connHandle);
    uint16 payloadSize = sizeof(CYBLE_GATT_DB_ATTR_HANDLE_T) +
                        sizeof(result) +
                        sizeof(IndParam->handleValPair.value.len) +
                        IndParam->handleValPair.value.len;
    
    CyS_SendEvent(EVT_CHARACTERISTIC_VALUE_INDICATION, 0, payloadSize,
                    sizeof(CYBLE_CONN_HANDLE_T), (uint8 *)&IndParam->connHandle);

    TransmitAdditionalData((uint8*)&IndParam->handleValPair.attrHandle,
                            sizeof(IndParam->handleValPair.attrHandle));
    TransmitAdditionalData((uint8*)&result, sizeof(uint16));
    TransmitAdditionalData((uint8*)&IndParam->handleValPair.value.len,
                            sizeof(IndParam->handleValPair.value.len));
    TransmitAdditionalData(IndParam->handleValPair.value.val,
                            IndParam->handleValPair.value.len);
}

/*******************************************************************************
* Function Name: CyS_GenericEventHandler
********************************************************************************
*
* Summary:
*  This function provides handling of events from the CYBLE Component and sending responses
*
* Parameters:  
*  uint8 event:       Event from the CYBLE component
*  void* eventParams: A structure instance for corresponding event type. The 
*                     list of event structure is described in the component data sheet.
*
* Return: 
*  None
*
*******************************************************************************/
void CyS_GenericEventHandler(uint32 event, void *eventParam)
{
    switch(event)
    {
        /**********************************************************
            *                       General Events
            ***********************************************************/
        case CYBLE_EVT_STACK_ON:
             {
                CYBLE_BLESS_CLK_CFG_PARAMS_T bleSSClkConfig;
                if(CYBLE_ERROR_OK == CyBle_GetBleClockCfgParam(&bleSSClkConfig))
                {
                    /* Set clock PPM as per hardware configuration */
                    bleSSClkConfig.bleLlSca = CYBLE_LL_SCA_000_TO_020_PPM;
                    CyBle_SetBleClockCfgParam(&bleSSClkConfig);
                }
                /* This event received when component is Started */
                isCySBleStackOn = true;

                CySmt_SendCommandComplete(CMD_INIT_BLE_STACK, CyBle_GapSetIoCap(CySIoCap));
            }
            break;

        case CYBLE_EVT_TIMEOUT:
            /* Check and handle the time-out reasons */
            switch((CYBLE_TO_REASON_CODE_T)*(uint8 *)eventParam)
                {
                  case CYBLE_GAP_ADV_MODE_TO:
                      /* Not valid for client */
                      break;

                  case CYBLE_GAP_SCAN_TO:
                      if( (primaryCmdInProgress) && (CMD_START_SCAN  == primaryCmd.opcode) )
                      {
                          CyS_SendEvent(EVT_SCAN_STOPPED_NOTIFICATION, 0, 0, 0, 0);
                          CySmt_SendCommandComplete(primaryCmd.opcode, CYBLE_ERROR_OK);
                      }
                      break;

                  case CYBLE_GATT_RSP_TO:
                      if(GATT_CMD_IN_PROGRESS)
                      {
                          CyS_SendEvent(EVT_GATT_TIMEOUT_NOTIFICATION, primaryCmd.opcode, 0, 0, 0);
                          CySmt_SendCommandComplete(primaryCmd.opcode, CYBLE_ERROR_OK);
                      }
                      break;

                  default:
                      /* No other time-outs */
                      break;
                }
            break;

        case CYBLE_EVT_HARDWARE_ERROR:    /* This event indicates that some internal error has occurred. */
            {
                /* This event doesn't have any event data */
                uint16 EvtParamSize = 0;
                CyS_SendEvent(EVT_REPORT_STACK_MISC_STATUS, 0, sizeof(EvtParamSize) + EvtParamSize,
                                sizeof(uint16), (uint8*)&event);
            
                /* Send size of parameters for this stack event */
                TransmitAdditionalData((uint8*)&EvtParamSize, sizeof(EvtParamSize));
            }
            break;

        case CYBLE_EVT_HCI_STATUS:
        case CYBLE_EVT_STACK_BUSY_STATUS:
        case CYBLE_EVT_GAP_ENCRYPT_CHANGE:
            {
                /* All these events contain, parameters of 1 byte length */
                uint16 EvtParamSize = sizeof(uint8);

                /* Send response event with stack event code */
                CyS_SendEvent(EVT_REPORT_STACK_MISC_STATUS, 0, sizeof(EvtParamSize) + EvtParamSize,
                                sizeof(uint16), (uint8*)&event);

                /* Send size of parameters for this stack event */
                TransmitAdditionalData((uint8*)&EvtParamSize, sizeof(EvtParamSize));

                /* Send parameters of this stack event */
                TransmitAdditionalData((uint8*)eventParam, EvtParamSize);
            }
            break;

        case CYBLE_EVT_L2CAP_COMMAND_REJ:
            {
                uint16 EvtParamSize = sizeof(CYBLE_L2CAP_COMMAND_REJ_REASON_T);
                CyS_SendEvent(EVT_REPORT_STACK_MISC_STATUS, 0, sizeof(EvtParamSize) + EvtParamSize,
                                sizeof(uint16), (uint8*)&event);
            
                /* Send size of parameters for this stack event */
                TransmitAdditionalData((uint8*)&EvtParamSize, sizeof(EvtParamSize));

                /* Send parameters of this stack event */
                TransmitAdditionalData((uint8*)eventParam, EvtParamSize);
            }
            break;

        /**********************************************************
            *                       GAP Events
            ***********************************************************/
        case CYBLE_EVT_GAPC_SCAN_PROGRESS_RESULT:
            if(CMD_START_SCAN  == primaryCmd.opcode)
            {
                CYBLE_API_RESULT_T status = Send_advt_report((CYBLE_GAPC_ADV_REPORT_T *)eventParam);

                /* Send complete with internal error, if the Advertisement report is not correct */
                if(CYBLE_ERROR_OK != status)
                {
                    CySmt_SendCommandComplete(primaryCmd.opcode, status);
                }
                else
                {
                    /* Wait for next progress result event */
                }
            }
            break;

        case CYBLE_EVT_GAP_AUTH_REQ:
            CyS_SendEvent(EVT_PAIRING_REQUEST_RECEIVED_NOTIFICATION, 0, sizeof(CYBLE_GAP_AUTH_INFO_T),
                            sizeof(cyBle_connHandle), (uint8*)&cyBle_connHandle);
            TransmitAdditionalData((uint8*)eventParam, sizeof(CYBLE_GAP_AUTH_INFO_T));
            break;

        case CYBLE_EVT_GAP_PASSKEY_ENTRY_REQUEST:
            CyS_SendEvent(EVT_PASSKEY_ENTRY_REQUEST, primaryCmd.opcode, 0,
                            sizeof(cyBle_connHandle), (uint8*)&cyBle_connHandle);
            break;

        case CYBLE_EVT_GAP_PASSKEY_DISPLAY_REQUEST:
            CyS_SendEvent(EVT_PASSKEY_DISPLAY_REQUEST, primaryCmd.opcode, sizeof(uint32),
                            sizeof(cyBle_connHandle), (uint8*)&cyBle_connHandle);
            TransmitAdditionalData((uint8*)eventParam, sizeof(uint32));
            break;

        case CYBLE_EVT_GAP_KEYINFO_EXCHNGE_CMPLT:
            {
                /* Send-out IRK keys to be stored by tool */
                CYBLE_GAP_SMP_KEY_DIST_T *smpKeyDist = (CYBLE_GAP_SMP_KEY_DIST_T*)eventParam;
                uint16 EvtParamSize = sizeof(smpKeyDist->irkInfo);
                CyS_SendEvent(EVT_REPORT_STACK_MISC_STATUS, 0, sizeof(EvtParamSize) + EvtParamSize,
                                sizeof(uint16), (uint8*)&event);
                TransmitAdditionalData((uint8*)&EvtParamSize, sizeof(EvtParamSize));
                TransmitAdditionalData(smpKeyDist->irkInfo, EvtParamSize);
            }
            break;

        case CYBLE_EVT_GAP_SMP_NEGOTIATED_AUTH_INFO:
            {
                uint8 reason = AUTH_PARAM_NEGOTIATED;

                CyS_SendEvent(EVT_NEGOTIATED_PAIRING_PARAMETERS, 0, sizeof(CYBLE_GAP_AUTH_INFO_T) + sizeof(reason),
                                sizeof(cyBle_connHandle), (uint8*)&cyBle_connHandle);
                TransmitAdditionalData(&reason, sizeof(reason));
                TransmitAdditionalData((uint8*)eventParam, sizeof(CYBLE_GAP_AUTH_INFO_T));
            }
            break;

        case CYBLE_EVT_GAP_AUTH_COMPLETE:
            {
                uint8 reason = AUTH_COMPLETED;

                CyS_SendEvent(EVT_NEGOTIATED_PAIRING_PARAMETERS, 0, sizeof(CYBLE_GAP_AUTH_INFO_T) + sizeof(reason),
                                sizeof(cyBle_connHandle), (uint8*)&cyBle_connHandle);
                TransmitAdditionalData(&reason, sizeof(reason));
                TransmitAdditionalData((uint8*)eventParam, sizeof(CYBLE_GAP_AUTH_INFO_T));

                CySmt_SendCommandComplete(primaryCmd.opcode, CYBLE_ERROR_OK);
            }
            break;

        case CYBLE_EVT_GAP_AUTH_FAILED:
            {
                /* Current command processing is complete */
                primaryCmdInProgress = false;

                CyS_SendEvent(EVT_AUTHENTICATION_ERROR_NOTIFICATION, primaryCmd.opcode,
                            sizeof(uint8), sizeof(cyBle_connHandle), (uint8*)&cyBle_connHandle);
                TransmitAdditionalData((uint8*)eventParam, sizeof(CYBLE_GAP_AUTH_FAILED_REASON_T));
            }
            break;

        case CYBLE_EVT_GATT_DISCONNECT_IND:
            break;

        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
            /* Send-out local stored handle as current stack supports only single connection */
            cyBle_connHandle.bdHandle = cysBdHandle;

            CyS_SendEvent(EVT_CONNECTION_TERMINATED_NOTIFICATION, 0, sizeof(uint8),
                            sizeof(cyBle_connHandle), (uint8*)&cyBle_connHandle);
            TransmitAdditionalData(eventParam, sizeof(uint8));

            /* Send command complete for terminate connection */
            if( (primaryCmdInProgress) && (
                 (CMD_TERMINATE_CONNECTION  == primaryCmd.opcode) ||
                 (CMD_TERMINATE_CONNECTION  == secondaryCmd.opcode)))
            {
                /* Terminate connection can be both primary or secondary, 
                             so send the op-code instead of using command structure */
                CySmt_SendCommandComplete(CMD_TERMINATE_CONNECTION, CYBLE_ERROR_OK);
            }
            break;

        case CYBLE_EVT_GAP_DEVICE_CONNECTED:
            {
                CYBLE_GAP_CONN_PARAM_UPDATED_IN_CONTROLLER_T *connParamUpdateResp = 
                    (CYBLE_GAP_CONN_PARAM_UPDATED_IN_CONTROLLER_T*)eventParam;

                uint16 payloadSize = sizeof(connParamUpdateResp->status) + sizeof(connParamUpdateResp->connIntv) +
                                    sizeof(connParamUpdateResp->connLatency)+ sizeof(connParamUpdateResp->supervisionTO);

                CyS_SendEvent(EVT_CURRENT_CONNECTION_PARAMETERS_NOTIFICATION, 0,
                    payloadSize, sizeof(CYBLE_CONN_HANDLE_T), (uint8*)&cyBle_connHandle);

                /* Send each parameter one by one */
                TransmitAdditionalData(&connParamUpdateResp->status,
                sizeof(connParamUpdateResp->status));
                TransmitAdditionalData((uint8*)&connParamUpdateResp->connIntv,
                    sizeof(connParamUpdateResp->connIntv));
                TransmitAdditionalData((uint8*)&connParamUpdateResp->connLatency,
                    sizeof(connParamUpdateResp->connLatency));
                TransmitAdditionalData((uint8*)&connParamUpdateResp->supervisionTO,
                    sizeof(connParamUpdateResp->supervisionTO));

                CySmt_SendCommandComplete(primaryCmd.opcode, CYBLE_ERROR_OK);
            }
            break;

        case CYBLE_EVT_GAP_CONNECTION_UPDATE_COMPLETE:
            {
                CYBLE_GAP_CONN_PARAM_UPDATED_IN_CONTROLLER_T *connParamUpdateResp = 
                    (CYBLE_GAP_CONN_PARAM_UPDATED_IN_CONTROLLER_T*)eventParam;

                uint16 payloadSize = sizeof(connParamUpdateResp->status) + sizeof(connParamUpdateResp->connIntv) +
                                    sizeof(connParamUpdateResp->connLatency)+ sizeof(connParamUpdateResp->supervisionTO);

                CyS_SendEvent(EVT_CURRENT_CONNECTION_PARAMETERS_NOTIFICATION, 0,
                    payloadSize, sizeof(CYBLE_CONN_HANDLE_T), (uint8*)&cyBle_connHandle);

                /* Send each parameter one by one */
                TransmitAdditionalData(&connParamUpdateResp->status,
                sizeof(connParamUpdateResp->status));
                TransmitAdditionalData((uint8*)&connParamUpdateResp->connIntv,
                    sizeof(connParamUpdateResp->connIntv));
                TransmitAdditionalData((uint8*)&connParamUpdateResp->connLatency,
                    sizeof(connParamUpdateResp->connLatency));
                TransmitAdditionalData((uint8*)&connParamUpdateResp->supervisionTO,
                    sizeof(connParamUpdateResp->supervisionTO));

                CySmt_SendCommandComplete(primaryCmd.opcode, CYBLE_ERROR_OK);
            }
            break;

        case CYBLE_EVT_L2CAP_CONN_PARAM_UPDATE_REQ:
            {
                CYBLE_GAP_CONN_UPDATE_PARAM_T *connParamUpdateReq = 
                    (CYBLE_GAP_CONN_UPDATE_PARAM_T*)eventParam;

                CyS_SendEvent(EVT_UPDATE_CONNECTION_PARAMETERS_NOTIFICATION, 0,
                            sizeof(CYBLE_GAP_CONN_UPDATE_PARAM_T),
                            sizeof(CYBLE_CONN_HANDLE_T), (uint8*)&cyBle_connHandle);

                /* Send each parameter one by one */
                TransmitAdditionalData((uint8*)&connParamUpdateReq->connIntvMin,
                sizeof(connParamUpdateReq->connIntvMin));
                TransmitAdditionalData((uint8*)&connParamUpdateReq->connIntvMax,
                    sizeof(connParamUpdateReq->connIntvMax));
                TransmitAdditionalData((uint8*)&connParamUpdateReq->connLatency,
                    sizeof(connParamUpdateReq->connLatency));
                TransmitAdditionalData((uint8*)&connParamUpdateReq->supervisionTO,
                    sizeof(connParamUpdateReq->supervisionTO));
            }
            break;

        case CYBLE_EVT_L2CAP_CONN_PARAM_UPDATE_RSP:
            /* Send complete with error code, if parameter update is rejected */
            if(CYBLE_L2CAP_CONN_PARAM_REJECTED == CyBle_Get16ByPtr(eventParam))
            {
                CySmt_SendCommandComplete(primaryCmd.opcode, CYBLE_ERROR_OK);
            }
            break;

        case CYBLE_EVT_GAPC_SCAN_START_STOP:
            {
                uint8 status = *(uint8 *)eventParam;
                /* If failure is returned for start scan, send complete with error code */
                if(CMD_STOP_SCAN  == secondaryCmd.opcode)
                {
                    /*Stop scan command will also close the start scan command*/
                    primaryCmdInProgress = false;
                    CyS_SendEvent(EVT_SCAN_STOPPED_NOTIFICATION, 0, 0, 0, 0);
                    CySmt_SendCommandComplete(secondaryCmd.opcode, (uint16)status);
                    memset((uint8*)&secondaryCmd, 0, sizeof(secondaryCmd));
                }

                if( (CMD_START_SCAN  == primaryCmd.opcode) && (0 != status) )
                {
                    CyS_SendEvent(EVT_SCAN_STOPPED_NOTIFICATION, 0, 0, 0, 0);
                    CySmt_SendCommandComplete(primaryCmd.opcode, (uint16)status);
                }
                else
                {
                    /* Ignore this event */
                }
            }
            break;

#if (CYBLE_M0S8BLESS_VERSION > 1)    /* BLE256DMA and later */

        case CYBLE_EVT_GAP_NUMERIC_COMPARISON_REQUEST:
            {
                CyS_SendEvent(EVT_NUMERIC_COMPARISON_REQUEST, primaryCmd.opcode, sizeof(uint32),
                                sizeof(cyBle_connHandle), (uint8*)&cyBle_connHandle);
                TransmitAdditionalData((uint8*)eventParam, sizeof(uint32));
            }
            break;

        case CYBLE_EVT_GAP_KEYPRESS_NOTIFICATION:
            {
                uint16 EvtParamSize = sizeof(CYBLE_GAP_KEYPRESS_NOTIFY_TYPE);
                CyS_SendEvent(EVT_REPORT_STACK_MISC_STATUS, 0, sizeof(EvtParamSize) + EvtParamSize,
                                sizeof(uint16), (uint8*)&event);
            
                /* Send size of parameters for this stack event */
                TransmitAdditionalData((uint8*)&EvtParamSize, sizeof(EvtParamSize));

                /* Send parameters of this stack event */
                TransmitAdditionalData((uint8*)eventParam, EvtParamSize);
            }
            break;

        case CYBLE_EVT_GAP_DATA_LENGTH_CHANGE:
            {
                uint16 payloadSize = sizeof(CYBLE_GAP_CONN_DATA_LENGTH_T);

                CyS_SendEvent(EVT_DATA_LENGTH_CHANGED_NOTIFICATION, 0, payloadSize,
                                sizeof(cyBle_connHandle), (uint8*)&cyBle_connHandle);
                TransmitAdditionalData((uint8*)eventParam, payloadSize);
            }
            break;

        case CYBLE_EVT_GAP_ENHANCE_CONN_COMPLETE:
            {
                CYBLE_GAP_ENHANCE_CONN_COMPLETE_T *connParam = (CYBLE_GAP_ENHANCE_CONN_COMPLETE_T*)eventParam;
                /* Send the addr Type as random to frame CySmart response */
                uint8 addrType = CYBLE_GAP_ADDR_TYPE_RANDOM;

                uint16 payloadSize = sizeof(CYBLE_CONN_HANDLE_T) +
                                     sizeof(connParam->role) + sizeof(connParam->masterClockAccuracy) +
                                     (sizeof(CYBLE_GAP_BD_ADDR_T) * 3) + sizeof(connParam->connIntv) +
                                     sizeof(connParam->connLatency)+ sizeof(connParam->supervisionTo);

                CyS_SendEvent(EVT_ENHANCED_CONNECTION_COMPLETE, primaryCmd.opcode,
                    payloadSize, sizeof(connParam->status), &connParam->status);

                /* Send each parameter one by one */
                TransmitAdditionalData((uint8*)&cyBle_connHandle, sizeof(CYBLE_CONN_HANDLE_T));
                TransmitAdditionalData(&connParam->role, sizeof(connParam->role));
                TransmitAdditionalData(&connParam->masterClockAccuracy, sizeof(connParam->masterClockAccuracy));

                TransmitAdditionalData(connParam->peerBdAddr, CYBLE_GAP_BD_ADDR_SIZE);
                TransmitAdditionalData((uint8*)&connParam->peerBdAddrType, sizeof(connParam->peerBdAddrType));

                TransmitAdditionalData(connParam->localResolvablePvtAddr, CYBLE_GAP_BD_ADDR_SIZE);
                /* Dummy field with random addr type */
                TransmitAdditionalData(&addrType, sizeof(addrType));

                TransmitAdditionalData(connParam->peerResolvablePvtAddr, CYBLE_GAP_BD_ADDR_SIZE);
                /* Dummy field with random addr type */
                TransmitAdditionalData(&addrType, sizeof(addrType));

                TransmitAdditionalData((uint8*)&connParam->connIntv, sizeof(connParam->connIntv));
                TransmitAdditionalData((uint8*)&connParam->connLatency, sizeof(connParam->connLatency));
                TransmitAdditionalData((uint8*)&connParam->supervisionTo, sizeof(connParam->supervisionTo));

                CySmt_SendCommandComplete(primaryCmd.opcode, CYBLE_ERROR_OK);
            }
            break;

        case CYBLE_EVT_GAPC_DIRECT_ADV_REPORT:
            {
                /* Send the addr Type as random to frame CySmart response */
                uint8 loaclAddrType = CYBLE_GAP_ADDR_TYPE_RANDOM;
                CYBLE_GAPC_DIRECT_ADV_REPORT_T *directAdvReport = (CYBLE_GAPC_DIRECT_ADV_REPORT_T*)eventParam;
                uint8 payloadSize = sizeof(directAdvReport->rssi) + (sizeof(CYBLE_GAP_BD_ADDR_T) * 2);

                CyS_SendEvent(EVT_DIRECT_ADV_SCAN_PROGRESS_RESULT, primaryCmd.opcode, payloadSize, 0, NULL);

                TransmitAdditionalData(directAdvReport->localBdAddr, CYBLE_GAP_BD_ADDR_SIZE);
                /* Dummy field with random addr type */
                TransmitAdditionalData(&loaclAddrType, sizeof(loaclAddrType));

                TransmitAdditionalData(directAdvReport->peerBdAddr, CYBLE_GAP_BD_ADDR_SIZE);
                TransmitAdditionalData((uint8*)&directAdvReport->peerBdAddrType, sizeof(directAdvReport->peerBdAddrType));

                TransmitAdditionalData((uint8*)&directAdvReport->rssi, sizeof(directAdvReport->rssi));
            }
            break;
#endif /* CYBLE_M0S8BLESS_VERSION > 1 */

        case CYBLE_EVT_GAP_OOB_GENERATED_NOTIFICATION:
            if(0  != ((CYBLE_GAP_OOB_DATA_T*)eventParam)->status)
            {
                CySmt_SendCommandComplete(primaryCmd.opcode, (uint16)((CYBLE_GAP_OOB_DATA_T*)eventParam)->status);
            }
            else
            {
                CYBLE_GAP_OOB_DATA_T *oobData = (CYBLE_GAP_OOB_DATA_T*)eventParam;
                uint16 payloadSize = CYBLE_SMP_RAND_128B_SIZE + oobData->oobDataLen + sizeof(oobData->oobDataLen);
                uint8 oobKeySize = CYBLE_SMP_RAND_128B_SIZE;

                CyS_SendEvent(EVT_GENERATE_SECURED_CONNECTION_OOB_DATA_RESPONSE, primaryCmd.opcode, payloadSize,
                                sizeof(oobKeySize), &oobKeySize);
                TransmitAdditionalData(oobData->key, oobKeySize);
                TransmitAdditionalData(&oobData->oobDataLen, sizeof(oobData->oobDataLen));
                TransmitAdditionalData(oobData->oobData, oobData->oobDataLen);
                CySmt_SendCommandComplete(primaryCmd.opcode, (uint16)oobData->status);
            }
            break;

        /**********************************************************
            *                       GATT Events
            ***********************************************************/
        case CYBLE_EVT_GATTC_ERROR_RSP:
            {
                CYBLE_GATTC_ERR_RSP_PARAM_T *GattErrResp = (CYBLE_GATTC_ERR_RSP_PARAM_T *)eventParam;
                /* Send-out error notification and clear current command if any for all other GATT errors */
                /* Calculate size of CYBLE_GATTC_ERR_RSP_PARAM_T to be sent as response payload */
                uint8 PayloadSize = sizeof(CYBLE_GATT_PDU_T) +
                                    sizeof(CYBLE_GATT_DB_ATTR_HANDLE_T) + 
                                    sizeof(CYBLE_GATT_ERR_CODE_T);

                primaryCmdInProgress = false;

                /* Send connection handle as part of header, other parameters will be 
                                sent as additional payload */
                CyS_SendEvent(EVT_GATT_ERROR_NOTIFICATION, primaryCmd.opcode,
                                PayloadSize, sizeof(CYBLE_CONN_HANDLE_T), 
                                (uint8 *)&GattErrResp->connHandle);

                /* Send GATT PDU op-code */
                TransmitAdditionalData((uint8 *)&GattErrResp->opCode,
                    sizeof(GattErrResp->opCode));

                /* Send ATT Handle */
                TransmitAdditionalData((uint8 *)&GattErrResp->attrHandle,
                    sizeof(GattErrResp->attrHandle));

                /* Send Error code */
                TransmitAdditionalData((uint8 *)&GattErrResp->errorCode,
                    sizeof(GattErrResp->errorCode));
            }
            break;

        case CYBLE_EVT_GATT_CONNECT_IND:
            cyBle_connHandle = *(CYBLE_CONN_HANDLE_T *)eventParam;

            /* Store bdhandle locally as required by CySmart tool disconnect notification */
            cysBdHandle = cyBle_connHandle.bdHandle;

            CyS_SendEvent(EVT_ESTABLISH_CONNECTION_RESPONSE, primaryCmd.opcode, 0,
                            sizeof(cyBle_connHandle), (uint8*)&cyBle_connHandle);
            break;

        case CYBLE_EVT_GATTC_XCHNG_MTU_RSP:
            if(CMD_EXCHANGE_GATT_MTU_SIZE  == primaryCmd.opcode)
            {
                CyS_SendEvent(EVT_EXCHANGE_GATT_MTU_SIZE_RESPONSE, primaryCmd.opcode, 0,
                                sizeof(CYBLE_GATT_XCHG_MTU_PARAM_T), (uint8*)eventParam);
                CySmt_SendCommandComplete(primaryCmd.opcode, CYBLE_ERROR_OK);
            }
            else
            {
                /* Ignore this event */
            }
            break;

        case CYBLE_EVT_GATTC_READ_BY_GROUP_TYPE_RSP:
            if( ReadByGroupEventHandler((CYBLE_GATTC_READ_BY_GRP_RSP_PARAM_T *)eventParam) )
            {
                CySmt_SendCommandComplete(primaryCmd.opcode, CYBLE_ERROR_OK);
            }
            else
            {
                /* Wait for next progress event */
            }
            break;

         case CYBLE_EVT_GATTC_READ_BY_TYPE_RSP:
             if( ReadByTypeEventHandler((CYBLE_GATTC_READ_BY_TYPE_RSP_PARAM_T *)eventParam) )
             {
                 CySmt_SendCommandComplete(primaryCmd.opcode, CYBLE_ERROR_OK);
             }
             else
             {
                 /* Wait for next progress event */
             }
             break;

        case CYBLE_EVT_GATTC_FIND_INFO_RSP:
            if(FindInfoEventHandler((CYBLE_GATTC_FIND_INFO_RSP_PARAM_T *)eventParam) )
            {
                CySmt_SendCommandComplete(primaryCmd.opcode, CYBLE_ERROR_OK);
            }
            else
            {
                /* Wait for next progress event */
            }
            break;

        case CYBLE_EVT_GATTC_FIND_BY_TYPE_VALUE_RSP:
            if(FindByTypeEventHandler((CYBLE_GATTC_FIND_BY_TYPE_RSP_PARAM_T *)eventParam))
            {
                CySmt_SendCommandComplete(primaryCmd.opcode, CYBLE_ERROR_OK);
            }
            else
            {
                /* Wait for next progress event */
            }
            break;

        case CYBLE_EVT_GATTC_READ_RSP:
        case CYBLE_EVT_GATTC_READ_BLOB_RSP:
        case CYBLE_EVT_GATTC_READ_MULTI_RSP:
            /* This event occurs only once for each operation */
            ReadRspEventHandler((CYBLE_GATTC_READ_RSP_PARAM_T *)eventParam);
            break;

        case CYBLE_EVT_GATTC_WRITE_RSP:
            /* Close the command on procedure end */
        case CYBLE_EVT_GATTC_LONG_PROCEDURE_END:
            if(primaryCmdInProgress)
            {
                CySmt_SendCommandComplete(primaryCmd.opcode, CYBLE_ERROR_OK);
            }
            break;

        case CYBLE_EVT_GATTC_EXEC_WRITE_RSP:
            /* Check for result and send complete if it is successful, else send error code */
            if(((CYBLE_GATTC_EXEC_WRITE_RSP_T*)eventParam)->result)
            {
                CySmt_SendCommandComplete(primaryCmd.opcode, CYBLE_ERROR_OK);
            }
            else
            {
                CySmt_SendCommandComplete(primaryCmd.opcode, CYBLE_ERROR_MAX);
            }
            break;

        case CYBLE_EVT_GATTC_HANDLE_VALUE_NTF:
            Gattc_NotificationHandler((CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *)eventParam);
            break;

        case CYBLE_EVT_GATTC_HANDLE_VALUE_IND:
            Gattc_IndicationHandler((CYBLE_GATTC_HANDLE_VALUE_IND_PARAM_T *)eventParam);
            break;

        case CYBLE_EVT_GATTC_STOP_CMD_COMPLETE:
            CyS_SendEvent(EVT_GATT_STOP_NOTIFICATION, CMD_GATT_STOP, 0, 0, NULL);
            CySmt_SendCommandComplete(CMD_GATT_STOP, CYBLE_ERROR_OK);
            break;

        /**********************************************************
            *                       L2CAP Events
            ***********************************************************/

        case CYBLE_EVT_L2CAP_CBFC_CONN_IND:
            {
                CYBLE_L2CAP_CBFC_CONN_IND_PARAM_T *connIndParam = 
                    (CYBLE_L2CAP_CBFC_CONN_IND_PARAM_T*)eventParam;

                uint16 payloadSize = sizeof(uint16) + sizeof(uint16) +
                                    sizeof(CYBLE_L2CAP_CBFC_CONNECT_PARAM_T);

                CYBLE_CONN_HANDLE_T connHandle;

                connHandle.bdHandle = connIndParam->bdHandle;

                CyS_SendEvent(EVT_CBFC_CONNECTION_INDICATION, 0, payloadSize,
                            sizeof(CYBLE_CONN_HANDLE_T), (uint8*)&connHandle);
                /* Send each parameter one by one */
                TransmitAdditionalData((uint8*)&connIndParam->lCid, sizeof(connIndParam->lCid));
                TransmitAdditionalData((uint8*)&connIndParam->psm, sizeof(connIndParam->psm));
                TransmitAdditionalData((uint8*)&connIndParam->connParam, 
                                        sizeof(connIndParam->connParam));
            }
            break;

        case CYBLE_EVT_L2CAP_CBFC_CONN_CNF:
            {
                CYBLE_L2CAP_CBFC_CONN_CNF_PARAM_T *connCnfParam = 
                    (CYBLE_L2CAP_CBFC_CONN_CNF_PARAM_T*)eventParam;

                uint16 payloadSize = sizeof(uint16) + sizeof(uint16) +
                                    sizeof(CYBLE_L2CAP_CBFC_CONNECT_PARAM_T);

                CYBLE_CONN_HANDLE_T connHandle;

                connHandle.bdHandle = connCnfParam->bdHandle;

                CyS_SendEvent(EVT_CBFC_CONNECTION_CONFIRMATION, primaryCmd.opcode, payloadSize,
                            sizeof(CYBLE_CONN_HANDLE_T), (uint8*)&connHandle);
                /* Send each parameter one by one */
                TransmitAdditionalData((uint8*)&connCnfParam->lCid, sizeof(connCnfParam->lCid));
                TransmitAdditionalData((uint8*)&connCnfParam->response, sizeof(connCnfParam->response));
                TransmitAdditionalData((uint8*)&connCnfParam->connParam, sizeof(connCnfParam->connParam));
                CySmt_SendCommandComplete(primaryCmd.opcode, CYBLE_ERROR_OK);
            }
            break;

        case CYBLE_EVT_L2CAP_CBFC_DISCONN_IND:
            /* Send local CID */
            CyS_SendEvent(EVT_CBFC_DISCONNECT_INDICATION, 0, 0, sizeof(uint16), (uint8*)eventParam);
            break;

        case CYBLE_EVT_L2CAP_CBFC_DISCONN_CNF:
            CyS_SendEvent(EVT_CBFC_DISCONNECT_CONFIRMATION, primaryCmd.opcode, 0,
                        sizeof(CYBLE_L2CAP_CBFC_DISCONN_CNF_PARAM_T), (uint8*)eventParam);
            CySmt_SendCommandComplete(primaryCmd.opcode, CYBLE_ERROR_OK);
            break;
        
        case CYBLE_EVT_L2CAP_CBFC_DATA_READ:
            {
                CYBLE_L2CAP_CBFC_RX_PARAM_T *rxDataParam = (CYBLE_L2CAP_CBFC_RX_PARAM_T*)eventParam;
                uint16 payloadSize = sizeof(rxDataParam->result) + sizeof(rxDataParam->rxDataLength)
                                    + rxDataParam->rxDataLength;

                CyS_SendEvent(EVT_CBFC_DATA_RECEIVIED_NOTIFICATION, 0, payloadSize,
                        sizeof(rxDataParam->lCid), (uint8*)&rxDataParam->lCid);
                /* Send each parameter one by one */
                TransmitAdditionalData((uint8*)&rxDataParam->result, sizeof(rxDataParam->result));
                TransmitAdditionalData((uint8*)&rxDataParam->rxDataLength, sizeof(rxDataParam->rxDataLength));
                TransmitAdditionalData(rxDataParam->rxData, rxDataParam->rxDataLength);
            }
            break;
        
        case CYBLE_EVT_L2CAP_CBFC_RX_CREDIT_IND:
            /* Send CYBLE_L2CAP_CBFC_LOW_RX_CREDIT_PARAM_T */
            CyS_SendEvent(EVT_CBFC_RX_CREDIT_INDICATION, 0, 0,
                    sizeof(CYBLE_L2CAP_CBFC_LOW_RX_CREDIT_PARAM_T), (uint8*)eventParam);
            break;
        
        case CYBLE_EVT_L2CAP_CBFC_TX_CREDIT_IND:
            /*Send CYBLE_L2CAP_CBFC_LOW_TX_CREDIT_PARAM_T*/
            CyS_SendEvent(EVT_CBFC_TX_CREDIT_INDICATION, 0, 0,
                    sizeof(CYBLE_L2CAP_CBFC_LOW_TX_CREDIT_PARAM_T), (uint8*)eventParam);
            break;

        case CYBLE_EVT_L2CAP_CBFC_DATA_WRITE_IND:
            {
                CYBLE_L2CAP_CBFC_DATA_WRITE_PARAM_T *writeDataParam = 
                    (CYBLE_L2CAP_CBFC_DATA_WRITE_PARAM_T*)eventParam;

                uint16 payloadSize = sizeof(writeDataParam->result) + sizeof(writeDataParam->bufferLength)
                                    + writeDataParam->bufferLength;

                CyS_SendEvent(EVT_CBFC_DATA_WRITE_INDICATION, primaryCmd.opcode, payloadSize,
                        sizeof(writeDataParam->lCid), (uint8*)&writeDataParam->lCid);
                /* Send each parameter one by one */
                TransmitAdditionalData((uint8*)&writeDataParam->result, sizeof(writeDataParam->result));
                TransmitAdditionalData((uint8*)&writeDataParam->bufferLength, sizeof(writeDataParam->bufferLength));
                TransmitAdditionalData(writeDataParam->buffer, writeDataParam->bufferLength);

                CySmt_SendCommandComplete(primaryCmd.opcode, CYBLE_ERROR_OK);
            }
            break;

        default:
            /* Undefined event, Do nothing */
            break;
    }
}

#endif /* CYSMART_SUPPORT */
/* [] END OF FILE */
