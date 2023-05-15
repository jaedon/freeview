/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  layer_module.h File Name을 적습니다.
	@brief	  file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__EPGMAIN_H__
#define	__EPGMAIN_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include "linkedlist.h"
#include <db_svc.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

#define EPG_LANG_ISO_639_LEN								3
#define EPG_EXT_ITEM_DESCRIPTION_LEN						12
#define EPG_EXT_ITEM_CHAR_LEN								52
#define EPG_GUIDANCE_TEXT_LEN								255


/*********************************/
/* FSAT ext spec.                */
/*********************************/
#define EPG_SHORT_EVENT_PROMO_LEN							255
#define EPG_MEDIA_LOCATOR_LEN								255
#define EPG_PREFIX_URI_LEN									255
#define EPG_ONLINE_AVAIL_INFO_MAX							1
#define EPG_SHORT_EVENT_PROMO_INFO_MAX					1
#define EPG_MEDIA_LOCATOR_INFO_MAX						10
//-------------------------------------------------------------
//	FIND
//-------------------------------------------------------------
#define EPG_FIND_KEYWORD_LEN								512

typedef enum tag_EPG_FIND_MASK
{
	EPG_FIND_MASK_PF				=	0x10000000,
	EPG_FIND_MASK_SCH				=	0x20000000,
	EPG_FIND_MASK_GENERICPF			=	0x40000000, /* Used in Freesat */

	EPG_FIND_MASK_TIME				=	0x00000001,
	EPG_FIND_MASK_GENRE				=	0x00000002,
	EPG_FIND_MASK_KEYWORD			=	0x00000004,
	EPG_FIND_MASK_EVENTID			=	0x00000008,

	EPG_FIND_MASK_CIT				=	0x00000010,	/* CONFIG_MW_EPG_CIT */
	EPG_FIND_MASK_CONTENTID			=	0x00000020, /* CONFIG_MW_EPG_CIT */
	EPG_FIND_MASK_CPT				=	0x00000040,	/* CONFIG_MW_EPG_CPT */

/****************************************************************************
	Spec 에 관련된 내용
****************************************************************************/
	/* CONFIG_OP_NORDIG_BOXER_NAVIGATOR */
	EPG_FIND_MASK_BOXER_DIRECTOR	=	0x00000100,
	EPG_FIND_MASK_BOXER_ACTOR		= 	0x00000200,
	EPG_FIND_MASK_BOXER_KEYWORD		=	0x00000400,
	EPG_FIND_MASK_BOXER_DATETIME	=	0x00000800,

	/* CONFIG_OP_ZIGGO_TVANYTIME_EPG, CONFIG_OP_PVR_BASED_ON_TVANYTIME */
	EPG_FIND_MASK_CONTENTKIND		= 	0x00001000,
	EPG_FIND_MASK_CRIDDATA			=	0x00002000,
	EPG_FIND_MASK_CRIDAUTH			=	0x00004000,
	EPG_FIND_MASK_CRIDIMI			=	0x00008000,

	/* CONFIG_MW_EPG_TVTV */
	EPG_FIND_MASK_PROGID			=	0x00010000,
	EPG_FIND_MASK_TIP				=	0x00020000,

	EPG_FIND_MASK_XXX1				=	0x00040000,
	EPG_FIND_MASK_XXX2				=	0x00080000,

	/* CONFIG_OP_NORDIG_BOXER_NAVIGATOR */
	EPG_FIND_MASK_BOXER_SERIES_ID	=	0x00100000,
	EPG_FIND_MASK_BOXER_STATUS		=	0x00200000,

	/****************************************************************************
		조합하는 Mask
	****************************************************************************/
	EPG_FIND_MASK_PF_SCH			= 	(EPG_FIND_MASK_PF | EPG_FIND_MASK_SCH),
	EPG_FIND_MASK_PF_SCH_TIME		= 	(EPG_FIND_MASK_PF | EPG_FIND_MASK_SCH | EPG_FIND_MASK_TIME),
	EPG_FIND_MASK_PF_SCH_KEYWORD	= 	(EPG_FIND_MASK_PF | EPG_FIND_MASK_SCH | EPG_FIND_MASK_KEYWORD),

 	EPG_FIND_MASK_ALL				=	0xFFFFFFFF
} SvcEpg_FindMask_e;

#if defined(CONFIG_MW_EPG_TVTV)
//-------------------------------------------------------------
//	TVTV
//-------------------------------------------------------------
#define EPG_TVTV_SERIES_NUMBER_LEN							12
#define EPG_TVTV_FILENAME_LEN								16
#define EPG_TVTV_CATEGORY_LEN								5
// TVTV DIGAL BASIC EPG CODE OF PRACTICE V 1.0.1
//#define EPG_TVTV_LINK_MAX									4
#define EPG_TVTV_COUNTRY_LEN								15
#define EPG_TVTV_YEAR_LEN									9

