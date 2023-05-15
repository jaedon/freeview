/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  om_swupdate.c
	@brief

	Description:  									\n
	Module: TVMANAGER / OAPI Manager				\n

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
#include <hlib.h>
#include <obama_rpcprotocol.h>
#include <oapi.h>
#include <octo_common.h>

//#include <mwc_util.h>

#include <svc_si.h>
#include <svc_sys.h>
#include <svc_pipe.h>
#include <svc_output.h>
#include <svc_av.h>

#include <bus.h>

#include <mgr_common.h>
#include <mgr_action.h>

#include <mgr_swup.h>
#include <mgr_liveota.h>

#include <otl.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	Handle_t				hSession;		// Session Unique Id from Media Control
	HUINT32				ulActionId;		// Action Id
	HUINT16 				usTsUniqId;
} omSwUpdate_Session_t;

typedef struct
{
	DxSwUpdate_Spec_e		eSwupdateSpec;
	HCHAR					szTargetUrl[eDxSWUPDATE_MAX_URL_LENGTH];
	DxSwUpdate_Step_e		aeSteps[eDxSWUPDATE_MAX_STEP];
} omSwUpdate_ConfigureParam;

typedef struct
{
	DxTransponder_t	stTunningInfo;
} omSwUpdate_DetectParam;

typedef struct tagSwupdateAccessInfo
{
	Handle_t				 		hSwupdate;			// hAction or id
	omSwUpdate_Session_t		 	stSession;
	HUINT32					 	ulAccesscode;		// Main Access code : Event ...
	omSwUpdate_ConfigureParam	stConfig;
	omSwUpdate_DetectParam		stDetect;
} omSwUpdate_AccessInfo_t;

typedef struct tagSwupdateObject
{
	Handle_t						hMgrSwupdate;		// mgr 에서 생성한 swupdate handle
	omSwUpdate_AccessInfo_t		stInfo;				// oapi 에서 관리되는 swupdate info
} omSwUpdate_Object_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HUINT32	s_ulSessionId;
STATIC HxList_t	*pstOmSwUpdateInfoList = NULL;		// List of omSwupdate_Object_t

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/

#define ___LOCAL_FUNCTIONS___

static HBOOL	om_swupdate_map_CB_findByType(void *pvUserData, void *pvListData)
{
	omSwUpdate_Object_t		*pstListInfo;
	DxSwUpdate_Spec_e		 eSwUpdateSpec;

	if (NULL == pvListData)
	{
		return FALSE;
	}

	pstListInfo = (omSwUpdate_Object_t *)pvListData;
	eSwUpdateSpec = (DxSwUpdate_Spec_e)pvUserData;

	return (pstListInfo->stInfo.stConfig.eSwupdateSpec == eSwUpdateSpec) ? TRUE : FALSE;
}

static HBOOL	om_swupdate_map_CB_findByHandle(void *pvUserData, void *pvListData)
{
	omSwUpdate_Object_t	*pstListInfo;
	Handle_t			hSwupdate;

	if (NULL == pvListData)
	{
		return FALSE;
	}

	pstListInfo = (omSwUpdate_Object_t *)pvListData;
	hSwupdate = (Handle_t)pvUserData;

	return (pstListInfo->stInfo.hSwupdate == hSwupdate) ? TRUE : FALSE;
}

static HBOOL	om_swupdate_map_CB_FindBySwMgrHandle(void *pvUserData, void *pvListData)
{
	omSwUpdate_Object_t 	*pstListInfo;
	Handle_t			hSwupMgr;

	if (NULL == pvListData)
	{
		return FALSE;
	}

	pstListInfo	= (omSwUpdate_Object_t *)pvListData;
	hSwupMgr	= (Handle_t)pvUserData;

	return (pstListInfo->hMgrSwupdate == hSwupMgr ) ? TRUE : FALSE;
}

static HERROR	om_swupdate_map_FindByHandle(Handle_t hSwupdate, omSwUpdate_Object_t **ppstObject)
{
	HxList_t			*pstFound;

	HxLOG_Debug("ist Num [%d]\n", HLIB_LIST_Length(pstOmSwUpdateInfoList));

	pstFound = HLIB_LIST_FindEx(pstOmSwUpdateInfoList, (const void *)hSwupdate, om_swupdate_map_CB_findByHandle);


	if (NULL == pstFound)
	{
		return ERR_FAIL;
	}

	if (ppstObject)
	{
		*ppstObject = (omSwUpdate_Object_t *)HLIB_LIST_Data(pstFound);
	}

	return ERR_OK;
}

static HERROR	om_swupdate_map_FindByType(DxSwUpdate_Spec_e eSwUpdateSpec, omSwUpdate_Object_t **ppstObject)
{
	HxList_t			*pstFound;

	pstFound = HLIB_LIST_FindEx(pstOmSwUpdateInfoList, (const void *)eSwUpdateSpec, om_swupdate_map_CB_findByType);
	if (NULL == pstFound)
	{
		return ERR_FAIL;
	}

	if (ppstObject)
	{
		*ppstObject = (omSwUpdate_Object_t *)HLIB_LIST_Data(pstFound);
	}

	return ERR_OK;
}

