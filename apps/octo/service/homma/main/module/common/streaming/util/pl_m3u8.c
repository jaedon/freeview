/**************************************************************
*	@file		hxm3u8.c
*
*	http://www.humaxdigital.com
*
*	@data			2012/10/24
*	@author			humax
**************************************************************/


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
* ⓒ 2011-2012 Humax Co., Ltd.
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


/**********************************************************************************************************/
#define ____INCLUDE_______________________
/**********************************************************************************************************/
#include "../int_streaming.h"



/**********************************************************************************************************/
#define ____INTERNAL_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/
#define	LOG				HxLOG_Print
#define	ERR_LOG			HxLOG_Error

/* memory control */
#define HxM3U8_MemAlloc				HLIB_STD_MemAlloc
#define HxM3U8_MemFree				HLIB_STD_MemFree
#define HxM3U8_Memset				HxSTD_MemSet
#define HxM3U8_Memcpy				HxSTD_MemCopy

#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

#ifndef NULL
#define NULL					0
#endif

#ifndef FALSE
#define FALSE					0
#endif

#ifndef TRUE
#define TRUE					1
#endif

/*
 * 참조 uri
   apple : http://developer.apple.com/library/ios/#documentation/networkinginternet/conceptual/streamingmediaguide/UsingHTTPLiveStreaming/UsingHTTPLiveStreaming.html#//apple_ref/doc/uid/TP40008332-CH102-SW1
              + refer profile : https://developer.apple.com/library/ios/#technotes/tn2224/_index.html
   android : https://developers.google.com/tv/android/articles/hls?hl=ko
   hello  : http://helloworld.naver.com/helloworld/7122
   hls ietf : http://tools.ietf.org/html/draft-pantos-http-live-streaming-10

 * segmenter
   http://tezina.tistory.com/tag/segmenter.c
   http://www.gdo.kr/post/4184/


 * test uri
 * http://devimages.apple.com/iphone/samples/bipbop/bipbopall.m3u8
 */


#define HxM3U8_EXTM3U						"#EXTM3U"					/* #EXTM3U
																			파일의 가장 첫 줄에 명시하여 파일이 m3u8 포맷임을 명시한다. */
#define HxM3U8_EXT_X_VERSION  				"#EXT-X-VERSION:3"

#define HxM3U8_EXT_X_PLAYLIST_TYPE			"#EXT-X-PLAYLIST-TYPE:EVENT"

#define HxM3U8_EXT_X_ALLOW_CACHE_NO			"#EXT-X-ALLOW-CACHE:NO"



#define HxM3U8_EXTINF						"#EXTINF:"					/* #EXTINF: <재생 시간:초>,<제목>
																			이 지시어의 다음에 명시된 콘텐츠의 재생 시간과 제목을 명시한다. */
#define HxM3U8_EXT_X_TARGETDURATION			"#EXT-X-TARGETDURATION:"	/* #EXT-X-TARGETDURATION: <시간: 초>
																			파일 목록에 나열된 각 파일의 최대 재생 시간을 명시한다. */
#define HxM3U8_EXT_X_ENDLIST				"#EXT-X-ENDLIST"			/* #EXT-X-ENDLIST
																			플레이 리스트에서 재생할 콘텐츠가 더 이상 없음을 의미한다.
																			이 지시어가 표시된 줄 이후의 콘텐츠는 무시한다. */
/*
	#EXT-X-STREAM-INF
	이 지시어 다음 줄의 콘텐츠에 대한 정보를 제공한다.#EXTINF는 재생 시간에 대한 정보만 제공하고, #EXT-X-STREAM-INF는 다음과 같은 정보를 제공한다.
	 - BANDWIDTH: 10진수로 표시한 bps 값
	 - PROGRAM-ID: 플레이 리스트 파일에 있는 콘텐츠가 갖는 고유 값
	 - CODEC: 해당 콘텐츠에 적용된 코텍(codec) 정보
	 - RESOLUTION: 해상도
*/
#define HxM3U8_EXT_X_STREAM_INF				"#EXT-X-STREAM-INF:"
#define HxM3U8_EXT_X_STREAM_INF_PROGRAM		"PROGRAM-ID="
#define HxM3U8_EXT_X_STREAM_INF_BANDWIDTH	"BANDWIDTH="
#define HxM3U8_EXT_X_STREAM_INF_CODEC		"CODEC="
#define HxM3U8_EXT_X_STREAM_INF_RESOLUTION	"RESOLUTION="


