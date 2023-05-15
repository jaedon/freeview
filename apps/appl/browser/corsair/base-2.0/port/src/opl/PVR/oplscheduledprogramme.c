/**************************************************************************************/
/**
 * @file oplscheduledprogramme.c
 *
 * Defines the query structure.
 *
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#include "oplscheduledprogramme.h"
#include "oplcollection.h"
#include <hlib.h>
#include <apk.h>
#include <oplrecording.h>

#define UID2SLOT(uid)				((HUINT32)(((uid) == 0) ? 0xFFFF : (uid)-1))
#define SLOT2UID(slot)				((HINT32)(((slot) >= 0xFF00) ? 0 : (slot) + 1))

#define	____PROTECTED_OPL_ScheduledProgramme________________________________________________________

OPL_ScheduledItem_t *
	opl_scheduleditem_Init (int type, unsigned int sizeOf)
{
	OPL_ScheduledItem_t *self;

	self = (OPL_ScheduledItem_t *)HLIB_STD_MemCalloc(sizeOf);
	if (self)
	{
		self->type   = type;
		self->sizeOf = sizeOf;
	}
	return self;
}

void
	opl_scheduleditem_DeInit (OPL_ScheduledItem_t *self)
{
	if (self)
	{
		#define __SAFE_FREE(m)	do{if(m){HLIB_STD_MemFree(m);m=NULL;}}while(0)
		{
			__SAFE_FREE(self->name);
			__SAFE_FREE(self->longName);
			__SAFE_FREE(self->description);
			__SAFE_FREE(self->longDescription);
			__SAFE_FREE(self->channelID);
			__SAFE_FREE(self->programmeID);
			__SAFE_FREE(self->seriesID);
			__SAFE_FREE(self->storageID);
			__SAFE_FREE(self->userID);
			__SAFE_FREE(self->deviceHandle);
			__SAFE_FREE(self->destinationId);
			__SAFE_FREE(self->dubbingId);
		}
		#undef	__SAFE_FREE

		if(self->crid)
			OPL_Collection_Delete(self->crid);

		HLIB_STD_MemFree(self);
	}
}

#define	____PRIVATE_OPL_ScheduledProgramme__________________________________________________________


static OPL_Channel_t
				opl_scheduleditem_FindChannel (HINT32 svcuid)
{
	return OPL_Channel_FindChannelByUID(OPL_Channel_GetManager(), (int)svcuid);
}



static OPL_ScheduledItem_t *
	opl_scheduleditem_NewByDlib (const DxSchedule_t *schedule)
{
	OPL_Channel_t		channel;
	OPL_ScheduledItem_t *self = NULL;
//	HINT32				i;
	HCHAR				buf[256];

	HxLOG_Trace();
	HxLOG_Assert(schedule);

	if(schedule == NULL)
		return NULL;

	switch (schedule->eRsvType)
	{
	case DxRSVTYPE_WATCHING_SERIES_EBR:
	case DxRSVTYPE_WATCHING_EBR:
	case DxRSVTYPE_WATCHING_TBR:
		self = opl_scheduleditem_Init (eOPL_SCHEDULED_PRO_WATCH, sizeof(OPL_ScheduledItem_t));
		if (self == NULL)
			return NULL;

		self->eventbased = ((schedule->eRsvType == DxRSVTYPE_WATCHING_SERIES_EBR)||(schedule->eRsvType == DxRSVTYPE_WATCHING_SERIES_EBR))? TRUE:FALSE;
		self->isScheduledAsSeries = (schedule->eRsvType == DxRSVTYPE_WATCHING_SERIES_EBR)? TRUE:FALSE;

		if (!HxSTD_StrEmpty(schedule->uExtInfo.stWch.szEvtName))
		{
			self->name = HLIB_STD_StrDup(schedule->uExtInfo.stWch.szEvtName);
		}

		if (schedule->eScheduleSpec == eDxSchedule_Spec_TvAnyTime)
		{
			DxRSV_TvAnyTimeScheduledCRID stCrid;

			self->crid= OPL_Collection_NewEx(HLIB_STD_MemFree_CB, NULL);
			if (self->crid == NULL) break;

			if (schedule->uExtInfo.stWch.ex.tva.ucCRIDType == DxCRID_TYPE_SERIES)
			{
				HxSTD_snprintf(stCrid.szCRID,DxRSV_CRID_LEN + 1,"%s",schedule->uExtInfo.stWch.ex.tva.szCRID);
				stCrid.ucCRIDType = DxCRID_TYPE_SERIES;
				(void) OPL_Collection_Add(self->crid, (void *)HLIB_MEM_MemDup(&stCrid,sizeof(DxRSV_TvAnyTimeScheduledCRID)));
				if (!HxSTD_StrEmpty(schedule->uExtInfo.stWch.ex.tva.szProgCRID))
				{
					HxSTD_snprintf(stCrid.szCRID,DxRSV_CRID_LEN + 1,"%s",schedule->uExtInfo.stWch.ex.tva.szProgCRID);
					stCrid.ucCRIDType = DxCRID_TYPE_PROG;
					(void) OPL_Collection_Add(self->crid, (void *)HLIB_MEM_MemDup(&stCrid,sizeof(DxRSV_TvAnyTimeScheduledCRID)));
				}
			}
			else if (schedule->uExtInfo.stWch.ex.tva.ucCRIDType == DxCRID_TYPE_PROG)
			{
				HxSTD_snprintf(stCrid.szCRID,DxRSV_CRID_LEN + 1,"%s",schedule->uExtInfo.stWch.ex.tva.szCRID);
				stCrid.ucCRIDType = DxCRID_TYPE_PROG;
				(void) OPL_Collection_Add(self->crid, (void *)HLIB_MEM_MemDup(&stCrid,sizeof(DxRSV_TvAnyTimeScheduledCRID)));
			}
		}
		break;

	case DxRSVTYPE_NETSTREAMING_LIVE:
		self = opl_scheduleditem_Init (eOPL_SCHEDULED_PRO_NETSTREAMING_LIVE, sizeof(OPL_ScheduledItem_t));
		if (self == NULL)
			return NULL;
		if ( !HxSTD_StrEmpty(schedule->uExtInfo.stStrm.szAppDisplayString))
		{
			self->name		= HLIB_STD_StrDup (schedule->uExtInfo.stStrm.szAppDisplayString);
		}

		self->diseqcInput  = (int)schedule->uExtInfo.stStrm.stTuningParam.sat.antInfo.info.diseqc.diseqcInput;
		self->frequency = (int)schedule->uExtInfo.stStrm.stTuningParam.sat.tuningInfo.ulFrequency;
		self->symbolRate = (int)schedule->uExtInfo.stStrm.stTuningParam.sat.tuningInfo.ulSymbolRate;
		self->polarisation = (int)schedule->uExtInfo.stStrm.stTuningParam.sat.tuningInfo.ePolarization;
		self->satType =  HLIB_STD_StrDup(schedule->uExtInfo.stStrm.stTuningParam.sat.tuningInfo.satType);
		break;

	case DxRSVTYPE_NETSTREAMING_FILE:
		self = opl_scheduleditem_Init (eOPL_SCHEDULED_PRO_NETSTREAMING_FILE, sizeof(OPL_ScheduledItem_t));
		if (self == NULL)
			return NULL;

		if ( !HxSTD_StrEmpty(schedule->uExtInfo.stStrm.szAppDisplayString))
		{
			self->name		= HLIB_STD_StrDup (schedule->uExtInfo.stStrm.szAppDisplayString);
		}

		break;

	case DxRSVTYPE_RECORD_SERIES_EBR:
	case DxRSVTYPE_RECORD_EBR:
	case DxRSVTYPE_RECORD_TBR:
	case DxRSVTYPE_RECORD_INSTANT:
	case DxRSVTYPE_RECORD_DELAYED:
		self = opl_scheduleditem_Init (eOPL_SCHEDULED_PRO_RECORD, sizeof(OPL_ScheduledItem_t));
		if (self == NULL)
			return NULL;
		self->eventbased = ((schedule->eRsvType == DxRSVTYPE_RECORD_SERIES_EBR)||(schedule->eRsvType == DxRSVTYPE_RECORD_EBR))? TRUE:FALSE;
		self->isScheduledAsSeries = (schedule->eRsvType == DxRSVTYPE_RECORD_SERIES_EBR)? TRUE:FALSE;

		self->startPadding = (unsigned long)schedule->uExtInfo.stRec.ulPreOffsetTime;
		self->endPadding   = (unsigned long)schedule->uExtInfo.stRec.ulPostOffsetTime;
		if (!HxSTD_StrEmpty(schedule->uExtInfo.stRec.aucStorageUUID))
		{
			self->storageID = HLIB_STD_StrDup(schedule->uExtInfo.stRec.aucStorageUUID);
		}
		if (!HxSTD_StrEmpty(schedule->uExtInfo.stRec.szEventName))
		{
			self->name = HLIB_STD_StrDup(schedule->uExtInfo.stRec.szEventName);
		}

		if ( schedule->eScheduleSpec == eDxSchedule_Spec_TvAnyTime)
		{
			DxRSV_TvAnyTimeScheduledCRID stCrid;
			self->crid= OPL_Collection_NewEx(HLIB_STD_MemFree_CB, NULL);
			if (self->crid == NULL)
			{
				break;
			}

			if (schedule->uExtInfo.stRec.ex.tva.ucCRIDType == DxCRID_TYPE_SERIES)
			{
				HxSTD_snprintf(stCrid.szCRID,DxRSV_CRID_LEN + 1,"%s",schedule->uExtInfo.stRec.ex.tva.szCRID);
				stCrid.ucCRIDType = DxCRID_TYPE_SERIES;
				(void) OPL_Collection_Add(self->crid, (void *)HLIB_MEM_MemDup(&stCrid,sizeof(DxRSV_TvAnyTimeScheduledCRID)));
				if (!HxSTD_StrEmpty(schedule->uExtInfo.stRec.ex.tva.szProgCRID))
				{
					HxSTD_snprintf(stCrid.szCRID,DxRSV_CRID_LEN + 1,"%s",schedule->uExtInfo.stRec.ex.tva.szProgCRID);
					stCrid.ucCRIDType = DxCRID_TYPE_PROG;
					(void) OPL_Collection_Add(self->crid, (void *)HLIB_MEM_MemDup(&stCrid,sizeof(DxRSV_TvAnyTimeScheduledCRID)));
				}
			}
			else if (schedule->uExtInfo.stRec.ex.tva.ucCRIDType == DxCRID_TYPE_PROG)
			{
				HxSTD_snprintf(stCrid.szCRID,DxRSV_CRID_LEN + 1,"%s",schedule->uExtInfo.stRec.ex.tva.szCRID);
				stCrid.ucCRIDType = DxCRID_TYPE_PROG;
				(void) OPL_Collection_Add(self->crid, (void *)HLIB_MEM_MemDup(&stCrid,sizeof(DxRSV_TvAnyTimeScheduledCRID)));
			}
		}
		else
		{
			if (schedule->uExtInfo.stRec.ulSeriesId != 0)
			{
				HCHAR szSeriesId[DxPVR_TVANYTIME_CRID_TOTAL_LEN];

				HxSTD_snprintf(szSeriesId,DxPVR_TVANYTIME_CRID_TOTAL_LEN,"%d",schedule->uExtInfo.stRec.ulSeriesId);
				self->seriesID = HLIB_STD_StrDup(szSeriesId);
			}
		}
		break;

	case DxRSVTYPE_RECORD_SATRECORD_TBR:
		self = opl_scheduleditem_Init (eOPL_SCHEDULED_PRO_SAT_RECORD, sizeof(OPL_ScheduledItem_t));
		if (self == NULL)
			return NULL;

		self->startPadding = (unsigned long)schedule->uExtInfo.stRec.ulPreOffsetTime;
		self->endPadding   = (unsigned long)schedule->uExtInfo.stRec.ulPostOffsetTime;
		if (!HxSTD_StrEmpty(schedule->uExtInfo.stRec.aucStorageUUID))
		{
			self->storageID = HLIB_STD_StrDup(schedule->uExtInfo.stRec.aucStorageUUID);
		}

		break;
	case DxRSVTYPE_DLNA_EBR:
	case DxRSVTYPE_DLNA_TBR:
		self = opl_scheduleditem_Init (eOPL_SCHEDULED_PRO_DLNA, sizeof(OPL_ScheduledItem_t));
		if (self == NULL)
			return NULL;

		self->eventbased = (schedule->eRsvType == DxRSVTYPE_DLNA_EBR)? TRUE:FALSE;
		/*
		if (schedule->deviceHandle)
			self->deviceHandle	= HLIB_STD_StrDup(schedule->deviceHandle);
		if (schedule->destinationId)
			self->destinationId	= HLIB_STD_StrDup(schedule->destinationId);
		*/
		break;

	case DxRSVTYPE_DLNA_DUBBING:
		self = opl_scheduleditem_Init (eOPL_SCHEDULED_PRO_DUBBING, sizeof(OPL_ScheduledItem_t));
		if (self == NULL)
			return NULL;

		/*
		if (schedule->deviceHandle)
			self->deviceHandle	= HLIB_STD_StrDup(schedule->deviceHandle);
		if (schedule->destinationId)
			self->destinationId	= HLIB_STD_StrDup(schedule->destinationId);
		if (schedule->dubbingId)
			self->dubbingId	= HLIB_STD_StrDup(schedule->dubbingId);
		*/
		break;

	case DxRSVTYPE_POWER_ON:
	case DxRSVTYPE_POWER_OFF:
		self = opl_scheduleditem_Init (eOPL_SCHEDULED_PRO_POWER_ON_TIMER, sizeof(OPL_ScheduledItem_t));
		if (self == NULL)
			return NULL;

		self->lcn	 = schedule->uExtInfo.stPwr.usChNum;
		self->volume = schedule->uExtInfo.stPwr.ucVolume;
		break;
	case DxRSVTYPE_REGULAR_OTA:
		self = opl_scheduleditem_Init (eOPL_SCHEDULED_PRO_REGULAR_OTA, sizeof(OPL_ScheduledItem_t));
		if (self == NULL)
			return NULL;
		break;
	case DxRSVTYPE_ASO_NOTIFY:
		self = opl_scheduleditem_Init (eOPL_SCHEDULED_PRO_ASO_NOTIFY, sizeof(OPL_ScheduledItem_t));
		if (self == NULL)
			return NULL;
		break;
	default:
		return NULL;
	}

	self->uid		= SLOT2UID(schedule->ulSlot);
	(void)HLIB_DATETIME_ConvertDateTimeToUnixTime (&(schedule->stStartTime), &self->startTime);
	self->duration	=  schedule->ulDuration;
	self->status	= (int)schedule->eStatus;

	if (schedule->eRepeat != DxRSVREPEAT_ONCE)
	{
		self->repeatDays = (int)schedule->ulRepeatDays;
	}

	// 더빙을 제외한 공통작업 수행.
	switch(schedule->eRsvType)
	{
	case DxRSVTYPE_WATCHING_SERIES_EBR:
	case DxRSVTYPE_WATCHING_EBR:
	case DxRSVTYPE_RECORD_SERIES_EBR:
	case DxRSVTYPE_RECORD_EBR:
		{
			// EVENT based
#ifndef CONFIG_FUNC_EPG_USE_SVCUID
			OPL_Channel_t channel = OPL_Channel_FindChannelByUID(NULL, schedule->ulSvcUid);
			OPL_Programme_MakeProgrammeID(buf, 256 , OPL_Channel_GetOrgNetworkID(channel), OPL_Channel_GetTransponderID(channel), OPL_Channel_GetServiceID(channel), (int)schedule->uExtInfo.stRec.nEvtId);
#else
			OPL_Programme_MakeProgrammeID(buf, 256, (int)schedule->ulSvcUid, (int)schedule->uExtInfo.stRec.nEvtId);
#endif
			self->programmeID = HLIB_STD_StrDup(buf);
		}
	case DxRSVTYPE_WATCHING_TBR:
	case DxRSVTYPE_NETSTREAMING_LIVE:
	case DxRSVTYPE_RECORD_TBR:
	case DxRSVTYPE_RECORD_INSTANT:
	case DxRSVTYPE_RECORD_DELAYED:
	case DxRSVTYPE_RECORD_SATRECORD_TBR:
	case DxRSVTYPE_DLNA_EBR:
	case DxRSVTYPE_DLNA_TBR:
	case DxRSVTYPE_POWER_ON:
	case DxRSVTYPE_POWER_OFF:
	case DxRSVTYPE_REGULAR_OTA:
	case DxRSVTYPE_ASO_NOTIFY:
		/*
		 * SAT>IP 에서는 유효한 svcUID를 알 수 없음.
		 * 아래 #IF문은 crash 를 막기 위해서 workaround 코드
		 */
		channel = opl_scheduleditem_FindChannel(schedule->ulSvcUid);
		if (NULL != channel)
		{
			self->channelID = HLIB_STD_StrDup(OPL_Channel_GetCCID(channel, buf, 256));
			if (self->channelID == NULL)
			{
				HxLOG_Error("Out of memory!\n");
				OPL_ScheduledItem_Delete(self);
				return NULL;
			}
		}
		else
		{
				self->channelID  = HLIB_STD_MemAlloc (sizeof(OPL_Channel_t)); // 뭔가 Crash 방지를 위해 넣은 듯?
		}
		break;

	case DxRSVTYPE_NETSTREAMING_FILE:
		break;
	case DxRSVTYPE_DLNA_DUBBING:
		channel = opl_scheduleditem_FindChannel(schedule->ulSvcUid);
		if( channel != NULL )
			self->channelID = HLIB_STD_StrDup(OPL_Channel_GetCCID(channel, buf, 256));
		break;

	default:
		break;
	}

	return self;
}

