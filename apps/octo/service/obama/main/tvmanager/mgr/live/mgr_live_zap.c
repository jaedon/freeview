/**
	@file     mgr_live_zap.c
	@brief    Tuner live service를 튜닝하고 상태를 관리하는 watchTV와 analogTV의 공통 모듈이다.

	Description:
	Module: MGR/LIVE/ZAP			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <db_svc.h>
#include <db_param.h>

#include <svc_pipe.h>
#include <svc_resources.h>
#include <svc_ch.h>
#include <svc_si.h>
//#include <mwc_util.h>
#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_live.h>
#include <pmgr_live.h>
//#include "./local_include/_mgr_live_int.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
//	#define	CONFIG_ZAPPING_WITHIN_NOCONFLICTION

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Function      ********************/
/*******************************************************************/

STATIC HERROR mgr_live_Start(HUINT32 ulActionId, Handle_t hSvc)
{
	HUINT32				ulMainActionId;
	MgrLive_START_t 	stStartArg;
	HERROR				hErr = ERR_FAIL;

	HxSTD_memset(&stStartArg, 0x00, sizeof(MgrLive_START_t));

	if (ulActionId == eActionId_MAX)
	{
		stStartArg.eLiveMode = eMgrLiveMode_MainLive;
	}
	else
	{
		ulMainActionId = MGR_ACTION_GetMainActionId();
		if (ulMainActionId == ulActionId)
		{
			stStartArg.eLiveMode = eMgrLiveMode_MainLive;
		}
		else
		{
			stStartArg.eLiveMode = eMgrLiveMode_PipLive;
		}
	}
	stStartArg.hMasterSvc = hSvc;
	stStartArg.hService = hSvc;

	HxLOG_Print("Start svc (0x%X, 0x%X) eLiveMode(%d) \n", stStartArg.hMasterSvc, stStartArg.hService, stStartArg.eLiveMode);
	hErr = MGR_LIVE_Start(&stStartArg, &ulMainActionId);
	if (hErr != ERR_OK)
	{
		HxLOG_Error ("MGR_LIVE_Start() failed.. hSvc(0x%X) ulActionId(%d) \n", stStartArg.hMasterSvc, ulMainActionId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

/*******************************************************************/
/************************	API Functions  *************************/
/*******************************************************************/

void MGR_LIVE_ZapInit(void)
{
	/* Current/Last channel info 초기화 */
	pmgr_live_zap_Init();
}

void MGR_LIVE_ResetZappingInfo(HBOOL bLoadSetup)
{
	pmgr_live_ResetZappingInfo(bLoadSetup);
}

void MGR_LIVE_RegisterLastSvcInfo(Handle_t hSvc)
{
	pmgr_live_RegisterLastSvcInfo(hSvc);
}

HERROR MGR_LIVE_CurrentSvcInfoGet(HUINT32 ulActionId, Handle_t *hCurSvc)
{
	return pmgr_live_GetCurrentSvcInfo(ulActionId, hCurSvc);
}

HERROR MGR_LIVE_CurrentSvcInfoSet(HUINT32 ulActionId, Handle_t hCurSvc)
{
	return pmgr_live_SetCurrentSvcInfo(ulActionId, hCurSvc);
}

HERROR MGR_LIVE_GetPrevSvcHandle(Handle_t *phPrevSvc)
{
	return pmgr_live_GetPrevSvcHandle(phPrevSvc);
}

HERROR MGR_LIVE_GetLastSvcHandle(MgrLive_LastSvcItem_t eLastSvcItem, Handle_t *phLastSvc)
{
	return pmgr_live_GetLastSvcHandle(eLastSvcItem, phLastSvc);
}

HBOOL MGR_LIVE_SvcLockIsPassed(HUINT32 ulActionId, Handle_t hSvc)
{
	return pmgr_live_IsSvcLockPassed(ulActionId, hSvc);
}

HERROR MGR_LIVE_SvcLockSetPassed(HUINT32 ulActionId, Handle_t hSvc)
{
	return pmgr_live_SetSvcLockPassed(ulActionId, hSvc);
}

HERROR MGR_LIVE_SvcLockResetPassed(HUINT32 ulActionId)
{
	return pmgr_live_ResetSvcLockPassed(ulActionId);
}


HERROR MGR_LIVE_CmdChangeTempPid(Handle_t hAction,  DxVideoCodec_e eVideoCodec, HUINT16 usVideoPid, DxAudioCodec_e eAudioCodec, HUINT16 usAudioPid, HUINT16 usPcrPid)
{
	HUINT32		ulVideoParam = 0;
	HUINT32		ulAudioParam = 0;

	ulVideoParam = (eVideoCodec << 16) | usVideoPid;
	ulAudioParam = (eAudioCodec << 16) | usAudioPid;

	BUS_SendMessage(NULL, eMEVT_LIVE_CHANGE_TEMP_PID, hAction, ulVideoParam, ulAudioParam, usPcrPid);

	return ERR_OK;
}

HERROR MGR_LIVE_CmdSendServiceStartMainView(Handle_t hSvc, ChListUiGroup_t eChListUiGroup, HUINT32 ulTuningMode)
{
	return mgr_live_Start(eActionId_MAX, hSvc);
}

HERROR MGR_LIVE_CmdPostServiceStartMainView(Handle_t hSvc, ChListUiGroup_t eChListUiGroup, HUINT32 ulTuningMode)
{
	return mgr_live_Start(eActionId_MAX, hSvc);
}

HERROR MGR_LIVE_CmdSendServiceStartByActionId(HUINT32 ulActionId, Handle_t hSvc)
{
	return mgr_live_Start(ulActionId, hSvc);
}

HERROR MGR_LIVE_CmdPostServiceStartByActionId(HUINT32 ulActionId, Handle_t hSvc)
{
	return mgr_live_Start(ulActionId, hSvc);
}

/* END OF FILE */
