/*******************************************************************************
* File name : drv_rf4ce_ti.h
* Author : D.K Lee
* description :
*
* Copyright (c)  by Humax Co., Ltd. Since 2007.
* All right reserved
*******************************************************************************/
 
#ifndef _DRV_RF4CE_TI_H_
#define _DRV_RF4CE_TI_H_

/*******************************************************************************
* Headers
*******************************************************************************/

#include "htype.h"
#include "drv_err.h"
#include "di_key.h"
#include "drv_rf4ce.h"

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/

///////////////////////////////////////////////////////////////////////////////////////////////////
// Common
///////////////////////////////////////////////////////////////////////////////////////////////////

// RPC Command Field Type
#define RPC_CMD_POLL        0x00
#define RPC_CMD_SREQ        0x20
#define RPC_CMD_AREQ        0x40
#define RPC_CMD_SRSP        0x60
#define RPC_CMD_RES4        0x80
#define RPC_CMD_RES5        0xA0
#define RPC_CMD_RES6        0xC0
#define RPC_CMD_RES7        0xE0

// RPC Command Field Subsystem
#define RPC_SYS_RES0        0
#define RPC_SYS_SYS         1
#define RPC_SYS_MAC         2
#define RPC_SYS_NWK         3
#define RPC_SYS_AF          4
#define RPC_SYS_ZDO         5
#define RPC_SYS_SAPI        6    // Simple API
#define RPC_SYS_UTIL        7
#define RPC_SYS_DBG         8
#define RPC_SYS_APP         9
#define RPC_SYS_RCAF        10   // Remote Control Application Framework
#define RPC_SYS_RCN         11   // Remote Control Network Layer
#define RPC_SYS_RCN_CLIENT  12   // Remote Control Network Layer Client
#define RPC_SYS_BOOT        13   // Serial Bootloader
#define RPC_SYS_MAX         14   // Maximum value, must be last

/* 1st byte is the length of the data field, 2nd/3rd bytes are command field. */
#define RPC_FRAME_HDR_SZ    3

/* The 3 MSB's of the 1st command field byte are for command type. */
#define RPC_CMD_TYPE_MASK   0xE0

/* The 5 LSB's of the 1st command field byte are for the subsystem. */
#define RPC_SUBSYSTEM_MASK  0x1F

/* position of fields in the general format frame */
#define RPC_POS_LEN         0
#define RPC_POS_CMD0        1
#define RPC_POS_CMD1        2
#define RPC_POS_DAT0        3

/* Error codes */
#define RPC_SUCCESS         0     /* success */
#define RPC_ERR_SUBSYSTEM   1     /* invalid subsystem */
#define RPC_ERR_COMMAND_ID  2     /* invalid command ID */
#define RPC_ERR_PARAMETER   3     /* invalid parameter */
#define RPC_ERR_LENGTH      4     /* invalid length */

///////////////////////////////////////////////////////////////////////////////////////////////////
// RTIS
///////////////////////////////////////////////////////////////////////////////////////////////////

// RTIS Command Ids for NPI Callback
// Add Command Here
#define RTIS_CMD_ID_RTI_READ_ITEM				0x01  // Deprecated for 0x21
#define RTIS_CMD_ID_RTI_WRITE_ITEM				0x02  // Deprecated for 0x22
#define RTIS_CMD_ID_RTI_INIT_REQ				0x03
#define RTIS_CMD_ID_RTI_PAIR_REQ				0x04
#define RTIS_CMD_ID_RTI_SEND_DATA_REQ			0x05
#define RTIS_CMD_ID_RTI_ALLOW_PAIR_REQ			0x06
#define RTIS_CMD_ID_RTI_STANDBY_REQ 			0x07
#define RTIS_CMD_ID_RTI_RX_ENABLE_REQ			0x08
#define RTIS_CMD_ID_RTI_ENABLE_SLEEP_REQ		0x09
#define RTIS_CMD_ID_RTI_DISABLE_SLEEP_REQ		0x0A
#define RTIS_CMD_ID_RTI_UNPAIR_REQ				0x0B
#define RTIS_CMD_ID_RTI_PAIR_ABORT_REQ			0x0C
#define RTIS_CMD_ID_RTI_ALLOW_PAIR_ABORT_REQ	0x0D

//
#define RTIS_CMD_ID_TEST_PING_REQ			   0x10
#define RTIS_CMD_ID_RTI_TEST_MODE_REQ		   0x11
#define RTIS_CMD_ID_RTI_RX_COUNTER_GET_REQ	   0x12
#define RTIS_CMD_ID_RTI_SW_RESET_REQ		   0x13
//
#define RTIS_CMD_ID_RTI_READ_ITEM_EX		   0x21
#define RTIS_CMD_ID_RTI_WRITE_ITEM_EX		   0x22

// HUMAX
#define RTIS_CMD_ID_RTI_GET_PAIRED_DEVICE_REQ	0x30
#define RTIS_CMD_ID_RTI_GET_PAIRING_TABLE_REQ	0x31
#define RTIS_CMD_ID_RTI_GET_NWK_INFO_REQ		0x32
#define RTIS_CMD_ID_RTI_SET_IEEE_ADDR_REQ		0x33
#define RTIS_CMD_ID_RTI_SET_FACTORY_MODE_REQ	0x34
#define RTIS_CMD_ID_RTI_SEND_TXRX_RSSI_REQ		0x35

#define RTIS_HUMAX_CMD_ID_RTI_SET_CHANNEL_REQ			0x40
#define RTIS_HUMAX_CMD_ID_RTI_SET_TX_POWER_REQ			0x41
#define RTIS_HUMAX_CMD_ID_RTI_SET_TX_MODE_REQ			0x42
#define RTIS_HUMAX_CMD_ID_RTI_SET_RX_MODE_REQ			0x43
#define RTIS_HUMAX_CMD_ID_RTI_GET_RSSI_REQ				0x44
#define RTIS_HUMAX_CMD_ID_RTI_SET_RX_ENABLE_REQ			0x45
#define RTIS_HUMAX_CMD_ID_RTI_GET_CONTEXT_REQ			0x46
#define RTIS_HUMAX_CMD_ID_RTI_SET_TEST_MODE_REQ						0x47

