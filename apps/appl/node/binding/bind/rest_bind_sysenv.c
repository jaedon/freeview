/*******************************************************************
	File Description
********************************************************************/
/**
	@file	 rest_bind_sysenv.c
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
#include <rest_maintask.h>
#include <rest_datatype.h>
#include <rest_bind.h>

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
STATIC HBOOL						 s_bRestBindSysEnv_Inited = FALSE;
STATIC HxList_t						*s_pstRestBindSysEnv_Notifier = NULL;

#define ___LOCAL_FUNCTIONS___
STATIC HERROR rest_bind_sysenv_ReadSystemEnv (RestSysEnv_t *pstSysEnv)
{
	if (NULL == pstSysEnv)				{ return ERR_FAIL; }

	(void)rest_datatype_CleanSysEnv(pstSysEnv);

	(void)APK_SCENARIO_GetMainLanguage(pstSysEnv->szMainLangCode);
	(void)APK_SCENARIO_GetAudioLanguage(0, pstSysEnv->szAudioLangCode);
	(void)APK_SCENARIO_GetSubtitleLanguage(0, pstSysEnv->szSubtitleLangCode);
	(void)APK_SCENARIO_GetHardOfHearing((HBOOL *)&(pstSysEnv->nHardOfHearing));
	(void)APK_SCENARIO_GetMaturityRating((DxRatingAge_e *)&(pstSysEnv->nMaturityRating));
	(void)APK_SCENARIO_GetCountryCode(&(pstSysEnv->eCountry));
	(void)APK_SCENARIO_GetOperatorType(&(pstSysEnv->eOperater));
	(void)APK_SCENARIO_GetAntennaConnection((DxAntennaConnection_e *)&(pstSysEnv->nAntConnectType));
	(void)APK_SCENARIO_GetStartPadding(&(pstSysEnv->nStartPaddingTime));
	(void)APK_SCENARIO_GetEndPadding(&(pstSysEnv->nEndPaddingTime));
	(void)APK_SCENARIO_GetAutoDelete((HBOOL *)&(pstSysEnv->nAutoDelete));
	(void)APK_SCENARIO_GetGMTOffset((HBOOL *)&(pstSysEnv->nGmtOffset));

	return ERR_OK;
}

STATIC HERROR rest_bind_sysenv_CheckAndSetSystemEnv (HxJSON_t jsPayLoad, RestSysEnv_t *pstResult)
{
	RestSysEnv_t			*pstOrgSysEnv = NULL, *pstNewSysEnv = NULL;

	if (NULL == pstResult)					{ return ERR_FAIL; }

	(void)rest_datatype_CleanSysEnv(pstResult);
	
	pstOrgSysEnv = (RestSysEnv_t *)HLIB_STD_MemAlloc(sizeof(RestSysEnv_t));
	pstNewSysEnv = (RestSysEnv_t *)HLIB_STD_MemAlloc(sizeof(RestSysEnv_t));
	if ((NULL == pstOrgSysEnv) || (NULL == pstNewSysEnv))
	{
		HxLOG_Error("memory allocation failed:\n");
		goto END_FUNC;
	}
	
	(void)rest_bind_sysenv_ReadSystemEnv(pstOrgSysEnv);
	HxSTD_MemCopy(pstNewSysEnv, pstOrgSysEnv, sizeof(RestSysEnv_t));
	(void)rest_datatype_DejsonizeSysEnv(jsPayLoad, pstNewSysEnv, FALSE);

	// Compare and set:
	if (HxSTD_StrNCmp(pstOrgSysEnv->szMainLangCode, pstNewSysEnv->szMainLangCode, 4) != 0)
	{
		(void)APK_SCENARIO_SetMainLanguage((const HCHAR *)pstNewSysEnv->szMainLangCode);
		HxSTD_StrNCpy(pstResult->szMainLangCode, pstNewSysEnv->szMainLangCode, 4);
	}
	
	if (HxSTD_StrNCmp(pstOrgSysEnv->szAudioLangCode, pstNewSysEnv->szAudioLangCode, 4) != 0)
	{
		(void)APK_SCENARIO_SetAudioLanguage(0, (const HCHAR *)pstNewSysEnv->szAudioLangCode);
		HxSTD_StrNCpy(pstResult->szAudioLangCode, pstNewSysEnv->szAudioLangCode, 4);
	}
	
	if (HxSTD_StrNCmp(pstOrgSysEnv->szSubtitleLangCode, pstNewSysEnv->szSubtitleLangCode, 4) != 0)
	{
		(void)APK_SCENARIO_SetSubtitleLanguage(0, (const HCHAR *)pstNewSysEnv->szSubtitleLangCode);
		HxSTD_StrNCpy(pstResult->szSubtitleLangCode, pstNewSysEnv->szSubtitleLangCode, 4);
	}

	if (pstOrgSysEnv->nHardOfHearing != pstNewSysEnv->nHardOfHearing)
	{
		(void)APK_SCENARIO_SetHardOfHearing((HBOOL)pstNewSysEnv->nHardOfHearing);
		pstResult->nHardOfHearing = pstNewSysEnv->nHardOfHearing;
	}

	if (pstOrgSysEnv->nMaturityRating != pstNewSysEnv->nMaturityRating)
	{
		(void)APK_SCENARIO_SetMaturityRating((DxRatingAge_e)pstNewSysEnv->nMaturityRating);
		pstResult->nMaturityRating = pstNewSysEnv->nMaturityRating;
	}

	if (pstOrgSysEnv->eCountry != pstNewSysEnv->eCountry)
	{
		(void)APK_SCENARIO_SetCountryCode(pstNewSysEnv->eCountry);
		pstResult->eCountry = pstNewSysEnv->eCountry;
	}

	if (pstOrgSysEnv->eOperater != pstNewSysEnv->eOperater)
	{
		(void)APK_SCENARIO_SetOperatorType(pstNewSysEnv->eOperater);
		pstResult->eOperater = pstNewSysEnv->eOperater;
	}

	if (pstOrgSysEnv->nAntConnectType != pstNewSysEnv->nAntConnectType)
	{
		(void)APK_SCENARIO_SetAntennaConnection((DxAntennaConnection_e)pstNewSysEnv->nAntConnectType);
		pstResult->nAntConnectType = pstNewSysEnv->nAntConnectType;
	}

	if (pstOrgSysEnv->nStartPaddingTime != pstNewSysEnv->nStartPaddingTime)
	{
		(void)APK_SCENARIO_SetStartPadding(pstNewSysEnv->nStartPaddingTime);
		pstResult->nStartPaddingTime = pstNewSysEnv->nStartPaddingTime;
	}

	if (pstOrgSysEnv->nEndPaddingTime != pstNewSysEnv->nEndPaddingTime)
	{
		(void)APK_SCENARIO_SetEndPadding(pstNewSysEnv->nEndPaddingTime);
		pstResult->nEndPaddingTime = pstNewSysEnv->nEndPaddingTime;
	}
	
	if (pstOrgSysEnv->nAutoDelete != pstNewSysEnv->nAutoDelete)
	{
		(void)APK_SCENARIO_SetAutoDelete((HBOOL)pstNewSysEnv->nAutoDelete);
		pstResult->nAutoDelete = pstNewSysEnv->nAutoDelete;
	}
	
	if (pstOrgSysEnv->nGmtOffset != pstNewSysEnv->nGmtOffset)
	{
		(void)APK_SCENARIO_SetGMTOffset(pstNewSysEnv->nGmtOffset);
		pstResult->nGmtOffset = pstNewSysEnv->nGmtOffset;
	}

END_FUNC:
	if (NULL != pstOrgSysEnv)				{ HLIB_STD_MemFree(pstOrgSysEnv); }
	if (NULL != pstNewSysEnv)				{ HLIB_STD_MemFree(pstNewSysEnv); }

	return ERR_OK;
}


#define _____COMMAND_FUNCTIONS_____
STATIC HERROR rest_bind_sysenv_CmdRegisterNotifier (RestBind_Notifier_t pfNotifier)
{
	if (HLIB_LIST_Find(s_pstRestBindSysEnv_Notifier, (const void *)pfNotifier) != NULL)
	{
		HxLOG_Error("Already the same notifier exists:\n");
		return ERR_OK;
	}

	s_pstRestBindSysEnv_Notifier = HLIB_LIST_Append(s_pstRestBindSysEnv_Notifier, (void *)pfNotifier);
	return ERR_OK;
}

STATIC HERROR rest_bind_sysenv_CbRegisterNotifier (void *pvArg)
{
	RestMsg_t			*pstMsg = (RestMsg_t *)pvArg;
	RestBind_Notifier_t	 pfNotifier = (RestBind_Notifier_t)pstMsg->aulArgs[0];
	HERROR				 hErr = ERR_FAIL;

	if (NULL != pfNotifier)
	{
		hErr = rest_bind_sysenv_CmdRegisterNotifier(pfNotifier);
	}

	pstMsg->pvResult = (void *)hErr;
	rest_task_SyncOut();

	return ERR_OK;
}

STATIC HERROR rest_bind_sysenv_CmdUnregisterNotifier (RestBind_Notifier_t pfNotifier)
{
	s_pstRestBindSysEnv_Notifier = HLIB_LIST_Remove(s_pstRestBindSysEnv_Notifier, (const void *)pfNotifier);
	return ERR_OK;
}

STATIC HERROR rest_bind_sysenv_CbUnregisterNotifier (void *pvArg)
{
	RestMsg_t			*pstMsg = (RestMsg_t *)pvArg;
	RestBind_Notifier_t	 pfNotifier = (RestBind_Notifier_t)pstMsg->aulArgs[0];
	HERROR				 hErr = ERR_FAIL;

	if (NULL != pfNotifier)
	{
		hErr = rest_bind_sysenv_CmdUnregisterNotifier(pfNotifier);
	}

	pstMsg->pvResult = (void *)hErr;
	rest_task_SyncOut();

	return ERR_OK;
}

STATIC HCHAR *rest_bind_sysenv_CmdGet (HCHAR *szSessionId, HxJSON_t jsArgs)
{
	HINT32					 nJsonWriter = rest_bind_OpenJsonWriter();
	HCHAR					*szMember = NULL;
	RestInstance_t			*pstInst = rest_task_GetInstance(szSessionId);
	RestSysEnv_t			*pstSysEnv = NULL;
	HINT32					 nErrCode = REST_ERR_CODE_OK;
	HCHAR					*szErrString = REST_ERR_STRING_OK;
	HCHAR					*szResult = NULL;

	if (0 == nJsonWriter)
	{
		HxLOG_Error("rest_bind_OpenJsonWriter failed:\n");
		nErrCode = REST_ERR_CODE_NOMEM;				szErrString = REST_ERR_STRING_NOMEM;
		goto END_FUNC;
	}

	if (NULL == pstInst)
	{
		HxLOG_Error("rest_task_GetInstance err:\n");
		nErrCode = REST_ERR_CODE_INVALID_SESSION;		szErrString = REST_ERR_STRING_INVALID_SESSION;
		goto END_FUNC;
	}

	pstSysEnv = (RestSysEnv_t *)HLIB_STD_MemAlloc(sizeof(RestSysEnv_t));
	if (NULL == pstSysEnv)
	{
		HxLOG_Error("memory allocation failed:\n");
		nErrCode = REST_ERR_CODE_NOMEM;				szErrString = REST_ERR_STRING_NOMEM;
		goto END_FUNC;
	}

	if (NULL != jsArgs)
	{
		szMember = (HCHAR *)HLIB_JSON_Object_GetString(jsArgs, "member");
	}
	
	(void)rest_bind_sysenv_ReadSystemEnv(pstSysEnv);

	HxJSONWRITER_ObjectBegin(nJsonWriter);
	{
		// System Environment
		rest_datatype_JsonizeSysEnv(nJsonWriter, pstSysEnv, "systemEnvironment", szMember);
		
		// Error
		(void)rest_bind_MakeErrorJsWriter(nJsonWriter, REST_ERR_CODE_OK, REST_ERR_STRING_OK);
	}
	HxJSONWRITER_ObjectEnd(nJsonWriter);
	
	szResult = rest_bind_CloseJsonWriter(nJsonWriter);

END_FUNC:
	if (NULL != pstSysEnv)					{ HLIB_STD_MemFree(pstSysEnv); }
	if (NULL == szResult)
	{
		szResult = rest_bind_MakeErrorString(nErrCode, szErrString, TRUE);
	}

	return szResult;
}

STATIC HCHAR *rest_bind_sysenv_CmdPut (HCHAR *szSessionId, HxJSON_t jsPayLoad)
{
	HINT32					 nJsonWriter = rest_bind_OpenJsonWriter();
	RestInstance_t			*pstInst = rest_task_GetInstance(szSessionId);
	RestSysEnv_t			*pstSysEnv = NULL;
	HINT32					 nErrCode = REST_ERR_CODE_OK;
	HCHAR					*szErrString = REST_ERR_STRING_OK;
	HCHAR					*szResult = NULL;

	if (0 == nJsonWriter)
	{
		HxLOG_Error("rest_bind_OpenJsonWriter failed:\n");
		nErrCode = REST_ERR_CODE_NOMEM;				szErrString = REST_ERR_STRING_NOMEM;
		goto END_FUNC;
	}

	if (NULL == pstInst)
	{
		HxLOG_Error("rest_task_GetInstance err:\n");
		nErrCode = REST_ERR_CODE_INVALID_SESSION;		szErrString = REST_ERR_STRING_INVALID_SESSION;
		goto END_FUNC;
	}

	pstSysEnv = (RestSysEnv_t *)HLIB_STD_MemAlloc(sizeof(RestSysEnv_t));
	if (NULL == pstSysEnv)
	{
		HxLOG_Error("memory allocation failed:\n");
		nErrCode = REST_ERR_CODE_NOMEM;				szErrString = REST_ERR_STRING_NOMEM;
		goto END_FUNC;
	}

	(void)rest_bind_sysenv_CheckAndSetSystemEnv(jsPayLoad, pstSysEnv);

	HxJSONWRITER_ObjectBegin(nJsonWriter);
	{
		// System Environment
		rest_datatype_JsonizeSysEnv(nJsonWriter, pstSysEnv, "systemEnvironment", NULL);
		
		// Error
		(void)rest_bind_MakeErrorJsWriter(nJsonWriter, REST_ERR_CODE_OK, REST_ERR_STRING_OK);
	}
	HxJSONWRITER_ObjectEnd(nJsonWriter);
	
	szResult = rest_bind_CloseJsonWriter(nJsonWriter);

END_FUNC:
	if (NULL != pstSysEnv)					{ HLIB_STD_MemFree(pstSysEnv); }
	if (NULL == szResult)
	{
		szResult = rest_bind_MakeErrorString(nErrCode, szErrString, TRUE);
	}

	return szResult;
}

STATIC HERROR rest_bind_sysenv_CbGet (void *pvArg)
{
	RestMsg_t			*pstMsg = (RestMsg_t *)pvArg;
	HxJSON_t			 jsArgs;
	HCHAR				*szResult;

	jsArgs = HLIB_JSON_Decode((const HCHAR *)pstMsg->aulArgs[0]);
	szResult = rest_bind_sysenv_CmdGet(pstMsg->szSessionId, jsArgs);
	pstMsg->pvResult = (void *)szResult;

	if (NULL != jsArgs)				{ HLIB_JSON_Delete(jsArgs); }
	rest_task_SyncOut();

	return ERR_OK;
}

STATIC HERROR rest_bind_sysenv_CbPut (void *pvArg)
{
	RestMsg_t			*pstMsg = (RestMsg_t *)pvArg;
	HxJSON_t			 jsPayload;
	HCHAR				*szResult;

	jsPayload = HLIB_JSON_Decode((const HCHAR *)pstMsg->aulArgs[0]);
	szResult = rest_bind_sysenv_CmdPut(pstMsg->szSessionId, jsPayload);
	pstMsg->pvResult = (void *)szResult;

	if (NULL != jsPayload)			{ HLIB_JSON_Delete(jsPayload); }
	rest_task_SyncOut();

	return ERR_OK;
}

#define ___MEMBER_FUNCTIONS___
HERROR rest_bind_sysenv_Init (void)
{
	if (TRUE != s_bRestBindSysEnv_Inited)
	{
		// Initialize the APPKIT
		(void)APK_SCENARIO_Init();

		s_bRestBindSysEnv_Inited = TRUE;
	}

	return ERR_OK;
}

#define ___API_FUNCTIONS___
int REST_BIND_SYSENV_RegisterNotifier (RestBind_Notifier_t pfNotifier)
{
	RestMsg_t				 stMsg;
	HERROR					 hErr;

	stMsg.pvResult		= (void *)0;
	stMsg.aulArgs[0]	= (HUINT32)pfNotifier;

	hErr = rest_task_RequestToExecute(rest_bind_sysenv_CbRegisterNotifier, (void *)&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("rest_task_RequestToExecute err:\n");
		return ERR_FAIL;
	}

	rest_task_SyncIn();

	return (int)stMsg.pvResult;
}

int REST_BIND_SYSENV_UnregisterNotifier (RestBind_Notifier_t pfNotifier)
{
	RestMsg_t				 stMsg;
	HERROR					 hErr;

	stMsg.pvResult		= (void *)0;
	stMsg.aulArgs[0]	= (HUINT32)pfNotifier;
	
	hErr = rest_task_RequestToExecute(rest_bind_sysenv_CbUnregisterNotifier, (void *)&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("rest_task_RequestToExecute err:\n");
		return ERR_FAIL;
	}

	rest_task_SyncIn();

	return (int)stMsg.pvResult;
}

// GET http://127.0.0.1:37564/stb-api/v1/system
char *REST_BIND_SYSENV_Get (char *szSessionId, char *szArgJson)
{
	RestMsg_t			 stMsg;
	char				*szResult = NULL;
	HERROR				 hErr;

	stMsg.szSessionId	= REST_DUMMY_SESSION_ID;
	stMsg.pvResult		= NULL;
	stMsg.aulArgs[0]	= (HUINT32)szArgJson;
	
	hErr = rest_task_RequestToExecute(rest_bind_sysenv_CbGet, (void *)&stMsg);
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

// PUT http://127.0.0.1:37564/stb-api/v1/system
char *REST_BIND_SYSENV_Put (char *szSessionId, char *szPayload)
{
	RestMsg_t			 stMsg;
	char				*szResult = NULL;
	HERROR				 hErr;

	stMsg.szSessionId	= REST_DUMMY_SESSION_ID;
	stMsg.pvResult		= NULL;
	stMsg.aulArgs[0]	= (HUINT32)szPayload;
	
	hErr = rest_task_RequestToExecute(rest_bind_sysenv_CbPut, (void *)&stMsg);
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



