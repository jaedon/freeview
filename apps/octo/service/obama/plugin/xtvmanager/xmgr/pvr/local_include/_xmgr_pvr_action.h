/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xmgr_pvr_int.h
	@brief	  internal player 헤더 파일

	Description: player internal use only. \n
	Module: AP/pvr			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__XMGR_PVR_ACTION_H___
#define	__XMGR_PVR_ACTION_H___

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <mgr_pvr_action.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/


// 실제로 Recording 하는 갯수. (Record + TSR)
// 차후 CONFIG에서 직접 넣는 걸로 고친다.
#define PVRMGR_MAX_TOTAL_RECORD						(CONFIG_MW_MM_MAX_REC_NUM + CONFIG_MW_MM_MAX_TSRREC_NUM)
#define PVRMGR_MAX_TSR								(CONFIG_MW_MM_MAX_TSRREC_NUM)
#define PVRMGR_MAX_NONTSR_REC						(CONFIG_MW_MM_MAX_REC_NUM)

#define PVRMGR_ACTION_PB_NUM						(NUM_VIEW_ACTION + NUM_BG_ACTION)
#define PVRMGR_ACTION_REC_NUM						(PVRMGR_MAX_TOTAL_RECORD)

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	ePvrDevState_NoDevice = 0,
	ePvrDevState_Malfunction,
	ePvrDevState_NeedToFormat,
	ePvrDevState_Formatting,
	ePvrDevState_Switching,

	ePvrDevState_OK		= 0x7F
} XmgrPvr_DeviceState_e;

typedef enum
{
	ePvrActType_None = 0,

	ePvrActType_PB				= 0x0100,	// 대그룹 Play Back :
	ePvrActType_PB_Normal,
	ePvrActType_PB_Chase,
	ePvrActType_PB_Tsr,
	ePvrActType_PB_LiveStream,

	ePvrActType_REC				= 0x0200,	// 대그룹 Record :
	ePvrActType_REC_Normal,					// Immediate 혹은 Time Reservation Recording
	ePvrActType_REC_EvtReserved,			// Event Reservation Recording
	ePvrActType_REC_Delayed,				// Delayed Recording
	ePvrActType_REC_ExtTsLine,				// HDD가 아니라 External TS Line에 Record
	ePvrActType_REC_MemBuffer,				// HDD가 아니라 메모리 버퍼로 잡아서 보내는 기능
	ePvrActType_REC_Descramble,

	ePvrActType_TSR				= 0x0400,	// 대그룹 TSR
	ePvrActType_TSR_Normal,
	ePvrActType_TSR_LiveStream,
	ePvrActType_TSR_Extern,					// OAPI 등의 요구로 인해 실행되는 TSR: 명령 없이 함부로 start/stop은 하지 않는다.

	ePvrActType_SUBITEMMASK		= 0x00FF,
	ePvrActType_MASK			= 0xFF00,
	ePvrActType_ALL				= 0xFFFF
} XmgrPvr_ActionType_e;

typedef enum
{
	ePvrActState_None = 0,
	ePvrActState_WaitingByTimer,			// 현재 동작 중은 아니지만 곧 GWM Timer에 의해 동작할 예정.
	ePvrActState_SettingUp,					// Action을 잡기 위해 준비하는 중.
	ePvrActState_Working,

	ePvrActState_MAX
} XmgrPvr_ActionState_e;

typedef enum
{
	ePvrActSrcType_Self,
	ePvrActSrcType_Action,

	ePvrActSrcType_EndOfCase
} XmgrPvr_ActionSrcType_e;

typedef enum
{
	ePvrCopyStatus_None = 0,
	ePvrCopyStatus_WaitingForStop,
	ePvrCopyStatus_Complete,
	ePvrCopyStatus_MAX
} XmgrPvr_CopyStatus_e;

