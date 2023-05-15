/*******************************************************************
	File Description
********************************************************************/
/**
	@file	 rest_bind_player.c
	@brief

	Description:  									\n
	Module: node/binding/bind		\n

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
} restBindPlayerCCbInfo_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HBOOL						 s_stRestBindPlay_Inited = FALSE;
STATIC HUINT32						 s_ulRestBindPlay_ContextNum = 0;
STATIC RestBindPlayerContext_t		*s_pstRestBindPlay_ContextArray = NULL;
STATIC HxList_t						*s_pstRestBindPlay_Notifier = NULL;

#define ___LOCAL_FUNCTIONS___
STATIC HERROR rest_bind_play_InitContext (void)
{
	HUINT32 					 ulCnt, ulViewNum = 0;
	RestBindPlayerContext_t 	*pstContextArray = NULL;
	HERROR						 hErr, hResult = ERR_FAIL;

	if (NULL != s_pstRestBindPlay_ContextArray)
	{
		hResult = ERR_OK;
		goto END_FUNC;
	}

	hErr = APK_MEDIA_PLAY_GetViewNumber(&ulViewNum);
	if ((ERR_OK != hErr) || (0 == ulViewNum))
	{
		HxLOG_Error("APK_MEDIA_PLAY_GetViewNumber err:\n");
		goto END_FUNC;
	}

	pstContextArray = (RestBindPlayerContext_t *)HLIB_STD_MemCalloc(sizeof(RestBindPlayerContext_t) * ulViewNum);
	if (NULL == pstContextArray)
	{
		HxLOG_Error("Memory Allocation for the context failed:\n");
		goto END_FUNC;
	}

	for (ulCnt = 0; ulCnt < ulViewNum; ulCnt++)
	{
		RestBindPlayerContext_t 	*pstContext = &(pstContextArray[ulCnt]);

		pstContext->ulViewId = ulCnt;
		(void)APK_MEDIA_PLAY_RegisterNotifier(ulCnt, rest_bind_play_NotifyApkMediaPlay, NULL, (void *)pstContext);
	}


	s_ulRestBindPlay_ContextNum = ulViewNum;
	s_pstRestBindPlay_ContextArray = pstContextArray;
	pstContextArray = NULL;
	hResult = ERR_OK;

END_FUNC:
	if (NULL != pstContextArray)
	{
		HLIB_STD_MemFree(pstContextArray);
	}

	return hResult;
}

STATIC HERROR rest_bind_play_CbInitContext (void *pvArg)
{
	HUINT32			 ulViewNum;
	HERROR			 hErr;

	HLIB_STD_TaskSleep(100);

	ulViewNum = 0;
	hErr = APK_MEDIA_PLAY_GetViewNumber(&ulViewNum);
	if ((ERR_OK != hErr) || (0 == ulViewNum))
	{
		hErr = rest_task_RequestToExecute(rest_bind_play_CbInitContext, (void *)NULL);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("rest_bind_play_CbInitContext failed:\n");
		}

		return ERR_FAIL;
	}

	HLIB_STD_TaskSleep(50);

	(void)rest_bind_play_InitContext();
	HxLOG_Info("Player Initialization succeeded:\n");

	return ERR_OK;
}

STATIC HERROR rest_bind_play_GetStartInfoFromPayload (HxJSON_t *pjsPayload, OxMediaPlay_StartInfo_t *pstStart, HCHAR **pszErrString)
{
	HCHAR					*szSrcType = NULL;
	HCHAR					*szSource = NULL;

	*pszErrString	= NULL;
	pstStart->eType	= eOxMP_MEDIATYPE_NONE;
	szSrcType		= (HCHAR *)HLIB_JSON_Object_GetString(pjsPayload, "sourceType");
	szSource		= (HCHAR *)HLIB_JSON_Object_GetString(pjsPayload, "source");

	if ((NULL == szSrcType) || (NULL == szSource))
	{
		HxLOG_Error("szSrcType(0x%08x), szSource(0x%08x)\n", (HUINT32)szSrcType, (HUINT32)szSource);
		*pszErrString = rest_bind_MakeErrorString(REST_ERR_CODE_REQUIRE_MANDATORYKEY, REST_ERR_STRING_REQUIRE_MANDATORYKEY, TRUE);
		return ERR_FAIL;
	}

	// Live Play
	if (HxSTD_StrCmp(szSrcType, "live") == 0)
	{
		HINT32			 nSvcUid;
		DxService_t 	*pstSvc;

		if (HLIB_STD_StrStartWith((const HCHAR *)szSource, "hchannel://") != TRUE)
		{
			HxLOG_Error("Source is not hchannel: [%s]\n", szSource);
			*pszErrString = rest_bind_MakeErrorString(REST_ERR_CODE_WRONG_SOURCE, REST_ERR_STRING_WRONG_SOURCE, TRUE);
			return ERR_FAIL;
		}

		nSvcUid = HLIB_STD_StrToINT32((const HCHAR *)(szSource + 11));
		if (nSvcUid <= 0)
		{
			HxLOG_Error("Source has no SVC UID: [%s]\n", szSource);
			*pszErrString = rest_bind_MakeErrorString(REST_ERR_CODE_WRONG_SOURCE, REST_ERR_STRING_WRONG_SOURCE, TRUE);
			return ERR_FAIL;
		}

		pstSvc = APK_META_SVC_GetService(nSvcUid);
		if (NULL == pstSvc)
		{
			HxLOG_Error("Source Wrong: szSource[%s], nSvcUid[%d]\n", szSource, nSvcUid);
			*pszErrString = rest_bind_MakeErrorString(REST_ERR_CODE_WRONG_SOURCE, REST_ERR_STRING_WRONG_SOURCE, TRUE);
			return ERR_FAIL;
		}

		pstStart->eType					= eOxMP_MEDIATYPE_LIVE;
		pstStart->stLive.eLiveType		= eOxMP_LIVETYPE_MAIN;
		pstStart->stLive.ulMasterSvcUid	= (HUINT32)nSvcUid;
		pstStart->stLive.ulSuppleSvcUid	= (HUINT32)nSvcUid;

		APK_META_SVC_Delete((void *)pstSvc);
	}
	// PVR Play
	else if (HxSTD_StrCmp(szSrcType, "pvrpb") == 0)
	{
		HUINT32			 ulContentsUid;
		DxRecListData_t *pstRecord;

		if (HLIB_STD_StrStartWith((const HCHAR *)szSource, "hrecord://") != TRUE)
		{
			HxLOG_Error("Source is not hrecord: [%s]\n", szSource);
			*pszErrString = rest_bind_MakeErrorString(REST_ERR_CODE_WRONG_SOURCE, REST_ERR_STRING_WRONG_SOURCE, TRUE);
			return ERR_FAIL;
		}

		ulContentsUid = HLIB_STD_StrToUINT32((const HCHAR *)(szSource + 10));
		if (0 == ulContentsUid)
		{
			HxLOG_Error("Source has no REC Contents UID: [%s]\n", szSource);
			*pszErrString = rest_bind_MakeErrorString(REST_ERR_CODE_WRONG_SOURCE, REST_ERR_STRING_WRONG_SOURCE, TRUE);
			return ERR_FAIL;
		}

		pstRecord = APK_META_RECORD_GetByCID((const HUINT32)ulContentsUid);
		if (NULL == pstRecord)
		{
			HxLOG_Error("Source Wrong: szSource[%s], ulContentsUid[%d]\n", szSource, ulContentsUid);
			*pszErrString = rest_bind_MakeErrorString(REST_ERR_CODE_WRONG_SOURCE, REST_ERR_STRING_WRONG_SOURCE, TRUE);
			return ERR_FAIL;
		}

		pstStart->eType					= eOxMP_MEDIATYPE_PVRPLAYBACK;
		pstStart->stPvrPb.ulContentId	= ulContentsUid;
		pstStart->stPvrPb.ulStartTime	= 0;

		APK_META_RECORD_Release(pstRecord);
	}
	// No supported media type:
	else
	{
		HxLOG_Error("Unsupported Media Type [%s]\n", szSrcType);
		*pszErrString = rest_bind_MakeErrorString(REST_ERR_CODE_WRONG_SOURCETYPE, REST_ERR_STRING_WRONG_SOURCETYPE, TRUE);
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HCHAR *rest_bind_play_WriteCmdResponse (HUINT32 ulPlayerId, HCHAR *szSelect)
{
	HINT32					 nJWriter = 0;
	HCHAR					*szResultStr = NULL;
	RestPlayerStatus_t		*pstStatus = NULL;
	HINT32					 nErrCode;
	HCHAR					*szErrString;

	pstStatus = rest_bind_play_GetPlayerStatus(ulPlayerId);
	if (NULL == pstStatus)
	{
		HxLOG_Error("Memory allocation failed:\n");
		nErrCode = REST_ERR_CODE_NOMEM;		szErrString = REST_ERR_STRING_NOMEM;
		goto END_FUNC;
	}

	nJWriter = rest_bind_OpenJsonWriter();
	if (0 == nJWriter)
	{
		HxLOG_Error("rest_bind_OpenJsonWriter() err:\n");
		nErrCode = REST_ERR_CODE_NOMEM;		szErrString = REST_ERR_STRING_NOMEM;
		goto END_FUNC;
	}

	// Jsonize the state
	HxJSONWRITER_ObjectBegin(nJWriter);
	{
		// Play Status
		(void)HxJSONWRITER_StringValue(nJWriter, "playStatus");
		(void)rest_datatype_JsonizePlayerStatus(nJWriter, pstStatus, szSelect);

		// Error
		(void)rest_bind_MakeErrorJsWriter(nJWriter, REST_ERR_CODE_OK, REST_ERR_STRING_OK);
	}
	HxJSONWRITER_ObjectEnd(nJWriter);

	szResultStr = rest_bind_CloseJsonWriter(nJWriter);
	nJWriter = 0;

END_FUNC:
	if (0 != nJWriter)
	{
		rest_bind_CloseJsonWriter(nJWriter);
	}

	if (NULL != pstStatus)
	{
		rest_bind_play_FreePlayerStatus(pstStatus);
	}

	if (NULL == szResultStr)
	{
		szResultStr = rest_bind_MakeErrorString(nErrCode, szErrString, TRUE);
	}

	return szResultStr;
}

#define _____COMMAND_FUNCTIONS_____
STATIC HERROR rest_bind_play_CmdRegisterNotifier (RestBind_Notifier_t pfNotifier)
{
	if (HLIB_LIST_Find(s_pstRestBindPlay_Notifier, (const void *)pfNotifier) != NULL)
	{
		HxLOG_Error("Already the same notifier exists:\n");
		return ERR_OK;
	}

	s_pstRestBindPlay_Notifier = HLIB_LIST_Append(s_pstRestBindPlay_Notifier, (void *)pfNotifier);
	return ERR_OK;
}

STATIC HERROR rest_bind_play_CbRegisterNotifier (void *pvArg)
{
	RestMsg_t					*pstMsg = (RestMsg_t *)pvArg;
	RestBind_Notifier_t			 pfNotifier = (RestBind_Notifier_t)pstMsg->aulArgs[0];
	HERROR						 hErr = ERR_FAIL;

	if (NULL != pfNotifier)
	{
		hErr = rest_bind_play_CmdRegisterNotifier(pfNotifier);
	}

	pstMsg->pvResult = (void *)hErr;
	rest_task_SyncOut();

	return ERR_OK;
}

STATIC HERROR rest_bind_play_CmdUnregisterNotifier (RestBind_Notifier_t pfNotifier)
{
	s_pstRestBindPlay_Notifier = HLIB_LIST_Remove(s_pstRestBindPlay_Notifier, (const void *)pfNotifier);
	return ERR_OK;
}

STATIC HERROR rest_bind_play_CbUnregisterNotifier (void *pvArg)
{
	RestMsg_t					*pstMsg = (RestMsg_t *)pvArg;
	RestBind_Notifier_t			 pfNotifier = (RestBind_Notifier_t)pstMsg->aulArgs[0];
	HERROR						 hErr = ERR_FAIL;

	if (NULL != pfNotifier)
	{
		hErr = rest_bind_play_CmdUnregisterNotifier(pfNotifier);
	}

	pstMsg->pvResult = (void *)hErr;
	rest_task_SyncOut();

	return ERR_OK;
}

STATIC HCHAR *rest_bind_play_CmdGetPlayersInfo (RestInstance_t *pstInst, HxJSON_t jsArgs)
{
	HINT32					 nJWriter = 0;
	HUINT32					 ulViewNum, ulMainViewId;
	HCHAR					*szResultStr = NULL;
	HERROR					 hErr;

	hErr = rest_bind_play_InitContext();
	if (ERR_OK != hErr)
	{
		HxLOG_Error("rest_bind_play_InitContext err:\n");
		return rest_bind_MakeErrorString(REST_ERR_CODE_NOTREADY, REST_ERR_STRING_NOTREADY, TRUE);
	}

	nJWriter = rest_bind_OpenJsonWriter();
	if (0 == nJWriter)
	{
		HxLOG_Error("rest_bind_OpenJsonWriter() err:\n");
		return rest_bind_MakeErrorString(REST_ERR_CODE_NOMEM, REST_ERR_STRING_NOMEM, TRUE);
	}

	hErr = APK_MEDIA_PLAY_GetViewNumber(&ulViewNum);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("APK_MEDIA_PLAY_GetViewNumber err:\n");
		ulViewNum = 0;
	}

	hErr = APK_MEDIA_PLAY_GetMainViewId(&ulMainViewId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("APK_MEDIA_PLAY_GetMainViewId err:\n");
		ulMainViewId = 0;
	}

	HxJSONWRITER_ObjectBegin(nJWriter);
	{
		// Players Overview
		HxJSONWRITER_Object(nJWriter, "playersOverview");
		{
			HxJSONWRITER_Integer(nJWriter, "numberOfPlayers", ulViewNum);
			if (ulViewNum > 0)
			{
				HxJSONWRITER_Integer(nJWriter, "mainPlayer", ulMainViewId);
			}
		}
		HxJSONWRITER_ObjectEnd(nJWriter);

		// Error
		(void)rest_bind_MakeErrorJsWriter(nJWriter, REST_ERR_CODE_OK, REST_ERR_STRING_OK);
	}
	HxJSONWRITER_ObjectEnd(nJWriter);

	szResultStr = rest_bind_CloseJsonWriter(nJWriter);

	return szResultStr;
}

STATIC HCHAR *rest_bind_play_CmdGetOnePlayerStatus (RestInstance_t *pstInst, HUINT32 ulPlayerId, HxJSON_t jsArgs)
{
	HINT32					 nJWriter = 0;
	HCHAR					*szResultStr = NULL;
	RestPlayerStatus_t		*pstStatus;
	HINT32					 nErrCode = REST_ERR_CODE_INTERNAL;
	HCHAR					*szErrString = REST_ERR_STRING_INTERNAL;

	pstStatus = rest_bind_play_GetPlayerStatus(ulPlayerId);
	if (NULL == pstStatus)
	{
		HxLOG_Error("Memory allocation failed:\n");
		nErrCode = REST_ERR_CODE_RESOURCE_NOTFOUND;		szErrString = REST_ERR_STRING_RESOURCE_NOTFOUND;
		goto END_FUNC;
	}

	nJWriter = rest_bind_OpenJsonWriter();
	if (0 == nJWriter)
	{
		HxLOG_Error("rest_bind_OpenJsonWriter() err:\n");
		nErrCode = REST_ERR_CODE_NOMEM;		szErrString = REST_ERR_STRING_NOMEM;
		goto END_FUNC;
	}

	// Jsonize the state
	HxJSONWRITER_ObjectBegin(nJWriter);
	{
		// Play Status
		(void)HxJSONWRITER_StringValue(nJWriter, "playStatus");
		(void)rest_datatype_JsonizePlayerStatus(nJWriter, pstStatus, NULL);

		// Error
		(void)rest_bind_MakeErrorJsWriter(nJWriter, REST_ERR_CODE_OK, REST_ERR_STRING_OK);
	}
	HxJSONWRITER_ObjectEnd(nJWriter);

	szResultStr = rest_bind_CloseJsonWriter(nJWriter);
	nJWriter = 0;

END_FUNC:
	if (0 != nJWriter)
	{
		rest_bind_CloseJsonWriter(nJWriter);
	}

	if (NULL != pstStatus)
	{
		rest_bind_play_FreePlayerStatus(pstStatus);
	}

	if (NULL == szResultStr)
	{
		szResultStr = rest_bind_MakeErrorString(nErrCode, szErrString, TRUE);
	}

	return szResultStr;
}

STATIC HCHAR *rest_bind_play_CmdGet (HCHAR *szSessionId, HINT32 nPlayerId, HxJSON_t jsArgs)
{
	RestInstance_t			*pstInst = rest_task_GetInstance(szSessionId);

	if (NULL == pstInst)
	{
		HxLOG_Error("rest_task_GetInstance err:\n");
		return rest_bind_MakeErrorString(REST_ERR_CODE_INVALID_SESSION, REST_ERR_STRING_INVALID_SESSION, TRUE);
	}

	// URL: http://127.0.0.1/stb-api/v1/players[?Options]
	if (nPlayerId < 0)
	{
		return rest_bind_play_CmdGetPlayersInfo(pstInst, jsArgs);
	}

	// URL: http://127.0.0.1/stb-api/v1/players/0[?Options]
	return rest_bind_play_CmdGetOnePlayerStatus(pstInst, (HUINT32)nPlayerId, jsArgs);
}

STATIC HERROR rest_bind_play_CbGet (void *pvArg)
{
	HINT32			 nPlayerId = -1;
	HCHAR			*szResult;
	HxJSON_t		 jsArgs = NULL;
	RestMsg_t		*pstMsg = (RestMsg_t *)pvArg;

	if (NULL == pstMsg)					{ return ERR_FAIL; }

	nPlayerId	= (HINT32)pstMsg->aulArgs[0];
	jsArgs = HLIB_JSON_Decode((const HCHAR *)pstMsg->aulArgs[1]);

	szResult = rest_bind_play_CmdGet(pstMsg->szSessionId, nPlayerId, jsArgs);
	pstMsg->pvResult = (void *)szResult;

	if (NULL != jsArgs)			{ HLIB_JSON_Delete(jsArgs); }
	rest_task_SyncOut();

	return ERR_OK;
}

#define _____PUT_FUNCTIONS_____
STATIC HCHAR *rest_bind_play_CmdExecPlay (RestInstance_t *pstInst, HUINT32 ulPlayerId, HxJSON_t *pjsPayload)
{
	STATIC OxMediaPlay_StartInfo_t	*s_pstStartInfo = NULL;
	HUINT32					 ulSessionId = 0;
	HCHAR					*szErrString = NULL;
	RestBindPlayerContext_t	*pstContext = rest_bind_play_GetContext(ulPlayerId);
	HERROR					 hErr;

	if (NULL == s_pstStartInfo)
	{
		s_pstStartInfo = (OxMediaPlay_StartInfo_t *)HLIB_STD_MemAlloc(sizeof(OxMediaPlay_StartInfo_t));
		if (NULL == s_pstStartInfo)
		{
			HxLOG_Error("s_pstStartInfo Memory allocation failed\n");
			return rest_bind_MakeErrorString(REST_ERR_CODE_NOMEM, REST_ERR_STRING_NODATA, TRUE);
		}
	}

	hErr = rest_bind_play_GetStartInfoFromPayload (pjsPayload, s_pstStartInfo, &szErrString);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("rest_bind_play_GetStartInfoFromPayload err:\n");
		return szErrString;
	}

	hErr = APK_MEDIA_PLAY_Start(ulPlayerId, s_pstStartInfo, &ulSessionId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("APK_MEDIA_PLAY_Start err:\n");
		return rest_bind_MakeErrorString(REST_ERR_CODE_CMD_FAILED, REST_ERR_STRING_CMD_FAILED, TRUE);
	}

	pstContext->ulSessionId = ulSessionId;

	return rest_bind_play_WriteCmdResponse(ulPlayerId, "viewId,sourceType,state,svcUid");
}

STATIC HCHAR *rest_bind_play_CmdExecStop (RestInstance_t *pstInst, HUINT32 ulPlayerId, HxJSON_t *pjsPayload)
{
	HERROR					 hErr;

	hErr = APK_MEDIA_PLAY_Stop(ulPlayerId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("APK_MEDIA_PLAY_Stop err:\n");
		return rest_bind_MakeErrorString(REST_ERR_CODE_CMD_FAILED, REST_ERR_STRING_CMD_FAILED, TRUE);
	}

	return rest_bind_play_WriteCmdResponse(ulPlayerId, "viewId,sourceType,state");
}

STATIC HCHAR *rest_bind_play_CmdPutOnePlayer (HCHAR *szSessionId, HUINT32 ulPlayerId, HCHAR *szJsPayload)
{
	HxJSON_t				*pjsPayload = NULL;
	RestInstance_t			*pstInst = rest_task_GetInstance(szSessionId);
	RestBindPlayerContext_t	*pstContext = rest_bind_play_GetContext(ulPlayerId);
	HCHAR					*szCmd = NULL;
	HCHAR					*szResult = NULL;
	HINT32					 nErrCode = REST_ERR_CODE_INTERNAL;
	HCHAR					*szErrString = REST_ERR_STRING_INTERNAL;

	if (NULL == pstInst)
	{
		HxLOG_Error("rest_task_GetInstance err:\n");
		nErrCode = REST_ERR_CODE_INVALID_SESSION;		szErrString = REST_ERR_STRING_INVALID_SESSION;
		goto END_FUNC;
	}

	if (NULL == pstContext)
	{
		HxLOG_Error("rest_bind_play_GetContext err:\n");
		nErrCode = REST_ERR_CODE_RESOURCE_NOTFOUND;			szErrString = REST_ERR_STRING_RESOURCE_NOTFOUND;
		goto END_FUNC;
	}

	if ((NULL == szJsPayload) || ('\0' == *szJsPayload))
	{
		HxLOG_Error("szJsPayload NULL\n");
		nErrCode = REST_ERR_CODE_REQUIRE_MANDATORYKEY;			szErrString = REST_ERR_STRING_REQUIRE_MANDATORYKEY;
		goto END_FUNC;
	}

	pjsPayload = HLIB_JSON_Decode((const HCHAR *)szJsPayload);
	if (NULL == pjsPayload)
	{
		HxLOG_Error("HLIB_JSON_Decode failed: String wrong?? [%s]\n", szJsPayload);
		nErrCode = REST_ERR_CODE_REQUIRE_MANDATORYKEY;			szErrString = REST_ERR_STRING_REQUIRE_MANDATORYKEY;
		goto END_FUNC;
	}

	szCmd = (HCHAR *)HLIB_JSON_Object_GetString(pjsPayload, "cmd");
	if (NULL == szCmd)
	{
		HxLOG_Error("HLIB_JSON_Object_GetString failed [cmd]: String wrong?? [%s]\n", szJsPayload);
		nErrCode = REST_ERR_CODE_REQUIRE_MANDATORYKEY;			szErrString = REST_ERR_STRING_REQUIRE_MANDATORYKEY;
		goto END_FUNC;
	}

	// Command [Play]
	if (HxSTD_StrCmp(szCmd, "play") == 0)
	{
		szResult = rest_bind_play_CmdExecPlay(pstInst, ulPlayerId, pjsPayload);
	}
	// Command [Stop]
	else if (HxSTD_StrCmp(szCmd, "stop") == 0)
	{
		szResult = rest_bind_play_CmdExecStop(pstInst, ulPlayerId, pjsPayload);
	}
	// Otherwise: Unknown Commands
	else
	{
		HxLOG_Error("Unknown Command [%s]\n", szCmd);
		nErrCode = REST_ERR_CODE_UNKNOWN_CMD;			szErrString = REST_ERR_STRING_UNKNOWN_CMD;
		goto END_FUNC;
	}

END_FUNC:
	if (NULL != pjsPayload)
	{
		HLIB_JSON_Delete(pjsPayload);
	}

	if (NULL == szResult)
	{
		szResult = rest_bind_MakeErrorString(nErrCode, szErrString, TRUE);
	}

	return szResult;
}

STATIC HERROR rest_bind_play_CbPutOnePlayer (void *pvArg)
{
	HUINT32			 ulPlayerId;
	HCHAR			*szJsPayload;
	HCHAR			*szResult;
	RestMsg_t		*pstMsg = (RestMsg_t *)pvArg;

	if (NULL == pstMsg)					{ return ERR_FAIL; }

	ulPlayerId	= (HUINT32)pstMsg->aulArgs[0];
	szJsPayload = (HCHAR *)pstMsg->aulArgs[1];

	szResult = rest_bind_play_CmdPutOnePlayer(pstMsg->szSessionId, ulPlayerId, szJsPayload);
	pstMsg->pvResult = (void *)szResult;

	rest_task_SyncOut();

	return ERR_OK;
}


#define ___MEMBER_FUNCTIONS___
HERROR rest_bind_play_Init (void)
{
	HERROR				 hErr;

	if (TRUE != s_stRestBindPlay_Inited)
	{
		// Initialize the APPKIT
		(void)APK_MEDIA_PLAY_Init();

		// Init 후 약간의 시간을 기다려야 Obama에서 제대로 된 정보를 주므로 wait 후 InitContext를 한다.
		hErr = rest_task_RequestToExecute(rest_bind_play_CbInitContext, (void *)NULL);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("rest_bind_play_CbInitContext failed:\n");
		}

		s_stRestBindPlay_Inited = TRUE;
	}

	return ERR_OK;
}

RestBindPlayerContext_t *rest_bind_play_GetContext (HUINT32 ulViewId)
{
	HERROR						 hErr;

	hErr = rest_bind_play_InitContext();
	if (ERR_OK != hErr)
	{
		HxLOG_Error("rest_bind_play_InitContext err:\n");
		return NULL;
	}

	if (ulViewId >= s_ulRestBindPlay_ContextNum)
	{
		HxLOG_Error("ulViewId(%d), s_ulRestBindPlay_ContextNum(%d)\n", ulViewId, s_ulRestBindPlay_ContextNum);
		return NULL;
	}

	return &(s_pstRestBindPlay_ContextArray[ulViewId]);
}

HERROR rest_bind_play_ResetComponent (RestPlayerComponent_t *pstComponent)
{
	OxMediaPlay_Component_e		 eCompType;

	if (NULL == pstComponent)					{ return ERR_FAIL; }


	if (NULL != pstComponent->pstCompArray)
	{
		HLIB_STD_MemFree(pstComponent->pstCompArray);
	}

	eCompType = pstComponent->eCompType;
	HxSTD_MemSet(pstComponent, 0, sizeof(RestPlayerComponent_t));
	pstComponent->eCompType = eCompType;
	pstComponent->nCurrIndex = -1;

	return ERR_OK;
}

HERROR rest_bind_play_GetMakeComponentsToStatus (RestPlayerStatus_t *pstStatus, OxMediaPlay_Component_e eCompType)
{
	HINT32						 nIndex;
	HINT32						 nCompIndex = -1, nCompNum = 0;
	RestPlayerComponent_t		*pstComponent;
	HERROR						 hErr;

	if ((NULL == pstStatus) || (eCompType >= eOxMP_COMPONENT_ENDOFCASE))
	{
		HxLOG_Error("argument wrong? pstStatus(%p), eCompType(%d)\n", pstStatus, eCompType);
		return ERR_FAIL;
	}

	pstComponent = &(pstStatus->astComponents[eCompType]);
	(void)rest_bind_play_ResetComponent(pstComponent);

	hErr  = APK_MEDIA_PLAY_GetComponentIndex(pstStatus->ulViewId, eCompType, &nCompIndex, NULL);
	hErr |= APK_MEDIA_PLAY_GetComponentNum(pstStatus->ulViewId, eCompType, (HUINT32 *)&nCompNum);
	if ((ERR_OK != hErr) || (nCompIndex < 0) || (nCompNum <= 0))
	{
		HxLOG_Info("No Component? eCompType(%d)", eCompType);
		return ERR_FAIL;
	}

	pstComponent->pstCompArray = (OxMediaPlay_Component_t *)HLIB_STD_MemCalloc(sizeof(OxMediaPlay_Component_t) * nCompNum);
	if (NULL == pstComponent->pstCompArray)
	{
		HxLOG_Error("pstCompArray allocation failed:\n");
		return ERR_FAIL;
	}

	for (nIndex = 0; nIndex < nCompNum; nIndex++)
	{
		(void)APK_MEDIA_PLAY_GetComponent(pstStatus->ulViewId, eCompType, nIndex, &(pstComponent->pstCompArray[nIndex]));
	}

	pstComponent->nCurrIndex = nCompIndex;
	pstComponent->nNumComponent = nCompNum;

	return ERR_OK;
}

void rest_bind_play_FreePlayerStatus (RestPlayerStatus_t *pstStatus)
{
	if (NULL != pstStatus)
	{
		HINT32			 nIndex;

		for (nIndex = 0; nIndex < eOxMP_COMPONENT_ENDOFCASE; nIndex++)
		{
			(void)rest_bind_play_ResetComponent(&(pstStatus->astComponents[nIndex]));
		}

		HLIB_STD_MemFree(pstStatus);
	}
}

RestPlayerStatus_t *rest_bind_play_GetPlayerStatus (HUINT32 ulPlayerId)
{
	OxMediaPlay_Component_e	 eCompType;
	RestBindPlayerContext_t	*pstContext;
	RestPlayerStatus_t		*pstStatus = NULL, *pstResult = NULL;
	HERROR					 hErr;

	pstStatus = (RestPlayerStatus_t *)HLIB_MEM_Calloc(sizeof(RestPlayerStatus_t));
	if (NULL == pstStatus)
	{
		HxLOG_Error("RestPlayerStatus_t allocation failed:\n");
		goto END_FUNC;
	}

	pstContext = rest_bind_play_GetContext(ulPlayerId);
	if (NULL == pstContext)
	{
		HxLOG_Error("rest_bind_play_GetContext err:\n");
		goto END_FUNC;
	}

	// Player ID
	pstStatus->ulViewId = ulPlayerId;

	// Play State
	hErr = APK_MEDIA_PLAY_GetPlayState(ulPlayerId, &(pstStatus->ePlayState));
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("APK_MEDIA_PLAY_GetPlayState err: ViewId(%d)\n", ulPlayerId);
	}

	// Media Type
	hErr = APK_MEDIA_PLAY_GetPlayType(ulPlayerId, &(pstStatus->unSource.eMediaType));
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("APK_MEDIA_PLAY_GetPlayState err: ViewId(%d)\n", ulPlayerId);
	}

	// Source
	switch (pstStatus->unSource.eMediaType)
	{
	case eOxMP_MEDIATYPE_LIVE:
	case eOxMP_MEDIATYPE_TSRPLAYBACK:
		hErr = APK_MEDIA_PLAY_GetRequestId(ulPlayerId, pstStatus->unSource.eMediaType, &(pstStatus->unSource.stLive.ulSvcUid));
		break;

	case eOxMP_MEDIATYPE_PVRPLAYBACK:
		hErr = APK_MEDIA_PLAY_GetRequestId(ulPlayerId, pstStatus->unSource.eMediaType, &(pstStatus->unSource.stPvrPb.ulContentsId));
		break;

	default:
		hErr = ERR_FAIL;
		break;
	}

	if (ERR_OK != hErr)
	{
		HxLOG_Warning("APK_MEDIA_PLAY_ Source Get err: ViewId(%d), MediaType(%d)\n", ulPlayerId, pstStatus->unSource.eMediaType);
	}

	// Play Speed
	hErr = APK_MEDIA_PLAY_GetPlaySpeed(ulPlayerId, &(pstStatus->nSpeed));
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("APK_MEDIA_PLAY_GetPlaySpeed err: ViewId(%d)\n", ulPlayerId);
	}

	pstStatus->nSpeed /= 100;

	// Start Time
	switch (pstStatus->unSource.eMediaType)
	{
	case eOxMP_MEDIATYPE_LIVE:
	case eOxMP_MEDIATYPE_TSRPLAYBACK:
		hErr = APK_MEDIA_PLAY_GetTsrStartTime(ulPlayerId, &(pstStatus->ulStartTime));
		break;

	default:
		hErr = ERR_OK;
		break;
	}

	if (ERR_OK != hErr)
	{
		HxLOG_Warning("APK_MEDIA_PLAY_ StartTime Get err: ViewId(%d), MediaType(%d)\n", ulPlayerId, pstStatus->unSource.eMediaType);
	}

	// Duration
	hErr = APK_MEDIA_PLAY_GetDurationTime(ulPlayerId, &(pstStatus->ulDuration));
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("APK_MEDIA_PLAY_GetDurationTime err: ViewId(%d)\n", ulPlayerId);
	}

	// Lock State
	hErr = APK_MEDIA_PLAY_GetLockState(ulPlayerId, &(pstStatus->eLockState));
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("APK_MEDIA_PLAY_GetLockState err: ViewId(%d)\n", ulPlayerId);
	}

	// Components
	for (eCompType = 0; eCompType < eOxMP_COMPONENT_ENDOFCASE; eCompType++)
	{
		(void)rest_bind_play_GetMakeComponentsToStatus(pstStatus, eCompType);
	}

	pstResult = pstStatus;
	pstStatus = NULL;

END_FUNC:
	if (NULL != pstStatus)
	{
		(void)rest_bind_play_FreePlayerStatus(pstStatus);
	}

	return pstResult;
}

#define _____NOTIFIERS_____
void rest_bind_play_NotifyApkMediaPlay (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eType, OxMediaPlay_Event_e eEvent, OxMediaPlay_NotifierData_t *pstNotifyData, void *pvUserData)
{
	HCHAR						*szPayload = NULL;
	RestBindPlayerContext_t		*pstContext = rest_bind_play_GetContext(ulViewId);

	if ((NULL != pstContext) && (NULL != s_pstRestBindPlay_Notifier))
	{
		switch (eType)
		{
		case eOxMP_MEDIATYPE_LIVE:
			szPayload = rest_bind_play_NotifyApkMediaPlay_Live(ulViewId, ulSessionId, eEvent, pstNotifyData);
			break;

		default:
			break;
		}
	}

	if (NULL != szPayload)
	{
		HxList_t					*pstListItem;

		for (pstListItem = s_pstRestBindPlay_Notifier; NULL != pstListItem; pstListItem = pstListItem->next)
		{
			RestBind_Notifier_t	pfNotifier = (RestBind_Notifier_t)HLIB_LIST_Data(pstListItem);

			if (NULL != pfNotifier)
			{
				pfNotifier("player", szPayload);
			}
		}

		rest_datatype_AppendStringBufferToTemporary(szPayload);
	}
}

#define ___API_FUNCTIONS___
int REST_BIND_PLAYER_RegisterNotifier (RestBind_Notifier_t pfNotifier)
{
	RestMsg_t				 stMsg;
	HERROR					 hErr;

	stMsg.pvResult		= (void *)0;
	stMsg.aulArgs[0]	= (HUINT32)pfNotifier;

	hErr = rest_task_RequestToExecute(rest_bind_play_CbRegisterNotifier, (void *)&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("rest_task_RequestToExecute err:\n");
		return ERR_FAIL;
	}

	rest_task_SyncIn();

	return (int)stMsg.pvResult;
}

int REST_BIND_PLAYER_UnregisterNotifier (RestBind_Notifier_t pfNotifier)
{
	RestMsg_t				 stMsg;
	HERROR					 hErr;

	stMsg.pvResult		= (void *)0;
	stMsg.aulArgs[0]	= (HUINT32)pfNotifier;

	hErr = rest_task_RequestToExecute(rest_bind_play_CbUnregisterNotifier, (void *)&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("rest_task_RequestToExecute err:\n");
		return ERR_FAIL;
	}

	rest_task_SyncIn();

	return (int)stMsg.pvResult;
}

// GET http://127.0.0.1:37564/stb-api/v1/players
char *REST_BIND_PLAYER_Get (char *szSessionId, char *szArgJson)
{
	RestMsg_t			 stMsg;
	char				*szResult = NULL;
	HERROR				 hErr;

	stMsg.szSessionId	= REST_DUMMY_SESSION_ID;
	stMsg.pvResult		= NULL;
	stMsg.aulArgs[0]	= (HUINT32)-1;
	stMsg.aulArgs[1]	= (HUINT32)szArgJson;

	hErr = rest_task_RequestToExecute(rest_bind_play_CbGet, (void *)&stMsg);
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

// GET http://127.0.0.1:37564/stb-api/v1/players/0
char *REST_BIND_PLAYER_GetOne (char *szSessionId, int nPlayerId, char *szArgJson)
{
	RestMsg_t			 stMsg;
	char				*szResult = NULL;
	HERROR				 hErr;

	stMsg.szSessionId	= REST_DUMMY_SESSION_ID;
	stMsg.pvResult		= NULL;
	stMsg.aulArgs[0]	= (HUINT32)nPlayerId;
	stMsg.aulArgs[1]	= (HUINT32)szArgJson;

	hErr = rest_task_RequestToExecute(rest_bind_play_CbGet, (void *)&stMsg);
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

char *REST_BIND_PLAYER_PutOne (char *szSessionId, int nPlayerId, char *szJsPayload)
{
	char					*szResult = NULL;
	RestMsg_t				 stMsg;
	HERROR					 hErr;

	stMsg.szSessionId	= REST_DUMMY_SESSION_ID;
	stMsg.pvResult		= NULL;
	stMsg.aulArgs[0]	= (HUINT32)nPlayerId;
	stMsg.aulArgs[1]	= (HUINT32)szJsPayload;

	hErr = rest_task_RequestToExecute(rest_bind_play_CbPutOnePlayer, (void *)&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("rest_task_RequestToExecute err:\n");
		goto ERROR;
	}

	rest_task_SyncIn();

	szResult = (char *)(stMsg.pvResult);
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