#define	____PUBLIC_OPL_ScheduledItem___________________________________________________________

OPL_ScheduledItem_t *
	OPL_ScheduledItem_New (const char *signature, ...)
{
	OPL_ScheduledItem_t *self;
	va_list	ap;

	HxLOG_Trace();
	HxLOG_Assert(signature);

	va_start(ap, signature);
	if (strcmp(signature, "o2") == 0)
	{
		void *obj = va_arg(ap, void *);	// for information hiding...

		self = opl_scheduleditem_NewByDlib((const DxSchedule_t *)obj);
	}
	else if (strcmp(signature, "io") == 0)
	{
		int	 type = va_arg(ap, int);
		void *obj = va_arg(ap, void *);

		self = OPL_ScheduledItem_NewWithProgramme(type, (OPL_ProgrammeHandle)obj, NULL);
	}
	else if (strcmp(signature, "iiiis") == 0)
	{
		int           type       = va_arg(ap, int);
		unsigned long startTime  = va_arg(ap, unsigned long);
		unsigned long duration   = va_arg(ap, unsigned long);
		int           repeatDays = va_arg(ap, int);
		const char *  channelID  = va_arg(ap, const char *);

		self = OPL_ScheduledItem_NewWithTime(type, startTime, duration, repeatDays, channelID, NULL,0);
	}
	else
	{
		HxLOG_Error("%s(%s) Invalid signature!!!\n", __FUNCTION__, signature);
		self = NULL;
	}
	va_end(ap);

	return self;
}

