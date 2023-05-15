/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _svc_cas_si_api.h
	@brief

	Description: 		\n
		[silib internal module] \n
		* si-semi parser \n
		* si callback lib \n
		* si raw section manager \n

	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__SVC_CAS_SI_API_H__
#define	__SVC_CAS_SI_API_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <isosvc.h>

#include <_svc_cas_sub_svc_api.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

#define MAX_STR_LEN			32
#if 0
#define ACT_HEAD				0
#define OTH_HEAD				1

#define ACT_PF_HEAD			0
#define OTH_PF_HEAD			1
#define ACT_SC_HEAD			2
#define OTH_SC_HEAD			3
#define OTH_PFEXT_HEAD			4
#define OTH_SCEXT_HEAD			5
#endif

#define INVALID_INDEX_HANDLE			0xffffffff

#define RCT_TIMEOUT_VALUE				60000

/* 다음 값을 16, 100 이렇게 줄여서도 테스트 했었음. */
/* sirawmgr.c 에서 이동 */
#define MAX_INDEX_HANDLE_NUM		128
//#define	MAX_SI_RAWSEC_NUM			4000

#define	MACRO_MAKE_INDEX_HANDLE(usUniqueId, usIdx)	(HUINT32)(usUniqueId << 16 | usIdx)
#define	MACRO_GET_UNIQUEID_FROM_IDXHANDLE(hIdx)		(HUINT16)(hIdx >> 16 & 0xffff)
#define	MACRO_GET_INDEX_FROM_IDXHANDLE(hIdx)		(HUINT16)(hIdx & 0xffff)

#if 0
#define EXTID_USE		0
#define EXTID_UNUSE		1
#define EXTID_PRIVATE	2

#define	MAX_SECTION_LIMIT		(unsigned) 4096
#define	MAX_DESCRIPTOR_LIMIT	(unsigned) 255

/** < ETSI EN 300 468 Annes A Table A.3 Character coding Tables */
#define eHxCP_DVBLANG_VALUE_8859_05	0x01
#define eHxCP_DVBLANG_VALUE_8859_06	0x02
#define eHxCP_DVBLANG_VALUE_8859_07	0x03
#define eHxCP_DVBLANG_VALUE_8859_08	0x04
#define eHxCP_DVBLANG_VALUE_8859_09	0x05
#define eHxCP_DVBLANG_VALUE_8859_10	0x06
#define eHxCP_DVBLANG_VALUE_8859_11	0x07
#define eHxCP_DVBLANG_VALUE_8859_12	0x08
#define eHxCP_DVBLANG_VALUE_8859_13	0x09
#define eHxCP_DVBLANG_VALUE_8859_14	0x0A
#define eHxCP_DVBLANG_VALUE_8859_15	0x0B

#define eHxCP_DVBLANG_VALUE_8859		0x10
#define eHxCP_DVBLANG_VALUE_UNICODE	0x11
#define eHxCP_DVBLANG_VALUE_KSX1001	0x12
#define eHxCP_DVBLANG_VALUE_GB_2312	0x13
#define eHxCP_DVBLANG_VALUE_BIG5		0x14
#define eHxCP_DVBLANG_VALUE_UTF_8		0x15
#endif

#define RAW_POOL_NAME_LEN		8

#define	TAG_ALL_DESCRIPTORS			0xFFFFFFFF


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/** @brief middle common의 error type */
enum
{
	ERR_CAS_SI_RESULT_OK = ERR_OK,
	ERR_CAS_SI_PARAM_NULL,					/** < param으로 넘어 온 데이터를 사용해야 할 때 그 내용이 NULL 인 경우. */
	ERR_CAS_SI_TARGET_NULL,					/** < param으로 넘어 온 주소에 내용을 채워야 할 때 그 주소가 NULL인 경우. */
	ERR_CAS_SI_SOURCE_NULL,					/** < 사용해야 할 source가 NULL 인경우. */
	ERR_CAS_SI_SOURCE_INVALID,				/** < */
	ERR_CAS_SI_RESULT_FAIL,

	/** < descriptor */
	ERR_CAS_SI_CANNOT_FIND_DESCRIPTOR,
	ERR_CAS_SI_INVALID_DESCRIPTOR,
	ERR_CAS_SI_INVALID_TAG,
	ERR_CAS_SI_ALLOC_DESCRIPTOR_FAIL,
	ERR_CAS_SI_NOT_ENOUGH_MEMORY,
	ERR_CAS_SI_ALREADY_EXIST
};


/** @brief MPEG & DVB Descriptor 구조체 정의 */

#define MACRO_IS_USER_DEFINED_DESTAG(x)			((x >= 0x80 && x <= 0xFE) ? TRUE : FALSE)
#define MACRO_IS_EXTENSION_DESTAG(x)			((x >= 0x00 && x <= 0x08) ? TRUE : FALSE)



// Service Type temp
//#define		SERVICE_VIDEO				0
#define SERVICE_TYPE_DIGITAL_TV		0x01
#define SERVICE_TYPE_DIGITAL_RADIO	0x02
#define SERVICE_TYPE_TELETEXT		0x03
#define SERVICE_TYPE_NVOD_REFERENCE	0x04
#define SERVICE_TYPE_NVOD_TIMESHIFT	0x05
#define SERVICE_TYPE_MOSAIC			0x06
#define SERVICE_TYPE_PAL			0x07
#define SERVICE_TYPE_SECAM			0x08
#define SERVICE_TYPE_DD2_MAC		0x09
#define SERVICE_TYPE_FM_RADIO		0x0A
#define SERVICE_TYPE_NTSC			0x0B
#define SERVICE_TYPE_DATA_BROADCAST	0x0C
#define SERVICE_TYPE_DVB_MHP		0x10

#define	SERVICE_TYPE_MPEG2_HD					0x11
#define	SERVICE_TYPE_MPEG2_HD_NVOD_TIMESHIFT	0x12
#define	SERVICE_TYPE_MPEG2_HD_NVOD_REFERENCE	0x13
#define	SERVICE_TYPE_AAC_AUDIO					0x14
#define	SERVICE_TYPE_AAC_V2_AUDIO				0x15
#define	SERVICE_TYPE_AVC_SD_DIGITAL_TV			0x16
#define	SERVICE_TYPE_AVC_SD_NVOD_TIMESHIFT		0x17
#define	SERVICE_TYPE_AVC_SD_NVOD_REFERENCE		0x18
#define	SERVICE_TYPE_AVC_HD_DIGITAL_TV			0x19
#define	SERVICE_TYPE_AVC_HD_NVOD_TIMESHIFT		0x1A
#define	SERVICE_TYPE_AVC_HD_NVOD_REFERENCE		0x1B
#define	SERVICE_TYPE_AVC_HD_DIGITAL_TV2			0x86
#define	SERVICE_TYPE_SUBFEED						0xD3

typedef struct
{
	HUINT8		ucDesTag;
} svcCas_SiBaseDesForm_t;

/** @brief raw section을 관리하는 구조체 */

#define NUM_SEC_FOR_RECEIVING_TABLE				16

typedef struct tagCAS_RECEIVING_RAW_SEC
{
	HBOOL		 bSecCompleted;					// 하나의 table에 해당하는 모든 section (0 ~ last_sec_number)를 다 받았는가?
	HUINT32		 ulCrc32;						// Section 0 의 CRC32
	HUINT16		 usTsUniqId;					// TS Idx
	HUINT16		 usExtId1, usExtId2;			// Table을 구분하기 위한 EXT ID 2개
	HUINT8		 ucVer;							// Section versions:
	HUINT8		 ucStartSecNum, ucEndSecNum;
	HUINT8		 ucLastSecNum;					// 에 기록되어 있는 last_section_number
	HUINT8		*apucRawSec[NUM_SEC_FOR_RECEIVING_TABLE];

	struct tagCAS_RECEIVING_RAW_SEC	*pstNext;
} svcCas_RawReceiving_t;

typedef struct
{
	SiTableType_e				 eSiTableType;
	HUINT16						 usSecNum;				// The number of the section in the index array
	HUINT16						 usMaxSecNum;			// The length of the phIdx : the maximum number of the section stored in the current array
	HUINT16						 usStartPos;
	HUINT16						 usInsertPos;			// The position for the section to be inserted in the next time
	HUINT32						*phIdx;					// index handle = unique_id + rawsec_index
	svcCas_RawReceiving_t		*pstReceivingTable;
	svcCas_RawReceiving_t		*pstFreeRcvTable;
} svcCas_RawSecTable_t;

typedef struct
{
	HUINT8			  szPoolName[RAW_POOL_NAME_LEN];
	svcCas_RawSecTable_t	  astSiTable[MAX_SI_TABLE_TYPE];

	HUINT16			  usLastEmptySecIdx;
	HUINT16			  usMaxNumRawSec;
	HUINT8			**ppucRawSec;

	// 2012.11.24 : jinh
	// MWC Raw Pool 에 각종 기능 (Private Descriptor Parsing, OP Type, Section Synthesize 등)을 할 수 있도록
	// 일단 Section Making 부터 하도록 한다.
	void			 *pfSectionMaker;			// svcCas_RawMakeSecCb_t
} svcCas_RawPool_t;


typedef struct
{
	svcCas_RawPool_t	*pstPool;

	SiTableType_e		 eTableType;
	HUINT16				 usTsUniqId;

	HBOOL				 bUseExtId;
	HUINT16				 usExtId;

	HUINT16				 usCurrIdx;
} svcCas_RawSecBookmark_t;

typedef struct
{
	HINT32		 nLeftDesLoop;
	HUINT8		*pucCurrPos;
} svcCas_SiDesIndex_t;

typedef struct
{
	HINT32		 nLeftInfoLoop;
	HUINT8		*pucCurrPos;
} svcCas_SiInfoIndex_t;

/*
	Section Composing (Making) 기능을 위한 설정들
*/
typedef struct
{
	SiTableType_e		 eSiType;
	HUINT16				 usTsId;
	HUINT16				 usSvcId;
	HUINT16				 usPmtPid;
} svcCas_RawMakeSecArgPat_t;

typedef enum
{
	eCAS_SI_MakePmtSrcType_SiSections,				// SI Filter를 걸어 받은 SI Raw Section들을 통해 합성한다. 기본적으로 모든 PID는 다 기록 (PVR REC용)
	eCAS_SI_MakePmtSrcType_SetByArgs,				// PID, CODEC 정보 등을 다 MakeSection을 부르는 곳에서 정해서 주도록 한다.

	eCAS_SI_MakePmtSrcType_Transcode,				// Transcode에 맞는 PMT를 만든다. Audio, Video StreamType 변경 등 수행
	eCAS_SI_MakePmtSrcType_EndOfCase
} svcCas_SiMakeSecArgPmtSrcType_e;

typedef struct
{
	HUINT32				 ulReserved;
} svcCas_RawMakeSecPmtSectionArg_t;	// SI Filter를 걸어 받은 SI Raw Section들을 통해 합성한다. 기본적으로 모든 PID는 다 기록 (PVR REC용)