static HERROR	om_swupdate_map_FindByMgrHandle(Handle_t hSwupMgr, omSwUpdate_Object_t **ppstObject)
{
	HxList_t			*pstFound;

	HxLOG_Debug("List Num [%d]\n", HLIB_LIST_Length(pstOmSwUpdateInfoList));

	pstFound = HLIB_LIST_FindEx(pstOmSwUpdateInfoList, (const void *)hSwupMgr, om_swupdate_map_CB_FindBySwMgrHandle);
	if (NULL == pstFound)
	{
		return ERR_FAIL;
	}

	if (ppstObject)
	{
		*ppstObject = (omSwUpdate_Object_t *)HLIB_LIST_Data(pstFound);
	}

	return ERR_OK;
}

static HERROR 	om_swupdate_map_RemoveByMgrHandle(Handle_t hSwupMgr)
{
	HxList_t 			*pstFound;
	omSwUpdate_Object_t 	*pstObj	= NULL;

	pstFound= HLIB_LIST_FindEx(pstOmSwUpdateInfoList, (const void *)hSwupMgr, om_swupdate_map_CB_FindBySwMgrHandle);

	if (NULL == pstFound)
	{
		return ERR_FAIL;
	}

	pstObj	= (omSwUpdate_Object_t *)HLIB_LIST_Data(pstFound);

	if(pstObj)
	{
		pstOmSwUpdateInfoList	= HLIB_LIST_Remove(pstOmSwUpdateInfoList, (const void *)pstObj);
		OxAP_Free(pstObj);
	}

	return ERR_OK;
}



static HERROR om_swupdate_MapCreate(DxSwUpdate_Spec_e eSwUpdateSpec, omSwUpdate_Object_t **ppstObject)
{
	omSwUpdate_Object_t *pstObj;

	pstObj = OxAP_Malloc(sizeof(omSwUpdate_Object_t));
	if (NULL == pstObj)
	{
		HxLOG_Error("NULL == pstObj\n");
		return ERR_FAIL;
	}

	HxSTD_MemSet(pstObj, 0, sizeof(omSwUpdate_Object_t));
	HxSTD_MemSet(&pstObj->stInfo.stSession, 0xFF, sizeof(omSwUpdate_Session_t));

	pstObj->hMgrSwupdate	= HANDLE_NULL;

	pstOmSwUpdateInfoList = HLIB_LIST_Append(pstOmSwUpdateInfoList, (void *)pstObj);

	if (ppstObject)
	{
		*ppstObject = pstObj;
	}

	return ERR_OK;
}

STATIC HERROR om_swupdate_Init(HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	pstOmSwUpdateInfoList = NULL;

	return ERR_OK;
}
STATIC HERROR om_swupdate_UnInit(HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	s_ulSessionId = HxOBJECT_INT(argv[0]);

	return ERR_OK;
}

STATIC HERROR om_swupdate_CreateHandle(HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	HERROR				hError;
	DxSwUpdate_Spec_e 	eSwUpdateSpec;
	HBOOL				bManualUpdate;
	omSwUpdate_Object_t 	*pstObj 	=	NULL;
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;
	DxTransponder_t *pTunningInfo = NULL;

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OBAMA_RpcProtocol_GetRpcHandle failed. \n");
	}

	eSwUpdateSpec	= (DxSwUpdate_Spec_e)HxOBJECT_INT(argv[0]);
	bManualUpdate 	= (HBOOL)HxOBJECT_INT(argv[1]);

	if (bManualUpdate)
	{
		pTunningInfo = (DxTransponder_t*)HxOBJECT_BIN_DATA(argv[2]);
	}

	// TODO: pvUserData에 대한 처리 방법 확인 필요..

	hError = om_swupdate_MapCreate(eSwUpdateSpec, &pstObj);
	if (ERR_OK != hError || NULL == pstObj)
	{
		HxLOG_Error("om_swupdate_MapCreate() Fail : hErr [0x%x] \n", hError);
		HLIB_RPC_Response(nRpcHandle, nClientHandle, nRequestId, ERR_FAIL,"i",  HANDLE_NULL);
		return ERR_FAIL;
	}

	pstObj->stInfo.stConfig.eSwupdateSpec	= eSwUpdateSpec;
	if(bManualUpdate == TRUE)
		HLIB_STD_StrUtf8NCpy(pstObj->stInfo.stConfig.szTargetUrl, "testurl ", eDxSWUPDATE_MAX_URL_LENGTH);
	else
		;

	pstObj->stInfo.stConfig.aeSteps[0] = eDxSWUPDATE_STEP_Config;
	pstObj->stInfo.stConfig.aeSteps[1] = eDxSWUPDATE_STEP_Connect;
	pstObj->stInfo.stConfig.aeSteps[2] = eDxSWUPDATE_STEP_Detect;
	pstObj->stInfo.stConfig.aeSteps[3] = eDxSWUPDATE_STEP_Download;

	// Assign SwUpdate Handle
	pstObj->stInfo.hSwupdate	= (Handle_t)pstObj;

	//set ota spec
	hError = MGR_SWUP_Start();
	if(hError == ERR_FAIL)
	{
		HxLOG_Error("MGR_SWUP_Start error \n");
		return ERR_FAIL;
	}

	hError = MGR_SWUP_SetSpec(eSwUpdateSpec);
	if(hError == ERR_OK)
	{
		HLIB_RPC_Response(nRpcHandle, nClientHandle, nRequestId, ERR_OK, "i", pstObj->stInfo.hSwupdate);

		pstObj->hMgrSwupdate = MGR_SWUP_Create(pTunningInfo, 0);
		if(pstObj->hMgrSwupdate != HANDLE_NULL)
		{
			MGR_SWUP_InitState(pstObj->hMgrSwupdate, MGR_SWUP_GetStartUpdateSource(eDxBAT_TYPE_NONE),0,0);

			return ERR_OK;
		}
		else
		{
			return ERR_FAIL;
		}
	}
	else
	{
		return ERR_FAIL;
	}
}
STATIC HERROR om_swupdate_DetectStart(HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	HERROR	hError;
	Handle_t hSwUpdate;
	omSwUpdate_Object_t		*pstObj		= 	NULL;


	hSwUpdate = HxOBJECT_INT(argv[0]);
	hError = om_swupdate_map_FindByHandle(hSwUpdate, &pstObj);
	if(hError == ERR_OK)
	{
		HxLOG_Debug("send eMEVT_SWUP_CONNECT_START ! \n");
		MGR_SWUP_PostMessage(pstObj->hMgrSwupdate, eMEVT_SWUP_CONNECT_START, (Handle_t) 0 , 0, 0, 0);
	}
	else
	{
		HxLOG_Error("cannot find sw update mgr handle\n");
		return ERR_FAIL;
	}

	return hError;
}