OPL_ScheduledItem_t *
	OPL_ScheduledItem_NewWithDummy (void)
{
	OPL_ScheduledItem_t *self;

	HxLOG_Trace();

	self = opl_scheduleditem_Init (eOPL_SCHEDULED_PRO_UNKNOWN, sizeof(OPL_ScheduledItem_t));
	if (self == NULL)
		return NULL;
	return self;
}

OPL_ScheduledItem_t *
	OPL_ScheduledItem_NewWithProgramme (int type, OPL_ProgrammeHandle programme, const char *storageID)
{
	OPL_ScheduledItem_t *self;

	HxLOG_Trace();
	HxLOG_Assert(programme);

	self = opl_scheduleditem_Init (type, sizeof(OPL_ScheduledItem_t));
	if (self == NULL)
		return NULL;

	self->startTime = OPL_Programme_GetStartTime(programme);
	self->duration  = OPL_Programme_GetDuration(programme);
	self->name      = HLIB_STD_StrDup(OPL_Programme_GetName(programme));
	self->longName  = NULL;
	self->description     = HLIB_STD_StrDup(OPL_Programme_GetDescription(programme));
	self->longDescription = HLIB_STD_StrDup(OPL_Programme_GetLongDescription(programme));

	self->channelID	  = HLIB_STD_StrDup(OPL_Programme_GetChannelID(programme));
	self->programmeID = HLIB_STD_StrDup(OPL_Programme_GetProgrammeID(programme));
	self->epgType	= (int)OPL_Programme_GetEpgType(programme);
	self->seriesID = HLIB_STD_StrDup(OPL_Programme_GetField(programme, "seriesID"));
	self->freeCaMode = OPL_Programme_GetIsCharged(programme);
	if( storageID != NULL )
		self->storageID = HLIB_STD_StrDup(storageID);

	if(eOPL_SCHEDULED_PRO_RECORD == type)
	{
		APK_SCENARIO_GetStartPadding((HINT32 *)&self->startPadding);
		APK_SCENARIO_GetEndPadding((HINT32 *)&self->endPadding);
		self->crid = OPL_Programme_GetGroupCRIDsAndType(programme);
	}
	return self;
}

