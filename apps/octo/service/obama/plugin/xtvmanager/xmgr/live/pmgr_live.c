/**
	@file     pmgr_live.c
	@brief    file_name & simple comment.

	Description: Applicatuion for searching Services\n
	Module: Application(Layer), Menu(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/**
  * Character encoding.
  *
  * MS949
  *
  * This source file that uses MS949 encoding.
  * MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
  * MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
  * Quelldatei, die MS949-Codierung verwendet.
  *
**/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <isosvc.h>
#include <stb_params.h>
#include <linkedlist.h>

#include <db_svc.h>
#include <svc_av.h>
#include <svc_transcoder.h>
#include <svc_cas.h>


#include <mgr_common.h>
#include <mgr_action.h>

#include <mgr_live.h>
#include <xmgr_live.h>

#include "./local_include/_xmgr_live.h"
#include "./local_include/_xmgr_live_view.h"
#include "./local_include/_xmgr_live_lock.h"
#include "./local_include/_xmgr_live_comp.h"
#include "./local_include/_xmgr_live_zap.h"

#if defined(CONFIG_SUPPORT_FCC)
#include <svc_epg.h>
#endif

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

HERROR pmgr_live_Init (void)
{
	HERROR	hErr;

	// init proc context
	hErr = xmgr_live_Init_Base();
	if (hErr != ERR_OK)
	{
		HxLOG_Error("xmgr_live_Init_Base failed. \n");
	}

	return ERR_OK;
}

HERROR pmgr_live_GetLiveViewProc (HUINT32 ulActionId, BUS_Callback_t *pfnProc)
{
	if (NULL == pfnProc)
		return ERR_FAIL;

	*pfnProc = XMGR_LiveView_BASE_GetProcByActionId(ulActionId);

	return (NULL == *pfnProc) ? ERR_FAIL : ERR_OK;
}

HERROR pmgr_live_GetLiveLockProc (HUINT32 ulActionId, BUS_Callback_t *pfnProc)
{
	if (NULL == pfnProc)
		return ERR_FAIL;

	*pfnProc = XMGR_LiveLock_BASE_GetProcByActionId(ulActionId);

	return (NULL == *pfnProc) ? ERR_FAIL : ERR_OK;
}

HERROR pmgr_live_GetLiveCompProc (HUINT32 ulActionId, BUS_Callback_t *pfnProc)
{
	if (NULL == pfnProc)
		return ERR_FAIL;

	*pfnProc = XMGR_LiveComp_BASE_GetProc();

	return (NULL == *pfnProc) ? ERR_FAIL : ERR_OK;
}

HERROR pmgr_live_GetState(HUINT32 ulActionId, MgrLive_ProcType_e eProcType, HUINT32 *pulUiState)
{
	HERROR		hErr = ERR_FAIL;

	if (NULL == pulUiState)
		return ERR_FAIL;

	hErr = xmgr_live_GetState_Base (ulActionId, eProcType, pulUiState);

	return hErr;
}

HERROR pmgr_live_GetLiveMode (HUINT32 ulActionId, MgrLive_LiveMode_e *peLiveMode)
{
	return XMGR_LiveView_BASE_GetLiveMode (ulActionId, peLiveMode);
}

HERROR pmgr_live_GetServiceHandle(HUINT32 ulActionId, MgrLiveView_PlaySvc_e eSvcType, Handle_t *phSvc)
{
	return XMGR_LiveView_BASE_GetServiceHandle(ulActionId, eSvcType, phSvc);
}

HERROR pmgr_live_GetSessionType(HUINT32 ulActionId, MgrLiveView_SessionType_t *pSessionType)
{
	return XMGR_LiveView_BASE_GetSessionType(ulActionId, pSessionType);
}

HERROR pmgr_live_GetSvcInfo (HUINT32 ulActionId, MgrLiveView_PlaySvc_e eSvcType, DbSvc_Info_t *pMasterSvcInfo)
{
	return XMGR_LiveView_BASE_GetSvcInfo(ulActionId, eSvcType, pMasterSvcInfo);
}

HERROR pmgr_live_GetTsInfo(HUINT32 ulActionId, DbSvc_TsInfo_t *pTsInfo)
{
	return XMGR_LiveView_BASE_GetTsInfo(ulActionId, pTsInfo);
}

HERROR pmgr_live_GetLastTuneStatus(HUINT32 ulActionId, HUINT32 *pulLastTuneStatus)
{
	return XMGR_LiveView_BASE_GetLastTuneStatus(ulActionId, pulLastTuneStatus);
}

