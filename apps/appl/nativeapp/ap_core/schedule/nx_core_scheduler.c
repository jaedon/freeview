/*
 * (c) 2011-2015 Humax Co., Ltd.
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

/******************************************************************************/
/**
 * @file	  		nx_core_scheduler.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2015 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

/******************************************************************************/
#define _________Private_Include______________________________________________
/******************************************************************************/

#include <nx_core_scheduler.h>
#include <nx_core_channel.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/
#define UID2SLOT(uid)				((HUINT32)(((uid) == 0) ? 0xFFFF : (uid)-1))
#define SLOT2UID(slot)				((HINT32)(((slot) >= 0xFF00) ? 0 : (slot) + 1))

/*******************************************************************/
/********************      Definitions    **************************/
/*******************************************************************/


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static NX_Scheduler_t	*g_SchedulerContext =	NULL;

/**************************************************************************************************/
#define ______________Private_prototype__________________________________________
/**************************************************************************************************/
static NX_Scheduler_t	*nx_scheduler_GetContext(void);
static void nx_scheduler_Command_Init(void);
static HERROR	nx_scheduler_SetExtWatch(DxRsvType_e eRsvType, NX_Programme_t *pstProgramme, DxService_t *pstService, DxSchedule_t *dstSchedule);
static HERROR	nx_scheduler_SetExtPower(HINT32 nLcn, HINT32 nVolume, DxSchedule_t * pstSchedule);
static HERROR	nx_scheduler_SetExtRecord(DxRsvType_e eRsvType, NX_Programme_t *pstProgramme, DxService_t *pstService, DxSchedule_t *dstSchedule);
static HERROR	nx_scheduler_SetExtStreaming(NX_Programme_t *pstProgramme, DxSchedule_t *pstSchedule);
static void		nx_scheduler_MakeConflictList(APK_META_SCH_Conflict_t *conflict);
static void 	nx_scheduler_EventListener (void *pvUserData, APK_META_SCH_MsgType_e eMsgType, HUINT32 ulSlot, DxSchedule_t *pstSchedule);


/**************************************************************************************************/
#define ______________Private_functions__________________________________________
/**************************************************************************************************/
static NX_Scheduler_t	*nx_scheduler_GetContext(void)
{
	if (g_SchedulerContext == NULL)
	{
		g_SchedulerContext = (NX_Scheduler_t*)NX_APP_Calloc(sizeof(NX_Scheduler_t));
	}
	return g_SchedulerContext;
}

static INLINE DxSCHED_REPEAT_t *nx_scheduler_ToSCHED_REPEAT (DxSCHED_REPEAT_t *repeat, DxRsvRepeat_e rsv, HUINT32 ulDays, UNIXTIME time)
{
	switch (rsv)
	{
	case DxRSVREPEAT_DAILY:
		repeat->cycle = eDxSCHED_CYCLE_DAYLY;
		repeat->date = eDxSCHED_DATE_ALL;
		break;

	case DxRSVREPEAT_WEEKLY:
		// err = HLIB_DATETIME_ConvertUnixTimeToDate(time, &date);
		// if (err != ERR_OK)
		//	return NULL;

		repeat->cycle = eDxSCHED_CYCLE_WEEKLY;
		repeat->date  = ulDays; // s_dayOfWeekMap[HLIB_DATETIME_GetWeekDayIndex(date) % 7];
		break;

	case DxRSVREPEAT_WEEKDAYS:
		repeat->cycle = eDxSCHED_CYCLE_WEEKLY;
		repeat->date  = eDxSCHED_DATE_WEEKDAYS;
		break;

	case DxRSVREPEAT_WEEKEND:
		repeat->cycle = eDxSCHED_CYCLE_WEEKLY;
		repeat->date  = eDxSCHED_DATE_WEEKEND;
		break;

	case DxRSVREPEAT_WITHOUT_SUNDAY:
		repeat->cycle = eDxSCHED_CYCLE_WEEKLY;
		repeat->date  = eDxSCHED_DATE_WEEKDAYS | eDxSCHED_DATE_SAT;
		break;

	case DxRSVREPEAT_FEWDAY_OF_WEEK:
		repeat->cycle = eDxSCHED_CYCLE_WEEKLY;
		repeat->date = ulDays;
		break;

	case DxRSVREPEAT_ONCE:
	default:
		return NULL;
	}

	NX_COMMON_NOT_USED_PARAM(time);

	return repeat;
}

static HBOOL		nx_scheduler_IsCashingType(DxRsvType_e eRsvType)
{
	if ((eRsvType == DxRSVTYPE_WATCHING_EBR)
		|| (eRsvType == DxRSVTYPE_WATCHING_TBR)
		|| (eRsvType == DxRSVTYPE_RECORD_EBR)
		|| (eRsvType == DxRSVTYPE_RECORD_TBR)
		|| (eRsvType == DxRSVTYPE_RECORD_INSTANT)
		|| (eRsvType == DxRSVTYPE_RECORD_DELAYED)
		|| (eRsvType == DxRSVTYPE_DUBBING)
		|| (eRsvType == DxRSVTYPE_DLNA_EBR)
		|| (eRsvType == DxRSVTYPE_DLNA_TBR)
		|| (eRsvType == DxRSVTYPE_DLNA_DUBBING)
		|| (eRsvType == DxRSVTYPE_NETSTREAMING_LIVE)
		|| (eRsvType == DxRSVTYPE_NETSTREAMING_FILE)
		|| (eRsvType == DxRSVTYPE_RECORD_SERIES_EBR)
		|| (eRsvType == DxRSVTYPE_RECORD_SATRECORD_TBR)
		|| (eRsvType == DxRSVTYPE_WATCHING_SERIES_EBR)
		|| (eRsvType == DxRSVTYPE_ASO_NOTIFY))
	{
		return TRUE;
	}
	return FALSE;
}

static HERROR	nx_scheduler_SetExtWatch(DxRsvType_e eRsvType, NX_Programme_t *pstProgramme, DxService_t *pstService, DxSchedule_t *dstSchedule)
{
	if (pstProgramme != NULL)
	{
		NX_METADATA_PROGRAMMES_GetName(pstProgramme, dstSchedule->uExtInfo.stWch.szEvtName);
		dstSchedule->uExtInfo.stWch.nEvtId = NX_METADATA_PROGRAMMES_GetEventID(pstProgramme);

		if (dstSchedule->eScheduleSpec == eDxSchedule_Spec_TvAnyTime)
		{
			DxEvent_t 	*pstEvent	= pstProgramme->event;
			if (pstEvent)
			{
				HINT32	len = 0, i =0;
				HBOOL			isScridExists = FALSE;
				DxEventCRID_t	*contentIdentifier = NULL;
				len = HLIB_VECTOR_Length(pstEvent->crid);
				for(i = 0; i < len; i++)
				{
					contentIdentifier = (DxEventCRID_t *)HLIB_VECTOR_ItemAt(pstEvent->crid, i);
					if (contentIdentifier == NULL)
						continue;
					HxLOG_Debug("type= 0x%x, crid = %s\n" , __FUNCTION__, __LINE__ ,i,contentIdentifier->type,contentIdentifier->crid );

					if (contentIdentifier->type == DxCRID_TYPE_SERIES)
					{
						isScridExists = TRUE;
						HxSTD_snprintf(dstSchedule->uExtInfo.stWch.ex.tva.szCRID, DxRSV_CRID_LEN + 1,"%s", contentIdentifier->crid);
						dstSchedule->uExtInfo.stWch.ex.tva.ucCRIDType = DxCRID_TYPE_SERIES;
					}
					else if (contentIdentifier->type == DxCRID_TYPE_PROG)
					{
						if (isScridExists == FALSE)
						{
							HxSTD_snprintf(dstSchedule->uExtInfo.stWch.ex.tva.szCRID, DxRSV_CRID_LEN + 1,"%s", contentIdentifier->crid);
							dstSchedule->uExtInfo.stWch.ex.tva.ucCRIDType = DxCRID_TYPE_PROG;
						}
						HxSTD_snprintf(dstSchedule->uExtInfo.stWch.ex.tva.szProgCRID, DxRSV_CRID_LEN + 1,"%s", contentIdentifier->crid);
					}
				}// end for
			}// if (pstEvent)
		}
	}
	else if(pstService != NULL)
	{
		HxSTD_snprintf(dstSchedule->uExtInfo.stWch.szEvtName, DxRSV_MAX_EVENT_NAME, "%s", pstService->name);
	}

	NX_COMMON_NOT_USED_PARAM(eRsvType);

	return ERR_OK;
}

