/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  thapi.c
	@brief

	Description:  								\n
	Module: Thma API Module					\n

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

/**************************************************************************************************/
#define _________THAPI_Private_Include_________________________________________________
/**************************************************************************************************/
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <vkernel.h>
#include <thma_int.h>

/**************************************************************************************************/
#define _________THAPI_Private_Macro_________________________________________________
/**************************************************************************************************/
#define	ENTER_CRITICAL		HxSEMT_Get(s_thapi->ulSemaphore)
#define	LEAVE_CRITICAL		HxSEMT_Release(s_thapi->ulSemaphore)

/**************************************************************************************************/
#define _________THAPI_Private_Struct_________________________________________________
/**************************************************************************************************/
typedef struct
{
	void	*pvUser;
	THAPI_GlobalNotifier	pfnNotifier;
} thapi_gnotifier_t;

typedef struct
{
	HUINT32			ulSemaphore;

	pid_t			stProcPid;
	HINT32			nSocketHandle;
	HINT32			nRpcHandle;

	HUINT32			ulTempSize;
	HxList_t		*pstMountPath;

	thapi_gnotifier_t	stGlobalNotifier;
} thapi_t;

/**************************************************************************************************/
#define _________THAPI_Private_Value_________________________________________________
/**************************************************************************************************/
static thapi_t		*s_thapi = NULL;

/**************************************************************************************************/
#define _________THAPI_Private_Prototype_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________THAPI_Private_Init__________________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________THAPI_Private_Destroy__________________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________THAPI_Private_FUNCTIONS__________________________________________________________
/**************************************************************************************************/
static HCHAR*	thapi_getStringBuffer(HINT32 size)
{
	static HxCBUF_t	s_hBufHandle = NULL;

	if (s_hBufHandle == NULL)
		s_hBufHandle = HLIB_CBUF_New(16384);

	return HLIB_CBUF_GetBuffer(s_hBufHandle, size);
}

static void thapi_connect(thapi_t	*thapi)
{
	thapi->nSocketHandle = HLIB_EXTRA_OpenClient("thma");
	thapi->nRpcHandle    = HLIB_RPC_Open(thapi->nSocketHandle, HLIB_STD_GetPID());
	thapi->stProcPid     = HLIB_STD_GetPID();
}

static void thapi_disconnect(thapi_t    *thapi)
{
    HLIB_RPC_Close(thapi->nRpcHandle);
    HLIB_SOCKET_Close(thapi->nSocketHandle);

    thapi->nRpcHandle    = 0;
    thapi->nSocketHandle = 0;
}

static void 	thapi_socket_changeStatus(void *userData, HxSocket_Status_e eStatus, HINT32 nClientHandle)
{
	thapi_t	*pThapi = (thapi_t*)userData;

	switch (eStatus)
	{
	case HxSOCKET_DISCONNECT_CLIENT:
		HxLOG_Critical("HxSOCKET_DISCONNECT_CLIENT!!!\n");
        thapi_disconnect(pThapi);
		break;
	default:
		break;
	}
}

static void 	thapi_socket_receivePacket (void *userData, HINT32 nSocketHandle, HUINT32 requestId, HINT32 size, void *pvData)
{
	thapi_t	*thapi = (thapi_t*)userData;

	ENTER_CRITICAL;
	HLIB_RPC_ProcessRPC(thapi->nRpcHandle, size, pvData, requestId, nSocketHandle);
	LEAVE_CRITICAL;
}

static void 	thapi_socket_listener (void *arg)
{
	thapi_t *thapi = (thapi_t *)arg;

	while(1)
	{
		if(thapi->nSocketHandle)
		{
         	HLIB_SOCKET_ListenTimeout(thapi->nSocketHandle, 10, thapi_socket_receivePacket, thapi_socket_changeStatus, (void *)thapi);
		}
		else
		{
            thapi_connect(thapi);
			HLIB_STD_TaskSleep(100);
		}
	}
}

static thapi_t*		thapi_getInstance(void)
{
	if (s_thapi == NULL)
	{
		HUINT32		tid;

		s_thapi = (thapi_t*)HLIB_STD_MemCalloc(sizeof(thapi_t));

		HxSEMT_Create(&s_thapi->ulSemaphore, "thapi_sem", 0);

		ENTER_CRITICAL;
        thapi_connect(s_thapi);
		HxLOG_Warning("** (%s) is connected to THMA\n", HLIB_STD_GetCurProcessName());

		VK_TASK_Create(thapi_socket_listener, 70, SIZE_64K, "thapi_listener", (void*)s_thapi, (unsigned long*)&tid, 0);
		VK_TASK_Start(tid);

		LEAVE_CRITICAL;
	}

	return s_thapi;
}