OPL_ScheduledItem_t *
	OPL_ScheduledItem_NewWithTime (int type, unsigned long startTime, unsigned long duration, int repeatDays, const char *channelID, const char *storageID, int volume)
{
	OPL_ScheduledItem_t 	*self;
	OPL_Channel_t			channel = NULL;
	HERROR					hErr = ERR_FAIL;
	HCHAR					szName[256];

	HxLOG_Trace();
	HxLOG_Assert(channelID);
	unsigned long endTime;

	if (type != eOPL_SCHEDULED_PRO_ASO_NOTIFY)
	{
		channel = opl_scheduleditem_FindChannel((HINT32)OPL_Channel_CCID2UniqueID(channelID));
		if (!channel)
			return NULL;
	}

	self = opl_scheduleditem_Init (type, sizeof(OPL_ScheduledItem_t));
	if (self == NULL)
		return NULL;

	self->startTime  = startTime;
	self->duration   = duration;
	self->repeatDays = repeatDays;
	self->channelID  = HLIB_STD_StrDup(channelID);
	self->freeCaMode = FALSE;
	self->volume	 = volume;
	endTime = startTime + duration;

	HxSTD_MemSet(szName, 0, 128);
	hErr = APK_META_QUERY_FindEventName(OPL_Channel_CCID2UniqueID(channelID), self->startTime, endTime, &szName, 0);
	if(hErr == ERR_OK)
	{
		self->name = HLIB_STD_StrDup(&szName);
	}

	if ((self->name == NULL)&&(channel != NULL))
	{
		self->name 		= HLIB_STD_StrDup(OPL_Channel_GetName(channel));
		self->lcn		= OPL_Channel_GetMajorChannel(channel);
	}

	if( storageID != NULL )
		self->storageID = HLIB_STD_StrDup(storageID);

	if(eOPL_SCHEDULED_PRO_RECORD == type)
	{
		APK_SCENARIO_GetStartPadding((HINT32 *)&self->startPadding);
		APK_SCENARIO_GetEndPadding((HINT32 *)&self->endPadding);
	}

	return self;
}


