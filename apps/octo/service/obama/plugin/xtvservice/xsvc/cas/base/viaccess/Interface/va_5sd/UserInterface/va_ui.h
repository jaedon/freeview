/******************************************************************************

    Copyright
    This code is strictly confidential and the receiver is obliged to use it
    exclusively for his or her own purposes. No part of Viaccess code may be
    reproduced or transmitted in any form or by any means, electronic or
    mechanical, including photocopying, recording, or by any information storage
    and retrieval system, without permission in writing from Viaccess.
    The information in this code is subject to change without notice. Viaccess
    does not warrant that this code is error free. If you find any problems
    with this code or wish to make comments, please report them to Viaccess.

    Trademarks
    Viaccess is a registered trademark of Viaccess � in France and/or other
    countries. All other product and company names mentioned herein are the
    trademarks of their respective owners.
    Viaccess may hold patents, patent applications, trademarks, copyrights
    or other intellectual property rights over the code hereafter. Unless
    expressly specified otherwise in a Viaccess written license agreement, the
    delivery of this code does not imply the concession of any license over
    these patents, trademarks, copyrights or other intellectual property.

******************************************************************************/

/*
    $Revision: 14190 $
    $Date: 2010-02-24 15:58:38 +0100 (mer., 24 févr. 2010) $
*/

#ifndef _VA_UI_H_
#define _VA_UI_H_

#include "va_types.h"
#include "va_errors.h"
#include "va_setup.h"


/********************************************************/
/*                CONSTANT DEFINITION                   */
/********************************************************/

#define kVA_UI_NOT_USED                         0xFFFFFFFF
#define kVA_UI_MAX_LISTENER                     8

#define kVA_UI_PIN_CODE_SIZE                    8
#define kVA_UI_UA_SIZE                          5

#define kVA_UI_MAX_LABEL_SIZE                   16
#define kVA_UI_MAX_CLASS_MASK_SIZE              32
#define kVA_UI_GCA_SIZE                         4

#define kVA_UI_MAX_CURRENCY_LABEL_SIZE          16

#define kVA_UI_MAX_CAMLOCK_SIZE                 16
#define kVA_UI_MAX_PAIRING_SMARTCARD_NUMBER     8
#define kVA_UI_STB_ID_SIZE                      5

#define kVA_UI_MAX_OPERATOR_ITEM_SIZE           256

#define kVA_UI_MAX_CAS_VARIANT_SIZE             64
#define kVA_UI_MAX_OPERATOR_NAME_SIZE           32
#define kVA_UI_MAX_VERSION_NAME_SIZE            64

#define kVA_UI_ORM_IDENTIFIED_BY_SOID           0x0000
#define kVA_UI_ORM_IDENTIFIED_BY_URI            0x0001
#define kVA_UI_MAX_APPLICATION_MSG_SIZE         233

#define kVA_UI_TERMINAL_ID_SIZE                 16
#define kVA_UI_ACTIVATION_CODE_SIZE             20
#define kVA_UI_MAX_SERVER_TYPE_SIZE             20
#ifdef CONFIG_MW_CAS_VIACCESS_DRIVER_TEST
#define kVA_UI_MAX_ADDRESS_SIZE                 20
#else
#define kVA_UI_MAX_ADDRESS_SIZE                 235
#endif

#define kVA_UI_MAX_MODULE_NAME_LENGTH           64 /* TBD */
#define kVA_UI_MAX_FILENAME_LENGTH              64 /* TBD */

/*********************************************************/
/*                  GENERAL TYPES                        */
/*********************************************************/

typedef BYTE    tVA_UI_Ua[kVA_UI_UA_SIZE];                      /* Unique Adress       */
typedef UINT32  tVA_UI_Soid;                                    /* Service Operator Id */
typedef BYTE    tVA_UI_PinCode[kVA_UI_PIN_CODE_SIZE];           /* PIN code            */
typedef BYTE    tVA_UI_Gca[kVA_UI_GCA_SIZE];                    /* GCA                 */
typedef char    tVA_UI_Currency[kVA_UI_MAX_CURRENCY_LABEL_SIZE+1];/* Currency            */
typedef BYTE    tVA_UI_StbId[kVA_UI_STB_ID_SIZE];               /* Stb Id              */
typedef BYTE    tVA_UI_TerminalId [kVA_UI_TERMINAL_ID_SIZE];    /* Terminal Id         */
typedef UINT32  tVA_UI_ProgramNumber;                           /* Program numbers     */



/*********************************************************/
/*                  Enumerations                         */
/*********************************************************/

/*-------------------------------------------------------*/
/*                  EVENT TYPE                           */
/*-------------------------------------------------------*/
typedef enum
{
    eVA_UI_DESCRAMBLING_OK_EVENT                            = 0x8020,
    eVA_UI_DESCRAMBLING_OK_RIGHT_OK_EVENT,
    eVA_UI_DESCRAMBLING_OK_PREVIEW_EVENT,
    eVA_UI_DESCRAMBLING_OK_PREVIEW_AND_IPPV_OFFER_EVENT,

    eVA_UI_DESCRAMBLING_KO_EVENT                            = 0x8040,
    eVA_UI_DESCRAMBLING_KO_NO_RIGHT_EVENT,
    eVA_UI_DESCRAMBLING_KO_MATURITY_RATING_EVENT,
    eVA_UI_DESCRAMBLING_KO_GEOGRAPHICAL_BLACKOUT_EVENT,
    eVA_UI_DESCRAMBLING_KO_MISSING_KEY_EVENT,
    eVA_UI_DESCRAMBLING_KO_IPPV_OFFER_EVENT,
    eVA_UI_DESCRAMBLING_KO_IPPV_OFFER_EXPIRED_EVENT,
    eVA_UI_DESCRAMBLING_KO_MATURITY_RATING_AND_IPPV_OFFER_EVENT,
    eVA_UI_DESCRAMBLING_KO_FORBIDDEN_MODE_EVENT,
    eVA_UI_DESCRAMBLING_KO_INSUFFICIENT_CREDIT_EVENT,
    eVA_UI_DESCRAMBLING_KO_RIGHT_EXPIRED_EVENT,
    eVA_UI_DESCRAMBLING_KO_RIGHT_CONSUMED_EVENT,
    eVA_UI_DESCRAMBLING_KO_CW_PROTECTION_MISMATCH_EVENT,

    eVA_UI_FREE_TO_AIR_EVENT                                = 0x8080,
    eVA_UI_VIACCESS_NOT_APPLICABLE_EVENT,
    eVA_UI_VIACCESS_SMARTCARD_NOT_FOUND_EVENT,
    eVA_UI_VIACCESS_TERMINAL_NOT_ACTIVATED_EVENT,

    eVA_UI_SMARTCARD_INSERTED_EVENT                         = 0x8100,
    eVA_UI_SMARTCARD_EXTRACTED_EVENT,
    eVA_UI_SMARTCARD_READY_EVENT,
    eVA_UI_SMARTCARD_NEEDED_EVENT,
    eVA_UI_SMARTCARD_FAILED_EVENT,
    eVA_UI_SMARTCARD_NOT_ACCEPTED_EVENT,
    eVA_UI_SMARTCARD_INSUFFICIENT_MEMORY_EVENT,
    eVA_UI_SMARTCARD_INVALIDATED_EVENT,

    eVA_UI_NEW_OPERATOR_MESSAGE_EVENT                       = 0x8200,
    eVA_UI_NEW_OPERATOR_DATA_EVENT,
    eVA_UI_CAMLOCK_ACTIVATED_OK_EVENT,
    eVA_UI_CAMLOCK_ACTIVATED_KO_EVENT,
    eVA_UI_CAMLOCK_ACTIVATED_NO_CARD_EVENT,
    eVA_UI_CAMLOCK_NOT_ACTIVATED_EVENT,
    eVA_UI_CAMLOCK_DISABLED_EVENT,
    eVA_UI_START_SECURITY_RESTRICTION_EVENT,
    eVA_UI_STOP_SECURITY_RESTRICTION_EVENT,
    eVA_UI_PAIRING_MISMATCH_EVENT,
    eVA_UI_OVERDRAFT_REACHED_EVENT ,
    eVA_UI_THRESHOLD_REACHED_EVENT,

    eVA_UI_RECORD_RIGHT_OK_EVENT                            = 0x8400,
    eVA_UI_TIMESHIFT_RIGHT_OK_EVENT,
    eVA_UI_RECORD_RIGHT_KO_EVENT,
    eVA_UI_TIMESHIFT_RIGHT_KO_EVENT,
    eVA_UI_RECORD_INFO_CHANGE_EVENT,

    eVA_UI_ORM_CONNECTION_OK_EVENT                          = 0x8800,
    eVA_UI_ORM_CONNECTION_KO_EVENT,
    eVA_UI_CW_SERVER_CONNECTION_OK_EVENT,
    eVA_UI_CW_SERVER_CONNECTION_KO_EVENT,
    eVA_UI_KS_SYNCHRO_START_EVENT,
    eVA_UI_KS_SYNCHRO_STOP_EVENT,

    eVA_UI_TERMINAL_NOT_ACTIVATED_EVENT                     = 0x9000,
    eVA_UI_TERMINAL_READY_EVENT,
    eVA_UI_TERMINAL_INSUFFICIENT_MEMORY_EVENT,
    eVA_UI_TERMINAL_CONFIGURATION_ERROR_EVENT,
    eVA_UI_TERMINAL_INVALIDATED_EVENT,
    eVA_UI_TERMINAL_PERSONALIZATION_OK_EVENT,
    eVA_UI_TERMINAL_PERSONALIZATION_KO_EVENT,


    eVA_UI_LAST_EVENT    /* Only use to know the end */

} tVA_UI_EventType;

