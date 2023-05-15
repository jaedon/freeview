/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pmgr_live.h
	@brief

	Description:			\n
	Module: 			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	___PMGR_LIVE_H___
#define	___PMGR_LIVE_H___


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <db_svc.h>
#include <mgr_live.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Plug-In Functions    ********************/
/*******************************************************************/
HERROR	pmgr_live_Init (void);

// Proc
HERROR	pmgr_live_GetLiveViewProc (HUINT32 ulActionId, BUS_Callback_t *pfnProc);
HERROR	pmgr_live_GetLiveLockProc (HUINT32 ulActionId, BUS_Callback_t *pfnProc);
HERROR	pmgr_live_GetLiveCompProc (HUINT32 ulActionId, BUS_Callback_t *pfnProc);

// UI State
HERROR	pmgr_live_ResetUiStateView (HUINT32 ulActionId, HBOOL bSvcStart);
HERROR	pmgr_live_ResetUiStateLock (HUINT32 ulActionId, HBOOL bSvcStart);
HERROR	pmgr_live_GetUiStateView (HUINT32 ulActionId, MgrLiveView_UiState_e *peState);
HERROR	pmgr_live_GetUiStateLock (HUINT32 ulActionId, MgrLiveView_UiState_e *peState);

HERROR 	pmgr_live_GetState (HUINT32 ulActionId, MgrLive_ProcType_e eProcType, HUINT32 *pulUiState);
HERROR  pmgr_live_GetLiveMode (HUINT32 ulActionId, MgrLive_LiveMode_e *peLiveMode);
HERROR 	pmgr_live_GetServiceHandle (HUINT32 ulActionId, MgrLiveView_PlaySvc_e eSvcType, Handle_t *phSvc);
HERROR 	pmgr_live_GetSessionType (HUINT32 ulActionId, MgrLiveView_SessionType_t *pSessionType);
HERROR 	pmgr_live_GetSvcInfo (HUINT32 ulActionId, MgrLiveView_PlaySvc_e eSvcType, DbSvc_Info_t *pMasterSvcInfo);
HERROR 	pmgr_live_GetTsInfo (HUINT32 ulActionId, DbSvc_TsInfo_t *pTsInfo);
HERROR 	pmgr_live_GetLastTuneStatus(HUINT32 ulActionId, HUINT32 *pulLastTuneStatus);
HERROR 	pmgr_live_GetRating (HUINT32 ulActionId, HBOOL *pbRatingValid, DxRatingAge_e *peRatingValue);
HERROR 	pmgr_live_SetAudioIndex (HUINT32 ulActionId, HUINT32 ulIndex);
HERROR 	pmgr_live_SetDualMonoConfig (HUINT32 ulActionId, DxDualMonoSelect_e eDualMonoConfig);
HERROR	pmgr_live_GetRingBufHandle (HUINT32 ulActionId, Handle_t *phRingBuf);
HERROR	pmgr_live_GetProfileFromJSON(const HCHAR *pProfileString, MgrLive_LiveStreamArg_t *pstDestArg);

// Zap
void	pmgr_live_zap_Init (void);
// Main Svc Info : Last Svc Info
void	pmgr_live_RegisterLastSvcInfo (Handle_t hSvc);
HERROR	pmgr_live_GetPrevSvcHandle (Handle_t *hPrevSvc);
HERROR	pmgr_live_GetLastSvcHandle (MgrLive_LastSvcItem_t eLastSvcItem, Handle_t *hLastSvc);

void	pmgr_live_ResetZappingInfo (HBOOL bLoadSetup);
HERROR	pmgr_live_GetCurrentSvcInfo (HUINT32 ulActionId, Handle_t *phCurSvc);
HERROR	pmgr_live_SetCurrentSvcInfo (HUINT32 ulActionId, Handle_t hCurSvc);
// Svc Lock
HBOOL	pmgr_live_IsSvcLockPassed (HUINT32 ulActionId, Handle_t hSvc);
HERROR	pmgr_live_SetSvcLockPassed (HUINT32 ulActionId, Handle_t hSvc);
HERROR	pmgr_live_ResetSvcLockPassed (HUINT32 ulActionId);

HERROR 	pmgr_live_StartSubtitle(Handle_t hAction);
HERROR 	pmgr_live_StopSubtitle(Handle_t hAction, ApiSyncMode_t eSync);
HERROR 	pmgr_live_StopAllEpg(Handle_t hAction);


#endif /* !___PMGR_LIVE_H___ */

