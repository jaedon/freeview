/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xmgr_pvr_int.h
	@brief	  internal player ��� ����

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


// ������ Recording �ϴ� ����. (Record + TSR)
// ���� CONFIG���� ���� �ִ� �ɷ� ��ģ��.
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

	ePvrActType_PB				= 0x0100,	// ��׷� Play Back :
	ePvrActType_PB_Normal,
	ePvrActType_PB_Chase,
	ePvrActType_PB_Tsr,
	ePvrActType_PB_LiveStream,

	ePvrActType_REC				= 0x0200,	// ��׷� Record :
	ePvrActType_REC_Normal,					// Immediate Ȥ�� Time Reservation Recording
	ePvrActType_REC_EvtReserved,			// Event Reservation Recording
	ePvrActType_REC_Delayed,				// Delayed Recording
	ePvrActType_REC_ExtTsLine,				// HDD�� �ƴ϶� External TS Line�� Record
	ePvrActType_REC_MemBuffer,				// HDD�� �ƴ϶� �޸� ���۷� ��Ƽ� ������ ���
	ePvrActType_REC_Descramble,

	ePvrActType_TSR				= 0x0400,	// ��׷� TSR
	ePvrActType_TSR_Normal,
	ePvrActType_TSR_LiveStream,
	ePvrActType_TSR_Extern,					// OAPI ���� �䱸�� ���� ����Ǵ� TSR: ��� ���� �Ժη� start/stop�� ���� �ʴ´�.

	ePvrActType_SUBITEMMASK		= 0x00FF,
	ePvrActType_MASK			= 0xFF00,
	ePvrActType_ALL				= 0xFFFF
} XmgrPvr_ActionType_e;

typedef enum
{
	ePvrActState_None = 0,
	ePvrActState_WaitingByTimer,			// ���� ���� ���� �ƴ����� �� GWM Timer�� ���� ������ ����.
	ePvrActState_SettingUp,					// Action�� ��� ���� �غ��ϴ� ��.
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

	HUINT32					 ulActionId;			// �� PVR Action�� ���� Action ID
	HUINT32					 ulRelatedActId;		// �� PVR Action�� ���������� ���� �ִ� Action ID (TSR->WTV, Chase PB->REC, TSR BP->TSR ��)

	HUINT32					 ulRelatedViewId;		// �� PVR Action�� ���������� �����ִ� View ID (FCC ������ Action ID�� ���� �ٲ� �� �ִ� )   
	MgrAction_Type_e		 eRelatedActType;

	Handle_t				 hMasterSvcHandle;
	Handle_t				 hCurrSvcHandle;
	HUINT32					 ulTimerId;
	HUINT32					 ulPvrIdx;				// HDD ���� ������ ���õ� action �� destory �ϱ� ���� PvrIdx

	XmgrPvr_CopyStatus_e	 eCopyStatus;			// for delayed recording copy status
	HINT32					 nRefCount;				// �ϳ��� Action�� ���� ��û�� ������ ��� : ����μ��� TSR���� ��ȿ�ϴ�. 

	BUS_Callback_t			 pfActFunc;
} XmgrPvr_Action_t;

typedef struct
{
	HBOOL					 bTimeAvailable;
#if defined(CONFIG_MW_MM_DELAYED_REC)
	HBOOL					bTsrToBeStarted;        // delayed record file copy ������ TSR ���� ���� ��� ���߿� �ٽ� ������ �� �ֵ���
#endif

	XmgrPvr_DeviceState_e	 ePvrDevState[MAX_CONNECTABLE_PVR_STORAGE_NUM];
	SvcFs_DevIdx_e			 ePvrDevIdx[MAX_CONNECTABLE_PVR_STORAGE_NUM];
	HUINT32 				 ulPvrPartIdx[MAX_CONNECTABLE_PVR_STORAGE_NUM];

	// TSR Acception from Application
	HBOOL			 		 bTsrAutoTrack[NUM_VIEW_ACTION];

	XmgrPvr_Action_t		 astPbActItem[PVRMGR_ACTION_PB_NUM];
	XmgrPvr_Action_t		 astRecActItem[PVRMGR_ACTION_REC_NUM];		// TSR/Rec ���� ����
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

