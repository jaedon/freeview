/**************************************************************
 *	@file		ipepg_downloader.c
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
#include <hapi.h>
#include <dlib.h>
#include <vkernel.h>
#include <apk.h>

#include <sys/types.h>

#include "../include/ipepg.h"
#include "ipepg_manager.h"
#include "ipepg_downloader.h"
#include "ipepg_svclistupload.h"
#include "rp_common.h"
#if defined(CONFIG_OP_UK_DTT)
#include "ipepg_fvc.h"
#endif


#define	IPEPG_MSGQ_COUNT				(128)

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

#define IPEPG_IsFlagSetted(e, f)    ((e&f)==f)
#define IPEPG_IsFlagUnsetted(e, f)  (!(IPEPG_IsFlagSetted(e,f)))
#define	IPEPG_SetFlag(e, f)			(e = e|f)
#define	IPEPG_UnsetFlag(e, f)		(e = e&(~f))
#if defined(CONFIG_DEBUG) || defined(CFG_OPEN_DEBUG)
static char *ipepg_GetMsgString(IPEPG_MSG_e msg)
{
	switch (msg)
	{
		ENUM_TO_STR(eIPEPG_MSG_STOP);
		ENUM_TO_STR(eIPEPG_MSG_START_DOWNLOAD);
		ENUM_TO_STR(eIPEPG_MSG_DONE_DOWNLOAD);
		ENUM_TO_STR(eIPEPG_MSG_START_UPDATE_SERVICELIST);
		ENUM_TO_STR(eIPEPG_MSG_DONE_UPDATE_SERVICELIST);
		ENUM_TO_STR(eIPEPG_MSG_START_ALL);
		ENUM_TO_STR(eIPEPG_MSG_START_ONLY_SVCLIST);
		ENUM_TO_STR(eIPEPG_MSG_REQUEST_SHUTDOWN_NOTI);
		ENUM_TO_STR(eIPEPG_MSG_START_RSV_DOWNLOAD);
		ENUM_TO_STR(eIPEPG_MSG_START_DOWNLOAD_FVC_SERIES_SCHEDULE);
		ENUM_TO_STR(eIPEPG_MSG_DONE_STB_UPLOAD);
		ENUM_TO_STR(eIPEPG_MSG_DONE_DOWNLOAD_FVC_SCHEDULE);
		ENUM_TO_STR(eIPEPG_MSG_PF_MERGE);
		ENUM_TO_STR(eIPEPG_MSG_DONE_WAITING_RETRY);
		ENUM_TO_STR(eIPEPG_MSG_START_DOWNLOAD_FVC_SCHEDULE);
		ENUM_TO_STR(eIPEPG_MSG_DONE_DOWNLOAD_FVC_SERIES_SCHEDULE);

		default: return "UNKNOWN_IPEPG_MSG_e";
	}
}
#else
static char *ipepg_GetMsgString(IPEPG_MSG_e msg)
{
	return NULL;
}
#endif


#if defined (CONFIG_DEBUG)
IPEPG_TASK_MGR_t * 			sTaskMgr = NULL;
#else
static IPEPG_TASK_MGR_t * 			sTaskMgr = NULL;
#endif
static	HUINT32 gTimerTime = 0;
static	HBOOL	gbSetTimer = FALSE;

HUINT32 gTimerFvcTime = 0;
HBOOL	gbSetFvcTimer = FALSE;

HUINT32 gTimerFvcRetryTime = 0;
HBOOL	gbSetFvcRetryTimer = FALSE;

HUINT32 gTimerFvcSeriesEpgTime = 0;
HBOOL	gbSetFvcSeriesEpgTimer = FALSE;



static HERROR ipepg_SetTimer(HUINT32 time);
static HERROR ipepg_DestroyTask(HUINT32 ulTaskId, HBOOL * pbNeedSleep);

extern HCHAR*	UAPI_FM_GetDefaultURL(HCHAR *pszDefaultURL, HCHAR *pszURLFileName);
IPEPG_FVC_STATE_e ipepg_RunState (IpEpgMsg_t *pstMsg, IPEPG_TASK_MGR_t *pstTaskMgr);

static	HERROR	ipepg_CheckForStartingIPEPG(void)
{
	HBOOL	bIpEpgEnabled = FALSE;

	IPEPG_FUNC_START();

	bIpEpgEnabled = (HBOOL)IPEPG_GetItemValue(eIPEPG_ITEM_FLAG_IPEPG_ONOFF);
	if (bIpEpgEnabled == TRUE)
	{
		IPEPG_SendCommand(eIPEPG_MSG_START_ALL);
	}

	IPEPG_FUNC_END();
	return ERR_OK;
}

static HERROR ipepg_GetFvcUri (void)
{
	HERROR	hErr = ERR_OK;
	HUINT8	*pFvcServerBaseUrl = NULL;

	pFvcServerBaseUrl = IPEPG_GetItemPoint(eIPEPG_ITEM_FVC_BASE_URL);
	if (pFvcServerBaseUrl != NULL)
	{
		HxLOG_Debug("Use cached uri. [%s] \n",pFvcServerBaseUrl);
		IPEPG_GOTO_IF_NO_PRINT(1, EXIT_FUNC);
	}

	hErr = IPEPG_RequestAuth();
	if (hErr != ERR_OK)
	{
		HxLOG_Error("Fail to get ipepg server url \n");
		hErr = ERR_FAIL;
	}

	// do nothing (SetItem is done in RequestAuth)

EXIT_FUNC:
	if (pFvcServerBaseUrl) {IPEPG_MemFree(pFvcServerBaseUrl);}

	return hErr;
}

static HERROR ipepg_GetServerUriFromFile(void)
{
	HERROR 	ret = ERR_FAIL;
	HUINT8	*tempURL = NULL;

	IPEPG_FUNC_START();

	tempURL = (HUINT8 *)UAPI_FM_GetDefaultURL((HCHAR *)IPEPG_SERVER_URL, (HCHAR *)IPEPG_URL_FILE);
	if(tempURL)
	{
		IPEPG_SetItem(eIPEPG_ITEM_SERVERURL,(void *)tempURL);
		HxLOG_Debug("SERVER URL[%s]\n", tempURL);
		ret  = ERR_OK;
	}

	IPEPG_FUNC_END();
	return ret;
}

HERROR	rp_AbleToDownload(IPEPG_DATAMGR_t 	* pstData)
{
	UNIXTIME	ulSystemTime = 0;
	HBOOL		bRpEnabled = FALSE;
	HBOOL		bServiceUpdated = FALSE;
	HBOOL		bConnected = 0;
	HUINT8		*pMacAddress = NULL;
	HERROR		err = ERR_FAIL;

	IPEPG_FUNC_START();

	if(ERR_OK != VK_CLOCK_GetTime((unsigned long*)&ulSystemTime))
	{
		HxLOG_Debug("[%s] GetTime is invalid\n", __FUNCTION__ );
		return ERR_FAIL;
	}

	// LastUpdate Time 과 current time을 비교해 하루가 지나지 않으면 skip 한다.
	// service list 가 update 되면 lastupdate time을 초기화 하여 ipepg를 다시 받도록 한다.
	// Lastupdate time이 setting 없으면 ipepg를 다시 받고 lastupdate time을 setting 후 ipepg schedule을 다시 설정 한다.

	bConnected = (HBOOL)IPEPG_GetItemValue(eIPEPG_ITEM_NETWORK);
	bRpEnabled = (HBOOL)IPEPG_GetItemValue(eIPEPG_ITEM_FLAG_RP_ONOFF);
	bServiceUpdated = (HBOOL)IPEPG_GetItemValue(eIPEPG_ITEM_FLAG_SVC_UPDATED);
	pMacAddress = (HUINT8 *)IPEPG_GetItemPoint(eIPEPG_ITEM_MAC);
	HxLOG_Debug("bRpEnabled[%d], bServiceUpdated[%d],  pMacAddress[%s], bConnected[%d]\n", bRpEnabled, bServiceUpdated, pMacAddress, bConnected);

	if (bRpEnabled == TRUE && pMacAddress != NULL)
	{
		err = ipepg_GetServerUriFromFile();		// RET OK or FAIL
	}

	IPEPG_MemFree(pMacAddress);
	IPEPG_FUNC_END();

	return err;
}



static HERROR	ipepg_AbleToServiceListUpload(IPEPG_DATAMGR_t *pstData)
{
	UNIXTIME	ulSystemTime = 0;
	HBOOL		bConnected = FALSE;
	HBOOL		bAbleToUseHdd = FALSE;
	HBOOL		bIpEpgEnabled = FALSE;
	HBOOL		bServiceUpdated = FALSE;
	HUINT8		*pMacAddress = NULL;
	HERROR		err = ERR_FAIL;
	HBOOL		bRpEnabled = FALSE;

	IPEPG_FUNC_START();

	if(ERR_OK != VK_CLOCK_GetTime((unsigned long*)&ulSystemTime))
	{
		HxLOG_Debug("[%s] GetTime is invalid\n", __FUNCTION__ );
		return ERR_FAIL;
	}
	bRpEnabled = (HBOOL)IPEPG_GetItemValue(eIPEPG_ITEM_FLAG_RP_ONOFF);
	bConnected = (HBOOL)IPEPG_GetItemValue(eIPEPG_ITEM_NETWORK);
	bIpEpgEnabled = (HBOOL)IPEPG_GetItemValue(eIPEPG_ITEM_FLAG_IPEPG_ONOFF);
	bServiceUpdated = (HBOOL)IPEPG_GetItemValue(eIPEPG_ITEM_FLAG_SVC_UPDATED);
	bAbleToUseHdd = (HBOOL)IPEPG_GetItemValue(eIPEPG_ITEM_HDD_STATUS);
	pMacAddress = (HUINT8 *)IPEPG_GetItemPoint(eIPEPG_ITEM_MAC);


	HxLOG_Debug("bIpEpgEnabled[%d], bServiceUpdated[%d],  pMacAddress[%s], bConnected[%d]\n", bIpEpgEnabled, bServiceUpdated, pMacAddress, bConnected);
	if(bConnected == TRUE && bIpEpgEnabled == TRUE && bAbleToUseHdd == TRUE && pMacAddress != NULL
		&& bServiceUpdated == FALSE && bRpEnabled == TRUE)
	{
		err = ipepg_GetServerUriFromFile();		// RET OK or FAIL
	}

	IPEPG_MemFree(pMacAddress);
	IPEPG_FUNC_END();

	return err;
}

static HERROR	ipepg_AbleToDownload(IPEPG_DATAMGR_t 	* pstData)
{
	HERROR		hErr = ERR_FAIL;
	HBOOL		bConnected = FALSE;
	HBOOL		bAbleToUseHdd = FALSE;
	HBOOL		bIpEpgEnabled = FALSE;

	IPEPG_FUNC_START();

	bConnected = (HBOOL)IPEPG_GetItemValue(eIPEPG_ITEM_NETWORK);
	bAbleToUseHdd = (HBOOL)IPEPG_GetItemValue(eIPEPG_ITEM_HDD_STATUS);
	bIpEpgEnabled = (HBOOL)IPEPG_GetItemValue(eIPEPG_ITEM_FLAG_IPEPG_ONOFF);


	HxLOG_Debug("Net[%d] Hdd[%d] Enable[%d] \n", bConnected, bAbleToUseHdd, bIpEpgEnabled);
	if( (bConnected == FALSE
		|| bIpEpgEnabled == FALSE
		|| bAbleToUseHdd == FALSE) )
	{
		HxLOG_Debug(HxANSI_COLOR_RED("Unable to download \n"));
		return ERR_FAIL;
	}

	// FVC의 경우 Start, Update, Stamp time등에 의해 막히면 안된다. 무조건 시도하는 스펙과 retry 스펙을 맞춘다.
	hErr = ipepg_GetFvcUri();
	if (hErr != ERR_OK)
	{
		HxLOG_Debug(HxANSI_COLOR_RED("Unable to download \n"));
		return ERR_FAIL;
	}

	return ERR_OK;

}

static HERROR ipepg_ClearRsvByType(DxRsvType_e eRsvType)
{
	HUINT32				 ulCnt, ulSlotNum, ulSchedNum;
	DxSchedule_t		 stDxSched;
	HUINT32				*pulSlotArray = NULL;
	HERROR				 hErr;

	IPEPG_FUNC_START();

	ulSchedNum = 0;
	hErr = APK_META_SCHEDULE_CountScheduleByType(eRsvType, &ulSchedNum);
	if ((ERR_OK == hErr) && (ulSchedNum > 0))
	{
		pulSlotArray = (HUINT32 *)IPEPG_MemAlloc(sizeof(HUINT32) * ulSchedNum);
		if (NULL != pulSlotArray)
		{
			for (ulCnt = 0, ulSlotNum = 0; ulCnt < ulSchedNum; ulCnt++)
			{
				hErr = APK_META_SCHEDULE_GetScheduleByType(eRsvType, ulCnt, &stDxSched);
				if (ERR_OK == hErr)
				{
					pulSlotArray[ulSlotNum] = stDxSched.ulSlot;
					ulSlotNum ++;
				}
			}

			for (ulCnt = 0; ulCnt < ulSlotNum; ulCnt++)
			{
				(void)APK_META_SCHEDULE_CancelSchedule(pulSlotArray[ulCnt]);
			}

			IPEPG_MemFree(pulSlotArray);
		}
	}

	IPEPG_FUNC_END();
	return ERR_OK;
}

static HERROR ipepg_ClearRsv(void)
{
	IPEPG_FUNC_START();

	(void)ipepg_ClearRsvByType(DxRSVTYPE_IP_EPG_GATHERING);
	(void)ipepg_ClearRsvByType(DxRSVTYPE_IP_REMOTE_PROGRAMMING);

	IPEPG_FUNC_END();
	return ERR_OK;
}

static	HBOOL	ipepg_IsDeleteReason(IPEPG_STOP_REASON_e eStopReason)
{
	HBOOL bRet = FALSE;

	switch(eStopReason)
	{
		case eIPEPG_STOPREASON_NETWORK:
		case eIPEPG_STOPREASON_FACTORYRESET:
			bRet = TRUE;
			break;
		case eIPEPG_STOPREASON_FORMAT:
#if defined(CONFIG_IPEPG_REPOSITORY_HDD)
			bRet = TRUE;
#endif
			break;
		default:
			break;
	}
	return bRet;
}

static	HBOOL	ipepg_IsStartSoon (void)
{
#define IPEPG_FVC_TIMER_TIME 30
	UNIXTIME	utCurrTime;

	utCurrTime = (UNIXTIME)HLIB_STD_GetSystemTime();
	if (gbSetFvcTimer == TRUE
		 && utCurrTime + IPEPG_FVC_TIMER_TIME > gTimerFvcTime)
	{
		return TRUE;
	}
	if (gbSetTimer == TRUE
		 && utCurrTime + IPEPG_FVC_TIMER_TIME > gTimerTime)
	{
		return TRUE;
	}

	return FALSE;
#undef IPEPG_FVC_TIMER_TIME
}

static HUINT32 ipepg_GetRsvTimeFromMac(void)
{
	HUINT8 * pMacAddress = NULL;
	HUINT32	ulNum = 0;

	IPEPG_FUNC_START();
	pMacAddress = (HUINT8 *)IPEPG_GetItemPoint(eIPEPG_ITEM_MAC);

	if(pMacAddress != NULL)
	{
		HUINT32 i = 0;
		HUINT32 ulStrLen = HxSTD_StrLen(pMacAddress);
		HUINT32 ulColonCount = 0;

		for(i = 0; i < ulStrLen; i++)
		{
			if(pMacAddress[i] == ':')
			{
				ulColonCount++;
			}

			if(ulColonCount == 5)
				break;
		}

		if(ulColonCount == 5)
		{
			HUINT8 * pLastAddress = &pMacAddress[i+1];
			ulNum = strtol(pLastAddress, NULL, 16);
		}
	}

	ulNum = (ulNum *210)/255;	// 255(mac)를 210분으로 정리 하도록 함. 3시간 30분 . 시작시간  : 1시 ~ 4시 30분 .

	IPEPG_MemFree(pMacAddress);
	IPEPG_FUNC_END();
	return ulNum;
}

static HERROR ipepg_SetRsvForIpepg (void)
{
	UNIXTIME			 utSystemTime = 0, utStartTime = 0, utExpiryTime =0;
	DxSchedule_t		 stDxSched;
	HERROR				 hErr;
	DxFvcExpiry_t		 stFvcExpiry;

	IPEPG_FUNC_START();

	HxLOG_Debug("eIPEPG_MSG_DONE_DOWNLOAD Success Epg Download \n");
	utSystemTime = HLIB_STD_GetSystemTime();

	(void)ipepg_ClearRsvByType(DxRSVTYPE_IP_EPG_GATHERING);

	if ((HBOOL)IPEPG_GetItemValue(eIPEPG_ITEM_FLAG_IPEPG_ONOFF) == FALSE)
	{
		IPEPG_FUNC_END();
		return ERR_OK;
	}

	hErr = IPEPG_GetServiceExpiry(&stFvcExpiry);
	if (hErr == ERR_OK)
	{
		utExpiryTime = stFvcExpiry.expiryTime;
	}

	utStartTime = (utExpiryTime < utSystemTime+1*60*60)? utSystemTime+1*60*60 : utExpiryTime; // max ( expiryTime, cur+1hour)

	IPEPG_SetItem(eIPEPG_ITEM_LAST_UPDATE_TIME, (void *)utSystemTime);
	IPEPG_SetItem(eIPEPG_ITEM_UPDATE_TIME, (void *)utStartTime);

	HxSTD_MemSet(&stDxSched, 0, sizeof(DxSchedule_t));
	stDxSched.eRsvType = DxRSVTYPE_IP_EPG_GATHERING;
	HLIB_DATETIME_ConvertUnixTimeToDateTime(utStartTime, &(stDxSched.stStartTime));
	stDxSched.ulDuration = 10;

	hErr = APK_META_SCHEDULE_AddSchedule(&stDxSched, NULL);


#if defined(CONFIG_DEBUG) || defined(CFG_OPEN_DEBUG)
	{
		HxDATETIME_t		 stTime;

		HLIB_DATETIME_ConvertUnixTimeToDateTime(utSystemTime, &stTime);
		HxLOG_Debug("SET Current Update Time [%04d-%02d-%02d %02d:%02d:%02d] ulSystemTime[%x]\n", stTime.stDate.usYear, stTime.stDate.ucMonth, stTime.stDate.ucDay , stTime.stTime.ucHour, stTime.stTime.ucMinute, stTime.stTime.ucSecond, utSystemTime);

		HLIB_DATETIME_ConvertUnixTimeToDateTime(utStartTime, &stTime);
		HxLOG_Debug("SET Next Update Time [%04d-%02d-%02d %02d:%02d:%02d] ulStartTime[%x]\n", stTime.stDate.usYear, stTime.stDate.ucMonth, stTime.stDate.ucDay , stTime.stTime.ucHour, stTime.stTime.ucMinute, stTime.stTime.ucSecond, utStartTime);
	}
#endif

	IPEPG_FUNC_END();
	return hErr;
}

static HERROR ipepg_SetRsvForRP(void)
{
	UNIXTIME			 utSystemTime, utStartTime, utRpInterval, utPeriodStart, utPeriodEnd, utNextUpdateTime;
	DxSchedule_t		 stDxSched;
	HxDATETIME_t		 stCurDateTime;
	HERROR				 hErr;

	IPEPG_FUNC_START();

	utSystemTime	= HLIB_STD_GetSystemTime();
	utRpInterval	= IPEPG_GetItemValue(eIPEPG_ITEM_RP_UPDATE_INTERVAL);
	utPeriodStart	= IPEPG_GetItemValue(eIPEPG_ITEM_RP_PERIOD_START);
	utPeriodEnd		= IPEPG_GetItemValue(eIPEPG_ITEM_RP_PERIOD_END);


#if defined(CONFIG_DEBUG) || defined(CFG_OPEN_DEBUG)
	HxLOG_Debug("Timer %d sec\n", utRpInterval);
#endif
	(void)ipepg_ClearRsvByType(DxRSVTYPE_IP_REMOTE_PROGRAMMING);

	if ((HBOOL)IPEPG_GetItemValue(eIPEPG_ITEM_FLAG_RP_ONOFF) == FALSE)
	{
		IPEPG_FUNC_END();
		return ERR_FAIL;
	}

	// Calculate Next Time
	utStartTime = utSystemTime + utRpInterval;

#if 1
	HLIB_DATETIME_ConvertUnixTimeToDateTime(utStartTime, &stCurDateTime);
	utNextUpdateTime = stCurDateTime.stTime.ucHour * 60 *60 + stCurDateTime.stTime.ucMinute * 60;

	HxLOG_Debug("%d : %d : %d\n", utPeriodStart, utNextUpdateTime, utPeriodEnd);
	if(utPeriodStart <= utNextUpdateTime && utNextUpdateTime <= utPeriodEnd)
	{
	}
	else if(utPeriodStart > utNextUpdateTime)	// next update time이 PeriodStart 전이면 같은 날 periodStart로 설정
	{
		utNextUpdateTime = utPeriodStart;
		stCurDateTime.stTime.ucHour =  utNextUpdateTime / 3600;
		stCurDateTime.stTime.ucMinute =  (utNextUpdateTime % 3600) / 60;
		HLIB_DATETIME_ConvertDateTimeToUnixTime(&stCurDateTime, &utStartTime);
	}
	else if(utPeriodEnd < utNextUpdateTime)		// periodEnd 보다 크면 다음 날 start time으로 설정한다.
	{
		utStartTime += 60*60*24;	// NextDay
		HLIB_DATETIME_ConvertUnixTimeToDateTime(utStartTime, &stCurDateTime);

		utNextUpdateTime = utPeriodStart;
		stCurDateTime.stTime.ucHour =  utNextUpdateTime / 3600;
		stCurDateTime.stTime.ucMinute =  (utNextUpdateTime % 3600) / 60;
		HLIB_DATETIME_ConvertDateTimeToUnixTime(&stCurDateTime, &utStartTime);
	}
#else  // For Test
	ulStartTime = ulSystemTime + 300;
#endif

	IPEPG_SetItem(eIPEPG_ITEM_RP_UPDATE_TIME, (void *)utStartTime);

	HxSTD_MemSet(&stDxSched, 0, sizeof(DxSchedule_t));
	stDxSched.eRsvType = DxRSVTYPE_IP_REMOTE_PROGRAMMING;
	HLIB_DATETIME_ConvertUnixTimeToDateTime(utStartTime, &(stDxSched.stStartTime));
	stDxSched.ulDuration = 10;

	hErr = APK_META_SCHEDULE_AddSchedule(&stDxSched, NULL);

#if defined(CONFIG_DEBUG) || defined(CFG_OPEN_DEBUG)
	{
		HxDATETIME_t		 stTime;

		HLIB_DATETIME_ConvertUnixTimeToDateTime(utSystemTime, &stTime);
		HxLOG_Print("RP Current Update Time [%04d-%02d-%02d %02d:%02d:%02d] ulSystemTime[%x]\n", stTime.stDate.usYear, stTime.stDate.ucMonth, stTime.stDate.ucDay , stTime.stTime.ucHour, stTime.stTime.ucMinute, stTime.stTime.ucSecond, utSystemTime);

		HLIB_DATETIME_ConvertUnixTimeToDateTime(utStartTime, &stTime);
		HxLOG_Debug("RP Next Update Time [%04d-%02d-%02d %02d:%02d:%02d] ulStartTime[%x]\n", stTime.stDate.usYear, stTime.stDate.ucMonth, stTime.stDate.ucDay , stTime.stTime.ucHour, stTime.stTime.ucMinute, stTime.stTime.ucSecond, utStartTime);
	}
#endif

	IPEPG_FUNC_END();
	return hErr;
}

static void ipepg_mainProc(void *arg)
{
	HUINT32 			ulMainTaskQId;
	IpEpgMsg_t			stMsg;
	IPEPG_TASK_MGR_t * pstTaskMgr = NULL;

	IPEPG_FUNC_START();

	pstTaskMgr = (IPEPG_TASK_MGR_t *)arg;
	if(pstTaskMgr == NULL || IPEPG_GetMsgQId(&ulMainTaskQId) == ERR_FAIL)
	{
		IPEPG_FUNC_END();
		return;
	}

	while(1)
	{
		if (VK_MSG_ReceiveTimeout(ulMainTaskQId, (void*)&stMsg, sizeof(IpEpgMsg_t), 300) == VK_OK)
		{
			ipepg_RunState(&stMsg, pstTaskMgr);
		}
	}

	IPEPG_FUNC_END();
}

static HERROR ipepg_DestroyTask(HUINT32 ulTaskId, HBOOL * pbNeedSleep)
{
	HINT32 ulRepeatCount = 0;
	VK_ERR_CODE	eVKResult = VK_ERROR;
	IPEPG_FUNC_START();

	if(ulTaskId == 0)
	{
		IPEPG_FUNC_END();
		return ERR_FAIL;
	}

	// MessageQueue의 Lock을 해제 하고 있지 못하는 경우를 회피 하기 위해.
	while(pbNeedSleep && TRUE == *pbNeedSleep)
	{
		HxLOG_Print( "[%s] Sleep 20ms [%d]\n", __FUNCTION__ , ulRepeatCount);
		HLIB_STD_TaskSleep(20);
		ulRepeatCount++;
		if(ulRepeatCount > 5)	// Max 100ms
		{
			HxLOG_Error( "[%s] Sleep Iimeout\n", __FUNCTION__ );
			break;
		}
	}

	eVKResult = VK_TASK_Stop( ulTaskId );
	if( VK_OK != eVKResult )
	{
		HxLOG_Error( "[%s] VK_TASK_Stop() .......... Fail\n", __FUNCTION__ );
		return ERR_FAIL;
	}

	eVKResult = VK_TASK_Destroy( ulTaskId );
	if( VK_OK != eVKResult )
	{
		HxLOG_Error( "[%s] VK_TASK_Destroy() .......... FAIL\n", __FUNCTION__ );
		return ERR_FAIL;
	}

	IPEPG_FUNC_END();
	return ERR_OK;
}


static IPEPG_TASK_MGR_t * ipepg_InitDownManager(IPEPG_DATAMGR_t * pData)
{
	IPEPG_FUNC_START();

	if(pData == NULL)
	{
		IPEPG_FUNC_END();
		return NULL;
	}

	if(sTaskMgr == NULL)
	{
		sTaskMgr =  (IPEPG_TASK_MGR_t*)IPEPG_MemCalloc(sizeof(IPEPG_TASK_MGR_t));
	}

	sTaskMgr->pstData = pData;

	IPEPG_FUNC_END();
	return sTaskMgr;
}

HERROR IPEPG_GetMsgQId(HUINT32 * pMsgQId)
{
	if(sTaskMgr != NULL && sTaskMgr->ulMainMsgQid != 0)
	{
		*pMsgQId = sTaskMgr->ulMainMsgQid;
		return ERR_OK;
	}
	return ERR_FAIL;
}

HERROR IPEPG_GetSvcUploadMsgQId(HUINT32 * pMsgQId)
{
	if(sTaskMgr != NULL && sTaskMgr->ulUploadSvcMsgQId != 0)
	{
		*pMsgQId = sTaskMgr->ulUploadSvcMsgQId;
		return ERR_OK;
	}
	return ERR_FAIL;
}

HERROR IPEPG_GetEpgDownloadMsgQId(HUINT32 * pMsgQId)
{
	if(sTaskMgr != NULL && sTaskMgr->ulEpgDownloadTaskQId != 0)
	{
		*pMsgQId = sTaskMgr->ulEpgDownloadTaskQId;
		return ERR_OK;
	}
	return ERR_FAIL;
}

HERROR IPEPG_SendCommand(IPEPG_MSG_e message)
{
	HUINT32 MsgQId = 0;

	HxLOG_Debug(HxANSI_COLOR_GREEN(" Send message (%d) \n"),message);
	if(IPEPG_GetMsgQId(&MsgQId) != ERR_FAIL)
	{
		IpEpgMsg_t	stMsg;

		HxSTD_MemSet(&stMsg, 0, sizeof(IpEpgMsg_t));
		stMsg.ulMsg    = message;

		VK_MSG_Send(MsgQId, (void*)&stMsg, sizeof(IpEpgMsg_t));
		return ERR_OK;
	}

	return ERR_FAIL;
}

HERROR IPEPG_UploadSvc_SendCommand(IPEPG_MSG_e message)
{
	HUINT32 MsgQId = 0;
	if(IPEPG_GetSvcUploadMsgQId(&MsgQId) != ERR_FAIL)
	{
		IpEpgMsg_t	stMsg;

		HxSTD_MemSet(&stMsg, 0, sizeof(IpEpgMsg_t));
		stMsg.ulMsg    = message;

		VK_MSG_Send(MsgQId, (void*)&stMsg, sizeof(IpEpgMsg_t));
		return ERR_OK;
	}

	return ERR_FAIL;
}

HERROR IPEPG_DownloadData_SendCommand(IPEPG_MSG_e message)
{
	HUINT32 MsgQId = 0;
	if(IPEPG_GetEpgDownloadMsgQId(&MsgQId) != ERR_FAIL)
	{
		IpEpgMsg_t	stMsg;

		HxSTD_MemSet(&stMsg, 0, sizeof(IpEpgMsg_t));
		stMsg.ulMsg    = message;

		VK_MSG_Send(MsgQId, (void*)&stMsg, sizeof(IpEpgMsg_t));
		return ERR_OK;
	}

	return ERR_FAIL;
}

HERROR RP_SendCommand(IPEPG_MSG_e message)
{
	if(sTaskMgr->ulRsvMsgQid != 0)
	{
		IpEpgMsg_t	stMsg;

		HxSTD_MemSet(&stMsg, 0, sizeof(IpEpgMsg_t));
		stMsg.ulMsg    = message;

		VK_MSG_Send(sTaskMgr->ulRsvMsgQid, (void*)&stMsg, sizeof(IpEpgMsg_t));

		return ERR_OK;
	}
	return ERR_FAIL;
}

HERROR IPEPG_SendCommandWithParam(IpEpgMsg_t	stMsg)
{
	HUINT32 MsgQId = 0;

	HxLOG_Debug(HxANSI_COLOR_GREEN(" Send message (%d) \n"),stMsg.ulMsg);
	if(IPEPG_GetMsgQId(&MsgQId) != ERR_FAIL)
	{
		VK_MSG_Send(MsgQId, (void*)&stMsg, sizeof(IpEpgMsg_t));
		return ERR_OK;
	}

	return ERR_FAIL;
}

static void ipepg_TimerTask(void *arg)
{
	UNIXTIME ulSystemTime = 0;
	IPEPG_FUNC_START();

	while(1)
	{
		if(VK_CLOCK_GetTime((unsigned long*)&ulSystemTime) == ERR_OK )
		{
			if(gbSetTimer == TRUE && gTimerTime == 0)	// 시간이 없는 경우 START 요청이 들어오면 시간이 들어온뒤 다시 SETTING 하도록 한다.
			{
				gbSetTimer = FALSE;
				gTimerTime = ulSystemTime + 5;	// 5초후 시작
			}

			// FVC EPG timer
			if(gTimerFvcTime != 0)
			{
				HxLOG_Debug("FvcTimerTask[%d][%d][left:%d]\n", ulSystemTime, gTimerFvcTime,gTimerFvcTime-ulSystemTime);
			}
			if(ulSystemTime > gTimerFvcTime && gTimerFvcTime != 0)
			{
				gTimerFvcTime = 0;
				gbSetFvcTimer = FALSE;
				IPEPG_SendCommand(eIPEPG_MSG_START_DOWNLOAD_FVC_SCHEDULE);
				HxLOG_Debug("==>Timer : Send Command eIPEPG_MSG_START_DOWNLOAD_FVC_SCHEDULE\n");
			}

			// FVC Retry Timer
			if(gTimerFvcRetryTime != 0)
			{
				HxLOG_Debug("FvcTimerRetryTask[%d][%d][left:%d]\n", ulSystemTime, gTimerFvcRetryTime,gTimerFvcRetryTime-ulSystemTime);
			}
			if(ulSystemTime > gTimerFvcRetryTime && gTimerFvcRetryTime != 0)
			{
				gTimerFvcRetryTime = 0;
				gbSetFvcRetryTimer = FALSE;
				IPEPG_SendCommand(eIPEPG_MSG_DONE_WAITING_RETRY);
				HxLOG_Debug("==>Timer : Send Command eIPEPG_MSG_DONE_WAITING_RETRY\n");
			}

			// FVC Retry Timer
			if(gTimerFvcSeriesEpgTime != 0)
			{
				HxLOG_Debug("FvcTimerSeriesEpgTask[%d][%d][left:%d]\n", ulSystemTime, gTimerFvcSeriesEpgTime,gTimerFvcSeriesEpgTime-ulSystemTime);
			}
			if(ulSystemTime > gTimerFvcSeriesEpgTime && gTimerFvcSeriesEpgTime != 0)
			{
				gTimerFvcSeriesEpgTime = 0;
				gbSetFvcSeriesEpgTimer = FALSE;
				IPEPG_SendCommand(eIPEPG_MSG_START_DOWNLOAD_FVC_SERIES_SCHEDULE);
				HxLOG_Debug("==>Timer : Send Command eIPEPG_MSG_START_DOWNLOAD_FVC_SERIES_SCHEDULE\n");
			}

			// Base Start Timer
			if(gTimerTime != 0)
			{
				HxLOG_Debug("TimerTask[%d][%d][left:%d]\n", ulSystemTime, gTimerTime,gTimerTime-ulSystemTime);
			}
			if(ulSystemTime > gTimerTime && gTimerTime != 0)
			{
				gTimerTime = 0;
				gbSetTimer = FALSE;
				IPEPG_SendCommand(eIPEPG_MSG_START_UPDATE_SERVICELIST);
				HxLOG_Debug("==>Timer : Send Command eIPEPG_MSG_START_UPDATE_SERVICELIST\n");
			}
		}
		else
		{
			HxLOG_Debug("==>Timer : Time was not setted\n");
		}

		HLIB_STD_TaskSleep(2000);
	}
	IPEPG_FUNC_END();
}

static void ipepg_CancelTimer(void)
{
	gTimerTime = 0;
	gbSetTimer = FALSE;
}


static HERROR ipepg_SetTimer(HUINT32 time)
{
	UNIXTIME ulSystemTime = 0;
	gbSetTimer = TRUE;
	if(VK_CLOCK_GetTime((unsigned long*)&ulSystemTime) == ERR_OK)
	{
		gTimerTime = ulSystemTime + time;
		IPEPG_FUNC_END();
		return ERR_OK;
	}
	return ERR_FAIL;
}

static	HBOOL	ipepg_IsSoonToDownloadFvc (void)
{
	UNIXTIME utCurrTime = 0;

	utCurrTime = (UNIXTIME)HLIB_STD_GetSystemTime();

	if (gbSetFvcTimer == TRUE
		&& gTimerFvcTime < utCurrTime + 10)
	{
		return TRUE;
	}

	return FALSE;
}

void IPEPG_CancelTimer(void)
{
	IPEPG_FUNC_START();

	ipepg_CancelTimer();

	IPEPG_FUNC_END();
}

#if defined(CONFIG_DEBUG)
HERROR IPEPG_SetTimerDebug(HUINT32 time, const HCHAR *function, HUINT32 line)
{
	HxLOG_Debug("IPEPG_SetTimer =>%s %d\n", function, line);
	return ipepg_SetTimer(time);
}

#else
HERROR IPEPG_SetTimer(HUINT32 time)
{
	HERROR hErr = ERR_OK;
	IPEPG_FUNC_START();

	hErr = ipepg_SetTimer(time);

 	IPEPG_FUNC_END();
	return hErr;
}
#endif

void IPEPG_StartProc(IPEPG_DATAMGR_t * pData)
{
	IPEPG_TASK_MGR_t * 	pstTaskMgr = NULL;

	IPEPG_FUNC_START();

	pstTaskMgr = ipepg_InitDownManager(pData);

	if(VK_MSG_Create(IPEPG_MSGQ_COUNT, sizeof(IpEpgMsg_t), "ipepg_msg", (unsigned long *)&(pstTaskMgr->ulMainMsgQid), VK_SUSPENDTYPE_FIFO) != 0)
	{
		HxLOG_Debug("[%s][%d] VK_MSG_Create fail \n", __FUNCTION__, __LINE__);
		IPEPG_FUNC_END();
		return;
	}

	VK_TASK_Create(ipepg_mainProc, VK_TASK_PRIORITY_MW_REF, SIZE_32K, (const char*)"ipepgMain", (void*)pstTaskMgr, (unsigned long *)&(pstTaskMgr->ulMainTaskId), 0);
	VK_TASK_Start(pstTaskMgr->ulMainTaskId);

	if(VK_MSG_Create(IPEPG_MSGQ_COUNT, sizeof(IpEpgMsg_t), "rsv_msg", (unsigned long *)&(pstTaskMgr->ulRsvMsgQid), VK_SUSPENDTYPE_FIFO) != 0)
	{
		HxLOG_Debug("[%s][%d] VK_MSG_Create fail \n", __FUNCTION__, __LINE__);
		IPEPG_FUNC_END();
		return;
	}

#if defined(IPEPG_REMOTE_RECORDING_ON)
	VK_TASK_Create(Rp_Task, VK_TASK_PRIORITY_MW_REF, SIZE_32K, (const char*)"RsvTask", (void*)pstTaskMgr, (unsigned long *)&(pstTaskMgr->ulRsvTaskId), 0);
	VK_TASK_Start(pstTaskMgr->ulRsvTaskId);
#endif

	if(VK_MSG_Create(IPEPG_MSGQ_COUNT, sizeof(IpEpgMsg_t), "uploadSvc_msg", (unsigned long *)&(pstTaskMgr->ulUploadSvcMsgQId), VK_SUSPENDTYPE_FIFO) != 0)
	{
		HxLOG_Debug("[%s][%d] VK_MSG_Create fail \n", __FUNCTION__, __LINE__);
		IPEPG_FUNC_END();
		return;
	}

	VK_TASK_Create(IPEPG_UploadSvcTask, VK_TASK_PRIORITY_MW_REF, SIZE_32K, (const char*)"ipepg_serviceUp", (void*)pstTaskMgr, (unsigned long *)&(pstTaskMgr->ulUploadSvcTaskId), 0);
	VK_TASK_Start(pstTaskMgr->ulUploadSvcTaskId);

	if(VK_MSG_Create(IPEPG_MSGQ_COUNT, sizeof(IpEpgMsg_t), "epgDownload_msg", (unsigned long *)&(pstTaskMgr->ulEpgDownloadTaskQId), VK_SUSPENDTYPE_FIFO) != 0)
	{
		HxLOG_Debug("[%s][%d] VK_MSG_Create fail \n", __FUNCTION__, __LINE__);
		IPEPG_FUNC_END();
		return;
	}

	VK_TASK_Create(IPEPG_DownloadDataTask, VK_TASK_PRIORITY_MW_REF, SIZE_32K, (const char*)"ipepg_serviceDown", (void*)pstTaskMgr, (unsigned long *)&(pstTaskMgr->ulEpgDownloadTaskId), 0);
	VK_TASK_Start(pstTaskMgr->ulEpgDownloadTaskId);

	VK_TASK_Create(ipepg_TimerTask, VK_TASK_PRIORITY_MW_REF, SIZE_1K, (const char*)"ipepgTimer", (void*)pstTaskMgr, (unsigned long *)&(pstTaskMgr->ulTimerTaskId), 0);
	VK_TASK_Start(pstTaskMgr->ulTimerTaskId);

	HxLOG_Debug("Qid[%x] taskId[%d] +++ \n", pstTaskMgr->ulMainMsgQid, pstTaskMgr->ulMainTaskId);

	HLIB_STD_TaskSleep(1000);	// ipepg_mainProc start를 대기 한다.
	if(APK_POWER_IsStandbyMode() != TRUE)
	{
		// Normal start 시에만 ipepg download 를 check 하고
		// standby 시에는 timer를 통해서만 동작 한다.
		ipepg_CheckForStartingIPEPG();
	}

	HxLOG_Debug("Qid[%x] taskId[%d] ---\n", pstTaskMgr->ulMainMsgQid, pstTaskMgr->ulMainTaskId);
	IPEPG_FUNC_END();
}




/*  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!! WARNING !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	version을 확인하고, 기존의 IPEPG관련 DB파일들을 초기화(삭제) 한다
	따라서, 반영하고자 하는 모델이 있으면 기본적인 로직을 확인하고 작업할 것
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! WARNING !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!	*/
void IPEPG_CheckVersionNRemoveDB(void)
{
	HBOOL bNeedRemove = FALSE;
	HUINT32 ulLenOfCurrVersion = 0;

	IPEPG_FUNC_START();

	ulLenOfCurrVersion = HxSTD_StrLen(IPEPG_VERSION_NUM);

	// 0. IPEPG version을 관리하는 폴더가 mount or 존재 하는지 확인.
	if (FALSE == HLIB_DIR_IsDirPath(IPEPG_VERSION_DIR))
	{
		HxLOG_Warning("[%s] [%s] folder is not exist \n" , __FUNCTION__, IPEPG_VERSION_DIR);
		goto ENDPROC;
	}

	// 1. version을 체크하는 파일이 존재 하지 않는 다면?
	#if defined(IPEPG_VERSION_CHECK_NO_VERSIONFILE)
	if( FALSE == HLIB_FILE_Exist(IPEPG_VERSION_FILE) )
	{
		bNeedRemove = TRUE;
	}
	#endif

	// 2. version 정보를 비교한다.
	if( FALSE == bNeedRemove )
	{
		#define BUFFER_VERSION_MAX_LEN	 (20)
		HCHAR * pVersionNo = NULL;
		HUINT32 ulSize = 0;
		HCHAR oldVersion[BUFFER_VERSION_MAX_LEN];


		if( ERR_FAIL == HLIB_FILE_ReadFile(IPEPG_VERSION_FILE,(void**)&pVersionNo,&ulSize))
		{
			HxLOG_Warning("[%s] [%s] Can't read. someting wrong~!!!! \n", __FUNCTION__, IPEPG_VERSION_FILE);
			goto ENDPROC;
		}
		if(ulSize>=BUFFER_VERSION_MAX_LEN)
		{
			HxLOG_Warning("[%s] [%s] version data is someting strange. \n", __FUNCTION__, IPEPG_VERSION_FILE);
			ulSize = BUFFER_VERSION_MAX_LEN-1;
		}

		HxSTD_memset(oldVersion,0,BUFFER_VERSION_MAX_LEN);
		if(pVersionNo)
		{
			HxSTD_StrNCpy(oldVersion, pVersionNo, ulSize);
			HLIB_FILE_FreeBuffer(pVersionNo);
		}

		// 2-1. Version 체크
		HxLOG_Debug("[%s] oldVersion[%s] len(%d) currVersion[%s] len(%d)\n", __FUNCTION__, oldVersion,ulSize, IPEPG_VERSION_NUM, ulLenOfCurrVersion);
		if (0 != HLIB_STD_StrNCaseCmp(IPEPG_VERSION_NUM, oldVersion, ulLenOfCurrVersion))
		{
			// Version이 다르면...
			bNeedRemove = TRUE;
			HxLOG_Debug("[%s] version is diffrent. need to update db \n", __FUNCTION__);
		}
		else
		{
			HxLOG_Debug("[%s] version is same. \n", __FUNCTION__);
		}
	}

	// 3. DB 삭제 및 version 갱신
	if( TRUE == bNeedRemove )
	{
		HxLOG_Debug("[%s] DeleteAll DB related in IPEPG and initialize other values\n", __FUNCTION__);
		// DB 삭제 및 캐시 초기화
		IPEPG_SetItem(eIPEPG_ITEM_FLAG_SVC_UPDATED, (void *)FALSE);
		IPEPG_SetItem(eIPEPG_ITEM_UPDATE_TIME, (void *)0);
		IPEPG_SetItem(eIPEPG_ITEM_LAST_UPDATE_TIME, (void *)0);
		IPEPG_SetItem(eIPEPG_ITEM_STAMP_TIME, (void *)0);
		IPEPG_SetItem(eIPEPG_ITEM_DOWNLOADPROGRESS, (void *)0);
		IPEPG_DeleteAllData();

		// version 갱신
		if( ERR_FAIL == HLIB_FILE_WriteFile(IPEPG_VERSION_FILE, IPEPG_VERSION_NUM, ulLenOfCurrVersion) )
		{
			HxLOG_Warning("[%s] [%s] Can't write. somting wrong~!!!! \n", __FUNCTION__, IPEPG_VERSION_FILE);
		}
		HxLOG_Debug("[%s] ipepg version file(%s) is updated. Value is (%s)\n", __FUNCTION__, IPEPG_VERSION_FILE, IPEPG_VERSION_NUM);
	}

ENDPROC:
	IPEPG_FUNC_END();
}


