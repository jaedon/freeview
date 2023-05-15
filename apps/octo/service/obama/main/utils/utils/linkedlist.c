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
#include <stdlib.h>
#include <octo_common.h>

#include "vkernel.h"
#include "linkedlist.h"


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define POINTERLIST_POOL_NUM			15
#define DEFAULT_POINTERLIST_NUM		4000






/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct	tagPointerListPool
{
	HUINT32					ulSemId;
	HUINT32					ulPoolSize;
	HUINT32					ulFreeCount;
	POINTER_LIST_T*			pBuffer[POINTERLIST_POOL_NUM];
	POINTER_LIST_T*			pPoolHead;
}POINTER_LIST_POOL;


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static POINTER_LIST_POOL		sPointerListPool = {0, 0, 0, {NULL, }, NULL};



/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
static HERROR				local_linkedlist_InitItemPool(void);
static HERROR				local_linkedlist_CreatePool(void);
static HERROR				local_linkedlist_AllocItem(POINTER_LIST_T **ppNewItem);
static HERROR				local_linkedlist_FreeItem(POINTER_LIST_T *pFreeItem);
static POINTER_LIST_T*		local_linkedlist_UnchainListItemFromPointerList(POINTER_LIST_T *pRoot, POINTER_LIST_T *pItem);
static POINTER_LIST_T*		local_linkedlist_DeleteListItemFromPointerList(POINTER_LIST_T *pRoot, POINTER_LIST_T *pItem, FREE_FUNC pfnMemFree);
//static POINTER_LIST_T*		local_linkedlist_FindNearestLowerRankListItemFromPointerList(POINTER_LIST_T *pRoot, void *pContents, RANK_FUNC pfnRank);
static POINTER_LIST_T*		local_linkedlist_FindNearestHigherRankListItemFromPointerList(POINTER_LIST_T *pRoot, void *pContents, RANK_FUNC pfnRank);
static POINTER_LIST_T*		local_linkedlist_InsertListItemToPointerList(POINTER_LIST_T *pRoot, void *pContents, RANK_FUNC pfnRank);
static POINTER_LIST_T*		local_linkedlist_AppendListItemToPointerList(POINTER_LIST_T *pRoot, void *pContents);
static POINTER_LIST_T*		local_linkedlist_InsertListItemToPointerListNext(POINTER_LIST_T *pRoot, POINTER_LIST_T *pCurrent, void *pContents);
static POINTER_LIST_T*		local_linkedlist_InsertListItemToPointerListPre(POINTER_LIST_T *pRoot, POINTER_LIST_T *pCurrent, void *pContents);
static POINTER_LIST_T*		local_linkedlist_FindNthListItemFromPointerList(POINTER_LIST_T *pRoot, HUINT16 usIndex, FILTER_FUNC pfnFilterFunc, void *pvFilter);
static POINTER_LIST_T*		local_linkedlist_FindListItemFromPointerList(POINTER_LIST_T *pRoot, HUINT16 *pusIndex, FILTER_FUNC pfnFilterFunc, void *pvFilter);
static HBOOL				local_linkedlist_FindListFromPointerList(POINTER_LIST_T *pRoot, FILTER_FUNC pfnFilterFunc, void *pvFilter);
static POINTER_LIST_T*		local_linkedlist_FindLastListItemFromPointerList(POINTER_LIST_T *pRoot);
static void 				local_linkedlist_BreakCircularizedPointerList(POINTER_LIST_T *pRoot);
static void 				local_linkedlist_CircularizePointerList(POINTER_LIST_T *pRoot);
static POINTER_LIST_T*		local_linkedlist_FreeWholeListItemsFromPointerList(POINTER_LIST_T *pRoot, FREE_FUNC pfnMemFree);
static HUINT16 			local_linkedlist_GetNumListItemsInPointerList(POINTER_LIST_T *pRoot, FILTER_FUNC pfnFilterFunc, void *pvFilter);
static POINTER_LIST_T*		local_linkedlist_IsListItemInPointerList(POINTER_LIST_T *pRoot, void *pvContents, RANK_FUNC pfnRank, FILTER_FUNC pfnFilterFunc, void *pvFilter);
#if 0 //Not used..
static POINTER_LIST_T*		local_linkedlist_ReplaceListItemConditionallyInPointerList(	POINTER_LIST_T 	*pRoot,
																						void 			*pvContents,
																						RANK_FUNC 		pfnFoundRankFunc,
																						RANK_FUNC 		pfnSortRankFunc,
																						FREE_FUNC 		pfnMemFree);
#endif
static POINTER_LIST_T*		local_linkedlist_DuplicatePointerList(POINTER_LIST_T *pList, RANK_FUNC pfnRank) ;
static POINTER_LIST_T*		local_linkedlist_CopyPointerList(POINTER_LIST_T *pRoot, HBOOL bSorted, RANK_FUNC pfnRank, FILTER_FUNC pfnFilterFunc, void *pvFilter, DATACOPY_FUNC pfnDataCopy);
static POINTER_LIST_T *	local_linkedlist_ExtractInPointerList(POINTER_LIST_T *pRoot, FILTER_FUNC pfnFilterFunc, void *pvFilter, FREE_FUNC pfnMemFree);
static POINTER_LIST_T*		local_linkedlist_DeleteListFromPointerList(POINTER_LIST_T *pRoot, HBOOL bSorted, FILTER_FUNC pfnFilterFunc, void *pvFilter, FREE_FUNC pfnFree);
static POINTER_LIST_T*		local_linkedlist_InsertListItemsInLatterListToFormerList(POINTER_LIST_T *pFormerList, POINTER_LIST_T *pLatterList, RANK_FUNC pfnRank);
static POINTER_LIST_T*		local_linkedlist_GetCommonListItemsFromPointerLists(POINTER_LIST_T *pFormerList, POINTER_LIST_T *pLatterList, RANK_FUNC pfnRank);
static POINTER_LIST_T*		local_linkedlist_CombineTwoPointerLists(	POINTER_LIST_T *pFormerList,
																		POINTER_LIST_T *pLatterList,
																		FILTER_FUNC pfnFilter,
																		RANK_FUNC pfnRank,
																		POINTER_LIST_T **ppRest);
static POINTER_LIST_T*		local_linkedlist_LinkListItems(POINTER_LIST_T *pPrev, POINTER_LIST_T *pItem, POINTER_LIST_T *pNext);
static POINTER_LIST_T*		local_linkedlist_MoveListItemInPointerList(POINTER_LIST_T *pRoot, POINTER_LIST_T *pItem, HINT16 usStepIndex, FILTER_FUNC pfnFilterFunc, void *pvFilter);
static HINT16 				local_linkedlist_GetListItemPositionInPointerList(POINTER_LIST_T *pRoot, void *pvContents, RANK_FUNC pfnRank, FILTER_FUNC pfnFilterCondition, void *pvFilter);
static void 				local_linkedlist_SwapContents(POINTER_LIST_T *pItem1, POINTER_LIST_T *pItem2);
static POINTER_LIST_T*		local_linkedlist_SortListItemInPointerList(POINTER_LIST_T *pItem, FILTER_COMP pfnFilter);
static HBOOL local_linkedlist_FindListFromPointer(POINTER_LIST_T *pRoot, FILTER_FUNC pfnFilterFunc, void *pvFilter);




#define	______GLOBAL_LINKEDLIST_FUNC_START______
HERROR UTIL_LINKEDLIST_InitItemPool(void)
{
	return local_linkedlist_InitItemPool();
}


