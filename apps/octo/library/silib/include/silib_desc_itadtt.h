/* $Header: $ */

#ifndef __SILIB_ITADTT_H__
#define __SILIB_ITADTT_H__

#ifndef _____INCLUDED_ONLY_IN_SILIB_H____
#error The file [silib_desc_itadtt.h] shall not be included except silib.h
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief eSIxDESCTAG_LOGICAL_CHANNEL(0x83) */
typedef struct tagSIxItadttLogicalChannelInfo
{
	HUINT16							usServiceId;
	HUINT8							ucVisibleServiceFlag;
	HUINT16							usLogicalChannelNumber;
	struct tagSIxItadttLogicalChannelInfo	*pstNext;
} SIxItadttLogicalChannelInfo_t;

typedef struct
{
	SIxDescriptorHeader_t					stHead;
	SIxItadttLogicalChannelInfo_t			*pstInfo;
} SIxItadttLogicalChannelDes_t;

/** @breaf eSIxDESCTAG_ITADTT_HD_SIMULCAST_LOGICAL_CHANNEL (0x88) */
typedef struct tagSIxItadttHDSimulLcnInfo
{
	HUINT16								usServiceId;
	HUINT8								ucVisibleServiceFlag;
	HUINT16								usLogicalChannelNumber;
	struct tagSIxItadttHDSimulLcnInfo	*pstNext;
} SIxItadttHDSimulLcnInfo_t;

typedef struct
{
	SIxDescriptorHeader_t			stHead;
	SIxItadttHDSimulLcnInfo_t		*pstInfo;
} SIxItadttHDSimulcastLcnDes_t;

#ifdef __cplusplus
}
#endif

#endif // __SILIB_ITADTT_H__


