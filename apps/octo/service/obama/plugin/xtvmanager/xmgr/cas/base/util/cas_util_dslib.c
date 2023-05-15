/**
	@file     .c
	@brief    normal mem using double linked list.

	Description: \n
	Module: Application(Layer)\n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2008/11/10		initiated					wjmoh\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#include <stdio.h>
#include <stdlib.h>

#include <octo_common.h>

#include <xmgr_cas_util_dslib.h>


#ifdef CONFIG_DEBUG
HUINT32 g_UtilDS_Level = 0; //DBG_DATA|DBG_PRINT|DBG_TRACE;
HUINT32 g_UtilDS_DbgColor = 0;

#define UDSPrint(message)		//( HxLOG_Print("[AppUtil:util dslib]"), HxLOG_Print("%s",message))
#define UDSAssert(c) 			//	((c) ? 0 : (HxLOG_Print("\nApUtil dslib assertion on %s: line%d\n\n", __FILE__, __LINE__)))
#define UDSData(message)		//(( HxLOG_Print message))
#define	UDSListFail(x)		//(( HxLOG_Print("[AppUtil:util dslib]_____Getting item from list is Fail (%i)\n", x)))
#else
#define UDSPrint(message)
#define UDSAssert(c)
#define UDSData(message)
#define	UDSListFail(x)
#endif

#define		USED_FUNCTION			1
#define		UNUSED_FUNCTION			0

#define		FUNCTION_USED_FLAG		UNUSED_FUNCTION

#if (FUNCTION_USED_FLAG == USED_FUNCTION)
STATIC void local_ll_InitList(struct l_list *pTop);
STATIC void local_ll_PrintItem(struct l_list *pCurrent, ll_printitem_method pfnPrintItem);
STATIC int local_ll_GetNumOfListItem(struct l_list *pTop);
STATIC struct l_list *local_ll_GoToIndexN(struct l_list *pTop, int nth);
STATIC struct l_list *local_ll_DeleteFront(struct l_list *pTop, void *pContent, ll_deleteitem_method pfnDestructor);
STATIC struct l_list *local_ll_DeleteRear(struct l_list *pTop, void *pContent, ll_deleteitem_method pfnDestructor);
STATIC struct l_list *local_ll_DeleteItem(struct l_list *pTop, void *pContent, ll_compitem_method pfnCompare, ll_deleteitem_method pfnDestructor);
STATIC struct l_list *local_ll_ForwardFindIndex(struct l_list *pLast,struct l_list *pIterator, int nth);
STATIC struct l_list *local_ll_BackwardFindIndex(struct l_list *pFirst, struct l_list *pIterator, int nth);
#endif
STATIC void local_ll_PrintListAll(struct l_list *pTop, ll_printitem_method pfnPrintItem);
STATIC struct l_list *local_ll_SetListCircular(struct l_list *pTop);
STATIC struct l_list *local_ll_NewNode(void *pContent);
STATIC struct l_list *local_ll_SetListLeaner(struct l_list *pTop);
STATIC struct l_list *local_ll_SetListEmpty(struct l_list *pTop, ll_deleteitem_method pfnDestructor);
STATIC struct l_list *local_ll_PushFront(struct l_list *pTop, struct l_list *pNew);
STATIC struct l_list *local_ll_PushRear(struct l_list *pLast, struct l_list *pNew);
STATIC struct l_list *local_ll_Pop(struct l_list *pItem);
STATIC struct l_list *local_ll_Push(struct l_list *pNew, struct l_list *pBefore, struct l_list *pAfter);
STATIC struct l_list *local_ll_PopFront(struct l_list **ppFirst);
STATIC struct l_list *local_ll_PopRear(struct l_list **ppLast);
STATIC struct l_list *local_ll_ForwardFindItem(struct l_list *pTop, struct l_list *pHere, void *pKeyContent, ll_finditem_method pfnFind);
STATIC struct l_list *local_ll_BackwardFindItem(struct l_list *pLast, struct l_list *pHere, void *pKeyContent, ll_finditem_method pfnFind);
STATIC struct l_list *local_ll_ForwardFind(struct l_list *pIterator, struct l_list *pFirst, struct l_list *pLast, void *pContent, ll_compitem_method pfnCompare);
STATIC struct l_list *local_ll_BackwardFind(struct l_list *pIterator, struct l_list *pFirst, struct l_list *pLast, void *pContent, ll_compitem_method pfnCompare);
STATIC struct l_list *local_ll_GotoNthItem(struct l_list *pTop, int nth);
STATIC struct l_list *local_ll_BacktoNthItem(struct l_list *pLast, int nth);
STATIC struct l_list *local_ll_SwapCurrentWithNext(struct l_list *pCur);
STATIC struct l_list *local_ll_SwapCurrentWithPrev(struct l_list *pCur);


#define	__________INTERNAL_FUNCS_BODY____________________________

struct l_list *local_ll_NewNode(void *pContent)
{
	struct l_list *pNew = NULL;

	pNew = (struct l_list *)OxAP_Malloc(sizeof(struct l_list));

	pNew->index = -1;
	pNew->pContent = pContent;
	pNew->pPrev = NULL;
	pNew->pNext = NULL;

	return pNew;
}


struct l_list *local_ll_SetListCircular(struct l_list *pTop)
{
	struct l_list *pLast = NULL;

	/* firstly, make leaner */
	if(pTop->pPrev != NULL)
	{
		pTop->pPrev->pNext = NULL;
		pTop->pPrev = NULL;
	}

	pLast = pTop;
	while(pLast->pNext != NULL)
	{
		pLast = pLast->pNext;
	}

	pLast->pNext = pTop;
	pTop->pPrev = pLast;

	return pTop;
}

