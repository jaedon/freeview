/* $Header: $ */

#ifndef __SILIB_ARIB_H__
#define __SILIB_ARIB_H__

#ifndef _____INCLUDED_ONLY_IN_SILIB_H____
#error The file [silib_desc_arib.h] shall not be included except silib.h
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <silib.h>

/* in SIT Broadcaster ID descriptor (0x85) */
/* eSIxDESCTAG_ARIB_BROADCASTER_ID (0x85) */
typedef struct
{
	SIxDescriptorHeader_t	stHead;
	HUINT16					usOriginalNetworkId;
	HUINT16					usTransportStreamId;
	HINT32					nEventId;
	HUINT8					ucBroadcasterId;
} SIxBroadcasterIdDes_t;

/* DTCP Descriptor (0x88) */
/* eSIxDESCTAG_ARIB_DTCP  (0x88) */
typedef struct
{
	SIxDescriptorHeader_t	stHead;
	HUINT8					ucRetentionMoveMode;
	HUINT8					ucRetentionState;
	HUINT8					ucEPN;
	HUINT8					ucDTCPCCI;
	HUINT8					ucAST;
	HUINT8					ucImageConstraintToken;
	HUINT8					ucAPS;
} SIxDvhsDTCPDes_t;

/* in PMT Hierarchical Transmission Descriptor */
/* eSIxDESCTAG_ARIB_HIERARCHICAL_TRANSMISSION (0xC0) */
typedef struct
{
	SIxDescriptorHeader_t	stHead;
	HBOOL					bQualityLevel;	/* 1 - high, 0 - low */
	HUINT16					usReferencePID;
} SIxHierarchicalTransmissionDes_t;

/* in PMT Digital Copy Control Descriptor */
/* eSIxDESCTAG_ARIB_DIGITAL_COPY_CONTROL (0xC1) */
typedef struct
{
	HUINT8				ucComponentTag;
	HUINT8				ucDigitalRecordingControlData;
	HUINT8				ucMaximumBitRateFlag;
	HUINT8				ucCopyControlType;
	HUINT8				ucAPSControlData;
	HUINT8				ucMaximumBitRate;
} SIxDigitalCopyControlDesComponent_t;

typedef struct tagSIxDigitalCopyControlDes
{
	SIxDescriptorHeader_t	stHead;
	HUINT8					ucDigitalRecordingControlData;
	HUINT8					ucMaximumBitRateFlag;
	HUINT8					ucComponentControlFlag;
	HUINT8					ucCopyControlType;
	HUINT8					ucAPSControlData;
	HUINT8					ucMaximumBitRate;
	HUINT8					ucNumComponents;
	SIxDigitalCopyControlDesComponent_t	*pstComponent;
} SIxDigitalCopyControlDes_t;

/* in SIT Network Identification Descriptor (0xC2) */
/* eSIxDESCTAG_ARIB_NETWORK_IDENTIFICATION (0xC2) */
typedef struct
{
	SIxDescriptorHeader_t 	stHead;
	HUINT32				 	ulCountryCode;
	HUINT16					usMediaType;
	HUINT16					usNetworkId;
	SIxPrivateDataInfo_t	*pstPriavetDataInfo;
} SIxNetworkIdentificationDes_t;

/* in SIT Partial TS Time Descriptor (0xC3) */
/* eSIxDESCTAG_ARIB_PARTIAL_TRANSPORT_STREAM_TIME (0xC3) */
typedef struct
{
	SIxDescriptorHeader_t 	stHead;
	HUINT8					ucEventVersionNumber;
	UNIXTIME				ulStartTime;
	UNIXTIME				ulDuration;
	UNIXTIME				ulOffset;
	HBOOL					bOffsetFlag;
	HBOOL					bOherDescriptorStatus;
	HBOOL					bJSTTimeFlag;
	UNIXTIME				ulJSTTime;
} SIxPartialTsTimeDes_t;

