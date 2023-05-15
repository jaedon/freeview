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
#define REST_BIND_SCANNER_STATIC_INST_ID					0

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
STATIC HBOOL						 s_stRestBindScan_Inited = FALSE;
STATIC HxList_t						*s_pstRestBindScan_Notifier = NULL;

STATIC HxList_t						*s_pstRestBindScan_SessionList = NULL;

#define ___LOCAL_FUNCTIONS___
STATIC RestScanParams_t *rest_bind_scan_GetScannerSession (HUINT32 ulSessionUid)
{
	HxList_t			*pstListItem;

	for (pstListItem = s_pstRestBindScan_SessionList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		RestScanParams_t		*pstParams = (RestScanParams_t *)HLIB_LIST_Data(pstListItem);
	
		if ((NULL != pstParams) && (ulSessionUid == pstParams->stBase.ulScannerUid))
		{
			return pstParams;
		}
	}

	return NULL;	
}

STATIC HUINT32 rest_bind_scan_MakeNewSessionUid (void)
{
	STATIC HUINT32		 s_ulLastSessionUid = 0;
	HUINT32				 ulSessionUid = s_ulLastSessionUid;

	do 
	{
		ulSessionUid ++;
		if (ulSessionUid >= 0xFFFFFFF0)
		{
			ulSessionUid = 1;
		}

		if (NULL == rest_bind_scan_GetScannerSession(ulSessionUid))
		{
			s_ulLastSessionUid = ulSessionUid;
			return ulSessionUid;
		}
		
	} while (ulSessionUid != s_ulLastSessionUid);

	return 0;	
}

STATIC RestScanParams_t *rest_bind_scan_CreateScannerSession (HUINT32 ulSessionUid)
{
	RestScanParams_t	*pstParam = (RestScanParams_t *)HLIB_STD_MemCalloc(sizeof(RestScanParams_t));

	if (NULL == pstParam)
	{
		HxLOG_Error("RestScanParams_t allocation failed:\n");
		return NULL;
	}

	pstParam->stBase.ulScannerUid = ulSessionUid;
	pstParam->stBase.eState = eREST_SCAN_STATE_Idle;

	s_pstRestBindScan_SessionList = HLIB_LIST_Append(s_pstRestBindScan_SessionList, (void *)pstParam);
	return pstParam;
}

STATIC HCHAR *rest_bind_WriteResultStatus (RestScanParams_t *pstParams, HUINT32 ulErrCode, HCHAR *szErrString)
{
	HINT32				 nJWriter;

	nJWriter = rest_bind_OpenJsonWriter();
	if (0 == nJWriter)
	{
		HxLOG_Error("rest_bind_OpenJsonWriter() err:\n");
		return rest_bind_MakeErrorString(REST_ERR_CODE_NOMEM, REST_ERR_STRING_NOMEM, TRUE);
	}

	HxJSONWRITER_ObjectBegin(nJWriter);
	{
		HxJSONWRITER_StringValue(nJWriter, "scannerStatus");
		(void)rest_datatype_JsonizeScanParams(nJWriter, pstParams, NULL);

		(void)rest_bind_MakeErrorJsWriter(nJWriter, ulErrCode, szErrString);
	}
	HxJSONWRITER_ObjectEnd(nJWriter);

	return rest_bind_CloseJsonWriter(nJWriter);
}

#define _____COMMAND_FUNCTIONS_____
STATIC HERROR rest_bind_scan_CmdRegisterNotifier (RestBind_Notifier_t pfNotifier)
{
	if (HLIB_LIST_Find(s_pstRestBindScan_Notifier, (const void *)pfNotifier) != NULL)
	{
		HxLOG_Error("Already the same notifier exists:\n");
		return ERR_OK;
	}

	s_pstRestBindScan_Notifier = HLIB_LIST_Append(s_pstRestBindScan_Notifier, (void *)pfNotifier);
	return ERR_OK;
}

STATIC HERROR rest_bind_scan_CbRegisterNotifier (void *pvArg)
{
	RestMsg_t					*pstMsg = (RestMsg_t *)pvArg;
	RestBind_Notifier_t	 pfNotifier = (RestBind_Notifier_t)pstMsg->aulArgs[0];
	HERROR						 hErr = ERR_FAIL;

	if (NULL != pfNotifier)
	{
		hErr = rest_bind_scan_CmdRegisterNotifier(pfNotifier);
	}

	pstMsg->pvResult = (void *)hErr;
	rest_task_SyncOut();

	return ERR_OK;
}

STATIC HERROR rest_bind_scan_CmdUnregisterNotifier (RestBind_Notifier_t pfNotifier)
{
	s_pstRestBindScan_Notifier = HLIB_LIST_Remove(s_pstRestBindScan_Notifier, (const void *)pfNotifier);
	return ERR_OK;
}

