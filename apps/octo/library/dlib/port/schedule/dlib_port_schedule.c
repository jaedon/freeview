#include <hlib.h>
#include "_dlib_port_schedule.h"

//#define WRITE_CAMID_TO_JSON

typedef	struct
{
	DxField_t		field;
	HBOOL			(* getData) (DxField_t *field, const void *src, HUINT32 size);
} DxSchedule_Field_t;

static	DxEnumStringTable_t	_stRsvTypeTable =
{
	(HCHAR*)DxNAMEOF(DxRsvType_e),
	{
		{	"nothing",				DxRSVTYPE_NOTHING	},
		{	"watch-ebr",			DxRSVTYPE_WATCHING_EBR	},
		{	"watch-tbr",			DxRSVTYPE_WATCHING_TBR	},
		{	"rec-ebr", 				DxRSVTYPE_RECORD_EBR	},
		{	"rec-tbr", 				DxRSVTYPE_RECORD_TBR	},
		{	"poweron",				DxRSVTYPE_POWER_ON	},
		{	"poweroff",				DxRSVTYPE_POWER_OFF },
		{	"regular-ota",			DxRSVTYPE_REGULAR_OTA	},
		{	"epg-gathering",		DxRSVTYPE_EPG_GATHERING },
		{	"emm",					DxRSVTYPE_EMM	},
		{	"update-task",			DxRSVTYPE_UPDATE_TASK	},
		{	"aso-notify", 			DxRSVTYPE_ASO_NOTIFY	},
		{	"myepg-update",			DxRSVTYPE_MYEPG_UPDATE	},
		{	"net-update", 			DxRSVTYPE_NET_UPDATE	},
		{	"br-tbr", 				DxRSVTYPE_BLURAY_TBR	},
		{	"br-ebr", 				DxRSVTYPE_BLURAY_EBR	},
		{	"rec-instant",			DxRSVTYPE_RECORD_INSTANT	},
		{	"rec-delayed",			DxRSVTYPE_RECORD_DELAYED	},
		{	"rec-keyword-notsure",	DxRSVTYPE_KEYWORD_RECORD_NOTSURE	},
		{	"cas-power-ctrl", 		DxRSVTYPE_CAS_POWER_CTRL	},
		{	"logo-down",			DxRSVTYPE_LOGO_DOWN },
		{	"rescan", 				DxRSVTYPE_RESCAN	},
		{	"cas-tel",				DxRSVTYPE_CAS_TEL	},
		{	"channel-mask",			DxRSVTYPE_CHANNEL_MASK	},
		{	"dubbing",				DxRSVTYPE_DUBBING	},
		{	"ota-bd", 				DxRSVTYPE_OTA_BD	},
		{	"ipepg-gathering",		DxRSVTYPE_IP_EPG_GATHERING	},
		{	"ipepg_rp",				DxRSVTYPE_IP_REMOTE_PROGRAMMING },
		{	"dlna_ebr",				DxRSVTYPE_DLNA_EBR },
		{	"dlna_tbr",				DxRSVTYPE_DLNA_TBR },
		{	"dlna_dubbing",			DxRSVTYPE_DLNA_DUBBING },
		{	"netstreaming_live",	DxRSVTYPE_NETSTREAMING_LIVE },
		{	"netstreaming_file",	DxRSVTYPE_NETSTREAMING_FILE },
		{	"rec-series_ebr",				DxRSVTYPE_RECORD_SERIES_EBR	},
		{	"rec-satrecord_tbr",				DxRSVTYPE_RECORD_SATRECORD_TBR},
		{	"activationtime",				DxRSVTYPE_ACTIVATION_TIME},
		{	"desc_rec_standby", 	DxRSVTYPE_DESC_REC_STANDBY },
		{	"ciplusoptune",			DxRSVTYPE_CIPLUS_OP_TUNE},
		{	"tvtvepgtimer",			DxRSVTYPE_TVTVEPG_TIMER},
		{	"satrecordchecktimer",			DxRSVTYPE_CHECK_SATRECORD_TIMER},
		{	"watch-series_ebr",			DxRSVTYPE_WATCHING_SERIES_EBR},
		{	"endofcase",			DxRSVTYPE_EndOfCase },
		{	NULL,								0}
	}
};

