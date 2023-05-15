/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  om_download_ipmedia.c
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
#include <om_media_play.h>
#include <om_download.h>
#include "_om_download.h"
#include <pthread.h>
#include <db_param.h>
#include <isosvc.h>


#ifdef CONFIG_SUPPORT_IMMA
#include "cso.h"
#include "mediapb_ipc.h"
#include <svc_mplayercso.h>
#endif

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define TS_DECYPT_BLOCK_SIZE	(188 * 87)  /* Decrypt는 188 byte 배수로 decrypt 된다. */


//#define PREFERRED_LANGUAGE_SELECTION
#define IPMEDIA_BUFFERING_INTERVAL 250 // 250msec
#define IPMEDIA_PENDING_STOP_THRESHOLD 20
#define IPMEDIA_PENDING_EXIT_THRESHOLD 20
/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HINT32				 	s_nRpcHandle = 0;

static HUINT32 		s_uiIpMediaDownloadSemaId = 0;
static HUINT32 		s_uiRemoveScheduleListId = 0;
static HBOOL 		s_bCallbackRegistered = FALSE;

static HBOOL 					s_bIsAliveDownloadMgrThread = FALSE;
static pthread_t 				s_DownloadMgrThread;

HxList_t *pCsoSessionList = NULL;
HxList_t *s_pRemoveScheduleList = NULL;

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
extern HERROR om_media_play_mediapb_cso_prepareSession(HCHAR* pszUri, HCHAR* pSaveFilePath, OmMedia_MediapbInfo_t *pMediaInfo);

STATIC void om_download_ipmedia_CleanCache(HINT32 ulCsoSessionId);
STATIC HERROR om_download_ipmedia_DeleteFile (HBOOL bAllFiles, HINT32 ulDownloadId);
STATIC HERROR om_download_ipmedia_GetSeekableInfo(HUINT32 ulSessionId, OxDownload_SeekableInfo_t *pstSeekableInfo);

#define _____OAPI_CALLBACK_FUNCTIONS_____
STATIC void om_download_ipmedia_CbOnDownloadInfoChanged (HUINT32 ulDownloadId, HUINT32 ulSessionId, OxDownload_BufferInfo_t *pstDownloadInfo)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_DOWNLOAD_OnDownloadInfoChanged, "iiib", ulDownloadId, ulSessionId, 0, pstDownloadInfo, sizeof(OxDownload_BufferInfo_t));
}

#define ___LOCAL_FUNCTIONS___
STATIC void om_download_ipmedia_EnterSection(void)
{
	HxSEMT_Get(s_uiIpMediaDownloadSemaId);
	return;
}

STATIC void om_download_ipmedia_ExitSection(void)
{
	HxSEMT_Release(s_uiIpMediaDownloadSemaId);
	return;
}

STATIC void om_download_ipmedia_LockSession(CsoSessionDNLD_t* pSession)
{
	if(pSession && pSession->sema)
	{
		HxSEMT_Get(pSession->sema);
	}
	return;
}

STATIC void om_download_ipmedia_UnlockSession(CsoSessionDNLD_t* pSession)
{
	if(pSession && pSession->sema)
	{
		HxSEMT_Release(pSession->sema);
	}
	return;
}

STATIC CsoSessionDNLD_t* om_download_ipmedia_GetSession(HUINT32 unSessionId)
{
	CsoSessionDNLD_t *pSession = NULL;
	HxList_t * node = NULL;pCsoSessionList;

	om_download_ipmedia_EnterSection();
	node = pCsoSessionList;
	while (node)
	{
		pSession = (CsoSessionDNLD_t*)(node->data);
		if(pSession && pSession->unSessionId == unSessionId)
		{
			om_download_ipmedia_ExitSection();
			return pSession;
		}
		node = HLIB_LIST_Next(node);
	}
	om_download_ipmedia_ExitSection();
	return NULL;
}

STATIC CsoSessionDNLD_t* om_download_ipmedia_AddSession(HUINT32 unSessionId, HUINT32 unDownloadId)
{
	CsoSessionDNLD_t *pSession = NULL;
	HUINT8 szName[256] = {0,};

	pSession = OxMGR_Malloc(sizeof(CsoSessionDNLD_t));
	if(pSession == NULL)
	{
		return NULL;
	}
	pSession->unSessionId = unSessionId;
	pSession->unDownloadId = unDownloadId;
	pSession->bAsyncStop = FALSE;
	pSession->bDownloadCompleted = FALSE;
	pSession->bProbeCompleted = FALSE;
	pSession->unPendingStop = 0;
	pSession->pSettings = NULL;
	pSession->pProbeInfo = NULL;
	snprintf(szName,VK_strlen("IpMediaDownloadSessSema_") + 10,"IpMediaDownloadSessSema_%u",(HUINT32)pSession);
	HxSEMT_Create(&pSession->sema, szName, HxSEMT_FIFO);
	om_download_ipmedia_EnterSection();
	pCsoSessionList = HLIB_LIST_Prepend(pCsoSessionList,pSession);
	HxLOG_Debug("[%s](L:%d) Add Session %p\n",__FUNCTION__, __LINE__,pSession);
	om_download_ipmedia_ExitSection();
	return pSession;
}

STATIC void om_download_ipmedia_AsyncStopSession(CsoSessionDNLD_t* pSession)
{
	if(pSession == NULL)
	{
		return;
	}

//	HxSEMT_Get(pSession->sema);
	HxLOG_Debug("[%s](L:%d) AsyncStop Session %p\n",__FUNCTION__, __LINE__,pSession);
	pSession->bAsyncStop = TRUE;
//	HxSEMT_Release(pSession->sema);
	return;
}

STATIC HERROR om_download_ipmedia_StopSession(CsoSessionDNLD_t* pSession)
{
	CSO_CTRL_PARAM_t tParam;
	CSO_ERR 	err;

	HxSTD_memset(&tParam,0x0,sizeof(CSO_CTRL_PARAM_t));

	err = MEDIAPB_IPC_CSO_Control(pSession->unSessionId, eCSO_CTRL_STOPBUFFERING, &tParam);
	if(err != CSO_ERR_OK)
	{
		HxLOG_Error("Calypso> stop buffering has failed but keep going cleanup session.\n");
	}

	err = MEDIAPB_IPC_CSO_Close(pSession->unSessionId);
	if(err != CSO_ERR_OK)
	{
		HxLOG_Error("Calypso> close session has failed!!\n");
		return ERR_FAIL;
	}

	HxLOG_Debug(">> DownloadId(0x%08x), CSO Session ID:(%d)\n", pSession->unDownloadId, pSession->unSessionId);

	if (pSession->pSettings)
	{
		(void)MEDIAPB_IPC_CSO_IPCMSG_MemFree_Settings(*((CSO_Settings_t*)pSession->pSettings));
		OxMGR_Free( pSession->pSettings );
		pSession->pSettings = NULL;
	}

	return ERR_OK;
}

STATIC void om_download_ipmedia_RemoveSession(CsoSessionDNLD_t* pSession)
{
	HUINT32 sema;
	if(pSession == NULL)
	{
		HxLOG_Error("[%s](L:%d) pSession is NULL!!!\n", __FUNCTION__, __LINE__);
		return;
	}
	/* clean cache */
	om_download_ipmedia_CleanCache (pSession->unSessionId);
	// TODO: CSO 에서 사용중일 때, 지우면 큰일.... Close evt 를 올리게 한 뒤 지우는게 좋을 듯.
	om_download_ipmedia_DeleteFile(FALSE, pSession->unDownloadId);

	sema = pSession->sema;
	pSession->sema = 0;
	HxSEMT_Get(sema);
	HxLOG_Debug("[%s](L:%d) Remove Session %p\n",__FUNCTION__, __LINE__,pSession);

	HxSEMT_Get(s_uiRemoveScheduleListId);
	s_pRemoveScheduleList = HLIB_LIST_Remove(s_pRemoveScheduleList,pSession);
	HxSEMT_Release(s_uiRemoveScheduleListId);

	if (pSession->pProbeInfo)
	{
		OxMGR_Free(pSession->pProbeInfo);
	}
	OxMGR_Free(pSession);
	HxSEMT_Release(sema);
	HxSEMT_Destroy(sema);
	return;
}

STATIC void om_download_ipmedia_cso_MemFree_BufferInfo(CSO_BufferInfo_t *pInfo)
{
	if (pInfo == NULL)
		return;

	if(pInfo->tBufferChunkStatus.pullStartOffsets != NULL ) 
	{
		free(pInfo->tBufferChunkStatus.pullStartOffsets);
		pInfo->tBufferChunkStatus.pullStartOffsets = NULL;
	}
	if(pInfo->tBufferChunkStatus.pullEndOffsets != NULL ) 
	{
		free(pInfo->tBufferChunkStatus.pullEndOffsets);
		pInfo->tBufferChunkStatus.pullEndOffsets = NULL;
	}

	if(pInfo->tSeekableRange.pullSeekableStart != NULL ) 
	{
		free(pInfo->tSeekableRange.pullSeekableStart);
		pInfo->tSeekableRange.pullSeekableStart = NULL;
	}
	if(pInfo->tSeekableRange.pullSeekableEnd != NULL ) 
	{
		free(pInfo->tSeekableRange.pullSeekableEnd);
		pInfo->tSeekableRange.pullSeekableEnd = NULL;
	}
}

