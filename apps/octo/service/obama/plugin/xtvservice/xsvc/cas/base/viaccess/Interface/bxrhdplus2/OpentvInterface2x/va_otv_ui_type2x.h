/*
    COPYRIGHT VIACCESS S.A.
    date        : 06/02/2006
    file        : va_otv_ui_type2x.h
    Description : It is the Viaccess'API of CAS functionalities

    $Revision: 1 $
*/

#ifndef _VA_OTV_UI_TYPE2X_H_
#define _VA_OTV_UI_TYPE2X_H_

#include "va_types.h"


/*******************************************************************/
/* SETUP */
#define kO_VA_SETUP_NBMAX_ACS               1
#define kO_VA_SETUP_NBMAX_SC                1


/*******************************************************************/
/* ERRORS */


/* Generic error codes */
#define kO_VA_OK                           (0)

#define kO_VA_ILLEGAL_HANDLE               (0xFFFFFFFF)
#define kO_VA_ERROR                        (-1)
#define kO_VA_INVALID_PARAMETER            (-2)
#define kO_VA_ALREADY_DONE                 (-3)
#define kO_VA_RESOURCE_BUSY                (-5)

#define kO_VA_NOT_IMPLEMENTED              8
/********************************************************/
/*                CONSTANT DEFINITION                   */
/********************************************************/

/*** Viaccess Constants ***/

/* Date management constants */
#define kO_VA_LOWER_DATE_YEAR                       1980
#define kO_VA_LOWER_DATE_MONTH                      1
#define kO_VA_LOWER_DATE_DAY                        1
#define kO_VA_HIGHER_DATE_YEAR                      2107
#define kO_VA_HIGHER_DATE_MONTH                     12
#define kO_VA_HIGHER_DATE_DAY                       31

/* Smartcard constants */
#define kO_VA_UI_PIN_CODE_SIZE                      8       /* length of PIN for the user */
#define kO_VA_UI_UA_SIZE                            5
#define kO_VA_UI_LOCKED                             0
#define kO_VA_UI_UNLOCKED                           1
#define kO_VA_UI_UNCHANGED                          0xFF
#define kO_VA_UI_DATA_COMMAND_MAX_SIZE              6

/* PPV management constants */
#define kO_VA_NOTHING_WITH_EVENT                    0
#define kO_VA_PURCHASE_EVENT                        1
#define kO_VA_CANCEL_EVENT                          2

/* EMM identifications constants */
#define kO_VA_TYPE_EMMGA                            0
#define kO_VA_TYPE_EMMGH                            1
#define kO_VA_TYPE_EMMU                             2

/* System constants */
#define kO_VA_UI_MAX_PE_LIST_SIZE                   32
#define kO_VA_VERSION_NUMBER_MAX_SIZE               2
#define kO_VA_UI_NOT_USED                           0xFFFFFFFF
#define kO_VA_UI_PARAM_NOT_USED                     0xFF
#define kO_VA_UI_STREAM_HANDLE_IS_NOT_USED          0xFFFFFFFF
/* SOID management constant */
#define kO_VA_UI_MAX_LABEL_SIZE                     16
#define kO_VA_UI_MAX_CLASS_MASK_SIZE                32
#define kO_VA_UI_MAX_CURRENCY_LABEL_SIZE            16

/* Integrated messaging system constant */
#define kO_VA_UI_MAX_OPERATOR_MSG_SIZE              200

/* CAMLOCK constant */
#define kO_VA_UI_MAX_CAMLOCK_SIZE                   16


/* Security capability constants */
#define kO_VA_UI_MAX_PAIRING_SMARTCARD_NUMBER       8
#define kO_VA_UI_MAX_PAIRING_STB_ID_NUMBER          3
#define kO_VA_UI_STB_ID_SIZE                        5

#define kO_VA_UI_POSITIVE                           0
#define kO_VA_UI_NEGATIVE                           1
#define kO_VA_UI_MAX_CAS_VARIANT_SIZE               64
#define kO_VA_UI_MAX_OPERATOR_NAME_SIZE             32
#define kO_VA_UI_MAX_VERSION_NAME_SIZE              64
#define kO_VA_UI_MAX_LISTENER                       8  /* The Maximum number of user */
#define kO_VA_UI_MAX_APPLICATION_MSG_SIZE           233
#define kO_VA_MAX_HEADEND_MSG_SIZE


/*define if stream are by component or program*/
#define kO_VA_UI_SCRAMBLED_BY_COMPONENT             0x00
#define kO_VA_UI_SCRAMBLED_BY_PROGRAM               0xFF

/*********************************************************/
/*                  GENERAL TYPES                        */
/*********************************************************/

typedef UINT32  tO_VA_UI_ProgramNumber;                     /* Program numbers     */
typedef UINT32  tO_VA_UI_Soid;                              /* Service Operator Id */
typedef BYTE    tO_VA_UI_PinCode[kO_VA_UI_PIN_CODE_SIZE];   /* PIN code            */
typedef BYTE    tO_VA_UI_Ua[kO_VA_UI_UA_SIZE];              /* Unique Adress       */
typedef BYTE    tO_VA_UI_StbId[kO_VA_UI_STB_ID_SIZE];       /* Stb Id              */

/* Money, tokens, credits */

typedef struct
{
    UINT16  uiInteger;
    UINT16  uiFraction;

} tO_VA_UI_Token;

/* Dates */

typedef struct
{                       /* If a date is before 1/1/1980, viaccess  */
    UINT16    uiYear;   /* transforms it in 1/1/1980.              */
    UINT8     uiMonth;  /* If after 12/31/2107,                    */
    UINT8     uiDay;    /* it transforms it in 12/31/2107...       */

} tO_VA_UI_Date;

typedef struct
{
    UINT16    uiYear;
    UINT16    uiMonth;
    UINT16    uiDay;
    UINT16    uiHour;
    UINT16    uiMinute;
    UINT16    uiSecond;

} tO_VA_UI_DateEx;

typedef struct
{
    tO_VA_UI_Date     stMin;   /* minimum is 1/1/1980  */
    tO_VA_UI_Date     stMax;   /* maximum is 12/31/2107*/

} tO_VA_UI_DateRange;


