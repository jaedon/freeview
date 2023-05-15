/*******************************************************************
	File Description
********************************************************************/
/**
	@file	 rest_datatype_record.c
	@brief

	Description:  									\n
	Module: node/binding/datatype					\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
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
STATIC RestEnumStringTable_t	 s_stRestDataType_RecState =
{
	(HCHAR*)DxNAMEOF(DxRecStatus_e),

	{
		{	"starting",			eDxREC_STS_RECORD_START		},
		{	"recording",		eDxREC_STS_RECORDING		},
		{	"completed",		eDxREC_STS_RECORDED			},
		{	"incompleted",		eDxREC_STS_INCOMPLETED		},
		{	NULL,				0							}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_RecFailReason =
{
	(HCHAR*)DxNAMEOF(DxRecEndReasonType_e),

	{
		{	"not_failed",			eDxREC_FAILED_NOFAIL			},
		{	"hdd_full",				eDxREC_FAILED_HDDFULL			},
		{	"action_full",			eDxREC_FAILED_MAXNUM			},
		{	"record_full",			eDxREC_FAILED_MAXRECORDLISTNUM	},
		{	"less_than_30sec",		eDxREC_FAILED_LESS30SEC			},
		{	"no_signal",			eDxREC_FAILED_NOSIGNAL			},
		{	"no_channel",			eDxREC_FAILED_NONSERVICE		},
		{	"channel_scanning",		eDxREC_FAILED_SEARCHING			},
		{	"tp_conflicted",		eDxREC_FAILED_CONFLICT_TP		},
		{	"drm",					eDxREC_FAILED_DRM				},
		{	"no_hdd", 				eDxREC_FAILED_NOHDD				},
		{	"incomplete_hdd_full",	eDxREC_INCOMPLETE_HDDFULL		},
		{	"unknown",				eDxREC_FAILED_UNKNOWN			},
		{	"no_usb_hdd",			eDxREC_FAILED_NOUSBHDD 			},
		{	"disconnect_hdd",		eDxREC_FAILED_DISCONNECTUSBHDD	},
		{	"power_off", 			eDxREC_FAILED_POWER_OFF 		},
		{	"norightscr", 			eDxREC_FAILED_NORIGHT_SCR		},

		{	NULL,				0							}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_RecType =
{
	(HCHAR*)DxNAMEOF(RestRecType_e),

	{
		{	"instant",			eREST_REC_TYPE_Instant		},
		{	"delayed",			eREST_REC_TYPE_Delayed		},

		{	NULL,				0							}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_RecSrcType =
{
	(HCHAR*)DxNAMEOF(RestRecSrcType_e),

	{
		{	"channel",			eREST_REC_SRCTYPE_Channel			},

		{	NULL,				0							}
	}
};

#define ___LOCAL_FUNCTIONS___
STATIC void rest_datatype_JsonizeChannelRecRequest (HINT32 nJsonWriter, OxMediaRec_StartInfo_t *pstChanRec, HCHAR *szSelect)
{
	UNIXTIME		 utStartTime = 0;
	HCHAR			 szRecSource[48];

	HxSTD_PrintToStrN(szRecSource, 47, "hchannel://%d", pstChanRec->ulMasterSvcUid);
	(void)rest_datatype_SetJsonString(nJsonWriter, "recSource", szRecSource, szSelect);
	
	if (pstChanRec->nStartOffset < 0)
	{
		(void)rest_datatype_SetJsonString(nJsonWriter, "recType", "instant", szSelect);
	}
	else
	{
		(void)rest_datatype_SetJsonString(nJsonWriter, "recType", "delayed", szSelect);
	}

	HLIB_DATETIME_ConvertDateTimeToUnixTime(&(pstChanRec->stStartTime), &utStartTime);
	(void)rest_datatype_SetJsonInteger(nJsonWriter, "startTime", utStartTime, szSelect);
	(void)rest_datatype_SetJsonInteger(nJsonWriter, "duration", pstChanRec->ulDuration, szSelect);
}

#define ___MEMBER_FUNCTIONS___
#define _____RECORDINGS_____
HINT32 rest_datatype_JsonizeRecord (HINT32 nJsonWriter, DxRecListData_t *pstRecData, HCHAR *szSelect)
{
	HINT32				 nWriter;

	if (NULL == pstRecData)
	{
		HxLOG_Error("pstRecData NULL\n");
		return nJsonWriter;
	}

	nWriter = (0 == nJsonWriter) ? HLIB_JSONWRITER_Open() : nJsonWriter;
	if (0 == nWriter)
	{
		HxLOG_Error(">> HLIB_JSONWRITER_Open failed:\n");
		return 0;
	}

	HxJSONWRITER_ObjectBegin(nWriter);
	{
		(void)rest_datatype_SetJsonInteger(nWriter, "recordUid", pstRecData->ulContentId, szSelect);
		(void)rest_datatype_SetJsonString(nWriter, "url", pstRecData->szUrl, szSelect);
		(void)rest_datatype_SetJsonString(nWriter, "name", pstRecData->szName, szSelect);

		(void)rest_datatype_SetJsonString(nWriter, "recSourceType", "channel", szSelect);
		(void)rest_datatype_SetJsonString(nWriter, "srcName", pstRecData->szSvcName, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "srcNo", pstRecData->nSvcNo, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "srcOrgNetId", pstRecData->stMeta.usOnId, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "srcTsId", pstRecData->stMeta.usTsId, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "srcSvcId", pstRecData->stMeta.usSvcId, szSelect);
		(void)rest_datatype_SetJsonEnum(nWriter, "srcChannelType", DxNAMEOF(DxSvcType_e), (HINT32)pstRecData->eSvcType, szSelect);
		(void)rest_datatype_SetJsonEnum(nWriter, "srcVideoType", DxNAMEOF(DxVideoType_e), (HINT32)pstRecData->eVideoType, szSelect);
		(void)rest_datatype_SetJsonEnum(nWriter, "srcCasType", DxNAMEOF(DxCasType_e), (HINT32)pstRecData->eCasType, szSelect);

		(void)rest_datatype_SetJsonEnum(nWriter, "state", DxNAMEOF(DxRecStatus_e), (HINT32)pstRecData->eRecStatus, szSelect);
		(void)rest_datatype_SetJsonEnum(nWriter, "endReason", DxNAMEOF(DxRecEndReasonType_e), (HINT32)pstRecData->eRecEndReason, szSelect);

		(void)rest_datatype_SetJsonInteger(nWriter, "startTime", pstRecData->ulRecStartTime, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "endTime", pstRecData->stMeta.ulEndTime, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "duration", pstRecData->ulRecDuration, szSelect);

		if ((pstRecData->bEventBased) && (pstRecData->bSeriesRec))
		{
			(void)rest_datatype_SetJsonString(nWriter, "recordType", "seriesBased", szSelect);
		}
		else if ((pstRecData->bEventBased) && (!pstRecData->bSeriesRec))
		{
			(void)rest_datatype_SetJsonString(nWriter, "recordType", "eventBased", szSelect);
			(void)rest_datatype_SetJsonInteger(nWriter, "srcEventId", pstRecData->nEventId, szSelect);
		}
		else
		{
			(void)rest_datatype_SetJsonString(nWriter, "recordType", "eventBased", szSelect);
		}

		if (pstRecData->bPlayed)
		{
			(void)rest_datatype_SetJsonInteger(nWriter, "lastPlayedTime", pstRecData->ulLastViewTime, szSelect);
		}
		(void)rest_datatype_SetJsonInteger(nWriter, "userLocked", pstRecData->bUserLock, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "blocked", pstRecData->bBlocked, szSelect);
	}
	HxJSONWRITER_ObjectEnd(nWriter);

	return nWriter;	
}

HINT32 rest_datatype_JsonizeRecRequest (HINT32 nJsonWriter, RestRecRequest_t *pstRequest, HCHAR *szObjectName, HCHAR *szSelect)
{
	HINT32				 nWriter;

	if (NULL == pstRequest)
	{
		HxLOG_Error("pstRecData NULL\n");
		return nJsonWriter;
	}

	nWriter = (0 == nJsonWriter) ? HLIB_JSONWRITER_Open() : nJsonWriter;
	if (0 == nWriter)
	{
		HxLOG_Error(">> HLIB_JSONWRITER_Open failed:\n");
		return 0;
	}


	if (NULL == szObjectName)
	{
		// No Object: Nothing to do
	}
	else if ('\0' == *szObjectName)
	{
		// Make the object, but no name
		HxJSONWRITER_ObjectBegin(nWriter);
	}
	else
	{
		// Make the object
		HxJSONWRITER_Object(nWriter, szObjectName);
	}

	// Contents
	{
		(void)rest_datatype_SetJsonEnum(nWriter, "recSourceType", DxNAMEOF(RestRecSrcType_e), (HINT32)pstRequest->eSrcType, szSelect);

		switch (pstRequest->eSrcType)
		{
		case eREST_REC_SRCTYPE_Channel:
			rest_datatype_JsonizeChannelRecRequest(nWriter, &(pstRequest->unArgs.stChanRec), szSelect);
			break;

		default:
			break;
		}
	}

	if (NULL != szObjectName)
	{
		HxJSONWRITER_ObjectEnd(nWriter);
	}

	return nWriter;	
}

HERROR rest_datatype_DejsonizeRecRequest (HxJSON_t *pstJson, RestRecRequest_t *pstRequest, HBOOL bClearObj)
{
	if (NULL == pstRequest)
	{
		HxLOG_Error("pstRequest NULL\n");
		return ERR_FAIL;
	}

	if (NULL == pstJson)
	{
		HxLOG_Error("pstJson NULL\n");
		return ERR_FAIL;
	}

	if (TRUE == bClearObj)
	{
		HxSTD_MemSet(pstRequest, 0, sizeof(pstRequest));
	}

	(void)rest_datatype_GetJsonEnum(pstJson, "recSourceType", DxNAMEOF(RestRecSrcType_e), (HINT32 *)&(pstRequest->eSrcType));

	switch (pstRequest->eSrcType)
	{
	case eREST_REC_SRCTYPE_Channel:
		return rest_datatype_DejsonizeChannelRecRequest(pstJson, pstRequest, FALSE);
		
	default:
		HxLOG_Error(">>> Recording Source Wrong value: (%d)\n", pstRequest->eSrcType);
		break;
	}

	return ERR_FAIL;
}

HERROR rest_datatype_DejsonizeChannelRecRequest (HxJSON_t *pstJson, RestRecRequest_t *pstRequest, HBOOL bClearObj)
{
	HINT32						 nSrcSvcUid;
	HINT32						 nDuration = -1, nStartOffset = -1;
	UNIXTIME					 utStartTime;
	RestRecType_e				 eRecType = eREST_REC_TYPE_None;
	HCHAR						*szSrc = NULL;
	OxMediaRec_StartInfo_t		*pstChanRec;

	if (NULL == pstRequest)
	{
		HxLOG_Error("pstRequest NULL\n");
		return ERR_FAIL;
	}

	if (NULL == pstJson)
	{
		HxLOG_Error("pstJson NULL\n");
		return ERR_FAIL;
	}

	if (NULL == szSrc)
	{
		HxLOG_Error("szSrc NULL: memory allocation failed\n");
		return ERR_FAIL;
	}

	if (TRUE == bClearObj)
	{
		HxSTD_MemSet(pstRequest, 0, sizeof(RestRecRequest_t));
		pstRequest->eSrcType = eREST_REC_SRCTYPE_Channel;
	}
	
	pstChanRec = &(pstRequest->unArgs.stChanRec);

	szSrc = (HCHAR *)HLIB_JSON_Object_GetString(pstJson, "recSource");
	if (NULL == szSrc)
	{
		HxLOG_Error("No argument [recSource] in JSON\n");
		return ERR_FAIL;
	}

	if (HLIB_STD_StrStartWith((const HCHAR *)szSrc, "hchannel://") != TRUE)
	{
		HxLOG_Error("Source is not hchannel: [%s]\n", szSrc);
		return ERR_FAIL;
	}

	nSrcSvcUid = HLIB_STD_StrToINT32((const HCHAR *)(szSrc + 11));
	if (nSrcSvcUid <= 0)
	{
		HxLOG_Error("Source has no SVC UID: [%s]\n", szSrc);
		return ERR_FAIL;
	}

	utStartTime = (UNIXTIME)HLIB_STD_GetSystemTime();
	if (utStartTime < (60 * 60 * 24 * 3))
	{
		HxLOG_Error("Current System Time wrong... (%d)\n", utStartTime);
		return ERR_FAIL;
	}
	
	(void)rest_datatype_GetJsonInteger(pstJson, "duration", &nDuration);
	if (nDuration < 10)
	{
		HxLOG_Error("No duration value or too short: (%d)\n", nDuration);
		return ERR_FAIL;
	}
	
	(void)rest_datatype_GetJsonEnum(pstJson, "recType", DxNAMEOF(RestRecType_e), (HINT32 *)&eRecType);
	switch (eRecType)
	{
	case eREST_REC_TYPE_Instant:
		nStartOffset = -1;
		break;

	case eREST_REC_TYPE_Delayed:
		(void)rest_datatype_GetJsonInteger(pstJson, "delayedOffset", &nStartOffset);
		if (nStartOffset < 5)
		{
			HxLOG_Error("No delayed offset or too short: (%d)\n", nStartOffset);
			nStartOffset = -1;
		}
		else
		{
			utStartTime -= nStartOffset;
		}
		
		break;

	default:
		HxLOG_Error("RecType Invalid:(%d)\n", eRecType);
		return ERR_FAIL;
	}

	pstChanRec->ulMasterSvcUid	= (HUINT32)nSrcSvcUid;
	pstChanRec->ulSuppleSvcUid	= (HUINT32)nSrcSvcUid;
	pstChanRec->eSuppSvcType	= eOxMP_SUPPLESVCTYPE_Normal;
	pstChanRec->nStartOffset	= nStartOffset;
	(void)HLIB_DATETIME_ConvertUnixTimeToDateTime(utStartTime, &(pstChanRec->stStartTime));
	pstChanRec->ulDuration		= nDuration;
	pstChanRec->eStorageType	= eOxMR_TargetStorage_IntHdd;

	return ERR_OK;
}


#define _____INITIALIZATION_____
HERROR rest_datatype_InitRecord (void)
{
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_RecState);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_RecFailReason);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_RecType);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_RecSrcType);

	return ERR_OK;
}


#define ___GLOBAL_FUNCTIONS___