static const HCHAR*	thapi_getDepthPathName(const HCHAR *pszURL)
{
	HINT32	i = THMA_PATH_DEPTH;
	HCHAR	*p;
	HCHAR	*pszDepth = thapi_getStringBuffer(1024);

	HxSTD_StrNCpy(pszDepth, pszURL, 1024);
	while ((p = HxSTD_StrRChar(pszDepth, '/')) && i)
	{
		*p = 0;
		i--;
	}

	HxLOG_Debug("without depth [%d] [%s]\n", THMA_PATH_DEPTH, pszDepth);

	return &pszURL[HxSTD_StrLen(pszDepth)];
}

static void		thapi_replaceChar(HCHAR *pszDest, const HCHAR *pszSource, HINT32 nLen)
{
	const HCHAR	*src;
	HCHAR	*dst, ch;

	dst = pszDest;
	src = pszSource;
	HLIB_STD_StrNCpySafe(dst, src, nLen);

	while (*dst)
	{
		ch = *dst;
		if ((ch >= 0x41 && ch <= 0x5a) || (ch >= 0x61 && ch <= 0x7A) || (ch >= 0x30 && ch <= 0x39))
			dst++;
		else
			*dst++ = (ch & 0x01) ? '-' : '_';
	}
}

typedef struct
{
	HCHAR	szMountPath[256];
	HBOOL	bWritable;
} th_mntinfo_t;

static HBOOL	thapi_findMountPathInList(void *pvURLPath, void *pvMountPath)
{
	HCHAR	*pszURLPath   = (HCHAR*)pvURLPath;
	th_mntinfo_t	*pstMountInfo = (th_mntinfo_t*)pvMountPath;

	if (HLIB_STD_StrStartWith(pszURLPath, pstMountInfo->szMountPath) == TRUE)
		return TRUE;

	return FALSE;
}

static HCHAR*	thapi_findMountPathByURL(thapi_t *thapi, const HCHAR *pszURL)
{
	th_mntinfo_t	*pstMountInfo;
	HCHAR		*pszMountPath;
	HCHAR		*pszMountOpt;
	HxList_t	*list;

	list = HLIB_LIST_FindEx(thapi->pstMountPath, (void*)pszURL, thapi_findMountPathInList);
	if (list)
	{
		pstMountInfo = (th_mntinfo_t*)HLIB_LIST_Data(list);

		if (pstMountInfo->bWritable)
			return pstMountInfo->szMountPath;

		//	TODO: if not writable....
		return "/tmp";
	}

	pszMountPath = thapi_getStringBuffer(1024);
	pszMountOpt  = thapi_getStringBuffer(80);
	if (HLIB_DIR_GetMountPath(pszURL, pszMountPath, 1023) == ERR_OK)
	{
		th_mntinfo_t	*pstMountInfo = HLIB_STD_MemCalloc(sizeof(th_mntinfo_t));

		HLIB_STD_StrNCpySafe(pstMountInfo->szMountPath, pszMountPath, 256);
		if (HLIB_DIR_GetMountOpts(pstMountInfo->szMountPath, pszMountOpt, 80) != ERR_OK)
			pstMountInfo->bWritable = TRUE;
		else
		{
			if (HxSTD_StrStr(pszMountOpt, "rw"))
				pstMountInfo->bWritable = TRUE;
			else
				pstMountInfo->bWritable = FALSE;

		}
		thapi->pstMountPath = HLIB_LIST_Append(thapi->pstMountPath, (void*)pstMountInfo);

		if (pstMountInfo->bWritable)
			return pstMountInfo->szMountPath;

		//	TODO: if not writable....
		return "/tmp";
	}

	return NULL;
}

static HERROR	thapi_makeCacheNameStep(HCHAR *pszResult, HINT32 size, const HCHAR *pszMountPath, const HCHAR *pszURL)
{
	HCHAR	*pszTempName  = (HCHAR*)thapi_getStringBuffer(1024);
	HCHAR	*pszDepthName = (HCHAR*)thapi_getDepthPathName(pszURL);

	thapi_replaceChar(pszTempName, (const HCHAR*)pszDepthName, 1024);
	HxSTD_snprintf(pszResult, size, "%s/.cache/%s.?", pszMountPath, pszTempName);

	return ERR_OK;
}

static HERROR	thapi_makeCacheName(HCHAR *pszResult, HINT32 size, const HCHAR *pszMountPath, const HCHAR *pszURL,HUINT32 playTime,const HCHAR *pszExt)
{
	HCHAR	*pszTempName  = (HCHAR*)thapi_getStringBuffer(1024);
	HCHAR	*pszDepthName = (HCHAR*)thapi_getDepthPathName(pszURL);

	thapi_replaceChar(pszTempName, (const HCHAR*)pszDepthName, 1024);
	HxSTD_snprintf(pszResult, size, "%s/.cache/%s-%d.%s", pszMountPath, pszTempName, playTime,pszExt);

	return ERR_OK;
}