struct l_list *local_ll_SetListLeaner(struct l_list *pTop)
{
	if(pTop == NULL)
	{
		return pTop;
	}

	if(pTop->pPrev != NULL)
	{
		pTop->pPrev->pNext = NULL;
		pTop->pPrev = NULL;
	}

	return pTop;
}


struct l_list *local_ll_SetListEmpty(struct l_list *pTop, ll_deleteitem_method pfnDestructor)
{
	struct l_list *pCurrent = NULL;
	struct l_list *pTmp = NULL;

	if(pTop == NULL)
	{
		return NULL;
	}

	/* prevent a cycle */
	pTop = local_ll_SetListLeaner(pTop);

	pCurrent = pTop;
	while(pCurrent != NULL)
	{
		if(pfnDestructor == NULL)
		{
//			UDSAssert(0);
			UDSPrint(("assert!! destruct function is NULL\n"));
		}
		else
		{
			pfnDestructor(pCurrent->pContent);
		}

		pTmp = pCurrent;
		pCurrent = pCurrent->pNext;
		OxAP_Free(pTmp);
	}

	return pTop = NULL;
}


void local_ll_AllForwardItemIndexIncrease1(struct l_list *pTop, struct l_list *pFromHere)
{
	struct l_list *pCurrent = NULL;

	pCurrent = pFromHere;
	while((pCurrent != NULL) && (pCurrent->pNext != pTop))
	{
		pCurrent->index++;

		UDSData(("%i.", pCurrent->index));
		pCurrent = pCurrent->pNext;
	}
}

void local_ll_AllForwardItemIndexDecrease1(struct l_list *pTop, struct l_list *pFromHere)
{
	struct l_list *pCurrent = NULL;

	pCurrent = pFromHere;
	while((pCurrent != NULL) && (pCurrent->pNext != pTop))
	{
		pCurrent->index--;
		if(pCurrent->index < 0)
		{
			UDSAssert(0);
		}

		UDSData(("%i.", pCurrent->index));
		pCurrent = pCurrent->pNext;
	}
}

void local_ll_AllBackwardItemIndexIncrease1(struct l_list *pLast, struct l_list *pFromHere)
{
	struct l_list *pCurrent = NULL;

	pCurrent = pFromHere;
	while((pCurrent != NULL) && (pCurrent->pPrev != pLast))
	{
		pCurrent->index++;

		UDSData( ("%i.", pCurrent->index));
		pCurrent = pCurrent->pPrev;
	}
}

void local_ll_AllBackwardItemIndexDecrease1(struct l_list *pLast, struct l_list *pFromHere)
{
	struct l_list *pCurrent = NULL;

	pCurrent = pFromHere;
	while((pCurrent != NULL) && (pCurrent->pPrev != pLast))
	{
		pCurrent->index--;
		if(pCurrent->index < 0)
		{
			UDSAssert(0);
		}

		UDSData( ("%i.", pCurrent->index));
		pCurrent = pCurrent->pPrev;
	}
}

void local_ll_PrintListAll(struct l_list *pTop, ll_printitem_method pfnPrintItem)
{
	struct l_list *pCurrent = NULL;

	if(pTop == NULL || pfnPrintItem == NULL)
	{
		return;
	}

	pCurrent = pTop;

	while(pCurrent != NULL)
	{
		UDSData( ("[%i]: ", pCurrent->index));
		pfnPrintItem(pCurrent->pContent);

		if((pCurrent->pNext == NULL) || (pCurrent->pNext == pTop))
		{
			break;
		}
		pCurrent = pCurrent->pNext;
	}

}

struct l_list *		local_ll_BackToIndexN(struct l_list *pLast, int nth)
{
	struct l_list *pCurrent = NULL;

	if(pLast == NULL)
	{
		return NULL;
	}

	pCurrent = pLast;

	/* 리스트 end 조건 */
	while((pCurrent != NULL) && (pCurrent->pPrev != pLast))
	{
		/* if matched */
		if(pCurrent->index == nth)
		{
			UDSData( ("find~~\n"));

			return pCurrent;
		}

		UDSData( ("%i.", pCurrent->index));
		pCurrent = pCurrent->pPrev;
	}

	return NULL;
}



struct l_list *local_ll_PushFront(struct l_list *pTop, struct l_list *pNew)
{
	pNew->pPrev = pTop->pPrev;
	pNew->pNext = pTop;

	pTop->pPrev = pNew;

	return pNew;
}

struct l_list *local_ll_PushRear(struct l_list *pLast, struct l_list *pNew)
{
	pNew->pPrev = pLast;
	pNew->pNext = pLast->pNext;

	pLast->pNext = pNew;

	return pNew;
}

struct l_list *local_ll_Push(struct l_list *pNew, struct l_list *pBefore, struct l_list *pAfter)
{
	pNew->pPrev = pBefore;
	pNew->pNext = pAfter;

	if(pBefore != NULL)
	{
		pBefore->pNext = pNew;
	}

	if(pAfter != NULL)
	{
		pAfter->pPrev = pNew;
	}

	return pNew;
}

struct l_list *local_ll_Pop(struct l_list *pItem)
{
	if(pItem == NULL)
	{
		return NULL;
	}

	if(pItem->pNext != NULL)
	{
		pItem->pNext->pPrev = pItem->pPrev;
	}
	if(pItem->pPrev != NULL)
	{
		pItem->pPrev->pNext = pItem->pNext;
	}

	return pItem;
}

struct l_list *local_ll_PopFront(struct l_list **ppTop)
{
	struct l_list *pTemp = NULL;
	struct l_list *pTmpTop = NULL;