// RTIS Confirm Ids
#define RTIS_CMD_ID_RTI_INIT_CNF			   0x01
#define RTIS_CMD_ID_RTI_PAIR_CNF			   0x02
#define RTIS_CMD_ID_RTI_SEND_DATA_CNF		   0x03
#define RTIS_CMD_ID_RTI_ALLOW_PAIR_CNF		   0x04
#define RTIS_CMD_ID_RTI_REC_DATA_IND		   0x05
#define RTIS_CMD_ID_RTI_STANDBY_CNF 		   0x06
#define RTIS_CMD_ID_RTI_RX_ENABLE_CNF		   0x07
#define RTIS_CMD_ID_RTI_ENABLE_SLEEP_CNF	   0x08
#define RTIS_CMD_ID_RTI_DISABLE_SLEEP_CNF	   0x09
#define RTIS_CMD_ID_RTI_UNPAIR_CNF			   0x0A
#define RTIS_CMD_ID_RTI_UNPAIR_IND			   0x0B
#define RTIS_CMD_ID_RTI_PAIR_ABORT_CNF			0x0C
#define RTIS_CMD_ID_RTI_RESET_IND				0x0D

// HUMAX
#define RTIS_CMD_ID_RTI_GET_PAIRED_DEVICE_CNF	0x30
#define RTIS_CMD_ID_RTI_GET_PAIRING_TABLE_CNF	0x31
#define RTIS_CMD_ID_RTI_GET_NWK_INFO_CNF		0x32
#define RTIS_CMD_ID_RTI_SET_IEEE_ADDR_CNF		0x33
#define RTIS_CMD_ID_RTI_SET_FACTORY_MODE_CNF	0x34

#define RTIS_HUMAX_CMD_ID_RTI_SET_CHANNEL_CNF			0x40
#define RTIS_HUMAX_CMD_ID_RTI_SET_TX_POWER_CNF			0x41
#define RTIS_HUMAX_CMD_ID_RTI_SET_TX_MODE_CNF			0x42
#define RTIS_HUMAX_CMD_ID_RTI_SET_RX_MODE_CNF			0x43
#define RTIS_HUMAX_CMD_ID_RTI_GET_RSSI_CNF				0x44
#define RTIS_HUMAX_CMD_ID_RTI_SET_RX_ENABLE_CNF			0x45
#define RTIS_HUMAX_CMD_ID_RTI_GET_CONTEXT_CNF			0x46
#define RTIS_HUMAX_CMD_ID_RTI_SET_TEST_MODE_CNF						0x47


// RTI States
enum
{
  RTIS_STATE_INIT,
  RTIS_STATE_READY,
  RTIS_STATE_NETWORK_LAYER_BRIDGE
};

// RTI API Status Values

// Application framework layer primitive status field values
// Error Table
#define RF4CE_SUCCESS                                      0
#define RF4CE_ERROR_INVALID_INDEX                          0xF9
#define RF4CE_ERROR_INVALID_PARAMETER                      0xE8
#define RF4CE_ERROR_UNSUPPORTED_ATTRIBUTE                  0xF4
#define RF4CE_ERROR_NO_ORG_CAPACITY                        0xB0
#define RF4CE_ERROR_NO_REC_CAPACITY                        0xB1
#define RF4CE_ERROR_NO_PAIRING_INDEX                       0xB2
#define RTI_ERROR_NO_RESPONSE                            0xB3
#define RTI_ERROR_NOT_PERMITTED                          0xB4
#define RTI_ERROR_FRAME_COUNTER_EXPIRED                  0xB6
#define RTI_ERROR_DISCOVERY_ERROR                        0xB7
#define RF4CE_ERROR_DISCOVERY_TIMEOUT                      0xB8
#define RTI_ERROR_SECURITY_TIMEOUT                       0xB9
#define RTI_ERROR_SECURITY_FAILURE                       0xBA
#define RTI_ERROR_NO_SECURITY_KEY                        0xBD
#define RTI_ERROR_OUT_OF_MEMORY                          0xBE
#define RTI_ERROR_OSAL_NO_TIMER_AVAIL                    0x08
#define RTI_ERROR_OSAL_NV_OPER_FAILED                    0x0A
#define RTI_ERROR_OSAL_NV_ITEM_UNINIT                    0x09
#define RTI_ERROR_OSAL_NV_BAD_ITEM_LEN                   0x0C
#define RTI_ERROR_MAC_TRANSACTION_EXPIRED                0xF0
#define RTI_ERROR_MAC_TRANSACTION_OVERFLOW               0xF1
#define RTI_ERROR_MAC_NO_RESOURCES                       0x1A
#define RTI_ERROR_MAC_UNSUPPORTED                        0x18
#define RTI_ERROR_MAC_BAD_STATE                          0x19
#define RTI_ERROR_MAC_CHANNEL_ACCESS_FAILURE             0xE1
#define RTI_ERROR_MAC_NO_ACK                             0xE9
#define RTI_ERROR_MAC_BEACON_LOST                        0xE0
#define RTI_ERROR_MAC_PAN_ID_CONFLICT                    0xEE
#define RTI_ERROR_MAC_SCAN_IN_PROGRESS                   0xFC
#define RTI_ERROR_UNKNOWN_STATUS_RETURNED                0x20
#define RF4CE_ERROR_FAILED_TO_DISCOVER                     0x21
#define RF4CE_ERROR_FAILED_TO_PAIR                         0x22
#define RF4CE_ERROR_ALLOW_PAIRING_TIMEOUT                  0x23
#define RTI_ERROR_FAILED_TO_CONFIGURE_ZRC                0x41
#define RTI_ERROR_FAILED_TO_CONFIGURE_ZID                0x42
#define RTI_ERROR_FAILED_TO_CONFIGURE_Z3D                0x43
#define RTI_ERROR_FAILED_TO_CONFIGURE_INV_MASK           (0x40)
  // reserve failure IDs 0x44-0x4A for future profiles
#define RTI_ERROR_SYNCHRONOUS_NPI_TIMEOUT                0xFF



// RTI API Parameter Constants

// RTI_SendDataReq TX Options Bit Mask
#define RTI_TX_OPTION_BROADCAST                          0x01
#define RTI_TX_OPTION_IEEE_ADDRESS                       0x02
#define RTI_TX_OPTION_ACKNOWLEDGED                       0x04
#define RTI_TX_OPTION_SECURITY                           0x08
#define RTI_TX_OPTION_SINGLE_CHANNEL                     0x10
#define RTI_TX_OPTION_CHANNEL_DESIGNATOR                 0x20
#define RTI_TX_OPTION_VENDOR_SPECIFIC                    0x40

