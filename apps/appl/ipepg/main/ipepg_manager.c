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

#define	ONEDAY_INTERVAL_TIME	24*60*60

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
	#define IPEPG_FUNC_END()	HxLOG_Debug( HxANSI_COLOR_CYAN("[%s][%d] --- \n") , __FUNCTION__, __LINE__);
#else
	#define IPEPG_FUNC_START()	HxLOG_Debug("[%s][%d] +++ \n" , __FUNCTION__, __LINE__);
	#define IPEPG_FUNC_END()	HxLOG_Debug("[%s][%d] --- \n" , __FUNCTION__, __LINE__);
#endif

static IPEPG_TASK_MGR_t *	sTaskMgr	= NULL;
static HUINT32				gTimerTime	= 0;
static HBOOL				gbSetTimer	= FALSE;
static HUINT32				s_ulSyncCallMainProcEventId;

static HERROR ipepg_SetTimer(HUINT32 time);

extern HCHAR* UAPI_FM_GetDefaultURL(HCHAR *pszDefaultURL, HCHAR *pszURLFileName);

static HERROR ipepg_CheckForStartingIPEPG(void)
{
	HBOOL	bIpEpgEnabled = FALSE;
	HBOOL	bRpEnabled = FALSE;
	HBOOL	bServiceUpdated = FALSE;
	UNIXTIME	ulUpdateTime = 0, ulLastUpdateTime = 0, utSystemTime =0;
	HUINT32		ulIpepgRsvCount = 0;

	IPEPG_FUNC_START();

	bIpEpgEnabled = (HBOOL)IPEPG_GetItemValue(eIPEPG_ITEM_FLAG_IPEPG_ONOFF);
	if(bIpEpgEnabled == FALSE)
	{
		HxLOG_Debug("IPEPG Disable...\n");
		goto TARGET_END;
	}

	bServiceUpdated = (HBOOL)IPEPG_GetItemValue(eIPEPG_ITEM_FLAG_SVC_UPDATED);
	if(bServiceUpdated == FALSE)
	{
		HxLOG_Warning("[eIPEPG_MSG_START_ALL : %d]\n", eIPEPG_MSG_START_ALL);
		HxLOG_Debug("Already Service Updated\n");
		IPEPG_SendCommand(eIPEPG_MSG_START_ALL);
		goto TARGET_END;
	}

	ulUpdateTime = IPEPG_GetItemValue(eIPEPG_ITEM_UPDATE_TIME);
	if(ulUpdateTime == 0)
	{
		HxLOG_Warning("[eIPEPG_MSG_START_ALL : %d]\n", eIPEPG_MSG_START_ALL);
		HxLOG_Debug("Update Time is never Updated\n");
		IPEPG_SendCommand(eIPEPG_MSG_START_ALL);
		goto TARGET_END;
	}

	ulLastUpdateTime = IPEPG_GetItemValue(eIPEPG_ITEM_LAST_UPDATE_TIME);
	if(ulLastUpdateTime == 0)
	{
		HxLOG_Warning("[eIPEPG_MSG_START_ALL : %d]\n", eIPEPG_MSG_START_ALL);
		HxLOG_Debug("Last Update Time is never Updated\n");
		IPEPG_SendCommand(eIPEPG_MSG_START_ALL);
		goto TARGET_END;
	}

	if(APK_META_SCHEDULE_CountScheduleByType(DxRSVTYPE_IP_EPG_GATHERING, &ulIpepgRsvCount) != ERR_OK)
	{
		HxLOG_Error("Error, SAPI_CountScheduleByType\n");
		utSystemTime = HLIB_STD_GetSystemTime();
		if(ulLastUpdateTime + ONEDAY_INTERVAL_TIME < utSystemTime)
		{
			HxLOG_Warning("[eIPEPG_MSG_START_ALL : %d]\n", eIPEPG_MSG_START_ALL);
			IPEPG_SendCommand(eIPEPG_MSG_START_ALL);
			goto TARGET_END;
		}
	}
	else
	{
		if(ulIpepgRsvCount < 1)
		{
			HxLOG_Warning("[eIPEPG_MSG_START_ALL : %d]\n", eIPEPG_MSG_START_ALL);
			HxLOG_Debug("There is none Ipepg Gathering Schedule.\n");

			IPEPG_SendCommand(eIPEPG_MSG_START_ALL);
			goto TARGET_END;
		}
	}

	bRpEnabled = (HBOOL)IPEPG_GetItemValue(eIPEPG_ITEM_FLAG_RP_ONOFF);
	if(bRpEnabled == TRUE)
	{
		HxLOG_Debug("Do RSV Download...\n");
		IPEPG_SendCommand(eIPEPG_MSG_START_RSV_DOWNLOAD);
	}

TARGET_END:

	IPEPG_FUNC_END();
	return ERR_OK;
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

	if(bRpEnabled == TRUE
		&& bServiceUpdated == TRUE && pMacAddress != NULL)
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

	IPEPG_FUNC_START();

	if(ERR_OK != VK_CLOCK_GetTime((unsigned long*)&ulSystemTime))
	{
		HxLOG_Debug("[%s] GetTime is invalid\n", __FUNCTION__ );
		return ERR_FAIL;
	}

	bConnected = (HBOOL)IPEPG_GetItemValue(eIPEPG_ITEM_NETWORK);
	bIpEpgEnabled = (HBOOL)IPEPG_GetItemValue(eIPEPG_ITEM_FLAG_IPEPG_ONOFF);
	bServiceUpdated = (HBOOL)IPEPG_GetItemValue(eIPEPG_ITEM_FLAG_SVC_UPDATED);
	bAbleToUseHdd = (HBOOL)IPEPG_GetItemValue(eIPEPG_ITEM_HDD_STATUS);
	pMacAddress = (HUINT8 *)IPEPG_GetItemPoint(eIPEPG_ITEM_MAC);

	if (bIpEpgEnabled == TRUE && bConnected == FALSE)
	{
	}

	HxLOG_Debug("bIpEpgEnabled[%d], bServiceUpdated[%d],  pMacAddress[%s], bConnected[%d]\n", bIpEpgEnabled, bServiceUpdated, pMacAddress, bConnected);
	if(bConnected == TRUE && bIpEpgEnabled == TRUE && bAbleToUseHdd == TRUE && pMacAddress != NULL
		&& bServiceUpdated == FALSE)
	{
		err = ipepg_GetServerUriFromFile();	// RET OK or FAIL
	}

	IPEPG_MemFree(pMacAddress);
	IPEPG_FUNC_END();

	return err;
}

