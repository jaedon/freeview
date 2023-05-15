/**
	@file     xmgr_epg_humax.c File Name을 적습니다.
	@brief    file_name & simple comment.

	Description: Network 을 통한 humax epg를 managering 한다.	\n
	Module: APP                                 			 	\n
	Remarks : 													\n

	Copyright (c) 2011 HUMAX Co., Ltd.							\n
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
#define	HMXEPG_RM_PAST_DATA_TIMER	0x18c8

typedef struct
{
	HxVector_t	*list;
	HxVector_t	*changes;
} HumaxEpgMgr_t;

static HumaxEpgMgr_t	s_humaxEpgMgr;

STATIC BUS_Result_t xproc_epg_ip_humax_Ukdtt(HINT32 message, Handle_t action, HINT32 p1, HINT32 p2, HINT32 p3);

STATIC HINT32	xmgr_epg_ip_humax_Compare_Ukdtt (const void *arg1, const void *arg2)
{
#ifndef CONFIG_FUNC_EPG_USE_SVCUID
	return HxSTD_StrCmp(arg1, arg2);
#else
	const DxIpService_t *lhs = *(const DxIpService_t **)arg1;
	const DxIpService_t *rhs = *(const DxIpService_t **)arg2;

	if (lhs->ulSvcUid > rhs->ulSvcUid)
		return +1;
	if (lhs->ulSvcUid < rhs->ulSvcUid)
		return -1;

	return 0;
#endif
}

STATIC void		xmgr_epg_ip_humax_RestartEPG_Ukdtt (HumaxEpgMgr_t *mgr)
{
	HUINT32		actionId;
	Handle_t	hAction;
	Handle_t	hService;
	HERROR		err;

	HxLOG_Debug("%s()\n", __FUNCTION__);

	actionId = MGR_ACTION_GetMainActionId();
	if (MGR_ACTION_GetType(actionId) != eViewType_WTV)
	{
		HxLOG_Debug("%s() only view action available!\n", __FUNCTION__);
		return;
	}
	hAction  = SVC_PIPE_GetActionHandle(actionId);

	err = MGR_LIVE_CurrentSvcInfoGet(actionId, &hService);
	if(ERR_OK != err)
	{ /* fix for prevent #170087 */
		HxLOG_Warning("Error(%d) in MGR_LIVE_CurrentSvcInfoGet\n", err);
	}

	err  = SVC_EPG_Stop (eEPG_KIND_DVB, eEPG_EVT_KIND_ALL , hAction);
	err |= SVC_EPG_Start(eEPG_KIND_DVB, eEPG_EVT_KIND_PF_ALL , hAction, hService, 0, 0, 0);
	err |= SVC_EPG_Start(eEPG_KIND_DVB, eEPG_EVT_KIND_SCH_ALL, hAction, hService, 0, 0, 0);

	HxLOG_Debug("%s() EPG Restart!!\n", __FUNCTION__);
}

STATIC void		xmgr_epg_ip_humax_OnDapiReceived_Ukdtt (const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata)
{
	DAxIterator_t	iter;
	HxVector_t		*list;
	DxIpService_t	*svc;

	//2 ASYNC FUNCTION으로 s_humaxEpgMgr 접근 불가!!!!
	HxLOG_Info("%s(%s) data:0x%X, size:%u\n", __FUNCTION__, name, data, size);

	if (data && size)
	{
		list = HLIB_VECTOR_NewEasy(128, (HxFreeFunc_t)HLIB_STD_MemFree_CB, (HxCompareFunc_t)xmgr_epg_ip_humax_Compare_Ukdtt);
		if (list)
		{
			DAPI_InitIterator(&iter, data, size, sizeof(DxIpService_t));
			while (DAPI_NextIterator(&iter))
			{
#ifndef CONFIG_FUNC_EPG_USE_SVCUID
				svc = (DxIpService_t *)iter.data;
				if (svc)
				{
					HCHAR	buf[32];
					HxSTD_PrintToStrN(buf, 32, "%x.%x.%x", svc->usOnId, svc->usTsId, svc->usSvcId);
					HLIB_VECTOR_Add(list, (void *)OxAP_StrDup(buf));
				}
#else
				svc = (DxIpService_t *)HLIB_STD_MemDup(iter.data, sizeof(DxIpService_t));
				if (svc)
				{
					// TODO: ip service에 만약 ip epg가 있는지 여부가 포함된다면, 그에 따라 관리해줘야 함
					HLIB_VECTOR_Add(list, (void *)svc);
				}
#endif
			}
			HxVECTOR_DELETE_IF_EMPTY(list);
		}
	}
	else list = NULL;

	if (list)
		HLIB_VECTOR_Sort(list, NULL);

	BUS_PostMessage(xproc_epg_ip_humax_Ukdtt, eMEVT_EPG_USER_COMMAND, (Handle_t)eHMXEPGCMD_IPSERVICE_UPDATED, (HINT32)list, (HINT32)userdata, 0);
}

