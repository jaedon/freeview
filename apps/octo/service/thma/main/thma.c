/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  thma.c
	@brief

	Description:  									\n
	Module: Thma API Module						\n

	Copyright (c) 2013 HUMAX Co., Ltd.					\n
	All rights reserved.								\n
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

/**************************************************************************************************/
#define _________THMA_Private_Include_________________________________________________
/**************************************************************************************************/
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/statvfs.h>
#include <dirent.h>

#include <vkernel.h>

#include <hlib.h>
#include <hapi.h>
#include <papi.h>
#include "thma_int.h"

#if defined(CONFIG_HUMAX_CRASHREPORT)
#include "hcrlib.h"
#endif

/**************************************************************************************************/
#define _________THMA_Private_Macro_________________________________________________
/**************************************************************************************************/
#define	THMA_CMD_NAME				"thmacmd"
#define	THMA_RETRY_RETRIEVE_TIME	500

#define	CACHE_MIN_SIZE		10000		//	10M
#define	CACHE_MAX_SIZE		500000		//	500M
#define MAX_WAITING         (3 * 60 * 1000)

/**************************************************************************************************/
#define _________THMA_Private_Struct_________________________________________________
/**************************************************************************************************/
#if defined(CONFIG_PROD_OS_EMULATOR)
//TODO;
#else
typedef struct
{
	HCHAR	szFullName[768];
	struct stat	fstat;
} thma_cache_t;
#endif /* CONFIG_PROD_OS_EMULATOR */

/**************************************************************************************************/
#define _________THMA_Private_Value_________________________________________________
/**************************************************************************************************/
static HUINT32	s_ulThmaMsgQueueId = 0;
static HUINT32	s_ulCacheMinSize;
static HUINT32	s_ulCacheMaxSize;

/**************************************************************************************************/
#define _________THMA_Private_Prototype_________________________________________________
/**************************************************************************************************/
#ifdef	CONFIG_DEBUG
static HINT32			thma_cmdtool_init(void);
#endif
static HUINT32	thma_process_job(thma_t *thma);
static void		thma_cache_rearrange(thma_t *thma, const HCHAR *pszMountPath);

/**************************************************************************************************/
#define _________THMA_Private_Init__________________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________THMA_Private_Destroy__________________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________THMA_Private_Process__________________________________________________________
/**************************************************************************************************/
static void thma_on_status_changed (void *userData, HxSocket_Status_e eStatus, HINT32 clientHandle)
{
	thma_t	*thma = (thma_t*)userData;

	if (eStatus == HxSOCKET_REMOVE_CLIENT)
		HLIB_RPC_RemoveClientHandle(thma->nRpcHandle, clientHandle);
}

static void	thma_on_packet_received (void *userData, HINT32 socketHandle, HUINT32 requestId, HINT32 size, void *pvData)
{
	thma_t	*thma = (thma_t*)userData;

	HLIB_RPC_ProcessRPC(thma->nRpcHandle, size, pvData, requestId, socketHandle);
}

static void	thma_server_listener (void *arg)
{
	thma_t	*thma = (thma_t*)arg;

	HLIB_SOCKET_Listen(thma->server, TRUE, thma_on_packet_received, thma_on_status_changed, thma);
}

//	i(ntime)s(source)s(dest)i(bUrgent)i(bSequential)
static HERROR	thma_rpc_getThumbnail(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	thma_t	*thma = (thma_t *)pvUserData;
	thma_pktinfo_t	*pktInfo;
	THMA_MSG_t		msg;

	HxLOG_Assert(nArgc == 5);

	pktInfo = (thma_pktinfo_t*)TH_MEM_Calloc(sizeof(thma_pktinfo_t));
	pktInfo->bCanceled = FALSE;
	pktInfo->ulTime    = HxOBJECT_INT(apArgv[0]);
	pktInfo->pszSource = TH_MEM_StrDup(HxOBJECT_STR(apArgv[1]));
	pktInfo->pszDest   = TH_MEM_StrDup(HxOBJECT_STR(apArgv[2]));
	pktInfo->bUrgent   = HxOBJECT_INT(apArgv[3]) ? TRUE : FALSE;
	pktInfo->bSequence = HxOBJECT_INT(apArgv[4]) ? TRUE : FALSE;

	msg.eMsg = eTHMA_MSG_PACKET;
	msg.ulParam1 = (HUINT32)pktInfo;

	VK_MSG_Send(thma->ulMsgQId, (void*)&msg, sizeof(THMA_MSG_t));

	return ERR_OK;
}

//	s : pszMountPath
static HERROR	thma_rpc_checkCacheBuffer(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	thma_t	*thma = (thma_t*)pvUserData;
	THMA_MSG_t	msg;

	HxLOG_Assert(nArgc == 1);

	msg.eMsg = eTHMA_MSG_CHECKCACHE;
	msg.ulParam1 = (HUINT32)HLIB_STD_StrDup(HxOBJECT_STR(apArgv[0]));

	VK_MSG_Send(thma->ulMsgQId, (void*)&msg, sizeof(THMA_MSG_t));

	return ERR_OK;
}

