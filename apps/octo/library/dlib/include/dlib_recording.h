/* $Header: $ */

#ifndef DLIB_RECORDING_H__
#define DLIB_RECORDING_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <hlib.h>
#include "dlib_copyrightcontrol.h"

#define	DxREC_ENCRYPT_HJM
//#define	DxREC_ENCRYPT_HJM_WITH_CRC


#define DxRECLIST_NAME_LEN	128
#define DxRECLIST_URL		512
#define DxRECLIST_PATH		256
#define DxRECLIST_ITEM		128
#define DxEXT_META_FILE		".hjm"
#define DxRECLIST_ONE_SCAN_UNIT_NUM			32
#define DxRECLIST_MAX_UPDATE_CMD   			5
#define DxRECLIST_MAX_UPDATE_TAG			32
#define DxRECLIST_LOCK_TAG					"LOCK"
#define DxRECLIST_KEEP_TAG					"KEEP"
#define DxRECLIST_KEEP_TIME_TAG				"KEEP_TIME"
#define DxRECLIST_BOOKMARK_NAME_LEN			32
#define DxRECLIST_NUM_BOOKMARK				32
#define DxRECLIST_EXT_EVT_MAX				16
#if defined(CONFIG_OP_UK_DTT)
/* UK D-BOOK isn't required "Extended event descriptor" */
#define DxRECLIST_EXT_TEXT_BUFSIZE			(1 * DxRECLIST_EXT_EVT_MAX)
#else
#define DxRECLIST_EXT_TEXT_BUFSIZE			(256 * DxRECLIST_EXT_EVT_MAX)
#endif

#define DxPVR_PARENTAL_RATING_MAX			3
#define DxPVR_EXT_ITEM_MAX					15

#define DxPVR_EVENT_TEXT					256
#define DxPVR_SHORT_EVENT_NAME_LEN			256
#define DxPVR_MAX_SERVICE_NAME_LEN			128
#define DxPVR_LANG_ISO_639_LEN				3
#define DxFREESAT_PVR_GUIDANCE_DATA_MAX		255
#define DxPVR_TVANYTIME_CRID_AUTH_LEN		32
#define DxPVR_TVANYTIME_CRID_DATA_LEN		29
#define DxPVR_TVANYTIME_CRID_IMI_LEN		3
#define DxPVR_TVANYTIME_CRID_TOTAL_LEN		(DxPVR_TVANYTIME_CRID_AUTH_LEN+DxPVR_TVANYTIME_CRID_DATA_LEN+DxPVR_TVANYTIME_CRID_IMI_LEN +3)
#define DxEPG_COMPONENT_DESC_MAX			10


#define DxRECLIST_STORAGE_ID_LEN				36

#define DxRECLIST_USERID_LEN	288
#define DxRECLIST_CAMID_LEN		8

#define DxUKDTT_PVR_AVAILABLE_CONTENT_COPY_NUMBER			1		/* for copy-once content */

#define _________enum_PVR_________________________________________________________


typedef enum {
	eDxREC_STS_RECORD_START = 0,
	eDxREC_STS_RECORDING,
	eDxREC_STS_RECORDED,
	eDxREC_STS_INCOMPLETED
}DxRecStatus_e;

typedef enum {
	eDxREC_FAILED_NOFAIL		= 0,
	eDxREC_FAILED_HDDFULL,
	eDxREC_FAILED_MAXNUM,
	eDxREC_FAILED_MAXRECORDLISTNUM,
	eDxREC_FAILED_LESS30SEC,
	eDxREC_FAILED_NOSIGNAL,
	eDxREC_FAILED_NONSERVICE,
	eDxREC_FAILED_SEARCHING,
	eDxREC_FAILED_CONFLICT_TP,
	eDxREC_FAILED_DRM,
	eDxREC_FAILED_NOSC_SCR,
	eDxREC_FAILED_NORIGHT_SCR,
	eDxREC_FAILED_NOHDD,
	eDxREC_INCOMPLETE_HDDFULL,
	eDxREC_FAILED_UNKNOWN,
	eDxREC_FAILED_NOUSBHDD,
	eDxREC_FAILED_DISCONNECTUSBHDD,
	eDxREC_FAILED_EXTUSB_CopyDel,
	eDxREC_FAILED_SAME_SVC_RECORDING,
	eDxREC_FAILED_HIGHER_PRIORITY,
	eDxREC_FAILED_UNABLE_TRACKING,
	eDxREC_FAILED_NO_VIDEO_AUDIO,
	eDxREC_FAILED_SI_TIMEOUT,
	eDxREC_FAILED_POWER_OFF,
}DxRecEndReasonType_e;