/* in EIT Audio Component Descriptor */
/* eSIxDESCTAG_ARIB_AUDIO_COMPONENT (0xC4) */
typedef struct tagSIxAudioComponentDes
{
	SIxDescriptorHeader_t	stHead;
	HUINT8					ucStreamContent;
	HUINT8					ucComponentType;
	HUINT8					ucComponentTag;
	HUINT8					ucStreamType;
	HUINT8					ucSimulcastGroupTag;
	HBOOL					bESMultiLlingualFlag;
	HBOOL					bMainComponentFlag;
	HUINT8					ucQualityIndicator;
	HUINT8					ucSamplingRate; /* 1 - 16k, 2-22.05k, 3-24k, 5-32k, 6-44.1k, 7-48k */
	HUINT32					ulISO639LanguageCode; /* jpn - 0x6A706E */
	HUINT32					ulISO639LanguageCode2;
	HUINT8					ucTextCharLen;
	HUINT8					*pucTextChar;
	struct tagSIxAudioComponentDes	*pstNext;
} SIxAudioComponentDes_t;


/* in EIT Data Content Descriptor */
/* eSIxDESCTAG_ARIB_DATA_CONTENTS (0xC7) */
typedef struct
{
	HUINT8				ucComponentTag;
	HUINT8				ucEventSectionFlag;
	HUINT8				ucComponentSizeFlag;
	HUINT8				ucDefaultTransactionIdFlag;
	HUINT8				ucDefaultTimeoutDIIFlag;
	HUINT8				ucDefaultLeakRateFlag;
	HUINT32				ulComponentSize;
	HUINT32				ulTransactionId;
	HUINT32				ulTimeoutValueDII;
	HUINT32				ulLeakRate;
} SIxAribCarouselComponent_t;

typedef struct
{
	HUINT8						 ulNumOfCarousels;
	SIxAribCarouselComponent_t	*pstCarouselComponent;
} SIxAribCarouselInfo_t;

typedef struct
{
	HUINT8				ucComponentTag;
	HUINT8				ucNumDataEventFlag;
	HUINT8				ucNumModulesFlag;
	HUINT8				ucNumResourcesFlag;
	HUINT8				ucCompressedComponentSizeFlag;
	HUINT8				ucComponentSizeFlag;
	HUINT8				ucDefaultTransactionIdFlag;
	HUINT8				ucDefaultTimeoutDIIFlag;
	HUINT8				ucDefaultLeakRateFlag;
	HUINT16				usNumDataEvent;
	HUINT32				ulNumModules;
	HUINT32				ulNumResoures;
	HUINT32				ulCompressedComponentSize;
	HUINT32				ulComponentSize;
	HUINT32				ulTransactionId;
	HUINT32				ulTimeoutValueDII;
	HUINT32				ulLeakRate;
} SIxAribStoredCarouselComponent_t;

typedef struct
{
	HUINT8								ucNumOfCarousels;
	SIxAribStoredCarouselComponent_t	*pstStoredCarouselComponent;
} SIxAribStoredCarouselInfo_t;