/*-------------------------------------------------------*/
/*                  REQUEST TYPE                         */
/*-------------------------------------------------------*/
typedef enum
{
    eVA_UI_GET_VERSION_NAME_REQUEST,
    eVA_UI_GET_VERSION_REQUEST,
    eVA_UI_GET_ACCESS_CONDITION_REQUEST,
    eVA_UI_GET_SMARTCARD_INFO_REQUEST,
    eVA_UI_GET_SMARTCARD_LIST_INFO_REQUEST,
    eVA_UI_GET_UA_REQUEST,
    eVA_UI_GET_SOID_LIST_REQUEST,
    eVA_UI_GET_LABEL_REQUEST,
    eVA_UI_GET_PIN_CODE_ATTEMPTS_REQUEST,
    eVA_UI_MODIFY_PIN_CODE_REQUEST,
    eVA_UI_UNLOCK_CONSULTATION_REQUEST,
    eVA_UI_GET_CLASS_LIST_REQUEST,
    eVA_UI_GET_THEME_LEVEL_LIST_REQUEST,
    eVA_UI_GET_PREBOOKED_LIST_REQUEST,
    eVA_UI_GET_PREVIEW_INFO_REQUEST,
    eVA_UI_GET_MATURITY_RATING_REQUEST,
    eVA_UI_MODIFY_MATURITY_RATING_REQUEST,
    eVA_UI_ACCEPT_MATURITY_RATING_REQUEST,
    eVA_UI_GET_PRESELECTION_REQUEST,
    eVA_UI_GET_OPERATOR_PRESELECTION_REQUEST,
    eVA_UI_MODIFY_PRESELECTION_REQUEST,
    eVA_UI_GET_GEOGRAPHICAL_CODE_ADDRESS_REQUEST,
    eVA_UI_GET_FAC_DATA_REQUEST,
    eVA_UI_GET_FAC_LIST_REQUEST,
    eVA_UI_SUBMIT_EXM_REQUEST,

    /* PPV Request */
    eVA_UI_GET_IPPVP_LIST_REQUEST,
    eVA_UI_GET_IPPVT_LIST_REQUEST,
    eVA_UI_GET_CREDIT_INFO_REQUEST,
    eVA_UI_GET_IPPV_INFO_REQUEST,
    eVA_UI_ACCEPT_IPPVP_REQUEST,
    eVA_UI_ACCEPT_IPPVT_REQUEST,
    eVA_UI_GET_THRESHOLD_REQUEST,
    eVA_UI_MODIFY_THRESHOLD_REQUEST,
    eVA_UI_START_IPPV_SURVEY_REQUEST,
    eVA_UI_CANCEL_IPPV_SURVEY_REQUEST,

    /* Messaging and data Request */
    eVA_UI_GET_OPERATOR_MESSAGE_REQUEST,
    eVA_UI_GET_OPERATOR_DATA_REQUEST,

    /* CAMLOCK Request */
    eVA_UI_GET_CAMLOCK_INFO_REQUEST,

    /* Viaccess reserved Request (to be ignored) */
    eVA_UI_RESERVED_1_REQUEST,

    /* Security Request */
    eVA_UI_GET_PAIRING_MODE_REQUEST,
    eVA_UI_GET_PAIRING_SMARTCARD_REQUEST,
    eVA_UI_GET_PAIRING_UA_LIST_REQUEST,
    eVA_UI_GET_STB_ID_REQUEST,

    eVA_UI_ASK_HEADEND_PROCESS_REQUEST,
    eVA_UI_HEADEND_CREDIT_REQUEST,
    eVA_UI_HEADEND_PROGRAM_NUMBER_REQUEST,

    /* SmartCardLess Requests */
    eVA_UI_GET_TERMINAL_ACCESS_CONDITION_REQUEST,
    eVA_UI_GET_TERMINAL_INFO_REQUEST,
    eVA_UI_GET_TERMINAL_UA_REQUEST,
    eVA_UI_GET_TERMINAL_SOID_LIST_REQUEST,
    eVA_UI_GET_TERMINAL_LABEL_REQUEST,
    eVA_UI_GET_TERMINAL_PIN_CODE_ATTEMPTS_REQUEST,
    eVA_UI_MODIFY_TERMINAL_PIN_CODE_REQUEST,
    eVA_UI_GET_TERMINAL_CLASS_LIST_REQUEST,
    eVA_UI_GET_TERMINAL_PREBOOKED_LIST_REQUEST,
    eVA_UI_GET_TERMINAL_PREVIEW_INFO_REQUEST,
    eVA_UI_GET_TERMINAL_MATURITY_RATING_REQUEST,
    eVA_UI_MODIFY_TERMINAL_MATURITY_RATING_REQUEST,
    eVA_UI_ACCEPT_TERMINAL_MATURITY_RATING_REQUEST,
    eVA_UI_GET_TERMINAL_FAC_DATA_REQUEST,
    eVA_UI_SUBMIT_TERMINAL_EXM_REQUEST,
    eVA_UI_GET_TERMINAL_ID_REQUEST,
    eVA_UI_SET_TERMINAL_ACTIVATION_CODE_REQUEST,
    eVA_UI_GET_TERMINAL_NETWORK_SETUP_REQUEST,

    /* Traces Request*/
    eVA_UI_GET_TRACE_OUTPUT_REQUEST,
    eVA_UI_SET_TRACE_OUTPUT_REQUEST,
    eVA_UI_GET_TRACE_LIST_REQUEST,
    eVA_UI_SET_TRACE_LEVEL_REQUEST,
    eVA_UI_FLUSH_TRACE_REQUEST,

    /* PVR Request */
    eVA_UI_GET_RECORD_INFO_REQUEST,
    eVA_UI_GET_PLAYBACK_INFO_REQUEST,
    eVA_UI_INCREMENT_PLAYBACK_COUNTER_REQUEST,

    /* Only use to know the end */
    eVA_UI_LAST_REQUEST

} tVA_UI_RequestType;

/*-------------------------------------------------------*/
/*                  REQUEST STATUS TYPE                  */
/*-------------------------------------------------------*/
typedef enum
{
    eVA_UI_OK_STATUS,
    eVA_UI_ARRAY_TOO_SMALL_STATUS,
    eVA_UI_WRONG_PIN_CODE_STATUS,
    eVA_UI_LOCKED_PIN_CODE_STATUS,
    eVA_UI_MAX_CHANGES_PIN_CODE_STATUS,
    eVA_UI_FORBIDDEN_PIN_CODE_STATUS,
    eVA_UI_NO_PIN_CODE_STATUS,
    eVA_UI_INVALID_MATURITY_RATING_STATUS,
    eVA_UI_SMARTCARD_INSUFFICIENT_MEMORY_STATUS,
    eVA_UI_WRONG_SLOT_STATUS,
    eVA_UI_SMARTCARD_ERROR_STATUS,
    eVA_UI_WRONG_SOID_STATUS,
    eVA_UI_OPERATION_NOT_ACCEPTED_STATUS,
    eVA_UI_PRESELECTION_AREA_FULL_STATUS,
    eVA_UI_REFERENCE_ALREADY_STORED_IN_PRESELECTION_AREA_STATUS,
    eVA_UI_REFERENCE_NOT_FOUND_IN_PRESELECTION_AREA_STATUS,
    eVA_UI_PRESELECTION_MODE_NOT_ACCEPTED_STATUS,
    eVA_UI_CONSULTATION_LOCKED_STATUS,
    eVA_UI_WRONG_HANDLE_STATUS,
    eVA_UI_NO_IPPV_OFFER_AVAILABLE_STATUS,
    eVA_UI_NO_ITEM_FOUND_STATUS,
    eVA_UI_INSUFFICIENT_MEMORY_STATUS,
    eVA_UI_INFO_NOT_AVAILABLE_STATUS,
    eVA_UI_WRONG_ACS_ID_STATUS,
    eVA_UI_RESOURCE_BUSY_STATUS,
    eVA_UI_OPERATING_ERROR_STATUS,
    eVA_UI_MODE_NOT_SETUP_STATUS,
    eVA_UI_WRONG_MODE_STATUS,
    eVA_UI_NO_STB_ID_STATUS,
    eVA_UI_PVR_NO_RIGHTS_STATUS,
    eVA_UI_CONNECTION_ERROR_STATUS,
    eVA_UI_WRONG_ACTIVATION_CODE_STATUS,
    eVA_UI_ACTIVATION_ERROR_STATUS,
    eVA_UI_TERMINAL_NOT_ACTIVATED_STATUS,
    eVA_UI_TERMINAL_LOCKED_STATUS,

    eVA_UI_ERROR_STATUS,

    /* Used to know the end */
    eVA_UI_LAST_STATUS

} tVA_UI_RequestStatus;

