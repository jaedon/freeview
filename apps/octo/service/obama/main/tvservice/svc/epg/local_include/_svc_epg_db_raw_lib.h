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

#ifndef	__EPG_EPGRAWDB_H__
#define	__EPG_EPGRAWDB_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <treeutil.h>
#include "isosvc.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define EPG_CHECKSVC_RAW_MAX_NUM							10
#define EPG_CONVERT_STRING_BUFSIZE		(1024*4)
#define EPG_FS_RAWEIT_SAVE_HDD_PATH					"/mnt/hd1/dvbepg/"

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef struct tagEpgCheckServiceListRaw
{
	HULONG				ulSemId;
	HxTREE_t 			*pstServices;
}SvcEpg_CheckSvcListRaw_t;

typedef struct tagServiceCheckInfoRaw
{
	Handle_t					hSvcHandle[EPG_CHECKSVC_RAW_MAX_NUM];
	HUINT16					usSvcType[EPG_CHECKSVC_RAW_MAX_NUM];
}SvcEpg_SvcCheckInfoRaw_t;

typedef struct tagServiceCheckInfo
{
	DxDeliveryType_e		deliType;
	HINT32				antuid;
	HINT32				tsuid;
	HINT32				onid;
	HINT32				tsid;
	HINT32				svcid;
	HINT32				svcuid;
}SvcEpg_SvcCheckInfo_t;

typedef struct tagEpgServiceNodeInfoRaw
{
	HUINT16					usTsId;
	HUINT16					usOnId;
	HUINT8					ucAntId;
}SvcEpg_ServiceTsInfoRaw_t;


typedef struct tagServiceEventListRaw
{
	HUINT32					ulEpgServiceTuneType;

	POINTER_LIST_T			*pSchExtList;
	POINTER_LIST_T			*pPfExtList;

	HUINT8					ucPfExtHead;
	HUINT8					ucSchExtHead;

	HUINT8					ucPfHead;
	POINTER_LIST_T			*pPfList;

	HUINT8					ucSchHead;
	POINTER_LIST_T			*pSchList;

	POINTER_LIST_T			*pSchAribExtSch;
}SvcEpg_SvcEventListRaw_t;

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
/// 사용됨..
// svc_epg_db_raw_lib.c
HERROR	SVC_EPGLIB_AddSectionList(	Handle_t hSvcHandle,
													EPG_SpecType_e eSpec,
													HUINT32 ulOrgDeliType,
													HUINT8 ucAntId,
													HUINT16	usTblId,
													POINTER_LIST_T *pSectionList);
void		SVC_EPGLIB_FreeSectionListContents(void *pvContents);
HERROR	SVC_EPGLIB_FindDelieveryItem(HTREEMGR hTree, HUINT32 ulDelievery, HTREEITEM *phDeliItem);
HERROR	SVC_EPGLIB_FindTsItem(SvcEpg_ServiceTsInfoRaw_t *pEpgTsInfo, HTREEITEM hDeliItem, HTREEITEM *phTsItem);
HERROR	SVC_EPGLIB_FindServiceItem(HUINT16 usSvcId, HTREEITEM hTsItem, HTREEITEM *phSvcItem);

// svc_epg_db_raw_service.c
HERROR	SVC_EPGSERVICE_ValidSvcCreateCheckServiceList(void);
HERROR	SVC_EPGSERVICE_ValidSvcCheckServiceValidation(HUINT32 ulOrgDeliType, HUINT8 ucAntId, DbSvc_TripleId_t *pSvcTripleId);

// svc_epg_db_raw_serction.c
HERROR	SVC_EPGSECTION_CreateSvcSectionManager(void);
HERROR	SVC_EPGSECTION_CheckSvcSection(Handle_t hAction, HUINT16 usTsUniqueId, svcEpg_ServiceInfo_t *pServiceInfo, HBOOL *pbConflict, HBOOL *pbVersionChange, HBOOL *pbIsFirstTime);
HERROR	SVC_EPGSECTION_InitSvcSectionManager(Handle_t hAction, HUINT16 usTsUniqueId, HUINT8 ucTableId);
HERROR	SVC_EPGSECTION_CheckSectionComplete(Handle_t hAction, HUINT8 ucTableId, HBOOL *pbComplete);
HERROR	SVC_EPGSECTION_GetSvcSectionInfo(Handle_t hAction, svcEpg_ServiceInfo_t *pSrcInfo, svcEpg_ServiceInfo_t *pDstInfo);

#endif /* !__EPG_EPGRAWDB_H__ */
