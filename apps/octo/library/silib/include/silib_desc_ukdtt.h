/* $Header: $ */

#ifndef __SILIB_UKDTT_H__
#define __SILIB_UKDTT_H__

#ifndef _____INCLUDED_ONLY_IN_SILIB_H____
#error The file [silib_desc_ukdtt.h] shall not be included except silib.h
#endif

#ifdef __cplusplus
extern "C" {
#endif


/** @brief eSIxDESCTAG_UKDTT_LOGICAL_CHANNEL(0x83) */
typedef struct tagSIxUkdttLogicalChannelInfo
{
	HUINT16							usServiceId;
	HUINT8							ucVisibleServiceFlag;
	HUINT16							usLogicalChannelNumber;
	struct tagSIxUkdttLogicalChannelInfo	*pstNext;
} SIxUkdttLogicalChannelInfo_t;

typedef struct
{
	SIxDescriptorHeader_t			stHead;
	SIxUkdttLogicalChannelInfo_t		*pstInfo;
} SIxUkdttLogicalChannelDes_t;

// @brief eSIxDESCTAG_UKDTT_PREFERRED_NAME_LIST (0x84)
typedef struct tagSIxUkdttPreferredName
{
	HUINT8		 ucNameId;
	HUINT8		*szName;
} PrefNameListName_t;

typedef struct tagSIxUkdttPreferredNameListInfo
{
	HUINT8				szIso639LangCode[4];
	HUINT8		 		ucNumName;
	PrefNameListName_t	*astName;
} SIxUkdttPreferredNameListInfo_t;

typedef struct 
{
	SIxDescriptorHeader_t			stHead;
	HUINT32		 				ulNumNameInfo;
	SIxUkdttPreferredNameListInfo_t	*astNameInfo;
} SIxUkdttPreferredNameListDesc_t;

// @brief eSIxDESCTAG_UKDTT_PREFERRED_NAME_IDENTIFIER (0x85)
typedef struct 
{
	SIxDescriptorHeader_t		stHead;
	HUINT8					ucNameId;
} SIxUkdttPrefNameIdenDes_t;

/** @brief eSIxDESCTAG_UKDTT_SERVICE_ATTRIBUTE (0x86) */
typedef struct tagSIxUkdttSvcAttrInfo
{
   HUINT16	usServiceId;
   HBOOL		bNumericSelectFlag;
   HBOOL		bVisibleSvcFlag;
   struct tagSIxUkdttSvcAttrInfo    *pstNext;
} SIxUkdttSvcAttrInfo_t;

typedef struct
{
	SIxDescriptorHeader_t			stHead;
	HUINT32						ulNumAttrInfo;
	SIxUkdttSvcAttrInfo_t 			*pstInfo;
} SIxUkdttSvcAttrDes_t;

// @breaf eSIxDESCTAG_UKDTT_SHORT_SERVICE_NAME (0x87)
typedef struct 
{
	SIxDescriptorHeader_t			stHead;
	HUINT8						*pszSvcName;
} SIxUkdttShortSvcNameDes_t;

/** @breaf eSIxDESCTAG_UKDTT_HD_SIMULCAST_LCN (0x88) */
typedef struct tagSIxUkdttHDSimulLcnInfo
{
	HUINT16		usSvcId;
	HUINT8		bVisibleSvcFlag;
	HUINT16		usLcnNumber;
	struct tagSIxUkdttHDSimulLcnInfo	 *pstNext;
} SIxUkdttHDSimulLcnInfo_t;

typedef struct
{
	SIxDescriptorHeader_t			stHead;
	HUINT32						ulNumLcn;
	SIxUkdttHDSimulLcnInfo_t			*pstInfo;
} SIxUkdttHDSimulcastLcnDes_t;

/** @brief Private Data ±¸Á¶ */
typedef struct 
{
	HUINT8							ucPrivateDataLen;
	HUINT8							*pPrivateData;
} SIxUkdttPrivateDataInfo_t;

/** @breaf eSIxDESCTAG_UKDTT_GUIDANCE (0x89) */
typedef struct tagSIxUkdttGuidanceInfo
{
	HUINT8							ucGuidanceMode;
	HUINT8  						szIso639LangCode[4];
	HUINT8  						*pszGuidance;
} SIxUkdttGuidanceInfo_t, *pSIxUkdttGuidanceInfo_t;

typedef struct 
{
	SIxDescriptorHeader_t			stHead;
	HUINT8						ucGuidanceType;
	union
	{
		// In case of guidance type 0 and 1
		SIxUkdttGuidanceInfo_t		*pUkDttGuidanceInfo;
		// In case of other value
		SIxUkdttPrivateDataInfo_t	*pPrivateData;
	} unInfo;
} SIxUkdttGuidanceDes_t, *pSIxUkdttGuidanceDes_t;


#ifdef __cplusplus
}
#endif

#endif // __SILIB_UKDTT_H__