typedef struct
{
	HCHAR	aucDescription[DxPVR_EVENT_TEXT+1];	// ext evt description
	HCHAR	aucItem[DxPVR_EVENT_TEXT+1]; 		// ext evt item
} DxRecList_Ext_ItemData_t;


typedef	struct
{
	HUINT8		ucDesNum;
	HUINT8		ucLastDesNum;
	HCHAR		szIso639LangCode[DxPVR_LANG_ISO_639_LEN+1];
	HUINT8		ucItemNum;
	DxRecList_Ext_ItemData_t	astItem[DxPVR_EXT_ITEM_MAX];
} DxRecList_ExtEvt_t;

typedef	struct
{
	HUINT32 ulPos;
	HCHAR 	szName[DxRECLIST_BOOKMARK_NAME_LEN+1];
}DxRecList_Bookmark_t;


/* @brief PMT -> ISO 639 language descriptor (Audio Type) */
typedef enum
{
	eDxAUDIO_TYPE_UNDEFINED			= 0,		/**< Normal Audio. */
	eDxAUDIO_TYPE_CLEAN_EFFECTS		= 1,
	eDxAUDIO_TYPE_HEARING_IMPAIRED	= 2,
	eDxAUDIO_TYPE_VISUAL_IMPAIRED	= 3,	/**< AD(Audio Description). */
	eDxAUDIO_TYPE_RESERVED			= 4
} DxAUDIO_Type_e;


#define DxPVR_MAX_AUDIO_ES_NUM				16
#define DxPVR_MAX_SUBTTL_ES_NUM				12

#define DxMAX_MWPVR_AUD_NUM 4
typedef struct tagDxMWPVR_639_LAN_INFO
{
	DxAUDIO_Type_e	eAudType;
	HUINT8			szIso639LangCode[DxPVR_LANG_ISO_639_LEN+1];
} DxMwPvrIso639LangInfo_t;


/*typedef enum
{
	eDxPVR_AUD_CPNT_SINGLE_MONO = 0,
	eDxPVR_AUD_CPNT_DUAL_MONO,
	eDxPVR_AUD_CPNT_STEREO,
	eDxPVR_AUD_CPNT_MULTILINGUAL,
	eDxPVR_AUD_CPNT_SURROUND,
	eDxPVR_AUD_CPNT_DOLBY,
} DxPVR_AUDIO_CPNT_e;//*/



typedef struct tagDxMETA_PVR_AUDIO_INFO
{
	HUINT32					ulLangNum;
	DxMwPvrIso639LangInfo_t	stLangInfo[DxMAX_MWPVR_AUD_NUM];
} DxMETA_PvrAudioInfo_t;


/** @brief Subtitle infomation in PMT */
typedef struct tagDxMETA_PVR_AUDIO_INFO_LIST
{
	HUINT32     			ulItemNum;
	DxMETA_PvrAudioInfo_t	astAudioInfo[DxPVR_MAX_AUDIO_ES_NUM];
} DxMETA_PvrAudioInfoList_t;


typedef struct tagDxMETA_PVR_SBTL_INFO
{
	HUINT8				ucComponentType;		/** < Teletext: 0x00~0x05, Subtitle: 0x10~0x13, 0x20~0x23 */
	HUINT8				aucLangCode [DxPVR_LANG_ISO_639_LEN+1];
} DxMETA_PvrSbtlInfo_t;