typedef struct
{
	HUINT16				 usPcrPid;

	HUINT8				 ucVideoStreamType;		// ISO_VIDEO_1 if MPEG2, ISO_VIDEO_4 if H.264/AVC
	HUINT16				 usVideoPid;

	HUINT8				 ucAudioStreamType;		// ISO_AUDIO_1 if MPEG Audio,  ISO_AUDIO_4 if MPEG4 AAC
	HUINT16				 usAudioPid;

} svcCas_RawMakeSecPmtSettingArg_t;	// PID, CODEC 정보 등을 다 MakeSection을 부르는 곳에서 정해서 주도록 한다.

typedef struct
{
	SiTableType_e		 eSiType;
	Handle_t			 hSvc;
	HUINT16				 usTsUniqId;
	HUINT16				 usSvcId;

	svcCas_SiMakeSecArgPmtSrcType_e		 eMakeSrcType;
	union
	{
		svcCas_RawMakeSecPmtSectionArg_t	 stSectionArg;
		svcCas_RawMakeSecPmtSettingArg_t	 stSettingArg;
	} unArgs;
} svcCas_RawMakeSecArgPmt_t;

typedef struct
{
	SiTableType_e		 eSiType;
	Handle_t			 hSvc;
	HUINT16				 usTsUniqId;
	HUINT16				 usOnId, usTsId, usSvcId;
} svcCas_RawMakeSecArgSit_t;

typedef union
{
	SiTableType_e				 eSiType;
	svcCas_RawMakeSecArgPat_t	 stPatArg;
	svcCas_RawMakeSecArgPmt_t	 stPmtArg;
	svcCas_RawMakeSecArgSit_t	 stSitArg;
} svcCas_RawMakeSecArg_t;

typedef HERROR (*svcCas_RawMakeSecCb_t) (svcCas_RawPool_t *pstPool, svcCas_RawMakeSecArg_t *punArg, HUINT8 *pucRawSec, HUINT32 *pulSecLen);


typedef struct
{
	HUINT16		usProgNum;
	HUINT16		usProgMapPid;
} svcCas_SiPatProgInfo_t;

typedef struct
{
	HUINT16	usTsId;
	HUINT8	ucVer;
	HUINT32	ulCrc32;
} svcCas_SiPatSectionInfo_t;

typedef struct
{
	HUINT16 usProgNum;
	HUINT8	ucVer;
	HBOOL	bCurNextInd;
	HUINT16 usPCRPid;
	HUINT32 ulCrc32;
} svcCas_SiPmtProgramInfo_t;

typedef struct
{
	HUINT16 usSvcId;
	HBOOL	bEitSched;
	HBOOL	bEitPreFollow;
	HUINT8	ucRunningStatus;
	HBOOL	bCaFree;
} svcCas_SiSdtServiceInfo_t;

// Descriptors
#if 1
/** @brief Private Data 구조 */
typedef struct tagPRIVATE_DATA_INFO
{
	HUINT8	ucPrivateDataLen;
	HUINT8	*pPrivateData;
} svcCas_PrivateDataInfo_t, *pPrivateDataInfo_t;

/** @brief TAG_VIDEO_STREAM (0x02) */
typedef struct tagVIDEO_STM_DES
{
	HUINT8	ucDesTag;
	HBOOL	bMultiFrameRate;
	HUINT8	ucFrameRateCode;
	HBOOL	bMPEG2;				/** < if TRUE then, MPEG1. else MPEG2 */
	HBOOL	bConstrainParam;
	HBOOL	bStillPic;
} svcCas_VideoStmDes_t, *pVideoStmDes_t;

/** @brief TAG_AUDIO_STREAM (0x03) */
typedef struct tagAUDIO_STM_DES
{
	HUINT8	ucDesTag;
	HBOOL	bFreeFormat;
	HUINT8	ucId;
	HUINT8	ucLayer;
	HUINT8	ucVarRateAudioIndicator;
} svcCas_AudioStmDes_t, *pAudioStmDes_t;

/** @brief TAG_HIERARCHY (0x04) */
typedef struct tagHIER_DES
{
	HUINT8	ucDesTag;
	HUINT8	ucHierType;
	HUINT8	ucHierLayerIndex;
	HUINT8	ucHierEmbedLayerIndex;
	HUINT8	ucHierChannel;
} svcCas_HierDes_t, *pHierDes_t;

/** @brief TAG_REGISTRATION (0x05) */
typedef struct tagREG_DES
{
	HUINT8	ucDesTag;
	HUINT32	ulFormatIdent;
	struct	tagPRIVATE_DATA_INFO	*pPrivateDataInfo;
} svcCas_RegDes_t, *pRegDes_t;

/** @brief TAG_DATA_STREAM_ALIGNMENT (0x06) */
typedef struct tagDATA_STM_ALIGN_DES
{
	HUINT8	ucDesTag;
	HUINT8	ucAlignType;
} svcCas_DataStmAlignDes_t, *pDataStmAlignDes_t;

/** @brief TAG_TARGET_BACKGROUND_GRID (0x07) */
typedef struct tagBACK_GRID_DES
{
	HUINT8	ucDesTag;
	HUINT16	usHoriSize;
	HUINT16	usVertSize;
	HUINT8	ucAspectRatio;
} svcCas_BackGridDes_t, *pBackGridDes_t;

/** @brief TAG_VIDEO_WINDOW (0x08) */
typedef struct tagVIDEO_WINDOW_DES
{
	HUINT8	ucDesTag;
	HUINT16	usHoriOff;
	HUINT16	usVertOff;
	HUINT8	ucWindowPriority;
} svcCas_VideoWindowDes_t, *pVideoWindowDes_t;

/** @brief TAG_CA (0x09) */
typedef struct tagCA_DES
{
	HUINT8	ucDesTag;
	HUINT16	usCaSysId;
	HUINT16	usCaPid;
	struct	tagPRIVATE_DATA_INFO	*pPrivateDataInfo;
} svcCas_CaDes_t, *pCaDes_t;

/** @brief TAG_ISO_639_LANGUAGE (0x0a) */
typedef struct tagLANG_INFO
{
	HUINT8	ucDesTag;
	HUINT8	szIso639LangCode[4];
	HUINT8	ucAudioType;
	struct	tagLANG_INFO	*pNextInfo;
} svcCas_LangInfo_t, *pLangInfo_t;

typedef struct tagISO_639_LANG_DES
{
	HUINT8	ucDesTag;
	struct	tagLANG_INFO	*pLangInfo;
} svcCas_Iso639LangDes_t, *pIso639LangDes_t;

/** @brief TAG_SYSTEM_CLOCK (0x0b) */
typedef struct tagSYS_CLOCK_DES
{
	HUINT8	ucDesTag;
	HUINT8	ucExtClockRef;
	HUINT8	ucClockAccuInt;
	HUINT8	ucClockAccuExp;
} svcCas_SysClockDes_t, *pSysClockDes_t;

/** @brief TAG_MULTIPLEX_BUFFER_UTILIZATION (0x0c) */
typedef struct tagMULTIPLEX_BUF_UTIL_DES
{
	HUINT8	ucDesTag;
	HBOOL	bBoundValid;
	HUINT16	usLtwOffLowBound;
	HUINT16	usLtwOffUpBound;
} svcCas_MultiplexBufUtilDes_t, *pMultiplexBufUtilDes_t;

/** @brief TAG_COPYRIGHT (0x0d) */
typedef struct tagCOPYRIGHT_DES
{
	HUINT8	ucDesTag;
	HUINT32	ulCopyrightIdent;
	struct	tagPRIVATE_DATA_INFO	*pPrivateDataInfo;
} svcCas_CopyRightDes_t, *pCopyRightDes_t;

/** @brief TAG_MAXIMUM_BITRATE (0x0e) */
typedef struct tagMAX_BITRATE_DES
{
	HUINT8	ucDesTag;
	HUINT32	ulMaxBitrate;
} svcCas_MaxBitrateDes_t, *pMaxBitrateDes_t;

/** @brief TAG_PRIVATE_DATA_INDICATOR (0x0f) */
typedef struct tagPRIVATE_DATA_INDICATOR_DES
{
	HUINT8	ucDesTag;
	HUINT32	ulPrivateDataIndicatorDes;
} svcCas_PrivateDataIndicatorDes_t, *pPrivateDataIndicatorDes_t;

/** @brief TAG_SMOOTHING_BUFFER (0x10) */
typedef struct tagSMOOTH_BUF_DES
{
	HUINT8	ucDesTag;
	HUINT32	ulLeakRate;
	HUINT32	ulSize;
} svcCas_SmoothBufDes_t, *pSmoothBufDes_t;

/** @brief TAG_STD (0x11) */
typedef struct tagSTD_DES
{
	HUINT8	ucDesTag;
	HBOOL	bLeakValid;
} svcCas_StdDes_t, *pStdDes_t;

/** @brief TAG_CAROUSEL_ID (0x13) */
typedef struct tagCAROUSEL_ID_DES_INFO
{
	HUINT8 	ucModuleVersion;
	HUINT16 usModuleId;
	HUINT16 usBlockSize;
	HUINT32 ulModuleSize;
	HUINT8 	ucCompressionMethod;
	HUINT32 ulOriginalSize;
	HUINT8 	ucTimeout;
	HUINT8 	aucObjectKeyData[4];
} svcCas_CarouselIdDesInfo_t, *pCarouselIdDesInfo_t;

typedef struct tagCAROUSEL_ID_DES
{
	HUINT8	ucDesTag;
	HUINT32 ulCarouselId;
	HUINT8 	ucFormatId;/** < 0x00 or 0x01 */

	/** < only if ucFormatId == 0x01, pstCarouselIdDesInfo is valid.
		otherwise, pstCarouselIdDesInfo == NULL */
	svcCas_CarouselIdDesInfo_t *pCarouselIdDesInfo;
} svcCas_CarouselIdDes_t, *pCarouselIdDes_t;

/** @brief TAG_ASSOCITION_TAG (0x14) */
typedef struct tagASSOC_TAG_DES
{
	HUINT8	ucDesTag;
	HUINT16 usAssocTag;
	HUINT16 usUse;	/** < 0x00, 0x01, or otherwise */

	/** < following 2 fields are valid only if usUse == 0x00
		otherwise, don't care about them.*/
	HUINT32 ulTransactionId;
	HUINT32 ulTimeout;
} svcCas_AssocTagDes_t, *pAssocTagDes_t;

/** @brief TAG_DEFERED_ASSOCIATION_TAG (0x15) */
typedef struct tagDEFER_ASSOC_TAG_DES
{
	HUINT8	ucDesTag;
	HUINT8	ucAssocTagSize;	/** < #. of assoc_tag */
	HUINT16 *pusAssocTags;		/** < array of assoc_tag */

	/** < triple ID */
	HUINT16 usTransportStreamId;
	HUINT16 usProgNum;
	HUINT16 usOriginalNetworkId;
} svcCas_DeferAssocTagDes_t, *pDeferAssocTagDes_t;

