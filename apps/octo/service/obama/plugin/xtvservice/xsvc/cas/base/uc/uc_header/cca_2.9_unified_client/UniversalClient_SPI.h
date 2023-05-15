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
 * @file UniversalClient_SPI.h
 * Header for Service Provider Interfaces (SPIs). This file contains the headers for the common Service Provider 
 * Interfaces that must be implemented on a platform. 
 *
 * \note It is the integrator's responsibility to implement these interfaces for a particular platform.
 */

#ifndef UNIVERSALCLIENT_SPI_H__INCLUDED__
#define UNIVERSALCLIENT_SPI_H__INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

#include "UniversalClient_API.h"

/**
 * (0x00000001) Current version of the Cloaked CA Agent SPI
 *
 * This is the value returned from ::UniversalClientSPI_GetVersion. 
 * It is used by the Cloaked CA Agent to ensure that the version of the
 * SPI implementation is not newer than the version of the client. 
 * A newer version of the SPI implementation 
 * is not compatible with an old version of the Cloaked CA Agent. 
 */
#define UNIVERSALCLIENTSPI_VERSION 0x00000001

/* Types */
/**
 * Semaphore handle type
 *
 * This is an implementation-specific handle type that is used in the \ref semaphorespi "Cloaked CA Agent Semaphore SPIs".
 *
 * \see ::UniversalClientSPI_Semaphore_Open
 * \see ::UniversalClientSPI_Semaphore_Close
 * \see ::UniversalClientSPI_Semaphore_Post
 * \see ::UniversalClientSPI_Semaphore_Wait
 * \see ::UniversalClientSPI_Semaphore_WaitTimeout
 */
typedef void *uc_semaphore_handle;

/**
 * Mutex handle type
 *
 * This is an implementation-specific handle type that is used in the \ref mutexspi "Cloaked CA Agent Mutex SPIs".
 *
 * \see ::UniversalClientSPI_Mutex_Open
 * \see ::UniversalClientSPI_Mutex_Close
 * \see ::UniversalClientSPI_Mutex_Unlock
 * \see ::UniversalClientSPI_Mutex_Lock
 */
typedef void *uc_mutex_handle;


/**
 * Thread handle type
 *
 * This is an implementation-specific handle type that is used in the \ref threadspi "Cloaked CA Agent Thread SPIs".
 *
 * \see ::UniversalClientSPI_Thread_Open
 * \see ::UniversalClientSPI_Thread_Sleep
 * \see ::UniversalClientSPI_Thread_Close
 */
typedef void *uc_thread_handle;

/**
 * Thread procedure type
 *
 * This is the prototype of the thread procedure pointer that is passed to ::UniversalClientSPI_Thread_Open. 
 */
typedef void (*uc_threadproc)(void* lpVoid);

/**
 * Copy capability information for copy control.
 *
 * The STB driver should use these values to control actions of content copy.
 */
typedef enum _uc_copycontrol_cci
{
    UC_COPYCONTROL_COPYFREELY = 0x0,/*Copy free:Copy is permitted*/
    UC_COPYCONTROL_NOFURTHERCOPY,   /*Copy No More: No further copying is permitted*/
    UC_COPYCONTROL_COPYONCE,        /*Copy Once: One generation copy is permitted*/
    UC_COPYCONTROL_COPYNEVER,       /*Copy never: Copying is prohibited*/
    UC_COPYCONTROL_COPYINVALID      /*Initial value*/
}uc_copycontrol_cci;


/**
 * Data structure for Copy Control Information, and it's known as CI Plus Uniform Rules Information (URI) Message.
 */
typedef struct _uc_copy_control_info
{
    /**
     * Version of URI, default value is 0x01. 0x00 is an invalid value.
     */
    uc_byte protocol_version;
    /**
     * 2-bit field describes the Analogue Protection System(APS) information. 
     * It is known as CCI analogue bits in ECM Header.
     * 0xFF indicates that this field is invalid.
     */
    uc_byte aps_copy_control_info;
    /**
     * 2-bit field describes the Encryption Mode Indicator(EMI) information,
     * It is known as CCI digital bits in ECM Header.
     * 0xFF indicates that this field is invalid.
     */
    uc_byte emi_copy_control_info;
    /**
     * 1-bit field describes the Image Constrained Trigger(ICT) information.
     * 0xFF indicates that this field is invalid.
     */
    uc_byte ict_copy_control_info;
    /**
     * 1-bit field describes the Encryption Plus Non-assert(RCT) information.
     * 0xFF indicates that this field is invalid.
     */
    uc_byte rct_copy_control_info;
    /**
     * This field describes the rerention limit after the recording 
     * and/or time-shift of an event is completed.
     * If protocol_version=1, it is 6 bits; If protocol_version=2, it is 8 bits.
     * 0xFF indicates that this field is invalid, with one exception, which is if
     * protocol_version=2 and emi_copy_control_info=3, 0xFF indicates unlimited
     * retention period.
     */
    uc_byte rl_copy_control_info;
    /**
     * 1-bit field describes the Digital Only Token(DOT) information.
     * 0xFF indicates that this field is invalid.
     */
    uc_byte dot_copy_control_info;
}uc_copy_control_info;

/**
 * Data structure for Macrovision mode. 
 */
typedef struct _uc_macrovision_mode
{
    /**
     *Only lower 6 bits are valid.
     */
    uc_byte mode;
} uc_macrovision_mode;

/**
 * Data structure for Macrovision configuration data.
 */
typedef struct _uc_macrovision_config
{
    /**
     *Version of macrovision, version=7 by default
     */
    uc_byte macrovisionVersion;
    /**
     *Configure data for macrovision
     */
    uc_buffer_st macrovisionConfig;
} uc_macrovision_config;

/**
 * Fatal error type.
 */
typedef enum _uc_fatal_error_type
{
    /**
     *Out of memory.
     */
    UC_FATAL_ERROR_MEMORY = 0x0,
    
    /**
     *Persistent storage is damaged.
     */
    UC_FATAL_ERROR_PS_DAMAGED = 0x1
}uc_fatal_error_type;

/**
 * Data structure for smartcard interfaces. 
 */

/** Type of smartcard errors.
 *
 */
typedef enum _uc_smartcard_error_type
{
    /** An error has happened on communications. 
     */
    UC_SC_COMM_ERROR,

    /** Some hardware error occurred. 
     */
    UC_SC_HARDWARE_ERROR,

    /** The card has been removed from the reader. 
     */
    UC_SC_CARD_OUT_ERROR,

    /** The comuniations have timed-out. 
     */
    UC_SC_TIME_OUT,

    /** The ATR received from the card is not up to the ISO7816-3 spec. 
     */
    UC_SC_ATR_INVALID_FORMAT,

    /** The ATR received from the card is not an Irdeto Access ATR. 
     */
    UC_SC_ATR_INVALID_HISTORY,

    /** JavaCard does not support Logical Channels.
     */
    UC_SC_NO_LOGICAL_CHAN_SUPPORT
} uc_smartcard_error_type;

/**
 * The error info for smartcard.
 */
typedef struct _uc_smartcard_error_info
{
    /** The card error type.
     */
    uc_smartcard_error_type cardErrorType;    
} uc_smartcard_error_info;

/** The smartcard ATR info
 * 
 */
typedef struct _uc_smartcard_ATR_info
{
    /** Length of the ATRData in bytes.
     */
    uc_uint16 ATRLength;

    /** The ATR data.
     * The buffer of ATRData shall be managed by application.
     */
    uc_byte* ATRData;
} uc_smartcard_ATR_info;

/** ATR info for Javecard channel.
 * 
 */
typedef struct _uc_smartcard_ATR_java_card_channel_info
{
    /** The ATR Info.
     * In some cases where the low-level SD-card smartcard driver does not send an ATR, the ATR that can be 
     * used to inform CloakedCA Agent is 0x3B 0x80 0x01 0x81
     */
    uc_smartcard_ATR_info ATRInfo;

    /** The channel number.
     */
    uc_byte channelNumber;
} uc_smartcard_ATR_java_card_channel_info;

/** The type of the smartcard status.
 *
 * This is used in the callback method provided by CloakedCA Agent to application ::uc_sc_status_notify_fn.
 * The corresponding pvInfo parameter in that callback method is also defined here.
 */
typedef enum _uc_sc_status_type
{
    /**
     * A card is inserted into the slot.
     * 
     * The pvInfo carries the ATR read from smartcard:
     * pvInfo.bytes points to an instance of ::uc_smartcard_ATR_info and pvInfo.length is the length of the instance.
     */
    UC_SC_CARD_IN,
    
    /**
     * A card is removed from the slot.
     * 
     * The pvInfo parameter shall be set to NULL.
     */
    UC_SC_CARD_OUT,
    
    /**
     * An error has occurred on insertion of the card.
     *
     * The pvInfo carries the actual error info:
     * pvInfo.bytes points to an instance of ::uc_smartcard_error_info and pvInfo.length is set to the
     * length of the instance.
     * 
     */
    UC_SC_CARD_IN_ERROR,
    
    /**
     * A JavaCard / SIM smartcard has been inserted.
     *
     * This is only applicable to JavaCards.
     *
     * The pvInfo carries the ATR info and Javecard channel number:
     * pvInfo.bytes points an instance of ::uc_smartcard_ATR_java_card_channel_info, and pvInfo.length is set to
     * the length of the instance.
     */
    UC_SC_JAVACARD_CHANNEL_OPENED
} uc_sc_status_type;

/**
 * Handle to a smartcard object 
 *
 * A smartcard handle is an implementation-specific type that is returned from
 * a call to ::UniversalClientSPI_Smartcard_Open. 
 * A smartcard object is a logical construct that represents a source smartcard. 
 */
typedef uc_uintptr uc_smartcard_handle;

/**
 * To save time info.
 *
 * Cloaked CA Agent currently uses the time info to evaluate the performance.
 * The time info can be a real time or the time since reboot. 
 */
typedef struct _uc_time
{
    /**
     * seconds of current time.
     */
    uc_uint32 second;

    /**
     * millisecond of current time.NOTE:millisecond but NOT microsecond!
     */
    uc_uint32 millisecond;
} uc_time;

/**
 * cryptography  type enumeration.
 *
 * This type is used by the ::UniversalClientSPI_Crypto_Verification to denote the cryptography algorithm 
 * type which used in manufacturer provided verification process. 
 */

 typedef enum
 {
    /**
     * UNKNOWN asymmetric cryptography
     */
    UC_CRIPYTO_UNKNOWN = 0,
 
    /**
     * RSA asymmetric cryptography
     */
    UC_CRYPTO_RSA = 1
 }uc_crypto_type;

/**
 *max Length of rsa modulus.
 *
 *Define the max size of rsa MODULUS that support by rsa key currently. 
 */
#define MAX_RSA_MODULUS_LEN 256

/**
 *max Length of rsa exponent.
 *
 *Define the max size of rsa EXPONENT that support by rsa key currently. 
 */
#define MAX_RSA_EXPONENT_LEN 256


/**
 *RSA algorithm mode been used.
 *
 *Currently, only SHA256 is supported.
 */
typedef enum
{
  /**
   *SHA256 mode
   */
   UC_SHA_ALGORITHM_SHA256  =  0x0
}uc_sha_algorith_mode;

/**
 *RSA algorithm mode been used for digital signature verification.
 *
 *Currently, only PSS is supported.
 */
