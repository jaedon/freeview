/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  hama_handler.c
	@brief

	Description:  									\n
	Module: HaMa Event Handler						\n

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
#include	<hlib.h>

#include	<directfb.h>
#include	<sawman.h>

#include	"hama_int.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HCHAR	szSignalName[256];
	HINT32	nSize;
	HUINT8	data[4];
} HAMA_BUFINFO_t;					//	buffered signal data type.

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ***************************/
/*******************************************************************/
static HERROR	hama_handler_doSocketRemoved(HAMA_t *pstHama, HINT32 nClientHandle)
{
	HamaProcessInfo_t	*pstProcInfo;
	HamaWindowInfo_t	*pstWindowInfo;
	//	It means, the process is terminated;

	HxLOG_Warning("remove client with socketHandle : [%d]\n");
	pstProcInfo = HAMA_TOOL_GetProcessInfoByClient(pstHama->pstAttachedProcessList, nClientHandle);
	if (pstProcInfo == NULL)
	{
		HxLOG_Error("Can't find process info with socket [%d]\n", nClientHandle);
		return ERR_FAIL;
	}

	HxLOG_Warning("Socket disconnected [%s]\n", pstProcInfo->szProcessName);

	while ((pstWindowInfo = (HamaWindowInfo_t*)HLIB_LIST_Data(pstProcInfo->pstWindowList)))
	{
		pstHama->pstWindowListFocusOrder = HLIB_LIST_Remove(pstHama->pstWindowListFocusOrder, (void*)pstWindowInfo);
		pstHama->pstZombieWindowList     = HLIB_LIST_Remove(pstHama->pstZombieWindowList,     (void*)pstWindowInfo);
		pstProcInfo->pstWindowList       = HLIB_LIST_Remove(pstProcInfo->pstWindowList,       (void*)pstWindowInfo);
	}

	//	remove attached process list and signal.
	pstHama->pstAttachedProcessList = HLIB_LIST_Remove(pstHama->pstAttachedProcessList, pstProcInfo);
	HAMA_Free(pstProcInfo);

	return ERR_OK;
}

static void		hama_handler_InitDirectFB(HAMA_t *pstHama)
{
#ifndef	USED_DFB_MASTER
	DFBResult err;

	HxLOG_Debug("Received Signal DFB READY!!!\n");
	pstParam = (H3PARAM_t*)data;
	//	Init DirectFB
	DFBCHECK(DirectFBInit(NULL, NULL));
	DFBCHECK(DirectFBCreate(&(applMgr->dfb)));
	DFBCHECK(applMgr->dfb->SetCooperativeLevel(applMgr->dfb, DFSCL_NORMAL));
	DFBCHECK(applMgr->dfb->GetDisplayLayer(applMgr->dfb, DLID_PRIMARY, &(applMgr->layer)));
	DFBCHECK(applMgr->layer->SetCooperativeLevel(applMgr->layer, DLSCL_ADMINISTRATIVE));

	//	Init SaWMan
	HAMA_SAWMAN_Init(pstHama);
#endif
}

static void		hama_handler_AddProcess(HAMA_t *pstHama, HINT32 nProgId, HINT32 nClientHandle, HCHAR *pszCProcessName, HCHAR *pszLxcName)
{
	HCHAR	*pszProcessName;
	HamaProcessInfo_t	*pstProcInfo;

	pstProcInfo = HAMA_TOOL_GetProcessInfoByPid(pstHama, nProgId);
	if (pstProcInfo)
	{
		HxLOG_Error("pid [%d] is already registered!\n", nProgId);
		return;
	}

	if (HxSTD_StrStr(pszLxcName, "lxc"))
	{
		HINT32		nTempProgId;

		//	LXC Flag가 존재하면, PID값을 재 설정해야 한다.
		if (HLIB_STD_GetProcessPidWithLXC(pszCProcessName, pszLxcName, &nTempProgId) == ERR_OK)
			nProgId = nTempProgId;
		else
			HxLOG_Error("fail to find process pid with (%s)(%s)\n", pszCProcessName, pszLxcName);
	}

	pszProcessName = HLIB_STD_GetProcessName(nProgId);
	pstProcInfo = (HamaProcessInfo_t*)HAMA_Calloc(sizeof(HamaProcessInfo_t));
	HLIB_STD_StrNCpySafe(pstProcInfo->szProcessName, pszProcessName, HAMA_PROCESS_NAME_LEN);

	pstProcInfo->nSocketHandle = nClientHandle;
	pstProcInfo->stProcessPid  = nProgId;
	pstProcInfo->ulStartTime   = HLIB_STD_GetSystemTick();

	pstHama->pstAttachedProcessList = HLIB_LIST_Append(pstHama->pstAttachedProcessList, (void*)pstProcInfo);

	//	Find window by pid in ZombieWindowList and move it.
	{
		HamaWindowInfo_t	*windowInfo;

		while ((windowInfo = HAMA_TOOL_GetWindowInfoByPid(pstHama->pstZombieWindowList, nProgId)))
		{
			pstHama->pstZombieWindowList = HLIB_LIST_Remove(pstHama->pstZombieWindowList, (void*)windowInfo);
			pstProcInfo->pstWindowList = HLIB_LIST_Append(pstProcInfo->pstWindowList, (void*)windowInfo);
		}
	}
}