//	nArgc == 0
static HERROR	thma_rpc_Cancel(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	thma_t	*thma = (thma_t*)pvUserData;
	thma_pktinfo_t	*pktInfo;
	THMA_MSG_t		msg;

	HxLOG_Assert(nArgc == 0);

	pktInfo = (thma_pktinfo_t*)TH_MEM_Calloc(sizeof(thma_pktinfo_t));
	pktInfo->bCanceled = TRUE;

	msg.eMsg = eTHMA_MSG_PACKET;
	msg.ulParam1 = (HUINT32)pktInfo;

	VK_MSG_Send(thma->ulMsgQId, (void*)&msg, sizeof(THMA_MSG_t));

	return ERR_OK;
}

static void	thma_job_freeInfo(thma_jobinfo_t *pstJob)
{
	if (pstJob->pszCacheURL)
		TH_MEM_Free(pstJob->pszCacheURL);
	if (pstJob->pszURL)
		TH_MEM_Free(pstJob->pszURL);
	TH_MEM_Free(pstJob);
}

static HUINT32	thma_job_getRequestId(thma_t *thma)
{
	thma->ulRequestId = (thma->ulRequestId + 1) & 0x0FFFFFFF;

	return thma->ulRequestId;
}

static void	thma_job_finish_callback(HUINT32 ulRequestId, HINT32 nProgress)
{
	THMA_MSG_t	msg;

	msg.eMsg     = eTHMA_MSG_CALLBACK;
	msg.ulParam1 = ulRequestId;
	msg.ulParam2 = nProgress;

	VK_MSG_Send(s_ulThmaMsgQueueId, &msg, sizeof(THMA_MSG_t));
}

static HUINT32	thma_job_getRetryCondition(thma_jobinfo_t *pstJobInfo)
{
	HCHAR	*p;

	p = pstJobInfo->pszURL;
	p = HxSTD_StrRChar(p, '.');
	if (p == NULL)
		return THMA_RETRY_RETRIEVE_TIME;

	if (HLIB_STD_StrCaseCmp(p, ".hjm") == 0)
		return 10000;

	return THMA_RETRY_RETRIEVE_TIME;
}

static HBOOL	thma_job_compare(void *pvSrcJob, void *pvDestJob)
{
	thma_jobinfo_t	*pstSrc  = (thma_jobinfo_t*)pvSrcJob;
	thma_jobinfo_t	*pstDest = (thma_jobinfo_t*)pvDestJob;

	if (HxSTD_StrCmp(pstSrc->pszURL, pstDest->pszURL) != 0)
		return FALSE;
	if (pstSrc->ulTime != pstDest->ulTime)
		return FALSE;
	if (pstSrc->bUrgent != pstDest->bUrgent)
		return FALSE;

	return TRUE;
}

#define	DEFAULT_CACHE_CHECK		5000			//	5sec
#define	DEFAULT_SLEEP_TICK		100
#define	DEFAULT_DEEPSLEEP_TICK	0xFFFFFFFF
static void	thma_system_freemem(void *p)
{
	TH_MEM_Free(p);
}

static void	thma_system_freejobinfo(void *p)
{
	thma_job_freeInfo((thma_jobinfo_t*)p);
}

static HBOOL	thma_job_compareRemove(void *userdata, void *data)
{
	HCHAR	*pszUnmountedPath = (HCHAR*)userdata;
	thma_jobinfo_t	*jobInfo = (thma_jobinfo_t*)data;

	if (HLIB_STD_StrStartWith(jobInfo->pszURL, pszUnmountedPath) == TRUE)
		return TRUE;
	if (HLIB_STD_StrStartWith(jobInfo->pszCacheURL, pszUnmountedPath) == TRUE)
		return TRUE;

	return FALSE;
}

#define	THMA_NOTIFIER_INTERVAL		5000

static void	thma_system_notifyState(thma_t *thma, HINT32 nRemainJobCount, const HCHAR *pszDoneFileName)
{
	HUINT32		ulCurTick = HLIB_STD_GetSystemTick();

	//	forced notifying if donefile is existed.
	if (HxSTD_StrEmpty(pszDoneFileName) == FALSE)
	{
		HLIB_RPC_Notify(thma->nRpcHandle, "thma.onDecodingState", "is", thma->nRemainJobCount, pszDoneFileName);
		thma->ulLastNotiTick = ulCurTick;

		return;
	}

	if (thma->nRemainJobCount != nRemainJobCount)
	{
		thma->nRemainJobCount = nRemainJobCount;
		if ((thma->nRemainJobCount == 0) || ((thma->nRemainJobCount > 0) && (ulCurTick > (thma->ulLastNotiTick + THMA_NOTIFIER_INTERVAL))))
		{
			HLIB_RPC_Notify(thma->nRpcHandle, "thma.onDecodingState", "is", thma->nRemainJobCount, pszDoneFileName);
			thma->ulLastNotiTick = ulCurTick;
		}
	}
}

