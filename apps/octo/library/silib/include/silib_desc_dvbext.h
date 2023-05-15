/* $Header: $ */

#ifndef __SILIB_DVBEXT_H__
#define __SILIB_DVBEXT_H__

#ifndef _____INCLUDED_ONLY_IN_SILIB_H____
#error The file [silib_desc_dvbext.h] shall not be included except silib.h
#endif

#ifdef __cplusplus
extern "C" {
#endif

// TAG_EXTENSION(0x7F)'s SUB descriptor eSIxDESCTAG_EXT_IMG_ICON(0x00)
typedef struct
{
	HUINT8			 ucDataLen;
	HUINT8			*pucImgData;
} SIxImageIconDataInfo_t;

typedef struct
{
	HUINT16			 usUrlLen;
	HUINT8			*pucImgUrl;
} SIxImageIconURLInfo_t;

typedef struct
{
	HUINT16			 usTypeLen;
	HUINT8			*pucImgType;
} SIxImageIconTypeInfo_t;

typedef struct
{
	HUINT8						 ucIconTransMode;
	HUINT8						 ucIconPosFlag;
	HUINT8						 ucCoordSystem;
	HUINT16						 usIconHorOrigin;
	HUINT16						 usIconVerOrigin;
	SIxImageIconTypeInfo_t		*pstImgTypeInfo;
	SIxImageIconURLInfo_t		*pstImgUrlInfo;
	SIxImageIconDataInfo_t		*pstImgIconInfo;
} SIxImageIconHeadInfo_t;

typedef struct
{
	SIxDescriptorHeader_t					 stHead;
	HUINT8						 ucDesNum;
	HUINT8						 ucLastDesNum;
	HUINT8						 ucIconId;
	SIxImageIconHeadInfo_t		*pstImgHeadInfo;
	SIxImageIconDataInfo_t		*pstImgIconInfo;
} SIxImageIconDes_t;

// TAG_EXTENSION(0x7F)'s SUB descriptor eSIxDESCTAG_EXT_T2_DELIVERY_SYSTEM(0x04)
typedef struct
{
	HUINT8								ucCellIdExtension;
	HUINT32								ulTransponderFreq;
} SIxT2DeliverySubCellInfo_t;

typedef struct tagSIxDeliveryCellInfo
{
	HUINT16								usCellId;
	HUINT8								ucFreqLoopCnt;
	HUINT32								*pulCentreFreq;
	HUINT8								ucSubCellInfoLoopCnt;
	SIxT2DeliverySubCellInfo_t			*pstInfo;
	struct tagSIxDeliveryCellInfo		*pstNext;
} SIxT2DeliveryCellInfo_t;

typedef struct
{
	HUINT8						 ucSMISO;
	HUINT8						 ucBandWidth;
	HUINT8						 ucGuardInterval;
	HUINT8						 ucTransmissionMode;
	HUINT8						 ucOtherFreqFlag;
	HUINT8						 ucTfsFlag;
	SIxT2DeliveryCellInfo_t		*pstInfo;
} SIxT2DeliveryDetailInfo_t;

typedef struct
{
	SIxDescriptorHeader_t					 stHead;
	HUINT8						 ucPlpId;
	HUINT16						 usT2SystemId;
	SIxT2DeliveryDetailInfo_t	*pstDetailInfo;
} SIxT2DeliverySystemDes_t;

// TAG_EXTENSION (0x7F)'s SUB descriptor eSIxDESCTAG_EXT_SUPPLEMENTARY_AUDIO(0x06)
typedef struct
{
	SIxDescriptorHeader_t		stHead;
	HUINT8						ucMixType;
	HUINT8					 	ucEditorialClassification;
	HUINT8						ucLanguageCodePresent;
	HUINT8						ISO_639[4];
	SIxPrivateDataInfo_t		*pstPrivateDataInfo;
} SIxSupplementaryAudioDes_t;

// TAG_EXTENSION(0x7F)'s SUB descriptor eSIxDESCTAG_EXT_NETWORK_CHANGE_NOTIFY(0x07)
typedef struct
{
	HUINT16		usMJD;
	HUINT32		ulBCD;
} SIxUtcTimeInfo_t;

typedef struct tagSIxNetChangeNotifySubInfo
{
	HUINT8									 ucNetChangeId;
	HUINT8									 ucNetChangeVer;
	HUINT8									 ucStartTimeOfChange[5];
	HUINT32									 ulBcdChangeDur;
	HUINT8									 ucReceiverCategory;
	HUINT8									 ucInvariantTsPresent;
	HUINT8									 ucChangeType;
	HUINT8									 ucMessageId;
	HUINT16									 usInvariantTsId;
	HUINT16									 usInvariantOnId;
	struct tagSIxNetChangeNotifySubInfo		*pstNext;
} SIxNetChangeNotifySubInfo_t;

typedef struct tagSIxNetChangeNotifyCellInfo
{
	HUINT16									usCellId;
	SIxNetChangeNotifySubInfo_t				*pstInfo;
	struct tagSIxNetChangeNotifyCellInfo	*pstNext;
} SIxNetChangeNotifyCellInfo_t;

typedef struct
{
	SIxDescriptorHeader_t					stHead;
	SIxNetChangeNotifyCellInfo_t			*pstInfo;
} SIxNetChangeNotifyDes_t;

// TAG_EXTENSION (0x7F)'s SUB descriptor eSIxDESCTAG_EXT_MESSAGE(0x08)
typedef struct
{
	SIxDescriptorHeader_t				stHead;
	HUINT8								ucMessageId;
	HUINT8								szIso639LangCode[4];
	HUINT8								*pszMessageText;
} SIxMessageDes_t;


// TAG_EXTENSION(0x7F)'s SUB descriptor eSIxDESCTAG_EXT_TARGET_REG(0x09)
typedef struct
{
	HUINT8								ucPrimaryRegionCode;
	HUINT8								ucSecondaryRegionCode;
	HUINT16								usTertiaryRegionCode;
} SIxTargetRegionCode_t;

typedef struct tagSIxTargetRegInfo
{
	HUINT8								 ucCountryCodeFlag;
	HUINT8								 ucRegionDepth;
	HUINT8								 ucCountryCode[4];
	SIxTargetRegionCode_t				 stTargetRegionInfo;
	struct tagSIxTargetRegInfo			*pstNext;
} SIxTargetRegionInfo_t;

typedef struct
{
	SIxDescriptorHeader_t				stHead;
	HUINT8								ucCountryCode[4];
	SIxTargetRegionInfo_t				*pstInfo;
} SIxTargetRegionDes_t;

// TAG_EXTENSION(0x7F)'s SUB descriptor eSIxDESCTAG_EXT_TARGET_REG_NAME(0x0A)
typedef struct tagSIxTargetRegNameInfo
{
	HUINT8								ucRegionDepth;
	HUINT8								ucRegionNameLen;
	HUINT8								*pszRegionName;
	SIxTargetRegionCode_t				stTargetRegionInfo;
	struct tagSIxTargetRegNameInfo		*pstNext;
} SIxTargetRegionNameInfo_t;

typedef struct
{
	SIxDescriptorHeader_t				stHead;
	HUINT8								ucCountryCode[4];
	HUINT8								ucIso639LangCode[4];
	SIxTargetRegionNameInfo_t			*pstInfo;
} SIxTargetRegionNameDes_t;


// TAG_EXTENSION (0x7F)'s SUB descriptor eSIxDESCTAG_URI_LINKAGE(0x13)
typedef struct
{
	SIxDescriptorHeader_t				stHead;
	HUINT8								ucUri_linkage_type;
	HUINT8						 		ucUriCharLen;
	HUINT8								*pszUriChar;
	HUINT16								ucMin_polling_interval;
	SIxPrivateDataInfo_t				*pstPrivateDataInfo;
} SIxUriLinkageDes_t;


#ifdef __cplusplus
}
#endif

#endif // __SILIB_DVBEXT_H__


