/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  oapi_resource.c
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

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <oapi.h>
#include <rlib.h>
#include <oapi_resource.h>
#include <obama_int.h>

#include "../../oapi_mem.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define ENTER_RSC_CRITICAL			HxSEMT_Get(s_ulOapiRsc_SemId)
#define LEAVE_RSC_CRITICAL			HxSEMT_Release(s_ulOapiRsc_SemId)

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	Handle_t				 hSession;
	Handle_t				 hSessionVirtual;
	OxRsc_InitialInfo_t		 stInitInfo;

	HUINT32					 ulActNum;
	OxRsc_Action_t			*pstActItemArray;
} oapiRscContext_t;

typedef struct
{
	OxRsc_Notifier_t		 pfListener;
	void					*pvUserData;
} oapiRscNotifier_t;


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HBOOL				 s_bOapiRsc_Inited = FALSE, s_bOapiRsc_Activated = FALSE;
STATIC HUINT32				 s_ulOapiRsc_SemId = 0;
STATIC oapiRscContext_t		 s_stOapiRsc_Context;
STATIC HxList_t				*s_pstOapiRsc_NotifierList = NULL;			// oapiRscNotifier_t

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
#define ___LOCAL_FUNCTIONS___
STATIC oapiRscContext_t *oapi_rsc_GetContext (void)
{
	return &s_stOapiRsc_Context;
}

STATIC void oapi_rsc_NotifyToListener (OxRsc_Event_e eEvent, OxRsc_NotifierData_t *pstData)
{
	HxList_t			*pstListItem;

	for(pstListItem = s_pstOapiRsc_NotifierList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		oapiRscNotifier_t			*pstNotifier = (oapiRscNotifier_t *)HLIB_LIST_Data(pstListItem);

		if ((NULL != pstNotifier) && (NULL != pstNotifier->pfListener))
		{
			(pstNotifier->pfListener) (eEvent, pstData, pstNotifier->pvUserData);
		}
	}
}

STATIC oapiRscNotifier_t *oapi_rsc_FindNotifier (OxRsc_Notifier_t pfListener, void *pvUserData)
{
	HxList_t			*pstListItem;

	for(pstListItem = s_pstOapiRsc_NotifierList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		oapiRscNotifier_t			*pstNotifier = (oapiRscNotifier_t *)HLIB_LIST_Data(pstListItem);

		if ((pfListener == pstNotifier->pfListener) &&
			(pvUserData == pstNotifier->pvUserData))
		{
			return pstNotifier;
		}
	}

	return NULL;
}

STATIC OxRsc_Action_t *oapi_rsc_GetAction (HUINT32 ulActionId)
{
	oapiRscContext_t		*pstContext = oapi_rsc_GetContext();

	if ((ulActionId < pstContext->ulActNum) && (NULL != pstContext->pstActItemArray))
	{
		return &(pstContext->pstActItemArray[ulActionId]);
	}

	return NULL;
}

STATIC HERROR oapi_rsc_SetAction (HUINT32 ulActionId, HUINT32 ulPriority)
{
	RxRscType_e				 eRscType;
	OxRsc_Action_t			*pstAction = oapi_rsc_GetAction(ulActionId);

	if (NULL == pstAction)
	{
		HxLOG_Error("oapi_rsc_GetAction NULL: ActionId(%d)\n", ulActionId);
		return ERR_FAIL;
	}

	HxSTD_MemSet(pstAction, 0, sizeof(OxRsc_Action_t));
	pstAction->ulActionId = ulActionId;
	pstAction->bActionTaken = TRUE;
	pstAction->ulActionPriority = ulPriority;

	for (eRscType = 0; eRscType < eRxRSCTYPE_MAX; eRscType++)
	{
		pstAction->astRscStatus[eRscType].eRscType = eRscType;
	}

	return ERR_OK;
}

STATIC HERROR oapi_rsc_ClearAction (HUINT32 ulActionId)
{
	RxRscType_e				 eRscType;
	OxRsc_Action_t			*pstAction = oapi_rsc_GetAction(ulActionId);

	if (NULL == pstAction)
	{
		HxLOG_Error("oapi_rsc_GetAction NULL: ActionId(%d)\n", ulActionId);
		return ERR_FAIL;
	}

	HxSTD_MemSet(pstAction, 0, sizeof(OxRsc_Action_t));
	pstAction->ulActionId = ulActionId;

	for (eRscType = 0; eRscType < eRxRSCTYPE_MAX; eRscType++)
	{
		pstAction->astRscStatus[eRscType].eRscType = eRscType;
	}

	return ERR_OK;
}

STATIC HERROR oapi_rsc_SetResource (HUINT32 ulActionId, RxRscType_e eRscType, HUINT32 ulRscId)
{
	OxRsc_Action_t			*pstAction = oapi_rsc_GetAction(ulActionId);

	if (NULL == pstAction)
	{
		HxLOG_Error("oapi_rsc_GetAction NULL: ActionId(%d)\n", ulActionId);
		return ERR_FAIL;
	}

	if (TRUE != pstAction->bActionTaken)
	{
		HxLOG_Error("Action(%d) shall be Set before...\n", ulActionId);
		return ERR_FAIL;
	}

	if (eRscType >= eRxRSCTYPE_MAX)
	{
		HxLOG_Error("eRscType(%d) is larger than eRxRSCTYPE_MAX\n", eRscType);
		return ERR_FAIL;
	}

	pstAction->astRscStatus[eRscType].bAllocated	= TRUE;
	pstAction->astRscStatus[eRscType].ulRscId		= ulRscId;

	return ERR_OK;
}

