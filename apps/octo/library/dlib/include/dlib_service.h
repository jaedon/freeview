/* $Header: $ */

#ifndef DLIB_SERVICE_H__
#define DLIB_SERVICE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <hlib.h>

typedef	enum
{
	eDxSVC_TYPE_All			= 0,
	eDxSVC_TYPE_TV			= 0x0001,
	eDxSVC_TYPE_RADIO		= 0x0002,
	eDxSVC_TYPE_DATA		= 0x0004,
	eDxSVC_TYPE_SUBFEED		= 0x0008,
	eDxSVC_TYPE_REPLACEMENT	= 0x0010,
	eDxSVC_TYPE_HIDDEN   	= 0x0020,
	eDxSVC_TYPE_MHPTV		= 0x0040,
	eDxSVC_TYPE_ENGINEERING	= 0x0080,
	eDxSVC_TYPE_RFVOD		= 0x0100,
} DxSvcType_e;

typedef	enum
{
	eDxSVC_UPDATE_NORMAL			= 0x00000000,
	eDxSVC_UPDATE_NEWCHANNEL		= 0x00000001,
	eDxSVC_UPDATE_UPDATECHANNEL		= 0x00000002,
	eDxSVC_UPDATE_READONLY			= 0x00000003
} DxSvcUpdateFlag_e;

typedef	enum
{
	eDxOrgSVC_TYPE_NONE							= 0x00,
	eDxOrgSVC_TYPE_DIGITAL_TV					= 0x01,
	eDxOrgSVC_TYPE_DIGITAL_RADIO				= 0x02,
	eDxOrgSVC_TYPE_TELETEXT						= 0x03,
	eDxOrgSVC_TYPE_NVOD_REFERENCE				= 0x04,
	eDxOrgSVC_TYPE_NVOD_TIMESHIFT				= 0x05,
	eDxOrgSVC_TYPE_MOSAIC   					= 0x06,
	eDxOrgSVC_TYPE_PAL							= 0x07,
	eDxOrgSVC_TYPE_SECAM						= 0x08,
	eDxOrgSVC_TYPE_DD2_MAC						= 0x09,
	eDxOrgSVC_TYPE_FM_RADIO						= 0x0A,
	eDxOrgSVC_TYPE_NTSC							= 0x0B,
	eDxOrgSVC_TYPE_DATA_BROADCAST				= 0x0C,
	eDxOrgSVC_TYPE_DVB_MHP						= 0x10,
	eDxOrgSVC_TYPE_MPEG2_HD						= 0x11,
	eDxOrgSVC_TYPE_MPEG2_HD_NVOD_TIMESHIFT		= 0x12,
	eDxOrgSVC_TYPE_MPEG2_HD_NVOD_REFERENCE		= 0x13,
	eDxOrgSVC_TYPE_AAC_AUDIO					= 0x14,
	eDxOrgSVC_TYPE_AAC_V2_AUDIO					= 0x15,
	eDxOrgSVC_TYPE_AVC_SD_DIGITAL_TV			= 0x16,
	eDxOrgSVC_TYPE_AVC_SD_NVOD_TIMESHIFT		= 0x17,
	eDxOrgSVC_TYPE_AVC_SD_NVOD_REFERENCE		= 0x18,
	eDxOrgSVC_TYPE_AVC_HD_DIGITAL_TV			= 0x19,
	eDxOrgSVC_TYPE_AVC_HD_NVOD_TIMESHIFT		= 0x1A,
	eDxOrgSVC_TYPE_AVC_HD_NVOD_REFERENCE		= 0x1B,
	eDxOrgSVC_TYPE_DIGITURK_DOWN_START			= 0x80,
	eDxOrgSVC_TYPE_DIGITURK_TV_NO_PTS			= 0x82,
	eDxOrgSVC_TYPE_DIGITURK_INTERACTIVE			= 0x83,
	eDxOrgSVC_TYPE_AVC_HD_DIGITAL_TV2			= 0x86,
	eDxOrgSVC_TYPE_DIGITURK_DOWN_END			= 0x8F,
	eDxOrgSVC_TYPE_ARIB_TEMP_VIDEO				= 0xA1,
	eDxOrgSVC_TYPE_ARIB_TEMP_AUDIO				= 0xA2,
	eDxOrgSVC_TYPE_ARIB_TEMP_DATA				= 0xA3,
	eDxOrgSVC_TYPE_ARIB_ENGINEERING				= 0xA4,
	eDxOrgSVC_TYPE_ARIB_PROMOTION_VIDEO			= 0xA5,
	eDxOrgSVC_TYPE_ARIB_PROMOTION_AUDIO			= 0xA6,
	eDxOrgSVC_TYPE_ARIB_PROMOTION_DATA			= 0xA7,
	eDxOrgSVC_TYPE_ARIB_BEFORE_ACCUMUL_DATA		= 0xA8,
	eDxOrgSVC_TYPE_ARIB_ACCUMUL_DATA			= 0xA9,
	eDxOrgSVC_TYPE_ARIB_BOOKMARK_LIST_DATA		= 0xAA,
	eDxOrgSVC_TYPE_DATA							= 0xC0,
	eDxOrgSVC_TYPE_SUBFEED						= 0xD3,
} DxOrgSvcType_e;