static	DxEnumStringTable_t	_stRsvReadyTable =
{
	(HCHAR*)DxNAMEOF(DxRsvReady_e),
	{
		{	"0sec",		DxRSVREADY_0_SEC	},
		{	"10sec",	DxRSVREADY_10_SEC	},
		{	"20sec",	DxRSVREADY_20_SEC	},
		{	"30sec", 	DxRSVREADY_30_SEC	},
		{	"40sec",	DxRSVREADY_40_SEC	},
		{	"60sec", 	DxRSVREADY_60_SEC	},
		{	"180sec",	DxRSVREADY_180_SEC	},
		{	"300sec",	DxRSVREADY_300_SEC	},
		{	"tracking",	DxRSVREADY_TVA_TRACKING },
		{	NULL,								0}
	}
};

static	DxEnumStringTable_t	_stRsvRepeatTable =
{
	(HCHAR*)DxNAMEOF(DxRsvRepeat_e),
	{
		{	"once",		DxRSVREPEAT_ONCE	},
		{	"daily",	DxRSVREPEAT_DAILY	},
		{	"weekly",	DxRSVREPEAT_WEEKLY	},
		{	"weekdays", DxRSVREPEAT_WEEKDAYS	},
		{	"weekend", 	DxRSVREPEAT_WEEKEND	},
		{	"wosunday",	DxRSVREPEAT_WITHOUT_SUNDAY	},
		{	"keyword",	DxRSVREPEAT_KEYWORD	},
		{	"fewweek",	DxRSVREPEAT_FEWDAY_OF_WEEK	},
		{	NULL,								0}
	}
};

static	DxEnumStringTable_t	_stRsvStatusTable =
{
	(HCHAR*)DxNAMEOF(DxRsvStatus_e),
	{
		{	"waiting",		DxRSVSTATUS_WAITING	},
		{	"ready",		DxRSVSTATUS_READY	},
		{	"ch-change",	DxRSVSTATUS_CH_CHANGE	},
		{	"running",		DxRSVSTATUS_RUNNING	},
		{	"stop", 		DxRSVSTATUS_STOP	},
		{	NULL,								0}
	}
};


static	DxEnumStringTable_t	_stEpgType =
{
	(HCHAR*)DxNAMEOF(DxEPG_Type_e),
	{
		{	"none",		eDxEPG_TYPE_NONE	},
		{	"pf",		eDxEPG_TYPE_PF	},
		{	"sc",	eDxEPG_TYPE_SC	},
		{	"pf_ex",		eDxEPG_TYPE_PF_EX	},
		{	"sc_ex", 		eDxEPG_TYPE_SC_EX	},
		{	"rf", 		eDxEPG_TYPE_RF	},
		{	"tvtv", 		eDxEPG_TYPE_TVTV	},
		{	"ip", 		eDxEPG_TYPE_IP	},
		{	"all", 		eDxEPG_TYPE_ALL	},
		{	NULL,								0}
	}
};

static	DxEnumStringTable_t	_stScheduleSpec =
{
	(HCHAR*)DxNAMEOF(DxSchedule_Spec_e),
	{
		{	"none",		eDxSchedule_Spec_None		},
		{	"base",		eDxSchedule_Spec_Base		},
		{	"freesat",	eDxSchedule_Spec_Freesat	},
		{	"tvAnyTime",eDxSchedule_Spec_TvAnyTime  },
		{	"endofcase",eDxSchedule_Spec_EndOfCase	},
		{	NULL,		0							}
	}
};



static	DxEnumStringTable_t	_stRemovedReason =
{
	(HCHAR*)DxNAMEOF(DxSchedule_RemovedReason_e),
	{
		{	"none",			DxSchedule_RemovedReason_None	},
		{	"servicedeleted",			DxSchedule_RemovedReason_ServiceDeleted	},
		{	"canceled",			DxSchedule_RemovedReason_Canceled	},
		{	"completed",			DxSchedule_RemovedReason_Completed	},
		{	"stopped",		DxSchedule_RemovedReason_Stopped	},
		{	"expired",		DxSchedule_RemovedReason_Expired	},
		{	"nopvrdevice",		DxSchedule_RemovedReason_NoPvrDevice},
		{	"cascopyrightcontrolfail",		DxSchedule_RemovedReason_CasCopyrightControlFail},
		{	"seriestracking",		DxSchedule_RemovedReason_SeriesTracking},
		{	"pendingbroken",		DxSchedule_RemovedReason_PendingBroken},
		{	"unabletotrack",		DxSchedule_RemovedReason_UnableToTrack},
		{	"hddfull",		DxSchedule_RemovedReason_HddFull},
		{	"unknown",		DxSchedule_RemovedReason_Unknown},
		{	"endofcase",		DxSchedule_RemovedReason_EndofCase	},
		{	NULL,								0}
	}
};