#define SAT_TVTV_SVC_ID										0x1C50
#define SAT_TVTV_TS_ID										0x0457
#define SAT_TVTV_ON_ID										0x0001

#define TVTVEPG_FILE_READ_UNIT					4096
#define TVTV_EVENTFILE_NUM_MAX					300

#define TVTV_FILENAME_TABLEEXTENSION			".tbl"
#define TVTV_FILENAME_DSI_TABLE					"tvtv.dsi"
#endif

#define INT_EPGMACRO_GET_TABLEID_N_RESULT(a, b, c)			\
{																\
	c = a & 0x0000ffff; 											\
	b = (a & 0xffff0000)>>16;										\
}

#define INT_EPGMACRO_GET_FILTERID_N_TSUNIQID(a, b, c)			\
{																\
	c = a & 0x0000ffff; 											\
	b = (a & 0xffff0000)>>16;										\
}

#define	PRINT_EPG_TIME													\
{																			\
	HxLOG_Print("\n[EPG] System Time (%d)\n", VK_TIMER_GetSystemTick()%100000);	\
}

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef	enum
{
	ERR_EPG_OK 					= 0,
	ERR_EPG_FAIL,
	ERR_EPG_NOT_SUPPORTED
}SvcEpg_Error_e;


typedef enum
{
	/* - Description : eit gathering done alaram 메시지
	   - Parameters Usage :
		   handle : Action handle
		   p1 : table id
		   p2 : TBD
		   p3 : TBD */
	eSEVT_EPG_EIT_DONE = eSEVT_EPG_START,


	/* - Description : tvtv download 진행률 전달 메시지.
	   - Parameters Usage :
		   handle : Action handle
		   p1 : percent
		   p2 : TBD
		   p3 : TBD */
	eSEVT_EPG_TVTV_PROGRESS,


	/* - Description : tvtv download done alarm message
	   - Parameters Usage :
		   handle : Action handle
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_EPG_TVTV_DONE,

	/* - Description : tvtv action(downloading & data loading) stop alarm message
	   - Parameters Usage :
		   handle : Action handle
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_EPG_TVTV_STOP_ACTION,

	/* - Description : Update Freeview series reservation(EIT Table done alarm msg)
	   - Parameters Usage :
		   handle : TBD
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_EPG_FPB_UPDATE_SCHEDULE_SERIES_SET,

	eSEVT_EPG_MAX
} SvcEpg_Event_e;

typedef enum enEpgDataType
{
	EPG_DATA_TYPE_EIT			= 0x01,
	EPG_DATA_TYPE_CIT,
	EPG_DATA_TYPE_CPT,
	EPG_DATA_TYPE_TVTV,

	EPG_DATA_TYPE_MAX
} SvcEpg_DataType_e;

typedef struct	tagEpgExtText
{
	HUINT8							aucTextChar[DxEPG_EXT_TEXT_LEN + 1];
	struct tagEpgExtText			*pNext;
} SvcEpg_ExtText_t;

typedef struct	tagEpgParentalRating
{
	HUINT8							aucIsoCountryCode[EPG_LANG_ISO_639_LEN + 1];
	HUINT8							ucParentalRating;
} SvcEpg_ParentalRating_t;

typedef struct	tagEpgDExtText
{
	HUINT8						aucTextChar[EPG_LANG_ISO_639_LEN + 800];
	struct tagEpgExtDText			*pNext;
} SvcEpg_ExtDText_t;

typedef struct	tagEpgShortText
{
	HUINT8							aucIsoCountryCode[EPG_LANG_ISO_639_LEN + 1];
	SvcEpg_ExtText_t					*pEventName;
	SvcEpg_ExtText_t					*pEventText;
	SvcEpg_ExtDText_t					*pEventDText;
} SvcEpg_ShortText_t;

typedef struct	tagEpgComponentDesc
{
	HUINT8							ucStreamContent;
	HUINT8							ucComponentType;
	HUINT8							aucIsoCountryCode[EPG_LANG_ISO_639_LEN + 1];
} SvcEpg_ComponentDesc_t;

// 64 bytes
typedef	struct	tagEpgExtendedEventItem
{
#if	defined(CONFIG_MW_EPG_TVTV)
	HUINT8							ucType;
#endif
	HINT16							ulType;
	SvcEpg_ExtText_t				*pItemDescription;
	SvcEpg_ExtText_t				*pItemChar;
} SvcEpg_ExtendedInfo_t;


typedef struct	tagEventId
{
	HINT32	nEventId;
	HUINT32							ulContentId;
#if	defined(CONFIG_MW_EPG_TVTV)
	HUINT16							usTvtvId;
	HUINT32							ulProgId;
#endif
} SvcEpg_EventId_t;

typedef	struct	tagExtendedInfo
{
	HUINT8							aucIsoCountryCode[EPG_LANG_ISO_639_LEN + 1];
	SvcEpg_ExtendedInfo_t				astItem[DxEPG_EXT_ITEM_MAX];
	SvcEpg_ExtText_t					*pstExtText;
} SvcEpg_ExtInfo_t;

typedef	struct	tagExtEvent
{
	SvcEpg_EventId_t						stEventId;
	UNIXTIME						ulStartTime;
	UNIXTIME						ulDuration;

#if	defined(CONFIG_MW_EPG_TVTV)
	HUINT8							aucCountry[EPG_LANG_ISO_639_LEN + EPG_TVTV_COUNTRY_LEN + 1];
	HUINT8							aucYear[EPG_LANG_ISO_639_LEN + EPG_TVTV_YEAR_LEN + 1];
#endif

	SvcEpg_ExtInfo_t					astExtInfo[DxEPG_EXT_INFO_MAX];

	struct tagExtEvent					*pNext;
} SvcEpg_ExtEvent_t;

/* HD Linkage, For freesat or UK DTT HD */
typedef	struct	tagHdLinkageInfo
{
	HUINT16							usOnId;
	HUINT16							usTsId;
	HUINT16							usSvcId;
	HINT32	nEventId;
	HUINT8							bSimul;
} SvcEpg_HdLinkageInfo_t;

#if	defined(CONFIG_MW_EPG_TVTV)
typedef struct tagEventLink
{
	HUINT8					ucType;
	HUINT32					ulProgId;
} SvcEpg_EventLink_t;


typedef struct tagEpgTvtvEvent
{
//-------------------------------------------------------------
//	common elements
//-------------------------------------------------------------
	HUINT8							ucEpgDataType;
	HUINT16							usVersion;
	HINT32	nEventId;
//-------------------------------------------------------------
//	specific element
//-------------------------------------------------------------
	HUINT16							usEventInfoMask;
	HUINT32							ulProgId;
	HUINT32							ulSeriesId;
	HUINT16							usTvtvId;

	// (.tbl) table file number
	HUINT32							ulTableId;
	HUINT16							usFileIndex;
#if	0
	HUINT16							ucFormat;
	HUINT16							usTotalNumSeries;
#endif
	SvcEpg_ParentalRating_t			astParentalRating[DxEPG_PARENTAL_RATING_MAX];
	SvcEpg_EventLink_t					astEventLink[EPG_TVTV_LINK_MAX];
	HUINT8							aucSeriesNumber[EPG_TVTV_SERIES_NUMBER_LEN];
	HUINT8							aucImageName[EPG_TVTV_FILENAME_LEN];
	HUINT16							ausCategory[EPG_TVTV_CATEGORY_LEN];
	HUINT8							ucIsImage;
	HUINT8							ucTip;
	HUINT8							ucContent;
	HUINT32 							ulSeason;
	HUINT32 							ulTotalNum;
	HUINT8							aucIsoCountryCode[EPG_LANG_ISO_639_LEN + 1];
} SvcEpg_TvtvEvent_t;


typedef struct tagTvtvEventFileTable
{
	HUINT32							ulTableVersion;
	HUINT32							ulSizeOfFile;
	HUINT32							ulNumOfFile;
	HUINT32							aulFileSizeTable[TVTV_EVENTFILE_NUM_MAX];
} SvcEpg_TvtvEventFileTable_t;



typedef enum
{
	eTvtvLog_DL_Progress,
	eTvtvLog_DL_SkipByHddData,
	eTvtvLog_DL_FailByHdd,


	eTvtvLog_Max
} SvcEpg_TvtvLog_e;

typedef struct tagTvtvLogTbl
{
	HUINT8		ucCurrent;
	HUINT8		aucLog[5][128 + 1];
} SvcEpg_TvtvLogTable_t;

#endif

typedef	struct tagGuidanceInfo
{
	HUINT8							ucGuidanceType;
	HUINT8							aucIsoCountryCode[EPG_LANG_ISO_639_LEN + 1];
	HUINT8							szGuidanceText[EPG_GUIDANCE_TEXT_LEN + 1];
} SvcEpg_GuidanceInfo_t;

/*********************************/
/* FSAT spec.                */
/*********************************/

typedef	struct tagOnlineAvailablabilityInfo
{
	HUINT8		ucMediaUsage;
	UNIXTIME	ulStartDateTime;
	UNIXTIME	ulStopDateTime;
	HBOOL		bAvailableNow;

} SvcEpg_OnlineAvailableInfo_t;

typedef	struct tagShortEventPromotionalInfo
{
// short_event_promotional_info
	HUINT8		aucIsoCountryCode[EPG_LANG_ISO_639_LEN + 1];
	HUINT8		szPromotionalText[EPG_SHORT_EVENT_PROMO_LEN + 1];
} SvcEpg_ShortEventPromotionalInfo_t;

typedef struct tagMediaLocatorInfo
{
	HUINT8		ucMediaType;
	HUINT8		ucMediaUsage;
	HBOOL		bMediaPrefixLocated;
	HUINT8		ucUriPrefixIdx;
	HUINT8		szURIChar[EPG_MEDIA_LOCATOR_LEN +1];
	struct tagMediaLocatorInfo *pNext;
} SvcEpg_MediaLocatorInfo_t;


/*********************************/
/* Japan ext spec.                */
/*********************************/


/*
	0x00 Reserved for future use
	0x01 Audio, 1/0 mode (single mono)
	0x02 Audio, 1/0+1/0 mode (dual mono)
	0x03 Audio, 2/0 mode (stereo)
	0x04 Audio, 2/1 mode
	0x05 Audio, 3/0 mode
	0x06 Audio, 2/2 mode
	0x07 Audio, 3/1 mode
	0x08 Audio, 3/2 mode
	0x09 Audio, 3/2+LFE mode
	0x0A . 0x3F Reserved for future use
	0x40 Audio description for the visually impaired
	0x41 Audio for the hard of hearing
	0x42 . 0xAF Reserved for future use
	0xB0 . 0xFE User-defined
	0xFF Reserved for future use
*/
typedef enum tagEpgAudioComponentType
{
	eEpgAudioType_None 				= 0x0,

	eEpgAudioType_SingleMono		= 0x1,
	eEpgAudioType_DualMono			= 0x2,
	eEpgAudioType_Stereo			= 0x3,

	eEpgAudioType_2_1				= 0x4,
	eEpgAudioType_3_0				= 0x5,
	eEpgAudioType_2_2				= 0x6,

	eEpgAudioType_Surround_3_1 		= 0x7,
	eEpgAudioType_Surround_3_2 		= 0x8,
	eEpgAudioType_Surround_3_2_LFE 	= 0x9,

	eEpgAudioType_Max				= 0xFF
} SvcEpg_AudioComponentType_e;




typedef enum tagEpgAudioStreamType
{
	eEpgAudioStreamType_MPEG1_AUDIO = 0x03,
	eEpgAudioStreamType_MPEG2_AUDIO = 0x04,
	eEpgAudioStreamType_AAC_LC 		= 0x0F,

	eEpgAudioStreamType_Dolby_AC3	= 0x81,

	eEpgAudioStreamType_PCM_AIFF 	= 0xAA,
	eEpgAudioStreamType_PCM_RIFF 	= 0xBB,

	eEpgAudioStreamType_Unknown 	= 0xFF,
	eEpgAudioStreamType_Max			= 0xFF
} SvcEpg_AudioStreamType_e;

/*
	0x00 Reserved for future use
	0x01 Video 480i(525i), 4:3 aspect ratio
	0x02 Video 480i(525i), 16:9 aspect ratio, with pan vectors
	0x03 Video 480i(525i), 16:9 aspect ratio, without pan vectors
	0x04 Video 480i(525i), >16:9 aspect ratio
	0x05 . 0xA0 Reserved for future use
	0xA1 Video 480p(525p), 4:3 aspect ratio
	0xA2 Video 480p(525p), 16:9 aspect ratio, with pan vectors
	0xA3 Video 480p(525p), 16:9 aspect ratio, without pan vectors
	0xA4 Video 480p(525p), >16:9 aspect ratio
	0xA5 . 0xB0 Reserved for future use
	0xB1 Video 1080i(1125i), 4:3 aspect ratio
	0xB2 Video 1080i(1125i), 16:9 aspect ratio, with pan vectors
	0xB3 Video 1080i(1125i), 16:9 aspect ratio, without pan vectors
	0xB4 Video 1080i(1125i), >16:9 aspect ratio
	0xB5 . 0xC0 Reserved for future use
	0xC1 Video 720p(750p), 4:3 aspect ratio
	0xC2 Video 720p(750p), 16:9 aspect ratio, with pan vectors
	0xC3 Video 720p(750p), 16:9 aspect ratio, without pan vectors
	0xC4 Video 720p(750p), >16:9 aspect ratio
	0xC5 . 0xD0 Reserved for future use
	0xD1 Video 240p, 4:3 aspect ratio
	0xD2 Video 240p, 4:3 aspect ratio, with pan vectors
	0xD3 Video 240p, 4:3 aspect ratio, without pan vector
	0xD4 Video 240p, 4:3 aspect ratio > 16:9
	0xD5 . 0xFF Reserved for future use
*/

typedef enum tagEpgVideoResoultion
{
	/*
		PV   : pan vectors
		WOPV : without pan vectors
		WPV  : with pan vectors
		OVER : 16_9 over
	*/
	eEpgVideoRes_525i_4_3 = 0x01,
	eEpgVideoRes_525i_16_9_WPV = 0x02,
	eEpgVideoRes_525i_16_9_WOPV = 0x03,
	eEpgVideoRes_525i_16_9_OVER = 0x04,

	eEpgVideoRes_525p_4_3 = 0xA1,
	eEpgVideoRes_525p_16_9_WPV = 0xA2,
	eEpgVideoRes_525p_16_9_WOPV = 0xA3,
	eEpgVideoRes_525p_16_9_OVER = 0xA4,

	eEpgVideoRes_1125i_4_3 = 0xB1,
	eEpgVideoRes_1125i_16_9_WPV = 0xB2,
	eEpgVideoRes_1125i_16_9_WOPV = 0xB3,
	eEpgVideoRes_1125i_16_9_OVER = 0xB4,

	eEpgVideoRes_750p_4_3 = 0xC1,
	eEpgVideoRes_750p_16_9_WPV = 0xC2,
	eEpgVideoRes_750p_16_9_WOPV = 0xC3,
	eEpgVideoRes_750p_16_9_OVER = 0xC4,

	eEpgVideoRes_240p_4_3 = 0xD1,
	eEpgVideoRes_240p_4_3_WPV = 0xD2,
	eEpgVideoRes_240p_4_3_WOPV = 0xD3,
	eEpgVideoRes_240p_4_3_OVER = 0xD4,

	eEpgVideoRes_Max			= 0xFF
} SvcEpg_VideoResoultion_e;


/*********************************/
/* Epg Eit Common.               */
/*********************************/

typedef struct tagEpgEitEvent
{
//-------------------------------------------------------------
//	common elements
//-------------------------------------------------------------
	HUINT8							ucEpgDataType;
	HUINT8							ucVersion;
	HINT32	nEventId;
//-------------------------------------------------------------
//	specific element
//-------------------------------------------------------------
	HUINT16							bwEitDesMask;
	SvcEpg_ParentalRating_t			astParentalRating[DxEPG_PARENTAL_RATING_MAX];	// in parental_rating Descriptor
	HUINT16							usRefServiceId;								// in time_shift Descriptor
	HINT32							usRefEventId;								// in time_shift Descriptor
//	HUINT8							ucContent;									// in Content Descriptor
	//-------------------------------------------------------------
	// drm : 3 bit of user_nibble_2 in Content Descriptor
	// content protection : 3 bit of user_nibble_1 in Content Descriptor
	//-------------------------------------------------------------
	HUINT8							aucUserContent[DxEPG_CONTENTS_MAX];			// in Content Descriptor
	SvcEpg_ComponentDesc_t			astComponentDesc[EPG_COMPONENT_DESC_MAX];

#if (defined(CONFIG_OP_UK_DTT_DBOOK_VER_6) || defined(CONFIG_OP_FREESAT))
	SvcEpg_HdLinkageInfo_t			astHdLinkageInfo[DxEPG_LINKAGE_INFO_MAX];
#endif
#if defined(CONFIG_MW_EPG_FTA_CONTENT_MGM_DES)
	HUINT8							ucFtaContMgmValue;
#endif
#if defined(CONFIG_OP_FREESAT)
	SvcEpg_GuidanceInfo_t				stGuidance[DxEPG_GUIDANCE_INFO_MAX];
#endif
#if defined(CONFIG_OP_FREESAT_GEN2)
	HUINT32 						ulEitDesFsatExtMask;			// EPG_FSATSPEC_DES_MASK_e
	SvcEpg_OnlineAvailableInfo_t			astOnlineAvailInfo[EPG_ONLINE_AVAIL_INFO_MAX];
	SvcEpg_ShortEventPromotionalInfo_t	astShortEventPromoInfo[DxEPG_SHORT_EVENT_PROMO_INFO_MAX];
	SvcEpg_MediaLocatorInfo_t		astMediaLocatorInfo[DxEPG_MEDIA_LOCATOR_INFO_MAX];
#endif
} SvcEpg_EitEvent_t;


typedef union tagEpgInfo
{
//-------------------------------------------------------------
//	common elements
//-------------------------------------------------------------
	HUINT8							ucEpgDataType;	/* SvcEpg_DataType_e */
//-------------------------------------------------------------
//	specific element
//-------------------------------------------------------------
	SvcEpg_EitEvent_t					stEitEventInfo;
#if	defined(CONFIG_MW_EPG_TVTV)
	SvcEpg_TvtvEvent_t				stTvtvEventInfo;
#endif
} SvcEpg_EpgInfo_u;

typedef struct tagEpgEvent
{
	UNIXTIME						ulStartTime;
	UNIXTIME						ulDuration;
	UNIXTIME						ulOptimizeDuration;

	HBIT							u3RunnigStatus			: 3;
	HBIT							u1FreeCaMode			: 1;
#if defined(CONFIG_OP_FREESAT)
	HBIT							u1ExistTeletext			: 1;
	HBIT							u1bSubtitle				: 1;
	HBIT							u1bAudioVisualImpared	: 1;
	HBIT							u1bAudioHardOfHearing	: 1;

	HBIT							u1bAudioAC3				: 1;
	HBIT							u1bAudioEAC3			: 1;
	HBIT							u1bAudioAAC				: 1;
	HBIT							u1bMHEG					: 1;
	HBIT							u1bInvalidEvent			: 1;

	HBIT							u1bHearingImpaired		: 1;	/* Open (in-vision) sign language interpretation for the deaf [ref.D-BOOK 6.2 & Freesat Part 3]*/
	HBIT							u18Reserved 			: 18;
#elif defined(CONFIG_MW_CAS_NAGRA_FEATURE_PPV) && defined(CONFIG_MW_CAS_NAGRA_OP_RCS)
	HBIT							u8bPpvFlag				: 8;	// Not(0), PPV(1), IPPV(2)
	HBIT	 						u20Reserved 			: 20;
#else
	HBIT							u20Reserved				: 28;
#endif
	HUINT32							ulRuningStatusTimeGap;
	HUINT8							aucContent[DxEPG_CONTENTS_MAX];

	SvcEpg_ShortText_t				astShortText[DxEPG_SHORT_TEXT_MAX];

	HUINT8						ucEventName[EPG_LANG_ISO_639_LEN + 50 + 1];
	HUINT8						ucEventText[EPG_LANG_ISO_639_LEN + 40 + 1];
	HUINT8						ucEventDText[EPG_LANG_ISO_639_LEN + 800 + 1];

	SvcEpg_EpgInfo_u						stEventInfo;

	struct tagEpgEvent				*pNext;
} SvcEpg_Event_t;


typedef struct tagEpgFindService
{
	Handle_t				hSvcHandle;
	HUINT16					usSvcType;
	POINTER_LIST_T			*pEventList;
} SvcEpg_FindSvc_t;

typedef struct tagEpgFindParam
{
	HUINT32			ulFindMask;			/* SvcEpg_FindMask_e */
	EPG_SpecType_e	eSpec;
	Handle_t		hSvcHandle;
	UNIXTIME		ulStartTime;
	UNIXTIME		ulEndTime;
	HUINT16			usMainGenre;
	HUINT16			usSubGenre;
	HUINT8			aucKeyword[EPG_FIND_KEYWORD_LEN + 1];
#if	defined(CONFIG_MW_EPG_TVTV)
	HUINT32			ulProgId;
	HUINT32			ulTableId;
	HUINT32			usFileIndex;
#endif
#if	defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	HUINT32		ulContentKind;
	HUINT8		astCRIDAuth[EPG_TVA_CRID_AUTH_LEN + 1];
	HUINT8		astCRIDData[EPG_TVA_CRID_DATA_LEN + 1];
	HUINT8		astCRIDIMI [EPG_TVA_CRID_IMI_LEN  + 1];
#endif
	HINT32	nEvtId;
	HUINT8		aucManuLang1[EPG_LANG_ISO_639_LEN + 1];
	HUINT8		aucManuLang2[EPG_LANG_ISO_639_LEN + 1];
	HUINT8		aucCountry[EPG_LANG_ISO_639_LEN + 1];
} SvcEpg_FindParam_t;


typedef enum tag_EPG_KIND
{
	eEPG_KIND_DVB = 0,
	eEPG_KIND_TVTV,
	eEPG_KIND_EIT_PLUS,
	eEPG_KIND_FSAT,
	eEPG_KIND_HUMAX,
	eEPG_KIND_MAX,
} SvcEpg_EpgKind_e;

typedef enum tag_EVT_KIND
{
	eEPG_EVT_KIND_ALL		= 0,
	eEPG_EVT_KIND_PF_ALL,
	eEPG_EVT_KIND_PF_ACT,
	eEPG_EVT_KIND_PF_OTH,
	eEPG_EVT_KIND_SCH_ALL,
	eEPG_EVT_KIND_SCH_ACT,
	eEPG_EVT_KIND_SCH_OTH,
	eEPG_EVT_KIND_PFEXT_OTH,
	eEPG_EVT_KIND_SCHEXT_OTH
} SvcEpg_EventKind_e;


// about epg3g manager

typedef enum
{
	eOxEPG_FIELD_NONE,

	eOxEPG_FIELD_START_TIME,
	eOxEPG_FIELD_END_TIME,
	eOxEPG_FIELD_DURATION,
	eOxEPG_FIELD_EVENT_NAME,
	eOxEPG_FIELD_EVENT_TEXT,
	eOxEPG_FIELD_EVENT_LONGTEXT,
	eOxEPG_FIELD_EVENT_ID,
	eOxEPG_FIELD_PARENTAL_RATING,
	eOxEPG_FIELD_VERSION,
	eOxEPG_FIELD_UNIQUE_ID,

	eOxEPG_FIELD_LINK,	// include CRID

	eOxEPG_FIELD_ALPHABET_FLAG,
	// add more field here (without #ifdef)
	eOxEPG_FIELD_GENRE,	// genre search

	eOxEPG_FIELD_MAX
} SvcEpg_Field_e;


typedef enum
{
	eOxEPG_COMPARISON_NONE,

	eOxEPG_COMPARISON_EQ,
	eOxEPG_COMPARISON_NOT_EQ,
	eOxEPG_COMPARISON_GT,
	eOxEPG_COMPARISON_GT_OR_EQ,
	eOxEPG_COMPARISON_LT,
	eOxEPG_COMPARISON_LT_OR_EQ,
	eOxEPG_COMPARISON_CONTAINS,
	eOxEPG_COMPARISON_EXIST,
	eOxEPG_COMPARISON_BIT,

	eOxEPG_COMPARISON_MAX
} SvcEpg_Comparison_e;

typedef struct event_query
{
	HCHAR				type;			///< type ('&', '|', '!', '\0')
	SvcEpg_Field_e		field;			///< field
	SvcEpg_Comparison_e	comparison; 	///< comparision
	HCHAR				*value;			///< value (String)
	HINT32				valueInt;		///< value (Integer)

	HxLANG_Id_e		language;
	HxCountry_e			country;

	struct event_query	*left;
	struct event_query	*right;
} SvcEpg_Query_t;


typedef enum event_type
{
	  eOxEPG_TYPE_NONE		= 0
	, eOxEPG_TYPE_PF		= 0x00000001
	, eOxEPG_TYPE_SC		= 0x00000002

//	CONFIG_OP_FREESAT	>>>
	, eOxEPG_TYPE_PF_EX	= 0x00000004	// PF++ : 0xD1
	, eOxEPG_TYPE_SC_EX	= 0x00000008	// SC++ : 0x
//	CONFIG_OP_FREESAT	<<<

	, eOxEPG_TYPE_DVB		= 0x000000FF

	, eOxEPG_TYPE_TVTV		= 0x00000100
	, eOxEPG_TYPE_IP		= 0x00001000

	, eOxEPG_TYPE_ALL		= 0xFFFFFFFF
} SvcEpg_Type_b;

typedef struct _SvcEpg_EventInfo	SvcEpg_EventInfo_t;
typedef struct _SvcEpg_EventClass	SvcEpg_EventClass_t;

struct _SvcEpg_EventInfo
{
	const SvcEpg_EventClass_t	*clazz;

	HINT32			refCount;	///< reference count
	SvcEpg_Type_b	type;		///< epg type (@see SvcEpg_Type_b)
	HUID			uid;		///< event unique id
	HINT32			svcuid;		///< service unique id
	HINT32			eventid;	///< event id
	HUINT32			startTime;	///< start time (UNIXTIME)
	HUINT32			duration;	///< duration   (UNIXTIME)
	HUINT32			version;	///< (DVB: crc, TVTV: svcid + continuity indicator, ...)
	HUINT32			runstatus;	///< running status
	/* #if defined( OxEPG_TABLESECTION )잇는 곳에서만 사용, 나머지는 값은 넣어지나 사용 안함 */
	HUINT32			tableSection;	// svcuid ( upper 16 bit ) + tableid ( 8 bit ) + sectionum ( 8 bit )
};

struct _SvcEpg_EventClass
{
	HUINT32					ulSize;

	SvcEpg_EventInfo_t *	(* fnCtor)   (SvcEpg_EventInfo_t *, va_list);
	void					(* fnDtor)   (SvcEpg_EventInfo_t *);
	SvcEpg_EventInfo_t *	(* fnClone)  (const SvcEpg_EventInfo_t *);
	HINT32					(* fnCompare)(const SvcEpg_EventInfo_t *, const SvcEpg_EventInfo_t *);
	HINT32					(* fnMatch)  (SvcEpg_EventInfo_t *, const SvcEpg_Query_t *);
	void					(* fnPrint)  (SvcEpg_EventInfo_t *, FILE *);
	HBOOL					(* fnIsUpdated) (SvcEpg_EventInfo_t *, va_list);

	const HCHAR *			(* fnGetName) (SvcEpg_EventInfo_t *, HxLANG_Id_e);
	const HCHAR *			(* fnGetText) (SvcEpg_EventInfo_t *, HxLANG_Id_e);
	const HCHAR *			(* fnGetLongText) (SvcEpg_EventInfo_t *, HxLANG_Id_e);
	HINT32					(* fnGetParentalRating) (SvcEpg_EventInfo_t *, HxCountry_e);
};

typedef HERROR (* SvcEpg_SiGetTimeOutCb_t)(Handle_t hAction, SiTableType_e eTableType, HUINT32 *pulTimeout);


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

//svc_epg.c
HERROR			SVC_EPG_Init(PostMsgToMgrCb_t fnSvcEpgPostMsgCb);
HERROR			SVC_EPG_InitAction(SvcEpg_EpgKind_e eEpgKind);
HERROR			SVC_EPG_Start(SvcEpg_EpgKind_e eEpgKind, SvcEpg_EventKind_e eEvtKind, Handle_t hAction, Handle_t hService, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);
HERROR			SVC_EPG_Stop(SvcEpg_EpgKind_e eEpgKind, SvcEpg_EventKind_e eEvtKind, Handle_t hAction);
HERROR			SVC_EPG_LoadData(SvcEpg_EpgKind_e eEpgKind, Handle_t hAction, Handle_t hService);
HERROR			SVC_EPG_UnLoadData(SvcEpg_EpgKind_e eEpgKind, Handle_t hAction);
HERROR			SVC_EPG_StartContent(SvcEpg_EpgKind_e eEpgKind, Handle_t hAction, Handle_t hService);
HERROR			SVC_EPG_StopContent(SvcEpg_EpgKind_e eEpgKind, Handle_t hAction);
HERROR			SVC_EPG_CreateCheckServiceList(void);
// --> ip epg
void			SVC_EPG_SetServiceConstraint(SvcEpg_Type_b bwType, const HxVector_t *svcuidList);
// <-- ip epg

// --> Freesat
void		SVC_EPG_StopSerialization (void);
#if 0
DesHeader_t * SVC_EPG_GetFirstCachedDescriptorWithLanguage (SvcEpg_EventInfo_t *self, HUINT8 tag, HxLANG_Id_e language);
#endif
//<-- Freesat


#if defined(CONFIG_MW_EPG_TVTV)
HERROR 		SVC_EPG_CmdCreateBuffer_Tvtv(void);
HERROR 		SVC_EPG_CmdCreateTvstInfo_Tvtv(void);
HERROR 		SVC_EPG_CmdCreateDgktInfo_Tvtv(HBOOL bNeedConverting);
HERROR		SVC_EPG_CmdCreateTvtvSystemInfo_Tvtv(HBOOL bNeedConverting);
HERROR		SVC_EPG_CmdCreateTsmtInfo_Tvtv(HBOOL bNeedConverting);
HERROR		SVC_EPG_CmdCreateLtotInfo_Tvtv(HBOOL bNeedConverting);
HERROR		SVC_EPG_CmdCreateEpgInfo_Tvtv(HBOOL bNeedConverting);
void 		SVC_EPG_CmdInitBuffer_Tvtv(void);
#endif

// svc_epg_api.c
#if defined(CONFIG_DEBUG)
void		SVC_EPG_PrintEventTree(void);
void		SVC_EPG_PrintEpgResource(void);
void		SVC_EPG_PrintFindParam(SvcEpg_FindParam_t *pParam);
void		SVC_EPG_PrintEventStruct(SvcEpg_Event_t *pstEpgEvt);
void		SVC_EPG_PrintFilterAllocCount(void);
void 		SVC_EPG_PrintEventsTime(HINT32 type, Handle_t hSvc, UNIXTIME utTime, UNIXTIME utEnd );
void 		SVC_EPG_PrintEventsGenre(HINT32 type, Handle_t hSvc, HUINT32 ulGenre );
void 		SVC_EPG_PrintEventsKeyword(HINT32 type, Handle_t hSvc, HCHAR *pstKeyword );
#endif
void			SVC_EPGSERVICE_Print(void);
HERROR		SVC_EPG_SiRegisterTimeoutCallback(SvcEpg_SiGetTimeOutCb_t fnSiTimeout);

#endif /* !__EPGMAIN_H__ */