// RTI_ReceiveDataInd RX Flags Bit Mask
#define RTI_RX_FLAGS_BROADCAST                           0x01
#define RTI_RX_FLAGS_SECURITY                            0x02
#define RTI_RX_FLAGS_VENDOR_SPECIFIC                     0x04

// RTI_RxEnableReq
#define RTI_RX_ENABLE_OFF                                0
#define RTI_RX_ENABLE_ON                                 0xFFFF

// RTI_StandbyReq
#define RTI_STANDBY_OFF                                  0
#define RTI_STANDBY_ON                                   1

// Network layer implicit constants as in standard used by attribute structure
#define RCN_MAX_NUM_DEV_TYPES                     3 // number of device types supported per device
#define RCN_SEC_KEY_LENGTH                        16 // length in bytes of network layer security key

#define RCN_MAX_NUM_PROFILE_IDS                   7 // number of profile IDs supported per device
#define RCN_VENDOR_STRING_LENGTH					5
#define RCN_USER_STRING_LENGTH                    15
#define RCN_SEC_KEY_SEED_LENGTH                   80

#ifndef RCN_FEATURE_SECURITY
# define RCN_FEATURE_SECURITY                     TRUE
#endif

#ifndef RCN_FEATURE_EXTRA_PAIR_INFO
# define RCN_FEATURE_EXTRA_PAIR_INFO              TRUE
#endif

// Constants Table Constants
#define RTI_CONST_SW_VERSION                             0x13   // V1.3
#define RTI_CONST_NWK_PROTOCOL_IDENTIFIER                0xCE   // Network layer protocol id
#define RTI_CONST_NWK_PROTOCOL_VERSION                   0x10   // V1.0

// Test Interface
#define RTI_TEST_MODE_TX_RAW_CARRIER                     0
#define RTI_TEST_MODE_TX_RANDOM_DATA                     1
#define RTI_TEST_MODE_RX_AT_FREQ                         2

// Configuration Capacity
#define RF4CE_MAX_NUM_SUPPORTED_TGT_TYPES                  6

// Bit masks for the Application Capabilities field (3.3.1.5, Figure 18).
#define RTI_APP_CAP_USER_STR_BM                          0x01
#define RTI_APP_CAP_NUM_DEVS_BM                          0x06
#define RTI_APP_CAP_RES_BIT3_BM                          0x08
#define RTI_APP_CAP_NUM_PROF_BM                          0x70
#define RTI_APP_CAP_RES_BIT7_BM                          0x80

// Bit masks for the Node Capabilities field (3.4.2.4, Figure 26).
#define RTI_NODE_CAP_NODE_TYPE_BM                        0x01
#define RTI_NODE_CAP_MAINS_PWR_BM                        0x02
#define RTI_NODE_CAP_SEC_CAP_BM                          0x04
#define RTI_NODE_CAP_CHAN_NORM_BM                        0x08
#define RTI_NODE_CAP_RESERVED_BM                         0xF0

// The follwoing constant values are dictated by
// RF4CE standard and hence cannot be modified at all.
#define RTI_MAX_NUM_DEV_TYPES                            RCN_MAX_NUM_DEV_TYPES
#define RTI_MAX_NUM_PROFILE_IDS                          RCN_MAX_NUM_PROFILE_IDS
#define RTI_VENDOR_STRING_LENGTH                         RCN_VENDOR_STRING_LENGTH
#define RTI_USER_STRING_LENGTH                           RCN_USER_STRING_LENGTH
#define RTI_DEST_PAN_ID_WILDCARD                         0xFFFF
#define RTI_DEST_NWK_ADDR_WILDCARD                       0xFFFF
#define RTI_REC_DEV_TYPE_WILDCARD                        0xFF
#define RTI_INVALID_PAIRING_REF                          0xFF
#define RTI_INVALID_DEVICE_TYPE                          0xFF

// Configuration Parameters (CP) Table Item Identifiers
#define RF4CE_CP_ITEM_START                                0xA0
#define RF4CE_CP_ITEM_STARTUP_CTRL                         0xA0   // Startup Control
#define RF4CE_CP_ITEM_NODE_CAPABILITIES                    0xA1   // Node Capabilities
#define RF4CE_CP_ITEM_NODE_SUPPORTED_TGT_TYPES             0xA2   // Node Capability::Supported Target Types
#define RF4CE_CP_ITEM_APPL_CAPABILITIES                    0xA3   // Application Capability::User String Specified
#define RF4CE_CP_ITEM_APPL_DEV_TYPE_LIST                   0xA4   // Application Capability::Device Type List
#define RF4CE_CP_ITEM_APPL_PROFILE_ID_LIST                 0xA5   // Application Capability::Profile Id List
#define RF4CE_CP_ITEM_STDBY_DEFAULT_ACTIVE_PERIOD          0xA6   // Standby Information::Default Standby Active Period
#define RF4CE_CP_ITEM_STDBY_DEFAULT_DUTY_CYCLE             0xA7   // Standby Information::Default Standby Duty Cycle
#define RF4CE_CP_ITEM_VENDOR_ID                            0xA8   // Vendor Information::Vendor Id
#define RF4CE_CP_ITEM_VENDOR_NAME                          0xA9   // Vendor Information::Vendor Name
#define RF4CE_CP_ITEM_DISCOVERY_DURATION                   0xAA   // Discovery Information::Discovery Duration
#define RF4CE_CP_ITEM_DEFAULT_DISCOVERY_LQI_THRESHOLD      0xAB   // Discovery Information::Default Discovery LQI Threshold
#define RF4CE_CP_ITEM_END                                  0xAF

