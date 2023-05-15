/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  om_download.c
	@brief

	Description:  									\n
	Module: TVMANAGER / OAPI Manager				\n

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
#include <obama_rpcprotocol.h>
#include <oapi.h>
#include <octo_common.h>
#include <bus.h>
#include <mgr_common.h>
#include <om_download.h>
#include "_om_download.h"

/*******************************************************************/
/********************      Defines         *************************/
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
STATIC omDownload_Context_t		 s_stOmDnld_Context;

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/


#define ___LOCAL_FUNCTIONS___
#define _____OAPI_COMMAND_FUNCTIONS_____
STATIC HERROR om_download_CbActivate (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	omDownload_Context_t	*pstContext = om_download_GetContext();

	if (TRUE != pstContext->bSubModuleInited)
	{
		/* sub-module*/
	#if defined(CONFIG_OTA_IP)
		om_download_ipmedia_Init();
	#endif

		pstContext->bSubModuleInited = TRUE;
	}

	HLIB_RPC_Response(pstContext->nRpcHandle, nClientHandle, nRequestId, ERR_OK, NULL);
	return ERR_OK;
}

STATIC HERROR om_download_CbProbe (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HUINT32					 ulDownloadId = (HUINT32)HxOBJECT_INT(apArgv[0]);
	DxDownload_Start_t		*punStart = (DxDownload_Start_t *)HxOBJECT_BIN_DATA(apArgv[1]);
	OmDownload_Instance_t	*pstInst = NULL;
	omDownload_Context_t	*pstContext = om_download_GetContext();
	HERROR					 hErr = ERR_OK, hResult = ERR_FAIL;
	HUINT32					 ulSessionId;
	HBOOL					 bUseExistSession = FALSE;
	OxMediaPlay_ErrorReason_e eReason = eOxMP_ERROR_REASON_NONE;

	pstInst = OM_DOWNLOAD_GetInstance(ulDownloadId);
	if (NULL != pstInst)
	{
		HxLOG_Debug("ulDownloadId(0x%08x) already exists:\n", ulDownloadId);
		bUseExistSession = TRUE;
	}
	else
	{
		if ((punStart->eType <= eDxDNLD_TYPE_NONE) || (punStart->eType >= eDxDNLD_TYPE_EndOfCase))
		{
			HxLOG_Error("ulDownloadId(0x%08x) Start Type wrone:(%d)\n", ulDownloadId, punStart->eType);
			goto END_FUNC;
		}

		pstInst = (OmDownload_Instance_t *)OxAP_Calloc(sizeof(OmDownload_Instance_t));
		if (NULL == pstInst)
		{
			HxLOG_Error("Memory allocation failed:\n");
			goto END_FUNC;
		}

		pstInst->eType = punStart->eType;
		pstInst->ulDownloadId = ulDownloadId;
		HxSTD_MemCopy(&(pstInst->unStartInfo), punStart, sizeof(DxDownload_Start_t));
	}

	HxLOG_Debug("[om_download_CbProbe] type=%d, url=%s\n", punStart->eType, punStart->stIpMedia.szUrl);
	switch (punStart->eType)
	{
	case eDxDNLD_TYPE_IpMediaStream:
	#if defined(CONFIG_OTA_IP)
		hErr = om_download_ipmedia_CbProbe(pstInst, ulDownloadId, punStart);
	#endif
		break;

	default:
		hErr = ERR_FAIL;
		break;
	}

	eReason = pstInst->stError.eReason;
	if (ERR_OK == hErr)
	{
		hResult = ERR_OK;
		if(bUseExistSession == FALSE)
		{
			pstContext->pstInstList = HLIB_LIST_Append(pstContext->pstInstList, (void *)pstInst);
			pstContext->anInstNum[punStart->eType]++;
		}
		ulSessionId = pstInst->unExtInfo.stIpMedia.ulCsoSessionId;
		pstInst = NULL;
	}

END_FUNC:

	if (ERR_OK != hResult)
	{
		HLIB_RPC_Notify(pstContext->nRpcHandle, RPC_OBAMA_DOWNLOAD_OnProbeFail, "ii", ulDownloadId, eReason);
	}

	if (NULL != pstInst)					{ OxAP_Free(pstInst); }

	return hResult;
}


