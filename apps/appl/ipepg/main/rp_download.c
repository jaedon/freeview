/**************************************************************
 *	@file		rp_download.c
 *	RP Rsvlist downloader for OCTO 2.0
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/08/01
 *	@author			humax
 *	@brief			RP Rsvlist  Downloader for OCTO 2.0 Project
 **************************************************************/
#include <unistd.h>
#include <signal.h>

#include <vkernel.h>
#include <apk.h>
#include "../include/ipepg.h"
#include "ipepg_manager.h"
#include "ipepg_auth.h"
#include "rp_common.h"

#if defined(CFG_RELEASE_DEBUG)
#undef HxLOG_Trace
#define HxLOG_Trace()	printf("[IPEPG] %s %d\n", __FUNCTION__, __LINE__);
#undef HxLOG_Debug
#define HxLOG_Debug		printf
#undef HxLOG_Print
#define HxLOG_Print		printf
#endif

#define	IPEPG_EVENT_GETNAME(vEventString)	((HCHAR*)(vEventString == NULL? NULL:((DxEventString_t *)HLIB_VECTOR_First(vEventString))->string))

static HERROR	local_cnxt_IpEpgHttpGetWithCallback(const HINT8 * szUrl, HxHttpGet_Callback *callback, HUINT32 ulTimeout)
{
	HERROR							hError = ERR_FAIL;
	HxTRANSPORT_Http_t			*pHttp = NULL;
	HxTRANSPORT_Httpheader_t		*pHttpHeader = NULL;
	HxTRANSPORT_HttpResponse_e	resCode;
	HFLOAT64						ulRealSize = 0;

    HxLOG_Trace();
	IPEPG_GOTO_IF(szUrl == NULL, EXIT_CNXTHUMAX_FUNC);

	pHttp = HLIB_TRANSPORT_HttpOpen();
	IPEPG_GOTO_IF(pHttp == NULL, EXIT_CNXTHUMAX_FUNC);
#if defined(TVPORTAL_ROOT_CA) && defined(TVPORTAL_CLIENT_CA_KEY)
	if(!strncmp(szUrl,HTTPS_PREFIX,HTTPS_PREFIX_URL_LENGTH))
	{
		hError = HLIB_TRANSPORT_HttpSetCACertificateFile(pHttp, TVPORTAL_ROOT_CA);
		IPEPG_GOTO_IF(hError != ERR_OK, EXIT_CNXTHUMAX_FUNC);

		hError = HLIB_TRANSPORT_HttpSetCertificateKeyFile(pHttp, TVPORTAL_CLIENT_CA_KEY, eHxTP_HTTP_CERTIFICATE_P12);
		IPEPG_GOTO_IF(hError != ERR_OK, EXIT_CNXTHUMAX_FUNC);

		hError = HLIB_TRANSPORT_HttpSetPeerVeryfication(pHttp, TRUE);
		IPEPG_GOTO_IF(hError != ERR_OK, EXIT_CNXTHUMAX_FUNC);

		hError = HLIB_TRANSPORT_HttpSetHostVeryfication(pHttp, FALSE);
		IPEPG_GOTO_IF(hError != ERR_OK, EXIT_CNXTHUMAX_FUNC);
	}
#endif
	pHttpHeader = HLIB_TRANSPORT_HttpAppendHeader(pHttpHeader, "Content-Type: text/xml");
	IPEPG_GOTO_IF(pHttpHeader == NULL, EXIT_CNXTHUMAX_FUNC);
	IPEPG_GOTO_IF(HLIB_TRANSPORT_HttpSetHeaderToTP(pHttp, pHttpHeader) != ERR_OK, EXIT_CNXTHUMAX_FUNC);

    if(ulTimeout != 0)
    {
        IPEPG_GOTO_IF(HLIB_TRANSPORT_HttpSetTimeout(pHttp, ulTimeout) != ERR_OK, EXIT_CNXTHUMAX_FUNC);
    }

	IPEPG_GOTO_IF(HLIB_TRANSPORT_HttpGetWithCallback(pHttp, szUrl, callback, &ulRealSize) != ERR_OK, EXIT_CNXTHUMAX_FUNC);

	hError = ERR_OK;
EXIT_CNXTHUMAX_FUNC :

	if ( hError != ERR_OK )
	{
		HLIB_TRANSPORT_HttpGetHttpResponseCode(pHttp, &resCode);
	}

	if(pHttpHeader != NULL) HLIB_TRANSPORT_HttpFreeHeader(pHttpHeader);
	if(pHttp != NULL) HLIB_TRANSPORT_HttpClose(pHttp);

	return hError;
}

