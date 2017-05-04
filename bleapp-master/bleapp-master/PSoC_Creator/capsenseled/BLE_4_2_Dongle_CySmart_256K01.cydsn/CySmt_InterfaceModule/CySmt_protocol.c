/******************************************************************************
* File Name         : CySmt_protocol.c
* Description       : Handles packet decoding and encoding as per CySmart protocol packet format.
*                     Interfaces with transport layer for packet transfers and and also invokes 
*                     functions to handle each command.
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

/*****************************************************************************
* Header files
*****************************************************************************/
#include "CySmt_CommandLayer.h"

/*****************************************************************************
* Defines used in this file
*****************************************************************************/
#ifdef CYSMART_SUPPORT

/* Wrapper for UART transmit */
#define SendResponseData                            (UART_SpiUartPutArray)

/* CySmart event packet format for status and complete responses */
typedef struct _Event_Status_Response
{
    uint16 lengthinbytes;
    uint16 evt_op_code;
    uint16 cmd_op_code;
    uint16 status;
}Event_Status_Response;
        
/* Only Primary and corresponding secondary commands will be allowed at a time */
Command_Format primaryCmd, secondaryCmd;

/* ON/OFF Status of BLE component */
bool isCySBleStackOn = false;

/* Status of last primary command */
bool primaryCmdInProgress = false;

/* Global flag for CySmart tool connection with Dongle */
bool isCySmtConnected = false; 

/* Command maps on group basis, contains command flags and function references to be invoked */
static const mapping generalMap[] = 
{
    /* Flags - 
        AllowedParamSize - Function reference */
    {(CHECK_PARAMETER_LENGTH | IMMEDIATE_RESPONSE | TRIGGER_COMPLETE),
        0, Cmd_Get_Device_Id_Api},

    {(CHECK_PARAMETER_LENGTH | IMMEDIATE_RESPONSE | TRIGGER_COMPLETE),
        0, Cmd_Get_Supported_Tool_Ver_Api},

    {(CHECK_PARAMETER_LENGTH | IMMEDIATE_RESPONSE | TRIGGER_COMPLETE),
        0, Cmd_Get_Firmware_Version_Api},

    {(CHECK_PARAMETER_LENGTH | IMMEDIATE_RESPONSE | TRIGGER_COMPLETE),
        0, Cmd_Get_Supported_Gap_Roles_Api},

    {(CHECK_PARAMETER_LENGTH | IMMEDIATE_RESPONSE | TRIGGER_COMPLETE),
        0, Cmd_Get_Current_Gap_Role_Api},

    {(CHECK_PARAMETER_LENGTH | IMMEDIATE_RESPONSE | TRIGGER_COMPLETE),
        0, Cmd_Get_Supported_Gatt_Roles_Api},

    {(CHECK_PARAMETER_LENGTH | IMMEDIATE_RESPONSE | TRIGGER_COMPLETE),
        0, Cmd_Get_Current_Gatt_Role_Api},

    {(CHECK_PARAMETER_LENGTH | API_RETURN                           ),
        0, Cmd_Init_Ble_Stack_Api},

    {(DISABLE_ALL_CHECK),
        0, Cmd_Tool_Disconnected_Api},

    {(DISABLE_ALL_CHECK),
        0, Cmd_Host_Timed_Out_Api},

    {(CHECK_PARAMETER_LENGTH | IMMEDIATE_RESPONSE | TRIGGER_COMPLETE),
        0, Cmd_Get_Device_Descriptor_Info},

    {(CHECK_PARAMETER_LENGTH | IMMEDIATE_RESPONSE | TRIGGER_COMPLETE),
        0, Cmd_Get_Hardware_Version_Api},

    {(CHECK_PARAMETER_LENGTH                      | TRIGGER_COMPLETE),
        0, Cmd_Get_Ble_Stack_Version_Api},

    {(CHECK_PARAMETER_LENGTH | IMMEDIATE_RESPONSE | TRIGGER_COMPLETE),
        0, Cmd_Get_Rssi_Api},

    {(CHECK_PARAMETER_LENGTH                      | TRIGGER_COMPLETE),
        sizeof(CYBLE_BLESS_PHY_CH_GRP_ID_T), Cmd_Get_TxPowerLevel_Api},

    {(CHECK_PARAMETER_LENGTH | API_RETURN         | TRIGGER_COMPLETE),
        sizeof(CYBLE_BLESS_PWR_IN_DB_T), Cmd_Set_TxPowerLevel_Api},

    {(CHECK_PARAMETER_LENGTH | API_RETURN         | TRIGGER_COMPLETE),
        (HOST_CHANNEL_CLASSIFICATION_MAP_SIZE + sizeof(uint8)), Cmd_Set_HostChannelClassification_Api},
};

