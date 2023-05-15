/* $Header: $ */
#ifndef DLIB_EPG_H__
#define DLIB_EPG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <hlib.h>

typedef enum
{
	  eDxEPG_TYPE_NONE		= 0
	, eDxEPG_TYPE_PF		= 0x0001
	, eDxEPG_TYPE_SC		= 0x0002
	, eDxEPG_TYPE_PF_EX		= 0x0004
	, eDxEPG_TYPE_SC_EX		= 0x0008
	, eDxEPG_TYPE_RF		= 0x00FF
	, eDxEPG_TYPE_TVTV		= 0x0100
	, eDxEPG_TYPE_IP		= 0x1000

	, eDxEPG_TYPE_ALL		= 0xFFFF
} DxEPG_Type_e;

#ifndef CONFIG_FUNC_EPG_USE_SVCUID
typedef enum
{
	  eDxEPG_UID_TVTV		= 0xFF01
	, eDxEPG_UID_IP			= 0xFF02
} DxEPG_UID_e;
/*UID 의 상위 16 bit로 epg type을 구분한다.*/
/*
UID	:	16 BIT		:		16 BIT		:		16 BIT		:		16 BIT
RF	:0000~FF00(onid):		svcid		:		tsid		:		eventid
TVTV:	FF01		:		0000		:			Program Id(32BIT)
IP	:	FF02		:		0000		:			unique Id(32BIT)
*/
#else
/*UID 의 상위 16 bit로 epg type을 구분한다.*/
/*
UID	:	16 BIT			:		16 BIT		:		16 BIT		:		16 BIT
RF	:eDxEPG_TYPE_PF/SC	:		svcuid		:		0000		:		eventid
TVTV:eDxEPG_TYPE_TVTV	:		svcuid		:			Program Id(32BIT)
IP	:eDxEPG_TYPE_IP		:		svcuid		:			unique Id(32BIT)
*/
#endif
#define	DxEPG_SPEC_LEN	32
typedef struct
{
	DxEPG_Type_e	type;
	HUID			uid;
	HCHAR			spec[DxEPG_SPEC_LEN];	// "639:operator:delivery" -> ex) "eng:freesat:sat"
} DxEPG_Header_t;

#define	DxEPG_NAMEOF(type)		(\
								  ((type) & eDxEPG_TYPE_PF) ? DxNAMEOF(eDxEPG_TYPE_PF)		\
								: ((type) & eDxEPG_TYPE_SC) ? DxNAMEOF(eDxEPG_TYPE_SC)		\
								: ((type) & eDxEPG_TYPE_IP) ? DxNAMEOF(eDxEPG_TYPE_IP)		\
								: ((type) & eDxEPG_TYPE_TVTV) ? DxNAMEOF(eDxEPG_TYPE_TVTV)	\
								: NULL	\
								)
#define DxEPG_CLONE_UID 		-1


#define	DxEPG_EVENT_GET_STRING(vEventString)	vEventString?((HCHAR*)((DxEventString_t *)HLIB_VECTOR_First(vEventString))->string):"NULL"

#define DxEPG_CRID_SEARCH_FLAG_ALLSERVICE	( 0x30 )
#define DxEPG_CRID_SEARCH_FLAG_SAMESERVICE	( 0x31 )

/////////////////////////////////////////////////////////////////////////////////////////////////
// RF EPG
/////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
	HxLANG_Id_e	language;
	HCHAR		*string;
} DxEventString_t;

typedef struct
{
	HCHAR		*item;
	HCHAR		*description;
} DxEventItem_t;

typedef struct
{
	HUINT8			content;
	HUINT8			type;
	HUINT8			tag;
	HxLANG_Id_e	language;
	HCHAR			*text;
} DxEventComponent_t;

typedef struct
{
	HUINT8			type;
	HCHAR			*crid;
	
#if defined(CONFIG_OP_TDC)
	HUINT8			search_flag;	// '0' : global search, '1' : single service search
	HCHAR			*title;
#endif
} DxEventCRID_t;

