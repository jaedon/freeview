/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xmgr_schedule_tva.c
	@brief

	Description:  									\n
	Module: OBAMA / plugin / tvmanager / schedule	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/
/*
 * (c) 2011-2013 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <sapi.h>
#include <papi.h>
#include <octo_common.h>
#include <otl.h>

#include <db_svc.h>
#include <db_param.h>

#include <svc_fs.h>
#include <svc_pipe.h>
#include <svc_ringbuf.h>
#include <svc_mediaserver.h>
#include <svc_output.h>
#include <svc_sys.h>
#include <svc_epg.h>

#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_schedule.h>
#include <mgr_live.h>
#include <mgr_pvr_action.h>
#include <mgr_satip.h>
#include <mgr_epgbgtracking.h>
#include <mgr_ntls.h>
#include <mgr_epg.h>
#if defined(CONFIG_APCORE_EMM_UPDATE)
#include <mgr_emmupdate.h>
#endif
#if defined(CONFIG_MW_SATRECORD)
#include <mgr_satrecord.h>
#endif
#include <om_common.h>
#include "../local_include/_xmgr_schedule.h"

/*******************************************************************/
/********************      Local Definition         *************************/
/*******************************************************************/
#if defined (CONFIG_OP_PVR_BASED_ON_TVANYTIME_ACCURATE_RECORD)
#define MGR_SCH_IS_TRACKING_SCHEDULE(pstRsv) ((pstRsv) && ((pstRsv)->eRsvType == DxRSVTYPE_RECORD_SERIES_EBR||(pstRsv)->eRsvType == DxRSVTYPE_RECORD_EBR ) && (pstRsv)->uExtInfo.stRec.ulPreOffsetTime == 0 && (pstRsv)->uExtInfo.stRec.ulPostOffsetTime == 0)
#else
#define MGR_SCH_IS_TRACKING_SCHEDULE(pstRsv)	(FALSE)
#endif
//#define REGULAR_AUTO_UPDATE_TEST_CODE
/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC XmgrSch_TvAnyTimeContext_t		 stXmgrSchedTvAnyTime_Context;

#define ___LOCAL_FUNCTIONS___
STATIC HUINT32 s_eStandbySch[] =
{
	DxRSVTYPE_EMM,
	DxRSVTYPE_DESC_REC_STANDBY,
	DxRSVTYPE_CHECK_SATRECORD_TIMER
};


STATIC HERROR xmgr_schedule_CancelStandbySchedule_tva (void)
{
	HUINT32 		ulCnt = 0, ulNumSch = 0, ulNumSch2 = 0;
	DxSchedule_t	*pstSchArray = NULL;
	HERROR			hErr = ERR_FAIL;
	DxRsvType_e		eRsvType;
	HUINT32			i = 0;
	HUINT32			ulTotalCnt = 0;

	HxLOG_Trace();

	ulTotalCnt = sizeof(s_eStandbySch) / sizeof(HUINT32);

	for(i=0; i<ulTotalCnt; i++)
	{
		eRsvType = s_eStandbySch[i];

		ulNumSch = 0;
		hErr = SAPI_CountScheduleByType(eRsvType, &ulNumSch);
		HxLOG_Debug("Schedule Count ... (%d) \n", ulNumSch);
		if(hErr == ERR_OK && ulNumSch > 0)
		{
			pstSchArray = (DxSchedule_t *)OxMGR_Malloc(sizeof(DxSchedule_t) * ulNumSch);
			if (pstSchArray)
			{
				hErr = SAPI_GetAllSchedulesByRsvType(ulNumSch, &ulNumSch2, pstSchArray, eRsvType);
				if(hErr == ERR_OK)
				{
					for (ulCnt = 0; ulCnt < ulNumSch2; ulCnt++)
					{
						DxSchedule_t	*pstSchedule = &(pstSchArray[ulCnt]);
						if(pstSchedule != NULL && pstSchedule->eRsvType == eRsvType)
						{
							HxLOG_Debug("Cancel Schedule Slot (%d) \n", pstSchedule->ulSlot);
							SAPI_CancelSchedule(pstSchedule->ulSlot);
						}
					}
				}
				OxMGR_Free(pstSchArray);
			}
		}
	}

	return ERR_OK;
}

STATIC HERROR xmgr_schedule_ChangePaddingTime_tva (HUINT32 ulOffsetTime, HBOOL bIsPreOffset)
{

	HxLOG_Trace();
	(void)SAPI_ChangePaddingTime(ulOffsetTime, bIsPreOffset);

	return ERR_OK;

}

HERROR xmgr_schedule_makeScheduleFailReason_tva (void *pvContext, DxSchedule_t *pstSchedule)
{
	XmgrSch_BaseContext_t	*pstContext = (XmgrSch_BaseContext_t *)pvContext;
	MgrRec_RecSetup_t		 stRecSetup;
	HERROR					 hErr;
	Handle_t				 hSvcHandle;
	MgrRec_EndReasonType_e	eEndReason = eMgrRecFailed_NOFAIL;

	if ((NULL == pstContext) || (NULL == pstSchedule))					{ return ERR_FAIL; }


	HxLOG_Debug("slot: %d, eStatus: %d,  eRemovedReason: %d\n", pstSchedule->ulSlot, pstSchedule->eStatus, pstSchedule->stRemovedInfo.eRemovedReason);

	if(pstSchedule->eStatus == DxRSVSTATUS_RUNNING)
	{
		HxLOG_Debug("do not consider running schedule here.\n");
		return ERR_FAIL;
	}

	// 여기서는 power off나 no hdd인 경우만 만들어 주면 된다. 다른 경우는 이미 pvr 에서 만들었을 테니까.
	switch(pstSchedule->stRemovedInfo.eRemovedReason)
	{
		case DxSchedule_RemovedReason_Expired:
			eEndReason = eMgrRecFailed_POWER_OFF;
			break;

		case DxSchedule_RemovedReason_NoPvrDevice:
			eEndReason = eMgrRecFailed_NOHDD;
			break;

		// TODO: recording setup시에 pending되었던 schedule은 higher priority error reason을 set 해줘야 한다.
		case DxSchedule_RemovedReason_PendingBroken:
			eEndReason = eMgrRecFailed_UNABLE_TRACKING;
			break;

		default:
			HxLOG_Debug("do not process eRemovedReason: %d \n", pstSchedule->stRemovedInfo.eRemovedReason);
			return ERR_OK;

	}

	HxSTD_MemSet(&stRecSetup, 0, sizeof(MgrRec_RecSetup_t));

	switch (pstSchedule->eRsvType)
	{

	case DxRSVTYPE_RECORD_EBR:
		stRecSetup.eRecType = eMgrRecType_EBR;
		break;

	case DxRSVTYPE_RECORD_SERIES_EBR:
		stRecSetup.eRecType = eMgrRecType_SBR;
		break;

	case DxRSVTYPE_RECORD_TBR:
	case DxRSVTYPE_RECORD_SATRECORD_TBR:
		stRecSetup.eRecType = eMgrRecType_TBR;
		break;

	default:
		HxLOG_Debug("Do not process eRsvType(%d)\n", pstSchedule->eRsvType);
		return ERR_OK;
	}

 	hErr = DB_SVC_GetServiceHandleBySvcIdx((HUINT16)(pstSchedule->ulSvcUid & 0xFFFF), &hSvcHandle);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("Cannot find SvcHandle: ulSvcUid(%d)\n", pstSchedule->ulSvcUid);
		return ERR_FAIL;
	}

 	stRecSetup.eRecDst				= eMgrRecOutput_HDD;
	stRecSetup.hMasterSvc			= hSvcHandle;
	stRecSetup.hCurrSvc				= hSvcHandle;
	stRecSetup.ulSlot				= pstSchedule->ulSlot;
	stRecSetup.ulSourceContentId	= 0xFFFFFFFF;

	hErr = MGR_RECORDER_CreateRecordFailReason(&stRecSetup, eEndReason, pstSchedule);

	return hErr;
}