typedef struct tagAVC_VIDEO_DES
{
	HUINT8	ucDesTag;
	HUINT8	ucProfile;					/** < profile_idc */
	HBOOL	bConstSet0Flag;			/** < constraint_set0_flag */
	HBOOL	bConstSet1Flag;			/** < constraint_set1_flag */
	HBOOL	bConstSet2Flag;			/** < constraint_set2_flag */
	HUINT8	ucAvcCompatibleFlags;		/** < avc_compatible_flags */
	HUINT8	ucLevel;					/** < level_idc */
	HUINT8	ucAvcStillPresent;		/** < avc_still_present */
	HBOOL	bAvc24hourPicFlag;		/** < vc_24_hour_picture_flag */
} svcCas_AvcVideoDes_t, *pAvcVideoDes_t;

/****************************************************/
/*     												*/
/*     DVB 정의한 Descriptor 구조체(ETS 300 468).  	*/
/*       											*/
/****************************************************/
/** @brief TAG_NETWORK_NAME (0x40) */
typedef struct tagNET_NAME_DES
{
	HUINT8	ucDesTag;
	HUINT8	*pszNetName;
} svcCas_NetNameDes_t, *pNetNameDes_t;

/** @brief TAG_SERVICE_LIST (0x41) */
typedef struct tagSERV_LIST_INFO
{
	HUINT8	ucDesTag;
	HUINT16	usSvcId;
	HUINT8	ucServType;
	struct	tagSERV_LIST_INFO	*pNextInfo;
} svcCas_SvcListInfo_t, *pSvcListInfo_t;

typedef struct tagSERV_LIST_DES
{
	HUINT8	ucDesTag;
	struct	tagSERV_LIST_INFO	*pServListInfo;
} svcCas_SvcListDes_t, *pSvcListDes_t;

/** @brief TAG_SATELLITE_DELIVERY_SYSTEM (0x43) */
typedef struct tagSAT_DELI_SYSTEM_DES {
	HUINT8	ucDesTag;
	HUINT32	ulFreq;
	HUINT16	usOrbitPos;
	HBOOL	bWestEastFlag;
	HUINT8	ucPolar;
	HUINT8	ucModul;
	HUINT8	ucModType;
	HUINT32	ulSymbolRate;
	HUINT8	ucFECInner;
	/* DVB S2 */
	HUINT8	ucTransSpec;
	HUINT8	ucRollOff;
} svcCas_SatDeliSystemDes_t, *pSatDeliSystemDes_t;

/** @brief TAG_CABLE_DELIVERY_SYSTEM (0x44) */
typedef struct tagCAB_DELI_SYSTEM_DES
{
	HUINT8	ucDesTag;
	HUINT32	ulFreq;
	HUINT8	ucFECOuter;
	HUINT8	ucModul;
	HUINT32	ulSymbolRate;
	HUINT8	ucFECInner;
} svcCas_CabDeliSystemDes_t, *pCabDeliSystemDes_t;

/** @brief TAG_BOUQUET_NAME (0x47) */
typedef struct tagBOUQ_NAME_DES
{
	HUINT8	ucDesTag;
	HUINT8	*pszBouqName;
} svcCas_BouqNameDes_t, *pBouqNameDes_t;

/** @brief TAG_SERVICE (0x48) */
typedef struct tagSERV_DES
{
	HUINT8	ucDesTag;
    HUINT8	ucServType;
    HUINT8	nPrvName;
    HUINT8	*pszProviderName;
    HUINT8	nSvcName;
    HUINT8	*pszServName;
} svcCas_SvcDes_t, *pSvcDes_t;

/** @brief TAG_COUNTRY_AVAILABILITY (0x49) */
typedef struct tagCOUNTRY_AVAIL_INFO
{
	HUINT8	szCountryAvailCode[4];
	struct	tagCOUNTRY_AVAIL_INFO	*pNextInfo;
} svcCas_CountryAvailInfo_t, *pCountryAvailInfo_t;

typedef struct tagCOUNTRY_AVAIL_DES
{
	HUINT8	ucDesTag;
	HBOOL	bCountryAvail;
	struct	tagCOUNTRY_AVAIL_INFO	*pCountryAvailInfo;
} svcCas_CountryAvailDes_t, *pCountryAvailDes_t;

typedef struct tagLINK_DES
{
	HUINT8	ucDesTag;
	HUINT16		usTsId;
	HUINT16		usOrgNetId;
	HUINT16		usSvcId;
	HUINT8		ucLinkType;		// Link Type 이후의 모든 Private Data Info는 받은 곳에서 직접 parsing 하도록 한다.

	struct	tagPRIVATE_DATA_INFO	*pPrivateDataInfo;
} svcCas_LinkDes_t, *pLinkDes_t;

/** @brief TAG_NVOD_REFERENCE (0x4b) */
typedef struct tagNVOD_REF_INFO
{
	HUINT16	usTsId;
	HUINT16	usOrgNetId;
	HUINT16	usSvcId;
	struct	tagNVOD_REF_INFO	*pNextInfo;
} svcCas_NvodRefInfo_t, *pNvodRefInfo_t;

typedef struct tagNVOD_REF_DES
{
	HUINT8	ucDesTag;
	struct	tagNVOD_REF_INFO	*pNvodRefInfo;
} svcCas_NvodRefDes_t, *pNvodRefDes_t;

/** @brief TAG_TIME_SHIFTED_SERVICE (0x4c) */
typedef struct tagTIME_SHIFT_SERV_DES {
	HUINT8	ucDesTag;
	HUINT16	usRefSvcId;
} svcCas_TimeShiftSvcDes_t, *pTimeShiftSvcDes_t;

/** @brief TAG_SHORT_EVENT (0x4d) */
typedef struct tagSHORT_EVT_DES
{
	HUINT8	ucDesTag;
	HUINT8	szIso639LangCode[4];
	HUINT8	ucEvtNameSize;
	HUINT8	*pszEvtName;
	HUINT8	ucTextSize;
	HUINT8	*pszText;
} svcCas_ShortEventDes_t, *pShortEventDes_t;

/** @brief TAG_EXTENDED_EVENT (0x4e) */
typedef struct tagEXT_EVT_INFO
{
	HUINT8	*pszDescription;
	HUINT8	*pszItem;

	struct	tagEXT_EVT_INFO		*pNextInfo;
} svcCas_ExtEvtInfo_t, *pExtEvtInfo_t;

typedef struct tagEXT_EVT_DES
{
	HUINT8	ucDesTag;
	HUINT8	ucDesNum;
	HUINT8	ucLastDesNum;
	HUINT8	szIso639LangCode[4];
	struct	tagEXT_EVT_INFO		*pExtEvtInfo;
	HUINT8	*pszChar;		// 이거... 다 합친 후 UTF-8로 변환해야 해서, 사용하면 안되지만, 일단 그냥 둠
	HUINT8	ucTextLength;
	HUINT8	*pucTextChar;
} svcCas_ExtEvtDes_t, *pExtEvtDes_t;

/** @brief TAG_TIME_SHIFTED_EVENT (0x4f) */
typedef struct tagTIME_SHIFT_EVT_DES {
	HUINT8	ucDesTag;
	HUINT16	usRefSvcId;
	HUINT16	usRefEvtId;
} svcCas_TimeShiftEvtDes_t, *pTimeShiftEvtDes_t;

/** @brief TAG_COMPONENT (0x50) */
typedef struct tagCOMP_DES
{
	HUINT8	ucDesTag;
	HUINT8	ucStmContent;
	HUINT8	ucCompType;
	HUINT8	ucCompTag;
	HUINT8	szIso639LangCode[4];
	HUINT8	*pszText;
} svcCas_CompDes_t, *pCompDes_t;

typedef struct tagMOSAIC_DES
{
	HUINT8	ucDesTag;
	HUINT8	ucEntryPoint;
	HUINT8	ucHoriElemCellNum;
	HUINT8	ucVertElemCellNum;
	struct	tagMOSAIC_INFO	*pCellInfo;
} svcCas_MosaicDes_t, *pMosaicDes_t;

/** @brief TAG_STREAM_IDENTIFIER (0x52) */
typedef struct tagSTRM_IDENT_DES
{
	HUINT8	ucDesTag;
	HUINT8	ucCompTag;
} svcCas_StmIdentDes_t, *pStmIdentDes_t;

/** @brief TAG_CA_IDENTIFIER (0x53) */
typedef struct tagCA_IDENT_INFO
{
	HUINT8	ucDesTag;
	HUINT16	usCaStmId;
	struct	tagCA_IDENT_INFO	*pNextInfo;
} svcCas_CaIdentInfo_t, *pCaIdentInfo_t;

typedef struct tagCA_IDENT_DES
{
	HUINT8	ucDesTag;
	struct	tagCA_IDENT_INFO	*pCaIdentInfo;
} svcCas_CaIdentDes_t, *pCaIdentDes_t;

/** @brief TAG_CONTENT (0x54) */
typedef struct tagCONTENT_INFO
{
	HUINT8	ucContentNibble;
	HUINT8	ucUserNibble;
	struct	tagCONTENT_INFO		*pNextInfo;
} svcCas_ContentInfo_t, *pContentInfo_t;

typedef struct tagCONTENT_DES
{
	HUINT8	ucDesTag;
	struct	tagCONTENT_INFO		*pContentInfo;
} svcCas_ContentDes_t, *pContentDes_t;

/** @brief TAG_PARENTAL_RATING (0x55) */
typedef struct tagPARENT_INFO
{
	HUINT8	szCountryCode[4];
	HUINT8	ucRate;
	struct	tagPARENT_INFO	*pNextInfo;
} svcCas_ParentInfo_t, *pParentInfo_t;

typedef struct tagPARENT_DES
{
	HUINT8	ucDesTag;
	struct	tagPARENT_INFO	*pParentInfo;
} svcCas_ParentDes_t, *pParentDes_t;

/** @brief TAG_TELETEXT (0x56) */
typedef struct tagTELETEXT_INFO
{
	HUINT8	szIso639LangCode[4];
	HUINT8	ucType;
	HUINT8	ucMagazineNum;
	HUINT8	ucPageNum;
	struct	tagTELETEXT_INFO	*pNextInfo;
} svcCas_TeletextInfo_t, *pTeletextInfo_t;

typedef struct tagTELETEXT_DES
{
	HUINT8	ucDesTag;
	struct	tagTELETEXT_INFO	*pTeletextInfo;
} svcCas_TeletextDes_t, *pTeletextDes_t;

/** @brief TAG_TELEPHONE (0x57) */
typedef struct tagTELEPHONE_DES
{
	HUINT8	ucDesTag;
	HUINT8	ucForeignAvail;
	HUINT8	ucConnectionType;
	HUINT8	*pszCountryPrefix;
	HUINT8	*pszInternationalAreaCode;
	HUINT8	*pszOperatorCode;
	HUINT8	*pszNationalAreaCode;
	HUINT8	*pszCoreNumber;
} svcCas_TelephoneDes_t, *pTelephoneDes_t;

/** @brief TAG_LOCAL_TIME_OFFSET (0x58) */
typedef struct tagLOCAL_TIME_INFO
{
	HUINT8	szCountryCode[4];
	HUINT8	ucRegionId;
	HUINT8	ucLocalTimeOffPolar;
	HUINT8	pucLocalTimeOff[2];
	UNIXTIME timeOfChange;
	HUINT8	pucNextTimeOffset[2];
	struct	tagLOCAL_TIME_INFO	*pNextInfo;
} svcCas_LocalTimeInfo_t, *pLocalTimeInfo_t;