typedef struct
{
	HUINT16			onid;
	HUINT16			tsid;
	HUINT16			svcid;
	HUINT8			linkageType;
	HUINT8			*privateData;
} DxEventLinkage_t;


typedef struct
{
	HUINT8			guidancetype;
	HCHAR			*guidancedata;
} DxEventGuidanceInfo_t;

typedef	struct
{
	HUINT8		mediausage;
	UNIXTIME	startdatetime;
	UNIXTIME	stopdatetime;
	HBOOL		availablenow;

} DxEventOnlineAvailInfo_t;

typedef	struct
{
	HUINT8		mediatype;
	HUINT8		mediausage;
	HBOOL		media_prefix_location;
	HUINT8		media_prefix_index;
	HCHAR		*URI_char;
} DxEventMediaLocatorInfo_t;


typedef struct
{
	HCHAR			*szShortEventPromo;
} DxEventShortEventPromotionInfo_t;



typedef struct
{
	HINT32		refCount;
	HINT32		size;
	HUINT8		*pucCollector;
}DxDataCollector_t;


typedef struct
{
	HUINT32		ulSize;
	HUINT8		*pucUrlType;
	HUINT8		*pucImageUrl;
}DxEventMediaImage_t;

typedef struct
{
	HUINT8		*pucTarget;
	HUINT8		*pucType;
	HUINT8		*pucUrl;
}DxEventMediaUrl_t;

typedef struct
{
	HUINT32		ulPubDate;
	HUINT32		ulExpireDate;
	HUINT32		ulTransmitDate;
	HUINT32		ulPrice;

	HCHAR		*pcMediaId;
	HxVector_t	*mediaUrlList;			// reference to. DxEventMediaUrl_t
	HxVector_t	*mediaImageList;		// reference to. DxEventMediaImage_t
} DxEventMedia_t;

typedef struct
{
	HUINT32		ulSize;
	HUINT32		ulStringType;			//DxIPEPG_UKDTT_TEXT_e
	HUINT8		*pucString;
}DxEventOnDemandString_t;

typedef struct
{
	HUINT32			ulStartOfAvailability;
	HUINT32			ulEndOfAvailability;
	HUINT32			ulPublishedDuration;
	HUINT8			ucFree;
	HUINT8			ucIsHD;
	HUINT32			ulOnDemandGenre;

	HxVector_t		*onDemandStringList;	// reference to. DxEventOnDemandString_t
}DxEventOnDemand_t;

typedef struct
{
	HUINT32 				ulProgId;
	HUINT32					ulStartTime;
	HUINT32					ulEndTime;
	HUINT32					ulEpisodeNum;
	HUINT32					ulSeason;
	HCHAR					*pcSubName;
} DxEventLink_t;


#define	DxEVENT_CONTENT						16




/////////////////////////////////////////////////////////////////////////////////////////////////
// HUMAX EPG
/////////////////////////////////////////////////////////////////////////////////////////////////


#define EPG_HUMAX_MAX_ACTOR_LEN		128
#define	EPG_HUMAX_LINK_MAX			4
#define	EPG_HUMAX_LANG_LEN			3
#define	EPG_HUMAX_NAME_LEN			128
#define EPG_HUMAX_URL_MAX_LEN		256
#define	EPG_HUMAX_SHORT_LEN			512
#define	EPG_HUMAX_DETAIL_LEN		2048


typedef struct tagEventLinkHumax
{
	HUINT8					ucType;
	HUINT32					ulProgId;
}EVENT_LINK_HUMAX_t;

typedef struct
{
	HUINT16					usOnId;		/**< originl network id */
	HUINT16					usTsId;		/**< ts id */
	HUINT16					usSvcId;	/**< service id */
	HUINT32					ulSvcUid;
	HUINT32					ulMediaSupport;	/**< ipepg's mediaSupport value*/
}DxIpService_t;