STATIC HERROR om_swupdate_DetectStop(HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	Handle_t hSwUpdate;
	omSwUpdate_Object_t		*pstObj		= 	NULL;


	hSwUpdate = HxOBJECT_INT(argv[0]);
	if (om_swupdate_map_FindByHandle(hSwUpdate, &pstObj) == ERR_OK)
	{
		HxLOG_Debug("send eMEVT_SWUP_DETECT_STOP ! \n");
		MGR_SWUP_PostMessage(pstObj->hMgrSwupdate, eMEVT_SWUP_DETECT_STOP, (Handle_t) 0 , 0, 0, 0);
	}
	else
	{
		HxLOG_Error("cannot find sw update mgr handle\n");
		return ERR_FAIL;
	}
	return ERR_OK;
}
STATIC HERROR om_swupdate_DownloadStart(HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	Handle_t 					hSwUpdate;
	omSwUpdate_Object_t		*pstObj = NULL;

	hSwUpdate = HxOBJECT_INT(argv[0]);

	if (om_swupdate_map_FindByHandle(hSwUpdate, &pstObj) == ERR_OK)
	{
		HERROR hErr = ERR_FAIL;
		SvcSwUpdate_DetectResult_t		*pstDetectResult = NULL;

		pstDetectResult= (SvcSwUpdate_DetectResult_t*)HLIB_STD_MemCalloc(sizeof(SvcSwUpdate_DetectResult_t));
		if (pstDetectResult == NULL)
		{
			HxLOG_Error("Memory alloc fail!!\n");
			return ERR_FAIL;
		}

		HxLOG_Debug("send eMEVT_SWUP_DOWNLOAD_START ! \n");
		hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_OTA_DETECT_INFO, (HUINT32*)pstDetectResult, sizeof(SvcSwUpdate_DetectResult_t)) ;
		if (hErr != ERR_OK)
		{
			HxLOG_Error("DB_PARAM_SetItem() return Error : result [0x%x]\n", hErr);

			if(pstDetectResult != NULL)
			{
				HLIB_STD_MemFree(pstDetectResult);
			}

			return ERR_FAIL;
		}
		else
		{
			hErr = MGR_SWUP_SetDetectInfo(pstObj->hMgrSwupdate, pstDetectResult);
			if (hErr == ERR_FAIL)
			{
				HxLOG_Error("Failed SetDetectInfo for download state!!\n\n");

				if(pstDetectResult != NULL)
				{
					HLIB_STD_MemFree(pstDetectResult);
				}

				return ERR_FAIL;
			}
		}
		MGR_SWUP_PostMessage(pstObj->hMgrSwupdate, eMEVT_SWUP_DOWNLOAD_START, (Handle_t) 0 , 0, 0, 0);

		if(pstDetectResult != NULL)
		{
			HLIB_STD_MemFree(pstDetectResult);
		}
	}
	else
	{
		HxLOG_Error("cannot find sw update mgr handle\n");
		return ERR_FAIL;
	}
	return ERR_OK;
}
STATIC HERROR om_swupdate_DownloadStop(HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	Handle_t hSwUpdate;
	omSwUpdate_Object_t		*pstObj		= 	NULL;

	hSwUpdate = HxOBJECT_INT(argv[0]);
	if (om_swupdate_map_FindByHandle(hSwUpdate, &pstObj) == ERR_OK)
	{
		HxLOG_Debug("send eMEVT_SWUP_DOWNLOAD_STOP ! \n");
		MGR_SWUP_PostMessage(pstObj->hMgrSwupdate, eMEVT_SWUP_DOWNLOAD_STOP,(Handle_t) 0 , 0, 0, 0);
	}
	else
	{
		HxLOG_Error("cannot find sw update mgr handle\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}
STATIC HERROR om_swupdate_InstallStart(HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	Handle_t hSwUpdate;
	omSwUpdate_Object_t		*pstObj		= 	NULL;

	hSwUpdate = HxOBJECT_INT(argv[0]);
	if (om_swupdate_map_FindByHandle(hSwUpdate, &pstObj) == ERR_OK)
	{
		HxLOG_Debug("send eMEVT_SWUP_INSTALL_START ! \n");
		MGR_SWUP_PostMessage(pstObj->hMgrSwupdate, eMEVT_SWUP_INSTALL_START, (Handle_t) 0 , 0, 0, 0);
	}
	else
	{
		HxLOG_Error("cannot find sw update mgr handle\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}
STATIC HERROR om_swupdate_InstallStop(HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	Handle_t hSwUpdate;
	omSwUpdate_Object_t		*pstObj		= 	NULL;

	hSwUpdate = HxOBJECT_INT(argv[0]);
	if (om_swupdate_map_FindByHandle(hSwUpdate, &pstObj) == ERR_OK)
	{
		HxLOG_Debug("send eMEVT_SWUP_INSTALL_STOP ! \n");
		MGR_SWUP_PostMessage(pstObj->hMgrSwupdate, eMEVT_SWUP_INSTALL_STOP, (Handle_t) 0 , 0, 0, 0);
	}
	else
	{
		HxLOG_Error("cannot find sw update mgr handle\n");
		return ERR_FAIL;
	}
	return ERR_OK;
}
STATIC HERROR om_swupdate_Cancel(HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	Handle_t 					hSwUpdate;
	omSwUpdate_Object_t		*pstObj	= NULL;

	HxLOG_Trace();

	hSwUpdate = HxOBJECT_INT(argv[0]);

	if (om_swupdate_map_FindByHandle(hSwUpdate, &pstObj) == ERR_OK)
	{
		MGR_SWUP_PostMessage(pstObj->hMgrSwupdate, eMEVT_SWUP_CANCEL, (Handle_t) 0 , 0, 0, 0);
	}
	else
	{
		HxLOG_Error("cannot find sw update mgr handle\n");
		return ERR_FAIL;
	}
	return ERR_OK;
}

#define _____RPC_INITIALIZATION_____
STATIC HERROR om_swupdate_InitRpcFunctions (void)
{
	HERROR			hError;
	HINT32			nRpcHandle = 0;


	hError = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if(hError == ERR_OK)
	{
		// APPKIT -> OBAMA
		hError = HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_SWUPDATE_Iint, NULL, (HxRPC_Func_t)om_swupdate_Init, NULL,
								"Activate the output.audio module: (void)");
		if(hError != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterMethod (nRpcHandle, %s) returned error(%d)\n", RPC_OAPI_SWUPDATE_Iint, hError);
		}

		hError = HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_SWUPDATE_UnInit, "ii", (HxRPC_Func_t)om_swupdate_UnInit, NULL,
								"Set audio master mute: (HBOOL bMute)");
		if(hError != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterMethod (nRpcHandle, %s) returned error(%d)\n", RPC_OAPI_SWUPDATE_UnInit, hError);
		}

		hError = HLIB_RPC_RegisterRequest (nRpcHandle, RPC_OAPI_SWUPDATE_CreateHandle, "ii", (HxRPC_Func_t)om_swupdate_CreateHandle, NULL,
								"SwUpdate Create Handle : (DxSwUpdate_Spec_e eSwUpdateSpec, HBOOL bManualUpdate) ");
		if(hError != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterRequest (nRpcHandle, %s) returned error(%d)\n", RPC_OAPI_SWUPDATE_CreateHandle, hError);
		}

		hError = HLIB_RPC_RegisterRequest (nRpcHandle, RPC_OAPI_SWUPDATE_CreateManualHandle, "iib", (HxRPC_Func_t)om_swupdate_CreateHandle, NULL,
								"SwUpdate Manual Create Handle : (DxSwUpdate_Spec_e eSwUpdateSpec, HBOOL bManualUpdate, void* pvTunningInfo) ");
		if(hError != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterRequest (nRpcHandle, %s) returned error(%d)\n", RPC_OAPI_SWUPDATE_CreateManualHandle, hError);
		}

		hError = HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_SWUPDATE_DetectStart, "i", (HxRPC_Func_t)om_swupdate_DetectStart, NULL,
								"SwUpdate Detect Start: (Handle_t hSwUpdate)");
		if(hError != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterMethod (nRpcHandle, %s) returned error(%d)\n", RPC_OAPI_SWUPDATE_DetectStart, hError);
		}

		hError = HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_SWUPDATE_DetectStop, "i", (HxRPC_Func_t)om_swupdate_DetectStop, NULL,
								"SwUpdate Detect Stop: (Handle_t hSwUpdate)");
		if(hError != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterMethod (nRpcHandle, %s) returned error(%d)\n", RPC_OAPI_SWUPDATE_DetectStop, hError);
		}

		hError = HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_SWUPDATE_DownloadStart, "i", (HxRPC_Func_t)om_swupdate_DownloadStart, NULL,
								"SwUpdate Download Start: (Handle_t hSwUpdate)");
		if(hError != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterMethod (nRpcHandle, %s) returned error(%d)\n", RPC_OAPI_SWUPDATE_DownloadStart, hError);
		}

		hError = HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_SWUPDATE_DownloadStop, "i", (HxRPC_Func_t)om_swupdate_DownloadStop, NULL,
								"SwUpdate Download Stop: (Handle_t hSwUpdate)");
		if(hError != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterMethod (nRpcHandle, %s) returned error(%d)\n", RPC_OAPI_SWUPDATE_DownloadStop, hError);
		}

		hError = HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_SWUPDATE_InstallStart, "i", (HxRPC_Func_t)om_swupdate_InstallStart, NULL,
								"SwUpdate Install Start: (Handle_t hSwUpdate)");
		if(hError != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterMethod (nRpcHandle, %s) returned error(%d)\n", RPC_OAPI_SWUPDATE_InstallStart, hError);
		}

		hError = HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_SWUPDATE_InstallStop, "i", (HxRPC_Func_t)om_swupdate_InstallStop, NULL,
								"SwUpdate Install Stop: (Handle_t hSwUpdate)");
		if(hError != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterMethod (nRpcHandle, %s) returned error(%d)\n", RPC_OAPI_SWUPDATE_InstallStop, hError);
		}

		hError = HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_SWUPDATE_Cancel, "i", (HxRPC_Func_t)om_swupdate_Cancel, NULL,
								"SwUpdate Cancel: (Handle_t hSwUpdate)");
		if(hError != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterMethod (nRpcHandle, %s) returned error(%d)\n", RPC_OAPI_SWUPDATE_Cancel, hError);
		}

		// OBAMA -> APPKIT
		hError = HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_SWUPDATE_ResultNotifier, NULL,
								"Audio output parameters and status changed.\n"
								"\t   - (OxOutputAudio_Cache_t *pstOutputAudioStatus)\n"
								);
		if(hError != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterMethod (nRpcHandle, %s) returned error(%d)\n", RPC_OBAMA_SWUPDATE_ResultNotifier, hError);
		}

	}


	return ERR_OK;
}