typedef struct
{
    DWORD       dwPreselectionLock;
    DWORD       dwConsultationLock;
    DWORD       dwMaturityRatingLock;
    DWORD       dwIppvLock;

} tO_VA_UI_Lock;


typedef struct
{
    tO_VA_UI_Date     stDate;
    UINT8           uiTheme;
    UINT8           uiLevel;

} tO_VA_UI_ThemeLevelReference;

typedef struct
{
    tO_VA_UI_Date     stDate;
    UINT8           uiClass;

} tO_VA_UI_ClassReference;

typedef struct
{
    tO_VA_UI_ProgramNumber    uiProgramNumber;
    tO_VA_UI_Token            stPrice;

} tO_VA_UI_IppvPReference;

typedef struct
{
    tO_VA_UI_ProgramNumber    uiProgramNumber;
    tO_VA_UI_Token            stCeiling;

} tO_VA_UI_IppvTReference;


/* Action to apply on message type */
typedef enum {
    eO_VA_ACTION_ADD,     /* Receive the messages */
    eO_VA_ACTION_REMOVE,  /* No more receive the messages */
    eO_VA_NEXT_ACTION     /* Only use to know the end */
} tO_VA_Action;

/* Type of events/messages the application want to receive */
typedef enum {
    eO_VA_ALL_EVENT =0, /* All the messages are wanted          */
    eO_VA_CARD_EVENT,   /* Messages smartcard are wanted        */
    eO_VA_SYSTEM_EVENT, /* Messages system are wanted           */
    eO_VA_SUB_EVENT,    /* Messages subscription are wanted     */
    eO_VA_NEXT_EVENT    /* Only use to know the end             */
} tO_VA_EventClass;

/*********************************************************/
/*                  Open TV message                      */
/*********************************************************/
typedef enum
{
    /*the requests*/
    eO_VA_UI_GET_SOID_LIST_REQUEST                  = 0,
    eO_VA_UI_GET_THEME_LEVEL_LIST_REQUEST,
    eO_VA_UI_GET_CLASS_LIST_REQUEST,
    eO_VA_UI_GET_PREBOOKED_LIST_REQUEST,
    eO_VA_UI_GET_FAC_DATA_REQUEST,
    eO_VA_UI_GET_LABEL_REQUEST,
    eO_VA_UI_GET_UA_REQUEST,
    eO_VA_UI_GET_LOCK_REQUEST,
    eO_VA_UI_GET_PRESELECTION_REQUEST,
    eO_VA_UI_GET_ACCESS_CONDITION_REQUEST,
    eO_VA_UI_GET_PIN_CODE_ATTEMPTS_REQUEST,
    eO_VA_UI_MODIFY_PIN_CODE_REQUEST,
    eO_VA_UI_MODIFY_MATURITY_RATING_REQUEST,
    eO_VA_UI_MODIFY_LOCK_REQUEST,
    eO_VA_UI_MODIFY_PRESELECTION_REQUEST,
    eO_VA_UI_GET_PREVIEW_INFO_REQUEST,
    eO_VA_UI_GET_MATURITY_RATING_REQUEST,
    eO_VA_UI_GET_CAMLOCK_INFO_REQUEST,
    eO_VA_UI_GET_OPERATOR_MESSAGE_REQUEST,
    eO_VA_UI_GET_IPPVP_LIST_REQUEST,
    eO_VA_UI_GET_IPPVT_LIST_REQUEST,
    eO_VA_UI_GET_CREDIT_INFO_REQUEST,
    eO_VA_UI_GET_CEILING_REQUEST,
    eO_VA_UI_GET_THRESHOLD_REQUEST,
    eO_VA_UI_GET_IPPV_PARAMETER_REQUEST,
    eO_VA_UI_ACCEPT_IPPV_REQUEST,
    eO_VA_UI_CANCEL_IPPV_REQUEST,
    eO_VA_UI_MODIFY_CEILING_REQUEST,
    eO_VA_UI_MODIFY_THRESHOLD_REQUEST,
    eO_VA_UI_GET_VERSION_REQUEST,
    eO_VA_UI_GET_VERSION_NAME_REQUEST,
    eO_VA_UI_GET_SMARTCARD_INFO_REQUEST,
    eO_VA_UI_GET_PAIRING_LEVEL_REQUEST,
    eO_VA_UI_GET_PAIRING_MODE_REQUEST,
    eO_VA_UI_GET_PAIRING_SMARTCARD_REQUEST,
    eO_VA_UI_GET_PAIRING_UA_LIST_REQUEST,
    eO_VA_UI_GET_PAIRING_STB_ID_LIST_REQUEST,
    eO_VA_UI_GET_STB_ID_REQUEST,
    eO_VA_UI_START_IPPV_SURVEY_REQUEST,
    eO_VA_UI_CANCEL_IPPV_SURVEY_REQUEST,

    /*the event*/
    /* Subscription */
    eO_VA_UI_DESCRAMBLING_OK_EVENT                          = 0x8000,
    eO_VA_UI_DESCRAMBLING_OK_RIGHT_OK_EVENT,
    eO_VA_UI_DESCRAMBLING_OK_PREVIEW_EVENT,
    eO_VA_UI_DESCRAMBLING_OK_PREVIEW_AND_IPPV_OFFER_EVENT,

    eO_VA_UI_DESCRAMBLING_KO_EVENT                          =0x8100,
    eO_VA_UI_DESCRAMBLING_KO_NO_RIGHT_EVENT,
    eO_VA_UI_DESCRAMBLING_KO_MATURITY_RATING_EVENT,
    eO_VA_UI_DESCRAMBLING_KO_GEOGRAPHICAL_BLACKOUT_EVENT,
    eO_VA_UI_DESCRAMBLING_KO_MISSING_KEY_EVENT,
    eO_VA_UI_DESCRAMBLING_KO_IPPV_OFFER_EVENT,
    eO_VA_UI_DESCRAMBLING_KO_IPPV_OFFER_EXPIRED_EVENT,
    eO_VA_UI_DESCRAMBLING_KO_MATURITY_RATING_AND_IPPV_OFFER_EVENT,
    eO_VA_UI_DESCRAMBLING_KO_FORBIDDEN_MODE_EVENT,
    eO_VA_UI_DESCRAMBLING_KO_INSUFFICIENT_CREDIT_EVENT,

    eO_VA_UI_FREE_TO_AIR_EVENT                              = 0x8200,
    eO_VA_UI_VIACCESS_NOT_APPLICABLE_EVENT,
    eO_VA_UI_OVERDRAFT_REACHED_EVENT,
    eO_VA_UI_THRESHOLD_REACHED_EVENT,

    /* SmartCard */
    eO_VA_UI_SMARTCARD_INSERTED_EVENT                       = 0x8400,
    eO_VA_UI_SMARTCARD_EXTRACTED_EVENT,
    eO_VA_UI_SMARTCARD_READY_EVENT,
    eO_VA_UI_SMARTCARD_NEEDED_EVENT,
    eO_VA_UI_SMARTCARD_FAILED_EVENT,
    eO_VA_UI_SMARTCARD_NOT_ACCEPTED_EVENT,
    eO_VA_UI_SMARTCARD_INSUFFICIENT_MEMORY_EVENT,
    eO_VA_UI_SMARTCARD_INVALIDATED_EVENT,

    /* System */
    eO_VA_UI_START_SECURITY_RESTRICTION_EVENT               = 0x8800,
    eO_VA_UI_STOP_SECURITY_RESTRICTION_EVENT,
    eO_VA_UI_NVM_INSUFFICIENT_MEMORY_EVENT,
    eO_VA_UI_NEW_OPERATOR_MESSAGE_EVENT,
    eO_VA_UI_CAMLOCK_ACTIVATED_OK_EVENT,
    eO_VA_UI_CAMLOCK_ACTIVATED_KO_EVENT,
    eO_VA_UI_CAMLOCK_ACTIVATED_NO_CARD_EVENT,
    eO_VA_UI_CAMLOCK_NOT_ACTIVATED_EVENT,
    eO_VA_UI_CAMLOCK_DISABLED_EVENT,
    
    eO_VA_UI_NO_RECORD_RIGHT_EVENT,
    eO_VA_UI_NO_TIMESHIFT_RIGHT_EVENT,


    eO_VA_UI_ORM_CONNECTION_OK_EVENT,
    eO_VA_UI_ORM_CONNECTION_KO_EVENT,
    eO_VA_UI_CW_SERVER_CONNECTION_OK_EVENT,
    eO_VA_UI_CW_SERVER_CONNECTION_KO_EVENT,
    
    eO_VA_UI_TERMINAL_NOT_ACTIVATED_EVENT,
    eO_VA_UI_TERMINAL_READY_EVENT,
    eO_VA_UI_TERMINAL_CONFIGURATION_ERROR_EVENT,
    eO_VA_UI_TERMINAL_INVALIDATED_EVENT,
    eO_VA_UI_TERMINAL_PERSONALIZATION_OK_EVENT,
    eO_VA_UI_TERMINAL_PERSONALIZATION_KO_EVENT,

    eO_VA_UI_LAST_MESSAGE
} tO_VA_UI_Message;