POINTER_LIST_T *UTIL_LINKEDLIST_InsertListItemToPointerList(POINTER_LIST_T *pRoot, void *pvContents, RANK_FUNC pfnRank)
{
	return local_linkedlist_InsertListItemToPointerList(pRoot, pvContents, pfnRank) ;
}

POINTER_LIST_T *UTIL_LINKEDLIST_AppendListItemToPointerList(POINTER_LIST_T *pRoot, void *pvContents)
{
	return local_linkedlist_AppendListItemToPointerList(pRoot, pvContents) ;
}

POINTER_LIST_T *UTIL_LINKEDLIST_InsertListItemToPointerListNext(POINTER_LIST_T *pRoot, POINTER_LIST_T *pCurrent, void *pvContents)
{
	return local_linkedlist_InsertListItemToPointerListNext(pRoot, pCurrent, pvContents) ;
}

POINTER_LIST_T *UTIL_LINKEDLIST_InsertListItemToPointerListPre(POINTER_LIST_T *pRoot, POINTER_LIST_T *pCurrent, void *pvContents)
{
	return local_linkedlist_InsertListItemToPointerListPre(pRoot, pCurrent, pvContents) ;
}

POINTER_LIST_T *UTIL_LINKEDLIST_FindNthListItemFromPointerList(POINTER_LIST_T *pRoot, HUINT16 usIndex, FILTER_FUNC pfntFilterFunc, void *pvFilter)
{
	return local_linkedlist_FindNthListItemFromPointerList(pRoot, usIndex, pfntFilterFunc, pvFilter) ;
}

POINTER_LIST_T *UTIL_LINKEDLIST_FindListItemFromPointerList(POINTER_LIST_T *pRoot, HUINT16 *pusIndex, FILTER_FUNC pfntFilterFunc, void *pvFilter)
{
	return local_linkedlist_FindListItemFromPointerList(pRoot, pusIndex, pfntFilterFunc, pvFilter) ;
}

HBOOL			UTIL_LINKEDLIST_FindListFromPointerList(POINTER_LIST_T *pRoot, FILTER_FUNC pfnFilterFunc, void *pvFilter)
{
	return local_linkedlist_FindListFromPointerList(pRoot, pfnFilterFunc, pvFilter);
}

HBOOL			UTIL_LINKEDLIST_FindListFromPointer(POINTER_LIST_T *pRoot, FILTER_FUNC pfnFilterFunc, void *pvFilter)
{
	return local_linkedlist_FindListFromPointer(pRoot, pfnFilterFunc, pvFilter);
}


POINTER_LIST_T *UTIL_LINKEDLIST_FindLastListItemFromPointerList(POINTER_LIST_T *pRoot)
{
	return local_linkedlist_FindLastListItemFromPointerList(pRoot) ;
}

POINTER_LIST_T *UTIL_LINKEDLIST_DuplicatePointerList(POINTER_LIST_T *pList, RANK_FUNC pfnRank)
{
	return local_linkedlist_DuplicatePointerList(pList, pfnRank) ;
}

POINTER_LIST_T *UTIL_LINKEDLIST_CopyPointerList(POINTER_LIST_T *pRoot, HBOOL bSorted, RANK_FUNC pfnRank, FILTER_FUNC pfnFilterFunc, void *pvFilter, DATACOPY_FUNC pfnDataCopy)
{
	return local_linkedlist_CopyPointerList(pRoot, bSorted, pfnRank, pfnFilterFunc, pvFilter, pfnDataCopy);
}

POINTER_LIST_T *UTIL_LINKEDLIST_ExtractInPointerList(POINTER_LIST_T *pRoot, FILTER_FUNC pfnFilterFunc, void *pvFilter, FREE_FUNC pfnMemFree)
{
	return local_linkedlist_ExtractInPointerList(pRoot, pfnFilterFunc, pvFilter, pfnMemFree);
}

POINTER_LIST_T *UTIL_LINKEDLIST_GetCommonListItemsFromPointerLists(POINTER_LIST_T *pList1, POINTER_LIST_T *pList2, RANK_FUNC pfnRank)
{
	return local_linkedlist_GetCommonListItemsFromPointerLists(pList1, pList2, pfnRank);
}


POINTER_LIST_T *UTIL_LINKEDLIST_MergeTwoPointerLists(POINTER_LIST_T *pList1, POINTER_LIST_T *pList2, RANK_FUNC pfnRank)
{
	POINTER_LIST_T	*pNewRoot = NULL ;

	pNewRoot = local_linkedlist_DuplicatePointerList(pList1, NULL) ;
	return local_linkedlist_InsertListItemsInLatterListToFormerList(pNewRoot, pList2, pfnRank) ;
}

POINTER_LIST_T *UTIL_LINKEDLIST_CombineTwoPointerLists(	POINTER_LIST_T *pFormerList,
																	POINTER_LIST_T *pLatterList,
																	FILTER_FUNC pfnFilter,
																	RANK_FUNC pfnRank,
																	POINTER_LIST_T **ppRest)
{
	return local_linkedlist_CombineTwoPointerLists(pFormerList, pLatterList, pfnFilter, pfnRank, ppRest) ;
}

POINTER_LIST_T *UTIL_LINKEDLIST_DeleteListItemFromPointerList(POINTER_LIST_T *pRoot, POINTER_LIST_T *pItem, FREE_FUNC pfnMemFree)
{
	return local_linkedlist_DeleteListItemFromPointerList(pRoot, pItem, pfnMemFree);
}

POINTER_LIST_T *UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(POINTER_LIST_T *pRoot, FREE_FUNC pfnMemFree)
{
	return local_linkedlist_FreeWholeListItemsFromPointerList(pRoot, pfnMemFree) ;
}

HUINT16 			UTIL_LINKEDLIST_GetNumListItemsInPointerList(POINTER_LIST_T *pRoot, FILTER_FUNC pfnFilterFunc, void *pvFilter)
{
	return local_linkedlist_GetNumListItemsInPointerList(pRoot, pfnFilterFunc, pvFilter) ;
}

void 			UTIL_LINKEDLIST_BreakCircularizedPointerList(POINTER_LIST_T *pRoot)
{
	local_linkedlist_BreakCircularizedPointerList(pRoot) ;
}

void 			UTIL_LINKEDLIST_CircularizePointerList(POINTER_LIST_T *pRoot)
{
	local_linkedlist_CircularizePointerList(pRoot) ;
}

POINTER_LIST_T *UTIL_LINKEDLIST_MoveListItemInPointerList(POINTER_LIST_T *pRoot, POINTER_LIST_T *pItem, HINT16 usStepIndex, FILTER_FUNC pfnFilterFunc, void *pvFilter)
{
	return local_linkedlist_MoveListItemInPointerList(pRoot, pItem, usStepIndex, pfnFilterFunc, pvFilter);
}

HINT16 			UTIL_LINKEDLIST_GetListItemPositionInPointerList(POINTER_LIST_T *pRoot, void *pvContents, RANK_FUNC pfnRank, FILTER_FUNC pfnFilterCondition, void *pvFilter)
{
	return local_linkedlist_GetListItemPositionInPointerList(pRoot, pvContents, pfnRank, pfnFilterCondition, pvFilter);
}

void 			UTIL_LINKEDLIST_SwapContents(POINTER_LIST_T *pItem1, POINTER_LIST_T *pItem2)
{
	local_linkedlist_SwapContents(pItem1, pItem2) ;
}