typedef enum
{
	eDxSvcUserFlag_All					= 0,
	eDxSvcUserFlag_01_FsatUserCustom		= 0x00000001,	/**< all or incluing this condition	, CONFIG_OP_FREESAT */
	eDxSvcUserFlag_02_Selectable			= 0x00000002,	/**< all or incluing this condition	, CONFIG_OP_FREESAT or CONFIG_OP_UK_DTT */
	eDxSvcUserFlag_03_SvcDeletedByUser	= 0x00000004,	/**< exclusive condition			, Italy DTT에서 사용 중(CONFIG_DB_SVC_TRASH)이나 같은 요구 사항 내에서 사용 가능 */
	eDxSvcUserFlag_04_CanalreadyTerLcn	= 0x00000008,	/**< excusive condition 			, Canal Ready 에서 사용 중 */
	eDxSvcUserFlag_05_reserved			= 0x00000010,	/**< exclusive condition			, SkyD에서 사용 중, preferred service가 DCM이후에 없어졌다는 의미*/
	eDxSvcUserFlag_06_TemporaryAlive		= 0x00000020,	/**< all or incluing this condition	, add or update 이후, one shot으로 delete하기 위해 사용하는 flag */
	eDxSvcUserFlag_07_SkipChUpDown		= 0x00000040,	/**< all or exclusive condition		, Channel List에서는 보이나 Channel Up/Down Key로는 Access 되지 않는 Service를 의미한다. */
	eDxSvcUserFlag_08_RegionalSvc			= 0x00000080,	/**< exclusive condition			, TNTSAT에서 사용 */
	eDxSvcUserFlag_09_AdultSvc			= 0x00000100,	/**< exclusive condition			, CONFIG_OP_FREESAT */
	eDxSvcUserFlag_10_NationalSvc			= 0x00000200,	/**<exclusive condition				, CanalReady TNTSat only */
	eDxSvcUserFlag_11_FreeCiMode			= 0x00000400,	/**<exclusive condition				, CI+ Model only */
	eDxSvcUserFlag_12_IsExistFreqList		= 0x00000800,
	eDxSvcUserFlag_13_VolatileSvc			= 0x00001000,	/**< exclusive condition    	     , 휘발성 Service로 저장하지 않는다. Tune Action에서 사용 */
	eDxSvcUserFlag_14_NoRecTsr			= 0x00002000,	/**< exclusive condition 			, Recodring 과 TSR을 허용하지 않는 service */
	eDxSvcUserFlag_15_TsrOnly				= 0x00004000,	/**< exclusive condition 			, TSR만 허용 하는 service */
	eDxSvcUserFlag_16_QuasiStaticAlive	= 0x00008000,
	eDxSvcUserFlag_17_TivuSatLcn			= 0x00010000,
	eDxSvcUserFlag_18_DigiturkEutelSat	= 0x00020000,	/**< exclusive condition 			 */
	eDxSvcUserFlag_19_DigiturkTurkSat		= 0x00040000,	/**< exclusive condition 			 */
	eDxSvcUserFlag_20_ParentalControl		= 0x00080000,	/**< all condistion					, Parental control이 있었던 서비스를 marking. 이 mark가 없으면 wtv가 parental check를 skip한다. */
	eDxSvcUserFlag_21_LockSvcByUser		= 0x00100000,	/**< exclusive condistion			, Japan에서 사용 중, 튜닝시 패스워드를 물어야 하는 채널락 */
	eDxSvcUserFlag_22_SkipSvcByUser		= 0x00200000,	/**< exclusive condistion			, Japan에서 사용 중, 채널업/다운시 해당 서비스 스킵 (eSvcUserFlag_07_SkipChUpDown과 구분되어야 함.) */
	eDxSvcUserFlag_23_PreferSvcByUser	= 0x00400000,	/**< exclusive condistion			, Japan에서 사용 중, 선호채널키 선택시 튜닝되어야하는 서비스를 설정 (Favorite group은 채널번호 및 네트워크순으로 sorting/find 지원하지 않기 때문에 사용할 수 없음.) */
	eDxSvcUserFlag_24_TempSvc			= 0x00800000,	/**< exclusive condistion			, Japan에서 사용 중, 임시서비스 */
	eDxSvcUserFlag_25_MaskCh			= 0x01000000,	/**< exclusive condistion			, Japan에서 사용 중, 채널마스킹 */
	eDxSvcUserFlag_26_NordigLcnV2		= 0x02000000,	/**< exclusive condistion			, nordig lcn v2라는 의미  */
	eDxSvcUserFlag_27_reserved			= 0x04000000,
	eDxSvcUserFlag_28_reserved			= 0x08000000,
	eDxSvcUserFlag_29_reserved			= 0x10000000,
	eDxSvcUserFlag_30_reserved			= 0x20000000,
	eDxSvcUserFlag_31_reserved			= 0x40000000,
	eDxSvcUserFlag_32_NewService			= 0x80000000	/**< all or including this condition , 새로운 Channel이 발견 되었을때 해당 플래그를 세워준다. OCTO 2.0기준 해당 플래그는 OCTO가 APPL로 줄수만 있고 APPL은 해당 플래그를 세워 줘도 OCTO가 무시한다.*/
} DxUserFlag_t;

