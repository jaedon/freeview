/*
 * Copyright 2009-2012 Irdeto B.V.
 *
 * This file and the information contained herein are the subject of copyright
 * and intellectual property rights under international convention. All rights
 * reserved. No part of this file may be reproduced, stored in a retrieval
 * system or transmitted in any form by any means, electronic, mechanical or
 * optical, in whole or in part, without the prior written permission of Irdeto
 * B.V.
 */

/**
 * @file UniversalClient_API.h
 * Cloaked CA Agent Base API. This file contains the common informational methods necessary
 * for any integration with the Cloaked CA Agent.
 */
#ifndef UNIVERSALCLIENT_API_H__INCLUDED__
#define UNIVERSALCLIENT_API_H__INCLUDED__

#include "UniversalClient_Types.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NULL
/**
 * Simple NULL typedef.
 */
#define NULL 0
#endif


/**
 * Generic size for ::uc_buffer_st used in APIs.
 *
 * If no special comments, the application can allocate memory with ::UC_BUFFER_GENERIC_SIZE bytes long
 * for uc_buffer_st::bytes used in some API.
 * For example, the application can allocate memory with ::UC_BUFFER_GENERIC_SIZE bytes for uc_buffer_st::bytes used in ::UniversalClient_GetVersion
 * and set uc_buffer_st::length to ::UC_BUFFER_GENERIC_SIZE.
 */
#define UC_BUFFER_GENERIC_SIZE (256)

/**
 * Generic size for ::uc_buffer_st used in APIs.
 *
 * ::UC_BYTES_GENERIC_SIZE is kept here in order to keep the compatibility.
 */

#define UC_BYTES_GENERIC_SIZE UC_BUFFER_GENERIC_SIZE

/**
 * Size for product id.
 *
 * Product id is used to uniquely identify a product.
 */
#define UC_SIZE_PRODUCT_ID (2)

/**
 * Max length of smartcard serial number buffer.
 */
#define UC_MAX_SMARTCARD_SERIAL_NUMBER_LENTH 20

/**
 * Size of status string
 */
#define UC_STATUS_LENGTH 9

/**
 * Maximum length of version string
 */
#define UC_MAX_VERSION_STRING_LENGTH        128

/**
 * maximu operator count
 * The maximum operator count supported.
 */
#define UC_MAX_OPERATOR_COUNT               8

/**
 * maximum operator name length
 */
#define UC_MAX_OPERATOR_NAME_LENGTH         20

/**
 * size of unique address
 */
#define UC_UNIQUE_ADDRESS_SIZE              4

/**
 * maximum length of information string
 */
#define UC_MAX_ITEM_LENGH                   100

/**
 * size of nationality
 */
#define UC_NATIONALITY_SIZE                 3

/**
 * size of homing channel data
 */
#define UC_HOMING_CHANNEL_USER_DATA_SIZE    32

/**
 * size of middleware user data
 */
#define UC_MW_DATA_USER_DATA_SIZE           128

/**
 * maximum sectors supported by each operator
 */
#define MAX_ACTIVE_SECTOR_COUNT_PER_OPERATOR   8


/**
 * Size of user data
 */
#define UC_TMS_USER_DATA_SIZE 32

/** @defgroup result Result Code List
 *  List of all possible values of ::uc_result codes.
 *
 *  Except where noted, applications and implementations should
 *  not expect particular failure codes to always be returned from functions, other than ::UC_ERROR_SUCCESS.
 *  This is because over time and across different implementations the error codes returned from different error situations
 *  may change.
 *  @{
 */

/**
 * Result code enumerator type.
 */
typedef enum {
    /** (0x00000000) The operation was successful. */
    UC_ERROR_SUCCESS                        = 0x00000000,

    /** (0x00000001) One or more required parameters were NULL. */
    UC_ERROR_NULL_PARAM                     = 0x00000001,

    /** (0x00000002) Memory could not be allocated. */
    UC_ERROR_OUT_OF_MEMORY                  = 0x00000002,

    /** (0x00000003) Unable to create requested resource. */
    UC_ERROR_UNABLE_TO_CREATE               = 0x00000003,

    /** (0x00000004) Generic OS failure. */
    UC_ERROR_OS_FAILURE                     = 0x00000004,

    /** (0x00000005) The timeout expired before the object was ready. */
    UC_ERROR_WAIT_ABORTED                   = 0x00000005,

    /** (0x00000006) The buffer passed in is too small. */
    UC_ERROR_INSUFFICIENT_BUFFER            = 0x00000006,

    /** (0x00000007) The specified resource could not be found. */
    UC_ERROR_RESOURCE_NOT_FOUND             = 0x00000007,

    /** (0x00000008) The resource name specified is invalid. */
    UC_ERROR_BAD_RESOURCE_NAME              = 0x00000008,

    /** (0x00000009) The requested feature is not implemented. */
    UC_ERROR_NOT_IMPLEMENTED                = 0x00000009,

    /** (0x0000000A) A connection is still open; stream cannot be closed. */
    UC_ERROR_CONNECTION_STILL_OPEN          = 0x0000000A,

    /** (0x0000000B) The handle is not valid. */
    UC_ERROR_INVALID_HANDLE                 = 0x0000000B,

    /** (0x0000000C) The handle is valid but is not the correct type. */
    UC_ERROR_WRONG_HANDLE_TYPE              = 0x0000000C,

    /** (0x0000000D) Too many handles are opened already. */
    UC_ERROR_TOO_MANY_HANDLES               = 0x0000000D,

    /** (0x0000000E) An attempt to shut down was made while a handle is still open. */
    UC_ERROR_HANDLE_STILL_OPEN              = 0x0000000E,

    /** (0x0000000F) This operation cannot be completed because the Cloaked CA Agent was already started. */
    UC_ERROR_ALREADY_STARTED                = 0x0000000F,

    /** (0x00000010) This operation cannot be completed because the Cloaked CA Agent was already stopped. */
    UC_ERROR_ALREADY_STOPPED                = 0x00000010,

    /** (0x00000011) An internal message queue is full and cannot accept more messages. */
    UC_ERROR_QUEUE_FULL                     = 0x00000011,

    /** (0x00000012) A required implementation of an SPI method is missing. */
    UC_ERROR_MISSING_SPI_METHOD             = 0x00000012,

    /** (0x00000013) The version of the SPI implementation is not compatible with the client. */
    UC_ERROR_INCOMPATIBLE_SPI_VERSION       = 0x00000013,

    /** (0x00000014) An invalid message was received. */
    UC_ERROR_MESSAGE_INVALID                = 0x00000014,

    /** (0x00000015) The specified length was not valid. */
    UC_ERROR_INVALID_LENGTH                 = 0x00000015,

    /** (0x00000016) Some required internal data is missing. */
    UC_ERROR_MISSING_DATA                   = 0x00000016,

    /** (0x00000017) Page is not what we want. */
    UC_ERROR_NOT_WANTED_ECMPAGE             = 0x00000017,

    /** (0x00000018) Generic driver failure. */
    UC_ERROR_DRIVER_FAILURE                 = 0x00000018,

    /** (0x00000019) Not entitled. */
    UC_ERROR_NOT_ENTITLED                   = 0x00000019,

    /** (0x0000001A) Invalid PK. */
    UC_ERROR_INVALID_PK                     = 0x0000001A,

    /** (0x0000001B) Invalid GK. */
    UC_ERROR_INVALID_GK                     = 0x0000001B,

    /** (0x0000001C) Invalid TG. */
    UC_ERROR_INVALID_TG                     = 0x0000001C,

    /** (0x0000001D) Missing CWDK. */
    UC_ERROR_INVALID_CWDK                   = 0x0000001D,

    /** (0x0000001E) No CA regional info. */
    UC_ERROR_NO_CA_REGIONAL_INFO            = 0x0000001E,

    /** (0x0000001F) Regional blocking. */
    UC_ERROR_REGIONAL_BLOCKING              = 0x0000001F,

    /** (0x00000020) Restricted opcode. */
    UC_ERROR_RESTRICTED_OPCODE              = 0x00000020,

    /** (0x00000021) Timestamp filter failed. */
    UC_ERROR_TIMESTAMP_FILTER               = 0x00000021,

    /** (0x00000022) Type filter failed. */
    UC_ERROR_TYPE_FILTER                    = 0x00000022,

    /** (0x00000023) Signature verification failed. */
    UC_ERROR_SIGNATURE_VERIFICATION         = 0x00000023,

    /** (0x00000024) TMS failed. */
    UC_ERROR_TMS_FAILED                     = 0x00000024,

    /** (0x00000025) Not PVR entitled. */
    UC_ERROR_NOT_PVR_ENTITLED               = 0x00000025,

    /** (0x00000026) This operation cannot be completed because PMT is not notified. */
    UC_ERROR_UNABLE_TO_CONFIG_PVR_RECORD    = 0x00000026,

    /** (0x00000027) This operation cannot be completed while a request for PVR record  is still being processed. */
    UC_ERROR_CONFIG_PVR_RECORD_STILL_OPEN   = 0x00000027,

    /** (0x00000028) This operation cannot be completed because PVR MSK is missing. */
    UC_ERROR_MISSING_PVR_MSK                = 0x00000028,

    /** (0x00000029) The recorded content expired */
    UC_ERROR_PVR_CONTENT_EXPIRED            = 0x00000029,

    /** (0x0000002A) Failed to generate PVR session key */
    UC_ERROR_FAILED_TO_GENERATE_SESSION_KEY = 0x0000002A,

    /** (0x0000002B) The PVR metadata is invalid */
    UC_ERROR_INVALID_PVR_METADATA          = 0x0000002B,

    /** (0x0000002C) The client type is wrong, some feature only support a certain client type.
    For example, PVR can work only on clients with secure chipset.
    */
    UC_ERROR_CLIENT_TYPE_ERROR              = 0x0000002C,

    /** (0x0000002D) Invalid sector. */
    UC_ERROR_INVALID_SECTOR                 = 0x0000002D,

    /** (0x0000002E) Client type error. PPV VOD feature should base on Secure Chipset mode. */
    UC_ERROR_VOD_CLIENT_TYPE_ERROR          = 0x0000002E,

    /** (0x0000002F) No serial number. */
    UC_ERROR_VOD_NO_SERIAL_NUMBER           = 0x0000002F,

    /** (0x00000030) Invalid nonce. */
    UC_ERROR_VOD_INVALID_NONCE              = 0x00000030,

    /** (0x00000031) Pkey Hash mismatch */
    UC_ERROR_PKEY_HASH_MISMATCH             = 0x00000031,

    /** (0x00000032) The variant of the device identifiers is not compatible with the client. */
    UC_ERROR_INCOMPATIBLE_VARIANT           = 0x00000032,

    /** (0x00000033) No Nationality. */
    UC_ERROR_NO_NATIONALITY                 = 0x00000033,

    /** (0x00000034) The uniqueaddress is invalid with the client. */
    UC_ERROR_INVALID_UNIQUE_ADDRESS         = 0x00000034,

    /** (0x00000035) This result code is not used now. */
    UC_ERROR_DIGITAL_COPY_NOMORE            = 0x00000035,

    /** (0x00000036) Black out. */
    UC_ERROR_BLACK_OUT                      = 0x00000036,

    /** (0x00000037) Homing Channel Failed. */
    UC_ERROR_HOMING_CHANNEL_FAILED          = 0x00000037,

    /** (0x00000038) FlexiFlash Failed. */
    UC_ERROR_FLEXIFLASH_FAILED          = 0x00000038,

    /** (0x00000039) Middleware User Data Failed*/
    UC_ERROR_MIDDLEWARE_USER_DATA_FAILED      = 0x00000039,

    /** (0x0000003A) Pre-Enable product expired */
    UC_ERROR_PRE_ENABLE_PRODUCT_EXPIRED     = 0x0000003A,

    /** (0x0000003B) Missing Pre-Enable Session Key */
    UC_ERROR_MISSING_PESK           = 0x0000003B,

    /** (0x0000003C) The OTP data is invalid */
    UC_ERROR_INVALID_OTP_DATA           = 0x0000003C,

    /** (0x0000003D) The personalized data is invalid */
    UC_ERROR_INVALID_PERSONALIZED_DATA           = 0x0000003D,

    /** (0x0000003E) New CG for FSU product received */
    UC_ERROR_NEW_CG_FOR_FSU_RECEIVED           = 0x0000003E,

    /** (0x0000003F) New CG for Push VOD product received */
    UC_ERROR_NEW_CG_FOR_PUSHVOD_RECEIVED           = 0x0000003F,

    /** (0x00000040) CG Mismatch */
    UC_ERROR_CG_MISMATCH           = 0x00000040,

    /** (0x00000041) SG Mismatch */
    UC_ERROR_SG_MISMATCH           = 0x00000041,

    /** (0x00000042) Invalid PK Index */
    UC_ERROR_INVALID_PK_INDEX           = 0x00000042,

    /** (0x00000043) Invalid GK Index */
    UC_ERROR_INVALID_GK_INDEX           = 0x00000043,

    /** (0x00000044) Macrovision Failed. */
    UC_ERROR_MACROVISION_FAILED           = 0x00000044,

    /** (0x00000045) SN RANGE Failed. */
    UC_ERROR_SN_RANGE_FAILED           = 0x00000045,

    /** (0x00000046) Unified Client Error. */
    UC_ERROR_CONVERGENT_CLIENT_GENERIC_ERROR      = 0x00000046,

    /** (0x00000047) Smartcard is out. */
    UC_ERROR_SMARTCARD_OUT      = 0x00000047,

    /** (0x00000048) Unknown Card (non-Irdeto), or the smartcard is upside down. */
    UC_ERROR_SMARTCARD_UNKNOWN    = 0x00000048,

    /** (0x00000049) Card Error, communications with Smartcard have failed. */
    UC_ERROR_SMARTCARD_ERROR      = 0x00000049,

    /** (0x0000004A) Corrupted Data, the data is corrupted */
    UC_ERROR_IO_DATA_CORRUPTED      = 0x0000004A,

    /** (0x0000004B) Invalid change verion. */
    UC_ERROR_INVALID_CHANGE_VERSION      = 0x0000004B,

    /** (0x0000004C) This Block has been downloaded.*/
    UC_ERROR_FLEXIFLASH_BLOCK_DOWNLOAD_DUPLICATION      = 0x0000004C,

    /** (0x0000004D) The Global SC EMM has been processed by Client.*/
    UC_ERROR_GLOBAL_SC_EMM_DUPLICATION      = 0x0000004D,

    /** (0x0000004E) Stop EMM processing.*/
    UC_ERROR_STOP_EMM_PROCESSING      = 0x0000004E,

    /** (0x0000004F) Load CCA package failed.*/
    UC_ERROR_LOAD_CCAPACKAGE_FAILED      = 0x0000004F,

    /** (0x00000050) Invalid CFG data.*/
    UC_ERROR_INVALID_CFG_DATA      = 0x00000050,

    /** (0x00000051) Package data is invalid.*/
    UC_ERROR_INVALID_PACKAGE_DATA      = 0x00000051,

    /** (0x00000052) The VM failed.*/
    UC_ERROR_VM_FAILURE      = 0x00000052,

    /** (0x00000053) The securecore is not loaded.*/
    UC_ERROR_SECURECORE_NOT_LOADED      = 0x00000053,

    /** (0x00000054) Invalid area index.*/
    UC_ERROR_INVALID_AREA_INDEX      = 0x00000054,

    /** (0x00000055) An character is not expected when parse a CFG file*/
    UC_ERROR_UNEXPECTED_CHARACTER = 0x00000055,

    /** (0x00000056) Get section Data without parse a CFG file .*/
    UC_ERROR_NO_CFG_PARSED = 0x00000056,

    /** (0x00000057) Parsing a CFG file before release the former parsing result .*/
    UC_ERROR_CFG_PARSED_ALREADY = 0x00000057,

    /** (0x00000058) No specified section found in the CFG file .*/
    UC_ERROR_SECTION_NOT_FOUND = 0x00000058,

    /** (0x00000059) The VM with special id has already defined .*/
    UC_ERROR_VM_ALREADY_DEFINED = 0x00000059,

    /** (0x0000005A) Error happen during create the "vm config" object .*/
    UC_ERROR_VM_CREATE_VMCONFIG = 0x0000005A,

    /** (0x0000005B) Error happen during create the vm instance .*/
    UC_ERROR_VM_CREATE = 0x0000005B,

    /** (0x0000005C) Too many vm created, vm number reach top limit .*/
    UC_ERROR_VM_TOP_LIMIT =  0x0000005C,

    /** (0x0000005D) Error ocurr during loading bytecode image to vm .*/
    UC_ERROR_VM_LOAD_IMAGE = 0x0000005D,

    /** (0x0000005E) Error ocurr during memory map from native memory space to vm memory space .*/
    UC_ERROR_VM_MEMMAP = 0x0000005E,

    /** (0x0000005F) Error ocurr during execute vm .*/
    UC_ERROR_VM_EXECUTE = 0x0000005F,

    /** (0x00000060) Error ocurr VM IO .*/
    UC_ERROR_VM_IO = 0x00000060,

    /** (0x00000061) Error ocurr VM RESET .*/
    UC_ERROR_VM_RESET = 0x00000061,

    /** (0x00000062) The root key hash check failed.*/
    UC_ERROR_ROOT_KEY_HASH_CHECK_FAILED = 0x00000062,

    /** (0x00000063) Unsupported package compress algorithm.*/
    UC_ERROR_COMPRESS_ALGORITHM_NOT_SUPPORT = 0x00000063,

    /** (0x00000064) The SYS ID is invalid.*/
    UC_ERROR_INVALID_SYS_ID = 0x00000064,

    /** (0x00000065) The version of the client is too low to support CCA package download.*/
    UC_ERROR_LOW_CLIENT_VERSION = 0x00000065,

    /** (0x00000066) The CA System ID is invalid */
    UC_ERROR_INVALID_CA_SYSTEM_ID = 0x00000066,

    /** (0x00000067) Anchor Failed(illegal device). */
    UC_ERROR_DEVICE_INVALID                 = 0x00000067,

    /** (0x00000068) Request entitlementkeys failed. */
    UC_ERROR_REQUEST_ENTITLEMENT_FAILED     = 0x00000068,

    /** (0x00000069) No response for request for a certain time interval. */
    UC_ERROR_REQUEST_ENTITLEMENT_RESPONSE_TIME_OUT   = 0x00000069,

    /** (0x0000006A) CCIS internal error. */
    UC_ERROR_CCIS_INTERNAL_ERROR            = 0x0000006A,

    /** (0x0000006B) Anchor failed(DeviceID/UA mismactch). */
    UC_ERROR_DEVICEID_UA_MISMATCH           = 0x0000006B,

    /** (0x0000006C) VOD not entitled. */
    UC_ERROR_VOD_NOT_ENTITLED               = 0x0000006C

} uc_result;

/** @} */ /* End of results */


/** @defgroup status_messages Status Messages
 *  List of all possible status message string. This status message strings can be used
 *  for technical support purposes or for debugging problems with a client integration.
 *  \note All Ixxx-0 error status are interim status. They should not be displayed in the error banner.
 *  The Application can simply ignore Ixxx-0 in the error banner.
 *  @{
 */

/**
 * Length of connection status string
 *
 * It is recommended to be not used anymore.
 */
#define UC_CONNECTION_STATUS_STRING_LENGTH (UC_STATUS_LENGTH-1)

/**
 * Size of connection status string
 *
 * Used by ::UniversalClient_GetServiceStatus.
 */
#define UC_CONNECTION_STATUS_STRING_SIZE (UC_STATUS_LENGTH)

/**
 * Service Status Screen Text: Service is currently descrambled.
 *
 * Being Descrambled. This message is sent  when an ECM has successfully been processed by the client,
 * and a descrambling key has already been returned to the stream implementation via ::UniversalClientSPI_Stream_SetDescramblingKey.
 */
#define ERR_MSG_D029        "D029-0  "

/**
 * Service Status Screen Text: EMM service OK or FTA service.
 *
 * EMM Accepted or FTA Service. For EMM service, this message means that an EMM has successfully been processed by the client.
 * For ECM service, this message means that current service is a clear service, i.e, no CA information is found in PMT.
 */
#define ERR_MSG_D100        "D100-0  "

/**
 * Service Status Screen Text: No matching CA System ID in CAT.
 *
 * No Matched CA System ID. This message is sent when no matched CA System ID found in CAT.
 */
#define ERR_MSG_D101        "D101-0  "

/**
 * Service Status Screen Text: PVR Record Request OK.
 *
 * PVR Record Request Accepted. This message is sent when a PVR record request has successfully been processed by the client. This also means that
 * PVR Session Key has already been returned to the SPI via ::UniversalClientSPI_PVR_SetSessionKey and PVR Session Metadata has already been notified
 * to the application via message uc_service_message_type::UC_SERVICE_PVR_SESSION_METADATA_REPLY.
 */
#define ERR_MSG_D126        "D126-0  "

/**
 * Service Status Screen Text: PVR Playback Request OK.
 *
 * PVR Playback Request Accepted. This message is sent when an PVR playback request has successfully been processed by the client. This also means that
 * PVR Session Key has already been returned to the SPI implementation
 * via ::UniversalClientSPI_PVR_SetSessionKey.
 */
#define ERR_MSG_D127        "D127-0  "

/**
 * Service Status Screen Text: CCA package successfully loaded.
 *
 * This message is sent when the CCA package has been loaded successfully.
 */
#define ERR_MSG_D500         "D500-0  "

/**
 * Service Status Screen Text: Incomplete Definition.
 *
 * CAT or PMT Absent. This message is sent when the service has been opened but no PMT or CAT is defined..
 * I101-0 is the default service status once the service has been opened.
 *
 * \note All Ixxx-0 error status are interim status. They should not be displayed in the error banner.
 * The Application can simply ignore Ixxx-0 in the error banner.
 */
#define ERR_MSG_I101        "I101-0  "

/**
 * Service Status Screen Text: No ECM/EMM data received.
 *
 * No ECM/EMM Received.
 *
 * For EMM service, this message is sent when the service has been opened and CAT has been defined,
 * but NO EMM has been received.
 *
 * For ECM service, this message is sent when the service has been opened and PMT has been defined,
 * but NO ECM has been received.
 *
 * \note All Ixxx-0 error status are interim status. They should not be displayed in the error banner.
 * The Application can simply ignore Ixxx-0 in the error banner.
 */
#define ERR_MSG_I102        "I102-0  "

/**
 * Service Status Screen Text: New CG for FSU product has been received.
 *
 * The compounds related new CG for FSU product has been received. This message is sent
 * when the Product Overwrite EMM with new CG for FSU product has been handled successfully by the client.
 *
 * \note All Ixxx-0 error status are interim status. They should not be displayed in the error banner.
 * The Application can simply ignore Ixxx-0 in the error banner.
 */
#define ERR_MSG_I300        "I300-0  "

/**
 * Service Status Screen Text: New CG for Push VOD product has been received.
 *
 * The compounds related new CG for Push VOD product has been received. This message is sent
 * when the Product Overwrite EMM with new CG for Push VOD product has been handled successfully by the client.
 *
 * \note All Ixxx-0 error status are interim status. They should not be displayed in the error banner.
 * The Application can simply ignore Ixxx-0 in the error banner.
 */
#define ERR_MSG_I301        "I301-0  "

/**
 * Service Status Screen Text: CG Mismatch.
 *
 * Incorrect CG. This message is sent when an EMM has been rejected by the client because the client does not have the
 * right CG the EMM refers to.
 *
 * \note All Ixxx-0 error status are interim status. They should not be displayed in the error banner.
 * The Application can simply ignore Ixxx-0 in the error banner.
 */