static const mapping gapMap[] = 
{
    /* Flags - 
        AllowedParamSize - Function reference */
    {(CHECK_PARAMETER_LENGTH | API_RETURN         | TRIGGER_COMPLETE),
        sizeof(uint8), Cmd_Set_Device_Io_Capabilities_Api},

    {(CHECK_PARAMETER_LENGTH | IMMEDIATE_RESPONSE | TRIGGER_COMPLETE),
        0, Cmd_Get_Device_Io_Capabilities_Api},

    {(CHECK_PARAMETER_LENGTH                      | TRIGGER_COMPLETE),
        sizeof(uint8),     Cmd_Get_Bluetooth_Device_Address_Api},

    {(CHECK_PARAMETER_LENGTH | API_RETURN         | TRIGGER_COMPLETE),
        sizeof(CYBLE_GAP_BD_ADDR_T), Cmd_Set_Bluetooth_Device_Address_Api},

    {(CHECK_PARAMETER_LENGTH                      | TRIGGER_COMPLETE | SECONDARY_CMD),
        sizeof(uint8), Cmd_Get_Peer_Bluetooth_Device_Address_Api},

    {(CHECK_PARAMETER_LENGTH                      | TRIGGER_COMPLETE),
        (sizeof(CYBLE_GAP_BD_ADDR_T)), Cmd_Get_Peer_Device_Handle_Api},

    {(CHECK_PARAMETER_LENGTH                      | TRIGGER_COMPLETE),
        (sizeof(CYBLE_GAP_ADDR_TYPE_T) + sizeof(uint8) + CYBLE_GAP_SMP_IRK_SIZE), Cmd_GenerateBd_Addr_Api},

    {(                         API_RETURN         | TRIGGER_COMPLETE),
        0, Cmd_Set_Oob_Data_Api},

    {(CHECK_PARAMETER_LENGTH | IMMEDIATE_RESPONSE | TRIGGER_COMPLETE),
        0, Cmd_Get_Connection_Parameters_Api},

    {(                         IMMEDIATE_RESPONSE | TRIGGER_COMPLETE),
        0, Cmd_Set_Connection_Parameters_Api},

    {(CHECK_PARAMETER_LENGTH | IMMEDIATE_RESPONSE | TRIGGER_COMPLETE),
        0, Cmd_Get_Scan_Parameters_Api},

    {(                         IMMEDIATE_RESPONSE | TRIGGER_COMPLETE),
        0, Cmd_Set_Scan_Parameters_Api},

    {(CHECK_PARAMETER_LENGTH | IMMEDIATE_RESPONSE | TRIGGER_COMPLETE),
        0, Cmd_Get_Local_Device_Security_Api},

    {(CHECK_PARAMETER_LENGTH | API_RETURN         | TRIGGER_COMPLETE),
        (sizeof(CYBLE_GAP_AUTH_INFO_T) + sizeof(uint8)), Cmd_Set_Local_Device_Security_Api},

    {(CHECK_PARAMETER_LENGTH                      | TRIGGER_COMPLETE),
        sizeof(uint8), Cmd_Get_Peer_Device_Security_Api},

    {(CHECK_PARAMETER_LENGTH |                      TRIGGER_COMPLETE | SECONDARY_CMD),
        0, Cmd_Get_White_List_Api},

    {(CHECK_PARAMETER_LENGTH | API_RETURN         | TRIGGER_COMPLETE | SECONDARY_CMD),
        (sizeof(CYBLE_GAP_BD_ADDR_T)), Cmd_Add_Device_To_White_List_Api},

    {(CHECK_PARAMETER_LENGTH | API_RETURN         | TRIGGER_COMPLETE | SECONDARY_CMD),
        (sizeof(CYBLE_GAP_BD_ADDR_T)), Cmd_Remove_Device_From_White_List_Api},

    {(CHECK_PARAMETER_LENGTH | API_RETURN         | TRIGGER_COMPLETE | SECONDARY_CMD),
        0, Cmd_Clear_White_List_Api},

    {(CHECK_PARAMETER_LENGTH | API_RETURN                           ),
        0, Cmd_Start_Scan_Api},

    {(CHECK_PARAMETER_LENGTH | API_RETURN                            | SECONDARY_CMD),
        0, Cmd_Stop_Scan_Api},

    {(CHECK_PARAMETER_LENGTH |                      TRIGGER_COMPLETE),
        sizeof(uint8), Cmd_Generate_Set_Keys_Api},

    {(CHECK_PARAMETER_LENGTH |                      TRIGGER_COMPLETE),
        (sizeof(CYBLE_GAP_SMP_KEY_DIST_T) + ((NUM_KEYS_GENERATED) * sizeof(uint8))), Cmd_Set_Authentication_Keys_Api},

    {(                         API_RETURN),
        0, Cmd_Establish_Connection_Api},

    {(CHECK_PARAMETER_LENGTH | API_RETURN                             | SECONDARY_CMD),
        sizeof(CYBLE_CONN_HANDLE_T), Cmd_Terminate_Connection_Api},

    {(CHECK_PARAMETER_LENGTH | API_RETURN                           ),
        sizeof(CYBLE_CONN_HANDLE_T), Cmd_Initiate_Pairing_Request_Api},

    {(CHECK_PARAMETER_LENGTH | API_RETURN         | TRIGGER_COMPLETE),
        sizeof(CYBLE_GAP_BD_ADDR_T), Cmd_Set_Identiry_Addr_Api},

    {(CHECK_PARAMETER_LENGTH | API_RETURN         | TRIGGER_COMPLETE | SECONDARY_CMD),
        (sizeof(CYBLE_CONN_HANDLE_T) + sizeof(uint32) + sizeof(uint8)), Cmd_Pairing_PassKey_Api},

    {(CHECK_PARAMETER_LENGTH | API_RETURN         | TRIGGER_COMPLETE),
        (sizeof(CYBLE_CONN_HANDLE_T) + sizeof(CYBLE_GAP_CONN_UPDATE_PARAM_T)), Cmd_Update_Connection_Params_Api},

    {(                                              TRIGGER_COMPLETE | SECONDARY_CMD),
        0, Cmd_Cancle_Connection_Api},

    {(CHECK_PARAMETER_LENGTH |                      TRIGGER_COMPLETE | SECONDARY_CMD),
        0, Cmd_Get_Bonded_Devices_By_Rank_Api},

    {(CHECK_PARAMETER_LENGTH | API_RETURN         | TRIGGER_COMPLETE),
        (sizeof(CYBLE_CONN_HANDLE_T) + sizeof(uint16)), Cmd_UpdateConnectionParam_Resp_Api},

    {(CHECK_PARAMETER_LENGTH |                      TRIGGER_COMPLETE),
        (sizeof(uint8)), Cmd_Get_PeerDevice_SecurityKeys_Api},

    {(CHECK_PARAMETER_LENGTH |                      TRIGGER_COMPLETE),
        (sizeof(CYBLE_GAP_BD_ADDR_T)), Cmd_Resolve_Set_Peer_Addr_Api},

    {(CHECK_PARAMETER_LENGTH                      | TRIGGER_COMPLETE | SECONDARY_CMD),
        0, Cmd_Get_LocalDevSecurityKeys_Api},

    {(CHECK_PARAMETER_LENGTH                      | TRIGGER_COMPLETE),
        sizeof(CYBLE_CONN_HANDLE_T), Cmd_Get_HostChannelMap_Api},

    {(CHECK_PARAMETER_LENGTH | API_RETURN         | TRIGGER_COMPLETE | SECONDARY_CMD),
        (sizeof(CYBLE_GAP_BD_ADDR_T)), Cmd_Remove_Device_From_Bond_List_Api},

    {(CHECK_PARAMETER_LENGTH | API_RETURN         | TRIGGER_COMPLETE | SECONDARY_CMD),
        0, Cmd_Clear_Bond_List_Api},

#if(CYBLE_M0S8BLESS_VERSION > 1)    /* BLE256DMA and later */

    {(CHECK_PARAMETER_LENGTH | API_RETURN         | TRIGGER_COMPLETE),
        (sizeof(CYBLE_CONN_HANDLE_T) + sizeof(uint16) + sizeof(uint16)), Cmd_Set_Conn_Data_Len_Api},

    {(CHECK_PARAMETER_LENGTH                      | TRIGGER_COMPLETE | SECONDARY_CMD),
        0, Cmd_Get_Default_Data_Len_Api},

    {(CHECK_PARAMETER_LENGTH | API_RETURN         | TRIGGER_COMPLETE),
        (sizeof(uint16) + sizeof(uint16)), Cmd_Set_Default_Data_Len_Api},

    {(CHECK_PARAMETER_LENGTH |                      TRIGGER_COMPLETE),
        (sizeof(CYBLE_GAP_PHY_TYPE_T) + sizeof(uint16)), Cmd_Convert_OctetToTime_Api},

    {(CHECK_PARAMETER_LENGTH |                      TRIGGER_COMPLETE | SECONDARY_CMD),
        0, Cmd_Get_Resolving_List_Api},

    {(                         API_RETURN         | TRIGGER_COMPLETE | SECONDARY_CMD),
        0, Cmd_Add_Device_To_Resolving_List_Api},

    {(CHECK_PARAMETER_LENGTH | API_RETURN         | TRIGGER_COMPLETE | SECONDARY_CMD),
        (sizeof(CYBLE_GAP_BD_ADDR_T)), Cmd_Remove_Device_From_Resolving_List_Api},

    {(CHECK_PARAMETER_LENGTH | API_RETURN         | TRIGGER_COMPLETE | SECONDARY_CMD),
        0, Cmd_Clear_Resolving_List_Api},

    {(CHECK_PARAMETER_LENGTH                      | TRIGGER_COMPLETE | SECONDARY_CMD),
        0, Cmd_Get_Peer_Resolvable_Addr_Api},

    {(CHECK_PARAMETER_LENGTH                      | TRIGGER_COMPLETE | SECONDARY_CMD),
        0, Cmd_Get_Local_Resolvable_Addr_Api},

    {(CHECK_PARAMETER_LENGTH | API_RETURN         | TRIGGER_COMPLETE | SECONDARY_CMD),
        (sizeof(uint16)), Cmd_Set_Resolvable_Addr_Timeout_Api},

    {(CHECK_PARAMETER_LENGTH | API_RETURN         | TRIGGER_COMPLETE | SECONDARY_CMD),
        (sizeof(uint8)), Cmd_Addr_Resolution_Control_Api},

#endif /* CYBLE_M0S8BLESS_VERSION > 1 */

    /* Send the status response before invoking this long blocking call */
    {(CHECK_PARAMETER_LENGTH | IMMEDIATE_RESPONSE | TRIGGER_COMPLETE),
        0, Cmd_GenerateLocalP256PublicKey_Api},

    {(CHECK_PARAMETER_LENGTH | API_RETURN         | TRIGGER_COMPLETE | SECONDARY_CMD),
        (sizeof(CYBLE_CONN_HANDLE_T) + sizeof(CYBLE_GAP_KEYPRESS_NOTIFY_TYPE)), Cmd_SendSecuredConnectionKeyPress_Api},

    {(                         API_RETURN),
        0, Cmd_GenerateSecuredConnectionOobData_Api},

};