typedef enum
{
	eDxSvcSection_All				= 0,
	eDxSvcSection_PreProgrammed		= 0x01,
	eDxSvcSection_Lcn				= 0x02,
	eDxSvcSection_NonLcn_Priority1	= 0x10,
	eDxSvcSection_NonLcn_Priority2	= 0x20,
	eDxSvcSection_NonLcn_Priority3	= 0x40,
	eDxSvcSection_NonLcn_Priority4	= 0x80
	/* 더 이상 추가할 수 없음 !!!!!!!!! */
} DxSvcSection_e;


#if	defined(CONFIG_OP_FREESAT)
typedef struct
{
	HINT32		svcId;				///< "fsat_svcid"
	HINT32		regionId;			///< "fsat_rgnid"
	HCHAR		defaultAuth[DxDEFAULT_AUTHORITY_LEN];	///< "default_auth"
	HINT32		contentMgr;			///< "fta_contentmgr"

	/* channel icon 임시 추가 - /usr/browser/webapps/image/logo/chlog/svcid.png */
	HCHAR		szShortName[DxFSAT_SVC_SHORT_NAME_LEN];

	HUINT8		ucUserCustomizable;
	HUINT8		ucUserSelectable;
	HUINT8		ucAdultSvc;
	HUINT8		ucVideoCodecH264;

	HUINT8		ucGuidanceType;
	HUINT8		ucGuidanceMode;
	HUINT8		szGuidanceStr[DxFSAT_SVC_GUIDANCE_LEN];

	HUINT8		ucPresentFollow;
	// TODO: add more freesat related code here
} DxFreesatService_t;
#endif


