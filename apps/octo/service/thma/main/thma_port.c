/**************************************************************
*	http://www.humaxdigital.com
*	@author			humax
**************************************************************/


/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding ?» »ç¿ëÇÏ´Â source file ?Ô´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ¨Ï 2011-2012 Humax Co., Ltd.
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
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/

/**************************************************************************************************/
#define _________THMA_Private_Include_________________________________________________
/**************************************************************************************************/
#include <sys/stat.h>

#include <vkernel.h>
#include <hlib.h>
#include "thma_int.h"

/**************************************************************************************************/
#define _________THMA_Private_Macro_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________THMA_Private_Struct_________________________________________________
/**************************************************************************************************/
typedef struct
{
	HUINT32			ulRequestId;
	HUINT32			ulTime;
	HBOOL			bSequential;
	HCHAR			*pszURL;
	HCHAR			*pszCacheURL;
	THMA_Progress_f	pfnCallback;
} thma_extinfo_t;

/**************************************************************************************************/
#define _________THMA_Private_Value_________________________________________________
/**************************************************************************************************/
static HUINT32	s_ulExtractQueueId = 0;
static HxHASH_t	*s_pstExtractTable = NULL;

/**************************************************************************************************/
#define _________THMA_Private_Prototype_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________THMA_Private_Init__________________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________THMA_Private_Destroy__________________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________THMA_Private_Process__________________________________________________________
/**************************************************************************************************/
STATIC void thma_extract_http_callback(HxTRANSPORT_e event, HINT32 param1, HINT32 param2)
{
	HxLOG_Debug("callback : event[%d] - (%d, %d)\n", event, param1, param2);
	switch(event)
	{
		case eHxTRANSPORT_EVENT_PROGRESS_CHANGED:
		case eHxTRANSPORT_EVENT_COMPLETED:
		case eHxTRANSPORT_EVENT_DOWNLOADSTOP:
		case eHxTRANSPORT_EVENT_UNDEFINED:
		default:
			break;
	}
}

static void	thma_port_makeCacheFolder(const HCHAR *pszFullName)
{
	HCHAR	szFolderName[512], *p;

	HxSTD_StrNCpy(szFolderName, pszFullName, 511);
	szFolderName[511] = 0;
	p = HxSTD_StrRChar(szFolderName, '/');
	if (p == NULL)
	{
		HxLOG_Error("can't resolve cache path[%s]\n", pszFullName);
		return;
	}
	*p = 0;

	if (HLIB_FILE_Exist(szFolderName) == FALSE)
	{
		HxLOG_Debug("create folder [%s]\n", szFolderName);
		HLIB_DIR_Make(szFolderName);

#if defined(CONFIG_DROP_ROOT_PRIVILEGES)
		if (HLIB_EXTRA_ChownWithParam(szFolderName, UID_thma, GID_pvr) != ERR_OK)
		{
			HxLOG_Error("Fail to chown path = %s \n", szFolderName);
		}

		if (HLIB_EXTRA_Chmod(szFolderName, 0770) != ERR_OK)
		{
			HxLOG_Error("	Fail to chmod path = %s \n", szFolderName);
		}
#endif

		if (HLIB_DIR_Chmod(szFolderName, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IXUSR | S_IXGRP) != ERR_OK)
		{
			HxLOG_Error ("Fail to HLIB_DIR_Chmod\n");
		}
	}
}