STATIC HERROR om_download_ipmedia_UpdateBufferInfo(CsoSessionDNLD_t *pSession, OxDownload_BufferInfo_t *pBufferInfo)
{
	CSO_BufferInfo_t stCSOBufferInfo = { 0 };
	CSO_ERR err = CSO_ERR_FAIL;
	HUINT32  ulSessionId	 =	 0;
	HUINT32	ulDownloadId	= 0;
	HUINT32  ulIdx			 =	 0;

	ulSessionId = pSession->unSessionId;
	ulDownloadId = pSession->unDownloadId;

	err = MEDIAPB_IPC_CSO_BufferInfo(ulSessionId, &stCSOBufferInfo);
	if ( err == CSO_ERR_INVALID_SESSION)
	{
		HxLOG_Error("[%s](L:%d) Error - MEDIAPB_IPC_CSO_BufferInfo (Invalid Session)\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}
	else if ( err != CSO_ERR_OK)
	{
		HxLOG_Error("[%s](L:%d) Error - MEDIAPB_IPC_CSO_BufferInfo\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	/* Fill buffer status */
	HxLOG_Debug("[%s](L:%d) ---------------------------------- \n", __FUNCTION__, __LINE__);
	HxLOG_Debug("[%s](L:%d) DownloadInfo(%u:Handle_%u) \n", __FUNCTION__, __LINE__, ulSessionId,ulDownloadId);
	HxLOG_Debug("[%s](L:%d) ---------------------------------- \n", __FUNCTION__, __LINE__);
	if( stCSOBufferInfo.tBufferChunkStatus.uiChunks > 0 )
	{
		/* TODO: will need change Dynamic Chunks */
		if(stCSOBufferInfo.tBufferChunkStatus.uiChunks > 20)
			stCSOBufferInfo.tBufferChunkStatus.uiChunks = 20;

		pBufferInfo->ulChunks = stCSOBufferInfo.tBufferChunkStatus.uiChunks;

		for(ulIdx=0; ulIdx<stCSOBufferInfo.tBufferChunkStatus.uiChunks; ulIdx++)
		{
			pBufferInfo->ullStartOffsets[ulIdx] = stCSOBufferInfo.tBufferChunkStatus.iStartOffsets[ulIdx];
			pBufferInfo->ullEndOffsets[ulIdx] = stCSOBufferInfo.tBufferChunkStatus.iEndOffsets[ulIdx];
			HxLOG_Debug("[%s](L:%d) Idx:%u Start:%u End:%u  \n", __FUNCTION__, __LINE__,
					ulIdx, pBufferInfo->ullStartOffsets[ulIdx], pBufferInfo->ullEndOffsets[ulIdx]);
		}
	}

	pBufferInfo->nArrivalBytedPerSec = stCSOBufferInfo.tBufferStatus.nArrivalBytedPerSecond;
	pBufferInfo->ulBufferPercent = stCSOBufferInfo.tBufferStatus.uiBufferPercentage;
	pBufferInfo->unRemainBufferingTime = stCSOBufferInfo.tBufferStatus.uiRemainBufferSec;
	pBufferInfo->llBufferedBytes = stCSOBufferInfo.tBufferStatus.llBufferedBytes;
	pBufferInfo->llTotalBytesRemaining = stCSOBufferInfo.tBufferStatus.llTotalBytesRemaining;
	pBufferInfo->llTotalStreamBytes = stCSOBufferInfo.tBufferStatus.llTotalStreamBytes;
	pBufferInfo->nStreamBytesPerSec = stCSOBufferInfo.tBufferStatus.nStreamBytesPerSecond;
	HxLOG_Debug("[%s](L:%d) nArrivalBytedPerSec(%d) \n", __FUNCTION__, __LINE__, pBufferInfo->nArrivalBytedPerSec);
	HxLOG_Debug("[%s](L:%d) ulBufferPercent(%u)\n", __FUNCTION__, __LINE__, pBufferInfo->ulBufferPercent);
	HxLOG_Debug("[%s](L:%d) unRemainBufferingTime(%u)\n", __FUNCTION__, __LINE__, pBufferInfo->unRemainBufferingTime);
	HxLOG_Debug("[%s](L:%d) llBufferedBytes(%lld) \n", __FUNCTION__, __LINE__, pBufferInfo->llBufferedBytes);
	HxLOG_Debug("[%s](L:%d) llTotalBytesRemaining(%lld) \n", __FUNCTION__, __LINE__, pBufferInfo->llTotalBytesRemaining);
	HxLOG_Debug("[%s](L:%d) llTotalStreamBytes(%lld) \n", __FUNCTION__, __LINE__, pBufferInfo->llTotalStreamBytes);
	HxLOG_Debug("[%s](L:%d) nStreamBytesPerSec(%d) \n", __FUNCTION__, __LINE__, pBufferInfo->nStreamBytesPerSec);

	if( stCSOBufferInfo.tSeekableRange.uiChunks > 0)
	{
		if(stCSOBufferInfo.tSeekableRange.uiChunks > 20)
			stCSOBufferInfo.tSeekableRange.uiChunks = 20;

		pBufferInfo->stSeekableInfo.ulSeekableChunks = stCSOBufferInfo.tSeekableRange.uiChunks;
		HxLOG_Debug("[%s](L:%d) Seekable chunks : %d\n", __FUNCTION__, __LINE__, stCSOBufferInfo.tSeekableRange.uiChunks);
		for(ulIdx=0; ulIdx<stCSOBufferInfo.tSeekableRange.uiChunks; ulIdx++)
		{
			pBufferInfo->stSeekableInfo.ullSeekableStart[ulIdx] = stCSOBufferInfo.tSeekableRange.pullSeekableStart[ulIdx];
			pBufferInfo->stSeekableInfo.ullSeekableEnd[ulIdx] = stCSOBufferInfo.tSeekableRange.pullSeekableEnd[ulIdx];
			HxLOG_Debug("[%s](L:%d) Idx:%u Seekabe Start:%llu, Seekable End:%llu\n", __FUNCTION__, __LINE__,
					ulIdx, pBufferInfo->stSeekableInfo.ullSeekableStart[ulIdx], pBufferInfo->stSeekableInfo.ullSeekableEnd[ulIdx]);
		}
	}
#if ((CALYPSO_VERSION_MAJOR >= 1) && (CALYPSO_VERSION_MINOR >= 1) && (CALYPSO_VERSION_MICRO >= 18))
	MEDIAPB_IPC_CSO_IPCMSG_MemFree_BufferInfo(&stCSOBufferInfo);
#else
	om_download_ipmedia_cso_MemFree_BufferInfo(&stCSOBufferInfo);
#endif
	HxLOG_Debug("[%s](L:%d) ---------------------------------- \n\n", __FUNCTION__, __LINE__);

	return ERR_OK;
}

STATIC void	*om_download_ipmedia_DownloadMgrTask(void *pParam)
{
	CsoSessionDNLD_t *pSession = NULL;
	HUINT32	ulPendingExit  	 =	 0;
	HUINT32	ulSessionSetIdx	 =	 0;
	HUINT32 ulBuffringInterval = IPMEDIA_BUFFERING_INTERVAL;
	OxDownload_BufferInfo_t stBufferInfo;

	CsoSessionDNLD_t *pSessionSet[OxDNLD_CACHE_NUM] = { NULL };
	CsoSessionDNLD_t *pRemoveSessionSet[OxDNLD_CACHE_NUM] = { NULL };
	HxList_t * node = NULL;

	while(TRUE)
	{
		if(pCsoSessionList == NULL && s_pRemoveScheduleList == NULL)
		{
			if ( ulPendingExit < IPMEDIA_PENDING_EXIT_THRESHOLD)
			{
				VK_TASK_Sleep(250);
				ulPendingExit++;
				continue;
			}

			s_bIsAliveDownloadMgrThread = FALSE;
			s_DownloadMgrThread = 0;
			break;
		}
		else
		{
			ulPendingExit = 0;
		}

		/* reset  pSessionSet */
		ulSessionSetIdx = 0;
		HxSTD_MemSet(pSessionSet ,0x00, sizeof(CsoSessionDNLD_t *) * OxDNLD_CACHE_NUM );

		om_download_ipmedia_EnterSection();
		node = pCsoSessionList;
		while(node != NULL)
		{
			if(ulSessionSetIdx >= OxDNLD_CACHE_NUM )
			{
				break;
			}
			pSessionSet[ulSessionSetIdx] = (CsoSessionDNLD_t*)(node->data);
			ulSessionSetIdx++;

			node = HLIB_LIST_Next(node);
		}
		om_download_ipmedia_ExitSection();

		ulSessionSetIdx = 0;
		while(ulSessionSetIdx < OxDNLD_CACHE_NUM)
		{
			pSession = (CsoSessionDNLD_t*)(pSessionSet[ulSessionSetIdx]);
			if (pSession == NULL)
			{
				break;
			}

			if (!pSession->bDownloadCompleted && pSession->bProbeCompleted)
			{
				HxSTD_MemSet(&stBufferInfo, 0, sizeof(OxDownload_BufferInfo_t));

				if (om_download_ipmedia_UpdateBufferInfo(pSession, &stBufferInfo) == ERR_OK)
				{
					om_download_ipmedia_CbOnDownloadInfoChanged(pSession->unDownloadId , pSession->unSessionId, &stBufferInfo);
				}
			}
			ulSessionSetIdx++;
		}

		/* reset pRemoveSessionSet */
		ulSessionSetIdx = 0;
		HxSTD_MemSet(pRemoveSessionSet ,0x00, sizeof(CsoSessionDNLD_t *) * OxDNLD_CACHE_NUM);

		HxSEMT_Get(s_uiRemoveScheduleListId);
		node = s_pRemoveScheduleList;
		while(node != NULL)
		{
			if(ulSessionSetIdx >= OxDNLD_CACHE_NUM )
			{
				break;
			}
			pRemoveSessionSet[ulSessionSetIdx] = (CsoSessionDNLD_t*)(node->data);
			ulSessionSetIdx++;

			node = HLIB_LIST_Next(node);
		}
		HxSEMT_Release(s_uiRemoveScheduleListId);

		ulSessionSetIdx = 0;
		while(ulSessionSetIdx < OxDNLD_CACHE_NUM)
		{
			pSession = (CsoSessionDNLD_t*)(pRemoveSessionSet[ulSessionSetIdx]);
			if (pSession == NULL)
			{
				break;
			}

			if (pSession->unPendingStop < IPMEDIA_PENDING_STOP_THRESHOLD)
			{
				pSession->unPendingStop++;
			}
			else if (pSession->unPendingStop == IPMEDIA_PENDING_STOP_THRESHOLD)
			{
				om_download_ipmedia_StopSession(pSession);
				pSession->unPendingStop++;
			}
			else
			{
				om_download_ipmedia_RemoveSession(pSession);
			}

			ulSessionSetIdx++;
		}

		VK_TASK_Sleep(ulBuffringInterval);
	}

	HxLOG_Debug("[%s](L:%d) Exit Task!!!\n", __FUNCTION__, __LINE__);

	om_download_ipmedia_EnterSection();
	s_bIsAliveDownloadMgrThread = FALSE;
	s_DownloadMgrThread = 0;
	om_download_ipmedia_ExitSection();
	VK_TASK_Sleep(100);
	return NULL;
}

STATIC HERROR om_download_ipmedia_CreateDownloadMgrTask(void)
{
	HINT32 nRet = 0;
	pthread_attr_t stack_attr;
	size_t stack_size = (32*1024);

//	if (NULL == pstContext) { return ERR_FAIL; }
//	if (NULL == pstContext->pExtension) 	{ return ERR_FAIL; }

	nRet = pthread_attr_init(&stack_attr);
	if (nRet < 0)
	{
		return ERR_FAIL;
	}
	else
	{
#if	0
		if (stack_size < PTHREAD_STACK_MIN)
		{
			cso_log("Error> Invalid stack size(%d)\n", stack_size);
			CSO_PRINT_ERROR(CSO_ERR_FAIL);
		}
#endif
		if( s_DownloadMgrThread > 0 )
		{
			HxLOG_Debug("[%s](L:%d) Buffering Info Task is already started\n", __FUNCTION__, __LINE__);
			return ERR_OK;
		}
		/* joinable */
		(void)pthread_attr_setdetachstate(&stack_attr, PTHREAD_CREATE_DETACHED);
		(void)pthread_attr_setstacksize(&stack_attr, stack_size);
		if (pthread_create(&s_DownloadMgrThread, &stack_attr, om_download_ipmedia_DownloadMgrTask,NULL) < 0)
		{
			return ERR_FAIL;
		}
		pthread_attr_destroy(&stack_attr);
		s_bIsAliveDownloadMgrThread = TRUE;
	}
	HxLOG_Debug("[%s](L:%d) Buffering Info Task Start\n", __FUNCTION__, __LINE__);
	return ERR_OK;
}


STATIC HERROR om_download_ipmedia_DeleteFile (HBOOL bAllFiles, HINT32 ulDownloadId)
{
	HERROR 					hErr = ERR_OK;
	SvcCso_Config_t			svcDnpConfig;
	HCHAR* 					pFilePath = NULL;
	HCHAR* 					pDeleteCmd = NULL;
	HUINT32					nLen = 0;

	HxSTD_memset(&svcDnpConfig,0x0,sizeof(SvcCso_Config_t));
	hErr = SVC_CSO_GetDnPConfig(NULL, (SvcCso_Config_t*)&svcDnpConfig);
	if ( hErr != ERR_OK )
	{
		HxLOG_Error ("SVC_CSO_GetDnPConfig() Failed.\n");
		return ERR_FAIL;
	}
	if(svcDnpConfig.bProgressiveDownload == FALSE)
	{
		HxLOG_Error ("not support download\n");
		return hErr;
	}
	pFilePath = OxMGR_Malloc(DxDNLD_MEDIAPLAY_URL);
	if(pFilePath == NULL)
	{
		HxLOG_Error("Memory Allocation failed for file path!\n");
		return ERR_FAIL;
	}
#define DELETE_CMD 6 //"rm -f "
	pDeleteCmd = OxMGR_Malloc(DxDNLD_MEDIAPLAY_URL + DELETE_CMD);
	if(pDeleteCmd == NULL)
	{
		HxLOG_Error("Memory Allocation failed for delete cmd!\n");
		OxMGR_Free(pFilePath);
		return ERR_FAIL;
	}
	HxSTD_memset(pFilePath,0x0,DxDNLD_MEDIAPLAY_URL);
	if(bAllFiles)
		snprintf((char*)pFilePath, DxDNLD_MEDIAPLAY_URL, "%s[0-9,A-z]*", svcDnpConfig.szPDLPath);
	else
		snprintf((char*)pFilePath, DxDNLD_MEDIAPLAY_URL, "%s%u", svcDnpConfig.szPDLPath,ulDownloadId);
	HxLOG_Error("Remove file path [ %s ]\n",pFilePath);

	/* get system path */
	if ( NULL != strstr((char*)pFilePath,"dvr://") )
	{
		nLen = VK_strlen("dvr://");
	}
	else if (NULL != strstr((char*)pFilePath, "file://") )
	{
		nLen = VK_strlen("file://");
	}
	else if (NULL == strstr((char*)pFilePath, "://") )
	{
		nLen = 0;
	}
	else
	{
		/* already checked a SavePath has valid prefix. */
		HxLOG_Error("%s(%d) Error> (%s) is invalid file path [ %s ]\n",__FUNCTION__,__LINE__,pFilePath);
		OxMGR_Free(pFilePath);
		OxMGR_Free(pDeleteCmd);
		return ERR_OK;
	}

	HxSTD_memset(pDeleteCmd,0x0,DxDNLD_MEDIAPLAY_URL + DELETE_CMD);
	snprintf((char*)pDeleteCmd, DxDNLD_MEDIAPLAY_URL + DELETE_CMD, "rm -f %s", pFilePath + nLen); //delete에 삭제할 디렉토리 위치 저장
	HxLOG_Error("Cmd start [ %s ]\n",pDeleteCmd);
	system(pDeleteCmd); //system명령어로 실행

	OxMGR_Free(pFilePath);
	OxMGR_Free(pDeleteCmd);

	return ERR_OK;
}

STATIC int om_download_ipmedia_cso_doConvertVideoCodec( HUINT32 CodecId )
{
    int codec = 0;

    switch (CodecId) {
	    case CSO_VCODEC_H264:
		    codec = eDxVIDEO_CODEC_H264;
		    break;
	    case CSO_VCODEC_MPEG1:
		    codec = eDxVIDEO_CODEC_MPEG1;
		    break;
	    case CSO_VCODEC_MPEG2:
		    codec = eDxVIDEO_CODEC_MPEG2;
		    break;
	    case CSO_VCODEC_MPEG4Part2:
		    codec = eDxVIDEO_CODEC_MPEG4_PART2;
		    break;
	    case CSO_VCODEC_VC1:
		    codec = eDxVIDEO_CODEC_VC1;
		    break;
	    case CSO_VCODEC_WMV2:
		    HxLOG_Error("we do not support WMV2 codec !!!\n");
		    break;
	    case CSO_VCODEC_VC1SimpleMain:
		    codec = eDxVIDEO_CODEC_VC1_SIMPLEMAIN;
		    break;
	    case CSO_VCODEC_MSMPEGV3:
		    codec = eDxVIDEO_CODEC_DIVX311;
		    break;
	    case CSO_VCODEC_H265:
		    codec = eDxVIDEO_CODEC_H265;
		    break;
#if defined(CALYPSO_REQUIRED_VERSION)
#if CALYPSO_REQUIRED_VERSION(1,1,11,0,0)
		case CSO_VCODEC_VP8:
			codec = eDxVIDEO_CODEC_VP8;
			break;
		case CSO_VCODEC_VP9:
			codec = eDxVIDEO_CODEC_VP9;
			break;
#endif
#endif
	    default :
		    HxLOG_Error("error - invalid  codec (%d) !!!  \n", CodecId);
		    codec = eDxVIDEO_CODEC_UNKNOWN;
		    break;
    }

	if ( codec != eDxVIDEO_CODEC_UNKNOWN )
	{
    		if ( !SVC_CSO_IsVideoCodecSupport(codec) )
		{
			HxLOG_Error("error - codec (%d) is not supported by Video Decoder!!!  \n", CodecId);
			codec = eDxVIDEO_CODEC_UNKNOWN;
		}
	}

    return codec;
}

STATIC int om_download_mediapb_cso_doConvertAudioCodec( HUINT32 CodecId )
{
	int codec = 0;

	switch (CodecId) {
		case CSO_ACODEC_MP3:
			codec = eDxAUDIO_CODEC_MP3;
			break;
		case CSO_ACODEC_AAC:
			codec = eDxAUDIO_CODEC_AAC;
			break;
		case CSO_ACODEC_AAC_PLUS_LOAS:
			codec = eDxAUDIO_CODEC_AAC_LOAS;
			break;
		case CSO_ACODEC_AAC_PLUS_ADTS:
			codec = eDxAUDIO_CODEC_AAC_PLUS_ADTS;
			break;
		case CSO_ACODEC_AC3:
			codec = eDxAUDIO_CODEC_DOLBY_AC3;
			break;
		case CSO_ACODEC_AC3_PLUS:
			codec = eDxAUDIO_CODEC_DOLBY_AC3P;
			break;
		case CSO_ACODEC_MPEG:
			codec = eDxAUDIO_CODEC_MPEG;
			break;
		case CSO_ACODEC_WMA_STD:
			codec = eDxAUDIO_CODEC_WMA_STD;
			break;
		case CSO_ACODEC_WMA_PRO:
			codec = eDxAUDIO_CODEC_WMA_PRO;
			break;
		case CSO_ACODEC_FLAC:
			codec = eDxAUDIO_CODEC_FLAC;
			break;
		case CSO_ACODEC_PCM_S16BE:
		case CSO_ACODEC_PCM_S16LE:
		case CSO_ACODEC_PCM_S24BE:
		case CSO_ACODEC_PCM_S24LE:
		case CSO_ACODEC_PCM_S32BE:
		case CSO_ACODEC_PCM_S32LE:
		case CSO_ACODEC_PCM_DVD:
			codec = eDxAUDIO_CODEC_PCMWAVE;
			break;
		case CSO_ACODEC_ADPCM_MS:
			codec = eDxAUDIO_CODEC_ADPCM;
			break;
#if ((CALYPSO_VERSION_MAJOR >= 1) && (CALYPSO_VERSION_MINOR >= 1) && (CALYPSO_VERSION_MICRO >= 11))
		case CSO_ACODEC_VORBIS:
			codec = eDxAUDIO_CODEC_VORBIS;
			break;
		case CSO_ACODEC_OPUS:
			codec = eDxAUDIO_CODEC_OPUS;
			break;
#endif
		case CSO_ACODEC_DTS:
			HxLOG_Error("error - Can't Play DTS. !!!\n");
			codec = eDxAUDIO_CODEC_UNKNOWN;
			break;
		default :
			HxLOG_Error("error - invalid  codec (%d) !!!\n", CodecId);
			codec = eDxAUDIO_CODEC_UNKNOWN;
			break;
	}
	return codec;
}

#if defined(CONFIG_PROD_FVP4000T)
STATIC int om_download_mediapb_cso_doConvertSubtitleType( HUINT32 type )
{
    int subtype = eOxMP_SUBTITLETYPE_Unknown;

    switch (type) {
	case eCSO_SUBTITLE_DVB :
		subtype = eOxMP_SUBTITLETYPE_Dvb;
		break;
	case eCSO_SUBTITLE_EBU :
		subtype = eOxMP_SUBTITLETYPE_Ebu;
		break;
	case eCSO_SUBTITLE_XSUB_DXSA :
		break;
	case eCSO_SUBTITLE_XSUB_DXSB :
		break;
	case eCSO_SUBTITLE_TTML :
		break;
	case eCSO_SUBTITLE_TEXT :
		break;
	case eCSO_SUBTITLE_NONE :
	default :
		HxLOG_Error("error - invalid subtype !!!\n");
		subtype = eOxMP_SUBTITLETYPE_Unknown;
		break;
    }
    return subtype;
}
#endif

STATIC void om_download_ipmedia_CleanCache(HINT32 ulCsoSessionId)
{
	omDownload_Context_t	*pstContext = om_download_GetContext();
	if( NULL == pstContext )
		return ;

	HxLOG_Print("om_download_ipmedia_CleanCache > ulSessionId : %d\n", ulCsoSessionId);

	/* clean cache */
	if ( ulCsoSessionId < OxDNLD_CACHE_NUM)
	{
		OxDownload_Cache_t *pCache = &pstContext->stCache[ulCsoSessionId];
		HxSTD_MemSet(&(pCache->stVideoCompTable),		0, sizeof(OxDownload_VideoCompTable_t) );
		HxSTD_MemSet(&(pCache->stAudioCompTable),		0, sizeof(OxDownload_AudioCompTable_t) );
		HxSTD_MemSet(&(pCache->stSubtitleCompTable),	0, sizeof(OxDownload_SubtitleCompTable_t) );
		HxSTD_MemSet(&(pCache->stDataCompTable),		0, sizeof(OxDownload_DataCompTable_t) );
	}
	else
	{
		HxLOG_Error("[%s](L:%d) ulCsoSessionId(%d) >= OxDNLD_CACHE_NUM(%d)\n", __FUNCTION__, __LINE__, ulCsoSessionId, OxDNLD_CACHE_NUM);
	}
	return ;
}


/**
* @b Function @b Description <br>
* this function makes Probing --> probed or probe failed
*
* @param[in] omMediaPlay_Context_t   : media pb context
*
* @return  #  returns ERR_OK or ERR_FAIL
*
* @attention don't call any functions with om_media_play_mediapb_cso_* named, or State machine may corrupted!
*/
STATIC HERROR om_download_ipmedia_TransferMetadata(CsoSessionDNLD_t *pSession, CSO_ProbeInfo_t *cso_probeinfo)
{
	HUINT32 		tracks = 0;
	HUINT32 		nTotalVideoTrack = 0;
	HUINT32 		nTotalAudioTrack = 0;
	HUINT32 		nTotalSubttlTrack = 0;
	HBOOL			bVideoPlayable = FALSE;
	HBOOL			bAudioPlayable = FALSE;
	HBOOL			bSubtitlePlayable = FALSE;
	HUINT16 		uValidVTrackCnt = 0; /* debugging */
	HUINT16 		uValidATrackCnt = 0; /* debugging */
	HUINT16 		uValidSTrackCnt = 0; /* debugging */
	OxDownload_AudioCompTable_t *pstAudioCompTable	= NULL;
	OxDownload_VideoCompTable_t *pstVideoCompTable	= NULL;
	OxDownload_SubtitleCompTable_t *pstSubtitleCompTable = NULL;

	omDownload_Context_t	*pstContext = om_download_GetContext();
	HINT32 ulDownloadId = 0;
	HUINT32 ulSessionId = 0;

	OxDownload_Cache_t *pCache = NULL;
	if( pstContext == NULL || pSession == NULL)
	{
		return ERR_FAIL;
	}

//	if (NULL == pstContext) { return ERR_FAIL; }
//	if (NULL == pstContext->pExtension) 	{ return ERR_FAIL; }
	ulDownloadId = pSession->unDownloadId;
	ulSessionId = pSession->unSessionId;

	pCache = &pstContext->stCache[ulSessionId];
	pstAudioCompTable = &pCache->stAudioCompTable;
	pstVideoCompTable =	&pCache->stVideoCompTable;
	pstSubtitleCompTable = &pCache->stSubtitleCompTable;

		/* AvailabilityStartTime 획득*/
	HxSTD_memcpy(pCache->szAvailabilityStartTime, cso_probeinfo->uacAvailabilityStartTime, sizeof(pCache->szAvailabilityStartTime));
	HxLOG_Debug("AvailabilityStartTime : %s\n", pCache->szAvailabilityStartTime);
	if(HxSTD_StrLen(pCache->szAvailabilityStartTime) > 0)
	{
		om_download_CbOnAvailabilityStartTimeChanged(ulDownloadId, ulSessionId, pCache->szAvailabilityStartTime);
	}

		/* VideoTrack 정보 획득...*/
	uValidVTrackCnt = 0;
	HxSTD_MemSet (pstVideoCompTable, 0, sizeof(OxDownload_VideoCompTable_t));
	if(cso_probeinfo->ucTotalVideoTrack > OxDOWNLOAD_VIDEO_ES_NUM_MAX)
	{
		HxLOG_Error ("MAX OxDOWNLOAD_VIDEO_ES_NUM_MAX is %d, But video track is %d from CSO_Probe. So the number of emitting OxMP_VIDEO_ES_NUM_MAX is %d\n", OxDOWNLOAD_VIDEO_ES_NUM_MAX, cso_probeinfo->ucTotalVideoTrack, OxDOWNLOAD_VIDEO_ES_NUM_MAX);
		nTotalVideoTrack = OxDOWNLOAD_VIDEO_ES_NUM_MAX;
	}
	else
	{
		nTotalVideoTrack = cso_probeinfo->ucTotalVideoTrack;
	}
	for ( tracks = 0; tracks < nTotalVideoTrack; tracks++ )
	{
		pstVideoCompTable->nCurrIndex		=	tracks;
		pstVideoCompTable->nArraySize		=	tracks+1;
		pstVideoCompTable->astCompArray[tracks].nPid		=	cso_probeinfo->stVideoTrack[tracks].video_stream_id;
		pstVideoCompTable->astCompArray[tracks].eCodec		=	om_download_ipmedia_cso_doConvertVideoCodec(cso_probeinfo->stVideoTrack[tracks].video_codec);
		pstVideoCompTable->astCompArray[tracks].eCompType 	=	eOxDNLD_COMPONENT_VIDEO;
		pstVideoCompTable->astCompArray[tracks].nHdVideo	=	0; // N/A
		pstVideoCompTable->astCompArray[tracks].nComponentTag =	0; // N/A
		pstVideoCompTable->astCompArray[tracks].nCasFound 	=	0; // N/A
		HxSTD_memcpy(pstVideoCompTable->astCompArray[tracks].aucCompName, cso_probeinfo->stVideoTrack[tracks].uacVideoName, OxMP_COMP_NAME_NUM);
		HxSTD_memcpy(pstVideoCompTable->astCompArray[tracks].aucCompRole, cso_probeinfo->stVideoTrack[tracks].uacVideoRole, OxMP_COMP_ROLE_NUM);

		if ( pstVideoCompTable->astCompArray[tracks].eCodec != eDxVIDEO_CODEC_UNKNOWN )
		{
			bVideoPlayable = TRUE;
			//pstMediaPlay->ulActivatedVideoCodecId = om_media_play_mediapb_cso_doConvertVideoCodec(cso_probeinfo->video_codec);
			//pstMediaPlay->ulActivatedVideoStreamId = cso_probeinfo->video_stream_id;
			uValidVTrackCnt++;
		}
	}
	pstVideoCompTable->nMaxSize			=	sizeof(OxDownload_VideoCompTable_t);

	for( tracks = 0; tracks < pstVideoCompTable->nArraySize; tracks++ )
	{
		HxLOG_Debug ("[%s](L:%d) Found track %d video codec %d \n", __FUNCTION__, __LINE__, tracks, pstVideoCompTable->astCompArray[tracks].eCodec );
	}

	/* AudioTrack 정보 획득...*/
	uValidATrackCnt = 0;
	HxSTD_MemSet (pstAudioCompTable, 0, sizeof(OxDownload_AudioCompTable_t));
	if(cso_probeinfo->ucTotalAudioTrack > OxDOWNLOAD_AUDIO_ES_NUM_MAX)
	{
		HxLOG_Error ("MAX OxDOWNLOAD_AUDIO_ES_NUM_MAX is %d, But audio track is %d from CSO_Probe. So the number of emitting OxDOWNLOAD_AUDIO_ES_NUM_MAX is %d\n", OxDOWNLOAD_AUDIO_ES_NUM_MAX, cso_probeinfo->ucTotalAudioTrack, OxDOWNLOAD_AUDIO_ES_NUM_MAX);
		nTotalAudioTrack = OxDOWNLOAD_AUDIO_ES_NUM_MAX;
	}
	else
	{
		nTotalAudioTrack = cso_probeinfo->ucTotalAudioTrack;
	}
	for ( tracks = 0; tracks < nTotalAudioTrack; tracks++ )
	{
		pstAudioCompTable->nCurrIndex		=	tracks;
		pstAudioCompTable->nArraySize		=	tracks+1;
		pstAudioCompTable->astCompArray[tracks].nPid			=	cso_probeinfo->stAudioTrack[tracks].audio_stream_id;
		pstAudioCompTable->astCompArray[tracks].eCompType 	=	eOxDNLD_COMPONENT_AUDIO;
		pstAudioCompTable->astCompArray[tracks].nComponentTag =	0; //N/A
		pstAudioCompTable->astCompArray[tracks].nLangNum		=	0; //stCsoAudioTrack.szAudioLanguage
		HxSTD_memcpy(pstAudioCompTable->astCompArray[tracks].aucCompRole, cso_probeinfo->stAudioTrack[tracks].uacAudioRole, OxMP_COMP_ROLE_NUM);
		HxSTD_memcpy(pstAudioCompTable->astCompArray[tracks].aucCompName, cso_probeinfo->stAudioTrack[tracks].uacAudioName, OxMP_COMP_NAME_NUM);

		/* 우선 CSO로 얻은 Audio Lang은 0번째 LangInfor에 저장한다. */
		HxSTD_memcpy(pstAudioCompTable->astCompArray[tracks].astLangInfo[0].szLangCode, cso_probeinfo->stAudioTrack[tracks].uacAudioLang, 3);
		pstAudioCompTable->astCompArray[tracks].eCodec		=	om_download_mediapb_cso_doConvertAudioCodec(cso_probeinfo->stAudioTrack[tracks].audio_codec);
		if ( pstAudioCompTable->astCompArray[tracks].eCodec == eDxAUDIO_CODEC_DOLBY_AC3 || pstAudioCompTable->astCompArray[tracks].eCodec == eDxAUDIO_CODEC_DOLBY_AC3P)
		{
			pstAudioCompTable->astCompArray[tracks].bIsDolby		=	TRUE;
		}
		else
		{
			pstAudioCompTable->astCompArray[tracks].bIsDolby		=	FALSE;
		}

		if ( pstAudioCompTable->astCompArray[tracks].eCodec !=  eDxAUDIO_CODEC_UNKNOWN)
		{
			bAudioPlayable = TRUE;
			//pstMediaPlay->ulActivatedAudioCodecId = om_media_play_mediapb_cso_doConvertAudioCodec(cso_probeinfo->audio_codec);
			//pstMediaPlay->ulActivatedAudioStreamId = cso_probeinfo->audio_stream_id;
			uValidATrackCnt++;
		}
	}
	pstAudioCompTable->nMaxSize			=	sizeof(OxDownload_AudioCompTable_t);

	for( tracks = 0; tracks < pstAudioCompTable->nArraySize; tracks++ )
	{
		HxLOG_Debug ("[%s](L:%d) Found track %d audio codec %d \n", __FUNCTION__, __LINE__, tracks, pstAudioCompTable->astCompArray[tracks].eCodec );
	}
#if defined(CONFIG_PROD_FVP4000T)
	if(cso_probeinfo->ucTotalSubTrack > OxMP_SUBTTL_ES_NUM_MAX)
	{
		HxLOG_Error ("MAX OxMP_AUDIO_ES_NUM_MAX is %d, But audio track is %d from CSO_Probe. So the number of emitting OxMP_AUDIO_ES_NUM_MAX is %d\n", OxMP_AUDIO_ES_NUM_MAX, cso_probeinfo->ucTotalAudioTrack, OxMP_AUDIO_ES_NUM_MAX);
		nTotalSubttlTrack = OxMP_SUBTTL_ES_NUM_MAX;
	}
	else
	{
		nTotalSubttlTrack = cso_probeinfo->ucTotalSubTrack;
	}

	/* SubtitleTrack 정보 획득...*/
	uValidSTrackCnt = 0;
	HxSTD_MemSet (pstSubtitleCompTable, 0, sizeof(OxDownload_SubtitleCompTable_t));
	for ( tracks = 0; tracks < nTotalSubttlTrack; tracks++ )
	{
		pstSubtitleCompTable->nCurrIndex		=	tracks;
		pstSubtitleCompTable->nArraySize		=	tracks+1;
		pstSubtitleCompTable->astCompArray[tracks].nPid		=	cso_probeinfo->stSubTrack[tracks].stream_id;
		pstSubtitleCompTable->astCompArray[tracks].eCompType 	=	eOxMP_COMPONENT_SUBTITLE;
		pstSubtitleCompTable->astCompArray[tracks].eSubtitleType = om_download_mediapb_cso_doConvertSubtitleType(cso_probeinfo->stSubTrack[tracks].eSubType);
		if ( pstSubtitleCompTable->astCompArray[tracks].eSubtitleType == eOxDNLD_SUBTITLETYPE_Dvb)
		{
			bSubtitlePlayable = TRUE;
			pstSubtitleCompTable->astCompArray[tracks].utArg.stDvbInfo.nTypeValue = pstSubtitleCompTable->astCompArray[tracks].eSubtitleType;
			pstSubtitleCompTable->astCompArray[tracks].utArg.stDvbInfo.nCompositionPageId = cso_probeinfo->stSubTrack[tracks].stDvb.usCompositionPageId;
			pstSubtitleCompTable->astCompArray[tracks].utArg.stDvbInfo.nAncillaryPageId = cso_probeinfo->stSubTrack[tracks].stDvb.usAncillaryPageId;
			HxSTD_memcpy(pstSubtitleCompTable->astCompArray[tracks].utArg.stDvbInfo.szLangCode,cso_probeinfo->stSubTrack[tracks].uacSubLang,3);
			uValidSTrackCnt++;
		}
		else if ( pstSubtitleCompTable->astCompArray[tracks].eSubtitleType == eOxDNLD_SUBTITLETYPE_Ebu)
		{
			bSubtitlePlayable = TRUE;
			pstSubtitleCompTable->astCompArray[tracks].utArg.stEbuInfo.nTypeValue = pstSubtitleCompTable->astCompArray[tracks].eSubtitleType;
			pstSubtitleCompTable->astCompArray[tracks].utArg.stEbuInfo.nMagazineNum = cso_probeinfo->stSubTrack[tracks].stEbu.ucMagazineNum;
			pstSubtitleCompTable->astCompArray[tracks].utArg.stEbuInfo.nPageNum = cso_probeinfo->stSubTrack[tracks].stEbu.ucPageNum;
			HxSTD_memcpy(pstSubtitleCompTable->astCompArray[tracks].utArg.stEbuInfo.szLangCode,cso_probeinfo->stSubTrack[tracks].uacSubLang,3);
			uValidSTrackCnt++;
		}
	}
	pstSubtitleCompTable->nMaxSize			=	sizeof(OxDownload_SubtitleCompTable_t);

	for( tracks = 0; tracks < pstSubtitleCompTable->nArraySize; tracks++ )
	{
		HxLOG_Debug ("[%s](L:%d) Found track %d Subtitle Type : %d(%s) \n", __FUNCTION__, __LINE__, tracks, pstSubtitleCompTable->astCompArray[tracks].eSubtitleType,pstSubtitleCompTable->astCompArray[tracks].utArg.stDvbInfo.szLangCode);
	}
#endif
#if 1//def AV_TRACK_CHANGE
	/* print AV track info */
	for( tracks = 0; tracks < pstVideoCompTable->nArraySize; tracks++ )
	{
		HxLOG_Error ("pstVideoCompTable->astCompArray[%d].nPid								   [ %d ] \n", tracks, pstVideoCompTable->astCompArray[tracks].nPid);
		HxLOG_Error ("pstVideoCompTable->astCompArray[%d].eCodec							   [ %d ] \n\n", tracks, pstVideoCompTable->astCompArray[tracks].eCodec);
	}
	for( tracks = 0; tracks < pstAudioCompTable->nArraySize; tracks++ )
	{
		HxLOG_Error ("pstAudioCompTable->astCompArray[%d].nPid								   [ %d ] \n", tracks, pstAudioCompTable->astCompArray[tracks].nPid);
		HxLOG_Error ("pstAudioCompTable->astCompArray[%d].eCodec							   [ %d ] \n\n", tracks, pstAudioCompTable->astCompArray[tracks].eCodec);
	}
#endif
#if 0 //def AV_TRACK_CHANGE
	/* 첫 번째 Video, Audio track 이 재생 */
	//pstMediaPlay->nActivatedAudioTrackNum = 0;
	//pstMediaPlay->nActivatedVideoTrackNum = 0;
#endif
#if 1 //((CALYPSO_VERSION_MAJOR >= 1) && (CALYPSO_VERSION_MINOR >= 1) && (CALYPSO_VERSION_MICRO >= 8) && (CALYPSO_VERSION_BCODE == 2))
	pstAudioCompTable->nCurrIndex = cso_probeinfo->nCurrentAudioTrack;
	pstVideoCompTable->nCurrIndex = cso_probeinfo->nCurrentVideoTrack;

	HxLOG_Error ("CSO's CurrentAudioTrack [ %d ] \n", cso_probeinfo->nCurrentAudioTrack);
	HxLOG_Error ("CSO's CurrentVideoTrack [ %d ] \n", cso_probeinfo->nCurrentVideoTrack);
#endif

#if 1
	/*this is Audio Only Contents...*/
	if ( (cso_probeinfo->ucTotalAudioTrack > 0) && (cso_probeinfo->ucTotalVideoTrack == 0 ) )
	{
		/* 오디오 only 일 때 Audio Probe 결과로만 판단하여 재생하기 위함. */
		bVideoPlayable = TRUE;
	}

	/*this is Video Only Contents...*/
	if ( (cso_probeinfo->ucTotalAudioTrack == 0 ) && (cso_probeinfo->ucTotalVideoTrack > 0 ) )
	{
		/* Video only 일 때 Video Probe 결과로만 판단하여 재생하기 위함. */
		bAudioPlayable = TRUE;
	}
#endif

	if( bSubtitlePlayable )
	{
		HERROR			 hErr;
		HxLANG_Id_e		eSubTitleLangId			= eLangID_MAX;
		HxLANG_Id_e		eTrackLangId1			= eLangID_MAX;

		pstSubtitleCompTable->nCurrIndex = 0;//SUBTITLE_OFF; // 우선 Subtitle OFF로 해놓고
		hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_SUBTITLE0, (HUINT32 *)&eSubTitleLangId, 0);
		if ((hErr == ERR_OK) && (eSubTitleLangId != eLangID_MAX))
		{
			for( tracks = 0; tracks < pstSubtitleCompTable->nArraySize; tracks++ )
			{
				// Check the language number:
				hErr = MWC_UTIL_GetLangIDBy639Code (pstSubtitleCompTable->astCompArray[tracks].utArg.stDvbInfo.szLangCode, &eTrackLangId1);
				if ( hErr != ERR_OK )
				{
					HxLOG_Error ("[%s](L:%d) Fail To get track subtitle language.... \n", __FUNCTION__, __LINE__ );
					eTrackLangId1 = eLangID_MAX;
				}

				if((eSubTitleLangId==eTrackLangId1))
				{
					HxLOG_Debug ("[%s](L:%d) Current Subtitle Index: %d \n", __FUNCTION__, __LINE__, tracks);
					pstSubtitleCompTable->nCurrIndex=tracks;
					break;
				}
			}
		}
		else
		{
			HxLOG_Error ("[%s](L:%d) Fail To get system subtitle language.... \n", __FUNCTION__, __LINE__ );
		}

		/* APP으로 Event를 보내 DVB-Subtitle이 있음을 알림. . */
		om_download_CbOnComponentSubtitleChanged (ulDownloadId, ulSessionId, pstSubtitleCompTable );
	}

	if( bAudioPlayable )
	{
		HERROR			hErr;
		HxLANG_Id_e 	eAudioLangId 		= eLangID_MAX;
		HxLANG_Id_e 	eTrackLangId1		= eLangID_MAX;

#if 1 //((CALYPSO_VERSION_MAJOR >= 1) && (CALYPSO_VERSION_MINOR >= 1) && (CALYPSO_VERSION_MICRO >= 8) && (CALYPSO_VERSION_BCODE == 2))
		pstAudioCompTable->nCurrIndex = cso_probeinfo->nCurrentAudioTrack; // cso 에서 선택된 Audio track 재생
#else
		pstAudioCompTable->nCurrIndex = 0; // 우선 0번째  Audio track 이 재생
#endif
		hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_AUDIO0, (HUINT32 *)&eAudioLangId, 0);
		if ((hErr == ERR_OK) && (eAudioLangId != eLangID_MAX))
		{
			for( tracks = 0; tracks < pstAudioCompTable->nArraySize; tracks++ )
			{
				// Check the language number:
				hErr = MWC_UTIL_GetLangIDBy639Code (pstAudioCompTable->astCompArray[tracks].astLangInfo[0].szLangCode, &eTrackLangId1);
				if ( hErr != ERR_OK )
				{
					HxLOG_Error ("[%s](L:%d) Fail To get track audio language.... \n", __FUNCTION__, __LINE__ );
					eTrackLangId1 = eLangID_MAX;
				}

				if((eAudioLangId==eTrackLangId1))
				{
					HxLOG_Debug ("[%s](L:%d) Current Audio Index: %d \n", __FUNCTION__, __LINE__, tracks);
#if 1 //((CALYPSO_VERSION_MAJOR >= 1) && (CALYPSO_VERSION_MINOR >= 1) && (CALYPSO_VERSION_MICRO >= 8) && (CALYPSO_VERSION_BCODE == 2))
					pstAudioCompTable->nCurrIndex = cso_probeinfo->nCurrentAudioTrack; // cso 에서 선택된 Audio track 재생
#else
					pstAudioCompTable->nCurrIndex = 0; // 우선 0번째  Audio track 이 재생
#endif
					/* 여기서는 코덱 아이디, 스트림아이디 도 변경해 준다. cso_start() 에서 바로 이용 가능하도록 */
					//pstMediaPlay->ulActivatedAudioCodecId  = pstAudioCompTable->astCompArray[tracks].eCodec;
					//pstMediaPlay->ulActivatedAudioStreamId = pstAudioCompTable->astCompArray[tracks].nPid;
					break;
				}
			}
		}
		else
		{
			HxLOG_Error ("[%s](L:%d) Fail To get audio subtitle language.... %d %x \n", __FUNCTION__, __LINE__, hErr, eAudioLangId );
		}
	}

	if ( bVideoPlayable && bAudioPlayable)
	{
		/*
		 * Media는 ActionTaken Message 대신 ProbeFinished 사용 : 왜 여기에 필요한지 문의 필요...
		 */
		//pstContext->bActionTaken	=	TRUE;

		/*
		 * ProbeFinished 시 Audio,Video Component Update 해야함.
		 */
		HxLOG_Debug ("[%s](L:%d) Play Video Codec = %08X, Stream ID = %d, Audio Codec = %08X, Stream ID = %d \n", __FUNCTION__, __LINE__, cso_probeinfo->video_codec, cso_probeinfo->video_stream_id, cso_probeinfo->audio_codec, cso_probeinfo->audio_stream_id );
		om_download_CbOnComponentVideoChanged (ulDownloadId, ulSessionId, pstVideoCompTable);
		om_download_CbOnComponentAudioChanged (ulDownloadId, ulSessionId, pstAudioCompTable);
	}
	else
	{
		HxLOG_Error ("[%s](L:%d) there is no Playable Audio, Video Codecs.... \n", __FUNCTION__, __LINE__ );
		//OxMGR_Free(cso_probeinfo);
		return ERR_FAIL;
	}
	/*pstMediaPlay->ulBitrate = cso_probeinfo->max_bitrate;
	  pstMediaPlay->ullDuration = cso_probeinfo->duration;

	  if ( cso_probeinfo->ullStartTime != 0 )
	  {
	  pstMediaPlay->ullFirstPts = cso_probeinfo->ullStartTime;
	  }
	  else
	  {
	  pstMediaPlay->ullFirstPts = 0;
	  }

	  pstMediaPlay->bSupport_seek = cso_probeinfo->support_seek;
	  if ( !pstMediaPlay->bSupport_seek )
	  {
	  HxLOG_Error("[%s](L:%d) This Content cann't be seeked.\n", __FUNCTION__, __LINE__);
	  }
	  pstMediaPlay->bSupport_trick = cso_probeinfo->support_trick;
	  if ( !pstMediaPlay->bSupport_trick )
	  {
	  HxLOG_Error("[%s](L:%d) This Content cann't be tricked.\n", __FUNCTION__, __LINE__);
	  }*/


	//OxMGR_Free(cso_probeinfo);
	//pstMediaPlay->stCsoStatus.eStatus = eCSO_PROBED;


	return ERR_OK;
}

STATIC HERROR om_download_ipmedia_DnpSetup(CsoSessionDNLD_t *pSession, OmDownload_IpMedia_t *pIpMedia)
{
	OmDownload_IpMediaInfo_t *pIpMediaInfo = NULL;
	CSO_Settings_t *pSettings = NULL;
	SvcCso_Config_t svcDnpConfig;

	CSO_CTRL_PARAM_t tParam;
	CSO_ERR err = CSO_ERR_OK;
	HERROR hErr = ERR_OK;

	HxSTD_memset(&tParam,0x0,sizeof(CSO_CTRL_PARAM_t));
	HxSTD_memset(&svcDnpConfig,0x0,sizeof(SvcCso_Config_t));

	hErr = SVC_CSO_GetDnPConfig(NULL, (SvcCso_Config_t*)&svcDnpConfig);
	if ( hErr != ERR_OK )
	{
		HxLOG_Error ("[%s](L:%d) SVC_CSO_GetDnPConfig() Failed. But keep going\n", __FUNCTION__, __LINE__);
	}
	if(svcDnpConfig.bProgressiveDownload == FALSE)
	{
		HxLOG_Error ("[%s](L:%d) not support download. But keep going\n", __FUNCTION__, __LINE__);
	}

	pIpMediaInfo = &pIpMedia->stMediaInfo;
	if(NULL != pIpMediaInfo->pCsoSettings)
	{
		pSettings = (CSO_Settings_t *)pIpMediaInfo->pCsoSettings;
	}
	if(pSettings == NULL)
	{
		pSettings = HLIB_STD_MemAlloc(sizeof(CSO_Settings_t));
		HxSTD_MemSet(pSettings, 0x00, sizeof(CSO_Settings_t));
		err = MEDIAPB_IPC_CSO_GetSettings(pSession->unSessionId, pSettings);
		if(err != CSO_ERR_OK)
		{
			HxLOG_Error("[%s](L:%d) Calypso> GetSettings has failed! But keep going\n", __FUNCTION__, __LINE__);
		}
	}
	else
	{
		HxLOG_Error("[%s](L:%d) Using cso'setting value from cache\n", __FUNCTION__, __LINE__);
	}

	if(pSettings->pSaveFilePath != NULL)
	{
		OxMGR_Free(pSettings->pSaveFilePath);
		pSettings->pSaveFilePath = NULL;
	}

	if (svcDnpConfig.bProgressiveDownload)
	{
		pSettings->pSaveFilePath = OxMGR_Malloc(DxDNLD_MEDIAPLAY_URL);
		if(pSettings->pSaveFilePath != NULL)
		{
			HxSTD_memset(pSettings->pSaveFilePath,0x0,DxDNLD_MEDIAPLAY_URL);
			HxLOG_Error("[%s:%d] HUK!!!!! Length of pSaveFilePath = %d\n", __FUNCTION__, __LINE__, VK_strlen(pSettings->pSaveFilePath));
			snprintf((char*)pSettings->pSaveFilePath, DxDNLD_MEDIAPLAY_URL, "%s%u",svcDnpConfig.szPDLPath,pSession->unDownloadId);
			HxLOG_Error("Setting file path [ %s ]\n",pSettings->pSaveFilePath);
		}
		else
		{
			HxLOG_Error("[%s](L:%d) Memory Allocation failed! But keep going\n", __FUNCTION__, __LINE__);
		}
	}

	err = MEDIAPB_IPC_CSO_SetSettings(pSession->unSessionId, pSettings);
	if(err != CSO_ERR_OK)
	{
		HxLOG_Error("[%s](L:%d) Calypso> SetSettings has failed! But keep going\n", __FUNCTION__, __LINE__);
	}
	err = MEDIAPB_IPC_CSO_Control(pSession->unSessionId, eCSO_CTRL_STARTBUFFERING, &tParam);
	if(err != CSO_ERR_OK)
	{
		HxLOG_Error("[%s](L:%d) Calypso> Start buffering has failed!\n", __FUNCTION__, __LINE__);
	}
	else
	{
		pIpMedia->bDownloadFileCreated = TRUE;
	}
	pIpMedia->bDownloadAfterVerify = FALSE;
		// store cache
	HxLOG_Error("[%s](L:%d) store cso's setting value to cache\n", __FUNCTION__, __LINE__);
	pIpMediaInfo->pCsoSettings = (HUINT8 *)pSettings;
	return ERR_OK;
}


/**
* @b Function @b Description <br>
* this function makes Probing --> probed or probe failed
*
* @param[in] omMediaPlay_Context_t   : media pb context
*
* @return  #  returns ERR_OK or ERR_FAIL
*
* @attention don't call any functions with om_media_play_mediapb_cso_* named, or State machine may corrupted!
*/
STATIC HERROR om_download_ipmedia_GetSeekableInfo(HUINT32 ulSessionId, OxDownload_SeekableInfo_t *pstSeekableInfo)
{
	CSO_BufferInfo_t stCSOBufferInfo;
	CSO_ERR err = CSO_ERR_FAIL;
	HUINT32  ulIdx			 =	 0;

	HxSTD_MemSet(&stCSOBufferInfo, 0, sizeof(CSO_BufferInfo_t));
	HxSTD_MemSet(pstSeekableInfo, 0, sizeof(OxDownload_SeekableInfo_t));
	err = MEDIAPB_IPC_CSO_BufferInfo(ulSessionId, &stCSOBufferInfo);
	if ( err == CSO_ERR_INVALID_SESSION)
	{
		HxLOG_Error("[%s](L:%d) Error - MEDIAPB_IPC_CSO_BufferInfo (Invalid Session)\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}
	else if ( err != CSO_ERR_OK)
	{
		HxLOG_Error("[%s](L:%d) Error - MEDIAPB_IPC_CSO_BufferInfo\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	HxLOG_Error("[%s](L:%d) Error - stCSOBufferInfo.tSeekableRange.uiChunks : %lu\n", __FUNCTION__, __LINE__, stCSOBufferInfo.tSeekableRange.uiChunks);
	if( stCSOBufferInfo.tSeekableRange.uiChunks > 0)
	{
		if(stCSOBufferInfo.tSeekableRange.uiChunks > 20)
			stCSOBufferInfo.tSeekableRange.uiChunks = 20;

		pstSeekableInfo->ulSeekableChunks = stCSOBufferInfo.tSeekableRange.uiChunks;
		HxLOG_Error("[%s](L:%d) Seekable chunks : %d\n", __FUNCTION__, __LINE__, stCSOBufferInfo.tSeekableRange.uiChunks);
		for(ulIdx=0; ulIdx<stCSOBufferInfo.tSeekableRange.uiChunks; ulIdx++)
		{
			pstSeekableInfo->ullSeekableStart[ulIdx] = stCSOBufferInfo.tSeekableRange.pullSeekableStart[ulIdx];
			pstSeekableInfo->ullSeekableEnd[ulIdx] = stCSOBufferInfo.tSeekableRange.pullSeekableEnd[ulIdx];
			HxLOG_Error("[%s](L:%d) Idx:%u Seekabe Start:%llu, Seekable End:%llu\n", __FUNCTION__, __LINE__, ulIdx, pstSeekableInfo->ullSeekableStart[ulIdx], pstSeekableInfo->ullSeekableEnd[ulIdx]);
		}
	}
#if ((CALYPSO_VERSION_MAJOR >= 1) && (CALYPSO_VERSION_MINOR >= 1) && (CALYPSO_VERSION_MICRO >= 18))
	MEDIAPB_IPC_CSO_IPCMSG_MemFree_BufferInfo(&stCSOBufferInfo);
#else
	om_download_ipmedia_cso_MemFree_BufferInfo(&stCSOBufferInfo);
#endif
	return ERR_OK;
}

#define _____MSG_PROCESS_FUNCTIONS_____

#define ___MEMBER_FUNCTIONS___
#define _____PROCEDURE_____
BUS_Result_t proc_om_download_ipmedia (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t			 eResult = ERR_BUS_NO_ERROR;

	switch (message)
	{
	default:
		eResult = MESSAGE_PASS;
		break;
	}

	return (MESSAGE_BREAK == eResult) ? MESSAGE_BREAK : MESSAGE_PASS;
}

HERROR om_download_ipmedia_Init (void)
{
	HxSEMT_Create(&s_uiIpMediaDownloadSemaId, "IpMediaDownloadCtxSema", HxSEMT_FIFO);
	HxSEMT_Create(&s_uiRemoveScheduleListId, "IpDnlRemoveSema", HxSEMT_FIFO);
	return ERR_OK;
}

HERROR om_download_ipmedia_InitRpcFunctions (HINT32 nRpcHandle)
{
	HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_DOWNLOAD_OnDownloadInfoChanged ,"iiib", "Download InfoChanged.	\n\t   - (HUINT32 ulDownloadId, HUINT32 ulSessionId, OxDownload_BufferInfo_t *pstBufferInfo)\n");
	s_nRpcHandle = nRpcHandle;
	return ERR_OK;
}

STATIC void om_download_ipmedia_EvtCallback(HUINT32 ulSessionId, HINT32 ev, HINT64 param)
{
	CSO_EVENT_e cso_event = (CSO_EVENT_e)ev;
	OmDownload_Instance_t	*pstInst = NULL;
	omDownload_Context_t	*pstContext = om_download_GetContext();
	CsoSessionDNLD_t *pSession = NULL;

	if(pstContext == NULL)
	{
		return;
	}
	pSession = om_download_ipmedia_GetSession(ulSessionId);
	if( pSession == NULL )
	{
		return;
	}

	switch ( cso_event )
	{
	case CSO_EVENT_BUFFERING_START:
		HLIB_RPC_Notify(pstContext->nRpcHandle, RPC_OBAMA_DOWNLOAD_OnStarted, "i");
		pSession->bDownloadCompleted = FALSE;
		{
			OxDownload_BufferInfo_t stBufferInfo = { 0 };
			if (om_download_ipmedia_UpdateBufferInfo(pSession, &stBufferInfo) == ERR_OK)
			{
				om_download_ipmedia_CbOnDownloadInfoChanged(pSession->unDownloadId , pSession->unSessionId, &stBufferInfo);

			}
		}
		break;
	case CSO_EVENT_BUFFERING_FAILURE:
		HLIB_RPC_Notify(pstContext->nRpcHandle, RPC_OBAMA_DOWNLOAD_OnFailed,"i", pSession->unDownloadId);
		pSession->bDownloadCompleted = FALSE;
		break;
	case CSO_EVENT_BUFFERING_COMPLETE:
		{
			OxDownload_BufferInfo_t stBufferInfo = { 0 };
			if (om_download_ipmedia_UpdateBufferInfo(pSession, &stBufferInfo) == ERR_OK)
			{
				om_download_ipmedia_CbOnDownloadInfoChanged(pSession->unDownloadId , pSession->unSessionId, &stBufferInfo);

			}
		}
		HLIB_RPC_Notify(pstContext->nRpcHandle, RPC_OBAMA_DOWNLOAD_OnCompleted,"i",pSession->unDownloadId);
		// download가 완료 되면 buffering status event를 더 이상 보내지 않음.
		pSession->bDownloadCompleted = TRUE;
		break;
	case CSO_EVENT_PROBE_COMPLETE:
		{
			CSO_ProbeInfo_t 	*cso_probeinfo = NULL;
			OxDownload_SeekableInfo_t stSeekableInfo;
			HERROR 		hErr = ERR_OK;

			pSession->bProbeCompleted = TRUE;

			pSession->pProbeInfo = OxMGR_Malloc(sizeof(CSO_ProbeInfo_t));
			if ( !pSession->pProbeInfo )
			{
				HxLOG_Error("[%s](L:%d) Error - Alloc failed!\n", __FUNCTION__, __LINE__);
				return ;
			}
			HxSTD_MemSet(pSession->pProbeInfo, 0, sizeof(CSO_ProbeInfo_t));
			cso_probeinfo = pSession->pProbeInfo;
			HxSTD_MemSet(&stSeekableInfo, 0, sizeof(OxDownload_SeekableInfo_t));

			hErr = MEDIAPB_IPC_CSO_ProbeInfo(pSession->unSessionId, cso_probeinfo);
			if (hErr != ERR_OK)
			{
				HxLOG_Error("[%s](L:%d) Error - Don't get probe infomation..........\n", __FUNCTION__, __LINE__);
			}
			hErr = om_download_ipmedia_GetSeekableInfo(pSession->unSessionId, &stSeekableInfo);
			if (hErr != ERR_OK)
			{
				HxLOG_Error("[%s](L:%d) Error - Don't get seekable infomation..........\n", __FUNCTION__, __LINE__);
			}

			om_download_ipmedia_TransferMetadata(pSession, cso_probeinfo);

			HxLOG_Error("%s(%d) [CRB] Receive Probe Complete from CSO. so send ProbeComplete event for download, Infifnity : %d\n",
					__FUNCTION__, __LINE__, (cso_probeinfo->duration == 0x4189374BC6A7)? TRUE: FALSE);

#if defined(CONFIG_TIMESTAMP_64)
			HLIB_RPC_Notify(pstContext->nRpcHandle, RPC_OBAMA_DOWNLOAD_OnProbeComplete,"iilliib",
					pSession->unDownloadId,ulSessionId,(HUINT64)cso_probeinfo->duration,(HUINT64)cso_probeinfo->ullInternalSeekedTime,
					cso_probeinfo->max_bitrate, (cso_probeinfo->duration == 0x4189374BC6A7)? TRUE: FALSE, &stSeekableInfo, sizeof(OxDownload_SeekableInfo_t));
#else
			HLIB_RPC_Notify(pstContext->nRpcHandle, RPC_OBAMA_DOWNLOAD_OnProbeComplete,"iiiiiib",
					pSession->unDownloadId,ulSessionId,(HUINT32)cso_probeinfo->duration,(HUINT32)cso_probeinfo->ullInternalSeekedTime,
					cso_probeinfo->max_bitrate, (cso_probeinfo->duration == 0x4189374BC6A7)? TRUE: FALSE, &stSeekableInfo, sizeof(OxDownload_SeekableInfo_t));
#endif
			pstInst = OM_DOWNLOAD_GetInstanceWithCsoId(ulSessionId);
			if(pstInst && pstInst->unExtInfo.stIpMedia.bDownloadAfterVerify == TRUE)
			{
				om_download_ipmedia_DnpSetup(pSession, &pstInst->unExtInfo.stIpMedia);
			}
		}
		break;
	case CSO_EVENT_MEDIA_UPDATE_BASE_POSITION:
		{
			OmDownload_IpMediaInfo_t *pstMediaInfo = OM_DOWNLOAD_GetMediaInfoByCsoId(ulSessionId);
	        pstMediaInfo->ullBasePosition = param;
		}
		break;
	case CSO_EVENT_PROBE_FAILURE:
		pstInst = OM_DOWNLOAD_GetInstanceWithCsoId(ulSessionId);
		if (pstInst)
				{
			HLIB_RPC_Notify(pstContext->nRpcHandle, RPC_OBAMA_DOWNLOAD_OnProbeFail,"ii", pSession->unDownloadId, pstInst->stError.eReason);
		}
		break;
	default:
		break;
	}
	return;
}

STATIC void om_download_ipmedia_ErrorEvtCallback(HUINT32 ulSessionId, HINT32 ev, HINT64 param)
{
	CSO_EVENT_e cso_event = (CSO_EVENT_e)ev;
	CSO_ERROR_REASON_e		eReason = (CSO_ERROR_REASON_e)param;
	OmDownload_Instance_t	*pstInst = OM_DOWNLOAD_GetInstanceWithCsoId(ulSessionId);
	omDownload_Context_t	*pstContext = om_download_GetContext();
	if( pstInst == NULL || pstContext == NULL)
	{
		return;
	}

	switch ( cso_event )
	{
		case CSO_EVENT_ADVANCED_ERROR_OTHER:
		case CSO_EVENT_ADVANCED_ERROR_SERVER:
		case CSO_EVENT_ADVANCED_ERROR_RECOVERED_SERVER:
		case CSO_EVENT_ADVANCED_ERROR_NETWORK:
		case CSO_EVENT_ADVANCED_ERROR_RECOVERED_NETWORK:
		case CSO_EVENT_ADVANCED_ERROR_RECOVERED_DATA:
		case CSO_EVENT_ADVANCED_ERROR_SYSTEM:
		case CSO_EVENT_ADVANCED_ERROR_RECOVERED_SYSTEM:
		case CSO_EVENT_ADVANCED_ERROR_IO:
		case CSO_EVENT_ADVANCED_ERROR_RECOVERED_IO:
		case CSO_EVENT_ADVANCED_ERROR_DRM_MARLIN:
		case CSO_EVENT_ADVANCED_ERROR_DRM_PLAYREADY:
		case CSO_EVENT_ADVANCED_ERROR_DRM_VIEWRIGHT:
		case CSO_EVENT_ADVANCED_ERROR_RECOVERED_DRM:
			break;
		case CSO_EVENT_ADVANCED_ERROR_DATA:
			HxLOG_Error("ulSessionId(0x%08x). Error Data(%d)\n", ulSessionId, eReason);
			if ( eReason == CSO_ERROR_R_DATA_MALFORMED_MHEG_X_KEY )
			{
				pstInst->stError.eReason = eOxMP_ERROR_REASON_X_KEY;
			}
			break;
		default:
			break;
	}
	return;
}



HERROR om_download_ipmedia_CbProbe (OmDownload_Instance_t *pstInst, HUINT32 ulDownloadId, DxDownload_Start_t *punStart)
{
#ifdef CONFIG_SUPPORT_IMMA
	HUINT32		ulCsoSessionId = 33333;
	CSO_ERR 	err;
	CSO_Settings_t settings;
	HERROR		hErr = ERR_FAIL;
	HBOOL 		bAudioDescription = FALSE;
#ifdef PREFERRED_LANGUAGE_SELECTION
	HxLANG_Id_e eAudioLangId = eLangID_MAX;
#endif

	if( pstInst == NULL || punStart == NULL)
	{
		HxLOG_Error("Download Handle is NULL\n");
		return ERR_FAIL;
	}

	HxSTD_memset(&settings,0x0,sizeof(CSO_Settings_t));

	om_download_ipmedia_EnterSection();
	if(!s_bCallbackRegistered)
	{
		MEDIAPB_IPC_RegisterCallback(eCSO_IPCMSG_REGISTERCALLBACK_MEDIAEVENT, 0, om_download_ipmedia_EvtCallback);
		MEDIAPB_IPC_RegisterCallback(eCSO_IPCMSG_REGISTERCALLBACK_ERROREVENT, 0, om_download_ipmedia_ErrorEvtCallback);
		s_bCallbackRegistered = TRUE;

		/* nfs 의 경우, download init 시 /mnt/hd3 가 mount 되지 않은 상태. 그래서 cache file 삭제가 안되므로 이곳에서 삭제. */
		om_download_ipmedia_DeleteFile(TRUE, 0);
	}
	om_download_ipmedia_ExitSection();

	if(pstInst->unExtInfo.stIpMedia.bAllocated == FALSE)
	{
		err = MEDIAPB_IPC_CSO_Open(punStart->stIpMedia.szUrl, eCSO_MODE_DNP, &ulCsoSessionId);
		if(err != CSO_ERR_OK)
		{
			HxLOG_Error("Calypso> Session open has failed!\n");
			return ERR_FAIL;
		}
	}
	else
	{
		ulCsoSessionId = pstInst->unExtInfo.stIpMedia.ulCsoSessionId;
	}

#if defined(CONFIG_PROD_FVP4000T)
	err = MEDIAPB_IPC_CSO_GetSettings(ulCsoSessionId, &settings);
	if(err != CSO_ERR_OK)
	{
		HxLOG_Error("[%s](L:%d) Calypso> GetSettings has failed!\n", __FUNCTION__, __LINE__);
		goto error_cleanup;
	}

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_AUDIO_DESCRIPTION, (HUINT32 *)&bAudioDescription, 0);
	if (ERR_OK != hErr)
	{
		bAudioDescription = FALSE;
	}
	if(bAudioDescription)
	{
		settings.bSelectAudioDescription = TRUE;
	}
	else
	{
		settings.bSelectAudioDescription = FALSE;
	}

#ifdef PREFERRED_LANGUAGE_SELECTION
	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_AUDIO0, (HUINT32 *)&eAudioLangId, 0);
	if (ERR_OK != hErr)
	{
		settings.ePreperredAudioLanguage = CSO_LANG_NONE;
	}
	else
	{
		settings.ePreperredAudioLanguage = (CSO_LangCode_t)eAudioLangId;	/* CSO_LANG is compatible with HxLANG. So directly assign here */
	}
#endif

	settings.bStartLowestStream = TRUE;
	err = MEDIAPB_IPC_CSO_SetSettings(ulCsoSessionId, &settings);
	if(err != CSO_ERR_OK)
	{
		HxLOG_Error("[%s](L:%d) Calypso> SetSettings has failed!\n", __FUNCTION__, __LINE__);
		goto error_cleanup;
	}
	(void)MEDIAPB_IPC_CSO_IPCMSG_MemFree_Settings(settings);
#endif

	err = MEDIAPB_IPC_CSO_Probe(ulCsoSessionId);
	if(err != CSO_ERR_OK)
	{
		HxLOG_Error("Calypso> Content Verification has failed!\n");
		goto error_cleanup;
	}
	pstInst->unExtInfo.stIpMedia.ulCsoSessionId = ulCsoSessionId;
	pstInst->unExtInfo.stIpMedia.bAllocated = TRUE;
	return ERR_OK;

error_cleanup:
	MEDIAPB_IPC_CSO_Close(ulCsoSessionId);
	return ERR_FAIL;
#else
	HxLOG_Error("not supported\n");
	return ERR_FAIL;
#endif
}

HERROR om_download_ipmedia_CbStart (OmDownload_Instance_t *pstInst, HUINT32 ulDownloadId, DxDownload_Start_t *punStart)
{
#ifdef CONFIG_SUPPORT_IMMA
	HUINT32		ulCsoSessionId = 33333;
	HERROR 		hErr = ERR_OK;
	CSO_ERR 	err;
	SvcCso_Config_t svcDnpConfig;
	CSO_CTRL_PARAM_t tParam;
	OmMedia_MediapbInfo_t stMediaInfo;
	CSO_Settings_t *pSettings = NULL;

	HxLOG_Error("%s(%d) [CRB] Start Download. \n", __FUNCTION__, __LINE__);

	if( pstInst == NULL || punStart == NULL)
	{
		HxLOG_Error("Download Handle is NULL\n");
		return ERR_FAIL;
	}
	HxSTD_memset(&svcDnpConfig,0x0,sizeof(SvcCso_Config_t));
	HxSTD_memset(&tParam,0x0,sizeof(CSO_CTRL_PARAM_t));
	HxSTD_memset(&stMediaInfo,0x0,sizeof(OmMedia_MediapbInfo_t));

	hErr = SVC_CSO_GetDnPConfig(NULL, (SvcCso_Config_t*)&svcDnpConfig);
	if ( hErr != ERR_OK )
	{
		HxLOG_Error ("SVC_CSO_GetDnPConfig() Failed.\n");
		return ERR_FAIL;
	}
	if(svcDnpConfig.bProgressiveDownload == FALSE)
	{
		HxLOG_Error ("[%s](L:%d) not support download. But keep going\n", __FUNCTION__, __LINE__);
//		return ERR_FAIL;
	}
	om_download_ipmedia_EnterSection();
	if(!s_bCallbackRegistered)
	{
		MEDIAPB_IPC_RegisterCallback(eCSO_IPCMSG_REGISTERCALLBACK_MEDIAEVENT, 0, om_download_ipmedia_EvtCallback);
		MEDIAPB_IPC_RegisterCallback(eCSO_IPCMSG_REGISTERCALLBACK_ERROREVENT, 0, om_download_ipmedia_ErrorEvtCallback);
		s_bCallbackRegistered = TRUE;

		/* nfs 의 경우, download init 시 /mnt/hd3 가 mount 되지 않은 상태. 그래서 cache file 삭제가 안되므로 이곳에서 삭제. */
		om_download_ipmedia_DeleteFile(TRUE, 0);
	}
	om_download_ipmedia_ExitSection();
	if(pstInst->unExtInfo.stIpMedia.bAllocated == FALSE)
	{
#if 0
		err = MEDIAPB_IPC_CSO_Open(punStart->stIpMedia.szUrl, eCSO_MODE_DNP, &ulCsoSessionId);
		if(err != CSO_ERR_OK)
		{
			HxLOG_Error("Calypso> Session open has failed!\n");
			return ERR_FAIL;
		}
#else
		hErr = om_media_play_mediapb_cso_prepareSession(punStart->stIpMedia.szUrl, NULL, &stMediaInfo);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("Calypso> Session open has failed!\n");
			return ERR_FAIL;
		}
		ulCsoSessionId = stMediaInfo.ulCsoSessionId;
		pstInst->unExtInfo.stIpMedia.stMediaInfo.bDlna = stMediaInfo.bDlna;
		pstInst->unExtInfo.stIpMedia.stMediaInfo.bAirplaying = stMediaInfo.bAirplaying;
		pstInst->unExtInfo.stIpMedia.stMediaInfo.fAirPlayingStartPercentage = stMediaInfo.fAirPlayingStartPercentage;
		pSettings = (CSO_Settings_t *)stMediaInfo.pCsoSettings;
#endif
	}
	else
	{
		ulCsoSessionId = pstInst->unExtInfo.stIpMedia.ulCsoSessionId;
	}

	/* create session -> setting -> probing */
	pstInst->unExtInfo.stIpMedia.pSession = om_download_ipmedia_AddSession(ulCsoSessionId, ulDownloadId);

	if(punStart->stIpMedia.bContentVerification == TRUE)
	{
		pstInst->unExtInfo.stIpMedia.bDownloadAfterVerify = TRUE;
		/* mheg ics stream이면 아래와 같은 설정을 해준다. */
		if (punStart->stIpMedia.bMhegIcsStream == TRUE)
		{
			if(pSettings == NULL)
			{
				pSettings = HLIB_STD_MemAlloc(sizeof(CSO_Settings_t));
				HxSTD_MemSet(pSettings, 0x00, sizeof(CSO_Settings_t));
				err = MEDIAPB_IPC_CSO_GetSettings(ulCsoSessionId, pSettings);
				if(err != CSO_ERR_OK)
				{
					HxLOG_Error("[%s](L:%d) MEDIAPB_IPC_CSO_GetSettings failed!\n", __FUNCTION__, __LINE__);
				}
			}
			else
			{
				HxLOG_Error("[%s](L:%d) Using cso'setting value from cache\n", __FUNCTION__, __LINE__);
			}
			HxLOG_Debug("Original  pSettings->uiBlockSize(%u)\n", pSettings->uiBlockSize);
			pSettings->uiBlockSize = TS_DECYPT_BLOCK_SIZE; // 188 byte 배수로 decrypt 된다.
			pSettings->bBypassing = TRUE; // calypso에서 pid를 remux하지 않고 바로 보내 준다. IP로 받은 stream의 dvb-subtitle을 사용하기 위해 ...
			err = MEDIAPB_IPC_CSO_SetSettings(ulCsoSessionId, pSettings);
			if(err != CSO_ERR_OK)
			{
				HxLOG_Error("Calypso> SetSettings has failed!\n");
			}
			HxLOG_Debug("New       pSettings->uiBlockSize(%u)\n", pSettings->uiBlockSize);
		}
		HxLOG_Error("%s(%d) [CRB] Start CSO_PROBE in DOWNLOAD. \n", __FUNCTION__, __LINE__);
		err = MEDIAPB_IPC_CSO_Probe(ulCsoSessionId);
		if(err != CSO_ERR_OK)
		{
			HxLOG_Error("[%s](L:%d) Content Verification has failed!\n", __FUNCTION__, __LINE__);
			goto error_cleanup;
		}
	}
	if( punStart->stIpMedia.bContentVerification == FALSE && svcDnpConfig.bProgressiveDownload == TRUE )
	{
		if(pSettings == NULL)
		{
			pSettings = HLIB_STD_MemAlloc(sizeof(CSO_Settings_t));
			HxSTD_MemSet(pSettings, 0x00, sizeof(CSO_Settings_t));
			err = MEDIAPB_IPC_CSO_GetSettings(ulCsoSessionId, pSettings);
		if(err != CSO_ERR_OK)
		{
			HxLOG_Error("[%s](L:%d) Calypso> GetSettings has failed!\n", __FUNCTION__, __LINE__);
			goto error_cleanup;
		}
		}
		else
		{
			HxLOG_Error("[%s](L:%d) Using cso'setting value from cache\n", __FUNCTION__, __LINE__);
		}
		if(pSettings->pSaveFilePath != NULL)
		{
			OxMGR_Free(pSettings->pSaveFilePath);
			pSettings->pSaveFilePath = NULL;
		}
		pSettings->pSaveFilePath = OxMGR_Malloc(DxDNLD_MEDIAPLAY_URL);
		if(pSettings->pSaveFilePath == NULL)
		{
			HxLOG_Error("[%s](L:%d) Memory Allocation failed!\n", __FUNCTION__, __LINE__);
			goto error_cleanup;
		}
		HxSTD_memset(pSettings->pSaveFilePath,0x0,DxDNLD_MEDIAPLAY_URL);
		HxLOG_Error("[%s:%d] Length of pSaveFilePath = %d\n", __FUNCTION__, __LINE__, VK_strlen(pSettings->pSaveFilePath));
		snprintf((char*)pSettings->pSaveFilePath, DxDNLD_MEDIAPLAY_URL, "%s%u",svcDnpConfig.szPDLPath,ulDownloadId);

		err = MEDIAPB_IPC_CSO_SetSettings(ulCsoSessionId, pSettings);
		if(err != CSO_ERR_OK)
		{
			HxLOG_Error("[%s](L:%d) Calypso> SetSettings has failed!\n", __FUNCTION__, __LINE__);
			goto error_cleanup;
		}
		err = MEDIAPB_IPC_CSO_Control(ulCsoSessionId, eCSO_CTRL_STARTBUFFERING, &tParam);
		if(err != CSO_ERR_OK)
		{
			HxLOG_Error("[%s](L:%d) Calypso> Start buffering has failed!\n", __FUNCTION__, __LINE__);
			goto error_cleanup;
		}
		else
		{
			pstInst->unExtInfo.stIpMedia.bDownloadFileCreated = TRUE;
	}
	}

	if(pSettings != NULL)
	{
		// store cache
		HxLOG_Error("[%s](L:%d) store cso's setting value to cache\n", __FUNCTION__, __LINE__);
		pstInst->unExtInfo.stIpMedia.stMediaInfo.pCsoSettings = (void *)pSettings;
	}

	pstInst->unExtInfo.stIpMedia.ulCsoSessionId = ulCsoSessionId;
	pstInst->unExtInfo.stIpMedia.bAllocated = TRUE;
#if 1 // TODO: TASK CREATE

	om_download_ipmedia_EnterSection();
	if(s_bIsAliveDownloadMgrThread == FALSE)
	{
		om_download_ipmedia_CreateDownloadMgrTask();
	}
	om_download_ipmedia_ExitSection();
#endif
	return ERR_OK;

error_cleanup:
	if(pstInst->unExtInfo.stIpMedia.pSession)
	{
		CsoSessionDNLD_t* pSession = (CsoSessionDNLD_t *)pstInst->unExtInfo.stIpMedia.pSession;
		if(pSession->pProbeInfo)
		{
			OxMGR_Free(pSession->pProbeInfo);
		}
		OxMGR_Free(pSession);
		pstInst->unExtInfo.stIpMedia.pSession = NULL;
	}
	MEDIAPB_IPC_CSO_Close(ulCsoSessionId);
	return ERR_FAIL;
#else
	HxLOG_Error("not supported\n");
	return ERR_FAIL;
#endif
}

HERROR om_download_ipmedia_CbStop (HUINT32 ulDownloadId)
{
#ifdef CONFIG_SUPPORT_IMMA
	OmDownload_Instance_t	*pstInst = OM_DOWNLOAD_GetInstance(ulDownloadId);
	OmDownload_IpMediaInfo_t *pIpMediaInfo = NULL;
	CsoSessionDNLD_t *pCsoSession = NULL;

	if (NULL == pstInst)
	{
		HxLOG_Error("No Instance with ulDownloadId(0x%08x)\n", ulDownloadId);
		return ERR_FAIL;
	}

	pIpMediaInfo = OM_DOWNLOAD_GetMediaInfoByCsoId(pstInst->unExtInfo.stIpMedia.ulCsoSessionId);
	pCsoSession = (CsoSessionDNLD_t *)pstInst->unExtInfo.stIpMedia.pSession;
	if (pCsoSession)
	{
		pCsoSession->pSettings = pIpMediaInfo->pCsoSettings;

		om_download_ipmedia_EnterSection();
		pCsoSessionList = HLIB_LIST_Remove(pCsoSessionList,pCsoSession);
		/* pCsoSession is not release!!. move to s_pRemoveScheduleList*/
		om_download_ipmedia_ExitSection();

		HxSEMT_Get(s_uiRemoveScheduleListId);
		s_pRemoveScheduleList = HLIB_LIST_Append(s_pRemoveScheduleList, pCsoSession);
		HxSEMT_Release(s_uiRemoveScheduleListId);

		om_download_ipmedia_AsyncStopSession(pCsoSession);
	}
	pstInst->unExtInfo.stIpMedia.pSession = NULL;

	HxLOG_Debug(">> DownloadId(0x%08x), CSO Session ID:(%d)\n", ulDownloadId, pstInst->unExtInfo.stIpMedia.ulCsoSessionId);

	pstInst->unExtInfo.stIpMedia.bAllocated = FALSE;
	pstInst->unExtInfo.stIpMedia.bDownloadAfterVerify = FALSE;
	return ERR_OK;
#else
	HxLOG_Error("not supported\n");
	return ERR_FAIL;
#endif
}

HERROR om_download_ipmedia_CbPause (HUINT32 ulDownloadId)
{
#ifdef CONFIG_SUPPORT_IMMA
	OmDownload_Instance_t	*pstInst = OM_DOWNLOAD_GetInstance(ulDownloadId);
	CSO_CTRL_PARAM_t tParam;
	CSO_ERR 	err;

	if (NULL == pstInst)
	{
		HxLOG_Error("No Instance with ulDownloadId(0x%08x)\n", ulDownloadId);
		return ERR_FAIL;
	}

	HxSTD_memset(&tParam,0x0,sizeof(CSO_CTRL_PARAM_t));

	err = MEDIAPB_IPC_CSO_Control(pstInst->unExtInfo.stIpMedia.ulCsoSessionId, eCSO_CTRL_PAUSEBUFFERING, &tParam);
	if(err != CSO_ERR_OK)
	{
		HxLOG_Error("Calypso> pause buffering has failed!!\n");
		return ERR_FAIL;
	}

	HxLOG_Debug(">> DownloadId(0x%08x), CSO Session ID:(%d)\n", ulDownloadId, pstInst->unExtInfo.stIpMedia.ulCsoSessionId);
	return ERR_OK;
#else
	HxLOG_Error("not supported\n");
	return ERR_FAIL;
#endif
}

HERROR om_download_ipmedia_CbResume (HUINT32 ulDownloadId)
{
#ifdef CONFIG_SUPPORT_IMMA
	OmDownload_Instance_t	*pstInst = OM_DOWNLOAD_GetInstance(ulDownloadId);
	CSO_CTRL_PARAM_t tParam;
	CSO_ERR 	err;

	if (NULL == pstInst)
	{
		HxLOG_Error("No Instance with ulDownloadId(0x%08x)\n", ulDownloadId);
		return ERR_FAIL;
	}

	HxSTD_memset(&tParam,0x0,sizeof(CSO_CTRL_PARAM_t));

	err = MEDIAPB_IPC_CSO_Control(pstInst->unExtInfo.stIpMedia.ulCsoSessionId, eCSO_CTRL_RESUMEBUFFERING, &tParam);
	if(err != CSO_ERR_OK)
	{
		HxLOG_Error("Calypso> resume buffering has failed!!\n");
		return ERR_FAIL;
	}

	HxLOG_Debug(">> DownloadId(0x%08x), CSO Session ID:(%d)\n", ulDownloadId, pstInst->unExtInfo.stIpMedia.ulCsoSessionId);

	return ERR_OK;
#else
	HxLOG_Error("not supported\n");
	return ERR_FAIL;
#endif
}