/*********************************************************/
/*                  REQUEST STATUS TYPE                  */
/*********************************************************/
typedef enum
{
    eO_VA_UI_OK_STATUS,
    eO_VA_UI_ARRAY_TOO_SMALL_STATUS,
    eO_VA_UI_WRONG_PIN_CODE_STATUS,
    eO_VA_UI_LOCKED_PIN_CODE_STATUS,
    eO_VA_UI_MAX_CHANGES_PIN_CODE_STATUS,
    eO_VA_UI_FORBIDDEN_PIN_CODE_STATUS,
    eO_VA_UI_NO_PIN_CODE_STATUS,
    eO_VA_UI_INVALID_MATURITY_RATING_STATUS,
    eO_VA_UI_SMARTCARD_INSUFFICIENT_MEMORY_STATUS,
    eO_VA_UI_WRONG_SOID_STATUS,
    eO_VA_UI_OPERATION_NOT_ACCEPTED_STATUS,
    eO_VA_UI_PRESELECTION_AREA_FULL_STATUS,
    eO_VA_UI_REFERENCE_ALREADY_STORED_IN_PRESELECTION_AREA_STATUS,
    eO_VA_UI_REFERENCE_NOT_FOUND_IN_PRESELECTION_AREA_STATUS,
    eO_VA_UI_PRESELECTION_MODE_NOT_ACCEPTED_STATUS,
    eO_VA_UI_SMARTCARD_LOCKED_STATUS,
    eO_VA_UI_WRONG_HANDLE_STATUS,
    eO_VA_UI_NO_IPPV_OFFER_AVAILABLE_STATUS,
    eO_VA_UI_NO_MESSAGE_FOUND_STATUS,
    eO_VA_UI_ERROR_STATUS,

    /*status for security capability*/

    eO_VA_UI_MODE_NOT_SETUP_STATUS,
    eO_VA_UI_WRONG_MODE_STATUS,
    eO_VA_UI_WRONG_LEVEL_STATUS,
    eO_VA_UI_NO_STB_ID_STATUS,
    eO_VA_UI_INVALID_SMARTCARD_STATUS,

    /*Status for IPCAS capability*/
    eO_VA_UI_ORM_UNREACHABLE_STATUS,
    eO_VA_UI_CONNECTION_BROKEN_BY_AS_STATUS,
    eO_VA_UI_CONNECTION_BROKEN_BY_ORM_STATUS,
    eO_VA_UI_CONNECTION_TIMEOUT_STATUS,

    eO_VA_UI_WRONG_SLOT_STATUS,
    eO_VA_UI_SMARTCARD_ERROR_STATUS,
    eO_VA_UI_ECM_WITH_NO_PREVIEW_STATUS,

    /* status for PVR capability */
    eO_VA_UI_INFO_NOT_AVAILABLE_STATUS,
    eO_VA_UI_WRONG_ACS_ID_STATUS,
    eO_VA_UI_PVR_NO_RIGHTS_STATUS,

    /* Used to know the end */
    eO_VA_UI_LAST_STATUS

} tO_VA_UI_RequestStatus;