typedef enum
{
    eVA_UI_SMARTCARD_INSERTED_STATE,
    eVA_UI_SMARTCARD_EXTRACTED_STATE,
    eVA_UI_SMARTCARD_READY_STATE,
    eVA_UI_SMARTCARD_FAILED_STATE,
    eVA_UI_SMARTCARD_NOT_ACCEPTED_STATE,
    eVA_UI_SMARTCARD_INVALIDATED_STATE

} tVA_UI_SmartcardState;

typedef enum
{
    eVA_UI_TERMINAL_NOT_ACTIVATED_STATE,
    eVA_UI_TERMINAL_READY_STATE,
    eVA_UI_TERMINAL_MEMORY_ERROR_STATE,
    eVA_UI_TERMINAL_INVALIDATED_STATE
} tVA_UI_TerminalState;

typedef enum
{
    eVA_UI_CAMLOCK_ACTIVATED_OK      = 0x01,
    eVA_UI_CAMLOCK_ACTIVATED_KO,
    eVA_UI_CAMLOCK_ACTIVATED_NO_CARD,
    eVA_UI_CAMLOCK_NOT_ACTIVATED,
    eVA_UI_CAMLOCK_DISABLED

} tVA_UI_CamlockDynamicState;

typedef enum
{
    eVA_UI_THEME_LEVEL_REFERENCE =1,
    eVA_UI_CLASS_REFERENCE,
    eVA_UI_PREBOOKED_REFERENCE,
    eVA_UI_IPPVP_REFERENCE,
    eVA_UI_IPPVT_REFERENCE

} tVA_UI_ReferenceType;

typedef enum
{
    eVA_UI_WRITE_ACTION,
    eVA_UI_DELETE_ACTION

} tVA_UI_ReferenceAction;

typedef enum
{
    eVA_POST_PAYMENT_MODE,
    eVA_PRE_PAYMENT_MODE,
    eVA_UNKNOWN_PAYMENT_MODE

} tVA_UI_PaymentMode;

typedef enum
{
    eVA_UI_LOW_PRIORITY,
    eVA_UI_HIGH_PRIORITY,
    eVA_UI_ALL_PRIORITY

} tVA_UI_OperatorPriority;

typedef enum
{
      eVA_UI_REASON_VOD_1_0           = 0x0,
      eVA_UI_REASON_VOD_2_0           = 0x1,
      eVA_UI_REASON_LAST              = 0xFFFF

} tVA_UI_CallReason;

typedef enum
{
    eVA_UI_OPERATOR_MESSAGE_ENCODING_ASCII         = 0,
    eVA_UI_OPERATOR_MESSAGE_ENCODING_UTF_8         = 1,
    eVA_UI_OPERATOR_MESSAGE_ENCODING_UTF_16        = 2,
    eVA_UI_OPERATOR_MESSAGE_ENCODING_UTF_32        = 3,
    eVA_UI_OPERATOR_MESSAGE_ENCODING_GB2312_80     = 4

} tVA_UI_OperatorMessageEncoding;


typedef enum
{
    eVA_UI_CONNECTED_SERVER,
    eVA_UI_CONNECTIONLESS_SERVER

} tVA_UI_ServerConnectionMode;

typedef enum
{
    eVA_UI_CONNECTION_OK,
    eVA_UI_CONNECTION_KO,
    eVA_UI_CONNECTION_NO_ACTIVITY

} tVA_UI_ConnectionStatus;

typedef enum
{
    eVA_UI_PROCESSED_MESSAGE_OK,
    eVA_UI_PROCESSED_MESSAGE_KO

} tVA_UI_ProcessedMessageStatus;

typedef enum
{
    eVA_UI_ECM_TYPE,
    eVA_UI_EMM_GA_TYPE,
    eVA_UI_EMM_U_TYPE,
    eVA_UI_EMM_GH_TYPE,
    eVA_UI_EMM_S_TYPE,
    eVA_UI_EMM_STB_U_TYPE,
    eVA_UI_EMM_STB_GA_TYPE

} tVA_UI_ExmType;

typedef enum
{
    eVA_UI_EXM_PROCESS_OK,
    eVA_UI_EXM_EXTRA_PARAMETER,
    eVA_UI_EXM_PREVIEW,
    eVA_UI_EXM_SERVICE_RESTRICTION,
    eVA_UI_EXM_HASH_ERROR,
    eVA_UI_EXM_MISSING_SERVICE,
    eVA_UI_EXM_INSUFFICIENT_MEMORY,
    eVA_UI_EXM_GEOGRAPHICAL_BLACKOUT,
    eVA_UI_EXM_IPPV_MODE,
    eVA_UI_EXM_MATURITY_RATING,
    eVA_UI_EXM_DATE_ERROR,
    eVA_UI_EXM_MISSING_KEY,
    eVA_UI_EXM_PROCESS_ERROR

} tVA_UI_ExmSubmissionStatus;

typedef enum
{
    eVA_UI_NO_MODE           = 0,
    eVA_UI_AUTOPAIRING_MODE  = 1,
    eVA_UI_PAIRING_MODE      = 4

} tVA_UI_PairingMode;

typedef enum
{
    eVA_UI_LEVEL_NONE,
    eVA_UI_LEVEL_ERROR,
    eVA_UI_LEVEL_INFO,
    eVA_UI_LEVEL_VERBOSE

} tVA_UI_TraceLevel;

typedef enum
{
    eVA_UI_OUTPUT_NONE,
    eVA_UI_OUTPUT_STDOUT,
    eVA_UI_OUTPUT_STBOUT,
    eVA_UI_OUTPUT_IP,
    eVA_UI_OUTPUT_FILE

} tVA_UI_TraceOutputType;

typedef enum
{
    eVA_UI_HEADER_NONE,
    eVA_UI_HEADER_DATE,
    eVA_UI_HEADER_RUNTIME

} tVA_UI_TraceHeaderType;

typedef enum
{
    eVA_UI_MODE_LIVE,
    eVA_UI_MODE_AUTOFLUSH,
    eVA_UI_MODE_MANUAL_FLUSH

} tVA_UI_TraceMode;


/*********************************************************/
/*                  STRUCTURES & UNIONS                  */
/*********************************************************/

typedef struct
{
    tVA_UI_ProgramNumber       uiMin;
    tVA_UI_ProgramNumber       uiMax;

} tVA_UI_ProgramNumberRange;


typedef struct
{                       /* If a date is before 1/1/1980, viaccess  */
    UINT16    uiYear;   /* transforms it in 1/1/1980.              */
    UINT8     uiMonth;  /* If after 12/31/2107,                    */
    UINT8     uiDay;    /* it transforms it in 12/31/2107...       */

} tVA_UI_Date;

typedef struct
{
    UINT16    uiYear;
    UINT16    uiMonth;
    UINT16    uiDay;
    UINT16    uiHour;
    UINT16    uiMinute;
    UINT16    uiSecond;

} tVA_UI_DateEx;

typedef struct
{
    tVA_UI_Date     stMin;   /* minimum is 1/1/1980  */
    tVA_UI_Date     stMax;   /* maximum is 12/31/2107*/

} tVA_UI_DateRange;