typedef struct
{
	DxEPG_Type_e			type;
	HUINT8					ucKind;
//-------------------------------------------------------------
//	event info
//-------------------------------------------------------------
	HUINT16 				usOnId; 	/**< originl network id */
	HUINT16 				usTsId; 	/**< ts id */
	HUINT16 				usSvcId;	/**< service id */
	HUINT32 				ulUniqueId;
	HUINT32 				ulSvcUid;
	UNIXTIME				ulStartTime;
	UNIXTIME				ulEndTime;
	HUINT8					ucRating;
	HUINT8					ucDrm;

//-------------------------------------------------------------
//	series info
//-------------------------------------------------------------
	HUINT32 				ulSeriesId;
	HUINT32 				ulEpisodeNum;
	HUINT32 				ulTotalNum;
	HUINT32 				ulSeason;
	HUINT8					ucLinkType;
	HUINT8					ucSeriesType;
	EVENT_LINK_HUMAX_t		astEventLink[EPG_HUMAX_LINK_MAX];	// 추후 alloc해서 사용하는 것으로 변경 필요.

//-------------------------------------------------------------
//	additional info
//-------------------------------------------------------------
	HUINT8					ucGenre;
	HUINT8					ucRecommendation;
	HUINT8					ucFtaContentValue;

//-------------------------------------------------------------
//	string info
//-------------------------------------------------------------
	HUINT8					ucLang[EPG_HUMAX_LANG_LEN];

}DxIpEventBase_t;

#define eDxIPEPG_TEXT_MAXCOUNT	20
#define eDxIPEPG_MEDIA_MAXCOUNT	20
#define eDxIPEPG_LINK_MAXCOUNT	20
#define eDxIPEPG_MEDIAURL_MAXCOUNT	6
#define eDxIPEPG_MEDIAIMAGE_MAXCOUNT 	6
#define eDxIPEPG_CRID_MAXCOUNT	4
#define eDxIPEPG_FVC_OD_MAXCOUNT	8

typedef enum
{
	eDxIPEPG_TEXT_NONE
	, eDxIPEPG_TEXT_NAME
	, eDxIPEPG_TEXT_SUBNAME
	, eDxIPEPG_TEXT_SHORT
	, eDxIPEPG_TEXT_DETAIL
	, eDxIPEPG_TEXT_THUMBNAIL
	, eDxIPEPG_TEXT_ACTOR
	, eDxIPEPG_TEXT_ACTRESS
	, eDxIPEPG_TEXT_DIRECTOR
	, eDxIPEPG_TEXT_PERSON
	, eDxIPEPG_TEXT_MAX
} DxIPEPG_Text_e;

typedef enum
{
	eDxIPEPG_UKDTT_TEXT_NONE
	, eDxIPEPG_UKDTT_TEXT_PROGRAMME_CRID			// optional
	, eDxIPEPG_UKDTT_TEXT_SERIES_CRID				// optional
	, eDxIPEPG_UKDTT_TEXT_RECOMMENDATION_CRID		// optional
	, eDxIPEPG_UKDTT_TEXT_GUIDANCE					// optional
	, eDxIPEPG_UKDTT_TEXT_OD_DELIVERY_MODE			// mandatory
	, eDxIPEPG_UKDTT_TEXT_OD_PROGRAM_URL			// mandatory
	, eDxIPEPG_UKDTT_TEXT_OD_PROGRAM_URL_TYPE		// mandatory
	, eDxIPEPG_UKDTT_TEXT_OD_AUXILIARY_URL			// mandatory
	, eDxIPEPG_UKDTT_TEXT_OD_AUXILIARY_URL_TYPE		// mandatory
} DxIPEPG_UKDTT_TEXT_e;