/*********************************************************/
/*            REQUESTS STRUCTURE DEFINITION              */
/*********************************************************/



/*------------------------------------------------------*/
/*         eO_VA_UI_GET_SOID_LIST_REQUEST               */
/*------------------------------------------------------*/
typedef struct
{
    UINT32                  uiMaxItemNumber;
    UINT32                  uiItemNumber;
    tO_VA_UI_Soid             *pItemArray;

} tO_VA_UI_GetSoidListRequest;

/*------------------------------------------------------*/
/*         eO_VA_UI_GET_THEME_LEVEL_LIST_REQUEST        */
/*------------------------------------------------------*/
typedef struct
{
    tO_VA_UI_DateRange        stDateRange;
    UINT8                   uiTheme;
    UINT8                   uiLevel;

} tO_VA_UI_ThemeLevelItem;

typedef struct
{
    tO_VA_UI_Soid             uiSoid;
    tO_VA_UI_DateRange        stDateRange;
    UINT32                  uiMaxItemNumber;
    UINT32                  uiItemNumber;
    tO_VA_UI_ThemeLevelItem   *pItemArray;

} tO_VA_UI_GetThemeLevelListRequest;

/*------------------------------------------------------*/
/*         eO_VA_UI_GET_CLASS_LIST_REQUEST              */
/*------------------------------------------------------*/
typedef struct
{
    tO_VA_UI_DateRange        stDateRange;
    UINT32                  uiClassMaskSize;
    BYTE                    pClassMaskBuffer[kO_VA_UI_MAX_CLASS_MASK_SIZE];
} tO_VA_UI_ClassItem;

typedef struct
{
    tO_VA_UI_Soid             uiSoid;
    tO_VA_UI_DateRange        stDateRange;
    UINT32                  uiMaxItemNumber;
    UINT32                  uiItemNumber;
    tO_VA_UI_ClassItem        *pItemArray;
} tO_VA_UI_GetClassListRequest;

/*------------------------------------------------------*/
/*         eO_VA_UI_GET_PREBOOKED_LIST_REQUEST          */
/*------------------------------------------------------*/

typedef struct
{
    tO_VA_UI_ProgramNumber       uiMin;
    tO_VA_UI_ProgramNumber       uiMax;

} tO_VA_UI_ProgramNumberRange;


typedef struct
{
    tO_VA_UI_Soid                 uiSoid;
    tO_VA_UI_ProgramNumberRange   stProgramNumberRange;
    UINT32                      uiMaxItemNumber;
    UINT32                      uiItemNumber;
    tO_VA_UI_ProgramNumberRange        *pItemArray;

} tO_VA_UI_GetPreBookedListRequest;

/*------------------------------------------------------*/
/*         eO_VA_UI_GET_FAC_DATA_REQUEST                */
/*------------------------------------------------------*/

typedef struct
{
    tO_VA_UI_Soid         uiSoid;
    UINT8               uiNP1;
    UINT8               uiNP2;
    UINT32              uiMaxByteNumber;
    UINT32              uiByteNumber;
    BYTE                *pFacDataBuffer;

} tO_VA_UI_GetFacDataRequest;

/*------------------------------------------------------*/
/*        eO_VA_UI_GET_LABEL_REQUEST                    */
/*------------------------------------------------------*/
typedef struct
{
    tO_VA_UI_Soid         uiSoid;
    char                pLabel[kO_VA_UI_MAX_LABEL_SIZE+1];

} tO_VA_UI_GetLabelRequest;

/*------------------------------------------------------*/
/*        eO_VA_UI_GET_UA_REQUEST                       */
/*------------------------------------------------------*/
typedef struct
{
    tO_VA_UI_Ua           pUa;
} tO_VA_UI_GetUaRequest;

/*---------------------------------------------*/
/*      eO_VA_UI_GET_PREVIEW_INFO_REQUEST        */
/*---------------------------------------------*/

typedef struct 
{
    tO_VA_UI_Soid     uiSoid;
    UINT8           uiPreviewIndex;    
    UINT16          uiPreviewNumber;
    UINT8           uiUsedCwNumber; 

} tO_VA_UI_GetPreviewInfoRequest;

/*------------------------------------------------------*/
/*        eO_VA_UI_GET_MATURITY_RATING_REQUEST          */
/*------------------------------------------------------*/

typedef struct
{
    UINT8                   uiMaturityRating;
    UINT8                   uiMaxMaturityRating;

} tO_VA_UI_GetMaturityRatingRequest;

/*------------------------------------------------------*/
/*        eO_VA_UI_GET_LOCK_REQUEST                     */
/*------------------------------------------------------*/
typedef struct
{

    tO_VA_UI_Lock             stLock;

} tO_VA_UI_GetLockRequest;

/*------------------------------------------------------*/
/*        eO_VA_UI_GET_PRESELECTION_REQUEST             */
/*------------------------------------------------------*/

typedef enum
{
    eO_VA_UI_THEME_LEVEL_REFERENCE =1,
    eO_VA_UI_CLASS_REFERENCE,
    eO_VA_UI_PREBOOKED_REFERENCE,
    eO_VA_UI_IPPVP_REFERENCE,
    eO_VA_UI_IPPVT_REFERENCE

} tO_VA_UI_ReferenceType;

typedef union
{
    tO_VA_UI_ThemeLevelReference      stThemeLevelReference;
    tO_VA_UI_ClassReference           stClassReference;
    tO_VA_UI_ProgramNumber            uiPrebookedReference;
    tO_VA_UI_IppvPReference           stIppvPReference;
    tO_VA_UI_IppvTReference           stIppvTReference;

} tO_VA_UI_ReferenceInfo;

typedef struct
{
    tO_VA_UI_Soid                 uiSoid;
    tO_VA_UI_ReferenceType        eType;
    tO_VA_UI_ReferenceInfo        uInfo;

} tO_VA_UI_ReferenceItem;

typedef struct
{
    UINT32                  uiMaxItemNumber;
    UINT32                  uiItemNumber;
    tO_VA_UI_ReferenceItem    *pItemArray;

} tO_VA_UI_GetPreselectionRequest;



