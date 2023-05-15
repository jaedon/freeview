/**
	@file     xmgr_epg_rf_base.c File Name을 적습니다.
	@brief    file_name & simple comment.

	Description: dama 를  통한 rf epg를 managering 한다.	\n
	Module: APP                                 			 	\n
	Remarks : 													\n

	Copyright (c) 2013 HUMAX Co., Ltd.							\n
	All rights reserved.										\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <db_param.h>


#include <mgr_action.h>
#include <mgr_live.h>
#include <svc_pipe.h>
#include <svc_sys.h>
#include <svc_epg.h>
#include <svc_fs.h>

#include <mgr_epg.h>

#include "../local_include/_xmgr_epg.h"

/*******************************************************************/
/********************      Define    *************************/
/*******************************************************************/
#define	RM_EPG_RM_PAST_DATA_TIMER	0x18c8

/*******************************************************************/
/********************      Static Function ProtoTypes    *********************/
/*******************************************************************/
#define _____Static_Function_ProtoTypes_____
STATIC BUS_Result_t xproc_epg_rf_base(HINT32 message, Handle_t action, HINT32 p1, HINT32 p2, HINT32 p3);


#define _____Static_Function_____
STATIC void	xmgr_epg_rf_base_OnDapiStatusChanged (const HCHAR *name, DAxSTATUS_e status, void *userdata, ...)
{
	va_list	ap;
	HUINT32	count;

	HxLOG_Trace();

	if (status != eDAPI_LIMIT)
		return;

	va_start(ap, userdata);
	count = va_arg(ap, HUINT32);
	va_end(ap);

	BUS_PostMessage(xproc_epg_rf_base, eMEVT_EPG_USER_COMMAND, (Handle_t)eHMXEPGCMD_RF_OVERFLOW, (HINT32)HLIB_STD_StrDup(name), (HINT32)count, 0);
}

STATIC void	xmgr_epg_rf_base_OnRfOverflow (const HCHAR *name, HINT32 count)
{
	HUINT32		actionId;
	Handle_t	hService;
	HINT32		sd;
	DbSvc_Info_t	svcInfo;
	DbSvc_Info_t	*svcList;
	HINT32		i, n;
	DbSvc_Condition_t	cond;

	HxLOG_Debug("%s(%s, count:%d)\n", __FUNCTION__, name, count);

	actionId = MGR_ACTION_GetMainActionId();
	if (MGR_ACTION_GetType(actionId) != eViewType_WTV)
	{
		HxLOG_Debug("%s() only view action available!\n", __FUNCTION__);
		return;
	}

	if(MGR_LIVE_CurrentSvcInfoGet(actionId, &hService) != ERR_OK)
		return;

	if(DB_SVC_GetServiceInfo(hService, &svcInfo) != ERR_OK)
		return;

	DB_SVC_InitSvcFindCondition(&cond);
	cond.nTsIdx = DbSvc_GetTsIdx(&svcInfo);
	if (DB_SVC_FindSvcInfoList(&cond, &n, &svcList) != ERR_OK)
	{
		HxLOG_Error("%s() cannot find service list by tsidx(%d)\n", __FUNCTION__, DbSvc_GetTsIdx(&svcInfo));
		return;
	}

	sd = HLIB_RWSTREAM_Open(NULL, 0);
	if (!sd)
	{
		HxLOG_Error("%s() out of memory!\n", __FUNCTION__);
		DB_SVC_FreeServiceInfoList(NULL, NULL, svcList);
		return;
	}

	HLIB_RWSTREAM_Print(sd, "DELETE FROM %s WHERE svcuid NOT IN(", name);
	for (i = 0 ; i < n ; i++)
	{
		if (i > 0)
			HLIB_RWSTREAM_PutChar(sd, ',');
		HLIB_RWSTREAM_Print(sd, "%d", DbSvc_GetSvcIdx(&svcList[i]));
	}
	HLIB_RWSTREAM_Print(sd, ")");
	DB_SVC_FreeServiceInfoList(NULL, NULL, svcList);
	HxLOG_Debug("%s() \'%s\'\n", __FUNCTION__, HLIB_RWSTREAM_GetBuf(sd));

	DAPI_Query(HLIB_RWSTREAM_GetBuf(sd), NULL, NULL);
	HLIB_RWSTREAM_Close(sd);
}

