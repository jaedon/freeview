/* $Header: $ */

#ifndef __SILIB_DESC_MBC_H__
#define __SILIB_DESC_MBC_H__

#ifndef _____INCLUDED_ONLY_IN_SILIB_H____
#error The file [silib_desc_mbc.h] shall not be included except silib.h
#endif

#ifdef __cplusplus
extern "C" {
#endif


/** @brief eSIxDESCTAG_MBC_LOGICAL_CHANNEL(0x83) */
typedef struct tagSIxMbcLogicalChannelInfo
{
	HUINT16								usServiceId;
	HUINT8								ucVisibleServiceFlag;
	HUINT16								usLogicalChannelNumber;

	struct tagSIxMbcLogicalChannelInfo	*pstNext;
} SIxMbcLogicalChannelInfo_t;

typedef struct
{
	SIxDescriptorHeader_t				stHead;
	SIxMbcLogicalChannelInfo_t			*pstInfo;
} SIxMbcLogicalChannelDes_t;

#ifdef __cplusplus
}
#endif

#endif // __SILIB_DESC_MBC_H__