// Device Type List
#define RF4CE_DEVICE_RESERVED_INVALID                   0x00
#define RF4CE_DEVICE_REMOTE_CONTROL                     0x01
#define RF4CE_DEVICE_TARGET_TYPE_START                  0x02
#define RF4CE_DEVICE_TELEVISION                         0x02
#define RF4CE_DEVICE_PROJECTOR                          0x03
#define RF4CE_DEVICE_PLAYER                             0x04
#define RF4CE_DEVICE_RECORDER                           0x05
#define RF4CE_DEVICE_VIDEO_PLAYER_RECORDER              0x06
#define RF4CE_DEVICE_AUDIO_PLAYER_RECORDER              0x07
#define RF4CE_DEVICE_AUDIO_VIDEO_RECORDER               0x08
#define RF4CE_DEVICE_SET_TOP_BOX                        0x09
#define RF4CE_DEVICE_HOME_THEATER_SYSTEM                0x0A
#define RF4CE_DEVICE_MEDIA_CENTER_PC                    0x0B
#define RF4CE_DEVICE_GAME_CONSOLE                       0x0C
#define RF4CE_DEVICE_SATELLITE_RADIO_RECEIVER           0x0D
#define RF4CE_DEVICE_IR_EXTENDER                        0x0E
#define RF4CE_DEVICE_MONITOR                            0x0F
#define RF4CE_DEVICE_TARGET_TYPE_END                    0x10

// Network Information Base (NIB) attribute identifier enumeration
#define RCN_NIB_NWK_START                         0x60 // enumeration starts
  // NOTE: active period NIB attribute is in miliseconds and it support 16 bit value
#define RCN_NIB_NWK_ACTIVE_PERIOD                 0x60 // nwkActivePeriod
#define RCN_NIB_NWK_BASE_CHANNEL                  0x61 // nwkBaseChannel
#define RCN_NIB_NWK_DISCOVERY_LQI_THRESHOLD       0x62 // nwkDiscoveryLQIThreshold
  // NOTE: discovery repetition internal NIB attribute is in miliseconds and supports 16 bit value
#define RCN_NIB_NWK_DISCOVERY_REPETITION_INTERVAL 0x63 // nwkDiscoveryRepetitionInterval
  // NOTE: duty cycle NIB attribute is in miliseconds and it supports 16 bit value
#define RCN_NIB_NWK_DUTY_CYCLE                    0x64 // nwkDutyCycle
#define RCN_NIB_NWK_FRAME_COUNTER                 0x65 // nwkFrameCounter
#define RCN_NIB_NWK_INDICATE_DISCOVERY_REQUESTS   0x66 // nwkIndicateDiscoveryRequests
#define RCN_NIB_NWK_IN_POWER_SAVE                 0x67 // nwkInPowerSave
#define RCN_NIB_NWK_PAIRING_TABLE                 0x68 // nwkPairingTable
#define RCN_NIB_NWK_MAX_DISCOVERY_REPETITIONS     0x69 // nwkMaxDiscoveryRepetitions
#define RCN_NIB_NWK_MAX_FIRST_ATTEMPT_CSMA_BACKOFFS 0x6a // nwkMaxFirstAttemptCSMABackoffs
#define RCN_NIB_NWK_MAX_FIRST_ATTEMPT_FRAME_RETRIES 0x6b // nwkMaxFirstAttemptFrameRetries
#define RCN_NIB_NWK_MAX_REPORTED_NODE_DESCRIPTORS 0x6c // nwkMaxReportedNodeDescriptors
  // NOTE: response wait time attribute is in miliseconds and it supports 16 bit value
#define RCN_NIB_NWK_RESPONSE_WAIT_TIME            0x6d // nwkResponseWaitTime
#define RCN_NIB_NWK_SCAN_DURATION                 0x6e // nwkScanDuration
#define RCN_NIB_NWK_USER_STRING                   0x6f // nwkUserString
#define RCN_NIB_NWK_RESERVED_0                    0x70 // end of standard attribute enumeration

// Proprietary attributes (these are not RF4CE NIB attribute but are used anyways)
#define RCN_NIB_CUSTOM_START                      0x80 // start of custom attribute enumeration
#define RCN_NIB_NWK_NODE_CAPABILITIES             0x80 // nwkcNodeCapabilities - constant in standard
#define RCN_NIB_NWK_VENDOR_IDENTIFIER             0x81 // nwkcVendorIdentifier - constant in standard
#define RCN_NIB_NWK_VENDOR_STRING                 0x82 // nwkcVendorString - constant in standard
#define RCN_NIB_STARTED                           0x83 // an indicator whether the node has
                                                       // started or not
#define RCN_NIB_IEEE_ADDRESS                      0x84 // IEEE address of the device
#define RCN_NIB_PAN_ID                            0x85 // target node PAN ID
#define RCN_NIB_SHORT_ADDRESS                     0x86 // target node short address
#define RCN_NIB_AGILITY_ENABLE                    0x87 // flag set to TRUE/FALSE for frequency agility enable
#define RCN_NIB_TRANSMIT_POWER                    0x88 // transmit power, uint8 value * -1dBm

// proprietary debug attributes
#define RCN_NIB_TRANSMIT_LATENCY                  0x89 // latency of last transmission
#define RCN_NIB_ENERGY_SAMPLE                     0x8a // energy scan samples
#define RCN_NIB_CUSTOM_RESERVED_0                 0x8b // end of custom attribute enumeration

#define RCN_NIB_NWK_RANGE_END                     0x8f // end of NIB attribute range

// Network layer implicit constants as in standard used by attribute structure
#define RCN_MAX_NUM_DEV_TYPES                     3 // number of device types supported per device
#define RCN_SEC_KEY_LENGTH                        16 // length in bytes of network layer security key
#define RCN_MAX_ATTRIB_SIZE_EXCEPT_PAIR_TBL       8 // maximum attribute value size in bytes except
                                                    // pairing table entry