#define _________STATUE_MACHINE_______

typedef IPEPG_FVC_STATE_e ipepg_stateFunc_t (IpEpgMsg_t *pstMsg, IPEPG_TASK_MGR_t *pstTaskMgr);
typedef void 				ipepg_transitionFunc_t (IPEPG_MSG_e eMsg, IPEPG_TASK_MGR_t *pstTaskMgr);

void ipepg_TransitIdle2Idle(IPEPG_MSG_e eMsg, IPEPG_TASK_MGR_t *pstTaskMgr);
void ipepg_TransitIdle2Processing(IPEPG_MSG_e eMsg, IPEPG_TASK_MGR_t *pstTaskMgr);
void ipepg_TransitProcessing2Idle(IPEPG_MSG_e eMsg, IPEPG_TASK_MGR_t *pstTaskMgr);
void ipepg_TransitProcessing2Processing(IPEPG_MSG_e eMsg, IPEPG_TASK_MGR_t *pstTaskMgr);
void ipepg_TransitProcessing2Stopping(IPEPG_MSG_e eMsg, IPEPG_TASK_MGR_t *pstTaskMgr);
void ipepg_TransitStopping2Idle(IPEPG_MSG_e eMsg, IPEPG_TASK_MGR_t *pstTaskMgr);


IPEPG_FVC_STATE_e ipepg_DoStateIdle (IpEpgMsg_t *pstMsg, IPEPG_TASK_MGR_t *pstTaskMgr);
IPEPG_FVC_STATE_e ipepg_DoStateProcessing (IpEpgMsg_t *pstMsg, IPEPG_TASK_MGR_t *pstTaskMgr);
IPEPG_FVC_STATE_e ipepg_DoStateStopping (IpEpgMsg_t *pstMsg, IPEPG_TASK_MGR_t *pstTaskMgr);