static void	thma_system_mainTask (void *arg)
{
	HUINT32		ulSleep;
	THMA_MSG_t	msg;
	thma_t	*thma = (thma_t*)arg;

	ulSleep = DEFAULT_SLEEP_TICK;
	while (1)
	{
		if (VK_MSG_ReceiveTimeout(thma->ulMsgQId, &msg, sizeof(THMA_MSG_t), ulSleep) == VK_OK)
		{
			ulSleep = DEFAULT_SLEEP_TICK;
			switch (msg.eMsg)
			{
				case eTHMA_MSG_FORMAT:
					HxLOG_Warning("Device is starting format!!!\n");
					thma->pstJobList = HLIB_LIST_RemoveAllFunc(thma->pstJobList, thma_system_freejobinfo);
					thma_system_notifyState(thma, 0, "");
					HxLOG_Warning("remained job list : [%d]\n", HLIB_LIST_Length(thma->pstJobList));
					break;

				case eTHMA_MSG_UNMOUNTED:
					if (msg.ulParam1)
					{
						HCHAR	*pszUnmountedPath = (HCHAR*)msg.ulParam1;

						HxLOG_Warning("Device is unmounted!!! [%s]\n", pszUnmountedPath);
						HxLOG_Warning("remove job list - before calibration [%d]\n", HLIB_LIST_Length(thma->pstJobList));
						thma->pstJobList = HLIB_LIST_RemoveIf(thma->pstJobList, (void*)pszUnmountedPath, thma_job_compareRemove, thma_system_freejobinfo);
						HxLOG_Warning("remained job list : [%d]\n", HLIB_LIST_Length(thma->pstJobList));
						thma_system_notifyState(thma, HLIB_LIST_Length(thma->pstJobList), "");

						HLIB_STD_MemFree(pszUnmountedPath);
					}
					break;
				case eTHMA_MSG_SHUTDOWN:
					if (thma->pstJobList)
					{
						#define	THMA_SHUTDOWN_WAITING_TIME		10000
						HINT32	nLen = HLIB_LIST_Length(thma->pstJobList);

						HxLOG_Warning("Receiving Shutdown message -- remained job [%d], request waiting [%d]\n", nLen, (nLen > 10) ? THMA_SHUTDOWN_WAITING_TIME : PxDEFAULT_WAIT);
						if (nLen > 10)
							PAPI_WaitForShutDown(THMA_SHUTDOWN_WAITING_TIME);
						else
							PAPI_WaitForShutDown(PxDEFAULT_WAIT);
					}
					break;

				case eTHMA_MSG_CANCEL:
					thma->pstJobList = HLIB_LIST_RemoveAllFunc(thma->pstJobList, thma_system_freejobinfo);
					thma_system_notifyState(thma, 0, "");
					break;

				case eTHMA_MSG_CHECKCACHE:
					{
						HCHAR	*pszMountPath = (HCHAR*)msg.ulParam1;

						thma_cache_rearrange(thma, pszMountPath);
						HLIB_STD_MemFree(pszMountPath);
					}
					break;

				case eTHMA_MSG_PACKET:
					{
						thma_pktinfo_t	*pktInfo = (thma_pktinfo_t*)msg.ulParam1;
						thma_jobinfo_t	*info;

						if (pktInfo->bCanceled)
						{
							HxLOG_Debug("JOB Canceled!\n");
							thma->pstJobList = HLIB_LIST_RemoveAllFunc(thma->pstJobList, thma_system_freejobinfo);

							thma_system_notifyState(thma, 0, "");
						} else
						{
							info = (thma_jobinfo_t*)TH_MEM_Calloc(sizeof(thma_jobinfo_t));
							info->bUrgent     = pktInfo->bUrgent;
							info->bSequential = pktInfo->bSequence;
							info->ulTime      = pktInfo->ulTime;
							info->pszURL      = pktInfo->pszSource;
							info->pszCacheURL = pktInfo->pszDest;
							HxLOG_Debug("add job : [%s][%s]\n", info->pszURL, info->pszCacheURL);
							info->ulConditionTick = HLIB_STD_GetSystemTick();

							if (HLIB_LIST_FindEx(thma->pstJobList, (void*)info, thma_job_compare))
							{
								HxLOG_Debug("ignore job with [%s] because same job is in Q\n", info->pszURL);
								TH_MEM_Free(info);

								break;
							}
#ifdef	THMA_DEBUG_JOBLIST
							{
								static FILE	*__file = NULL;
								static HINT32 _i = 0;

								if (__file == NULL)
									__file = fopen("/thumb.log", "w");
								fprintf(__file, "[%04d] -> %s\n", _i++, info->pszURL);
								fflush(__file);

							}
#endif
							if (info->bUrgent)
								thma->pstJobList = HLIB_LIST_Prepend(thma->pstJobList, (void*)info);
							else
								thma->pstJobList = HLIB_LIST_Append (thma->pstJobList, (void*)info);

							thma_system_notifyState(thma, thma->nRemainJobCount + 1, "");
						}
						TH_MEM_Free(pktInfo);
					}
					break;

				case eTHMA_MSG_CALLBACK:
					if(thma->pstCurrentJob)
					{
						HINT32		nProgress;
						HUINT32		ulRequestId;

						ulRequestId = msg.ulParam1;
						nProgress   = msg.ulParam2;

						if (nProgress == -1)
						{
							#define	THMA_EXTRACT_RETRY_COUNT	1

							//	retrieve fail --> add current job to tail.
							HxLOG_Warning("retrieve thumbnail is failed! - [%s]\n", thma->pstCurrentJob->pszURL);
							thma->pstCurrentJob->nRetryCount++;
							if (thma->pstCurrentJob->nRetryCount >= THMA_EXTRACT_RETRY_COUNT)
							{
								thma_job_freeInfo(thma->pstCurrentJob);
							} else
							{
								HxLOG_Debug("retry count : (%s)[%d]\n", thma->pstCurrentJob->pszURL, thma->pstCurrentJob->nRetryCount);
								thma->pstCurrentJob->bInProgress = FALSE;
								thma->pstCurrentJob->nRetryCount++;
								thma->pstCurrentJob->ulConditionTick = HLIB_STD_GetSystemTick() + thma_job_getRetryCondition(thma->pstCurrentJob);
								thma->pstJobList = HLIB_LIST_Append(thma->pstJobList, (void*)thma->pstCurrentJob);
								thma_system_notifyState(thma, thma->nRemainJobCount + 1, "");
							}
							thma->pstCurrentJob = NULL;
						} else
						if (nProgress == 100)
						{
							HxLOG_Debug("retrieve success : [%s](%s)\n", thma->pstCurrentJob->pszURL, thma->pstCurrentJob->pszCacheURL);
							thma_system_notifyState(thma, thma->nRemainJobCount, thma->pstCurrentJob->pszURL);
							thma_job_freeInfo(thma->pstCurrentJob);
							thma->pstCurrentJob = NULL;
						}
					}
					break;
				default:
					break;
			}
		} else
		{
			ulSleep = thma_process_job(thma);
		}
	}
}