typedef struct
{
	HUINT8				szCountryCode[4];
	HUINT8				ucRegionDepth; // 0xFF is Invalid mark
	HUINT32				ulRegionCode; // Primari(8) + Second(8) + Teritary(16)
} DxTvAnyTime_RegionInfo_t;

typedef struct
{
	HUINT8		                    ucGuidanceType;
	HUINT8		                    ucGuidanceMode;
	HCHAR		                    szGuidanceStr[DxMAX_GUIDANCE_INFO_LEN];
	HUINT32 		                ulFTAContentMgr;
	HCHAR		                    szDefaultAuth[DxDEFAULT_AUTHORITY_LEN];	///< "default_auth"
	DxTvAnyTime_RegionInfo_t		stRegionInfo[DxMAX_T_REGION_NUM];
	HUINT16		                    usLcnSub;

	HUINT8		                    ucUserSelectable;

	// TODO: add more TV-AnyTime related code here
} DxTvAnyTimeService_t;

#if defined(CONFIG_OP_SES)
typedef struct
{
	HUINT16		usVirtualSvcId;
	HBOOL		bNewSvc;

	// TODO: add more SES related code here
} DxSesService_t;
#endif

#if defined(CONFIG_OP_ITALY_DBOOK)
typedef struct
{
	HUINT8		ucUserSelectable;
	HUINT8		ucUserDeletedByUser;
} DxItaDttService_t;
#endif

#define	SVC_AUDIO_LANGUAGE_CODE			// Temp
#define	SVC_SUBTITLE_LANGUAGE_CODE		// Temp

typedef struct
{
	HINT32				uid;
	const HCHAR			*typeOf;
	HINT32				version;
	HINT32				tsuid;
	HINT32				prvuid;
	HINT32				antuid;
	HINT32				grpuids[DxGROUP_LEN];
	HINT32				bqtuids[DxBOUQUET_LEN];

	HINT32				svcid;
	HINT32				tsid;
	HINT32				onid;

	HINT32				lcn;
	HINT32				orglcn;
	
	HCHAR				name[DxNAME_LEN];
	DxSvcType_e			svcType;
	DxOrgSvcType_e		orgSvcType;	/* DxOrgSvcType_e */
	HINT32				optype;
	DxDeliveryType_e 	deliType;
	HCHAR				satType[DxANTINFO_NAME_LEN];
	HBOOL				visibleFlag;
	HBOOL				locked;
	HBOOL				removed;
	HBOOL				renamed;

	HINT32				pmtPid;
	HINT32				pcrPid;
	HINT32				videoPid;
	HINT32				audioPid;
	HINT32				ttxPid;
	HINT32				audioAuxPid;
	HINT32				dolbyPid;

	DxVideoType_e		highDefinition;
	DxCasType_e			casType;
	HBOOL				dolbyFlag;
	DxVideoCodec_e		videoCodec;
	DxAudioCodec_e		audioCodec;
	DxAudioCodec_e		audioAuxCodec;
	DxAudioCodec_e		dolbyCodec;
	HBOOL				lcnFlag;
	DxSvcSection_e		svcSection;

#if defined(CONFIG_OP_NORDIG)
	HUINT16 			simulcastOnid;
	HUINT16 			simulcastTsid;
	HUINT16 			simulcastSvcid;
#endif

	struct {
		HBOOL				subtitle;
		HBOOL				audio;
#if defined(SVC_AUDIO_LANGUAGE_CODE)
		HUINT8				audioLanguage[DxLANG_CODE_LEN];
		HBOOL				audioDescription;
		DxDualMonoSelect_e	dualMonoSelect;
#endif
#if defined(SVC_SUBTITLE_LANGUAGE_CODE)
		HUINT8				subtitleLanguage[DxLANG_CODE_LEN];
		HBOOL				hardOfHearing;
#endif
		HINT32				subtitleTrack;
		DxStereoSelect_e	audioTrack;
		HUINT32				ratingPassedEventId;
	} user;	///< user 선택 값

	struct {
		HUINT32			instType;
		HUINT32			slotId;
		HBOOL			selectableFlag;
		HUINT8			operatorName[DxNAME_LEN];
		HUINT8			camId[DxCAMID_LEN];
	} cas;

	DxSvcUpdateFlag_e	updateSvcFlag;
	union {
#if defined(CONFIG_OP_FREESAT)
		DxFreesatService_t		freesat;
#endif
#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	        DxTvAnyTimeService_t		tva;
#endif
#if defined(CONFIG_OP_SES)
		DxSesService_t			ses;
#endif
		void *	__;
	} ex;

    HUINT32                 __CRC__;  //CHECK CRC
} DxService_t;