static HERROR	thapi_rpcCallback(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	thapi_t	*thapi = thapi_getInstance();

	if (nArgc != 2)
	{
		HxLOG_Error("error nArgc is [%d]\n", nArgc);
		return ERR_FAIL;
	}

	if (thapi->stGlobalNotifier.pfnNotifier)
		thapi->stGlobalNotifier.pfnNotifier(HxOBJECT_INT(apArgv[0]), HxOBJECT_STR(apArgv[1]), thapi->stGlobalNotifier.pvUser);

	return ERR_OK;
}

/**************************************************************************************************/
#define _________THAPI_Private_Proc_MSG_RUNNIG__________________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________THAPI_Public_Proc____________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________THAPI_Public_Functions__________________________________________________________
/**************************************************************************************************/
void			THAPI_SetGlobalNotifier(void *pvUser, THAPI_GlobalNotifier onDecodingState)
{
	thapi_t		*thapi = thapi_getInstance();

	ENTER_CRITICAL;

	if (thapi->stGlobalNotifier.pfnNotifier == NULL)
	{
		thapi->stGlobalNotifier.pvUser = pvUser;
		thapi->stGlobalNotifier.pfnNotifier = onDecodingState;

		HLIB_RPC_RegisterNotifyCallback(thapi->nRpcHandle, "thma.onDecodingState", "is", thapi, thapi_rpcCallback);
	} else
	{
		HxLOG_Error("Global Notifier is already Registered!!!\n");
	}


	LEAVE_CRITICAL;
}

//	Get LargeThumbnailURL from CacheUrl.
HCHAR*	THAPI_GetLargeThumbnailUrl(HCHAR *pszCacheUrl, HINT32 nLen)
{
	return thma_getLargeThumbnailName(pszCacheUrl, nLen);
}

HERROR			THAPI_CheckCacheBuffer(const HCHAR *pszMountUrl)
{
	thapi_t		*thapi = thapi_getInstance();

	if (HxSTD_StrEmpty(pszMountUrl) == TRUE)
	{
		HxLOG_Error("mount path is NULL\n");
		return ERR_FAIL;
	}

	ENTER_CRITICAL;

	HLIB_RPC_Call(thapi->nRpcHandle, "thma.checkCacheBuffer", "s", pszMountUrl);

	LEAVE_CRITICAL;

	return ERR_OK;
}

//	pszURL : target image file (include mp3...)
//	ulTime : target image start time (for video file)
//	pszMountPath : target mount directory. if null, set data to URL's mounted folder
//	bRetExpectURL : return new url
const HCHAR*	THAPI_GetThumbnailURL(const HCHAR *pszURL, HUINT32 ulTime, const HCHAR *pszMountPath, HBOOL bRetExpectURL)
{
	HCHAR				*pszTempUrl = NULL;
	thapi_t				*thapi = thapi_getInstance();
	static HCHAR		aszPrevUrl[1024] = {0,};

	if (HxSTD_StrEmpty(pszURL))
		return NULL;

	ENTER_CRITICAL;

	if (thapi->nSocketHandle)
	{
		//	pszMountPath가 NULL일 경우, pszURL로 부터 Mounted Path를 가지고 온다.
		//	해당 Mounted Path가 read-only일 경우, /tmp에 해당 thumbnail을 저장한다.
		if (pszMountPath == NULL)
			pszMountPath = thapi_findMountPathByURL(thapi, pszURL);

		if (pszMountPath == NULL)
		{
			HxLOG_Critical("Mount path is not valid!!! [%s]\n", pszURL);
			LEAVE_CRITICAL;
			return NULL;
		}

		pszTempUrl = thapi_getStringBuffer(1024);

		if(pszURL[0] == '/')
		{
			HCHAR	*pszExtName = NULL, szExtName[16];

			pszExtName = HLIB_PATH_Extension(pszURL);
			HxSTD_snprintf(szExtName, 15, "%s", pszExtName);

			if(HLIB_STD_StrNCaseCmp(szExtName, "png", 3) == 0
				|| HLIB_STD_StrNCaseCmp(szExtName, "bmp", 3) == 0
				|| HLIB_STD_StrNCaseCmp(szExtName, "gif", 3) == 0
				)
			{
				HxLOG_Warning("THMA is not yet supported these formats [%s]. file[%s]\n",szExtName, pszURL);
				HLIB_STD_StrNCpySafe(pszTempUrl,pszURL,1024);
				LEAVE_CRITICAL;
				return pszTempUrl;
			}
		}

		thapi_makeCacheName(pszTempUrl, 1024, pszMountPath, pszURL,ulTime ,"jpg");

		//	cache folder에 이미 해당 화일이 존재한다면?
		if (HLIB_FILE_IsValid(pszTempUrl) == ERR_OK)
		{
			LEAVE_CRITICAL;
			return pszTempUrl;
		}

		//	THMA로 Thumbnail생성을 요청한다.
		HLIB_RPC_Call(thapi->nRpcHandle, "thma.getThumbnail", "issii", ulTime, pszURL, pszTempUrl, TRUE, FALSE);
		HLIB_STD_StrNCpySafe(aszPrevUrl,pszTempUrl,1024);
	}
	else
	{
		HxLOG_Critical("THAPI is not initialized yet!!\n");
		HxLOG_Critical("check your server.\n");
	}

	LEAVE_CRITICAL;

	if (bRetExpectURL)
		return pszTempUrl;

	return NULL;
}

