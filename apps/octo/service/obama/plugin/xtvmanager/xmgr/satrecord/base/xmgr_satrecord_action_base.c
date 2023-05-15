/**
	@file     ap_sat_search_svc.c
		---> xmgr_sw_update_action_humax.c
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
#include <sapi.h>
#include <octo_common.h>
//#include <linkedlist.h>
#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_rscmap.h>
#include <mgr_live.h>
#include <mgr_satrecord.h>
#include <svc_download.h>

#include <_xmgr_satrecord_base.h>

#include <otl.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
//#define ASTRA_RECORDING_LINKAGE			0xA0
//#define HDPLUS_RECORDING_LINKAGE			0xA1

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef enum
{
	eSatRecState_Start = 0,
	eSatRecState_NonReceiverId,
	eSatRecState_Waiting,
	eSatRecState_Wait_MainTune,
	eSatRecState_BgTuning,
	eSatRecState_DownloadDsmcc,
	#if 0
	eSatRecState_HomeTp_Tuning,
	eSatRecState_HomeTp_WatingSi,
	eSatRecState_RefNitTp_Tuning,
	eSatRecState_RefNitTp_WatingSi,
	eSatRecState_SatRecTp_Tuning,
	eSatRecState_SatRecTp_WatingSi,

	eSatRecState_SearchDone,
	eSatRecState_SearchFail,

	eSatRecState_Finish,
	#endif
} SatRec_State_e;

typedef enum
{
	eSatRec_FileType_FastBooking,
	eSatRec_FileType_Booking,
	eSatRec_FileType_Max
}SatRec_FileType_e;

typedef struct
{
	HBOOL					bExistSched;
	DxSchedule_t			*pstSchedule;
} SatRec_ScheduleInfo;

typedef struct
{
	HUINT32			ulActionId;
	Handle_t		hAction;

	HBOOL			bActionTaken;
	SatRec_State_e	eState;

	HUINT32			ulReceiverID;

	HINT32			ulDsmccPid;
	DbSvc_TsInfo_t	stDetectTsInfo;
} xmgrSatrec_BaseActionContents_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
STATIC xmgrSatrec_BaseActionContents_t	s_stBase_Contents;


/*******************************************************************/
/********************      Functions         *************************/
/*******************************************************************/
#define _____Static_Function_____

STATIC xmgrSatrec_BaseActionContents_t *xmgr_satrecord_GetContents_Base (void)
{
	return &s_stBase_Contents;
}

static INLINE void xmgr_satrecord_IncreaseSessionVersion (xmgrSatrec_BaseActionContents_t *pstContents)
{
	pstContents->hAction = SVC_PIPE_IncreaseActionVersion (pstContents->ulActionId);
}

static INLINE HERROR xmgr_satrecord_CheckActionId (xmgrSatrec_BaseActionContents_t *pstContents, Handle_t hAction)
{
	HUINT32			 ulActionId;

	ulActionId = SVC_PIPE_GetActionId (hAction);
	if (pstContents->ulActionId != ulActionId)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}


#define	_____PROCESSING_FUNCTIONS_____

static HERROR xmgr_satrecord_ReleaseAction (xmgrSatrec_BaseActionContents_t *pstContents)
{
	HERROR		 hErr;

	if (pstContents->bActionTaken == TRUE)
	{
		hErr = SVC_CH_StopTune (pstContents->hAction, (HUINT16)-1, eSyncMode);

		// Release the action :
		MGR_ACTION_Release (pstContents->ulActionId, eActionType_BG_TUNE);
		pstContents->bActionTaken = FALSE;
	}

	return ERR_OK;
}

static HERROR xmgr_satrecord_StartSatRecDataDownload (xmgrSatrec_BaseActionContents_t *pstContents)
{
	HxLOG_Print("[%s:%d] dsmccPid = 0x%x\n", __FUNCTION__, __LINE__, pstContents->ulDsmccPid);
	return SVC_DOWNLOAD_Start(pstContents->hAction, DB_SVC_GetAllUniqueId(), eDOWN_DOWNLOAD_SATRECORD,  pstContents->ulDsmccPid, 0, 0, 0);
}