typedef enum _uc_rsa_alg_mode
{
  /**
   *PSS mode
   */
    UC_RSASSA_PSS      =   0x0
}uc_rsa_algorith_mode;

/**
 *RSA key infomation for the rsa digital signature verification.
 *
 *As a part of uc_rsa_st to provide key info.
 */
typedef struct _uc_rsa_pk_st
{
  /**
   *length of modulus in byte
   */
    uc_uint32  modulusLength;                              

  /**
   *public/private key's Modulus
   */
    uc_byte modulus[MAX_RSA_MODULUS_LEN];    
 
   /**
   *length of Exponent in byte
   */
    uc_uint32  exponentLength;   

  /**
   *public/private key's Exponent
   */
    uc_byte exponent[MAX_RSA_EXPONENT_LEN];  
} uc_rsa_pk_st;



/**
  *RSA infomation for the RSA digital signature verification.
  *
  *If the cryptoType equals to UC_CRYPTO_RSA, this structure will be used as the parameter be
  *passed to UniversalClientSPI_Crypto_Verify
  */
typedef struct _uc_rsa_st
{
    /**
     *data to be signed
     */
    uc_byte*      pDataToBeSigned;     

    /**
     *size of data to be signed, i.e. buffer size of pDataToBeSigned.
     */
    uc_uint32       dataLength;   

    /**
     *signature for data in the buffer pDataToBeSigned.
     */
    const uc_byte*      pSignature;  

    /**
     *size of signature, i.e. buffer size of pSignature.
     */
    uc_uint32       signatureLength;

    /**
     * key for the signature
     */
    uc_rsa_pk_st*           pKey;       

    /**
     *RSA algorithm mode
     */
    uc_rsa_algorith_mode        rsaMode;    

    /**
     *SHA algorithm mode
     */
    uc_sha_algorith_mode        shaMode;  

}uc_rsa_st;

/**
 * cryptography information structure
 *
 * This is the structure containing cryptography information, which is used by the manufacturer to provide verification.
 * This structure is used by ::UniversalClientSPI_Crypto_Verify.
 */
 typedef struct _uc_crypto_info
 {
    /**
     * crypto type
     */
    uc_crypto_type cryptoType;

    /**
     * parameter of crypto info can change according to crypto algorithm.
     *
     * currently, only RSA crypto algorithm is supported,.
     */
    union
    {
        /**
         * if  cryptoType equals to UC_CRYPTO_RSA. 
         */
        uc_rsa_st rsaInfo;
    }param;
 }uc_crypto_info;


/**
 * Device security state structure
 *
 * This is a structure that is passed to a stream implementation from 
 * a call to ::UniversalClientSPI_Device_GetSecurityState. It contains security state information about
 * the device. 
 * \note The status data of RSA Mode, JTAG and CW Mode should be read from secure chipset registers.
 */
typedef struct _uc_device_security_state
{
    /**
     * Status of the secure boot loader.
     *
     * Set the lowest significant bit to 1 if the secure boot is enabled,
     * else secure boot is NOT enabled.
     */
    uc_uint32 rsaMode;

    /**
     * Status of the JTAG port.
     *
     * 0: open
     * 1: locked (with key or password)
     * 2: disabled permanently
     */
    uc_uint32 jtag;

    /**
     * Type of CW the descrambler accepts.
     *
     * Set the lowest sinificant bit to 1 if only encrypted CWs can be loaded (secure CW mode) to descramblers.
     */
    uc_uint32 cwMode;

    /**
     * crypto type for protecting key.
     *
     * 0: Not support secure chip.
     * 1: TDES algorithm with 128-bit key support.
     */
    uc_byte crypto;

} uc_device_security_state;


/**
 * Device platform identifiers structure
 *
 * This is a structure that is passed to a stream implementation from 
 * a call to ::UniversalClientSPI_Device_GetPlatformIdentifiers. It contains platform identifiers about
 * the device. 
 * \note Some platforms may not have all the information in this structure if no Irdeto Loader exists.
 */
typedef struct _uc_device_platform_identifiers
{
    /**
     * System id from loader.
     */
    uc_uint16 systemId;

    /**
     * Variant from loader.
     */    
    uc_uint16 variant;

    /**
     * Key version from loader.
     */ 
    uc_uint16 keyVersion;

    /**
     * Signature version from loader.
     */
    uc_uint16 signatureVersion;
    
    /**
     * Manufacturer ID from loader.
     */
    uc_uint16 manufacturerId;

    /**
     * Loader version from loader.
     * The most significant 8 bits stands for Loader Major Version whereas the least significant 8 bits stands for
     * Loader Minor Version.
     */
    uc_uint16 loaderVersion;

    /**
     * Hardware version from loader.
     */
    uc_uint16 hardwareVersion;

    /**
     * Download version number from loader.
     */
    uc_uint16 loadVersion;

} uc_device_platform_identifiers;


/**
 * Function pointer type used to notify the client of the smartcard status.
 *
 * A function pointer of this type is passed  via the ::UniversalClientSPI_Smartcard_Open
 * method. The implementation is expected to send the current status of smartcard to Cloaked CA Agent.
 *
 *
 * @param[in] smartcardID the smartcard id previously return from the ::UniversalClientSPI_Smartcard_Open method. 
 * @param[in] smartcardHandle the smartcard handle previously passed by the ::UniversalClientSPI_Smartcard_Open method.
 * @param[in] reason the notification reason, see ::uc_sc_status_type.
 * @param[in] pvInfo Buffer containing data to send to Cloaked CA Agent. The uc_buffer_st::bytes member points to 
 *            the payload of the info, and the uc_buffer_st::length member contains the number of bytes of the info
 *            to send. The definition of format of the payload info can be found in ::uc_sc_status_type.
 */
typedef void (* uc_sc_status_notify_fn)(
            uc_uint32 smartcardID,
            uc_smartcard_handle smartcardHandle,
            uc_sc_status_type reason, 
            uc_buffer_st *pvInfo);

/**
 * Data structure of notification function for the smartcard interfaces. 
 */                    
typedef struct _uc_sc_open_parameter
{
    /**
     * notification function, this function shall be called to notify the changed smartcard status 
     * to Cloaked CA Agent.
     */
    uc_sc_status_notify_fn    pfnStatusNotify;
    /**
     * smartcard handle associated with this open operation.
     */
    uc_smartcard_handle       smartcardHandle;
} uc_sc_open_parameter;

/**
 * Function pointer type used to notify the client with a timeout event.
 *
 * A function pointer of this type is passed to the implementation of a timer via the ::UniversalClientSPI_Timer_Open
 * method. The timer implementation is expected to notify a timeout event.
 *
 *
 * @param[in] timerId The id of the timer which has triggered a timeout event. 
 * @param[in] timerContext timer context parameter which is passed to the implementation via ::UniversalClientSPI_Timer_Open 
 */
typedef void (*uc_timer_callback)(
    uc_uint32 timerId,
    void * timerContext);

/**
 * Data structure of notification function for the timer interfaces. 
 */           
typedef struct _uc_timer_info
{
    /**
     * Timer context, this should be sent back to Cloaked CA via the notification function \b onTimerCallBack.
     */
    void * timerContext;

    /**
     * Specify the timeout value, in milliseconds.
     */
    uc_uint32 timeout;

    /**
     * Indicate if the timer is repetitive.
     * If the timer is repetitive, once the timeout event happens, the implementation should notify Cloaked CA Agent 
     * and reclock to wait the next timeout. 
     * The process will go on until ::UniversalClientSPI_Timer_Close or ::UniversalClientSPI_Timer_Stop is called.
     */
    uc_bool isRepetitive;

    /**
     * Notification function, this function shall be called to notify the timeout event 
     * to Cloaked CA Agent.
     */
    uc_timer_callback onTimeCallback;
}uc_timer_info;

/**
 * Data structure for the soapAction and its payload data.
 *
 */
typedef struct _uc_web_service_info
{
    /**
     * SOAP Action
     *
     * The action for this request.
     *
     * bytes in soapAction points to a null-terminated string, e.g. "SOAPAction: "http://irdeto.com/ccis/requestEntitlement""
     *
     * length in soapAction equals to strlen("SOAPAction: "http://irdeto.com/ccis/requestEntitlement"").
     */
    uc_buffer_st soapAction;
    /**
     * Message data
     *
     * The XML message is the payload for this request.
     *
     * bytes in messageData points to a null-terminated string, e.g. "<?xml version="1.0" encoding="utf-8"?> <soap:Envelope><soap:Body><clientRegister><deviceId>000000000000</deviceId><Pin>00000000</Pin></clientRegister></soap:Body></soap:Envelope>"
     *
     * length in messageData equals to strlen("<?xml version="1.0" encoding="utf-8"?> <soap:Envelope><soap:Body><clientRegister><deviceId>000000000000</deviceId><Pin>00000000</Pin></clientRegister></soap:Body></soap:Envelope>").
     */
    uc_buffer_st messageData;
}uc_web_service_info;


/**
 * Data structure for stream send ::UniversalClientSPI_Stream_Send.
 *
 */
typedef struct _uc_stream_send_payload
{
    /**
     * Indicates the type of stream.
     *
     * This determines which members of the union is valid.
     * Now the type is only ::UC_CONNECTION_STREAM_TYPE_IP, hence the union's member must be ::uc_web_service_info.
     */
    uc_connection_stream_type streamType;
    /**
     * Lists all the members of the union.
     */
    union
    {
         /**
           * The payload sends to web service.
           */
        uc_web_service_info webServciePayload;
    } payload;
} uc_stream_send_payload;

/** @defgroup universalclient_spi Cloaked CA Agent SPIs

 *  Cloaked CA Agent SPIs
 *
 *  Service Provider Interfaces(SPIs) are driver-level APIs called by Cloaked CA Agent.
 *  Client device platform must implement these SPIs in order to run Cloaked CA Agent.
 *
 *  \note In the SPI implementation, APIs shall not be called to avoid dead lock between the 
 *  application and Cloaked CA Agent thread.
 *  @{
 */

/** @page allspis Cloaked CA Agent SPIs
 * Cloaked CA Agent SPIs
 *
 * Service Provider Interfaces(SPIs) are the components that must be implemented on a per-platform basis.
 * These SPIs are grouped into sections according to 
 * common functionality:
 *
 * - \ref memoryspi "Cloaked CA Agent Memory SPIs"
 * - \ref semaphorespi "Cloaked CA Agent Semaphore SPIs"
 * - \ref mutexspi "Cloaked CA Agent Mutex SPIs"
 * - \ref threadspi "Cloaked CA Agent Thread SPIs"
 * - \ref psspi "Cloaked CA Agent Persistent Storage SPIs"
 * - \ref devicespi "Cloaked CA Agent Device SPIs"
 * - \ref pvrspi "Cloaked CA Agent PVR SPIs"
 * - \ref streamspi "Cloaked CA Agent Stream SPIs"
 * - \ref copycontrolspi "Cloaked CA Agent Copy Control SPIs"
 * - \ref smartcardspi "Cloaked CA Agent Smartcard SPIs"
 * - \ref datetimespi "Cloaked CA Agent Date Time SPIs"
 *
 * In order to indicate which SPIs are implemented, the integrator MUST
 * implement the top-level SPIs 
 * ::UniversalClientSPI_GetVersion and ::UniversalClientSPI_GetImplementation.
 */