STATIC void		xmgr_epg_rf_base_RemovePastData (void)
{
	HCHAR	*query;
	HUINT32	time;
	HUINT32	n;
	static const DxEPG_Type_e	s_types[] = {eDxEPG_TYPE_PF, eDxEPG_TYPE_SC};

	HxLOG_Trace();

	time  = HLIB_STD_GetSystemTime();

	n = sizeof(s_types) / sizeof(s_types[0]);
	while (n--)
	{
		query = HLIB_STD_FmtString(NULL, 0, "DELETE FROM %s WHERE end_time < %u", DxEPG_NAMEOF(s_types[n]), time);
		if (query)
		{
			HxLOG_Info(HxANSI_COLOR_CYAN("remove past data: %s\n"), query);
			DAPI_Query(query, NULL, NULL);
			HLIB_STD_FreeFmtString(query);
		}
	}
}

STATIC void		xmgr_epg_rf_base_OnTimeout (HINT32 timerId)
{
	if (timerId == RM_EPG_RM_PAST_DATA_TIMER)
	{
		xmgr_epg_rf_base_RemovePastData();
	}
}


STATIC void		xmgr_epg_rf_base_OnUserCommand (HINT32 cmd, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxLOG_Trace();
	HxLOG_Debug("%s(cmd:%d, p1:0x%X, p2:0x%X, p3:0x%X)\n", __FUNCTION__, cmd, p1, p2, p3);

	switch (cmd)
	{
	case eHMXEPGCMD_RF_OVERFLOW:
		xmgr_epg_rf_base_OnRfOverflow((HCHAR *)p1, p2);
		if (p1)
			HLIB_STD_MemFree((void *)p1);
		break;

	default:
		break;
	}
}

STATIC BUS_Result_t xproc_epg_rf_base(HINT32 message, Handle_t action, HINT32 p1, HINT32 p2, HINT32 p3)
{
	static HBOOL bCreated = FALSE;
	switch (message)
	{
	case eMEVT_BUS_CREATE:
		HxLOG_Debug("%s(eMEVT_BUS_CREATE)\n", __FUNCTION__);
		if (bCreated == FALSE)
		{
 			DAPI_AddStatusMonitor(DxNAMEOF(eDxEPG_TYPE_PF), xmgr_epg_rf_base_OnDapiStatusChanged, NULL);
			DAPI_AddStatusMonitor(DxNAMEOF(eDxEPG_TYPE_SC), xmgr_epg_rf_base_OnDapiStatusChanged, NULL);
 			BUS_SetTimer(RM_EPG_RM_PAST_DATA_TIMER, 1000 * 60 * 10);
			bCreated = TRUE;
		}
		break;

	case eMEVT_BUS_DESTROY:
		HxLOG_Critical("%s(eMEVT_BUS_DESTROY)\n", __FUNCTION__);
		BUS_KillTimer(RM_EPG_RM_PAST_DATA_TIMER);
		break;

	case eMEVT_BUS_TIMER:
		HxLOG_Debug("%s(eMEVT_BUS_TIMER)\n", __FUNCTION__);
		xmgr_epg_rf_base_OnTimeout(p1);
		break;

	case eMEVT_EPG_USER_COMMAND:
		HxLOG_Debug("%s(eMEVT_EPG_USER_COMMAND)\n", __FUNCTION__);
		xmgr_epg_rf_base_OnUserCommand((HINT32)action, p1, p2, p3);
		break;

	case eSEVT_DB_SVCLIST_CHANGED_BY_DAMA:
		SVC_EPG_CreateCheckServiceList();

	default:
		break;
	}
	return MESSAGE_PASS;
}


HERROR xmgr_epg_RfStart_Base (void)
{
	BUS_Result_t	eRes = ERR_BUS_NO_OBJECT;

	eRes = BUS_MGR_Create("xproc_epg_rf_base", APP_DEFAULT_PRIORITY, xproc_epg_rf_base, 0, 0, 0, 0);

	return (eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
}

/*********************** End of File ******************************/