typedef struct
{
	XmgrPvr_ActionState_e	 eState;
	XmgrPvr_ActionType_e	 eType;

	HUINT32					 ulActionId;			// 이 PVR Action이 잡은 Action ID
	HUINT32					 ulRelatedActId;		// 이 PVR Action과 직접적으로 연관 있는 Action ID (TSR->WTV, Chase PB->REC, TSR BP->TSR 등)

	HUINT32					 ulRelatedViewId;		// 이 PVR Action과 직접적으로 연관있는 View ID (FCC 때문에 Action ID는 자주 바뀔 수 있다 )   
	MgrAction_Type_e		 eRelatedActType;

	Handle_t				 hMasterSvcHandle;
	Handle_t				 hCurrSvcHandle;
	HUINT32					 ulTimerId;
	HUINT32					 ulPvrIdx;				// HDD 연결 해제시 관련된 action 만 destory 하기 위한 PvrIdx

	XmgrPvr_CopyStatus_e	 eCopyStatus;			// for delayed recording copy status
	HINT32					 nRefCount;				// 하나의 Action에 여러 요청이 공유될 경우 : 현재로서는 TSR만이 유효하다. 

	BUS_Callback_t			 pfActFunc;
} XmgrPvr_Action_t;

typedef struct
{
	HBOOL					 bTimeAvailable;
#if defined(CONFIG_MW_MM_DELAYED_REC)
	HBOOL					bTsrToBeStarted;        // delayed record file copy 때문에 TSR 시작 못한 경우 나중에 다시 시작할 수 있도록
#endif

	XmgrPvr_DeviceState_e	 ePvrDevState[MAX_CONNECTABLE_PVR_STORAGE_NUM];
	SvcFs_DevIdx_e			 ePvrDevIdx[MAX_CONNECTABLE_PVR_STORAGE_NUM];
	HUINT32 				 ulPvrPartIdx[MAX_CONNECTABLE_PVR_STORAGE_NUM];

	// TSR Acception from Application
	HBOOL			 		 bTsrAutoTrack[NUM_VIEW_ACTION];

	XmgrPvr_Action_t		 astPbActItem[PVRMGR_ACTION_PB_NUM];
	XmgrPvr_Action_t		 astRecActItem[PVRMGR_ACTION_REC_NUM];		// TSR/Rec 동시 관할
	XmgrPvr_Action_t		 astTrPbAction[NUM_BG_ACTION];
} XmgrPvr_Context_t;

typedef struct
{
	HUINT32					 ulRelatedViewId;
	Handle_t				 hMasterSvc;
	Handle_t				 hSuppleSvc;
	HINT32					 eSessionType;
	XmgrPvr_ActionType_e	 eTsrActionType;
	HUINT32					 ulStartTime;

	HERROR					 hResult;
} XmgrPvr_TsrRequest_t;

typedef HBOOL (* XmgrPvr_ActionFilter_Cb)(XmgrPvr_Action_t *pstActItem, XmgrPvr_Action_t *pstFilter);


/*******************************************************************/
/********************      Internal Functions Definition     ********************/
/*******************************************************************/
/*		PVR Player
 */
// xmgr_pvr_context.c
XmgrPvr_Context_t *	xmgr_pvr_GetContext (void);
HERROR				xmgr_pvr_ConvertAmActTypeToPvrActType (MgrAction_Type_e eAmActType, XmgrPvr_ActionType_e *pePvrActType);
HERROR				xmgr_pvr_ConvertPvrActTypeToAmActType (XmgrPvr_ActionType_e ePvrActType, MgrAction_Type_e *peAmActType);
XmgrPvr_Action_t *	xmgr_pvr_GetEmptyPvrAction (XmgrPvr_ActionType_e ePvrActType);
XmgrPvr_Action_t *	xmgr_pvr_FindPvrActionWithFilter (XmgrPvr_ActionType_e ePvrActType, XmgrPvr_Action_t *pstFilter, XmgrPvr_ActionFilter_Cb pfFilterFunc);
XmgrPvr_Action_t *	xmgr_pvr_FindPvrActionByServiceHandle (XmgrPvr_ActionType_e ePvrActType, MgrLiveView_SessionType_t eSessionType, Handle_t hMasterSvc, Handle_t hCurrSvc);
XmgrPvr_Action_t *	xmgr_pvr_FindPvrActionByActionId (XmgrPvr_ActionType_e ePvrActType, HUINT32 ulActionId);
XmgrPvr_Action_t *	xmgr_pvr_FindPvrActionByRelatedActionId (XmgrPvr_ActionType_e ePvrActType, HUINT32 ulActionId);
XmgrPvr_Action_t *	xmgr_pvr_FindPvrActionByRelatedViewId (XmgrPvr_ActionType_e ePvrActType, HUINT32 ulViewId);
XmgrPvr_Action_t *	xmgr_pvr_FindPvrActionByTimerId (XmgrPvr_ActionType_e ePvrActType, HUINT32 ulTimerId);
HERROR				xmgr_pvr_DestroyPvrAction (XmgrPvr_Action_t *pstAction);
HERROR				xmgr_pvr_ResetPvrAction (XmgrPvr_Action_t *pstAction);