typedef struct
{
	HUINT8						 ucTransmissionFormat;		/* 2 bit */
	HUINT8						 ucAutoStartFlag;			/* 1 bit */
	HUINT8						 ucDocumentResolution;		/* 4 bit */
	HUINT8						 ucUseXml;					/* 1 bit */
	HUINT8						 ucDefaultVersionFlag;		/* 1 bit */
	HUINT8						 ucIndependentFlag;			/* 1 bit */
	HUINT8						 ucContentIdFlag;			/* 1 bit */
	HUINT8						 ucAssociatedContentsFlag;	/* 1 bit */
	HUINT8						 ucStyleForTvFlag;			/* 1 bit */
	HUINT8						 ucUpdateFlag;				/* 1 bit */
	HUINT32						 ulISO639LanguageCode;		/* 24 bit */
	HUINT32						 ulContentId;				/* 32 bit */
	HUINT16						 usContentVersion;			/* 16 bit */
	HUINT16						 usBmlMajorVersion;			/* 16 bit */
	HUINT16						 usBmlMinorVersion;			/* 16 bit */
	HUINT16						 usBxmlMajorVersion;		/* 16 bit */
	HUINT16						 usBxmlMinorVersion;		/* 16 bit */
	SIxAribCarouselInfo_t		*pstCarouselInfo;			/* ARIB STD-B24 3분책 C.2.1 */
	HUINT8						 ucOndemandRetrievalFlag;
	HUINT8						 ucFileStorableFlag;
	SIxAribStoredCarouselInfo_t	*pstStoredCarouselInfo;		/* ARIB STD-B24 3분책 C.2.2 */
} SIxAribBxmlInfo_t;

typedef struct
{
	HUINT8					 ucNumLanguages;
	HUINT8					*pucLanguageTag;			/* 3 bit */
	HUINT8					*pucDMF;					/* 4 bit */
	HUINT32					*pulISO639LanguageCode;	/* 24 bit */
} SIxAribCaptionInfo_t;

typedef struct tagSIxDataContentDes
{
	SIxDescriptorHeader_t					 stHead;
	HUINT16						 usDataComponentId;
	HUINT8						 ucEntryComponent;
	HUINT8						 ucSelectorLen;
	SIxAribBxmlInfo_t			*pstAribBxmlInfo;			/* ARIB STD-B24 2분책 9.3.3 */
	SIxAribCaptionInfo_t			*pstAribCaptionInfo;		/* ARIB STD-B24 1분책 9.6.2 */
	HUINT8						 ucNumOfComponentRef;
	HUINT8						*pucComponentRef;
	HUINT32						 ulISO639LanguageCode;
	HUINT8						 ucTextLen;
	HUINT8						*pucTextChar;
	struct tagSIxDataContentDes	*pstNext;
} SIxDataContentDes_t;

/* in PMT Video Decode Control Descriptor */
/* eSIxDESCTAG_ARIB_VIDEO_DECODE_CONTROL (0xC8) */
typedef struct tagSIxVideoDecodeControlDes
{
	SIxDescriptorHeader_t					stHead;
	HBOOL						bStillPictureFlag;
	HBOOL						bSequenceEndCodeFlag;
	HUINT8						ucVideoEncodeFormat;
} SIxVideoDecodeControlDes_t;

/* in NIT CA EMM TS Descriptor (0xCA) */
/* eSIxDESCTAG_ARIB_CA_EMM_TS (0xCA) */
typedef struct tagSIxCaEmmTsDes
{
	SIxDescriptorHeader_t					 stHead;
	HUINT16						 usCaSystemId;
	HUINT16						 usTransportStreamId;
	HUINT16						 usOriginalNetworkId;
	HUINT8						 ucPowerSupplyPeriod;
	struct tagSIxCaEmmTsDes		*pstNext;
} SIxCaEmmTsDes_t;

/* in EIT/SDT CA Contract Info Descriptor (0xCB) */
/* eSIxDESCTAG_ARIB_CA_CONTRACT_INFOMATION (0xCB) */
typedef struct tagSIxCaContractInfoDes
{
	SIxDescriptorHeader_t				 stHead;
	HUINT16					 usCASystemId;
	HUINT8					 ucCAUnitId;
	HUINT8					 ucNumOfComponent;
	HUINT8					*pucComponentTag;
	HUINT8					 ucContractVerificationInfoLength;
	HUINT8					*pucContractVerificationInfo;
	HUINT8					 ucFeeNameLength;
	HUINT8					*pucFeeName;
	struct tagSIxCaContractInfoDes *pstNext;
} SIxCaContractInfoDes_t;