#define HxM3U8_EXT_X_DISCONTINUITY			"#EXT-X-DISCONTINUITY"		/* #EXT-X-DISCONTINUITY
																			이 지시어가 표지된 줄을 기준으로 이전 줄과 이후 줄에서 재생하는
																			콘텐츠의 정보가 변경되었음을 표시한다.
																			예를 들어 이전 콘텐츠와 이후 콘텐츠의 파일 포맷, 파일이 갖고 있는 미디어 트랙의 개수,
																			인코딩 정보, 재생 시간 정보 등이 변경되면 이 지시어를 플레이리스트에서
																			정보가 바뀌는 파일 사이에 명시하여 플레이어가 새로운 정보를 사용해야 하는 시점을 알려 준다.. */
#define HxM3U8_EXT_X_MEDIA_SEQUENCE			"#EXT-X-MEDIA-SEQUENCE:"		/* #EXT-X-MEDIA-SEQUENCE: <첫 파일의 일련번호>
																			제일 먼저 플레이해야하는 파일의 일련번호를 명시한다.
																			예를 들어 0,1,2의 파일이 있을 경우 이 지시어의 값은 0이 된다.
																			이 지시어가 포함되지 않은 경우 첫 분할 파일의 일련 번호는 0으로 간주한다. */
#define HxM3U8_EXT_X_KEY					"#EXT-X-KEY"				/* #EXT-X-KEY: <암호화 방법>[, <key>]
																			암호화된 파일을 해독하는 키 값을 명시한다.
																			HTTP Live Streaming에서는 재생 시간에 따라 분할된 각 파일을 암호화하여 전송할 수 있다.
																			암호화된 파일을 해독할 때 필요한 키 값을 플레이어에게 알려 주기 위해 사용한다. */


#define HxM3U8_M3U8_NAME_TEMPLATE			"%s_%dk.m3u8"
#define HxM3U8_TS_NAME_TEMPLATE				"%s_%dk_%08d.ts"

struct _PLM3U8
{
	HxList_t	*item; /* item : MxSTR_t */
	HINT32		max_size;  /* queue size */
	HINT32		cur_size;
};

typedef struct _HxM3U8_toBuffer
{
	HINT32 	len;
	HUINT8 	*buf;
} HxM3U8_toBuffer_t;

typedef enum _eHxM3U8_Kindof
{
	eHxM3U8_Kindof_Item 	= 0,
	eHxM3U8_Kindof_TS,
	eHxM3U8_Kindof_M3U8,
	eHxM3U8_Kindof_String,
	eHxM3U8_Kindof_Integer,

} ePL_M3U8_Kindof_t;


STATIC HCHAR *s_szM3U8_ExtTag[]  =
{
	HxM3U8_EXTM3U,
	HxM3U8_EXT_X_VERSION,
	HxM3U8_EXT_X_PLAYLIST_TYPE,
	HxM3U8_EXT_X_ALLOW_CACHE_NO,
	HxM3U8_EXTINF,
	HxM3U8_EXT_X_TARGETDURATION,
	HxM3U8_EXT_X_ENDLIST,
	HxM3U8_EXT_X_STREAM_INF,
	HxM3U8_EXT_X_DISCONTINUITY,
	HxM3U8_EXT_X_MEDIA_SEQUENCE,
	HxM3U8_EXT_X_KEY,
	NULL,
};



/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/
STATIC void _plm3u8_DeleteListItem(void *data)
{
	HLIB_STR_Delete((HxSTR_t *)data);
}

STATIC HBOOL _plm3u8_FindListItem(void *userdata, void *data)
{
	HxSTR_t *pStr = (HxSTR_t *)data;

	if(0 != HLIB_STR_GetUserTag(pStr))
	{
		return TRUE;
	}

	return FALSE;
}


STATIC HxSTR_t	*_plm3u8_FindExtTag(HxSTR_t *pStr)
{
	HINT32	i = 0;
	HINT32	nTagLen = 0;

	for(i=0; s_szM3U8_ExtTag[i] != NULL; i++)
	{
		if(s_szM3U8_ExtTag[i][4] != HLIB_STR_GetAtBuffer(pStr, 4))
		{
			continue;
		}

		if(s_szM3U8_ExtTag[i][7] != HLIB_STR_GetAtBuffer(pStr, 7))
		{
			continue;
		}

		nTagLen = HxSTD_StrLen(s_szM3U8_ExtTag[i]);
		if(nTagLen > HLIB_STR_GetNumOfBytes(pStr))
		{
			continue;
		}

		if(HLIB_STR_CompareN(pStr, s_szM3U8_ExtTag[i], nTagLen) == 0)
		{
			return HLIB_STR_New(s_szM3U8_ExtTag[i]);
		}
	}

	return NULL;
}


