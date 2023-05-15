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
#include <octo_common.h>
#include <xsvc_epg.h>
#include <_svc_epg_common.h>
#include <_svc_epg_common_lib.h>
#include <_svc_epg_context_mgr.h>
#include <_svc_epg_common_lib.h>

#include "../local_include/_xsvc_epg.h"

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
STATIC HERROR				xsvc_epg_StartLive_Base(Handle_t hAction, Handle_t hService);
STATIC HERROR				xsvc_epg_StartLivePF_Base(Handle_t hAction, Handle_t hService);
STATIC HERROR				xsvc_epg_StartLiveSCH_Base(Handle_t hAction, Handle_t hService);
STATIC HERROR				xsvc_epg_StopLive_Base(Handle_t hAction);
STATIC HERROR				xsvc_epg_StopLivePF_Base(Handle_t hAction);
STATIC HERROR				xsvc_epg_StopLiveSCH_Base(Handle_t hAction);

#define ____GLOBAL_FUNC____

HERROR xsvc_epg_InitAction_Base(SvcEpg_EpgKind_e eEpgKind)
{
	if (eEPG_KIND_MAX <= eEpgKind)
	{
		return ERR_EPG_FAIL;
	}
	return ERR_EPG_NOT_SUPPORTED;
}

HERROR xsvc_epg_Start_Base(SvcEpg_EpgKind_e eEpgKind, SvcEpg_EventKind_e eEvtKind, Handle_t hAction, Handle_t hService, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	HERROR	hErr;

	if (eEPG_KIND_MAX <= eEpgKind)
	{
		return ERR_EPG_FAIL;
	}

	hErr = ERR_EPG_NOT_SUPPORTED;
	switch (eEpgKind)
	{
	case eEPG_KIND_DVB:
		switch(eEvtKind)
		{
		case eEPG_EVT_KIND_ALL:
			return xsvc_epg_StartLive_Base(hAction, hService);
		case eEPG_EVT_KIND_PF_ALL:
			return xsvc_epg_StartLivePF_Base(hAction, hService);
		case eEPG_EVT_KIND_PF_ACT:
		case eEPG_EVT_KIND_PF_OTH:
			return ERR_EPG_NOT_SUPPORTED;
		case eEPG_EVT_KIND_SCH_ALL:
			return xsvc_epg_StartLiveSCH_Base(hAction, hService);
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

HERROR xsvc_epg_Stop_Base(SvcEpg_EpgKind_e eEpgKind, SvcEpg_EventKind_e eEvtKind, Handle_t hAction)
{
	HERROR	hErr;

	if (eEPG_KIND_MAX <= eEpgKind)
	{
		return ERR_EPG_FAIL;
	}

	hErr = ERR_EPG_NOT_SUPPORTED;
	switch (eEpgKind)
	{
	case eEPG_KIND_DVB:
		switch(eEvtKind)
		{
		case eEPG_EVT_KIND_ALL:
			return xsvc_epg_StopLive_Base(hAction);
		case eEPG_EVT_KIND_PF_ALL:
			return xsvc_epg_StopLivePF_Base(hAction);
		case eEPG_EVT_KIND_SCH_ALL:
			return xsvc_epg_StopLiveSCH_Base(hAction);
		case eEPG_EVT_KIND_PF_ACT:
		case eEPG_EVT_KIND_PF_OTH:
		case eEPG_EVT_KIND_SCH_ACT:
		case eEPG_EVT_KIND_SCH_OTH:
		default:
			HxLOG_Error("Not supported evtKind(%d) \n",eEvtKind);
			break;
		}
		break;
	default:
		HxLOG_Debug("Not supported epgKind(%d) \n",eEpgKind);
		break;
	}

	return hErr;
}

HERROR xsvc_epg_LoadData_Base(SvcEpg_EpgKind_e eEpgKind, Handle_t hAction, Handle_t hService)
{
	if (eEPG_KIND_MAX <= eEpgKind)
	{
		return ERR_EPG_FAIL;
	}
	return ERR_EPG_NOT_SUPPORTED;
}

HERROR xsvc_epg_UnLoadData_Base(SvcEpg_EpgKind_e eEpgKind, Handle_t hAction)
{
	if (eEPG_KIND_MAX <= eEpgKind)
	{
		return ERR_EPG_FAIL;
	}
	return ERR_EPG_NOT_SUPPORTED;
}

HERROR xsvc_epg_StartContent_Base(SvcEpg_EpgKind_e eEpgKind, Handle_t hAction, Handle_t hService)
{
	return ERR_EPG_NOT_SUPPORTED;
}

HERROR xsvc_epg_StopContent_Base(SvcEpg_EpgKind_e eEpgKind, Handle_t hAction)
{
	return ERR_EPG_NOT_SUPPORTED;
}

#define ____LOCAL_FUNC____


STATIC HERROR xsvc_epg_StartLive_Base(Handle_t hAction, Handle_t hService)
{
	HERROR				hError = ERR_EPG_FAIL;
	HUINT32				ulMessage = 0;
	DbSvc_Info_t		stSvcInfo = { { 0 }, };
	EPG_SpecType_e		eSpec = eEPG_SPEC_BASE;

	HxLOG_Debug("Enter!!\n");

	hError = DB_SVC_GetServiceInfo(hService, &stSvcInfo);
	if (hError != ERR_OK)
	{
		HxLOG_Print("DB_SVC_GetServiceInfo fail!!\n");
		goto EPGMAIN_FUCN_EXIT;
	}
	hError = SVC_EPG_GetSpec(DbSvc_GetDeliType(stSvcInfo), hService, &eSpec);
	if (hError != ERR_OK)
	{
		HxLOG_Print("SVC_EPG_GetSpec fail!!\n");
		goto EPGMAIN_FUCN_EXIT;
	}

	switch (eSpec)
	{
		case eEPG_SPEC_FSAT:
			HxLOG_Error(": Don't start in fsat \n");
			hError = ERR_EPG_OK;
			break;
		default:
			hError = SVC_EPG_MakeEpgMessage(EPG_CMD_START,  eEPG_PROCESS_EIT_PF, &ulMessage);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Print("MW_EPG_MakeEpgMessage fail!!\n");
				goto EPGMAIN_FUCN_EXIT;
			}
			hError = SVC_EPG_SendMessage(ulMessage, hAction, (HUINT32)hService, (HUINT32)0);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Print("MW_EPG_MakeEpgMessage fail!!\n");
				goto EPGMAIN_FUCN_EXIT;
			}

			hError = SVC_EPG_MakeEpgMessage(EPG_CMD_START,  eEPG_PROCESS_EIT_SCH, &ulMessage);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Print("MW_EPG_MakeEpgMessage fail!!\n");
				goto EPGMAIN_FUCN_EXIT;
			}
			hError = SVC_EPG_SendMessage(ulMessage, hAction, (HUINT32)hService, (HUINT32)0);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Print("MW_EPG_MakeEpgMessage fail!!\n");
				goto EPGMAIN_FUCN_EXIT;
			}

			hError = ERR_EPG_OK;
			break;
	}

