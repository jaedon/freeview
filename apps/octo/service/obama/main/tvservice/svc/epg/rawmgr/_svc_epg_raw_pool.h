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

#ifndef	__EPG_RAWMGR_POOL_H__
#define	__EPG_RAWMGR_POOL_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/



/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#define	SvcEpg_MAX_SI_RAWSEC_NUM								4000
#define SvcEpg_INVALID_INDEX_HANDLE								0xffffffff
#define SvcEpg_MAX_INDEX_HANDLE_NUM								128

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

svcEpg_RawPool_t *svc_epg_RawPoolAlloc(HUINT16 usMaxRawSec, HUINT8 *szPoolName);
HUINT32 svc_epg_RawPoolFreeSection(svcEpg_RawPool_t *pstPool, HUINT16 usUniqueId, SiTableType_e eSiTableType, HBOOL bUseExtId, HUINT16 usExtId);
void svc_epg_RawPoolFreeAllSection(svcEpg_RawPool_t *pstPool, HUINT16 usUniqueId, SiTableType_e eSiTableType);
void svc_epg_RawPoolClear(svcEpg_RawPool_t *pstPool);
HUINT32 svc_epg_RawPoolAddSection(svcEpg_RawPool_t *pstPool, HUINT16 usUniqueId, SiTableType_e eSiTableType, HUINT8 *pucRawSec);

#endif /* !__EPG_RAWMGR_POOL_H__ */