POINTER_LIST_T *UTIL_LINKEDLIST_SortListItemInPointerList(POINTER_LIST_T *pItem, FILTER_COMP pfnFilter)
{
	return local_linkedlist_SortListItemInPointerList(pItem, pfnFilter) ;
}

POINTER_LIST_T *UTIL_LINKEDLIST_IsListItemInPointerList(POINTER_LIST_T *pRoot, void *pvContents, RANK_FUNC pfnRank, FILTER_FUNC pfnFilterFunc, void *pvFilter)
{
	return local_linkedlist_IsListItemInPointerList(pRoot, pvContents, pfnRank, pfnFilterFunc, pvFilter);
}


POINTER_LIST_T *UTIL_LINKEDLIST_DeleteListFromPointerList(POINTER_LIST_T *pRoot, HBOOL bSorted, FILTER_FUNC pfnFilterFunc, void *pvFilter, FREE_FUNC pfnFree)
{
	return local_linkedlist_DeleteListFromPointerList(pRoot, bSorted, pfnFilterFunc, pvFilter, pfnFree);
}

POINTER_LIST_T *UTIL_LINKEDLIST_UnchainListItemFromPointerList(POINTER_LIST_T *pRoot, POINTER_LIST_T *pItem)
{
	return local_linkedlist_UnchainListItemFromPointerList(pRoot, pItem);
}


#define	______LOCAL_LINKEDLIST_FUNC_START______

static HERROR			local_linkedlist_InitItemPool(void)
{
	HERROR			hError = ERR_FAIL;

	HxSTD_memset(&sPointerListPool, 0x00, sizeof(POINTER_LIST_POOL));

	hError = VK_SEM_Create((unsigned long*)&sPointerListPool.ulSemId, (const char*)"llist_pool_sem", VK_SUSPENDTYPE_PRIORITY);

	return (VK_OK == hError) ? ERR_OK : ERR_FAIL;
}


static HERROR			local_linkedlist_CreatePool(void)
{
	HERROR			hError = ERR_FAIL;
	HUINT32			ulIndex = 0;
	HUINT32			ulBufIndex = 0;
	POINTER_LIST_T	*pTempItem = NULL;
	POINTER_LIST_T	*pNewBuffer = NULL;
	POINTER_LIST_T	*pNextItem = NULL;

	for(ulIndex = 0; ulIndex < POINTERLIST_POOL_NUM ; ulIndex++)
	{
		if(sPointerListPool.pBuffer[ulIndex] == NULL)
		{
			ulBufIndex = ulIndex;
			break;
		}
	}
	if(ulBufIndex == POINTERLIST_POOL_NUM)
	{
		HxLOG_Print("[local_linkedlist_CreatePool] Buffer full!!\n");
		goto EXIT_LINKEDLIST_FUNC;
	}

	pNewBuffer = (POINTER_LIST_T*)OxPAL_Malloc(sizeof(POINTER_LIST_T)*DEFAULT_POINTERLIST_NUM);
	if(pNewBuffer == NULL)
	{
		HxLOG_Print("[local_linkedlist_CreatePool] Buffer null!!\n");
		goto EXIT_LINKEDLIST_FUNC;
	}
	HxSTD_memset(pNewBuffer, 0x00, sizeof(POINTER_LIST_T)*DEFAULT_POINTERLIST_NUM);
	sPointerListPool.pBuffer[ulBufIndex] = pNewBuffer;

	for(ulIndex = 0; ulIndex < DEFAULT_POINTERLIST_NUM - 1; ulIndex++)
	{
		pTempItem = &(pNewBuffer[ulIndex]);
		pTempItem->pNext = &(pNewBuffer[ulIndex + 1]);
		if(pTempItem->pNext == NULL)
		{
			HxLOG_Print("[local_linkedlist_CreatePool] pNext null!!\n");
		}
		sPointerListPool.ulFreeCount++;
		sPointerListPool.ulPoolSize++;
	}
	sPointerListPool.ulFreeCount++;
	sPointerListPool.ulPoolSize++;

	//swap
	pNextItem = sPointerListPool.pPoolHead;
	sPointerListPool.pPoolHead = pNewBuffer;
	pNewBuffer[DEFAULT_POINTERLIST_NUM-1].pNext = pNextItem;

	hError = ERR_OK;

EXIT_LINKEDLIST_FUNC :

	return hError;

}


static HERROR			local_linkedlist_AllocItem(POINTER_LIST_T **ppNewItem)
{
	HERROR				hError = ERR_FAIL;
	POINTER_LIST_T		*pFreeItem = NULL;

	VK_SEM_Get(sPointerListPool.ulSemId);
	if(ppNewItem == NULL)
	{
		HxLOG_Print("[local_linkedlist_AllocItem] Param null!!\n");
		goto EXIT_LINKEDLIST_FUNC;
	}
	*ppNewItem = NULL;

	if(sPointerListPool.ulFreeCount == 0)
	{
		HxLOG_Print("[local_linkedlist_AllocItem] ulFreeCount zero!!\n");
		hError = local_linkedlist_CreatePool();
		if(hError != ERR_OK)
		{
			HxLOG_Print("[local_linkedlist_AllocItem] ulFreeCount zero!!\n");
			goto EXIT_LINKEDLIST_FUNC;
		}
	}

	pFreeItem = sPointerListPool.pPoolHead;
	if(pFreeItem == NULL)
	{
		HxLOG_Error("[local_linkedlist_FreeItem] pFreeItem null!!\n");
		/* prevent defect: FORWORD_NULL, pFreeItem이 NULL일 경우
			아래 next 참조에서 죽을 수 있다. */
		goto EXIT_LINKEDLIST_FUNC;
	}
	sPointerListPool.pPoolHead = pFreeItem->pNext;
	if(pFreeItem->pNext == NULL)
	{
		HxLOG_Print("[local_linkedlist_FreeItem] pFreeItem->pNext null!!\n");
	}
	sPointerListPool.ulFreeCount--;


	HxSTD_memset(pFreeItem, 0x00, sizeof(POINTER_LIST_T));
	pFreeItem->pNext = pFreeItem->pPrev = NULL;
	*ppNewItem = pFreeItem;

	hError = ERR_OK;

EXIT_LINKEDLIST_FUNC :
	VK_SEM_Release(sPointerListPool.ulSemId);

	return hError;
}


static HERROR			local_linkedlist_FreeItem(POINTER_LIST_T *pFreeItem)
{
	HERROR				hError = ERR_FAIL;
	POINTER_LIST_T		*pPrevItem = NULL;

	VK_SEM_Get(sPointerListPool.ulSemId);
	if(pFreeItem == NULL)
	{
		HxLOG_Print("[local_linkedlist_FreeItem] Param null!!\n");
		goto EXIT_LINKEDLIST_FUNC;
	}

	pPrevItem = sPointerListPool.pPoolHead;
	sPointerListPool.pPoolHead = pFreeItem;
	pFreeItem->pNext = pPrevItem;
	sPointerListPool.ulFreeCount++;

	if(pPrevItem == NULL)
	{
		HxLOG_Print("[local_linkedlist_FreeItem] head null!!\n");
	}

	hError = ERR_OK;

EXIT_LINKEDLIST_FUNC :
	VK_SEM_Release(sPointerListPool.ulSemId);

	return hError;

}




