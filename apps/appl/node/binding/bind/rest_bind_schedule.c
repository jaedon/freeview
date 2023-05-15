/*******************************************************************
	File Description
********************************************************************/
/**
	@file	 rest_bind_schedule.c
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
	ApkMetaSched_Handle_t	 hApkSchHandle;
} RestEpgContext_t;


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC RestEpgContext_t				 s_stRestBindSch_Context;
STATIC HxList_t						*s_pstRestBindSch_Notifier = NULL;

#define ___LOCAL_FUNCTIONS___
STATIC RestEpgContext_t *rest_bind_schedule_GetContext (void)
{
	return &s_stRestBindSch_Context;
}

#define _____COMMAND_FUNCTIONS_____
STATIC HERROR rest_bind_schedule_CmdRegisterNotifier (RestBind_Notifier_t pfNotifier)
{
	if (HLIB_LIST_Find(s_pstRestBindSch_Notifier, (const void *)pfNotifier) != NULL)
	{
		HxLOG_Error("Already the same notifier exists:\n");
		return ERR_OK;
	}

	s_pstRestBindSch_Notifier = HLIB_LIST_Append(s_pstRestBindSch_Notifier, (void *)pfNotifier);
	return ERR_OK;
}

STATIC HERROR rest_bind_schedule_CbRegisterNotifier (void *pvArg)
{
	RestMsg_t			*pstMsg = (RestMsg_t *)pvArg;
	RestBind_Notifier_t	 pfNotifier = (RestBind_Notifier_t)pstMsg->aulArgs[0];
	HERROR				 hErr = ERR_FAIL;

	if (NULL != pfNotifier)
	{
		hErr = rest_bind_schedule_CmdRegisterNotifier(pfNotifier);
	}

	pstMsg->pvResult = (void *)hErr;
	rest_task_SyncOut();

	return ERR_OK;
}

STATIC HERROR rest_bind_schedule_CmdUnregisterNotifier (RestBind_Notifier_t pfNotifier)
{
	s_pstRestBindSch_Notifier = HLIB_LIST_Remove(s_pstRestBindSch_Notifier, (const void *)pfNotifier);
	return ERR_OK;
}

STATIC HERROR rest_bind_schedule_CbUnregisterNotifier (void *pvArg)
{
	RestMsg_t			*pstMsg = (RestMsg_t *)pvArg;
	RestBind_Notifier_t	 pfNotifier = (RestBind_Notifier_t)pstMsg->aulArgs[0];
	HERROR				 hErr = ERR_FAIL;

	if (NULL != pfNotifier)
	{
		hErr = rest_bind_schedule_CmdUnregisterNotifier(pfNotifier);
	}

	pstMsg->pvResult = (void *)hErr;
	rest_task_SyncOut();

	return ERR_OK;
}

STATIC HCHAR *rest_bind_schedule_CmdAddSchedule (DxSchedule_t *pstSchedule)
{
	HUINT32				 ulSlot;
	HINT32				 nJsonWriter = rest_bind_OpenJsonWriter();
	HINT32				 nErrCode = REST_ERR_CODE_INTERNAL;
	HCHAR				*szErrString = REST_ERR_STRING_INTERNAL;
	HERROR				 hErr;

	if (0 == nJsonWriter)
	{
		HxLOG_Error("rest_bind_OpenJsonWriter failed:\n");
		return rest_bind_MakeErrorString(REST_ERR_CODE_NOMEM, REST_ERR_STRING_NOMEM, TRUE);
	}

	HxJSONWRITER_ObjectBegin(nJsonWriter);

	hErr = APK_META_SCHEDULE_AddSchedule(pstSchedule, &ulSlot);
	if (ERR_OK == hErr)
	{
		HxJSONWRITER_Object(nJsonWriter, "schedule");
		(void)rest_datatype_JsonizeSchedule(nJsonWriter, pstSchedule, NULL);
		HxJSONWRITER_ObjectEnd(nJsonWriter);

		nErrCode = REST_ERR_CODE_OK;
		szErrString = REST_ERR_STRING_OK;		
	}
	else
	{
		nErrCode = REST_ERR_CODE_INTERNAL;
		szErrString = REST_ERR_STRING_INTERNAL;
	}

	(void)rest_bind_MakeErrorJsWriter(nJsonWriter, nErrCode, szErrString);
	HxJSONWRITER_ObjectEnd(nJsonWriter);
	
	return rest_bind_CloseJsonWriter(nJsonWriter);
}

STATIC HCHAR *rest_bind_schedule_CmdUpdateSchedule (HUINT32 ulSlot, DxSchedule_t *pstSchedule)
{
	HINT32				 nJsonWriter = rest_bind_OpenJsonWriter();
	HINT32				 nErrCode = REST_ERR_CODE_INTERNAL;
	HCHAR				*szErrString = REST_ERR_STRING_INTERNAL;
	HERROR				 hErr;

	if (0 == nJsonWriter)
	{
		HxLOG_Error("rest_bind_OpenJsonWriter failed:\n");
		return rest_bind_MakeErrorString(REST_ERR_CODE_NOMEM, REST_ERR_STRING_NOMEM, TRUE);
	}

	HxJSONWRITER_ObjectBegin(nJsonWriter);

	hErr = APK_META_SCHEDULE_UpdateSchedule(ulSlot, pstSchedule);
	if (ERR_OK == hErr)
	{
		HxJSONWRITER_Object(nJsonWriter, "schedule");
		(void)rest_datatype_JsonizeSchedule(nJsonWriter, pstSchedule, NULL);
		HxJSONWRITER_ObjectEnd(nJsonWriter);

		nErrCode = REST_ERR_CODE_OK;
		szErrString = REST_ERR_STRING_OK;		
	}
	else
	{
		nErrCode = REST_ERR_CODE_INTERNAL;
		szErrString = REST_ERR_STRING_INTERNAL;
	}

	(void)rest_bind_MakeErrorJsWriter(nJsonWriter, nErrCode, szErrString);
	HxJSONWRITER_ObjectEnd(nJsonWriter);
	
	return rest_bind_CloseJsonWriter(nJsonWriter);
}

STATIC HCHAR *rest_bind_schedule_CmdGetSchedule (HUINT32 ulSlot, HCHAR *szMember)
{
	HINT32				 nJsonWriter = rest_bind_OpenJsonWriter();
	HINT32				 nErrCode = REST_ERR_CODE_INTERNAL;
	HCHAR				*szErrString = REST_ERR_STRING_INTERNAL;
	DxSchedule_t		*pstSchedule = NULL;
	HERROR				 hErr;

	if (0 == nJsonWriter)
	{
		HxLOG_Error("rest_bind_OpenJsonWriter failed:\n");
		return rest_bind_MakeErrorString(REST_ERR_CODE_NOMEM, REST_ERR_STRING_NOMEM, TRUE);
	}

	HxJSONWRITER_ObjectBegin(nJsonWriter);

	pstSchedule = (DxSchedule_t *)HLIB_STD_MemCalloc(sizeof(DxSchedule_t));
	if (NULL == pstSchedule)
	{
		HxLOG_Error("memory allocation failed:\n");
		nErrCode = REST_ERR_CODE_NOMEM;			szErrString = REST_ERR_STRING_NOMEM;
		goto END_FUNC;
	}

	hErr = APK_META_SCHEDULE_GetSchedule(ulSlot, pstSchedule);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("APK_META_SCHEDULE_GetSchedule err: ulSlot(%d)\n", ulSlot);
		nErrCode = REST_ERR_CODE_NODATA;			szErrString = REST_ERR_STRING_NODATA;
		goto END_FUNC;
	}

	HxJSONWRITER_StringValue(nJsonWriter, "schedule");
	(void)rest_datatype_JsonizeSchedule(nJsonWriter, pstSchedule, szMember);

	nErrCode = REST_ERR_CODE_OK;
	szErrString = REST_ERR_STRING_OK;		

END_FUNC:
	if (NULL != pstSchedule)			{ HLIB_STD_MemFree(pstSchedule); }
		
	(void)rest_bind_MakeErrorJsWriter(nJsonWriter, nErrCode, szErrString);
	HxJSONWRITER_ObjectEnd(nJsonWriter);
	
	return rest_bind_CloseJsonWriter(nJsonWriter);
}

STATIC HCHAR *rest_bind_schedule_CmdGetAllSchedule (HCHAR *szMember)
{
	HUINT32				 ulIndex, ulSchNum = 0;
	HINT32				 nJsonWriter = rest_bind_OpenJsonWriter();
	HINT32				 nErrCode = REST_ERR_CODE_INTERNAL;
	HCHAR				*szErrString = REST_ERR_STRING_INTERNAL;
	DxSchedule_t		*pstSchedule = NULL;
	HERROR				 hErr;

	if (0 == nJsonWriter)
	{
		HxLOG_Error("rest_bind_OpenJsonWriter failed:\n");
		return rest_bind_MakeErrorString(REST_ERR_CODE_NOMEM, REST_ERR_STRING_NOMEM, TRUE);
	}

	HxJSONWRITER_ObjectBegin(nJsonWriter);

	pstSchedule = (DxSchedule_t *)HLIB_STD_MemCalloc(sizeof(DxSchedule_t));
	if (NULL == pstSchedule)
	{
		HxLOG_Error("memory allocation failed:\n");
		nErrCode = REST_ERR_CODE_NOMEM;			szErrString = REST_ERR_STRING_NOMEM;
		goto END_FUNC;
	}

	hErr = APK_META_SCHEDULE_CountSchedule(&ulSchNum);
	if ((ERR_OK != hErr) || (0 == ulSchNum))
	{
		HxLOG_Error("APK_META_SCHEDULE_CountSchedule err:\n");
		nErrCode = REST_ERR_CODE_NODATA;			szErrString = REST_ERR_STRING_NODATA;
		goto END_FUNC;
	}

	HxJSONWRITER_Array(nJsonWriter, "schedule");

	for (ulIndex = 0; ulIndex < ulSchNum; ulIndex++)
	{
		hErr = APK_META_SCHEDULE_GetScheduleByIndex(ulIndex, pstSchedule);
		if (ERR_OK == hErr)
		{
			(void)rest_datatype_JsonizeSchedule(nJsonWriter, pstSchedule, szMember);
		}
	}
	
	HxJSONWRITER_ArrayEnd(nJsonWriter);

	nErrCode = REST_ERR_CODE_OK;
	szErrString = REST_ERR_STRING_OK;		

END_FUNC:
	if (NULL != pstSchedule)			{ HLIB_STD_MemFree(pstSchedule); }
		
	(void)rest_bind_MakeErrorJsWriter(nJsonWriter, nErrCode, szErrString);
	HxJSONWRITER_ObjectEnd(nJsonWriter);
	
	return rest_bind_CloseJsonWriter(nJsonWriter);
}

STATIC HCHAR *rest_bind_schedule_CmdDeleteSchedule (HUINT32 ulSlot, HCHAR *szMember)
{
	HINT32				 nJsonWriter = rest_bind_OpenJsonWriter();
	HINT32				 nErrCode = REST_ERR_CODE_INTERNAL;
	HCHAR				*szErrString = REST_ERR_STRING_INTERNAL;
	DxSchedule_t		*pstSchedule = NULL;
	HERROR				 hErr;

	if (0 == nJsonWriter)
	{
		HxLOG_Error("rest_bind_OpenJsonWriter failed:\n");
		return rest_bind_MakeErrorString(REST_ERR_CODE_NOMEM, REST_ERR_STRING_NOMEM, TRUE);
	}

	HxJSONWRITER_ObjectBegin(nJsonWriter);

	pstSchedule = (DxSchedule_t *)HLIB_STD_MemCalloc(sizeof(DxSchedule_t));
	if (NULL == pstSchedule)
	{
		HxLOG_Error("memory allocation failed:\n");
		nErrCode = REST_ERR_CODE_NOMEM;			szErrString = REST_ERR_STRING_NOMEM;
		goto END_FUNC;
	}

	hErr = APK_META_SCHEDULE_GetSchedule(ulSlot, pstSchedule);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("APK_META_SCHEDULE_GetSchedule err: ulSlot(%d)\n", ulSlot);
		nErrCode = REST_ERR_CODE_NODATA;			szErrString = REST_ERR_STRING_NODATA;
		goto END_FUNC;
	}

	hErr = APK_META_SCHEDULE_CancelSchedule(ulSlot);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("APK_META_SCHEDULE_CancelSchedule err: ulSlot(%d)\n", ulSlot);
		nErrCode = REST_ERR_CODE_INTERNAL;	szErrString = REST_ERR_STRING_INTERNAL;
		goto END_FUNC;
	}

	HxJSONWRITER_Object(nJsonWriter, "schedule");
	(void)rest_datatype_JsonizeSchedule(nJsonWriter, pstSchedule, szMember);
	HxJSONWRITER_ObjectEnd(nJsonWriter);

	nErrCode = REST_ERR_CODE_OK;
	szErrString = REST_ERR_STRING_OK;		

END_FUNC:
	if (NULL != pstSchedule)			{ HLIB_STD_MemFree(pstSchedule); }
		
	(void)rest_bind_MakeErrorJsWriter(nJsonWriter, nErrCode, szErrString);
	HxJSONWRITER_ObjectEnd(nJsonWriter);
	
	return rest_bind_CloseJsonWriter(nJsonWriter);
}

STATIC HERROR rest_bind_schedule_CbPost (void *pvArg)
{
	DxSchedule_t		*pstSchedule = NULL;
	HCHAR				*szResult = NULL;
	RestMsg_t			*pstMsg = (RestMsg_t *)pvArg;
	HERROR				 hErr;

	pstSchedule = (DxSchedule_t *)HLIB_STD_MemAlloc(sizeof(DxSchedule_t));
	if (NULL != pstSchedule)
	{
		HxJSON_t			 jsPayLoad = HLIB_JSON_Decode((HCHAR *)pstMsg->aulArgs[0]);

		if (NULL != jsPayLoad)
		{
			hErr = rest_datatype_DejsonizeSchedule(jsPayLoad, pstSchedule, TRUE);
			if (ERR_OK == hErr)
			{
				szResult = rest_bind_schedule_CmdAddSchedule(pstSchedule);
			}
			else
			{
				szResult = rest_bind_MakeErrorString(REST_ERR_CODE_REQUIRE_MANDATORYKEY, REST_ERR_STRING_REQUIRE_MANDATORYKEY, TRUE);
			}

			HLIB_JSON_Delete(jsPayLoad);
		}
		else
		{
			szResult = rest_bind_MakeErrorString(REST_ERR_CODE_REQUIRE_MANDATORYKEY, REST_ERR_STRING_REQUIRE_MANDATORYKEY, TRUE);
		}

		HLIB_STD_MemFree(pstSchedule);
	}
	else
	{
		szResult = rest_bind_MakeErrorString(REST_ERR_CODE_NOMEM, REST_ERR_STRING_NOMEM, TRUE);
	}

	pstMsg->pvResult = (void *)szResult;
	rest_task_SyncOut();

	return ERR_OK;
}

STATIC HERROR rest_bind_schedule_CbPut (void *pvArg)
{
	HUINT32				 ulSlot;
	DxSchedule_t		*pstOrgSch = NULL, *pstNewSch = NULL;
	HCHAR				*szResult = NULL;
	RestMsg_t			*pstMsg = (RestMsg_t *)pvArg;
	HxJSON_t			 jsPayLoad = NULL;
	HERROR				 hErr;

	pstOrgSch = (DxSchedule_t *)HLIB_STD_MemCalloc(sizeof(DxSchedule_t));
	pstNewSch = (DxSchedule_t *)HLIB_STD_MemCalloc(sizeof(DxSchedule_t));
	if ((NULL == pstOrgSch) || (NULL == pstNewSch))
	{
		HxLOG_Error("Memory allocation failed: pstOrgSch(%p), pstNewSch(%d)\n", pstOrgSch, pstNewSch);
		szResult = rest_bind_MakeErrorString(REST_ERR_CODE_NOMEM, REST_ERR_STRING_NOMEM, TRUE);
		goto END_FUNC;
	}

	ulSlot = pstMsg->aulArgs[0];

	hErr = APK_META_SCHEDULE_GetSchedule(ulSlot, pstOrgSch);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("No such schedule: ulSlot(%d)\n", ulSlot);
		szResult = rest_bind_MakeErrorString(REST_ERR_CODE_NODATA, REST_ERR_STRING_NODATA, TRUE);
		goto END_FUNC;
	}

	HxSTD_MemCopy(pstNewSch, pstOrgSch, sizeof(DxSchedule_t));

	jsPayLoad = HLIB_JSON_Decode((HCHAR *)pstMsg->aulArgs[1]);
	if (NULL == jsPayLoad)
	{
		HxLOG_Error("Payload decode failed... Wrong JSON?\n");
		szResult = rest_bind_MakeErrorString(REST_ERR_CODE_REQUIRE_MANDATORYKEY, REST_ERR_STRING_REQUIRE_MANDATORYKEY, TRUE);
		goto END_FUNC;
	}

	hErr = rest_datatype_DejsonizeSchedule(jsPayLoad, pstNewSch, FALSE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("rest_datatype_DejsonizeSchedule err:\n");
		szResult = rest_bind_MakeErrorString(REST_ERR_CODE_REQUIRE_MANDATORYKEY, REST_ERR_STRING_REQUIRE_MANDATORYKEY, TRUE);
		goto END_FUNC;
	}

	// If changed, update the schedule
	if (HxSTD_MemCmp(pstNewSch, pstOrgSch, sizeof(DxSchedule_t)) != 0)
	{
		szResult = rest_bind_schedule_CmdUpdateSchedule(ulSlot, pstNewSch);
	}
	else
	{
		HxLOG_Error("No schedule changed: not updated.\n");
		szResult = rest_bind_MakeErrorString(REST_ERR_CODE_OK, REST_ERR_STRING_OK, TRUE);
	}

END_FUNC:
	if (NULL != pstOrgSch)						{ HLIB_MEM_Free(pstOrgSch); }
	if (NULL != pstNewSch)						{ HLIB_MEM_Free(pstNewSch); }
	if (NULL != jsPayLoad)						{ HLIB_JSON_Delete(jsPayLoad); }

	pstMsg->pvResult = (void *)szResult;
	rest_task_SyncOut();

	return ERR_OK;
}

STATIC HERROR rest_bind_schedule_CbGet (void *pvArg)
{
	HUINT32 			 ulSlot;
	HCHAR				*szArgJson = NULL;
	HCHAR				*szResult = NULL, *szMember = NULL;
	RestMsg_t			*pstMsg = (RestMsg_t *)pvArg;

	ulSlot = pstMsg->aulArgs[0];
	szArgJson = (HCHAR *)pstMsg->aulArgs[1];
	if ((NULL != szArgJson) && ('\0' != *szArgJson))
	{
		HxJSON_t			 jsArgs = HLIB_JSON_Decode(szArgJson);

		if (NULL != jsArgs)
		{
			szMember = (HCHAR *)HLIB_JSON_Object_GetString(jsArgs, "member");
		}
	}
	
	if ((HUINT32)-1 == ulSlot)
	{
		szResult = rest_bind_schedule_CmdGetAllSchedule(szMember);
	}
	else
	{
		szResult = rest_bind_schedule_CmdGetSchedule(ulSlot, szMember);
	}

	pstMsg->pvResult = (void *)szResult;
	rest_task_SyncOut();

	return ERR_OK;
}

STATIC HERROR rest_bind_schedule_CbDelete (void *pvArg)
{
	HUINT32 			 ulSlot;
	HCHAR				*szResult = NULL, *szMember = NULL;
	RestMsg_t			*pstMsg = (RestMsg_t *)pvArg;

	ulSlot = pstMsg->aulArgs[0];

	szResult = rest_bind_schedule_CmdDeleteSchedule(ulSlot, szMember);
	
	pstMsg->pvResult = (void *)szResult;
	rest_task_SyncOut();

	return ERR_OK;
}

#define ___MEMBER_FUNCTIONS___
HERROR rest_bind_schedule_Init (void)
{
	STATIC HBOOL		 s_bInited = FALSE;

	if (TRUE != s_bInited)
	{
		RestEpgContext_t	*pstContext = rest_bind_schedule_GetContext();

		HxSTD_MemSet(pstContext, 0, sizeof(RestEpgContext_t));
		
		pstContext->hApkSchHandle = APK_META_SCHEDULE_GetInstance();

		s_bInited = TRUE;
	}

	return ERR_OK;
}

#define ___API_FUNCTIONS___
int REST_BIND_SCHEDULE_RegisterNotifier (RestBind_Notifier_t pfNotifier)
{
	RestMsg_t			 stMsg;
	HERROR				 hErr;

	stMsg.pvResult		= (void *)0;
	stMsg.aulArgs[0]	= (HUINT32)pfNotifier;

	hErr = rest_task_RequestToExecute(rest_bind_schedule_CbRegisterNotifier, (void *)&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("rest_task_RequestToExecute err:\n");
		return ERR_FAIL;
	}

	rest_task_SyncIn();

	return (int)stMsg.pvResult;
}

int REST_BIND_SCHEDULE_UnregisterNotifier (RestBind_Notifier_t pfNotifier)
{
	RestMsg_t			 stMsg;
	HERROR				 hErr;

	stMsg.pvResult		= (void *)0;
	stMsg.aulArgs[0]	= (HUINT32)pfNotifier;
	
	hErr = rest_task_RequestToExecute(rest_bind_schedule_CbUnregisterNotifier, (void *)&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("rest_task_RequestToExecute err:\n");
		return ERR_FAIL;
	}

	rest_task_SyncIn();

	return (int)stMsg.pvResult;
}

// POST http://127.0.0.1:37564/stb-api/v1/schedules
char *REST_BIND_SCHEDULE_Post (char *szSessionId, char *szPayload)
{
	RestMsg_t			 stMsg;
	char				*szResult = NULL;
	HERROR				 hErr;

	stMsg.szSessionId	= REST_DUMMY_SESSION_ID;
	stMsg.pvResult		= NULL;
	stMsg.aulArgs[0]	= (HUINT32)szPayload;
	
	hErr = rest_task_RequestToExecute(rest_bind_schedule_CbPost, (void *)&stMsg);
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

// PUT http://127.0.0.1:37564/stb-api/v1/schedules/1
char *REST_BIND_SCHEDULE_PutOne (char *szSessionId, int nScheduleUid, char *szPayload)
{
	RestMsg_t			 stMsg;
	char				*szResult = NULL;
	HERROR				 hErr;

	stMsg.szSessionId	= REST_DUMMY_SESSION_ID;
	stMsg.pvResult		= NULL;
	stMsg.aulArgs[0]	= (HUINT32)nScheduleUid;
	stMsg.aulArgs[1]	= (HUINT32)szPayload;
	
	hErr = rest_task_RequestToExecute(rest_bind_schedule_CbPut, (void *)&stMsg);
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

// GET http://127.0.0.1:37564/stb-api/v1/schedules
char *REST_BIND_SCHEDULE_Get (char *szSessionId, char *szArgJson)
{
	RestMsg_t			 stMsg;
	char				*szResult = NULL;
	HERROR				 hErr;

	stMsg.szSessionId	= REST_DUMMY_SESSION_ID;
	stMsg.pvResult		= NULL;
	stMsg.aulArgs[0]	= (HUINT32)-1;
	stMsg.aulArgs[1]	= (HUINT32)szArgJson;
	
	hErr = rest_task_RequestToExecute(rest_bind_schedule_CbGet, (void *)&stMsg);
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

// GET http://127.0.0.1:37564/stb-api/v1/schedules/1
char *REST_BIND_SCHEDULE_GetOne (char *szSessionId, int nScheduleUid, char *szArgJson)
{
	RestMsg_t			 stMsg;
	char				*szResult = NULL;
	HERROR				 hErr;

	stMsg.szSessionId	= REST_DUMMY_SESSION_ID;
	stMsg.pvResult		= NULL;
	stMsg.aulArgs[0]	= (HUINT32)nScheduleUid;
	stMsg.aulArgs[1]	= (HUINT32)szArgJson;
	
	hErr = rest_task_RequestToExecute(rest_bind_schedule_CbGet, (void *)&stMsg);
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

// DELETE http://127.0.0.1:37564/stb-api/v1/schedules/1
char *REST_BIND_SCHEDULE_DeleteOne (char *szSessionId, int nScheduleUid)
{
	RestMsg_t			 stMsg;
	char				*szResult = NULL;
	HERROR				 hErr;

	stMsg.szSessionId	= REST_DUMMY_SESSION_ID;
	stMsg.pvResult		= NULL;
	stMsg.aulArgs[0]	= (HUINT32)nScheduleUid;
	
	hErr = rest_task_RequestToExecute(rest_bind_schedule_CbDelete, (void *)&stMsg);
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