/* in NIT TS Information Descriptor (0xCD) */
/* eSIxDESCTAG_ARIB_TS_INFORMATION (0xCD) */
typedef struct
{
	HUINT8					 ucTransmissionTypeInfo;
	HUINT8					 ucNumOfService;
	HUINT16					*pusServiceId;
} SIxAribTsInfoDesTransmissionTypeInfo_t;

typedef struct
{
	SIxDescriptorHeader_t								 stHead;
	HUINT8									 ucRemoteControlKeyId;
	HUINT8									 ucLengthOfTsName;
	HUINT8									 ucTransmissionTypeCount;
	HUINT8									*pucTsNameChar;
	SIxAribTsInfoDesTransmissionTypeInfo_t	*pstTransmissionTypeInfo;
} SIxTsInformationDes_t;

/* in BIT Extended Broadcaster Descriptor (0xCE) */
/* eSIxDESCTAG_ARIB_EXTENDED_BROADCASTER (0xCE) */
typedef struct
{
	SIxDescriptorHeader_t				 stHead;
	HUINT8					 ucBroadcasterType;
	HUINT16					 usTerrestrialBroadcasterId;
	HUINT8					 ucNumberOfAffiliationIdLoop;
	HUINT8					 ucNumberOfBroadcasterIdLoop;
	HUINT8					*pucAffiliationId;
	HUINT16					*pusOriginalNetworkId;
	HUINT8					*pucBroadcasterId;
} SIxExtendedBroadcasterDes_t;

/* Logo Transmission (0xCF) */
/* eSIxDESCTAG_ARIB_LOGO_TRANSMISSION (0xCF) */
typedef struct
{
	SIxDescriptorHeader_t				stHead;
	HUINT8					ucLogoTransmissionType;
	HUINT16					usLogoId;
} SIxLogoTransmissionDes_t;

/* in NIT  Cable TS Division System Descriptor, Specified by JCTEA STD-003 (0xF9) */
/* eSIxDESCTAG_ARIB_CABLE_TS_DIVISION_SYSTEM (0xF9) */
typedef struct
{
	HUINT32					 ulFrequency; 		/* 4bits BCD 8행 ex. 733MHz -> 0x07330000 */
	HUINT8					 ucFrameType;		/* 0x1 : 복수 TS 전송방식, 0xF : 단일 TS전송방식 */
	HUINT8					 ucFECOuter;		/* 0010 : RS(204,188) */
	HUINT8					 ucModulation;		/* 0x03 : 64QAM, 0x05 : 256QAM */
	HUINT32					 ulSymbolRate;		/* 4bits BCD code 7행 ex. 0x0052740 */
	HUINT8					 ucFECInner;		/* 111 : 고정값 */
	HUINT8					 ucFutureUseDataFlag;
	HUINT8					 ucFutureUseDataLength;
	HUINT8					*pucFutureUseData;
	HUINT8					 ucNumberOfServices;
	HUINT16					*pusServiceId;
} SIxCableTsDivisionSystemInfo_t;

typedef struct
{
	SIxDescriptorHeader_t							 stHead;
	HUINT8				 				 ucNumLoops;
	SIxCableTsDivisionSystemInfo_t		*pstInfo;
} SIxCableTsDivisionSystemDes_t;

/* in NIT Terrestrial Delivery System Descriptor (0xFA) */
/* eSIxDESCTAG_ARIB_TERRESTRIAL_DELIVERY_SYSTEM (0xFA) */
typedef struct
{
	SIxDescriptorHeader_t				 stHead;
	HUINT16					 usAreaCode;
	HUINT8					 ucGuardInterval;
	HUINT8					 ucTransmissionMode;
	HUINT8					 ucNumFreq;
	HUINT16					*pusFrequency;
} SIxTerrestrialDeliverySystemDes_t;