static HERROR	nx_scheduler_SetExtPower(HINT32 nLcn, HINT32 nVolume, DxSchedule_t * pstSchedule)
{
	pstSchedule->uExtInfo.stPwr.ucVolume	= (HUINT8)nVolume;
	pstSchedule->uExtInfo.stPwr.usChNum		= (HUINT16)nLcn;

	return ERR_OK;
}

static HERROR	nx_scheduler_SetExtRecord(DxRsvType_e eRsvType, NX_Programme_t *pstProgramme, DxService_t *pstService, DxSchedule_t *dstSchedule)
{
	if (pstProgramme != NULL)
	{
		NX_METADATA_PROGRAMMES_GetName(pstProgramme, dstSchedule->uExtInfo.stRec.szEventName);
		dstSchedule->uExtInfo.stRec.nEvtId = NX_METADATA_PROGRAMMES_GetEventID(pstProgramme);
		APK_SCENARIO_GetStartPadding((HINT32 *)&dstSchedule->uExtInfo.stRec.ulPreOffsetTime);
		APK_SCENARIO_GetEndPadding((HINT32 *)&dstSchedule->uExtInfo.stRec.ulPostOffsetTime);

	#if 0
		if( APK_META_IsEnableTBRPadding() == TRUE
			&& pstSchedule->eRsvType == DxRSVTYPE_RECORD_TBR)
		{
			APK_SCENARIO_GetStartPadding((HINT32 *)&pstSchedule->uExtInfo.stRec.ulPreOffsetTime);
			APK_SCENARIO_GetEndPadding((HINT32 *)&pstSchedule->uExtInfo.stRec.ulPostOffsetTime);
		}
	#endif
		if (dstSchedule->eScheduleSpec == eDxSchedule_Spec_TvAnyTime)
		{
			DxEvent_t 	*pstEvent	= pstProgramme->event;
			if (pstEvent)
			{
				HINT32	len = 0, i =0;
				HBOOL			isScridExists = FALSE;
				DxEventCRID_t	*contentIdentifier = NULL;
				len = HLIB_VECTOR_Length(pstEvent->crid);
				for(i = 0; i < len; i++)
				{
					contentIdentifier = (DxEventCRID_t *)HLIB_VECTOR_ItemAt(pstEvent->crid, i);
					if (contentIdentifier == NULL)
						continue;
					HxLOG_Debug("type= 0x%x, crid = %s\n" , __FUNCTION__, __LINE__ ,i,contentIdentifier->type,contentIdentifier->crid );

					if (contentIdentifier->type == DxCRID_TYPE_SERIES)
					{
						isScridExists = TRUE;
						HxSTD_snprintf(dstSchedule->uExtInfo.stRec.ex.tva.szCRID, DxRSV_CRID_LEN + 1,"%s", contentIdentifier->crid);
						dstSchedule->uExtInfo.stRec.ex.tva.ucCRIDType = DxCRID_TYPE_SERIES;
					}
					else if (contentIdentifier->type == DxCRID_TYPE_PROG)
					{
						if (isScridExists == FALSE)
						{
							HxSTD_snprintf(dstSchedule->uExtInfo.stRec.ex.tva.szCRID, DxRSV_CRID_LEN + 1,"%s", contentIdentifier->crid);
							dstSchedule->uExtInfo.stRec.ex.tva.ucCRIDType = DxCRID_TYPE_PROG;
						}
						HxSTD_snprintf(dstSchedule->uExtInfo.stRec.ex.tva.szProgCRID, DxRSV_CRID_LEN + 1,"%s", contentIdentifier->crid);
					}
				}
			}
		}
		else
		{
			dstSchedule->uExtInfo.stRec.ulSeriesId = NX_METADATA_PROGRAMMES_GetSeriesID(pstProgramme);
		}
	}
	else if (pstService != NULL)
	{
		HxSTD_snprintf(dstSchedule->uExtInfo.stRec.szEventName, DxRSV_MAX_EVENT_NAME, "%s", pstService->name);

		if (APK_META_IsEnableTBRPadding() == TRUE && eRsvType == DxRSVTYPE_RECORD_TBR)
		{
			APK_SCENARIO_GetStartPadding((HINT32 *)&dstSchedule->uExtInfo.stRec.ulPreOffsetTime);
			APK_SCENARIO_GetEndPadding((HINT32 *)&dstSchedule->uExtInfo.stRec.ulPostOffsetTime);
		}
	}
	return ERR_OK;
}

static HERROR	nx_scheduler_SetExtStreaming(NX_Programme_t *pstProgramme, DxSchedule_t *pstSchedule)
{
	NX_COMMON_NOT_USED_PARAM(pstProgramme);
	NX_COMMON_NOT_USED_PARAM(pstSchedule);

	return ERR_OK;
}

static void		nx_scheduler_MakeConflictList(APK_META_SCH_Conflict_t *conflict)
{
	HERROR		hError = ERR_FAIL;
	HINT32		listLen = 0, ulSlot = -1;
	HUINT32		i = 0;
	NX_Scheduler_t *pstContext = nx_scheduler_GetContext();
	NX_ScheConflict_t *pstConflict = &pstContext->stConflict;

	if (pstConflict->conflictList == NULL)
	{
		pstConflict->conflictList = HLIB_VECTOR_NewEx(NULL, 0, (void *(*)(void *))NULL,
									(void *(*)(void *))HLIB_STD_MemFree_CB,	NULL);
	}

	listLen = HLIB_VECTOR_Length(pstConflict->conflictList);
	if (listLen > 0)
	{
		HxLOG_Debug("Remove All Scheduler Conflict list.\n");
		HLIB_VECTOR_RemoveAll(pstConflict->conflictList);
	}

	if (pstConflict->pstTriedSchedule)
	{
		NX_APP_Free(pstConflict->pstTriedSchedule);
		pstConflict->pstTriedSchedule = NULL;
	}

	pstConflict->pstTriedSchedule = (DxSchedule_t*)NX_APP_Calloc(sizeof(DxSchedule_t));
	if (pstConflict->pstTriedSchedule == NULL)
	{
		NX_APP_Error("Error!!! NX_APP_Calloc() Fail !!\n");
		return;
	}

	pstConflict->eConflictType = conflict->eType;
	HxSTD_MemCopy(pstConflict->pstTriedSchedule, &(conflict->stTriedSchedule), sizeof(DxSchedule_t));

	for (i = 0; i < conflict->ulNumConflict; i++)
	{
		DxSchedule_t	*pstDxSched = (DxSchedule_t*)NX_APP_Calloc(sizeof(DxSchedule_t));
		ulSlot = conflict->pstConfArray[i].ulSlot;
		hError = APK_META_SCHEDULE_GetSchedule(ulSlot, pstDxSched);
		if (ERR_OK != hError)
		{
			NX_APP_Free(pstDxSched);
			continue;
		}

		HxLOG_Print("\n\n\n[%s.%d] eRsvType(%d), slot(%d), uid(%d), (%04d.%02d.%02d) (%02d:%02d.%02d)\n\n\n\n",__FUNCTION__,__LINE__,
			pstDxSched->eRsvType, pstDxSched->ulSlot, pstDxSched->ulSvcUid,
			pstDxSched->stStartTime.stDate.usYear, pstDxSched->stStartTime.stDate.ucMonth, pstDxSched->stStartTime.stDate.ucDay,
			pstDxSched->stStartTime.stTime.ucHour, pstDxSched->stStartTime.stTime.ucMinute, pstDxSched->stStartTime.stTime.ucSecond);
		HLIB_VECTOR_Add(pstConflict->conflictList, (void*)pstDxSched);
	}
	pstConflict->listNum = i;
}

