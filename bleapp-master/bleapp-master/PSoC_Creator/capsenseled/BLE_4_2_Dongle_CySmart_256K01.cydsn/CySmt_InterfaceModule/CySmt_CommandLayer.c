/******************************************************************************
* File Name         : CySmt_CommandLayer.c
* Description       : Command processor module for all CySmart commands
*                     Checks for validity of command and sends Ack status
*                     Processes command parameters and invokes corresponding BLE operations.
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

#include "CySmt_CommandLayer.h"
#include "CySmt_BleEventHandler.h"
#include "Application.h"
#include "led.h"

#ifdef CYSMART_SUPPORT

typedef enum
{
    CHARACTERISTIC_DESCRIPTOR = 0,
    CHARACTERISTIC_VALUE,
    CHARACTERISTIC_VALUE_WITHOUT_RESP,
    SIGNED_WRITE_WITHOUT_RESP,
}Write_Req_Type;

/* GATT Long procedures requires the arguments to be global */
static CYBLE_GATTC_PREP_WRITE_REQ_T gPrepLongWriteReq;

/*************************************************************************
 *  General Commands API 
 *************************************************************************/
CYBLE_API_RESULT_T Cmd_Get_Device_Id_Api(Command_Format *currentCmd)
{    
    uint32 devId = DEVICE_ID;
    
    CyS_SendEvent(EVT_GET_DEVICE_ID_RESPONSE, currentCmd->opcode, 0,
                    sizeof(devId), (uint8 *)&devId);
    return CYBLE_ERROR_OK;
}

CYBLE_API_RESULT_T Cmd_Get_Supported_Tool_Ver_Api(Command_Format *currentCmd)
{
    uint16 toolVerResp[4] = {MIN_SUPPORTED_MAJOR_VER, MIN_SUPPORTED_MINOR_VER,
                                RECOMM_TOOL_MAJOR_VER, RECOMM_TOOL_MINOR_VER};
    
    CyS_SendEvent(EVT_GET_SUPPORTED_TOOL_VERSION_RESPONSE, currentCmd->opcode,
                    0, sizeof(toolVerResp), (uint8 *)&toolVerResp);
    return CYBLE_ERROR_OK;
}

CYBLE_API_RESULT_T Cmd_Get_Firmware_Version_Api(Command_Format *currentCmd)
{
    uint16 fwVerResp[4] = {FW_MAJOR_VER, FW_MINOR_VER, FW_PATCH_VER, FW_BUILD_NUM};
    
    CyS_SendEvent(EVT_GET_FIRMWARE_VERSION_RESPONSE, currentCmd->opcode,
                    0, sizeof(fwVerResp), (uint8 *)&fwVerResp);
    return CYBLE_ERROR_OK;
}

CYBLE_API_RESULT_T Cmd_Get_Supported_Gap_Roles_Api(Command_Format *currentCmd)
{
    uint8 role = CYBLE_GAP_BOTH;

    CyS_SendEvent(EVT_GET_SUPPORTED_GAP_ROLES_RESPONSE, currentCmd->opcode, 0,
                    sizeof(uint8), &role);
    return CYBLE_ERROR_OK;
}

CYBLE_API_RESULT_T  Cmd_Get_Current_Gap_Role_Api(Command_Format *currentCmd)
{
    uint8 role = CYBLE_GAP_ROLE;

    CyS_SendEvent(EVT_GET_CURRENT_GAP_ROLE_RESPONSE, currentCmd->opcode, 0,
                    sizeof(uint8), &role);
    return CYBLE_ERROR_OK;
}

CYBLE_API_RESULT_T Cmd_Get_Supported_Gatt_Roles_Api(Command_Format *currentCmd)
{
    uint8 role = CYBLE_GATT_BOTH;

    CyS_SendEvent(EVT_GET_SUPPORTED_GATT_ROLES_RESPONSE, currentCmd->opcode, 0,
                    sizeof(uint8), &role);
    return CYBLE_ERROR_OK;
}

CYBLE_API_RESULT_T Cmd_Get_Current_Gatt_Role_Api(Command_Format *currentCmd)
{
    uint8 role = CYBLE_GATT_ROLE;

    CyS_SendEvent(EVT_GET_CURRENT_GATT_ROLE_RESPONSE, currentCmd->opcode, 0,
                    sizeof(uint8), &role);
    return CYBLE_ERROR_OK;
}

/* Resets stack and CySmart related flags */
static void Stack_Reset(void)
{
    /* Issue disconnect to peripheral, if any active connection exist */
    if(CYBLE_STATE_CONNECTED == cyBle_state)
    {
        CyBle_GapDisconnect(cyBle_connHandle.bdHandle);
    }

    BLE_DeInit();

    /* Tool is closed, reset all CyS structures */
    isCySmtConnected = false; 
    isCySBleStackOn = false;
}

CYBLE_API_RESULT_T Cmd_Init_Ble_Stack_Api(Command_Format *currentCmd)
{
    /* This command doesn't require any parameters */
    (void)currentCmd;

    Stack_Reset();

    /* Turn off LED */
    Led_Stop();

    /* Disable automatic authentication procedures by component */
    cyBle_eventHandlerFlag |= CYBLE_DISABLE_AUTOMATIC_AUTH;

    /* Start BLE component register callback created for CySmart handling */
    return CyBle_Start(CyS_GenericEventHandler);
}

CYBLE_API_RESULT_T Cmd_Tool_Disconnected_Api(Command_Format *currentCmd)
{
    /* This command doesn't require any parameters */
    (void)currentCmd;

    primaryCmdInProgress = false;
    if(isCySmtConnected)
    {
        /* Wait till flash write is fully completed */
        while(CYBLE_ERROR_OK != CyBle_StoreBondingData(1));
        Stack_Reset();

        /* Restarting the BLE in application mode */
        BLE_Init();
    }
    return CYBLE_ERROR_OK;
}

CYBLE_API_RESULT_T Cmd_Host_Timed_Out_Api(Command_Format *currentCmd)
{
    /* This command doesn't require any parameters */
    (void)currentCmd;

    /* Time out, clear all operations */
    primaryCmdInProgress = false;
    
    /* Reset any ongoing stack operations */
    if(GATT_CMD_IN_PROGRESS)
    {
        CyBle_GattcStopCmd();
    }
    else if(CMD_ESTABLISH_CONNECTION == primaryCmd.opcode)
    {
        CyBle_GapcCancelDeviceConnection();
    }
    else if(CMD_START_SCAN == primaryCmd.opcode)
    {
        CyBle_GapcStopScan();
    }
    else
    {
        /* Do nothing */
    }
    return CYBLE_ERROR_OK;
}

CYBLE_API_RESULT_T Cmd_Get_Device_Descriptor_Info(Command_Format *currentCmd)
{
    uint8 payloadLen = sizeof(MANUFACTURER_STRING) + sizeof(PRODUCT_STRING) + sizeof(uint8);
    uint8 length = sizeof(MANUFACTURER_STRING);

    /* Send Mfg string length and data */
    CyS_SendEvent(EVT_GET_DEVICE_DESCRIPTION_RESPONSE, currentCmd->opcode,
                    payloadLen, sizeof(uint8), &length);
    TransmitAdditionalData((uint8*)MANUFACTURER_STRING, length);

    /* Send Product string length and data */
    length = sizeof(PRODUCT_STRING);
    TransmitAdditionalData(&length, sizeof(uint8));
    TransmitAdditionalData((uint8*)PRODUCT_STRING, length);

    return CYBLE_ERROR_OK;
}

CYBLE_API_RESULT_T Cmd_Get_Hardware_Version_Api(Command_Format *currentCmd)
{
    uint16 hwVerResp[4] = {HW_MAJOR_VER, HW_MINOR_VER, HW_PATCH_VER, HW_BUILD_NUM};
    
    CyS_SendEvent(EVT_GET_HARDWARE_VERSION_RESPONSE, currentCmd->opcode,
                    0, sizeof(hwVerResp), (uint8 *)&hwVerResp);
    return CYBLE_ERROR_OK;
}

CYBLE_API_RESULT_T Cmd_Get_Ble_Stack_Version_Api(Command_Format *currentCmd)
{
    CYBLE_STACK_LIB_VERSION_T stackLibVer;
    CYBLE_API_RESULT_T status = CyBle_GetStackLibraryVersion(&stackLibVer);

    CySmt_SendCommandStatus(currentCmd->opcode, status);

    /* Send response data, if stack returns OK */
    if(CYBLE_ERROR_OK == status)
    {
        CyS_SendEvent(EVT_GET_BLE_STACK_VERSION_RESPONSE, currentCmd->opcode,
                        0, sizeof(stackLibVer), (uint8*)&stackLibVer);
    }

    /* Pass on the result to handle command complete event */
    return status;
}

CYBLE_API_RESULT_T Cmd_Get_Rssi_Api(Command_Format *currentCmd)
{
    int8 rssi = CyBle_GetRssi();

    CyS_SendEvent(EVT_GET_RSSI_RESPONSE, currentCmd->opcode, 0, sizeof(rssi), (uint8*)&rssi);

    /* Pass on the result to handle command complete event */
    return CYBLE_ERROR_OK;
}

CYBLE_API_RESULT_T Cmd_Get_TxPowerLevel_Api(Command_Format *currentCmd)
{
    CYBLE_BLESS_PWR_IN_DB_T bleSsPwrLvl;
    CYBLE_API_RESULT_T status = CYBLE_ERROR_INVALID_OPERATION;

    bleSsPwrLvl.bleSsChId = (CYBLE_BLESS_PHY_CH_GRP_ID_T)*currentCmd->parameters;

    status = CyBle_GetTxPowerLevel(&bleSsPwrLvl);
    CySmt_SendCommandStatus(currentCmd->opcode, status);

    /* Send response data, if stack returns OK */
    if(CYBLE_ERROR_OK == status)
    {
        CyS_SendEvent(EVT_GET_TX_POWER_RESPONSE, currentCmd->opcode, sizeof(bleSsPwrLvl.blePwrLevelInDbm),
            sizeof(bleSsPwrLvl.bleSsChId), (uint8*)&(bleSsPwrLvl.bleSsChId));
        TransmitAdditionalData((uint8*)&bleSsPwrLvl.blePwrLevelInDbm, sizeof(bleSsPwrLvl.blePwrLevelInDbm));
    }

    /* Pass on the result to handle command complete event */
    return status;
}

CYBLE_API_RESULT_T Cmd_Set_TxPowerLevel_Api(Command_Format *currentCmd)
{
    CYBLE_BLESS_PWR_IN_DB_T bleSsPwrLvl;

    bleSsPwrLvl.bleSsChId = (CYBLE_BLESS_PHY_CH_GRP_ID_T)*currentCmd->parameters;
    bleSsPwrLvl.blePwrLevelInDbm = (CYBLE_BLESS_PWR_LVL_T)currentCmd->parameters[sizeof(bleSsPwrLvl.bleSsChId)];

    /* Pass on the result to handle command complete event */
    return CyBle_SetTxPowerLevel(&bleSsPwrLvl);
}