static HUINT8		*s_rp_buffer = NULL;
static HINT32		s_rpdata_size = 0;
static RPList_t		stRpList;


RPList_t *	Rp_GetRpLists(void)
{
	return &stRpList;
}

static HINT32   rp_DownloadCallback(void *ptr, HINT32 size, HINT32 nmemb, void *stream)
{
    HINT32 uiDataSize = 0;
	if(s_rp_buffer == NULL)
		return 0;

	uiDataSize = size * nmemb;
	if(s_rpdata_size + uiDataSize > IPEPG_RP_LIST_MAX_SIZE)
		return  0;

	HxSTD_MemCopy(s_rp_buffer+s_rpdata_size, ptr, uiDataSize);
	s_rpdata_size += uiDataSize;

    return uiDataSize;
}

static HERROR	rp_DownloadRsvList(HxList_t **ppRpListAttr, HxList_t **ppRpList)
{
	HERROR		hError = ERR_FAIL;
	HUINT8		pServiceListUrl[IPEPG_URL_MAX_LEN] = {0,};
	HUINT32		ulReceivedSize = 0;
	HUINT8		*pucMaccAddr = NULL;
	HUINT8		*pucServerURL = NULL;

	pucMaccAddr = IPEPG_GetItemPoint(eIPEPG_ITEM_MAC);
	pucServerURL = IPEPG_GetItemPoint(eIPEPG_ITEM_SERVERURL);


	HxSTD_MemSet(pServiceListUrl, 0x00, IPEPG_URL_MAX_LEN);
	HxSTD_snprintf(pServiceListUrl, IPEPG_URL_MAX_LEN, "%s/%s%s", pucServerURL, HUMAX_GETRPLIST_URL, pucMaccAddr);

	HxLOG_Debug("[rp_DownloadRsvList]URL : %s \n", pServiceListUrl);

	if(s_rp_buffer == NULL)
	{
		s_rp_buffer = (HUINT8 *)IPEPG_MemAlloc(IPEPG_RP_LIST_MAX_SIZE);
	}

	HxSTD_MemSet(s_rp_buffer, 0x00, IPEPG_RP_LIST_MAX_SIZE);
	s_rpdata_size = 0;
	hError = local_cnxt_IpEpgHttpGetWithCallback(pServiceListUrl, rp_DownloadCallback, IPEPG_HTTP_TIMEOUT);
	if(hError != ERR_OK)
	{
		IPEPG_MemFree(s_rp_buffer);
		s_rp_buffer = NULL;
		s_rpdata_size = 0;
		IPEPG_MemFree(pucMaccAddr);
		IPEPG_MemFree(pucServerURL);
		return ERR_FAIL;
	}

	ulReceivedSize = (HUINT32)s_rpdata_size;

	HxLOG_Debug("RP LIST DATA: %s [%d]\n", s_rp_buffer, ulReceivedSize);

	/* RpList XML Parsing */
	Rp_ParseRsvList(s_rp_buffer, ulReceivedSize, ppRpListAttr, ppRpList);
	s_rpdata_size = 0;
	IPEPG_MemFree(pucMaccAddr);
	IPEPG_MemFree(pucServerURL);
	IPEPG_MemFree(s_rp_buffer);
	s_rp_buffer = NULL;
	return ERR_OK;
}