ipepg_stateFunc_t * const gStateTable[ IPEPG_FVC_STATE_NUMBER ] = {
    ipepg_DoStateIdle,
	ipepg_DoStateProcessing,
	ipepg_DoStateStopping
};

ipepg_transitionFunc_t * const gTransitionTable[ IPEPG_FVC_STATE_NUMBER ][ IPEPG_FVC_STATE_NUMBER ] = {
    { ipepg_TransitIdle2Idle		,ipepg_TransitIdle2Processing		,NULL },
    { ipepg_TransitProcessing2Idle	,ipepg_TransitProcessing2Processing	,ipepg_TransitProcessing2Stopping },
    { ipepg_TransitStopping2Idle		,NULL								,NULL }
};

/*********************************************************
* 1. state_func
*	1-1. next state를 결정한다.
*
* 2. transition_func
*	2-1. left job과 job간의 dependency에 따라 실제 동작을 한다.
*
* 3. msg
*	3-1. start_XXX
*		* idle |process -> process로 만든다.
*		* left job을 설정한다.
*	3-2. done_XXX
*		* process |stopping -> idle로 만든다.
*		* job을 정리한다.
*	3-3. stop
*		* process | stopping -> stopping 으로 만든다.
*	3-4. shutdown
*		* process | stopping -> idle로 만들어야????
*
* 4. state
*	4-1. idle
*		* 유효 입력: start_XXX,                        shutdown
*	4-2. processing
*		* 유효 입력: start_XXX, done_XXX, stop, shutdown
*	4-3. stopping
*		* 유효 입력:               done_XXX, stop, shutdown
* 5. ETC
*	5-1. state func에서 leftjob에 따라 idle과 processing을 구해낸다.
*	5-2. processing으로 들어갈땐 handle job을 한다.
**********************************************************/
#define ______RUN
IPEPG_FVC_STATE_e ipepg_RunState (IpEpgMsg_t *pstMsg, IPEPG_TASK_MGR_t *pstTaskMgr)
{
	IPEPG_FVC_STATE_e eCurState;
    IPEPG_FVC_STATE_e eNextState = IPEPG_FVC_STATE_IDLE;
	ipepg_stateFunc_t	  *getNextState = NULL;
    ipepg_transitionFunc_t *transition = NULL;
	IPEPG_MSG_e		  eMsg;

	eMsg = (IPEPG_MSG_e)pstMsg->ulMsg;
	HxLOG_Debug(HxANSI_COLOR_CYAN("Get Next State by: msg(%d)[%s] \n"),eMsg,ipepg_GetMsgString(eMsg));
	eCurState = pstTaskMgr->eCurState;
	getNextState = gStateTable[ eCurState ];
	if (getNextState)
	{
		eNextState = getNextState(pstMsg, pstTaskMgr);
	}

	HxLOG_Debug(HxANSI_COLOR_CYAN("State Transition: (%d)->(%d)\n") ,eCurState, eNextState);
	HxLOG_Debug(HxANSI_COLOR_CYAN("Transition by: msg(%d)[%s] \n"),eMsg,ipepg_GetMsgString(eMsg));
	HxLOG_Debug(HxANSI_COLOR_CYAN("Before Trans -Left job: (0x%x) eStatus[0x%08x] \n"),pstTaskMgr->eLeftJobs, pstTaskMgr->eStatus);

	transition = gTransitionTable[ eCurState ][ eNextState ];
    if (transition)
	{
        transition(eMsg, pstTaskMgr);
    }

	HxLOG_Debug(HxANSI_COLOR_CYAN("After Trans -Left job: (0x%x) eStatus[0x%08x] \n"),pstTaskMgr->eLeftJobs, pstTaskMgr->eStatus);
	pstTaskMgr->eCurState = eNextState;

    return eNextState;
}

