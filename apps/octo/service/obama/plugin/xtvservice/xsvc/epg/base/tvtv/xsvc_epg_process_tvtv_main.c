/**
	@file     file_name.c File Name을 적습니다.
	@brief    file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <xsvc_epg.h>
//#include <xsvc_module_epg.h>

#include "_xsvc_epg_tvtv.h"
#include "_svc_epg_common_lib.h"
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
STATIC HERROR				xsvc_epg_StartLive_Tvtv(Handle_t hAction, void *pParam);
STATIC HERROR				xsvc_epg_StopLive_Tvtv(Handle_t hAction);
STATIC HERROR				xsvc_epg_StopTvtvAction_Tvtv(void);


#define ____GLOBAL_FUNC____

HERROR xsvc_epg_InitAction_Tvtv(SvcEpg_EpgKind_e eEpgKind)
{
	HERROR	hErr;

	if (eEPG_KIND_MAX <= eEpgKind)
	{
		return ERR_EPG_FAIL;
	}

	hErr = ERR_EPG_NOT_SUPPORTED;
	switch (eEpgKind)
	{
	case eEPG_KIND_TVTV:
		return xsvc_epg_StopTvtvAction_Tvtv();
	default:
		break;
	}

	return hErr;
}

HERROR xsvc_epg_Start_Tvtv(SvcEpg_EpgKind_e eEpgKind, SvcEpg_EventKind_e eEvtKind, Handle_t hAction, Handle_t hService, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	HERROR	hErr;

	if (eEPG_KIND_MAX <= eEpgKind)
	{
		return ERR_EPG_FAIL;
	}

	hErr = ERR_EPG_NOT_SUPPORTED;
	switch (eEpgKind)
	{
	case eEPG_KIND_TVTV:
		switch(eEvtKind)
		{
		case eEPG_EVT_KIND_ALL:
			return xsvc_epg_StartLive_Tvtv(hAction, (void *)ulParam1);
		case eEPG_EVT_KIND_PF_ALL:
		case eEPG_EVT_KIND_PF_ACT:
		case eEPG_EVT_KIND_PF_OTH:
		case eEPG_EVT_KIND_SCH_ALL:
		case eEPG_EVT_KIND_SCH_ACT:
		case eEPG_EVT_KIND_SCH_OTH:
		default:
			break;
		}
	default:
		break;
	}

	return hErr;
}

HERROR xsvc_epg_Stop_Tvtv(SvcEpg_EpgKind_e eEpgKind, SvcEpg_EventKind_e eEvtKind, Handle_t hAction)
{
	HERROR	hErr;

	if (eEPG_KIND_MAX <= eEpgKind)
	{
		return ERR_EPG_FAIL;
	}

	hErr = ERR_EPG_NOT_SUPPORTED;
	switch (eEpgKind)
	{
	case eEPG_KIND_TVTV:
		switch(eEvtKind)
		{
		case eEPG_EVT_KIND_ALL:
			return xsvc_epg_StopLive_Tvtv(hAction);
		case eEPG_EVT_KIND_PF_ALL:
		case eEPG_EVT_KIND_PF_ACT:
		case eEPG_EVT_KIND_PF_OTH:
		case eEPG_EVT_KIND_SCH_ALL:
		case eEPG_EVT_KIND_SCH_ACT:
		case eEPG_EVT_KIND_SCH_OTH:
		default:
			break;
		}
	default:
		break;
	}

	return hErr;
}

#define ____LOCAL_FUNC____

STATIC HERROR				xsvc_epg_StartLive_Tvtv(Handle_t hAction, void *pParam)
{
	HERROR				hError = ERR_EPG_FAIL;
	HUINT32				ulMessage = 0;
	DbSvc_TripleId_t		*pTvtvTripleId = (DbSvc_TripleId_t*)pParam;
	DbSvc_TripleId_t		*pTempTvtvTripleId = NULL;

#if	0
{
	Handle_t				hTvtvService = HANDLE_NULL;
	DbSvc_TripleId_t		stTripleId = {0, };

	stTripleId.usTsId = SAT_TVTV_TS_ID;
	stTripleId.usOnId = SAT_TVTV_ON_ID;
	stTripleId.usSvcId = SAT_TVTV_SVC_ID;
	hError = DB_SVC_FindServiceHandleByTripleId(eDxDELIVERY_TYPE_SAT, stTripleId, &hTvtvService);
	if(hError != ERR_OK)
	{
		HxLOG_Print("[xsvc_epg_StartLive_Tvtv] DB_SVC_FindServiceHandleByTripleId fail!!\n");
		goto EPGMAIN_FUCN_EXIT;
	}
	hService = hTvtvService;
}
#endif
	pTempTvtvTripleId = (DbSvc_TripleId_t*)OxEPG_Malloc(sizeof(DbSvc_TripleId_t));
	if(pTempTvtvTripleId == NULL)
	{
		HxLOG_Print("[xsvc_epg_StartLive_Tvtv] pTempTvtvTripleId == NULL!!\n");
		goto EPGMAIN_FUCN_EXIT;
	}
	HxSTD_memcpy(pTempTvtvTripleId, pTvtvTripleId, sizeof(DbSvc_TripleId_t));

	hError = SVC_EPG_MakeEpgMessage(EPG_CMD_START,  eEPG_PROCESS_TVTV, &ulMessage);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("[xsvc_epg_StartLive_Tvtv] MW_EPG_MakeEpgMessage fail!!\n");
		goto EPGMAIN_FUCN_EXIT;
	}

	hError = SVC_EPG_SendMessage(ulMessage, hAction, (HUINT32)pTempTvtvTripleId, (HUINT32)0);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("[xsvc_epg_StartLive_Tvtv] MW_EPG_MakeEpgMessage fail!!\n");
		goto EPGMAIN_FUCN_EXIT;
	}

	hError = ERR_EPG_OK;

EPGMAIN_FUCN_EXIT :

	return hError;
}

STATIC HERROR				xsvc_epg_StopLive_Tvtv(Handle_t hAction)
{
	HERROR				hError = ERR_EPG_FAIL;
	HUINT32				ulMessage = 0;

	hError = SVC_EPG_MakeEpgMessage(EPG_CMD_STOP,  eEPG_PROCESS_TVTV, &ulMessage);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("[xsvc_epg_StopLive_Tvtv] MW_EPG_MakeEpgMessage fail!!\n");
		goto EPGMAIN_FUCN_EXIT;
	}

	hError = SVC_EPG_SendMessage(ulMessage, hAction, (HUINT32)HANDLE_NULL, 0);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("[xsvc_epg_StopLive_Tvtv] MW_EPG_MakeEpgMessage fail!!\n");
		goto EPGMAIN_FUCN_EXIT;
	}

	hError = ERR_EPG_OK;

EPGMAIN_FUCN_EXIT :

	return hError;
}

STATIC HERROR				xsvc_epg_StopTvtvAction_Tvtv(void)
{
	HERROR				hError = ERR_EPG_FAIL;
	HUINT32				ulMessage = 0;

	hError = SVC_EPG_MakeEpgMessage(EPG_CMD_STOP_TVTV_ACTION,  eEPG_PROCESS_TVTV, &ulMessage);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("[MW_EPG_StopLoadXMLFile] MW_EPG_MakeEpgMessage fail!!\n");
		goto EPGMAIN_FUCN_EXIT;
	}

	hError = SVC_EPG_SendMessage(ulMessage, (Handle_t)0, (HUINT32)0, (HUINT32)0);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("[MW_EPG_StopLoadXMLFile] MW_EPG_MakeEpgMessage fail!!\n");
		goto EPGMAIN_FUCN_EXIT;
	}

	hError = ERR_EPG_OK;

EPGMAIN_FUCN_EXIT :

	return hError;
}

/* =============================================================================
 * EPG Context
 * ============================================================================= */

// TODO: should be implemented



/*********************** End of File ******************************/