static const mapping gattMap[] = 
{
    /* Flags - 
        AllowedParamSize - Function reference */
    {(CHECK_PARAMETER_LENGTH | API_RETURN),
        sizeof(CYBLE_CONN_HANDLE_T), Cmd_Discover_All_Primary_Services_Api},

    {                         (API_RETURN),
        0, Cmd_Discover_Primary_Services_By_Uuid_Api},

    {(CHECK_PARAMETER_LENGTH | API_RETURN),
        (sizeof(CYBLE_CONN_HANDLE_T) + sizeof(CYBLE_GATT_ATTR_HANDLE_RANGE_T)), 
            Cmd_Find_Included_Services_Api},

    {(CHECK_PARAMETER_LENGTH | API_RETURN),
        sizeof(CYBLE_CONN_HANDLE_T) + sizeof(CYBLE_GATT_ATTR_HANDLE_RANGE_T), 
            Cmd_Discover_All_Characteristics_Api},

    {                         (API_RETURN),
        0, Cmd_Discover_Characteristics_By_Uuid_Api},

    {(CHECK_PARAMETER_LENGTH | API_RETURN),
        sizeof(CYBLE_CONN_HANDLE_T) + sizeof(CYBLE_GATTC_FIND_INFO_REQ_T), 
            Cmd_Discover_All_Characteristic_Descriptors_Api},

    {(CHECK_PARAMETER_LENGTH | API_RETURN),
        sizeof(CYBLE_CONN_HANDLE_T) + sizeof(CYBLE_GATTC_READ_REQ_T), 
            Cmd_Read_Characteristic_Value_Api},

    {                         (API_RETURN),
        0, Cmd_Read_Using_Characteristic_Uuid_Api},

    {(CHECK_PARAMETER_LENGTH | API_RETURN),
        sizeof(CYBLE_CONN_HANDLE_T) + sizeof(CYBLE_GATTC_READ_BLOB_REQ_T), 
            Cmd_Read_Long_Characteristic_Values_Api},

    {                         (API_RETURN),
        0, Cmd_Read_Multiple_Characteristic_Values_Api},

    {                         (API_RETURN         | TRIGGER_COMPLETE),
        0, Cmd_Characteristic_Value_Write_Without_Response_Api},

    {                         (API_RETURN),
        0, Cmd_Write_Characteristic_Value_Api},

    {                         (API_RETURN),
        0, Cmd_Write_Long_Characteristic_Value_Api},

    {                         (API_RETURN),
        0, Cmd_Reliable_Characteristic_Value_Writes_Api},

    {(CHECK_PARAMETER_LENGTH | API_RETURN),
        sizeof(CYBLE_CONN_HANDLE_T) + sizeof(CYBLE_GATTC_READ_REQ_T), 
            Cmd_Read_Characteristic_Descriptor_Api},

    {(CHECK_PARAMETER_LENGTH | API_RETURN),
        sizeof(CYBLE_CONN_HANDLE_T) + sizeof(CYBLE_GATTC_READ_BLOB_REQ_T), 
            Cmd_Read_Long_Characteristic_Descriptor_Api},

    {                         (API_RETURN),
        0, Cmd_Write_Characteristic_Descriptor_Api},

    {                         (API_RETURN),
        0, Cmd_Write_Long_Characteristic_Descriptor_Api},

    {(CHECK_PARAMETER_LENGTH | API_RETURN),
        sizeof(CYBLE_CONN_HANDLE_T) + sizeof(uint16), Cmd_Exchange_GATT_MTU_Size_Api},

    {(CHECK_PARAMETER_LENGTH | API_RETURN                            | SECONDARY_CMD),
        sizeof(CYBLE_CONN_HANDLE_T), Cmd_GATT_Stop_Api},

    {                         (API_RETURN         | TRIGGER_COMPLETE),
        0, Cmd_Signed_Write_Without_Response_Api},

    {(CHECK_PARAMETER_LENGTH | API_RETURN),
        sizeof(CYBLE_CONN_HANDLE_T) + sizeof(uint8), Cmd_Execute_Write_Request_Api}
};