OPL_ScheduledItem_t *
	OPL_ScheduledItem_NewWithCrid (int recType, int isScheduledAsSeries, const char *Crid, const char *title, const char *channelID, const char *storageID)
{
	OPL_ScheduledItem_t *self;
	OPL_Collection_t	*coll;
	OPL_ScheduledTvAnyTimetCRID_t	stCrid;
	OPL_Channel_t			channel;

	HxLOG_Trace();
	HxLOG_Assert(Crid);

	channel = opl_scheduleditem_FindChannel((HINT32)OPL_Channel_CCID2UniqueID(channelID));
	if (!channel)
		return NULL;

	coll = OPL_Collection_NewEx(OPL_ScheduledItem_FreeCrid, OPL_ScheduledItem_CloneCrid);
	if (coll == NULL)
		return NULL;

	self = opl_scheduleditem_Init (recType, sizeof(OPL_ScheduledItem_t));
	if (self == NULL)
	{
		OPL_Collection_Delete(coll);
		return NULL;
	}

	self->startTime  = 0;
	self->duration   = 0;
	self->repeatDays = 0;
	self->channelID  = HLIB_STD_StrDup(channelID);
	self->name = HLIB_STD_StrDup(title);
	if( storageID != NULL )
		self->storageID = HLIB_STD_StrDup(storageID);
	else
		self->storageID = NULL;

	self->programmeID = NULL;
	self->isScheduledAsSeries = isScheduledAsSeries;

	if (isScheduledAsSeries == TRUE)
	{
		stCrid.ucCRIDType = DxCRID_TYPE_SERIES;
		HxSTD_snprintf(stCrid.szCRID,OPL_SCHEDULED_CRID_LEN,"%s",Crid);
	}
	else
	{
		stCrid.ucCRIDType = DxCRID_TYPE_PROG;
		HxSTD_snprintf(stCrid.szCRID,OPL_SCHEDULED_CRID_LEN,"%s",Crid);
	}
	(void) OPL_Collection_Add(coll, OPL_ScheduledItem_CloneCrid(&stCrid));
	self->crid = coll;

	return self;
}

