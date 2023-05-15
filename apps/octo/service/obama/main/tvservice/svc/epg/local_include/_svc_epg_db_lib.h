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

#ifndef	__EPG_EPGDB_H__
#define	__EPG_EPGDB_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <treeutil.h>


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/



/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


typedef struct tagServiceCheckInfo
{
	Handle_t					hSvcHandle[EPG_CHECKSVC_MAX_NUM];
	HUINT16					usSvcType[EPG_CHECKSVC_MAX_NUM];
}SvcEpg_CheckInfo_t;


typedef struct tagEpgServiceNodeInfo
{
	HUINT16					usTsId;
	HUINT16					usOnId;
	HUINT8					ucAntId;
}SvcEpg_ServiceTsInfo_t;



typedef struct tagServiceEventList
{
	HUINT32					ulEpgServiceTuneType;

	POINTER_LIST_T			*pSchExtList;
	POINTER_LIST_T			*pPfExtList;

	HUINT8					ucPfHead;
	POINTER_LIST_T			*pPfList;

	HUINT8					ucSchHead;
	POINTER_LIST_T			*pSchList;

#if	defined(CONFIG_MW_EPG_TVTV)
	HUINT32					ulTvtvId;
#endif
}SvcEpg_SvcEventList_t;

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

#if 0
HULONG				EPGDB_GetSemId(void);
HERROR				EPGDB_GetTreeMgr(HTREEMGR *phTreeMgr);
HERROR				EPGDB_SetTreeMgr(HTREEMGR hTreeMgr);
HERROR				EPGDB_FreeEvent(SvcEpg_Event_t *pEvent);
HERROR				EPGDB_AllocEvent(SvcEpg_Event_t **ppEvent);
HERROR				EPGDB_FreeExtEvent(SvcEpg_ExtEvent_t *pExtEvent);
void*				EPGDB_CopyEvent(void* pvContents);
HERROR				EPGDB_AllocExtEvent(SvcEpg_ExtEvent_t **ppExtEvent);
HERROR				EPGDB_FreeExtText(SvcEpg_ExtText_t *pExtText);
HERROR				EPGDB_AllocExtText(SvcEpg_ExtText_t **ppExtText);
HERROR				EPGDB_CopyTextList(SvcEpg_ExtText_t *pSourceList, SvcEpg_ExtText_t **ppDstText);
void*				EPGDB_CopyExtEvent(void* pvContents);
HERROR				EPGDB_FindDelieveryItem(HTREEMGR hTree, HUINT32 ulDelievery, HTREEITEM *phDeliItem);
HERROR				EPGDB_FindTsItem(SvcEpg_ServiceTsInfo_t *pEpgTsInfo, HTREEITEM hDeliItem, HTREEITEM *phTsItem);
HERROR				EPGDB_FindServiceItem(HUINT16 usSvcId, HTREEITEM hTsItem, HTREEITEM *phSvcItem);
HERROR				EPGDB_AddEventList(	Handle_t hSvcHandle,
												EPG_SpecType_e eSpec,
												HUINT32 ulOrgDeliType,
												HUINT8 ucAntId,
												svcEpg_ServiceInfo_t *pEpgServiceInfo,
												POINTER_LIST_T *pEventList,
												POINTER_LIST_T *pExtList);

HERROR				EPGDB_CollectFreeEvent(void);
HERROR				EPGDB_FreePastPfEvent(void);
HERROR				EPGDB_FreePastSchEvent(void);


HERROR				EPGDB_InitResource(void);
HERROR				EPGDB_MakeEpgTextList(HUINT8 *pucSource, SvcEpg_ExtText_t *pFormerList, SvcEpg_ExtText_t **ppEpgText);
HERROR				EPGDB_ConvertEpgTextToString(SvcEpg_ExtText_t *pEpgTextList, HUINT8 **ppucString);
HERROR				EPGDB_GetEventIdAndNameByTime(Handle_t hSvc, UNIXTIME utTime, HINT32 *nEvtId, HUINT8 *pEvtName, HUINT32 n);

#if defined(CONFIG_DEBUG)
HERROR				EPGDB_PrintEventTree(void);
HERROR				EPGDB_PrintEpgResource(void);
#endif



HERROR				EPGDB_ReleaseWholeFindList(POINTER_LIST_T *pFindList);
HERROR				EPGDB_ReleaseEventList(POINTER_LIST_T *pEventList);
HERROR				EPGDB_ReleaseExtEventList(POINTER_LIST_T *pExtEventList);
HERROR				EPGDB_CreateEpgPool(void);
HERROR				EPGDB_DestroyTreeMgr(void);
HERROR				EPGDB_GetEventPoolSize(HUINT32 *pulEventCount, HUINT32 *pulExtCount);
void					EPGDB_FreeEventListContents(void *pvContents);
void					EPGDB_FreeExtEventListContents(void *pvContents);
void					EPGDB_FreeServiceInfo(void *pvContents);

HERROR				EPGDB_InitBMSearch(HUINT8 *pucKeyword);
HERROR				EPGDB_FindStringMatchPos(HUINT8 *pucText, HUINT8 **ppucFindPos);


HERROR				EPGDB_VALIDSVC_CreateCheckServiceList(HUINT32 ulMode, DxDeliveryType_e eOrgDeliveryType, HUINT8 ucAntIndex);
HERROR				EPGDB_VALIDSVC_CheckServiceValidation(HUINT32 ulOrgDeliType, HUINT8 ucAntId, DbSvc_TripleId_t *pSvcTripleId, HBOOL *pbValid, SvcEpg_CheckInfo_t *pCheckInfo);




HERROR				EPGDB_SECTION_CreateSvcSectionManager(void);
HERROR				EPGDB_SECTION_CheckSvcSection(HUINT16 usTsUniqueId, svcEpg_ServiceInfo_t *pServiceInfo, HBOOL *pbConflict, HBOOL *pbVersionChange);
HERROR				EPGDB_SECTION_InitSvcSectionManager(HUINT16 usTsUniqueId, HUINT8 ucTableId);
HERROR				EPGDB_SECTION_CheckSectionComplete(HUINT8 ucTableId, HBOOL *pbComplete);
HERROR				EPGDB_SECTION_GetSvcSectionInfo(svcEpg_ServiceInfo_t *pSrcInfo, svcEpg_ServiceInfo_t *pDstInfo);




extern	HERROR		EPGDB_MakeAuthority(HUINT8 *pszDefAuth, POINTER_LIST_T *pEvtList);

#endif
#endif /* !__EPG_EPGDB_H__ */