EPGMAIN_FUCN_EXIT :
	HxLOG_Debug("Leave (retVal:0x%X)!!\n",hError);
	return hError;
}


STATIC HERROR xsvc_epg_StartLivePF_Base(Handle_t hAction, Handle_t hService)
{
	HERROR				hError = ERR_EPG_FAIL;
	HUINT32				ulMessage = 0;
	DbSvc_Info_t		stSvcInfo;
	EPG_SpecType_e		eSpec = eEPG_SPEC_BASE;

	HxLOG_Debug("Enter!!\n");

	HxSTD_MemSet(&stSvcInfo, 0, sizeof(DbSvc_Info_t));

	hError = DB_SVC_GetServiceInfo(hService, &stSvcInfo);
	if (hError != ERR_OK)
	{
		HxLOG_Print("DB_SVC_GetServiceInfo fail!!\n");
		goto EPGMAIN_FUCN_EXIT;
	}
	hError = SVC_EPG_GetSpec(DbSvc_GetDeliType(stSvcInfo), hService, &eSpec);
	if (hError != ERR_OK)
	{
		HxLOG_Print("SVC_EPG_GetSpec fail!!\n");
		goto EPGMAIN_FUCN_EXIT;
	}

	switch (eSpec)
	{
		case eEPG_SPEC_FSAT:
			HxLOG_Error(": Don't start in fsat \n");
			hError = ERR_EPG_OK;
			break;
		default:
			hError = SVC_EPG_MakeEpgMessage(EPG_CMD_START,  eEPG_PROCESS_EIT_PF, &ulMessage);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Print("MW_EPG_MakeEpgMessage fail!!\n");
				goto EPGMAIN_FUCN_EXIT;
			}
			hError = SVC_EPG_SendMessage(ulMessage, hAction, (HUINT32)hService, (HUINT32)0);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Print("MW_EPG_MakeEpgMessage fail!!\n");
				goto EPGMAIN_FUCN_EXIT;
			}
			hError = ERR_EPG_OK;
			break;
	}