static const mapping l2capMap[] = 
{
    /* Flags - 
        AllowedParamSize - Function reference */
    {(CHECK_PARAMETER_LENGTH | API_RETURN         | TRIGGER_COMPLETE),
        sizeof(uint16) + sizeof(uint16), Cmd_Register_PSM_Api},

    {(CHECK_PARAMETER_LENGTH | API_RETURN         | TRIGGER_COMPLETE),
        sizeof(uint16), Cmd_Unregister_PSM_Api},

    {(CHECK_PARAMETER_LENGTH | API_RETURN         ),
        sizeof(CYBLE_CONN_HANDLE_T) + sizeof(uint16) + sizeof(uint16) + sizeof(CYBLE_L2CAP_CBFC_CONNECT_PARAM_T),
        Cmd_CBFC_SendConnectionReq_Api},

    {(CHECK_PARAMETER_LENGTH | API_RETURN         | TRIGGER_COMPLETE),
        sizeof(uint16) + sizeof(uint16) + sizeof(CYBLE_L2CAP_CBFC_CONNECT_PARAM_T),
        Cmd_CBFC_SendConnectionResp_Api},

    {(CHECK_PARAMETER_LENGTH | API_RETURN         | TRIGGER_COMPLETE),
        sizeof(uint16) + sizeof(uint16), Cmd_CBFC_SendFlowControlCredit_Api},

    {(                         API_RETURN),
        0, Cmd_CBFC_SendData_Api},

    {(CHECK_PARAMETER_LENGTH | API_RETURN),
        sizeof(uint16), Cmd_CBFC_SendDisconnectReq_Api}
};