static HERROR	dlib_schedule_Encoder(const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	HINT32			jw;
	HCHAR			szDateTime[128];
	DxSchedule_t	*pstInfo = (DxSchedule_t*)src;

	HxLOG_Assert(sizeof(DxSchedule_t) == srcSize);

	jw = HLIB_JSONWRITER_Open();
	HxJSONWRITER_ObjectBegin(jw);
		HxJSONWRITER_Integer(jw, "slot", pstInfo->ulSlot);
		HxJSONWRITER_String (jw, "rsvtype", dlib_ConvertEnum2Str(DxNAMEOF(DxRsvType_e), pstInfo->eRsvType));
		HxJSONWRITER_Integer(jw, "svchandle", (pstInfo->ulSvcUid & 0xFFFF)); // 이전모델 (hms ph1) 과의 호환성. 이전 모델에서는 svc handle 을 저장하고 있었다.

		HxJSONWRITER_Object(jw, "tripleid");
			HxJSONWRITER_Integer(jw, "onid", pstInfo->stTripleId.usOnId);
			HxJSONWRITER_Integer(jw, "tsid", pstInfo->stTripleId.usTsId);
			HxJSONWRITER_Integer(jw, "svcid", pstInfo->stTripleId.usSvcId);
		HxJSONWRITER_ObjectEnd(jw);

		HxJSONWRITER_String (jw, "rsvready", dlib_ConvertEnum2Str(DxNAMEOF(DxRsvReady_e), pstInfo->eRsvReady));
		HLIB_DATETIME_GetStringByDateTime(&pstInfo->stStartTime, szDateTime, 128);
		HxJSONWRITER_String (jw, "starttime", szDateTime);
		HxJSONWRITER_Integer(jw, "duration", pstInfo->ulDuration);
		HxJSONWRITER_String (jw, "repeat", dlib_ConvertEnum2Str(DxNAMEOF(DxRsvRepeat_e), pstInfo->eRepeat));
		HxJSONWRITER_Integer(jw, "repeatdays", pstInfo->ulRepeatDays);
		HxJSONWRITER_String (jw, "status", dlib_ConvertEnum2Str(DxNAMEOF(DxRsvStatus_e), pstInfo->eStatus));
		HxJSONWRITER_Boolean(jw, "exceptconflict", pstInfo->bExceptConflict);

		HxJSONWRITER_String (jw, "epgtype", dlib_ConvertEnum2Str(DxNAMEOF(DxEPG_Type_e), pstInfo->eEpgType));

		HxJSONWRITER_Boolean(jw, "removed", pstInfo->bRemoved);
		HxJSONWRITER_String (jw, "removedreason", dlib_ConvertEnum2Str(DxNAMEOF(DxSchedule_RemovedReason_e), pstInfo->stRemovedInfo.eRemovedReason));
		HxJSONWRITER_Integer (jw, "removedreasoninfo", pstInfo->stRemovedInfo.eDetailInfo);

		HxJSONWRITER_String (jw, "schedulespec", dlib_ConvertEnum2Str(DxNAMEOF(DxSchedule_Spec_e), pstInfo->eScheduleSpec));
		HxJSONWRITER_Binary (jw, "extinfo", &pstInfo->uExtInfo, sizeof(pstInfo->uExtInfo));

#if defined(CONFIG_MW_CI_PLUS_V_1_3)
#if defined(WRITE_CAMID_TO_JSON)
		HxJSONWRITER_String	(jw, "camid", pstInfo->aucCamId);
#endif
#endif

#if 1
		HxJSONWRITER_Binary(jw, "ex", &pstInfo->ex, sizeof(pstInfo->ex));
#else
		HxJSONWRITER_Object (jw, "freesat");
		HxJSONWRITER_Integer(jw, "mastersvc", pstInfo->ex.freesat.ulMasterSvc);
		HxJSONWRITER_ObjectEnd(jw);
#endif

	HxJSONWRITER_ObjectEnd(jw);

	if (HLIB_JSONWRITER_Error(jw))
	{
		HxLOG_Error("JSON Error : (%s)\n", HLIB_JSONWRITER_Error(jw));

		*dstSize = 0;
		*dst = NULL;
	} else
	{
		*dstSize = HLIB_JSONWRITER_GetLength(jw);
		*dst     = DLIB_StrDup(HLIB_JSONWRITER_GetJSON(jw));
		HLIB_JSONWRITER_Close(jw);
	}

	if (*dstSize > 0)
		return ERR_OK;

	return ERR_FAIL;
}