#define _____NOTIFICATION_CALLBACKS_____
// resource.onActionTaken (int nActionId, int nPriority, int nNumRscAllocated, RxRscType_e *peRscType, RxRscObject_t *astRscObjArray)
STATIC void oapi_rsc_CbOnActionTaken (HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{
	ENTER_RSC_CRITICAL;

	if (argc == 5)
	{
		HUINT32					 ulActionId = (HUINT32)HxOBJECT_INT(argv[0]);
		HUINT32					 ulPriority = (HUINT32)HxOBJECT_INT(argv[1]);
		HUINT32					 ulRscNum = (HUINT32)HxOBJECT_INT(argv[2]);
		HUINT32					*pulRscTypeArray = (HUINT32 *)HxOBJECT_BIN_DATA(argv[3]);
		void					*pvRscObjArray = (void *)HxOBJECT_BIN_DATA(argv[4]);
		OxRsc_NotifierData_t	 stNotiData;
		oapiRscContext_t		*pstContext = oapi_rsc_GetContext();

		HxLOG_Info("ActionID(%d)\n", ulActionId);

		if (ulActionId < pstContext->ulActNum)
		{
			HUINT32					 ulCnt;
			RxRscObject_t			*pstRscObjArray = (RxRscObject_t *)pvRscObjArray;
			oapiRscContext_t		*pstContext = oapi_rsc_GetContext();
			HERROR					 hErr;

			(void)oapi_rsc_ClearAction(ulActionId);
			(void)oapi_rsc_SetAction(ulActionId, ulPriority);
			for (ulCnt = 0; ulCnt < ulRscNum; ulCnt++)
			{
				(void)oapi_rsc_SetResource(ulActionId, (RxRscType_e)pulRscTypeArray[ulCnt], pstRscObjArray[ulCnt].ulRscId);
			}

			stNotiData.stRscTaken.ulActionId		= ulActionId;
			stNotiData.stRscTaken.ulPriority		= ulPriority;
			stNotiData.stRscTaken.ulNumRscAllocated = ulRscNum;
			stNotiData.stRscTaken.pulRscType		= pulRscTypeArray;
			stNotiData.stRscTaken.pvRscObj			= pvRscObjArray;

			// Set the RLIB setting
			hErr = RLIB_ClearActionResource(pstContext->hSession, ulActionId);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("RLIB_ClearActionResource err: ActionId(%d)\n", ulActionId);
			}

			for (ulCnt = 0; ulCnt < ulRscNum; ulCnt++)
			{
				hErr = RLIB_SetResourceObject(pstContext->hSession, ulActionId, (RxRscType_e)pulRscTypeArray[ulCnt], &(pstRscObjArray[ulCnt]));
				if (ERR_OK != hErr)
				{
					HxLOG_Error ("RLIB_SetResourceObject err: ActionId(%d), eRscType(%d)\n", ulActionId, (RxRscType_e)pulRscTypeArray[ulCnt]);
				}
			}

			oapi_rsc_NotifyToListener(eOxRSC_EVENT_RSC_TAKEN, &stNotiData);
		}
	}
	else
	{
		HxLOG_Error("resource.onActionTaken has wrong argument number:(%d)\n", argc);
	}

	LEAVE_RSC_CRITICAL;
}

// resource.onActionReleased (int nActionId)
STATIC void oapi_rsc_CbOnActionReleased (HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{
	ENTER_RSC_CRITICAL;

	if (argc == 1)
	{
		HUINT32					 ulActionId = (HUINT32)HxOBJECT_INT(argv[0]);
		OxRsc_NotifierData_t	 stNotiData;
		oapiRscContext_t		*pstContext = oapi_rsc_GetContext();

		HxLOG_Info("ActionID(%d)\n", ulActionId);

		if (ulActionId < pstContext->ulActNum)
		{
			HERROR					 hErr;

			(void)oapi_rsc_ClearAction(ulActionId);

			stNotiData.stRscReleased.ulActionId		= ulActionId;

			hErr = RLIB_ClearActionResource (pstContext->hSession, ulActionId);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("RLIB_ClearActionResource err: ActionId(%d)\n", ulActionId);
			}

			oapi_rsc_NotifyToListener(eOxRSC_EVENT_RSC_RELEASED, &stNotiData);
		}
	}
	else
	{
		HxLOG_Error("resource.onActionReleased has wrong argument number:(%d)\n", argc);
	}

	LEAVE_RSC_CRITICAL;
}

