/* $Header: $ */

#ifndef __SILIB_FSAT_H__
#define __SILIB_FSAT_H__

#ifndef _____INCLUDED_ONLY_IN_SILIB_H____
#error The file [silib_desc_fsat.h] shall not be included except silib.h
#endif

#ifdef __cplusplus
extern "C" {
#endif

// eSIxDESCTAG_FSAT_TUNNELLED_DATA					( 0xD0 )
typedef struct
{
	HUINT8			ucTunnelledDataType;
	HUINT8			ucComponentTag;
} SIxTunnelledDataInfo_t;

typedef struct
{
	SIxDescriptorHeader_t				 stHead;
	HUINT32					 ulNumInfo;
	SIxTunnelledDataInfo_t	*astInfo;
} SIxTunnelledDataDes_t;

// eSIxDESCTAG_FSAT_ALT_TUNNELLED_DATA				( 0xD1 )
typedef struct
{
	HUINT8					ucTunnelledDataType;
} SIxAltTunnelledDataInfo_t;

typedef struct
{
	SIxDescriptorHeader_t					 stHead;
	HUINT32						 ulNumInfo;
	SIxAltTunnelledDataInfo_t	*astInfo;
} SIxAltTunnelledDataDes_t;

// eSIxDESCTAG_FSAT_LINKAGE						( 0xD2 )
typedef struct tagSIxFsatLinkageInfo
{
	HUINT16							 usTsId;
	HUINT16							 usOrgNetId;
	HUINT16							 usSvcId;
	HUINT8							 ucStandardTypeNum;
	HUINT8							 ucTunnelledTypeNum;
	HUINT8							*aucStandardLinkageType;
	HUINT8							*aucTunnelledLinkageType;
	struct tagSIxFsatLinkageInfo	*pstNext;
} SIxFSatLinkageInfo_t;

typedef struct
{
	SIxDescriptorHeader_t				 stHead;
	SIxFSatLinkageInfo_t		*pstInfo;
} SIxFSatLinkageDes_t;


// eSIxDESCTAG_FSAT_REGIONALISED_LOGICAL_CHANNEL	( 0xD3 )
typedef struct tagSIxFsatLcnInfo
{
	HBIT						numeric_selection_flag:1;
	HBIT						visible_selection_flag:1;
	HBIT						user_customisable:1;
	HBIT						reserved:1;
	HBIT						logical_channel_number:12;
	HBIT						region_id:16;
	struct tagSIxFsatLcnInfo	*pstNext;
} SIxFSatLcnInfo_t;

typedef struct tagSIxFsatLcnSvcInfo
{
	HUINT16							 usSvcId;
	HUINT16							 usSvcIdentifier;
	SIxFSatLcnInfo_t 				*pstInfo;
	struct tagSIxFsatLcnSvcInfo		*pstNext;
} SIxFSatLcnSvcInfo_t;

typedef struct
{
	SIxDescriptorHeader_t			stHead;
	SIxFSatLcnSvcInfo_t				*pstInfo;
} SIxRegionalisedLcnDes_t;

// eSIxDESCTAG_FSAT_REGION_NAME				( 0xD4 )
typedef struct tagSIxRegionNameInfo
{
	HUINT16							 usRegion_id;
	HUINT8							 szIso639LangCode[4];
	HUINT8							*pszRegionName;
	struct tagSIxRegionNameInfo		*pstNext;
} SIxRegionNameInfo_t;

typedef struct
{
	SIxDescriptorHeader_t			stHead;
	SIxRegionNameInfo_t				*pstInfo;
} SIxRegionNameDes_t;

// eSIxDESCTAG_FSAT_SERVICE_GROUP				( 0xD5 )
typedef struct tagSIxFsatSvcIdentifierInfo
{
	HUINT16								 usSvcIdentifier;
	struct tagSIxFsatSvcIdentifierInfo	*pstNext;
} SIxFSatSvcIdentifierInfo_t;

typedef struct
{
	HBIT		 default_visable_flag:1;
	HBIT		 adult_channels_group_flag:1;
	HBIT		 ip_channels_group_flag:1;
	HBIT		 ca_channels_group_flag:1;
	HBIT		 reserved:4;
} SIxExtentionFlagInfo_t;

typedef struct tagSIxSvcGroupInfo
{
	HUINT8							 ucNondestructiveTuneFlag;
	HUINT8							 ucReturnChannelAccessFlag;
	HUINT8							 ucG2ExtensionFlag;
	SIxExtentionFlagInfo_t			 stFlagInfo;
	HUINT16							 usSvcGroupId;
	SIxFSatSvcIdentifierInfo_t		*pstInfo;
	struct tagSIxSvcGroupInfo	*pstNext;
} SIxSvcGroupInfo_t;

typedef struct
{
	SIxDescriptorHeader_t		stHead;
	SIxSvcGroupInfo_t			*pstInfo;
} SIxSvcGroupDes_t;

// eSIxDESCTAG_FSAT_INTERACTIVE_STORAGE		( 0xD6 )
typedef struct tagSIxInteractiveStorageInfo
{
	// if ucGroupFlag is 0, unInfo assigned by free_satellite_service_identifier (15bit)
	// if ucGroupFlag is 1, unInfo assigned by free_satellite_servicegroup_id (11bit)
	HUINT8 ucGroupFlag;
	union
	{
		HUINT16		usSvcGroupId;
		HUINT16		usSvcIdentifier;
	} unInfo;
	HUINT8								 ucNumBlocks;
	struct tagSIxInteractiveStorageInfo	*pstNext;
} SIxInteractiveStorageInfo_t;

typedef struct
{
	SIxDescriptorHeader_t					 stHead;
	HUINT8						 ucAllocationUnit;
	SIxInteractiveStorageInfo_t	*pstInfo;
} SIxInteractiveStorageDes_t;

// eSIxDESCTAG_FSAT_INFO_LOCATION				( 0xD7 )
typedef struct tagSIxFsatHomeTpInfo
{
	HUINT16						 usTsId;
	HUINT16						 usOrgNetId;
	struct tagSIxFsatHomeTpInfo	*pstNext;
} SIxFSatHomeTpInfo_t;

typedef struct tagSIxTunnelledSvcInfo
{
	HUINT16							 usTsId;
	HUINT16							 usOrgNetId;
	HUINT16							 usSvcId;
	struct tagSIxTunnelledSvcInfo	*pstNext;
} SIxTunnelledSvcInfo_t;

typedef struct
{
	SIxDescriptorHeader_t				 stHead;
	SIxFSatHomeTpInfo_t		*pstHomeTpInfo;
	SIxTunnelledSvcInfo_t	*pstTunnelledSvcInfo;
} SIxInfoLocationDes_t;

// eSIxDESCTAG_FSAT_SERVICEGROUP_NAME			( 0xD8 )
typedef struct tagSIxSvcGroupNameData
{
	HUINT8							 szIso639LangCode[4];
	HUINT8							*pszSvcGroupName;
	struct tagSIxSvcGroupNameData	*pstNext;
} SIxSvcGroupNameData_t;

typedef struct tagSIxSvcGroupNameInfo
{
	HUINT8							 ucGroupType;
	HUINT16							 usSvcGroupId;
	SIxSvcGroupNameData_t			*pstSvcGroupData;
	struct tagSIxSvcGroupNameInfo	*pstNext;
} SIxSvcGroupNameInfo_t;

typedef struct
{
	SIxDescriptorHeader_t				 stHead;
	SIxSvcGroupNameInfo_t	*pstInfo;
} SIxSvcGroupNameDes_t;

// eSIxDESCTAG_FSAT_SHORT_SERVICE_NAME			( 0xD9 )
typedef struct tagSIxShortNameInfo
{
	HUINT8						 szIso639LangCode[4];
	HUINT8						*pszShortSvcName;
	struct tagSIxShortNameInfo	*pstNext;
} SIxShortSvcNameInfo_t;

typedef struct
{
	SIxDescriptorHeader_t		stHead;
	SIxShortSvcNameInfo_t		*pstInfo;
} SIxShortSvcNameDes_t;

// eSIxDESCTAG_FSAT_GUIDANCE					( 0xDA )
typedef struct
{
	HUINT8			 	 szIso639LangCode[4];
	HUINT8				*pszGuidance;
} SIxGuidanceInfo_t;

typedef struct
{
	SIxDescriptorHeader_t		stHead;
	HUINT8 						ucGuidanceType;
	union
	{
		// In case of guidance type zero
		SIxGuidanceInfo_t	*pstInfo;
		// In case of other value
		//mwPrivateDataInfo	*pPrivateData;
	} unInfo;
} SIxGuidanceDes_t;

// eSIxDESCTAG_FSAT_INTERACTIVE_RESTRICTION	( 0xDB )
typedef struct tagSIxInteractiveRestrictionInfo
{
	HUINT16									 usSvcId;
	struct tagSIxInteractiveRestrictionInfo	*pstNext;
} SIxInteractiveRestrictionInfo_t;

typedef struct
{
	SIxDescriptorHeader_t						 stHead;
	SIxInteractiveRestrictionInfo_t	*pstInfo;
} SIxInteractiveRestrictionDes_t;


// eSIxDESCTAG_FSAT_CONTENT_MANAGEMENT			( 0xDC )
typedef struct
{
	SIxDescriptorHeader_t	stHead;
	HUINT8					ucDoNotScramble;
	HUINT8					ucNotRedistributeOverInternet;
	HUINT8					ucDoNotApplyRevocation;
} SIxContentManagementDes_t;

// eSIxDESCTAG_FSAT_ONLINE_AVAILABILITY		( 0xDE )
typedef struct
{
	SIxDescriptorHeader_t	stHead;
	HUINT8					ucMediaUsage;
	UNIXTIME				StartDateTime;
	UNIXTIME				StopDateTime;
	HBOOL					bAvailableNow;
} SIxOnlineAvailabilityDes_t;

// TAG_FREESAT_PREFIX
typedef struct tagSIxPrefixInfo
{
	HUINT8					 ucMediaURIPrefixIdx;
	HUINT8 					*pszURIPrefix;
	struct tagSIxPrefixInfo	*pstNext;
} SIxPrefixInfo_t;

typedef struct
{
	SIxDescriptorHeader_t	stHead;
	SIxPrefixInfo_t			*pstInfo;
} SIxPrefixDes_t;

// TAG_FREESAT_MEDIA_LOCATOR
typedef struct
{
	SIxDescriptorHeader_t	stHead;
	HUINT8					ucMediaType;
	HUINT8					ucMediaUsage;
	HBOOL					bMediaPrefixLocated;
	HUINT8					ucUriPrefixIdx;
	HUINT8					*pszURIChar;
} SIxMediaLocatorDes_t;

// TAG_FREESAT_SHORT_EVENT_PROMOTIONAL
typedef struct tagSIxShortEventPromotionalInfo
{
	HUINT8									 szIso639LangCode[4];
	HUINT8									*pszShortEventPromotional;
	struct tagSIxShortEventPromotionalInfo	*pstNext;
} SIxShortEventPromotionalInfo_t;

typedef struct
{
	SIxDescriptorHeader_t				stHead;
	SIxShortEventPromotionalInfo_t		*pstInfo;
} SIxShortEventPromotionalDes_t;


#ifdef __cplusplus
}
#endif

#endif // __SILIB_FSAT_H__