static POINTER_LIST_T *local_linkedlist_UnchainListItemFromPointerList(POINTER_LIST_T *pRoot, POINTER_LIST_T *pItem)
{

	if ( pRoot == NULL )
	{
		return NULL ;
	}

	// just one item in list
	if (	((pRoot->pNext == pRoot && pRoot->pPrev == pRoot) || (pRoot->pNext == pRoot->pPrev && pRoot->pNext == NULL))
		&& (pRoot == pItem) )
	{
		return NULL ;
	}

	if ( pItem->pPrev )
	{
		pItem->pPrev->pNext = pItem->pNext ;
	}
	if ( pItem->pNext )
	{
		pItem->pNext->pPrev = pItem->pPrev ;
	}

	if ( pItem == pRoot )
	{
		pRoot = pItem->pNext ;
	}

	pItem->pNext = NULL ;
	pItem->pPrev = NULL ;

	return pRoot ;
}


static POINTER_LIST_T *local_linkedlist_DeleteListItemFromPointerList(POINTER_LIST_T *pRoot, POINTER_LIST_T *pItem, FREE_FUNC pfnMemFree)
{
	HERROR		hError = ERR_FAIL;

	if (pItem == NULL)
	{
		return pRoot;
	}

	pRoot = local_linkedlist_UnchainListItemFromPointerList(pRoot, pItem) ;

	if ( (pfnMemFree != NULL) && (pItem->pvContents != NULL) )
	{
		pfnMemFree(pItem->pvContents) ;
	}
	hError = local_linkedlist_FreeItem(pItem);
	if(hError != ERR_OK)
	{
		HxLOG_Print("[local_linkedlist_DeleteListItemFromPointerList] Free fail!!\n");
	}

	return pRoot ;
}


#if 0 //Not used..
static POINTER_LIST_T *local_linkedlist_FindNearestLowerRankListItemFromPointerList(POINTER_LIST_T *pRoot, void *pContents, RANK_FUNC pfnRank)
{
	POINTER_LIST_T		*pCurrent  = NULL;
	POINTER_LIST_T		*pFound = NULL ;
	void					*pTempContents = NULL ;


	if ( pContents == NULL || pfnRank == NULL || pRoot == NULL )
	{
		return NULL ;
	}

	pCurrent = pRoot->pPrev ;

	while ( pCurrent )
	{
		pTempContents = pCurrent->pvContents ;
		if ( pfnRank( pTempContents, pContents ) < 0 )
		{
			break ;
		}

		pFound = pCurrent ;
		pCurrent = pCurrent->pPrev ;
		if ( pRoot->pPrev == pCurrent )
		{
			break ;
		}
	}

	return pFound ;
}
#endif


static POINTER_LIST_T *local_linkedlist_FindNearestHigherRankListItemFromPointerList(POINTER_LIST_T *pRoot, void *pContents, RANK_FUNC pfnRank)
{
	POINTER_LIST_T		*pCurrent = NULL;
	POINTER_LIST_T		*pFound = NULL ;
	void					*pTempContents = NULL;

	pCurrent = pRoot ;
	if ( pContents == NULL || pfnRank == NULL )
	{
		return NULL ;
	}

	while ( pCurrent )
	{
		pTempContents = pCurrent->pvContents ;
		if ( pfnRank( pTempContents, pContents ) > 0 )
		{
			break ;
		}

		pFound = pCurrent ;
		pCurrent = pCurrent->pNext ;
		if ( pRoot == pCurrent )
		{
			break ;
		}
	}

	return pFound ;
}



static POINTER_LIST_T *local_linkedlist_InsertListItemToPointerList(POINTER_LIST_T *pRoot, void *pContents, RANK_FUNC pfnRank)
{
	POINTER_LIST_T		*pItem = NULL;
	POINTER_LIST_T		*pFound = NULL;
	HERROR				hError = ERR_FAIL;

	if ( pContents == NULL)
	{
		return pRoot ;
	}


	//pItem = (POINTER_LIST_T *)OxPAL_Malloc(sizeof(POINTER_LIST_T)) ;
	hError = local_linkedlist_AllocItem(&pItem);
	if(hError != ERR_OK)
	{
		HxLOG_Print("*** MW_LINKEDLIST_InsertListItemToPointerList : MEM ALLOC FAIL ***\n");
		return pRoot ;
	}

	pItem->pvContents = pContents ;
	pItem->pPrev = pItem->pNext = NULL ;
	if ( pRoot == NULL )
	{
		pRoot = pItem ;
		return pRoot ;
	}

	pFound = local_linkedlist_FindNearestHigherRankListItemFromPointerList(pRoot, pContents, pfnRank) ;
	if ( pFound == NULL )
	{
		pItem->pPrev = pRoot->pPrev ;
		if ( pRoot->pPrev )
		{
			pRoot->pPrev->pNext = pItem ;
		}

		pRoot->pPrev = pItem ;
		pItem->pNext = pRoot ;

		pRoot = pItem ;
	}
	else
	{
		pItem->pNext = pFound->pNext ;
		pFound->pNext = pItem ;
		pItem->pPrev = pFound ;
		if ( pItem->pNext != NULL )
		{
			pItem->pNext->pPrev = pItem ;
		}
	}

	return pRoot ;
}




static POINTER_LIST_T *local_linkedlist_AppendListItemToPointerList(POINTER_LIST_T *pRoot, void *pContents)
{
	POINTER_LIST_T		*pItem = NULL;
	POINTER_LIST_T		*pFound = NULL;
	HERROR				hError = ERR_FAIL;

	if ( pContents == NULL )
	{
		return pRoot ;
	}

	hError = local_linkedlist_AllocItem(&pItem);
	if(hError != ERR_OK)
	{
		HxLOG_Print("*** MW_LINKEDLIST_AppendListITemToPointerList : MEM ALLOC FAIL ***\n");
		return pRoot ;
	}

	pItem->pvContents = pContents ;
	pItem->pPrev = pItem->pNext = NULL ;
	if ( pRoot == NULL )
	{
		pRoot = pItem ;
		return pRoot ;
	}

	// guarantee that found NOT NULL at this point
	pFound = local_linkedlist_FindLastListItemFromPointerList(pRoot) ;
	local_linkedlist_LinkListItems(pFound, pItem, pFound->pNext) ;

	return pRoot ;
}


static POINTER_LIST_T *local_linkedlist_InsertListItemToPointerListNext(POINTER_LIST_T *pRoot, POINTER_LIST_T *pCurrent, void *pContents)
{
	POINTER_LIST_T		*pItem = NULL;
	POINTER_LIST_T		*pFound = NULL;
	HERROR				hError = ERR_FAIL;

	if ( pContents == NULL )
	{
		return pRoot;
	}

	hError = local_linkedlist_AllocItem(&pItem);
	if(hError != ERR_OK)
	{
		HxLOG_Print("*** local_linkedlist_InsertListItemToPointerListNext : MEM ALLOC FAIL ***\n");
		return pRoot ;
	}

	pItem->pvContents = pContents ;
	pItem->pPrev = pItem->pNext = NULL ;

	if ( pRoot == NULL )
	{
		pRoot = pItem ;

		return pRoot ;
	}

	if ( pCurrent == NULL )
	{
		pFound = local_linkedlist_FindLastListItemFromPointerList(pRoot);
		local_linkedlist_LinkListItems(pFound, pItem, pFound->pNext);

		return pRoot;
	}
#if 0 // tsjung, 20101221 bug: first index에 insert할 시 next item에 대한 link를 잃는다...
	if ( pCurrent == pRoot )
	{
		local_linkedlist_LinkListItems(pRoot, pItem, pRoot);
	}
	else
#endif
	if ( pCurrent->pNext == NULL )
	{
		local_linkedlist_LinkListItems(pCurrent, pItem, NULL);
	}
	else
	{
		local_linkedlist_LinkListItems(pCurrent, pItem, pCurrent->pNext);
	}

	return pRoot ;
}