STATIC HERROR om_download_CbStart (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HUINT32					 ulDownloadId = (HUINT32)HxOBJECT_INT(apArgv[0]);
	DxDownload_Start_t		*punStart = (DxDownload_Start_t *)HxOBJECT_BIN_DATA(apArgv[1]);
	OmDownload_Instance_t	*pstInst = NULL;
	omDownload_Context_t	*pstContext = om_download_GetContext();
	HERROR					 hErr = ERR_OK, hResult = ERR_FAIL;
	HUINT32					 ulSessionId;
	HBOOL					 bUseExistSession = FALSE;
	pstInst = OM_DOWNLOAD_GetInstance(ulDownloadId);
	if (NULL != pstInst)
	{
		HxLOG_Debug("ulDownloadId(0x%08x) already exists:\n", ulDownloadId);
		bUseExistSession = TRUE;
	}
	else
	{
		if ((punStart->eType <= eDxDNLD_TYPE_NONE) || (punStart->eType >= eDxDNLD_TYPE_EndOfCase))
		{
			HxLOG_Error("ulDownloadId(0x%08x) Start Type wrone:(%d)\n", ulDownloadId, punStart->eType);
			goto END_FUNC;
		}

		pstInst = (OmDownload_Instance_t *)OxAP_Calloc(sizeof(OmDownload_Instance_t));
		if (NULL == pstInst)
		{
			HxLOG_Error("Memory allocation failed:\n");
			goto END_FUNC;
		}

		pstInst->eType = punStart->eType;
		pstInst->ulDownloadId = ulDownloadId;
		HxSTD_MemCopy(&(pstInst->unStartInfo), punStart, sizeof(DxDownload_Start_t));
	}
	HxLOG_Debug("[om_download_CbStart] type=%d, url=%s\n", punStart->eType,  punStart->stIpMedia.szUrl);
	switch (punStart->eType)
	{
	case eDxDNLD_TYPE_IpMediaStream:
	#if defined(CONFIG_OTA_IP)
		hErr = om_download_ipmedia_CbStart(pstInst, ulDownloadId, punStart);
	#endif
		break;

	default:
		hErr = ERR_FAIL;
		break;
	}

	if (ERR_OK == hErr)
	{
		hResult = ERR_OK;
		if(bUseExistSession == FALSE)
		{
			pstContext->pstInstList = HLIB_LIST_Append(pstContext->pstInstList, (void *)pstInst);
			pstContext->anInstNum[punStart->eType]++;
		}
		ulSessionId = pstInst->unExtInfo.stIpMedia.ulCsoSessionId;
		pstInst = NULL;
	}

END_FUNC:
	if (ERR_OK != hResult)
	{
		HLIB_RPC_Notify(pstContext->nRpcHandle, RPC_OBAMA_DOWNLOAD_OnFailed, "i", ulDownloadId);
	}
	if (NULL != pstInst)					{ OxAP_Free(pstInst); }

	return hResult;
}

STATIC HERROR om_download_CbStop (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HUINT32 				 ulDownloadId = (HUINT32)HxOBJECT_INT(apArgv[0]);
	OmDownload_Instance_t	*pstInst = OM_DOWNLOAD_GetInstance(ulDownloadId);
	omDownload_Context_t	*pstContext = om_download_GetContext();
	HERROR					 hErr = ERR_OK;
	HUINT32					 ulSessionId = 0;

	pstInst = OM_DOWNLOAD_GetInstance(ulDownloadId);
	if (NULL == pstInst)
	{
		HxLOG_Error("No instance with ulDownloadId(0x%08x)\n", ulDownloadId);
		return ERR_FAIL;
	}

	switch (pstInst->eType)
	{
	case eDxDNLD_TYPE_IpMediaStream:
	#if defined(CONFIG_OTA_IP)
		hErr = om_download_ipmedia_CbStop(ulDownloadId);
	#endif
		break;
	default:
		hErr = ERR_FAIL;
		break;
	}

	if (ERR_OK == hErr)
	{
		ulSessionId = pstInst->unExtInfo.stIpMedia.ulCsoSessionId;
		HLIB_RPC_Notify(pstContext->nRpcHandle, RPC_OBAMA_DOWNLOAD_OnStopped, "ii", ulDownloadId, ulSessionId);

		pstContext->pstInstList = HLIB_LIST_Remove(pstContext->pstInstList, (const void *)pstInst);
		OxAP_Free(pstInst);
	}

	return hErr;
}