typedef struct
{
    tVA_UI_Ua               pUa;
    tVA_UI_SmartcardState   eSmartcardState;

} tVA_UI_SmartcardInfo;

typedef struct
{
    tVA_UI_Ua               pUa;
    tVA_UI_TerminalState    eTerminalState;

} tVA_UI_TerminalInfo;


typedef struct
{
    tVA_UI_DateRange        stDateRange;
    UINT8                   uiTheme;
    UINT8                   uiLevel;

} tVA_UI_ThemeLevelItem;

typedef struct
{
    tVA_UI_DateRange        stDateRange;
    UINT32                  uiClassMaskSize;
    BYTE                    pClassMaskBuffer[kVA_UI_MAX_CLASS_MASK_SIZE];

} tVA_UI_ClassItem;

typedef struct
{
    INT32   iInteger;
    UINT16  uiFraction;

} tVA_UI_Price;

typedef struct
{
    tVA_UI_Date                stTotamDate;
    tVA_UI_Price               stTotam;
    tVA_UI_Date                stOverDate;
    tVA_UI_Price               stOver;
    tVA_UI_Price               stSum1;
    tVA_UI_Price               stSum2;
    UINT32                     uiPurchaseNumber;

} tVA_UI_DetailedCreditInfo;

typedef struct
{
    BOOL                    bIsIppvAgreementNeeded;
    BOOL                    bIsIppvPresent;


} tVA_UI_IppvInfo;

typedef struct
{
    tVA_UI_ProgramNumber    uiProgramNumber;
    tVA_UI_Price            stCost;

} tVA_UI_IppvItem;

typedef struct
{
    BOOL        bIsPreviewPresent;
    UINT8       uiPreviewIndex;
    UINT16      uiPreviewNumber;
    UINT8       uiMaxCwNumber;

} tVA_UI_PreviewInfo;

typedef struct
{
    BOOL        bIsMaturityRatingPresent;
    BOOL        bIsMaturityRatingAgreementNeeded;
    BOOL        bIsMaturityRatingExceeded;
    UINT8       uiMaturityRatingValue;

} tVA_UI_MaturityRatingInfo;

typedef struct
{
    tVA_UI_Date     stDate;
    UINT8           uiTheme;
    UINT8           uiLevel;

} tVA_UI_ThemeLevelReference;

typedef struct
{
    tVA_UI_Date     stDate;
    UINT8           uiClass;

} tVA_UI_ClassReference;

typedef struct
{
    tVA_UI_ProgramNumber    uiProgramNumber;
    tVA_UI_Price            stPrice;

} tVA_UI_IppvPReference;

typedef struct
{
    tVA_UI_ProgramNumber    uiProgramNumber;
    tVA_UI_Price            stCeiling;

} tVA_UI_IppvTReference;

typedef union
{
    tVA_UI_ThemeLevelReference      stThemeLevelReference;
    tVA_UI_ClassReference           stClassReference;
    tVA_UI_ProgramNumber            uiPrebookedReference;
    tVA_UI_IppvPReference           stIppvPReference;
    tVA_UI_IppvTReference           stIppvTReference;

} tVA_UI_ReferenceInfo;

typedef struct
{
    tVA_UI_Soid             uiSoid;
    tVA_UI_ReferenceType    eType;
    tVA_UI_ReferenceInfo    uInfo;

} tVA_UI_ReferenceItem;

typedef struct
{
    UINT8           uiNP1;
    UINT8           uiNP2;
    BOOL            bIsDateValid;
    tVA_UI_Date     stDate;

} tVA_UI_FacItem;

typedef struct
{
    tVA_UI_ConnectionStatus eConnectionStatus;
    BOOL                    bIsConnectionDateValid;
    tVA_UI_DateEx           stConnectionDate;
    BOOL                    bIsErrorDateValid;
    tVA_UI_DateEx           stErrorDate;

} tVA_UI_ConnectedServerInfo;

typedef struct
{
    BOOL                            bIsMessageDateValid;
    tVA_UI_DateEx                   stMessageDate;
    tVA_UI_ProcessedMessageStatus   eProcessedMessageStatus;
    BOOL                            bIsProcessedMessageDateValid;
    tVA_UI_DateEx                   stProcessedMessageDate;

} tVA_UI_ConnectionlessServerInfo;

typedef struct
{
    char        szAddress[kVA_UI_MAX_ADDRESS_SIZE+1];
    UINT16      uiPort;

} tVA_UI_IpAddress;

typedef union
{
    tVA_UI_ConnectedServerInfo          stConnectedServerInfo;
    tVA_UI_ConnectionlessServerInfo stConnectionlessServerInfo;

} tVA_UI_ServerInfo;

typedef struct
{
    char                        pServerType[kVA_UI_MAX_SERVER_TYPE_SIZE+1];
    tVA_UI_IpAddress            stIpAddress;
    tVA_UI_ServerConnectionMode eServerConnectionMode;
    tVA_UI_ServerInfo           uServerInfo;

} tVA_UI_NetworkItem;

typedef struct
{
    char                szModuleName[kVA_UI_MAX_MODULE_NAME_LENGTH+1];
    tVA_UI_TraceLevel   eTraceLevel;

} tVA_UI_TraceModule;

typedef union
{
    tVA_UI_IpAddress    stOutputIpAddress;
    char                szOutputFilename[kVA_UI_MAX_FILENAME_LENGTH];
} tVA_UI_TraceOutputParameters;


/*********************************************************/
/*            REQUESTS STRUCTURE DEFINITION              */
/*********************************************************/

/*----------------------------------------------*/
/*      eVA_UI_GET_VERSION_NAME_REQUEST         */
/*----------------------------------------------*/
typedef struct
{
    char        pVersionName[kVA_UI_MAX_VERSION_NAME_SIZE+1];

} tVA_UI_GetVersionNameRequest;

/*---------------------------------------------*/
/*      eVA_UI_GET_VERSION_REQUEST             */
/*---------------------------------------------*/
typedef struct
{
    UINT32      uiDeliveryVersionNumber;
    UINT32      uiMajorVersionNumber;
    UINT32      uiMinorVersionNumber;
    UINT32      uiEditionVersionNumber;
    UINT32      uiRevisionVersionNumber;
    char        pCasVariant[kVA_UI_MAX_CAS_VARIANT_SIZE+1];
    DWORD       dwOptionMask;
    char        pOperatorName[kVA_UI_MAX_OPERATOR_NAME_SIZE+1];

}tVA_UI_GetVersionRequest;

/*------------------------------------------------------*/
/*        eVA_UI_GET_ACCESS_CONDITION_REQUEST           */
/*------------------------------------------------------*/
typedef struct
{
    DWORD                       dwAcsId;
    DWORD                       dwStbStreamHandle;
    DWORD                       dwStreamType;
    tVA_UI_EventType            eAccessConditionEvent;
    tVA_UI_Soid                 uiSoid;
    tVA_UI_IppvInfo             stIppvInfo;
    tVA_UI_MaturityRatingInfo   stMaturityRatingInfo;
    tVA_UI_PreviewInfo          stPreviewInfo;

} tVA_UI_GetAccessConditionRequest;

/*---------------------------------------------*/
/*      eVA_UI_GET_SMARTCARD_INFO_REQUEST      */
/*---------------------------------------------*/
typedef struct
{
    tVA_UI_SmartcardInfo    stSmartcardInfo;

} tVA_UI_GetSmartcardInfoRequest;

/*---------------------------------------------*/
/*   eVA_UI_GET_SMARTCARD_LIST_INFO_REQUEST    */
/*---------------------------------------------*/
typedef struct
{
    UINT8                   uiNumberOfAvailableSlot;
    tVA_UI_SmartcardInfo    pSmartcardInfo[kVA_SETUP_NBMAX_SC];

} tVA_UI_GetSmartcardListInfoRequest;

/*------------------------------------------------------*/
/*        eVA_UI_GET_UA_REQUEST                         */
/*------------------------------------------------------*/
typedef struct
{
    tVA_UI_Ua           pUa;

} tVA_UI_GetUaRequest;

/*------------------------------------------------------*/
/*         eVA_UI_GET_SOID_LIST_REQUEST                 */
/*------------------------------------------------------*/
typedef struct
{
    UINT32                  uiMaxItemNumber;
    UINT32                  uiItemNumber;
    tVA_UI_Soid             *pItemArray;

} tVA_UI_GetSoidListRequest;

/*------------------------------------------------------*/
/*        eVA_UI_GET_LABEL_REQUEST                      */
/*------------------------------------------------------*/
typedef struct
{
    tVA_UI_Soid         uiSoid;
    char                pLabel[kVA_UI_MAX_LABEL_SIZE+1];

} tVA_UI_GetLabelRequest;