void ipepg_SetJobs (IpEpgMsg_t *pstMsg,  IPEPG_TASK_MGR_t *pstTaskMgr)
{
	IPEPG_MSG_e eMsg;

	eMsg = pstMsg->ulMsg;
	switch (eMsg)
	{
		case eIPEPG_MSG_START_ONLY_SVCLIST: 	// SVC Upload
			IPEPG_SetItem(eIPEPG_ITEM_FLAG_SVC_UPDATED, (void *)FALSE);
			if ( ipepg_AbleToServiceListUpload(pstTaskMgr->pstData) == ERR_OK )
			{
				pstTaskMgr->eLeftJobs |= IPEPG_FVC_LEFT_JOB_SVC_UPLOAD;
				break;
			}
			break;

		case eIPEPG_MSG_START_UPDATE_SERVICELIST:	// SVC Upload, EPG download, RP
			if (ipepg_AbleToServiceListUpload(pstTaskMgr->pstData) == ERR_OK )
			{
				pstTaskMgr->eLeftJobs |= IPEPG_FVC_LEFT_JOB_SVC_UPLOAD;
			}
#if defined(IPEPG_FVC_EPG_CACHING_ON)
			if (ipepg_AbleToDownload(pstTaskMgr->pstData) == ERR_OK )
			{

				pstTaskMgr->eLeftJobs |= IPEPG_FVC_LEFT_JOB_SVC_DOWNLOAD;
				pstTaskMgr->eLeftJobs |= IPEPG_FVC_LEFT_JOB_EPG_DOWNLOAD;

			}
#endif
#if defined(IPEPG_REMOTE_RECORDING_ON)
			if (rp_AbleToDownload(pstTaskMgr->pstData) == ERR_OK)
			{
				pstTaskMgr->eLeftJobs |= IPEPG_FVC_LEFT_JOB_RP_UPLOAD;
				pstTaskMgr->eLeftJobs |= IPEPG_FVC_LEFT_JOB_RP_DOWNLOAD;
			}
#endif
			break;

		case eIPEPG_MSG_START_DOWNLOAD:			// EPG download, RP
#if defined(IPEPG_FVC_EPG_CACHING_ON)
			if (ipepg_AbleToDownload(pstTaskMgr->pstData) == ERR_OK )
			{
				pstTaskMgr->eLeftJobs |= IPEPG_FVC_LEFT_JOB_SVC_DOWNLOAD;
				pstTaskMgr->eLeftJobs |= IPEPG_FVC_LEFT_JOB_EPG_DOWNLOAD;
			}
#endif
#if defined(IPEPG_REMOTE_RECORDING_ON)
			if (rp_AbleToDownload(pstTaskMgr->pstData) == ERR_OK)
			{
				pstTaskMgr->eLeftJobs |= IPEPG_FVC_LEFT_JOB_RP_UPLOAD;
				pstTaskMgr->eLeftJobs |= IPEPG_FVC_LEFT_JOB_RP_DOWNLOAD;
			}
#endif
			break;

		case eIPEPG_MSG_START_DOWNLOAD_FVC_SERIES_SCHEDULE: 	// EPG download for specific service & time
#if defined(IPEPG_GET_SERIES_SCHEDULE_SEASON_EPISODE)
			if (ipepg_AbleToDownload(pstTaskMgr->pstData) == ERR_OK )
			{
				pstTaskMgr->eLeftJobs |= IPEPG_FVC_LEFT_JOB_SERIES_EPG_DOWNLOAD;
			}
			else
			{
				HxLOG_Debug(HxANSI_COLOR_RED("unable to download event \n"));
				HxLOG_Debug(HxANSI_COLOR_RED("retry after 600 seconds.  \n"));
				pstTaskMgr->eLeftJobs = (~IPEPG_FVC_LEFT_JOB_SERIES_EPG_DOWNLOAD) & (pstTaskMgr->eLeftJobs);
				IPEPG_SetFvcSeriesEpgTimer(600);
			}
#endif
			break;

		case eIPEPG_MSG_START_RSV_DOWNLOAD:		// RP
#if defined(IPEPG_REMOTE_RECORDING_ON)
			if (ipepg_AbleToServiceListUpload(pstTaskMgr->pstData) == ERR_OK )
			{
				pstTaskMgr->eLeftJobs |= IPEPG_FVC_LEFT_JOB_SVC_UPLOAD;
			}

			if (rp_AbleToDownload(pstTaskMgr->pstData) == ERR_OK)
			{
				pstTaskMgr->eLeftJobs |= IPEPG_FVC_LEFT_JOB_RP_UPLOAD;
				pstTaskMgr->eLeftJobs |= IPEPG_FVC_LEFT_JOB_RP_DOWNLOAD;
			}
#endif
			break;

		case eIPEPG_MSG_START_DOWNLOAD_FVC_SCHEDULE:		// FVC
#if defined(IPEPG_FVC_EPG_CACHING_ON)
			if (ipepg_AbleToDownload(pstTaskMgr->pstData) == ERR_OK )
			{
				pstTaskMgr->eLeftJobs |= IPEPG_FVC_LEFT_JOB_EPG_DOWNLOAD;
			}
#endif
			break;

		default:
			break;
	}
}