/*------------------------------------------------------*/
/*        eO_VA_UI_GET_ACCESS_CONDITION_REQUEST         */
/*------------------------------------------------------*/

typedef struct
{
    DWORD                   dwIppvHandle;
    BOOL                    bIsIppvLocked;
    BOOL                    bIsIppvPPresent;
    tO_VA_UI_ProgramNumber    uiPnumIppvP;
    tO_VA_UI_Token            stCostIppvP;
    BOOL                    bIsIppvTPresent;
    tO_VA_UI_ProgramNumber    uiPnumIppvT;
    tO_VA_UI_Token            stCostIppvT;
} tO_VA_UI_IppvInfo;

typedef struct
{
    BOOL        bIsMaturityRatingPresent;
    BOOL        bIsMaturityRatingLocked;
    BOOL        bIsMaturityRatingExceeded;
    UINT8       uiProgramMaturityRating;
} tO_VA_UI_MaturityRatingInfo;

typedef struct
{
    BOOL        bIsPreviewPresent;
    UINT8       uiPreviewIndex;
    UINT16      uiPreviewNumber;
    UINT8       uiMaxCwNumber;
} tO_VA_UI_PreviewInfo;

typedef struct
{
    DWORD                         dwVaStreamHandle;
    tO_VA_UI_Soid                 uiSoid;
    tO_VA_UI_IppvInfo             stIppvInfo;
    tO_VA_UI_MaturityRatingInfo   stMaturityRatingInfo;
    tO_VA_UI_PreviewInfo          stPreviewInfo;
    tO_VA_UI_Message              eAccessConditionEvent;
    DWORD                         dwStbStreamTag;
    DWORD                         dwStreamType;
} tO_VA_UI_GetAccessConditionRequest;


/*------------------------------------------------------*/
/*     eO_VA_UI_GET_PIN_CODE_ATTEMPTS_REQUEST           */
/*------------------------------------------------------*/
typedef struct
{
    UINT8       uiRemainingAttemptsCount;
    UINT8       uiMaxAttempts;

} tO_VA_UI_GetPinCodeAttemptsRequest;

/*------------------------------------------------------*/
/*        eO_VA_UI_MODIFY_PIN_CODE_REQUEST              */
/*------------------------------------------------------*/
typedef struct
{
    tO_VA_UI_PinCode      pPinCode;
    tO_VA_UI_PinCode      pNewPinCode;

} tO_VA_UI_ModifyPinCodeRequest;

/*------------------------------------------------------*/
/*        eO_VA_UI_MODIFY_MATURITY_RATING_REQUEST       */
/*------------------------------------------------------*/
typedef struct
{
    tO_VA_UI_PinCode              pPinCode;
    UINT8                       uiNewMaturityRating;

} tO_VA_UI_ModifyMaturityRatingRequest;

/*------------------------------------------------------*/
/*        eO_VA_UI_MODIFY_LOCK_REQUEST                  */
/*------------------------------------------------------*/
typedef struct
{
    tO_VA_UI_PinCode          pPinCode;
    tO_VA_UI_Lock             stLock;

} tO_VA_UI_ModifyLockRequest;

/*------------------------------------------------------*/
/*        eO_VA_UI_MODIFY_PRESELECTION_REQUEST          */
/*------------------------------------------------------*/

typedef enum
{
    eO_VA_UI_WRITE_ACTION,
    eO_VA_UI_DELETE_ACTION

} tO_VA_UI_ReferenceAction;


typedef struct
{
    tO_VA_UI_Soid                 uiSoid;
    tO_VA_UI_ReferenceAction      eAction;
    tO_VA_UI_ReferenceType        eType;
    tO_VA_UI_ReferenceInfo        uInfo;

} tO_VA_UI_ModifyPreselectionRequest;

/*------------------------------------------------------*/
/*        eO_VA_UI_GET_CAMLOCK_INFO_REQUEST             */
/*------------------------------------------------------*/
typedef enum
{
    eO_VA_UI_CAMLOCK_ACTIVATED_OK      = 0x01  ,
    eO_VA_UI_CAMLOCK_ACTIVATED_KO              ,
    eO_VA_UI_CAMLOCK_ACTIVATED_NO_CARD         ,
    eO_VA_UI_CAMLOCK_NOT_ACTIVATED             ,
    eO_VA_UI_CAMLOCK_DISABLED

} tO_VA_UI_CamlockDynamicState;

typedef struct
{
    tO_VA_UI_CamlockDynamicState  eCamlockDynamicState;

} tO_VA_UI_GetCamlockInfoRequest;


/*------------------------------------------------------*/
/*        eO_VA_UI_GET_OPERATOR_MESSAGE_REQUEST         */
/*------------------------------------------------------*/
typedef enum
{
    eO_VA_UI_LOW_PRIORITY_OPERATOR_MESSAGE,
    eO_VA_UI_HIGH_PRIORITY_OPERATOR_MESSAGE ,
    eO_VA_UI_ALL_PRIORITY_OPERATOR_MESSAGE

} tO_VA_UI_OperatorMessagePriority;

typedef enum
{
    eO_VA_UI_OPERATOR_MESSAGE_ENCODING_ASCII         = 0,
    eO_VA_UI_OPERATOR_MESSAGE_ENCODING_UTF_8         = 1,
    eO_VA_UI_OPERATOR_MESSAGE_ENCODING_UTF_16        = 2,
    eO_VA_UI_OPERATOR_MESSAGE_ENCODING_UTF_32        = 3,
    eO_VA_UI_OPERATOR_MESSAGE_ENCODING_GTB2312_80    = 4

} tO_VA_UI_OperatorMessageEncoding;

typedef struct
{
    tO_VA_UI_OperatorMessagePriority  ePriority;
    tO_VA_UI_OperatorMessageEncoding  eEncoding;
    UINT8                           uiMessageSize;
    BYTE                            pOperatorMessageBuffer[kO_VA_UI_MAX_OPERATOR_MSG_SIZE];

} tO_VA_UI_GetOperatorMessageRequest;


/*---------------------------------*/
/*           ppv requests          */
/*---------------------------------*/

/*------------------------------------------------------*/
/*         eO_VA_UI_GET_IPPVP_LIST_REQUEST              */
/*------------------------------------------------------*/