	if(*ppTop == NULL)
	{
		return *ppTop;
	}

	pTmpTop = *ppTop;
	pTemp = pTmpTop;
	/* only one item */
	if((pTmpTop->pNext == NULL) || (pTmpTop->pNext == pTmpTop))
	{
		return pTemp;
	}

	if(pTmpTop->pPrev != NULL)
	{
		pTmpTop->pPrev->pNext = pTmpTop->pNext;
	}
	if(pTmpTop->pNext != NULL)
	{
		pTmpTop->pNext->pPrev = pTmpTop->pPrev;
	}

	*ppTop = pTmpTop->pNext;

	return pTemp;
}

struct l_list *local_ll_PopRear(struct l_list **ppLast)
{
	struct l_list *pTemp = NULL;
	struct l_list *pTmpLast = NULL;

	if(*ppLast == NULL)
	{
		return *ppLast;
	}

	pTmpLast = *ppLast;
	pTemp = pTmpLast;
	/* only one item */
	if((pTmpLast->pPrev == NULL) || (pTmpLast->pPrev == pTmpLast))
	{
		return pTemp;
	}

	if(pTmpLast->pPrev != NULL)
	{
		pTmpLast->pPrev->pNext = pTmpLast->pNext;
	}
	if(pTmpLast->pNext != NULL)
	{
		pTmpLast->pNext->pPrev = pTmpLast->pPrev;
	}

	*ppLast = pTmpLast->pPrev;

	return pTemp;
}

struct l_list *local_ll_ForwardFindItem(struct l_list *pTop, struct l_list *pHere, void *pKeyContent, ll_finditem_method pfnFind)
{
	struct l_list *pCurrent = NULL;

	if(pTop == NULL || pHere == NULL || pKeyContent == NULL || pfnFind == NULL)
	{
		return NULL;
	}

	pCurrent = pHere;
	while((pCurrent != NULL) && (pCurrent->pNext != pTop))
	{
		if(pfnFind(pCurrent->pContent, pKeyContent) == TRUE)
		{
			UDSData( ("f find~!\n"));
			return pCurrent;
		}
		UDSData( ("."));
		pCurrent = pCurrent->pNext;
	}

	UDSData( ("f fail~!\n"));
	return NULL;
}


struct l_list *local_ll_BackwardFindItem(struct l_list *pLast, struct l_list *pHere, void *pKeyContent, ll_finditem_method pfnFind)
{
	struct l_list *pCurrent = NULL;

	if(pLast == NULL || pHere == NULL || pKeyContent == NULL || pfnFind == NULL)
	{
		return NULL;
	}

	pCurrent = pHere;
	while((pCurrent != NULL) && (pCurrent->pPrev != pLast))
	{
		if(pfnFind(pCurrent->pContent, pKeyContent) == TRUE)
		{
			UDSData( ("b find~!\n"));
			return pCurrent;
		}
		UDSData( ("."));
		pCurrent = pCurrent->pPrev;
	}

	UDSData( ("b fail~!\n"));
	return NULL;
}

/* only use at sorted list, if not, can cause loop */
struct l_list *local_ll_ForwardFind(struct l_list *pIterator, struct l_list *pFirst, struct l_list *pLast, void *pContent, ll_compitem_method pfnCompare)
{
	HINT32	nResult = 0;

	if((pIterator == NULL) || (pIterator->pNext == pLast))
	{
		return NULL;
	}

	nResult = pfnCompare(pIterator->pContent, pContent);
	if(nResult == 0)
	{
		return pIterator;
	}
	else if(nResult < 0)
	{
		return local_ll_BackwardFind(pIterator->pPrev, pFirst, pLast, pContent, pfnCompare);
	}
	else /* (nResult > 0) */
	{
		return local_ll_ForwardFind(pIterator->pNext, pFirst, pLast, pContent, pfnCompare);
	}
}

/* only use at sorted list, if not, can cause loop */
struct l_list *local_ll_BackwardFind(struct l_list *pIterator, struct l_list *pFirst, struct l_list *pLast, void *pContent, ll_compitem_method pfnCompare)
{
	HINT32	nResult = 0;

	if((pIterator == NULL) || (pIterator->pPrev == pFirst))
	{
		return NULL;
	}

	nResult = pfnCompare(pIterator->pContent, pContent);
	if(nResult == 0)
	{
		return pIterator;
	}
	else if(nResult < 0)
	{
		return local_ll_BackwardFind(pIterator->pPrev, pFirst, pLast, pContent, pfnCompare);
	}
	else /* (nResult > 0) */
	{
		return local_ll_ForwardFind(pIterator->pNext, pFirst, pLast, pContent, pfnCompare);
	}
}


struct l_list *local_ll_GotoNthItem(struct l_list *pTop, int nth)
{
	int count = 0;
	struct l_list *pCurrent = NULL;

	UDSData( ("local_ll_GotoNthItem (%i) ", nth));

	pCurrent = pTop;
	while((pCurrent != NULL) && (pCurrent->pNext != pTop))
	{
		if(count == nth)
		{
			UDSData( ("find~!\n"));
			return pCurrent;
		}

		count++;
		UDSData( ("."));
		pCurrent = pCurrent->pNext;
	}

	UDSData( ("Cant find~!\n"));

	return NULL;
}


struct l_list *local_ll_BacktoNthItem(struct l_list *pLast, int nth)
{
	int count = 0;
	struct l_list *pCurrent = NULL;

	UDSData( ("local_ll_BacktoNthItem (%i) ", nth));