OPL_ScheduledItem_t *
	OPL_ScheduledItem_Clone (const OPL_ScheduledItem_t *self)
{
	OPL_ScheduledItem_t	*clone;

	//HxLOG_Trace();

	clone = opl_scheduleditem_Init (self->type, self->sizeOf);
	if (clone == NULL)
	{
		HxLOG_Error("Out of memory!\n");
		return NULL;
	}

	clone->uid           = self->uid;
	clone->startTime     = self->startTime;
	clone->duration      = self->duration;
	clone->startPadding  = self->startPadding;
	clone->endPadding    = self->endPadding;
	clone->repeatDays    = self->repeatDays;
	clone->targetStorage = self->targetStorage;
	clone->isScheduledAsSeries = self->isScheduledAsSeries;
	clone->episode       = self->episode;
	clone->season       = self->season;
	clone->totalEpisodes = self->totalEpisodes;
	clone->recMode		= self->recMode;
	clone->fromDap		= self->fromDap;
	clone->Rating		= self->Rating;
	clone->lcn			= self->lcn;
	clone->volume		= self->volume;
	clone->status		= self->status;
	clone->eventbased	= self->eventbased;

	if (self->name)
		clone->name = HLIB_STD_StrDup(self->name);
	if (self->longName)
		clone->longName = HLIB_STD_StrDup(self->longName);
	if (self->description)
		clone->description = HLIB_STD_StrDup(self->description);
	if (self->longDescription)
		clone->longDescription = HLIB_STD_StrDup(self->longDescription);
	if (self->channelID)
		clone->channelID = HLIB_STD_StrDup(self->channelID);
	if (self->programmeID)
		clone->programmeID = HLIB_STD_StrDup(self->programmeID);
	if (self->seriesID)
		clone->seriesID = HLIB_STD_StrDup(self->seriesID);
	if (self->storageID)
		clone->storageID = HLIB_STD_StrDup(self->storageID);
	if (self->userID)
		clone->userID = HLIB_STD_StrDup(self->userID);
	if (self->deviceHandle)
		clone->deviceHandle = HLIB_STD_StrDup(self->deviceHandle);
	if (self->destinationId)
		clone->destinationId = HLIB_STD_StrDup(self->destinationId);
	if (self->dubbingId)
		clone->dubbingId = HLIB_STD_StrDup(self->dubbingId);

	if (self->crid)
		clone->crid	= OPL_Collection_Clone(self->crid,(void * (*)(void*))HLIB_STD_StrDup_CB);

	clone->serviceId = self->serviceId;
	clone->transportStreamId = self->transportStreamId;
	clone->originalNetworkId = self->originalNetworkId;

	clone->diseqcInput = self->diseqcInput;
	clone->frequency = self->frequency;
	clone->symbolRate = self->symbolRate;
	clone->polarisation = self->polarisation;
	if (self->satType)
		clone->satType =  HLIB_STD_StrDup(self->satType);

	return clone;
}


