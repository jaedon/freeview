/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  hapi.h
	@brief

	Description:  									\n
	Module: HaMa Control Client API					\n

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
#include	<vkernel.h>
#include	<hlib.h>

#include	"hapi_int.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HAPI_t	*pstHapi;
	HCHAR	*pszSignalName;
	HBOOL	bIsSystemSignal;

	HxList_t	*pstList;
} HAPI_NotiList_t;

typedef struct
{
	void	*pvUserData;
	HxRPC_Noti_t	cbNotifier;
} HAPI_Noti_t;

typedef struct
{
	HUINT32		ulWinId;
	HUINT32		ulKeyEvent;
//	HxList_t	*pstList;
} HAPI_KeyHandler_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
static CBHAPIMonitorHandler s_pfHapiSysMonitor = NULL;

/*******************************************************************/
/********************      Functions     ***************************/
/*******************************************************************/
static void		hapi_FreeSignalNotiList(void *pvData)
{
	HAPI_NotiList_t	*pstNotiList = (HAPI_NotiList_t *)pvData;

	if (pstNotiList->pszSignalName)
		HAMA_Free(pstNotiList->pszSignalName);
	HLIB_LIST_RemoveAllFunc(pstNotiList->pstList, HAMA_Free_CB);

	HAMA_Free(pstNotiList);
}

static HBOOL	hapi_compareNotifier(void *pvUser, void *pvList)
{
	HxRPC_Noti_t	cbRpcNotifier = (HxRPC_Noti_t)pvUser;
	HAPI_Noti_t		*pstNotiInfo = (HAPI_Noti_t*)pvList;

	if (cbRpcNotifier == pstNotiInfo->cbNotifier)
		return TRUE;

	return FALSE;
}

static HAPI_KeyHandler_t*	hapi_GetKeyHandlerById(HAPI_t *hapi, HxList_t *pstList, HUINT32 ulWinId)
{
	HxList_t	*plist = pstList;
	HAPI_KeyHandler_t	*pInfo;

	while (plist)
	{
		pInfo = (HAPI_KeyHandler_t*)HLIB_LIST_Data(plist);
		if (pInfo->ulWinId == ulWinId)
			return pInfo;
		plist = plist->next;
	}

	pInfo = (HAPI_KeyHandler_t*)HAMA_Calloc(sizeof(HAPI_KeyHandler_t));
	pInfo->ulWinId = ulWinId;
	HLIB_MSG_Create(16, sizeof(HInputEvent_t), "hapikey", (HULONG*)&pInfo->ulKeyEvent, eHLIB_SUSPENDTYPE_FIFO);

	hapi->pstKeyHandlerList = HLIB_LIST_Append(hapi->pstKeyHandlerList, (void*)pInfo);

	return NULL;
}

//	apArgv[0] : HWINID_e
//	apArgv[1] : HInputEvent_t
static HERROR	hapi_onKeyPressed(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HAPI_t	*hapi;
	HInputEvent_t	*pstInputEvent;
	HAPI_KeyHandler_t	*pstKeyHandler;

	HAMA_ASSERT(nArgc == 2);

	hapi = hapi_getInstance();
	pstInputEvent = HxOBJECT_BIN_DATA(apArgv[1]);

	pstKeyHandler = hapi_GetKeyHandlerById(hapi, hapi->pstKeyHandlerList, HxOBJECT_INT(apArgv[0]));
	if (pstKeyHandler == NULL)
	{
		pstKeyHandler = (HAPI_KeyHandler_t*)HAMA_Calloc(sizeof(HAPI_KeyHandler_t));
		pstKeyHandler->ulWinId = HxOBJECT_INT(apArgv[0]);
		HLIB_MSG_Create(16, sizeof(HInputEvent_t), "hapikey", (HULONG*)&pstKeyHandler->ulKeyEvent, eHLIB_SUSPENDTYPE_FIFO);

		hapi->pstKeyHandlerList = HLIB_LIST_Append(hapi->pstKeyHandlerList, (void*)pstKeyHandler);
	}

	if (HLIB_MSG_Send(pstKeyHandler->ulKeyEvent, (void*)pstInputEvent, sizeof(HInputEvent_t)) != ERR_OK)
	{
		HxLOG_Error("Key event buffer is full with window id (%d)\n", pstKeyHandler->ulWinId);
	}

	hapi_releaseInstance(hapi);

	return ERR_OK;
}

