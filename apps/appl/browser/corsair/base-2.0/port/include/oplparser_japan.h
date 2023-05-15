/**************************************************************************************/
/**
 * @file oplparser.h
 *
 * Programme_ngs
 *
 * @author  Seung bum kim (sbkim@humaxdigital.com)
 * @date    2012/09/20
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#ifndef __OPLPARSER_H__
#define __OPLPARSER_H__

#include "oplcollection.h"
#include <hlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define	ARIB_DES_CHECK_MEM(p)		if (p == NULL){HxLOG_Print("\n\nMemory Allocation Failed. %s %d\n\n", __FILE__, __LINE__);}
#define	ARIB_DES_EOS					'\0'

#define 	TAG_PRIVATE_DATA				0x00
#define 	TAG_CA							0x09	/* 0x09 CA Descriptor (PMT)								*/
#define 	TAG_DIGITAL_COPY_CONTROL		0xC1 	/* 0xC1 Digital copy control descriptor (PMT)				*/
#define 	TAG_AUDIO_COMPONENT				0xC4	/* 0xC4 Audio component descriptor (EIT)          			*/
#define 	TAG_DATA_CONTENTS				0xC7	/* 0xC7 Data contents descriptor (EIT)            			*/
#define 	TAG_EVENT_GROUP 				0xD6 	/* 0xD6 Event group descriptor								*/
#define 	TAG_COMPONENT_GROUP				0xD9	/* 0xD9 Component group descriptor (EIT)          			*/
#define 	TAG_CONTENT_AVAILABILITY		0xDE	/* 0xDE Content availability descriptor (EIT)     			*/
#define 	TAG_CA_CONTRACT_INFOMATION 		0xCB	/* 0xCB CA contract information descriptor (EIT)			*/

#define	TAG_ALL_DESCRIPTORS			0xFFFFFFFF

#define	MAX_DESCRIPTOR_LIMIT	(unsigned) 255

/* EIT Privte Tag - oplprogramme 에서 파싱함 */
#define ____EIT_PRIVATE____
typedef struct tagEventDigitalCopyControlDesComponent
{
	HUINT8	ucComponentTag;
	HUINT8	ucDigitalRecordingControlData;
	HUINT8	ucMaximumBitRateFlag;
	HUINT8 	ucCopyControlType;
	HUINT8	ucAPSControlData;
	HUINT8	ucMaximumBitRate;
}APKS_EventDigitalCopyControlComponent_t;

typedef struct tagEventDigitalCopyControlDes
{
	HUINT8	ucDesTag;
	HUINT8	ucDigitalRecordingControlData;
	HUINT8	ucMaximumBitRateFlag;
	HUINT8	ucComponentControlFlag;
	HUINT8	ucCopyControlType;
	HUINT8	ucAPSControlData;
	HUINT8	ucMaximumBitRate;
	HUINT8	ucNumComponents;
	APKS_EventDigitalCopyControlComponent_t	*pstComponent;
}APKS_EventDigitalCopyControl_t;

typedef struct tagEventAudioComponentDes
{
	HUINT8	ucDesTag;
	HUINT8	ucStreamContent;
	HUINT8	ucComponentType;
	HUINT8	ucComponentTag;
	HUINT8	ucStreamType;
	HUINT8	ucSimulcastGroupTag;
	HBOOL	bESMultiLlingualFlag;
	HBOOL	bMainComponentFlag;
	HUINT8	ucQualityIndicator;
	HUINT8	ucSamplingRate; /* 1 - 16k, 2-22.05k, 3-24k, 5-32k, 6-44.1k, 7-48k */
	HUINT32	unISO639LanguageCode; /* jpn - 0x6A706E */
	HUINT32	unISO639LanguageCode2;
	HUINT8	ucTextCharLen;
	HUINT8	*pucTextChar;
	struct tagEventAudioComponentDes	*pstNext;
}APKS_EventAudioComponent_t;

typedef struct
{
	HUINT8	ucComponentTag;
	HUINT8	ucEventSectionFlag;
	HUINT8 	ucComponentSizeFlag;
	HUINT8	ucDefaultTransactionIdFlag;
	HUINT8	ucDefaultTimeoutDIIFlag;
	HUINT8	ucDefaultLeakRateFlag;
	HUINT32	unComponentSize;
	HUINT32	unTransactionId;
	HUINT32	unTimeoutValueDII;
	HUINT32	unLeakRate;
}AribCarouselComponent;