STATIC HBOOL	xmgr_schedule_cbCompare (void *userdata, void *data)
{
	DxSchedule_t			*pstSchedule = (DxSchedule_t *)userdata;
	XmgrSch_TrackingSch_t	*pstTracking = (XmgrSch_TrackingSch_t *)data;

	if (pstSchedule != NULL && pstTracking != NULL)
	{
		if (pstSchedule->ulSlot == pstTracking->ulSlot)
			return TRUE;
	}

	return FALSE;
}
STATIC HBOOL xmgr_schedule_cbReadyCompare (void *userdata, void *data)
{
	DxSchedule_t			*pstSchedule = (DxSchedule_t *)userdata;
	XmgrSch_ReadyRec_t		*pstReady = (XmgrSch_ReadyRec_t *)data;

	if (pstSchedule != NULL && pstReady != NULL)
	{
		if (pstSchedule->ulSlot == pstReady->ulSlot)
			return TRUE;
	}

	return FALSE;

}

STATIC HBOOL xmgr_schedule_CmpRunningSlot_tva (void *pvUserData, void *pvListData)
{
	DxSchedule_t			*pstSchedule = (DxSchedule_t *)pvUserData;
	XmgrSch_RunningRec_t	*pstRunRec = (XmgrSch_RunningRec_t *)pvListData;

	if ((NULL != pstSchedule) && (NULL != pstRunRec))
	{
		if (pstSchedule->ulSlot == pstRunRec->ulSlot)
		{
			return TRUE;
		}
	}

	return FALSE;
}

STATIC HERROR	xmgr_schedule_StartTracking_tva (void *pvContext, DxSchedule_t *pstReadySchedule)
{
	XmgrSch_TvAnyTimeContext_t		*pstContext = (XmgrSch_TvAnyTimeContext_t *)pvContext;
	XmgrSch_TrackingSch_t		*pstTrackingSch;
	HERROR						hErr;
	Handle_t					hSvc;

	HxLOG_Debug("ready schedule slot=(%d) \n",pstReadySchedule->ulSlot);

	// if is not tracking schedule then ignore
	if (!MGR_SCH_IS_TRACKING_SCHEDULE(pstReadySchedule))
	{
		HxLOG_Debug("Not a Tracking Schedule. slot=(%d) \n", pstReadySchedule->ulSlot);
		return ERR_OK;
	}

	pstTrackingSch = (XmgrSch_TrackingSch_t *)OxMGR_Malloc (sizeof(XmgrSch_TrackingSch_t));
	if (NULL == pstTrackingSch)
	{
		HxLOG_Debug("Memory Allocation failed:\n");

		return ERR_FAIL;
	}

	hErr = DB_SVC_GetServiceHandleBySvcIdx((HUINT16)(pstReadySchedule->ulSvcUid& 0xFFFF), &hSvc);
	if (ERR_OK != hErr)
	{
		HxLOG_Debug("Cannot find SvcHandle: ulSvcUid(%d)\n", pstReadySchedule->ulSvcUid);
		OxMGR_Free(pstTrackingSch);
		return ERR_FAIL;
	}

	pstTrackingSch->ulSlot		= pstReadySchedule->ulSlot;
	pstTrackingSch->hSvc		= hSvc;
	(void)HLIB_DATETIME_ConvertDateTimeToUnixTime (&(pstReadySchedule->stStartTime), &pstTrackingSch->utStartTime);
	pstTrackingSch->utEndTime	= pstTrackingSch->utStartTime + pstReadySchedule->ulDuration;

	pstContext->pstTrackingSchList = HLIB_LIST_Append (pstContext->pstTrackingSchList, (void *)pstTrackingSch);

	HxLOG_Debug("Start Tracking. \n");
	MGR_EPGBGTRACKING_Start();

	return ERR_OK;
}

STATIC HERROR		xmgr_schedule_StopTracking_tva (void *pvContext, DxSchedule_t *pstStopSchedule)
{
	XmgrSch_TvAnyTimeContext_t		*pstContext = (XmgrSch_TvAnyTimeContext_t *)pvContext;
	HERROR						    hErr = ERR_OK;
	HUINT32						    ulBeforeNumOfSch = 0;
	HUINT32						    ulAfterNumOfSch = 0;

	ulBeforeNumOfSch = HLIB_LIST_Length(pstContext->pstTrackingSchList);

	pstContext->pstTrackingSchList = HLIB_LIST_RemoveIf(pstContext->pstTrackingSchList
														, (void*)pstStopSchedule, xmgr_schedule_cbCompare, OxMGR_Free_CB);
	ulAfterNumOfSch = HLIB_LIST_Length(pstContext->pstTrackingSchList);

	if ( ulAfterNumOfSch == 0
		&& ulBeforeNumOfSch > 0)
	{
		HxLOG_Debug(HxANSI_COLOR_GREEN("There is No Tracking Schedule. Stop Tracking. \n"));

		hErr = MGR_EPGBGTRACKING_Stop();
		if (hErr != ERR_OK)
	{
			HxLOG_Error("MGR_EPGBGTRACKING_Stop failed. \n");
		}
	}
	else
	{
		HxLOG_Debug(HxANSI_COLOR_GREEN("Still there are schedules to track. num=(%d)->(%d) \n"),ulBeforeNumOfSch,ulAfterNumOfSch);
	}

	return hErr;
}

STATIC HERROR	xmgr_schedule_AddReadySchedule_tva (void *pvContext, DxSchedule_t *pstSchedule)
{
	XmgrSch_TvAnyTimeContext_t		*pstContext = (XmgrSch_TvAnyTimeContext_t *)pvContext;
	HxList_t					*pListItem;
	XmgrSch_ReadyRec_t 			*pstReadyRec = NULL;

	if (pvContext == NULL || pstSchedule == NULL)
	{
		HxLOG_Error("null param. \n");
		return ERR_FAIL;
	}

	pListItem = HLIB_LIST_FindEx(pstContext->pstReadyRecList, (const void*)pstSchedule, xmgr_schedule_cbReadyCompare);
	if (pListItem != NULL)
	{
		HxLOG_Debug("already in the list. slot=(%d) \n", pstSchedule->ulSlot);
		return ERR_OK;
	}

	pstReadyRec = (XmgrSch_ReadyRec_t *)OxMGR_Calloc (sizeof(XmgrSch_ReadyRec_t));
	if (NULL == pstReadyRec)
	{
		HxLOG_Error("Memory Allocation failed:\n");
		return ERR_FAIL;
	}

	pstReadyRec->ulSlot		= pstSchedule->ulSlot;

	pstContext->pstReadyRecList = HLIB_LIST_Append (pstContext->pstReadyRecList, (void *)pstReadyRec);

	return ERR_OK;
}

STATIC HERROR	xmgr_schedule_RemoveReadySchedule_tva (void *pvContext, DxSchedule_t *pstSchedule)
{
	XmgrSch_TvAnyTimeContext_t		*pstContext = (XmgrSch_TvAnyTimeContext_t *)pvContext;

	pstContext->pstReadyRecList = HLIB_LIST_RemoveIf(pstContext->pstReadyRecList
														, (void*)pstSchedule, xmgr_schedule_cbReadyCompare, OxMGR_Free_CB);

	return ERR_OK;
}

STATIC HBOOL	xmgr_schedule_IsAlreadyRecording_tva (void *pvContext, DxSchedule_t *pstSchedule)
{
	XmgrSch_BaseContext_t		*pstContext = (XmgrSch_BaseContext_t *)pvContext;
	HxList_t					*pstListItem;

	pstListItem = HLIB_LIST_FindEx(pstContext->pstRunRecList, (const void *)pstSchedule, xmgr_schedule_CmpRunningSlot_tva);
	if (pstListItem != NULL)
	{
		HxLOG_Error("Already running recording. slot=(%d) \n", pstSchedule->ulSlot);
		return TRUE;
	}

	return FALSE;
}
STATIC HUINT64	xmgr_schedule_CalcRecDiskSpace(HUINT32 ulDuration, HBOOL bIsHD)
{
	HUINT64	ullRecSpace = 0;

	if(bIsHD)
	{
		/* HD : 1시간에 4G 기준. 초당 KByte */
		ullRecSpace = (STORAGE_SIZE_FOR_HD_PER_HOUR * 1024 * 1024) / (SECONDS_PER_HOUR);
	}
	else
	{
		/* SD : 1시간에 1.5G 기준. 초당  KByte */
		ullRecSpace = (STORAGE_SIZE_FOR_SD_PER_HOUR * 1024 * 1024) / (SECONDS_PER_HOUR);
	}
	ullRecSpace *= ulDuration;

	return ullRecSpace;
}