static HERROR	hapi_onSysMonitor(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HSystemMointorInfo_t *pInfo;

	HAMA_ASSERT(nArgc == 1);

	pInfo = (HSystemMointorInfo_t*)HxOBJECT_BIN_DATA(apArgv[0]);
	if(s_pfHapiSysMonitor)
	{
		s_pfHapiSysMonitor(pInfo); // rpc thread buffer...
	}
	return ERR_OK;
}


static HAPI_NotiList_t*	hapi_cloneNotiList(HAPI_NotiList_t *pstNotiList)
{
	HAPI_Noti_t		*pstNotiInfo;
	HAPI_NotiList_t	*pstResult;
	HxList_t		*list;

	pstResult = (HAPI_NotiList_t*)HAMA_Calloc(sizeof(HAPI_NotiList_t));
	pstResult->pstHapi         = pstNotiList->pstHapi;
	pstResult->pszSignalName   = HAMA_StrDup(pstNotiList->pszSignalName);
	pstResult->bIsSystemSignal = pstNotiList->bIsSystemSignal;

	list = pstNotiList->pstList;
	while (list)
	{
		pstNotiInfo = (HAPI_Noti_t*)HLIB_LIST_Data(list);
		pstResult->pstList = HLIB_LIST_Append(pstResult->pstList, (void*)HAMA_MemDup(pstNotiInfo, sizeof(HAPI_Noti_t)));
		list = list->next;
	}

	return pstResult;
}

static void				hapi_freeNotiList(HAPI_NotiList_t *pstNotiList)
{
	if (pstNotiList)
	{
		HLIB_LIST_RemoveAllFunc(pstNotiList->pstList, HAMA_Free_CB);
		if (pstNotiList->pszSignalName)
			HAMA_Free(pstNotiList->pszSignalName);
		HAMA_Free(pstNotiList);
	}
}

static HERROR	hapi_DefaultNotifier(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HAPI_t				*hapi;

	HxList_t			*pstList;
	HAPI_Noti_t			*pstNotiInfo;
	HAPI_NotiList_t		*pstNotiList;

	HINT32		nParam;
	HxObject_t	*pstParamObject;
	HxObject_t	**ppArgv;

	hapi = hapi_getInstance();
	pstNotiList = hapi_cloneNotiList((HAPI_NotiList_t*)pvUserData);
	hapi_releaseInstance(hapi);

	HxLOG_Debug("(%s) occurred\n", pstNotiList->pszSignalName);
	if (pstNotiList->bIsSystemSignal)
	{
		pstList = pstNotiList->pstList;
		while (pstList)
		{
			pstNotiInfo = (HAPI_Noti_t*)HLIB_LIST_Data(pstList);
			if (pstNotiInfo->cbNotifier)
				pstNotiInfo->cbNotifier(nArgc, (void*)apArgv, nRequestId, pstNotiInfo->pvUserData);
			pstList = pstList->next;
		}
	} else
	{
		HAMA_ASSERT(nArgc == 1);

		pstParamObject = HLIB_SERIALIZER_UnpackWithCache(pstNotiList->pstHapi->stPool.pstObj, HxOBJECT_BIN_DATA(apArgv[0]), HxOBJECT_BIN_SIZE(apArgv[0]), FALSE);
		if (pstParamObject == NULL)
		{
			HxLOG_Error("Invalid notifier (%s)\n", pstNotiList->pszSignalName);
			goto END_RTN;
		}

		if (pstParamObject)
		{
			nParam = HxOBJECT_ARR_LEN(pstParamObject);
			ppArgv = pstParamObject->u.array.values;
		}

		pstList = pstNotiList->pstList;
		while (pstList)
		{
			pstNotiInfo = (HAPI_Noti_t*)HLIB_LIST_Data(pstList);
			if (pstNotiInfo->cbNotifier)
				pstNotiInfo->cbNotifier(nParam, (void*)ppArgv, nRequestId, pstNotiInfo->pvUserData);
			pstList = pstList->next;
		}

		HLIB_SERIALIZER_FreeObjectWithCache(pstNotiList->pstHapi->stPool.pstObj, pstParamObject);
	}

END_RTN:
	hapi_freeNotiList(pstNotiList);

	return ERR_OK;
}