typedef struct
{
	HUINT8	unNumOfCarousels;
	AribCarouselComponent *pstCarouselComponent;
}AribCarouselInfo;

typedef struct
{
	HUINT8	ucComponentTag;
	HUINT8	ucNumDataEventFlag;
	HUINT8 	ucNumModulesFlag;
	HUINT8	ucNumResourcesFlag;
	HUINT8	ucCompressedComponentSizeFlag;
	HUINT8	ucComponentSizeFlag;
	HUINT8	ucDefaultTransactionIdFlag;
	HUINT8	ucDefaultTimeoutDIIFlag;
	HUINT8	ucDefaultLeakRateFlag;
	HUINT16	usNumDataEvent;
	HUINT32	unNumModules;
	HUINT32	unNumResoures;
	HUINT32	unCompressedComponentSize;
	HUINT32	unComponentSize;
	HUINT32	unTransactionId;
	HUINT32	unTimeoutValueDII;
	HUINT32	unLeakRate;
}AribStoredCarouselComponent;

typedef struct
{
	HUINT8	unNumOfCarousels;
	AribStoredCarouselComponent *pstStoredCarouselComponent;
}AribStoredCarouselInfo;

typedef struct
{
	HUINT8	ucTransmissionFormat;		/* 2 bit */
	HUINT8	ucAutoStartFlag;				/* 1 bit */
	HUINT8	ucDocumentResolution;		/* 4 bit */
	HUINT8	ucUseXml;					/* 1 bit */
	HUINT8	ucDefaultVersionFlag;		/* 1 bit */
	HUINT8	ucIndependentFlag;			/* 1 bit */
	HUINT8	ucContentIdFlag;			/* 1 bit */
	HUINT8	ucAssociatedContentsFlag;	/* 1 bit */
	HUINT8	ucStyleForTvFlag;			/* 1 bit */
	HUINT8	ucUpdateFlag;				/* 1 bit */
	HUINT32	unISO639LanguageCode;		/* 24 bit */
	HUINT32	unContentId;				/* 32 bit */
	HUINT16	usContentVersion;			/* 16 bit */
	HUINT16	usBmlMajorVersion;			/* 16 bit */
	HUINT16	usBmlMinorVersion;			/* 16 bit */
	HUINT16	usBxmlMajorVersion;			/* 16 bit */
	HUINT16	usBxmlMinorVersion;			/* 16 bit */
	AribCarouselInfo	*pstCarouselInfo;				/* ARIB STD-B24 3분책 C.2.1 */
	HUINT8	ucOndemandRetrievalFlag;
	HUINT8	ucFileStorableFlag;
	AribStoredCarouselInfo	*pstStoredCarouselInfo;		/* ARIB STD-B24 3분책 C.2.2 */
} AribBxmlInfo;

typedef struct
{
	HUINT8	ucNumLanguages;
	HUINT8	*pucLanguageTag;			/* 3 bit */
	HUINT8	*pucDMF;					/* 4 bit */
	HUINT32	*punISO639LanguageCode;	/* 24 bit */
} AribCaptionInfo;

typedef struct tagEventDataContentDes
{
	HUINT8			ucDesTag;
	HUINT16			usDataComponentId;
	HUINT8			ucEntryComponent;
	HUINT8			ucSelectorLen;
	AribBxmlInfo		*pstAribBxmlInfo;			/* ARIB STD-B24 2분책 9.3.3 */
	AribCaptionInfo	*pstAribCaptionInfo;		/* ARIB STD-B24 1분책 9.6.2 */
	HUINT8			ucNumOfComponentRef;
	HUINT8			*pucComponentRef;
	HUINT32			unISO639LanguageCode;
	HUINT8			ucTextLen;
	HUINT8			*pucTextChar;
	struct tagEventDataContentDes	*pstNext;
} APKS_EventDataContentDes_t;

typedef struct
{
	HUINT16	usOrigNetworkId;
	HUINT16	usTransStreamId;
	HUINT16	usServiceId;
	HINT32	nEventId;
}AribEventGroupOtherNetworkInfo;

typedef struct tagEventGroupDes
{
	HUINT8	ucDesTag;
	HUINT8	ucGroupType;
	HUINT8	ucEventCount;
	HUINT16	*pusServiceId;
	HINT32 *pusEventId;
	HUINT8	ucNumLoop;
	AribEventGroupOtherNetworkInfo	*pstOtherNetworkInfo;
	HUINT8	ucPrivateDataByteLen;
	HUINT8	*pucDataByte;
	struct tagEventGroupDes *pstNext;
}APKS_EventGroupDes_t;

