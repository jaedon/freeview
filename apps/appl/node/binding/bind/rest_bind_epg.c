/*******************************************************************
	File Description
********************************************************************/
/**
	@file	 rest_bind_epg.c
	@brief

	Description:  									\n
	Module: node/binding/datatype		\n

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
#include <dapi.h>
#include <apk.h>
#include <rest_maintask.h>
#include <rest_datatype.h>
#include <rest_bind.h>

/*******************************************************************/
/********************      Local Definition         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{

} RestEpgContext_t;

typedef struct
{
	DxEPG_Type_e	 eEpgType;
	HCHAR			*szSqlName;
} RestEpgSqlName_t;

typedef struct
{
	HINT32			 nJsonWriter;
	HCHAR			*szMember;

	HUINT32			 ulNumEvent;
	HxVector_t		*pstEvents;
} RestEpgSearchResult_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HBOOL						 s_stRestBindEpg_Inited = FALSE;
STATIC HxList_t						*s_pstRestBindEpg_Notifier = NULL;
STATIC RestEpgSqlName_t				 s_astRestBindEpg_SqlName[] = {
																	{ eDxEPG_TYPE_PF, DxNAMEOF(eDxEPG_TYPE_PF) },
																	{ eDxEPG_TYPE_SC, DxNAMEOF(eDxEPG_TYPE_SC) },
																	{ eDxEPG_TYPE_IP, DxNAMEOF(eDxEPG_TYPE_IP) },
																	{ eDxEPG_TYPE_TVTV, DxNAMEOF(eDxEPG_TYPE_TVTV) },

																	{ eDxEPG_TYPE_NONE, NULL}
																};

#define ___LOCAL_FUNCTIONS___

#define _____COMMAND_FUNCTIONS_____
STATIC HERROR rest_bind_epg_CmdRegisterNotifier (RestBind_Notifier_t pfNotifier)
{
	if (HLIB_LIST_Find(s_pstRestBindEpg_Notifier, (const void *)pfNotifier) != NULL)
	{
		HxLOG_Error("Already the same notifier exists:\n");
		return ERR_OK;
	}

	s_pstRestBindEpg_Notifier = HLIB_LIST_Append(s_pstRestBindEpg_Notifier, (void *)pfNotifier);
	return ERR_OK;
}

STATIC HERROR rest_bind_epg_CbRegisterNotifier (void *pvArg)
{
	RestMsg_t			*pstMsg = (RestMsg_t *)pvArg;
	RestBind_Notifier_t	 pfNotifier = (RestBind_Notifier_t)pstMsg->aulArgs[0];
	HERROR				 hErr = ERR_FAIL;

	if (NULL != pfNotifier)
	{
		hErr = rest_bind_epg_CmdRegisterNotifier(pfNotifier);
	}

	pstMsg->pvResult = (void *)hErr;
	rest_task_SyncOut();

	return ERR_OK;
}

STATIC HERROR rest_bind_epg_CmdUnregisterNotifier (RestBind_Notifier_t pfNotifier)
{
	s_pstRestBindEpg_Notifier = HLIB_LIST_Remove(s_pstRestBindEpg_Notifier, (const void *)pfNotifier);
	return ERR_OK;
}

STATIC HERROR rest_bind_epg_CbUnregisterNotifier (void *pvArg)
{
	RestMsg_t			*pstMsg = (RestMsg_t *)pvArg;
	RestBind_Notifier_t	 pfNotifier = (RestBind_Notifier_t)pstMsg->aulArgs[0];
	HERROR				 hErr = ERR_FAIL;

	if (NULL != pfNotifier)
	{
		hErr = rest_bind_epg_CmdUnregisterNotifier(pfNotifier);
	}

	pstMsg->pvResult = (void *)hErr;
	rest_task_SyncOut();

	return ERR_OK;
}

STATIC void rest_bind_epg_OnDataReceived (const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata)
{
	RestEpgSearchResult_t	*pstResult = (RestEpgSearchResult_t *)userdata;

	HxJSONWRITER_ObjectBegin(pstResult->nJsonWriter);
	{
		// Events:
		if (data && size)
		{
			DAxIterator_t		 stIter;

			HxJSONWRITER_Array(pstResult->nJsonWriter, "EpgPrograms");

			DAPI_InitIterator(&stIter, data, size, 0);
			while (DAPI_NextIterator(&stIter))
			{
				DxEvent_t		*pstEvent = (DxEvent_t *)DLIB_NewType(stIter.name, stIter.uid, stIter.data, stIter.size);

				if (NULL != pstEvent)
				{
					(void)rest_datatype_JsonizeEvent(pstResult->nJsonWriter, pstEvent, pstResult->szMember);

					pstResult->ulNumEvent++;
					APK_EVENT_Delete(pstEvent);
				}
			}

			HxJSONWRITER_ArrayEnd(pstResult->nJsonWriter);
		}

		// Error:
		if (pstResult->ulNumEvent > 0)
		{
			(void)rest_bind_MakeErrorJsWriter(pstResult->nJsonWriter, REST_ERR_CODE_OK, REST_ERR_STRING_OK);
		}
		else
		{
			(void)rest_bind_MakeErrorJsWriter(pstResult->nJsonWriter, REST_ERR_CODE_NODATA, REST_ERR_STRING_NODATA);
		}
	}
	HxJSONWRITER_ObjectEnd(pstResult->nJsonWriter);
}

STATIC HCHAR *rest_bind_epg_CmdGetSvcEpg (DxEPG_Type_e bEpgType, HINT32 nSvcUid, HUINT32 ulStartTime, HUINT32 ulDuration, HCHAR *szMember)
{
	HBOOL					 bWritten = FALSE;
	HINT32					 nCnt = 0;
	HINT32					 nWriter = rest_bind_OpenRwstreamWriter();
	HINT32					 nJsonWriter = rest_bind_OpenJsonWriter();
	HCHAR					*szQuery = NULL, *szResult = NULL;
	RestEpgSearchResult_t	 stResult;

	if ((0 == nWriter) || (0 == nJsonWriter))
	{
		return rest_bind_MakeErrorString(REST_ERR_CODE_NOMEM, REST_ERR_STRING_NOMEM, TRUE);
	}

	HxSTD_MemSet(&stResult, 0, sizeof(RestEpgSearchResult_t));

	for (nCnt = 0; eDxEPG_TYPE_NONE != s_astRestBindEpg_SqlName[nCnt].eEpgType; nCnt++)
	{
		RestEpgSqlName_t	*pstSqlName = &(s_astRestBindEpg_SqlName[nCnt]);

		if (pstSqlName->eEpgType & bEpgType)
		{
			if (bWritten)
			{
				HLIB_RWSTREAM_Print(nWriter, " UNION ");
			}

			HLIB_RWSTREAM_Print(nWriter,
				"SELECT \'%s\' AS tablename, uid, link, linkdata FROM %s WHERE svcuid=%d AND start_time>=%d AND start_time<%d",
				pstSqlName->szSqlName, pstSqlName->szSqlName, nSvcUid, ulStartTime, ulStartTime + ulDuration);

			bWritten = TRUE;
		}
	}

	szQuery = rest_bind_CloseRwstreamWriter(nWriter);
	if ((NULL != szQuery) && ('\0' != *szQuery))
	{
		HINT32		 nReqId = 0;

		stResult.nJsonWriter	= nJsonWriter;
		stResult.szMember		= szMember;
		nReqId = DAPI_Query(szQuery, rest_bind_epg_OnDataReceived, (void *)&stResult);
		if (0 != nReqId)
		{
			(void)DAPI_Wait(5000, nReqId);
		}
	}

	if (NULL != szQuery)
	{
		HLIB_STD_MemFree(szQuery);
	}

	szResult = rest_bind_CloseJsonWriter(nJsonWriter);
	if ((NULL != szResult) && ('\0' == *szResult))
	{
		HLIB_STD_MemFree(szResult);
		szResult = rest_bind_MakeErrorString(REST_ERR_CODE_INTERNAL, REST_ERR_STRING_INTERNAL, TRUE);
	}

	return szResult;
}

STATIC HCHAR *rest_bind_epg_CmdGetEpgStatus (HCHAR *szSelect)
{
	HINT32					 nJsonWriter = rest_bind_OpenJsonWriter();
	HCHAR					*szResult = NULL;

	if (0 == nJsonWriter)
	{
		return rest_bind_MakeErrorString(REST_ERR_CODE_NOMEM, REST_ERR_STRING_NOMEM, TRUE);
	}

	HxJSONWRITER_ObjectBegin(nJsonWriter);
	{
		// EPG Status
		HxJSONWRITER_Object(nJsonWriter, "epgStatus");
		{
			(void)rest_datatype_SetJsonInteger(nJsonWriter, "currentTime", HLIB_STD_GetSystemTime(), szSelect);
			(void)rest_datatype_SetJsonInteger(nJsonWriter, "maxSupportDuration", REST_BIND_EPG_DURATION_MAX, szSelect);
		}
		HxJSONWRITER_ObjectEnd(nJsonWriter);

		// Error:
		(void)rest_bind_MakeErrorJsWriter(nJsonWriter, REST_ERR_CODE_OK, REST_ERR_STRING_OK);
	}
	HxJSONWRITER_ObjectEnd(nJsonWriter);

	szResult = rest_bind_CloseJsonWriter(nJsonWriter);
	if ((NULL != szResult) && ('\0' == *szResult))
	{
		HLIB_STD_MemFree(szResult);
		szResult = rest_bind_MakeErrorString(REST_ERR_CODE_INTERNAL, REST_ERR_STRING_INTERNAL, TRUE);
	}

	return szResult;
}
STATIC HERROR rest_bind_epg_CbGet (void *pvArg)
{
	HINT32				 nSvcUid = -1;
	HUINT32				 ulStartTime = 0, ulDuration = 0;
	HCHAR				*szMember = NULL;
	HCHAR				*szResult = NULL;
	RestMsg_t			*pstMsg = (RestMsg_t *)pvArg;
	HxJSON_t			 jsArgs;

	jsArgs = HLIB_JSON_Decode((HCHAR *)pstMsg->aulArgs[0]);
	if (NULL != jsArgs)
	{
		(void)rest_bind_GetNumberValueFromJson(jsArgs, "svcuid", &nSvcUid);
		(void)rest_bind_GetNumberValueFromJson(jsArgs, "starttime", (HINT32 *)&ulStartTime);
		(void)rest_bind_GetNumberValueFromJson(jsArgs, "duration", (HINT32 *)&ulDuration);
		(void)rest_bind_GetStringValueFromJson(jsArgs, "member", &szMember);
	}

	if (nSvcUid > 0)
	{
		if (0 == ulStartTime)			{ ulStartTime = HLIB_STD_GetSystemTime(); }
		if (0 == ulDuration)			{ ulDuration = REST_BIND_EPG_DURATION_MAX; }

		szResult = rest_bind_epg_CmdGetSvcEpg(eDxEPG_TYPE_ALL, nSvcUid, ulStartTime, ulDuration, szMember);
	}
	else
	{
		szResult = rest_bind_epg_CmdGetEpgStatus(szMember);
	}

	pstMsg->pvResult = (void *)szResult;
	rest_task_SyncOut();

	return ERR_OK;
}

#define ___MEMBER_FUNCTIONS___
HERROR rest_bind_epg_Init (void)
{
	if (TRUE != s_stRestBindEpg_Inited)
	{
		// Initialize the APPKIT
		(void)APK_META_QUERY_Init(TRUE);

		s_stRestBindEpg_Inited = TRUE;
	}

	return ERR_OK;
}

#define ___API_FUNCTIONS___
int REST_BIND_EPG_RegisterNotifier (RestBind_Notifier_t pfNotifier)
{
	RestMsg_t			 stMsg;
	HERROR				 hErr;

	stMsg.pvResult		= (void *)0;
	stMsg.aulArgs[0]	= (HUINT32)pfNotifier;

	hErr = rest_task_RequestToExecute(rest_bind_epg_CbRegisterNotifier, (void *)&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("rest_task_RequestToExecute err:\n");
		return ERR_FAIL;
	}

	rest_task_SyncIn();

	return (int)stMsg.pvResult;
}

int REST_BIND_EPG_UnregisterNotifier (RestBind_Notifier_t pfNotifier)
{
	RestMsg_t			 stMsg;
	HERROR				 hErr;

	stMsg.pvResult		= (void *)0;
	stMsg.aulArgs[0]	= (HUINT32)pfNotifier;

	hErr = rest_task_RequestToExecute(rest_bind_epg_CbUnregisterNotifier, (void *)&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("rest_task_RequestToExecute err:\n");
		return ERR_FAIL;
	}

	rest_task_SyncIn();

	return (int)stMsg.pvResult;
}

char *REST_BIND_EPG_Get (char *szSessionId, char *szArgJson)
{
	RestMsg_t			 stMsg;
	char				*szResult = NULL;
	HERROR				 hErr;

	stMsg.szSessionId	= REST_DUMMY_SESSION_ID;
	stMsg.pvResult		= NULL;
	stMsg.aulArgs[0]	= (HUINT32)szArgJson;

	hErr = rest_task_RequestToExecute(rest_bind_epg_CbGet, (void *)&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("rest_task_RequestToExecute err:\n");
		goto ERROR;
	}

	rest_task_SyncIn();

	szResult = (char *)stMsg.pvResult;
	if (NULL == szResult)
	{
		HxLOG_Error("cmd not executed:\n");
		goto ERROR;
	}

	rest_datatype_AppendStringBufferToTemporary(szResult);
	return szResult;

ERROR:
	return "";
}