#define _____PROCEDURE_____
STATIC BUS_Result_t proc_om_swupdate(HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t		eRes = MESSAGE_PASS;
	HERROR			hError = ERR_FAIL;
	Handle_t			hMgrSwupdate;
	omSwUpdate_Object_t	*pstObject;
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;

	DxSwUpdate_EventData_t	stSwUpdateEvent;

	HxSTD_memset(&stSwUpdateEvent, 0x00, sizeof(DxSwUpdate_EventData_t));

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);

	switch(message)
	{
	case eMEVT_DN_SWUP_CONNECT_SUCCESS:
		HxLOG_Print("eMEVT_DN_SWUP_CONNECT_SUCCESS\n\n");
		eRes = MESSAGE_BREAK;
		break;

	case eMEVT_DN_SWUP_CONNECT_FAIL:
		HxLOG_Print("eMEVT_DN_SWUP_CONNECT_FAIL\n\n");
		eRes = MESSAGE_BREAK;
		break;

	case eMEVT_DN_SWUP_DETECT_SUCCESS:
		HxLOG_Print("eMEVT_DN_SWUP_DETECT_SUCCESS\n\n");

		if (nRpcHandle)
		{
			SvcSwUpdate_DetectResult_t	*pstDetectedInform = NULL;
			HCHAR					szCurrentVersion[32], szNewSWVersion[32];
			HUINT32					ulSystemVersion;

			hMgrSwupdate= (Handle_t)p1;

			// if LIVE OTA..
			if (hMgrSwupdate == LIVE_SWUPDATE_HANDLE)
			{
				if (p2 == eDxSWUPDATE_SIGNAL_MULTI)
				{
					stSwUpdateEvent.eEvent			=	eDxSWUPDATE_EVENT_DetectSuccessInLive;
					stSwUpdateEvent.eSignalType		=	eDxSWUPDATE_SIGNAL_MULTI;
					stSwUpdateEvent.eMethodType		=	eDxSWUPDATE_METHOD_LIVE;
					stSwUpdateEvent.eState			=	eDxSWUPDATE_STATE_DETECT_SUCCESSED;

					HLIB_RPC_NotifyWithoutSignatureCheck(nRpcHandle, RPC_OBAMA_SWUPDATE_ResultNotifier, "b", &stSwUpdateEvent, sizeof(DxSwUpdate_EventData_t));
				}
				else	// eDxSWUPDATE_SIGNAL_SINGLE
				{


					pstDetectedInform = (SvcSwUpdate_DetectResult_t*)p3;
					if(pstDetectedInform == NULL)
					{
						HxLOG_Error(" pstDetectedInform is NULL \n");
						eRes = MESSAGE_BREAK;
						break;
					}

					SVC_SYS_GetSystemVersion(eVERSION_TYPE_APPL, &ulSystemVersion);

					SVC_SYS_MakeSystemVerString(eVERSION_TYPE_APPL, ulSystemVersion, szCurrentVersion);
					SVC_SYS_MakeSystemVerString(eVERSION_TYPE_APPL, pstDetectedInform->ulDataVersion, szNewSWVersion);

					if(pstDetectedInform->eSource == eDxSWUPDATE_SOURCE_Rf)
					{

					}

					stSwUpdateEvent.eSourceType 	=	pstDetectedInform->eSource;
					stSwUpdateEvent.eEvent			=	eDxSWUPDATE_EVENT_DetectSuccessInLive;
					stSwUpdateEvent.eSignalType		=	eDxSWUPDATE_SIGNAL_SINGLE;
					stSwUpdateEvent.eMethodType		=	eDxSWUPDATE_METHOD_LIVE;
					stSwUpdateEvent.eState			=	eDxSWUPDATE_STATE_DETECT_SUCCESSED;
					HLIB_STD_StrUtf8NCpy(stSwUpdateEvent.unEventData.stDetectEvent.szCurrentVer, szCurrentVersion, eDxSWUPDATE_MAX_STRING);
					HLIB_STD_StrUtf8NCpy(stSwUpdateEvent.unEventData.stDetectEvent.szFoundVer, szNewSWVersion, eDxSWUPDATE_MAX_STRING);

					HLIB_RPC_NotifyWithoutSignatureCheck(nRpcHandle, RPC_OBAMA_SWUPDATE_ResultNotifier, "b", &stSwUpdateEvent, sizeof(DxSwUpdate_EventData_t));
				}
			}
			else		// is MENU OTA..
			{
				DxSwUpdate_Spec_e		eSwUpdateSpec;
				SvcSi_SwupSignal_t			stOtaInfo;

				hError = om_swupdate_map_FindByMgrHandle(hMgrSwupdate, &pstObject);
				if(hError != ERR_OK)
				{
					HxLOG_Error(" pstObject Error  (hMgrSwupdate : 0x%08x)!!\n", hMgrSwupdate);
					eRes = MESSAGE_BREAK;
					break;
				}

				pstDetectedInform = (SvcSwUpdate_DetectResult_t*)p3;
				if(pstDetectedInform == NULL)
				{
					HxLOG_Error(" pstDetectedInform is NULL \n");
					eRes = MESSAGE_BREAK;
					break;
				}

				MGR_SWUP_GetSpec(&eSwUpdateSpec);

				// **   set common event data   ** //
				stSwUpdateEvent.eSourceType 	=	pstDetectedInform->eSource;
				stSwUpdateEvent.eEvent			=	eDxSWUPDATE_EVENT_DetectSuccess;
				stSwUpdateEvent.eSignalType		=	eDxSWUPDATE_SIGNAL_SINGLE;
				stSwUpdateEvent.eMethodType		=	eDxSWUPDATE_METHOD_MENU;
				stSwUpdateEvent.eState			=	eDxSWUPDATE_STATE_DETECT_SUCCESSED;
				// ** end set common event data ** //

			#if defined(CONFIG_OTA_IRDETO)
				if(eSwUpdateSpec == eDxSwUpdate_SPEC_IRDETO)
				{
					HERROR				hErr;
					DbSvc_TsInfo_t		stTsInfo;
					HINT32				i32dnTimeInSec = 0;

					if (MGR_SWUPINFO_IsOtaFound (FALSE) == TRUE)
					{
						hErr = MGR_SWUPINFO_GetSearchedInfo (&stTsInfo, sizeof(SvcSi_SwupSignal_t), (void *)&stOtaInfo);
						if (hErr == ERR_OK)
						{
						#if !defined(CONFIG_OTA_IRDETO_LOADER_V5)
							i32dnTimeInSec = stOtaInfo.stDownloadTime.ucHour * 60 * 60 + stOtaInfo.stDownloadTime.ucMinute * 60 + stOtaInfo.stDownloadTime.ucSecond;
							HxLOG_Debug("current ucLoadSeqNumber %d\n", p2);
							HxLOG_Debug("ucTableId %d\n", stOtaInfo.ucTableId);
							HxLOG_Debug("stDownloadTime %d Seconds\n", i32dnTimeInSec);
							HxSTD_PrintToStrN (szCurrentVersion, 32, "%d", p2);
							HxSTD_PrintToStrN (szNewSWVersion, 32, "%d", stOtaInfo.ucLoadSeqNumber);

							stSwUpdateEvent.unExEventData.stExSwupIrdetoData.ucLoadSeqNumber = p2;
							stSwUpdateEvent.unExEventData.stExSwupIrdetoData.ucTableId = stOtaInfo.ucTableId;
							stSwUpdateEvent.unExEventData.stExSwupIrdetoData.nDnTimeInSec = i32dnTimeInSec;
						#endif
							HLIB_STD_StrUtf8NCpy(stSwUpdateEvent.unEventData.stDetectEvent.szCurrentVer, szCurrentVersion, eDxSWUPDATE_MAX_STRING);
							HLIB_STD_StrUtf8NCpy(stSwUpdateEvent.unEventData.stDetectEvent.szFoundVer, szNewSWVersion, eDxSWUPDATE_MAX_STRING);
						}
					}
				}
				else
			#endif
				{
					SVC_SYS_MakeSystemVerString(eVERSION_TYPE_APPL, p2, szCurrentVersion);
					SVC_SYS_MakeSystemVerString(eVERSION_TYPE_APPL, pstDetectedInform->ulDataVersion, szNewSWVersion);

					HLIB_STD_StrUtf8NCpy(stSwUpdateEvent.unEventData.stDetectEvent.szCurrentVer, szCurrentVersion, eDxSWUPDATE_MAX_STRING);
					HLIB_STD_StrUtf8NCpy(stSwUpdateEvent.unEventData.stDetectEvent.szFoundVer, szNewSWVersion, eDxSWUPDATE_MAX_STRING);

					if(pstDetectedInform->eSource == eDxSWUPDATE_SOURCE_Rf)
					{
						hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_OTA_OTAINFO, (HUINT32 *)&stOtaInfo, sizeof(SvcSi_SwupSignal_t));
						if(hErr == ERR_OK)
						{
							stSwUpdateEvent.unExEventData.stExSwupTypeData.ucSwupType = stOtaInfo.ucSwupType;
							if(stOtaInfo.ucSwupType == SvcSi_SWUP_TYPE_BURST_USER_SELECT || stOtaInfo.ucSwupType == SvcSi_SWUP_TYPE_BURST_FORCED)
							{
								SvcSi_SwupSchedule_t	stSchedule;

								hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_OTA_SCHEDULEINFO, (HUINT32 *)&stSchedule, (HUINT32)sizeof(SvcSi_SwupSchedule_t));

								if(hErr == ERR_OK)
								{
									stSwUpdateEvent.unExEventData.stExSwupTypeData.ulStartTime = stSchedule.startDateTime[0];
									stSwUpdateEvent.unExEventData.stExSwupTypeData.ulEndTime = stSchedule.endDateTime[0];
								}
							}
						}
					}

				}
				HLIB_RPC_NotifyWithoutSignatureCheck(nRpcHandle, RPC_OBAMA_SWUPDATE_ResultNotifier, "b", &stSwUpdateEvent, sizeof(DxSwUpdate_EventData_t));
			}
		}
		else
		{
			HxLOG_Error(" nRpcHandle Error (nRpcHandle : %d)!!\n", nRpcHandle);
		}

		if((SvcSwUpdate_DetectResult_t	*)p3 != NULL)
			HLIB_STD_MemFree((SvcSwUpdate_DetectResult_t	*)p3);

		eRes = MESSAGE_BREAK;

		break;

	case eMEVT_DN_SWUP_DETECT_FAIL:
		HxLOG_Print("eMEVT_DN_SWUP_DETECT_FAIL\n\n");
		{
			stSwUpdateEvent.eEvent			=	eDxSWUPDATE_EVENT_DetectFail;

			if (p3 == eSWUP_FAILREASON_Latest_Version)
			{
				HxLOG_Warning("Fail Reason : eSWUP_FAILREASON_Latest_Version\n");
				stSwUpdateEvent.eState			=	eDxSWUPDATE_STATE_DETECT_FAIL_LATEST_VERSION;
			}
			else
			{
				stSwUpdateEvent.eState			=	eDxSWUPDATE_STATE_DETECT_FAIL;
			}

			HLIB_RPC_NotifyWithoutSignatureCheck(nRpcHandle, RPC_OBAMA_SWUPDATE_ResultNotifier, "b", &stSwUpdateEvent, sizeof(DxSwUpdate_EventData_t));

			eRes = MESSAGE_BREAK;
		}
		break;

	case eMEVT_DN_SWUP_DOWNLOAD_SUCCESS:
		HxLOG_Print("eMEVT_DN_SWUP_DOWNLOAD_SUCCESS\n\n");

		if (nRpcHandle)
		{
			stSwUpdateEvent.eEvent			=	eDxSWUPDATE_EVENT_DownloadSuccess;
			stSwUpdateEvent.eState			=	eDxSWUPDATE_STATE_DOWNLOAD_SUCCESSED;

			HLIB_RPC_NotifyWithoutSignatureCheck(nRpcHandle, RPC_OBAMA_SWUPDATE_ResultNotifier, "b", &stSwUpdateEvent, sizeof(DxSwUpdate_EventData_t));

		}
		eRes = MESSAGE_BREAK;

		break;

	case eMEVT_DN_SWUP_DOWNLOAD_FAIL:
		HxLOG_Print("eMEVT_DN_SWUP_DOWNLOAD_FAIL\n\n");

		if (nRpcHandle)
		{
			stSwUpdateEvent.eEvent			=	eDxSWUPDATE_EVENT_DownloadFail;
			stSwUpdateEvent.eState			=	eDxSWUPDATE_STATE_DOWNLOAD_FAIL;
			stSwUpdateEvent.unEventData.stProgressEvent.nProgress	=	-1;
			stSwUpdateEvent.unEventData.stProgressEvent.nProgressMax=	-1;

			HLIB_RPC_NotifyWithoutSignatureCheck(nRpcHandle, RPC_OBAMA_SWUPDATE_ResultNotifier, "b", &stSwUpdateEvent, sizeof(DxSwUpdate_EventData_t));

		}
		eRes = MESSAGE_BREAK;
		break;

	case eMEVT_DN_SWUP_DOWNLOAD_PROGRESS:
		HxLOG_Print("eMEVT_DN_SWUP_DOWNLOAD_PROGRESS - swHandle(0x%X), [ %ld : %ld ]\n\n", (Handle_t)p1, (HUINT32)p2, (HUINT32)p3);

		if (nRpcHandle)
		{
			stSwUpdateEvent.eEvent			=	eDxSWUPDATE_EVENT_DownloadProgress;
			stSwUpdateEvent.eState			=	eDxSWUPDATE_STATE_DOWNLOAD_PROGRESS;
			stSwUpdateEvent.unEventData.stProgressEvent.nProgress	=	(HINT32)p2/2;
			stSwUpdateEvent.unEventData.stProgressEvent.nProgressMax=	(HINT32)p3;

			HLIB_RPC_NotifyWithoutSignatureCheck(nRpcHandle, RPC_OBAMA_SWUPDATE_ResultNotifier, "b", &stSwUpdateEvent, sizeof(DxSwUpdate_EventData_t));

		}

		eRes = MESSAGE_BREAK;
		break;

	case eMEVT_DN_SWUP_INSTALL_SUCCESS:
		HxLOG_Print("eMEVT_DN_SWUP_INSTALL_SUCCESS\n\n");

		if (nRpcHandle)
		{
			stSwUpdateEvent.eEvent			=	eDxSWUPDATE_EVENT_InstallSuccess;
			stSwUpdateEvent.eState			=	eDxSWUPDATE_STATE_INSTALL_SUCCESSED;
			stSwUpdateEvent.unEventData.stProgressEvent.nProgress	=	100;
			stSwUpdateEvent.unEventData.stProgressEvent.nProgressMax=	100;

			HLIB_RPC_NotifyWithoutSignatureCheck(nRpcHandle, RPC_OBAMA_SWUPDATE_ResultNotifier, "b", &stSwUpdateEvent, sizeof(DxSwUpdate_EventData_t));

		}
		eRes = MESSAGE_BREAK;
		break;

	case eMEVT_DN_SWUP_INSTALL_FAIL:
		HxLOG_Print("eMEVT_DN_SWUP_INSTALL_FAIL\n\n");

		if (nRpcHandle)
		{
			stSwUpdateEvent.eEvent			=	eDxSWUPDATE_EVENT_InstallFail;
			stSwUpdateEvent.eState			=	eDxSWUPDATE_STATE_INSTALL_FAIL;
			stSwUpdateEvent.unEventData.stProgressEvent.nProgress	=	-1;
			stSwUpdateEvent.unEventData.stProgressEvent.nProgressMax=	-1;

			HLIB_RPC_NotifyWithoutSignatureCheck(nRpcHandle, RPC_OBAMA_SWUPDATE_ResultNotifier, "b", &stSwUpdateEvent, sizeof(DxSwUpdate_EventData_t));

		}

		eRes = MESSAGE_BREAK;
		break;

	case eMEVT_DN_SWUP_INSTALL_PROGRESS:
		HxLOG_Print("eMEVT_DN_SWUP_INSTALL_PROGRESS - swHandle(0x%X), [ %ld : %ld ]\n\n", (Handle_t)p1, (HUINT32)p2, (HUINT32)p3);

		if (nRpcHandle)
		{
			stSwUpdateEvent.eEvent			=	eDxSWUPDATE_EVENT_InstallProgress;
			stSwUpdateEvent.eState			=	eDxSWUPDATE_STATE_INSTALL_PROGRESS;
			stSwUpdateEvent.unEventData.stProgressEvent.nProgress	=	((HINT32)p2/2) + ((HINT32)p3/2);
			stSwUpdateEvent.unEventData.stProgressEvent.nProgressMax=	(HINT32)p3;

			HLIB_RPC_NotifyWithoutSignatureCheck(nRpcHandle, RPC_OBAMA_SWUPDATE_ResultNotifier, "b", &stSwUpdateEvent, sizeof(DxSwUpdate_EventData_t));

		}
		eRes = MESSAGE_BREAK;
		break;


	case eMEVT_DN_SWUP_ERROR:
	case eMEVT_DN_SWUP_FINISH:
		if(eMEVT_DN_SWUP_ERROR == message)	{ HxLOG_Print("eMEVT_DN_SWUP_ERROR\n\n"); }
		if(eMEVT_DN_SWUP_FINISH == message)	{ HxLOG_Print("eMEVT_DN_SWUP_FINISH\n\n"); }

		{
			hMgrSwupdate	= (Handle_t)p1;

			if (om_swupdate_map_RemoveByMgrHandle(hMgrSwupdate) == ERR_OK)
			{
				stSwUpdateEvent.eEvent			=	eDxSWUPDATE_EVENT_Finish;

				HLIB_RPC_NotifyWithoutSignatureCheck(nRpcHandle, RPC_OBAMA_SWUPDATE_ResultNotifier, "b", &stSwUpdateEvent, sizeof(DxSwUpdate_EventData_t));
			}
		}
		return MESSAGE_PASS;

	default:
		break;
	}

	return eRes;
}