/** @brief Subtitle infomation in PMT */
typedef struct tagDxMETA_PVR_SUBTTL_INFO_LIST
{
	HUINT32     			ulItemNum;				/**< Subtitle ���� */
	DxMETA_PvrSbtlInfo_t	astSbtlInfo[DxPVR_MAX_SUBTTL_ES_NUM];
} DxMETA_PvrSbtlInfoList_t;


typedef struct	tagDxPvrParentalRating
{
	HUINT8					aucIsoCountryCode[DxPVR_LANG_ISO_639_LEN+1];
	HUINT8					ucParentalRating;
}DxPvr_ParentalRating_t;

typedef struct	tagDxPvrEvtComponent
{
	HUINT8					ucStreamContent;
	HUINT8					ucComponentType;
	HUINT8					aucIsoCountryCode[DxPVR_LANG_ISO_639_LEN + 1];
}DxPvr_EvtComponent_t;


typedef	struct
{
	UNIXTIME				ulStartTime;
	UNIXTIME				ulDuration;
	HUINT8					ucPRNum;
	DxPvr_ParentalRating_t	astParentalRating[DxPVR_PARENTAL_RATING_MAX];

	HUINT8		ucContentNibble;				// genre
	HUINT8		ucUserNibble;					// genre

	HUINT8		ucRunStatus;
	HUINT8		ucFreeCaMode;
	HCHAR		aucEventName[DxPVR_SHORT_EVENT_NAME_LEN+1];
	HCHAR		aucText[DxPVR_EVENT_TEXT+1];	// description
	HCHAR		aucSeriesId[DxPVR_TVANYTIME_CRID_TOTAL_LEN];

	HUINT32 	ulEpisodeNum;
	HUINT16 	ulSeason;
	DxPvr_EvtComponent_t stComponent[DxEPG_COMPONENT_DESC_MAX];
} DxRecList_EVTData_t;



typedef enum
{
	eDxFSat_GUIDANCE_TYPE_NONE	= 0,
	eDxFSat_GUIDANCE_TYPE_MAX	,
} DxFreesat_PVR_GuidanceType_e;


typedef struct	tagDxPvrFreesatGuidance
{
	DxFreesat_PVR_GuidanceType_e	eType;
	HUINT8					stLangCode[DxPVR_LANG_ISO_639_LEN+1];
	HUINT8					stData[DxFREESAT_PVR_GUIDANCE_DATA_MAX+1];
}DxFreesat_Pvr_Guidance_t;

typedef struct tagDxFsatFtaCntMgmt
{
	HUINT8		ucDoNotScramble;
	HUINT8		ucCtrlRAccOverINet;
	HUINT8		ucDoNotApplyRevocation;
} DxFreesat_PVR_FtaCntMgmt_t;

typedef struct
{
	DxFreesat_Pvr_Guidance_t	astGuidance;
	DxFreesat_PVR_FtaCntMgmt_t	stFtaCntMgmt;
} DxFreesat_RecList_EVTData_t;

typedef struct
{
	HUINT32					ulOrgDuration;	//	HUINT32		ulRecOrgDuration;
	HUINT32						ulFtaCmVal;
	HUINT8						ucCopyTime;
	HUINT32						ulTimeOffsetAtRec;

	HUINT32				nMasterChNumber;
	HUINT8				aucMasterServiceName[DxPVR_MAX_SERVICE_NAME_LEN+1];
	HUINT16				usMasterSvcId;
	HUINT16				usMasterTsId;
	HUINT16				usMasterOnId;
	DxFreesat_RecList_EVTData_t	stEvt;
} DxFreesatRecListData_t;

typedef enum
{
	eDxUkdtt_GUIDANCE_TYPE_LANGCODE	= 0,
	eDxUkdtt_GUIDANCE_TYPE_MODE_SPECIFIED,
	eDxUkdtt_GUIDANCE_TYPE_DATA,
	eDxUkdtt_GUIDANCE_TYPE_MAX
} DxUkdtt_PVR_GuidanceType_e;

