/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   dash_mpdparser.h  $
 * Version:			$Revision:   $
 * Original Author:	
 * Current Author:	$Author: parkjh4@humaxdigital.com $
 * Date:			$Date: 2012.07.30
 * File Description:	     dash mpd parser module header
 * Module:
 * Remarks:	
 */	

/**
 * @ingroup DASH_MPDPARSER
 */

/**
 * @author 
 * @date 30 Jul 2012
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
 * @file dash_mpdparser.h
 */


#ifndef __DASH_MPDPARSER_H__
#define __DASH_MPDPARSER_H__

#ifdef  __cplusplus
extern "C" {
#endif

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
/* Start Including Header Files */
#include "htype.h"
#include "util_dllist.h"
#include <sys/time.h> 	 /** for checking delay */

/* End Including Headers*/

/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/
/* Start Extern variablee */

/* End Extern variable */

/*******************************************************************/
/************************ Constant Definition *************************/
/*******************************************************************/
#define DASH_MAX_URL_LEN (1024)
#define DASH_MPDPARSER_CONST_ATTR_BUFSIZE (512)
#define DASH_WALLCLK_NOT_EXIST (-1LL)
static const int DASH_MPDPARSER_CONST_UTCFORMAT_SIZE = 21;
static const int DASH_MPDPARSER_CONST_PROTECTION_BUFSIZE = 65535;
static const int DASH_MPDPARSER_CONST_SYSTEMID_SIZE = 36;
static const int DASH_MINIMUM_TASK_SLEEP = 100;
static const HINT32 DASH_INT32_MAX = 0x7FFFFFFF;
static const HINT32 DASH_INT32_MIN = 0x0;
static const HINT64 DASH_INT64_MAX = 0x7FFFFFFFFFFFFFFFLL;
static const HINT64 DASH_INT64_MIN = 0x0LL;
/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */

typedef HINT32 DASH_BANDWIDTH_t;
typedef HINT64 DASH_WALLCLOCK_t;
typedef HINT64 DASH_TIME_t;

/** 
 * state of MPD
 */
typedef enum
{
	DASH_MPD_STATE_NOTREADY,	/**< not ready state */
	DASH_MPD_STATE_READY,		/**< ready state */
	DASH_MPD_STATE_UPDATING,	/**< update state */
	DASH_MPD_STATE_ERROR		/**< error state */
} DASH_MPD_STATE_e;

/** 
 * Segment type definition
 */
typedef enum
{
	DASH_SEGMENT_NONE,			/**< unknown segment */
	DASH_SEGMENT_INITIALISATION,/**< initialisation segment */
	DASH_SEGMENT_INDEX,			/**< index segment */
	DASH_SEGMENT_MEDIA,			/**< media segment */
} DASH_SEGMENT_e;

/** 
 * MPD Error code
 */
typedef enum
{
	DASH_MPD_ERR_OK,			/**< OK */
	DASH_MPD_ERR_ERROR,			/**< Error */
	DASH_MPD_ERR_PARSE_ERROR,	/**< Error : while parse MPD */
	DASH_MPD_ERR_MERGE_NONE,	/**< There are nothing to be merged */
	DASH_MPD_ERR_MERGE_ERROR,	/**< Error : while merge MPD */
	DASH_MPD_ERR_INVALID_MPD,	/**< Error : Invalid MPD */
	DASH_MPD_ERR_ATTR_NOTFOUND,	/**< Error : attribute does not exist */
	DASH_MPD_ERR_ERROR_BUFFERSIZE,	/**< Error : Buffer size is not enough */
	DASH_MPD_ERR_REPRESENTATION_NOTFOUND,	/**< Error : representation does not exist */
	DASH_MPD_ERR_ADAPTATION_NOTFOUND,		/**< Error : adaptation set does not exist */
	DASH_MPD_ERR_PERIOD_NOTFOUND,			/**< Error : period does not exist */
	DASH_MPD_ERR_EOF						/**< End of file */
} DASH_MPD_ERROR_e;

/** 
 * MPD Type
 */
typedef enum
{
	DASH_MPD_TYPE_STATIC,		/**< OnDemane - not updated MPD */
	DASH_MPD_TYPE_DYNAMIC		/**< Live - will be updated in certain time */
} DASH_MPD_TYPE_e;

/** 
 * MPD Period Type (currently not used)
 */
typedef enum
{
	DASH_PERIOD_TYPE_REGULAR,
	DASH_PERIOD_TYPE_EARLY_ACCESS
} DASH_PERIOD_TYPE_e;

/** 
 * Adaptationset(track) type
 */
typedef enum
{
	DASH_ADAPTATIONSET_TYPE_NONE, 		/**< unknown */
	DASH_ADAPTATIONSET_TYPE_VIDEO_ONLY,	/**< video only */
	DASH_ADAPTATIONSET_TYPE_AUDIO_ONLY, /**< audio only */
	DASH_ADAPTATIONSET_TYPE_AV,			/**< audio + video */
	DASH_ADAPTATIONSET_TYPE_TEXT,		/**< text */
	NR_DASH_ADAPTATIONSET_TYPE			
} DASH_ADAPTATIONSET_TYPE_e;

/** 
 * Locale type
 */
typedef enum
{
	DASH_LOCALE_TYPE_NONE,	/**< unknown */
	DASH_LOCALE_TYPE_EN,	/**< english */
	DASH_LOCALE_TYPE_KR,	/**< korean	*/
	DASH_LOCALE_TYPE_JP,	/**< japanese */
	DASH_LOCALE_TYPE_FR,	/**< french */
	/* DASH_LOCALE_TYPE_MORE, */
} DASH_LOCALE_TYPE_e;

/** 
 * PAR type
 */
typedef enum
{
	DASH_PAR_16_9,	/**< 16:9 */
	DASH_PAR_4_3	/**< 4:3 */
	//...
} DASH_PAR_e;

/** 
 * SAR type
 */
typedef enum
{
	DASH_SAR_16_9,	/**< 16:9 */
	DASH_SAR_4_3	/**< 4:3 */
	//...
} DASH_SAR_e;

/** 
 * Xlink show type (currently not used)
 */
typedef enum
{
	DASH_XLINK_SHOW_EMBED
	//...
} DASH_XLINK_SHOW_e;

/** 
 * Xlink type (currently not used)
 */
typedef enum
{
	DASH_XLINK_TYPE_SIMPLE
} DASH_XLINK_TYPE_e;

/** 
 * Xlink actuate
 */
typedef enum
{
	DASH_XLINK_ACTUATE_ONREQUEST,	/**< xlink will be download on request */
	DASH_XLINK_ACTUATE_ONLOAD,		/**< xlink will be download on load */
} DASH_XLINK_ACTUATE_e;

/** 
 * segment list method in MPD
 */
typedef enum
{
	DASH_LIST_DEFINE_BY_LIST,		/**< list is enumerated by list */
	DASH_LIST_DEFINE_BY_TEMPLATE	/**< list is derived by template */
} DASH_SEGMENTLIST_METHOD_e;

/** 
 * video scan type
 */
typedef enum
{
	DASH_VIDEOSCAN_PROGRESSIVE,		/**< progressive */
	DASH_VIDEOSCAN_INTERACE,		/**< interace */
	DASH_VIDEOSCAN_UNKNOWN			/**< unknown */
} DASH_VIDEOSCAN_e;

/** 
 * DASH codec definition
 */
typedef enum
{
	DASH_CODEC_NONE 		= 0x00000000,
	DASH_AUDIO_CODEC_AC3 	= 0x00000001,
	DASH_AUDIO_CODEC_ALAC 	= 0x00000002,
	DASH_AUDIO_CODEC_DRA1 	= 0x00000004,
	DASH_AUDIO_CODEC_DTSC 	= 0x00000008,
	DASH_AUDIO_CODEC_DTSE 	= 0x00000010,
	DASH_AUDIO_CODEC_DTSH 	= 0x00000020,
	DASH_AUDIO_CODEC_DTSL 	= 0x00000040,
	DASH_AUDIO_CODEC_EC3 	= 0x00000080,
	DASH_AUDIO_CODEC_ENCA 	= 0x00000100,
	DASH_AUDIO_CODEC_G719 	= 0x00000200,
	DASH_AUDIO_CODEC_G726 	= 0x00000400,
	DASH_AUDIO_CODEC_M4AE 	= 0x00000800,
	DASH_AUDIO_CODEC_MLPA 	= 0x00001000,
	DASH_AUDIO_CODEC_AAC 	= 0x00002000,
	DASH_AUDIO_CODEC_MP4A 	= 0x00004000,
	DASH_AUDIO_CODEC_UNCOMP = 0x00008000,
	DASH_AUDIO_CODEC_SAMR 	= 0x00010000,
	DASH_AUDIO_CODEC_SAWB 	= 0x00020000,
	DASH_AUDIO_CODEC_SAWP 	= 0x00040000,
	DASH_AUDIO_CODEC_SEVC 	= 0x00080000,
	DASH_AUDIO_CODEC_SQCP 	= 0x00100000,
	DASH_AUDIO_CODEC_SSMV 	= 0x00200000,
	DASH_AUDIO_CODEC_UNCOMP_16bAudio = 0x00400000,
	DASH_AUDIO_CODECS		= 0x004FFFFF,		/**< Audio codec filter */
	DASH_VIDEO_CODEC_MP4V	= 0x00800000,
	DASH_VIDEO_CODEC_AVC1 	= 0x01000000,
	DASH_VIDEO_CODEC_AVC2 	= 0x02000000,
	DASH_VIDEO_CODEC_AVCP 	= 0x04000000,
	DASH_VIDEO_CODEC_DRAC	= 0x08000000,
	DASH_VIDEO_CODEC_ENCV	= 0x10000000,
	DASH_VIDEO_CODEC_S263	= 0x20000000,
	DASH_VIDEO_CODEC_VC1	= 0x40000000,
	//DASH_VIDEO_CODEC_MJP2,
	//DASH_VIDEO_CODEC_MVC1,
	//DASH_VIDEO_CODEC_MVC2,
	//DASH_VIDEO_CODEC_SVC,
	DASH_VIDEO_CODECS		= 0x4F800000,		/**< Video codec filter */
	//DASH_TEXT_CODEC_ENCT	= 0x80000000,
	//DASH_TEXT_CODECS		= 0x80000000,
}DASH_CODEC_e;

/** 
 * Segment format
 */
typedef enum
{
	DASH_SEGMENT_UNKNOWN,	/**< unknown format */
	DASH_SEGMENT_TS,		/**< mpeg2-ts */
	DASH_SEGMENT_MP4,		/**< mpeg4 */
} DASH_SEGMENT_FORMAT_e;

/* List structure type definition */
typedef UTIL_DLL_t DASH_SegmentURLSet_t;
typedef UTIL_DLL_t DASH_StringSet_t;
typedef UTIL_DLL_t DASH_DescriptorSet_t;
typedef UTIL_DLL_t DASH_MetricsSet_t;
typedef UTIL_DLL_t DASH_RangeSet_t;
typedef UTIL_DLL_t DASH_ContentComponentSet_t;
typedef UTIL_DLL_t DASH_BaseURLSet_t;
typedef UTIL_DLL_t DASH_SubrepresentationSet_t;
typedef UTIL_DLL_t DASH_HUINT32Set_t;
typedef UTIL_DLL_t DASH_RepresentationSet_t;
typedef UTIL_DLL_t DASH_AdaptationSetSet_t;
typedef UTIL_DLL_t DASH_SubsetSet_t;
typedef UTIL_DLL_t DASH_ProgramInformationSet_t;
typedef UTIL_DLL_t DASH_LocationSet_t;
typedef UTIL_DLL_t DASH_PeriodSet_t;
typedef UTIL_DLL_t DASH_SegmentTimelineSet_t;

/** 
 * DASH string
 */
typedef struct DASH_STRING_s
{
	HINT8* 					str;	/* string */
	HUINT32					length;	/* length */
} DASH_STRING_t;

/** 
 * DASH xlink
 */
typedef struct DASH_XLINK_s
{
	DASH_STRING_t			href;		/* url */
	DASH_XLINK_ACTUATE_e	eActuate;	/* actuate */
	DASH_XLINK_TYPE_e		type;		/* type */
	DASH_XLINK_SHOW_e		show;		/* show */
} DASH_XLINK_t;

/** 
 * DASH byte-range
 */
typedef struct DASH_BYTERANGE_s
{
	HUINT32					start;		/* start offset */
	HUINT32					end;		/* end offset */
} DASH_BYTERANGE_t;

/** 
 * DASH url
 */
typedef struct DASH_URLTYPE_s
{
	DASH_STRING_t			sourceURL;	/* url */
	DASH_BYTERANGE_t		byteRange;	/* byte-range */
} DASH_URLTYPE_t;

/** 
 * DASH descriptor
 */
typedef struct DASH_DESCRIPTOR_s
{	
	DASH_STRING_t			schemeIdUri;	/* scheme ID Uri */
	DASH_STRING_t			value;			/* value */
} DASH_DESCRIPTOR_t;

/** 
 * DASH range
 */
typedef struct DASH_RANGE_s
{
	DASH_WALLCLOCK_t		starttime;		/* start time */
	DASH_WALLCLOCK_t		endtime;		/* end time */
} DASH_RANGE_t;

/** 
 * DASH program information
 */
typedef struct DASH_ProgramInformation_s
{
	/* attributes */
	DASH_STRING_t lang;					/**< language */
	DASH_STRING_t moreInformationURL;	/**< more information URL */

	/* element */
	DASH_STRING_t title;				/**< title */
	DASH_STRING_t source;				/**< source */
	DASH_STRING_t copyright;			/**< copyright */
} DASH_ProgramInformation_t;

/** 
 * DASH base url
 */
typedef struct DASH_BaseURL_s
{
	/* attributes */
	DASH_STRING_t 		serviceLocation;	/**< service Location */
	DASH_BYTERANGE_t	byteRange;			/**< byte range */
} DASH_BaseURL_t;

/** 
 * DASH Location
 */
typedef struct DASH_Location_s
{
	/* attributes */
	DASH_STRING_t uri;						/**< url */
} DASH_Location_t;

/** 
 * DASH segment base
 */
typedef struct DASH_SegmentBase_s
{
	/* attributes */
	DASH_TIME_t 		timescale;
	DASH_TIME_t		 	presentationTimeOffset;
	DASH_BYTERANGE_t 	indexRange;
	HBOOL 				indexRangeExact;
	/* element */
	DASH_URLTYPE_t		Initialisation;
	DASH_URLTYPE_t		RepresentationIndex;
} DASH_SegmentBase_t;

/** 
 * DASH segment timeline
 */
typedef struct DASH_SegmentTimeline_s
{
	DASH_TIME_t	t;	/**< time */
	DASH_TIME_t	d;	/**< duration */
	HUINT32		r;	/**< repeat */
} DASH_SegmentTimeline_t;

/** 
 * DASH multiple segment base
 */
typedef struct DASH_MultipleSegmentBase_s
{
	/* attributes */
	HUINT32 duration;
	HUINT32 startNumber;

	/* elements */
	DASH_SegmentBase_t 		SegmentBaseInformation;
	DASH_SegmentTimelineSet_t	SegmentTimeline;
	DASH_URLTYPE_t			BitstreamSwitching;

	/* operation */
	HBOOL isSegmentTimeline;
} DASH_MultipleSegmentBase_t;

/** 
 * DASH segment list
 */
typedef struct DASH_SegmentList_s
{
	DASH_XLINK_t 	xlink;
	DASH_MultipleSegmentBase_t MultipleSegmentBaseInformation;
	DASH_SegmentURLSet_t SegmentUrl;
	DASH_URLTYPE_t	media;			/**< not used, this is filled via SegmentUrl.byteRange */
	DASH_BYTERANGE_t	mediaRange;	/**< not used, this is filled via SegmentUrl.byteRange */
	DASH_URLTYPE_t	index;
	DASH_BYTERANGE_t	indexRange;

	/* operation */
	HBOOL isXlink;					/**< xlink existence */
} DASH_SegmentList_t;

/** 
 * DASH segment template
 */
typedef struct DASH_SegmentTemplate_s
{
	DASH_MultipleSegmentBase_t MultipleSegmentBaseInformation;
	DASH_STRING_t	media;
	DASH_STRING_t	index;
	DASH_STRING_t	initialisation;
	DASH_STRING_t	bitstreamSwitching;
} DASH_SegmentTemplate_t;

/** 
 * DASH subset
 */
typedef struct DASH_Subset_s
{
	DASH_StringSet_t contains;
} DASH_Subset_t;

/** 
 * DASH content component
 */
typedef struct DASH_ContentComponent_s
{
	/* Attributes */
	DASH_STRING_t	id;
	DASH_STRING_t	lang;
	DASH_STRING_t	contentType;
	DASH_PAR_e		par;

	/* Elements */
	DASH_DescriptorSet_t Accessibility;
	DASH_DescriptorSet_t Role;
	DASH_DescriptorSet_t Rating;
	DASH_DescriptorSet_t ViewPoint;
} DASH_ContentComponent_t;

/** 
 * DASH common attribute elements
 */
typedef struct DASH_CommonAttributesElements_s
{
	/* Attributes */
	DASH_STRING_t	profile;
	HUINT32			width;
	HUINT32			height;
	DASH_SAR_e		sar;
	DASH_STRING_t	frameRate;
	DASH_STRING_t	audioSamplingRate;
	DASH_STRING_t	mimeType;
	DASH_STRING_t	segmentProfiles;
	DASH_STRING_t	codecs;
	double			maximumSAPPeriod;
	HUINT32			startWithSAP;
	double			maxPlayRate;
	HBOOL			codingDependency;
	DASH_VIDEOSCAN_e	scantype;

	/* Elements */
	DASH_DescriptorSet_t FramePacking;
	DASH_DescriptorSet_t AudioChannelConfiguration;
	DASH_DescriptorSet_t ContentProtection;

	/* Operatable */
	DASH_CODEC_e	eCodec;
} DASH_CommonAttributesElements_t;

/** 
 * DASH metrics
 */
typedef struct DASH_Metrics_s
{
	/* Attributes */
	DASH_MetricsSet_t	metrics;

	/* Elements */
	DASH_RangeSet_t 	Range;
	DASH_DescriptorSet_t Reporting;
} DASH_Metrics_t;

/** 
 * DASH sub-representation
 */
typedef struct DASH_Subrepresentation_s
{
	/* Attributes */
	HUINT32	level;
	DASH_StringSet_t dependencyLevel;
	DASH_BANDWIDTH_t bandwidth;
	DASH_StringSet_t contentComponent;

	/* Elements */
	HBOOL isSelected;
	DASH_CommonAttributesElements_t CommonAttributesElements;
} DASH_Subrepresentation_t;

/** 
 * DASH representation
 */
typedef struct DASH_Representation_s
{
	/* Attributes */
	DASH_STRING_t	id;
	DASH_BANDWIDTH_t bandwidth;
	HUINT32	qualityRanking;
	DASH_StringSet_t dependencyId;
	DASH_StringSet_t mediaStreamStructureId;

	/* Elements */ 
	DASH_CommonAttributesElements_t CommonAttributesElements;
	DASH_BaseURLSet_t BaseURL;
	DASH_SubrepresentationSet_t SubRepresentation;
	DASH_SegmentBase_t SegmentBase;
	DASH_SegmentList_t SegmentList;
	DASH_SegmentTemplate_t SegmentTemplate;

	/* Operation */
	HBOOL isBaseURL;
	HBOOL isSegmentList;
	HBOOL isSegmentTemplate;
	HBOOL isSegmentBase;
	HBOOL isSelected;
	DASH_SEGMENTLIST_METHOD_e listMethod;
} DASH_Representation_t;

/** 
 * DASH adaptation set
 */
typedef struct DASH_AdaptationSet_s
{
	/* Attributes */
	DASH_XLINK_t xlink;
	DASH_STRING_t id;
	DASH_STRING_t group;
	DASH_STRING_t lang;
	DASH_STRING_t contentType;
	DASH_PAR_e par;
	DASH_BANDWIDTH_t minBandwidth;
	DASH_BANDWIDTH_t maxBandwidth;
	HUINT32	minWidth;
	HUINT32 maxWidth;
	HUINT32 minHeight;
	HUINT32 maxHeight;
	DASH_STRING_t minFrameRate;
	DASH_STRING_t maxFrameRate;
	HBOOL segmentAlignment;
	HBOOL subsegmentAlignment;
	HUINT32 subsegmentStartsWithSAP;
	HBOOL bitstreamSwitching;
	DASH_CommonAttributesElements_t CommonAttributesElements;

	/* Elements */
	DASH_DescriptorSet_t Accessibility;
	DASH_DescriptorSet_t Role;
	DASH_DescriptorSet_t Rating;
	DASH_DescriptorSet_t ViewPoint;
	DASH_ContentComponentSet_t ContentComponent;
	DASH_BaseURLSet_t BaseURL;		
	DASH_SegmentBase_t	SegmentBase;
	DASH_SegmentList_t	SegmentList;
	DASH_SegmentTemplate_t SegmentTemplate;
	DASH_RepresentationSet_t Representation;

	/* Operation */
	HBOOL isBaseURL;
	HBOOL isSegmentList;
	HBOOL isSegmentTemplate;
	HBOOL isSegmentBase;
	HBOOL isSelected;
	HBOOL isXlink;
	DASH_SEGMENTLIST_METHOD_e listMethod;
	DASH_ADAPTATIONSET_TYPE_e trackType;
	DASH_LOCALE_TYPE_e		  localeType;
} DASH_AdaptationSet_t;

/** 
 * DASH period
 */
typedef struct DASH_Period_s
{
	/* Attributes */
	DASH_XLINK_t xlink;
	DASH_STRING_t id;
	DASH_TIME_t start;
	DASH_TIME_t duration;
	HBOOL bitstreamSwitching;

	/* Elements */
	DASH_BaseURLSet_t BaseURL;
	DASH_SegmentBase_t SegmentBase;
	DASH_SegmentList_t SegmentList;
	DASH_SegmentTemplate_t SegmentTemplate;
	DASH_AdaptationSetSet_t AdaptationSet;
	DASH_SubsetSet_t Subset;

	DASH_PERIOD_TYPE_e	type;
	
	/* Operation */
	HBOOL isBaseURL;
	HBOOL isSegmentList;
	HBOOL isSegmentTemplate;
	HBOOL isSegmentBase;
	HBOOL isXlink;
	DASH_SEGMENTLIST_METHOD_e listMethod;
} DASH_Period_t;

/** 
 * DASH MPD
 */
typedef struct DASH_MPD_s
{
	/* Attributes */
	DASH_STRING_t	id;
	DASH_STRING_t	profiles;
	DASH_MPD_TYPE_e		type;
	DASH_WALLCLOCK_t	availibilityStartTime;
	DASH_WALLCLOCK_t	availibilityEndTime;
	DASH_TIME_t	mediaPresentationDuration;
	DASH_TIME_t	minimumUpdatePeriod;
	DASH_TIME_t	minBufferTime;
	DASH_TIME_t	timeShiftBufferDepth;
	DASH_TIME_t	suggestedPresentationDelay;
	DASH_TIME_t	maxSegmentDuration;
	DASH_TIME_t	maxSubsegmentDuration;

	/* Elements */
	DASH_ProgramInformationSet_t ProgramInformation;
	DASH_BaseURLSet_t BaseURL;
	DASH_LocationSet_t Location;
	DASH_PeriodSet_t Period;
	DASH_MetricsSet_t Metrics;

	/* Operation */
	HBOOL isBaseURL;
	DASH_TIME_t timeScale;
} DASH_MPD_t;

/** 
 * DASH xlink list
 */
typedef struct DASH_XLINK_LIST_s
{
	UTIL_DLL_t periods;
	UTIL_DLL_t adaptations;
	UTIL_DLL_t segmentlist;
} DASH_XLINK_LIST_t;

/** 
 * DASH segment information
 */
typedef struct DASH_SEGMENTINFO_s
{
	HUINT8		pucSegmentURL[DASH_MAX_URL_LEN];
	HINT64 		llSegmentStarttime;
	HINT64 		llSegmentDuration;
	HINT64		llSegmentSeektime;	/* requested seek time if exists */
	HUINT32		szSegment;
	HBOOL		bIsLastSegment;
	DASH_SEGMENT_e 	eSegmentType;
	DASH_ADAPTATIONSET_TYPE_e	eTrackType;
	DASH_CODEC_e	eCodec;
	DASH_BANDWIDTH_t nBandwidth;
	/* byte range */
	HUINT32		startoffset;
	HUINT32		endoffset;
} DASH_SEGMENTINFO_t;

typedef HINT32 (*DASH_XLINK_DOWN_CB_t)(DASH_XLINK_LIST_t*);

/** 
 * DASH Parser - Top structure
 */
typedef struct DASH_MPDParser_s
{
	void *document;					/* document for libxml2 */
	void *root;						/* root for libxml2 */
	void *parsedMPD;				/* parsed MPD data */
	DASH_MPD_STATE_e state;			/* MPD State */
	DASH_XLINK_DOWN_CB_t XlinkDown; /* xlink download callback */
	HINT32 revision;				/* revision number */
	DASH_SEGMENT_FORMAT_e format;
	HULONG ulSema;
} DASH_MPDParser_t;
typedef DASH_MPDParser_t* DASH_MPDPARSER_HANDLE_t;

/*! 
 *	\brief Create MPD parser
 *	\return MPD parser handle
 *	\see 
 */
DASH_MPDPARSER_HANDLE_t DASH_MPD_Create(void);

/*! 
 *	\brief Destroy MPD parser
 *	\param mpd : MPD parser handle
 *	\return Error code
 *	\see 
 */
DASH_MPD_ERROR_e DASH_MPD_Destroy(DASH_MPDPARSER_HANDLE_t mpd);

/*! 
 *	\brief Register function to download xlink
 *	\param mpd : MPD parser handle
 *	\param cb : xlink download function 
 *	\return Error code
 *	\see 
 */
DASH_MPD_ERROR_e DASH_MPD_REGISTER_XLINKDOWN_CB(DASH_MPDPARSER_HANDLE_t mpd, DASH_XLINK_DOWN_CB_t cb);

/*! 
 *	\brief Start to parse XML
 *	\param mpd : MPD parser handle
 *	\param buf : buffer
 *	\param szBuf : size of buffer
 *	\return Error code
 *	\see 
 */
DASH_MPD_ERROR_e	DASH_MPD_ParseXML(DASH_MPDPARSER_HANDLE_t mpd, void* buf, HUINT32 szBuf);

/*! 
 *	\brief Start to parse MPD
 *	\param mpd : MPD parser handle
 *	\return Error code
 *	\see 
 */
DASH_MPD_ERROR_e	DASH_MPD_ParseMPD(DASH_MPDPARSER_HANDLE_t mpd);

/*! 
 *	\brief Merge between current MPD and new MPD
 *	\param current : current MPD parser handle
 *	\param newer : new MPD parser handle
 *	\return Error code
 *	\see 
 */
DASH_MPD_ERROR_e	DASH_MPD_MergeMPD(DASH_MPDPARSER_HANDLE_t current, DASH_MPDPARSER_HANDLE_t newer);

/*! 
 *	\brief Get MPD update period
 *	\param mpd : MPD parser handle
 *	\return update period value
 *	\see 
 */
HINT32 				DASH_MPD_GetMPDUpdatePeriod(DASH_MPDPARSER_HANDLE_t mpd);

/*! 
 *	\brief Get available start time of MPD
 *	\param mpd : MPD parser handle
 *	\return available start time in UTC
 *	\see 
 */
HINT64 				DASH_MPD_GetAvailableStartTime(DASH_MPDPARSER_HANDLE_t mpd);

/*! 
 *	\brief Get available end time of MPD
 *	\param mpd : MPD parser handle
 *	\return available end time in UTC
 *	\see 
 */
HINT64 				DASH_MPD_GetAvailableEndTime(DASH_MPDPARSER_HANDLE_t mpd);

/*! 
 *	\brief Get is MPD static or dynamic
 *	\param mpd : MPD parser handle
 *	\return true or not
 *	\see 
 */
HBOOL 				DASH_MPD_Get_IsLiveUpdate(DASH_MPDPARSER_HANDLE_t mpd);

/*! 
 *	\brief Get xlink list
 *	\param parser : MPD parser handle
 *	\param eActuate : filter actuate (onrequest , onload)
 *	\param list	: xlink list
 *	\return Error code
 *	\see 
 */
DASH_MPD_ERROR_e	DASH_MPD_GetXlink(DASH_MPDPARSER_HANDLE_t parser, DASH_XLINK_ACTUATE_e eActuate ,DASH_XLINK_LIST_t** list);

/*! 
 *	\brief Set MPD state
 *	\param mpd : MPD parser handle
 *	\param state : MPD state
 *	\return Error code
 *	\see 
 */
DASH_MPD_ERROR_e	DASH_MPD_SetState(DASH_MPDPARSER_HANDLE_t mpd, DASH_MPD_STATE_e state);

/*! 
 *	\brief Get MPD state
 *	\param mpd : MPD parser handle
 *	\return Error code
 *	\see 
 */
DASH_MPD_STATE_e	DASH_MPD_GetState(DASH_MPDPARSER_HANDLE_t mpd);

/*! 
 *	\brief Wait certain MPD state
 *	\param mpd : MPD parser handle
 *	\param state : MPD state
 *	\return Error code
 *	\see 
 */
DASH_MPD_ERROR_e 	DASH_MPD_WaitState(DASH_MPDPARSER_HANDLE_t mpd, DASH_MPD_STATE_e state);

/*! 
 *	\brief Parse period (for xlink download cb)
 *	\param element : xlink node
 *	\param per : period
 *	\return Error code
 *	\see 
 */
DASH_MPD_ERROR_e 	DASH_MPD_Parse_Period(void* element , DASH_Period_t* per);

/*! 
 *	\brief Parse adaptation (for xlink download cb)
 *	\param element : xlink node
 *	\param per : adaptation set
 *	\return Error code
 *	\see 
 */
DASH_MPD_ERROR_e 	DASH_MPD_Parse_AdaptationSet(void* element, DASH_AdaptationSet_t *adap);

/*! 
 *	\brief Parse segmentlist (for xlink download cb)
 *	\param element : xlink node
 *	\param segmentlist : segment list
 *	\return Error code
 *	\see 
 */
DASH_MPD_ERROR_e 	DASH_MPD_Parse_SegmentList(void* element, DASH_SegmentList_t *segmentlist);

/*! 
 *	\brief MPD post processing : analyze MPD (start of period, duration, types and so on)
 *	\param mpdparser : MPD parser handle
 *	\return Error code
 *	\see 
 */
DASH_MPD_ERROR_e	DASH_MPD_PostProcess(DASH_MPDPARSER_HANDLE_t mpdparser);

/*! 
 *	\brief Get period information based on given time
 *	\param mpd : MPD parser handle
 *	\param period : period
 *	\param time : time
 *	\return Error code
 *	\see 
 */
DASH_MPD_ERROR_e	DASH_MPD_GetPeriodInfo(DASH_MPDPARSER_HANDLE_t mpd, DASH_Period_t** period, DASH_TIME_t time);

/*! 
 *	\brief Get period information based on given time, track type and locale information
 *	\param mpd : MPD parser handle
 *	\param adaptationset : period
 *	\param type : adaptation set type(track type)
 *	\param locale : locale (if track is audio)
 *	\param time : time
 *	\return Error code
 *	\see 
 */
DASH_MPD_ERROR_e	DASH_MPD_GetAdaptationSetInfo(DASH_MPDPARSER_HANDLE_t mpd, DASH_AdaptationSet_t** adaptationset, DASH_ADAPTATIONSET_TYPE_e type, DASH_LOCALE_TYPE_e locale, DASH_TIME_t time);

/*! 
 *	\brief Get representation information based on given time, track type, locale information and bandwidth
 *	\param mpd : MPD parser handle
 *	\param adaptationset : period
 *	\param type : adaptation set type(track type)
 *	\param locale : locale (if track is audio)
 *	\param time : time
 *	\param bandwidth : bandwidth
 *	\return Error code
 *	\see 
 */
DASH_MPD_ERROR_e	DASH_MPD_GetRepresentationSetInfo(DASH_MPDPARSER_HANDLE_t mpd, DASH_Representation_t** adaptationset, DASH_ADAPTATIONSET_TYPE_e type, DASH_LOCALE_TYPE_e locale, DASH_TIME_t time, DASH_BANDWIDTH_t bandwidth);

/*! 
 *	\brief Get media duration of MPD
 *	\param mpd : MPD parser handle
 *	\return duration
 *	\see 
 */
HINT64 				DASH_MPD_GetMediaDuration(DASH_MPDPARSER_HANDLE_t mpd);

/*! 
 *	\brief Get the number of tracks (based on 1st period)
 *	\param mpd : MPD parser handle
 *	\return the number of tracks
 *	\see 
 */
HINT32 				DASH_MPD_GetTotalTracks(DASH_MPDPARSER_HANDLE_t mpd);

/*! 
 *	\brief Get track type
 *	\param mpd : MPD parser handle
 *	\param nTrack : track number
 *	\return track type
 *	\see 
 */
DASH_ADAPTATIONSET_TYPE_e DASH_MPD_GetTrackType(DASH_MPDPARSER_HANDLE_t mpd, HUINT32 nTrack);

/*! 
 *	\brief Get time scale
 *	\param mpd : MPD parser handle
 *	\return time scale value
 *	\see 
 */
DASH_TIME_t 		DASH_MPD_GetTimeScale(DASH_MPDPARSER_HANDLE_t mpd);

/*! 
 *	\brief Get codec of track
 *	\param mpd : MPD parser handle
 *	\param nTrack : track number
 *	\return codec value
 *	\see 
 */
DASH_CODEC_e 		DASH_MPD_GetTrackCodec(DASH_MPDPARSER_HANDLE_t mpd, HUINT32 nTrack);

/*! 
 *	\brief Get language of track
 *	\param mpd : MPD parser handle
 *	\param nTrack : track number
 *	\return language string
 *	\see 
 */
DASH_STRING_t 		DASH_MPD_GetTrackLang(DASH_MPDPARSER_HANDLE_t mpd, HUINT32 nTrack);

/*! 
 *	\brief Get next bandwidth based track type, locale information, and current bandwidth
 *	\param mpd : MPD parser handle
 *	\param type : adaptation set type(track type)
 *	\param locale : locale information
 *	\param bandwidth : current bandwidth
 *	\return bandwidth
 *	\see 
 */
DASH_BANDWIDTH_t 	DASH_MPD_GetNextBandwidth(DASH_MPDPARSER_HANDLE_t mpd, DASH_ADAPTATIONSET_TYPE_e type, DASH_LOCALE_TYPE_e locale, DASH_TIME_t time, DASH_BANDWIDTH_t bandwidth);

/*! 
 *	\brief Get previous bandwidth based track type, locale information, and current bandwidth
 *	\param mpd : MPD parser handle
 *	\param type : adaptation set type(track type)
 *	\param locale : locale information
 *	\param bandwidth : current bandwidth
 *	\return bandwidth
 *	\see 
 */
DASH_BANDWIDTH_t	DASH_MPD_GetPrevBandwidth(DASH_MPDPARSER_HANDLE_t mpd, DASH_ADAPTATIONSET_TYPE_e type, DASH_LOCALE_TYPE_e locale, DASH_TIME_t time, DASH_BANDWIDTH_t bandwidth);

/*! 
 *	\brief Get segment format of MPD
 *	\param mpd : MPD parser handle
 *	\return segment format value
 *	\see 
 */
DASH_SEGMENT_FORMAT_e	DASH_MPD_GetSegmentFormat(DASH_MPDPARSER_HANDLE_t mpd);


/* Debug */
void 				DASH_MPD_Print_Representation(DASH_AdaptationSet_t* Adaptation);
void 				DASH_MPD_Print_Adaptation(DASH_Period_t* period);
void 				DASH_MPD_Print_Period(DASH_MPD_t* mpd);
void 				DASH_MPD_Print_MPD(DASH_MPD_t* mpd);

#endif
