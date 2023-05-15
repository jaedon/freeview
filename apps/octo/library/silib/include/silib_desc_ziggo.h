/* $Header: $ */

#ifndef __SILIB_ZIGGO_H__
#define __SILIB_ZIGGO_H__

#ifndef _____INCLUDED_ONLY_IN_SILIB_H____
#error The file [silib_desc_ausdtt.h] shall not be included except silib.h
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief eSIxDESCTAG_LOGICAL_CHANNEL(0x83) */
typedef struct tagSIxZiggoLogicalChannelInfo
{
	HUINT16								usServiceId;
	HUINT8								ucVisibleServiceFlag;
	HUINT16								usLogicalChannelNumber;
	struct tagSIxZiggoLogicalChannelInfo	*pstNext;
} SIxZiggoLogicalChannelInfo_t;

typedef struct
{
	SIxDescriptorHeader_t					stHead;
	SIxZiggoLogicalChannelInfo_t			*pstInfo;
} SIxZiggoLogicalChannelDes_t;


/** @breaf eSIxDESCTAG_ZIGGO_HD_SIMULCAST_LOGICAL_CHANNEL (0x88) */
typedef struct tagSIxZiggoHDSimulLcnInfo
{
	HUINT16								usServiceId;
	HUINT8								ucVisibleServiceFlag;
	HUINT16								usLogicalChannelNumber;
	struct tagSIxZiggoHDSimulLcnInfo	 	*pstNext;
} SIxZiggoHDSimulLcnInfo_t;

typedef struct
{
	SIxDescriptorHeader_t					stHead;
	SIxZiggoHDSimulLcnInfo_t				*pstInfo;
} SIxZiggoHDSimulcastLcnDes_t;

#ifdef __cplusplus
}
#endif

#endif // __SILIB_ZIGGO_H__