STATIC HERROR		xmgr_schedule_RequestAutoDelete (void *pvContext, DxSchedule_t *pstSchedule)
{
	XmgrSch_TvAnyTimeContext_t	*pstTvAnyTimeContext = (XmgrSch_TvAnyTimeContext_t *)pvContext;
	XmgrSch_BaseContext_t	*pstBaseContext  = &(pstTvAnyTimeContext->stBaseContext);
	HERROR					hErr;
	HUINT32					i;
	HUINT32					ulRecCount;
	HxList_t				*pstListItemRec;
	XmgrSch_ReadyRec_t		*pstReadyRec;
	XmgrSch_RunningRec_t	*pstRunningRec;
	DxSchedule_t			*pstDummySchedule;
	UNIXTIME				utCurrTime;
	UNIXTIME				utStartTime;
	UNIXTIME				utEndTime;

	Handle_t		hSvc = HANDLE_NULL;
	DbSvc_Info_t	stSvcInfo;
	HBOOL			bIsHD = TRUE;

	HUINT64	ullRequestSizeKb = 0;
	HUINT32	ulRequestSizeKbUpper;
	HUINT32	ulRequestSizeKbLower;

	if (  pstSchedule->eRsvType != DxRSVTYPE_RECORD_EBR
		&& pstSchedule->eRsvType != DxRSVTYPE_RECORD_TBR
		&& pstSchedule->eRsvType != DxRSVTYPE_RECORD_SERIES_EBR )
	{
		HxLOG_Debug("Not supported rsv type. type=(%d)[%s] \n",pstSchedule->eRsvType,DLIB_ConvertEnum2Str(DxNAMEOF(DxRsvType_e), pstSchedule->eRsvType));
		return ERR_FAIL;
	}
	pstDummySchedule = (DxSchedule_t *)OxMGR_Malloc(sizeof(DxSchedule_t));
	if (pstDummySchedule == NULL)
	{
		return ERR_FAIL;
	}

	utCurrTime = (UNIXTIME)HLIB_STD_GetSystemTime();

	ullRequestSizeKb = 0;

	// 1. tracking ready ~ ready ~ start 사이의 schedule이 차지할 공간을 고려한다.
	ulRecCount = HLIB_LIST_Length(pstTvAnyTimeContext->pstReadyRecList);
	for (i=0;i < ulRecCount; i++)
	{
		pstListItemRec = HLIB_LIST_GetListItem(HLIB_LIST_First(pstTvAnyTimeContext->pstReadyRecList), i);
		pstReadyRec = HLIB_LIST_Data(pstListItemRec);
		if (pstReadyRec == NULL)
		{
			HxLOG_Error("Fail to find running rec. index=(%d)\n",i);
			continue;
		}

		hErr = SAPI_GetSchedule(pstReadyRec->ulSlot, pstDummySchedule);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("SAPI_GetSchedule failed. slot=(%d) \n", pstReadyRec->ulSlot);
			continue;
		}

		bIsHD = TRUE;
		hErr = DB_SVC_GetServiceHandleBySvcIdx((HUINT16)(pstDummySchedule->ulSvcUid& 0xFFFF), &hSvc);
		if (ERR_OK == hErr)
		{
			hErr = DB_SVC_GetServiceInfo(hSvc, &stSvcInfo);
			if (ERR_OK == hErr)
			{
				if(DbSvc_GetSvcType(&stSvcInfo) != eDxSVC_TYPE_TV || DbSvc_GetVideoType(&stSvcInfo) != eDxVIDEO_TYPE_HD)
				{
					// 최악의 경우를 위해 default를 TRUE로 넣고, HD가 아니라는 것을 확정할 때 FALSE로 넣는다.
					bIsHD = FALSE;
				}

			}
		}

		ullRequestSizeKb += xmgr_schedule_CalcRecDiskSpace(pstDummySchedule->ulDuration, bIsHD);
		HxLOG_Debug("(%d/%d) ready sch requested Size=(%lld), duration = (%d) \n",i, ulRecCount, ullRequestSizeKb, pstDummySchedule->ulDuration);
	}

	// 2. 현재 running rec의 남아 있는 duration이 차지할 공간을 고려한다.
	ulRecCount = HLIB_LIST_Length(pstBaseContext->pstRunRecList);
	for (i=0;i < ulRecCount; i++)
	{
		pstListItemRec = HLIB_LIST_GetListItem(HLIB_LIST_First(pstBaseContext->pstRunRecList), i);
		pstRunningRec = HLIB_LIST_Data(pstListItemRec);
		if (pstRunningRec == NULL)
		{
			HxLOG_Error("Fail to find running rec. index=(%d)\n",i);
			continue;
		}

		hErr = SAPI_GetSchedule(pstRunningRec->ulSlot, pstDummySchedule);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("SAPI_GetSchedule failed. slot=(%d) \n", pstRunningRec->ulSlot);
			continue;
		}

		bIsHD = TRUE;
		hErr = DB_SVC_GetServiceHandleBySvcIdx((HUINT16)(pstDummySchedule->ulSvcUid& 0xFFFF), &hSvc);
		if (ERR_OK == hErr)
		{
			hErr = DB_SVC_GetServiceInfo(hSvc, &stSvcInfo);
			if (ERR_OK == hErr)
			{
				if(DbSvc_GetSvcType(&stSvcInfo) != eDxSVC_TYPE_TV || DbSvc_GetVideoType(&stSvcInfo) != eDxVIDEO_TYPE_HD)
				{
					// 최악의 경우를 위해 default를 TRUE로 넣고, HD가 아니라는 것을 확정할 때 FALSE로 넣는다.
					bIsHD = FALSE;
				}

			}
		}

		HLIB_DATETIME_ConvertDateTimeToUnixTime(&(pstDummySchedule->stStartTime), &utStartTime);
		utStartTime -= pstDummySchedule->uExtInfo.stRec.ulPreOffsetTime;
		utEndTime	= utStartTime + pstDummySchedule->ulDuration + pstDummySchedule->uExtInfo.stRec.ulPostOffsetTime;
		utStartTime = (utStartTime < utCurrTime)? utCurrTime : utStartTime; // MAX

		if ( utEndTime > utStartTime)
		{
			ullRequestSizeKb += xmgr_schedule_CalcRecDiskSpace(utEndTime - utStartTime, bIsHD);
		}
		else
		{
			HxLOG_Error("start time is past end time. start time=(%d) end time=(%d) \n",utStartTime,utEndTime);
		}
		HxLOG_Debug("(%d/%d) running sch requested Size=(%lld), duration = (%d) \n",i, ulRecCount, ullRequestSizeKb, utEndTime - utStartTime);
	}

	ulRequestSizeKbUpper = (ullRequestSizeKb >> 32);
	ulRequestSizeKbLower = ullRequestSizeKb & 0xffffffff;

	HxLOG_Debug("req %lld \n",ullRequestSizeKb);
	HxLOG_Debug("upper %u \n",ulRequestSizeKbUpper);
	HxLOG_Debug("lower %u \n",ulRequestSizeKbLower);
	BUS_PostMessage(NULL, eOEVT_METARECLIST_REQUEST_AUTO_DELETE, HANDLE_NULL, (HINT32)ulRequestSizeKbUpper, (HINT32)ulRequestSizeKbLower, 0);

	OxMGR_Free(pstDummySchedule);

	return ERR_OK;
}

STATIC void xmgr_schedule_CheckIfTrackingAvailable (void *pvContext)
{
#ifdef CONFIG_DEBUG
	XmgrSch_TvAnyTimeContext_t	*pstTvAnyTimeContext = xmgr_schedule_GetContext_tva();
	HBOOL					bIsTrackingShouldOn = FALSE;
	HxList_t				*pstList;

	if (HLIB_LIST_Length(pstTvAnyTimeContext->pstTrackingSchList) >0)
	{
		bIsTrackingShouldOn = TRUE;
	}

	if (bIsTrackingShouldOn != TRUE)
	{
		return;
	}

	if (BUS_MGR_Get(MGR_EPGBGTRACKING_GetProc()) == NULL)
	{
		HxLOG_Error("Tracking failed !!.");
		for (pstList = HLIB_LIST_First(pstTvAnyTimeContext->pstTrackingSchList); pstList != NULL; pstList = HLIB_LIST_Next(pstList))
		{
			XmgrSch_TrackingSch_t *pstTracking = HLIB_LIST_Data(pstList);
			if (pstTracking == NULL)
			{
				HxLOG_Error("null tracking list data \n");
				continue;
			}
			HxLOG_Error("Tracking slot=(%d), hSvc=(%d) \n", pstTracking->ulSlot, pstTracking->hSvc);
		}
		HxLOG_Error("Tracking fail case. abort !!");

		// 무조건 죽여서 해당 케이스를 확인한다.
		HxLOG_Assert(1);
		HxLOG_Assert(0);
	}
#endif
}

