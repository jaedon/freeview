/* $Header: $ */

#ifndef DLIB_SCHEDULE_H__
#define DLIB_SCHEDULE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <hlib.h>

typedef	enum
{
	DxRSVTYPE_NOTHING					= 0x0000,		/**< Nothing. */

	/**@brief Two Way Reservation. - Dest : HDD */
	DxRSVTYPE_WATCHING_EBR				= 0x0001,			/**< Event base watching Rsv. */
	DxRSVTYPE_WATCHING_TBR				= 0x0002,			/**< Time base watching Rsv. */
	DxRSVTYPE_RECORD_EBR				= 0x0003,
	DxRSVTYPE_RECORD_TBR				= 0x0004,

	/**@brief One Way Reservation. */
	DxRSVTYPE_POWER_ON 					= 0x0005,			/**< Power On Rsv. */
	DxRSVTYPE_POWER_OFF 				= 0x0006,			/**< Power Off Rsv. */
	DxRSVTYPE_REGULAR_OTA 				= 0x0007,			/**< Regular OTA Rsv. */
	DxRSVTYPE_EPG_GATHERING 			= 0x0008,			/**< EPG gathering Rsv. */
	DxRSVTYPE_EMM 						= 0x0009,			/**< EMM(CAS) Rsv. */
	DxRSVTYPE_UPDATE_TASK 				= 0x000a,
	DxRSVTYPE_ASO_NOTIFY 				= 0x000b,			/* Analog Switch Off  Rsv */
 	DxRSVTYPE_MYEPG_UPDATE 				= 0x000c,
	DxRSVTYPE_NET_UPDATE 				= 0x000d,	    	/* RSV_TYPE_UPDATE_TASK와 유사하나 network scan만 실행 */

	/**@brief Two Way Reservation.  - Dest : Other Device */
	DxRSVTYPE_BLURAY_TBR 				= 0x000e,
	DxRSVTYPE_BLURAY_EBR 				= 0x000f,

	/**@brief Instant Recording */
	DxRSVTYPE_RECORD_INSTANT			= 0x0010,
	DxRSVTYPE_RECORD_DELAYED 			= 0x0011,

	DxRSVTYPE_KEYWORD_RECORD_NOTSURE	= 0x0012, 			//현재 epg에서 keyword로 검색된거 없지만 만약 있을 경우 찾아서 reserve할수 있는 type

	DxRSVTYPE_CAS_POWER_CTRL			= 0x0013,
	DxRSVTYPE_LOGO_DOWN					= 0x0014,
	DxRSVTYPE_RESCAN					= 0x0015,
	DxRSVTYPE_CAS_TEL					= 0x0016,
	DxRSVTYPE_CHANNEL_MASK				= 0x0017,
	/**@brief Rsv Dubbing */
	DxRSVTYPE_DUBBING					= 0x0018, 	/* 더빙의 특수한 상황으로 인해 추가. 실제 예약에 사용되는것은 아니고 예약과의 충돌 체크를 위해 사용함. 더빙은 1개의 예약과 충돌 하더라도 컨플릿 상황이 되어야 함 */
	DxRSVTYPE_OTA_BD					= 0x0019,			/** BD OTA 예약 : STB예약과는 별개로 추가될 수 있어 따로 정의.*/

	DxRSVTYPE_IP_EPG_GATHERING 			= 0x001a,			/** IP EPG GATHERING **/
	DxRSVTYPE_IP_REMOTE_PROGRAMMING 	= 0x001b, 		/** IP REMOTE PROTRAMMING **/

	/**@brief Two Way Reservation. - Dest : DLNA */
	DxRSVTYPE_DLNA_EBR					= 0x001c,
	DxRSVTYPE_DLNA_TBR					= 0x001d,
	DxRSVTYPE_DLNA_DUBBING				= 0x001e,

	/**
	 * @brief http live stream.
	 * HLS가 시작하는 경우, 충돌 처리 및 ap가 알아야 한다.
	 * 그로 인해 rsv에 들어가게 되었으며, 의미상 instant watch라고 보면 된다.
	 *
	 * 충돌 list 출력시 있어야 하고, 예약 리스트에는 표시되면 안된다. (당연한가?)
	 */
	DxRSVTYPE_NETSTREAMING_LIVE			= 0x0020,
	DxRSVTYPE_NETSTREAMING_FILE			= 0x0021,

	DxRSVTYPE_RECORD_SERIES_EBR			= 0x0022,

	DxRSVTYPE_RECORD_SATRECORD_TBR		= 0x0023,

	DxRSVTYPE_ACTIVATION_TIME			= 0x0024, /* Active standby 를 유지해야 하는 경우 셋팅된 time 에 wake 하기위해 */
	DxRSVTYPE_DESC_REC_STANDBY			= 0x0025, /*Standby에서만 사용해야 하며, Conflict체크를 지원하지 않음*/
	DxRSVTYPE_CIPLUS_OP_TUNE			= 0X0026,


	/**@brief TVTV EPG Timer  */
	DxRSVTYPE_TVTVEPG_TIMER						= 0x0027,

	/**@brief SatRecord check Timer  */
	DxRSVTYPE_CHECK_SATRECORD_TIMER						= 0x0028,

	DxRSVTYPE_WATCHING_SERIES_EBR		= 0x0029,

	/**@brief dynamic update  */
	DxRSVTYPE_DYNAMIC_UPDATE			= 0x0084,

	/* 위로 추가하세요 */
	DxRSVTYPE_EndOfCase					= 0xFFFF
} DxRsvType_e;