static HERROR xmgr_satrecord_ConvertRecDataToDxSchedule (DxSchedule_t *pstSchedule ,SvcDownload_SatRecBooking_t *pstRecData)
{
	DbSvc_Condition_t	cond;
	HINT32				svcuid[16];
	HINT32				count;
	DxOperator_e		eOperatorType;

	HUINT32 			ulCurrTime;
	HxDATETIME_t		stCurrTime, stTempTime;

	if(pstSchedule == NULL || pstRecData == NULL)
	{
		return ERR_FAIL;
	}

	HxSTD_MemSet(pstSchedule, 0, sizeof(DxSchedule_t));

	pstSchedule->eRsvType = DxRSVTYPE_RECORD_SATRECORD_TBR;

	DB_SVC_InitSvcFindCondition(&cond);

	cond.nOrgDeliType = (HINT32)eDxDELIVERY_TYPE_SAT;
	cond.nSvcId = (HINT32)pstRecData->usSvcId;
	cond.nTsId  = (HINT32)pstRecData->usTsId;
	cond.nOnId  = (HINT32)pstRecData->usOnId;
	if(DB_PARAM_GetItem(eDB_PARAM_ITEM_OPERATOR_TYPE, (HUINT32 *)&eOperatorType, 0) == ERR_OK)
	{
		cond.eSvcOpType = (HINT32)eOperatorType;//OTL_CONV_SysOperator2SvcOperator(eOperatorType);
	}

	count = DB_SVC_FindSvcUidList(&cond, svcuid, 16);
	if(count == 0)
	{
		HxLOG_Warning("err: cannot get svcuid in optype (%d)\n", eOperatorType);
		cond.eSvcOpType = (HINT32)eDxOPERATOR_None;//OTL_CONV_SysOperator2SvcOperator(eOperatorType);
		count = DB_SVC_FindSvcUidList(&cond, svcuid, 16);

		if(count == 0)
		{
			HxLOG_Error("err: cannot get svcuid\n");
			return ERR_FAIL;
		}
	}

	pstSchedule->ulSvcUid = svcuid[0];
	pstSchedule->stTripleId.usOnId = pstRecData->usOnId;
	pstSchedule->stTripleId.usTsId = pstRecData->usTsId;
	pstSchedule->stTripleId.usSvcId= pstRecData->usSvcId;

	pstSchedule->eRsvReady = DxRSVREADY_30_SEC;

	ulCurrTime = HLIB_STD_GetSystemTime();
	(void)HLIB_DATETIME_ConvertUnixTimeToDateTime(ulCurrTime, &stCurrTime);

	HxSTD_MemSet(&stTempTime, 0x0, sizeof(HxDATETIME_t));

	stTempTime.stDate.usYear = stCurrTime.stDate.usYear;
	stTempTime.stDate.ucMonth = pstRecData->ucMonth;
	stTempTime.stDate.ucDay = pstRecData->ucDay;

	stTempTime.stTime.ucHour = pstRecData->ucStartHour;
	stTempTime.stTime.ucMinute = pstRecData->ucStartMinute;

	if (HLIB_DATETIME_CompareDate(stCurrTime.stDate, stTempTime.stDate) > 0)
	{
		stTempTime.stDate.usYear += 1;
	}

	HxSTD_memcpy(&(pstSchedule->stStartTime), &stTempTime, sizeof(HxDATETIME_t));

	pstSchedule->ulDuration = (HUINT32)((pstRecData->ucDurHour * 60 + pstRecData->ucDurMinutes) * 60);

	pstSchedule->eRepeat = DxRSVREPEAT_ONCE;
	pstSchedule->eEpgType = eDxEPG_TYPE_NONE;
	pstSchedule->eScheduleSpec = eDxSchedule_Spec_Base;

//	pstSchedule->uExtInfo.stRec.aucStorageUUID

	return ERR_OK;

}