#define ERR_MSG_I302        "I302-0  "

/**
 * Service Status Screen Text: SG Mismatch.
 *
 * Incorrect SG. This message is sent when an EMM has been rejected by the client because the client does not have the
 * right SG the EMM refers to.
 *
 * \note All Ixxx-0 error status are interim status. They should not be displayed in the error banner.
 * The Application can simply ignore Ixxx-0 in the error banner.
 */
#define ERR_MSG_I303        "I303-0  "

/**
 * Service Status Screen Text: Invalid P-Key Index.
 *
 * Incorrect P-Key Index. This message is sent when an EMM has been rejected by the client because the client does not support
 * the P-Key index the EMM refers to.
 *
 * \note All Ixxx-0 error status are interim status. They should not be displayed in the error banner.
 * The Application can simply ignore Ixxx-0 in the error banner.
 */
#define ERR_MSG_I304        "I304-0  "

/**
 * Service Status Screen Text: Invalid G-Key Index.
 *
 * Incorrect G-Key Index. This message is sent when an EMM has been rejected by the client because the client does not support
 * the G-Key index the EMM refers to.
 *
 * \note All Ixxx-0 error status are interim status. They should not be displayed in the error banner.
 * The Application can simply ignore Ixxx-0 in the error banner.
 */
#define ERR_MSG_I305        "I305-0  "

/**
 * Service Status Screen Text: Invalid Timestamp received.
 *
 * Invalid Timestamp. This message is sent when an EMM has been rejected by the client because the timestamp the EMM refers to isn't valid.
 *
 * \note All Ixxx-0 error status are interim status. They should not be displayed in the error banner.
 * The Application can simply ignore Ixxx-0 in the error banner.
 */
#define ERR_MSG_I306        "I306-0  "

/**
 * Service Status Screen Text: Variant Mismatch.
 *
 * Incorrect Variant. This message is sent when an EMM has been rejected by the client because the client does not have the
 * right Variant the EMM refers to.
 *
 * \note All Ixxx-0 error status are interim status. They should not be displayed in the error banner.
 * The Application can simply ignore Ixxx-0 in the error banner.
 */
#define ERR_MSG_I307        "I307-0  "

/**
 * Service Status Screen Text: P-Key Hash Mismatch.
 *
 * Incorrect P-Key Hash. This message is sent when an EMM has been rejected by the client because the client does not have the
 * right P-Key Hash the EMM refers to.
 *
 * \note All Ixxx-0 error status are interim status. They should not be displayed in the error banner.
 * The Application can simply ignore Ixxx-0 in the error banner.
 */
#define ERR_MSG_I308        "I308-0  "

/**
 * Service Status Screen Text: Opcode Restricted.
 *
 * Incorrect Opcode. This message is sent when an EMM has been rejected by the client because the client does not support
 * the Opcode the EMM refers to.
 *
 * \note All Ixxx-0 error status are interim status. They should not be displayed in the error banner.
 * The Application can simply ignore Ixxx-0 in the error banner.
 */
#define ERR_MSG_I309        "I309-0  "

/**
 * Service Status Screen Text: Invalid VOD Nonce.
 *
 * Incorrect VOD Nonce. This message is sent when an EMM has been rejected by the client because the client does not have the
 * right VOD Nonce the EMM refers to.
 *
 * \note All Ixxx-0 error status are interim status. They should not be displayed in the error banner.
 * The Application can simply ignore Ixxx-0 in the error banner.
 */
#define ERR_MSG_I310        "I310-0  "

/**
 * Service Status Screen Text: TMS Failed.
 *
 * TMS Failed. This message is sent when an EMM related to TMS has been handled unsuccessfully by the client.
 *
 * \note All Ixxx-0 error status are interim status. They should not be displayed in the error banner.
 * The Application can simply ignore Ixxx-0 in the error banner.
 */
#define ERR_MSG_I311        "I311-0  "

/**
 * Service Status Screen Text: Homing Channel Failed.
 *
 * Homing Channel Failed. This message is sent when an EMM related to Homing Channel has been handled unsuccessfully by the client.
 *
 * \note All Ixxx-0 error status are interim status. They should not be displayed in the error banner.
 * The Application can simply ignore Ixxx-0 in the error banner.
 */
#define ERR_MSG_I312        "I312-0  "

/**
 * Service Status Screen Text: Invalid Middleware User Data.
 *
 * Incorrect Middleware User Data. This message is sent when an EMM related to Middleware User Data has been handled unsuccessfully by the client.
 *
 * \note All Ixxx-0 error status are interim status. They should not be displayed in the error banner.
 * The Application can simply ignore Ixxx-0 in the error banner.
 */
#define ERR_MSG_I313        "I313-0  "

/**
 * Service Status Screen Text: Flexi Flash Failed.
 *
 * Flexi Flash Failed. This message is sent when an EMM related to Flexi Flash has been handled unsuccessfully by the client.
 *
 * \note All Ixxx-0 error status are interim status. They should not be displayed in the error banner.
 * The Application can simply ignore Ixxx-0 in the error banner.
 */
#define ERR_MSG_I314        "I314-0  "

/**
 * Service Status Screen Text: SN Mapping EMM handled successfully.
 *
 * SN Mapping EMM handled successfully. This message is sent when SN Mapping EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error status are interim status. They should not be displayed in the error banner.
 * The Application can simply ignore Ixxx-0 in the error banner.
 */
#define ERR_MSG_I315        "I315-0  "

/**
 * Service Status Screen Text: No valid entitlement found.
 *
 * Not Entitled. This message is sent when an EMM has been rejected by the client because the client is not entitled
 * to use the product the EMM refers to.
 *
 * \note All Ixxx-0 error status are interim status. They should not be displayed in the error banner.
 * The Application can simply ignore Ixxx-0 in the error banner.
 */
#define ERR_MSG_I316        "I316-0  "

/**
 * Service Status Screen Text: No valid sector found.
 *
 * Incorrect Sector. This message is sent when an EMM has been rejected by the client because the client does not have the
 * right sector the EMM refers to.
 *
 * \note All Ixxx-0 error status are interim status. They should not be displayed in the error banner.
 * The Application can simply ignore Ixxx-0 in the error banner.
 */
#define ERR_MSG_I317        "I317-0  "

/**
 * Service Status Screen Text: Sector Overwrite EMM handled successfully.
 *
 * Sector Overwrite EMM handled successfully. This message is sent when Sector Overwrite EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error status are interim status. They should not be displayed in the error banner.
 * The Application can simply ignore Ixxx-0 in the error banner.
 */
#define ERR_MSG_I318        "I318-0  "

/**
 * Service Status Screen Text: Pairing EMM handled successfully.
 *
 * Pairing EMM handled successfully. This message is sent when Pairing EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error status are interim status. They should not be displayed in the error banner.
 * The Application can simply ignore Ixxx-0 in the error banner.
 */
#define ERR_MSG_I319        "I319-0  "

/**
 * Service Status Screen Text: Product Key EMM handled successfully.
 *
 * Product Key EMM handled successfully. This message is sent when Product Key EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error status are interim status. They should not be displayed in the error banner.
 * The Application can simply ignore Ixxx-0 in the error banner.
 */
#define ERR_MSG_I320        "I320-0  "

/**
 * Service Status Screen Text: Product Overwrite EMM handled successfully.
 *
 * Product Overwrite EMM handled successfully. This message is sent when Product Overwrite EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error status are interim status. They should not be displayed in the error banner.
 * The Application can simply ignore Ixxx-0 in the error banner.
 */
#define ERR_MSG_I321        "I321-0  "

/**
 * Service Status Screen Text: Region Control EMM handled successfully.
 *
 * Region Control EMM handled successfully. This message is sent when Region Control EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error status are interim status. They should not be displayed in the error banner.
 * The Application can simply ignore Ixxx-0 in the error banner.
 */
#define ERR_MSG_I322        "I322-0  "

/**
 * Service Status Screen Text: PVR MSK EMM handled successfully.
 *
 * PVR MSK EMM handled successfully. This message is sent when PVR MSK EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error status are interim status. They should not be displayed in the error banner.
 * The Application can simply ignore Ixxx-0 in the error banner.
 */
#define ERR_MSG_I323        "I323-0  "

/**
 * Service Status Screen Text: VOD Product Overwrite EMM handled successfully.
 *
 * VOD Product Overwrite EMM handled successfully. This message is sent when VOD Product Overwrite EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error status are interim status. They should not be displayed in the error banner.
 * The Application can simply ignore Ixxx-0 in the error banner.
 */
#define ERR_MSG_I324        "I324-0  "

/**
 * Service Status Screen Text: VOD Asset ID EMM handled successfully.
 *
 * VOD Asset ID EMM handled successfully. This message is sent when VOD Asset ID EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error status are interim status. They should not be displayed in the error banner.
 * The Application can simply ignore Ixxx-0 in the error banner.
 */
#define ERR_MSG_I325        "I325-0  "

/**
 * Service Status Screen Text: Nationality EMM handled successfully.
 *
 * Nationality EMM handled successfully. This message is sent when Nationality EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error status are interim status. They should not be displayed in the error banner.
 * The Application can simply ignore Ixxx-0 in the error banner.
 */
#define ERR_MSG_I326        "I326-0  "

/**
 * Service Status Screen Text: Product Delete EMM handled successfully.
 *
 * Product Delete EMM handled successfully. This message is sent when Product Delete EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error status are interim status. They should not be displayed in the error banner.
 * The Application can simply ignore Ixxx-0 in the error banner.
 */
#define ERR_MSG_I327        "I327-0  "

/**
 * Service Status Screen Text: Entitlement Property EMM handled successfully.
 *
 * Entitlement Property EMM handled successfully. This message is sent when Entitlement Property EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error status are interim status. They should not be displayed in the error banner.
 * The Application can simply ignore Ixxx-0 in the error banner.
 */
#define ERR_MSG_I328        "I328-0  "

/**
 * Service Status Screen Text: Timestamp EMM handled successfully.
 *
 * Timestamp EMM handled successfully. This message is sent when Timestamp EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error status are interim status. They should not be displayed in the error banner.
 * The Application can simply ignore Ixxx-0 in the error banner.
 */
#define ERR_MSG_I329        "I329-0  "


/**
 * Service Status Screen Text: P-Key mismatch.
 *
 * Incorrect PKey. This message is sent when an EMM has been rejected by the client because the client does not have the
 * right P-key the EMM refers to.
 *
 * \note All Ixxx-0 error status are interim status. They should not be displayed in the error banner.
 * The Application can simply ignore Ixxx-0 in the error banner.
 */
#define ERR_MSG_I330        "I330-0  "

/**
 * Service Status Screen Text: User Area EMM handled successfully.
 *
 * User Area EMM handled successfully. This message is sent when User Area EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error status are interim status. They should not be displayed in the error banner.
 * The Application can simply ignore Ixxx-0 in the error banner.
 */
#define ERR_MSG_I331        "I331-0  "

/**
 * Service Status Screen Text: Application Data EMM handled successfully.
 *
 * Application Data EMM handled successfully. This message is sent when Application Data EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error status are interim status. They should not be displayed in the error banner.
 * The Application can simply ignore Ixxx-0 in the error banner.
 */
#define ERR_MSG_I332        "I332-0  "

/**
 * Service Status Screen Text: Filter Criteria EMM handled successfully.
 *
 * Filter Criteria EMM handled successfully. This message is sent when Filter Criteria EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error status are interim status. They should not be displayed in the error banner.
 * The Application can simply ignore Ixxx-0 in the error banner.
 */
#define ERR_MSG_I333        "I333-0  "

/**
 * Service Status Screen Text: Package EMM handled successfully.
 *
 * Package EMM handled successfully. This message is sent when Package EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error status are interim status. They should not be displayed in the error banner.
 * The Application can simply ignore Ixxx-0 in the error banner.
 */
#define ERR_MSG_I334        "I334-0  "

/**
 * Service Status Screen Text: Block Download EMM handled successfully.
 *
 * Block Download EMM handled successfully. This message is sent when Block Download EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error status are interim status. They should not be displayed in the error banner.
 * The Application can simply ignore Ixxx-0 in the error banner.
 */
#define ERR_MSG_I335        "I335-0  "

/**
 * Service Status Screen Text: IRD EMM handled successfully.
 *
 * IRD EMM handled successfully. This message is sent when IRD EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error status are interim status. They should not be displayed in the error banner.
 * The Application can simply ignore Ixxx-0 in the error banner.
 */
#define ERR_MSG_I336        "I336-0  "

/**
 * Service Status Screen Text: Unique SN Mapping EMM handled successfully.
 *
 * Unique SN Mapping EMM handled successfully. This message is sent when Unique SN Mapping EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error status are interim status. They should not be displayed in the error banner.
 * The Application can simply ignore Ixxx-0 in the error banner.
 */
#define ERR_MSG_I337        "I337-0  "

/**
 * Service Status Screen Text: Signed CCP CAM EMM handled successfully.
 *
 * Signed CCP CAM EMM handled successfully. This message is sent when Signed CCP CAM EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error status are interim status. They should not be displayed in the error banner.
 * The Application can simply ignore Ixxx-0 in the error banner.
 */
#define ERR_MSG_I338        "I338-0  "

/**
 * Service Status Screen Text: TM Message EMM handled successfully.
 *
 * TM Message EMM handled successfully. This message is sent when TM Message EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error status are interim status. They should not be displayed in the error banner.
 * The Application can simply ignore Ixxx-0 in the error banner.
 */
#define ERR_MSG_I339        "I339-0  "

/**
 * Service Status Screen Text: Macrovision Configuration EMM handled successfully.
 *
 * Macrovision Configuration EMM handled successfully. This message is sent when Macrovision Configuration EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error status are interim status. They should not be displayed in the error banner.
 * The Application can simply ignore Ixxx-0 in the error banner.
 */
#define ERR_MSG_I340        "I340-0  "

/**
 * Service Status Screen Text: Extended TMS Message EMM handled successfully.
 *
 * Extended TMS Message EMM handled successfully. This message is sent when Extended TMS Message EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error status are interim status. They should not be displayed in the error banner.
 * The Application can simply ignore Ixxx-0 in the error banner.
 */
#define ERR_MSG_I341        "I341-0  "

/**
 * Service Status Screen Text: Reset To Factory State EMM handled successfully.
 *
 * Reset To Factory State EMM handled successfully. This message is sent when Reset To Factory State EMM has been handled successfully by the client.
 *
 * \note All Ixxx-0 error status are interim status. They should not be displayed in the error banner.
 * The Application can simply ignore Ixxx-0 in the error banner.
 */
#define ERR_MSG_I342        "I342-0  "

/**
 * Service Status Screen Text: Invalid change version.
 *
 * Invalid change version. This message is sent when an EMM related to Adverstisement has been handled unsuccessfully by the client.
 *
 * \note All Ixxx-0 error status are interim status. They should not be displayed in the error banner.
 * The Application can simply ignore Ixxx-0 in the error banner.
 */
#define ERR_MSG_I343        "I343-0  "

/**
 * Service Status Screen Text: Block Download EMM duplication.
 *
 * This Block Download EMM has been downloaded. This message is sent when an EMM related to Block Download has been download by the client.
 *
 * \note All Ixxx-0 error status are interim status. They should not be displayed in the error banner.
 * The Application can simply ignore Ixxx-0 in the error banner.
 */
#define ERR_MSG_I344        "I344-0  "

/**
 * Service Status Screen Text: No Secure Core is loaded.
 *
 * This message is sent when an EMM is rejected because no matching secure core has been loaded.
 *
 * \note All Ixxx-0 error status are interim status. They should not be displayed in the error banner.
 * The Application can simply ignore Ixxx-0 in the error banner.
 */
#define ERR_MSG_I345        "I345-0  "

/**
 * Service Status Screen Text: No valid entitlement found.
 *
 * Not Entitled. This message is sent when an ECM has been rejected by the client because the client is not entitled to use the VOD product the ECM refers to.
 */
#define ERR_MSG_E015        "E015-0  "

/**
 * Service Status Screen Text: No valid entitlement found.
 *
 * Not Entitled. This message is sent when an ECM has been rejected by the client because the client is not entitled
 * to use the product the ECM refers to.
 */
#define ERR_MSG_E016        "E016-0  "

/**
 * Service Status Screen Text: No valid sector found.
 *
 * Incorrect Sector. This message is sent when an ECM has been rejected by the client because the client does not have the
 * right sector the ECM refers to.
 */
#define ERR_MSG_E017        "E017-0  "

/**
 * Service Status Screen Text: Product blackout.
 *
 * Black out. This message is sent when an ECM has been rejected by the client due to a product blackout filter imposed by the Headend.
 */
#define ERR_MSG_E018        "E018-0  "

/**
 * Service Status Screen Text: P-Key mismatch.
 *
 * Incorrect PKey. This message is sent when an ECM has been rejected by the client because the client does not have the
 * right Pkey the ECM refers to.
 */
#define ERR_MSG_E030        "E030-0  "

/**
 * Service Status Screen Text: G-Key mismatch.
 *
 * Incorrect GKey. When an ECM has been rejected by the client because the client does not have the
 * right Gkey the ECM refers to. This message is not used currently.
 */
#define ERR_MSG_E031        "E031-0  "

/**
 * Service Status Screen Text: TG mismatch.
 *
 * Incorrect TG. This message is sent when an ECM has been rejected by the client because the client does not have the
 * right TG the ECM refers to.
 */
#define ERR_MSG_E032        "E032-0  "


/**
 * Service Status Screen Text: CWDK mismatch.
 *
 * Incorrect CWDK. This message is sent when an ECM has been rejected by the client because the client does not have the
 * right CWDK the ECM refers to.
 */
#define ERR_MSG_E033        "E033-0  "

/**
 * Service Status Screen Text:  Macrovision blocking enforced.
 *
 * The watching service is a Macrovision service,
 * but the client device doesn't support Macrovision currently.
 * If the application want the client device to support Macrovision,
 * the application must call ::UniversalClient_EnableMacrovision.
 * E055 will NOT be used from Cloaked CA Agent v2.1.0. It is still here only for compatibility consideration.
 */
#define ERR_MSG_E055        "E055-0  "


/**
 * Service Status Screen Text: Invalid PVR metadata.
 *
 * Invalid PVR Metadata. This message is sent when PVR playback has been rejected by the client because the submitted PVR metadata can not be processed.
 */
#define ERR_MSG_E094        "E094-0  "

/**
 * Service Status Screen Text: Incomplete Definition.
 *
 * CAT or PMT Absent. This message is sent when the service has been opened but no PMT or CAT is defined..
 * E101-0 is the default service status once the service has been opened.
 *
 * \note E101-0 and E102-0 have been replaced by I101-0 and I102-0 from Cloaked CA Agent v2.0.2. They are still here for compatibility consideration only.
 * E101-0 and E102-0 will NOT be used from Cloaked CA Agent v2.0.2.
 */
#define ERR_MSG_E101        "E101-0  "

/**
 * Service Status Screen Text: No ECM/EMM data received.
 *
 * No ECM/EMM Received.
 *
 * For EMM service, this message is sent when the service has been opened and CAT has been defined,
 * but NO EMM has been received.
 *
 * For ECM service, this message is sent when the service has been opened and PMT has been defined,
 * but NO ECM has been received.
 *
 * \note E101-0 and E102-0 have been replaced by I101-0 and I102-0 from Cloaked CA Agent v2.0.2. They are still here for compatibility consideration only.
 * E101-0 and E102-0 will NOT be used from Cloaked CA Agent v2.0.2.
 */
#define ERR_MSG_E102        "E102-0  "

/**
 * Service Status Screen Text: No valid ECM found.
 *
 * ECM Rejected. This message is sent when an ECM has been rejected by the client for a generic reason.
 * This means that a descrambling key will not be sent to the stream implementation for this ECM.
 */
#define ERR_MSG_E103        "E103-0  "

/**
 * Service Status Screen Text: Invalid EMM received.
 *
 * EMM Rejected. This message is sent when an EMM has been rejected by the client. This means that if the client was depending
 * on this EMM in order to decode an ECM, that decoding will not occur.
 */
#define ERR_MSG_E104        "E104-0  "

/**
 * Service Status Screen Text: No matching CA System ID in PMT.
 *
 * Incorrect CA System ID. This message is sent when no matched CA System ID found in PMT.
 */
#define ERR_MSG_E106        "E106-0  "

/**
 * Service Status Screen Text: No PVR Session Key to perform PVR operation.
 *
 * No PVR Session Key. This message is sent when a PVR record/playback request has been rejected by the client because the client can not get the PVR Session Key to perform PVR record/playback.
 */
#define ERR_MSG_E111        "E111-0  "

/**
 * Service Status Screen Text: Not entitled to perform PVR operation.
 *
 * PVR Not Entitled. This message is sent when a PVR Record/Playback Request has been rejected by the client because the client doesn't have
 * the entitlement to perform PVR record/playback.
 */
#define ERR_MSG_E128        "E128-0  "

/**
 * Service Status Screen Text: The PVR content is expired.
 *
 * The PVR content is expired, unable to view. This message is sent when a PVR playback request has been rejected by the client because the content is expired.
 */
#define ERR_MSG_E129        "E129-0  "

/**
 * Service Status Screen Text: PVR Record is not allowed.
 *
 * PVR Not Allowed. This message is sent when a PVR Record Request has been rejected by the client because PVR is not allowed
 * for this service.
 * \note This is not used after Cloaked CA Agent Rabbit 2.1.0.
 *
 */
#define ERR_MSG_E130        "E130-0  "

/**
 * Service Status Screen Text: No PVR Master Session Key to perform PVR operation.
 *
 * No PVR Master Session Key. This message is sent when a PVR record/playback request has been rejected by the client because the client doesn't have
 * PVR Master Session Key to perform PVR record/playback.
 */
#define ERR_MSG_E131        "E131-0  "


/**
 * Service Status Screen Text: No valid CA Regional Information found.
 *
 * CA Regional Information Absent. This message is sent when an ECM has been rejected by the client because the client does not have any
 * Regional Filtering Descriptor but Regional Filtering has been enabled via ::UniversalClient_EnableRegionalFiltering.
 */
#define ERR_MSG_E139        "E139-0  "

/**
 * Service Status Screen Text: Region or Sub-Region mismatch.
 *
 * Regional Blocking. This message is sent when an ECM has been rejected by the client because Regional Filtering is active and
 * either region or sub-region does not match.
 */
#define ERR_MSG_E140        "E140-0  "

/**
 * Service Status Screen Text: The Pre-Enablement product is expired.
 *
 * The Pre-Enablement product is expired.This message is sent when an ECM, that just for the Pre-Enablement service, has been rejected by the client because the Pre enablement product is expired.
 * If the client(not initialized) is watching a service protected by both normal product and Pre-Enablement product and the Pre-Enablement product has already expired, E017-0 will appear as the error banner which is triggered by the normal product.
 */
#define ERR_MSG_E150        "E150-0  "

/**
 * Service Status Screen Text: PESK mismatch.
 *
 * Incorrect PESK. This message is sent when an ECM, that just for the Pre-Enablement service, has been rejected by the client because of the incorrect PESK.
 */
#define ERR_MSG_E151        "E151-0  "

/**
 * Service Status Screen Text: FlexiFlash not initialized.
 *
 * This message is sent if the FlexiFlash is not initialized.
 */
#define ERR_MSG_E501        "E501-0  "

/**
 * Service Status Screen Text: Invalid CFG data.
 *
 * Invalid CFG data.This message is sent if the CFG data is invalid.
 */
#define ERR_MSG_E502        "E502-0  "

/**
 * Service Status Screen Text: Invalid CCA package data.
 *
 * This message is sent when an invalid CCA package is rejected to be loaded.
 */