STATIC void xmgr_schedule_PrepareTimeUp (DxSchedule_t *pstSchedule, SAPI_TimerItemType_e eTimerType)
{
	// 안전하게 ready로. 혹은...start 로 해도 무방 ??
	if ( (eTimerType == eSAPI_TIMER_TYPE_Ready || eTimerType == eSAPI_TIMER_TYPE_ReadyTracking)
		&& MGR_ACTION_GetBatchContext() != NULL)
	{

		if(SAPI_GetSapiActionTypeBySchedule(pstSchedule) == eSAPI_ACTION_TYPE_Record)
		{
			BUS_SendMessage(NULL, eMEVT_ACTION_SHUTDOWN_DESTROY, HANDLE_NULL, 0, 0, 0);
		}
	}
}
static HBOOL xmgr_schedule_IsAutoDeleteNeeded (DxSchedule_t *pstSchedule, SAPI_TimerItemType_e eTimer)
{
	if (eTimer == eSAPI_TIMER_TYPE_ReadyTracking)
	{
		return TRUE;
	}
	else if (eTimer == eSAPI_TIMER_TYPE_Ready)
	{
		if (!MGR_SCH_IS_TRACKING_SCHEDULE(pstSchedule))
		{
			return TRUE;
		}

	}
	return FALSE;
}

HBOOL xmgr_schedule_IsScheduleExistsInTime_tva (HUINT32	ulMin)
{
	UNIXTIME			ulCurTime;
	HUINT32				ulNumSch = 0;
	HUINT32				i;
	DxSchedule_t		*pstSchArray = NULL;
	DxSchedule_t		*pstSchedule = NULL;
	HBOOL				isScheduleExists = FALSE;

	DxSchedule_FindFilterAttr_t stFilter;

	ulCurTime = HLIB_STD_GetSystemTime();

	SAPI_FindScheduleFilterInit(&stFilter);
	stFilter.stOverlapTimeRange.utOverlapTimeRangeStart = ulCurTime;
	stFilter.stOverlapTimeRange.utOverlapTimeRangeEnd = ulCurTime + ulMin*60;

	SAPI_FindScheduleUsingFilter(&stFilter, &ulNumSch, &pstSchArray);

	for (i=0; i<ulNumSch; i++)
	{
		pstSchedule = &(pstSchArray[i]);
		if (!(pstSchedule->eRsvType == DxRSVTYPE_WATCHING_EBR
			||pstSchedule->eRsvType == DxRSVTYPE_WATCHING_TBR
			||pstSchedule->eRsvType == DxRSVTYPE_WATCHING_SERIES_EBR
			||pstSchedule->eRsvType == DxRSVTYPE_RECORD_EBR
			||pstSchedule->eRsvType == DxRSVTYPE_RECORD_TBR
			||pstSchedule->eRsvType == DxRSVTYPE_RECORD_INSTANT
			||pstSchedule->eRsvType == DxRSVTYPE_RECORD_DELAYED
			||pstSchedule->eRsvType == DxRSVTYPE_RECORD_SERIES_EBR
			||pstSchedule->eRsvType == DxRSVTYPE_NETSTREAMING_LIVE
			||pstSchedule->eRsvType == DxRSVTYPE_NETSTREAMING_FILE
			||pstSchedule->eRsvType == DxRSVTYPE_ACTIVATION_TIME
			||pstSchedule->eRsvType == DxRSVTYPE_POWER_ON))
		{
			// conflict type: watch or record or power-on
			HxLOG_Debug("Not a Conflict Type. type=(%d) \n",pstSchedule->eRsvType);
			continue;
		}

		isScheduleExists = TRUE;
	}
	SAPI_FindScheduleUsingFilterFreeList(pstSchArray);

	return isScheduleExists;
}



