/******************************************************************************
* File Name         : CySmt_protocol.h
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

#ifndef _CYSMT_PROTOCOL_H_
#define _CYSMT_PROTOCOL_H_
    
#include "CySmt_TransportLayer.h"

/* Macros to decode command group from command op-code */
#define CYS_CMD_CG_SHIFT        (0x07u)
#define CYS_CMD_CID_MASK        (0x7Fu)
#define CYS_CMD_CG_MASK         (0x07u)

/* Check if any primary GATT command is in progress */
#define GATT_CMD_IN_PROGRESS    (GATT_GROUP == ((primaryCmd.opcode >> CYS_CMD_CG_SHIFT) & CYS_CMD_CG_MASK))

/* HCI code to be sent as start header of CySmart event packet */
#define CYSMT_EVT_HEADER_CODE   (0xA7BDu)

/*****************************************************************************
* Typedefs
*****************************************************************************/        

/* CySmart command packet format */
typedef struct _Command_Format
{
    /* command opcode as per protocol */
    uint16 opcode;
    /* Total length of payload */
    uint16 paramlen;
    /* Payload pointer to buffer */
    uint8 *parameters;
}Command_Format; 

/* Generic function pointer for all commands, function arguments will be redundant for some commands */
typedef CYBLE_API_RESULT_T (*fn_pointer)(Command_Format *currentCmd); 

/* Command property map */
typedef struct _Mapping_
{
    /* Flags to perform required checks for sending status/complete responses */
    uint8 flags;

    /* Total expected parameter size of each command */
    uint16 AllowedParamSize;

    /* Function pointer to perform the action required for each command */
    fn_pointer fnptr;
}mapping;

/* Command groups as per protocol */
typedef enum
{
    GENERAL_GROUP = 0,
    HCI_GROUP,
    L2CAP_GROUP,
    ATT_GROUP,
    GATT_GROUP,
    GAP_GROUP,
    RESERVED,
}Command_Group;

/* Command flags for checking input parameters and to send response events */
#define DISABLE_ALL_CHECK                                       (0x0u)
#define CHECK_PARAMETER_LENGTH                                  (0x1u)
#define IMMEDIATE_RESPONSE                                      (0x2u)
#define API_RETURN                                              (0x4u)
#define TRIGGER_COMPLETE                                        (0x8u)
#define SECONDARY_CMD                                           (0x10u)

