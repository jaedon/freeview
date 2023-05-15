/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  hapi.h
	@brief

	Description:  									\n
	Module: HaMa Control APIs Header				\n

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

#ifndef	__HAPI_H__
#define	__HAPI_H__

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
//#define	HAMA_SIGNAL_UDS				"/tmp/.hama_signal.tmp"

#define	HAMA_ASSERT(expr)			\
	if (!(expr)) {\
		HxLOG_Error("HAMA_ASSERT : Argument is not valid!!! \n");\
		abort();\
	}

#define HAMA_Malloc			HLIB_STD_MemAlloc
#define HAMA_Calloc			HLIB_STD_MemCalloc
#define	HAMA_Realloc		HLIB_STD_MemRealloc
#define	HAMA_StrDup			HLIB_STD_StrDup
#define	HAMA_MemDup			HLIB_STD_MemDup
#define HAMA_Free			HLIB_STD_MemFree
#define	HAMA_Malloc_CB		HLIB_STD_MemAlloc_CB
#define	HAMA_Free_CB		HLIB_STD_MemFree_CB

#define	HAMA_TOOL_CONVDFB2HINPUT(b, a)					\
	{													\
		(a)->window_id	= 0;							\
		(a)->request_id	= 0;							\
		(a)->eInputReturnType = eHINPUTRET_NONE;		\
		/*	Same With DFBInputEvent. */					\
		(a)->type		= (b)->type;					\
		(a)->device_id	= (b)->device_id;				\
		(a)->flags		= (b)->flags;					\
		(a)->key_code	= (b)->key_code;				\
		(a)->key_symbol	= (b)->key_symbol;				\
		(a)->modifiers	= (b)->modifiers;				\
		(a)->locks 		= (b)->locks;					\
		(a)->button		= (b)->button;					\
		(a)->buttons	= (b)->buttons;					\
		(a)->axis		= (b)->axis;					\
		(a)->axisabs	= (b)->axisabs;					\
		(a)->axisrel	= (b)->axisrel;					\
		(a)->min		= (b)->min;						\
		(a)->max		= (b)->max;						\
	}

#define HAMA_MAX_MONITOR_PROCESS_NUM (15)

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef	enum
{
	eAWINDOW_NOT_HANDLED		= 0x00000000,
	eAWINDOW_HAMA,
	eAWINDOW_DAMA,
	eAWINDOW_TR069,
	eAWINDOW_PAMA,
	eAWINDOW_GWMPOPUP,
	eAWINDOW_KEYBOARD,
	eAWINDOW_MHEGPLAYER,
	eAWINDOW_MHEGPLAYER_IMG_STILL,
	eAWINDOW_SUBTITLEPLAYER,
	eAWINDOW_JAPAN_NATIVE_UI,
	eAWINDOW_ARIBCC_SUP,
	eAWINDOW_ARIBCC_CAP,
	eAWINDOW_BML,
	eAWINDOW_BMLSTILL,
	eAWINDOW_NETFLIXMANAGER,
	eAWINDOW_NETFLIX,
	eAWINDOW_HBBTV,
	eAWINDOW_OIPF,
	eAWINDOW_OTVB,
	eAWINDOW_OTVS,
	eAWINDOW_BSAUTOMSG,
	eAWINDOW_MHEGIB,
	eAWINDOW_SCREENSAVER,
	eAWINDOW_RFVOD,
	eAWINDOW_ANDROID,
	eAWINDOW_WIND3,

	eAWINDOW_TESTAPP,

	eAWINDOW_OTHER,			//	not defined application
	eAWINDOW_MAX
} HWINID_e;

typedef	enum
{
	eHRELATION_UP,
	eHRELATION_DOWN
} HRELATION_e;

typedef	enum
{
	eHWS_WindowInputEnable		= 0x00000001,			//	자동으로 Mapping되나 변경할수 있다.
	eHWS_WindowNoSurface		= 0x00000002,			//	자동으로 Mapping되나 변경할수 있다.
	eHWS_WindowInputAlways		= 0x00000004,
	eHWS_WindowInputFilter		= 0x00000008,
	eHWS_WindowOwnControl		= 0x00000010,

	eHWS_NONE					= 0x00000000
} HWINDOWSTYLE_e;

//	same with DFBInputEvent
typedef enum
{
	eHINPUTRET_NONE		= 0,
	eHINPUTRET_PASS,
	eHINPUTRET_CONSUMED
} HInputRetType_e;