/** @defgroup toplevelspi Cloaked CA Agent Top Level SPIs
 *  Cloaked CA Agent Top Level SPIs
 *
 *  These SPIs implement a version compatibility mechanism that allows
 *  new version releases of the Cloaked CA Agent API to be linked with
 *  an existing SPI implementation without having to update the SPI implementation
 *  in order to build. 
 * 
 * \note It is the integrator's responsibility to implement these methods for a particular platform.
 *
 *  @{
 */

/**
 * Get the current version of the Cloaked CA Agent SPI.
 *
 * This function is called when the client is started is called in order to check
 * that the version of the SPI implementation is compatible with the version of the client.
 *
 * This function MUST return #UNIVERSALCLIENTSPI_VERSION. Recompiling an existing SPI implementation
 * will update this version automatically. 
 */
uc_uint32 UniversalClientSPI_GetVersion(void);

/**
 * SPI implementation dispatch table
 *
 * This structure is passed to ::UniversalClientSPI_GetImplementation,
 * and must be filled out by the integrator. Each member of 
 * this structure must be set to the implementation of the
 * corresponding methods. Any methods not explicitly set in this structure
 * will be considered 'not implemented' by the Cloaked CA Agent and ignored. 
 */
typedef struct _uc_spi_implementation_st
{
    /**
     * See ::UniversalClientSPI_Memory_Malloc
     */
    void* (*Memory_Malloc)(uc_uint32 bytes);

    /**
     * See ::UniversalClientSPI_Memory_Free
     */
    void (*Memory_Free)(void * lpVoid);

    /**
     * See ::UniversalClientSPI_Semaphore_Open
     */
    uc_result (*Semaphore_Open)(uc_uint32 initialValue, 
                                             uc_semaphore_handle * pSemaphoreHandle);

    /**
     * See ::UniversalClientSPI_Semaphore_Post
     */
    uc_result (*Semaphore_Post)(uc_semaphore_handle semaphoreHandle);

    /**
     * See ::UniversalClientSPI_Semaphore_Wait
     */
    uc_result (*Semaphore_Wait)(uc_semaphore_handle semaphoreHandle);

    /**
     * See ::UniversalClientSPI_Semaphore_Close
     */
    uc_result (*Semaphore_Close)(uc_semaphore_handle * pSemaphoreHandle);

    /**
     * See ::UniversalClientSPI_Mutex_Open
     */
    uc_result (*Mutex_Open)(uc_mutex_handle * pMutexHandle);

    /**
     * See ::UniversalClientSPI_Mutex_Lock
     */
    uc_result (*Mutex_Lock)(uc_mutex_handle mutexHandle);

    /**
     * See ::UniversalClientSPI_Mutex_Unlock
     */
    uc_result (*Mutex_Unlock)(uc_mutex_handle mutexHandle);

    /**
     * See ::UniversalClientSPI_Mutex_Close
     */
    uc_result (*Mutex_Close)(uc_mutex_handle * pMutexHandle);


    /**
     * See ::UniversalClientSPI_Thread_Open
     */
    uc_result (*Thread_Open)(uc_threadproc threadProc, 
                                       void* lpVoid, uc_thread_handle * pThreadHandle);
    /**
     * See ::UniversalClientSPI_Thread_Sleep
     */
    uc_result (*Thread_Sleep)(uc_thread_handle hThreadHandle, uc_uint16 wDelay);

    /**
     * See ::UniversalClientSPI_Thread_Close
     */
    uc_result (*Thread_Close)(uc_thread_handle * pThreadHandle);

    /**
     * See ::UniversalClientSPI_PS_Delete
     */
    uc_result (*PS_Delete)(uc_uint32 index);

    /**
     * See ::UniversalClientSPI_PS_Write
     */
    uc_result (*PS_Write)(uc_uint32 index, uc_uint32 offset, const uc_buffer_st *pData);

    /**
     * See ::UniversalClientSPI_PS_Read
     */
    uc_result (*PS_Read)(uc_uint32 index, uc_uint32 offset, uc_buffer_st *pData);

    /**
     * See ::UniversalClientSPI_PS_Initialize
     */
    uc_result (*PS_Initialize)(void);

    /**
     * See ::UniversalClientSPI_PS_Terminate
     */
    uc_result (*PS_Terminate)(void);

    /**
     * See ::UniversalClientSPI_Device_GetCSSN
     */
    uc_result (*Device_GetCSSN)(uc_buffer_st * pData);

    /**
     * See ::UniversalClientSPI_Device_SetCSSK
     */
    uc_result (*Device_SetCSSK)(const uc_buffer_st * pKeyMaterial);

    /**
     * See ::UniversalClientSPI_Device_SetExtendedCSSK
     */
    uc_result (*Device_SetExtendedCSSK)(const uc_cssk_info * pCSSKInfo);

    /**
     * See ::UniversalClientSPI_Stream_Open
     *
     * \note ::UniversalClientSPI_Stream_Open is a special SPI.
     * For DVB client, the prototype for this SPI in fact is
     * ::uc_result ::UniversalClientSPI_Stream_Open(
     *       ::uc_connection_stream_type \b streamType,
     *       \b const ::uc_stream_open_params \b *pStreamOpenParams,
     *       ::uc_stream_handle \b *pStreamHandle);
     * The SPI implementation can make necessary type convertion for this SPI if there is any compile warning.
     *
     */
    uc_result (*Stream_Open)(
        uc_connection_stream_type streamType,
        void *pStreamOpenParams,
        uc_stream_handle *pStreamHandle);

    /**
     * See ::UniversalClientSPI_Stream_AddComponent
     *
     * \note ::UniversalClientSPI_Stream_AddComponent is a special SPI.
     * For DVB client, the prototype for this SPI in fact is
     * ::uc_result ::UniversalClientSPI_Stream_AddComponent(
     *    ::uc_stream_handle \b streamHandle,
     *    \b const ::uc_elementary_component \b *pComponent);
     * The SPI implementation can make necessary type convertion for this SPI if there is any compile warning.
     *
     */
    uc_result (*Stream_AddComponent)(
        uc_stream_handle streamHandle,
        const uc_component *pComponent);

    /**
     * See ::UniversalClientSPI_Stream_RemoveComponent
     *
     * \note ::UniversalClientSPI_Stream_RemoveComponent is a special SPI.
     * For DVB client, the prototype for this SPI in fact is
     * ::uc_result ::UniversalClientSPI_Stream_RemoveComponent(
     *   ::uc_stream_handle \b streamHandle,
     *   \b const ::uc_elementary_component \b *pComponent);
     * The SPI implementation can make necessary type convertion for this SPI if there is any compile warning.
     *
     */
    uc_result (*Stream_RemoveComponent)(
        uc_stream_handle streamHandle,
        const uc_component *pComponent);

    /**
     * See ::UniversalClientSPI_Stream_Start
     */
    uc_result (*Stream_Start)(uc_stream_handle streamHandle);

    /**
     * See ::UniversalClientSPI_Stream_Stop
     */
    uc_result (*Stream_Stop)(uc_stream_handle streamHandle);

    /**
     * See ::UniversalClientSPI_Stream_OpenFilter
     */
    uc_result (*Stream_OpenFilter)(
        uc_stream_handle streamHandle,
        uc_filter_handle *pFilterHandle);

    /**
     * See ::UniversalClientSPI_Stream_SetFilter
     */
    uc_result (*Stream_SetFilter)(
        uc_filter_handle filterHandle,
        const uc_filter *pFilterRules);

    /**
     * See ::UniversalClientSPI_Stream_CloseFilter
     */
    uc_result (*Stream_CloseFilter)(
        uc_stream_handle streamHandle,
        uc_filter_handle *pFilterHandle);

    /**
     * See ::UniversalClientSPI_Stream_Connect
     */
    uc_result (*Stream_Connect)(
        uc_stream_handle streamHandle,
        uc_connection_handle connectionHandle,
        uc_notify_callback notifyCallback);

    /**
     * See ::UniversalClientSPI_Stream_Disconnect
     */
    uc_result (*Stream_Disconnect)(
        uc_stream_handle streamHandle,
        uc_connection_handle connectionHandle);

    /**
     * See ::UniversalClientSPI_Stream_SetDescramblingKey
     */
    uc_result (*Stream_SetDescramblingKey)(
        uc_stream_handle streamHandle,
        const uc_key_info *pKeyInfo,
        uc_uint32 keyVersion);

    /**
     * See ::UniversalClientSPI_Stream_CleanDescramblingKey
     */
    uc_result (*Stream_CleanDescramblingKey)(
        uc_stream_handle streamHandle);        

    /**
     * See ::UniversalClientSPI_Stream_Close
     */
    uc_result (*Stream_Close)(
        uc_stream_handle *pStreamHandle);

    /**
     * See ::UniversalClientSPI_Device_GetPrivateData
     */
    uc_result (*Device_GetPrivateData)(uc_buffer_st * pData);


    /**
     * See ::UniversalClientSPI_Device_GetSecurityState
     */
    uc_result (*Device_GetSecurityState)(uc_device_security_state * pDeviceSecurityState);

    /**
     * See ::UniversalClientSPI_Device_GetPlatformIdentifiers
     */
    uc_result (*Device_GetPlatformIdentifiers)(uc_device_platform_identifiers * pDevicePlatformIdentifiers);

    /**
     * See ::UniversalClientSPI_PVR_SetSessionKey
     */
   uc_result (*PVR_SetSessionKey)(uc_stream_handle streamHandle, const uc_buffer_st *pPVRSessionKey);

    /**
     * See ::UniversalClientSPI_PVR_SetExtendedSessionKey
     */
   uc_result (*PVR_SetExtendedSessionKey)(uc_stream_handle streamHandle, const uc_pvrsk_info *pPVRSKInfo);

    /**
     * See ::UniversalClientSPI_CopyControl_Macrovision_SetConfig
     */
   uc_result (*CopyControl_Macrovision_SetConfig)(uc_macrovision_config mac_config_data);
    /**
     * See ::UniversalClientSPI_CopyControl_Macrovision_SetMode
     */
   uc_result  (*CopyControl_Macrovision_SetMode)(uc_stream_handle handle,uc_macrovision_mode mac_mode);
    /**
     * See ::UniversalClientSPI_CopyControl_SetCCI
     */
   uc_result  (*CopyControl_SetCCI)(uc_stream_handle streamHandle,uc_copy_control_info* pCopyControlInfo);
   /**
     * See ::UniversalClientSPI_FatalError
     */
   void  (*FatalError)(uc_fatal_error_type type,void* lpVoid);
   /**
     * See ::UniversalClientSPI_Smartcard_Open
     */
   uc_result  (*Smartcard_Open)(uc_uint32 *pSmartcardID, uc_sc_open_parameter *pSCOpenData);
   /**
     * See ::UniversalClientSPI_Smartcard_Close
     */
   uc_result  (*Smartcard_Close)(uc_uint32 smartcardID);
   /**
     * See ::UniversalClientSPI_Smartcard_Reset
     */
   uc_result  (*Smartcard_Reset)(uc_uint32 smartcardID);
   /**
     * See ::UniversalClientSPI_Smartcard_Communicate
     */ 
   uc_result  (*Smartcard_Communicate)(uc_uint32 smartcardID, uc_uint32 headerLen, uc_uint32 payloadLen,uc_byte *pSendBuffer, uc_uint32 *pRecvDataLen, uc_byte *pRecvBuffer );

    /**
      * See ::UniversalClientSPI_DateTime_GetTimeOfDay. Optional!
      */ 
    uc_result  (*DateTime_GetTimeOfDay)(uc_time *pCurrentTime);

    /**
      * See ::UniversalClientSPI_Crypto_Verify. Optional!
      */ 
    uc_result  (*Crypto_Verify)(uc_crypto_info *pCryptoInfo);

    /**
     * See ::UniversalClientSPI_Device_GetPersonalizedData
     */
    uc_result (*Device_GetPersonalizedData)(uc_buffer_st* pData);

    /**
     * See ::UniversalClientSPI_Timer_Open
     */
    uc_result (*Timer_Open)(uc_uint32* pTimerId,uc_timer_info* pTimerInfo);

    /**
     * See ::UniversalClientSPI_Timer_Close
     */
    uc_result (*Timer_Close)(uc_uint32 timerId);

    /**
     * See ::UniversalClientSPI_Timer_Start
     */
    uc_result (*Timer_Start)(uc_uint32 timerId);

    /**
     * See ::UniversalClientSPI_Timer_Stop
     */
    uc_result (*Timer_Stop)(uc_uint32 timerId);

    /**
     * See ::UniversalClientSPI_Semaphore_WaitTimeout
     */
    uc_result (*Semaphore_WaitTimeout)(uc_semaphore_handle semaphoreHandle, uc_uint32 milliseconds);

    /**
     * See ::UniversalClientSPI_Device_GetDeviceID
     */
    uc_result (*Device_GetDeviceID)(uc_buffer_st * pData);

    /**
     * See ::UniversalClientSPI_Device_GetIPAddress
     */
    uc_result (*Device_GetIPAddress)(uc_buffer_st * pData);

    /**
     * See ::UniversalClientSPI_Stream_Send
     */
    uc_result (*Stream_Send)(
     uc_stream_handle streamHandle,
     const uc_stream_send_payload *pBytes);

} uc_spi_implementation_st;