static HERROR	ipepg_AbleToDownload(IPEPG_DATAMGR_t 	* pstData)
{
	UNIXTIME	ulSystemTime = 0;
	UNIXTIME	ulUpdateTime = 0, ulLastUpdateTime = 0;
	HBOOL		bConnected = FALSE;
	HBOOL		bIpEpgEnabled = FALSE;
	HBOOL		bAbleToUseHdd = FALSE;
	HBOOL		bServiceUpdated = FALSE;
	HUINT8		*pMacAddress = NULL;
	HERROR		err = ERR_FAIL;

	IPEPG_FUNC_START();

	if(ERR_OK != VK_CLOCK_GetTime((unsigned long*)&ulSystemTime))
	{
		HxLOG_Debug("[%s] GetTime is invalid\n", __FUNCTION__ );
		return ERR_FAIL;
	}

	ulUpdateTime = IPEPG_GetItemValue(eIPEPG_ITEM_UPDATE_TIME);
	ulLastUpdateTime = IPEPG_GetItemValue(eIPEPG_ITEM_LAST_UPDATE_TIME);

	// Check UpdateTime
	if(ulSystemTime < ulUpdateTime && ulUpdateTime != 0 && ulLastUpdateTime != 0)
	{
#if defined(CONFIG_DEBUG) || defined(CFG_OPEN_DEBUG)
		HxDATETIME_t stStartTime;
		HLIB_DATETIME_ConvertUnixTimeToDateTime(ulSystemTime, &stStartTime);
		HxLOG_Debug("Current Update Time [%04d-%02d-%02d %02d:%02d:%02d] ulSystemTime[%x]\n", stStartTime.stDate.usYear, stStartTime.stDate.ucMonth, stStartTime.stDate.ucDay , stStartTime.stTime.ucHour, stStartTime.stTime.ucMinute, stStartTime.stTime.ucSecond, ulSystemTime);
		HLIB_DATETIME_ConvertUnixTimeToDateTime(ulUpdateTime, &stStartTime);
		HxLOG_Debug("NO NEED UPDATE Last Update Time [%04d-%02d-%02d %02d:%02d:%02d] ulUpdateTime[%x]\n", stStartTime.stDate.usYear, stStartTime.stDate.ucMonth, stStartTime.stDate.ucDay , stStartTime.stTime.ucHour, stStartTime.stTime.ucMinute, stStartTime.stTime.ucSecond, ulUpdateTime);
#endif
		IPEPG_FUNC_END();
		return ERR_FAIL;
	}
	else
	{
#if defined(CONFIG_DEBUG) || defined(CFG_OPEN_DEBUG)
		HxDATETIME_t stStartTime;
		HLIB_DATETIME_ConvertUnixTimeToDateTime(ulSystemTime, &stStartTime);
		HxLOG_Debug("Current Update Time [%04d-%02d-%02d %02d:%02d:%02d] ulSystemTime[%x]\n", stStartTime.stDate.usYear, stStartTime.stDate.ucMonth, stStartTime.stDate.ucDay , stStartTime.stTime.ucHour, stStartTime.stTime.ucMinute, stStartTime.stTime.ucSecond, ulSystemTime);
		HLIB_DATETIME_ConvertUnixTimeToDateTime(ulUpdateTime, &stStartTime);
		HxLOG_Debug("Last Update Time [%04d-%02d-%02d %02d:%02d:%02d] ulUpdateTime[%x]\n", stStartTime.stDate.usYear, stStartTime.stDate.ucMonth, stStartTime.stDate.ucDay , stStartTime.stTime.ucHour, stStartTime.stTime.ucMinute, stStartTime.stTime.ucSecond, ulUpdateTime);
#endif
	}
	// LastUpdate Time 과 current time을 비교해 하루가 지나지 않으면 skip 한다.
	// service list 가 update 되면 lastupdate time을 초기화 하여 ipepg를 다시 받도록 한다.
	// Lastupdate time이 setting 없으면 ipepg를 다시 받고 lastupdate time을 setting 후 ipepg schedule을 다시 설정 한다.
	bIpEpgEnabled = (HBOOL)IPEPG_GetItemValue(eIPEPG_ITEM_FLAG_IPEPG_ONOFF);
	bConnected = (HBOOL)IPEPG_GetItemValue(eIPEPG_ITEM_NETWORK);
	if ((bIpEpgEnabled == TRUE)	&&(bConnected == FALSE))
	{
	}

	bServiceUpdated = (HBOOL)IPEPG_GetItemValue(eIPEPG_ITEM_FLAG_SVC_UPDATED);
	bAbleToUseHdd = (HBOOL)IPEPG_GetItemValue(eIPEPG_ITEM_HDD_STATUS);
	pMacAddress = (HUINT8 *)IPEPG_GetItemPoint(eIPEPG_ITEM_MAC);
	HxLOG_Debug("GET eIPEPG_ITEM_HDD_STATUS %s [%d]\n", bAbleToUseHdd?"TRUE":"FALSE", __LINE__);

	if(bConnected == TRUE && bIpEpgEnabled == TRUE && bAbleToUseHdd == TRUE
		&& bServiceUpdated == TRUE && pMacAddress != NULL)
	{
		err = ipepg_GetServerUriFromFile();		// RET OK or FAIL
	}

	IPEPG_MemFree(pMacAddress);
	IPEPG_FUNC_END();

	return err;
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

static HBOOL ipepg_IsDeleteReason(IPEPG_STOP_REASON_e eStopReason)
{
	HBOOL bRet = FALSE;

	/* HDD Format and HDD Off is only available when CONFIG_IPEPG_REPOSITORY_HDD is enabled. */
	switch(eStopReason)
	{
		case eIPEPG_STOPREASON_GETALL:
		case eIPEPG_STOPREASON_FACTORYRESET:
		case eIPEPG_STOPREASON_ONOFF:
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

	HxLOG_Warning("[eStopReason:%d] Delete all IP EPG DB files : %s\n", eStopReason, (bRet==TRUE) ? "True":"False");

	return bRet;
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

static HERROR ipepg_SetRsvForIpepg(HBOOL bRetryIPEPGDownloadSoon)
{
	UNIXTIME			utSystemTime = 0, utStartTime = 0;
	DxSchedule_t		stDxSched;
	HxDATETIME_t		stStartTime;
	HUINT8				ucHour = IPEPG_START_HOUR;
	HUINT8				ucMinute = IPEPG_START_MINUTE;
	HBOOL				bOverTime = FALSE;
	HUINT32				macTime = ipepg_GetRsvTimeFromMac();	// 0~210
	HERROR				hErr;

	IPEPG_FUNC_START();

	HxLOG_Debug("eIPEPG_MSG_DOWNLOAD_DONE Success Epg Download \n");

	utSystemTime = HLIB_STD_GetSystemTime();

	(void)ipepg_ClearRsvByType(DxRSVTYPE_IP_EPG_GATHERING);

	if ((HBOOL)IPEPG_GetItemValue(eIPEPG_ITEM_FLAG_IPEPG_ONOFF) == FALSE)
	{
		IPEPG_FUNC_END();
		return ERR_OK;
	}

	HLIB_DATETIME_ConvertUnixTimeToDateTime(utSystemTime, &stStartTime);

	HxLOG_Print("ipepg_SetRsvForIpepg Time[%d] Min[%d]\n", macTime/60, macTime%60);

	if(bRetryIPEPGDownloadSoon == TRUE)
	{
		HUINT32 macTime2ByMin = (macTime*90/210);			// MAC 주소에 따라서 0~90의 값
		macTime2ByMin = macTime2ByMin + 30;					// 30~120의 범위로 수정

		// Test
		//macTime2ByMin = macTime2ByMin /15;

		ucHour; ucMinute; bOverTime;
		utStartTime = utSystemTime + (macTime2ByMin *60);	// current time + offset

		// UI상 lastUpdate 시간은 보이지만 실패한 것임.
		HxLOG_Error("ipepg_SetRsvForIpepg Time[%d] Min[%d] Later. \n", macTime2ByMin/60, macTime2ByMin%60);
	}
	else
	{
#if defined(IPEPG_TEST_DOWNLOAD_REPEAT_TIME)
		ucHour; ucMinute; bOverTime;
		utStartTime = utSystemTime + IPEPG_TEST_DOWNLOAD_REPEAT_TIME;  // 20분뒤 다시 시작하도록 설정 후 test
#else
		if(macTime != 0)
		{
			ucHour = IPEPG_DEFAULT_START_HOUR + macTime / 60;
			ucMinute = macTime % 60;
		}

		if(stStartTime.stTime.ucHour * 60 + stStartTime.stTime.ucMinute > ucHour*60 + ucMinute)
		{
			bOverTime = TRUE;
		}

		stStartTime.stTime.ucHour = ucHour;
		stStartTime.stTime.ucMinute = ucMinute;
		HLIB_DATETIME_ConvertDateTimeToUnixTime(&stStartTime, &utStartTime);

		if(bOverTime == TRUE)
		{
			utStartTime += (60*60*24);
		}
#endif
	}

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
		HxLOG_Print("SET Current Update Time [%04d-%02d-%02d %02d:%02d:%02d] ulSystemTime[%x]\n", stTime.stDate.usYear, stTime.stDate.ucMonth, stTime.stDate.ucDay , stTime.stTime.ucHour, stTime.stTime.ucMinute, stTime.stTime.ucSecond, utSystemTime);

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
	IPEPG_TASK_MGR_t 	*pstTaskMgr = NULL;
	static HUINT32		ulRpProcessingCnt = 0;
	HBOOL				bRetryIPEPGDownloadSoon = FALSE;

	IPEPG_FUNC_START();

	// signal that mainProc is started
	VK_EVENT_Send((unsigned long)s_ulSyncCallMainProcEventId,(unsigned long)0);

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
			switch (stMsg.ulMsg)
			{
				case eIPEPG_MSG_START_ALL :
					// Initialize all data as well as upload svc lists and download ipepg.
					HxLOG_Warning("[eIPEPG_MSG_START_ALL : %d]\n", eIPEPG_MSG_START_ALL);

					IPEPG_SetItem(eIPEPG_ITEM_FLAG_SVC_UPDATED, (void *)FALSE);
					IPEPG_SetItem(eIPEPG_ITEM_UPDATE_TIME, (void *)0);
					IPEPG_SetItem(eIPEPG_ITEM_LAST_UPDATE_TIME, (void *)0);
					IPEPG_SetItem(eIPEPG_ITEM_STAMP_TIME, (void *)0);
					IPEPG_SetItem(eIPEPG_ITEM_DOWNLOADPROGRESS, (void *)0);
					IPEPG_SetItem(eIPEPG_ITEM_SVC_COUNT, (void *)0);
					IPEPG_SetItem(eIPEPG_ITEM_EVT_COUNT, (void *)0);

					if(ipepg_IsDeleteReason(eIPEPG_STOPREASON_GETALL))
					{
						IPEPG_DeleteAllData();
					}

					if ((pstTaskMgr->eStatus & eIPEPG_UPLOAD_START)|| (pstTaskMgr->eStatus & eIPEPG_DOWN_START))
					{
						IpEpgMsg_t	stNewMsg;
						HxSTD_MemSet(&stNewMsg, 0, sizeof(IpEpgMsg_t));
						stNewMsg.ulMsg = eIPEPG_MSG_STOP;
						stNewMsg.ulParam1 = eIPEPG_STOPREASON_GETALL;
						IPEPG_SendCommandWithParam(stNewMsg);
					}
					else
					{
						IPEPG_SendCommand(eIPEPG_MSG_START_UPDATE_SERVICELIST);
					}
					break;

				case eIPEPG_MSG_START_ONLY_SVCLIST :
					HxLOG_Warning("[eIPEPG_MSG_START_ONLY_SVCLIST : %d]\n", eIPEPG_MSG_START_ONLY_SVCLIST);

					HxLOG_Debug("[%d] eIPEPG_MSG_START_ONLY_SVCLIST\n", __LINE__);

					IPEPG_SetItem(eIPEPG_ITEM_FLAG_SVC_UPDATED, (void *)FALSE);
					if(ipepg_AbleToServiceListUpload(pstTaskMgr->pstData) == ERR_OK)
					{
						// Only upload svc list.
						pstTaskMgr->eStatus = pstTaskMgr->eStatus | eIPEPG_UPLOAD_START;
						IPEPG_UploadSvc_SendCommand(eIPEPG_MSG_START_ONLY_SVCLIST);
					}
					else
					{
						// Do not try to download ipepg because svclist's uid has not been changed.
					}

					break;

				case eIPEPG_MSG_START_UPDATE_SERVICELIST :
					HxLOG_Warning("[eIPEPG_MSG_START_UPDATE_SERVICELIST : %d]\n", eIPEPG_MSG_START_UPDATE_SERVICELIST);

					if(ipepg_AbleToServiceListUpload(pstTaskMgr->pstData) == ERR_OK)
					{
						// If svc lists has not been uploaded yet, upload them to server.
						pstTaskMgr->eStatus = pstTaskMgr->eStatus | eIPEPG_UPLOAD_START;
						IPEPG_UploadSvc_SendCommand(eIPEPG_MSG_START_UPDATE_SERVICELIST);
					}
					else
					{
						// If svc lists has been uploaded, try to download ipepg.
						HxLOG_Debug(" => [%d] command eIPEPG_MSG_START_DOWNLOAD\n", __LINE__);
						IPEPG_SendCommand(eIPEPG_MSG_START_DOWNLOAD);
					}
					break;

				case eIPEPG_MSG_START_DOWNLOAD :
					HxLOG_Warning("[eIPEPG_MSG_START_DOWNLOAD : %d]\n", eIPEPG_MSG_START_DOWNLOAD);

					if(ipepg_AbleToDownload(pstTaskMgr->pstData) == ERR_OK)
					{
						pstTaskMgr->eStatus = pstTaskMgr->eStatus | eIPEPG_DOWN_START;
						IPEPG_DownloadData_SendCommand(eIPEPG_MSG_START_DOWNLOAD);
					}
					else
					{
						IPEPG_SendCommand(eIPEPG_MSG_START_RSV_DOWNLOAD);
					}
					break;

				case eIPEPG_MSG_STOP :
					// In case of stopping request set eStatus with eIPEPG_STOP at eStatus of thread.
					HxLOG_Warning("[eIPEPG_MSG_STOP : %d]\n", eIPEPG_MSG_STOP);

					if(ipepg_IsDeleteReason(stMsg.ulParam1))
					{
						ipepg_ClearRsv();
						IPEPG_SetItem(eIPEPG_ITEM_FLAG_SVC_UPDATED,(void *)FALSE);
					}

					if ((pstTaskMgr->eStatus & eIPEPG_UPLOAD_START)||(pstTaskMgr->eStatus & eIPEPG_DOWN_START))
					{
						pstTaskMgr->eStatus = pstTaskMgr->eStatus | eIPEPG_STOP;
						pstTaskMgr->eStopReason = stMsg.ulParam1;
					}
					else
					{
						HxLOG_Debug(" => eIPEPG_MSG_STOP No Task\n");

						if(ipepg_IsDeleteReason(stMsg.ulParam1))
						{
							IPEPG_DeleteAllData();
						}
					}
					break;

				case eIPEPG_MSG_DONE_DOWNLOAD :
					// Upload -> Download -> Done (Thread End)
					// If the thread gets the event in terms of uploading svc lists and downloading ipepg, restart it.
					HxLOG_Warning("[eIPEPG_MSG_DONE_DOWNLOAD : %d]\n", eIPEPG_MSG_DONE_DOWNLOAD);

					HxLOG_Debug("[%d] eIPEPG_MSG_DOWNLOAD_DONE pstTaskMgr->eStatus[0x%08x] upload[%d] down[%d]\n", __LINE__, pstTaskMgr->eStatus, pstTaskMgr->ulUploadSvcTaskId, pstTaskMgr->ulEpgDownloadTaskId);
					if(stMsg.ulParam1 == eIPEPG_DOWN_START)
					{
						HxLOG_Debug(" => DOWNLOAD STOP\n");
						pstTaskMgr->eStatus = (~eIPEPG_DOWN_START) & (pstTaskMgr->eStatus);
						// Download Successfully
					}
					else if(stMsg.ulParam1 == eIPEPG_UPLOAD_START)
					{
						HxLOG_Debug(" => UPLOAD STOP\n");
						pstTaskMgr->eStatus = (~eIPEPG_UPLOAD_START) & (pstTaskMgr->eStatus);
					}

					if (!(pstTaskMgr->eStatus & (eIPEPG_DOWN_START|eIPEPG_UPLOAD_START)))
					{
						if((pstTaskMgr->eStatus & eIPEPG_STOP) == eIPEPG_STOP)
						{
							if(ipepg_IsDeleteReason(pstTaskMgr->eStopReason))
							{
								IPEPG_DeleteAllData();
							}

							pstTaskMgr->eStatus= pstTaskMgr->eStatus & (~eIPEPG_STOP);
							if(pstTaskMgr->eStopReason == eIPEPG_STOPREASON_GETALL)
							{
								ipepg_SetTimer(5);
							}

							pstTaskMgr->eStopReason = eIPEPG_STOPREASON_NOTHING;
						}
						else
						{
							if(stMsg.ulParam1 == eIPEPG_UPLOAD_START && stMsg.ulParam2 == ERR_OK)
							{
								HxLOG_Debug("  ==> Command Start Download\n");
								IPEPG_SendCommand(eIPEPG_MSG_START_DOWNLOAD);
							}
							else if(stMsg.ulParam1 == eIPEPG_DOWN_START /*&& stMsg.ulParam2 == ERR_OK*/)
							{
								// If downloading IPEPG XML Data is continuously failed with IPEPG_MAX_DOWNLOAD_RETRY_COUNT, retry it soon.
								if(stMsg.ulParam3 == ERR_FAIL)
								{
									bRetryIPEPGDownloadSoon = TRUE;
								}

								ipepg_SetRsvForIpepg(bRetryIPEPGDownloadSoon);
								HxLOG_Debug("  ==> Command Start RP\n");
								IPEPG_SendCommand(eIPEPG_MSG_START_RSV_DOWNLOAD);
							}
						}
					}
					HxLOG_Debug("eIPEPG_MSG_DOWNLOAD_DONE  End pstTaskMgr->eStatus[0x%08x] upload[%d] down[%d]\n", pstTaskMgr->eStatus, pstTaskMgr->ulUploadSvcTaskId, pstTaskMgr->ulEpgDownloadTaskId);
					break;

				case eIPEPG_MSG_REQUEST_SHUTDOWN_NOTI :
					HxLOG_Warning("[eIPEPG_MSG_REQUEST_SHUTDOWN_NOTI : %d]\n", eIPEPG_MSG_REQUEST_SHUTDOWN_NOTI);

					HxLOG_Debug("[%d] eIPEPG_MSG_REQUEST_SHUTDOWN_NOTI status[%d]\n", __LINE__, pstTaskMgr->eStatus);
					if((pstTaskMgr->eStatus & eIPEPG_UPLOAD_START) || (pstTaskMgr->eStatus & eIPEPG_DOWN_START)
						|| (pstTaskMgr->eStatus & eIPEPG_CHLOGO_DOWN_START) || ulRpProcessingCnt != 0)
					{
						HxLOG_Debug("[%d] eHSIG_WaitShutDown ulRpProcessingCnt[%d]\n", __LINE__, ulRpProcessingCnt);
						HxLOG_Debug("Current Status: 0x%x\n", pstTaskMgr->eStatus);
						APK_POWER_WaitShutDown(4000);
					}
					else
					{
						if(APK_POWER_IsStandbyMode() == TRUE)
						{
							HUINT32 ulSystemTime = 0;
							HUINT32 nextUpdateTime = IPEPG_GetItemValue(eIPEPG_ITEM_UPDATE_TIME);
							HUINT32 nextRpUpdateTime = IPEPG_GetItemValue(eIPEPG_ITEM_RP_UPDATE_TIME);
							(void)VK_CLOCK_GetTime((unsigned long*)&ulSystemTime);

							if((nextUpdateTime != 0 && (nextUpdateTime - 10 > ulSystemTime) && nextUpdateTime - 120 < ulSystemTime  )
								|| (nextRpUpdateTime != 0 && (nextRpUpdateTime - 10 > ulSystemTime) && nextRpUpdateTime < ulSystemTime + 120 )
								// Start Time 이 지난후에도 시작을 위한 buffer time 이나 그외의 이유로 download 및 upload process 가 동 작하지 않은 경우에 대하여
								// waiting 하도록 한다. 	2분 전
								|| gTimerTime != 0
								// timer 에 의 해 start 하기 위해 timer 가 걸려 있는 경우에 대하여 waiting 하게 한다.
								)
							{
								HxLOG_Debug("Clear Next UpdateTime[%d] systemTime[%d] gTimerTime[%d]\n", nextUpdateTime, ulSystemTime, gTimerTime);
								HxLOG_Debug("APK_POWER_WaitShutDown[%d]\n", __LINE__);
								APK_POWER_WaitShutDown(4000);
							}
						}
					}
					break;

				case eIPEPG_MSG_START_RSV_DOWNLOAD :
					HxLOG_Warning("[eIPEPG_MSG_START_RSV_DOWNLOAD : %d]\n", eIPEPG_MSG_START_RSV_DOWNLOAD);

					if(rp_AbleToDownload(pstTaskMgr->pstData) == ERR_OK)
					{
						HxLOG_Debug("eIPEPG_MSG_START_RSV_DOWNLOAD\n");
						ulRpProcessingCnt++;
						RP_SendCommand(eIPEPG_MSG_START_RSV_DOWNLOAD);
					}
					break;

				case eIPEPG_MSG_DONE_STB_UPLOAD :
					HxLOG_Warning("[eIPEPG_MSG_DONE_STB_UPLOAD : %d]\n", eIPEPG_MSG_DONE_STB_UPLOAD);

					ulRpProcessingCnt--;
					ipepg_SetRsvForRP();
					APK_POWER_WaitShutDown(4000);
					break;

				default :
					HxLOG_Debug("Critical Error Error [%s] [%d] msg[%d]\n", __FUNCTION__, __LINE__, stMsg.ulMsg);
					break;
			}
		}
	}

	IPEPG_FUNC_END();
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
			if(gTimerTime != 0)
			{
				HxLOG_Debug("TimerTask[%d][%d]\n", ulSystemTime, gTimerTime);
			}
			if(gbSetTimer == TRUE && gTimerTime == 0)	// 시간이 없는 경우 START 요청이 들어오면 시간이 들어온뒤 다시 SETTING 하도록 한다.
			{
				gbSetTimer = FALSE;
				gTimerTime = ulSystemTime + 5;	// 5초후 시작
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
	HUINT32				ulDummy;
	int					vkResult;

	IPEPG_FUNC_START();

	pstTaskMgr = ipepg_InitDownManager(pData);

	IPEPG_ChLogoCreateTask(pstTaskMgr);

	if(VK_MSG_Create(IPEPG_MSGQ_COUNT, sizeof(IpEpgMsg_t), "ipepg_msg", (unsigned long *)&(pstTaskMgr->ulMainMsgQid), VK_SUSPENDTYPE_FIFO) != 0)
	{
		HxLOG_Debug("[%s][%d] VK_MSG_Create fail \n", __FUNCTION__, __LINE__);
		IPEPG_FUNC_END();
		return;
	}

	(void)VK_EVENT_Create((unsigned long *)&s_ulSyncCallMainProcEventId, "waitMainEvent");

	VK_TASK_Create(ipepg_mainProc, VK_TASK_PRIORITY_MW_REF, SIZE_32K, (const char*)"ipepgMain", (void*)pstTaskMgr, (unsigned long *)&(pstTaskMgr->ulMainTaskId), 0);
	VK_TASK_Start(pstTaskMgr->ulMainTaskId);


	if(VK_MSG_Create(IPEPG_MSGQ_COUNT, sizeof(IpEpgMsg_t), "rsv_msg", (unsigned long *)&(pstTaskMgr->ulRsvMsgQid), VK_SUSPENDTYPE_FIFO) != 0)
	{
		HxLOG_Debug("[%s][%d] VK_MSG_Create fail \n", __FUNCTION__, __LINE__);
		IPEPG_FUNC_END();
		return;
	}

	VK_TASK_Create(Rp_Task, VK_TASK_PRIORITY_MW_REF, SIZE_32K, (const char*)"RsvTask", (void*)pstTaskMgr, (unsigned long *)&(pstTaskMgr->ulRsvTaskId), 0);
	VK_TASK_Start(pstTaskMgr->ulRsvTaskId);

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

	// ipepg_mainProc start를 대기 한다.
	vkResult = VK_EVENT_ReceiveTimeout((unsigned long)s_ulSyncCallMainProcEventId, (unsigned long *)&ulDummy, 1000);
	if (vkResult != VK_OK)
	{
		HxLOG_Error("VK_EVENT_ReceiveTimeout failed. \n");
	}

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
		IPEPG_SetItem(eIPEPG_ITEM_SVC_COUNT, (void *)0);
		IPEPG_SetItem(eIPEPG_ITEM_EVT_COUNT, (void *)0);

		HxLOG_Warning("[Different Version] Delete all IP EPG DB files.\n");
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

/*********************** End of File ******************************/

