/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  oapi_resource.h
	@brief

	Description:  									\n
	Module: OAPI / API					\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/
/*
 * (c) 2011-2013 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

#ifndef __OAPI_RESOURCE_H__
#define __OAPI_RESOURCE_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <rlib.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define OxRSC_RSCTYPE_MAX			16

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eOxRSC_EVENT_NONE				= 0,

	eOxRSC_EVENT_ACTIVATED,
	eOxRSC_EVENT_RSC_USAGE_TYPE_SET,
	eOxRSC_EVENT_RFINPUT_CAPACITY_SET,
	eOxRSC_EVENT_TUNER_CAPACITY_SET,
	eOxRSC_EVENT_RSC_TAKEN,					// OxRsc_RscTaken_t
	eOxRSC_EVENT_RSC_RELEASED,				// OxRsc_RscReleased_t
	eOxRSC_EVENT_PRIORITY_CHANGED,			// OxRsc_PriorityChanged_t
	eOxRSC_EVENT_MAIN_ACTID_CHANGED,		// OxRsc_MainActIdChanged_t

	eOxRSC_EVENT_ENDOFCASE
} OxRsc_Event_e;

typedef struct
{
	HUINT32			 ulTotalActionNum;
	HUINT32			 ulRscTypeNum;

	HUINT32			 ulMainViewActId;
	HUINT32			 ulViewActIdFirst, ulViewActIdLast;
	HUINT32			 ulRecActIdFirst, ulRecActIdLast;
	HUINT32			 ulBgActIdFirst, ulBgActIdLast;

	HUINT32			 ulViewActPriority;
	HUINT32			 ulRecActPriority;
	HUINT32			 ulLsActPriority;

	HUINT32			 aulRscNum[OxRSC_RSCTYPE_MAX];
} OxRsc_InitialInfo_t;

typedef struct
{
	RxRscType_e				 eRscType;

	HBOOL					 bAllocated;
	HUINT32					 ulRscId;
} OxRsc_RscStatus_t;

typedef struct
{
	HUINT32					 ulActionId;

	HBOOL					 bActionTaken;
	HUINT32					 ulActionPriority;
	OxRsc_RscStatus_t		 astRscStatus[eRxRSCTYPE_MAX];
} OxRsc_Action_t;

// resource.onRscUsageTypeSet (RxRscType_e eRscType, int nRscNum, RxUsageType_e *astRscUsageArray)
typedef struct
{
	HUINT32						 ulRscType;					// RxRscType_e
	HUINT32						 ulRscNum;					// HUINT32
	void						*pvRscUsageArray;			// RxUsageType_e
} OxRsc_RscUsageTypeSet_t;

// resource.onRfInputSet (int nRfInputId, RxRfInfoCapacity_t *pstCapacity)
typedef struct
{
	HUINT32						 ulRfInputId;				// HUINT32
	void						*pvCapacity;				// RxRfInfoCapacity_t
} OxRsc_RfInputSet_t;

// resource.onTunerSet (int nTunerId, DxDeliveryType_e eSupportedDeliType)
typedef struct
{
	HUINT32						 ulTunerId;					// HUINT32
	HUINT32						 ulSupportedDeliType;		// RxRfInfoCapacity_t
} OxRsc_TunerSet_t;

// resource.onActionTaken (int nActionId, int nPriority, int nNumRscAllocated, RxRscType_e *peRscType, RxRscObject_t *astRscObjArray)
typedef struct
{
	HUINT32						 ulActionId;
	HUINT32						 ulPriority;
	HUINT32						 ulNumRscAllocated;

	HUINT32						*pulRscType;				// RxRscType_e
	void						*pvRscObj;					// RxRscObject_t
} OxRsc_RscTaken_t;

// resource.onActionReleased (int nActionId)
typedef struct
{
	HUINT32						 ulActionId;
} OxRsc_RscReleased_t;

// resource.onActionPriorityChanged (int nActionId, int nPriority)
typedef struct
{
	HUINT32						 ulActionId;
	HUINT32						 ulPriority;
} OxRsc_PriorityChanged_t;

// resource.onMainActionIdChanged (int nMainActionId)
typedef struct
{
	HUINT32						 ulActionId;
} OxRsc_MainActIdChanged_t;

typedef union
{
	OxRsc_InitialInfo_t			 stActivated;				// eOxRSC_EVENT_ACTIVATED
	OxRsc_RscUsageTypeSet_t		 stRscUsageSet;				// eOxRSC_EVENT_RSC_USAGE_TYPE_SET
	OxRsc_RfInputSet_t			 stRfInputSet;				// eOxRSC_EVENT_RFINPUT_CAPACITY_SET
	OxRsc_TunerSet_t			 stTunerSet;				// eOxRSC_EVENT_TUNER_CAPACITY_SET
	OxRsc_RscTaken_t			 stRscTaken;				// eOxRSC_EVENT_RESOURCE_TAKEN
	OxRsc_RscReleased_t			 stRscReleased;				// eOxRSC_EVENT_RESOURCE_RELEASED
	OxRsc_PriorityChanged_t		 stPriorityChanged;			// eOxRSC_EVENT_PRIORITY_CHANGED
	OxRsc_MainActIdChanged_t	 stMainActIdChanged;		// eOxRSC_EVENT_MAIN_ACTID_CHANGED
} OxRsc_NotifierData_t;


typedef void (*OxRsc_Notifier_t) (
											OxRsc_Event_e eEvent,
											OxRsc_NotifierData_t *pstNotifyData,
											void *pvUserData
										);


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
HERROR				OAPI_RSC_Init (void);
HERROR				OAPI_RSC_SetListener (OxRsc_Notifier_t pfNotifier, void *pvUserData);
HERROR				OAPI_RSC_ResetListener (OxRsc_Notifier_t pfNotifier, void *pvUserData);
HERROR				OAPI_RSC_GetInitialInfo (OxRsc_InitialInfo_t *pstInitInfo);
HERROR				OAPI_RSC_GetResourceId (HUINT32 ulActionId, RxRscType_e eRscType, HUINT32 *pulRscId);
RxRscReqHandle_t	OAPI_RSC_MakeResourceRequestHandle (HUINT32 ulActionId, HUINT32 ulActionPriority, HBOOL bUseVirtualHandle);
RxRscReqHandle_t	OAPI_RSC_DuplicateRequestHandle (RxRscReqHandle_t hRscReq);
HERROR				OAPI_RSC_AddRemovableActionIdToRequestHandle (Handle_t hRscReq, HUINT32 ulActionId, HUINT32 ulActionPriority);
HERROR				OAPI_RSC_AddIgnorableActionIdToRequestHandle (Handle_t hRscReq, HUINT32 ulActionId);
HERROR				OAPI_RSC_AddRequiredResourceToRequestHandle (Handle_t hRscReq, RxRscType_e eRscType, HUINT32 ulResourceId, HBOOL bSharable, RxUsageType_e eUsageType);
HERROR				OAPI_RSC_AddArgumentToRequestHandle (Handle_t hRscReq, RxTrialArg_t *punArg);
HERROR				OAPI_RSC_TryResourceRequest (Handle_t hRscReq);
RxRscTrialResult_t*	OAPI_RSC_GetRequestTrialResult (Handle_t hRscReq);
HERROR				OAPI_RSC_DestroyResourceRequestHandle (RxRscReqHandle_t hRscReq);
HERROR 				OAPI_RSC_SetResourcesByTrialResults (RxRscReqHandle_t hRscReq);
HERROR 				OAPI_RSC_GetEmptyRecordAndBgActionIdArray (HUINT32 *pulActionIdArray, HUINT32 ulNum);
HERROR 				OAPI_RSC_GetEmptyRecordActionIdArray (HUINT32 *pulActionIdArray, HUINT32 ulNum);
HERROR				OAPI_RSC_GetEmptyRecordActionId (HUINT32 *pulActionId);
HERROR				OAPI_RSC_GetEmptyBgActionId (HUINT32 *pulActionId);
HERROR				OAPI_RSC_GetActionPriority (HUINT32 ulActionId, HUINT32 *pulPriority);
HERROR 				OAPI_RSC_ResetVirtualHandleInfo (void);
HERROR              OAPI_RSC_GetResourceNumber ( RxRscType_e eRscType, HUINT32 *pulRscNum);

#endif // __OAPI_RESOURCE_H__