STATIC HERROR om_download_CbPause (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HUINT32 				 ulDownloadId = (HUINT32)HxOBJECT_INT(apArgv[0]);
	OmDownload_Instance_t	*pstInst = OM_DOWNLOAD_GetInstance(ulDownloadId);
	HERROR					 hErr = ERR_OK;

	pstInst = OM_DOWNLOAD_GetInstance(ulDownloadId);
	if (NULL == pstInst)
	{
		HxLOG_Error("No instance with ulDownloadId(0x%08x)\n", ulDownloadId);
		return ERR_FAIL;
	}

	switch (pstInst->eType)
	{
	case eDxDNLD_TYPE_IpMediaStream:
	#if defined(CONFIG_OTA_IP)
		hErr = om_download_ipmedia_CbPause(ulDownloadId);
	#endif
		break;

	default:
		hErr = ERR_FAIL;
		break;
	}

	return hErr;
}

STATIC HERROR om_download_CbResume (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HUINT32 				 ulDownloadId = (HUINT32)HxOBJECT_INT(apArgv[0]);
	OmDownload_Instance_t	*pstInst = OM_DOWNLOAD_GetInstance(ulDownloadId);
	HERROR					 hErr;

	pstInst = OM_DOWNLOAD_GetInstance(ulDownloadId);
	if (NULL == pstInst)
	{
		HxLOG_Error("No instance with ulDownloadId(0x%08x)\n", ulDownloadId);
		return ERR_FAIL;
	}

	switch (pstInst->eType)
	{
	default:
		hErr = ERR_FAIL;
		break;
	}

	return hErr;
}

void om_download_CbOnComponentVideoChanged (HINT32 ulDownloadId, HUINT32 ulSessionId, OxDownload_VideoCompTable_t *pstVideoCompTable)
{
	omDownload_Context_t	*pstContext = om_download_GetContext();
	HLIB_RPC_Notify (pstContext->nRpcHandle, RPC_OBAMA_DOWNLOAD_OnComponentVideoChanged, "iib", ulDownloadId, ulSessionId, pstVideoCompTable, sizeof(OxDownload_VideoCompTable_t));
}
void om_download_CbOnComponentAudioChanged (HINT32 ulDownloadId, HUINT32 ulSessionId, OxDownload_AudioCompTable_t *pstAudioCompTable)
{
	omDownload_Context_t	*pstContext = om_download_GetContext();
	HLIB_RPC_Notify (pstContext->nRpcHandle, RPC_OBAMA_DOWNLOAD_OnComponentAudioChanged, "iib", ulDownloadId, ulSessionId, pstAudioCompTable, sizeof(OxDownload_AudioCompTable_t) );
}
void om_download_CbOnComponentSubtitleChanged (HINT32 ulDownloadId, HUINT32 ulSessionId, OxDownload_SubtitleCompTable_t *pstSubtitleCompTable)
{
	omDownload_Context_t	*pstContext = om_download_GetContext();
	HLIB_RPC_Notify (pstContext->nRpcHandle, RPC_OBAMA_DOWNLOAD_OnComponentSubtitleChanged, "iib", ulDownloadId, ulSessionId, pstSubtitleCompTable, sizeof(OxDownload_SubtitleCompTable_t) );
}
void om_download_CbOnComponentDataChanged (HINT32 ulDownloadId, HUINT32 ulSessionId, OxDownload_DataCompTable_t *pstDataCompTable)
{
	omDownload_Context_t	*pstContext = om_download_GetContext();
	HLIB_RPC_Notify (pstContext->nRpcHandle, RPC_OBAMA_DOWNLOAD_OnComponentDataChanged, "iib", ulDownloadId, ulSessionId, pstDataCompTable, sizeof(OxDownload_DataCompTable_t) );
}
void om_download_CbOnComponentSelected (HINT32 ulDownloadId, HUINT32 ulSessionId, OxDownload_Component_e eCompType, HINT32 nSelectIndex, HINT32 nSelectSubIndex)
{
	omDownload_Context_t	*pstContext = om_download_GetContext();
	HLIB_RPC_Notify (pstContext->nRpcHandle, RPC_OBAMA_DOWNLOAD_OnComponentSelected, "iiiii", ulDownloadId, ulSessionId, eCompType, nSelectIndex, nSelectSubIndex);
}
void om_download_CbOnAvailabilityStartTimeChanged (HINT32 ulDownloadId, HUINT32 ulSessionId, HCHAR *pString)
{
	omDownload_Context_t	*pstContext = om_download_GetContext();
	HLIB_RPC_Notify (pstContext->nRpcHandle, RPC_OBAMA_DOWNLOAD_OnAvailabilityStartTimeChanged, "iis", ulDownloadId, ulSessionId, pString);
}



