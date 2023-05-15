/*******************************************************************
	File Description
********************************************************************/
/**
	@file	 rest_bind_channel.c
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
#include <dapi.h>
#include <thapi.h>
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
	eREST_CHAN_GETTYPE_None = 0,

	eREST_CHAN_GETTYPE_SvcLcn,
	eREST_CHAN_GETTYPE_Index,

	eREST_CHAN_GETTYPE_EndOfCase
} restBindChanGetType_t;

typedef struct
{
	DxSvcType_e		 eSvcType;
	restBindChanGetType_t	 eGetType;
	HINT32			 nStart, nEnd, nRequestedCnt;
	HINT32			 nGetNumber, nIndexCnt;
	HCHAR			*szMember;

	HINT32			 nJsonWriter;
} restBindChanCbInfo_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HBOOL						 s_stRestBindChan_Inited = FALSE;
STATIC RestBindChanContext_t		 s_stRestBindChan_Context;
STATIC HxList_t						*s_pstRestBindChan_Notifier = NULL;

#define ___LOCAL_FUNCTIONS___
STATIC HBOOL rest_bind_chan_cbWriteChannelBySvcLcn (void *data, void *userdata)
{
	STATIC RestChanExtInfo_t	*s_pstExtInfo = NULL;
	DxService_t 			*pstSvc = (DxService_t *)data;
	restBindChanCbInfo_t	*pstCbInfo = (restBindChanCbInfo_t *)userdata;

	if ((NULL != pstCbInfo) && (NULL != pstSvc))
	{
		if ((eDxSVC_TYPE_All != pstCbInfo->eSvcType) && (pstSvc->svcType != pstCbInfo->eSvcType))
		{
			return FALSE;
		}

		if (pstCbInfo->nGetNumber >= pstCbInfo->nRequestedCnt)
		{
			return TRUE;
		}
		else if ((pstCbInfo->nEnd >= 0) && (pstSvc->lcn > pstCbInfo->nEnd))
		{
			return FALSE;
		}
		else if (pstSvc->lcn >= pstCbInfo->nStart)
		{
			if (NULL == s_pstExtInfo)
			{
				s_pstExtInfo = (RestChanExtInfo_t *)HLIB_STD_MemAlloc(sizeof(RestChanExtInfo_t));
			}

			if (NULL != s_pstExtInfo)
			{
				HxSTD_MemSet(s_pstExtInfo, 0, sizeof(RestChanExtInfo_t));

				// Insert the thumbnail data:
				{
					STATIC DxChannelLogo_t		*s_pstChLogo = NULL;
					HCHAR						*szFileName = NULL, *szFileFullUrl = NULL;

					if (NULL == s_pstChLogo)
					{
						s_pstChLogo = (DxChannelLogo_t *)HLIB_STD_MemAlloc(sizeof(DxChannelLogo_t));
					}

					if (NULL != s_pstChLogo)
					{

						HxSTD_MemSet(s_pstChLogo, 0, sizeof(DxChannelLogo_t));

						(void)APK_META_SVC_GetLogoUrl(pstSvc->uid, s_pstChLogo);
#if defined(CONFIG_SUPPORT_IPEPG_CH_LOGO_STORE_HDD)
						szFileFullUrl = (HCHAR*)THAPI_GetThumbnailURL(s_pstChLogo->ucServerLogoUrl, 0, "/mnt/hd1", FALSE);
#else
						szFileFullUrl = (HCHAR*)THAPI_GetThumbnailURL(s_pstChLogo->ucServerLogoUrl, 0, "/var/lib/humaxtv_user", FALSE);
#endif
						szFileName = HLIB_STD_GetFileNameFromUrl(szFileFullUrl);

						if (NULL != szFileName)
						{
							HxSTD_snprintf(s_pstExtInfo->stThumbnailInfo.szMimeType, REST_MEDIA_MIME_LEN, "image/png" );
							HxSTD_snprintf(s_pstExtInfo->stThumbnailInfo.szThumbnailUrl, REST_MEDIA_URL_LEN, "/thumbnails/channel/%s", szFileName);
						}
					}
				}
			}

			(void)rest_datatype_JsonizeService(pstCbInfo->nJsonWriter, pstSvc, s_pstExtInfo, pstCbInfo->szMember);
			pstCbInfo->nGetNumber++;
		}
	}

	return FALSE;
}

STATIC HBOOL rest_bind_chan_cbWriteChannelByIndex (void *data, void *userdata)
{
	STATIC RestChanExtInfo_t	*s_pstExtInfo = NULL;
	DxService_t 			*pstSvc = (DxService_t *)data;
	restBindChanCbInfo_t	*pstCbInfo = (restBindChanCbInfo_t *)userdata;

	if ((NULL != pstCbInfo) && (NULL != pstSvc))
	{
		if ((eDxSVC_TYPE_All != pstCbInfo->eSvcType) && (pstSvc->svcType != pstCbInfo->eSvcType))
		{
			return FALSE;
		}

		if (pstCbInfo->nGetNumber >= pstCbInfo->nRequestedCnt)
		{
			return TRUE;
		}
		else if ((pstCbInfo->nEnd >= 0) && (pstSvc->uid > pstCbInfo->nEnd))
		{
			return FALSE;
		}
		else if (pstSvc->uid >= pstCbInfo->nStart)
		{
			if (NULL == s_pstExtInfo)
			{
				s_pstExtInfo = (RestChanExtInfo_t *)HLIB_STD_MemAlloc(sizeof(RestChanExtInfo_t));
			}

			if (NULL != s_pstExtInfo)
			{
				HxSTD_MemSet(s_pstExtInfo, 0, sizeof(RestChanExtInfo_t));

				// Insert the thumbnail data:
				{
					STATIC DxChannelLogo_t		*s_pstChLogo = NULL;
					HCHAR						*szFileName = NULL, *szFileFullUrl = NULL;

					if (NULL == s_pstChLogo)
					{
						s_pstChLogo = (DxChannelLogo_t *)HLIB_STD_MemAlloc(sizeof(DxChannelLogo_t));
					}

					if (NULL != s_pstChLogo)
					{

						HxSTD_MemSet(s_pstChLogo, 0, sizeof(DxChannelLogo_t));

						(void)APK_META_SVC_GetLogoUrl(pstSvc->uid, s_pstChLogo);
#if defined(CONFIG_SUPPORT_IPEPG_CH_LOGO_STORE_HDD)
						szFileFullUrl = (HCHAR*)THAPI_GetThumbnailURL(s_pstChLogo->ucServerLogoUrl, 0, "/mnt/hd1", FALSE);
#else
						szFileFullUrl = (HCHAR*)THAPI_GetThumbnailURL(s_pstChLogo->ucServerLogoUrl, 0, "/var/lib/humaxtv_user", FALSE);
#endif
						szFileName = HLIB_STD_GetFileNameFromUrl(szFileFullUrl);

						if (NULL != szFileName)
						{
							HxSTD_snprintf(s_pstExtInfo->stThumbnailInfo.szMimeType, REST_MEDIA_MIME_LEN, "image/png" );
							HxSTD_snprintf(s_pstExtInfo->stThumbnailInfo.szThumbnailUrl, REST_MEDIA_URL_LEN, "/thumbnails/channel/%s", szFileName);
						}
					}
				}
			}

			(void)rest_datatype_JsonizeService(pstCbInfo->nJsonWriter, pstSvc, s_pstExtInfo, pstCbInfo->szMember);
			pstCbInfo->nGetNumber++;
		}

		pstCbInfo->nIndexCnt++;
	}

	return FALSE;
}

STATIC HBOOL rest_bind_chan_CbMakeChanListStatus (void *data,void *userdata)
{
	DxService_t 			*pstSvc = (DxService_t *)data;
	RestChanListStatus_t	*pstStatus = (RestChanListStatus_t *)userdata;

	if ((NULL != pstStatus) && (NULL != pstSvc))
	{
		pstStatus->ulSvcAllNum ++;

		switch (pstSvc->svcType)
		{
		case eDxSVC_TYPE_TV:
			pstStatus->ulSvcTvNum ++;
			if (pstStatus->nMaxTvLcn < 0)
			{
				pstStatus->nMinTvLcn = pstSvc->lcn;
				pstStatus->nMaxTvLcn = pstSvc->lcn;
			}
			else
			{
				if (pstStatus->nMinTvLcn > pstSvc->lcn)
				{
					pstStatus->nMinTvLcn = pstSvc->lcn;
				}
				else if (pstStatus->nMaxTvLcn < pstSvc->lcn)
				{
					pstStatus->nMaxTvLcn = pstSvc->lcn;
				}
			}

			break;

		case eDxSVC_TYPE_RADIO:
			pstStatus->ulSvcRadioNum ++;
			if (pstStatus->nMaxRadioLcn < 0)
			{
				pstStatus->nMinRadioLcn = pstSvc->lcn;
				pstStatus->nMaxRadioLcn = pstSvc->lcn;
			}
			else
			{
				if (pstStatus->nMinRadioLcn > pstSvc->lcn)
				{
					pstStatus->nMinRadioLcn = pstSvc->lcn;
				}
				else if (pstStatus->nMaxRadioLcn < pstSvc->lcn)
				{
					pstStatus->nMaxRadioLcn = pstSvc->lcn;
				}
			}

			break;

		default:
			break;
		}
	}

	return FALSE;
}

STATIC HBOOL rest_bind_chan_CbAppendSvcPtrToVector (void *data, void *userdata)
{
	DxService_t 			*pstSvc = (DxService_t *)data;
	HxVector_t				*pstSvcVector = (HxVector_t *)userdata;

	if ((NULL != pstSvc) && (NULL != pstSvcVector))
	{
		(void)HLIB_VECTOR_Add(pstSvcVector, (void *)pstSvc);
	}

	return FALSE;
}

STATIC HBOOL rest_bind_chan_CbMakeTsListStatus (void *data,void *userdata)
{
	DxTransponder_t 		*pstTs = (DxTransponder_t *)data;
	RestTsListStatus_t		*pstStatus = (RestTsListStatus_t *)userdata;

	if ((NULL != pstStatus) && (NULL != pstTs))
	{
		pstStatus->ulTsInfoNum ++;
	}

	return FALSE;
}

STATIC HBOOL rest_bind_chan_cbWriteTransponderByIndex (void *data, void *userdata)
{
	DxTransponder_t 		*pstTs = (DxTransponder_t *)data;
	restBindChanCbInfo_t	*pstCbInfo = (restBindChanCbInfo_t *)userdata;

	if ((NULL != pstCbInfo) && (NULL != pstTs))
	{
		if (pstCbInfo->nGetNumber >= pstCbInfo->nRequestedCnt)
		{
			return TRUE;
		}
		else if (pstCbInfo->nIndexCnt >= pstCbInfo->nStart)
		{
			(void)rest_datatype_JsonizeTransponder(pstCbInfo->nJsonWriter, pstTs, pstCbInfo->szMember);
			pstCbInfo->nGetNumber++;
		}

		pstCbInfo->nIndexCnt++;
	}

	return FALSE;
}

STATIC HBOOL rest_bind_chan_CbMakeAntListStatus (void *data,void *userdata)
{
	DxAntInfo_t 			*pstAnt = (DxAntInfo_t *)data;
	RestAntListStatus_t		*pstStatus = (RestAntListStatus_t *)userdata;

	if ((NULL != pstStatus) && (NULL != pstAnt))
	{
		pstStatus->ulAntInfoNum ++;

		if ((pstStatus->nMinAntUid < 0) || (pstStatus->nMinAntUid > pstAnt->uid))
		{
			pstStatus->nMinAntUid = pstAnt->uid;
		}

		if ((pstStatus->nMaxAntUid < 0) || (pstStatus->nMaxAntUid < pstAnt->uid))
		{
			pstStatus->nMaxAntUid = pstAnt->uid;
		}
	}

	return FALSE;
}

STATIC HBOOL rest_bind_chan_cbWriteAntennaByIndex (void *data, void *userdata)
{
	DxAntInfo_t 			*pstAnt = (DxAntInfo_t *)data;
	restBindChanCbInfo_t	*pstCbInfo = (restBindChanCbInfo_t *)userdata;

	if ((NULL != pstCbInfo) && (NULL != pstAnt))
	{
		if (pstCbInfo->nGetNumber >= pstCbInfo->nRequestedCnt)
		{
			return TRUE;
		}
		else if (pstCbInfo->nIndexCnt >= pstCbInfo->nStart)
		{
			(void)rest_datatype_JsonizeAntenna(pstCbInfo->nJsonWriter, pstAnt, pstCbInfo->szMember);
			pstCbInfo->nGetNumber++;
		}

		pstCbInfo->nIndexCnt++;
	}

	return FALSE;
}

#if 0
STATIC HBOOL rest_bind_chan_CbPrintSvc (void *data, void *userdata)
{
	DxService_t 			*pstSvc = (DxService_t *)data;

	if (NULL != pstSvc)
	{
		printf(">>> SVC UID=%d, No=%d, Name=[%s]\n", pstSvc->uid, pstSvc->lcn, pstSvc->name);
	}

	return FALSE;
}
#endif

STATIC HINT32 rest_bind_chan_CbCmpSvcByLcn (const void *pvData1, const void *pvData2)
{
	DxService_t				*pstSvc1 = *(DxService_t **)pvData1;
	DxService_t				*pstSvc2 = *(DxService_t **)pvData2;

	if ((NULL != pstSvc1) && (NULL != pstSvc2))
	{
		return pstSvc1->lcn - pstSvc2->lcn;
	}

	return 0;
}


#define _____NOTIFIER_FUNCTIONS_____

#define _____COMMAND_FUNCTIONS_____
STATIC HERROR rest_bind_chan_CmdRegisterNotifier (RestBind_Notifier_t pfNotifier)
{
	if (HLIB_LIST_Find(s_pstRestBindChan_Notifier, (const void *)pfNotifier) != NULL)
	{
		HxLOG_Error("Already the same notifier exists:\n");
		return ERR_OK;
	}

	s_pstRestBindChan_Notifier = HLIB_LIST_Append(s_pstRestBindChan_Notifier, (void *)pfNotifier);
	return ERR_OK;
}

STATIC HERROR rest_bind_chan_CbRegisterNotifier (void *pvArg)
{
	RestMsg_t			*pstMsg = (RestMsg_t *)pvArg;
	RestBind_Notifier_t	 pfNotifier = (RestBind_Notifier_t)pstMsg->aulArgs[0];
	HERROR				 hErr = ERR_FAIL;

	if (NULL != pfNotifier)
	{
		hErr = rest_bind_chan_CmdRegisterNotifier(pfNotifier);
	}

	pstMsg->pvResult = (void *)hErr;
	rest_task_SyncOut();

	return ERR_OK;
}

STATIC HERROR rest_bind_chan_CmdUnregisterNotifier (RestBind_Notifier_t pfNotifier)
{
	s_pstRestBindChan_Notifier = HLIB_LIST_Remove(s_pstRestBindChan_Notifier, (const void *)pfNotifier);
	return ERR_OK;
}

STATIC HERROR rest_bind_chan_CbUnregisterNotifier (void *pvArg)
{
	RestMsg_t			*pstMsg = (RestMsg_t *)pvArg;
	RestBind_Notifier_t	 pfNotifier = (RestBind_Notifier_t)pstMsg->aulArgs[0];
	HERROR				 hErr = ERR_FAIL;

	if (NULL != pfNotifier)
	{
		hErr = rest_bind_chan_CmdUnregisterNotifier(pfNotifier);
	}

	pstMsg->pvResult = (void *)hErr;
	rest_task_SyncOut();

	return ERR_OK;
}

#define _____CHANNEL_FUNCTIONS_____
STATIC HCHAR *rest_bind_chan_CmdGetChannelListStatus (HCHAR *szSessionId, HCHAR *szMember)
{
	HINT32					 nJWriter = 0;
	HCHAR					*szResultStr = NULL;
	RestBindChanContext_t	*pstContext = rest_bind_chan_GetContext();
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

	(void)HxJSONWRITER_ObjectBegin(nJWriter);
	{
		// chanListStatus
		(void)HxJSONWRITER_StringValue(nJWriter, "chanListStatus");
		(void)rest_datatype_JsonizeChanListStatus(nJWriter, &(pstContext->stChanListStatus), szMember);

		// error
		(void)rest_bind_MakeErrorJsWriter(nJWriter, REST_ERR_CODE_OK, REST_ERR_STRING_OK);
	}
	(void)HxJSONWRITER_ObjectEnd(nJWriter);

	szResultStr = rest_bind_CloseJsonWriter(nJWriter);
	return szResultStr;
}

STATIC HCHAR *rest_bind_chan_CmdGetChannels (HCHAR *szSessionId, HCHAR *szListType, restBindChanGetType_t eGetType, HINT32 nStart, HINT32 nEnd, HINT32 nSvcNum, HCHAR *szMember)
{
	HINT32					 nJWriter = 0;
	HCHAR					*szResultStr = NULL;
	restBindChanCbInfo_t	 stCbInfo;
	RestBindChanContext_t	*pstContext = rest_bind_chan_GetContext();
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

	HxSTD_MemSet(&stCbInfo, 0, sizeof(restBindChanCbInfo_t));

	if (NULL == pstContext)
	{
		HxLOG_Error("rest_bind_chan_GetContext() NULL\n");
		return rest_bind_MakeErrorString(REST_ERR_CODE_NOTREADY, REST_ERR_STRING_NOTREADY, TRUE);
	}

	if (HxSTD_StrCmp(szListType, "all") == 0)
	{
		stCbInfo.eSvcType = eDxSVC_TYPE_All;
	}
	else if (HxSTD_StrCmp(szListType, "tv") == 0)
	{
		stCbInfo.eSvcType = eDxSVC_TYPE_TV;
	}
	else if (HxSTD_StrCmp(szListType, "radio") == 0)
	{
		stCbInfo.eSvcType = eDxSVC_TYPE_RADIO;
	}
	else
	{
		stCbInfo.eSvcType = eDxSVC_TYPE_All;
	}

	stCbInfo.eGetType		= eGetType;
	stCbInfo.nStart			= nStart;
	stCbInfo.nEnd			= nEnd;
	stCbInfo.nRequestedCnt	= nSvcNum;
	stCbInfo.szMember		= szMember;

	stCbInfo.nJsonWriter	= nJWriter;

	HxJSONWRITER_ObjectBegin(nJWriter);
	{
		// Channels:
		if ((NULL != pstContext->pstSvcVector) && (HLIB_VECTOR_Length(pstContext->pstSvcVector) > 0))
		{
			HxJSONWRITER_Array(nJWriter, "Channels");

			switch (eGetType)
			{
			case eREST_CHAN_GETTYPE_SvcLcn:
				HLIB_VECTOR_ForEach(pstContext->pstSvcLcnVector, rest_bind_chan_cbWriteChannelBySvcLcn, (void *)&stCbInfo);
				break;

			case eREST_CHAN_GETTYPE_Index:
				HLIB_VECTOR_ForEach(pstContext->pstSvcVector, rest_bind_chan_cbWriteChannelByIndex, (void *)&stCbInfo);
				break;

			default:
				break;
			}

			HxJSONWRITER_ArrayEnd(nJWriter);
		}

		// Error:
		if (stCbInfo.nGetNumber > 0)
		{
			(void)rest_bind_MakeErrorJsWriter(nJWriter, REST_ERR_CODE_OK, REST_ERR_STRING_OK);
		}
		else
		{
			(void)rest_bind_MakeErrorJsWriter(nJWriter, REST_ERR_CODE_NODATA, REST_ERR_STRING_NODATA);
		}
	}
	HxJSONWRITER_ObjectEnd(nJWriter);

	szResultStr = rest_bind_CloseJsonWriter(nJWriter);
	return szResultStr;
}

STATIC HERROR rest_bind_chan_CbGetChannels (void *pvArg)
{
	HINT32			 nStartNo = -1, nStartIdx = -1, nEndNo = -1, nEndIdx = -1, nCnt = -1;
	HCHAR			*szListType = NULL, *szDataType = NULL;
	HCHAR			*szMember = NULL;
	RestMsg_t		*pstMsg = (RestMsg_t *)pvArg;
	HxJSON_t		 jsArgs = NULL;
	HCHAR			*szResult = NULL;
	HERROR			 hResult = ERR_FAIL;

	if (NULL == pstMsg)					{ goto END_FUNC; }

	jsArgs = HLIB_JSON_Decode((HCHAR *)pstMsg->aulArgs[0]);
	if (NULL != jsArgs)
	{
		(void)rest_bind_GetStringValueFromJson(jsArgs, "listtype", &szListType);
		(void)rest_bind_GetStringValueFromJson(jsArgs, "member", &szDataType);
		(void)rest_bind_GetNumberValueFromJson(jsArgs, "startno", &nStartNo);
		(void)rest_bind_GetNumberValueFromJson(jsArgs, "startidx", &nStartIdx);
		(void)rest_bind_GetNumberValueFromJson(jsArgs, "endno", &nEndNo);
		(void)rest_bind_GetNumberValueFromJson(jsArgs, "endidx", &nEndIdx);
		(void)rest_bind_GetNumberValueFromJson(jsArgs, "cnt", &nCnt);
		(void)rest_bind_GetStringValueFromJson(jsArgs, "member", &szMember);

		if (nCnt <= 0)
		{
			if ((nStartNo >= 0) && (nStartIdx < 0) && (nEndNo < 0) && (nEndIdx >= 0))
			{
				// startno ~ endidx 가 argument로 들어간 경우
				szResult = rest_bind_MakeErrorString(REST_ERR_CODE_INVALID_KEY, REST_ERR_STRING_INVALID_KEY, TRUE);
				pstMsg->pvResult = (void *)szResult;
				hResult = ERR_OK;
				goto END_FUNC;
			}
			else if ((nStartNo < 0) && (nStartIdx >= 0) && (nEndNo >= 0) && (nEndIdx < 0))
			{
				// startidx ~ endno 가 argument로 들어간 경우
				szResult = rest_bind_MakeErrorString(REST_ERR_CODE_INVALID_KEY, REST_ERR_STRING_INVALID_KEY, TRUE);
				pstMsg->pvResult = (void *)szResult;
				hResult = ERR_OK;
				goto END_FUNC;
			}
		}

		if ((nCnt < 1) || (nCnt > REST_BIND_CHANNEL_TRANSFER_MAX))
		{
			nCnt = REST_BIND_CHANNEL_TRANSFER_MAX;
		}

		if (NULL == szListType)
		{
			szListType = "all";
		}
	}

	if (nStartNo >= 0)
	{
		szResult = rest_bind_chan_CmdGetChannels(pstMsg->szSessionId, szListType, eREST_CHAN_GETTYPE_SvcLcn, nStartNo, nEndNo, nCnt, szMember);
	}
	else if (nStartIdx >= 0)
	{
		szResult = rest_bind_chan_CmdGetChannels(pstMsg->szSessionId, szListType, eREST_CHAN_GETTYPE_Index, nStartIdx, nEndIdx, nCnt, szMember);
	}
	else
	{
		szResult = rest_bind_chan_CmdGetChannelListStatus(pstMsg->szSessionId, szMember);
	}

	pstMsg->pvResult = (void *)szResult;
	hResult = ERR_OK;

END_FUNC:
	if (NULL != jsArgs)
	{
		HLIB_JSON_Delete(jsArgs);
	}

	rest_task_SyncOut();
	return hResult;
}

STATIC HERROR rest_bind_chan_CbGetOneChannel (void *pvArg)
{
	HINT32				 nSvcUid;
	HINT32				 nJWriter = 0;
	HUINT32				 ulLsNum = 0;
	HCHAR				*szJsArgs = NULL;
	HCHAR				*szMember = NULL;
	RestMsg_t			*pstMsg = (RestMsg_t *)pvArg;
	DxService_t			*pstSvc = NULL;
	RestChannelExt_t	*pstChanExt = NULL;
	APKS_DLNA_DMS_LiveStreamInfo_t	*pstLsInfo = NULL;
	HxJSON_t			 jsArgs = NULL;
	HCHAR				*szResult = NULL;
	HERROR				 hErr, hResult = ERR_FAIL;

	if (NULL == pstMsg)					{ goto END_FUNC; }

	nJWriter = rest_bind_OpenJsonWriter();
	if (0 == nJWriter)
	{
		HxLOG_Error("rest_bind_OpenJsonWriter() err:\n");
		szResult = rest_bind_MakeErrorString(REST_ERR_CODE_NOMEM, REST_ERR_STRING_NOMEM, TRUE);
		goto END_FUNC;
	}

	nSvcUid = (HINT32)pstMsg->aulArgs[0];
	szJsArgs = (HCHAR *)pstMsg->aulArgs[1];

	pstSvc = APK_META_SVC_GetService(nSvcUid);
	if (NULL == pstSvc)
	{
		HxLOG_Error("APK_META_SVC_GetService err: nSvcUid(%d)\n", nSvcUid);
		szResult = rest_bind_MakeErrorString(REST_ERR_CODE_RESOURCE_NOTFOUND, REST_ERR_STRING_RESOURCE_NOTFOUND, TRUE);
		goto END_FUNC;
	}

	pstChanExt = (RestChannelExt_t *)HLIB_STD_MemCalloc(sizeof(RestChannelExt_t));
	if (NULL == pstChanExt)
	{
		HxLOG_Error("memory allocation failed:\n");
		szResult = rest_bind_MakeErrorString(REST_ERR_CODE_NOMEM, REST_ERR_STRING_NOMEM, TRUE);
		goto END_FUNC;
	}

	HxSTD_MemCopy(&(pstChanExt->stSvcInfo), pstSvc, sizeof(DxService_t));

	hErr = APK_DLNADMS_GetLiveStreamInfo(nSvcUid, &ulLsNum, &pstLsInfo);
	if ((ERR_OK == hErr) && (ulLsNum > 0) && (NULL != pstLsInfo))
	{
		HxSTD_StrNCpy(pstChanExt->stLiveStreamInfo.szMimeType, pstLsInfo[0].szMIME, REST_MEDIA_MIME_LEN);
		HxSTD_StrNCpy(pstChanExt->stLiveStreamInfo.szLiveStreamUrl, pstLsInfo[0].szURI, REST_MEDIA_URL_LEN);
	}

	// TODO: Thumbnail 정보를 임시로 아래와 같이 함.
	{
		HCHAR				*szFileName, *szFileFullUrl;
		DxChannelLogo_t		 stChanLogo;

		APK_META_SVC_GetLogoUrl(nSvcUid, &stChanLogo);
#if defined(CONFIG_SUPPORT_IPEPG_CH_LOGO_STORE_HDD)
		szFileFullUrl = (HCHAR*)THAPI_GetThumbnailURL(stChanLogo.ucServerLogoUrl, 0, "/mnt/hd1", FALSE);
#else
		szFileFullUrl = (HCHAR*)THAPI_GetThumbnailURL(stChanLogo.ucServerLogoUrl, 0, "/var/lib/humaxtv_user", FALSE);
#endif
		szFileName = HLIB_STD_GetFileNameFromUrl(szFileFullUrl);

		if (NULL != szFileName)
		{
			HxSTD_snprintf( pstChanExt->stThumbnailInfo.szMimeType, REST_MEDIA_MIME_LEN, "image/png" );
			HxSTD_snprintf( pstChanExt->stThumbnailInfo.szThumbnailUrl, REST_MEDIA_URL_LEN, "/thumbnails/channel/%s", szFileName);
		}
	}

	jsArgs = HLIB_JSON_Decode(szJsArgs);
	if (NULL != jsArgs)
	{
		(void)rest_bind_GetStringValueFromJson(jsArgs, "member", &szMember);
	}

	// 여기서 직접 JSONIZE 하도록 한다.
	HxJSONWRITER_ObjectBegin(nJWriter);
	{
		// Channels:
		HxJSONWRITER_Array(nJWriter, "Channels");
		{
			(void)rest_datatype_JsonizeChannelExt(nJWriter, pstChanExt, szMember);
		}
		HxJSONWRITER_ArrayEnd(nJWriter);

		// Error:
		(void)rest_bind_MakeErrorJsWriter(nJWriter, REST_ERR_CODE_OK, REST_ERR_STRING_OK);
	}
	HxJSONWRITER_ObjectEnd(nJWriter);

	szResult = rest_bind_CloseJsonWriter(nJWriter);
	hResult = ERR_OK;

END_FUNC:
	pstMsg->pvResult = (void *)szResult;
	if (NULL != jsArgs)
	{
		HLIB_JSON_Delete(jsArgs);
	}

	if (NULL != pstSvc)
	{
		APK_META_SVC_Delete(pstSvc);
	}

	if (NULL != pstChanExt)
	{
		HLIB_STD_MemFree(pstChanExt);
	}

	if (NULL != pstLsInfo)
	{
		APK_DLNADMS_FreeLiveStreamInfo(ulLsNum, pstLsInfo);
	}

	rest_task_SyncOut();
	return hResult;
}
#define _____TRANSPONDERS_FUNCTIONS_____
STATIC HCHAR *rest_bind_chan_CmdGetTsListStatus (HCHAR *szSessionId)
{
	HINT32					 nJWriter = 0;
	HCHAR					*szResultStr = NULL;
	RestBindChanContext_t	*pstContext = rest_bind_chan_GetContext();
	RestInstance_t			*pstInst;
	RestTsListStatus_t		 stTsStatus;

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

	HxSTD_MemSet(&stTsStatus, 0, sizeof(RestTsListStatus_t));
	(void)HLIB_VECTOR_ForEach(pstContext->pstTsVector, rest_bind_chan_CbMakeTsListStatus, (void *)&stTsStatus);

	(void)HxJSONWRITER_ObjectBegin(nJWriter);
	{
		// tsListStatus
		(void)HxJSONWRITER_Object(nJWriter, "tsListStatus");
		{
			(void)HxJSONWRITER_Integer(nJWriter, "tsNumber", stTsStatus.ulTsInfoNum);
		}
		(void)HxJSONWRITER_ObjectEnd(nJWriter);

		// error
		(void)rest_bind_MakeErrorJsWriter(nJWriter, REST_ERR_CODE_OK, REST_ERR_STRING_OK);
	}
	(void)HxJSONWRITER_ObjectEnd(nJWriter);

	szResultStr = rest_bind_CloseJsonWriter(nJWriter);
	return szResultStr;
}

STATIC HCHAR *rest_bind_chan_CmdGetTsLists (HCHAR *szSessionId, HINT32 nStartIdx, HINT32 nTsNum, HCHAR *szMember)
{
	HINT32					 nJWriter = 0;
	HCHAR					*szResultStr = NULL;
	restBindChanCbInfo_t	 stCbInfo;
	RestBindChanContext_t	*pstContext = rest_bind_chan_GetContext();
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

	HxSTD_MemSet(&stCbInfo, 0, sizeof(restBindChanCbInfo_t));

	if (NULL == pstContext)
	{
		HxLOG_Error("rest_bind_chan_GetContext() NULL\n");
		return rest_bind_MakeErrorString(REST_ERR_CODE_NOTREADY, REST_ERR_STRING_NOTREADY, TRUE);
	}

	stCbInfo.nStart			= nStartIdx;
	stCbInfo.nRequestedCnt	= nTsNum;
	stCbInfo.nJsonWriter	= nJWriter;
	stCbInfo.szMember		= szMember;

	HxJSONWRITER_ObjectBegin(nJWriter);
	{
		// Transponders:
		if ((NULL != pstContext->pstTsVector) && (HLIB_VECTOR_Length(pstContext->pstTsVector) > 0))
		{
			HxJSONWRITER_Array(nJWriter, "Transponders");
			HLIB_VECTOR_ForEach(pstContext->pstTsVector, rest_bind_chan_cbWriteTransponderByIndex, (void *)&stCbInfo);
			HxJSONWRITER_ArrayEnd(nJWriter);
		}

		// Error:
		if (stCbInfo.nGetNumber > 0)
		{
			(void)rest_bind_MakeErrorJsWriter(nJWriter, REST_ERR_CODE_OK, REST_ERR_STRING_OK);
		}
		else
		{
			(void)rest_bind_MakeErrorJsWriter(nJWriter, REST_ERR_CODE_NODATA, REST_ERR_STRING_NODATA);
		}
	}
	HxJSONWRITER_ObjectEnd(nJWriter);

	szResultStr = rest_bind_CloseJsonWriter(nJWriter);
	return szResultStr;
}

STATIC HCHAR *rest_bind_chan_CmdGetTsTunable (HCHAR *szSessionId, HCHAR *szMember)
{
	HINT32					 nJWriter = 0;
	HCHAR					*szResultStr = NULL;
	RestBindChanContext_t	*pstContext = rest_bind_chan_GetContext();
	RestInstance_t			*pstInst;

	HxSEMT_Get(pstContext->ulSemForTunable);

	pstInst = rest_task_GetInstance(szSessionId);
	if (NULL == pstInst)
	{
		HxLOG_Error("rest_task_GetInstance err:\n");
		szResultStr = rest_bind_MakeErrorString(REST_ERR_CODE_INVALID_SESSION, REST_ERR_STRING_INVALID_SESSION, TRUE);
		goto END_FUNC;
	}

	if ((NULL == pstContext->pstTunableArray) || (0 == pstContext->ulTunableInfoNum))
	{
		(void)rest_bind_tunable_CheckTunableTps();
		if ((NULL == pstContext->pstTunableArray) || (0 == pstContext->ulTunableInfoNum))
		{
			HxLOG_Error("No Tunable Information at all...\n");
			szResultStr = rest_bind_MakeErrorString(REST_ERR_CODE_NO_TUNABLE_INFO, REST_ERR_STRING_NO_TUNABLE_INFO, TRUE);
			goto END_FUNC;
		}
	}

	nJWriter = rest_bind_OpenJsonWriter();
	if (0 == nJWriter)
	{
		HxLOG_Error("rest_bind_OpenJsonWriter() err:\n");
		szResultStr = rest_bind_MakeErrorString(REST_ERR_CODE_NOMEM, REST_ERR_STRING_NOMEM, TRUE);
		goto END_FUNC;
	}

	(void)HxJSONWRITER_ObjectBegin(nJWriter);
	{
		// Transponders Tunable:
		(void)rest_datatype_JsonizeTpTunable(nJWriter, pstContext->pstTunableArray, pstContext->ulTunableInfoNum, "tunableTpUids", szMember);

		// error
		(void)rest_bind_MakeErrorJsWriter(nJWriter, REST_ERR_CODE_OK, REST_ERR_STRING_OK);
	}
	(void)HxJSONWRITER_ObjectEnd(nJWriter);

	szResultStr = rest_bind_CloseJsonWriter(nJWriter);

END_FUNC:
	HxSEMT_Release(pstContext->ulSemForTunable);
	return szResultStr;
}

STATIC HERROR rest_bind_chan_CbGetTransponders (void *pvArg)
{
	HINT32			 nStartIdx = -1, nCnt = -1;
	HCHAR			*szMember = NULL;
	HCHAR			*szMode = NULL;
	HCHAR			*szResult = NULL;
	RestMsg_t		*pstMsg = (RestMsg_t *)pvArg;
	HxJSON_t		 jsArgs = NULL;
	HERROR			 hResult = ERR_FAIL;

	if (NULL == pstMsg)					{ goto END_FUNC; }

	jsArgs = HLIB_JSON_Decode((HCHAR *)pstMsg->aulArgs[0]);
	if (NULL != jsArgs)
	{
		(void)rest_bind_GetStringValueFromJson(jsArgs, "member", &szMember);
		(void)rest_bind_GetNumberValueFromJson(jsArgs, "startidx", &nStartIdx);
		(void)rest_bind_GetNumberValueFromJson(jsArgs, "cnt", &nCnt);
		(void)rest_bind_GetStringValueFromJson(jsArgs, "mode", &szMode);

		if ((nCnt < 1) || (nCnt > REST_BIND_TS_TRANSFER_MAX))
		{
			nCnt = REST_BIND_TS_TRANSFER_MAX;
		}
	}

	if (NULL != szMode)
	{
		if (HLIB_STD_StrCaseCmp((const HCHAR *)szMode, "tunable") == 0)
		{
			szResult = rest_bind_chan_CmdGetTsTunable(pstMsg->szSessionId, szMember);
		}
	}

	if (NULL == szResult)
	{
		if (nStartIdx >= 0)
		{
			szResult = rest_bind_chan_CmdGetTsLists(pstMsg->szSessionId, nStartIdx, nCnt, szMember);
		}
		else
		{
			szResult = rest_bind_chan_CmdGetTsListStatus(pstMsg->szSessionId);
		}
	}

	pstMsg->pvResult = (void *)szResult;
	hResult = ERR_OK;

END_FUNC:
	if (NULL != jsArgs)
	{
		HLIB_JSON_Delete(jsArgs);
	}

	rest_task_SyncOut();
	return hResult;
}

STATIC HBOOL rest_bind_chan_CbSetAntennaToTransponder (void *data, void *userdata)
{
	STATIC HINT32			 s_nLastAntId = -1;
	STATIC DxAntInfo_t		 s_stLastAnt;

	DxTransponder_t			*pstTs = (void *)data;

	if ((NULL != pstTs) && (eDxDELIVERY_TYPE_SAT == pstTs->deliType))
	{
		if ((-1 == s_nLastAntId) || (pstTs->tuningParam.sat.tuningInfo.antuid != s_nLastAntId))
		{
			DxAntInfo_t			*pstAnt;

			pstAnt = APK_META_ANTENNA_GetInfo(pstTs->tuningParam.sat.tuningInfo.antuid);
			if (NULL == pstAnt)
			{
				HxLOG_Error("APK_META_ANTENNA_GetInfo err: UID(%d)\n", pstTs->tuningParam.sat.tuningInfo.antuid);
				return FALSE;
			}

			s_nLastAntId = pstTs->tuningParam.sat.tuningInfo.antuid;
			s_stLastAnt = *pstAnt;

			APK_META_ANTENNA_InfoFree(pstAnt);
		}

		pstTs->tuningParam.sat.antInfo = s_stLastAnt;
	}

	return FALSE;
}

#define _____ANTENNA_FUNCTIONS_____
STATIC HCHAR *rest_bind_chan_CmdGetAntennaListStatus (RestInstance_t *pstInst, HCHAR *szMember)
{
	HINT32					 nJWriter = 0;
	HCHAR					*szResultStr = NULL;
//	RestBindChanContext_t	*pstContext = rest_bind_chan_GetContext();
	RestAntListStatus_t		 stAntStatus;

	nJWriter = rest_bind_OpenJsonWriter();
	if (0 == nJWriter)
	{
		HxLOG_Error("rest_bind_OpenJsonWriter() err:\n");
		return rest_bind_MakeErrorString(REST_ERR_CODE_NOMEM, REST_ERR_STRING_NOMEM, TRUE);
	}

	(void)HxJSONWRITER_ObjectBegin(nJWriter);
	{
		HxVector_t			*pstAntVector = APK_META_ANTENNA_GetList();

		HxSTD_MemSet(&stAntStatus, 0, sizeof(RestAntListStatus_t));
		stAntStatus.nMinAntUid	= -1;
		stAntStatus.nMaxAntUid	= -1;
		stAntStatus.nAntConnectType = -1;

		(void)APK_SCENARIO_GetAntennaConnection((DxAntennaConnection_e *)&(stAntStatus.nAntConnectType));

		if ((NULL != pstAntVector) && (HLIB_VECTOR_Length(pstAntVector) > 0))
		{
			HLIB_VECTOR_ForEach(pstAntVector, rest_bind_chan_CbMakeAntListStatus, (void *)&stAntStatus);
		}

		if (stAntStatus.ulAntInfoNum > 0)
		{
			// antListStatus
			(void)HxJSONWRITER_Object(nJWriter, "antListStatus");
			{
				(void)HxJSONWRITER_Integer(nJWriter, "antNumber", stAntStatus.ulAntInfoNum);
				(void)HxJSONWRITER_Integer(nJWriter, "minAntennaUid", stAntStatus.nMinAntUid);
				(void)HxJSONWRITER_Integer(nJWriter, "maxAntennaUid", stAntStatus.nMaxAntUid);
				(void)rest_datatype_SetJsonEnum(nJWriter, "antennaConnectionType", DxNAMEOF(DxAntennaConnection_e), stAntStatus.nAntConnectType, szMember);
			}
			(void)HxJSONWRITER_ObjectEnd(nJWriter);

			// Error:
			(void)rest_bind_MakeErrorJsWriter(nJWriter, REST_ERR_CODE_OK, REST_ERR_STRING_OK);
		}
		else
		{
			// Error:
			(void)rest_bind_MakeErrorJsWriter(nJWriter, REST_ERR_CODE_NODATA, REST_ERR_STRING_NODATA);
		}

		if (NULL != pstAntVector)
		{
			HLIB_VECTOR_Delete(pstAntVector);
		}
	}
	(void)HxJSONWRITER_ObjectEnd(nJWriter);

	szResultStr = rest_bind_CloseJsonWriter(nJWriter);
	return szResultStr;
}

STATIC HCHAR *rest_bind_chan_CmdGetAntennaLists (RestInstance_t *pstInst, HINT32 nStartIdx, HINT32 nItemNum, HCHAR *szMember)
{
	HINT32					 nJWriter = 0;
	HCHAR					*szResultStr = NULL;
	restBindChanCbInfo_t	 stCbInfo;
	RestBindChanContext_t	*pstContext = rest_bind_chan_GetContext();

	nJWriter = rest_bind_OpenJsonWriter();
	if (0 == nJWriter)
	{
		HxLOG_Error("rest_bind_OpenJsonWriter() err:\n");
		return rest_bind_MakeErrorString(REST_ERR_CODE_NOMEM, REST_ERR_STRING_NOMEM, TRUE);
	}

	HxSTD_MemSet(&stCbInfo, 0, sizeof(restBindChanCbInfo_t));

	if (NULL == pstContext)
	{
		HxLOG_Error("rest_bind_chan_GetContext() NULL\n");
		return rest_bind_MakeErrorString(REST_ERR_CODE_NOTREADY, REST_ERR_STRING_NOTREADY, TRUE);
	}

	stCbInfo.nStart			= nStartIdx;
	stCbInfo.nRequestedCnt	= nItemNum;
	stCbInfo.nJsonWriter	= nJWriter;
	stCbInfo.szMember		= szMember;

	HxJSONWRITER_ObjectBegin(nJWriter);
	{
		// Antenna:
		HxVector_t			*pstAntVector = APK_META_ANTENNA_GetList();

		if ((NULL != pstAntVector) && (HLIB_VECTOR_Length(pstAntVector) > 0))
		{
			HxJSONWRITER_Array(nJWriter, "Antennas");
			HLIB_VECTOR_ForEach(pstAntVector, rest_bind_chan_cbWriteAntennaByIndex, (void *)&stCbInfo);
			HxJSONWRITER_ArrayEnd(nJWriter);
		}

		// Error:
		if (stCbInfo.nGetNumber > 0)
		{
			(void)rest_bind_MakeErrorJsWriter(nJWriter, REST_ERR_CODE_OK, REST_ERR_STRING_OK);
		}
		else
		{
			(void)rest_bind_MakeErrorJsWriter(nJWriter, REST_ERR_CODE_NODATA, REST_ERR_STRING_NODATA);
		}

		if (NULL != pstAntVector)
		{
			HLIB_VECTOR_Delete(pstAntVector);
		}
	}
	HxJSONWRITER_ObjectEnd(nJWriter);

	szResultStr = rest_bind_CloseJsonWriter(nJWriter);
	return szResultStr;
}

STATIC HCHAR *rest_bind_chan_CmdGetAntennas (HCHAR *szSessionId, HxJSON_t jsArgs)
{
	HINT32			 nStartIdx = -1, nCnt = -1;
	HCHAR			*szMember = NULL;
	HCHAR			*szResult = NULL;
	RestInstance_t	*pstInst;

	pstInst = rest_task_GetInstance(szSessionId);
	if (NULL == pstInst)
	{
		HxLOG_Error("rest_task_GetInstance err:\n");
		return rest_bind_MakeErrorString(REST_ERR_CODE_INVALID_SESSION, REST_ERR_STRING_INVALID_SESSION, TRUE);
	}

	if (NULL != jsArgs)
	{
		(void)rest_bind_GetStringValueFromJson(jsArgs, "member", &szMember);
		(void)rest_bind_GetNumberValueFromJson(jsArgs, "startidx", &nStartIdx);
		(void)rest_bind_GetNumberValueFromJson(jsArgs, "cnt", &nCnt);

		if ((nCnt < 1) || (nCnt > REST_BIND_TS_TRANSFER_MAX))
		{
			nCnt = REST_BIND_TS_TRANSFER_MAX;
		}
	}

	if (nStartIdx < 0)
	{
		szResult = rest_bind_chan_CmdGetAntennaListStatus(pstInst, szMember);
	}
	else
	{
		szResult = rest_bind_chan_CmdGetAntennaLists(pstInst, nStartIdx, nCnt, szMember);
	}

	return szResult;
}

STATIC HERROR rest_bind_chan_CbGetAntennas (void *pvArg)
{
	HCHAR			*szResult = NULL;
	RestMsg_t		*pstMsg = (RestMsg_t *)pvArg;
	HxJSON_t		 jsArgs = NULL;

	if (NULL != pstMsg)
	{
		jsArgs = HLIB_JSON_Decode((HCHAR *)pstMsg->aulArgs[0]);
		szResult = rest_bind_chan_CmdGetAntennas(pstMsg->szSessionId, jsArgs);
	}
	else
	{
		szResult = rest_bind_MakeErrorString(REST_ERR_CODE_INTERNAL, REST_ERR_STRING_INTERNAL, TRUE);
	}

	pstMsg->pvResult = (void *)szResult;

	if (NULL != jsArgs)			{ HLIB_JSON_Delete(jsArgs); }
	rest_task_SyncOut();

	return ERR_OK;
}

#define _____NOTIFY_CALLBACK_FUNCTIONS_____
STATIC HERROR rest_bind_chan_ReloadChannelLists (RestBindChanContext_t *pstContext)
{
	HxVector_t				*pstSvcVector = NULL;
	HxVector_t				*pstTsVector = NULL;

	// Load the service list:
	HxSTD_MemSet(&(pstContext->stChanListStatus), 0, sizeof(RestChanListStatus_t));
	pstContext->stChanListStatus.nMinTvLcn		= -1;
	pstContext->stChanListStatus.nMaxTvLcn		= -1;
	pstContext->stChanListStatus.nMinRadioLcn	= -1;
	pstContext->stChanListStatus.nMaxRadioLcn	= -1;

	if (NULL != pstContext->pstSvcVector)
	{
		HLIB_VECTOR_Delete(pstContext->pstSvcVector);
		pstContext->pstSvcVector = NULL;
	}

	if (NULL != pstContext->pstSvcLcnVector)
	{
		HLIB_VECTOR_Delete(pstContext->pstSvcLcnVector);
		pstContext->pstSvcLcnVector = NULL;
	}

	if (NULL != pstContext->pstTsVector)
	{
		HLIB_VECTOR_Delete(pstContext->pstTsVector);
		pstContext->pstTsVector = NULL;
	}

	pstSvcVector = APK_META_SVC_GetServiceList();
	if (NULL != pstSvcVector)
	{
		pstContext->pstSvcVector = pstSvcVector;

		(void)HLIB_VECTOR_ForEach(pstContext->pstSvcVector, rest_bind_chan_CbMakeChanListStatus, (void *)&(pstContext->stChanListStatus));

		// Make the LCN-sorted list:
		pstContext->pstSvcLcnVector = HLIB_VECTOR_NewEasy(0, NULL, rest_bind_chan_CbCmpSvcByLcn);
		if (NULL != pstContext->pstSvcLcnVector)
		{
			(void)HLIB_VECTOR_ForEach(pstContext->pstSvcVector, rest_bind_chan_CbAppendSvcPtrToVector, (void *)pstContext->pstSvcLcnVector);
			(void)HLIB_VECTOR_Sort(pstContext->pstSvcLcnVector, rest_bind_chan_CbCmpSvcByLcn);
			// (void)HLIB_VECTOR_ForEach(pstContext->pstSvcLcnVector, rest_bind_chan_CbPrintSvc, NULL);
		}
	}

	// Load the transponder list:
	pstTsVector = APK_META_SVC_GetTransponderList();
	if (NULL != pstTsVector)
	{
		// Add the transponder list to the antenna info...
		HLIB_VECTOR_ForEach(pstTsVector, rest_bind_chan_CbSetAntennaToTransponder, NULL);
		pstContext->pstTsVector = pstTsVector;
	}

	return ERR_OK;
}

STATIC HCHAR *rest_bind_chan_MakeNotifierMsg (HCHAR *szEvent)
{
	HINT32					 nJWriter = 0;
	HCHAR					*szResultStr = NULL;
	HINT32					 nErrCode = REST_ERR_CODE_INTERNAL;
	HCHAR					*szErrString = REST_ERR_STRING_INTERNAL;

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
		// Event
		(void)HxJSONWRITER_String(nJWriter, "event", szEvent);

		// Data
		(void)HxJSONWRITER_Object(nJWriter, "data");
		{
			(void)HxJSONWRITER_String(nJWriter, "dummy", "dummy");
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

STATIC void rest_bind_chan_CbApkMetaSvcNotified (HINT32 event, const HINT32 *args, void *userdata)
{
	HCHAR					*szPayLoad = NULL;
	RestBindChanContext_t	*pstContext = rest_bind_chan_GetContext();

	if (NULL == pstContext)
	{
		HxLOG_Error("Context NULL\n");
		return;
	}

	(void)rest_bind_chan_ReloadChannelLists(pstContext);
	szPayLoad = rest_bind_chan_MakeNotifierMsg("channelUpdated");

	if (NULL != szPayLoad)
	{
		HxList_t					*pstListItem;

		for (pstListItem = s_pstRestBindChan_Notifier; NULL != pstListItem; pstListItem = pstListItem->next)
		{
			RestBind_Notifier_t	pfNotifier = (RestBind_Notifier_t)HLIB_LIST_Data(pstListItem);

			if (NULL != pfNotifier)
			{
				pfNotifier("channel", szPayLoad);
			}
		}

		rest_datatype_AppendStringBufferToTemporary(szPayLoad);
	}
}

#define ___MEMBER_FUNCTIONS___
RestBindChanContext_t *rest_bind_chan_GetContext (void)
{
	if (TRUE != s_stRestBindChan_Inited)
	{
		HxLOG_Error("rest_bind_channel not initialized:\n");
		return NULL;
	}

	return &s_stRestBindChan_Context;
}

HERROR rest_bind_chan_Init (void)
{
	if (TRUE != s_stRestBindChan_Inited)
	{
		RestBindChanContext_t	*pstContext = &s_stRestBindChan_Context;

		// Initialize the APPKIT
		(void)APK_META_SVC_Init(FALSE);
		(void)APK_META_ANTENNA_Init(FALSE);
		(void)APK_DLNADMS_Init();

		//pstContext->ulSvcNotifier	= DAPI_AddTableNotifier(DxNAMEOF(DxService_t), rest_bind_chan_CbDamaOnSvcUpdated, NULL);
		//pstContext->ulTsNotifier	= DAPI_AddTableNotifier(DxNAMEOF(DxTransponder_t), rest_bind_chan_CbDamaOnTsUpdated, NULL);
		//pstContext->ulSvcNotifier	= DAPI_AddTableNotifier(DxNAMEOF(DxAntInfo_t), rest_bind_chan_CbDamaOnAntUpdated, NULL);
		(void)APK_META_SVC_SetListener(rest_bind_chan_CbApkMetaSvcNotified, NULL);

		HxSTD_MemSet(pstContext, 0, sizeof(RestBindChanContext_t));

		HxSEMT_Create(&(pstContext->ulSemId), "bind_chan", HxSEMT_FIFO);

		(void)rest_bind_chan_ReloadChannelLists(pstContext);

		s_stRestBindChan_Inited = TRUE;

		// Tunable Init:
		(void)rest_bind_tunable_Init();

	}

	return ERR_OK;
}

HERROR rest_bind_chan_ExecNotifier (HCHAR *szEvent, HCHAR *szPayLoad)
{
	HCHAR						*szDupEvent = NULL;
	HCHAR						*szDupPayLoad = NULL;
	HxList_t					*pstListItem;
	HERROR						 hResult = ERR_FAIL;

	if ((NULL == szEvent) || ('\0' == *szEvent))
	{
		HxLOG_Error("Event NULL or no string\n");
		goto END_FUNC;
	}

	if ((NULL == szPayLoad) || ('\0' == *szPayLoad))
	{
		HxLOG_Error("PayLoad NULL or no string\n");
		goto END_FUNC;
	}

	szDupEvent = HLIB_STD_StrDup(szEvent);
	szDupPayLoad = HLIB_STD_StrDup(szPayLoad);
	if ((NULL == szDupEvent) || (NULL == szDupPayLoad))
	{
		HxLOG_Error("string dupplication failed:\n");
		goto END_FUNC;
	}

	for (pstListItem = s_pstRestBindChan_Notifier; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		RestBind_Notifier_t pfNotifier = (RestBind_Notifier_t)HLIB_LIST_Data(pstListItem);

		if (NULL != pfNotifier)
		{
			pfNotifier(szDupEvent, szDupPayLoad);
		}
	}

	hResult = ERR_OK;

END_FUNC:
	if (NULL != szDupEvent)
	{
		rest_datatype_AppendStringBufferToTemporary(szDupEvent);
	}

	if (NULL != szDupPayLoad)
	{
		rest_datatype_AppendStringBufferToTemporary(szDupPayLoad);
	}

	return hResult;
}

#define ___API_FUNCTIONS___
int REST_BIND_CHAN_RegisterNotifier (RestBind_Notifier_t pfNotifier)
{
	RestMsg_t				 stMsg;
	HERROR					 hErr;

	stMsg.pvResult		= (void *)0;
	stMsg.aulArgs[0]	= (HUINT32)pfNotifier;

	hErr = rest_task_RequestToExecute(rest_bind_chan_CbRegisterNotifier, (void *)&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("rest_task_RequestToExecute err:\n");
		return ERR_FAIL;
	}

	rest_task_SyncIn();

	return (int)stMsg.pvResult;
}

int REST_BIND_CHAN_UnregisterNotifier (RestBind_Notifier_t pfNotifier)
{
	RestMsg_t				 stMsg;
	HERROR					 hErr;

	stMsg.pvResult		= (void *)0;
	stMsg.aulArgs[0]	= (HUINT32)pfNotifier;

	hErr = rest_task_RequestToExecute(rest_bind_chan_CbUnregisterNotifier, (void *)&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("rest_task_RequestToExecute err:\n");
		return ERR_FAIL;
	}

	rest_task_SyncIn();

	return (int)stMsg.pvResult;
}

#define _____CHANNEL_____

// GET http://127.0.0.1/stb-api/v1/channels
char *REST_BIND_CHAN_GetChannels (char *szSessionId, char *szArgJson)
{
	RestMsg_t				 stMsg;
	char					*szResult = NULL;
	HERROR					 hErr;

	stMsg.szSessionId	= REST_DUMMY_SESSION_ID;
	stMsg.pvResult		= NULL;
	stMsg.aulArgs[0]	= (HUINT32)szArgJson;

	hErr = rest_task_RequestToExecute(rest_bind_chan_CbGetChannels, (void *)&stMsg);
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

// GET http://127.0.0.1/stb-api/v1/channels/1
char *REST_BIND_CHAN_GetChannelOne (char *szSessionId, int nChannelUid, char *szArgJson)
{
	RestMsg_t				 stMsg;
	char					*szResult = NULL;
	HERROR					 hErr;

	stMsg.szSessionId	= REST_DUMMY_SESSION_ID;
	stMsg.pvResult		= NULL;
	stMsg.aulArgs[0]	= (HUINT32)nChannelUid;
	stMsg.aulArgs[1]	= (HUINT32)szArgJson;

	hErr = rest_task_RequestToExecute(rest_bind_chan_CbGetOneChannel, (void *)&stMsg);
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

#define _____TRANSPONDER_____

// GET http://127.0.0.1/stb-api/v1/transponders
char *REST_BIND_CHAN_GetTransponders (char *szSessionId, char *szArgJson)
{
	RestMsg_t				 stMsg;
	char					*szResult = NULL;
	HERROR					 hErr;

	stMsg.szSessionId	= REST_DUMMY_SESSION_ID;
	stMsg.pvResult		= NULL;
	stMsg.aulArgs[0]	= (HUINT32)szArgJson;

	hErr = rest_task_RequestToExecute(rest_bind_chan_CbGetTransponders, (void *)&stMsg);
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


#define _____ANTENNA_____

// GET http://127.0.0.1/stb-api/v1/antennas
char *REST_BIND_CHAN_GetAntennas (char *szSessionId, char *szArgJson)
{
	RestMsg_t				 stMsg;
	char					*szResult = NULL;
	HERROR					 hErr;

	stMsg.szSessionId	= REST_DUMMY_SESSION_ID;
	stMsg.pvResult		= NULL;
	stMsg.aulArgs[0]	= (HUINT32)szArgJson;

	hErr = rest_task_RequestToExecute(rest_bind_chan_CbGetAntennas, (void *)&stMsg);
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