// resource.onActionPriorityChanged (int nActionId, int nPriority)
STATIC void oapi_rsc_CbOnActionPriorityChanged (HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{
	ENTER_RSC_CRITICAL;

	if (argc == 2)
	{
		HUINT32					 ulActionId = (HUINT32)HxOBJECT_INT(argv[0]);
		HUINT32					 ulPriority = (HUINT32)HxOBJECT_INT(argv[1]);
		OxRsc_NotifierData_t	 stNotiData;
		oapiRscContext_t		*pstContext = oapi_rsc_GetContext();

		HxLOG_Info("ActionID(%d)\n", ulActionId);

		if (ulActionId < pstContext->ulActNum)
		{
			OxRsc_Action_t			*pstAction = oapi_rsc_GetAction(ulActionId);

			if (pstAction != NULL && pstAction->ulActionPriority != ulPriority)
			{
				pstAction->ulActionPriority = ulPriority;

				stNotiData.stPriorityChanged.ulActionId		= ulActionId;
				stNotiData.stPriorityChanged.ulPriority		= ulPriority;
				oapi_rsc_NotifyToListener(eOxRSC_EVENT_PRIORITY_CHANGED, &stNotiData);
			}
		}
	}
	else
	{
		HxLOG_Error("resource.onActionPriorityChanged has wrong argument number:(%d)\n", argc);
	}

	LEAVE_RSC_CRITICAL;
}

// resource.onMainActionIdChanged (int nMainActionId)
STATIC void oapi_rsc_CbOnMainActionIdChanged (HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{
	ENTER_RSC_CRITICAL;

	if (argc == 1)
	{
		HUINT32					 ulActionId = (HUINT32)HxOBJECT_INT(argv[0]);
		OxRsc_NotifierData_t	 stNotiData;
		oapiRscContext_t		*pstContext = oapi_rsc_GetContext();

		HxLOG_Info("ActionID(%d)\n", ulActionId);

		if (ulActionId < pstContext->ulActNum)
		{
			if (pstContext->stInitInfo.ulMainViewActId != ulActionId)
			{
				pstContext->stInitInfo.ulMainViewActId = ulActionId;

				stNotiData.stMainActIdChanged.ulActionId = ulActionId;
				oapi_rsc_NotifyToListener(eOxRSC_EVENT_MAIN_ACTID_CHANGED, &stNotiData);
			}
		}
	}
	else
	{
		HxLOG_Error("resource.onMainActionIdChanged has wrong argument number:(%d)\n", argc);
	}

	LEAVE_RSC_CRITICAL;
}

// resource.onActivated (OxRsc_InitialInfo_t *pstInitInfo)
STATIC void oapi_rsc_CbOnActivated (HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{
	RxRscType_e 			 eRscType;
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;
	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);

	if ((argc == 1) && (TRUE != s_bOapiRsc_Activated))
	{
		OxRsc_InitialInfo_t		*pstInitInfo = (OxRsc_InitialInfo_t *)HxOBJECT_BIN_DATA(argv[0]);
		oapiRscContext_t		*pstContext = oapi_rsc_GetContext();
		OxRsc_NotifierData_t	 stNotiData;

		HxSTD_MemCopy(&(pstContext->stInitInfo), pstInitInfo, sizeof(OxRsc_InitialInfo_t));
		pstContext->pstActItemArray = ( OxRsc_Action_t *)HLIB_STD_MemCalloc(sizeof( OxRsc_Action_t) * pstInitInfo->ulTotalActionNum);
		if (NULL != pstContext->pstActItemArray)
		{
			HUINT32			 ulActionId, ulRscType;

			pstContext->ulActNum = pstInitInfo->ulTotalActionNum;

			for (ulActionId = 0; ulActionId < pstContext->ulActNum; ulActionId++)
			{
				OxRsc_Action_t		*pstAction = &(pstContext->pstActItemArray[ulActionId]);

				pstAction->ulActionId = ulActionId;

				for (ulRscType = 0; ulRscType < eRxRSCTYPE_MAX; ulRscType++)
				{
					OxRsc_RscStatus_t	*pstRscStatus = &(pstAction->astRscStatus[ulRscType]);

					pstRscStatus->eRscType = (RxRscType_e)ulRscType;
				}
			}
		}

		// Activate the notifiers:
		HLIB_RPC_RegisterNotifyCallback (nRpcHandle, "resource.onActionTaken", "iiibb", NULL, (HxRPC_Noti_t)oapi_rsc_CbOnActionTaken);
		HLIB_RPC_RegisterNotifyCallback (nRpcHandle, "resource.onActionReleased", "i", NULL, (HxRPC_Noti_t)oapi_rsc_CbOnActionReleased);
		HLIB_RPC_RegisterNotifyCallback (nRpcHandle, "resource.onActionPriorityChanged", "ii", NULL, (HxRPC_Noti_t)oapi_rsc_CbOnActionPriorityChanged);
		HLIB_RPC_RegisterNotifyCallback (nRpcHandle, "resource.onMainActionIdChanged", "i", NULL, (HxRPC_Noti_t)oapi_rsc_CbOnMainActionIdChanged);

		s_bOapiRsc_Activated = TRUE;

		// Set the RLIB setting
		pstContext->hSession = RLIB_CreateSessionHandle("OAPI_RSC");
		(void)RLIB_SetActionNumber(pstContext->hSession, pstInitInfo->ulTotalActionNum);
		pstContext->hSessionVirtual = RLIB_CreateSessionHandle("OAPI_RSC_VIRTUAL");
		(void)RLIB_SetActionNumber(pstContext->hSessionVirtual, pstInitInfo->ulTotalActionNum);


		for (eRscType = 0; eRscType < eRxRSCTYPE_MAX; eRscType++)
		{
			hErr = RLIB_SetResourceNumber (pstContext->hSession, eRscType, pstInitInfo->aulRscNum[eRscType]);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("RLIB_SetResourceNumber err: eRscType(%d)\n", eRscType);
			}
		}

		HxSTD_MemCopy(&(stNotiData.stActivated), pstInitInfo, sizeof(OxRsc_InitialInfo_t));
		oapi_rsc_NotifyToListener(eOxRSC_EVENT_ACTIVATED, &stNotiData);
	}
	else
	{
		HxLOG_Error("resource.onActivated has wrong argument number:(%d)\n", argc);
	}
}

// resource.onRscUsageTypeSet (RxRscType_e eRscType, int nRscNum, RxUsageType_e *astRscUsageArray)
STATIC void oapi_rsc_CbOnRscUsageTypeSet (HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{
	ENTER_RSC_CRITICAL;

	if (argc == 3)
	{
		HUINT32 				 ulRscId;
		OxRsc_NotifierData_t	 stNotiData;
		oapiRscContext_t		*pstContext = oapi_rsc_GetContext();
		RxUsageType_e			*peRscUsageArray;
		HERROR					 hErr;

		stNotiData.stRscUsageSet.ulRscType			= (HUINT32)HxOBJECT_INT(argv[0]);
		stNotiData.stRscUsageSet.ulRscNum			= (HUINT32)HxOBJECT_INT(argv[1]);
		stNotiData.stRscUsageSet.pvRscUsageArray	= (void *)HxOBJECT_BIN_DATA(argv[2]);

		// Set the RLIB setting
		peRscUsageArray = (RxUsageType_e *)stNotiData.stRscUsageSet.pvRscUsageArray;

		for (ulRscId = 0; ulRscId < stNotiData.stRscUsageSet.ulRscNum; ulRscId++)
		{
			hErr = RLIB_SetResourceUsageType(pstContext->hSession, stNotiData.stRscUsageSet.ulRscType, ulRscId, peRscUsageArray[ulRscId]);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("RLIB_SetResourceUsageType err: eRscType(%d), ulRscId(%d), UsageType(%d)\n", stNotiData.stRscUsageSet.ulRscType, ulRscId, peRscUsageArray[ulRscId]);
			}
		}

		oapi_rsc_NotifyToListener(eOxRSC_EVENT_RSC_USAGE_TYPE_SET, &stNotiData);
	}
	else
	{
		HxLOG_Error("resource.onRscUsageTypeSet has wrong argument number:(%d)\n", argc);
	}

	LEAVE_RSC_CRITICAL;
}

// resource.onRfInputSet (int nRfInputId, RxRfInfoCapacity_t *pstCapacity)
STATIC void oapi_rsc_CbOnRfInputCapabilitySet (HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{
	ENTER_RSC_CRITICAL;

	if (argc == 2)
	{
		OxRsc_NotifierData_t	 stNotiData;
		oapiRscContext_t		*pstContext = oapi_rsc_GetContext();
		HERROR					 hErr;

		stNotiData.stRfInputSet.ulRfInputId	= (HUINT32)HxOBJECT_INT(argv[0]);
		stNotiData.stRfInputSet.pvCapacity	= (void *)HxOBJECT_BIN_DATA(argv[1]);

		// Set the RLIB setting
		hErr = RLIB_SetRfInfoCapacity(pstContext->hSession, stNotiData.stRfInputSet.ulRfInputId, (RxRfInfoCapacity_t *)stNotiData.stRfInputSet.pvCapacity);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("RLIB_SetRfInfoCapacity err: ulRscId(%d)\n", stNotiData.stRfInputSet.ulRfInputId);
		}

		oapi_rsc_NotifyToListener(eOxRSC_EVENT_RFINPUT_CAPACITY_SET, &stNotiData);
	}
	else
	{
		HxLOG_Error("resource.onRfInputSet has wrong argument number:(%d)\n", argc);
	}

	LEAVE_RSC_CRITICAL;
}

// resource.onTunerSet (int nTunerId, DxDeliveryType_e eSupportedDeliType)
STATIC void oapi_rsc_CbOnTunerCapabilitySet (HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{
	ENTER_RSC_CRITICAL;

	if (argc == 2)
	{
		OxRsc_NotifierData_t	 stNotiData;
		oapiRscContext_t		*pstContext = oapi_rsc_GetContext();
		HERROR					 hErr;

		stNotiData.stTunerSet.ulTunerId				= (HUINT32)HxOBJECT_INT(argv[0]);
		stNotiData.stTunerSet.ulSupportedDeliType	= (HUINT32)HxOBJECT_INT(argv[1]);

		// Set the RLIB setting
		hErr = RLIB_SetTunerCapacity(pstContext->hSession, stNotiData.stTunerSet.ulTunerId, (DxDeliveryType_e)stNotiData.stTunerSet.ulSupportedDeliType);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("RLIB_SetTunerCapacity err: ulRscId(%d)\n", stNotiData.stTunerSet.ulTunerId);
		}

		oapi_rsc_NotifyToListener(eOxRSC_EVENT_TUNER_CAPACITY_SET, &stNotiData);
	}
	else
	{
		HxLOG_Error("resource.onTunerSet has wrong argument number:(%d)\n", argc);
	}

	LEAVE_RSC_CRITICAL;
}

// resource.onRfInputAbilityChanged (int nRfInputId, HBOOL bAbility)
STATIC void oapi_rsc_CbOnRfInputAbilityChanged (HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{
	ENTER_RSC_CRITICAL;

	if (argc == 2)
	{
		HUINT32					 ulRfInputId = (HUINT32)HxOBJECT_INT(argv[0]);
		HBOOL					 bAbility = (HBOOL)HxOBJECT_INT(argv[1]);
		oapiRscContext_t		*pstContext = oapi_rsc_GetContext();
		HERROR					 hErr;

		// Set the RLIB setting
		hErr = RLIB_SetRfInputAbility(pstContext->hSession, ulRfInputId, bAbility);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("RLIB_SetRfInputAbility err: ulRscId(%d)\n", ulRfInputId);
		}
	}
	else
	{
		HxLOG_Error("resource.onTunerSet has wrong argument number:(%d)\n", argc);
	}

	LEAVE_RSC_CRITICAL;
}

#define _____OAPI_FUNCTION_BODY_____
HERROR oapi_rsc_ExecInit (void)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (TRUE != s_bOapiRsc_Inited)
	{
		oapiRscContext_t		*pstContext = oapi_rsc_GetContext();

		hErr = HxSEMT_Create(&s_ulOapiRsc_SemId, "oapiRscSem", 0);

		HxSTD_MemSet(pstContext, 0, sizeof(oapiRscContext_t));
		HLIB_RPC_RegisterNotifyCallback (nRpcHandle, "resource.onActivated", "b", NULL, (HxRPC_Noti_t)oapi_rsc_CbOnActivated);
		HLIB_RPC_RegisterNotifyCallback (nRpcHandle, "resource.onRscUsageTypeSet", "iib", NULL, (HxRPC_Noti_t)oapi_rsc_CbOnRscUsageTypeSet);
		HLIB_RPC_RegisterNotifyCallback (nRpcHandle, "resource.onRfInputSet", "ib", NULL, (HxRPC_Noti_t)oapi_rsc_CbOnRfInputCapabilitySet);
		HLIB_RPC_RegisterNotifyCallback (nRpcHandle, "resource.onTunerSet", "ii", NULL, (HxRPC_Noti_t)oapi_rsc_CbOnTunerCapabilitySet);
		HLIB_RPC_RegisterNotifyCallback (nRpcHandle, "resource.onRfInputAbilityChanged", "ii", NULL, (HxRPC_Noti_t)oapi_rsc_CbOnRfInputAbilityChanged);

		HLIB_RPC_Call (nRpcHandle, "resource.activate", NULL);
		s_bOapiRsc_Inited = TRUE;
	}

	return ERR_OK;
}

