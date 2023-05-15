/**************************************************************************************/
/**
 * @file oplscheduler.c
 *
 *
 *
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#include "oplscheduler.h"
#include "oplrecorditem.h"
#include "oplprogramme.h"

#include <hlib.h>
#include <apk.h>
#include <time.h>


#define UID2SLOT(uid)				((HUINT32)(((uid) == 0) ? 0xFFFF : (uid)-1))
#define SLOT2UID(slot)				((HINT32)(((slot) >= 0xFF00) ? 0 : (slot) + 1))

#undef  return_if
#define	return_if(expr, err)	do{if(expr){HxLOG_Debug(" %s failed!!\n", #expr);return (err);}}while(0)

typedef struct
{
	ApkMetaSched_Handle_t	schedule;

	HxVector_t	*scheduledList;
	char		tempBuf[256];

	OPL_Scheduler_EventListener_t	listener;
#ifdef CONFIG_OP_JAPAN
	OPL_Scheduler_HNReservationFinishedEventListener_t	hnlistener;
#endif
} OPL_Scheduler_t;

#define	____PRIVATE_OPL_Scheduler__________________________________________________________

#define ____CONVERT_FUNCTIONS_____

static OPL_SchedulerState	opl_scheduler_GetOplState (APK_META_SCH_MsgType_e eMsgType)
{
	OPL_SchedulerState eState = eOPL_SCHEDULER_EVENT_UNREALIZED;
	switch (eMsgType)
	{
		case eAPK_METASCH_MSG_SchedulesUpdated:
			eState = eOPL_SCHEDULER_EVENT_LOAD;
			break;
		case eAPK_METASCH_MSG_ScheduleAdded:
			eState = eOPL_SCHEDULER_EVENT_SCHEDULED;
			break;
		case eAPK_METASCH_MSG_ScheduleChanged:
			eState = eOPL_SCHEDULER_EVENT_UPDATED;
			break;
		case eAPK_METASCH_MSG_ScheduleRemoved:
			eState = eOPL_SCHEDULER_EVENT_DELETED;
			break;
		case eAPK_METASCH_MSG_TimeUpReady:
			eState = eOPL_SCHEDULER_EVENT_PRESTART;
			break;
		case eAPK_METASCH_MSG_TimeUpStart:
			eState = eOPL_SCHEDULER_EVENT_STARTED;
			break;
		case eAPK_METASCH_MSG_TimeUpStop:
			eState = eOPL_SCHEDULER_EVENT_UNREALIZED;
			break;
		case eAPK_METASCH_MSG_ConflictScheduleAdded:
			eState = eOPL_SCHEDULER_EVENT_ERROR;
			break;
		case eAPK_METASCH_MSG_ConflictInReady:
			eState = eOPL_SCHEDULER_EVENT_START_ERROR;
			break;
		case eAPK_METASCH_MSG_AlternativeScheduleAdded:
			eState = eOPL_SCHEDULER_EVENT_ERROR;
			break;
		default:
			break;
	}

	return eState;
}

static INLINE DxRsvType_e opl_scheduler_GetRsvType_v2 (OPL_ScheduledItem_t *pstOplScheduled)
{
	HBOOL bEventBased = FALSE;

	if (pstOplScheduled->programmeID != NULL)
	{
		bEventBased = TRUE;
	}
	switch (pstOplScheduled->type)
	{
	case eOPL_SCHEDULED_PRO_WATCH:
		return bEventBased ? (pstOplScheduled->isScheduledAsSeries ? DxRSVTYPE_WATCHING_SERIES_EBR : DxRSVTYPE_WATCHING_EBR)  : DxRSVTYPE_WATCHING_TBR;

	case eOPL_SCHEDULED_PRO_RECORD:
		return bEventBased ? (pstOplScheduled->isScheduledAsSeries ? DxRSVTYPE_RECORD_SERIES_EBR : DxRSVTYPE_RECORD_EBR) : (DxRSVTYPE_RECORD_TBR);

	case eOPL_SCHEDULED_PRO_POWER_ON_TIMER:
		return DxRSVTYPE_POWER_ON;

	case eOPL_SCHEDULED_PRO_NETSTREAMING_LIVE:
		return DxRSVTYPE_NETSTREAMING_LIVE;

	case eOPL_SCHEDULED_PRO_NETSTREAMING_FILE:
		return DxRSVTYPE_NETSTREAMING_FILE;

	case eOPL_SCHEDULED_PRO_DLNA:
		return bEventBased ? DxRSVTYPE_DLNA_EBR: DxRSVTYPE_DLNA_TBR;

	case eOPL_SCHEDULED_PRO_DUBBING:
		return DxRSVTYPE_DLNA_DUBBING;

	case eOPL_SCHEDULED_PRO_SAT_RECORD:
		return DxRSVTYPE_RECORD_SATRECORD_TBR;

	case eOPL_SCHEDULED_PRO_REGULAR_OTA:
		return DxRSVTYPE_REGULAR_OTA;

	case eOPL_SCHEDULED_PRO_ASO_NOTIFY:
		return DxRSVTYPE_ASO_NOTIFY;

	default:
		break;
	}


	return DxRSVTYPE_NOTHING;
}

#define _____________________
static HBOOL		opl_scheduler_IsCashingType(DxRsvType_e eRsvType)
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
		|| (eRsvType == DxRSVTYPE_ASO_NOTIFY)
		|| (eRsvType == DxRSVTYPE_POWER_ON))
	{
		return TRUE;
	}
	return FALSE;
}

static HBOOL		opl_scheduler_EmitEvent (OPL_Scheduler_t *self, int state, int error
								, OPL_ScheduledItem_t *sched, OPL_Collection_t *conflict, OPL_Collection_t *alternative, OPL_Collection_t *altevent)
{
	HxLOG_Trace();
#if defined (CONFIG_DEBUG)
	HxLOG_Warning("opl_scheduler_EmitEvent send state = %d\n", state);
#endif
	if (self->listener)
	{	// scheduler_EventListener
		return self->listener(state, error, sched, conflict, alternative, altevent) ? TRUE : FALSE;
	}
	return FALSE;
}

#ifdef CONFIG_OP_JAPAN
static HBOOL		opl_scheduler_EmitHNReservationFinishedEvent (OPL_Scheduler_t *self, OPL_ScheduledItem_t *sched, int error)
{
	HxLOG_Trace();

	if (self->hnlistener)
	{	// scheduler_EventListener
		return self->hnlistener(sched, error) ? TRUE : FALSE;
	}
	return FALSE;
}
#endif

static HBOOL	opl_scheduler_CompareItemUID (void * data, void * userdata)
{
	OPL_ScheduledItem_t *prog = (OPL_ScheduledItem_t *)data;

	if (prog->uid == (int)userdata)
		return TRUE;
	return FALSE;
}

STATIC HERROR opl_scheduler_CCID2TripleID (const char *ccid, DxSchedule_TripleId_t *pstTripleId)
{
	HCHAR	buf[64];
	HCHAR	*token, *save;
	HUINT8	nCnt = 0;

	HxLOG_Trace();
	HxLOG_Assert(ccid);

	HxLOG_Debug("ccid: %s \n", ccid);

	if (!HLIB_STD_StrStartWith((const HCHAR *)ccid, "ccid:"))
	{
		return ERR_FAIL;
	}

	HLIB_STD_StrNCpySafe(buf, (const HCHAR *)ccid + 5, 64);

	// uniqueId.onId.tsId.svcId

	token = HxSTD_StrTok(buf, ".", &save);
	while (token && nCnt < 3)
	{
		token = HxSTD_StrTok(NULL, ".", &save);

		if (token == NULL)
		{
			HxLOG_Error("token is NULL \n");
			return ERR_FAIL;
		}
		HxLOG_Debug("token: %s \n", token);
		if (!HLIB_STD_IsInteger(token))
		{
			HxLOG_Error("token is not a integer \n");
			return ERR_FAIL;
		}

		if(nCnt == 0)
		{
			pstTripleId->usOnId = (int)HLIB_STD_StrToINT32(token);
		}
		else if(nCnt == 1)
		{
			pstTripleId->usTsId = (int)HLIB_STD_StrToINT32(token);
		}
		else if(nCnt == 2)
		{
			pstTripleId->usSvcId = (int)HLIB_STD_StrToINT32(token);
		}

		nCnt++;

	}

	HxLOG_Debug("onId: %d, tsId: %d, svcId: %d \n", pstTripleId->usOnId, pstTripleId->usTsId, pstTripleId->usSvcId);


	return ERR_OK;
}



static DxSchedule_t *
			opl_scheduler_ToDlibSchedule (DxSchedule_t *sched, const OPL_ScheduledItem_t *prog)
{
	int i =0;
	memset(sched, 0, sizeof(DxSchedule_t));

	sched->ulSlot = UID2SLOT(prog->uid);
	HLIB_DATETIME_ConvertUnixTimeToDateTime (prog->startTime, &(sched->stStartTime));
	sched->ulDuration  = prog->duration;
	sched->eEpgType  = prog->epgType;

	sched->eScheduleSpec = APK_META_SCHEDULE_GetSpec();
	sched->eRsvType		 = opl_scheduler_GetRsvType_v2(prog);
	sched->eRsvReady	= DxRSVREADY_30_SEC;
	switch (prog->type)
	{
	case eOPL_SCHEDULED_PRO_WATCH:
		sched->ulSvcUid = OPL_Channel_CCID2UniqueID(prog->channelID);
		(void)opl_scheduler_CCID2TripleID(prog->channelID, &(sched->stTripleId));
		if (prog->name != NULL)
		{
			HxSTD_snprintf(sched->uExtInfo.stWch.szEvtName,DxRSV_MAX_EVENT_NAME,"%s",prog->name);
		}
		if (prog->programmeID != NULL)
		{
			sched->uExtInfo.stWch.nEvtId = OPL_Programme_GetEventIDFromProgrammeID(prog->programmeID);
		}
		if (sched->eScheduleSpec == eDxSchedule_Spec_TvAnyTime)
		{
			if ( prog->crid )
			{
				HBOOL			isScridExists = FALSE;
				OPL_ScheduledTvAnyTimetCRID_t	*crid;
				for (i=0; i < OPL_Collection_Length(prog->crid); i++)
				{
					crid = (OPL_ScheduledTvAnyTimetCRID_t *)OPL_Collection_ItemAt(prog->crid,i);
					if (crid == NULL)
					{
						HxLOG_Debug("Null crid. \n");
						continue;
					}

				 	if (crid->ucCRIDType == DxCRID_TYPE_SERIES)
				 	{
						isScridExists = TRUE;
						HxSTD_snprintf(sched->uExtInfo.stWch.ex.tva.szCRID,DxRSV_CRID_LEN + 1,"%s",crid->szCRID);
						sched->uExtInfo.stWch.ex.tva.ucCRIDType = DxCRID_TYPE_SERIES;
				 	}
					else if (crid->ucCRIDType == DxCRID_TYPE_PROG)
					{
						if (isScridExists == FALSE)
						{
							HxSTD_snprintf(sched->uExtInfo.stWch.ex.tva.szCRID,DxRSV_CRID_LEN + 1,"%s",crid->szCRID);
							sched->uExtInfo.stWch.ex.tva.ucCRIDType = DxCRID_TYPE_PROG;
						}
						HxSTD_snprintf(sched->uExtInfo.stWch.ex.tva.szProgCRID,DxRSV_CRID_LEN + 1,"%s",crid->szCRID);
					}

				}
			}
		}
		break;
	case eOPL_SCHEDULED_PRO_POWER_ON_TIMER:
		sched->ulSvcUid = OPL_Channel_CCID2UniqueID(prog->channelID);
		(void)opl_scheduler_CCID2TripleID(prog->channelID, &(sched->stTripleId));
		sched->uExtInfo.stPwr.ucVolume = prog->volume;
		//sched->param.power.onOff = TRUE;
		sched->uExtInfo.stPwr.usChNum = prog->lcn;
		break;
	case eOPL_SCHEDULED_PRO_NETSTREAMING_LIVE:
	case eOPL_SCHEDULED_PRO_NETSTREAMING_FILE:
		sched->ulSvcUid = OPL_Channel_CCID2UniqueID(prog->channelID);
		(void)opl_scheduler_CCID2TripleID(prog->channelID, &(sched->stTripleId));
		if (prog->name != NULL)
		{
			HxSTD_snprintf(sched->uExtInfo.stStrm.szAppDisplayString,DxRSV_APPDISPLAYSTRING_LEN,"%s",prog->name);
		}
		break;

	case eOPL_SCHEDULED_PRO_SAT_RECORD:
	case eOPL_SCHEDULED_PRO_RECORD:
		sched->ulSvcUid = OPL_Channel_CCID2UniqueID(prog->channelID);
		(void)opl_scheduler_CCID2TripleID(prog->channelID, &(sched->stTripleId));

		if (prog->name != NULL)
		{
			HxSTD_snprintf(sched->uExtInfo.stRec.szEventName,DxRSV_MAX_EVENT_NAME,"%s",prog->name);
		}
		if (prog->programmeID != NULL)
		{
			sched->uExtInfo.stRec.nEvtId = OPL_Programme_GetEventIDFromProgrammeID(prog->programmeID);
			sched->uExtInfo.stRec.ulPreOffsetTime = prog->startPadding;
			sched->uExtInfo.stRec.ulPostOffsetTime = prog->endPadding;
		}
		if(APK_META_IsEnableTBRPadding()==TRUE
			&&sched->eRsvType == DxRSVTYPE_RECORD_TBR)
		{
			sched->uExtInfo.stRec.ulPreOffsetTime = prog->startPadding;
			sched->uExtInfo.stRec.ulPostOffsetTime = prog->endPadding;
		}

		if (prog->storageID)
		{
			HxSTD_snprintf(sched->uExtInfo.stRec.aucStorageUUID,DxRSV_MAX_UUID_BUFSIZE,"%s",prog->storageID);
		}

		if (sched->eScheduleSpec == eDxSchedule_Spec_TvAnyTime)
		{
			if(prog->crid)
			{
				HBOOL			isScridExists = FALSE;
				OPL_ScheduledTvAnyTimetCRID_t	*crid;
				for (i=0; i < OPL_Collection_Length(prog->crid); i++)
				{
					crid = (OPL_ScheduledTvAnyTimetCRID_t *)OPL_Collection_ItemAt(prog->crid,i);
					if (crid == NULL)
					{
						continue;
					}
				 	if (crid->ucCRIDType == DxCRID_TYPE_SERIES)
				 	{
                        isScridExists = TRUE;
						HxSTD_snprintf(sched->uExtInfo.stRec.ex.tva.szCRID,DxRSV_CRID_LEN + 1,"%s",crid->szCRID);
						sched->uExtInfo.stRec.ex.tva.ucCRIDType = DxCRID_TYPE_SERIES;

				 	}
					else if (crid->ucCRIDType == DxCRID_TYPE_PROG)
					{
						if (isScridExists == FALSE)
						{
							HxSTD_snprintf(sched->uExtInfo.stRec.ex.tva.szCRID,DxRSV_CRID_LEN + 1,"%s",crid->szCRID);
							sched->uExtInfo.stRec.ex.tva.ucCRIDType = DxCRID_TYPE_PROG;
						}
						HxSTD_snprintf(sched->uExtInfo.stRec.ex.tva.szProgCRID,DxRSV_CRID_LEN + 1,"%s",crid->szCRID);
					}
				}
			}
		}
		else
		{
			if(prog->seriesID && prog->isScheduledAsSeries)
			{
				sched->uExtInfo.stRec.ulSeriesId = atoi(prog->seriesID);
			}
		}

		break;

	case eOPL_SCHEDULED_PRO_DUBBING:
	case eOPL_SCHEDULED_PRO_DLNA:
		sched->ulSvcUid = OPL_Channel_CCID2UniqueID(prog->channelID);
		(void)opl_scheduler_CCID2TripleID(prog->channelID, &(sched->stTripleId));
		break;

	case eOPL_SCHEDULED_PRO_ASO_NOTIFY:
		break;

	default:
		return NULL;
	}

	if (prog->repeatDays)
	{
		if (prog->repeatDays  == eDxSCHED_DATE_ALL)
		{
			sched->eRepeat = DxRSVREPEAT_DAILY;
		}
		else if (prog->repeatDays == eDxSCHED_DATE_WEEKDAYS)
		{
			sched->eRepeat = DxRSVREPEAT_WEEKDAYS;
		}
		/*
		DO not use weekend
		else if (prog->repeatDays  == eDxSCHED_DATE_WEEKEND)
		{
			sched->eRepeat = DxRSVREPEAT_WEEKEND;
		}
		*/
		else if (prog->repeatDays == eDxSCHED_DATE_SUN
				||prog->repeatDays == eDxSCHED_DATE_MON
				||prog->repeatDays == eDxSCHED_DATE_TUE
				||prog->repeatDays == eDxSCHED_DATE_WED
				||prog->repeatDays == eDxSCHED_DATE_THU
				||prog->repeatDays == eDxSCHED_DATE_FRI
				||prog->repeatDays == eDxSCHED_DATE_SAT)
		{
			sched->eRepeat = DxRSVREPEAT_WEEKLY;
		}
		else
		{
			sched->eRepeat = DxRSVREPEAT_FEWDAY_OF_WEEK;
		}
		sched->ulRepeatDays = prog->repeatDays;
	}
	else
	{
		sched->eRepeat = DxRSVREPEAT_ONCE;
	}


	return sched;
}

