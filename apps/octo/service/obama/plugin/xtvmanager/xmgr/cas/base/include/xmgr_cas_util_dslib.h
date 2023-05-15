/**
	@file     .h
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

#ifndef	_UTIL_DS_LIB_H_
#define _UTIL_DS_LIB_H_

#include <htype.h>

typedef		void	*ItemRef;
typedef		void	**ItemPRef;


/* function for finding item,
	if item1 and item2 is equal, return TRUE,
	if items is different, return FALSE.
*/
typedef HBOOL		(*ll_finditem_method)(void *item1, void *item2);

/* function for comparing item,
	if item1 and item2 is equal, return 0,
	if item1 is bigger than item2, return positive integer.
	if item1 is lesser than item2, return negative integer.
*/
typedef HINT32		(*ll_compitem_method)(void *item1, void *item2);

/* function for destructing content of item,
	need to dealloc memory.
*/
typedef HERROR		(*ll_deleteitem_method)(void *item1);

/* function for printing content of item,
	need to print content.
*/
typedef HINT32		(*ll_printitem_method)(void *item1);

struct l_list {

	int		index;
	void 	*pContent;
	struct l_list *pPrev;
	struct l_list *pNext;
};

typedef struct tag_list {

	struct l_list *iterator;

	int	nNumOfListItem;
	struct l_list *pFirst;
	struct l_list *pLast;
	int	bCircular;
} Custom_list_t, Custom_queue_t;

/* 위 structure와 항상 사이즈 위치가 같아야 한다. rename했을 뿐임 */
typedef struct tag_Stack {

	struct l_list *iterator;

	int	nNumOfListItem;
	struct l_list *pTop;
	struct l_list *pBottom;
	int	bReserved;
} Custom_stack_t;

struct l_tree {

	void *pContent;

	struct l_tree *pParent;		/**< parent */
	int nNumOfChild;			/**< child */
	struct l_tree **ppChild;	/**< child */
};

typedef struct tag_Tree {

	struct l_tree *iterator;

	int nDepthOfTree;
	struct l_tree *pTop;
	struct l_tree **ppLeaf;
} Custom_tree_t;
extern HERROR		CAS_UTIL_LQ_InitQueue(Custom_queue_t * pQueue);
extern HERROR		CAS_UTIL_LQ_Enqueue(Custom_queue_t *pQueue, void *pContent);
extern HERROR		CAS_UTIL_LQ_Dequeue(Custom_queue_t *pQueue, void **ppContent);
extern void		CAS_UTIL_LQ_SetQueueEmpty(Custom_queue_t * pQueue, ll_deleteitem_method pfnDestructor);
extern HERROR		CAS_UTIL_LL_InitList(Custom_list_t *pList);
extern HERROR			CAS_UTIL_LL_SetListEmpty(Custom_list_t *pList, ll_deleteitem_method pfnDestructor);
extern HERROR		CAS_UTIL_LL_SetIteratorFront(Custom_list_t *pList);
extern HERROR		CAS_UTIL_LL_SetIteratorLast(Custom_list_t * pList);
extern HERROR		CAS_UTIL_LL_SetIteratorNthIndex(Custom_list_t * pList, HINT32 nth);
extern HERROR		CAS_UTIL_LL_SetIteratorPrev(Custom_list_t * pList);
extern HERROR		CAS_UTIL_LL_SetIteratorNext(Custom_list_t * pList);
extern HERROR		CAS_UTIL_LL_SetCurrentSwapWithNext(Custom_list_t * pList);
extern HERROR		CAS_UTIL_LL_SetCurrentSwapWithPrev(Custom_list_t * pList);
extern HERROR		CAS_UTIL_LL_SetIndexNthSwapWithNext(Custom_list_t * pList, int nth);
extern HERROR		CAS_UTIL_LL_SetIndexNthSwapWithPrev(Custom_list_t * pList, int nth);
extern int 		CAS_UTIL_LL_GetNumOfListItem(Custom_list_t *pList);
extern HERROR		CAS_UTIL_LL_ReadCurrentItem(Custom_list_t *pList, void **ppContent);
extern HERROR		CAS_UTIL_LL_ReadIndexNthItem(Custom_list_t *pList, int nth, void **ppContent);
extern HERROR		CAS_UTIL_LL_PushFront(Custom_list_t *pList, void *pContent);
extern HERROR		CAS_UTIL_LL_PushRear(Custom_list_t *pList, void *pContent);
extern HERROR		CAS_UTIL_LL_AddItem(Custom_list_t *pList, void *pContent);
extern HERROR		CAS_UTIL_LL_PopFront(Custom_list_t *pList, void **ppContent);
extern HERROR		CAS_UTIL_LL_PopRear(Custom_list_t *pList, void **ppContent);
extern HERROR		CAS_UTIL_LL_PopCurrent(Custom_list_t *pList, void **ppOutContent);
extern HERROR		CAS_UTIL_LL_PopContent(Custom_list_t *pList, void *pKeyContent, ll_finditem_method pfnFind, void **ppOutContent);

#endif /* _UTIL_DS_LIB_H_ */

/* end of file */

