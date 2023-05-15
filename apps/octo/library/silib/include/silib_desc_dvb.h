/* $Header: $ */

#ifndef __SILIB_DVB_H__
#define __SILIB_DVB_H__

#ifndef _____INCLUDED_ONLY_IN_SILIB_H____
#error The file [silib_desc_dvb.h] shall not be included except silib.h
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
	from 300 468 Table 58: Linkage type coding
	Linkage_type Description
	0x00         reserved for future use
	0x01         information service
	0x02         EPG service
	0x03         CA replacement service
	0x04         TS containing complete Network/Bouquet SI
	0x05         service replacement service
	0x06         data broadcast service
	0x07         RCS Map
	0x08         mobile hand-over
	0x09         System Software Update Service (TS 102 006 [11])
	0x0A         TS containing SSU BAT or NIT (TS 102 006 [11])
	0x0B         IP/MAC Notification Service (EN 301 192 [4])
	0x0C         TS containing INT BAT or NIT (EN 301 192 [4])
	0x0D         event linkage (see note)
	0x0E         to 0x7F reserved for future use
	0x80         to 0xFE user defined
	0xFF         reserved for future use
	NOTE: A linkage_type with value 0x0D is only valid when the descriptor is carried in the EIT.
*/
typedef enum
{
	eLinkageType_Information        = 0x01,
	eLinkageType_Epg                = 0x02,
	eLinkageType_CaReplacement      = 0x03,
	eLinkageType_Home               = 0x04,
	eLinkageType_ServiceReplacement = 0x05,
	eLinkageType_DataBroadcast      = 0x06,
	eLinkageType_RcsMap             = 0x07,
	eLinkageType_MobileHandOver     = 0x08,
	eLinkageType_Ssu                = 0x09,
	eLinkageType_SsuBatNit          = 0x0A,
	eLinkageType_IpMacNotification  = 0x0B,
	eLinkageType_IntBatNit          = 0x0C,
	eLinkageType_Event              = 0x0D

} SIxLinkageType_e;

/** @brief Private Data 구조 */
typedef struct
{
	SIxDescriptorHeader_t	stHead;
	HUINT8					ucPrivateDataLen;
	HUINT8					pucPrivateData[];	// [0] : Tag, [1] : Length
} SIxPrivateDataInfo_t;

/** @brief eSIxDESCTAG_VIDEO_STREAM (0x02) */
typedef struct
{
	SIxDescriptorHeader_t	stHead;
	HBOOL					bMultiFrameRate;
	HUINT8					ucFrameRateCode;
	HBOOL					bMPEG2;				/** < if TRUE then, MPEG1. else MPEG2 */
	HBOOL					bConstrainParam;
	HBOOL					bStillPic;
} SIxVideoStmDes_t;

/** @brief eSIxDESCTAG_AUDIO_STREAM (0x03) */
typedef struct
{
	SIxDescriptorHeader_t	stHead;
	HBOOL					bFreeFormat;
	HUINT8					ucId;
	HUINT8					ucLayer;
	HUINT8					ucVarRateAudioIndicator;
} SIxAudioStmDes_t;

/** @brief eSIxDESCTAG_HIERARCHY (0x04) */
typedef struct
{
	SIxDescriptorHeader_t	stHead;
	HUINT8					ucHierType;
	HUINT8					ucHierLayerIndex;
	HUINT8					ucHierEmbedLayerIndex;
	HUINT8					ucHierChannel;
} SIxHierDes_t;

/** @brief eSIxDESCTAG_REGISTRATION (0x05) */
typedef struct
{
	SIxDescriptorHeader_t	stHead;
	HUINT32					ulFormatIdent;
	SIxPrivateDataInfo_t	*pstPrivateDataInfo;
} SIxRegDes_t;

/** @brief eSIxDESCTAG_DATA_STREAM_ALIGNMENT (0x06) */
typedef struct
{
	SIxDescriptorHeader_t	stHead;
	HUINT8					ucAlignType;
} SIxDataStmAlignDes_t;

/** @brief eSIxDESCTAG_TARGET_BACKGROUND_GRID (0x07) */
typedef struct
{
	SIxDescriptorHeader_t	stHead;
	HUINT16					usHoriSize;
	HUINT16					usVertSize;
	HUINT8					ucAspectRatio;
} SIxBackGridDes_t;

/** @brief eSIxDESCTAG_VIDEO_WINDOW (0x08) */
typedef struct
{
	SIxDescriptorHeader_t	stHead;
	HUINT16					usHoriOff;
	HUINT16					usVertOff;
	HUINT8					ucWindowPriority;
} SIxVideoWindowDes_t;

