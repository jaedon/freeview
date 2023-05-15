/* $Header: $ */

#ifndef __SILIB_SES_H__
#define __SILIB_SES_H__

#ifndef _____INCLUDED_ONLY_IN_SILIB_H____
#error The file [silib_desc_ses.h] shall not be included except silib.h
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief eSIxDESCTAG_SES_SERVICE_LIST_NAME(0x88) */
typedef struct
{
	SIxDescriptorHeader_t		stHead;
	HUINT8						szIso639LangCode[4];
	HUINT8						*pszSvcListName;
} SIxServiceListNameDes_t;

/** @brief eSIxDESCTAG_SES_BOUQUET_LIST(0x93) */
typedef struct tagSIxBouquetListInfo
{
	HUINT8							ucBouquetNameLength;
	HUINT8							*pszBouquetName;
	struct tagSIxBouquetListInfo	*pstNext;
} SIxBouquetListInfo_t;

typedef struct
{
	SIxDescriptorHeader_t		stHead;
	SIxBouquetListInfo_t		*pstInfo;
} SIxBouquetListDes_t;

/** @brief eSIxDESCTAG_SES_VIRTUAL_SERVICE_ID(0xd1) */
typedef struct
{
	SIxDescriptorHeader_t		stHead;
	HUINT16						usVirtualServiceId;
} SIxVirtualServiceIdDes_t;

/** @brief eSIxDESCTAG_SES_COPY_CONTROL(0xde) */
typedef struct tagSIxCopyControlInfo
{
	HUINT8							ucCopyControl;
	struct tagSIxCopyControlInfo	*pstNext;
} SIxCopyControlInfo_t;

typedef struct
{
	SIxDescriptorHeader_t		stHead;
	HUINT8						ucVersion;
	SIxCopyControlInfo_t		*pstInfo;
} SIxCopyControlDes_t;

/* DTCP Descriptor (0x88) */
/* eSIxDESCTAG_SES_DTCP  (0x88) */
typedef struct tagSIxSesDtcpDataInfo
{
	HUINT8					ucRetentionMoveMode;
	HUINT8					ucRetentionState;
	HUINT8					ucEPN;
	HUINT8					ucDTCPCCI;
	HUINT8					ucDOT;
	HUINT8					ucAST;
	HUINT8					ucImageConstraintToken;
	HUINT8					ucAPS;

	struct tagSIxSesDtcpDataInfo	*pstNext;
} SIxSesDtcpDataInfo_t;

typedef struct
{
	SIxDescriptorHeader_t		 stHead;
	HUINT16						 usCaSystemId;		// 0x0fff

	SIxSesDtcpDataInfo_t	*pstInfo;
} SIxSesDtcpDes_t;

#ifdef __cplusplus
}
#endif

#endif // __SILIB_SES_H__


