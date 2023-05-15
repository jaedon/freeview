/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  oapi_swupdate.c
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
//#include <obama_int.h>

#include "../../oapi_mem.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef struct
{
	DxSwUpdate_Spec_e			eSwUpdateSpec;
	Handle_t						hSwUpdate;
	HBOOL						bManual;
	DxTransponder_t				stManualTpInfo;
	DxSwUpdate_Event_e			eStateEvent;
	//_DL_SWUPDATE_AccessInfo_t		stAccessInfo;

	// step 이 겹칠수 없으므로 listener 는 한개만 사용. 대신 정확히 release 필요
	//OxSwUpdate_Notifier_t			eventListener;
} OxSwUpdateInfo_t;

typedef struct
{
	HxList_t				*pstSwUpdateInfo;		// OxSwUpdateInfo_t
} OxSwUpdateInfoList_t;


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
//STATIC HUINT32				s_ulSessionId;
STATIC OxSwUpdateInfoList_t		s_stSwUpdateInfoList = { NULL, };
STATIC OxSwUpdate_Notifier_t	s_pfSwUpdateApkNotifier;

static Handle_t hTempSwUpdate ;

STATIC OxSwUpdate_NotifierForAppkit_t	s_fnSwUpApkNotifier;

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
#if defined(CONFIG_DEBUG)
STATIC const HINT8 * oapi_swupdate_MakeStrEventType(DxSwUpdate_Event_e eEvtType)
{
	switch (eEvtType)
	{
		ENUM_TO_STR(eDxSWUPDATE_EVENT_ConnectSuccess);
		ENUM_TO_STR(eDxSWUPDATE_EVENT_ConnectFail);
		ENUM_TO_STR(eDxSWUPDATE_EVENT_DetectSuccess);
		ENUM_TO_STR(eDxSWUPDATE_EVENT_DetectSuccessInLive);
		ENUM_TO_STR(eDxSWUPDATE_EVENT_DetectFail);
		ENUM_TO_STR(eDxSWUPDATE_EVENT_DownloadSuccess);
		ENUM_TO_STR(eDxSWUPDATE_EVENT_DownloadFail);
		ENUM_TO_STR(eDxSWUPDATE_EVENT_DownloadProgress);
		ENUM_TO_STR(eDxSWUPDATE_EVENT_InstallSuccess);
		ENUM_TO_STR(eDxSWUPDATE_EVENT_InstallFail);
		ENUM_TO_STR(eDxSWUPDATE_EVENT_InstallProgress);
		ENUM_TO_STR(eDxSWUPDATE_EVENT_Erorr);
		ENUM_TO_STR(eDxSWUPDATE_EVENT_Finish);
		default:
			break;
	}
	return ("Unknown");



}
#endif

