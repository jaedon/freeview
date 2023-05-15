/** **********************************************************************************************************
	@file 		svc_mplayersi.h

	@date		2012/01/03
	@author		IPM-SW1T (tipmsw1@humaxdigital.com)
	@breif

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. ("Humax") and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
	EITHER EXPRESS, IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
	IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR
	INDIRECT, SPECIAL, CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR
	INABILITY TO USE THE SOFTWARE.

	This License is effective until terminated. You may terminate this License at any time
	by destroying all copies of the Software including all documentation. This License will terminate
	immediately without notice from Humax to you if you fail to comply with any provision of this License.
	Upon termination, you must destroy all copies of the Software and all documentation.

	The laws of the Republic of Korea will apply to any disputes arising out of or relating to
	this Copyright Notice. All claims arising out of or relating to this Copyright Notice will be litigated
	in the Seoul Central District Court, in the Republic of Korea.
*********************************************************************************************************** */

#ifndef	__INT_MW_MP_H__
#define	__INT_MW_MP_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#if 0
#if defined(CONFIG_MW_MM_DLNA_DMP_SI)

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#define	MEDIA_MAX_SECTION_NUM			256
#define	MEDIA_FILTER_ID_NULL			0xFFFF
#define	MEDIA_RxRSCID_NULL				0xFFFFFFFF

#define MEDIA_SI_SHORT_EVENT_NAME_LEN		256
#define MEDIA_SI_EVENT_TEXT					256
#define MEDIA_SI_MAX_LEN_AUD_NAME			37 /* spec 33 bytes + preStr 3 bytes + 1 */
#define MEDIA_SI_MAX_AUDIO_ES_NUM			16
#define MEDIA_SI_MAX_AUDIO_LANG_NUM 		4
#define MEDIA_SI_MAX_VIDEO_ES_NUM			16
#define MEDIA_SI_MAX_ES_STRING_LEN			81
#define MEDIA_SI_MAX_SUBTTL_ES_NUM			12
#define MEDIA_SI_MAX_TELTXT_ES_NUM			8
#define MEDIA_SI_MAX_TTX_INITAIL_PAGE		MEDIA_SI_MAX_TELTXT_ES_NUM

#define MEDIA_SI_AUDIO_CHANNEL_NUM_NA				0x00
#define MEDIA_SI_AUDIO_CHANNEL_NUM_STEREO			0x01
#define MEDIA_SI_AUDIO_CHANNEL_NUM_MULTI			0x02

// subtitle/teletext
#define MEDIA_SUBTITLE_INDEX_NULL		0xff
#define	MEDIA_INITIAL_TELETEXT			0x01
#define MEDIA_TELETEXT_SUBTITLE 		0x02
#define MEDIA_ADDITIONAL_INFO_PAGE		0x03
#define MEDIA_PROG_SCHEDULE_PAGE		0x04
#define MEDIA_TTX_SBTL_PAGE_HERARING_IMPAIRED	0x05
#define MEDIA_TTX_MAGAZINE_NONE			9

#define	MEDIA_Util_AC3_ComponentTypeToChannelNum(a)		(((a&0x07)<=0x03) ? MEDIA_SI_AUDIO_CHANNEL_NUM_STEREO : (((a&0x07)<=0x05) ? MEDIA_SI_AUDIO_CHANNEL_NUM_MULTI : MEDIA_SI_AUDIO_CHANNEL_NUM_NA))
#define	MEDIA_Util_AAC_ProfileLevelToChannelNum(a)		(((a>=0x50) && (a<=0x59)) ? MEDIA_SI_AUDIO_CHANNEL_NUM_STEREO : (((a>=0x5A) && (a<=0x5B)) ? MEDIA_SI_AUDIO_CHANNEL_NUM_MULTI : MEDIA_SI_AUDIO_CHANNEL_NUM_NA))


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eMEDIA_RETRIEVE_PAT				= 0x00000001,
	eMEDIA_RETRIEVE_PMT				= 0x00000002,
	eMEDIA_RETRIEVE_CAT				= 0x00000004,
	eMEDIA_RETRIEVE_BAT				= 0x00000008,
	eMEDIA_RETRIEVE_SDT_ACT			= 0x00000010,
	eMEDIA_RETRIEVE_SDT_OTH			= 0x00000020,
	eMEDIA_RETRIEVE_NIT_ACT			= 0x00000040,
	eMEDIA_RETRIEVE_NIT_OTH			= 0x00000080,
	eMEDIA_RETRIEVE_EIT_ACT_PF		= 0x00000100,
	eMEDIA_RETRIEVE_EIT_OTH_PF		= 0x00000200,
	eMEDIA_RETRIEVE_EIT_ACT_SCH		= 0x00000400,
	eMEDIA_RETRIEVE_EIT_OTH_SCH		= 0x00000800,
	eMEDIA_RETRIEVE_TDT				= 0x00001000,
	eMEDIA_RETRIEVE_TOT				= 0x00002000,
	eMEDIA_RETRIEVE_BIT				= 0x00004000,
	eMEDIA_RETRIEVE_SIT				= 0x00008000,

	eMEDIA_RETRIEVE_DRM				= 0x10000000,  // DRM table
	eMEDIA_RETRIEVE_END				= 0xFFFFFFFF
} MEDIA_RetrieveTableType_t;