// State Attributes (SA) Table Item Identifiers
#define RTI_SA_ITEM_START                                RCN_NIB_NWK_START
#define RTI_SA_ITEM_STANDBY_ACTIVE_PERIOD                RCN_NIB_NWK_ACTIVE_PERIOD  // RW
#define RTI_SA_ITEM_CURRENT_CHANNEL                      RCN_NIB_NWK_BASE_CHANNEL   // RO
#define RTI_SA_ITEM_DISCOVERY_LQI_THRESHOLD              RCN_NIB_NWK_DISCOVERY_LQI_THRESHOLD // RW
#define RTI_SA_ITEM_DUTY_CYCLE                           RCN_NIB_NWK_DUTY_CYCLE     // RW
#define RTI_SA_ITEM_FRAME_COUNTER                        RCN_NIB_NWK_FRAME_COUNTER  // RO (RW in test mode)
#define RTI_SA_ITEM_IN_POWER_SAVE                        RCN_NIB_NWK_IN_POWER_SAVE  // RO
#define RTI_SA_ITEM_MAX_FIRST_ATTEMPT_CSMA_BACKOFFS      RCN_NIB_NWK_MAX_FIRST_ATTEMPT_CSMA_BACKOFFS // RW
#define RTI_SA_ITEM_MAX_FIRST_ATTEMPT_FRAME_RETRIES      RCN_NIB_NWK_MAX_FIRST_ATTEMPT_FRAME_RETRIES // RW
#define RTI_SA_ITEM_RESPONSE_WAIT_TIME                   RCN_NIB_NWK_RESPONSE_WAIT_TIME  // RW
#define RTI_SA_ITEM_SCAN_DURATION                        RCN_NIB_NWK_SCAN_DURATION  // RW
#define RTI_SA_ITEM_USER_STRING                          RCN_NIB_NWK_USER_STRING    // RW
#define RTI_SA_ITEM_PAN_ID                               RCN_NIB_PAN_ID             // RO (RW in test mode)
#define RTI_SA_ITEM_SHORT_ADDRESS                        RCN_NIB_SHORT_ADDRESS      // RO (RW in test mode)

#define RTI_DEVICE_GENERIC                            0xFE
#define RTI_DEVICE_RESERVED_FOR_WILDCARDS             0xFF

#define MAX_AVAIL_DEVICE_TYPES (RF4CE_DEVICE_TARGET_TYPE_END - RF4CE_DEVICE_TARGET_TYPE_START)

// Pairing Table
#define RF4CE_SA_ITEM_PT_NUMBER_OF_ACTIVE_ENTRIES          0xB0	// RO
#define RF4CE_SA_ITEM_PT_CURRENT_ENTRY_INDEX               0xB1	// RW
#define RF4CE_SA_ITEM_PT_CURRENT_ENTRY                     0xB2	// RW
#define RF4CE_SA_ITEM_PT_END                               0xB3

// Constants (CONST) Table Item Idenifiers
#define RF4CE_CONST_ITEM_START                             0xC0
#define RF4CE_CONST_ITEM_SW_VERSION                        0xC0
#define RF4CE_CONST_ITEM_MAX_PAIRING_TABLE_ENTRIES         0xC1
#define RF4CE_CONST_ITEM_NWK_PROTOCOL_IDENTIFIER           0xC2
#define RF4CE_CONST_ITEM_NWK_PROTOCOL_VERSION              0xC3
#define RF4CE_CONST_ITEM_END                               0xC4

// profile identifiers
#define RF4CE_PROFILE_RTI                               0xFF
#define RF4CE_PROFILE_ID_START                          0x01
#define RF4CE_PROFILE_GDP                               0x00
#define RF4CE_PROFILE_ZRC                               0x01
#define RF4CE_PROFILE_ZID                               0x02
#define RF4CE_PROFILE_Z3S                               0x03
#define RF4CE_PROFILE_ID_END                            0x04

// CERC command codes
#define RTI_CERC_USER_CONTROL_PRESSED                 0x01
#define RTI_CERC_USER_CONTROL_REPEATED                0x02
#define RTI_CERC_USER_CONTROL_RELEASED                0x03
#define RTI_CERC_COMMAND_DISCOVERY_REQUEST            0x04
#define RTI_CERC_COMMAND_DISCOVERY_RESPONSE           0x05

// RTI_SendDataReq TX Options Bit Mask
#define RTI_TX_OPTION_BROADCAST                          0x01
#define RTI_TX_OPTION_IEEE_ADDRESS                       0x02
#define RTI_TX_OPTION_ACKNOWLEDGED                       0x04
#define RTI_TX_OPTION_SECURITY                           0x08
#define RTI_TX_OPTION_SINGLE_CHANNEL                     0x10
#define RTI_TX_OPTION_CHANNEL_DESIGNATOR                 0x20
#define RTI_TX_OPTION_VENDOR_SPECIFIC                    0x40

// vendor identifiers
#define RF4CE_VENDOR_PANASONIC                          0x0001
#define RF4CE_VENDOR_SONY                               0x0002
#define RF4CE_VENDOR_SAMSUNG                            0x0003
#define RF4CE_VENDOR_PHILIPS                            0x0004
#define RF4CE_VENDOR_FREESCALE                          0x0005
#define RF4CE_VENDOR_OKI                                0x0006
#define RF4CE_VENDOR_TEXAS_INSTRUMENTS                  0x0007
#define RF4CE_VENDOR_HUMAX							  0x1102

#define SA_EVT_NONE                   0x0000 // No event
#define SA_EVT_INIT                   0x0001 // Initial Boot up Event



#define RF4CE_MAX_BUF_LEN	128

/* Extended address length */
#define RF4CE_SADDR_EXT_LEN   8

/* Address modes */
#define RF4CE_SADDR_MODE_NONE       0       /* Address not present */
#define RF4CE_SADDR_MODE_SHORT      2       /* Short address */
#define RF4CE_SADDR_MODE_EXT        3       /* Extended address */

#define BUS_ID			2
#define DEVICE_ID		1
 
#define SRDY_STATE_OK		0x00
#define SRDY_STATE_FAULT	0xff

#define SADDR_EXT_LEN   8

 // Endianness conversion macros
#define RF4CE_ENDIAN_CONV16(_value) ((((_value) & 0xff)<<8)|(((_value) & 0xff00)>>8))
#define RF4CE_ENDIAN_CONV32(_value) \
   ((((_value) & 0xff)<<24)|(((_value) & 0xff00)<<8)| \
	(((_value) & 0xff0000)>>8)|(((_value) & 0xff000000)>>24))
#define RF4CE_PAIRING_ENTRY_REQUIRED_LEN(_field) \
   ((uint16) (&((RF4CE_PairingEntry_t *) 0)->_field) + sizeof(((RF4CE_PairingEntry_t *) 0)->_field))
 
#define RTI_BUILD_NODE_CAPABILITIES(_target,_ac_powered,_security_capable,_ch_norm_capable ) \
   ((_target) | ((_ac_powered)<< 1) | ((_security_capable)<<2) | ((_ch_norm_capable)<<3))
 
 // Macro to build a byte for app capabilities - each parameter has to be set to either 0 or 1.