typedef struct	tagDxPvrFreeviewGuidance
{
	DxUkdtt_PVR_GuidanceType_e	eType;							/* if 0, lang_code, and data are valid */
																/* if 1, mode, lang_code, and data are valid */
																/* if over 2, only data is valid for future use */
	HUINT8					ucMode;
	HUINT8					stLangCode[DxPVR_LANG_ISO_639_LEN+1];
	HUINT8					stData[DxMAX_GUIDANCE_DATA_LEN+1];
}DxUkdtt_Pvr_Guidance_t;

typedef struct	tagDxPvrFreeviewFtaCntMgmt
{
	HUINT8		ucDoNotScramble;
	HUINT8		ucCtrlRAccOverINet;
	HUINT8		ucDoNotApplyRevocation;
}DxUkdtt_Pvr_FtaCntMgmt_t;
typedef struct
{
	DxUkdtt_Pvr_Guidance_t		stGuidance;
	DxUkdtt_Pvr_FtaCntMgmt_t	stFtaCntMgmt;
} DxUkdtt_RecList_EVTData_t;

typedef enum
{
	eDxUkdtt_CONTENT_MGMT_STATE_FreeAcc					= 0,	/* encrypt on, move & copy free */
	eDxUkdtt_CONTENT_MGMT_STATE_FreeAcc_NoEnc,					/* encrypt off, move & copy free */
	eDxUkdtt_CONTENT_MGMT_STATE_ManagedCopy,					/* encrypt on, 1 copy permitted */
	eDxUkdtt_CONTENT_MGMT_STATE_Reserved,						/* encrypt on, no HD copies */
	eDxUkdtt_CONTENT_MGMT_STATE_None,							/* NO FTA state specified */
} DxUkdtt_PVR_FtaContentMgmtState_e;

typedef struct
{
	HUINT32						ulFtaCmVal;						/* DxUkdtt_PVR_FtaContentMgmtState_e */
	HUINT8						ucCopyTime;
	HUINT32						ulTimeOffsetAtRec;
	DxUkdtt_RecList_EVTData_t	stEvt;
} DxUkdttRecListData_t;

typedef struct
{
	HBOOL			bCamNeedContent;
	HUINT8			aucBindedCamId[DxRECLIST_CAMID_LEN];
} DxCiPlusRecListData_t;

typedef struct
{
	HUINT32					ulInstType;
	HUINT32					ulSlotId;

	DxCiPlusRecListData_t	ciplus;
} DxCasRecListData_t;

typedef struct
{
	HCHAR		szConaxMetaName[DxRECLIST_PATH+1];
} DxConaxRecListData_t;

#define DxMAX_CRID_DATA_NUM		5
#define DxMAX_SERIES_TITLE_LEN	52

typedef struct tagDxTvaCrid
{
	HUINT8		ucType;
	HUINT8		szCrid[DxPVR_TVANYTIME_CRID_TOTAL_LEN];
} DxPVR_TvaCrid_t;





#if defined(CONFIG_PROD_HMS1000S) // ���� ph1�� ����� ���� �����ִ� code��.
typedef enum
{
	eDxDRM_TYPE_NONE,
	eDxDRM_TYPE_V1,
	eDxDRM_TYPE_CIPLUS,
	eDxDRM_TYPE_ARIB_CPINFO,
	eDxDRM_TYPE_MAX
} DxDRM_Type_e;

// ���� DRM Info
// DRM ���� �޶��� ������ ���⿡�� �߰� �ʿ�
typedef struct
{
	HUINT32	 		ulDrmType;
	HUINT8			aucReserved[60];  // ���� ������ �ʿ��� ������ ȣȯ�� ���ؼ� �ش� �ڷᱸ����  �ִ밪(60 byte)�� ������
} DxDrmInfo_t;
#endif