/*******************************************************************************
* Function Name: CyS_SendEvent
********************************************************************************
*
* Summary:
*  Sends response events as per protocol format
*  Provides support to optionally send additional data by the caller
*
* Parameters:
*  EventOpCode:        Event op-code to be sent
*
*  CommandOpCode: Command op-code to be appended
*                               pass "0" if command code is not required
*
*  addlDataLen:         Number of additional bytes to be added to payload size, 
*                               additional payload data should be transmitted by the caller
*
*  ParamSize:               Number of bytes to be sent along with event header
*
*  parameters:          Pointer to transmit data buffer, caller should make sure to pass valid pointer
*
* Return:
*  NONE
*
* Theory:
*  NONE
*
* Side Effects:
*  If "addlDataLen" is non zero, this function should always be followed by TransmitAdditionalData()
*
* Note:
*
*******************************************************************************/
void CyS_SendEvent(Event EventOpCode, uint16 CommandOpCode, uint16 addlDataLen,
                        uint16 ParamSize, const uint8 *parameters)
{
    Event_Status_Response evt;
    uint8 evtHeaderSize;
    uint16 evtHeader = CYSMT_EVT_HEADER_CODE;

    /* Send out Header first, it will be followed by response packet */
    SendResponseData((uint8 *)&evtHeader, sizeof(evtHeader));

    evt.evt_op_code = (uint16)EventOpCode;

    /* Add event op-code length to parameter size */
    evt.lengthinbytes = ParamSize + sizeof(evt.evt_op_code);

    /* Event packet header contains:  packet size + event code + optional command code */
    evtHeaderSize = sizeof(evt.lengthinbytes) + sizeof(evt.evt_op_code);

    /* Add Command op-code and it's size if required */
    if(0 != CommandOpCode)
    {
        evt.cmd_op_code = CommandOpCode;
        /* Add command code size for total length and packet header length */
        evtHeaderSize += sizeof(CommandOpCode);
        evt.lengthinbytes += sizeof(CommandOpCode);
    }
    
    /* Add additional data length which will be sent as follow-up packet */
    evt.lengthinbytes += addlDataLen;
    
    /* Send event header */
    SendResponseData((uint8 *)&evt, evtHeaderSize);

    /* Send payload data */
    if((ParamSize) && parameters != NULL)
    {
        SendResponseData(parameters, ParamSize);
    }
}