typedef enum
{
	eDxLCNUPDATE_START_OK	=	0,
	eDxLCNUPDATE_START_FAIL,
	eDxLCNUPDATE_SAVE_OK,
	eDxLCNUPDATE_SAVE_FAIL,
	eDxLCNUPDATE_STOP_OK,
	eDxLCNUPDATE_STOP_FAIL,
	eDxLCNUPDATE_GET_INFO
}DxLcnUpdateEventType_e;

typedef struct
{
	HUINT32		index;
	HCHAR		szSvcListName[DxLCN_SVC_LIST_NAME_LEN];
} DxLcnUpdateServiceList_t;

typedef struct
{
	HUINT32		lcn;
	HCHAR		szSvcName[DxNAME_LEN];
	HUINT16		usOnId;
	HUINT16		usTsId;
	HUINT16		usSvcId;
	HINT32		ulSvcUid;
} DxLcnUpdateService_t;

typedef struct
{
	HUINT32							ulSvcListNum;
	DxLcnUpdateServiceList_t		*svcList;
} DxLcnUpdateStartData_t;

typedef struct
{
	HUINT32							ulNewSvcNum;
	DxLcnUpdateService_t			*newSvc;

	HUINT32							ulReplacedSvcNum;
	DxLcnUpdateService_t			*replacedSvc;
} DxLcnUpdateSaveData_t;

typedef struct
{
	HCHAR		szServiceListName[64];
	HUINT32		ulTvNum;
	HUINT32		ulRadioNum;
	HCHAR		szLastUpdatedTime[20];
	HCHAR		szLastCheckedTime[20];
	HBOOL		bDisplayInfo;
	HUINT32		ulColdbootResult;
} DxLcnUpdateInfo_t;

typedef struct
{
	DxLcnUpdateEventType_e			eventType;

	// Using for start event
	DxLcnUpdateStartData_t			*lcnUpdateStartData;

	// Using for save event
	DxLcnUpdateSaveData_t			*lcnUpdateSaveData;

	// Using for get info
	DxLcnUpdateInfo_t				*lcnUpdateInfo;
} DxLcnUpdateInform_t;


/* **********************************
	link Info in RCT(Related Content Table)
	ETSI TS 102 333 참조.
		* Table Id : 0x76;
************************************/
typedef enum	// ETSI TS 102 333 Table 110. Link_type 참조.
{
	eDxSVC_RctLinkType_Url 			= 0x0,	// Link information is a URI string only
	eDxSVC_RctLinkType_Binary 		= 0x1,	// Link information is a binary locator only
	eDxSVC_RctLinkType_Both 			= 0x2,	// Link information is both a binary locator and a URI string
	eDxSVC_RctLinkType_Descriptor 	= 0x3,	// Link information is through means of a descriptor

	eDxSVC_RctLinkType_Reserved_Start 	= 0x4,	// DVB reserved (0x4 ~ 0xF)
	eDxSVC_RctLinkType_Reserved_End		= 0xF,	// DVB reserved (0x4 ~ 0xF)

	eDxSVC_RctLinkType_Max		= 0xFF	//

}DxService_RctLinkType_e;

#if 0 // TODO: 확인 필요..
typedef enum	// ETSI EN 300 468  Table 126. Coordinate system 참조.
{
	eDxSVC_RctImgCordinateSystem_720X576 			= 0x0,	// The coordinate system is 720 x 576
	eDxSVC_RctImgCordinateSystem_1280X720			= 0x1,	// The coordinate system is 1280 x 720
	eDxSVC_RctImgCordinateSystem_1920X1080			= 0x2,	// The coordinate system is 1920 x 1080
	eDxSVC_RctImgCordinateSystem_Reserved_Start			= 0x3,	// DVB reserved (0x03 ~ 0x06)
	eDxSVC_RctImgCordinateSystem_Reserved_End 			= 0x6,	// DVB reserved (0x03 ~ 0x06)
	eDxSVC_RctImgCordinateSystem_Private 				= 0x7	// User Private

	eDxSVC_RctImgCordinateSystem_Max 				= 0xFF	// User Private
}DxService_RctImgDesCoordinateSystem_e;
#endif