static HERROR	opl_scheduler_LoadScheduledList (OPL_Scheduler_t *self)
{
	HINT32			count;
	HINT32			i, n = 0;
//	DxSchedule_t		 stDxSched;
	OPL_ScheduledItem_t	*item;
	HERROR				 hErr;

	HxLOG_Trace();

        if (self && self->scheduledList)
        {
                HLIB_VECTOR_RemoveAll(self->scheduledList);

        	(void)APK_META_SCHEDULE_CountSchedule((HUINT32 *)&n);
        	for (i = 0, count = 0 ; i < n ; i++)
        	{
        		DxSchedule_t		 stDxSched;

        		hErr = APK_META_SCHEDULE_GetScheduleByIndex(i, &stDxSched);
        		if (ERR_OK != hErr)					{ continue; }

        		if (!opl_scheduler_IsCashingType(stDxSched.eRsvType))
        		{
        			HxLOG_Debug("Not caching type. slot(%d) type(%d) \n",stDxSched.ulSlot,stDxSched.eRsvType);
        			continue;
        		}

        		item = OPL_ScheduledItem_New("o2", &stDxSched);
        		if (item != NULL)
        		{
        			HLIB_VECTOR_Add(self->scheduledList, (void *)item);
        		}
         	}
        }

	return ERR_OK;
}

static HERROR	opl_scheduler_MakeAlternative_v2(APK_META_SCH_Alternative_t *alternative, OPL_Collection_t * *altList, OPL_Collection_t * *altevtList)
{
	HUINT32 i;
	HUINT32 ulSlot;
	HERROR	hErr;

	// make alternative list
	if (altList != NULL)
	{
		OPL_ScheduledItem_t *item=NULL;
		DxSchedule_t		stDxSched;
		OPL_Collection_t	*list;

		list = OPL_Collection_NewEx((void(*)(void *))OPL_ScheduledItem_Delete, (void *(*)(void *))OPL_ScheduledItem_Clone);
		if (list == NULL)
		{
			HxLOG_Error("OPL_Collection_NewEx failed. \n");
			goto END_FUNC;
		}

		for (i = 0 ; i < alternative->stAltItems.ulNumAlternatives ; i++)
		{
			ulSlot = alternative->stAltItems.astAltItemArray[i].ulSlot;
			item   = NULL;
			if(ulSlot == alternative->stConfResult.stTriedSchedule.ulSlot)	// tried schedule has alternative event
			{
				item  = OPL_ScheduledItem_New("o2", &alternative->stConfResult.stTriedSchedule);
			}
			else
			{
				hErr = APK_META_SCHEDULE_GetSchedule(ulSlot, &stDxSched);
				if (hErr != ERR_OK)
				{
					continue;
				}
				item  = OPL_ScheduledItem_New("o2", &stDxSched);
			}

			if (item == NULL)
				continue;

			OPL_Collection_Add(list, (void *)item);
		}
		*altList = list;
	}

	// make alternative programme list
	if (altevtList)
	{
//		OPL_ScheduledItem_t *item=NULL;
//		DxSchedule_t		stDxSched;
		OPL_Collection_t	*list;
		HINT32				svcuid;
		HINT32	eventid;
		DxEvent_t			*event;
		OPL_ProgrammeHandle programme = NULL;

		list = OPL_Collection_NewEx((void(*)(void *))OPL_Programme_Delete, (void *(*)(void *))OPL_Programme_Clone);
		if (list == NULL)
		{
			HxLOG_Error("OPL_Collection_NewEx failed. \n");
			goto END_FUNC;
		}

		for (i = 0 ; i < alternative->stAltItems.ulNumAlternatives; i++)
		{
#ifndef CONFIG_FUNC_EPG_USE_SVCUID
			OPL_Channel_t	stChannel;
#endif
			svcuid	= alternative->stAltItems.astAltItemArray[i].ulAltSvcUid;
			eventid = alternative->stAltItems.astAltItemArray[i].ulAltEvtId;
#ifndef CONFIG_FUNC_EPG_USE_SVCUID
			stChannel = OPL_Channel_FindChannelByUID (NULL, svcuid);
			event = APK_META_QUERY_GetEventWithIds(OPL_Channel_GetOrgNetworkID (stChannel), OPL_Channel_GetTransponderID (stChannel), OPL_Channel_GetServiceID (stChannel), eventid);
#else
			event = APK_META_QUERY_GetEventWithIds(svcuid, eventid);
#endif
			programme = OPL_Programme_New("o", APK_EVENT_Incref(event));

			if(programme == NULL)
				continue;
			OPL_Collection_Add(list, (void *)programme);
		}

		*altevtList = list;
	}

END_FUNC:
	return ERR_OK;
}

static HERROR
		opl_scheduler_MakeConflict_v2 (const APK_META_SCH_Conflict_t *conflict, int *error, OPL_Collection_t **conflictList)
{
//	HUINT32 			 i;
	HUINT32 			 ulSlot;
	OPL_Collection_t	*list;
	DxSchedule_t		 stDxSched;
	HERROR				 hErr;
	HUINT32 			j;
	OPL_ScheduledItem_t *item;

	HxLOG_Trace();

	if (conflictList == NULL)
	{
		HxLOG_Error("Null param. \n");
		goto END_FUNC;
	}

	list = OPL_Collection_NewEx((void(*)(void *))OPL_ScheduledItem_Delete, (void *(*)(void *))OPL_ScheduledItem_Clone);
	if (list == NULL)
	{
		HxLOG_Error("OPL_Collection_NewEx failed. \n");
		goto END_FUNC;
	}

	// make error
	if (error != NULL)
	{
		switch(conflict->eType)
		{
		case eAPK_METASCH_CONFTYPE_Tp:
			*error = eOPL_SCHEDULER_ERROR_TUNER_CONFLICT;
			break;

		case eAPK_METASCH_CONFTYPE_OverCapacity:
			*error = eOPL_SCHEDULER_ERROR_RESOURCE_LIMITATION;
			break;

		case eAPK_METASCH_CONFTYPE_Alternative:
			*error = eOPL_SCHEDULER_ERROR_HAVE_ALTERNATIVE;
			break;

		case eAPK_METASCH_CONFTYPE_RfOta:
			*error = eOPL_SCHEDULER_ERROR_RFOTA;
			break;

		case eAPK_METASCH_CONFTYPE_Duplicated:
		case eAPK_METASCH_CONFTYPE_View:
		case eAPK_METASCH_CONFTYPE_PaddingTime:
		case eAPK_METASCH_CONFTYPE_Unknown:
		default:
			*error = eOPL_SCHEDULER_ERROR_UNKNOWN;
			break;
		}
	}

	// make conflict list
	for (j = 0; j < conflict->ulNumConflict; j++)
	{
		ulSlot = conflict->pstConfArray[j].ulSlot;
		hErr = APK_META_SCHEDULE_GetSchedule(ulSlot, &stDxSched);
		if (ERR_OK != hErr)				{ continue; }

		item = OPL_ScheduledItem_New("o2", &stDxSched);
		if (item == NULL)
		{
			continue;
		}
		OPL_Collection_Add(list, (void *)item);
	}

	*conflictList = list;

END_FUNC:
	return ERR_OK;
}