static HERROR	hapi_DisconnectSignal(HAPI_t *hapi, const HCHAR *pszSignalName, HxRPC_Noti_t cbRpcNotifier)
{
	HxList_t		*list;
	HAPI_NotiList_t	*pstNotiList;
	HAPI_Noti_t		*pstNotiInfo;

	pstNotiList = (HAPI_NotiList_t*)HLIB_HASH_Lookup(hapi->pstSignalTable, (void*)pszSignalName);
	if (pstNotiList == NULL)
		return ERR_FAIL;

	list = HLIB_LIST_FindEx(pstNotiList->pstList, cbRpcNotifier, hapi_compareNotifier);
	if (list)
	{
		pstNotiInfo = (HAPI_Noti_t*)HLIB_LIST_Data(list);
		pstNotiList->pstList = HLIB_LIST_Remove(pstNotiList->pstList, (void*)pstNotiInfo);
		HAMA_Free(pstNotiInfo);

		return ERR_OK;
	}

	return ERR_FAIL;
}

static HERROR	hapi_ConnectSignal(HAPI_t *hapi, const HCHAR *pszSignalName, const HCHAR *pszSignature, HBOOL bSystemSignal, void *pvUserData, HxRPC_Noti_t cbRpcNotifier)
{
	HAPI_NotiList_t	*pstNotiList;
	HAPI_Noti_t		*pstNotiInfo;

	pstNotiList = (HAPI_NotiList_t*)HLIB_HASH_Lookup(hapi->pstSignalTable, (void*)pszSignalName);
	if (pstNotiList == NULL)
	{
		pstNotiList = (HAPI_NotiList_t *)HAMA_Malloc(sizeof(HAPI_NotiList_t));
		pstNotiList->pstHapi = hapi;
		pstNotiList->pszSignalName = HAMA_StrDup(pszSignalName);
		pstNotiList->pstList = NULL;
		pstNotiList->bIsSystemSignal = bSystemSignal;

		HLIB_HASH_Insert(hapi->pstSignalTable, (void*)pstNotiList->pszSignalName, (void*)pstNotiList);

		HLIB_RPC_RegisterNotifyCallback(hapi->nRPCHandle, pszSignalName, pszSignature, pstNotiList, hapi_DefaultNotifier);
	}

	if (HLIB_LIST_FindEx(pstNotiList->pstList, cbRpcNotifier, hapi_compareNotifier))
	{
		HxLOG_Error("Same callback function is registered!!\n");

		return ERR_FAIL;
	}

	pstNotiInfo = (HAPI_Noti_t*)HAMA_Malloc(sizeof(HAPI_Noti_t));
	pstNotiInfo->pvUserData = pvUserData;
	pstNotiInfo->cbNotifier = cbRpcNotifier;
	pstNotiList->pstList = HLIB_LIST_Append(pstNotiList->pstList, (void*)pstNotiInfo);

	HLIB_RPC_Call(hapi->nRPCHandle, "hama.signal.buffered", "s", pszSignalName);

	return ERR_OK;
}

static void		hapi_event_onPacketReceived(void *userData, HINT32 nSocketHandle, HUINT32 requestId, HINT32 size, void *pvData)
{
	HAPI_t	*hapi = (HAPI_t*)userData;

	HLIB_RPC_ProcessRPC(hapi->nRPCHandle, size, pvData, requestId, nSocketHandle);
}

