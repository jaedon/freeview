/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   sstr_manifestparser.h  $
 * Version:			$Revision:   $
 * Original Author:	YoungKi Kim
 * Current Author:	$Author: ykkim3@humaxdigital.com $
 * Date:			$Date:   $
 * File Description:	     SSTR Manifest Controller for Streaming Header
 * Module:
 * Remarks:	
 */	

/**
 * @ingroup SSTR_MANIFESTPARSER
 */

/**
 * @author YoungKi Kim(ykkim3@humaxdigital.com)
 * @date 28 Feb 2012
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
 * @file sstr_manifestparser.h
 */


#include "htype.h"
#include "util_dllist.h"

#ifndef __SSTR_MANIFESTPARSER_H__
#define __SSTR_MANIFESTPARSER_H__

/*----------------------------------------------------------------------------------------------
 *	define
 *---------------------------------------------------------------------------------------------*/
/** 
 * Double Linked List 구조로 각 Element 들을 구성
 * \See UTIL_DLList
 */
typedef UTIL_DLL_t sstrStreamSet_t; /**< Stream Structure Set */
typedef UTIL_DLL_t sstrFragmentSet_t; /**< Fragment Structure Set */
typedef UTIL_DLL_t sstrTrackSet_t; /**< Track Structure Set */
typedef UTIL_DLL_t sstrTrackContentSet_t; /**< Custom Track Constent Structure Set */
typedef UTIL_DLLNode_t sstrNode_t; /**< SSTR 의 Structure Set 탐색을 위한 Node Structure redefine */


/*----------------------------------------------------------------------------------------------
 *	Config
 *---------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------
 *	Constant
 *---------------------------------------------------------------------------------------------*/
#define SSTR_MANIFESTPARSER_CONST_UTCFORMAT_SIZE (21)
#ifdef CONFIG_DI_MAX_URL_LENGTH
#define SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE CONFIG_DI_MAX_URL_LENGTH
#else
#define SSTR_MANIFESTPARSER_CONST_ATTR_BUFSIZE (2048)
#endif
#define SSTR_MANIFESTPARSER_CONST_PROTECTION_BUFSIZE (65535)
#define SSTR_MANIFESTPARSER_CONST_SYSTEMID_SIZE (36)

//#define SSTR_MANIFESTPARSER_CONFIG_GC_SUPPORT

typedef void sstrElement_t;
typedef sstrElement_t sstrElement_Media_t;


/** 
 * Garbage Collector
 * \See UTIL_DLList
 */
typedef struct _sstrparser_gc_t {
	UTIL_DLL_t	streamGC; /**< Stream Structure Set */
	UTIL_DLL_t	trackGC; /**< Track Structure Set */
	UTIL_DLL_t	fragmentGC; /**< Fragment Structure Set */
} sstrParserGC_t;

/** 
 * Parser Handle
 */
typedef struct _sstrparser_t {
	void		*document;  /**< document for libxml2 */
	void		*root;  /**< root for libxml2 */
	void		*media; /**< Manifest Data */
	sstrParserGC_t gc; /**< Garbage Collector Handle */
} sstrParser_t;

/** 
 * Parser ErrCode
 */
typedef enum
{
	SSTR_ERR_OK,
	SSTR_ERR_BUFFERSIZE,
	SSTR_ERR_ATTR_NOTFOUND,		
	SSTR_ERR_MEMORY_ALLOC,		
	SSTR_ERR_MEDIA_NOTFOUND,
	SSTR_ERR_CONNECT_PRECONDITION_FAIL,
	SSTR_ERR_CONNECT_FAIL,
	SSTR_ERR_DOWNLOAD_FAIL,
	SSTR_ERR_DOWNLOAD_PANIC,
	SSTR_ERR_DOWNLOAD_FORCED_EXIT,
	SSTR_ERR_FAIL
} sstrErr_e;

/** 
 * Stream Type
 */
typedef enum
{
	SSTR_TYPE_VIDEO,	
	SSTR_TYPE_AUDIO,
	SSTR_TYPE_TEXT,
	SSTR_TYPE_UNKNOWN,
} sstrStreamType_e;

/** 
 * Fragment Structure
 */
typedef struct _sstr_Fragment_t {
	unsigned int ulNum; /**< Fragment Number */
	unsigned long long  ullDuration; /**< Fragment Duration */
	unsigned long long  ullStartTime; /**< Start Time(for control)  */
	unsigned long long  ullTime; /**< Fragment Time */
	
	/* not supported yet */
	unsigned int ulTrackFragmentNum;
	unsigned int *pulIndex;	
	char *pExtensionTrackData; /**< VendorExtensionTrackData(XML) */
} sstrFragment_t;

