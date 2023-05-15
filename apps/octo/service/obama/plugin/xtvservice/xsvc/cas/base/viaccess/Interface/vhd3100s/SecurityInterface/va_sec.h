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
    Viaccess is a registered trademark of Viaccess ® in France and/or other 
    countries. All other product and company names mentioned herein are the 
    trademarks of their respective owners.
    Viaccess may hold patents, patent applications, trademarks, copyrights 
    or other intellectual property rights over the code hereafter. Unless 
    expressly specified otherwise in a Viaccess written license agreement, the
    delivery of this code does not imply the concession of any license over 
    these patents, trademarks, copyrights or other intellectual property.
 
******************************************************************************/

/*
    $Revision: 3902 $
    $Date: 2007-01-16 11:45:04 +0100 (mar., 16 janv. 2007) $
*/

#ifndef __VA_SEC_H_
#define __VA_SEC_H_

#include    "va_ui.h"

/******************************************************/
/*                  Constants                         */
/******************************************************/

/* Maximum number of sanctions that can be defined for pairing */
#define     kVA_SEC_MAX_PAIRING_SANCTIONS       3

/* Maximum number of sanctions that can be defined for the partitioning */
#define     kVA_SEC_MAX_PARTITIONING_SANCTIONS  4 /* previously 6*/

/* Number of detections for the partitioning service set by the system */
#define     kVA_SEC_DETECTION_NUMBER            6

#define kVA_SEC_SYSTEM_OK                  0x00
#define kVA_SEC_PAIRING_OK                 0x01
#define kVA_SEC_PARTITIONING_OK            0x02
#define kVA_SEC_PAIRING_DEFAULT            0x04
#define kVA_SEC_PARTITIONING_DEFAULT       0x08
#define kVA_SEC_PAIRING_NOT_SUPPORTED      0x10
#define kVA_SEC_PARTITIONING_NOT_SUPPORTED 0x20

/******************************************************/
/*                  Enumerations                      */
/******************************************************/

typedef enum
{
    eVA_SEC_SANCTION_DISABLED,
    eVA_SEC_SANCTION_ENABLED
} tVA_SEC_SanctionConfigState;


typedef enum
{
    eVA_SEC_SANCTION_NOT_APPLIED,
    eVA_SEC_SANCTION_APPLIED
} tVA_SEC_SanctionState;


typedef enum
{
    eVA_SEC_DETECTION_DISABLED,
    eVA_SEC_DETECTION_ENABLED
} tVA_SEC_DetectionConfigState;


typedef enum
{
    eVA_SEC_DETECTION_NOT_DETECTED,
    eVA_SEC_DETECTION_DETECTED
} tVA_SEC_DetectionState;


typedef enum
{
    eVA_SEC_DISABLE,
    eVA_SEC_ENABLE,
    eVA_SEC_DONT_CHANGE
} tVA_SEC_ConfigStateChange;


typedef enum
{
    eVA_SEC_RESET_DETECTION,
    eVA_SEC_DONT_CHANGE_DETECTION
} tVA_SEC_ResetDetectionState;


typedef enum
{
    eVA_SEC_SUSPEND_PAIRING_SANCTIONS,
    eVA_SEC_RESUME_PAIRING_SANCTIONS
}tVA_SEC_PairingSanctionsPolicy;


typedef enum
{
    eVA_SEC_GET_SECURITY_STATUS_REQUEST,
    eVA_SEC_GET_PAIRING_SANCTIONS_REQUEST,
    eVA_SEC_GET_FORBIDDEN_SOID_LIST_REQUEST,  
    eVA_SEC_GET_DETECTIONS_CONFIG_REQUEST,
    eVA_SEC_SET_PAIRING_MODE_REQUEST,
    eVA_SEC_SET_PAIRING_SANCTIONS_CONFIG_REQUEST,
    eVA_SEC_RESET_PAIRING_REQUEST,
    eVA_SEC_SET_PAIRING_SANCTIONS_POLICY_REQUEST,
    eVA_SEC_LOCK_PAIRING_REQUEST,   
    eVA_SEC_SET_SMARTCARD_NUMBER_REQUEST,
    eVA_SEC_SET_DETECTIONS_CONFIG_REQUEST,
    eVA_SEC_RESET_DETECTIONS_REQUEST,   
    eVA_SEC_SET_PARTITIONING_SANCTIONS_CONFIG_REQUEST,
    eVA_SEC_LOCK_PARTITIONING_REQUEST,
    eVA_SEC_SET_LIMITED_MODE_REQUEST
} tVA_SEC_RequestType;


typedef enum
{
    eVA_SEC_REQUEST_OK_STATUS,
    eVA_SEC_REQUEST_WRONG_MODE_STATUS,
    eVA_SEC_REQUEST_MODE_NOT_SETUP_STATUS,
    eVA_SEC_REQUEST_SAME_MODE_STATUS,
    eVA_SEC_REQUEST_WRONG_CONFIG_STATUS,
    eVA_SEC_REQUEST_TOO_MANY_SMARTCARD_STATUS,
    eVA_SEC_REQUEST_ARRAY_TOO_SMALL_STATUS,
    eVA_SEC_REQUEST_LOCKED_STATUS,
    eVA_SEC_REQUEST_ERROR_STATUS
} tVA_SEC_RequestStatus;



/******************************************************/
/*                  Structures                        */
/******************************************************/

typedef struct
{
    UINT32      uiKeySize;
    BYTE*       pKey;
} tVA_SEC_Key;