static POINTER_LIST_T *local_linkedlist_InsertListItemToPointerListPre(POINTER_LIST_T *pRoot, POINTER_LIST_T *pCurrent, void *pContents)
{
	POINTER_LIST_T		*pItem = NULL;
//	POINTER_LIST_T		*pFound = NULL;
	HERROR				hError = ERR_FAIL;

	if ( pContents == NULL || pCurrent == NULL )
	{
		return pRoot;
	}

	hError = local_linkedlist_AllocItem(&pItem);
	if(hError != ERR_OK)
	{
		HxLOG_Print("*** local_linkedlist_InsertListItemToPointerListPre : MEM ALLOC FAIL ***\n");
		return pRoot ;
	}

	pItem->pvContents = pContents ;
	pItem->pPrev = pItem->pNext = NULL ;

	if ( pRoot == NULL )
	{
		pRoot = pItem ;

		return pRoot ;
	}

#if 0 // This is useless code.
	if ( pCurrent == NULL )
	{
		pFound = local_linkedlist_FindLastListItemFromPointerList(pRoot);
		local_linkedlist_LinkListItems(pFound, pItem, pRoot);

		return pItem;
	}
#endif
	if ( pCurrent == pRoot )
	{
		local_linkedlist_LinkListItems(pRoot, pItem, pRoot);
	}
	else if ( pCurrent->pPrev == NULL )
	{
		local_linkedlist_LinkListItems(NULL, pItem, pCurrent);
	}
	else
	{
		local_linkedlist_LinkListItems(pCurrent->pPrev, pItem, pCurrent);

		return pRoot;
	}

	return pItem ;
}


static POINTER_LIST_T *local_linkedlist_FindNthListItemFromPointerList(POINTER_LIST_T *pRoot, HUINT16 usIndex, FILTER_FUNC pfnFilterFunc, void *pvFilter)
{
	POINTER_LIST_T	*pCurrent = NULL ;
	HUINT16			usCount = 0 ;
	HBOOL			bFound = FALSE ;

	if ( !pRoot )
	{
		return NULL ;
	}

	pCurrent = pRoot ;
	do
	{
		if ( pfnFilterFunc )
		{
			if ( pfnFilterFunc(pCurrent->pvContents, pvFilter) )
			{
				if ( usCount == usIndex )
				{
					bFound = TRUE ;
					break ;
				}
				usCount++ ;
			}
		}
		else
		{
			if ( usCount == usIndex )
			{
				bFound = TRUE ;
				break ;
			}
			usCount++ ;
		}
		pCurrent = pCurrent->pNext ;

	}while( pCurrent != pRoot && pCurrent != NULL ) ;

	if (bFound == FALSE)
	{
		return NULL ;
	}

	return pCurrent ;
}



static POINTER_LIST_T *local_linkedlist_FindListItemFromPointerList(POINTER_LIST_T *pRoot, HUINT16 *pusIndex, FILTER_FUNC pfnFilterFunc, void *pvFilter)
{
	POINTER_LIST_T	*pCurrent = NULL ;
	HUINT16			usCount = 0 ;
	HBOOL			bFound = FALSE ;

	if ( !pRoot || !pfnFilterFunc )
	{
		return NULL ;
	}

	pCurrent = pRoot ;
	do
	{
		if ( pfnFilterFunc(pCurrent->pvContents, pvFilter) )
		{
			if(pusIndex)
			{
				*pusIndex = usCount;
			}
			bFound = TRUE ;
			break ;
		}

		pCurrent = pCurrent->pNext ;
		usCount++ ;

	}while( pCurrent != pRoot && pCurrent != NULL ) ;

	if (bFound == FALSE)
	{
		return NULL ;
	}

	return pCurrent ;
}



static HBOOL local_linkedlist_FindListFromPointerList(POINTER_LIST_T *pRoot, FILTER_FUNC pfnFilterFunc, void *pvFilter)
{
	POINTER_LIST_T	*pCurrent = NULL ;
	HBOOL			bFound = FALSE ;

	if ( !pRoot || !pfnFilterFunc )
	{
		return FALSE ;
	}

	pCurrent = pRoot ;
	do
	{
		if ( pfnFilterFunc(pCurrent->pvContents, pvFilter) )
		{
			bFound = TRUE ;
		}
		pCurrent = pCurrent->pNext ;

	}while( pCurrent != pRoot && pCurrent != NULL ) ;

	return bFound ;
}


static HBOOL local_linkedlist_FindListFromPointer(POINTER_LIST_T *pRoot, FILTER_FUNC pfnFilterFunc, void *pvFilter)
{
	POINTER_LIST_T	*pCurrent = NULL ;
//	HBOOL			bFound = FALSE ;

	if ( !pRoot || !pfnFilterFunc )
	{
		return FALSE ;
	}

	pCurrent = pRoot ;
	do
	{
		if ( pfnFilterFunc(pCurrent->pvContents, pvFilter) )
		{
			return TRUE;
		}
		pCurrent = pCurrent->pNext ;

	}while( pCurrent != pRoot && pCurrent != NULL ) ;

	return FALSE ;
}





static POINTER_LIST_T *local_linkedlist_FindLastListItemFromPointerList(POINTER_LIST_T *pRoot)
{
	POINTER_LIST_T		*pCurrent = NULL;
	POINTER_LIST_T		*pFound = NULL ;

	if ( !pRoot )
	{
		return NULL ;
	}

	// circular list
	if ( pRoot->pPrev )
	{
		return pRoot->pPrev ;
	}

	pCurrent = pRoot ;
	while ( pCurrent )
	{
		pFound = pCurrent ;
		pCurrent = pCurrent->pNext ;
		if ( pRoot == pCurrent )
		{
			break ;
		}
	}

	return pFound;
}



static void local_linkedlist_BreakCircularizedPointerList(POINTER_LIST_T *pRoot)
{
	if (	pRoot != NULL
		&& pRoot->pPrev
	)
	{
		pRoot->pPrev->pNext = NULL ;
		pRoot->pPrev = NULL ;
	}
}



static void local_linkedlist_CircularizePointerList(POINTER_LIST_T *pRoot)
{
	POINTER_LIST_T	*pLast = NULL ;

	if ( pRoot )
	{
		if(	pRoot->pPrev != NULL
			&& pRoot->pPrev->pNext == pRoot
		)
		{
			return ;
		}

		pLast = local_linkedlist_FindLastListItemFromPointerList(pRoot) ;

		pRoot->pPrev = pLast ;
		pLast->pNext = pRoot ;
	}
}



static POINTER_LIST_T *local_linkedlist_FreeWholeListItemsFromPointerList(POINTER_LIST_T *pRoot, FREE_FUNC pfnMemFree)
{
	POINTER_LIST_T	*pItem = NULL ;
	HERROR			hError = ERR_FAIL;

	if ( pRoot == NULL )
	{
		return pRoot ;
	}

	// Linearize
  	if ( pRoot->pPrev )
	{
  		pRoot->pPrev->pNext = NULL ;
  		pRoot->pPrev = NULL ;
  	}

	while ( pRoot )
	{
		pItem = pRoot ;
		pRoot = pItem->pNext ;

		if ( pfnMemFree )
		{
			pfnMemFree(pItem->pvContents) ;
		}
		//OxPAL_Free(pItem) ;
		hError = local_linkedlist_FreeItem(pItem);
		if(hError != ERR_OK)
		{
			HxLOG_Print("[local_linkedlist_FreeWholeListItemsFromPointerList] Free fail!!\n");
		}
	}

	return NULL ;
}