static BUS_Result_t xmgr_satrecord_checkSameSchInSchedList (DxSchedule_t *pstSchedule)
{
	DxSchedule_FindFilterAttr_t stFilter;
	HUINT32			ulNumSch = 0, i;

	DxSchedule_t	*pstSchArray = NULL;
	DxSchedule_t	tempSchedule;
	HBOOL			bFoundSameSch = FALSE;

	SAPI_FindScheduleFilterInit(&stFilter);
	stFilter.eRsvType = pstSchedule->eRsvType;
	HxSTD_memcpy(&(stFilter.stTripleId), &(pstSchedule->stTripleId), sizeof(DxSchedule_TripleId_t));

	SAPI_FindScheduleUsingFilter(&stFilter, &ulNumSch, &pstSchArray);

	for(i = 0; i < ulNumSch; i++)
	{
		HxSTD_memcpy(&tempSchedule, &(pstSchArray[i]), sizeof(DxSchedule_t));

		if(HLIB_DATETIME_CompareDate(tempSchedule.stStartTime.stDate, pstSchedule->stStartTime.stDate) == 0)
		{
			if((HLIB_DATETIME_CompareTime(tempSchedule.stStartTime.stTime, pstSchedule->stStartTime.stTime) == 0 )
				&& (tempSchedule.ulDuration == pstSchedule->ulDuration))
			{
				bFoundSameSch = TRUE;
				break;
			}
		}
	}

	SAPI_FindScheduleUsingFilterFreeList(pstSchArray);

	if(bFoundSameSch)
	{
		HxLOG_Error("Schedule already exist\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC void xmgr_satrecord_OnDamaCanceledSchedReceived (const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata)
{
	HBOOL				bFoundSameSch = FALSE;

	DAxIterator_t		stIterator;
	DxSchedule_t		*pstSrcSch;
	DxSchedule_t		stSrcSched;
	SatRec_ScheduleInfo *pstSchedInfo = (SatRec_ScheduleInfo *)userdata;


	HxSTD_memcpy(&stSrcSched, pstSchedInfo->pstSchedule, sizeof(DxSchedule_t));

	DAPI_InitIterator(&stIterator, data, size, 0);

	while (DAPI_NextIterator(&stIterator))
	{
		pstSrcSch = (DxSchedule_t *)stIterator.data;

		if(stSrcSched.stTripleId.usOnId == pstSrcSch->stTripleId.usOnId
			&& stSrcSched.stTripleId.usTsId == pstSrcSch->stTripleId.usTsId
			&& stSrcSched.stTripleId.usSvcId== pstSrcSch->stTripleId.usSvcId)
		{
			bFoundSameSch = TRUE;
			break;
		}
	}

	pstSchedInfo->bExistSched = bFoundSameSch;

	return;
}

static BUS_Result_t xmgr_satrecord_MakeSchedule (DxSchedule_t *pstSchedule)
{
	HERROR			hErr;
	HUINT32 		ulSlot = pstSchedule->ulSlot;
	SAPI_Conflict_t	stConflict;

	HxLOG_Debug("Available Slot = %d\n", ulSlot);

	hErr = SAPI_MakeSchedule(ulSlot, pstSchedule, &stConflict);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("SAPI_MakeSchedule err: ulSlot(%d)", ulSlot);
		return ERR_FAIL;
	}

	return ERR_OK;
}

static BUS_Result_t xmgr_satrecord_AddSchedule(SvcDownload_SatRecData_t *pstBookingData)
{
	HERROR			hErr;
	DxSchedule_t	stSchedule;
	HINT32			ulSlot;

	HCHAR			query[256] = {0,};
	UNIXTIME	unixTime;
	HUINT32		rId;
	SatRec_ScheduleInfo	stSchedInfo;


	if (NULL == pstBookingData)					{ return ERR_FAIL; }

	hErr = xmgr_satrecord_ConvertRecDataToDxSchedule(&stSchedule, &(pstBookingData->stBookingData));
	if (ERR_OK != hErr)
	{
		HxLOG_Error("err: cannot convert schedule data\n");
		return ERR_FAIL;
	}

	hErr = SAPI_GetAvailableSlot(&ulSlot);

	if (ERR_OK != hErr)
	{
		HxLOG_Error("SAPI_GetAvailableSlot err:\n");
		return ERR_FAIL;
	}

	stSchedule.ulSlot = ulSlot;

	if (ERR_OK != hErr)
	{
		HxLOG_Error("err: cannot convert schedule data\n");
		return ERR_FAIL;
	}

	HLIB_DATETIME_ConvertDateTimeToUnixTime(&stSchedule.stStartTime, &unixTime);
	stSchedInfo.pstSchedule = &stSchedule;
	stSchedInfo.bExistSched = FALSE;


	HxSTD_PrintToStrN(query, 256
		, "SELECT * FROM DxDB_SatRecord WHERE starttime = %d AND duration = %d"
		, unixTime
		, stSchedule.ulDuration
	);

	rId = DAPI_Query(query, (DAPI_Getter)xmgr_satrecord_OnDamaCanceledSchedReceived, (void *)&stSchedInfo);
	DAPI_Wait(1000, rId);

	if(stSchedInfo.bExistSched)
	{
		HxLOG_Error("Schedule already exist\n");
		return ERR_FAIL;
	}

	hErr = xmgr_satrecord_checkSameSchInSchedList(&stSchedule);
	if(hErr == ERR_OK)
	{
		hErr = xmgr_satrecord_MakeSchedule(&stSchedule);
	}

	return hErr;
}


static HERROR xmgr_satrecord_CancelSchedule_Ses (SvcDownload_SatRecData_t *pstBookingData)
{
	HERROR			hErr;
	HUINT32			ulNumSch = 0, i, ulSlot;
	DxSchedule_t	*pstSchArray = NULL;
	DxSchedule_t	stSchedule, tempSchedule;
	HBOOL			bFoundSch = FALSE;

	DxSchedule_FindFilterAttr_t stFilter;

	//1. get uid from db by time & triple id
	if (NULL == pstBookingData)					{ return ERR_FAIL; }

	hErr = xmgr_satrecord_ConvertRecDataToDxSchedule(&stSchedule, &(pstBookingData->stBookingData));
	if (ERR_OK != hErr)
	{
		HxLOG_Error("err: cannot convert schedule data\n");
		return ERR_FAIL;
	}

	SAPI_FindScheduleFilterInit(&stFilter);
	stFilter.eRsvType = stSchedule.eRsvType;
	HxSTD_memcpy(&(stFilter.stTripleId), &(stSchedule.stTripleId), sizeof(DxSchedule_TripleId_t));

	SAPI_FindScheduleUsingFilter(&stFilter, &ulNumSch, &pstSchArray);

	for(i = 0; i < ulNumSch; i++)
	{
		HxSTD_memcpy(&tempSchedule, &(pstSchArray[i]), sizeof(DxSchedule_t));

		if(HLIB_DATETIME_CompareDate(tempSchedule.stStartTime.stDate, stSchedule.stStartTime.stDate) == 0)
		{
			if((HLIB_DATETIME_CompareTime(tempSchedule.stStartTime.stTime, stSchedule.stStartTime.stTime) == 0 )
				&& (tempSchedule.ulDuration == stSchedule.ulDuration))
			{
				bFoundSch = TRUE;
				ulSlot = tempSchedule.ulSlot;
				break;
			}
		}
	}

	if(bFoundSch)
	{
		SAPI_CancelSchedule(ulSlot);
	}

	SAPI_FindScheduleUsingFilterFreeList(pstSchArray);

	return ERR_OK;
}

static BUS_Result_t xmgr_satrecord_ProcessBookingData (xmgrSatrec_BaseActionContents_t *pstContents)
{
	HERROR	hErr;
	HUINT32	nDataNum, i;
	SvcDownload_SatRecData_t	*pstBookingData;

	hErr = SVC_DOWNLOAD_GetData(pstContents->hAction,eDOWN_DATA_SATRECORD_BOOKINGDATA, &nDataNum, (void **)&pstBookingData);

	if(hErr == ERR_OK && nDataNum > 0)
	{
		for(i = 0; i < nDataNum; i++)
		{
			if(pstBookingData[i].bRecordCancelFlag == TRUE)
			{
				if(xmgr_satrecord_AddSchedule(&pstBookingData[i]) != ERR_OK)
				{
					HxLOG_Warning("FAIL : ADD schedule - %d/%d, %d:%d (%dh %dm)\n",
						pstBookingData[i].stBookingData.ucMonth, pstBookingData[i].stBookingData.ucDay,
						pstBookingData[i].stBookingData.ucStartHour,pstBookingData[i].stBookingData.ucStartMinute,
						pstBookingData[i].stBookingData.ucDurHour,pstBookingData[i].stBookingData.ucDurMinutes);
				}
			}
			else
			{
				if(xmgr_satrecord_CancelSchedule_Ses(&pstBookingData[i]) != ERR_OK)
				{
					HxLOG_Warning("FAIL : CANCEL schedule - %d/%d, %d:%d (%dh %dm)\n",
						pstBookingData[i].stBookingData.ucMonth, pstBookingData[i].stBookingData.ucDay,
						pstBookingData[i].stBookingData.ucStartHour,pstBookingData[i].stBookingData.ucStartMinute,
						pstBookingData[i].stBookingData.ucDurHour,pstBookingData[i].stBookingData.ucDurMinutes);
				}
			}
		}
	}

	SVC_DOWNLOAD_FreeData(pstContents->hAction,eDOWN_DATA_SATRECORD_BOOKINGDATA, nDataNum,(void *)pstBookingData);

	return hErr;
}

static HERROR xmgr_satrecord_ProcessFastBookingData (xmgrSatrec_BaseActionContents_t *pstContents)
{
	HERROR	hErr;
	HUINT32	nDataNum, i;
	SvcDownload_SatRecData_t	*pstFastBookingData;

	hErr = SVC_DOWNLOAD_GetData(pstContents->hAction,eDOWN_DATA_SATRECORD_FASTBOOKINGDATA, &nDataNum, (void **)&pstFastBookingData);

	if(hErr == ERR_OK && nDataNum > 0)
	{
		for(i = 0; i < nDataNum; i++)
		{
			HxLOG_Print("%d/%d, %d:%d (%dh %dm)\n", pstFastBookingData[i].stBookingData.ucMonth, pstFastBookingData[i].stBookingData.ucDay, pstFastBookingData[i].stBookingData.ucStartHour,
				pstFastBookingData[i].stBookingData.ucStartMinute, pstFastBookingData[i].stBookingData.ucDurHour,pstFastBookingData[i].stBookingData.ucDurMinutes);
			if(pstFastBookingData[i].bRecordCancelFlag == TRUE)
			{
				if(xmgr_satrecord_AddSchedule(&pstFastBookingData[i]) != ERR_OK)
				{
					HxLOG_Warning("FAIL : ADD schedule - %d/%d, %d:%d (%dh %dm)\n",
						pstFastBookingData[i].stBookingData.ucMonth, pstFastBookingData[i].stBookingData.ucDay,
						pstFastBookingData[i].stBookingData.ucStartHour,pstFastBookingData[i].stBookingData.ucStartMinute,
						pstFastBookingData[i].stBookingData.ucDurHour,pstFastBookingData[i].stBookingData.ucDurMinutes);
				}
			}
			else
			{
				if(xmgr_satrecord_CancelSchedule_Ses(&pstFastBookingData[i]) != ERR_OK)
				{
					HxLOG_Warning("FAIL : CANCEL schedule - %d/%d, %d:%d (%dh %dm)\n",
						pstFastBookingData[i].stBookingData.ucMonth, pstFastBookingData[i].stBookingData.ucDay,
						pstFastBookingData[i].stBookingData.ucStartHour,pstFastBookingData[i].stBookingData.ucStartMinute,
						pstFastBookingData[i].stBookingData.ucDurHour,pstFastBookingData[i].stBookingData.ucDurMinutes);
				}
			}
		}
	}

	SVC_DOWNLOAD_FreeData(pstContents->hAction,eDOWN_DATA_SATRECORD_FASTBOOKINGDATA, nDataNum,(void *)pstFastBookingData);

	return hErr;
}

static HUINT32 xmgr_satrecord_getWakeupInterval(void)
{
	HERROR hErr;
	HUINT32 nUpdateInterval;
	HUINT32 nPollingPeriod;

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_SATREC_UPDATE_INTERVAL, (HUINT32 *)&nUpdateInterval, 0);

	if(hErr != ERR_OK)
	{
		HxLOG_Error(": Fail to get DB Value for update interval\n");
		return 0;
	}

	nPollingPeriod = nUpdateInterval & 0x0FFFFFFF;

	return nPollingPeriod;
}

static HERROR xmgr_satrecord_setAutoPollingPeriod(xmgrSatrec_BaseActionContents_t *pstContents, HBOOL bVersionUp)
{
	HERROR hErr;
	HUINT32 nDataNum;
	HUINT32 nUpdateInterval;
	HUINT16 nPollingPeriod, nTempPolling;
	HUINT16 *pstPollingPeriod;
	DxSatRecIntervalType_e	eIntervalType;

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_SATREC_UPDATE_INTERVAL, (HUINT32 *)&nUpdateInterval, 0);

	if(hErr != ERR_OK)
	{
		HxLOG_Error(": Fail to get DB Value for update interval\n");
		return ERR_FAIL;
	}

	eIntervalType = nUpdateInterval & 0xF0000000;

	if(eIntervalType != eDxSATREC_INTERVAL_AUTO)
	{
		HxLOG_Print("[%s:%d] : update type is not AUTO\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	nPollingPeriod = (HUINT16)(nUpdateInterval - eIntervalType);

	if(bVersionUp == FALSE && nPollingPeriod != 0)
	{
		HxLOG_Print("[%s:%d] : update time is already set.\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	hErr = SVC_DOWNLOAD_GetData(pstContents->hAction,eDOWN_DATA_SATRECORD_POLLINGPERIOD, &nDataNum,(void **)&pstPollingPeriod);

	if(hErr == ERR_OK)
	{
		nTempPolling = *pstPollingPeriod;

		if(nPollingPeriod != nTempPolling)
		{
			nPollingPeriod = nTempPolling;
			nUpdateInterval = eIntervalType + (HUINT32)nPollingPeriod;

			DB_PARAM_SetItem(eDB_PARAM_ITEM_SATREC_UPDATE_INTERVAL, nUpdateInterval, 0);
		}
	}

	SVC_DOWNLOAD_FreeData(pstContents->hAction,eDOWN_DATA_SATRECORD_POLLINGPERIOD, nDataNum,(void *)pstPollingPeriod);

	return hErr;
}


#define	_____INTERNAL_MSG_PROCESS_FUNCTION_____

static BUS_Result_t xmgr_satrecord_MsgBusCreate_Base (xmgrSatrec_BaseActionContents_t *pstContents)
{
	HERROR	hErr;

	xmgrSatrec_DetectInfo_t		stDetectInfo;

	if (pstContents == NULL)
	{
		HxLOG_Error ("Context NULL!!!\n");
		return MESSAGE_PASS;
	}

	HxSTD_MemSet(pstContents, 0, sizeof(xmgrSatrec_BaseActionContents_t));

	// Get Detect Info
	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_SATREC_DETECTINFO, (HUINT32*)&stDetectInfo,  sizeof(xmgrSatrec_DetectInfo_t));
	if(hErr != ERR_OK || stDetectInfo.nDsmccPid == -1)
	{
		HxLOG_Error ("Fail Get Detect Info!!!\n");
		BUS_MGR_Destroy(NULL);
		return MESSAGE_BREAK;
	}

	pstContents->ulActionId = MGR_ACTION_GetSubActionId ();
	pstContents->hAction = SVC_PIPE_GetActionHandle(pstContents->ulActionId);

	pstContents->ulDsmccPid = stDetectInfo.nDsmccPid;
	HxSTD_memcpy(&pstContents->stDetectTsInfo, &stDetectInfo.stDetectTsInfo, sizeof(DbSvc_TsInfo_t));

	HxLOG_Print("[%s:%d] SatRecord dsmcc pidid (0x%x) ulActionId(%d)\n", __FUNCTION__, __LINE__, pstContents->ulDsmccPid, pstContents->ulActionId);

	return MESSAGE_BREAK;
}

static BUS_Result_t xmgr_satrecord_MsgBusDestroy_Base (xmgrSatrec_BaseActionContents_t *pstContents)
{
	if(pstContents->eState == eSatRecState_DownloadDsmcc)
	{
		(void)SVC_DOWNLOAD_Stop(pstContents->hAction, eSyncMode);
	}

	xmgr_satrecord_ReleaseAction(pstContents);

	// Destroy the contents
//	xmgr_satrecord_detect_DestroyContents (pstContents);

	return MESSAGE_BREAK;
}

static BUS_Result_t xmgr_satrecord_EvtActionStart_Base (xmgrSatrec_BaseActionContents_t *pstContents)
{
	HERROR			hErr;
	HUINT32			ulReceiverID = 0;

	SvcRsc_Info_t	stRscInfo;
	DxTuneParam_t	stChTuneInfo;

	pstContents->eState = eSatRecState_Start;

	//1. Get Reciever ID
	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_SATREC_RECEIVERID, &ulReceiverID, 0);
	if(hErr != ERR_OK || ulReceiverID == 0)
	{
		HxLOG_Error ("Fail Get Reciever ID!!!\n");
		pstContents->eState = eSatRecState_NonReceiverId;
		return MESSAGE_BREAK;
	}

	pstContents->ulReceiverID = ulReceiverID;

	xmgr_satrecord_IncreaseSessionVersion (pstContents);

	HxLOG_Print("[%s:%d] Frequency : %d, symborate : %d\n", __FUNCTION__, __LINE__, pstContents->stDetectTsInfo.stTuningParam.sat.ulFrequency,
		pstContents->stDetectTsInfo.stTuningParam.sat.ulSymbolRate);

	//2. Take the Action
	OTL_CONV_DbTuningInfo2DxTuneParam((const DbSvc_TsInfo_t *)&(pstContents->stDetectTsInfo), &stChTuneInfo);

	hErr = MGR_ACTION_SetupSearch (pstContents->ulActionId, &stChTuneInfo, eAmPriority_BGJOB);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("FAIL : setup Search!\n");
		goto FUNC_ERROR;
	}

	hErr = MGR_ACTION_Take (pstContents->ulActionId, eActionType_BG_TUNE, BUS_MGR_GetMgrCallback(BUS_MGR_GetThis()));
	if (hErr != ERR_OK)
	{
		HxLOG_Error("FAIL : take action!\n");
		goto FUNC_ERROR;
	}

	pstContents->bActionTaken = TRUE;
	hErr = MGR_RSC_GetResourceInfo (pstContents->ulActionId, &stRscInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("FAIL : get resource info!\n");
		xmgr_satrecord_ReleaseAction(pstContents);
		goto FUNC_ERROR;
	}

	hErr = SVC_CH_StartTune (pstContents->hAction, &stRscInfo, (HUINT16)-1, &stChTuneInfo);
	if (hErr == ERR_OK)
	{
		pstContents->eState = eSatRecState_BgTuning;
		return MESSAGE_BREAK;
	}

	FUNC_ERROR:
		pstContents->eState = eSatRecState_Waiting;

	return MESSAGE_BREAK;
}

static BUS_Result_t xmgr_satrecord_EvtChLocked_Base (xmgrSatrec_BaseActionContents_t *pstContents, Handle_t hAction)
{
	HERROR	hErr;

	if(pstContents->eState == eSatRecState_Wait_MainTune)
	{
		BUS_SendMessage(MGR_SATRECORD_GetDownloadAction(), eMEVT_SATRECORD_ACTION_START, HANDLE_NULL, 0, 0, 0);

		return ERR_BUS_NO_ERROR;
	}
	else if (pstContents->eState == eSatRecState_BgTuning)
	{
		hErr = xmgr_satrecord_CheckActionId (pstContents, hAction);
		if (hErr != ERR_OK)
		{
			return MESSAGE_PASS;
		}

		hErr = xmgr_satrecord_StartSatRecDataDownload(pstContents);

		if(hErr == ERR_OK)
		{
			pstContents->eState = eSatRecState_DownloadDsmcc;
		}
		else
		{
			xmgr_satrecord_ReleaseAction(pstContents);
			pstContents->eState = eSatRecState_Waiting;
		}
	}
	else
	{
		return ERR_BUS_NO_ERROR;
	}

	return MESSAGE_BREAK;
}

static BUS_Result_t xmgr_satrecord_EvtChNoSignal_Base(xmgrSatrec_BaseActionContents_t *pstContents, Handle_t hAction)
{
	HERROR	hErr;

	hErr = xmgr_satrecord_CheckActionId (pstContents, hAction);
	if (hErr != ERR_OK)
	{
		return MESSAGE_PASS;
	}

	if(pstContents->eState != eSatRecState_BgTuning || pstContents->eState != eSatRecState_DownloadDsmcc)
	{
		return MESSAGE_PASS;
	}

	if(pstContents->eState == eSatRecState_DownloadDsmcc)
	{
		(void)SVC_DOWNLOAD_Stop(pstContents->hAction, eSyncMode);
	}

	(void)xmgr_satrecord_ReleaseAction(pstContents);
	pstContents->eState = eSatRecState_Waiting;

	return MESSAGE_BREAK;
}

static BUS_Result_t xmgr_satrecord_EvtSvcChange_Base (xmgrSatrec_BaseActionContents_t *pstContents, Handle_t hAction)
{
	if(pstContents->eState == eSatRecState_Waiting)
	{
		pstContents->eState= eSatRecState_Wait_MainTune;
	}

	return MESSAGE_PASS;
}

static BUS_Result_t xmgr_satrecord_EvtDnldChanged_Base (xmgrSatrec_BaseActionContents_t *pstContents, Handle_t hAction)
{
	HERROR	hErr;

	if(pstContents->eState != eSatRecState_DownloadDsmcc)
	{
		return MESSAGE_PASS;
	}

	hErr = xmgr_satrecord_CheckActionId (pstContents, hAction);
	if (hErr != ERR_OK)
	{
		return MESSAGE_PASS;
	}

	hErr = SVC_DOWNLOAD_Stop(pstContents->hAction, eSyncMode);
	if(hErr == ERR_OK)
	{
		hErr = xmgr_satrecord_StartSatRecDataDownload(pstContents);

		if(hErr == ERR_OK)
		{
			return MESSAGE_BREAK;
		}

		xmgr_satrecord_ReleaseAction(pstContents);
		pstContents->eState = eSatRecState_Waiting;
	}

	return MESSAGE_BREAK;
}

// eSEVT_DOWN_DOWNLOAD_SUCCESS:
static BUS_Result_t xmgr_satrecord_EvtDownloadSuccess_Base (xmgrSatrec_BaseActionContents_t *pstContents, Handle_t hAction)
{
	HERROR	hErr;
	HUINT8	*pstSatRecVersion;
	HUINT32	nDataNum;
	HUINT32	ulOldVersion, ulNewVersion;

	if(pstContents->eState != eSatRecState_DownloadDsmcc)
	{
		return MESSAGE_PASS;
	}

	hErr = SVC_DOWNLOAD_GetData(pstContents->hAction,eDOWN_DATA_SATRECORD_VERSION, &nDataNum,(void **)&pstSatRecVersion);

	if (hErr != ERR_OK || nDataNum == 0)
	{
		return ERR_FAIL;
	}

	HxLOG_Print("[%s:%d] Fast BOOKING Data Version : 0x%x, Booking Data Version : 0x%x\n", __FUNCTION__, __LINE__,
		pstSatRecVersion[eSatRec_FileType_FastBooking], pstSatRecVersion[eSatRec_FileType_Booking]);

	ulNewVersion = pstSatRecVersion[eSatRec_FileType_Booking];
	if(ulNewVersion != 0xFF)
	{
		hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_SATREC_BDVERSION, (HUINT32 *)&ulOldVersion, 0);

		HxLOG_Print("[%s:%d] BOOKING Data new Version : 0x%x, old Version : 0x%x\n", __FUNCTION__, __LINE__,ulOldVersion, ulNewVersion);
		if(hErr == ERR_OK && ulOldVersion != ulNewVersion)
		{
			hErr = xmgr_satrecord_ProcessBookingData(pstContents);
			DB_PARAM_SetItem(eDB_PARAM_ITEM_SATREC_BDVERSION, (HUINT32)ulNewVersion, 0);

			xmgr_satrecord_setAutoPollingPeriod(pstContents, TRUE);
		}
		else
		{
			xmgr_satrecord_setAutoPollingPeriod(pstContents, FALSE);
		}
	}
	else
	{
		HxLOG_Error(": Can't Receive Booking Data\n");
	}

	ulNewVersion = pstSatRecVersion[eSatRec_FileType_FastBooking];
	if(ulNewVersion!= 0xFF)
	{
		hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_SATREC_FBDVERSION, (HUINT32 *)&ulOldVersion, 0);

		HxLOG_Print("[%s:%d] Fast BOOKING Data new Version : 0x%x, old Version : 0x%x\n", __FUNCTION__, __LINE__,ulOldVersion, ulNewVersion);
		if(hErr == ERR_OK && ulOldVersion != ulNewVersion)
		{
			hErr = xmgr_satrecord_ProcessFastBookingData(pstContents);
			DB_PARAM_SetItem(eDB_PARAM_ITEM_SATREC_FBDVERSION, (HUINT32)ulNewVersion, 0);
		}
	}
	else
	{
		HxLOG_Error(": Can't Receive Fast Booking Data\n");
	}

	SVC_DOWNLOAD_FreeData(pstContents->hAction,eDOWN_DATA_SATRECORD_VERSION, nDataNum,(void *)pstSatRecVersion);

	return ERR_OK;

}