#define MW_MEDIA_MAX_VIDEO_ES_NUM			(2)
#define MW_MEDIA_MAX_AUDIO_ES_NUM			(16)
#define MW_MEDIA_MAX_CAPTION_ES_NUM			(2)		/* Super 포함 */
#define MW_MEDIA_MAX_DATA_ES_NUM			(12)	/* TR-B14 제3편 2.1.2.5 ES수의 제한 */
#define MW_MEDIA_MAX_ES_STRING_LEN			(81) 	/* video : 16, audio : 33, data : 80 + 1 */
#define MW_MEDIA_MAX_LEN_AUD_NAME			(32)
#define MW_MEDIA_MAX_AUD_NUM 				(4)


#define MW_MEDIA_LANG_ISO_639_LEN			(3)
#define MW_MEDIA_EXT_ITEM_MAX				(15)
#define MW_MEDIA_PARENTAL_RATING_MAX		(3)
#define MW_MEDIA_SHORT_EVENT_NAME_LEN		(256)
#define MW_MEDIA_EVENT_TEXT					(256)


/** @brief VIDEO infomation in PMT */
typedef struct tagMW_MEDIA_VIDEO_INFO
{
	HUINT16			usPid;						/** < Video PID */
	DxVideoCodec_e	eVideoCodec;				/** < Video Codec */
	HUINT8			ucComponentTag;				/** < componet tag value */
	HBOOL			bIsHD;

	/* AVVideoComponent Class에 데이터 전달을 위해 필요 */
	HUINT8			ucStreamType;
	HUINT8			ucComponentType;
	HUINT8			aucStreamName[MW_MEDIA_MAX_ES_STRING_LEN];
} MW_MEDIA_VideoInfo_t;

typedef struct tagMW_MEDIA_ISO_639_LAN_INFO
{
	AUDIO_Type_t	eAudType;
	HUINT8			szIso639LangCode[MW_MEDIA_LANG_ISO_639_LEN + 1];
} MW_MEDIA_Iso639LangInfo_t;

/** @brief AUDIO infomation in PMT */
typedef struct tagMW_MEDIA_AUDIO_INFO
{
	HUINT16			usPid;						/** < Auido PID */
	DxAudioCodec_e	eAudCodec;					/** < Audio Codec */
	MW_MEDIA_Iso639LangInfo_t	stIsoLangInfo[MW_MEDIA_MAX_AUD_NUM];
	HUINT8			ucComponentTag;				/** < componet tag value */
	HUINT8			ucChannelNum;
	HUINT8			ucComponentType;
	HUINT8			ucStreamType;
	HUINT8			ucStreamContent;			/* For JLabs */
	HUINT8			aucStreamName[MW_MEDIA_MAX_LEN_AUD_NAME];
} MW_MEDIA_AudioInfo_t;

/** @brief CcSuper infomation in PMT */
typedef struct tagMW_MEDIA_CC_SUPER_INFO
{
	HUINT16		usPid;
	HUINT8		ucComponentTag;
	HUINT32		unISO639LanguageCode;
} MW_MEDIA_CcSuperInfo_t;