typedef struct
{
	HCHAR		szPath[DxRECLIST_PATH+1];
	HCHAR		szFileName[DxRECLIST_PATH+1];

	UNIXTIME		ulEndTime;
	HCHAR		aucDisplayEvtName[DxRECLIST_NAME_LEN+1];

	HUINT16 			usSvcId;		   /**< ��ġ�� �Ҵ��. ���� �Ұ�. service id */
	HUINT16 			usTsId; 		   /**< ��ġ�� �Ҵ��. ���� �Ұ�. transport stream id */
	HUINT16 			usOnId; 		   /**< ��ġ�� �Ҵ��. ���� �Ұ�. original network id */

	HUINT16				usPmtPid;				/**< pmt pid, pat�� ���ǵ� */
	HUINT16				usPcrPid;				/**< pcr pid from pmt */
	HUINT16				usTtxPid;				/**< teletext pid from pmt */
	HUINT16				usCaptionPid;			/**< caption pid from pmt */

	HUINT16				usVideoPid;				/**< video pid from pmt */
	HUINT32				eVideoCodec;			/**< �ش� es�� video codec, db file�� 1 byte�� ����� */

	HUINT16				usAudioPid;				/**< audio pid from pmt */
	HUINT32				eAudioCodec;			/**< �ش� es�� audio codec, db file�� 1 byte�� ����� */

	HUINT16				usAudioAuxPid;			/**< for audio descriptor or dolby */
	HUINT32				eAudioAuxCodec;			/**< �ش� es�� audio codec, db file�� 1 byte�� ����� */

	HUINT16				usDolbyPid;				/**< dolby audio pid from pmt */
	HUINT32				eDolbyCodec;			/**< �ش� es�� audio codec, db file�� 1 byte�� ����� */
	HBOOL				bDolbyFlag;				/**< main audio�� dolby�� ��� TRUE�� ���� */
												/*----------------------------------------------------------------------
													bDolbyFlag�� TRUE�� �Ǵ� ����,
													(1) sound track â���� dolby audio�� �����ϰų�,
													(2) menu�� dolby priority�� �����ǰ�, dolby audio�� ������ ����
														-> MW SI���� �Ǵ��Ͽ� ������//*/

	HUINT8				ucSubttlIdx;			/**< user�� ���� ���� �� Subtitle Index */
	HBOOL				bSubttlUserFlag;		/**< user�� ���� ���� �Ǿ������� ��Ÿ���� Flag */
	HUINT32				eSoundMode; 			/**< sound mode : STEREO, MONO L, MONO R, MONO L/R */
	HUINT32				eDualMono;				/* Stereo�� �ƴ� Dualmono���� ����� �ʿ������ �� �ʳ� Content�� DualMono ������ ���� �� ��� �ʿ��ؼ� �߰��� */
	HBOOL				bAudUserSetFlag;		/**< sound track menu���� user�� �ٸ� �� dolby audio ���� �����ߴ��� ���ߴ� ���� ���θ� ��Ÿ�� */

	HUINT16				usTrRecState;			// 0: not started, 1: started, 2: finished (TR�߿� HDD ���� Ȥ�� ���� off�� �Ǵ��ϱ����� field)

#if defined(CONFIG_PROD_HMS1000S)
// ���� ph1�� ����� ���� �����ִ� code��. �Ʒ� CopyrightControl_t�� ����Ͽ��� ��. �ܼ� convert���� ���� �����ϹǷ�, ������ stCcInfo�� ����ؾ���.
// �� ���� hms ph1 ���� stDrm�� �ִ� ������ stCcInfo�� convert��.
	DxDrmInfo_t			stDrm;
	HUINT32 				ulDrmCrc;
#endif

	DxCopyrightControl_t		stCcInfo;
	HUINT32 				ulCcInfoCrc;

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	DxPVR_TvaCrid_t		astTvaCrids[DxMAX_CRID_DATA_NUM];

#if defined(CONFIG_OP_TDC)
	HUINT8				szSeriesTitle[DxMAX_SERIES_TITLE_LEN];
#endif

#endif
} DxRecListData_Meta_t;		// Octo PVR Meta information