static HERROR	dlib_schedule_Decoder(const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	HxJSON_t		root;
	HCHAR			*pszBinary;
	DxSchedule_t	*pstInfo;

	HxLOG_Assert(srcSize && src);

	root = HLIB_JSON_Decode(src);
	HxLOG_Assert(root);

	pstInfo = (DxSchedule_t*)DLIB_Calloc(sizeof(DxSchedule_t));
	pstInfo->ulSlot = HLIB_JSON_Object_GetInteger(root, "slot");
	pstInfo->eRsvType = dlib_ConvertStr2Enum(DxNAMEOF(DxRsvType_e), HLIB_JSON_Object_GetString(root, "rsvtype"));
	pstInfo->ulSvcUid = HLIB_JSON_Object_GetInteger(root, "svchandle");
	pstInfo->ulSvcUid = (pstInfo->ulSvcUid & 0xFFFF);// 이전모델 (hms ph1) 과의 호환성. 이전 모델에서는 svc handle 을 저장하고 있었다.
	{
		HxJSON_t	tripleid;
		tripleid = HxJSON_Object_Get(root, "tripleid");
		if(NULL != tripleid)
		{
			pstInfo->stTripleId.usOnId = HLIB_JSON_Object_GetInteger(tripleid, "onid");
			pstInfo->stTripleId.usTsId = HLIB_JSON_Object_GetInteger(tripleid, "tsid");
			pstInfo->stTripleId.usSvcId = HLIB_JSON_Object_GetInteger(tripleid, "svcid");
		}
	}

	pstInfo->eRsvReady = dlib_ConvertStr2Enum(DxNAMEOF(DxRsvReady_e), HLIB_JSON_Object_GetString(root, "rsvready"));
	HLIB_DATETIME_GetDateTimeByString(HLIB_JSON_Object_GetString(root, "starttime"), &pstInfo->stStartTime);
	pstInfo->ulDuration = HLIB_JSON_Object_GetInteger(root, "duration");
	pstInfo->eRepeat = dlib_ConvertStr2Enum(DxNAMEOF(DxRsvRepeat_e), HLIB_JSON_Object_GetString(root, "repeat"));
	pstInfo->ulRepeatDays = HLIB_JSON_Object_GetInteger(root, "repeatdays");
	pstInfo->eStatus = dlib_ConvertStr2Enum(DxNAMEOF(DxRsvStatus_e), HLIB_JSON_Object_GetString(root, "status"));
	pstInfo->bExceptConflict = HLIB_JSON_Object_GetBoolean(root, "exceptconflict");

#if defined(CONFIG_MW_CI_PLUS_V_1_3)
#if defined(WRITE_CAMID_TO_JSON)
	if (HLIB_JSON_Object_GetString(root, "camid") != NULL)
	{
		HxSTD_MemCopy(pstInfo->aucCamId, HLIB_JSON_Object_GetString(root, "camid"), 8);
	}
#endif
#endif

	// 나중에 추가된 항목 이므로 없을 수 있는 argument들은 여기에 넣는다.
	pstInfo->bRemoved = HLIB_JSON_Object_GetBoolean(root, "removed");
	{
		HCHAR			*szRemovedReason = (HCHAR *)HLIB_JSON_Object_GetString(root, "removedreason");
		HUINT32			eRemovedReasonInfo = HLIB_JSON_Object_GetInteger(root, "removedreasoninfo");

		if(NULL != szRemovedReason)
		{
			pstInfo->stRemovedInfo.eRemovedReason= dlib_ConvertStr2Enum(DxNAMEOF(DxSchedule_RemovedReason_e), szRemovedReason);
			pstInfo->stRemovedInfo.eDetailInfo= eRemovedReasonInfo;
		}

	}

	// 나중에 추가된 항목 이므로 없을 수 있는 argument들은 여기에 넣는다.
	{
		HCHAR			*szScheduleSpec = (HCHAR *)HLIB_JSON_Object_GetString(root, "schedulespec");

		if(NULL != szScheduleSpec)
		{
			pstInfo->eScheduleSpec = dlib_ConvertStr2Enum(DxNAMEOF(DxSchedule_Spec_e), szScheduleSpec);
		}
	}

	//	Get Binary with "uExtInfo"
	pszBinary = (HCHAR*)HLIB_JSON_Object_GetString(root, "extinfo");
	HLIB_MATH_DecodeBase64((void*)&pstInfo->uExtInfo, pszBinary, HxSTD_StrLen(pszBinary));

	// 나중에 추가된 항목 이므로 없을 수 있는 argument들은 여기에 넣는다.
	{
		HCHAR			*szEpgType = (HCHAR *)HLIB_JSON_Object_GetString(root, "epgtype");

		if (NULL != szEpgType)
		{
			pstInfo->eEpgType = dlib_ConvertStr2Enum(DxNAMEOF(DxEPG_Type_e), szEpgType);
		}
	}

	// 기존에 잘못 들어간 ex.Hms를 pstInst->eEpgType에 넣는 것이다.
	{
		HxJSON_t	Hms;

		Hms = HxJSON_Object_Get(root, "Hms");
		if (NULL != Hms)
		{
			pstInfo->eEpgType = HLIB_JSON_Object_GetInteger(Hms, "epgtype");
		}
	}
	//

	// 기존 ex.freesat 으로 들어간 것은..
	{
		HxJSON_t	freesat;

		freesat = HxJSON_Object_Get(root, "freesat");
		if(NULL != freesat)
		{
			pstInfo->ex.freesat.ulMasterSvc = HLIB_JSON_Object_GetInteger(freesat, "mastersvc");
		}
	}

	{
		HxJSON_t	ex;
		HCHAR			*pszExBinary;

		ex = HxJSON_Object_Get(root, "ex");
		if(NULL != ex)
		{
			//	Get Binary with "uExtInfo"
			pszExBinary = (HCHAR*)HLIB_JSON_Object_GetString(root, "ex");
			HLIB_MATH_DecodeBase64((void*)&pstInfo->ex, pszExBinary, HxSTD_StrLen(pszExBinary));
		}
	}


	HLIB_JSON_Delete(root);

	*dst = (void*)pstInfo;
	*dstSize = sizeof(DxSchedule_t);

	return ERR_OK;}