HERROR oapi_rsc_ExecSetListener (OxRsc_Notifier_t pfNotifier, void *pvUserData)
{
	oapiRscNotifier_t	*pstNotifier;

	if (NULL == pfNotifier)		{ return ERR_FAIL; }

	pstNotifier = oapi_rsc_FindNotifier(pfNotifier, pvUserData);
	if (NULL != pstNotifier)
	{
		HxLOG_Error("Already registered...\n");
		return ERR_OK;
	}

	pstNotifier = (oapiRscNotifier_t *)HLIB_STD_MemAlloc(sizeof(oapiRscNotifier_t));
	if (NULL == pstNotifier)
	{
		HxLOG_Error("Memory allocation failed\n");
		return ERR_FAIL;
	}

	pstNotifier->pfListener	= pfNotifier;
	pstNotifier->pvUserData	= pvUserData;

	s_pstOapiRsc_NotifierList = HLIB_LIST_Append(s_pstOapiRsc_NotifierList, (void *)pstNotifier);
	return ERR_OK;
}

HERROR oapi_rsc_ExecResetListener (OxRsc_Notifier_t pfNotifier, void *pvUserData)
{
	oapiRscNotifier_t	*pstNotifier;

	if (NULL == pfNotifier)		{ return ERR_FAIL; }

	pstNotifier = oapi_rsc_FindNotifier(pfNotifier, pvUserData);
	if (NULL == pstNotifier)
	{
		HxLOG_Error("No Notifier...\n");
		return ERR_OK;
	}

	s_pstOapiRsc_NotifierList = HLIB_LIST_Remove(s_pstOapiRsc_NotifierList, (void *)pstNotifier);
	HLIB_STD_MemFree (pstNotifier);

	return ERR_OK;
}