HERROR pmgr_live_GetRating(HUINT32 ulActionId, HBOOL *pbRatingValid, DxRatingAge_e *peRatingValue)
{
	return XMGR_LiveLock_BASE_GetRating(ulActionId, pbRatingValid, peRatingValue);
}

HERROR pmgr_live_SetAudioIndex(HUINT32 ulActionId, HUINT32 ulIndex)
{
	return ERR_OK;
}

HERROR pmgr_live_SetDualMonoConfig(HUINT32 ulActionId, DxDualMonoSelect_e eDualMonoConfig)
{
	return ERR_OK;
}

HERROR pmgr_live_ResetUiStateView (HUINT32 ulActionId, HBOOL bSvcStart)
{
	return XMGR_LiveView_BASE_ResetUiState(ulActionId, bSvcStart);
}

HERROR pmgr_live_ResetUiStateLock (HUINT32 ulActionId, HBOOL bSvcStart)
{
	return XMGR_LiveLock_BASE_ResetUiState(ulActionId, bSvcStart);
}

HERROR pmgr_live_GetUiStateView (HUINT32 ulActionId, MgrLiveView_UiState_e *peState)
{
	if (NULL == peState)
		return ERR_FAIL;

	return XMGR_LiveView_BASE_GetState(ulActionId, peState);
}

HERROR pmgr_live_GetUiStateLock (HUINT32 ulActionId, MgrLiveLock_UiState_e *peState)
{
	if (NULL == peState)
		return ERR_FAIL;

	return XMGR_LiveLock_BASE_GetState(ulActionId, peState);
}

HERROR pmgr_live_GetRingBufHandle (HUINT32 ulActionId, Handle_t *phRingBuf)
{
	return XMGR_LiveView_BASE_GetRingBufHandle(ulActionId, phRingBuf);
}

HERROR pmgr_live_GetProfileFromJSON(const HCHAR *pProfileString, MgrLive_LiveStreamArg_t *pstDestArg)
{
	return XMGR_LiveView_BASE_GetProfileFromJSON (pProfileString, pstDestArg);
}

/*		Zap
 */

void pmgr_live_zap_Init(void)
{
	XMGR_ZAP_Init_Base();
}

void		pmgr_live_ResetZappingInfo (HBOOL bLoadSetup)
{
	XMGR_ZAP_ResetZappingInfo (bLoadSetup);
}

void		pmgr_live_RegisterLastSvcInfo (Handle_t hSvc)
{
	XMGR_ZAP_RegisterLastSvcInfo (hSvc);
}

HERROR		pmgr_live_GetPrevSvcHandle (Handle_t *phPrevSvc)
{
	return XMGR_ZAP_GetPrevSvcHandle (phPrevSvc);
}

HERROR		pmgr_live_GetLastSvcHandle (MgrLive_LastSvcItem_t eLastSvcItem, Handle_t *phLastSvc)
{
	return XMGR_ZAP_GetLastSvcHandle (eLastSvcItem, phLastSvc);
}

HBOOL		pmgr_live_IsSvcLockPassed (HUINT32 ulActionId, Handle_t hSvc)
{
	return XMGR_ZAP_IsSvcLockPassed (ulActionId, hSvc);
}

HERROR		pmgr_live_SetSvcLockPassed (HUINT32 ulActionId, Handle_t hSvc)
{
	return XMGR_ZAP_SetSvcLockPassed (ulActionId, hSvc);
}

HERROR		pmgr_live_ResetSvcLockPassed (HUINT32 ulActionId)
{
	return XMGR_ZAP_ResetSvcLockPassed (ulActionId);
}

HERROR		pmgr_live_GetCurrentSvcInfo (HUINT32 ulActionId, Handle_t *phCurSvc)
{
	return XMGR_ZAP_GetCurrentSvcInfo (ulActionId, phCurSvc);
}

HERROR		pmgr_live_SetCurrentSvcInfo (HUINT32 ulActionId, Handle_t hCurSvc)
{
	return XMGR_ZAP_SetCurrentSvcInfo (ulActionId, hCurSvc);
}

HERROR 		pmgr_live_StartSubtitle(Handle_t hAction)
{
	return XMGR_LiveComp_BASE_StartSubtitle(hAction);
}

HERROR 		pmgr_live_StopSubtitle(Handle_t hAction, ApiSyncMode_t eSync)
{
	return XMGR_LiveComp_BASE_StopSubtitle(hAction, eSync);
}

HERROR 		pmgr_live_StopAllEpg(Handle_t hAction)
{
	return XMGR_LiveComp_BASE_StopEpg(eEPG_KIND_DVB, eEPG_EVT_KIND_ALL, hAction);
}

/* EOF */
