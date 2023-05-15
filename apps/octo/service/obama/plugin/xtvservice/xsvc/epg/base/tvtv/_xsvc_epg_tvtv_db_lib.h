/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  
	@brief

	Description: 		\n

	Module:	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__EPG_FSAT_H__
#define	__EPG_FSAT_H__

#include <octo_common.h>

#ifdef __cplusplus
	extern "C" {
#endif


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

	HUINT32					ulTvtvId;
}SvcEpg_SvcEventList_t;

typedef struct tagEpgEventPool
{
	HULONG					ulSemId;
	HUINT32					ulEventPoolSize;
	HUINT32					ulEventFreeCount;
	SvcEpg_Event_t				*pEventBuffer;
	SvcEpg_Event_t				*pEventHead;

	HUINT32					ulExtPoolSize;
	HUINT32					ulExtFreeCount;
	SvcEpg_ExtEvent_t			*pExtBuffer;
	SvcEpg_ExtEvent_t			*pExtHead;

	HUINT32					ulExtTextPoolSize;
	HUINT32					ulExtTextFreeCount;
	SvcEpg_ExtText_t			*pExtTextBuffer;
	SvcEpg_ExtText_t			*pExtTextHead;

	HUINT32					ulTVTVEPGCount;

}EPG_POOL_t;

HERROR		xsvc_epgdblib_CreateEpgPool(void);
HERROR		xsvc_epgdblib_GetEventPoolSize(HUINT32 *pulEventCount, HUINT32 *pulExtCount);
void			xsvc_epgdblib_FreeEventListContents(void *pContents);
void			xsvc_epgdblib_FreeExtEventListContents(void *pContents);
HERROR		xsvc_epgdblib_MakeEpgTextList(HUINT8 *pucSource, SvcEpg_ExtText_t *pFormerList, SvcEpg_ExtText_t **ppEpgText);
HERROR		xsvc_epgdblib_AllocEvent(SvcEpg_Event_t **ppEvent);
HERROR		xsvc_epgdblib_GetTreeMgr(HTREEMGR *phTreeMgr);
HERROR		xsvc_epgdblib_FindDelieveryItem(HTREEMGR hTree, HUINT32 ulDelievery, HTREEITEM *phDeliItem);

#ifdef __cplusplus
}
#endif

#endif
