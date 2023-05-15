/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   sstr_manifestparser.c  $
 * Version:			$Revision:   1.0  $
 * Original Author:		YoungKi Kim $
 * Current Author:	$Author: ykkim3@humaxdigital.com $
 * Date:			$Date: 2012.02.28
 * File Description:	SSTR Manifest Parser
 * Module:
 * Remarks:
 */

/**
 * @defgroup SSTR_MANIFESTPARSER
 * @brief SSTR Manifest Parser Module 이며 Manifest를 Parsing하여 Controller 가 운용하기 좋은 구조로 생성한다.
 */

/**
 * @author YoungKi Kim (ykkim3@humaxdigital.com)
 * @date 28 Feb 2012
 */

/**
 * @brief SSTR Manifest Parser Module 이며 Manifest를 Parsing하여 Controller 가 운용하기 좋은 구조로 생성한다.
 */

/**
 * @note
 * Copyright (C) 2012 Humax Corporation. All Rights Reserved. <br>
 * This software is the confidential and proprietary information
 * of Humax Corporation. You may not use or distribute this software
 * except in compliance with the terms and conditions of any applicable license
 * agreement in writing between Humax Corporation and you.
 */

 /*@{*/

/**
 * @file sstr_manifestparser.c
 */

#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#include 	<malloc.h> /** malloc_trim */
#endif

#include "xmlmemory.h"
#include "parserInternals.h"
#include "parser.h"
#include "tree.h"

#include "vkernel.h"
#include "sstr_manifestparser.h"
 #include <ctype.h>
#include <inttypes.h>	 /** for 64bit MACRO ex>PRId64 */
#include "di_iptuner_config.h"

/*----------------------------------------------------------------------------------------------
 *	define
 *---------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------
 *	MACRO
 *---------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------
 *	TAG and ATTR
 *---------------------------------------------------------------------------------------------*/

/* I only define real tag/attr name because the XML syntax distinguishes between upper and lower letters.
   shall i define lower/upper name ? */

#define SSTR_TAG_SSTRMEDIA				"SmoothStreamingMedia"
#define SSTR_ATTR_MAJORVERSION		 	"MajorVersion"
#define SSTR_ATTR_MINORVERSION			"MinorVersion"
#define SSTR_ATTR_TIMESCALE		 		"TimeScale"
#define SSTR_ATTR_DURATION				"Duration"

#define SSTR_ATTR_ISLIVE				"IsLive"
#define SSTR_ATTR_LOOKAHEADCOUNT		"LookaheadCount"
#define SSTR_ATTR_LOOKAHEADFRAGCOUNT	"LookAheadFragmentCount"

#define SSTR_ATTR_DVRWINDOWLENGTH		"DVRWindowLength"

#define SSTR_TAG_PROTECTION				"Protection"
#define SSTR_TAG_SYSTEMID				"SystemID"
#define SSTR_TAG_PROTECTIONHEADER		"ProtectionHeader"

#define SSTR_TAG_STREAM					"StreamIndex"
#define SSTR_ATTR_TYPE					"Type"
#define SSTR_ATTR_SUBTYPE	 			"Subtype"
#define SSTR_ATTR_LANGUAGE	 			"Language"
#define SSTR_ATTR_STREAMTIMESCALE	 	"TimeScale"
#define SSTR_ATTR_NAME		 			"Name"
#define SSTR_ATTR_NUMOFFRAGMENTS		"Chunks"
#define SSTR_ATTR_NUMOFTRACKS			"QualityLevels"
#define SSTR_ATTR_URL					"Url"
#define SSTR_ATTR_STREAMMAXWIDTH		"MaxWidth"
#define SSTR_ATTR_STREAMMAXHEIGHT		"MaxHeight"
#define SSTR_ATTR_DISPLAYWIDTH			"DisplayWidth"
#define SSTR_ATTR_DISPLAYHEIGHT			"DisplayHeight"

#define SSTR_TAG_TRACK					"QualityLevel"
#define SSTR_ATTR_INDEX					"Index"
#define SSTR_ATTR_BITRATE				"Bitrate"
#define SSTR_ATTR_MAXWIDTH			 	"MaxWidth"
#define SSTR_ATTR_MAXHEIGHT		 		"MaxHeight"
#define SSTR_ATTR_CODECPRIVATEDATA		"CodecPrivateData"
#define SSTR_ATTR_SAMPLERATE			"SamplingRate"
#define SSTR_ATTR_CHANNELS				"Channels"
#define SSTR_ATTR_BITSPERSAMPLES		"BitsPerSample"
#define SSTR_ATTR_PACKETSIZE			"PacketSize"
#define SSTR_ATTR_AUDIOTAG				"AudioTag"
#define SSTR_ATTR_FOURCC				"FourCC"
#define SSTR_ATTR_NALUNITLENGTHFIELD	"NALUnitLengthField"
#define SSTR_ATTR_CUSTOMATTRIBUTES		"CustomAttributes"
#define SSTR_ATTR_ATTRIBUTE				"Attribute"
#define SSTR_ATTR_CUSTOMATTR_NAME		"Name"
#define SSTR_ATTR_CUSTOMATTR_VALUE		"Value"

#define SSTR_TAG_STREAMFRAGMENT			"c"
#define SSTR_ATTR_FRAGMENTNUMBER		"n"
#define SSTR_ATTR_FRAGMENTDURATION		"d"
#define SSTR_ATTR_FRAGMENTTIME		 	"t"

#define SSTR_TAG_TRACKFRAGMENT			"f"
#define SSTR_ATTR_TRACKFRAGMENTINDEX	"i"


#define SSTR_VAL_TYPE_VIDEO "video"
#define SSTR_VAL_TYPE_AUDIO "audio"
#define SSTR_VAL_TYPE_TEXT 	"text"

#define SSTR_VAL_TRUE 	"TRUE"
#define SSTR_VAL_FALSE 	"FALSE"

/*----------------------------------------------------------------------------------------------
 *	static functions
 *---------------------------------------------------------------------------------------------*/
static inline xmlDoc *__xmldoc(void *document) { return (xmlDoc *)document; }

static inline xmlNode *__xmlnode(void *node) { return (xmlNode *)node; }

static inline sstrMedia_t *__sstrMedia(void *media) { return (sstrMedia_t*)media; }

#ifdef SSTR_MANIFESTPARSER_CONFIG_UPPERCASE
static int
sstrUpperStrcmp(const xmlChar *str1, const xmlChar *str2) {
	char upc1, upc2;
    register int tmp;

    if ((str1 == NULL) && (str2 == NULL)) return(0);
    if (str1 == NULL) return(-1);
    if (str2 == NULL) return(1);
    do {
    	upc1 = VK_toupper(*str1++);
    	upc2 = VK_toupper(*str2++);
  		tmp = upc1 - upc2;
	if (tmp != 0) return(tmp);
    } while ((*str1 != 0) && (*str2 != 0));
    return (*str1 - *str2);
}
#endif

#if 0 /* Avoid Compile Warning " defined but not used " */
static long long _atoi64(const char *nptr)
{
  char *s = (char *)nptr;
  long long acc = 0;
  int neg = 0;

  while(isspace((int)*s))
    s++;
  if (*s == '-')
    {
      neg = 1;
      s++;
    }
  else if (*s == '+')
    s++;

  while (VK_isdigit((int)*s))
    {
      acc = 10 * acc + ((int)*s - '0');
      s++;
    }

  if (neg)
    acc *= -1;
  return acc;
}
#endif