/*------------------------------------------------------*/
/*     eVA_UI_GET_PIN_CODE_ATTEMPTS_REQUEST             */
/*------------------------------------------------------*/
typedef struct
{
    UINT8       uiRemainingAttemptsCount;
    UINT8       uiMaxAttempts;

} tVA_UI_GetPinCodeAttemptsRequest;

/*------------------------------------------------------*/
/*        eVA_UI_MODIFY_PIN_CODE_REQUEST                */
/*------------------------------------------------------*/
typedef struct
{
    tVA_UI_PinCode          pPinCode;
    tVA_UI_PinCode      pNewPinCode;

} tVA_UI_ModifyPinCodeRequest;

/*------------------------------------------------------*/
/*        eVA_UI_UNLOCK_CONSULTATION_REQUEST            */
/*------------------------------------------------------*/
typedef struct
{
    tVA_UI_PinCode      pPinCode;

} tVA_UI_UnlockConsultationRequest;

/*------------------------------------------------------*/
/*         eVA_UI_GET_CLASS_LIST_REQUEST                */
/*------------------------------------------------------*/
typedef struct
{
    tVA_UI_Soid             uiSoid;
    tVA_UI_DateRange        stDateRange;
    UINT32                  uiMaxItemNumber;
    UINT32                  uiItemNumber;
    tVA_UI_ClassItem        *pItemArray;

} tVA_UI_GetClassListRequest;

/*------------------------------------------------------*/
/*         eVA_UI_GET_THEME_LEVEL_LIST_REQUEST          */
/*------------------------------------------------------*/

typedef struct
{
    tVA_UI_Soid             uiSoid;
    tVA_UI_DateRange        stDateRange;
    UINT32                  uiMaxItemNumber;
    UINT32                  uiItemNumber;
    tVA_UI_ThemeLevelItem   *pItemArray;

} tVA_UI_GetThemeLevelListRequest;

/*------------------------------------------------------*/
/*         eVA_UI_GET_PREBOOKED_LIST_REQUEST            */
/*------------------------------------------------------*/


typedef struct
{
    tVA_UI_Soid                 uiSoid;
    tVA_UI_ProgramNumberRange   stProgramNumberRange;
    UINT32                      uiMaxItemNumber;
    UINT32                      uiItemNumber;
    tVA_UI_ProgramNumberRange   *pItemArray;

} tVA_UI_GetPreBookedListRequest;

/*---------------------------------------------*/
/*      eVA_UI_GET_PREVIEW_INFO_REQUEST        */
/*---------------------------------------------*/

typedef struct
{
    tVA_UI_Soid     uiSoid;
    UINT8           uiPreviewIndex;
    UINT16          uiPreviewNumber;
    UINT8           uiUsedCwNumber;

} tVA_UI_GetPreviewInfoRequest;

/*------------------------------------------------------*/
/*        eVA_UI_GET_MATURITY_RATING_REQUEST            */
/*------------------------------------------------------*/

typedef struct
{
    UINT8                   uiMaturityRating;
    UINT8                   uiMaxMaturityRating;

} tVA_UI_GetMaturityRatingRequest;

/*------------------------------------------------------*/
/*        eVA_UI_MODIFY_MATURITY_RATING_REQUEST         */
/*------------------------------------------------------*/
typedef struct
{
    tVA_UI_PinCode              pPinCode;
    UINT8                       uiNewMaturityRating;

} tVA_UI_ModifyMaturityRatingRequest;

/*------------------------------------------------------*/
/*        eVA_UI_ACCEPT_MATURITY_RATING_REQUEST         */
/*------------------------------------------------------*/
typedef struct
{
	DWORD						dwAcsId;
    tVA_UI_PinCode              pPinCode;

} tVA_UI_AcceptMaturityRatingRequest;

/*------------------------------------------------------*/
/*        eVA_UI_GET_PRESELECTION_REQUEST               */
/*------------------------------------------------------*/
typedef struct
{
    UINT32                  uiMaxItemNumber;
    UINT32                  uiItemNumber;
    tVA_UI_ReferenceItem    *pItemArray;

} tVA_UI_GetPreselectionRequest;

/*---------------------------------------------*/
/*  eVA_UI_GET_OPERATOR_PRESELECTION_REQUEST   */
/*---------------------------------------------*/
typedef struct
{
    tVA_UI_Soid             uiSoid;
    UINT32                  uiMaxItemNumber;
    UINT32                  uiItemNumber;
    tVA_UI_ReferenceItem    *pItemArray;

} tVA_UI_GetOperatorPreselectionRequest;

/*------------------------------------------------------*/
/*        eVA_UI_MODIFY_PRESELECTION_REQUEST            */
/*------------------------------------------------------*/
typedef struct
{
    tVA_UI_Soid                 uiSoid;
    tVA_UI_PinCode              pPinCode;
    tVA_UI_ReferenceAction      eAction;
    tVA_UI_ReferenceType        eType;
    tVA_UI_ReferenceInfo        uInfo;

} tVA_UI_ModifyPreselectionRequest;

/*-----------------------------------------------*/
/*  eVA_UI_GET_GEOGRAPHICAL_CODE_ADDRESS_REQUEST */
/*-----------------------------------------------*/
typedef struct
{
    tVA_UI_Soid     uiSoid;
    tVA_UI_Gca      pGca;

} tVA_UI_GetGeographicalCodeAddressRequest;

/*------------------------------------------------------*/
/*         eVA_UI_GET_FAC_DATA_REQUEST                  */
/*------------------------------------------------------*/
typedef struct
{
    tVA_UI_Soid         uiSoid;
    UINT8               uiNP1;
    UINT8               uiNP2;
    UINT32              uiMaxByteNumber;
    UINT32              uiByteNumber;
    BYTE                *pFacDataBuffer;

} tVA_UI_GetFacDataRequest;

/*------------------------------------------------------*/
/*         eVA_UI_GET_FAC_LIST_REQUEST                  */
/*------------------------------------------------------*/
typedef struct
{
    tVA_UI_Soid         uiSoid;
    UINT32              uiMaxItemNumber;
    UINT32              uiItemNumber;
    tVA_UI_FacItem      *pItemArray;

} tVA_UI_GetFacListRequest;

/*------------------------------------------------------*/
/*         eVA_UI_GET_IPPVP_LIST_REQUEST                */
/*------------------------------------------------------*/
typedef struct
{
    tVA_UI_Soid                 uiSoid;
    tVA_UI_ProgramNumberRange   stProgramNumberRange;
    UINT32                      uiMaxItemNumber;
    UINT32                      uiItemNumber;
    tVA_UI_IppvItem             *pItemArray;

} tVA_UI_GetIppvPListRequest;

/*------------------------------------------------------*/
/*         eVA_UI_GET_IPPVT_LIST_REQUEST                */
/*------------------------------------------------------*/
typedef struct
{
    tVA_UI_Soid                 uiSoid;
    tVA_UI_ProgramNumberRange   stProgramNumberRange;
    UINT32                      uiMaxItemNumber;
    UINT32                      uiItemNumber;
    tVA_UI_IppvItem             *pItemArray;

} tVA_UI_GetIppvTListRequest;

/*------------------------------------------------------*/
/*        eVA_UI_GET_CREDIT_INFO_REQUEST                */
/*------------------------------------------------------*/
typedef struct
{
    tVA_UI_Soid                 uiSoid;
    tVA_UI_Currency             pCurrency;
    tVA_UI_DetailedCreditInfo   stDetailedCreditInfo;
    tVA_UI_Price                stRemainingCredit;

} tVA_UI_GetCreditInfoRequest;

/*------------------------------------------------------*/
/*         eVA_UI_GET_IPPV_INFO_REQUEST                 */
/*------------------------------------------------------*/
typedef struct
{
    DWORD                   dwAcsId;
    DWORD                   dwStbStreamHandle;
    tVA_UI_Soid             uiSoid;
    tVA_UI_PaymentMode      ePaymentMode;
    tVA_UI_Currency         pCurrency;
    tVA_UI_Price            stRemainingCredit;
    BOOL                    bIsIppvPAgreementNeeded;
    BOOL                    bIsIppvPPresent;
    tVA_UI_ProgramNumber    uiPnumIppvP;
    tVA_UI_Price            stCostIppvP;
    BOOL                    bIsIppvTAgreementNeeded;
    BOOL                    bIsIppvTPresent;
    tVA_UI_ProgramNumber    uiPnumIppvT;
    tVA_UI_Price            stCostIppvT;
    tVA_UI_Price            stCeiling;

} tVA_UI_GetIppvInfoRequest;