typedef struct
{
    tO_VA_UI_ProgramNumber    uiProgramNumber;
    tO_VA_UI_Token            stCost;
} tO_VA_UI_IppvItem;

typedef struct
{
    tO_VA_UI_Soid                 uiSoid;
    tO_VA_UI_ProgramNumberRange   stProgramNumberRange;
    UINT32                      uiMaxItemNumber;
    UINT32                      uiItemNumber;
    tO_VA_UI_IppvItem             *pItemArray;
} tO_VA_UI_GetIppvPListRequest;

/*------------------------------------------------------*/
/*         eO_VA_UI_GET_IPPVT_LIST_REQUEST              */
/*------------------------------------------------------*/
typedef struct
{
    tO_VA_UI_Soid                 uiSoid;
    tO_VA_UI_ProgramNumberRange   stProgramNumberRange;
    UINT32                      uiMaxItemNumber;
    UINT32                      uiItemNumber;
    tO_VA_UI_IppvItem             *pItemArray;

} tO_VA_UI_GetIppvTListRequest;

/*------------------------------------------------------*/
/*         eO_VA_UI_GET_IPPV_PARAMETER_REQUEST          */
/*------------------------------------------------------*/

typedef enum
{
    eO_VA_POST_PAYMENT_MODE,
    eO_VA_PRE_PAYMENT_MODE,
    eO_VA_UNKNOWN_PAYMENT_MODE
} tO_VA_UI_PaymentMode;

typedef struct
{
    UINT8                   uiConversionRateNumerator;
    UINT8                   uiConversionRateDenominator;
    UINT16                  uiIppvTPeriod;
    char                    pCurrency[kO_VA_UI_MAX_CURRENCY_LABEL_SIZE+1];
} tO_VA_UI_CurrencyInfo;

typedef struct
{
    tO_VA_UI_Soid             uiSoid;
    BOOL                    bIppvWithoutModem;
    tO_VA_UI_PaymentMode      ePaymentMode;
    tO_VA_UI_CurrencyInfo     stCurrencyInfo;
} tO_VA_UI_GetIppvParameterRequest;

/*------------------------------------------------------*/
/*        eO_VA_UI_GET_CREDIT_INFO_REQUEST              */
/*------------------------------------------------------*/
typedef struct
{
    tO_VA_UI_Date                stTotamDate;
    tO_VA_UI_Token               stTotam;
    tO_VA_UI_Date                stOverDate;
    tO_VA_UI_Token               stOver;
    tO_VA_UI_Token               stSum1;
    tO_VA_UI_Token               stSum2;
} tO_VA_UI_DetailledCreditInfo;


typedef struct
{
    INT32                       iSign;     /* kO_VA_UI_POSITIVE or kO_VA_UI_NEGATIVE */
    tO_VA_UI_Token                stCredit;

} tO_VA_UI_SignedCredit;

typedef struct
{
    tO_VA_UI_Soid                 uiSoid;
    tO_VA_UI_DetailledCreditInfo  stDetailledCreditInfo;
    tO_VA_UI_SignedCredit         stRemainingCredit;

} tO_VA_UI_GetCreditInfoRequest;

/*------------------------------------------------------*/
/*        eO_VA_UI_GET_CEILING_REQUEST                  */
/*------------------------------------------------------*/
typedef struct
{
    tO_VA_UI_Token        stCeiling;

} tO_VA_UI_GetCeilingRequest;

/*------------------------------------------------------*/
/*        eO_VA_UI_GET_THRESHOLD_REQUEST                */
/*------------------------------------------------------*/

typedef struct
{
    tO_VA_UI_Token        stThreshold;

} tO_VA_UI_GetThresholdRequest;

/*------------------------------------------------------*/
/*        eO_VA_UI_ACCEPT_IPPV_REQUEST                  */
/*------------------------------------------------------*/

typedef enum
{
    eO_VA_IPPVP_MODE = 0,
    eO_VA_IPPVT_MODE

} tO_VA_UI_PurchaseMode;

typedef struct
{
    DWORD                   dwIppvHandle;
    tO_VA_UI_PurchaseMode     ePurchaseMode;
    tO_VA_UI_ProgramNumber    uiProgramNumber;
    tO_VA_UI_Token            stPrice;

} tO_VA_UI_AcceptIppvRequest;

/*------------------------------------------------------*/
/*        eO_VA_UI_CANCEL_IPPV_REQUEST                  */
/*------------------------------------------------------*/
typedef struct
{
    DWORD        dwIppvHandle;

} tO_VA_UI_CancelIppvRequest;

/*------------------------------------------------------*/
/*        eO_VA_UI_MODIFY_CEILING_REQUEST               */
/*------------------------------------------------------*/
typedef struct
{
    tO_VA_UI_PinCode          pPinCode;
    tO_VA_UI_Token            stNewCeiling;

} tO_VA_UI_ModifyCeilingRequest;

/*------------------------------------------------------*/
/*        eO_VA_UI_MODIFY_THRESHOLD_REQUEST             */
/*------------------------------------------------------*/
typedef struct
{
    tO_VA_UI_PinCode          pPinCode;
    tO_VA_UI_Token            stNewThreshold;

} tO_VA_UI_ModifyThresholdRequest;

/*------------------------------------------------------*/
/*        eO_VA_UI_START_IPPV_SURVEY_REQUEST              */
/*------------------------------------------------------*/
typedef struct 
{
    tO_VA_UI_Soid		uiSoid;

} tO_VA_UI_StartIppvSurveyRequest;

/*------------------------------------------------------*/
/*        eO_VA_UI_CANCEL_IPPV_SURVEY_REQUEST             */
/*------------------------------------------------------*/

/* No structure needed */

/*---------------------------------------------*/
/*      eO_VA_UI_GET_VERSION_REQUEST           */
/*---------------------------------------------*/

