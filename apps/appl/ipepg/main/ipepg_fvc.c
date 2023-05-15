/**************************************************************
 *	@file		ipepg_fvc.c
 *	IP EPG downloader for OCTO 2.0
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/08/01
 *	@author			humax
 *	@brief			IP EPG Downloader for OCTO 2.0 Project
 **************************************************************/
#include <unistd.h>
#include <signal.h>
#include <vkernel.h>
#include "../include/ipepg.h"
#include "ipepg_manager.h"
#include "ipepg_downloader.h"
#include "ipepg_fvc.h"

#if defined(CFG_OPEN_DEBUG)
#undef HxLOG_Trace
#define HxLOG_Trace()	printf("[IPEPG] %s %d\n", __FUNCTION__, __LINE__);
#undef HxLOG_Debug
#define HxLOG_Debug		printf
#undef HxLOG_Print
#define HxLOG_Print		printf
#endif

#ifdef IPEPG_FUNCTION_COLOR_TRACE
	#define IPEPG_FUNC_START()	HxLOG_Debug( HxANSI_COLOR_CYAN("[%s][%d] +++ \n") , __FUNCTION__, __LINE__);
	#define IPEPG_FUNC_END()	HxLOG_Debug(  HxANSI_COLOR_CYAN("[%s][%d] --- \n") , __FUNCTION__, __LINE__);
#else
	#define IPEPG_FUNC_START()	HxLOG_Debug("[%s][%d] +++ \n" , __FUNCTION__, __LINE__);
	#define IPEPG_FUNC_END()	HxLOG_Debug("[%s][%d] --- \n" , __FUNCTION__, __LINE__);
#endif


extern HCHAR*	UAPI_FM_GetDefaultURL(HCHAR *pszDefaultURL, HCHAR *pszURLFileName);

#define _____________________Fvc_Module


HUINT32	ipepg_GetUidHashKey	(HUINT32 usOnId, HUINT32 utStartTime)
{
	// 60*60*3: FVC의 api는 3hr 간격으로 query 가능
	return (HUINT32)((HUINT32)(usOnId<<16) | ((utStartTime/(60*60*3))&0xFFFF));
}

STATIC void ipepg_OnDamaFvcsReceived(const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata)
{
	DxFvcExpiry_t		*fvc;
	DAxIterator_t		iter;
	HxHASH_t			**pstExpiryHash = (HxHASH_t**)userdata;
	HxHASH_t			*pstHash = NULL;

	pstHash = *pstExpiryHash;
	DAPI_InitIterator(&iter, data, size, 0);
	while (DAPI_NextIterator(&iter))
	{
		fvc = (DxFvcExpiry_t *)DLIB_NewType(iter.name, iter.uid, iter.data, iter.size);
		if (fvc)
		{
			DxFvcExpiry_t	*pstNewFvc;
			pstNewFvc = (DxFvcExpiry_t *)IPEPG_MemDup(fvc, sizeof(DxFvcExpiry_t));

			HLIB_HASH_Insert(pstHash, (void *)ipepg_GetUidHashKey(pstNewFvc->onid, pstNewFvc->startTime), (void*)pstNewFvc);
		}
		DLIB_FreeType(iter.name, fvc);
	}

	*pstExpiryHash = pstHash;

	return;
}

STATIC void ipepg_OnDamaIpSvcsReceived(const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata)
{
	DxIpService_t		*ipSvc;
	DAxIterator_t		iter;
	HxList_t			**ppstIpSvcList = (HxList_t**)userdata;
	HxList_t			*pstList = NULL;

	pstList = *ppstIpSvcList;
	DAPI_InitIterator(&iter, data, size, 0);
	while (DAPI_NextIterator(&iter))
	{
		ipSvc = (DxIpService_t *)DLIB_NewType(iter.name, iter.uid, iter.data, iter.size);
		if (ipSvc)
		{
			DxIpService_t	*pstNewIpSvc;
			pstNewIpSvc = (DxIpService_t *)IPEPG_MemDup(ipSvc, sizeof(DxIpService_t));
			pstList = HLIB_LIST_Append(pstList,pstNewIpSvc);
		}
		DLIB_FreeType(iter.name, ipSvc);
	}

	*ppstIpSvcList = pstList;

	return;
}