	pCurrent = pLast;
	while((pCurrent != NULL) && (pCurrent->pPrev != pLast))
	{
		if(count == nth)
		{
			UDSData( ("find~!\n"));
			return pCurrent;
		}

		count++;
		UDSData( ("."));
		pCurrent = pCurrent->pPrev;
	}

	UDSData( ("Cant find~!\n"));

	return NULL;
}


void		local_ll_IncreaseIndexToLast(struct l_list *pFrom, struct l_list *pTop)
{
	struct l_list *pCurrent = NULL;

	UDSData( ("local_ll_IncreaseItemIndex (%i)\n", pFrom->index));

	pCurrent = pFrom;
	while((pCurrent != NULL) && (pCurrent->pNext != pTop))
	{
		pCurrent->index++;

		UDSData( ("%i.", pCurrent->index));
		pCurrent = pCurrent->pNext;
	}

	UDSData( ("end Increase~!\n"));
}


void		local_ll_DecreaseIndexToLast(struct l_list *pFrom, struct l_list *pTop)
{
	struct l_list *pCurrent = NULL;

	UDSData( ("local_ll_InceaseItemIndex (%i)\n", pFrom->index));

	pCurrent = pFrom;
	while((pCurrent != NULL) && (pCurrent->pNext != pTop))
	{
		pCurrent->index--;

		UDSData( ("%i.", pCurrent->index));
		pCurrent = pCurrent->pNext;
	}

	UDSData( ("end Increase~!\n"));
}


void		local_ll_IncreaseIndexToFirst(struct l_list *pFrom, struct l_list *pLast)
{
	struct l_list *pCurrent = NULL;

	UDSData( ("local_ll_IncreaseItemIndex (%i)\n", pFrom->index));

	pCurrent = pFrom;
	while((pCurrent != NULL) && (pCurrent->pPrev != pLast))
	{
		pCurrent->index++;

		UDSData( ("%i.", pCurrent->index));
		pCurrent = pCurrent->pPrev;
	}

	UDSData( ("end Increase~!\n"));
}


void		local_ll_DecreaseIndexToFirst(struct l_list *pFrom, struct l_list *pLast)
{
	struct l_list *pCurrent = NULL;

	UDSData( ("local_ll_InceaseItemIndex (%i)\n", pFrom->index));

	pCurrent = pFrom;
	while((pCurrent != NULL) && (pCurrent->pPrev != pLast))
	{
		pCurrent->index--;

		UDSData( ("%i.", pCurrent->index));
		pCurrent = pCurrent->pPrev;
	}

	UDSData( ("end Increase~!\n"));
}


struct l_list *local_ll_SwapCurrentWithNext(struct l_list *pCur)
{
	int		nTmp = 0;
	struct l_list *pCurNext = NULL;
	struct l_list *pCurPrev = NULL;

	if((pCur == NULL) || (pCur->pNext == NULL))
	{
		return NULL;
	}

	nTmp = pCur->index;
	pCurNext = pCur->pNext;
	pCurPrev = pCur->pPrev;

	pCur->pPrev = pCurNext;
	pCur->pNext = pCurNext->pNext;
	pCur->index = pCurNext->index;

	pCurNext->pPrev = pCurPrev;
	pCurNext->pNext = pCur;
	pCurNext->index = nTmp;

	return pCur;
}


struct l_list *local_ll_SwapCurrentWithPrev(struct l_list *pCur)
{
	int		nTmp = 0;
	struct l_list *pCurNext = NULL;
	struct l_list *pCurPrev = NULL;

	if((pCur == NULL) || (pCur->pPrev == NULL))
	{
		return NULL;
	}

	nTmp = pCur->index;
	pCurNext = pCur->pNext;
	pCurPrev = pCur->pPrev;

	pCur->pPrev = pCurPrev->pPrev;
	pCur->pNext = pCurPrev;
	pCur->index = pCurPrev->index;

	pCurPrev->pPrev = pCur;
	pCurPrev->pNext = pCurNext;
	pCurPrev->index = nTmp;

	return pCur;
}

#if (FUNCTION_USED_FLAG == USED_FUNCTION)
void local_ll_InitList(struct l_list *pTop)
{
	pTop = NULL;
}

void local_ll_PrintItem(struct l_list *pCurrent, ll_printitem_method pfnPrintItem)
{
	if(pCurrent == NULL || pfnPrintItem == NULL)
	{
		return;
	}

	pfnPrintItem(pCurrent->pContent);
}

int local_ll_GetNumOfListItem(struct l_list *pTop)
{
	struct l_list *pCurrent = NULL;
	int	nNumOfList = 0;

	if(pTop == NULL)
	{
		return 0;
	}

	pCurrent = pTop;
	while(pCurrent != NULL)
	{
		nNumOfList++;
		pCurrent = pCurrent->pNext;
		if(pCurrent == pTop)
		{
			break;
		}
	}

	return nNumOfList;
}

struct l_list *		local_ll_GoToIndexN(struct l_list *pTop, int nth)
{
	struct l_list *pCurrent = NULL;

	if(pTop == NULL)
	{
		return NULL;
	}

	pCurrent = pTop;

	/* 리스트 end 조건 */
	while((pCurrent != NULL) && (pCurrent->pNext != pTop))
	{
		/* if matched */
		if(pCurrent->index == nth)
		{
			UDSData( ("find~~\n"));

			return pCurrent;
		}

		UDSData( ("%i.", pCurrent->index));
		pCurrent = pCurrent->pNext;
	}

	return NULL;
}

struct l_list *local_ll_DeleteFront(struct l_list *pTop, void *pContent, ll_deleteitem_method pfnDestructor)
{
	struct l_list *pTemp = NULL;

	if(pTop == NULL || pContent == NULL)
	{
		return pTop;
	}

