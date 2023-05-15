/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:       $Workfile:   abr_mpdparser.h  $
 * Version:         $Revision:   1.0  $
 * Original Author: Young Ki Kim(ykkim3@humaxdigital.com)
 * Current Author:  $Author:  $
 * Date:            $Date:  
 * File Description:	     MPD(ABR Manifest) Parser Header File
 * Module:
 * Remarks:
 */

/*******************************************************************/
/* Copyright (c) 2011 HUMAX Co., Ltd.                              */
/* All rights reserved.                                            */
/*******************************************************************/

#include "htype.h"

//#ifdef CONFIG_3RD_XML
//#endif

#ifndef __ABR_MPDPARSER_H__
#define __ABR_MPDPARSER_H__

/*----------------------------------------------------------------------------------------------
 *	define
 *---------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------
 *	Config
 *---------------------------------------------------------------------------------------------*/
// #define ABR_MPDPARSER_CONFIG_UPPERCASE
// #define ABR_MPDPARSER_CONFIG_NOT_USE_VK 1
#define ABR_MPDPARSER_CONFIG_3RD_XML 1

/*----------------------------------------------------------------------------------------------
 *	Constant
 *---------------------------------------------------------------------------------------------*/
#define ABR_MPDPARSER_CONST_UTCFORMAT_SIZE (21)
#define ABR_MPDPARSER_CONST_ATTR_BUFSIZE (512)

typedef void mpdElement_t;
typedef mpdElement_t mpdElement_Root_t;
typedef mpdElement_t mpdElement_ProgInfo_t;
typedef mpdElement_t mpdElement_Period_t;
typedef mpdElement_t mpdElement_SegInfoDef_t;
typedef mpdElement_t mpdElement_Represent_t;
typedef mpdElement_t mpdElement_Protection_t;
typedef mpdElement_t mpdElement_TrickMode_t;
typedef mpdElement_t mpdElement_UrlTemplete_t;
typedef mpdElement_t mpdElement_Segment_t;

typedef enum
{
	MPD_ERR_OK,
	MPD_ERR_FAIL_BUFFER_SIZE,		
	MPD_ERR_FAIL_ATTR_NOTFOUND,		
	MPD_ERR_FAIL_ELEMENT_NOTFOUND,		
	MPD_ERR_FAIL_MEMORY_ALLOC,		
	MPD_ERR_FAIL_MPD_NOTFOUND,
	MPD_ERR_FAIL_PERIOD_NOTFOUND,		
	MPD_ERR_FAIL_REPRESENT_NOTFOUND,
	MPD_ERR_FAIL_SEGMENT_NOTFOUND,
	MPD_ERR_FAIL
} mpdErr_e;
// #define 

typedef enum
{
	MPD_TYPE_ONDEMAND,	
	MPD_TYPE_LIVE		
} mpdType_e;

typedef enum
{
	MPD_SEGMENT_TYPE_URL,
	MPD_SEGMENT_TYPE_TEMPLATE		
} mpdSegType_e;

typedef struct _mpd_segmentUrl_t {
	char *pSrcUrl;
	unsigned int ulRange;
} mpdSegUrl_t;

typedef struct _mpd_urlTemplate_t {
	char *pSrcUrl;
	int endIndex;
} mpdUrlTemplate_t;

typedef struct _mpd_segmentInfo_t {
	unsigned int ulDuration;
	int startIndex;
	char *pBaseUrl;
	int bExistInitSegUrl;
	mpdSegUrl_t initSegUrl;
	mpdSegType_e segType;
	mpdUrlTemplate_t urlTemplate;
	mpdElement_Segment_t *firstSegment;
	int nTotalSegment;
} mpdSegInfo_t;

typedef struct _mpd_represent_t {
	unsigned long long ullBandwidth;
	int nId;
	/* omit other attributes */
	int nQualityRanking;
	int nTotalProtection;
	mpdElement_Protection_t *firstProtection;
	mpdElement_TrickMode_t *trickMode;
	mpdSegInfo_t segInfo;
} mpdRepresent_t;

