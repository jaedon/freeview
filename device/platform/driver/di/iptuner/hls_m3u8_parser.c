/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   hls_m3u8_parser.c  $
 * Version:			$Revision:   1.0  $
 * Original Author:		Yang Hyun Uk $
 * Current Author:	$Author: ykkim3@humaxdigital.com $
 * Date:			$Date: 2011.07.20
 * File Description:	M3U8 URL Parser
 * Module:
 * Remarks:
 */

/**
 * @defgroup M3U8_Parser
 * @brief M3U8_Parser 이며 Parsing 및 구조화 작업을 한다. <br> 구조화된 데이터는 HLS_Controller에서 운용하기 쉽도록 한다.
 */

/**
 * @author Hyunuk Yang(huyang@humaxdigital.com), YoungKi Kim (ykkim3@humaxdigital.com)
 * @date 22 July 2011
 */

/**
 * @brief M3U8_Parser 이며 Parsing 및 구조화 작업을 한다. <br> 구조화된 데이터는 HLS_Controller에서 운용하기 쉽도록 한다.
 */

/**
 * @note
 * Copyright (C) 2011 Humax Corporation. All Rights Reserved. <br>
 * This software is the confidential and proprietary information
 * of Humax Corporation. You may not use or distribute this software
 * except in compliance with the terms and conditions of any applicable license
 * agreement in writing between Humax Corporation and you.
 */

/**
 * @todo X86 에서 테스트를 위해 humax def 를 사용하지 않았다. 변경 필요하다면 해야함.
 */

/*@{*/

/**
 * @file hls_m3u8_parser.c
 */

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
/* Start Including Header Files */
#include "di_iptuner_config.h"
#include "di_iptuner_common.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <inttypes.h>	 /** for 64bit MACRO ex>PRId64 */

#include "hls_m3u8_parser.h"
#include "di_uart.h"
/* End Including Headers */
/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
//#define HLS_M3U8_PARSER_DEBUG

#ifdef IPTUNER_ALL_DEBUG	/* DEBUG Define */
#ifdef HLS_M3U8_PARSER_DEBUG
#define PrintData		DI_UART_Print
#define PrintDebug		DI_UART_Print
#define PrintError		DI_UART_Print
#define PrintEnter		DI_UART_Print("(+)%s\n", __FUNCTION__)
#define PrintExit		DI_UART_Print("(-)%s\n", __FUNCTION__)
#else
#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
#define PrintEnter
#define PrintExit
#define PrintError		DI_UART_Print
#endif

#else

#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
#define PrintError		while (0) ((int (*)(char *, ...)) 0)
#define PrintEnter
#define PrintExit
#endif /* The END of DEBUG DEFINE */

#ifdef HLS_M3U8_PARSER_DEBUG
#define PrintCriticalDebug      DI_UART_Print
#else
#define PrintCriticalDebug      while (0) ((int (*)(char *, ...)) 0)
#endif

#define UNUSED_PARAM(x)  ((void)x)
/* End #define */

#define M3U8_CONST_VERSION 3

/* Codec string */
#define HLS_CODECSTR_MP4A				"mp4a"
#define HLS_CODECSTR_MP4V				"mp4v"
#define HLS_CODECSTR_AVC1				"avc1"
#define HLS_CODECSTR_HEVC				"hevc"
#define HLS_CODECSTR_HEV1				"hev1"
#define HLS_CODECSTR_EC3				"ec-3"
#define HLS_CODECSTR_AVC3				"avc3"

#define HLS_CODECSUB_AVC1_BASELINE		"42001e"
#define HLS_CODECSUB_AVC1_BASELINE2	"42C028"
#define HLS_CODECSUB_AVC1_BASELINE3	"42C01E"
#define HLS_CODECSUB_AVC1_UNKNOWN		"640028"
#define HLS_CODECSUB_AVC1_BASELINE_PROF "66"
#define HLS_CODECSUB_AVC1_BASELINE_LEVEL "30"
#define HLS_CODECSUB_AVC1_BASELINE_LEVEL2 "40"
#define HLS_CODECSUB_AVC1_MAIN			"4d001e"
#define HLS_CODECSUB_AVC1_MAIN_PROF	"77"
#define HLS_CODECSUB_AVC1_MAIN_LEVEL	"30"

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */
typedef enum
{
	eM3U8_TAG_EXTINF = 0,
	eM3U8_TAG_EXT_X_KEY,
	eM3U8_TAG_EXT_X_STREAM_INF,
	eM3U8_TAG_EXT_X_TARGETDURATION,
	eM3U8_TAG_EXT_X_MEDIA_SEQUENCE,
	eM3U8_TAG_EXTM3U,
	eM3U8_TAG_EXT_X_PLAYLIST_TYPE,
	eM3U8_TAG_EXT_X_PROGRAM_DATE_TIME,
	eM3U8_TAG_EXT_X_ALLOW_CACHE,
	eM3U8_TAG_EXT_X_ENDLIST,
	eM3U8_TAG_EXT_X_DISCONTINUITY,
	eM3U8_TAG_EXT_X_VERSION,
	eM3U8_TAG_EXT_X_BYTERANGE,
	eM3U8_TAG_EXT_X_MEDIA,
	eM3U8_TAG_EXT_X_I_FRAMES_ONLY,
	eM3U8_TAG_EXT_X_I_FRAME_STREAM_INF,
	eM3U8_TAG_MAX
} m3u8Tag_e;
/* End typedef */

typedef struct m3u8TagTable_s
{
	HUINT8 ucTagName[M3U8_CONST_MAX_TAGNAME];
	HUINT32 unVersion;
} m3u8TagTable_t;

/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/
/* Start global variable */
static const HUINT32 g_ParserVersion = M3U8_CONST_VERSION;

static const m3u8TagTable_t g_m3u8TagTable[eM3U8_TAG_MAX] =
{
	{M3U8_TAG_EXTINF,1},					// eM3U8_TAG_EXTINF
	{M3U8_TAG_EXT_X_KEY,1},					// eM3U8_TAG_EXT_X_KEY
	{M3U8_TAG_EXT_X_STREAM_INF,1},			// eM3U8_TAG_EXT_X_STREAM_INF,
	{M3U8_TAG_EXT_X_TARGETDURATION,1},		// eM3U8_TAG_EXT_X_TARGETDURATION
	{M3U8_TAG_EXT_X_MEDIA_SEQUENCE,1},		// eM3U8_TAG_EXT_X_MEDIA_SEQUENCE
	{M3U8_TAG_EXTM3U,1},					// eM3U8_TAG_EXTM3U
	{M3U8_TAG_EXT_X_PLAYLIST_TYPE,1},		// eM3U8_TAG_EXT_X_PLAYLIST_TYPE,
	{M3U8_TAG_EXT_X_PROGRAM_DATE_TIME,1},	// eM3U8_TAG_EXT_X_PROGRAM_DATE_TIME,
	{M3U8_TAG_EXT_X_ALLOW_CACHE,1},			// eM3U8_TAG_EXT_X_ALLOW_CACHE,
	{M3U8_TAG_EXT_X_ENDLIST,1},				// eM3U8_TAG_EXT_X_ENDLIST,
	{M3U8_TAG_EXT_X_DISCONTINUITY,1},		// eM3U8_TAG_EXT_X_DISCONTINUITY,
	{M3U8_TAG_EXT_X_VERSION,1},				// eM3U8_TAG_EXT_X_VERSION,
	{M3U8_TAG_EXT_X_BYTERANGE,4},			// eM3U8_TAG_EXT_X_BYTERANGE,
	{M3U8_TAG_EXT_X_MEDIA,4},				// eM3U8_TAG_EXT_X_MEDIA,
	{M3U8_TAG_EXT_X_I_FRAMES_ONLY,4},		// eM3U8_TAG_EXT_X_I_FRAMES_ONLY,
	{M3U8_TAG_EXT_X_I_FRAME_STREAM_INF,4},	// eM3U8_TAG_EXT_X_I_FRAME_STREAM_INF,
};

/* End global variable */

/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
/* Start static variable */
/* End static variable */


/*******************************************************************/
/************************ static funtions **************************/
/*******************************************************************/

static HUINT64 _atohui64(char *nptr)
{
	char *s = (char *)nptr;
	HUINT64 acc = 0;
	// int neg = 0;

  while(isspace((int)*s))
    s++;
/*  if (*s == '-')
    {
      neg = 1;
      s++;
    }
  else if (*s == '+')
    s++;
*/
  while (VK_isdigit((int)*s))
    {
      acc = 10 * acc + ((int)*s - '0');
      s++;
    }
/*
  if (neg)
    acc *= -1;
*/
  return acc;
}

/**
 * @b Function @b Description <br>
 * iso8601 형식의 DateTime을 UTC Time(long) 으로 변경한다. <br>
 *
 * @param[in] datetime	: DataTime의 버퍼포인터
 *
 * @return  #  UTC Time <br>
 *
 */
static HLONG _iso8601toUTC(const char *datetime)
{
	HLONG utc = 0;
	struct tm curtime;
	VK_memset(&curtime, 0, sizeof(struct tm));
	strptime(datetime, "%FT%T%z", &curtime);
	utc = mktime(&curtime);
	return utc;
}

static int __hls_split(HINT8* msg, HINT8* split, HINT8*** result)
{
    HUINT32 i=0;
    HUINT32 charCount=0;
	HUINT32 totalCount=0;
    HINT8 *prevPoint=msg;
    HINT8 *currPoint=NULL;
    HINT8 **array2d=NULL;

	do
	{
        currPoint=(int8_t*)VK_strstr((const char*)prevPoint, (const char*)split);
        if(currPoint!=NULL){
            totalCount=currPoint-msg;					/*total length of msg so far.*/
            if(prevPoint==msg) charCount=totalCount;	/*total length of msg so far(1st)*/
            else charCount=currPoint-prevPoint;			/*difference between currPoint - prevPoint*/

			if(i==0)
			{
	            array2d=(int8_t**)VK_MEM_Alloc(sizeof(char*));
			}
			else
			{
				array2d=(int8_t**)VK_MEM_Realloc(array2d, sizeof(char*)*(i+1));
			}
            array2d[i]=(int8_t*)VK_MEM_Alloc(charCount + 1);
			if(array2d[i] == NULL)
			{
				PrintError("%s (%d) : Error> Malloc Fail.\n",__FUNCTION__,__LINE__);
				VK_MEM_Free(array2d);
				return 0;
			}
            VK_strncpy((char*)array2d[i], (const char*)prevPoint, charCount);
            array2d[i][charCount]='\0';
            prevPoint=currPoint+VK_strlen(split);
        }
    }
	while(currPoint!=NULL && ++i);
    if( i > 0 )
	{
        array2d=(int8_t**)VK_MEM_Realloc( array2d, sizeof(char*)*(i+1));
        charCount=VK_strlen(msg)-totalCount;
        array2d[i]=(int8_t*)VK_MEM_Alloc(charCount + 1);
        VK_strncpy((char*)array2d[i], (const char*)prevPoint, charCount);
        array2d[i][charCount]='\0';
        ++i;
        *result=array2d;
    }
    return i;
}

void __hls_freeSplit(HINT8** result, HINT32 count)
{
	while (--count>-1)
	{
		VK_MEM_Free(result[count]);
		result[count] = NULL;
	}
	VK_MEM_Free(result);
	result = NULL;
}

HUINT32 __hls_lTrimString(HUINT8 *src, HINT32 length, HUINT8 *dest)
{
	HINT32 i;

	for (i=0; i<length; i++)
	{
		if (src[i] != 0x20/*SPACE*/)
			break;
	}

	if (i >= length)
		return !ERR_OK;

	VK_memcpy(dest, src+i, length-i);
	return ERR_OK;
}

HUINT32 __hls_rTrimString(HUINT8 *src, HINT32 length, HUINT8 *dest)
{
	HINT32 i;

	for (i=length-1; i>=0; i--)
	{
		if (src[i] != 0x20/*SPACE*/)
			break;
	}

	if (i <= 0)
		return !ERR_OK;

	VK_memcpy(dest, src, i+1);
	return ERR_OK;
}

HUINT32 __hls_bothTrimString( HUINT8 *src, HINT32 length, HUINT8 *dest)
{
	if (src == NULL || length <= 0 || dest == NULL)
		return !ERR_OK;

	__hls_lTrimString(src, length, dest);
	VK_memset(src, 0, length);
	VK_memcpy(src, dest, VK_strlen(dest));
	VK_memset(dest, 0, VK_strlen(dest));
	return __hls_rTrimString(src, VK_strlen(src), dest);
}

/**
 * @b Function @b Description <br>
 * 읽어온 Tag에서 TAG를 알아낸다. <br>
 *
 * @param[in] pTag	: Tag의 버퍼포인터
 *
 * @return  #  m3u8Tag_e(Tag Enum) : 성공 시 <br>
 *          #  eM3U8_TAG_MAX(Tag Enum) : 실패시 <br>
 */
static m3u8Tag_e P_HLS_M3U8_IdentifyTag(HINT8 * pTag)
{
	HUINT32 i = 0;
	m3u8Tag_e eTag = eM3U8_TAG_MAX;
	for(i = 0 ; i < eM3U8_TAG_MAX ; i++)
	{
		if(VK_strlen(pTag) == VK_strlen(g_m3u8TagTable[i].ucTagName))
		{
			if(!VK_strncmp(pTag,g_m3u8TagTable[i].ucTagName,VK_strlen(g_m3u8TagTable[i].ucTagName)))
			{
				if(g_m3u8TagTable[i].unVersion <= HLS_M3U8_GetParserVersion())
				{
					eTag = i;
				}
				else
					break;
			}
		}
	}
	return eTag;
}

/**
 * @b Function @b Description <br>
 * 읽어온 Line에서 TAG를 얻는다. <br>
 *
 * @param[in] buffer : Line의 버퍼포인터
 * @param[out] found : 찾아낸 TAG
 * @return  #  TRUE : 성공 <br>
 *          #  FALSE : 실패시 <br>
 */
static HBOOL P_HLS_M3U8_GetTag(const char * buffer, HINT8* found)
{
        HINT8 *p = NULL;
        HINT32 i=0,result=0;
        p = (HINT8*)buffer;
        if ( p )
        {
                for(i =0; i< (int)VK_strlen(p); i++ )
                {
                        result = VK_strncmp(p+/*skip+*/i, ":" ,1);
                        if ( result == 0 )
                        {
                                break;
                        }
                        result = VK_strncmp(p+/*skip+*/i, "\n", 1);
                        if ( result == 0 )
                        {
                                break;
                        }
                        result = VK_strncmp(p+/*skip+*/i, "\0", 1);
                        if ( result == 0 )
                        {
                                break;
                        }
                }
                VK_strncpy(found, p, i);
                return TRUE;
        }
        else
        {
                return FALSE;
        }
}


/**
 * @b Function @b Description <br>
 * URL 의 Base URL 을 얻어 낸다. <br>
 *
 * @param[in] url	: Base URL 을 얻어 낼 버퍼포인터
 *
 * @return  #  BASE URL 의 Pointer : 성공 시 <br>
 *          #  NULL : 실패시 <br>
 */
static HINT8 * P_HLS_M3U8_GetAndFillBaseURL(HINT8 * url )
{
	HINT8 * baseurl;
	HINT32 i, url_length;
//	PrintEnter;
	url_length = (HINT32)VK_strlen(url);
	/* Address 끝 Character 부터 "/" 를 검색 */
	for (  i = url_length-1; i > 0 ; i-- )
	{
		if ( 0 == VK_strncmp( url+i, "/", 1))
		{
			/* Protocol 이 있으면 BASE URL 을 찾지 못한 것임. */
			if ( 0 == VK_strncmp( url+i-2, "://", 3) )
			{
				PrintError("Not found Base URL!!!! \n");
//				PrintExit;
				return NULL;
			}
			//PrintDebug("found Base URL!!!! ");
			baseurl = (HINT8*)strndup ((const char*)url, i );
			//PrintDebug("%s\n", baseurl);
//			PrintExit;
			return baseurl;
		}
	}
	PrintError("Not found Base URL!!!! \n");
//	PrintExit;
	return NULL;
 }