/** 
 * Track Content Structure
 */
typedef struct _sstr_TrackContent_t {
	int nNameLen;
	char *pName;
	int nValueLen;
	char *pValue;
} sstrTrackContent_t;

/** 
 * Track Structure
 */
typedef struct _sstr_Track_t {
	int bNoIndex;
	unsigned int ulIndex;
	unsigned int ulBitrate;
	unsigned int ulMaxWidth;
	unsigned int ulMaxHeight;
	int nCodecPrivateDataLen;
	char *pCodecPrivateData;
	unsigned int ulSamplingRate;
	unsigned int ulChannels;
	unsigned int ulBitsPerSample;
	unsigned int ulPacketSize;
	unsigned int ulAudioTag;
	char FourCC[5];
	unsigned int ulNALUnitLengthField;
//	unsigned int ulTrackContents;
	sstrTrackContentSet_t trackContentSet; /* TrackContent(Custom Attribute) */
} sstrTrack_t;

/** 
 * Stream Structure
 */
typedef struct _sstr_Stream_t {
	int bNoIndex;
	unsigned int ulIndex;
	sstrStreamType_e eType;
	char subType[5]; 
	unsigned long long  ullTimeScale;
	int nNameLen;
	char *pName;
	int nLangLen;
	char *pLanguage;
//	unsigned int ulChunks; /* Number of Fragments */
//	unsigned int ulQualityLevels; /* Number of Tracks */
	int nUrlLen;
	char *pUrlPattern;
	unsigned int ulMaxWidth;
	unsigned int ulMaxHeight;
	unsigned int ulDisplayWidth;
	unsigned int ulDisplayHeight;
	sstrTrackSet_t trackSet;
	sstrFragmentSet_t fragmentSet;

	/* for Control */
	unsigned int ulFragmentSeqNum; /* seq num of fragment */
	unsigned int ulCurruntTrackNum;
} sstrStream_t;

/** 
 * Protection Structure
 */
typedef struct _sstr_Protection_t {
	int bIsProtected;
	char systemId[40];
	unsigned char *pHeaderContent; /* ProtectionHeaderContent */
	int nHeaderContentLen;
	unsigned char *pDecodedHeaderContent;
	int nDecodedHeaderContentLen;
	char *pLA_Url;
	int nLA_UrlLen;
	sstrParser_t *protectParser;
} sstrProtection_t;

/** 
 * Media Structure
 */
typedef struct _sstr_Media_t {
	int nMajorVersion;
	int nMinorVersion;
	unsigned long long  ullTimeScale;
	unsigned long long  ullDuration;
	unsigned long long  ullStartTime;
	int bIsLive;
	int nLookAheadFragmentCount;
	unsigned long long  ullDVRWindowLength;
//	int nStreamCnt;
//	sstrStream_t *streamSet;
	sstrStreamSet_t streamSet;
	sstrProtection_t protection;
} sstrMedia_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @b Function @b Description <br>
 * Open the SSTR parser  <br>
 *
 * param[in] szPath the pathname placed the SSTR file
 *
 * @return if the SSTR parser opening succeeds, the handle for SSTR parser is returned.
 * On error NULL is returned.
 */
sstrParser_t *SSTR_ManifestParser_FileOpen(const char *szPath);

/**
 * @b Function @b Description <br>
 * Open the SSTR parser <br>
 *
 * param[in] pbuffer buffered SSTR in memory
 *
 * @return if the SSTR parser opening succeeds, the handle for SSTR parser is returned.
 * On error NULL is returned.
 */
sstrParser_t *SSTR_ManifestParser_MemOpen(const char *pBuffer, int64_t nBufsize);

/**
 * @b Function @b Description <br>
 * Close the SSTR parser. <br>
 *
 * param[in] parser the handle for SSTR parser
 */
void SSTR_ManifestParser_Close(sstrParser_t *parser, sstrParserGC_t *pGC);

/**
 * @b Function @b Description <br>
 * Parse basic information for SSTR. <br>
 *
 * param[in] parser the handle for SSTR parser
 */
sstrErr_e SSTR_ManifestParser_Parse(sstrParser_t *parser, sstrParserGC_t *pGC);

/**
 * @b Function @b Description <br>
 * Merging Manifest <br>
 *
 * param[in] DstParser Dest Manifest Parser
 * param[in] SrcParser Source Manifest Parser
 *
 */
sstrErr_e SSTR_ManifestParser_Merge(sstrParser_t *DstParser, sstrParser_t *SrcParser);

#if 0
void SSTR_ManifestParser_print(SSTRParser_t *parser);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __SSTR_MANIFESTPARSER_H__ */

/*@}*/

