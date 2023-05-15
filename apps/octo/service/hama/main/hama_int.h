/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  hama_int.h
	@brief

	Description:  									\n
	Module: HaMa Internal Header                    \n

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

#ifndef __HAMA_INT_H__
#define __HAMA_INT_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include	<hlib.h>

#include	<directfb.h>
#include	<sawman.h>

#include	<hapi.h>

#ifdef	OBAMA_IS_DFBMASTER
#else
	#define	USED_DFB_MASTER
#endif

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	HAMA_MAX_KEY_BUFFER_NUM		1
#define	HAMA_KEY_RETURN_TICK		1000
#define	HAMA_PROCESS_NAME_LEN		128
#define	HAMA_SCENARIO_FILE			"/etc/octo_system.ini"

#define	HAMA_KEYIDLE_TIMEOUT		14400000		//	(no DB: 4hours) xdb_param_default_xxxx.c ... (model default value)
#define	HAMA_WAKEUP_DEFAULT			5000			//	Hama Wakeup every 5 sec

#if defined(WIN32)
#define DFBCHECK(x)                                                     \
               err = x;                                                    \
               if (err != DFB_OK) {                                        \
                    fprintf( stdout, "%s <%d>:\n\t", __FILE__, __LINE__ ); \
                    DirectFBErrorFatal( #x, err );                         \
               }
#else
#define DFBCHECK(x...)                                                     \
               err = x;                                                    \
               if (err != DFB_OK) {                                        \
                    fprintf( stdout, "%s <%d>:\n\t", __FILE__, __LINE__ ); \
                    DirectFBErrorFatal( #x, err );                         \
               }
#endif

#define	eHSIGSYS_DFB_READY				0x00000001		//	DFB Master생성 --> 필요없음.

#define	eHSIGSYS_APP_ADD				0x00000002
#define	eHSIGSYS_APP_READY				0x00000003
#define	eHSIGSYS_APP_REMOVE				0x00000004

#define	eHSIGSYS_WINDOW_ADD				0x00000005
#define	eHSIGSYS_WINDOW_GRABKEY			0x00000006
#define	eHSIGSYS_WINDOW_ACTIVE			0x00000007
#define	eHSIGSYS_WINDOW_HOOKEVENT		0x00000008
#define	eHSIGSYS_WINDOW_ACCEPTABLEKEY	0x00000009
#define	eHSIGSYS_WINDOW_SETSTYLE		0x0000000A
#define	eHSIGSYS_WINDOW_DEFAULTORDER	0x0000000B
#define	eHSIGSYS_WINDOW_CHANGEORDER		0x0000000C
#define	eHSIGSYS_WINDOW_SENDEVENT		0x0000000D
#define	eHSIGSYS_WINDOW_CONSUMED		0x0000000E

#define	eHSIGSYS_SIGNAL_BROADCAST		0x00010020
#define	eHSIGSYS_SIGNAL_TRIGGERBUFFER	0x00010021

#define	eHSIGSYS_SAWMAN_INPUTEVENT		0x10000000

#define	eHSIGSYS_INTERNAL_SOCKETREMOVE	0x20000000

#define	eHSIGSYS_APP_EXITED				0x40000000
#define	eHSIGSYS_APP_LAUNCHCONDITION	0x40000001

#define	eHSIGPRAM_REG_NOTI_CHANGED		0x00000000
#define	eHSIGPRAM_REG_NOTI_REMOVED		0x00000001

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HUINT32		ulMsg;
	HUINT32		ulParam1;
	HUINT32		ulParam2;
	HUINT32		ulParam3;
	HUINT32		ulParam4;
} HamaMsg_t;

typedef	struct
{
	pid_t		stProcessPid;
	HUINT32		ulApplId;
	pid_t		stHookPid;

	HxList_t	*acceptableKeyList;

	//	DFB Window Relatives
	HBOOL		bAlreadyRemovePlane;
	HINT32		nCurrentOpacity;
	HINT32		nLogicalOpacity;

	HWINDOWSTYLE_e	eWindowStyle;

	HINT32		nFocusPriority;
	HINT32		nStackPriority;
	HINT32		nPanelPriority;

	HUINT32				ulDFBWindowId;
	IDirectFBWindow		*dfbWindow;
	SaWManWindowHandle	sWindowHandle;
} HamaWindowInfo_t;

typedef	struct
{
	HCHAR		szProcessName[HAMA_PROCESS_NAME_LEN];
	pid_t		stProcessPid;
	HUINT32		ulStartTime;

	HINT32		nSocketHandle;

	HxList_t	*pstWindowList;					//	HamaWindowInfo_t
} HamaProcessInfo_t;

typedef	struct
{
	HBOOL		bWaitingReturn;
	HUINT32		ulLastEventSendTime;
	HUINT32		ulWaitingRequestId;

	HamaProcessInfo_t	*pstCurrentProcessInfo;
	HamaWindowInfo_t	*pstCurrentWindowInfo;
} HamaKeyProcessInfo_t;

typedef struct
{
	HINT32		pid;
	HCHAR		szProcessName[256];
} HamaReadyProcessInfo_t;

typedef	struct
{
	HBOOL			bInit;
	HUINT32			ulSemId;
	HUINT32			ulHamaEvtQId;
	HINT32			nServerHandle;
	HINT32			nRPCHandle;
	HxCBUF_t		nCBuf;

	HUINT32			ulLastKeyActionTime;					//	Last Key Action MONOTONIC TIME

	HxList_t		*pstEventList;
	HxHASH_t		*pstBufferedSignal;						//	Buffered Signal

	IDirectFB		*dfb;
	ISaWMan			*sMan;
	ISaWManManager	*sMgr;
	IDirectFBDisplayLayer	*layer;

	HxList_t				*pstAttachedProcessList;			//	HamaProcessInfo_t;
	HxList_t				*pstZombieWindowList;				//	Process생성전의 Window List관리.
	HxList_t				*pstWindowListFocusOrder;			//	HamaWindowInfo_t with Focus order.
	HxList_t				*pstKeyEventList;					//	Key Event List

	HamaKeyProcessInfo_t	stKeyProcessInfo;
	HxList_t				*pstReadyProcessList;

#ifdef	CONFIG_DEBUG
	struct
	{
		void		*pstObj;
		HxQueue_t	*pstJob;
	} stPool;
#endif
} HAMA_t;

typedef struct
{
	HUINT32	ulDummy;
} HAMA_EVT_t;
/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ***************************/
/*******************************************************************/
extern HERROR	HAMA_RPC_Register (HAMA_t *pstHama);

extern HERROR	HAMA_SCENARIO_Init(HCHAR *pszFileName);
extern HINT32	HAMA_SCENARIO_GetReadyAppList(HINT32 *pnProgIdList, HINT32 nMaxNum);
extern HERROR	HAMA_SCENARIO_AddReadyApp(HINT32 nProgId, HCHAR *readyAppName);
extern HERROR	HAMA_SCENARIO_SetMonitor(const HCHAR *pszProcName, HBOOL bMonitor);
extern HERROR	HAMA_SCENARIO_ProcessStartedSignal(const HCHAR *pszProcessName, HINT32 pid);
extern HERROR	HAMA_SCENARIO_ProcessStoppedSignal(pid_t pid, int st);
extern HERROR	HAMA_SCENARIO_RegisterProcessByLauncher(HCHAR *pszArg);
extern HERROR	HAMA_SCENARIO_RunModule(HAMA_t *pstHama,  const HCHAR *pszModules, void *userData);
extern HINT32	HAMA_SCENARIO_GetPriority(HWINID_e applId, HSTACKTYPE_e eStackType);
extern HBOOL	HAMA_SCENARIO_IsManagerControl(HWINID_e applId);
extern HBOOL	HAMA_SCENARIO_IsExternalAppId(HWINID_e applId);
extern HUINT32	HAMA_SCENARIO_GetLowerFocusAppId(HWINID_e applId);
extern HUINT32	HAMA_SCENARIO_GetLowerStackAppId(HWINID_e applId);

extern HamaProcessInfo_t*	HAMA_TOOL_GetProcessInfoByPid(HAMA_t *pstHama, HINT32 pid);
extern HamaProcessInfo_t*	HAMA_TOOL_GetProcessInfoByClient(HxList_t *pstProcessList, HINT32 nClientHandle);
extern HamaWindowInfo_t*	HAMA_TOOL_GetWindowInfoBySHandle(HxList_t *pstWindowList, HINT32 sHandle);
extern HamaWindowInfo_t*	HAMA_TOOL_GetWindowInfoByAppId(HxList_t *pstList, HWINID_e eWindowId);
extern HERROR				HAMA_TOOL_RemoveWindowInfoInProcess(HAMA_t *pstHama, HamaWindowInfo_t *pstWindowInfo);
extern HamaWindowInfo_t*	HAMA_TOOL_GetWindowInfoByPid(HxList_t *pstList, HINT32 nProgId);
extern HINT32	HAMA_TOOL_CompWindowFocusOrder(void *a, void *b);
extern HERROR	HAMA_TOOL_ConvertDFBKeyToHInputKey(DFBInputEvent *pstDFBKey, HInputEvent_t *pstInputKey);
extern HERROR	HAMA_TOOL_AddKeyEventToList(HAMA_t *pstHama, HInputEvent_t *pstInputEvent);
extern HERROR	HAMA_TOOL_SendInputEvent(HAMA_t *pstHama, HInputEvent_t *pstReturnEvent);
extern HERROR	HAMA_TOOL_ChangeFocusOrder(HAMA_t *pstHama, HUINT32 targetId, HUINT32 baseId, HRELATION_e eRelation);
extern HERROR	HAMA_TOOL_ChangeStackOrder(HAMA_t *pstHama, HUINT32 ulTargetId, HUINT32 ulBaseId, HRELATION_e eRelation);
extern void*	HAMA_TOOL_GetRPCBinary(HAMA_t *pstHama, HINT32 *pnSize, const HCHAR *pszSignature, ...);
extern void		HAMA_TOOL_SendWakeup(HAMA_t *pstHama);
extern void		HAMA_TOOL_ProcessMonitor(HBOOL bActive);
extern void	HAMA_HANDLER_MainLoop(void *arg);

extern void	HAMA_SAWMAN_ShowWindow(HAMA_t *pstHama, SaWManWindowHandle sHandle, HINT32 nOpacity);
extern void	HAMA_SAWMAN_HideWindow(HAMA_t *pstHama, SaWManWindowHandle sHandle, HINT32 nOpacity);
extern void	HAMA_SAWMAN_HideWindowWOCurrent(HAMA_t *pstHama, HUINT32 ulCurrentAppId);
extern void	HAMA_SAWMAN_ShowWindowWOCurrent(HAMA_t *pstHama, HUINT32 ulCurrentAppId);
extern void	HAMA_SAWMAN_Init(HAMA_t *pstHama);

extern HERROR	HAMA_AddEvent(HAMA_t *pstHama, HUINT32 ulEventId, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3, HUINT32 ulParam4);
extern HERROR	HAMA_GetEvent(HAMA_t *pstHama, HamaMsg_t *pstMsg);
extern HAMA_t*	HAMA_GetInstance(void);
extern void		HAMA_ReleaseInstance(HAMA_t *pstHama);

extern HAMA_t*		HAMA_InitInstance(void);

//	HAMA REGISTRY
extern HERROR	HAMA_REGISTRY_Init(HAMA_t *pstHama);
extern HERROR	HAMA_REGISTRY_SaveData(HAMA_t *pstHama);
extern HERROR	HAMA_REGISTRY_NotifyRegistry(HAMA_t *pstHama, HINT32 nClientHandle);
extern void 	HAMA_REGISTRY_GetAutomaticPowerdown(HUINT32* pTimeout);
#ifdef	CONFIG_DEBUG
extern void	HAMA_DEBUG_InitCommandTools(void);
extern HERROR	HAMA_REGISTRY_PrintCMD(HAMA_t *pstHama);
#endif

#endif

