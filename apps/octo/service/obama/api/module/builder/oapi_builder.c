/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  oapi_builder.c
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
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
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
#include <vkernel.h>
#include <hlib.h>
#include <oapi.h>
#include <obama_int.h>

#include "../../oapi_mem.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HUINT32					 ulBuilderId;
	HxList_t				*pstBuildInfoList;			// DxBuilder_BuildInfo_t
	HxList_t				*pstSentBuildInfoList;		// DxBuilder_BuildInfo_t
	OxBuilder_Option_t		 stOption;

	//search result
	OxBuilder_BuilderState_t		 stBuilderState;
	HxList_t						*pstSearchSvcList;	// DxBuilder_SearchedInfo_t

} oapiBuilder_Context_t;

typedef struct
{
	OxBuilder_Listener_t	 pfListener;
	void					*pvUserData;
} oapiBuilder_Notifier_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HULONG					s_ulOapiBuilderSemId;


STATIC HBOOL					 s_bOapiBuild_Inited = FALSE, s_bOapiBuild_Activated = FALSE;
STATIC OxBuilder_Activated_t	 s_pfOapiBuild_CbActivated = NULL;
STATIC HUINT32					 s_ulOapiBuild_BuilderIdx = 0x0001;
STATIC HxList_t					*s_pstOapiBuild_ContextList = NULL;			// oapiBuilder_Context_t
STATIC HxList_t					*s_pstOapiBuild_ListenerList = NULL;		// oapiBuilder_Notifier_t

// builder id를 사용하는 것이 아니라 context에 넣기 애매해서 별도로..
STATIC HINT32					 s_lMyLongitude = 0;
STATIC HINT32					 s_lMyLatitude = 0;

#define USE_BUILDER_SEM

#if defined(USE_BUILDER_SEM)
#define	ENTER_BUILDER_CACHE			VK_SEM_Get(s_ulOapiBuilderSemId)
#define	LEAVE_BUILDER_CACHE			VK_SEM_Release(s_ulOapiBuilderSemId)
#else
#define	ENTER_BUILDER_CACHE
#define	LEAVE_BUILDER_CACHE
#endif

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
#define ___LOCAL_FUNCTIONS___

#define _____BUILDINFO_FUNCTIONS_____
STATIC HBOOL oapi_builder_CompareBuildInfo (DxBuilder_BuildInfo_t *pstInfo1, DxBuilder_BuildInfo_t *pstInfo2)
{
	if (pstInfo1->eType == pstInfo2->eType)
	{
		switch (pstInfo1->eType)
		{
		case eDxBUILDER_BUILDINFO_TP:
			if (HxSTD_MemCmp(&(pstInfo1->unArgs.stTuneParam), &(pstInfo2->unArgs.stTuneParam), sizeof(DxTuneParam_t)) == 0)
			{
				return TRUE;
			}

			break;

		default:
			break;
		}
	}

	return FALSE;
}

STATIC DxBuilder_BuildInfo_t *oapi_builder_FindSameBuildInfo (oapiBuilder_Context_t *pstContext, DxBuilder_BuildInfo_t *pstInfo)
{
	HxList_t			*pstListItem;

	for (pstListItem = pstContext->pstBuildInfoList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		DxBuilder_BuildInfo_t	*pstCurrInfo = (DxBuilder_BuildInfo_t *)HLIB_LIST_Data (pstListItem);

		if (NULL != pstCurrInfo)
		{
			if (TRUE == oapi_builder_CompareBuildInfo (pstCurrInfo, pstInfo))
			{
				return pstCurrInfo;
			}
		}
	}

	return NULL;
}

STATIC DxBuilder_BuildInfo_t *oapi_builder_FindSameBuildInfoFromSent (oapiBuilder_Context_t *pstContext, DxBuilder_BuildInfo_t *pstInfo)
{
	HxList_t			*pstListItem;

	for (pstListItem = pstContext->pstSentBuildInfoList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		DxBuilder_BuildInfo_t	*pstCurrInfo = (DxBuilder_BuildInfo_t *)HLIB_LIST_Data (pstListItem);

		if (NULL != pstCurrInfo)
		{
			if (TRUE == oapi_builder_CompareBuildInfo (pstCurrInfo, pstInfo))
			{
				return pstCurrInfo;
			}
		}
	}

	return NULL;
}

STATIC void oapi_builder_CbFreeBuildInfo (void *pvData)
{
	DxBuilder_BuildInfo_t		*pstBuildInfo = (DxBuilder_BuildInfo_t *)pvData;

	if (NULL != pstBuildInfo)
	{
		HLIB_STD_MemFree (pstBuildInfo);
	}

	return;
}

STATIC void oapi_builder_CbFreeSearchedSvcInfo (void *pvData)
{
	DxBuilder_SearchedInfo_t		*pstSvcInfo = (DxBuilder_SearchedInfo_t *)pvData;

	if (NULL != pstSvcInfo)
	{
		HLIB_STD_MemFree (pstSvcInfo);
	}

	return;
}