#define _____MSG_PROCESS_FUNCTIONS_____
// Create와 Destroy 만은 Context가 NULL일 수 있다.
STATIC BUS_Result_t om_download_MsgBusCreate (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	omDownload_Context_t	*pstContext = om_download_GetContext();

	HxSTD_MemSet(pstContext, 0, sizeof(omDownload_Context_t));

	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_download_MsgBusDestroy (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return MESSAGE_PASS;
}

#define _____RPC_INITIALIZATION_____
STATIC HERROR om_download_InitRpcFunctions (void)
{
	omDownload_Context_t	*pstContext = om_download_GetContext();
	HERROR					 hErr;

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&(pstContext->nRpcHandle));
	if (ERR_OK != hErr )
	{
		HxLOG_Error("OBAMA_RpcProtocol_GetRpcHandle err:\n");
		return ERR_FAIL;
	}

	// APPKIT -> OBAMA
	HLIB_RPC_RegisterRequest(pstContext->nRpcHandle, RPC_OAPI_DOWNLOAD_Activate,	NULL, 	om_download_CbActivate	, NULL, "Activate the download module: (void)");
	HLIB_RPC_RegisterCall(pstContext->nRpcHandle, RPC_OAPI_DOWNLOAD_Probe,			"ib",	om_download_CbProbe		, NULL, "Probe : (HUINT32 ulDownloadId, DxDownload_Start_t *punStart)");
	HLIB_RPC_RegisterCall(pstContext->nRpcHandle, RPC_OAPI_DOWNLOAD_Start,			"ib",	om_download_CbStart		, NULL, "Start the download: (HUINT32 ulDownloadId, DxDownload_Start_t *punStart)");
	HLIB_RPC_RegisterCall(pstContext->nRpcHandle, RPC_OAPI_DOWNLOAD_Stop,			"i",	om_download_CbStop		, NULL, "Stop the download: (HUINT32 ulDownloadId)");
	HLIB_RPC_RegisterCall(pstContext->nRpcHandle, RPC_OAPI_DOWNLOAD_Pause,			"i",	om_download_CbPause		, NULL, "Pause the download: (HUINT32 ulDownloadId)");
	HLIB_RPC_RegisterCall(pstContext->nRpcHandle, RPC_OAPI_DOWNLOAD_Resume,			"i",	om_download_CbResume	, NULL, "Resume the paused download: (HUINT32 ulDownloadId)");

	// OBAMA -> APPKIT
	HLIB_RPC_CreateNotification(pstContext->nRpcHandle, RPC_OBAMA_DOWNLOAD_OnStarted	, "iib", "BASE Started.		\n\t   - (HUINT32 ulDownloadId, HUINT32 ulSessionId, DxDownload_Start_t *punStart)\n");
	HLIB_RPC_CreateNotification(pstContext->nRpcHandle, RPC_OBAMA_DOWNLOAD_OnCompleted	, "i",  "BASE Completed.		\n\t   - (HUINT32 ulDownloadId)\n");
	HLIB_RPC_CreateNotification(pstContext->nRpcHandle, RPC_OBAMA_DOWNLOAD_OnFailed		, "i",  "BASE Failed.		\n\t   - (HUINT32 ulDownloadId)\n");
	HLIB_RPC_CreateNotification(pstContext->nRpcHandle, RPC_OBAMA_DOWNLOAD_OnStopped	, "ii",  "BASE Stopped by API.		\n\t   - (HUINT32 ulDownloadId, HUINT32 ulSessionId)\n");
#if defined(CONFIG_TIMESTAMP_64)
	HLIB_RPC_CreateNotification(pstContext->nRpcHandle, RPC_OBAMA_DOWNLOAD_OnProbeComplete, "iilliib",  "BASE Probe Complete.		\n\t   - (HUINT32 ulDownloadId, HUINT32 ulSessionId, HUINT64 ulDuration, HUINT64 ulStartPos, HUINT32 ulMaxBitrate, HBOOL bInfinity, OxDownload_SeekableInfo_t *pstSeekableInfo)\n");
#else
	HLIB_RPC_CreateNotification(pstContext->nRpcHandle, RPC_OBAMA_DOWNLOAD_OnProbeComplete, "iiiiiib",	"BASE Probe Complete.		\n\t   - (HUINT32 ulDownloadId, HUINT32 ulSessionId, HUINT32 ulDuration, HUINT32 ulStartPos, HUINT32 ulMaxBitrate, HBOOL bInfinity, OxDownload_SeekableInfo_t *pstSeekableInfo)\n");
#endif
	HLIB_RPC_CreateNotification(pstContext->nRpcHandle, RPC_OBAMA_DOWNLOAD_OnProbeFail, "ii",  "BASE Probe Failed.		\n\t   - (HUINT32 ulDownloadId, CSO_ERROR_REASON_e eFailReason)\n");
	HLIB_RPC_CreateNotification(pstContext->nRpcHandle, RPC_OBAMA_DOWNLOAD_OnReceiving	, "ib", "BASE Receiving the data.		\n\t   - (HUINT32 ulDownloadId, DxDownload_Start_t *punStart)\n");
	HLIB_RPC_CreateNotification (pstContext->nRpcHandle, RPC_OBAMA_DOWNLOAD_OnComponentVideoChanged 	, "iib", "BASE ComVideoChanged.	\n\t   - (HUINT32 ulDownloadId, HUINT32 ulSessionId, OxDownload_VideoCompTable_t *pstVideoCompTable)\n");
	HLIB_RPC_CreateNotification (pstContext->nRpcHandle, RPC_OBAMA_DOWNLOAD_OnComponentAudioChanged 	, "iib", "BASE ComAudioChanged.	\n\t   - (HUINT32 ulDownloadId, HUINT32 ulSessionId, OxDownload_AudioCompTable_t *pstAudioCompTable)\n");
	HLIB_RPC_CreateNotification (pstContext->nRpcHandle, RPC_OBAMA_DOWNLOAD_OnComponentSubtitleChanged, "iib", "BASE ComSubtitleChanged.	\n\t   - (HUINT32 ulDownloadId, HUINT32 ulSessionId, OxDownload_SubtitleCompTable_t *pstSubtitleCompTable)\n");
	HLIB_RPC_CreateNotification (pstContext->nRpcHandle, RPC_OBAMA_DOWNLOAD_OnComponentDataChanged	, "iib", "BASE ComDataChanged.	\n\t   - (HUINT32 ulDownloadId, HUINT32 ulSessionId, OxDownload_DataCompTable_t *pstDataCompTable)\n");
	HLIB_RPC_CreateNotification (pstContext->nRpcHandle, RPC_OBAMA_DOWNLOAD_OnComponentSelected 		, "iiiii", "BASE ComSelected.		\n\t   - (HUINT32 ulDownloadId, HUINT32 ulSessionId, OxDownload_Component_e eCompType, HINT32 nSelectIndex, HINT32 nSelectSubIndex)\n");
	HLIB_RPC_CreateNotification (pstContext->nRpcHandle, RPC_OBAMA_DOWNLOAD_OnAvailabilityStartTimeChanged, "iis", "AvailabilityStartTime Changed.	\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, HCHAR *pszString)\n");

	/*sub*/
#if defined(CONFIG_OTA_IP)
	om_download_ipmedia_InitRpcFunctions(pstContext->nRpcHandle);
#endif
	return ERR_OK;
}