typedef	enum
{
	eHSTACK_FOCUS,
	eHSTACK_WINDOW,
	eHSTACK_PANEL,
	eHSTACK_MAX
} HSTACKTYPE_e;

typedef struct
{
	HUINT32	window_id;				//	target window
	HUINT32	request_id;				//	request id
	HInputRetType_e	eInputReturnType;	//	input event return type;

	HUINT32	type;
	HUINT32	device_id;
	HUINT32	flags;

	HINT32	key_code;
	HINT32	key_id;
	HINT32	key_symbol;
	HINT32	modifiers;

	HINT32	locks;
	HINT32	button;
	HINT32	buttons;

	HINT32	axis;
	HINT32	axisabs;
	HINT32	axisrel;

	HINT32	min;
	HINT32	max;
} HInputEvent_t;

typedef HERROR	(* CBHAPIRegHandler) (void *pvUserData, HBOOL bRemoved);

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************     Function Prototype    ********************/
/*******************************************************************/

//	??? will be moved to global header configuration.
typedef enum
{
	eHBootUpReason_POWER,
	eHBootUpReason_TIMER,
	eHBootUpReason_USER,
	eHBootUpReason_UNKNOWN
} HBootUpReason_e;

typedef enum
{
	eHSystemOperationMode_NORMAL,
	eHSystemOperationMode_SLEEP,
	eHSystemOperationMode_UNKNOWN
} HSystemOperationMode_e;

typedef struct
{
	HBootUpReason_e			eBootUpReason;
	HSystemOperationMode_e	eSystemCurrentMode;
} HSystemBootupStatus_t;

typedef struct
{
	HUINT32 unUsedMemSizeMB;
	HUINT32 unMemRate;
	HUINT32 unCpuRate;
	HCHAR	szProcessName[32];
}HSystemProcessInfo_t;

typedef struct
{
	HUINT32 unCpuRate;
	HUINT32 unMemRate;
	HUINT32 unMemFreeSizeMB;
	HSystemProcessInfo_t stProcess[HAMA_MAX_MONITOR_PROCESS_NUM];
}HSystemMointorInfo_t;

typedef void	(* CBHAPIMonitorHandler) (HSystemMointorInfo_t *pInfo);


/*****************************************************************************
	HaMa API description
 *****************************************************************************/
extern HERROR	HAPI_Init(void);
extern HERROR	HAPI_HookKeyEvent(HWINID_e eAppId, HBOOL bHooked);
extern HERROR	HAPI_ReturnKeyEvent(HWINID_e eAppId, HInputRetType_e eRetType, HInputEvent_t *pEvent);
extern HBOOL	HAPI_GetInputEvent(HWINID_e eApplId, HInputEvent_t *pstInputEvent, HINT32 nTimeout);
extern HERROR	HAPI_AddVirtualWindow(HWINID_e eAppId);
extern void		HAPI_SetApplicationScenarioDefault(HWINID_e eApplId);
extern HERROR	HAPI_SetApplicationFocusOrder(HWINID_e eTargetId, HWINID_e eBaseId, HRELATION_e relation);
extern HERROR	HAPI_SetApplicationWindowOrder(HWINID_e eTargetId, HWINID_e eBaseId, HRELATION_e relation);
extern HERROR	HAPI_RegisterAcceptableKey(HWINID_e eTargetId, HUINT32 eSymbol, HBOOL bRegister);
extern HERROR	HAPI_SetWindowStyle(HWINID_e eTargetId, HWINDOWSTYLE_e eWindowStyle, HBOOL bClear);
extern HERROR	HAPI_DeleteMyApplication(pid_t pid);
extern HERROR	HAPI_SendReadySignal(pid_t pid);
extern HERROR	HAPI_SendDFBReadySignal(void);
extern HERROR	HAPI_SendKeyGrab(HWINID_e eWindowId, HBOOL bGrab);
extern HERROR	HAPI_SendWindowActive(HWINID_e eWindowId, HBOOL bActive);
extern HERROR	HAPI_SendRemoteRCUKey(HUINT32 ulKeySymbol);
extern HERROR	HAPI_SendRemoteRCUKeyRelease(HUINT32 ulKeySymbol);
extern HERROR	HAPI_SendRemoteKey(void *pvWindowEvent);
extern HERROR	HAPI_DisconnectSignal(const HCHAR *pszSignalName, HxRPC_Noti_t cbRpcNotifier);
extern HERROR	HAPI_ConnectSignal(const HCHAR *pszSignalName, void *pvUserData, HxRPC_Noti_t cbRpcNotifier);
extern HERROR	HAPI_ConnectSystemSignal(const HCHAR *pszSignalName, const HCHAR *pszSignature, void *pvUserData, HxRPC_Noti_t cbRpcNotifier);
extern HERROR	HAPI_BroadcastSignal(const HCHAR *pszSignalName, HBOOL bBuffered, const HCHAR *pszSignature, ...);

