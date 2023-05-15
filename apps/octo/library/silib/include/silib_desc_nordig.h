/* $Header: $ */

#ifndef __SILIB_DESC_NORDIG_H__
#define __SILIB_DESC_NORDIG_H__

#ifndef _____INCLUDED_ONLY_IN_SILIB_H____
#error The file [silib_desc_nordig.h] shall not be included except silib.h
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_OP_TDC
/** @brief eSIxDESCTAG_TDC_CHANNEL(0x82) */
typedef struct tagSIxTdcChannelInfo
{
	HUINT16							usServiceId;

	/*
		TDC Rules of Operation:

		This field indicates the logical channel number for the service. It shall
		be used as the channel number of the service in the CPE¡¯s main
		channel list (for visible services).
		The range of channel numbers is 1 to 65535
	*/
	HUINT16							usChannelNumber;

	struct tagSIxTdcChannelInfo		*pstNext;
} SIxTdcChannelInfo_t;

typedef struct
{
	SIxDescriptorHeader_t			stHead;
	SIxTdcChannelInfo_t				*pstInfo;
} SIxTdcChannelDes_t;
#endif

/** @brief eSIxDESCTAG_NORDIG_LOGICAL_CHANNEL(0x83) */
typedef struct tagSIxNordigLogicalChannelInfo
{
	HUINT16							usServiceId;
	HUINT8							ucVisibleServiceFlag;
	HUINT16							usLogicalChannelNumber;

	struct tagSIxNordigLogicalChannelInfo	*pstNext;
} SIxNordigLogicalChannelInfo_t;

typedef struct
{
	SIxDescriptorHeader_t				stHead;
	SIxNordigLogicalChannelInfo_t		*pstInfo;
} SIxNordigLogicalChannelDes_t;

/** @brief eSIxDESCTAG_NORDIG_LOGICAL_CHANNEL_V2(0x87) */
typedef struct tagSIxNordigLogicalChannelV2SvcInfo
{
	HUINT16							usServiceId;
	HUINT8							ucVisibleServiceFlag;
	HUINT16							usLogicalChannelNumber;

	struct tagSIxNordigLogicalChannelV2SvcInfo	*pstNext;
} SIxNordigLogicalChannelv2SvcInfo_t;

typedef struct tagSIxNordigLogicalChannelV2Info
{
	HUINT8								 ucChannelListId;
	HUINT8								*pszChannelListName;
	HUINT8								 szCountryCode[4];			// ISO 3166
	SIxNordigLogicalChannelv2SvcInfo_t	*pstInfo;

	struct tagSIxNordigLogicalChannelV2Info	*pstNext;
} SIxNordigLogicalChannelv2Info_t;

typedef struct
{
	SIxDescriptorHeader_t				stHead;
	SIxNordigLogicalChannelv2Info_t		*pstInfo;
} SIxNordigLogicalChannelv2Des_t;

/* DTCP Descriptor (0x88) */
/* eSIxDESCTAG_NORDIG_DTCP  (0x88) */
typedef struct tagSIxNordigDtcpDataInfo
{
	HUINT8					ucRetentionMoveMode;
	HUINT8					ucRetentionState;
	HUINT8					ucEPN;
	HUINT8					ucDTCPCCI;
	HUINT8					ucDOT;
	HUINT8					ucAST;
	HUINT8					ucImageConstraintToken;
	HUINT8					ucAPS;

	struct tagSIxNordigDtcpDataInfo	*pstNext;
} SIxNordigDtcpDataInfo_t;

typedef struct
{
	SIxDescriptorHeader_t		 stHead;
	HUINT16						 usCaSystemId;		// 0x0fff

	SIxNordigDtcpDataInfo_t	*pstInfo;
} SIxNordigDtcpDes_t;

/* Content protection Descriptor (0x88) */
/* eSIxDESCTAG_NORDIG_CONTENT_PROTECTION  (0xa0) */
typedef struct
{
	SIxDescriptorHeader_t		 stHead;
	HUINT8						 ucContentProtectionLevel;
} SIxNordigContentProtectionDes_t;

#ifdef __cplusplus
}
#endif

#endif // __SILIB_DESC_NORDIG_H__


