/*******************************************************************
	File Description
********************************************************************/
/**
	@file	 rest_bind_common.c
	@brief

	Description:  									\n
	Module: node/binding/bind		\n

	Copyright (c) 2014 HUMAX Co., Ltd.				\n
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
	
} RestCommonContext_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HBOOL						 s_stRestBindCommon_Inited = FALSE;

#define ___LOCAL_FUNCTIONS___

#define _____COMMAND_FUNCTIONS_____

STATIC HCHAR *rest_bind_common_CmdGetSystemTime (HCHAR *szSelect)
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
		HxJSONWRITER_Object(nJsonWriter, "systemTime");
		{
			(void)rest_datatype_SetJsonInteger(nJsonWriter, "currentTime", HLIB_STD_GetSystemTime(), szSelect);
			(void)rest_datatype_SetJsonInteger(nJsonWriter, "currentTick", HLIB_STD_GetSystemTick(), szSelect);
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
STATIC HERROR rest_bind_common_time_CbGet (void *pvArg)
{
	HCHAR				*szMember = NULL;
	HCHAR				*szResult = NULL;
	RestMsg_t			*pstMsg = (RestMsg_t *)pvArg;
	HxJSON_t			 jsArgs;

	jsArgs = HLIB_JSON_Decode((HCHAR *)pstMsg->aulArgs[0]);
	if (NULL != jsArgs)
	{
		(void)rest_bind_GetStringValueFromJson(jsArgs, "member", &szMember);
	}

	szResult = rest_bind_common_CmdGetSystemTime(szMember);


	pstMsg->pvResult = (void *)szResult;
	rest_task_SyncOut();

	return ERR_OK;
}

#define ___MEMBER_FUNCTIONS___
HERROR rest_bind_common_Init (void)
{
	if (TRUE != s_stRestBindCommon_Inited)
	{
		s_stRestBindCommon_Inited = TRUE;
	}

	return ERR_OK;
}

#define ___API_FUNCTIONS___

char *REST_BIND_COMMON_TIME_Get (char *szSessionId, char *szArgJson)
{
	RestMsg_t			 stMsg;
	char				*szResult = NULL;
	HERROR				 hErr;

	stMsg.szSessionId	= REST_DUMMY_SESSION_ID;
	stMsg.pvResult		= NULL;
	stMsg.aulArgs[0]	= (HUINT32)szArgJson;
	
	hErr = rest_task_RequestToExecute(rest_bind_common_time_CbGet, (void *)&stMsg);
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