void ipepg_StartPossibleJobs (IPEPG_TASK_MGR_t *pstTaskMgr)
{
	IPEPG_FVC_LEFT_JOB_e ePossibleJobs;

	// 0. init
	ePossibleJobs = pstTaskMgr->eLeftJobs;

	// TODO: dependency check 시에 사라져야 하는 job이 현재 진행 중이면 뭘 해야 할까?
	// 1. dependency check
	if (pstTaskMgr->eLeftJobs & IPEPG_FVC_LEFT_JOB_EPG_DOWNLOAD)
	{
		ePossibleJobs = (~IPEPG_FVC_LEFT_JOB_RP_UPLOAD) & (ePossibleJobs);
		ePossibleJobs = (~IPEPG_FVC_LEFT_JOB_RP_DOWNLOAD) & (ePossibleJobs);
	}

	if (pstTaskMgr->eLeftJobs & IPEPG_FVC_LEFT_JOB_SVC_DOWNLOAD)
	{
		ePossibleJobs = (~IPEPG_FVC_LEFT_JOB_EPG_DOWNLOAD) & (ePossibleJobs);
	}

	if (pstTaskMgr->eLeftJobs & IPEPG_FVC_LEFT_JOB_SVC_UPLOAD)
	{
		ePossibleJobs = (~IPEPG_FVC_LEFT_JOB_SVC_DOWNLOAD) & (ePossibleJobs);
		ePossibleJobs = (~IPEPG_FVC_LEFT_JOB_EPG_DOWNLOAD) & (ePossibleJobs);
		ePossibleJobs = (~IPEPG_FVC_LEFT_JOB_RP_UPLOAD) & (ePossibleJobs);
		ePossibleJobs = (~IPEPG_FVC_LEFT_JOB_RP_DOWNLOAD) & (ePossibleJobs);
	}

	if (pstTaskMgr->eLeftJobs & IPEPG_FVC_LEFT_JOB_WAIT_RETRY)
	{
		ePossibleJobs = (~IPEPG_FVC_LEFT_JOB_SVC_DOWNLOAD) & (ePossibleJobs);
		ePossibleJobs = (~IPEPG_FVC_LEFT_JOB_EPG_DOWNLOAD) & (ePossibleJobs);
		ePossibleJobs = (~IPEPG_FVC_LEFT_JOB_SERIES_EPG_DOWNLOAD) & (ePossibleJobs);
	}

	if (ipepg_IsSoonToDownloadFvc() == TRUE)
	{
		ePossibleJobs = (~IPEPG_FVC_LEFT_JOB_RP_UPLOAD) & (ePossibleJobs);
		ePossibleJobs = (~IPEPG_FVC_LEFT_JOB_RP_DOWNLOAD) & (ePossibleJobs);
	}


	HxLOG_Debug(HxANSI_COLOR_CYAN("Possible job: (0x%x) \n"),ePossibleJobs);

	// 2. if any job is left to do & the job is not currently going on
	//     then do the job
	if ((ePossibleJobs & IPEPG_FVC_LEFT_JOB_SVC_UPLOAD)
		&& IPEPG_IsFlagUnsetted(pstTaskMgr->eStatus, eIPEPG_UPLOAD_START))
	{
		HxLOG_Debug("start upload svc \n");
		pstTaskMgr->eStatus = pstTaskMgr->eStatus | eIPEPG_UPLOAD_START;
		IPEPG_UploadSvc_SendCommand(eIPEPG_MSG_START_ONLY_SVCLIST);
	}

	if ((ePossibleJobs & IPEPG_FVC_LEFT_JOB_SVC_DOWNLOAD)
		&& IPEPG_IsFlagUnsetted(pstTaskMgr->eStatus, eIPEPG_DOWN_START))
	{
		HxLOG_Debug("start download svc \n");
		pstTaskMgr->eStatus = pstTaskMgr->eStatus | eIPEPG_DOWN_START;
		IPEPG_DownloadData_SendCommand(eIPEPG_MSG_START_DOWNLOAD);
	}

	if ((ePossibleJobs & IPEPG_FVC_LEFT_JOB_EPG_DOWNLOAD)
		&& IPEPG_IsFlagUnsetted(pstTaskMgr->eStatus, eIPEPG_FVC_START))

	{
		HxLOG_Debug("start download epg \n");
		pstTaskMgr->eStatus = pstTaskMgr->eStatus | eIPEPG_FVC_START;
		IPEPG_DownloadData_SendCommand(eIPEPG_MSG_START_DOWNLOAD_FVC_SCHEDULE);
	}

	if ((ePossibleJobs & IPEPG_FVC_LEFT_JOB_SERIES_EPG_DOWNLOAD)
		&& IPEPG_IsFlagUnsetted(pstTaskMgr->eStatus, eIPEPG_FVC_SERIES_EPG_START))
	{
		HxLOG_Debug("start download series epg \n");
		pstTaskMgr->eStatus = pstTaskMgr->eStatus | eIPEPG_FVC_SERIES_EPG_START;
		IPEPG_DownloadData_SendCommand(eIPEPG_MSG_START_DOWNLOAD_FVC_SERIES_SCHEDULE);
	}

	if ((ePossibleJobs & IPEPG_FVC_LEFT_JOB_RP_UPLOAD)
		&& IPEPG_IsFlagUnsetted(pstTaskMgr->eStatus, eIPEPG_RP_START))
	{
		HxLOG_Debug("start rp upload \n");
		pstTaskMgr->eStatus = pstTaskMgr->eStatus | eIPEPG_RP_START;
		RP_SendCommand(eIPEPG_MSG_START_RSV_DOWNLOAD);
	}

	if ((ePossibleJobs & IPEPG_FVC_LEFT_JOB_WAIT_RETRY)
		&& (pstTaskMgr->bIsWaitingNow == FALSE))
	{
		HUINT32 utRetryTime = 1;
		HUINT32 i = 0;

		// Rule of  D-Book 7.0 part b 2.4.6.3
		for (i=0; i<pstTaskMgr->ulCurrentRetry && i<=10; i++)
		{
			utRetryTime *= 4;
		}
		utRetryTime /= 10;	// 100 milliseconds -> 1 seconds
		IPEPG_SetFvcRetryTimer(utRetryTime);
		pstTaskMgr->bIsWaitingNow = TRUE;
		++pstTaskMgr->ulCurrentRetry;
	}

#if  0 //defined(CONFIG_DEBUG)
	if ((ePossibleJobs&IPEPG_FVC_LEFT_JOB_WHOLE_MASK) == IPEPG_FVC_LEFT_JOB_NOTHING)
	{
		HxLOG_Error("No Job is set. \n");
		HxLOG_Assert(1);
		HxLOG_Assert(0);
	}
#endif
}