/**
 * Get the list of implemented methods
 *
 * This method is called when the client is started in order to 
 * get the list of SPI methods currently available on the platform.
 *
 * @param [in,out] pImpl On input, this structure is zeroed out. 
 *    The implementation MUST populate this structure in order to successfully
 *    integrate with the platform.
 *
 */
extern void UniversalClientSPI_GetImplementation(
    uc_spi_implementation_st *pImpl);

/** @} */ /* End of toplevelspi*/

/** @defgroup memoryspi Cloaked CA Agent Memory SPIs
 *  Cloaked CA Agent Memory SPIs
 *
 *  These SPIs implement basic memory management functionality required for most operations.  
 * 
 * \note It is the integrator's responsibility to implement these methods for a particular platform.
 *
 *  @{
 */
/**
 * Allocates a block of uninitialized memory. 
 *
 * This function is called to allocate a block of memory and return a pointer to the beginning
 * of the block. If the system is out of memory, or another failure occurs, it must return NULL.
 * When the memory is no longer needed, ::UniversalClientSPI_Memory_Free will be called to free it.
 *
 * @param[in] bytes Number of bytes to allocate. If this is 0, the implementation MUST return an empty block
 *     of allocated memory.
 */
extern void* UniversalClientSPI_Memory_Malloc(uc_uint32 bytes);
/**
 * Frees a block of memory.
 *
 * This function is called to free a block of memory previously allocated using ::UniversalClientSPI_Memory_Malloc.
 *
 * @param[in] lpVoid Pointer to a block of previously allocated memory. If this is NULL, the implementation
 *     MUST handle it gracefully and ignore it. If this points to an area of memory that was not previously
 *     allocated by ::UniversalClientSPI_Memory_Malloc, OR it has already been freed, the result is undefined.
 */
extern void UniversalClientSPI_Memory_Free(void * lpVoid);

/**
 * Report a fatal error.
 *
 * This function is called to report a fatal error.
 * The device application doesn't need to take any specific operation when it's triggered. 
 *
 * @param[in] type Error code, see ::uc_fatal_error_type.
 * @param[in] lpVoid Pointer to the null-terminated string to describe this error.
 */
extern void  UniversalClientSPI_FatalError(uc_fatal_error_type type,void* lpVoid);

/** @} */ /* End of memory */

/** @defgroup semaphorespi Cloaked CA Agent Semaphore SPIs
 *  Cloaked CA Agent Semaphore SPIs
 *
 * These SPIs implement basic thread synchronization, semaphores.
 *
 * \note It is the integrator's responsibility to implement these methods for a particular platform.
 *
 *  @{
 */
/**
 * Create a semaphore
 *
 * This function is called to create a semaphore.
 *
 * A 'semaphore' is a basic synchronization object with the following properties:
 * - It maintains a value, which is used internally to determine when to unblock waiters.
 * - Calling ::UniversalClient_Semaphore_Post atomically increments the value.
 * - Calling ::UniversalClient_Semaphore_Wait waits until the value > 0, then atomically decrements the value. 
 *
 * Semaphores are used by the Cloaked CA Agent to signal when events occur and to protect
 * data structures from simultaneous access.
 *
 * @param[in] initialValue Initial value for the semaphore.
 * @param[out] pSemaphoreHandle Receives a handle to a newly created semaphore. When the semaphore is no longer needed,
 *     it will be closed using ::UniversalClientSPI_Semaphore_Close. 
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_Semaphore_Open(uc_uint32 initialValue, 
                                             uc_semaphore_handle * pSemaphoreHandle);
/**
 * Increment the semaphore value
 *
 * This function is called to atomically increment the value of a semaphore. 
 * If the value goes above 0, this should cause threads waiting in a call to 
 * ::UniversalClientSPI_Semaphore_Wait to be unblocked accordingly. 
 *
 * @param[in] semaphoreHandle Handle of semaphore previously created using ::UniversalClientSPI_Semaphore_Open.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_Semaphore_Post(uc_semaphore_handle semaphoreHandle);
/**
 * Decrement the semaphore value
 *
 * This function is called to atomically decrement the value of a semaphore.
 * It waits for the semaphore value to be greater than 0, then atomically decrements the value.
 * For example, if there are 3 threads blocked waiting on the semaphore, the value will be 0. 
 * If another thread calls ::UniversalClientSPI_Semaphore_Post, it will increment the value to 1, and
 * ONE of the threads will be unblocked so that it can decrement the value back to 0. The other two
 * threads remain blocked. 
 * 
 *
 * @param[in] semaphoreHandle Handle of semaphore previously created using ::UniversalClientSPI_Semaphore_Open.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_Semaphore_Wait(uc_semaphore_handle semaphoreHandle);

/**
 * Decrement the semaphore value with timeout interval.
 *
 * This function is called to atomically decrement the value of a semaphore.
 * It waits for the semaphore value to be greater than 0, then atomically decrements the value.
 * For example, if there are 3 threads blocked waiting on the semaphore, the value will be 0. 
 * If another thread calls ::UniversalClientSPI_Semaphore_Post, it will increment the value to 1, and
 * ONE of the threads will be unblocked so that it can decrement the value back to 0. The other two
 * threads remain blocked. 
 * 
 *
 * @param[in] semaphoreHandle Handle of semaphore previously created using ::UniversalClientSPI_Semaphore_Open.
 * @param[in] milliseconds : The time-out interval, in milliseconds. If a nonzero value is specified, the function waits until the 
 * object is signaled or the interval elapses. 
 * If milliseconds are zero, the function does not enter a wait state if the object is not signaled; it always returns immediately.
 * If milliseconds is 0xffffffff (INFINITE), the function will return only when the object is signaled, this is exactly identical with
 * ::UniversalClientSPI_Semaphore_Wait. 
 *
 * \note This SPI is for PULL EMM. \n
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_WAIT_ABORTED
 * @retval
 *     ::UC_ERROR_OS_FAILURE
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 *
 *\note ::UC_ERROR_SUCCESS , this error code means the semaphore is signaled without timeout.
 *      ::UC_ERROR_NULL_PARAM , this error code means null input parameters.
 *      ::UC_ERROR_WAIT_ABORTED , this error code means timeout(time value of milliseconds)for the semaphore waiting.
 *      ::UC_ERROR_OS_FAILURE , this error code means error happened for the semaphore waiting.
 */
extern uc_result UniversalClientSPI_Semaphore_WaitTimeout(uc_semaphore_handle semaphoreHandle, uc_uint32 milliseconds);

/**
 * Destroy a semaphore object.
 *
 * This function is called to destroy a semaphore previously created using ::UniversalClientSPI_Semaphore_Open.
 * 
 * @param[in] pSemaphoreHandle Pointer to handle of semaphore to destroy. The implementation
 *     should free any resources associated with the semaphore, and MUST set the value of *pSemaphoreHandle to NULL. 
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_Semaphore_Close(uc_semaphore_handle * pSemaphoreHandle);
/** @} */ /* End of semaphore */


/** @defgroup mutexspi Cloaked CA Agent Mutex SPIs
 *  Cloaked CA Agent Mutex SPIs
 *
 * These SPIs implement basic thread mutual exclusion via Mutex objects. This
 * is used to prevent multiple threads from simultaneously accessing 
 * mutex-protected objects at the same time. 
 *
 * Recursive mutex support is \b not required. That is, one thread will never
 * attempt to lock a mutex that is already locked by the thread. The mutex 
 * implementation may safely ignore this case. 
 *
 * \note The 'mutex' abstraction is provided to allow platforms to implement
 *    mutexes in a higher-performance manner than a typical semaphore implementation.
 *    Platforms which do not support a separate 'mutex' concept may safely use
 *    a semaphore internal to the implementation of the mutex.
 * 
 *  
 *
 * \note It is the integrator's responsibility to implement these methods for a particular platform.
 *
 *  @{
 */
/**
 * Create a mutex
 *
 * This function is called to create a mutex object.
 *
 * A 'mutex' is a basic synchronization object with the following properties:
 * - Calling ::UniversalClientSPI_Mutex_Lock locks a mutex. If the mutex was already locked, this
 *   waits until the mutex is unlocked, then locks it.
 * - Calling ::UniversalClientSPI_Mutex_Unlock unlocks the mutex.
 *
 * Mutexes are used by the Cloaked CA Agent API to protect internal data structures
 * from being corrupted by simultaneous access. 
 *
 * @param[out] pMutexHandle Receives a handle to a newly created mutex. When the mutex is no longer needed,
 *     it will be closed using ::UniversalClientSPI_Mutex_Close. 
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_Mutex_Open(uc_mutex_handle * pMutexHandle);

/**
 * Locks a mutex
 *
 * This function is called to lock a mutex to prevent other threads from entering
 * a potentially unsafe section of code until the mutex is unlocked. 
 *
 * @param[in] mutexHandle Handle of mutex previously created using ::UniversalClientSPI_Mutex_Open.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_Mutex_Lock(uc_mutex_handle mutexHandle);
/**
 * Unlocks a mutex
 *
 * This function is called to unlock a mutex, and release a single thread waiting in 
 * call to ::UniversalClientSPI_Mutex_Lock, if any.  If no threads are waiting, then
 * the first thread to attempt to lock the mutex will not have to wait.
 *
 * @param[in] mutexHandle Handle of mutex previously created using ::UniversalClientSPI_Mutex_Open.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_Mutex_Unlock(uc_mutex_handle mutexHandle);

/**
 * Destroy a mutex object.
 *
 * This function is called to destroy a mutex object previously created 
 * using ::UniversalClientSPI_Mutex_Open.
 * 
 * @param[in] pMutexHandle Pointer to handle of semaphore to destroy. 
 *     The implementation
 *     should free any resources associated with the mutex, and MUST set 
 *     the value of *pMutexHandle to NULL. 
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_Mutex_Close(uc_mutex_handle * pMutexHandle);
/** @} */ /* End of mutexspi */