/**
 * @b Function @b Description <br>
 * Buffer 의 Niddle 이후의 값을 얻어 온다, ",", "\n", NULL 을 끝으로 인지한다. <br>
 * Attribute의 Value 값을 읽는다. <br>
 *
 * @param[in] buffer	: 얻어올 Value 가 있는 버퍼 포인터.
 * @param[in] niddle	: 찾고자 하는 값의 정의. <br> ex) "BANDWIDTH=", "EXTINF:" 등
 * @param[out] found	: 얻어올 Value 를 저장할 버퍼 포인터.
 *
 * @return  #  TRUE : 성공 시 <br>
 *          #  FALSE : 실패시 <br>
 * @note found 버퍼는 반드시 할당된 버퍼로 전달되어야 한다.
 */
static m3u8Err_e P_HLS_M3U8_GetValue(const char * buffer, char * niddle, char * found)
{
	HINT8 *p = NULL;
	HINT32 i=0,skip=0,result=0;
	HBOOL bOpen = FALSE;
//	PrintEnter;
	p = (HINT8*) VK_strstr((const char*)buffer,(const char*)niddle);
	if ( p )
	{
		for(i =0; i<(HINT32)VK_strlen(p); i++ )
		{
			result = VK_strncmp(p+/*skip+*/i+VK_strlen(niddle), "\"" ,1);
			if ( result == 0 )
			{
				skip++;
				i++;
				for(; i<(HINT32)VK_strlen(p); i++ )
				{
					result = VK_strncmp(p+/*skip+*/i+VK_strlen(niddle), "\"" ,1);
					if ( result == 0 )
					{
						break;
					}
				}
				if ( result == 0 )
				{
					break;
				}
				else
					continue;
			}
			result = VK_strncmp(p+/*skip+*/i+VK_strlen(niddle), "=" ,1);
			if ( result == 0 )
			{
				if(bOpen == FALSE)
				{
					skip++;
					bOpen = TRUE;
				}
				continue;
			}
			result = VK_strncmp(p+/*skip+*/i+VK_strlen(niddle), ":" ,1);
			if ( result == 0 )
			{
				if(bOpen == FALSE)
				{
					skip++;
					bOpen = TRUE;
				}
				continue;
			}
			result = VK_strncmp(p+/*skip+*/i+VK_strlen(niddle), "," ,1);
			if ( result == 0 )
			{
				break;
			}
			result = VK_strncmp(p+/*skip+*/i+VK_strlen(niddle), "\n", 1);
			if ( result == 0 )
			{
				break;
			}
			result = VK_strncmp(p+/*skip+*/i+VK_strlen(niddle), "\0", 1);
			if ( result == 0 )
			{
				break;
			}
			if ( p+/*skip+*/i+VK_strlen(niddle) == NULL )
			{
				PrintError("Not found Value with %s\n", niddle);
				PrintExit;
				return M3U8_ERR_FAIL;
			}
		}
		VK_strncpy(found, p+skip+VK_strlen(niddle), i-skip);
//		PrintDebug ("GetValue %s(%d,%d,%d)\n",found,skip,VK_strlen(niddle), i);
//		PrintDebug ("RealLine %s\n",p);
//		PrintExit;
		return M3U8_ERR_OK;
	}
	else
	{
//		PrintDebug("%s(%d) Not found Value with %s\n", __FUNCTION__, __LINE__, niddle);
//		PrintExit;
		return M3U8_ERR_FAIL;
	}
}

/**
 * @b Function @b Description <br>
 * HOST URL 을 조합한다.<br>
 *
 * @param[in] pHostUrl :  host url
 * @param[in] pBaseUrl : BaseUrl
 *
 * @return  #  URL pointer : 성공시 <br>
 *          #  NULL : 실패시 <br>
 */
static HINT8* P_HLS_M3U8_MakeHostUrl(HINT8 *pHostUrl, HINT8 *pBaseUrl)
{
	HINT32 total_strlen=0;
	HINT8* new_full_url;
	HINT8* pProtocolPrefix = NULL;

	if(NULL != VK_strstr((const char*)pBaseUrl,"http://"))
	{
		total_strlen += VK_strlen("http://");
		pProtocolPrefix = "http://";
	}
	else if(NULL != VK_strstr((const char*)pBaseUrl,"https://"))
	{
		total_strlen += VK_strlen("https://");
		pProtocolPrefix = "https://";
	}
	else
	{
		PrintError("%s(%d) It's weird. base url : %s \n",__FUNCTION__,__LINE__, pBaseUrl);
		PrintExit;
		return NULL;
	}

	total_strlen += VK_strlen(pHostUrl);
	/* 전체 사이즈 +3 중간에 "/" 를 넣을 공간 및 "\0" 추가 공간.*/
	new_full_url = VK_MEM_Alloc(total_strlen+3);
	if(new_full_url == NULL)
	{
		PrintError("%s(%d) Memory Allocation Failed\n",__FUNCTION__,__LINE__);
		PrintExit;
		return NULL;
	}
	VK_memset(new_full_url,0x00,total_strlen+3);
	VK_snprintf((char*)new_full_url,total_strlen+3, "%s%s", pProtocolPrefix, pHostUrl);
	//cso_log(CSO_LOG_ERROR,"total Base url size %d\n", total_strlen+2);
	//cso_log(CSO_LOG_ERROR,"New host url : %s\n", new_full_url);
	return new_full_url;
}

/**
 * @b Function @b Description <br>
 * top_m3u8, m3u8, segment 정보로 Base URL 기반으로 Full URL(절대 경로) 을 조합한다.<br>
 *
 * @param[in] pBaseUrl : BaseUrl
 * @param[in] pUrl :  상대 경로
 *
 * @return  #  URL pointer : 성공시 <br>
 *          #  NULL : 실패시 <br>
 */
static HINT8* P_HLS_M3U8_MakeUrlFromBaseUrl(HINT8 *pBaseUrl, HINT8 *pUrl)
{
	HINT32 total_strlen=0;
	HINT8* new_full_url;

	total_strlen += VK_strlen(pBaseUrl);
	total_strlen += VK_strlen(pUrl);
	/* 전체 사이즈 +3 중간에 "/" 를 넣을 공간 및 "\0" 추가 공간.*/
	new_full_url = VK_MEM_Alloc(total_strlen+3);
	if(new_full_url == NULL)
	{
		PrintError("%s(%d) Memory Allocation Failed\n",__FUNCTION__,__LINE__);
		PrintExit;
		return NULL;
	}
	VK_memset(new_full_url,0x00,total_strlen+3);
	VK_snprintf((char*)new_full_url,total_strlen+3, "%s/%s", pBaseUrl,pUrl);
//	PrintDebug("total Base url size %d\n", total_strlen+2);
//	PrintDebug("New url %s\n", new_full_url);
	return new_full_url;
}

/**
 * @b Function @b Description <br>
 * M3U8 Data를 Line 단위로 읽는다.
 *
 * @param[out] pLine	: 읽어온 Line Data
 * @param[in] pM3U8	: M3U8 Data
 * @param[in] unSize	: M3U8 Data의 Size
 *
 * @return  읽어온 Line의 길이
 * @note pLine은 외부에서 Free 해야한다.
 */
static HUINT32 P_HLS_M3U8_ReadLine(HINT8** pLine, HINT8* pM3U8, HUINT32 unSize)
{
	HINT8 *p = pM3U8;
	HUINT32 unLineLen = 0;
	HINT8 *pEnd = NULL;
//	PrintEnter;
	if(p == NULL)
	{
		*pLine = NULL;
//		PrintExit;
		return 0;
	}
	pEnd = p + unSize;
	while(p < pEnd)
	{
		if( *p == '\n' || *p == '\r')
		{
//			p++;
			if(unLineLen == 0)
			{
				*pLine = NULL;
//				PrintExit;
				return 1;
			}
			unLineLen++;
			break;
		}
		else
		{
			p++;
			unLineLen++;
		}
	}
	if(unLineLen == 0)
	{
		p++;
		*pLine = NULL;
//		PrintExit;
		return 0;
	}
	*pLine = DD_MEM_Alloc(unLineLen+1);
	VK_memset(*pLine,0x00,unLineLen+1);
	VK_MEM_Memcpy(*pLine, pM3U8, p - pM3U8);
//	PrintError("Read %s (%d:%d) \n", *pLine, unLineLen, p - pM3U8);
//	PrintExit;
	return unLineLen;
}

/**
 * @b Function @b Description <br>
 * M3U8이 Valid한지 M3U8 TAG로 유무로 확인
 *
 * @param[in] pBuf	: M3U8 Data
 * @param[in] unSize	: M3U8 사이즈
 *
 * @return  ErrCode
 */
