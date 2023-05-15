/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _xmgr_live_view.h
	@brief

	Description:			\n
	Module: 			 	\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	___XMGR_LIVE_VIEW_H___
#define	___XMGR_LIVE_VIEW_H___

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <bus.h>
#include <mgr_common.h>
#include <xmgr_live.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/** @brief WatchTV application state enumerates */
typedef enum
{
   	eLiveViewState_IDLE, 				/**< WTV가 아무것도 안하는 상태 */
	eLiveViewState_CH_STOP,				/**< 현재 튜닝중이던 서비스를 중단하기 위하여 채널 서비스에게 stop을 요청하고 기다리는 상태 */
	eLiveViewState_SVC_STOP,			/**< MW의 CH service가 stop된 후 나머지 SI, CAS, AV가 stop되기를 기다리는 상태 */
	eLiveViewState_SVC_LOCKED,			/**< 유저 Lock된 서비스의 PIN code 입력 대기 상태 */
	eLiveViewState_NO_SIGNAL,			/**< 튜닝중인 서비스의 신호가 없는 상태 */
	eLiveViewState_EMPTY_SVC,			/**< 현재 튜닝중인 서비스가 중단된 상태. PAT에서 삭제되었거나 PMT가 안들어오는 상태. */
	eLiveViewState_REMOVE_CH,			/**< 시청 중인 채널이 삭제된 상태 */
	eLiveViewState_PMT_PID_CHANGED,		/**< PMT 정보를 기다리는 상태 */
	eLiveViewState_SVC_NOT_RUNNING,		/**< SDT의 Running Status 가 NOT running으로 바뀌는 경우 or CanalReady에서 현재 RegionalService가 National로 변경되는 경우. */

/*************** 이 지점을 경계로 PID start된 경우와 아닌 경우로 나뉨 ****************/
	eLiveViewState_PID_START,			/**< PID start/stop의 경계를 표시하는 값. 이 state에 머무는 경우는 없어야 함. */
/*************** 이 지점을 경계로 PID start된 경우와 아닌 경우로 나뉨 ****************/

/*************** 이 지점을 경계로 Program Show된 경우와 아닌 경우로 나뉨 ****************/
	eLiveViewState_PROGRAM_SHOW,		/**< Program Show/Hide의 경계를 표시하는 값. 이 state에 머무는 경우는 없어야 함. */
/*************** 이 지점을 경계로 Program Show된 경우와 아닌 경우로 나뉨 ****************/

	eLiveViewState_AV,					/**< AV가 나오는 정상적인 TV시청 상태 */
	eLiveViewState_CAS_PREVIEW,			/**< CAS preview 상태 */
	eLiveViewState_CAS,					/**< CAS에 의하여 시청이 제어되고 있는 상태 */

	eLiveViewState_MAX
} XmgrLiveView_State_e;