typedef enum
{
	eDxIPEPG_UKDTT_COMPONENT_NONE				 = 0x00000000
	, eDxIPEPG_UKDTT_COMPONENT_AUDIO_DESCRIPTION = 0x00000001
	, eDxIPEPG_UKDTT_COMPONENT_HD				 = 0x00000002
	, eDxIPEPG_UKDTT_COMPONENT_DVB_SUBTITLE		 = 0x00000004
	, eDxIPEPG_UKDTT_COMPONENT_EBU_SUBTITLE		 = 0x00000008
	, eDxIPEPG_UKDTT_COMPONENT_SIGNED			 = 0x00000010
	, eDxIPEPG_UKDTT_COMPONENT_MONO_AUDIO		 = 0x00000020
	, eDxIPEPG_UKDTT_COMPONENT_STEREO_AUDIO		 = 0x00000040
	, eDxIPEPG_UKDTT_COMPONENT_5_1_AUDIO		 = 0x00000080
} DxIPEPG_UKDTT_COMPONENT_e;

typedef enum
{
	eDxIPEPG_UKDTT_GUIDANCE_NONE				 = 0x00000000
	, eDxIPEPG_UKDTT_RATING_BBFC_UNIVERSAL		 = 0x00000001
	, eDxIPEPG_UKDTT_RATING_BBFC_PG				 = 0x00000002
	, eDxIPEPG_UKDTT_RATING_BBFC_12				 = 0x00000004
	, eDxIPEPG_UKDTT_RATING_BBFC_15				 = 0x00000008
	, eDxIPEPG_UKDTT_RATING_BBFC_18				 = 0x00000010
	, eDxIPEPG_UKDTT_RATING_BBFC_MASK			 = 0x000000FF
	, eDxIPEPG_UKDTT_RATING_FREEVIEW_NO_PC		 = 0x00000100
	, eDxIPEPG_UKDTT_RATING_FREEVIEW_UNRATED	 = 0x00000200
	, eDxIPEPG_UKDTT_RATING_FREEVIEW_PG			 = 0x00000400
	, eDxIPEPG_UKDTT_RATING_FREEVIEW_12			 = 0x00000800
	, eDxIPEPG_UKDTT_RATING_FREEVIEW_15			 = 0x00001000
	, eDxIPEPG_UKDTT_RATING_FREEVIEW_16			 = 0x00002000
	, eDxIPEPG_UKDTT_RATING_FREEVIEW_18			 = 0x00004000
	, eDxIPEPG_UKDTT_RATING_FREEVIEW_MASK		 = 0x0000FF00
	, eDxIPEPG_UKDTT_WARNING_GUIDANCE			 = 0x00100000
	, eDxIPEPG_UKDTT_WARNING_WATERSHED			 = 0x00200000
	, eDxIPEPG_UKDTT_WARNING_MASK				 = 0x00F00000
} DxIPEPG_UKDTT_GUIDANCE_e;

typedef enum
{
	eDxIPEPG_UKDTT_GENRE_MEDIA	= 0x00000001
	, eDxIPEPG_UKDTT_GENRE_FEPG	= 0x00000002
} DxIPEPG_UKDTT_GENRE_e;

typedef struct
{
	HUINT16 				usSize;
	DxIPEPG_UKDTT_TEXT_e	eType;
	HCHAR					ucLang[EPG_HUMAX_LANG_LEN];
	HUINT8					*ucStr;
}DxIpUKDTTEventStr_t;

typedef struct
{
	HUINT16					usSize;
	DxIPEPG_Text_e			eType;
	HCHAR					ucLang[EPG_HUMAX_LANG_LEN];
	HUINT8					*ucStr;
}DxIpEventStr_t;

typedef struct
{
	DxIpEventStr_t			stTarget;
	DxIpEventStr_t			stType;
	DxIpEventStr_t			stUrl;
}DxIpEventMediaUrl_t;

typedef struct
{
	HUINT32					ulUrlSize;
	DxIpEventStr_t			stUrlType;
	DxIpEventStr_t			stImageUrl;
}DxIpEventMediaImage_t;