	/* item destructed */
	if(pfnDestructor == NULL)
	{
		UDSAssert(0);
		UDSPrint(("assert!! destruct function is NULL\n"));
	}
	else
	{
		pfnDestructor(pTop->pContent);
	}

	pTemp = pTop;

	/* if circular */
	pTop->pNext->pPrev = pTop->pPrev;
	pTop = pTop->pNext;

	OxAP_Free(pTemp);

	return pTop;
}

struct l_list *local_ll_DeleteRear(struct l_list *pTop, void *pContent, ll_deleteitem_method pfnDestructor)
{
	struct l_list *pTemp = NULL;
	struct l_list *pCurrent = NULL;

	if(pTop == NULL || pContent == NULL)
	{
		return pTop;
	}

	pCurrent = pTop;
	while(pCurrent != NULL)
	{
		if((pCurrent->pNext == NULL) && (pCurrent->pNext == pTop))
		{
			break;
		}
		pCurrent = pCurrent->pNext;
	}

	/* item destructed */
	if(pfnDestructor == NULL)
	{
		UDSAssert(0);
		UDSPrint(("assert!! destruct function is NULL\n"));
	}
	else
	{
		pfnDestructor(pTop->pContent);
	}

	pTemp= pCurrent;

	pCurrent->pPrev->pNext = pCurrent->pNext;
	pCurrent->pNext->pPrev = pCurrent->pPrev;

	OxAP_Free(pTemp);

	return pTop;
}


struct l_list *local_ll_DeleteItem(struct l_list *pTop, void *pContent, ll_compitem_method pfnCompare, ll_deleteitem_method pfnDestructor)
{
	struct l_list *pTemp = NULL;
	struct l_list *pCurrent = NULL;

	if(pTop == NULL || pContent == NULL || pfnCompare == NULL)
	{
		return pTop;
	}

	pCurrent = pTop;
	while(pfnCompare(pCurrent->pContent, pContent))
	{
		if((pCurrent->pNext == NULL) && (pCurrent->pNext == pTop))
		{
			return pTop;
		}
		pCurrent = pCurrent->pNext;
	}

	/* item destructed */
	if(pfnDestructor == NULL)
	{
		UDSAssert(0);
		UDSPrint(("assert!! destruct function is NULL\n"));
	}
	else
	{
		pfnDestructor(pCurrent->pContent);
	}

	pTemp = pCurrent;

	pCurrent->pPrev->pNext = pCurrent->pNext;
	pCurrent->pNext->pPrev = pCurrent->pPrev;

	OxAP_Free(pTemp);

	return pTop;
}

struct l_list *local_ll_SortList(struct l_list *pTop, int sortOrder, ll_compitem_method pfnCompare)
{
	struct l_list	*ppSortTmpList = NULL;
	void		*pFindedItem = NULL;
	int		i = 0;
	int		nNumOfList = 0;
	struct l_list *pCurrent = NULL;
	struct l_list compareOerand;

	if(pTop == NULL || pfnCompare == NULL)
	{
		return pTop;
	}

	nNumOfList = CAS_UTIL_LL_GetNumOfListItem(pTop);

	/* temp sorting */
	pCurrent = pTop;
	ppSortTmpList = (struct l_list	*)OxAP_Malloc(sizeof(struct l_list)*nNumOfList);
	for(i = 0; i < nNumOfList; i++)
	{
		memcpy(&(ppSortTmpList[i]), pCurrent, sizeof(struct l_list));
		pCurrent = pCurrent->pNext;
	}

	qsort(ppSortTmpList, nNumOfList, sizeof(struct l_list), pfnCompare);

	pCurrent = pTop;
	if(sortOrder == 0)
	{
		for(i = 0; i < nNumOfList; i++)
		{
			pCurrent->pContent = ppSortTmpList[i].pContent;
			pCurrent= pCurrent->pNext;
		}
	}
	else
	{
		for(i = nNumOfList - 1; i >= 0; i--)
		{
			pCurrent->pContent = ppSortTmpList[i].pContent;
			pCurrent = pCurrent->pNext;

		}
	}

	OxAP_Free(ppSortTmpList);

	return pTop;
}

struct l_list *local_ll_ForwardFindIndex(struct l_list *pLast, struct l_list *pIterator, int nth)
{
	struct l_list *pCurrent = pIterator;

	if(pCurrent->index == nth)
	{
		return pCurrent;
	}

	while((pCurrent != NULL) && pCurrent->pNext != pLast)
	{
		if(pCurrent->index == nth)
		{
			return pCurrent;
		}

		pCurrent = pCurrent->pNext;
	}

	return NULL;
}

struct l_list *local_ll_BackwardFindIndex(struct l_list *pFirst, struct l_list *pIterator, int nth)
{
	struct l_list *pCurrent = pIterator;

	if(pCurrent->index == nth)
	{
		return pCurrent;
	}

	while((pCurrent != NULL) && pCurrent->pPrev != pFirst)
	{
		if(pCurrent->index == nth)
		{
			return pCurrent;
		}

		pCurrent = pCurrent->pPrev;
	}

	return NULL;
}

struct l_list *local_ll_BsearchItem(struct l_list *pTop, void *pContent, ll_compitem_method pfnCompare, struct l_list **pFindedNode)
{
	struct l_list	*ppSortTmpList = NULL;
	void		*pFindedItem = NULL;
	int		i = 0;
	int		nNumOfList = 0;
	struct l_list *pCurrent = NULL;
	struct l_list compareOerand;

	if(pTop == NULL || pContent == NULL || pfnCompare == NULL)
	{
		return pTop;
	}