#define _____PROCEDURE_____
STATIC BUS_Result_t proc_om_download (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	omDownload_Context_t	*pstContext = om_download_GetContext();
	BUS_Result_t			 eResult = ERR_BUS_NO_ERROR;

	switch (message)
	{
	case eMEVT_BUS_CREATE:
		eResult = om_download_MsgBusCreate(hHandle, p1, p2, p3);
		break;

	case eMEVT_BUS_DESTROY:
		eResult = om_download_MsgBusDestroy(hHandle, p1, p2, p3);
		break;

	default:
		eResult = MESSAGE_PASS;
		break;
	}

	if (MESSAGE_BREAK != eResult)
	{
		if (pstContext->anInstNum[eDxDNLD_TYPE_IpMediaStream] > 0)
		{
		#if defined(CONFIG_OTA_IP)
			eResult = proc_om_download_ipmedia(message, hHandle, p1, p2, p3);
		#else
			eResult = MESSAGE_PASS;
		#endif
		}
	}

	return (MESSAGE_BREAK == eResult) ? MESSAGE_BREAK : MESSAGE_PASS;
}

#define ___MEMBER_FUNCTIONS___
omDownload_Context_t *om_download_GetContext (void)
{
	return &s_stOmDnld_Context;
}

OmDownload_Instance_t *OM_DOWNLOAD_GetInstance (HUINT32 ulDownloadId)
{
	HxList_t				*pstListItem;
	omDownload_Context_t	*pstContext = om_download_GetContext();

	for (pstListItem = pstContext->pstInstList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		OmDownload_Instance_t	*pstInst = (OmDownload_Instance_t *)HLIB_LIST_Data(pstListItem);

		if ((NULL != pstInst) && (ulDownloadId == pstInst->ulDownloadId))
		{
			return pstInst;
		}
	}

	return NULL;
}