typedef enum	//  ETSI TS 102 333 Table 111. how_related_classification_scheme_id 참조.
{
	eDxSVC_RctClassificationScheme_2004 			= 0x0,	// The coordinate system is 720 x 576
	eDxSVC_RctClassificationScheme_2005			= 0x1,	// The coordinate system is 1280 x 720
	eDxSVC_RctClassificationScheme_2007			= 0x2,	// The coordinate system is 1920 x 1080
	eDxSVC_RctClassificationScheme_Reserved_Start 		= 0x3,	// DVB reserved (0x02 ~ 0x2f)
	eDxSVC_RctClassificationScheme_Reserved_End		= 0x2F,	// DVB reserved (0x02 ~ 0x2f)
	eDxSVC_RctImgCordinateSystem_Private_Start 		= 0x30,	// User Private (0x30 ~ 0x3f)
	eDxSVC_RctImgCordinateSystem_Private_End 			= 0x3F,	// User Private	(0x30 ~ 0x3f)

	eDxSVC_RctImgCordinateSystem_Max 			= 0xFF	//
}DxService_RctClassification_SchemeId_e;

typedef enum	// D-Book 7 Part A v2 Table 8-34. HowRelated types for Trailer Booking Service
{
	eDxSVC_RctTermId_Programme					= 0x0002,	// programme CRID
	eDxSVC_RctTermId_Series						= 0x0005,	// series CRID
	eDxSVC_RctTermId_Max 			= 0xFF	//
}DxService_RctTermId_e;

typedef struct
{
	HUINT8		ucMediaUriLen;
	HUINT8		szMediaUri[DxRCT_MAX_MEDIA_URI + 1];
} DxService_RctUriInfo_t;

typedef struct
{
	HUINT8		ucIdentifierType;
	HUINT8		ucScheTimeReliability;
	HUINT8		ucInlineSvc;
	HUINT16		usStartDate;

	// if (inline service == '0')
	HUINT16		usDvbSvcTripletId;
	// else
	HUINT16		usTsId;
	HUINT16		usOnId;
	HUINT16		usSvcId;

	HUINT16		usStartTime;
	HUINT16		usDuration;

	// if (identifier type == '01')
	HUINT16		usEvtId;
	// if (identifier type == '10') is TVA id only
	// if (identifier type == '11') is TVA id and component
	HUINT16		usTvaId;
	HUINT8		ucComponent;

	// if (identifier type == '00' && scheduled time reliability == '1'))
	HUINT8		ucEarlyStartWin;
	HUINT8		ucLateEndWin;
} DxService_RctDvbBinaryLocator_t;

typedef struct
{
	HUINT8		szLangCode[DxLANG_CODE_LEN];
	HUINT8		ucPromotionalTextLen;
	HUINT8		szPromotionalText[DxRCT_MAX_PROMOTIONAL_TEXT + 1];
}DxService_RctPromotionalText_t;

typedef struct
{
	HUINT8		szLangCode[DxLANG_CODE_LEN];
	HUINT8		szEventName[DxRCT_MAX_SHORT_EVENT_NAME];
	HUINT8		szEventText[DxRCT_MAX_SHORT_EVENT_TEXT];
}DxService_RctShortEvtInfo_t;

typedef struct
{
	HUINT8		ucIconId;
	HUINT8		ucTransPortMode;
	HUINT8		ucPositionFlag;
	HUINT8		ucCoordinateSystem;
	HUINT16		usHorizontalOrigin;
	HUINT16		usVerticalOrigin;
	HUINT8		szIconTypeChar[DxRCT_MAX_IMAGE_ICON_TYPE_CHART + 1];
	HUINT32		ulDataByteSize;
	HUINT8		*pucDatabyte;
	HUINT8		szIconUrl[DxRCT_MAX_IMAGE_ICON_URL_LEN + 1];
} DxService_RctImageIconInfo_t;