#define ERR_MSG_E503        "E503-0  "

/**
 * Service Status Screen Text: CCA package not loaded.
 *
 * This message is sent when a required CCA package has not been loaded.
 */
#define ERR_MSG_E504        "E504-0  "

/**
 * Service Status Screen Text: Invalid CCA package index.
 *
 * This message is sent when an invalid CCA package index is found in EMMs.
 */
#define ERR_MSG_E505        "E505-0  "

/**
 * Service Status Screen Text: Duplicated VM ID.
 *
 * This message is sent when the VM instance is not created because the VM ID has already been used.
 */
#define ERR_MSG_E506        "E506-0  "

/**
 * Service Status Screen Text: Creat VM context failed.
 *
 * This message is sent when the VM context failed to be created.
 */
#define ERR_MSG_E507        "E507-0  "

/**
 * Service Status Screen Text: Creat VM failed.
 *
 * This message is sent when a VM instance failed to be created.
 */
#define ERR_MSG_E508        "E508-0  "

/**
 * Service Status Screen Text: Too many vm created.
 *
 * This message is sent when a VM instance is not created due to too many instances have been created.
 */
#define ERR_MSG_E509        "E509-0  "

/**
 * Service Status Screen Text: VM load image failed.
 *
 * This message is sent when the VM failed to load the CCA package.
 */
#define ERR_MSG_E510        "E510-0  "

/**
 * Service Status Screen Text: VM memmap failed.
 *
 * This message is sent when the VM failed to take the memory map operation.
 */
#define ERR_MSG_E511        "E511-0  "

/**
 * Service Status Screen Text: VM execution failed.
 *
 * This message is sent when the VM failed to execute.
 */
#define ERR_MSG_E512        "E512-0  "

/**
 * Service Status Screen Text: VM IO failed.
 *
 *  This message is sent when the VM failed to take IO operations.
 */
#define ERR_MSG_E513        "E513-0  "

/**
 * Service Status Screen Text: VM reset failed.
 *
 *  VM reset failed.This message is sent when VM not runing because the VM reset failed.
 */
#define ERR_MSG_E514        "E514-0  "

/**
 * Service Status Screen Text: Root key check failed.
 *
 *  The root key hash check failed when loading CCA package.
 */
#define ERR_MSG_E515        "E515-0  "

/**
 * Service Status Screen Text: Invalid package signature.
 *
 *  The package signature is invalid.
 */
#define ERR_MSG_E516        "E516-0  "

/**
 * Service Status Screen Text: Unsupported compress algorithm.
 *
 *  The compress algorithm is not supported.
 */
#define ERR_MSG_E517        "E517-0  "

/**
 * Service Status Screen Text: Invalid SYS ID.
 *
 *  The SYS ID is invalid.
 */
#define ERR_MSG_E518        "E518-0  "

/**
 * Service Status Screen Text: Client type mismatch.
 *
 *  The client type mismatch.
 */
#define ERR_MSG_E519        "E519-0  "

/**
 * Service Status Screen Text: The client version too low to support FlexiFlash download.
 *
 *  The client version too low.
 */
#define ERR_MSG_E520        "E520-0  "

/**
 * Service Status Screen Text: Invalid Variant.
 *
 *  The variant is invalid.
 */
#define ERR_MSG_E521        "E521-0  "

/**
 * Service Status Screen Text: Initialization has not been performed yet.
 *
 * Client Not Initialized. This message is sent when an ECM has been rejected by the client because the client has NOT been initialized,
 * i.e. the client currently doesn't have a valid unique address.
 */
#define ERR_MSG_E600        "E600-0  "


/**
 * Service Status Screen Text: Card In.
 *
 * Card In, card is a valid Irdeto Access Smartcard.
 */
#define ERR_MSG_D000_4         "D000-4  "

/**
 * Service Status Screen Text: Service is currently descrambled.
 *
 * The currently watched service is being descrambled successfully.
 */
#define ERR_MSG_D029_4         "D029-4  "

/**
 * Service Status Screen Text: DVB EMM Service Ok.
 *
 * Service Ok.
 */
#define ERR_MSG_D100_5        "D100-5  "

/**
 * Service Status Screen Text: Descrambler Service Ok.
 *
 * Service Ok.
 */
#define ERR_MSG_D100_8         "D100-8  "

/**
 * Service Status Screen Text: No Matched Vendor ID.
 *
 * No EMM PID could be found matching the Smartcard's vendor ID. No EMMs will be filtered.
 */
#define ERR_MSG_D101_6        "D101-6  "

/**
 * Service Status Screen Text: No Vendor ID.
 *
 * There is no vendor ID (card is not in and verified).
 */
#define ERR_MSG_D102_9         "D102-9  "

/**
 * Service Status Screen Text: FTA Service.
 *
 * This service is deemed to be free-to-air because there are no CA Descriptors in the PMT.
 */
#define ERR_MSG_D105_9         "D105-9  "

/**
 * Service Status Screen Text: Please insert the correct smartcard.
 *
 * This stream is scrambled by an alternate vendor with non-matching CA System ID.
 */
#define ERR_MSG_D106_9         "D106-9  "

/**
 * Service Status Screen Text: Checking Smartcard.
 *
 * Card verifying.
 */
#define ERR_MSG_I007_4         "I007-4  "

/**
 * Service Status Screen Text: Validating subscription, please wait.
 *
 * The scrambling status was not determined yet.
 */
#define ERR_MSG_I034_9         "I034-9  "

/**
 * Service Status Screen Text: Subscription update in progress.
 *
 * All products expired, still can continue descrambling until grace period ends.
 */
#define ERR_MSG_I055_4         "I055-4  "

/**
 * Service Status Screen Text: Currently unable to view this channel.
 *
 * The Smartcard returned a status that is not handled by SoftCell.
 */
#define ERR_MSG_I102_4         "I102-4  "

/**
  * Service Status Screen Text: Validating subscription, please wait.
  *
  * Waiting for Regional Filtering information.
  */
#define ERR_MSG_I139_4         "I139-4  "

/**
 * Service Status Screen Text: Service not scrambled.
 *
 * The currently watched service is not scrambled. No message will appear on the screen, the status will only be shown on the CA Status screen.
 */
#define ERR_MSG_E000_32        "E000-32 "

/**
 * Service Status Screen Text: Please insert Smartcard.
 *
 * Card Out, Please insert Smartcard.
 */
#define ERR_MSG_E004_4         "E004-4  "

/**
 * Service Status Screen Text: Unknown Smartcard.
 *
 * Unknown Card (non-Irdeto Access).
 */
#define ERR_MSG_E005_4         "E005-4  "

/**
 * Service Status Screen Text: Smartcard Failure.
 *
 * Card Error, communications with Smartcard have failed.
 */
#define ERR_MSG_E006_4         "E006-4  "

/**
 * Service Status Screen Text: No permission to view this channel.
 *
 * The viewer is not entitled to watch this service because the product in the ECM playout is not on the Smartcard.
 */
#define ERR_MSG_E016_4         "E016-4  "

/**
 * Service Status Screen Text: Service is currently scrambled.
 *
 * The viewer is not entitled to watch this service because the Smartcard is rejected by the Smartcard marriage function.
 */
#define ERR_MSG_E017_13        "E017-13 "

/**
 * Service Status Screen Text: This channel is not available in this location.
 *
 * The viewer is not entitled to watch this service due to a product blackout filter imposed by the Headend.
 */
#define ERR_MSG_E018_4        "E018-4  "

/**
 * Service Status Screen Text: The subscription to this channel has expired.
 *
 * The viewer is not entitled to watch this service because the product entitlement on the Smart Card has expired.
 */
#define ERR_MSG_E019_4        "E019-4  "

/**
 * Service Status Screen Text: Currently unable to view this channel.
 *
 * An authentication failure occurred during ECM submission by SoftCell to the Smartcard. This is due to a key mismatch between the playout and the Smartcard.
 */
#define ERR_MSG_E030_4         "E030-4  "

/**
 * Service Status Screen Text: Viewing is temporarily blocked, please stay tuned.
 *
 * The viewer is not entitled to watch this service because surf locking is active.
 */
#define ERR_MSG_E032_4         "E032-4  "

/**
 * Service Status Screen Text: Invalid Smartcard.
 *
 * Ci-Layer failure.
 */
#define ERR_MSG_E033_4         "E033-4  "

/**
 * Service Status Screen Text: Service is not currently running.
 *
 * There is no PMT for the service.
 */
#define ERR_MSG_E038_32         "E038-32 "

/**
 * Service Status Screen Text: Service not allowed.
 *
 * An attempt is made to tune to a service on a transport stream, which is blocked by the mux verifier.
 */
#define ERR_MSG_E045_32        "E045-32 "

/**
 * Service Status Screen Text: Currently unable to view this channel.
 *
 * No de-Tweak key received for Custom CA Yet.
 */
#define ERR_MSG_E080_35        "E080-35 "

/**
 * Service Status Screen Text: Currently unable to view this channel.
 *
 * De-Tweak key mismatch.
 */
#define ERR_MSG_E081_35        "E081-35 "

/**
 * Service Status Screen Text: Currently unable to view this channel.
 *
 * The viewer is not entitled to watch this service because the date code on the card has gone backwards.
 */
#define ERR_MSG_E100_4         "E100-4  "

/**
 * Service Status Screen Text: Currently unable to view this channel.
 *
 * The viewer is not entitled to watch this service because the sector number in the ECM playout is not on the Smartcard.
 */
#define ERR_MSG_E101_4         "E101-4  "

/**
 * Service Status Screen Text: Please insert the correct smartcard.
 *
 * This stream is scrambled by an alternate vendor with non-matching CA System ID.
 */
#define ERR_MSG_E106_9         "E106-9  "

/**
 * Service Status Screen Text: The smartcard is not fully authorized.
 *
 * The Smartcard and STB are not matched.
 */
#define ERR_MSG_E107_4         "E107-4  "

/**
 * Service Status Screen Text: Currently unable to view this channel.
 *
 * An authentication failure occurred during ECM submission by SoftCell to the Smartcard. This is due to Patch Filter fails.
 */
#define ERR_MSG_E108_4         "E108-4  "

/**
 * Service Status Screen Text: Currently unable to view this channel.
 *
 * No ECMs found that match the smartcard, with correct CCPversion. Or ECM rejected by the Smartcard, for incorrect CCPversion.
 */
#define ERR_MSG_E109_4         "E109-4  "

/**
 * Service Status Screen Text: Currently unable to view this channel.
 *
 * Ci-layer type indicator in ECM header is higher than current built up Ci-layer.
 */
#define ERR_MSG_E118_4         "E118-4  "

/**
 * Service Status Screen Text: Please wait, while the card is being synchronized.
 *
 * The card is synchronizing the timestamp with the system.
 */
#define ERR_MSG_E120_4         "E120-4  "

/**
 * Service Status Screen Text: Currently unable to view this channel.
 *
 * The viewer is not entitled to watch this service because the ECM timestamp expired.
 */
#define ERR_MSG_E133_4         "E133-4  "

/**
 * Service Status Screen Text: This channel is age restricted, insert the parental pin to view.
 *
 * The current event is Maturity Rating blocked. PIN for user profile of sufficient age needed to unblock.
 */
#define ERR_MSG_E136_4         "E136-4  "

/**
 * Service Status Screen Text: Currently unable to view this channel.
 *
 * The current event is Maturity Rating blocked. The MR profile ID on the card is not in the ECMs.
 */
#define ERR_MSG_E137_4         "E137-4  "

/**
  * Service Status Screen Text: This channel is not available in this location.
  *
  * Regional Blocking.
  */
#define ERR_MSG_E140_4         "E140-4  "

/**
 * Service Status Screen Text: Currently unable to view this channel.
 *
 * Home Gateway Proximity Control disallows the smartcard to be used for descrambling in the decoder.
 */
#define ERR_MSG_E141_4         "E141-4  "

/**
 * Service Status Screen Text: Insert the secondary card into the primary decoder to continue viewing with the secondary decoder.
 *
 * Home Gateway Proximity Control HNR timer expired on the smartcard. No descrambling.
 */
#define ERR_MSG_E142_4         "E142-4  "

/**
 * Service Status Screen Text: Wrong Home Network.
 *
 * This error banner is added in HGPC feature.
 */
#define ERR_MSG_E144_4         "E144-4  "

/**
 * Service Status Screen Text: Preview time period time out.
 *
 * The preview time reach the maximum limitation of this epoch.
 */
#define ERR_MSG_E152_4         "E152-4  "

/**
 * Service Status Screen Text: The device is invalid.
 *
 * Client Invalid. This message is sent when pull emm response has been rejected by the client because the client is invalid,
 * i.e. the client currently is not a legal one.
 */
#define ERR_MSG_E116        "E116-0  "

/**
 * Service Status Screen Text: Timeout to receive the request entitlement response.
 *
 * Timeout to receive the request entitlement response. This message is sent when the client cannot receive the expected request
 * entitlement response at a certain time interval.i.e. the client cannot receive the response within 10 seconds.
 */
#define ERR_MSG_E202        "E202-0  "

/**
 * Service Status Screen Text: Request entitlement has failed.
 *
 * Request entitlement has failed. This message is sent when the client request entitlement response has been rejected by the client,
 * i.e. the response message is not correct.
 */
#define ERR_MSG_E203        "E203-0  "

/**
 * Service Status Screen Text: CCIS internal error has happened.
 *
 * Request has failed. This message is sent when the client request entitlement reponse has been rejected by the client,
 * because internal errors occured within CCIS.
 */
#define ERR_MSG_E204        "E204-0  "

/**
 * Service Status Screen Text: DeviceID/UniqueAddress pair mismatches.
 *
 * DeviceID/UniqueAddress pair mismatches.This message is sent when pull emm response has been rejected by the client because
 * DeviceID/UniqueAddress pair of the client does NOT match the one in the head-end.
 */
#define ERR_MSG_E205        "E205-0  "

/* !! TODO new messages go here */

/** @} */ /* End of status_messages */

/** @defgroup servicetlvtags Cloaked CA Agent TLV Tags for Service
 *  List of all possible TLV (Tag, Length, Value) values to config service.
 *  These tags are used to carry specific parameters.
 *  Each tag is with different payload format and has its own meaning.
 *  TLV parameter is used for some advanced features.
 *  No need to call this API unless the application is using these advanced features.
 *  All TLV parameter have 1 byte tag, 2 bytes length, and N bytes value (payload).
 *
 *  \note Data fields in TLV parameter are all in big-endian (network) order.
 *  @{
 */


/**
 * Tag: PVR record.
 *
 * This tag is used to send a PVR Record request to Cloaked CA Agent.
 * When the application is trying to record a program, it must make a request to Cloaked CA Agent via this TLV.
 * If this TLV is processed successfully, Cloaked CA Agent will output a PVR session key via ::UniversalClientSPI_PVR_SetSessionKey,
 * together with one or more service messages of a PVR Session Metadata. The message type is uc_service_message_type::UC_SERVICE_PVR_SESSION_METADATA_REPLY.
 * The PVR session key should be used to encrypt the descrambled content. No matter success or not, Cloaked CA Agent will send out the status via
 * uc_service_message_type::UC_SERVICE_PVR_RECORD_STATUS_REPLY.
 * The TLV is in the following format:
 \code

 TLV_PVR_Record()
 {
     Field Name         Field Length        Field Value

     Tag                1  byte             UC_TLV_TAG_FOR_PVR_RECORD
     Length             2  bytes            0
 }
 \endcode
 */
#define UC_TLV_TAG_FOR_PVR_RECORD    (0x00)


/**
 * Tag: Stop PVR record.
 *
 * This tag is used to send a stop PVR record request to Cloaked CA Agent.
 * When the application is trying to stop a recording on a program, it must make a request to Cloaked CA Agent via this TLV.
 * If this TLV is processed successfully, Cloaked CA Agent will stop the recording.
 * The TLV is in the following format:
 \code

 TLV_PVR_Stop_Record()
 {
     Field Name         Field Length        Field Value

     Tag                1  byte             UC_TLV_TAG_FOR_STOP_PVR_RECORD
     Length             2  bytes            0
 }
 \endcode
 */
#define UC_TLV_TAG_FOR_STOP_PVR_RECORD    (0x01)


/**
 * Tag: ECM connection handle list for PVR record
 *
 * \note This tag is rarely used.
 * This tag is used to send the ECM connection handles with in the service to Cloaked CA Agent.
 * Cloaked CA Agent will use these handles to judge the entitlement of the PVR record.
 \code

 TLV_ECM_Connection_Handle_List()
 {
     Field Name         Field Length        Field Value

     Tag                1  byte             UC_TLV_TAG_FOR_ECM_CONNECTION_HANDLE_LIST
     Length             2  bytes            n*4
     Connection Handles n*4 bytes           n ECM Connection Handles
 }
 \endcode
 */
#define UC_TLV_TAG_FOR_ECM_CONNECTION_HANDLE_LIST  (0x02)


/**
 * Tag: Monitor Switch.
 *
 * This tag is used to send a request to Cloaked CA Agent to enable or disable the monitor
 * messages ::UC_SERVICE_ECM_MONITOR_STATUS, ::UC_SERVICE_EMM_MONITOR_STATUS,
 * ::UC_SERVICE_EMM_MONITOR_STATUS_FOR_CONVERGENT_CLIENT and ::UC_SERVICE_ECM_MONITOR_STATUS_FOR_CONVERGENT_CLIENT.
 * Monitor must be enabled upon displaying the Status Screen and disabled upon the exit of Status Screen.
 * The TLV is in the following format:
 \code

 TLV_Monitor_Switch()
 {
     Field Name         Field Length        Field Value

     Tag                1  byte             UC_TLV_TAG_FOR_MONITOR_SWITCH
     Length             2  bytes            0x01
     Switch Flag        1  byte             0x00/0x01 (0x00: Disable Monitor; 0x01: Enable Monitor)
 }
 \endcode
 */
#define UC_TLV_TAG_FOR_MONITOR_SWITCH (0x03)

/**
 * Tag: Trigger ECM reception
 *
 * This tag is used to send a request to Universal Client to trigger
 * an ECM section reception immediately for a service
 * to avoid the unnecessary waiting till next crypto period.
 * Each time after a jump operation (for example, step-forward while playing VOD content ),
 * the application should configure the service immediately
 * by making a request to Universal Client via this tag.
 * If this TLV is processed successfully, Universal Client will process
 * the first coming ECM section anyway and do not care
 * whether it is duplicated or new one compared with the previous ECM section.
 * The TLV is in the following format:
 \code

 TLV_TRIGGER_ECM_RECEPTION()
 {
     Field Name         Field Length        Field Value

     Tag                1  byte             UC_TLV_TAG_FOR_TRIGGER_ECM_RECEPTION
     Length             2  bytes            0
 }
 \endcode
 */
#define UC_TLV_TAG_FOR_TRIGGER_ECM_RECEPTION (0x04)

/**
 * Tag: ECM Filter Skip.
 *
 * This tag is used to request the Cloaked CA Agent to process all ECM sections and skip the ECM filters for a service.
 * Skip the filter means every ECM notified to Cloaked CA Agent is processed.
 * The TLV is in the following format:
 \code

 TLV_ECM_FILTER_SKIP()
 {
     Field Name         Field Length        Field Value

     Tag                1  byte             UC_TLV_TAG_FOR_ECM_FILTER_SKIP
     Length             2  bytes            0
 }
 \endcode

 *  \note Refer to \subpage basic_emm_pulling and \subpage basic_ott.

 */
#define UC_TLV_TAG_FOR_ECM_FILTER_SKIP (0x05)



/* !! TODO new tags for service on here */

/** @} */ /* End of servicetlvtags */


/** @defgroup vodtlvtags Cloaked CA Agent TLV Tags for VOD
 *  List of all possible TLV (Tag, Length, Value) values for VOD feature.
 *  These tags are used to carry specific parameters.
 *  Each tag is with different payload format and has its own meaning.
 *  TLV parameter is used for some advanced features.
 *  No need to call this API unless the application is using these advanced features.
 *  All TLV parameter have 1 byte tag, 2 bytes length, and N bytes value (payload).
 *  Data fields in TLV parameter are all in big-endian (network) order.
 *  @{
 */

/**
 * Tag: VOD Nonce.
 *
 * This tag is used to request a client transaction data for \ref glossary_ppv_vod "PPV VOD" feature from Cloaked CA Agent.
 * When the application is trying to request a VOD session via ::UniversalClient_PPV_VOD_PrepareRequest,
 * it must get a client transaction data include this TLV.
 * The TLV is in the following format:
 \code

 TLV_VOD_Nonce()
 {
     Field Name         Field Length        Field Value

     Tag                1  byte             UC_TAG_TYPE_VOD_NONCE
     Length             1  byte             16
     VODNonce           16 bytes            The 16 bytes hex data for the VOD nonce
 }
 \endcode
 */
#define UC_TAG_TYPE_VOD_NONCE           (0x00)

/**
 * Tag: Unique Address.
 *
 * This tag is used to request a client transaction data for \ref glossary_ppv_vod "PPV VOD" feature from Cloaked CA Agent.
 * When the application is trying to request a VOD session via ::UniversalClient_PPV_VOD_PrepareRequest,
 * it must get a client transaction data include this TLV.
 * The TLV is in the following format:

 \code

 TLV_Unique_Address()
 {
     Field Name         Field Length        Field Value

     Tag                1  byte             UC_TAG_TYPE_UNIQUE_ADDRESS
     Length             1  byte             4
     Unique Address     4  bytes            The 4 bytes hex data for Unique Address.
 }
 \endcode
 */
#define UC_TAG_TYPE_UNIQUE_ADDRESS      (0x01)

/**
 * Tag: Client Version.
 *
 * This tag is used to request a client transaction data for \ref glossary_ppv_vod "PPV VOD" feature from Cloaked CA Agent.
 * When the application is trying to request a VOD session via ::UniversalClient_PPV_VOD_PrepareRequest,
 * it must get a client transaction data include this TLV.
 * The TLV is in the following format:

 \code

 TLV_Client_Version()
 {
     Field Name         Field Length        Field Value

     Tag                1  byte             UC_TAG_TYPE_CLIENT_VERSION
     Length             1  byte             4
     ClientVersion      4  bytes            The 4 bytes hex data for client version,
                                            for more information, see following Client_Version_Format().
 }
 \endcode


 * The Client Version is in the following format:
 \code

 Client_Version_Format()
 {
     Field Name         Field Length        Field Description

     MAJOR (M)          4  bits             Major version number
     MINOR (m)          4  bits             Minor version number

     Release(R)         8  bits             Release number

     reserved(r)        4  bits             reserved 4 bits
     variant (v)        4  bits             Spatial Variant. Valid number is 1~10

     CG                 4  bits             Compound Generation
     SG                 4  bits             Stream Generation
 }
 \endcode

 In general,
 - \b MAJOR different major number have major functionality differences.
 - \b MINOR different minor number have minor functionality differences.
 - \b Release different release number include small bug-fixes.
 - \b reserved 4 bits for reserved.
 - \b variant different variant number have the same level of security but different implementations (EMMs are incompatible).
 - \b CG different CG have different security levels (ECMs and possibly also EMMs are incompatible).
 - \b SG different SG have different security levels (ECMs and possibly also EMMs are incompatible).
 */
#define UC_TAG_TYPE_CLIENT_VERSION      (0x02)

/* !! TODO new tags for VOD on here */

/** @} */ /* End of vodtlvtags */