static HBOOL	thma_system_compMountPath(void *pvCachePath, void *pvMountPath)
{
	HCHAR	*pszCachePath = (HCHAR*)pvCachePath;
	HCHAR	*pszMountPath = (HCHAR*)pvMountPath;

	if (HLIB_STD_StrStartWith(pszCachePath, pszMountPath))
		return TRUE;

	return FALSE;
}

static HUINT32	thma_cache_getBufferSize(const HCHAR *pszMountPath, HUINT32 ulMountSize, HUINT32 ulMountFreeSize)
{

	HUINT32		ulThresHold;

	ulThresHold = ulMountSize / 50;
	if (ulThresHold > s_ulCacheMaxSize)
		ulThresHold = s_ulCacheMaxSize;
	if (ulThresHold < s_ulCacheMinSize)
		ulThresHold = s_ulCacheMinSize;
//	if (ulThresHold > s_ulCacheMinSize && ulThresHold < s_ulCacheMaxSize)
//		ulThresHold = s_ulCacheMaxSize / 2;

	if (ulThresHold > ulMountFreeSize)
		ulThresHold = ulMountFreeSize / 2;
	if (ulThresHold < s_ulCacheMinSize)
		ulThresHold = s_ulCacheMinSize;

	HxLOG_Warning("_pszMountPath[%s] ulMountSize[%d] ulMountFreeSize[%d] ulThresHold[%d] \n", pszMountPath, ulMountSize, ulMountFreeSize, ulThresHold);

	return ulThresHold;
}