static void		hapi_event_onStatusChanged(void *userData, HxSocket_Status_e eStatus, HINT32 nClientHandle)
{
	(void)userData;
	(void)eStatus;
	(void)nClientHandle;
}

static void		hapi_event_listener(void *arg)
{
	HAPI_t	*hapi = (HAPI_t*)arg;

	HLIB_SOCKET_Listen(hapi->nClientHandle, TRUE, hapi_event_onPacketReceived, hapi_event_onStatusChanged, arg);
}

HAPI_t*	hapi_getInstance(void)
{
	static HAPI_t	*s_hapi = NULL;

	if (s_hapi == NULL)
	{
		unsigned int	tid;

		s_hapi = (HAPI_t*)HAMA_Calloc(sizeof(HAPI_t));

		HxSEMT_Create(&s_hapi->ulSemId, "hapisem", 0);
		HxSEMT_Get(s_hapi->ulSemId);
		s_hapi->nClientHandle  = HLIB_EXTRA_OpenClient("hama");//HLIB_SOCKET_OpenClient(HxSOCKET_UDS, HAMA_SIGNAL_UDS, 0);
		s_hapi->nRPCHandle     = HLIB_RPC_OpenEx(s_hapi->nClientHandle, HLIB_STD_GetPID(), TRUE);
		s_hapi->pstSignalTable = HLIB_HASH_NewFull(HLIB_HASH_GetStringKey, HLIB_HASH_IsStringEqual, NULL, hapi_FreeSignalNotiList);

		s_hapi->stPool.pstObj  = HLIB_SERIALIZER_OpenUnpackCache();
		s_hapi->stPool.pstJob  = HLIB_QUEUE_New((HxQueue_RemoveCallback_t)HLIB_STD_MemFree_CB);

		HLIB_DBG_SetupTrace("HaMaKey", 5, "p:key", "p:type", "i:requestid", "s:from", "s:to");

		VK_TASK_Create(hapi_event_listener, 70, SIZE_64K, "hapi_event_listener", (void*)s_hapi, (unsigned long*)&tid, 0);
		VK_TASK_Start(tid);

		HLIB_RPC_Call(s_hapi->nRPCHandle, "hama.process.add", "iss", HLIB_STD_GetPID(), HLIB_STD_GetCurProcessName(), HLIB_STD_GetCurLxcName());
		HLIB_RPC_RegisterNotifyCallback(s_hapi->nRPCHandle, "hama:onKeyPressed", "ib", (void*)s_hapi, hapi_onKeyPressed);
		HLIB_RPC_RegisterNotifyCallback(s_hapi->nRPCHandle, "hama:onSendProcessInfo", "b", (void*)s_hapi, hapi_onSysMonitor);

		//	Init HaMa Registry
		HxAPI_RegistryInit(s_hapi);
	}
	else
	{
		HxSEMT_Get(s_hapi->ulSemId);
	}

	return s_hapi;
}

void		hapi_releaseInstance(HAPI_t *pstHapi)
{
	HxLOG_Assert(pstHapi);

	HxSEMT_Release(pstHapi->ulSemId);
}

#define	____PUBLIC_APIs_____________
HERROR	HAPI_Init(void)
{
	hapi_releaseInstance(hapi_getInstance());

	return ERR_OK;
}

HERROR	HAPI_HookKeyEvent(HWINID_e eAppId, HBOOL bHooked)
{
	HAPI_t	*hapi;

	hapi = hapi_getInstance();

	if (hapi)
		HLIB_RPC_Call(hapi->nRPCHandle, "hama.window.hook", "ii", eAppId, bHooked ? 1 : 0);

	hapi_releaseInstance(hapi);

	return ERR_OK;
}