STATIC HERROR oapi_builder_SendBuildInfos (oapiBuilder_Context_t *pstContext)
{
	HUINT32					 ulCount = 0, ulBuildInfoNum = 0;
	HxList_t				*pstListItem;
	DxBuilder_BuildInfo_t	*pstBuildInfoArray = NULL;
	HERROR					 hResult = ERR_FAIL;
	HINT32					 nRpcHandle = 0;
	HERROR					 hErr = ERR_FAIL;

	ulBuildInfoNum = HLIB_LIST_Length (pstContext->pstBuildInfoList);
	if (0 == ulBuildInfoNum)
	{
		HxLOG_Error ("No BuildInfo:\n");
		goto END_FUNC;
	}

	pstBuildInfoArray = (DxBuilder_BuildInfo_t *)HLIB_STD_MemCalloc (sizeof(DxBuilder_BuildInfo_t) * ulBuildInfoNum);
	if (NULL == pstBuildInfoArray)
	{
		HxLOG_Error ("Memory Allocation failed:\n");
		goto END_FUNC;
	}

	for (pstListItem = pstContext->pstBuildInfoList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		DxBuilder_BuildInfo_t			*pstBuildInfo = (DxBuilder_BuildInfo_t *)HLIB_LIST_Data (pstListItem);

		if (NULL != pstBuildInfo)
		{
			HxSTD_MemCopy (&(pstBuildInfoArray[ulCount]), pstBuildInfo, sizeof(DxBuilder_BuildInfo_t));
			ulCount ++;

			pstContext->pstSentBuildInfoList = HLIB_LIST_Append (pstContext->pstSentBuildInfoList, (void *)pstBuildInfo);
		}
	}

	pstContext->pstBuildInfoList = HLIB_LIST_RemoveAll (pstContext->pstBuildInfoList);

	if (0 == ulCount)
	{
		HxLOG_Error ("No Build Info: Something wrong!\n");
		goto END_FUNC;
	}

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OAPI_RpcProtocol_GetRpcHandle() failed.. \n");
		return ERR_FAIL;
	}

	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_BUILDER_AddBuildInfo, "iib", pstContext->ulBuilderId, ulCount, pstBuildInfoArray, sizeof(DxBuilder_BuildInfo_t) * ulCount);

	hResult = ERR_OK;

END_FUNC:
	if (NULL != pstBuildInfoArray)					{ HLIB_STD_MemFree (pstBuildInfoArray); }

	return hResult;
}


STATIC HERROR oapi_builder_SendOption (oapiBuilder_Context_t *pstContext)
{
	HINT32					 nRpcHandle = 0;
	HERROR					 hErr = ERR_FAIL;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OAPI_RpcProtocol_GetRpcHandle() failed.. \n");
		return ERR_FAIL;
	}

	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_BUILDER_SetOption, "ib", pstContext->ulBuilderId, &(pstContext->stOption), sizeof(OxBuilder_Option_t));
	return ERR_OK;
}

#define _____CONTEXT_FUNCTIONS_____
STATIC oapiBuilder_Context_t *oapi_builder_GetContextByBuilderId (HUINT32 ulBuilderId)
{
	HxList_t			*pstListItem;

	for (pstListItem = s_pstOapiBuild_ContextList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		oapiBuilder_Context_t			*pstContext = (oapiBuilder_Context_t *)HLIB_LIST_Data (pstListItem);

		if ((NULL != pstContext) && (ulBuilderId == pstContext->ulBuilderId))
		{
			return pstContext;
		}
	}

	return NULL;
}

STATIC HERROR oapi_build_FreeContext (oapiBuilder_Context_t *pstContext)
{
	if (NULL != pstContext)
	{
		pstContext->pstBuildInfoList 		= HLIB_LIST_RemoveAllFunc (pstContext->pstBuildInfoList, oapi_builder_CbFreeBuildInfo);
		pstContext->pstBuildInfoList  		= NULL;

		pstContext->pstSentBuildInfoList 	= HLIB_LIST_RemoveAllFunc (pstContext->pstSentBuildInfoList, oapi_builder_CbFreeBuildInfo);
		pstContext->pstSentBuildInfoList	= NULL;

		pstContext->pstSearchSvcList 		= HLIB_LIST_RemoveAllFunc (pstContext->pstSearchSvcList, oapi_builder_CbFreeSearchedSvcInfo);
		pstContext->pstSearchSvcList		= NULL;

		HLIB_STD_MemFree (pstContext);
		pstContext = NULL;
	}

	return ERR_OK;
}

STATIC HUINT32 oapi_builder_getNextBuilderId (void)
{
	HUINT32		 ulBuilderId;

	ulBuilderId = s_ulOapiBuild_BuilderIdx + 1;
	if (ulBuilderId >= 0xFFFE)
	{
		ulBuilderId = 0x0001;
	}

	while (ulBuilderId != s_ulOapiBuild_BuilderIdx)
	{
		if (NULL == oapi_builder_GetContextByBuilderId (ulBuilderId))
		{
			s_ulOapiBuild_BuilderIdx = ulBuilderId;
			return ulBuilderId;
		}

		ulBuilderId ++;
		if (ulBuilderId >= 0xFFFE)
		{
			ulBuilderId = 0x0001;
		}
	}

	return 0;
}

STATIC oapiBuilder_Notifier_t *oapi_builder_FindNotifier (OxBuilder_Listener_t pfListener, void *pvUserData)
{
	HxList_t			*pstListItem;

	for (pstListItem = s_pstOapiBuild_ListenerList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		oapiBuilder_Notifier_t			*pstNotifier = (oapiBuilder_Notifier_t *)HLIB_LIST_Data (pstListItem);

		if ((pfListener == pstNotifier->pfListener) &&
			(pvUserData == pstNotifier->pvUserData))
		{
			return pstNotifier;
		}
	}

	return NULL;
}