OmDownload_Instance_t *OM_DOWNLOAD_GetInstanceWithCsoId (HUINT32 ulCalypsoId)
{
	HxList_t				*pstListItem;
	omDownload_Context_t	*pstContext = om_download_GetContext();

	for (pstListItem = pstContext->pstInstList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		OmDownload_Instance_t	*pstInst = (OmDownload_Instance_t *)HLIB_LIST_Data(pstListItem);

		if ((NULL != pstInst) && (ulCalypsoId == pstInst->unExtInfo.stIpMedia.ulCsoSessionId))
		{
			return pstInst;
		}
	}

	return NULL;
}

OmDownload_IpMediaInfo_t* OM_DOWNLOAD_GetMediaInfoByCsoId (HUINT32 ulCalypsoId)
{
	HxList_t				*pstListItem;
	omDownload_Context_t	*pstContext = om_download_GetContext();
	OmDownload_IpMediaInfo_t *pIpMediaInfo = NULL;
	for (pstListItem = pstContext->pstInstList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		OmDownload_Instance_t	*pstInst = (OmDownload_Instance_t *)HLIB_LIST_Data(pstListItem);

		if ((NULL != pstInst) && (ulCalypsoId == pstInst->unExtInfo.stIpMedia.ulCsoSessionId))
		{
			pIpMediaInfo = &pstInst->unExtInfo.stIpMedia.stMediaInfo;
			return pIpMediaInfo;
		}
	}

	return NULL;
}

HERROR om_download_RemoveInstance (HUINT32 ulDownloadId)
{
	omDownload_Context_t	*pstContext = om_download_GetContext();
	OmDownload_Instance_t	*pstInst = OM_DOWNLOAD_GetInstance(ulDownloadId);

	if (NULL == pstInst)				{ return ERR_FAIL; }

	pstContext->pstInstList = HLIB_LIST_Remove(pstContext->pstInstList, (const void *)pstInst);
	pstContext->anInstNum[pstInst->eType]--;

	OxAP_Free(pstInst);
	return ERR_FAIL;
}


#define ___GLOBAL_FUNCTIONS___
HERROR OM_DOWNLOAD_Init (void)
{
	BUS_MGR_Create("proc_om_download", APP_ACTION_PRIORITY, proc_om_download, HANDLE_NULL, 0, 0, 0);

	om_download_InitRpcFunctions();

	return ERR_OK;
}