STATIC BUS_Result_t xproc_schedule_TvAnyTime (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrSch_TvAnyTimeContext_t	*pstTvAnyTimeContext = xmgr_schedule_GetContext_tva();
	XmgrSch_BaseContext_t	*pstBaseContext	 = &(pstTvAnyTimeContext->stBaseContext);
	BUS_Result_t			 eRet = MESSAGE_PASS;
	HERROR				hErr = ERR_FAIL;
	HUINT32				ulOffsetTime = 0;

	switch (message)
	{
	case eMEVT_BUS_CREATE:
		HxLOG_Info("eMEVT_BUS_CREATE \n");
		BUS_MGR_SetMgrName(BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
		HxSTD_MemSet(pstTvAnyTimeContext, 0, sizeof(XmgrSch_TvAnyTimeContext_t));
		eRet = MESSAGE_BREAK;
		break;


	/********************
	*  schedule
	*********************/
#if defined(CONFIG_STREAMING_SATIP)
	case eMEVT_SCHEDULE_SATIP_CHANGEPID:
		HxLOG_Info("eMEVT_SCHEDULE_SATIP_CHANGEPID \n");
		eRet = xmgr_schedule_MsgSatIpChangePid_Base((void *)pstBaseContext, hAction, p1, p2, p3);
		break;
#endif

#if defined(CONFIG_NON_TRANSCODING_STREAMING)
	case eMEVT_SCHEDULE_NTLS_CHANGEPID:
		HxLOG_Info("eMEVT_SCHEDULE_NTLS_CHANGEPID \n");
		eRet = xmgr_schedule_MsgNtlsChangePid_Base((void *)pstBaseContext, hAction, p1, p2, p3);
		break;
#endif

	case eMEVT_SCHEDULE_TIME_UP:
		HxLOG_Info("eMEVT_SCHEDULE_TIME_UP \n");
		xmgr_schedule_PrepareTimeUp((DxSchedule_t *)p3, (SAPI_TimerItemType_e)p2);
		eRet = xmgr_schedule_MsgTimeUp_tva((void *)pstTvAnyTimeContext, hAction, p1, p2, p3);
		break;

	case eMEVT_SCHEDULE_CANCELED:
		HxLOG_Info("eMEVT_SCHEDULE_CANCELED \n");
		(void)xmgr_schedule_makeScheduleFailReason_tva((void *)pstBaseContext, (DxSchedule_t *)p3);
		eRet = MGR_SCHEDULE_SetScheduleInCancelDB(p2, (DxSchedule_t *)p3);
		eRet |= xmgr_schedule_MsgTimeUp_tva((void *)pstTvAnyTimeContext, hAction, p1, eSAPI_TIMER_TYPE_Stop, p3);
		break;

	case eMEVT_SCHEDULE_EVENT_OCCURED:
		HxLOG_Info("eMEVT_SCHEDULE_EVENT_OCCURED\n");
		(void)MGR_SCHEDULE_CheckFrontPannelDisplay();
		(void)xmgr_schedule_MsgScheduleEventOccured_tva((void *)pstTvAnyTimeContext, hAction, p1, p2, p3);
		break;

	/*********************
	*	PVR
	**********************/
	case eMEVT_PVR_NOTIFY_RECORD_SETUP_FAILED:
		HxLOG_Info("eMEVT_PVR_NOTIFY_RECORD_SETUP_FAILED \n");
		eRet = xmgr_schedule_MsgRecordSetupFail_Base((void *)pstBaseContext, hAction, p1, p2, p3);
		break;

	case eMEVT_PVR_RECORD_CHANGE_TSR_REC_TO_DELAYED_RECORD_RESULT:
		HxLOG_Info("eMEVT_PVR_RECORD_CHANGE_TSR_REC_TO_DELAYED_RECORD_RESULT\n");
		eRet = xmgr_schedule_MsgDelayedRecordSetupResult_Base((void *)pstBaseContext, hAction, p1, p2, p3);
		break;

	case eMEVT_PVR_PLAYBACK_STOP:
		HxLOG_Info("eMEVT_PVR_PLAYBACK_STOP \n");
		eRet = xmgr_schedule_MsgPvrPlaybackStop_Base((void *)pstBaseContext, hAction, p1, p2, p3);
		break;


	/*********************
	*	SI , EPG
	**********************/
	case eSEVT_SI_UPDATE_NCD_SCHEDULE:
		HxLOG_Info("eSEVT_SI_UPDATE_NCD_SCHEDULE \n");
		eRet = xmgr_schedule_MsgUpdateNcdSchedule_tva((void *)pstBaseContext, hAction, p1, p2, p3);
		break;

	case eSEVT_EPG_FPB_UPDATE_SCHEDULE_SERIES_SET:
		HxLOG_Info("eSEVT_EPG_FPB_UPDATE_SCHEDULE_SERIES_SET \n");
		eRet = SAPI_UpdateSeries();
		break;

	/*********************
	*	action
	**********************/
	case eMEVT_ACTION_NOTIFY_TAKEN:
	case eMEVT_ACTION_NOTIFY_RELEASED:
		//xmgr_schedule_CheckIfTrackingAvailable((void *)pstTvAnyTimeContext);
		eRet = MESSAGE_PASS;
		break;

	/*********************
	*	ETC
	**********************/

#if defined(CONFIG_STREAMING_SATIP)
	case eMEVT_SATIP_NOTIFY_START_FAILED:
		HxLOG_Info("eMEVT_SATIP_NOTIFY_START_FAILED \n");
		eRet = xmgr_schedule_MsgSatipStartFailed_Base((void *)pstBaseContext, hAction, p1, p2, p3);
		break;
#endif

#if defined(CONFIG_NON_TRANSCODING_STREAMING)
	case eMEVT_NTLS_NOTIFY_START_STOPPED:
		HxLOG_Info("eMEVT_NTLS_NOTIFY_START_STOPPED \n");
		eRet = xmgr_schedule_MsgNtlsStopped_Base((void *)pstBaseContext, hAction, p1, p2, p3);
		break;

	case eMEVT_NTLS_NOTIFY_START_FAILED:
		HxLOG_Info("eMEVT_NTLS_NOTIFY_START_FAILED \n");
		eRet = xmgr_schedule_MsgNtlsStartFailed_Base((void *)pstBaseContext, hAction, p1, p2, p3);
		break;
#endif

	case eSEVT_DB_PARAM_UPDATED:
		HxLOG_Info("eSEVT_DB_PARAM_UPDATED p1(%d) \n", p1);
		switch(p1)
		{
			case eDB_PARAM_ITEM_PADDINGTIME_START:
				HxLOG_Info("eDB_PARAM_ITEM_PADDINGTIME_START\n");
				hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_PADDINGTIME_START, (HUINT32 *)&ulOffsetTime, 0);

				if(hErr == ERR_FAIL)
				{
					HxLOG_Error("get padding time error \n");
					return hErr;
				}

				xmgr_schedule_ChangePaddingTime_tva(ulOffsetTime, TRUE);
				break;

			case eDB_PARAM_ITEM_PADDINGTIME_STOP:
				HxLOG_Info("eDB_PARAM_ITEM_PADDINGTIME_STOP\n");
				hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_PADDINGTIME_STOP, (HUINT32 *)&ulOffsetTime, 0);

				if(hErr == ERR_FAIL)
				{
					HxLOG_Error("get padding time error \n");
					return hErr;
				}

				xmgr_schedule_ChangePaddingTime_tva(ulOffsetTime, FALSE);
				break;

			default:
				break;

		}
		eRet = MESSAGE_PASS;
		break;

	case eMEVT_BUS_GO_OPERATION:
		HxLOG_Info("eMEVT_BUS_GO_OPERATION\n");
		(void)xmgr_schedule_CancelStandbySchedule_tva();
		break;

	default:
		eRet = MESSAGE_PASS;
		break;
	}

	return (MESSAGE_BREAK == eRet) ? MESSAGE_BREAK : MESSAGE_PASS;
}


