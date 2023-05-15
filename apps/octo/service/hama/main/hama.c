/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  hama.c
	@brief

	Description:  									\n
	Module: HaMa Main application					\n

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

#include	<directfb.h>
#include	<sawman.h>
#include	<sys/wait.h>

#include	"hama_int.h"

#if defined(CONFIG_HUMAX_CRASHREPORT)
#include "hcrlib.h"
#endif

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ***************************/
/*******************************************************************/
static void	hama_system_WaitingDFBMaster(void)
{
	HUINT32	ulSystemTick;
	ulSystemTick = VK_TIMER_GetSystemTick();
	HxLOG_Print("[%s](%d) Waiting for DFB Master Initialization!\n", __FUNCTION__, __LINE__);
	while (HLIB_FILE_Exist("/tmp/.dfbmaster.init") == FALSE)
	{
		HLIB_STD_TaskSleep(100);
	}
	HxLOG_Warning("DFB is initialized : [%d]ms\n", VK_TIMER_GetSystemTick() - ulSystemTick);
}

static void	hama_process_initializeDFB(HAMA_t *pstHama)
{
	DFBResult err;

	DFBCHECK(DirectFBInit(NULL, NULL));
	DFBCHECK(DirectFBCreate(&(pstHama->dfb)));
	DFBCHECK(pstHama->dfb->SetCooperativeLevel(pstHama->dfb, DFSCL_NORMAL));
	DFBCHECK(pstHama->dfb->GetDisplayLayer(pstHama->dfb, DLID_PRIMARY, &(pstHama->layer)));
	DFBCHECK(pstHama->layer->SetCooperativeLevel(pstHama->layer, DLSCL_ADMINISTRATIVE));

	//	Init SaWMan
	HAMA_SAWMAN_Init(pstHama);
}

#define	____HAMA_RPC_ROUTINE_____________________
//	apArgv[0] = nProgId
//	apArgv[1] = pszProcessName
static HERROR	hama_rpc_ReadyProcess(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HAMA_t	*pstHama;

	HAMA_ASSERT(nArgc == 2);

	pstHama = HAMA_GetInstance();

	HAMA_AddEvent(pstHama, eHSIGSYS_APP_READY, HxOBJECT_INT(apArgv[0]), (HUINT32)HLIB_STD_StrDup(HxOBJECT_STR(apArgv[1])), 0, 0);

	HAMA_ReleaseInstance(pstHama);

	return ERR_OK;
}

//	apArgv[0] = nProgId
static HERROR	hama_rpc_AddProcess(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HAMA_t	*pstHama;

	HAMA_ASSERT(nArgc == 3);

	pstHama = HAMA_GetInstance();

	HAMA_AddEvent(pstHama, eHSIGSYS_APP_ADD, HxOBJECT_INT(apArgv[0]), nClientHandle, (HUINT32)HLIB_STD_StrDup(HxOBJECT_STR(apArgv[1])), (HUINT32)HLIB_STD_StrDup(HxOBJECT_STR(apArgv[2])));

	HAMA_ReleaseInstance(pstHama);

	return ERR_OK;
}

//	apArgv[0] = nProgId
static HERROR	hama_rpc_RemoveProcess(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HAMA_t	*pstHama;

	HAMA_ASSERT(nArgc == 0);

	pstHama = HAMA_GetInstance();

	HAMA_AddEvent(pstHama, eHSIGSYS_APP_REMOVE, HxOBJECT_INT(apArgv[0]), nClientHandle,  0, 0);

	HAMA_ReleaseInstance(pstHama);

	return ERR_OK;
}

//	nArgc == 0
static HERROR	hama_rpc_ReadyDFB(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HAMA_t	*pstHama;
//	HINT32	nProgId;
	HamaProcessInfo_t	*processInfo;

	HAMA_ASSERT(nArgc == 0);

	pstHama = HAMA_GetInstance();

	processInfo = HAMA_TOOL_GetProcessInfoByClient(pstHama->pstAttachedProcessList, nClientHandle);
	if (processInfo)
	{
		HAMA_AddEvent(pstHama, eHSIGSYS_DFB_READY, processInfo->stProcessPid, 0, 0, 0);
	} else
	{
		HxLOG_Error("Fail to get program id [%d]\n", nClientHandle);
	}

	HAMA_ReleaseInstance(pstHama);

	return ERR_OK;
}