HERROR oapi_rsc_ExecGetInitialInfo (OxRsc_InitialInfo_t *pstInitInfo)
{
	oapiRscContext_t	*pstContext = oapi_rsc_GetContext();

	if (NULL == pstInitInfo)	{ return ERR_FAIL; }
	if (TRUE != s_bOapiRsc_Activated)
	{
		HxLOG_Error("Not Activated yet:\n");
		return ERR_FAIL;
	}

	HxSTD_MemCopy(pstInitInfo, &(pstContext->stInitInfo),sizeof(OxRsc_InitialInfo_t));
	return ERR_OK;
}

HERROR oapi_rsc_ExecGetResourceId (HUINT32 ulActionId, RxRscType_e eRscType, HUINT32 *pulRscId)
{
	HUINT32				 ulRscId = RxRSCID_NULL;
	oapiRscContext_t	*pstContext = oapi_rsc_GetContext();
	HERROR				 hErr;

	if (NULL != pulRscId)					{ *pulRscId = RxRSCID_NULL; }

	if (TRUE != s_bOapiRsc_Activated)
	{
		HxLOG_Error("Not Activated yet:\n");
		return ERR_FAIL;
	}

	if (ulActionId >= pstContext->ulActNum)
	{
		HxLOG_Error ("ActionId (%d)\n", ulActionId);
		return ERR_FAIL;
	}

	if (eRscType >= eRxRSCTYPE_MAX)
	{
		HxLOG_Error ("eRscType (%d)\n", eRscType);
		return ERR_FAIL;
	}

	hErr = RLIB_GetResourceId(pstContext->hSession, ulActionId, eRscType, &ulRscId);
	if ((ERR_OK != hErr) || (ulRscId >= pstContext->stInitInfo.aulRscNum[eRscType]))
	{
		HxLOG_Error ("RLIB_GetResourceId err: ActionId(%d) RscType(%d)\n", ulActionId, eRscType);
		return ERR_FAIL;
	}

	if (NULL != pulRscId)					{ *pulRscId = ulRscId; }
	return ERR_OK;
}

RxRscReqHandle_t oapi_rsc_ExecMakeResourceRequestHandle (HUINT32 ulActionId, HUINT32 ulActionPriority, HBOOL bUseVirtualHandle)
{
	oapiRscContext_t	*pstContext = oapi_rsc_GetContext();
	Handle_t			hSession;

	if (TRUE != s_bOapiRsc_Activated)
	{
		HxLOG_Error("Not Activated yet:\n");
		return (Handle_t)NULL;
	}
	if(bUseVirtualHandle == TRUE)
	{
		hSession = pstContext->hSessionVirtual;
	}
	else
	{
		hSession = pstContext->hSession;
	}

	return RLIB_MakeResourceRequestHandle(hSession, ulActionId, ulActionPriority);
}

RxRscReqHandle_t oapi_rsc_ExecDuplicateRequestHandle (RxRscReqHandle_t hSession)
{
	if (TRUE != s_bOapiRsc_Activated)
	{
		HxLOG_Error("Not Activated yet:\n");
		return ERR_FAIL;
	}

	return RLIB_DuplicateRequestHandle(hSession);
}

HERROR oapi_rsc_ExecAddRemovableActionIdToRequestHandle (Handle_t hRscReq, HUINT32 ulActionId, HUINT32 ulActionPriority)
{
//	oapiRscContext_t	*pstContext = oapi_rsc_GetContext();

	if (TRUE != s_bOapiRsc_Activated)
	{
		HxLOG_Error("Not Activated yet:\n");
		return ERR_FAIL;
	}

	return RLIB_AddRemovableActionIdToRequestHandle((RxRscReqHandle_t)hRscReq, ulActionId, ulActionPriority);
}

HERROR oapi_rsc_ExecAddIgnorableActionIdToRequestHandle (Handle_t hRscReq, HUINT32 ulActionId)
{
//	oapiRscContext_t	*pstContext = oapi_rsc_GetContext();

	if (TRUE != s_bOapiRsc_Activated)
	{
		HxLOG_Error("Not Activated yet:\n");
		return ERR_FAIL;
	}

	return RLIB_AddIgnorableActionIdToRequestHandle((RxRscReqHandle_t)hRscReq, ulActionId);
}