static void		thma_cache_rearrange(thma_t *thma, const HCHAR *pszMountPath)
{
#if defined(CONFIG_PROD_OS_EMULATOR)
//TODO;
#else
	HINT32	nThresFileNum, nScanFileNum;
	HUINT32	ulMountSize, ulThresHold, ulMountFreeSize;
	HCHAR	szMountPath[128];
	HCHAR	*pszStatName;
	HxList_t	*pstCacheList = NULL;
	struct statvfs	vfs;

	struct dirent	**items;
	HINT32			nItems, i;

	HxLOG_Warning("<---- start cache arrange....\n");
	if (statvfs(pszMountPath, &vfs) != 0)
	{
		HxLOG_Critical("invalid mount path : check your cache folder! (%s)\n", pszMountPath);
		return;
	}

	ulMountSize = (vfs.f_blocks * (long long)vfs.f_bsize) / 1024;
	ulMountFreeSize = (vfs.f_blocks * (long long)vfs.f_bfree) / 1024;
	ulThresHold = thma_cache_getBufferSize(pszMountPath, ulMountSize, ulMountFreeSize);
	nThresFileNum = ulThresHold / 5;
	if (nThresFileNum >= 0x20000)
		nThresFileNum = 0x20000;

	HxLOG_Warning("(%d) rearrange %s/.cache : threshold (%d) thresholdNum (%d)\n", HLIB_STD_GetSystemTick(), pszMountPath, ulThresHold, nThresFileNum);

	HxSTD_snprintf(szMountPath, 128, "%s/.cache", pszMountPath);
	HxLOG_Debug("scan dir :[%s]\n", szMountPath);
	nItems = scandir(szMountPath, &items, NULL, NULL);
	HxLOG_Debug("nItems : [%d] tick [%d]\n", nItems, HLIB_STD_GetSystemTick());
	if (nItems > (nThresFileNum * 2) || nItems < 0)
	{
		HxLOG_Error("<---- remove all....(%d / %d)\n", nItems, nThresFileNum);
		HLIB_DIR_Delete(szMountPath);
//		HxLOG_Debug("tick [%d]\n", HLIB_STD_GetSystemTick());

		return;
	}

	pszStatName = (HCHAR*)TH_MEM_Alloc(768);
	if (NULL == pszStatName)
	{
		HxLOG_Error("TH_MEM_Alloc NULL\n");
		return;
	}

	for (i = 0; i < nItems; i++)
	{
		struct stat	fstat;
		if (!HxSTD_StrCmp(items[i]->d_name, ".") || !HxSTD_StrCmp(items[i]->d_name, ".."))
			continue;

		HxSTD_snprintf(pszStatName, 768, "%s/%s", szMountPath, items[i]->d_name);
		if (stat(pszStatName, &fstat) == 0)
		{
			thma_cache_t	*info = (thma_cache_t*)TH_MEM_Alloc(sizeof(thma_cache_t));

			HLIB_STD_StrNCpySafe(info->szFullName, pszStatName, 768);
			HxSTD_MemCopy(&info->fstat, &fstat, sizeof(struct stat));

			pstCacheList = HLIB_LIST_Append(pstCacheList, (void*)info);
		}
	}
//	HxLOG_Debug("tick [%d]\n", HLIB_STD_GetSystemTick());
	nScanFileNum = nItems;
	while(nItems--)
		free(items[nItems]);
	free(items);

	TH_MEM_Free(pszStatName);

//	HxLOG_Debug("tick [%d]\n", HLIB_STD_GetSystemTick());
	{
		HUINT32			ulDataSize = 0;
		HxList_t		*list;
		thma_cache_t	*info;
		time_t			minTime, maxTime, thresTime;

		minTime = 0x7fffffff;
		maxTime = 0;
		list = pstCacheList;
		while (list)
		{
			info = (thma_cache_t*)HLIB_LIST_Data(list);
			ulDataSize += info->fstat.st_size;
			list = list->next;
			if (info->fstat.st_atime > maxTime)
				maxTime = info->fstat.st_atime;
			if (info->fstat.st_atime < minTime)
				minTime = info->fstat.st_atime;
		}

		ulDataSize /= 1024;
		HxLOG_Debug("dataSize[%d] thresHold[%d] minTime[%d] maxTime[%d]\n", ulDataSize, ulThresHold, minTime, maxTime);
		if ((ulDataSize > ulThresHold) || (nScanFileNum > nThresFileNum))
		{
			//HUINT32	ulUapiId;

			thresTime = minTime + (maxTime - minTime) / 2;
			list = pstCacheList;
			while (list)
			{
				info = (thma_cache_t*)HLIB_LIST_Data(list);
				if (info->fstat.st_atime < thresTime)
				{
					HxLOG_Error("THMA Thumbnailer : remove cache : [%s]\n", info->szFullName);
					HLIB_FILE_Delete(info->szFullName);
					if (thma_IsLargeThumbnailFile(info->szFullName) == FALSE)
					{
						HCHAR	*pszLargeUrl = thma_getLargeThumbnailName(info->szFullName, 768);

						HLIB_FILE_Delete(pszLargeUrl);
					}

					//UAPI_Remove(info->szFullName, "", FALSE, TRUE, &ulUapiId);
				}
				list = list->next;
			}

		}
	}
//	HxLOG_Debug("tick [%d]\n", HLIB_STD_GetSystemTick());
	HLIB_LIST_RemoveAllFunc(pstCacheList, thma_system_freemem);
//	HxLOG_Debug("tick [%d]\n", HLIB_STD_GetSystemTick());

	HxLOG_Warning("<---- end cache arrange....\n");
#endif /* CONFIG_PROD_OS_EMULATOR */
}

static void		thma_print_job(void *pvData)
{
	thma_jobinfo_t	*job = (thma_jobinfo_t*)pvData;

	HxLOG_Debug("--[%s] (%d) (%d)\n", job->pszURL, job->nRetryCount, job->ulConditionTick);
}