#define ___GLOBAL_FUNCTIONS___
HERROR xmgr_schedule_Init_tva (void)
{
	HERROR			 hErr;

	hErr = BUS_MGR_Create(NULL, APP_ACTION_PRIORITY, xproc_schedule_TvAnyTime,
							HANDLE_NULL, 0, 0, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("BUS_MGR_Create err:\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

XmgrSch_TvAnyTimeContext_t *xmgr_schedule_GetContext_tva (void)
{
	return &stXmgrSchedTvAnyTime_Context;
}

Handle_t	xmgr_schedule_GetEarliestTrackingSvc_tva (void)
{
	XmgrSch_TvAnyTimeContext_t *pstTvAnyTimeContext		= xmgr_schedule_GetContext_tva();
	Handle_t				hEarliestSvc		= HANDLE_NULL;
	UNIXTIME				utEarliestTime		= 0xFFFFFFFF;
	UNIXTIME				utStartTime 		= 0xFFFFFFFF;
	UNIXTIME				utEndTime 			= 0;
	XmgrSch_TrackingSch_t	*pstTrackingSch;
	HxList_t				*pstListItem;
	DxSchedule_t			*pstSchedule;
	HERROR					hErr = ERR_OK;

	pstSchedule = (DxSchedule_t *)OxMGR_Malloc(sizeof(DxSchedule_t));
	if (pstSchedule == NULL)
	{
		return HANDLE_NULL;
	}

	pstListItem = HLIB_LIST_First(pstTvAnyTimeContext->pstTrackingSchList);
	while ( pstListItem )
	{
		pstTrackingSch = (XmgrSch_TrackingSch_t *)HLIB_LIST_Data(pstListItem);
		if ( pstTrackingSch == NULL )
		{
			pstListItem = HLIB_LIST_Next(pstListItem);
			continue;
		}

		hErr = SAPI_GetSchedule(pstTrackingSch->ulSlot, pstSchedule);
		if (hErr != ERR_OK)
		{
			HxLOG_Debug(HxANSI_COLOR_GREEN("Tracking Schedule already deleted. slot =(%d)\n "), pstTrackingSch->ulSlot);
			pstListItem = HLIB_LIST_Next(pstListItem);
			continue;
		}

		if (pstSchedule->eStatus == DxRSVSTATUS_RUNNING)
		{
			// ignore running schedule's start time
			utStartTime = 0xFFFFFFFF;
		}
		else
		{
			utStartTime = pstTrackingSch->utStartTime;
		}

		utEndTime = pstTrackingSch->utEndTime;

		if ( utStartTime < utEarliestTime )
		{
			utEarliestTime = utStartTime;
			hEarliestSvc  = pstTrackingSch->hSvc;
		}

		if (utEndTime < utEarliestTime)
		{
			utEarliestTime = utEndTime;
			hEarliestSvc		= pstTrackingSch->hSvc;
		}

		pstListItem = HLIB_LIST_Next(pstListItem);
	}

	HxLOG_Debug(HxANSI_COLOR_GREEN(" hEarliestSvc (0x%x) utEarliestTime=(%d)\n"),hEarliestSvc,utEarliestTime);

	OxMGR_Free(pstSchedule);

	return hEarliestSvc;
}

HERROR		xmgr_schedule_MakeRegularOta_tva (void)
{
	HERROR			hErr = ERR_OK;
	UNIXTIME		utDummyTimeGap;
	UNIXTIME		utCurrTime;
	UNIXTIME		utStartTime;
	HxDATETIME_t	stCurrTime;
	HUINT32 		ulSlot;
	DxSchedule_t	*pstSchedule;
	UNIXTIME		utBalancingTime = 0;
	HUINT32			ulRegularOtaCnt = 0;



	hErr = SAPI_GetAvailableSlot(&ulSlot);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("SAPI_GetAvailableSlot err:\n");
		return ERR_FAIL;
	}

	(void)SAPI_CountScheduleByType(DxRSVTYPE_REGULAR_OTA, &ulRegularOtaCnt);
	if (ulRegularOtaCnt > 0)
	{
		HxLOG_Debug(HxANSI_COLOR_GREEN("Already Regular ota exists. \n"));
		return ERR_OK;
	}

	pstSchedule = (DxSchedule_t *)OxMGR_Calloc(sizeof(DxSchedule_t));
	if (pstSchedule == NULL)
	{
		return ERR_FAIL;
	}

	utCurrTime = (UNIXTIME)HLIB_STD_GetSystemTime();

#if defined(REGULAR_AUTO_UPDATE_TEST_CODE)
	HLIB_DATETIME_ConvertUnixTimeToDateTime(utCurrTime, &(pstSchedule->stStartTime));
	HxLOG_Debug(HxANSI_COLOR_GREEN("Curr time %d-%d-%d %d:%d:%d \n")
									,stCurrTime.stDate.usYear,stCurrTime.stDate.ucMonth,stCurrTime.stDate.ucDay
									,stCurrTime.stTime.ucHour,stCurrTime.stTime.ucMinute,stCurrTime.stTime.ucSecond);

	// if 0 ~ 2 AM	  : Regular ota at  the day
	// else past 2 AM : Regular ota at  the next day
	if (stCurrTime.stTime.ucHour < 2)
	{
		utStartTime = utCurrTime;
	}
	else
	{
		HLIB_DATETIME_GetNextDay(utCurrTime, &utStartTime, &utDummyTimeGap);
	}
	pstSchedule->stStartTime.stTime.ucMinute	+= 10; // regular auto update interval
    /*
	// random 3 ~ 5 AM
	HLIB_DATETIME_ConvertUnixTimeToDateTime(utStartTime, &(pstSchedule->stStartTime));
	utBalancingTime = (utCurrTime * 23523)%(2<<30); // random
	utBalancingTime = utBalancingTime % (2 * 60 * 60); // 0 ~ 2 hr
	pstSchedule->stStartTime.stTime.ucHour	= (HUINT8)3 + (utBalancingTime/3600);
	pstSchedule->stStartTime.stTime.ucMinute	= (HUINT8)0 + ((utBalancingTime/2)/60);
	pstSchedule->stStartTime.stTime.ucSecond  = (HUINT8)0 + (utBalancingTime%60);
    */
	pstSchedule->ulDuration = 1; // 0보다는 크게..

	HxLOG_Debug(HxANSI_COLOR_GREEN("Make Regular Ota at %d-%d-%d %d:%d:%d \n")
									,pstSchedule->stStartTime.stDate.usYear,pstSchedule->stStartTime.stDate.ucMonth,pstSchedule->stStartTime.stDate.ucDay
									,pstSchedule->stStartTime.stTime.ucHour,pstSchedule->stStartTime.stTime.ucMinute,pstSchedule->stStartTime.stTime.ucSecond);
#else

	HLIB_DATETIME_ConvertUnixTimeToDateTime(utCurrTime, &(stCurrTime));
	HxLOG_Debug(HxANSI_COLOR_GREEN("Curr time %d-%d-%d %d:%d:%d \n")
									,stCurrTime.stDate.usYear,stCurrTime.stDate.ucMonth,stCurrTime.stDate.ucDay
									,stCurrTime.stTime.ucHour,stCurrTime.stTime.ucMinute,stCurrTime.stTime.ucSecond);

	// if 0 ~ 2 AM	  : Regular ota at  the day
	// else past 2 AM : Regular ota at  the next day
	if (stCurrTime.stTime.ucHour < 2)
	{
		utStartTime = utCurrTime;
	}
	else
	{
		HLIB_DATETIME_GetNextDay(utCurrTime, &utStartTime, &utDummyTimeGap);
	}

	// random 3 ~ 5 AM
	HLIB_DATETIME_ConvertUnixTimeToDateTime(utStartTime, &(pstSchedule->stStartTime));
	utBalancingTime = (utCurrTime * 23523)%(2<<30); // random
	utBalancingTime = utBalancingTime % (2 * 60 * 60); // 0 ~ 2 hr
	pstSchedule->stStartTime.stTime.ucHour	= (HUINT8)3 + (utBalancingTime/3600);
	pstSchedule->stStartTime.stTime.ucMinute	= (HUINT8)0 + ((utBalancingTime/2)/60);
	pstSchedule->stStartTime.stTime.ucSecond  = (HUINT8)0 + (utBalancingTime%60);
	pstSchedule->ulDuration = 1; // 0보다는 크게..
	HxLOG_Debug(HxANSI_COLOR_GREEN("Make Regular Ota at %d-%d-%d %d:%d:%d \n")
									,pstSchedule->stStartTime.stDate.usYear,pstSchedule->stStartTime.stDate.ucMonth,pstSchedule->stStartTime.stDate.ucDay
									,pstSchedule->stStartTime.stTime.ucHour,pstSchedule->stStartTime.stTime.ucMinute,pstSchedule->stStartTime.stTime.ucSecond);
#endif
	pstSchedule->ulSlot		= ulSlot;
	pstSchedule->eRsvType		= DxRSVTYPE_REGULAR_OTA;
	pstSchedule->eRsvReady	= DxRSVREADY_30_SEC;
	pstSchedule->eRepeat		= DxRSVREPEAT_ONCE;


	hErr = SAPI_MakeSchedule (ulSlot, pstSchedule, NULL);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SAPI_MakeSchedule err:\n");
		OxMGR_Free(pstSchedule);
		return ERR_FAIL;
	}

	OxMGR_Free(pstSchedule);

	return ERR_OK;
}

HERROR		xmgr_schedule_CheckConflictSchedule_tva (void)
{
	if (xmgr_schedule_IsScheduleExistsInTime_tva(40) == TRUE
		|| MGR_ACTION_GetRecordCount() > 0)
	{
		HxLOG_Debug(HxANSI_COLOR_GREEN("Conflict: Schedule in 40 min or recording is going on \n"));
		return ERR_FAIL;
	}

	HxLOG_Debug(HxANSI_COLOR_GREEN("No Conflict: No schedule in 40 min and no recording is going on \n"));

	return ERR_OK;
}


#define ___MESSAGE_FUNCTIONS___


BUS_Result_t xmgr_schedule_MsgTimeUp_tva (void *pvContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulSlotId		= (HUINT32)p1;
	SAPI_TimerItemType_e	 eTimerType		= (SAPI_TimerItemType_e)p2;
	DxSchedule_t			*pstSchedule	= (DxSchedule_t *)p3;
	XmgrSch_TvAnyTimeContext_t		*pstTvAnyTimeContext = (XmgrSch_TvAnyTimeContext_t *)pvContext;
	XmgrSch_BaseContext_t		*pstBaseContext;

	if (NULL == pstSchedule || NULL == pstTvAnyTimeContext)
	{
		HxLOG_Error("pstSchedule NULL: SlotId(%d)\n", ulSlotId);
		return MESSAGE_PASS;
	}

	pstBaseContext = &(pstTvAnyTimeContext->stBaseContext);

	HxLOG_Debug("pstSchedule->eRsvType: 0x%x \n", pstSchedule->eRsvType);
	switch (pstSchedule->eRsvType)
	{
		case DxRSVTYPE_NETSTREAMING_LIVE:
		case DxRSVTYPE_NETSTREAMING_FILE:
			switch (eTimerType)
			{
			case eSAPI_TIMER_TYPE_Start:
				(void)xmgr_schedule_StartMemRecord_Base(pstBaseContext, pstSchedule);
				break;

			case eSAPI_TIMER_TYPE_Stop:
				(void)xmgr_schedule_StopMemRecord_Base(pstBaseContext, pstSchedule);
				break;

			default:
				break;
			}

			break;

		case DxRSVTYPE_RECORD_EBR:
		case DxRSVTYPE_RECORD_TBR:
		case DxRSVTYPE_RECORD_INSTANT:
		case DxRSVTYPE_RECORD_DELAYED:
		case DxRSVTYPE_RECORD_SERIES_EBR:
		case DxRSVTYPE_RECORD_SATRECORD_TBR:
		case DxRSVTYPE_DESC_REC_STANDBY:
			switch (eTimerType)
			{
			case eSAPI_TIMER_TYPE_ReadyTracking:
			case eSAPI_TIMER_TYPE_Ready:
				if (eTimerType == eSAPI_TIMER_TYPE_ReadyTracking)
				{
					(void)xmgr_schedule_StartTracking_tva(pstTvAnyTimeContext, pstSchedule);
				}
				(void)xmgr_schedule_AddReadySchedule_tva(pstTvAnyTimeContext, pstSchedule);
				if (xmgr_schedule_IsAutoDeleteNeeded(pstSchedule, eTimerType) == TRUE)
				{
					(void)xmgr_schedule_RequestAutoDelete(pstTvAnyTimeContext, pstSchedule);
				}
				break;
			case eSAPI_TIMER_TYPE_Start:
				if ( xmgr_schedule_IsAlreadyRecording_tva(pstBaseContext, pstSchedule) == TRUE)
				{
					// tracking <-> padding 간의 간섭으로 start를 두번 하는 경우에 대한 예외처리
					HxLOG_Debug("Already running rec. ignore start timer. slot=(%d) \n", pstSchedule->ulSlot);
					break;
				}
				(void)xmgr_schedule_StartRecord_Base(pstBaseContext, pstSchedule);
				(void)xmgr_schedule_RemoveReadySchedule_tva(pstTvAnyTimeContext, pstSchedule);
				break;

			case eSAPI_TIMER_TYPE_Stop:
				(void)xmgr_schedule_StopTracking_tva(pstTvAnyTimeContext, pstSchedule);
				(void)xmgr_schedule_RemoveReadySchedule_tva(pstTvAnyTimeContext, pstSchedule);
				(void)xmgr_schedule_StopRecord_Base(pstBaseContext, pstSchedule);
				break;

			case eSAPI_TIMER_TYPE_PaddingStopReady:
				(void)xmgr_schedule_RecordStopPadding_Base(pstBaseContext, pstSchedule);
				break;

			default:
				break;
			}

			break;

		case DxRSVTYPE_WATCHING_EBR:
		case DxRSVTYPE_WATCHING_TBR:
			xmgr_schedule_reminder_base();
			break;

		case DxRSVTYPE_WATCHING_SERIES_EBR:
		case DxRSVTYPE_POWER_ON:
			/* 해당처리가.. 웹으로 이동... */
			break;

		case DxRSVTYPE_POWER_OFF:
			switch (eTimerType)
			{
			case eSAPI_TIMER_TYPE_Start:
				(void)PAPI_RequestShutDown();
				break;

			default:
				break;
			}
			break;

		case DxRSVTYPE_REGULAR_OTA:
			{
				MgrAction_State_e	eActionState;
				HERROR				hErr;

				eActionState = MGR_ACTION_GetState();
				HxLOG_Debug("Regular ota timeup timer=(%d), actionState=(%d) \n",eTimerType,eActionState);

				if (eTimerType == eSAPI_TIMER_TYPE_Stop)
				{
					HxLOG_Debug(HxANSI_COLOR_GREEN("Regular ota Stopped. Make Next one \n"));
					MGR_SCHEDULE_MakeRegularOta();
				}

				if (eTimerType != eSAPI_TIMER_TYPE_Start)
				{
					HxLOG_Debug("Not OTA prerequisite. prerequisite = OTA time up & in standby \n ");
					break;
				}

				if (eActionState != eAmState_ACTIVE_STANDBY)
				{
					HxLOG_Debug("Not OTA prerequisite. prerequisite = OTA time up & in standby \n ");
					break;
				}


				hErr = MGR_SCHEDULE_CheckConflictSchedule();
				if (hErr != ERR_OK)
				{
					HxLOG_Debug("Conflict: Can't execute reboot for regular ota. \n");
					break;
				}

				HxLOG_Error("Reboot by regular OTA. \n");
				MGR_ACTION_RebootSystem();
			}
			break;
#if defined (CONFIG_MW_SATRECORD)
		case DxRSVTYPE_CHECK_SATRECORD_TIMER:
			if(eTimerType == eSAPI_TIMER_TYPE_Start)
			{
				MgrAction_State_e eActionState;

				eActionState = MGR_ACTION_GetState();
				if (eAmState_ACTIVE_STANDBY == eActionState)
				{
					xmgr_schedule_BatchAddSatRecProOnStandby_base();
				}
			}
			break;
#endif
		case DxRSVTYPE_ASO_NOTIFY:
			HxLOG_Debug("NCD schedule. slot(%d) timer(%d) status(%d) removed(%d) \n",pstSchedule->ulSlot, eTimerType, pstSchedule->eStatus, pstSchedule->stRemovedInfo.eRemovedReason);
			if ((eTimerType == eSAPI_TIMER_TYPE_Stop)
				&& (pstSchedule->stRemovedInfo.eRemovedReason == DxSchedule_RemovedReason_Expired))
			{
				HUINT32 utCurrTime;
				utCurrTime = (UNIXTIME)HLIB_STD_GetSystemTime();
				// TODO: if POWER ON/OFF currTime+40 else currTime + 3
				BUS_PostMessage(NULL, eSEVT_SI_UPDATE_NCD_SCHEDULE, HANDLE_NULL, utCurrTime+40, 0, 0);
			}
			break;

		default:
			break;
	}

	NOT_USED_PARAM (p1);
	NOT_USED_PARAM (p2);
	NOT_USED_PARAM (p3);

	return MESSAGE_PASS;
}

BUS_Result_t xmgr_schedule_MsgUpdateNcdSchedule_tva (void *pvContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR			hErr = ERR_OK;
	HUINT32			ulNumSch = 0, ulNumSch2 = 0;
	HUINT32			i;
	DxSchedule_t	*pstSchArray = NULL;
	DxSchedule_t	*pstAsoSchedule = NULL;
	HUINT32			utStartTime;
	HBOOL			isToClear = FALSE;
	HBOOL			isScheduleLoaded = FALSE;

	(void)isScheduleLoaded;
	NOT_USED_PARAM(pvContext);
	NOT_USED_PARAM(hAction);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

//#define TEST_NCD_SCHEDULE
#if defined(TEST_NCD_SCHEDULE)
	isScheduleLoaded = SAPI_IsScheduleLoaded();
	if (isScheduleLoaded == FALSE)
	{
		HxLOG_Error("Schedule is Not Loaded yet.");
		HxLOG_Assert(0);
	}
#endif

	hErr = SAPI_CountScheduleByType(DxRSVTYPE_ASO_NOTIFY, &ulNumSch);
	if ( hErr != ERR_OK )
	{
		HxLOG_Error("Sapi count ASO schedule failed. \n");
		return MESSAGE_BREAK;
	}

	utStartTime = p1;
	// 0xFFFF : xsvc_si_ncd_ukdtt
	if (utStartTime == 0xFFFF)
	{
		isToClear = TRUE;
	}

	if (isToClear == TRUE)
	{
		// case 1. clear NCD schedules
		if (ulNumSch == 0)
		{
			HxLOG_Warning("There is no NCD Schedules to clear\n ");
			return MESSAGE_BREAK;
		}

		pstSchArray = (DxSchedule_t *)OxMGR_Malloc(sizeof(DxSchedule_t) * ulNumSch);
		if ( pstSchArray == NULL )
		{
			HxLOG_Error("Malloc failed. \n");
			return MESSAGE_BREAK;
		}
		hErr = SAPI_GetAllSchedulesByRsvType(ulNumSch, &ulNumSch2, pstSchArray, DxRSVTYPE_ASO_NOTIFY);
		if ( hErr != ERR_OK )
		{
			HxLOG_Error("SAPI_GetAllSchedulesByRsvType DxRSVTYPE_ASO_NOTIFY failed. \n");
			OxMGR_Free(pstSchArray);
			return MESSAGE_BREAK;
		}

		for ( i=0; (i<ulNumSch2) && (i<ulNumSch); i++ )
		{
			pstAsoSchedule = &(pstSchArray[i]);
			HxLOG_Warning("Clear NCD Schedule. slot=(%d) \n ", pstAsoSchedule->ulSlot);
			SAPI_CancelSchedule(pstAsoSchedule->ulSlot);
		}

		OxMGR_Free(pstSchArray);
	}
	else
	{
		if (ulNumSch == 0)
		{
			// case 2. Make NCD Schedule
			HUINT32 					 ulSlot;
			DxSchedule_t				 *pstSchedule;

			hErr = SAPI_GetAvailableSlot(&ulSlot);
			if (ERR_OK != hErr)
			{
				HxLOG_Error("SAPI_GetAvailableSlot err:\n");
				return MESSAGE_BREAK;
			}

			pstSchedule = (DxSchedule_t *)OxMGR_Calloc(sizeof(DxSchedule_t));
			if (pstSchedule == NULL)
			{
				return MESSAGE_BREAK;
			}

			pstSchedule->ulSlot		= ulSlot;
			pstSchedule->eRsvType		= DxRSVTYPE_ASO_NOTIFY;
			pstSchedule->eRsvReady	= DxRSVREADY_30_SEC;
			pstSchedule->eRepeat		= DxRSVREPEAT_ONCE;
			HLIB_DATETIME_ConvertUnixTimeToDateTime(utStartTime, &(pstSchedule->stStartTime));
			HxLOG_Debug(HxANSI_COLOR_GREEN("Make Aso at %d-%d-%d %d:%d:%d \n")
											,pstSchedule->stStartTime.stDate.usYear,pstSchedule->stStartTime.stDate.ucMonth,pstSchedule->stStartTime.stDate.ucDay
											,pstSchedule->stStartTime.stTime.ucHour,pstSchedule->stStartTime.stTime.ucMinute,pstSchedule->stStartTime.stTime.ucSecond);

			hErr = SAPI_MakeSchedule (ulSlot, pstSchedule, NULL);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("SAPI_MakeSchedule err:\n");
				OxMGR_Free(pstSchedule);
				return MESSAGE_BREAK;
			}
			OxMGR_Free(pstSchedule);
		}
		else
		{
			// case 3. Update NCD Schedule
			ulNumSch = 1;	// update only one
			pstSchArray = (DxSchedule_t *)OxMGR_Malloc(sizeof(DxSchedule_t) * ulNumSch);
			if ( pstSchArray == NULL )
			{
				HxLOG_Error("Malloc failed. \n");
				return MESSAGE_BREAK;
			}
			hErr = SAPI_GetAllSchedulesByRsvType(ulNumSch, &ulNumSch2, pstSchArray, DxRSVTYPE_ASO_NOTIFY);
			if ( hErr != ERR_OK )
			{
				HxLOG_Error("SAPI_GetAllSchedulesByRsvType DxRSVTYPE_ASO_NOTIFY failed. \n");
				OxMGR_Free(pstSchArray);
				return MESSAGE_BREAK;
			}

			if (ulNumSch2 > 0)
			{
				pstAsoSchedule = &(pstSchArray[0]);
				HLIB_DATETIME_ConvertUnixTimeToDateTime(utStartTime, &(pstAsoSchedule->stStartTime));
				HxLOG_Debug(HxANSI_COLOR_GREEN("Update Aso at %d-%d-%d %d:%d:%d \n")
												,pstAsoSchedule->stStartTime.stDate.usYear,pstAsoSchedule->stStartTime.stDate.ucMonth,pstAsoSchedule->stStartTime.stDate.ucDay
												,pstAsoSchedule->stStartTime.stTime.ucHour,pstAsoSchedule->stStartTime.stTime.ucMinute,pstAsoSchedule->stStartTime.stTime.ucSecond);
				pstAsoSchedule->bRemoved = FALSE;
				pstAsoSchedule->stRemovedInfo.eRemovedReason = DxSchedule_RemovedReason_None;
				(void)SAPI_UpdateSchedule(pstAsoSchedule->ulSlot, pstAsoSchedule, NULL, TRUE, TRUE);
			}

			OxMGR_Free(pstSchArray);
		}

	}

	return MESSAGE_BREAK;
}