#define ___LOCAL_FUNCTIONS___
STATIC void oapi_swupdate_HandleCreated(HERROR hErr, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{
	hTempSwUpdate = (Handle_t)HxOBJECT_INT(argv[0]) ;
	return;
}

STATIC HBOOL	oapi_swupdate_CbFindByHandle(void *pvUserData, void *pvListData)
{
	OxSwUpdateInfo_t	*pstSwUpdateInfo = NULL;
	HUINT32			 hSwUpdate;

	if (NULL == pvListData){
		return FALSE;
	}
	hSwUpdate = (HUINT32)pvUserData;
	pstSwUpdateInfo = (OxSwUpdateInfo_t *)pvListData;

	return (hSwUpdate == pstSwUpdateInfo->hSwUpdate) ? TRUE : FALSE;
}

STATIC HBOOL	oapi_swupdate_FindSwUpdateInfoByHandle(HUINT32 hSwUpdate, OxSwUpdateInfo_t **ppstSwUpdateInfo)
{
	HxList_t				*pstFound = NULL;

	pstFound = HLIB_LIST_FindEx(s_stSwUpdateInfoList.pstSwUpdateInfo, (const void *)hSwUpdate, oapi_swupdate_CbFindByHandle);

	if (ppstSwUpdateInfo)
	{
		*ppstSwUpdateInfo = (OxSwUpdateInfo_t *)HLIB_LIST_Data(pstFound);
	}

	return (pstFound) ? TRUE : FALSE;
}

STATIC HERROR oapi_swupdate_AddSwUpdateInfo(DxSwUpdate_Spec_e eSwUpdateSpec, HUINT32 hSwUpdate, HBOOL bMaual, DxTransponder_t *pstTunningInfo)
{
	OxSwUpdateInfo_t *pstLocalSwUpdateInfo;

	pstLocalSwUpdateInfo = OxMEM_Malloc(sizeof(OxSwUpdateInfo_t));
	if (NULL == pstLocalSwUpdateInfo)
	{
		HxLOG_Error("NULL == pstInfo \n");
		return ERR_FAIL;
	}

	HxSTD_MemSet(pstLocalSwUpdateInfo, 0, sizeof(OxSwUpdateInfo_t));

	pstLocalSwUpdateInfo->eSwUpdateSpec 	= eSwUpdateSpec;
	pstLocalSwUpdateInfo->hSwUpdate 		= hSwUpdate;
	pstLocalSwUpdateInfo->bManual 			= bMaual;
	if(pstLocalSwUpdateInfo->bManual  == TRUE)
	{
		HxSTD_MemCopy(&pstLocalSwUpdateInfo->stManualTpInfo, pstTunningInfo, sizeof(DxTransponder_t));
	}

	s_stSwUpdateInfoList.pstSwUpdateInfo = HLIB_LIST_Append(s_stSwUpdateInfoList.pstSwUpdateInfo, (void *)pstLocalSwUpdateInfo);

	return ERR_OK;
}

STATIC HERROR oapi_swupdate_RemoveSwUpdateInfo(HUINT32 hSwupdate, OxSwUpdateInfo_t **ppstInfo)
{
	OxSwUpdateInfo_t	*pstInfo = NULL;
	HBOOL		bFound = FALSE;

	bFound =  oapi_swupdate_FindSwUpdateInfoByHandle(hSwupdate, &pstInfo);
	if (bFound == FALSE)
	{
		HxLOG_Error("hSwUpdate is not Found (hSwUpdate : %p)\n", hSwupdate);
		return ERR_FAIL;
	}

	s_stSwUpdateInfoList.pstSwUpdateInfo = HLIB_LIST_Remove(s_stSwUpdateInfoList.pstSwUpdateInfo, (const void *)pstInfo);
	if (ppstInfo)
	{
		*ppstInfo = pstInfo;
	}

	return ERR_OK;
}

STATIC HERROR oapi_swupdate_Destroy(HUINT32 hSwUpdate)
{
	OxSwUpdateInfo_t	*pstInfo = NULL;

	// detache into from map

	oapi_swupdate_RemoveSwUpdateInfo(hSwUpdate, &pstInfo);

	if(pstInfo)				// free info
		OxMEM_Free(pstInfo);

	return ERR_OK;
}


#define _____NOTIFICATION_CALLBACKS_____
STATIC void	oapi_swupdate_freeEventCb(HINT32 nArgc, void *apArgV[])
{
	void	*pvData;


	HxLOG_Assert(nArgc == 2);

	pvData = apArgV[1];

	if (pvData)
	{
		HLIB_STD_MemFree(pvData);
		pvData = NULL;
	}

}


STATIC void oapi_swupdate_EventNotifier(HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{
	DxSwUpdate_EventData_t		*pstSwUpdateEvent;

	// **   set new event notify    ** //
	DxSwUpdate_EventData_t		*pstRcvSwUpdateEvent	=	(DxSwUpdate_EventData_t *)HxOBJECT_BIN_DATA(argv[0]);

	if (pstRcvSwUpdateEvent == NULL)
	{
		HxLOG_Error("No Ota Rcv Event Data !!\n");
		return;
	}

	if (s_pfSwUpdateApkNotifier != NULL)
	{
		pstSwUpdateEvent = (DxSwUpdate_EventData_t *)HLIB_STD_MemCalloc(sizeof(DxSwUpdate_EventData_t));
		if(pstSwUpdateEvent == NULL)
		{
			HxLOG_Error("alloc fail ! \n");
			return;
		}

		HxSTD_MemCopy(pstSwUpdateEvent, pstRcvSwUpdateEvent, sizeof(DxSwUpdate_EventData_t));

		s_fnSwUpApkNotifier(s_pfSwUpdateApkNotifier, oapi_swupdate_freeEventCb, pstSwUpdateEvent);
	}
	// ** end set new event notify  ** //
}

#define ___GLOBAL_FUNCTIONS___

void OAPI_SWUPDATE_Init(OxSwUpdate_NotifierForAppkit_t pfnAppkitNotifier)
{
	HxSTD_MemSet(&s_stSwUpdateInfoList, 0, sizeof(OxSwUpdateInfoList_t));
	s_fnSwUpApkNotifier = pfnAppkitNotifier;

}

void OAPI_SWUPDATE_UnInit(void)
{
	if(s_stSwUpdateInfoList.pstSwUpdateInfo != NULL)
	{
		HLIB_LIST_RemoveAll(s_stSwUpdateInfoList.pstSwUpdateInfo);
	}

	HxSTD_MemSet(&s_stSwUpdateInfoList, 0, sizeof(OxSwUpdateInfoList_t));
}

HERROR OAPI_SWUPDATE_CreateHandle(DxSwUpdate_Spec_e eSwUpdateSpec, HUINT32 *phSwUpdate, HBOOL	bMaual, void *pvTunningInfo)
{
	HINT32	nReqId;
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);

	if (bMaual)
		nReqId = HLIB_RPC_Request (nRpcHandle, (HxRPC_Response_t)oapi_swupdate_HandleCreated, NULL, RPC_OAPI_SWUPDATE_CreateManualHandle, "iib", eSwUpdateSpec, bMaual, pvTunningInfo, sizeof(DxTransponder_t));
	else
		nReqId = HLIB_RPC_Request (nRpcHandle, (HxRPC_Response_t)oapi_swupdate_HandleCreated, NULL, RPC_OAPI_SWUPDATE_CreateHandle, "ii", eSwUpdateSpec, bMaual);

	if (HLIB_RPC_Join (nRpcHandle, nReqId, 10000) == ERR_FAIL)
	{
		HxLOG_Error("==========> ERROR !!!! %s() has HLIB_RPC_Request (nRpcHandle, ) time out !!!!\n\n", __FUNCTION__);
		*phSwUpdate = HANDLE_NULL;

		return ERR_FAIL;
	}
	if(hTempSwUpdate == HANDLE_NULL)
	{
		return ERR_FAIL;
	}

	*phSwUpdate = hTempSwUpdate;

	oapi_swupdate_AddSwUpdateInfo(eSwUpdateSpec, hTempSwUpdate, bMaual, (DxTransponder_t *)pvTunningInfo);

	return ERR_OK;
}

HERROR OAPI_SWUPDATE_SwupdateInit(OxSwUpdate_Notifier_t fnApkNotifier)
{
	HINT32	nRpcHandle = 0;

	HERROR	hErr = ERR_FAIL;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);

	HLIB_RPC_RegisterNotifyCallback (nRpcHandle, RPC_OBAMA_SWUPDATE_ResultNotifier, NULL, NULL, (HxRPC_Noti_t)oapi_swupdate_EventNotifier);

	s_pfSwUpdateApkNotifier = fnApkNotifier;

	return ERR_OK;
}