/**
 * Base structure of component.
 *
 * This is the base structure of a component definition that is
 * passed to ::UniversalClientSPI_Stream_AddComponent and ::UniversalClientSPI_Stream_RemoveComponent.
 *
 * 'Components' are used to identify recipients of descrambler keys in a later call to ::UniversalClientSPI_SetDescramblerKey.
 * When this structure is actually used, it will be as part of another structure with additional information,
 * with the 'size' parameter indicating the total size of the larger structure.
 *
 * \note Structures based on this structure must not contain pointers to non-static memory, since the structure
 *     is copied and used asynchronously.
 *
 * For example:
 * \code
 * // structure that 'inherits' from uc_component
 * typedef struct _uc_mydescrambler_component
 * {
 *   uc_component base;
 *   int extra_field;
 *   int another_field;
 * } uc_mydescrambler_component;
 * \endcode
 *
 */
typedef struct _uc_component
{
    /**
     * size of structure based on uc_component.
     *
     * This is the size, in bytes, of the structure that 'inherits' from this structure.
     * This should be set to (sizeof(uc_component) + additional data).
     */
    uc_uint32 size;
} uc_component;

/**
 * Global message type enumeration.
 *
 * This type is used by the ::uc_global_message_proc callback provided by an application to
 * ::UniversalClient_StartCaClient. This indicates the type of message being sent to
 * the application. The application may choose to ignore messages, or
 * it may perform additional processing based on the type of the message.
 */
typedef enum
{
    /** (0x00000000) Debug message type for ::uc_global_message_proc callback.
     *
     * Debug messages are internal informational messages given to the application
     * in order to identify problems. They are provided in debug builds only-- a release
     * build will have all debug messages removed.
     *
     * For messages with this type, the \a lpVoid argument is a pointer to
     * the NULL-terminated text of the debug message.
     */
    UC_GLOBAL_DEBUG                         = 0x00000000,

     /**
     * (0x00000001) Raw IRD message received.
     *
     * This message is replaced by ::UC_GLOBAL_EXTENDED_RAW_IRD_MESSAGE from
     * Cloaked CA Agent 2.9.0. It is reserved for drop-in integartion for devices which
     * have integrated 2.8.x and lower.
     *
     * This command means that a raw IRD message was delivered to the
     * client that it did not understand. The actual execution of this command is
     * implementation-specific. This command may be used to send custom
     * commands to the client, like, text/mail/decoder control messages.
     *
     * Please refer to 705410 CCP IRD Messages for more detail information.
     *
     * For messages with this type, the \a lpVoid argument points to a ::uc_buffer_st
     * structure. uc_buffer_st::bytes points to a buffer containing the raw message,
     * uc_buffer_st::length is the length of the message.
     */
    UC_GLOBAL_RAW_IRD_MESSAGE               = 0x00000001,

    /**
     * (0x00000002) Serial Number Changed.
     *
     * This command means that a CAM EMM message was delivered to the client that
     * cause the client to update the device Unique Address (Serial Number).
     *
     * For messages with this type, the \a lpVoid argument points to a ::uc_buffer_st
     * structure. uc_buffer_st::bytes points to a buffer containing new serial number,
     * uc_buffer_st::length is the length of the serial number (currently defined as 4 bytes).
     */
    UC_GLOBAL_SERIAL_NUMBER_CHANGED         = 0x00000002,

    /**
     * (0x00000003) Status in reply to an OOB EMM.
     *
     * This is the message that indicates the result of OOB EMM processing.
     *
     * For messages with this type, the \a lpVoid argument points to
     * a buffer containing an error code status string.
     * See \ref status_messages "Status Messages" for a list of
     * possible values and what they mean.
     */
    UC_GLOBAL_OOB_EMM_STATUS_REPLY          = 0x00000003,

    /**
     * (0x00000004) Entitlement is changed.
     *
     * This is the message that indicates the entitlement change.
     *
     * For messages with this type, the \a lpVoid argument is NULL.
     * The application should use ::UniversalClient_GetProductList to check the changes.
     *
     */
    UC_GLOBAL_NOTIFY_ENTITLEMENT_CHANGE     = 0x00000004,

    /**
     * (0x00000005) ECM/EMM counter changed.
     *
     * This is the message indicates count of ECM sections or EMM sections processed by Cloaked CA Agent.
     *
     * For messages with this type, the \a lpVoid argument points to ::uc_ca_section_count structure.
     * The format of ::uc_ca_section_count is as follows:
     *
     \code
      typedef struct _uc_ca_section_count
      {
        uc_uint32 ecm_count;
        uc_uint32 emm_count;
      }uc_ca_section_count;
     \endcode

        Explanation of the fields:
        - \b ecm_count: Count of ECM sections that have been processed.
        - \b emm_count: Count of EMM sections that have been processed.
     *
     * \note In order to save the system cost, the message will NOT be notified to the Application any more since Cloaked CA Agent v2.2.0.
     * The application can still use ::UniversalClient_GetEcmEmmCount to get the latest ECM/EMM section count.
     */
    UC_GLOBAL_NOTIFY_CASECTION_COUNT        = 0x00000005,

    /**
     * (0x00000006) Region information changed.
     *
     * This is the message that notifies the latest region and subregion for a specific sector.
     *
     * For messages with this type, the \a lpVoid argument points to ::uc_region_information structure.
     *
     */
    UC_GLOBAL_NOTIFY_REGION_SUBREGION_INFO  = 0x00000006,

    /**
     * (0x00000007) TMS data changed.
     *
     * This is the message that indicates the TMS data change.
     *
     * For messages with this type, the \a lpVoid argument is NULL.
     * The application should use ::UniversalClient_GetTmsData to get the new
     * TMS data.
     *
     */
    UC_GLOBAL_NOTIFY_TMS_DATA_CHANGED       = 0x00000007,

    /**
     * (0x00000008) Nationality is changed.
     *
     * This is the message that indicates the nationality change.
     *
     * For messages with this type, the \a lpVoid argument is NULL
     * The application should use ::UniversalClient_GetNationality to check the change.
     *
     */
    UC_GLOBAL_NOTIFY_NATIONALITY_CHANGED     = 0x00000008,

    /**
     * (0x00000009) Homing Channel data is changed.
     *
     * This is the message that indicates the Homing Channel data change.
     *
     * For messages with this type, the \a lpVoid argument is NULL
     * The application should use ::UniversalClient_GetHomingChannelData to check the change.
     *
     */
    UC_GLOBAL_NOTIFY_HOMING_CHANNEL_DATA_CHANGED     = 0x00000009,

    /**
     * (0x0000000A) Middleware user data is changed.
     *
     * This is the message that indicates the Middleware user data change.
     *
     * For messages with this type, the \a lpVoid argument is NULL
     * The application should use ::UniversalClient_GetMiddlewareUserData to check the change.
     */
    UC_GLOBAL_NOTIFY_MIDDLEWARE_USER_DATA_CHANGED     = 0x0000000A,

    /**
     * (0x0000000B) Reset to factory state.
     *
     * This is the message that notifies device application to reset the device to factory state.
     * When Cloaked CA Agent receives the reset to factory EMM from head-end, the CA data in
     * persistent storage is cleared, then this message is sent to device application. The device
     * application shall erase necessary data to restore the device to factory state.
     *
     * For messages with this type, the \a lpVoid argument point to uc_uint32 caSystemID.
     * \note For CCA Agent version 2.8.x and lower, this \a lpVoid is NULL.
     *
     * \note ::UniversalClient_ResetCAData shall not be called, as the CA data has already been cleared.
     */
    UC_GLOBAL_NOTIFY_RESET_TO_FACTORY     = 0x0000000B,

    /**
     * (0x0000000C) CA data is cleared.
     *
     * This is the message that notifies device application that the CA data has already been cleared by
     * Cloaked CA Agent.
     * This message is just a notification and it is sent in two cases: When Cloaked CA Agent
     * receives a reset to factory EMM from head-end or ::UniversalClient_ResetCAData is called
     * by the device application.
     *
     * For messages with this type, the \a lpVoid argument is NULL
     * \note Device application doesn't need to perform any actions on this message.
     */
    UC_GLOBAL_NOTIFY_CA_DATA_CLEARED     = 0x0000000C,

    /**
     * (0x0000000D) The Smartcard Status changed.
     *
     * This is the message that notifies the change of the smartcard status if smartcard has been activated.
     *
     * For messages with this type, the \a lpVoid argument points to ::uc_smartcard_status structure.
     */
    UC_GLOBAL_NOTIFY_SMARTCARD_STATUS    = 0x0000000D,

    /**
     * (0x0000000E) The Smartcard nationality changed.
     * This is the message that notifies the change of the smartcard nationality.
     *
     * For messages with this type, the \a lpVoid argument points to NULL.
     * The application should use ::UniversalClient_GetSmartcardNationality to check the change.
     */
    UC_GLOBAL_NOTIFY_SMARTCARD_NATIONALITY_CHANGED = 0x0000000E,

     /**
     * (0x0000000F) The valid client changed.
     *
     * This is the message that notifies the change of the valid client when the CAT updated or the card inserted or pulled out.
     *
     * For messages with this type, the \a lpVoid argument points to ::uc_valid_client_type value.
     */
    UC_GLOBAL_NOTIFY_VALID_CLIENT_CHANGED     = 0x0000000F,

     /**
     * (0x00000010) Smartcard Raw IRD message received.
     *
     * This command means that a Smartcard raw IRD message was delivered to the
     * client that it did not understand. The actual execution of this command is
     * implementation-specific. This command may be used to send custom
     * commands to the client, like, text/mail/decoder control messages.
     *
     * Please refer to 705410 CCP IRD Messages for more detail information.
     *
     * For messages with this type, the \a lpVoid argument points to a ::uc_buffer_st
     * structure. uc_buffer_st::bytes points to a buffer containing the raw message,
     * uc_buffer_st::length is the length of the message.
     */
    UC_GLOBAL_SMARTCARD_RAW_IRD_MESSAGE     = 0x00000010,

    /**
     * (0x00000011) FlexiFlash information changed.
     *
     * This is the message indicates information of FlexiFlash changed. The information contains the status of FlexiFlash
     * and the progress of CCA package downloaded, either the two items foresaid is changed, this message type will be triggered by Agent.
     *
     * For messages with this type, the \a lpVoid argument points to ::uc_flexiflash_msg value.
     */
    UC_GLOBAL_NOTIFY_FLEXIFLASH_MESSAGE        = 0x00000011,

    /**
    * (0x00000012) Extended Raw IRD message received.
    *
    * This message is added from Cloaked CA Agent 2.9.0. It is a replace version of
    * ::UC_GLOBAL_RAW_IRD_MESSAGE. For devices integrated with version 2.9.0 or higher,
    * ::UC_GLOBAL_EXTENDED_RAW_IRD_MESSAGE shall be used.
    *
    * This command means that a raw IRD message from specifc operator was delivered to the
    * client that it did not understand. The actual execution of this command is
    * implementation-specific. This command may be used to send custom
    * commands to the client, like, text/mail/decoder control messages.
    *
    * Please refer to 705410 CCP IRD Messages for more detail information.
    *
    * For messages with this type, the \a lpVoid argument points to a ::uc_raw_ird_msg
    * structure. uc_raw_ird_msg::caSystemID identifies which operator sends the message,
    * uc_raw_ird_msg::rawIrdMsg points to a buffer containing the raw message,
    * uc_raw_ird_msg::length is the length of the message.
    */
    UC_GLOBAL_EXTENDED_RAW_IRD_MESSAGE         = 0x00000012
} uc_global_message_type;

/**
 * Byte buffer structure
 *
 * This structure is used for passing arbitrary blocks of memory to the Cloaked CA Agent API.
 *
 * The caller sets the \a bytes member to point to a
 * buffer, and sets the \a length member to be the length of the buffer.
 *
 * When used as an output parameter, the function being called will copy data up to
 * the length of the buffer into the memory specified by \a bytes. When used as an
 * input parameter, the function leaves the contents of the buffer alone.
 */
typedef struct _uc_buffer_st
{
    /**
     * Pointer to a valid region of memory. Depending on the operation, this
     * may be a block of memory to write into or to read from.
     */
    uc_byte *bytes;

    /**
     * Length of the buffer. Depending on the operation, the number of bytes to
     * read to or write from the \a bytes member.
     */
    uc_uint32 length;
} uc_buffer_st;

/**
* ::uc_bytes now is redefined to ::uc_buffer_st in order to remove the confusion between ::uc_bytes and ::uc_byte.
* The existing integration shall not be affected.
*/
#define uc_bytes uc_buffer_st

/**
 * simple boolean type.
 */
#define uc_bool int

/**
 * The false value of ::uc_bool
 */
#define UC_FALSE 0

/**
 * The true value of ::uc_bool
 */
#define UC_TRUE 1

/**
 * Handle to a connection object
 *
 * A 'connection' is a logical construct that represents the point at which CA
 * functionality is connected to a stream implementation. The connection object
 * interacts with a stream implementation to set filters (::uc_filter_handle)
 * and receive ECMs and EMMs. After processing ECMs, keys are passed
 * to the stream implementation for descrambling the content.
 *
 * A connection handle is passed to an instance of a stream via ::UniversalClientSPI_Stream_Connect.
 *
 */
typedef uc_uintptr uc_connection_handle;


/**
 * Function pointer type used to notify the client of new ECM or EMM sections.
 *
 * A function pointer of this type is passed to the implementation of a stream via the ::UniversalClientSPI_Stream_Connect
 * method. The stream implementation is expected to deliver new ECM or EMM sections that match the currently active set
 * of filters (see ::UniversalClientSPI_Stream_OpenFilter) to this function,
 * after a call to ::UniversalClientSPI_Stream_Start and until a call to ::UniversalClientSPI_Stream_Disconnect occurs.
 *
 * \note Implementations that receive one section at a time can safely pass the data directly, whereas implementations that receive blocks of sections
 *     can also safely pass the data directly without having to parse the sections and break it into individual section.
 *
 * @param[in] connectionHandle The value of the connection handle previously passed to the stream implementation's
 *     ::UniversalClientSPI_Stream_Connect method.
 * @param[in] pSections Containing the MPEG-2 sections to process.
 *     The uc_buffer_st::bytes member must point to a buffer containing the raw data of
 *     the sections, and the uc_buffer_st::length member must be set to the number of bytes
 *     in the sections.
 */
typedef void (*uc_notify_callback)(
    uc_connection_handle connectionHandle,
    const uc_buffer_st * pSections);

/**
 * Handle to a filter object
 *
 * A filter handle is an implementation specific value that is allocated by the implementation
 * of a stream object when the ::UniversalClientSPI_Stream_OpenFilter method is called on a connected stream.
 * While the filter is active, the stream implementation is expected to deliver ECM or EMM sections
 * that arrive on the stream that match the filter criteria.
 */
typedef uc_uintptr uc_filter_handle;

/**
 * Filter definition
 *
 * This structure contains the definition of a filter for ECM and EMM sections
 * that are sent to the client for processing. This structure is passed to
 * ::UniversalClientSPI_Stream_OpenFilter to notify a stream implementation of the pattern
 * of sections it should deliver on the connection.
 *
 * When a section arrives, the driver should match the first filterDepth
 * bytes of the section against the filter pattern.
 * The filter pattern is defined as a 'mask', which specifies which bits are relevant
 * for the comparison, and a 'match', which specifies the value the corresponding bits
 * should have in order to have successfully matched the pattern.
 *
 * Here is some sample code for performing the comparison to illustrate how it is used:
 *
 * \code
 *
 * uc_filter *pFilter = ...; // filter definition previously passed to ::UniversalClientSPI_Stream_OpenFilter.
 * uc_byte *pSection = ...; // contents of MPEG-2 section read from hardware
 *
 * uc_bool bMatch = UC_TRUE;
 *
 * for (i=0; bMatch && i<pFilter->filterDepth; ++i)
 * {
 *     if ((pSection[i] & pFilter->mask[i]) != pFilter->match[i])
 *     {
 *         // section does NOT match
 *         bMatch = UC_FALSE;
 *     }
 * }
 *
 * // ... at this point, use bMatch to determine if section should be passed to the
 * // ::uc_notify_callback function.
 * \endcode
 *
 * \note Integrators should be aware that filtering is an optional step intended for performance enhancement
 *     of the client and improved battery life for the device. Implementations that do not have access to
 *     hardware-level filtering can safely ignore filters and pass on all sections received, and the client
 *     will still function correctly.
 */
typedef struct _uc_filter {
    /**
     * Specifies which bits are important for the pattern match.
     *
     * This points to a buffer whose length is determined by the \a filterDepth member.
     */
    uc_byte * mask;

    /**
     * Specifies which bits to compare the masked bits against.
     *
     * This points to a buffer whose length is determined by the \a filterDepth member.
     */
    uc_byte * match;

    /**
     * Specifies the number of bytes in the \a mask and \a match buffers
     *
     * A filterDepth of 0 means "match all sections".
     */
    uc_uint32 filterDepth;
} uc_filter;


/**
 * Handle to a stream object
 *
 * A stream handle is an implementation-specific type that is returned from
 * a call to ::UniversalClientSPI_Stream_Open.
 * A stream object is a logical construct that represents a source of ECMs or EMMs,
 * and a destination for the corresponding descrambling keys.
 */
typedef uc_uintptr uc_stream_handle;

/**
 * Connection or SPI stream type
 *
 * This type is used to indicate which type stream that will be opened when ::UniversalClientSPI_Stream_Open is called.
 * The following are the stream types:\n
 * ::UC_CONNECTION_STREAM_TYPE_EMM \n
 * ::UC_CONNECTION_STREAM_TYPE_ECM \n
 * ::UC_CONNECTION_STREAM_TYPE_PVR_RECORD \n
 * ::UC_CONNECTION_STREAM_TYPE_PVR_PLAYBACK \n
 * ::UC_CONNECTION_STREAM_TYPE_FTA \n
 * ::UC_CONNECTION_STREAM_TYPE_IP \n
 * ::UC_CONNECTION_STREAM_TYPE_RESERVED
 */
typedef uc_sint32 uc_connection_stream_type;


/** @defgroup streamtype Connection and SPI Stream Type
 *  The SPI stream type is defined, and all of the types are listed. The stream type is
 *  also used as the Connection Type inside Cloaked CA Agent.
 *  @{
 */

/**
 * EMM connection or SPI stream type
 */
#define UC_CONNECTION_STREAM_TYPE_EMM 0

/**
 * ECM connection or SPI stream type
 */
#define UC_CONNECTION_STREAM_TYPE_ECM 1

/**
 * PVR record connection or SPI stream type
 */
#define UC_CONNECTION_STREAM_TYPE_PVR_RECORD 2

/**
 * PVR playback connection or SPI stream type
 */
#define UC_CONNECTION_STREAM_TYPE_PVR_PLAYBACK 3

/**
 * FTA connection or SPI stream type
 */
#define UC_CONNECTION_STREAM_TYPE_FTA 4

/**
 * IP connection or stream type
 */
#define UC_CONNECTION_STREAM_TYPE_IP 5

/**
 * Reserved connection or stream type
 */
#define UC_CONNECTION_STREAM_TYPE_RESERVED 6



/** @} */ /* End of stream types */


/**
 * Product type enumeration.
 *
 * This type is used by the ::UniversalClient_GetProductList to obtain the detailed product type.
 */
typedef enum
{
    /** (0x00000000) Normal Product.
      */
    UC_PRODUCT_TYPE_NORMAL          = 0x00000000,

     /**
      * (0x00000001) PVR Product.
      */
    UC_PRODUCT_TYPE_PVR             = 0x00000001,

    /**
      * (0x00000002) Push VOD Product.
      */
    UC_PRODUCT_TYPE_PUSH_VOD        = 0x00000002,

    /**
      * (0x00000003) Pre Enablement Product.
      */
    UC_PRODUCT_TYPE_PRE_ENABLEMENT  = 0x00000003

} uc_global_product_type;

/**
 * Product status structure
 *
 * This is the structure of each element in the list returned by a call to ::UniversalClient_GetProductList.
 * A 'product' is a unit of entitlement that gives the client the ability to descramble a service.
 * A service may be associated with one or more products. The head end is aware of this association
 * when it scrambles a service, but communicating this association to the device is outside of the scope of the
 * Cloaked CA Agent, so it us up to the application layer to determine the service that is associated
 * with a given product, using the product ID from inside this structure.
 *
 */
typedef struct _uc_product_status
{
    /**
     * Sector number that contains this product
     *
     * Some Cloaked CA Agents may support separate stores for client entitlements called 'sectors'.
     * The use of sectors is operator-dependent.
     */
    uc_byte sector_number;

    /**
     * Entitlement state of the product
     *
     * This value indicates whether the client is allowed to use this product or not.
     * If this value is true, the client is entitled and can descramble any services that are associated
     * with this product. If this value is false, the client is not entitled and cannot descramble these
     * services.
     */
    uc_bool entitled;

    /**
     * Unique product identifier
     *
     * This value uniquely identifies a product and is the big-endian representation
     * of the 'ID' field for the corresponding product configuration in the CA server.
     * For example, if the product ID in the head end is 1000 decimal,
     * the product ID in this structure will be {0x03, 0xe8}.
     * See ::UC_SIZE_PRODUCT_ID for its size.
     */
    uc_byte product_id[UC_SIZE_PRODUCT_ID];

    /**
     * The date from which the entitlement is valid.
     *
     * The date is a sequential counter that indicates the number of days elapsed since 1-1-2000
     * and is increased by 1 date code tick every midnight UTC.
     * startingDate must be converted to have a year/month/day to show on the screen.
     * \note Please see \ref dateconversionfunction "Date Conversion Function" for sample function to convert starting date for display purpose.
     */
    uc_uint16 startingDate;

    /**
     * How long the product entitlement is valid.
     *
     * The unit of duration is DAY.
     */
    uc_byte duration;

    /**
     * Product Type.
     */
    uc_global_product_type productType;

    /**
     * CA System ID
     */
    uc_uint32 caSystemID;
}uc_product_status;

/**
 * ECM/EMM Counter structure
 *
 * This is the structure containing count of CA sections that have been processed by Cloaked CA Agent.
 * This structure is used by ::UC_GLOBAL_NOTIFY_CASECTION_COUNT and ::UniversalClient_GetEcmEmmCount.
 *
 */
typedef struct _uc_ca_section_count
{
    /**
     * Count of ECM sections that have been processed
     *
     */
    uc_uint32 ecm_count;

    /**
     * Count of EMM sections that have been processed
     *
     */
    uc_uint32 emm_count;

}uc_ca_section_count;

/**
 * Region information structure
 *
 * This is the structure containing the latest region information for a specific sector.
 * This structure is used by message ::UC_GLOBAL_NOTIFY_REGION_SUBREGION_INFO and ::UniversalClient_GetRegionInfoList.
 *
 */

typedef struct _uc_region_information
{
    /**
     * Sector number containing this region information.
     *
     */
    uc_byte sector_number;

    /**
     * Region information for this sector.
     *
     */
    uc_byte region;

    /**
     * Subregion information for this sector.
     *
     */
    uc_byte subregion;

    /**
     * Ca system ID for this sector.
     *
     */
    uc_uint32 caSystemID;

}uc_region_information;

/**
 * Descrambling key algorithm type
 *
 * This type is part of the ::uc_key_info structure and is passed to ::UniversalClientSPI_Stream_SetDescramblingKey
 * after an ECM arrives on a connected stream and is processed by the client.
 * It refers to the algorithm that the descrambler should use to descrambler the content using the descrambling key.
 */
