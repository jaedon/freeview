/*******************************************************************
	File Description
********************************************************************/
/**
	@file	 rest_bind_record.c
	@brief

	Description:  									\n
	Module: node/binding/bind			\n

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
typedef enum
{
	eREST_REC_STATE_None		= 0,

	eREST_REC_STATE_Starting,
	eREST_REC_STATE_Running,
	eREST_REC_STATE_Finished,

	eREST_REC_STATE_EndOfCase
} RestRecState_e;

typedef struct
{
	HUINT32				 ulSessionId;
	RestMsg_t			*pstReqMsg;
	RestRecState_e		 eState;
} RestRecInst_t;

typedef struct
{
	HUINT32				 ulReserved;
	HxList_t			*pstRecInstList;			// RestRecInst_t
} RestRecContext_t;

typedef struct
{
	HINT32				 nStartUid;
	HINT32				 nEndUid;
	HINT32				 nRecCnt;

	HINT32				 nListCnt;
} RestRecGetFilter_t;

typedef struct
{
	HINT32				 nMinRecUid;
	HINT32				 nMaxRecUid;
	HINT32				 nRecNum;
} RestRecGetBriefInfo_t;

typedef struct
{
	HINT32				 nJWriter;
	HCHAR				*szMember;
} RestRecVectorWrite_t;


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC RestRecContext_t			 s_stRestBindRec_Context;
STATIC HxList_t					*s_pstRestBindRec_Notifier = NULL;

#define ___LOCAL_FUNCTIONS___
STATIC HCHAR *	rest_bind_record_MakeReplyRecStartString (RestRecInst_t *pstRecInst, HUINT32 ulContentId, OxMediaRec_StartInfo_t *pstStart);


STATIC RestRecContext_t *rest_bind_record_GetContext (void)
{
	return &s_stRestBindRec_Context;
}

STATIC HBOOL rest_bind_record_cmpReqInstBySessionId (void * pvUserData,void * pvListItem)
{
	RestRecInst_t			*pstUserReq = (RestRecInst_t *)pvUserData;
	RestRecInst_t			*pstListReq = (RestRecInst_t *)pvListItem;

	if ((NULL != pstUserReq) && (NULL != pstListReq))
	{
		if (pstUserReq->ulSessionId == pstListReq->ulSessionId)
		{
			return TRUE;
		}
	}

	return FALSE;
}

STATIC RestRecInst_t *rest_bind_record_FindReqInstBySlotUid (RestRecContext_t *pstContext, HUINT32 ulSlotUid)
{
	HxList_t			*pstListItem;
	RestRecInst_t	 stReqCmp;

	stReqCmp.ulSessionId = ulSlotUid;

	pstListItem = HLIB_LIST_FindEx(pstContext->pstRecInstList, (const void *)&stReqCmp, rest_bind_record_cmpReqInstBySessionId);
	return (RestRecInst_t *)HLIB_LIST_Data(pstListItem);
}

STATIC void rest_bind_record_NotifierApkSchedule (void *pvUserData, APK_META_SCH_MsgType_e eMsgType, HUINT32 ulSlot, DxSchedule_t *pstSchedule)
{
	RestRecContext_t	*pstContext = (RestRecContext_t *)pvUserData;
	RestRecInst_t	*pstRecInst;

	switch (eMsgType)
	{
	case eAPK_METASCH_MSG_TimeUpStop:
	case eAPK_METASCH_MSG_ScheduleRemoved:
		pstRecInst = rest_bind_record_FindReqInstBySlotUid(pstContext, ulSlot);
		if (NULL != pstRecInst)
		{
			HCHAR			*szResult = NULL;

			HLIB_LIST_Remove(pstContext->pstRecInstList, (const void *)pstRecInst);

			szResult = rest_bind_MakeErrorString(REST_ERR_CODE_CMD_FAILED, REST_ERR_STRING_CMD_FAILED, TRUE);
			pstRecInst->pstReqMsg->pvResult = (void *)szResult;
			rest_task_SyncOut();

			HLIB_STD_MemFree(pstRecInst);
			return;
		}

		break;

	default:
		break;
	}

	// Notifier 동작 관련은 여기서 작업한다.
}

STATIC void rest_bind_record_NotifierApkRecord (HUINT32 ulSvcUid, HUINT32 ulSlot, OxMediaRec_Event_e eEvent, OxMediaRec_NotifierData_t *punNotifyData, void *pvUserData)
{
	RestRecContext_t	*pstContext = (RestRecContext_t *)pvUserData;
	RestRecInst_t	*pstRecInst;

	switch (eEvent)
	{
	case eOxMR_EVENT_REC_STARTED:
		pstRecInst = rest_bind_record_FindReqInstBySlotUid(pstContext, ulSlot);
		if (NULL != pstRecInst)
		{
			HUINT32			 ulContentId = 0;
			HCHAR			*szResult = NULL;

			HLIB_LIST_Remove(pstContext->pstRecInstList, (const void *)pstRecInst);
			APK_MEDIA_RECORD_UnregisterNotifier(ulSvcUid, rest_bind_record_NotifierApkRecord, (void *)pstContext);

			APK_MEDIA_RECORD_GetContentId(ulSlot, &ulContentId);

			szResult = rest_bind_record_MakeReplyRecStartString(pstRecInst, ulContentId, &(punNotifyData->stRecStarted));
			pstRecInst->pstReqMsg->pvResult = (void *)szResult;
			rest_task_SyncOut();

			HLIB_STD_MemFree(pstRecInst);
			return;
		}

		break;

	case eOxMR_EVENT_REC_STOPPED:
		pstRecInst = rest_bind_record_FindReqInstBySlotUid(pstContext, ulSlot);
		if (NULL != pstRecInst)
		{
			HCHAR			*szResult = NULL;

			HLIB_LIST_Remove(pstContext->pstRecInstList, (const void *)pstRecInst);
			APK_MEDIA_RECORD_UnregisterNotifier(ulSvcUid, rest_bind_record_NotifierApkRecord, (void *)pstContext);

			szResult = rest_bind_MakeErrorString(REST_ERR_CODE_CMD_FAILED, REST_ERR_STRING_CMD_FAILED, TRUE);
			pstRecInst->pstReqMsg->pvResult = (void *)szResult;
			rest_task_SyncOut();

			HLIB_STD_MemFree(pstRecInst);
			return;
		}

		break;

	default:
		break;
	}

	// Notifier 동작 관련은 여기서 작업한다.
}

STATIC HCHAR *rest_bind_record_MakeReplyRecStartString (RestRecInst_t *pstRecInst, HUINT32 ulContentId, OxMediaRec_StartInfo_t *pstStart)
{
	HINT32					 nJWriter = 0;
	HCHAR					*szResultStr = NULL;
	HINT32					 nErrCode = REST_ERR_CODE_INTERNAL;
	HCHAR					*szErrString = REST_ERR_STRING_INTERNAL;
	DxRecListData_t			*pstRecord = NULL;

	nJWriter = rest_bind_OpenJsonWriter();
	if (0 == nJWriter)
	{
		HxLOG_Error("rest_bind_OpenJsonWriter() err:\n");
		nErrCode = REST_ERR_CODE_NOMEM;		szErrString = REST_ERR_STRING_NOMEM;
		goto END_FUNC;
	}

	pstRecord = APK_META_RECORD_GetByCID(ulContentId);
	if (NULL == pstRecord)
	{
		HxLOG_Error("APK_META_RECORD_GetByCID NULL: ulContentId(%d)\n", ulContentId);
		nErrCode = REST_ERR_CODE_RESOURCE_NOTFOUND;		szErrString = REST_ERR_STRING_RESOURCE_NOTFOUND;
		goto END_FUNC;
	}

	// Jsonize the state
	HxJSONWRITER_ObjectBegin(nJWriter);
	{
		// RecordUid
		(void)HxJSONWRITER_Integer(nJWriter, "recordUid", ulContentId);

		// Play Status
		(void)HxJSONWRITER_StringValue(nJWriter, "record");
		(void)rest_datatype_JsonizeRecord(nJWriter, pstRecord, NULL);

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

	if (NULL != pstRecord)
	{
		APK_META_RECORD_Release(pstRecord);
	}

	if (NULL == szResultStr)
	{
		szResultStr = rest_bind_MakeErrorString(nErrCode, szErrString, TRUE);
	}

	return szResultStr;
}

STATIC HBOOL rest_bind_record_CbCmpRecordVectors (const void *pvVectorItem, const void *pvKey)
{
	DxRecListData_t		*pstRecData = (DxRecListData_t *)pvVectorItem;
	RestRecGetFilter_t	*pstFilter = (RestRecGetFilter_t *)pvKey;

	if ((NULL != pstRecData) && (NULL != pstFilter))
	{
		if (pstRecData->ulContentId >= pstFilter->nStartUid)
		{
			if ((pstFilter->nEndUid >= 0) && (pstRecData->ulContentId > pstFilter->nEndUid))
			{
				return FALSE;
			}

			if ((pstFilter->nRecCnt >= 0) && (pstFilter->nListCnt >= pstFilter->nRecCnt))
			{
				return FALSE;
			}

			pstFilter->nListCnt++;
			return TRUE;
		}
	}

	return FALSE;
}

STATIC HBOOL rest_bind_record_CbGetRecordsBriefFromVector (const void *pvVectorItem, const void *pvKey)
{
	DxRecListData_t			*pstRecData = (DxRecListData_t *)pvVectorItem;
	RestRecGetBriefInfo_t	*pstBrief = (RestRecGetBriefInfo_t *)pvKey;

	if ((NULL != pstRecData) && (NULL != pstBrief))
	{
		if (pstBrief->nMinRecUid < 0)
		{
			pstBrief->nMinRecUid = (HINT32)pstRecData->ulContentId;
			pstBrief->nMaxRecUid = (HINT32)pstRecData->ulContentId;
		}
		else if (pstBrief->nMinRecUid > pstRecData->ulContentId)
		{
			pstBrief->nMinRecUid = (HINT32)pstRecData->ulContentId;
		}
		else if (pstBrief->nMaxRecUid < pstRecData->ulContentId)
		{
			pstBrief->nMaxRecUid = (HINT32)pstRecData->ulContentId;
		}

		pstBrief->nRecNum ++;
	}

	return FALSE;
}

STATIC HBOOL rest_bind_record_CbWriteRecordsToJson (void *pvData, void *pvUserData)
{
	DxRecListData_t			*pstRecord = (DxRecListData_t *)pvData;
	RestRecVectorWrite_t	*pstWrite = (RestRecVectorWrite_t *)pvUserData;

	if ((NULL != pstRecord) && (NULL != pstWrite))
	{
		(void)rest_datatype_JsonizeRecord(pstWrite->nJWriter, pstRecord, pstWrite->szMember);
	}

	return FALSE;
}

#define _____COMMAND_FUNCTIONS_____
STATIC HERROR rest_bind_record_CmdRegisterNotifier (RestBind_Notifier_t pfNotifier)
{
	if (HLIB_LIST_Find(s_pstRestBindRec_Notifier, (const void *)pfNotifier) != NULL)
	{
		HxLOG_Error("Already the same notifier exists:\n");
		return ERR_OK;
	}

	s_pstRestBindRec_Notifier = HLIB_LIST_Append(s_pstRestBindRec_Notifier, (void *)pfNotifier);
	return ERR_OK;
}

STATIC HERROR rest_bind_record_CbRegisterNotifier (void *pvArg)
{
	RestMsg_t			*pstMsg = (RestMsg_t *)pvArg;
	RestBind_Notifier_t	 pfNotifier = (RestBind_Notifier_t)pstMsg->aulArgs[0];
	HERROR				 hErr = ERR_FAIL;

	if (NULL != pfNotifier)
	{
		hErr = rest_bind_record_CmdRegisterNotifier(pfNotifier);
	}

	pstMsg->pvResult = (void *)hErr;
	rest_task_SyncOut();

	return ERR_OK;
}

STATIC HERROR rest_bind_record_CmdUnregisterNotifier (RestBind_Notifier_t pfNotifier)
{
	s_pstRestBindRec_Notifier = HLIB_LIST_Remove(s_pstRestBindRec_Notifier, (const void *)pfNotifier);
	return ERR_OK;
}

STATIC HERROR rest_bind_record_CbUnregisterNotifier (void *pvArg)
{
	RestMsg_t			*pstMsg = (RestMsg_t *)pvArg;
	RestBind_Notifier_t	 pfNotifier = (RestBind_Notifier_t)pstMsg->aulArgs[0];
	HERROR				 hErr = ERR_FAIL;

	if (NULL != pfNotifier)
	{
		hErr = rest_bind_record_CmdUnregisterNotifier(pfNotifier);
	}

	pstMsg->pvResult = (void *)hErr;
	rest_task_SyncOut();

	return ERR_OK;
}

STATIC HCHAR *rest_bind_record_CmdStartRecordChannel (RestMsg_t *pstMsg, RestRecRequest_t *pstRecRequest)
{
	HBOOL				 bRecNotifier = FALSE;
	HCHAR				 szRecordId[64];
	HUINT32				 ulSessionId, ulSvcUid = pstRecRequest->unArgs.stChanRec.ulMasterSvcUid;
	RestRecInst_t		*pstRecInst;
	RestRecContext_t	*pstContext = rest_bind_record_GetContext();
	HINT32				 nErrCode = REST_ERR_CODE_INTERNAL;
	HCHAR				*szErrStr = REST_ERR_STRING_INTERNAL;
	HERROR				 hErr;

	hErr = APK_MEDIA_RECORD_RegisterNotifier(ulSvcUid, rest_bind_record_NotifierApkRecord, (void *)pstContext);
	if (ERR_OK == hErr)
	{
		bRecNotifier = TRUE;
	}

	hErr = APK_MEDIA_RECORD_Start(&(pstRecRequest->unArgs.stChanRec), &ulSessionId, szRecordId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("APK_MEDIA_RECORD_Start err:\n");
		nErrCode = REST_ERR_CODE_CMD_FAILED;		szErrStr = REST_ERR_STRING_CMD_FAILED;
		goto END_FUNC;
	}

	pstRecInst = (RestRecInst_t *)HLIB_STD_MemAlloc(sizeof(RestRecInst_t));
	if (NULL == pstRecInst)
	{
		HxLOG_Error("pstRequest memory allocation failed:\n");
		nErrCode = REST_ERR_CODE_NOMEM;			szErrStr = REST_ERR_STRING_NOMEM;
		goto END_FUNC;
	}

	pstRecInst->ulSessionId	= ulSessionId;
	pstRecInst->pstReqMsg	= pstMsg;
	pstRecInst->eState		= eREST_REC_STATE_Starting;

	pstContext->pstRecInstList = HLIB_LIST_Append(pstContext->pstRecInstList, (void *)pstRecInst);

	// Success: wait until the appkit replied the results
	return REST_REC_WAIT_UNTIL_SERVICE_REPLIED;

END_FUNC:
	if (TRUE == bRecNotifier)			{ APK_MEDIA_RECORD_UnregisterNotifier(ulSvcUid, rest_bind_record_NotifierApkRecord, (void *)pstContext); }
	return rest_bind_MakeErrorString(nErrCode, szErrStr, TRUE);
}

STATIC HCHAR *rest_bind_record_CmdGetRecords (HINT32 nStartUid, HINT32 nEndUid, HINT32 nRecCnt, HCHAR *szMember)
{
	HINT32				 nJWriter = 0;
	HxVector_t			*pstRecVectors = NULL;
	RestRecGetFilter_t	 stGetFilter;
	HINT32				 nErrCode;
	HCHAR				*szErrString;
	HCHAR				*szResultStr = NULL;

	stGetFilter.nStartUid	= nStartUid;
	stGetFilter.nEndUid		= nEndUid;
	stGetFilter.nRecCnt		= nRecCnt;
	stGetFilter.nListCnt	= 0;

	pstRecVectors = APK_META_RECORD_GetRecordings(rest_bind_record_CbCmpRecordVectors, (void *)&stGetFilter);
	if (NULL == pstRecVectors)
	{
		HxLOG_Error("APK_META_RECORD_GetRecordings NULL\n");
		nErrCode = REST_ERR_CODE_NODATA;		szErrString = REST_ERR_STRING_NODATA;
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
		// Records Array
		(void)HxJSONWRITER_Array(nJWriter, "record");
		{
			RestRecVectorWrite_t		 stWrite;

			stWrite.nJWriter = nJWriter;
			stWrite.szMember = szMember;

			(void)HLIB_VECTOR_ForEach(pstRecVectors, rest_bind_record_CbWriteRecordsToJson, (void *)&stWrite);

		}
		(void)HxJSONWRITER_ArrayEnd(nJWriter);

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

	if (NULL != pstRecVectors)
	{
		HLIB_VECTOR_Delete(pstRecVectors);
	}

	if (NULL == szResultStr)
	{
		szResultStr = rest_bind_MakeErrorString(nErrCode, szErrString, TRUE);
	}

	return szResultStr;
}

STATIC HCHAR *rest_bind_record_CmdGetRecordsBrief (HCHAR *szMember)
{
	HINT32					 nJWriter = 0;
	RestRecGetBriefInfo_t	 stBrief;
	HINT32					 nErrCode;
	HCHAR					*szErrString;
	HCHAR					*szResultStr = NULL;

	HxSTD_MemSet(&stBrief, 0, sizeof(RestRecGetBriefInfo_t));
	stBrief.nMinRecUid	= -1;
	stBrief.nMaxRecUid	= -1;

	(void)APK_META_RECORD_GetRecordings(rest_bind_record_CbGetRecordsBriefFromVector, (void *)&stBrief);
	if (stBrief.nRecNum <= 0)
	{
		HxLOG_Error("No Records...\n");
		nErrCode = REST_ERR_CODE_NODATA;		szErrString = REST_ERR_STRING_NODATA;
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
		// Records Status
		if (rest_datatype_CheckWriteSelected("recordsStatus", szMember))
		{
			(void)HxJSONWRITER_Object(nJWriter, "recordsStatus");
			{
				if (stBrief.nRecNum > 0)
				{
					(void)rest_datatype_SetJsonInteger(nJWriter, "minRecordUid", stBrief.nMinRecUid, NULL);
					(void)rest_datatype_SetJsonInteger(nJWriter, "maxRecordUid", stBrief.nMaxRecUid, NULL);
				}

				(void)rest_datatype_SetJsonInteger(nJWriter, "recordNumbers", stBrief.nRecNum, NULL);
			}
			(void)HxJSONWRITER_ObjectEnd(nJWriter);
		}

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

STATIC HCHAR *rest_bind_record_CmdDeleteRecord (HUINT32 ulRecordUid)
{
	HINT32					 nJWriter = 0;
	APK_META_REC_DELETE_t	 stRecDelete;
	DxRecListData_t			*pstRecord = NULL;
	HINT32					 nErrCode;
	HCHAR					*szErrString;
	HCHAR					*szResultStr = NULL;
	HERROR					 hErr;

	HxSTD_MemSet(&stRecDelete, 0, sizeof(APK_META_REC_DELETE_t));

	pstRecord = APK_META_RECORD_GetByCID(ulRecordUid);
	if (NULL == pstRecord)
	{
		HxLOG_Error("APK_META_RECORD_GetByCID NULL: ulRecordUid(%d)\n", ulRecordUid);
		nErrCode = REST_ERR_CODE_NODATA; 	szErrString = REST_ERR_STRING_NODATA;
		goto END_FUNC;
	}

	stRecDelete.ulParamCount	= 1;
	stRecDelete.paramList		= (APK_META_REC_PARAM_t *)HLIB_STD_MemCalloc(sizeof(APK_META_REC_PARAM_t) * stRecDelete.ulParamCount);
	if (NULL == stRecDelete.paramList)
	{
		HxLOG_Error("stRecDelete.paramList allocation failed:");
		nErrCode = REST_ERR_CODE_NOMEM; 	szErrString = REST_ERR_STRING_NOMEM;
		goto END_FUNC;
	}

	HxSTD_StrNCpy(stRecDelete.paramList[0].szUrl, pstRecord->szUrl, DxRECLIST_URL);

	hErr = APK_META_RECORD_Delete(&stRecDelete, FALSE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("APK_META_RECORD_Delete failed:\n");
		nErrCode = REST_ERR_CODE_INTERNAL; 	szErrString = REST_ERR_STRING_INTERNAL;
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
		// RecordUid
		(void)HxJSONWRITER_Integer(nJWriter, "recordUid", pstRecord->ulContentId);

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

	if (NULL != stRecDelete.paramList)
	{
		HLIB_STD_MemFree(stRecDelete.paramList);
	}

	if (NULL != pstRecord)
	{
		APK_META_RECORD_Release(pstRecord);
	}

	if (NULL == szResultStr)
	{
		szResultStr = rest_bind_MakeErrorString(nErrCode, szErrString, TRUE);
	}

	return szResultStr;
}

STATIC HERROR rest_bind_record_CbPost (void *pvArg)
{
	RestRecRequest_t	 stRecRequest;
	HCHAR				*szResult = NULL;
	RestMsg_t			*pstMsg = (RestMsg_t *)pvArg;
	HxJSON_t			 jsPayLoad = NULL;
	HERROR				 hErr;

	jsPayLoad = HLIB_JSON_Decode((HCHAR *)pstMsg->aulArgs[0]);
	if (NULL != jsPayLoad)
	{
		hErr = rest_datatype_DejsonizeRecRequest(jsPayLoad, &stRecRequest, TRUE);
		if (ERR_OK == hErr)
		{
			switch (stRecRequest.eSrcType)
			{
			case eREST_REC_SRCTYPE_Channel:
				szResult = rest_bind_record_CmdStartRecordChannel(pstMsg, &stRecRequest);
				break;

			default:
				szResult = rest_bind_MakeErrorString(REST_ERR_CODE_WRONG_SOURCETYPE, REST_ERR_STRING_WRONG_SOURCETYPE, TRUE);
				break;
			}
		}
		else
		{
			szResult = rest_bind_MakeErrorString(REST_ERR_CODE_WRONG_SOURCETYPE, REST_ERR_STRING_WRONG_SOURCETYPE, TRUE);
		}

		HLIB_JSON_Delete(jsPayLoad);
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

	return ERR_OK;
}

STATIC HERROR rest_bind_record_CbGet (void *pvArg)
{
	HINT32				 nStartUid = -1, nEndUid = -1, nRecCnt = -1;
	HUINT32 			 ulContentId;
	HCHAR				*szArgJson = NULL;
	HCHAR				*szResult = NULL, *szMember = NULL;
	RestMsg_t			*pstMsg = (RestMsg_t *)pvArg;
	HxJSON_t			 jsArgs = NULL;

	ulContentId = pstMsg->aulArgs[0];
	szArgJson = (HCHAR *)pstMsg->aulArgs[1];
	jsArgs = HLIB_JSON_Decode(szArgJson);

	if (NULL != jsArgs)
	{
		szMember = (HCHAR *)HLIB_JSON_Object_GetString(jsArgs, "member");
		(void)rest_datatype_GetJsonInteger(jsArgs, "startuid", &nStartUid);
		(void)rest_datatype_GetJsonInteger(jsArgs, "enduid", &nEndUid);
		(void)rest_datatype_GetJsonInteger(jsArgs, "cnt", &nRecCnt);

		if ((HUINT32)-1 == ulContentId)
		{
			if (nStartUid >= 0)
			{
				if ((nEndUid >= nStartUid) || (nRecCnt > 0))
				{
					szResult = rest_bind_record_CmdGetRecords(nStartUid, nEndUid, nRecCnt, szMember);
				}
				else
				{
					szResult = rest_bind_record_CmdGetRecords(nStartUid, nStartUid, -1, szMember);
				}
			}
			else
			{
				szResult = rest_bind_record_CmdGetRecordsBrief(szMember);
			}
		}
		else
		{
			szResult = rest_bind_record_CmdGetRecords((HINT32)ulContentId, (HINT32)ulContentId, -1, szMember);
		}

		HLIB_JSON_Delete(jsArgs);
	}
	else
	{
		szResult = rest_bind_MakeErrorString(REST_ERR_CODE_REQUIRE_MANDATORYKEY, REST_ERR_STRING_REQUIRE_MANDATORYKEY, TRUE);
	}


	pstMsg->pvResult = (void *)szResult;
	rest_task_SyncOut();

	return ERR_OK;
}

STATIC HERROR rest_bind_record_CbDelete (void *pvArg)
{
	HUINT32 			 ulContentId;
	HCHAR				*szResult = NULL;
	RestMsg_t			*pstMsg = (RestMsg_t *)pvArg;

	ulContentId = pstMsg->aulArgs[0];

	szResult = rest_bind_record_CmdDeleteRecord(ulContentId);

	pstMsg->pvResult = (void *)szResult;
	rest_task_SyncOut();

	return ERR_OK;
}

#define ___MEMBER_FUNCTIONS___
HERROR rest_bind_record_Init (void)
{
	STATIC HBOOL		 s_bInited = FALSE;

	if (TRUE != s_bInited)
	{
		RestRecContext_t	*pstContext = rest_bind_record_GetContext();

		HxSTD_MemSet(pstContext, 0, sizeof(RestRecContext_t));

		(void)APK_META_RECORD_Init();
		(void)APK_META_SCHEDULE_RegisterNotifier(rest_bind_record_NotifierApkSchedule, (void *)pstContext);

		s_bInited = TRUE;
	}

	return ERR_OK;
}

#define ___API_FUNCTIONS___
int REST_BIND_RECORD_RegisterNotifier (RestBind_Notifier_t pfNotifier)
{
	RestMsg_t			 stMsg;
	HERROR				 hErr;

	stMsg.pvResult		= (void *)0;
	stMsg.aulArgs[0]	= (HUINT32)pfNotifier;

	hErr = rest_task_RequestToExecute(rest_bind_record_CbRegisterNotifier, (void *)&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("rest_task_RequestToExecute err:\n");
		return ERR_FAIL;
	}

	rest_task_SyncIn();

	return (int)stMsg.pvResult;
}

int REST_BIND_RECORD_UnregisterNotifier (RestBind_Notifier_t pfNotifier)
{
	RestMsg_t			 stMsg;
	HERROR				 hErr;

	stMsg.pvResult		= (void *)0;
	stMsg.aulArgs[0]	= (HUINT32)pfNotifier;

	hErr = rest_task_RequestToExecute(rest_bind_record_CbUnregisterNotifier, (void *)&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("rest_task_RequestToExecute err:\n");
		return ERR_FAIL;
	}

	rest_task_SyncIn();

	return (int)stMsg.pvResult;
}

// POST http://127.0.0.1:37564/stb-api/v1/records
char *REST_BIND_RECORD_Post (char *szSessionId, char *szPayload)
{
	RestMsg_t			 stMsg;
	char				*szResult = NULL;
	HERROR				 hErr;

	stMsg.szSessionId	= REST_DUMMY_SESSION_ID;
	stMsg.pvResult		= NULL;
	stMsg.aulArgs[0]	= (HUINT32)szPayload;

	hErr = rest_task_RequestToExecute(rest_bind_record_CbPost, (void *)&stMsg);
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

// GET http://127.0.0.1:37564/stb-api/v1/records
char *REST_BIND_RECORD_Get (char *szSessionId, char *szArgJson)
{
	RestMsg_t			 stMsg;
	char				*szResult = NULL;
	HERROR				 hErr;

	stMsg.szSessionId	= REST_DUMMY_SESSION_ID;
	stMsg.pvResult		= NULL;
	stMsg.aulArgs[0]	= (HUINT32)-1;
	stMsg.aulArgs[1]	= (HUINT32)szArgJson;

	hErr = rest_task_RequestToExecute(rest_bind_record_CbGet, (void *)&stMsg);
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

// GET http://127.0.0.1:37564/stb-api/v1/records/1
char *REST_BIND_RECORD_GetOne (char *szSessionId, int nRecordUid, char *szArgJson)
{
	RestMsg_t			 stMsg;
	char				*szResult = NULL;
	HERROR				 hErr;

	stMsg.szSessionId	= REST_DUMMY_SESSION_ID;
	stMsg.pvResult		= NULL;
	stMsg.aulArgs[0]	= (HUINT32)nRecordUid;
	stMsg.aulArgs[1]	= (HUINT32)szArgJson;

	hErr = rest_task_RequestToExecute(rest_bind_record_CbGet, (void *)&stMsg);
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

// DELETE http://127.0.0.1:37564/stb-api/v1/records/1
char *REST_BIND_RECORD_DeleteOne (char *szSessionId, int nScheduleUid)
{
	RestMsg_t			 stMsg;
	char				*szResult = NULL;
	HERROR				 hErr;

	stMsg.szSessionId	= REST_DUMMY_SESSION_ID;
	stMsg.pvResult		= NULL;
	stMsg.aulArgs[0]	= (HUINT32)nScheduleUid;

	hErr = rest_task_RequestToExecute(rest_bind_record_CbDelete, (void *)&stMsg);
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