static void		hama_handler_RemoveProcess(HAMA_t *pstHama, HINT32 nProgId, HINT32 nClientHandle)
{
	HxLOG_Error("Not implemented!! Normally, the function will not be called!!!\n");
}

static void		hama_handler_TriggerSystemSignal(HAMA_t *pstHama, HINT32 nClientHandle, const HCHAR *pszSignalName)
{
	if (HxSTD_StrCmp(pszSignalName, "hama:onReadyProcess") == 0)
	{
		HxList_t		*list;
		HamaReadyProcessInfo_t	*pstReadyProcInfo;

		list = pstHama->pstReadyProcessList;
		while (list)
		{
			pstReadyProcInfo = (HamaReadyProcessInfo_t*)HLIB_LIST_Data(list);
			HLIB_RPC_CallbackSpecific(pstHama->nRPCHandle, nClientHandle, "hama:onReadyProcess", "si", pstReadyProcInfo->szProcessName, pstReadyProcInfo->pid);
			list = HLIB_LIST_Next(list);
		}

		HLIB_RPC_CallbackSpecific(pstHama->nRPCHandle, nClientHandle, "hama:onReadyProcess", "si", HLIB_STD_GetCurProcessName(), HLIB_STD_GetPID());
	} else
	if (HxSTD_StrCmp(pszSignalName, "hama:onRegistryChanged") == 0)
	{
		HAMA_REGISTRY_NotifyRegistry(pstHama, nClientHandle);
	} else
	if (HxSTD_StrCmp(pszSignalName, "hama:onPanelPriority") == 0)
	{
		HamaWindowInfo_t	*windowInfo;
		HamaProcessInfo_t	*procInfo;

		procInfo = HAMA_TOOL_GetProcessInfoByClient(pstHama->pstAttachedProcessList, nClientHandle);
		if (procInfo)
		{
			windowInfo = (HamaWindowInfo_t*)HLIB_LIST_Data(procInfo->pstWindowList);
			if (windowInfo)
			{
				HLIB_RPC_CallbackSpecific(pstHama->nRPCHandle, nClientHandle, "hama:onPanelPriority", "i", HAMA_SCENARIO_GetPriority(windowInfo->ulApplId, eHSTACK_PANEL));
			} else
			{
				HxLOG_Warning("no registered window for panel priority\n");
			}
		}
	}
}

static void		hama_handler_ReadyProcess(HAMA_t *pstHama, HINT32 nProgId, HCHAR *pszProcessName)
{
	HamaReadyProcessInfo_t	*pstReadyProcInfo;

	pstReadyProcInfo = (HamaReadyProcessInfo_t*)HLIB_STD_MemCalloc(sizeof(HamaReadyProcessInfo_t));
	pstReadyProcInfo->pid = nProgId;
	HLIB_STD_StrNCpySafe(pstReadyProcInfo->szProcessName, pszProcessName, 256);

	pstHama->pstReadyProcessList = HLIB_LIST_Append(pstHama->pstReadyProcessList, (void*)pstReadyProcInfo);

	HLIB_RPC_Notify(pstHama->nRPCHandle, "hama:onReadyProcess", "si", pszProcessName, nProgId);
}

