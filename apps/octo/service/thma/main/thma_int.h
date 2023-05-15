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
#ifndef __THMA_INT_H__
#define __THMA_INT_H__

#ifdef __cplusplus
extern "C" {
#endif


/**************************************************************************************************/
#define _________THMA_Internal_Include_________________________________________________
/**************************************************************************************************/
#include <malloc.h>
#include <stdlib.h>
#include <thapi.h>

//#define	USE_THMA_TMP_CACHE
/**************************************************************************************************/
#define _________THMA_Internal_Macro_________________________________________________
/**************************************************************************************************/
#define TH_MEM_Alloc				HLIB_STD_MemAlloc
#define TH_MEM_Calloc				HLIB_STD_MemCalloc
#define	TH_MEM_Realloc				HLIB_STD_MemRealloc
#define	TH_MEM_StrDup				HLIB_STD_StrDup
#define	TH_MEM_MemDup				HLIB_STD_MemDup
#define TH_MEM_Free					HLIB_STD_MemFree

#define	THMA_PATH_DEPTH		2
#define	THxNAMEOF(type)		#type
//#define	THMA_SIGNAL_UDS		"/tmp/.thma_signal.tmp"
#ifdef	USE_THMA_TMP_CACHE
#define	THMA_TMP_PATH		"/tmp/.cache"
#endif

#define	THMA_KEY_CANCEL		"cancel"
#define	THMA_KEY_URL		"url"
#define	THMA_KEY_CACHE_URL	"cache"
#define	THMA_KEY_TIME		"time"
#define	THMA_KEY_URGENT		"urgent"
#define	THMA_KEY_SEQUENTIAL	"sequential"

#define	THAPI_TEMP_THRESHOLD		(20 * 1024 * 1024)
#define	THAPI_TEMP_THRESHOLD_FIX	(15 * 1024 * 1024)

#define	THMA_THUMBNAIL_WIDTH		354
#define	THMA_THUMBNAIL_HEIGHT		200			//	16:9 aspect ration 354:200 = 16:9

/**************************************************************************************************/
#define _________THMA_Internal_typedef_________________________________________________
/**************************************************************************************************/
typedef enum
{
	eTHAPICMD_REGISTER,
	eTHAPICMD_LOAD
} THMA_CMD_e;

typedef enum
{
	eTHMA_MSG_PACKET,
	eTHMA_MSG_CHECKCACHE,
	eTHMA_MSG_CALLBACK,
	eTHMA_MSG_EXTRACT,

	eTHMA_MSG_CANCEL,
	eTHMA_MSG_UNMOUNTED,
	eTHMA_MSG_FORMAT,
	eTHMA_MSG_SHUTDOWN,

	eTHMA_MSG_END
} THMA_MSG_e;

typedef enum
{
	eTHMA_ITEM_FVC_BASE_URL,
	eTHMA_ITEM_FVC_AUTH_HASH,
	eTHMA_ITEM_FVC_AUTH_TIME_STAMP,

	eTHMA_ITEM_MAX
} THMA_ITEM_e;

//	eTHMA_MSG_PACKET ulParam1
typedef struct
{
	HBOOL	bCanceled;
	HBOOL	bUrgent;
	HBOOL	bSequence;
	HUINT32	ulTime;
	HCHAR	*pszSource;
	HCHAR	*pszDest;
} thma_pktinfo_t;

typedef struct
{
	THMA_MSG_e	eMsg;

	HUINT32		ulParam1;
	HUINT32		ulParam2;
	HUINT32		ulParam3;
} THMA_MSG_t;

typedef struct
{
	HBOOL	bInProgress;
	HBOOL	bUrgent;
	HBOOL	bSequential;
	HINT32	nRetryCount;
	HUINT32	ulConditionTick;
	HUINT32	ulRequestId;
	HUINT32	ulTime;
	HCHAR	*pszURL;
	HCHAR	*pszCacheURL;
} thma_jobinfo_t;

typedef	struct
{
	HUINT32			ulMsgQId;
	HINT32			server;
	HINT32			nRpcHandle;

	HBOOL			bCacheIsChanged;
	HUINT32			ulLastCacheCheckTick;
	HxList_t		*pstMountPath;
	HUINT32			ulRequestId;
	thma_jobinfo_t	*pstCurrentJob;
	HxList_t		*pstJobList;

	HINT32			nRemainJobCount;
	HUINT32			ulLastNotiTick;
} thma_t;

typedef void (*THMA_Progress_f)(HUINT32 ulReqId, HINT32 nProgress);
/**************************************************************************************************/
#define _________THMA_Internal_Value_________________________________________________
/**************************************************************************************************/
typedef struct
{
	HCHAR	*pszExt;
	HCHAR	*pszExtType;
} thma_extpair_t;

static inline
HCHAR*	thma_getLargeThumbnailName(HCHAR *pszUrl, HINT32 nLen)
{
	HCHAR	*pszExt;

	pszExt = HLIB_PATH_Extension(pszUrl);
	if (pszExt)
		HLIB_STD_StrNCpySafe(pszExt, "L.jpg", nLen - HxSTD_StrLen(pszUrl) - 4);

	return pszUrl;
}

static inline
HBOOL	thma_IsLargeThumbnailFile(HCHAR *pszUrl)
{
	return HLIB_STD_StrEndWith(pszUrl, ".L.jpg");
}

/**************************************************************************************************/
#define _________THMA_Internal_Functions__________________________________________________________
/**************************************************************************************************/
extern void	THMA_PORT_Init(void);
extern void	THMA_PORT_GetData(thma_t *thma, HUINT32 ulRequestId, THMA_Progress_f pfnCallback);

extern HERROR	THMA_PORT_MEDIA_Init(HxHASH_t *pstHashTable);
extern HERROR	THMA_PORT_RECORDING_Init(HxHASH_t *pstHashTable);

extern void		THMA_ITEM_InitDataManager(void);
extern void		THMA_ITEM_SetItem (THMA_ITEM_e eItem, void *pData);
extern void 	*THMA_ITEM_GetItemPoint (THMA_ITEM_e eItem);
extern HUINT32	THMA_ITEM_GetItemValue(THMA_ITEM_e eItem);


#ifdef	__cplusplus
}
#endif

#endif //end of __THMA_INT_H__