/** @brief ES information in PMT. */
typedef struct tagMW_MEDIA_PMT_INFO
{
	HUINT16			usServiceId;				/** < Service ID */
	HUINT16			usPcrPid;					/** < PCR PID */
	HUINT8			ucVerNum;
	HUINT32			ulCrc;

	HUINT16					usVideoNum;										/** < number of VIDEO */
	MW_MEDIA_VideoInfo_t	stVideoInfo [MW_MEDIA_MAX_VIDEO_ES_NUM];		/** < VIDEO contents */

	HUINT16					usAudioNum;										/** < number of AUDIO */
	MW_MEDIA_AudioInfo_t	stAudioInfo [MW_MEDIA_MAX_AUDIO_ES_NUM];		/** < AUDIO contents */

	HUINT16					usCaptionNum;									/** < number of caption */
	MW_MEDIA_CcSuperInfo_t	stCaptionInfo [MW_MEDIA_MAX_CAPTION_ES_NUM];	/** < Caption contents */
	DxCopyrightControl_t 				stDrmInfo;
} MW_MEDIA_PmtInfo_t;

typedef struct	tagMW_MEDIA_PvrParentalRating
{
	HUINT8			aucIsoCountryCode[MW_MEDIA_LANG_ISO_639_LEN + 1];
	HUINT8			ucParentalRating;
} MW_MEDIA_ParentalRating_t;

typedef struct tagMW_MEDIA_EXT_EVT_ITEM
{
	HUINT8			*pszDescription;
	HUINT8			*pszItem;
} MW_MEDIA_ExtEvtItem_t;

typedef struct tagMW_MEDIA_EXT_EVT
{
	HUINT8			ucDesNum;
	HUINT8			ucLastDesNum;
	HUINT8			szIso639LangCode[MW_MEDIA_LANG_ISO_639_LEN + 1];
	HUINT8			ucItemNum;
	MW_MEDIA_ExtEvtItem_t	astItem[MW_MEDIA_EXT_ITEM_MAX];
	HUINT8			*pszChar;

	struct tagMW_MEDIA_EXT_EVT *pNext;
} MW_MEDIA_ExtEvt_t;

typedef struct tagMW_MEDIA_EventInfo
{
	UNIXTIME		ulStartTime;
	UNIXTIME		ulDuration;
//------------------ 32 bit Align ---------------------------------------
	HBIT			u3RunnigStatus			: 3;
	HBIT			u1FreeCaMode			: 1;
	HBIT			ucReserved				: 28;
//-----------------------------------------------------------------------

	HUINT8			ucParentalRatingNum;
	MW_MEDIA_ParentalRating_t	astParentalRating[MW_MEDIA_PARENTAL_RATING_MAX];
	HUINT8			ucContentNibble;
	HUINT8			ucUserNibble;

	HUINT8			aucEventName[MW_MEDIA_SHORT_EVENT_NAME_LEN];
	HUINT8			aucEventText[MW_MEDIA_EVENT_TEXT];
	HINT32	nEvtId;

	MW_MEDIA_ExtEvt_t		*pstExtEvt;
} MW_MEDIA_EventInfo_t;


/** @brief Media si context */
typedef struct
{
	HBOOL				bInit;
	Handle_t			hAction;
	HULONG				ulSemId;
	HULONG				ulRawSem;
#if 0
	MwcRawPool			*pstRawDataPool;
#endif
	HUINT32				ulDemuxId;
	HUINT16				usPmtSvcId;					// PMT Program Number (Service ID)
	HUINT16				usPmtPid;					// PMT PID
	HUINT32				ulRetrieveTables;
	HUINT32				ulReceivedTables;
	HUINT16				usNetworkId;
	HUINT16				usTsUniqId;
	HUINT16				usTsId;
	HUINT16 			usMediaType;
	HUINT32				aulFilterId[MAX_SI_TABLE_TYPE];	// for checking the timeout

	HBOOL					bPmtValid;
	MW_MEDIA_PmtInfo_t		stPmtInfo;
	HBOOL					bEventValid;
	MW_MEDIA_EventInfo_t 	stEvtInfo;
} MW_MediaSiContext_t;


/**@Breif Audio Info */
typedef struct
{
	HUINT16			usPid;					/**< Audio PID */
	DxAudioCodec_e	eCodec;					/**< Audio Codec */
	HBOOL			bIsDolby;				/**< Dolby? */
	MW_MEDIA_Iso639LangInfo_t	stIsoLangInfo[MW_MEDIA_MAX_AUD_NUM];

	/* Jlabs AVAudioComponent Class에는 변환 되지 않는 값이 필요하다. */
	HUINT8			ucComponentTag;
	HUINT8			ucComponentType;
	HUINT8			ucStreamType;
	HUINT8			ucStreamContent;			/* For JLabs */
	HUINT8			aucStreamName[MW_MEDIA_MAX_LEN_AUD_NAME];
} MW_Media_AUDIO_Info_t;