typedef struct _mpd_segmentInfoDefault_t {
	unsigned int ulDuration;
	int startIndex;
	char *pBaseUrl;
	char *pSrcUrlTemplate;
} mpdSegInfoDef_t;

typedef struct _mpd_period_t {
	unsigned int ulStart;
	int nId;
	int bSegAlignFlag;
	int bBitstreamSwitchFlag;
	int bExistSegInfoDef;
	mpdSegInfoDef_t segInfoDef;
	int nTotalRepresent;
	mpdRepresent_t *representSet;
	mpdElement_Represent_t *firstRepresent;
} mpdPeriod_t;

typedef struct _mpd_data_t {
	mpdType_e etype;
	unsigned int ulDuration;
	unsigned int ulMinBufferTime;
	/* omit other attributes */
	// char szAvailStartTime[MPD_UTCFORMAT_SIZE];
	// char szAvailEndTime[MPD_UTCFORMAT_SIZE];
	char *pBaseUrl;
	mpdElement_ProgInfo_t *progInfo;
	int nTotalPeriod;
	mpdPeriod_t *periodSet;
	mpdElement_Period_t *firstPeriod;
} mpdData_t;

typedef struct _mpdparser_t {
	void		*document;
	void		*root;
	void		*mpdData;
} mpdParser_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Open the MPD parser
 *
 * param[in] szPath the pathname placed the MPD file
 *
 * @return if the MPD parser opening succeeds, the handle for MPD parser is returned.
 * On error NULL is returned.
 */
mpdParser_t *ABR_MPDParser_FileOpen(const char *szPath);

/**
 * Open the MPD parser
 *
 * param[in] pbuffer buffered MPD in memory
 *
 * @return if the MPD parser opening succeeds, the handle for MPD parser is returned.
 * On error NULL is returned.
 */
mpdParser_t *ABR_MPDParser_MemOpen(const char *pBuffer, int nBufsize);

/**
 * Close the MPD parser.
 *
 * param[in] parser the handle for MPD parser
 */
void ABR_MPDParser_Close(mpdParser_t *parser);

/**
 * Parse basic information for MPD.
 *
 * param[in] parser the handle for MPD parser
 */
mpdErr_e ABR_MPDParser_Parse(mpdParser_t *parser);

#if 0
void ABR_MPDParser_print(mpdParser_t *parser);
#endif

mpdElement_Segment_t* ABR_MPDParser_NextSegment(mpdElement_Segment_t* curSegment);
mpdElement_Segment_t* ABR_MPDParser_PrevSegment(mpdElement_Segment_t* curSegment);
mpdElement_Segment_t* ABR_MPDParser_n_Segment(mpdElement_Segment_t* segment, int n);
mpdErr_e ABR_MPDParser_GetSegmentUrl(mpdElement_Segment_t* curSegment, char *szBuf, int nBufsize);

// void ABR_MPDParser_mpdBox_parse(mpdparser_t *parser);
// void ABR_MPDParser_periodBox_parse(mpdparser_t *parser);

/*
int ABR_MPDParser_get_represent_count(mpdparser_t *parser);

int ABR_MPDParser_get_segment_count(mpdparser_t *parser);

mpdSegment_t ABR_MPDParser_get_first_segment(mpdparser_t *parser,int periodId,int representId);
mpdSegment_t ABR_MPDParser_get_next_segment(mpdSegment_t *segment);
mpdSegment_t ABR_MPDParser_get_prev_segment(mpdSegment_t *segment);
mpdSegment_t ABR_MPDParser_get_n_segment(mpdparser_t *parser, int n);
void ABR_MPDParser_get_url_bySegment(mpdSegment_t *segment, char *szBuf, int nBufsize);

mpdRepresentation_t ABR_MPDParser_get_first_represent(mpdparser_t *parser);
mpdRepresentation_t ABR_MPDParser_get_next_represent(mpdRepresentation_t *represent);
mpdRepresentation_t ABR_MPDParser_get_prev_represent(mpdRepresentation_t *represent);*/

// void ABR_MPDParser_get_first_represent(mpdparser_t *parser);	

#ifdef __cplusplus
}
#endif

#endif

