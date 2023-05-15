/**
	@file     file_name.c File Name을 적습니다.
	@brief    file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <treeutil.h>
#include "_svc_epg_common.h"
#include "_xsvc_epg_tvtv_db_lib.h"

STATIC EPG_POOL_t			s_stEpgPool = {0, };
//STATIC HULONG				sulDBSemaphore = 0;
STATIC HTREEMGR			shEpgTreeMgr = NULL;

STATIC HERROR				xsvc_epgdblib_InitEpgPool(void)
{
	HERROR				hError = ERR_EPG_FAIL;
	HINT32				lResult = VK_ERROR;


	lResult = VK_SEM_Create((unsigned long*)&s_stEpgPool.ulSemId, "epg_pool_sem", VK_SUSPENDTYPE_PRIORITY);
	if(lResult != VK_OK)
	{
		HxLOG_Print("[local_epgdb_InitEpgPool] VK_SEM_Create fail!!\n");
		return hError;
	}

	VK_SEM_Get(s_stEpgPool.ulSemId);

	s_stEpgPool.pEventBuffer = NULL;
	s_stEpgPool.pEventHead = NULL;
	s_stEpgPool.ulEventFreeCount = 0;
	s_stEpgPool.ulEventPoolSize = 0;

	s_stEpgPool.ulTVTVEPGCount = 0;

	s_stEpgPool.pExtBuffer = NULL;
	s_stEpgPool.pExtHead = NULL;
	s_stEpgPool.ulExtFreeCount = 0;
	s_stEpgPool.ulExtPoolSize = 0;

	s_stEpgPool.pExtTextBuffer = NULL;
	s_stEpgPool.pExtTextHead = NULL;
	s_stEpgPool.ulExtTextFreeCount = 0;
	s_stEpgPool.ulExtTextPoolSize = 0;

	hError = ERR_EPG_OK;

	VK_SEM_Release(s_stEpgPool.ulSemId);

	return hError;
}


HERROR				xsvc_epgdblib_CreateEpgPool(void)
{
	HERROR				hError = ERR_EPG_FAIL;
	HUINT32				ulIndex = 0;
	SvcEpg_Event_t			*pstEventPool = NULL;
	SvcEpg_ExtEvent_t		*pstExtPool = NULL;
	SvcEpg_ExtText_t		*pstExtTextPool = NULL;

//	VK_Print("\n\n============== CreateEpgPool ==============\n\n");

	xsvc_epgdblib_InitEpgPool();

	VK_SEM_Get(s_stEpgPool.ulSemId);

//	PRINT_EPG_TIME;
	pstEventPool = (SvcEpg_Event_t*)MW_MEM_Alloc(sizeof(SvcEpg_Event_t) * DxEPG_EVENT_CELL_NUM);
	if(pstEventPool == NULL)
	{
		HxLOG_Print("[local_epgdb_CreateEpgPool] alloc fail!!\n");
		goto EXIT_EPGDB_FUNC;
	}
	for(ulIndex = 0; ulIndex < DxEPG_EVENT_CELL_NUM - 1; ulIndex++)
	{
		pstEventPool[ulIndex].pNext = &(pstEventPool[ulIndex + 1]);
	}
	pstEventPool[ulIndex].pNext = NULL;

	s_stEpgPool.pEventBuffer = pstEventPool;
	s_stEpgPool.pEventHead = pstEventPool;
	s_stEpgPool.ulEventFreeCount = DxEPG_EVENT_CELL_NUM;
	s_stEpgPool.ulEventPoolSize = DxEPG_EVENT_CELL_NUM;
	s_stEpgPool.ulTVTVEPGCount = 0;

	pstExtPool = (SvcEpg_ExtEvent_t*)MW_MEM_Alloc(sizeof(SvcEpg_ExtEvent_t) * DxEPG_EXT_EVENT_CELL_NUM);
	if(pstExtPool == NULL)
	{
		HxLOG_Print("[local_epgdb_CreateEpgPool] alloc fail!!\n");
		goto EXIT_EPGDB_FUNC;
	}
	for(ulIndex = 0; ulIndex < DxEPG_EXT_EVENT_CELL_NUM - 1; ulIndex++)
	{
		pstExtPool[ulIndex].pNext = &(pstExtPool[ulIndex + 1]);
	}
	pstExtPool[ulIndex].pNext = NULL;

	s_stEpgPool.pExtBuffer = pstExtPool;
	s_stEpgPool.pExtHead = pstExtPool;
	s_stEpgPool.ulExtFreeCount = DxEPG_EXT_EVENT_CELL_NUM;
	s_stEpgPool.ulExtPoolSize = DxEPG_EXT_EVENT_CELL_NUM;


	pstExtTextPool = (SvcEpg_ExtText_t*)MW_MEM_Alloc(sizeof(SvcEpg_ExtText_t) * (DxEPG_EXT_TEXT_NUM));
	if(pstExtTextPool == NULL)
	{
		HxLOG_Print("[local_epgdb_CreateEpgPool] alloc fail!!\n");
		goto EXIT_EPGDB_FUNC;
	}
	for(ulIndex = 0; ulIndex < DxEPG_EXT_TEXT_NUM - 1; ulIndex++)
	{
		pstExtTextPool[ulIndex].pNext = &(pstExtTextPool[ulIndex + 1]);
	}
	pstExtTextPool[ulIndex].pNext = NULL;

	s_stEpgPool.pExtTextBuffer = pstExtTextPool;
	s_stEpgPool.pExtTextHead = pstExtTextPool;
	s_stEpgPool.ulExtTextFreeCount = DxEPG_EXT_TEXT_NUM;
	s_stEpgPool.ulExtTextPoolSize = DxEPG_EXT_TEXT_NUM;

	hError = ERR_EPG_OK;


EXIT_EPGDB_FUNC :
	VK_SEM_Release(s_stEpgPool.ulSemId);

	return hError;
}

HERROR				xsvc_epgdblib_GetEventPoolSize(HUINT32 *pulEventCount, HUINT32 *pulExtCount)
{
	HERROR				hError = ERR_EPG_FAIL;

	VK_SEM_Get(s_stEpgPool.ulSemId);

	*pulEventCount = s_stEpgPool.ulEventPoolSize;
	*pulExtCount = s_stEpgPool.ulExtPoolSize;

	hError = ERR_EPG_OK;

	VK_SEM_Release(s_stEpgPool.ulSemId);

	return hError;
}

HERROR				xsvc_epgdblib_AllocEvent(SvcEpg_Event_t **ppEvent)
{
	HERROR				hError = ERR_EPG_FAIL;
	SvcEpg_Event_t			*pTempEvent = NULL;

	VK_SEM_Get(s_stEpgPool.ulSemId);
	if(ppEvent == NULL)
	{
		HxLOG_Print("[local_epgdb_AllocEvent] param fail!!\n");
		goto EXIT_EPGDB_FUNC;
	}
	if(s_stEpgPool.ulEventFreeCount == 0)
	{
		HxLOG_Print("[local_epgdb_AllocExtEvent] event count zero!!\n");
		goto EXIT_EPGDB_FUNC;
	}
	*ppEvent = NULL;

	pTempEvent = s_stEpgPool.pEventHead;
	s_stEpgPool.pEventHead = pTempEvent->pNext;
	s_stEpgPool.ulEventFreeCount--;

	pTempEvent->pNext = NULL;

	VK_memset32(pTempEvent, 0x00, sizeof(SvcEpg_Event_t));

	*ppEvent = pTempEvent;


	hError = ERR_EPG_OK;

EXIT_EPGDB_FUNC :
	VK_SEM_Release(s_stEpgPool.ulSemId);

	return hError;
}

STATIC HERROR				xsvc_epgdblib_FreeExtEvent(SvcEpg_ExtEvent_t *pExtEvent)
{
	HERROR				hError = ERR_EPG_FAIL;
	SvcEpg_ExtText_t		*pTempExtText = NULL;
	SvcEpg_ExtText_t		*pFreeExtText = NULL;
	HUINT8				ucIndex = 0;
	HINT32				lExtInfoIndex = 0;

	VK_SEM_Get(s_stEpgPool.ulSemId);
	if(pExtEvent == NULL)
	{
		HxLOG_Print("[local_epgdb_FreeExtEvent] param fail!!\n");
		goto EXIT_EPGDB_FUNC;
	}


	for(lExtInfoIndex = 0; lExtInfoIndex < DxEPG_EXT_INFO_MAX; lExtInfoIndex++)
	{
		if(	pExtEvent->astExtInfo[lExtInfoIndex].pstExtText == NULL
			&& pExtEvent->astExtInfo[lExtInfoIndex].astItem[0].pItemChar == NULL
			&& pExtEvent->astExtInfo[lExtInfoIndex].astItem[0].pItemDescription == NULL)
		{
			break;
		}

		pFreeExtText = pTempExtText = NULL;
		pTempExtText = pExtEvent->astExtInfo[lExtInfoIndex].pstExtText;
		while(pTempExtText)
		{
			pFreeExtText = pTempExtText;
			pTempExtText = pTempExtText->pNext;

			pFreeExtText->pNext = s_stEpgPool.pExtTextHead;
			s_stEpgPool.pExtTextHead = pFreeExtText;
			s_stEpgPool.ulExtTextFreeCount++;
//			VK_Print("[local_epgdb_FreeExtEvent] (pstExtText)s_stEpgPool.ulExtTextFreeCount+++++++++++++\n");
		}

		for(ucIndex = 0; ucIndex < DxEPG_EXT_ITEM_MAX; ucIndex++)
		{
			if(	pExtEvent->astExtInfo[lExtInfoIndex].astItem[ucIndex].pItemChar == NULL
				&& pExtEvent->astExtInfo[lExtInfoIndex].astItem[ucIndex].pItemDescription == NULL)
			{
				break;
			}
			pFreeExtText = pTempExtText = NULL;
			pTempExtText = pExtEvent->astExtInfo[lExtInfoIndex].astItem[ucIndex].pItemChar;
			while(pTempExtText)
			{
				pFreeExtText = pTempExtText;
				pTempExtText = pTempExtText->pNext;

				pFreeExtText->pNext = s_stEpgPool.pExtTextHead;
				s_stEpgPool.pExtTextHead = pFreeExtText;
				s_stEpgPool.ulExtTextFreeCount++;
				//VK_Print("[local_epgdb_FreeExtEvent] (astItem)s_stEpgPool.ulExtTextFreeCount+++++++++++++\n");
			}

			pFreeExtText = pTempExtText = NULL;
			pTempExtText = pExtEvent->astExtInfo[lExtInfoIndex].astItem[ucIndex].pItemDescription;
			while(pTempExtText)
			{
				pFreeExtText = pTempExtText;
				pTempExtText = pTempExtText->pNext;

				pFreeExtText->pNext = s_stEpgPool.pExtTextHead;
				s_stEpgPool.pExtTextHead = pFreeExtText;
				s_stEpgPool.ulExtTextFreeCount++;
			}
		}
	}

	pExtEvent->pNext = s_stEpgPool.pExtHead;
	s_stEpgPool.pExtHead = pExtEvent;
	s_stEpgPool.ulExtFreeCount++;
	//VK_Print("[local_epgdb_FreeExtEvent] s_stEpgPool.ulExtFreeCount++++++++++++\n");

	hError = ERR_EPG_OK;

EXIT_EPGDB_FUNC :
	VK_SEM_Release(s_stEpgPool.ulSemId);

	return hError;
}

STATIC HERROR				xsvc_epgdblib_FreeEvent(SvcEpg_Event_t *pEvent)
{
	HERROR				hError = ERR_EPG_FAIL;
	SvcEpg_ExtText_t		*pTempExtText = NULL;
	SvcEpg_ExtText_t		*pFreeExtText = NULL;
	HINT32				lIndex = 0;

	VK_SEM_Get(s_stEpgPool.ulSemId);
	if(pEvent == NULL)
	{
		HxLOG_Print("[local_epgdb_FreeEvent] param fail!!\n");
		goto EXIT_EPGDB_FUNC;
	}

	for(lIndex = 0; lIndex < DxEPG_SHORT_TEXT_MAX; lIndex++)
	{
		pTempExtText = NULL;
		pTempExtText = pEvent->astShortText[lIndex].pEventName;
		while(pTempExtText)
		{
			pFreeExtText = pTempExtText;
			pTempExtText = pTempExtText->pNext;

			pFreeExtText->pNext = s_stEpgPool.pExtTextHead;
			s_stEpgPool.pExtTextHead = pFreeExtText;
			s_stEpgPool.ulExtTextFreeCount++;
		}

		pTempExtText = NULL;
		pTempExtText = pEvent->astShortText[lIndex].pEventText;
		while(pTempExtText)
		{
			pFreeExtText = pTempExtText;
			pTempExtText = pTempExtText->pNext;

			pFreeExtText->pNext = s_stEpgPool.pExtTextHead;
			s_stEpgPool.pExtTextHead = pFreeExtText;
			s_stEpgPool.ulExtTextFreeCount++;
		}
	}

	pEvent->pNext =  s_stEpgPool.pEventHead;
	 s_stEpgPool.pEventHead = pEvent;
	s_stEpgPool.ulEventFreeCount++;

	if(pEvent->stEventInfo.ucEpgDataType == EPG_DATA_TYPE_TVTV)
	{
		s_stEpgPool.ulTVTVEPGCount--;
	}

	hError = ERR_EPG_OK;

EXIT_EPGDB_FUNC :
	VK_SEM_Release(s_stEpgPool.ulSemId);

	return hError;
}

void			xsvc_epgdblib_FreeExtEventListContents(void *pContents)
{
	HERROR				hError = ERR_EPG_FAIL;
	SvcEpg_ExtEvent_t		*pExtEvent = NULL;

	if(pContents == NULL)
	{
		HxLOG_Print("[local_epgdb_FreeExtEventListContents] param fail!!\n");
		goto EXIT_EPGDB_FUNC;
	}
	pExtEvent = (SvcEpg_ExtEvent_t*)pContents;

	hError = xsvc_epgdblib_FreeExtEvent(pExtEvent);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("[local_epgdb_FreeExtEventListContents] local_epgdb_FreeExtEvent fail!!\n");
		goto EXIT_EPGDB_FUNC;
	}

EXIT_EPGDB_FUNC :

	return;
}

void			xsvc_epgdblib_FreeEventListContents(void *pContents)
{
	HERROR				hError = ERR_EPG_FAIL;
	SvcEpg_Event_t			*pEvent = NULL;

	if(pContents == NULL)
	{
		HxLOG_Print("[local_epgdb_FreeEventListContents] param fail!!\n");
		goto EXIT_EPGDB_FUNC;
	}
	pEvent = (SvcEpg_Event_t*)pContents;

	hError = xsvc_epgdblib_FreeEvent(pEvent);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("[local_epgdb_FreeEventListContents] local_epgdb_FreeEvent fail!!\n");
		goto EXIT_EPGDB_FUNC;
	}

EXIT_EPGDB_FUNC :

	return;
}

STATIC HERROR				xsvc_epgdblib_FreeExtText(SvcEpg_ExtText_t *pExtText)
{
	HERROR					hError = ERR_EPG_FAIL;
	SvcEpg_ExtText_t			*pTempText = NULL;
	SvcEpg_ExtText_t			*pPrevText = NULL;


	VK_SEM_Get(s_stEpgPool.ulSemId);
	if(pExtText == NULL)
	{
		HxLOG_Print("[local_epgdb_FreeExtText] param fail!!\n");
		goto EXIT_EPGDB_FUNC;
	}

	pTempText = pPrevText = pExtText;
	while(pTempText)
	{
		pPrevText = pTempText;
		pTempText = pTempText->pNext;

		pPrevText->pNext = s_stEpgPool.pExtTextHead;
		s_stEpgPool.pExtTextHead = pPrevText;
		s_stEpgPool.ulExtTextFreeCount++;
		//VK_Print("[local_epgdb_FreeExtText] s_stEpgPool.ulExtTextFreeCount++++++++++++++\n");
 	}

	hError = ERR_EPG_OK;

EXIT_EPGDB_FUNC :
	VK_SEM_Release(s_stEpgPool.ulSemId);

	return hError;
}

STATIC HERROR				xsvc_epgdblib_AllocParseExtText(SvcEpg_ExtText_t **ppExtText)
{
	HERROR				hError = ERR_EPG_FAIL;
	SvcEpg_ExtText_t		*pTempExtText = NULL;

	VK_SEM_Get(s_stEpgPool.ulSemId);
	if(ppExtText == NULL)
	{
		HxLOG_Print("[local_epgdb_AllocParseExtText] param fail!!\n");
		goto EXIT_EPGDB_FUNC;
	}
	if(s_stEpgPool.ulExtTextFreeCount < 400)
	{
		HxLOG_Print("[local_epgdb_AllocParseExtText] event count zero!!\n");
		goto EXIT_EPGDB_FUNC;
	}
	*ppExtText = NULL;

	pTempExtText = s_stEpgPool.pExtTextHead;
	s_stEpgPool.pExtTextHead = pTempExtText->pNext;
	s_stEpgPool.ulExtTextFreeCount--;
	//VK_Print("[local_epgdb_AllocExtText] s_stEpgPool.ulExtFreeCount-------------\n");
	pTempExtText->pNext = NULL;

	VK_memset32(pTempExtText, 0x00, sizeof(SvcEpg_ExtText_t));

	*ppExtText = pTempExtText;

	hError = ERR_EPG_OK;

EXIT_EPGDB_FUNC :
	VK_SEM_Release(s_stEpgPool.ulSemId);

	return hError;
}

HERROR				xsvc_epgdblib_MakeEpgTextList(HUINT8 *pucSource, SvcEpg_ExtText_t *pFormerList, SvcEpg_ExtText_t **ppEpgText)
{
	HERROR					hError = ERR_EPG_FAIL;
	HBOOL					bExtTextComplete = FALSE;
	HUINT16					usTotalLength = 0;
	HUINT16					usLength = 0;
	HUINT16					usCopyLen = 0;
	HUINT16					usExtTextLoop = 0;
	HUINT8					ucIndex = 0;
	SvcEpg_ExtText_t			*pTempText = NULL;
	SvcEpg_ExtText_t			*pRootText = NULL;
	SvcEpg_ExtText_t			*pNewText = NULL;


	if(pucSource == NULL || ppEpgText == NULL)
	{
		HxLOG_Print("[local_epgdb_MakeEpgTextList] param fail!!\n");
		goto EXIT_EPGDB_FUNC;
	}
	*ppEpgText = NULL;

	pRootText = pFormerList;
	bExtTextComplete = TRUE;
	usLength = usTotalLength = (HUINT16)HxSTD_StrLen(pucSource);
	usExtTextLoop = (usTotalLength / DxEPG_EXT_TEXT_LEN);
	if( (usLength - (DxEPG_EXT_TEXT_LEN*usExtTextLoop)) > 0)
	{
		usExtTextLoop += 1;
	}
	bExtTextComplete = FALSE;
	for(ucIndex = 0; ucIndex < usExtTextLoop; ucIndex++)
	{

		pTempText = pNewText = NULL;
		hError = xsvc_epgdblib_AllocParseExtText(&pNewText);
		if(hError != ERR_EPG_OK)
		{
			HxLOG_Print("[local_epgdb_MakeEpgTextList] local_epgdb_AllocParseExtText fail!!\n");
			break;
		}
		usCopyLen = 0;
		usCopyLen = (HUINT16)(DxEPG_EXT_TEXT_LEN > usLength ? usLength : DxEPG_EXT_TEXT_LEN);
		HxSTD_memcpy(	pNewText->aucTextChar,
					&(pucSource[DxEPG_EXT_TEXT_LEN*ucIndex]),
					usCopyLen);
		usLength -= usCopyLen;

		pTempText = pRootText;
		if(pTempText)
		{
			while(pTempText->pNext)
			{
				pTempText = pTempText->pNext;
			}
			pTempText->pNext = pNewText;
		}
		else
		{
			pRootText = pNewText;
		}
	}

	pTempText = NULL;
	if(usExtTextLoop == ucIndex)
	{
		bExtTextComplete = TRUE;
	}
	else
	{
		hError = xsvc_epgdblib_FreeExtText(pRootText);
		if(hError != ERR_EPG_OK)
		{
			HxLOG_Print("[local_epgdb_MakeEpgTextList] EPGDB_FreeExtText fail!!\n");
			goto EXIT_EPGDB_FUNC;
		}
		pRootText = NULL;
	}

	*ppEpgText = pRootText;

	NOT_USED_PARAM(bExtTextComplete);
	hError = ERR_EPG_OK;

EXIT_EPGDB_FUNC :

	return hError;

}
HERROR				xsvc_epgdblib_GetTreeMgr(HTREEMGR *phTreeMgr)
{
	HERROR			hError = ERR_EPG_FAIL;

	if(phTreeMgr == NULL)
	{
		HxLOG_Print("[local_epgdb_GetTreeMgr] param fail!!\n");
		goto EXIT_EPGDB_FUNC;
	}

	*phTreeMgr = shEpgTreeMgr;

	hError = ERR_EPG_OK;

EXIT_EPGDB_FUNC :

	return hError;
}


HERROR				xsvc_epgdblib_FindDelieveryItem(HTREEMGR hTree, HUINT32 ulDelievery, HTREEITEM *phDeliItem)
{
	HERROR				hError = ERR_EPG_FAIL;
	HTREEITEM			hFirstChild = NULL;
	HTREEITEM			hTempItem = NULL;
	HUINT32				ulItemDelievery = 0;

	if(phDeliItem == NULL || hTree == NULL)
	{
		HxLOG_Print("[local_epgdb_FindDelieveryItem] param fail!!\n");
		goto EXIT_EPGDB_FUNC;
	}
	*phDeliItem = NULL;

	hError = UTIL_TREEUTIL_GetFirstChildItem(hTree, &hFirstChild);
	if(hError != ERR_TREEUTIL_OK)
	{
		HxLOG_Print("[local_epgdb_FindDelieveryItem] GetFirstChildItem fail!!\n");
		goto EXIT_EPGDB_FUNC;
	}

	hTempItem = hFirstChild;
	while(hTempItem)
	{
		hError = UTIL_TREEUTIL_GetItemUserInfo(hTempItem, sizeof(HUINT32), (void*)&ulItemDelievery);
		if(hError != ERR_OK)
		{
			HxLOG_Print("[local_epgdb_FindDelieveryItem] GetItemUserInfo fail!!\n");
			hError = UTIL_TREEUTIL_GetNextItem(hTempItem, &hTempItem);
			if(hError != ERR_OK)
			{
				HxLOG_Print("[local_epgdb_FindDelieveryItem] GetNextItem fail!!\n");
				goto EXIT_EPGDB_FUNC;
			}
			continue;
		}
		if(ulItemDelievery == ulDelievery)
		{
			*phDeliItem = hTempItem;
			break;
		}

		hError = UTIL_TREEUTIL_GetNextItem(hTempItem, &hTempItem);
		if(hError != ERR_OK)
		{
			HxLOG_Print("[local_epgdb_FindDelieveryItem] GetNextItem fail!!\n");
			goto EXIT_EPGDB_FUNC;
		}
	}

	hError = ERR_EPG_OK;

EXIT_EPGDB_FUNC :

	return hError;
}


