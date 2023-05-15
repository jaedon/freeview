
#ifndef _LINKEDLIST_H_
#define	_LINKEDLIST_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <htype.h>


/********************************************************************
*	include
********************************************************************/


/********************************************************************
*	Definitions
********************************************************************/

typedef	struct tagPointerListType
{
   void	*pvContents;

   struct tagPointerListType	*pPrev;
   struct tagPointerListType	*pNext;
} POINTER_LIST_T ;


typedef 	HINT8		(*RANK_FUNC)(void *pvItem1, void *pvItem2) ;
typedef 	void			(*FREE_FUNC)(void *pvItem1) ;
typedef 	HBOOL		(*FILTER_FUNC)(void *pvItem, void *pvFilter) ;
typedef 	HINT32 		(*FILTER_COMP)(void*, void*)  ;
typedef void*		(*DATACOPY_FUNC)(void* pvContents);

#define	UTIL_LINKEDLIST_GetContents(p)								((void*) ((p)->pvContents))
#define	UTIL_LINKEDLIST_GetNextPointer(p)							((p)->pNext)
#define	UTIL_LINKEDLIST_GetPrevPointer(p)							((p)->pPrev)


/********************************************************************
*	function prototypes
********************************************************************/
HERROR			UTIL_LINKEDLIST_InitItemPool(void);
POINTER_LIST_T*	UTIL_LINKEDLIST_InsertListItemToPointerList(POINTER_LIST_T *pRoot, void *pvContents, RANK_FUNC pfnRank);
POINTER_LIST_T* UTIL_LINKEDLIST_InsertListItemToPointerListNext(POINTER_LIST_T *pRoot, POINTER_LIST_T *pCurrent, void *pvContents);
POINTER_LIST_T* UTIL_LINKEDLIST_InsertListItemToPointerListPre(POINTER_LIST_T *pRoot, POINTER_LIST_T *pCurrent, void *pvContents);
/* root pointer를 주고 last에 집어 넣는다 */
POINTER_LIST_T*	UTIL_LINKEDLIST_AppendListItemToPointerList(POINTER_LIST_T *pRoot, void *pvContents);
POINTER_LIST_T*	UTIL_LINKEDLIST_FindNthListItemFromPointerList(POINTER_LIST_T *pRoot, HUINT16 usIndex, FILTER_FUNC pfntFilterFunc, void *pvFilter);

/**
  @description	: pfntFilterFunc와 pvFilter에 맞는 list item을 찾는다.
  @type			: Inline
  @return		: pusIndex(= list index), list item pointer (return값)
				: 못 찾으면 NULL
*/
POINTER_LIST_T* UTIL_LINKEDLIST_FindListItemFromPointerList(POINTER_LIST_T *pRoot, HUINT16 *pusIndex, FILTER_FUNC pfntFilterFunc, void *pvFilter);

HBOOL			UTIL_LINKEDLIST_FindListFromPointerList(POINTER_LIST_T *pRoot, FILTER_FUNC pfnFilterFunc, void *pvFilter);
HBOOL			UTIL_LINKEDLIST_FindListFromPointer(POINTER_LIST_T *pRoot, FILTER_FUNC pfnFilterFunc, void *pvFilter);

POINTER_LIST_T*	UTIL_LINKEDLIST_FindLastListItemFromPointerList(POINTER_LIST_T *pRoot);
POINTER_LIST_T*	UTIL_LINKEDLIST_DuplicatePointerList(POINTER_LIST_T *pList, RANK_FUNC pfnRank);
POINTER_LIST_T*	UTIL_LINKEDLIST_CopyPointerList(POINTER_LIST_T *pRoot, HBOOL bSorted, RANK_FUNC pfnRank, FILTER_FUNC pfnFilterFunc, void *pvFilter, DATACOPY_FUNC pfnDataCopy);
POINTER_LIST_T * UTIL_LINKEDLIST_ExtractInPointerList(POINTER_LIST_T *pRoot, FILTER_FUNC pfnFilterFunc, void *pvFilter, FREE_FUNC pfnMemFree);
POINTER_LIST_T*	UTIL_LINKEDLIST_GetCommonListItemsFromPointerLists(POINTER_LIST_T *pList1, POINTER_LIST_T *pList2, RANK_FUNC pfnRank);
POINTER_LIST_T*	UTIL_LINKEDLIST_MergeTwoPointerLists(POINTER_LIST_T *pList1, POINTER_LIST_T *pList2, RANK_FUNC pfnRank);

POINTER_LIST_T*	UTIL_LINKEDLIST_CombineTwoPointerLists(	POINTER_LIST_T *pFormerList,
																	POINTER_LIST_T *pLatterList,
																	FILTER_FUNC pfnFilter,
																	RANK_FUNC pfnRank,
																	POINTER_LIST_T **ppRest);

POINTER_LIST_T*	UTIL_LINKEDLIST_DeleteListItemFromPointerList(POINTER_LIST_T *pRoot, POINTER_LIST_T *pItem, FREE_FUNC pfnMemFree);
POINTER_LIST_T*	UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(POINTER_LIST_T *pRoot, FREE_FUNC pfnMemFree);
HUINT16 			UTIL_LINKEDLIST_GetNumListItemsInPointerList(POINTER_LIST_T *pRoot, FILTER_FUNC pfnFilterFunc, void *pvFilter);
void 			UTIL_LINKEDLIST_BreakCircularizedPointerList(POINTER_LIST_T *pRoot);
/* root pointer를 주고, last와 first를 잇는다 */
void 			UTIL_LINKEDLIST_CircularizePointerList(POINTER_LIST_T *pRoot);
POINTER_LIST_T*	UTIL_LINKEDLIST_MoveListItemInPointerList(POINTER_LIST_T *pRoot, POINTER_LIST_T *pItem, HINT16 usStepIndex, FILTER_FUNC pfnFilterFunc, void *pvFilter);
HINT16 			UTIL_LINKEDLIST_GetListItemPositionInPointerList(POINTER_LIST_T *pRoot, void *pvContents, RANK_FUNC pfnRank, FILTER_FUNC pfnFilterCondition, void *pvFilter);
void 			UTIL_LINKEDLIST_SwapContents(POINTER_LIST_T *pItem1, POINTER_LIST_T *pItem2);
POINTER_LIST_T*	UTIL_LINKEDLIST_SortListItemInPointerList(POINTER_LIST_T *pItem, FILTER_COMP pfnFilter);
POINTER_LIST_T *UTIL_LINKEDLIST_IsListItemInPointerList(POINTER_LIST_T *pRoot, void *pvContents, RANK_FUNC pfnRank, FILTER_FUNC pfnFilterFunc, void *pvFilter);
POINTER_LIST_T *UTIL_LINKEDLIST_DeleteListFromPointerList(POINTER_LIST_T *pRoot, HBOOL bSorted, FILTER_FUNC pfnFilterFunc, void *pvFilter, FREE_FUNC pfnFree);
POINTER_LIST_T *UTIL_LINKEDLIST_UnchainListItemFromPointerList(POINTER_LIST_T *pRoot, POINTER_LIST_T *pItem);


#endif	//_LINKEDLIST_H_