extern HERROR	HAPI_RemoveRegistry(void);
extern HERROR	HAPI_SetRegistryListener(void *pvUser, CBHAPIRegHandler pfnRegHandler);
extern HERROR	HAPI_SetRegistryEx(const HCHAR *pszKey, HxObjectTag_e eType, const void *pvData, HINT32 nSize);
extern HERROR	HAPI_GetRegistryEx(const HCHAR *pszKey, HxObjectTag_e eType, void *pvData, HINT32 nMax);
extern HERROR	HAPI_SetRegistryCvtStr(const HCHAR *pszKey, const HCHAR *pszTagType, HINT32 nValue, const HCHAR* (*cvtInt2Str)(const HCHAR *, HINT32));
extern HERROR	HAPI_GetRegistryCvtStr(const HCHAR *pszKey, const HCHAR *pszTagType, HINT32 *pnEnum, HINT32 (*cvtStr2Int)(const HCHAR *, const HCHAR *));

extern HERROR 	HAPI_SetSysMonitor(HBOOL bActive);
extern HERROR 	HAPI_SysMonitorListener(CBHAPIMonitorHandler pfnListener);


#define	HAPI_SetRegistryInt(pszKey, nValue)			\
				HAPI_SetRegistryEx(pszKey, HxOBJECT_TYPE_INT, (HINT32*)nValue, 4)

#define	HAPI_SetRegistryStr(pszKey, pszValue)		\
				HAPI_SetRegistryEx(pszKey, HxOBJECT_TYPE_STR, (pszValue == NULL) ? "" : pszValue, (pszValue == NULL) ? 2 : HxSTD_StrLen(pszValue) + 1)

#define	HAPI_GetRegistryInt(pszKey, pnValue)		\
				HAPI_GetRegistryEx(pszKey, HxOBJECT_TYPE_INT, (void*)pnValue, 4)

#define	HAPI_GetRegistryStr(pszKey, pszValue, nMax)	\
				HAPI_GetRegistryEx(pszKey, HxOBJECT_TYPE_STR, (void*)pszValue, nMax)

#define	HAPI_SetRegistryEnum(pszKey, pszTagType, nEnum)			\
			HAPI_SetRegistryCvtStr(pszKey, pszTagType, (HINT32)nEnum, DLIB_ConvertEnum2Str)
#define	HAPI_GetRegistryEnum(pszKey, pszTagType, pnEnum)		\
			HAPI_GetRegistryCvtStr(pszKey, pszTagType, pnEnum, DLIB_ConvertStr2Enum)

#ifdef __cplusplus
}
#endif

#endif