EPGMAIN_FUCN_EXIT :

	HxLOG_Debug("Leave (retVal:0x%X)!!\n",hError);
	return hError;
}


STATIC HERROR xsvc_epg_StartLiveSCH_Base(Handle_t hAction, Handle_t hService)
{
	HERROR				hError = ERR_EPG_FAIL;
	HUINT32				ulMessage = 0;
	DbSvc_Info_t		stSvcInfo;
	EPG_SpecType_e		eSpec = eEPG_SPEC_BASE;

	HxLOG_Debug("Enter!!\n");

	HxSTD_MemSet(&stSvcInfo, 0, sizeof(DbSvc_Info_t));

	hError = DB_SVC_GetServiceInfo(hService, &stSvcInfo);
	if (hError != ERR_OK)
	{
		HxLOG_Print("DB_SVC_GetServiceInfo fail!!\n");
		goto EPGMAIN_FUCN_EXIT;
	}
	hError = SVC_EPG_GetSpec(DbSvc_GetDeliType(stSvcInfo), hService, &eSpec);
	if (hError != ERR_OK)
	{
		HxLOG_Print("SVC_EPG_GetSpec fail!!\n");
		goto EPGMAIN_FUCN_EXIT;
	}

	switch (eSpec)
	{
		case eEPG_SPEC_FSAT:
			HxLOG_Error(": Don't start in fsat \n");
			hError = ERR_EPG_OK;
			break;
		default:
			hError = SVC_EPG_MakeEpgMessage(EPG_CMD_START,  eEPG_PROCESS_EIT_SCH, &ulMessage);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Print("MW_EPG_MakeEpgMessage fail!!\n");
				goto EPGMAIN_FUCN_EXIT;
			}
			hError = SVC_EPG_SendMessage(ulMessage, hAction, (HUINT32)hService, (HUINT32)0);
			if(hError != ERR_EPG_OK)
			{
				HxLOG_Print("MW_EPG_MakeEpgMessage fail!!\n");
				goto EPGMAIN_FUCN_EXIT;
			}
			hError = ERR_EPG_OK;
			break;
	}

EPGMAIN_FUCN_EXIT :

	HxLOG_Debug("Leave (retVal:0x%X)!!\n",hError);
	return hError;
}