STATIC void oapi_builder_NotifyToListener (HUINT32 ulBuilderId, DxBuilder_Event_e eEvent, DxBuilder_NotifierData_t *pstNotifyData)
{
	HxList_t			*pstListItem;

	for (pstListItem = s_pstOapiBuild_ListenerList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		oapiBuilder_Notifier_t			*pstNotifier = (oapiBuilder_Notifier_t *)HLIB_LIST_Data (pstListItem);

		if ((NULL != pstNotifier) && (NULL != pstNotifier->pfListener))
		{
			(pstNotifier->pfListener) (ulBuilderId, eEvent, pstNotifyData, pstNotifier->pvUserData);
		}
	}

	return;
}



#define _____DESERIALIZATION_____


#define _____NOTIFICATION_CALLBACKS_____
STATIC void oapi_builder_CbOnStateChanged (HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{
	oapiBuilder_Context_t			*pstContext = NULL;
	HUINT32 						ulBuilderId, i, ulSvcCount;
	DxBuilder_State_e				eState;
	OxBuilder_BuilderState_t		*pstBuilderState;
	DxBuilder_SearchedInfo_t 		*pstSvcInfo, *pstNewSvc;

	HxLOG_Info ("Builder StateChanged:\n");

	if(argc != 5)
	{
		HxLOG_Error("==========> ERROR !!!! %s() has invalid argc[%d]\n\n", argc);
		return;
	}
	else if(HxOBJECT_BIN_SIZE(argv[2]) != sizeof(OxBuilder_BuilderState_t))
	{
		HxLOG_Error("==========> ERROR !!!! %s() has invalid struct size [%d]\n\n", HxOBJECT_BIN_SIZE(argv[2]));
		return;
	}


	ulBuilderId 		= HxOBJECT_INT(argv[0]);
	eState 				= HxOBJECT_INT(argv[1]);
	pstBuilderState 	= (OxBuilder_BuilderState_t *)HxOBJECT_BIN_DATA (argv[2]);
	ulSvcCount			= HxOBJECT_INT(argv[3]);
	pstSvcInfo 			= (DxBuilder_SearchedInfo_t *)HxOBJECT_BIN_DATA (argv[4]);

	HxLOG_Debug("ulBuilderId: %d \n", ulBuilderId);
	pstContext = oapi_builder_GetContextByBuilderId (ulBuilderId);
	if (NULL == pstContext)
	{
		HxLOG_Error("==========> ERROR !!!! oapi_builder_GetContextByBuilderId() failed.. builderId(%d) \n", ulBuilderId);
		return;
	}

	if (pstContext->stBuilderState.ulCompleteTuningNum >= pstBuilderState->ulCompleteTuningNum)
	{
/* MOONSG : 원 code로 하게되면 첫번째 뿐만 아니라 진행중인 TP num이 동일 할 경우 TP정보를 실제 lock하는 것과 다르게 출력되는 문제가 있어 수정함.
    ulSvcCount = 0 주는 이유는 search이후 재 search시 기존 search된 service정보가 잘못 올라오는 문제가 있어서 수정함 */
#if (1)
		ulSvcCount = 0;
#else
		return;
#endif
	}

	ENTER_BUILDER_CACHE;

	// builder state를 context에 저장하고
	HxSTD_MemCopy(&pstContext->stBuilderState, pstBuilderState, sizeof(OxBuilder_BuilderState_t));


	HxLOG_Debug("ulTotalTuningNum: %d, ulCompleteTuningNum: %d, ulSearchedSvcNum: %d, ulSearchState: %d, ulFrequency: %d, eDeliveryType: %d, eDeliveryType: %d, ulSignalStrength: %d, ulSignalQuality: %d \n",
		pstContext->stBuilderState.ulTotalTuningNum,
		pstContext->stBuilderState.ulCompleteTuningNum,
		pstContext->stBuilderState.ulSearchedSvcNum,
		pstContext->stBuilderState.ulSearchState,
		pstContext->stBuilderState.ulFrequency,
		pstContext->stBuilderState.eDeliveryType,
		pstContext->stBuilderState.eDeliveryType,
		pstContext->stBuilderState.ulSignalStrength,
		pstContext->stBuilderState.ulSignalQuality);

	// svc info list도 context에 추가하고
	for (i = 0; i < ulSvcCount; i++)
	{
		pstNewSvc= (DxBuilder_SearchedInfo_t *)HLIB_STD_MemCalloc (sizeof(DxBuilder_SearchedInfo_t));
		if (NULL == pstNewSvc)
		{
			HxLOG_Error("==========> mem alloc failed.. builderId(%d) \n", ulBuilderId);
			break;
		}

		HxSTD_MemCopy(pstNewSvc, &pstSvcInfo[i], sizeof(DxBuilder_SearchedInfo_t));
		pstContext->pstSearchSvcList = HLIB_LIST_Append (pstContext->pstSearchSvcList, (void *)pstNewSvc);
	}
	LEAVE_BUILDER_CACHE;

	oapi_builder_NotifyToListener(ulBuilderId, eDxBUILDER_SEARCH_STATUS_CHANGED, NULL);

	return;
}

STATIC void oapi_builder_CbOnActivated (HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{
	HINT32					 nRpcHandle = 0;
	HERROR					 hErr = ERR_FAIL;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OAPI_RpcProtocol_GetRpcHandle() failed.. \n");
		return;
	}

	// Activate the notifiers:
	HLIB_RPC_RegisterNotifyCallback (nRpcHandle, RPC_OBAMA_BUILDER_onStateChanged, "iibib", NULL, (HxRPC_Noti_t)oapi_builder_CbOnStateChanged);

	// store current my longitude, latitude
	if(argc != 2)
	{
		HxLOG_Error("==========> ERROR !!!! %s() has invalid argc[%d]\n\n", argc);
		s_lMyLongitude = s_lMyLatitude = 0;
	}
	else
	{
		s_lMyLongitude 		= HxOBJECT_INT(argv[0]);
		s_lMyLatitude 		= HxOBJECT_INT(argv[1]);
	}

	HxLOG_Info ("Builder Activated:\n");
	s_bOapiBuild_Activated = TRUE;

	if (NULL != s_pfOapiBuild_CbActivated)
	{
		s_pfOapiBuild_CbActivated();
	}
}


#define ___GLOBAL_FUNCTIONS___
HERROR OAPI_BUILDER_Init (OxBuilder_Activated_t pfActivated)
{
	HINT32					 nRpcHandle = 0;
	HERROR					 hErr = ERR_FAIL;

	HxLOG_Info("\n");

	if (TRUE != s_bOapiBuild_Inited)
	{
#if defined(USE_BUILDER_SEM)
		if (VK_SEM_Create(&s_ulOapiBuilderSemId, "OapiBuilderSema", VK_SUSPENDTYPE_PRIORITY) != VK_OK)
		{
			HxLOG_Error("VK_SEM_Create error! \n");
		}
#endif

		hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("OAPI_RpcProtocol_GetRpcHandle() failed.. \n");
			return ERR_FAIL;
		}

		HLIB_RPC_RegisterNotifyCallback (nRpcHandle, RPC_OBAMA_BUILDER_Inited, "ii", NULL, (HxRPC_Noti_t)oapi_builder_CbOnActivated);
		HLIB_RPC_Call (nRpcHandle, RPC_OAPI_BUILDER_Init, NULL);

		s_pfOapiBuild_CbActivated = pfActivated;
		s_bOapiBuild_Inited = TRUE;
	}

	return ERR_OK;
}