/** @defgroup threadspi Cloaked CA Agent Thread SPIs
 *  Cloaked CA Agent Thread SPIs
 *
 *  These SPIs implement basic thread management functions. The Cloaked CA Agent requires
 *  one background thread that is uses for processing commands, ECMs, and EMMs. 
 *
 * \note It is the integrator's responsibility to implement these methods for a particular platform.
 *  @{
 */
/**
 * Create a new thread and begin executing it.
 *
 * \note It is better to let the threads opened by Cloaked CA Agent have a higher priority. Choose a suitable priority when implementing UniversalClientSPI_Thread_Open.
 * Cloaked CA Agent may have requirements on the threads, for example the stack size, the count of threads. Please find the details in the integration documents. Be careful when implementing UniversalClientSPI_Thread_Open!
 *
 * This function is called to create a new instance of a thread, a thread procedure and associated parameter.
 *
 * @param[in] threadProc Starting point of new thread
 * @param[in] lpVoid Parameter to pass to newly created thread
 * @param[out] pThreadHandle Receives an implementation-specific handle to the newly created thread.
 *     The caller will close the handle in a subsequent call to ::UniversalClientSPI_Thread_Close.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_Thread_Open(uc_threadproc threadProc, 
                                       void* lpVoid, uc_thread_handle * pThreadHandle);
/**
 * Suspend the thread for the period specified.
 * 
 * This function is called to suspend the thread for at least the period specified. The minimum granularity of the timer should be 10ms.
 *
 * \note If the requested delay time does not correspond with a boundary of the granularity, the delay time needs to be extended to the next available boundary of the granularity. \n
 *     For example: \n
 *     for platforms that have  1 ms granularity, UniversalClientSPI_Thread_Sleep(hThreadHandle, 1) is to suspend the thread  1 ms; \n
 *     for platforms that have  5 ms granularity, UniversalClientSPI_Thread_Sleep(hThreadHandle, 1) is to suspend the thread 5 ms; \n
 *     for platforms that have 10 ms granularity, UniversalClientSPI_Thread_Sleep(hThreadHandle, 1) is to suspend the thread 10 ms. \n
 *
 * @param[in] hThreadHandle Pointer to handle of thread.
 * @param[in] wDelay is the time in milliseconds to suspend the thread.  
 *
 * \note If the value of hThreadHandle is NULL, it is an invalid value, the SPI implementation shall get the real handle of the thread which is calling ::UniversalClientSPI_Thread_Sleep. 
 * If the value of hThreadHandle is not NULL, it is a valid thread handle.
 *
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_Thread_Sleep(uc_thread_handle hThreadHandle, uc_uint16 wDelay);
/**
 * Closes a thread 
 * 
 * This function is called to close a thread object. This function should block until
 * the thread has completed execution. After this, the function should clean up any data associated with
 * the thread and free the handle.
 *
 * \note Threads are typically not closed until ::UniversalClient_Stop is called. 
 *     For platforms that do not have the ability to properly wait for a thread to complete execution (i.e. thread 'join'),
 *     this can be simulated as necessary using some platform-specific approximation-- it will only affect the client during 
 *     shutdown. For platforms that do not have the means to shut down, this can be ignored. 
 *
 * @param[in] pThreadHandle Pointer to handle of thread to close. 
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_Thread_Close(uc_thread_handle * pThreadHandle);
/** @} */ /* End of thread */

/** @defgroup psspi Cloaked CA Agent Persistent Storage SPIs
 *  Cloaked CA Agent Persistent Storage SPIs
 *
 *  These SPIs implement the Persistent Storage layer that is required in order to save contents of EMMs
 *  that are delivered to the device. This persistent storage layer must be transaction safe, such that if
 *  the device loses power in the middle of a 'write' operation, the data is not corrupted and the device
 *  can continue to function when it is powered back on. 
 *
 *  \note It is the integrator's responsibility to implement these methods for a particular platform.
 *  Please refer to \ref Note9 "Persistent storage requirements" to get more information.
 *
 *  @{
 */
/**
 * Deletes an existing resource.
 *
 * This function is called to delete an existing resource in the Persistent Store, identified by 'index'. 
 * If the resource
 * cannot be found, the function should return ::UC_ERROR_RESOURCE_NOT_FOUND.
 *
 * The index may have previously been created by a call to ::UniversalClientSPI_PS_Write.
 * A very simple implementation of this may just delete the file corresponding with the index of the resource.
 *
 * @param[in] index Index of the resource to delete. Different indexes correspond to independent resources.
 *     For example, resources for index 0 and 1 should be stored in two separate files.\n 
 *     Index 0 is used to store CA data.\n
 *     Index 41~47 are also used to store CA data for multiple secure cores feature.\n
 *     Index 1~8 and 31~38 are used to store Cloakded CA Agent Package if FlexiFlash is enabled. In most cases, only part of these indexes 
 *     need to be supported. For example if only one Cloaked CA Agent Package is deployed, only index 1 and 31 are needed.\n
 *     Index 9 is used to store the configuration file which is provided by Irdeto. The file must be embedded by manufacturer.\n
 *     Index 10, 11, and 12 are used for OTP(one time program) device.   
 *     Currently, only devices with Pre-enablement feature need to access these OTP areas. 
 *     Other devices without Pre-enablement support can ignore these 3 indexes. 
 *     Pre-enablement is not available to most of the devices by default.\n
 *     Index 20 is for Softcell if the Cloaked CA Agent is configured to support Unified Client.\n 
 *     New indexes may be used for other features in future.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     ::UC_ERROR_RESOURCE_NOT_FOUND
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_PS_Delete(uc_uint32 index);

/**
 * Writes data to an existing resource.  
 *
 * This function is called to write a block of data to a resource identified by an index.  The data length and offset
 * within the resource to begin writing is also specified.
 *
 * If the resource did not exist before the call, it should be created. 
 *
 * A very simple implementation of this may just write the data to a file corresponding to the index of the resource.
 *
 * @param[in] index Index of the resource to write into. Different indexes correspond to independent resources.
 *     For example, resources for index 0 and 1 should be stored in two separate files.\n 
 *     Index 0 is used to store CA data.\n
 *     Index 41~47 are also used to store CA data for multiple secure cores feature.\n
 *     Index 1~8 and 31~38 are used to store Cloakded CA Agent Package if FlexiFlash is enabled. In most cases, only part of these indexes 
 *     need to be supported. For example if only one Cloaked CA Agent Package is deployed, only index 1 and 31 are needed.\n
 *     Index 9 is used to store the configuration file which is provided by Irdeto. The file must be embedded by manufacturer.\n
 *     Index 10, 11, and 12 are used for OTP(one time program) device.   
 *     Currently, only devices with Pre-enablement feature need to access these OTP areas. 
 *     Each area can store only one Pre-enablement product. If only one is supported, please bind it to index 10. 
 *     If two are supported, bind them to index 10 and 11. Please refer to integration document for Pre-enablement for details.
 *     Other devices without Pre-enablement support can ignore these 3 indexes. 
 *     Pre-enablement is not available to most of the devices by default.\n
 *     Index 20 is for Softcell if the Cloaked CA Agent is configured to support Unified Client.\n 
 *     New indexes may be used for other features in future.
 *
 * @param[in] offset Offset within the resource to begin writing. Data before this offset must not be affected.
 * @param[in] pData Buffer containing data to write to the resource. The uc_buffer_st::bytes member 
 *     points to the start of the buffer, and the uc_buffer_st::length member contains the number of bytes in the buffer
 *     to write.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     ::UC_ERROR_RESOURCE_NOT_FOUND
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_PS_Write(uc_uint32 index, uc_uint32 offset, const uc_buffer_st *pData);

/**
 * Reads data from an existing resource.
 *
 * This function is called to read a block of data from a resource identified by an index. The data length and offset
 * within the resource to begin reading is also specified.
 *
 * If the resource did not exist before the call, the function should return ::UC_ERROR_RESOURCE_NOT_FOUND.
 *
 * A very simple implementation of this may just read the data from a file corresponding to the index of the resource.
 * 
 * \note If the device declares to support N bytes for this index, it must be able to be read by Cloaked CA Agent in any position 
 * from offset 0 to N-1 at anytime, even before any data is actually written to it. 
 *
 * @param[in] index Index of the resource to read from. Different indexes correspond to independent resources.
 *     For example, resources for index 0 and 1 should be stored in two separate files.\n 
 *     Index 0 is used to store CA data.\n
 *     Index 41~47 are also used to store CA data for multiple secure cores feature.\n
 *     Index 1~8 and 31~38 are used to store Cloakded CA Agent Package if FlexiFlash is enabled. In most cases, only part of these indexes 
 *     need to be supported. For example if only one Cloaked CA Agent Package is deployed, only index 1 and 31 are needed.\n
 *     Index 9 is used to store the configuration file which is provided by Irdeto. The file must be embedded by manufacturer.\n
 *     Index 10, 11, and 12 are used for OTP(one time program) device.   
 *     Currently, only devices with Pre-enablement feature need to access these OTP areas. 
 *     During startup, Cloaked CA Agent tries to read index 10, 11 and 12 to detect how many OTP areas are actually supported by the device by calling
 *     this SPI. All return values other than ::UC_ERROR_SUCCESS and ::UC_ERROR_IO_DATA_CORRUPTED are regarded as this index is not supported.
 *     Each area can store only one Pre-enablement product. If only one is supported, please bind it to index 10. 
 *     If two are supported, bind them to index 10 and 11. ::UC_ERROR_IO_DATA_CORRUPTED is regarded as the OTP data has been corrupted, and Pre-Enablement
 *     will be blocked. Please refer to integration document for Pre-enablement for details.
 *     Other devices without Pre-enablement support can ignore these 3 indexes. 
 *     Pre-enablement is not available to most of the devices by default.\n
 *     Index 20 is for Softcell if the Cloaked CA Agent is configured to support Unified Client.\n
 *     New indexes may be used for other features in future.
 *
 * @param[in] offset Offset within the resource to begin reading. Data before this offset must not be affected.
 * @param[in,out] pData Buffer to read resource data into. 
 *     On input, the uc_buffer_st::bytes member points to a buffer in memory, and the uc_buffer_st::length member 
 *     to the length of the buffer, in bytes.  The function will read uc_buffer_st::length bytes from the resource
 *     and store them in uc_buffer_st::bytes. 
 *     
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     ::UC_ERROR_RESOURCE_NOT_FOUND
 * @retval
 *     ::UC_ERROR_IO_DATA_CORRUPTED
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_PS_Read(uc_uint32 index, uc_uint32 offset, uc_buffer_st *pData);

/**
 * Performs any initialization of the persistent storage layer
 *
 * This function is called during to allow any initialization of the persistent storage layer to occur
 * before any calls to ::UniversalClientSPI_PS_Read, ::UniversalClientSPI_PS_Write, or ::UniversalClientSPI_PS_Delete are made.
 * 
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_PS_Initialize(void);

/**
 * Performs any termination of the persistent storage layer
 *
 * This function is called during to allow any clean up of the persistent storage layer done by ::UniversalClientSPI_PS_Initialize to occur.
 * No calls to ::UniversalClientSPI_PS_Read, ::UniversalClientSPI_PS_Write, or ::UniversalClientSPI_PS_Delete will be made after this function
 * returns. 
 * 
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_PS_Terminate(void);


/** @} */ /* End of ps */