static OPL_ScheduledItem_t *
		opl_scheduler_MakeSchedule (OPL_Scheduler_t *self, OPL_ScheduledItem_t *item, OPL_ReservationTryType type)
{
	HINT32					uid;
	HUINT32					ulSlot = 0;
	DxSchedule_t			stDxSched;
	HERROR					hErr;

	HxLOG_Trace();

	if (item == NULL)
	{
		HxLOG_Error(" Invalid Parameter!!\n");
		return NULL;
	}

	if (opl_scheduler_ToDlibSchedule(&stDxSched, item) == NULL)
	{
		HxLOG_Error(" Cannot convert to appkit type...!!\n");
		OPL_Scheduler_ScheduleDelete(item);
		return NULL;
	}

	switch (type)
	{
		case eOPL_SCHEDULER_TRY_TYPE_NORMAL:
			break;
		case eOPL_SCHEDULER_TRY_TYPE_FORCE:
			if (stDxSched.eScheduleSpec == eDxSchedule_Spec_TvAnyTime)
			{
				{
					DxRSV_TvAnyTimeRecord_t *pstTvAnyTime = NULL;
					if (item->type == eOPL_SCHEDULED_PRO_WATCH)
					{
						pstTvAnyTime = &stDxSched.uExtInfo.stWch.ex.tva;
					}
					else if (item->type == eOPL_SCHEDULED_PRO_RECORD)
					{
						pstTvAnyTime = &stDxSched.uExtInfo.stRec.ex.tva;
					}

					if (pstTvAnyTime != NULL)
					{
						pstTvAnyTime->ucRecKind |= 0x10; //SAMA_SERIES_CONTENT_KIND_ALTINST
					}
				}
			}
			break;
		case eOPL_SCHEDULER_TRY_TYPE_CRID:
			if (stDxSched.eScheduleSpec == eDxSchedule_Spec_TvAnyTime)
			{
				stDxSched.eRsvType = item->isScheduledAsSeries ? DxRSVTYPE_RECORD_SERIES_EBR : DxRSVTYPE_RECORD_EBR;
				stDxSched.uExtInfo.stRec.ex.tva.ucRecKind |= 0x40; //SAMA_SERIES_CONTENT_KIND_CRID
			}
			break;
	}

	hErr = APK_META_SCHEDULE_AddSchedule(&stDxSched, &ulSlot);
	uid = SLOT2UID(ulSlot);

	if(uid ==0){
		HxLOG_Debug(" Cannot Add Schedule item...!!\n");
		OPL_Scheduler_ScheduleDelete(item);
		return NULL;
	}

	return item;
}



STATIC void opl_scheduler_EventListener_v2 (void *pvUserData, APK_META_SCH_MsgType_e eMsgType, HUINT32 ulSlot, DxSchedule_t *pstSchedule)
{
	OPL_Scheduler_t	*scheduler = (OPL_Scheduler_t *)pvUserData;
	OPL_ScheduledItem_t *prog = NULL;
	OPL_SchedulerState	eState;

	APK_META_SCH_Conflict_t *pstConflict = NULL;
	APK_META_SCH_Alternative_t *pstAlternative = NULL;
	OPL_Collection_t		*conflictList = NULL;
	OPL_Collection_t		*altlist = NULL;
	OPL_Collection_t		*altevtlist = NULL;
	OPL_SchedulerError		eError;

	HUINT32					ulRemoved;

	HxLOG_Debug("slot(%d) type(%d) \n",ulSlot, eMsgType);
	// pre process (cache management, error event)
	switch (eMsgType)
	{
		case eAPK_METASCH_MSG_SchedulesUpdated:
			opl_scheduler_LoadScheduledList(scheduler);
			prog = OPL_ScheduledItem_NewWithDummy();
			if (prog == NULL)
			{
				HxLOG_Debug("fail to make opl schedule. type(%d) slot(%d) \n",pstSchedule->eRsvType, pstSchedule->ulSlot);
				goto END_FUNC;
			}
			eState = opl_scheduler_GetOplState(eMsgType);
			opl_scheduler_EmitEvent(scheduler, eState, eOPL_SCHEDULER_ERROR_NONE, prog, NULL, NULL, NULL);
			break;
		default:
			break;
	}

	if (pstSchedule == NULL
		&& (eMsgType !=eAPK_METASCH_MSG_ConflictScheduleAdded)
		&& (eMsgType !=eAPK_METASCH_MSG_AlternativeScheduleAdded))
	{
		HxLOG_Debug("Null schedule. msgType(%d) \n",eMsgType);
		goto END_FUNC;
	}

	switch (eMsgType)
	{
		case eAPK_METASCH_MSG_ScheduleAdded:
			prog = OPL_ScheduledItem_New("o2", pstSchedule);
			if (prog != NULL)
			{
				HLIB_VECTOR_Add(scheduler->scheduledList, (void *)prog);
				prog = NULL;
			}
			break;
		case eAPK_METASCH_MSG_ScheduleRemoved:
			prog = (OPL_ScheduledItem_t *)HLIB_VECTOR_Find(scheduler->scheduledList, opl_scheduler_CompareItemUID, (void *)SLOT2UID(ulSlot));
			if (prog != NULL)
			{
				if (pstSchedule->stRemovedInfo.eRemovedReason == DxSchedule_RemovedReason_CasCopyrightControlFail)
				{
					opl_scheduler_EmitEvent(scheduler, eOPL_SCHEDULER_EVENT_PARTIALY_COMPLETED, eOPL_SCHEDULER_ERROR_DRM_RESTRICTION, prog, NULL, NULL, NULL);
				}
				else if (pstSchedule->stRemovedInfo.eRemovedReason == DxSchedule_RemovedReason_HddFull)
				{
					opl_scheduler_EmitEvent(scheduler, eOPL_SCHEDULER_EVENT_ERROR, eOPL_SCHEDULER_ERROR_INSUFFICIENT_STORAGE, prog, NULL, NULL, NULL);
				}
				else if (pstSchedule->stRemovedInfo.eRemovedReason == DxSchedule_RemovedReason_SatipFailed)
				{
					opl_scheduler_EmitEvent(scheduler, eOPL_SCHEDULER_EVENT_ERROR, eOPL_SCHEDULER_ERROR_SATIP_FAILED, prog, NULL, NULL, NULL);
				}
				else if (pstSchedule->stRemovedInfo.eRemovedReason == DxSchedule_RemovedReason_MaxRecList)
				{
					opl_scheduler_EmitEvent(scheduler, eOPL_SCHEDULER_EVENT_ERROR, eOPL_SCHEDULER_ERROR_MAX_RECORD_LIST, prog, NULL, NULL, NULL);
				}
				prog = NULL;
			}
			ulRemoved = HLIB_VECTOR_RemoveIf(scheduler->scheduledList,opl_scheduler_CompareItemUID, (void *)SLOT2UID(ulSlot));
			if (ulRemoved == 0)
			{
				HxLOG_Debug("fail to remove slot(%d) \n",ulSlot);
			}
			break;
		case eAPK_METASCH_MSG_ScheduleChanged:
			ulRemoved = HLIB_VECTOR_RemoveIf(scheduler->scheduledList,opl_scheduler_CompareItemUID, (void *)SLOT2UID(ulSlot));
			if (ulRemoved == 0)
			{
				HxLOG_Debug("fail to remove slot(%d) \n",ulSlot);
			}

			prog = OPL_ScheduledItem_New("o2", pstSchedule);
			if (prog != NULL)
			{
				HLIB_VECTOR_Add(scheduler->scheduledList, (void *)prog);
				prog = NULL;
			}
			break;
		case eAPK_METASCH_MSG_TimeUpReady:
		case eAPK_METASCH_MSG_TimeUpStart:
		case eAPK_METASCH_MSG_TimeUpStop:
			prog = (OPL_ScheduledItem_t *)HLIB_VECTOR_Find(scheduler->scheduledList, opl_scheduler_CompareItemUID, (void *)SLOT2UID(ulSlot));
			if (prog != NULL)
			{
				prog->status = pstSchedule->eStatus;

				if (pstSchedule->stRemovedInfo.eRemovedReason == DxSchedule_RemovedReason_CasCopyrightControlFail)
				{
					opl_scheduler_EmitEvent(scheduler, eOPL_SCHEDULER_EVENT_PARTIALY_COMPLETED, eOPL_SCHEDULER_ERROR_DRM_RESTRICTION, prog, NULL, NULL, NULL);
				}
				prog = NULL;
			}

		default:
			break;
	}

	// process
	if (eMsgType == eAPK_METASCH_MSG_ConflictScheduleAdded
		|| eMsgType == eAPK_METASCH_MSG_AlternativeScheduleAdded)
	{
		eState = opl_scheduler_GetOplState(eMsgType);
		if (eMsgType == eAPK_METASCH_MSG_ConflictScheduleAdded)
		{
			pstConflict = APK_META_SCHEDULE_GetConflictResult(ulSlot);
			if (pstConflict == NULL)
			{
				HxLOG_Error("APK_META_SCHEDULE_GetConflictResult failed. slot(%d) \n",ulSlot);
				goto END_FUNC;
			}

			prog = OPL_ScheduledItem_New("o2", &pstConflict->stTriedSchedule);
			if (prog == NULL)
			{
				HxLOG_Debug("fail to make opl schedule. type(%d) slot(%d) \n",pstConflict->stTriedSchedule.eRsvType, pstConflict->stTriedSchedule.ulSlot);
				goto END_FUNC;
			}

			opl_scheduler_MakeConflict_v2(pstConflict, &eError, &conflictList);

			opl_scheduler_EmitEvent(scheduler, eState, eError, prog, conflictList, NULL, NULL);
		}
		else if (eMsgType == eAPK_METASCH_MSG_AlternativeScheduleAdded)
		{
			pstAlternative = APK_META_SCHEDULE_GetAlternativeResult(ulSlot);
			if (pstAlternative == NULL)
			{
				HxLOG_Error("APK_META_SCHEDULE_GetAlternativeResult failed. slot(%d) \n",ulSlot);
				goto END_FUNC;
			}

			prog = OPL_ScheduledItem_New("o2", &pstAlternative->stConfResult.stTriedSchedule);
			if (prog == NULL)
			{
				HxLOG_Debug("fail to make opl schedule. type(%d) slot(%d) \n",pstAlternative->stConfResult.stTriedSchedule.eRsvType, pstAlternative->stConfResult.stTriedSchedule.ulSlot);
				goto END_FUNC;
			}

			opl_scheduler_MakeConflict_v2(&pstAlternative->stConfResult, &eError, &conflictList);
			opl_scheduler_MakeAlternative_v2(pstAlternative, &altlist, &altevtlist);

			opl_scheduler_EmitEvent(scheduler, eState, eError, prog, conflictList, altlist, altevtlist);
		}
	}
	else
	{
		prog = OPL_ScheduledItem_New("o2", pstSchedule);
		if (prog == NULL)
		{
			HxLOG_Debug("fail to make opl schedule. type(%d) slot(%d) \n",pstSchedule->eRsvType, pstSchedule->ulSlot);
			goto END_FUNC;
		}

		eState = opl_scheduler_GetOplState(eMsgType);
		opl_scheduler_EmitEvent(scheduler, eState, eOPL_SCHEDULER_ERROR_NONE, prog, NULL, NULL, NULL);
	}

END_FUNC:
	if (conflictList != NULL)
	{
		OPL_Collection_Delete(conflictList);
	}
	if (altlist != NULL)
	{
		OPL_Collection_Delete(altlist);
	}
	if (altevtlist != NULL)
	{
		OPL_Collection_Delete(altevtlist);
	}
	if (prog != NULL)
	{
		OPL_ScheduledItem_Delete(prog);
	}
	if (pstConflict != NULL)
	{
		APK_META_SCHEDULE_FreeConflictResult(pstConflict);
	}
	if (pstAlternative != NULL)
	{
		APK_META_SCHEDULE_FreeAlternativeResult(pstAlternative);
	}
	return;
}