typedef enum
{
	DxRSVREADY_0_SEC = 0,
	DxRSVREADY_10_SEC = 10,
	DxRSVREADY_20_SEC = 20,
	DxRSVREADY_30_SEC = 30,
	DxRSVREADY_40_SEC = 40,
	DxRSVREADY_60_SEC = 60,
	DxRSVREADY_180_SEC = 180,
	DxRSVREADY_300_SEC = 300,
	DxRSVREADY_TVA_TRACKING = 900		// DTG test에 의한 값. 5분+10분
} DxRsvReady_e;

typedef enum
{
	DxRSVREPEAT_ONCE						= 0x00,	/**< 특정일 하루. */
	DxRSVREPEAT_DAILY						= 0x01,	/**< 매일. */
	DxRSVREPEAT_WEEKLY						= 0x02,	/**< 매주. */
	DxRSVREPEAT_WEEKDAYS					= 0x03,	/**< 평일. */
	DxRSVREPEAT_WEEKEND						= 0x04,	/**< 주말. */
	DxRSVREPEAT_WITHOUT_SUNDAY				= 0x05,	/**< 월 ~ 토. */
	DxRSVREPEAT_KEYWORD						= 0x06,	/**< keyword가 보이는대로 */
	DxRSVREPEAT_FEWDAY_OF_WEEK				= 0x07,	/**< 월 ~ 일 중 2일 이상 */

	DxRSVREPEAT_EndOfRepeatCase				= 0xFF
} DxRsvRepeat_e;

typedef enum
{
	eDxSCHED_ACTION_NONE,

	eDxSCHED_ACTION_WATCH,
	eDxSCHED_ACTION_RECORD,
	eDxSCHED_ACTION_POWER,
	eDxSCHED_ACTION_CHECK,
	eDxSCHED_ACTION_DLNA,
	eDxSCHED_ACTION_DUBBING,
	eDxSCHED_ACTION_NETSTREAMING_LIVE,
	eDxSCHED_ACTION_NETSTREAMING_FILE,
	eDxSCHED_ACTION_SAT_RECORD,
	eDxSCHED_ACTION_DESCRAMBLE_RECORD,
	eDxSCHED_ACTION_REGULAR_OTA,
	eDxSCHED_ACTION_ASO_NOTIFY,
	eDxSCHED_ACTION_TVTVUPDATE,
	eDxSCHED_ACTION_MAX
} DxSCHED_ACTION_e;


/**
 * @brief Rsv Status
 추가는  RsvStatus_t도 같이 추가해주세요!!
 please add to enum (SvcRsv_Status_e)!!!
 **/