/** @defgroup devicespi Cloaked CA Agent Device SPIs
 *  Cloaked CA Agent Device SPIs
 *
 *  These SPIs are responsible for retrieving values that are provisioned onto a device at manufacture time,
 *  and for setting any hardware specific parameters in the device.
 *  
 * \note It is the integrator's responsibility to implement these methods for a particular platform.
 *  @{
 */

/**
 * Gets the client device ID
 *
 * This function is called to retrieve the ID associated with the device.
 * \note Currently, the device ID is treated as the CSSN of the device. bytes in pData points to an 
 * array , the data type of the array is HEX, e.g. ::uc_byte DeviceID[] = {0x01, 0x00, 0x00, 0x02}.
 *
 * @param[in] pData Buffer to read device ID data into. 
 *     On input, the uc_buffer_st::bytes member points to a buffer in memory, and the uc_buffer_st::length member 
 *     to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer to include
 *     the device ID, and set the uc_buffer_st::length member to the actual length of data returned. 
 *     If the uc_buffer_st::length member is not large enough to hold the entire data, the uc_buffer_st::length member
 *     should be set to the minimum required buffer size, and the function should return ::UC_ERROR_INSUFFICIENT_BUFFER.
 *
 * \note This SPI is for PULL EMM. \n
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_Device_GetDeviceID(uc_buffer_st * pData);

/**
 * Gets the Secure Chipset Serial Number
 *
 * This function is called to retrieve the secure chipset serial number associated with the device.
 * This serial number is used to deliver EMMs addressed to this particular instance of a client.
 *
 * @param[in] pData Buffer to read serial number data into. 
 *     On input, the uc_buffer_st::bytes member points to a buffer in memory, and the uc_buffer_st::length member 
 *     to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer to include
 *     the secure chipset serial number, and set the uc_buffer_st::length member 
 *     to the actual length of data returned. 
 *     If the uc_buffer_st::length member is not large enough to hold the entire data, the uc_buffer_st::length member
 *     should be set to the minimum required buffer size, and the function should return ::UC_ERROR_INSUFFICIENT_BUFFER.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_Device_GetCSSN(uc_buffer_st * pData);

/**
 * Gets the device private data
 *
 * This returns the personalized device specific private data.  This is an optional SPI, the device may not implement it
 * if no such private data is available.
 *
 * \note Currently, device private data is 16 bytes long. These data are provided by manufacturer and need to be personalized into device persistently.
 * Devices that are for demo purpose or for test purpose may NOT have private data at the first beginning, in such case, this SPI implementation
 * can just set the pData->length to ZERO and return UC_ERROR_SUCCESS directly.

 *
 * @param[in] pData Buffer to read private data into. 
 *     On input, the uc_buffer_st::bytes member points to a buffer in memory, and the uc_buffer_st::length member 
 *     to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer to include
 *     the private data, and set the uc_buffer_st::length member to the actual length of data returned. 
 *     If the uc_buffer_st::length member is not large enough to hold the entire data, the uc_buffer_st::length member
 *     should be set to the minimum required buffer size, and the function should return ::UC_ERROR_INSUFFICIENT_BUFFER.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_Device_GetPrivateData(uc_buffer_st * pData);

/**
 * Sets the Secure Chipset Session Key
 *
 * This function is called to provision a secure descrambler chipset with a secure session key.  This session key is
 * used to protect descrambler keys that are delivered to the system by further encrypting them with another
 * key that only this device knows about. 
 *
 * See ::uc_dk_protection for list of the types of protection that can be provided by a platform.
 *
 * \note Some secure chipsets require to swap CSSK or/and CW. SoftCell3 will perform the swap internally so that the output meets the secure chipset's requirement.
 *    Cloaked CA Agent solution doesn't support to perform the swap internally. In order to run Cloaked CA Agent successfully, the swap function MUST be disabled.
 *    The client device manufacturers shall contact chipset vendor to figure out how to disable swap function in such case.
 *    If the descramble status of Cloaked CA Agent is D029, but no audio/video, it is possible that the chipset needs to be configured to disable swap function.
 *    The client device manufacturers may consult with Irdeto if necessary.
 *
 *    \n
 *
 * \note Cloaked CA Agent ONLY supports 2-TDES key ladder currently, i.e. CSSK is encrypted by chipset unique key and control word is encrypted by CSSK.
 *    Encryption algorithm is TDES.
 *
 *    \n
 *
 * \note Whether the chipset needs to use secure control word mode or clear control word mode is not decided by CSSK. 
 *    Once a client with secure chipset has been initialized, Cloaked CA Agent will always set the CSSK to the chipset at startup or on CSSK update.
 *    The driver must judge current working mode using the control word type from ::UniversalClientSPI_Stream_SetDescramblingKey.
 *    In order to support 911 mode, client device with secure chipset must support to switch the working mode dynamically.
 *
 *    \n
 *
 * \note The CSSK belongs to CW key ladder. Refer to ::UniversalClientSPI_Device_SetExtendedCSSK.
 *
 *
 * @param[in] pKeyMaterial CSSK key material to provision into the device. This is normally protected by a secure chipset unique key
 *     (CSUK) that resides within the secure chipset such that an attacker gains no advantage by intercepting this method.
 *     The uc_buffer_st::bytes member points to a buffer containing the raw key material, and the uc_buffer_st::length member contains 
 *     the number of bytes in the buffer.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_Device_SetCSSK(const uc_buffer_st * pKeyMaterial);

/**
 * Sets the Secure Chipset Session Key With key Infomation to support multi key ladder.
 *
 * This is the extension of ::UniversalClientSPI_Device_SetCSSK function with:
 * additional key protection information provided, in order to support different key ladders;
 * additional key ladder information provided, in order to support multiple key ladders.
 * This session key is used to protect descrambler keys that are delivered to the 
 * system by further encrypting them with another key that only this device knows about. 
 *
 * See ::uc_dk_protection for list of the types of protection that can be provided by a platform.
 * See ::uc_dk_ladder for list of the ladder types that can be provided by a platform.
 *
 * \note Some secure chipsets require to swap CSSK or/and CW. SoftCell3 will perform the swap internally so that the output meets the secure chipset's requirement.
 *    Cloaked CA Agent solution doesn't support the ability to perform the swap internally. In order to run Cloaked CA Agent successfully, the swap function MUST be disabled.
 *    The client device manufacturers shall contact chipset vendor to figure out how to disable swap function in such case.
 *    If the descramble status of Cloaked CA Agent is D029, but no audio/video, it is possible that the chipset needs to be configured to disable swap function.
 *    The client device manufacturers may consult with Irdeto if necessary.
 *
 *    \n
 *
 * \note Cloaked CA Agent ONLY supports 2-TDES key ladder currently, i.e. CSSK is encrypted by chipset unique key and control word is encrypted by CSSK.
 *    Encryption algorithm is TDES.
 *
 * @param[in] pCSSKInfo Information about the CSSK being used to protected the descrambling key. The CSSK is also protected by CSUK.
 *     This contain CSSK protection, the key ladder CSSK belongs to, and CSSK material. See ::uc_cssk_info.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_Device_SetExtendedCSSK(const uc_cssk_info * pCSSKInfo);


/**
 * Gets device's security state
 *
 * This function is called to retrieve security state information from a device.
 * Cloaked CA Agent will combine the information from this method to generate a \ref glossary_tsc "TSC" CHIP record. 
 *
 * @param[in,out] pDeviceSecurityState Secure chipset working information to be filled in by the platform.
 *
 * See ::uc_device_security_state for the detail information needed.
 * 
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_Device_GetSecurityState(uc_device_security_state * pDeviceSecurityState);


/**
 * Gets device's platform identifiers
 *
 * This function is called to retrieve relevant identifiers from a device.
 *
 * @param[in,out] pDevicePlatformIdentifiers Device's identifiers to be filled in by the platform.
 *
 * See ::uc_device_platform_identifiers for the detail information needed.
 * 
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_Device_GetPlatformIdentifiers(uc_device_platform_identifiers * pDevicePlatformIdentifiers);

/**
 * Gets device's personalized data
 *
 * This function is called to retrieve personalized data in a device. 
 * The personalized data was written to device on the product line.
 * Now this SPI is only used in Pre-Enablement, which is not a generally available feature. It can be ignored if 
 * Pre-Enablement is not integrated and no personalized data is used.
 *
 * @param[in,out] pData The device's personalized data.
 *     On input, the uc_buffer_st::bytes member points to a buffer in memory, and the uc_buffer_st::length member 
 *     to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer to include
 *     the personalized data, and set the uc_buffer_st::length member to the actual length of data returned. 
 *     If the uc_buffer_st::length member is not large enough to hold the entire data, the uc_buffer_st::length member
 *     should be set to the minimum required buffer size, and the function should return ::UC_ERROR_INSUFFICIENT_BUFFER.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */
extern uc_result UniversalClientSPI_Device_GetPersonalizedData(uc_buffer_st* pData );

/**
 * Gets the Client's IP Address
 *
 * This function is called to retrieve Client's IP Address, bytes in pData points to a null-terminated char string, 
 * e.g. bytes in pData points to "192.xx.xx.01", length in pData equals to strlen("192.xx.xx.01").
 *
 * @param[in] pData Buffer to read Client's IP Address. 
 *     On input, the uc_buffer_st::bytes member points to a buffer in memory, and the uc_buffer_st::length member 
 *     to the maximum length of the buffer, in bytes.  The function will modify the contents of the buffer to include
 *     the data string, and set the uc_buffer_st::length member 
 *     to the actual length of data returned. 
 *     If the uc_buffer_st::length member is not large enough to hold the entire data, the uc_buffer_st::length member
 *     should be set to the minimum required buffer size, and the function should return ::UC_ERROR_INSUFFICIENT_BUFFER.
 *
 * \note This SPI is for PULL EMM. \n
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     ::UC_ERROR_INSUFFICIENT_BUFFER
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */

extern uc_result UniversalClientSPI_Device_GetIPAddress(uc_buffer_st * pData);

/** @} */ /* End of devicespi */

/** @defgroup pvrspi Cloaked CA Agent PVR SPIs
 *  Cloaked CA Agent PVR SPIs
 *
 *  Cloaked CA Agent will output PVR relevant info via these SPIs.
 *  To access PVR functionality, the application should also invoke \ref pvrapis "Cloaked CA Agent PVR APIs"
 *  to cooperate with these SPIs.
 *
 *  
 *  @{
 */