/*------------------------------------------------------*/
/*        eVA_UI_ACCEPT_IPPVP_REQUEST                   */
/*------------------------------------------------------*/
typedef struct
{
    DWORD                   dwAcsId;
    DWORD                   dwStbStreamHandle;
    tVA_UI_PinCode          pPinCode;
    tVA_UI_ProgramNumber    uiProgramNumber;

} tVA_UI_AcceptIppvPRequest;

/*------------------------------------------------------*/
/*        eVA_UI_ACCEPT_IPPVT_REQUEST                    */
/*------------------------------------------------------*/
typedef struct
{
    DWORD                   dwAcsId;
    DWORD                   dwStbStreamHandle;
    tVA_UI_PinCode          pPinCode;
    tVA_UI_ProgramNumber    uiProgramNumber;
    tVA_UI_Price            stCeiling;

} tVA_UI_AcceptIppvTRequest;

/*------------------------------------------------------*/
/*        eVA_UI_GET_THRESHOLD_REQUEST                  */
/*------------------------------------------------------*/
typedef struct
{
    tVA_UI_Price        stThreshold;

} tVA_UI_GetThresholdRequest;

/*------------------------------------------------------*/
/*        eVA_UI_MODIFY_THRESHOLD_REQUEST               */
/*------------------------------------------------------*/
typedef struct
{
    tVA_UI_PinCode          pPinCode;
    tVA_UI_Price            stNewThreshold;

} tVA_UI_ModifyThresholdRequest;

/*------------------------------------------------------*/
/*        eVA_UI_START_IPPV_SURVEY_REQUEST              */
/*------------------------------------------------------*/
typedef struct
{
    tVA_UI_Soid     uiSoid;

} tVA_UI_StartIppvSurveyRequest;

/*------------------------------------------------------*/
/*        eVA_UI_CANCEL_IPPV_SURVEY_REQUEST             */
/*------------------------------------------------------*/

/* No structure needed */

/*------------------------------------------------------*/
/*        eVA_UI_GET_CAMLOCK_INFO_REQUEST               */
/*------------------------------------------------------*/
typedef struct
{
    tVA_UI_CamlockDynamicState  eCamlockDynamicState;
    char                        pCamlock[kVA_UI_MAX_CAMLOCK_SIZE+1];

} tVA_UI_GetCamlockInfoRequest;

/*---------------------------------------------*/
/*      eVA_UI_GET_RECORD_INFO_REQUEST       */
/*---------------------------------------------*/
typedef struct
{
    DWORD       dwAcsId;
    DWORD       dwStbStreamHandle;
    tVA_UI_Date stExpirationDate;
    UINT32      uiPlaybackDurationValidityInDay;
    UINT32      uiMaxNumberOfPlayback;

} tVA_UI_GetRecordInfoRequest;

/*---------------------------------------------*/
/*      eVA_UI_GET_PLAYBACK_INFO_REQUEST       */
/*---------------------------------------------*/
typedef struct
{
    DWORD           dwAcsId;
    UINT32          uiHeaderPlaybackCounter;
    tVA_UI_DateEx   stRecordDate;
    tVA_UI_DateEx   stFirstPlaybackDate;

} tVA_UI_GetPlaybackInfoRequest;

/*---------------------------------------------*/
/*  eVA_UI_INCREMENT_PLAYBACK_COUNTER_REQUEST  */
/*---------------------------------------------*/
typedef struct
{
    DWORD           dwAcsId;

} tVA_UI_IncrementPlaybackCounterRequest;

/*------------------------------------------------------*/
/*        eVA_UI_GET_OPERATOR_MESSAGE_REQUEST           */
/*------------------------------------------------------*/
typedef struct
{
    UINT32                          uiSoid;
    tVA_UI_OperatorPriority         ePriority;
    tVA_UI_OperatorMessageEncoding  eEncoding;
    UINT8                           uiMessageSize;
    BYTE                            pOperatorMessageBuffer[kVA_UI_MAX_OPERATOR_ITEM_SIZE];

} tVA_UI_GetOperatorMessageRequest;


/*------------------------------------------------------*/
/*        eVA_UI_GET_OPERATOR_DATA_REQUEST              */
/*------------------------------------------------------*/
typedef struct
{
    UINT32                          uiSoid;
    tVA_UI_OperatorPriority         ePriority;
    UINT8                           uiBufferSize;
    BYTE                            pOperatorDataBuffer[kVA_UI_MAX_OPERATOR_ITEM_SIZE];

} tVA_UI_GetOperatorDataRequest;

/*---------------------------------------------*/
/*      eVA_UI_SUBMIT_EXM_REQUEST              */
/*---------------------------------------------*/
typedef struct
{
    tVA_UI_ExmType              eExmType;
    UINT32                      uiExmSize;
    BYTE                        *pExmBuffer;
    tVA_UI_ExmSubmissionStatus  eExmSubmissionStatus;
    UINT32                      uiMaxReplyBufferSize;
    UINT32                      uiReplyBufferSize;
    BYTE                        *pReplyBuffer;

} tVA_UI_SubmitExmRequest;

/*---------------------------------------------*/
/*      eVA_UI_GET_PAIRING_MODE_REQUEST        */
/*---------------------------------------------*/
typedef struct
{
    tVA_UI_PairingMode    ePairingMode;

} tVA_UI_GetPairingModeRequest;

/*---------------------------------------------*/
/*      eVA_UI_GET_PAIRING_SMARTCARD_REQUEST   */
/*---------------------------------------------*/
typedef struct
{
    UINT32      uiMaxNumberOfSmartcard;
    UINT32      uiNumberOfSmartcard;

} tVA_UI_GetPairingSmartcardRequest;

/*---------------------------------------------*/
/*      eVA_UI_GET_PAIRING_UA_LIST_REQUEST     */
/*---------------------------------------------*/
typedef struct
{
    UINT32      uiUaNumber;
    tVA_UI_Ua   pUaArray[kVA_UI_MAX_PAIRING_SMARTCARD_NUMBER];

} tVA_UI_GetPairingUaListRequest;

/*---------------------------------------------*/
/*      eVA_UI_GET_STB_ID_REQUEST              */
/*---------------------------------------------*/
typedef struct
{
    tVA_UI_StbId    pStbId;
    BOOL            bIsDateValid;
    tVA_UI_DateEx   stDate;

} tVA_UI_GetStbIdRequest;

/*---------------------------------------------*/
/*      eVA_UI_ASK_HEADEND_PROCESS_REQUEST     */
/*---------------------------------------------*/
typedef struct
{
    tVA_UI_Soid     uiSoid;
    WORD            wReason;
    WORD            wApplicationId;
    WORD            wApplicationServerId;
    WORD            wOrmId;
    UINT32          uiOrmAddressSize;
    char*           pOrmAddress;
    UINT16          uiOrmPort;
    UINT32          uiApplicationRequestSize;
    BYTE            *pApplicationRequestBuffer;
    UINT32          uiMaxReplyBufferSize;
    UINT32          uiReplyBufferSize;
    BYTE            *pReplyBuffer;

} tVA_UI_AskHeadendProcessRequest;

/*---------------------------------------------*/
/*      eVA_UI_HEADEND_CREDIT_REQUEST          */
/*---------------------------------------------*/
typedef struct
{
    tVA_UI_Soid     uiSoid;
    tVA_UI_Ua       pUa;
    tVA_UI_Price    stCreditAmount;

} tVA_UI_HeadendCreditRequest;

/*---------------------------------------------*/
/*    eVA_UI_HEADEND_PROGRAM_NUMBER_REQUEST    */
/*---------------------------------------------*/
typedef struct
{
    tVA_UI_Soid             uiSoid;
    tVA_UI_Ua               pUa;
    tVA_UI_ProgramNumber    uiProgramNumber;

} tVA_UI_HeadendProgramNumberRequest;

/*---------------------------------------------*/
/*       eVA_UI_GET_TERMINAL_INFO_REQUEST      */
/*---------------------------------------------*/
typedef struct
{
    tVA_UI_TerminalInfo     stTerminalInfo;

} tVA_UI_GetTerminalInfoRequest;