HERROR OAPI_BUILDER_SetListener (OxBuilder_Listener_t pfListener, void *pvUserData)
{
	oapiBuilder_Notifier_t	*pstNotifier;

	if (NULL == pfListener)			{ return ERR_FAIL; }

	HxLOG_Info("\n");

	pstNotifier = oapi_builder_FindNotifier (pfListener, pvUserData);
	if (NULL != pstNotifier)
	{
		HxLOG_Error ("Already registered...\n");
		return ERR_OK;
	}

	pstNotifier = (oapiBuilder_Notifier_t *)HLIB_STD_MemAlloc (sizeof(oapiBuilder_Notifier_t));
	if (NULL == pstNotifier)
	{
		HxLOG_Error ("Memory allocation failed\n");
		return ERR_FAIL;
	}

	pstNotifier->pfListener	= pfListener;
	pstNotifier->pvUserData	= pvUserData;

	s_pstOapiBuild_ListenerList = HLIB_LIST_Append (s_pstOapiBuild_ListenerList, (void *)pstNotifier);
	return ERR_OK;
}

HERROR OAPI_BUILDER_ResetListener (OxBuilder_Listener_t pfListener, void *pvUserData)
{
	oapiBuilder_Notifier_t	*pstNotifier;

	HxLOG_Info("\n");

	if (NULL == pfListener)			{ return ERR_FAIL; }

	pstNotifier = oapi_builder_FindNotifier (pfListener, pvUserData);
	if (NULL == pstNotifier)
	{
		HxLOG_Error ("No notifier...\n");
		return ERR_OK;
	}

	s_pstOapiBuild_ListenerList = HLIB_LIST_Remove (s_pstOapiBuild_ListenerList, (const void *)pstNotifier);
	HLIB_STD_MemFree (pstNotifier);

	return ERR_OK;
}

HERROR OAPI_BUILDER_Open (HUINT32 *pulBuilderId)
{
	HUINT32				 	ulBuilderId;
	oapiBuilder_Context_t	*pstContext;
	HINT32					 nRpcHandle = 0;
	HERROR					 hErr = ERR_FAIL;

	HxLOG_Info("+++\n");

	if (NULL == pulBuilderId)					{ return ERR_FAIL; }
	if (TRUE != s_bOapiBuild_Activated)
	{
		HxLOG_Error ("Builder is not activated!\n");
		return ERR_FAIL;
	}

	pstContext= (oapiBuilder_Context_t *)HLIB_STD_MemCalloc (sizeof(oapiBuilder_Context_t));
	if (NULL == pstContext)
	{
		return ERR_FAIL;
	}

	ulBuilderId = oapi_builder_getNextBuilderId();

	pstContext->ulBuilderId = ulBuilderId;
	pstContext->stOption.eOptionType = eOxBUILDER_OPTIONTYPE_NONE;

	s_pstOapiBuild_ContextList = HLIB_LIST_Append (s_pstOapiBuild_ContextList, (void *)pstContext);

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OAPI_RpcProtocol_GetRpcHandle() failed.. \n");
		return ERR_FAIL;
	}

	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_BUILDER_Open, "i", ulBuilderId);

	HxLOG_Info("ulBuilderId(%d) \n", ulBuilderId);

	if (NULL != pulBuilderId)					{ *pulBuilderId = ulBuilderId; }
	return ERR_OK;
}