static HUINT16 local_linkedlist_GetNumListItemsInPointerList(POINTER_LIST_T *pRoot, FILTER_FUNC pfnFilterFunc, void *pvFilter)
{
	POINTER_LIST_T	*pCurrent = NULL;
	HUINT16			usCount = 0 ;

	pCurrent = pRoot ;
	while ( pCurrent != NULL )
	{
		if ( pfnFilterFunc != NULL )
		{
			if ( pfnFilterFunc(pCurrent->pvContents, pvFilter) )
			{
				usCount++ ;
			}
		}
		else
		{
			usCount++ ;
		}

		pCurrent = pCurrent->pNext ;
		if ( pCurrent == pRoot )
		{
			break ;
		}
	}

	return usCount ;
}



static POINTER_LIST_T *local_linkedlist_IsListItemInPointerList(POINTER_LIST_T *pRoot, void *pvContents, RANK_FUNC pfnRank, FILTER_FUNC pfnFilterFunc, void *pvFilter)
{
	POINTER_LIST_T		*pItem = NULL;
	void					*pTempContents = NULL;
	HBOOL				bMatched = FALSE ;

	if ( pRoot == NULL || (pfnRank && pvContents == NULL) )
	{
		return NULL ;
	}

	pItem = pRoot ;

	while ( pItem )
	{

		pTempContents = pItem->pvContents ;
		if ( pfnFilterFunc )
		{
			bMatched = pfnFilterFunc(pTempContents, pvFilter) ;
		}

		if ( pfnRank )
		{
			if (	pfnRank(pTempContents, pvContents) == 0 && bMatched == TRUE)
			{
				return pItem ;
			}
		}
		else
		{
			if ( pvContents )
			{
				if ( pTempContents == pvContents && bMatched )
				{
					return pItem ;
				}
			}
			else
			{
				if ( bMatched )
				{
					return pItem ;
				}
			}
		}

		pItem = pItem->pNext ;
		if ( pItem == pRoot )
		{
			break ;
		}
	}

	return NULL ;
}



#if 0 //Not used..

static POINTER_LIST_T *local_linkedlist_ReplaceListItemConditionallyInPointerList(	POINTER_LIST_T 	*pRoot,
																						void 			*pvContents,
																						RANK_FUNC 		pfnFoundRankFunc,
																						RANK_FUNC 		pfnSortRankFunc,
																						FREE_FUNC 		pfnMemFree)
{
	POINTER_LIST_T	*pFound = FALSE;
	HERROR			hError = ERR_FAIL;

	if ( pfnFoundRankFunc == NULL || pfnMemFree == NULL )
	{
		return pRoot ;
	}

	pFound = local_linkedlist_IsListItemInPointerList(pRoot, pvContents, pfnFoundRankFunc, NULL, NULL) ;

	if ( pFound )
	{
		if ( pfnSortRankFunc )
		{
			if ( pfnSortRankFunc(pvContents, pFound->pvContents) < 0 )
			{
				pRoot = local_linkedlist_UnchainListItemFromPointerList(pRoot, pFound) ;
				pfnMemFree(pFound->pvContents) ;

				//OxPAL_Free(pFound) ;
				hError = local_linkedlist_FreeItem(pFound);
				if(hError != ERR_OK)
				{
					HxLOG_Print("*** local_linkedlist_ReplaceListItemConditionallyInPointerList : Free FAIL ***\n");
				}
				pRoot = local_linkedlist_InsertListItemToPointerList(pRoot, pvContents, pfnSortRankFunc) ;
			}
			else
			{
				pfnMemFree(pvContents) ;
			}
		}
		else
		{
			pfnMemFree(pFound->pvContents) ;
			pFound->pvContents = pvContents ;
		}
	}

	return pRoot ;
}


#endif

static POINTER_LIST_T *local_linkedlist_DuplicatePointerList(POINTER_LIST_T *pList, RANK_FUNC pfnRank)
{
	POINTER_LIST_T	*pCurrent = NULL;
	POINTER_LIST_T	*pNewRoot = NULL ;

	if ( !pList )
	{
		return NULL ;
	}

	pCurrent = pList ;
	do
	{
		if ( pfnRank )
		{
			pNewRoot = local_linkedlist_InsertListItemToPointerList(pNewRoot, pCurrent->pvContents, pfnRank) ;
		}
		else
		{
			pNewRoot = local_linkedlist_AppendListItemToPointerList(pNewRoot, pCurrent->pvContents) ;
		}
		pCurrent = pCurrent->pNext ;

	} while ( pCurrent != pList && pCurrent != NULL ) ;

	// circular list
	if ( pCurrent == pList )
	{
#if		0
		local_linkedlist_CircularizePointerList(pNewRoot) ;
#endif
	}

	return pNewRoot ;
}



static POINTER_LIST_T *local_linkedlist_DeleteListFromPointerList(POINTER_LIST_T *pRoot, HBOOL bSorted, FILTER_FUNC pfnFilterFunc, void *pvFilter, FREE_FUNC pfnFree)
{
	POINTER_LIST_T	*pCurrent = NULL;
	POINTER_LIST_T	*pTempItem = NULL;
	HBOOL			bMatched = TRUE;
	HBOOL			bStart = FALSE;
	HUINT16 			usListNum = 0, usCount = 0;

	if ( pRoot == NULL )
	{
		return NULL ;
	}

	usListNum= local_linkedlist_GetNumListItemsInPointerList( pRoot, NULL, NULL );

	pCurrent = pRoot ;
	do
	{
		bMatched = FALSE;
		pTempItem = pCurrent;
		pCurrent = pCurrent->pNext;
		usCount++;
		if ( pfnFilterFunc )
		{
			bMatched = pfnFilterFunc(pTempItem->pvContents, pvFilter);
			if(bMatched)
			{
				pRoot = local_linkedlist_DeleteListItemFromPointerList(pRoot, pTempItem, pfnFree);
			}
		}

#if		1
		if(bSorted == TRUE)
		{
			if(bMatched == TRUE)
			{
				if(bStart == FALSE)
				{
					bStart = TRUE;
				}
			}
			else
			{
				if(bStart == TRUE)
				{
					break;
				}
			}
		}
#endif
		if( usCount >= usListNum )
		{
			break;
		}
	} while ( pCurrent ) ;

	return pRoot ;
}