/*******************************************************************************
* Function Name: CySmt_SendCommandStatus
********************************************************************************
*
* Summary:
*  Sends status response for command, also clears current command if status is not OK
*
* Parameters:
*  CommandOpCode: Command op-code to be appended
*
*  status:                 Return status from stack, will be transmitted as 2-byte value 
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
void CySmt_SendCommandStatus(uint16 CommandOpCode, uint16 status)
{
    Event_Status_Response evt;
    uint16 evtHeader = CYSMT_EVT_HEADER_CODE;

    /* Current command processing is complete if status is not OK */
    if(CYBLE_ERROR_OK != status)
    {
        if(primaryCmd.opcode == CommandOpCode)
        {
            /* Primary command processing is complete */
            primaryCmdInProgress = false;
            memset(primaryCmdRxBuf, 0, sizeof(primaryCmdRxBuf));
        }
        else
        {
            memset(secondaryCmdRxBuf, 0, sizeof(secondaryCmdRxBuf));
        }
    }

    /* Send out Header first, it will be followed by response packet */
    SendResponseData((uint8 *)&evtHeader, sizeof(evtHeader));


    /* Length for STATUS and COMPLETE events: 2 (EVT_OP_CODE) + 2 (CMD_OP_CODE) + 2 (STATUS) */
    evt.lengthinbytes = sizeof(evt.evt_op_code) + sizeof(CommandOpCode) + sizeof(evt.status);

    evt.evt_op_code = (uint16)EVT_COMMAND_STATUS;
    evt.cmd_op_code = CommandOpCode;
    evt.status = status;
    
    SendResponseData((uint8 *)&evt, sizeof(evt.lengthinbytes) + evt.lengthinbytes);
}