typedef enum
{
    /**
     * (0) Unknown or proprietary algorithm.
     *
     * A descrambling key may arrive with this type if a proprietary scrambling solution is being used
     * that the client does not know about. In this case, it is the responsibility of the stream implementation
     * to select what descrambling algorithm to use.
     */
    UC_DK_ALGORITHM_UNKNOWN =0,

    /**
     * (1) DVB Common Scrambling Algorithm
     *
     * A descrambling key may arrive with this type when the scrambling solution is known to be
     * DVB-CSA. The stream implementation may use this information as it sees fit.
     */
    UC_DK_ALGORITHM_DVB_CSA  =1,

    /**
     * (2) Advanced Encryption Standard in Reverse Cipher Block Chaining mode, 128 bit key.
     *
     * A descrambling key may arrive with this type when the scrambling solution is known to be
     * AES-128 RCBC. The stream implementation may use this information as it sees fit.
     */
    UC_DK_ALGORITHM_AES_128_RCBC = 2,

    /**
     * (3) Advanced Encryption Standard in Cipher Block Chaining mode, 128 bit key.
     *
     * A descrambling key may arrive with this type when the scrambling solution is known to be
     * AES-128 CBC. The stream implementation may use this information as it sees fit.
     */
    UC_DK_ALGORITHM_AES_128_CBC = 3

} uc_dk_algorithm;

/**
 * Up to Cloaked CA Agent release 2.2.1, the enum is JUST used to indicate descrambling key protection mechanism.
 * This type is part of the ::uc_key_info structure and is passed to ::UniversalClientSPI_Stream_SetDescramblingKey.
 * after an ECM arrives on a connected stream and is processed by the client.
 * It refers to the method used to protect the descrambling key in transit.
 *
 * After 2.2.1 released, Cloaked CA Agent expect to support different key ladder providing additional key protection
 * choice,especially the AES algorithm.
 * Hence,the enum is not only used in descrambling key protection but also in the protection of CSSK and PVRSK.
 *
 * For CSSK protection, this type is part of the ::uc_cssk_info structure and is pased to ::UniversalClientSPI_Device_SetExtendedCSSK
 * after an Pairring EMM arrives on a connected stream and is processed by client.
 * The type also pased to client through UniversalClient_SetExtendedTestingKey as a part of ::uc_cssk_info structure,
 * when client is in Testing mode.
 * It refers to the method used to protect the CSSK in transit.
 *
 * For PVRSK protection, this type is part of the ::uc_pvrsk_info structure and is pased to ::UniversalClientSPI_PVR_SetExtendedSessionKey,
 * It refers to the method used to protect the PVRSK in transit.
 *
 * For descrambing key protection, the method of using the enum has not changed
 */
typedef enum
{
    /**
     * (0) Unknown or proprietary protection algorithm
     *
     * Up to Cloaked CA Agent release 2.2.1, this protection type means that the descrambling key is being sent to the
     * descrambler using a method that the client has no knowledge of.
     * In this case, it is the responsibility of the stream implementation to select which protection
     * algorithm to use.
     *
     * After 2.2.1 released,this protection type means key is protected by a unknown algorithm.
     */
    UC_DK_PROTECTION_UNKNOWN =0,

    /**
     * (1) Triple-DES encrypted
     *
     * Up to Cloaked CA Agent release 2.2.1,this protection type means that the descrambling key is triple-DES encrypted
     * with the hardware or descrambler-specific session key. Descrambling keys are typically delivered with this type
     * to ensure that they cannot be shared with another instance of a descrambler.
     * This is intended to protect against a 'control word sharing' attack on the CA system.
     *
     * After 2.2.1 released,this protection type means key is protected by triple-DES algorithm.
     */
    UC_DK_PROTECTION_TDES =1,

    /**
     * (2) Not encrypted
     *
     * Up to Cloaked CA Agent release 2.2.1,this protection type means that the descrambling key is not encrypted and can be
     * directly used to descramble content. Descrambling keys are delivered with this
     * type when a service is set in \b 911 \b mode or current client is with Security ID anchor.
     * \b 911 \b mode is a special mode that a service
     * can be placed in to bypass the regular CA system during emergencies or
     * other appropriate situations.
     *
     * After 2.2.1 released,this protection type means key is not encrypted.
     */
     UC_DK_PROTECTION_CLEAR =2,

    /**
     * (3) AES encrypted
     *
     * The enum value is added after 2.2.1 released,this protection type means key is protected by AES algorithm.
     * This is necessary for the AES key ladder supporting.
     */
     UC_DK_PROTECTION_AES =3
} uc_dk_protection;


/**
 * Key info structure
 *
 * This is a structure that is passed to a stream implementation from
 * a call to ::UniversalClientSPI_Stream_SetDescramblingKey. It contains information about
 * a particular descrambling key.
 */
typedef struct _uc_key_info
{
    /**
     * The scrambling algorithm that this key will be used for.
     *
     * Typically a descrambler will only support one type of algorithm, so this data
     * is really used for validation purposes in practice. However, it can also be
     * used to select which algorithm to use when multiple algorithms are available.
     */
    uc_dk_algorithm descramblingKeyAlgorithm;

    /**
     * The protection method used to secure this key.
     *
     * For example, this may be used to tie a descrambling key to a unique instance
     * of a descrambler hardware to prevent the key from being useful in another
     * descrambling environment.
     */
    uc_dk_protection descramblingKeyProtection;

    /**
     * The key material of the descrambling key.
     *
     * This contains the raw data of the key. How this key is protected is determined by the
     * uc_key_info::descramblingKeyProtection member, and how the key will be used by the
     * descrambler is determined by the uc_key_info::descramblingKeyAlgorithm member.
     * The uc_buffer_st::bytes member points to the raw data of the key, and
     * the uc_buffer_st::length member is the number of bytes in the key.
     */
    uc_buffer_st *pDescramblingKey;
} uc_key_info;

/**
 * Indicates the type of key ladder.
 *
 * This type is used by ::uc_cssk_info to indicate the destination key ladder that CSSK should be set to.
 */
typedef enum _uc_dk_ladder
{
    /**
     * The CW key ladder.
     */
    UC_DK_LADDER_CW = 0,
    /**
     * The PVR key ladder.
     */
    UC_DK_LADDER_PVR = 1
} uc_dk_ladder;

/**
 *
 * CSSK Key info structure
 *
 * The structure is added after release 2.2.1.
 * The introduction of this structure is to support different key ladder (AES key ladder, TDES key ladder; CW key ladder, PVR key ladder).
 * It contains: 1, Information about the method to protect CSSK; 2, Destination where CSSK should be set to.
 * The structure is used in these place:
 * 1, Pased to ::UniversalClientSPI_Device_SetExtendedCSSK.
 * 2, When client is in testing mode, pased to client through API ::UniversalClient_SetExtendedTestingKey.
 */
typedef struct _uc_cssk_info
{
    /**
     * The protection method used to secure CSSK.
     */
    uc_dk_protection KeyProtection;

    /**
     * The destination key ladder below CSSK should be set to.
     */
    uc_dk_ladder keyLadder;

    /**
     * The key material of the CSSK
     */
    uc_buffer_st *pCSSK;
} uc_cssk_info;


/**
 *
 * PVRSK Key info structure
 *
 * The structure is added after release 2.2.1.
 * The introduction of this structure is to support different key ladder (AES key ladder, TDES key ladder).
 * It contains information about the method to protect PVRSK.
 * The structure is Pased to ::UniversalClientSPI_PVR_SetSessionKey, indicate the PVRSK protection method.
 */

typedef struct _uc_pvrsk_info
{
    /**
     * The protection method used to secure PVRSK.
     */
    uc_dk_protection KeyProtection;

    /**
     * The key material of the PVRSK
     */
    uc_buffer_st *pPVRSK;
}uc_pvrsk_info;


/**
 * Smartcard status structure
 *
 * This is the structure containing smartcard status information.
 * This structure is used by ::UniversalClient_GetSmartcardStatus.
 */
typedef struct _uc_smartcard_status
{
    /**
    * Smartcard id.
    * This is not used.
    */
    uc_uint32 smartcardId;

    /**
    * Status string.
    */
    uc_char status[UC_STATUS_LENGTH];

} uc_smartcard_status;


/**
 * Smartcard info structure
 *
 * This is the structure containing smartcard information.
 * This structure is used by ::UniversalClient_GetSmartcardInfo.
 */
typedef struct _uc_smartcard_info
{
    /**
    * Smartcard id. This is not used.
    */
    uc_uint32 smartcardId;

    /**
    * Smartcard serial number string.
    */
    uc_char serialNumber[UC_MAX_SMARTCARD_SERIAL_NUMBER_LENTH];

    /**
    * Smartcard type.
    */
    uc_uint16 smartcardType;

    /**
    * Major version.
    */
    uc_byte majorVersion;

    /**
    * Minor version.
    */
    uc_byte minorVersion;

    /**
    * Build infomation.
    */
    uc_byte build;

    /**
    * Variant.
    */
    uc_byte variant;

    /**
    * Patch level.
    */
    uc_uint16 patchLevel;

    /**
    * Owner id.
    */
    uc_uint16 ownerId;
} uc_smartcard_info;

/**
 * DVB tuned information structure
 *
 * This is the structure containing the latest tuned information for current playing program.
 * This structure is used by ::UniversalClient_SetTunedInfoList.
 */
typedef struct _uc_dvb_tuned_info
{
    /**
     * Service ID.
     */
    uc_uint16 serviceID;

    /**
     * Transport ID.
     */
    uc_uint16 transportID;

    /**
     * Network ID.
     */
    uc_uint16 networkID;
}uc_dvb_tuned_info;

/**
 * DVB tuned information structure
 *
 * This is the structure containing the latest tuned information for current playing program.
 * This structure is used by ::UniversalClient_SetTMSAttributes.
 */
typedef struct _uc_tms_attribute_item_info
{
    /**
     * start position of the attribute in 32-bytes attributes bitmap.
     */
    uc_uint8 startbit;

    /**
     * length of current attribute length.
     */
    uc_uint8 length;

    /**
     * attribute value.
     */
    uc_uint32 value;
}uc_tms_attribute_item_info;

/**
 * Indicate the valid CA client.
 *
 * When Unified Client is integrated, the valid CA client may be changed because of the CAT update
 * or the card in/out. This type is used by ::UC_GLOBAL_NOTIFY_VALID_CLIENT_CHANGED to indicate the active CA client.
 *
 */
typedef enum _uc_valid_client_type
{
    /**
     * No valid Irdeto CAS
     *
     */
    UC_CLIENT_UNDEFINED = 0,

    /**
     * Smartcard CA Client valid
     *
     */
    UC_SOFTCELL_VALID = 1,

    /**
     * Cloaked CA Client valid
     *
     */
    UC_CLOAKEDCA_VALID = 2,

    /**
     * Both Cloaked CA Client and Smartcard CA Client are valid
     *
     */
    UC_BOTH_VALID = 3

}uc_valid_client_type;

/**
 * FlexiFlash message structure
 *
 * This is the structure containing the status of the secure core and the progress of CCA package download.
 * This structure is used by the message ::UC_GLOBAL_NOTIFY_FLEXIFLASH_MESSAGE when FlexiFlash is enabled.
 */
typedef struct _uc_flexiflash_msg
{
    /**
     * Status of the secure core
     *
     * This is a string that indicates status of secure core.
     * The status will be defined as below:\n
     * "(I%02d) %s M%02d m%02d N%02d C%02x S%02x V%02x T%02x "\n
     * The following is an example:\n
     * "(I01) D500-0   M02 m06 N00 C01 S00 V01 T00 "\n
     * When a secure core was loaded unsuccessfully, CCA Agent will cannot obtain version information of the secure core,
     * so will use "xx" to substitute the version information.\n
     * The following is an example:\n
     * "(I01) D500-0   Mxx mxx Nxx Cxx Sxx Vxx Txx "\n
     *
     * \note
     *      I: Area Index\n
     *      %s: string of error code, 8-byte length such as "D500-0  ", "E504-0  "\n
     *      M: Major\n
     *      m: Minor\n
     *      N: Build Number\n
     *      C: CG\n
     *      S: SG\n
     *      V: Variant\n
     *      T: Anchor Type\n
     *
     */
    uc_string secureCoreListStatus;

    /**
     * Status of the CCA package downloaded
     *
     * When a CCA package downloading is triggered, this string indicates the progress of the downloading CCA package.
     * The status will be defined as below:\n
     * "(I%02d) %3d%% M%02d m%02d N%02d C%02x S%02x "\n
     * The following is an example:\n
     * "(I31)  80% M02 m06 N00 C01 S00 "\n
     * When no CCA package download is triggered, this string is empty.
     *
     * \note
     *      I: Area Index\n
     *      %3d: percentage of downloaded progress\n
     *      M: Major\n
     *      m: Minor\n
     *      N: Build Number\n
     *      C: CG\n
     *      S: SG\n
     *
     */
    uc_string packagesDownloadProgressInfo;
}uc_flexiflash_msg;

/**
 * TMS data per operator.
 */
typedef struct _uc_tms_data_per_operator
{
    /**
     * identify the operator by its CA System ID
     */
    uc_uint32 caSystemID;

    /**
     * detail tms data for the operator which is identified by caSystemID
     */
    uc_byte tmsData[UC_TMS_USER_DATA_SIZE];
} uc_tms_data_per_operator;

/**
 * TMS data for Mutil-Secure core.
 *
 * This structure is passed to ::UniversalClient_Extended_GetTmsData
 */
typedef struct _uc_tms_data
{
    /**
     * the number of current valid operators
     */
    uc_uint32 validOperatorCount;

    /**
     * tms data for specific operator
     * The number of operators is identified by validOperatorCount.
     */
    uc_tms_data_per_operator tms[UC_MAX_OPERATOR_COUNT];
} uc_tms_data;

/**
 * regional info list per operator
 */
typedef struct _uc_regional_info_list_per_operator
{
    /**
     * the number of valid sectors
     */
    uc_uint32 validSectorCount;

    /**
     * the region and subregion info for the sector.
     * The number of sector is identified by validSectorCount.
     */
    uc_region_information regionInfoListPerOperator[MAX_ACTIVE_SECTOR_COUNT_PER_OPERATOR];
} uc_regional_info_list_per_operator;

/**
 * regional info list for multiple operators
 *
 * This structure is passed to ::UniversalClient_Extended_GetRegionInfoList
 */
typedef struct _uc_regional_info_list
{
    /**
     * the number of current valid operators
     */
    uc_uint32 validOperatorCount;

    /**
     * region and subregion information for specific operator
     * The number of operators is identified by validOperatorCount.
     */
    uc_regional_info_list_per_operator regionalInfoList[UC_MAX_OPERATOR_COUNT];
} uc_regional_info_list;

/**
 * version string for each secure core. The secure core is for one valid operator.
 */
typedef struct _uc_single_securecore_version
{
    /**
     * identify the operator by its CA System ID
     */
    uc_uint32 caSystemID;

    /**
     * detail version string for the secure core which is identified by the caSystemID.
     */
    uc_byte versionString[UC_MAX_VERSION_STRING_LENGTH];
} uc_single_securecore_version;

/**
 * secure core version string for multiple operators
 *
 * This structure is passed to ::UniversalClient_Extended_GetSecurecoreVersion
 */
typedef struct _uc_securecore_version
{
    /**
     * the number of current valid operators
     */
    uc_uint32 validOperatorCount;

    /**
     * securecore version for specific operator
     * The number of operators is identified by validOperatorCount.
     */
    uc_single_securecore_version version[UC_MAX_OPERATOR_COUNT];
} uc_securecore_version;

/**
 * serial number per operator
 */
typedef struct _uc_serial_number_per_operator
{
    /**
     * identify the operator by its CA System ID
     */
    uc_uint32 caSystemID;

    /**
     * detail serial number for the operator which is identified by the caSystemID.
     */
    uc_byte serialNumberBytes[UC_UNIQUE_ADDRESS_SIZE];
} uc_serial_number_per_operator;

/**
 * serial number information for multiple operators
 * This structure is passed to ::UniversalClient_Extended_GetSerialNumber
 */
typedef struct _uc_serial_number
{
    /**
     * the number of current valid operators
     */
    uc_uint32 validOperatorCount;

    /**
     * serial number for specific operator
     * The number of operators is identified by validOperatorCount
     */
    uc_serial_number_per_operator serialNumber[UC_MAX_OPERATOR_COUNT];
} uc_serial_number;

/**
 * client ID per operator
 */
typedef struct _uc_client_id_per_operator
{
    /**
     * identify the operator by its CA System ID
     */
    uc_uint32 caSystemID;

    /**
     * detail client ID string for the operator which is identified by the caSystemID
     */
    uc_byte clientIDString[UC_MAX_ITEM_LENGH];
} uc_client_id_per_operator;

/**
 * client ID for multiple operators
 *
 * This strucure is passed to ::UniversalClient_Extended_GetClientIDString
 */
typedef struct _uc_client_id
{
    /**
     * the number of current valid operators
     */
    uc_uint32 validOperatorCount;

    /**
     * client ID for specific operator
     * The number of operators is identified by validOperatorCount.
     */
    uc_client_id_per_operator clientID[UC_MAX_OPERATOR_COUNT];
} uc_client_id;

/**
 * CA System ID for multiple operators
 *
 * This structure is passed to ::UniversalClient_Extended_ResetCAData
 */
typedef struct _uc_ca_system_id
{
    /**
     * the number of current valid operators
     *
     */
    uc_uint32 validOperatorCount;

    /**
     * the CA System IDs for valid operators.
     * The number of CA System ID is identified by validOperatorCount.
     *
     */
    uc_uint32 caSystemID[UC_MAX_OPERATOR_COUNT];
} uc_ca_system_id;

/**
 * operator information for single operator
 */
typedef struct _uc_single_operator
{
    /**
     * if the operator exists in real network
     */
    uc_bool active;

    /**
     * the valid caSystemID matches real network
     */
    uc_uint32 activeCaSystemID;

    /**
     * start value of the operator range
     */
    uc_uint32 caSystemIDStart;

    /**
     * end value of the operator range
     */
    uc_uint32 caSystemIDEnd;

    /**
     * operator name. A NULL-termined string.
     */
    uc_byte operatorName[UC_MAX_OPERATOR_NAME_LENGTH];
} uc_single_operator;

/**
 * operator information for multiple operators
 *
 * This structure is passed to ::UniversalClient_Extended_GetOperatorInfo
 */
typedef struct _uc_operator
{
    /**
     * the number of available operators defined in cfg file
     */
    uc_uint32 validOperatorCount;

    /**
     * operator information for specific operator
     * The number of supported operators is identified by validOperatorCount.
     */
    uc_single_operator operatorInfo[UC_MAX_OPERATOR_COUNT];
} uc_operator;

/**
 * nationality per operator
 */
typedef struct _uc_nationality_per_operator
{
    /**
     * identify the operator by its CA System ID
     */
    uc_uint32 caSystemID;

    /**
     * detail nationality for the operator which is identified by the caSystemID
     */
    uc_byte nationalityData[UC_NATIONALITY_SIZE];
} uc_nationality_per_operator;

/**
 * nationality for multiple operators
 *
 * This structure is passed to ::UniversalClient_Extended_GetNationality
 */
typedef struct _uc_nationality
{
    /**
     * the number of current valid operators
     */
    uc_uint32 validOperatorCount;

    /**
     * nationality for specific operator
     * The number of operators is identified by validOperatorCount.
     */
    uc_nationality_per_operator nationality[UC_MAX_OPERATOR_COUNT];
} uc_nationality;

/**
 * homing channel data per operator
 */
typedef struct _uc_homing_channel_per_operator
{
    /**
     * identify the operator by its CA System ID
     */
    uc_uint32 caSystemID;

    /**
     * detail homing channel data for the operator which is identified by the caSystemID
     */
    uc_byte homingChannelData[UC_HOMING_CHANNEL_USER_DATA_SIZE];
} uc_homing_channel_per_operator;

/**
 * homing channel data for multiple operators
 *
 * This structure is passed to ::UniversalClient_Extended_GetHomingChannelData
 */
typedef struct _uc_homing_channel
{
    /**
     * the number of current valid operators
     */
    uc_uint32 validOperatorCount;

    /**
     * homing channel information for specific operator
     * The number of operators is identified by validOperatorCount.
     */
    uc_homing_channel_per_operator homingChannel[UC_MAX_OPERATOR_COUNT];
} uc_homing_channel;

/**
 * middleware user data per operator
 */
typedef struct _uc_middleware_user_data_per_operator
{
    /**
     * identify the operator by its CA System ID
     */
    uc_uint32 caSystemID;

    /**
     * detail middleware user data for the operator which is identified by the caSystemID
     */
    uc_byte middlewareUserData[UC_MW_DATA_USER_DATA_SIZE];
} uc_middleware_user_data_per_operator;

/**
 * middleware user data for multiple operators
 *
 * This structure is passed to ::UniversalClient_Extended_GetMiddlewareUserData
 */
typedef struct _uc_middleware_user_data
{
    /**
     * the number of current valid operators
     */
    uc_uint32 validOperatorCount;

    /**
     * middleware user data for specific operator
     * The number of operators is identified by validOperatorCount.
     */
    uc_middleware_user_data_per_operator middlewareUserData[UC_MAX_OPERATOR_COUNT];
} uc_middleware_user_data;

/**
 * ecm & emm count per operator
 */
typedef struct _uc_ca_section_count_per_operator
{
    /**
     * identify the operator by its CA System ID
     */
    uc_uint32 caSystemID;

    /**
     * ecm count for the operator which is identified by caSystemID
     */
    uc_uint32 ecm_count;

    /**
     * emm count for the operator which is identified by caSystemID
     */
    uc_uint32 emm_count;
} uc_ca_section_count_per_operator;

/**
 * ecm and emm count for multiple operators
 *
 * This structure is passed to ::UniversalClient_Extended_GetEcmEmmCount
 */
typedef struct _uc_ca_extended_section_count
{
    /**
     * the number of current valid operators
     */
    uc_uint32 validOperatorCount;

    /**
     * section count for specific operator
     * The number of operators is identified by validOperatorCount
     */
    uc_ca_section_count_per_operator caSectionCount[UC_MAX_OPERATOR_COUNT];
} uc_ca_extended_section_count;

/**
 * Extended raw ird message structure
 *
 * This structure is extended for supporting multiple operators by identifying ca system id
 * for the operator who sends the raw ird message.
 *
 * This structure is used by global message UC_GLOBAL_EXTENDED_RAW_IRD_MESSAGE.
 */
typedef struct _uc_raw_ird_msg
{
    /**
     * identify the operator by its CA System ID
     */
    uc_uint32 caSystemID;

    /**
     * the length of raw IRD message
     */
    uc_uint32 length;

    /**
     * raw IRD message
     */
    uc_byte *rawIrdMsg;
} uc_raw_ird_msg;

/**
 * Callback called to notify the application of a global event.
 *
 * An application can implement a callback function of this type and pass it to
 * ::UniversalClient_StartCaClient.  It will then receive notifications from the client
 * when global events occur that are not associated with a particular object.
 *
 * Because this callback may occur in the context of a client thread,
 * an application should take care to queue this message for later processing and return immediately
 * from the function. In particular, attempting to call back into any UniversalClient_XXX methods
 * from the same thread as the callback may result in undefined behavior.
 *
 * When processing, the application can do anything it wants with the message. Examples: log it to file,
 * display it onscreen, display a web page, or ignore it.
 * However, the callback implementation must be sure to make copies of any of data referenced by the pointers
 * passed into it that it wants to use outside of the context of this function, as the pointers become invalid
 * as soon as this function returns.
 *
 * @param[in] message Message ID for this message. See the definition of ::uc_global_message_type for a description
 *     of the different types.
 * @param[in] lpVoid Message defined pointer. The value of this depends on the message type. See ::uc_global_message_type
 *     for details.
 *
 * <b>Example:</b>
 * \code
void MyGlobalMessageProc(
    uc_global_message_type message,
    void* lpVoid)
{
    switch(message)
    {
        case UC_GLOBAL_DEBUG:
        {
            uc_char *pText = (uc_char *)lpVoid;
            HxLOG_Print("Debug output: %s\n", pText);
            break;
        }
        default:
        {
            HxLOG_Print("Unknown message type.\n");
            break;
        }
    }
}

void main()
{
    UniversalClient_StartCaClient(MyGlobalMessageProc);
}

 * \endcode
 */