static HBOOL	dlib_schedule_GetRsvType(DxField_t *field, const void *src, HUINT32 size)
{
	const DxSchedule_t	*schedule = (DxSchedule_t*)src;

	HxLOG_Assert(src && size);
	field->u.string = DLIB_StrDup(dlib_ConvertEnum2Str(DxNAMEOF(DxRsvType_e), schedule->eRsvType));

	return TRUE;
}

static HBOOL	dlib_schedule_GetStartTime(DxField_t *field, const void *src, HUINT32 size)
{
	UNIXTIME	unixTime;
	const DxSchedule_t	*schedule = (DxSchedule_t*)src;

	HxLOG_Assert(src && size);
	HLIB_DATETIME_ConvertDateTimeToUnixTime((HxDATETIME_t*)&schedule->stStartTime, &unixTime);
	field->u.integer = unixTime;

	return TRUE;
}

static HBOOL	dlib_schedule_GetDuration(DxField_t *field, const void *src, HUINT32 size)
{
	const DxSchedule_t	*schedule = (DxSchedule_t*)src;

	HxLOG_Assert(src && size);
	field->u.integer = schedule->ulDuration;

	return TRUE;
}

static HBOOL	dlib_schedule_GetRepeat(DxField_t *field, const void *src, HUINT32 size)
{
	const DxSchedule_t	*schedule = (DxSchedule_t*)src;

	HxLOG_Assert(src && size);
	field->u.integer = schedule->eRepeat;

	return TRUE;
}