typedef	struct
{
	// NOTE: ���⿡ field �߰��� local_RECGetCRC()���� �߰��� �־�� ��.

	/////////////////////////////////////////////////
	/* ------------------- �⺻ ���� ------------------ */
	/////////////////////////////////////////////////
	HUINT32		ulContentId;
	HBOOL		bNotified;
	HCHAR		szUrl[DxRECLIST_URL+1];
	HCHAR		szName[DxRECLIST_NAME_LEN+1];
	HCHAR		szThumbnailPath[DxRECLIST_URL+1];

	HUINT32		ulRecStartTime;
	HUINT32		ulRecDuration;
	HUINT32		ulRecOrgDuration;

	HUINT32		ulType;

	DxRecStatus_e			eRecStatus;
	DxRecEndReasonType_e	eRecEndReason;
	HBOOL		bScrambled;
	HBOOL		bEncrypted;
	HBOOL		bEventBased;
	HINT32		nEventId;
	HBOOL		bSeriesRec;

	HUINT32		ulLastViewTime;
	HBOOL		bUserLock;
	HBOOL		bWillBeDeleted;
	HBOOL		bAudoDeleted;
	HBOOL		bPlayed;
	HBOOL		bBlocked;	// svc lock or pin lock or pr lock ...

	HBOOL		bDoNotDelete;
	HUINT32		ulDoNotDeleteSetTime;

	/* szStorageId : ��ȭ�� HDD�� �ĺ��ϴ� StorageId
	// ���� ���:  JCOM(�Ϻ�) ��翡���� �� ���� webapp�ʿ� discInfo class�� discId��� property�� �÷��־� recording ���� �����Ҷ� ��ȭ�� storage�� �����ϱ����� ���ȴ�.
	//                 ���� �ٸ� ��翡���� �� ���� �ʿ��� ������ ����Ǿ� ���� config ġ�� �ʴ´�.
	// �߰� ����: pvr meta file(.hjm)���� �� ���� ������ ���� ������, API�� �ξ� path�� ������ StorageId�� ��ȯ �ϵ��� �Ѵ�.
	//                 JCOM������ �� ���� storage��  HDD�� UUID�� �����ϵ��� �Ǿ��ִ�. ��� ���� �����Ǵ� ������ �޶��� �� ������ �� API���� ��纰�� ��ȯ�Ǵ� ���� �޸��ϵ��� �����ϱ⸦ �����Ѵ�.
	*/
	HCHAR		szStorageId[DxRECLIST_STORAGE_ID_LEN+1];
	HUINT32		ulChipId;

	HUINT16		usNumOfBookmark;
	HUINT16		usNumOfPausedPoints;
	HCHAR				szSvcName[DxRECLIST_NAME_LEN+1];
	HINT32				nSvcNo;
	HINT32				svcUId;
	DxSvcType_e			eSvcType;
	DxVideoType_e		eVideoType;
	DxCasType_e			eCasType;

    	DxMETA_PvrAudioInfoList_t	stAudioInfoList;
	DxMETA_PvrSbtlInfoList_t	stSbtlInfoList;

	// Transcode
	HBOOL		bTranscoded;
	HUINT32 	ulRecMode;			 // 1 : DR,  2 : 2��,	3 : 3��, 5 : 5��,	7 : 7��

	// Information in the octo meta
	DxRecListData_Meta_t	stMeta;

	// Event Information
	DxRecList_EVTData_t 	stEvt;
	HUINT32					ulExtEvtCnt;

	HUINT32                 __CRC__;  //CHECK CRC

	/////////////////////////////////////////////////
	/* ------------------- Ȯ�� ���� ------------------ */
	/////////////////////////////////////////////////
	// size�� Ŀ�� ���� ���ڵ�/���ڵ� �ϰų�, ���� �Ҵ� ����ü�� ���⿡ �߰��Ѵ�.
	// Note : ���� field �߰��� ��� binary JSON encoding�� �⺻���� ũ�� ����� ����  DxRecData_EncodeBin()�� size ��� �κ� �߰� �ʿ���
	// below data are encode & decoded as extension (due to size ;;)
	DxRecList_Bookmark_t	astBookmarks[DxRECLIST_NUM_BOOKMARK];
	DxRecList_ExtEvt_t		*pstExtEvts;
	HCHAR					aucExtText[DxRECLIST_EXT_TEXT_BUFSIZE+1]; 	// ext evt text
	DxRecList_Bookmark_t	astPausedPoints[DxRECLIST_NUM_BOOKMARK];

	union
	{
		DxCasRecListData_t		cas;
		DxConaxRecListData_t	conax;
#if defined(CONFIG_OP_SES) // defined(CONFIG_APS_HD)
		HUINT8					ucSCPinCodeFlag;	// Playback�� SC Pin-Code�� ��������ϴ��� Ȯ���ϴ� flag
		HUINT8					reserved[511];
#else
		HUINT8					reserved[512];
#endif
	} ucas;

	union
	{
#if defined(CONFIG_OP_FREESAT)
		DxFreesatRecListData_t	freesat;
#endif
		DxUkdttRecListData_t	ukdtt;
	} extension;
} DxRecListData_t;