// workaround.
// 정석으로 하려면, 내부에서 polling 하는 것 보다는, listener에서 요청한 schedule이 처리되는 것을 확인 하는 것이 좋지만 간단하지 않고,
// 코드 가독성도 떨어진다.
// 따라서 다음과 같이 polling으로 처리해 놓았음, 전용 Task에서 수행하는 것이므로, 큰 문제는 없을 듯.
static HBOOL	rp_AddRsv(DxSchedule_t* pstDxSched)
{
	HERROR						hError = ERR_FAIL;
	HUINT32						ulSlot = 0;
	HUINT32						retryCount = 60;	// retryCount * 100ms

	if(pstDxSched)
	{
		DxSchedule_t Schedule;

		if (APK_META_SCHEDULE_AddSchedule(pstDxSched, &ulSlot) != ERR_OK)
		{
			HxLOG_Trace();
			HxLOG_Warning("Failed!!!\n");
			return ERR_FAIL;
		}

		do
		{
			HxSTD_MemSet((void*)&Schedule, 0, sizeof(DxSchedule_t));
			hError = APK_META_SCHEDULE_GetSchedule(ulSlot,&Schedule);
			if(hError == ERR_FAIL)
			{
				HxLOG_Debug("GetSchedule  retry[%d] slot[%d]\n", retryCount, ulSlot);
				HLIB_STD_TaskSleep(100);
				retryCount--;
			}
		}
		while(hError != ERR_OK && retryCount > 0);
	}
	return hError;
}


static HUINT32		ipepg_AddRPSchedule (HINT32	eventId, HUINT32 startTime, HUINT32 duration, HUINT32 svcUId, HUINT32 action, HUINT32 ucRepeat, HUINT32 seriesID, HUINT32 uniqueId, HBOOL bSeries)
{
	DxSchedule_t			 stDxSched;
	DxEvent_t 					*event = NULL;
	HINT32						lTsId=0, lOnId=0, lSvcId=0;

	HxLOG_Trace();

	HxSTD_MemSet(&stDxSched, 0, sizeof(DxSchedule_t));

	switch (action)
	{
	case eDxSCHED_ACTION_WATCH:
		stDxSched.eRsvType = DxRSVTYPE_WATCHING_EBR;
		stDxSched.uExtInfo.stWch.nEvtId = eventId;
		stDxSched.uExtInfo.stWch.ulSeriesId = seriesID;
		break;

	case eDxSCHED_ACTION_RECORD:
		stDxSched.uExtInfo.stRec.nEvtId = eventId;
		stDxSched.uExtInfo.stRec.ulSeriesId = seriesID;
#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
		if (bSeries)
#else
		if(seriesID > 0)
#endif
		{
			stDxSched.eRsvType = DxRSVTYPE_RECORD_SERIES_EBR;
		}
		else
		{
			stDxSched.eRsvType = DxRSVTYPE_RECORD_EBR;
		}

		if (APK_SCENARIO_GetStartPadding(&stDxSched.uExtInfo.stRec.ulPreOffsetTime) != ERR_OK)
		{
		   HxLOG_Error("Fail to GetStartPaddingTime\n");
		   stDxSched.uExtInfo.stRec.ulPreOffsetTime = 0;
		}
		if (APK_SCENARIO_GetEndPadding(&stDxSched.uExtInfo.stRec.ulPostOffsetTime) != ERR_OK)
		{
		   HxLOG_Error("Fail to GetEndPaddingTime\n");
		   stDxSched.uExtInfo.stRec.ulPostOffsetTime = 0;
		}

		break;

	default:
		HxLOG_Error("[%s:%d] Unknown action(%d)\n", __FUNCTION__, __LINE__, action);
		break;
	}

	HLIB_DATETIME_ConvertUnixTimeToDateTime(startTime, &stDxSched.stStartTime);
	stDxSched.ulDuration	= duration;
	stDxSched.ulSvcUid = svcUId;
	stDxSched.eEpgType = eDxEPG_TYPE_IP;

	if(APK_META_SVC_GetServiceTriplet(svcUId, &lTsId, &lOnId, &lSvcId) == TRUE)
	{
		stDxSched.stTripleId.usTsId = lTsId;
		stDxSched.stTripleId.usOnId = lOnId;
		stDxSched.stTripleId.usSvcId = lSvcId;
	}

#ifndef CONFIG_FUNC_EPG_USE_SVCUID
	event = APK_META_QUERY_GetEvent (eDxEPG_TYPE_IP, lOnId, lTsId, lSvcId, uniqueId, NULL, NULL);
#else
	{
		HUID eventUid = DxEVENT_UID(svcUId, eventId);
		event = APK_META_QUERY_GetEvent (eDxEPG_TYPE_IP, eventUid, NULL, NULL);
	}
#endif

	if(event != NULL)
	{
		HLIB_STD_StrNCpySafe(stDxSched.uExtInfo.stRec.szEventName, IPEPG_EVENT_GETNAME(event->name), DxRSV_EVENT_NAME_LEN);
	}

	HxLOG_Trace();

	// AddSchedule 이후에, Cache에 반영될 때까지 기다린다.
	if ( rp_AddRsv(&stDxSched) != ERR_OK)
	{
		HxLOG_Trace();
		HxLOG_Warning("AddRsv failed !!!\n");
		return ERR_FAIL;
	}

	HxLOG_Trace();
	HxLOG_Debug("[%s:%d] Success!!!\n", __FUNCTION__, __LINE__);

	return ERR_OK;
}