typedef enum
{
	DxRSVSTATUS_WAITING 	= 0,
	DxRSVSTATUS_READY 		= 1,
	DxRSVSTATUS_RUNNING 	= 2,
	DxRSVSTATUS_STOP 		= 3,
	DxRSVSTATUS_CH_CHANGE 	= 4,


	DxRSVSTATUS_MAX		 	= 0xFF
} DxRsvStatus_e;


/*
	기본적으로 svc uid를 보고 판단
*/
typedef enum
{
	DxRSVSTREAM_TYPE_UNKNOWN				= 0x0000,
	DxRSVSTREAM_TYPE_MASK					= 0xFF00,
	DxRSVSTREAM_TYPE_MEDIATYPE_MASK			= 0x00FF,

	/* hls */
	DxRSVSTREAM_TYPE_HLS_MASK				= 0x0100,
	DxRSVSTREAM_TYPE_HLS_LIVE_VIDEO			= 0x0101,
	DxRSVSTREAM_TYPE_HLS_LIVE_AUDIO			= 0x0102,
	DxRSVSTREAM_TYPE_HLS_FILE_MEDIA			= 0x0103,// conflict check 제외
	DxRSVSTREAM_TYPE_HLS_FILE_TSR			= 0x0104,// conflict check 제외
	DxRSVSTREAM_TYPE_HLS_FILE_PVR			= 0x0105,// conflict check 제외
	DxRSVSTREAM_TYPE_HLS_FILE_PVR_AUDIO		= 0x0106,// conflict check 제외
	DxRSVSTREAM_TYPE_HLS_FILE_MUSIC			= 0x0107,// conflict check 제외

	/* sat/ip */
	DxRSVSTREAM_TYPE_SATIP_MASK				= 0x0200, // tuning param을 보고 check해야 함(svc uid가 없음)
	DxRSVSTREAM_TYPE_SATIP_LIVE				= 0x0201,
	DxRSVSTREAM_TYPE_SATIP_LIVE_ALLPID		= 0x0202,
	DxRSVSTREAM_TYPE_SATIP_LIVE_AUTOPID		= 0x0208,

	/* by pass */
	DxRSVSTREAM_TYPE_BYPASS_MASK			= 0x0300,
	DxRSVSTREAM_TYPE_BYPASS_LIVE			= 0x0301,// tuning param을 보고 check해야 함(svc uid가 없음)
	DxRSVSTREAM_TYPE_BYPASS_LIVE_ALLPID		= 0x0302,
	DxRSVSTREAM_TYPE_BYPASS_LIVE_AUTOPID	= 0x0308,

	/* ntls (none transcoding live streaming) */
	DxRSVSTREAM_TYPE_NTLS_MASK				= 0x0400,
	DxRSVSTREAM_TYPE_NTLS_LIVE_VIDEO		= 0x0401,
	DxRSVSTREAM_TYPE_NTLS_LIVE_AUDIO		= 0x0402,
	DxRSVSTREAM_TYPE_NTLS_FILE_MEDIA		= 0x0403,// conflict check 제외
	DxRSVSTREAM_TYPE_NTLS_FILE_TSR			= 0x0404,// conflict check 제외
	DxRSVSTREAM_TYPE_NTLS_FILE_PVR			= 0x0405,// conflict check 제외
	DxRSVSTREAM_TYPE_NTLS_FILE_PVR_AUDIO	= 0x0406,// conflict check 제외
	DxRSVSTREAM_TYPE_NTLS_FILE_MUSIC		= 0x0407,// conflict check 제외

	/* max */
	DxRSVSTREAM_TYPE_MAX					= 0xFFFF
} DxRsvStreamType_e;

typedef enum
{
	DxRsvStream_Update_None					= 0x0000,
	DxRsvStream_Update_Time					= 0x0001,
	DxRsvStream_Update_Pids					= 0x0002,
	DxRsvStream_Update_PlayPosition			= 0x0004,
	DxRsvStream_Update_Priority				= 0x0008,
	DxRsvStream_Update_Transcoding			= 0x0010,

	/* max */
	DxRsvStream_Update_Max
} DxRsvStream_Update_e;