typedef struct tagLOCAL_TIME_DES
{
	HUINT8	ucDesTag;
	struct	tagLOCAL_TIME_INFO	*pLocalTimeInfo;
} svcCas_LocalTimeDes_t, *pLocalTimeDes_t;

/** @brief TAG_SUBTITLING (0x59) */
typedef struct tagSUB_TITLE_INFO
{
	HUINT8	szIso639LangCode[4];
	HUINT8	ucSubTitleType;
	HUINT16	usCompositionPageId;
	HUINT16	usAncillaryPageId;
	struct	tagSUB_TITLE_INFO	*pNextInfo;
} svcCas_SubTitleInfo_t, *pSubTitleInfo_t;

typedef struct tagSUB_TITLE_DES
{
	HUINT8	ucDesTag;
	struct	tagSUB_TITLE_INFO	*pSubTitleInfo;
} svcCas_SubTitleDes_t, *pSubTitleDes_t;

/** @brief TAG_TERRESTRIAL_DELIVERY_SYSTEM (0x5a) */
typedef	struct	tagTER_DELI_SYSTEM_DES
{
	HUINT8	ucDesTag;
	HUINT32	ulFreq;
	HUINT8	ucBandWidth;
	HUINT8	ucPriority;
	HUINT8	ucConstellation;
	HUINT8	ucHierInfo;
	HUINT8	ucCodeRateHPStream;
	HUINT8	ucCodeRateLPStream;
	HUINT8	ucGuardInterval;
	HUINT8	ucTransmissionMode;
	HBOOL	bOtherFreqFlag;
} svcCas_TerDeliSystemDes_t, *pTerDeliSystemDes_t;

/** @brief TAG_MULTILINGUAL_NETWORK_NAME (0x5b) */
typedef struct tagMULTI_NET_NAME_INFO
{
	HUINT8	szIso639LangCode[4];
	HUINT8	*pszNetName;
	struct	tagMULTI_NET_NAME_INFO	*pNextInfo;
} svcCas_MultiNetNameInfo_t, *pMultiNetNameInfo_t;

typedef struct tagMULTI_NET_NAME_DES
{
	HUINT8	ucDesTag;
	struct	tagMULTI_NET_NAME_INFO	*pMultiNetNameInfo;
} svcCas_MultiNetNameDes_t, *pMultiNetNameDes_t;

/** @brief TAG_MULTILINGUAL_BOUQUET_NAME (0x5c) */
typedef struct tagMULTI_BOUQ_NAME_INFO
{
	HUINT8	szIso639LangCode[4];
	HUINT8	*pszBouqName;
	struct	tagMULTI_BOUQ_NAME_INFO		*pNextInfo;
} svcCas_MultiBouqNameInfo_t, *pMultiBouqNameInfo_t;

typedef struct tagMULTI_BOUQ_NAME_DES
{
	HUINT8	ucDesTag;
	struct	tagMULTI_BOUQ_NAME_INFO		*pMultiBouqNameInfo;
} svcCas_MultiBouqNameDes_t, *pMultiBouqNameDes_t;

/** @brief TAG_MULTILINGUAL_SERVICE_NAME (0x5d) */
typedef struct tagMULTI_SERV_NAME_INFO
{
	HUINT8	szIso639LangCode[4];
	HUINT8	*pszServProviderName;
	HUINT8	*pszServName;
	struct	tagMULTI_SERV_NAME_INFO		*pNextInfo;
} svcCas_MultiSvcNameInfo_t, *pMultiSvcNameInfo_t;

typedef struct tagMULTI_SERV_NAME_DES
{
	HUINT8	ucDesTag;
	struct	tagMULTI_SERV_NAME_INFO		*pMultiServNameInfo;
} svcCas_MultiSvcNameDes_t, *pMultiSvcNameDes_t;

/** @brief TAG_MULTILINGUAL_COMPONENT (0x5e) */
typedef struct tagMULTI_COMP_INFO
{
	HUINT8	szIso639LangCode[4];
	HUINT8	*pszTextDes;
	struct	tagMULTI_COMP_INFO	*pNextInfo;
} svcCas_MultiCompInfo_t, *pMultiCompInfo_t;

typedef struct tagMULTI_COMP_DES
{
	HUINT8	ucDesTag;
	HUINT8	ucCompTag;
	struct	tagMULTI_COMP_INFO	*pMultiCompInfo;
} svcCas_MultiCompDes_t, *pMultiCompDes_t;

/** @brief TAG_PRIVATE_DATA_SPECIFIER (0x5f) */
typedef struct tagPRIVATE_DATA_SPEC_DES
{
	HUINT8	ucDesTag;
	HUINT32	ulPrivateDataSpec;
} svcCas_PrivateDataSpecDes_t, *pPrivateDataSpecDes_t;

/** @brief TAG_SERVICE_MOVE (0x60) */
typedef struct tagSERV_MOVE_DES
{
	HUINT8	ucDesTag;
	HUINT16	usNewOrgNetId;
	HUINT16	usNewTsId;
	HUINT16	usNewServId;
} svcCas_SvcMoveDes_t, *pSvcMoveDes_t;

/** @brief TAG_SHORT_SMOOTHING_BUFFER (0x61) */
typedef struct tagSHORT_SMOOTH_BUF_DES
{
	HUINT8	ucDesTag;
	HUINT8	ucSbSize;
	HUINT8	ucSbLeakRate;
	struct	tagPRIVATE_DATA_INFO	*pPrivateDataInfo;
} svcCas_ShortSmoothBufDes_t, *pShortSmoothBufDes_t;

/** @brief TAG_FREQUENCY_LIST (0x62) for NorDic */
typedef struct tagTAG_FREQUENCY_LIST {
	HUINT8	ucDesTag;
	HUINT8	codingType;	/** < 1-satellite, 2-cable, 3-terrestrial */
	HUINT8	numFreq;
	HUINT32 *centreFreq;
} svcCas_FrequencyListDes_t;

/** @brief TAG_DATA_BROADCAST (0x64) Data BroadCast ID : 0x0006 */
typedef struct tagDATA_CAROUSEL_INFO
{
	HUINT8 	ucCarouselId;
	HUINT8	ucReserved[3];
	HUINT32 ulTransactionId;
	HUINT32	ulTimeOutValueDSI;
	HUINT32	ulTimeOutValueDII;
	HUINT32	ulLeakRate;
} svcCas_DataCarouselInfo_t, *pDataCarouselInfo_t;

typedef struct tagOBJECT_CAROUSEL_INFO
{
	HUINT8 	ucCarouselId;
	HUINT8	ucReserved[3];
	HUINT32 ulTransactionId;
	HUINT32	ulTimeOutValueDSI;
	HUINT32	ulTimeOutValueDII;
	HUINT32	ulLeakRate;
	HUINT8	szIso639LangCode[4];
	HUINT8	*pszObjName;
} svcCas_ObjectCarouselInfo_t, *pObjectCarouselInfo_t;

typedef struct tagDATA_BROADCAST_DES
{
	HUINT8	ucDesTag;
	HUINT16	usDataBroadcastId;
	HUINT8	ucCompTag;
	HUINT8	*pszSelector;
	HUINT8	szIso639LangCode[4];
	HUINT8	*pszText;
} svcCas_DataBroadcastDes_t, *pDataBroadcastDes_t;

typedef struct tagDATA_BROADCAST_ID_DES
{
	HUINT8	ucDesTag;
	HUINT16	usDataBroadcastId;
	HUINT16 usSelectorLen;
	HUINT8	*pszIdSelector;
} svcCas_DataBroadcastIdDes_t, *pDataBroadcastIdDes_t;

/** @brief TAG_PDC (0x69) */
typedef	struct tagPDC_DES
{
	HUINT8	ucDesTag;
	HUINT8		ucMonth;
	HUINT8		ucDay;
	HUINT8		ucHour;
	HUINT8		ucMinute;
} svcCas_PDCDes_t, *pPDCDes_t;

/** @brief TAG_DOLBY_AC3 (0x6a) */
typedef	struct tagDOLBY_AC3_DES
{
	HUINT8	ucDesTag;
	HBOOL	bComponentTypeFlag;
	HBOOL	bBsidFlag;
	HBOOL	bMainIdFlag;
	HBOOL	bAsvcFlag;
	HUINT8	ucComponentType;
	HUINT8	ucBsId;
	HUINT8	ucMainId;
	HUINT8	ucAsvc;
} svcCas_DolbyAc3Des_t, *pDolbyAc3Des_t;

/** @brief TAG_CELL_LIST (0x6a) */
typedef struct tagSUBCELL_LIST_INFO
{
	HUINT8	ucCellIdExtension;
	HUINT16	usSubCellLatitude;
	HUINT16	usSubCellLongitude;
	HUINT16	usSubCellExtentOfLatitude;
	HUINT16	usSubCellExtentOfLongitude;
	struct	tagSUBCELL_LIST_INFO	*pNextInfo;
} svcCas_SubCellListInfo_t, *pSubCellListInfo_t;

typedef struct tagCELL_LIST_INFO
{
	HUINT16	usCellId;
	HUINT16	usCellLatitude;
	HUINT16 usCellLongitude;
	HUINT16	usCellExtentOfLatitude;
	HUINT16	usCellExtentOfLongitude;
	svcCas_SubCellListInfo_t	*pSubCellListInfo;
	struct	tagCELL_LIST_INFO	*pNextInfo;
} svcCas_CellListInfo_t, *pCellListInfo_t;

typedef struct tagCELL_LIST_DES
{
	HUINT8	ucDesTag;
	svcCas_CellListInfo_t *pCellListInfo;
} svcCas_CellListDes_t, *pCellListDes_t;


/** @brief TAG_APPLICATION_SIGNALLING (0x6f) */
typedef struct tagAPP_SIGNALLING_INFO
{
	HUINT16 usAppType;
	HUINT8 ucReserved;
	HUINT8 ucAitVersion;
	struct tagAPP_SIGNALLING_INFO *pNext;
} svcCas_AppSignallingInfo_t, *pAppSignallingInfo_t;

typedef struct tagAPP_SIGNALLING_DES
{
	HUINT8	ucDesTag;
	svcCas_AppSignallingInfo_t *pInfo;
} svcCas_AppSignallingDes_t, *pAppSignallingDes_t;

/** @brief TAG_DEFAULT_AUTHORITY (0x73) */
typedef	struct	tagDEFAULT_AUTHORITY_DES
{
	HUINT8	ucDesTag;
	HUINT8	*pszDefaultAuthority;
} svcCas_DefaultAuthorityDes_t, *pDefaultAuthorityDes_t;

typedef	struct	tagCRID_INFO
{
	HUINT8	ucCridType;
	HUINT8	ucCridLocation;
	HUINT16	usCridRef;
	HUINT8	*pszCridBytes;

	struct	tagCRID_INFO	*pNext;
} svcCas_CridInfo_t, *pCridInfo_t;

/** @brief TAG_RELATED_CONTENT (0x74) */
typedef	struct	tagRELATED_CONTENT_DES
{
	HUINT8	ucDesTag;
	HUINT8	*pPrivateData; // Spec상 없지만 형식을 위해서....
} svcCas_RelatedContentDes_t, *pRelatedContentDes_t;