/**
 * Set PVR session key
 *
 * This function is called to set a session key in response to the following requests:
 * -# Request for a PVR record via ::UniversalClient_ConfigService or ::UniversalClient_ConfigConnection
 * -# Request for a PVR playback via ::UniversalClient_SubmitPVRSessionMetadata
 *
 * At a PVR record session, SPI implementation should use this PVR Session Key to encrypt the descrambled content before saving it to storage.
 * At a PVR plackback session, SPI implementation should use this PVR Session Key to decrypt the encrypted content before viewing the content.
 *
 * @param[in] streamHandle The handle of the stream returned by ::UniversalClientSPI_Stream_Open for PVR record.
 * @param[in] pPVRSessionKey Buffer containing a PVR Session Key. 
 *     The uc_buffer_st::bytes member points to a buffer containing the PVR Session Key, and
 *     the uc_buffer_st::length member is set to the length of the session key.Session key length currently is 16 bytes long.
 *     
 * 
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */ 
extern uc_result UniversalClientSPI_PVR_SetSessionKey(
                        uc_stream_handle streamHandle,
                        const uc_buffer_st *pPVRSessionKey);

/**
 * Set PVR session key with key infomation to support multi key ladder
 *
 * This is the extension of ::UniversalClientSPI_PVR_SetSessionKey function with additional key protection infomation provided, 
 * in order to support differents key ladder.This function is called to set a session key in response to the following requests:
 * -# Request for a PVR record via ::UniversalClient_ConfigService or ::UniversalClient_ConfigConnection
 * -# Request for a PVR playback via ::UniversalClient_SubmitPVRSessionMetadata
 *
 * At a PVR record session, SPI implementation should use this PVR Session Key to encrypt the descrambled content before saving it to storage.
 * At a PVR plackback session, SPI implementation should use this PVR Session Key to decrypt the encrypted content before viewing the content.
 *
 * @param[in] streamHandle The handle of the stream returned by ::UniversalClientSPI_Stream_Open for PVR record.
 * @param[in] pPVRSKInfo Information about the PVRSK, contains CSSK protection and CSSK material.See ::uc_pvrsk_info.    
 * 
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List". 
 */ 
extern uc_result UniversalClientSPI_PVR_SetExtendedSessionKey(
                        uc_stream_handle streamHandle,
                        const uc_pvrsk_info * pPVRSKInfo);


/** @} */ /* End of pvrspi */



/** @defgroup streamspi Cloaked CA Agent Stream SPIs
 * Cloaked CA Agent Stream SPIs
 *  
 * These SPIs implement the functionality of a 'stream'. A stream is an object that the client 
 * can connect to in order to set up filters and receive CA messages, like ECMs and EMMs, and receive
 * keys that can be used to descramble content. 
 *
 * Streams are created by the client when a method that requires a stream instance is
 * called. 
 *
 * \note Cloaked CA Agent doesn't have the concept of descrambler, please refer to \ref Note12 "When and how to open descrambler"
 * for more information on descrambler handling.
 *
 * A stream implementation is responsible for the following:
 *
 * - Managing lifetime of an instance of a stream, via ::UniversalClientSPI_Stream_Open and ::UniversalClientSPI_Stream_Close methods.
 * - Connecting to the CA processing aspect of the client via ::UniversalClientSPI_Stream_Connect and ::UniversalClientSPI_Stream_Disconnect.
 * - Handling filters via the ::UniversalClientSPI_Stream_OpenFilter and ::UniversalClientSPI_Stream_CloseFilter methods.
 * - Providing sections data via the ::uc_notify_callback method.
 * - Forwarding control words received via ::UniversalClientSPI_Stream_SetDescramblingKey to the corresponding descrambler(s).
 *
 * \note It is the integrator's responsibility to implement these methods for a particular platform.
 *
 * For more information on how streams are used, see \ref stream_interface_call_sequence "Stream Interface Call Sequence". 
 *  @{
 */

/**
 * Starts all filters for this stream.
 *
 * This function is called to start all filters that were allocated and set for this stream
 * using ::UniversalClientSPI_Stream_OpenFilter and ::UniversalClientSPI_Stream_SetFilter. 
 * 
 * Once this method is called, a stream implementation is free to call the ::uc_notify_callback 
 * function originally passed to ::UniversalClientSPI_Stream_Connect for this stream instance.
 *
 *
 * @param[in] streamHandle Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval 
 *    ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
*/
uc_result UniversalClientSPI_Stream_Start(uc_stream_handle streamHandle);

/**
 * Stops all filters for this stream.
 *
 * This function is called to stop all filters that were allocated and set for this stream
 * using ::UniversalClientSPI_Stream_OpenFilter and ::UniversalClientSPI_Stream_SetFilter. 
 * 
 * Once this method is called, then no more data should be passed to ::uc_notify_callback function 
 * originally passed to ::UniversalClientSPI_Stream_Connect for this stream instance.
 *
 * 
 * @param[in] streamHandle Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval 
 *    ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClientSPI_Stream_Stop(uc_stream_handle streamHandle);

/**
 * Stream method for opening a filter.
 *
 * A stream is controlled by the adding 
 * and removal of 'filters'. Each filter describes
 * what kinds of sections are expected to be received from the stream. The stream implementation
 * is only required to provide sections that match one of the currently active filters. 
 *
 * This method is called to add a filter to an existing stream. Before this callback is 
 * called, no data should be provided via the notification function. 
 *
 * After an EMM stream is opened and connected, this method is called several times to set up different filters.
 * Each EMM stream will need 7 filters.
 * 
 * For an ECM stream, only a single filter is used. 
 *
 * After this method is called, ::UniversalClientSPI_Stream_SetFilter is called to provide or change the filter
 * rules on the filter handle. After all filters are initially set up, ::UniversalClientSPI_Stream_Start is called.
 *
 * #UC_INVALID_HANDLE_VALUE is not a valid filter handle. If the function returns success, it must not
 * set *pFilterHandle to #UC_INVALID_HANDLE_VALUE. 
 *
 *
 * @param[in] streamHandle Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 * @param[out] pFilterHandle Receives an implementation defined filter handle that can be used to 
 *      identify the filter in a subsequent call to ::UniversalClientSPI_Stream_CloseFilter.
 *      
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval 
 *    ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClientSPI_Stream_OpenFilter(
    uc_stream_handle streamHandle,
    uc_filter_handle *pFilterHandle);

/**
 * Stream method for setting filter information.
 * 
 * This method is called after ::UniversalClientSPI_Stream_OpenFilter for each filter.  This method is also
 * called when a message is received that requires changing of filter rules. 
 *
 * \note Mask/match/filterDepth used by UniversalClientSPI_Stream_SetFilter includes 2 bytes after table_id, i.e. section_syntax_indicator and section_length.
 * If filterDepth is equal to 2 or above and client device driver doesn't support to set filter conditions to these 2 bytes, Client device shall disregard these 2 bytes.
 *
 *  - For EMM streams, filter rules change when a new group unique address is assigned to the client. 
 *    In a typical Irdeto CA system, a group unique address can be reassigned as necessary to 
 *    minimize bandwidth.  
 *  - For ECM streams, filter rules change when a new ECM arrives with a different table ID than the 
 *    previous ECM. In a typical Irdeto CA system, old ECMs are played out until the descrambling key
 *    cycles. After that, the new ECM is delivered with an alternate table IDs, and repeated with that
 *    table ID until the next key cycle. This filter is used to prevent the resubmission of duplicate 
 *    ECMs to the client.
 *
 * @param[in] filterHandle Filter handle previously allocated by ::UniversalClientSPI_Stream_OpenFilter.
 * @param[in] pFilterRules The rule set of the filter to create.  If this is NULL, the implementation
 *      should return all data that arrives, regardless of pattern i.e. NULL means data is unfiltered. 
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval 
 *    ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClientSPI_Stream_SetFilter(
    uc_filter_handle filterHandle,
    const uc_filter *pFilterRules);


/**
 * Stream method for closing a filter
 *
 * This function is called to close an existing filter from a stream that was previously
 * added by a call to ::UniversalClientSPI_Stream_OpenFilter.  
 *
 *
 * @param[in] streamHandle Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 * @param[in,out] pFilterHandle On input, points to a filter handle previously allocated by 
 *      ::UniversalClientSPI_Stream_OpenFilter.  Any private data 
 *      associated with filterHandle should be cleaned up by this method. When the method returns,
 *      it must set *pFilterHandle to #UC_INVALID_HANDLE_VALUE to indicate it has freed the handle. 
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_NOT_IMPLEMENTED
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClientSPI_Stream_CloseFilter(
    uc_stream_handle streamHandle,
    uc_filter_handle *pFilterHandle);

/**
 * Method to connect to a stream.
 *
 * After a stream is opened with a call to ::UniversalClientSPI_Stream_Open, internally the client will call 
 * this stream interface method in order to complete the connection setup. 
 *
 * Part of the connection process involves informing the stream of a notification callback and connection handle
 * that are used to deliver new ECM and EMM sections to the client.
 *
 * After connecting to a stream, the client may set up filters by calling ::UniversalClientSPI_Stream_OpenFilter.
 * After this is done, the stream may begin sending sections to the notification callback, specifying the 
 * connection handle passed into this call to \a Connect. 
 *
 *
 * @param[in] streamHandle Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 * @param[in] connectionHandle Connection handle to associate with this stream. This must also be 
 *     passed to the \a notifyCallback function when sections arrive. 
 * @param[in] notifyCallback Function to be called when sections arrive.Implementations that receive one section at a time can safely pass the data directly
 *     (possibly in parallel, as this function is reentrant), whereas implementations that receive
 *      blocks of sections can also safely pass the data directly without having to parse the sections
 *      and break it into individual section.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval 
 *    ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClientSPI_Stream_Connect(
    uc_stream_handle streamHandle,
    uc_connection_handle connectionHandle,
    uc_notify_callback notifyCallback);

/**
 * Stream method to disconnect a stream. 
 *
 * This method is provided to give the stream an opportunity to clean up resources related to a connection previously
 * made by a call to ::UniversalClientSPI_Stream_Connect.  
 *
 * A stream that has been disconnected should not continue to call ::uc_notify_callback 
 * after this call returns. 
 *
 * @param[in] streamHandle Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 * @param[in] connectionHandle Connection handle originally passed to ::UniversalClientSPI_Stream_Connect.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval 
 *    ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClientSPI_Stream_Disconnect(
    uc_stream_handle streamHandle,
    uc_connection_handle connectionHandle);

/**
 * Stream method to pass a key to the descrambler
 *
 * For ECM streams only: this method is provided to send descrambling keys extracted from ECMs on
 * this stream to the descrambler for each active component previously added by ::UniversalClientSPI_Stream_AddComponent.
 *
 * For other streams, this method will not be called.
 *
 * \note Currently, Cloaked CA Agent will output clear CW when the service is in 911 mode or the client is anchored with security ID.
 *    For platform with secure chipset anchor, it is highly recommended that the platform supports dual mode, i.e. the platform shall be able to disable or enable secure chipset function dynamically, 
 *    so that the secure chipset can descramble the content with clear CW. In case the platform can NOT support dual mode, the platform will NOT be able to descramble the service in 911 mode.
 *
 *    \n
 *
 * \note Some secure chipsets require to swap CSSK or/and CW. SoftCell3 will perform the swap internally so that the output meets the secure chipset's requirement.
 *    Cloaked CA Agent solution doesn't support the ability to perform the swap internally. In order to run Cloaked CA Agent successfully, the swap function MUST be disabled.
 *    The client device manufacturers shall contact chipset vendor to figure out how to disable swap function in such case.
 *    If the descramble status of Cloaked CA Agent is D029, but no audio/video, it is possible that the chipset needs to be configured to disable swap function.
 *    The client device manufacturers may consult with Irdeto if necessary.
 *
 *    \n
 *
 * \note Cloaked CA Agent ONLY supports 2-TDES key ladder currently, i.e. CSSK is encrypted by chipset unique key and control word is encrypted by CSSK.
 *    Encryption algorithm is TDES.
 *
 *    \n
 *
 * \note For Hybrid client, if the stream handle binds to HLS service, the descrabmling key MUST be set to PVR key ladder,
 *    refer to ::UniversalClientSPI_Device_SetExtendedCSSK.
 *
 *
 *
 * @param[in] streamHandle Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 * @param[in] pKeyInfo Information about the key being used to descramble content. This contains key algorithm, protection,
 *     cryptography context, and the key material.
 * @param[in] keyVersion Key version that this descrambling key is associated with. 
 *     Different key versions refer to potentially different keys. The descrambler 
 *     should maintain separate descrambling keys for each key version. The key version
 *     typically cycles between 1 and N, where N is the number of different key 
 *     versions supported by the scrambler. This value is used to 
 *     allow for smooth transition from one key to the next when the descrambling key 
 *     changes. At a minimum, implementations should be able to support at least 2 
 *     key identifiers, 0 and 1.
 *     \note For DVB, 0 indicates Even Key and 1 indicates Odd Key. For HLS, Even key and Odd Key are identical, Application can ignored this parameter, 
 *     and uses either  Even key or Odd Key.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClientSPI_Stream_SetDescramblingKey(
    uc_stream_handle streamHandle,
    const uc_key_info *pKeyInfo,
    uc_uint32 keyVersion);

/**
 * Stream method to clean the descrambler keys stored in client device.
 * 
 * Cloaked CA Agent will invoke this SPI immediately when the client is not entitled to view the content. 
 * Client device shall make descramblers stop working by setting random keys(both even and odd) to descramblers or by stopping descramblers.
 * 
 * @param[in] streamHandle Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 *
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClientSPI_Stream_CleanDescramblingKey(
    uc_stream_handle streamHandle);    

/**
 * Stream method to stop and cleanup a stream instance
 *
 * This method is called when the stream is being closed by UniversalClient_Stream_Close and is 
 * intended to give the stream implementation a chance to clean up any leftover resources set 
 * up during a call to open.
 *
 * @param[in,out] pStreamHandle On input, points to a stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 *      On output, the function must set this to #UC_INVALID_HANDLE_VALUE.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClientSPI_Stream_Close(
    uc_stream_handle *pStreamHandle);

/**
 * Stream method to send data to the CCIS stream.
 *
 * This method is used to send data to a CCIS stream.
 * 'pBytes' contained the information associated with the CCIS stream. 
 *
 *
 * \note This SPI is for PULL EMM. \n
 *
 * @param[in] streamHandle Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 * @param[in] pBytes Data structure that identifies the payload for the send message, also refer to ::uc_stream_send_payload.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval 
 *    ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClientSPI_Stream_Send(
     uc_stream_handle streamHandle,
     const uc_stream_send_payload *pBytes);


/** @} */ /* End of streamspi */