HERROR OAPI_SWUPDATE_DetectStart(HUINT32 hSwUpdate,  OxSwUpdate_Notifier_t fnApkNotifier)
{
	OxSwUpdateInfo_t *pstInfo = NULL;
	HBOOL	bFound = FALSE;
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	// manual & auto 같이 동작하도록 한다.
	bFound =  oapi_swupdate_FindSwUpdateInfoByHandle(hSwUpdate, &pstInfo) ;
	if (bFound == FALSE)
	{
		HxLOG_Error("hSwUpdate is not Found (hSwUpdate : %p)\n", hSwUpdate);
		return ERR_FAIL;
	}

	HLIB_RPC_RegisterNotifyCallback (nRpcHandle, RPC_OBAMA_SWUPDATE_ResultNotifier, NULL, NULL, (HxRPC_Noti_t)oapi_swupdate_EventNotifier);

	s_pfSwUpdateApkNotifier = fnApkNotifier;

	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_SWUPDATE_DetectStart, "i", hSwUpdate);
	return ERR_OK;
}

HERROR OAPI_SWUPDATE_DetectStop(HUINT32 hSwUpdate)
{
	OxSwUpdateInfo_t *pstInfo = NULL;
	HBOOL	bFound = FALSE;
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	bFound =  oapi_swupdate_FindSwUpdateInfoByHandle(hSwUpdate, &pstInfo) ;
	if (bFound == FALSE)
	{
		HxLOG_Error("hSwUpdate is not Found (hSwUpdate : %p)\n", hSwUpdate);
		// TODO:
		//OAPI_UnregisterCallback (RPC_OBAMA_SWUPDATE_ResultNotifier, oapi_swupdate_EventNotifier);
		return ERR_FAIL;
	}

	// TODO:
	//OAPI_UnregisterCallback (RPC_OBAMA_SWUPDATE_ResultNotifier, oapi_swupdate_EventNotifier);

	//s_pfSwUpdateApkNotifier = NULL;

	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_SWUPDATE_DetectStop, "i", hSwUpdate);
	oapi_swupdate_Destroy(hSwUpdate);

	return ERR_OK;
}