/** @brief eSIxDESCTAG_CA (0x09) */
typedef struct
{
	SIxDescriptorHeader_t	stHead;
	HUINT16				 	usCaSysId;
	HUINT16					usCaPid;
	SIxPrivateDataInfo_t	*pstPrivateDataInfo;
} SIxCaDes_t;

/** @brief eSIxDESCTAG_ISO_639_LANGUAGE (0x0a) */
typedef struct tagSIxLANG_INFO
{
	HUINT8					 szIso639LangCode[4];
	HUINT8					 ucAudioType;
	struct tagSIxLANG_INFO	*pstNext;
} SIxLangInfo_t;

typedef struct
{
	SIxDescriptorHeader_t	stHead;
	SIxLangInfo_t			*pstInfo;
} SIxIso639LangDes_t;

/** @brief eSIxDESCTAG_SYSTEM_CLOCK (0x0b) */
typedef struct
{
	SIxDescriptorHeader_t	stHead;
	HUINT8					ucExtClockRef;
	HUINT8					ucClockAccuInt;
	HUINT8					ucClockAccuExp;
} SIxSysClockDes_t;

/** @brief eSIxDESCTAG_MULTIPLEX_BUFFER_UTILIZATION (0x0c) */
typedef struct
{
	SIxDescriptorHeader_t	stHead;
	HBOOL					bBoundValid;
	HUINT16					usLtwOffLowBound;
	HUINT16					usLtwOffUpBound;
} SIxMultiplexBufUtilDes_t;

/** @brief eSIxDESCTAG_COPYRIGHT (0x0d) */
typedef struct
{
	SIxDescriptorHeader_t	stHead;
	HUINT32					ulCopyrightIdent;
	SIxPrivateDataInfo_t	*pstPrivateDataInfo;
} SIxCopyRightDes_t;

/** @brief eSIxDESCTAG_MAXIMUM_BITRATE (0x0e) */
typedef struct
{
	SIxDescriptorHeader_t	stHead;
	HUINT32					ulMaxBitrate;
} SIxMaxBitrateDes_t;

/** @brief eSIxDESCTAG_PRIVATE_DATA_INDICATOR (0x0f) */
typedef struct
{
	SIxDescriptorHeader_t	stHead;
	HUINT32					ulPrivateDataIndicatorDes;
} SIxPrivateDataIndicatorDes_t;

/** @brief eSIxDESCTAG_SMOOTHING_BUFFER (0x10) */
typedef struct
{
	SIxDescriptorHeader_t	stHead;
	HUINT32					ulLeakRate;
	HUINT32					ulSize;
} SIxSmoothBufDes_t;

/** @brief eSIxDESCTAG_STD (0x11) */
typedef struct
{
	SIxDescriptorHeader_t	stHead;
	HBOOL					bLeakValid;
} SIxStdDes_t;

/** @brief eSIxDESCTAG_CAROUSEL_ID (0x13) */
typedef struct
{
	HUINT8		 ucModuleVersion;
	HUINT16		 usModuleId;
	HUINT16		 usBlockSize;
	HUINT32		 ulModuleSize;
	HUINT8		 ucCompressionMethod;
	HUINT32		 ulOriginalSize;
	HUINT8		 ucTimeout;
	HUINT8		 aucObjectKeyData[4];
} SIxCarouselIdDesInfo_t;

typedef struct
{
	SIxDescriptorHeader_t	stHead;
	HUINT32					ulCarouselId;
	HUINT8					ucFormatId;		/** < 0x00 or 0x01 */

	/** < only if ucFormatId == 0x01, pstCarouselIdDesInfo is valid.
		otherwise, pstCarouselIdDesInfo == NULL */
	SIxCarouselIdDesInfo_t	*pstInfo;
} SIxCarouselIdDes_t;

/** @brief eSIxDESCTAG_ASSOCITION_TAG (0x14) */
typedef struct
{
	SIxDescriptorHeader_t	stHead;
	HUINT16					usAssocTag;
	HUINT16					usUse;			/** < 0x00, 0x01, or otherwise */

	/** < following 2 fields are valid only if usUse == 0x00
		otherwise, don't care about them.*/
	HUINT32					ulTransactionId;
	HUINT32					ulTimeout;
} SIxAssocTagDes_t;

/** @brief eSIxDESCTAG_DEFERED_ASSOCIATION_TAG (0x15) */
typedef struct
{
	SIxDescriptorHeader_t	stHead;
	HUINT8					ucAssocTagSize;	/** < #. of assoc_tag */
	HUINT16					*ausAssocTags;		/** < array of assoc_tag */

	/** < triple ID */
	HUINT16					usTransportStreamId;
	HUINT16					usProgNum;
	HUINT16					usOriginalNetworkId;
} SIxDeferAssocTagDes_t;

