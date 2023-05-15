/*******************************************************************
	File Description
********************************************************************/
/**
	@file	 rest_bind_recorder.c
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
} restBindRecorderCCbInfo_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HBOOL						 s_stRestBindRecorder_Inited = FALSE;
STATIC HUINT32						 s_ulRestBindRecorder_MaxRecNum = 0;
STATIC HUINT32						 s_ulRestBindRecorder_InstNum = 0;
STATIC RestBindRecorderInst_t		*s_pstRestBindRecorder_InstArray = NULL;
STATIC HxList_t						*s_pstRestBindRecorder_Notifier = NULL;

#define ___LOCAL_FUNCTIONS___

STATIC HUINT32 rest_bind_recorder_CountInstance (void)
{
	return s_ulRestBindRecorder_InstNum;
}

STATIC RestBindRecorderInst_t *rest_bind_recorder_GetInstance (HUINT32 ulIndex)
{
	if ((s_ulRestBindRecorder_InstNum > 0) && (NULL != s_pstRestBindRecorder_InstArray))
	{
		if (ulIndex < s_ulRestBindRecorder_InstNum)
		{
			return &(s_pstRestBindRecorder_InstArray[ulIndex]);
		}
	}

	return NULL;	
}

STATIC RestBindRecorderInst_t *rest_bind_recorder_GetInstanceBySlot (HUINT32 ulSlot)
{
	HUINT32					 ulCnt;
	
	if ((s_ulRestBindRecorder_InstNum > 0) && (NULL != s_pstRestBindRecorder_InstArray))
	{
		for (ulCnt = 0; ulCnt < s_ulRestBindRecorder_InstNum; ulCnt++)
		{
			RestBindRecorderInst_t		*pstRecInst = &(s_pstRestBindRecorder_InstArray[ulCnt]);

			if ((eOxMR_RECSTATE_NONE != pstRecInst->eState) &&
				(ulSlot == pstRecInst->ulSlot))
			{
				return pstRecInst;
			}
		}
	}

	return NULL;	
}

STATIC HINT32 rest_bind_recorder_GetEmptyInstanceIndex (void)
{
	HINT32		 nCnt;
	
	if ((s_ulRestBindRecorder_InstNum > 0) && (NULL != s_pstRestBindRecorder_InstArray))
	{
		for (nCnt = 0; nCnt < s_ulRestBindRecorder_InstNum; nCnt++)
		{
			RestBindRecorderInst_t		*pstRecInst = &(s_pstRestBindRecorder_InstArray[nCnt]);

			if ((eOxMR_RECSTATE_NONE == pstRecInst->eState) || (eOxMR_RECSTATE_STOPPED == pstRecInst->eState))
			{
				return nCnt;
			}
		}
	}

	return -1;	
}

STATIC RestBindRecorderInst_t *rest_bind_recorder_GetEmptyInstance (void)
{
	HINT32		 nCnt = rest_bind_recorder_GetEmptyInstanceIndex();

	if (-1 != nCnt)
	{
		return &(s_pstRestBindRecorder_InstArray[nCnt]);
	}

	return NULL;
}

STATIC HERROR rest_bind_recorder_ClearInstance (RestBindRecorderInst_t *pstRecInst)
{
	HUINT32						 ulInstIndex;

	ulInstIndex = pstRecInst->ulInstIndex;

	HxSTD_MemSet(pstRecInst, 0, sizeof(RestBindRecorderInst_t));

	pstRecInst->ulInstIndex = ulInstIndex;

	return ERR_OK;
}

#define _____ACTION_FUNCTIONS_____
STATIC HCHAR *rest_bind_recorder_StartRecord (RestMsg_t *pstMsg, HUINT32 ulRecId, RestRecRequest_t *pstRecRequest)
{
	HUINT32					 ulSessionId;
	RestBindRecorderInst_t	*pstRecInst = rest_bind_recorder_GetInstance(ulRecId);
	HCHAR					 szRecordId[64];
	HINT32					 nErrCode = REST_ERR_CODE_INTERNAL;
	HCHAR					*szErrStr = REST_ERR_STRING_INTERNAL;
	HERROR					 hErr;

	if (NULL == pstRecInst)
	{
		HxLOG_Error("No Recorder Instance: RecID failed(%d)\n", ulRecId);
		nErrCode = REST_ERR_CODE_RESOURCE_NOTFOUND;		szErrStr = REST_ERR_STRING_RESOURCE_NOTFOUND;
		goto END_FUNC;
	}

	if ((eOxMR_RECSTATE_NONE != pstRecInst->eState) && (eOxMR_RECSTATE_STOPPED != pstRecInst->eState))
	{
		HxLOG_Error("APK_MEDIA_RECORD_Start err:\n");
		nErrCode = REST_ERR_CODE_RESOURCE_NOTFOUND;		szErrStr = REST_ERR_STRING_RESOURCE_NOTFOUND;
		goto END_FUNC;
	}

	switch (pstRecRequest->eSrcType)
	{
	case eREST_REC_SRCTYPE_Channel:
		hErr = APK_MEDIA_RECORD_Start(&(pstRecRequest->unArgs.stChanRec), &ulSessionId, szRecordId);
		break;

	default:
		HxLOG_Error("Rec Source Type wrong: %d\n", pstRecRequest->eSrcType);
		hErr = ERR_FAIL;
		break;
	}
	
	if (ERR_OK != hErr)
	{
		HxLOG_Error("Record Start err:\n");
		nErrCode = REST_ERR_CODE_CMD_FAILED;		szErrStr = REST_ERR_STRING_CMD_FAILED;
		goto END_FUNC;
	}

	pstRecInst->eState = eOxMR_RECSTATE_REQUESTED;
	pstRecInst->ulSlot = ulSessionId;
	pstRecInst->ulContentsId = (HUINT32)-1;
	pstRecInst->pvReqMsgObj = (void *)pstMsg;
	HxSTD_MemCopy(&(pstRecInst->stRecStartInfo), pstRecRequest, sizeof(RestRecRequest_t));
	
	// Success: wait until the appkit replied the results
	return REST_REC_WAIT_UNTIL_SERVICE_REPLIED;

END_FUNC:
	return rest_bind_MakeErrorString(nErrCode, szErrStr, TRUE);
}


#define _____COMMAND_FUNCTIONS_____
STATIC HERROR rest_bind_recorder_CmdRegisterNotifier (RestBind_Notifier_t pfNotifier)
{
	if (HLIB_LIST_Find(s_pstRestBindRecorder_Notifier, (const void *)pfNotifier) != NULL)
	{
		HxLOG_Error("Already the same notifier exists:\n");
		return ERR_OK;
	}

	s_pstRestBindRecorder_Notifier = HLIB_LIST_Append(s_pstRestBindRecorder_Notifier, (void *)pfNotifier);
	return ERR_OK;
}

STATIC HERROR rest_bind_recorder_CbRegisterNotifier (void *pvArg)
{
	RestMsg_t					*pstMsg = (RestMsg_t *)pvArg;
	RestBind_Notifier_t			 pfNotifier = (RestBind_Notifier_t)pstMsg->aulArgs[0];
	HERROR						 hErr = ERR_FAIL;

	if (NULL != pfNotifier)
	{
		hErr = rest_bind_recorder_CmdRegisterNotifier(pfNotifier);
	}

	pstMsg->pvResult = (void *)hErr;
	rest_task_SyncOut();

	return ERR_OK;
}

STATIC HERROR rest_bind_recorder_CmdUnregisterNotifier (RestBind_Notifier_t pfNotifier)
{
	s_pstRestBindRecorder_Notifier = HLIB_LIST_Remove(s_pstRestBindRecorder_Notifier, (const void *)pfNotifier);
	return ERR_OK;
}

STATIC HERROR rest_bind_recorder_CbUnregisterNotifier (void *pvArg)
{
	RestMsg_t					*pstMsg = (RestMsg_t *)pvArg;
	RestBind_Notifier_t			 pfNotifier = (RestBind_Notifier_t)pstMsg->aulArgs[0];
	HERROR						 hErr = ERR_FAIL;

	if (NULL != pfNotifier)
	{
		hErr = rest_bind_recorder_CmdUnregisterNotifier(pfNotifier);
	}

	pstMsg->pvResult = (void *)hErr;
	rest_task_SyncOut();

	return ERR_OK;
}

STATIC HCHAR *rest_bind_recorder_CmdGetRecordersInfo (RestInstance_t *pstInst, HxJSON_t jsArgs)
{
	HINT32					 nJWriter = 0;
	HUINT32					 ulCnt, ulRecInstNum, ulRunningRecInst;
	OxMediaRec_Capability_t	 stCap;
	HCHAR					*szResultStr = NULL;
	HERROR					 hErr;
	
	nJWriter = rest_bind_OpenJsonWriter();
	if (0 == nJWriter)
	{
		HxLOG_Error("rest_bind_OpenJsonWriter() err:\n");
		return rest_bind_MakeErrorString(REST_ERR_CODE_NOMEM, REST_ERR_STRING_NOMEM, TRUE);
	}

	hErr = APK_MEDIA_RECORD_GetCapability(&stCap);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("APK_MEDIA_RECORD_GetCapability err:\n");
		HxSTD_MemSet(&stCap, 0, sizeof(OxMediaRec_Capability_t));
	}

	ulRunningRecInst = 0;
	ulRecInstNum = rest_bind_recorder_CountInstance();
	for (ulCnt = 0; ulCnt < ulRecInstNum; ulCnt++)
	{
		RestBindRecorderInst_t		*pstRecInst = rest_bind_recorder_GetInstance(ulCnt);

		if ((NULL != pstRecInst) &&
			((eOxMR_RECSTATE_NONE != pstRecInst->eState) && (eOxMR_RECSTATE_STOPPED != pstRecInst->eState)))
		{
			ulRunningRecInst ++;			
		}
	}

	HxJSONWRITER_ObjectBegin(nJWriter);
	{
		// Players Overview
		HxJSONWRITER_Object(nJWriter, "recordersOverview");
		{
			HxJSONWRITER_Integer(nJWriter, "maxRecordingNumber", stCap.ulMaxRec);
			HxJSONWRITER_Integer(nJWriter, "minRecorderId", 0);
			HxJSONWRITER_Integer(nJWriter, "maxRecorderId", stCap.ulMaxRecId - 1);
			HxJSONWRITER_Integer(nJWriter, "runningRecorders", ulRunningRecInst);
			HxJSONWRITER_Integer(nJWriter, "leftRecordableCount", stCap.ulMaxRec - ulRunningRecInst);
		}
		HxJSONWRITER_ObjectEnd(nJWriter);
		
		// Error
		(void)rest_bind_MakeErrorJsWriter(nJWriter, REST_ERR_CODE_OK, REST_ERR_STRING_OK);
	}
	HxJSONWRITER_ObjectEnd(nJWriter);

	szResultStr = rest_bind_CloseJsonWriter(nJWriter);

	return szResultStr;
}

STATIC HCHAR *rest_bind_recorder_CmdGetOneRecorderStatus (RestInstance_t *pstInst, HUINT32 ulRecorderId, HxJSON_t jsArgs)
{
	HINT32					 nJWriter = 0;
	HCHAR					*szResultStr = NULL;
	RestBindRecorderInst_t	*pstRecInst;
	HINT32					 nErrCode = REST_ERR_CODE_INTERNAL;
	HCHAR					*szErrString = REST_ERR_STRING_INTERNAL;

	pstRecInst = rest_bind_recorder_GetInstance(ulRecorderId);
	if (NULL == pstRecInst)
	{
		HxLOG_Error("rest_bind_recorder_GetInstance failed\n");
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
		// Recorder Status
		(void)rest_datatype_JsonizeRecorderInstance(nJWriter, pstRecInst, "recorderStatus", NULL);

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
	
	if (NULL == szResultStr)
	{
		szResultStr = rest_bind_MakeErrorString(nErrCode, szErrString, TRUE);
	}

	return szResultStr;
}

STATIC HCHAR *rest_bind_recorder_CmdGetMultipleRecorderStatus (RestInstance_t *pstInst, HxJSON_t jsArgs)
{
	HINT32					 nJWriter = 0;
	HINT32					 nStartIdx = -1, nEndIdx = -1, nCount = -1;
	HINT32					 nIndex, nMaxIdx;
	HCHAR					*szResultStr = NULL;
	RestBindRecorderInst_t	*pstRecInst;
	HINT32					 nErrCode = REST_ERR_CODE_INTERNAL;
	HCHAR					*szErrString = REST_ERR_STRING_INTERNAL;

	(void)rest_bind_GetNumberValueFromJson(jsArgs, "startidx", &nStartIdx);
	(void)rest_bind_GetNumberValueFromJson(jsArgs, "endidx", &nEndIdx);
	(void)rest_bind_GetNumberValueFromJson(jsArgs, "cnt", &nCount);
	nMaxIdx = (HINT32)rest_bind_recorder_CountInstance() - 1;

	if ((nStartIdx < 0) || (nStartIdx > nMaxIdx))					{ nStartIdx = 0; }
	if ((nEndIdx < 0) || (nEndIdx > nMaxIdx))						{ nEndIdx = nMaxIdx; }
	if ((nCount < 0) || (nCount > (nMaxIdx + 1)))					{ nCount = nMaxIdx + 1; }

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
		// Recorders:
		HxJSONWRITER_Array(nJWriter, "recorders");
		for (nIndex = 0; ((nIndex + nStartIdx) <= nEndIdx) && (nIndex < nCount); nIndex++)
		{
			pstRecInst = rest_bind_recorder_GetInstance(nIndex + nStartIdx);
			if (NULL != pstRecInst)
			{
				(void)rest_datatype_JsonizeRecorderInstance(nJWriter, pstRecInst, "", NULL);
			}
			
		}
		HxJSONWRITER_ArrayEnd(nJWriter);

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
	
	if (NULL == szResultStr)
	{
		szResultStr = rest_bind_MakeErrorString(nErrCode, szErrString, TRUE);
	}

	return szResultStr;
}


STATIC HCHAR *rest_bind_recorder_CmdGet (HCHAR *szSessionId, HINT32 nRecorderId, HxJSON_t jsArgs)
{
	RestInstance_t			*pstInst = rest_task_GetInstance(szSessionId);

	if (NULL == pstInst)
	{
		HxLOG_Error("rest_task_GetInstance err:\n");
		return rest_bind_MakeErrorString(REST_ERR_CODE_INVALID_SESSION, REST_ERR_STRING_INVALID_SESSION, TRUE);
	}

	// URL: http://127.0.0.1/stb-api/v1/recorders[?Options]
	if (nRecorderId < 0)
	{
		HINT32			 nStartIdx = -1;
		
		(void)rest_bind_GetNumberValueFromJson(jsArgs, "startidx", &nStartIdx);

		// Start Index가 없으면 Recorders Brief 를...
		if (nStartIdx < 0)
		{
			return rest_bind_recorder_CmdGetRecordersInfo(pstInst, jsArgs);
		}
		else
		{
			return rest_bind_recorder_CmdGetMultipleRecorderStatus(pstInst, jsArgs);
		}
	}
	
	// URL: http://127.0.0.1/stb-api/v1/recorders/0[?Options]
	return rest_bind_recorder_CmdGetOneRecorderStatus(pstInst, (HUINT32)nRecorderId, jsArgs);
}

STATIC HERROR rest_bind_recorder_CbGet (void *pvArg)
{
	HINT32			 nRecorderId = -1;
	HCHAR			*szResult;
	HxJSON_t		 jsArgs = NULL;
	RestMsg_t		*pstMsg = (RestMsg_t *)pvArg;

	if (NULL == pstMsg)					{ return ERR_FAIL; }
	
	nRecorderId	= (HINT32)pstMsg->aulArgs[0];
	jsArgs = HLIB_JSON_Decode((const HCHAR *)pstMsg->aulArgs[1]);

	szResult = rest_bind_recorder_CmdGet(pstMsg->szSessionId, nRecorderId, jsArgs);
	pstMsg->pvResult = (void *)szResult;

	if (NULL != jsArgs)			{ HLIB_JSON_Delete(jsArgs); }
	rest_task_SyncOut();

	return ERR_OK;
}

STATIC HCHAR *rest_bind_recorder_GetReplyPayload (RestBindRecorderInst_t *pstRecInst)
{
	HINT32					 nJWriter = 0;
	HCHAR					*szResultStr = NULL;
	HINT32					 nErrCode = REST_ERR_CODE_INTERNAL;
	HCHAR					*szErrString = REST_ERR_STRING_INTERNAL;

	nJWriter = rest_bind_OpenJsonWriter();
	if (0 == nJWriter)
	{
		HxLOG_Error("rest_bind_OpenJsonWriter() err:\n");
		nErrCode = REST_ERR_CODE_NOMEM; 	szErrString = REST_ERR_STRING_NOMEM;
		goto END_FUNC;
	}

	// Jsonize the state
	HxJSONWRITER_ObjectBegin(nJWriter);
	{
		// Play Status
		(void)rest_datatype_JsonizeRecorderInstance(nJWriter, pstRecInst, "recorderStatus", NULL);
		
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
	
	if (NULL == szResultStr)
	{
		szResultStr = rest_bind_MakeErrorString(nErrCode, szErrString, TRUE);
	}

	return szResultStr;
}


STATIC HCHAR *rest_bind_recorder_CmdRecordStart (HCHAR *szSessionId, HUINT32 ulRecorderId, HxJSON_t jsPayLoad, RestMsg_t *pstMsg)
{
	HUINT32					 ulSessionId;
	HCHAR					 szRecordId[64];
	HCHAR					*szResult = NULL;
	RestInstance_t			*pstInst = rest_task_GetInstance(szSessionId);
	RestBindRecorderInst_t	*pstRecInst = rest_bind_recorder_GetInstance(ulRecorderId);
	RestRecRequest_t		 stRecRequest;
	HINT32					 nErrCode = REST_ERR_CODE_INTERNAL;
	HCHAR					*szErrStr = REST_ERR_STRING_INTERNAL;
	HERROR					 hErr;

	if (NULL == pstInst)
	{
		HxLOG_Error("rest_task_GetInstance err:\n");
		nErrCode = REST_ERR_CODE_INVALID_SESSION;			szErrStr = REST_ERR_STRING_INVALID_SESSION;
		goto END_FUNC;
	}

	if (NULL == pstRecInst)
	{
		HxLOG_Error("rest_bind_recorder_GetInstance err:\n");
		nErrCode = REST_ERR_CODE_RESOURCE_NOTFOUND;			szErrStr = REST_ERR_STRING_RESOURCE_NOTFOUND;
		goto END_FUNC;		
	}

	hErr = rest_datatype_DejsonizeRecRequest(jsPayLoad, &stRecRequest, TRUE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("rest_datatype_DejsonizeRecRequest err:\n");
		nErrCode = REST_ERR_CODE_REQUIRE_MANDATORYKEY;		szErrStr = REST_ERR_STRING_REQUIRE_MANDATORYKEY;
		goto END_FUNC;
	}

	if ((eOxMR_RECSTATE_NONE != pstRecInst->eState) && (eOxMR_RECSTATE_STOPPED != pstRecInst->eState))
	{
		HxLOG_Error("Recorder Instance not idle...\n");
		nErrCode = REST_ERR_CODE_RESOURCE_NOTFOUND; 	szErrStr = REST_ERR_STRING_RESOURCE_NOTFOUND;
		goto END_FUNC;
	}

	switch (stRecRequest.eSrcType)
	{
	case eREST_REC_SRCTYPE_Channel:
		hErr = APK_MEDIA_RECORD_Start(&(stRecRequest.unArgs.stChanRec), &ulSessionId, szRecordId);
		break;

	default:
		HxLOG_Error("Rec Source Type wrong: %d\n", stRecRequest.eSrcType);
		hErr = ERR_FAIL;
		break;
	}

	if (ERR_OK != hErr)
	{
		HxLOG_Error("Record Start err:\n");
		nErrCode = REST_ERR_CODE_CMD_FAILED;		szErrStr = REST_ERR_STRING_CMD_FAILED;
		goto END_FUNC;
	}

	pstRecInst->eState = eOxMR_RECSTATE_REQUESTED;
	pstRecInst->ulSlot = ulSessionId;
	pstRecInst->ulContentsId = (HUINT32)-1;
	pstRecInst->pvReqMsgObj = (void *)pstMsg;
	HxSTD_MemCopy(&(pstRecInst->stRecStartInfo), &stRecRequest, sizeof(RestRecRequest_t));

	// Success: wait until the appkit replied the results
	szResult = REST_REC_WAIT_UNTIL_SERVICE_REPLIED;

END_FUNC:
	if (NULL == szResult)
	{
		szResult = rest_bind_MakeErrorString(nErrCode, szErrStr, TRUE);
	}

	return szResult;
}

STATIC HCHAR *rest_bind_recorder_CmdRecordStop (HCHAR *szSessionId, HUINT32 ulRecorderId, HxJSON_t jsPayLoad, RestMsg_t *pstMsg)
{
	HCHAR					*szResult = NULL;
	RestInstance_t			*pstInst = rest_task_GetInstance(szSessionId);
	RestBindRecorderInst_t	*pstRecInst = rest_bind_recorder_GetInstance(ulRecorderId);
	HINT32					 nErrCode = REST_ERR_CODE_INTERNAL;
	HCHAR					*szErrStr = REST_ERR_STRING_INTERNAL;
	HERROR					 hErr;

	if (NULL == pstInst)
	{
		HxLOG_Error("rest_task_GetInstance err:\n");
		nErrCode = REST_ERR_CODE_INVALID_SESSION;			szErrStr = REST_ERR_STRING_INVALID_SESSION;
		goto END_FUNC;
	}

	if (NULL == pstRecInst)
	{
		HxLOG_Error("rest_bind_recorder_GetInstance err:\n");
		nErrCode = REST_ERR_CODE_RESOURCE_NOTFOUND;			szErrStr = REST_ERR_STRING_RESOURCE_NOTFOUND;
		goto END_FUNC;		
	}

	if ((eOxMR_RECSTATE_NONE == pstRecInst->eState) || (eOxMR_RECSTATE_STOPPED == pstRecInst->eState))
	{
		HxLOG_Error("Recorder Instance idle...\n");
		nErrCode = REST_ERR_CODE_RESOURCE_NOTFOUND; 	szErrStr = REST_ERR_STRING_RESOURCE_NOTFOUND;
		goto END_FUNC;
	}

	switch (pstRecInst->stRecStartInfo.eSrcType)
	{
	case eREST_REC_SRCTYPE_Channel:
		hErr = APK_MEDIA_RECORD_Stop(pstRecInst->ulSlot);
		break;

	default:
		HxLOG_Error("Rec Source Type wrong: %d\n", pstRecInst->stRecStartInfo.eSrcType);
		hErr = ERR_FAIL;
		break;
	}

	if (ERR_OK != hErr)
	{
		HxLOG_Error("Record Start err:\n");
		nErrCode = REST_ERR_CODE_CMD_FAILED;		szErrStr = REST_ERR_STRING_CMD_FAILED;
		goto END_FUNC;
	}

	pstRecInst->eState = eOxMR_RECSTATE_STOPPING;
	szResult = rest_bind_recorder_GetReplyPayload(pstRecInst);

END_FUNC:
	if (NULL == szResult)
	{
		szResult = rest_bind_MakeErrorString(nErrCode, szErrStr, TRUE);
	}

	return szResult;
}

STATIC HERROR rest_bind_recorder_CbPut (void *pvArg)
{
	HUINT32				 ulRecorderId = (HUINT32)-1;
	HCHAR				*szResult = NULL;
	RestMsg_t			*pstMsg = (RestMsg_t *)pvArg;
	HxJSON_t			 jsPayLoad = NULL;

	if (NULL == pstMsg)					{ return ERR_FAIL; }
	
	ulRecorderId	= (HUINT32)pstMsg->aulArgs[0];
	jsPayLoad = HLIB_JSON_Decode((const HCHAR *)pstMsg->aulArgs[1]);
	if (NULL != jsPayLoad)
	{
		HCHAR			*szCmd = (HCHAR *)HLIB_JSON_Object_GetString(jsPayLoad, "cmd");

		if (!HxSTD_StrEmpty(szCmd))
		{
			// Command [Record]
			if (HxSTD_StrCmp(szCmd, "record") == 0)
			{
				szResult = rest_bind_recorder_CmdRecordStart(pstMsg->szSessionId, ulRecorderId, jsPayLoad, pstMsg);
			}
			// Command [Stop]
			else if (HxSTD_StrCmp(szCmd, "stop") == 0)
			{
				szResult = rest_bind_recorder_CmdRecordStop(pstMsg->szSessionId, ulRecorderId, jsPayLoad, pstMsg);
			}
			// Otherwise: Unknown Commands
			else
			{
				HxLOG_Error("Unknown Command [%s]\n", szCmd);
				szResult = rest_bind_MakeErrorString(REST_ERR_CODE_UNKNOWN_CMD, REST_ERR_STRING_UNKNOWN_CMD, TRUE);
			}
		}
		else
		{
			szResult = rest_bind_MakeErrorString(REST_ERR_CODE_REQUIRE_MANDATORYKEY, REST_ERR_STRING_REQUIRE_MANDATORYKEY, TRUE);
		}
	}
	else
	{
		szResult = rest_bind_MakeErrorString(REST_ERR_CODE_REQUIRE_MANDATORYKEY, REST_ERR_STRING_REQUIRE_MANDATORYKEY, TRUE);
	}

	// Notify 받고 return 할 필요가 없는 경우임 
	if (HxSTD_StrCmp(szResult, REST_REC_WAIT_UNTIL_SERVICE_REPLIED) != 0)
	{
		pstMsg->pvResult = (void *)szResult;
		rest_task_SyncOut();
	}

	if (NULL != jsPayLoad)			{ HLIB_JSON_Delete(jsPayLoad); }
	return ERR_OK;
}


#define _____NOTIFIERS_____

STATIC HCHAR *rest_bind_recorder_GetNotifyPayload (RestBindRecorderInst_t *pstRecInst, HCHAR *szEventStr, OxMediaRec_NotifierData_t *punNotifyData)
{
	HINT32					 nJWriter = 0;
	HCHAR					*szResultStr = NULL;
	HINT32					 nErrCode = REST_ERR_CODE_INTERNAL;
	HCHAR					*szErrString = REST_ERR_STRING_INTERNAL;

	nJWriter = rest_bind_OpenJsonWriter();
	if (0 == nJWriter)
	{
		HxLOG_Error("rest_bind_OpenJsonWriter() err:\n");
		nErrCode = REST_ERR_CODE_NOMEM; 	szErrString = REST_ERR_STRING_NOMEM;
		goto END_FUNC;
	}

	// Jsonize the state
	HxJSONWRITER_ObjectBegin(nJWriter);
	{
		// Event
		(void)rest_datatype_SetJsonString(nJWriter, "event", szEventStr, NULL);
		
		// Data
		(void)HxJSONWRITER_Object(nJWriter, "data");
		{
			// Play Status
			(void)rest_datatype_JsonizeRecorderInstance(nJWriter, pstRecInst, "recorderStatus", NULL);
		}
		(void)HxJSONWRITER_ObjectEnd(nJWriter);
		
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
	
	if (NULL == szResultStr)
	{
		szResultStr = rest_bind_MakeErrorString(nErrCode, szErrString, TRUE);
	}

	return szResultStr;
}

STATIC void rest_bind_recorder_NotifyApkMediaRec (HUINT32 ulSvcUid, HUINT32 ulSlot, OxMediaRec_Event_e eEvent, OxMediaRec_NotifierData_t *pstNotifyData, void *pvUserData)
{
	HCHAR						*szNotify = NULL, *szReply = NULL;
	RestBindRecorderInst_t		*pstRecInst = rest_bind_recorder_GetInstanceBySlot(ulSlot);

	switch (eEvent)
	{
	case eOxMR_EVENT_REC_STARTED:
		if (NULL != pstRecInst)
		{
			if (eOxMR_RECSTATE_REQUESTED == pstRecInst->eState)
			{
				pstRecInst->eState = eOxMR_RECSTATE_RECORDING;
				(void)APK_MEDIA_RECORD_GetContentId(ulSlot, &(pstRecInst->ulContentsId));
				
				// REST_BIND를 통한 Record Start: REST 명령에 의한 결과 result를 보내야 한다.
				szReply = rest_bind_recorder_GetReplyPayload(pstRecInst);
			}
			else
			{
				HxLOG_Error("State something wrong: %d", pstRecInst->eState);
			}
		}
		// REST_BIND를 통하지 않은 Record Start
		else	
		{
			pstRecInst = rest_bind_recorder_GetEmptyInstance();
			if (NULL == pstRecInst)
			{
				HxLOG_Error("rest_bind_recorder_GetEmptyInstance NULL:\n");
				return;
			}

			pstRecInst->eState = eOxMR_RECSTATE_RECORDING;
			pstRecInst->ulSlot = ulSlot;
			(void)APK_MEDIA_RECORD_GetContentId(ulSlot, &(pstRecInst->ulContentsId));
		}

		szNotify = rest_bind_recorder_GetNotifyPayload(pstRecInst, "recordStarted", pstNotifyData);

		break;

	case eOxMR_EVENT_REC_STOPPED:
		if (NULL != pstRecInst)
		{
			pstRecInst->eState = eOxMR_RECSTATE_STOPPED;
			szNotify = rest_bind_recorder_GetNotifyPayload(pstRecInst, "recordStopped", pstNotifyData);
			(void)rest_bind_recorder_ClearInstance(pstRecInst);
		}
		else
		{
			HxLOG_Error("No running instance...\n");
		}

		break;

	default:
		break;
	}

	if ((NULL != pstRecInst) && (NULL != pstRecInst->pvReqMsgObj))
	{
		RestMsg_t					*pstReqMsgObj = (RestMsg_t *)pstRecInst->pvReqMsgObj;

		if (NULL == szReply)
		{
			szReply = rest_bind_MakeErrorString(REST_ERR_CODE_INTERNAL, REST_ERR_STRING_INTERNAL, TRUE);
		}
		
		pstReqMsgObj->pvResult = (void *)szReply;
		pstRecInst->pvReqMsgObj = NULL;
		szReply = NULL;
		rest_task_SyncOut();
	}
	
	if (NULL != szNotify)
	{
		HxList_t					*pstListItem;

		for (pstListItem = s_pstRestBindRecorder_Notifier; NULL != pstListItem; pstListItem = pstListItem->next)
		{
			RestBind_Notifier_t	pfNotifier = (RestBind_Notifier_t)HLIB_LIST_Data(pstListItem);

			if (NULL != pfNotifier)
			{
				pfNotifier("recorder", szNotify);
			}
		}

		rest_datatype_AppendStringBufferToTemporary(szNotify);
	}

	if (NULL != szReply)
	{
		rest_datatype_AppendStringBufferToTemporary(szReply);
	}
}

STATIC void rest_bind_recorder_NotifierApkSchedule (void *pvUserData, APK_META_SCH_MsgType_e eMsgType, HUINT32 ulSlot, DxSchedule_t *pstSchedule)
{
	RestBindRecorderInst_t	*pstRecInst;

	switch (eMsgType)
	{
	case eAPK_METASCH_MSG_TimeUpStop:
	case eAPK_METASCH_MSG_ScheduleRemoved:
		pstRecInst = rest_bind_recorder_GetInstanceBySlot(ulSlot);
		if (NULL != pstRecInst)
		{
			HCHAR			*szResult = NULL;

			pstRecInst->eState = eOxMR_RECSTATE_NONE;

			if (NULL != pstRecInst->pvReqMsgObj)
			{
				RestMsg_t					*pstReqMsgObj = (RestMsg_t *)pstRecInst->pvReqMsgObj;

				szResult = rest_bind_MakeErrorString(REST_ERR_CODE_CMD_FAILED, REST_ERR_STRING_CMD_FAILED, TRUE);
				pstReqMsgObj->pvResult = (void *)szResult;
				pstRecInst->pvReqMsgObj = NULL;
				rest_task_SyncOut();

				return;
			}

			return;
		}

		break;

	default:
		break;
	}

	// Notifier 동작 관련은 여기서 작업한다.
}

STATIC HERROR rest_bind_recorder_InitInstances (void)
{
	HUINT32 					 ulCnt;
	RestBindRecorderInst_t 		*pstRecInstArray = NULL;
	OxMediaRec_Capability_t		 stCap;
	HERROR						 hErr, hResult = ERR_FAIL;

	if (NULL != s_pstRestBindRecorder_InstArray)
	{
		hResult = ERR_OK;
		goto END_FUNC;
	}

	hErr = APK_MEDIA_RECORD_GetCapability(&stCap);
	if ((ERR_OK != hErr) || (0 == stCap.ulMaxRecId))
	{
		HxLOG_Error("APK_MEDIA_RECORD_GetCapability err:\n");
		goto END_FUNC;
	}

	pstRecInstArray = (RestBindRecorderInst_t *)HLIB_STD_MemCalloc(sizeof(RestBindRecorderInst_t) * stCap.ulMaxRecId);
	if (NULL == pstRecInstArray)
	{
		HxLOG_Error("Memory Allocation for the context failed:\n");
		goto END_FUNC;
	}

	for (ulCnt = 0; ulCnt < stCap.ulMaxRecId; ulCnt++)
	{
		RestBindRecorderInst_t			*pstRecInst = &(pstRecInstArray[ulCnt]);

		pstRecInst->ulInstIndex	= ulCnt;
		pstRecInst->eState		= eOxMR_RECSTATE_NONE;
	}

	(void)APK_MEDIA_RECORD_RegisterNotifier((HUINT32)-1, rest_bind_recorder_NotifyApkMediaRec, NULL);

	s_ulRestBindRecorder_InstNum		= stCap.ulMaxRecId;
	s_ulRestBindRecorder_MaxRecNum		= stCap.ulMaxRec;
	s_pstRestBindRecorder_InstArray		= pstRecInstArray;
	
	pstRecInstArray = NULL;
	hResult = ERR_OK;

END_FUNC:
	if (NULL != pstRecInstArray)
	{
		HLIB_STD_MemFree(pstRecInstArray);
	}

	return hResult;
}

STATIC HERROR rest_bind_recorder_CbInitInstances (void *pvArg)
{
	OxMediaRec_Capability_t	 stCap;
	HERROR					 hErr;

	HLIB_STD_TaskSleep(50);

	HxSTD_MemSet(&stCap, 0, sizeof(OxMediaRec_Capability_t));
	hErr = APK_MEDIA_RECORD_GetCapability(&stCap);
	if (ERR_OK != hErr)
	{
		hErr = rest_task_RequestToExecute(rest_bind_recorder_CbInitInstances, (void *)NULL);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("rest_bind_recorder_CbInitInstances failed:\n");
		}

		return ERR_FAIL;
	}

	HLIB_STD_TaskSleep(50);
	
	(void)rest_bind_recorder_InitInstances();
	HxLOG_Info("Player Initialization succeeded:\n");	

	return ERR_OK;
}


#define ___MEMBER_FUNCTIONS___
HERROR rest_bind_recorder_Init (void)
{
	HERROR				 hErr;
	
	if (TRUE != s_stRestBindRecorder_Inited)
	{
		// Initialize the APPKIT
		(void)APK_MEDIA_RECORD_Init();
		(void)APK_META_SCHEDULE_RegisterNotifier(rest_bind_recorder_NotifierApkSchedule, NULL);

		// Init 후 약간의 시간을 기다려야 Obama에서 제대로 된 정보를 주므로 wait 후 InitContext를 한다. 
		hErr = rest_task_RequestToExecute(rest_bind_recorder_CbInitInstances, (void *)NULL);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("rest_bind_recorder_CbInitInstances failed:\n");
		}
		
		s_stRestBindRecorder_Inited = TRUE;
	}

	return ERR_OK;
}

#define ___API_FUNCTIONS___
int REST_BIND_RECORDER_RegisterNotifier (RestBind_Notifier_t pfNotifier)
{
	RestMsg_t				 stMsg;
	HERROR					 hErr;

	stMsg.pvResult		= (void *)0;
	stMsg.aulArgs[0]	= (HUINT32)pfNotifier;
	
	hErr = rest_task_RequestToExecute(rest_bind_recorder_CbRegisterNotifier, (void *)&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("rest_task_RequestToExecute err:\n");
		return ERR_FAIL;
	}

	rest_task_SyncIn();

	return (int)stMsg.pvResult;
}

int REST_BIND_RECORDER_UnregisterNotifier (RestBind_Notifier_t pfNotifier)
{
	RestMsg_t				 stMsg;
	HERROR					 hErr;

	stMsg.pvResult		= (void *)0;
	stMsg.aulArgs[0]	= (HUINT32)pfNotifier;
	
	hErr = rest_task_RequestToExecute(rest_bind_recorder_CbUnregisterNotifier, (void *)&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("rest_task_RequestToExecute err:\n");
		return ERR_FAIL;
	}

	rest_task_SyncIn();

	return (int)stMsg.pvResult;
}

// GET http://127.0.0.1:37564/stb-api/v1/recorders
char *REST_BIND_RECORDER_Get (char *szSessionId, char *szArgJson)
{
	RestMsg_t			 stMsg;
	char				*szResult = NULL;
	HERROR				 hErr;

	stMsg.szSessionId	= REST_DUMMY_SESSION_ID;
	stMsg.pvResult		= NULL;
	stMsg.aulArgs[0]	= (HUINT32)-1;
	stMsg.aulArgs[1]	= (HUINT32)szArgJson;
	
	hErr = rest_task_RequestToExecute(rest_bind_recorder_CbGet, (void *)&stMsg);
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

// GET http://127.0.0.1:37564/stb-api/v1/recorders/0
char *REST_BIND_RECORDER_GetOne (char *szSessionId, int nRecorderId, char *szArgJson)
{
	RestMsg_t			 stMsg;
	char				*szResult = NULL;
	HERROR				 hErr;

	stMsg.szSessionId	= REST_DUMMY_SESSION_ID;
	stMsg.pvResult		= NULL;
	stMsg.aulArgs[0]	= (HUINT32)nRecorderId;
	stMsg.aulArgs[1]	= (HUINT32)szArgJson;
	
	hErr = rest_task_RequestToExecute(rest_bind_recorder_CbGet, (void *)&stMsg);
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

// GET http://127.0.0.1:37564/stb-api/v1/recorders/0
char *REST_BIND_RECORDER_PutOne (char *szSessionId, int nRecorderId, char *szJsPayload)
{
	RestMsg_t			 stMsg;
	char				*szResult = NULL;
	HERROR				 hErr;

	stMsg.szSessionId	= REST_DUMMY_SESSION_ID;
	stMsg.pvResult		= NULL;
	stMsg.aulArgs[0]	= (HUINT32)nRecorderId;
	stMsg.aulArgs[1]	= (HUINT32)szJsPayload;
	
	hErr = rest_task_RequestToExecute(rest_bind_recorder_CbPut, (void *)&stMsg);
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