typedef struct
{
    UINT32      uiDeliveryVersionNumber;
    UINT32      uiMajorVersionNumber;
    UINT32      uiMinorVersionNumber;
    UINT32      uiEditionVersionNumber;
    UINT32      uiRevisionVersionNumber;
    char        pCasVariant[kO_VA_UI_MAX_CAS_VARIANT_SIZE+1];
    DWORD       dwOptionMask;
    char        pOperatorName[kO_VA_UI_MAX_OPERATOR_NAME_SIZE+1];

}tO_VA_UI_GetVersionRequest;

/*----------------------------------------------*/
/*      eO_VA_UI_GET_VERSION_NAME_REQUEST       */
/*----------------------------------------------*/

typedef struct
{
    char        pVersionName[kO_VA_UI_MAX_VERSION_NAME_SIZE+1];

} tO_VA_UI_GetVersionNameRequest;


/*---------------------------------------------*/
/*      eO_VA_UI_GET_SMARTCARD_INFO_REQUEST    */
/*---------------------------------------------*/

typedef enum
{
    eO_VA_UI_SMARTCARD_EXTRACTED_STATE    = 0x00,
    eO_VA_UI_SMARTCARD_INSERTED_STATE     = 0x01,
    eO_VA_UI_SMARTCARD_READY_STATE        = 0x03,
    eO_VA_UI_SMARTCARD_NOT_ACCEPTED_STATE = 0x04,
    eO_VA_UI_SMARTCARD_FAILED_STATE       = 0x05,
    eO_VA_UI_SMARTCARD_LAST_STATE         = 0x06

} tO_VA_UI_SmartcardState;

typedef struct
{
    tO_VA_UI_Ua               pUa;
    tO_VA_UI_SmartcardState   eSmartcardState;

} tO_VA_UI_SmartcardInfo;


typedef struct
{
    tO_VA_UI_SmartcardInfo    stSmartcardInfo;

} tO_VA_UI_GetSmartcardInfoRequest;


/*---------------------------------------------*/
/*      eO_VA_UI_GET_PAIRING_LEVEL_REQUEST     */
/*---------------------------------------------*/
typedef enum
{
    eO_VA_UI_LEVEL_1,
    eO_VA_UI_LEVEL_2

} tO_VA_UI_PairingLevel;

typedef struct
{
    tO_VA_UI_PairingLevel    ePairingLevel;

} tO_VA_UI_GetPairingLevelRequest;


/*---------------------------------------------*/
/*      eO_VA_UI_GET_PAIRING_MODE_REQUEST      */
/*---------------------------------------------*/
typedef enum
{
    eO_VA_UI_NO_MODE,
    eO_VA_UI_A_MODE,
    eO_VA_UI_B_MODE,
    eO_VA_UI_C_MODE

} tO_VA_UI_PairingMode;

typedef struct
{
    tO_VA_UI_PairingMode    ePairingMode;

} tO_VA_UI_GetPairingModeRequest;


/*---------------------------------------------*/
/*      eO_VA_UI_GET_PAIRING_SMARTCARD_REQUEST */
/*---------------------------------------------*/

typedef struct
{
    UINT32      uiMaxNumberOfSmartcard;
    UINT32      uiNumberOfSmartcard;

} tO_VA_UI_GetPairingSmartcardRequest;

/*---------------------------------------------*/
/*      eO_VA_UI_GET_PAIRING_UA_LIST_REQUEST   */
/*---------------------------------------------*/

typedef struct
{
    UINT32      uiUaNumber;
    tO_VA_UI_Ua   pUaArray[kO_VA_UI_MAX_PAIRING_SMARTCARD_NUMBER];

} tO_VA_UI_GetPairingUaListRequest;


/*----------------------------------------------*/
/*   eO_VA_UI_GET_PAIRING_STB_ID_LIST_REQUEST   */
/*----------------------------------------------*/

typedef struct
{
    tO_VA_UI_Ua       pUa;
    UINT32          uiStbIdNumber;
    tO_VA_UI_StbId    pStbIdArray[kO_VA_UI_MAX_PAIRING_STB_ID_NUMBER];

} tO_VA_UI_GetPairingStbIdListRequest;

/*---------------------------------------------*/
/*      eO_VA_UI_GET_STB_ID_REQUEST            */
/*---------------------------------------------*/
typedef struct
{
    tO_VA_UI_StbId    pStbId;
    BOOL            bIsDateValid;
    tO_VA_UI_DateEx   stDate;

} tO_VA_UI_GetStbIdRequest;





typedef union _uVA_UI_Request
{
    /*-----------------------*/
    /* any request structure */
    /*-----------------------*/
    tO_VA_UI_GetSoidListRequest               stGetSoidListRequest;
    tO_VA_UI_GetThemeLevelListRequest         stGetThemeLevelListRequest;
    tO_VA_UI_GetClassListRequest              stGetClassListRequest;
    tO_VA_UI_GetPreBookedListRequest          stGetPreBookedListRequest;
    tO_VA_UI_GetFacDataRequest                stGetFacDataRequest;
    tO_VA_UI_GetLabelRequest                  stGetLabelRequest;
    tO_VA_UI_GetUaRequest                     stGetUaRequest;
    tO_VA_UI_GetPreviewInfoRequest            stGetPreviewInfoRequest;
    tO_VA_UI_GetMaturityRatingRequest         stGetMaturityRatingRequest;
    tO_VA_UI_GetLockRequest                   stGetLockRequest;
    tO_VA_UI_GetPreselectionRequest           stGetPreselectionRequest;
    tO_VA_UI_GetAccessConditionRequest        stGetAccessConditionRequest;
    tO_VA_UI_GetPinCodeAttemptsRequest        stGetPinCodeAttemptsRequest;

    tO_VA_UI_ModifyPinCodeRequest             stModifyPinCodeRequest;
    tO_VA_UI_ModifyMaturityRatingRequest      stModifyMaturityRatingRequest ;
    tO_VA_UI_ModifyLockRequest                stModifyLockRequest;
    tO_VA_UI_ModifyPreselectionRequest        stModifyPreselectionRequest;

    tO_VA_UI_GetCamlockInfoRequest            stGetCamlockInfoRequest;
    tO_VA_UI_GetOperatorMessageRequest        stGetOperatorMessageRequest;

    /*-----------------------------*/
    /*    ppv request structure    */
    /*-----------------------------*/
    tO_VA_UI_GetIppvPListRequest              stGetIppvPListRequest;
    tO_VA_UI_GetIppvTListRequest              stGetIppvTListRequest;
    tO_VA_UI_GetCreditInfoRequest             stGetCreditInfoRequest;
    tO_VA_UI_GetIppvParameterRequest          stGetIppvParameterRequest;
    tO_VA_UI_GetCeilingRequest                stGetCeilingRequest;
    tO_VA_UI_GetThresholdRequest              stGetThresholdRequest;
    tO_VA_UI_AcceptIppvRequest                stAcceptIppvRequest;
    tO_VA_UI_CancelIppvRequest                stCancelIppvRequest;
    tO_VA_UI_ModifyCeilingRequest             stModifyCeilingRequest;
    tO_VA_UI_ModifyThresholdRequest           stModifyThresholdRequest;
    tO_VA_UI_StartIppvSurveyRequest           stStartIppvSurveyRequest;
    
    /*---------------------------------*/
    /* version request structure       */
    /*---------------------------------*/
    tO_VA_UI_GetVersionRequest                stGetVersionRequest;
    tO_VA_UI_GetVersionNameRequest            stGetVersionNameRequest;
    tO_VA_UI_GetSmartcardInfoRequest          stGetSmartcardInfoRequest;
    tO_VA_UI_GetPairingLevelRequest           stGetPairingLevelRequest;
    tO_VA_UI_GetPairingModeRequest            stGetPairingModeRequest;
    tO_VA_UI_GetPairingSmartcardRequest       stGetPairingSmartcardRequest;
    tO_VA_UI_GetPairingUaListRequest          stGetPairingUaListRequest;
    tO_VA_UI_GetPairingStbIdListRequest       stGetPairingStbIdListRequest;
    tO_VA_UI_GetStbIdRequest                  stGetStbIdRequest;

} uVA_UI_Request;