typedef struct
{
	SIxDescriptorHeader_t	stHead;
	HUINT8					ucProfile;					/** < profile_idc */
	HBOOL					bConstSet0Flag;				/** < constraint_set0_flag */
	HBOOL					bConstSet1Flag;				/** < constraint_set1_flag */
	HBOOL					bConstSet2Flag;				/** < constraint_set2_flag */
	HUINT8					ucAvcCompatibleFlags;		/** < avc_compatible_flags */
	HUINT8					ucLevel;					/** < level_idc */
	HUINT8					ucAvcStillPresent;			/** < avc_still_present */
	HBOOL					bAvc24hourPicFlag;			/** < vc_24_hour_picture_flag */
} SIxAvcVideoDes_t;

/****************************************************/
/*     												*/
/*     DVB 정의한 Descriptor 구조체(ETS 300 468).  	*/
/*       											*/
/****************************************************/
/** @brief eSIxDESCTAG_NETWORK_NAME (0x40) */
typedef struct
{
	SIxDescriptorHeader_t	stHead;
	HUINT8					*pszNetName;
} SIxNetNameDes_t;

/** @brief eSIxDESCTAG_SERVICE_LIST (0x41) */
typedef struct tagSIxSvcListInfo
{
	HUINT16						 usSvcId;
	HUINT8						 ucServType;
	struct tagSIxSvcListInfo	*pstNext;
} SIxSvcListInfo_t;

typedef struct
{
	SIxDescriptorHeader_t		stHead;
	SIxSvcListInfo_t			*pstInfo;
} SIxSvcListDes_t;

/** @brief eSIxDESCTAG_SATELLITE_DELIVERY_SYSTEM (0x43) */
typedef struct
{
	SIxDescriptorHeader_t		stHead;
	HUINT32						ulFreq;
	HUINT16						usOrbitPos;
	HBOOL						bWestEastFlag;
	HUINT8						ucPolar;
	HUINT8						ucModul;
	HUINT8						ucModType;
	HUINT32						ulSymbolRate;
	HUINT8						ucFECInner;
	/* DVB S2 */
	HUINT8						ucTransSpec;
	HUINT8						ucRollOff;
} SIxSatDeliSystemDes_t;

/** @brief eSIxDESCTAG_CABLE_DELIVERY_SYSTEM (0x44) */
typedef struct
{
	SIxDescriptorHeader_t		stHead;
	HUINT32						ulFreq;
	HUINT8						ucFECOuter;
	HUINT8						ucModul;
	HUINT32						ulSymbolRate;
	HUINT8						ucFECInner;
} SIxCabDeliSystemDes_t;

/** @brief eSIxDESCTAG_BOUQUET_NAME (0x47) */
typedef struct
{
	SIxDescriptorHeader_t		 stHead;
	HUINT8			*pszBouqName;
} SIxBouqNameDes_t;

/** @brief eSIxDESCTAG_SERVICE (0x48) */
typedef struct
{
	SIxDescriptorHeader_t		 stHead;
    HUINT8						 ucServType;
    HUINT8						 ucPrvName;
    HUINT8						*pszProviderName;
    HUINT8						 ucSvcName;
    HUINT8						*pszServName;
} SIxSvcDes_t;

/** @brief eSIxDESCTAG_COUNTRY_AVAILABILITY (0x49) */
typedef struct tagSIxCountryAvailInfo
{
	HUINT8							szCountryAvailCode[4];
	struct tagSIxCountryAvailInfo	*pstNext;
} SIxCountryAvailInfo_t;

typedef struct
{
	SIxDescriptorHeader_t		stHead;
	HBOOL						bCountryAvail;
	SIxCountryAvailInfo_t		*pstInfo;
} SIxCountryAvailDes_t;

/** @brief eSIxDESCTAG_LINKAGE (0x4a) */
typedef struct tagSIxOuiInfo
{
	HUINT32					 ulOui;
	HUINT8					 ucSelectorLen;
	HUINT8					*pucSelector;
	struct tagSIxOuiInfo	*pstNext;
} SIxOuiInfo_t;

typedef struct tagSIxLink09Info	/** < betadigital */
{
	HUINT8					 ucPrivateDataLen;
	HUINT8					*pucPrivateData;
	SIxOuiInfo_t			*pstInfo;
} SIxLink09Info_t;

typedef struct tagSIxLink0AInfo	/** < betadigital */
{
	HUINT8		ucTableId;
} SIxLink0AInfo_t;

typedef	struct tagSIxLink80Info	/** < Irdeto */
{
	HUINT8		ucManufacturerCode;
	HUINT8		ucControlByte;
	struct 		tagSIxLink80Info *pstNext;
} SIxLink80Info_t;

