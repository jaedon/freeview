/**************************************************************
 *	@file		meta_schedule.c
 *	appkit for SQC Output
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2011/10/24
 *	@author			humax
 *	@brief			appkit for META Network
 **************************************************************/
 #if 0
#include	<sapi.h>
#include	<oapi.h>
#include	<apk.h>

#include	"_apk_int.h"
//#include	"meta_int.h"
//#include	"appkit_int.h"

//#define	HxLOG_DOMAIN	DOMAIN_APPKIT
//#define	HxLOG_DOMAIN	"meta"

#define UID2SLOT(uid)				((HUINT32)(((uid) == 0) ? 0xFFFF : (uid)-1))
#define SLOT2UID(slot)				((HINT32)(((slot) >= 0xFF00) ? 0 : (slot) + 1))

#define	____CODEC_API___________________________________________________________________________

typedef enum
{
	eMETHOD_ADD,
	eMETHOD_CANCEL,
	eMETHOD_LIST,

	eMETHOD_NONE
} _Method_e;

typedef struct
{
	HINT32		uid;
	HINT32		conflict;
	HINT32		list[32];
	HINT32		ts_list[32];
	HINT32		size;
	HINT32		alt_list[32];
	HINT32		alt_evt_list[32];
	HINT32		alt_size;
} _AddResult_t;

//static HCHAR *		metasched_EncodeEx (const void *extension); //warning 제거
//static void *		metasched_DecodeEx (HxJSON_t json); //warning 제거
//static	HUINT32		_ulModuleSemaphoreId = 0;
#if 0
HCHAR *		METASCHED_EncodeSCHED_REPEAT (const DxSCHED_REPEAT_t *data, const HCHAR *name)
{
	HINT32		sd;
	HINT32		i, c;
	HCHAR		*ret;

	HxLOG_Trace();

	ret = APKI_OBJDEC_EncodeEnum(_meta_cycle_table.taglist, data->cycle);
	if (ret == NULL)
		return NULL;

	sd = HLIB_RWSTREAM_OpenEx(NULL, 0, APK_Malloc, APK_Free);
	if (!sd)
		return NULL;

	if (name)
		HLIB_RWSTREAM_Print(sd, "\"%s\":{\"every\":\"%s\"", name, ret);
	else
		HLIB_RWSTREAM_Print(sd, "{\"every\":\"%s\"", ret);

	if (data->cycle == eDxSCHED_CYCLE_WEEKLY)
	{
		HLIB_RWSTREAM_Print(sd, ",\"on\":[");
		for (i = 0, c = 0 ; i < 7 ; i++)
		{
			if (!(data->date & (1 << i)))
				continue;

			ret = APKI_OBJDEC_EncodeEnum(_meta_date_table.taglist, (1 << i));
			if (ret == NULL)
				continue;

			if (c > 0)
				HLIB_RWSTREAM_Print(sd, ",");
			HLIB_RWSTREAM_Print(sd, "\"%s\"", ret);
			c++;
		}
		HLIB_RWSTREAM_Print(sd, "]");
	}
	HLIB_RWSTREAM_Print(sd, "}");

	return HLIB_RWSTREAM_CloseWithoutBuf(sd);
}


HERROR		METASCHED_DecodeSCHED_REPEAT (DxSCHED_REPEAT_t *data, HxJSON_t json)
{
	HxJSON_t	value;
	HxJSON_t	date;
	HINT32		i, n;
	DxSCHED_DATE_e	d;

	HxLOG_Trace();

	if (json == (HxJSON_t)NULL || data == NULL)
		return ERR_FAIL;

	memset(data, 0, sizeof(DxSCHED_REPEAT_t));

	value = HxJSON_Object_Get(json, "every");
	if (value == NULL)
		return ERR_FAIL;
	APKI_OBJDEC_DecodeEnum(&_meta_cycle_table, value, (HUINT32)&data->cycle, 0);

	switch(data->cycle)
	{
	case eDxSCHED_CYCLE_WEEKLY:
		date = HLIB_JSON_Object_GetArray(json, "on");
		if (!date)
			return ERR_FAIL;
		n = HLIB_JSON_GetSize(date);
		for (i = 0 ; i < n ; i++)
		{
			value = HxJSON_Array_Get(date, i);
			if (!value || !HxJSON_IsString(value))
				continue;

			d = eDxSCHED_DATE_NONE;
			APKI_OBJDEC_DecodeEnum(&_meta_date_table, value, (HUINT32)&d, 0);
			data->date |= d;
		}
		if (data->date == eDxSCHED_DATE_NONE)
			return ERR_FAIL;
		break;
	case eDxSCHED_CYCLE_DAYLY:
		data->date = eDxSCHED_DATE_ALL;
		break;

	default:
		break;
	}
	return ERR_OK;
}