STATIC HERROR	_plm3u8_GetListItem(PL_M3U8_t *pSelf, ePL_M3U8_Kindof_t eKindOfM3U8, const HxSTR_t *current, HxSTR_t **ret_str )
{
	HxList_t	*item = NULL;
	HxSTR_t		*str = NULL;
	ePL_M3U8_Kindof_t str_kind = eHxM3U8_Kindof_Item;

	if(ret_str == NULL)
	{
		return ERR_FAIL;
	}

	item = pSelf->item;
	if(current != NULL)
	{
		while(item)
		{
			str = HLIB_LIST_Data(item);
			if(str == current)
			{
				item = HLIB_LIST_Next(item);
				break;
			}
			item = HLIB_LIST_Next(item);
		}
	}

	while(item)
	{
		str = HLIB_LIST_Data(item);
		if(str)
		{
			 str_kind = (ePL_M3U8_Kindof_t)HLIB_STR_GetUserTag(str);
			 if(str_kind == eKindOfM3U8)
			 {
			 	*ret_str = str;
				break;
			 }
		}
		item = HLIB_LIST_Next(item);
	}

	return ERR_OK;
}

/*
STATIC HBOOL _plm3u8_RemoveDuplicationM3U8Tag(void *userdata, void *data)
{
	HxSTR_t *pStr = (HxSTR_t *)data;
	HxSTR_t *pFindStr = (HxSTR_t *)userdata;

	if(pStr == NULL)
	{
		return FALSE;
	}

	if(HLIB_STR_GetUserTag(pFindStr) != HLIB_STR_GetUserTag(pStr))
	{
		return FALSE;
	}

	if(HLIB_STR_CompareNHxSTR(pStr, pFindStr, HLIB_STR_GetNumOfBytes(pFindStr)) == 0)
	{
		return TRUE;
	}

	return FALSE;
}
*/
STATIC HxList_t *_plm3u8_RemoveDuplicationM3U8Tag(PL_M3U8_t *pSelf, HxList_t *deleteItem, HxSTR_t *pOnlyTag)
{
	HxList_t *tmp = (pSelf->item);
	HxSTR_t *pStr = NULL;

	while(tmp)
	{
		if(tmp == deleteItem)
		{
			return NULL;
		}
		pStr = HLIB_LIST_Data(tmp);

		if(HLIB_STR_GetUserTag(pOnlyTag) == HLIB_STR_GetUserTag(pStr))
		{
			if(HLIB_STR_CompareNHxSTR(pStr, pOnlyTag, HLIB_STR_GetNumOfBytes(pOnlyTag)) == 0)
			{
				return tmp;
			}
		}

		tmp = HLIB_LIST_Next(tmp);
	}

	return tmp;
}


STATIC void _plm3u8_FindDuplicationM3U8Tag(PL_M3U8_t *pSelf, HxList_t *deleteItem)
{
	HxList_t *tmp = HLIB_LIST_Next(deleteItem);
	HxList_t *dupTag = NULL;
	HxSTR_t *pStr = NULL;
	HxSTR_t	*onlyTag = NULL;
	HxSTR_t *deleteStr = NULL;

	while(tmp)
	{
		pStr = HLIB_LIST_Data(tmp);

		if(HLIB_STR_GetUserTag(pStr) != eHxM3U8_Kindof_Item)
		{
			break;
		}

		onlyTag = _plm3u8_FindExtTag(pStr);
		if(onlyTag == NULL)
		{
			break;
		}

		dupTag = _plm3u8_RemoveDuplicationM3U8Tag(pSelf, deleteItem, onlyTag);
		if(dupTag)
		{
			deleteStr = HLIB_LIST_Data(dupTag);
			pSelf->item = HLIB_LIST_Remove(pSelf->item, deleteStr);
			HLIB_STR_Delete(deleteStr);
		}
		HLIB_STR_Delete(onlyTag);
		onlyTag = NULL;

		tmp = HLIB_LIST_Next(tmp);
	}
}


STATIC void		_plm3u8_DeleteOldItem(PL_M3U8_t *pSelf)
{
	HxSTR_t	*pStr = NULL;
	HxList_t *deleteItem = NULL;

	deleteItem = HLIB_LIST_FindEx(pSelf->item, NULL, _plm3u8_FindListItem);
	if(deleteItem == NULL)
	{
		return;
	}

	_plm3u8_FindDuplicationM3U8Tag(pSelf, deleteItem);

//	HLIB_LIST_FindEx(HLIB_LIST_Next(deleteItem), HLIB_LIST_Data(nextFirstItem), _plm3u8_RemoveDuplicationM3U8Tag);

	pStr = HLIB_LIST_Data(deleteItem);
	pSelf->item = HLIB_LIST_Remove(pSelf->item, pStr);
	_plm3u8_DeleteListItem(pStr);
	pSelf->cur_size--;

}


STATIC HxSTR_t 	*_plm3u8_AppendItem_String(HxSTR_t *pStr, HBOOL *isNextItem, HCHAR *tag_name, HCHAR *string)
{
	if(string == NULL)
	{
		return pStr;
	}

	if(*isNextItem)
	{
		pStr = HLIB_STR_Append(pStr, ",");
	}
	if(tag_name)
	{
		pStr = HLIB_STR_Append(pStr, tag_name);
	}
	pStr = HLIB_STR_Append(pStr, string);
	*isNextItem = TRUE;

	return pStr;

}