HERROR	HAPI_ReturnKeyEvent(HWINID_e eAppId, HInputRetType_e eRetType, HInputEvent_t *pEvent)
{
	HAPI_t	*hapi;

	hapi = hapi_getInstance();

	if (hapi)
	{
		pEvent->eInputReturnType = eRetType;
		HLIB_RPC_Call(hapi->nRPCHandle, "hama.window.returnkey", "ib", eAppId, pEvent, sizeof(HInputEvent_t));
	}

	hapi_releaseInstance(hapi);

	HLIB_DBG_Trace("HaMaKey", {}, pEvent->key_symbol, pEvent->type, pEvent->request_id, HLIB_STD_GetCurProcessName(), "HaMa");
//	HLIB_DBG_BeginTrace("HaMaKey");
//	HLIB_DBG_Trace("HaMaKey", pEvent->key_symbol, pEvent->type, pEvent->request_id, HLIB_STD_GetCurProcessName(), "HaMa");
//	HLIB_DBG_EndTrace("HaMaKey");

	return ERR_OK;
}

HBOOL	HAPI_GetInputEvent(HWINID_e eApplId, HInputEvent_t *pstInputEvent, HINT32 nTimeout)
{
	HAPI_t	*hapi;
	HERROR	err;
	HInputEvent_t		stInputEvent;
	HAPI_KeyHandler_t	*pstHandler;

	hapi = hapi_getInstance();
	pstHandler = hapi_GetKeyHandlerById(hapi, hapi->pstKeyHandlerList, eApplId);
	hapi_releaseInstance(hapi);

	if (pstHandler)
	{
		if (nTimeout == -1)
			err = HLIB_MSG_Receive(pstHandler->ulKeyEvent, &stInputEvent, sizeof(HInputEvent_t));
		else
			err = HLIB_MSG_ReceiveTimeout(pstHandler->ulKeyEvent, &stInputEvent, sizeof(HInputEvent_t), nTimeout);

		if (err == ERR_OK)
		{
			HxSTD_MemCopy(pstInputEvent, &stInputEvent, sizeof(HInputEvent_t));
			HLIB_DBG_Trace("HaMaKey", {}, pstInputEvent->key_symbol, pstInputEvent->type, pstInputEvent->request_id, "HaMa", HLIB_STD_GetCurProcessName());

			HxLOG_Print(HxANSI_COLOR_YELLOW("key_symbol = 0x%x, key_code = %d, type = %d from %s\n")
				, pstInputEvent->key_symbol
				, pstInputEvent->key_code
				, pstInputEvent->type
				, HLIB_STD_GetCurProcessName()
				);

			return TRUE;
		}
	}

	return FALSE;
}

HERROR	HAPI_AddVirtualWindow(HWINID_e eAppId)
{
	HAPI_t	*hapi;

	hapi = hapi_getInstance();

	if (hapi)
		HLIB_RPC_Call(hapi->nRPCHandle, "hama.window.add", "i", eAppId);

	hapi_releaseInstance(hapi);

	return ERR_OK;
}

void	HAPI_SetApplicationScenarioDefault(HWINID_e eApplId)
{
	HAPI_t	*hapi;

	hapi = hapi_getInstance();
	if (hapi)
	{
		HLIB_RPC_Call(hapi->nRPCHandle, "hama.window.deforder", "ii", eApplId, eHSTACK_FOCUS);
		HLIB_RPC_Call(hapi->nRPCHandle, "hama.window.deforder", "ii", eApplId, eHSTACK_WINDOW);
	}

	hapi_releaseInstance(hapi);
}

HERROR	HAPI_SetApplicationFocusOrder(HWINID_e eTargetId, HWINID_e eBaseId, HRELATION_e relation)
{
	HAPI_t	*hapi;

	hapi = hapi_getInstance();
	if (hapi)
		HLIB_RPC_Call(hapi->nRPCHandle, "hama.window.setorder", "iiii", eHSTACK_FOCUS, eTargetId, eBaseId, relation);

	hapi_releaseInstance(hapi);

	return ERR_OK;
}

HERROR	HAPI_SetApplicationWindowOrder(HWINID_e eTargetId, HWINID_e eBaseId, HRELATION_e relation)
{
	HAPI_t	*hapi;

	hapi = hapi_getInstance();
	if (hapi)
		HLIB_RPC_Call(hapi->nRPCHandle, "hama.window.setorder", "iiii", eHSTACK_WINDOW, eTargetId, eBaseId, relation);

	hapi_releaseInstance(hapi);

	return ERR_OK;
}