CYBLE_API_RESULT_T Cmd_Set_HostChannelClassification_Api(Command_Format *currentCmd)
{
    CYBLE_API_RESULT_T status = CyBle_IsLLControlProcPending();
    if(CYBLE_ERROR_OK == status)
    {
        /* This command has length followed by channel map data - Ignore the length as it's fixed to 5 bytes */
        status = CyBle_GapcSetHostChannelClassification(&currentCmd->parameters[sizeof(uint8)]);
    }
    return status;
}

/*************************************************************************
*  Gap Commands API 
*************************************************************************/

CYBLE_API_RESULT_T Cmd_Set_Device_Io_Capabilities_Api(Command_Format *currentCmd)
{
    CySIoCap = (CYBLE_GAP_IOCAP_T)*currentCmd->parameters;
    return CyBle_GapSetIoCap(CySIoCap);
}

CYBLE_API_RESULT_T Cmd_Get_Device_Io_Capabilities_Api(Command_Format *currentCmd)
{

    CyS_SendEvent(EVT_GET_DEVICE_IO_CAPABILITIES_RESPONSE, currentCmd->opcode,
                    0, sizeof(CySIoCap), (uint8*)&CySIoCap);
    return CYBLE_ERROR_OK;
}

CYBLE_API_RESULT_T Cmd_Get_Bluetooth_Device_Address_Api(Command_Format *currentCmd)
{
    CYBLE_GAP_BD_ADDR_T addr = {{0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u}, 0x00u };
    CYBLE_API_RESULT_T status;

    /* Configure the type of address to be requested */
    addr.type = (uint8)*currentCmd->parameters;
    status = CyBle_GetDeviceAddress(&addr);

    CySmt_SendCommandStatus(currentCmd->opcode, status);

    /* Send response data, if stack returns OK */
    if(CYBLE_ERROR_OK == status)
    {
        /* Send BD address first, followed by address type */
        CyS_SendEvent(EVT_GET_BLUETOOTH_DEVICE_ADDRESS_RESPONSE, currentCmd->opcode,
                        sizeof(addr.type), sizeof(addr.bdAddr), (uint8*)addr.bdAddr);
        TransmitAdditionalData(&addr.type, sizeof(addr.type));
    }

    /* Pass on the result to handle command complete event */
    return status;
}

CYBLE_API_RESULT_T Cmd_Set_Bluetooth_Device_Address_Api(Command_Format *currentCmd)
{
    return CyBle_SetDeviceAddress((CYBLE_GAP_BD_ADDR_T*)currentCmd->parameters);
}

CYBLE_API_RESULT_T Cmd_Get_Peer_Bluetooth_Device_Address_Api(Command_Format *currentCmd)
{
    CYBLE_API_RESULT_T status;
    CYBLE_GAP_BD_ADDR_T peerBDAddr = {{0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u}, 0x00u };

    status = CyBle_GapGetPeerBdAddr(*currentCmd->parameters, &peerBDAddr);

    CySmt_SendCommandStatus(currentCmd->opcode, status);

    /* Send response data, if stack returns OK */
    if(CYBLE_ERROR_OK == status)
    {
        CyS_SendEvent(EVT_GET_PEER_BLUETOOTH_DEVICE_ADDRESS_RESPONSE, currentCmd->opcode,
            sizeof(peerBDAddr.bdAddr) + sizeof(peerBDAddr.type),
            sizeof(uint8), currentCmd->parameters);
        
        TransmitAdditionalData(peerBDAddr.bdAddr, sizeof(peerBDAddr.bdAddr));
        TransmitAdditionalData(&peerBDAddr.type, sizeof(peerBDAddr.type));
    }

    /* Pass on the result to handle command complete event */
    return status;
}

CYBLE_API_RESULT_T Cmd_Get_Peer_Device_Handle_Api(Command_Format *currentCmd)
{
    uint8 bdHandle;
    CYBLE_API_RESULT_T status = CyBle_GapGetPeerBdHandle(&bdHandle, (CYBLE_GAP_BD_ADDR_T*)currentCmd->parameters);
    CySmt_SendCommandStatus(currentCmd->opcode, status);

    /* Send response data, if stack returns OK */
    if(CYBLE_ERROR_OK == status)
    {
        CyS_SendEvent(EVT_GET_PEER_DEVICE_HANDLE_RESPONSE, currentCmd->opcode,
            sizeof(CYBLE_GAP_BD_ADDR_T), sizeof(bdHandle), &bdHandle);

        TransmitAdditionalData(currentCmd->parameters, sizeof(CYBLE_GAP_BD_ADDR_T));
    }

    /* Pass on the result to handle command complete event */
    return status;
}

CYBLE_API_RESULT_T Cmd_GenerateBd_Addr_Api(Command_Format *currentCmd)
{
    CYBLE_GAP_BD_ADDR_T addr;
    CYBLE_API_RESULT_T status = CyBle_GapGenerateDeviceAddress(&addr, (CYBLE_GAP_ADDR_TYPE_T)*(currentCmd->parameters),
                                     &currentCmd->parameters[sizeof(CYBLE_GAP_ADDR_TYPE_T) + sizeof(uint8)]);
    CySmt_SendCommandStatus(currentCmd->opcode, status);

    /* Send response data, if stack returns OK */
    if(CYBLE_ERROR_OK == status)
    {
        CyS_SendEvent(EVT_GENERATE_BD_ADDR_RESPONSE, currentCmd->opcode,
                sizeof(addr.type), sizeof(addr.bdAddr), (uint8*)addr.bdAddr);
        TransmitAdditionalData(&addr.type, sizeof(addr.type));
    }

    /* Pass on the result to handle command complete event */
    return status;
}

/* Index macros for Set OOB Data Command */
#define SET_OOB_DATA_LEN_FIELD_INDEX    ( 20u )
#define SET_OOB_DATA_FIELD_INDEX        ( 21u )

CYBLE_API_RESULT_T Cmd_Set_Oob_Data_Api(Command_Format *currentCmd)
{
    uint8 *paramBuf = currentCmd->parameters;

    cyBle_connHandle = *(CYBLE_CONN_HANDLE_T *)paramBuf;
    return CyBle_GapSetOobData(cyBle_connHandle.bdHandle, paramBuf[sizeof(CYBLE_CONN_HANDLE_T)],
            /* Ignore the length field of Oobkey from the command params */
            &paramBuf[sizeof(CYBLE_CONN_HANDLE_T) + sizeof(uint8) + sizeof(uint8)],
            &paramBuf[SET_OOB_DATA_FIELD_INDEX], &paramBuf[SET_OOB_DATA_LEN_FIELD_INDEX]);
}

CYBLE_API_RESULT_T Cmd_Get_Connection_Parameters_Api(Command_Format *currentCmd)
{
    /* Send all the parameters as payload except scanIntv, which is sent along with header */
    uint16 payloadSize = sizeof(cyBle_connectionParameters.scanWindow) +
                        sizeof(cyBle_connectionParameters.initiatorFilterPolicy) +
                        sizeof(cyBle_connectionParameters.peerBdAddr) +
                        sizeof(cyBle_connectionParameters.peerAddrType) +
                        sizeof(cyBle_connectionParameters.ownAddrType) +
                        sizeof(cyBle_connectionParameters.connIntvMin) +
                        sizeof(cyBle_connectionParameters.connIntvMax) +
                        sizeof(cyBle_connectionParameters.connLatency) +
                        sizeof(cyBle_connectionParameters.supervisionTO) +
                        sizeof(cyBle_connectionParameters.minCeLength) +
                        sizeof(cyBle_connectionParameters.maxCeLength);

    CyS_SendEvent(EVT_GET_CONNECTION_PARAMETERS_RESPONSE, currentCmd->opcode, payloadSize,
                    sizeof(cyBle_connectionParameters.scanIntv),
                    (uint8*)&cyBle_connectionParameters.scanIntv);

    /* Send all the parameters one by one */
    TransmitAdditionalData((uint8*)&cyBle_connectionParameters.scanWindow,
                            sizeof(cyBle_connectionParameters.scanWindow));

    TransmitAdditionalData(&cyBle_connectionParameters.initiatorFilterPolicy,
                            sizeof(cyBle_connectionParameters.initiatorFilterPolicy));

    TransmitAdditionalData(cyBle_connectionParameters.peerBdAddr, 
                            sizeof(cyBle_connectionParameters.peerBdAddr));

    TransmitAdditionalData(&cyBle_connectionParameters.peerAddrType,
                            sizeof(cyBle_connectionParameters.peerAddrType));

    TransmitAdditionalData(&cyBle_connectionParameters.ownAddrType,
                            sizeof(cyBle_connectionParameters.ownAddrType));

    TransmitAdditionalData((uint8*)&cyBle_connectionParameters.connIntvMin,
                            sizeof(cyBle_connectionParameters.connIntvMin));

    TransmitAdditionalData((uint8*)&cyBle_connectionParameters.connIntvMax,
                            sizeof(cyBle_connectionParameters.connIntvMax));

    TransmitAdditionalData((uint8*)&cyBle_connectionParameters.connLatency,
                            sizeof(cyBle_connectionParameters.connLatency));

    TransmitAdditionalData((uint8*)&cyBle_connectionParameters.supervisionTO,
                            sizeof(cyBle_connectionParameters.supervisionTO));

    TransmitAdditionalData((uint8*)&cyBle_connectionParameters.minCeLength,
                            sizeof(cyBle_connectionParameters.minCeLength));

    TransmitAdditionalData((uint8*)&cyBle_connectionParameters.maxCeLength,
                            sizeof(cyBle_connectionParameters.maxCeLength));

    return CYBLE_ERROR_OK;
}