STATIC HxSTR_t 	*_plm3u8_AppendItem_Inteager(HxSTR_t *pStr, HBOOL *isNextItem, HCHAR *tag_name, HINT32 inteager)
{
	if(inteager < 0)
	{
		return pStr;
	}

	if(*isNextItem)
	{
		pStr = HLIB_STR_Append(pStr, ",");
	}
	if(tag_name)
	{
		pStr = HLIB_STR_Append(pStr, tag_name);
	}
	pStr = HLIB_STR_AppendFormat(pStr, "%d", inteager);
	*isNextItem = TRUE;

	return pStr;
}


STATIC HBOOL _plm3u8_GetItemSize(void *data, void *userdata)
{
	HxSTR_t *pStr = (HxSTR_t *)data;
	HINT32	*pSize = userdata;

	*pSize += HLIB_STR_GetLength(pStr);
	*pSize += 2; /* /r/n 공간 */

	return FALSE;
}


STATIC HBOOL	_plm3u8_toBuffer(void *data, void *userdata)
{
	HxSTR_t *pStr 	= (HxSTR_t *)data;
	HINT32	len 	= HLIB_STR_GetLength(pStr);

	HxM3U8_toBuffer_t	*pBuf = (HxM3U8_toBuffer_t *)userdata;

	HxSTD_MemCopy((pBuf->buf + pBuf->len), HLIB_STR_toUTF8(pStr), len);
	pBuf->len += len;
	HxSTD_MemCopy((pBuf->buf + pBuf->len), "\r\n", 2);
	pBuf->len += 2;

	return FALSE;
}

STATIC HBOOL	_plm3u8_toFile(void *data, void *userdata)
{
	HxSTR_t *pStr 	= (HxSTR_t *)data;
	HINT32	len 	= HLIB_STR_GetLength(pStr);
	HxFILE_t hFile  = (HxFILE_t)userdata;


	HLIB_FILE_Write(hFile, HLIB_STR_toUTF8(pStr), len, 1);
	HLIB_FILE_Write(hFile, "\r\n", 2, 1);

	return FALSE;
}



/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/

/**
* #EXTM3U
* 파일의 가장 첫 줄에 명시하여 파일이 m3u8 포맷임을 명시한다.
*
*
* @param
* @return
*/
PL_M3U8_t		*PL_M3U8_New(HINT32 queueSize)
{
	PL_M3U8_t *pSelf = NULL;

	pSelf = HxM3U8_MemAlloc(sizeof(PL_M3U8_t));
	HxLOG_Assert(pSelf);

	HxM3U8_Memset(pSelf, 0, sizeof(PL_M3U8_t));

	pSelf->item = HLIB_LIST_Append(pSelf->item, HLIB_STR_New(HxM3U8_EXTM3U));
	pSelf->item = HLIB_LIST_Append(pSelf->item, HLIB_STR_New(HxM3U8_EXT_X_VERSION));
	pSelf->max_size = queueSize;

	return pSelf;
}


/**
*
*
*
* @param
* @return
*/
void	PL_M3U8_Delete(PL_M3U8_t *pSelf)
{
	if(pSelf)
	{
		pSelf->item = HLIB_LIST_RemoveAllFunc(pSelf->item, _plm3u8_DeleteListItem);
		HLIB_STD_MemFree(pSelf);
		pSelf = NULL;
	}
}


/**
*
*
*
* @param
* @return
*/
void	PL_M3U8_Clear(PL_M3U8_t *pSelf)
{
	if(pSelf)
	{
		pSelf->item = HLIB_LIST_RemoveAllFunc(pSelf->item, _plm3u8_DeleteListItem);
		pSelf->item = NULL;
		pSelf->cur_size = 0;


	 	pSelf->item = HLIB_LIST_Append(pSelf->item, HLIB_STR_New(HxM3U8_EXTM3U));
		pSelf->item = HLIB_LIST_Append(pSelf->item, HLIB_STR_New(HxM3U8_EXT_X_VERSION));
	}
}



/**
* #EXTINF: <재생 시간:초>,<제목>
* 이 지시어의 다음에 명시된 콘텐츠의 재생 시간과 제목을 명시한다.
*
*
* @param
* @return
*/
HERROR	PL_M3U8_AppendInf(PL_M3U8_t *pSelf, HINT32 second, HCHAR *title)
{
	HxSTR_t *pStr = NULL;
	HBOOL	isNextItem = FALSE;

	HxLOG_Assert(pSelf);

	pStr = HLIB_STR_New(HxM3U8_EXTINF);
	pStr = _plm3u8_AppendItem_Inteager(pStr, &isNextItem, NULL, second);
	pStr = _plm3u8_AppendItem_String(pStr, &isNextItem, NULL, title);

	if (!title) {
		pStr = HLIB_STR_Append(pStr, ",");
	}

	pSelf->item = HLIB_LIST_Append(pSelf->item, pStr);

	return ERR_OK;
}