/* CySmart Event opcodes as per protocol */
typedef enum
{
    /* General Events */
    EVT_COMMAND_STATUS                                          = 0x047Eu,
    EVT_COMMAND_COMPLETE                                        = 0x047Fu,
    EVT_GET_DEVICE_ID_RESPONSE                                  = 0x0400u,
    EVT_GET_SUPPORTED_TOOL_VERSION_RESPONSE                     = 0x0401u,
    EVT_GET_FIRMWARE_VERSION_RESPONSE                           = 0x0402u,
    EVT_GET_BLE_STACK_VERSION_RESPONSE                          = 0x0403u,
    EVT_REPORT_STACK_MISC_STATUS                                = 0x0404u,
    EVT_GET_SUPPORTED_GAP_ROLES_RESPONSE                        = 0x0405u,
    EVT_GET_CURRENT_GAP_ROLE_RESPONSE                           = 0x0406u,
    EVT_GET_SUPPORTED_GATT_ROLES_RESPONSE                       = 0x0407u,
    EVT_GET_CURRENT_GATT_ROLE_RESPONSE                          = 0x0408u,
    EVT_GET_RSSI_RESPONSE                                       = 0x0409u,
    EVT_GET_DEVICE_DESCRIPTION_RESPONSE                         = 0x040Au,
    EVT_GET_HARDWARE_VERSION_RESPONSE                           = 0x040Bu,
    EVT_GET_TX_POWER_RESPONSE                                   = 0x040Cu,

    /* FW specific events, not used by CySmart tool */
    HID_EP1_PACKET                                              = 0x0461u,
    HID_EP2_PACKET                                              = 0x0462u,
    AUDIO_REPORT                                                = 0x0463u,
    AUDIO_CONTROL_STATUS                                        = 0x0464u,
    AUDIO_SYNC_PACKET                                           = 0x0465u,
    AUDIO_RAW_DATA                                              = 0x0466u,

    /* GATT Events */
    EVT_DISCOVER_ALL_PRIMARY_SERVICES_RESULT_PROGRESS           = 0x0600u,
    EVT_DISCOVER_PRIMARY_SERVICES_BY_UUID_RESULT_PROGRESS       = 0x0601u,
    EVT_FIND_INDLUDED_SERVICES_RESULT_PROGRESS                  = 0x0602u,
    EVT_DISCOVER_ALL_CHARACTERISTICS_RESULT_PROGRESS            = 0x0603u,
    EVT_DISCOVER_CHARACTERISTICS_BY_UUID_RESULT_PROGRESS        = 0x0604u,
    EVT_DISCOVER_ALL_CHARACTERISTIC_DESCRIPTORS_RESULT_PROGRESS = 0x0605u,
    EVT_READ_CHARACTERISTIC_VALUE_RESPONSE                      = 0x0606u,
    EVT_READ_USING_CHARACTERISTIC_UUID_RESPONSE                 = 0x0607u,
    EVT_READ_LONG_CHARACTERISTIC_VALUE_RESPONSE                 = 0x0608u,
    EVT_READ_MULTIPLE_CHARACTERISTIC_VALUES_RESPONSE            = 0x0609u,
    EVT_READ_CHARACTERISTIC_DESCRIPTOR_RESPONSE                 = 0x060Au,
    EVT_READ_LONG_CHARACTERISTIC_DESCRIPTOR_RESPONSE            = 0x060Bu,
    EVT_CHARACTERISTIC_VALUE_NOTIFICATION                       = 0x060Cu,
    EVT_CHARACTERISTIC_VALUE_INDICATION                         = 0x060Du,
    EVT_GATT_ERROR_NOTIFICATION                                 = 0x060Eu,
    EVT_EXCHANGE_GATT_MTU_SIZE_RESPONSE                         = 0x060Fu,
    EVT_GATT_STOP_NOTIFICATION                                  = 0x0610u,
    EVT_GATT_TIMEOUT_NOTIFICATION                               = 0x0611u,

    /* GAP Events */
    EVT_GET_DEVICE_IO_CAPABILITIES_RESPONSE                     = 0x0680u,
    EVT_GET_BLUETOOTH_DEVICE_ADDRESS_RESPONSE                   = 0x0681u,
    EVT_GET_PEER_BLUETOOTH_DEVICE_ADDRESS_RESPONSE              = 0x0682u,
    EVT_GET_PEER_DEVICE_HANDLE_RESPONSE                         = 0x0683u,
    EVT_CURRENT_CONNECTION_PARAMETERS_NOTIFICATION              = 0x0684u,
    EVT_GET_CONNECTION_PARAMETERS_RESPONSE                      = 0x0685u,
    EVT_GET_SCAN_PARAMETERS_RESPONSE                            = 0x0686u,
    EVT_GET_LOCAL_DEVICE_SECURITY_RESPONSE                      = 0x0687u,
    EVT_GET_PEER_DEVICE_SECURITY_RESPONSE                       = 0x0688u,
    EVT_GET_WHITE_LIST_RESPONSE                                 = 0x0689u,
    EVT_SCAN_PROGRESS_RESULT                                    = 0x068Au,
    EVT_GENERATE_BD_ADDR_RESPONSE                               = 0x068Bu,
    EVT_CURRENT_LOCAL_KEYS_RESPONSE                             = 0x068Cu,
    EVT_PASSKEY_ENTRY_REQUEST                                   = 0x068Du,
    EVT_PASSKEY_DISPLAY_REQUEST                                 = 0x068Eu,
    EVT_ESTABLISH_CONNECTION_RESPONSE                           = 0x068Fu,
    EVT_CONNECTION_TERMINATED_NOTIFICATION                      = 0x0690u,
    EVT_SCAN_STOPPED_NOTIFICATION                               = 0x0691u,
    EVT_PAIRING_REQUEST_RECEIVED_NOTIFICATION                   = 0x0692u,
    EVT_AUTHENTICATION_ERROR_NOTIFICATION                       = 0x0693u,
    EVT_CONNECTION_CANCELLED_NOTIFICATION                       = 0x0694u,
    EVT_GET_BONDED_DEVICES_BY_RANK_RESPONSE                     = 0x0695u,
    EVT_UPDATE_CONNECTION_PARAMETERS_NOTIFICATION               = 0x0696u,
    EVT_GET_PEER_DEVICE_SECURITY_KEYS_RESPONSE                  = 0x0697u,
    EVT_RESOLVE_AND_SET_PEER_BD_ADDRESS_RESPONSE                = 0x0698u,
    EVT_GET_LOCAL_DEVICE_SECURITY_KEYS_RESPONSE                 = 0x0699u,
    EVT_GET_HOST_CHANNEL_MAP_RESPONSE                           = 0x069Au,
    EVT_GET_DATA_LENGTH_RESPONSE                                = 0x069Bu,
    EVT_CONVERT_OCTET_TO_TIME_RESPONSE                          = 0x069Cu,
    EVT_DATA_LENGTH_CHANGED_NOTIFICATION                        = 0x069Du,
    EVT_GET_RESOLVABLE_ADDRESS_RESPONSE                         = 0x069Eu,
    EVT_GET_RESOLVING_LIST_RESPONSE                             = 0x069Fu,
    EVT_ENHANCED_CONNECTION_COMPLETE                            = 0x06A0u,
    EVT_DIRECT_ADV_SCAN_PROGRESS_RESULT                         = 0x06A1u,
    EVT_GENERATE_SECURED_CONNECTION_OOB_DATA_RESPONSE           = 0x06A2u,
    EVT_NUMERIC_COMPARISON_REQUEST                              = 0x06A3u,
    EVT_NEGOTIATED_PAIRING_PARAMETERS                           = 0x06A4u,

    /* L2CAP Events */
    EVT_CBFC_CONNECTION_INDICATION                              = 0x0500u,
    EVT_CBFC_CONNECTION_CONFIRMATION                            = 0x0501u,
    EVT_CBFC_DISCONNECT_INDICATION                              = 0x0502u,
    EVT_CBFC_DISCONNECT_CONFIRMATION                            = 0x0503u,
    EVT_CBFC_DATA_RECEIVIED_NOTIFICATION                        = 0x0504u,
    EVT_CBFC_RX_CREDIT_INDICATION                               = 0x0505u,
    EVT_CBFC_TX_CREDIT_INDICATION                               = 0x0506u,
    EVT_CBFC_DATA_WRITE_INDICATION                              = 0x0507u

}Event;