/** @brief TAG_CONTENT_IDENTIFIER (0x76) */
typedef	struct	tagCONTENT_IDENTIFIER_DES
{
	HUINT8		ucDesTag;
	svcCas_CridInfo_t	*pInfo;
} svcCas_ContentIdentifierDes_t, *pContentIdentifierDes_t;

/** @brief TAG_S2_SATELLITE_DELIVERY_SYSTEM (0x79) */
typedef struct tagS2_SAT_DELI_SYSTEM_DES
{
	HUINT8	ucDesTag;
	HUINT8	ucScrambleSeqSelect;
	HUINT8	ucMultipleInStreamFlag;
	HUINT8	ucBackCompatibilityIndicator;
	HUINT8	ucReserved1;
	HUINT8	ucReserved2;
	HUINT32	ulScramblingSeqIndex;
	HUINT8	ucInputStreamIdentifier;
} svcCas_S2SatDeliSystemDes_t, *pS2SatDeliSystemDes_t;

/** @brief TAG_ENHANCED_AC3(0x7a) */
typedef struct tagENHANCED_AC3_DES
{
	HUINT8	ucDesTag;
	HBOOL	bComponentTypeFlag;
	HBOOL	bBsidFlag;
	HBOOL	bMainIdFlag;
	HBOOL	bAsvcFlag;
	HBOOL	bMixinfoexists;
	HBOOL	bSubStream1Flag;
	HBOOL	bSubStream2Flag;
	HBOOL	bSubStream3Flag;
	HUINT8	ucComponentType;
	HUINT8	ucBsId;
	HUINT8	ucMainId;
	HUINT8	ucAsvc;
	HUINT8	ucSubStream1;
	HUINT8	ucSubStream2;
	HUINT8	ucSubStream3;
} svcCas_EnhancedAC3Des_t, *pEnhancedAC3Des_t;

/** @brief TAG_AAC(0x7C) */
typedef struct tagAAC_DES {
	HUINT8	ucDesTag;
	HUINT8	ucProfileAndLevel;
	HBOOL	bAACTypeFlag;
	HUINT8	ucAACType;
} svcCas_AACDes_t, *pAACDes_t;

/* in SIT Partial Transport Stream Descriptor */
typedef struct tagPartialTsDes
{
	HUINT8	ucDesTag;
	HUINT32	ulPeakRate;
	HUINT32	ulMinimumOverallSmoothingRate;
	HUINT32	ulMaximumOverallSmoothingBuffer;
} svcCas_PartialTsDes, *pPartialTsDes;


#define ____FTA_CONTENT_MANAGEMENT_DESCRIPTOR____
// TAG_FTA_CONTENT_MANAGEMENT(0x7E)
typedef struct tagFTA_CONTENT_MANAGEMENT_DES {
	HUINT8		ucDesTag;
	HUINT8		ucDoNotScramble;
	HUINT8		ucCtrlRAccOverINet;
	HUINT8		ucDoNotApplyRevocation;
} svcCas_FtaContentManagementDes_t, *pFtaContentManagementDes_t;

#define ____EXTENSION_DESCRIPTOR____
// TAG_EXTENSION(0x7F)
typedef struct tagEXTENSION_DES {
	HUINT8		ucDesTag;
	HUINT8		ucExtDesTag;
	HUINT8		ucExtDesLen;
	HUINT8		*pExtDesData;
} svcCas_ExtensionDes_t, *pExtensionDes;

// TAG_EXTENSION(0x7F)'s SUB descriptor TAG_EXT_IMG_ICON(0x04)
typedef struct tagIMG_ICON_DATA_INFO {
	HUINT16			usDataLen;
	HUINT8			*pucImgData;
} ImageIconDataInfo, *pImageIconDataInfo;

typedef struct tagIMG_ICON_URL_INFO {
	HUINT16			usUrlLen;
	HUINT8			*pucImgUrl;
} ImageIconURLInfo, *pImageIconURLInfo;

typedef struct tagIMG_ICON_TYPE_INFO {
	HUINT16			usTypeLen;
	HUINT8			*pucImgType;
} ImageIconTypeInfo, *pImageIconTypeInfo;

typedef struct tagIMG_ICON_HEAD_INFO {
	HUINT8					ucIconTransMode;
	HUINT8					ucIconPosFlag;
	HUINT8					ucCoordSystem;
	HUINT16					usIconHorOrigin;
	HUINT16					usIconVerOrigin;
	ImageIconTypeInfo		*pstImgTypeInfo;
	ImageIconURLInfo			*pstImgUrlInfo;
	ImageIconDataInfo		*pstImgIconInfo;
} ImageIconHeadInfo, *pImageIconHeadInfo;

typedef struct tagIMG_ICON_EXT_DES {
	HUINT8					ucDesExtTag;
	HUINT8					ucDesNum;
	HUINT8					ucLastDesNum;
	HUINT8					ucIconId;
	ImageIconHeadInfo		*pstImgHeadInfo;
	ImageIconDataInfo		*pstImgIconInfo;
} svcCas_ImageIconExtDes_t, *pImageIconExtDes;

// TAG_EXTENSION(0x7F)'s SUB descriptor TAG_EXT_T2_DELIVERY_SYSTEM(0x04)
typedef struct tagT2_DELIVERY_SUBCELL_INFO {
	HUINT8		ucCellIdExtension;
	HUINT32		ulTransponderFreq;
} T2DeliverySubCellInfo, *pT2DeliverySubCellInfo;

typedef struct tagT2_DELIVERY_CELL_INFO {
	HUINT16								usCellId;
	HUINT8								ucFreqLoopCnt;
	HUINT32								*pulCentreFreq;
	HUINT8								ucSubCellInfoLoopCnt;
	T2DeliverySubCellInfo				*pstSubCellInfo;
	struct	tagT2_DELIVERY_CELL_INFO	*pNextInfo;
} svcCas_T2DeliveryCellInfo_t, *pT2DeliveryCellInfo;

typedef struct tagT2_DELIVERY_DETAIL_INFO {
	HUINT8				ucSMISO;
	HUINT8				ucBandWidth;
	HUINT8				ucGuardInterval;
	HUINT8				ucTransmissionMode;
	HUINT8				ucOtherFreqFlag;
	HUINT8				ucTfsFlag;
	svcCas_T2DeliveryCellInfo_t	*pstCellInfo;
} svcCas_T2DeliveryDetailInfo_t, *pT2DeliveryDetailInfo;

typedef struct tagT2_DELIVERY_SYSTEM_EXT_DES {
	HUINT8					ucDesExtTag;
	HUINT8					ucPlpId;
	HUINT16					usT2SystemId;
	svcCas_T2DeliveryDetailInfo_t	*pstDetailInfo;
} svcCas_T2DeliverySystemExtDes_t, *pT2DeliverySystemExtDes;

// TAG_EXTENSION (0x7F)'s SUB descriptor TAG_EXT_SUPPLEMENTARY_AUDIO(0x06)
typedef struct tagSUPPLEMENTARY_AUDIO_EXT_DES {
	HUINT8					ucDesExtTag;
	HUINT8					ucMixType;
	HUINT8					ucEditorialClassification;
	HUINT8					ucLanguageCodePresent;
	HUINT8					ISO_639[4];
	HUINT8					*pPrivateDataByte;
} svcCas_SupplementaryAudioExtDes, *pSupplementaryAudioExtDes;

// TAG_EXTENSION(0x7F)'s SUB descriptor TAG_EXT_NETWORK_CHANGE_NOTIFY(0x07)
typedef struct tagUTC_TIME_INFO {
	HUINT16		usMJD;
	HUINT32		ulBCD;
} UtcTimeInfo, *pUtcTimeInfo;

typedef struct tagNET_CHANGE_NOTIFY_SUB_INFO {
	HUINT8									ucNetChangeId;
	HUINT8									ucNetChangeVer;
	HUINT8									ucStartTimeOfChange[5];
	HUINT32									ulBcdChangeDur;
	HUINT8									ucReceiverCategory;
	HUINT8									ucInvariantTsPresent;
	HUINT8									ucChangeType;
	HUINT8									ucMessageId;
	HUINT16									usInvariantTsId;
	HUINT16									usInvariantOnId;
	struct	tagNET_CHANGE_NOTIFY_SUB_INFO	*pNextInfo;
} svcCas_NetChangeNotifySubInfo_t, *pNetChangeNotifySubInfo;

typedef struct tagNET_CHANGE_NOTIFY_CELL_INFO {
	HUINT16									usCellId;
	svcCas_NetChangeNotifySubInfo_t					*pstNetChNotifySubInfo;
	struct	tagNET_CHANGE_NOTIFY_CELL_INFO	*pNextInfo;
} svcCas_NetChangeNotifyCellInfo_t, *pNetChangeNotifyCellInfo;

typedef struct tagNETWORK_CHANGE_NOTIFY_EXT_DES {
	HUINT8					ucDesExtTag;
	svcCas_NetChangeNotifyCellInfo_t	*pstNetChNotifyCellInfo;
} svcCas_NetChangeNotifyExtDes_, *pNetChangeNotifyExtDes;

// TAG_EXTENSION (0x7F)'s SUB descriptor TAG_EXT_MESSAGE(0x07)
typedef struct tagMESSAGE_EXT_DES {
	HUINT8		ucDesExtTag;
	HUINT8		ucMessageId;
	HUINT8		szIso639LangCode[3];
	HUINT8		*pszMessageText;
} svcCas_MessageExtDes_t, *pMessageExtDes;


// TAG_EXTENSION(0x7F)'s SUB descriptor TAG_EXT_TARGET_REG(0x09)
typedef struct tagTARGET_REGION_INFO
{
	HUINT8	PrimaryRegionCode;
	HUINT8	SecondaryRegionCode;
	HUINT16	TertiaryRegionCode;
} TargetRegionInfo_t, *pTargetRegionInfo_t;

typedef struct tagTARGET_REG_DES_INFO {
	HUINT8			ucCountryCodeFlag;
	HUINT8			ucRegionDepth;
	HUINT8			ucCountryCode[4];
	TargetRegionInfo_t		stTargetRegionInfo;

	struct tagTARGET_REG_DES_INFO		*pNextInfo;
} svcCas_TargetRegionExtDesInfo, *pTargetRegionExtDesInfo;

typedef struct tagTARGET_REG_DES {
	HUINT8					ucDesExtTag;
	HUINT8					ucCountryCode[4];
	svcCas_TargetRegionExtDesInfo		*pstTargetRegionDesInfo;
} svcCas_TargetRegionExtDes, *pTargetRegionExtDes;

// TAG_EXTENSION(0x7F)'s SUB descriptor TAG_EXT_TARGET_REG_NAME(0x0A)
typedef struct tagTARGET_REG_NAME_DES_INFO {
	HUINT8									ucRegionDepth;
	HUINT8									ucRegionNameLen;
	HUINT8									*pszRegionName;
	TargetRegionInfo_t							stTargetRegionInfo;
	struct tagTARGET_REG_NAME_DES_INFO	*pNextInfo;
} TargetRegionNameExtDesInfo, *pTargetRegionNameExtDesInfo;