typedef void (*uc_global_message_proc)(
    uc_global_message_type message,
    void* lpVoid);

/** @defgroup universalclient_api Cloaked CA Agent APIs

 *  Cloaked CA Agent APIs
 *
 *  These API methods provide the necessary interfaces for the Application in client device.
 *
 *  @{
 */


/** @defgroup universalclient Cloaked CA Agent Base APIs
 *  Cloaked CA Agent Base APIs
 *
 *  These API methods provide the necessary basic functionality for querying information about
 *  the client, gathering product information, and controlling regional filtering.
 *
 *  @{
 */

/**
 * Obtain the current version of the Cloaked CA Agent
 *
 * This function can be called to obtain the current version of the Cloaked CA Agent. This version can be used by applications to
 * check for supported features and application compatibility.  An application might also display a status screen with
 * version information on it that can be used for technical support purposes.
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[in,out] pVersion Receives the version string associated with the client.
 *     The application must set the uc_buffer_st::bytes member to point to a buffer in memory, and the uc_buffer_st::length member
 *     to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer to contain the version
 *     string, including NULL terminator, and set the uc_buffer_st::length member to the actual length of data returned.
 *     If the uc_buffer_st::length member is not large enough to hold the entire version string, the uc_buffer_st::length member
 *     will be set to the minimum required amount, and the function will return ::UC_ERROR_INSUFFICIENT_BUFFER. The application
 *     can then allocate memory of a sufficient size and attempt to call the function again.
 *
 *     \note For simplicity, the application can allocate ::UC_BUFFER_GENERIC_SIZE bytes memory for uc_buffer_st::bytes
 *       and set uc_buffer_st::length to ::UC_BUFFER_GENERIC_SIZE. Cloaked CA Agent will set uc_buffer_st::length to the actual size,
 *       and the length includes 1 byte NULL terminator.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_GetVersion(
        uc_buffer_st *pVersion);

/**
 * Obtain the build information of the Cloaked CA Agent
 *
 * This function can be called to obtain the build information of the Cloaked CA Agent.
 * The Application shall display the build info in the client status screen.
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[in,out] pBuildInformation Receives the build string associated with the client.
 *     The application must set the uc_buffer_st::bytes member to point to a buffer in memory, and the uc_buffer_st::length member
 *     to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer to contain the build
 *     string, including NULL terminator, and set the uc_buffer_st::length member to the actual length of data returned.
 *     If the uc_buffer_st::length member is not large enough to hold the entire build string, the uc_buffer_st::length member
 *     will be set to the minimum required amount, and the function will return ::UC_ERROR_INSUFFICIENT_BUFFER. The application
 *     can then allocate memory of a sufficient size and attempt to call the function again.
 *
 *     \note For simplicity, the application can allocate ::UC_BUFFER_GENERIC_SIZE bytes memory for uc_buffer_st::bytes
 *       and set uc_buffer_st::length to ::UC_BUFFER_GENERIC_SIZE. Cloaked CA Agent will set uc_buffer_st::length to the actual size,
 *       and the length includes 1 byte NULL terminator.
 *
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_GetBuildInformation(
        uc_buffer_st *pBuildInformation);

/**
 * Obtain a description for functionalities provided by Cloaked CA Agent
 *
 * This function can to called to obtain a description for the functionalities provided by Cloaked CA Agent.
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[in,out] pCapabilities Receives the string of functionalities description associated with the client.
 *     On output, the uc_buffer_st::bytes member will be filled with a NULL-terminated string describing the functionalities.
 *
 *     The application must set the uc_buffer_st::bytes member to point to a buffer in memory, and the uc_buffer_st::length member
 *     to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer to contain the capabilities
 *     string, including NULL terminator, and set the uc_buffer_st::length member to the actual length of data returned.
 *     If the uc_buffer_st::length member is not large enough to hold the entire string of functionalities description, the uc_buffer_st::length member
 *     will be set to the minimum required amount, and the function will return ::UC_ERROR_INSUFFICIENT_BUFFER. The application
 *     can then allocate memory of a sufficient size and attempt to call the function again.
 *
 *     \note For simplicity, the application can allocate ::UC_BUFFER_GENERIC_SIZE bytes memory for uc_buffer_st::bytes
 *       and set uc_buffer_st::length to ::UC_BUFFER_GENERIC_SIZE. Cloaked CA Agent will set uc_buffer_st::length to the actual size,
 *       and the length includes 1 byte NULL terminator.
 *
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_GetCapabilities(
        uc_buffer_st *pCapabilities);

/**
 * Obtain the secure type of Cloaked CA Agent
 *
 * This function can be called to obtain the secure type of Cloaked CA Agent.
 * If the output value is 0, the secure type is "Secure Chipset".
 * If the output value is 1, the secure type is "Security ID".
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[out] pSecureType Gets the current secure type of Cloaked CA Agent.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_GetSecureType(
        uc_byte *pSecureType);

/**
 * Obtain the current LOCK ID in Cloaked CA Agent
 *
 * This function can be called to obtain the LOCK ID embeded in the Cloaked CA Agent.
 * A LOCK ID is a value assigned to a manufacturer.
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[out] pLockId Receives the current LOCK ID in Cloaked CA Agent.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_GetLockId(
        uc_uint16 *pLockId);

/**
 * Obtain the current Serial Number of the Cloaked CA Agent
 *
 * This API is obsolete from Cloaked CA Agent 2.9.0. To get serial number,
 * please refer to ::UniversalClient_Extended_GetSerialNumber.
 *
 * This function can be used to obtain the Serial Number of the Cloaked CA Agent.  This serial
 * number is assigned by the headend system, and is used to uniquely address a particular device
 * when sending \ref glossary_emm EMMs to it.
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * \note This serial number MAY BE DIFFERENT from the Chipset Serial Number (CSSN) that is returned
 * from ::UniversalClientSPI_Device_GetCSSN.
 *
 * @param[in,out] pSN Receives the serial number assigned to the client.
 *     The application must set the uc_buffer_st::bytes member to point to a buffer in memory, and the uc_buffer_st::length
 *     member to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer
 *     to contain the serial number in binary big-endian format, without any terminating character, and set the
 *     uc_buffer_st::length member to the actual length of data returned.
 *     If the uc_buffer_st::length member is not large
 *     enough to hold the entire serial number, the uc_buffer_st::length member will be set to the minimum required
 *     amount, and the function will return ::UC_ERROR_INSUFFICIENT_BUFFER. The application can then allocate
 *     memory of a sufficient size and attempt to call the function again.
 *
 *     \note For simplicity, the application can allocate ::UC_BUFFER_GENERIC_SIZE bytes memory for uc_buffer_st::bytes
 *       and set uc_buffer_st::length to ::UC_BUFFER_GENERIC_SIZE. Cloaked CA Agent will set uc_buffer_st::length to the actual size.
 *
 *     \n
 *     \note
 *     Suppose the Serial Number is 2864434397 i.e. 0xAABBCCDD, uc_buffer_st::bytes will have the following values: \n
 *     uc_buffer_st::bytes[0] = 0xAA, \n
 *     uc_buffer_st::bytes[1] = 0xBB, \n
 *     uc_buffer_st::bytes[2] = 0xCC, \n
 *     uc_buffer_st::bytes[3] = 0xDD. \n
 *     and uc_buffer_st::length is set to 4.
 *     \n

 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_GetSerialNumber(
        uc_buffer_st *pSN);


/**
 * Obtain the current Secure Chipset Serial Number
 *
 * This function can be called to obtain the secure chipset serial number. This secure chipset
 * serial number is associated with the device.
 *
 * @param[in,out] pCSSN Receives the CSSN assigned to the client.
 *     The application must set the uc_buffer_st::bytes member to point to a buffer in memory, and the uc_buffer_st::length
 *     member to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer
 *     to contain the CSSN in binary big-endian format, without any terminating character, and set the
 *     uc_buffer_st::length member to the actual length of data returned. If the uc_buffer_st::length member is not large
 *     enough to hold the entire CSSN, the uc_buffer_st::length member will be set to the minimum required
 *     amount, and the function will return ::UC_ERROR_INSUFFICIENT_BUFFER. The application can then allocate
 *     memory of a sufficient size and attempt to call the function again.
 *
 *     \note For simplicity, the application can allocate ::UC_BUFFER_GENERIC_SIZE bytes memory for uc_buffer_st::bytes
 *       and set uc_buffer_st::length to ::UC_BUFFER_GENERIC_SIZE. Cloaked CA Agent will set uc_buffer_st::length to the actual size.
 *     \n
 *     \note
 *     Suppose the CSSN is 4293844428 i.e. 0xFFEEDDCC, uc_buffer_st::bytes will have the following values: \n
 *     uc_buffer_st::bytes[0] = 0xFF, \n
 *     uc_buffer_st::bytes[1] = 0xEE, \n
 *     uc_buffer_st::bytes[2] = 0xDD, \n
 *     uc_buffer_st::bytes[3] = 0xCC. \n
 *     and uc_buffer_st::length is set to 4.
 *     \n
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
  * @retval
 *    ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_GetCSSN(
        uc_buffer_st *pCSSN);

/**
 * Obtain a string to describe the ID of current client.
 *
 * This API is obsolete from Cloaked CA Agent 2.9.0. To get client ID,
 * please refer to ::UniversalClient_Extended_GetClientIDString.
 *
 * This function can be called to obtain the client ID string.
 * The client ID string is supposed to be shown in the error banner, so that the viewer can read the identifiers directly from the error banner.
 * If current client has been initialized with a unique address, the string will look like "xxxxxxxxxx(y)". xxxxxxxxxx will be the unique address in decimal format.
 * If current client is a virgin one, the string will look like "xxxxxxxxxx(y)-z". xxxxxxxxxx will be CSSN in decimal format and z will be the Lock ID in decimal format.
 * y is the check digit for xxxxxxxxxx.\n
 * An example string containing a unique address:0000004097(2) \n
 * An example string containing a CSSN and Lock ID:0000004097(2)-1 \n
 *
 * \note The information from this method must be displayed in the error banner dialog.
 *
 * @param[in,out] pClientIDString Receives the Client ID string assigned to the client.
 *     The application must set the uc_buffer_st::bytes member to point to a buffer in memory, and the uc_buffer_st::length
 *     member to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer
 *     to contain the Client ID string, including NULL terminator, and set the uc_buffer_st::length member to the actual length of data returned.
 *     If the uc_buffer_st::length member is not large enough to hold the entire Client ID string, the uc_buffer_st::length member will be set to the minimum required
 *     amount, and the function will return ::UC_ERROR_INSUFFICIENT_BUFFER. The application can then allocate
 *     memory of a sufficient size and attempt to call the function again.
 *
 *     \note For simplicity, the application can allocate ::UC_BUFFER_GENERIC_SIZE bytes memory for uc_buffer_st::bytes
 *       and set uc_buffer_st::length to ::UC_BUFFER_GENERIC_SIZE. Cloaked CA Agent will set uc_buffer_st::length to the actual size,
 *       and this length include 1 byte terminator.
 *
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_GetClientIDString(uc_buffer_st *pClientIDString);


/**
 * Obtain the current CA System ID
 *
 * This API is obsolete from Cloaked CA Agent 2.9.0. To get operator infomation such as CA System ID,
 * please refer to ::UniversalClient_Extended_GetOperatorInfo.
 *
 * This method can be called to obtain the current CA System ID. This value is typically used to
 * select the correct Irdeto CA stream when multiple streams exist from different CA vendors.
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[out] pCASystemID Receives the current CA System ID. When Cloaked CA Agent support a range
 * of CA System IDs, the output of this method may vary: If Cloaked CA Agent has chosen a valid ID in
 * the range as its active ID, it returns the chosen ID, otherwise, it returns the start ID in its range.
 * If Cloaked CA Agent only supports a single ID, the result is the single one. See ::UniversalClient_GetCASystemIDRange
 * for getting the range of CA System IDs.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_GetCASystemID(
        uc_uint32 * pCASystemID);

/**
 * Obtain the CA System ID range
 *
 * This API is obsolete from Cloaked CA Agent 2.9.0. To get operator infomation such as CA System ID range,
 * please refer to ::UniversalClient_Extended_GetOperatorInfo.
 *
 * This method can be called to obtain the valid CA System ID range if Cloaked CA Agent supports
 * a range of CA System IDs. A valid CA System ID should be: (start ID <= valid ID <= end ID). If only
 * a single CA System ID is supported, this can be omitted.
 *
 *
 * @param[out] pCASystemIDStart Receives the start value of the CA System ID range.
 * @param[out] pCASystemIDEnd Receives the end value of the CA System ID range.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_GetCASystemIDRange(
    uc_uint32 *pCASystemIDStart,
    uc_uint32 *pCASystemIDEnd);

/**
 * Obtain the Nationality
 *
 * This API is obsolete from Cloaked CA Agent 2.9.0. To get nationality,
 * please refer to ::UniversalClient_Extended_GetNationality.
 *
 * This function can be called to obtain the nationality of the Cloaked CA Agent.
 * The nationality is described by three ISO 3166-1 Alpha-3 identifier (e.g. 'CHN', 'GBR', 'USA').
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[in,out] pNationality Receives the nationality associated with the client.
 *     On output, the uc_buffer_st::bytes member will be filled with a NULL-terminated string.
 *
 *     The application must set the uc_buffer_st::bytes member to point to a buffer in memory, and the uc_buffer_st::length member
 *     to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer to contain the nationality
 *     , including NULL terminator, and set the uc_buffer_st::length member to the actual length of data returned.
 *     If the uc_buffer_st::length member is not large enough to hold the entire nationality, the uc_buffer_st::length member
 *     will be set to the minimum required amount, and the function will return ::UC_ERROR_INSUFFICIENT_BUFFER.
 *
 *     \note For simplicity, the application can allocate ::UC_BUFFER_GENERIC_SIZE bytes memory for uc_buffer_st::bytes
 *       and set uc_buffer_st::length to ::UC_BUFFER_GENERIC_SIZE. Cloaked CA Agent will set uc_buffer_st::length to the actual size.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_NO_NATIONALITY
 * @retval
 *    ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_GetNationality(
        uc_buffer_st *pNationality);


/**
* Obtain Homing Channel data
*
* This API is obsolete from Cloaked CA Agent 2.9.0. To get homing channel data,
* please refer to ::UniversalClient_Extended_GetHomingChannelData.
*
* This function can be called to obtain Homing Channel data.
*
*
* @param[in,out] pHomingChannelData Receives Homing Channel data.
*     The application must set the uc_buffer_st::bytes member to point to a buffer in memory, and the uc_buffer_st::length member
*     to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer to contain the Homing Channel data,
*     in binary big-endian format, without any terminating character and set the uc_buffer_st::length member to the actual length of data returned.
*     If the uc_buffer_st::length member is not large enough to hold the entire Homing Channel data, the uc_buffer_st::length member
*     will be set to the minimum required amount, and the function will return ::UC_ERROR_INSUFFICIENT_BUFFER. The application
*     can then allocate memory of a sufficient size and attempt to call the function again.
*
*     \note For simplicity, the application can allocate ::UC_BUFFER_GENERIC_SIZE bytes memory for uc_buffer_st::bytes
*       and set uc_buffer_st::length to ::UC_BUFFER_GENERIC_SIZE. Cloaked CA Agent will set uc_buffer_st::length to the actual size.
*
*     @code
*     Format of the data(The bit order is bslbf):
*
*     Field Name      Length(bits)
*     DTH_IRD         1
*     ProfDec_IRD     2
*     Spare           4
*     Monitoring_IRD  1
*     Homing_Channel  96
*     Spare           152
*
*     In the above strucure, Homing_Channel is defined as:
*
*     Field Name            Length(bits)
*     Frequency             32
*     Orbital_Position      16
*     Symbol_Rate           28
*     Reserved              4
*     MuxVerifier_Mode      4
*     FEC_Inner             4
*     West_East_Flag        1
*     Polarisation          2
*     Roll off              2
*     Modulation System     1
*     Modulation Type       2
*
*     The detailed bit format of the above fields in Home_Channel structure is:
*
*     Field Name          Value                     Description
*     Frequency           DD D.D DD DD              where D = BCD digit
*     Orbital Pos         DD D.D                    where D = BCD digit
*     Symbol Rate         DD D.D DD D               where D = BCD digit
*     Mux Verifier        0                         Mux No Allowed
*                         1                         Mux Early
*                         2                         Mux Late
*                         3                         Mux Middle
*     FEC lnner           1                         1/2
*                         2                         2/3
*                         3                         3/4
*                         4                         5/6
*                         5                         7/8
*     West/East           0                         West
*                         1                         East
*     Polarisation        0                         Lin Horizontal
*                         1                         Lin Vertical
*                         2                         Cir Left
*                         3                         Cir Right
*     Modulation system   0                         DVB-S
*                         1                         DVB-S2
*     Roll off            0                         a=0.35
*                         1                         a=0.25
*                         2                         a=0.20
*                         3                         Reserved
*     Modulation type
*     (DVB-S)             0                         Reserved
*                         1                         QPSK
*                         2                         8PSK
*                         3                         16-QAM
*     Modulation type
*     (DVB-S2)            0                         Reserved
*                         1                         QPSK
*                         2                         8PSK
*                         3                         Reserved
*
*     Below is an example of a 32 Byte Homeing Channel Data:
*
*     DTH IRD with PAS4 settings
*     Byte             Value            Description
*     [0]              0x80             DTH IRD
*     [1]              0x01             Frequency = 12.5175
*     [2]              0x25
*     [3]              0x17
*     [4]              0x50
*     [5]              0x06             Orbital Pos = 68.5 deg
*     [6]              0x85
*     [7]              0x02             Symbol rate = 21.850
*     [8]              0x18
*     [9]              0x50
*     [10]             0x00
*     [11]             0x04             FEC 5/6
*     [12]             0x95             East, DVB-S2, a=0.20  Horizontal, QPSK
*     [13]-[31]        0x00             Spare
*     @endcode
*
* @retval
*    ::UC_ERROR_SUCCESS
* @retval
*    ::UC_ERROR_NULL_PARAM
* @retval
*    ::UC_ERROR_INSUFFICIENT_BUFFER
* @retval
*     Other The method may return any other errors listed in \ref result "Result Code List".
*
*/
uc_result UniversalClient_GetHomingChannelData(uc_buffer_st* pHomingChannelData);


/**
* Obtain Middleware user data
*
* This API is obsolete from Cloaked CA Agent 2.9.0. To get middleware user data,
* please refer to ::UniversalClient_Extended_GetMiddlewareUserData.
*
* This function can be called to obtain Middleware user data.
*
* @param[in,out] pMiddlewareUserData Receives Middleware user data.
*     The application must set the uc_buffer_st::bytes member to point to a buffer in memory, and the uc_buffer_st::length member
*     to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer to contain the Middleware user data,
*     in binary big-endian format, without any terminating character and set the uc_buffer_st::length member to the actual length of data returned.
*     If the uc_buffer_st::length member is not large enough to hold the entire Middleware user data, the uc_buffer_st::length member
*     will be set to the minimum required amount, and the function will return ::UC_ERROR_INSUFFICIENT_BUFFER. The application
*     can then allocate memory of a sufficient size and attempt to call the function again.
*
*     \note For simplicity, the application can allocate ::UC_BUFFER_GENERIC_SIZE bytes memory for uc_buffer_st::bytes
*       and set uc_buffer_st::length to ::UC_BUFFER_GENERIC_SIZE. Cloaked CA Agent will set uc_buffer_st::length to the actual size.
*
*
* @retval
*    ::UC_ERROR_SUCCESS
* @retval
*    ::UC_ERROR_NULL_PARAM
* @retval
*    ::UC_ERROR_INSUFFICIENT_BUFFER
* @retval
*     Other The method may return any other errors listed in \ref result "Result Code List".
*
*/

uc_result UniversalClient_GetMiddlewareUserData(uc_buffer_st * pMiddlewareUserData);


/**
 * Obtains the list of current products.
 *
 * This function can be used to obtain a list of products.  Each product consists of an ID and
 * an entitlement state.  This function could be used for example to display a different image or
 * color in a program guide for programs associated with these products.
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[out] pProductCount If the function succeeds, this receives the number of
 *     elements returned in the ppProductList array.
 * @param[out] ppProductList If the function succeeds, this receives a pointer to a function-allocated
 *     array of ::uc_product_status structures.  When the caller no longer needs the information in the
 *     array, it must call ::UniversalClient_FreeProductList to free the array.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_GetProductList(uc_uint32 *pProductCount, uc_product_status **ppProductList);

/**
 * Free resources allocated in ::UniversalClient_GetProductList.
 *
 * This function will free the memory allocated by a previous call to ::UniversalClient_GetProductList.
 *
 * @param[in,out] ppProductList On input, this is the address of the pointer received in a previous call to ::UniversalClient_GetProductList.
 *       On output, the function sets this to NULL.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_FreeProductList(uc_product_status **ppProductList);

/**
 * Obtain the count of ECM and EMM sections processed by Cloaked CA Agent.
 *
 * This API is obsolete from Cloaked CA Agent 2.9.0. To get ECM and EMM count,
 * please refer to ::UniversalClient_Extended_GetEcmEmmCount.
 *
 * This function can be called to obtain count of CA sections that have been processed by Cloaked CA Agent.
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[out] pCaSectionCount If the function succeeds, this receives the structure of
 *     ::uc_ca_section_count structures contained the count of ECM and EMM processed by Cloaked CA Agent.
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_GetEcmEmmCount(uc_ca_section_count *pCaSectionCount);

/**
 * Notify OOB EMM sections to the Client
 *
 * This function is intended to notify the client with oob emm sections.
 *
  *
  * @param[in] pOobEmmSection Buffer containing the raw content of at least one oob emm sections.
  *     The uc_buffer_st::bytes member must point to a buffer containing the section data, and
  *     the uc_buffer_st::length must be set to the number of bytes in the buffer.
  *
  * @retval
  *    ::UC_ERROR_SUCCESS
  * @retval
  *    ::UC_ERROR_OUT_OF_MEMORY
  * @retval
  *    ::UC_ERROR_NULL_PARAM
  * @retval
  *     Other The method may return any other errors listed in \ref result "Result Code List".
  */
uc_result UniversalClient_SendOobEmmSection(const uc_buffer_st * pOobEmmSection);


/**
 * This function will free the generic memory allocated by Cloaked CA Agent.
 *
 * @param[in,out] pMemoryData On input, the uc_buffer_st::bytes member is
 *       the address holding a buffer, and the uc_buffer_st::length member is the number of bytes in the buffer.
 *       On output, the function frees the memeory, sets uc_buffer_st::bytes member to NULL,
 *       and sets uc_buffer_st::bytes member to 0.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */

uc_result UniversalClient_FreeBuffer(
    uc_buffer_st *pMemoryData );


