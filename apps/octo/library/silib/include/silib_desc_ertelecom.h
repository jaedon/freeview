/* $Header: $ */

#ifndef __SILIB_ERTELECOM_H__
#define __SILIB_ERTELECOM_H__

#ifndef _____INCLUDED_ONLY_IN_SILIB_H____
#error The file [silib_desc_ertelecom.h] shall not be included except silib.h
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief eSIxDESCTAG_LOGICAL_CHANNEL(0x83) */
typedef struct tagSIxErtelecomLogicalChannelInfo
{
	HUINT16							usServiceId;
	HUINT8							ucVisibleServiceFlag;
	HUINT16							usLogicalChannelNumber;

	struct tagSIxErtelecomLogicalChannelInfo	*pstNext;
} SIxErtelecomLogicalChannelInfo_t;

typedef struct
{
	SIxDescriptorHeader_t					stHead;
	SIxErtelecomLogicalChannelInfo_t		*pstInfo;
} SIxErtelecomLogicalChannelDes_t;

/* DTCP Descriptor (0x88) */
/* eSIxDESCTAG_ERTELECOM_DTCP  (0x88) */
typedef struct tagSIxErtelecomDtcpDataInfo
{
	HUINT8					ucRetentionMoveMode;
	HUINT8					ucRetentionState;
	HUINT8					ucEPN;
	HUINT8					ucDTCPCCI;
	HUINT8					ucDOT;
	HUINT8					ucAST;
	HUINT8					ucImageConstraintToken;
	HUINT8					ucAPS;

	struct tagSIxErtelecomDtcpDataInfo	*pstNext;
} SIxErtelecomDtcpDataInfo_t;

typedef struct
{
	SIxDescriptorHeader_t		 stHead;
	HUINT16						 usCaSystemId;		// 0x0fff

	SIxErtelecomDtcpDataInfo_t	*pstInfo;
} SIxErtelecomDtcpDes_t;

#ifdef __cplusplus
}
#endif

#endif // __SILIB_ERTELECOM_H__