static HERROR	thma_port_extractHttp(const HCHAR *pszURL, const HCHAR *pszCacheURL)
{
	HERROR		err;
	HFLOAT64	ullDownloadSize;

	HxTRANSPORT_Http_t			*hHttpHandle = NULL;
#if defined(CONFIG_OP_UK_DTT)
	HxTRANSPORT_Httpheader_t	*pHttpHeader = NULL;
#endif
	HxLOG_Debug("pszURL : [%s] pszCacheURL : [%s]\n", pszURL, pszCacheURL);

	hHttpHandle = HLIB_TRANSPORT_HttpOpen();
	if (hHttpHandle == NULL)
		goto err_rtn;

#if defined(CONFIG_OP_UK_DTT)
{
	HCHAR		szHeaderBuffer[128] = {0,};
	HUINT8		*pAuthHash = NULL;
	HUINT8		*pAuthTimeStamp = NULL;

	pAuthHash = THMA_ITEM_GetItemPoint(eTHMA_ITEM_FVC_AUTH_HASH);
	if (pAuthHash != NULL)
	{
		HxSTD_snprintf(szHeaderBuffer,128,"%s: %s","x-auth-hash", pAuthHash);
		pHttpHeader = HLIB_TRANSPORT_HttpAppendHeader(pHttpHeader, szHeaderBuffer);
		if (pHttpHeader == NULL)
		{
			if (pAuthHash) {TH_MEM_Free(pAuthHash);}
			if (pAuthTimeStamp) {TH_MEM_Free(pAuthTimeStamp);}
			if (pHttpHeader)HLIB_TRANSPORT_HttpFreeHeader(pHttpHeader);
			if (hHttpHandle) HLIB_TRANSPORT_HttpClose(hHttpHandle);

			return ERR_FAIL;
		}

	}

	pAuthTimeStamp = THMA_ITEM_GetItemPoint(eTHMA_ITEM_FVC_AUTH_TIME_STAMP);
	if (pAuthTimeStamp != NULL)
	{
		HxSTD_snprintf(szHeaderBuffer,128,"%s: %s","x-auth-timestamp", pAuthTimeStamp);
		pHttpHeader = HLIB_TRANSPORT_HttpAppendHeader(pHttpHeader, szHeaderBuffer);
		if (pHttpHeader == NULL)
		{
			if (pAuthHash) {TH_MEM_Free(pAuthHash);}
			if (pAuthTimeStamp) {TH_MEM_Free(pAuthTimeStamp);}
			if (pHttpHeader)HLIB_TRANSPORT_HttpFreeHeader(pHttpHeader);
			if (hHttpHandle) HLIB_TRANSPORT_HttpClose(hHttpHandle);


			return ERR_FAIL;
		}
	}

	if (HLIB_TRANSPORT_HttpSetHeaderToTP(hHttpHandle, pHttpHeader) != ERR_OK)
	{
		if (pAuthHash) {TH_MEM_Free(pAuthHash);}
		if (pAuthTimeStamp) {TH_MEM_Free(pAuthTimeStamp);}
		if (pHttpHeader)HLIB_TRANSPORT_HttpFreeHeader(pHttpHeader);
		if (hHttpHandle) HLIB_TRANSPORT_HttpClose(hHttpHandle);

		return ERR_FAIL;
	}

	if (pAuthHash) {TH_MEM_Free(pAuthHash);}
	if (pAuthTimeStamp) {TH_MEM_Free(pAuthTimeStamp);}
}
#endif

	HLIB_TRANSPORT_HttpSetListener(hHttpHandle, thma_extract_http_callback, (HxTRANSPORT_Http_t *)hHttpHandle);
	err = HLIB_TRANSPORT_HttpSetPeerVeryfication(hHttpHandle, FALSE);
	if(err != ERR_OK)
	{
		HxLOG_Error("peer verification set was failed\n");

		goto err_rtn;
	}

//	HERROR HLIB_TRANSPORT_HttpSetTimeout(HxTRANSPORT_Http_t *ptHttp, HUINT32 ulTimeoutSec)
	HxLOG_Debug("get url [%s]\n", pszURL);
	err = HLIB_TRANSPORT_HttpGetAndStore(hHttpHandle, pszURL, pszCacheURL, &ullDownloadSize);

#if defined(CONFIG_OP_UK_DTT)
	if (pHttpHeader == NULL)
		HLIB_TRANSPORT_HttpFreeHeader(pHttpHeader);
#endif
	if (hHttpHandle)
		HLIB_TRANSPORT_HttpClose(hHttpHandle);

	HxLOG_Debug("[%s](%d) ---< end\n");

	return err;

err_rtn:
#if defined(CONFIG_OP_UK_DTT)
	if (pHttpHeader)
		HLIB_TRANSPORT_HttpFreeHeader(pHttpHeader);
#endif
	if (hHttpHandle)
		HLIB_TRANSPORT_HttpClose(hHttpHandle);

	return ERR_FAIL;
}