typedef struct tagTARGET_REG_NAME_DES {
	HUINT8							ucDesExtTag;
	HUINT8							ucCountryCode[4];
	HUINT8							ucIso639LangCode[4];
	TargetRegionNameExtDesInfo		*pstTargetRegionNameDesInfo;
} svcCas_TargetRegionNameExtDes, *pTargetRegionNameExtDes;
#endif


//=================================================
// For SI retrieving
// ================================================

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

/* Public Function 마다 필히 기재하도록 함 */
/**
  @brief     LAYER_MODULE_DoSomething 함수.

  Function Name:
  Function Description:
  Parameter:
  type: (Sync or Async)
  Return Value:
*/

/*---------------------------------------------
            si raw section manager
----------------------------------------------*/
/** @brief 다음 함수들은 외부에서 접근가능하나, 주로 si callback에서 사용됨 \n
    세마포어로 보호되는 함수들
*/

#define _______COMMON_RAW_MANAGER_FUNCTION__________

HERROR			svc_cas_RawInit(void);
svcCas_RawPool_t*		svc_cas_RawAllocRawPool(HUINT16 usMaxRawSec, HUINT8 *szPoolName);
void			svc_cas_RawClearRawPool(svcCas_RawPool_t *pstPool);
void			svc_cas_RawFreeRawPool(svcCas_RawPool_t *pstPool);
HUINT32			svc_cas_RawAddRawSection(svcCas_RawPool_t *pstPool, HUINT16 usUniqueId, SiTableType_e eSiTableType, HUINT8 *pucRawSec);
void			svc_cas_RawFreeAllSection(svcCas_RawPool_t *pstPool, HUINT16 usUniqueId, SiTableType_e eSiTableType);
HUINT32			svc_cas_RawFreeSection(svcCas_RawPool_t *pstPool, HUINT16 usUniqueId, SiTableType_e eSiTableType, HBOOL bUseExtId, HUINT16 usExtId);
HUINT32			svc_cas_RawGetSectionNum(svcCas_RawPool_t *pstPool, HUINT16 usUniqueId, SiTableType_e eSiTableType, HUINT16 *pusCnt);
HUINT32			svc_cas_RawGetSectionList(svcCas_RawPool_t *pstPool, HUINT16 usUniqueId, SiTableType_e eSiTableType, HUINT16 *pusSecIdxList);
//HUINT32			svc_cas_RawGetSectionNumAndList(svcCas_RawPool_t *pstPool, HUINT16 usUniqueId, SiTableType_e eSiTableType, HUINT16 *pusCnt, HUINT16 **ppusSecIdxList);
HERROR			svc_cas_RawGetVerAndCrc(svcCas_RawPool_t *pstPool, HUINT16 usUniqueId, SiTableType_e eSiType, HBOOL bUseExtId, HUINT16 usExtId, HUINT8 *pucVer, HUINT32 *pulCrc32);
#if defined(CONFIG_DEBUG)
void			svc_cas_RawPrintRawPool(svcCas_RawPool_t *pstPool);
#endif

svcCas_RawSecTable_t*	svc_cas_RawGetRawTable(svcCas_RawPool_t *pstPool, SiTableType_e eTableType);
HUINT8*			svc_cas_RawGetRawSection(svcCas_RawPool_t *pstPool, HUINT32 ulSecIdx);
HUINT8**		svc_cas_RawGetRawArrays(svcCas_RawPool_t *pstPool,
										HUINT16 usTsUniqId,
										SiTableType_e eSiType,
										HBOOL bUseExtId,
										HUINT16 usExtId,
										HUINT32 *pulArrayLen);
HERROR			svc_cas_RawFreeRawArrays(HUINT8 **ppucRawArrays);
HUINT16			svc_cas_RawGetExtId(svcCas_RawPool_t *pstPool, HUINT16 usSecIdx);
HUINT16			svc_cas_RawGetExtIdFromRawData(HUINT8 *pucRaw);

HERROR			svc_cas_RawCheckRawSamenessInReceivingTable(svcCas_RawPool_t *pstPool,
														HUINT16 usTsUniqId,
														SiTableType_e eSiType,
														HUINT16 usExtId,
														HUINT8 *pucRaw);
HERROR			svc_cas_RawAddRawSecToReceivingTable(svcCas_RawPool_t *pstPool,
														HUINT16 usTsUniqId,
														SiTableType_e eSiType,
														HUINT16 usExtId,
														HUINT8 *pucRaw);
HERROR			svc_cas_RawIsMultiSecFinishedInReceivingTable (svcCas_RawPool_t *pstPool,
														HUINT16 usTsUniqId,
														SiTableType_e eSiType,
														HUINT16 usExtId);
HUINT8**		svc_cas_RawGetRawArraysFromReceivingTable(svcCas_RawPool_t *pstPool,
																	HUINT16 usTsUniqId,
																	SiTableType_e eSiType,
																	HUINT16 usExtId,
																	HUINT32	*pulArrayLen);
HERROR			svc_cas_RawFreeReceivingTables (svcCas_RawPool_t *pstPool, HUINT16 usTsUniqId, SiTableType_e eSiType);

HERROR			svc_cas_RawRegisterSectionMakeFunction(svcCas_RawPool_t *pstPool, svcCas_RawMakeSecCb_t pfSecMake);
svcCas_RawMakeSecCb_t	svc_cas_RawGetSectionMakeFunction(svcCas_RawPool_t *pstPool);


/*---------------------------------------------
                si util
----------------------------------------------*/

HUINT32		svc_cas_SiUtilStrlen(const HUINT8 *pszStr);


/*---------------------------------------------
                si semi-parser
----------------------------------------------*/

#define _______COMMON_SI_FUNCTION__________

void*	svc_cas_SiParseDescriptor(SIxTextEncoding_e eTextEnc, HUINT8 *pucRaw);
HERROR	svc_cas_SiFreeDescriptor(void *ppvDes);
HERROR	svc_cas_SiFreeDescriptorRaw(HUINT8 *pucDesRaw);
HERROR	svc_cas_SiGetDescriptorTag(void *pvDes, HUINT32 *pulDesTag);
HBOOL	svc_cas_SiIsUserDefinedDescriptor(HUINT32 ulTag);
HUINT32	svc_cas_SiPreStrMgr(SIxTextEncoding_e eEncType, HUINT8 *aucLangCode, HUINT8 ucSrcStrLen, HUINT8 *pSrcStr, HUINT8 **pDestStr);

// == CAT ==
HERROR svc_cas_SiCountCatSection(svcCas_RawPool_t *pstPool, HUINT16 usTsUniqId, HUINT32 *pulCnt);
HERROR svc_cas_SiCountCatDescriptor(svcCas_RawPool_t *pstPool, HUINT16 usTsUniqId, HUINT32 ulIdx, HUINT32 ulTag,	HUINT32 *pnCnt);
HERROR svc_cas_SiGetCatDescriptorTag(svcCas_RawPool_t *pstPool, HUINT16 usTsUniqId, HUINT32 ulIdx, HUINT32 nDesIdx, HUINT8 *pucTag);
HERROR svc_cas_SiGetCatDescriptor(svcCas_RawPool_t *pstPool, SIxTextEncoding_e eTextEnc, HUINT16 usTsUniqId, HUINT32 ulIdx, HUINT32 ulTag, HUINT32 nDesIdx, void **ppvDes);
HERROR svc_cas_SiGetCatDescriptorRaw(svcCas_RawPool_t *pstPool, HUINT16 usTsUniqId, HUINT32 ulIdx, HUINT32 ulTag, HUINT32 nDesIdx, HUINT32 *pulDesRawSize, HUINT8 **ppucDesRaw);

// == PAT ==

HERROR svc_cas_SiCountPatSection(svcCas_RawPool_t *pstPool, HUINT16 usTsUniqId, HUINT32 *pulCnt);
HERROR svc_cas_SiGetPatSectionFromRaw(HUINT8 *pucRaw, svcCas_SiPatSectionInfo_t *pInfo);
HERROR svc_cas_SiGetPatSection(svcCas_RawPool_t *pstPool,
									HUINT16 usTsUniqId,
									HUINT32 ulIdx,
									svcCas_SiPatSectionInfo_t *pInfo);
HERROR svc_cas_SiCountPatProgramFromRaw(HUINT8 *pucRaw, HUINT32 *pulCnt);
HERROR svc_cas_SiCountPatProgram(svcCas_RawPool_t *pstPool, HUINT16 usTsUniqId, HUINT32 ulIdx, HUINT32 *pulCnt);
HERROR svc_cas_SiGetPatProgramInfoFromRaw(HUINT8 *pucRaw, HUINT32 ulInfoIdx, svcCas_SiPatProgInfo_t *pstInfo);
HERROR svc_cas_SiGetPatProgramInfo(svcCas_RawPool_t *pstPool,
											HUINT16 usTsUniqId,
											HUINT32 ulIdx,
											HUINT32 ulSubIdx,
											svcCas_SiPatProgInfo_t *pstInfo);


// == PAT SvcId ==

HERROR svc_cas_SiGetPmtPidByServiceIdFromRaw(HUINT8 *pucRaw, HUINT16 usSvcId, HUINT16 *pusPmtPid);
HERROR svc_cas_SiGetPmtPidByServiceId(svcCas_RawPool_t *pstPool,
												HUINT16 usTsUniqId,
												HUINT16 usSvcId,
												HUINT16 *pusPmtPid);
HERROR svc_cas_SiGetFirstPmtPid(svcCas_RawPool_t *pstPool, HUINT16 usTsUniqId, HUINT16 *pusProgNum, HUINT16 *pusPmtPid);


// == PMT ==
HERROR svc_cas_SiCountTotalPmtProgramDescriptor(svcCas_RawPool_t *pstPool,
														HUINT16 usTsUniqId,
														HUINT32 ulSecIdx,
														HUINT32 *pnCnt);
HERROR svc_cas_SiGetTotalPmtProgramDescriptor(svcCas_RawPool_t *pstPool,
													HUINT16 usTsUniqId,
													HUINT32 ulSecIdx,
													HUINT32 nDesIdx,
													void **ppvDes);
HERROR svc_cas_SiCountTotalPmtElementDescriptor(svcCas_RawPool_t *pstPool,
													HUINT16 usTsUniqId,
													HUINT32 ulIdx,
													HUINT32 nSubIdx,
													HUINT32 *pnDesCnt);
HERROR svc_cas_SiGetTotalPmtElementDescriptor(svcCas_RawPool_t *pstPool,
													HUINT16 usTsUniqId,
													HUINT32 ulIdx,
													HUINT32 nSubIdx,
													HUINT32 nDesIdx,
													void **ppvDes);

HERROR svc_cas_SiCountPmtSection(svcCas_RawPool_t *pstPool, HUINT16 usTsUniqId, HUINT32 *pnCnt);
HERROR svc_cas_SiGetPmtProgramInfoFromRaw(HUINT8 *pucRaw, svcCas_SiPmtProgramInfo_t *pstInfo);
HERROR svc_cas_SiGetPmtProgramInfo(svcCas_RawPool_t *pstPool,
											HUINT16 usTsUniqId,
											HUINT32 ulIdx,
											svcCas_SiPmtProgramInfo_t *pstInfo);