static void		thma_free_func(void *pvData)
{
    thma_job_freeInfo((thma_jobinfo_t*)pvData);
}
static HBOOL	thma_condition_func(void *userdata, void *data)
{
	thma_jobinfo_t	*job = (thma_jobinfo_t*)data;

    if(job->ulConditionTick  + MAX_WAITING < HLIB_STD_GetSystemTick())
    {
        HxLOG_Debug("CancelJob : %s \n",job->pszURL);
        return TRUE;
    }
	return FALSE;
}

static HERROR thma_check(thma_t *thma)
{
    thma_jobinfo_t	*job;

    thma->pstJobList = HLIB_LIST_RemoveIf(thma->pstJobList, NULL, thma_condition_func, thma_free_func);
    if(thma->pstCurrentJob)
    {
        job = thma->pstCurrentJob;
        if(job->ulConditionTick + MAX_WAITING < HLIB_STD_GetSystemTick())
        {
            HxLOG_Warning("Cancel CurJob : %s \n",job->pszURL);
            thma_job_finish_callback(job->ulRequestId , 100);
            return ERR_FAIL;
        }
    }
    return ERR_OK;
}

static HUINT32	thma_process_job(thma_t *thma)
{
	thma_jobinfo_t	*job;

    if(thma_check(thma) == ERR_FAIL)
        return DEFAULT_SLEEP_TICK;

	if (thma->pstCurrentJob == NULL)
	{
		if (thma->pstJobList)
		{

			job = (thma_jobinfo_t*)HLIB_LIST_Data(thma->pstJobList);
			HxLOG_Debug("Condition Tick : [%d : %d]\n", job->ulConditionTick, HLIB_STD_GetSystemTick());
			if (job->ulConditionTick < HLIB_STD_GetSystemTick())
			{
				thma->pstCurrentJob = (thma_jobinfo_t*)HLIB_LIST_Data(thma->pstJobList);
				thma->pstJobList = HLIB_LIST_Remove(thma->pstJobList, thma->pstCurrentJob);
				thma_system_notifyState(thma, thma->nRemainJobCount - 1, "");
			} else
			{
				thma->pstJobList = HLIB_LIST_Remove(thma->pstJobList, (void*)job);
				thma->pstJobList = HLIB_LIST_Append(thma->pstJobList, (void*)job);
			}

#ifdef	CONFIG_DEBUG
			HxLOG_Debug("Remain Job List. - (%d) - (%p)\n", HLIB_STD_GetSystemTick(), thma->pstCurrentJob);
			HLIB_LIST_Foreach(thma->pstJobList, thma_print_job);
#endif
		}
	}

	if (thma->pstCurrentJob)
	{
		job = thma->pstCurrentJob;
		if (job->bInProgress)
			return DEFAULT_SLEEP_TICK;

		job->bInProgress = TRUE;
		job->ulRequestId = thma_job_getRequestId(thma);

		//	try to make thumbnail again if input include bSequential flag.
		if ((HLIB_FILE_Exist(job->pszCacheURL) == TRUE) && (!job->bSequential))
		{
			thma_job_finish_callback(job->ulRequestId, 100);
		} else
		{
			THMA_PORT_GetData(thma, job->ulRequestId, thma_job_finish_callback);

			if (HLIB_LIST_FindEx(thma->pstMountPath, job->pszCacheURL, thma_system_compMountPath) == NULL)
			{
				HCHAR	szMountPath[512];

				if (HLIB_DIR_GetMountPath(job->pszCacheURL, szMountPath, 512) == ERR_OK)
					thma->pstMountPath = HLIB_LIST_Append(thma->pstMountPath, (void*)TH_MEM_StrDup(szMountPath));
			}
		}

		thma->bCacheIsChanged = TRUE;

		return DEFAULT_SLEEP_TICK;
	} else
	{
		//	Nothing to do
		//	rearrange cache buffer which is registered in thma->pstMountPath list.
		if (thma->pstMountPath == NULL)
			return DEFAULT_DEEPSLEEP_TICK;

		if ((thma->ulLastCacheCheckTick + DEFAULT_CACHE_CHECK) > HLIB_STD_GetSystemTick() &&
			(thma->bCacheIsChanged == TRUE))
		{
			HCHAR	*pszMountPath;

			pszMountPath = (HCHAR*)HLIB_LIST_Data(thma->pstMountPath);
			thma->pstMountPath = HLIB_LIST_Remove(thma->pstMountPath, pszMountPath);
			thma_cache_rearrange(thma, pszMountPath);
			TH_MEM_Free(pszMountPath);

			thma->ulLastCacheCheckTick = HLIB_STD_GetSystemTick();
			thma->bCacheIsChanged = FALSE;
		}

		return DEFAULT_SLEEP_TICK;
	}
}