typedef struct
{
	HUINT8	ucCaUnitId;
	HUINT8	ucNumOfComponent;
	HUINT8	*pucComponentTag;
}AribComponentGroupDesCaUnit;

typedef struct
{
	HUINT8	ucComponentGroupId;
	HUINT8	ucNumOfCaUnit;
	AribComponentGroupDesCaUnit	*pstCaUnit;
	HUINT8	ucTotalBitRate;
	HUINT8	ucTextLen;
	HUINT8	*pucTextChar;
}AribComponentGroupDesGroup;

typedef struct tagEventComponentGroupDes
{
	HUINT8	ucDesTag;
	HUINT8	ucComponentGroupType;
	HUINT8	ucTotalBitRateFlag;
	HUINT8	ucNumOfGroup;
	AribComponentGroupDesGroup	*pstGroup;
} APKS_EventComponentGroupDes_t;

typedef struct tagEventContentAvailabilityDes
{
	HUINT8	ucDesTag;
	HUINT8	ucCopyRestrictionMode;
	HUINT8	ucImageContraintToken;
	HUINT8	ucRetentionMode;
	HUINT8	ucRetentionState;
	HUINT8	ucEncryptionMode;
} APKS_EventContentAvailabilityDes_t;

/* in EIT/SDT CA Contract Info Descriptor */
typedef struct tagCaContractInfoDes
{
	HUINT8	ucDesTag;
	HUINT16	usCASystemId;
	HUINT8	ucCAUnitId;
	HUINT8	ucNumOfComponent;
	HUINT8	*pucComponentTag;
	HUINT8	ucContractVerificationInfoLength;
	HUINT8	*pucContractVerificationInfo;
	HUINT8	ucFeeNameLength;
	HUINT8	*pucFeeName;
	struct tagCaContractInfoDes *pstNext;
}APKS_CaContractInfoDes_t;

typedef struct tagPRIVATE_DATA_INFO
{
	HUINT8	ucPrivateDataLen;
	HUINT8	*pPrivateData;
} APKS_PrivateDataInfo_t;

/** @brief TAG_CA (0x09) */
typedef struct tagCA_DES
{
	HUINT8	ucDesTag;
	HUINT16	usCaSysId;
	HUINT16	usCaPid;
	APKS_PrivateDataInfo_t	*pPrivateDataInfo;
}APKS_CaDes_t;


/*
	Arib Spec : 6-STD-B10v4_6-E2.pdf 206p (data_component_id)

	0x0007	ARIB-XML-base multimedia coding
	0x0008	ARIB-Subtitle& teletext coding
	0x0009	ARIB-Data download
	0x000A	G-guide (G-Guide Gold)
	0x000B	BML for 110°E CS
	0x000C	Multimedia coding for digital terrestrial broadcasting(A profile)
	0x000D	Multimedia coding for digital terrestrial broadcasting(C profile)
	0x000E	Multimedia coding for digital terrestrial broadcasting(P profile)
	0x000F	Multimedia coding for digital terrestrial broadcasting(E profile)
	0x0010	Real-time data service(Mobile profile)
	0x0011	Accumulation-type data service (Mobile profile)
	0x0012	Subtitle coding for digital terrestrial broadcasting(C profile)
	0x0013	Multimedia coding for digital terrestrial broadcasting(P2 profile)
	0x0014	Data carousel scheme for TYPE2 content transmission
	0x0015	DSM-CC section scheme for transmission of program start time information
	0x0016	ARIB-Descriptive language type metadata coding

*/
enum
{
	eAribDataCompId_None						= 0x0000,

	eAribDataComId_ARIB_XML_base_multimedia		= 0x0007,
	eAribDataComId_ARIB_Sub_txt_coding			= 0x0008,
	eAribDataComId_ARIB_Data_download			= 0x0009,
	eAribDataComId_G_guide						= 0x000A,
	eAribDataComId_BML_110E_CS					= 0x000B,
	eAribDataComId_Multimedia_DTT_A				= 0x000C,
	eAribDataComId_Multimedia_DTT_C				= 0x000D,
	eAribDataComId_Multimedia_DTT_P				= 0x000E,
	eAribDataComId_Multimedia_DTT_E				= 0x000F,
	eAribDataComId_Realtime_dataservice			= 0x0010,
	eAribDataComId_Accumulation_dataservice		= 0x0011,
	eAribDataComId_Subtitle_DTT_C				= 0x0012,
	eAribDataComId_Multimedia_DTT_P2				= 0x0013,
	eAribDataComId_Data_Carousel_TYPE2			= 0x0014,
	eAribDataComId_DSMCC_Sec_StartTime			= 0x0015,
	eAribDataComId_ARIB_Delang_metadata			= 0x0016,