#define RTI_BUILD_APP_CAPABILITIES(_usr_str, _num_devs, _num_prof) \
   ((_usr_str) | ((_num_devs) << 1) | ((_num_prof) << 4))

 // Macro to generate little endian 32 bit word to be used as NPI interface
#define RF4CE_SET_ITEM_WORD( pVal, attrib )				   \
   (pVal)[0] = (HUINT8)( ((HUINT32)(attrib) >>	0) & 0xFF );  \
   (pVal)[1] = (HUINT8)( ((HUINT32)(attrib) >>	8) & 0xFF );  \
   (pVal)[2] = (HUINT8)( ((HUINT32)(attrib) >> 16) & 0xFF );  \
   (pVal)[3] = (HUINT8)( ((HUINT32)(attrib) >> 24) & 0xFF );
 
 // Macro to generate little endian 16 bit value to be used as NPI interface
#define RF4CE_SET_ITEM_HALFWORD( pVal, attrib )			   \
   (pVal)[0] = (HUINT8)( ((HUINT16)(attrib) >>	0) & 0xFF );  \
   (pVal)[1] = (HUINT8)( ((HUINT16)(attrib) >>	8) & 0xFF );

/* ------------------------------------------------------------------------------------------------
 *                                           SBL
 * ------------------------------------------------------------------------------------------------
 */

// Commands to Target Application
#define SB_TGT_BOOTLOAD					0x10

// Commands to Bootloader
#define SBL_WRITE_CMD                0x01
#define SBL_READ_CMD                 0x02
#define SBL_ENABLE_CMD               0x03
#define SBL_HANDSHAKE_CMD            0x04

// If an Application is to increase the number of NV pages used, the SBL AP can dynamically change
// the NV area "skipped" by the BootLoader. The payload is the Address to begin skipping / 4.
#define SBL_SKIP_ADDR_BEG_CMD        0x08
// If SBL_READ_CMD is disabled for security and the BootLoader must calculated the CRC, the code
// address end for the calculation with differ with the flash size. Instead of requiring a
// different BootLoader image for each flash size, and this flexibility to dynamically increase
// this code address end. This flexibility will also allow custom use of the end of the flash range
// for a 2nd, proprietary NV or other storage area.
#define SBL_CODE_ADDR_END_CMD        0x09

// Commands to Target Application
#define SBL_TGT_BOOTLOAD             0x10  // Erase the image valid signature & jump to bootloader.

// Responses from Bootloader - for historical consistency, SBL has OR'ed the MSBit of all commands
// when responding - this is probably not necessary for smooth functionality.
#define SBL_RSP_MASK                 0x80

// Status codes
#define SBL_SUCCESS                  0
#define SBL_FAILURE                  1
#define SBL_INVALID_FCS              2
#define SBL_INVALID_FILE             3
#define SBL_FILESYSTEM_ERROR         4
#define SBL_ALREADY_STARTED          5
#define SBL_NO_RESPOSNE              6

// Indices into the RPC data (RPC_POS_DAT0):
#define SBL_REQ_ADDR_LSB             RPC_POS_DAT0
#define SBL_REQ_ADDR_MSB            (SBL_REQ_ADDR_LSB+1)
#define SBL_REQ_DAT0                (SBL_REQ_ADDR_MSB+1)
#define SBL_RSP_STATUS               RPC_POS_DAT0
#define SBL_RSP_ADDR_LSB            (SBL_RSP_STATUS+1)
#define SBL_RSP_ADDR_MSB            (SBL_RSP_ADDR_LSB+1)
#define SBL_RSP_DAT0                (SBL_RSP_ADDR_MSB+1)
#define SBL_READ_HDR_LEN            (SBL_RSP_DAT0 - SBL_RSP_STATUS)


/* ------------------------------------------------------------------------------------------------
 *                                           Typedefs
 * ------------------------------------------------------------------------------------------------
 */

/* Extended address */
typedef HUINT8 sAddrExt_t[RF4CE_SADDR_EXT_LEN];

/* Combined short/extended device address */
typedef struct
{
  union
  {
    HUINT16      shortAddr;    /* Short address */
    sAddrExt_t  extAddr;      /* Extended address */
  } addr;
  HUINT8         addrMode;     /* Address mode */
} RF4CE_Addr_t;

// RF4CE API and RF4CE Callback Message structure
// NOTE: Fields are position dependent. Do not rearrange!
typedef struct RF4CE_MSG
{
	HUINT8 len;
	HUINT8 subSys;
	HUINT8 cmdId;
	HUINT8 pData[RF4CE_MAX_BUF_LEN];
} RF4CE_Msg_t;

// Startup Control
enum
{
  RESTORE_STATE,
  CLEAR_STATE,
  CLEAR_CONFIG_CLEAR_STATE
};

// Pairing table entry
typedef struct RF4CE_PairingEntry
{
	HUINT8	  pairingRef;	  // pairing reference
	HUINT16	  srcNwkAddress; // this device's own short address
	HUINT8	  logicalChannel; // expected channel of the peer
	sAddrExt_t ieeeAddress;	  // IEEE address of the peer
	HUINT16	  panId;		  // PAN identifier of the peer
	HUINT16	  nwkAddress;	// network address of the peer
	HUINT8	  recCapabilities; // capabilities bitmap of the target

#if RCN_FEATURE_SECURITY
	HUINT8	  securityKeyValid; // whether the key is valid or not
	HUINT8	  securityKey[RCN_SEC_KEY_LENGTH]; // security link key
#endif // RCN_FEATURE_SECURITY

#if RCN_FEATURE_EXTRA_PAIR_INFO
	// NOTE that RF4CE spec does not include the following as part of pairing entry
	HUINT16	  vendorIdentifier; // vendor identifier of the target
	HUINT8	  devTypeList[RCN_MAX_NUM_DEV_TYPES]; // list of device types supported by the peer
#endif // RCN_FEATURE_EXTRA_PAIR_INFO

	HUINT32	  recFrameCounter;	  // frame counter last received
}RF4CE_PairingEntry_t;

typedef struct RF4CE_TARGET_INFO
{
	HUINT8	dstIndex;
	HUINT8	devType;
	RF4CE_PairingEntry_t paringEntry;
} RF4CE_TargetInfo_t;