static HBOOL	dlib_schedule_GetStatus(DxField_t *field, const void *src, HUINT32 size)
{
	const DxSchedule_t	*schedule = (DxSchedule_t*)src;

	HxLOG_Assert(src && size);
	field->u.integer = schedule->eStatus;

	return TRUE;
}

static const DxSchedule_Field_t	s_Schedule_FIELDS[] =
{
	  {{eDxTYPE_STRING , "rsvtype"    , FALSE, {0}}, dlib_schedule_GetRsvType}
	, {{eDxTYPE_INTEGER, "starttime"  , FALSE, {0}}, dlib_schedule_GetStartTime}
	, {{eDxTYPE_INTEGER, "duration"   , FALSE, {0}}, dlib_schedule_GetDuration}
	, {{eDxTYPE_INTEGER, "repeat"     , FALSE, {0}}, dlib_schedule_GetRepeat}
	, {{eDxTYPE_INTEGER, "status"     , FALSE, {0}}, dlib_schedule_GetStatus}
};

void			DLIB_SCHEDULE_Init(void)
{
	dlib_RegisterEnumTable(&_stRsvTypeTable);
	dlib_RegisterEnumTable(&_stRsvReadyTable);
	dlib_RegisterEnumTable(&_stRsvRepeatTable);
	dlib_RegisterEnumTable(&_stRsvStatusTable);
	dlib_RegisterEnumTable(&_stEpgType);
	dlib_RegisterEnumTable(&_stScheduleSpec);
	dlib_RegisterEnumTable(&_stRemovedReason);


	dlib_RegisterDECModule(DxNAMEOF(DxSchedule_t), dlib_schedule_Encoder, dlib_schedule_Decoder);
}

HINT32			DLIB_SCHEDULE_GetFieldCount (void)
{
	return sizeof(s_Schedule_FIELDS) / sizeof(s_Schedule_FIELDS[0]);
}

DxField_t *		DLIB_SCHEDULE_GetField (HINT32 index, const void *src, HUINT32 srcSize)
{
	DxField_t field;

	if (DLIB_SCHEDULE_GetFieldCount() <= index)
	{
		HxLOG_Warning("%s(index) index(index) out of range~!\n", __FUNCTION__, index);
		return NULL;
	}
	memcpy(&field, &(s_Schedule_FIELDS[index].field), sizeof(DxField_t));

	if (src && srcSize && s_Schedule_FIELDS[index].getData)
	{
		if (!s_Schedule_FIELDS[index].getData(&field, src, srcSize))
		{
			HxLOG_Warning("%s(%s) Cannot get data\n", __FUNCTION__, s_Schedule_FIELDS[index].field.name);
			return NULL;
		}
	}

	// 메모리 함수는 HxSTD_MemXXX를 사용한다.
	return (DxField_t *)DLIB_MemDup(&field, sizeof(DxField_t));
}

HERROR			DLIB_SCHEDULE_Encode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	(void)(uid);

	return dlib_schedule_Encoder(src, srcSize, dst, dstSize);
}

HERROR			DLIB_SCHEDULE_Decode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	(void)(uid);

	return dlib_schedule_Decoder(src, srcSize, dst, dstSize);
}

HERROR			DLIB_SCHEDULE_Compress (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	// 데이터 전송 시, 효율을 높이기 위해 압축이 필요한 경우 이 함수 호출됨
	// 압축 방식은 진짜 압축이 아닌, 그냥 string을 binary로 바꾸는 등
	// 아무튼 데이터를 줄이기만 하면 됨
	// dst는 메모리 할당을 해서 전달해야 함 (HxSTD_MemXXX를 사용)

	return ERR_FAIL;
}

HERROR			DLIB_SCHEDULE_Extract (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	// 압축된 데이터 원복
	// dst는 메모리 할당을 해서 전달해야 함 (HxSTD_MemXXX를 사용)

	return ERR_FAIL;
}


HERROR			DLIB_SCHEDULE_MakeType (HUID uid, const void *src, HUINT32 srcSize, void **type)
{
	return ERR_FAIL;
}

void			DLIB_SCHEDULE_FreeType (void *type)
{
}