static	HERROR	ipepg_GetAllIpSvcs (HxList_t	**ppstIpSvcList)
{
	HCHAR				query[1024] = {0,};
	HUINT32 			id;
	DERROR				dErr;

	HxSTD_snprintf(query, 1024,"select * from DxIpService_t");

	HxLOG_Debug("==============================================================================\n");
	HxLOG_Debug("query:\n\t%s\n", query);
	HxLOG_Debug("==============================================================================\n");

	id = DAPI_Query(query, (DAPI_Getter)ipepg_OnDamaIpSvcsReceived, (void*)ppstIpSvcList);
	HxLOG_Debug("DAPI_Query() id: %u \n", id);

	dErr = DAPI_Wait(10000, id);
	if (id == 0 || dErr != DERR_OK)
	{
		HxLOG_Error("Query FAIL \n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR	ipepg_GetAllFvcData (HxHASH_t **ppstExpiryHash)
{
	HCHAR				query[1024] = {0,};
	HUINT32 			id;
	DERROR				dErr;

	HxSTD_snprintf(query, 1024,"select * from DxFvcExpiry_t where type=%d"
								,eDxFvcExpiry_Type_Schedule);

	HxLOG_Debug("==============================================================================\n");
	HxLOG_Debug("query:\n\t%s\n", query);
	HxLOG_Debug("==============================================================================\n");

	id = DAPI_Query(query, (DAPI_Getter)ipepg_OnDamaFvcsReceived, (void*)ppstExpiryHash);
	HxLOG_Debug("DAPI_Query() id: %u \n", id);

	dErr = DAPI_Wait(10000, id);
	if (id == 0 || dErr != DERR_OK)
	{
		HxLOG_Error("Query FAIL \n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR	ipepg_GetNextExpiryTime (UNIXTIME *putNextExpiryTime)
{
	HCHAR				query[1024] = {0,};
	HUINT32 			id;
	HxHASH_t			*pstExpiryHash;
	HxHASH_TableIter_t	iter;
	DxFvcExpiry_t		*pstFvcExpiry = NULL;
	HERROR				hResult = ERR_FAIL;
	DERROR				dErr;

	pstExpiryHash = HLIB_HASH_NewFull(HLIB_HASH_GetIntKey, HLIB_HASH_IsIntEqual, NULL, HLIB_STD_MemFree_CB);

	HxSTD_snprintf(query, 1024,"select * from DxFvcExpiry_t where type=%d and statusCode!=%d ORDER BY expiryTime ASC LIMIT 1"
								,eDxFvcExpiry_Type_Schedule,IPEPG_FVC_STATUS_CODE_BAD);

	HxLOG_Debug("==============================================================================\n");
	HxLOG_Debug("query:\n\t%s\n", query);
	HxLOG_Debug("==============================================================================\n");

	id = DAPI_Query(query, (DAPI_Getter)ipepg_OnDamaFvcsReceived, (void*)&pstExpiryHash);
	HxLOG_Debug("DAPI_Query() id: %u \n", id);

	dErr = DAPI_Wait(10000, id);
	if (id == 0 || dErr != DERR_OK)
	{
		HLIB_HASH_Destroy(pstExpiryHash);
		HxLOG_Error("Query FAIL \n");
		return ERR_FAIL;
	}

	HLIB_HASH_IterInit(&iter, pstExpiryHash);
	while (HLIB_HASH_IterNext(&iter, NULL, (void **)&pstFvcExpiry))
	{
		if (pstFvcExpiry != NULL)
		{
			hResult = ERR_OK;
			*putNextExpiryTime = pstFvcExpiry->expiryTime;
			break;
		}
	}
	HLIB_HASH_Destroy(pstExpiryHash);

	HxLOG_Debug(HxANSI_COLOR_BLUE("next wake up %s \n"), HLIB_DATETIME_UnixTimeToString(*putNextExpiryTime,NULL,0));

	return hResult;
}


void	ipepg_DeleteOldExpirys (HxHASH_t	*pstExpiryHash)
{
	HxHASH_TableIter_t	iter;
	DxFvcExpiry_t		*iterFvc;
	HINT32 				nStream =0;
	HBOOL				isFirst = TRUE;
	HCHAR				*query = NULL;
	HUINT32				ulCount = 0;
	HUINT32 			id;

	IPEPG_RETURN_IF_VOID(pstExpiryHash == NULL);
	IPEPG_RETURN_IF_VOID(HLIB_HASH_Size(pstExpiryHash) == 0);

	nStream = HLIB_RWSTREAM_OpenEx(NULL, 512+IPEPG_FVC_MAX_DELETE_QUERY*16, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
	IPEPG_RETURN_IF_VOID(nStream ==0);
	HxLOG_Info(HxANSI_COLOR_BLUE(" delete size(%d)  \n"),__FUNCTION__, __LINE__,HLIB_HASH_Size(pstExpiryHash));

	HLIB_RWSTREAM_Print(nStream,"DELETE FROM %s WHERE uid in(" , DxNAMEOF(DxFvcExpiry_t));

	HLIB_HASH_IterInit(&iter, pstExpiryHash);
	while (HLIB_HASH_IterNext(&iter, NULL, (void **)&iterFvc))
	{
		++ulCount;

		if (ulCount > IPEPG_FVC_MAX_DELETE_QUERY)
		{
			HxLOG_Error("Can't delete more than %d uids per one request. \n", IPEPG_FVC_MAX_DELETE_QUERY);
			break;
		}
		if (isFirst)
		{
				HLIB_RWSTREAM_Print(nStream,"%lld",iterFvc->uid);
				isFirst = FALSE;
		}
		else
		{
			HLIB_RWSTREAM_Print(nStream,", %lld",iterFvc->uid);
		}

	}
	HLIB_RWSTREAM_Print(nStream,")");

	HxLOG_Info(HxANSI_COLOR_BLUE(" offset (%d)  \n"),HLIB_RWSTREAM_GetOffset(nStream));

	query = HLIB_RWSTREAM_CloseWithoutBuf(nStream);
	HxLOG_Debug("==============================================================================\n");
	HxLOG_Info("query:\n\t%s\n", query);
	HxLOG_Debug("==============================================================================\n");
	id = DAPI_Query(query,NULL, NULL);
	HxLOG_Debug("DAPI_Query() id: %u \n", id);


	if (query)
		HLIB_RWSTREAM_MemFree(query);
}

static void ipepg_CancelFvcTimer(void)
{
	gTimerFvcTime = 0;
	gbSetFvcTimer = FALSE;
}

static void ipepg_CancelFvcRetryTimer(void)
{
	gTimerFvcRetryTime = 0;
	gbSetFvcRetryTimer = FALSE;
}

static void ipepg_CancelFvcSeriesEpgTimer(void)
{
	gTimerFvcSeriesEpgTime = 0;
	gbSetFvcSeriesEpgTimer = FALSE;
}

static HERROR ipepg_SetFvcTimer(HUINT32 time)
{
	UNIXTIME ulSystemTime = 0;
	gbSetFvcTimer = TRUE;
	if(VK_CLOCK_GetTime((unsigned long*)&ulSystemTime) == ERR_OK)
	{
		gTimerFvcTime = ulSystemTime + time;
		IPEPG_FUNC_END();
		return ERR_OK;
	}
	return ERR_FAIL;
}

static HERROR ipepg_SetFvcRetryTimer(HUINT32 time)
{
	UNIXTIME ulSystemTime = 0;
	gbSetFvcRetryTimer = TRUE;
	if(VK_CLOCK_GetTime((unsigned long*)&ulSystemTime) == ERR_OK)
	{
		gTimerFvcRetryTime = ulSystemTime + time;
		IPEPG_FUNC_END();
		return ERR_OK;
	}
	return ERR_FAIL;
}

static HERROR ipepg_SetFvcSeriesEpgTimer(HUINT32 time)
{
	UNIXTIME ulSystemTime = 0;
	gbSetFvcSeriesEpgTimer = TRUE;
	if(VK_CLOCK_GetTime((unsigned long*)&ulSystemTime) == ERR_OK)
	{
		gTimerFvcSeriesEpgTime = ulSystemTime + time;
		IPEPG_FUNC_END();
		return ERR_OK;
	}
	return ERR_FAIL;
}


HERROR	IPEPG_GetScheduleExpiryHash (HxHASH_t **ppstExpiryHash, HUINT32 ulLimitSize)
{
	HCHAR				query[1024] = {0,};
	HUINT32 			id;
	UNIXTIME			utCurrTime;
	DERROR				dErr;

	utCurrTime = (UNIXTIME)HLIB_STD_GetSystemTime();


	HxSTD_snprintf(query, 1024,"select * from DxFvcExpiry_t where type=%d and expiryTime < %d and statusCode!=%d ORDER BY startTime ASC LIMIT %d"
								,eDxFvcExpiry_Type_Schedule, utCurrTime+1,IPEPG_FVC_STATUS_CODE_BAD, ulLimitSize);

	HxLOG_Debug("==============================================================================\n");
	HxLOG_Debug("query:\n\t%s\n", query);
	HxLOG_Debug("==============================================================================\n");

	id = DAPI_Query(query, (DAPI_Getter)ipepg_OnDamaFvcsReceived, (void*)ppstExpiryHash);
	HxLOG_Debug("DAPI_Query() id: %u \n", id);

	dErr = DAPI_Wait(100000, id);
	if (id == 0 || dErr != DERR_OK)
	{
		HxLOG_Error("Query FAIL \n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR	IPEPG_GetServiceExpiry (DxFvcExpiry_t	*pstFvcServiceExpiry)
{
	HCHAR				query[1024] = {0,};
	HUINT32 			id;

	HxHASH_t			*pstExpiryHash;
	HxHASH_TableIter_t	iter;
	DxFvcExpiry_t		*pstFvcExpiry = NULL;
	HERROR				hResult = ERR_FAIL;
	DERROR				dErr;

	pstExpiryHash = HLIB_HASH_NewFull(HLIB_HASH_GetIntKey, HLIB_HASH_IsIntEqual, NULL, HLIB_STD_MemFree_CB);

	HxSTD_snprintf(query, 1024,"select * from DxFvcExpiry_t where type=%d LIMIT 1"
								,eDxFvcExpiry_Type_Service);

	HxLOG_Debug("==============================================================================\n");
	HxLOG_Debug("query:\n\t%s\n", query);
	HxLOG_Debug("==============================================================================\n");

	id = DAPI_Query(query, (DAPI_Getter)ipepg_OnDamaFvcsReceived, (void*)&pstExpiryHash);
	HxLOG_Debug("DAPI_Query() id: %u \n", id);

	dErr = DAPI_Wait(10000, id);
	if (id == 0 || dErr != DERR_OK)
	{
		HxLOG_Error("Query FAIL \n");
		return ERR_FAIL;
	}

	HLIB_HASH_IterInit(&iter, pstExpiryHash);
	while (HLIB_HASH_IterNext(&iter, NULL, (void **)&pstFvcExpiry))
	{
		if (pstFvcExpiry != NULL)
		{
			hResult = ERR_OK;
			HxSTD_memcpy(pstFvcServiceExpiry,pstFvcExpiry, sizeof(DxFvcExpiry_t));
			break;
		}
	}
	HLIB_HASH_Destroy(pstExpiryHash);

	return hResult;
}

HERROR	IPEPG_SetNextWakeup (void)
{
	UNIXTIME	utNextExpiryTime = 0;
	UNIXTIME	utCurrTime = 0;
	HERROR		hError = ERR_FAIL;

	hError = ipepg_GetNextExpiryTime(&utNextExpiryTime);
	if (hError != ERR_OK)
	{
		HxLOG_Debug(HxANSI_COLOR_RED("there is no expiry. \n"));
		return ERR_FAIL;
	}

	utCurrTime = (UNIXTIME)HLIB_STD_GetSystemTime();
	if (utNextExpiryTime > utCurrTime)
	{
		HxLOG_Debug(HxANSI_COLOR_BLUE(" Wake up in future. set next wakeup (%d) \n"),utNextExpiryTime-utCurrTime + 1);
		IPEPG_SetFvcTimer(utNextExpiryTime-utCurrTime + 2);
	}
	else
	{
		HxLOG_Debug(HxANSI_COLOR_BLUE(" Passed Wakeup. set next wakeup now \n"));
		IPEPG_SetFvcTimer(2);
	}

	return ERR_OK;
}


void	IPEPG_RefreshExpiryQueue (void *arg)
{
	HxHASH_t		*pstExpiryHash = NULL;

	UNIXTIME		utCurrTime;
	UNIXTIME		utCurrMidnightTime;
	UNIXTIME		utBackward7DayMidnightTime;
	UNIXTIME		utForward7DayMidnightTime;
	UNIXTIME		utStarttime;


	DxFvcExpiry_t	stFvcExpiry;
	void			*pvDetached = NULL;

	HINT32			onId;
	HxVector_t		*pstNetworkVector = NULL;
	HINT32			nListCount = 0;
	DxNetwork_t		*pstNetwork = NULL;

	// 0. get onid
	pstNetworkVector = APK_META_SVC_GetNetworkList();
	IPEPG_GOTO_IF(pstNetworkVector == NULL, END_FUNC);

	nListCount = HLIB_VECTOR_Length(pstNetworkVector);
	IPEPG_GOTO_IF(nListCount < 1, END_FUNC);

	pstNetwork = (DxNetwork_t *)HLIB_VECTOR_ItemAt(pstNetworkVector, 0);
	IPEPG_GOTO_IF(pstNetwork == NULL, END_FUNC);
	onId = pstNetwork->onid;

	// 1. get all fvc expiry - using list? hash? -> hash
	pstExpiryHash = HLIB_HASH_NewFull(HLIB_HASH_GetIntKey, HLIB_HASH_IsIntEqual, NULL, HLIB_STD_MemFree_CB);
	ipepg_GetAllFvcData(&pstExpiryHash);
	HxLOG_Debug(HxANSI_COLOR_GREEN("Retrieved Expiry Size=(%d) \n"),HLIB_HASH_Size(pstExpiryHash));

	utCurrTime = (UNIXTIME)HLIB_STD_GetSystemTime();
	utCurrMidnightTime = utCurrTime - utCurrTime%(24*60*60);

	utBackward7DayMidnightTime = utCurrMidnightTime -(IPEPG_TOTAL_TIME)*60*60;
	utForward7DayMidnightTime  = utCurrMidnightTime +(1*24 + IPEPG_BACKWARD_TOTAL_TIME)*60*60;	// 8: curr(1) + next(7)

	HxSTD_memset((void*)&stFvcExpiry,0,sizeof(DxFvcExpiry_t));

	// 2. find missing fvc expiry
	DAPI_BatchBegin(DxNAMEOF(DxFvcExpiry_t));

	for (utStarttime = utBackward7DayMidnightTime; utStarttime<utForward7DayMidnightTime ;utStarttime += IPEPG_PART_OF_TIME*60*60)
	{
		if ((pvDetached = HLIB_HASH_Detach(pstExpiryHash, (void *)ipepg_GetUidHashKey(onId,utStarttime))) != NULL)
		{
			IPEPG_MemFree(pvDetached);
			continue;
		}

		stFvcExpiry.uid = DxFVC_UID(onId, utStarttime);
		stFvcExpiry.expiryTime = 1;		// 0은 sql에서 null로 취급되어서, 후에 earliest expiry db를 얻어올때 우선순위를 갖지 못 한다.
		stFvcExpiry.startTime = utStarttime;
		stFvcExpiry.onid = onId;

		// 3. set missing fvc expiry
		DAPI_Set(DxNAMEOF(DxFvcExpiry_t), stFvcExpiry.uid, &stFvcExpiry, sizeof(DxFvcExpiry_t));
	}

	DAPI_BatchEnd(DxNAMEOF(DxFvcExpiry_t));
	DAPI_Sync(DxNAMEOF(DxFvcExpiry_t));

	ipepg_DeleteOldExpirys(pstExpiryHash);
	DAPI_Sync(DxNAMEOF(DxFvcExpiry_t));

END_FUNC:
	if (pstNetworkVector) {HLIB_VECTOR_Delete(pstNetworkVector);}
	if (pstExpiryHash != NULL) {HLIB_HASH_Destroy(pstExpiryHash);}
}

HERROR	IPEPG_CancelFvcTimer (void)
{
	IPEPG_FUNC_START();

	ipepg_CancelFvcTimer();

	IPEPG_FUNC_END();

	return ERR_OK;
}

HERROR	IPEPG_CancelFvcRetryTimer (void)
{
	IPEPG_FUNC_START();

	ipepg_CancelFvcRetryTimer();

	IPEPG_FUNC_END();

	return ERR_OK;
}

HERROR	IPEPG_CancelFvcSeriesEpgTimer (void)
{
	IPEPG_FUNC_START();

	ipepg_CancelFvcSeriesEpgTimer();

	IPEPG_FUNC_END();

	return ERR_OK;
}



HERROR	IPEPG_SetFvcTimer (HUINT32 time)
{
	HERROR hErr = ERR_OK;
	IPEPG_FUNC_START();

	hErr = ipepg_SetFvcTimer(time);

 	IPEPG_FUNC_END();
	return hErr;
}



HERROR	IPEPG_SetFvcRetryTimer (HUINT32 time)
{
	HERROR hErr = ERR_OK;
	IPEPG_FUNC_START();

	hErr = ipepg_SetFvcRetryTimer(time);

 	IPEPG_FUNC_END();
	return hErr;
}

HERROR	IPEPG_SetFvcSeriesEpgTimer (HUINT32 time)
{
	HERROR hErr = ERR_OK;
	IPEPG_FUNC_START();

	hErr = ipepg_SetFvcSeriesEpgTimer(time);

 	IPEPG_FUNC_END();
	return hErr;
}


HERROR	IPEPG_RequestAuth (void)
{
	HERROR		hError 			  = ERR_FAIL;

	HCHAR		*uriPrefix		  = NULL;
	HxVector_t	*pstNetworkVector = NULL;
	DxNetwork_t	*pstNetwork		  = NULL;

	IPEPG_FUNC_START();

	// priority 1. USB
	uriPrefix = (HUINT8 *)UAPI_FM_GetDefaultURL(NULL, (HCHAR *)IPEPG_FVC_URL_FILE);
	if (uriPrefix == NULL)
	{
		// priority 2. Network uri -> head -> baseURL
		pstNetworkVector = APK_META_SVC_GetNetworkList();
		IPEPG_GOTO_IF(pstNetworkVector == NULL, END_GETFVCBASEURI);

		pstNetwork = (DxNetwork_t *)HLIB_VECTOR_ItemAt(pstNetworkVector, 0);
		IPEPG_GOTO_IF(pstNetwork == NULL, END_GETFVCBASEURI);

		uriPrefix = pstNetwork->ex.tva.UriPrefix;
	}

	IPEPG_GOTO_IF(uriPrefix == NULL, END_GETFVCBASEURI);

	hError = IPEPG_HeadFvcPrefixUrl(uriPrefix);
	IPEPG_GOTO_IF(hError != ERR_OK, END_GETFVCBASEURI);

	// TODO: baseURL, authHash, authTimestamp는 HAPI로 날려야 한다.
	// TODO: IPEPG 도 동일하게 HAPI로 받아서 하되, 이 함수 내에서는 sync로 처리 할 수 있는 구조를 생각하자.

//	HAPI_BroadcastSignal("signal:onFvcAuthHash", 0, "i", 1);
//	HAPI_BroadcastSignal("signal:onFvcAuthHash", 0, "i", 1);


END_GETFVCBASEURI:
	if (pstNetworkVector) {HLIB_VECTOR_Delete(pstNetworkVector);}

	IPEPG_FUNC_END();

	return hError;

}

void	IPEPG_DeleteOldIpEpg (void)
{
	UNIXTIME	utEndTime = 0;
	UNIXTIME	utCurrTime = 0;

	utCurrTime = (UNIXTIME)HLIB_STD_GetSystemTime();
	if (utCurrTime == 0 )
	{
		return ;
	}

	if (utCurrTime < -1*(IPEPG_OFFSET_TIME) )
	{
		HxLOG_Error("curr time small then offset. curr=(%d) \n",utCurrTime);
		return;
	}
	utEndTime = utCurrTime + IPEPG_OFFSET_TIME;

	if (utEndTime < IPEPG_BACKWARD_TOTAL_TIME*60*60)
	{
		HxLOG_Error("end time small then backward time. end=(%d) \n",utEndTime);
		return;
	}
	utEndTime -= IPEPG_BACKWARD_TOTAL_TIME*60*60;

	APK_META_QUERY_DeletIpEventByEndTime(utEndTime);
}

/*********************** End of File ******************************/