BUS_Result_t xmgr_schedule_MsgScheduleEventOccured_tva (void *pvContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrSch_TvAnyTimeContext_t		*pstTvAnyTimeContext = (XmgrSch_TvAnyTimeContext_t *)pvContext;
	XmgrSch_BaseContext_t		*pstBaseContext;
	HxList_t					*iter;
	HERROR						hErr;
	DxSchedule_t				*pstDummySchedule;

	pstBaseContext = &(pstTvAnyTimeContext->stBaseContext);

	if (p1 != eSAPI_NOTIFY_TYPE_SchedulesLoaded)
	{
		return MESSAGE_PASS;
	}

	pstDummySchedule = (DxSchedule_t *)OxMGR_Malloc(sizeof(DxSchedule_t));
	if (pstDummySchedule == NULL)
	{
		return MESSAGE_PASS;
	}

	// 1. running list 중에 없어진 schedule을 stop 한다.
	HxLOG_Debug("Stop erased-recording-schedule. \n");
	for (iter = HLIB_LIST_First(pstBaseContext->pstRunRecList); iter != NULL; )
	{
		XmgrSch_RunningRec_t		*pstListItem = (XmgrSch_RunningRec_t *)HLIB_LIST_Data(iter);
		iter = HLIB_LIST_Next(iter);
		if (pstListItem == NULL)
		{
			continue;
		}

		hErr = SAPI_GetSchedule(pstListItem->ulSlot, pstDummySchedule);
		if (hErr == ERR_OK)
		{
			HxLOG_Debug("schedule exists. keep recording. slot=(%d) \n",pstListItem->ulSlot);
			continue;
		}

		HxLOG_Error("schedules erased. stop recording. slot=(%d) \n",pstListItem->ulSlot);

		pstDummySchedule->ulSlot = pstListItem->ulSlot;
		(void)xmgr_schedule_StopTracking_tva(pstTvAnyTimeContext, pstDummySchedule);
		(void)xmgr_schedule_RemoveReadySchedule_tva(pstTvAnyTimeContext, pstDummySchedule);
		(void)xmgr_schedule_StopRecord_Base(pstBaseContext, pstDummySchedule);
	}

	// 2. ready list 중에 없어진 schedule을 stop 한다.
	HxLOG_Debug("Remove erased-ready-schedule. \n");
	for (iter = HLIB_LIST_First(pstTvAnyTimeContext->pstReadyRecList); iter != NULL; )
	{
		XmgrSch_ReadyRec_t *pstListItem = (XmgrSch_ReadyRec_t *)HLIB_LIST_Data(iter);
		iter = HLIB_LIST_Next(iter);
		if (pstListItem == NULL)
		{
			continue;
		}

		hErr = SAPI_GetSchedule(pstListItem->ulSlot, pstDummySchedule);
		if (hErr == ERR_OK)
		{
			HxLOG_Debug("schedule exists. keep ready. slot=(%d) \n",pstListItem->ulSlot);
			continue;
		}

		HxLOG_Error("schedules erased. stop ready. slot=(%d) \n",pstListItem->ulSlot);

		pstDummySchedule->ulSlot = pstListItem->ulSlot;
		(void)xmgr_schedule_StopTracking_tva(pstTvAnyTimeContext, pstDummySchedule);
		(void)xmgr_schedule_RemoveReadySchedule_tva(pstTvAnyTimeContext, pstDummySchedule);
	}

	// 3. tracking list 중에 없어진 schedule을 stop 한다.
	HxLOG_Debug("Stop erased-tracking-schedule. \n");
	for (iter = HLIB_LIST_First(pstTvAnyTimeContext->pstTrackingSchList); iter != NULL; )
	{
		XmgrSch_TrackingSch_t *pstListItem = (XmgrSch_TrackingSch_t *)HLIB_LIST_Data(iter);
		iter = HLIB_LIST_Next(iter);
		if (pstListItem == NULL)
		{
			continue;
		}

		hErr = SAPI_GetSchedule(pstListItem->ulSlot, pstDummySchedule);
		if (hErr == ERR_OK)
		{
			HxLOG_Debug("schedule exists. keep tracking. slot=(%d) \n",pstListItem->ulSlot);
			continue;
		}

		HxLOG_Error("schedules erased. stop tracking. slot=(%d) \n",pstListItem->ulSlot);

		pstDummySchedule->ulSlot = pstListItem->ulSlot;
		(void)xmgr_schedule_StopTracking_tva(pstTvAnyTimeContext, pstDummySchedule);
	}

	OxMGR_Free(pstDummySchedule);

	NOT_USED_PARAM(hAction);
	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return MESSAGE_PASS;
}

#define _____DEBUG_FUNCTION_____
#if defined(CONFIG_DEBUG)
#endif