	nNumOfList = CAS_UTIL_LL_GetNumOfListItem(pTop);

	/* temp sorting */
	pCurrent = pTop;
	ppSortTmpList = (struct l_list	*)OxAP_Malloc(sizeof(struct l_list)*nNumOfList);
	for(i = 0; i < nNumOfList; i++)
	{
		memcpy(&(ppSortTmpList[i]), pCurrent, sizeof(struct l_list));
		pCurrent = pCurrent->pNext;
	}

	qsort(ppSortTmpList, nNumOfList, sizeof(struct l_list), pfnCompare);

	compareOerand.pContent = pContent;
	pFindedItem = bsearch(&compareOerand, ppSortTmpList, nNumOfList, sizeof(struct l_list), pfnCompare);

	/* item destructed */
	*pFindedNode = pFindedItem;

	OxAP_Free(ppSortTmpList);

	return pTop;
}
#endif



#define	__________EXTERNAL_FUNCS_BODY______________________________
HERROR		CAS_UTIL_LQ_InitQueue(Custom_queue_t * pQueue)
{
	return CAS_UTIL_LL_InitList((Custom_list_t *)pQueue);
}

HERROR		CAS_UTIL_LQ_Enqueue(Custom_queue_t *pQueue, void *pContent)
{
	return CAS_UTIL_LL_PushFront((Custom_list_t *)pQueue, pContent);
}

HERROR		CAS_UTIL_LQ_Dequeue(Custom_queue_t *pQueue, void **ppContent)
{
	return CAS_UTIL_LL_PopRear((Custom_list_t *)pQueue, ppContent);
}

void		CAS_UTIL_LQ_SetQueueEmpty(Custom_queue_t * pQueue, ll_deleteitem_method pfnDestructor)
{
	CAS_UTIL_LL_SetListEmpty((Custom_list_t *)pQueue, pfnDestructor);
}

HERROR		CAS_UTIL_LL_InitList(Custom_list_t *pList)
{
	if(pList == NULL)
	{
		return ERR_FAIL;
	}

	/* set attributes zero */
	pList->nNumOfListItem = 0;
	pList->bCircular = FALSE;

	pList->pFirst = NULL;
	pList->pLast = NULL;

	pList->iterator = NULL;

	return ERR_OK;
}


HERROR		CAS_UTIL_LL_SetListEmpty(Custom_list_t *pList, ll_deleteitem_method pfnDestructor)
{
	if(pList == NULL)
	{
		return ERR_FAIL;
	}

	if(pList->nNumOfListItem == 0)
	{
		return ERR_OK;
	}

	pList->pFirst = local_ll_SetListEmpty(pList->pFirst, (ll_deleteitem_method)pfnDestructor);

	CAS_UTIL_LL_InitList(pList);

	return ERR_OK;
}

HERROR		CAS_UTIL_LL_SetIteratorFront(Custom_list_t *pList)
{
	if((pList == NULL) || (pList->nNumOfListItem == 0))
	{
		return ERR_FAIL;
	}

	pList->iterator = pList->pFirst;

	return ERR_OK;
}

HERROR		CAS_UTIL_LL_SetIteratorNthIndex(Custom_list_t * pList, HINT32 nth)
{
	if((pList == NULL) || (nth < 0) || (nth >= pList->nNumOfListItem))
	{
		return ERR_FAIL;
	}

	if(pList->iterator != NULL)
	{
		UDSData( ("CAS_UTIL_LL_ReadIndexNthItem iterator index[%i] to(%i)\n", pList->iterator->index, nth));

		if(pList->iterator->index != nth)
		{
			if(nth < pList->iterator->index)
			{
				pList->iterator = local_ll_BacktoNthItem(pList->iterator, (pList->iterator->index - nth));
			}
			else
			{
				pList->iterator = local_ll_GotoNthItem(pList->iterator, (nth - pList->iterator->index));
			}

			/* 이런 경우가 있을까 마는 */
			if(pList->iterator == NULL)
			{
				UDSAssert(0);

				return ERR_FAIL;
			}
		}
		/* 당연히 iterator index와 같으면 아무 할 일이 없다. */
	}
	else	/* iterator is NULL */
	{
		UDSData( ("CAS_UTIL_LL_ReadIndexNthItem iterator to(%i)\n", nth));

		if(nth <= (pList->nNumOfListItem >> 1))
		{
			pList->iterator = local_ll_GotoNthItem(pList->pFirst, nth);
		}
		else
		{
			pList->iterator = local_ll_BacktoNthItem(pList->pLast, ((pList->nNumOfListItem - 1) - nth));
		}
	}

	return ERR_OK;
}


HERROR		CAS_UTIL_LL_SetIteratorPrev(Custom_list_t * pList)
{
	if((pList == NULL)
		|| (pList->nNumOfListItem == 0)
		|| (pList->iterator == NULL)
		|| (pList->iterator->pPrev == NULL)
		|| (pList->iterator->pPrev == pList->pLast))
	{
		return ERR_FAIL;
	}

	pList->iterator = pList->iterator->pPrev;

	return ERR_OK;
}


HERROR		CAS_UTIL_LL_SetIteratorNext(Custom_list_t * pList)
{
	if((pList == NULL)
		|| (pList->nNumOfListItem == 0)
		|| (pList->iterator == NULL)
		|| (pList->iterator->pNext == NULL)
		|| (pList->iterator->pNext == pList->pFirst))
	{
		return ERR_FAIL;
	}

	pList->iterator = pList->iterator->pNext;

	return ERR_OK;
}

int 		CAS_UTIL_LL_GetNumOfListItem(Custom_list_t *pList)
{
	if(pList == NULL)
	{
		return -1;
	}

	return pList->nNumOfListItem;
}