#if	0
//	SIGNAL은 NORMAL과 BUFFERED로 나누어 짐.
//	BUFFERED SIGNAL는 HAMA에서 마지막 상태를 기억하고 새로운 Process가 Attach후 Connection시도 시
//	마지막 상태를 알려줌.
typedef	enum
{
	//	0 BUFFERED SYSTEM SIGNAL
	eSSIG_PanelPriority,			//	Scenario로 부터 panel priority를 부여 받는다.
	/* hama:onPanelPriority */
	eSSIG_InputNotify,				//	System Monitor, Input 왔을때 Connection된 곳으로 Notifying해준다.
	/* hama:onInputNotify */
	eSSIG_AttachedNotify,			//	System Monitor, Window가 Attach되었을 때 Connection된 곳으로 Notifying해준다.

	//	3 BUFFERED APPS SIGNAL
	eHSIG_ShowSubtitle,				//	Configuration for system
	/* signal:onSubtitleState */
	eHSIG_ShowTeleText,
	/* signal:onTeletextState */
	eHSIG_VariableFont,
	/* signal:onFontTypeChanged */
	eHSIG_CountryID,
	/* signal:onCountryIdChanged */
	eHSIG_SelectSubtitleType,		//  DBV/TSR == 0   EBU == 1
	/* signal:onSubtitleTypeChanged */

	//	8 deprecated
	eHSIG_Mount,
	/* signal:onStorageMounted */
	eHSIG_UMount,
	/* signal:onStorageUnmounted */

	eHSIG_AppsReady,				//	eHSIGSYS_READY_APP 이 HAMA에 도착하면 eHSIG_AppsReady를 다시 전파한다.
	/* signal:onApplicationReady */
	eHSIG_SystemBootupState,		//	System Bootup status를알려준다. HSystemBootupStatus_t 해당 구조체 참고.
	/* signal:onBootupStateChanged */
	eHSIG_SystemState,				//	system mode {ePAMA_STATUS_Undefined, ePAMA_STATUS_Operation, ePAMA_STATUS_Reboot, ePAMA_STATUS_Shutdown}
	/* signal:onSystemStateChanged */
	eHSIG_EndOfBuffered,			//	아래의 Signal들은 버퍼링 되지 않는다.

	//	14 NORMAL APPS SIGNAL
	eHSIG_HWindowEvent,				//	For HWindowEvent (key event and hevent...)
	/* signal:onHWindowEvent */
	eHSIG_NetworkConnected,
	/* signal:onNetworkConnected */
	eHSIG_StartPortalAppl,
	/* signal:onStartPortal */
	eHSIG_StopPortalAppl,
	/* signal:onStopPortal */
	eHSIG_ProcessKilled,
	eHSIG_Mute,

	eHSIG_StartFullBrowser,			// 	Start FullBrowser
	/* signal:onStartFullBrowser */
	eHSIG_SurferShutdown,			//	FullBrowser is killed
	/* signal:onSurferShutdown */
	eHSIG_StopFullBrowser,			// 	Exit FullBrowser
	/* signal:onStopFullBrowser */

	eHSIG_KeyboardMode,				//	password or plain
	/* signal:onOSKModeChanged */
	eHSIG_KeyboardCharacterMode,		//	카타카나 or 히라가나 or 영문 or 숫자.
	/* signal:onOSKCharModeChanged */
	eHSIG_KeyboardInitText,			//	set initial text
	/* signal:onOSKDefaultString */
	eHSIG_KeyboardInitPosition,		//	편집할 초기 위치.
	/* signal:onOSKInitPosition */
	eHSIG_KeyboardMaxLength,		//	입력 가능한 최대 문자 길이 (바이트)
	/* signal:onOSKMaxLengthChanged */
	eHSIG_ShowKeyboard,			// 	키보드를 띄우거나.
	/* signal:onOSKShow */
	eHSIG_DestroyKeyboard,			// 	강제 종료 시키거나.
	/* signal:onOSKDestroy */
	eHSIG_KeyboardResult,			// 	keyboard 에서 입력 받은 string을 전달함
	/* signal:onOSKResult */
	eHSIG_KeyboardCanceled,		// 	입력 도중에 취소 되었음.
	/* signal:onOSKCancel */

	eHSIG_ShowBsAutoMsg,
	/* signal:onShowBsAutoMsg */

	eHSIG_ShowSimpleKeyboard,
	/* signal:onShowSimpleKeyboard */
	eHSIG_SimpleKeyboardStr,
	/* signal:onSimpleKeyboardStr */

	eHSIG_RemoteAppKeyboardString,
	/* signal:onRemoteAppString */

	/* screen saver 관련 명령어 */
	eHSIG_ScreenSaverStart,
	eHSIG_ScreenSaverEnd,

	/* ipepg 관련 명령어 */
	eHSIG_FactoryDefault,
	/* signal:onFactoryDefault */
	eHSIG_NewChannelSaved,
	/* signal:onNewServiceIsAdded */

	eHSIG_IpEpgOnOff,
	/* signal:onIpEPGStateChanged */
	eHSIG_RpOnOff,
	/* signal:onRpStateChanged */
	eHSIG_RpInterval,
	/* signal:onRpIntervalChanged */
	eHSIG_RpPeriodStart,
	/* signal:onRpPeriodChanged */
	eHSIG_RpPeriodEnd,
	/* signal:onRpPeriodChanged */

	eHSIG_HbbTVApplicationStart,
	/* signal:onHbbTVAppStart */
	eHSIG_RunTeleText,
	/* signal:onTeletextRun */

	eHSIG_Format,
	/* signal:onFormatOccurred */

	eHSIG_SystemLanguageChanged,
	/* signal:onSystemLanguageChanged */

	eHSIG_ReadyMainApplication,		//	Main Application이 Ready상태 임을 알려 준다.
	/* signal:onMainApplicationReady */

	eHSIGNAL_END
} HSIGNAL_e;
#endif