// xmgr_pvr_action_base.c
HERROR			xmgr_pvr_Create_Base(void);
HERROR 			xmgr_pvr_StopPvrFunction_Base (HUINT32 ulPvrIdx);
HBOOL 			xmgr_pvr_CheckPVRDeivceUsed_Base (HUINT32 ulPvrIdx);
HERROR			xmgr_pvr_CheckPvrDevStatus_Base (HBOOL *pbIsAvailable);
HERROR			xmgr_pvr_DestroyPvrAction_BASE (XmgrPvr_Action_t *pstAction);


// Playback
HERROR			xmgr_pvr_StartTsrChasePlayback_Base (HUINT32 ulActionId, MgrPvr_TrickState_e eTrick, MgrPb_PlayType_e ePbPosOrigin, HUINT32 ulOffset, MgrPb_Option_e ePbOption);
HERROR			xmgr_pvr_StartPlayback_Base (HUINT32 ulContentId, HUINT32 ulStartTime);
HERROR			xmgr_pvr_MakeThumbnail_Base (HUINT32 ulContentId);
HERROR			xmgr_pvr_StartPlaybackTranscodeToRingBuf_Base (MgrPvr_PbTrcdToMemSetup_t *pstSetup, HUINT32 *pulActionId);
HERROR 			xmgr_pvr_StopPlaybackTranscodeToRingBuf_Base (HUINT32 ulActionId);
// Recording
HERROR			xmgr_pvr_GetProfileFromJSON_Base(const HCHAR *pProfileString, MgrTrcd_StreamProfile *pstProfile);
HERROR			xmgr_pvr_StartTrRecording_Base (HUINT32 ulContentId, HUINT32 ulRecMode, HUINT32 ulPvrIdx, HUINT32 *pulActionId);
HERROR			xmgr_pvr_CheckRecordPossible_Base (MgrRec_RecSetup_t *pstRecSetup, HUINT32 *pulActionId, HUINT32 *pulFailReason);
HERROR			xmgr_pvr_GetCurrentRecSvcHandles_Base (HUINT32 *pulRecNum, Handle_t *phSvcArray);
MgrRec_EndReasonType_e			xmgr_pvr_StartRecording_Base (MgrRec_RecSetup_t *pstRecSetup, HUINT32 *pulActionId);
HERROR			xmgr_pvr_GetTsrAutoTracking (HUINT32 ulViewId, HBOOL *pbTsrAutoTrack);
HERROR			xmgr_pvr_SetTsrAutoTracking (HUINT32 ulViewId, HBOOL bTsrAutoTrack);
HERROR			xmgr_pvr_StartTsrRecording_Base (HUINT32 ulRelatedViewId, Handle_t hMasterSvc, Handle_t hSuppleSvc, HINT32 eSessionType, MgrPvr_TsrType_e eTsrType, HUINT32 ulStartTime);
HERROR			xmgr_pvr_StopTsrRecording_Base (Handle_t hMasterSvc, Handle_t hSuppleSvc, MgrPvr_TsrType_e eTsrType);
HERROR			xmgr_pvr_GetTsrRecActionId_Base (Handle_t hSvcHandle, HUINT32 *pulActionId);
// Live Streaming
HERROR			xmgr_pvr_StartLiveStreaming_Base (Handle_t hSvcHandle, HUINT32 *pulActionId, Handle_t *phLiveStream);
HERROR			xmgr_pvr_StopLiveStreaming_Base (Handle_t hLiveStream);


#if defined(CONFIG_DEBUG)
HERROR			xmgr_pvr_PrintPvrStatus_Base (void);
#endif

// xmgr_pvr_tsr_base.c
HERROR			xmgr_pvr_CreateAutoTrackProc_Base (void);
HERROR			xmgr_pvr_DestroyAutoTrackProc_Base (void);

#endif /* __XMGR_PVR_ACTION_H___ */