HERROR		CAS_UTIL_LL_ReadCurrentItem(Custom_list_t *pList, void **ppContent)
{
	if((pList == NULL) || (pList->nNumOfListItem == 0) || (pList->iterator == NULL))
	{
		return ERR_FAIL;
	}

	/* init output */
	*ppContent = NULL;
	*ppContent = pList->iterator->pContent;

	return ERR_OK;
}


HERROR		CAS_UTIL_LL_ReadIndexNthItem(Custom_list_t *pList, int nth, void **ppContent)
{
	HERROR	resResult = ERR_FAIL;

	if(nth < 0 || nth >= pList->nNumOfListItem)
	{
		return ERR_FAIL;
	}

	/* init output */
	*ppContent = NULL;

	/* set iterator Nth index */
	resResult = CAS_UTIL_LL_SetIteratorNthIndex(pList, nth);
	if(resResult != ERR_OK)
	{
		return resResult;
	}

	/* read */
	*ppContent = pList->iterator->pContent;

	return ERR_OK;
}

HERROR		CAS_UTIL_LL_PushFront(Custom_list_t *pList, void *pContent)
{
	struct l_list *pNew = NULL;

	if((pList == NULL) || (pContent == NULL))
	{
		return ERR_FAIL;
	}

	pNew = local_ll_NewNode(pContent);
	pList->nNumOfListItem++;

	pNew->index = 0;

	if(pList->pFirst == NULL)
	{
		pList->pFirst = pNew;
		pList->pLast = pNew;
		pList->iterator = pNew;

		UDSData( ("PushFront one item\n"));

		return ERR_OK;
	}

	pList->pFirst = local_ll_PushFront(pList->pFirst, pNew);
	local_ll_AllForwardItemIndexIncrease1(pList->pFirst, pList->pFirst->pNext);

	pList->iterator = pList->pFirst;

	UDSData( ("PushFront other\n"));

	return ERR_OK;
}

HERROR		CAS_UTIL_LL_PushRear(Custom_list_t *pList, void *pContent)
{
	struct l_list *pNew = NULL;

	if((pList == NULL) || (pContent == NULL))
	{
		return ERR_FAIL;
	}

	pNew = local_ll_NewNode(pContent);
	pList->nNumOfListItem++;

	pNew->index = pList->nNumOfListItem - 1;

	if(pList->pLast == NULL)
	{
		pList->pFirst = pNew;
		pList->pLast = pNew;
		pList->iterator = pNew;

		UDSData( ("PushRear one item\n"));

		return ERR_OK;
	}

	pList->pLast = local_ll_PushRear(pList->pLast, pNew);
	pList->iterator = pList->pLast;

	UDSData( ("PushRear other\n"));

	return ERR_OK;
}



HERROR		CAS_UTIL_LL_AddItem(Custom_list_t *pList, void *pContent)
{
	return CAS_UTIL_LL_PushRear(pList, pContent);
}


HERROR		CAS_UTIL_LL_PopFront(Custom_list_t *pList, void **ppContent)
{
	struct l_list *pTemp = NULL;
	struct l_list *pNextFirst = NULL;

	if((pList == NULL) || (pList->nNumOfListItem <= 0) || (ppContent == NULL))
	{
		return ERR_FAIL;
	}

	/* init output */
	*ppContent = NULL;

	/* item passed */
	*ppContent = pList->pFirst->pContent;
	pTemp = pList->pFirst;

	if(pList->nNumOfListItem == 1)
	{
		CAS_UTIL_LL_InitList(pList);
	}
	else
	{
		pNextFirst = pList->pFirst->pNext;
		local_ll_Pop(pList->pFirst);

		pList->nNumOfListItem--;
		pList->pFirst = pNextFirst;
//		pList->iterator = pNextFirst;
		pList->iterator = NULL;

		local_ll_AllForwardItemIndexDecrease1(pList->pFirst, pList->pFirst);
	}

	OxAP_Free(pTemp);

	return ERR_OK;
}

HERROR		CAS_UTIL_LL_PopRear(Custom_list_t *pList, void **ppContent)
{
	struct l_list *pTemp = NULL;
	struct l_list *pNextLast = NULL;

	if((pList == NULL) || (pList->nNumOfListItem <= 0) || (ppContent == NULL))
	{
		return ERR_FAIL;
	}

	/* init output */
	*ppContent = NULL;

	/* item passed */
	*ppContent = pList->pLast->pContent;
	pTemp = pList->pLast;

	if(pList->nNumOfListItem == 1)
	{
		CAS_UTIL_LL_InitList(pList);
	}
	else
	{
		pNextLast = pList->pLast->pPrev;
		local_ll_Pop(pList->pLast);

		pList->nNumOfListItem--;
		pList->pLast = pNextLast;
//		pList->iterator = pNextLast;
		pList->iterator = NULL;
	}

	OxAP_Free(pTemp);

	return ERR_OK;
}