void	rp_DownloadData(void)
{
	HxList_t			*pRpListAttr = NULL, *pRpDataList = NULL;
	HxList_t		*pTempRpDataList = NULL;
	RP_Data_t 		*pst_RpData = NULL;
    RPListAttr_t	*pstAttr = NULL;
	HUINT32			uiCount;
	HERROR				ret = ERR_OK;
	HINT32			result, i;

	HxLOG_Debug("[%s] tid(%d) +++ \n", __FUNCTION__, HLIB_STD_GetTID());

	// INIT.
	HxSTD_MemSet((void*)&stRpList, 0, sizeof(RPList_t));

	// Get Rsv list from RP Server
	ret = rp_DownloadRsvList(&pRpListAttr, &pRpDataList);
	if(ret != ERR_OK)
	{
		HxLOG_Error("[%s] reservation list download fail.\n", __FUNCTION__);
		return;
	}

	/* RPLIST Attr */
	if(pRpListAttr)
	{
		pstAttr = (RPListAttr_t *)HLIB_LIST_Data(pRpListAttr);
		if(pstAttr)
		{
			VK_memcpy(&stRpList.stAttr, pstAttr, sizeof(RPListAttr_t));

			/* RPDATA의 개수 만큼 메모리 할당 */
			stRpList.pstRpData = (RP_Data_t *)IPEPG_MemAlloc( sizeof(RP_Data_t) * stRpList.stAttr.ucCount );
			if(stRpList.pstRpData != NULL)
			{
				HxSTD_MemSet(stRpList.pstRpData, 0, sizeof(RP_Data_t) * stRpList.stAttr.ucCount );
				HxLOG_Debug("[rp_DownloadData] %d %d\n", pstAttr->ucCount, stRpList.stAttr.ucCount );
			}
		}
		HLIB_LIST_Foreach(pRpListAttr , IPEPG_MemFree);

		pRpListAttr = HLIB_LIST_RemoveAll(pRpListAttr);
		if(pRpListAttr != NULL)
		{
			HxLOG_Debug("[rp_DownloadRsvList] Pointer Free FAIL \n");
			pRpListAttr = NULL;
		}
	}

	/* RPDATA */
	uiCount = 0;
	pTempRpDataList = pRpDataList;
	while(pTempRpDataList)
	{
		pst_RpData = (RP_Data_t *)HLIB_LIST_Data(pTempRpDataList);
		if(pst_RpData)
		{
			if(stRpList.pstRpData != NULL)
			{
				VK_memcpy(&stRpList.pstRpData[uiCount], pst_RpData, sizeof(RP_Data_t));
				HxLOG_Debug("[rp_DownloadData] hSvc = %d, UniqueId=%d\n",
				stRpList.pstRpData[uiCount].hSvc, stRpList.pstRpData[uiCount].usUniqueId);
			}
			/*---------------------------------------
					Reservation or Cancel
			---------------------------------------*/
			uiCount += 1;
		}
		/* Get Next RPData */
		pTempRpDataList = pTempRpDataList->next;
	}

	if(pRpDataList)
	{
		HLIB_LIST_Foreach(pRpDataList , IPEPG_MemFree);
		pRpDataList= HLIB_LIST_RemoveAll(pRpDataList);
		if(pRpDataList != NULL)
		{
			HxLOG_Debug("[rp_DownloadRsvList] Pointer Free fail \n");
			pRpDataList = NULL;
		}
	}

	for(i = 0; i < stRpList.stAttr.ucCount; i++)
	{
		HUINT32	action = 0;
		HUINT32	repeat = 0;
		HUINT32	seriesId = 0;
		HINT32	eventId = (stRpList.pstRpData[i].usUniqueId);
		HBOOL	bSeries = FALSE;

#if defined(CONFIG_DEBUG)
		{
			HxDATETIME_t	stStartTime;
			HLIB_DATETIME_ConvertUnixTimeToDateTime(stRpList.pstRpData[i].utStartTime, &stStartTime);
			HxLOG_Debug("==================>>>>>> RP RECORD Time [%04d-%02d-%02d %02d:%02d:%02d] stRpList.pstRpData[%d] utStartTime[%x] repeat[%d]\n",
			stStartTime.stDate.usYear, stStartTime.stDate.ucMonth, stStartTime.stDate.ucDay , stStartTime.stTime.ucHour, stStartTime.stTime.ucMinute, stStartTime.stTime.ucSecond, i, stRpList.pstRpData[i].utStartTime, stRpList.pstRpData[i].ucRepeat);
		}
#endif

		if(stRpList.pstRpData[i].usRsvType == RP_RSVTYPE_WATCHING)
		{
			action = eDxSCHED_ACTION_WATCH;
		}
		else if(stRpList.pstRpData[i].usRsvType == RP_RSVTYPE_REC_EBR)
		{
			action = eDxSCHED_ACTION_RECORD;
		}
		else if(stRpList.pstRpData[i].usRsvType == RP_RSVTYPE_REC_TBR)
		{
			action = eDxSCHED_ACTION_RECORD;
		}

		if(stRpList.pstRpData[i].ucRepeat == RP_REPEAT_ONCE)
		{
			repeat = eDxSCHED_CYCLE_NONE;
		}
		else if(stRpList.pstRpData[i].ucRepeat == RP_REPEAT_DAILY)
		{
			repeat = eDxSCHED_CYCLE_DAYLY;
		}
		else if(stRpList.pstRpData[i].ucRepeat == RP_REPEAT_WEEKLY)
		{
			repeat = eDxSCHED_CYCLE_WEEKLY;
		}
		else if(stRpList.pstRpData[i].ucRepeat == RP_REPEAT_SERIES)
		{
			DxEvent_t *event = NULL;
#ifndef CONFIG_FUNC_EPG_USE_SVCUID
			HINT32 lTsId=0, lOnId=0, lSvcId=0;
			if(APK_META_SVC_GetServiceTriplet(stRpList.pstRpData[i].hSvc, &lTsId, &lOnId, &lSvcId) == TRUE)
				event = APK_META_QUERY_GetEvent (eDxEPG_TYPE_IP, lOnId, lTsId, lSvcId, stRpList.pstRpData[i].usUniqueId, NULL, NULL);
#else
			HUID eventUid = DxEVENT_UID(stRpList.pstRpData[i].hSvc, stRpList.pstRpData[i].usUniqueId);
			event = APK_META_QUERY_GetEvent (eDxEPG_TYPE_IP, eventUid, NULL, NULL);
#endif
			repeat = eDxSCHED_CYCLE_NONE;
			bSeries = TRUE;
			if(event) seriesId = event->extension.ipEpg.seriesId;
		}

		result = ipepg_AddRPSchedule(eventId, stRpList.pstRpData[i].utStartTime, stRpList.pstRpData[i].utEndTime - stRpList.pstRpData[i].utStartTime, stRpList.pstRpData[i].hSvc, action, repeat, seriesId, stRpList.pstRpData[i].usUniqueId,bSeries);
		if (result != ERR_OK)
		{
			HxLOG_Warning("==================>>>>>>     [%s:%d] Failed to operation \"APK_META_SCHEDULE_AddSchedule()\"\n", __HxFILE__, __HxLINE__);
			stRpList.pstRpData[i].usRsvResult = RPLIST_RESULT_FAIL_CONFLICT;
		}
		else
		{
			HxLOG_Warning("==================>>>>>>     [%s:%d] Success to operation \"APK_META_SCHEDULE_AddSchedule()\"\n", __HxFILE__, __HxLINE__);
			stRpList.pstRpData[i].usRsvResult = RPLIST_RESULT_OK;
		}
	}
}