HERROR OAPI_BUILDER_Close (HUINT32 ulBuilderId)
{
	oapiBuilder_Context_t		*pstContext = oapi_builder_GetContextByBuilderId (ulBuilderId);
	HINT32					 	 nRpcHandle = 0;
	HERROR					 	 hErr = ERR_FAIL;

	HxLOG_Info("ulBuilderId(%d) \n", ulBuilderId);

	if (NULL == pstContext)						{ return ERR_FAIL; }

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OAPI_RpcProtocol_GetRpcHandle() failed.. \n");
		return ERR_FAIL;
	}

	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_BUILDER_Close, "i", ulBuilderId);
	s_pstOapiBuild_ContextList = HLIB_LIST_Remove (s_pstOapiBuild_ContextList, (void *)pstContext);

	ENTER_BUILDER_CACHE;
	oapi_build_FreeContext (pstContext);
	LEAVE_BUILDER_CACHE;

	return ERR_OK;
}

HERROR OAPI_BUILDER_ClearBuildInfo (HUINT32 ulBuilderId)
{
	oapiBuilder_Context_t		*pstContext = oapi_builder_GetContextByBuilderId (ulBuilderId);

	HxLOG_Info("ulBuilderId(%d) \n", ulBuilderId);

	if (NULL == pstContext)						{ return ERR_FAIL; }

	ENTER_BUILDER_CACHE;
	pstContext->pstBuildInfoList 		= HLIB_LIST_RemoveAllFunc (pstContext->pstBuildInfoList, oapi_builder_CbFreeBuildInfo);
	pstContext->pstBuildInfoList  		= NULL;

	pstContext->pstSentBuildInfoList 	= HLIB_LIST_RemoveAllFunc (pstContext->pstSentBuildInfoList, oapi_builder_CbFreeBuildInfo);
	pstContext->pstSentBuildInfoList	= NULL;

	pstContext->pstSearchSvcList 		= HLIB_LIST_RemoveAllFunc (pstContext->pstSearchSvcList, oapi_builder_CbFreeSearchedSvcInfo);
	pstContext->pstSearchSvcList		= NULL;
	LEAVE_BUILDER_CACHE;

	return ERR_OK;
}

HERROR OAPI_BUILDER_AddBuildInfo(HUINT32 ulBuilderId, DxBuilder_BuildInfo_t *pstParam)
{
	DxBuilder_BuildInfo_t	*pstNewParam;
	oapiBuilder_Context_t		*pstContext = oapi_builder_GetContextByBuilderId (ulBuilderId);

	HxLOG_Info("ulBuilderId(%d) \n", ulBuilderId);

	if (NULL == pstContext)						{ return ERR_FAIL; }

	if (NULL != oapi_builder_FindSameBuildInfo (pstContext, pstParam))
	{
		HxLOG_Error ("Already Build Info added\n");
		return ERR_OK;
	}

	if (NULL != oapi_builder_FindSameBuildInfoFromSent (pstContext, pstParam))
	{
		HxLOG_Error ("Already Build Info added and sent to OBAMA\n");
		return ERR_OK;
	}

	pstNewParam = (DxBuilder_BuildInfo_t *)HLIB_STD_MemAlloc (sizeof(DxBuilder_BuildInfo_t));
	if (NULL == pstNewParam)
	{
		HxLOG_Error ("Memory allocation failed\n");
		return ERR_FAIL;
	}

	HxSTD_MemCopy (pstNewParam, pstParam, sizeof(DxBuilder_BuildInfo_t));
	pstContext->pstBuildInfoList = HLIB_LIST_Append (pstContext->pstBuildInfoList, (void *)pstNewParam);

	return ERR_OK;
}

HERROR OAPI_BUILDER_RemoveBuildInfo(HUINT32 ulBuilderId, DxBuilder_BuildInfo_t *pstParam)
{
	DxBuilder_BuildInfo_t	*pstCurrParam;
	oapiBuilder_Context_t		*pstContext = oapi_builder_GetContextByBuilderId (ulBuilderId);

	HxLOG_Info("ulBuilderId(%d) \n", ulBuilderId);

	if (NULL == pstContext)						{ return ERR_FAIL; }

	pstCurrParam = oapi_builder_FindSameBuildInfo (pstContext, pstParam);
	if (NULL != pstCurrParam)
	{
		pstContext->pstBuildInfoList = HLIB_LIST_Remove (pstContext->pstBuildInfoList, (void *)pstCurrParam);
	}
	else
	{
		pstCurrParam = oapi_builder_FindSameBuildInfoFromSent (pstContext, pstParam);
		if (NULL == pstCurrParam)
		{
			HxLOG_Error ("No such build info:\n");
			return ERR_FAIL;
		}

		pstContext->pstSentBuildInfoList = HLIB_LIST_Remove (pstContext->pstSentBuildInfoList, (void *)pstCurrParam);
	}

	HLIB_STD_MemFree (pstCurrParam);

	return ERR_OK;
}