/** @brief Multi instance 용 WatchTV context structure  */
typedef struct
{
	MgrLive_LiveMode_e	 		eLiveMode;
	XmgrLive_MwSvcExecPhase_e	aeServicesStartPhase[eLiveMwSvcType_MAX];
	XmgrLive_MwSvcState_e		aeServicesState[eLiveMwSvcType_MAX];

	HUINT32						ulActionId;					/* 이 context가 할당 받은 action ID */
	Handle_t					hAction;					/* 이 context의 현재 zapping session version이 포함된 action handle */
	HUINT32						ulMainActionId;				/* 이 context 생성시 알고 있는 main action ID. swap시 사용함. */

	XmgrLiveView_State_e		eAppState;					/* 이 context의 WatchTV application state */
	MgrLiveView_UiState_e		eUiState;					/* 이 context의 WatchTV UI state */
	HBOOL						bRadioBackground;			/* True if radio background is enabled */

	MgrLiveView_SessionType_t	sessionType;				/* Normal/Quiet/Replacement/MultiFeed 등 WTV session tuning type */

	Handle_t					hSvc; 						/* Service handle of this session */
	HINT32						nChNum;						/* Channel number of the this session */
	ChListUiGroup_t				svcListGroup;				/* 현재의 튜닝 그룹 */
	DbSvc_Info_t					stSvcInfo;					/* 현재 서비스 information */

	DbSvc_TsInfo_t				tsInfo;						/* 현재 TS information. 튜닝 delivery type이 들어 있음. */

/* +++++ ARIB closed caption 제어 정보 +++++ */
	HUINT16						usCaptionPid;				/* Closed caption PID. 일본향 사용 */
	HUINT16						usSuperPid;					/* Super impose PID. 일본향 사용 */
/* ----- ARIB closed caption 제어 정보 ----- */

/* +++++ Subtitle 제어 정보 +++++ */
	HBOOL						bSubtitleAllowed;			/* Subtitle 시작해도 된다는 flag. MENU에서 AUTO이거나 subtitle list popup이 start한 경우 TRUE */
	HBOOL						bTtxPageEnabled;			/* TELETEXT page를 보여주고 있음. Subtitle은 enable될 수 없음. */
	SvcSi_SubtitleInfo_t				subtitleInfo;				/* svcInfo에 있는 index로 PMT에서 골라낸 subtitle info */
	HUINT32						nDVBSbtCPageID;				/*subtitle에서 page id 를 임의로 설정할 경우 사용함. */
	HUINT32						nDVBSbtAPageID;
	HUINT32						nEBUSbtMagNum;
	HUINT32						nEBUSbtPageNum;
/* ----- Subtitle 제어 정보 ----- */

	HBOOL						bSiStopped;					/* SI session를 stop 했는가? */
	HBOOL						bCasStopped;				/* CAS session를 stop 했는가? */
	HBOOL						bSubtitle_CC_Stopped;		/* Subtitle or Closed caption session을 stop 했는가? */
	HBOOL						bTeletextStopped;			/* Teletext session를 stop 했는가? */
	HBOOL						bPmtValid;					/* PMT가 전달됨. */


/* DRM & Copy protection management */
	// TODO: siDrmInfo/casDrmInfo 따로 둘 필요 없다.
	HBOOL						bSiDrmValid;				/* SI로부터 Copy protection 정보가 detect 됨. */
	DxCopyrightControl_t					drmInfo;					/* SI drm info */
	HBOOL						bCasDrmValid;				/* CAS로부터 Copy protection 정보가 detect 됨. SI보다는 CAS가 우선 순위가 있음.*/

/* CAS management */
	HBOOL						bCasOk;						/* CAS에게 시청 허가를 받았는가? */
	HBOOL						bSCPinCodeValid;			/* Smart card Pin code가 사용 가능한가? */
	DxCAS_GroupId_e				nCasSystemId;				/* EVT_CAS_FAIL메시지의 p1으로 전달되는 CAS system ID (Nagra, Ir, Conax, ...) */
	void						*pCasInfo;					/* EVT_CAS_FAIL메시지의 p2로 전달되는 CAS proc에서 사용할 data point */
	HINT32						nCasEvt;					/* CAS proc에게 넘겨줄 CAS event (eSEVT_CAS_OK, eSEVT_CAS_FAIL) */
	SvcCas_ShowType_e				nCasShow;					/* Audio, video에 대한 각각의 subscription 상태 */
	HBOOL						bAudioShow;					/* Audio subscription 상태 */
	HBOOL						bVideoShow;					/* Video subscription 상태 */

/* AV decoder management */
	HBOOL						bAvUnderrun;				/* AV underrun flag */
	HBOOL						bHdmiBlocked;				/* HDMI was blocked by HDCP fail or etc. */

/* MHEG 등 데이터 방송용 support items */
	HUINT16 					usOrgVideoPid;				/* temp로 바뀌기 전의 original video PID */
	DxVideoCodec_e				eOrgVideoCodec;				/* temp로 바뀌지 전의 original video codec */
	HUINT16 					usOrgAudioPid;				/* temp로 바뀌기 전의 original audio PID */
	HUINT16 					usOrgDolbyPid;				/* temp로 바뀌기 전의 original Dolby audio PID */
	DxAudioCodec_e				eOrgAudioCodec;				/* temp로 바뀌지 전의 original audio codec */
	DxAudioCodec_e				eOrgDolbyCodec;				/* temp로 바뀌지 전의 original Dolby audio codec */
	HUINT16 					usOrgPcrPid;				/* temp로 바뀌기 전의 original pcr PID */

	HUINT16 					usOrgCaptionPid;			/* temp로 바뀌기 전의 original caption PID. 일본향에서 사용 */
	HUINT16 					usOrgSuperPid;				/* temp로 바뀌기 전의 original super PID. 일본향에서 사용 */

/* Supplementary Service support items */
	Handle_t					hMasterSvc;					/* Supplement service 일 경우에 master handle backup. */
	DbSvc_Info_t					stMasterSvcInfo;				/* Master service의 service information backup */

/* Replacement Service support items */
	HUINT32						ulReplaceMessage;			/* Replacement service event 메시지 기록 */
	Handle_t					hSuppleSvc;					/* Supplement service save for replacement service */

	SvcRsc_Info_t					stRscInfo;


/*
	여기서부터 WatchTv에서 Copy 하지 않은 Original Arguments 들.
*/
	MgrLive_START_t				stStartArg;
	HBOOL						bUnselectVideo;
	HBOOL						bUnselectAudio;
	HBOOL						bBlockUiStateNoti;	// ui state noti를 context setup하는 동안 막는 flag

	/*
		etc..
	*/
	HUINT32						ulLastTuneStatus;	// fcc에서 사용하는 last tune status
} XmgrLiveView_Context_t;


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
HERROR			XMGR_LiveView_BASE_Init (void);
BUS_Callback_t 	XMGR_LiveView_BASE_GetProcByActionId (HUINT32 ulActionId);