void rp_Process(void)
{
	HUINT32 			MsgQId = 0;
	HERROR				ret = ERR_OK;
	HBOOL				bUpdated = FALSE;
	HxLOG_Trace();

	ret = IPEPG_CheckPrivacyPolicyVer(&bUpdated);
	/* bUpdated is TRUE means that can't access Humax Portal Server, because of GDPR */
	if((ret == ERR_OK) && (bUpdated == FALSE))
	{
		rp_DownloadData();
		Rp_UploadData();
	}

	ret = IPEPG_GetMsgQId(&MsgQId);
	if(ERR_FAIL != ret)
	{
		IpEpgMsg_t	stMsg = {0,};
		stMsg.ulMsg    = eIPEPG_MSG_DONE_STB_UPLOAD;
		VK_MSG_Send(MsgQId, (void*)&stMsg, sizeof(IpEpgMsg_t));
	}
	else
	{
		HxLOG_Debug("Can't send message. GetMsgQId's return value(%d) \n", ret);
	}
}


void Rp_Task(void * pstDownMgr)
{
	HUINT32 			ulRpTaskQId;
	IpEpgMsg_t			stMsg;
	IPEPG_TASK_MGR_t * pstTaskMgr = NULL;

	HxLOG_Trace();

	pstTaskMgr = (IPEPG_TASK_MGR_t *)pstDownMgr;
	if(pstTaskMgr == NULL || pstTaskMgr->ulRsvMsgQid == 0)
	{
		return;
	}
	ulRpTaskQId = pstTaskMgr->ulRsvMsgQid;

	while(1)
	{
		if (VK_MSG_ReceiveTimeout(ulRpTaskQId, (void*)&stMsg, sizeof(IpEpgMsg_t), 300) == VK_OK)
		{
			switch (stMsg.ulMsg)
			{
				case eIPEPG_MSG_START_RSV_DOWNLOAD :
					rp_Process();
					break;
				default :
					HxLOG_Error("Rp_Task reveived OtherMessage[%d]\n", stMsg.ulMsg);
					break;
			}
		}
	}
	return;
}


/*********************** End of File ******************************/