void ipepg_StopJobs (IPEPG_TASK_MGR_t *pstTaskMgr)
{
	// 1. stop the jobs which haven't started yet
	if (IPEPG_IsFlagUnsetted(pstTaskMgr->eStatus,eIPEPG_UPLOAD_START))
	{
		pstTaskMgr->eLeftJobs = (~IPEPG_FVC_LEFT_JOB_SVC_UPLOAD) & (pstTaskMgr->eLeftJobs);
	}
	if (IPEPG_IsFlagUnsetted(pstTaskMgr->eStatus,eIPEPG_DOWN_START))
	{
		pstTaskMgr->eLeftJobs = (~IPEPG_FVC_LEFT_JOB_SVC_DOWNLOAD) & (pstTaskMgr->eLeftJobs);
	}
	if (IPEPG_IsFlagUnsetted(pstTaskMgr->eStatus,eIPEPG_RP_START))
	{
		pstTaskMgr->eLeftJobs = (~IPEPG_FVC_LEFT_JOB_RP_UPLOAD) & (pstTaskMgr->eLeftJobs);
		pstTaskMgr->eLeftJobs = (~IPEPG_FVC_LEFT_JOB_RP_DOWNLOAD) & (pstTaskMgr->eLeftJobs);
	}
	if (IPEPG_IsFlagUnsetted(pstTaskMgr->eStatus,eIPEPG_FVC_START))
	{
		pstTaskMgr->eLeftJobs = (~IPEPG_FVC_LEFT_JOB_EPG_DOWNLOAD) & (pstTaskMgr->eLeftJobs);
		IPEPG_CancelFvcTimer();
	}
	if (IPEPG_IsFlagUnsetted(pstTaskMgr->eStatus,eIPEPG_FVC_SERIES_EPG_START))
	{
		pstTaskMgr->eLeftJobs = (~IPEPG_FVC_LEFT_JOB_SERIES_EPG_DOWNLOAD) & (pstTaskMgr->eLeftJobs);
		IPEPG_CancelFvcSeriesEpgTimer();
	}

	// 2. stop the jobs which have started already
	if (pstTaskMgr->bIsWaitingNow != FALSE)
	{
		pstTaskMgr->eLeftJobs = (~IPEPG_FVC_LEFT_JOB_WAIT_RETRY) & (pstTaskMgr->eLeftJobs);
		IPEPG_CancelFvcRetryTimer();
		pstTaskMgr->bIsWaitingNow = FALSE;
	}
}