#define ___GLOBAL_FUNCTIONS___

HERROR OM_SWUPDATE_Init(void)
{
	HERROR	hError;

	HxLOG_Debug("\n");

	// Create event handler proc
	BUS_MGR_Create ((HINT8*)"proc_om_swupdate", APP_ACTION_PRIORITY, proc_om_swupdate, 0, 0, 0, 0);

	hError = om_swupdate_InitRpcFunctions();
	if(hError != ERR_OK)
	{
		HxLOG_Error("\tom_swupdate_InitRpcFunctions returned error(%d)\n", hError);
	}

	return ERR_OK;
}


HUINT32	OM_SWUPDATE_GetSwupdateNum(void)
{
	if (pstOmSwUpdateInfoList)
		return HLIB_LIST_Length(pstOmSwUpdateInfoList);
	else
		return 0;
}

// index 별 S/W Update 객체의 핸들. 실재 S/W 를 수행하는 S/W MGR 의 Handle
Handle_t 	OM_SWUPDATE_GetSwupdateHandle(HUINT32 index)
{
	omSwUpdate_Object_t	*pSwupdateData	=	NULL;
	HxList_t			*pstFound		=	NULL;

	if (NULL == pstOmSwUpdateInfoList)
	{
		return (Handle_t)NULL;
	}

	pstFound	= HLIB_LIST_GetListItem(pstOmSwUpdateInfoList, index);

	if (NULL == pstFound)
	{
		return (Handle_t)NULL;
	}

	pSwupdateData = (omSwUpdate_Object_t *)HLIB_LIST_Data(pstFound);

	if (pSwupdateData)
	{
		return pSwupdateData->hMgrSwupdate;
	}

	return (Handle_t)NULL;
}