// return reclist count to hint for the remove success ? or not
static	int		opl_scheduler_Recording_Remove(OPL_Recording_t	*pstRecording)
{
	HERROR							hErr		= ERR_FAIL;
	APK_META_REC_DELETE_t	stDelete	= {0};


	if (NULL == pstRecording)
	{
		return -1;
	}

	if (NULL == pstRecording->id)
	{
		return -1;
	}

	stDelete.ulParamCount	= 1;
	stDelete.paramList		= (APK_META_REC_PARAM_t*)HLIB_STD_MemCalloc(sizeof(APK_META_REC_PARAM_t) * stDelete.ulParamCount);
	if (stDelete.paramList)
	{
		HxLOG_Print(" id (%s)\n", pstRecording->id);

		HLIB_STD_StrUtf8NCpy(stDelete.paramList[0].szUrl, pstRecording->id, DxRECLIST_URL);
		hErr = APK_META_RECORD_DeleteAsync(&stDelete);
		HLIB_STD_MemFree(stDelete.paramList);
	}

	return ERR_FAIL;
}

static	int 	opl_scheduler_Recording_Update(OPL_Recording_t	*pstNewRecording)
{
	OPL_Recording_t	orgRecording;
	DxRecListData_t *pstOrgRecData 	= NULL;
	APK_META_REC_UPDATE_t	stUpdate	= {0};
	HERROR	hErr = ERR_FAIL;
	UNIXTIME		utCurrTime;

	// compare the new recording with org recording
	if (NULL == pstNewRecording)
	{
		HxLOG_Error(" NULL recording param\n");
		return ERR_FAIL;
	}
	pstOrgRecData = APK_META_RECORD_GetByUrl(pstNewRecording->id);
	if (NULL == pstOrgRecData)
	{
		HxLOG_Error(" can't find APK RECData with id (%s)\n", pstNewRecording->id);
		return ERR_FAIL;
	}

	// get the updatable values -doNotDelete, saveDays, saveEpisodes. (isManualLocked : hms1000s)
	OPL_Recording_GetUpdatableData(&orgRecording, (OPL_HANDLE)pstOrgRecData);

	stUpdate.ulParamCount	= 1;
	stUpdate.paramList		= (APK_META_REC_UPDATE_PARAM_t*)HLIB_STD_MemCalloc(sizeof(APK_META_REC_UPDATE_PARAM_t)
		* stUpdate.ulParamCount);
	// use the local static memory value (if extension data needed then change as pointer and clone & release)
	HxSTD_MemCopy(&stUpdate.paramList[0].recUpdateData, pstOrgRecData, sizeof(DxRecListData_t));
	stUpdate.paramList[0].recUpdateData.ulExtEvtCnt = 0;
	stUpdate.paramList[0].recUpdateData.pstExtEvts	= NULL;
	APK_META_RECORD_Release(pstOrgRecData);

	HLIB_STD_StrUtf8NCpy(stUpdate.paramList[0].szUrl, pstNewRecording->id,DxRECLIST_URL);
	stUpdate.paramList[0].nNumOfTags	= 0;

	// check doNotDelete
	// -> do not check doNotDelete
	if (pstNewRecording->doNotDelete == OPL_TRUE)  //Merge r.12683
	{
		HxLOG_Print(" doNotDelete updated cur numOfTag (%d)!\n", stUpdate.paramList[0].nNumOfTags);

		utCurrTime = (UNIXTIME)HLIB_STD_GetSystemTime();
		stUpdate.paramList[0].recUpdateData.ulDoNotDeleteSetTime = utCurrTime;
		stUpdate.paramList[0].recUpdateData.bDoNotDelete = pstNewRecording->doNotDelete;
		HLIB_STD_StrUtf8NCpy(stUpdate.paramList[0].szTags[stUpdate.paramList[0].nNumOfTags],DxRECLIST_KEEP_TAG, DxRECLIST_MAX_UPDATE_TAG);
		stUpdate.paramList[0].nNumOfTags++;

		HxLOG_Print(" num of tags (%d) !\n" , stUpdate.paramList[0].nNumOfTags);
	}
	// check blocked
	if (orgRecording.isManualLocked != pstNewRecording->isManualLocked)
	{
		HxLOG_Print(" isManualLocked updated cur numOfTag (%d)!\n", stUpdate.paramList[0].nNumOfTags);
		stUpdate.paramList[0].recUpdateData.bUserLock = pstNewRecording->isManualLocked;	// TODO : use convertion function ?
		stUpdate.paramList[0].recUpdateData.bBlocked = pstNewRecording->isManualLocked;	// TODO : use convertion function ?
		HLIB_STD_StrUtf8NCpy(stUpdate.paramList[0].szTags[stUpdate.paramList[0].nNumOfTags],DxRECLIST_LOCK_TAG, DxRECLIST_MAX_UPDATE_TAG);
		stUpdate.paramList[0].nNumOfTags++;

		HxLOG_Print(" num of tags (%d) !\n" , stUpdate.paramList[0].nNumOfTags);
	}
	// TODO : check saveDays, saveEpisodes

	if (0 < stUpdate.paramList[0].nNumOfTags)
	{
		HxLOG_Print(" id (%s), numOfUpdate(%d)\n"
			, pstNewRecording->id, stUpdate.paramList[0].nNumOfTags);

		hErr = APK_META_RECORD_Update(&stUpdate, TRUE);
	}

	HLIB_STD_MemFree(stUpdate.paramList);

	return hErr;
}

static	int		opl_scheduler_Recording_Stop(OPL_Recording_t	*pstRecording)
{
	HERROR							hErr		= ERR_FAIL;
	APK_META_REC_STOP_REC_t	stStopRec	= {0};

	if (NULL == pstRecording)
	{
		return -1;
	}

	if (NULL == pstRecording->id)
	{
		return -1;
	}

	stStopRec.ulParamCount	= 1;
	stStopRec.paramList		= (APK_META_REC_PARAM_t*)HLIB_STD_MemCalloc(sizeof(APK_META_REC_PARAM_t) * stStopRec.ulParamCount);
	if (stStopRec.paramList)
	{
		DxRecListData_t *pstRecData = APK_META_RECORD_GetByUrl(pstRecording->id);
		if (pstRecData)
		{
			HxLOG_Print(" id (%s)\n", pstRecording->id);

			HLIB_STD_StrUtf8NCpy(stStopRec.paramList[0].szUrl, pstRecording->id, DxRECLIST_URL);
			hErr = APK_META_RECORD_StopRecording(&stStopRec, TRUE);
		}

		APK_META_RECORD_Release(pstRecData);

		HLIB_STD_MemFree(stStopRec.paramList);

		return hErr;
	}

	return ERR_FAIL;
}

static OPL_BOOL	opl_scheduler_CancelAllSpecificRservationType (OPL_HANDLE self, DxRsvType_e eRsvType)
{
	HUINT32				 ulCnt, ulNumSch = 0;

	(void)APK_META_SCHEDULE_CountScheduleByType(eRsvType, &ulNumSch);
	if (ulNumSch > 0)
	{
		DxSchedule_t		*pstSchArray = (DxSchedule_t *)HLIB_STD_MemCalloc(sizeof(DxSchedule_t) * ulNumSch);

		if (NULL != pstSchArray)
		{
			// Asynchronous �� Get->Remove �̷��� �ϸ� �� ���� �� �ִ�
			for (ulCnt = 0; ulCnt < ulNumSch; ulCnt++)
			{
				(void)APK_META_SCHEDULE_GetScheduleByType(eRsvType, ulCnt, &(pstSchArray[ulCnt]));
			}

			for (ulCnt = 0; ulCnt < ulNumSch; ulCnt++)
			{
				(void)APK_META_SCHEDULE_CancelSchedule(pstSchArray[ulCnt].ulSlot);
			}

			HLIB_STD_MemFree(pstSchArray);
		}

		return OPL_TRUE;
	}

	HxLOG_Debug(" There is no %d !\n", eRsvType);

	return OPL_TRUE;
}


static OPL_BOOL	opl_scheduler_ActivationTimeCancelAll (OPL_HANDLE self)
{
	return opl_scheduler_CancelAllSpecificRservationType(self, DxRSVTYPE_ACTIVATION_TIME);
}

static int opl_scheduler_RecordingComp(const void	*src, void *dst)
{
	OPL_Recording_t		*srcItem = (OPL_Recording_t *)src;
	OPL_ScheduledItem_t	*dstItem = (OPL_ScheduledItem_t*)dst;

	if (src == NULL || dst == NULL)
		return OPL_FALSE;

	if( srcItem->super.programmeID != NULL && dstItem->programmeID != NULL )
	{
		if (0 == HxSTD_StrCmp((HCHAR*)srcItem->super.programmeID, (HCHAR*)dstItem->programmeID))
			return OPL_TRUE;
	}
	else
	{
		if ( NULL != srcItem->super.name && NULL != dstItem->name &&
		        0 == HxSTD_StrCmp((HCHAR*)srcItem->super.name, (HCHAR*)dstItem->name))
			return OPL_TRUE;
	}

	return OPL_FALSE;
}

/* End : Recording List */
#define ____DEBUG_FUNC___________
#if defined(CONFIG_DEBUG)
STATIC void opl_scheduler_DebugPrintContext(void *pvArg)
{
	OPL_Scheduler_t *scheduler;
	HUINT32			i, ulNumOfSchedule;
	OPL_ScheduledItem_t	*pstOplSch;

	scheduler = OPL_Scheduler_GetInstance();
	if (scheduler == NULL)
	{
		return HxCMD_ERR;
	}

	if (scheduler->scheduledList != NULL)
	{
		HLIB_CMD_Printf("Print cached opl schedules \n");
		ulNumOfSchedule = HLIB_VECTOR_Length(scheduler->scheduledList);

		for (i=0;i<ulNumOfSchedule; i++)
		{
			pstOplSch = HLIB_VECTOR_ItemAt(scheduler->scheduledList, i);
			if (pstOplSch == NULL)
			{
				continue;
			}

			HLIB_CMD_Printf("uid(%d) type(%d) status(%d) name[%s] \n",pstOplSch->uid, pstOplSch->type,pstOplSch->status, pstOplSch->name);
		}
	}

}
#endif
#define	____PUBLIC_OPL_Scheduler___________________________________________________________
OPL_HANDLE	OPL_Scheduler_GetInstance (void)
{
	static OPL_Scheduler_t	s_scheduler;

	HxLOG_Trace();

	if (!s_scheduler.schedule)	//  if not initialized!!
	{
		s_scheduler.schedule      = APK_META_SCHEDULE_GetInstance();
		s_scheduler.scheduledList = HLIB_VECTOR_NewEx(NULL, 0
											, (void *(*)(void *))NULL
											, (void *(*)(void *))OPL_ScheduledItem_Delete
											, NULL
										);

#if defined (CONFIG_DEBUG)
	#define hCmdHandle		"OPL"
	HLIB_CMD_RegisterWord(hCmdHandle, opl_scheduler_DebugPrintContext,
						(HCHAR *)"print_sch",
						(HCHAR *)"print_sch",
						(HCHAR *)"print_sch");
#endif
	}
	return &s_scheduler;
}