/*******************************************************************************
* Function Name: CySmt_SendCommandComplete
********************************************************************************
*
* Summary:
*  Sends complete response for command, clears current command
*
* Parameters:
*  CommandOpCode: Command op-code to be appended
*
*  status:                 Return status from stack, will be transmitted as 2-byte value 
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
void CySmt_SendCommandComplete(uint16 CommandOpCode, uint16 status)
{
    Event_Status_Response evt;
    uint16 evtHeader = CYSMT_EVT_HEADER_CODE;

    /* Send out Header first, it will be followed by response packet */
    SendResponseData((uint8 *)&evtHeader, sizeof(evtHeader));

    if(primaryCmd.opcode == CommandOpCode)
    {
        /* Primary command processing is complete */
        primaryCmdInProgress = false;
        memset(primaryCmdRxBuf, 0, sizeof(primaryCmdRxBuf));
    }
    else
    {
        /* Secondary buffer should only be filled after current secondary command is completed */
        memset(secondaryCmdRxBuf, 0, sizeof(secondaryCmdRxBuf));
    }

    /* Set Tool connected flag, if stack is ON */
    if(isCySBleStackOn & (CYBLE_ERROR_OK == status))
    {
        isCySmtConnected = true;
    }

    /* Length for STATUS and COMPLETE events: 2 (EVT_OP_CODE) + 2 (CMD_OP_CODE) + 1 (STATUS) */
    evt.lengthinbytes = sizeof(evt.evt_op_code) + sizeof(CommandOpCode) + sizeof(evt.status);

    evt.evt_op_code = (uint16)EVT_COMMAND_COMPLETE;
    evt.cmd_op_code = CommandOpCode;
    evt.status = status;
    
    SendResponseData((uint8 *)&evt, sizeof(evt.lengthinbytes) + evt.lengthinbytes);
}

/*******************************************************************************
* Function Name: Get_Cmd
********************************************************************************
*
* Summary:
*  Utility function to check RX buffers and parse the current command to be handled
*   If any command is already in progress, returns secondary command
*
* Parameters:
*  NONE
*
* Return:
*  Pointer to current command to be handled
*
* Theory:
*  NONE
*
* Side Effects:
*
* Note:
*
*******************************************************************************/
static Command_Format* Get_Cmd(void)
{
    uint8 *currentCmdBuf = primaryCmdRxBuf;
    Command_Format *currentCmd = &primaryCmd;

    if(primaryCmdInProgress)
    {
        currentCmdBuf = secondaryCmdRxBuf;
        currentCmd = &secondaryCmd;
    }
    
    /* Data is in Little endian Format. Copy 16 bit Command data */
    currentCmd->opcode = CyBle_Get16ByPtr(currentCmdBuf);

    /* Copy 16 bit data length */
    currentCmd->paramlen = CyBle_Get16ByPtr(&currentCmdBuf[sizeof(uint16)]);

    /* If length of data is non-zero, refer to parameters copied in command buffer */
    if(currentCmd->paramlen)
    {
        /* Update pointer to RX buffer */
        currentCmd->parameters = &currentCmdBuf[COMMAND_HEADER_SIZE];
    }
    return currentCmd;
}