STATIC void		xmgr_epg_ip_humax_OnDapiUpdated_Ukdtt (const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata)
{
	//2 ASYNC FUNCTION으로 s_humaxEpgMgr 접근 불가!!!!
	(void)uid;
	(void)data;
	(void)size;
	(void)userdata;

	HxLOG_Info("%s(%s)\n", __FUNCTION__, name);
	DAPI_GetAll(name, xmgr_epg_ip_humax_OnDapiReceived_Ukdtt, NULL);
}


STATIC void		xmgr_epg_ip_humax_OnDapiStatusChanged_Ukdtt (const HCHAR *name, DAxSTATUS_e status, void *userdata, ...)
{
	va_list	ap;
	HUINT32	count;

	if (status != eDAPI_LIMIT)
		return;

	va_start(ap, userdata);
	count = va_arg(ap, HUINT32);
	va_end(ap);

	BUS_PostMessage(xproc_epg_ip_humax_Ukdtt, eMEVT_EPG_USER_COMMAND, (Handle_t)eHMXEPGCMD_RF_OVERFLOW, (HINT32)HLIB_STD_StrDup(name), (HINT32)count, 0);
}


STATIC void		xmgr_epg_ip_humax_OnIpServiceUpdated_Ukdtt (HumaxEpgMgr_t *mgr, HxVector_t *list, HBOOL firstTime)
{
	HxVector_t		*changes;

	HxLOG_Trace();
	HxLOG_Debug("%s(mgr->list:0x%X, list:0x%X)\n", __FUNCTION__, (int)mgr->list, (int)list);

	if (list == NULL)
	{
		if (mgr->list == NULL)
		{
			HxLOG_Debug("%s() do nothing...\n", __FUNCTION__);
			return;
		}
		HxLOG_Debug("%s() All IP services are removed (maybe 'turn off')\n", __FUNCTION__);

		SVC_EPG_SetServiceConstraint(eOxEPG_TYPE_DVB&(~eOxEPG_TYPE_PF), NULL);
		HLIB_VECTOR_Delete(mgr->list);
		mgr->list = NULL;
		if (!firstTime)
			xmgr_epg_ip_humax_RestartEPG_Ukdtt(mgr);
		return;
	}

	changes = mgr->changes;
	HLIB_VECTOR_RemoveAll(changes);

	{
#ifndef CONFIG_FUNC_EPG_USE_SVCUID
		HCHAR	*tripleid;

		HxVECTOR_EACH(list, HCHAR *, tripleid, {
			if (mgr->list == NULL || HLIB_VECTOR_BSearch(mgr->list, (void *)tripleid, NULL) < 0)
			{
				HLIB_VECTOR_Add(changes, (void *)OxAP_StrDup(tripleid));
			}
		});
#else
		DxIpService_t	*svc;

		HxVECTOR_EACH(list, DxIpService_t *, svc, {
			if (mgr->list == NULL || HLIB_VECTOR_BSearch(mgr->list, (void *)svc, NULL) < 0)
			{
				HLIB_VECTOR_Add(changes, (void *)svc->ulSvcUid);
			}
		});
#endif
	}

	if (HLIB_VECTOR_Length(changes) > 0)
	{
		if (!firstTime)
		{
			HINT32 c  = 0;
			HINT32 sd = HLIB_RWSTREAM_Open(NULL, 0);
			if (sd)
			{
#ifndef CONFIG_FUNC_EPG_USE_SVCUID
				HCHAR *	tripleid;

				HLIB_RWSTREAM_Print(sd, "DELETE FROM %s WHERE tripleid IN(", DxNAMEOF(eDxEPG_TYPE_SC));
				HxVECTOR_EACH(changes, HCHAR *, tripleid, {
					if (c++ > 0)
						HLIB_RWSTREAM_PutChar(sd, ',');
					HLIB_RWSTREAM_Print(sd, "\'%s\'", tripleid);
				});
#else
				HINT32			uid;

				HLIB_RWSTREAM_Print(sd, "DELETE FROM %s WHERE svcuid IN(", DxNAMEOF(eDxEPG_TYPE_SC));
				HxVECTOR_EACH(changes, HINT32, uid, {
					if (c++ > 0)
						HLIB_RWSTREAM_PutChar(sd, ',');
					HLIB_RWSTREAM_Print(sd, "%d", uid);
				});
#endif
				HLIB_RWSTREAM_Print(sd, ")");

				HxLOG_Debug("%s() SQL:\'%s\'\n", __FUNCTION__, HLIB_RWSTREAM_GetBuf(sd));
				DAPI_Query(HLIB_RWSTREAM_GetBuf(sd), NULL, NULL);
				HLIB_RWSTREAM_Close(sd);
			}
		}
		if (mgr->list)
			HLIB_VECTOR_Delete(mgr->list);
		mgr->list = list;

		HxLOG_Debug("%s(): IP services(count:%d) updated!\n", __FUNCTION__, HLIB_VECTOR_Length(list));
		SVC_EPG_SetServiceConstraint(eOxEPG_TYPE_DVB&(~eOxEPG_TYPE_PF), changes);
		if (!firstTime)
			xmgr_epg_ip_humax_RestartEPG_Ukdtt(mgr);
	}
	else
	{
		HxLOG_Debug("%s(): There is no changes!\n", __FUNCTION__);
		HLIB_VECTOR_Delete(list);
	}
}