typedef struct
{
	HUINT32					ulPubDate;
	HUINT32					ulExPireDate;
	HUINT32					ulTransmitDate;
	HUINT32					ulPrice;

	DxIpEventStr_t			stMediaId;			// SBS ondemand
	HINT16					ulMediaUrlCount;
	DxIpEventMediaUrl_t		stMediaUrl[eDxIPEPG_MEDIAURL_MAXCOUNT];
	HINT16					ulMediaImageCount;
	DxIpEventMediaImage_t	stMediaImage[eDxIPEPG_MEDIAIMAGE_MAXCOUNT];
}DxIpEventMedia_t;

typedef struct
{
	HUINT32 				ulProgId;
	HUINT32					ulStartTime;
	HUINT32					ulEndTime;
	HUINT32					ulEpisodeNum;
	HUINT32					ulSeason;
	DxIpEventStr_t			stSubName;
}DxIpEventSeriesLink_t;

typedef struct
{
	HUINT32					ulStartOfAvailability;
	HUINT32					ulEndOfAvailability;
	HUINT32					ulPublishedDuration;
	HUINT8					ucFree;
	HUINT8					ucIsHD;
	HUINT32					ulOnDemandGenre;		// DxIPEPG_UKDTT_GENRE_e

	HINT16					usOnDemandStrCount;
	DxIpUKDTTEventStr_t		stOnDemandStr[eDxIPEPG_FVC_OD_MAXCOUNT];
}DxIpEventFvcOnDemand_t;

typedef struct
{
	DxIpUKDTTEventStr_t		stProgramId;
	HINT16					usCRIDCount;
	DxIpUKDTTEventStr_t		stCRID[eDxIPEPG_CRID_MAXCOUNT];
	HUINT32					ulGuidanceType;		// DxIPEPG_UKDTT_GUIDANCE_e
	HINT16					usGuidanceCount;
	DxIpUKDTTEventStr_t		stGuidance;
	HUINT16					usHDLinkageOnId;
	HUINT16					usHDLinkageTsId;
	HUINT16					usHDLinkageSvcId;
	HUINT16					usHDLinkageEvtId;
	HUINT32					eComponent;			// DxIPEPG_UKDTT_COMPONENT_e
	HINT16					ulOnDemandCount;
	DxIpEventFvcOnDemand_t	stFvcOnDemand;
	HUINT32					ulUkdttReserved;
	HUINT32					ulUkdttReserved2;
}DxIpEventUkdtt_t;

typedef struct
{
	DxIpEventBase_t			stBase;
	HINT16					ulStrCount;
	DxIpEventStr_t 			stText[eDxIPEPG_TEXT_MAXCOUNT];
	HINT16					ulLinkCount;
	DxIpEventSeriesLink_t	stLink[eDxIPEPG_LINK_MAXCOUNT];
	HINT16					ulMediaCount;
	DxIpEventMedia_t		stMedia[eDxIPEPG_MEDIA_MAXCOUNT];

	// 추후 model 마다 추가 해야 하는 것은 무조건 ext로 추가 된다.
	union {
		DxIpEventUkdtt_t	ukdtt;
		HUINT32		 		dummy;
		}ext;

}DxIpEvent_t;

typedef struct
{
	HUINT8					drm;
	HUINT32					seriesId;		// if seriesid/episodeid are same, it is same contents
	HUINT32					episodeNum;		// if seriesid/episodeid are same, it is same contents
	HUINT32					totalNum;		// Total edisode Count
	HUINT32					season;			// season num

	HUINT8					seriesType;		// 0x01 :  using Series ID		0x02 : using Crid
	EVENT_LINK_HUMAX_t		eventLink[EPG_HUMAX_LINK_MAX];

	HUINT8					*actor;
	HUINT8					*actress;
	HUINT8					*director;
	HUINT8					*person;
	HCHAR					*thumbnailURL;
	HxVector_t				*linkList;
	HxVector_t				*mediaList;		// reference to DxEventMedia_t

	// ukdtt
	DxEventOnDemand_t		*onDemand;
	HUINT8					*guidance;
	HUINT32					guidanceType;	// DxIPEPG_UKDTT_GUIDANCE_e
	HUINT32					component;		// DxIPEPG_UKDTT_COMPONENT_e
}Ext_IpEpgInfo_t;