CYBLE_API_RESULT_T Cmd_Set_Connection_Parameters_Api(Command_Format *currentCmd)
{
    uint8 index = 0;
    uint8 *paramBuf = currentCmd->parameters;

    /* Fill-up the connection parameters structure from the input byte stream */
    cyBle_connectionParameters.scanIntv = CyBle_Get16ByPtr(&paramBuf[index]);
    index += sizeof(cyBle_connectionParameters.scanIntv);

    cyBle_connectionParameters.scanWindow = CyBle_Get16ByPtr(&paramBuf[index]);
    index += sizeof(cyBle_connectionParameters.scanWindow);

    cyBle_connectionParameters.initiatorFilterPolicy = paramBuf[index];
    index += sizeof(cyBle_connectionParameters.initiatorFilterPolicy);

    memcpy(cyBle_connectionParameters.peerBdAddr, &paramBuf[index], CYBLE_GAP_BD_ADDR_SIZE);
    index += sizeof(cyBle_connectionParameters.peerBdAddr);

    cyBle_connectionParameters.peerAddrType = paramBuf[index];
    index += sizeof(cyBle_connectionParameters.peerAddrType);

    cyBle_connectionParameters.ownAddrType = paramBuf[index];
    index += sizeof(cyBle_connectionParameters.ownAddrType);

    cyBle_connectionParameters.connIntvMin = CyBle_Get16ByPtr(&paramBuf[index]);
    index += sizeof(cyBle_connectionParameters.connIntvMin);

    cyBle_connectionParameters.connIntvMax = CyBle_Get16ByPtr(&paramBuf[index]);
    index += sizeof(cyBle_connectionParameters.connIntvMax);

    cyBle_connectionParameters.connLatency = CyBle_Get16ByPtr(&paramBuf[index]);
    index += sizeof(cyBle_connectionParameters.connLatency);

    cyBle_connectionParameters.supervisionTO = CyBle_Get16ByPtr(&paramBuf[index]);
    index += sizeof(cyBle_connectionParameters.supervisionTO);

    cyBle_connectionParameters.minCeLength = CyBle_Get16ByPtr(&paramBuf[index]);
    index += sizeof(cyBle_connectionParameters.minCeLength);

    cyBle_connectionParameters.maxCeLength = CyBle_Get16ByPtr(&paramBuf[index]);
    index += sizeof(cyBle_connectionParameters.maxCeLength);

    return CYBLE_ERROR_OK;
}

CYBLE_API_RESULT_T Cmd_Get_Scan_Parameters_Api(Command_Format *currentCmd)
{
    /* Send all the parameters as payload except discProcedure, which is sent along with header */
    uint16 payloadSize = sizeof(cyBle_discoveryInfo.scanType) +
                        sizeof(cyBle_discoveryInfo.scanIntv) +
                        sizeof(cyBle_discoveryInfo.scanWindow) +
                        sizeof(cyBle_discoveryInfo.ownAddrType) +
                        sizeof(cyBle_discoveryInfo.scanFilterPolicy) +
                        sizeof(cyBle_discoveryInfo.scanTo) +
                        sizeof(cyBle_discoveryInfo.filterDuplicates);

    CyS_SendEvent(EVT_GET_SCAN_PARAMETERS_RESPONSE, currentCmd->opcode, payloadSize,
                    sizeof(cyBle_discoveryInfo.discProcedure),
                    &cyBle_discoveryInfo.discProcedure);

    /* Send all the parameters one by one */
    TransmitAdditionalData(&cyBle_discoveryInfo.scanType,
                            sizeof(cyBle_discoveryInfo.scanType));

    TransmitAdditionalData((uint8*)&cyBle_discoveryInfo.scanIntv,
                            sizeof(cyBle_discoveryInfo.scanIntv));

    TransmitAdditionalData((uint8*)&cyBle_discoveryInfo.scanWindow,
                            sizeof(cyBle_discoveryInfo.scanWindow));

    TransmitAdditionalData(&cyBle_discoveryInfo.ownAddrType,
                            sizeof(cyBle_discoveryInfo.ownAddrType));

    TransmitAdditionalData(&cyBle_discoveryInfo.scanFilterPolicy,
                            sizeof(cyBle_discoveryInfo.scanFilterPolicy));

    TransmitAdditionalData((uint8*)&cyBle_discoveryInfo.scanTo,
                            sizeof(cyBle_discoveryInfo.scanTo));

    TransmitAdditionalData(&cyBle_discoveryInfo.filterDuplicates,
                            sizeof(cyBle_discoveryInfo.filterDuplicates));

    return CYBLE_ERROR_OK;
}

CYBLE_API_RESULT_T Cmd_Set_Scan_Parameters_Api(Command_Format *currentCmd)
{
    uint8 index = 0;
    uint8 *paramBuf = currentCmd->parameters;

    cyBle_discoveryInfo.discProcedure = paramBuf[index];
    index += sizeof(cyBle_discoveryInfo.discProcedure);

    cyBle_discoveryInfo.scanType = paramBuf[index];
    index += sizeof(cyBle_discoveryInfo.scanType);

    cyBle_discoveryInfo.scanIntv = CyBle_Get16ByPtr(&paramBuf[index]);
    index += sizeof(cyBle_discoveryInfo.scanIntv);

    cyBle_discoveryInfo.scanWindow = CyBle_Get16ByPtr(&paramBuf[index]);
    index += sizeof(cyBle_discoveryInfo.scanWindow);

    cyBle_discoveryInfo.ownAddrType = paramBuf[index];
    index += sizeof(cyBle_discoveryInfo.ownAddrType);

    cyBle_discoveryInfo.scanFilterPolicy = paramBuf[index];
    index += sizeof(cyBle_discoveryInfo.scanFilterPolicy);

    cyBle_discoveryInfo.scanTo = CyBle_Get16ByPtr(&paramBuf[index]);
    index += sizeof(cyBle_discoveryInfo.scanTo);

    cyBle_discoveryInfo.filterDuplicates = paramBuf[index];
    index += sizeof(cyBle_discoveryInfo.filterDuplicates);

    return CYBLE_ERROR_OK;
}

CYBLE_API_RESULT_T Cmd_Get_Local_Device_Security_Api(Command_Format *currentCmd)
{
    CyS_SendEvent(EVT_GET_LOCAL_DEVICE_SECURITY_RESPONSE, currentCmd->opcode,
        0, sizeof(cyBle_authInfo), (uint8*)&cyBle_authInfo);

    return CYBLE_ERROR_OK;
}

CYBLE_API_RESULT_T Cmd_Set_Local_Device_Security_Api(Command_Format *currentCmd)
{
    uint8 index = 0;
    uint8 *paramBuf = currentCmd->parameters;

    cyBle_authInfo.security = paramBuf[index];
    index += sizeof(cyBle_authInfo.security);

    cyBle_authInfo.bonding = paramBuf[index];
    index += sizeof(cyBle_authInfo.bonding);

    cyBle_authInfo.ekeySize = paramBuf[index];
    index += sizeof(cyBle_authInfo.ekeySize);

    cyBle_authInfo.authErr = (CYBLE_GAP_AUTH_FAILED_REASON_T)paramBuf[index];
    index += sizeof(cyBle_authInfo.authErr);

    cyBle_authInfo.pairingProperties = paramBuf[index];
    index += sizeof(cyBle_authInfo.pairingProperties);

    /* For strict pairing control use the stack API */
    return CyBle_GapSetSecureConnectionsOnlyMode(paramBuf[index]);
}

CYBLE_API_RESULT_T Cmd_Get_Peer_Device_Security_Api(Command_Format *currentCmd)
{
    CYBLE_GAP_AUTH_INFO_T peerSecurity;
    CYBLE_API_RESULT_T status;

    status = CyBle_GapGetPeerDevSecurity(*(currentCmd->parameters), &peerSecurity);

    CySmt_SendCommandStatus(currentCmd->opcode, status);

    /* Send response data, if stack returns OK */
    if(CYBLE_ERROR_OK == status)
    {
        CyS_SendEvent(EVT_GET_PEER_DEVICE_SECURITY_RESPONSE, currentCmd->opcode,
            sizeof(peerSecurity), sizeof(uint8), currentCmd->parameters);
        TransmitAdditionalData((uint8*)&peerSecurity, sizeof(peerSecurity));
    }

    /* Pass on the result to handle command complete event */
    return status;
}

CYBLE_API_RESULT_T Cmd_Get_White_List_Api(Command_Format *currentCmd)
{
    uint8 whiteListCount;
    /*In current implementation, bonded devices will be added to white-list automatically, so the list will include both*/
    CYBLE_GAP_BD_ADDR_T whiteList[CYBLE_GAP_SIZE_OF_WHITELIST];
    CYBLE_API_RESULT_T status = CyBle_GapGetDevicesFromWhiteList(&whiteListCount, whiteList);

    CySmt_SendCommandStatus(currentCmd->opcode, status);
    if(CYBLE_ERROR_OK == status)
    {
        uint16 index, payloadSize;
        payloadSize = whiteListCount * (CYBLE_GAP_BD_ADDR_SIZE + sizeof(uint8));

        CyS_SendEvent(EVT_GET_WHITE_LIST_RESPONSE, currentCmd->opcode,
                        payloadSize, 0, NULL);

        for(index = 0; index < whiteListCount;index++)
        {
            TransmitAdditionalData(whiteList[index].bdAddr, sizeof(whiteList[index].bdAddr));
            TransmitAdditionalData(&whiteList[index].type, sizeof(uint8));
        }
    }

    return status;
}

CYBLE_API_RESULT_T Cmd_Add_Device_To_White_List_Api(Command_Format *currentCmd)
{
    return CyBle_GapAddDeviceToWhiteList((CYBLE_GAP_BD_ADDR_T*)currentCmd->parameters);
}

CYBLE_API_RESULT_T Cmd_Remove_Device_From_White_List_Api(Command_Format *currentCmd)
{
    return CyBle_GapRemoveDeviceFromWhiteList((CYBLE_GAP_BD_ADDR_T*)currentCmd->parameters);
}

CYBLE_API_RESULT_T Cmd_Clear_White_List_Api(Command_Format *currentCmd)
{
    CYBLE_GAP_BD_ADDR_T peerBDAddr = {{0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u}, 0x00u };
    /* This command doesn't require any parameters */
    (void)currentCmd;

    return CyBle_GapRemoveDeviceFromWhiteList(&peerBDAddr);
}

CYBLE_API_RESULT_T Cmd_Start_Scan_Api(Command_Format *currentCmd)
{
    /* This command doesn't require any parameters */
    (void)currentCmd;

    return CyBle_GapcStartScan(CYBLE_SCANNING_CUSTOM);
}

CYBLE_API_RESULT_T Cmd_Stop_Scan_Api(Command_Format *currentCmd)
{
    CYBLE_API_RESULT_T status = CYBLE_ERROR_OK;

    /* This command doesn't require any parameters */
    (void)currentCmd;

    if(CMD_START_SCAN == primaryCmd.opcode)
    {
        CyBle_GapcStopScan();
    }
    else
    {
        /* Scanning is not in progress, so send-out invalid operation */
        status = CYBLE_ERROR_INVALID_OPERATION;
    }

    /* Send command status */
    return status;
}