HERROR oapi_rsc_ExecAddRequiredResourceToRequestHandle (Handle_t hRscReq, RxRscType_e eRscType, HUINT32 ulResourceId, HBOOL bSharable, RxUsageType_e eUsageType)
{
//	oapiRscContext_t	*pstContext = oapi_rsc_GetContext();

	if (TRUE != s_bOapiRsc_Activated)
	{
		HxLOG_Error("Not Activated yet:\n");
		return ERR_FAIL;
	}

	return RLIB_AddRequiredResourceToRequestHandle((RxRscReqHandle_t)hRscReq, eRscType, ulResourceId, bSharable, eUsageType);
}

HERROR oapi_rsc_ExecAddArgumentToRequestHandle (Handle_t hRscReq, RxTrialArg_t *punArg)
{
//	oapiRscContext_t	*pstContext = oapi_rsc_GetContext();

	if (TRUE != s_bOapiRsc_Activated)
	{
		HxLOG_Error("Not Activated yet:\n");
		return ERR_FAIL;
	}

	return RLIB_AddArgumentToRequestHandle((RxRscReqHandle_t)hRscReq, punArg);
}

HERROR oapi_rsc_ExecTryResourceRequest (Handle_t hRscReq)
{
//	oapiRscContext_t	*pstContext = oapi_rsc_GetContext();

	if (TRUE != s_bOapiRsc_Activated)
	{
		HxLOG_Error("Not Activated yet:\n");
		return ERR_FAIL;
	}

	return RLIB_TryResourceRequest((RxRscReqHandle_t)hRscReq);
}

RxRscTrialResult_t *oapi_rsc_ExecGetRequestTrialResult (Handle_t hRscReq)
{
//	oapiRscContext_t	*pstContext = oapi_rsc_GetContext();

	if (TRUE != s_bOapiRsc_Activated)
	{
		HxLOG_Error("Not Activated yet:\n");
		return NULL;
	}

	return RLIB_GetRequestTrialResult((RxRscReqHandle_t)hRscReq);
}

HERROR oapi_rsc_ExecDestroyResourceRequestHandle (RxRscReqHandle_t hRscReq)
{
//	oapiRscContext_t	*pstContext = oapi_rsc_GetContext();

	if (TRUE != s_bOapiRsc_Activated)
	{
		HxLOG_Error("Not Activated yet:\n");
		return ERR_FAIL;
	}

	return RLIB_DestroyResourceRequestHandle((RxRscReqHandle_t)hRscReq);
}

HERROR oapi_rsc_ExecSetResourcesByTrialResults (RxRscReqHandle_t hRscReq)
{
//	oapiRscContext_t	*pstContext = oapi_rsc_GetContext();

	if (TRUE != s_bOapiRsc_Activated)
	{
		HxLOG_Error("Not Activated yet:\n");
		return ERR_FAIL;
	}

	return RLIB_SetResourcesByTrialResults((RxRscReqHandle_t)hRscReq);
}


HERROR oapi_rsc_ExecGetEmptyRecordAndBgActionIdArray (HUINT32 *pulActionIdArray, HUINT32 ulNum)
{
	HUINT32				 ulActionId;
	oapiRscContext_t	*pstContext = oapi_rsc_GetContext();
	HUINT32				unCnt = 0;

	if (NULL == pulActionIdArray || ulNum == 0)					{ return ERR_FAIL; }

	if (TRUE != s_bOapiRsc_Activated)
	{
		HxLOG_Error("Not Activated yet:\n");
		return ERR_FAIL;
	}

	for (ulActionId = pstContext->stInitInfo.ulRecActIdFirst; ulActionId <= pstContext->stInitInfo.ulRecActIdLast; ulActionId++)
	{
		OxRsc_Action_t		*pstAction = oapi_rsc_GetAction(ulActionId);

		if(unCnt >= ulNum)
		{
			break;
		}

		if ((NULL != pstAction) && (TRUE != pstAction->bActionTaken))
		{
			(*pulActionIdArray++) = ulActionId;
			unCnt++;
		}
	}


	for (ulActionId = pstContext->stInitInfo.ulBgActIdFirst; ulActionId <= pstContext->stInitInfo.ulBgActIdLast; ulActionId++)
	{
		OxRsc_Action_t		*pstAction = oapi_rsc_GetAction(ulActionId);

		if(unCnt >= ulNum)
		{
			break;
		}

		if ((NULL != pstAction) && (TRUE != pstAction->bActionTaken))
		{
			(*pulActionIdArray++) = ulActionId;
			unCnt++;
		}
	}




	if(unCnt < ulNum)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}


HERROR oapi_rsc_ExecGetEmptyRecordActionIdArray (HUINT32 *pulActionIdArray, HUINT32 ulNum)
{
	HUINT32				 ulActionId;
	oapiRscContext_t	*pstContext = oapi_rsc_GetContext();
	HUINT32				unCnt = 0;

	if (NULL == pulActionIdArray || ulNum == 0)					{ return ERR_FAIL; }

	if (TRUE != s_bOapiRsc_Activated)
	{
		HxLOG_Error("Not Activated yet:\n");
		return ERR_FAIL;
	}

	for (ulActionId = pstContext->stInitInfo.ulRecActIdFirst; ulActionId <= pstContext->stInitInfo.ulRecActIdLast; ulActionId++)
	{
		OxRsc_Action_t		*pstAction = oapi_rsc_GetAction(ulActionId);

		if(unCnt >= ulNum)
		{
			break;
		}

		if ((NULL != pstAction) && (TRUE != pstAction->bActionTaken))
		{
			(*pulActionIdArray++) = ulActionId;
			unCnt++;
		}
	}

	if(unCnt < ulNum)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}