// Controller Node Information
typedef struct tgtTypeInfo
{
	HUINT8 supportedTgtTypes[ RF4CE_MAX_NUM_SUPPORTED_TGT_TYPES ];
} RF4CE_TargetTypeInfo_t;

// Node Information
typedef struct nodeCap
{
	RF4CE_TargetTypeInfo_t supportedTgtTypes;
} RF4CE_NodeCap_t;

// Application Information
typedef struct rtiAppCap
{
	HUINT8 appCapabilities;
	HUINT8 devTypeList[ RTI_MAX_NUM_DEV_TYPES ];
	HUINT8 profileIdList[ RTI_MAX_NUM_PROFILE_IDS ];
} rtiAppCap_t;

// Standby Information
typedef struct stdbyInfo
{
	HUINT16 standbyActivePeriod;
	HUINT16 standbyDutyCycle;
} stdByInfo_s;

// Vendor Id
// <please see rti_constants.h>

// Vendor Information
typedef struct vendorInfo
{
	HUINT16 vendorId;
	HUINT8  vendorName[ RTI_VENDOR_STRING_LENGTH ];
} RF4CE_VendorInfo_t;

typedef struct discInfo
{
	HUINT16 discDuration; // in miliseconds
	HUINT8  discLQIThres;
} discInfo_s;

// Configuration Parameters Table
typedef struct configParams
{
	HUINT8		startupCtrl;							  // RTI_NVID_STARTUP_CONTROL
	RF4CE_NodeCap_t	nodeCap;								  // RTI_NVID_NODE_CAP
	rtiAppCap_t	appCap; 								  // RTI_NVID_APP_CAP
	stdByInfo_s	stdByInfo;								  // RTI_NVID_STDBY_INFO
	discInfo_s	discoveryInfo;							  // RTI_NVID_DISCOV_INFO
	HUINT8		nodeCapabilities;
	RF4CE_VendorInfo_t vendorInfo;
} RF4CE_ConfigParams_t;

// State Attributes Table
typedef struct stateAttribs
{
	HUINT8  curPairTableIndex;
} RF4CE_StateAttrs_t;

typedef enum
{
	DRV_RF4CE_NOTIFY_EVT_INIT_SUCCESS,
	DRV_RF4CE_NOTIFY_EVT_INIT_FAILED,
	DRV_RF4CE_NOTIFY_EVT_PAIRING_SUCCESS,
	DRV_RF4CE_NOTIFY_EVT_PAIRING_FAILED,
	DRV_RF4CE_NOTIFY_EVT_MAX
} DRV_RF4CE_NOTIFY_EVT_e;

typedef   struct
{
	DRV_RF4CE_NOTIFY_EVT_e       eEvent;    
	void     *pvEventData;
} DRV_RF4CE_CALLBACK_DATA_t;

// CERTIFICATION
typedef enum
{
	RF4CE_CHANNEL_MIN = 0,
	RF4CE_CHANNEL_15,
	RF4CE_CHANNEL_20,
	RF4CE_CHANNEL_25,
	RF4CE_CHANNEL_MAX
} RF4CE_Channel_e;

typedef enum
{
	RF4CE_TX_POWER_MIN = 0,
	RF4CE_TX_POWER_MINUS_4P1,
	RF4CE_TX_POWER_0P3,
	RF4CE_TX_POWER_4P5,
	RF4CE_TX_POWER_7P0,
	RF4CE_TX_POWER_MAX
} RF4CE_Tx_Power_e;

typedef enum
{
	RF4CE_TX_MODE_MIN = 0,
	RF4CE_TX_MODE_UNMODULATED,
	RF4CE_TX_MODE_MODULATED,
	RF4CE_TX_MODE_MAX
} RF4CE_Tx_Mode_e;

typedef enum
{
	RF4CE_RX_MODE_MIN = 0,
	RF4CE_RX_MODE_UNMODULATED,
	RF4CE_RX_MODE_MODULATED,
	RF4CE_RX_MODE_MAX
} RF4CE_Rx_Mode_e;

typedef enum
{
	RF4CE_TEST_MODE_MIN = 0,
	RF4CE_TEST_MODE_TX_UNMODULATED,
	RF4CE_TEST_MODE_TX_MODULATED,
	RF4CE_TEST_MODE_RX_UNMODULATED,
	RF4CE_TEST_MODE_RX_MODULATED,
	RF4CE_TEST_MODE_PACKET_TX,
	RF4CE_TEST_MODE_PACKET_RX,
	RF4CE_TEST_MODE_MAX
} RF4CE_TestMode_e;

typedef enum
{
	RF4CE_RX_ENABLE_MIN = 0,
	RF4CE_RX_ENABLE_STXON,
	RF4CE_RX_ENABLE_SRXON,
	RF4CE_RX_ENABLE_SRFOFF,
	RF4CE_RX_ENABLE_SRXMASKBITSET,
	RF4CE_RX_ENABLE_SRXMASKBITCLR,
	RF4CE_RX_ENABLE_MAX
} RF4CE_RxEnable_e;

/*******************************************************************************
* Global variables and structures
*******************************************************************************/


/*******************************************************************************
* External variables and functions
*******************************************************************************/


/*******************************************************************************
* Static variables and structures
*******************************************************************************/


/*******************************************************************************
* Static function prototypes
*******************************************************************************/


 /* ------------------------------------------------------------------------------------------------
 *                                           Function Progotypes
 * ------------------------------------------------------------------------------------------------
 */
DRV_Error DRV_RF4CE_TI_Init(void);
DRV_Error DRV_RF4CE_InterruptInit(void);
DRV_Error DRV_RF4CE_TI_Reset(void);

DRV_Error DRV_RF4CE_TI_ConfigParam(void);

DRV_Error DRV_RF4CE_KeepPingOn(void);
DRV_Error DRV_RF4CE_KeepPingOff(void);

DRV_Error DRV_RF4CE_Receive(void);

DRV_Error DRV_RF4CE_TI_ReadItemEx( HUINT8 profileId, HUINT8 itemId, HUINT8 len, HUINT8 *pValue, HUINT32 *status);
DRV_Error DRV_RF4CE_TI_ReadItem( HUINT8 itemId, HUINT8 len, HUINT8 *pValue, HUINT32 *status );
DRV_Error DRV_RF4CE_TI_WriteItemEx( HUINT8 profileId, HUINT8 itemId, HUINT8 len, HUINT8 *pValue, HUINT32 *status );
DRV_Error DRV_RF4CE_TI_WriteItem( HUINT8 itemId, HUINT8 len, HUINT8 *pValue, HUINT32 *status );