/**@Breif Audio Info List */
typedef struct
{
	HUINT32				nAudio;
	MW_Media_AUDIO_Info_t	stAudioInfo[MW_MEDIA_MAX_AUDIO_ES_NUM];
} MW_Media_AUDIO_InfoList_t;

typedef struct
{
	HUINT16			usPid;
	HUINT8			ucComponentTag;
	HUINT8			ucStreamType;
	HUINT8			ucComponentType;
	HUINT8			aucStreamName[MW_MEDIA_MAX_ES_STRING_LEN];
} MW_Media_VIDEO_Info_t;

typedef struct
{
	HUINT32					nVideo;
	MW_Media_VIDEO_Info_t	stVideoInfo[MW_MEDIA_MAX_VIDEO_ES_NUM];
} MW_Media_VIDEO_InfoList_t;

typedef struct
{
	HUINT16			usPid;
	HUINT8			ucComponentTag;
	HUINT32			unISO639LanguageCode;				/* For caption and super */
	HUINT8			aucTextChar[MW_MEDIA_MAX_ES_STRING_LEN];	/* For data(BML) */
} MW_Media_DATA_Info_t;

typedef struct
{
	HUINT32					nData;
	MW_Media_DATA_Info_t	stDataInfo[MW_MEDIA_MAX_DATA_ES_NUM];
} MW_Media_DATA_InfoList_t;

typedef struct
{
	MW_Media_VIDEO_InfoList_t	stVideoInfoList;
	MW_Media_AUDIO_InfoList_t	stAudioInfoList;
	MW_Media_DATA_InfoList_t		stCaptionInfoList;
} MW_Media_ES_InfoList_t;	/* Multi-component */


HERROR	SVC_MP_SI_Init(void);
HERROR	SVC_MP_SI_Setup(SvcMp_MediaPlayer_t *player);
HERROR	SVC_MP_SI_Release(void);
HERROR	SVC_MP_SI_StartSI(SvcMp_MediaPlayer_t *player);
HERROR 	SVC_MP_SI_CmdSi(Handle_t hAction, HUINT32 ulRetrieveType);
HERROR	SVC_MP_SI_ReceivedPAT(Handle_t hAction, HUINT32 param1, HUINT32 param2);
HERROR	SVC_MP_SI_ReceivedPMT(Handle_t hAction, HUINT32 param1, HUINT32 param2);
HERROR	SVC_MP_SI_ReceivedSIT(Handle_t hAction, HUINT32 param1, HUINT32 param2);
HERROR	SVC_MP_SI_ReceivedPfEIT(Handle_t	hAction, HUINT32 param1, HUINT32 param2);
HERROR 	SVC_MP_SI_GetTripleId(Handle_t hAction, DbSvc_TripleId_t *pstTripleId);
HERROR 	SVC_MP_SI_GetDrmInfo(Handle_t hAction, DxCopyrightControl_t *pstDrmInfo);
HERROR	SVC_MP_SI_GetVideoList(Handle_t hAction, MW_Media_VIDEO_InfoList_t *pVideoInfo);
HERROR 	SVC_MP_SI_GetAudioList(Handle_t hAction, MW_Media_AUDIO_InfoList_t *pAudInfo);
HERROR 	SVC_MP_SI_GetCaptionSuperList(Handle_t hAction, MW_Media_DATA_InfoList_t *pCaptionSuperInfo);
void	SVC_MP_SI_FreeExtEvt(MW_MEDIA_ExtEvt_t *pstExtEvt);
HERROR 	SVC_MP_SI_GetEventInfo(Handle_t hAction, MW_MEDIA_EventInfo_t *pstEvtInfo);
HERROR 	SVC_MP_SI_FreeEventInfo(MW_MEDIA_EventInfo_t *pstEvtInfo);
HERROR 	SVC_MP_SI_UpdateAudioStereoInfoByStereoType(Handle_t hAction, DxStereoFormat_e eStereoType, HBOOL *pbChangeFlag);

#endif
#endif

#endif	// __INT_MW_MP_H__