static void		hama_handler_AddWindow(HAMA_t *pstHama, HINT32 nProgId, HWINID_e eWindowId)
{
	HamaProcessInfo_t	*processInfo;
	HamaWindowInfo_t	*windowInfo;

	processInfo = HAMA_TOOL_GetProcessInfoByPid(pstHama, nProgId);

	windowInfo = HAMA_TOOL_GetWindowInfoByAppId(pstHama->pstWindowListFocusOrder, eWindowId);
	if (windowInfo)
	{
		//	LXC일 경우 DFB를 통해서 오는 PID가 Internal PID --> Global PID로 변경하여야한다.
		windowInfo->stProcessPid = nProgId;
		//	move zombie window to process window.
		{
			HamaWindowInfo_t	*windowInfo;

			while ((windowInfo = HAMA_TOOL_GetWindowInfoByAppId(pstHama->pstZombieWindowList, eWindowId)))
			{
				pstHama->pstZombieWindowList = HLIB_LIST_Remove(pstHama->pstZombieWindowList, (void*)windowInfo);
				processInfo->pstWindowList = HLIB_LIST_Append(processInfo->pstWindowList, (void*)windowInfo);
			}
		}
		return;
	}

	windowInfo = (HamaWindowInfo_t*)HAMA_Calloc(sizeof(HamaWindowInfo_t));
	windowInfo->stProcessPid   = nProgId;
	windowInfo->ulDFBWindowId  = 0;
	windowInfo->sWindowHandle  = 0;
	windowInfo->ulApplId	   = eWindowId;
	windowInfo->nFocusPriority = HAMA_SCENARIO_GetPriority(eWindowId, eHSTACK_FOCUS);
	windowInfo->nStackPriority = HAMA_SCENARIO_GetPriority(eWindowId, eHSTACK_WINDOW);
	windowInfo->nPanelPriority = HAMA_SCENARIO_GetPriority(eWindowId, eHSTACK_PANEL);

	windowInfo->eWindowStyle |= eHWS_WindowInputEnable;
//	windowInfo->eWindowStyle |= eHWS_WindowNoSurface;

	pstHama->pstWindowListFocusOrder = HLIB_LIST_AppendSortEx(pstHama->pstWindowListFocusOrder, (void*)windowInfo, HAMA_TOOL_CompWindowFocusOrder);

	if (processInfo == NULL)
		pstHama->pstZombieWindowList = HLIB_LIST_Append(pstHama->pstZombieWindowList, (void*)windowInfo);
	else
		processInfo->pstWindowList	= HLIB_LIST_Append(processInfo->pstWindowList,  (void*)windowInfo);
}

static void		hama_handler_GrabWindow(HAMA_t *pstHama, HWINID_e eWindowId, HINT32 nGrab)
{
	HamaWindowInfo_t	*windowInfo;

	windowInfo = HAMA_TOOL_GetWindowInfoByAppId(pstHama->pstWindowListFocusOrder, eWindowId);
	if (windowInfo)
	{
		if (nGrab)
			windowInfo->eWindowStyle |= eHWS_WindowInputFilter;
		else
			windowInfo->eWindowStyle &= ~(eHWS_WindowInputFilter);
	}
}

static void		hama_handler_ActiveWindow(HAMA_t *pstHama, HWINID_e eWindowId, HINT32 nActive)
{
	if (HAMA_TOOL_GetWindowInfoByAppId(pstHama->pstWindowListFocusOrder, eWindowId))
	{
		if (nActive)
			HAMA_SAWMAN_HideWindowWOCurrent(pstHama, eWindowId);
		else
			HAMA_SAWMAN_ShowWindowWOCurrent(pstHama, eWindowId);
	}
}

static void		hama_handler_HookWindow(HAMA_t *pstHama, HINT32 nProgId, HWINID_e eWindowId, HINT32 nHooked)
{
	HamaWindowInfo_t	*windowInfo;

	windowInfo = HAMA_TOOL_GetWindowInfoByAppId(pstHama->pstWindowListFocusOrder, eWindowId);
	if (windowInfo == NULL)
	{
		HxLOG_Error("Can't find window handle for application id [%d]\n", nProgId);
		return;
	}

	if (nHooked)
		windowInfo->stHookPid = nProgId;
	else
		windowInfo->stHookPid = 0;
}