HERROR	THAPI_AddThumbnailCache(const HCHAR *pszURL, HUINT32 ulTime, const HCHAR *pszMountPath, HBOOL bUrgent)
{
	HCHAR		*pszTempUrl;
	thapi_t		*thapi = thapi_getInstance();

	if (HxSTD_StrEmpty(pszURL))
		return ERR_FAIL;

	ENTER_CRITICAL;

	if (thapi->nSocketHandle)
	{
		pszTempUrl = thapi_getStringBuffer(1024);
		if (pszMountPath == NULL)
			pszMountPath = thapi_findMountPathByURL(thapi, pszURL);

		if (pszMountPath == NULL)
		{
			HxLOG_Critical("Mount path is not valid [%s]\n", pszURL);
			LEAVE_CRITICAL;
			return ERR_FAIL;
		}
		thapi_makeCacheName(pszTempUrl, 1024, pszMountPath, pszURL,ulTime, "jpg");

		//	cache folder에 이미 해당 화일이 존재한다면?
		if (HLIB_FILE_IsValid(pszTempUrl) == ERR_OK)
		{
			LEAVE_CRITICAL;
			return ERR_OK;
		}
		HLIB_RPC_Call(thapi->nRpcHandle, "thma.getThumbnail", "issii", ulTime, pszURL, pszTempUrl, bUrgent, FALSE);
	} else

	{
		HxLOG_Critical("THAPI is not initialized yet!!\n");
		HxLOG_Critical("check your server.\n");
	}

	LEAVE_CRITICAL;

	return ERR_OK;
}

//	use mount .cache folder directly if target is step thumbnail image.
const HCHAR*	THAPI_MakeStepThumbnail(const HCHAR *pszURL, HUINT32 ulStepTime, const HCHAR *pszMountPath)
{
	HCHAR				*pszTempUrl = NULL;
	thapi_t				*thapi = thapi_getInstance();

	if (HxSTD_StrEmpty(pszURL))
		return NULL;

	ENTER_CRITICAL;

	if (thapi->nSocketHandle)
	{
		pszTempUrl = thapi_getStringBuffer(1024);

		if (pszMountPath == NULL)
			pszMountPath = thapi_findMountPathByURL(thapi, pszURL);

		if (pszMountPath == NULL)
		{
			HxLOG_Critical("Mount path is not valid!!! [%s]\n", pszURL);
			LEAVE_CRITICAL;

			return NULL;
		}

		thapi_makeCacheNameStep(pszTempUrl, 1024, pszMountPath, pszURL);
		HxSTD_StrNCat(pszTempUrl, ".jpg", 1024 - HxSTD_StrLen(pszTempUrl));
		HxLOG_Debug("makeStepThumbnail cache url is [%s]\n", pszTempUrl);
		//	cache folder에 이미 해당 화일이 존재한다면?
		if (HLIB_FILE_IsValid(pszTempUrl) == ERR_OK)
		{
			LEAVE_CRITICAL;
			return pszTempUrl;
		}
		HLIB_RPC_Call(thapi->nRpcHandle, "thma.getThumbnail", "issii", ulStepTime, pszURL, pszTempUrl, TRUE, TRUE);
	} else
	{
		HxLOG_Critical("THAPI is not initialized yet!!\n");
		HxLOG_Critical("check your server.\n");
	}

	LEAVE_CRITICAL;

	return pszTempUrl;
}

void	THAPI_Quit(void)
{
	thapi_t	*thapi = thapi_getInstance();

	ENTER_CRITICAL;

	HLIB_RPC_Call(thapi->nRpcHandle, "thma.cancel", NULL);

	LEAVE_CRITICAL;
}

/**************************************************************************************************/
#define _________THAPI_End_Of_File____________________________________________________
/**************************************************************************************************/