static void 	nx_scheduler_EventListener (void *pvUserData, APK_META_SCH_MsgType_e eMsgType, HUINT32 ulSlot, DxSchedule_t *pstSchedule)
{
	DxRsvType_e eRcvRsyType = DxRSVTYPE_NOTHING;

#if defined(CONFIG_DEBUG)
	HxLOG_Error("\nInfo. eMsgType : (%d), ulSlot (%d)\n\n",eMsgType, ulSlot);
	if(pstSchedule)
	{
		DxSchedule_t stDebugSch;

		HxLOG_Error("received: eRsvType(%d), ulSlot(%d), svcUid(%d)\n",
			pstSchedule->eRsvType, pstSchedule->ulSlot, pstSchedule->ulSvcUid);

		HxSTD_MemSet(&stDebugSch, 0, sizeof(DxSchedule_t));
		if(APK_META_SCHEDULE_GetSchedule(ulSlot, &stDebugSch) == ERR_OK)
		{
			HxLOG_Error("get schedule: eRsvType(%d), ulSlot(%d), svcUid(%d)\n",
						stDebugSch.eRsvType, stDebugSch.ulSlot, stDebugSch.ulSvcUid);
		}
	}
#endif


	switch(eMsgType)
	{
	case eAPK_METASCH_MSG_SchedulesUpdated:
		// load all schedule list
		break;
	default:
		break;
	}

	if (pstSchedule == NULL
		&& (eMsgType !=eAPK_METASCH_MSG_ConflictScheduleAdded)
		&& (eMsgType !=eAPK_METASCH_MSG_AlternativeScheduleAdded))
	{
		HxLOG_Debug("Null schedule. msgType(%d) \n",eMsgType);
		//goto END_FUNC;
	}

	switch(eMsgType)
	{
	case eAPK_METASCH_MSG_ScheduleAdded:
		ONDK_GWM_PostMessage(NULL, MSG_CORE_SCHEDULE_NOTIFY, 0, eMsgType, ulSlot, 0);
		break;

	case eAPK_METASCH_MSG_ScheduleRemoved:
		ONDK_GWM_PostMessage(NULL, MSG_CORE_SCHEDULE_NOTIFY, 0, eMsgType, ulSlot, 0);
		break;

	case eAPK_METASCH_MSG_ScheduleChanged:
		ONDK_GWM_PostMessage(NULL, MSG_CORE_SCHEDULE_NOTIFY, 0, eMsgType, ulSlot, 0);
		break;

	case eAPK_METASCH_MSG_TimeUpReady:
	case eAPK_METASCH_MSG_TimeUpStart:
	case eAPK_METASCH_MSG_TimeUpStop:
		if(pstSchedule)
		{
			if (pstSchedule->stRemovedInfo.eRemovedReason == DxSchedule_RemovedReason_CasCopyrightControlFail)
			{
				// opl 에서는 이렇게 한듯한데..
				//opl_scheduler_EmitEvent(scheduler, eOPL_SCHEDULER_EVENT_PARTIALY_COMPLETED, eOPL_SCHEDULER_ERROR_DRM_RESTRICTION, prog, NULL, NULL, NULL);
			}

			eRcvRsyType = pstSchedule->eRsvType;
		}
		ONDK_GWM_PostMessage(NULL, MSG_CORE_SCHEDULE_NOTIFY, 0, eMsgType, eRcvRsyType, ulSlot);
		break;

	case eAPK_METASCH_MSG_ConflictScheduleAdded:
		{
			APK_META_SCH_Conflict_t *pstConflict = NULL;

			pstConflict = APK_META_SCHEDULE_GetConflictResult(ulSlot);
			if (pstConflict == NULL)
			{
				HxLOG_Error("APK_META_SCHEDULE_GetConflictResult failed. slot(%d) \n",ulSlot);
				goto END_FUNC;
			}

			nx_scheduler_MakeConflictList(pstConflict);

			ONDK_GWM_PostMessage(NULL, MSG_CORE_SCHEDULE_NOTIFY, 0, eMsgType, ulSlot, 0);

			APK_META_SCHEDULE_FreeConflictResult(pstConflict);
		}
		break;

	case eAPK_METASCH_MSG_AlternativeScheduleAdded:
		break;

	default:
		break;
	}

	NX_COMMON_NOT_USED_PARAM(pvUserData);

END_FUNC:
	return ;
}

#define _______________SUPPORTED_PVR_CONFIG__________________
#if defined(CONFIG_MW_MM_PVR)
static void 	nx_scheduler_Record_EventListener (ApkMetaRec_Handle_t self, HINT32 event, const HINT32 *args, void *userdata)
{
	NX_COMMON_NOT_USED_PARAM(self);
	NX_COMMON_NOT_USED_PARAM(event);
	NX_COMMON_NOT_USED_PARAM(args);
	NX_COMMON_NOT_USED_PARAM(userdata);
}
#endif

/**************************************************************************************************/
#define _____________Public_functions____________________________________________
/**************************************************************************************************/
HERROR NX_SCHEDULER_Init(void)
{
	NX_Scheduler_t *pstContext = nx_scheduler_GetContext();

	pstContext->schedule = NX_PORT_SCHEDULE_GetInstance();

	APK_META_SCHEDULE_RegisterNotifier(nx_scheduler_EventListener, (void *)pstContext);

#if defined(CONFIG_MW_MM_PVR)
	APK_META_RECORD_EnableLoadEvent();
	APK_META_RECORD_AddNotifier(nx_scheduler_Record_EventListener, NULL);
#endif

#if defined(CONFIG_DEBUG)
	nx_scheduler_Command_Init();
#endif

	return ERR_OK;
}


NX_ScheConflict_t	*NX_SCHEDULER_GetConflictList(HUINT32 ulSlot)
{
	NX_Scheduler_t *pstContext = nx_scheduler_GetContext();

	if (pstContext == NULL)
	{
		HxLOG_Error("NX_Scheduler Context is NULL!!\n\n");
		return NULL;
	}

	NX_COMMON_NOT_USED_PARAM(ulSlot);

	return &pstContext->stConflict;
}


HERROR	NX_SCHEDULER_CancelAllScheduleByType(DxRsvType_e eRsvType)
{
	HUINT32				 ulCnt, ulNumSch = 0;

	(void)APK_META_SCHEDULE_CountScheduleByType(eRsvType, &ulNumSch);
	if (ulNumSch > 0)
	{
		DxSchedule_t		*pstSchArray = (DxSchedule_t *)NX_APP_Calloc(sizeof(DxSchedule_t) * ulNumSch);

		if (NULL != pstSchArray)
		{
			// Asynchronous 라서 Get->Remove 이렇게 하면 안 맞을 수 있다
			for (ulCnt = 0; ulCnt < ulNumSch; ulCnt++)
			{
				(void)APK_META_SCHEDULE_GetScheduleByType(eRsvType, ulCnt, &(pstSchArray[ulCnt]));
			}

			for (ulCnt = 0; ulCnt < ulNumSch; ulCnt++)
			{
				(void)APK_META_SCHEDULE_CancelSchedule(pstSchArray[ulCnt].ulSlot);
			}

			NX_APP_Free(pstSchArray);
		}

		return ERR_OK;
	}

	HxLOG_Debug(" There is no %d !\n", eRsvType);

	return ERR_OK;
}


HERROR		NX_SCHEDULER_StopAllScheduleByType(DxRsvType_e eRsvType)
{
	HUINT32				 ulCnt, ulNumSch = 0;

	(void)APK_META_SCHEDULE_CountScheduleByType(eRsvType, &ulNumSch);
	if (ulNumSch > 0)
	{
		DxSchedule_t	*pstSchArray = (DxSchedule_t *)NX_APP_Calloc(sizeof(DxSchedule_t) * ulNumSch);

		if (NULL != pstSchArray)
		{
			// Asynchronous 라서 Get->Remove 이렇게 하면 안 맞을 수 있다
			for (ulCnt = 0; ulCnt < ulNumSch; ulCnt++)
			{
				(void)APK_META_SCHEDULE_GetScheduleByType(eRsvType, ulCnt, &(pstSchArray[ulCnt]));
			}

			for (ulCnt = 0; ulCnt < ulNumSch; ulCnt++)
			{
				(void)APK_META_SCHEDULE_StopSchedule(pstSchArray[ulCnt].ulSlot);
			}

			NX_APP_Free(pstSchArray);
		}

		return ERR_OK;
	}

	HxLOG_Debug(" There is no %d !\n", eRsvType);

	return ERR_OK;
}