HERROR svc_cas_SiCountPmtProgramDescriptorFromRaw(HUINT8 *pucRaw, HUINT32 ulTag, HUINT32 *pnCnt);
HERROR svc_cas_SiCountPmtProgramDescriptor(svcCas_RawPool_t *pstPool,
														HUINT16 usTsUniqId,
														HUINT32 ulIdx,
														HUINT32 ulTag,
														HUINT32 *pnCnt);
HERROR svc_cas_SiGetPmtProgramDescriptorTagFromRaw(HUINT8 *pucRaw,
																HUINT32 ulDesIdx,
																HUINT8 *pucTag);
HERROR svc_cas_SiGetPmtProgramDescriptorTag(svcCas_RawPool_t *pstPool,
													HUINT16 usTsUniqId,
													HUINT32 ulIdx,
													HUINT32 ulDesIdx,
													HUINT8 *pucTag);
HERROR svc_cas_SiGetPmtProgramDescriptorFromRaw(HUINT8 *pucRaw,
												SIxTextEncoding_e eTextEnc,
												HUINT32 ulTag,
												HUINT32 nDesIdx,
												void **ppvDes);
HERROR svc_cas_SiGetPmtProgramDescriptor(svcCas_RawPool_t *pstPool,
											SIxTextEncoding_e eTextEnc,
											HUINT16 usTsUniqId,
											HUINT32 ulIdx,
											HUINT32 ulTag,
											HUINT32 nDesIdx,
											void **ppvDes);
HERROR svc_cas_SiGetPmtProgramDesRawFromRaw(HUINT8 *pucRaw,
																HUINT32 ulTag,
																HUINT32 nDesIdx,
																HUINT32 *pulDesRawSize,
																HUINT8 **ppucDesRaw);
HERROR svc_cas_SiGetPmtProgramAllDesRawFromRaw(HUINT8 *pucRaw,
													HUINT32 *pulDesRawSize,
													HUINT8 **ppucDesRaw);
HERROR svc_cas_SiGetPmtProgramDesRaw(svcCas_RawPool_t *pstPool,
													HUINT16 usTsUniqId,
													HUINT32 ulIdx,
													HUINT32 ulTag,
													HUINT32 nDesIdx,
													HUINT32 *pulDesRawSize,
													HUINT8 **ppucDesRaw);
HERROR svc_cas_SiGetPmtProgramAllDesRaw(svcCas_RawPool_t *pstPool,
													HUINT16 usTsUniqId,
													HUINT32 ulIdx,
													HUINT32 *pulDesRawSize,
													HUINT8 **ppucDesRaw);
HERROR svc_cas_SiCountPmtElementFromRaw(HUINT8 *pucRaw, HUINT32 *pnCnt);
HERROR svc_cas_SiCountPmtElement(svcCas_RawPool_t *pstPool, HUINT16 usTsUniqId, HUINT32 ulIdx, HUINT32 *pnCnt);
HERROR svc_cas_SiGetPmtElementInfoFromRaw(HUINT8 *pucRaw, HUINT32 ulElemIdx, svcCas_SiPmtElementInfo_t *pstInfo);
HERROR svc_cas_SiGetPmtElementInfo(svcCas_RawPool_t *pstPool,
											HUINT16 usTsUniqId,
											HUINT32 ulIdx,
											HUINT32 nSubIdx,
											svcCas_SiPmtElementInfo_t *pstInfo);
HERROR svc_cas_SiCountPmtElementDescriptorFromRaw(HUINT8 *pucRaw,
																HUINT32 ulElemIdx,
																HUINT32 ulTag,
																HUINT32 *pnDesCnt);
HERROR svc_cas_SiCountPmtElementDescriptor(svcCas_RawPool_t *pstPool,
													HUINT16 usTsUniqId,
													HUINT32 ulIdx,
													HUINT32 nSubIdx,
													HUINT32 ulTag,
													HUINT32 *pnDesCnt);
HERROR svc_cas_SiGetPmtElementDescriptorTagFromRaw(HUINT8 *pucRaw,
																HUINT32 ulElemIdx,
																HUINT32 ulDesIdx,
																HUINT8 *pucTag);
HERROR svc_cas_SiGetPmtElementDescriptorTag(svcCas_RawPool_t *pstPool,
													HUINT16 usTsUniqId,
													HUINT32 ulIdx,
													HUINT32 ulSubIdx,
													HUINT32 ulDesIdx,
													HUINT8 *pucTag);
HERROR svc_cas_SiGetPmtElementDescriptorFromRaw(HUINT8 *pucRaw,
												SIxTextEncoding_e eTextEnc,
												HUINT32 ulElemIdx,
												HUINT32 ulTag,
												HUINT32 nDesIdx,
												void **ppvDes);
HERROR svc_cas_SiGetPmtElementDescriptor(svcCas_RawPool_t *pstPool,
													SIxTextEncoding_e eTextEnc,
													HUINT16 usTsUniqId,
													HUINT32 ulIdx,
													HUINT32 nSubIdx,
													HUINT32 ulTag,
													HUINT32 nDesIdx,
													void **ppvDes);
#if 0
HERROR svc_cas_SiGetPmtElementExtDescriptor(svcCas_RawPool_t *pstPool,
											HUINT16 usTsUniqId,
											HUINT32 ulIdx,
											HUINT32 nSubIdx,
											HUINT32 ulExtTag,
											HUINT32 nDesExtIdx,
											void **ppvDes);
#endif

HERROR svc_cas_SiGetPmtElementDesRawFromRaw(HUINT8 *pucRaw,
													HUINT32 ulElemIdx,
													HUINT32 ulTag,
													HUINT32 nDesIdx,
													HUINT32 *pulDesRawSize,
													HUINT8 **ppucDesRaw);
HERROR svc_cas_SiGetPmtElementAllDesRawFromRaw(HUINT8 *pucRaw,
													HUINT32 ulElemIdx,
													HUINT32 *pulDesRawSize,
													HUINT8 **ppucDesRaw);
HERROR svc_cas_SiGetPmtElementDesRaw(svcCas_RawPool_t *pstPool,
													HUINT16 usTsUniqId,
													HUINT32 ulIdx,
													HUINT32 nSubIdx,
													HUINT32 ulTag,
													HUINT32 ulDesIdx,
													HUINT32 *pulDesRawSize,
													HUINT8 **ppucDesRaw);
HERROR svc_cas_SiGetPmtElementAllDesRaw(svcCas_RawPool_t *pstPool,
													HUINT16 usTsUniqId,
													HUINT32 ulIdx,
													HUINT32 nSubIdx,
													HUINT32 *pulDesRawSize,
													HUINT8 **ppucDesRaw);

// == PMT SvcId ==

HERROR svc_cas_SiGetPmtProgNumFromRaw(HUINT8 *pucRaw, HUINT16 *pusProgNum);
HERROR svc_cas_SiGetPmtIndexByServiceId(svcCas_RawPool_t *pstPool,
													HUINT16 usTsUniqId,
													HUINT16 usSvcId,
													HUINT32 *pulPmtIndex);
HERROR svc_cas_SiGetPmtProgramInfoByServiceId(svcCas_RawPool_t *pstPool,
															HUINT16 usTsUniqId,
															HUINT16 usSvcId,
															svcCas_SiPmtProgramInfo_t *pstInfo);


// == SDT ==

HERROR svc_cas_SiCountSdtSection(svcCas_RawPool_t *pstPool,
										HUINT16 usTsUniqId,
										HUINT32 ulHead,
										HUINT32 *pulCnt);
HERROR svc_cas_SiGetSdtSectionInfoFromRaw(HUINT8 *pucRaw, svcCas_SiSdtSectionInfo_t *pstInfo);
HERROR svc_cas_SiGetSdtSectionInfo(svcCas_RawPool_t *pstPool,
											HUINT16 usTsUniqId,
											HUINT32 ulHead,
											HUINT32 ulIdx,
											svcCas_SiSdtSectionInfo_t *pstInfo);
HERROR svc_cas_SiCountSdtServiceFromRaw(HUINT8 *pucRaw, HUINT32 *pulCnt);
HERROR svc_cas_SiCountSdtService(svcCas_RawPool_t *pstPool,
										HUINT16 usTsUniqId,
										HUINT32 ulHead,
										HUINT32 ulIdx,
										HUINT32 *pulCnt);
HERROR svc_cas_SiGetSdtServiceInfoFromRaw(HUINT8 *pucRaw, HUINT32 ulSvcIdx, svcCas_SiSdtServiceInfo_t *pstInfo);
HERROR svc_cas_SiGetSdtServiceInfo(svcCas_RawPool_t *pstPool,
											HUINT16 usTsUniqId,
											HUINT32 ulHead,
											HUINT32 ulIdx,
											HUINT32 ulSubIdx,
											svcCas_SiSdtServiceInfo_t *pstInfo);
HERROR svc_cas_SiCountSdtServiceDescriptorFromRaw(HUINT8 *pucRaw,
																HUINT32 ulSvcIdx,
																HUINT32 ulTag,
																HUINT32 *pnDesCnt);
HERROR svc_cas_SiCountSdtServiceDescriptor(svcCas_RawPool_t *pstPool,
													HUINT16 usTsUniqId,
													HUINT32 ulHead,
													HUINT32 ulIdx,
													HUINT32 ulSubIdx,
													HUINT32 ulTag,
													HUINT32 *pnDesCnt);

HERROR svc_cas_SiGetSdtServiceDescriptorTagFromRaw(HUINT8 *pucRaw,
															HUINT32 ulSvcIdx,
															HUINT32 ulDesIdx,
															HUINT8 *pucTag);

HERROR svc_cas_SiGetSdtServiceDescriptorTag(svcCas_RawPool_t *pstPool,
													HUINT16 usTsUniqId,
													HUINT32 ulHead,
													HUINT32 ulIdx,
													HUINT32 ulSubIdx,
													HUINT32 nDesIdx,
													HUINT8 *pucTag);

HERROR svc_cas_SiGetSdtServiceDescriptorFromRaw(HUINT8 *pucRaw,
													SIxTextEncoding_e eTextEnc,
													HUINT32 ulSvcIdx,
													HUINT32 ulTag,
													HUINT32 ulDesIdx,
													void **ppvDes);
HERROR svc_cas_SiGetSdtServiceDescriptor(svcCas_RawPool_t *pstPool,
													SIxTextEncoding_e eTextEnc,
													HUINT16 usTsUniqId,
													HUINT32 ulHead,
													HUINT32 ulIdx,
													HUINT32 ulSubIdx,
													HUINT32 ulTag,
													HUINT32 nDesIdx,
													void **ppvDes);
HERROR svc_cas_SiGetSdtServiceDesRawFromRaw(HUINT8 *pucRaw,
															HUINT32 ulSvcIdx,
															HUINT32 ulTag,
															HUINT32 ulDesIdx,
															HUINT32 *pulDesRawSize,
															HUINT8 **ppucDesRaw);
HERROR svc_cas_SiGetSdtServiceDesRaw(svcCas_RawPool_t *pstPool,
													HUINT16 usTsUniqId,
													HUINT32 ulHead,
													HUINT32 ulIdx,
													HUINT32 ulSubIdx,
													HUINT32 ulTag,
													HUINT32 nDesIdx,
													HUINT32 *pulDesRawSize,
													HUINT8 **ppucDesRaw);