/*******************************************************************************
* Function Name: CySmt_ProcessCommands
********************************************************************************
*
* Summary:
* This function will trigger the required stack operations via command wrapper functions.
*  It performs checks for validity of command before triggering the operation and uses
*   command flags to process status/complete responses
*
* Parameters:
*  NONE
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
void CySmt_ProcessCommands(void)
{
    CYBLE_API_RESULT_T status = CYBLE_ERROR_NO_CONNECTION;
    const mapping *map = NULL;
    Command_Format *currentCmd = Get_Cmd();
    uint8 commandID = currentCmd->opcode & CYS_CMD_CID_MASK;
    Command_Group commandGrp = (Command_Group)((currentCmd->opcode >> CYS_CMD_CG_SHIFT) & CYS_CMD_CG_MASK);

    /* Check Command Group (CG) */
    switch(commandGrp)
    {
        case GENERAL_GROUP:
            /* Check if command is in supported list */
            if(commandID < sizeof(generalMap)/sizeof(mapping))
            {
                map = &generalMap[commandID];
            }
            break;

        case HCI_GROUP:
            break;

        case L2CAP_GROUP:
            /* Check if command is in supported list */
            if(commandID < sizeof(l2capMap)/sizeof(mapping))
            {
                map = &l2capMap[commandID];
            }
            break;

        case ATT_GROUP:
            break;

        case GATT_GROUP:
            /* Check if command is in supported list */
            if(commandID < sizeof(gattMap)/sizeof(mapping))
            {
                map = &gattMap[commandID];
            }
            break;

        case GAP_GROUP:
            /* Check if command is in supported list */
            if(commandID < sizeof(gapMap)/sizeof(mapping))
            {
                map = &gapMap[commandID];
            }
            break;

        default:
            /* Invalid group, return invalid operation */
            break;
    }

    if( (!isCySBleStackOn) && (GENERAL_GROUP != commandGrp) )
    {
        /* Accept only general commands, If BLE stack is not running */
        /* Clear this command, ISR can copy next command */
        newCmdRxDoneFlag = false;

        CySmt_SendCommandStatus(currentCmd->opcode, CYBLE_ERROR_INVALID_OPERATION);
    }

    /* Only secondary commands are allowed from non-general groups if any primary command is in progress */
    if( (primaryCmdInProgress && (GENERAL_GROUP != commandGrp) && (!(map->flags & SECONDARY_CMD)))
          || (NULL == map) || (NULL == map->fnptr) )
    {
        /* Clear this command, ISR can copy next command */
        newCmdRxDoneFlag = false;

        CySmt_SendCommandStatus(currentCmd->opcode, CYBLE_ERROR_INVALID_OPERATION);
    }
    else if(MAX_PAYLOAD_SIZE < currentCmd->paramlen)
    {
        /* Clear this command, ISR can copy next command */
        newCmdRxDoneFlag = false;

        /* Command payload is too big to handle by Dongle, send error code */
        CySmt_SendCommandStatus(currentCmd->opcode, CYS_FW_ERR_INSUFFICIENT_RESOURCES);
    }
    else if( ((map->flags & CHECK_PARAMETER_LENGTH) && (map->AllowedParamSize != currentCmd->paramlen))
        || ( (currentCmd->paramlen) && (NULL == currentCmd->parameters)) )
    {
        /* Clear this command, ISR can copy next command */
        newCmdRxDoneFlag = false;

        /* Command parameters and length are invalid as per command definition */
        CySmt_SendCommandStatus(currentCmd->opcode, CYBLE_ERROR_INVALID_PARAMETER);
    }
    else
    {
        /* Send BLE_STATUS_OK for commands which require only local data */
        if(map->flags & IMMEDIATE_RESPONSE)
        {
            CySmt_SendCommandStatus(currentCmd->opcode, CYBLE_ERROR_OK);
        }

        /* Handle stack events before invoking any new command */
        CyBle_ProcessEvents();

        /* Set command in progress flag */
        primaryCmdInProgress = true;

        /* Clear this command, ISR can copy next command */
        newCmdRxDoneFlag = false;

        /* Invoke the Command API */
        status = (map->fnptr(currentCmd));

        /* Send status response from the API */
        if(map->flags & API_RETURN)
        {
            CySmt_SendCommandStatus(currentCmd->opcode, status);
        }

        /* Send complete for non progress commands */
        if( (CYBLE_ERROR_OK == status) && (map->flags & TRIGGER_COMPLETE) )
        {
            CySmt_SendCommandComplete(currentCmd->opcode, status);
        }
    }
}
#endif /* CYSMART_SUPPORT */

/* [] END OF FILE */
