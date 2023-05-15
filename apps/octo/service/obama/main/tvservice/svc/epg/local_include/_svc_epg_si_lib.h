/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  layer_module.h File Name을 적습니다.
	@brief	  file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__SVC_EPG_SI_LIB_H__
#define	__SVC_EPG_SI_LIB_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <silib.h>
#include <octo_common.h>
#include "_svc_epg_raw_mgr.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#define SvcEpg_NUM_SEC_FOR_RECEIVING_TABLE				16
#define SvcEpg_RAW_POOL_NAME_LEN						8
#define SvcEpg_INVALID_INDEX_HANDLE						0xffffffff

#define	SvcEpg_TAG_ALL_DESCRIPTORS						0xFF


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

HERROR svc_epg_SiSetDefaultRetrieveParam(SiRetrieveParam_t *pstRetrieveParam,
										HUINT32 hAction, HUINT16 usTsUniqId, HUINT32 ulDemuxId);
HERROR svc_epg_SiStopOneSection(HUINT16 usFilterId);
HERROR svc_epg_SiRetrievePatSection(SiRetrieveParam_t *pstTable,
										void *pfnParse,
										void *pfnFree,
										HUINT16 *pusFilterId);
HERROR svc_epg_SiRetrievePmtSection (SiRetrieveParam_t *pstTable,
										void *pfnParse,
										void *pfnFree,
										HUINT16 *pusFilterId);
HERROR svc_epg_SiRetrieveDsiSection(SiRetrieveParam_t *pstTable,
										void *pfnParse,
										void *pfnFree,
										HUINT16 *pusFilterId);
HERROR svc_epg_SiRetrieveDdbSection(SiRetrieveParam_t *pstTable,
										void *pfnParse,
										void *pfnFree,
										HUINT16 *pusFilterId);
HERROR svc_epg_SiCountPmtSection(svcEpg_RawPool_t *pstPool, HUINT16 usTsUniqId,
									HUINT32 *pulCount);
HERROR svc_epg_SiGetPmtPidByServiceId(svcEpg_RawPool_t *pstPool, HUINT16 usTsUniqId, HUINT16 usSvcId,
									HUINT16 *pusPmtPid);
HERROR svc_epg_SiGetPmtProgramInfo(svcEpg_RawPool_t *pstPool, HUINT16 usTsUniqId, HUINT32 ulIdx,
									PmtProgramInfo_t *pstInfo);
HERROR svc_epg_SiCountPmtElement(svcEpg_RawPool_t *pstPool, HUINT16 usTsUniqId, HUINT32 ulIdx,
									HUINT32 *pulCount);
HERROR svc_epg_SiGetPmtElementInfo(svcEpg_RawPool_t *pstPool, HUINT16 usTsUniqId, HUINT32 ulIdx, HUINT32 ulSubIdx,
									PmtElementInfo_t *pstInfo);
HERROR svc_epg_SiCountPmtElementDescriptor(svcEpg_RawPool_t *pstPool, HUINT16 usTsUniqId, HUINT32 ulIdx, HUINT32 ulSubIdx,
											SIxDescriptorTag_e eTag, HUINT32 *pulCount);
HERROR svc_epg_SiGetPmtElementDescriptor(svcEpg_RawPool_t *pstPool, HUINT16 usTsUniqId, HUINT32 ulIdx, HUINT32 ulSubIdx,
											SIxDescriptorTag_e eTag, HUINT32 ulDesIdx, SIxDescriptor_t **ppstDes);

#endif /* !__SVC_EPG_SI_LIB_H__ */