HERROR		CAS_UTIL_LL_PopCurrent(Custom_list_t *pList, void **ppOutContent)
{
	struct l_list *pTemp = NULL;

	if((pList == NULL) || (pList->nNumOfListItem <= 0) || (ppOutContent == NULL))
	{
		return ERR_FAIL;
	}

	/* init output */
	*ppOutContent = NULL;

	if(pList->iterator == NULL)
	{
		*ppOutContent = NULL;

		return ERR_FAIL;
	}

	/* item passed */
	*ppOutContent = pList->iterator->pContent;
	pTemp = pList->iterator;

	if(pList->nNumOfListItem == 1)
	{
		CAS_UTIL_LL_InitList(pList);
	}
	else
	{
		/* 처음 아이팀인지 끝 아이템인지 검사*/
		if(pTemp == pList->pFirst)
		{
			pList->pFirst = pList->pFirst->pNext;
//			pList->iterator = pList->pFirst;

			local_ll_AllForwardItemIndexDecrease1(pList->pFirst, pList->pFirst);
		}
		else if(pTemp == pList->pLast)
		{
			pList->pLast = pList->pLast->pPrev;
//			pList->iterator = pList->pLast;
		}
		else
		{
			local_ll_AllForwardItemIndexDecrease1(pList->pFirst, pTemp->pNext);
		}

		local_ll_Pop(pTemp);
		pList->iterator = NULL;
		pList->nNumOfListItem--;
	}

	OxAP_Free(pTemp);

	return ERR_OK;
}


HERROR		CAS_UTIL_LL_PopContent(Custom_list_t *pList, void *pKeyContent, ll_finditem_method pfnFind, void **ppOutContent)
{
	HBOOL		bResult = FALSE;
	struct l_list *pFindedItem = NULL;

	if((pList == NULL)
		|| (pList->nNumOfListItem <= 0)
		|| (pKeyContent == NULL)
		|| (pfnFind == NULL)
		|| (ppOutContent == NULL))

	{
		return ERR_FAIL;
	}

	/* init output */
	*ppOutContent = NULL;

	if(pList->iterator != NULL)
	{
		bResult = pfnFind(pList->iterator->pContent, pKeyContent);
	}

	/* dont care whether iterator is NULL or not, bResult init value is. */
	if(bResult == FALSE)
	{
		UDSData( ("CAS_UTIL_LL_PushBefore first iterator cant find\n"));
		if(pList->iterator != NULL)
		{
			/* 전방향 검색 */
			UDSData( ("CAS_UTIL_LL_PushBefore 2nd iterator forward find start\n"));
			pFindedItem = local_ll_ForwardFindItem(pList->pFirst, pList->iterator, pKeyContent, pfnFind);
			if(pFindedItem == NULL)
			{
				/* 역방향 검색 */
				UDSData( ("CAS_UTIL_LL_PushBefore 3th iterator backward find start\n"));
				pFindedItem = local_ll_BackwardFindItem(pList->pLast, pList->iterator->pPrev, pKeyContent, pfnFind);
			}
		}
		else
		{
			UDSData( ("CAS_UTIL_LL_PushBefore 2nd first forward find start\n"));
			pFindedItem = local_ll_ForwardFindItem(pList->pFirst, pList->pFirst, pKeyContent, pfnFind);
		}
	}

	if(pFindedItem == NULL)
	{
		*ppOutContent = NULL;

		return ERR_FAIL;
	}

	/* item passed */
	*ppOutContent = pFindedItem->pContent;

	if(pList->nNumOfListItem == 1)
	{
		CAS_UTIL_LL_InitList(pList);
	}
	else
	{
		/* 처음 아이팀인지 끝 아이템인지 검사*/
		if(pFindedItem == pList->pFirst)
		{
			pList->pFirst = pList->pFirst->pNext;
//			pList->iterator = pList->pFirst;

			local_ll_AllForwardItemIndexDecrease1(pList->pFirst, pList->pFirst);
		}
		else if(pFindedItem == pList->pLast)
		{
			pList->pLast = pList->pLast->pPrev;
//			pList->iterator = pList->pLast;
		}
		else
		{
			local_ll_AllForwardItemIndexDecrease1(pList->pFirst, pFindedItem->pNext);
		}

		local_ll_Pop(pFindedItem);
		pList->iterator = NULL;
		pList->nNumOfListItem--;
	}

	OxAP_Free(pFindedItem);

	return ERR_OK;
}


HERROR		CAS_UTIL_LL_FindContents(Custom_list_t *pList, void *pKeyContent, ll_finditem_method pfnFind, void **ppContent)
{
	HBOOL	bResult = FALSE;
	struct l_list *pFindedItem = NULL;

	if((pList == NULL) || (pKeyContent == NULL) || (pfnFind == NULL) || (ppContent == NULL))
	{
		return ERR_FAIL;
	}

	/* init output */
	*ppContent = NULL;

	/* iterator가 NULL이 아니면 캐쉬 포인트 부터 forward 검색해 본다. */
	if(pList->iterator != NULL)
	{
		bResult = pfnFind(pList->iterator->pContent, pKeyContent);
	}

	if(bResult == FALSE)
	{
		UDSData( ("CAS_UTIL_LL_PushBefore first iterator cant find\n"));
		if(pList->iterator != NULL)
		{
			/* 전방향 검색 */
			UDSData( ("CAS_UTIL_LL_PushBefore 2nd iterator forward find start\n"));
			pFindedItem = local_ll_ForwardFindItem(pList->pFirst, pList->iterator, pKeyContent, pfnFind);
			if(pFindedItem == NULL)
			{
				/* 역방향 검색 */
				UDSData( ("CAS_UTIL_LL_PushBefore 3th iterator backward find start\n"));
				pFindedItem = local_ll_BackwardFindItem(pList->pLast, pList->iterator->pPrev, pKeyContent, pfnFind);
			}
		}
		else
		{
			UDSData( ("CAS_UTIL_LL_PushBefore 2nd first forward find start\n"));
			pFindedItem = local_ll_ForwardFindItem(pList->pFirst, pList->pFirst, pKeyContent, pfnFind);
		}
	}

	if(pFindedItem == NULL)
	{
		return ERR_FAIL;
	}

	*ppContent = pFindedItem->pContent;
	pList->iterator = pFindedItem;

	return ERR_OK;
}


/* end of file */