BUS_Result_t 	XMGR_LiveView_BASE_MsgBusCreate(XmgrLiveView_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t 	XMGR_LiveView_BASE_MsgBusDestroy(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t 	XMGR_LiveView_BASE_MsgLiveStartSvc (XmgrLiveView_Context_t *pstContext, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t 	XMGR_LiveView_BASE_MsgLiveStopSvc (XmgrLiveView_Context_t *pstContext, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3);

BUS_Result_t 	XMGR_LiveView_BASE_MsgLiveLockSvcLocked (XmgrLiveView_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t 	XMGR_LiveView_BASE_MsgLiveLockSvcUnlocked (XmgrLiveView_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

BUS_Result_t 	XMGR_LiveView_BASE_EvtChStopped(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t 	XMGR_LiveView_BASE_EvtSiStopped(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t 	XMGR_LiveView_BASE_EvtCasStopped(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t 	XMGR_LiveView_BASE_EvtAvStopped(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3);

BUS_Result_t 	XMGR_LiveView_BASE_EvtChNoSignal(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t 	XMGR_LiveView_BASE_EvtChMovingMotor(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t 	XMGR_LiveView_BASE_EvtChLocked(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3);

BUS_Result_t 	XMGR_LiveView_EvtSiDrmSignalled(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3);

BUS_Result_t 	XMGR_LiveView_BASE_EvtSiPmt(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t 	XMGR_LiveView_BASE_EvtSiPmtTimeout(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t 	XMGR_LiveView_BASE_EvtSiPmtRemoved(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t 	XMGR_LiveView_BASE_EvtSiPmtChanged(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t 	XMGR_LiveView_BASE_EvtSiServiceMove(XmgrLiveView_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

BUS_Result_t 	XMGR_LiveView_BASE_EvtAvVideoUnderrun(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t 	XMGR_LiveView_BASE_EvtAvVideoStarted(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t 	XMGR_LiveView_BASE_EvtAvAudioUnderrun(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t 	XMGR_LiveView_BASE_EvtAvAudioStarted(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3);

#if defined(CONFIG_MW_CAS)
BUS_Result_t 	XMGR_LiveView_BASE_EvtCasOk(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t 	XMGR_LiveView_BASE_EvtCasFail(XmgrLiveView_Context_t *pstContext, HUINT32 event, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t 	XMGR_LiveView_BASE_EvtCasSystemInfoUpdated(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t 	XMGR_LiveView_BASE_EvtCasDecodeForceTune(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t 	XMGR_LiveView_BASE_EvtCasScRemoved(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3);
#endif

BUS_Result_t 	XMGR_LiveView_BASE_EvtDbCheckSvcDb(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t 	XMGR_LiveView_BASE_MsgMgrSvcPidChanged(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t 	XMGR_LiveView_BASE_MsgMgrChangeTempPid(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t 	XMGR_LiveView_BASE_MsgMgrActionMainChanged(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t 	XMGR_LiveView_BASE_EvtSiSdtRunningStatusChanged(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t 	XMGR_LiveView_BASE_EvtSiNetworkChangedNotify(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t 	XMGR_LiveView_BASE_MsgLiveSetMainviewAction(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t 	XMGR_LiveView_BASE_MsgLiveSetFccMainviewAction(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t    XMGR_LiveView_BASE_FccTuneStatus(XmgrLiveView_Context_t  *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t 	XMGR_LiveView_BASE_EvtTranscoderToEvtLiveStream(HINT32 nEvent, XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t 	XMGR_LiveView_BASE_MsgLiveUnselectComponent(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t 	XMGR_LiveView_BASE_MsgBusGoStandby(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	XMGR_LiveView_BASE_MsgBusTimer(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	XMGR_LiveView_BASE_MsgMhegStatusChange(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3);

HERROR 			XMGR_LiveView_BASE_GetSessionType(HUINT32 ulActionId, MgrLiveView_SessionType_t *pSessionType);
HERROR 			XMGR_LiveView_BASE_GetRingBufHandle(HUINT32 ulActionId, Handle_t *phRingBuf);
HERROR			XMGR_LiveView_BASE_GetProfileFromJSON(const HCHAR *pProfileString, MgrLive_LiveStreamArg_t *pstDestArg);

HERROR 			XMGR_LiveView_BASE_GetSvcInfo (HUINT32 ulActionId, MgrLiveView_PlaySvc_e eSvcType, DbSvc_Info_t *pstMasterSvcInfo);
HERROR 			XMGR_LiveView_BASE_GetServiceHandle(HUINT32 ulActionId, MgrLiveView_PlaySvc_e eSvcType, Handle_t *phSvc);
Handle_t 		XMGR_LiveView_BASE_GetMasterSvcHandle(HUINT32 ulActionId);
HERROR 			XMGR_LiveView_BASE_GetTsInfo(HUINT32 ulActionId, DbSvc_TsInfo_t *pTsInfo);
HERROR 			XMGR_LiveView_BASE_GetLastTuneStatus(HUINT32 ulActionId, HUINT32 *pulLastTuneStatus);

HERROR 			XMGR_LiveView_BASE_GetState(HUINT32 ulActionId, MgrLiveView_UiState_e *peViewState);
void 			XMGR_LiveView_BASE_ShowProgram(XmgrLiveView_Context_t *pstContext);
HERROR 			XMGR_LiveView_BASE_StopMwService(XmgrLiveView_Context_t *pstContext, XmgrLive_MwSvcType_e eMwSvcType, ApiSyncMode_t eSync);
void 			XMGR_LiveView_BASE_SetAppState(XmgrLiveView_Context_t *pstContext, XmgrLiveView_State_e eAppState);

HERROR			XMGR_LiveView_BASE_GetLiveMode (HUINT32 ulActionId, MgrLive_LiveMode_e *peLiveMode);

XmgrLiveView_Context_t*		XMGR_LiveView_BASE_GetContext (HUINT32 ulActionId);
HERROR			XMGR_LiveView_BASE_InitContext (void);
HERROR 			XMGR_LiveView_BASE_ResetUiState(HUINT32 ulActionId, HBOOL bSvcStart);

#if defined (CONFIG_DEBUG)
int 			XMGR_LiveView_BASE_DBG_DumpContext(void *context);
int 			XMGR_LiveView_BASE_DBG_PrintContext(void);
int 			XMGR_LiveView_BASE_DBG_PmtReload(HUINT32 ulActionId, HBOOL bPmtTimeout);
int 			XMGR_LiveView_BASE_DBG_PrintCasDrmInfo(void);
#endif

#endif /* !___XMGR_LIVE_VIEW_H___ */