/* in NIT Partial Reception Descriptor (0xFB) */
/* eSIxDESCTAG_ARIB_PARTIAL_RECEPTION (0xFB) */
typedef struct
{
	SIxDescriptorHeader_t				 stHead;
	HUINT8					 ucNumLoops;
	HUINT16					*pusServiceId;
} SIxPartialReceptionDes_t;

/* in PMT and NIT Emergency Information Descriptor (0xFC) */
/* eSIxDESCTAG_ARIB_EMERGENCY_INFORMATION (0xFC) */
typedef struct
{
	SIxDescriptorHeader_t				  stHead;
	HUINT8					  ucNumServices;
	HUINT16					 *pusServiceId;
	HBOOL					 *pbStartEndFlag;
	HBOOL					 *pbSignalLevel;
	HUINT8					 *pucAreaCodeLen;
	HUINT16					**pausAreaCode;
} SIxEmergencyInformationDes_t;

/* in PMT Data Component Descriptor (0xFD) */
/* eSIxDESCTAG_ARIB_DATA_COMPONENT (0xFD) */
typedef struct
{
	HUINT8				ucDMF;
	HUINT8				ucTiming;
} SIxAdditionalAribCaptionInfo_t;

typedef struct
{
	HUINT8				ucDataEventId;
	HUINT8				ucEventSectionFlag;
} SIxAdditionalAribCarouselInfo_t;

typedef struct
{
	HUINT8							ucTransmissionFormat;
	HUINT8							ucEntryPointFlag;
	HUINT8							ucAutoStartFlag;
	HUINT8							ucDocumentResolution;
	HUINT8							ucUseXml;
	HUINT8							ucDefaultVersionFlag;
	HUINT8							ucIndependentFlag;
	HUINT8							ucStyleForTvFlag;
	HUINT16							usBmlMajorVersion;
	HUINT16							usBmlMinorVersion;
	HUINT16							usBxmlMajorVersion;
	HUINT16							usBxmlMinorVersion;
	SIxAdditionalAribCarouselInfo_t	stCarouselInfo; /* ARIB STD-B24 제3편 부록규정 C.1 */
	HUINT8							ucOndemandRetrievalFlag;
	HUINT8							ucFileStorableFlag;
} SIxAdditionalAribBxmlInfo_t;

typedef struct
{
	SIxDescriptorHeader_t						 stHead;
	HUINT16							 usDataComponentId;
	HUINT8							 ucNumCaptionInfo;
	SIxAdditionalAribCaptionInfo_t	*pstCaptionInfo;	 /* ARIB STD-B24 제1편 제3부 표 9-15 */
	HUINT8							 ucNumBxmlInfo;
	SIxAdditionalAribBxmlInfo_t		*pstBxmlInfo;		 /* ARIB STD-B24 제2편 표 9-12 */
} SIxDataComponentDes_t;

/* in PMT and NIT, TLV-NIT System Management  Descriptor (0xFE) */
/* eSIxDESCTAG_ARIB_SYSTEM_MANAGEMENT (0xFE) */
typedef struct
{
	SIxDescriptorHeader_t						 stHead;
	HUINT8							 ucBroadcastingFlag;
	HUINT8							 ucBroadcastingIdentifier;
	HUINT8							 ucAdditionalBroadcastingIdentification;
	HUINT8							 ucAdditionalIdentificationInfoLen;
	HUINT8							*pucAdditionalIdentificationInfo;
} SIxSystemManagementDes_t;

/* in EIT Series Descriptor (0xD5) */
/* eSIxDESCTAG_ARIB_SERIES (0xD5) */
typedef struct
{
	SIxDescriptorHeader_t						 stHead;
	HUINT16							 usSeriesId;
	HUINT8							 ucRepeatLabel;
	HUINT8							 ucProgramPattern;
	HUINT8							 ucExpireDateValidFlag;
	HUINT8							 ucExpireDate;
	HUINT8							 ucEpisodeNumber;
	HUINT16							 usLastEpisodeNumber;
	HUINT16							 usSeriesNameCharLen;
	HUINT8							*pucSeriesNameChar;
} SIxSeriesDes_t;