void ipepg_ProcessSucceedJob (IpEpgMsg_t *pstMsg,  IPEPG_TASK_MGR_t *pstTaskMgr)
{
	IPEPG_MSG_e eMsg;

	eMsg = pstMsg->ulMsg;
	switch (eMsg)
	{
		case eIPEPG_MSG_DONE_DOWNLOAD:				// SVC download done
			HxLOG_Debug(" => DOWNLOAD STOP.  \n");
			pstTaskMgr->eStatus = (~eIPEPG_DOWN_START) & (pstTaskMgr->eStatus);
			pstTaskMgr->eLeftJobs = (~IPEPG_FVC_LEFT_JOB_SVC_DOWNLOAD) & (pstTaskMgr->eLeftJobs);
			IPEPG_RefreshExpiryQueue(NULL);
			IPEPG_DeleteOldIpEpg();
			ipepg_SetRsvForIpepg();
			break;

		case eIPEPG_MSG_DONE_UPDATE_SERVICELIST:	// SVC Upload done
			HxLOG_Debug(" => UPLOAD STOP.  \n");
			pstTaskMgr->eStatus = (~eIPEPG_UPLOAD_START) & (pstTaskMgr->eStatus);
			pstTaskMgr->eLeftJobs = (~IPEPG_FVC_LEFT_JOB_SVC_UPLOAD) & (pstTaskMgr->eLeftJobs);

			IPEPG_SetItem(eIPEPG_ITEM_FLAG_SVC_UPDATED, (void *)TRUE);
			break;

		case eIPEPG_MSG_DONE_DOWNLOAD_FVC_SCHEDULE:	// FVC EPG download done
			HxLOG_Debug(" => FVC STOP.  \n");
			pstTaskMgr->eStatus = (~eIPEPG_FVC_START) & (pstTaskMgr->eStatus);
			pstTaskMgr->eLeftJobs = (~IPEPG_FVC_LEFT_JOB_EPG_DOWNLOAD) & (pstTaskMgr->eLeftJobs);
			pstTaskMgr->ulCurrentRetry = 0;

			(void)IPEPG_SetNextWakeup();
			break;

		case eIPEPG_MSG_DONE_STB_UPLOAD:			// RP done
			HxLOG_Debug("eIPEPG_MSG_DONE_STB_UPLOAD TaskId (0x%x) \n", pstTaskMgr->ulRsvTaskId);
			pstTaskMgr->eStatus = (~eIPEPG_RP_START) & (pstTaskMgr->eStatus);
			pstTaskMgr->eLeftJobs = (~IPEPG_FVC_LEFT_JOB_RP_UPLOAD) & (pstTaskMgr->eLeftJobs);
			pstTaskMgr->eLeftJobs = (~IPEPG_FVC_LEFT_JOB_RP_DOWNLOAD) & (pstTaskMgr->eLeftJobs);

#if defined(IPEPG_REMOTE_RECORDING_ON)
			(void)ipepg_SetRsvForRP();
#endif
			break;

		case eIPEPG_MSG_DONE_WAITING_RETRY:		// currentRetry Waiting done
			HxLOG_Debug("eIPEPG_MSG_DONE_WAITING_RETRY \n");
			pstTaskMgr->bIsWaitingNow = FALSE;
			pstTaskMgr->eLeftJobs = (~IPEPG_FVC_LEFT_JOB_WAIT_RETRY) & (pstTaskMgr->eLeftJobs);
			break;

		case eIPEPG_MSG_DONE_DOWNLOAD_FVC_SERIES_SCHEDULE:				// FVC Series EPG download done
			HxLOG_Debug(" => FVC SERIES DOWNLOAD STOP. \n");
			pstTaskMgr->eStatus = (~eIPEPG_FVC_SERIES_EPG_START) & (pstTaskMgr->eStatus);
			pstTaskMgr->eLeftJobs = (~IPEPG_FVC_LEFT_JOB_SERIES_EPG_DOWNLOAD) & (pstTaskMgr->eLeftJobs);
			pstTaskMgr->ulCurrentRetry = 0;
			IPEPG_DeleteOldIpEpg();
			APK_META_SCHEDULE_UpdateSeries();
			break;

		default:
			break;
	}
}


