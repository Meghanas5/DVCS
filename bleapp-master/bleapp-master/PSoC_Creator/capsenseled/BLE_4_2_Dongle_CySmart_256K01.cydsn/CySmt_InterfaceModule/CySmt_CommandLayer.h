/******************************************************************************
* File Name         : CySmt_CommandLayer.h
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

#ifndef _CYSMT_COMMANDLAYER_H_
#define _CYSMT_COMMANDLAYER_H_

#include "CySmt_protocol.h"
 

/* Total size of host channel classification command parameters - Length + channel map */
#define HOST_CHANNEL_CLASSIFICATION_MAP_SIZE  (5u)

/* Below macro reflects the number of elements present in CYBLE_GAP_SMP_KEY_DIST_T */
#define NUM_KEYS_GENERATED      0x05

/* Command interpretor related API declaration */

/*******************************************************************************
* Function Name: Cmd_xxx_Api
********************************************************************************
*
* Summary:
* Each of the command mapped to one unique function and they follow unique definition
*
* Parameters:
* Command_Format *currentCmd
*
* Return:
*  CYBLE_API_RESULT_T - This will be used in status and complete responses
*
* Theory:
*  The command code will be processed by protocol Layer and corresponding function will be triggered, 
*  The command API functions will parse the data and trigger BLE actions and send the Status/Complete responses.
*
* Side Effects:
*
* Note:
*
*******************************************************************************/

/* General Commands API */
CYBLE_API_RESULT_T Cmd_Get_Device_Id_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Get_Supported_Tool_Ver_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Get_Firmware_Version_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Get_Supported_Gap_Roles_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T  Cmd_Get_Current_Gap_Role_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Get_Supported_Gatt_Roles_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Get_Current_Gatt_Role_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Init_Ble_Stack_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Tool_Disconnected_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Host_Timed_Out_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Get_Device_Descriptor_Info(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Get_Hardware_Version_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Get_Ble_Stack_Version_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Get_Rssi_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Get_TxPowerLevel_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Set_TxPowerLevel_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Set_HostChannelClassification_Api(Command_Format *currentCmd);

/* Gap Commands API */
CYBLE_API_RESULT_T Cmd_Set_Device_Io_Capabilities_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Get_Device_Io_Capabilities_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Get_Bluetooth_Device_Address_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Set_Bluetooth_Device_Address_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Get_Peer_Bluetooth_Device_Address_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Get_Peer_Device_Handle_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_GenerateBd_Addr_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Set_Oob_Data_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Get_Connection_Parameters_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Set_Connection_Parameters_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Get_Scan_Parameters_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Set_Scan_Parameters_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Get_Local_Device_Security_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Set_Local_Device_Security_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Get_Peer_Device_Security_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Get_White_List_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Add_Device_To_White_List_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Remove_Device_From_White_List_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Clear_White_List_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Start_Scan_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Stop_Scan_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Generate_Set_Keys_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Set_Authentication_Keys_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Establish_Connection_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Terminate_Connection_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Initiate_Pairing_Request_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Set_Identiry_Addr_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Pairing_PassKey_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Update_Connection_Params_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Cancle_Connection_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Get_Bonded_Devices_By_Rank_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_UpdateConnectionParam_Resp_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Get_PeerDevice_SecurityKeys_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Resolve_Set_Peer_Addr_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Get_LocalDevSecurityKeys_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Get_HostChannelMap_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Remove_Device_From_Bond_List_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Clear_Bond_List_Api(Command_Format *currentCmd);

#if(CYBLE_M0S8BLESS_VERSION > 1)    /* BLE256DMA and later */
CYBLE_API_RESULT_T Cmd_Set_Conn_Data_Len_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Get_Default_Data_Len_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Set_Default_Data_Len_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Convert_OctetToTime_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Get_Resolving_List_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Add_Device_To_Resolving_List_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Remove_Device_From_Resolving_List_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Clear_Resolving_List_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Get_Peer_Resolvable_Addr_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Get_Local_Resolvable_Addr_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Set_Resolvable_Addr_Timeout_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Addr_Resolution_Control_Api(Command_Format *currentCmd);
#endif /* CYBLE_M0S8BLESS_VERSION > 1 */
CYBLE_API_RESULT_T Cmd_GenerateLocalP256PublicKey_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_SendSecuredConnectionKeyPress_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_GenerateSecuredConnectionOobData_Api(Command_Format *currentCmd);

/* Gatt Commands API */
CYBLE_API_RESULT_T Cmd_Discover_All_Primary_Services_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Discover_Primary_Services_By_Uuid_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Find_Included_Services_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Discover_All_Characteristics_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Discover_Characteristics_By_Uuid_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Discover_All_Characteristic_Descriptors_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Read_Characteristic_Value_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Read_Using_Characteristic_Uuid_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Read_Long_Characteristic_Values_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Read_Multiple_Characteristic_Values_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Characteristic_Value_Write_Without_Response_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Write_Characteristic_Value_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Write_Long_Characteristic_Value_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Reliable_Characteristic_Value_Writes_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Read_Characteristic_Descriptor_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Read_Long_Characteristic_Descriptor_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Write_Characteristic_Descriptor_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Write_Long_Characteristic_Descriptor_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Exchange_GATT_MTU_Size_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_GATT_Stop_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Signed_Write_Without_Response_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Execute_Write_Request_Api(Command_Format *currentCmd);

/* L2CAP Commands API */
CYBLE_API_RESULT_T Cmd_Register_PSM_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_Unregister_PSM_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_CBFC_SendConnectionReq_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_CBFC_SendConnectionResp_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_CBFC_SendFlowControlCredit_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_CBFC_SendData_Api(Command_Format *currentCmd);
CYBLE_API_RESULT_T Cmd_CBFC_SendDisconnectReq_Api(Command_Format *currentCmd);

#endif    /* _CYSMT_COMMANDLAYER_H_ */
/* [] END OF FILE */