//////////////////////////////////////////////////////////////////////////////////
//						TVTV EPG
//////////////////////////////////////////////////////////////////////////////////
#define	EPG_TVTV_LINK_MAX					(EPG_HUMAX_LINK_MAX)
#define 	EPG_TVTV_MAX_LANG					(EPG_HUMAX_LANG_LEN)

#if defined(CONFIG_MW_EPG_TVTV_NAND_STORAGE)
#define	TVTVEPG_THUMBNAIL_PATH		"/var/lib/humaxtv_user/TVTV/IMAGE"
#else
#define	TVTVEPG_THUMBNAIL_PATH			"/mnt/hd1/TVTV/IMAGE"
#endif

#if defined(CONFIG_IPEPG_EVENT_THUMBNAIL_MOUNT_PATH)
#define IPEPG_EVENT_THUMBNAIL_MOUNT_PATH		CONFIG_IPEPG_EVENT_THUMBNAIL_MOUNT_PATH
#define IPEPG_CHANNEL_LOGO_MOUNT_PATH			CONFIG_IPEPG_EVENT_THUMBNAIL_MOUNT_PATH"/chlogo"
#else
#define IPEPG_EVENT_THUMBNAIL_MOUNT_PATH		"/var/lib/humaxtv_user"
#define IPEPG_CHANNEL_LOGO_MOUNT_PATH			"/var/lib/humaxtv_user/chlogo"
#endif

typedef enum
{
	  eDxTVTVEPG_TEXT_NAME
	, eDxTVTVEPG_TEXT_SHORT
	, eDxTVTVEPG_TEXT_DETAIL
	, eDxTVTVEPG_TEXT_THUMBNAIL
	, eDxTVTVEPG_TEXT_ACTOR
	, eDxTVTVEPG_TEXT_ACTRESS
	, eDxTVTVEPG_TEXT_DIRECTOR
	, eDxTVTVEPG_TEXT_PERSON
	, eDxTVTVEPG_TEXT_MAX
} DxTVTVEPG_Text_e;

typedef struct tagEventLinkTvtv
{
	HUINT8					ucType;
	HUINT32					ulProgId;
}EVENT_LINK_TVTV_t;

typedef struct
{
	HUINT32					usTvtvId;		/**< tvtv id */
	HUINT16					usOnId;		/**< originl network id */
	HUINT16					usTsId;		/**< ts id */
	HUINT16					usSvcId;		/**< service id */
	HUINT32					ulSvcUid;
}DxTvtvService_t;

typedef struct
{
	DxEPG_Type_e			type;			/**< eDxEPG_TYPE_TVTV */
//	HUINT8					ucKind;			/**< ??? */
	HUINT16 					usOnId; 			/**< originl network id */
	HUINT16 					usTsId; 			/**< ts id */
	HUINT16 					usSvcId;			/**< service id */

//-------------------------------------------------------------
//	event info
//-------------------------------------------------------------
	HUINT16					usVer;			/**< export version */
	HUINT16					usTvtvId;
	HUINT32 					ulProgId;			/**< internal unique programme ID */
	HUINT16					usEventId;		/**< EIT event id for futher reference*/
	HUINT32					ulSvcUid;			/**< unique service ID */
	UNIXTIME					ulStartTime;		/**< programme start date */
	UNIXTIME					ulEndTime;		/**< programme end date */
	HUINT8					ucRating;			/**< fsk program guide */

//-------------------------------------------------------------
//	series info
//-------------------------------------------------------------
	HUINT32 					ulSeason;		/**< season number */
	HUINT32 					ulSeriesId;		/**< series unique ID for serial recordings etc */
	HUINT32 					ulTotalNum;		/**< total number of eposides in a season */
	HUINT32 					ulEpisodeNum;	/**< series episode number */
	EVENT_LINK_TVTV_t		astEventLink[EPG_TVTV_LINK_MAX];	// 추후 alloc해서 사용하는 것으로 변경 필요.

//-------------------------------------------------------------
//	additional info
//-------------------------------------------------------------
	HUINT8					aucContent[DxEVENT_CONTENT];	// ucGenre임
	HUINT8					ucTip;			/**< Viewing recommendation */ // ip의 ucRecommendation와 동일한 기능이라 생각됨.. 확인 필요

//-------------------------------------------------------------
//	string info
//-------------------------------------------------------------
	HUINT8					ucLang[EPG_TVTV_MAX_LANG];
}DxTvtvEventBase_t;