/**
* #EXT-X-TARGETDURATION: <시간: 초>
* 파일 목록에 나열된 각 파일의 최대 재생 시간을 명시한다.
*
*
* @param
* @return
*/
HERROR	PL_M3U8_AppendTargetDuration(PL_M3U8_t *pSelf, HINT32 second)
{
	HxSTR_t *pStr = NULL;
	HBOOL	isNextItem = FALSE;

	HxLOG_Assert(pSelf);

	pStr = HLIB_STR_New(HxM3U8_EXT_X_TARGETDURATION);
	pStr = _plm3u8_AppendItem_Inteager(pStr, &isNextItem, NULL, second);

	pSelf->item = HLIB_LIST_Append(pSelf->item, pStr);

	return ERR_OK;
}


/**
* #EXT-X-STREAM-INF
* 이 지시어 다음 줄의 콘텐츠에 대한 정보를 제공한다.#EXTINF는 재생 시간에 대한 정보만 제공하고, #EXT-X-STREAM-INF는 다음과 같은 정보를 제공한다.
*  - BANDWIDTH: 10진수로 표시한 bps 값
*  - PROGRAM-ID: 플레이 리스트 파일에 있는 콘텐츠가 갖는 고유 값
*  - CODEC: 해당 콘텐츠에 적용된 코텍(codec) 정보
*  - RESOLUTION: 해상도
*
*
* @param
* @return
*/
HERROR	PL_M3U8_AppendStreamInf(PL_M3U8_t *pSelf, HINT32 program_id, HINT32 bandwidth,  HCHAR *codec, HCHAR *resoultion)
{
	HxSTR_t *pStr = NULL;
	HBOOL	isNextItem = FALSE;

	HxLOG_Assert(pSelf);

	pStr = HLIB_STR_New(HxM3U8_EXT_X_STREAM_INF);
	pStr = _plm3u8_AppendItem_Inteager(pStr, &isNextItem, HxM3U8_EXT_X_STREAM_INF_PROGRAM, program_id);
	pStr = _plm3u8_AppendItem_Inteager(pStr, &isNextItem, HxM3U8_EXT_X_STREAM_INF_BANDWIDTH, bandwidth);
	pStr = _plm3u8_AppendItem_String(pStr, &isNextItem, HxM3U8_EXT_X_STREAM_INF_CODEC, codec);
	pStr = _plm3u8_AppendItem_String(pStr, &isNextItem, HxM3U8_EXT_X_STREAM_INF_RESOLUTION, resoultion);

	pSelf->item = HLIB_LIST_Append(pSelf->item, pStr);

	return ERR_OK;
}


/**
* #EXT-X-DISCONTINUITY
* 이 지시어가 표지된 줄을 기준으로 이전 줄과 이후 줄에서 재생하는
* 콘텐츠의 정보가 변경되었음을 표시한다.
* 예를 들어 이전 콘텐츠와 이후 콘텐츠의 파일 포맷, 파일이 갖고 있는 미디어 트랙의 개수,
* 인코딩 정보, 재생 시간 정보 등이 변경되면 이 지시어를 플레이리스트에서
* 정보가 바뀌는 파일 사이에 명시하여 플레이어가 새로운 정보를 사용해야 하는 시점을 알려 준다..
*
*
* @param
* @return
*/
HERROR	PL_M3U8_AppendDiscontinuity(PL_M3U8_t *pSelf)
{
	HxSTR_t *pStr = NULL;

	HxLOG_Assert(pSelf);

	pStr = HLIB_STR_New(HxM3U8_EXT_X_DISCONTINUITY);

	pSelf->item = HLIB_LIST_Append(pSelf->item, pStr);

	return ERR_OK;
}

/**
*
*
* @param
* @return
*/
HERROR	PL_M3U8_AppendAllowCacheNo(PL_M3U8_t *pSelf)
{
	HxSTR_t *pStr = NULL;

	HxLOG_Assert(pSelf);

	pStr = HLIB_STR_New(HxM3U8_EXT_X_ALLOW_CACHE_NO);

	pSelf->item = HLIB_LIST_Append(pSelf->item, pStr);

	return ERR_OK;
}