static HERROR	hama_handler_RegisterEvent(HAMA_t *pstHama, HWINID_e eWindowId, HINT32 nKeySymbol, HINT32 nRegister)
{
	HamaWindowInfo_t	*windowInfo;

	windowInfo = HAMA_TOOL_GetWindowInfoByAppId(pstHama->pstWindowListFocusOrder, eWindowId);
	if (windowInfo)
	{
		if (nRegister)
		{
			HxList_t	*list = HLIB_LIST_Find(windowInfo->acceptableKeyList, (void*)nKeySymbol);

			if (list)
			{
				HxLOG_Error("Key Symbol [%x] is already registered...\n", nKeySymbol);

				return ERR_FAIL;
			}

			windowInfo->acceptableKeyList = HLIB_LIST_Append(windowInfo->acceptableKeyList, (void*)nKeySymbol);
		}
		else
		{
			if (nKeySymbol == -1)
			{
				//	Unregister all key
				windowInfo->acceptableKeyList = HLIB_LIST_RemoveAll(windowInfo->acceptableKeyList);
			} else
				windowInfo->acceptableKeyList = HLIB_LIST_Remove(windowInfo->acceptableKeyList, (void*)nKeySymbol);
		}

		return ERR_OK;
	}

	HxLOG_Critical("Can't find target id [%x]\n", eWindowId);

	return ERR_FAIL;
}

static HERROR	hama_handler_SetWindowStyle(HAMA_t *pstHama, HWINID_e eWindowId, HWINDOWSTYLE_e eWindowStyle, HINT32 nClear)
{
	HamaWindowInfo_t	*windowInfo;

	windowInfo = HAMA_TOOL_GetWindowInfoByAppId(pstHama->pstWindowListFocusOrder, eWindowId);
	if (windowInfo)
	{
		if (nClear)
			windowInfo->eWindowStyle  = eWindowStyle;
		else
			windowInfo->eWindowStyle |= eWindowStyle;

		return ERR_OK;
	}

	HxLOG_Error("Can't find target id [%x]\n", eWindowId);
	return ERR_FAIL;
}

static void		hama_handler_SetWindowDefaultOrder(HAMA_t *pstHama, HWINID_e eWindowId, HSTACKTYPE_e eStackType)
{
	HUINT32		ulBaseId;
	HamaWindowInfo_t	*pstWindowInfo;

	switch (eStackType)
	{
		case eHSTACK_FOCUS:
			ulBaseId = HAMA_SCENARIO_GetLowerFocusAppId(eWindowId);
			if (ulBaseId)
				HAMA_TOOL_ChangeFocusOrder(pstHama, eWindowId, ulBaseId, eHRELATION_UP);
			break;
		case eHSTACK_WINDOW:
			pstWindowInfo = HAMA_TOOL_GetWindowInfoByAppId(pstHama->pstWindowListFocusOrder, eWindowId);
			if (pstWindowInfo)
				HAMA_SAWMAN_ShowWindow(pstHama, pstWindowInfo->sWindowHandle, pstWindowInfo->nCurrentOpacity);
			break;
		default:
			break;
	}
}

static void		hama_handler_ChangeWindowOrder(HAMA_t *pstHama, HSTACKTYPE_e eStackType, HWINID_e eTargetId, HWINID_e eBaseId, HRELATION_e eRelation)
{
	switch (eStackType)
	{
		case eHSTACK_FOCUS:
			HAMA_TOOL_ChangeFocusOrder(pstHama, eTargetId, eBaseId, eRelation);
			break;
		case eHSTACK_WINDOW:
			HAMA_TOOL_ChangeStackOrder(pstHama, eTargetId, eBaseId, eRelation);
			break;
		default:
			break;
	}
}

static void		hama_handler_TriggerSpecificSignal(HAMA_t *pstHama, HINT32 nClientHandle, const HCHAR *pszSignalName)
{
	HAMA_BUFINFO_t	*pstBufInfo;

	HxLOG_Debug("Trigger Specific Signal : [%s]\n", pszSignalName);
	pstBufInfo = (HAMA_BUFINFO_t*)HLIB_HASH_Lookup(pstHama->pstBufferedSignal, (void*)pszSignalName);
	if (pstBufInfo)
	{
		HxLOG_Debug("Trigger Buffered [%s]\n", pszSignalName);
		//HLIB_RPC_Notify(pstHama->nRPCHandle, pszSignalName, "b", pstBufInfo->data, pstBufInfo->nSize);	// broadcast
		HLIB_RPC_CallbackSpecific(pstHama->nRPCHandle, nClientHandle, pszSignalName, "b", pstBufInfo->data, pstBufInfo->nSize);		// unicast
	} else
	{
		hama_handler_TriggerSystemSignal(pstHama, nClientHandle, pszSignalName);
	}
}

