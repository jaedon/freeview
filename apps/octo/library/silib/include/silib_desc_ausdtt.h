/* $Header: $ */

#ifndef __SILIB_AUSDTT_H__
#define __SILIB_AUSDTT_H__

#ifndef _____INCLUDED_ONLY_IN_SILIB_H____
#error The file [silib_desc_ausdtt.h] shall not be included except silib.h
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief eSIxDESCTAG_LOGICAL_CHANNEL(0x83) */
typedef struct tagSIxAusdttLogicalChannelInfo
{
	HUINT16							usServiceId;
	HUINT8							ucVisibleServiceFlag;
	HUINT16							usLogicalChannelNumber;
	struct tagSIxAusdttLogicalChannelInfo	*pstNext;
} SIxAusdttLogicalChannelInfo_t;

typedef struct
{
	SIxDescriptorHeader_t					stHead;
	SIxAusdttLogicalChannelInfo_t			*pstInfo;
} SIxAusdttLogicalChannelDes_t;

#ifdef __cplusplus
}
#endif

#endif // __SILIB_AUSDTT_H__