/**************************************************************************************************/
#define _________THMA_Private_Proc_MSG_RUNNIG__________________________________________________________
/**************************************************************************************************/
static HERROR	thma_system_onSystemStatusChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	THMA_MSG_t	stMsg;
	PxSTATUS_e	ePapiStatus;

	HxLOG_Assert(nArgc == 1);

	ePapiStatus = HxOBJECT_INT(apArgv[0]);

	if(ePapiStatus == ePAMA_STATUS_Shutdown)
	{
		//	TODO: go on in standby mode.
		stMsg.eMsg = eTHMA_MSG_SHUTDOWN;
		VK_MSG_Send(s_ulThmaMsgQueueId, (void*)&stMsg, sizeof(THMA_MSG_t));
	}

	return ERR_OK;
}

static HERROR	thma_system_onStorageUnmounted(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	THMA_MSG_t	stMsg;

	HxLOG_Assert(nArgc == 1);

	stMsg.eMsg = eTHMA_MSG_UNMOUNTED;
	stMsg.ulParam1 = (HUINT32)HLIB_STD_StrDup(HxOBJECT_STR(apArgv[0]));
	VK_MSG_Send(s_ulThmaMsgQueueId, (void*)&stMsg, sizeof(THMA_MSG_t));

	return ERR_OK;
}

//	s : mountpath
static HERROR	thma_system_onFormatOccurred(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	THMA_MSG_t	stMsg;

	stMsg.eMsg = eTHMA_MSG_FORMAT;
	VK_MSG_Send(s_ulThmaMsgQueueId, (void*)&stMsg, sizeof(THMA_MSG_t));

	return ERR_OK;
}

static HERROR	thma_system_onFvcBaseUrlChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	if (nArgc != 2)
	{
		return ERR_FAIL;
	}

	THMA_ITEM_SetItem(eTHMA_ITEM_FVC_BASE_URL, (char*)HxOBJECT_STR(apArgv[0]));

	return ERR_OK;
}

static HERROR	thma_system_onFvcAuthHashChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	if (nArgc != 2)
	{
		return ERR_FAIL;
	}

	THMA_ITEM_SetItem(eTHMA_ITEM_FVC_AUTH_HASH, (char*)HxOBJECT_STR(apArgv[0]));

	return ERR_OK;
}

static HERROR	thma_system_onFvcAuthTimeStampChanged (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	if (nArgc != 2)
	{
		return ERR_FAIL;
	}

	THMA_ITEM_SetItem(eTHMA_ITEM_FVC_AUTH_TIME_STAMP, (char*)HxOBJECT_STR(apArgv[0]));

	return ERR_OK;
}

/**************************************************************************************************/
#define _________THMA_Public_Proc____________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________THMA_Public_Functions__________________________________________________________
/**************************************************************************************************/
#ifdef	CONFIG_DEBUG
static thma_t	*s_pstThma = NULL;
#endif

void	THMA_Main(HINT32 argc, HCHAR **argv)
{
	thma_t			*thma;
	unsigned long	tid;
	int				vkError;

#ifdef	CONFIG_DEBUG
	thma_cmdtool_init();
#endif

	thma = (thma_t*)TH_MEM_Calloc(sizeof(thma_t));
	thma->server = HLIB_EXTRA_OpenServer("thma");//HLIB_SOCKET_OpenServer(HxSOCKET_UDS, THMA_SIGNAL_UDS);
	thma->nRpcHandle = HLIB_RPC_Open(thma->server, HLIB_STD_GetPID());
	thma->ulLastCacheCheckTick = 0;

#ifdef	CONFIG_DEBUG
	s_pstThma = thma;
#endif

	vkError = VK_MSG_Create(512, sizeof(THMA_MSG_t), "thma_msg", (unsigned long*)&thma->ulMsgQId, VK_SUSPENDTYPE_FIFO);
	if (VK_OK != vkError)
	{
		HxLOG_Error ("[%s:%d] VK_MSG_Create err:0x%08x\n", __FUNCTION__, __LINE__, vkError);
	}
	s_ulThmaMsgQueueId = thma->ulMsgQId;

	vkError = VK_TASK_Create(thma_system_mainTask, VK_TASK_PRIORITY_MW_REF, SIZE_512K, "thmaMain", (void*)thma, &tid, 0);
	if (VK_OK != vkError)
	{
		HxLOG_Error ("[%s:%d] VK_TASK_Create err:0x%08x\n", __FUNCTION__, __LINE__, vkError);
	}
	vkError = VK_TASK_Start(tid);
	if (VK_OK != vkError)
	{
		HxLOG_Error ("[%s:%d] VK_TASK_Start err:0x%08x\n", __FUNCTION__, __LINE__, vkError);
	}

	vkError = VK_TASK_Create(thma_server_listener, VK_TASK_PRIORITY_MW_REF, SIZE_64K, "thma", (void*)thma, &tid, 0);
	if (VK_OK != vkError)
	{
		HxLOG_Error ("[%s:%d] VK_TASK_Create err:0x%08x\n", __FUNCTION__, __LINE__, vkError);
	}
	vkError = VK_TASK_Start(tid);
	if (VK_OK != vkError)
	{
		HxLOG_Error ("[%s:%d] VK_TASK_Start err:0x%08x\n", __FUNCTION__, __LINE__, vkError);
	}

	THMA_PORT_Init();

	HLIB_RPC_RegisterCall(thma->nRpcHandle, "thma.getThumbnail", "issii", thma_rpc_getThumbnail,
		thma, "get Thumbnail: (HINT32 nTime, HCHAR *source, HCHAR *dst, HBOOL bUrgent, HBOOL bSequential)");
	HLIB_RPC_RegisterCall(thma->nRpcHandle, "thma.cancel", NULL, thma_rpc_Cancel,
		thma, "cancel retrieveing thumbnail.");
	HLIB_RPC_RegisterCall(thma->nRpcHandle, "thma.checkCacheBuffer", "s", thma_rpc_checkCacheBuffer,
		thma, "check and rearrange cache buffer : (const HCHAR *pszMountPath)");
	HLIB_RPC_CreateNotification(thma->nRpcHandle, "thma.onDecodingState", "is", "notify thma operation (HINT32 nRemainedJobCount, const HCHAR *pszDoneFileName)");

	(void)argc;
	(void)argv;
}