void		OPL_Scheduler_AddEventListener (OPL_HANDLE self, OPL_Scheduler_EventListener_t listener)
{
	OPL_Scheduler_t *thiz = (OPL_Scheduler_t *)self;

	HxLOG_Trace();
	HxLOG_Assert(thiz);

	if (listener)
	{
		APK_META_SCHEDULE_RegisterNotifier(opl_scheduler_EventListener_v2, (void *)thiz);
	}
	thiz->listener = listener;
}

OPL_ScheduledItem_t *
		OPL_Scheduler_MakeSchedule (OPL_HANDLE self, OPL_ScheduledItem_t *item, OPL_ReservationTryType type)
{
	OPL_Scheduler_t		*thiz = (OPL_Scheduler_t *)self;

	HxLOG_Assert(thiz && item);

	return opl_scheduler_MakeSchedule(thiz, item, type);
}

OPL_ScheduledItem_t *
			OPL_Scheduler_Record (OPL_HANDLE self, OPL_ProgrammeHandle programme)
{
	OPL_Scheduler_t		*thiz = (OPL_Scheduler_t *)self;
	OPL_ScheduledItem_t	*item = NULL;
	int					 programmeIdType = 0; //OPL_ProgrammeIDType
	int					isScheduledAsSeries = 0;
	const char			*groupCrid = NULL;
	const char			*channelId = NULL;
	const char			*name = NULL;
	OPL_ReservationTryType	rsvTryType = 0;


	HxLOG_Trace();
	HxLOG_Assert(thiz && programme);

	programmeIdType = OPL_Programme_GetProgrammeIDType(programme);
	if (programmeIdType == eOPL_ProgrammeID_TVA_GROUP_CRID
		||programmeIdType == eOPL_ProgrammeID_TVA_CRID)
	{
		if (programmeIdType == eOPL_ProgrammeID_TVA_GROUP_CRID)
		{
			isScheduledAsSeries = 1;
		}
		else
		{
			isScheduledAsSeries = 0;
		}
		groupCrid = OPL_Programme_GetProgrammeID(programme);
		channelId = OPL_Programme_GetChannelID(programme);
		name = OPL_Programme_GetName(programme);

		if (groupCrid == NULL || channelId == NULL)
		{
			HxLOG_Error("invalid groupCrid(0x%x) channelId(0x%x) \n",groupCrid, channelId);
			return NULL;
		}

		item = OPL_ScheduledItem_NewWithCrid(eOPL_SCHEDULED_PRO_RECORD, isScheduledAsSeries, groupCrid,name, channelId, NULL);

		rsvTryType = eOPL_SCHEDULER_TRY_TYPE_CRID;
	}
	else
	{
		item = OPL_ScheduledItem_NewWithProgramme(eOPL_SCHEDULED_PRO_RECORD, programme, NULL);

		rsvTryType = eOPL_SCHEDULER_TRY_TYPE_NORMAL;
	}

	if (item == NULL)
	{
		HxLOG_Trace();
		return NULL;
	}

	return opl_scheduler_MakeSchedule(thiz, item, rsvTryType);
}


OPL_ScheduledItem_t *
			OPL_Scheduler_SeriesRecord (OPL_HANDLE self, OPL_ProgrammeHandle programme)
{
	OPL_Scheduler_t		*thiz = (OPL_Scheduler_t *)self;
	OPL_ScheduledItem_t	*item;

	HxLOG_Trace();
	HxLOG_Assert(thiz && programme);

	item = OPL_ScheduledItem_NewWithProgramme(eOPL_SCHEDULED_PRO_RECORD, programme, NULL);
	if (item == NULL)
	{
		HxLOG_Trace();
		return NULL;
	}
	item->isScheduledAsSeries = OPL_TRUE;
	return opl_scheduler_MakeSchedule(thiz, item, eOPL_SCHEDULER_TRY_TYPE_NORMAL);
}



OPL_ScheduledItem_t *
			OPL_Scheduler_RecordAt (
						  OPL_HANDLE self
						, unsigned long startTime
						, unsigned long duration
						, int repeatDays
						, const char *channelID
						, int volume
					)
{
	OPL_Scheduler_t		*thiz = (OPL_Scheduler_t *)self;
	OPL_ScheduledItem_t	*item;

	HxLOG_Trace();
	HxLOG_Assert(thiz && channelID);

	item = OPL_ScheduledItem_NewWithTime(eOPL_SCHEDULED_PRO_RECORD, startTime, duration, repeatDays, channelID, NULL,volume);
	if (item == NULL)
	{
		HxLOG_Trace();
		return NULL;
	}

	return opl_scheduler_MakeSchedule(thiz, item, eOPL_SCHEDULER_TRY_TYPE_NORMAL);
}


OPL_ScheduledItem_t *
			OPL_Scheduler_Watch (OPL_HANDLE self, OPL_ProgrammeHandle programme)
{
	OPL_Scheduler_t		*thiz = (OPL_Scheduler_t *)self;
	OPL_ScheduledItem_t	*item;

	HxLOG_Trace();
	HxLOG_Assert(thiz && programme);

	item = OPL_ScheduledItem_NewWithProgramme(eOPL_SCHEDULED_PRO_WATCH, programme, NULL);
	if (item == NULL)
	{
		HxLOG_Trace();
		return NULL;
	}

	return opl_scheduler_MakeSchedule(thiz, item, eOPL_SCHEDULER_TRY_TYPE_NORMAL);
}

OPL_ScheduledItem_t *
			OPL_Scheduler_WatchAt (
						  OPL_HANDLE self
						, unsigned long startTime
						, unsigned long duration
						, int repeatDays
						, const char *channelID
					)
{
	OPL_Scheduler_t		*thiz = (OPL_Scheduler_t *)self;
	OPL_ScheduledItem_t	*item;

	HxLOG_Trace();
	HxLOG_Assert(thiz && channelID);

	item = OPL_ScheduledItem_NewWithTime(eOPL_SCHEDULED_PRO_WATCH, startTime, duration, repeatDays, channelID, NULL,0);
	if (item == NULL)
	{
		HxLOG_Trace();
		return NULL;
	}

	return opl_scheduler_MakeSchedule(thiz, item, eOPL_SCHEDULER_TRY_TYPE_NORMAL);
}




OPL_BOOL	OPL_Scheduler_ActivationTimeSet (OPL_HANDLE self, unsigned long startTime, unsigned long duration, BPL_BOOL bEnable)
{
	HUINT32 			 ulSlot;
	DxSchedule_t		 stDxSched;
	HERROR				 hErr;
	OPL_BOOL		err;
	UNIXTIME		ulCurTime = HLIB_STD_GetSystemTime();
	HxDATETIME_t	stCurTime;

	err = opl_scheduler_ActivationTimeCancelAll(self);
	if(err == OPL_FALSE)
	{
		HxLOG_Error (" opl_scheduler_ActivationTimeCancelAll err:\n");
	}

	if(bEnable == TRUE)
	{
		HxSTD_MemSet(&stDxSched, 0, sizeof(DxSchedule_t));

		stDxSched.eRsvType = DxRSVTYPE_ACTIVATION_TIME;

		(void)HLIB_DATETIME_ConvertUnixTimeToDateTime(ulCurTime, &stCurTime);
		(void)HLIB_DATETIME_ConvertUnixTimeToDateTimeWithOutOffset((UNIXTIME)startTime, &(stDxSched.stStartTime));

		stDxSched.stStartTime.stDate.usYear = stCurTime.stDate.usYear;
		stDxSched.stStartTime.stDate.ucMonth = stCurTime.stDate.ucMonth;
		stDxSched.stStartTime.stDate.ucDay = stCurTime.stDate.ucDay;

		stDxSched.ulDuration = duration;

		stDxSched.eRepeat = DxRSVREPEAT_DAILY;

		hErr = APK_META_SCHEDULE_AddSchedule (&stDxSched, &ulSlot);


		if (ERR_OK != hErr)
		{
			HxLOG_Error (" APK_META_SCHEDULE_AddSchedule err:\n");
			err = OPL_FALSE;
		}
		else
		{
			err = OPL_TRUE;
		}
	}

	return err;

}


OPL_BOOL	OPL_Scheduler_Wakeup (OPL_HANDLE self, unsigned long time, int repeatDays, const char *ccid, int volume)
{
//	HUINT32				 ulCnt, ulNumSch = 0;
	HUINT32				 ulSlot;
	DxSchedule_t		 stDxSched;
	HERROR				 hErr;


	// Remove the wake up if it is left
	(void)OPL_Scheduler_CancelWakeup(self);

	HxSTD_MemSet(&stDxSched, 0, sizeof(DxSchedule_t));

	stDxSched.eRsvType = DxRSVTYPE_POWER_ON;
	(void)HLIB_DATETIME_ConvertUnixTimeToDateTime((UNIXTIME)time, &(stDxSched.stStartTime));
	stDxSched.ulSvcUid = (HUINT32)OPL_Channel_CCID2UniqueID(ccid);
	(void)opl_scheduler_CCID2TripleID(ccid, &(stDxSched.stTripleId));
	stDxSched.uExtInfo.stPwr.ucVolume = (HUINT8)volume;
	stDxSched.eRsvReady = DxRSVREADY_30_SEC;
	switch (repeatDays)
	{
	case eDxSCHED_DATE_ALL:
		stDxSched.eRepeat = DxRSVREPEAT_DAILY;
		break;

	case eDxSCHED_DATE_WEEKEND:
		stDxSched.eRepeat = DxRSVREPEAT_WEEKEND;
		break;

	case eDxSCHED_DATE_WEEKDAYS:
		stDxSched.eRepeat = DxRSVREPEAT_WEEKDAYS;
		break;

	case eDxSCHED_DATE_SUN:
	case eDxSCHED_DATE_MON:
	case eDxSCHED_DATE_TUE:
	case eDxSCHED_DATE_WED:
	case eDxSCHED_DATE_THU:
	case eDxSCHED_DATE_FRI:
	case eDxSCHED_DATE_SAT:
		stDxSched.eRepeat = DxRSVREPEAT_WEEKLY;
		break;

	case eDxSCHED_DATE_NONE:
	default:
		stDxSched.eRepeat = DxRSVREPEAT_ONCE;
		break;
	}

	hErr = APK_META_SCHEDULE_AddSchedule (&stDxSched, &ulSlot);
	if (ERR_OK != hErr)
	{
		HxLOG_Error (" APK_META_SCHEDULE_AddSchedule err:\n");
		return OPL_FALSE;
	}

	return OPL_TRUE;
}

OPL_BOOL	OPL_Scheduler_CancelWakeup (OPL_HANDLE self)
{
	return opl_scheduler_CancelAllSpecificRservationType(self, DxRSVTYPE_POWER_ON);
}