HERROR OAPI_SWUPDATE_DownloadStart(HUINT32 hSwUpdate, OxSwUpdate_Notifier_t fnApkNotifier)
{
	OxSwUpdateInfo_t *pstInfo = NULL;
	HBOOL	bFound = FALSE;
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	bFound =  oapi_swupdate_FindSwUpdateInfoByHandle(hSwUpdate, &pstInfo) ;
	if (bFound == FALSE)
	{
		HxLOG_Error("hSwUpdate is not Found (hSwUpdate : %p)\n", hSwUpdate);
		return ERR_FAIL;
	}
	HLIB_RPC_RegisterNotifyCallback (nRpcHandle, RPC_OBAMA_SWUPDATE_ResultNotifier, NULL, NULL, (HxRPC_Noti_t)oapi_swupdate_EventNotifier);

	s_pfSwUpdateApkNotifier = fnApkNotifier;

	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_SWUPDATE_DownloadStart, "i", hSwUpdate);
	return ERR_OK;
}

HERROR OAPI_SWUPDATE_DownloadStop(HUINT32 hSwUpdate)
{
	OxSwUpdateInfo_t *pstInfo = NULL;
	HBOOL	bFound = FALSE;
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	bFound =  oapi_swupdate_FindSwUpdateInfoByHandle(hSwUpdate, &pstInfo) ;
	if (bFound == FALSE)
	{
		HxLOG_Error("hSwUpdate is not Found (hSwUpdate : %p)\n", hSwUpdate);
		// TODO:
		//OAPI_UnregisterCallback(RPC_OBAMA_SWUPDATE_ResultNotifier, oapi_swupdate_EventNotifier);
		return ERR_FAIL;
	}
	//OAPI_UnregisterCallback(RPC_OBAMA_SWUPDATE_ResultNotifier, oapi_swupdate_EventNotifier);

	//s_pfSwUpdateApkNotifier = NULL;

	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_SWUPDATE_DownloadStop, "i", hSwUpdate);
	oapi_swupdate_Destroy(hSwUpdate);

	return ERR_OK;
}

HERROR OAPI_SWUPDATE_InstallStart(HUINT32 hSwUpdate, OxSwUpdate_Notifier_t fnApkNotifier)
{
	OxSwUpdateInfo_t *pstInfo = NULL;
	HBOOL	bFound = FALSE;
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	bFound =  oapi_swupdate_FindSwUpdateInfoByHandle(hSwUpdate, &pstInfo) ;
	if (bFound == FALSE)
	{
		HxLOG_Error("hSwUpdate is not Found (hSwUpdate : %p)\n", hSwUpdate);
		return ERR_FAIL;
	}
	HLIB_RPC_RegisterNotifyCallback (nRpcHandle, RPC_OBAMA_SWUPDATE_ResultNotifier, NULL, NULL, (HxRPC_Noti_t)oapi_swupdate_EventNotifier);

	s_pfSwUpdateApkNotifier = fnApkNotifier;

	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_SWUPDATE_InstallStart, "i", hSwUpdate);
	return ERR_OK;
}

HERROR OAPI_SWUPDATE_InstallStop(HUINT32 hSwUpdate)
{
	OxSwUpdateInfo_t *pstInfo = NULL;
	HBOOL	bFound = FALSE;
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	bFound =  oapi_swupdate_FindSwUpdateInfoByHandle(hSwUpdate, &pstInfo) ;
	if (bFound == FALSE)
	{
		HxLOG_Error("hSwUpdate is not Found (hSwUpdate : %p)\n", hSwUpdate);
		// TODO:
		//OAPI_UnregisterCallback(RPC_OBAMA_SWUPDATE_ResultNotifier, oapi_swupdate_EventNotifier);
		return ERR_FAIL;
	}
	// TODO:
	//OAPI_UnregisterCallback(RPC_OBAMA_SWUPDATE_ResultNotifier, oapi_swupdate_EventNotifier);

	//s_pfSwUpdateApkNotifier = NULL;

	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_SWUPDATE_InstallStop, "i", hSwUpdate);
	oapi_swupdate_Destroy(hSwUpdate);

	return ERR_OK;
}

HERROR OAPI_SWUPDATE_Cancel(HUINT32 hSwUpdate)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	// TODO:
	//OAPI_UnregisterCallback(RPC_OBAMA_SWUPDATE_ResultNotifier, oapi_swupdate_EventNotifier);

	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_SWUPDATE_Cancel, "i", hSwUpdate);
	oapi_swupdate_Destroy(hSwUpdate);

	return ERR_OK;
}