static BUS_Result_t xmgr_satrecord_EvtDownloadFail_Base (xmgrSatrec_BaseActionContents_t *pstContents, Handle_t hAction)
{
	HERROR	hErr;

	if(pstContents->eState != eSatRecState_DownloadDsmcc)
	{
		return MESSAGE_PASS;
	}

	hErr = xmgr_satrecord_CheckActionId (pstContents, hAction);
	if (hErr != ERR_OK)
	{
		return MESSAGE_PASS;
	}

	hErr = SVC_DOWNLOAD_Stop(pstContents->hAction, eSyncMode);

	xmgr_satrecord_ReleaseAction(pstContents);
	pstContents->eState = eSatRecState_Waiting;

	return MESSAGE_BREAK;
}


static BUS_Result_t xmgr_satrecord_EvtSetReceiverID_Base (xmgrSatrec_BaseActionContents_t *pstContents)
{
	if(pstContents->eState == eSatRecState_NonReceiverId)
	{
		BUS_SendMessage(MGR_SATRECORD_GetDownloadAction(), eMEVT_SATRECORD_ACTION_START, HANDLE_NULL, 0, 0, 0);
	}

	return MESSAGE_BREAK;
}

static BUS_Result_t xmgr_satrecord_EvtSetUpdateInterval_Base(xmgrSatrec_BaseActionContents_t *pstContents)
{
	xmgr_satrecord_setAutoPollingPeriod(pstContents, FALSE);

	return MESSAGE_BREAK;
}

