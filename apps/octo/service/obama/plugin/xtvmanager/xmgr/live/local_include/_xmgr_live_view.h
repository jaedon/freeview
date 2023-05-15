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
   	eLiveViewState_IDLE, 				/**< WTV�� �ƹ��͵� ���ϴ� ���� */
	eLiveViewState_CH_STOP,				/**< ���� Ʃ�����̴� ���񽺸� �ߴ��ϱ� ���Ͽ� ä�� ���񽺿��� stop�� ��û�ϰ� ��ٸ��� ���� */
	eLiveViewState_SVC_STOP,			/**< MW�� CH service�� stop�� �� ������ SI, CAS, AV�� stop�Ǳ⸦ ��ٸ��� ���� */
	eLiveViewState_SVC_LOCKED,			/**< ���� Lock�� ������ PIN code �Է� ��� ���� */
	eLiveViewState_NO_SIGNAL,			/**< Ʃ������ ������ ��ȣ�� ���� ���� */
	eLiveViewState_EMPTY_SVC,			/**< ���� Ʃ������ ���񽺰� �ߴܵ� ����. PAT���� �����Ǿ��ų� PMT�� �ȵ����� ����. */
	eLiveViewState_REMOVE_CH,			/**< ��û ���� ä���� ������ ���� */
	eLiveViewState_PMT_PID_CHANGED,		/**< PMT ������ ��ٸ��� ���� */
	eLiveViewState_SVC_NOT_RUNNING,		/**< SDT�� Running Status �� NOT running���� �ٲ�� ��� or CanalReady���� ���� RegionalService�� National�� ����Ǵ� ���. */

/*************** �� ������ ���� PID start�� ���� �ƴ� ���� ���� ****************/
	eLiveViewState_PID_START,			/**< PID start/stop�� ��踦 ǥ���ϴ� ��. �� state�� �ӹ��� ���� ����� ��. */
/*************** �� ������ ���� PID start�� ���� �ƴ� ���� ���� ****************/

/*************** �� ������ ���� Program Show�� ���� �ƴ� ���� ���� ****************/
	eLiveViewState_PROGRAM_SHOW,		/**< Program Show/Hide�� ��踦 ǥ���ϴ� ��. �� state�� �ӹ��� ���� ����� ��. */
/*************** �� ������ ���� Program Show�� ���� �ƴ� ���� ���� ****************/

	eLiveViewState_AV,					/**< AV�� ������ �������� TV��û ���� */
	eLiveViewState_CAS_PREVIEW,			/**< CAS preview ���� */
	eLiveViewState_CAS,					/**< CAS�� ���Ͽ� ��û�� ����ǰ� �ִ� ���� */

	eLiveViewState_MAX
} XmgrLiveView_State_e;