HERROR OAPI_BUILDER_GetOption(HUINT32 ulBuilderId, OxBuilder_Option_t *pstOption)
{
	oapiBuilder_Context_t		*pstContext = oapi_builder_GetContextByBuilderId (ulBuilderId);

	HxLOG_Info("ulBuilderId(%d) \n", ulBuilderId);

	if (NULL == pstContext)						{ return ERR_FAIL; }

	HxSTD_MemCopy (pstOption, &(pstContext->stOption), sizeof(OxBuilder_Option_t));

	return ERR_OK;
}

HERROR OAPI_BUILDER_SetOption (HUINT32 ulBuilderId, OxBuilder_Option_t *pstOption)
{
	oapiBuilder_Context_t		*pstContext = oapi_builder_GetContextByBuilderId (ulBuilderId);

	HxLOG_Info("ulBuilderId(%d) \n", ulBuilderId);

	if (NULL == pstContext)						{ return ERR_FAIL; }
	if (NULL == pstOption)						{ return ERR_FAIL; }

	HxSTD_MemCopy (&(pstContext->stOption), pstOption, sizeof(OxBuilder_Option_t));
	return ERR_OK;
}

HERROR OAPI_BUILDER_GetBuilderStatus (HUINT32 ulBuilderId, OxBuilder_BuilderState_t *pstBuilderStatus)
{
	oapiBuilder_Context_t		*pstContext = oapi_builder_GetContextByBuilderId (ulBuilderId);

	HxLOG_Info("ulBuilderId(%d) \n", ulBuilderId);

	if (NULL == pstContext)						{ return ERR_FAIL; }
	if (NULL == pstBuilderStatus)				{ return ERR_FAIL; }

	ENTER_BUILDER_CACHE;
	HxSTD_MemCopy (pstBuilderStatus, &(pstContext->stBuilderState), sizeof(OxBuilder_BuilderState_t));
	LEAVE_BUILDER_CACHE;

	return ERR_OK;
}

HERROR OAPI_BUILDER_GetBuilderSvcInfo (HUINT32 ulBuilderId, HINT32 lStartIndex, HINT32 lCount, HINT32 *plResultNo, DxBuilder_SearchedInfo_t **ppstSvcInfoList)
{
	oapiBuilder_Context_t			*pstContext = oapi_builder_GetContextByBuilderId (ulBuilderId);
	DxBuilder_SearchedInfo_t				*pstNewSvcList = NULL;
	HUINT32 						 ulListLen, i;
	HERROR							 hErr = ERR_FAIL;

	HxLOG_Info("ulBuilderId(%d) \n", ulBuilderId);

	if (NULL == ppstSvcInfoList) 					{ return ERR_FAIL; }

	if (NULL == pstContext)
	{
		*ppstSvcInfoList = NULL;
		return ERR_FAIL;
	}

	ENTER_BUILDER_CACHE;
	ulListLen = HLIB_LIST_Length(pstContext->pstSearchSvcList);
	if (ulListLen == 0)
	{
		HxLOG_Print ("no svc list now \n");
		hErr = ERR_OK;
		goto END_FUNC;
	}

	if (lStartIndex + lCount > ulListLen)
	{
		lCount = ulListLen - lStartIndex;
	}

	pstNewSvcList = (DxBuilder_SearchedInfo_t *)HLIB_STD_MemCalloc(sizeof(DxBuilder_SearchedInfo_t) * lCount);
	if (pstNewSvcList == NULL)
	{
		HxLOG_Error ("mem alloc failed.. \n");
		goto END_FUNC;
	}

	for (i = 0; i < lCount; i++)
	{
		HxList_t				*pstSvcInfoList = NULL;
		DxBuilder_SearchedInfo_t		*pstCurSvc = NULL;

		pstSvcInfoList = HLIB_LIST_GetListItem(pstContext->pstSearchSvcList, lStartIndex + i);
		if (pstSvcInfoList == NULL)
		{
			HxLOG_Critical ("HLIB_LIST_GetListItem() failed.. len(%d) curPos(%d)\n", ulListLen, lStartIndex+i);
			continue;
		}

		pstCurSvc = (DxBuilder_SearchedInfo_t *)HLIB_LIST_Data(pstSvcInfoList);
		if (pstCurSvc == NULL)
		{
			HxLOG_Critical ("HLIB_LIST_Data() failed.. len(%d) curPos(%d)\n", ulListLen, lStartIndex+i);
			continue;
		}

		HxLOG_Print("%d. %s\n", pstCurSvc->ulLcn, pstCurSvc->szSvcName);

		HxSTD_MemCopy(&pstNewSvcList[i], pstCurSvc, sizeof(DxBuilder_SearchedInfo_t));
	}

	*ppstSvcInfoList = 	pstNewSvcList;
	*plResultNo = lCount;
	hErr = ERR_OK;

	HxLOG_Debug("ulBuilderId(%d) return svc count(%d) \n", ulBuilderId, lCount);

END_FUNC:
	LEAVE_BUILDER_CACHE;
	return hErr;
}

HERROR OAPI_BUILDER_FreeBuilderSvcInfo (DxBuilder_SearchedInfo_t *pstSvcInfoList)
{
	HxLOG_Info("\n");

	if (pstSvcInfoList == NULL)
	{
		HxLOG_Error ("null pointer free error \n");
		return ERR_FAIL;
	}

	HLIB_STD_MemFree(pstSvcInfoList);
	pstSvcInfoList = NULL;

	return ERR_OK;
}