BUS_Result_t xproc_satrecord_proc_Base(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t	eResult = MESSAGE_PASS;

	xmgrSatrec_BaseActionContents_t 	*pstContents = xmgr_satrecord_GetContents_Base();

	switch (message)
	{
		case eMEVT_BUS_CREATE:
			HxLOG_Print ("[%s:%d] eMEVT_BUS_CREATE (0x%08x:%d,%d,%d)\n", __FUNCTION__, __LINE__, hAction, p1, p2, p3);
			BUS_MGR_SetMgrName(BUS_MGR_GetThis(), "XMGR_SATRECORD_BASE_Proc");
			eResult = xmgr_satrecord_MsgBusCreate_Base (pstContents);
			break;

		case eMEVT_SATRECORD_ACTION_START:
			HxLOG_Print ("[%s:%d] eMEVT_SATRECORD_ACTION_START (0x%08x:%d,%d,%d)\n", __FUNCTION__, __LINE__, hAction, p1, p2, p3);
			eResult = xmgr_satrecord_EvtActionStart_Base(pstContents);
			break;

		case eMEVT_BUS_DESTROY:
			HxLOG_Print ("[%s:%d] (%d) eMEVT_BUS_DESTROY (0x%08x:%d,%d,%d)\n", __FUNCTION__, __LINE__, pstContents->ulActionId, hAction, p1, p2, p3);
			eResult = xmgr_satrecord_MsgBusDestroy_Base(pstContents);
			break;

		case eSEVT_CH_LOCKED :
			HxLOG_Print ("[%s:%d] (%d) eSEVT_CH_LOCKED (0x%08x:%d,%d,%d)\n", __FUNCTION__, __LINE__, pstContents->ulActionId, hAction, p1, p2, p3);
			eResult = xmgr_satrecord_EvtChLocked_Base(pstContents, hAction);
			break;

		case eSEVT_CH_NO_SIGNAL:
			HxLOG_Print ("[%s:%d] (%d) eSEVT_CH_NO_SIGNAL (0x%08x:%d,%d,%d)\n", __FUNCTION__, __LINE__, pstContents->ulActionId, hAction, p1, p2, p3);
			eResult = xmgr_satrecord_EvtChNoSignal_Base(pstContents, hAction);
			break;

		case eMEVT_LIVE_NOTIFY_SVC_CHANGED:
			HxLOG_Print ("[%s:%d] (%d) eMEVT_LIVE_NOTIFY_SVC_CHANGED (0x%08x:%d,%d,%d)\n", __FUNCTION__, __LINE__, pstContents->ulActionId, hAction, p1, p2, p3);
			eResult = xmgr_satrecord_EvtSvcChange_Base(pstContents, hAction);
			break;

		case eSEVT_DOWN_DOWNLOAD_CHANGED:
			HxLOG_Print ("[%s:%d] (%d) eSEVT_DOWN_DOWNLOAD_CHANGED (0x%08x:%d,%d,%d)\n", __FUNCTION__, __LINE__, pstContents->ulActionId, hAction, p1, p2, p3);
			eResult = xmgr_satrecord_EvtDnldChanged_Base(pstContents, hAction);
			break;

		case eSEVT_DOWN_DOWNLOAD_SUCCESS:
			HxLOG_Print ("[%s:%d] (%d) eSEVT_DOWN_DOWNLOAD_SUCCESS (0x%08x:%d,%d,%d)\n", __FUNCTION__, __LINE__, pstContents->ulActionId, hAction, p1, p2, p3);
			eResult = xmgr_satrecord_EvtDownloadSuccess_Base(pstContents, hAction);
			break;

		case eSEVT_DOWN_DOWNLOAD_FAIL:
			HxLOG_Print ("[%s:%d] (%d) eSEVT_DOWN_DOWNLOAD_FAIL (0x%08x:%d,%d,%d)\n", __FUNCTION__, __LINE__, pstContents->ulActionId, hAction, p1, p2, p3);
			eResult = xmgr_satrecord_EvtDownloadFail_Base(pstContents, hAction);
			break;

		case eSEVT_DB_PARAM_UPDATED:
			HxLOG_Print ("[%s:%d] (%d) eSEVT_DB_PARAM_UPDATED (0x%08x:%d,%d,%d)\n", __FUNCTION__, __LINE__, pstContents->ulActionId, hAction, p1, p2, p3);
			if(p1 == eDB_PARAM_ITEM_SATREC_RECEIVERID)
			{
				eResult = xmgr_satrecord_EvtSetReceiverID_Base(pstContents);
			}
			else if (p1 == eDB_PARAM_ITEM_SATREC_UPDATE_INTERVAL)
			{
				eResult = xmgr_satrecord_EvtSetUpdateInterval_Base(pstContents);
			}
			break;
		default:
			break;
	}

	if (eResult == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}

int XMGR_SatRecord_DBG_MakeSchedule(void)
{
#if defined(CONFIG_DEBUG)
	HUINT32		actionId;
	Handle_t	hService;
	HUINT32 		ulCurrTime;
	HxDATETIME_t	stCurrTime;
	DbSvc_Info_t	svcInfo;
	SvcDownload_SatRecData_t stBookingData;

	actionId = MGR_ACTION_GetMainActionId();

	if(MGR_LIVE_CurrentSvcInfoGet(actionId, &hService) != ERR_OK)
	{
		return -1;
	}

	if(DB_SVC_GetServiceInfo(hService, &svcInfo) != ERR_OK)
	{
		return -1;
	}

	ulCurrTime = HLIB_STD_GetSystemTime();

	if(ulCurrTime == 0)
	{
		return -1;
	}
	(void)HLIB_DATETIME_ConvertUnixTimeToDateTime(ulCurrTime, &stCurrTime);

	HxSTD_MemSet(&stBookingData, 0, sizeof(SvcDownload_SatRecData_t));
	stBookingData.bSeriesFlag = FALSE;
	stBookingData.bRecordCancelFlag = TRUE;

	stBookingData.stBookingData.usOnId = DbSvc_GetOnId(&svcInfo);
	stBookingData.stBookingData.usTsId = DbSvc_GetTsId(&svcInfo);
	stBookingData.stBookingData.usSvcId = DbSvc_GetSvcId(&svcInfo);

	stBookingData.stBookingData.ucMonth = stCurrTime.stDate.ucMonth;
	stBookingData.stBookingData.ucDay = stCurrTime.stDate.ucDay + 1;
	stBookingData.stBookingData.ucStartHour = stCurrTime.stTime.ucHour;
	stBookingData.stBookingData.ucStartMinute = stCurrTime.stTime.ucSecond;
	stBookingData.stBookingData.ucDurHour = 1;
	stBookingData.stBookingData.ucDurMinutes = 30;

	if(xmgr_satrecord_AddSchedule(&stBookingData) != ERR_OK)
	{
		HxLOG_Warning("FAIL : ADD schedule - %d/%d, %d:%d (%dh %dm)\n",
			stBookingData.stBookingData.ucMonth, stBookingData.stBookingData.ucDay,
			stBookingData.stBookingData.ucStartHour,stBookingData.stBookingData.ucStartMinute,
			stBookingData.stBookingData.ucDurHour,stBookingData.stBookingData.ucDurMinutes);
	}
#endif

	return 0;
}

HERROR xmgr_SatRecord_BatchDeleteExistSchedule(void)
{
	DxSchedule_FindFilterAttr_t stFilter;
	HUINT32			ulNumSch = 0, i;

	DxSchedule_t	*pstSchArray = NULL;

	SAPI_FindScheduleFilterInit(&stFilter);
	stFilter.eRsvType = DxRSVTYPE_CHECK_SATRECORD_TIMER;

	SAPI_FindScheduleUsingFilter(&stFilter, &ulNumSch, &pstSchArray);

	for(i = 0; i < ulNumSch; i++)
	{
		DxSchedule_t	*pstSchedule = &(pstSchArray[i]);

		SAPI_CancelSchedule(pstSchedule->ulSlot);
	}

	SAPI_FindScheduleUsingFilterFreeList(pstSchArray);
	return ERR_OK;
}

HERROR xmgr_SatRecord_BatchMakeSchedule (HxDATETIME_t *pstStartTime)
{
	DxSchedule_t		stSchedule = {0, };
	HERROR			hErr = ERR_FAIL;
	Handle_t 			hSvc = HANDLE_NULL;
	SAPI_Conflict_t	stConflict;

	if(pstStartTime==NULL)
	{
		HxLOG_Error(" Can not found Time to reserve.\n");
		return ERR_FAIL;
	}

	VK_memset(&stSchedule, 0, sizeof(DxSchedule_t) );
	HxSTD_MemCopy(&stSchedule.stStartTime, pstStartTime, sizeof(HxDATETIME_t));

	hErr = SAPI_GetAvailableSlot(&stSchedule.ulSlot);
	if (hErr != ERR_OK)
	{
		HxLOG_Error(" There is no empty slot for sama!\n");
		return ERR_FAIL;
	}
	HxLOG_Debug("Available Slot ... (%d)\n", stSchedule.ulSlot);

	stSchedule.eRsvType = DxRSVTYPE_CHECK_SATRECORD_TIMER;

	stSchedule.ulSvcUid			= DB_SVC_GetServiceUID(hSvc);		// 깨어날때 처리하자.
	stSchedule.eRsvReady		= DxRSVREADY_30_SEC;
	stSchedule.ulDuration		= 0;
	stSchedule.eRepeat			= DxRSVREPEAT_ONCE;
	stSchedule.ulRepeatDays 	= 0;
	stSchedule.eEpgType 		= eDxEPG_TYPE_NONE;

	HxLOG_Debug("on Solt: %d,  SVC UID: 0x%x\n", stSchedule.ulSlot, stSchedule.ulSvcUid);

	hErr = SAPI_MakeSchedule (stSchedule.ulSlot, &stSchedule, &stConflict);

	return ERR_OK;
}

HERROR	XMGR_SatRecord_RescheduleBatch_Base(void)
{
	HUINT32 		ulStartTime = 0;
	HxDATETIME_t		stStartTime;
	HUINT32		ulInterval = 0;

	HxLOG_Trace();

	ulStartTime = HLIB_STD_GetSystemTime();

	ulInterval = xmgr_satrecord_getWakeupInterval();

	if(ulInterval <= 0)
	{
		HxLOG_Error("xmgr_satrecord_getWakeupInterval() fail! \n");
		return ERR_FAIL;
	}

	ulStartTime += ulInterval;

	if(HLIB_DATETIME_ConvertUnixTimeToDateTime(ulStartTime, &stStartTime) == ERR_FAIL)
	{
		HxLOG_Error(" Fail to Convert Current Time.\n");
		return ERR_FAIL;
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Error("stStartTime(%d) : %d-%d-%d %d:%d:%d \n", ulStartTime,
				stStartTime.stDate.usYear, stStartTime.stDate.ucMonth, stStartTime.stDate.ucDay,
				stStartTime.stTime.ucHour, stStartTime.stTime.ucMinute, stStartTime.stTime.ucSecond);
#endif

	// cancel existing emm schedule
	xmgr_SatRecord_BatchDeleteExistSchedule();

	// make reservation for Emm Wakeup
	xmgr_SatRecord_BatchMakeSchedule(&stStartTime);

	return ERR_OK;
}
/* End Of File */