/**
* #EXT-X-MEDIA-SEQUENCE: <첫 파일의 일련번호>
* 제일 먼저 플레이해야하는 파일의 일련번호를 명시한다.
* 예를 들어 0,1,2의 파일이 있을 경우 이 지시어의 값은 0이 된다.
* 이 지시어가 포함되지 않은 경우 첫 분할 파일의 일련 번호는 0으로 간주한다.
*
*
* @param
* @return
*/
HERROR	PL_M3U8_AppendMediaSequence(PL_M3U8_t *pSelf, HINT32 start_file_number)
{
	HxSTR_t *pStr = NULL;
	HBOOL	isNextItem = FALSE;

	HxLOG_Assert(pSelf);

	pStr = HLIB_STR_New(HxM3U8_EXT_X_MEDIA_SEQUENCE);
	pStr = _plm3u8_AppendItem_Inteager(pStr, &isNextItem, NULL, start_file_number);


	pSelf->item = HLIB_LIST_Append(pSelf->item, pStr);

	return ERR_OK;
}


/**
*
*
*
*
* @param
* @return
*/
HERROR	PL_M3U8_AppendEvent(PL_M3U8_t *pSelf)
{
	HxSTR_t *pStr = NULL;

	HxLOG_Assert(pSelf);

	pStr = HLIB_STR_New(HxM3U8_EXT_X_PLAYLIST_TYPE);

	pSelf->item = HLIB_LIST_Append(pSelf->item, pStr);

	return ERR_OK;
}



/**
* #EXT-X-KEY: <암호화 방법>[, <key>]
* 암호화된 파일을 해독하는 키 값을 명시한다.
* HTTP Live Streaming에서는 재생 시간에 따라 분할된 각 파일을 암호화하여 전송할 수 있다.
* 암호화된 파일을 해독할 때 필요한 키 값을 플레이어에게 알려 주기 위해 사용한다.
*
*
* @param
* @return
*/
HERROR	PL_M3U8_AppendKey(PL_M3U8_t *pSelf, HCHAR *kindof_encrypt, HCHAR *key)
{
	HxSTR_t *pStr = NULL;
	HBOOL	isNextItem = FALSE;

	HxLOG_Assert(pSelf);

	pStr = HLIB_STR_New(HxM3U8_EXT_X_KEY);
	pStr = _plm3u8_AppendItem_String(pStr, &isNextItem, NULL, kindof_encrypt);
	pStr = _plm3u8_AppendItem_String(pStr, &isNextItem, NULL, key);

	pSelf->item = HLIB_LIST_Append(pSelf->item, pStr);

	return ERR_OK;
}


/**
*
*
*
* @param
* @return
*/
HERROR	PL_M3U8_AppendString(PL_M3U8_t *pSelf, HCHAR *string)
{
	HxSTR_t *pStr = NULL;

	HxLOG_Assert(pSelf);

	if(pSelf->max_size <= pSelf->cur_size)
	{
		_plm3u8_DeleteOldItem(pSelf);
	}

	pStr = HLIB_STR_New(string);
	HLIB_STR_SetUserTag(pStr, (HINT32)eHxM3U8_Kindof_String);

	pSelf->item = HLIB_LIST_Append(pSelf->item, pStr);
	pSelf->cur_size++;

	return ERR_OK;
}


/**
*
*
*
* @param
* @return
*/
HERROR	PL_M3U8_AppendHxSTR(PL_M3U8_t *pSelf, HxSTR_t *string)
{
	HxSTR_t *pStr = NULL;

	HxLOG_Assert(pSelf);

	if(pSelf->max_size <= pSelf->cur_size)
	{
		_plm3u8_DeleteOldItem(pSelf);
	}

	pStr = HLIB_STR_Clone(string);
	HLIB_STR_SetUserTag(pStr, (HINT32)eHxM3U8_Kindof_String);

	pSelf->item = HLIB_LIST_Append(pSelf->item, pStr);
	pSelf->cur_size++;

	return ERR_OK;
}



/**
*
*
*
* @param
* @return
*/
HERROR	PL_M3U8_AppendInteger(PL_M3U8_t *pSelf, HINT32 integer)
{
	HxSTR_t *pStr = NULL;

	HxLOG_Assert(pSelf);

	if(pSelf->max_size <= pSelf->cur_size)
	{
		_plm3u8_DeleteOldItem(pSelf);
	}

	pStr = HLIB_STR_NewFormat("%d", integer);
	HLIB_STR_SetUserTag(pStr, (HINT32)eHxM3U8_Kindof_Integer);

	pSelf->item = HLIB_LIST_Append(pSelf->item, pStr);
	pSelf->cur_size++;

	return ERR_OK;
}


/**
*
*
*
* @param
* @return
*/
HERROR	PL_M3U8_AppendM3U8(PL_M3U8_t *pSelf, HCHAR *name, HINT32 bandwidth_k)
{
	HxSTR_t *pStr = NULL;

	HxLOG_Assert(pSelf);

	if(pSelf->max_size <= pSelf->cur_size)
	{
		_plm3u8_DeleteOldItem(pSelf);
	}

	pStr = HLIB_STR_NewFormat(HxM3U8_M3U8_NAME_TEMPLATE, name, bandwidth_k);
	HLIB_STR_SetUserTag(pStr, (HINT32)eHxM3U8_Kindof_M3U8);

	pSelf->item = HLIB_LIST_Append(pSelf->item, pStr);
	pSelf->cur_size++;

	return ERR_OK;
}