/* Utility API for packeting and sending SMP Keys to UART */
void Packetize_Send_SMP_Keys(CYBLE_GAP_SMP_KEY_DIST_T* keyInfo)
{
    uint8 keysize;

    /* Send each key one by one along with length of each */
    keysize = sizeof(keyInfo->ltkInfo);
    TransmitAdditionalData(&keysize, sizeof(keysize));
    TransmitAdditionalData(keyInfo->ltkInfo, keysize);

    keysize = sizeof(keyInfo->midInfo);
    TransmitAdditionalData(&keysize, sizeof(keysize));
    TransmitAdditionalData(keyInfo->midInfo, keysize);

    keysize = sizeof(keyInfo->irkInfo);
    TransmitAdditionalData(&keysize, sizeof(keysize));
    TransmitAdditionalData(keyInfo->irkInfo, keysize);

    /* Swap and send the BD address type after the address */
    TransmitAdditionalData((keyInfo->idAddrInfo + 1), sizeof(keyInfo->idAddrInfo) - 1);
    TransmitAdditionalData(keyInfo->idAddrInfo, sizeof(uint8));

    keysize = sizeof(keyInfo->csrkInfo);
    TransmitAdditionalData(&keysize, sizeof(keysize));
    TransmitAdditionalData(keyInfo->csrkInfo, keysize);
}

CYBLE_API_RESULT_T Cmd_Generate_Set_Keys_Api(Command_Format *currentCmd)
{
    CYBLE_GAP_SMP_KEY_DIST_T keyInfo;
    CYBLE_API_RESULT_T status = CyBle_GapGenerateKeys(*(currentCmd->parameters), &keyInfo);

    CySmt_SendCommandStatus(currentCmd->opcode, status);

    if(CYBLE_ERROR_OK == status)
    {
        CyS_SendEvent(EVT_CURRENT_LOCAL_KEYS_RESPONSE, currentCmd->opcode,
            sizeof(keyInfo) + ((NUM_KEYS_GENERATED - 1) * sizeof(uint8)), 0, NULL);

        Packetize_Send_SMP_Keys(&keyInfo);
    }

    return status;
}

CYBLE_API_RESULT_T Cmd_Set_Authentication_Keys_Api(Command_Format *currentCmd)
{
    uint8 index = 0, *paramBuf = currentCmd->parameters;
    CYBLE_GAP_SMP_KEY_DIST_T keyInfo;
    CYBLE_API_RESULT_T status = CYBLE_ERROR_MAX;
    uint8 keysFlag = paramBuf[index++];

    (void)memset(&keyInfo, 0, sizeof(keyInfo));

    /* Get each key one by one along with length of each - Ignore the length fields */
    memcpy(keyInfo.ltkInfo, &paramBuf[++index], sizeof(keyInfo.ltkInfo));
    index += sizeof(keyInfo.ltkInfo);

    memcpy(keyInfo.midInfo, &paramBuf[++index], sizeof(keyInfo.midInfo));
    index += sizeof(keyInfo.midInfo);

    memcpy(keyInfo.irkInfo, &paramBuf[++index], sizeof(keyInfo.irkInfo));
    index += sizeof(keyInfo.irkInfo);

    /* Swap and load -- BD address type is after the address */
    memcpy(&keyInfo.idAddrInfo[1], &paramBuf[index], CYBLE_GAP_BD_ADDR_SIZE);
    index += CYBLE_GAP_BD_ADDR_SIZE;
    keyInfo.idAddrInfo[0] = paramBuf[index++];  /* Load addr type */

    memcpy(keyInfo.csrkInfo, &paramBuf[++index], sizeof(keyInfo.csrkInfo));
    index += sizeof(keyInfo.csrkInfo);

    status = CyBle_GapSetSecurityKeys(keysFlag, &keyInfo);
    CySmt_SendCommandStatus(currentCmd->opcode, status);

    if(CYBLE_ERROR_OK == status)
    {
        CyS_SendEvent(EVT_CURRENT_LOCAL_KEYS_RESPONSE, currentCmd->opcode,
            sizeof(keyInfo) + ((NUM_KEYS_GENERATED - 1) * sizeof(uint8)), 0, NULL);

        Packetize_Send_SMP_Keys(&keyInfo);
    }

    return status;
}

CYBLE_API_RESULT_T Cmd_Establish_Connection_Api(Command_Format *currentCmd)
{
    return CyBle_GapcConnectDevice((CYBLE_GAP_BD_ADDR_T*)currentCmd->parameters);
}

CYBLE_API_RESULT_T Cmd_Terminate_Connection_Api(Command_Format *currentCmd)
{
    /* Check if the device is connected, else send the terminate notification */
    if(CYBLE_STATE_CONNECTED != CyBle_GetState())
    {
        uint8 disconnectReason = 0; /* Unknown reason */
        CyS_SendEvent(EVT_CONNECTION_TERMINATED_NOTIFICATION, 0, sizeof(uint8),
                        sizeof(cyBle_connHandle), (uint8*)&cyBle_connHandle);
        TransmitAdditionalData(&disconnectReason, sizeof(uint8));

        primaryCmdInProgress = false;
        return CYBLE_ERROR_OK;
    }

    /* This command can be triggered as both primary or secondary */
    return CyBle_GapDisconnect(((CYBLE_CONN_HANDLE_T *)currentCmd->parameters)->bdHandle);
}

CYBLE_API_RESULT_T Cmd_Initiate_Pairing_Request_Api(Command_Format *currentCmd)
{
    cyBle_connHandle = *(CYBLE_CONN_HANDLE_T *)currentCmd->parameters;
    return CyBle_GapAuthReq(cyBle_connHandle.bdHandle, &cyBle_authInfo);
}

CYBLE_API_RESULT_T Cmd_Set_Identiry_Addr_Api(Command_Format *currentCmd)
{
    return CyBle_GapSetIdAddress((CYBLE_GAP_BD_ADDR_T*)currentCmd->parameters);
}

CYBLE_API_RESULT_T Cmd_Pairing_PassKey_Api(Command_Format *currentCmd)
{
    uint8 index = sizeof(CYBLE_CONN_HANDLE_T);
    uint8 *paramBuf = currentCmd->parameters;
    uint32 passkey = (uint32)paramBuf[index++];

    /* This command should always occur, only when pairing process is ON */
    if(CMD_INITIATE_PAIRING_REQUEST != primaryCmd.opcode)
    {
        return CYBLE_ERROR_INVALID_OPERATION;
    }

    /* Convert byte array to passkey format */
    passkey |= (uint32)paramBuf[index++] << CHAR_BIT;
    passkey |= (uint32)paramBuf[index++] << (CHAR_BIT * sizeof(uint16));
    passkey |= (uint32)paramBuf[index++] << (CHAR_BIT * (sizeof(uint16) + sizeof(uint8)));

    cyBle_connHandle = *(CYBLE_CONN_HANDLE_T *)paramBuf;
    return CyBle_GapAuthPassKeyReply(cyBle_connHandle.bdHandle, passkey, paramBuf[index]);
}

CYBLE_API_RESULT_T Cmd_Update_Connection_Params_Api(Command_Format *currentCmd)
{
    CYBLE_API_RESULT_T status = CyBle_IsLLControlProcPending();
    if(CYBLE_ERROR_OK == status)
    {
        uint8 *paramBuf = currentCmd->parameters;

        cyBle_connHandle = *(CYBLE_CONN_HANDLE_T *)paramBuf;
        status = CyBle_GapcConnectionParamUpdateRequest(cyBle_connHandle.bdHandle,
            (CYBLE_GAP_CONN_UPDATE_PARAM_T*)&paramBuf[sizeof(CYBLE_CONN_HANDLE_T)]);
    }
    return status;
}

CYBLE_API_RESULT_T Cmd_Cancle_Connection_Api(Command_Format *currentCmd)
{
    CYBLE_API_RESULT_T status = CyBle_GapcCancelDeviceConnection();

    CySmt_SendCommandStatus(currentCmd->opcode, status);
    if(CYBLE_ERROR_OK == status)
    {
        /* Current command processing is complete */
        primaryCmdInProgress = false;

        CyS_SendEvent(EVT_CONNECTION_CANCELLED_NOTIFICATION, currentCmd->opcode, 0,
                        0,
                        NULL);
    }
    return status;
}

CYBLE_API_RESULT_T Cmd_Get_Bonded_Devices_By_Rank_Api(Command_Format *currentCmd)
{
    CYBLE_GAP_DEVICE_ADDR_LIST_T bondedDevList;
    CYBLE_API_RESULT_T status = CyBle_GapGetBondedDevicesByRank(&bondedDevList);

    CySmt_SendCommandStatus(currentCmd->opcode, status);
    if(CYBLE_ERROR_OK == status)
    {
        uint16 index, payloadSize = bondedDevList.count * (sizeof(CYBLE_GAP_BD_ADDR_T) + sizeof(uint8));

        CyS_SendEvent(EVT_GET_BONDED_DEVICES_BY_RANK_RESPONSE, currentCmd->opcode,
                        payloadSize, 0, NULL);

        for(index = 0; index < bondedDevList.count; index++)
        {
            TransmitAdditionalData(bondedDevList.bdHandleAddrList[index].bdAddr.bdAddr,
                sizeof(bondedDevList.bdHandleAddrList[index].bdAddr.bdAddr));
            TransmitAdditionalData(&bondedDevList.bdHandleAddrList[index].bdAddr.type, sizeof(uint8));
            TransmitAdditionalData(&bondedDevList.bdHandleAddrList[index].bdHandle, sizeof(uint8));
        }
    }
    return status;
}

CYBLE_API_RESULT_T Cmd_UpdateConnectionParam_Resp_Api(Command_Format *currentCmd)
{
    uint8 *paramBuf = currentCmd->parameters;
    uint16 result = CyBle_Get16ByPtr(&paramBuf[sizeof(CYBLE_CONN_HANDLE_T)]);
    CYBLE_API_RESULT_T status = CYBLE_ERROR_OK, controllerStatus = CyBle_IsLLControlProcPending();

    if(CYBLE_L2CAP_CONN_PARAM_ACCEPTED == result)
    {
        if(CYBLE_ERROR_OK != controllerStatus)
        {
            /* Reject the conn update request when controller is busy */
            result = CYBLE_L2CAP_CONN_PARAM_REJECTED;
        }
    }

    cyBle_connHandle = *(CYBLE_CONN_HANDLE_T *)paramBuf;
    status = CyBle_L2capLeConnectionParamUpdateResponse(cyBle_connHandle.bdHandle, result);

    /* Make sure the controller busy is informed via status event */
    if(CYBLE_ERROR_CONTROLLER_BUSY == controllerStatus)
    {
        return controllerStatus;
    }
    else
    {
        return status;
    }
}

CYBLE_API_RESULT_T Cmd_Get_PeerDevice_SecurityKeys_Api(Command_Format *currentCmd)
{
    CYBLE_GAP_SMP_KEY_DIST_T keyInfo;
    uint8 keysFlag = 0;
    CYBLE_API_RESULT_T status = CyBle_GapGetPeerDevSecurityKeyInfo(*currentCmd->parameters, &keysFlag, &keyInfo);

    CySmt_SendCommandStatus(currentCmd->opcode, status);

    if(CYBLE_ERROR_OK == status)
    {
        CyS_SendEvent(EVT_GET_PEER_DEVICE_SECURITY_KEYS_RESPONSE, currentCmd->opcode,
            sizeof(keysFlag) + sizeof(keyInfo) + ((NUM_KEYS_GENERATED - 1) * sizeof(uint8)),
            sizeof(uint8), currentCmd->parameters);

        /* Send each key one by one along with length of each */
        TransmitAdditionalData(&keysFlag, sizeof(keysFlag));

        Packetize_Send_SMP_Keys(&keyInfo);
    }

    return status;
}