OPL_BOOL	OPL_Scheduler_GetWakeupParam (OPL_HANDLE self, unsigned long *time, int *repeatDays, const char **ccid, int *volume)
{
	HUINT32				 ulNumSch = 0;
	DxSchedule_t		 stDxSched;
	HERROR				 hErr;

	(void)APK_META_SCHEDULE_CountScheduleByType(DxRSVTYPE_POWER_ON, &ulNumSch);
	if (ulNumSch > 0)
	{
		hErr = APK_META_SCHEDULE_GetScheduleByType(DxRSVTYPE_POWER_ON, 0, &stDxSched);
		if (ERR_OK == hErr)
		{
			if (time)
			{
				(void)HLIB_DATETIME_ConvertDateTimeToUnixTime(&(stDxSched.stStartTime), (UNIXTIME  *)time);
			}

			if (repeatDays)
			{
				switch (stDxSched.eRepeat)
				{
				case DxRSVREPEAT_DAILY:
					*repeatDays = eDxSCHED_DATE_ALL;
					break;

				case DxRSVREPEAT_WEEKEND:
					*repeatDays = eDxSCHED_DATE_WEEKEND;
					break;

				case DxRSVREPEAT_WEEKDAYS:
					*repeatDays = eDxSCHED_DATE_WEEKDAYS;
					break;

				case DxRSVREPEAT_WEEKLY:
					*repeatDays = eDxSCHED_DATE_MON; // sama���� ������ �������� �ʾ� ������ ������ �������?������. Web���� �� ó�� �ϵ���...
					break;

				case DxRSVREPEAT_ONCE:
				default:
					*repeatDays = eDxSCHED_DATE_NONE;
					break;
				}
			}

			if (ccid)
			{
				OPL_Channel_t	channel;
				*ccid = NULL;

				channel = OPL_Channel_FindChannelByUID(NULL, (int)stDxSched.ulSvcUid);
				if (channel)
				{
					static char		 szTempBuf[128];

					*ccid = OPL_Channel_GetCCID(channel, szTempBuf, 128);
				}
			}

			if (volume)
			{
				*volume = (int)stDxSched.uExtInfo.stPwr.ucVolume;
			}
			HxLOG_Debug("[OPL_Scheduler_GetWakeupParam] *volume = %d\n", stDxSched.uExtInfo.stPwr.ucVolume);
		}

		return OPL_TRUE;
	}

	HxLOG_Debug(" There is no Wakeup!\n");
	return OPL_FALSE;
}

OPL_BOOL	OPL_Scheduler_TVTVEPG_Download (OPL_HANDLE self, unsigned long time, HBOOL isTvtvStandbyEnable)
{
//	HUINT32				 ulCnt, ulNumSch = 0;
	HUINT32				 ulSlot;
	DxSchedule_t		 stDxSched;
	HERROR				 hErr;

	// Remove the wake up if it is left
	(void)OPL_Scheduler_CancelTVTVEPG_Download(self);

	HxSTD_MemSet(&stDxSched, 0, sizeof(DxSchedule_t));

	stDxSched.eRsvType = DxRSVTYPE_TVTVEPG_TIMER;
	(void)HLIB_DATETIME_ConvertUnixTimeToDateTime((UNIXTIME)time, &(stDxSched.stStartTime));
	stDxSched.eRepeat = DxRSVREPEAT_DAILY;

	hErr = APK_META_SCHEDULE_AddSchedule (&stDxSched, &ulSlot);
	if (ERR_OK != hErr)
	{
		HxLOG_Error (" APK_META_SCHEDULE_AddSchedule err:\n");
		return OPL_FALSE;
	}
	return OPL_TRUE;
}

OPL_BOOL	OPL_Scheduler_SetTVTVEPGDownload_StandbyEnable (HBOOL isTvtvStandbyEnable)
{
	HERROR				 hErr;

	hErr = APK_SCENARIO_SetTVTVEPGDownload_StandbyEnable (isTvtvStandbyEnable);
	if (ERR_OK != hErr)
	{
		HxLOG_Error (" APK_SCENARIO_SetTVTVEPGDownload_StandbyEnable err:\n");
		return OPL_FALSE;
	}
	return OPL_TRUE;
}

OPL_BOOL	OPL_Scheduler_CancelTVTVEPG_Download (OPL_HANDLE self)
{
	return opl_scheduler_CancelAllSpecificRservationType(self, DxRSVTYPE_TVTVEPG_TIMER);
}

OPL_BOOL	OPL_Scheduler_GetTVTVEPG_DownloadParam (OPL_HANDLE self, unsigned long *time, HBOOL *isTvtvStandbyEnable)
{
	HUINT32				 ulNumSch = 0;
	DxSchedule_t		 stDxSched;
	HERROR				 hErr;
	HBOOL                isTvtvStandbyEnableTemp;

	(void)APK_META_SCHEDULE_CountScheduleByType(DxRSVTYPE_TVTVEPG_TIMER, &ulNumSch);

	hErr = APK_SCENARIO_GetTVTVEPGDownload_StandbyEnable(&isTvtvStandbyEnableTemp);

	if (ERR_OK != hErr)
	{
		if (isTvtvStandbyEnable)
		{
			*isTvtvStandbyEnable = isTvtvStandbyEnableTemp;
		}
	}

	if (ulNumSch > 0)
	{
		hErr = APK_META_SCHEDULE_GetScheduleByType(DxRSVTYPE_TVTVEPG_TIMER, 0, &stDxSched);
		if (ERR_OK == hErr)
		{
			if (time)
			{
				(void)HLIB_DATETIME_ConvertDateTimeToUnixTime(&(stDxSched.stStartTime), (UNIXTIME  *)time);
			}
		}
		return OPL_TRUE;
	}

	HxLOG_Debug(" There is no Wakeup!\n");
	return OPL_FALSE;
}

void		OPL_Scheduler_Remove (OPL_HANDLE self, OPL_ScheduledItem_t *item)
{
	OPL_Scheduler_t *thiz = (OPL_Scheduler_t *)self;

	HxLOG_Trace();
	HxLOG_Assert(thiz && item);
	HxLOG_Print(" OPL_Scheduler_Remove! uid = %d\n", item->uid);

	if( item->type == eOPL_SCHEDULED_PRO_DLNA ||  item->type == eOPL_SCHEDULED_PRO_DUBBING )
	{
		HINT32	slot;

		slot = UID2SLOT(item->uid & 0x0000ffff);

		APK_META_SCHEDULE_CancelSchedule(slot);
	}
	else
	{
		if (OPL_ScheduledItem_InstanceOf(item, OPL_Recording_t))
		{
			OPL_Recording_t	*recItem = (OPL_Recording_t *)item;

			if(recItem->showType == OPLRECORDING_SHOWTYPE_LIVE)
			{
				OPL_Scheduler_Stop(self, recItem);
			}

			// remove the item from recording list
			opl_scheduler_Recording_Remove(recItem);
		}
		else
		{
			HINT32	slot;

			slot = UID2SLOT(item->uid & 0x0000ffff);

			APK_META_SCHEDULE_CancelSchedule(slot);
		}
	}
}

void		OPL_Scheduler_RemoveWholeSeries (OPL_HANDLE self, OPL_ScheduledItem_t *item)
{
	OPL_Scheduler_t *thiz = (OPL_Scheduler_t *)self;

	HxLOG_Trace();
	HxLOG_Assert(thiz && item);
	HxLOG_Print(" OPL_Scheduler_Remove! uid = %d\n", item->uid);

	if (OPL_ScheduledItem_InstanceOf(item, OPL_Recording_t))
	{
		OPL_Recording_t	*recItem = (OPL_Recording_t *)item;

		OPL_Scheduler_Stop(self, recItem);

		// remove the item from recording list
		opl_scheduler_Recording_Remove(recItem);
	}
	else
	{
		HINT32	slot;

		slot = (item->uid & 0x0000ffff);

		APK_META_SCHEDULE_RemoveWholeSeries(thiz->schedule, slot);
	}
}

void		OPL_Scheduler_RemoveFirstItem (OPL_HANDLE self, OPL_ScheduledItem_t *item)
{
	OPL_Scheduler_t *thiz = (OPL_Scheduler_t *)self;

	HxLOG_Trace();
	HxLOG_Assert(thiz && item);
	HxLOG_Print(" OPL_Scheduler_Remove! uid = %d\n", item->uid);

	if (OPL_ScheduledItem_InstanceOf(item, OPL_Recording_t))
	{
		OPL_Recording_t	*recItem = (OPL_Recording_t *)item;

		OPL_Scheduler_Stop(self, recItem);

		// remove the item from recording list
		opl_scheduler_Recording_Remove(recItem);
	}
	else
	{
		HINT32	slot;

		slot = (item->uid & 0x0000ffff);
#if defined (CONFIG_OP_UK_DTT)
		// BASE: oipf remove(object) -> remove,
		// BASE: oipf remove(object, bool) -> removeFirstItem
		// UKDTT: oipf remove(object, bool) -> removeWholeSeries
		// TODO: add another oipf remove that can call removeWholeSeries, and delete this CONFIG
		APK_META_SCHEDULE_RemoveWholeSeries(thiz->schedule, slot);
#else
		APK_META_SCHEDULE_RemoveFirstItemOnSeries(thiz->schedule, slot);
#endif
	}
}

void		OPL_Scheduler_Confirm (OPL_HANDLE self, OPL_ScheduledItem_t *item, int type, int check)
{
	OPL_Scheduler_t *thiz = (OPL_Scheduler_t *)self;

	HxLOG_Trace();
	HxLOG_Assert(thiz && item);

	if (OPL_ScheduledItem_InstanceOf(item, OPL_Recording_t))
	{
		// recording
	}
	else
	{
		APK_META_SCHEDULE_Confirm(thiz->schedule, item->uid, type, check);
	}
}

void		OPL_Scheduler_Stop (OPL_HANDLE self, OPL_Recording_t *recording)
{
	OPL_Scheduler_t		*thiz = (OPL_Scheduler_t *)self;
	OPL_ScheduledItem_t	*item = (OPL_ScheduledItem_t *)recording;
	HINT32				slot;

	HxLOG_Trace();
	HxLOG_Assert(thiz && recording);

	if (item->type == eOPL_SCHEDULED_PRO_RECORD || item->type == eOPL_SCHEDULED_PRO_SAT_RECORD)
	{
		if (!OPL_ScheduledItem_InstanceOf(recording, OPL_Recording_t))
		{
			HxLOG_Error(" Only \"Recording\" can be stopped!!\n");
			return;
		}

		opl_scheduler_Recording_Stop(recording);
	}
	else
	{
		slot = UID2SLOT(item->uid & 0x0000ffff);

		APK_META_SCHEDULE_StopSchedule(slot);
	}
}


void		OPL_Scheduler_Refresh (OPL_HANDLE self)
{
	OPL_Scheduler_t	*thiz = (OPL_Scheduler_t *)self;

	HxLOG_Trace();
	HxLOG_Assert(thiz);

	if (APK_META_SCHEDULE_Refresh(thiz->schedule) == ERR_OK)
	{
		opl_scheduler_LoadScheduledList(thiz);
	}
}

OPL_BOOL	OPL_Scheduler_Update (OPL_HANDLE self, OPL_ScheduledItem_t *item)
{
	OPL_Scheduler_t *thiz = (OPL_Scheduler_t *)self;

	HxLOG_Trace();
	HxLOG_Assert(thiz && item);

	if (OPL_ScheduledItem_InstanceOf(item, OPL_Recording_t))
	{	// -> for the doNotDelete, saveDays, saveEpisodes ? (isManualLock : hms1000s)
		// TODO: update recording item!
		if (ERR_OK != opl_scheduler_Recording_Update((OPL_Recording_t *)item))
		{
			return OPL_FALSE;
		}
	}
	else	// scheduled programme
	{
		DxSchedule_t	*pstDxSched = NULL;
		HERROR			hErr;

		pstDxSched = (DxSchedule_t *)HLIB_STD_MemCalloc(sizeof(DxSchedule_t));
		if (pstDxSched == NULL)
		{
			HxLOG_Error("alloc error \n");
			return OPL_FALSE;
		}

		if (opl_scheduler_ToDlibSchedule(pstDxSched, item) == NULL)
		{
			HxLOG_Warning("fail to convert schedule. \n");
			HLIB_STD_MemFree(pstDxSched);
			return OPL_FALSE;
		}

		hErr = APK_META_SCHEDULE_UpdateSchedule(pstDxSched->ulSlot, pstDxSched);
		if (hErr != ERR_OK)
		{
			HxLOG_Warning("Fail to update schedule. \n");
			HLIB_STD_MemFree(pstDxSched);
			return OPL_FALSE;
		}

		HLIB_STD_MemFree(pstDxSched);
	}
	return OPL_TRUE;
}