static void		hama_handler_BroadcastSignal(HAMA_t *pstHama, const HCHAR *pszSignalName, HINT32 nBuffered, void *pvData, HINT32 nSize)
{
	if (nBuffered)
	{
		HAMA_BUFINFO_t	*pstBufInfo;

		pstBufInfo = (HAMA_BUFINFO_t*)HAMA_Malloc(sizeof(HAMA_BUFINFO_t) + nSize);
		HLIB_STD_StrNCpySafe(pstBufInfo->szSignalName, pszSignalName, 256);
		pstBufInfo->nSize = nSize;
		HxSTD_MemCopy(pstBufInfo->data, pvData, nSize);

		if (HLIB_HASH_Lookup(pstHama->pstBufferedSignal, (void*)pszSignalName))
			HLIB_HASH_Replace(pstHama->pstBufferedSignal, (void*)pstBufInfo->szSignalName, (void*)pstBufInfo);
		else
			HLIB_HASH_Insert (pstHama->pstBufferedSignal, (void*)pstBufInfo->szSignalName, (void*)pstBufInfo);
	}

	HLIB_RPC_Notify(pstHama->nRPCHandle, pszSignalName, "b", pvData, nSize);

#ifdef	CONFIG_DEBUG
	if (HLIB_RPC_FindCallbackPrototype(pstHama->nRPCHandle, pszSignalName, "b") == TRUE)
	{
		HUINT32		i;
		HxObject_t	*pstObject;
		HCHAR		*pszSignature;

		HxLOG_Debug("Broadcast Signal Info\n");
		pstObject = HLIB_SERIALIZER_UnpackWithCache(pstHama->stPool.pstObj, pvData, nSize, FALSE);
		if (pstObject == NULL)
		{
			HxLOG_Error("error signal\n");
			return;
		}

		pszSignature = HLIB_CBUF_GetBuffer(pstHama->nCBuf, 64);
		HxSTD_MemSet(pszSignature, 0, 64);

		for (i = 0; (i < HxOBJECT_ARR_LEN(pstObject)) && (i < 63); i++)
		{
			HxObject_t	*pstValue = HxOBJECT_ARR_VAL(pstObject, i);

			pszSignature[i] = pstValue->type;
#if	0
			switch (pstValue->type)
			{
				case HxOBJECT_TYPE_BIN:
					pszSignature[i] = HxOBJECT_TYPE_BIN;
					HxLOG_Debug("arg(%d) Binary (%d)(%p)\n", i, HxOBJECT_BIN_SIZE(pstValue), HxOBJECT_BIN_DATA(pstValue));
					break;
				case HxOBJECT_TYPE_INT:
					HxLOG_Debug("arg(%d) Integer (%d)\n", i, HxOBJECT_INT(pstValue));
					break;
				case HxOBJECT_TYPE_STR:
					HxLOG_Debug("arg(%d) String  (%s)\n", i, HxOBJECT_STR(pstValue));
					break;
				default:
					break;
			}
#endif
		}
		HLIB_SERIALIZER_FreeObjectWithCache(pstHama->stPool.pstObj, pstObject);

		if (HxSTD_StrEmpty(pszSignature) == FALSE)
			HLIB_RPC_SetFakeCallbackPrototype(pstHama->nRPCHandle, pszSignalName, "b", pszSignature);
	}
#endif
}