typedef enum
{
	DxRsvStream_AllocMode_None					= 0,
	DxRsvStream_AllocMode_BEG					= 1,
	DxRsvStream_AllocMode_FORCE					= 2,
} DxRsvStream_AllocMode_e;

typedef enum
{
	DxRsvTva_ContentKind_Pending_Rec			= 0x00000001,
	DxRsvTva_ContentKind_Pending_LiveStreaming	= 0x00000002,
	DxRsvTva_ContentKind_SeasonEpisode_Done		= 0x00000004,
	DxRsvTva_ContentKind_Mask 					= 0xFFFFFFFF,
} DxRsvTva_ContentKind_e;

#define	DxRSV_MAX_UUID_BUFSIZE		64
#define	DxRSV_EVENT_NAME_LEN		256
#define	DxRSV_HLS_NAME_LEN			256

#define DxRSV_STREAM_URI_LEN		1024
#define DxRSV_USERAGENT_LEN			64
#define DxRSV_IPADDR_LEN			32
#define DxRSV_STREAMKEY_LEN			256
#define DxRSV_PROFILESTRING_LEN		512
#define DxRSV_PIDTABLE_COUNT		128
#define DxRSV_APPDISPLAYSTRING_LEN	128

#define DxRSV_SLOT_NULL			0xffff



#if	1//defined(CONFIG_OP_FREESAT)
#define	DxRSV_NUM_TO_RECORD_PROG_SAVE					20 	/* save program to record to prevent conflict on making initial series set after booting */
#define	DxRSV_CRID_AUTH_LEN								32
#define	DxRSV_CRID_DATA_LEN								29
#define	DxRSV_CRID_IMI_LEN								 3
#define	DxRSV_CRID_TOTAL_LEN							(DxRSV_CRID_AUTH_LEN + DxRSV_CRID_DATA_LEN + DxRSV_CRID_IMI_LEN + 3)
#define	DxRSV_CRID_PROGLIST_LEN							1024
#define	DxRSV_CRID_LEN									(DxRSV_CRID_AUTH_LEN + DxRSV_CRID_DATA_LEN + DxRSV_CRID_IMI_LEN)
#define	DxRSV_NUM_RECORDED_PROG_SAVE					 5

#define DxRSV_CRID_SERIES_TITLE_LEN						52

typedef	struct
{
	HUINT32			ulMasterSvc;
} DxFreesatReservation_t;

typedef	struct
{
	HUINT8				ucRecKind;						/* CONTENT_KIND_SINGLE, CONTENT_KIND_SERIES, CONTENT_KIND_SPLIT */
	HUINT8				ucCRIDType;
	HCHAR				szCRID[DxRSV_CRID_LEN + 1];
	HCHAR				szFPBRecPath[DxRSV_EVENT_NAME_LEN];
	HCHAR				szRecordedProgCrid[(DxRSV_CRID_LEN + 2) * DxRSV_NUM_RECORDED_PROG_SAVE];
	HCHAR				szEventToRecord[(DxRSV_CRID_LEN + 2) * DxRSV_NUM_TO_RECORD_PROG_SAVE];
	HUINT32				aulEventToRecordInfo[DxRSV_NUM_TO_RECORD_PROG_SAVE][4]; /* (Handle, Start Time, End Time, Event ID) * 20 */
	HUINT8				bRecomRsv;
	HINT32				usLastRecordedEvtId;
} DxRSV_FreesatRecord_t;
#endif

typedef struct{
	HUINT8 		ucCRIDType;
	HCHAR 		szCRID[DxRSV_CRID_LEN +1];
}DxRSV_TvAnyTimeScheduledCRID;