typedef	struct tagSIxLink82Info	/** < Irdeto */
{
	HUINT8		ucManufacturerCode;
	HUINT8		ucHardwareCode;
	HUINT8		ucVariantHi;
	HUINT8		ucVariantLo;
	HUINT8		ucLoadSequenceNumber;
	HUINT8		ucControlByte;
	struct 		tagSIxLink82Info *pstNext;
} SIxLink82Info_t;

typedef	struct tagSIxLinkIr09Info	/** < Irdeto */
{
	HUINT16		usManufacturerId;
	HUINT16		usHardwareVersion;
	HUINT16		usVariant;
	HUINT16		usSubVariant;
	HUINT16		usDownloadSequenceNumber;
	HUINT8		ucTiggerControlByte;
	HUINT8		ucDownloadMode;
	struct 		tagSIxLinkIr09Info *pstNext;
} SIxLinkIr09Info_t;

typedef	struct			/** < humax ota */
{
	HUINT32		ulUniqueID;
	HUINT32		ulHardwareVersion;
	HUINT32		ulSoftwareVersion;
	HUINT8		ucForcaOtaFlag;
} SIxLink85Info_t;

typedef struct		 /** < NorDic, Bootloader */
{
	HUINT16		usManufactureId;
	HUINT32		ulVersionHigh;
	HUINT32		ulVersionLow;
	HUINT32		ulPrivateId;
	UNIXTIME	startTime;
} SIxNorDicBootLoader_t;

typedef struct	/** < NorDic, Bootloader */
{
	HUINT8					ucNumBootLoader;
	SIxNorDicBootLoader_t	*astBootLoader;
} SIxLink81Info_t;

typedef struct 	/** < KDG, Bouquet ID list */
{
	HUINT8				ucNumBqId;
	HUINT16				*ausBouquetId;
} SIxLink9BInfo_t;

typedef struct
{
	HUINT8				ucOuiDataLen;
	HUINT8				*pucPrivateData;
 	SIxOuiInfo_t		*pstInfo;
} SIxSsuLinkInfo_t;

typedef struct
{
	SIxDescriptorHeader_t	stHead;
	HUINT16					usTsId;
	HUINT16					usOrgNetId;
	HUINT16					usSvcId;
	HUINT8					ucLinkType;		// Link Type 이후의 모든 Private Data Info는 받은 곳에서 직접 parsing 하도록 한다.
	SIxPrivateDataInfo_t	*pstPrivateDataInfo;
} SIxLinkDes_t;

/** @brief eSIxDESCTAG_NVOD_REFERENCE (0x4b) */
typedef struct tagSIxNvodRefInfo
{
	HUINT16						usTsId;
	HUINT16						usOrgNetId;
	HUINT16						usSvcId;
	struct tagSIxNvodRefInfo	*pstNext;
} SIxNvodRefInfo_t;

typedef struct
{
	SIxDescriptorHeader_t		stHead;
	SIxNvodRefInfo_t			*pstInfo;
} SIxNvodRefDes_t;

/** @brief eSIxDESCTAG_TIME_SHIFTED_SERVICE (0x4c) */
typedef struct
{
	SIxDescriptorHeader_t		stHead;
	HUINT16						usRefSvcId;
} SIxTimeShiftSvcDes_t;

/** @brief eSIxDESCTAG_SHORT_EVENT (0x4d) */
typedef struct
{
	SIxDescriptorHeader_t		stHead;
	HUINT8						szIso639LangCode[4];
	HUINT8						ucEvtNameSize;
	HUINT8						*pszEvtName;
	HUINT8						ucTextSize;
	HUINT8						*pszText;
} SIxShortEvtDes_t;

/** @brief eSIxDESCTAG_EXTENDED_EVENT (0x4e) */
typedef struct tagSIxExtEvtInfo
{
	HUINT8						*pszDescription;
	HUINT8						*pszItem;
	/* 일본향에서는 pszItem 이 descriptor에 2개 걸쳐서온다. 그래서
	문자를 변환해서 올리지 못하고 한번에 합쳐서 변환해서 글자가 안깨진다.
	- pszItem만 변환 안해서 올린다. */
	HUINT8						ucItemLength;
	struct tagSIxExtEvtInfo		*pstNext;
} SIxExtEvtInfo_t;

typedef struct
{
	SIxDescriptorHeader_t		stHead;
	HUINT8						ucDesNum;
	HUINT8						ucLastDesNum;
	HUINT8						szIso639LangCode[4];
	SIxExtEvtInfo_t				*pstInfo;
	HUINT8						*pszChar;		// 이거... 다 합친 후 UTF-8로 변환해야 해서, 사용하면 안되지만, 일단 그냥 둠
	HUINT8						ucTextLength;
	HUINT8						*pucTextChar;
} SIxExtEvtDes_t;