void	HAMA_HANDLER_MainLoop(void *arg)
{
	HAMA_t		*pstHama;
	HamaMsg_t	stMsg;

	while (1)
	{
		pstHama = HAMA_GetInstance();

		if (HAMA_GetEvent(pstHama, &stMsg) == ERR_OK)
		{
			switch (stMsg.ulMsg)
			{
				case eHSIGSYS_SAWMAN_INPUTEVENT:
					HAMA_TOOL_AddKeyEventToList(pstHama, (HInputEvent_t*)stMsg.ulParam1);
					HAMA_Free((void*)stMsg.ulParam1);
					break;
				case eHSIGSYS_INTERNAL_SOCKETREMOVE:
					hama_handler_doSocketRemoved(pstHama, stMsg.ulParam1);
					break;
#if 0
				case eHSIGSYS_APP_EXITED:
				case eHSIGSYS_APP_LAUNCHCONDITION:
					HAMA_SCENARIO_RunModule(pstHama, NULL, NULL);
					break;
#endif

				case eHSIGSYS_DFB_READY:
					hama_handler_InitDirectFB(pstHama);
					break;

				case eHSIGSYS_APP_ADD:
					hama_handler_AddProcess(pstHama, stMsg.ulParam1/*program id*/, stMsg.ulParam2 /*client handle*/, (HCHAR*)stMsg.ulParam3, (HCHAR*)stMsg.ulParam4);
					HAMA_Free((void*)stMsg.ulParam3);
					HAMA_Free((void*)stMsg.ulParam4);
					break;
				case eHSIGSYS_APP_REMOVE:
					hama_handler_RemoveProcess(pstHama, stMsg.ulParam1/*program id*/, stMsg.ulParam2 /*client handle */);
					break;
				case eHSIGSYS_APP_READY:
					hama_handler_ReadyProcess(pstHama, stMsg.ulParam1 /*program id*/, (HCHAR*)stMsg.ulParam2 /* pszProcessName */);
					break;

				case eHSIGSYS_WINDOW_ADD:
					hama_handler_AddWindow(pstHama, stMsg.ulParam1/*program id*/, stMsg.ulParam2 /*window id*/);
					break;
				case eHSIGSYS_WINDOW_GRABKEY:
					hama_handler_GrabWindow(pstHama, stMsg.ulParam1/*window id*/, stMsg.ulParam2 /* bGrab */);
					break;
				case eHSIGSYS_WINDOW_ACTIVE:
					hama_handler_ActiveWindow(pstHama, stMsg.ulParam1/*window id*/, stMsg.ulParam2 /* bActive */);
					break;
				case eHSIGSYS_WINDOW_HOOKEVENT:
					hama_handler_HookWindow(pstHama, stMsg.ulParam1 /*program id*/, stMsg.ulParam2/*window id*/, stMsg.ulParam3 /* bHooked */);
					break;
				case eHSIGSYS_WINDOW_ACCEPTABLEKEY:
					hama_handler_RegisterEvent(pstHama, stMsg.ulParam1 /*window id*/, stMsg.ulParam2 /* key symbole */, stMsg.ulParam3 /* bRegister */);
					break;
				case eHSIGSYS_WINDOW_SETSTYLE:
					hama_handler_SetWindowStyle(pstHama, stMsg.ulParam1 /*window id*/, stMsg.ulParam2 /* style */, stMsg.ulParam3 /* nClear */);
					break;
				case eHSIGSYS_WINDOW_DEFAULTORDER:
					hama_handler_SetWindowDefaultOrder(pstHama, stMsg.ulParam1 /*window id*/, stMsg.ulParam2 /* HSTACKTYPE_e */);
					break;
				case eHSIGSYS_WINDOW_CHANGEORDER:
					hama_handler_ChangeWindowOrder(pstHama, stMsg.ulParam1 /*HSTACKTYPE_e*/, stMsg.ulParam2 /*target window id*/, stMsg.ulParam3 /*base window id*/, stMsg.ulParam4 /*HRELATION_e*/);
					break;
				case eHSIGSYS_WINDOW_SENDEVENT:
					HAMA_TOOL_AddKeyEventToList(pstHama, (HInputEvent_t*)stMsg.ulParam2);
					HAMA_Free((void*)stMsg.ulParam2);
					break;
				case eHSIGSYS_WINDOW_CONSUMED:
					HAMA_TOOL_SendInputEvent(pstHama, (HInputEvent_t*)stMsg.ulParam2);
					HAMA_Free((void*)stMsg.ulParam2);
					break;
				case eHSIGSYS_SIGNAL_TRIGGERBUFFER:
					hama_handler_TriggerSpecificSignal(pstHama, stMsg.ulParam1, (const HCHAR*)stMsg.ulParam2);
					HAMA_Free((void*)stMsg.ulParam2);
					break;
				case eHSIGSYS_SIGNAL_BROADCAST:
					hama_handler_BroadcastSignal(pstHama, (const HCHAR*)stMsg.ulParam1, stMsg.ulParam2, (void*)stMsg.ulParam3, stMsg.ulParam4);
					HAMA_Free((void*)stMsg.ulParam1);
					HAMA_Free((void*)stMsg.ulParam3);
					//	HCHAR *signalName
					//	HBOOL bBuffered;
					//	HxSERIALIZER_t *pvData
					//	HINT3	size
					break;
				default:
					break;
			}
		}
		else
		{
			HamaKeyProcessInfo_t	*pstKeyProcInfo = &pstHama->stKeyProcessInfo;

            if(pstKeyProcInfo == NULL)
            {
                HAMA_ReleaseInstance(pstHama);
                continue;
            }

			if (pstKeyProcInfo->bWaitingReturn && ((pstKeyProcInfo->ulLastEventSendTime + HAMA_KEY_RETURN_TICK) < HLIB_STD_GetSystemTick()))
			{
				HInputEvent_t	*pstWindowEventInfo;

				pstWindowEventInfo = (HInputEvent_t*)HLIB_LIST_Data(pstHama->pstKeyEventList);
				if (pstWindowEventInfo == NULL)
				{
					HxLOG_Critical("need check!\n");
				}
				else
				{
                    HInputEvent_t	stInputEventInfo;

					// When key return is timeout, process to cunsume to prevent hang-up
					HxLOG_Warning(" @@@ Application [%s](%d) does not return key...(%d) => consume key ..!!\n\n\n", pstKeyProcInfo->pstCurrentProcessInfo->szProcessName, pstKeyProcInfo->pstCurrentWindowInfo->ulApplId, pstKeyProcInfo->ulWaitingRequestId);
					HxSTD_MemCopy(&stInputEventInfo, pstWindowEventInfo, sizeof(HInputEvent_t));
					stInputEventInfo.request_id = pstKeyProcInfo->ulWaitingRequestId;
					stInputEventInfo.window_id  = pstKeyProcInfo->pstCurrentWindowInfo->ulApplId;
                    stInputEventInfo.eInputReturnType = eHINPUTRET_CONSUMED;
                    HAMA_TOOL_SendInputEvent(pstHama, &stInputEventInfo);
				}
			}
			else
			{
				if (HAMA_TOOL_SendInputEvent(pstHama, NULL) == ERR_FAIL)
				{
					HAMA_EVT_t	stEvt;
					HUINT32		ulCurSystemTick;
					HINT32		nTickDiff;
					HINT32		nlTimeOut = HAMA_KEYIDLE_TIMEOUT;
					//	Save registry before hama sleep.
					HAMA_REGISTRY_SaveData(pstHama);
					HAMA_ReleaseInstance(pstHama);

					//HxLOG_Warning("<--- HAMA is now going to wait new operations.\n");
					ulCurSystemTick = HLIB_STD_GetSystemTick();
					nTickDiff = ulCurSystemTick - pstHama->ulLastKeyActionTime;
					//HAMA_REGISTRY_GetAutomaticPowerdown(&ulTimeOut); 이미 3시간으로 나간 모델때문에..
					if (nTickDiff > nlTimeOut)
					{
                        HxLOG_Warning(" @@@ TickDiff:Auto Power down TimeOut(%d:%d)\n\n", nTickDiff, nlTimeOut);
						HxLOG_Warning("system is idle ---> broadcast signal...\n");
						//	broadcast signal : system is idle.
						pstHama->ulLastKeyActionTime = ulCurSystemTick;			//	reset idle time.

						HLIB_RPC_Notify(pstHama->nRPCHandle, "hama:onSystemUserActionIdle", NULL);
					}
					HLIB_MSG_ReceiveTimeout(pstHama->ulHamaEvtQId, &stEvt, sizeof(HAMA_EVT_t), HAMA_WAKEUP_DEFAULT);
					while (HLIB_MSG_ReceiveTimeout(pstHama->ulHamaEvtQId, &stEvt, sizeof(HAMA_EVT_t), 0) == ERR_OK);		//	Flush Event Buffer

					//HxLOG_Warning("---> HAMA is now going to process operations.\n");
					continue;
				}
			}
		}

		HAMA_ReleaseInstance(pstHama);
	}
}