/**
*
*
*
* @param
* @return
*/
HERROR	PL_M3U8_AppendTs(PL_M3U8_t *pSelf, HCHAR *name, HINT32 bandwidth_k, HINT32 index)
{
	HxSTR_t *pStr = NULL;

	HxLOG_Assert(pSelf);

	if(pSelf->max_size <= pSelf->cur_size)
	{
		_plm3u8_DeleteOldItem(pSelf);
	}

	pStr = HLIB_STR_NewFormat(HxM3U8_TS_NAME_TEMPLATE, name, bandwidth_k, index);
	HLIB_STR_SetUserTag(pStr, (HINT32)eHxM3U8_Kindof_TS);

	pSelf->item = HLIB_LIST_Append(pSelf->item, pStr);
	pSelf->cur_size++;

	return ERR_OK;
}


/**
*
*
*
*
* @param
* @return
*/
HERROR	PL_M3U8_AppendInfWithTS(PL_M3U8_t *pSelf, HINT32 inf_second, HCHAR *inf_title, HCHAR *ts_name, HINT32 ts_bandwidth_k, HINT32 ts_startindex, HINT32 ts_endindex)
{
	HxSTR_t *pStr = NULL;
	HxSTR_t	*pInfStr = NULL;
	HINT32	i = 0;
	HINT32	len = 0;
	HINT32	max_length = 0;
	HCHAR	*buf = NULL;
	HCHAR	*alloced = NULL;
	HBOOL	isNextItem = FALSE;

	HCHAR	*tempBuf = NULL;
	HCHAR	*szUtfStr = NULL;
	HINT32	nUtfStrLen = 0;

	HxLOG_Assert(pSelf);

	max_length = (sizeof(HCHAR) * (ts_endindex - ts_startindex) * 400);
	if(0 >= max_length)
	{
		HxLOG_Warning("ts_endindex (%d) - ts_startindex (%d) --> 0 (waiting next...)\n", ts_endindex, ts_startindex);
		return ERR_FAIL;
	}

	pInfStr = HLIB_STR_New(HxM3U8_EXTINF);
	pInfStr = _plm3u8_AppendItem_Inteager(pInfStr, &isNextItem, NULL, inf_second);
	pInfStr = _plm3u8_AppendItem_String(pInfStr, &isNextItem, NULL, inf_title);
	if (!inf_title) {
		pInfStr = HLIB_STR_Append(pInfStr, ",");
	}

	alloced = buf = HLIB_STD_MemCalloc(max_length);
	tempBuf = HLIB_STD_MemCalloc(max_length);
	HxLOG_Assert(alloced);

	HxLOG_Debug("* ts_endindex   = %d\n", ts_endindex);
	HxLOG_Debug("* ts_startindex = %d\n", ts_startindex);
	HxLOG_Debug("* max_length    = %d\n", max_length);

	for(i=ts_startindex; i < ts_endindex; i++)
	{
		szUtfStr = HLIB_STR_toUTF8(pInfStr);
		nUtfStrLen = HxSTD_StrLen(szUtfStr);
		HxSTD_StrNCpy(buf, szUtfStr, nUtfStrLen);
		max_length -= HLIB_STR_GetLength(pInfStr);
		buf += HLIB_STR_GetLength(pInfStr);

		if(max_length < 3) break;

		HxSTD_StrNCpy(buf, "\r\n", 2);
		max_length -= 2;
		buf += 2;

		if(max_length < 1) break;

		HxSTD_snprintf(tempBuf, max_length, HxM3U8_TS_NAME_TEMPLATE, ts_name, ts_bandwidth_k, i);
		len = HxSTD_StrLen(tempBuf);
		HxSTD_StrNCpy(buf, tempBuf, len);
		max_length -= len;
		buf += len;

		if(max_length < 3) break;

		HxSTD_StrNCpy(buf, "\r\n", 2);
		max_length -= 2;
		buf += 2;

		// can't reach to this code
		//if(max_length < 1) break;
	}
	*buf = 0;

	HLIB_STR_Delete(pInfStr);

	pStr = HLIB_STR_New(alloced);
	pSelf->item = HLIB_LIST_Append(pSelf->item, pStr);

	HLIB_STD_MemFree(alloced);
	if(tempBuf)	HLIB_STD_MemFree(tempBuf);

	return ERR_OK;
}