STATIC HERROR rest_bind_scan_CbUnregisterNotifier (void *pvArg)
{
	RestMsg_t					*pstMsg = (RestMsg_t *)pvArg;
	RestBind_Notifier_t	 pfNotifier = (RestBind_Notifier_t)pstMsg->aulArgs[0];
	HERROR						 hErr = ERR_FAIL;

	if (NULL != pfNotifier)
	{
		hErr = rest_bind_scan_CmdUnregisterNotifier(pfNotifier);
	}

	pstMsg->pvResult = (void *)hErr;
	rest_task_SyncOut();

	return ERR_OK;
}

STATIC HCHAR *rest_bind_scan_CmdGet (HCHAR *szSessionId, HxJSON_t jsArgs)
{
	RestInstance_t			*pstInst = rest_task_GetInstance(szSessionId);
	RestScanParams_t		*punParams;

	if (NULL == pstInst)
	{
		HxLOG_Error("rest_task_GetInstance err:\n");
		return rest_bind_MakeErrorString(REST_ERR_CODE_INVALID_SESSION, REST_ERR_STRING_INVALID_SESSION, TRUE);
	}

	// Multi-Instance 를 구현하기 위해서는 여기서 Argument 받은 값을 통해 GetScannerSession을 돌려야 한다.

	punParams = rest_bind_scan_GetScannerSession(REST_BIND_SCANNER_STATIC_INST_ID);
	if (NULL == punParams)
	{
		HxLOG_Error("rest_bind_scan_GetScannerSession NULL\n");
		return rest_bind_MakeErrorString(REST_ERR_CODE_INVALID_SESSION, REST_ERR_STRING_INVALID_SESSION, TRUE);
	}

	switch (punParams->stBase.eScanType)
	{
	case eREST_SCAN_TYPE_ChannelScan:
		return rest_bind_scan_chan_CmdGet(pstInst, &(punParams->stChanScan), jsArgs);

	case eREST_SCAN_TYPE_None:
		{
			HINT32				 nJWriter = rest_bind_OpenJsonWriter();
			
			if (0 == nJWriter)
			{
				HxLOG_Error("rest_bind_OpenJsonWriter() err:\n");
				return rest_bind_MakeErrorString(REST_ERR_CODE_NOMEM, REST_ERR_STRING_NOMEM, TRUE);
			}
			
			HxJSONWRITER_ObjectBegin(nJWriter);
			{
				// scannerStatus
				HxJSONWRITER_StringValue(nJWriter, "scannerStatus");
				(void)rest_datatype_JsonizeScanParams_None(nJWriter, punParams, "~scanTps");
				
				// error:
				(void)rest_bind_MakeErrorJsWriter(nJWriter, REST_ERR_CODE_OK, REST_ERR_STRING_OK);
			}
			HxJSONWRITER_ObjectEnd(nJWriter);
			return rest_bind_CloseJsonWriter(nJWriter);
		}

		break;

	default:
		break;
	}

	// Nothing:
	HxLOG_Error("No Scanning Type found\n");
	return rest_bind_MakeErrorString(REST_ERR_CODE_INVALID_VALUE, REST_ERR_STRING_INVALID_VALUE, TRUE);
}

STATIC HCHAR *rest_bind_scan_CmdPut (HCHAR *szSessionId, HxJSON_t jsPayLoad)
{
	RestScanCmd_e			 eScanCmd = eREST_SCAN_CMD_None;
	RestScanType_e			 eScanType = eREST_SCAN_TYPE_None;
	RestInstance_t			*pstInst = rest_task_GetInstance(szSessionId);
	RestScanParams_t		*punParams;

	if (NULL == pstInst)
	{
		HxLOG_Error("rest_task_GetInstance err:\n");
		return rest_bind_MakeErrorString(REST_ERR_CODE_INVALID_SESSION, REST_ERR_STRING_INVALID_SESSION, TRUE);
	}

	if (NULL == jsPayLoad)
	{
		HxLOG_Error("jsArgs NULL: Wrong payload.\n");
		return rest_bind_MakeErrorString(REST_ERR_CODE_REQUIRE_MANDATORYKEY, REST_ERR_STRING_REQUIRE_MANDATORYKEY, TRUE);
	}

	(void)rest_datatype_GetJsonEnum(jsPayLoad, "scanType", DxNAMEOF(RestScanType_e), (HINT32 *)&eScanType);
	(void)rest_datatype_GetJsonEnum(jsPayLoad, "scanCmd", DxNAMEOF(RestScanCmd_e), (HINT32 *)&eScanCmd);

	punParams = rest_bind_scan_GetScannerSession(REST_BIND_SCANNER_STATIC_INST_ID);
	if (NULL == punParams)
	{
		HxLOG_Error("rest_bind_scan_GetScannerSession NULL\n");
		return rest_bind_MakeErrorString(REST_ERR_CODE_INVALID_SESSION, REST_ERR_STRING_INVALID_SESSION, TRUE);
	}

	if ((eREST_SCAN_TYPE_None == eScanType) && (eREST_SCAN_TYPE_None != punParams->stBase.eScanType))
	{
		eScanType = punParams->stBase.eScanType;
	}

	(void)rest_datatype_DejsonizeScanParams(jsPayLoad, punParams, FALSE);
	
	switch (eScanType)
	{
	case eREST_SCAN_TYPE_ChannelScan:
		return rest_bind_scan_chan_CmdPut(pstInst, &(punParams->stChanScan), eScanCmd);

	default:
		break;
	}

	// Nothing:
	HxLOG_Error("No Scanning Type found\n");
	return rest_bind_MakeErrorString(REST_ERR_CODE_INVALID_VALUE, REST_ERR_STRING_INVALID_VALUE, TRUE);
}