typedef struct{
	HUINT8		ucRecKind;						/* CONTENT_KIND_SINGLE, CONTENT_KIND_SERIES, CONTENT_KIND_SPLIT */
	HUINT8		ucCRIDType;
	HCHAR		szCRID[DxRSV_CRID_LEN + 1];		// Series CRID or Programme CRID
	HCHAR		szProgCRID[DxRSV_CRID_LEN + 1];	// Programme CRID
	HUINT32		ulSeason;
	HUINT32		ulEpisodeNum;
	DxRsvTva_ContentKind_e	eContentKind;
	HUINT8		ulReserved[256];

#if defined(CONFIG_OP_TDC)
	HUINT8		szSeriesTitle[DxRSV_CRID_SERIES_TITLE_LEN];
	HUINT32 	ucSearchFlag;
#endif
}DxRSV_TvAnyTimeRecord_t;


#if defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE) || defined(CONFIG_PROD_HMS1000T)
typedef struct
{
	DxEPG_Type_e	eEpgType;
} DxHmsReservation_t;
#endif

#define	DxRSV_MAX_EVENT_NAME			256
typedef	struct
{
	HINT32	nEvtId;
	HUINT32				ulProgramId;
	HUINT32				ulSeriesId;
	HCHAR				szEvtName[DxRSV_MAX_EVENT_NAME];

	union
	{
		DxRSV_TvAnyTimeRecord_t		tva;
		HUINT32					    dummy;
	} ex;
} DxRSV_Watching_t;

typedef	struct
{
	HUINT8				ucVolume;
	HUINT8				ucInputMode;
	HUINT16				usChNum;
} DxRSV_Power_t;

typedef enum
{
	DxRSVTarget_StorageManual = 0,
	DxRSVTarget_StorageIntHdd = 1,
	DxRSVTarget_StorageIntSubHdd = 2,
	DxRSVTarget_StorageExtHdd = 3
} DxRSVTarget_e;

typedef	 struct
{
	HINT32				nEvtId;
	HUINT32				ulProgramId;
	HUINT32				ulSeriesId;
	HCHAR				szEventName[DxRSV_EVENT_NAME_LEN];

	HUINT32				ulPreOffsetTime;	//	Padding Head
	HUINT32				ulPostOffsetTime;	//	Padding Tail
	DxRSVTarget_e		eStorage;			//	target storage
	HUINT8				aucStorageUUID[DxRSV_MAX_UUID_BUFSIZE];

	// eScheduleSpec 로 구분할 수.
	union
	{
		DxRSV_FreesatRecord_t	    freesat;
		DxRSV_TvAnyTimeRecord_t		tva;
	} ex;
} DxRSV_Record_t;

typedef struct
{
	DxVideoCodec_e			eVideoCodec;
	DxAudioCodec_e			eAudioCodec;

	HINT32					nAudioBitrateKbps;
	HINT32					nVideoBitrateKbps;
	HINT32					nHdVideoBitrateKbps;
	HINT32					nFrameRate;
	HINT32					nScreenWidth;
	HINT32					nScreenHeight;

	HBOOL					bInterlaceMode;

	DxAspectRatio_e			eVideoAspectRatio;

	/*video-codec extension*/
	DxVideoEncodeProfile_e	eVideoCodecProfile;
	DxVideoEncodeLevel_e	eVideoCodecLevel;

	/*need probe-info from live*/
	HBOOL					bUsingLiveAudioCodec;	/* (true : bypass, false : using eAudioCodec) */
	HBOOL					bUsingLiveAspectRatio;	/* (true : bypass, false : using eVideoAspectRatio) */
} DxRsvStream_TranscodingProfile_t;

typedef struct
{
	HUINT16								usOnId;
	HUINT16								usTsId;
	HUINT16								usSvcId;
} DxSchedule_TripleId_t;

typedef struct
{
	HUINT32								ulMsgId;
	UNIXTIME							ulPlayerPosition;
	DxRsvStreamType_e					eStreamType;
	HUINT32								ulSvcUId;
	HUINT32								hAction;
	HUINT32								ulContentUId;
	DxTuneParam_t		 				stTuningParam;
	HBOOL				 				bTranscoding;
	HBOOL								bDrm;
	DxSchedule_TripleId_t				stTripleId;
	DxRsvStream_TranscodingProfile_t 	stProfile;
	HUINT32				 				ulStartTime;
	HBOOL				 				bPcrTsPacketInsert;
	HCHAR				 				szStreamingKey[DxRSV_STREAMKEY_LEN];			// 256
	HCHAR				 				szProfileString[DxRSV_PROFILESTRING_LEN];		// 512
	DxRsvStream_Update_e 				eUpdate;
	HINT32				 				nPIDsCount;
	HUINT16				 				usPIDs[DxRSV_PIDTABLE_COUNT];					// 128
	HCHAR				 				szAppDisplayString[DxRSV_APPDISPLAYSTRING_LEN]; // 128
	DxRsvStream_AllocMode_e				eAllocMode;
	HUINT16								usPriority;
} DxRSV_Streaming_t;