/** @defgroup copycontrolspi Cloaked CA Agent Copy Control SPIs
 *  Cloaked CA Agent Copy Control SPIs
 *
 *  Cloaked CA Agent will output copy control information via these SPIs.
 *  To access copy control functionality, the application should also invoke \ref copycontrolapis "Cloaked CA Agent Copy Control APIs"
 *  to cooperate with these SPIs.
 *
 *  
 *  @{
 */

/**
 * Sets the macrovision configuration data
 *
 * This method is called during the initialization of Cloaked CA Agent, or when the Cloaked CA Agent received Macrovision CAM emm.
 *
 * @param[in] mac_config_data macrovsion configuration data structure. this contains MacrovisionVersion and 
    Macrovision configuration data.
 *
 * See ::uc_macrovision_config for the detail information needed.
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result  UniversalClientSPI_CopyControl_Macrovision_SetConfig(uc_macrovision_config mac_config_data);


/**
 * CopyControl method to set the Macrovision mode
 *
 * This method is called when the received stream containes Macrovision mode info or submitted Session Metadata containes Macrovision mode.
 *
 * @param[in] streamHandle Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 * @param[in] mac_mode The mode of macrovision to be set
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result  UniversalClientSPI_CopyControl_Macrovision_SetMode(uc_stream_handle streamHandle,uc_macrovision_mode mac_mode);
/**
 * CopyControl and CI Plus method to set the CCI bits info [uc_copy_control_info] to client
 *
 * This method is called when the received ECM containes the CopyControl/CI Plus info or the submitted Session Metadata containes the CopyControl/CI Plus info.
 *
 * @param[in] streamHandle Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 * @param[in] pCopyControlInfo points to the structure of detail copy control information.
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result  UniversalClientSPI_CopyControl_SetCCI(uc_stream_handle streamHandle, uc_copy_control_info* pCopyControlInfo);

/** @} */ /* End of copycontrolspi */

/** @defgroup smartcardspi Cloaked CA Agent Smartcard SPIs
 *  Cloaked CA Agent Smartcard SPIs
 *
 *  Cloaked CA Agent will communicate with smartcard via these SPIs.
 *  \note Currently, if Cloaked CA Agent is configured without Unified Client support, the smartcard SPIs are used by \ref glossary_testcore "TestCore" only. 
 *  They are NOT used by Cloaked CA Agent. If Unified Client is used, these SPIs must be supported.
 *  \ref glossary_testcore "TestCore" will use these SPIs to communicate with smartcard, so that the smartcard slot can be verified.
 *  
 *  @{
 */

/**
 * Smartcard method to initialize the smartcard device.
 *
 * This method is called during the initialization of Cloaked CA Agent.
 *
 * @param[out] pSmartcardID identity for the smartcard device, Cloaked CA Agent uses the
 *            resource ID to map to a smartcard reader slot.  
 * @param[in] pSCOpenData pointer of data transmitted to smartcard driver, the pointer 
 *            points to structure ::uc_sc_open_parameter.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClientSPI_Smartcard_Open(uc_uint32 *pSmartcardID, uc_sc_open_parameter *pSCOpenData);


/**
 * Smartcard method to close the smartcard device.
 *
 * This method is called when closing the Cloaked CA Agent.
 *
 * @param[in] smartcardID identity for the smartcard device, Cloaked CA Agent uses the
 *            resource ID to map to a smartcard reader slot.
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClientSPI_Smartcard_Close(uc_uint32 smartcardID);

/**
 * Smartcard method to reset the smartcard device.
 *
 * This method is called when Cloaked CA Agent wants to reset the smartcard device.
 *
 * @param[in] smartcardID identity for the smartcard device, Cloaked CA Agent uses the
 *            resource ID to map to a smartcard reader slot.
 */
 uc_result UniversalClientSPI_Smartcard_Reset(uc_uint32 smartcardID);

/**
 * Smartcard method to communicate with the smartcard device
 *
 * This method is called when Cloaked CA Agent wants to read/write data from/to the smartcard device
 *
 * @param[in] smartcardID identity for the smartcard device, Cloaked CA Agent uses this ID
 *            to map to a smartcard reader slot.
 * @param[in] headerLen header length of the APDU.
 * @param[in] payloadLen payload length of the APDU.
 * @param[in] pSendBuffer buffer for the  APDU.
 * @param[out] pRecvDataLen received buffer length.
 * @param[out] pRecvBuffer received buffer.
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClientSPI_Smartcard_Communicate(uc_uint32 smartcardID, uc_uint32 headerLen, uc_uint32 payloadLen,uc_byte *pSendBuffer, uc_uint32 *pRecvDataLen, uc_byte *pRecvBuffer );

/** @} */ /* End of smartcardspi */

/** @defgroup datetimespi Cloaked CA Agent Date Time SPIs
 *  Cloaked CA Agent Date Time SPIs
 *
 *  Cloaked CA Agent will retrieve date time info via these SPIs.
 *  
 *  @{
 */


/**
 * Method to get current date time.
 *
 * For PULL EMM, this is a must-be SPI; otherwise, this SPI is optional.
 *
 * This method is called to retrieve second and microsecond of current time info.
 * In Linux-like system, gettimeofday can be used to get the result(but the result needs to be converted to millisecond).
 * In MS Windows-like system, GetTickCount, GetLocalTime and GetSystemTime can be used to get the result.
 *
 * \note Cloaked CA Agent only uses the time info to measure the performance. So the time can be time elapsed since reboot, or 
 * it can also be the real day time.
 *
 *    \n
 *
 * \note For PULL EMM, the time info is used to calculate the time of receiving Timestamp EMM . So the time must be time elapsed since reboot.
 *
 * @param[out] pCurrentTime To save current time info.  
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClientSPI_DateTime_GetTimeOfDay(uc_time *pCurrentTime);

/** @} */ /* End of datetimespi */

/** @defgroup manufacturercryptospi Cloaked CA Agent Crypto SPIs
 *  Cloaked CA Agent Crypto SPIs
 *
 *  Cloaked CA Agent will do the verification step for digital signature via these SPIs.
 *  
 *  @{
 */


/**
 * Method to do the verification for digital signature.
 *
 * This SPI is optional currently.
 *
 * These functional modules are implemented by both Cloaked CA Agent and STB manufacturer.
 * Generally speaking, the SPI form is faster than the Cloaked CA Agent's implementation.
 * Cloaked CA Agent will decide which implementation should be used in digital signature process.
 *
 * @param[in] pCryptoInfo send relative info indicating the deciphering algorithm type and the parameter related to it.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClientSPI_Crypto_Verify(uc_crypto_info *pCryptoInfo);

/** @} */ /* End of manufacturercryptospi */

/** @defgroup timerspi Cloaked CA Agent Timer SPIs
 *  Cloaked CA Agent Timer SPIs
 *
 *  Cloaked CA Agent will do timing via these SPIs.
 *  
 *  @{
 */

/**
 * Open a timer. 
 *
 * @param[out] pTimerId identity for the timer, Cloaked CA Agent uses this to map to a timer.  
 * @param[in] pTimerInfo pointer of data transmitted to timer driver, pTimerInfo 
 *            points to structure ::uc_timer_info.
 *
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClientSPI_Timer_Open(uc_uint32* pTimerId,uc_timer_info* pTimerInfo);

/**
 * Close the timerId
 *
 * @param[in] timerId identity for the timer, Cloaked CA Agent uses this to map to a timer.
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClientSPI_Timer_Close(uc_uint32 timerId);

/**
 * Stops the timer.
 *
 * This method is called when Cloaked CA Agent wants to stop a timer.
 * Cloaked CA Agent may reuse the timer indicated by \b timerId later on.
 *
 * @param[in] timerId identity for the timer, Cloaked CA Agent uses this to map to a timer.
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClientSPI_Timer_Stop(uc_uint32 timerId);


/**
 * Start the timer. 
 *
 * Cloaked CA Agent calls this to restart the timer which stopped is by ::UniversalClientSPI_Timer_Stop before.
 *
 * @param[in] timerId identity for the timer, Cloaked CA Agent uses this to map to a timer.
 * @retval 
 *    ::UC_ERROR_SUCCESS
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
uc_result UniversalClientSPI_Timer_Start(uc_uint32 timerId);

/** @} */ /* End of timerspi */


/** @} */ /* End of Cloaked CA Agent SPIs*/


#ifdef __cplusplus
}
#endif

#endif /* !UNIVERSALCLIENT_SPI_H__INCLUDED__ */