typedef struct
{
	HUINT16					usSize;
	HUINT8					*ucStr;
}DxTvtvEventStr_t;

typedef struct
{
	DxTvtvEventBase_t		stBase;
	DxTvtvEventStr_t 			stText[eDxTVTVEPG_TEXT_MAX];
}DxTvtvEvent_t;

typedef struct
{
	HUINT16					ver;
	HUINT16					tvtvId;
	HUINT32					season;
	HUINT32					seriesId;
	HUINT32					totalNum;
	HUINT32					episodeNum;		// if seriesid/episodeid are same, it is same contents

//	HUINT8					seriesType;		// 0x01 :  using Series ID		0x02 : using Crid
	EVENT_LINK_TVTV_t		eventLink[EPG_TVTV_LINK_MAX];

	HUINT8					*actor;
	HUINT8					*actress;
	HUINT8					*director;
	HUINT8					*person;
	HCHAR					*thumbnailURL;
}Ext_TvtvEpgInfo_t;

#ifdef CONFIG_FUNC_EPG_USE_SVCUID
#define	DxEVENT_UID(svcuid, eventId)	(HUID)(((HUID)svcuid) << 32 | eventId)
#define DxEVENT_UID_EVENTID(uid)		(HINT32)(uid & 0xFFFFFFFFULL)
#else
#define DxEVENT_UID_ONID(uid)			(HINT32)((uid >> 48) & 0x0000FFFFULL)
#define DxEVENT_UID_TSID(uid)			(HINT32)((uid >> 32) & 0x0000FFFFULL)
#define DxEVENT_UID_SVCID(uid)			(HINT32)((uid >> 16) & 0x0000FFFFULL)
#endif
typedef struct
{
	DxEPG_Type_e		type;
	HINT32				refCount;
	HUID				uid;
	HINT32				svcuid;
	HINT32				onid;
	HINT32				tsid;
	HINT32				svcid;
	HINT32				eventId;
	HUINT32				startTime;
	HUINT32				duration;
	HUINT8				runStatus;
	HBOOL				freeCaMode;

	HxVector_t			*name;
	HxVector_t			*text;
	HxVector_t			*longText;

	HINT32				parentalRating;
	HUINT16				content[DxEVENT_CONTENT];

	HxVector_t			*item;			///< extended info items : DxEventItem_t *
	HxVector_t			*linkage;		///< .... the others : DxEventLinkage_t *
	HxVector_t			*component; 	///< .... the others : DxEventComponent_t *
	HxVector_t			*crid;			///< .... the others : DxEventCRID_t *
	HxVector_t			*descriptor;	///< raw descriptor




	// TODO: multi-lingual

	// type으로 분류
	union {
		Ext_TvtvEpgInfo_t 	tvtvEpg;
		Ext_IpEpgInfo_t 	ipEpg;
	} extension;

	DxDataCollector_t	*pstCollector;
} DxEvent_t;



HCHAR *			DxEVENT_Encode (HINT32 jsonWriter, const DxEvent_t *event);
DxEvent_t *		DxEVENT_Decode (DxEvent_t *event, HxJSON_t json);
DxEvent_t	*	DxEVENT_Clone (const DxEvent_t *event);
void			DxEVENT_Delete (DxEvent_t *event);
HxFreeFunc_t	DxEVENT_GetFreeFunc (const HCHAR *type);


#ifdef __cplusplus
}
#endif

#endif // DLIB_EPG_H__