HERROR	HAPI_RegisterAcceptableKey(HWINID_e eTargetId, HUINT32 eSymbol, HBOOL bRegister)
{
	HAPI_t	*hapi;

	hapi = hapi_getInstance();
	if (hapi)
		HLIB_RPC_Call(hapi->nRPCHandle, "hama.window.regkey", "iii", eTargetId, eSymbol, bRegister ? 1 : 0);

	hapi_releaseInstance(hapi);

	return ERR_OK;
}

HERROR	HAPI_SetWindowStyle(HWINID_e eTargetId, HWINDOWSTYLE_e eWindowStyle, HBOOL bClear)
{
	HAPI_t	*hapi;

	hapi = hapi_getInstance();
	if (hapi)
		HLIB_RPC_Call(hapi->nRPCHandle, "hama.window.setstyle", "iii", eTargetId, eWindowStyle, (bClear ? 1 : 0));

	hapi_releaseInstance(hapi);

	return ERR_OK;
}

HERROR	HAPI_DeleteMyApplication(pid_t pid)
{
	HAPI_t	*hapi;

	hapi = hapi_getInstance();
	if (hapi)
		HLIB_RPC_Call(hapi->nRPCHandle, "hama.process.remove", "i", pid);

	hapi_releaseInstance(hapi);

	return ERR_OK;
}

HERROR	HAPI_SendReadySignal(pid_t pid)
{
	HAPI_t	*hapi;

	hapi = hapi_getInstance();
	if (hapi)
		HLIB_RPC_Call(hapi->nRPCHandle, "hama.process.ready", "is", pid, HLIB_STD_GetProcessName(pid));

	hapi_releaseInstance(hapi);

	return ERR_OK;
}

//	DFB초기화 관련 이슈 수정 전 까지 DFB를 최초로 초기화 되면 (OCTO) 해당 Signal을 날려준다.
HERROR	HAPI_SendDFBReadySignal(void)
{
	HAPI_t	*hapi;

	hapi = hapi_getInstance();
	if (hapi)
		HLIB_RPC_Call(hapi->nRPCHandle, "hama.process.dfbready", NULL);

	hapi_releaseInstance(hapi);

	return ERR_OK;
}

HERROR	HAPI_SendKeyGrab(HWINID_e eWindowId, HBOOL bGrab)
{
	HAPI_t	*hapi;

	hapi = hapi_getInstance();
	if (hapi)
		HLIB_RPC_Call(hapi->nRPCHandle, "hama.window.grabkey", "ii", eWindowId, bGrab ? 1 : 0);

	hapi_releaseInstance(hapi);

	return ERR_OK;
}

HERROR	HAPI_SendWindowActive(HWINID_e eWindowId, HBOOL bActive)
{
	HAPI_t	*hapi;

	hapi = hapi_getInstance();
	if (hapi)
		HLIB_RPC_Call(hapi->nRPCHandle, "hama.window.active", "ii", eWindowId, bActive ? 1 : 0);

	hapi_releaseInstance(hapi);

	return ERR_OK;
}

HERROR	HAPI_SendRemoteRCUKey(HUINT32 ulKeySymbol)
{
	HAPI_t	*hapi;

	hapi = hapi_getInstance();
	if (hapi)
	{
		HInputEvent_t	stInputEvent;

		HxSTD_MemSet(&stInputEvent, 0, sizeof(HInputEvent_t));
		stInputEvent.key_symbol = ulKeySymbol;
		stInputEvent.type       = DIET_KEYPRESS;
		HLIB_RPC_Call(hapi->nRPCHandle, "hama.window.sendkey", "ib", eAWINDOW_HAMA, &stInputEvent, sizeof(HInputEvent_t));

		hapi_releaseInstance(hapi);
	}

	return ERR_OK;
}