//	apArgv[0] = HWINID_e
static HERROR	hama_rpc_AddVirtualWindow(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HAMA_t	*pstHama;
//	HINT32	nProgId;
	HamaProcessInfo_t	*processInfo;

	HAMA_ASSERT(nArgc == 1);

	pstHama = HAMA_GetInstance();

	processInfo = HAMA_TOOL_GetProcessInfoByClient(pstHama->pstAttachedProcessList, nClientHandle);
	if (processInfo)
	{
		HAMA_AddEvent(pstHama, eHSIGSYS_WINDOW_ADD, processInfo->stProcessPid, HxOBJECT_INT(apArgv[0]),  0, 0);
	} else
	{
		HxLOG_Error("Fail to get program id [%d]\n", nClientHandle);
	}

	HAMA_ReleaseInstance(pstHama);

	return ERR_OK;
}

//	apArgv[0] = HWINID_e
//	apArgv[1] = bGrab
static HERROR	hama_rpc_GrabWindowKey(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HAMA_t	*pstHama;

	HAMA_ASSERT(nArgc == 2);

	pstHama = HAMA_GetInstance();

	HAMA_AddEvent(pstHama, eHSIGSYS_WINDOW_GRABKEY, HxOBJECT_INT(apArgv[0]), HxOBJECT_INT(apArgv[1]), 0, 0);

	HAMA_ReleaseInstance(pstHama);

	return ERR_OK;
}

//	apArgv[0] = HWINID_e
//	apArgv[1] = bActive
static HERROR	hama_rpc_ActiveWindow(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HAMA_t	*pstHama;

	HAMA_ASSERT(nArgc == 2);

	pstHama = HAMA_GetInstance();

	HAMA_AddEvent(pstHama, eHSIGSYS_WINDOW_ACTIVE, HxOBJECT_INT(apArgv[0]), HxOBJECT_INT(apArgv[1]), 0, 0);

	HAMA_ReleaseInstance(pstHama);

	return ERR_OK;
}

//	apArgv[0] = HWINID_e
//	apArgv[1] = bHooked
static HERROR	hama_rpc_HookWindow(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HAMA_t	*pstHama;
//	HINT32	nProgId;
	HamaProcessInfo_t	*processInfo;

	HAMA_ASSERT(nArgc == 2);

	pstHama = HAMA_GetInstance();

	processInfo = HAMA_TOOL_GetProcessInfoByClient(pstHama->pstAttachedProcessList, nClientHandle);
	if (processInfo)
	{
		HAMA_AddEvent(pstHama, eHSIGSYS_WINDOW_HOOKEVENT, processInfo->stProcessPid, HxOBJECT_INT(apArgv[0]), HxOBJECT_INT(apArgv[1]), 0);
	} else
	{
		HxLOG_Error("Fail to get program id [%d]\n", nClientHandle);
	}

	HAMA_ReleaseInstance(pstHama);

	return ERR_OK;
}

//	apArgv[0] = HWINID_e
//	apArgv[1] = nKeySymbol
//	apArgv[2] = bRegister
static HERROR	hama_rpc_RegisterEvent(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HAMA_t	*pstHama;

	HAMA_ASSERT(nArgc == 3);

	pstHama = HAMA_GetInstance();

	HAMA_AddEvent(pstHama, eHSIGSYS_WINDOW_ACCEPTABLEKEY, HxOBJECT_INT(apArgv[0]), HxOBJECT_INT(apArgv[1]), HxOBJECT_INT(apArgv[2]), 0);

	HAMA_ReleaseInstance(pstHama);

	return ERR_OK;
}

//	apArgv[0] = HWINID_e
//	apArgv[1] = HWINDOWSTYLE_e
//	apArgv[2] = bClear
static HERROR	hama_rpc_SetWindowStyle(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HAMA_t	*pstHama;

	HAMA_ASSERT(nArgc == 3);

	pstHama = HAMA_GetInstance();

	HAMA_AddEvent(pstHama, eHSIGSYS_WINDOW_SETSTYLE, HxOBJECT_INT(apArgv[0]), HxOBJECT_INT(apArgv[1]), HxOBJECT_INT(apArgv[2]), 0);

	HAMA_ReleaseInstance(pstHama);

	return ERR_OK;
}

//	apArgv[0] = HWINID_e
//	apArgv[1] = HSTACKTYPE_e
static HERROR	hama_rpc_SetDefaultOrder(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HAMA_t	*pstHama;

	HAMA_ASSERT(nArgc == 2);

	pstHama = HAMA_GetInstance();

	HAMA_AddEvent(pstHama, eHSIGSYS_WINDOW_DEFAULTORDER, HxOBJECT_INT(apArgv[0]), HxOBJECT_INT(apArgv[1]), 0, 0);

	HAMA_ReleaseInstance(pstHama);

	return ERR_OK;
}