static POINTER_LIST_T *local_linkedlist_CopyPointerList(POINTER_LIST_T *pRoot, HBOOL bSorted, RANK_FUNC pfnRank, FILTER_FUNC pfnFilterFunc, void *pvFilter, DATACOPY_FUNC pfnDataCopy)
{
	POINTER_LIST_T	*pCurrent = NULL;
	POINTER_LIST_T	*pNewRoot = NULL ;
	HBOOL			bMatched = TRUE;
	void				*pvContents = NULL;
	HBOOL			bStart = FALSE;


	if ( !pRoot )
	{
		return NULL ;
	}

	pCurrent = pRoot ;
	do
	{
		if ( pfnFilterFunc )
		{
			bMatched = pfnFilterFunc(pCurrent->pvContents, pvFilter);
			if(bMatched)
			{
				if(pfnDataCopy)
				{
					//duplicate
					pvContents = pfnDataCopy(pCurrent->pvContents);
				}
				else
				{
					pvContents = pCurrent->pvContents;
				}
			}
		}
		else
		{
			if(pfnDataCopy)
			{
				//duplicate
				pvContents = pfnDataCopy(pCurrent->pvContents);
			}
			else
			{
				pvContents = pCurrent->pvContents;
			}
		}

#if		1
		if(bSorted == TRUE)
		{
			if(bMatched == TRUE)
			{
				if(bStart == FALSE)
				{
					bStart = TRUE;
				}
			}
			else
			{
				if(bStart == TRUE)
				{
					break;
				}
			}
		}
#endif

		if ( pfnRank )
		{
			if (bMatched)
			{
				pNewRoot = local_linkedlist_InsertListItemToPointerList(pNewRoot, pvContents, pfnRank) ;
			}
		}
		else
		{
			if (bMatched)
			{
				pNewRoot = local_linkedlist_AppendListItemToPointerList(pNewRoot, pvContents) ;
			}
		}

		pCurrent = pCurrent->pNext ;
		if ( pCurrent == pRoot )
		{
#if			0
			local_linkedlist_CircularizePointerList(pNewRoot) ;
#endif
			break;
		}
	} while ( pCurrent && pCurrent != pRoot ) ;


	return pNewRoot ;
}



static POINTER_LIST_T *local_linkedlist_ExtractInPointerList(POINTER_LIST_T *pRoot, FILTER_FUNC pfnFilterFunc, void *pvFilter, FREE_FUNC pfnMemFree)
{
	POINTER_LIST_T	*pCurrent = NULL;
	POINTER_LIST_T	*pTempItem = NULL;
	HBOOL			bMatched = TRUE;
	HUINT16 			usListNum = 0, usCount = 0;

	if ( pRoot == NULL )
	{
		return NULL ;
	}

	usListNum= local_linkedlist_GetNumListItemsInPointerList( pRoot, NULL, NULL );

	pCurrent = pRoot ;
	do
	{
		bMatched = FALSE;
		pTempItem = pCurrent;
		pCurrent = pCurrent->pNext;
		usCount++;
		if ( pfnFilterFunc )
		{
			bMatched = pfnFilterFunc(pTempItem->pvContents, pvFilter);
			if(!bMatched)
			{
				pRoot = local_linkedlist_DeleteListItemFromPointerList(pRoot, pTempItem, pfnMemFree);
			}
		}

		if( usCount >= usListNum )
		{
			break;
		}
	} while ( pCurrent ) ;

	return pRoot ;
}





static POINTER_LIST_T *local_linkedlist_InsertListItemsInLatterListToFormerList(POINTER_LIST_T *pFormerList, POINTER_LIST_T *pLatterList, RANK_FUNC pfnRank)
{
	while ( pLatterList )
	{
		if ( pfnRank )
		{
			pFormerList = local_linkedlist_InsertListItemToPointerList(pFormerList, pLatterList->pvContents, pfnRank) ;
		}
		else
		{
			pFormerList = local_linkedlist_AppendListItemToPointerList(pFormerList, pLatterList->pvContents) ;
		}
		pLatterList = pLatterList->pNext ;
	}

	return pFormerList ;
}


static POINTER_LIST_T *local_linkedlist_GetCommonListItemsFromPointerLists(POINTER_LIST_T *pFormerList, POINTER_LIST_T *pLatterList, RANK_FUNC pfnRank)
{
	POINTER_LIST_T	*pNewRoot = NULL;
	POINTER_LIST_T	*pCurrent = NULL;
	POINTER_LIST_T	*pFound = NULL;

	pCurrent = pFormerList ;
	while ( pCurrent )
	{
		// same group filtering
		pFound = local_linkedlist_IsListItemInPointerList(pLatterList, pCurrent->pvContents, pfnRank, NULL, NULL) ;
		if ( pFound )
		{
			pNewRoot = local_linkedlist_AppendListItemToPointerList(pNewRoot, pFound->pvContents) ;
#if			0
			local_linkedlist_CircularizePointerList(pNewRoot) ;
#endif
		}

		pCurrent = pCurrent->pNext ;
		if ( pCurrent == pFormerList )
		{
			break ;
		}
	}

	return pNewRoot ;
}



static POINTER_LIST_T *local_linkedlist_CombineTwoPointerLists(	POINTER_LIST_T *pFormerList,
																	POINTER_LIST_T *pLatterList,
																	FILTER_FUNC pfnFilter,
																	RANK_FUNC pfnRank,
																	POINTER_LIST_T **ppRest)
{
	POINTER_LIST_T	*pCurrent = NULL;
	POINTER_LIST_T	*pTemp = NULL;
	POINTER_LIST_T	*pLast1 = NULL;
	POINTER_LIST_T	*pLast2 = NULL;
	HERROR			hError = ERR_FAIL;
	POINTER_LIST_T	*pRootRest = NULL;
	POINTER_LIST_T	*pOrgFound = NULL;
	POINTER_LIST_T	*pFound = NULL;
	void				*pOldContents = NULL;
	void				*pNewContents = NULL;

	if (pLatterList == NULL)
	{
		return pFormerList ;
	}
	if (pFormerList == NULL)
	{
		return pLatterList ;
	}

	*ppRest = NULL;

	// from this point, no lists are null
	if ( pfnRank )
	{
		pCurrent = pLatterList ;
		while ( pCurrent )
		{
			pFound = NULL;
			pFound = local_linkedlist_IsListItemInPointerList(pFormerList, NULL , NULL, pfnFilter, (void*)pCurrent->pvContents);
			if(pFound != NULL)
			{
				pTemp = NULL;
				pTemp = pCurrent;
				pCurrent = pCurrent->pNext ;
				pTemp->pNext = pTemp->pPrev = NULL;

				pOldContents = NULL;
				pOldContents = pFound->pvContents;

				pOrgFound = NULL;
				pOrgFound = pFound;
				pOrgFound->pvContents = NULL;

				// swap!!
				pNewContents = NULL;
				pNewContents = pTemp->pvContents;
				pTemp->pvContents = pOldContents;

				if(pRootRest == NULL)
				{
					pRootRest = pTemp;
				}
				else
				{
					pTemp->pNext = pRootRest;
					pRootRest = pTemp;
				}

#if 1
				pFound = NULL;
				pFound = local_linkedlist_IsListItemInPointerList(pFormerList, NULL , NULL, pfnFilter, (void*)pNewContents);
				while(pFound)
				{
					pFormerList = local_linkedlist_UnchainListItemFromPointerList(pFormerList, pFound);

					if(pRootRest == NULL)
					{
						pRootRest = pFound;
					}
					else
					{
						pFound->pNext = pRootRest;
						pRootRest = pFound;
					}
					pFound = NULL;
					pFound = local_linkedlist_IsListItemInPointerList(pFormerList, NULL , NULL, pfnFilter, (void*)pNewContents);
				}
				if(pOrgFound)
				{
					pOrgFound->pvContents = pNewContents;
				}
#endif

				if ( pCurrent == pLatterList )
				{
					*ppRest = pRootRest;
					return pFormerList;
				}
				continue;
			}

			pFormerList = local_linkedlist_InsertListItemToPointerList(pFormerList, pCurrent->pvContents, pfnRank) ;

			pTemp = pCurrent ;
			pCurrent = pCurrent->pNext ;

			//OxPAL_Free(pTemp) ;
			hError = local_linkedlist_FreeItem(pTemp);
			if(hError != ERR_OK)
			{
				HxLOG_Print("*** local_linkedlist_CombineTwoPointerLists : FREE FAIL ***\n");
			}

			if ( pCurrent == pLatterList )
			{
				break ;
			}
		}
	}
	else
	{
		pLast1 = local_linkedlist_FindLastListItemFromPointerList(pFormerList) ;
		pLast2 = local_linkedlist_FindLastListItemFromPointerList(pLatterList) ;

		pLast2->pNext = pLast1->pNext;			// fixed whether formerList is circular or not
		pLatterList->pPrev = pLast1 ;			// fixed whether both lists are circular or not
		pLast1->pNext = pLatterList ;			// fixed whether both lists are circular or not
		if ( pFormerList->pPrev )				// in case that formerList is circular
		{
			pFormerList->pPrev = pLast2 ;
		}
	}

	*ppRest = pRootRest;

	return pFormerList ;
}