void
	OPL_ScheduledItem_Delete (OPL_ScheduledItem_t *self)
{
	HxLOG_Trace();
	HxLOG_Assert(OPL_ScheduledItem_InstanceOf(self, OPL_ScheduledItem_t));

	opl_scheduleditem_DeInit(self);
}

//OPL_Collection_t *
//	OPL_ScheduledItem_GetParentalRating (OPL_ScheduledItem_t *self)
int
	OPL_ScheduledItem_GetParentalRating (OPL_ScheduledItem_t *self)
{
	int				parentRating = 0;
	int				i, eventId, svcUid;
	DxEvent_t			*pEvtItem = NULL;
	DxRecListData_t	*pRecItem = NULL;


	HxLOG_Trace();


	if (OPL_ScheduledItem_InstanceOf(self, OPL_ScheduledItem_t))
	{
		if(self->programmeID == NULL)
			return 0;

		eventId = OPL_Programme_GetEventIDFromProgrammeID(self->programmeID);
		svcUid = OPL_Programme_GetSvcUIDFromProgrammeID(self->programmeID);
#ifdef CONFIG_FUNC_EPG_USE_SVCUID
		pEvtItem = APK_META_QUERY_GetEventWithIds(svcUid, eventId);
#else
		pEvtItem = APK_META_QUERY_GetEventWithIds(self->originalNetworkId, self->transportStreamId, self->serviceId, eventId);
#endif
		if( pEvtItem != NULL)
		{
			parentRating = pEvtItem->parentalRating;
			APK_EVENT_Delete(pEvtItem);
		}
	}
	else
	{
		OPL_Recording_t *item = (OPL_Recording_t*)self;
		pRecItem	= APK_META_RECORD_GetByUrl(item->id);
		if( pRecItem != NULL )
		{
			for(i = 0; i < DxPVR_PARENTAL_RATING_MAX; i++)
			{
				if( parentRating < pRecItem->stEvt.astParentalRating[i].ucParentalRating )
					parentRating = pRecItem->stEvt.astParentalRating[i].ucParentalRating;
			}
			APK_META_RECORD_Release(pRecItem);
		}
	}

	return parentRating;
}

void *
	OPL_ScheduledItem_CloneCrid (void * data)
{
#if 0
	DxEventCRID_t *pCrid = (DxEventCRID_t *)data;
	DxEventCRID_t *clone = NULL;

	if (pCrid == NULL)
		return NULL;

	clone = HLIB_STD_MemDup(pCrid,sizeof(DxEventCRID_t));
	if (clone)
	{
		if (pCrid->crid)
			clone->crid = HLIB_STD_StrDup(pCrid->crid);
	}

	return clone;
#else
	OPL_ScheduledTvAnyTimetCRID_t *crid = (OPL_ScheduledTvAnyTimetCRID_t *)data;

	// crid type이 앞에 있는 것에 의존해서 strdup .. 추후 수정
	return HLIB_STD_StrDup((const HCHAR *)crid);
#endif
}

void
	OPL_ScheduledItem_FreeCrid (void *data)
{
#if 0
	DxEventCRID_t *pCrid = (DxEventCRID_t *)data;

	if (pCrid->crid)
		HLIB_MEM_Free(pCrid->crid);

	HLIB_MEM_Free(pCrid);
#else
	HLIB_STD_MemFree(data);
#endif
}