OPL_BOOL	OPL_Scheduler_UpdateProperty(OPL_HANDLE self, OPL_ScheduledItem_t *item)
{
	OPL_BOOL				bError = OPL_TRUE;
	OPL_Scheduler_t			*thiz = (OPL_Scheduler_t *)self;

	HxLOG_Trace();
	HxLOG_Assert(thiz && item);

	if (OPL_ScheduledItem_InstanceOf(item, OPL_Recording_t))
	{
		bError = OPL_FALSE;
	}
	else	// scheduled programme
	{
		DxSchedule_t	*pstDxSched = NULL;
		HERROR			hErr;

		pstDxSched = (DxSchedule_t *)HLIB_STD_MemCalloc(sizeof(DxSchedule_t));
		if (pstDxSched == NULL)
		{
			HxLOG_Error("alloc error \n");
			return OPL_FALSE;
		}

		opl_scheduler_ToDlibSchedule(pstDxSched, item);

		hErr = APK_META_SCHEDULE_UpdateSchedule(pstDxSched->ulSlot, pstDxSched);
		if (hErr != ERR_OK)
		{
			HxLOG_Warning("Fail to update schedule. \n");
			HLIB_STD_MemFree(pstDxSched);
			return OPL_FALSE;
		}

		HLIB_STD_MemFree(pstDxSched);
	}
	return bError;
}

int			OPL_Scheduler_CountScheduledProgrammes (OPL_HANDLE self)
{
	OPL_Scheduler_t *thiz = (OPL_Scheduler_t *)self;

	HxLOG_Trace();
	HxLOG_Assert(thiz);

        if (thiz == NULL || thiz->scheduledList == NULL)
                return 0;

	return (int)HLIB_VECTOR_Length(thiz->scheduledList);
}

OPL_ScheduledItem_t *
			OPL_Scheduler_GetScheduledProgrammeAt (OPL_HANDLE self, int index)
{
	OPL_Scheduler_t *thiz = (OPL_Scheduler_t *)self;

	HxLOG_Trace();
	HxLOG_Assert(thiz);

        if (thiz == NULL || thiz->scheduledList == NULL)
                return NULL;

	return (OPL_ScheduledItem_t *)HLIB_VECTOR_ItemAt(thiz->scheduledList, index);
}

OPL_HANDLE		OPL_Scheduler_ScheduleClone (OPL_HANDLE self)
{
	if (self == NULL)
		return NULL;

		return OPL_ScheduledItem_Clone((OPL_ScheduledItem_t*)self);
}


void		OPL_Scheduler_ScheduleDelete (OPL_HANDLE self)
{
	if (self == NULL)
		return;

	OPL_ScheduledItem_Delete((OPL_ScheduledItem_t*)self);
}


OPL_ScheduledItem_t *
			OPL_Scheduler_FindScheduleExceptRecording(OPL_HANDLE self, const char *programmeID)
{
	OPL_Scheduler_t     *thiz = (OPL_Scheduler_t *)self;
	OPL_ScheduledItem_t *item;
	int	i, c;
	int	targetEventId, eventId;
	int	targetSvcUid, svcUid;
	int	targetFsatSvcUid, fsatSvcUid;

	HxLOG_Trace();
	HxLOG_Assert(thiz && programmeID);

	targetEventId = OPL_Programme_GetEventIDFromProgrammeID(programmeID);
	targetSvcUid = OPL_Programme_GetSvcUIDFromProgrammeID(programmeID);
	targetFsatSvcUid = OPL_Channel_GetFsatSvcIdByUID (NULL, targetSvcUid);


    c = OPL_Scheduler_CountScheduledProgrammes(self);
	for (i = 0 ; i < c ; i++)
	{
		item = OPL_Scheduler_GetScheduledProgrammeAt(self, i);
		if (item == NULL)
			continue;
		if (item->programmeID == NULL)
			continue;
		if (strcmp(item->programmeID, programmeID) == 0)
			return item;
		// fsat svc id ·Î ºñ±³ ÇÏ´Â ºÎºÐ ÇÊ¿ä.
		eventId = OPL_Programme_GetEventIDFromProgrammeID(item->programmeID);
		if(eventId == targetEventId)
		{
			svcUid = OPL_Programme_GetSvcUIDFromProgrammeID(item->programmeID);
			if(targetSvcUid && svcUid)
			{
				fsatSvcUid = OPL_Channel_GetFsatSvcIdByUID (NULL, svcUid);
				if(targetFsatSvcUid && fsatSvcUid && targetFsatSvcUid == fsatSvcUid)
				{
					return item;
				}

			}
		}
	}
	HxLOG_Debug("Cannot find schedule by programme(%s)\n", programmeID);
	return NULL;
}


// ���� ������ �������ε�, ���ڵ� ���� ���� �˻��Կ� ���?�̹� ��ȭ�� �Ϸ��?�������� �ִ� ���?EPG���� ���������� ǥ�� �Ҽ� ���?������ ���� �������� ������.
// function is unified because HMS have same problem
OPL_ScheduledItem_t *
			OPL_Scheduler_FindSchedule (OPL_HANDLE self, const char *programmeID)
{
	OPL_Scheduler_t     *thiz = (OPL_Scheduler_t *)self;
	OPL_ScheduledItem_t *item;
	int	i, c;
	int	targetEventId, eventId;
	int	targetSvcUid, svcUid;
	int	targetFsatSvcUid, fsatSvcUid;
#if 0
	DxRecListData_t  *pstRecData;
	HINT32			 RecLen;
	HxVector_t		 *vector;
#endif
	HxLOG_Trace();
	HxLOG_Assert(thiz && programmeID);

	targetEventId = OPL_Programme_GetEventIDFromProgrammeID(programmeID);
	targetSvcUid = OPL_Programme_GetSvcUIDFromProgrammeID(programmeID);
	targetFsatSvcUid = OPL_Channel_GetFsatSvcIdByUID (NULL, targetSvcUid);

	c = OPL_Scheduler_CountScheduledProgrammes(self);
	for (i = 0 ; i < c ; i++)
	{
		item = OPL_Scheduler_GetScheduledProgrammeAt(self, i);
		if (item == NULL)
			continue;
		if (item->programmeID == NULL)
			continue;
		if (strcmp(item->programmeID, programmeID) == 0)
		{
			return item;
		}
		// fsat svc id �� �� �ϴ� �κ� �ʿ�.
		eventId = OPL_Programme_GetEventIDFromProgrammeID(item->programmeID);
		if(eventId == targetEventId)
		{
			svcUid = OPL_Programme_GetSvcUIDFromProgrammeID(item->programmeID);
			if(targetSvcUid && svcUid)
			{
				fsatSvcUid = OPL_Channel_GetFsatSvcIdByUID (NULL, svcUid);
				if(targetFsatSvcUid && fsatSvcUid && targetFsatSvcUid == fsatSvcUid)
				{
					return item;
				}

			}
		}
	}

#if 0
		vector= APK_META_RECORD_GetItems();
		if(vector== NULL)
			return NULL;

		RecLen = HLIB_VECTOR_Length(vector);
		for(i=0; i< RecLen; i++)
		{
			char	szRecProgrammeID[128 + 1] = {0};
			pstRecData	 = (DxRecListData_t*)HLIB_VECTOR_ItemAt(vector, i);
			if (pstRecData == NULL || pstRecData->bEventBased == FALSE)
				continue;


	#ifndef CONFIG_FUNC_EPG_USE_SVCUID
				OPL_Channel_t channel = OPL_Channel_FindChannelByUID(NULL, pstRecData->svcUId);
				if(channel)
					OPL_Programme_MakeProgrammeID(szRecProgrammeID, 128 , OPL_Channel_GetOrgNetworkID(channel), OPL_Channel_GetTransponderID(channel), OPL_Channel_GetServiceID(channel), (int)pstRecData->nEventId);
				else
				{
					continue;
				}
	#else
				OPL_Programme_MakeProgrammeID(szRecProgrammeID, 128 , (int)pstRecData->svcUId, (int)pstRecData->nEventId);
	#endif

			if(HxSTD_StrEmpty(szRecProgrammeID) == TRUE)
				continue;

			//printf("[%s:%d][%s - %s]\n",__FUNCTION__,__LINE__,szRecProgrammeID, programmeID);
			if (HxSTD_StrCmp(szRecProgrammeID, programmeID) == 0)
			{
				item =	(OPL_ScheduledItem_t*)OPL_Recording_New("o", pstRecData);
				HLIB_VECTOR_Delete(vector);
				return	item;
			}
			// fsat svc id �� �� �ϴ� �κ� �ʿ�.
			eventId = OPL_Programme_GetEventIDFromProgrammeID(szRecProgrammeID);
			if(eventId == targetEventId)
			{
				svcUid = OPL_Programme_GetSvcUIDFromProgrammeID(szRecProgrammeID);
				if(targetSvcUid && svcUid)
				{
					fsatSvcUid = OPL_Channel_GetFsatSvcIdByUID (NULL, svcUid);
					if(targetFsatSvcUid && fsatSvcUid && targetFsatSvcUid == fsatSvcUid)
					{
						item =	(OPL_ScheduledItem_t*)OPL_Recording_New("o", pstRecData);
						HLIB_VECTOR_Delete(vector);
						return	item;
					}
				}
			}
		}
		HLIB_VECTOR_Delete(vector);
#endif

//	HxLOG_Debug("Cannot find schedule by programme(%s)\n", programmeID);
	return NULL;
}