//	apArgv[0] = HSTACKTYPE_e
//	apArgv[1] = HWINID_e (TARGET)
//	apArgv[2] = HWINID_e (BASE)
//	apArgv[3] = HRELATION_e
static HERROR	hama_rpc_SetFocusOrder(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HAMA_t	*pstHama;

	HAMA_ASSERT(nArgc == 4);

	pstHama = HAMA_GetInstance();

	HAMA_AddEvent(pstHama, eHSIGSYS_WINDOW_CHANGEORDER, HxOBJECT_INT(apArgv[0]), HxOBJECT_INT(apArgv[1]), HxOBJECT_INT(apArgv[2]), HxOBJECT_INT(apArgv[3]));

	HAMA_ReleaseInstance(pstHama);

	return ERR_OK;
}

//	apArgv[0] = HWINID_e --> if arg is eWINDOWID_HAMA, the key will be traversed.
//	apArgv[1] = void *pvKeyEvent (HInputEvent_t)
static HERROR	hama_rpc_SendKeyEvent(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HAMA_t	*pstHama;

	HAMA_ASSERT(nArgc == 2);

	pstHama = HAMA_GetInstance();

	HAMA_AddEvent(pstHama, eHSIGSYS_WINDOW_SENDEVENT, HxOBJECT_INT(apArgv[0]), (HUINT32)HAMA_MemDup(HxOBJECT_BIN_DATA(apArgv[1]), HxOBJECT_BIN_SIZE(apArgv[1])), HxOBJECT_BIN_SIZE(apArgv[1]), 0);

	HAMA_ReleaseInstance(pstHama);

	return ERR_OK;
}

//	apArgv[0] = HWINID_e --> if arg is eWINDOWID_HAMA, the key will be traversed.
//	apArgv[1] = void *pvKeyEvent (HInputEvent_t)
static HERROR	hama_rpc_ReturnKeyEvent(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HAMA_t	*pstHama;

	HAMA_ASSERT(nArgc == 2);

	pstHama = HAMA_GetInstance();

	HAMA_AddEvent(pstHama, eHSIGSYS_WINDOW_CONSUMED, HxOBJECT_INT(apArgv[0]), (HUINT32)HAMA_MemDup(HxOBJECT_BIN_DATA(apArgv[1]), HxOBJECT_BIN_SIZE(apArgv[1])), HxOBJECT_BIN_SIZE(apArgv[1]), 0);

	HAMA_ReleaseInstance(pstHama);

	return ERR_OK;
}

//	apArgv[0] = HCHAR* signal name
//	apArgv[1] = HBOOL bBuffered
//	apArgv[2] = void * pvHxSerializerPackedData
static HERROR	hama_rpc_BroadcastSignal(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HAMA_t	*pstHama;

	HAMA_ASSERT(nArgc == 3);

	pstHama = HAMA_GetInstance();

	HAMA_AddEvent(pstHama, eHSIGSYS_SIGNAL_BROADCAST, (HUINT32)HAMA_StrDup(HxOBJECT_STR(apArgv[0])), HxOBJECT_INT(apArgv[1]), (HUINT32)HAMA_MemDup(HxOBJECT_BIN_DATA(apArgv[2]), HxOBJECT_BIN_SIZE(apArgv[2])), HxOBJECT_BIN_SIZE(apArgv[2]));

	HAMA_ReleaseInstance(pstHama);

	return ERR_OK;
}

//	apArgv[0] = HCHAR *signalName;
static HERROR	hama_rpc_TriggerBufferedSignal(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HAMA_t	*pstHama;

	HAMA_ASSERT(nArgc == 1);

	pstHama = HAMA_GetInstance();

	HAMA_AddEvent(pstHama, eHSIGSYS_SIGNAL_TRIGGERBUFFER, nClientHandle, (HUINT32)HAMA_StrDup(HxOBJECT_STR(apArgv[0])), 0, 0);

	HAMA_ReleaseInstance(pstHama);

	return ERR_OK;
}

static HERROR	hama_rpc_SetProcessMonitor(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HAMA_ASSERT(nArgc == 1);

	HAMA_TOOL_ProcessMonitor(HxOBJECT_INT(apArgv[0]));

	return ERR_OK;
}


