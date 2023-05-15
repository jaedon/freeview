/**
	@file     ap_sat_search_svc.c
	@brief    file_name & simple comment.

	Description: Applicatuion for searching Services\n
	Module: Application(Layer), Menu(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2008/01/15		initiated					JK Baek\n
	 2008/03/14		renamed					wjmoh\n
	 2008/09/05		Satellite part is divided		jinh\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/


#include <db_param.h>
#include <db_svc.h>
#include <svc_si.h>
#include <vkernel.h>
#include <mgr_swup.h>
#include <linkedlist.h>


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/


#define 	OtaInfoAssert	HxLOG_Error


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef struct
{
	SWUP_DownloadType_e	 eOtaType;
	DbSvc_TsInfo_t		 stTsInfo;
	SvcSi_SwupSignal_t		 stOtaInfo;
} StandbyOtaInfo_t;


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

static POINTER_LIST_T		*s_pstApOtaInfo_HomeTpList = NULL;

static HBOOL				 s_bApOtaInfo_StandbyOta = FALSE;
static StandbyOtaInfo_t		 s_stApOtaInfo_StandbyOtaInfo;

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/

static void local_otainfo_FreeListItem (void *pvItem);
static HERROR local_otainfo_SetNextOtaTime (HUINT32 ulHour, HUINT32 ulMin, UNIXTIME *putNextTime);


static void local_otainfo_FreeListItem (void *pvItem)
{
	if (pvItem != NULL)
	{
		OxAP_Free (pvItem);
	}
}

static HERROR local_otainfo_SetNextOtaTime (HUINT32 ulHour, HUINT32 ulMin, UNIXTIME *putNextTime)
{
	UNIXTIME	 	utCurrTime, utNextTime;
	HxDATETIME_t	stDateTime;
	HERROR			hErr;

	if (putNextTime == NULL)				{ return ERR_BUS_TARGET_NULL; }

	hErr = VK_CLOCK_GetTime ((unsigned long *)&utCurrTime);
	if (hErr == ERR_OK)
	{
		HLIB_DATETIME_ConvertUnixTimeToDateTime (utCurrTime, &stDateTime);
		if ((stDateTime.stTime.ucHour > ulHour) || (stDateTime.stTime.ucHour == ulHour && stDateTime.stTime.ucMinute >= ulMin))
		{
			UNIXTIME	utTmpCurrTime;

			utTmpCurrTime = utCurrTime + SECONDS_PER_DAY;
			HLIB_DATETIME_ConvertUnixTimeToDateTime (utTmpCurrTime, &stDateTime);
		}

		stDateTime.stTime.ucHour	= (HUINT8)ulHour;
		stDateTime.stTime.ucMinute	= (HUINT8)ulMin;
		stDateTime.stTime.ucSecond	= 0;
		stDateTime.stTime.usMillisecond = 0;

		HLIB_DATETIME_ConvertDateTimeToUnixTime (&stDateTime, &utNextTime);

		// 이미 지난 시간이라면 그 다음 날로 한다.
		if (utNextTime <= utCurrTime)
		{
			stDateTime.stDate.ucDay++;
			HLIB_DATETIME_ConvertDateTimeToUnixTime (&stDateTime, &utNextTime);
		}
	}
	else
	{
		stDateTime.stDate.usYear	= 2001;
		stDateTime.stDate.ucMonth	= 1;
		stDateTime.stDate.ucDay 	= 1;
		stDateTime.stTime.ucHour	= ulHour;
		stDateTime.stTime.ucMinute	= ulMin;
		stDateTime.stTime.ucSecond	= 0;
		stDateTime.stTime.usMillisecond = 0;

		HLIB_DATETIME_ConvertDateTimeToUnixTime (&stDateTime, &utNextTime);
	}

	*putNextTime = utNextTime;
	return ERR_OK;
}


#define _____HOME_TP_____

HERROR MGR_SWUPINFO_ResetHomeTp (void)
{
	s_pstApOtaInfo_HomeTpList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (s_pstApOtaInfo_HomeTpList, (FREE_FUNC)local_otainfo_FreeListItem);
	return ERR_OK;
}

HERROR MGR_SWUPINFO_AddOtaHomeTp (DbSvc_TsInfo_t *pstTsInfo)
{
	DbSvc_TsInfo_t	*pstNewTsInfo;

	if (pstTsInfo == NULL)					{ return ERR_FAIL; }

	pstNewTsInfo = (DbSvc_TsInfo_t *)OxAP_Malloc (sizeof(DbSvc_TsInfo_t));
	if (pstNewTsInfo == NULL)				{ return ERR_FAIL; }

	*pstNewTsInfo = *pstTsInfo;

	s_pstApOtaInfo_HomeTpList = UTIL_LINKEDLIST_AppendListItemToPointerList (s_pstApOtaInfo_HomeTpList, (void *)pstNewTsInfo);
	return ERR_OK;
}

HUINT32 MGR_SWUPINFO_CountOtaHomeTp (void)
{
	return (HUINT32)UTIL_LINKEDLIST_GetNumListItemsInPointerList (s_pstApOtaInfo_HomeTpList, NULL, NULL);
}

HERROR MGR_SWUPINFO_GetOtaHomeTp (HUINT32 ulIndex, DbSvc_TsInfo_t *pstTsInfo)
{
	DbSvc_TsInfo_t		*pstInfo;
	POINTER_LIST_T		*pstItem;

	if (pstTsInfo == NULL)					{ return ERR_FAIL; }

	pstItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList (s_pstApOtaInfo_HomeTpList, (HUINT16)ulIndex, NULL, NULL);
	if (pstItem == NULL)					{ return ERR_FAIL; }

	pstInfo = (DbSvc_TsInfo_t *)pstItem->pvContents;
	if (pstInfo == NULL)					{ return ERR_FAIL; }

	*pstTsInfo = *pstInfo;
	return ERR_OK;
}


#define _____OTA_SEARCH_RESULT_____

HERROR MGR_SWUPINFO_ResetSearchedInfo (void)
{
	DB_PARAM_SetItem (eDB_PARAM_ITEM_OTA_FLAG, (HUINT32)FALSE, 0);
	DB_PARAM_Sync();
	DAPI_Sync(DxDBSETUP_SYSTEM);

	return ERR_OK;
}

HERROR MGR_SWUPINFO_SetSearchedInfo (DbSvc_TsInfo_t *pstTsInfo, HUINT32 ulOtaInfoLength, void *pvOtaInfo)
{
	HERROR	hErr;

	if (pstTsInfo == NULL)					{ return ERR_FAIL; }
	if (pvOtaInfo == NULL)					{ return ERR_FAIL; }
	if (ulOtaInfoLength == 0)				{ return ERR_FAIL; }

	hErr = DB_PARAM_SetItem (eDB_PARAM_ITEM_OTA_FLAG, (HUINT32)TRUE, 0);
	if(hErr != ERR_OK) 	{	HxLOG_Critical("\n");	}
	hErr= DB_PARAM_SetItem (eDB_PARAM_ITEM_OTA_TSINFO, (HUINT32)pstTsInfo, (HUINT32)sizeof(DbSvc_TsInfo_t));
	if(hErr != ERR_OK) 	{	HxLOG_Critical("\n");	}
	hErr = DB_PARAM_SetItem (eDB_PARAM_ITEM_OTA_OTAINFO, (HUINT32)pvOtaInfo, (HUINT32)ulOtaInfoLength);
	if(hErr != ERR_OK) 	{	HxLOG_Critical("\n");	}
	DB_PARAM_Sync();
	DAPI_Sync(DxDBSETUP_SYSTEM);

	return ERR_OK;
}

HERROR MGR_SWUPINFO_GetSearchedInfo(DbSvc_TsInfo_t *pstTsInfo, HUINT32 ulOtaInfoLength, void *pvOtaInfo)
{
	HBOOL			 bOtaFound;
	HERROR			 hErr;

	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_OTA_FLAG, (HUINT32 *)&bOtaFound, 0);
	if (hErr != ERR_OK || bOtaFound != TRUE)
	{
		HxLOG_Critical(" hErr(%d) ulValue(%d) \n", hErr, bOtaFound);
		return ERR_FAIL;
	}

	if (pstTsInfo != NULL)
	{
		hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_OTA_TSINFO, (HUINT32 *)pstTsInfo, (HUINT32)sizeof(DbSvc_TsInfo_t));
		if (hErr != ERR_OK)
		{
			HxLOG_Critical("\n\n");
			return ERR_FAIL;
		}
	}

	if (pvOtaInfo != NULL && ulOtaInfoLength > 0)
	{
		hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_OTA_OTAINFO, (HUINT32 *)pvOtaInfo, ulOtaInfoLength);
		if (hErr != ERR_OK)
		{
			HxLOG_Critical("\n\n");
			return ERR_FAIL;
		}
	}

	return ERR_OK;
}

HERROR		MGR_SWUPINFO_SetWakeUpOtaFlag(HINT32 nWakeUpOta)
{
	HERROR		hError = ERR_FAIL;

	hError = DB_PARAM_SetItem (eDB_PARAM_ITEM_OTA_WAKEUP_FLAG, (HUINT32)nWakeUpOta, (HUINT32)0);
	if (hError != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return	hError;
	}

	DB_PARAM_Sync();
	DAPI_Sync(DxDBSETUP_SYSTEM);

	return	hError;
}


HERROR	MGR_SWUPINFO_GetWakeUpOtaFlag(HINT32 *pnWakeUpOta)
{
	HERROR		hError = ERR_FAIL;
	HUINT32		ulValue = 0;

	if (pnWakeUpOta == NULL)		{	return ERR_FAIL;	}

	hError = DB_PARAM_GetItem (eDB_PARAM_ITEM_OTA_WAKEUP_FLAG, &ulValue, 0);
	if (hError != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return	hError;
	}

	*pnWakeUpOta = (HINT32)ulValue;

	return	hError;
}


HBOOL MGR_SWUPINFO_IsOtaFound (HBOOL bSkipUserCancelled)
{
	HUINT32			 ulValue;
	HUINT16			 usCanceledType;
	HUINT32			 ulCanceledVer;
	SvcSi_SwupSignal_t	 stOtaInfo;
	HERROR			 hErr;

	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_OTA_FLAG, &ulValue, 0);
	if (hErr != ERR_OK || ulValue != (HUINT32)TRUE)
	{
		return FALSE;
	}

	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_OTA_OTAINFO, (HUINT32 *)&stOtaInfo, sizeof(SvcSi_SwupSignal_t));
	if (hErr != ERR_OK)
	{
		return FALSE;
	}

	// User Selectable이 아니라면 Cancel과 상관없어야 한다.
	if (stOtaInfo.ucSwupType == SvcSi_SWUP_TYPE_FORCED)// || stOtaInfo.ucSwupType == SvcSi_SWUP_TYPE_CONDITIONAL)	//conditonal swup type 삭제.
	{
		return TRUE;
	}

	if (bSkipUserCancelled == TRUE)
	{
		hErr = MGR_SWUPINFO_GetOtaCanceledVersion (&usCanceledType, &ulCanceledVer);
		if (hErr == ERR_OK)
		{
			if (ulCanceledVer == stOtaInfo.ulDataVersion && usCanceledType == stOtaInfo.usDataType)
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

// Retry Scenario

HERROR	MGR_SWUPINFO_GetOtaRetryCount (HUINT16 *pusRetryOtaType, HUINT32 *pulRetryOtaVer, HUINT32 *pulRetryCount)
{
	HUINT32			 ulValue;
	HERROR			 hErr;

	if (pusRetryOtaType != NULL)
	{
		hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_OTA_RETRYTYPE, &ulValue, sizeof(HUINT32));
		if (hErr != ERR_OK)
		{
			return ERR_FAIL;
		}

		*pusRetryOtaType = (HUINT16)ulValue;
	}

	if (pulRetryOtaVer != NULL)
	{
		hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_OTA_RETRYVER, &ulValue, sizeof(HUINT32));
		if (hErr != ERR_OK)
		{
			return ERR_FAIL;
		}

		*pulRetryOtaVer = (HUINT32)ulValue;
	}

	if (pulRetryCount != NULL)
	{
		hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_OTA_RETRYCOUNT, &ulValue, sizeof(HUINT32));
		if (hErr != ERR_OK)
		{
			ulValue = 0;
		}

		*pulRetryCount = (HUINT32)ulValue;
	}

	return ERR_OK;
}

HERROR MGR_SWUPINFO_ResetOtaRetryCount (void)
{
	HERROR			 hErr;

	hErr = DB_PARAM_SetItem (eDB_PARAM_ITEM_OTA_RETRYCOUNT, (HUINT32)NULL, (HUINT32)0);
	if (hErr == ERR_OK)
	{
		DB_PARAM_Sync();
		DAPI_Sync(DxDBSETUP_SYSTEM);
	}

	return hErr;
}

HERROR	MGR_SWUPINFO_IncreaseOtaRetryCount (HUINT16 usRetryOtaType, HUINT32 ulRetryOtaVer)
{
	HINT32			 nOtaType, nOtaVer, nCount;
	HERROR			 hErr;

	hErr  = DB_PARAM_GetItem (eDB_PARAM_ITEM_OTA_RETRYTYPE, (HUINT32 *)&nOtaType, 0);
	hErr |= DB_PARAM_GetItem (eDB_PARAM_ITEM_OTA_RETRYVER, (HUINT32 *)&nOtaVer, 0);
	if (hErr != ERR_OK || (HINT32)usRetryOtaType != nOtaType || (HINT32)ulRetryOtaVer != nOtaVer)
	{
		DB_PARAM_SetItem (eDB_PARAM_ITEM_OTA_RETRYTYPE, (HUINT32)usRetryOtaType, (HUINT32)0);
		DB_PARAM_SetItem (eDB_PARAM_ITEM_OTA_RETRYVER, (HUINT32)ulRetryOtaVer, (HUINT32)0);
		DB_PARAM_SetItem (eDB_PARAM_ITEM_OTA_RETRYCOUNT, (HUINT32)1, (HUINT32)0);

		DB_PARAM_Sync();
		DAPI_Sync(DxDBSETUP_SYSTEM);

		return ERR_OK;
	}

	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_OTA_RETRYCOUNT, (HUINT32 *)&nCount, 0);
	if (hErr != ERR_OK)
	{
		nCount = 0;
	}

	nCount ++;

	DB_PARAM_SetItem (eDB_PARAM_ITEM_OTA_RETRYCOUNT, (HUINT32)nCount, 0);
	DB_PARAM_Sync();
	DAPI_Sync(DxDBSETUP_SYSTEM);

	return hErr;
}

// User Cancel Scenario

HERROR MGR_SWUPINFO_GetOtaCanceledVersion (HUINT16 *pusCanceledType, HUINT32 *pulCanceledVer)
{
	HUINT32			 ulValue;
	HERROR			 hErr;

	if (pusCanceledType != NULL)
	{
		hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_OTA_USERCANCELLED_TYPE, &ulValue, 0);
		if (hErr != ERR_OK)
		{
			return ERR_FAIL;
		}

		*pusCanceledType = (HUINT16)ulValue;
	}

	if (pusCanceledType != NULL)
	{
		hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_OTA_USERCANCELLED_VER, &ulValue, 0);
	if (hErr != ERR_OK)
	{
			return ERR_FAIL;
		}

		*pulCanceledVer = (HUINT32)ulValue;
	}

	return ERR_OK;

}

HERROR MGR_SWUPINFO_SetOtaCanceledVersion (HUINT16 usCanceledType, HUINT32 ulCanceledVer)
{
	HERROR		 hErr;

	hErr = DB_PARAM_SetItem (eDB_PARAM_ITEM_OTA_USERCANCELLED_TYPE, (HUINT32)usCanceledType, 0);
	if (hErr == ERR_OK)
	{
		DB_PARAM_Sync();
		DAPI_Sync(DxDBSETUP_SYSTEM);
	}

	hErr = DB_PARAM_SetItem (eDB_PARAM_ITEM_OTA_USERCANCELLED_VER, ulCanceledVer, 0);
	if (hErr == ERR_OK)
	{
		DB_PARAM_Sync();
		DAPI_Sync(DxDBSETUP_SYSTEM);
	}

	return ERR_OK;
}


#define _____DAILY_OTA_FUNCTION_____

HERROR MGR_SWUPINFO_GetNextOtaTime (HUINT32 ulHour, HUINT32 ulMin, UNIXTIME *putNextTime)
{
	return local_otainfo_SetNextOtaTime(ulHour, ulMin, putNextTime);
}

HERROR MGR_SWUPINFO_SetDailyOta (HUINT32 ulHour, HUINT32 ulMin)
{
	// should be implemented.
	return ERR_OK;
}


#define _____STANDBY_OTA_FUNCTION_____

HERROR MGR_SWUPINFO_ResetStandbyOtaInfo (void)
{
	s_bApOtaInfo_StandbyOta = FALSE;

	return ERR_OK;
}

HERROR MGR_SWUPINFO_GetStandbyOtaInfo (SWUP_DownloadType_e *peOtaType, DbSvc_TsInfo_t *pstTsInfo, SvcSi_SwupSignal_t *pstOtaInfo)
{
	if (s_bApOtaInfo_StandbyOta == FALSE)
	{
		return ERR_FAIL;
	}

	if (peOtaType != NULL)			{ *peOtaType  = s_stApOtaInfo_StandbyOtaInfo.eOtaType; }
	if (pstTsInfo != NULL)			{ *pstTsInfo  = s_stApOtaInfo_StandbyOtaInfo.stTsInfo; }
	if (pstOtaInfo != NULL)			{ *pstOtaInfo = s_stApOtaInfo_StandbyOtaInfo.stOtaInfo; }

	return ERR_OK;
}

HERROR MGR_SWUPINFO_SetStandbyOtaInfo (SWUP_DownloadType_e eOtaType, DbSvc_TsInfo_t *pstTsInfo, SvcSi_SwupSignal_t *pstOtaInfo)
{
	if (pstTsInfo == NULL || pstOtaInfo == NULL)
	{
		return ERR_FAIL;
	}

	s_bApOtaInfo_StandbyOta = TRUE;
	s_stApOtaInfo_StandbyOtaInfo.eOtaType  = eOtaType;
	s_stApOtaInfo_StandbyOtaInfo.stTsInfo  = *pstTsInfo;
	s_stApOtaInfo_StandbyOtaInfo.stOtaInfo = *pstOtaInfo;

	return ERR_OK;
}


#define	______OCTO2_OTA_SIGNAL____________________________________________________________

typedef struct
{
	HxVector_t	*infoList;
} OtaInfoManager_t;

static void *	local_otainfo_Mk_OTA_Signal (void *data)
{
	void *sig = OxAP_Malloc(sizeof(SvcSi_SwupSignal_t));
	if (sig)
	{
		memcpy(sig, data, sizeof(SvcSi_SwupSignal_t));
	}
	return sig;
}

static void *	local_otainfo_Rm_OTA_Signal (void *data)
{
	if (data)
	{
		OxAP_Free(data);
	}
	return NULL;
}

static OtaInfoManager_t *	local_otainfo_GetManager(void)
{
	static OtaInfoManager_t	s_mgr;

	if (s_mgr.infoList == NULL)
	{
		s_mgr.infoList = HLIB_VECTOR_NewEx(NULL, 10, local_otainfo_Mk_OTA_Signal, local_otainfo_Rm_OTA_Signal, NULL);
		HxLOG_Assert(s_mgr.infoList);
	}
	return &s_mgr;
}

HERROR	MGR_SWUPINFO_ResetSignal(void)
{
	OtaInfoManager_t	*mgr;

	HxLOG_Trace();

	mgr = local_otainfo_GetManager();

	HLIB_VECTOR_RemoveAll(mgr->infoList);

	return ERR_OK;
}

HINT32	MGR_SWUPINFO_AddSignal (const SvcSi_SwupSignal_t *pstOtaInfo)
{
	OtaInfoManager_t	*mgr;
	HINT32				n;

	HxLOG_Trace();
	HxLOG_Assert(pstOtaInfo);

	mgr = local_otainfo_GetManager();

	n = HLIB_VECTOR_Length(mgr->infoList);

	HLIB_VECTOR_Add(mgr->infoList, (void *)pstOtaInfo);

	return n;
}

HERROR	MGR_SWUPINFO_GetSignal (HINT32 nKey, SvcSi_SwupSignal_t *pstOtaInfo)
{
	OtaInfoManager_t	*mgr;
	SvcSi_SwupSignal_t		*data;

	HxLOG_Trace();
	HxLOG_Assert(pstOtaInfo);

	mgr  = local_otainfo_GetManager();

	data = (SvcSi_SwupSignal_t *)HLIB_VECTOR_ItemAt(mgr->infoList, nKey);
	if (data == NULL)
		return ERR_FAIL;

	memcpy(pstOtaInfo, data, sizeof(SvcSi_SwupSignal_t));
	return ERR_OK;
}

HERROR MGR_SWUP_GetNextBurstOtaSchedule(UNIXTIME *startTime)
{
	HERROR					hErr;
	HUINT8					i;
	SvcSi_SwupSchedule_t	stSchedule, stTempSchedule;
	
	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_OTA_SCHEDULEINFO, (HUINT32 *)&stSchedule, (HUINT32)sizeof(SvcSi_SwupSchedule_t));
	if(hErr == ERR_OK)
	{
		HxSTD_MemSet(&stTempSchedule, 0, sizeof(SvcSi_SwupSchedule_t));

		stTempSchedule.ulDataVersion = stSchedule.ulDataVersion;

		for(i = 0; i <SvcSi_MAX_SCHUDULE-1; i++)
		{
			stTempSchedule.startDateTime[i] = stSchedule.startDateTime[i+1];
			stTempSchedule.endDateTime[i] = stSchedule.endDateTime[i+1];
		}

		stTempSchedule.startDateTime[SvcSi_MAX_SCHUDULE-1] = stTempSchedule.endDateTime[SvcSi_MAX_SCHUDULE-1] = 0;
		HxSTD_memcpy(&stTempSchedule.stTripleId, &stSchedule.stTripleId, sizeof(DbSvc_TripleId_t));	

		if(startTime != NULL)
			*startTime = stTempSchedule.startDateTime[0];

		(void) DB_PARAM_SetItem (eDB_PARAM_ITEM_OTA_SCHEDULEINFO, (HUINT32)&stTempSchedule, (HUINT32)sizeof(SvcSi_SwupSchedule_t));
	}

	return hErr;
}

/* End of File. ---------------------------------------------------------- */