/*-------------------------------------------------*/
/*   eVA_UI_GET_TERMINAL_ACCESS_CONDITION_REQUEST  */
/*-------------------------------------------------*/
typedef struct
{
    DWORD                       dwAcsId;
    DWORD                       dwStbStreamHandle;
    DWORD                       dwStreamType;
    tVA_UI_EventType            eAccessConditionEvent;
    tVA_UI_Soid                 uiSoid;
    tVA_UI_MaturityRatingInfo   stMaturityRatingInfo;
    tVA_UI_PreviewInfo          stPreviewInfo;

} tVA_UI_GetTerminalAccessConditionRequest;

/*---------------------------------------------*/
/*         eVA_UI_GET_TERMINAL_UA_REQUEST           */
/*---------------------------------------------*/
typedef struct
{
    tVA_UI_Ua       pUa;

} tVA_UI_GetTerminalUaRequest;

/*---------------------------------------------*/
/*      eVA_UI_GET_TERMINAL_SOID_LIST_REQUEST       */
/*---------------------------------------------*/
typedef struct
{
    UINT32          uiMaxItemNumber;
    UINT32          uiItemNumber;
    tVA_UI_Soid     *pItemArray;

} tVA_UI_GetTerminalSoidListRequest;

/*-----------------------------------------------*/
/*       eVA_UI_GET_TERMINAL_LABEL_REQUEST       */
/*-----------------------------------------------*/
typedef struct
{
    tVA_UI_Soid     uiSoid;
    char            pLabel[kVA_UI_MAX_LABEL_SIZE+1];

} tVA_UI_GetTerminalLabelRequest;


/*---------------------------------------------------*/
/*    eVA_UI_GET_TERMINAL_PIN_CODE_ATTEMPTS_REQUEST  */
/*---------------------------------------------------*/
typedef struct
{
    UINT8       uiRemainingAttemptsCount;
    UINT8       uiMaxAttempts;

} tVA_UI_GetTerminalPinCodeAttemptsRequest;

/*---------------------------------------------*/
/*    eVA_UI_MODIFY_TERMINAL_PIN_CODE_REQUEST  */
/*---------------------------------------------*/
typedef struct
{
    tVA_UI_PinCode      pPinCode;
    tVA_UI_PinCode      pNewPinCode;

} tVA_UI_ModifyTerminalPinCodeRequest;

/*--------------------------------------------------*/
/*       eVA_UI_GET_TERMINAL_CLASS_LIST_REQUEST     */
/*--------------------------------------------------*/
typedef struct
{
    tVA_UI_Soid         uiSoid;
    tVA_UI_DateRange    stDateRange;
    UINT32              uiMaxItemNumber;
    UINT32              uiItemNumber;
    tVA_UI_ClassItem    *pItemArray;

} tVA_UI_GetTerminalClassListRequest;

/*---------------------------------------------------*/
/*     eVA_UI_GET_TERMINAL_PREBOOKED_LIST_REQUEST    */
/*---------------------------------------------------*/
typedef struct
{
    tVA_UI_Soid                     uiSoid;
    tVA_UI_ProgramNumberRange       stProgramNumberRange;
    UINT32                          uiMaxItemNumber;
    UINT32                          uiItemNumber;
    tVA_UI_ProgramNumberRange       *pItemArray;

} tVA_UI_GetTerminalPrebookedListRequest;

/*----------------------------------------------*/
/*   eVA_UI_GET_TERMINAL_PREVIEW_INFO_REQUEST   */
/*----------------------------------------------*/
typedef struct
{
    tVA_UI_Soid     uiSoid;
    UINT8           uiPreviewIndex;
    UINT16          uiPreviewNumber;
    UINT8           uiUsedCwNumber;

} tVA_UI_GetTerminalPreviewInfoRequest;

/*-----------------------------------------------*/
/*  eVA_UI_GET_TERMINAL_MATURITY_RATING_REQUEST  */
/*-----------------------------------------------*/
typedef struct
{
    UINT8       uiMaturityRating;
    UINT8       uiMaxMaturityRating;

} tVA_UI_GetTerminalMaturityRatingRequest;

/*----------------------------------------------------*/
/*   eVA_UI_MODIFY_TERMINAL_MATURITY_RATING_REQUEST   */
/*----------------------------------------------------*/
typedef struct
{
    tVA_UI_PinCode      pPinCode;
    UINT8               uiNewMaturityRating;

} tVA_UI_ModifyTerminalMaturityRatingRequest;

/*----------------------------------------------------*/
/*   eVA_UI_ACCEPT_TERMINAL_MATURITY_RATING_REQUEST   */
/*----------------------------------------------------*/
typedef struct
{
	DWORD				dwAcsId;
    tVA_UI_PinCode      pPinCode;

} tVA_UI_AcceptTerminalMaturityRatingRequest;

/*---------------------------------------------*/
/*    eVA_UI_GET_TERMINAL_FAC_DATA_REQUEST     */
/*---------------------------------------------*/
typedef struct
{
    tVA_UI_Soid     uiSoid;
    UINT8           uiNP1;
    UINT8           uiNP2;
    UINT32          uiMaxByteNumber;
    UINT32          uiByteNumber;
    BYTE            *pFacDataBuffer;

} tVA_UI_GetTerminalFacDataRequest;

/*---------------------------------------------*/
/*     eVA_UI_SUBMIT_TERMINAL_EXM_REQUEST      */
/*---------------------------------------------*/
typedef struct
{
    tVA_UI_ExmType                      eExmType;
    UINT32                              uiExmSize;
    BYTE                                *pExmBuffer;
    tVA_UI_ExmSubmissionStatus          eExmSubmissionStatus;
    UINT32                              uiMaxReplyBufferSize;
    UINT32                              uiReplyBufferSize;
    BYTE                                *pReplyBuffer;

} tVA_UI_SubmitTerminalExmRequest;

/*---------------------------------------------*/
/*      eVA_UI_GET_TERMINAL_ID_REQUEST         */
/*---------------------------------------------*/
typedef struct
{
    tVA_UI_TerminalId   pTerminalId;

} tVA_UI_GetTerminalIdRequest;

/*-------------------------------------------------------*/
/*      eVA_UI_SET_TERMINAL_ACTIVATION_CODE_REQUEST      */
/*-------------------------------------------------------*/
typedef struct
{
    char  pActivationCode[kVA_UI_ACTIVATION_CODE_SIZE+1];

} tVA_UI_SetTerminalActivationCodeRequest;

/*------------------------------------------------------*/
/*       eVA_UI_GET_TERMINAL_NETWORK_SETUP_REQUEST      */
/*------------------------------------------------------*/
typedef struct
{
    tVA_UI_Soid             uiSoid;
    UINT32                  uiMaxItemNumber;
    UINT32                  uiItemNumber;
    tVA_UI_NetworkItem      *pItemArray;

} tVA_UI_GetTerminalNetworkSetupRequest;

/*------------------------------------------------------*/
/*       eVA_UI_GET_TRACE_OUTPUT_REQUEST                */
/*------------------------------------------------------*/
typedef struct
{
    tVA_UI_TraceOutputType          eOutputType;
    tVA_UI_TraceMode                eTraceMode;
    tVA_UI_TraceHeaderType          eHeaderType;
    UINT32                          uiMaxLineWidth;
    tVA_UI_TraceOutputParameters    uTraceOutputParam;

} tVA_UI_GetTraceOutputRequest;

/*------------------------------------------------------*/
/*       eVA_UI_SET_TRACE_OUTPUT_REQUEST                */
/*------------------------------------------------------*/
typedef struct
{
    tVA_UI_TraceOutputType          eOutputType;
    tVA_UI_TraceMode                eTraceMode;
    tVA_UI_TraceHeaderType          eHeaderType;
    UINT32                          uiMaxLineWidth;
    tVA_UI_TraceOutputParameters    uTraceOutputParam;

} tVA_UI_SetTraceOutputRequest;

/*------------------------------------------------------*/
/*       eVA_UI_GET_TRACE_LIST_REQUEST                  */
/*------------------------------------------------------*/
typedef struct
{
    UINT32                  uiMaxItemNumber;
    UINT32                  uiItemNumber;
    tVA_UI_TraceModule      *pItemArray;

} tVA_UI_GetTraceListRequest;

/*------------------------------------------------------*/
/*       eVA_UI_SET_TRACE_LEVEL_REQUEST                 */
/*------------------------------------------------------*/
typedef struct
{
    tVA_UI_TraceModule      stNewLevel;

} tVA_UI_SetTraceLevelRequest;

/*------------------------------------------------------*/
/*        eVA_UI_FLUSH_TRACE_REQUEST                    */
/*------------------------------------------------------*/

/* No structure needed */