/* in EIT Event Group Descriptor (0xD6) */
/* eSIxDESCTAG_ARIB_EVENT_GROUP */
typedef struct
{
	HUINT16							usOrigNetworkId;
	HUINT16							usTransStreamId;
	HUINT16							usServiceId;
	HINT32	nEventId;
} SIxAribEventGroupOtherNetworkInfo_t;

typedef struct tagSIxEventGroupDes
{
	SIxDescriptorHeader_t							 stHead;
	HUINT8								 ucGroupType;
	HUINT8								 ucEventCount;
	HUINT16								*pusServiceId;
	HINT32 *pusEventId;
	HUINT8								 ucNumLoop;
	SIxAribEventGroupOtherNetworkInfo_t	*pstOtherNetworkInfo;
	HUINT8								 ucPrivateDataByteLen;
	HUINT8								*pucDataByte;
	struct tagSIxEventGroupDes 			*pstNext;
} SIxEventGroupDes_t;

/* in BIT SI Parameter Descriptor (0xD7) */
/* eSIxDESCTAG_ARIB_SI_PARAMETER (0xD7) */
typedef struct
{
	HUINT8								 ucTableId;
	HUINT8								 ucTableDescriptionLength;
	HUINT8								*pucTableDescriptionByte; /* Which specification has this description? */
} SIxAribSiParameterDesTableLoop_t;

typedef struct
{
	SIxDescriptorHeader_t							 stHead;
	HUINT8								 ucParameterVersion;
	HUINT16								 usUpdateTime;
	HUINT8								 ucNumParameters;
	SIxAribSiParameterDesTableLoop_t		*pstTableLoop;
} SIxSiParameterDes_t;

/* in BIT Broadcaster Name Descriptor (0xD8) */
/* eSIxDESCTAG_ARIB_BROADCASTER_NAME (0xD8) */
typedef struct
{
	SIxDescriptorHeader_t							 stHead;
	HUINT8								 ucBroadcasterNameLen;
	HUINT8								*pucBroadcasterName;
} SIxBroadcasterNameDes_t;

/* in EIT Component Group Descriptor (0xD9) */
/* eSIxDESCTAG_ARIB_COMPONENT_GROUP (0xD9) */
typedef struct
{
	HUINT8							 ucCaUnitId;
	HUINT8							 ucNumOfComponent;
	HUINT8							*pucComponentTag;
} SIxAribComponentGroupDesCaUnit_t;

typedef struct
{
	HUINT8								 ucComponentGroupId;
	HUINT8								 ucNumOfCaUnit;
	SIxAribComponentGroupDesCaUnit_t		*pstCaUnit;
	HUINT8								 ucTotalBitRate;
	HUINT8								 ucTextLen;
	HUINT8								*pucTextChar;
} SIxAribComponentGroupDesGroup_t;

typedef struct
{
	SIxDescriptorHeader_t							 stHead;
	HUINT8								 ucComponentGroupType;
	HUINT8								 ucTotalBitRateFlag;
	HUINT8								 ucNumOfGroup;
	SIxAribComponentGroupDesGroup_t		*pstGroup;
} SIxComponentGroupDes_t;

/* in PMT/SDT/EIT Content Availability Descriptor (0xDE) */
/* eSIxDESCTAG_ARIB_CONTENT_AVAILABILITY (0xDE) */
typedef struct
{
	SIxDescriptorHeader_t						stHead;
	HUINT8							ucCopyRestrictionMode;
	HUINT8							ucImageContraintToken;
	HUINT8							ucRetentionMode;
	HUINT8							ucRetentionState;
	HUINT8							ucEncryptionMode;
} SIxContentAvailabilityDes_t;

#ifdef __cplusplus
}
#endif

#endif // __SILIB_ARIB_H__