static POINTER_LIST_T *local_linkedlist_LinkListItems(POINTER_LIST_T *pPrev, POINTER_LIST_T *pItem, POINTER_LIST_T *pNext)
{
	if ( !pItem )
	{
		return NULL ;
	}

	if ( pPrev != pItem )
	{
		if ( pPrev )
		{
			pPrev->pNext = pItem ;
		}
		pItem->pPrev = pPrev ;
	}

	if ( pNext != pItem )
	{
		if ( pNext )
		{
			pNext->pPrev = pItem ;
		}
		pItem->pNext = pNext ;
	}

	return pPrev ;
}



static POINTER_LIST_T *local_linkedlist_MoveListItemInPointerList(POINTER_LIST_T *pRoot, POINTER_LIST_T *pItem, HINT16 usStepIndex, FILTER_FUNC pfnFilterFunc, void *pvFilter)
{
	HBOOL			bForward;
	HBOOL			bChangeRoot = FALSE ;
	POINTER_LIST_T	*pMark = NULL ;
	HINT16			usCount = 0 ;

	if ( !pRoot || !pItem)
	{
		return NULL ;
	}

	bForward = (usStepIndex > 0 ? TRUE : FALSE) ;
	usStepIndex *= (bForward ? 1 : (-1)) ;

	// for just one item list
	if ( (pRoot == pItem) && ((pRoot->pNext == pRoot && pRoot->pPrev == pRoot) || (pRoot->pNext == pRoot->pPrev && pRoot->pNext == NULL)) )
	{
		return pRoot ;
	}

	pMark = pItem ;
	do
	{
		if ( pfnFilterFunc )
		{
			if ( pfnFilterFunc(pMark->pvContents, pvFilter) )
			{
				if ( usCount == usStepIndex )
				{
					break ;
				}
				usCount++ ;
			}
		}
		else
		{
			if ( usCount == usStepIndex )
			{
				break ;
			}
			usCount++ ;
		}
		pMark = (bForward ? pMark->pNext : pMark->pPrev) ;


	} while ( pMark != pItem && pMark != NULL ) ;

	// nowhere to move
	if ( pMark == pItem )
	{
		return pRoot ;
	}

	// two items in list
	if ( (pRoot->pNext == pItem && pRoot->pPrev == pItem) && (usStepIndex % 2 == 1) )
	{
		return pRoot->pNext ;
	}

	if ( (!bForward && pItem == pRoot) || (bForward && pItem == pRoot->pPrev) )
	{
		bChangeRoot = TRUE ;
	}

	pRoot = local_linkedlist_UnchainListItemFromPointerList(pRoot, pItem) ;
	if ( pMark )
	{
		if ( bForward )
		{
			if ( bChangeRoot )
			{
				local_linkedlist_LinkListItems(pMark->pPrev, pItem, pMark) ;
				pRoot = pItem ;
			}
			else
			{
				local_linkedlist_LinkListItems(pMark, pItem, pMark->pNext) ;
			}
		}
		else
		{
			if ( bChangeRoot )
			{
				local_linkedlist_LinkListItems(pMark, pItem, pMark->pNext) ;
			}
			else
			{
				local_linkedlist_LinkListItems(pMark->pPrev, pItem, pMark) ;
				if ( pMark == pRoot )
				{
					pRoot = pItem ;
				}
			}
		}
	}
	else
	{
		if ( bForward )
		{
			pMark = local_linkedlist_FindLastListItemFromPointerList(pRoot) ;
			local_linkedlist_LinkListItems(pMark, pItem, pMark->pNext) ;
		}
		else
		{
			local_linkedlist_LinkListItems(pRoot->pPrev, pItem, pRoot) ;
			pRoot = pItem ;
		}
	}
	return pRoot ;

}




static HINT16 local_linkedlist_GetListItemPositionInPointerList(POINTER_LIST_T *pRoot, void *pvContents, RANK_FUNC pfnRank, FILTER_FUNC pfnFilterCondition, void *pvFilter)
{
	HINT16				usPosition = -1 ;
	POINTER_LIST_T		*pCurrent = NULL;

	if ( !pRoot )
	{
		return usPosition ;
	}

	pCurrent = pRoot ;

	do
	{
		if ( pfnFilterCondition )
		{
			if ( pfnFilterCondition(pCurrent->pvContents, pvFilter) )
			{
				usPosition++ ;
			}
		}
		else
		{
			usPosition++ ;
		}

		if ( pfnRank )
		{
			if ( pfnRank(pCurrent->pvContents, pvContents) == 0 )
			{
				return usPosition ;
			}
		}
		else
		{
			if ( pCurrent->pvContents == pvContents )
			{
				return usPosition ;
			}
		}
		pCurrent = pCurrent->pNext ;
	} while ( pCurrent != pRoot && pCurrent ) ;

	return -1 ;
}




static void local_linkedlist_SwapContents(POINTER_LIST_T *pItem1, POINTER_LIST_T *pItem2)
{
	void	*pTemp = NULL;

	pTemp = pItem1->pvContents ;
	pItem1->pvContents = pItem2->pvContents ;
	pItem2->pvContents = pTemp ;
}


static POINTER_LIST_T*	local_linkedlist_SortListItemInPointerList(POINTER_LIST_T *pItem, FILTER_COMP pfnFilter)
{
	void  				**ppSortTable  = NULL;
	HUINT16				usNumTable = 0;
	HUINT16				usIndex = 0;
	POINTER_LIST_T		*pCurrent = NULL;

	if (!pItem)
	{
		return NULL ;
	}

	usNumTable = local_linkedlist_GetNumListItemsInPointerList(pItem, NULL, NULL) ;
	if (!usNumTable)
	{
		return pItem ;
	}

	ppSortTable = (void **) OxPAL_Malloc(sizeof(void *)*usNumTable ) ;
	if ( !ppSortTable )
	{
		HxLOG_Print("*** local_linkedlist_SortListItemInPointerList : MEM ALLOC FAIL ***\n");
		return NULL ;
	}

	pCurrent = pItem ;
	for ( usIndex = 0 ; usIndex < usNumTable ; usIndex++ )
	{
		ppSortTable[usIndex] = (void *) (pCurrent->pvContents);
		pCurrent = pCurrent->pNext ;
	}

	qsort(ppSortTable, usNumTable, sizeof(void*), (int (*)(const void *, const void *))pfnFilter) ;

	pCurrent = pItem ;
	for ( usIndex = 0 ; usIndex < usNumTable ; usIndex++ )
	{
		pCurrent->pvContents = ppSortTable[usIndex] ;
		pCurrent = pCurrent->pNext ;
	}
	OxPAL_Free(ppSortTable) ;

	return pItem ;
}


