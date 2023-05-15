/*******************************************************************
	File Description
********************************************************************/
/**
	@file	 rest_bind_scan_channel.c
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
#include <apk.h>
#include <rest_datatype.h>
#include <rest_maintask.h>
#include <rest_bind.h>

/*******************************************************************/
/********************      Local Definition         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HUINT32			 ulScannerId;
	
} RestChanScanContext_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HBOOL						 s_stRestBindScanChan_Inited = FALSE;
STATIC HxList_t						*s_pstRestBindScanChan_Notifier = NULL;

#define ___LOCAL_FUNCTIONS___

#define _____COMMAND_FUNCTIONS_____
STATIC HERROR rest_bind_scanch_CmdRegisterNotifier (RestBind_Notifier_t pfNotifier)
{
	if (HLIB_LIST_Find(s_pstRestBindScanChan_Notifier, (const void *)pfNotifier) != NULL)
	{
		HxLOG_Error("Already the same notifier exists:\n");
		return ERR_OK;
	}

	s_pstRestBindScanChan_Notifier = HLIB_LIST_Append(s_pstRestBindScanChan_Notifier, (void *)pfNotifier);
	return ERR_OK;
}

STATIC HERROR rest_bind_scanch_CbRegisterNotifier (void *pvArg)
{
	RestMsg_t					*pstMsg = (RestMsg_t *)pvArg;
	RestBind_Notifier_t	 pfNotifier = (RestBind_Notifier_t)pstMsg->aulArgs[0];
	HERROR						 hErr = ERR_FAIL;

	if (NULL != pfNotifier)
	{
		hErr = rest_bind_scanch_CmdRegisterNotifier(pfNotifier);
	}

	pstMsg->pvResult = (void *)hErr;
	rest_task_SyncOut();

	return ERR_OK;
}

STATIC HERROR rest_bind_scanch_CmdUnregisterNotifier (RestBind_Notifier_t pfNotifier)
{
	s_pstRestBindScanChan_Notifier = HLIB_LIST_Remove(s_pstRestBindScanChan_Notifier, (const void *)pfNotifier);
	return ERR_OK;
}

STATIC HERROR rest_bind_scanch_CbUnregisterNotifier (void *pvArg)
{
	RestMsg_t					*pstMsg = (RestMsg_t *)pvArg;
	RestBind_Notifier_t	 pfNotifier = (RestBind_Notifier_t)pstMsg->aulArgs[0];
	HERROR						 hErr = ERR_FAIL;

	if (NULL != pfNotifier)
	{
		hErr = rest_bind_scanch_CmdUnregisterNotifier(pfNotifier);
	}

	pstMsg->pvResult = (void *)hErr;
	rest_task_SyncOut();

	return ERR_OK;
}

STATIC HCHAR *rest_bind_scanch_CmdGetScannerNumbers (HCHAR *szSessionId)
{
	HINT32					 nJWriter = 0;
	HCHAR					*szResultStr = NULL;
	RestInstance_t			*pstInst;
	
	pstInst = rest_task_GetInstance(szSessionId);
	if (NULL == pstInst)
	{
		HxLOG_Error("rest_task_GetInstance err:\n");
		return rest_bind_MakeErrorString(REST_ERR_CODE_INVALID_SESSION, REST_ERR_STRING_INVALID_SESSION, TRUE);
	}

	nJWriter = rest_bind_OpenJsonWriter();
	if (0 == nJWriter)
	{
		HxLOG_Error("rest_bind_OpenJsonWriter() err:\n");
		return rest_bind_MakeErrorString(REST_ERR_CODE_NOMEM, REST_ERR_STRING_NOMEM, TRUE);
	}

	HxJSONWRITER_ObjectBegin(nJWriter);
	{
		HxJSONWRITER_Object(nJWriter, "channerScannerOverview");
		{
			HxJSONWRITER_Integer(nJWriter, "numberOfScanners", REST_CHANSCAN_INST_NUM);
		}
		HxJSONWRITER_ObjectEnd(nJWriter);
	}
	HxJSONWRITER_ObjectEnd(nJWriter);

	szResultStr = rest_bind_CloseJsonWriter(nJWriter);

	return szResultStr;
}

STATIC HERROR rest_bind_scanch_CbGetScannerNumbers (void *pvArg)
{
	RestMsg_t		*pstMsg = (RestMsg_t *)pvArg;
	HCHAR			*szResult;

	if (NULL == pstMsg)					{ return ERR_FAIL; }
	
	szResult = rest_bind_scanch_CmdGetScannerNumbers(pstMsg->szSessionId);
	pstMsg->pvResult = (void *)szResult;

	rest_task_SyncOut();

	return ERR_OK;
}

STATIC HCHAR *rest_bind_scanch_CmdGetScannerStatus (HCHAR *szSessionId, HUINT32 ulScannerId)
{
	HINT32					 nJWriter = 0;
	HCHAR					*szResultStr = NULL;
	RestInstance_t			*pstInst;
	
	pstInst = rest_task_GetInstance(szSessionId);
	if (NULL == pstInst)
	{
		HxLOG_Error("rest_task_GetInstance err:\n");
		return rest_bind_MakeErrorString(REST_ERR_CODE_INVALID_SESSION, REST_ERR_STRING_INVALID_SESSION, TRUE);
	}

	nJWriter = rest_bind_OpenJsonWriter();
	if (0 == nJWriter)
	{
		HxLOG_Error("rest_bind_OpenJsonWriter() err:\n");
		return rest_bind_MakeErrorString(REST_ERR_CODE_NOMEM, REST_ERR_STRING_NOMEM, TRUE);
	}

	HxJSONWRITER_ObjectBegin(nJWriter);
	{
		HxJSONWRITER_Object(nJWriter, "channerScannerOverview");
		{
			HxJSONWRITER_Integer(nJWriter, "numberOfScanners", REST_CHANSCAN_INST_NUM);
		}
		HxJSONWRITER_ObjectEnd(nJWriter);
	}
	HxJSONWRITER_ObjectEnd(nJWriter);

	szResultStr = rest_bind_CloseJsonWriter(nJWriter);

	return szResultStr;
}

STATIC HERROR rest_bind_scanch_CbGetScannerStatus (void *pvArg)
{
	HUINT32			 ulScannerId;
	RestMsg_t		*pstMsg = (RestMsg_t *)pvArg;
	HCHAR			*szResult;

	if (NULL == pstMsg)					{ return ERR_FAIL; }

	ulScannerId = pstMsg->aulArgs[0];
	
	szResult = rest_bind_scanch_CmdGetScannerStatus(pstMsg->szSessionId, ulScannerId);
	pstMsg->pvResult = (void *)szResult;

	rest_task_SyncOut();

	return ERR_OK;
}

#define ___MEMBER_FUNCTIONS___
HERROR rest_bind_scanch_Init (void)
{
	if (TRUE != s_stRestBindScanChan_Inited)
	{
		// Initialize the APPKIT
		(void)APK_BUILDER_Init();
		
		s_stRestBindScanChan_Inited = TRUE;
	}

	return ERR_OK;
}

#define ___API_FUNCTIONS___
int REST_BIND_SCANCHAN_RegisterNotifier (RestBind_Notifier_t pfNotifier)
{
	RestMsg_t				 stMsg;
	HERROR					 hErr;

	stMsg.pvResult		= (void *)0;
	stMsg.aulArgs[0]	= (HUINT32)pfNotifier;

	hErr = rest_task_RequestToExecute(rest_bind_scanch_CbRegisterNotifier, (void *)&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("rest_task_RequestToExecute err:\n");
		return ERR_FAIL;
	}

	rest_task_SyncIn();

	return (int)stMsg.pvResult;
}

int REST_BIND_SCANCHAN_UnregisterNotifier (RestBind_Notifier_t pfNotifier)
{
	RestMsg_t				 stMsg;
	HERROR					 hErr;

	stMsg.pvResult		= (void *)0;
	stMsg.aulArgs[0]	= (HUINT32)pfNotifier;
	
	hErr = rest_task_RequestToExecute(rest_bind_scanch_CbUnregisterNotifier, (void *)&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("rest_task_RequestToExecute err:\n");
		return ERR_FAIL;
	}

	rest_task_SyncIn();

	return (int)stMsg.pvResult;
}

char *REST_BIND_SCANCHAN_GetScannerNumbers (char *szSessionId)
{
	char					*szResult = NULL;
	RestMsg_t				 stMsg;
	HERROR					 hErr;

	stMsg.szSessionId	= REST_DUMMY_SESSION_ID;
	stMsg.pvResult		= NULL;
	
	hErr = rest_task_RequestToExecute(rest_bind_scanch_CbGetScannerNumbers, (void *)&stMsg);
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

	rest_bind_AppendStringBufferToTemporary(szResult);
	return szResult;

ERROR:
	return "";
}

char *REST_BIND_SCANCHAN_GetScannerStatus (char *szSessionId, int nScannerId)
{
	char					*szResult = NULL;
	RestMsg_t				 stMsg;
	HERROR					 hErr;

	stMsg.szSessionId	= REST_DUMMY_SESSION_ID;
	stMsg.pvResult		= NULL;
	stMsg.aulArgs[0]	= (HUINT32)nScannerId;

	hErr = rest_task_RequestToExecute(rest_bind_scanch_CbGetScannerStatus, (void *)&stMsg);
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

	rest_bind_AppendStringBufferToTemporary(szResult);
	return szResult;

ERROR:
	return "";
}