DRV_Error DRV_RF4CE_REQ_PingReq(void);
DRV_Error DRV_RF4CE_REQ_InitReq(void);
DRV_Error DRV_RF4CE_REQ_PairReq( void );
DRV_Error DRV_RF4CE_REQ_PairAbortReq( void );
DRV_Error DRV_RF4CE_TI_REQ_AllowPairReq( void );
DRV_Error DRV_RF4CE_REQ_AllowPairAbortReq( void );
DRV_Error DRV_RF4CE_REQ_UnpairReq( HUINT8 dstIndex );
DRV_Error DRV_RF4CE_REQ_SendDataReq( HUINT8 dstIndex, HUINT8 profileId, HUINT16 vendorId, HUINT8 txOptions, HUINT8 len, HUINT8 *pData );
DRV_Error DRV_RF4CE_REQ_StandbyReq( HUINT8 mode );
DRV_Error DRV_RF4CE_REQ_RxEnableReq( HUINT16 duration );
DRV_Error DRV_RF4CE_REQ_EnableSleepReq( void );
DRV_Error DRV_RF4CE_REQ_DisableSleepReq( void );
DRV_Error DRV_RF4CE_REQ_SwResetReq( void );
DRV_Error DRV_RF4CE_REQ_TestModeReq();
DRV_Error DRV_RF4CE_REQ_TestRxCounterGetReq(HUINT8 resetFlag, HUINT8 *status);

DRV_Error DRV_RF4CE_REQ_GetPairedDeviceReq( void );
DRV_Error DRV_RF4CE_REQ_GetPairingTableReq( void );
DRV_Error DRV_RF4CE_REQ_GetNWKInfoReq( void );
DRV_Error DRV_RF4CE_REQ_SetIEEEAddrReq( void );
DRV_Error DRV_RF4CE_REQ_SetFactoryModeReq( void );
DRV_Error DRV_RF4CE_REQ_SendTxRxRSSIReq( void );

DRV_Error DRV_RF4CE_TI_CNF_InitCnf(HUINT8 status);
DRV_Error DRV_RF4CE_CNF_PairCnf( HUINT8 status, HUINT8 dstIndex, HUINT8 devType );
DRV_Error DRV_RF4CE_TI_CNF_AllowPairCnf( HUINT8 status, HUINT8 dstIndex, HUINT8 devType );
DRV_Error DRV_RF4CE_CNF_SendDataCnf( HUINT8 status );
DRV_Error DRV_RF4CE_CNF_StandbyCnf( HUINT8 status );
DRV_Error DRV_RF4CE_CNF_RxEnableCnf( HUINT8 status );
DRV_Error DRV_RF4CE_CNF_EnableSleepCnf( HUINT8 status );
DRV_Error DRV_RF4CE_CNF_DisableSleepCnf( HUINT8 status );
DRV_Error DRV_RF4CE_IND_UnpairInd( HUINT8 dstIndex );
DRV_Error DRV_RF4CE_CNF_PairAbortCnf( HUINT8 status );
DRV_Error DRV_RF4CE_CNF_UnpairCnf( HUINT8 status, HUINT8 dstIndex );
DRV_Error DRV_RF4CE_TI_IND_ReceiveDataInd( HUINT8 srcIndex, HUINT8 profileId, HUINT16 vendorId, HUINT8 rxLQI, HUINT8 rxFlags, HUINT8 len, HUINT8 *pData );

DRV_Error DRV_RF4CE_CNF_GetPairedDeviceCnf( void );
DRV_Error DRV_RF4CE_CNF_GetPairingTableCnf( void );
DRV_Error DRV_RF4CE_CNF_GetNWKInfoCnf( void );
DRV_Error DRV_RF4CE_CNF_SetIEEEAddrCnf( void );
DRV_Error DRV_RF4CE_CNF_SetFactoryModeCnf( void );
DRV_Error DRV_RF4CE_CNF_SendTxRxRSSICnf( void );

DRV_Error DRV_RF4CE_CE_SetTxPowerReq(RF4CE_Tx_Power_e e_TxPower);
DRV_Error DRV_RF4CE_CE_SetChannelReq(RF4CE_Channel_e e_Channel);
DRV_Error DRV_RF4CE_CE_SetTxModeReq(RF4CE_Tx_Mode_e e_TxMode);
DRV_Error DRV_RF4CE_CE_SetRxModeReq(RF4CE_Rx_Mode_e e_RxMode);
DRV_Error DRV_RF4CE_CE_SetRxEnableReq(RF4CE_RxEnable_e e_RxEnable);
DRV_Error DRV_RF4CE_CE_GetContextReq( void );
DRV_Error DRV_RF4CE_CE_GetRssiReq( void );

DRV_Error DRV_RF4CE_CE_SetTxPowerCnf(RF4CE_Msg_t* pMsg);
DRV_Error DRV_RF4CE_CE_SetChannelCnf(RF4CE_Msg_t* pMsg);
DRV_Error DRV_RF4CE_CE_SetTxModeCnf(RF4CE_Msg_t* pMsg);
DRV_Error DRV_RF4CE_CE_SetRxModeCnf(RF4CE_Msg_t* pMsg);
DRV_Error DRV_RF4CE_CE_SetRxEnableCnf(RF4CE_Msg_t* pMsg);
DRV_Error DRV_RF4CE_CE_GetContextCnf(RF4CE_Msg_t* pMsg);
DRV_Error DRV_RF4CE_CE_GetRssiCnf(RF4CE_Msg_t* pMsg);

DRV_Error DRV_RF4CE_CE_SetTxPower(RF4CE_Tx_Power_e e_TxPower);
DRV_Error DRV_RF4CE_CE_SetChannel(RF4CE_Channel_e e_Channel);
DRV_Error DRV_RF4CE_CE_SetTxMode(RF4CE_Tx_Mode_e e_TxMode);
DRV_Error DRV_RF4CE_CE_SetRxMode(RF4CE_Rx_Mode_e e_RxMode);

#endif /* _DRV_RF4CE_TI_H_ */


/* end of file */