STATIC HERROR xsvc_epg_StopLive_Base(Handle_t hAction)
{
	HERROR				hError = ERR_EPG_FAIL;
	HUINT32				ulMessage = 0;

	HxLOG_Debug("Enter!!\n");

#if defined(CONFIG_OP_FREESAT)
	//local_epgmain_StopFsatLive(hAction);
	SVC_EPG_Stop(eEPG_KIND_FSAT, eEPG_EVT_KIND_ALL, hAction);
#endif

	hError = SVC_EPG_MakeEpgMessage(EPG_CMD_STOP,  eEPG_PROCESS_EIT_PF, &ulMessage);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("MW_EPG_MakeEpgMessage fail!!\n");
		goto EPGMAIN_FUCN_EXIT;
	}

	hError = SVC_EPG_SendMessage(ulMessage, hAction, (HUINT32)HANDLE_NULL, 0);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("MW_EPG_MakeEpgMessage fail!!\n");
		goto EPGMAIN_FUCN_EXIT;
	}

	hError = SVC_EPG_MakeEpgMessage(EPG_CMD_STOP,  eEPG_PROCESS_EIT_SCH, &ulMessage);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("MW_EPG_MakeEpgMessage fail!!\n");
		goto EPGMAIN_FUCN_EXIT;
	}

	hError = SVC_EPG_SendMessage(ulMessage, hAction, (HUINT32)HANDLE_NULL, 0);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("MW_EPG_MakeEpgMessage fail!!\n");
		goto EPGMAIN_FUCN_EXIT;
	}

	hError = ERR_EPG_OK;

EPGMAIN_FUCN_EXIT :

	HxLOG_Debug("Leave (retVal:0x%X)!!\n",hError);
	return hError;
}

STATIC HERROR xsvc_epg_StopLivePF_Base(Handle_t hAction)
{
	HERROR				hError = ERR_EPG_FAIL;
	HUINT32				ulMessage = 0;

	HxLOG_Debug("Enter!!\n");

#if defined(CONFIG_OP_FREESAT)
	//local_epgmain_StopFsatLive(hAction);
	SVC_EPG_Stop(eEPG_KIND_FSAT, eEPG_EVT_KIND_ALL, hAction);
#endif

	hError = SVC_EPG_MakeEpgMessage(EPG_CMD_STOP,  eEPG_PROCESS_EIT_PF, &ulMessage);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("MW_EPG_MakeEpgMessage fail!!\n");
		goto EPGMAIN_FUCN_EXIT;
	}

	hError = SVC_EPG_SendMessage(ulMessage, hAction, (HUINT32)HANDLE_NULL, 0);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("MW_EPG_MakeEpgMessage fail!!\n");
		goto EPGMAIN_FUCN_EXIT;
	}

	hError = ERR_EPG_OK;

EPGMAIN_FUCN_EXIT :

	HxLOG_Debug("Leave (retVal:0x%X)!!\n",hError);
	return hError;
}

STATIC HERROR xsvc_epg_StopLiveSCH_Base(Handle_t hAction)
{
	HERROR				hError = ERR_EPG_FAIL;
	HUINT32				ulMessage = 0;

	HxLOG_Debug("Enter!!\n");

#if defined(CONFIG_OP_FREESAT)
	//local_epgmain_StopFsatLive(hAction);
	SVC_EPG_Stop(eEPG_KIND_FSAT, eEPG_EVT_KIND_ALL, hAction);
#endif

	hError = SVC_EPG_MakeEpgMessage(EPG_CMD_STOP,  eEPG_PROCESS_EIT_SCH, &ulMessage);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("MW_EPG_MakeEpgMessage fail!!\n");
		goto EPGMAIN_FUCN_EXIT;
	}

	hError = SVC_EPG_SendMessage(ulMessage, hAction, (HUINT32)HANDLE_NULL, 0);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("MW_EPG_MakeEpgMessage fail!!\n");
		goto EPGMAIN_FUCN_EXIT;
	}

	hError = ERR_EPG_OK;

EPGMAIN_FUCN_EXIT :

	HxLOG_Debug("Leave (retVal:0x%X)!!\n",hError);
	return hError;
}

/* =============================================================================
 * EPG Context
 * ============================================================================= */

// TODO: should be implemented


/*********************** End of File ******************************/