HERROR	HAPI_SendRemoteRCUKeyRelease(HUINT32 ulKeySymbol)
{
	HAPI_t	*hapi;

	hapi = hapi_getInstance();
	if (hapi)
	{
		HInputEvent_t	stInputEvent;

		HxSTD_MemSet(&stInputEvent, 0, sizeof(HInputEvent_t));
		stInputEvent.key_symbol = ulKeySymbol;
		stInputEvent.type       = DIET_KEYRELEASE;
		HLIB_RPC_Call(hapi->nRPCHandle, "hama.window.sendkey", "ib", eAWINDOW_HAMA, &stInputEvent, sizeof(HInputEvent_t));

		hapi_releaseInstance(hapi);
	}

	return ERR_OK;
}

HERROR	HAPI_SendRemoteKey(void *pvWindowEvent)
{
	HAPI_t	*hapi;

	hapi = hapi_getInstance();
	if (hapi)
	{
		HInputEvent_t	stInputEvent;

		HAMA_TOOL_CONVDFB2HINPUT((DFBInputEvent*)pvWindowEvent, &stInputEvent);
		HLIB_RPC_Call(hapi->nRPCHandle, "hama.window.sendkey", "ib", eAWINDOW_HAMA, &stInputEvent, sizeof(HInputEvent_t));
	}

	hapi_releaseInstance(hapi);

	return ERR_OK;
}

HERROR	HAPI_DisconnectSignal(const HCHAR *pszSignalName, HxRPC_Noti_t cbRpcNotifier)
{
	HAPI_t	*hapi;
	HERROR	ret = ERR_FAIL;

	hapi = hapi_getInstance();

	if (hapi)
		ret = hapi_DisconnectSignal(hapi, pszSignalName, cbRpcNotifier);

	hapi_releaseInstance(hapi);

	return ret;
}

HERROR	HAPI_ConnectSystemSignal(const HCHAR *pszSignalName, const HCHAR *pszSignature, void *pvUserData, HxRPC_Noti_t cbRpcNotifier)
{
	HAPI_t	*hapi;
	HERROR	ret = ERR_FAIL;

	hapi = hapi_getInstance();

	if (hapi)
		ret = hapi_ConnectSignal(hapi, pszSignalName, pszSignature, TRUE, pvUserData, cbRpcNotifier);

	hapi_releaseInstance(hapi);

	return ret;
}

HERROR	HAPI_ConnectSignal(const HCHAR *pszSignalName, void *pvUserData, HxRPC_Noti_t cbRpcNotifier)
{
	HAPI_t	*hapi;
	HERROR	ret = ERR_FAIL;

	hapi = hapi_getInstance();

	if (hapi)
		ret = hapi_ConnectSignal(hapi, pszSignalName, "b", FALSE, pvUserData, cbRpcNotifier);

	hapi_releaseInstance(hapi);

	return ret;
}

HERROR	HAPI_BroadcastSignal(const HCHAR *pszSignalName, HBOOL bBuffered, const HCHAR *pszSignature, ...)
{
	HAPI_t	*hapi;

	if (HxSTD_StrEmpty(pszSignature) == TRUE)
	{
		HxLOG_Error("pszSignature is NULL!!! (plz add dummy)\n");
		return ERR_FAIL;
	}

	HxLOG_Debug("<--- broadcast : [%s][%s]\n", pszSignalName, pszSignature);

	hapi = hapi_getInstance();

	if (hapi)
	{
		va_list	ap;
		void	*pMakeBuffer;
		void	*pvData = NULL;
		HINT32	nSize = 0;

		va_start(ap, pszSignature);
		pMakeBuffer = HLIB_RPC_MakeSerialize(hapi->nRPCHandle, pszSignature, ap, &nSize);
		if(NULL != pMakeBuffer)
		{
			pvData = HAMA_MemDup(pMakeBuffer, nSize);
		}
		va_end(ap);

        if(pvData)
        {
    		HLIB_RPC_Call(hapi->nRPCHandle, "hama.signal.broadcast", "sib", pszSignalName, bBuffered ? 1 : 0, pvData, nSize);
   		    HAMA_Free(pvData);
        }
	}

	hapi_releaseInstance(hapi);

	return ERR_OK;
}