CYBLE_API_RESULT_T Cmd_Resolve_Set_Peer_Addr_Api(Command_Format *currentCmd)
{
    /* Send the info event with status of peer address resolution */
    uint8 resolutionStatus = 0x01;
    CYBLE_GAP_BD_ADDR_T peerBDAddr;
    CYBLE_GAP_DEVICE_ADDR_LIST_T bondedDevList;
    CYBLE_API_RESULT_T status = CyBle_GapGetBondedDevicesByRank(&bondedDevList);

    CySmt_SendCommandStatus(currentCmd->opcode, status);

    if(CYBLE_ERROR_OK == status)
    {
        uint8 index = 0;

        memcpy(peerBDAddr.bdAddr, currentCmd->parameters, CYBLE_GAP_BD_ADDR_SIZE);
        /* Address type is followed by address */
        peerBDAddr.type = currentCmd->parameters[CYBLE_GAP_BD_ADDR_SIZE];

        for(index = 0; index < bondedDevList.count; index++)
        {
            CYBLE_GAP_SMP_KEY_DIST_T keyInfo;
            uint8 keyflag = 0;

            /* For public address just match the ID Info, no resolution required */
            if(CYBLE_GAP_ADDR_TYPE_PUBLIC == peerBDAddr.type)
            {
                /* Skip public bonded addresses and only check for random addresses if any */
                if(CYBLE_GAP_ADDR_TYPE_PUBLIC != bondedDevList.bdHandleAddrList[index].bdAddr.type)
                {
                    /* Get ID Info for each bdhandle */
                    status = CyBle_GapGetPeerDevSecurityKeyInfo(
                                bondedDevList.bdHandleAddrList[index].bdHandle, &keyflag, &keyInfo);
                    if(CYBLE_ERROR_OK == status)
                    {
                        /* Check if peer address matches to stored public address */
                        if(!memcmp(&keyInfo.idAddrInfo[1], peerBDAddr.bdAddr, sizeof(peerBDAddr.bdAddr)))
                        {
                            /* Set the new public address into this bdhandle */
                            status = CyBle_GapcSetRemoteAddr(bondedDevList.bdHandleAddrList[index].bdHandle, peerBDAddr);
                            if(CYBLE_ERROR_OK == status)
                            {
                                /* Peer address is resolved and set in the bond info */
                                resolutionStatus = 0x00;
                                break;
                            }
                        }
                    }
                }
            }
            else
            {
                /* Get IRK for each bdhandle */
                status = CyBle_GapGetPeerDevSecurityKeyInfo(
                            bondedDevList.bdHandleAddrList[index].bdHandle, &keyflag, &keyInfo);
                if(CYBLE_ERROR_OK == status)
                {
                    /* If IRK is present try to resolve it */
                    if(keyflag & CYBLE_GAP_SMP_INIT_IRK_KEY_DIST)
                    {
                        status = CyBle_GapcResolveDevice(peerBDAddr.bdAddr, (const uint8*)&keyInfo.irkInfo);
                        /* Private address is resolved, set this address */
                        if(CYBLE_ERROR_OK == status)
                        {
                            status = CyBle_GapcSetRemoteAddr(
                                        bondedDevList.bdHandleAddrList[index].bdHandle, peerBDAddr);
                            if(CYBLE_ERROR_OK == status)
                            {
                                /* Peer address is resolved and set in the bond info */
                                resolutionStatus = 0x00;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    CyS_SendEvent(EVT_RESOLVE_AND_SET_PEER_BD_ADDRESS_RESPONSE, currentCmd->opcode,
                    0, sizeof(resolutionStatus), &resolutionStatus);

    /* Send command complete as success */
    return CYBLE_ERROR_OK;
}

CYBLE_API_RESULT_T Cmd_Get_LocalDevSecurityKeys_Api(Command_Format *currentCmd)
{
    uint8 keysFlag = 0;
    CYBLE_GAP_SMP_KEY_DIST_T keyInfo;
    CYBLE_API_RESULT_T status = CyBle_GapGetDevSecurityKeyInfo(&keysFlag, &keyInfo);

    CySmt_SendCommandStatus(currentCmd->opcode, status);
    if(CYBLE_ERROR_OK == status)
    {
        CyS_SendEvent(EVT_GET_LOCAL_DEVICE_SECURITY_KEYS_RESPONSE, currentCmd->opcode,
            sizeof(keyInfo) + ((NUM_KEYS_GENERATED - 1) * sizeof(uint8)), sizeof(keysFlag), &keysFlag);

        Packetize_Send_SMP_Keys(&keyInfo);
    }

    return status;
}

CYBLE_API_RESULT_T Cmd_Get_HostChannelMap_Api(Command_Format *currentCmd)
{
    uint8 channelMap[HOST_CHANNEL_CLASSIFICATION_MAP_SIZE] = {0};
    CYBLE_API_RESULT_T status = CYBLE_ERROR_INVALID_OPERATION;

    cyBle_connHandle = *(CYBLE_CONN_HANDLE_T *)currentCmd->parameters;
    status = CyBle_GapGetChannelMap(cyBle_connHandle.bdHandle, channelMap);
    CySmt_SendCommandStatus(currentCmd->opcode, status);

    /* Send response data, if stack returns OK */
    if(CYBLE_ERROR_OK == status)
    {
        uint8 chMapSize = HOST_CHANNEL_CLASSIFICATION_MAP_SIZE;
        CyS_SendEvent(EVT_GET_HOST_CHANNEL_MAP_RESPONSE, currentCmd->opcode, sizeof(channelMap),
            sizeof(chMapSize), &chMapSize);
        TransmitAdditionalData(channelMap, chMapSize);
    }

    /* Pass on the result to handle command complete event */
    return status;
}

CYBLE_API_RESULT_T Cmd_Remove_Device_From_Bond_List_Api(Command_Format *currentCmd)
{
    return CyBle_GapRemoveBondedDevice((CYBLE_GAP_BD_ADDR_T*)currentCmd->parameters);
}

CYBLE_API_RESULT_T Cmd_Clear_Bond_List_Api(Command_Format *currentCmd)
{
    CYBLE_GAP_BD_ADDR_T peerBDAddr = {{0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u}, 0x00u };
    /* This command doesn't require any parameters */
    (void)currentCmd;

    return CyBle_GapRemoveBondedDevice(&peerBDAddr);
}

#if(CYBLE_M0S8BLESS_VERSION > 1)    /* BLE256DMA and later */
CYBLE_API_RESULT_T Cmd_Set_Conn_Data_Len_Api(Command_Format *currentCmd)
{
    cyBle_connHandle = *(CYBLE_CONN_HANDLE_T *)currentCmd->parameters;

    return CyBle_GapSetDataLength(cyBle_connHandle.bdHandle,
                CyBle_Get16ByPtr(&currentCmd->parameters[sizeof(CYBLE_CONN_HANDLE_T)]),
                CyBle_Get16ByPtr(&currentCmd->parameters[sizeof(CYBLE_CONN_HANDLE_T) + sizeof(uint16)]));
}

CYBLE_API_RESULT_T Cmd_Get_Default_Data_Len_Api(Command_Format *currentCmd)
{
    CYBLE_GAP_DATA_LENGTH_T readParam;
    CYBLE_API_RESULT_T status = CyBle_GapGetDataLength(&readParam);

    CySmt_SendCommandStatus(currentCmd->opcode, status);
    if(CYBLE_ERROR_OK == status)
    {
        CyS_SendEvent(EVT_GET_DATA_LENGTH_RESPONSE, currentCmd->opcode,
            0, sizeof(readParam), (uint8*)&readParam);
    }

    return status;
}

CYBLE_API_RESULT_T Cmd_Set_Default_Data_Len_Api(Command_Format *currentCmd)
{
    return CyBle_GapSetSuggestedDataLength(CyBle_Get16ByPtr(currentCmd->parameters),
                CyBle_Get16ByPtr(&currentCmd->parameters[sizeof(uint16)]));
}

CYBLE_API_RESULT_T Cmd_Convert_OctetToTime_Api(Command_Format *currentCmd)
{
    uint16 time = 0, octets= CyBle_Get16ByPtr(&currentCmd->parameters[sizeof(CYBLE_GAP_PHY_TYPE_T)]);
    CYBLE_API_RESULT_T status = CyBle_GapConvertOctetToTime((CYBLE_GAP_PHY_TYPE_T)*currentCmd->parameters,
                                        octets, &time);

    CySmt_SendCommandStatus(currentCmd->opcode, status);
    if(CYBLE_ERROR_OK == status)
    {
        CyS_SendEvent(EVT_CONVERT_OCTET_TO_TIME_RESPONSE, currentCmd->opcode,
                            sizeof(time), sizeof(octets), (uint8*)&octets);
        TransmitAdditionalData((uint8*)&time, sizeof(time));
    }

    return status;
}

CYBLE_API_RESULT_T Cmd_Get_Resolving_List_Api(Command_Format *currentCmd)
{
    CYBLE_GAP_RESOLVING_LIST_T  curResolvingList;
    CYBLE_API_RESULT_T status = CyBle_GapReadResolvingList(&curResolvingList);

    CySmt_SendCommandStatus(currentCmd->opcode, status);
    if(CYBLE_ERROR_OK == status)
    {
        uint16 index, payloadSize;
        payloadSize = curResolvingList.noOfDevice * (sizeof(CYBLE_GAP_RESOLVING_DEVICE_INFO_T)
                        /* Need to send the size of IRK fields as well as per protocol */
                        + sizeof(uint8) + sizeof(uint8));

        CyS_SendEvent(EVT_GET_RESOLVING_LIST_RESPONSE, currentCmd->opcode,
                        payloadSize, 0, NULL);

        for(index = 0; index < curResolvingList.noOfDevice; index++)
        {
            uint8 fieldSize = sizeof(curResolvingList.resolvingList[index].peerIrk);
            TransmitAdditionalData(&fieldSize, sizeof(fieldSize));
            TransmitAdditionalData(curResolvingList.resolvingList[index].peerIrk, fieldSize);

            fieldSize = sizeof(curResolvingList.resolvingList[index].localIrk);
            TransmitAdditionalData(&fieldSize, sizeof(fieldSize));
            TransmitAdditionalData(curResolvingList.resolvingList[index].localIrk, fieldSize);

            TransmitAdditionalData(curResolvingList.resolvingList[index].bdAddr,
                sizeof(curResolvingList.resolvingList[index].bdAddr));
            TransmitAdditionalData(&curResolvingList.resolvingList[index].type,
                sizeof(curResolvingList.resolvingList[index].type));
        }
    }

    return status;
}

CYBLE_API_RESULT_T Cmd_Add_Device_To_Resolving_List_Api(Command_Format *currentCmd)
{
    CYBLE_GAP_RESOLVING_DEVICE_INFO_T rpaInfo;
    uint8 *paramBuf = currentCmd->parameters;

    /* Each item is followed by the size of the field */
    uint8 index = sizeof(uint8), fieldSize = *paramBuf;

    /* Get PeerIRK size and data */
    memcpy(rpaInfo.peerIrk, &paramBuf[index], fieldSize);
    index += fieldSize;

    /* Get LocalIRK size and data */
    fieldSize = paramBuf[index++];
    memcpy(rpaInfo.localIrk, &paramBuf[index], fieldSize);
    index += fieldSize;

    /* Get bdAddr and Type */
    memcpy(rpaInfo.bdAddr, &paramBuf[index], CYBLE_GAP_BD_ADDR_SIZE);
    index += CYBLE_GAP_BD_ADDR_SIZE;
    rpaInfo.type = paramBuf[index];

    return CyBle_GapAddDeviceToResolvingList(&rpaInfo);
}

CYBLE_API_RESULT_T Cmd_Remove_Device_From_Resolving_List_Api(Command_Format *currentCmd)
{
    return CyBle_GapRemoveDeviceFromResolvingList((CYBLE_GAP_BD_ADDR_T*)currentCmd->parameters);
}

CYBLE_API_RESULT_T Cmd_Clear_Resolving_List_Api(Command_Format *currentCmd)
{
    /* This command doesn't require any parameters */
    (void)currentCmd;

    return CyBle_GapClearResolvingList();
}

CYBLE_API_RESULT_T Cmd_Get_Peer_Resolvable_Addr_Api(Command_Format *currentCmd)
{
    uint8 peerResolvableAddr[CYBLE_GAP_BD_ADDR_SIZE] = {0}, addrType = CYBLE_GAP_ADDR_TYPE_RANDOM;
    CYBLE_API_RESULT_T status = CyBle_GapReadPeerResolvableAddress(
                                (CYBLE_GAP_BD_ADDR_T*)currentCmd->parameters, peerResolvableAddr);

    CySmt_SendCommandStatus(currentCmd->opcode, status);
    if(CYBLE_ERROR_OK == status)
    {
        CyS_SendEvent(EVT_GET_RESOLVABLE_ADDRESS_RESPONSE, currentCmd->opcode,
            sizeof(CYBLE_GAP_BD_ADDR_T) + sizeof(CYBLE_GAP_BD_ADDR_T), 0, NULL);

        TransmitAdditionalData(currentCmd->parameters, sizeof(CYBLE_GAP_BD_ADDR_T));
        TransmitAdditionalData(peerResolvableAddr, sizeof(peerResolvableAddr));
        /* Dummy field with random addr type */
        TransmitAdditionalData(&addrType, sizeof(addrType));
    }
    return status;
}

CYBLE_API_RESULT_T Cmd_Get_Local_Resolvable_Addr_Api(Command_Format *currentCmd)
{
    uint8 localResolvableAddr[CYBLE_GAP_BD_ADDR_SIZE] = {0}, addrType = CYBLE_GAP_ADDR_TYPE_RANDOM;
    CYBLE_API_RESULT_T status = CyBle_GapReadLocalResolvableAddress(
                                (CYBLE_GAP_BD_ADDR_T*)currentCmd->parameters, localResolvableAddr);

    CySmt_SendCommandStatus(currentCmd->opcode, status);
    if(CYBLE_ERROR_OK == status)
    {
        CyS_SendEvent(EVT_GET_RESOLVABLE_ADDRESS_RESPONSE, currentCmd->opcode,
            sizeof(CYBLE_GAP_BD_ADDR_T) + sizeof(CYBLE_GAP_BD_ADDR_T), 0, NULL);

        TransmitAdditionalData(currentCmd->parameters, sizeof(CYBLE_GAP_BD_ADDR_T));
        TransmitAdditionalData(localResolvableAddr, sizeof(localResolvableAddr));
        /* Dummy field with random addr type */
        TransmitAdditionalData(&addrType, sizeof(addrType));
    }
    return status;
}

CYBLE_API_RESULT_T Cmd_Set_Resolvable_Addr_Timeout_Api(Command_Format *currentCmd)
{
    return CyBle_GapSetResolvablePvtAddressTimeOut(CyBle_Get16ByPtr(currentCmd->parameters));
}

/* Macros for Central Address resolution Characteristic, the Handle number is from the GATT DB generated by Component*/
#define GAP_CAR_CHAR_VAL_SIZE    ( 1u )
#define GAP_CAR_CHAR_ATT_HANDLE  ( 0x0007u )

CYBLE_API_RESULT_T Cmd_Addr_Resolution_Control_Api(Command_Format *currentCmd)
{
    uint8 enable = *currentCmd->parameters;
    CYBLE_API_RESULT_T status =  CyBle_GapSetAddressResolutionEnable(enable);

    /* Update the Central Address Resolution Characteristic to reflect the status */
    if(CYBLE_ERROR_OK == status)
    {
        CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;
        uint8 resolutionSupported = (0 == enable) ? 0 : 1;
        /* Central Address Resolution Characteristic Handle from the GATT BD */
        locHandleValuePair.attrHandle = GAP_CAR_CHAR_ATT_HANDLE;
        locHandleValuePair.value.len = GAP_CAR_CHAR_VAL_SIZE;
        locHandleValuePair.value.val = &resolutionSupported;
        if(CYBLE_GATT_ERR_NONE != CyBle_GattsWriteAttributeValue(&locHandleValuePair, 0u,
                                        NULL, CYBLE_GATT_DB_LOCALLY_INITIATED))
        {
            status = CYBLE_ERROR_INVALID_PARAMETER;
        }
    }
    return status;
}

#endif /* CYBLE_M0S8BLESS_VERSION > 1 */

CYBLE_API_RESULT_T Cmd_GenerateLocalP256PublicKey_Api(Command_Format *currentCmd)
{
    CYBLE_API_RESULT_T status = CyBle_GapGenerateLocalP256Keys();

    /* This command doesn't require any parameters */
    (void)currentCmd;

    /* Handle stack events before starting any process, as this blocking call takes longer time */
    CyBle_ProcessEvents();
    return status;
}

CYBLE_API_RESULT_T Cmd_SendSecuredConnectionKeyPress_Api(Command_Format *currentCmd)
{
    cyBle_connHandle = *(CYBLE_CONN_HANDLE_T *)currentCmd->parameters;
    return CyBle_GapAuthSendKeyPress(cyBle_connHandle.bdHandle,
        (CYBLE_GAP_KEYPRESS_NOTIFY_TYPE)currentCmd->parameters[sizeof(CYBLE_CONN_HANDLE_T)]);
}

CYBLE_API_RESULT_T Cmd_GenerateSecuredConnectionOobData_Api(Command_Format *currentCmd)
{
    /* Check if valid random number is provided, else use stack's generated rand */
    if(0 == (*currentCmd->parameters))
    {
        return CyBle_GapGenerateOobData(NULL);
    }
    else
    {
        return CyBle_GapGenerateOobData(&currentCmd->parameters[sizeof(uint8)]);
    }
}

/*************************************************************************
 *  GATT Commands API 
 *************************************************************************/
CYBLE_API_RESULT_T Cmd_Discover_All_Primary_Services_Api(Command_Format *currentCmd)
{
    return CyBle_GattcDiscoverAllPrimaryServices(*(CYBLE_CONN_HANDLE_T *)currentCmd->parameters);
}

CYBLE_API_RESULT_T Cmd_Discover_Primary_Services_By_Uuid_Api(Command_Format *currentCmd)
{
    uint8 index = sizeof(CYBLE_CONN_HANDLE_T);
    uint8 UuidType = 0;
    CYBLE_GATT_VALUE_T value;

    /* Parameter order: connection Handle + UuidType + UUID_Val */
    cyBle_connHandle = *(CYBLE_CONN_HANDLE_T *)currentCmd->parameters;
    UuidType = currentCmd->parameters[index++];
    if(CYBLE_GATT_16_BIT_UUID_FORMAT == UuidType)
    {
        value.len = CYBLE_GATT_16_BIT_UUID_SIZE;
    }
    else if(CYBLE_GATT_128_BIT_UUID_FORMAT == UuidType)
    {
        value.len = CYBLE_GATT_128_BIT_UUID_SIZE;
    }
    else
    {
        /* Do nothing, other UUID types are invalid for BLE central */
    }

    /* Check for valid parameter size */
    if(currentCmd->paramlen != (value.len + index) )
    {
        return CYBLE_ERROR_INVALID_PARAMETER;
    }

    value.val = &currentCmd->parameters[index];
    return CyBle_GattcDiscoverPrimaryServiceByUuid(cyBle_connHandle, value);
}

CYBLE_API_RESULT_T Cmd_Find_Included_Services_Api(Command_Format *currentCmd)
{
    CYBLE_GATT_ATTR_HANDLE_RANGE_T *handleRange = 
        (CYBLE_GATT_ATTR_HANDLE_RANGE_T *)&currentCmd->parameters[sizeof(CYBLE_CONN_HANDLE_T)];

    cyBle_connHandle = *(CYBLE_CONN_HANDLE_T *)currentCmd->parameters;
    /* Store end handle to check for response completion */
    cys_charEndHandle = handleRange->endHandle;

    /* Handle range is followed by connection handle in input byte stream */
    return CyBle_GattcFindIncludedServices(cyBle_connHandle, handleRange);
}

CYBLE_API_RESULT_T Cmd_Discover_All_Characteristics_Api(Command_Format *currentCmd)
{
    CYBLE_GATT_ATTR_HANDLE_RANGE_T *handleRange = 
        (CYBLE_GATT_ATTR_HANDLE_RANGE_T *)&currentCmd->parameters[sizeof(CYBLE_CONN_HANDLE_T)];

    cyBle_connHandle = *(CYBLE_CONN_HANDLE_T *)currentCmd->parameters;
    /* Store end handle to check for response completion */
    cys_charEndHandle = handleRange->endHandle;

    /* Handle range is followed by connection handle in input byte stream */
    return CyBle_GattcDiscoverAllCharacteristics(cyBle_connHandle, *handleRange);
}

static CYBLE_API_RESULT_T Handle_Characteristic_Uuid_Req(bool IsDiscover, Command_Format *currentCmd)
{
    /* Pointer to input byte stream */
    uint8 index = sizeof(CYBLE_CONN_HANDLE_T);
    CYBLE_GATTC_READ_BY_TYPE_REQ_T readByTypeReqParam;
    CYBLE_API_RESULT_T status = CYBLE_ERROR_INVALID_OPERATION;

    /* Expected size of input parameters */
    uint8 expectedParamSize = sizeof(CYBLE_CONN_HANDLE_T) +
                                sizeof(CYBLE_GATT_ATTR_HANDLE_RANGE_T) +
                                sizeof(readByTypeReqParam.uuidFormat);

    /* Parameter order: connection Handle + UuidType + UUID_Val + Handle Range */
    cyBle_connHandle = *(CYBLE_CONN_HANDLE_T *)currentCmd->parameters;
    readByTypeReqParam.uuidFormat = currentCmd->parameters[index++];

    /* Update expected input parameters size and parameter index */
    if(CYBLE_GATT_16_BIT_UUID_FORMAT == readByTypeReqParam.uuidFormat)
    {
        readByTypeReqParam.uuid.uuid16 = CyBle_Get16ByPtr(&currentCmd->parameters[index]);
        expectedParamSize += sizeof(readByTypeReqParam.uuid.uuid16);
        index += sizeof(readByTypeReqParam.uuid.uuid16);
    }
    else if(CYBLE_GATT_128_BIT_UUID_FORMAT == readByTypeReqParam.uuidFormat)
    {
        memcpy(&readByTypeReqParam.uuid.uuid128, &currentCmd->parameters[index], CYBLE_GATT_128_BIT_UUID_SIZE);
        expectedParamSize += sizeof(readByTypeReqParam.uuid.uuid128);
        index += sizeof(readByTypeReqParam.uuid.uuid128);
    }
    else
    {
         /* Other UUID types are not valid for client, server will send 16/128 UUID over the air */
    }

    /* Check if received length is correct */
    if(currentCmd->paramlen == expectedParamSize)
    {
        readByTypeReqParam.range.startHandle = CyBle_Get16ByPtr(&currentCmd->parameters[index]);
        index += sizeof(CYBLE_GATT_DB_ATTR_HANDLE_T);
        readByTypeReqParam.range.endHandle = CyBle_Get16ByPtr(&currentCmd->parameters[index]);

        if(IsDiscover)
        {
            /* Store end handle to check for response completion */
            cys_charEndHandle = readByTypeReqParam.range.endHandle;
            status = CyBle_GattcDiscoverCharacteristicByUuid(cyBle_connHandle, &readByTypeReqParam);
        }
        else
        {
            status = CyBle_GattcReadUsingCharacteristicUuid(cyBle_connHandle, &readByTypeReqParam);
        }
    }
    else
    {
        status = CYBLE_ERROR_INVALID_PARAMETER;
    }

    return status;
}

CYBLE_API_RESULT_T Cmd_Discover_Characteristics_By_Uuid_Api(Command_Format *currentCmd)
{
    /* Discover by UUID Request */
    return Handle_Characteristic_Uuid_Req(true, currentCmd);
}

CYBLE_API_RESULT_T Cmd_Discover_All_Characteristic_Descriptors_Api(Command_Format *currentCmd)
{
    /* Handle range is followed by connection handle in input byte stream */
    CYBLE_GATTC_FIND_INFO_REQ_T *findInfoReq = 
        (CYBLE_GATTC_FIND_INFO_REQ_T *)&currentCmd->parameters[sizeof(CYBLE_CONN_HANDLE_T)];

    cyBle_connHandle = *(CYBLE_CONN_HANDLE_T *)currentCmd->parameters;
    /* Store end handle to check for response completion */
    cys_charEndHandle = findInfoReq->endHandle;
    return CyBle_GattcDiscoverAllCharacteristicDescriptors(cyBle_connHandle, findInfoReq);
}

CYBLE_API_RESULT_T Cmd_Read_Characteristic_Value_Api(Command_Format *currentCmd)
{
    /* Params: connection Handle + Att (Attribute) Handle */
    CYBLE_GATTC_READ_REQ_T *readReq = 
            (CYBLE_GATTC_READ_REQ_T *)&currentCmd->parameters[sizeof(CYBLE_CONN_HANDLE_T)];

    cyBle_connHandle = *(CYBLE_CONN_HANDLE_T *)currentCmd->parameters;
    return CyBle_GattcReadCharacteristicValue(cyBle_connHandle, *readReq);
}

CYBLE_API_RESULT_T Cmd_Read_Using_Characteristic_Uuid_Api(Command_Format *currentCmd)
{
    /* Read by UUID Request */
    return Handle_Characteristic_Uuid_Req(false, currentCmd);
}

CYBLE_API_RESULT_T Cmd_Read_Long_Characteristic_Values_Api(Command_Format *currentCmd)
{
    /* Params: connection Handle + Att Handle + offset */
    CYBLE_GATTC_READ_BLOB_REQ_T *readBlobReq = 
            (CYBLE_GATTC_READ_BLOB_REQ_T *)&currentCmd->parameters[sizeof(CYBLE_CONN_HANDLE_T)];

    cyBle_connHandle = *(CYBLE_CONN_HANDLE_T *)currentCmd->parameters;
    return CyBle_GattcReadLongCharacteristicValues(cyBle_connHandle, readBlobReq);
}

CYBLE_API_RESULT_T Cmd_Read_Multiple_Characteristic_Values_Api(Command_Format *currentCmd)
{
    CYBLE_GATTC_READ_MULT_REQ_T readMultiReqParam;

    readMultiReqParam.listCount = CyBle_Get16ByPtr(&currentCmd->parameters[sizeof(CYBLE_CONN_HANDLE_T)]);

    /* Check for expected length */
    if(currentCmd->paramlen != (sizeof(CYBLE_CONN_HANDLE_T) + sizeof(readMultiReqParam.listCount) +
        (readMultiReqParam.listCount * sizeof(CYBLE_GATT_DB_ATTR_HANDLE_T))) )
    {
        return CYBLE_ERROR_INVALID_PARAMETER;
    }

    cyBle_connHandle = *(CYBLE_CONN_HANDLE_T *)currentCmd->parameters;
    readMultiReqParam.handleList = 
        (uint16*)&currentCmd->parameters[sizeof(CYBLE_CONN_HANDLE_T) + sizeof(uint16)];
    return CyBle_GattcReadMultipleCharacteristicValues(cyBle_connHandle, &readMultiReqParam);
}

static CYBLE_API_RESULT_T Write_item(Write_Req_Type type, Command_Format *currentCmd)
{
    CYBLE_GATTC_WRITE_REQ_T writeReq;
    uint8 index = sizeof(CYBLE_CONN_HANDLE_T);
    CYBLE_API_RESULT_T status = CYBLE_ERROR_INVALID_OPERATION;

    cyBle_connHandle = *(CYBLE_CONN_HANDLE_T *)currentCmd->parameters;
    /* Fill-up CYBLE_GATTC_WRITE_REQ_T */
    writeReq.attrHandle = CyBle_Get16ByPtr(&currentCmd->parameters[index]);
    index += sizeof(writeReq.attrHandle);

    writeReq.value.len = CyBle_Get16ByPtr(&currentCmd->parameters[index]);
    index += sizeof(writeReq.value.len);
    
    writeReq.value.val = &currentCmd->parameters[index];

    /* Check for expected length */
    if( (currentCmd->paramlen != writeReq.value.len + index) || (NULL == writeReq.value.val) )
    {
        status = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else
    {
        /* Write descriptor or value */
        switch(type)
        {
            case CHARACTERISTIC_DESCRIPTOR: 
                status = CyBle_GattcWriteCharacteristicDescriptors(cyBle_connHandle, &writeReq);
                break;

            case CHARACTERISTIC_VALUE: 
                status = CyBle_GattcWriteCharacteristicValue(cyBle_connHandle, &writeReq);
                break;

            case CHARACTERISTIC_VALUE_WITHOUT_RESP: 
                status = CyBle_GattcWriteWithoutResponse(cyBle_connHandle, 
                            (CYBLE_GATTC_WRITE_CMD_REQ_T *)&writeReq);
                break;

            case SIGNED_WRITE_WITHOUT_RESP: 
                status = CyBle_GattcSignedWriteWithoutRsp(cyBle_connHandle, 
                            (CYBLE_GATTC_SIGNED_WRITE_CMD_REQ_T *)&writeReq);
                break;

            default: 
                /* Undefined operation, Do nothing */
                break;
        }
    }

    return status;
}

CYBLE_API_RESULT_T Cmd_Characteristic_Value_Write_Without_Response_Api(Command_Format *currentCmd)
{
    /* Write Value without response */
    return Write_item(CHARACTERISTIC_VALUE_WITHOUT_RESP, currentCmd);
}

CYBLE_API_RESULT_T Cmd_Write_Characteristic_Value_Api(Command_Format *currentCmd)
{
    /* Write Value */
    return Write_item(CHARACTERISTIC_VALUE, currentCmd);
}

static CYBLE_API_RESULT_T Write_Long_item(Write_Req_Type type, Command_Format *currentCmd)
{
    uint8 index = sizeof(CYBLE_CONN_HANDLE_T);
    CYBLE_API_RESULT_T status = CYBLE_ERROR_INVALID_OPERATION;

    cyBle_connHandle = *(CYBLE_CONN_HANDLE_T *)currentCmd->parameters;

    /* Fill-up CYBLE_GATTC_PREP_WRITE_REQ_T */
    gPrepLongWriteReq.handleValuePair.attrHandle = CyBle_Get16ByPtr(&currentCmd->parameters[index]);
    index += sizeof(gPrepLongWriteReq.handleValuePair.attrHandle);

    gPrepLongWriteReq.offset = CyBle_Get16ByPtr(&currentCmd->parameters[index]);
    index += sizeof(gPrepLongWriteReq.offset);

    gPrepLongWriteReq.handleValuePair.value.len = CyBle_Get16ByPtr(&currentCmd->parameters[index]);
    index += sizeof(gPrepLongWriteReq.handleValuePair.value.len);

    /* Use the global UART primary cmd buffer, as no other primary command is accepted until long procedure is complete */
    gPrepLongWriteReq.handleValuePair.value.val = &currentCmd->parameters[index];

    /* Check for expected length */
    if( (currentCmd->paramlen != gPrepLongWriteReq.handleValuePair.value.len + index)
            || (NULL == gPrepLongWriteReq.handleValuePair.value.val) )
    {
        status = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else if(CHARACTERISTIC_DESCRIPTOR == type)
    {
        /* Write descriptor or value */
        status = CyBle_GattcWriteLongCharacteristicDescriptors(cyBle_connHandle, &gPrepLongWriteReq);
    }
    else if(CHARACTERISTIC_VALUE == type)
    {
        status = CyBle_GattcWriteLongCharacteristicValues(cyBle_connHandle, &gPrepLongWriteReq);
    }

    return status;
}

CYBLE_API_RESULT_T Cmd_Write_Long_Characteristic_Value_Api(Command_Format *currentCmd)
{
    /* Write long value */
    return Write_Long_item(CHARACTERISTIC_VALUE, currentCmd);
}

/* Note: CySmart FW module uses Heap section for multiple write operation, 
        so make sure 1K of heap area is allocated */
CYBLE_API_RESULT_T Cmd_Reliable_Characteristic_Value_Writes_Api(Command_Format *currentCmd)
{
    uint8 index, *paramBuf = currentCmd->parameters, ParamIndex = sizeof(CYBLE_CONN_HANDLE_T);
    CYBLE_API_RESULT_T status = CYBLE_ERROR_INVALID_OPERATION;
    CYBLE_GATTC_PREP_WRITE_REQ_T *WriteParamList;

    /* Extract 2 byte count value from parameter list */
    uint8 WritesCount = (uint8)CyBle_Get16ByPtr(&paramBuf[ParamIndex]);

    /* Allocate memory for write request array; this will freed once the primary command is completed */
    heapBuffer = calloc(WritesCount, sizeof(CYBLE_GATTC_PREP_WRITE_REQ_T));

    /* Return FW resources error, if dynamic allocation fails */
    if(NULL == heapBuffer)
    {
        /* Command payload is too big to handle by Dongle, send error code */
        CySmt_SendCommandStatus(currentCmd->opcode, CYS_FW_ERR_INSUFFICIENT_RESOURCES);

        /* Don't send command complete */
        return CYBLE_ERROR_INVALID_PARAMETER;
    }
    WriteParamList = (CYBLE_GATTC_PREP_WRITE_REQ_T *)heapBuffer;

    /* Move the index for writes count */
    ParamIndex += sizeof(uint16);

    for(index = 0; index < WritesCount; index++)
    {
        WriteParamList[index].handleValuePair.attrHandle = 
            (CYBLE_GATT_DB_ATTR_HANDLE_T)CyBle_Get16ByPtr(&paramBuf[ParamIndex]);
        ParamIndex += sizeof(CYBLE_GATT_DB_ATTR_HANDLE_T);

        WriteParamList[index].offset = CyBle_Get16ByPtr(&paramBuf[ParamIndex]);
        ParamIndex += sizeof(uint16);

        WriteParamList[index].handleValuePair.value.len = CyBle_Get16ByPtr(&paramBuf[ParamIndex]);
        ParamIndex += sizeof(uint16);

        /* Send the RX input buffer data directly for GATT Values alone */
        WriteParamList[index].handleValuePair.value.val = &paramBuf[ParamIndex];
        /* Move parameter index by value length */
        ParamIndex += WriteParamList[index].handleValuePair.value.len;
    }

    /* Check for expected length */
    if(currentCmd->paramlen != ParamIndex)
    {
        status = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else
    {
        cyBle_connHandle = *(CYBLE_CONN_HANDLE_T *)paramBuf;
        status = CyBle_GattcReliableWrites(cyBle_connHandle, WriteParamList, WritesCount);
    }
    return status;
}

CYBLE_API_RESULT_T Cmd_Read_Characteristic_Descriptor_Api(Command_Format *currentCmd)
{
    /* Params: connection Handle + Att Handle */

    cyBle_connHandle = *(CYBLE_CONN_HANDLE_T *)currentCmd->parameters;
    return CyBle_GattcReadCharacteristicDescriptors(cyBle_connHandle,
                CyBle_Get16ByPtr(&currentCmd->parameters[sizeof(CYBLE_CONN_HANDLE_T)]));
}

CYBLE_API_RESULT_T Cmd_Read_Long_Characteristic_Descriptor_Api(Command_Format *currentCmd)
{
    /* Params: connection Handle + Att Handle + offset */
    CYBLE_GATTC_READ_BLOB_REQ_T *readBlobReq = 
        (CYBLE_GATTC_READ_BLOB_REQ_T *)&currentCmd->parameters[sizeof(CYBLE_CONN_HANDLE_T)];

    cyBle_connHandle = *(CYBLE_CONN_HANDLE_T *)currentCmd->parameters;
    return CyBle_GattcReadLongCharacteristicDescriptors(cyBle_connHandle, readBlobReq);
}

CYBLE_API_RESULT_T Cmd_Write_Characteristic_Descriptor_Api(Command_Format *currentCmd)
{
    /* Write Descriptor */
    return Write_item(CHARACTERISTIC_DESCRIPTOR, currentCmd);
}

CYBLE_API_RESULT_T Cmd_Write_Long_Characteristic_Descriptor_Api(Command_Format *currentCmd)
{
    /* Write long Descriptor */
    return Write_Long_item(CHARACTERISTIC_DESCRIPTOR, currentCmd);
}

CYBLE_API_RESULT_T Cmd_Exchange_GATT_MTU_Size_Api(Command_Format *currentCmd)
{
    /* Params: connection Handle + MTU Size */
    cyBle_connHandle = *(CYBLE_CONN_HANDLE_T *)currentCmd->parameters;
    return CyBle_GattcExchangeMtuReq(cyBle_connHandle,
                CyBle_Get16ByPtr(&currentCmd->parameters[sizeof(CYBLE_CONN_HANDLE_T)]));
}

CYBLE_API_RESULT_T Cmd_GATT_Stop_Api(Command_Format *currentCmd)
{
    CYBLE_API_RESULT_T status = CYBLE_ERROR_INVALID_OPERATION;

    /* This command doesn't require any parameters */
    (void)currentCmd;

    if(GATT_CMD_IN_PROGRESS)
    {
        CyBle_GattcStopCmd();
        status = CYBLE_ERROR_OK;
    }

    return status;
}

CYBLE_API_RESULT_T Cmd_Signed_Write_Without_Response_Api(Command_Format *currentCmd)
{
    /* Write Value without response */
    return Write_item(SIGNED_WRITE_WITHOUT_RESP, currentCmd);
}

CYBLE_API_RESULT_T Cmd_Execute_Write_Request_Api(Command_Format *currentCmd)
{
    /* Params: connection Handle + MTU Size */
    cyBle_connHandle = *(CYBLE_CONN_HANDLE_T *)currentCmd->parameters;
    return CyBle_GattcSendExecuteWriteReq(cyBle_connHandle, currentCmd->parameters[sizeof(CYBLE_CONN_HANDLE_T)]);
}

/*************************************************************************
 *  L2CAP Commands API 
 *************************************************************************/

CYBLE_API_RESULT_T Cmd_Register_PSM_Api(Command_Format *currentCmd)
{
    uint16 l2capPsm = CyBle_Get16ByPtr(currentCmd->parameters);
    uint16 creditLwm = CyBle_Get16ByPtr(&currentCmd->parameters[sizeof(l2capPsm)]);

    return CyBle_L2capCbfcRegisterPsm(l2capPsm, creditLwm);
}

CYBLE_API_RESULT_T Cmd_Unregister_PSM_Api(Command_Format *currentCmd)
{
    uint16 l2capPsm = CyBle_Get16ByPtr(currentCmd->parameters);

    return CyBle_L2capCbfcUnregisterPsm(l2capPsm);
}

CYBLE_API_RESULT_T Cmd_CBFC_SendConnectionReq_Api(Command_Format *currentCmd)
{
    CYBLE_L2CAP_CBFC_CONNECT_PARAM_T connectParam;
    uint16 remotePsm, localPsm;
    uint8 *paramBuf = currentCmd->parameters;
    uint8 index = sizeof(CYBLE_CONN_HANDLE_T);

    /* Parse input parameters */
    remotePsm = CyBle_Get16ByPtr(&paramBuf[index]);
    index += sizeof(remotePsm);

    localPsm = CyBle_Get16ByPtr(&paramBuf[index]);
    index += sizeof(localPsm);

    connectParam.mtu = CyBle_Get16ByPtr(&paramBuf[index]);
    index += sizeof(connectParam.mtu);

    connectParam.mps = CyBle_Get16ByPtr(&paramBuf[index]);
    index += sizeof(connectParam.mps);

    connectParam.credit = CyBle_Get16ByPtr(&paramBuf[index]);
    index += sizeof(connectParam.credit);

    return CyBle_L2capCbfcConnectReq(((CYBLE_CONN_HANDLE_T *)paramBuf)->bdHandle,
                                        remotePsm, localPsm, &connectParam);
}

CYBLE_API_RESULT_T Cmd_CBFC_SendConnectionResp_Api(Command_Format *currentCmd)
{
    CYBLE_L2CAP_CBFC_CONNECT_PARAM_T connectParam;
    uint16 localCid, responseCode;
    uint8 *paramBuf = currentCmd->parameters;
    uint8 index = 0;

    /* Parse input parameters */
    localCid = CyBle_Get16ByPtr(&paramBuf[index]);
    index += sizeof(localCid);

    responseCode = CyBle_Get16ByPtr(&paramBuf[index]);
    index += sizeof(responseCode);

    connectParam.mtu = CyBle_Get16ByPtr(&paramBuf[index]);
    index += sizeof(connectParam.mtu);

    connectParam.mps = CyBle_Get16ByPtr(&paramBuf[index]);
    index += sizeof(connectParam.mps);

    connectParam.credit = CyBle_Get16ByPtr(&paramBuf[index]);
    index += sizeof(connectParam.credit);

    return CyBle_L2capCbfcConnectRsp(localCid, responseCode, &connectParam);
}

CYBLE_API_RESULT_T Cmd_CBFC_SendFlowControlCredit_Api(Command_Format *currentCmd)
{
    uint16 localCid = CyBle_Get16ByPtr(currentCmd->parameters);
    uint16 credit = CyBle_Get16ByPtr(&currentCmd->parameters[sizeof(localCid)]);

    return CyBle_L2capCbfcSendFlowControlCredit(localCid, credit);
}

CYBLE_API_RESULT_T Cmd_CBFC_SendData_Api(Command_Format *currentCmd)
{
    uint16 localCid, bufferLen;
    uint8 *paramBuf = currentCmd->parameters;
    uint8 index = sizeof(CYBLE_CONN_HANDLE_T);
    CYBLE_API_RESULT_T status = CYBLE_ERROR_INVALID_PARAMETER;

    /* Parse input parameters */
    localCid = CyBle_Get16ByPtr(&paramBuf[index]);
    index += sizeof(localCid);

    bufferLen = CyBle_Get16ByPtr(&paramBuf[index]);
    index += sizeof(bufferLen);

    if((bufferLen + index) == currentCmd->paramlen)
    {
        status = CyBle_L2capChannelDataWrite(((CYBLE_CONN_HANDLE_T *)paramBuf)->bdHandle,
                        localCid, &paramBuf[index], bufferLen);
    }

    return status;
}

CYBLE_API_RESULT_T Cmd_CBFC_SendDisconnectReq_Api(Command_Format *currentCmd)
{
    uint16 localCid = CyBle_Get16ByPtr(currentCmd->parameters);
    return CyBle_L2capDisconnectReq(localCid);
}
#endif /* CYSMART_SUPPORT */

/* [] END OF FILE */