#if 0 // Not Used
HERROR svc_cas_SiSetDesIndexToPmtProgDescFromRaw(HUINT8 *pucRaw, svcCas_SiDesIndex_t *pstDesIndex);
HERROR svc_cas_SiSetDesIndexToPmtProgDesc(svcCas_RawSecBookmark_t *pstSecBmk, svcCas_SiDesIndex_t *pstDesIndex);
HERROR svc_cas_SiSetInfoIndexToPmtElementInfoFromRaw(HUINT8 *pucRaw, svcCas_SiInfoIndex_t *pstInfoIndex);
HERROR svc_cas_SiSetInfoIndexToPmtElementInfo(svcCas_RawSecBookmark_t *pstSecBmk, svcCas_SiInfoIndex_t *pstInfoIndex);
HERROR svc_cas_SiSetInfoIndexToNextPmtElementInfo(svcCas_SiInfoIndex_t *pstCurrIndex, svcCas_SiInfoIndex_t *pstNextIndex);
HERROR svc_cas_SiGetPmtProgramInfoFromBookmark(svcCas_RawSecBookmark_t *pstSecBmk, svcCas_SiPmtProgramInfo_t *pstSecInfo);
HERROR svc_cas_SiGetPmtElementInfoFromInfoIndex(svcCas_SiInfoIndex_t *pstInfoIndex, svcCas_SiPmtElementInfo_t *pstInfo, svcCas_SiInfoIndex_t *pstNextIndex);
HERROR svc_cas_SiSetDesIndexToPmtElementDes(svcCas_SiInfoIndex_t *pstInfoIndex, svcCas_SiDesIndex_t *pstDesIndex);
#endif

#define ____COMMON_SI_RETRIEVE_FUNCTION____
HERROR	svc_cas_DmxGetTimeout(Handle_t hAction, SiTableType_e eTableType, HUINT32 *pulTimeout);

HERROR	svc_cas_DmxSetDefaultRetrieveParam(svcCas_SiRetrieveParam_t	*pstRetrieveParam,
										HUINT32						 hAction,
										HUINT16						 usTsUniqId,
										HUINT32						 ulDemuxId);

HERROR	svc_cas_DmxStopOneSection(HUINT16 usFilterId);

HERROR	svc_cas_DmxRetrieveSectionBase(svcCas_SiRetrieveParam_t	*pstTable,
									HUINT8						 ucTableId,
									HUINT8						*pucFilterValue,
									HUINT8						*pucFilterMask,
									HUINT32						 ulFilterSize,
									HUINT32						 ulFilterTimeout,
									SvcCas_SiParseCb_t			 fnParse,
									SvcCas_SiFreeCb_t			 fnFree,
									HUINT16						*pusFilterId);

// PAT
HERROR	svc_cas_DmxRetrievePatSection(svcCas_SiRetrieveParam_t	*pstTable,
									SvcCas_SiParseCb_t			 fnParse,
									SvcCas_SiFreeCb_t			 fnFree,
									HUINT16						*pusFilterId);

// CAT
HERROR	svc_cas_DmxRetrieveCatSection(svcCas_SiRetrieveParam_t	*pstTable,
									SvcCas_SiParseCb_t			 fnParse,
									SvcCas_SiFreeCb_t			 fnFree,
									HUINT16						*pusFilterId);

// PMT
HERROR	svc_cas_DmxRetrievePmtSection(svcCas_SiRetrieveParam_t	*pstTable,
									SvcCas_SiParseCb_t			 fnParse,
									SvcCas_SiFreeCb_t			 fnFree,
									HUINT16						*pusFilterId);

// NIT
HERROR	svc_cas_DmxRetrieveNitSection(svcCas_SiRetrieveParam_t	*pstTable,
									SvcCas_SiParseCb_t			 fnParse,
									SvcCas_SiFreeCb_t			 fnFree,
									HUINT16						*pusFilterId);

// SDT
HERROR	svc_cas_DmxRetrieveSdtSection(svcCas_SiRetrieveParam_t	*pstTable,
									SvcCas_SiParseCb_t			 fnParse,
									SvcCas_SiFreeCb_t			 fnFree,
									HUINT16		 		 		*pusFilterId);

// EIT SCH : Monitor
HERROR	svc_cas_DmxMonitorEitSchSection(svcCas_SiRetrieveParam_t	*pstTable,
									SvcCas_SiParseCb_t				 fnParse,
									SvcCas_SiFreeCb_t				 fnFree,
									HUINT16							*pusFilterId);

// EIT PF : Monitor
HERROR	svc_cas_DmxMonitorEitPfSection(svcCas_SiRetrieveParam_t	*pstTable,
									SvcCas_SiParseCb_t			 fnParse,
									HUINT16						*pusFilterId);

// EIT SCH : Retrieve
HERROR	svc_cas_DmxRetrieveEitSchSection(svcCas_SiRetrieveParam_t	*pstTable,
										SvcCas_SiParseCb_t			 fnParse,
										SvcCas_SiFreeCb_t			 fnFree,
										HUINT16		 		 		*pusFilterId);

// EIT SCH Other : Retrieve
HERROR	svc_cas_DmxRetrieveEitSchOtherSection(svcCas_SiRetrieveParam_t	*pstTable,
											HUINT8						 ucTableId,
											SvcCas_SiParseCb_t			 fnParse,
											SvcCas_SiFreeCb_t			 fnFree,
											HUINT16						*pusFilterId);

// EIT PF : Retrieve
HERROR svc_cas_DmxRetrieveEitPfSection(svcCas_SiRetrieveParam_t		*pstTable,
									SvcCas_SiParseCb_t				 fnParse,
									SvcCas_SiFreeCb_t				 fnFree,
									HUINT16							*pusFilterId);

// TDT : Retrieve
HERROR svc_cas_DmxRetrieveTdtSection(svcCas_SiRetrieveParam_t	*pstTable,
									SvcCas_SiParseCb_t			 fnParse,
									SvcCas_SiFreeCb_t			 fnFree,
									HUINT16						*pusFilterId);

// BAT : Retrieve
HERROR svc_cas_DmxRetrieveBatSection(svcCas_SiRetrieveParam_t	*pstTable,
									SvcCas_SiParseCb_t			 fnParse,
									SvcCas_SiFreeCb_t			 fnFree,
									HUINT16						*pusFilterId);

// DESC : Retrieve
HERROR svc_cas_DmxRetrieveDescSection(svcCas_SiRetrieveParam_t	*pstTable,
									SvcCas_SiParseCb_t			 fnParse,
									SvcCas_SiFreeCb_t			 fnFree,
									HUINT16						*pusFilterId);

// DSI : Retrieve
HERROR svc_cas_DmxRetrieveDsiSection(svcCas_SiRetrieveParam_t	*pstTable,
									SvcCas_SiParseCb_t			 fnParse,
									SvcCas_SiFreeCb_t			 fnFree,
									HUINT16						*pusFilterId);

// DDB : Retrieve
HERROR svc_cas_DmxRetrieveDdbSection(svcCas_SiRetrieveParam_t	*pstTable,
									SvcCas_SiParseCb_t			 fnParse,
									SvcCas_SiFreeCb_t			 fnFree,
									HUINT16						*pusFilterId);

// ECM : Retrieve
HERROR svc_cas_DmxRetrieveEcmSection(svcCas_SiRetrieveParam_t	*pstTable,
									HUINT8						 ucTableId,
									HUINT8						*pucFilterValue,
									HUINT8						*pucFilterMask,
									HUINT32						 ulFilterSize,
									HUINT32						 ulFilterTimeout,
									SvcCas_SiParseCb_t			 fnParse,
									SvcCas_SiFreeCb_t			 fnFree,
									HUINT16						*pusFilterId);

// EMM : Retrieve
HERROR svc_cas_DmxRetrieveEmmSection (svcCas_SiRetrieveParam_t	*pstTable,
									HUINT8						 ucTableId,
									HUINT8						*pucFilterValue,
									HUINT8						*pucFilterMask,
									HUINT32						 ulFilterSize,
									HUINT32						 ulFilterTimeout,
									SvcCas_SiParseCb_t			 fnParse,
									SvcCas_SiFreeCb_t			 fnFree,
									HUINT16						*pusFilterId);

// CIT : Retrieve
HERROR svc_cas_DmxRetrieveCitSection(svcCas_SiRetrieveParam_t	*pstTable,
									SvcCas_SiParseCb_t			 fnParse,
									SvcCas_SiFreeCb_t			 fnFree,
									HUINT16						*pusFilterId);

// CPT : Retrieve
HERROR svc_cas_DmxRetrieveCptSection(svcCas_SiRetrieveParam_t	*pstTable,
									SvcCas_SiParseCb_t			 fnParse,
									SvcCas_SiFreeCb_t			 fnFree,
									HUINT16						*pusFilterId);

// RCT : Retrieve
HERROR svc_cas_DmxRetrieveRctSection (svcCas_SiRetrieveParam_t	*pstTable,
									SvcCas_SiParseCb_t			 fnParse,
									SvcCas_SiFreeCb_t			 fnFree,
									HUINT16						*pusFilterId);

// BIT : Retrieve
HERROR svc_cas_DmxRetrieveBitSection(svcCas_SiRetrieveParam_t	*pstTable,
									SvcCas_SiParseCb_t			 fnParse,
									SvcCas_SiFreeCb_t			 fnFree,
									HUINT16						*pusFilterId);

// SIT : Retrieve
HERROR svc_cas_DmxRetrieveSitSection(svcCas_SiRetrieveParam_t	*pstTable,
									SvcCas_SiParseCb_t			 fnParse,
									SvcCas_SiFreeCb_t			 fnFree,
									HUINT16						*pusFilterId);

// Low Protection SDTT : Retrieve
HERROR svc_cas_DmxRetrieveLowProtectionSdttSection(svcCas_SiRetrieveParam_t	*pstTable,
												SvcCas_SiParseCb_t			 fnParse,
												SvcCas_SiFreeCb_t			 fnFree,
												HUINT16						*pusFilterId);

// Hi Protection SDTT : Retrieve
HERROR svc_cas_DmxRetrieveHighProtectionSdttSection(svcCas_SiRetrieveParam_t	*pstTable,
													SvcCas_SiParseCb_t			 fnParse,
													SvcCas_SiFreeCb_t			 fnFree,
													HUINT16						*pusFilterId);

// CDT : Retrieve
HERROR svc_cas_DmxRetrieveCdtSection(svcCas_SiRetrieveParam_t	*pstTable,
									SvcCas_SiParseCb_t			 fnParse,
									SvcCas_SiFreeCb_t			 fnFree,
									HUINT16						*pusFilterId);

// DII : Retrieve
HERROR svc_cas_DmxRetrieveDiiSection(svcCas_SiRetrieveParam_t	*pstTable,
									SvcCas_SiParseCb_t			 fnParse,
									SvcCas_SiFreeCb_t			 fnFree,
									HUINT16						*pusFilterId);

#endif /* !__SVC_CAS_SI_API_H__ */