typedef	union
{
	DxRSV_Watching_t		stWch;
	DxRSV_Power_t			stPwr;
	DxRSV_Record_t			stRec;
	DxRSV_Streaming_t		stStrm;
} DxExtReservation_u;

typedef enum
{
	eDxSchedule_Spec_None			= 0,

	eDxSchedule_Spec_Base			= 0x0001,
	eDxSchedule_Spec_Freesat		= 0x0002,
    eDxSchedule_Spec_TvAnyTime      = 0x0004,
	eDxSchedule_Spec_EndOfCase
} DxSchedule_Spec_e;

typedef enum
{
	DxSchedule_DetailInfo_None = 0,
	DxSchedule_DetailInfo_SatIp_NoTuner,
	DxSchedule_DetailInfo_SatIp_ForcePossible,
	DxSchedule_DetailInfo_SatIp_NoTransCoder,
	DxSchedule_DetailInfo_SatIp_NoLNB,
} DxSchedule_DetailInfo;

typedef enum
{
	DxSchedule_RemovedReason_None		= 0,

	DxSchedule_RemovedReason_ServiceDeleted,
	DxSchedule_RemovedReason_Canceled,
	DxSchedule_RemovedReason_Completed,
	DxSchedule_RemovedReason_Stopped,
	DxSchedule_RemovedReason_Expired,
	DxSchedule_RemovedReason_NoPvrDevice,
	DxSchedule_RemovedReason_CasCopyrightControlFail,
	DxSchedule_RemovedReason_SeriesTracking,
	DxSchedule_RemovedReason_HddFull,
	DxSchedule_RemovedReason_SameSeriesRecItemExist,
	DxSchedule_RemovedReason_PendingBroken,
	DxSchedule_RemovedReason_SatipFailed,
	DxSchedule_RemovedReason_UnableToTrack,
	DxSchedule_RemovedReason_MaxRecList,

	DxSchedule_RemovedReason_Unknown,

	DxSchedule_RemovedReason_EndofCase

} DxSchedule_RemovedReason_e;

typedef struct
{
	DxSchedule_RemovedReason_e	eRemovedReason;
	HUINT32						eDetailInfo;
}DxSchedule_RemovedInfo_t;

typedef struct
{
	UNIXTIME				utOverlapTimeRangeStart; // padding 고려한 시간.
	UNIXTIME				utOverlapTimeRangeEnd; // padding 고려한 시간.
} DxSchedule_FindFilterAttr_OverlapTimeRange_t;

typedef struct
{
	DxRsvType_e				eRsvType;
	HUINT32					ulSvcUid;
	DxSchedule_TripleId_t 	stTripleId;
	DxRsvStatus_e			eStatus;
	DxEPG_Type_e			eEpgType;
	UNIXTIME				utStartTime; // padding 고려한 시간.
	UNIXTIME				utEndTime; // padding 고려한 시간.
	HBOOL					bRemoved;
	DxSchedule_FindFilterAttr_OverlapTimeRange_t stOverlapTimeRange;
} DxSchedule_FindFilterAttr_t;

typedef struct
{
	HUINT8			aucCamId[8];
} DxCiPlusScheduleData_t;

typedef struct
{
	DxCiPlusScheduleData_t	ciplus;
} DxCasScheduleData_t;