STATIC void		xmgr_epg_ip_humax_OnRfOverflow_Ukdtt (HumaxEpgMgr_t *mgr, const HCHAR *name, HINT32 count)
{
	HERROR				hErr;
	HUINT32				actionId;
	Handle_t			hAction;
	Handle_t			hService;
	HINT32				sd;
	DbSvc_Info_t		svcInfo;
	DbSvc_Info_t		*svcList;
	HINT32				i, n;
	DbSvc_Condition_t	cond;

	HxLOG_Debug("%s(%s, count:%d)\n", __FUNCTION__, name, count);

	actionId = MGR_ACTION_GetMainActionId();
	if (MGR_ACTION_GetType(actionId) != eViewType_WTV)
	{
		HxLOG_Debug("%s() only view action available!\n", __FUNCTION__);
		return;
	}
	hAction  = SVC_PIPE_GetActionHandle(actionId);

	hErr = MGR_LIVE_CurrentSvcInfoGet(actionId, &hService);
	if (ERR_OK != hErr)
	{
		HxLOG_Debug("%s() MGR_LIVE_CurrentSvcInfoGet(%d) Error\n", __FUNCTION__, actionId);
		return;
	}

	hErr = DB_SVC_GetServiceInfo(hService, &svcInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Debug("%s() DB_SVC_GetServiceInfo(%x) Error\n", __FUNCTION__, hService);
		return;
	}

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
#ifndef CONFIG_FUNC_EPG_USE_SVCUID
	HLIB_RWSTREAM_Print(sd, "DELETE FROM %s WHERE tripleid NOT IN(", name);
	for (i = 0 ; i < n ; i++)
	{
		if (i > 0)
			HLIB_RWSTREAM_PutChar(sd, ',');
		HLIB_RWSTREAM_Print(sd, "\'%x.%x.%x\'", DbSvc_GetOnId(&svcList[i]), DbSvc_GetTsId(&svcList[i]), DbSvc_GetSvcId(&svcList[i]));
	}
#else
	HLIB_RWSTREAM_Print(sd, "DELETE FROM %s WHERE svcuid NOT IN(", name);
	for (i = 0 ; i < n ; i++)
	{
		if (i > 0)
			HLIB_RWSTREAM_PutChar(sd, ',');
		HLIB_RWSTREAM_Print(sd, "%d", DbSvc_GetSvcIdx(&svcList[i]));
	}
#endif
	HLIB_RWSTREAM_Print(sd, ")");
	DB_SVC_FreeServiceInfoList(NULL, NULL, svcList);
#if 0
	if (strcmp(name, DxNAMEOF(eDxEPG_TYPE_SC)) == 0)
	{
		HUINT32 time = HLIB_STD_GetSystemTime();
		HLIB_RWSTREAM_Print(sd, " AND start_time > %u", time + (60 * 60 * 24/*24 hours*/));
	}
#endif
	HxLOG_Debug("%s() \'%s\'\n", __FUNCTION__, HLIB_RWSTREAM_GetBuf(sd));

	DAPI_Query(HLIB_RWSTREAM_GetBuf(sd), NULL, NULL);
	HLIB_RWSTREAM_Close(sd);
}