static HERROR	thma_port_extractFtp(const HCHAR *pszURL, const HCHAR *pszCacheURL)
{
	HERROR		err;
	HFLOAT64	ullDownloadSize;

	HxTRANSPORT_Ftp_t	*hFtpHandle;

	hFtpHandle = HLIB_TRANSPORT_FtpOpen();
	err = HLIB_TRANSPORT_FtpDownload(hFtpHandle, pszURL, pszCacheURL, &ullDownloadSize);
	HLIB_TRANSPORT_FtpClose(hFtpHandle);

	return err;
}

static HERROR	thma_port_extractFile(const HCHAR *pszURL, const HCHAR *pszCacheURL, HUINT32 ulTime, HBOOL bSequential)
{
	HCHAR	*pszExtName, szExtName[16];
	HCHAR	*pszFileName;
	HERROR	(*extract_func)(const HCHAR *pszURL, const HCHAR *pszCacheURL, HUINT32 ulTime);

	//	TODO: add function for extract thumbnail from mp3, avi, ts and etc which is based on file://
	pszFileName = (HCHAR*)pszURL;
	if (HLIB_STD_StrStartWith(pszURL, "file:") == TRUE)
		pszFileName += HxSTD_StrLen("file://");
	else if (HLIB_STD_StrCaseStartWith(pszURL, "HPVR:")== TRUE)
		pszFileName += HxSTD_StrLen("HPVR:/");

    // check media file..
    if(HLIB_FILE_Exist(pszFileName)==FALSE)
    {
        HxLOG_Error("can't find source... (%s)\n", pszFileName);
        return ERR_FAIL;
    }

    HxLOG_Debug("pszURL :%s\n",pszURL);
	pszExtName = HxSTD_StrRChar(pszFileName, '.');
	if (pszExtName == NULL)
	{
		HxLOG_Error("can't find file extension. (%s)\n", pszURL);
		return ERR_FAIL;
	}
	pszExtName += 1;
	if (bSequential)
		HxSTD_snprintf(szExtName, 15, "%s_a", pszExtName);
	else
		HLIB_STD_StrNCpySafe(szExtName, pszExtName, 16);

	pszExtName = HLIB_STD_StrLower(szExtName);
	extract_func = HLIB_HASH_Lookup(s_pstExtractTable, (void*)pszExtName);
	if (extract_func)
		return extract_func(pszURL, pszCacheURL, ulTime);

	HxLOG_Debug("not defined extension. (%s)\n", pszURL);

	return ERR_FAIL;
}

static void thma_port_extractTask(void *arg)
{
	HERROR		err;
	THMA_MSG_t	msg;

	HLIB_TRANSPORT_FtpInit();
	HLIB_TRANSPORT_HttpInit();

	while (1)
	{
		if (VK_MSG_Receive(s_ulExtractQueueId, &msg, sizeof(THMA_MSG_t)) == VK_OK)
		{
			if (msg.eMsg == eTHMA_MSG_EXTRACT)
			{
				thma_extinfo_t	*extInfo = (thma_extinfo_t*)msg.ulParam1;

				if (extInfo && extInfo->pszCacheURL)
				{
					err = ERR_FAIL;

					thma_port_makeCacheFolder(extInfo->pszCacheURL);
					if (HLIB_STD_StrStartWith(extInfo->pszURL, "http:"))
					{
						err = thma_port_extractHttp(extInfo->pszURL, extInfo->pszCacheURL);
					} else
					if (HLIB_STD_StrStartWith(extInfo->pszURL, "https:"))
					{
						err = thma_port_extractHttp(extInfo->pszURL, extInfo->pszCacheURL);
					} else
					if (HLIB_STD_StrStartWith(extInfo->pszURL, "ftp:"))
					{
						err = thma_port_extractFtp(extInfo->pszURL, extInfo->pszCacheURL);
					} else
					{
						err = thma_port_extractFile(extInfo->pszURL, extInfo->pszCacheURL, extInfo->ulTime, extInfo->bSequential);
					}

					if (err == ERR_OK)
					{
						if (extInfo->pfnCallback)
							extInfo->pfnCallback(extInfo->ulRequestId, 100);
					} else
					{
						if (extInfo->pfnCallback)
							extInfo->pfnCallback(extInfo->ulRequestId, -1);
					}

#if defined(CONFIG_DROP_ROOT_PRIVILEGES)
					if (err == ERR_OK)
					{
						if (HLIB_EXTRA_ChownWithParam(extInfo->pszCacheURL, UID_thma, GID_pvr) != ERR_OK)
						{
							HxLOG_Error("Fail to chown path = %s \n", extInfo->pszCacheURL);
						}

						if (HLIB_EXTRA_Chmod(extInfo->pszCacheURL, 0640) != ERR_OK)
						{
							HxLOG_Error("	Fail to chmod path = %s \n", extInfo->pszCacheURL);
						}
					}
#endif
					if (extInfo->pszURL)
						TH_MEM_Free(extInfo->pszURL);
					if (extInfo->pszCacheURL)
						TH_MEM_Free(extInfo->pszCacheURL);
					TH_MEM_Free(extInfo);
				}
			}
		}
	}

	HLIB_TRANSPORT_HttpDestory();
	HLIB_TRANSPORT_FtpDestory();
}

