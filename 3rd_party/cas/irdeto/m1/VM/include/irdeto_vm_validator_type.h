#ifndef VM_VALIDATOR_TYPE_H
#define VM_VALIDATOR_TYPE_H

#ifndef NULL
#define NULL 0
#endif

#define uc_bool             int
#define UC_FALSE        0
#define UC_TRUE         1

/*Redefine for less letters.*/
#define UC_strlen                   UniversalClient_Stdlib_strlen
#define UC_strcpy                   UniversalClient_Stdlib_strcpy
#define UC_memcpy               UniversalClient_Stdlib_memcpy
#define UC_memset               UniversalClient_Stdlib_memset
#define UC_memcmp           UniversalClient_Stdlib_memcmp
#define UC_sprintf                  UniversalClient_Stdlib_sprintf
#define UC_printf                   UniversalClient_Stdlib_printf
#define UC_MALLOC           UniversalClientSPI_Memory_Malloc
#define UC_FREE                 UniversalClientSPI_Memory_Free
#define Utils_Calloc(size, ptr)           UniversalClientPriv_Utils_CallocLineNo( size, ptr)

typedef unsigned int    uc_uint32;
typedef unsigned short  uc_uint16;
typedef short           uc_int16;
typedef unsigned char   uc_byte;
typedef char *          uc_string;
typedef char            uc_char;
typedef int             uc_sint32;
typedef unsigned char   uc_uint8;
typedef int             uc_intptr;  /**< Signed int of the same size as a pointer */
typedef unsigned int    uc_uintptr; /**< Unsigned int of the same size as a pointer */  
typedef float						uc_float;

#define offsetof(type,field)	((uc_uint32)(&((type*)0)->field))

#define DEFAULT_MAGIC_NUMBER "CCA"

typedef struct _uc_buffer_st
{
    uc_byte *bytes;
    uc_uint32 length;
} uc_buffer_st;

typedef struct _uc_time
{
    uc_uint32 second;
    uc_uint32 millisecond;
}uc_time;

typedef enum _uc_anchor_type
{
    UC_ANCHOR_SECURE_CHIP           = 0,
    UC_ANCHOR_SECURITY_ID_ONE_WAY   = 1,
    UC_ANCHOR_SECURITY_ID_TWO_WAY   = 2,
    UC_ANCHOR_ACTIVE_NODE_LOCK      = 3
} uc_anchor_type;

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

    /** (0x0000006B) Anchor failed(DeviceID/UA mismactch. For IPSTBs, the application should register again). */
    UC_ERROR_DEVICEID_UA_MISMATCH           = 0x0000006B,

    /** (0x0000006C) VOD not entitled. */
    UC_ERROR_VOD_NOT_ENTITLED               = 0x0000006C,

    /** (0x0000006D) Missing Secure PVR CPSK. */
    UC_ERROR_MISSING_CPSK               = 0x0000006D,

    /** (0x0000006E) Asset ID mismatch. */
    UC_ERROR_ASSET_ID_MISMATCH = 0x0000006E,

    /** (x00000006F) PVR Recording is prohibited due to copy control setting*/
    UC_ERROR_PVR_COPY_CONTROL_PROHIBITED = 0x0000006F,


    /** (x000000070) PVR sharing is prohibited due to is shareable setting*/
    UC_ERROR_HN_PVR_NOT_SHAREABLE = 0x00000070,

    /** (x000000071) PVR sharing is prohibited due to domain id mismatch*/
    UC_ERROR_HN_PVR_NOT_IN_DOMAIN = 0x00000071,

    /** (0x00000072) Fail to process Shared PVRMSK EMM. */
    UC_ERROR_SHARED_PVRMSK_PROCESS_FAIL = 0x00000072,

    /** (0x00000073) White Box Algorithm is invalid. */
    UC_ERROR_INVALID_WB_ALGORITHM = 0x00000073,

    /** (0x00000074) White Box Algorithm is invalid for Pairing EMM. */
    UC_ERROR_INVALID_WB_ALGORITHM_FOR_PAIRING_EMM = 0x00000074,

    /** (0x00000075) White Box Algorithm is invalid for Product Overwrite EMM. */
    UC_ERROR_INVALID_WB_ALGORITHM_FOR_PO_EMM = 0x00000075,

    /** (0x00000076) White Box Algorithm is invalid for Advertisement EMM. */
    UC_ERROR_INVALID_WB_ALGORITHM_FOR_AD_EMM = 0x00000076,

    /** (0x00000077) The chip configuration check is not supported. */
    UC_ERROR_CCC_NOT_SUPPORT = 0x00000077,

    /** (0x00000078) Client registry Failed. */
    UC_ERROR_INITIALIZATION_FAILED = 0x00000078,

    /** (0x00000079) no response for request for a certain time interval. */
    UC_ERROR_CLIENT_REGISTRATION_RESPONSE_TIME_OUT = 0x00000079,

    /** (0x00000080) PIN CODE Limitation Failed. */
    UC_ERROR_PIN_CODE_LIMIT_FAILED = 0x00000080,

    /** (0x00000081) Cannot find current CG's secret private data. */
    UC_ERROR_CG_PRIVATE_DATA_NOT_PAIRING = 0x00000081
} uc_result;

#endif
