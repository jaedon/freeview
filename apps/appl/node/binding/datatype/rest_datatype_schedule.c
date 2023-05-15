/*******************************************************************
	File Description
********************************************************************/
/**
	@file	 rest_datatype_schedule.c
	@brief

	Description:  									\n
	Module: node/binding/datatype_convert		\n

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
#include <dlib.h>
#include <apk.h>
#include <rest_bind_api.h>
#include <rest_datatype.h>

/*******************************************************************/
/********************      Local Definition         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/



/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
// Converting Types:
STATIC RestEnumStringTable_t	 s_stRestDataType_SchAction =
{
	(HCHAR*)DxNAMEOF(RestSchAction_e),

	{
		{	"live",			eREST_SCH_ACTION_Live		},
		{	"record",		eREST_SCH_ACTION_Record		},
		{	"satrec",		eREST_SCH_ACTION_SatRecord	},
		{	"poweron",		eREST_SCH_ACTION_PowerOn	},
		{	"poweroff",		eREST_SCH_ACTION_PowerOff	},
		{	"swupdate",		eREST_SCH_ACTION_SwUpdate	},
		{	"stream_via_live", 	eREST_SCH_ACTION_StreamingViaLive	},
		{	"stream_via_file", 	eREST_SCH_ACTION_StreamingViaFile	},
		{	NULL,			0							}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_SchType =
{
	(HCHAR*)DxNAMEOF(RestSchType_e),

	{
		{	"time",			eREST_SCH_TYPE_TimeBased	},
		{	"event",		eREST_SCH_TYPE_EventBased   },
		{	"series",		eREST_SCH_TYPE_SeriesBased	},
		{	"instant",		eREST_SCH_TYPE_Instant		},
		{	"delayed",		eREST_SCH_TYPE_Delayed		},
		{	NULL,			0							}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_SchRepeat =
{
	(HCHAR*)DxNAMEOF(DxRsvRepeat_e),

	{
		{	"once",			DxRSVREPEAT_ONCE			},
		{	"daily",		DxRSVREPEAT_DAILY			},
		{	"weekly",		DxRSVREPEAT_WEEKLY			},
		{	"weekdays",		DxRSVREPEAT_WEEKDAYS		},
		{	"weekend",		DxRSVREPEAT_WEEKEND			},
		{	"notsunday",	DxRSVREPEAT_WITHOUT_SUNDAY	},
		{	"keyword",		DxRSVREPEAT_KEYWORD			},
		{	NULL,			0							}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_SchState =
{
	(HCHAR*)DxNAMEOF(DxRsvStatus_e),

	{
		{	"waiting",		DxRSVSTATUS_WAITING		},
		{	"ready",		DxRSVSTATUS_READY		},
		{	"running",		DxRSVSTATUS_RUNNING		},
		{	"stopped",		DxRSVSTATUS_STOP		},
		{	"changing",		DxRSVSTATUS_CH_CHANGE	},
		{	NULL,			0						}
	}
};

#define ___LOCAL_FUNCTIONS___
STATIC HERROR rest_datatype_ConvertRsvType2SchActAndType (DxRsvType_e eRsvType, RestSchAction_e *peAction, RestSchType_e *peType)
{
	switch (eRsvType)
	{
	case DxRSVTYPE_WATCHING_EBR:
		*peAction = eREST_SCH_ACTION_Live;			*peType = eREST_SCH_TYPE_EventBased;
		break;
		
	case DxRSVTYPE_WATCHING_TBR:
		*peAction = eREST_SCH_ACTION_Live;			*peType = eREST_SCH_TYPE_TimeBased;
		break;

	case DxRSVTYPE_RECORD_EBR:
		*peAction = eREST_SCH_ACTION_Record;		*peType = eREST_SCH_TYPE_EventBased;
		break;

	case DxRSVTYPE_RECORD_TBR:
		*peAction = eREST_SCH_ACTION_Record;		*peType = eREST_SCH_TYPE_TimeBased;
		break;

	case DxRSVTYPE_POWER_ON:
		*peAction = eREST_SCH_ACTION_PowerOn;		*peType = eREST_SCH_TYPE_TimeBased;
		break;

	case DxRSVTYPE_POWER_OFF:
		*peAction = eREST_SCH_ACTION_PowerOff;		*peType = eREST_SCH_TYPE_TimeBased;
		break;

	case DxRSVTYPE_REGULAR_OTA:
		*peAction = eREST_SCH_ACTION_SwUpdate;		*peType = eREST_SCH_TYPE_TimeBased;
		break;

	case DxRSVTYPE_RECORD_INSTANT:
		*peAction = eREST_SCH_ACTION_Record;		*peType = eREST_SCH_TYPE_Instant;
		break;
		
	case DxRSVTYPE_RECORD_DELAYED:	
		*peAction = eREST_SCH_ACTION_Record;		*peType = eREST_SCH_TYPE_Delayed;
		break;

	case DxRSVTYPE_RECORD_SERIES_EBR:
		*peAction = eREST_SCH_ACTION_Record;		*peType = eREST_SCH_TYPE_SeriesBased;
		break;

	case DxRSVTYPE_RECORD_SATRECORD_TBR:
		*peAction = eREST_SCH_ACTION_SatRecord;		*peType = eREST_SCH_TYPE_TimeBased;
		break;

	case DxRSVTYPE_NETSTREAMING_LIVE:
		*peAction = eREST_SCH_ACTION_StreamingViaLive;	*peType = eREST_SCH_TYPE_Instant;
		break;
		
	case DxRSVTYPE_NETSTREAMING_FILE:
		*peAction = eREST_SCH_ACTION_StreamingViaFile;	*peType = eREST_SCH_TYPE_Instant;
		break;
		
	default:
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR rest_datatype_ConvertSchActAndType2RsvType (RestSchAction_e eAction, RestSchType_e eType, DxRsvType_e *peRsvType)
{
	switch (eAction)
	{
	case eREST_SCH_ACTION_Live:
		switch (eType)
		{
		case eREST_SCH_TYPE_TimeBased:		*peRsvType = DxRSVTYPE_WATCHING_TBR;		return ERR_OK;
		case eREST_SCH_TYPE_EventBased:		*peRsvType = DxRSVTYPE_WATCHING_EBR;		return ERR_OK;
		default:
			break;
		}

		break;

	case eREST_SCH_ACTION_Record:
		switch (eType)
		{
		case eREST_SCH_TYPE_TimeBased:		*peRsvType = DxRSVTYPE_RECORD_TBR;			return ERR_OK;
		case eREST_SCH_TYPE_EventBased:		*peRsvType = DxRSVTYPE_RECORD_EBR;			return ERR_OK;
		case eREST_SCH_TYPE_SeriesBased:	*peRsvType = DxRSVTYPE_RECORD_SERIES_EBR;	return ERR_OK;
		case eREST_SCH_TYPE_Instant:		*peRsvType = DxRSVTYPE_RECORD_INSTANT;		return ERR_OK;
		case eREST_SCH_TYPE_Delayed:		*peRsvType = DxRSVTYPE_RECORD_DELAYED;		return ERR_OK;
		default:
			break;
		}

		break;

	case eREST_SCH_ACTION_SatRecord:
		switch (eType)
		{
		case eREST_SCH_TYPE_TimeBased:		*peRsvType = DxRSVTYPE_RECORD_SATRECORD_TBR;	return ERR_OK;
		default:
			break;
		}

		break;

	case eREST_SCH_ACTION_PowerOn:
		switch (eType)
		{
		case eREST_SCH_TYPE_TimeBased:		*peRsvType = DxRSVTYPE_POWER_ON;			return ERR_OK;
		default:
			break;
		}

		break;

	case eREST_SCH_ACTION_PowerOff:
		switch (eType)
		{
		case eREST_SCH_TYPE_TimeBased:		*peRsvType = DxRSVTYPE_POWER_OFF;			return ERR_OK;
		default:
			break;
		}

		break;
		
	case eREST_SCH_ACTION_SwUpdate:
		switch (eType)
		{
		case eREST_SCH_TYPE_TimeBased:		*peRsvType = DxRSVTYPE_REGULAR_OTA;			return ERR_OK;
		default:
			break;
		}
	
		break;

	case eREST_SCH_ACTION_StreamingViaLive:
		switch (eType)
		{
		case eREST_SCH_TYPE_Instant:		*peRsvType = DxRSVTYPE_NETSTREAMING_LIVE; 		return ERR_OK;
		default:
			break;
		}
	
		break;

	case eREST_SCH_ACTION_StreamingViaFile:
		switch (eType)
		{
		case eREST_SCH_TYPE_Instant:		*peRsvType = DxRSVTYPE_NETSTREAMING_FILE;		return ERR_OK;
		default:
			break;
		}
	
		break;

	default:
		break;
	}

	return ERR_FAIL;
}

#define ___MEMBER_FUNCTIONS___
#define _____SCHEDULE_____
HINT32 rest_datatype_JsonizeSchedule (HINT32 nJsonWriter, DxSchedule_t *pstSchedule, HCHAR *szSelect)
{
	HINT32				 nWriter;
	HINT32				 nEvtId = 0;
	HUINT32				 ulSeriesId = 0, ulProgramId = 0;
	RestSchAction_e		 eSchAction;
	RestSchType_e		 eSchType;
	UNIXTIME			 utStartTime;
	HERROR				 hErr;

	if (NULL == pstSchedule)
	{
		HxLOG_Error("pstSchedule NULL\n");
		return nJsonWriter;
	}

	hErr = rest_datatype_ConvertRsvType2SchActAndType(pstSchedule->eRsvType, &eSchAction, &eSchType);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("rest_datatype_ConvertRsvType2SchActAndType failed: eRsvType(%d)\n", pstSchedule->eRsvType);
		return nJsonWriter;
	}

	(void)HLIB_DATETIME_ConvertDateTimeToUnixTime(&(pstSchedule->stStartTime), &utStartTime);

	nWriter = (0 == nJsonWriter) ? HLIB_JSONWRITER_Open() : nJsonWriter;
	if (0 == nWriter)
	{
		HxLOG_Error(">> HLIB_JSONWRITER_Open failed:\n");
		return 0;
	}

	HxJSONWRITER_ObjectBegin(nWriter);
	{
		(void)rest_datatype_SetJsonInteger(nWriter, "scheduleUid", pstSchedule->ulSlot, szSelect);
		(void)rest_datatype_SetJsonEnum(nWriter, "scheduleAction", DxNAMEOF(RestSchAction_e), (HINT32)eSchAction, szSelect);
		(void)rest_datatype_SetJsonEnum(nWriter, "scheduleType", DxNAMEOF(RestSchType_e), (HINT32)eSchType, szSelect);
		(void)rest_datatype_SetJsonEnum(nWriter, "repeatType", DxNAMEOF(DxRsvRepeat_e), (HINT32)pstSchedule->eRepeat, szSelect);
		(void)rest_datatype_SetJsonEnum(nWriter, "state", DxNAMEOF(DxRsvStatus_e), (HINT32)pstSchedule->eStatus, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "sourceChannelUid", pstSchedule->ulSvcUid, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "startTime", utStartTime, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "duration", pstSchedule->ulDuration, szSelect);

		switch (eSchAction)
		{
		case eREST_SCH_ACTION_Live:
			nEvtId = pstSchedule->uExtInfo.stWch.nEvtId;
			ulSeriesId = pstSchedule->uExtInfo.stWch.ulSeriesId;
			ulProgramId = pstSchedule->uExtInfo.stWch.ulProgramId;
			break;

		case eREST_SCH_ACTION_Record:
			nEvtId = pstSchedule->uExtInfo.stRec.nEvtId;
			ulSeriesId = pstSchedule->uExtInfo.stRec.ulSeriesId;
			ulProgramId = pstSchedule->uExtInfo.stRec.ulProgramId;

			(void)rest_datatype_SetJsonInteger(nWriter, "paddingHead", pstSchedule->uExtInfo.stRec.ulPreOffsetTime, szSelect);
			(void)rest_datatype_SetJsonInteger(nWriter, "paddingTail", pstSchedule->uExtInfo.stRec.ulPostOffsetTime, szSelect);

			break;

		case eREST_SCH_ACTION_PowerOn:
			(void)rest_datatype_SetJsonInteger(nWriter, "soundVolume", pstSchedule->uExtInfo.stPwr.ucVolume, szSelect);
			(void)rest_datatype_SetJsonInteger(nWriter, "channelNumber", pstSchedule->uExtInfo.stPwr.usChNum, szSelect);
			break;

		case eREST_SCH_ACTION_StreamingViaLive:
		case eREST_SCH_ACTION_StreamingViaFile:
			break;

		default:
			break;			
		}

		switch (eSchType)
		{
		case eREST_SCH_TYPE_EventBased:
			(void)rest_datatype_SetJsonInteger(nWriter, "evtId", nEvtId, szSelect);
			break;
		
		case eREST_SCH_TYPE_SeriesBased:
			(void)rest_datatype_SetJsonInteger(nWriter, "seriesId", ulSeriesId, szSelect);
			(void)rest_datatype_SetJsonInteger(nWriter, "programId", ulProgramId, szSelect);
			break;

		default:
			break;
		}
	}
	HxJSONWRITER_ObjectEnd(nWriter);

	return nWriter;	
}

HERROR rest_datatype_DejsonizeSchedule (HxJSON_t jsJson, DxSchedule_t *pstSchedule, HBOOL bClearObj)
{
	HBOOL				 bEvtId = FALSE, bSeriesId = FALSE, bProgramId = FALSE;
	HINT32				 nEvtId = 0;
	HINT32				 nSndVolume = 0, nChNum = 0;
	HUINT32				 ulSeriesId = 0, ulProgramId = 0;
	RestSchAction_e		 eSchAction = eREST_SCH_ACTION_None;
	RestSchType_e		 eSchType = eREST_SCH_TYPE_None;
	DxRsvType_e			 eRsvType = DxRSVTYPE_NOTHING;
	UNIXTIME			 utStartTime;
	HERROR				 hErr;

	if (NULL == pstSchedule)
	{
		HxLOG_Error("pstEvent NULL\n");
		return ERR_FAIL;
	}

	if (NULL == jsJson)
	{
		HxLOG_Error("jsJson NULL\n");
		return ERR_FAIL;
	}

	if (TRUE == bClearObj)
	{
		HxSTD_MemSet(pstSchedule, 0, sizeof(DxSchedule_t));
	}

	(void)rest_datatype_GetJsonEnum(jsJson, "scheduleAction", DxNAMEOF(RestSchAction_e), (HINT32 *)&eSchAction);
	(void)rest_datatype_GetJsonEnum(jsJson, "scheduleType", DxNAMEOF(RestSchType_e), (HINT32 *)&eSchType);

	hErr = rest_datatype_ConvertSchActAndType2RsvType(eSchAction, eSchType, &eRsvType);
	if ((ERR_OK == hErr) && (DxRSVTYPE_NOTHING == pstSchedule->eRsvType))
	{
		pstSchedule->eRsvType = eRsvType;
	}
	else if ((ERR_OK != hErr) && (DxRSVTYPE_NOTHING == pstSchedule->eRsvType))
	{
		HxLOG_Error("New Schedule shall have the proper the action(%d) and type(%d)\n", eSchAction, eSchType);
		return ERR_FAIL;
	}

	// 어떤 경우에도 scheduleUid 를 덮어 치는 경우는 있을 수 없다.
	(void)rest_datatype_GetJsonEnum(jsJson, "repeatType", DxNAMEOF(DxRsvRepeat_e), (HINT32 *)&(pstSchedule->eRepeat));
	(void)rest_datatype_GetJsonEnum(jsJson, "state", DxNAMEOF(DxRsvStatus_e), (HINT32 *)&(pstSchedule->eStatus));
	(void)rest_datatype_GetJsonInteger(jsJson, "sourceChannelUid", (HINT32 *)&(pstSchedule->ulSvcUid));

	switch (eSchType)
	{
	case eREST_SCH_TYPE_TimeBased:
		if (rest_datatype_GetJsonInteger(jsJson, "startTime", (HINT32 *)&utStartTime) == ERR_OK)
		{
			(void)HLIB_DATETIME_ConvertUnixTimeToDateTime(utStartTime, &(pstSchedule->stStartTime));
		}
		(void)rest_datatype_GetJsonInteger(jsJson, "duration", (HINT32 *)&(pstSchedule->ulDuration));
		break;
	
	case eREST_SCH_TYPE_EventBased:
		bEvtId = (rest_datatype_GetJsonInteger(jsJson, "evtId", (HINT32 *)&nEvtId) == ERR_OK);
		break;
	
	case eREST_SCH_TYPE_SeriesBased:
		bSeriesId = (rest_datatype_GetJsonInteger(jsJson, "seriesId", (HINT32 *)&ulSeriesId) == ERR_OK);
		bProgramId = (rest_datatype_GetJsonInteger(jsJson, "programId", (HINT32 *)&ulProgramId) == ERR_OK);
		break;
	
	default:
		break;
	}

	switch (eSchAction)
	{
	case eREST_SCH_ACTION_Live:
		if (bEvtId)				{ pstSchedule->uExtInfo.stWch.nEvtId = nEvtId; }
		if (bSeriesId)			{ pstSchedule->uExtInfo.stWch.ulSeriesId = ulSeriesId; }
		if (bProgramId)			{ pstSchedule->uExtInfo.stWch.ulProgramId = ulProgramId; }
		break;
	
	case eREST_SCH_ACTION_Record:
		if (bEvtId)				{ pstSchedule->uExtInfo.stRec.nEvtId = nEvtId; }
		if (bSeriesId)			{ pstSchedule->uExtInfo.stRec.ulSeriesId = ulSeriesId; }
		if (bProgramId)			{ pstSchedule->uExtInfo.stRec.ulProgramId = ulProgramId; }
		(void)rest_datatype_GetJsonInteger(jsJson, "paddingHead", (HINT32 *)&(pstSchedule->uExtInfo.stRec.ulPreOffsetTime));
		(void)rest_datatype_GetJsonInteger(jsJson, "paddingTail", (HINT32 *)&(pstSchedule->uExtInfo.stRec.ulPostOffsetTime));
		break;

	case eREST_SCH_ACTION_PowerOn:
		if (rest_datatype_GetJsonInteger(jsJson, "soundVolume", &nSndVolume) == ERR_OK)
		{
			pstSchedule->uExtInfo.stPwr.ucVolume = (HUINT8)nSndVolume;
		}

		if (rest_datatype_GetJsonInteger(jsJson, "channelNumber", &nChNum) == ERR_OK)
		{
			pstSchedule->uExtInfo.stPwr.usChNum = (HUINT16)nChNum;
		}
		
		break;
	
	case eREST_SCH_ACTION_StreamingViaLive:
	case eREST_SCH_ACTION_StreamingViaFile:
		break;
	
	default:
		break;			
	}

	return ERR_OK;	
}

#define _____INITIALIZATION_____
HERROR rest_datatype_InitSchedule (void)
{
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_SchAction);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_SchType);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_SchRepeat);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_SchState);

	return ERR_OK;
}


#define ___GLOBAL_FUNCTIONS___