HERROR	NX_SCHEDULER_AddSchedule(DxSchedule_t *pstSchedule)
{
	HERROR		hError = ERR_OK;
	HUINT32		ulSlot = 0;

	if (pstSchedule == NULL)
	{
		HxLOG_Error("Schedule Item is NULL !\n");
		return ERR_FAIL;
	}

	hError = APK_META_SCHEDULE_AddSchedule(pstSchedule, &ulSlot);
	if (hError != ERR_OK)
	{
		HxLOG_Error("Cannot Add Schedule Item. ulSlot ID (%d) \n", ulSlot);
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR	NX_SCHEDULER_CancelSchedule(HUINT32 ulSlot)
{
	HERROR		hError = ERR_OK;

	hError = APK_META_SCHEDULE_CancelSchedule(ulSlot);
	if (hError != ERR_OK)
	{
		HxLOG_Error("Error!!! APK_META_SCHEDULE_CancelSchedule()- ulSlot ID (%d) \n", ulSlot);
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR	NX_SCHEDULER_StopSchedule(HUINT32 ulSlot)
{
	HERROR		hError = ERR_OK;

	hError = APK_META_SCHEDULE_StopSchedule(ulSlot);
	if (hError != ERR_OK)
	{
		HxLOG_Error("Error!!! APK_META_SCHEDULE_StopSchedule()- ulSlot ID (%d) \n", ulSlot);
		return ERR_FAIL;
	}

	return ERR_OK;
}


HERROR	NX_SCHEDULER_UpdateSchedule(DxSchedule_t *pstSchedule)
{
	HERROR		hError = ERR_OK;
	HUINT32		ulSlot = 0;

	if (pstSchedule == NULL)
	{
		HxLOG_Error("Schedule Item is NULL !\n");
		return ERR_FAIL;
	}

	ulSlot = pstSchedule->ulSlot;

	hError = APK_META_SCHEDULE_UpdateSchedule(ulSlot, pstSchedule);
	if (hError != ERR_OK)
	{
		HxLOG_Error("Cannot Update Schedule Item. ulSlot ID (%d) \n", ulSlot);
		return ERR_FAIL;
	}

	return ERR_OK;
}


HERROR	NX_SCHEDULER_CreateScheduleWithProgrammeInfo(DxRsvType_e eRsvType, NX_Programme_t *srcProgramme, DxSchedule_t *dstSchedule)
{
	DxEvent_t 			*pstEvent = NULL;
	DxService_t			*pstFindService = NULL;

	pstEvent = srcProgramme->event;

	if (pstEvent == NULL)
	{
		HxLOG_Error("DxEvent_t In NX_Programme_t is NULL!!!!\n\n");
		return ERR_FAIL;
	}

	dstSchedule->ulSlot			= 0;
	dstSchedule->eRsvType		= eRsvType;
	dstSchedule->ulSvcUid		= srcProgramme->svcuid;
	HLIB_DATETIME_ConvertUnixTimeToDateTime (NX_METADATA_PROGRAMMES_GetStartTime(srcProgramme), &(dstSchedule->stStartTime));
	dstSchedule->ulDuration		= NX_METADATA_PROGRAMMES_GetDuration(srcProgramme);
	dstSchedule->eEpgType		= NX_METADATA_PROGRAMMES_GetEpgType(srcProgramme);
	dstSchedule->eScheduleSpec	= APK_META_SCHEDULE_GetSpec();
	dstSchedule->eRsvReady		= DxRSVREADY_30_SEC;

	dstSchedule->stTripleId.usOnId	= (HUINT16)pstEvent->onid;
	dstSchedule->stTripleId.usTsId	= (HUINT16)pstEvent->tsid;
	dstSchedule->stTripleId.usSvcId	= (HUINT16)pstEvent->svcid;


	pstFindService = NX_CHANNEL_FindServiceBySvcUid(srcProgramme->svcuid);

	switch(eRsvType)
	{
	case DxRSVTYPE_WATCHING_EBR:
	case DxRSVTYPE_WATCHING_TBR:
	case DxRSVTYPE_WATCHING_SERIES_EBR:
		nx_scheduler_SetExtWatch(eRsvType, srcProgramme, pstFindService, dstSchedule);
		break;

	case DxRSVTYPE_RECORD_EBR:
	case DxRSVTYPE_RECORD_TBR:
	case DxRSVTYPE_RECORD_SERIES_EBR:
	case DxRSVTYPE_RECORD_SATRECORD_TBR:
		nx_scheduler_SetExtRecord(eRsvType, srcProgramme, pstFindService, dstSchedule);
		break;

	default:
		break;
	}

#if defined(CONFIG_DEBUG)
	{
		HxLOG_Error("\n\n===================== MYLOG Scheudler MYLOG =========================\n");
		HxLOG_Error("ulSlot (%d), eRsvType(%d), ulSvcUid(%d), eRsvReady(%d), ulDuration(%d), eRepeat(%d), ulRepeatDays(%d)\n",
				dstSchedule->ulSlot, dstSchedule->eRsvType, dstSchedule->ulSvcUid, dstSchedule->eRsvReady, dstSchedule->ulDuration,
				dstSchedule->eRepeat, dstSchedule->ulRepeatDays);
		HxLOG_Error("eStatus(%d), bExceptConflict(%d), eEpgType(%d)\n",
				dstSchedule->eStatus, dstSchedule->bExceptConflict, dstSchedule->eEpgType);
		HxLOG_Error("\n[stWch] nEvtId(%d), ulProgramId(%d), ulSeriesId(%d), szEvtName(%s)\n",
				dstSchedule->uExtInfo.stWch.nEvtId, dstSchedule->uExtInfo.stWch.ulProgramId,
				dstSchedule->uExtInfo.stWch.ulSeriesId, dstSchedule->uExtInfo.stWch.szEvtName);
		HxLOG_Error("\n[stPwr] ucVolume(%d), ucInputMode(%d), usChNum(%d)\n",
				dstSchedule->uExtInfo.stPwr.ucVolume, dstSchedule->uExtInfo.stPwr.ucInputMode, dstSchedule->uExtInfo.stPwr.usChNum);
		HxLOG_Error("\n[stRec] nEvtId(%d), ulProgramId(%d), ulSeriesId(%d), szEventName(%s)\n",
				dstSchedule->uExtInfo.stRec.nEvtId, dstSchedule->uExtInfo.stRec.ulProgramId,
				dstSchedule->uExtInfo.stRec.ulSeriesId, dstSchedule->uExtInfo.stRec.szEventName);
		HxLOG_Error("ulPreOffsetTime(%d), ulPostOffsetTime(%d), eStorage(%d)\n\n",
				dstSchedule->uExtInfo.stRec.ulPreOffsetTime, dstSchedule->uExtInfo.stRec.ulPostOffsetTime,
				dstSchedule->uExtInfo.stRec.eStorage);
		HxLOG_Error("===================== MYLOG Scheudler END =========================\n\n");
	}
#endif

	return ERR_OK;
}

HERROR	NX_SCHEDULER_CreateScheduleWithTime(DxRsvType_e eRsvType, HUINT32 ulSvcUid, HINT32 nLcn, HUINT32 ulStartTime, HUINT32 ulDuration, HINT32 nVolume, DxSchedule_t *dstSchedule)
{
	DxService_t			*pstFindService = NULL;

	pstFindService = NX_CHANNEL_FindServiceBySvcUid(ulSvcUid);

	if (pstFindService == NULL)
	{
		HxLOG_Error("Not Found DxService_t !!!\n");
		return ERR_FAIL;
	}

	dstSchedule->ulSlot	= 0;
	dstSchedule->eRsvType = eRsvType;
	dstSchedule->ulSvcUid = pstFindService->uid;

	HLIB_DATETIME_ConvertUnixTimeToDateTime (ulStartTime, &(dstSchedule->stStartTime));
	HxLOG_Debug("%04d.%02d.%02d , %02d:%02d:%02d\n",
		dstSchedule->stStartTime.stDate.usYear, dstSchedule->stStartTime.stDate.ucMonth,dstSchedule->stStartTime.stDate.ucDay,
		dstSchedule->stStartTime.stTime.ucHour, dstSchedule->stStartTime.stTime.ucMinute, dstSchedule->stStartTime.stTime.ucSecond);

	dstSchedule->ulDuration		= ulDuration;
	dstSchedule->eScheduleSpec	= APK_META_SCHEDULE_GetSpec();
	dstSchedule->eRsvReady		= DxRSVREADY_30_SEC;

	dstSchedule->stTripleId.usOnId	= (HUINT16)pstFindService->onid;
	dstSchedule->stTripleId.usTsId	= (HUINT16)pstFindService->tsid;
	dstSchedule->stTripleId.usSvcId	= (HUINT16)pstFindService->svcid;

	switch(eRsvType)
	{
	case DxRSVTYPE_WATCHING_EBR:
	case DxRSVTYPE_WATCHING_TBR:
	case DxRSVTYPE_WATCHING_SERIES_EBR:
		nx_scheduler_SetExtWatch(eRsvType, NULL, pstFindService, dstSchedule);
		break;

	case DxRSVTYPE_POWER_ON:
		nx_scheduler_SetExtPower(nLcn, nVolume, dstSchedule);
		break;

	case DxRSVTYPE_RECORD_EBR:
	case DxRSVTYPE_RECORD_TBR:
	case DxRSVTYPE_RECORD_SERIES_EBR:
	case DxRSVTYPE_RECORD_SATRECORD_TBR:
		nx_scheduler_SetExtRecord(eRsvType, NULL, pstFindService, dstSchedule);
		break;

	default:
		break;
	}

	return ERR_OK;
}

HERROR	NX_SCHEDULER_SetPowerOn(HUINT32 ulSvcUid, HUINT32 ulWakeupTime, HINT32  nRepeatDays, HINT32 nVolume)
{
	DxSchedule_t		*pstDxSched	= NULL;
	HERROR				 hErr	= ERR_FAIL;

	pstDxSched = (DxSchedule_t*)NX_APP_Calloc(sizeof(DxSchedule_t));

	if (ERR_OK != NX_SCHEDULER_CreateScheduleWithTime(DxRSVTYPE_POWER_ON, ulSvcUid, 0, ulWakeupTime, 0, nVolume, pstDxSched))
	{
		HxLOG_Error("Can't Create Schedule With Time!\n");
		goto END_FUNC;
	}

	switch (nRepeatDays)
	{
	case eDxSCHED_DATE_ALL:
		pstDxSched->eRepeat = DxRSVREPEAT_DAILY;
		break;

	case eDxSCHED_DATE_WEEKEND:
		pstDxSched->eRepeat = DxRSVREPEAT_WEEKEND;
		break;

	case eDxSCHED_DATE_WEEKDAYS:
		pstDxSched->eRepeat = DxRSVREPEAT_WEEKDAYS;
		break;

	case eDxSCHED_DATE_SUN:
	case eDxSCHED_DATE_MON:
	case eDxSCHED_DATE_TUE:
	case eDxSCHED_DATE_WED:
	case eDxSCHED_DATE_THU:
	case eDxSCHED_DATE_FRI:
	case eDxSCHED_DATE_SAT:
		pstDxSched->eRepeat = DxRSVREPEAT_WEEKLY;
		break;

	case eDxSCHED_DATE_NONE:
	default:
		pstDxSched->eRepeat = DxRSVREPEAT_ONCE;
		break;
	}

	pstDxSched->ulRepeatDays = nRepeatDays;

	{
		HxLOG_Print("\n\n\n[%s.%d] ===================== MYLOG Scheduelr =========================\n",__FUNCTION__,__LINE__);

		HxLOG_Print("ulSlot (%d), eRsvType(%d), ulSvcUid(%d), eRsvReady(%d), ulDuration(%d), eRepeat(%d), ulRepeatDays(%d)\n",
				pstDxSched->ulSlot, pstDxSched->eRsvType, pstDxSched->ulSvcUid, pstDxSched->eRsvReady, pstDxSched->ulDuration,
				pstDxSched->eRepeat, pstDxSched->ulRepeatDays);

		HxLOG_Print("eStatus(%d), bExceptConflict(%d), eEpgType(%d)\n",
				pstDxSched->eStatus, pstDxSched->bExceptConflict, pstDxSched->eEpgType);

		HxLOG_Print("\n[stWch] nEvtId(%d), ulProgramId(%d), ulSeriesId(%d), szEvtName(%s)\n",
				pstDxSched->uExtInfo.stWch.nEvtId, pstDxSched->uExtInfo.stWch.ulProgramId,
				pstDxSched->uExtInfo.stWch.ulSeriesId, pstDxSched->uExtInfo.stWch.szEvtName);

		HxLOG_Print("\n[stPwr] ucVolume(%d), ucInputMode(%d), usChNum(%d)\n",
				pstDxSched->uExtInfo.stPwr.ucVolume, pstDxSched->uExtInfo.stPwr.ucInputMode, pstDxSched->uExtInfo.stPwr.usChNum);

		HxLOG_Print("\n[stRec] nEvtId(%d), ulProgramId(%d), ulSeriesId(%d), szEventName(%s)\n",
				pstDxSched->uExtInfo.stRec.nEvtId, pstDxSched->uExtInfo.stRec.ulProgramId,
				pstDxSched->uExtInfo.stRec.ulSeriesId, pstDxSched->uExtInfo.stRec.szEventName);

		HxLOG_Print("ulPreOffsetTime(%d), ulPostOffsetTime(%d), eStorage(%d)\n\n\n",
				pstDxSched->uExtInfo.stRec.ulPreOffsetTime, pstDxSched->uExtInfo.stRec.ulPostOffsetTime,
				pstDxSched->uExtInfo.stRec.eStorage);

		HxLOG_Print("\n[%s.%d] ===================== MYLOG Scheduler END =========================\n\n\n\n",__FUNCTION__,__LINE__);
	}

	NX_SCHEDULER_CancelAllScheduleByType(DxRSVTYPE_POWER_ON);

	hErr = NX_SCHEDULER_AddSchedule (pstDxSched);
	if (ERR_OK != hErr)
	{
		HxLOG_Error (" APK_META_SCHEDULE_AddSchedule err:\n");
		goto END_FUNC;
	}

END_FUNC:

	NX_APP_Free(pstDxSched);
	pstDxSched = NULL;

	return hErr;
}

HERROR	NX_SCHEDULER_SetPowerOff(HUINT32 ulWakeupTime, HINT32  nRepeatDays)
{
	DxSchedule_t		*pstDxSched	= NULL;
	HERROR				 hErr	= ERR_FAIL;

	pstDxSched = (DxSchedule_t*)NX_APP_Calloc(sizeof(DxSchedule_t));

	pstDxSched->ulSlot	= 0;
	pstDxSched->eRsvType = DxRSVTYPE_POWER_OFF;

	HLIB_DATETIME_ConvertUnixTimeToDateTime (ulWakeupTime, &(pstDxSched->stStartTime));
	HxLOG_Debug("%04d.%02d.%02d , %02d:%02d:%02d\n",
		pstDxSched->stStartTime.stDate.usYear, pstDxSched->stStartTime.stDate.ucMonth,pstDxSched->stStartTime.stDate.ucDay,
		pstDxSched->stStartTime.stTime.ucHour, pstDxSched->stStartTime.stTime.ucMinute, pstDxSched->stStartTime.stTime.ucSecond);

	pstDxSched->eScheduleSpec	= APK_META_SCHEDULE_GetSpec();
	pstDxSched->eRsvReady		= DxRSVREADY_30_SEC;

	switch (nRepeatDays)
	{
	case eDxSCHED_DATE_ALL:
		pstDxSched->eRepeat = DxRSVREPEAT_DAILY;
		break;

	case eDxSCHED_DATE_WEEKEND:
		pstDxSched->eRepeat = DxRSVREPEAT_WEEKEND;
		break;

	case eDxSCHED_DATE_WEEKDAYS:
		pstDxSched->eRepeat = DxRSVREPEAT_WEEKDAYS;
		break;

	case eDxSCHED_DATE_SUN:
	case eDxSCHED_DATE_MON:
	case eDxSCHED_DATE_TUE:
	case eDxSCHED_DATE_WED:
	case eDxSCHED_DATE_THU:
	case eDxSCHED_DATE_FRI:
	case eDxSCHED_DATE_SAT:
		pstDxSched->eRepeat = DxRSVREPEAT_WEEKLY;
		break;

	case eDxSCHED_DATE_NONE:
	default:
		pstDxSched->eRepeat = DxRSVREPEAT_ONCE;
		break;
	}

	pstDxSched->ulRepeatDays = nRepeatDays;

	NX_SCHEDULER_CancelAllScheduleByType(DxRSVTYPE_POWER_ON);

	hErr = NX_SCHEDULER_AddSchedule (pstDxSched);
	if (ERR_OK != hErr)
	{
		HxLOG_Error (" APK_META_SCHEDULE_AddSchedule err:\n");

	}

	NX_APP_Free(pstDxSched);
	pstDxSched = NULL;

	return hErr;
}


HERROR	NX_SCHEDULER_GetDuration(DxSchedule_t *pstSchedule, HUINT32 *ulDuration)
{
	UNIXTIME 		unixtime = 0;

	*ulDuration = 0;
	if (pstSchedule == NULL)
	{
		NX_APP_Error("Error!! pstSchedule == NULL \n");
		return	ERR_FAIL;
	}

	HLIB_DATETIME_ConvertDateTimeToUnixTime(&pstSchedule->stStartTime, &unixtime);
	unixtime += pstSchedule->ulDuration;
	*ulDuration = unixtime;

	return	ERR_OK;
}


HERROR	NX_SCHEDULER_GetDurationWithDataTime(DxSchedule_t *pstSchedule, HxDATETIME_t *pstDateTime)
{
	UNIXTIME 		unixtime = 0;

	if ((pstSchedule == NULL) || (pstDateTime == NULL))
	{
		NX_APP_Error("Error!! pstSchedule == NULL \n");
		return	ERR_FAIL;
	}

	HLIB_DATETIME_ConvertDateTimeToUnixTime(&pstSchedule->stStartTime, &unixtime);
	unixtime += pstSchedule->ulDuration;

	HxSTD_memset(pstDateTime, 0x00, sizeof(HxDATETIME_t));
	HLIB_DATETIME_ConvertUnixTimeToDateTime(unixtime, pstDateTime);

	return	ERR_OK;
}


HBOOL	NX_SCHEDULER_IsSeries(DxSchedule_t *pstSchedule, HUINT32 *ulSeriesId)
{
	HBOOL	bIsSeries = FALSE;

	if (pstSchedule == NULL)
	{
		NX_APP_Error("Error!! pstSchedule == NULL \n");
		return	FALSE;
	}

	NX_APP_Info("eRsvType :(0x%x)\n", pstSchedule->eRsvType);
	*ulSeriesId = 0;
	switch (pstSchedule->eRsvType)
	{
		case DxRSVTYPE_WATCHING_EBR:
		//case DxRSVTYPE_WATCHING_TBR:
			bIsSeries = (pstSchedule->uExtInfo.stWch.ulSeriesId > 0) ? TRUE : FALSE;
			*ulSeriesId = pstSchedule->uExtInfo.stWch.ulSeriesId;
			break;

		case DxRSVTYPE_RECORD_SERIES_EBR:
		case DxRSVTYPE_RECORD_EBR:
		//case DxRSVTYPE_RECORD_TBR:
			bIsSeries = (pstSchedule->uExtInfo.stRec.ulSeriesId > 0) ? TRUE : FALSE;
			*ulSeriesId = pstSchedule->uExtInfo.stRec.ulSeriesId;
			break;
		default:
			bIsSeries = FALSE;
			break;
	}

	NX_APP_Info("bIsSeries :(%d)\n", bIsSeries);

	return bIsSeries;
}


HERROR	NX_SCHEDULER_GetSeriveUidBySlotID(HUINT32 ulSlotId, HUINT32 *pulSvcUid)
{
	HERROR			hError = ERR_FAIL;
	DxSchedule_t 	*pstSchedule = NULL;

	if (pulSvcUid == NULL)
	{
		NX_APP_Error("Error!! pulSvcUid == NULL \n");
		return ERR_FAIL;
	}

	pstSchedule = NX_APP_Calloc(sizeof(DxSchedule_t));
	if (pstSchedule == NULL)
	{
		NX_APP_Error("ERROR!!! NX_APP_Calloc() Fail !!\n");
		return	ERR_FAIL;
	}

	hError = NX_SCHEDULER_GetScheduleBySlotID(ulSlotId, pstSchedule);
	if (hError != ERR_OK)
	{
		if (pstSchedule)
		{
			NX_APP_Free(pstSchedule);
			pstSchedule = NULL;
		}
		*pulSvcUid = 0;
		NX_APP_Error("ERROR!!! NX_SCHEDULER_GetScheduleBySlotID() - ulSlotId:(0x%x) Fail !!\n", ulSlotId);
		return	ERR_FAIL;
	}

	*pulSvcUid = pstSchedule->ulSvcUid;

	if (pstSchedule)
	{
		NX_APP_Free(pstSchedule);
		pstSchedule = NULL;
	}

	return ERR_OK;
}


HERROR	NX_SCHEDULER_GetEventIDBySlotID(HUINT32 ulSlotId, HINT32 *plEventId)
{
	HERROR			hError = ERR_FAIL;
	DxSchedule_t 	*pstSchedule = NULL;

	if (plEventId == NULL)
	{
		NX_APP_Error("Error!! plEventId == NULL \n");
		return ERR_FAIL;
	}

	pstSchedule = NX_APP_Calloc(sizeof(DxSchedule_t));
	if (pstSchedule == NULL)
	{
		NX_APP_Error("ERROR!!! NX_APP_Calloc() Fail !!\n");
		return	ERR_FAIL;
	}

	hError = NX_SCHEDULER_GetScheduleBySlotID(ulSlotId, pstSchedule);
	if (hError != ERR_OK)
	{
		if (pstSchedule)
		{
			NX_APP_Free(pstSchedule);
			pstSchedule = NULL;
		}
		*plEventId = 0;
		NX_APP_Error("ERROR!!! NX_SCHEDULER_GetScheduleBySlotID() - ulSlotId:(0x%x) Fail !!\n", ulSlotId);
		return	ERR_FAIL;
	}

	switch (pstSchedule->eRsvType)
	{
		case DxRSVTYPE_WATCHING_EBR:
		case DxRSVTYPE_WATCHING_SERIES_EBR:
			*plEventId = pstSchedule->uExtInfo.stWch.nEvtId;
			break;
		case DxRSVTYPE_RECORD_EBR:
		case DxRSVTYPE_RECORD_SERIES_EBR:
			*plEventId = pstSchedule->uExtInfo.stRec.nEvtId;
			break;
		default:
			*plEventId = 0;
			break;
	}

	if (pstSchedule)
	{
		NX_APP_Free(pstSchedule);
		pstSchedule = NULL;
	}

	return ERR_OK;
}


HERROR		NX_SCHEDULER_CountScheduleAll(HUINT32 *pulNumSch)
{
	HERROR	hError = ERR_FAIL;

	if (pulNumSch == NULL)
	{
		NX_APP_Error("Error!! pulNumSch == NULL \n");
		return ERR_FAIL;
	}

	hError = APK_META_SCHEDULE_CountSchedule(pulNumSch);

	return hError;
}


HERROR		NX_SCHEDULER_CountSchedule(HUINT32 *pulNumSch)
{
	HINT32			lIndex = 0;
	HUINT32 		i = 0, ulLength = 0;
	NX_Scheduler_t	*pstContext = nx_scheduler_GetContext();
	DxSchedule_t	stDxSched;
	HERROR			hError = ERR_FAIL;

	*pulNumSch = 0;

	if (pstContext == NULL)
	{
		NX_APP_Error("Error!!! Schedule data is NULL....!!\n");
		return	ERR_FAIL;
	}

	(void)APK_META_SCHEDULE_CountSchedule(&ulLength);

	for (i = 0; i < ulLength; i++)
	{
		HxSTD_memset(&stDxSched, 0x00, sizeof(stDxSched));
		hError = APK_META_SCHEDULE_GetScheduleByIndex(i, &stDxSched);
		if (ERR_OK != hError)
		{
			continue;
		}

		switch (stDxSched.eRsvType)
		{
			case DxRSVTYPE_WATCHING_EBR:
			case DxRSVTYPE_WATCHING_TBR:
			case DxRSVTYPE_WATCHING_SERIES_EBR:
			case DxRSVTYPE_RECORD_EBR:
			case DxRSVTYPE_RECORD_TBR:
			case DxRSVTYPE_RECORD_SERIES_EBR:
			case DxRSVTYPE_RECORD_SATRECORD_TBR:
				lIndex++;
				break;
			case DxRSVTYPE_NOTHING:
			case DxRSVTYPE_POWER_ON:
			case DxRSVTYPE_POWER_OFF:
			case DxRSVTYPE_REGULAR_OTA:
			case DxRSVTYPE_EPG_GATHERING:
			case DxRSVTYPE_EMM:
			case DxRSVTYPE_UPDATE_TASK:
			case DxRSVTYPE_ASO_NOTIFY:
			case DxRSVTYPE_MYEPG_UPDATE:
			case DxRSVTYPE_NET_UPDATE:
			case DxRSVTYPE_BLURAY_TBR:
			case DxRSVTYPE_BLURAY_EBR:
			case DxRSVTYPE_RECORD_INSTANT:
			case DxRSVTYPE_RECORD_DELAYED:
			case DxRSVTYPE_KEYWORD_RECORD_NOTSURE:
			case DxRSVTYPE_CAS_POWER_CTRL:
			case DxRSVTYPE_LOGO_DOWN:
			case DxRSVTYPE_RESCAN:
			case DxRSVTYPE_CAS_TEL:
			case DxRSVTYPE_CHANNEL_MASK:
			case DxRSVTYPE_DUBBING:
			case DxRSVTYPE_OTA_BD:
			case DxRSVTYPE_IP_EPG_GATHERING:
			case DxRSVTYPE_IP_REMOTE_PROGRAMMING:
			case DxRSVTYPE_DLNA_EBR:
			case DxRSVTYPE_DLNA_TBR:
			case DxRSVTYPE_DLNA_DUBBING:
			case DxRSVTYPE_NETSTREAMING_LIVE:
			case DxRSVTYPE_NETSTREAMING_FILE:
			case DxRSVTYPE_ACTIVATION_TIME:
			case DxRSVTYPE_DESC_REC_STANDBY:
			case DxRSVTYPE_CIPLUS_OP_TUNE:
			case DxRSVTYPE_CHECK_SATRECORD_TIMER:
			case DxRSVTYPE_DYNAMIC_UPDATE:
			case DxRSVTYPE_EndOfCase:
				break;
			default:
				break;
		}
	}
	*pulNumSch = lIndex;

	return ERR_OK;
}


HERROR	NX_SCHEDULER_CountScheduleByType(DxRsvType_e eType, HUINT32 *pulNumSch)
{
	HINT32			lIndex = 0;
	HUINT32			i = 0, ulLength = 0;
	NX_Scheduler_t	*pstContext = nx_scheduler_GetContext();
	DxSchedule_t	stDxSched;
	HERROR	hErr = ERR_FAIL;

	if (pulNumSch == NULL)
	{
		NX_APP_Error("Error!! pulNumSch == NULL \n");
		return ERR_FAIL;
	}

	*pulNumSch = 0;

	if (pstContext == NULL)
	{
		NX_APP_Error("Error!!! Schedule data is NULL....!!\n");
		return	ERR_FAIL;
	}

	(void)APK_META_SCHEDULE_CountSchedule(&ulLength);

	for (i = 0; i < ulLength; i++)
	{
		hErr = APK_META_SCHEDULE_GetScheduleByIndex(i, &stDxSched);
		if (ERR_OK != hErr)			{	continue; 	}

		if (stDxSched.eRsvType == eType)
			lIndex++;
	}
	*pulNumSch = lIndex;

	return ERR_OK;
}

HERROR	NX_SCHEDULER_GetScheduleBySlotID(HUINT32 ulSlotId, DxSchedule_t *pstSchedule)
{
	return APK_META_SCHEDULE_GetSchedule(ulSlotId, pstSchedule);
}

HERROR	NX_SCHEDULER_GetScheduleList(HxVector_t *stScheduleList, HINT32 *lNum)
{
	HINT32			lIndex = 0;
	HUINT32 		i = 0, ulLength = 0;
	NX_Scheduler_t	*pstContext = nx_scheduler_GetContext();
	DxSchedule_t	stDxSched;
	HERROR			hError = ERR_FAIL;

	*lNum = 0;

	if (pstContext == NULL)
	{
		NX_APP_Error("Error!!! Schedule data is NULL....!!\n");
		return	ERR_FAIL;
	}

	(void)APK_META_SCHEDULE_CountSchedule(&ulLength);

	for (i = 0; i < ulLength; i++)
	{
		HxSTD_memset(&stDxSched, 0x00, sizeof(stDxSched));
		hError = APK_META_SCHEDULE_GetScheduleByIndex(i, &stDxSched);
		if (ERR_OK != hError)
		{
			continue;
		}

		switch (stDxSched.eRsvType)
		{
			case DxRSVTYPE_WATCHING_EBR:
			case DxRSVTYPE_WATCHING_TBR:
			case DxRSVTYPE_WATCHING_SERIES_EBR:
			case DxRSVTYPE_RECORD_EBR:
			case DxRSVTYPE_RECORD_TBR:
			case DxRSVTYPE_RECORD_SERIES_EBR:
			case DxRSVTYPE_RECORD_SATRECORD_TBR:
				HLIB_VECTOR_Add(stScheduleList, HLIB_MEM_MemDup(&stDxSched, sizeof(DxSchedule_t)));
				lIndex++;
				break;
			case DxRSVTYPE_NOTHING:
			case DxRSVTYPE_POWER_ON:
			case DxRSVTYPE_POWER_OFF:
			case DxRSVTYPE_REGULAR_OTA:
			case DxRSVTYPE_EPG_GATHERING:
			case DxRSVTYPE_EMM:
			case DxRSVTYPE_UPDATE_TASK:
			case DxRSVTYPE_ASO_NOTIFY:
			case DxRSVTYPE_MYEPG_UPDATE:
			case DxRSVTYPE_NET_UPDATE:
			case DxRSVTYPE_BLURAY_TBR:
			case DxRSVTYPE_BLURAY_EBR:
			case DxRSVTYPE_RECORD_INSTANT:
			case DxRSVTYPE_RECORD_DELAYED:
			case DxRSVTYPE_KEYWORD_RECORD_NOTSURE:
			case DxRSVTYPE_CAS_POWER_CTRL:
			case DxRSVTYPE_LOGO_DOWN:
			case DxRSVTYPE_RESCAN:
			case DxRSVTYPE_CAS_TEL:
			case DxRSVTYPE_CHANNEL_MASK:
			case DxRSVTYPE_DUBBING:
			case DxRSVTYPE_OTA_BD:
			case DxRSVTYPE_IP_EPG_GATHERING:
			case DxRSVTYPE_IP_REMOTE_PROGRAMMING:
			case DxRSVTYPE_DLNA_EBR:
			case DxRSVTYPE_DLNA_TBR:
			case DxRSVTYPE_DLNA_DUBBING:
			case DxRSVTYPE_NETSTREAMING_LIVE:
			case DxRSVTYPE_NETSTREAMING_FILE:
			case DxRSVTYPE_ACTIVATION_TIME:
			case DxRSVTYPE_DESC_REC_STANDBY:
			case DxRSVTYPE_CIPLUS_OP_TUNE:
			case DxRSVTYPE_CHECK_SATRECORD_TIMER:
			case DxRSVTYPE_DYNAMIC_UPDATE:
			case DxRSVTYPE_EndOfCase:
				break;
			default:
				break;
		}
	}
	*lNum = lIndex;

	return ERR_OK;
}


HERROR	NX_SCHEDULER_GetScheduleListByType(DxRsvType_e eType, HxVector_t *stScheduleList, HINT32 *lNum)
{
	HINT32			lIndex = 0;
	HUINT32			i = 0, ulLength = 0;
	NX_Scheduler_t	*pstContext = nx_scheduler_GetContext();
	DxSchedule_t	stDxSched;
	HERROR			hErr = ERR_FAIL;

	*lNum = 0;

	if (pstContext == NULL)
	{
		NX_APP_Error("Error!!! Schedule data is NULL....!!\n");
		return	ERR_FAIL;
	}

	(void)APK_META_SCHEDULE_CountSchedule(&ulLength);

	for (i = 0; i < ulLength; i++)
	{
		hErr = APK_META_SCHEDULE_GetScheduleByIndex(i, &stDxSched);
		if (ERR_OK != hErr)			{	continue; 	}

		if (stDxSched.eRsvType == eType)
		{
			HLIB_VECTOR_Add(stScheduleList, HLIB_MEM_MemDup(&stDxSched, sizeof(DxSchedule_t)));
			lIndex++;
		}
	}
	*lNum = lIndex;

	return ERR_OK;
}


HERROR	NX_SCHEDULER_GetScheduleListBySeriesIdAndServiceUid(HUINT32 ulSeriesId, HUINT32 ulSvcUid, HxVector_t *stScheduleList, HINT32 *lNum)
{
	HINT32			lIndex = 0;
	HUINT32			i = 0, ulLength = 0;
	HBOOL			bIsSeries = FALSE;
	HUINT32			ulFindSeriesId = 0;
	NX_Scheduler_t	*pstContext = nx_scheduler_GetContext();
	DxSchedule_t	stDxSched;
	HERROR			hErr = ERR_FAIL;


	*lNum = 0;

	if (pstContext == NULL)
	{
		NX_APP_Error("Error!!! Schedule data is NULL....!!\n");
		return	ERR_FAIL;
	}

	if (ulSeriesId == 0)
	{
		NX_APP_Error("Error!!! Pls Check Series Id!!\n");
		return	ERR_FAIL;
	}

	(void)APK_META_SCHEDULE_CountSchedule(&ulLength);

	for (i = 0; i < ulLength; i++)
	{
		hErr = APK_META_SCHEDULE_GetScheduleByIndex(i, &stDxSched);
		if (ERR_OK != hErr) 		{	continue;	}

		if (stDxSched.eStatus == DxRSVSTATUS_RUNNING)
		{

			continue;
		}

		if (stDxSched.ulSvcUid == ulSvcUid)
		{
			bIsSeries = NX_SCHEDULER_IsSeries(&stDxSched, &ulFindSeriesId);
			if ((bIsSeries == TRUE) && (ulSeriesId == ulFindSeriesId))
			{
				if (stScheduleList)
					HLIB_VECTOR_Add(stScheduleList, HLIB_MEM_MemDup(&stDxSched, sizeof(DxSchedule_t)));
			}
			lIndex++;
		}
	}
	*lNum = lIndex;

	return ERR_OK;
}


HERROR	NX_SCHEDULER_GetScheduleListByProgrammeInfo(NX_Programme_t *pstProgramme, HxVector_t *pstScheduleList, HINT32 *lNum)
{
	HERROR			hErr = ERR_FAIL;
	HINT32			lIndex = 0, lEvendId = -1;
	HUINT32 		i = 0, ulLength = 0;
	NX_Scheduler_t	*pstContext = nx_scheduler_GetContext();
	DxSchedule_t	stDxSched;


	*lNum = 0;

	if (pstContext == NULL)
	{
		NX_APP_Error("Error!!! Schedule data is NULL....!!\n");
		return	ERR_FAIL;
	}

	if (pstProgramme == NULL)
	{
		NX_APP_Error("Error!!! Pls Check pstProgramme is NULL !!\n");
		return	ERR_FAIL;
	}

	(void)APK_META_SCHEDULE_CountSchedule(&ulLength);

	for (i = 0; i < ulLength; i++)
	{
		hErr = APK_META_SCHEDULE_GetScheduleByIndex(i, &stDxSched);
		if (ERR_OK != hErr) 		{	continue;	}

		if (stDxSched.eStatus == DxRSVSTATUS_RUNNING)
		{
			continue;
		}

		if (stDxSched.ulSvcUid == pstProgramme->svcuid)
		{
			switch (stDxSched.eRsvType)
			{
				case DxRSVTYPE_WATCHING_EBR:
				case DxRSVTYPE_WATCHING_TBR:
				case DxRSVTYPE_WATCHING_SERIES_EBR:
					lEvendId = stDxSched.uExtInfo.stWch.nEvtId;
					break;

				case DxRSVTYPE_RECORD_EBR:
				case DxRSVTYPE_RECORD_TBR:
				case DxRSVTYPE_RECORD_SERIES_EBR:
				case DxRSVTYPE_RECORD_SATRECORD_TBR:
					lEvendId = stDxSched.uExtInfo.stRec.nEvtId;
					break;

				default:
					lEvendId = -1;
					break;
			}

			if (lEvendId == pstProgramme->event->eventId)
			{
				if (pstScheduleList)
				{
					HLIB_VECTOR_Add(pstScheduleList, HLIB_MEM_MemDup(&stDxSched, sizeof(DxSchedule_t)));
				}
			}
			lIndex++;
		}
	}

	*lNum = lIndex;

	return ERR_OK;
}


#define ______________NX_SCHEDULER_COMMAND_______________
static HINT32 cmdScheduler_Refresh(void *szArgs)
{
	NX_COMMON_NOT_USED_PARAM(szArgs);
//	NX_Scheduler_t *pstContext = nx_scheduler_GetContext();
	return 0;
}

static HERROR cmdScheduler_MakeRemind(void *pvArg)
{
	HCHAR			*aucArg=NULL;
	HUINT32			uUID = 0;
	HUINT32			ulStartTime =0;
	HUINT32			ulDuration =0;
	DxRsvType_e		eRsvType=0;
	DxSchedule_t		stSchedule = {0, };

	aucArg = (HCHAR *)HLIB_CMD_GetNextArg((HCHAR**)&pvArg);
	if( aucArg == NULL )
		return HxCMD_ERR;
	if( sscanf(aucArg, "%d", &uUID) == 0 )
		return HxCMD_ERR;

	aucArg = (HCHAR *)HLIB_CMD_GetNextArg((HCHAR**)&pvArg);
	if( aucArg == NULL )
		return HxCMD_ERR;
	if( sscanf(aucArg, "%d", (int*)&eRsvType) == 0 )
		return HxCMD_ERR;

	aucArg = (HCHAR *)HLIB_CMD_GetNextArg((HCHAR**)&pvArg);
	if( aucArg == NULL )
		return HxCMD_ERR;
	if( sscanf(aucArg, "%d", &ulStartTime) == 0 )
		return HxCMD_ERR;

	aucArg = (HCHAR *)HLIB_CMD_GetNextArg((HCHAR**)&pvArg);
	if( aucArg == NULL )
		return HxCMD_ERR;
	if( sscanf(aucArg, "%d", &ulDuration) == 0 )
		return HxCMD_ERR;

	HxLOG_Print("\n\n\n[%s.%d] uUid : %d,  rsyType : %d , startTime : %d,  duration : %d\n\n\n",__FUNCTION__,__LINE__,
		uUID, eRsvType, ulStartTime, ulDuration);

	stSchedule.eRsvType			= eRsvType;
	stSchedule.ulSvcUid			= uUID;
	stSchedule.eRsvReady		= DxRSVREADY_30_SEC;
	(void)HLIB_DATETIME_ConvertUnixTimeToDateTime(ulStartTime, &(stSchedule.stStartTime));
	stSchedule.ulDuration		= ulDuration;
	stSchedule.eRepeat			= DxRSVREPEAT_ONCE;
	stSchedule.ulRepeatDays		= 0;
	stSchedule.eStatus			= DxRSVSTATUS_WAITING;
	stSchedule.bExceptConflict	= FALSE;
	stSchedule.eEpgType			= eDxEPG_TYPE_SC;
	stSchedule.uExtInfo.stRec.ulPreOffsetTime = 180;

	return HxCMD_ERR;
}

static HINT32 cmdScheduler_PowerTimer(void *pvArg)
{
	HCHAR			*aucArg=NULL;
	HUINT32			uUID = 0;
	HUINT32			ulStartTime =0;
	HINT32			nRepeatDay = 0;
	HUINT32			ulVolume =0;

	aucArg = (HCHAR *)HLIB_CMD_GetNextArg((HCHAR**)&pvArg);
	if( aucArg == NULL )
		return HxCMD_ERR;
	if( sscanf(aucArg, "%d", &uUID) == 0 )
		return HxCMD_ERR;

	aucArg = (HCHAR *)HLIB_CMD_GetNextArg((HCHAR**)&pvArg);
	if( aucArg == NULL )
		return HxCMD_ERR;
	if( sscanf(aucArg, "%d", &ulStartTime) == 0 )
		return HxCMD_ERR;

	aucArg = (HCHAR *)HLIB_CMD_GetNextArg((HCHAR**)&pvArg);
	if( aucArg == NULL )
		return HxCMD_ERR;
	if( sscanf(aucArg, "%d", &nRepeatDay) == 0 )
		return HxCMD_ERR;

	aucArg = (HCHAR *)HLIB_CMD_GetNextArg((HCHAR**)&pvArg);
	if( aucArg == NULL )
		return HxCMD_ERR;
	if( sscanf(aucArg, "%d", (int*)&ulVolume) == 0 )
		return HxCMD_ERR;

	HxLOG_Print("\n\n\n[%s.%d] uUid : %d,  nRepeatDay : %d , startTime : %d,  ulVolume : %d\n\n\n",__FUNCTION__,__LINE__,
		uUID, nRepeatDay, ulStartTime, ulVolume);

	NX_SCHEDULER_SetPowerOn(uUID, ulStartTime, (HINT32)eDxSCHED_DATE_ALL, ulVolume);

	return HxCMD_ERR;
}


static void nx_scheduler_Command_Init(void)
{
#define cmdHandle		"nxschedulercmd"

	HLIB_CMD_RegisterWord(cmdHandle, cmdScheduler_Refresh,
						 (HINT8 *)"refresh",
						 (HINT8 *)"refresh schedule list",
						 (HINT8 *)"refresh");

	HLIB_CMD_RegisterWord(cmdHandle, cmdScheduler_PowerTimer,
						 (HINT8 *)"powertimer",
						 (HINT8 *)"powertimer svcuid , starttime , nRepeatDay, volume",
						 (HINT8 *)"powertimer");
#if 0
	HLIB_CMD_RegisterWord(cmdHandle, cmdScheduler_PrintScheduleList,
						(HINT8 *)"print",
						(HINT8 *)"print schedule list",
						(HINt8 *)"print sch");
#endif
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