/** @brief eSIxDESCTAG_TIME_SHIFTED_EVENT (0x4f) */
typedef struct
{
	SIxDescriptorHeader_t		stHead;
	HUINT16						usRefSvcId;
	HUINT16						usRefEvtId;
} SIxTimeShiftEvtDes_t;

/** @brief eSIxDESCTAG_COMPONENT (0x50) */
typedef struct
{
	SIxDescriptorHeader_t		stHead;
	HUINT8			 			ucStmContent;
	HUINT8						ucCompType;
	HUINT8						ucCompTag;
	HUINT8						szIso639LangCode[4];
	HUINT8						*pszText;
} SIxCompDes_t;

/** @brief eSIxDESCTAG_MOSAIC (0x51) */
typedef struct
{
	HUINT16	usBouqId;
} SIxBouqCellInfo_t;

typedef struct
{
	HUINT16	usOrgNetId;
	HUINT16	usTsId;
	HUINT16	usSvcId;
} SIxSvcCellInfo_t;

typedef struct
{
	HUINT16	usOrgNetId;
	HUINT16	usTsId;
	HUINT16	usSvcId;
} SIxOtherMosaicCellInfo_t;

typedef struct
{
	HUINT16	usOrgNetId;
	HUINT16	usTsId;
	HUINT16	usSvcId;
	HINT32	nEvtId;
} SIxEvtCellInfo_t;

typedef union
{
	SIxBouqCellInfo_t				stBouqCellInfo;
	SIxSvcCellInfo_t				stServCellInfo;
	SIxOtherMosaicCellInfo_t		stOtherMosaicCellInfo;
	SIxEvtCellInfo_t				stEvtCellInfo;
} SIxCellLinkInfo_t;

typedef struct tagSIxELEM_CELL_INFO
{
	HUINT8						ucElemCellId;
	struct tagSIxELEM_CELL_INFO	*pstNext;
} SIxElemCellInfo_t;

typedef struct tagSIxMOSAIC_INFO
{
	HUINT8						ucLogicalCellId;
	HUINT8						ucLogicalCellPresentationInfo;
	HUINT8						ucCellLinkageInfo;
	HUINT8						ucReserved;
	SIxElemCellInfo_t			*pstInfo;
	SIxCellLinkInfo_t			*pstCellLinkInfo;
	struct tagSIxMOSAIC_INFO	*pstNext;
} SIxMosaicInfo_t;

typedef struct
{
	SIxDescriptorHeader_t		stHead;
	HUINT8						ucEntryPoint;
	HUINT8						ucHoriElemCellNum;
	HUINT8						ucVertElemCellNum;
	SIxMosaicInfo_t				*pstInfo;
} SIxMosaicDes_t;

/** @brief eSIxDESCTAG_STREAM_IDENTIFIER (0x52) */
typedef struct
{
	SIxDescriptorHeader_t		stHead;
	HUINT8						ucCompTag;
} SIxStmIdentDes_t;

/** @brief eSIxDESCTAG_CA_IDENTIFIER (0x53) */
typedef struct tagSIxCaIdentInfo
{
	HUINT16						 usCaStmId;
	struct tagSIxCaIdentInfo	*pstNext;
} SIxCaIdentInfo_t;

typedef struct
{
	SIxDescriptorHeader_t		stHead;
	SIxCaIdentInfo_t			*pstInfo;
} SIxCaIdentDes_t;

/** @brief eSIxDESCTAG_CONTENT (0x54) */
typedef struct tagSIxCONTENT_INFO
{
	HUINT8						ucContentNibble;
	HUINT8						ucUserNibble;
	struct tagSIxCONTENT_INFO	*pstNext;
} SIxContentInfo_t;

typedef struct
{
	SIxDescriptorHeader_t		stHead;
	SIxContentInfo_t			*pstInfo;
} SIxContentDes_t;

/** @brief eSIxDESCTAG_PARENTAL_RATING (0x55) */
typedef struct tagSIxParentInfo
{
	HUINT8						szCountryCode[4];
	HUINT8						ucRate;
	struct tagSIxParentInfo	*pstNext;
} SIxParentInfo_t;

typedef struct
{
	SIxDescriptorHeader_t		stHead;
	SIxParentInfo_t				*pstInfo;
} SIxParentDes_t;

/** @brief eSIxDESCTAG_TELETEXT (0x56) */
typedef struct tagSIxTeletextInfo
{
	HUINT8						szIso639LangCode[4];
	HUINT8						ucType;
	HUINT8						ucMagazineNum;
	HUINT8						ucPageNum;
	struct tagSIxTeletextInfo	*pstNext;
} SIxTeletextInfo_t;

/** @brief Closed Caption and Super infomation in PMT */
typedef struct
{
	HUINT16		usPid;
	HUINT8		aucLangCode[8]; /* CAPTION_SUPER_LANG_MAX랑 같은 값 */
	HUINT8		ucComponentTag;
} SIxCC_SUPERInfo_t;