typedef	struct
{
	HUINT32				ulSlot;
	DxRsvType_e			eRsvType;
	HUINT32				ulSvcUid;
	DxSchedule_TripleId_t stTripleId;
	DxRsvReady_e		eRsvReady;
	HxDATETIME_t		stStartTime;
	HUINT32				ulDuration;
	DxRsvRepeat_e		eRepeat;
	HUINT32				ulRepeatDays;
	DxRsvStatus_e		eStatus;
	HBOOL				bExceptConflict;
	DxEPG_Type_e		eEpgType;

	DxSchedule_Spec_e	eScheduleSpec; // ex와 uExtInfo.stRec 의 config 풀기 위함. (즉 어느 item 을 사용하고 있는지 알기 위함.)

	HBOOL				bRemoved;

	DxSchedule_RemovedInfo_t stRemovedInfo;

	DxExtReservation_u	uExtInfo;

	// TODO: Schedule에 CAS 관련 정보를 담기 위한 union 구성
//	union
//	{
//		DxCasScheduleData_t	cas;
//		HUINT8				reserved[512];
//	} ucas;

	HUINT8	aucCamId[8];

	// eScheduleSpec 로 구분할 수.
	union {
		DxFreesatReservation_t		freesat;
	} ex;

	/*
		Volitile  Items:
			Schedule 내에서 DAMA를 통해 저장되지 않는 값들.
	*/
	HBOOL				 bPaused;

	// removing status (between remove request and removed response)
	HBOOL				 bRemoving;
} DxSchedule_t;





#if 1 // from oapi_types.h 정리 필요.
/* start 	Schedule */
#include "dlib_recording.h" // should be removed.



typedef enum
{
	eDxSCHED_CYCLE_NONE,

	eDxSCHED_CYCLE_DAYLY,
	eDxSCHED_CYCLE_WEEKLY,
	eDxSCHED_CYCLE_MONTHLY,
	eDxSCHED_CYCLE_YEARLY,

	eDxSCHED_CYCLE_MAX
} DxSCHED_CYCLE_e;

typedef enum
{
	eDxSCHED_DATE_SUN = (1 << 0),	// 1		0x0001
	eDxSCHED_DATE_MON = (1 << 1),	// 2		0x0010
	eDxSCHED_DATE_TUE = (1 << 2),	// 4		0x0100
	eDxSCHED_DATE_WED = (1 << 3),	// 8		0x1000
	eDxSCHED_DATE_THU = (1 << 4),	// 16		0x   1 0000
	eDxSCHED_DATE_FRI = (1 << 5),	// 32		0x  10 0000
	eDxSCHED_DATE_SAT = (1 << 6),	// 64		0x 100 0000

	eDxSCHED_DATE_WEEKEND  = (eDxSCHED_DATE_SAT | eDxSCHED_DATE_SUN),
	eDxSCHED_DATE_WEEKDAYS = (eDxSCHED_DATE_MON | eDxSCHED_DATE_TUE | eDxSCHED_DATE_WED | eDxSCHED_DATE_THU | eDxSCHED_DATE_FRI),

	eDxSCHED_DATE_NONE = 0,
	eDxSCHED_DATE_ALL  = (eDxSCHED_DATE_WEEKEND | eDxSCHED_DATE_WEEKDAYS)
} DxSCHED_DATE_e;

typedef enum
{
	eDxSCHED_CHECK_RP,
	eDxSCHED_CHECK_IPEPG,
	eDxSCHED_CHECK_EMM,
	eDxSCHED_CHECK_TVTVEPG,
	eDxSCHED_CHECK_CI_OP,
	eDxSCHED_CHECK_OTA,
	eDxSCHED_CHECK_SATRECORD,
	eDxSCHED_CHECK_NONE,
} DxSCHED_CHECK_e;

typedef struct
{
	DxSCHED_CYCLE_e	cycle;
	HUINT32			date;
} DxSCHED_REPEAT_t;

typedef struct
{
	DxSCHED_CHECK_e	type;
} DxSCHED_CHECK_t;
typedef struct
{
	HINT32		volume;
	HBOOL		onOff;
	HINT32 		lcn;
} DxSCHED_POWER_t;

typedef struct
{
	HINT32		programId;
	HINT32		seriesId;

	HBOOL		seriesWatch;
	union{
		DxRSV_TvAnyTimeRecord_t		tva;
	}ex;
} DxSCHED_WATCH_t;