/**************************************************************************************************/
#define _________THMA_CMD_FUNCTION_________________________________________________________
/**************************************************************************************************/
#ifdef	CONFIG_DEBUG
#if	0
static HINT32	thma_cmdtool_scanDir(void *szArgList)
{

	return 0;
}

static HINT32	thma_cmdtool_printJobList(void *szArgList)
{
	return 0;
}
#endif

static HINT32	thma_cmdtool_printjob(void *szArgList)
{
	thma_t	*thma = s_pstThma;
	HxList_t	*list;
	thma_jobinfo_t *jobInfo;

	if (thma == NULL)
	{
		HLIB_CMD_Printf("THMA is not initialized!\n");
		return HxCMD_ERR;
	}

	HLIB_CMD_Printf("CurJob - [%s]\n", (thma->pstCurrentJob) ?thma->pstCurrentJob->pszURL : "NULL");
	list = thma->pstJobList;
	while(list)
	{
		jobInfo = (thma_jobinfo_t*)HLIB_LIST_Data(list);
		HLIB_CMD_Printf("Job - [%s]\n", jobInfo->pszURL);
		list = list->next;
	}

	return HxCMD_OK;
}

static HINT32	thma_cmdtool_init(void)
{
	HLIB_CMD_RegisterWord("main", thma_cmdtool_printjob, "printjob", "printjob", "printjob");

	return 0;
}
#endif

/**************************************************************************************************/
#define _________THMA_End_Of_File____________________________________________________
/**************************************************************************************************/
int	thma_main(int argc, char *argv[])
{
	HCHAR	*pszEnvCacheMax, *pszEnvCacheMin;

	VK_Init();
	HLIB_DBG_Init();
#if defined(CONFIG_HUMAX_CRASHREPORT)
//	HCR_Init(NULL);
#endif

	s_ulCacheMaxSize = CACHE_MAX_SIZE;
	s_ulCacheMinSize = CACHE_MIN_SIZE;

	pszEnvCacheMax = getenv("THMA_CACHE_MAX");
	pszEnvCacheMin = getenv("THMA_CACHE_MIN");
	if (pszEnvCacheMax)
	{
		s_ulCacheMaxSize = HLIB_STD_StrToUINT32(pszEnvCacheMax);
		HxLOG_Warning("THMA Cache is redefined (MAX : %dKB)\n", s_ulCacheMaxSize);
	}
	if (pszEnvCacheMin)
	{
		s_ulCacheMinSize = HLIB_STD_StrToUINT32(pszEnvCacheMin);
		HxLOG_Warning("THMA Cache is redefined (MIN : %dKB)\n", s_ulCacheMinSize);
	}

	//	Drop Root Previleges --> if CONFIG_DROP_ROOT_PRIVILEGES is ON,
	HLIB_EXTRA_DropRootPrivileges("thma");

	HLIB_TRANSPORT_HttpInit();
	HLIB_TRANSPORT_FtpInit();

	THMA_ITEM_InitDataManager();

	HAPI_ConnectSignal("signal:onFvcBaseUrlChanged", 		NULL, thma_system_onFvcBaseUrlChanged);
	HAPI_ConnectSignal("signal:onFvcAuthHashChanged",   	NULL, thma_system_onFvcAuthHashChanged);
	HAPI_ConnectSignal("signal:onFvcAuthTimeStampChanged",	NULL, thma_system_onFvcAuthTimeStampChanged);

	HAPI_ConnectSignal("signal:onSystemStateChanged", NULL, thma_system_onSystemStatusChanged);
	HAPI_ConnectSignal("signal:onStorageUnmounted",   NULL, thma_system_onStorageUnmounted);
	HAPI_ConnectSignal("signal:onFormatOccurred",	  NULL, thma_system_onFormatOccurred);

	THMA_Main(argc, argv);

	return 0;
}

int	main(int argc, char *argv[])
{
	thma_main(argc, argv);

	while (1)
		VK_TASK_Sleep(100000);

	return 0;
}