typedef struct
{
    tVA_SEC_DetectionConfigState    eDetectionConfig;
    tVA_SEC_DetectionState          eDetectionState;
    tVA_SEC_SanctionConfigState     eSanctionConfig 
                                        [kVA_SEC_MAX_PARTITIONING_SANCTIONS];
    tVA_SEC_SanctionState           eSanctionState
                                        [kVA_SEC_MAX_PARTITIONING_SANCTIONS];

} tVA_SEC_Detection; 


typedef struct
{
    UINT32      uiSoidService;
    UINT32      uiSoidIssuer;
    BYTE        byClass;

} tVA_SEC_PartitioningSoid;



typedef struct
{
    DWORD       dwSecurityStatus;

} tVA_SEC_SecurityStatusRequest;


typedef struct
{
    UINT32                      uiSanctionsNumber;
    tVA_SEC_SanctionConfigState eSanctionsConfig
                                            [kVA_SEC_MAX_PAIRING_SANCTIONS];
    tVA_SEC_SanctionState       eSanctionsState
                                            [kVA_SEC_MAX_PAIRING_SANCTIONS];

} tVA_SEC_PairingSanctionsRequest;


typedef struct
{
    UINT32                            uiMaxItemNumber;
    UINT32                            uiItemNumber;
    tVA_SEC_PartitioningSoid*         pItemArray;

} tVA_SEC_ForbiddenSoidListRequest;


typedef struct
{
    UINT32                      uiPartitioningSanctions;
    tVA_SEC_Detection           stDetection [kVA_SEC_DETECTION_NUMBER];

} tVA_SEC_DetectionsConfigRequest;



typedef struct
{
    tVA_UI_PairingMode         ePairingMode;

} tVA_SEC_SetPairingModeRequest;


typedef struct
{
    tVA_SEC_ConfigStateChange   eChangeSanctionsConfig  
                                            [kVA_SEC_MAX_PAIRING_SANCTIONS];

} tVA_SEC_SetPairingSanctionsConfigRequest;


typedef struct
{
    tVA_SEC_PairingSanctionsPolicy  eSanctionsPolicy;

} tVA_SEC_SetPairingSanctionsPolicyRequest;


typedef struct
{
    tVA_SEC_ConfigStateChange   eLockSanctionsConfig[kVA_SEC_MAX_PAIRING_SANCTIONS];

} tVA_SEC_LockPairingRequest;


typedef struct
{
    UINT32                      uiSmartcardNumber;

} tVA_SEC_SetSmartcardNumberRequest;


typedef struct
{
    tVA_SEC_ConfigStateChange       eChangeDetectionConfig 
                                                [kVA_SEC_DETECTION_NUMBER];

} tVA_SEC_SetDetectionsConfigRequest;


typedef struct
{
    tVA_SEC_ResetDetectionState     eDetectionState [kVA_SEC_DETECTION_NUMBER];

} tVA_SEC_ResetDetectionsRequest;
 

typedef struct
{
    tVA_SEC_ConfigStateChange       eChangeSanctionsConfig 
                                          [kVA_SEC_DETECTION_NUMBER]
                                          [kVA_SEC_MAX_PARTITIONING_SANCTIONS];

} tVA_SEC_SetPartitioningSanctionsConfigRequest;


typedef struct
{
    tVA_SEC_ConfigStateChange       eLockSanctionsConfig
                                          [kVA_SEC_MAX_PARTITIONING_SANCTIONS];

} tVA_SEC_LockPartitioningRequest;



/******************************************************/
/*                  Unions                            */
/******************************************************/

typedef union
{
    tVA_SEC_SecurityStatusRequest                    stSecurityStatus;
    tVA_SEC_PairingSanctionsRequest                  stPairingSanctions;
    tVA_SEC_ForbiddenSoidListRequest                 stForbiddenSoidList;
    tVA_SEC_DetectionsConfigRequest                  stDetectionsConfig;
    tVA_SEC_SetPairingModeRequest                    stSetPairingMode;
    tVA_SEC_SetPairingSanctionsConfigRequest         stSetPairingSanctions;
    tVA_SEC_SetPairingSanctionsPolicyRequest         stSetPairingPolicy;
    tVA_SEC_LockPairingRequest                       stLockPairing;
    tVA_SEC_SetSmartcardNumberRequest                stSetSmartcardNumber; 
    tVA_SEC_SetDetectionsConfigRequest               stSetDetections;
    tVA_SEC_ResetDetectionsRequest                   stResetDetectionsState;
    tVA_SEC_SetPartitioningSanctionsConfigRequest    stSetPartitioningSanctionsConfig;
    tVA_SEC_LockPartitioningRequest                  stLockPartitioning;

} tVA_SEC_RequestArg;


/******************************************************/
/*               Request   Structures  / Functions    */
/******************************************************/
struct _stVA_SEC_Request;

/* calback functions */
typedef void (*tVA_SEC_RequestCallback)   (struct _stVA_SEC_Request * pRequest);

typedef struct
{
    tVA_SEC_Key             stRequestKey;
    tVA_SEC_RequestType     eRequestType;
    tVA_SEC_RequestStatus   eRequestStatus;
    tVA_SEC_RequestCallback pCallback;
    void                    *pUserArg;

} tVA_SEC_RequestHeader;

typedef struct _stVA_SEC_Request
{
    tVA_SEC_RequestHeader     stRequestHeader;
    tVA_SEC_RequestArg        uRequest;

} tVA_SEC_Request;


/* request function */
INT VA_SEC_Request (tVA_SEC_Request* pRequest);



#endif 