#if 0
static HERROR	hama_rpc_LauncherStarted(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HAMA_t	*pstHama;

	HAMA_ASSERT(nArgc == 2);
	HxLOG_Warning("Process Started - [%s : %d]\n", HxOBJECT_STR(apArgv[0]), HxOBJECT_INT(apArgv[1]));

	pstHama = HAMA_GetInstance();

	HAMA_SCENARIO_ProcessStartedSignal(HxOBJECT_STR(apArgv[0]), HxOBJECT_INT(apArgv[1]));

	HAMA_ReleaseInstance(pstHama);

	return ERR_OK;
}

static HERROR	hama_rpc_LauncherStopped(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HAMA_t	*pstHama;

	HAMA_ASSERT(nArgc == 2);
	HxLOG_Warning("Process Stopped - [%d, %d]\n", HxOBJECT_INT(apArgv[0]), HxOBJECT_INT(apArgv[1]));

	pstHama = HAMA_GetInstance();

	//	Process Stopped가 왔을경우, 해당 Client를 RPC에서 제외 시킨다.
	{
		HamaProcessInfo_t	*pstProcInfo;

		pstProcInfo = HAMA_TOOL_GetProcessInfoByPid(pstHama, HxOBJECT_INT(apArgv[0]));
		if (pstProcInfo)
			HLIB_RPC_RemoveClientHandle(pstHama->nRPCHandle, pstProcInfo->nSocketHandle);
	}

	HAMA_SCENARIO_ProcessStoppedSignal(HxOBJECT_INT(apArgv[0]), HxOBJECT_INT(apArgv[1]));
	HAMA_AddEvent(pstHama, eHSIGSYS_APP_EXITED, 0, 0, 0, 0);
	HAMA_ReleaseInstance(pstHama);

	return ERR_OK;
}

static HERROR	hama_rpc_LauncherRegister(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HAMA_t	*pstHama;

	HAMA_ASSERT(nArgc == 1);
	HxLOG_Warning("HamaLauncher is ready with (%s)\n", HxOBJECT_STR(apArgv[0]));

	pstHama = HAMA_GetInstance();

	HAMA_SCENARIO_RegisterProcessByLauncher(HxOBJECT_STR(apArgv[0]));
	HAMA_AddEvent(pstHama, eHSIGSYS_APP_LAUNCHCONDITION, 0, 0, 0, 0);

	HAMA_ReleaseInstance(pstHama);

	return ERR_OK;
}
#endif