STATIC void		xmgr_epg_ip_humax_RemovePastData_Ukdtt (void)
{
	HCHAR	*query;
	HUINT32	time;
	HUINT32	n;
	static const DxEPG_Type_e	s_types[] = {
		eDxEPG_TYPE_PF, eDxEPG_TYPE_SC
	};

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

STATIC void		xmgr_epg_ip_humax_OnTimeout_Ukdtt (HumaxEpgMgr_t *mgr, HINT32 timerId)
{
	if (timerId == HMXEPG_RM_PAST_DATA_TIMER)
	{
		// 이 기능은 base로 들어가야 하지만, 일단 지금은 이렇게...
		xmgr_epg_ip_humax_RemovePastData_Ukdtt();
	}
}


STATIC void		xmgr_epg_ip_humax_OnUserCommand_Ukdtt (HumaxEpgMgr_t *mgr, HINT32 cmd, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxLOG_Trace();
	HxLOG_Debug("%s(cmd:%d, p1:0x%X, p2:0x%X, p3:0x%X)\n", __FUNCTION__, cmd, p1, p2, p3);

	switch (cmd)
	{
	case eHMXEPGCMD_IPSERVICE_UPDATED:
		xmgr_epg_ip_humax_OnIpServiceUpdated_Ukdtt(mgr, (HxVector_t *)p1, (HBOOL)p2);
		break;

	case eHMXEPGCMD_RF_OVERFLOW:
		xmgr_epg_ip_humax_OnRfOverflow_Ukdtt(mgr, (HCHAR *)p1, p2);
		if (p1)
		{
			HLIB_STD_MemFree((void *)p1);
		}
		break;

	default:
		break;
	}
}

#ifndef CONFIG_FUNC_EPG_USE_SVCUID
void xmgr_epg_ip_humax_tripleFree(void * testMem)
{
	HLIB_STD_MemFree(testMem);
}
#endif

STATIC BUS_Result_t xproc_epg_ip_humax_Ukdtt(HINT32 message, Handle_t action, HINT32 p1, HINT32 p2, HINT32 p3)
{
	switch (message)
	{
	case eMEVT_BUS_CREATE:
		HxLOG_Debug("%s(eMEVT_BUS_CREATE)\n", __FUNCTION__);
		if (s_humaxEpgMgr.changes == NULL)
		{
#ifndef CONFIG_FUNC_EPG_USE_SVCUID
			s_humaxEpgMgr.changes = HLIB_VECTOR_NewEasy(128, (HxFreeFunc_t)xmgr_epg_ip_humax_tripleFree/*HLIB_STD_MemFree*/, (HxCompareFunc_t)xmgr_epg_ip_humax_Compare);
#else
			s_humaxEpgMgr.changes = HLIB_VECTOR_New(NULL, NULL);
#endif

			DAPI_GetAll(DxNAMEOF(DxIpService_t), xmgr_epg_ip_humax_OnDapiReceived_Ukdtt, (void *)TRUE);
			DAPI_AddTableNotifier(DxNAMEOF(DxIpService_t), xmgr_epg_ip_humax_OnDapiUpdated_Ukdtt, NULL);
			DAPI_AddStatusMonitor(DxNAMEOF(eDxEPG_TYPE_SC), xmgr_epg_ip_humax_OnDapiStatusChanged_Ukdtt, NULL);

			BUS_SetTimer(HMXEPG_RM_PAST_DATA_TIMER, 1000 * 60 * 10);
		}
		break;

	case eMEVT_BUS_DESTROY:
		HxLOG_Warning("%s(eMEVT_BUS_DESTROY)\n", __FUNCTION__);
		BUS_KillTimer(HMXEPG_RM_PAST_DATA_TIMER);
		break;

	case eMEVT_BUS_TIMER:
		xmgr_epg_ip_humax_OnTimeout_Ukdtt(&s_humaxEpgMgr, p1);
		break;

	case eMEVT_EPG_USER_COMMAND:
		xmgr_epg_ip_humax_OnUserCommand_Ukdtt(&s_humaxEpgMgr, (HINT32)action, p1, p2, p3);
		break;

	default:
		break;
	}
	return MESSAGE_PASS;
}


HERROR xmgr_epg_IpHumaxStart_Ukdtt (void)
{
	BUS_Result_t	eRes = ERR_BUS_NO_OBJECT;

	eRes = BUS_MGR_Create("xproc_epg_ip_humax", APP_DEFAULT_PRIORITY, xproc_epg_ip_humax_Ukdtt, 0, 0, 0, 0);

	return (eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
}

/*********************** End of File ******************************/