/** @brief Multi instance �� WatchTV context structure  */
typedef struct
{
	MgrLive_LiveMode_e	 		eLiveMode;
	XmgrLive_MwSvcExecPhase_e	aeServicesStartPhase[eLiveMwSvcType_MAX];
	XmgrLive_MwSvcState_e		aeServicesState[eLiveMwSvcType_MAX];

	HUINT32						ulActionId;					/* �� context�� �Ҵ� ���� action ID */
	Handle_t					hAction;					/* �� context�� ���� zapping session version�� ���Ե� action handle */
	HUINT32						ulMainActionId;				/* �� context ������ �˰� �ִ� main action ID. swap�� �����. */

	XmgrLiveView_State_e		eAppState;					/* �� context�� WatchTV application state */
	MgrLiveView_UiState_e		eUiState;					/* �� context�� WatchTV UI state */
	HBOOL						bRadioBackground;			/* True if radio background is enabled */

	MgrLiveView_SessionType_t	sessionType;				/* Normal/Quiet/Replacement/MultiFeed �� WTV session tuning type */

	Handle_t					hSvc; 						/* Service handle of this session */
	HINT32						nChNum;						/* Channel number of the this session */
	ChListUiGroup_t				svcListGroup;				/* ������ Ʃ�� �׷� */
	DbSvc_Info_t					stSvcInfo;					/* ���� ���� information */

	DbSvc_TsInfo_t				tsInfo;						/* ���� TS information. Ʃ�� delivery type�� ��� ����. */

/* +++++ ARIB closed caption ���� ���� +++++ */
	HUINT16						usCaptionPid;				/* Closed caption PID. �Ϻ��� ��� */
	HUINT16						usSuperPid;					/* Super impose PID. �Ϻ��� ��� */
/* ----- ARIB closed caption ���� ���� ----- */

/* +++++ Subtitle ���� ���� +++++ */
	HBOOL						bSubtitleAllowed;			/* Subtitle �����ص� �ȴٴ� flag. MENU���� AUTO�̰ų� subtitle list popup�� start�� ��� TRUE */
	HBOOL						bTtxPageEnabled;			/* TELETEXT page�� �����ְ� ����. Subtitle�� enable�� �� ����. */
	SvcSi_SubtitleInfo_t				subtitleInfo;				/* svcInfo�� �ִ� index�� PMT���� ��� subtitle info */
	HUINT32						nDVBSbtCPageID;				/*subtitle���� page id �� ���Ƿ� ������ ��� �����. */
	HUINT32						nDVBSbtAPageID;
	HUINT32						nEBUSbtMagNum;
	HUINT32						nEBUSbtPageNum;
/* ----- Subtitle ���� ���� ----- */

	HBOOL						bSiStopped;					/* SI session�� stop �ߴ°�? */
	HBOOL						bCasStopped;				/* CAS session�� stop �ߴ°�? */
	HBOOL						bSubtitle_CC_Stopped;		/* Subtitle or Closed caption session�� stop �ߴ°�? */
	HBOOL						bTeletextStopped;			/* Teletext session�� stop �ߴ°�? */
	HBOOL						bPmtValid;					/* PMT�� ���޵�. */


/* DRM & Copy protection management */
	// TODO: siDrmInfo/casDrmInfo ���� �� �ʿ� ����.
	HBOOL						bSiDrmValid;				/* SI�κ��� Copy protection ������ detect ��. */
	DxCopyrightControl_t					drmInfo;					/* SI drm info */
	HBOOL						bCasDrmValid;				/* CAS�κ��� Copy protection ������ detect ��. SI���ٴ� CAS�� �켱 ������ ����.*/

/* CAS management */
	HBOOL						bCasOk;						/* CAS���� ��û �㰡�� �޾Ҵ°�? */
	HBOOL						bSCPinCodeValid;			/* Smart card Pin code�� ��� �����Ѱ�? */
	DxCAS_GroupId_e				nCasSystemId;				/* EVT_CAS_FAIL�޽����� p1���� ���޵Ǵ� CAS system ID (Nagra, Ir, Conax, ...) */
	void						*pCasInfo;					/* EVT_CAS_FAIL�޽����� p2�� ���޵Ǵ� CAS proc���� ����� data point */
	HINT32						nCasEvt;					/* CAS proc���� �Ѱ��� CAS event (eSEVT_CAS_OK, eSEVT_CAS_FAIL) */
	SvcCas_ShowType_e				nCasShow;					/* Audio, video�� ���� ������ subscription ���� */
	HBOOL						bAudioShow;					/* Audio subscription ���� */
	HBOOL						bVideoShow;					/* Video subscription ���� */

/* AV decoder management */
	HBOOL						bAvUnderrun;				/* AV underrun flag */
	HBOOL						bHdmiBlocked;				/* HDMI was blocked by HDCP fail or etc. */

/* MHEG �� ������ ��ۿ� support items */
	HUINT16 					usOrgVideoPid;				/* temp�� �ٲ�� ���� original video PID */
	DxVideoCodec_e				eOrgVideoCodec;				/* temp�� �ٲ��� ���� original video codec */
	HUINT16 					usOrgAudioPid;				/* temp�� �ٲ�� ���� original audio PID */
	HUINT16 					usOrgDolbyPid;				/* temp�� �ٲ�� ���� original Dolby audio PID */
	DxAudioCodec_e				eOrgAudioCodec;				/* temp�� �ٲ��� ���� original audio codec */
	DxAudioCodec_e				eOrgDolbyCodec;				/* temp�� �ٲ��� ���� original Dolby audio codec */
	HUINT16 					usOrgPcrPid;				/* temp�� �ٲ�� ���� original pcr PID */

	HUINT16 					usOrgCaptionPid;			/* temp�� �ٲ�� ���� original caption PID. �Ϻ��⿡�� ��� */
	HUINT16 					usOrgSuperPid;				/* temp�� �ٲ�� ���� original super PID. �Ϻ��⿡�� ��� */

/* Supplementary Service support items */
	Handle_t					hMasterSvc;					/* Supplement service �� ��쿡 master handle backup. */
	DbSvc_Info_t					stMasterSvcInfo;				/* Master service�� service information backup */

/* Replacement Service support items */
	HUINT32						ulReplaceMessage;			/* Replacement service event �޽��� ��� */
	Handle_t					hSuppleSvc;					/* Supplement service save for replacement service */

	SvcRsc_Info_t					stRscInfo;


/*
	���⼭���� WatchTv���� Copy ���� ���� Original Arguments ��.
*/
	MgrLive_START_t				stStartArg;
	HBOOL						bUnselectVideo;
	HBOOL						bUnselectAudio;
	HBOOL						bBlockUiStateNoti;	// ui state noti�� context setup�ϴ� ���� ���� flag

	/*
		etc..
	*/
	HUINT32						ulLastTuneStatus;	// fcc���� ����ϴ� last tune status
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