/**
* #EXT-X-ENDLIST
* 플레이 리스트에서 재생할 콘텐츠가 더 이상 없음을 의미한다.
* 이 지시어가 표시된 줄 이후의 콘텐츠는 무시한다.
*
*
* @param
* @return
*/
HERROR	PL_M3U8_AppendEndList(PL_M3U8_t *pSelf)
{
	HxSTR_t *pStr = NULL;

	HxLOG_Assert(pSelf);

	pStr = HLIB_STR_New(HxM3U8_EXT_X_ENDLIST);

	pSelf->item = HLIB_LIST_Append(pSelf->item, pStr);

	return ERR_OK;
}



/**
*
*
*
* @param
* @return
*/
HERROR	PL_M3U8_GetSize(PL_M3U8_t *pSelf, HINT32 *pSize)
{
	HINT32 size = 0;

	if(pSize == NULL)
	{
		return ERR_FAIL;
	}

	HLIB_LIST_ForeachEx(pSelf->item, _plm3u8_GetItemSize, &size);
	*pSize = size;

	return ERR_OK;
}


/**
*
*
*
* @param
* @return
*/
HERROR	PL_M3U8_GetFirstM3U8(PL_M3U8_t *pSelf, HxSTR_t **ret_str)
{
	return _plm3u8_GetListItem(pSelf, eHxM3U8_Kindof_M3U8, NULL, ret_str);
}


/**
*
*
*
* @param
* @return
*/
HERROR	PL_M3U8_GetNextM3U8(PL_M3U8_t *pSelf, const HxSTR_t *cur_str, HxSTR_t **ret_str)
{
	return _plm3u8_GetListItem(pSelf, eHxM3U8_Kindof_M3U8, cur_str, ret_str);
}


/**
*
*
*
* @param
* @return
*/
HERROR	PL_M3U8_GetFirstTS(PL_M3U8_t *pSelf, HxSTR_t **ret_str)
{
	return _plm3u8_GetListItem(pSelf, eHxM3U8_Kindof_TS, NULL, ret_str);
}


/**
*
*
*
* @param
* @return
*/
HERROR	PL_M3U8_GetNextTS(PL_M3U8_t *pSelf, const HxSTR_t *cur_str, HxSTR_t **ret_str)
{
	return _plm3u8_GetListItem(pSelf, eHxM3U8_Kindof_TS, cur_str, ret_str);
}


/**
*
*
*
* @param
* @return
*/
HxSTR_t *PL_M3U8_ToBuffer(PL_M3U8_t *pSelf)
{
	HINT32 	size = 0;
	HERROR	hErr = ERR_OK;
	HUINT8	*pBuf = NULL;
	HxSTR_t	*pStr = NULL;

	HxM3U8_toBuffer_t tmp;

	hErr = PL_M3U8_GetSize(pSelf, &size);
	if(hErr != ERR_OK)
	{
		return NULL;
	}

	if(size == 0)
	{
		return NULL;
	}

	pBuf = (HCHAR *)HLIB_STD_MemAlloc(size);
	HxLOG_Assert(pBuf);
	HxSTD_MemSet(pBuf, 0, size);

	HxSTD_MemSet(&tmp, 0, sizeof(HxM3U8_toBuffer_t));
	tmp.buf = pBuf;

	HLIB_LIST_ForeachEx(pSelf->item, _plm3u8_toBuffer, &tmp);

	pStr = HLIB_STR_NewLength(tmp.buf, tmp.len);

	HLIB_STD_MemFree(pBuf);

	return pStr;
}


/**
*
*
*
* @param
* @return
*/
HERROR	HxM3U8_toFile(PL_M3U8_t *pSelf, HxFILE_t file_op)
{
	HLIB_LIST_ForeachEx(pSelf->item, _plm3u8_toFile, file_op);

	return ERR_OK;
}

/* *************************************************************************************************** */
#define ___DEBUG_FUNCTION_______________________________________________________________________________
/* *************************************************************************************************** */

STATIC void _plm3u8_ListDump(void *data)
{
#if defined(CONFIG_DEBUG)
	HxSTR_t *pStr = (HxSTR_t *)data;
	if(pStr)
	{
		LOG("    - tag(%d) string(%s)\n", HLIB_STR_GetUserTag(pStr), HLIB_STR_toUTF8(pStr));
	}
#endif
}


void	PL_M3U8_Dump(PL_M3U8_t *pSelf)
{
#if defined(CONFIG_DEBUG)
	if(pSelf == NULL)
	{
		return;
	}

	LOG("pSelf(0x%x)\n", (HUINT32)pSelf);
	LOG("  + cur_size: %d\n", pSelf->cur_size);
	LOG("  + max_size: %d\n", pSelf->max_size);
	LOG("  + Item (0x%x) : \n", (HUINT32)(pSelf->item));
	HLIB_LIST_Foreach(pSelf->item, _plm3u8_ListDump);
#endif
}