static unsigned long long _atoui64(char *nptr)
{
	char *s = (char *)nptr;
	unsigned long long acc = 0;
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

#if 0 /* Avoid Compile Warning " defined but not used " */
static unsigned int mpdXsDurationToMs(char *pStr, int nSize)
{
	char buff[5] = {0};
	unsigned int res = 0;
	int nPos = 0;
	int nStart = 0;
	int nEnd = 0;
	char *p = pStr;
	while(p != NULL && nSize > nPos)
	{
		VK_memset(buff,0x0,sizeof(buff));
		switch(*p)
		{
			case 'P' :
				nStart = nPos+1;
				break;
			case 'T' :
				nStart = nPos+1;
				break;
			case 'Y' :
				/* not supported */
				break;
			case 'M' :
				/* not supported for "Month"*/
				nEnd = nPos;
				VK_strncpy(buff,pStr+nStart,nEnd-nStart);
				nStart = nPos+1;
				res += VK_atoi(buff) * 60 * 1000;
				break;
			case 'D' :
				/* not supported */
				break;
			case 'H' :
				nEnd = nPos;
				VK_strncpy(buff,pStr+nStart,nEnd-nStart);
				nStart = nPos+1;
				res += VK_atoi(buff) * 60 * 60 * 1000;
				break;
			case 'S' :
				nEnd = nPos;
				VK_strncpy(buff,pStr+nStart,nEnd-nStart);
				nStart = nPos+1;
				res += atof(buff) * 1000;

				break;
		}
		p++;
		nPos++;
	}
	return res;
}
#endif

/*********************************/
/* for Base 64 Decoding */
/*********************************/
static const HINT32 Base64DecodeTable[256] =
{
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 0,-1,-1,-1, 1,
     2, 3, 4, 5, 6, 7, 8, 9,10,11,-1,-1,-1,-1,-1,-1,
    -1,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,
    27,28,29,30,31,32,33,34,35,36,37,-1,-1,-1,-1,-1,
    -1,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,
    53,54,55,56,57,58,59,60,61,62,63,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
};
static const HINT32 Base64CharMapTable[64] =
{
	62,63,52,53,54,55,56,57,58,59,60,61, 0, 1, 2, 3,  /* 00-0F */
	 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,  /* 10-1F */
	20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,  /* 20-2F */
	36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,  /* 30-3F */
};
#if 0
typedef union
{
	struct
	{
#if __BYTE_ORDER == __LITTLE_ENDIAN
		unsigned char c1,c2,c3;
#else
		unsigned char c3,c2,c1;
#endif
	}c;
	struct
	{
#if __BYTE_ORDER == __LITTLE_ENDIAN
		unsigned int e1:6,e2:6,e3:6,e4:6;
#else
		unsigned int e4:6,e3:6,e2:6,e1:6;
#endif
	}e;
} BF;

/**
 * @b Function @b Description <br>
 * Base64 Decoding <br>
 *
 * @param[in] dst	:  원본 Data
 * @param[out] text	:  Decoded Data
 * @param[in] numBytes	:  원본 Data 크기
 *
 * @return  #  Decoded Data 크기 <br>
 */
static int local_base64_decode(char *src, unsigned char *result, int numBytes )
{
	int i, j = 0, src_length, blank = 0;
	BF temp;

	src_length = numBytes;/* VK_strlen(src);*/
	for(i = 0 ; i < src_length ; i = i+4, j = j+3)
	{
		temp.e.e4 = Base64DecodeTable[(int)src[i]];
		temp.e.e3 = Base64DecodeTable[(int)src[i+1]];
		if(src[i+2] == '='){
			temp.e.e2 = 0x00;
			blank++;
		}
		else
		{
			temp.e.e2 = Base64DecodeTable[(int)src[i+2]];
		}
		if(src[i+3] == '='){
			temp.e.e1 = 0x00;
			blank++;
		}
		else
		{
			temp.e.e1 = Base64DecodeTable[(int)src[i+3]];
		}
		result[j]   = temp.c.c3;
		result[j+1] = temp.c.c2;
		result[j+2] = temp.c.c1;
	}
	return j-blank;
}
#else
static HINT32 local_base64_decode(HINT8 *src, HUINT8 *result, HINT32 numBytes)
{
	HINT32 nIndex = 0;
	HINT32 nDecodePhase = 0;
	const HINT8 *pCursor = NULL;
	HINT32 cur_d = 0;
	HINT32 prev_d = 0;
	HUINT8 cDecodedChar;
	pCursor = src;
	while (*pCursor != '\0')
	{
		cur_d = Base64DecodeTable[(HINT32)*pCursor];
		if ( cur_d != -1 )
		{
			cur_d = Base64CharMapTable[cur_d];
			switch ( nDecodePhase )
			{
			case 0:
				++nDecodePhase;
				break;
			case 1:
				cDecodedChar = ( ( prev_d << 2 ) | ( ( cur_d & 0x30 ) >> 4 ) );
				if ( nIndex < numBytes )
				{
					result[nIndex++] = cDecodedChar;
				}
				++nDecodePhase;
				break;
			case 2:
				cDecodedChar = ( ( ( prev_d & 0xf ) << 4 ) | ( ( cur_d & 0x3c ) >> 2 ) );
				if ( nIndex < numBytes )
				{
					result[nIndex++] = cDecodedChar;
				}
				++nDecodePhase;
				break;
			case 3:
				cDecodedChar = ( ( ( prev_d & 0x03 ) << 6 ) | cur_d );
				if ( nIndex < numBytes )
				{
					result[nIndex++] = cDecodedChar;
				}
				nDecodePhase = 0;
				break;
			}
			prev_d = cur_d;
		}
		++pCursor;
	}
	return nIndex;
}
#endif
/**
 * @b Function @b Description <br>
 * Unicode String 탐색  <br>
 *
 * @param[in] pSrc	:  원본 Data
 * @param[in] srcBytes	:  원본 Data 크기
 * @param[in] pDst	:  찾을 Data
 * @param[in] pDst	:  찾을 Data 크기
 *
 * @return  #  원본 Data 내의 찾은 Data의 시작 위치 <br>
 */
static HUINT8* local_unicode_strstr(HUINT8 *pSrc, int srcBytes, HUINT8 *pDst, int dstBytes)
{
	HUINT32 nPos = 0;
	for(HINT32 i = 0 ; i < srcBytes ; i++)
	{
		for(HINT32 j = 0 ; j < dstBytes; j++)
		{
			if(pSrc[i + j] == pDst[j])
			{
				if(j == dstBytes - 1)
				{
					nPos = (i + j) - (dstBytes - 1);
					return pSrc + nPos;
				}
			}
			else
			{
				break;
			}
		}
	}
	return NULL;
}

/**
 * @b Function @b Description <br>
 * Garbage Colllector 수행 <br>
 * Manifest Data의 DLLIST Node 및 각 Data를 Free 하지 않고 GC Handle에 추가한다. <br>
 *
 * @param[in] media	:  Manifest Data
 * @param[in] pGC		:  Garbage Collector Handle
 *
 * @return  #  sstrErr_e <br>
 */
static sstrErr_e local_manifestparser_gc_collect(sstrMedia_t *media, sstrParserGC_t *pGC)
{
	sstrNode_t *pStreamNode = NULL;
	sstrNode_t *pTrackNode = NULL;
	sstrNode_t *pFragNode = NULL;
	sstrNode_t *pTrackContentNode = NULL;
	sstrStream_t *pStream = NULL;
	sstrTrack_t *pTrack = NULL;
	sstrTrackContent_t *pTrackContent = NULL;
	if(media == NULL)
	{
		return SSTR_ERR_FAIL;
	}
	if(pGC == NULL)
	{
		return SSTR_ERR_FAIL;
	}
	pStreamNode = UTIL_DLLIST_GetHeadNode(&media->streamSet);
	while(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*) pStreamNode->pData;
		if(pStream != NULL)
		{
			pFragNode = UTIL_DLLIST_Extract_toEnd(&pStream->fragmentSet,0);
			if(pFragNode != NULL)
			{
				UTIL_DLLIST_AppendNode(&pGC->fragmentGC, pFragNode);
			}
			pTrackNode = UTIL_DLLIST_GetHeadNode(&pStream->trackSet);
			while(pTrackNode != NULL)
			{
				pTrack = (sstrTrack_t*) pTrackNode->pData;
				if(pTrack != NULL)
				{
					pTrackContentNode = UTIL_DLLIST_GetHeadNode(&pTrack->trackContentSet);
					while(pTrackContentNode != NULL)
					{
						pTrackContent = (sstrTrackContent_t*) pTrackContentNode->pData;
						if(pTrackContent != NULL)
						{
							if(pTrackContent->pName != NULL)
							{
								VK_MEM_Free(pTrackContent->pName);
							}
							if(pTrackContent->pValue != NULL)
							{
								VK_MEM_Free(pTrackContent->pValue);
							}
						}
						if(pTrackContentNode->pData != NULL)
						{
							VK_MEM_Free(pTrackContentNode->pData);
							pTrackContentNode->pData = NULL;
						}
						pTrackContentNode = UTIL_DLLIST_NextNode(pTrackContentNode);
					}
					UTIL_DLLIST_Reset(&pTrack->trackContentSet);
				}
				pTrackNode = UTIL_DLLIST_NextNode(pTrackNode);
			}
			pTrackNode = UTIL_DLLIST_Extract_toEnd(&pStream->trackSet,0);
			UTIL_DLLIST_AppendNode(&pGC->trackGC, pTrackNode);
		}
		pStreamNode = UTIL_DLLIST_NextNode(pStreamNode);
	}
	pStreamNode = UTIL_DLLIST_Extract_toEnd(&media->streamSet,0);
	UTIL_DLLIST_AppendNode(&pGC->streamGC, pStreamNode);
	return SSTR_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Garbage Colllector 를 비운다. <br>
 *
 * @param[in] pGC		:  Garbage Collector Handle
 *
 * @return  #  sstrErr_e <br>
 */
static sstrErr_e local_manifestparser_gc_free(sstrParserGC_t *pGC)
{
	sstrNode_t *pStreamNode = NULL;
	sstrNode_t *pTrackNode = NULL;
	sstrNode_t *pFragNode = NULL;
	sstrNode_t *pTrackContentNode = NULL;
	sstrStream_t *pStream = NULL;
	sstrTrack_t *pTrack = NULL;
	sstrFragment_t *pFragment = NULL;
	sstrTrackContent_t *pTrackContent = NULL;

	pStreamNode = UTIL_DLLIST_GetHeadNode(&pGC->streamGC);
	while(pStreamNode != NULL)
	{
		pStream = (sstrStream_t*) pStreamNode->pData;
		if(pStream != NULL)
		{
			if(pStream->pName != NULL)
			{
				VK_MEM_Free(pStream->pName);
			}
			if(pStream->pUrlPattern != NULL)
			{
				VK_MEM_Free(pStream->pUrlPattern);
			}
			if(pStream->pLanguage != NULL)
			{
				VK_MEM_Free(pStream->pLanguage);
			}
			if(pStreamNode->pData != NULL)
			{
				VK_MEM_Free(pStreamNode->pData);
				pStreamNode->pData = NULL;
			}
			pStreamNode = UTIL_DLLIST_NextNode(pStreamNode);
		}
	}
	UTIL_DLLIST_Reset(&pGC->streamGC);
	pFragNode = UTIL_DLLIST_GetHeadNode(&pGC->fragmentGC);
	while(pFragNode != NULL)
	{
		pFragment = (sstrFragment_t*)pFragNode->pData;
		if(pFragment != NULL)
		{
			if(pFragment->pulIndex != NULL)
			{
				VK_MEM_Free(pFragment->pulIndex);
			}
			if(pFragment->pExtensionTrackData != NULL)
			{
				VK_MEM_Free(pFragment->pExtensionTrackData);
			}
		}
		if(pFragNode->pData != NULL)
		{
			VK_MEM_Free(pFragNode->pData);
			pFragNode->pData = NULL;
		}
		pFragNode = UTIL_DLLIST_NextNode(pFragNode);
	}
	UTIL_DLLIST_Reset(&pGC->fragmentGC);
	pTrackNode = UTIL_DLLIST_GetHeadNode(&pGC->trackGC);
	while(pTrackNode != NULL)
	{
		pTrack = (sstrTrack_t*) pTrackNode->pData;
		if(pTrack != NULL)
		{
			if(pTrack->pCodecPrivateData != NULL)
			{
				VK_MEM_Free(pTrack->pCodecPrivateData);
			}
			pTrackContentNode = UTIL_DLLIST_GetHeadNode(&pTrack->trackContentSet);
			while(pTrackContentNode != NULL)
			{
				pTrackContent = (sstrTrackContent_t*) pTrackContentNode->pData;
				if(pTrackContent != NULL)
				{
					if(pTrackContent->pName != NULL)
					{
						VK_MEM_Free(pTrackContent->pName);
					}
					if(pTrackContent->pValue != NULL)
					{
						VK_MEM_Free(pTrackContent->pValue);
					}
				}
				if(pTrackContentNode->pData != NULL)
				{
					VK_MEM_Free(pTrackContentNode->pData);
					pTrackContentNode->pData = NULL;
				}
				pTrackContentNode = UTIL_DLLIST_NextNode(pTrackContentNode);
			}
			UTIL_DLLIST_Reset(&pTrack->trackContentSet);
		}
		if(pTrackNode->pData != NULL)
		{
			VK_MEM_Free(pTrackNode->pData);
			pTrackNode->pData = NULL;
		}
		pTrackNode = UTIL_DLLIST_NextNode(pTrackNode);
	}
	UTIL_DLLIST_Reset(&pGC->trackGC);
	return SSTR_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Manifest Data 를 제거한다. <br>
 *
 * @param[in] media		:  Manifest Data
 *
 * @return  #  sstrErr_e <br>
 */
static sstrErr_e local_media_free(sstrMedia_t *media)
{
	int unStream = 0;
	int unTrack = 0;
	int unFrag = 0;
	sstrNode_t *pStreamNode = NULL;
	sstrNode_t *pTrackNode = NULL;
	sstrNode_t *pFragNode = NULL;
	sstrNode_t *pTrackContentNode = NULL;
	sstrStream_t *pStream = NULL;
	sstrTrack_t *pTrack = NULL;
	sstrFragment_t *pFragment = NULL;
	sstrTrackContent_t *pTrackContent = NULL;
	if(media == NULL)
	{
		return SSTR_ERR_FAIL;
	}
	pStreamNode = UTIL_DLLIST_GetHeadNode(&media->streamSet);
	while(pStreamNode != NULL)
	{
		unStream++;
		pStream = (sstrStream_t*) pStreamNode->pData;
		if(pStream != NULL)
		{
			if(pStream->pName != NULL)
			{
				VK_MEM_Free(pStream->pName);
			}
			if(pStream->pUrlPattern != NULL)
			{
				VK_MEM_Free(pStream->pUrlPattern);
			}
			if(pStream->pLanguage != NULL)
			{
				VK_MEM_Free(pStream->pLanguage);
			}
			pFragNode = UTIL_DLLIST_GetHeadNode(&pStream->fragmentSet);
			while(pFragNode != NULL)
			{
				unFrag++;
				pFragment = (sstrFragment_t*)pFragNode->pData;
				if(pFragment != NULL)
				{
					if(pFragment->pulIndex != NULL)
					{
						VK_MEM_Free(pFragment->pulIndex);
					}
					if(pFragment->pExtensionTrackData != NULL)
					{
						VK_MEM_Free(pFragment->pExtensionTrackData);
					}
				}
				if(pFragNode->pData != NULL)
				{
					VK_MEM_Free(pFragNode->pData);
					pFragNode->pData = NULL;
				}
				pFragNode = UTIL_DLLIST_NextNode(pFragNode);
			}
			UTIL_DLLIST_Reset(&pStream->fragmentSet);
			pTrackNode = UTIL_DLLIST_GetHeadNode(&pStream->trackSet);
			while(pTrackNode != NULL)
			{
				unTrack++;
				pTrack = (sstrTrack_t*) pTrackNode->pData;
				if(pTrack != NULL)
				{
					if(pTrack->pCodecPrivateData != NULL)
					{
						VK_MEM_Free(pTrack->pCodecPrivateData);
					}
					pTrackContentNode = UTIL_DLLIST_GetHeadNode(&pTrack->trackContentSet);
					while(pTrackContentNode != NULL)
					{
						pTrackContent = (sstrTrackContent_t*) pTrackContentNode->pData;
						if(pTrackContent != NULL)
						{
							if(pTrackContent->pName != NULL)
							{
								VK_MEM_Free(pTrackContent->pName);
							}
							if(pTrackContent->pValue != NULL)
							{
								VK_MEM_Free(pTrackContent->pValue);
							}
						}
						if(pTrackContentNode->pData != NULL)
						{
							VK_MEM_Free(pTrackContentNode->pData);
							pTrackContentNode->pData = NULL;
						}
						pTrackContentNode = UTIL_DLLIST_NextNode(pTrackContentNode);
					}
					UTIL_DLLIST_Reset(&pTrack->trackContentSet);
				}
				if(pTrackNode->pData != NULL)
				{
					VK_MEM_Free(pTrackNode->pData);
					pTrackNode->pData = NULL;
				}
				pTrackNode = UTIL_DLLIST_NextNode(pTrackNode);
			}
			UTIL_DLLIST_Reset(&pStream->trackSet);
		}
		if(pStreamNode->pData != NULL)
		{
			VK_MEM_Free(pStreamNode->pData);
			pStreamNode->pData = NULL;
		}
		pStreamNode = UTIL_DLLIST_NextNode(pStreamNode);
	}
	UTIL_DLLIST_Reset(&media->streamSet);
	// PrintDebug ("TRACK %d / FRAG %d / STREAM %d\n",unTrack,unFrag,unStream);
	if(media->protection.pHeaderContent != NULL)
	{
		VK_MEM_Free(media->protection.pHeaderContent);
	}
	if(media->protection.pDecodedHeaderContent != NULL)
	{
		VK_MEM_Free(media->protection.pDecodedHeaderContent);
	}
	if(media != NULL)
	{
		VK_MEM_Free(media);
	}
	return SSTR_ERR_OK;
}


/**
 * @b Function @b Description <br>
 * libxml node 탐색 (모든 Node를 탐색한다.) <br>
 * 재귀 호출 함수 이므로 사용에 주의 <br>
 *
 * @param[in] startNode	:  시작 Node
 * @param[in] szName		:  찾을 Node 이름
 *
 * @return	#  찾은 Node : 성공<br>
 * 			#  NULL : 실패 <br>
 */
static xmlNode *local_find_node(xmlNode *startNode, const xmlChar *szName)
{
	xmlNode *retNode = NULL;

	if (startNode == NULL) { return NULL; }

	#ifdef SSTR_MANIFESTPARSER_CONFIG_UPPERCASE
	if (!sstrUpperStrcmp(startNode->name, szName))
	{
		return startNode;
	}
	#else
	if (!xmlStrcmp(startNode->name, szName))
	{
		return startNode;
	}
	#endif
	else
	{
		retNode = local_find_node(startNode->children, szName);
	}

	if (retNode == NULL)
	{
		retNode = local_find_node(startNode->next, szName);
	}

	return retNode;
}

/**
 * @b Function @b Description <br>
 * finding a specific node from the starting node to all the next node <br>
 *
 * @param[in] startNode :  starting node for finding
 * @param[in] szName :  node name
 *
 * @return  #  xmlNode <br>
 * @remark      this function travels all nodes in same level to find the one matched to @szName
 * @see 	local_find_node
 */
static xmlNode *local_find_next_node(xmlNode *startNode, const xmlChar *szName)
{
	if (startNode == NULL) { return NULL; }
	while(startNode != NULL)
	{
		if (!xmlStrcmp(startNode->name, szName))
		{
			return startNode;
		}
		else
		{
			startNode = startNode->next;
		}
	}
	return NULL;
}

/**
 * @b Function @b Description <br>
 * finding a specific node from the starting node to all the previous node <br>
 *
 * @param[in] startNode :  starting node for finding
 * @param[in] szName :  node name
 *
 * @return  #  xmlNode <br>
 * @remark      this function travels all nodes in same level to find the one matched to @szName
 * @see 	local_find_next_node
 */
#if 0 /* Avoid Compile Warning " defined but not used " */
static xmlNode *local_find_prev_node(xmlNode *startNode, const xmlChar *szName)
{
	xmlNode *retNode = NULL;
#if 0 /* recursive */
	if (startNode == NULL) { return NULL; }
#ifdef SSTR_MANIFESTPARSER_CONFIG_UPPERCASE
	if (!sstrUpperStrcmp(startNode->name, szName))
	{
		return startNode;
	}
#else
	if (!xmlStrcmp(startNode->name, szName))
	{
		return startNode;
	}
#endif
	else
	{
		retNode = local_find_node(startNode->prev, szName);
	}

	return retNode;
#else
	if (startNode == NULL) { return NULL; }
	while(startNode != NULL)
	{
		if (!xmlStrcmp(startNode->name, szName))
		{
			return startNode;
		}
		else
		{
			startNode = startNode->prev;
		}
	}
	return NULL;
#endif
	return retNode;
}

#endif

/*
static xmlNode *local_find_child_node(xmlNode *startNode, const char *szName)
{
	xmlNode *retNode = NULL;

	if (startNode == NULL) { return NULL; }

	if (!xmlStrcmp(startNode->name, szName))
	{
		return startNode;
	}
	else
	{
		retNode = local_find_child_node(startNode->childs, szName);
	}

	return retNode;
}
*/

/**
 * @b Function @b Description <br>
 * get the "ROOT" of Manifest from parser handle <br>
 *
 * @param[in] parser :  parser handle
 * @param[in] szName :  node name
 *
 * @return  #  element handle of the "ROOT"  <br>
 * @see 	local_find_next_node
 */
static sstrElement_Media_t *local_manifestparser_get_media(sstrParser_t *parser)
{
	xmlNode *pNode = NULL;

	if (__xmlnode(parser->root)->children != NULL )
	{
		pNode = local_find_node(__xmlnode(parser->root), (xmlChar*)SSTR_TAG_SSTRMEDIA);
		if ( pNode != NULL )
		{
			return (sstrElement_Media_t *)pNode;
		}
	}
	return NULL;
}

/**
 * @b Function @b Description <br>
 * finding a specific element from the child node of starting element to all the next element <br>
 *
 * @param[in] element : starting element for finding
 * @param[in] szName : element name
 *
 * @return  #  element handle  <br>
 * @see 	local_find_node
 */
#if 0/* Avoid Compile Warning " defined but not used " */
static sstrElement_t *local_manifestparser_get_element(sstrElement_t *element, const xmlChar *szName)
{
	xmlNode *pNode = NULL;

	if (__xmlnode(element)->children != NULL )
	{
		pNode = local_find_node(__xmlnode(element)->children, szName);
		if ( pNode != NULL )
		{
			return (sstrElement_t *)pNode;
		}
	}

	if (__xmlnode(element)->next == NULL )
	{
		return NULL;
	}

	return local_find_node(__xmlnode(element)->next, szName);
}
#endif

/**
 * @b Function @b Description <br>
 * finding a specific element from the node of starting element to all the next element <br>
 *
 * @param[in] element : starting element for finding
 * @param[in] szName : element name
 *
 * @return  #  element handle  <br>
 * @see 	local_find_node
 */
static sstrElement_t *local_manifestparser_get_next_element(sstrElement_t *element, const xmlChar *szName)
{
	// xmlNode *pNode = NULL;

	if (__xmlnode(element)->next == NULL )
	{
		return NULL;
	}

	return local_find_next_node(__xmlnode(element)->next, szName);
}

/**
 * @b Function @b Description <br>
 * finding a specific element from the node of starting element to all the previous element <br>
 *
 * @param[in] element : starting element for finding
 * @param[in] szName : element name
 *
 * @return  #  element handle  <br>
 * @see 	local_find_node
 */
#if 0 /* Avoid Compile Warning " defined but not used " */
static sstrElement_t *local_manifestparser_get_prev_element(sstrElement_t *element, const xmlChar *szName)
{
	// xmlNode *pNode = NULL;

	if (__xmlnode(element)->prev == NULL )
	{
		return NULL;
	}

	return local_find_prev_node(__xmlnode(element)->prev, szName);
}
#endif

/**
 * @b Function @b Description <br>
 * get a child element of starting element <br>
 *
 * @param[in] element : starting element for finding
 * @param[in] szName : element name
 *
 * @return  #  element handle  <br>
 * @see 	local_find_node
 */
static sstrElement_t *local_manifestparser_get_child_element(sstrElement_t *element, const xmlChar *szName)
{
	xmlNode *pNode = NULL;

	if (__xmlnode(element)->children != NULL )
	{
		pNode = local_find_next_node(__xmlnode(element)->children, szName);
		if ( pNode != NULL )
		{
			return (sstrElement_t *)pNode;
		}
		else
			return NULL;
	}
	else
		return NULL;
}

/*
static mpdSegment_t *local_manifestparser_get_segment(mpdSegment_t *segment, const char *szName)
{
	xmlNode *pNode = NULL;

	if (__xmlnode(segment)->childs != NULL )
	{
		pNode = local_find_node(__xmlnode(segment)->childs, szName);
		if ( pNode != NULL )
		{
			return (asxentry_t *)pNode;
		}
	}

	if (__xmlnode(segment)->next == NULL )
	{
		return NULL;
	}

	return local_find_node(__xmlnode(segment)->next, szName);
}
*/

/**
 * @b Function @b Description <br>
 * get a attribute value  <br>
 *
 * @param[in] entry : entry point for finding attribute
 * @param[in] szAttrName : attribute name
 * @param[out] szBuf : the memory buffer for storing attribute value
 * @param[in] nBufsize : size of the memory buffer
 *
 * @return  #  sstrErr_e  <br>
 * @see
 */
static sstrErr_e local_manifestparser_get_content(void *entry, char *szBuf, int nBufsize)
{
	//unsigned int	nAttrLen = 0;
	int	nAttrLen = 0;
	xmlChar 		*xmlContent = NULL;

	xmlContent = xmlNodeGetContent(__xmlnode(entry));
	if ( xmlContent == NULL )
	{
		return SSTR_ERR_ATTR_NOTFOUND;
	}

	nAttrLen = xmlStrlen(xmlContent);
	if ( nBufsize <= nAttrLen )
	{
		xmlFree(xmlContent);
		return SSTR_ERR_BUFFERSIZE;
	}
	VK_strncpy(szBuf, (char *)xmlContent, nAttrLen);
	szBuf[nAttrLen] = '\0';
	xmlFree(xmlContent);
	return SSTR_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * get a content length  <br>
 *
 * @param[in] entry : entry point for finding attribute
 * @param[out] nLen : content length
 *
 * @return  #  sstrErr_e  <br>
 * @see
 */
static sstrErr_e local_manifestparser_get_contentlength(void *entry, int *nLen)
{
	//unsigned int	nAttrLen = 0;
	int	nAttrLen = 0;
	xmlChar 		*xmlContent = NULL;

	xmlContent = xmlNodeGetContent(__xmlnode(entry));
	if ( xmlContent == NULL )
	{
		return SSTR_ERR_ATTR_NOTFOUND;
	}
	nAttrLen = xmlStrlen(xmlContent);
	*nLen = nAttrLen;
	xmlFree(xmlContent);
	return SSTR_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * get a attribute value   <br>
 *
 * @param[in] 	entry : entry point for finding attribute
 * @param[in] 	szAttrName : attribute name
 * @param[out] 	szBuf : the memory buffer for storing attribute value
 * @param[in]	nBufsize : size of the memory buffer
 *
 * @return  #  sstrErr_e  <br>
 * @see
 */
static sstrErr_e local_manifestparser_get_attr_value(void *entry, const xmlChar *szAttrName,
									char *szBuf, int nBufsize)
{
	//unsigned int	nAttrLen = 0;
	int	nAttrLen = 0;
	xmlChar 		*pXmlAttr = NULL;

	pXmlAttr = xmlGetProp(__xmlnode(entry), szAttrName);
	if ( pXmlAttr == NULL )
	{
		return SSTR_ERR_ATTR_NOTFOUND;
	}

	nAttrLen = xmlStrlen(pXmlAttr);
	if ( nBufsize <= nAttrLen )
	{
		xmlFree(pXmlAttr);
		return SSTR_ERR_BUFFERSIZE;
	}
	VK_strncpy(szBuf, (char *)pXmlAttr, nAttrLen);
	szBuf[nAttrLen] = '\0';
	xmlFree(pXmlAttr);
	return SSTR_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * get the number of elements <br>
 *
 * @param[in] 	element : starting element for finding
 * @param[in] 	szName : element name
 *
 * @return  #  the number of elements  <br>
 * @see
 */
int local_manifestparser_get_element_countByfirstElement(sstrElement_t *firstElement, const xmlChar *szName)
{
	int nCnt = 0;
	sstrElement_t *element = firstElement;

	while(element != NULL)
	{
		nCnt++;
		element = local_manifestparser_get_next_element(element, szName);
	}
	return nCnt;
}

/**
 * @b Function @b Description <br>
 * get the 'n'th element <br>
 *
 * @param[in] 	element : starting element for finding
 * @param[in] 	szName : element name
 * @param[in] 	n : 'n' th
 * @return  #  element handle  <br>
 * @see
 */
sstrElement_t* local_manifestparser_get_n_element(sstrElement_t *firstElement, const xmlChar *szName, int n)
{
	int nCnt = 0;
	sstrElement_t *element = firstElement;
	if(firstElement == NULL)
	{
		return NULL;
	}
	while(nCnt < n && element != NULL)
	{
		element = local_manifestparser_get_next_element(element, szName);
		nCnt++;
	}
	return element;
}

/**
 * @b Function @b Description <br>
 * Protection Field Parsing <br>
 *
 * @param[out] 	protection : Protection Structure
 * @param[in] 	protectionElement : Protection Field Element
 *
 * @return  #  sstrErr_e  <br>
 * @see
 */
sstrErr_e local_manifestparser_parse_protection(sstrProtection_t *protection, sstrElement_t *protectionElement)
{
	HUINT8 ucWrmHeaderUnicodeBegin[]={'<','\0','W','\0','R','\0','M','\0','H','\0','E','\0','A','\0','D','\0','E','\0','R','\0'};
	HUINT8 ucWrmHeaderUnicodeEnd[]={'<','\0','/','\0','W','\0','R','\0','M','\0','H','\0','E','\0','A','\0','D','\0','E','\0','R','\0','>','\0'};
	HUINT8 *pDecodeBuff = NULL;
	HUINT8 *pWrmStart = NULL;
	HUINT8 *pWrmEnd = NULL;
	sstrErr_e errCode = SSTR_ERR_OK;
	HINT32 nContentLen = 0;
	errCode = local_manifestparser_get_contentlength(protectionElement,&nContentLen);
	if(errCode != SSTR_ERR_OK)
	{
		protection->bIsProtected = 0;
		return errCode;
	}
	protection->nHeaderContentLen = nContentLen;
	pDecodeBuff = VK_MEM_Alloc(protection->nHeaderContentLen + 1);
	protection->pHeaderContent = VK_MEM_Alloc(protection->nHeaderContentLen + 1);
	protection->pDecodedHeaderContent = VK_MEM_Alloc(protection->nHeaderContentLen + 1);
	if(protection->pHeaderContent == NULL || protection->pDecodedHeaderContent == NULL || pDecodeBuff == NULL )
	{
		protection->bIsProtected = 0;
		if(pDecodeBuff != NULL)
		{
			VK_MEM_Free(pDecodeBuff);
		}
		return SSTR_ERR_MEMORY_ALLOC;
	}

	VK_MEM_Memset(pDecodeBuff,0x00,protection->nHeaderContentLen + 1);
	VK_MEM_Memset(protection->pHeaderContent,0x00,protection->nHeaderContentLen + 1);
	VK_MEM_Memset(protection->pDecodedHeaderContent,0x00,protection->nHeaderContentLen + 1);
	errCode = local_manifestparser_get_content(protectionElement,(char*)protection->pHeaderContent,protection->nHeaderContentLen + 1);
	if(errCode != SSTR_ERR_OK)
	{
		protection->bIsProtected = 0;
		if(pDecodeBuff != NULL)
		{
			VK_MEM_Free(pDecodeBuff);
		}
		return errCode;
	}
	nContentLen = local_base64_decode((HINT8*)protection->pHeaderContent, (HUINT8*)pDecodeBuff, protection->nHeaderContentLen);
	pWrmStart = local_unicode_strstr((unsigned char*)pDecodeBuff,nContentLen,(unsigned char*)ucWrmHeaderUnicodeBegin,sizeof(ucWrmHeaderUnicodeBegin));
	if(pWrmStart == NULL)
	{
		protection->bIsProtected = 0;
		if(pDecodeBuff != NULL)
		{
			VK_MEM_Free(pDecodeBuff);
		}
		return errCode;

	}
	pWrmEnd = local_unicode_strstr(pDecodeBuff,nContentLen,ucWrmHeaderUnicodeEnd,sizeof(ucWrmHeaderUnicodeEnd));
	if(pWrmEnd == NULL)
	{
		protection->bIsProtected = 0;
		if(pDecodeBuff != NULL)
		{
			VK_MEM_Free(pDecodeBuff);
		}
		return errCode;

	}
	pWrmEnd += sizeof(ucWrmHeaderUnicodeEnd);
	nContentLen = (pWrmEnd - pWrmStart);
	VK_MEM_Memcpy(protection->pDecodedHeaderContent, pWrmStart, nContentLen);
	protection->nDecodedHeaderContentLen = nContentLen;
	protection->bIsProtected = 1;
	if(pDecodeBuff != NULL)
	{
		VK_MEM_Free(pDecodeBuff);
	}
	return SSTR_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Media "ROOT" Parsing <br>
 *
 * @param[out] 	sstrMedia : Media Structure
 * @param[in] 	sstrElement : "ROOT" Element
 *
 * @return  #  sstrErr_e  <br>
 * @see
 */
sstrErr_e local_manifestparser_parse_media(sstrMedia_t *sstrMedia, sstrElement_Media_t *sstrElement)
{
	sstrErr_e errCode = SSTR_ERR_OK;
	sstrElement_t *element = NULL;
	char szBuf[SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE]={0}; // check buffer size
	/* Parse "MajorVersion" */
	VK_MEM_Memset(szBuf, 0x00, SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
  	errCode = local_manifestparser_get_attr_value(sstrElement,(xmlChar*)SSTR_ATTR_MAJORVERSION,szBuf,SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	if(errCode == SSTR_ERR_ATTR_NOTFOUND)
	{
		sstrMedia->nMajorVersion = 2;
	}
	else if(errCode == SSTR_ERR_OK)
	{
		sstrMedia->nMajorVersion = VK_atoi(szBuf);
	}
	else
	{
		return errCode;
	}

	/* Parse "MinorVersion" */
	VK_MEM_Memset(szBuf, 0x00, SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
  	errCode = local_manifestparser_get_attr_value(sstrElement,(xmlChar*)SSTR_ATTR_MINORVERSION,szBuf,SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	if(errCode == SSTR_ERR_ATTR_NOTFOUND)
	{
		sstrMedia->nMinorVersion = 0;
	}
	else if(errCode == SSTR_ERR_OK)
	{
		sstrMedia->nMinorVersion = VK_atoi(szBuf);
	}
	else
	{
		return errCode;
	}

	/* Parse "TimeScale" */

	VK_MEM_Memset(szBuf, 0x00, SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
  	errCode = local_manifestparser_get_attr_value(sstrElement,(xmlChar*)SSTR_ATTR_TIMESCALE,szBuf,SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	if(errCode == SSTR_ERR_ATTR_NOTFOUND)
	{
		sstrMedia->ullTimeScale = 0;
	}
	else if(errCode == SSTR_ERR_OK)
	{
		sstrMedia->ullTimeScale = _atoui64(szBuf);
	}
	else
	{
		//sstrMedia->ullTimeScale = 0;
		return errCode;
	}

	/* Parse "Duration" */
	VK_MEM_Memset(szBuf, 0x00, SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
  	errCode = local_manifestparser_get_attr_value(sstrElement,(xmlChar*)SSTR_ATTR_DURATION,szBuf,SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	if(errCode == SSTR_ERR_ATTR_NOTFOUND)
	{
		sstrMedia->ullDuration = 0;
	}
	else if(errCode == SSTR_ERR_OK)
	{
		sstrMedia->ullDuration = _atoui64(szBuf);
	}
	else
	{
		return errCode;
	}

	/* Live Smooth Streaming */
	/* Parse "IsLive" */
	VK_MEM_Memset(szBuf, 0x00, SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	errCode = local_manifestparser_get_attr_value(sstrElement,(xmlChar*)SSTR_ATTR_ISLIVE,szBuf,SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	if(errCode == SSTR_ERR_ATTR_NOTFOUND)
	{
		sstrMedia->bIsLive = FALSE;
	}
	else if(errCode == SSTR_ERR_OK)
	{
		if(!VK_strncmp(szBuf,SSTR_VAL_TRUE,VK_strlen(SSTR_VAL_TRUE)))
		{
			sstrMedia->bIsLive = TRUE;
		}
		else
		{
			sstrMedia->bIsLive = FALSE;
		}
	}
	else
	{
		return errCode;
	}

	if(sstrMedia->bIsLive == TRUE)
	{

		/* Parse "LookAheadCount" */
		VK_MEM_Memset(szBuf, 0x00, SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
		errCode = local_manifestparser_get_attr_value(sstrElement,(xmlChar*)SSTR_ATTR_LOOKAHEADCOUNT,szBuf,SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
		if(errCode == SSTR_ERR_OK)
		{
			sstrMedia->nLookAheadFragmentCount = VK_atoi(szBuf);
		}
		else
		{
			VK_MEM_Memset(szBuf, 0x00, SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
			errCode = local_manifestparser_get_attr_value(sstrElement,(xmlChar*)SSTR_ATTR_LOOKAHEADFRAGCOUNT,szBuf,SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
			if(errCode == SSTR_ERR_OK)
			{
				sstrMedia->nLookAheadFragmentCount = VK_atoi(szBuf);
			}
			else
			{
				sstrMedia->nLookAheadFragmentCount = 0;
			}
		}

		/* Parse "DVRWindowLength" */
		VK_MEM_Memset(szBuf, 0x00, SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
		errCode = local_manifestparser_get_attr_value(sstrElement,(xmlChar*)SSTR_ATTR_DVRWINDOWLENGTH,szBuf,SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
		if(errCode == SSTR_ERR_OK)
		{
			sstrMedia->ullDVRWindowLength = _atoui64(szBuf);
		}
		else
		{
			sstrMedia->ullDVRWindowLength = 0;
		}
	}

	/* Protection */
	element = local_manifestparser_get_child_element(sstrElement, (const xmlChar*)SSTR_TAG_PROTECTION);
	if(element == NULL)
	{
		sstrMedia->protection.bIsProtected = 0;
	}
	else
	{
		element = local_manifestparser_get_child_element(element, (const xmlChar*)SSTR_TAG_PROTECTIONHEADER);
		if(element == NULL)
		{
			sstrMedia->protection.bIsProtected = 0;
			return SSTR_ERR_ATTR_NOTFOUND;
		}
		errCode = local_manifestparser_parse_protection(&sstrMedia->protection,element);
		if(errCode != SSTR_ERR_OK)
		{
			sstrMedia->protection.bIsProtected = 0;
			sstrMedia->protection.pHeaderContent = NULL;
			sstrMedia->protection.pDecodedHeaderContent = NULL;
			return SSTR_ERR_ATTR_NOTFOUND;
		}
		errCode = local_manifestparser_get_attr_value(element,(xmlChar*)SSTR_TAG_SYSTEMID,szBuf,SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
		if(errCode == SSTR_ERR_OK)
		{
			VK_strncpy(sstrMedia->protection.systemId, szBuf, SSTR_MANIFESTPARSER_CONST_SYSTEMID_SIZE); /* Size of "SYSTEM ID" 36 */
		}
		else
		{
			sstrMedia->protection.bIsProtected = 0;
			VK_MEM_Free(sstrMedia->protection.pHeaderContent);
			return errCode;
		}
	}
	return SSTR_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Parse Track <br>
 *
 * @param[out] 	track : track Structure
 * @param[in] 	trackElement : Track Element
 *
 * @return  #  sstrErr_e  <br>
 * @see
 */
sstrErr_e local_manifestparser_parse_track(sstrTrack_t *track, sstrElement_t *trackElement)
{
	sstrErr_e errCode = SSTR_ERR_OK;
	char szBuf[SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE]={0}; // check buffer size
	sstrElement_t *customAttrElement = NULL;
	sstrElement_t *attributeElement = NULL;

	if(track == NULL)
	{
		return SSTR_ERR_FAIL;
	}
	/* Parse "Index" */
	VK_MEM_Memset(szBuf, 0x00, SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	errCode = local_manifestparser_get_attr_value(trackElement,(xmlChar*)SSTR_ATTR_INDEX,szBuf,SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	if(errCode == SSTR_ERR_OK)
	{
		track->ulIndex = VK_atoi(szBuf);
		track->bNoIndex = FALSE;
	}
	else
	{
		track->ulIndex = 0;
		track->bNoIndex = TRUE;
		// return errCode;
	}

	/* Parse "Bitrate" */
	VK_MEM_Memset(szBuf, 0x00, SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	errCode = local_manifestparser_get_attr_value(trackElement,(xmlChar*)SSTR_ATTR_BITRATE,szBuf,SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	if(errCode == SSTR_ERR_OK)
	{
		track->ulBitrate = VK_atoi(szBuf);
	}
	else
	{
		track->ulBitrate = 0;
	}
	/* Parse "Max Width" */
	VK_MEM_Memset(szBuf, 0x00, SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	errCode = local_manifestparser_get_attr_value(trackElement,(xmlChar*)SSTR_ATTR_MAXWIDTH,szBuf,SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	if(errCode == SSTR_ERR_OK)
	{
		track->ulMaxWidth = VK_atoi(szBuf);
	}
	else
	{
		track->ulMaxWidth = 0;
	}
	/* Parse "Max Height" */
	VK_MEM_Memset(szBuf, 0x00, SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	errCode = local_manifestparser_get_attr_value(trackElement,(xmlChar*)SSTR_ATTR_MAXHEIGHT,szBuf,SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	if(errCode == SSTR_ERR_OK)
	{
		track->ulMaxHeight = VK_atoi(szBuf);
	}
	else
	{
		track->ulMaxHeight = 0;
	}
	/* Parse "Codec Private Data" */
	VK_MEM_Memset(szBuf, 0x00, SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	errCode = local_manifestparser_get_attr_value(trackElement,(xmlChar*)SSTR_ATTR_CODECPRIVATEDATA,szBuf,SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	if(errCode == SSTR_ERR_OK && VK_strlen(szBuf) > 0)
	{
		if(track->nCodecPrivateDataLen < (int)VK_strlen(szBuf))
		{
			if(track->pCodecPrivateData != NULL)
			{
				VK_MEM_Free(track->pCodecPrivateData);
			}
			track->pCodecPrivateData = (char*)VK_MEM_Alloc(VK_strlen(szBuf) + 1);
			if(track->pCodecPrivateData == NULL)
			{
				return SSTR_ERR_MEMORY_ALLOC;
			}
		}
		track->nCodecPrivateDataLen = VK_strlen(szBuf);
		VK_MEM_Memset(track->pCodecPrivateData,0x00,track->nCodecPrivateDataLen + 1);
		VK_strncpy(track->pCodecPrivateData,szBuf,track->nCodecPrivateDataLen);
	}
	else
	{
		if(track->pCodecPrivateData != NULL)
		{
			VK_MEM_Free(track->pCodecPrivateData);
			track->nCodecPrivateDataLen = 0;
		}
	}
	/* Parse "Sampling Rate" */
	VK_MEM_Memset(szBuf, 0x00, SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	errCode = local_manifestparser_get_attr_value(trackElement,(xmlChar*)SSTR_ATTR_SAMPLERATE,szBuf,SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	if(errCode == SSTR_ERR_OK)
	{
		track->ulSamplingRate = VK_atoi(szBuf);
	}
	else
	{
		track->ulSamplingRate = 0;
	}
	/* Parse "Channels" */
	VK_MEM_Memset(szBuf, 0x00, SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	errCode = local_manifestparser_get_attr_value(trackElement,(xmlChar*)SSTR_ATTR_CHANNELS,szBuf,SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	if(errCode == SSTR_ERR_OK)
	{
		track->ulChannels = VK_atoi(szBuf);
	}
	else
	{
		track->ulChannels = 0;
	}
	/* Parse "Sampling Rate" */
	VK_MEM_Memset(szBuf, 0x00, SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	errCode = local_manifestparser_get_attr_value(trackElement,(xmlChar*)SSTR_ATTR_BITSPERSAMPLES,szBuf,SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	if(errCode == SSTR_ERR_OK)
	{
		track->ulBitsPerSample = VK_atoi(szBuf);
	}
	else
	{
		track->ulBitsPerSample = 0;
	}
	/* Parse "Packet Size" */
	VK_MEM_Memset(szBuf, 0x00, SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	errCode = local_manifestparser_get_attr_value(trackElement,(xmlChar*)SSTR_ATTR_PACKETSIZE,szBuf,SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	if(errCode == SSTR_ERR_OK)
	{
		track->ulPacketSize= VK_atoi(szBuf);
	}
	else
	{
		track->ulPacketSize = 0;
	}

	/* Parse "Audio Tag" */
	VK_MEM_Memset(szBuf, 0x00, SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	errCode = local_manifestparser_get_attr_value(trackElement,(xmlChar*)SSTR_ATTR_AUDIOTAG,szBuf,SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	if(errCode == SSTR_ERR_OK)
	{
		track->ulAudioTag = VK_atoi(szBuf);
	}
	else
	{
		track->ulAudioTag = 0;
	}

	/* Parse "FourCC" */
	VK_MEM_Memset(szBuf, 0x00, SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	errCode = local_manifestparser_get_attr_value(trackElement,(xmlChar*)SSTR_ATTR_FOURCC,szBuf,SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	if(errCode == SSTR_ERR_OK)
	{
		VK_MEM_Memset(track->FourCC,0x00,sizeof(track->FourCC));
		VK_strncpy(track->FourCC,szBuf,4);
	}
	else
	{
		VK_MEM_Memset(track->FourCC,0x00,sizeof(track->FourCC));
	}

	/* Parse "NALUnitLengthField" */
	VK_MEM_Memset(szBuf, 0x00, SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	errCode = local_manifestparser_get_attr_value(trackElement,(xmlChar*)SSTR_ATTR_NALUNITLENGTHFIELD,szBuf,SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	if(errCode == SSTR_ERR_OK)
	{
		track->ulNALUnitLengthField = VK_atoi(szBuf);
	}
	else
	{
		track->ulNALUnitLengthField = 0;
	}

	/* parse "TrackContent(Custom Attribute)" */
#if 1
	VK_MEM_Memset(szBuf, 0x00, SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	customAttrElement = local_manifestparser_get_child_element(trackElement, (const xmlChar*) SSTR_ATTR_CUSTOMATTRIBUTES);
	if(customAttrElement != NULL)
	{
		attributeElement = local_manifestparser_get_child_element(customAttrElement, (const xmlChar*)SSTR_ATTR_ATTRIBUTE);
//		track->ulTrackContents = local_manifestparser_get_element_countByfirstElement(attributeElement, (const xmlChar*)SSTR_ATTR_ATTRIBUTE);
		while(attributeElement != NULL)
		{
			sstrTrackContent_t *pTrackContent = (sstrTrackContent_t*)VK_MEM_Alloc(sizeof(sstrTrackContent_t));
			if(pTrackContent == NULL)
			{
				return SSTR_ERR_MEMORY_ALLOC;
			}
			VK_MEM_Memset(szBuf, 0x00, SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
			VK_MEM_Memset(pTrackContent,0x00,sizeof(sstrTrackContent_t));
			errCode = local_manifestparser_get_attr_value(attributeElement,(xmlChar*)SSTR_ATTR_CUSTOMATTR_NAME,szBuf,SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
			if(errCode == SSTR_ERR_OK && VK_strlen(szBuf) > 0)
			{
				pTrackContent->nNameLen = VK_strlen(szBuf);
				pTrackContent->pName = (char*)VK_MEM_Alloc(pTrackContent->nNameLen + 1);
				VK_MEM_Memset(pTrackContent->pName,0x00,pTrackContent->nNameLen + 1);
				VK_strncpy(pTrackContent->pName,szBuf,pTrackContent->nNameLen);
			}
			VK_MEM_Memset(szBuf, 0x00, SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
			errCode = local_manifestparser_get_attr_value(attributeElement,(xmlChar*)SSTR_ATTR_CUSTOMATTR_VALUE,szBuf,SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
			if(errCode == SSTR_ERR_OK && VK_strlen(szBuf) > 0)
			{
				pTrackContent->nValueLen = VK_strlen(szBuf);
				pTrackContent->pValue = (char*)VK_MEM_Alloc(pTrackContent->nValueLen + 1);
				VK_MEM_Memset(pTrackContent->pValue,0x00,pTrackContent->nValueLen + 1);
				VK_strncpy(pTrackContent->pValue,szBuf,pTrackContent->nValueLen);
			}
			else
			{
				if(pTrackContent != NULL)
				{
					if(pTrackContent->pValue != NULL)
					{
						VK_MEM_Free(pTrackContent->pValue);
					}
					if(pTrackContent->pName != NULL)
					{
						VK_MEM_Free(pTrackContent->pName);
					}
					VK_MEM_Free(pTrackContent);
				}
				return errCode;
			}
			UTIL_DLLIST_Append(&track->trackContentSet, pTrackContent);
			attributeElement = local_manifestparser_get_next_element(attributeElement, (xmlChar*)SSTR_ATTR_ATTRIBUTE);
		}
	}
#endif
	return SSTR_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Parse Fragment <br>
 *
 * @param[out] 	fragment : fragment Structure
 * @param[in] 	fragmentElement : Fragment Element
 *
 * @return  #  sstrErr_e  <br>
 * @see
 */
sstrErr_e local_manifestparser_parse_fragment(sstrFragment_t *fragment, sstrElement_t *fragmentElement)
{
	sstrErr_e errCode = SSTR_ERR_OK;
	char szBuf[SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE]={0}; // check buffer size

	/* Parse "Fragment Number" */
	VK_MEM_Memset(szBuf, 0x00, SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	errCode = local_manifestparser_get_attr_value(fragmentElement,(xmlChar*)SSTR_ATTR_FRAGMENTNUMBER,szBuf,SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	if(errCode == SSTR_ERR_OK)
	{
		fragment->ulNum = VK_atoi(szBuf);
	}

	/* Parse "Fragment Duration" */
	VK_MEM_Memset(szBuf, 0x00, SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	errCode = local_manifestparser_get_attr_value(fragmentElement,(xmlChar*)SSTR_ATTR_FRAGMENTDURATION,szBuf,SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	if(errCode == SSTR_ERR_OK)
	{
		fragment->ullDuration = _atoui64(szBuf);
	}
	else
	{
		fragment->ullDuration = 0;
	}

	/* Parse "Fragment Time" */
	VK_MEM_Memset(szBuf, 0x00, SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	errCode = local_manifestparser_get_attr_value(fragmentElement,(xmlChar*)SSTR_ATTR_FRAGMENTTIME,szBuf,SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	if(errCode == SSTR_ERR_OK)
	{
		fragment->ullTime = _atoui64(szBuf);
	}
	else
	{
		fragment->ullTime = 0;
	}
	/* Parse "Track Fragment" not supported yet" */
	fragment->pulIndex = NULL;
	fragment->pExtensionTrackData = NULL;

	return SSTR_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Parse Stream <br>
 * Garbage Collector 필요한 Resource 가 존재할 경우 Allocation 하지 않음 <br>
 *
 * @param[out] 	stream : Stream Structure
 * @param[in] 	streamElement : Stream Element
 * @param[in] 	pGC : Garbage Collector
 *
 * @return  #  sstrErr_e  <br>
 * @see
 */
sstrErr_e local_manifestparser_parse_stream(sstrStream_t *stream, sstrElement_t *streamElement, sstrParserGC_t *pGC)
{
	HUINT64 ullStartTime = 0;
	unsigned long ulQualityLevels = 0;
	unsigned long ulChunks = 0;
	sstrErr_e errCode = SSTR_ERR_OK;
	sstrElement_t *trackElement = NULL;
	sstrElement_t *fragmentElement = NULL;
	sstrTrack_t* pTrack = NULL;
	sstrFragment_t* pFragment = NULL;
	sstrNode_t *pNode = NULL;
	sstrNode_t *pTrackNode = NULL;
	sstrNode_t *pFragNode = NULL;
	char szBuf[SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE]={0}; // check buffer size
	if(stream == NULL)
	{
		return SSTR_ERR_FAIL;
	}
	/* Parse "Index" */
	VK_MEM_Memset(szBuf, 0x00, SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	errCode = local_manifestparser_get_attr_value(streamElement,(xmlChar*)SSTR_ATTR_INDEX,szBuf,SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	if(errCode == SSTR_ERR_OK)
	{
		stream->ulIndex = VK_atoi(szBuf);
		stream->bNoIndex = FALSE;
	}
	else
	{
		stream->ulIndex = 0;
		stream->bNoIndex = TRUE;
		// return errCode;
	}
	/* Parse "Stream Type" Video/Audio/Text */
	VK_MEM_Memset(szBuf, 0x00, SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	errCode = local_manifestparser_get_attr_value(streamElement,(xmlChar*)SSTR_ATTR_TYPE,szBuf,SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	if(errCode == SSTR_ERR_OK)
	{
		if(NULL != VK_strstr(szBuf, SSTR_VAL_TYPE_VIDEO))
		{
			stream->eType = SSTR_TYPE_VIDEO;
		}
		else if(NULL != VK_strstr(szBuf, SSTR_VAL_TYPE_AUDIO))
		{
			stream->eType = SSTR_TYPE_AUDIO;
		}
		else if(NULL != VK_strstr(szBuf, SSTR_VAL_TYPE_TEXT))
		{
			stream->eType = SSTR_TYPE_TEXT;
		}
		else
		{
			return SSTR_ERR_FAIL;
		}
	}
	else
	{
		return errCode;
	}
	/* Parse "SubType" */
	VK_MEM_Memset(szBuf, 0x00, SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	errCode = local_manifestparser_get_attr_value(streamElement,(xmlChar*)SSTR_ATTR_SUBTYPE,szBuf,SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	if(errCode == SSTR_ERR_OK)
	{
		VK_MEM_Memset(stream->subType,0x00,sizeof(stream->subType));
		VK_strncpy(stream->subType,szBuf,4);
	}
	/* Parse "Stream TimeScale" */
	VK_MEM_Memset(szBuf, 0x00, SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	errCode = local_manifestparser_get_attr_value(streamElement,(xmlChar*)SSTR_ATTR_STREAMTIMESCALE,szBuf,SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	if(errCode == SSTR_ERR_ATTR_NOTFOUND)
	{
		stream->ullTimeScale = 0;
	}
	else if(errCode == SSTR_ERR_OK)
	{
		stream->ullTimeScale = _atoui64(szBuf);
	}
	else
	{
		return errCode;
	}

	/* Parse "Name" */
	VK_MEM_Memset(szBuf, 0x00, SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	errCode = local_manifestparser_get_attr_value(streamElement,(xmlChar*)SSTR_ATTR_NAME,szBuf,SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	if(errCode == SSTR_ERR_OK && VK_strlen(szBuf) > 0)
	{
		if(stream->nNameLen < (int)VK_strlen(szBuf))
		{
			if(stream->pName != NULL)
			{
				VK_MEM_Free(stream->pName);
			}
			stream->pName = (char*)VK_MEM_Alloc(VK_strlen(szBuf) + 1);
			if(stream->pName == NULL)
			{
				return SSTR_ERR_MEMORY_ALLOC;
			}
		}
		stream->nNameLen = VK_strlen(szBuf);
		VK_MEM_Memset(stream->pName,0x00,stream->nNameLen + 1);
		VK_strncpy(stream->pName,szBuf,stream->nNameLen);
	}
	else
	{
		if(stream->pName != NULL)
		{
			VK_MEM_Free(stream->pName);
			stream->nNameLen = 0;
		}
	}
	/* Parse "Language" */
	if(stream->eType == SSTR_TYPE_AUDIO)
	{
		VK_MEM_Memset(szBuf, 0x00, SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
		errCode = local_manifestparser_get_attr_value(streamElement,(xmlChar*)SSTR_ATTR_LANGUAGE,szBuf,SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
		if(errCode == SSTR_ERR_OK && VK_strlen(szBuf) > 0)
		{
			if(stream->nLangLen < (int)VK_strlen(szBuf))
			{
				if(stream->pLanguage != NULL)
				{
					VK_MEM_Free(stream->pLanguage);
				}
				stream->pLanguage = (char*)VK_MEM_Alloc(VK_strlen(szBuf) + 1);
				if(stream->pLanguage == NULL)
				{
					return SSTR_ERR_MEMORY_ALLOC;
				}
			}
			stream->nLangLen = VK_strlen(szBuf);
			VK_MEM_Memset(stream->pLanguage,0x00,stream->nLangLen + 1);
			VK_strncpy(stream->pLanguage,szBuf,stream->nLangLen);
		}
		else
		{
			if(stream->pLanguage != NULL)
			{
				VK_MEM_Free(stream->pLanguage);
				stream->nLangLen = 0;
			}
		}
	}
	else
	{
		if(stream->pLanguage != NULL)
		{
			VK_MEM_Free(stream->pLanguage);
			stream->nLangLen = 0;
		}
	}
	/* Parse "Number of Fragments(Chunks)" */
	VK_MEM_Memset(szBuf, 0x00, SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	errCode = local_manifestparser_get_attr_value(streamElement,(xmlChar*)SSTR_ATTR_NUMOFFRAGMENTS,szBuf,SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);

	if(errCode == SSTR_ERR_OK)
	{
		ulChunks = VK_atoi(szBuf);
	}

	if(ulChunks <= 0 || errCode != SSTR_ERR_OK)
	{
		fragmentElement = local_manifestparser_get_child_element(streamElement, (xmlChar*)SSTR_TAG_STREAMFRAGMENT);
		ulChunks = local_manifestparser_get_element_countByfirstElement(fragmentElement,(xmlChar*)SSTR_TAG_STREAMFRAGMENT);
		if(ulChunks <= 0)
		{
			return errCode;
		}
	}
	/* Parse "Number of Tracks(QualityLevels)" */
	VK_MEM_Memset(szBuf, 0x00, SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	errCode = local_manifestparser_get_attr_value(streamElement,(const xmlChar*)SSTR_ATTR_NUMOFTRACKS,szBuf,SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	if(errCode == SSTR_ERR_OK)
	{
		ulQualityLevels = VK_atoi(szBuf);
	}
	else
	{
		trackElement = local_manifestparser_get_child_element(streamElement, (xmlChar*)SSTR_TAG_TRACK);
		ulQualityLevels = local_manifestparser_get_element_countByfirstElement(trackElement, (xmlChar*)SSTR_TAG_TRACK);
		if(ulQualityLevels <= 0)
		{
			return errCode;
		}
	}

	/* Parse "Url" */
	VK_MEM_Memset(szBuf, 0x00, SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	errCode = local_manifestparser_get_attr_value(streamElement,(xmlChar*)SSTR_ATTR_URL,szBuf,SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	if(errCode == SSTR_ERR_OK && VK_strlen(szBuf) > 0)
	{
		if(stream->nUrlLen < (int)VK_strlen(szBuf))
		{
			if(stream->pUrlPattern != NULL)
			{
				VK_MEM_Free(stream->pUrlPattern);
			}
			stream->pUrlPattern = (char*)VK_MEM_Alloc(VK_strlen(szBuf) + 1);
			if(stream->pUrlPattern == NULL)
			{
				return SSTR_ERR_MEMORY_ALLOC;
			}
		}
		stream->nUrlLen = VK_strlen(szBuf);
		VK_MEM_Memset(stream->pUrlPattern,0x00,stream->nUrlLen + 1);
		VK_strncpy(stream->pUrlPattern,szBuf,stream->nUrlLen);
	}
	else
	{
		if(stream->pUrlPattern != NULL)
		{
			VK_MEM_Free(stream->pUrlPattern);
			stream->nUrlLen = 0;
		}
		return errCode;
	}

	/* Parse "Stream Max Width" */
	VK_MEM_Memset(szBuf, 0x00, SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	errCode = local_manifestparser_get_attr_value(streamElement,(xmlChar*)SSTR_ATTR_STREAMMAXWIDTH,szBuf,SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	if(errCode == SSTR_ERR_OK)
	{
		stream->ulMaxWidth = VK_atoi(szBuf);
	}
	else
	{
		stream->ulMaxWidth = 0;
	}

	/* Parse "Stream Max Height" */
	VK_MEM_Memset(szBuf, 0x00, SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	errCode = local_manifestparser_get_attr_value(streamElement,(xmlChar*)SSTR_ATTR_STREAMMAXHEIGHT,szBuf,SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	if(errCode == SSTR_ERR_OK)
	{
		stream->ulMaxHeight = VK_atoi(szBuf);
	}
	else
	{
		stream->ulMaxHeight = 0;
	}

	/* Parse "Display Width" */
	VK_MEM_Memset(szBuf, 0x00, SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	errCode = local_manifestparser_get_attr_value(streamElement,(xmlChar*)SSTR_ATTR_DISPLAYWIDTH,szBuf,SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	if(errCode == SSTR_ERR_OK)
	{
		stream->ulDisplayWidth= VK_atoi(szBuf);
	}
	else
	{
		stream->ulDisplayWidth = 0;
	}

	/* Parse "Display Height" */
	VK_MEM_Memset(szBuf, 0x00, SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	errCode = local_manifestparser_get_attr_value(streamElement,(xmlChar*)SSTR_ATTR_DISPLAYHEIGHT,szBuf,SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE);
	if(errCode == SSTR_ERR_OK)
	{
		stream->ulDisplayHeight= VK_atoi(szBuf);
	}
	else
	{
		stream->ulDisplayHeight = 0;
	}

	/* Parse "Track(QualityLevel)" */
	trackElement = local_manifestparser_get_child_element(streamElement, (xmlChar*)SSTR_TAG_TRACK);
//	stream->trackSet = (sstrTrack_t*)VK_MEM_Alloc(sizeof(sstrTrack_t) * stream->ulQualityLevels);
	while(trackElement != NULL)
	{
		if(pGC != NULL )
		{
			pTrackNode = UTIL_DLLIST_ExtractHead (&pGC->trackGC);
			if(pTrackNode != NULL)
			{
				if(pTrackNode->pData != NULL)
				{
					pTrack = (sstrTrack_t*)pTrackNode->pData;
				}
			}
		}
		if(pTrack == NULL)
		{
			pTrack = (sstrTrack_t*)VK_MEM_Alloc(sizeof(sstrTrack_t));
			if(pTrack == NULL)
			{
				if(pTrackNode != NULL)
				{
					UTIL_DLLIST_AppendNode(&pGC->trackGC, pTrackNode);
				}
				return SSTR_ERR_MEMORY_ALLOC;
			}
			if( pTrackNode != NULL )
			{
				pTrackNode->pData = pTrack;
			}
			VK_MEM_Memset(pTrack,0x00,sizeof(sstrTrack_t));
		}
/*
		pTrack = (sstrTrack_t*)VK_MEM_Alloc(sizeof(sstrTrack_t));
		if(pTrack == NULL)
		{
			return SSTR_ERR_MEMORY_ALLOC;
		}
		VK_MEM_Memset(pTrack,0x00,sizeof(sstrTrack_t));
*/
		errCode = local_manifestparser_parse_track(pTrack, trackElement);
		if(SSTR_ERR_OK != errCode)
		{
			if(pTrackNode != NULL)
			{
				UTIL_DLLIST_AppendNode(&pGC->trackGC, pTrackNode);
			}
			else
			{
				if(pTrack != NULL)
				{
					VK_MEM_Free(pTrack);
				}
			}
			return errCode;
		}
		if(pTrackNode == NULL)
		{
			UTIL_DLLIST_Append(&stream->trackSet, pTrack);
		}
		else
		{
			UTIL_DLLIST_AppendNode(&stream->trackSet, pTrackNode);
		}
		pTrackNode = NULL;
		pTrack = NULL;
//		UTIL_DLLIST_Append(&stream->trackSet, pTrack);
		trackElement = local_manifestparser_get_next_element(trackElement, (xmlChar*)SSTR_TAG_TRACK);
	}
	/* Parse "Stream Fragment(Chunk)" */
	fragmentElement = local_manifestparser_get_child_element(streamElement, (const xmlChar*)SSTR_TAG_STREAMFRAGMENT);
	while(fragmentElement != NULL)
	{
		if(pGC != NULL )
		{
			pFragNode = UTIL_DLLIST_ExtractHead (&pGC->fragmentGC);
			if(pFragNode != NULL)
			{
				if(pFragNode->pData != NULL)
				{
					pFragment = (sstrFragment_t*)pFragNode->pData;
				}
			}
		}
		if(pFragment == NULL)
		{
			pFragment = (sstrFragment_t*)VK_MEM_Alloc(sizeof(sstrFragment_t));
			if(pFragment == NULL)
			{
				if(pFragNode != NULL)
				{
					UTIL_DLLIST_AppendNode(&pGC->fragmentGC, pFragNode);
				}
				return SSTR_ERR_MEMORY_ALLOC;
			}
			if( pFragNode != NULL )
			{
				pFragNode->pData = pFragment;
			}
			VK_MEM_Memset(pFragment,0x00,sizeof(sstrFragment_t));
		}
/*
		pFragment = (sstrFragment_t*)VK_MEM_Alloc(sizeof(sstrFragment_t));
		if(pFragment == NULL)
		{
			return SSTR_ERR_MEMORY_ALLOC;
		}
		VK_MEM_Memset(pFragment,0x00,sizeof(sstrFragment_t));
*/
		errCode = local_manifestparser_parse_fragment(pFragment, fragmentElement);
		if(SSTR_ERR_OK != errCode)
		{
			if(pFragNode != NULL)
			{
				UTIL_DLLIST_AppendNode(&pGC->fragmentGC, pFragNode);
			}
			else
			{
				if(pFragment != NULL)
				{
					VK_MEM_Free(pFragment);
				}
			}
			return errCode;
		}
		if(pFragNode == NULL)
		{
			UTIL_DLLIST_Append(&stream->fragmentSet, pFragment);
		}
		else
		{
			UTIL_DLLIST_AppendNode(&stream->fragmentSet, pFragNode);
		}
		pFragNode = NULL;
		pFragment = NULL;
//		UTIL_DLLIST_Append(&stream->fragmentSet, pFragment);
		fragmentElement = local_manifestparser_get_next_element(fragmentElement, (xmlChar*)SSTR_TAG_STREAMFRAGMENT);
	}

	pNode = UTIL_DLLIST_GetHeadNode(&stream->fragmentSet);
	while(pNode != NULL)
	{
		pFragment = (sstrFragment_t*)pNode->pData;
		if(pFragment != NULL)
		{
			if(pFragment->ullTime <= 0)
			{
					pFragment->ullStartTime = ullStartTime;
					ullStartTime += pFragment->ullDuration;
			}
			else
			{
					pFragment->ullStartTime = pFragment->ullTime;
					ullStartTime = pFragment->ullTime + pFragment->ullDuration;
			}
		}
		else
		{
			return SSTR_ERR_FAIL;
		}
		pNode = UTIL_DLLIST_NextNode(pNode);
	}
	return SSTR_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Parse Manifest (from File)  <br>
 *
 * @param[in] 	szPath : Manifest 파일 Path
 *
 * @return  #  Parser Handle  <br>
 * @see
 */
sstrParser_t* SSTR_ManifestParser_FileOpen(const char *szPath)
{
	sstrMedia_t *pMedia = NULL;
	sstrParser_t *parser = (sstrParser_t *)VK_MEM_Alloc(sizeof(sstrParser_t));
	if ( parser == NULL )
	{
		return NULL;
	}
	VK_MEM_Memset(parser,0x0,sizeof(sstrParser_t));
	pMedia = (sstrMedia_t *)VK_MEM_Alloc(sizeof(sstrMedia_t));
	if (pMedia == NULL)
	{
		// SSTR_ManifestParser_Close(parser);
		if(parser != NULL)
		{
			VK_MEM_Free(parser);
		}
		return NULL;
	}
	VK_MEM_Memset(pMedia,0x0,sizeof(sstrMedia_t));

	parser->media = pMedia;
	parser->document = (void *)xmlParseFile(szPath);
	if ( parser->document == NULL )
	{
		//SSTR_ManifestParser_Close(parser);
		if(parser != NULL)
		{
			VK_MEM_Free(parser);
		}
		return NULL;
	}
	parser->root = (void *)xmlDocGetRootElement(__xmldoc(parser->document));
	if (parser->root == NULL)
	{
		SSTR_ManifestParser_Close(parser,NULL);
		return NULL;
	}
	return parser;
}

/**
 * @b Function @b Description <br>
 * Parse Manifest (from Memory)  <br>
 *
 * @param[in] 	pBuffer : stored manifest in the memory buffer
 * @param[in] 	nBufsize : size of the memory buffer
 *
 * @return  #  Parser Handle  <br>
 * @see
 */
sstrParser_t* SSTR_ManifestParser_MemOpen(const char *pBuffer,int64_t nBufsize)
{
	sstrMedia_t *pMedia = NULL;
	sstrParser_t *parser = (sstrParser_t *)VK_MEM_Alloc(sizeof(sstrParser_t));
	if ( parser == NULL )
	{
		return NULL;
	}
	VK_MEM_Memset(parser,0x0,sizeof(sstrParser_t));
	pMedia = (sstrMedia_t *)VK_MEM_Alloc(sizeof(sstrMedia_t));
	if (pMedia == NULL)
	{
		SSTR_ManifestParser_Close(parser,NULL);
		return NULL;
	}
//	xmlInitParser();
	VK_MEM_Memset(pMedia,0x00,sizeof(sstrMedia_t));
	parser->media = (void*)pMedia;
	parser->document = (void *)xmlParseMemory(pBuffer,nBufsize);
	if ( parser->document == NULL )
	{
		SSTR_ManifestParser_Close(parser,NULL);
		return NULL;
	}
	parser->root = (void *)xmlDocGetRootElement(__xmldoc(parser->document));
	if (parser->root == NULL)
	{
		SSTR_ManifestParser_Close(parser,NULL);
		return NULL;
	}
	return parser;
}

/**
 * @b Function @b Description <br>
 * Parser Handle 제거 <br>
 * Garbage Collector 가 NULL 이 아니면 Resource 를 제거하지 않고 GC 로 수집한다.
 *
 * @param[in] 	parser : Parser Handle
 * @param[in] 	pGC : Garbage Collector (optional)
 *
 * @return  #  void  <br>
 * @see
 */
void SSTR_ManifestParser_Close(sstrParser_t *parser, sstrParserGC_t *pGC)
{
	if ( parser )
	{
		if(pGC != NULL)
		{
			local_manifestparser_gc_collect(__sstrMedia(parser->media),pGC);
		}
		local_manifestparser_gc_free(&parser->gc);
		local_media_free(__sstrMedia(parser->media));

		if ( parser->document )
		{
			xmlFreeDoc(__xmldoc(parser->document));
			parser->document = NULL;
		}
		parser->root = NULL;
		VK_MEM_Free(parser);
		parser = NULL;
		xmlCleanupParser();
		// malloc_trim(0); /* Risk 에 대한 검증 이 필요하여 Block 처리 */
	}
}

/**
 * @b Function @b Description <br>
 * Manifest Parsing <br>
 * Garbage Collector 가 NULL 이 아니면 GC의 Resource 를 활용한다.
 *
 * @param[in] 	parser : Parser Handle
 * @param[in] 	pGC : Garbage Collector (optional)
 *
 * @return  #  sstrErr_e  <br>
 * @see
 */
sstrErr_e SSTR_ManifestParser_Parse(sstrParser_t *parser, sstrParserGC_t *pGC)
{
	sstrErr_e errCode = SSTR_ERR_OK;
	sstrMedia_t *media = NULL;
	sstrElement_Media_t *mediaElement = NULL;
	sstrElement_t *streamElement;
	sstrStream_t* pStream = NULL;
	sstrNode_t* pStreamNode = NULL;
	if(parser == NULL)
	{
		return SSTR_ERR_FAIL;
	}
	media = __sstrMedia(parser->media);
	mediaElement = local_manifestparser_get_media(parser);
	if(mediaElement == NULL)
	{
		return SSTR_ERR_MEDIA_NOTFOUND;
	}
	errCode = local_manifestparser_parse_media(media, mediaElement);
	if(SSTR_ERR_OK != errCode)
	{
		return errCode;
	}
	streamElement = local_manifestparser_get_child_element(mediaElement, (xmlChar*)SSTR_TAG_STREAM);
//	nStreamCnt = local_manifestparser_get_element_countByfirstElement(streamElement, SSTR_TAG_STREAM);
//	media->streamSet = (sstrStream_t*)VK_MEM_Alloc(sizeof(sstrStream_t) * media->nStreamCnt);
	while(streamElement != NULL)
	{
		if(pGC != NULL )
		{
			pStreamNode = UTIL_DLLIST_ExtractHead (&pGC->streamGC);
			if(pStreamNode != NULL)
			{
				if(pStreamNode->pData != NULL)
				{
					pStream = (sstrStream_t*)pStreamNode->pData;
				}
			}
		}
		if(pStream == NULL)
		{
			pStream = (sstrStream_t*)VK_MEM_Alloc(sizeof(sstrStream_t));
			if(pStream == NULL)
			{
				if(pStreamNode != NULL)
				{
					UTIL_DLLIST_AppendNode(&pGC->streamGC, pStreamNode);
				}
				return SSTR_ERR_MEMORY_ALLOC;
			}
			if(	pStreamNode != NULL )
			{
				pStreamNode->pData = pStream;
			}
			VK_MEM_Memset(pStream,0x00,sizeof(sstrStream_t));
		}
		errCode = local_manifestparser_parse_stream(pStream, streamElement, pGC);
		if(SSTR_ERR_OK != errCode)
		{
			if(pStream != NULL)
			{
				if(pStreamNode != NULL)
				{
					UTIL_DLLIST_AppendNode(&pGC->streamGC, pStreamNode);
				}
				else
				{
					VK_MEM_Free(pStream);
				}
			}
			return errCode;
		}
		if(pStreamNode == NULL)
		{
			UTIL_DLLIST_Append(&media->streamSet, pStream);
		}
		else
		{
			UTIL_DLLIST_AppendNode(&media->streamSet, pStreamNode);
		}
		//PrintDebug ("() TRACK %d / FRAG %d\n",UTIL_DLLIST_Size(&pStream->trackSet),UTIL_DLLIST_Size(&pStream->fragmentSet));
		pStreamNode = NULL;
		pStream = NULL;
		streamElement = local_manifestparser_get_next_element(streamElement, (xmlChar*)SSTR_TAG_STREAM);
	}
/*
	if ( parser->document )
	{
		xmlFreeDoc(__xmldoc(parser->document));
		parser->document = NULL;
	}
	parser->root = NULL;
	xmlCleanupParser();
	malloc_trim(0);
*/
	return SSTR_ERR_OK;
}

/**
 * @b Function @b Description <br>
 * Manifest Merging <br>
 *
 * @param[in] 	DstParser : 합쳐질 Parser
 * @param[in] 	SrcParser : Merging 후 제거될 Parser
 *
 * @return  #  sstrErr_e  <br>
 * @see
 */
sstrErr_e SSTR_ManifestParser_Merge(sstrParser_t *DstParser, sstrParser_t *SrcParser)
{
	sstrNode_t *pSrcStreamNode = NULL;
	sstrNode_t *pDstStreamNode = NULL;
//	sstrNode_t *pSrcTrackNode = NULL;
//	sstrNode_t *pDstTrackNode = NULL;
	sstrNode_t *pSrcFragNode = NULL;
	sstrNode_t *pDstFragNode = NULL;
	sstrStream_t *pSrcStream = NULL;
	sstrStream_t *pDstStream = NULL;
//	sstrTrack_t *pSrcTrack = NULL;
//	sstrTrack_t *pDstTrack = NULL;
	sstrFragment_t *pSrcFrag = NULL;
	sstrFragment_t *pDstFrag = NULL;
	sstrMedia_t *pDstMedia = NULL;
	sstrMedia_t *pSrcMedia = NULL;
	if(DstParser == NULL || SrcParser == NULL)
	{
		return SSTR_ERR_FAIL;
	}
	pDstMedia = __sstrMedia(DstParser->media);
	pSrcMedia = __sstrMedia(SrcParser->media);
	if(DstParser == NULL || SrcParser == NULL)
	{
		return SSTR_ERR_FAIL;
	}

	pDstMedia->bIsLive = pSrcMedia->bIsLive;
	pDstMedia->nMajorVersion = pSrcMedia->nMajorVersion;
	pDstMedia->nMinorVersion = pSrcMedia->nMinorVersion;
	pDstMedia->nLookAheadFragmentCount = pSrcMedia->nLookAheadFragmentCount;
	pDstMedia->ullDuration = pSrcMedia->ullDuration;
	pDstMedia->ullDVRWindowLength = pSrcMedia->ullDVRWindowLength;
	pDstMedia->ullTimeScale = pSrcMedia->ullTimeScale;

	pSrcStreamNode = UTIL_DLLIST_GetHeadNode(&pSrcMedia->streamSet);
	while(pSrcStreamNode != NULL)
	{
		pSrcStream = (sstrStream_t*) pSrcStreamNode->pData;

		pDstStreamNode = UTIL_DLLIST_GetHeadNode(&pDstMedia->streamSet);
		while(pDstStreamNode != NULL)
		{
			pDstStream = (sstrStream_t*) pDstStreamNode->pData;
			if(pDstStream->eType == pSrcStream->eType)
			{

				if((pDstStream->pName != NULL && pSrcStream->pName != NULL && (!VK_strcmp(pDstStream->pName,pSrcStream->pName))) /* compare Name */
				|| ((pDstStream->bNoIndex == FALSE && pSrcStream->bNoIndex == FALSE) && (pDstStream->ulIndex == pSrcStream->ulIndex)) /* compare index */
				|| ((pDstStream->bNoIndex == TRUE && pSrcStream->bNoIndex == TRUE))) /* no index (workaround) */
				{

#if 1 /* workaround */
					if(pDstStream->pUrlPattern != NULL && pSrcStream->pUrlPattern != NULL)
					{
						if(VK_strcmp(pDstStream->pUrlPattern,pSrcStream->pUrlPattern))
						{
							if(pDstStream->pUrlPattern != NULL)
							{
								VK_MEM_Free(pDstStream->pUrlPattern);
							}
							pDstStream->pUrlPattern = VK_StrDup(pSrcStream->pUrlPattern);
							pDstStream->nUrlLen 	= pSrcStream->nUrlLen;
						}
					}
#endif

#if 0 /* Merging Track */
					pSrcTrackNode = UTIL_DLLIST_GetHeadNode(&pSrcStream->trackSet);
					while(pSrcTrackNode != NULL)
					{
						pSrcTrack = (sstrTrack_t*) pSrcTrackNode->pData;

						pDstTrackNode = UTIL_DLLIST_GetHeadNode(&pDstStream->trackSet);
						while(pDstTrackNode != NULL)
						{
							pDstTrack = (sstrFragment_t*) pDstTrackNode->pData;

							/* NEXT NODE */
							pDstTrackNode = UTIL_DLLIST_NextNode(pDstTrackNode);
						}
						/* NEXT NODE */
						pSrcTrackNode = UTIL_DLLIST_NextNode(pSrcTrackNode);
					}
#endif
					/* Merging Fragment */
					pDstFragNode = UTIL_DLLIST_GetTailNode(&pDstStream->fragmentSet);
					if(pDstFragNode == NULL)
					{
						/* Join */
						sstrNode_t *pSplittedNode = NULL;
						pSplittedNode = UTIL_DLLIST_Extract_toEnd(&pSrcStream->fragmentSet,0);

						if(pSplittedNode != NULL)
						{
							UTIL_DLLIST_AppendNode(&pDstStream->fragmentSet, pSplittedNode);
						}
						break;
					}
					else
					{
						HUINT32 unSplitNode = 0;
						sstrNode_t *pSplittedNode = NULL;
#if !defined(SSTR_MANIFESTPARSER_CONFIG_GC_SUPPORT)
						sstrNode_t *pTempNode = NULL;
						sstrFragment_t *pTempFragment = NULL;
#endif
						pDstFrag = (sstrFragment_t*) pDstFragNode->pData;
						pSrcFragNode = UTIL_DLLIST_GetHeadNode(&pSrcStream->fragmentSet);
						while(pSrcFragNode != NULL)
						{
							pSrcFrag = (sstrFragment_t*) pSrcFragNode->pData;
							if(pDstFrag->ullStartTime < pSrcFrag->ullStartTime)
							{
								/* Split Source */
								pSplittedNode = UTIL_DLLIST_Extract_toEnd(&pSrcStream->fragmentSet,unSplitNode);
								if(pSplittedNode != NULL)
								{
									/* Join Splitted Source to Dest */
									UTIL_DLLIST_AppendNode(&pDstStream->fragmentSet, pSplittedNode);
								}
								break;
							}
							/* NEXT NODE */
							pSrcFragNode = UTIL_DLLIST_NextNode(pSrcFragNode);
							unSplitNode++;
						}

						/* Discards Destination FragmentSet */
						unSplitNode = 0;
						pSrcFragNode = UTIL_DLLIST_GetHeadNode(&pSrcStream->fragmentSet);
						pDstFragNode = UTIL_DLLIST_GetHeadNode(&pDstStream->fragmentSet);
						if(pSrcFragNode != NULL && pSrcFragNode->pData != NULL)
						{
							pSrcFrag = (sstrFragment_t*) pSrcFragNode->pData;
							if(pSrcStream->eType == SSTR_TYPE_VIDEO || pSrcStream->eType == SSTR_TYPE_AUDIO)
							{
								pDstMedia->ullStartTime = pSrcFrag->ullStartTime;
							}
							while(pDstFragNode != NULL && pDstFragNode->pData != NULL)
							{
								pDstFrag = (sstrFragment_t*) pDstFragNode->pData;
								if(pDstFrag->ullStartTime >= pSrcFrag->ullStartTime)
								{
									if(unSplitNode == 0 || pDstStream->ulFragmentSeqNum <= unSplitNode)
									{
										break;
									}
									pSplittedNode = UTIL_DLLIST_Extract_toStart(&pDstStream->fragmentSet,unSplitNode);
									if(pSplittedNode != NULL)
									{
										/* Join Splitted Source to Dest */
#if defined(SSTR_MANIFESTPARSER_CONFIG_GC_SUPPORT)
										UTIL_DLLIST_AppendNode(&DstParser->gc.fragmentGC, pSplittedNode);
#else
										pTempNode = pSplittedNode;
										while(pTempNode != NULL)
										{
											if(pTempNode->pData != NULL)
											{
												pTempFragment = (sstrFragment_t*)pTempNode->pData;
												if(pTempFragment->pulIndex != NULL)
												{
													VK_MEM_Free(pTempFragment->pulIndex);
													pTempFragment->pulIndex = NULL;
												}
												if(pTempFragment->pExtensionTrackData != NULL)
												{
													VK_MEM_Free(pTempFragment->pExtensionTrackData);
													pTempFragment->pExtensionTrackData = NULL;
												}
												VK_MEM_Free(pTempNode->pData);
												pTempNode->pData = NULL;
											}
											pTempNode = UTIL_DLLIST_NextNode(pTempNode);
										}
										if(UTIL_DLLIST_DeleteNodes(pSplittedNode) != DLL_ERR_OK)
										{
//											PrintError("%s(%d) Err >> DeleteNodes Fail.\n",__FUNCTION__,__LINE__);
											UTIL_DLLIST_AppendNode(&DstParser->gc.fragmentGC, pSplittedNode);
										}
#endif
										pDstStream->ulFragmentSeqNum -= (unSplitNode+1);
									}
									break;
								}
								else
								{
									pDstFragNode = UTIL_DLLIST_NextNode(pDstFragNode);
									unSplitNode++;
								}
							}
						}
					}
				}
			}
			/* NEXT NODE */
			pDstStreamNode = UTIL_DLLIST_NextNode(pDstStreamNode);
		}
		/* NEXT NODE */
		pSrcStreamNode = UTIL_DLLIST_NextNode(pSrcStreamNode);
	}
	return SSTR_ERR_OK;
}

/*@}*/

