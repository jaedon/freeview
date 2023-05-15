/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _xmgr_live_lock.h
	@brief

	Description:			\n
	Module: 			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	___XMGR_LIVE_LOCK_H___
#define	___XMGR_LIVE_LOCK_H___

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
	eLiveLockState_IDLE, 				/**< WTV�� �ƹ��͵� ���ϴ� ���� */
	eLiveLockState_LOCK_PIN,

/*************** �� ������ ���� PID start�� ���� �ƴ� ���� ���� ****************/
	eLiveLockState_PID_START,			/**< PID start/stop�� ��踦 ǥ���ϴ� ��. �� state�� �ӹ��� ���� ����� ��. */
/*************** �� ������ ���� PID start�� ���� �ƴ� ���� ���� ****************/

	eLiveLockState_WAIT_RATING,			/**< Rating������ ��ٸ��� ����. �� ���� state�� PID�� start�� ������. */
	eLiveLockState_RATING_PIN,			/**< Rating PIN code �Է� ��� ���� */
	eLiveLockState_SC_PIN,				/**< Smart card PIN code �Է� ��� ����*/
	eLiveLockState_AV,

	eLiveLockState_MAX
} xMGR_LiveLock_AppState_e;


typedef struct
{
	MgrLive_LiveMode_e		eLiveMode;
	HUINT32					ulActionId;
	xMGR_LiveLock_AppState_e	eAppState;
	MgrLiveLock_UiState_e			eUiState;

//	Handle_t					hAction;

	// Current Service : Viewing Service
	Handle_t					 hSvc;
	DbSvc_Info_t				 stSvcInfo;

	// Master Service
	Handle_t					 hMasterSvc;

	// Current States
//	XmgrLive_MwSvcExecPhase_e	 aeServicesStartPhase[eLiveMwSvcType_MAX];
//	XmgrLive_MwSvcState_e		 aeServicesState[eLiveMwSvcType_MAX];

	// PMT
#if defined(CONFIG_MW_SI_PMT_RATING)
	HBOOL					bPmtValid;
	HBOOL					bPmtRatingValid;
#endif

	// EIT
	HBOOL					bEitValid;
	HUINT32					ulEventId;

	// rating
	HBOOL					bRatingPassByLockPass;
	HBOOL					bRatingValid;
	HUINT32					eRatingValue;
	HBOOL					bRatingPinPassed;
	HBOOL					bLockPinPassed;
	HBOOL					bCasOk;
	HBOOL					bGetCasEvt;
	HBOOL					bCasStateChanged;
	HBOOL					bNoSignal;
	HBOOL					bSCPinCodeValid;
	HBOOL					bSCPinPassed;
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	HBOOL					bSCPinRemoved;
#endif	
#if defined(CONFIG_MW_CI_PLUS_V_1_3)
	HBOOL					bIsCiPlusContent;
#endif

	SvcRsc_Info_t				stRscInfo;

/*
	���⼭���� WatchTv���� Copy ���� ���� Original Arguments ��.
*/
	MgrLive_START_t			stStartArg;

} xMGR_LiveLock_Context_t;


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
HERROR			XMGR_LiveLock_BASE_Init (void);

xMGR_LiveLock_Context_t*		XMGR_LiveLock_BASE_GetContext (HUINT32 ulActionId);
HERROR			XMGR_LiveLock_BASE_InitContext (void);
HERROR 			XMGR_LiveLock_BASE_ResetUiState(HUINT32 ulActionId, HBOOL bSvcStart);

HERROR			XMGR_LiveLock_BASE_StartSession (xMGR_LiveLock_Context_t *pstContext, Handle_t hAction, MgrLive_START_t *pstStart);
HERROR			XMGR_LiveLock_BASE_StopSession (xMGR_LiveLock_Context_t *pstContext);

BUS_Result_t	XMGR_LiveLock_BASE_MsgBusCreate (xMGR_LiveLock_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	XMGR_LiveLock_BASE_MsgBusDestroy (xMGR_LiveLock_Context_t *pstContext, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	XMGR_LiveLock_BASE_MsgLiveStartSvc (xMGR_LiveLock_Context_t *pstContext, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	XMGR_LiveLock_BASE_MsgLiveStopSvc (xMGR_LiveLock_Context_t *pstContext, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3);

BUS_Result_t 	XMGR_LiveLock_BASE_EvtSiEit (xMGR_LiveLock_Context_t *pstContext, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t 	XMGR_LiveLock_BASE_EvtSiEitTimeout (xMGR_LiveLock_Context_t *pstContext, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3);
#if defined(CONFIG_MW_SI_PMT_RATING)
BUS_Result_t 	XMGR_LiveLock_BASE_EvtSiPmt (xMGR_LiveLock_Context_t *pstContext, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3);
#endif

BUS_Result_t 	XMGR_LiveLock_BASE_MsgWtvSetRatingPassedFlag(xMGR_LiveLock_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t 	XMGR_LiveLock_BASE_MsgAppPinOk(xMGR_LiveLock_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t 	XMGR_LiveLock_BASE_MsgAppPinDenied(xMGR_LiveLock_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t 	XMGR_LiveLock_BASE_MsgAppCheckRating(xMGR_LiveLock_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t 	XMGR_LiveLock_BASE_MsgAppCheckLock(xMGR_LiveLock_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t 	XMGR_LiveLock_BASE_EvtChLocked(xMGR_LiveLock_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3);

#if defined(CONFIG_MW_CAS)
BUS_Result_t 	XMGR_LiveLock_BASE_EvtCasOk(xMGR_LiveLock_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t 	XMGR_LiveLock_BASE_EvtCasFail(xMGR_LiveLock_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3);
#endif
BUS_Result_t 	XMGR_LiveLock_BASE_MsgLiveSetMainviewAction(xMGR_LiveLock_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t 	XMGR_LiveLock_BASE_MsgBusGoStandby(xMGR_LiveLock_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3);

BUS_Result_t	XMGR_LiveLock_BASE_ProtoProc (xMGR_LiveLock_Context_t *pstContext, HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Callback_t	XMGR_LiveLock_BASE_GetProcByActionId (HUINT32 ulActionId);
HERROR 			XMGR_LiveLock_BASE_GetState(HUINT32 ulActionId, MgrLiveLock_UiState_e *peLockState);
HERROR 			XMGR_LiveLock_BASE_GetRating(HUINT32 ulActionId, HBOOL *pbRatingValid, DxRatingAge_e *peRatingValue);

//<< for DEBUG >>
void 			XMGR_LiveLock_BASE_DumpContext(xMGR_LiveLock_Context_t *pstContext);
void 			XMGR_LiveLock_BASE_PrintContext(void);

#endif /* !___XMGR_LIVE_LOCK_H___ */