/**
 * Clear the CA data on Persistent Storage.
 *
 * This API is obsolete from Cloaked CA Agent 2.9.0. To reset CA data,
 * please refer to ::UniversalClient_Extended_ResetCAData.
 *
 * This function will clear all CA data on Persistent Storage. All CA data will be lost.
 * This function shall be called when the user wants to reset its device to factory state.
 * After the data is cleared, ::UC_GLOBAL_NOTIFY_CA_DATA_CLEARED will be sent to device application
 * as a notification.
 *
 * \note This function is dangerous! Device application or the middleware shall make sure that it's
 * never triggered by accidental user actions.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_ALREADY_STOPPED
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result  UniversalClient_ResetCAData(void);


/**
 * Obtain the current version of Secure Core.
 *
 * This API is obsolete from Cloaked CA Agent 2.9.0. To get secure core version,
 * please refer to ::UniversalClient_Extended_GetSecureCoreVersion.
 *
 * This function can be called to obtain the current version of Secure Core. This version can be used by applications to
 * check for supported features and application compatibility.  An application might also display a status screen with
 * version information on it that can be used for technical support purposes.
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[in,out] pVersion Receives the version string associated with Secure Core.
 *     The application must set the uc_buffer_st::bytes member to point to a buffer in memory, and the uc_buffer_st::length member
 *     to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer to contain the version
 *     string, including NULL terminator, and set the uc_buffer_st::length member to the actual length of data returned.
 *     If the uc_buffer_st::length member is not large enough to hold the entire version string, the uc_buffer_st::length member
 *     will be set to the minimum required amount, and the function will return ::UC_ERROR_INSUFFICIENT_BUFFER. The application
 *     can then allocate memory of a sufficient size and attempt to call the function again.
 *
 *     \note For simplicity, the application can allocate ::UC_BUFFER_GENERIC_SIZE bytes memory for uc_buffer_st::bytes
 *       and set uc_buffer_st::length to ::UC_BUFFER_GENERIC_SIZE. Cloaked CA Agent will set uc_buffer_st::length to the actual size,
 *       and the length includes 1 byte NULL terminator.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_GetSecureCoreVersion( uc_buffer_st *pVersion);


/* Followings are extended APIs for multiple operators */

/**
 * Obtain the serial number for multiple operators
 *
 * This API is a replace version. For Cloaked CA Agent 2.8.x and lower, please use
 * ::UniversalClient_GetSerialNumber.
 *
 * This function can be used to obtain the Serial Number of the Cloaked CA Agent.  This serial
 * number is assigned by the headend system, and is used to uniquely address a particular device
 * when sending \ref glossary_emm EMMs to it.
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * \note This serial number MAY BE DIFFERENT from the Chipset Serial Number (CSSN) that is returned
 * from ::UniversalClientSPI_Device_GetCSSN.
 *
 * @param[out] pSN contains the serial number assigned to the client.
 *     The uc_serial_number::validOperatorCount indicates the number of current valid operator.
 *     The uc_serial_number::serialNumber indicates the client ID string for specific operator.
 *     The uc_serial_number_per_operator::caSystemID indicates the operator by its CA System ID.
 *     The uc_serial_number_per_operator::serialNumberBytes will be filled with 4 bytes without a NULL-terminated string.
 *
 *     \n
 *     \note
 *     Suppose the Serial Number is 2864434397 i.e. 0xAABBCCDD, uc_serial_number_per_operator::serialNumberBytes will have
 *     the following values: \n
 *     uc_serial_number_per_operator::serialNumberBytes[0] = 0xAA, \n
 *     uc_serial_number_per_operator::serialNumberBytes[1] = 0xBB, \n
 *     uc_serial_number_per_operator::serialNumberBytes[2] = 0xCC, \n
 *     uc_serial_number_per_operator::serialNumberBytes[3] = 0xDD. \n
 *     \n
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_Extended_GetSerialNumber(uc_serial_number *pSN);

/**
 * Obtain the client ID string for multiple operators
 *
 * This API is a replace version. For Cloaked CA Agent 2.8.x and lower, please use
 * ::UniversalClient_GetClientIDString.
 *
 * This function can be called to obtain the client ID string.
 * The client ID string is supposed to be shown in the error banner, so that the viewer can read the identifiers directly from the error banner.
 * If current client has been initialized with a unique address, the string will look like "xxxxxxxxxx(y)". xxxxxxxxxx will be the unique address in decimal format.
 * If current client is a virgin one, the string will look like "xxxxxxxxxx(y)-z". xxxxxxxxxx will be CSSN in decimal format and z will be the Lock ID in decimal format.
 * y is the check digit for xxxxxxxxxx.\n
 * An example string containing a unique address:0000004097(2) \n
 * An example string containing a CSSN and Lock ID:0000004097(2)-1 \n
 *
 * \note The information from this method must be displayed in the error banner dialog.
 *
 * @param[out] pClientIDString Receives the Client ID string assigned to the client.
 *     The uc_client_id::validOperatorCount indicates the number of current valid operator.
 *     The uc_client_id::clientID indicates the client ID string for specific operator.
 *     The uc_client_id_per_operator::caSystemID indicates the operator by its CA System ID.
 *     The uc_client_id_per_operator::clientIDString will be filled a NULL-terminated string.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_Extended_GetClientIDString(uc_client_id *pClientIDString);

/**
 * Obtain the operator information for multiple operators
 *
 * This API is a replace version. For Cloaked CA Agent 2.8.x and lower, please use
 * ::UniversalClient_GetCASytemID and ::UniversalClient_GetCASystemIDRange to get
 * operator's CA System ID and range.
 *
 * This function can be called to obtain the operator information supported.
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[out] pOperatorInfo contains the operator information supported by the device.
 *     The uc_operator::validOperatorCount indicates the number of current valid operator.
 *     The uc_operator::operatorInfo describes the information for specific operator.
 *     The uc_single_operator::active indicates if the operator is active. If UC_TRUE, the operator is active. Or UC_FALSE will be returned.
 *     The uc_single_operator::activeCaSystemID indicates the CA System ID which matches the real network for the operator.
 *     The uc_single_operator::caSystemIDStart indicates the start value of the range for the operator.
 *     The uc_single_oeprator::caSystemIDEnd indicates the end value of the range for the operator.
 *     The uc_single_oeprator::operatorName will be filled with a NULL-terminated string.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_NO_NATIONALITY
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_Extended_GetOperatorInfo(uc_operator *pOperatorInfo);

/**
 * Obtain the nationality for multiple operators
 *
 * This API is a replace version. For Cloaked CA Agent 2.8.x and lower, please use
 * ::UniversalClient_GetNationality.
 *
 * This function can be called to obtain the nationality of the Cloaked CA Agent.
 * The nationality is described by three ISO 3166-1 Alpha-3 identifier (e.g. 'CHN', 'GBR', 'USA').
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[out] pNationality contains the nationality associated with the client.
 *     The uc_nationality::validOperatorCount indicates the number of current valid operator.
 *     The uc_nationality::nationality describes the nationality data for specific operator.
 *     The uc_nationality_per_operator::caSystemID indicates the operator by its CA System ID.
 *     The uc_nationality_per_operator::nationalityData member will be filled with 3 bytes data without a NULL-terminated.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_NO_NATIONALITY
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_Extended_GetNationality(uc_nationality *pNationality);

/**
 * Obtain the homing channel for multiple operators
 *
 * This API is a replace version. For Cloaked CA Agent 2.8.x and lower, please use
 * ::UniversalClient_GetHomingChannelData.
 *
 * This function can be called to obtain Homing Channel data.
 *
 *
 * @param[out] pHomingChannelData contains Homing Channel data.
 *     The uc_homing_channel::validOperatorCount indicates the number of current valid operator.
 *     The uc_homing_channel::homingChannel describes the homing channel data for specific operator.
 *     The uc_homing_channel_per_operator::caSystemID indicates the operator by its CA System ID.
 *     The uc_homing_channel_per_operator::homingChannelData contains detail homing channel data.
 *
 *     @code
 *     Format of the data(The bit order is bslbf):
 *
 *     Field Name      Length(bits)
 *     DTH_IRD         1
 *     ProfDec_IRD     2
 *     Spare           4
 *     Monitoring_IRD  1
 *     Homing_Channel  96
 *     Spare           152
 *
 *     In the above strucure, Homing_Channel is defined as:
 *
 *     Field Name            Length(bits)
 *     Frequency             32
 *     Orbital_Position      16
 *     Symbol_Rate           28
 *     Reserved              4
 *     MuxVerifier_Mode      4
 *     FEC_Inner             4
 *     West_East_Flag        1
 *     Polarisation          2
 *     Roll off              2
 *     Modulation System     1
 *     Modulation Type       2
 *
 *     The detailed bit format of the above fields in Home_Channel structure is:
 *
 *     Field Name          Value                     Description
 *     Frequency           DD D.D DD DD              where D = BCD digit
 *     Orbital Pos         DD D.D                    where D = BCD digit
 *     Symbol Rate         DD D.D DD D               where D = BCD digit
 *     Mux Verifier        0                         Mux No Allowed
 *                         1                         Mux Early
 *                         2                         Mux Late
 *                         3                         Mux Middle
 *     FEC lnner           1                         1/2
 *                         2                         2/3
 *                         3                         3/4
 *                         4                         5/6
 *                         5                         7/8
 *     West/East           0                         West
 *                         1                         East
 *     Polarisation        0                         Lin Horizontal
 *                         1                         Lin Vertical
 *                         2                         Cir Left
 *                         3                         Cir Right
 *     Modulation system   0                         DVB-S
 *                         1                         DVB-S2
 *     Roll off            0                         a=0.35
 *                         1                         a=0.25
 *                         2                         a=0.20
 *                         3                         Reserved
 *     Modulation type
 *     (DVB-S)             0                         Reserved
 *                         1                         QPSK
 *                         2                         8PSK
 *                         3                         16-QAM
 *     Modulation type
 *     (DVB-S2)            0                         Reserved
 *                         1                         QPSK
 *                         2                         8PSK
 *                         3                         Reserved
 *
 *     Below is an example of a 32 Byte Homeing Channel Data:
 *
 *     DTH IRD with PAS4 settings
 *     Byte             Value            Description
 *     [0]              0x80             DTH IRD
 *     [1]              0x01             Frequency = 12.5175
 *     [2]              0x25
 *     [3]              0x17
 *     [4]              0x50
 *     [5]              0x06             Orbital Pos = 68.5 deg
 *     [6]              0x85
 *     [7]              0x02             Symbol rate = 21.850
 *     [8]              0x18
 *     [9]              0x50
 *     [10]             0x00
 *     [11]             0x04             FEC 5/6
 *     [12]             0x95             East, DVB-S2, a=0.20  Horizontal, QPSK
 *     [13]-[31]        0x00             Spare
 *     @endcode
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 *
 */
uc_result UniversalClient_Extended_GetHomingChannelData(uc_homing_channel *pHomingChannelData);

/**
 * Obtain the middleware user data for multiple operators
 *
 * This API is a replace version. For Cloaked CA Agent 2.8.x and lower, please use
 * ::UniversalClient_GetMiddlewareUserData.
 *
 * This function can be called to obtain Middleware user data for multiple operators.
 *
 * @param[out] pMiddlewareUserData Receives Middleware user data.
 *     The uc_middleware_user_data::validOperatorCount indicates the number of current valid operator.
 *     The uc_middleware_user_data::middlewareUserData describes the middleware data for specific operator.
 *     The uc_middleware_user_data_per_operator::caSystemID indicates the operator by its CA System ID.
 *     The uc_middleware_user_data_per_operator::middlewareUserData contains detail middleware user data.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_Extended_GetMiddlewareUserData(uc_middleware_user_data *pMiddlewareUserData);

/**
 * Obtain the ecm and emm count for multiple operators
 *
 * This API is a replace version. For Cloaked CA Agent 2.8.x and lower, please use
 * ::UniversalClient_GetEcmEmmCount.
 *
 * This function can be called to obtain count of CA sections that have been processed by Cloaked CA Agent.
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[out] pCaSectionCount If the function succeeds, this receives the structure of
 *     ::uc_ca_extended_section_count structures contained the count of ECM and EMM processed by Cloaked CA Agent.
 *     The uc_ca_extended_section_count::validOperatorCount indicates the number of current valid operator.
 *     The uc_ca_extended_section_count::caSectionCount describes the CA section count for specific operator.
 *     The uc_ca_section_count_per_operator::caSystemID indicates the operator by its CA System ID.
 *     The uc_ca_section_count_per_operator::ecm_count contains the ecm section count.
 *     The uc_ca_section_count_per_operator::emm_count contains the emm section count.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_Extended_GetEcmEmmCount(uc_ca_extended_section_count *pCaSectionCount);

/**
 * Clear the CA data on Persistent Storage for multiple operators.
 *
 * This API is a replace version. For Cloaked CA Agent 2.8.x and lower, please use
 * ::UniversalClient_ResetCAData.
 *
 * This function will clear CA data on Persistent Storage for specific operators. The CA data will be lost.
 * This function shall be called when the user wants to reset its device to factory state.
 * After the data is cleared, ::UC_GLOBAL_NOTIFY_CA_DATA_CLEARED will be sent to device application
 * as a notification.
 *
 * \note This function is dangerous! Device application or the middleware shall make sure that it's
 * never triggered by accidental user actions.
 *
 * @param[in] pCASystemID indicates the operator whose CA data will be cleared.
 *     The uc_ca_system_id::validOperatorCount indicates the number of operators. If the validOperatorCount is 0, all operators' CA
 *     data will be cleared. If the validOperatorCount is greater than 0, the CA data for the operator who is identifies by CA System ID in
 *     uc_ca_system_id::caSystemID will be cleared.
 *     The uc_ca_system_id::caSystemID indicates the operator by its CA System ID. The CA System ID can be gotten by
 *     UniversalClient_Extended_GetOperatorInfo().
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_ALREADY_STOPPED
 * @retval
 *    ::UC_ERROR_INVALID_CA_SYSTEM_ID
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_Extended_ResetCAData(const uc_ca_system_id *pCASystemID);

/**
 * Obtain the secure core version for multiple operators
 *
 * This API is a replace version. For Cloaked CA Agent 2.8.x and lower, please use
 * ::UniversalClient_GetSecureCoreVersion.
 *
 * This function can be called to obtain the current versions of Multi Secure Cores. These version can be used by applications to
 * check for supported features and application compatibility.  An application might also display a status screen with
 * version information on it that can be used for technical support purposes.
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[out] pVersion Receives the version string associated with Multi Secure Cores.
 *     The uc_securecore_version::validOperatorCount indicates the number of current valid operators, and the uc_securecore_version::version
 *     describes the securecore version information for specific operator.
 *     The uc_single_securecore_version::caSystemID indicates the operator by its CA System ID, and the uc_single_securecore_version::versionString
 *     contains the securecore information.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_Extended_GetSecureCoreVersion(uc_securecore_version *pVersion);

/** @} */ /* End of groups */


/** @defgroup tmsapi Cloaked CA Agent TMS APIs
 *  Cloaked CA Agent TMS APIs
 *
 *  These API methods provide interfaces needed by Targeted Messaging System (TMS).
 *  TMS handles text and fingerprint messages to target specific groups of subscribers according to attributes,
 *  which can be dynamically generated and set. These specific groups are identified by attribute settings, set by CA server.
 *
 *  @{
 */


/**
* Set current service ID, transport ID and network ID for TMS
*
* This method is designed to set current tuned service information for TMS. The tuned service information
* is represented by the uc_dvb_tuned_info structure that contains serviceID, transportID and networkID. See ::uc_dvb_tuned_info.
*
* If TMS is activated, it must be called when any one of service ID, transport ID and networkID is changed.
* ServiceID/TransportID/NetworkID is one of the ways to set TMS data.
*
* \note If the client device application supports ONLY 1 descramble service, there is no need to invoke ::UniversalClient_SetTunedInfoList.
* ::UniversalClient_SetTunedInfo is designed to support 1 descramble service ONLY, and ::UniversalClient_SetTunedInfoList is designed to support multiple descramble services.
* Each time this function is called, Univesal Client will clean up the existing tuned service info set via ::UniversalClient_SetTunedInfo or ::UniversalClient_SetTunedInfoList.
*
* @param[in] serviceID current service ID
*
* @param[in] transportID current tansport ID
*
* @param[in] networkID current network ID
*
* @retval
*    ::UC_ERROR_SUCCESS
* @retval
*     Other The method may return any other errors listed in \ref result "Result Code List".
*
*/
uc_result UniversalClient_SetTunedInfo(uc_uint16 serviceID, uc_uint16 transportID, uc_uint16 networkID);

/**
* Set a list of tuned services for TMS to Cloaked CA Agent.
*
* This method is designed to set a list of tuned services for TMS.
* If TMS is activated, it must be called when the tuned information is changed.
*
* \note This function is similar to ::UniversalClient_SetTunedInfo, but this function can set a list of tuned services to Cloaked CA Agent.
* Each time this function is called, Univesal Client will clean up the existing tuned service info set via ::UniversalClient_SetTunedInfo or ::UniversalClient_SetTunedInfoList.
* When ::tunedInfoCount is 1, this function acts like ::UniversalClient_SetTunedInfo.
*
* @param[in] tunedInfoCount The number of elements in the pTunedInfoList. If the value of tunedInfoCount is 0,
*        Cloaked CA Agent will clean up the existing tuned information and return directly.
*
* @param[in] pTunedInfoList Pointer to a ::uc_dvb_tuned_info array containing ::tunedInfoCount items.
*
* @retval
*    ::UC_ERROR_SUCCESS
* @retval
*     Other The method may return any other errors listed in \ref result "Result Code List".
*
*/
uc_result UniversalClient_SetTunedInfoList(uc_uint32 tunedInfoCount, uc_dvb_tuned_info* pTunedInfoList);


/**
* Obtain TMS data
*
* This API is obsolete from Cloaked CA Agent 2.9.0. To get TMS data,
* please refer to ::UniversalClient_Extended_GetTmsData.
*
* This function can be called to obtain TMS data.
*
* \note The information from this method must be displayed in a GUI dialog. TMS data should be displayed in a hexadecimal format.
* TMS data from this method is for display purpose only.
*
*
* @param[in,out] pTmsData Receives TMS data.
*     The application must set the uc_buffer_st::bytes member to point to a buffer in memory, and the uc_buffer_st::length member
*     to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer to contain the TMS data,
*     in binary big-endian format, without any terminating character and set the uc_buffer_st::length member to the actual length of data returned.
*     If the uc_buffer_st::length member is not large enough to hold the entire TMS data, the uc_buffer_st::length member
*     will be set to the minimum required amount, and the function will return ::UC_ERROR_INSUFFICIENT_BUFFER. The application
*     can then allocate memory of a sufficient size and attempt to call the function again.
*
*     \note For simplicity, the application can allocate ::UC_BUFFER_GENERIC_SIZE bytes memory for uc_buffer_st::bytes
*       and set uc_buffer_st::length to ::UC_BUFFER_GENERIC_SIZE. Cloaked CA Agent will set uc_buffer_st::length to the actual size.
*
*
* @retval
*    ::UC_ERROR_SUCCESS
* @retval
*    ::UC_ERROR_NULL_PARAM
* @retval
*    ::UC_ERROR_INSUFFICIENT_BUFFER
* @retval
*     Other The method may return any other errors listed in \ref result "Result Code List".
*
*/
uc_result UniversalClient_GetTmsData(uc_buffer_st* pTmsData);

/**
* Obtain TMS data for multiple operators.
*
* This API is a replace version. For Cloaked CA Agent 2.8.x and lower, please use
* ::UniversalClient_GetTmsData.
*
* This function can be called to obtain TMS data for multiple operators.
*
* \note The information from this method must be displayed in a GUI dialog. TMS data should be displayed in a hexadecimal format.
* TMS data from this method is for display purpose only.
*
*
* @param[out] pTmsData Receives TMS data.
*     The uc_tms_data::validOperatorCount indicates the number of current valid operator.
*     The uc_tms_data::tms indicates the TMS data for specific operator.
*     The uc_tms_data_per_operator::caSystemID indicates the operator by its CA System ID.
*     The uc_tms_data_per_operator::tmsData contains detail TMS data.
*
*
* @retval
*    ::UC_ERROR_SUCCESS
* @retval
*    ::UC_ERROR_NULL_PARAM
* @retval
*    ::UC_ERROR_INSUFFICIENT_BUFFER
* @retval
*     Other The method may return any other errors listed in \ref result "Result Code List".
*
*/
uc_result UniversalClient_Extended_GetTmsData(uc_tms_data *pTmsData);

/**
* Set TMS attributes.
*
* The function shall be called when middleware wants to set client-based tms attributes.
*
*
* @param[in] attributeCount Indicate the count of client-based TMS attributes in ::pTmsAttributesItemInfoList.
*
* @param[in] pTmsAttributesItemInfoList Pointer to an array of ::uc_tms_attribute_item_info items.
*     The device application is responsible for maintaining this buffer.
*
*
* @retval
*    ::UC_ERROR_SUCCESS
* @retval
*    ::UC_ERROR_NULL_PARAM
* @retval
*     Other The method may return any other errors listed in \ref result "Result Code List".
*
*/
uc_result UniversalClient_SetTMSAttributes(uc_uint16 attributeCount,  uc_tms_attribute_item_info* pTmsAttributesItemInfoList);


/** @} */ /* End of groups */


/** @defgroup copycontrolapis Cloaked CA Agent Copy Control APIs
 *  Cloaked CA Agent Copy Control APIs
 *
 *  These APIs are for copy control functionality.
 *  In order to achieve copy control functionality, \ref copycontrolspi "Cloaked CA Agent Copy Control SPIs" must be implemented.
 *
 *  @{
 */

/**
  * Enable/Disable Macrovision
  *
  * \note This API is abandoned. Cloaked CA Agent will check the SPI interfaces to decide whether the client device supports Macrovision.
  * If the Macrovision SPI interfaces in \ref copycontrolspi "Cloaked CA Agent Copy Control SPIs" are implemented, Cloaked CA Agent will treat the client device
  * as Macrovision supported.
  *
  * @param[in] enable Set to ::UC_TRUE if the platform supports Macrovision, else ::UC_FALSE.
  *
  * @retval
  *    ::UC_ERROR_SUCCESS
  * @retval
  *     Other The method may return any other errors listed in \ref result "Result Code List".
  */
uc_result UniversalClient_EnableMacrovision(uc_bool enable);

/** @} */ /* End of groups */


/** @defgroup regionalfilteringapis Cloaked CA Agent Regional Filtering APIs
 *  Cloaked CA Agent Regional Filtering APIs
 *
 *  Regional Filtering is not a mandatory feature, but if the operator requires the client devices to implement it, then it is a must.
 *
 *  These APIs are for Regional Filtering.
 *  Regional Filtering is a feature to lock the secure client in a specific operational region.
 *  The region info is played out in certain service information tables, and also downloaded into the secure client.
 *  Cloaked CA Agent checks before descrambling whether the region info of the service and that for the secure client match,
 *  unless Cloaked CA Agent or secure client is configured to ignore the checking.
 *
 *  @{
 */