/*
	registry functions
*/

HERROR	HAPI_SetRegistryListener(void *pvUser, CBHAPIRegHandler pfnRegHandler)
{
	HAPI_t	*hapi;
	HERROR	ret;

	hapi = hapi_getInstance();

	ret = HxAPI_SetRegistryListener(hapi, pvUser, pfnRegHandler);

	hapi_releaseInstance(hapi);

	return ret;
}

static HAPI_t*	hapi_JoinRegistry(void)
{
	HAPI_t	*hapi;

	hapi = hapi_getInstance();

	while (hapi->bRegistryLoaded == FALSE)
	{
		hapi_releaseInstance(hapi);

		HLIB_STD_TaskSleep(10);

		hapi = hapi_getInstance();
	}

	return hapi;
}

HERROR	HAPI_RemoveRegistry(void)
{
	HAPI_t	*hapi;

	hapi = hapi_getInstance();

	HLIB_RPC_Call(hapi->nRPCHandle, "hama.registry.remove", "i", HLIB_STD_GetPID());

	hapi_releaseInstance(hapi);

	return ERR_OK;
}

HERROR	HAPI_SetRegistryEx(const HCHAR *pszKey, HxObjectTag_e eType, const void *pvData, HINT32 nSize)
{
	HAPI_t	*hapi;
	HERROR	ret;

	hapi = hapi_JoinRegistry();

	ret = HxAPI_SetRegistryEx(hapi, pszKey, eType, pvData);

	hapi_releaseInstance(hapi);

	return ret;
}

HERROR	HAPI_GetRegistryEx(const HCHAR *pszKey, HxObjectTag_e eType, void *pvData, HINT32 nMax)
{
	HAPI_t	*hapi;
	HERROR	ret;

	hapi = hapi_JoinRegistry();

	ret = HxAPI_GetRegistryEx(hapi, pszKey, eType, pvData, nMax);

	hapi_releaseInstance(hapi);

	return ret;
}

HERROR	HAPI_SetRegistryCvtStr(const HCHAR *pszKey, const HCHAR *pszTagType, HINT32 nValue, const HCHAR* (*cvtInt2Str)(const HCHAR *, HINT32))
{
	const HCHAR	*pszEnumStr;

	HxLOG_Assert(pszKey && pszTagType);

	pszEnumStr = cvtInt2Str(pszTagType, nValue);
	if (HxSTD_StrEmpty(pszEnumStr))
	{
		HxLOG_Error("enum is not valid [%s](%d)\n", pszTagType, nValue);
		return ERR_FAIL;
	}

	return HAPI_SetRegistryEx(pszKey, HxOBJECT_TYPE_STR, (void*)pszEnumStr, HxSTD_StrLen(pszEnumStr) + 1);
}

HERROR	HAPI_GetRegistryCvtStr(const HCHAR *pszKey, const HCHAR *pszTagType, HINT32 *pnEnum, HINT32 (*cvtStr2Int)(const HCHAR *, const HCHAR *))
{
	static HCHAR	__szEnumStr[64];

	HxLOG_Assert(pszKey && pszTagType && pnEnum);
	if (HAPI_GetRegistryEx(pszKey, HxOBJECT_TYPE_STR, __szEnumStr, 64) == ERR_OK)
	{
		*pnEnum = cvtStr2Int(pszTagType, __szEnumStr);
		return ERR_OK;
	}

	return ERR_FAIL;
}

HERROR 	HAPI_SetSysMonitor(HBOOL bActive)
{
	HAPI_t	*hapi;

	hapi = hapi_getInstance();

	HLIB_RPC_Call(hapi->nRPCHandle, "hama.process.monitor", "i", bActive);

	hapi_releaseInstance(hapi);

	return ERR_OK;
}

HERROR 	HAPI_SysMonitorListener(CBHAPIMonitorHandler pfnListener)
{
	s_pfHapiSysMonitor = pfnListener;

	return ERR_OK;
}