HERROR OAPI_BUILDER_Start (HUINT32 ulBuilderId)
{
	oapiBuilder_Context_t		*pstContext = oapi_builder_GetContextByBuilderId (ulBuilderId);
	HINT32					 	 nRpcHandle = 0;
	HERROR					 	 hErr = ERR_FAIL;

	HxLOG_Info("ulBuilderId(%d) \n", ulBuilderId);

	if (NULL == pstContext)						{ return ERR_FAIL; }
	if (eOxBUILDER_OPTIONTYPE_NONE == pstContext->stOption.eOptionType)
	{
		HxLOG_Error ("SetOption not called: cannot start.\n");
		return ERR_FAIL;
	}

	if (NULL != pstContext->pstBuildInfoList)
	{
		(void)oapi_builder_SendBuildInfos (pstContext);
	}

	(void)oapi_builder_SendOption (pstContext);

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OAPI_RpcProtocol_GetRpcHandle() failed.. \n");
		return ERR_FAIL;
	}

	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_BUILDER_Start, "i", ulBuilderId);
	return ERR_OK;
}

HERROR OAPI_BUILDER_Stop (HUINT32 ulBuilderId)
{
	HINT32					 nRpcHandle = 0;
	HERROR					 hErr = ERR_FAIL;

	HxLOG_Info("ulBuilderId(%d) \n", ulBuilderId);

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OAPI_RpcProtocol_GetRpcHandle() failed.. \n");
		return ERR_FAIL;
	}

	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_BUILDER_Stop, "i", ulBuilderId);
	return ERR_OK;
}

HERROR OAPI_BUILDER_Pause (HUINT32 ulBuilderId)
{
	HINT32					 nRpcHandle = 0;
	HERROR					 hErr = ERR_FAIL;

	HxLOG_Info("ulBuilderId(%d) \n", ulBuilderId);

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OAPI_RpcProtocol_GetRpcHandle() failed.. \n");
		return ERR_FAIL;
	}

	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_BUILDER_Pause, "i", ulBuilderId);
	return ERR_OK;
}

HERROR OAPI_BUILDER_Resume (HUINT32 ulBuilderId)
{
	HINT32					 nRpcHandle = 0;
	HERROR					 hErr = ERR_FAIL;

	HxLOG_Info("ulBuilderId(%d) \n", ulBuilderId);

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OAPI_RpcProtocol_GetRpcHandle() failed.. \n");
		return ERR_FAIL;
	}

	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_BUILDER_Resume, "i", ulBuilderId);
	return ERR_OK;
}

HERROR OAPI_BUILDER_Finish (HUINT32 ulBuilderId)
{
	HINT32					 nRpcHandle = 0;
	HERROR					 hErr = ERR_FAIL;

	HxLOG_Info("ulBuilderId(%d) \n", ulBuilderId);

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OAPI_RpcProtocol_GetRpcHandle() failed.. \n");
		return ERR_FAIL;
	}

	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_BUILDER_Finish, "i", ulBuilderId);
	return ERR_OK;
}

HERROR OAPI_BUILDER_MotorGotoPosition (HUINT32 ulBuilderId, HINT32 lPosition)
{
	HINT32					 nRpcHandle = 0;
	HERROR					 hErr = ERR_FAIL;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OAPI_RpcProtocol_GetRpcHandle() failed.. \n");
		return ERR_FAIL;
	}

	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_BUILDER_MotorGotoPosition, "ii", ulBuilderId, lPosition);
	return ERR_OK;
}

HERROR OAPI_BUILDER_MotorStorePosition (HUINT32 ulBuilderId, HINT32 lPosition)
{
	HINT32					 nRpcHandle = 0;
	HERROR					 hErr = ERR_FAIL;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OAPI_RpcProtocol_GetRpcHandle() failed.. \n");
		return ERR_FAIL;
	}

	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_BUILDER_MotorStorePosition, "ii", ulBuilderId, lPosition);
	return ERR_OK;
}

HERROR OAPI_BUILDER_MotorResetPosition (HUINT32 ulBuilderId)
{
	HINT32					 nRpcHandle = 0;
	HERROR					 hErr = ERR_FAIL;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OAPI_RpcProtocol_GetRpcHandle() failed.. \n");
		return ERR_FAIL;
	}

	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_BUILDER_MotorResetPosition, "i", ulBuilderId);
	return ERR_OK;
}

HERROR OAPI_BUILDER_MotorRecalculate (HUINT32 ulBuilderId, HINT32 lPosition)
{
	HINT32					 nRpcHandle = 0;
	HERROR					 hErr = ERR_FAIL;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OAPI_RpcProtocol_GetRpcHandle() failed.. \n");
		return ERR_FAIL;
	}

	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_BUILDER_MotorRecalculate, "ii", ulBuilderId, lPosition);
	return ERR_OK;
}

HERROR OAPI_BUILDER_MotorMove (HUINT32 ulBuilderId, DxSat_MOTOR_DIRECTION_e eDirection, DxSat_MOTOR_DRIVEMODE_e eDriveMode, HINT32 lStep)
{
	HINT32					 nRpcHandle = 0;
	HERROR					 hErr = ERR_FAIL;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OAPI_RpcProtocol_GetRpcHandle() failed.. \n");
		return ERR_FAIL;
	}

	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_BUILDER_MotorMove, "iiii", ulBuilderId, eDirection, eDriveMode, lStep);
	return ERR_OK;
}