/* CySmart Command opcodes as per protocol */
typedef enum
{
    /* General Commands */
    CMD_INIT_BLE_STACK                                          = 0xFC07u,

    /* Application specific Commands - Not to be used by CySmart protocol */
    CMD_PEER_ADDR_FROM_UART                                     = 0xFC61u,

    /* GAP Commands */
    CMD_START_SCAN                                              = 0xFE93u,
    CMD_STOP_SCAN                                               = 0xFE94u,
    CMD_ESTABLISH_CONNECTION                                    = 0xFE97u,
    CMD_TERMINATE_CONNECTION                                    = 0xFE98u,
    CMD_INITIATE_PAIRING_REQUEST                                = 0xFE99u,

    /* GATT Commands */
    CMD_FIND_INCLUDED_SERVICES                                  = 0xFE02u,
    CMD_DISCOVER_ALL_CHARACTERISTICS                            = 0xFE03u,
    CMD_DISCOVER_CHARACTERISTICS_BY_UUID                        = 0xFE04u,

    CMD_READ_CHARACTERISTIC_VALUE                               = 0xFE06u,
    CMD_READ_USING_CHARACTERISTIC_UUID                          = 0xFE07u,
    CMD_READ_LONG_CHARACTERISTIC_VALUES                         = 0xFE08u,
    CMD_READ_MULTIPLE_CHARACTERISTIC_VALUES                     = 0xFE09u,
    
    CMD_READ_CHARACTERISTIC_DESCRIPTOR                          = 0xFE0Eu,
    CMD_READ_LONG_CHARACTERISTIC_DESCRIPTOR                     = 0xFE0Fu,
    CMD_EXCHANGE_GATT_MTU_SIZE                                  = 0xFE12u,
    CMD_GATT_STOP                                               = 0xFE13u
}Command;
    
/* Only Primary and corresponding secondary commands will be allowed at a time */
extern Command_Format primaryCmd, secondaryCmd;

/* ON/OFF Status of BLE component */
extern bool isCySBleStackOn;

/* Status of last primary command */
extern bool primaryCmdInProgress; 

/* Global flag for CySmart tool connection with Dongle */
extern bool isCySmtConnected;

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
extern void CyS_SendEvent(Event EventOpCode, uint16 CommandOpCode, uint16 addlDataLen,
                                    uint16 ParamSize, const uint8 *parameters);

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
extern void CySmt_SendCommandStatus(uint16 CommandOpCode, uint16 status);

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
*  status:  Return status from stack, will be transmitted as 2-byte value 
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
extern void CySmt_SendCommandComplete(uint16 CommandOpCode, uint16 status);

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
extern void CySmt_ProcessCommands(void);

#endif /* _CYSMT_PROTOCOL_H_ */
/* [] END OF FILE */
