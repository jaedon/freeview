/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  layer_module.h File Name을 적습니다.
	@brief	  file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__HBBTV_DOWNLOAD_H__
#define	__HBBTV_DOWNLOAD_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#if 0
#include "svc_download.h"
#include "download_base.h"
#endif

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#if 0
#define DSMCC_SECTION_HEADER_LEN			(8)
#define DDB_HEADER_LEN						(12)
#define SECTION_MESSAGEID_POS				(10)
#define MAX_FILTERING_NUM					(3)
#define INVALID_INFO_INDEX					(99)
#define MAX_CACHE_NUM						(12)
#define ONE_BYTE_FILTER						0x000000FF
#define BASE_BIT_POSITION					(24)
#define ONE_BYTE_POSITION					(8)
#endif

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
#if 0
typedef enum{
	DSMCC_TABLE_LLCNAP = 0x3A,
	DSMCC_TABLE_USER_NETWORKMESSAGE,
	DSMCC_TABLE_DATA_MESSAGE,
	DSMCC_TABLE_DESCRIPTOR_LIST,
	DSMCC_TABLE_PRIVATEDATA,
}DSMCC_TABLE;

typedef enum{												/* 13818-6, clause 8, table 8-1 */
	DSMCC_STREAMDESCRIPTOR_CAROUSEL_IDENDTIFIER = 19,		/* 13818-6, clause 11. */
	DSMCC_STREAMDESCRIPTOR_ASSOCIATION_TAG,					/* 13818-6, clause 11. */
	DSMCC_STREAMDESCRIPTOR_DEFERRED_ASSOCIATION_TAGS, 		/* 13818-6, clause 11. */
	DSMCC_STREAMDESCRIPTOR_13818_6_reserved,
	DSMCC_STREAMDESCRIPTOR_NPT_REFERENCE,					/* 13818-6, clause 8.1.1, table 8-1 */
	DSMCC_STREAMDESCRIPTOR_NPT_ENDPOINT,
	DSMCC_STREAMDESCRIPTOR_STREAM_MODE,
	DSMCC_STREAMDESCRIPTOR_STREAM_EVENT
}DSMCC_STREAMDESCRIPTOR;

typedef enum{
	DSMCC_STREAMMODE_OPEN = 0x00,
	DSMCC_STREAMMODE_PAUSE,
	DSMCC_STREAMMODE_TRANSPORT,
	DSMCC_STREAMMODE_TRANSPORT_PAUSE,
	DSMCC_STREAMMODE_SEARCH_TRANSPORT,
	DSMCC_STREAMMODE_SEARCH_TRANSPORT_PAUSE,
	DSMCC_STREAMMODE_PAUSE_SEARCH_TRANSPORT,
	DSMCC_STREAMMODE_END_OF_STREAM,
	DSMCC_STREAMMODE_PRESEARCH_TRANSPORT,
	DSMCC_STREAMMODE_PRESEARCH_TRANSPORT_PAUSE,
}DSMCC_STREAMMODE;

typedef enum
{
	eDSI_MODE_RECEIVING,
	eDSI_MODE_MONITORING,
	eDSI_MODE_CHANGED
} XsvcDownload_DsmccDsiMode_e;

typedef enum
{
	DSMCC_STREAMTYPE_MULTIPROTOCOL_ENCAPSULATION = 0x0A,
	DSMCC_STREAMTYPE_UN_MESSAGES,
	DSMCC_STREAMTYPE_STREAM_DESCRIPTION,
	DSMCC_STREAMTYPE_SECTION,
}XsvcDownload_DsmccStreamType_e;

typedef struct
{
	HUINT16				usCacheIndex;
	HUINT16				usPid;
	HUINT16				usDsiFilterId, usDdbFilterId, usDescFilterId;
	XsvcDownload_DsmccStreamType_e	eStreamType;
	XsvcDownload_DsmccDsiMode_e		eDsiMode;
	HUINT32				ulDsiCrc32, ulDiiCrc32, ulDescCrc32;
	HBOOL				bDsiParsed, bDiiParsed;
	HBOOL				bDdbZeroBlockParsed;
	HBOOL				bDdbAllReceived;
} XsvcDownload_DsmccDnInfo_t;

typedef struct
{
	XsvcDownload_DsmccDnInfo_t		*pstDnInfo;
	POINTER_LIST_T		*pstEventList;
} XsvcDownload_DsmccContainer_t;

typedef struct
{
	HUINT16				usPid;
	HUINT16				usIndex;
} XsvcDownload_DsmccPidBuffer_t;
#endif

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

HERROR xsvc_download_InitAction_Hbbtv (void **ppvAction, void **ppvData, void *pvSiMsg);
HERROR xsvc_download_TermAction_Hbbtv (void **ppvAction, void **ppvData);
HERROR xsvc_download_GetData_Hbbtv (void *pvData, SvcDownload_DataType_e eDataType, HUINT32 *pulDataNum, void **ppvDataArray);
HERROR xsvc_download_FreeData_Hbbtv (SvcDownload_DataType_e eDataType, HUINT32 ulDataNum, void *pvDataArray);

#endif /* !__HBBTV_DOWNLOAD_H__ */