static m3u8Err_e P_HLS_M3U8_IsValid(HINT8* pBuf, HUINT32 unSize)
{
	HINT8 *pLine = NULL;
	HUINT32 unLineLen = 0;
//	PrintEnter;
	if(pBuf == NULL)
	{
		PrintError("%s(%d) M3U8 Buffer is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return M3U8_ERR_FAIL;
	}
	if((unLineLen= P_HLS_M3U8_ReadLine(&pLine,pBuf,unSize)) <= 0)
	{
		PrintError("%s(%d) Read Line Fail\n",__FUNCTION__,__LINE__);
		PrintExit;
		return M3U8_ERR_FAIL;
	}
	if(pLine == NULL)
	{
		PrintError("%s(%d) Line Data is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return M3U8_ERR_FAIL;
	}
#if 1
	if(unLineLen < VK_strlen(M3U8_TAG_EXTM3U))
	{
		PrintError("%s(%d) Line Data is ( %s : %u : %d)\n",__FUNCTION__,__LINE__,pLine,unLineLen,VK_strlen(M3U8_TAG_EXTM3U));
		if( pLine != NULL )
		{
			DD_MEM_Free(pLine);
		}
		PrintExit;
		return M3U8_ERR_FAIL;
	}
#endif
	if(VK_strncmp(pLine,M3U8_TAG_EXTM3U,VK_strlen(M3U8_TAG_EXTM3U)))
	{
		if( pLine != NULL )
		{
			DD_MEM_Free(pLine);
		}
		PrintExit;
		return M3U8_ERR_FAIL;
	}
	if( pLine != NULL )
	{
		DD_MEM_Free(pLine);
	}
//	PrintExit;
	return M3U8_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * M3U8이 Valid한지 M3U8 TAG로 유무로 확인
 *
 * @param[in] pBuf	: M3U8 Data
 * @param[in] unSize	: M3U8 사이즈
 *
 * @return  ErrCode
 */
static m3u8Err_e P_HLS_M3U8_ConvertDateTime(HINT8* pLine, HLONG *plUTC)
{
	HINT8 value[M3U8_CONST_VALUE_BUFFSIZE] = {0};
	PrintEnter;
	if(pLine == NULL)
	{
		PrintError("%s(%d) M3U8 Buffer is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return M3U8_ERR_FAIL;
	}
	if(P_HLS_M3U8_GetValue((const char*)pLine, M3U8_TAG_EXT_X_PROGRAM_DATE_TIME,(char*)value) != M3U8_ERR_OK)
	{
		PrintError("%s(%d) %s is not exist.\n",__FUNCTION__,__LINE__,M3U8_TAG_EXT_X_PROGRAM_DATE_TIME);
		*plUTC = 0;
	}
	else
	{
		PrintError("%s(%d) %s \n",__FUNCTION__,__LINE__,value);
		*plUTC = _iso8601toUTC((char*)value);
	}
	PrintExit;
	return M3U8_ERR_OK;
}


static void P_HLS_ConvertStrToCodec(HUINT8* str, HLS_CODEC_e* codec)
{
	HUINT32 split_cnt = 0;
	HUINT8 **split = NULL;
	HUINT8 *str_trimmed = NULL;
	HUINT32 charCount=VK_strlen(str);

	str_trimmed = VK_MEM_Alloc(charCount+1);
	VK_MEM_Memset(str_trimmed, 0x00,charCount+1);
	__hls_bothTrimString(str, charCount, str_trimmed);
	if(VK_strlen(str_trimmed) <= charCount){
		str_trimmed[VK_strlen(str_trimmed)]	= '\0';
	}

	split_cnt = __hls_split((HINT8*)str_trimmed, (HINT8*)".", (HINT8***)&split);
	if(split_cnt == 0)
	{
		if(!VK_strcmp(str_trimmed, HLS_CODECSTR_AVC1))
		{
			*codec |= HLS_VIDEO_CODEC_AVC1;
		}
		else if(!VK_strcmp(str_trimmed, HLS_CODECSTR_MP4V))
		{
			*codec |= HLS_VIDEO_CODEC_MP4V;
		}
		else if(!VK_strcmp(str_trimmed, HLS_CODECSTR_EC3))
		{
			*codec |= HLS_AUDIO_CODEC_EC3;
		}
		else if(!VK_strcmp(str_trimmed, HLS_CODECSTR_AVC3))
		{
			*codec |= HLS_VIDEO_CODEC_AVC3;
		}
		else if(!VK_strcmp(str_trimmed, HLS_CODECSTR_HEVC))
		{
			*codec |= HLS_VIDEO_CODEC_HEVC;
		}
		else if(!VK_strcmp(str_trimmed, HLS_CODECSTR_HEV1))
		{
			*codec |= HLS_VIDEO_CODEC_HEVC;
		}
	}
	else if(split_cnt > 0)
	{
		/* prefix : mp4a */
		if(split[0] != NULL && !VK_strcmp(split[0], HLS_CODECSTR_MP4A))
		{
			/* OTI for mp4a */
			if(split[1] != NULL && (!VK_strcmp(split[1], "40") || !VK_strcmp(split[1], "0x40")))
			{
				*codec |= HLS_AUDIO_CODEC_AAC;
				/* OTI for audio : 0x40 */
				if(split[2] != NULL && !VK_strcmp(split[2], "2"))
				{
					*codec |= HLS_AUDIO_CODEC_AAC; // Low complexible
				}
				else if(split[2] != NULL && !VK_strcmp(split[2], "5"))
				{
					*codec |= HLS_AUDIO_CODEC_AAC_HE;
				}
				else if(split[2] != NULL && !VK_strcmp(split[2], "29"))
				{
					*codec |= HLS_AUDIO_CODEC_AAC_HE2;
				}
			}

		}
		/* prefix : avc1 */
		else if(split[0] != NULL && !VK_strcmp(split[0], HLS_CODECSTR_AVC1))
		{
			/* OTI for avc1 */
			if(split[1] != NULL &&
				( !VK_strcmp(split[1], HLS_CODECSUB_AVC1_BASELINE) ||
				!VK_strcmp(split[1], HLS_CODECSUB_AVC1_BASELINE2) ||
				!VK_strcmp(split[1], HLS_CODECSUB_AVC1_BASELINE3) ||
				!VK_strcmp(split[1], HLS_CODECSUB_AVC1_MAIN) ||
				!VK_strcmp(split[1], HLS_CODECSUB_AVC1_UNKNOWN) )
				)
			{
				/* Baseline profile */
				*codec |= HLS_VIDEO_CODEC_AVC1;
			}
			else if(split[1] != NULL && !VK_strcmp(split[1], HLS_CODECSUB_AVC1_BASELINE_PROF))
			{
				if(split[2] != NULL && (!VK_strcmp(split[2], HLS_CODECSUB_AVC1_BASELINE_LEVEL) || !VK_strcmp(split[2], HLS_CODECSUB_AVC1_BASELINE_LEVEL2)))
				{
					/* Baseline profile : another notation */
					*codec |= HLS_VIDEO_CODEC_AVC1;
				}
			}
			else if(split[1] != NULL && !VK_strcmp(split[1], HLS_CODECSUB_AVC1_MAIN_PROF))
			{
				if(split[2] != NULL && !VK_strcmp(split[2], HLS_CODECSUB_AVC1_MAIN_LEVEL))
				{
					/* Main profile */
					*codec |= HLS_VIDEO_CODEC_AVC1;
				}
			}
			else
			{
				/* We assume all codecs is AVC1 except identified codecs, if prefix is AVC1 */
				*codec |= HLS_VIDEO_CODEC_AVC1;
			}
		}
		/* prefix : avc3 */
		else if(split[0] != NULL && !VK_strcmp(split[0], HLS_CODECSTR_AVC3))
		{
			*codec |= HLS_VIDEO_CODEC_AVC3;
		}
		/* prefix : mp4v */
		else if(split[0] != NULL && !VK_strcmp(split[0], HLS_CODECSTR_AVC1))
		{
			*codec |= HLS_VIDEO_CODEC_AVC1;
		}
		else if(split[0] != NULL && !VK_strcmp(split[0], HLS_CODECSTR_HEVC))
		{
			*codec |= HLS_VIDEO_CODEC_HEVC;
		}
		else if(split[0] != NULL && !VK_strcmp(split[0], HLS_CODECSTR_HEV1))
		{
			*codec |= HLS_VIDEO_CODEC_HEVC;
		}
		else
		{
			PrintError("%s(%d) Not supported codec by HLS \n", __FUNCTION__, __LINE__);
			*codec = HLS_CODEC_NONE;
		}
		__hls_freeSplit((int8_t**)split, split_cnt);
		split = NULL;
	}
	VK_MEM_Free(str_trimmed);
	str_trimmed = NULL;
}

 /**
  * @b Function @b Description <br>
  * Segment 정보 Parsing
  *
  * @param[in] m3u8  : m3u8 Handle
  * @param[in] pPlaylist : 현재의 Segment 정보가 축적될 pPlaylist
  * @param[in] pLine : Parsing 할 Line Data
  * @param[in] pSubLine  : Parsing 할 Sub Line Data (URL Data를 갖는 Line, Segment의 정보는 두줄의 Line으로 구성됨.)
  * @return  codec
  */
 static HLS_CODEC_e P_HLS_ParseCodec(HINT8* codecstr )
 {
 	HINT32 nrcodecs = 0;
	HUINT8 **codecs = NULL;
	HUINT32 loopcnt = 0;
	HLS_CODEC_e codec = HLS_CODEC_NONE;

	nrcodecs = __hls_split(codecstr, (HINT8*)",", (HINT8***)&codecs);

	if(nrcodecs == 0)
	{
		P_HLS_ConvertStrToCodec(codecstr, &codec);
	}
	else if(nrcodecs > 0)
	{
		for(loopcnt = 0; loopcnt < nrcodecs; loopcnt++)
		{
			P_HLS_ConvertStrToCodec(codecs[loopcnt], &codec);
		}
		__hls_freeSplit((int8_t**)codecs, nrcodecs);
		codecs = NULL;
	}
	return codec;
 }

/**
 * @b Function @b Description <br>
 * EXTINF 이외의 Simple 한 구조의 TAG Parsing
 *
 * @param[in] m3u8	: m3u8 Handle
 * @param[in] pPlaylist	: 현재의 Stream 정보가 축적될 pPlaylist
 * @param[in] pLine	: Parsing 할 Line Data
 * @return  ErrCode
 */
static m3u8Err_e P_HLS_M3U8_ParseSimpleTags(m3u8_t *m3u8, m3u8Playlist_t *pPlaylist, m3u8Tag_e eTag, HINT8 *pLine)
{
	HINT8 value[M3U8_CONST_VALUE_BUFFSIZE] = {0};
	m3u8Segment_t *pSegment = NULL;
	m3u8Stream_t *pStream = NULL;
	m3u8Program_t *pProgram = NULL;
	m3u8Node_t *pNode = NULL;
//	PrintEnter;
	if(m3u8 == NULL)
	{
		PrintError("%s(%d) M3U8 is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return M3U8_ERR_FAIL;
	}
	if(pLine == NULL)
	{
		PrintError("%s(%d) Line Data is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return M3U8_ERR_FAIL;
	}
	if(pPlaylist == NULL)
	{
		PrintError("%s(%d) M3U8 Url Structure is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return M3U8_ERR_FAIL;
	}

	/*
	*	When the first m3u8 included other m3u8 list has received, the pPlaylist argument is dummy playlist.
	*	That dummy playlist is used to know base url in the M3U8_Create function.
	*	So, check the playlist argument if it is dummy or not. In case of dummy, ProgramId has the M3U8_CONST_DUMMY_PROGRAM_ID.
	*	If the playlist is the dummy list, would be return to function.
	*/
	if(pPlaylist->nProgramId == M3U8_CONST_DUMMY_PROGRAM_ID)
	{
		// TODO : #EXT-X-ENDLIST tag 외의 다른 tag가 들어오는 경우가 있으면 해당내용 처리 해줘야 함.
		switch(eTag)
		{
			case eM3U8_TAG_EXT_X_ENDLIST:
				/*
				*	RTL app 에서 program list가 오지 않고 처음 m3u8에 stream list가 들어 있는 경우도 있다.
				*	따라서 program list가 없이 stream list가 왔을 경우 eM3U8_TAG_EXT_X_ENDLIST 처리는 기존의 flow를 따라야 한다.
				*	pPlaylist->pStream 가 NULL 인것은 stream list가 없이 eM3U8_TAG_EXT_X_ENDLIST 나온 경우로 이것은 program list를 나타낸다.
				*/
				if(pPlaylist->pStream == NULL)
				{
					PrintDebug("%s(%d) The first m3u8 list included other m3u8 has received \n",__FUNCTION__,__LINE__);
					PrintExit;
					return M3U8_ERR_OK;
				}
				break;

			default:
				PrintDebug("%s(%d) The first m3u8 list\n",__FUNCTION__,__LINE__);
				break;
		}
	}

	if(pPlaylist->pStream == NULL)
	{
		VK_memset(value,0x00,M3U8_CONST_VALUE_BUFFSIZE);
		switch(eTag)
		{
			case eM3U8_TAG_EXT_X_VERSION:
				if(P_HLS_M3U8_GetValue((const char*)pLine, M3U8_TAG_EXT_X_VERSION, (char*)value) != M3U8_ERR_OK)
				{
					PrintError("%s(%d) VERSION is not exist.\n",__FUNCTION__,__LINE__);
					m3u8->unVersion = 1;
				}
				else
				{
					m3u8->unVersion = VK_atoi((const char*)value);
					PrintData("%s(%d) VERSION = (%u).\n",__FUNCTION__,__LINE__,m3u8->unVersion);
				}
				return M3U8_ERR_OK;
			case eM3U8_TAG_EXT_X_ALLOW_CACHE:
				if(P_HLS_M3U8_GetValue((const char*)pLine, M3U8_TAG_EXT_X_ALLOW_CACHE, (char*)value) != M3U8_ERR_OK)
				{
					PrintError("%s(%d) EXT_X_ALLOW_CACHE is not exist.\n",__FUNCTION__,__LINE__);
					m3u8->bAllowCache = FALSE;
				}
				else
				{
					if(!VK_strncmp(value,M3U8_STRING_YES,VK_strlen(M3U8_STRING_YES)))
					{
						m3u8->bAllowCache = TRUE;
					}
					else
					{
						m3u8->bAllowCache = FALSE;
					}
					PrintData("%s(%d) EXT_X_ALLOW_CACHE = (%u).\n",__FUNCTION__,__LINE__,m3u8->bAllowCache);
				}
				return M3U8_ERR_OK;
			case eM3U8_TAG_EXT_X_PLAYLIST_TYPE:
				if(P_HLS_M3U8_GetValue((const char*)pLine, M3U8_TAG_EXT_X_PLAYLIST_TYPE, (char*)value) != M3U8_ERR_OK)
				{
					PrintError("%s(%d) EXT_X_PLAYLIST_TYPE is not exist.\n",__FUNCTION__,__LINE__);
					m3u8->ePlayType = M3U8_PLAYTYPE_NONE;
				}
				else
				{
				if(!VK_strncmp((const char*)value,M3U8_STRING_EVENT,VK_strlen(M3U8_STRING_EVENT)))
					{
						m3u8->ePlayType = M3U8_PLAYTYPE_EVENT;
					}
				else if(!VK_strncmp((const char*)value,M3U8_STRING_VOD,VK_strlen(M3U8_STRING_VOD)))
					{
						m3u8->ePlayType = M3U8_PLAYTYPE_VOD;
					}
					else
					{
						m3u8->ePlayType = M3U8_PLAYTYPE_NONE;
					}
					PrintData("%s(%d) EXT_X_PLAYLIST_TYPE = (%u).\n",__FUNCTION__,__LINE__,m3u8->ePlayType);
				}
				return M3U8_ERR_OK;
		case eM3U8_TAG_EXT_X_ENDLIST:
			PrintDebug("%s(%d) Has EXT_X_ENDLIST Tag.\n",__FUNCTION__,__LINE__);
			return M3U8_ERR_OK;
			default:
				break;
		}
		/* Simple Playlist File ? */
		PrintDebug("%s(%d) Simple Playlist File ?\n",__FUNCTION__,__LINE__);
		pPlaylist->pStream = (m3u8Stream_t*)DD_MEM_Alloc(sizeof(m3u8Stream_t));
		if(pPlaylist->pStream == NULL)
		{
			PrintError("%s(%d) Stream Memory Allocation Fail\n",__FUNCTION__,__LINE__);
			PrintExit;
			return M3U8_ERR_FAIL;
		}
		else
		{
			VK_memset(pPlaylist->pStream,0x00,sizeof(m3u8Stream_t));
		}

		pNode = UTIL_DLLIST_GetHeadNode(&m3u8->programSet);
		while(pNode != NULL)
		{
			pProgram = (m3u8Program_t*)pNode->pData;
			if(pProgram->nProgramId == M3U8_CONST_DEFAULT_PROGRAM_ID)
			{
				break;
			}
			pNode = UTIL_DLLIST_NextNode(pNode);
		}
		if(pProgram == NULL)
		{
			pProgram = (m3u8Program_t*)DD_MEM_Alloc(sizeof(m3u8Program_t));
			if(pProgram == NULL)
			{
				PrintError("%s(%d) Program Memory Allocation Fail\n",__FUNCTION__,__LINE__);
				PrintExit;
				return M3U8_ERR_FAIL;
			}
			else
			{
				VK_memset(pProgram,0x00,sizeof(m3u8Program_t));
				pProgram->nProgramId = M3U8_CONST_DEFAULT_PROGRAM_ID;
				if(UTIL_DLLIST_Preppend(&m3u8->programSet, pProgram) != DLL_ERR_OK)
				{
					PrintError("%s(%d) Insert Program Fail\n",__FUNCTION__,__LINE__);
					PrintExit;
					return M3U8_ERR_FAIL;
				}
			}
		}
		if(UTIL_DLLIST_Preppend(&pProgram->streamSet,pPlaylist->pStream ) != DLL_ERR_OK)
		{
			PrintError("%s(%d) Insert Stream Fail\n",__FUNCTION__,__LINE__);
			PrintExit;
			return M3U8_ERR_FAIL;
		}
	}
	pStream = pPlaylist->pStream;
	if(pStream == NULL)
	{
		PrintError("%s(%d) Stream is not exist \n",__FUNCTION__,__LINE__);
		PrintExit;
		return M3U8_ERR_FAIL;
	}
	VK_memset(value,0x00,M3U8_CONST_VALUE_BUFFSIZE);
	switch(eTag)
	{
		case eM3U8_TAG_EXT_X_TARGETDURATION:
			if(P_HLS_M3U8_GetValue((const char*)pLine, M3U8_TAG_EXT_X_TARGETDURATION,(char*)value) != M3U8_ERR_OK)
			{
				PrintError("%s(%d) %s is not exist.\n",__FUNCTION__,__LINE__,M3U8_TAG_EXT_X_TARGETDURATION);
				pStream->unTargetDuration = 0;
			}
			else
			{
				pStream->unTargetDuration = strtoul((const char*)value,NULL,10);
			}
			break;
		case eM3U8_TAG_EXT_X_MEDIA_SEQUENCE:
			if(P_HLS_M3U8_GetValue((const char*)pLine, M3U8_TAG_EXT_X_MEDIA_SEQUENCE, (char*)value) != M3U8_ERR_OK)
			{
				PrintError("%s(%d) %s is not exist.\n",__FUNCTION__,__LINE__,M3U8_TAG_EXT_X_MEDIA_SEQUENCE);
				pPlaylist->ullSequenceNum = 0;
				pPlaylist->unSegmentNum = 0;
				//pStream->ullSequenceNum = 0;
			}
			else
			{
				//pStream->ullSequenceNum = strtoul((const char*)value,NULL,10);
				//pStream->ullSequenceNum = _atohui64((char*)value);
				pPlaylist->unSegmentNum = 0;
				pPlaylist->ullSequenceNum = _atohui64((char*)value);
				// PrintError("%s(%d) !! SEQUENCE NUMBER = %"PRId64" \n",__FUNCTION__,__LINE__,pPlaylist->ullSequenceNum);
			}
			break;
		case eM3U8_TAG_EXT_X_VERSION:
			if(P_HLS_M3U8_GetValue((const char*)pLine, M3U8_TAG_EXT_X_VERSION, (char*)value) != M3U8_ERR_OK)
			{
				PrintError("%s(%d) VERSION is not exist.\n",__FUNCTION__,__LINE__);
				pStream->unVersion = 1;
			}
			else
			{
				pStream->unVersion = VK_atoi((const char*)value);
				PrintData("%s(%d) VERSION = (%u).\n",__FUNCTION__,__LINE__,pStream->unVersion);
			}
			break;
		case eM3U8_TAG_EXT_X_ALLOW_CACHE:
			if(P_HLS_M3U8_GetValue((const char*)pLine, M3U8_TAG_EXT_X_ALLOW_CACHE, (char*)value) != M3U8_ERR_OK)
			{
				PrintError("%s(%d) EXT_X_ALLOW_CACHE is not exist.\n",__FUNCTION__,__LINE__);
				pStream->bAllowCache = FALSE;
			}
			else
			{
				if(!VK_strncmp(value,M3U8_STRING_YES,VK_strlen(M3U8_STRING_YES)))
				{
					pStream->bAllowCache = TRUE;
				}
				else
				{
					pStream->bAllowCache = FALSE;
				}
				PrintData("%s(%d) EXT_X_ALLOW_CACHE = (%u).\n",__FUNCTION__,__LINE__,pStream->bAllowCache);
			}
			break;
		case eM3U8_TAG_EXT_X_PLAYLIST_TYPE:
			if(P_HLS_M3U8_GetValue((const char*)pLine, M3U8_TAG_EXT_X_PLAYLIST_TYPE, (char*)value) != M3U8_ERR_OK)
			{
				PrintError("%s(%d) EXT_X_PLAYLIST_TYPE is not exist.\n",__FUNCTION__,__LINE__);
				pStream->ePlayType = M3U8_PLAYTYPE_NONE;
			}
			else
			{
				if(!VK_strncmp(value,M3U8_STRING_EVENT,VK_strlen(M3U8_STRING_EVENT)))
				{
					pStream->ePlayType = M3U8_PLAYTYPE_EVENT;
				}
				else if(!VK_strncmp(value,M3U8_STRING_VOD,VK_strlen(M3U8_STRING_VOD)))
				{
					pStream->ePlayType = M3U8_PLAYTYPE_VOD;
				}
				else
				{
					pStream->ePlayType = M3U8_PLAYTYPE_NONE;
				}
				PrintData("%s(%d) EXT_X_PLAYLIST_TYPE = (%u).\n",__FUNCTION__,__LINE__,pStream->ePlayType);
			}
			break;
		case eM3U8_TAG_EXT_X_I_FRAMES_ONLY:
			pStream->bIFrameOnly = TRUE;
			break;
		case eM3U8_TAG_EXT_X_ENDLIST :
			pNode = UTIL_DLLIST_GetTailNode(&pStream->segmentSet);
			if(pNode != NULL)
			{
				pSegment = (m3u8Segment_t*)pNode->pData;
				if(pSegment != NULL)
				{
					pSegment->bLastSegment = TRUE;
				}
				else
				{
					PrintError("%s(%d) Tail Segment is NULL : %s\n",__FUNCTION__,__LINE__,pLine);
				}
				pStream->bEndList = TRUE;
			}
			PrintData("%s(%d) eM3U8_TAG_EXT_X_ENDLIST\n",__FUNCTION__,__LINE__);
			break;
		default:
			PrintDebug("%s(%d) Line Skip : %s\n",__FUNCTION__,__LINE__,pLine);
			break;
	}
//	PrintExit;
	return M3U8_ERR_OK;
}

#if defined(CONFIG_SES_HDPLUS_SMARTTV)

/**
 * @b Function @b Description <br>
 * KEY 정보 Parsing
 *
 * @param[in] m3u8	: m3u8 Handle
 * @param[in] pPlaylist	: 현재의 Stream 정보가 축적될 pPlaylist
 * @param[in] pLine	: Parsing 할 Line Data
 * @return  ErrCode
 */
static m3u8Err_e P_HLS_M3U8_ParseKeyInformation(m3u8_t *m3u8, m3u8Playlist_t *pPlaylist, HINT8 *pLine)
{
//	HINT8 value[M3U8_CONST_VALUE_BUFFSIZE] = {0};
	HINT8 *pValue = NULL;
	HINT32 nValueLen = 0;
	m3u8Stream_t *pStream = NULL;
	m3u8Program_t *pProgram = NULL;
	m3u8Key_t *pKey = NULL;
	m3u8Key_t *pTempKey = NULL;
	m3u8Node_t *pNode = NULL;
//	PrintEnter;
	if(m3u8 == NULL)
	{
		PrintError("%s(%d) M3U8 is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return M3U8_ERR_FAIL;
	}
	if(pLine == NULL)
	{
		PrintError("%s(%d) Line Data is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return M3U8_ERR_FAIL;
	}
	if(pPlaylist == NULL)
	{
		PrintError("%s(%d) M3U8 Url Structure is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return M3U8_ERR_FAIL;
	}
	if(pPlaylist->pStream == NULL)
	{
		/* Simple Playlist File ? */
		PrintError("%s(%d) Simple Playlist File ?\n",__FUNCTION__,__LINE__);
		pPlaylist->pStream = (m3u8Stream_t*)DD_MEM_Alloc(sizeof(m3u8Stream_t));
		if(pPlaylist->pStream == NULL)
		{
			PrintError("%s(%d) Stream Memory Allocation Fail\n",__FUNCTION__,__LINE__);
			PrintExit;
			return M3U8_ERR_FAIL;
		}
		else
		{
			VK_memset(pPlaylist->pStream,0x00,sizeof(m3u8Stream_t));
		}

		pNode = UTIL_DLLIST_GetHeadNode(&m3u8->programSet);
		while(pNode != NULL)
		{
			pProgram = (m3u8Program_t*)pNode->pData;
			if(pProgram->nProgramId == M3U8_CONST_DEFAULT_PROGRAM_ID)
			{
				break;
			}
			pNode = UTIL_DLLIST_NextNode(pNode);
		}
		if(pProgram == NULL)
		{
			pProgram = (m3u8Program_t*)DD_MEM_Alloc(sizeof(m3u8Program_t));
			if(pProgram == NULL)
			{
				PrintError("%s(%d) Program Memory Allocation Fail\n",__FUNCTION__,__LINE__);
				PrintExit;
				return M3U8_ERR_FAIL;
			}
			else
			{
				VK_memset(pProgram,0x00,sizeof(m3u8Program_t));
				pProgram->nProgramId = M3U8_CONST_DEFAULT_PROGRAM_ID;
				if(UTIL_DLLIST_Preppend(&m3u8->programSet, pProgram) != DLL_ERR_OK)
				{
					PrintError("%s(%d) Insert Program Fail\n",__FUNCTION__,__LINE__);
					PrintExit;
					return M3U8_ERR_FAIL;
				}
			}
		}
		if(UTIL_DLLIST_Preppend(&pProgram->streamSet,pPlaylist->pStream ) != DLL_ERR_OK)
		{
			PrintError("%s(%d) Insert Stream Fail\n",__FUNCTION__,__LINE__);
			PrintExit;
			return M3U8_ERR_FAIL;
		}
	}
	pStream = pPlaylist->pStream;
	if(pStream == NULL)
	{
		PrintError("%s(%d) Stream is not exist \n",__FUNCTION__,__LINE__);
		PrintExit;
		return M3U8_ERR_FAIL;
	}
	pKey = (m3u8Key_t*)DD_MEM_Alloc(sizeof(m3u8Key_t));
	if(pKey == NULL)
	{
		PrintError("%s(%d) Key Memory Allocation Fail\n",__FUNCTION__,__LINE__);
		PrintExit;
		return M3U8_ERR_FAIL;
	}
	else
	{
		VK_memset(pKey,0x00,sizeof(m3u8Key_t));
		pKey->eEncPadding = M3U8_ENCPADDING_UNKNOWN;
		pKey->eMethod = M3U8_METHOD_UNKNOWN;
	}
	nValueLen = VK_strlen(pLine) - VK_strlen(M3U8_TAG_EXT_X_KEY);
	pValue = DD_MEM_Alloc(nValueLen);
	if(pValue == NULL)
	{
		PrintError("%s(%d) Value Buffer Memory Allocation Fail\n",__FUNCTION__,__LINE__);
		if(pKey != NULL)
		{
			DD_MEM_Free(pKey);
		}
		PrintExit;
		return M3U8_ERR_FAIL;
	}
	VK_memset(pValue,0x00,nValueLen);
	if(P_HLS_M3U8_GetValue((const char*)pLine, M3U8_ATTR_METHOD,(char*)pValue) != M3U8_ERR_OK)
	{
		PrintError("%s(%d) %s is not exist.\n",__FUNCTION__,__LINE__,M3U8_ATTR_URI);
		VK_printf("[tritop]Parsekeyinformation P_HLS_M3U8_GetValue error  \n");
//		pKey->eMethod = M3U8_METHOD_AESCBC128;
//		pKey->eEncPadding = M3U8_ENCPADDING_PKCS5;
	}
	else
	{
		if(VK_strstr((const char*)pValue,M3U8_STRING_AES_128) != NULL)
		{
			pKey->eMethod = M3U8_METHOD_AESCBC128;
			pKey->eEncPadding = M3U8_ENCPADDING_PKCS7;
		}
		else if(VK_strstr((const char*)pValue,M3U8_STRING_NONE) != NULL)
		{
			VK_printf("[tritop]Parsekeyinformation M3U8_ATTR_METHOD is NONE  \n");
			pKey->eMethod = M3U8_METHOD_NONE;
		}
		else
		{
			VK_printf("[tritop]Parsekeyinformation M3U8_ATTR_METHOD is else  \n");
			PrintError("%s(%d) UNKNOWN Method %s \n",__FUNCTION__,__LINE__,pValue);
			pKey->eMethod = M3U8_METHOD_UNKNOWN;
		}
	}

	VK_memset(pValue,0x00,nValueLen);
	if(P_HLS_M3U8_GetValue((const char*)pLine, M3U8_ATTR_URI, (char*)pValue) != M3U8_ERR_OK)
	{
		PrintError("%s(%d) %s is not exist.\n",__FUNCTION__,__LINE__,M3U8_ATTR_URI);
		// pKey->key_uri = NULL;
		if(pKey != NULL)
		{
			DD_MEM_Free(pKey);
		}
		if(pValue != NULL)
		{
			DD_MEM_Free(pValue);
		}
		PrintExit;
		return M3U8_ERR_FAIL;
	}
	else
	{
		if((NULL == VK_strstr((const char*)pValue,"http://") && NULL == VK_strstr((const char*)pValue,"https://") && pValue[0] != M3U8_ABSOLUTE_PATH)
		&& NULL == VK_strstr((const char*)pValue,"file:/"))
		{
			pKey->key_uri = P_HLS_M3U8_MakeUrlFromBaseUrl(pPlaylist->pBaseUrl,pValue);
		}
		else if((NULL == VK_strstr((const char*)pValue,"http://") && NULL == VK_strstr((const char*)pValue,"https://") && pValue[0] == M3U8_ABSOLUTE_PATH)
		&& NULL == VK_strstr((const char*)pValue,"file:/"))
		{
			pKey->key_uri = P_HLS_M3U8_MakeUrlFromBaseUrl(m3u8->pHostUrl, pValue + 1);
		}
		else
		{
			pKey->key_uri = (HINT8*)VK_StrDup((const char*)pValue);
		}
	}
	/**
	 * 중복 체크
	 */
	pNode = UTIL_DLLIST_GetHeadNode(&pStream->keySet);
	while(pNode != NULL)
	{
		if(pNode->pData != NULL)
		{
			pTempKey = (m3u8Key_t*)pNode->pData;
			if(!VK_strcmp(pTempKey->key_uri,pKey->key_uri))
			{
				if(pKey->key_uri != NULL)
				{
					VK_MEM_Free(pKey->key_uri);
				}
				if(pKey != NULL)
				{
					VK_MEM_Free(pKey);
				}
				if(pValue != NULL)
				{
					DD_MEM_Free(pValue);
				}
				pStream->bEncrypted = TRUE;
				pPlaylist->pCurKey = pTempKey;
				PrintExit;
				return M3U8_ERR_OK;
			}
		}
		pNode = UTIL_DLLIST_NextNode(pNode);
	}
	VK_memset(pValue,0x00,nValueLen);
	if(P_HLS_M3U8_GetValue((const char*)pLine, M3U8_ATTR_IV, (char*)pValue) != M3U8_ERR_OK)
	{
//		PrintDebug("%s(%d) %s is not exist.\n",__FUNCTION__,__LINE__,M3U8_ATTR_IV);
		pKey->pIV = NULL;
	}
	else
	{
		pKey->pIV = VK_StrDup((const char*)pValue);
	}
	if(UTIL_DLLIST_Append(&pStream->keySet, pKey) != DLL_ERR_OK)
	{
		PrintError("%s(%d) Insert Key Fail\n",__FUNCTION__,__LINE__);
		if(pValue != NULL)
		{
			DD_MEM_Free(pValue);
		}
		PrintExit;
		return M3U8_ERR_FAIL;
	}
	if(pKey->eMethod != M3U8_METHOD_NONE)
	{
		pStream->bEncrypted = TRUE;
	}
	else
	{
		pStream->bEncrypted = FALSE;
	}

	pPlaylist->pCurKey = pKey;
	if(pValue != NULL)
	{
		DD_MEM_Free(pValue);
	}
//	PrintExit;
	return M3U8_ERR_OK;
}
#else
/**
 * @b Function @b Description <br>
 * KEY 정보 Parsing
 *
 * @param[in] m3u8	: m3u8 Handle
 * @param[in] pPlaylist	: 현재의 Stream 정보가 축적될 pPlaylist
 * @param[in] pLine	: Parsing 할 Line Data
 * @return  ErrCode
 */

static m3u8Err_e P_HLS_M3U8_ParseKeyInformation(m3u8_t *m3u8, m3u8Playlist_t *pPlaylist, HINT8 *pLine)
{
//	HINT8 value[M3U8_CONST_VALUE_BUFFSIZE] = {0};
	HINT8 *pValue = NULL;
	HINT32 nValueLen = 0;
	m3u8Stream_t *pStream = NULL;
	m3u8Program_t *pProgram = NULL;
	m3u8Key_t *pKey = NULL;
	m3u8Key_t *pTempKey = NULL;
	m3u8Node_t *pNode = NULL;
//	PrintEnter;
	if(m3u8 == NULL)
	{
		PrintError("%s(%d) M3U8 is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return M3U8_ERR_FAIL;
	}
	if(pLine == NULL)
	{
		PrintError("%s(%d) Line Data is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return M3U8_ERR_FAIL;
	}
	if(pPlaylist == NULL)
	{
		PrintError("%s(%d) M3U8 Url Structure is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return M3U8_ERR_FAIL;
	}
	if(pPlaylist->pStream == NULL)
	{
		/* Simple Playlist File ? */
		PrintError("%s(%d) Simple Playlist File ?\n",__FUNCTION__,__LINE__);
		pPlaylist->pStream = (m3u8Stream_t*)DD_MEM_Alloc(sizeof(m3u8Stream_t));
		if(pPlaylist->pStream == NULL)
		{
			PrintError("%s(%d) Stream Memory Allocation Fail\n",__FUNCTION__,__LINE__);
			PrintExit;
			return M3U8_ERR_FAIL;
		}
		else
		{
			VK_memset(pPlaylist->pStream,0x00,sizeof(m3u8Stream_t));
		}

		pNode = UTIL_DLLIST_GetHeadNode(&m3u8->programSet);
		while(pNode != NULL)
		{
			pProgram = (m3u8Program_t*)pNode->pData;
			if(pProgram->nProgramId == M3U8_CONST_DEFAULT_PROGRAM_ID)
			{
				break;
			}
			pNode = UTIL_DLLIST_NextNode(pNode);
		}
		if(pProgram == NULL)
		{
			pProgram = (m3u8Program_t*)DD_MEM_Alloc(sizeof(m3u8Program_t));
			if(pProgram == NULL)
			{
				PrintError("%s(%d) Program Memory Allocation Fail\n",__FUNCTION__,__LINE__);
				PrintExit;
				return M3U8_ERR_FAIL;
			}
			else
			{
				VK_memset(pProgram,0x00,sizeof(m3u8Program_t));
				pProgram->nProgramId = M3U8_CONST_DEFAULT_PROGRAM_ID;
				if(UTIL_DLLIST_Preppend(&m3u8->programSet, pProgram) != DLL_ERR_OK)
				{
					PrintError("%s(%d) Insert Program Fail\n",__FUNCTION__,__LINE__);
					PrintExit;
					return M3U8_ERR_FAIL;
				}
			}
		}
		if(UTIL_DLLIST_Preppend(&pProgram->streamSet,pPlaylist->pStream ) != DLL_ERR_OK)
		{
			PrintError("%s(%d) Insert Stream Fail\n",__FUNCTION__,__LINE__);
			PrintExit;
			return M3U8_ERR_FAIL;
		}
	}
	pStream = pPlaylist->pStream;
	if(pStream == NULL)
	{
		PrintError("%s(%d) Stream is not exist \n",__FUNCTION__,__LINE__);
		PrintExit;
		return M3U8_ERR_FAIL;
	}
	pKey = (m3u8Key_t*)DD_MEM_Alloc(sizeof(m3u8Key_t));
	if(pKey == NULL)
	{
		PrintError("%s(%d) Key Memory Allocation Fail\n",__FUNCTION__,__LINE__);
		PrintExit;
		return M3U8_ERR_FAIL;
	}
	else
	{
		VK_memset(pKey,0x00,sizeof(m3u8Key_t));
		pKey->eEncPadding = M3U8_ENCPADDING_UNKNOWN;
		pKey->eMethod = M3U8_METHOD_UNKNOWN;
	}
	nValueLen = VK_strlen(pLine) - VK_strlen(M3U8_TAG_EXT_X_KEY);
	pValue = DD_MEM_Alloc(nValueLen);
	if(pValue == NULL)
	{
		PrintError("%s(%d) Value Buffer Memory Allocation Fail\n",__FUNCTION__,__LINE__);
		if(pKey != NULL)
		{
			DD_MEM_Free(pKey);
		}
		PrintExit;
		return M3U8_ERR_FAIL;
	}
	VK_memset(pValue,0x00,nValueLen);
	if(P_HLS_M3U8_GetValue((const char*)pLine, M3U8_ATTR_URI, (char*)pValue) != M3U8_ERR_OK)
	{
		PrintError("%s(%d) %s is not exist.\n",__FUNCTION__,__LINE__,M3U8_ATTR_URI);
		// pKey->key_uri = NULL;
		if(pKey != NULL)
		{
			DD_MEM_Free(pKey);
		}
		if(pValue != NULL)
		{
			DD_MEM_Free(pValue);
		}
		PrintExit;
		return M3U8_ERR_FAIL;
	}
	else
	{
		if((NULL == VK_strstr((const char*)pValue,"http://") && NULL == VK_strstr((const char*)pValue,"https://"))
		&& NULL == VK_strstr((const char*)pValue,"file:/"))
		{
			pKey->key_uri = P_HLS_M3U8_MakeUrlFromBaseUrl(pPlaylist->pBaseUrl,pValue);
		}
		else
		{
			pKey->key_uri = (HINT8*)VK_StrDup((const char*)pValue);
		}
	}
	/**
	 * 중복 체크
	 */
	pNode = UTIL_DLLIST_GetHeadNode(&pStream->keySet);
	while(pNode != NULL)
	{
		if(pNode->pData != NULL)
		{
			pTempKey = (m3u8Key_t*)pNode->pData;
			if(!VK_strcmp(pTempKey->key_uri,pKey->key_uri))
			{
				if(pKey->key_uri != NULL)
				{
					VK_MEM_Free(pKey->key_uri);
				}
				if(pKey != NULL)
				{
					VK_MEM_Free(pKey);
				}
				if(pValue != NULL)
				{
					DD_MEM_Free(pValue);
				}
				pStream->bEncrypted = TRUE;
				pPlaylist->pCurKey = pTempKey;
				PrintExit;
				return M3U8_ERR_OK;
			}
		}
		pNode = UTIL_DLLIST_NextNode(pNode);
	}
	VK_memset(pValue,0x00,nValueLen);
	if(P_HLS_M3U8_GetValue((const char*)pLine, M3U8_ATTR_METHOD,(char*)pValue) != M3U8_ERR_OK)
	{
		PrintError("%s(%d) %s is not exist.\n",__FUNCTION__,__LINE__,M3U8_ATTR_URI);
//		pKey->eMethod = M3U8_METHOD_AESCBC128;
//		pKey->eEncPadding = M3U8_ENCPADDING_PKCS5;
	}
	else
	{
		if(VK_strstr((const char*)pValue,M3U8_STRING_AES_128) != NULL)
		{
			pKey->eMethod = M3U8_METHOD_AESCBC128;
			pKey->eEncPadding = M3U8_ENCPADDING_PKCS7;
		}
		else
		{
			PrintError("%s(%d) UNKNOWN Method %s \n",__FUNCTION__,__LINE__,pValue);
			pKey->eMethod = M3U8_METHOD_UNKNOWN;
		}
	}
	VK_memset(pValue,0x00,nValueLen);
	if(P_HLS_M3U8_GetValue((const char*)pLine, M3U8_ATTR_IV, (char*)pValue) != M3U8_ERR_OK)
	{
//		PrintDebug("%s(%d) %s is not exist.\n",__FUNCTION__,__LINE__,M3U8_ATTR_IV);
		pKey->pIV = NULL;
	}
	else
	{
		pKey->pIV = (HUINT8*)VK_StrDup((const char*)pValue);
	}
	if(UTIL_DLLIST_Append(&pStream->keySet, pKey) != DLL_ERR_OK)
	{
		PrintError("%s(%d) Insert Key Fail\n",__FUNCTION__,__LINE__);
		if(pValue != NULL)
		{
			DD_MEM_Free(pValue);
		}
		PrintExit;
		return M3U8_ERR_FAIL;
	}
	pStream->bEncrypted = TRUE;
	pPlaylist->pCurKey = pKey;
	if(pValue != NULL)
	{
		DD_MEM_Free(pValue);
	}
//	PrintExit;
	return M3U8_ERR_OK;
}

#endif

/**
 * @b Function @b Description <br>
 * Stream 정보 Parsing
 *
 * @param[in] m3u8	: m3u8 Handle
 * @param[in] pCurPlaylist	: 현재의 Stream 정보가 축적될 pPlaylist
 * @param[in] pLine	: Parsing 할 Line Data
 * @param[in] pSubLine	: Parsing 할 Sub Line Data (URL Data를 갖는 Line, Stream의 정보는 두줄의 Line으로 구성됨.)
 * @return  ErrCode
 */
static m3u8Err_e P_HLS_M3U8_ParseStreamInformation(m3u8_t *m3u8, m3u8Playlist_t *pCurPlaylist, HINT8 *pLine, HINT8 *pSubLine)
{
	HINT32 nProgramId = 0;
	HUINT32 unBandwidth = 0;
	HLS_CODEC_e eCodecs = HLS_CODEC_NONE;
	m3u8MediaType_e eMediaType = M3U8_MEDIA_AV;
	HINT8 value[M3U8_CONST_VALUE_BUFFSIZE] = {0};
	m3u8Stream_t *pStream = NULL;
	m3u8Program_t *pProgram = NULL;
	m3u8Node_t *pNode = NULL;
	m3u8Playlist_t *pPlaylist = NULL;
	PrintEnter;
	if(m3u8 == NULL)
	{
		PrintError("%s(%d) M3U8 is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return M3U8_ERR_FAIL;
	}
	if(pLine == NULL)
	{
		PrintError("%s(%d) Line Data is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return M3U8_ERR_FAIL;
	}
	if(pSubLine == NULL)
	{
		PrintError("%s(%d) SubLine Data is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return M3U8_ERR_FAIL;
	}

	/** Parse PROGRAM-ID */
	VK_memset(value,0x00,M3U8_CONST_VALUE_BUFFSIZE);
	if(P_HLS_M3U8_GetValue((const char*)pLine, M3U8_ATTR_PROGRAM_ID, (char*)value) != M3U8_ERR_OK)
	{
		PrintError("%s(%d) PROGRAM-ID is not exist.\n",__FUNCTION__,__LINE__);
		nProgramId = -1;
	}
	else
	{
		nProgramId = VK_atoi((const char*)value);
		PrintDebug("%s(%d) PROGRAM-ID = (%d).\n",__FUNCTION__,__LINE__,nProgramId);
	}
	/** Parse BANDWIDTH */
	VK_memset(value,0x00,M3U8_CONST_VALUE_BUFFSIZE);
	if(P_HLS_M3U8_GetValue((const char*)pLine, M3U8_ATTR_BANDWIDTH,(char*)value) != M3U8_ERR_OK)
	{
		PrintError("%s(%d) BANDWIDTH is not exist.\n",__FUNCTION__,__LINE__);
		unBandwidth = 0;
	}
	else
	{
		unBandwidth = strtoul((const char*)value,NULL,10);
		PrintDebug("%s(%d) BANDWIDTH = (%u).\n",__FUNCTION__,__LINE__,unBandwidth);
	}
	/** Parse CODEC */
	VK_memset(value,0x00,M3U8_CONST_VALUE_BUFFSIZE);
	if(P_HLS_M3U8_GetValue((const char*)pLine, M3U8_ATTR_CODECS,(char*)value) != M3U8_ERR_OK)
	{
		PrintError("%s(%d) CODECS is not exist.\n",__FUNCTION__,__LINE__);
		eCodecs = HLS_CODEC_NONE;
	}
	else
	{
		PrintDebug("%s(%d) CODECS = (%s).\n",__FUNCTION__,__LINE__,value);
		eCodecs = P_HLS_ParseCodec(value);
	}
	if(HLS_M3U8_GetParserVersion() >= 4)
	{
		VK_memset(value,0x00,M3U8_CONST_VALUE_BUFFSIZE);
		if(P_HLS_M3U8_GetValue((const char*)pLine, M3U8_ATTR_CODECS,(char*)value) != M3U8_ERR_OK)
		{
			PrintDebug("%s(%d) M3U8_ATTR_CODECS is not exist.\n",__FUNCTION__,__LINE__);
		}
		else
		{
			PrintDebug("%s(%d) M3U8_ATTR_CODECS = (%s).\n",__FUNCTION__,__LINE__,value);
		}
		VK_memset(value,0x00,M3U8_CONST_VALUE_BUFFSIZE);
		if(P_HLS_M3U8_GetValue((const char*)pLine, M3U8_ATTR_RESOLUTION,(char*)value) != M3U8_ERR_OK)
		{
			PrintDebug("%s(%d) M3U8_ATTR_RESOLUTION is not exist.\n",__FUNCTION__,__LINE__);
		}
		else
		{
			PrintDebug("%s(%d) M3U8_ATTR_RESOLUTION = (%s).\n",__FUNCTION__,__LINE__,value);
		}
		VK_memset(value,0x00,M3U8_CONST_VALUE_BUFFSIZE);
		if(P_HLS_M3U8_GetValue((const char*)pLine, M3U8_ATTR_VIDEO,(char*)value) != M3U8_ERR_OK)
		{
			PrintDebug("%s(%d) M3U8_ATTR_VIDEO is not exist.\n",__FUNCTION__,__LINE__);
		}
		else
		{
			eMediaType = M3U8_MEDIA_VIDEO;
			PrintDebug("%s(%d) M3U8_ATTR_VIDEO = (%u).\n",__FUNCTION__,__LINE__,unBandwidth);
		}
		VK_memset(value,0x00,M3U8_CONST_VALUE_BUFFSIZE);
		if(P_HLS_M3U8_GetValue((const char*)pLine, M3U8_ATTR_AUDIO,(char*)value) != M3U8_ERR_OK)
		{
			PrintDebug("%s(%d) M3U8_ATTR_AUDIO is not exist.\n",__FUNCTION__,__LINE__);
		}
		else
		{
			eMediaType = M3U8_MEDIA_AUDIO;
			PrintDebug("%s(%d) M3U8_ATTR_AUDIO = (%u).\n",__FUNCTION__,__LINE__,unBandwidth);
		}
	}
	if(NULL == VK_strstr((const char*)pSubLine,".m3u8"))
	{
		PrintError("%s(%d) Stream Url is not exist.\n",__FUNCTION__,__LINE__);
		PrintExit;
		return M3U8_ERR_FAIL;
	}
	pNode = UTIL_DLLIST_GetHeadNode(&m3u8->programSet);
	while(pNode != NULL)
	{
		pProgram = (m3u8Program_t*)pNode->pData;
		if(pProgram->nProgramId == nProgramId)
		{
			break;
		}
		pNode = UTIL_DLLIST_NextNode(pNode);
	}
	if(pProgram == NULL)
	{
		pProgram = (m3u8Program_t*)DD_MEM_Alloc(sizeof(m3u8Program_t));
		if (pProgram == NULL)
		{
			PrintError("%s(%d) Program Memory Allocation Fail\n",__FUNCTION__,__LINE__);
			PrintExit;
			return M3U8_ERR_FAIL;
		}
		else
		{
			VK_memset(pProgram,0x00,sizeof(m3u8Program_t));
			pProgram->nProgramId = nProgramId;
			if(UTIL_DLLIST_Preppend(&m3u8->programSet, pProgram) != DLL_ERR_OK)
			{
				PrintError("%s(%d) Insert Program Fail\n",__FUNCTION__,__LINE__);
				PrintExit;
				return M3U8_ERR_FAIL;
			}
		}
	}
	pStream = (m3u8Stream_t*)DD_MEM_Alloc(sizeof(m3u8Stream_t));
	if (pStream == NULL)
	{
		PrintError("%s(%d) Stream Memory Allocation Fail\n",__FUNCTION__,__LINE__);
		PrintExit;
		return M3U8_ERR_FAIL;
	}
	else
	{
		VK_memset(pStream,0x00,sizeof(m3u8Stream_t));
		pStream->unBandwidth = unBandwidth;
		pStream->eMediaType = eMediaType;
		pStream->eCodec = eCodecs;
		if(UTIL_DLLIST_Insert(&pProgram->streamSet,pStream,pStream->unBandwidth) != DLL_ERR_OK)
		{
			PrintError("%s(%d) Insert Stream Fail\n",__FUNCTION__,__LINE__);
			PrintExit;
			return M3U8_ERR_FAIL;
		}
	}

	pPlaylist = (m3u8Playlist_t*)DD_MEM_Alloc(sizeof(m3u8Playlist_t));
	if (pPlaylist == NULL)
	{
		PrintError("%s(%d) UrlNode Memory Allocation Fail\n",__FUNCTION__,__LINE__);
		PrintExit;
		return M3U8_ERR_FAIL;
	}
	else
	{
		VK_memset(pPlaylist,0x00,sizeof(m3u8Playlist_t));
		if((NULL == VK_strstr((const char*)pSubLine,"http://") && NULL == VK_strstr((const char*)pSubLine,"https://") && pSubLine[0] != M3U8_ABSOLUTE_PATH)
		&& NULL == VK_strstr((const char*)pSubLine,"file:/"))
		{
			pPlaylist->pUrl = P_HLS_M3U8_MakeUrlFromBaseUrl(pCurPlaylist->pBaseUrl,pSubLine);
		}
		else if((NULL == VK_strstr((const char*)pSubLine,"http://") && NULL == VK_strstr((const char*)pSubLine,"https://") && pSubLine[0] == M3U8_ABSOLUTE_PATH)
		&& NULL == VK_strstr((const char*)pSubLine,"file:/"))
		{
			pPlaylist->pUrl = P_HLS_M3U8_MakeUrlFromBaseUrl(m3u8->pHostUrl, pSubLine + 1);
		}
		else
		{
			pPlaylist->pUrl = (HINT8*)VK_StrDup((const char*)pSubLine);
		}
		pPlaylist->pBaseUrl = P_HLS_M3U8_GetAndFillBaseURL(pPlaylist->pUrl);
		pPlaylist->pStream = pStream;
		pPlaylist->nProgramId = nProgramId;
	}
	if(UTIL_DLLIST_Preppend(&m3u8->playlistSet, pPlaylist) != DLL_ERR_OK)
	{
		PrintError("%s(%d) Insert Stream Fail\n",__FUNCTION__,__LINE__);
		PrintExit;
		return M3U8_ERR_FAIL;
	}
	PrintExit;
	return M3U8_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Segment 정보 Parsing
 *
 * @param[in] m3u8	: m3u8 Handle
 * @param[in] pPlaylist	: 현재의 Segment 정보가 축적될 pPlaylist
 * @param[in] pLine	: Parsing 할 Line Data
 * @param[in] pSubLine	: Parsing 할 Sub Line Data (URL Data를 갖는 Line, Segment의 정보는 두줄의 Line으로 구성됨.)
 * @return  ErrCode
 */

#if 1 // __SUPPORT_BYTERANGE__
static m3u8Err_e P_HLS_M3U8_ParseSegmentInformation(m3u8_t *m3u8, m3u8Playlist_t *pPlaylist, HINT8 *pLine, HINT8 *pSubLine, HBOOL bDiscontinuity, HLONG utc, HINT8 *pByteRange)
#else
static m3u8Err_e P_HLS_M3U8_ParseSegmentInformation(m3u8_t *m3u8, m3u8Playlist_t *pPlaylist, HINT8 *pLine, HINT8 *pSubLine, HBOOL bDiscontinuity, HLONG utc )
#endif
{
	m3u8Stream_t *pStream = NULL;
	m3u8Segment_t *pSegment = NULL;
	m3u8Segment_t *pTailSegment = NULL;
	m3u8Program_t *pProgram = NULL;
	m3u8Node_t *pNode = NULL;
	m3u8Node_t *pTailSegmentNode = NULL;
	HFLOAT32 fStartTime = 0;
	HBOOL bCanAppend = FALSE;
	HBOOL bResetSequence = FALSE;
	HINT8 value[M3U8_CONST_VALUE_BUFFSIZE] = {0};
#if 1 // __SUPPORT_BYTERANGE__
	HINT64 ullByteRangeLength = 0;
	HINT64 ullByteRangeOffset = 0;
	HINT8 *pByteOffset = NULL;
#endif
//	PrintEnter;
	if(m3u8 == NULL)
	{
		PrintError("%s(%d) M3U8 is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return M3U8_ERR_FAIL;
	}
	if(pLine == NULL)
	{
		PrintError("%s(%d) Line Data is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return M3U8_ERR_FAIL;
	}
	if(pSubLine == NULL)
	{
		PrintError("%s(%d) SubLine Data is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return M3U8_ERR_FAIL;
	}
	if(pPlaylist == NULL)
	{
		PrintError("%s(%d) M3U8 Url Structure is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return M3U8_ERR_FAIL;
	}
	if(pPlaylist->pStream == NULL)
	{
		/* Simple Playlist File ? */
		PrintError("%s(%d) Simple Playlist File ?\n",__FUNCTION__,__LINE__);
		pPlaylist->pStream = (m3u8Stream_t*)DD_MEM_Alloc(sizeof(m3u8Stream_t));
		if(pPlaylist->pStream == NULL)
		{
			PrintError("%s(%d) Stream Memory Allocation Fail\n",__FUNCTION__,__LINE__);
			PrintExit;
			return M3U8_ERR_FAIL;
		}
		else
		{
			VK_memset(pPlaylist->pStream,0x00,sizeof(m3u8Stream_t));
		}

		pNode = UTIL_DLLIST_GetHeadNode(&m3u8->programSet);
		while(pNode != NULL)
		{
			pProgram = (m3u8Program_t*)pNode->pData;
			if(pProgram->nProgramId == M3U8_CONST_DEFAULT_PROGRAM_ID)
			{
				break;
			}
			pNode = UTIL_DLLIST_NextNode(pNode);
		}
		if(pProgram == NULL)
		{
			pProgram = (m3u8Program_t*)DD_MEM_Alloc(sizeof(m3u8Program_t));
			if(pProgram == NULL)
			{
				PrintError("%s(%d) Program Memory Allocation Fail\n",__FUNCTION__,__LINE__);
				PrintExit;
				return M3U8_ERR_FAIL;
			}
			else
			{
				VK_memset(pProgram,0x00,sizeof(m3u8Program_t));
				pProgram->nProgramId = M3U8_CONST_DEFAULT_PROGRAM_ID;
				if(UTIL_DLLIST_Preppend(&m3u8->programSet, pProgram) != DLL_ERR_OK)
				{
					PrintError("%s(%d) Insert Program Fail\n",__FUNCTION__,__LINE__);
					PrintExit;
					return M3U8_ERR_FAIL;
				}
			}
		}
		if(UTIL_DLLIST_Preppend(&pProgram->streamSet,pPlaylist->pStream ) != DLL_ERR_OK)
		{
			PrintError("%s(%d) Insert Stream Fail\n",__FUNCTION__,__LINE__);
			PrintExit;
			return M3U8_ERR_FAIL;
		}
	}
	pStream = pPlaylist->pStream;
	if(pStream == NULL)
	{
		PrintError("%s(%d) Stream is not exist \n",__FUNCTION__,__LINE__);
		PrintExit;
		return M3U8_ERR_FAIL;
	}
	VK_memset(value,0x00,M3U8_CONST_VALUE_BUFFSIZE);
	if(P_HLS_M3U8_GetValue((const char*)pLine, M3U8_TAG_EXTINF, (char*)value) != M3U8_ERR_OK)
	{
		PrintError("%s(%d) M3U8_TAG_EXTINF is not exist.\n",__FUNCTION__,__LINE__);
		PrintExit;
		return M3U8_ERR_FAIL;
	}
	pTailSegmentNode = UTIL_DLLIST_GetTailNode(&pStream->segmentSet);
	if(pTailSegmentNode == NULL)
	{
		fStartTime = 0;
		pStream->ullSequenceNum = pPlaylist->ullSequenceNum;
		bCanAppend = TRUE;
	}
	else
	{
		pTailSegment = pTailSegmentNode->pData;
		if(pTailSegment != NULL)
	{
			if(pTailSegment->ullNum < pPlaylist->ullSequenceNum + pPlaylist->unSegmentNum)
		{
				if((pTailSegment->ullNum + 1) != pPlaylist->ullSequenceNum + pPlaylist->unSegmentNum)
				{
					PrintError("%s(%d)Sequence is Broken!! %llu(%llu _[B]%u):%llu(%llu _[B]%u)\n",
							__FUNCTION__,
							__LINE__,
							pTailSegment->ullNum,
							pStream->ullSequenceNum,
							pStream->unBandwidth,
							pPlaylist->ullSequenceNum + pPlaylist->unSegmentNum,
							pPlaylist->ullSequenceNum,
							pPlaylist->pStream->unBandwidth);
				}
				fStartTime = pTailSegment->starttime_of_segment + pTailSegment->duration_of_segment;
				bCanAppend = TRUE;
		}
			else if( pPlaylist->ullSequenceNum < pStream->ullSequenceNum && pPlaylist->ullSequenceNum < pStream->ullLastSequenceNum )
		{
				PrintError("%s(%d) [YKKIM] Sequence Number has Reset (Prev PlaylistSeq %llu -> CurPlaylistSeq %llu )\n",__FUNCTION__,__LINE__, pStream->ullLastSequenceNum, pPlaylist->ullSequenceNum);
				fStartTime = pTailSegment->starttime_of_segment + pTailSegment->duration_of_segment;
			bCanAppend = TRUE;
				bResetSequence = TRUE;
			}
		}
	}
	pStream->ullLastSequenceNum = pPlaylist->ullSequenceNum;
	if(bCanAppend == TRUE)
	{
		pSegment = (m3u8Segment_t*)DD_MEM_Alloc(sizeof(m3u8Segment_t));
		if(pSegment == NULL)
		{
			PrintError("%s(%d) Segment Memory Allocation Fail\n",__FUNCTION__,__LINE__);
			PrintExit;
			return M3U8_ERR_FAIL;
		}
		else
		{
			VK_memset(pSegment,0x00,sizeof(m3u8Segment_t));
			pSegment->datetime = utc;
			pSegment->bDiscontinuity = bDiscontinuity;
			pSegment->bResetSequence = bResetSequence;
			pSegment->duration_of_segment = atof((const char*)value);
			pSegment->starttime_of_segment = fStartTime;
	//		pPlaylist->unDuration += pSegment->duration_of_segment;
			pSegment->pKey = pPlaylist->pCurKey;
			if(pPlaylist->pCurKey != NULL && pPlaylist->pCurKey->key_uri != NULL)
				pPlaylist->pCurKey->uUsedCount++;
			pSegment->ullNum = pPlaylist->ullSequenceNum + pPlaylist->unSegmentNum;
			if((NULL == VK_strstr((const char*)pSubLine,"http://") && NULL == VK_strstr((const char*)pSubLine,"https://") && pSubLine[0] != M3U8_ABSOLUTE_PATH)
			&& NULL == VK_strstr((const char*)pSubLine,"file:/"))
			{
				pSegment->segment_url = P_HLS_M3U8_MakeUrlFromBaseUrl(pPlaylist->pBaseUrl,pSubLine);
			}
			else if((NULL == VK_strstr((const char*)pSubLine,"http://") && NULL == VK_strstr((const char*)pSubLine,"https://") && pSubLine[0] == M3U8_ABSOLUTE_PATH)
			&& NULL == VK_strstr((const char*)pSubLine,"file:/"))
			{
				pSegment->segment_url = P_HLS_M3U8_MakeUrlFromBaseUrl(m3u8->pHostUrl, pSubLine + 1);
			}
			else
			{
				pSegment->segment_url = (HINT8*)VK_StrDup((const char*)pSubLine);
			}
#if 1 // __SUPPORT_BYTERANGE__
			if(NULL != pByteRange)
			{
				VK_memset(value,0x00,M3U8_CONST_VALUE_BUFFSIZE);
				if(P_HLS_M3U8_GetValue((const char*)pByteRange, M3U8_TAG_EXT_X_BYTERANGE, (char*)value) != M3U8_ERR_OK)
				{
					PrintError("%s(%d) M3U8_TAG_EXT_X_BYTERANGE is not exist.\n",__FUNCTION__,__LINE__);
					PrintExit;
					return M3U8_ERR_FAIL;
				}
				else
				{
					/* @ 를 기준으로 ByteRagne Length, Offset 값을 파싱. */
					pByteOffset = VK_strstr((const char*)value,"@");
					if(NULL != pByteOffset)
					{
						PrintError("%s(%d) ByteRange has length & offset\n",__FUNCTION__,__LINE__);
						ullByteRangeLength = VK_atoi((const char*)value);
						ullByteRangeOffset = VK_atoi((const char*)(pByteOffset+1));
						PrintError("%s(%d) Offset OK : ullByteRangeLength [ %llu ], ullByteRangeOffset [ %llu ]\n"
						,__FUNCTION__,__LINE__, ullByteRangeLength, ullByteRangeOffset);
					}
					else
					{
						ullByteRangeLength = VK_atoi((const char*)value);
						ullByteRangeOffset = pTailSegment->ullByteRangeLength + pTailSegment->ullByteRangeOffset;
						PrintError("%s(%d) Offset NO : ullByteRangeLength [ %llu ], ullByteRangeOffset [ %llu ]\n"
						,__FUNCTION__,__LINE__, ullByteRangeLength, ullByteRangeOffset );
					}
					pSegment->bByteRangeSupport = TRUE;
					pSegment->ullByteRangeLength = ullByteRangeLength;
					pSegment->ullByteRangeOffset = ullByteRangeOffset;
					if(pSegment->ullByteRangeOffset == 0 || pTailSegment == NULL)
					{
						pSegment->ullByteRangeIndex = 0;
					}
					else
					{
						if(!VK_strcmp(pTailSegment->segment_url, pSegment->segment_url))
						{
							pSegment->ullByteRangeIndex = pTailSegment->ullByteRangeIndex+1;
						}
					}
				}
			}
#endif
		}
		if(UTIL_DLLIST_Append(&pStream->segmentSet, pSegment) != DLL_ERR_OK)
		{
			PrintError("%s(%d) Insert Stream Fail\n",__FUNCTION__,__LINE__);
			PrintExit;
			return M3U8_ERR_FAIL;
		}
		pPlaylist->bUpdated = TRUE;
	}
//	PrintExit;
	return M3U8_ERR_OK;
}

 /**
  * @b Function @b Description <br>
  * TOP M3U8 전체를  Print 하는 함수
  *
  * @param[in] topm3u8	: Top M3U8 Strcut 의 포인터
  *
  * @return  #  VOID <br>

  */
void HLS_M3U8_Print(m3u8_t *m3u8)
{
	m3u8Node_t *pProgNode = NULL;
	m3u8Node_t *pStreamNode = NULL;
	m3u8Node_t *pKeyNode = NULL;
	m3u8Node_t *pSegNode = NULL;
	m3u8Program_t *pProgram = NULL;
	m3u8Stream_t *pStream = NULL;
	m3u8Key_t *pKey = NULL;
	m3u8Segment_t *pSegment = NULL;
	HINT32 nProgramSize = 0;
	HINT32 nStreamSize = 0;
	HINT32 nKeySize = 0;
	HINT32 nSegSize = 0;
//	PrintDebug("top_m3u8_url = %s\n", topm3u8->top_m3u8_url);
//	PrintDebug("base_url = %s\n", topm3u8->base_url);
	PrintEnter;
	PrintDebug("*---- M3U8 >>\n");
	PrintDebug("!---- TOP Url = %s \n",m3u8->pTopUrl);
//	PrintDebug("!---- Base Url = %s \n",m3u8->pBaseUrl);
	PrintDebug("!---- AllowCache = %d \n",m3u8->bAllowCache);

	PrintDebug("*---- Program >>\n");
	nProgramSize = UTIL_DLLIST_Size(&m3u8->programSet);
	PrintDebug("!---- Total Program = %d \n",nProgramSize);
	pProgNode = UTIL_DLLIST_GetHeadNode(&m3u8->programSet);
	while(pProgNode != NULL)
	{
		pProgram = (m3u8Program_t*)pProgNode->pData;
		PrintDebug(">>--- Program (%d) >>\n",pProgram->nProgramId);
		nStreamSize = UTIL_DLLIST_Size(&pProgram->streamSet);
		PrintDebug("!---- Total Stream = %d \n",nStreamSize);
		pStreamNode = UTIL_DLLIST_GetHeadNode(&pProgram->streamSet);
		while(pStreamNode != NULL)
		{
			pStream = (m3u8Stream_t*)pStreamNode->pData;
			PrintDebug(">>>-- Stream (Bandwidth = %u) >>\n",pStream->unBandwidth);
//			PrintDebug("!---- Stream Duration = %u \n",pStream->unStreamDuration);
			PrintDebug("!---- Target Duration = %u \n",pStream->unTargetDuration);
			PrintDebug("!---- SequenceNumber = %"PRId64" \n",pStream->ullSequenceNum);
			nKeySize = UTIL_DLLIST_Size(&pStream->keySet);
			PrintDebug("!---- Total Key = %d \n",nKeySize);
			pKeyNode = UTIL_DLLIST_GetHeadNode(&pStream->keySet);
			while(pKeyNode != NULL)
			{
				pKey = (m3u8Key_t*)pKeyNode->pData;
				PrintDebug(">>>>- Key (URI = %s) >>\n",pKey->key_uri);
				PrintDebug("!---- Method = %u \n",pKey->eMethod);
				PrintDebug("!---- IV = %s \n",pKey->pIV);
				PrintDebug("!---- Used Count = %d \n", pKey->uUsedCount);
				pKeyNode = UTIL_DLLIST_NextNode(pKeyNode);
			}
			PrintDebug("END-- KEY \n");
			nSegSize = UTIL_DLLIST_Size(&pStream->segmentSet);
			PrintDebug("!---- Total Segment = %d \n",nSegSize);
			pSegNode = UTIL_DLLIST_GetHeadNode(&pStream->segmentSet);
			while(pSegNode != NULL)
			{
				pSegment = (m3u8Segment_t*)pSegNode->pData;
				PrintDebug(">>>>- Segment (%"PRId64") >>\n",pSegment->ullNum);
				PrintDebug("!---- Discontinuity = %d \n",pSegment->bDiscontinuity);
				PrintDebug("!---- DateTime = %ld \n",pSegment->datetime);
				PrintDebug("!---- URL = %s \n",pSegment->segment_url);
				PrintDebug("!---- Duration = %lf \n",pSegment->duration_of_segment);
				PrintDebug("!---- Start Time = %lf \n",pSegment->starttime_of_segment);
#if 1 // __SUPPORT_BYTERANGE__
				PrintDebug("!------ ByteRangeSupport = %s \n",(pSegment->bByteRangeSupport == TRUE?"TRUE":"FALSE"));
				PrintDebug("!------ Byte Length      = %llu \n",pSegment->ullByteRangeLength);
				PrintDebug("!------ Byte Offset      = %llu \n",pSegment->ullByteRangeOffset);
				PrintDebug("!------ Byte Index       = %llu \n",pSegment->ullByteRangeIndex);
#endif
				if(pSegment->pKey != NULL)
				{
					PrintDebug("!---- Key URI = %s \n",pSegment->pKey->key_uri);
					PrintDebug("!---- IV = %s \n",pSegment->pKey->pIV);
				}
				pSegNode = UTIL_DLLIST_NextNode(pSegNode);
			}
			PrintDebug("END-- SEGMENT \n");
			pStreamNode = UTIL_DLLIST_NextNode(pStreamNode);
		}
		PrintDebug("END-- STREAM \n");
		pProgNode = UTIL_DLLIST_NextNode(pProgNode);
	}
	PrintDebug("END-- PROGRAM \n");
	PrintExit;
}

m3u8Err_e HLS_M3U8_Destroy(m3u8_t *m3u8)
{
	m3u8Node_t *pProgNode = NULL;
	m3u8Node_t *pStreamNode = NULL;
	m3u8Node_t *pKeyNode = NULL;
	m3u8Node_t *pSegNode = NULL;
	m3u8Node_t *pPlayNode = NULL;
	m3u8Program_t *pProgram = NULL;
	m3u8Stream_t *pStream = NULL;
	m3u8Key_t *pKey = NULL;
	m3u8Segment_t *pSegment = NULL;
	m3u8Playlist_t *pPlaylist = NULL;
	PrintEnter;
	if(m3u8 == NULL)
	{
		PrintError("%s(%d) M3U8 Data is not exist\n",__FUNCTION__,__LINE__);
		PrintExit;
		return M3U8_ERR_FAIL;

	}
	if(m3u8->pHostUrl != NULL)
	{
		DD_MEM_Free(m3u8->pHostUrl);
	}
	if(m3u8->pTopUrl != NULL)
	{
		DD_MEM_Free(m3u8->pTopUrl);
	}
	pProgNode = UTIL_DLLIST_GetTailNode(&m3u8->programSet);
	while(pProgNode != NULL)
	{
		pProgram = (m3u8Program_t*)pProgNode->pData;
		pStreamNode = UTIL_DLLIST_GetTailNode(&pProgram->streamSet);
		while(pStreamNode != NULL)
		{
			pStream = (m3u8Stream_t*)pStreamNode->pData;
			if(pStream == NULL)
			{
				PrintError("%s(%d) Stream Data is not exist\n",__FUNCTION__,__LINE__);
				PrintExit;
				return M3U8_ERR_FAIL;
			}
			pKeyNode = UTIL_DLLIST_GetTailNode(&pStream->keySet);
			while(pKeyNode != NULL)
			{
				pKey = (m3u8Key_t*)pKeyNode->pData;
				if(pKey == NULL)
				{
					PrintError("%s(%d) Key Data is not exist\n",__FUNCTION__,__LINE__);
					PrintExit;
					return M3U8_ERR_FAIL;
				}
				if(pKey->key_uri != NULL)
				{
					DD_MEM_Free(pKey->key_uri);
				}
				if(pKey->pIV!= NULL)
				{
					DD_MEM_Free(pKey->pIV);
				}
				if(pKey != NULL)
				{
					DD_MEM_Free(pKey);
					pKeyNode->pData = NULL;
					pKey = NULL;
				}
				pKeyNode = UTIL_DLLIST_PrevNode(pKeyNode);
			}
			if(UTIL_DLLIST_Reset(&pStream->keySet) != DLL_ERR_OK)
			{
				PrintError("%s(%d) didn't clean KeySet. Key Data is exist\n",__FUNCTION__,__LINE__);
				PrintExit;
				return M3U8_ERR_FAIL;
			}
			pSegNode = UTIL_DLLIST_GetTailNode(&pStream->segmentSet);
			while(pSegNode != NULL)
			{
				pSegment = (m3u8Segment_t*)pSegNode->pData;
				if(pSegment == NULL)
				{
					PrintError("%s(%d) Segment Data is not exist\n",__FUNCTION__,__LINE__);
					PrintExit;
					return M3U8_ERR_FAIL;
				}
				if(pSegment->segment_url!= NULL)
				{
					DD_MEM_Free(pSegment->segment_url);
				}
				if(pSegment->backup_url != NULL)
				{
					DD_MEM_Free(pSegment->backup_url);
				}
				if(pSegment != NULL)
				{
					DD_MEM_Free(pSegment);
					pSegNode->pData = NULL;
					pSegment = NULL;
				}
				pSegNode = UTIL_DLLIST_PrevNode(pSegNode);
			}
			if(UTIL_DLLIST_Reset(&pStream->segmentSet) != DLL_ERR_OK)
			{
				PrintError("%s(%d) didn't clean Segment Set. Segment Data is exist\n",__FUNCTION__,__LINE__);
				PrintExit;
				return M3U8_ERR_FAIL;
			}
			if(pStream != NULL)
			{
				DD_MEM_Free(pStream);
				pStreamNode->pData = NULL;
				pStream = NULL;
			}
			pStreamNode = UTIL_DLLIST_PrevNode(pStreamNode);
		}
		if(pProgram == NULL)
		{
			PrintError("%s(%d) Program Data is not exist\n",__FUNCTION__,__LINE__);
			PrintExit;
			return M3U8_ERR_FAIL;
		}
		if(UTIL_DLLIST_Reset(&pProgram->streamSet) != DLL_ERR_OK)
		{
			PrintError("%s(%d) didn't clean Stream Set. Stream Data is exist\n",__FUNCTION__,__LINE__);
			PrintExit;
			return M3U8_ERR_FAIL;
		}
		if(pProgram != NULL)
		{
			DD_MEM_Free(pProgram);
			pProgNode->pData = NULL;
			pProgram = NULL;
		}
		pProgNode = UTIL_DLLIST_PrevNode(pProgNode);
	}
	if(UTIL_DLLIST_Reset(&m3u8->programSet) != DLL_ERR_OK)
	{
		PrintError("%s(%d) didn't clean Program Set. Program Data is exist\n",__FUNCTION__,__LINE__);
		PrintExit;
		return M3U8_ERR_FAIL;
	}

	pPlayNode = UTIL_DLLIST_GetTailNode(&m3u8->playlistSet);
	while(pPlayNode != NULL)
	{
		pPlaylist= (m3u8Playlist_t*)pPlayNode->pData;
		if(pPlaylist == NULL)
                {
                        PrintError("%s(%d) Playlist Data is not exist\n",__FUNCTION__,__LINE__);
                        PrintExit;
                        return M3U8_ERR_FAIL;
                }
		if(pPlaylist->pUrl != NULL)
		{
			DD_MEM_Free(pPlaylist->pUrl);
		}
		if(pPlaylist->pBaseUrl != NULL)
		{
			DD_MEM_Free(pPlaylist->pBaseUrl);
		}
		if(pPlaylist != NULL)
		{
			DD_MEM_Free(pPlaylist);
			pPlayNode->pData = NULL;
			pPlaylist = NULL;
		}
		pPlayNode = UTIL_DLLIST_PrevNode(pPlayNode);
	}
	if(UTIL_DLLIST_Reset(&m3u8->playlistSet) != DLL_ERR_OK)
	{
		PrintError("%s(%d) didn't clean Playlist Set. Playlist Data is exist\n",__FUNCTION__,__LINE__);
		PrintExit;
		return M3U8_ERR_FAIL;
	}
	if(m3u8 != NULL)
	{
		DD_MEM_Free(m3u8);
		m3u8 = NULL;
	}
	PrintExit;
	return M3U8_ERR_OK;
}

m3u8Err_e HLS_M3U8_DiscardStream(m3u8Node_t *pStreamNode)
{
	m3u8Node_t *pKeyNode = NULL;
	m3u8Node_t *pSegNode = NULL;
	m3u8Stream_t *pStream = NULL;
	m3u8Key_t *pKey = NULL;
	m3u8Segment_t *pSegment = NULL;

	if(pStreamNode != NULL)
	{
		pStream = (m3u8Stream_t*)pStreamNode->pData;
		if(pStream == NULL)
		{
			PrintError("%s(%d) Stream Data is not exist\n",__FUNCTION__,__LINE__);
			PrintExit;
			return M3U8_ERR_FAIL;
		}
		pKeyNode = UTIL_DLLIST_GetTailNode(&pStream->keySet);
		while(pKeyNode != NULL)
		{
			pKey = (m3u8Key_t*)pKeyNode->pData;
			if(pKey == NULL)
			{
				PrintError("%s(%d) Key Data is not exist\n",__FUNCTION__,__LINE__);
				PrintExit;
				return M3U8_ERR_FAIL;
			}
			if(pKey->key_uri != NULL)
			{
				DD_MEM_Free(pKey->key_uri);
			}
			if(pKey->pIV != NULL)
			{
				DD_MEM_Free(pKey->pIV);
			}
			if(pKey != NULL)
			{
				DD_MEM_Free(pKey);
				pKeyNode->pData = NULL;
				pKey = NULL;
			}
			pKeyNode = UTIL_DLLIST_PrevNode(pKeyNode);
		}
		if(UTIL_DLLIST_Reset(&pStream->keySet) != DLL_ERR_OK)
		{
			PrintError("%s(%d) didn't clean KeySet. Key Data is exist\n",__FUNCTION__,__LINE__);
			PrintExit;
			return M3U8_ERR_FAIL;
		}
		pSegNode = UTIL_DLLIST_GetTailNode(&pStream->segmentSet);
		while(pSegNode != NULL)
		{
			pSegment = (m3u8Segment_t*)pSegNode->pData;
			if(pSegment == NULL)
			{
				PrintError("%s(%d) Segment Data is not exist\n",__FUNCTION__,__LINE__);
				PrintExit;
				return M3U8_ERR_FAIL;
			}
			if(pSegment->segment_url!= NULL)
			{
				DD_MEM_Free(pSegment->segment_url);
			}
			if(pSegment->backup_url != NULL)
			{
				DD_MEM_Free(pSegment->backup_url);
			}
			if(pSegment != NULL)
			{
				DD_MEM_Free(pSegment);
				pSegNode->pData = NULL;
				pSegment = NULL;
			}
			pSegNode = UTIL_DLLIST_PrevNode(pSegNode);
		}
		if(UTIL_DLLIST_Reset(&pStream->segmentSet) != DLL_ERR_OK)
		{
			PrintError("%s(%d) didn't clean Segment Set. Segment Data is exist\n",__FUNCTION__,__LINE__);
			PrintExit;
			return M3U8_ERR_FAIL;
		}
		if(pStream != NULL)
		{
			DD_MEM_Free(pStream);
			pStreamNode->pData = NULL;
			pStream = NULL;
		}
		if(pStreamNode != NULL)
		{
			DD_MEM_Free(pStreamNode);
			pStreamNode = NULL;
		}
	}
	PrintExit;
	return M3U8_ERR_OK;
}

m3u8Err_e HLS_M3U8_DiscardSegments(m3u8Stream_t *pStream, HUINT64 ullNum)
{
	m3u8Node_t *pExtractedNode = NULL;
	m3u8Node_t *pSegNode = NULL;
	m3u8Segment_t *pSegment = NULL;
	HUINT32 unNode = 0;
	if(pStream == NULL)
	{
		return M3U8_ERR_FAIL;
	}
	if( ullNum < pStream->ullSequenceNum )
	{
		return M3U8_ERR_FAIL;
	}
	unNode = ullNum - pStream->ullSequenceNum;
	pSegNode = UTIL_DLLIST_Extract_toStart(&pStream->segmentSet, unNode);
	if(pSegNode == NULL)
	{
		return M3U8_ERR_FAIL;
	}
	pExtractedNode = pSegNode;
	while(pSegNode != NULL)
	{
		pSegment = (m3u8Segment_t*)pSegNode->pData;
		if(pSegment == NULL)
		{
			PrintError("%s(%d) Segment Data is not exist\n",__FUNCTION__,__LINE__);
			PrintExit;
			return M3U8_ERR_FAIL;
		}
		if(pSegment->segment_url!= NULL)
		{
			DD_MEM_Free(pSegment->segment_url);
		}
		if(pSegment->backup_url != NULL)
		{
			DD_MEM_Free(pSegment->backup_url);
		}
		if(pSegment != NULL)
		{
			DD_MEM_Free(pSegment);
			pSegNode->pData = NULL;
			pSegment = NULL;
		}
		pSegNode = UTIL_DLLIST_NextNode(pSegNode);
	}
	UTIL_DLLIST_DeleteNodes(pExtractedNode);
	pExtractedNode = NULL;
	return M3U8_ERR_OK;
}

m3u8Err_e HLS_M3U8_DiscardKey(m3u8Stream_t *pStream, HINT8* pKeyUri)
{
	HUINT32 unNode = 0;
	HBOOL bIsDiscard = FALSE;
	m3u8Key_t *pKey = NULL;
	m3u8Node_t *pKeyNode = NULL;

	if(pStream == NULL || pKeyUri == NULL)
	{
		PrintError("%s(%d) Discard key failed\n", __FUNCTION__, __LINE__);
		return M3U8_ERR_FAIL;
	}

	pKeyNode = UTIL_DLLIST_GetHeadNode(&pStream->keySet);
	if(pKeyNode == NULL)
	{
		PrintError("%s(%d) Discard key failed\n", __FUNCTION__, __LINE__);
		return M3U8_ERR_FAIL;
	}

	while(pKeyNode != NULL)
	{
		pKey = (m3u8Key_t*)pKeyNode->pData;
		if(pKey != NULL && pKey->key_uri != NULL)
		{
			if(!VK_strcmp(pKey->key_uri, pKeyUri))
			{
				bIsDiscard = TRUE;
				break;
			}
		}
		unNode++;
		pKeyNode = UTIL_DLLIST_NextNode(pKeyNode);
	}

	if(bIsDiscard)
	{
		pKeyNode = UTIL_DLLIST_Extract_n_Node(&pStream->keySet, unNode);
		if(pKeyNode)
		{
			 pKey = (m3u8Key_t*)pKeyNode->pData;
			 if(pKey)
			 {
			 	if(pKey->pIV != NULL)
			 	{
					DD_MEM_Free(pKey->pIV);
					pKey->pIV = NULL;
			 	}
				if(pKey->key_uri != NULL)
				{
					DD_MEM_Free(pKey->key_uri);
					pKey->key_uri = NULL;
				}
				DD_MEM_Free(pKey);
				pKeyNode->pData = NULL;
				pKey = NULL;
			 }
			 PrintError("%s(%d) Discard key...\n", __FUNCTION__, __LINE__);
			 UTIL_DLLIST_DeleteNodes(pKeyNode);
		}
	}
	return M3U8_ERR_OK;
}

m3u8Err_e HLS_M3U8_DiscardSegmentsAndKeys(m3u8Stream_t *pStream, HUINT64 ullNum)
{
	m3u8Node_t *pExtractedNode = NULL;
	m3u8Node_t *pSegNode = NULL;
	m3u8Segment_t *pSegment = NULL;
	HUINT32 unNodeCount = 0;
	if(pStream == NULL)
	{
		return M3U8_ERR_FAIL;
	}
	if( ullNum < pStream->ullSequenceNum )
	{
		return M3U8_ERR_FAIL;
	}

	pSegNode = UTIL_DLLIST_GetHeadNode(&pStream->segmentSet);
	while(pSegNode != NULL)
	{
		pSegment = (m3u8Segment_t*)pSegNode->pData;
		if(pSegment->ullNum >= ullNum)
		{
			break;
		}
		unNodeCount++;
		pSegNode = UTIL_DLLIST_NextNode(pSegNode);
	}

	if(unNodeCount == 0)
	{
		PrintError("%s(%d) Discard is not necessary[SegNum(%lld)]\n", __FUNCTION__,__LINE__,ullNum);
		return M3U8_ERR_FAIL;
	}

	pSegNode = UTIL_DLLIST_Extract_toStart(&pStream->segmentSet, unNodeCount - 1);
	if(pSegNode == NULL)
	{
		return M3U8_ERR_FAIL;
	}
	pExtractedNode = pSegNode;
	while(pSegNode != NULL)
	{
		pSegment = (m3u8Segment_t*)pSegNode->pData;
		if(pSegment == NULL)
		{
			PrintError("%s(%d) Segment Data is not exist\n",__FUNCTION__,__LINE__);
			PrintExit;
			return M3U8_ERR_FAIL;
		}
		if(pSegment->segment_url!= NULL)
		{
			DD_MEM_Free(pSegment->segment_url);
		}
		if(pSegment->backup_url != NULL)
		{
			DD_MEM_Free(pSegment->backup_url);
		}
		if(pSegment->pKey != NULL && pSegment->pKey->key_uri != NULL)
		{
			pSegment->pKey->uUsedCount--;
			if(pSegment->pKey->uUsedCount == 0)
			{
				HLS_M3U8_DiscardKey(pStream, pSegment->pKey->key_uri);
			}
		}
		if(pSegment != NULL)
		{
			DD_MEM_Free(pSegment);
			pSegNode->pData = NULL;
			pSegment = NULL;
		}
		pSegNode = UTIL_DLLIST_NextNode(pSegNode);
	}
	UTIL_DLLIST_DeleteNodes(pExtractedNode);
	pExtractedNode = NULL;
	return M3U8_ERR_OK;
}


m3u8Err_e HLS_M3U8_DiscardSegmentsAndKeysUntilBaseSegment(m3u8Stream_t *pStream, m3u8Segment_t *pBaseSegment)
{
	m3u8Node_t *pExtractedNode = NULL;
	m3u8Node_t *pSegNode = NULL;
	m3u8Segment_t *pSegment = NULL;
	HUINT32 unNodeCount = 0;
	if(pStream == NULL)
	{
		return M3U8_ERR_FAIL;
	}
	if(pBaseSegment == NULL)
	{
		return M3U8_ERR_FAIL;
	}
	pSegNode = UTIL_DLLIST_GetHeadNode(&pStream->segmentSet);
	while(pSegNode != NULL)
	{
		pSegment = (m3u8Segment_t*)pSegNode->pData;
		if(pSegment == pBaseSegment)
		{
			break;
		}
		unNodeCount++;
		pSegNode = UTIL_DLLIST_NextNode(pSegNode);
	}

	if(unNodeCount == 0)
	{
		PrintError("%s(%d) Discard is not necessary]\n", __FUNCTION__,__LINE__);
		return M3U8_ERR_FAIL;
	}

	pSegNode = UTIL_DLLIST_Extract_toStart(&pStream->segmentSet, unNodeCount - 1);
	if(pSegNode == NULL)
	{
		return M3U8_ERR_FAIL;
	}
	pExtractedNode = pSegNode;
	while(pSegNode != NULL)
	{
		pSegment = (m3u8Segment_t*)pSegNode->pData;
		if(pSegment == NULL)
		{
			PrintError("%s(%d) Segment Data is not exist\n",__FUNCTION__,__LINE__);
			PrintExit;
			return M3U8_ERR_FAIL;
		}
		if(pSegment->segment_url!= NULL)
		{
			DD_MEM_Free(pSegment->segment_url);
		}
		if(pSegment->backup_url != NULL)
		{
			DD_MEM_Free(pSegment->backup_url);
		}
		if(pSegment->pKey != NULL && pSegment->pKey->key_uri != NULL)
		{
			pSegment->pKey->uUsedCount--;
			if(pSegment->pKey->uUsedCount == 0)
			{
				HLS_M3U8_DiscardKey(pStream, pSegment->pKey->key_uri);
			}
		}
		if(pSegment != NULL)
		{
			DD_MEM_Free(pSegment);
			pSegNode->pData = NULL;
			pSegment = NULL;
		}
		pSegNode = UTIL_DLLIST_NextNode(pSegNode);
	}
	UTIL_DLLIST_DeleteNodes(pExtractedNode);
	pExtractedNode = NULL;
	return M3U8_ERR_OK;
}

HUINT32 HLS_M3U8_GetParserVersion() { return g_ParserVersion; }

m3u8_t* HLS_M3U8_Create(HINT8* pM3U8Buf, HUINT32 unBufSize, HINT8* url, HINT8* pHostUrl)
{
	m3u8Playlist_t *pPlaylist = NULL;
	m3u8_t *m3u8 = NULL;
	PrintEnter;
	m3u8 = (m3u8_t*)DD_MEM_Alloc(sizeof(m3u8_t));
	if (m3u8 == NULL)
	{
		PrintError("%s(%d) Memory Allocation Failed\n",__FUNCTION__,__LINE__);
		PrintExit;
		return NULL;
	}
	else
	{
		VK_memset(m3u8,0x00,sizeof(m3u8_t));
	}
	/* else */

	m3u8->pHostUrl = P_HLS_M3U8_MakeHostUrl(pHostUrl, url);
	/* Fill URL info and BASE URL */
	m3u8->pTopUrl = (HINT8*)VK_StrDup((const char*)url);
	m3u8->unVersion = 1;
	pPlaylist = VK_MEM_Alloc(sizeof(m3u8Playlist_t));
	if(pPlaylist == NULL)
	{
		if(m3u8 != NULL)
		{
			DD_MEM_Free(m3u8);
		}
		PrintExit;
		return NULL;
	}
	VK_MEM_Memset(pPlaylist,0x00,sizeof(m3u8Playlist_t));
	pPlaylist->pUrl = VK_StrDup(url);
	pPlaylist->pBaseUrl = P_HLS_M3U8_GetAndFillBaseURL(url);
	pPlaylist->nProgramId = M3U8_CONST_DEFAULT_PROGRAM_ID;
	pPlaylist->bMasterPlaylist = TRUE;
	if(HLS_M3U8_Update(m3u8, pPlaylist ,pM3U8Buf, unBufSize) != M3U8_ERR_OK)
	{
		if(m3u8 != NULL)
		{
			DD_MEM_Free(m3u8);
	}
		PrintExit;
		return NULL;
	}
	if(UTIL_DLLIST_Append(&m3u8->playlistSet, pPlaylist) != DLL_ERR_OK)
	{
		PrintError("%s(%d) Insert Stream Fail\n",__FUNCTION__,__LINE__);
		PrintExit;
		return NULL;
	}
	PrintExit;
	return m3u8;
}

m3u8Err_e HLS_M3U8_Update(m3u8_t* m3u8, m3u8Playlist_t* pPlaylist, HINT8* pM3U8Buf, HUINT32 unBufSize)
{
	HUINT32 unSize = unBufSize;
	HINT8* pLine = NULL;
	HUINT32 unLineSize = 0;
	HINT8* pSubLine = NULL;
//	HUINT32 unSubLineSize = 0;
	HINT8* p = pM3U8Buf;
	HBOOL bDiscontinuity = FALSE;
	HLONG utc = 0;
	m3u8Tag_e eTag = eM3U8_TAG_MAX;
	HINT8 value[M3U8_CONST_VALUE_BUFFSIZE] = {0};
	HINT8* pFirstLine = NULL;
	HINT8* pFirstDirPos = NULL;
	HBOOL isChangedTs = FALSE;
#if 1 // __SUPPORT_BYTERANGE__
	HINT8* pByteRangeSupport = NULL;
#endif

	PrintEnter;
	pPlaylist->bUpdated = FALSE;
	if(P_HLS_M3U8_IsValid(pM3U8Buf,unBufSize) != M3U8_ERR_OK)
	{
		PrintError("%s(%d) It is not M3U8\n",__FUNCTION__,__LINE__);
		PrintExit;
		return M3U8_ERR_FAIL;
	}
	while( unSize > 0 && ((unLineSize = P_HLS_M3U8_ReadLine(&pLine, p, unSize)) > 0) )
	{
		eTag = eM3U8_TAG_MAX;
		if(pLine == NULL)
		{
			// PrintDebug("%s(%d) Line Data is NULL (Line Skipping)\n",__FUNCTION__,__LINE__);
			p += unLineSize;
			unSize -= unLineSize;
			continue;
		}
		else
		{
			p += unLineSize;
			unSize -= unLineSize;
		}
		VK_MEM_Memset(value,0x00,M3U8_CONST_VALUE_BUFFSIZE);
		if(P_HLS_M3U8_GetTag((const char*)pLine,value) == FALSE)
		{
			if(pLine != NULL)
			{
				DD_MEM_Free(pLine);
			}
			pLine = NULL;
			break;
		}
		eTag = P_HLS_M3U8_IdentifyTag(value);
		/* Parsing */
		switch(eTag)
		{
			case eM3U8_TAG_MAX :
				PrintDebug("%s(%d) TAG Ignored\n",__FUNCTION__,__LINE__);
				break;
			case eM3U8_TAG_EXT_X_STREAM_INF :
				while( unSize > 0 && ((unLineSize = P_HLS_M3U8_ReadLine(&pSubLine, p, unSize)) > 0))
				{
					if(pSubLine == NULL)
					{
						PrintDebug("%s(%d) SubLine Data is NULL (Line Skipping)\n",__FUNCTION__,__LINE__);
						p += unLineSize;
						unSize -= unLineSize;
						continue;
					}
					else
					{
						p += unLineSize;
						unSize -= unLineSize;

						if(NULL == VK_strstr((const char*)pSubLine, ".m3u8"))
						{
							DD_MEM_Free(pSubLine);
							pSubLine = NULL;
							continue;
						}

						if(P_HLS_M3U8_ParseStreamInformation(m3u8,pPlaylist,pLine,pSubLine) != M3U8_ERR_OK)
						{
							PrintError("%s(%d) Stream Information Parse Fail\n",__FUNCTION__,__LINE__);
						}
						DD_MEM_Free(pSubLine);
						pSubLine = NULL;
						break;
					}
				}
				break;
			case eM3U8_TAG_EXTINF :
				while( unSize > 0 && ((unLineSize = P_HLS_M3U8_ReadLine(&pSubLine, p, unSize)) > 0))
				{
					if(pSubLine == NULL)
					{
						// PrintDebug("%s(%d) SubLine Data is NULL (Line Skipping)\n",__FUNCTION__,__LINE__);
						p += unLineSize;
						unSize -= unLineSize;
						continue;
					}
					else
					{
						p += unLineSize;
						unSize -= unLineSize;
#if 1
						if(pSubLine[0] == '#')
						{
							if(!VK_strncmp(pSubLine,M3U8_TAG_EXT_X_KEY,VK_strlen(M3U8_TAG_EXT_X_KEY)))
							{
								if(P_HLS_M3U8_ParseKeyInformation(m3u8,pPlaylist,pSubLine) != M3U8_ERR_OK)
								{
									PrintError("%s(%d) Ext Information Parse Fail\n",__FUNCTION__,__LINE__);
								}
								DD_MEM_Free(pSubLine);
								pSubLine = NULL;
							}
							else if(!VK_strncmp(pSubLine,M3U8_TAG_EXT_X_BYTERANGE,VK_strlen(M3U8_TAG_EXT_X_BYTERANGE)))
							{
								pByteRangeSupport = pSubLine;
								pSubLine = NULL;
							}
							else
							{
								DD_MEM_Free(pSubLine);
								pSubLine = NULL;
							}
						}
						else
						{
							if(P_HLS_M3U8_ParseSegmentInformation(m3u8,pPlaylist,pLine,pSubLine,bDiscontinuity,utc, pByteRangeSupport) != M3U8_ERR_OK)
							{
								PrintError("%s(%d) Segment Information Parse Fail\n",__FUNCTION__,__LINE__);
							}
							bDiscontinuity = FALSE;
							utc = 0;
							pPlaylist->unSegmentNum++;
							DD_MEM_Free(pSubLine);
							pSubLine = NULL;
							if(pByteRangeSupport != NULL)
							{
								DD_MEM_Free(pByteRangeSupport);
							}
							pByteRangeSupport = NULL;
							break;
						}
#else
						#if 1
						if(pSubLine[0] == '#')
						{
							if(!VK_strncmp(pSubLine,M3U8_TAG_EXT_X_KEY,VK_strlen(M3U8_TAG_EXT_X_KEY)))
							{
								if(P_HLS_M3U8_ParseKeyInformation(m3u8,pPlaylist,pSubLine) != M3U8_ERR_OK)
								{
									PrintError("%s(%d) Ext Information Parse Fail\n",__FUNCTION__,__LINE__);
								}
								DD_MEM_Free(pSubLine);
								pSubLine = NULL;
							}
							else
							{
								DD_MEM_Free(pSubLine);
								pSubLine = NULL;
							}
						}
						else
						#endif
						{
							if(P_HLS_M3U8_ParseSegmentInformation(m3u8,pPlaylist,pLine,pSubLine,bDiscontinuity,utc) != M3U8_ERR_OK)
							{
								PrintError("%s(%d) Segment Information Parse Fail\n",__FUNCTION__,__LINE__);
							}
							bDiscontinuity = FALSE;
							utc = 0;
							pPlaylist->unSegmentNum++;
							DD_MEM_Free(pSubLine);
							pSubLine = NULL;
							break;
						}
#endif
					}
				}
				break;
			case eM3U8_TAG_EXT_X_KEY :
				if(P_HLS_M3U8_ParseKeyInformation(m3u8,pPlaylist,pLine) != M3U8_ERR_OK)
				{
					PrintError("%s(%d) Ext Information Parse Fail\n",__FUNCTION__,__LINE__);
				}
				break;
			case eM3U8_TAG_EXT_X_DISCONTINUITY :
				bDiscontinuity = TRUE;
				break;
			case eM3U8_TAG_EXTM3U :
//				PrintData("%s(%d) EXTM3U Tag Skipping\n",__FUNCTION__,__LINE__);
				break;
			case eM3U8_TAG_EXT_X_PROGRAM_DATE_TIME:
				(void)P_HLS_M3U8_ConvertDateTime(pLine,&utc);
				PrintData("%s(%d) eM3U8_TAG_EXT_X_PROGRAM_DATE_TIME\n",__FUNCTION__,__LINE__);
				break;
			default:
				if(P_HLS_M3U8_ParseSimpleTags(m3u8,pPlaylist,eTag,pLine) != M3U8_ERR_OK)
				{
					PrintError("%s(%d) Ext Information Parse Fail\n",__FUNCTION__,__LINE__);
				}
				break;
		}
//		PrintDebug("%s(%d) Line : %s \n",__FUNCTION__,__LINE__,pLine);
		DD_MEM_Free(pLine);
		pLine = NULL;
		if(eTag == eM3U8_TAG_EXT_X_ENDLIST)
		{
			break;
		}
	}

	pFirstDirPos = NULL;
	if(pFirstLine != NULL)
		VK_MEM_Free(pFirstLine);
	pFirstLine = NULL;

	PrintExit;
	return M3U8_ERR_OK;
}

HBOOL HLS_M3U8_IsLiveStream(m3u8Stream_t *pStream)
{
	if(pStream->bEndList)
		return FALSE;

	return TRUE;
}

HFLOAT32 HLS_M3U8_GetTimeGapBetweenCurAndLive(m3u8Stream_t* pStream, HUINT64 ullCurSegmentNum, HUINT64 ullLastSequenceNum)
{
	m3u8Segment_t* pSegment = NULL;
	m3u8Node_t* pSegNode = NULL;
	HFLOAT32 flTimeGap = 0;
	HFLOAT32 flCurTime = 0;
	HFLOAT32 flLiveTime = 0;
	HUINT64 ullNum = 0;

	if(pStream == NULL)
	{
		return 0;
	}

	if(ullCurSegmentNum - 1 < pStream->ullSequenceNum)
	{
		return 0;
	}

	if(ullLastSequenceNum - 1 < pStream->ullSequenceNum)
		return 0;

	// 현재 segment의 play 시간을 가져온다.
	ullNum = (ullCurSegmentNum - 1) - pStream->ullSequenceNum;
	pSegNode = UTIL_DLLIST_Get_n_Node(&pStream->segmentSet, ullNum);
	if(pSegNode == NULL)
	{
		return 0;
	}
	pSegment = (m3u8Segment_t*)pSegNode->pData;
	flCurTime = pSegment->starttime_of_segment;

	// 업데이트 된 segment의 play 시간을 가져온다.
	ullNum = (ullLastSequenceNum - 1) - pStream->ullSequenceNum;
	pSegNode = UTIL_DLLIST_Get_n_Node(&pStream->segmentSet, ullNum);
	if(pSegNode == NULL)
	{
		return 0;
	}
	pSegment = (m3u8Segment_t*)pSegNode->pData;
	flLiveTime = pSegment->starttime_of_segment;

	// 두 시간의 차이를 리턴
	if(flLiveTime < flCurTime)
		return 0;
	flTimeGap = flLiveTime - flCurTime;

	PrintDebug("%s(%d):return Time gap(%f)\n", __FUNCTION__, __LINE__, flTimeGap);

	return flTimeGap;
}

/*@}*/