	eAribDataCompId_Max
};

#define ____EIT_PRIVATE____

void  	OPL_PARSER_JAPAN_FreeDigitalCopyControlDes(void *pucDesc);
HUINT8*	OPL_PARSER_JAPAN_AllocDigitalCopyControlDes(void);
HERROR  OPL_PARSER_JAPAN_ParseDigitalCopyControlDes(HUINT8 *pucRaw, HUINT8 *pucDes);
void  	OPL_PARSER_JAPAN_PrintDigitalCopyControlDes(HUINT8 *pucDes);

void  	OPL_PARSER_JAPAN_FreeAudioComponentDes(void *pucDesc);
HUINT8*	OPL_PARSER_JAPAN_AllocAudioComponentDes(void);
HERROR  OPL_PARSER_JAPAN_ParseAudioComponentDes(HUINT8 *pucRaw, HUINT8 *pucDes);
void  	OPL_PARSER_JAPAN_PrintAudioComponentDes(HUINT8 *pucDes);

void  	OPL_PARSER_JAPAN_FreeDataContentsDes(void *pucDesc);
HUINT8* 	OPL_PARSER_JAPAN_AllocDataContentsDes(void);
HERROR  OPL_PARSER_JAPAN_ParseDataContentsDes(HUINT8 *pucRaw, HUINT8 *pucDes);
void  	OPL_PARSER_JAPAN_PrintDataContentsDes(HUINT8 *pucDes);

void  	OPL_PARSER_JAPAN_FreeEventGroupDes(void *pucDesc);
HUINT8*	OPL_PARSER_JAPAN_AllocEventGroupDes(void);
HERROR  OPL_PARSER_JAPAN_ParseEventGroupDes(HUINT8 *pucRaw, HUINT8 *pucDes);
void  	OPL_PARSER_JAPAN_PrintEventGroupDes(HUINT8 *pucDes);

void  	OPL_PARSER_JAPAN_FreeEventComponentGroupDes(void *pucDesc);
HUINT8*	OPL_PARSER_JAPAN_AllocEventComponentGroupDes(void);
HERROR  OPL_PARSER_JAPAN_ParseEventComponentGroupDes(HUINT8 *pucRaw, HUINT8 *pucDes);
void  	OPL_PARSER_JAPAN_PrintEventComponentGroupDes(HUINT8 *pucDes);

void  	OPL_PARSER_JAPAN_FreeEventContentAvailabilityDes(void *pucDesc);
HUINT8*	OPL_PARSER_JAPAN_AllocEventContentAvailabilityDes(void);
HERROR  OPL_PARSER_JAPAN_ParseEventContentAvailabilityDes(HUINT8 *pucRaw, HUINT8 *pucDes);
void  	OPL_PARSER_JAPAN_PrintEventContentAvailabilityDes(HUINT8 *pucDes);

void  	OPL_PARSER_JAPAN_FreeCaContractInfoDes(void *pucDesc);
HUINT8*	OPL_PARSER_JAPAN_AllocCaContractInfoDes(void);
HERROR  OPL_PARSER_JAPAN_ParseCaContractInfoDes(HUINT8 *pucRaw, HUINT8 *pucDes);
void  	OPL_PARSER_JAPAN_PrintCaContractInfoDes(HUINT8 *pucDes);

void 	OPL_PARSER_JAPAN_FreeCaDes(void *pucDesc);
HUINT8*	OPL_PARSER_JAPAN_AllocCaDes(void);
HERROR	OPL_PARSER_JAPAN_ParseCaDes(HUINT8 *pucRaw, HUINT8 *pucDes);
void  	OPL_PARSER_JAPAN_PrintCaDes(HUINT8 *pucDes);

HxVector_t *OPL_PARSER_JAPAN_ParsingDesc( HxVector_t *descriptor, HINT32 tag );
HERROR OPL_PARSER_JAPAN_GetPmtProgramDescriptorFromRaw (HUINT8 *pucRaw, HUINT32 ulTag, HUINT32 ulDesIdx, HxVector_t *pDest);

#ifdef __cplusplus
};
#endif

#endif