STATIC HERROR rest_bind_scan_CbGet (void *pvArg)
{
	RestMsg_t			*pstMsg = (RestMsg_t *)pvArg;
	HxJSON_t			 jsArgs;
	HCHAR				*szResult;

	jsArgs = HLIB_JSON_Decode((const HCHAR *)pstMsg->aulArgs[0]);
	szResult = rest_bind_scan_CmdGet(pstMsg->szSessionId, jsArgs);
	pstMsg->pvResult = (void *)szResult;

	if (NULL != jsArgs)				{ HLIB_JSON_Delete(jsArgs); }
	rest_task_SyncOut();

	return ERR_OK;
}

STATIC HERROR rest_bind_scan_CbPut (void *pvArg)
{
	RestMsg_t			*pstMsg = (RestMsg_t *)pvArg;
	HxJSON_t			 jsPayload;
	HCHAR				*szResult;

	jsPayload = HLIB_JSON_Decode((const HCHAR *)pstMsg->aulArgs[0]);
	szResult = rest_bind_scan_CmdPut(pstMsg->szSessionId, jsPayload);
	pstMsg->pvResult = (void *)szResult;

	if (NULL != jsPayload)			{ HLIB_JSON_Delete(jsPayload); }
	rest_task_SyncOut();

	return ERR_OK;
}

#define ___MEMBER_FUNCTIONS___
HERROR rest_bind_scan_Init (void)
{
	if (TRUE != s_stRestBindScan_Inited)
	{
		// Initialize the APPKIT
		(void)APK_BUILDER_Init();

		if (rest_bind_scan_CreateScannerSession(REST_BIND_SCANNER_STATIC_INST_ID) == NULL)
		{
			HxLOG_Critical("!!! rest_bind_scan_CreateScannerSession error !!!\n");
			HxLOG_Critical("!!! rest_bind_scan_CreateScannerSession error !!!\n");
			HxLOG_Critical("!!! rest_bind_scan_CreateScannerSession error !!!\n");
		}

		s_stRestBindScan_Inited = TRUE;
	}

	return ERR_OK;
}

#define ___API_FUNCTIONS___
int REST_BIND_SCAN_RegisterNotifier (RestBind_Notifier_t pfNotifier)
{
	RestMsg_t				 stMsg;
	HERROR					 hErr;

	stMsg.pvResult		= (void *)0;
	stMsg.aulArgs[0]	= (HUINT32)pfNotifier;

	hErr = rest_task_RequestToExecute(rest_bind_scan_CbRegisterNotifier, (void *)&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("rest_task_RequestToExecute err:\n");
		return ERR_FAIL;
	}

	rest_task_SyncIn();

	return (int)stMsg.pvResult;
}

int REST_BIND_SCAN_UnregisterNotifier (RestBind_Notifier_t pfNotifier)
{
	RestMsg_t				 stMsg;
	HERROR					 hErr;

	stMsg.pvResult		= (void *)0;
	stMsg.aulArgs[0]	= (HUINT32)pfNotifier;
	
	hErr = rest_task_RequestToExecute(rest_bind_scan_CbUnregisterNotifier, (void *)&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("rest_task_RequestToExecute err:\n");
		return ERR_FAIL;
	}

	rest_task_SyncIn();

	return (int)stMsg.pvResult;
}

// GET http://127.0.0.1:37564/stb-api/v1/channel-scanners
char *REST_BIND_SCAN_Get (char *szSessionId, char *szArgJson)
{
	RestMsg_t			 stMsg;
	char				*szResult = NULL;
	HERROR				 hErr;

	stMsg.szSessionId	= REST_DUMMY_SESSION_ID;
	stMsg.pvResult		= NULL;
	stMsg.aulArgs[0]	= (HUINT32)szArgJson;
	
	hErr = rest_task_RequestToExecute(rest_bind_scan_CbGet, (void *)&stMsg);
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

// PUT http://127.0.0.1:37564/stb-api/v1/channel-scanners
char *REST_BIND_SCAN_Put (char *szSessionId, char *szPayload)
{
	RestMsg_t			 stMsg;
	char				*szResult = NULL;
	HERROR				 hErr;

	stMsg.szSessionId	= REST_DUMMY_SESSION_ID;
	stMsg.pvResult		= NULL;
	stMsg.aulArgs[0]	= (HUINT32)szPayload;
	
	hErr = rest_task_RequestToExecute(rest_bind_scan_CbPut, (void *)&stMsg);
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