HERROR HAMA_RPC_Register(HAMA_t *pstHama)
{
	HLIB_RPC_RegisterCall(pstHama->nRPCHandle, "hama.process.dfbready",NULL,   hama_rpc_ReadyDFB, NULL, "dfb is ready : NULL");

	HLIB_RPC_RegisterCall(pstHama->nRPCHandle, "hama.process.add",     "iss",   hama_rpc_AddProcess, NULL, "add application (HINT32 nProgId, HCHAR *pszProcessName, HCHAR *pszLxcName)");
	HLIB_RPC_RegisterCall(pstHama->nRPCHandle, "hama.process.ready",   "is",   hama_rpc_ReadyProcess, NULL, "ready application (HINT32 nProgId)");
	HLIB_RPC_RegisterCall(pstHama->nRPCHandle, "hama.process.remove",  "i",   hama_rpc_RemoveProcess, NULL, "remove application (HINT32 nProgId)");
	HLIB_RPC_RegisterCall(pstHama->nRPCHandle, "hama.process.monitor",  "i",   hama_rpc_SetProcessMonitor, NULL, "Set hama System monitor (HBOOL bActive)");

	HLIB_RPC_RegisterCall(pstHama->nRPCHandle, "hama.window.add",       "i",   hama_rpc_AddVirtualWindow, NULL, "add window w/o dfb (just for key event receiving) : (HWINID_e eWindowId)");
	HLIB_RPC_RegisterCall(pstHama->nRPCHandle, "hama.window.grabkey",   "ii",  hama_rpc_GrabWindowKey, NULL, "window grabbed key : (HWINID_e eWindowId, HBOOL bGrab)");
	HLIB_RPC_RegisterCall(pstHama->nRPCHandle, "hama.window.active",    "ii",  hama_rpc_ActiveWindow, NULL, "active window : (HWINID_e eWindowId, HBOOL bActive)");
	HLIB_RPC_RegisterCall(pstHama->nRPCHandle, "hama.window.hook",      "ii",  hama_rpc_HookWindow, NULL, "hook key event : (HWINID_e eWindowId, HBOOL bHooked)");
	HLIB_RPC_RegisterCall(pstHama->nRPCHandle, "hama.window.regkey",    "iii", hama_rpc_RegisterEvent, NULL, "register event : (HWINID_e eWindowId, HINT32 nKeySymbol, HBOOL bEnable)");
	HLIB_RPC_RegisterCall(pstHama->nRPCHandle, "hama.window.setstyle",  "iii", hama_rpc_SetWindowStyle, NULL, "change window style : (HWINID_e eWindowId, HWINDOWSTYLE_e eStyle, HBOOL bClear)");
	HLIB_RPC_RegisterCall(pstHama->nRPCHandle, "hama.window.deforder",  "ii",  hama_rpc_SetDefaultOrder, NULL, "set order default : (HWINID_e eWindowId, HSTACKTYPE_e)");
	HLIB_RPC_RegisterCall(pstHama->nRPCHandle, "hama.window.setorder",  "iiii",hama_rpc_SetFocusOrder, NULL, "set focus order : (HINT32 nType(focus(0), stack(1), ...) , HWINID_e eTargetId, HWINID_e eBaseId, HRELATION_e eUpDown)");
	HLIB_RPC_RegisterCall(pstHama->nRPCHandle, "hama.window.sendkey",   "ib",  hama_rpc_SendKeyEvent, NULL, "send key event (HWINID_e eWinId (eAWINDOW_HAMA means traverse key events), HInputEvent_t)");
	HLIB_RPC_RegisterCall(pstHama->nRPCHandle, "hama.window.returnkey", "ib",  hama_rpc_ReturnKeyEvent, NULL, "return key event (HWINID_e, HInputEvent_t *)");

	HLIB_RPC_RegisterCall(pstHama->nRPCHandle, "hama.signal.broadcast", "sib", hama_rpc_BroadcastSignal, NULL, "broadcast signal : (HCHAR *pszSignalName, HBOOL bBuffered, void *HxSERIALIZER_PACKED_DATA");
	HLIB_RPC_RegisterCall(pstHama->nRPCHandle, "hama.signal.buffered",  "s",   hama_rpc_TriggerBufferedSignal, NULL, "trigger buffered signal : (HCHAR *pszSignalName)");

	HLIB_RPC_CreateNotification(pstHama->nRPCHandle, "hama:onReadyProcess", "si", "notify the process is ready (char *processName, int pid)");
	HLIB_RPC_CreateNotification(pstHama->nRPCHandle, "hama:onKeyPressed", "ib", "notify key event (HWINID_e, HInputEvent_t)");
	HLIB_RPC_CreateNotification(pstHama->nRPCHandle, "hama:onInputNotify", "ii", "notify key input");
	HLIB_RPC_CreateNotification(pstHama->nRPCHandle, "hama:onPanelPriority", "i", "notify panel priority");
	HLIB_RPC_CreateNotification(pstHama->nRPCHandle, "hama:onSystemUserActionIdle", NULL, "notify system user action is idle");
	HLIB_RPC_CreateNotification(pstHama->nRPCHandle, "hama:onSendProcessInfo", NULL, "notify process info");

	HAMA_REGISTRY_Init(pstHama);

	return ERR_OK;
}

HINT32	HAMA_Main(HINT32 argc, char **argv)
{
	HAMA_t	*pstHama;

#if defined(CONFIG_HUMAX_CRASHREPORT)
	HCR_Init(NULL);
#endif

	pstHama = HAMA_InitInstance();

#ifdef	USED_DFB_MASTER
	//	waiting for initializing DFB Master.
	HxLOG_Warning("Waiting for DFB Initializing...\n");
	hama_system_WaitingDFBMaster();
	hama_process_initializeDFB(pstHama);
#endif

#ifdef	CONFIG_DEBUG
	HLIB_DBG_Init();
	HLIB_DBG_SetupTrace("HaMaKey", 5, "p:key", "p:type", "i:requestid", "s:from", "s:to");
	HLIB_DBG_SetupTrace("HaMaSignalRecv", 4, "s:from", "p:signal", "p:method", "p:type");
#endif

	//	Drop Root Previleges --> if CONFIG_DROP_ROOT_PRIVILEGES is ON,
	HLIB_EXTRA_DropRootPrivileges("hama");

	pstHama = HAMA_GetInstance();

	HAMA_ReleaseInstance(pstHama);

#ifdef	CONFIG_DEBUG
	HAMA_DEBUG_InitCommandTools();
#endif

	return 0;
}