typedef struct
{
	SIxDescriptorHeader_t		stHead;
	SIxTeletextInfo_t	*pstInfo;
} SIxTeletextDes_t;

/** @brief eSIxDESCTAG_TELEPHONE (0x57) */
typedef struct
{
	SIxDescriptorHeader_t		stHead;
	HUINT8						ucForeignAvail;
	HUINT8						ucConnectionType;
	HUINT8						*pszCountryPrefix;
	HUINT8						*pszInternationalAreaCode;
	HUINT8						*pszOperatorCode;
	HUINT8						*pszNationalAreaCode;
	HUINT8						*pszCoreNumber;
} SIxTelephoneDes_t;

/** @brief eSIxDESCTAG_LOCAL_TIME_OFFSET (0x58) */
typedef struct tagSIxLOCAL_TIME_INFO
{
	HUINT8							szCountryCode[4];
	HUINT8							ucRegionId;
	HUINT8							ucLocalTimeOffPolar;
	HUINT8							pucLocalTimeOff[2];
	UNIXTIME 						timeOfChange;
	HUINT8							pucNextTimeOffset[2];
	struct tagSIxLOCAL_TIME_INFO	*pstNext;
} SIxLocalTimeInfo_t;

typedef struct
{
	SIxDescriptorHeader_t			stHead;
	SIxLocalTimeInfo_t				*pstInfo;
} SIxLocalTimeDes_t;

/** @brief eSIxDESCTAG_SUBTITLING (0x59) */
typedef struct tagSIxSubtitleInfo
{
	HUINT8						 szIso639LangCode[4];
	HUINT8						 ucSubtitleType;
	HUINT16						 usCompositionPageId;
	HUINT16						 usAncillaryPageId;
	struct tagSIxSubtitleInfo	*pstNext;
} SIxSubtitleInfo_t;

typedef struct
{
	SIxDescriptorHeader_t		stHead;
	SIxSubtitleInfo_t			*pstInfo;
} SIxSubtitleDes_t;

/** @brief eSIxDESCTAG_TERRESTRIAL_DELIVERY_SYSTEM (0x5a) */
typedef	struct
{
	SIxDescriptorHeader_t		stHead;
	HUINT32						ulFreq;
	HUINT8						ucBandWidth;
	HUINT8						ucPriority;
	HUINT8						ucConstellation;
	HUINT8						ucHierInfo;
	HUINT8						ucCodeRateHPStream;
	HUINT8						ucCodeRateLPStream;
	HUINT8						ucGuardInterval;
	HUINT8						ucTransmissionMode;
	HBOOL						bOtherFreqFlag;
} SIxTerDeliSystemDes_t;

/** @brief eSIxDESCTAG_MULTILINGUAL_NETWORK_NAME (0x5b) */
typedef struct tagSIxMULTI_NET_NAME_INFO
{
	HUINT8								szIso639LangCode[4];
	HUINT8								*pszNetName;
	struct tagSIxMULTI_NET_NAME_INFO	*pstNext;
} SIxMultiNetNameInfo_t;

typedef struct
{
	SIxDescriptorHeader_t				stHead;
	SIxMultiNetNameInfo_t				*pstInfo;
} SIxMultiNetNameDes_t;

/** @brief eSIxDESCTAG_MULTILINGUAL_BOUQUET_NAME (0x5c) */
typedef struct tagSIxMULTI_BOUQ_NAME_INFO
{
	HUINT8								szIso639LangCode[4];
	HUINT8								*pszBouqName;
	struct tagSIxMULTI_BOUQ_NAME_INFO	*pstNext;
} SIxMultiBouqNameInfo_t;

typedef struct
{
	SIxDescriptorHeader_t				stHead;
	SIxMultiBouqNameInfo_t				*pstInfo;
} SIxMultiBouqNameDes_t;

/** @brief eSIxDESCTAG_MULTILINGUAL_SERVICE_NAME (0x5d) */
typedef struct tagSIxMultiSvcNameInfo
{
	HUINT8								szIso639LangCode[4];
	HUINT8								*pszServProviderName;
	HUINT8								*pszServName;
	struct tagSIxMultiSvcNameInfo	*pstNext;
} SIxMultiSvcNameInfo_t;

typedef struct
{
	SIxDescriptorHeader_t				stHead;
	SIxMultiSvcNameInfo_t				*pstInfo;
} SIxMultiSvcNameDes_t;

/** @brief eSIxDESCTAG_MULTILINGUAL_COMPONENT (0x5e) */
typedef struct tagSIxMultiCompInfo
{
	HUINT8							szIso639LangCode[4];
	HUINT8							*pszTextDes;
	struct tagSIxMultiCompInfo	*pstNext;
} SIxMultiCompInfo_t;