HCHAR *		METASCHED_EncodeSCHEDULE (const DxSCHEDULE_t *data, const HCHAR *name)
{
	HxLOG_Trace();

	{
		HINT32		stream		= HLIB_RWSTREAM_OpenEx(NULL, 0, APK_Malloc, APK_Free);
		HxJSONBIN_t	enc			= HLIB_JSONBIN_ENCODER_Open();
		const HCHAR	*encodeBuf 	= DxSchedule_EncodeBin(enc, data);

		if (name)
			HLIB_RWSTREAM_Print(stream, "\"%s\":", name);
		if (encodeBuf)
		{
			HLIB_RWSTREAM_PutString(stream, encodeBuf);
		}

		HLIB_JSONBIN_ENCODER_Close(enc);
		return (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(stream);
	}
	// TODO : encode the SCHEDULE by OAPI_TYPES API
}

HERROR		METASCHED_DecodeSCHEDULE (DxSCHEDULE_t *data, HxJSON_t json)
{
	DxSCHEDULE_t	*p;
	HxLOG_Trace();

	if (json == (HxJSON_t)NULL || data == NULL)
		return ERR_FAIL;

	memset(data, 0, sizeof(DxSCHEDULE_t));
	p = DxSchedule_DecodeBin(NULL, json);
	if (p)
	{
		HxSTD_MemCopy(data, p, sizeof(DxSCHEDULE_t));
		if (p->name)
			data->name = DxSchedule_StrDup(p->name);
		if (p->description)
			data->description = DxSchedule_StrDup(p->description);
		if (p->longDescription)
			data->longDescription = DxSchedule_StrDup(p->longDescription);
		if (p->storageID)
			data->storageID = DxSchedule_StrDup(p->storageID);
		if (p->userID)
			data->userID = DxSchedule_StrDup(p->userID);
		if (p->deviceHandle)
			data->deviceHandle = DxSchedule_StrDup(p->deviceHandle);
		if (p->destinationId)
			data->destinationId = DxSchedule_StrDup(p->destinationId);
		if (p->dubbingId)
			data->dubbingId = DxSchedule_StrDup(p->dubbingId);

		DxSchedule_FreeBin(p);
		return ERR_OK;
	}

	return ERR_FAIL;
}
#endif


#define	____PRIVATE_TYPE___________________________________________________________________________

typedef struct _META_SCHEDULER_t_
{
	HxVector_t			*list;
	HUINT32				accessCode;
	HUINT32				notifierId;
	ApkMetaSched_Listener_t	listener;
	void *				userdata;
} _META_SCHEDULER_t;

#define	____PRIVATE_API___________________________________________________________________________

static _META_SCHEDULER_t	*	metasched_GetManager (void);

static HINT32		metasched_CompareUniqueID (const void *arg1, const void *arg2)
{
	const DxSCHEDULE_t *lhs = *(const DxSCHEDULE_t **)arg1;
	const DxSCHEDULE_t *rhs = *(const DxSCHEDULE_t **)arg2;

	return lhs->uid - rhs->uid;
}

#define ____CONVERT_FUNCTIONS_____

static INLINE DxSCHEDULE_t *metasched_SetActionByRsvType (DxSCHEDULE_t *sched, DxRsvType_e type)
{
	switch (type)
	{
	case DxRSVTYPE_WATCHING_EBR:
		sched->action = eDxSCHED_ACTION_WATCH;
		sched->eventBase = TRUE;
		break;

	case DxRSVTYPE_WATCHING_TBR:
		sched->action = eDxSCHED_ACTION_WATCH;
		sched->eventBase = FALSE;
		break;

	case DxRSVTYPE_RECORD_EBR:
		sched->action = eDxSCHED_ACTION_RECORD;
		sched->eventBase = TRUE;
		break;

	case DxRSVTYPE_RECORD_SERIES_EBR:
		sched->action = eDxSCHED_ACTION_RECORD;
		sched->eventBase = TRUE;
		sched->param.record.seriesRec = TRUE;
		break;

	case DxRSVTYPE_RECORD_TBR:
	case DxRSVTYPE_RECORD_INSTANT:
	case DxRSVTYPE_RECORD_DELAYED:
	case DxRSVTYPE_RECORD_SATRECORD_TBR:
		sched->action = eDxSCHED_ACTION_RECORD;
		sched->eventBase = FALSE;
		break;

	case DxRSVTYPE_POWER_ON:
		sched->action = eDxSCHED_ACTION_POWER;
		sched->param.power.onOff = TRUE;
		break;

	case DxRSVTYPE_POWER_OFF:
		sched->action = eDxSCHED_ACTION_POWER;
		sched->param.power.onOff = FALSE;
		break;

	case DxRSVTYPE_IP_EPG_GATHERING:
		sched->action = eDxSCHED_ACTION_CHECK;
		sched->param.check.type = eDxSCHED_CHECK_IPEPG;
		break;

	case DxRSVTYPE_IP_REMOTE_PROGRAMMING:
		sched->action = eDxSCHED_ACTION_CHECK;
		sched->param.check.type = eDxSCHED_CHECK_RP;
		break;

	case DxRSVTYPE_EMM:
		sched->action = eDxSCHED_ACTION_CHECK;
		sched->param.check.type = eDxSCHED_CHECK_EMM;
		break;

	case DxRSVTYPE_NETSTREAMING_LIVE:
		sched->action = eDxSCHED_ACTION_NETSTREAMING_LIVE;
		sched->eventBase = FALSE;
		break;

	case DxRSVTYPE_NETSTREAMING_FILE:
		sched->action = eDxSCHED_ACTION_NETSTREAMING_FILE;
		sched->eventBase = FALSE;
		break;

	case DxRSVTYPE_REGULAR_OTA:
	case DxRSVTYPE_EPG_GATHERING:
	case DxRSVTYPE_UPDATE_TASK:
	case DxRSVTYPE_ASO_NOTIFY:
	case DxRSVTYPE_MYEPG_UPDATE:
	case DxRSVTYPE_NET_UPDATE:
	case DxRSVTYPE_BLURAY_TBR:
	case DxRSVTYPE_BLURAY_EBR:
	default:
		HxLOG_Error("[%s:%d] Not implemented yet...\n", __HxFILE__, __HxLINE__);
		return NULL;
	}
	return sched;
}

static INLINE DxRsvRepeat_e metasched_ToRsvRepeat (DxSCHED_REPEAT_t *repeat)
{
	switch (repeat->cycle)
	{
	case eDxSCHED_CYCLE_NONE:
		break;

	case eDxSCHED_CYCLE_DAYLY:
		return DxRSVREPEAT_DAILY;

	case eDxSCHED_CYCLE_WEEKLY:
		if (repeat->date == eDxSCHED_DATE_WEEKDAYS)
			return DxRSVREPEAT_WEEKDAYS;
		if (repeat->date == eDxSCHED_DATE_WEEKEND)
			return DxRSVREPEAT_WEEKEND;
		if (repeat->date == ( eDxSCHED_DATE_WEEKDAYS
							| eDxSCHED_DATE_SAT))
			return DxRSVREPEAT_WITHOUT_SUNDAY;

		switch(repeat->date)
		{
		case eDxSCHED_DATE_SUN:
		case eDxSCHED_DATE_MON:
		case eDxSCHED_DATE_TUE:
		case eDxSCHED_DATE_WED:
		case eDxSCHED_DATE_THU:
		case eDxSCHED_DATE_FRI:
		case eDxSCHED_DATE_SAT:
			return DxRSVREPEAT_WEEKLY;
		default:
			return DxRSVREPEAT_FEWDAY_OF_WEEK;
		}
		break;

	case eDxSCHED_CYCLE_MONTHLY:
	case eDxSCHED_CYCLE_YEARLY:
	default:
		HxLOG_Error("[%s:%d] Not implemented yet...\n", __HxFILE__, __HxLINE__);
		break;
	}
	return DxRSVREPEAT_ONCE;
}

static INLINE DxSCHED_REPEAT_t *metasched_ToSCHED_REPEAT (DxSCHED_REPEAT_t *repeat, DxRsvRepeat_e rsv, HUINT32 ulDays, UNIXTIME time)
{
	HERROR		err;
	HxDATETIME_Date_t	date;
	static const DxSCHED_DATE_e	s_dayOfWeekMap[] = {
		  eDxSCHED_DATE_SUN
		, eDxSCHED_DATE_MON
		, eDxSCHED_DATE_TUE
		, eDxSCHED_DATE_WED
		, eDxSCHED_DATE_THU
		, eDxSCHED_DATE_FRI
		, eDxSCHED_DATE_SAT
	};

	switch (rsv)
	{
	case DxRSVREPEAT_DAILY:
		repeat->cycle = eDxSCHED_CYCLE_DAYLY;
		repeat->date = eDxSCHED_DATE_ALL;
		break;

	case DxRSVREPEAT_WEEKLY:
		err = HLIB_DATETIME_ConvertUnixTimeToDate(time, &date);
		if (err != ERR_OK)
			return NULL;

		repeat->cycle = eDxSCHED_CYCLE_WEEKLY;
		repeat->date  = s_dayOfWeekMap[HLIB_DATETIME_GetWeekDayIndex(date) % 7];
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
	return repeat;
}

static INLINE DxRsvRepeat_e metasched_GetRsvTypebyCheckType (DxSCHED_CHECK_e etype)
{
	switch(etype)
	{
	case eDxSCHED_CHECK_RP :		return DxRSVTYPE_IP_REMOTE_PROGRAMMING;
	case eDxSCHED_CHECK_IPEPG :		return DxRSVTYPE_IP_EPG_GATHERING;
	case eDxSCHED_CHECK_EMM :		return DxRSVTYPE_EMM;
	default :						return DxRSVTYPE_NOTHING;
	}
}
static INLINE DxRsvRepeat_e metasched_GetRsvType (const DxSCHEDULE_t *pstApkSched)
{
	switch (pstApkSched->action)
	{
	case eDxSCHED_ACTION_WATCH:
		return pstApkSched->eventBase ? DxRSVTYPE_WATCHING_EBR : DxRSVTYPE_WATCHING_TBR;

	case eDxSCHED_ACTION_RECORD:
		return pstApkSched->eventBase ? (pstApkSched->param.record.seriesRec ? DxRSVTYPE_RECORD_SERIES_EBR : DxRSVTYPE_RECORD_EBR) : (DxRSVTYPE_RECORD_TBR);

	case eDxSCHED_ACTION_POWER:
		return pstApkSched->param.power.onOff ? DxRSVTYPE_POWER_ON : DxRSVTYPE_POWER_OFF;

	case eDxSCHED_ACTION_NETSTREAMING_LIVE:
		return DxRSVTYPE_NETSTREAMING_LIVE;

	case eDxSCHED_ACTION_NETSTREAMING_FILE:
		return DxRSVTYPE_NETSTREAMING_FILE;

	case eDxSCHED_ACTION_CHECK:
		return metasched_GetRsvTypebyCheckType (pstApkSched->param.check.type);

	default:
		break;
	}

	return DxRSVTYPE_NOTHING;
}

static DxSCHEDULE_t *metasched_ConvertDxSchedule2ApkSchedule (DxSCHEDULE_t *pstDst, DxSchedule_t *pstSrc)
{
	UNIXTIME			 utSrcStartTime;

	HxSTD_MemSet (pstDst, 0, sizeof(DxSCHEDULE_t));

	if (metasched_SetActionByRsvType (pstDst, pstSrc->eRsvType) == NULL)
	{
        HxLOG_Error("[%s:%d] metasched_SetActionByRsvType fail rsvType(%d)!!\n"
            , __HxFILE__, __HxLINE__, pstSrc->eRsvType);
		return NULL;
	}

	(void)HLIB_DATETIME_ConvertDateTimeToUnixTime (&(pstSrc->stStartTime), &utSrcStartTime);

	pstDst->uid			= SLOT2UID(pstSrc->ulSlot);
	pstDst->schedId		= SLOT2UID(pstSrc->ulSlot);
	pstDst->svcUId		= pstSrc->ulSvcHandle;
	pstDst->epgType		= eDxEPG_TYPE_RF;
	pstDst->startTime	= utSrcStartTime;
	pstDst->duration	= pstSrc->ulDuration;
	pstDst->preStart	= pstSrc->eRsvReady;

	(void)metasched_ToSCHED_REPEAT (&(pstDst->repeat), pstSrc->eRepeat, pstSrc->ulRepeatDays, utSrcStartTime);

	switch (pstDst->action)
	{
	case eDxSCHED_ACTION_WATCH:
		pstDst->eventId		= (HINT32)pstSrc->uExtInfo.stWch.nEvtId;
		pstDst->param.watch.seriesId	= (HINT32)pstSrc->uExtInfo.stWch.ulSeriesId;
		if (pstSrc->uExtInfo.stWch.szEvtName[0] != 0)
		{
			pstDst->name	= APK_StrDup (pstSrc->uExtInfo.stWch.szEvtName);
		}
		pstDst->param.watch.programId = (HINT32)pstSrc->uExtInfo.stWch.ulProgramId;
		break;

	case eDxSCHED_ACTION_RECORD:
		pstDst->eventId		= (HINT32)pstSrc->uExtInfo.stRec.nEvtId;
		if (pstSrc->uExtInfo.stRec.szEventName[0] != 0)
		{
			pstDst->name	= APK_StrDup (pstSrc->uExtInfo.stRec.szEventName);
		}
		pstDst->param.record.startPadding	= pstSrc->uExtInfo.stRec.ulPreOffsetTime;
		pstDst->param.record.endPadding		= pstSrc->uExtInfo.stRec.ulPostOffsetTime;
		if (pstSrc->uExtInfo.stRec.aucStorageUUID[0] != 0)
		{
			pstDst->storageID	= APK_StrDup (pstSrc->uExtInfo.stRec.aucStorageUUID);
		}

		break;

	case eDxSCHED_ACTION_POWER:
		pstDst->param.power.volume	= (HINT32)pstSrc->uExtInfo.stPwr.ucVolume;
		break;

	case eDxSCHED_ACTION_NETSTREAMING_LIVE:
	case eDxSCHED_ACTION_NETSTREAMING_FILE:
		if (pstSrc->uExtInfo.stHLS.szHLSName[0] != 0)
		{
			pstDst->name		= APK_StrDup (pstSrc->uExtInfo.stHLS.szHLSName);
		}

		pstDst->eventId	= 0;
		pstDst->param.hls.hProfileHandle		= (HUINT32)pstSrc->uExtInfo.stHLS.hProfileHandle;
		pstDst->param.hls.ringbuf_handle		= (HUINT32)pstSrc->uExtInfo.stHLS.ringbuf_handle;
		pstDst->param.hls.root_m3u8_handle		= (HUINT32)pstSrc->uExtInfo.stHLS.root_m3u8_handle;
		pstDst->param.hls.bitrate_m3u8_handle	= (HUINT32)pstSrc->uExtInfo.stHLS.bitrate_m3u8_handle;
		break;

	default:
		break;
	}

	return pstDst;
}

static DxSchedule_t *metasched_ConvertApkSchedule2DxSchedule (DxSchedule_t *pstDst, const DxSCHEDULE_t *pstSrc)
{

	HxSTD_MemSet (pstDst, 0, sizeof(DxSchedule_t));

	pstDst->eRsvType = metasched_GetRsvType (pstSrc);
	if (DxRSVTYPE_NOTHING == pstDst->eRsvType)
	{
		return NULL;
	}

	pstDst->ulSlot		= UID2SLOT(pstSrc->uid);
	pstDst->ulSvcHandle	= pstSrc->svcUId;
	pstDst->eRsvReady	= DxRSVREADY_30_SEC;
	HLIB_DATETIME_ConvertUnixTimeToDateTime (pstSrc->startTime, &(pstDst->stStartTime));
	pstDst->ulDuration	= (HUINT32)pstSrc->duration;
	pstDst->eRepeat		= metasched_ToRsvRepeat (&(pstSrc->repeat));

	switch (pstSrc->action)
	{
	case eDxSCHED_ACTION_WATCH:
		if (pstSrc->eventBase)
		{
			pstDst->uExtInfo.stWch.nEvtId		= pstSrc->eventId;
			pstDst->uExtInfo.stWch.ulSeriesId	= pstSrc->param.watch.seriesId;
		}

		break;

	case eDxSCHED_ACTION_RECORD:
		if ((NULL != pstSrc->storageID) && (0 != pstSrc->storageID[0]))
		{
			HLIB_STD_StrNCpySafe (pstDst->uExtInfo.stRec.aucStorageUUID, pstSrc->storageID, DxRSV_MAX_UUID_BUFSIZE);
		}

		if (pstSrc->eventBase)
		{
			pstDst->uExtInfo.stRec.nEvtId	= pstSrc->eventId;
			pstDst->uExtInfo.stRec.ulPreOffsetTime	= pstSrc->param.record.startPadding;
			pstDst->uExtInfo.stRec.ulPostOffsetTime	= pstSrc->param.record.endPadding;
		}

		break;

	case eDxSCHED_ACTION_NETSTREAMING_LIVE:
	case eDxSCHED_ACTION_NETSTREAMING_FILE:
		pstDst->uExtInfo.stHLS.hProfileHandle		= pstSrc->param.hls.hProfileHandle;
		pstDst->uExtInfo.stHLS.ringbuf_handle		= pstSrc->param.hls.ringbuf_handle;
		pstDst->uExtInfo.stHLS.root_m3u8_handle		= pstSrc->param.hls.root_m3u8_handle;
		pstDst->uExtInfo.stHLS.bitrate_m3u8_handle	= pstSrc->param.hls.bitrate_m3u8_handle;
		break;

	case eDxSCHED_ACTION_POWER:
		pstDst->uExtInfo.stPwr.ucVolume				= (HUINT8)pstSrc->param.power.volume;
		break;

	default:
		break;
	}

	return pstDst;
}

static APK_META_SCH_Conflict_e metasched_ConvertMetaConfType2ApksConfType (APK_META_SCH_Conflict_e eType)
{
	switch (eType)
	{
	case eAPK_METASCH_CONFTYPE_None:			return eAPK_METASCH_CONFTYPE_None;
	case eAPK_METASCH_CONFTYPE_Tp:				return eAPK_METASCH_CONFTYPE_Tp;
	case eAPK_METASCH_CONFTYPE_OverCapacity:	return eAPK_METASCH_CONFTYPE_OverCapacity;
	case eAPK_METASCH_CONFTYPE_View:			return eAPK_METASCH_CONFTYPE_View;
	case eAPK_METASCH_CONFTYPE_Duplicated:		return eAPK_METASCH_CONFTYPE_Duplicated;
	case eAPK_METASCH_CONFTYPE_PaddingTime:		return eAPK_METASCH_CONFTYPE_PaddingTime;
	case eAPK_METASCH_CONFTYPE_Alternative:		return eAPK_METASCH_CONFTYPE_Alternative;
	case eAPK_METASCH_CONFTYPE_RfOta:			return eAPK_METASCH_CONFTYPE_RfOta;
	case eAPK_METASCH_CONFTYPE_DataOnly:		return eAPK_METASCH_CONFTYPE_DataOnly;
	case eAPK_METASCH_CONFTYPE_Unknown:			return eAPK_METASCH_CONFTYPE_Unknown;
	default:			break;
	}

	return eAPK_METASCH_CONFTYPE_Unknown;
}

static APKS_SCHED_CONFLICT_t *metasched_ConvertMetaConflict2ApksConflict (APKS_SCHED_CONFLICT_t *pstOldConf, APK_META_SCH_Conflict_t *pstApkConf)
{
	if (NULL != pstOldConf)
	{
		HxSTD_MemSet(pstOldConf, 0, sizeof(APKS_SCHED_CONFLICT_t));
	}

	if ((NULL != pstOldConf) && (NULL != pstApkConf))
	{
		HUINT32				 ulCnt;
		HUINT32				 ulLength = (pstApkConf->ulNumConflict > APKD_META_SCHED_MAX_CONFLICT) ? APKD_META_SCHED_MAX_CONFLICT : pstApkConf->ulNumConflict;

		pstOldConf->type = metasched_ConvertMetaConfType2ApksConfType(pstApkConf->eType);
		if ((pstApkConf->ulNumConflict > 0) && (NULL != pstApkConf->pstConfArray))
		{
			pstOldConf->length[0] = ulLength;
			for (ulCnt = 0; ulCnt < ulLength; ulCnt++)
			{
				pstOldConf->uidList[0][ulCnt] = pstApkConf->pstConfArray[ulCnt].ulSlot;
			}
		}
	}

	return pstOldConf;
}

static void metasched_NotifyByApkMetaSch (void *pvUserData, APK_META_SCH_MsgType_e eMsgType, HUINT32 ulSlot)
{
	HINT32					 nEvent;
	HUINT32					 ulUid = SLOT2UID (ulSlot);
	HINT32					 nArgs[4];
	_META_SCHEDULER_t		*pstScheduler = (_META_SCHEDULER_t *)pvUserData;

	if (NULL != pstScheduler->listener)
	{
		//typedef void (* ApkMetaSched_Listener_t) (ApkMetaSched_Handle_t self, HINT32 event, const HINT32 *args, void *userdata);

		nArgs[0] = (HINT32)ulUid;

		switch (eMsgType)
		{
		case eAPK_METASCH_MSG_SchedulesUpdated:
			nEvent = eSCHEDULE_EVENT_LOAD_COMPLETED;
			break;

		case eAPK_METASCH_MSG_ScheduleAdded:
		case eAPK_METASCH_MSG_ScheduleChanged:
		case eAPK_METASCH_MSG_ScheduleRemoved:
			nEvent = eSCHEDULE_EVENT_UPDATED;
			break;

		case eAPK_METASCH_MSG_TimeUpReady:
			nEvent = eSCHEDULE_EVENT_PRESTART;
			break;

		case eAPK_METASCH_MSG_TimeUpStart:
			nEvent = eSCHEDULE_EVENT_STARTED;
			break;

		case eAPK_METASCH_MSG_TimeUpStop:
			nEvent = eSCHEDULE_EVENT_COMPLETED;
			break;

		case eAPK_METASCH_MSG_ConflictScheduleAdded:
		{
			APKS_SCHED_CONFLICT_t		*pstOldConflict;
			APK_META_SCH_Conflict_t		*pstNewConflict;
			DxSCHEDULE_t				*pstOldSchedule;

			pstOldConflict = (APKS_SCHED_CONFLICT_t *)HLIB_STD_MemCalloc(sizeof(APKS_SCHED_CONFLICT_t));
			pstOldSchedule = (DxSCHEDULE_t *)HLIB_STD_MemCalloc(sizeof(DxSCHEDULE_t));

			pstNewConflict = APK_META_SCH_GetConflictResult(ulSlot);
			if (NULL != pstNewConflict)
			{
				(void)metasched_ConvertDxSchedule2ApkSchedule(pstOldSchedule, &(pstNewConflict->stTriedSchedule));
				(void)metasched_ConvertMetaConflict2ApksConflict(pstOldConflict, pstNewConflict);
				nArgs[1] = (HINT32)pstOldConflict;
				nArgs[2] = (HINT32)pstOldSchedule;
			}

			nEvent = eSCHEDULE_EVENT_RSV_CONFLICT;
			break;
		}

		default:
			return;
		}

		pstScheduler->listener ((ApkMetaSched_Handle_t)pstScheduler, nEvent, (const HINT32 *)nArgs, pstScheduler->userdata);
	}
}

#if	0
static HCHAR *		metasched_Encode (const void *obj)
{
	const DxSCHEDULE_t	*data = (const DxSCHEDULE_t *)obj;

	if (data == NULL)
		return NULL;

	return METASCHED_EncodeSCHEDULE(data, NULL);
}
#endif

static void *		metasched_Release (void *obj)
{
	DxSCHEDULE_t *sched;

	if (obj == NULL)
		return NULL;

	sched = (DxSCHEDULE_t *)obj;

	if (sched->name)
		APK_Free(sched->name);
	if (sched->description)
		APK_Free(sched->description);
	if (sched->longDescription)
		APK_Free(sched->longDescription);
	if (sched->storageID)
		APK_Free(sched->storageID);
	if (sched->userID)
		APK_Free(sched->userID);
	if (sched->deviceHandle)
		APK_Free(sched->deviceHandle);
	if (sched->destinationId)
		APK_Free(sched->destinationId);
	if (sched->dubbingId)
		APK_Free(sched->dubbingId);

	APK_Free(obj);
	return NULL;
}

static _META_SCHEDULER_t *	metasched_GetManager (void)
{
	static _META_SCHEDULER_t s_manager;

	{
		STATIC HBOOL			 s_bInited = FALSE;

		if (TRUE != s_bInited)
		{
			(void)APK_META_SCH_Init();
			(void)APK_META_SCH_RegistNotifier (metasched_NotifyByApkMetaSch, (void *)&s_manager);
			s_bInited = TRUE;
		}
	}

	if (s_manager.list == NULL)
	{
		s_manager.list = HLIB_VECTOR_NewEx(NULL, 0, NULL, metasched_Release, metasched_CompareUniqueID);
		HxLOG_Assert(s_manager.list != NULL);
	}
	return &s_manager;
}

static void			metasched_RemoveSchedule (_META_SCHEDULER_t *scheduler, HINT32 uid)
{
	DxSCHEDULE_t sched;
	HINT32			index;

	sched.uid = uid;
	index = HLIB_VECTOR_IndexOf(scheduler->list, &sched);
	if (index < 0)
		return;
	(void) HLIB_VECTOR_Remove(scheduler->list, index);
}

static HBOOL	metasched_FindSameAction (const DxSCHEDULE_t *sched, void *action)
{
	if (sched && sched->action == (DxSCHED_ACTION_e)action)
		return TRUE;
	return FALSE;
}


#define	____PUBLIC_API______________________________________________________________________________

ApkMetaSched_Handle_t	APK_METASCHED_GetInstance (void)
{
	HxLOG_Trace();

	return (ApkMetaSched_Handle_t)metasched_GetManager();
}

void		APK_METASCHED_Delete (DxSCHEDULE_t *sched)
{
	if (sched)
	{
		/**
		 * extension 이 scheduler에 포함되지만, init 후에는 건드리지 않을 것이므로..
		 * 세마포 보호 없이 그냥 release
		 */
		(void) metasched_Release((void *)sched);
	}
}


void		APK_METASCHED_RemoveFromList (HINT32 uid)
{
	metasched_RemoveSchedule(metasched_GetManager(), uid);
}


void		APK_METASCHED_AddListener (ApkMetaSched_Handle_t hMeta, ApkMetaSched_Listener_t listener, void *userdata)
{
	_META_SCHEDULER_t	*self = (_META_SCHEDULER_t *)hMeta;

	self->listener = listener;
	self->userdata = userdata;
}


DxSCHEDULE_t *	APK_METASCHED_Get (ApkMetaSched_Handle_t hMeta, HINT32 uid)
{
	HUINT32				 ulSlot = UID2SLOT (uid);
	DxSchedule_t		 stDxSched;
	DxSCHEDULE_t		*pstApkSched;
	HERROR				 hErr;

	hErr = APK_META_SCH_GetSchedule (ulSlot, &stDxSched);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("[%s:%d] APK_META_SCH_GetSchedule\n", __FUNCTION__, __LINE__);
		return NULL;
	}

	pstApkSched = (DxSCHEDULE_t *)HLIB_STD_MemAlloc (sizeof(DxSCHEDULE_t));
	if (NULL == pstApkSched)
	{
		HxLOG_Error ("[%s:%d] Memory allocation failed:\n", __FUNCTION__, __LINE__);
		return NULL;
	}

	metasched_ConvertDxSchedule2ApkSchedule (pstApkSched, &stDxSched);
	return pstApkSched;
}


HINT32		APK_METASCHED_Add (ApkMetaSched_Handle_t hMeta, const DxSCHEDULE_t *data, HBOOL bWithAlternative, APKS_SCHED_CONFLICT_t *conflict)
{
	HUINT32				 ulSlot, ulUid;
	DxSchedule_t		 stDxSched;
	HERROR				 hErr;

	if (NULL == data)				{ return 0; }
	metasched_ConvertApkSchedule2DxSchedule(&stDxSched, data);

	hErr = APK_META_SCH_AddSchedule(&stDxSched, &ulSlot);
	if (ERR_OK != hErr)
	{
		APK_META_SCH_Conflict_t		*pstApkConf = APK_META_SCH_GetConflictResult(ulSlot);

		HxLOG_Error("[%s:%d] APK_META_SCH_AddSchedule err:\n", __FUNCTION__, __LINE__);

		if (NULL != pstApkConf)
		{
			metasched_ConvertMetaConflict2ApksConflict(conflict, pstApkConf);
			APK_META_SCH_FreeConflictResult(pstApkConf);
		}

		return 0;
	}

	ulUid = SLOT2UID(ulSlot);
	HxLOG_Debug ("[%s:%d] SlotId(%d), UID(%d)\n", __FUNCTION__, __LINE__, ulSlot, ulUid);

	return (HINT32)ulUid;
}


HERROR		APK_METASCHED_Update (ApkMetaSched_Handle_t hMeta, HINT32 uid, const DxSCHEDULE_t *data, APKS_SCHED_CONFLICT_t *conflict)
{
	HUINT32				 ulSlot = UID2SLOT(uid);
	DxSchedule_t		 stDxSched;
	HERROR				 hErr;

	if (NULL == data)				{ return ERR_FAIL; }
	metasched_ConvertApkSchedule2DxSchedule(&stDxSched, data);

	hErr = APK_META_SCH_UpdateSchedule(ulSlot, &stDxSched);
	if (ERR_OK != hErr)
	{
		APK_META_SCH_Conflict_t		*pstApkConf = APK_META_SCH_GetConflictResult(ulSlot);

		HxLOG_Error ("[%s:%d] APK_META_SCH_UpdateSchedule err:\n", __FUNCTION__, __LINE__);

		if (NULL != pstApkConf)
		{
			metasched_ConvertMetaConflict2ApksConflict(conflict, pstApkConf);
			APK_META_SCH_FreeConflictResult(pstApkConf);
		}

		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR		APK_METASCHED_Refresh (ApkMetaSched_Handle_t hMeta)
{
	return ERR_OK;
}

HERROR		APK_METASCHED_Confirm (ApkMetaSched_Handle_t hMeta, HINT32 uid, HINT32 type, HINT32 check)
{
	return ERR_OK;
}

HERROR		APK_METASCHED_Remove (ApkMetaSched_Handle_t hMeta, HINT32 uid)
{
	HUINT32				 ulSlot = UID2SLOT (uid);
	HERROR				 hErr;

	hErr = APK_META_SCH_CancelSchedule (ulSlot);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("[%s:%d] APK_META_SCH_CancelSchedule err:\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	return ERR_OK;
}

HINT32		APK_METASCHED_NextSeriesItemAdd (ApkMetaSched_Handle_t hMeta, const DxSCHEDULE_t *data)
{
	// ToDo: Series Recording 관련은 나중에 구현
	return 0;
}


// Series Recording 의 경우 User가 원하면 현재 Conflict나는 이벤트만 삭제하고 나머지 시리즈는 녹화 가능해야 한다.
HERROR		APK_METASCHED_RemoveFirstItemOnSeries (ApkMetaSched_Handle_t hMeta, HINT32 uid)
{
	// ToDo: Series Recording 관련은 나중에 구현
	return ERR_FAIL;
}


void		APK_METASCHED_RemoveIf (ApkMetaSched_Handle_t hMeta, HBOOL (* _if) (const DxSCHEDULE_t *, void *), void *userdata)
{
	HUINT32				 ulIndex, ulSchNum = 0;
	DxSCHEDULE_t		 stApkItemSch;
	DxSchedule_t		 stDxItemSch;
	HERROR				 hErr;

	(void)APK_META_SCH_CountSchedule (&ulSchNum);
	if (ulSchNum > 0)
	{
		for (ulIndex = 0; ulIndex < ulSchNum; ulIndex++)
		{
			hErr = APK_META_SCH_GetScheduleByIndex (ulIndex, &stDxItemSch);
			if (ERR_OK == hErr)
			{
				(void)metasched_ConvertDxSchedule2ApkSchedule (&stApkItemSch, &stDxItemSch);
				if (_if(&stApkItemSch, userdata))
				{
					APK_META_SCH_CancelSchedule (stDxItemSch.ulSlot);
				}
			}
		}

	}
}

void		APK_METASCHED_RemoveAll (ApkMetaSched_Handle_t hMeta)
{
	HUINT32				 ulIndex, ulSchNum = 0;
	DxSCHEDULE_t		 stApkItemSch;
	DxSchedule_t		 stDxItemSch;
	HERROR				 hErr;

	(void)APK_META_SCH_CountSchedule (&ulSchNum);
	if (ulSchNum > 0)
	{
		for (ulIndex = 0; ulIndex < ulSchNum; ulIndex++)
		{
			hErr = APK_META_SCH_GetScheduleByIndex (ulIndex, &stDxItemSch);
			if (ERR_OK == hErr)
			{
				(void)metasched_ConvertDxSchedule2ApkSchedule (&stApkItemSch, &stDxItemSch);
				APK_META_SCH_CancelSchedule (stDxItemSch.ulSlot);
			}
		}

	}
}

HERROR		APK_METASCHED_Check (ApkMetaSched_Handle_t hMeta, HUINT32 time, DxSCHED_REPEAT_t repeat, HINT32 svcuid, DxSCHED_CHECK_e checkType)
{
	// ToDo: 정확한 목적을 모르는 기능은 차후 구현
	return ERR_OK;
}

DxSCHEDULE_t	*APK_METASCHED_GetCheck (ApkMetaSched_Handle_t hMeta, DxSCHED_CHECK_e checkType)
{
	// ToDo: 정확한 목적을 모르는 기능은 차후 구현
	return NULL;
}


HERROR		APK_METASCHED_Wakeup (ApkMetaSched_Handle_t hMeta, HUINT32 time, DxSCHED_REPEAT_t repeat, HINT32 svcuid, HINT32 volume)
{
	HUINT32				 ulSlot;
	DxSchedule_t		 stDxSched;
	HERROR				 hErr;

	(void)APK_METASCHED_RemoveIf (hMeta, metasched_FindSameAction, (void *)eDxSCHED_ACTION_POWER);

	HxSTD_MemSet (&stDxSched, 0, sizeof(DxSchedule_t));

	stDxSched.eRsvType	= DxRSVTYPE_POWER_ON;
	(void)HLIB_DATETIME_ConvertUnixTimeToDateTime (time, &(stDxSched.stStartTime));
	stDxSched.ulDuration	= 1;
	stDxSched.eRepeat		= metasched_ToRsvRepeat (&repeat);
	stDxSched.ulSvcHandle	= svcuid;
	stDxSched.uExtInfo.stPwr.ucVolume	= volume;

	hErr = APK_META_SCH_AddSchedule (&stDxSched, &ulSlot);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("[%s:%d] APK_META_SCH_AddSchedule err:\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	return ERR_OK;
}

static void*	metasched_Clone(const void *obj)
{
	DxSCHEDULE_t	*sched;
	DxSCHEDULE_t	*clone;

	if (obj == NULL)
		return NULL;

	sched = (DxSCHEDULE_t *)obj;
	clone = (DxSCHEDULE_t *)APK_MemDup(sched, sizeof(DxSCHEDULE_t));
	if (clone)
	{
		if (sched->name)
			clone->name = APK_StrDup(sched->name);
		if (sched->description)
			clone->description = APK_StrDup(sched->description);
		if (sched->longDescription)
			clone->longDescription = APK_StrDup(sched->longDescription);
		if (sched->storageID)
			clone->storageID = APK_StrDup(sched->storageID);
		if (sched->userID)
			clone->userID = APK_StrDup(sched->userID);
		if (sched->deviceHandle)
			clone->deviceHandle = APK_StrDup(sched->deviceHandle);
		if (sched->destinationId)
			clone->destinationId = APK_StrDup(sched->destinationId);
		if (sched->dubbingId)
			clone->dubbingId = APK_StrDup(sched->dubbingId);
	}
	return clone;
}

DxSCHEDULE_t	*APK_METASCHED_GetWakeup (ApkMetaSched_Handle_t hMeta)
{
	HUINT32				 ulIndex, ulSchNum = 0;
	DxSCHEDULE_t		 stApkItemSch;
	DxSchedule_t		 stDxItemSch;
	HERROR				 hErr;

	(void)APK_META_SCH_CountSchedule (&ulSchNum);
	if (ulSchNum > 0)
	{
		for (ulIndex = 0; ulIndex < ulSchNum; ulIndex++)
		{
			hErr = APK_META_SCH_GetScheduleByIndex (ulIndex, &stDxItemSch);
			if (ERR_OK == hErr)
			{
				if (DxRSVTYPE_POWER_ON == stDxItemSch.eRsvType)
				{
					(void)metasched_ConvertDxSchedule2ApkSchedule (&stApkItemSch, &stDxItemSch);
					return (DxSCHEDULE_t *)metasched_Clone(&stApkItemSch);
				}
			}
		}
	}

	return NULL;
}

HUINT32		APK_METASCHED_Count (ApkMetaSched_Handle_t hMeta)
{
	HUINT32			 ulCount = 0;

	(void)APK_META_SCH_CountSchedule (&ulCount);
	return ulCount;
}

DxSCHEDULE_t	*APK_METASCHED_GetAt (ApkMetaSched_Handle_t hMeta, HUINT32 index)
{
	DxSCHEDULE_t		 stApkItemSch;
	DxSchedule_t		 stDxItemSch;
	HERROR				 hErr;

	hErr = APK_META_SCH_GetScheduleByIndex (index, &stDxItemSch);
	if (ERR_OK == hErr)
	{
		(void)metasched_ConvertDxSchedule2ApkSchedule (&stApkItemSch, &stDxItemSch);
		return (DxSCHEDULE_t *)metasched_Clone(&stApkItemSch);
	}

	return NULL;
}

HBOOL	APK_METASCHED_SetName (HINT32 sched_uid, HCHAR *name)
{
	HUINT32				 ulSlot = UID2SLOT (sched_uid);
	DxSchedule_t		 stDxItemSch;
	HERROR				 hErr;

	if (NULL != name)
	{
		hErr = APK_META_SCH_GetSchedule (ulSlot, &stDxItemSch);
		if (ERR_OK == hErr)
		{
			switch (stDxItemSch.eRsvType)
			{
			case DxRSVTYPE_WATCHING_EBR:
			case DxRSVTYPE_WATCHING_TBR:
				HLIB_STD_StrNCpySafe(stDxItemSch.uExtInfo.stWch.szEvtName, name, DxRSV_MAX_EVENT_NAME);
				break;

			case DxRSVTYPE_RECORD_EBR:
			case DxRSVTYPE_RECORD_TBR:
			case DxRSVTYPE_RECORD_INSTANT:
			case DxRSVTYPE_RECORD_DELAYED:
			case DxRSVTYPE_DUBBING:
			case DxRSVTYPE_RECORD_SERIES_EBR:
			case DxRSVTYPE_RECORD_SATRECORD_TBR:
				HLIB_STD_StrNCpySafe(stDxItemSch.uExtInfo.stRec.szEventName, name, DxRSV_MAX_EVENT_NAME);
				break;

			case DxRSVTYPE_NETSTREAMING_LIVE:
				HLIB_STD_StrNCpySafe(stDxItemSch.uExtInfo.stHLS.szHLSName, name, DxRSV_HLS_NAME_LEN);
				break;

			case DxRSVTYPE_NETSTREAMING_FILE:
				HLIB_STD_StrNCpySafe(stDxItemSch.uExtInfo.stHLS.szHLSName, name, DxRSV_HLS_NAME_LEN);
				break;

			default:
				return FALSE;
			}

			hErr = APK_META_SCH_UpdateSchedule (ulSlot, &stDxItemSch);
			if (ERR_OK == hErr)
			{
				return TRUE;
			}
		}

	}

	return FALSE;
}


void	APK_METASCHED_DumpAll (ApkMetaSched_Handle_t hMeta)
{
#if defined(CONFIG_DEBUG)
	DxSCHEDULE_t	*item;
	HUINT32			i, n;



	HxLOG_Debug("<%s:%d>\n", __FUNCTION__,__LINE__);

	n = APK_METASCHED_Count(hMeta);
	for (i = 0 ; i < n ; i++)
	{
		item = APK_METASCHED_GetAt(hMeta, i);
		if (item == NULL)
			continue;

//		APK_METASCHED_DumpSchedule(item);
		APK_METASCHED_Delete(item);
	}
	HxLOG_Debug("</%s:%d>\n", __FUNCTION__,__LINE__);

#endif // #if defined(CONFIG_DEBUG)
}

#define ____PROTECTED_API___________________________________________________________________________


#if defined(CONFIG_DEBUG)
HBOOL	APKI_METASCHED_Dump(DxSCHEDULE_t *sch)
{
	if(sch==NULL)
	{
		return FALSE;
	}

	HxLOG_Print("sch(0x%x)\r\n", sch);
	HxLOG_HexIntDump(sch->uid);
	HxLOG_HexIntDump(sch->schedId);
	HxLOG_BoolDump(sch->eventBase);
	HxLOG_HexIntDump(sch->eventId);
	HxLOG_DecIntDump(sch->startTime);
	HxLOG_DecIntDump(sch->duration);
	HxLOG_DecIntDump(sch->svcUId);
	HxLOG_DecIntDump(sch->preStart);
	HxLOG_DecIntDump(sch->groupIndex);
	HxLOG_DecIntDump(sch->recMode);
	HxLOG_BoolDump(sch->fromDap);
	HxLOG_BoolDump(sch->freeCaMode);
	HxLOG_DecIntDump(sch->Rating);
	HxLOG_DecIntDump(sch->epgType);
	HxLOG_DecIntDump(sch->action);
	HxLOG_DecIntDump(sch->repeat.cycle);
	HxLOG_DecIntDump(sch->repeat.date);
	HxLOG_StringDump(sch->name);
	HxLOG_StringDump(sch->description);
	HxLOG_StringDump(sch->longDescription);
	HxLOG_StringDump(sch->storageID);
	HxLOG_StringDump(sch->userID);
	HxLOG_StringDump(sch->deviceHandle);
	HxLOG_StringDump(sch->destinationId);
	HxLOG_StringDump(sch->dubbingId);



	return TRUE;
}
#endif

#endif