typedef	struct	tagRCTLinkInfo
{
	HUINT8		ucLinkType;
	HUINT8		ucHowRelatedSchemeId;
	HUINT16		usTermId;
	HUINT8		ucGroupId;
	HUINT8		ucPrecedence;
	DxService_RctUriInfo_t			stMediaUri;
	DxService_RctDvbBinaryLocator_t	stDvbBinLocator;
	HUINT8		ucProTextNum;
	DxService_RctPromotionalText_t	stPromotionalText;

	HBOOL		bDefaultIcon;
	HUINT8		ucIconId;

	DxService_RctShortEvtInfo_t		stShortEvtInfoInLink;
} DxService_RctLinkInfo_t;


typedef	struct	tagRelatedContent
{
	HUINT16 		usServiceId;
	HUINT16		usYearOffset;
	HUINT32		ulTotalLinkCnt;

	DxService_RctLinkInfo_t			*pstLinkInfoList;
	DxService_RctShortEvtInfo_t		stShortEvtInfo;
	DxService_RctImageIconInfo_t	stImageIconInfo;
} DxService_RctInfo_t;


typedef enum
{
	eDxTRDCONFLICT_UNKNOWN =	0,
	eDxTRDCONFLICT_GET_INFO,

	eDxTRDCONFLICT_SET_INFO ,
}DxTrdConflictEventType_e;

typedef struct
{
	HUINT16		usTsIdx;
	HUINT16		usSvcId;
} DxTrdSvcUniqueData_t;

typedef struct
{
	HUINT32					 ulSvcNum, ulArrayLen;
	HUINT16			 		usLcn;
	DxTrdSvcUniqueData_t	*pstUniqDataArray;
} DxTrdConflictServiceList_t;

typedef struct
{
	HUINT32			 		 ulTblNum, ulArrayLen;
	DxTrdConflictServiceList_t		*pstArray;
} DxTrdConflictInfo_t;

typedef struct
{
	HUINT32				 ulLen;
	HUINT8				szTrdConflictedSvcName[DxTRD_MAX_CONFLICT_SVC_NAME];
} DxTrdConflictSvc_t;
typedef struct
{
	HUINT32				 ulArrayNum;
	DxTrdConflictSvc_t	 	stTrdConflictedArray[DxTRD_MAX_T_REGION_NUM];
} DxTrdConflictList_t;

typedef struct
{
	DxTrdConflictEventType_e			eventType;
	HUINT32				 ulTrdConflictCount;
	// Using for get info
	DxTrdConflictList_t				*trdconflictInfo;
} DxTrdConflictData_t;
typedef enum
{
	eDxNCDEVT_NONE,
	eDxNCDEVT_GET_INFO,
	eDxNCDEVT_END
}DxNcdEvent_e;

typedef struct
{
	DxNcdEvent_e			eventType;
	HUINT8				szNcdnetworkMsg[DxNCD_MAX_MESSAGE_LEN];
} DxNcdNetworkInfo_t;

typedef enum
{
	eDxNETWORKURIEVT_NONE,
	eDxNETWORKURIEVT_GET_INFO,
	eDxNETWORKURIEVT_END
}DxNetworkUriEvent_e;

typedef struct
{
	DxNetworkUriEvent_e		eventType;
	HUINT8				szNetworkUriPrefix[DxMAX_URI_PREFIX_INFO_LEN];
} DxNetworkUriInfo_t;


HCHAR *			DxSERVICE_Encode (HINT32 jsonWriter, const DxService_t *service);
DxService_t *	DxSERVICE_Decode (DxService_t *service, HxJSON_t json);
DxService_t	*	DxSERVICE_Clone (const DxService_t *service);
void			DxSERVICE_Delete (DxService_t *service);

#ifdef __cplusplus
}
#endif

#endif // DLIB_SERVICE_H__