HERROR OAPI_BUILDER_MotorStop (HUINT32 ulBuilderId)
{
	HINT32					 nRpcHandle = 0;
	HERROR					 hErr = ERR_FAIL;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OAPI_RpcProtocol_GetRpcHandle() failed.. \n");
		return ERR_FAIL;
	}

	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_BUILDER_MotorStop, "i", ulBuilderId);
	return ERR_OK;
}

HERROR OAPI_BUILDER_MotorSetLimit (HUINT32 ulBuilderId, DxSat_MOTOR_DIRECTION_e eDirection)
{
	HINT32					 nRpcHandle = 0;
	HERROR					 hErr = ERR_FAIL;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OAPI_RpcProtocol_GetRpcHandle() failed.. \n");
		return ERR_FAIL;
	}

	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_BUILDER_MotorSetLimit, "ii", ulBuilderId, eDirection);
	return ERR_OK;
}

HERROR OAPI_BUILDER_MotorEnableLimit (HUINT32 ulBuilderId)
{
	HINT32					 nRpcHandle = 0;
	HERROR					 hErr = ERR_FAIL;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OAPI_RpcProtocol_GetRpcHandle() failed.. \n");
		return ERR_FAIL;
	}

	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_BUILDER_MotorEnableLimit, "i", ulBuilderId);
	return ERR_OK;
}

HERROR OAPI_BUILDER_MotorDisableLimit (HUINT32 ulBuilderId)
{
	HINT32					 nRpcHandle = 0;
	HERROR					 hErr = ERR_FAIL;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OAPI_RpcProtocol_GetRpcHandle() failed.. \n");
		return ERR_FAIL;
	}

	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_BUILDER_MotorDisableLimit, "i", ulBuilderId);
	return ERR_OK;
}

HERROR OAPI_BUILDER_MotorGotoDegree (HUINT32 ulBuilderId, HINT32 lDegree)
{
	HINT32					 nRpcHandle = 0;
	HERROR					 hErr = ERR_FAIL;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OAPI_RpcProtocol_GetRpcHandle() failed.. \n");
		return ERR_FAIL;
	}

	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_BUILDER_MotorGotoDegree, "ii", ulBuilderId, lDegree);
	return ERR_OK;
}

HERROR OAPI_BUILDER_MotorGotoSatellite (HUINT32 ulBuilderId, HINT32 lSatellite)
{
	HINT32					 nRpcHandle = 0;
	HERROR					 hErr = ERR_FAIL;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OAPI_RpcProtocol_GetRpcHandle() failed.. \n");
		return ERR_FAIL;
	}

	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_BUILDER_MotorGotoSatellite, "ii", ulBuilderId, lSatellite);
	return ERR_OK;
}

HERROR OAPI_BUILDER_MotorSetMyLongitude (HINT32 lNewLongitude)
{
	HINT32					 nRpcHandle = 0;
	HERROR					 hErr = ERR_FAIL;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OAPI_RpcProtocol_GetRpcHandle() failed.. \n");
		return ERR_FAIL;
	}

	s_lMyLongitude = lNewLongitude;
	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_BUILDER_MotorSetMyLongitude, "i", lNewLongitude);
	return ERR_OK;
}

HERROR OAPI_BUILDER_MotorSetMyLatitude (HINT32 lNewLatitude)
{
	HINT32					 nRpcHandle = 0;
	HERROR					 hErr = ERR_FAIL;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OAPI_RpcProtocol_GetRpcHandle() failed.. \n");
		return ERR_FAIL;
	}

	s_lMyLatitude = lNewLatitude;
	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_BUILDER_MotorSetMyLatitude, "i", lNewLatitude);
	return ERR_OK;
}

HERROR OAPI_BUILDER_MotorGetMyLongitude (HINT32 *plMyLongitude)
{
	if (plMyLongitude != NULL)
	{
		*plMyLongitude = s_lMyLongitude;
		return ERR_OK;
	}

	return ERR_FAIL;
}

HERROR OAPI_BUILDER_MotorGetMyLatitude (HINT32 *plMyLatitude)
{
	if (plMyLatitude != NULL)
	{
		*plMyLatitude = s_lMyLatitude;
		return ERR_OK;
	}

	return ERR_FAIL;
}

HERROR OAPI_BUILDER_SetSelectedConflictLcnSvcInfo (HUINT32 ulBuilderId, DxBuilder_SearchedInfo_t *pstSvcInfoList, HUINT32 ulCount)
{
	HINT32					 nRpcHandle = 0;
	HERROR					 hErr = ERR_FAIL;

	HxLOG_Info("ulBuilderId(%d) \n", ulBuilderId);

	if (pstSvcInfoList == NULL)
	{
		HxLOG_Error ("null pointer free error \n");
		return ERR_FAIL;
	}

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OAPI_RpcProtocol_GetRpcHandle() failed.. \n");
		return ERR_FAIL;
	}

   	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_BUILDER_SetSelectedConflictLcn, "iib", ulBuilderId, ulCount, pstSvcInfoList, sizeof(DxBuilder_SearchedInfo_t) * ulCount);

	HLIB_STD_MemFree(pstSvcInfoList);
	pstSvcInfoList = NULL;

	return ERR_OK;
}

/* End of File */