typedef struct
{
	SIxDescriptorHeader_t			stHead;
	HUINT8							ucCompTag;
	SIxMultiCompInfo_t				*pstInfo;
} SIxMultiCompDes_t;

/** @brief eSIxDESCTAG_PRIVATE_DATA_SPECIFIER (0x5f) */
typedef struct
{
	SIxDescriptorHeader_t			stHead;
	HUINT32							ulPrivateDataSpec;
} SIxPrivateDataSpecDes_t;

/** @brief eSIxDESCTAG_SERVICE_MOVE (0x60) */
typedef struct
{
	SIxDescriptorHeader_t		stHead;
	HUINT16						usNewOrgNetId;
	HUINT16						usNewTsId;
	HUINT16						usNewServId;
} SIxSvcMoveDes_t;

/** @brief eSIxDESCTAG_SHORT_SMOOTHING_BUFFER (0x61) */
typedef struct
{
	SIxDescriptorHeader_t		stHead;
	HUINT8						ucSbSize;
	HUINT8						ucSbLeakRate;
	SIxPrivateDataInfo_t		*pstPrivateDataInfo;
} SIxShortSmoothBufDes_t;

/** @brief eSIxDESCTAG_FREQUENCY_LIST (0x62) for NorDic */
typedef struct
{
	SIxDescriptorHeader_t		stHead;
	HUINT8						ucCodingType;	/** < 1-satellite, 2-cable, 3-terrestrial */
	HUINT8						ucNumFreq;
	HUINT32						*aulCentreFreq;
} SIxFrequencyListDes_t;

/** @brief in SIT Partial Transport Stream Descriptor (0x63) */
typedef struct
{
	HUINT8			ucDesTag;
	HUINT32			ulPeakRate;
	HUINT32			ulMinimumOverallSmoothingRate;
	HUINT32			ulMaximumOverallSmoothingBuffer;
} SIxPartialTsDes_t;


/** @brief eSIxDESCTAG_DATA_BROADCAST (0x64) Data BroadCast ID : 0x0006 */
typedef struct
{
	HUINT8 			ucCarouselId;
	HUINT8			ucReserved[3];
	HUINT32 		ulTransactionId;
	HUINT32			ulTimeOutValueDSI;
	HUINT32			ulTimeOutValueDII;
	HUINT32			ulLeakRate;
} SIxDataCarouselInfo_t;

typedef struct
{
 	HUINT8 	 		 ucCarouselId;
 	HUINT8	 		 ucReserved[3];
 	HUINT32  		 ulTransactionId;
 	HUINT32	 		 ulTimeOutValueDSI;
 	HUINT32	 		 ulTimeOutValueDII;
 	HUINT32	 		 ulLeakRate;
 	HUINT8	 		 szIso639LangCode[4];
	HUINT8			*pszObjName;
} SIxObjectCarouselInfo_t;

typedef struct
{
	SIxDescriptorHeader_t		stHead;
	HUINT16						usDataBroadcastId;
	HUINT8						ucCompTag;
	HUINT8						*pucSelector;
	HUINT8						szIso639LangCode[4];
	HUINT8						*pszText;
} SIxDataBroadcastDes_t;

typedef struct
{
	SIxDescriptorHeader_t		stHead;
	HUINT16						usDataBroadcastId;
	HUINT16						usSelectorLen;
	HUINT8						*pucIdSelector;
} SIxDataBroadcastIdDes_t;

/** @brief eSIxDESCTAG_PDC (0x69) */
typedef	struct
{
	SIxDescriptorHeader_t		stHead;
	HUINT8						ucMonth;
	HUINT8						ucDay;
	HUINT8						ucHour;
	HUINT8						ucMinute;
} SIxPDCDes_t;

/** @brief eSIxDESCTAG_DOLBY_AC3 (0x6A) */
typedef	struct
{
	SIxDescriptorHeader_t		stHead;
	HBOOL						bComponentTypeFlag;
	HBOOL						bBsidFlag;
	HBOOL						bMainIdFlag;
	HBOOL						bAsvcFlag;
	HUINT8						ucComponentType;
	HUINT8						ucBsId;
	HUINT8						ucMainId;
	HUINT8						ucAsvc;
} SIxDolbyAC3Des_t;

/** @brief eSIxDESCTAG_CELL_LIST (0x6C) */
typedef struct tagSIxSubCellListInfo
{
	HUINT8							 ucCellIdExtension;
	HUINT16							 usSubCellLatitude;
	HUINT16							 usSubCellLongitude;
	HUINT16							 usSubCellExtentOfLatitude;
	HUINT16							 usSubCellExtentOfLongitude;
	struct tagSIxSubCellListInfo	*pstNext;
} SIxSubCellListInfo_t;