OPL_Collection_t *
			OPL_Scheduler_FindSchedules (OPL_HANDLE self, const char *programmeID)
{
	OPL_Scheduler_t     *thiz = (OPL_Scheduler_t *)self;
	OPL_ScheduledItem_t *item;
	OPL_Collection_t	*scheduleColl;
	int	i, c = 0;
	int	targetEventId, eventId;
	int	targetSvcUid, svcUid;
	int	targetFsatSvcUid, fsatSvcUid;

	HxLOG_Trace();
	HxLOG_Assert(thiz && programmeID);

	targetEventId = OPL_Programme_GetEventIDFromProgrammeID(programmeID);
	targetSvcUid = OPL_Programme_GetSvcUIDFromProgrammeID(programmeID);
	targetFsatSvcUid = OPL_Channel_GetFsatSvcIdByUID (NULL, targetSvcUid);

	scheduleColl = OPL_Collection_NewEx(OPL_Scheduler_ScheduleDelete, (void *(*)(void *))OPL_Scheduler_ScheduleClone);
	return_if(scheduleColl == NULL, NULL);

	OPL_Collection_t *RecCollection = OPL_RecordItem_GetRecordings();
	if (RecCollection != NULL)
	{
		c = OPL_Collection_Length(RecCollection);
	}

	for (i = 0 ; i < c ; i++)
	{
		item = (OPL_ScheduledItem_t *)OPL_Collection_ItemAt(RecCollection,i);
		if (item == NULL || item->programmeID ==NULL)
			continue;

		if (strcmp(item->programmeID, programmeID) == 0)
		{
			(void) OPL_Collection_Add(scheduleColl, (void *)OPL_Recording_Clone(item));
			continue;
		}
		// fsat svc id ·Î ºñ±³ ÇÏ´Â ºÎºÐ ÇÊ¿ä.
		eventId = OPL_Programme_GetEventIDFromProgrammeID(item->programmeID);
		if(eventId == targetEventId)
		{
			svcUid = OPL_Programme_GetSvcUIDFromProgrammeID(item->programmeID);
			if(targetSvcUid && svcUid)
			{
				fsatSvcUid = OPL_Channel_GetFsatSvcIdByUID (NULL, svcUid);
				if(targetFsatSvcUid && fsatSvcUid && targetFsatSvcUid == fsatSvcUid)
				{
					(void) OPL_Collection_Add(scheduleColl, (void *)OPL_Recording_Clone(item));
				}
			}
		}
	}
	OPL_Collection_Delete(RecCollection);

    c = OPL_Scheduler_CountScheduledProgrammes(self);
	for (i = 0 ; i < c ; i++)
	{
		item = OPL_Scheduler_GetScheduledProgrammeAt(self, i);
		if (item == NULL)
			continue;
		if (item->programmeID == NULL)
			continue;
		if (strcmp(item->programmeID, programmeID) == 0)
		{
			(void) OPL_Collection_Add(scheduleColl, (void *)OPL_ScheduledItem_Clone(item));
			continue;
		}
		// fsat svc id ·Î ºñ±³ ÇÏ´Â ºÎºÐ ÇÊ¿ä.
		eventId = OPL_Programme_GetEventIDFromProgrammeID(item->programmeID);
		if(eventId == targetEventId)
		{
			svcUid = OPL_Programme_GetSvcUIDFromProgrammeID(item->programmeID);
			if(targetSvcUid && svcUid)
			{
				fsatSvcUid = OPL_Channel_GetFsatSvcIdByUID (NULL, svcUid);
				if(targetFsatSvcUid && fsatSvcUid && targetFsatSvcUid == fsatSvcUid)
				{
					(void) OPL_Collection_Add(scheduleColl, (void *)OPL_ScheduledItem_Clone(item));
				}
			}
		}
	}

	if (OPL_Collection_Length(scheduleColl) == 0)
	{
		HxLOG_Debug("Cannot find schedule by programme(%s)\n", programmeID);
		OPL_Collection_Delete(scheduleColl);
		return NULL;
	}

	HxLOG_Debug("finded schedule count (%d)\n", OPL_Collection_Length(scheduleColl));

	return scheduleColl;
}

// OPL_Scheduler_GetRunningRecordings -> APK ÅëÇØ¼­ OCTO·Î ºÎÅÍ ³ìÈ­Áß?Î Item °¡Á®¿È
// OPL_Scheduler_GetCurrentRecordings -> ?Ì¹Ì ?Ö´Â List¿¡¼­ ³ìÈ­Áß?Î ¾Æ?ÌÅÛ?» °¡Á®¿È
OPL_Collection_t *		OPL_Scheduler_GetCurrentRecordings(OPL_HANDLE self)
{
	OPL_Scheduler_t		*thiz = (OPL_Scheduler_t *)self;
	OPL_ScheduledItem_t	*schItem;
	OPL_Recording_t		*recItem;
	OPL_Collection_t		*scheduleColl,*RecColl;
	int	nIdx;
	int	i, c;
	HUINT32				ulCurrentTime;
	HUINT32				numRunRec = 0;

	HxLOG_Trace();
	HxLOG_Assert(thiz);

	ulCurrentTime = HLIB_STD_GetSystemTime();

	scheduleColl = OPL_Collection_NewEx(OPL_Scheduler_ScheduleDelete, (void *(*)(void *))OPL_Scheduler_ScheduleClone);
	return_if(scheduleColl == NULL, NULL);

	RecColl = OPL_RecordItem_GetRunningRecordings();
	if (RecColl != NULL)
	{
		numRunRec = OPL_Collection_Length(RecColl);
	}

	for(i=0; i < numRunRec;i++)
	{
		recItem = (OPL_Recording_t *)OPL_Collection_ItemAt(RecColl,i);
		if (recItem == NULL)
			continue;
		(void) OPL_Collection_Add(scheduleColl, (void *)recItem);
	}
	OPL_Collection_Delete(RecColl);

	c= OPL_Scheduler_CountScheduledProgrammes(self);
	for( i=0; i< c; i++ )
	{
		schItem = (OPL_ScheduledItem_t *)OPL_Scheduler_GetScheduledProgrammeAt(self, i);
		if (schItem == NULL)
			continue;

		if( (ulCurrentTime+EARLYSTARTTIME) < schItem->startTime)
			continue;

		switch(schItem->type)
		{
			case eOPL_SCHEDULED_PRO_RECORD:
			case eOPL_SCHEDULED_PRO_SAT_RECORD:
				nIdx = OPL_Collection_Find(scheduleColl, (void*)schItem, opl_scheduler_RecordingComp);
				if (nIdx < 0)
					break;

				recItem = (OPL_Recording_t *)OPL_Collection_ItemAt(scheduleColl, nIdx);

				recItem->super.uid			= schItem->uid;
				recItem->super.startTime	= schItem->startTime;
				recItem->super.duration	= schItem->duration;
				recItem->super.repeatDays	= schItem->repeatDays;
				recItem->super.recMode	= schItem->recMode;
				break;

			case eOPL_SCHEDULED_PRO_WATCH:
			case eOPL_SCHEDULED_PRO_DLNA:
			case eOPL_SCHEDULED_PRO_DUBBING:
				(void) OPL_Collection_Add(scheduleColl, (void *)OPL_Scheduler_ScheduleClone(schItem));
				break;
		}
	}

	if (OPL_Collection_Length(scheduleColl) == 0)
	{
		HxLOG_Debug("Cannot find running recording \n");
		OPL_Collection_Delete(scheduleColl);
		return NULL;
	}

	HxLOG_Debug("finded running recording count (%d)\n", OPL_Collection_Length(scheduleColl));

	return scheduleColl;
}

unsigned int	OPL_Scheduler_GetSystemTime()
{
	return HLIB_STD_GetSystemTime();
}


HBOOL	OPL_Scheduler_SetName (OPL_ScheduledItem_t *self, unsigned char *name)
{
	HBOOL	ret;
	APK_META_REC_RENAME_t		cmd;
	APK_META_REC_RENAME_PARAM_t	stList;

	if( self->type == eOPL_SCHEDULED_PRO_RECORD)
	{
		OPL_Recording_t	*item = (OPL_Recording_t*)self;

		if(item->id != NULL)
		{
			cmd.ulParamCount = 1;
			cmd.paramList = &stList;
			HxSTD_StrNCpy(stList.szUrl, item->id, DxRECLIST_URL);
			HxSTD_StrNCpy(stList.szRename, (HCHAR*)name, DxRECLIST_NAME_LEN);
			ret = APK_META_RECORD_Rename(&cmd, TRUE);
			if( ret == ERR_OK )
			{
				ret = TRUE;
			}
			else
			{
				ret = FALSE;
			}
		}
		else
		{
			ret = APK_META_SCHEDULE_SetName(item->super.uid , (HCHAR*)name );
		}
	}
	else
	{
		OPL_ScheduledItem_t	*item = (OPL_ScheduledItem_t*)self;
		ret = APK_META_SCHEDULE_SetName(item->uid , (HCHAR*)name );
	}

	return ret;
}

OPL_BOOL	OPL_Scheduler_SetSeriesManualLock (OPL_HANDLE self, const char *seriesID, OPL_BOOL bLock)
{
	OPL_Scheduler_t 					*thiz = (OPL_Scheduler_t *)self;
	HERROR								hErr = ERR_FAIL;

	HxLOG_Trace();
	HxLOG_Assert(thiz && seriesID);

	HxLOG_Debug(" seriesID %s bLock %d\n", seriesID, bLock);

	hErr = APK_META_RECORD_SetSeriesLock((HCHAR*)seriesID, bLock, TRUE);
	if(hErr != ERR_OK)
	{
		return OPL_TRUE;
	}
	return OPL_FALSE;
}


OPL_Collection_t	*OPL_Scheduler_GetGroupCRIDs(OPL_ScheduledItem_t *self)
{
	OPL_Collection_t		*coll = NULL;
	HINT32				i, n;
	OPL_ScheduledTvAnyTimetCRID_t			*crid;
	HxLOG_Trace();
	return_if(!self, NULL);
	return_if(!self->crid, NULL);
	coll = OPL_Collection_NewEx(HLIB_STD_MemFree_CB, (void *(*)(void *))HLIB_STD_StrDup_CB);
	return_if(coll == NULL, NULL);
	n = OPL_Collection_Length(self->crid);

	for(i=0; i<n; i++)
	{
		crid = (OPL_ScheduledTvAnyTimetCRID_t *)OPL_Collection_ItemAt(self->crid, i);
		if (crid == NULL || crid->ucCRIDType != DxCRID_TYPE_SERIES)
			continue;
		(void) OPL_Collection_Add(coll, (void *)HLIB_STD_StrDup(crid->szCRID));
	}
	if (OPL_Collection_Length(coll) == 0)
	{
		OPL_Collection_Delete(coll);
		return NULL;
	}

	return coll;
}


const char *	OPL_Scheduler_GetProgrammeCRID(OPL_ScheduledItem_t *self)
{
	HINT32						i, n;
	OPL_ScheduledTvAnyTimetCRID_t	*crid;

	HxLOG_Trace();
	return_if(!self, NULL);
	return_if(!self->crid, NULL);

	n = OPL_Collection_Length(self->crid);
	for(i=0; i<n; i++)
	{
		crid = (OPL_ScheduledTvAnyTimetCRID_t *)OPL_Collection_ItemAt(self->crid, i);
		if (crid != NULL && crid->ucCRIDType == DxCRID_TYPE_PROG )
		{
			return HLIB_STD_StrDup(crid->szCRID);
		}
	}
	return NULL;
}



void OPL_Scheduler_StartSatRecord(void)
{
	APK_EXE_StartSatRecord();
	return;
}


#ifdef CONFIG_OP_JAPAN
void		OPL_Scheduler_AddHNEventListener (OPL_HANDLE self, OPL_Scheduler_HNReservationFinishedEventListener_t hnlistener )
{
	OPL_Scheduler_t *thiz = (OPL_Scheduler_t *)self;

	HxLOG_Trace();
	HxLOG_Assert(thiz);

	thiz->hnlistener = hnlistener;
}

OPL_BOOL OPL_Scheduler_SetDefaultDMS(const char *device, const char *destinationId)
{
	HERROR		hErr = ERR_FAIL;

	hErr = APK_SQCSCENARIOJAPAN_SetDefaultDMS((HCHAR*)device, (HCHAR*)destinationId);
	if (hErr == ERR_OK)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

OPL_BOOL OPL_Scheduler_GetDefaultDMSDeviceHandle(unsigned char **dms)
{
	static char strTemp[257];
	HERROR		hErr;

	HxSTD_MemSet(strTemp, 0x0, sizeof(strTemp));
	hErr = APK_SQCSCENARIOJAPAN_GetDefaultDMSDeviceHandle(strTemp, 257);
	if( hErr == ERR_OK)
	{
		*dms = (unsigned char *)strTemp;
		return TRUE;
	}

	return FALSE;
}

OPL_BOOL OPL_Scheduler_GetDefaultDMSRecordDestinationId(unsigned char **dms)
{
	static char strTemp[257];
	HERROR		hErr;

	HxSTD_MemSet(strTemp, 0x0, sizeof(strTemp));
	hErr = APK_SQCSCENARIOJAPAN_GetDefaultDMSDestinationId(strTemp, 257);
	if( hErr == ERR_OK)
	{
		*dms = (unsigned char *)strTemp;
		return TRUE;
	}

	return FALSE;
}


#endif