typedef enum
{
	eDxREC_TIMESTAMP_UNKNOWN,
	eDxREC_TIMESTAMP_PMT,
	eDxREC_TIMESTAMP_EIT,
	eDxREC_TIMESTAMP_DRM,
	eDxREC_TIMESTAMP_LICENSE,
	eDxREC_TIMESTAMP_MAX
} DxRecTimestampType_e;

typedef struct
{
	DxRecTimestampType_e	type;
	HUINT32					time;
	HUINT32					position;

	DxService_t				*service;
	DxEvent_t				*event;
} DxRecTimestamp_t;

HCHAR *				DxRECORDING_MakeURL (
								HCHAR *url
								, HUINT32 n
								, const HCHAR *uuid
								, HINT32 onid
								, HINT32 tsid
								, HINT32 sid
								, HINT32 svcuid
								, HUINT32 time
							);

void				DxRECLISTDATA_Delete (DxRecListData_t *rec);
DxRecListData_t *	DxRECLISTDATA_Clone (const DxRecListData_t *org);
HCHAR *				DxRECLISTDATA_Encode (const DxRecListData_t *rec, const HxCryptKey_t *key);
DxRecListData_t *	DxRECLISTDATA_Decode (HxJSON_t json, const HxCryptKey_t *key);
HINT32				DxRECLISTDATA_Compare (const DxRecListData_t *lhs, const DxRecListData_t *rhs);
void *				DxRECLISTDATA_Pack (const DxRecListData_t *rec, HUINT32 *size);
DxRecListData_t *	DxRECLISTDATA_Unpack (void *data);

/**
 * encryption�� json�� decrypt�ؿ�~~
 *
 * @param	json
 * @param	fileExt	"hjm" or "hjtsm"
 * @return	decrypted json
 */
HCHAR *				DxRECLISTDATA_Decrypt (HxJSON_t json, const HCHAR *fileExt);
const HxCryptKey_t *DxRECLISTDATA_GetCryptKey (void);
void				DxRECLISTDATA_ValidateName (DxRecListData_t *rec);

HCHAR *				DxRECLISTDATA_EncodeTimestamp (const DxRecTimestamp_t *self);
DxRecTimestamp_t *	DxRECLISTDATA_DecodeTimestamp (HxJSON_t json);

HCHAR *	DxRECORDING_Encrypt (const HCHAR *json, const HxCryptKey_t *key);
HCHAR *	DxRECORDING_Decrypt (const HCHAR *base64, const HxCryptKey_t *key);
void	DxRECORDING_FreeData(void *p);

#ifdef __cplusplus
}
#endif

#endif // DLIB_RECORDING_H__