/*********************************************************/
/*                  REQUEST UNIONS                       */
/*********************************************************/
typedef union _tVA_UI_RequestArg
{
    /*---------------------------------*/
    /* version request structure       */
    /*---------------------------------*/
    tVA_UI_GetVersionNameRequest                    stGetVersionNameRequest;
    tVA_UI_GetVersionRequest                        stGetVersionRequest;
    tVA_UI_GetAccessConditionRequest                stGetAccessConditionRequest;
    tVA_UI_GetSmartcardInfoRequest                  stGetSmartcardInfoRequest;
    tVA_UI_GetSmartcardListInfoRequest              stGetSmartcardListInfoRequest;
    tVA_UI_GetUaRequest                             stGetUaRequest;
    tVA_UI_GetSoidListRequest                       stGetSoidListRequest;
    tVA_UI_GetLabelRequest                          stGetLabelRequest;
    tVA_UI_GetPinCodeAttemptsRequest                stGetPinCodeAttemptsRequest;
    tVA_UI_ModifyPinCodeRequest                     stModifyPinCodeRequest;
    tVA_UI_UnlockConsultationRequest                stUnlockConsultationRequest;
    tVA_UI_GetClassListRequest                      stGetClassListRequest;
    tVA_UI_GetThemeLevelListRequest                 stGetThemeLevelListRequest;
    tVA_UI_GetPreBookedListRequest                  stGetPreBookedListRequest;
    tVA_UI_GetPreviewInfoRequest                    stGetPreviewInfoRequest;
    tVA_UI_GetMaturityRatingRequest                 stGetMaturityRatingRequest;
    tVA_UI_ModifyMaturityRatingRequest              stModifyMaturityRatingRequest ;
    tVA_UI_AcceptMaturityRatingRequest              stAcceptMaturityRatingRequest ;
    tVA_UI_GetPreselectionRequest                   stGetPreselectionRequest;
    tVA_UI_GetOperatorPreselectionRequest           stGetOperatorPreselectionRequest;
    tVA_UI_ModifyPreselectionRequest                stModifyPreselectionRequest;
    tVA_UI_GetGeographicalCodeAddressRequest        stGetGeographicalCodeAddressRequest;
    tVA_UI_GetFacDataRequest                        stGetFacDataRequest;
    tVA_UI_GetFacListRequest                        stGetFacListRequest;
    tVA_UI_SubmitExmRequest                         stSubmitExmRequest;
    tVA_UI_GetIppvPListRequest                      stGetIppvPListRequest;
    tVA_UI_GetIppvTListRequest                      stGetIppvTListRequest;
    tVA_UI_GetCreditInfoRequest                     stGetCreditInfoRequest;
    tVA_UI_GetIppvInfoRequest                       stGetIppvInfoRequest;
    tVA_UI_AcceptIppvPRequest                       stAcceptIppvPRequest;
    tVA_UI_AcceptIppvTRequest                       stAcceptIppvTRequest;
    tVA_UI_GetThresholdRequest                      stGetThresholdRequest;
    tVA_UI_ModifyThresholdRequest                   stModifyThresholdRequest;
    tVA_UI_StartIppvSurveyRequest                   stStartIppvSurveyRequest;
    tVA_UI_GetOperatorMessageRequest                stGetOperatorMessageRequest;
    tVA_UI_GetOperatorDataRequest                   stGetOperatorDataRequest;
    tVA_UI_GetCamlockInfoRequest                    stGetCamlockInfoRequest;
    tVA_UI_GetRecordInfoRequest                     stGetRecordInfoRequest;
    tVA_UI_GetPlaybackInfoRequest                   stGetPlaybackInfoRequest;
    tVA_UI_IncrementPlaybackCounterRequest          stIncrementPlaybackCounterRequest;
    tVA_UI_GetPairingModeRequest                    stGetPairingModeRequest;
    tVA_UI_GetPairingSmartcardRequest               stGetPairingSmartcardRequest;
    tVA_UI_GetPairingUaListRequest                  stGetPairingUaListRequest;
    tVA_UI_GetStbIdRequest                          stGetStbIdRequest;
    tVA_UI_AskHeadendProcessRequest                 stAskHeadendProcessRequest;
    tVA_UI_HeadendCreditRequest                     stHeadendCreditRequest;
    tVA_UI_HeadendProgramNumberRequest              stHeadendProgramNumberRequest;
    /*-------------------------------------------*/
    /* SmartCardLess request structure           */
    /*-------------------------------------------*/
    tVA_UI_GetTerminalInfoRequest                   stGetTerminalInfoRequest;
    tVA_UI_GetTerminalAccessConditionRequest        stGetTerminalAccessConditionRequest;
    tVA_UI_GetTerminalUaRequest                     stGetTerminalUaRequest;
    tVA_UI_GetTerminalSoidListRequest               stGetTerminalSoidListRequest;
    tVA_UI_GetTerminalLabelRequest                  stGetTerminalLabelRequest;
    tVA_UI_GetTerminalPinCodeAttemptsRequest        stGetTerminalPinCodeAttemptsRequest;
    tVA_UI_ModifyTerminalPinCodeRequest             stModifyTerminalPinCodeRequest;
    tVA_UI_GetTerminalClassListRequest              stGetTerminalClassListRequest;
    tVA_UI_GetTerminalPrebookedListRequest          stGetTerminalPrebookedListRequest;
    tVA_UI_GetTerminalPreviewInfoRequest            stGetTerminalPreviewInfoRequest;
    tVA_UI_GetTerminalMaturityRatingRequest         stGetTerminalMaturityRatingRequest;
    tVA_UI_ModifyTerminalMaturityRatingRequest      stModifyTerminalMaturityRatingRequest;
    tVA_UI_AcceptTerminalMaturityRatingRequest      stAcceptTerminalMaturityRatingRequest;
    tVA_UI_GetTerminalFacDataRequest                stGetTerminalFacDataRequest;
    tVA_UI_SubmitTerminalExmRequest                 stSubmitTerminalExmRequest;
    tVA_UI_GetTerminalIdRequest                     stGetTerminalIdRequest;
    tVA_UI_SetTerminalActivationCodeRequest         stSetTerminalActivationCodeRequest;
    tVA_UI_GetTerminalNetworkSetupRequest           stGetTerminalNetworkSetupRequest;

    /*-------------------------------------------*/
    /* Traces request structure                  */
    /*-------------------------------------------*/
    tVA_UI_GetTraceOutputRequest                    stGetTraceOutputRequest;
    tVA_UI_SetTraceOutputRequest                    stSetTraceOutputRequest;
    tVA_UI_GetTraceListRequest                      stGetTraceListRequest;
    tVA_UI_SetTraceLevelRequest                     stSetTraceLevelRequest;

} tVA_UI_RequestArg;

struct _stVA_UI_Request;
/* The call back function type for a VA_UI_Request */
typedef void (*tVA_UI_RequestCallback)(struct _stVA_UI_Request *pRequest);


/*------------------------------------------------------*/
/*                    REQUEST API                       */
/*------------------------------------------------------*/

typedef struct
{
    tVA_UI_RequestType      eType;
    tVA_UI_RequestStatus    eStatus;
    DWORD                   dwScSlot;
    tVA_UI_RequestCallback  pCallback;
    void                    *pUserArg;

} tVA_UI_RequestHeader;

typedef struct _stVA_UI_Request
{
    tVA_UI_RequestHeader     stRequestHeader;
    tVA_UI_RequestArg        uRequest;

} tVA_UI_Request;


INT VA_UI_Request(tVA_UI_Request *pRequest);


/*------------------------------------------------------*/
/*                     EVENT API                        */
/*------------------------------------------------------*/
/* Parameters of the notification callback */
typedef struct
{
    tVA_UI_EventType    eEventType;             /* The event that has been received */
    DWORD               dwScSlot;               /* The Smart Card slot that may have generated the message */
    DWORD               dwAcsId;                /* The Identifier of the ACS that may have generated the message */
    DWORD               dwStbStreamHandle;      /* Handle that allows users to ask for more information */
    void                *pUserArg;              /* Gives back arg of registration */
    BOOL                bIsScrambledByProgram;  /* Indicates if the stream i scrambled by program or by component*/
    UINT32              uiStreamsNumber;        /* Indicates the number of streams of the program */

} tVA_UI_Event;


/* Definition of the Callback function */
typedef void (*tVA_UI_EventCallback)(tVA_UI_Event *pEvent);

/* Definition of the registration function */
DWORD VA_UI_AddEventListener(tVA_UI_EventCallback pEventCallbackFunction, void  *pUserArg);

/* Definition of the unregistration function */
INT VA_UI_RemoveEventListener(DWORD dwVaListenerHandle);

#endif