/**************************************************************************************************/
#define _________THMA_Private_Proc_MSG_RUNNIG__________________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________THMA_Public_Proc____________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________THMA_Public_Functions__________________________________________________________
/**************************************************************************************************/
void	THMA_PORT_Init(void)
{
	HUINT32		tid;
	int			vkError;

	s_pstExtractTable = HLIB_HASH_NewFull(HLIB_HASH_GetStringKey, HLIB_HASH_IsStringEqual, NULL, NULL);

	//	TODO: add thumbnail extracter !!
	THMA_PORT_MEDIA_Init(s_pstExtractTable);
	THMA_PORT_RECORDING_Init(s_pstExtractTable);

	vkError = VK_MSG_Create(512, sizeof(THMA_MSG_t), "thmaex_msg", (unsigned long*)&s_ulExtractQueueId, VK_SUSPENDTYPE_FIFO);
	if (VK_OK != vkError)
	{
		HxLOG_Error ("[%s:%d] VK_MSG_Create err:0x%08x\n", __FUNCTION__, __LINE__, vkError);
	}
	vkError = VK_TASK_Create(thma_port_extractTask, VK_TASK_PRIORITY_MW_REF, SIZE_64K, (void*)"thmaExtract", (void*)NULL, (unsigned long*)&tid, 0);
	if (VK_OK != vkError)
	{
		HxLOG_Error ("[%s:%d] VK_TASK_Create err:0x%08x\n", __FUNCTION__, __LINE__, vkError);
	}
	vkError = VK_TASK_Start(tid);
	if (VK_OK != vkError)
	{
		HxLOG_Error ("[%s:%d] VK_TASK_Start err:0x%08x\n", __FUNCTION__, __LINE__, vkError);
	}
}

void	THMA_PORT_GetData(thma_t *thma, HUINT32 ulRequestId, THMA_Progress_f pfnCallback)
{
	THMA_MSG_t		msg;
	thma_extinfo_t	*job;

	job = (thma_extinfo_t*)TH_MEM_Calloc(sizeof(thma_extinfo_t));
	job->ulRequestId = ulRequestId;
	job->ulTime      = thma->pstCurrentJob->ulTime;
	job->pszURL      = TH_MEM_StrDup(thma->pstCurrentJob->pszURL);
	job->pszCacheURL = TH_MEM_StrDup(thma->pstCurrentJob->pszCacheURL);
	job->bSequential = thma->pstCurrentJob->bSequential;
	job->pfnCallback = pfnCallback;

	msg.eMsg = eTHMA_MSG_EXTRACT;
	msg.ulParam1 = (HUINT32)job;
	msg.ulParam2 = 0;
	msg.ulParam3 = 0;

	VK_MSG_Send(s_ulExtractQueueId, (void*)&msg, sizeof(THMA_MSG_t));
}

/**************************************************************************************************/
#define _________THMA_End_Of_File____________________________________________________
/**************************************************************************************************/