void ipepg_ProcessFailedJob (IpEpgMsg_t *pstMsg,  IPEPG_TASK_MGR_t *pstTaskMgr)
{
	IPEPG_MSG_e eMsg;
	HUINT32		ulHttpStatusCode;

	eMsg = pstMsg->ulMsg;
	ulHttpStatusCode = pstMsg->ulParam3;
	HxLOG_Debug(HxANSI_COLOR_RED("Failed job. msg=(%d), statusCode=(%d) \n"),eMsg, ulHttpStatusCode);
	switch (eMsg)
	{
		case eIPEPG_MSG_DONE_DOWNLOAD:				// SVC download done
		case eIPEPG_MSG_DONE_DOWNLOAD_FVC_SCHEDULE:	// FVC EPG download done
		case eIPEPG_MSG_DONE_DOWNLOAD_FVC_SERIES_SCHEDULE: // FVC EPG download done
			if (ulHttpStatusCode == 500
				|| ulHttpStatusCode == 504
				|| ulHttpStatusCode == 503)
			{
				HxLOG_Debug("Failed Job. Wait retry. statusCode=(%d) \n",ulHttpStatusCode);
				pstTaskMgr->eLeftJobs |= IPEPG_FVC_LEFT_JOB_WAIT_RETRY;
			}

			if (ulHttpStatusCode == IPEPG_FVC_STATUS_CODE_UNAUTHORIZED)
			{
				IPEPG_RequestAuth();
			}

			if (eMsg == eIPEPG_MSG_DONE_DOWNLOAD)
			{
				HxLOG_Debug(" => DOWNLOAD STOP. \n");

				pstTaskMgr->eStatus = (~eIPEPG_DOWN_START) & (pstTaskMgr->eStatus);
				IPEPG_RefreshExpiryQueue(NULL);
				IPEPG_DeleteOldIpEpg();
				ipepg_SetRsvForIpepg();
				//(void)IPEPG_SetNextWakeup();	// 해야 하나?
			}
			else if (eMsg == eIPEPG_MSG_DONE_DOWNLOAD_FVC_SCHEDULE)
			{
				HxLOG_Debug(" => FVC STOP.  \n");

				pstTaskMgr->eStatus = (~eIPEPG_FVC_START) & (pstTaskMgr->eStatus);

				(void)IPEPG_SetNextWakeup();
			}
			else if (eMsg == eIPEPG_MSG_DONE_DOWNLOAD_FVC_SERIES_SCHEDULE)
			{
				HxLOG_Debug(" => FVC SERIES EVT STOP. \n");

				pstTaskMgr->eStatus = (~eIPEPG_FVC_SERIES_EPG_START) & (pstTaskMgr->eStatus);

				IPEPG_SetFvcSeriesEpgTimer(10);
			}


			break;

		default:
			break;
	}
}
#define ______idle_state
IPEPG_FVC_STATE_e ipepg_DoStateIdle (IpEpgMsg_t *pstMsg, IPEPG_TASK_MGR_t *pstTaskMgr)
{
	IPEPG_FVC_STATE_e	eNextState = IPEPG_FVC_STATE_IDLE;
	IPEPG_MSG_e 		eMsg;
	IPEPG_STOP_REASON_e eStopReason;

	// 1. set new jobs for start messages
	ipepg_SetJobs(pstMsg, pstTaskMgr);

	eMsg = pstMsg->ulMsg;
	switch (eMsg)
	{
		case eIPEPG_MSG_REQUEST_SHUTDOWN_NOTI:
			if (ipepg_IsStartSoon() == TRUE)
			{
				HxLOG_Error("Still processing ipepg. left job=(0x%x)",pstTaskMgr->eLeftJobs);
				APK_POWER_WaitShutDown(4000);
			}
			else
			{
				if (pstTaskMgr->eLeftJobs != IPEPG_FVC_LEFT_JOB_NOTHING)
				{
					HxLOG_Error("There are left jobs. jobs=(0x%x) \n",pstTaskMgr->eLeftJobs);
				}
				pstTaskMgr->eLeftJobs = IPEPG_FVC_LEFT_JOB_NOTHING;
				IPEPG_CancelFvcRetryTimer();
				pstTaskMgr->bIsWaitingNow = FALSE;
				IPEPG_CancelFvcTimer();
				IPEPG_CancelTimer();
				IPEPG_CancelFvcSeriesEpgTimer();
			}
			break;

		case eIPEPG_MSG_START_ALL:					// SVC Upload, EPG download, RP
			IPEPG_SetItem(eIPEPG_ITEM_FLAG_SVC_UPDATED, (void *)FALSE);
			IPEPG_SetItem(eIPEPG_ITEM_UPDATE_TIME, (void *)0);
			IPEPG_SetItem(eIPEPG_ITEM_LAST_UPDATE_TIME, (void *)0);
			IPEPG_SetItem(eIPEPG_ITEM_STAMP_TIME, (void *)0);
			IPEPG_SetItem(eIPEPG_ITEM_DOWNLOADPROGRESS, (void *)0);
			return IPEPG_FVC_STATE_IDLE;

		case eIPEPG_MSG_STOP:
			eStopReason = pstMsg->ulParam1;
			if (ipepg_IsDeleteReason(eStopReason) == TRUE)
			{
				IPEPG_DeleteAllData();
			}
			break;

		default:
			break;
	}

	// 4. next state
	if (pstTaskMgr->eLeftJobs == IPEPG_FVC_LEFT_JOB_NOTHING)
	{
		eNextState = IPEPG_FVC_STATE_IDLE;
	}
	else
	{
		eNextState = IPEPG_FVC_STATE_PROCESSING;
	}

	return eNextState;
}

void ipepg_TransitIdle2Idle(IPEPG_MSG_e eMsg, IPEPG_TASK_MGR_t *pstTaskMgr)
{
	switch (eMsg)
	{
		case eIPEPG_MSG_START_ALL:					// SVC Upload, EPG download, RP
			IPEPG_SendCommand(eIPEPG_MSG_START_UPDATE_SERVICELIST);
			break;

		default:
			break;
	}
}

void ipepg_TransitIdle2Processing(IPEPG_MSG_e eMsg, IPEPG_TASK_MGR_t *pstTaskMgr)
{
	ipepg_StartPossibleJobs(pstTaskMgr);
}

#define ______Processing_state
IPEPG_FVC_STATE_e ipepg_DoStateProcessing (IpEpgMsg_t *pstMsg, IPEPG_TASK_MGR_t *pstTaskMgr)
{
	IPEPG_FVC_STATE_e 	eNextState;
	IPEPG_MSG_e 		eMsg;
	IPEPG_STOP_REASON_e eStopReason;
	HERROR				hIsJobSucceed = ERR_FAIL;

	// 1. set new jobs for start messages
	ipepg_SetJobs(pstMsg, pstTaskMgr);

	// 2. clean up job for done messages
	hIsJobSucceed = pstMsg->ulParam2;
	if (hIsJobSucceed == ERR_OK)
	{
		ipepg_ProcessSucceedJob(pstMsg, pstTaskMgr);
	}
	else
	{
		ipepg_ProcessFailedJob(pstMsg, pstTaskMgr);
	}

	// 3. other processing
	eMsg = pstMsg->ulMsg;
	switch (eMsg)
	{
		// TODO: retry 중에 shutdown noti에 대한 처리가 필요하다.
		case eIPEPG_MSG_REQUEST_SHUTDOWN_NOTI:
			HxLOG_Error("Still processing ipepg. left job=(0x%x) \n",pstTaskMgr->eLeftJobs);
			APK_POWER_WaitShutDown(4000);
			return IPEPG_FVC_STATE_STOPPING;

		case eIPEPG_MSG_START_ALL:					// SVC Upload, EPG download, RP
			IPEPG_SetItem(eIPEPG_ITEM_FLAG_SVC_UPDATED, (void *)FALSE);
			IPEPG_SetItem(eIPEPG_ITEM_UPDATE_TIME, (void *)0);
			IPEPG_SetItem(eIPEPG_ITEM_LAST_UPDATE_TIME, (void *)0);
			IPEPG_SetItem(eIPEPG_ITEM_STAMP_TIME, (void *)0);
			IPEPG_SetItem(eIPEPG_ITEM_DOWNLOADPROGRESS, (void *)0);
			// TODO: stop 후에 처음부터 다시 받아야 한다.
			return IPEPG_FVC_STATE_STOPPING;

		case eIPEPG_MSG_STOP:
			HxLOG_Debug("Stop all operation. \n");
			eStopReason = pstMsg->ulParam1;
			if (ipepg_IsDeleteReason(eStopReason) == TRUE)
			{
				IPEPG_DeleteAllData();
			}
			ipepg_StopJobs(pstTaskMgr);
			if (pstTaskMgr->eLeftJobs == IPEPG_FVC_LEFT_JOB_NOTHING)
			{
				return IPEPG_FVC_STATE_IDLE;
			}
			else
			{
				return IPEPG_FVC_STATE_STOPPING;
			}


		default:
			break;
	}

	// 4. next state
	if (pstTaskMgr->eLeftJobs == IPEPG_FVC_LEFT_JOB_NOTHING)
	{
		eNextState = IPEPG_FVC_STATE_IDLE;
	}
	else
	{
		eNextState = IPEPG_FVC_STATE_PROCESSING;
	}

	return eNextState;
}
void ipepg_TransitProcessing2Idle(IPEPG_MSG_e eMsg, IPEPG_TASK_MGR_t *pstTaskMgr)
{

}

void ipepg_TransitProcessing2Processing(IPEPG_MSG_e eMsg, IPEPG_TASK_MGR_t *pstTaskMgr)
{
	ipepg_StartPossibleJobs(pstTaskMgr);
}

void ipepg_TransitProcessing2Stopping(IPEPG_MSG_e eMsg, IPEPG_TASK_MGR_t *pstTaskMgr)
{
	switch (eMsg)
	{
		case eIPEPG_MSG_START_ALL:					// SVC Upload, EPG download, RP
			IPEPG_SendCommand(eIPEPG_MSG_STOP);
			pstTaskMgr->eStopReason = eIPEPG_STOPREASON_GETALL;
			break;

		default:
			break;
	}
}

#define ______Stopping_state
IPEPG_FVC_STATE_e ipepg_DoStateStopping (IpEpgMsg_t *pstMsg, IPEPG_TASK_MGR_t *pstTaskMgr)
{
	IPEPG_MSG_e			eMsg;
	IPEPG_STOP_REASON_e eStopReason;

	eMsg = pstMsg->ulMsg;
	// 1. don't set jobs for start messages

	// 2. clean up job for done messages
	// even if it's failed job, process it as succeed
	ipepg_ProcessSucceedJob(pstMsg, pstTaskMgr);

	// 3. other
	switch (eMsg)
	{
		case eIPEPG_MSG_REQUEST_SHUTDOWN_NOTI:
			HxLOG_Error("Still processing ipepg. left job=(0x%x) \n",pstTaskMgr->eLeftJobs);
			APK_POWER_WaitShutDown(4000);
			break;
		case eIPEPG_MSG_STOP:
			eStopReason = pstMsg->ulParam1;
			if (ipepg_IsDeleteReason(eStopReason) == TRUE)
			{
				IPEPG_DeleteAllData();
			}
			// TODO: get all 에 대한 reason일 경우 새로 시작할 수 있어야 한다.epg c
			break;
		default:
			break;
	}

	// 3. if no job is going on
	//    then ignore leftjobs and go quit
	if (pstTaskMgr->eStatus == 0)
	{
		HxLOG_Debug("All stopped. \n");
		return IPEPG_FVC_STATE_IDLE;
	}

	// return current state
	return IPEPG_FVC_STATE_STOPPING;
}

void ipepg_TransitStopping2Idle(IPEPG_MSG_e eMsg, IPEPG_TASK_MGR_t *pstTaskMgr)
{
	if (pstTaskMgr->eLeftJobs != IPEPG_FVC_LEFT_JOB_NOTHING)
	{
		HxLOG_Error("There are left jobs. But ignore left jobs=(0x%x) \n",pstTaskMgr->eLeftJobs);
	}

	pstTaskMgr->eLeftJobs = IPEPG_FVC_LEFT_JOB_NOTHING;
	IPEPG_CancelFvcRetryTimer();
	pstTaskMgr->bIsWaitingNow = FALSE;
	IPEPG_CancelFvcTimer();
	IPEPG_CancelFvcSeriesEpgTimer();
}



/*********************** End of File ******************************/