typedef struct tagSIxCellListInfo
{
	HUINT16							usCellId;
	HUINT16							usCellLatitude;
	HUINT16							usCellLongitude;
	HUINT16							usCellExtentOfLatitude;
	HUINT16							usCellExtentOfLongitude;
	SIxSubCellListInfo_t			*pstInfo;
	struct tagSIxCellListInfo		*pstNext;
} SIxCellListInfo_t;

typedef struct
{
	SIxDescriptorHeader_t			stHead;
	SIxCellListInfo_t				*pstInfo;
} SIxCellListDes_t;


/** @brief eSIxDESCTAG_APPLICATION_SIGNALLING (0x6F) */
typedef struct tagSIxAppSignalingInfo
{
	HUINT16	 						usAppType;
	HUINT8	 						ucReserved;
	HUINT8	 						ucAitVersion;
	struct tagSIxAppSignalingInfo 	*pstNext;
} SIxAppSignallingInfo_t;

typedef struct
{
	SIxDescriptorHeader_t			stHead;
	SIxAppSignallingInfo_t			*pstInfo;
} SIxAppSignallingDes_t;

/** @brief eSIxDESCTAG_DEFAULT_AUTHORITY (0x73) */
typedef	struct
{
	SIxDescriptorHeader_t			stHead;
	HUINT8							*pszDefaultAuthority;
} SIxDefaultAuthorityDes_t;

/** @brief eSIxDESCTAG_RELATED_CONTENT (0x74) */
typedef	struct
{
	SIxDescriptorHeader_t			stHead;
//	HUINT8							*pstPrivateDataInfo; // Spec상 없지만 형식을 위해서....
	SIxPrivateDataInfo_t			*pstPrivateDataInfo;
} SIxRelatedContentDes_t;

/** @brief eSIxDESCTAG_CONTENT_IDENTIFIER (0x76) */
typedef	struct tagSIxCridInfo
{
	HUINT8					ucCridType;
	HUINT8					ucCridLocation;
	HUINT16					usCridRef;
	HUINT8					*pszCridBytes;
	struct tagSIxCridInfo	*pstNext;
} SIxCridInfo_t;

typedef	struct
{
	SIxDescriptorHeader_t	stHead;
	SIxCridInfo_t			*pstInfo;
} SIxContentIdentifierDes_t;

/** @brief eSIxDESCTAG_S2_SATELLITE_DELIVERY_SYSTEM (0x79) */
typedef struct
{
	SIxDescriptorHeader_t		stHead;
	HUINT8						ucScrambleSeqSelect;
	HUINT8						ucMultipleInStreamFlag;
	HUINT8						ucBackCompatibilityIndicator;
	HUINT8						ucReserved1;
	HUINT8						ucReserved2;
	HUINT32						ulScramblingSeqIndex;
	HUINT8						ucInputStreamIdentifier;
} SIxS2SatDeliSystemDes_t;

/** @brief eSIxDESCTAG_ENHANCED_AC3(0x7a) */
typedef struct
{
	SIxDescriptorHeader_t		stHead;
	HBOOL						bComponentTypeFlag;
	HBOOL						bBsidFlag;
	HBOOL						bMainIdFlag;
	HBOOL						bAsvcFlag;
	HBOOL						bMixinfoexists;
	HBOOL						bSubStream1Flag;
	HBOOL						bSubStream2Flag;
	HBOOL						bSubStream3Flag;
	HUINT8						ucComponentType;
	HUINT8						ucBsId;
	HUINT8						ucMainId;
	HUINT8						ucAsvc;
	HUINT8						ucSubStream1;
	HUINT8						ucSubStream2;
	HUINT8						ucSubStream3;
} SIxEnhancedAC3Des_t;

/** @brief eSIxDESCTAG_AAC(0x7C) */
typedef struct
{
	SIxDescriptorHeader_t		stHead;
	HUINT8						ucProfileAndLevel;
	HBOOL						bAACTypeFlag;
	HUINT8						ucAACType;
} SIxAACDes_t;

/** @brief eSIxDESCTAG_FTA_CONTENT_MANAGEMENT(0x7E) */
typedef struct
{
	SIxDescriptorHeader_t		stHead;
	HUINT8						ucDoNotScramble;
	HUINT8						ucCtrlRAccOverINet;
	HUINT8						ucDoNotApplyRevocation;
} SIxFtaContentManagementDes_t;

/** @brief eSIxDESCTAG_EXTENSION(0x7F)	*/
typedef struct
{
	SIxDescriptorHeader_t		 stHead;
	HUINT8			 			*pucRawData;	/** < [0] : Tag, [1] : Length */
} SIxExtensionDes_t;

#ifdef __cplusplus
}
#endif

#endif // __SILIB_DVB_H__