/**
 * Enable or disable regional filtering
 *
 * This function can be used to enable or disable regional filtering from the application level.
 * If regional filtering is enabled, devices are prevented from descrambling services in
 * regions where they are not blocked.
 *
 * By default, regional filtering is disabled.  The client does not remember the state
 * of regional filtering between restarts, so this function must be called at each initialization time.
 *
 * @param[in] enable Indicates whether to enable or disable regional filtering. If this value is ::UC_TRUE,
 *        Regional Filtering will be enabled. Otherwise, it will be disabled.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_EnableRegionalFiltering(uc_bool enable);


/**
 * Notify the client of a new regional filtering descriptor
 *
 * This function is intended to notify a client of a new IA regional filtering descriptor.
 * A 'IA regional filtering descriptor' is a block of data that is intended to notify a device
 * of which geographical region it is in. If regional filtering is enabled, this information
 * is later used to determine if a client is entitled to descramble services in this region.
 * A client is entitled to descramble services in a region if it has received an appropriate
 * 'regional filtering EMM' via an open EMM service.
 *
 * In a typical \ref glossary_dvb "DVB" system, regional filtering descriptors are included
 * as part of the 'CA descriptor' field within the \ref glossary_cat "CAT". In unusual configurations
 * or in other transports, the regional filtering may take different forms.
 *
 * \note Regional filtering does not apply until it is enabled by calling ::UniversalClient_EnableRegionalFiltering.
 *
 * The format of the regional filtering descriptor is as follows
 *
 \code
    Regional_Filtering_Descriptor()
    {
        descriptor_tag = 0xA0                       (8 bits)
        descriptor_length                           (8 bits)

        IA_Regional_Filtering_Descriptor()
        {
            descriptor_tag = 0xA0                   (8 bits)
            descriptor_version                      (8 bits)
            sector_count                            (8 bits)
            for(i=0;i<sector_count;I++)
            {
                sector_index                        (8 bits)
                region_info                         (8 bits)
                sub_region_info                     (8 bits)
            }
        }
    }
 \endcode
 *
 * \note IA_Regional_Filtering_Descriptor() is different from Regional_Filtering_Descriptor().
 *
    Explanation of the fields:
    - \b descriptor_tag: The 8-bit tag value for this descriptor will always be set to 0xA0.
    - \b descriptor_length:  The length of the descriptor in bytes.
    - \b descriptor_tag: The 8-bit tag value for this descriptor will always be set to 0xA0.
    - \b descriptor_version:  The 8-bit version value should be different for different descriptor.
    - \b sector_count: The 8-bit count value should be the total sector count in this RF descriptor.
    - \b sector_index:  The 8-bit sector index value should be the index num of the sector.
    - \b region_info: The 8-bit region value should be the region of the sector.
    - \b sub_region_info: The 8-bit sub region value should be the sub region of the sector.
  *
  * @param[in] pDescriptor Buffer containing the raw contents of the IA_Regional_Filtering_Descriptor.
  *     The uc_buffer_st::bytes member must point to a buffer containing the descriptor, and
  *     the uc_buffer_st::length must be set to the number of bytes in the buffer.
  *
  * @retval
  *    ::UC_ERROR_SUCCESS
  * @retval
  *    ::UC_ERROR_OUT_OF_MEMORY
  * @retval
  *    ::UC_ERROR_NULL_PARAM
  * @retval
  *     Other The method may return any other errors listed in \ref result "Result Code List".
  */
uc_result UniversalClient_NotifyRegionalFilteringDescriptor(
     const uc_buffer_st *pDescriptor);

/**
 * Obtains the list of Region Information
 *
 * This API is obsolete from Cloaked CA Agent 2.9.0. To get regional information,
 * please refer to ::UniversalClient_Extended_GetRegionInfoList.
 *
 * This function can be called to obtain the list of region and subregion indexed by sector number.
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[out] pRegioninfoCount If the function succeeds, this receives the number of
 *     elements returned in the ppRegioninfoList array.
 * @param[out] ppRegioninfoList If the function succeeds, this receives a pointer to a function-allocated
 *     array of ::uc_region_information structures.  When the caller no longer needs the information in the
 *     array, it must call ::UniversalClient_FreeRegionInfoList to free the array.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_MISSING_DATA
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_GetRegionInfoList(uc_uint32 *pRegioninfoCount, uc_region_information **ppRegioninfoList);

/**
 * Free resources allocated in ::UniversalClient_GetRegionInfoList.
 *
 * This function will free the memory allocated by a previous call to ::UniversalClient_GetRegionInfoList.
 *
 * @param[in,out] ppRegioninfoList On input, this is the address of the pointer received in a previous call to ::UniversalClient_GetRegionInfoList.
 *       On output, the function sets this to NULL.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_FreeRegionInfoList(uc_region_information **ppRegioninfoList);

/**
 * Obtains the list of Region Information for multiple operators.
 *
 * This API is a replace version. For Cloaked CA Agent 2.8.x and lower, please use
 * ::UniversalClient_GetRegionInfoList.
 *
 * This function can be called to obtain the list of region and subregion indexed by sector number.
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[out] pRegionalInfoList If the function succeeds, this receives a pointer to a function-allocated
 *     array of ::uc_regional_info_list structures.
 *     The uc_regional_info_list::validOperatorCount indicates the number of current valid operator.
 *     The uc_regional_info_list::regionalInfoList indicates the region and subregion for specific operator.
 *     The uc_regional_info_list_per_operator::validSectorCount indicates the number of valid sector.
 *     The uc_regional_info_list_per_operator::regionInfoListPerOperator indicates the region and subregion information for specific sector.
 *     The uc_region_information::sector_number indicates the sector number which contains followed region information.
 *     The uc_region_information::region indicates the region information for the sector.
 *     The uc_region_information::subregion indicates the subregion information for the sector.
 *     The uc_region_information::caSystemID indicates the operator for the sector by its CA System ID.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_MISSING_DATA
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_Extended_GetRegionInfoList(uc_regional_info_list *pRegionalInfoList);


/** @} */ /* End of groups */



/** @defgroup testingapi Cloaked CA Agent Testing APIs
 *  Cloaked CA Agent Testing APIs
 *
 *  These API methods provide the necessary basic functionality to test secure chip with STB file from SCPC,
 *  and generate \ref glossary_tsc "TSC" CHIP record needed by SCPC to create upload files for this device.
 *
 *  Testing APIs are ONLY used on the product stage. They can be ignored in the trial stage.
 *
 *  Please refer to \ref usestestingkeys "Use device testing APIs" on how to use these APIs.
 *
 *  @{
 */

/**
 * Max length of a \ref glossary_tsc "TSC" CHIP record.
 * Used by ::UniversalClient_CreateTscChipRecord
 */
#define SIZE_MAX_TSC_CHIP_RECORD (4096)


/**
 * Set testing CSSK and CW.
 *
 * This method is designed to test secure chip key ladder.It shall ONLY be called at verification phase.
 * When this method is called, Cloaked CA Agent will enter Test Mode.
 * In Test Mode, any incoming ECM section will trigger the call to ::UniversalClientSPI_Device_SetCSSK and ::UniversalClientSPI_Stream_SetDescramblingKey.
 * with the CSSK and CW from this method.
 *
 * Irdeto will provide CW and CSSK together with a corresponding bitstream.
 *
 * This method will just cache the CSSK and CW in a internal buffer and then return.
 *
 *
 * @param[in] pCssk CSSK key material to the device. This is normally protected by a secure chipset unique key
 *     (CSUK) that resides within the secure chipset such that an attacker gains no advantage by intercepting this method.
 *     The uc_buffer_st::bytes member points to a buffer containing the raw key material, and the uc_buffer_st::length member contains
 *     the number of bytes in the buffer.
 * @param[in] pEvenDescramblingKeyInfo Information about the even key being used to descramble content. This contains key algorithm, protection,
 *     cryptography context, and the key material. See ::uc_key_info.
 * @param[in] pOddDescramblingKeyInfo Information about the odd key being used to descramble content. This contains key algorithm, protection,
 *     cryptography context, and the key material. See ::uc_key_info.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_INVALID_HANDLE
 * @retval
 *    ::UC_ERROR_WRONG_HANDLE_TYPE
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_SetTestingKey(
    const uc_buffer_st *pCssk,
    const uc_key_info *pEvenDescramblingKeyInfo,
    const uc_key_info *pOddDescramblingKeyInfo
    );

/**
 * Set testing CSSK and CW with key infomation to support multi key ladder.
 *
 * This is the extension of ::UniversalClient_SetTestingKey function with additional key protection infomation provided,
 * in order to support differents key ladder.It shall ONLY be called at verification phase.
 * When this method is called, Cloaked CA Agent will enter Test Mode.Any incoming ECM section will trigger the call to
 * ::UniversalClientSPI_Device_SetCSSK and ::UniversalClientSPI_Stream_SetDescramblingKey.
 *
 * Irdeto will provide CW, CSSK, their key protection info together with a corresponding bitstream.
 *
 * This method will just cache the CSSK, CW, their key protection info in a internal buffer and then return.
 *
 *  \note This function is not formally supported, shall be ignored in current release.
 *
 * @param[in] pCSSKInfo Information about the CSSK being used to protect the descrambling key. The CSSK is also protected by CSUK.
 *     This contain CSSK protection and CSSK material.See ::uc_cssk_info.
 * @param[in] pEvenDescramblingKeyInfo Information about the even key being used to descramble content. This contains key algorithm, protection,
 *     cryptography context, and the key material. See ::uc_key_info.
 * @param[in] pOddDescramblingKeyInfo Information about the odd key being used to descramble content. This contains key algorithm, protection,
 *     cryptography context, and the key material. See ::uc_key_info.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_INVALID_HANDLE
 * @retval
 *    ::UC_ERROR_WRONG_HANDLE_TYPE
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_SetExtendedTestingKey(
    const uc_cssk_info * pCSSKInfo,
    const uc_key_info *pEvenDescramblingKeyInfo,
    const uc_key_info *pOddDescramblingKeyInfo
    );

/**
 * Request Cloaked CA Agent to reload personalized data from Persistent Storage.
 *
 * This method can be used to let Cloaked CA Agent reload the personalized data by calling
 * ::UniversalClientSPI_Device_GetPersonalizedData.
 *
 * \note This API is not used anymore.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_ReloadPersonalizedData(void);


/**
 * Create a tsc CHIP record to describe current device.
 *
 * This method will generate a \ref glossary_tsc "TSC" CHIP record.
 * Irdeto SCPC is expected to receive the \ref glossary_tsc "TSC" CHIP record in order to generate upload file for this device.
 * The call to this method will lead to the invoke to the following SPIs.
 *     -# ::UniversalClientSPI_Device_GetCSSN
 *     -# ::UniversalClientSPI_Device_GetSecurityState
 *     -# ::UniversalClientSPI_Device_GetPlatformIdentifiers
 *     -# ::UniversalClientSPI_Device_GetPrivateData
 *
 *
 * @param[in,out] tscChipRecord On input, Cloaked CA Agent will ZERO the incoming buffer.
 *     On output, it will be filled with a NULL-ended ASCII string with a max length ::SIZE_MAX_TSC_CHIP_RECORD,
 *     suitable for being inserted into the relevant CHIP entry of the \ref glossary_tsc "TSC File".
 *
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_INVALID_HANDLE
 * @retval
 *    ::UC_ERROR_WRONG_HANDLE_TYPE
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_CreateTscChipRecord(
    uc_char tscChipRecord[SIZE_MAX_TSC_CHIP_RECORD]
    );

/** @} */ /* End of groups */


/** @defgroup ppvvodapis Cloaked CA Agent PPV VOD APIs
 *  Cloaked CA Agent PPV VOD APIs
 *
 *  These API methods provide the necessary functionality for \ref glossary_ppv_vod "PPV VOD".
 *
 *  In this solution, Cloaked CA Agent will generate the Client Transaction Data at the start of VOD session via ::UniversalClient_PPV_VOD_PrepareRequest.
 *  The application should submit the Client Transaction Data to the CA server and send the response from CA server to Cloaked CA Agent via ::UniversalClient_SendOobEmmSection.
 *  VOD content is regarded as a normal scrambled service. If the client is entitled to view the VOD content, control words will be output to the SPI via ::UniversalClientSPI_Stream_SetDescramblingKey
 *  as normal.
 *
 *  To cancel a VOD request, the client device application can call ::UniversalClient_PPV_VOD_CancelRequest with the nonce returned from ::UniversalClient_PPV_VOD_PrepareRequest.
 *
 *  Refer to \ref ppv_vod_client_communication "PPV VOD Flow" to get an
 *  overview on how to play a \ref glossary_ppv_vod "PPV VOD" service.
 *
 *  For more details for integration code, please refer to \ref playppvvodservie "User plays PPV VOD service".
 *
 *  \note Cloaked CA Agent ONLY provides \ref glossary_ppv_vod "PPV VOD" feature for platform with secure chipset support.
 *
 *  @{
 */

/**
 * Request the Client Transaction Data at the start of \ref glossary_ppv_vod "PPV VOD" session.
 *
 * The client device application can call this method to get the Client Transaction Data from Cloaked CA Agent.
 * The Client Transaction Data is generated by Cloaked CA Agent in response to a VOD request.
 *
 * Refer to \ref ppv_vod_client_communication "PPV VOD Flow" to get an
 * overview on how to play a \ref glossary_ppv_vod "PPV VOD" service.
 *
 * The Client Transaction Data is organized with the following TLV format.
 * The application can parse the TLV format to get related information, including the VOD nonce,
 * the client Unique Address, client version, etc.
 *
 \code
Client_Transaction_Data()
{
    Field Name                      Field Length     Field Value

    tag_number                      8   bits         The total number of the tags
    length                          16  bits         The total byte length of the tags
    for(i=0;i<tag_number;i++)                        Loop tag_number times for the tags
    {
        tag                         8   bits         Tag. This can be UC_TAG_TYPE_VOD_NONCE, UC_TAG_TYPE_UNIQUE_ADDRESS and UC_TAG_TYPE_CLIENT_VERSION currently.
        length(N)                   8   bits         Tag length
        value                       8*N bits         Tag value
    }
    HMAC                            64  bits         The HMAC value of the whole parameters.
                                                     The application do not need to care about it.
}
 \endcode

 * \note For above tags, please refer to \ref vodtlvtags "Cloaked CA Agent TLV Tags for PPV VOD" for a list of TVL tags and its values.
 *
 * @param[out] pTransactionData Buffer containing the client transaction data.
 *     If the function succeeds, the uc_buffer_st::bytes member points to a function-allocated buffer containing a Client_Transaction_Data().
 *     The uc_buffer_st::length member must be set to the number of bytes in the buffer.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_VOD_CLIENT_TYPE_ERROR
 * @retval
 *    ::UC_ERROR_VOD_NO_SERIAL_NUMBER
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 *
 * \note When the caller no longer needs the information, it must call ::UniversalClient_FreeBuffer
         to free the uc_buffer_st::bytes member.
 *
 */
uc_result UniversalClient_PPV_VOD_PrepareRequest(
    uc_buffer_st *pTransactionData );

/**
 * Cancel a \ref glossary_ppv_vod "PPV VOD" request to the client.
 *
 * The client device application can call this method to cancel a request to the client with a VOD nonce data.
 *
 * Refer to \ref ppv_vod_client_communication "PPV VOD Flow" to get an
 * overview on how to play a \ref glossary_ppv_vod "PPV VOD" service.
 *
 * @param[in] pNonceData Buffer containing the nonce data.
 *     The uc_buffer_st::bytes member must point to a buffer containing the nonce data,
 *     which is the VOD nonce previously retrieved by a call to ::UniversalClient_PPV_VOD_PrepareRequest.
 *     The uc_buffer_st::length member must be set to the number of bytes in the buffer.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_VOD_INVALID_NONCE
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_PPV_VOD_CancelRequest(
    const uc_buffer_st *pNonceData );


/** @} */ /* End of groups */

/** @} */ /* End of Univeral Client APIs */


/** @defgroup convergentclient_api Unified Client Specific APIs

 *  Unified Client APIs
 *
 *  These API methods provide the necessary interfaces for the Unified Client which includes the functionality
 *  of Cloaked CA Agent and Softcell. When Unified Client is integrated, the following APIs shall be used in replace
 *  of the corresponding Universal Client APIs that are only for Cloaked CA Agent. However, not all APIs have its Unified
 *  Client specific version. Some of them can be used without caring whether Cloaked CA Agent or Unified Client is integrated.
 *  , such as ::UniversalClient_GetCSSN.
 *  @{
 */


/**
 * Obtain the current version of Softcell.
 *
 * This function can be called to obtain the current version of Softcell. This version can be used by applications to
 * check for supported features and application compatibility.  An application might also display a status screen with
 * version information on it that can be used for technical support purposes.
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[in,out] pVersion Receives the version string associated with Softcell.
 *     The application must set the uc_buffer_st::bytes member to point to a buffer in memory, and the uc_buffer_st::length member
 *     to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer to contain the version
 *     string, including NULL terminator, and set the uc_buffer_st::length member to the actual length of data returned.
 *     If the uc_buffer_st::length member is not large enough to hold the entire version string, the uc_buffer_st::length member
 *     will be set to the minimum required amount, and the function will return ::UC_ERROR_INSUFFICIENT_BUFFER. The application
 *     can then allocate memory of a sufficient size and attempt to call the function again.
 *
 *     \note For simplicity, the application can allocate ::UC_BUFFER_GENERIC_SIZE bytes memory for uc_buffer_st::bytes
 *       and set uc_buffer_st::length to ::UC_BUFFER_GENERIC_SIZE. Cloaked CA Agent will set uc_buffer_st::length to the actual size,
 *       and the length includes 1 byte NULL terminator.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_GetSoftcellVersion( uc_buffer_st *pVersion);


/**
 * Obtain the build information of Softcell
 *
 * This function can be called to obtain the build information of Softcell.
 * The application shall display the build info in the client status screen.
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[in,out] pBuildInfo Receives the build string associated with the client.
 *     The application must set the uc_buffer_st::bytes member to point to a buffer in memory, and the uc_buffer_st::length member
 *     to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer to contain the build
 *     string, including NULL terminator, and set the uc_buffer_st::length member to the actual length of data returned.
 *     If the uc_buffer_st::length member is not large enough to hold the entire build string, the uc_buffer_st::length member
 *     will be set to the minimum required amount, and the function will return ::UC_ERROR_INSUFFICIENT_BUFFER. The application
 *     can then allocate memory of a sufficient size and attempt to call the function again.
 *
 *     \note For simplicity, the application can allocate ::UC_BUFFER_GENERIC_SIZE bytes memory for uc_buffer_st::bytes
 *       and set uc_buffer_st::length to ::UC_BUFFER_GENERIC_SIZE. Cloaked CA Agent will set uc_buffer_st::length to the actual size,
 *       and the length includes 1 byte NULL terminator.
 *
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_GetSoftcellBuildInfo(
    uc_buffer_st *pBuildInfo);


/**
 * Obtain the status of smartcard.
 *
 * This function can be called to obtain smartcard status.
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[in] smartcardID smartcard id. This parameter is not used.
 * @param[out] pSmartcardStatus If the function succeeds, the smartcard status information is copied in it.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_GetSmartcardStatus(
    uc_uint32 smartcardID,
    uc_smartcard_status* pSmartcardStatus );


/**
 * Obtain the smartcard infomation.
 *
 * This function can be called to obtain smartcard infomation.
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[in] smartcardID smartcard id. This parameter is not used.
 * @param[out] pSmartcardInfo If the function succeeds, the smartcard info is copied in it.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_GetSmartcardInfo(
    uc_uint32 smartcardID,
    uc_smartcard_info* pSmartcardInfo );


/**
 * Obtain the CA System ID from smartcard.
 *
 * This method can be called to obtain the active CA sytem ID from the smartcard.
 *
 * @param[in] smartcardID smartcard id. This parameter is not used.
 * @param[out] pCaSystemID If the function succeeds, the active CA system ID in smartcard is copied in it.
 *
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_GetSmartcardCASystemID(
    uc_uint32 smartcardID,
    uc_uint32 *pCaSystemID);


/**
 * Obtain the nationality of smartcard.
 *
 * This function can be called to obtain the nationality of Smartcard.
 * The nationality is described by three ISO 3166-1 Alpha-3 identifier (e.g. 'CHN', 'GBR', 'USA').
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[in] smartcardID smartcard id. This parameter is not used.
 * @param[in,out] pNationality Receives the nationality associated with the client.
 *     On output, the uc_buffer_st::bytes member will be filled with a NULL-terminated string.
 *
 *     The application must set the uc_buffer_st::bytes member to point to a buffer in memory, and the uc_buffer_st::length member
 *     to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer to contain the nationality
 *     , including NULL terminator, and set the uc_buffer_st::length member to the actual length of data returned.
 *     If the uc_buffer_st::length member is not large enough to hold the entire nationality, the uc_buffer_st::length member
 *     will be set to the minimum required amount, and the function will return ::UC_ERROR_INSUFFICIENT_BUFFER.
 *
 *     \note For simplicity, the application can allocate ::UC_BUFFER_GENERIC_SIZE bytes memory for uc_buffer_st::bytes
 *       and set uc_buffer_st::length to ::UC_BUFFER_GENERIC_SIZE. Cloaked CA Agent will set uc_buffer_st::length to the actual size.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_NO_NATIONALITY
 * @retval
 *    ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_GetSmartcardNationality(
        uc_uint32 smartcardID,
        uc_buffer_st *pNationality);

/**
 * Obtain the TMS data of smartcard.
 *
 * This function can be called to obtain TMS data of Smartcard.
 *
 * @param[in] smartcardID smartcard id. This parameter is not used.
 * @param[in,out] pTMSData Receives the TMS data of smartcard.
 *
 *     The application must set the uc_buffer_st::bytes member to point to a buffer in memory, and the uc_buffer_st::length member
 *     to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer to contain the nationality
 *     , including NULL terminator, and set the uc_buffer_st::length member to the actual length of data returned.
 *     If the uc_buffer_st::length member is not large enough to hold the entire nationality, the uc_buffer_st::length member
 *     will be set to the minimum required amount, and the function will return ::UC_ERROR_INSUFFICIENT_BUFFER.
 *
 *     \note For simplicity, the application can allocate ::UC_BUFFER_GENERIC_SIZE bytes memory for uc_buffer_st::bytes
 *       and set uc_buffer_st::length to ::UC_BUFFER_GENERIC_SIZE. Cloaked CA Agent will set uc_buffer_st::length to the actual size.
 *
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *    ::UC_ERROR_ALREADY_STOPPED
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_GetSmartcardTMSData(
    uc_uint32 smartcardID,
    uc_buffer_st *pTMSData);

/**
 * Obtain the unique address of smartcard.
 *
 * This function can be called to obtain smartcard unique address.
 *
 * \note The information from this method must be displayed in a GUI dialog.
 *
 * @param[in] smartcardID smartcard id. This parameter is not used.
 * @param[out] pSmartcardUA receives the smartcard Unique Address.
 *      The application must set the uc_buffer_st::bytes member to point to a buffer in memory, and the uc_buffer_st::length
 *      member to the maximum length of the buffer, in bytes. The function will modify the contents of the buffer
 *      to contain the Smartcard Unique Address in binary big-endian format, without any terminating character, and set the
 *      uc_buffer_st::length member to the actual length of data returned. If the uc_buffer_st::length member is not large enough
 *      to hold the entire Smartcard Unique Address, the uc_buffer_st::length member will be set to the minimum required value,
 *      and the function will return ::UC_ERROR_INSUFFICIENT_BUFFER. The application can then allocate memory of a sufficient size
 *      and attempt to call the function again.
 *
 *     \note For simplicity, the application can allocate ::UC_BUFFER_GENERIC_SIZE bytes memory for uc_buffer_st::bytes
 *       and set uc_buffer_st::length to ::UC_BUFFER_GENERIC_SIZE. Cloaked CA Agent will set uc_buffer_st::length to the actual size.
 *     \n
 *     \note
 *     Suppose the Smartcard Unique Address is 12345678 i.e. 0xBC614E, uc_buffer_st::bytes will have the following values: \n
 *     uc_buffer_st::bytes[0] = 0xBC, \n
 *     uc_buffer_st::bytes[1] = 0x61, \n
 *     uc_buffer_st::bytes[2] = 0x4E, \n
 *     and uc_buffer_st::length is set to 3.
 *     \n
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
  * @retval
 *    ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClient_GetSmartcardUniqueAddress(
    uc_uint32 smartcardID,
    uc_buffer_st* pSmartcardUA );


/** @} */ /* End of Unified Client APIs */


#ifdef __cplusplus
}
#endif

#endif /* !UNIVERSALCLIENT_API_H__INCLUDED__ */