typedef struct
{
	HINT32		programId;
	HCHAR		seriesId[DxPVR_TVANYTIME_CRID_TOTAL_LEN];
	HBOOL		seriesRec;
	HUINT32		startPadding;
	HUINT32		endPadding;
	HUINT32		episodeNum;
	HUINT16		season;

	union{
		DxRSV_TvAnyTimeRecord_t		tva;
	}ex;
} DxSCHED_REC_t;

typedef struct
{
	HUINT32 			 ulMsgId;
	UNIXTIME			 ulPlayerPosition;
	DxRsvStreamType_e	 eStreamType;
	HUINT32 			 ulSvcUId;
	HUINT32 			 ulContentUId;
	DxTuneParam_t		 stTuningParam;
	HBOOL				 bTranscoding;
	HBOOL				 bPcrTsPacketInsert;
	HCHAR				 szStreamingKey[DxRSV_STREAMKEY_LEN];
	HCHAR				 szProfileString[DxRSV_PROFILESTRING_LEN];
	DxRsvStream_Update_e eUpdate;
	HINT32				 nPIDsCount;
	HUINT16 			 usPIDs[DxRSV_PIDTABLE_COUNT];
	HCHAR				 szAppDisplayString[DxRSV_APPDISPLAYSTRING_LEN];
} DxSCHED_STREAMING_t;

typedef struct
{
	HBOOL				bUpdate;
} DxSCHED_TVTVEPG_t;

typedef union
{
	DxSCHED_CHECK_t	check;
	DxSCHED_POWER_t	power;
	DxSCHED_WATCH_t	watch;
	DxSCHED_REC_t	record;
	DxSCHED_STREAMING_t	streaming;
} DxSCHED_PARAM_t;



typedef struct
{
	HINT32			uid;		// same as schedId -> will be merged
	HINT32			schedId;

	HBOOL			eventBase;
	HINT32			eventId;

	HUINT32			startTime;
	HUINT32			duration;
	HINT32			svcUId;
	DxSchedule_TripleId_t stTripleId;
	HUINT32			preStart;
	HINT32			groupIndex;
	HINT32			recMode;
	HBOOL			fromDap;
	HBOOL			freeCaMode;
	HINT32			Rating;
	DxEPG_Type_e	epgType;
	DxSCHED_ACTION_e	action;
	DxSCHED_REPEAT_t	repeat;

	DxSCHED_PARAM_t	param;
	DxSchedule_Spec_e	eScheduleSpec; // ex와 uExtInfo.stRec 의 config 풀기 위함. (즉 어느 item 을 사용하고 있는지 알기 위함.)
    HUINT32                 __CRC__;  //CHECK CRC
	HCHAR			*name;
	HCHAR			*description;
	HCHAR			*longDescription;
	HCHAR			*storageID;
	HCHAR			*userID;
	HCHAR			*deviceHandle;
	HCHAR			*destinationId;
	HCHAR			*dubbingId;
	DxRsvStatus_e	eStatus;
	HBOOL			bRemoved;
	DxSchedule_RemovedInfo_t stRemovedInfo;
} DxSCHEDULE_t;


typedef struct
{
	HINT32			uid;		// same as schedId -> will be merged
	HINT32			schedId;

	HBOOL			eventBase;
	HINT32			eventId;

	HUINT32			startTime;
	HUINT32			duration;
	HINT32			svcUId;
	HUINT32			preStart;
	HINT32			groupIndex;
	HINT32			recMode;
	HBOOL			fromDap;
	HBOOL			freeCaMode;
	HINT32			Rating;
	DxEPG_Type_e	epgType;

	DxSCHED_ACTION_e	action;
	DxSCHED_REPEAT_t	repeat;

	DxSCHED_PARAM_t	param;
    HUINT32                 __CRC__;  //CHECK CRC
} DxSCHEDULE_ExcepPointers_t;






/* end 		Schedule */
#endif


#ifdef __cplusplus
}
#endif

#endif // DLIB_SERVICE_H__