#ifdef CONFIG_OP_JAPAN
OPL_ScheduledItem_t *
	OPL_ScheduledItem_jlabsNewWithProgramme (int type, OPL_ProgrammeHandle programme, int recmode, const char *storageID, const char *devicehandle, const char *destinationId)
{
	OPL_ScheduledItem_t *self;

	HxLOG_Trace();
	HxLOG_Assert(programme);

	self = opl_scheduleditem_Init (type, sizeof(OPL_ScheduledItem_t));
	if (self == NULL)
		return NULL;

	self->startTime = OPL_Programme_GetStartTime(programme);
	self->duration  = OPL_Programme_GetDuration(programme);
	self->name      = HLIB_STD_StrDup(OPL_Programme_GetName(programme));
	self->longName  = NULL;
	self->description     = HLIB_STD_StrDup(OPL_Programme_GetDescription(programme));
	self->longDescription = HLIB_STD_StrDup(OPL_Programme_GetLongDescription(programme));

	self->channelID	  = HLIB_STD_StrDup(OPL_Programme_GetChannelID(programme));
	self->programmeID = HLIB_STD_StrDup(OPL_Programme_GetProgrammeID(programme));
	self->seriesID = NULL;
	self->recMode = recmode;
	self->freeCaMode = OPL_Programme_GetIsCharged(programme);
	if( storageID != NULL )
		self->storageID = HLIB_STD_StrDup(storageID);
	if( devicehandle != NULL )
		self->deviceHandle = HLIB_STD_StrDup(devicehandle);
	if( destinationId != NULL )
		self->destinationId = HLIB_STD_StrDup(destinationId);

	if(eOPL_SCHEDULED_PRO_RECORD == type)
	{
		APK_SCENARIO_GetStartPadding((HUINT32 *)&self->startPadding);
		APK_SCENARIO_GetEndPadding((HUINT32 *)&self->endPadding);
	}

	return self;
}

OPL_ScheduledItem_t *
	OPL_ScheduledItem_jlabsNewWithTime (int type, unsigned long startTime, unsigned long duration, int repeatDays, const char *channelID, int recmode, const char *storageID, const char *devicehandle, const char *destinationId)
{
	OPL_ScheduledItem_t *self;
	OPL_Channel_t		channel;

	HxLOG_Trace();
	HxLOG_Assert(channelID);

	channel = opl_scheduleditem_FindChannel((HINT32)OPL_Channel_CCID2UniqueID(channelID));
	if (!channel)
		return NULL;

	self = opl_scheduleditem_Init (type, sizeof(OPL_ScheduledItem_t));
	if (self == NULL)
		return NULL;

	self->startTime  = startTime;
	self->duration   = duration;
	self->repeatDays = repeatDays;
	self->channelID  = HLIB_STD_StrDup(channelID);

	self->name = HLIB_STD_StrDup(OPL_Channel_GetName(channel));
	self->recMode = recmode;
	self->freeCaMode = FALSE;
	if( storageID != NULL )
		self->storageID = HLIB_STD_StrDup(storageID);
	if( devicehandle != NULL )
		self->deviceHandle = HLIB_STD_StrDup(devicehandle);
	if( destinationId != NULL )
		self->destinationId = HLIB_STD_StrDup(destinationId);

	if(eOPL_SCHEDULED_PRO_RECORD == type)
	{
		APK_SCENARIO_GetStartPadding((HUINT32 *)&self->startPadding);
		APK_SCENARIO_GetEndPadding((HUINT32 *)&self->endPadding);
	}

	return self;
}

OPL_ScheduledItem_t *
	OPL_ScheduledItem_jlabsNewWithRecording (int type, unsigned long startTime, OPL_RecordingHandle identifier, const char *devicehandle, const char *destinationId)
{
	OPL_ScheduledItem_t	*self;
	OPL_Recording_t		*item = (OPL_Recording_t *)identifier;

	HxLOG_Trace();

	self = opl_scheduleditem_Init (type, sizeof(OPL_ScheduledItem_t));
	if (self == NULL)
		return NULL;

	self->startTime = startTime;
	self->duration  = item->recordingDuration;
	self->recMode = 0;

	if( devicehandle != NULL )
		self->deviceHandle = HLIB_STD_StrDup(devicehandle);
	if( destinationId != NULL )
		self->destinationId = HLIB_STD_StrDup(destinationId);
	if( item->id != NULL )
		self->dubbingId = HLIB_STD_StrDup(item->id);

	if(eOPL_SCHEDULED_PRO_RECORD == type)
	{
		APK_SCENARIO_GetStartPadding((HUINT32 *)&self->startPadding);
		APK_SCENARIO_GetEndPadding((HUINT32 *)&self->endPadding);
	}

	return self;
}


#endif