/*------------------------------------------------------*/
/*                    REQUEST API                       */
/*------------------------------------------------------*/

typedef struct
{
    tO_VA_UI_Message        eType;
    tO_VA_UI_RequestStatus  eStatus;
    DWORD                   dwScSlot;
    void                    *pUserArg;
} tO_VA_UI_RequestHeader;

typedef struct _stO_VA_UI_Request
{
    tO_VA_UI_RequestHeader   stRequestHeader;
    uVA_UI_Request           uRequest;
} tO_VA_UI_Request;

/*------------------------------------------------------*/
/*                     OPENTV                           */
/*------------------------------------------------------*/
/*Open tv structures*/

/*the request structure*/
typedef struct {
    void*   ptr;
    UINT32   dwProgression;
} tO_VA_UI_RequestMessage;

/*the event structure*/
typedef struct {
    UINT8   bScSlot;
    UINT8   bAcsId;
    UINT8   bScrambledType;
    UINT8   bCamlockStatus;
    DWORD   dwStbStreamHandle;
} tO_VA_UI_EventMessage;

typedef struct {
    UINT16  uiClass;
    UINT16  uiMessage;
    union {
        tO_VA_UI_RequestMessage request;
        tO_VA_UI_EventMessage   event;
    } info;
} tO_VA_UI_OpenTvMessage;


/*******************************************************************/
/*        INITIALIZATION                                           */


#define kO_VA_INIT_MACROVISION_IS_AVAILABLE        0x01
#define kO_VA_INIT_FINGERPRINTING_IS_AVAILABLE     0x02
#define kO_VA_INIT_DIGITAL_OUT_IS_AVAILABLE        0x04

typedef struct
{
    UINT8 uiSerialNumberSize;
    BYTE  *pSerialNumber;
} tO_VA_INIT_StbSerialNb;

typedef struct
{
    tO_VA_INIT_StbSerialNb  stBoxIdentity ;
    BYTE                    byConstructorIdentifier;
    BYTE                    byModelIdentifier;
    BYTE                    byHardwareType;
    UINT8                   uiHardwareVersion;
    BYTE                    bySoftwareType;
    UINT8                   uiSoftwareVersion;
} tO_VA_INIT_StbInformation;

typedef struct
{
    UINT16 uiMaxNumberOfFilters ;
    UINT16 uiMaxSizeOfFilter;
} tO_VA_INIT_Demux;

typedef struct
{
    UINT16 uiMaxNumberOfChannels;
} tO_VA_INIT_Descrambler;

typedef struct
{
    tO_VA_INIT_Demux        stDemux;
    tO_VA_INIT_Descrambler  stDescrambler;
} tO_VA_INIT_CasResources ;

typedef struct
{
    UINT32                      uiNbCas ;
    UINT32                      uiNbSmartCard ;
    tO_VA_INIT_CasResources     aCas[kO_VA_SETUP_NBMAX_ACS];
    tO_VA_INIT_StbInformation   stStbInformation ;
} tO_VA_INIT_ConfigurationParameters ;


/**************/
/*   Macros   */
/**************/
#define O_UI_GetTheMessage(_msg)        (((tO_VA_UI_OpenTvMessage*)(_msg))->uiMessage)

#define O_UI_GetTheRequestPtr(_msg)     (((tO_VA_UI_OpenTvMessage*)(_msg))->info.request.ptr)
#define O_UI_GetTheProgression(_msg)    (((tO_VA_UI_OpenTvMessage*)(_msg))->info.request.dwProgression)

#define O_UI_GetTheSlotId(_msg)         (((tO_VA_UI_OpenTvMessage*)(_msg))->info.event.bScSlot)
#define O_UI_GetTheAcsId(_msg)          (((tO_VA_UI_OpenTvMessage*)(_msg))->info.event.bAcsId)
#define O_UI_GetTheScrambledType(_msg)  (((tO_VA_UI_OpenTvMessage*)(_msg))->info.event.bScrambledType)
#define O_UI_GetTheStreamHandle(_msg)   (((tO_VA_UI_OpenTvMessage*)(_msg))->info.event.dwStbStreamHandle)
#define O_UI_GetTheQevStatus(_msg)      (((tO_VA_UI_OpenTvMessage*)(_msg))->info.event.bCamlockStatus)


#endif /* _VA_OTV_UI_TYPE2X_H_ */