HERROR oapi_rsc_ExecGetEmptyRecordActionId (HUINT32 *pulActionId)
{
	HUINT32				 ulActionId;
	oapiRscContext_t	*pstContext = oapi_rsc_GetContext();

	if (NULL == pulActionId)					{ return ERR_FAIL; }

	if (TRUE != s_bOapiRsc_Activated)
	{
		HxLOG_Error("Not Activated yet:\n");
		return ERR_FAIL;
	}

	for (ulActionId = pstContext->stInitInfo.ulRecActIdFirst; ulActionId <= pstContext->stInitInfo.ulRecActIdLast; ulActionId++)
	{
		OxRsc_Action_t		*pstAction = oapi_rsc_GetAction(ulActionId);

		if ((NULL != pstAction) && (TRUE != pstAction->bActionTaken))
		{
			*pulActionId = ulActionId;
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

HERROR oapi_rsc_ExecGetEmptyBgActionId (HUINT32 *pulActionId)
{
	HUINT32				 ulActionId;
	oapiRscContext_t	*pstContext = oapi_rsc_GetContext();

	if (NULL == pulActionId)					{ return ERR_FAIL; }

	if (TRUE != s_bOapiRsc_Activated)
	{
		HxLOG_Error("Not Activated yet:\n");
		return ERR_FAIL;
	}

	for (ulActionId = pstContext->stInitInfo.ulBgActIdFirst; ulActionId <= pstContext->stInitInfo.ulBgActIdLast; ulActionId++)
	{
		OxRsc_Action_t		*pstAction = oapi_rsc_GetAction(ulActionId);

		if ((NULL != pstAction) && (TRUE != pstAction->bActionTaken))
		{
			*pulActionId = ulActionId;
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

HERROR oapi_rsc_ExecGetActionPriority (HUINT32 ulActionId, HUINT32 *pulPriority)
{
	OxRsc_Action_t		*pstAction = oapi_rsc_GetAction(ulActionId);

	if (NULL != pulPriority)
	{
		if ((NULL != pstAction) && (TRUE == pstAction->bActionTaken))
		{
			*pulPriority = pstAction->ulActionPriority;
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}


HERROR oapi_rsc_ExecResetVirtualHandleInfo (void)
{
	oapiRscContext_t	*pstContext = oapi_rsc_GetContext();
	HERROR			hErr = ERR_FAIL;

	if (TRUE != s_bOapiRsc_Activated)
	{
		HxLOG_Error("Not Activated yet:\n");
		return ERR_FAIL;
	}

	hErr = RLIB_ClearSessionHandleInfo(pstContext->hSessionVirtual);
	if(hErr == ERR_FAIL)
	{
		HxLOG_Error("RLIB_ClearSessionHandle() fail!:\n");
		return ERR_FAIL;
	}

	hErr = RLIB_CopySessionHandleInfo(pstContext->hSession, pstContext->hSessionVirtual);
	if(hErr == ERR_FAIL)
	{
		HxLOG_Error("RLIB_CopySessionHandleInfo() fail!:\n");
		return ERR_FAIL;
	}

	return ERR_OK;

}

HERROR oapi_rsc_GetResourceNumber( RxRscType_e eRscType, HUINT32 *pulRscNum)
{
	oapiRscContext_t	*pstContext = oapi_rsc_GetContext();
	HERROR			hErr = ERR_FAIL;

	if (TRUE != s_bOapiRsc_Activated)
	{
		HxLOG_Error("Not Activated yet:\n");
		return ERR_FAIL;
	}

	hErr = RLIB_GetResourceNumber(pstContext->hSession, eRscType,pulRscNum);
	if(hErr == ERR_FAIL)
	{
        *pulRscNum = 0;
		HxLOG_Error("RLIB_GetResourceNumber() fail!:\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}




#define ___GLOBAL_FUNCTIONS___
HERROR OAPI_RSC_Init (void)
{
	HERROR			 hErr;

	hErr = oapi_rsc_ExecInit();

	return hErr;
}

HERROR OAPI_RSC_SetListener (OxRsc_Notifier_t pfNotifier, void *pvUserData)
{
	HERROR			 hErr;

	ENTER_RSC_CRITICAL;
	hErr = oapi_rsc_ExecSetListener(pfNotifier, pvUserData);
	LEAVE_RSC_CRITICAL;

	return hErr;
}

HERROR OAPI_RSC_ResetListener (OxRsc_Notifier_t pfNotifier, void *pvUserData)
{
	HERROR			 hErr;

	ENTER_RSC_CRITICAL;
	hErr = oapi_rsc_ExecResetListener(pfNotifier, pvUserData);
	LEAVE_RSC_CRITICAL;

	return hErr;
}

HERROR OAPI_RSC_GetInitialInfo (OxRsc_InitialInfo_t *pstInitInfo)
{
	HERROR			 hErr;

	ENTER_RSC_CRITICAL;
	hErr = oapi_rsc_ExecGetInitialInfo(pstInitInfo);
	LEAVE_RSC_CRITICAL;

	return hErr;
}

HERROR OAPI_RSC_GetResourceId (HUINT32 ulActionId, RxRscType_e eRscType, HUINT32 *pulRscId)
{
	HERROR			 hErr;

	ENTER_RSC_CRITICAL;
	hErr = oapi_rsc_ExecGetResourceId(ulActionId, eRscType, pulRscId);
	LEAVE_RSC_CRITICAL;

	return hErr;
}

RxRscReqHandle_t OAPI_RSC_MakeResourceRequestHandle (HUINT32 ulActionId, HUINT32 ulActionPriority, HBOOL bUseVirtualHandle)
{
	RxRscReqHandle_t	 hReq;

	ENTER_RSC_CRITICAL;
	hReq = oapi_rsc_ExecMakeResourceRequestHandle(ulActionId, ulActionPriority, bUseVirtualHandle);
	LEAVE_RSC_CRITICAL;

	return hReq;
}

RxRscReqHandle_t OAPI_RSC_DuplicateRequestHandle (RxRscReqHandle_t hRscReq)
{
	RxRscReqHandle_t	 hReq;

	ENTER_RSC_CRITICAL;
	hReq = oapi_rsc_ExecDuplicateRequestHandle(hRscReq);
	LEAVE_RSC_CRITICAL;

	return hReq;
}

HERROR OAPI_RSC_AddRemovableActionIdToRequestHandle (Handle_t hRscReq, HUINT32 ulActionId, HUINT32 ulActionPriority)
{
	HERROR			 hErr;

	ENTER_RSC_CRITICAL;
	hErr = oapi_rsc_ExecAddRemovableActionIdToRequestHandle(hRscReq, ulActionId, ulActionPriority);
	LEAVE_RSC_CRITICAL;

	return hErr;
}

HERROR OAPI_RSC_AddIgnorableActionIdToRequestHandle (Handle_t hRscReq, HUINT32 ulActionId)
{
	HERROR			 hErr;

	ENTER_RSC_CRITICAL;
	hErr = oapi_rsc_ExecAddIgnorableActionIdToRequestHandle(hRscReq, ulActionId);
	LEAVE_RSC_CRITICAL;

	return hErr;
}

HERROR OAPI_RSC_AddRequiredResourceToRequestHandle (Handle_t hRscReq, RxRscType_e eRscType, HUINT32 ulResourceId, HBOOL bSharable, RxUsageType_e eUsageType)
{
	HERROR			 hErr;

	ENTER_RSC_CRITICAL;
	hErr = oapi_rsc_ExecAddRequiredResourceToRequestHandle(hRscReq, eRscType, ulResourceId, bSharable, eUsageType);
	LEAVE_RSC_CRITICAL;

	return hErr;
}

HERROR OAPI_RSC_AddArgumentToRequestHandle (Handle_t hRscReq, RxTrialArg_t *punArg)
{
	HERROR			 hErr;

	ENTER_RSC_CRITICAL;
	hErr = oapi_rsc_ExecAddArgumentToRequestHandle(hRscReq, punArg);
	LEAVE_RSC_CRITICAL;

	return hErr;
}

HERROR OAPI_RSC_TryResourceRequest (Handle_t hRscReq)
{
	HERROR			 hErr;

	ENTER_RSC_CRITICAL;
	hErr = oapi_rsc_ExecTryResourceRequest(hRscReq);
	LEAVE_RSC_CRITICAL;

	return hErr;
}

RxRscTrialResult_t *OAPI_RSC_GetRequestTrialResult (Handle_t hRscReq)
{
	RxRscTrialResult_t	*pstTrial;

	ENTER_RSC_CRITICAL;
	pstTrial = oapi_rsc_ExecGetRequestTrialResult(hRscReq);
	LEAVE_RSC_CRITICAL;

	return pstTrial;
}

HERROR OAPI_RSC_DestroyResourceRequestHandle (RxRscReqHandle_t hRscReq)
{
	HERROR			 hErr;

	ENTER_RSC_CRITICAL;
	hErr = oapi_rsc_ExecDestroyResourceRequestHandle(hRscReq);
	LEAVE_RSC_CRITICAL;

	return hErr;
}

HERROR OAPI_RSC_SetResourcesByTrialResults (RxRscReqHandle_t hRscReq)
{
	HERROR			 hErr;

	ENTER_RSC_CRITICAL;
	hErr = oapi_rsc_ExecSetResourcesByTrialResults(hRscReq);
	LEAVE_RSC_CRITICAL;

	return hErr;
}


HERROR OAPI_RSC_GetEmptyRecordAndBgActionIdArray (HUINT32 *pulActionIdArray, HUINT32 ulNum)
{
	HERROR			 hErr;

	ENTER_RSC_CRITICAL;
	hErr = oapi_rsc_ExecGetEmptyRecordAndBgActionIdArray(pulActionIdArray, ulNum);
	LEAVE_RSC_CRITICAL;

	return hErr;
}


HERROR OAPI_RSC_GetEmptyRecordActionIdArray (HUINT32 *pulActionIdArray, HUINT32 ulNum)
{
	HERROR			 hErr;

	ENTER_RSC_CRITICAL;
	hErr = oapi_rsc_ExecGetEmptyRecordActionIdArray(pulActionIdArray, ulNum);
	LEAVE_RSC_CRITICAL;

	return hErr;
}


HERROR OAPI_RSC_GetEmptyRecordActionId (HUINT32 *pulActionId)
{
	HERROR			 hErr;

	ENTER_RSC_CRITICAL;
	hErr = oapi_rsc_ExecGetEmptyRecordActionId(pulActionId);
	LEAVE_RSC_CRITICAL;

	return hErr;
}

HERROR OAPI_RSC_GetEmptyBgActionId (HUINT32 *pulActionId)
{
	HERROR			 hErr;

	ENTER_RSC_CRITICAL;
	hErr = oapi_rsc_ExecGetEmptyBgActionId(pulActionId);
	LEAVE_RSC_CRITICAL;

	return hErr;
}

HERROR OAPI_RSC_GetActionPriority (HUINT32 ulActionId, HUINT32 *pulPriority)
{
	HERROR			 hErr;

	ENTER_RSC_CRITICAL;
	hErr = oapi_rsc_ExecGetActionPriority(ulActionId, pulPriority);
	LEAVE_RSC_CRITICAL;

	return hErr;
}

HERROR OAPI_RSC_ResetVirtualHandleInfo (void)
{
	HERROR			 hErr;

	ENTER_RSC_CRITICAL;
	hErr = oapi_rsc_ExecResetVirtualHandleInfo();
	LEAVE_RSC_CRITICAL;

	return hErr;
}

HERROR OAPI_RSC_GetResourceNumber ( RxRscType_e eRscType, HUINT32 *pulRscNum)
{
	HERROR			 hErr;

	ENTER_RSC_CRITICAL;
	hErr = oapi_rsc_GetResourceNumber(eRscType,pulRscNum);
	LEAVE_RSC_CRITICAL;

	return hErr;
}




