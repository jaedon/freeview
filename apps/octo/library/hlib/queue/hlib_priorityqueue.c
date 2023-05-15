

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <stdarg.h>

#include "_hlib_priorityqueue.h"
#include "_hlib_queue.h"
#include "_hlib_tree.h"

//#define	HxPRIORITYQUEUE_ANSI
#ifdef	HxPRIORITYQUEUE_ANSI
#define	QUEUE_MALLOC	HLIB_MEM_Malloc
#define	QUEUE_CALLOC(s)	HLIB_MEM_Calloc(s)// calloc(sizeof(char), s)
#define	QUEUE_FREE		HLIB_MEM_Free
#define	QUEUE_ASSERT	assert
#else
#include "_hlib_mem.h"
#include "_hlib_log.h"

#define	QUEUE_MALLOC	HLIB_MEM_Malloc
#define	QUEUE_CALLOC	HLIB_MEM_Calloc
#define	QUEUE_FREE		HLIB_MEM_Free

#define	QUEUE_ASSERT	HxLOG_Assert
#endif

struct hxpriorityqueue
{
#if defined(CONFIG_DEBUG)	
	HCHAR			* debugText;
	HUINT32 		debugLine;
#endif
	HxCompareFunc_t	compare;
	HxFreeFunc_t	free_element;

	HUINT32			length;
	HxTREE_t		*tree;
	HxQueue_t		*key_q;
	HxQueue_t		*freed;
};

static HINT32		priorityqueue_Compare (HxQueue_t *lhs, HxQueue_t *rhs, HxCompareFunc_t compare)
{
	void *litem = HLIB_QUEUE_Front(lhs);
	void *ritem = HLIB_QUEUE_Front(rhs);

	if (compare)
		return compare(litem, ritem);
	return (HINT32)litem - (HINT32)ritem;
}

static HxQueue_t *	priorityqueue_AllocLinearQ (HxPriorityQueue_t *queue)
{
	HxQueue_t *linearQ;

	linearQ = (HxQueue_t *)HLIB_QUEUE_Pop(queue->freed);
	if (linearQ == NULL)
	{
		return HLIB_QUEUE_New((HxQueue_RemoveCallback_t)queue->free_element);
	}
	return linearQ;
}

static void			priorityqueue_FreeLinearQ (HxPriorityQueue_t *queue, HxQueue_t *linearQ)
{
	HLIB_QUEUE_Clear(linearQ);
	HLIB_QUEUE_Push(queue->freed, linearQ);
}

HxPriorityQueue_t *	_HLIB_PRIORITYQUEUE_New (HxCompareFunc_t _comp, HxFreeFunc_t _free, const HCHAR * file, HUINT32 line)
{
	HxPriorityQueue_t	*queue;

#if defined(CONFIG_DEBUG)	
	queue = (HxPriorityQueue_t *)DS_MEM_Calloc(sizeof(HxPriorityQueue_t), NULL, file, line);
#else
	queue = (HxPriorityQueue_t *)QUEUE_CALLOC(sizeof(HxPriorityQueue_t));
#endif
	if (queue)
	{
#if defined(CONFIG_DEBUG)	
		queue->debugText = (HCHAR *)file;
		queue->debugLine = line;
#endif		
		queue->compare = _comp;
		queue->free_element = _free;
		queue->tree = HLIB_TREE_NewFull(
				  (HxCompareDataFunc)priorityqueue_Compare
				, (void *)_comp
				, NULL
				, (HxDestroyNotify)HLIB_QUEUE_Delete
			);
		queue->key_q = HLIB_QUEUE_New(NULL);
		queue->freed = HLIB_QUEUE_NewEx(0, TRUE, (HxQueue_RemoveCallback_t)HLIB_QUEUE_Delete);

		if (queue->tree == NULL
			|| queue->key_q == NULL
			|| queue->freed == NULL)
		{
			HLIB_PRIORITYQUEUE_Delete(queue);
			return NULL;
		}
		HLIB_TREE_NodeCache(queue->tree, TRUE);
	}
	return queue;
}

void		HLIB_PRIORITYQUEUE_Delete (HxPriorityQueue_t *queue)
{
	if (queue)
	{
		if (queue->tree)
			HLIB_TREE_Destroy(queue->tree);
		if (queue->key_q)
			HLIB_QUEUE_Delete(queue->key_q);
		if (queue->freed)
			HLIB_QUEUE_Delete(queue->freed);
#if defined(CONFIG_DEBUG)	
		DS_MEM_Free(queue,queue,queue->debugText, queue->debugLine);
#else
		QUEUE_FREE(queue);
#endif
	}
}

HUINT32		HLIB_PRIORITYQUEUE_Length (HxPriorityQueue_t *queue)
{
	QUEUE_ASSERT(queue);
	return queue->length;
}

void		HLIB_PRIORITYQUEUE_Clear (HxPriorityQueue_t *queue)
{
	QUEUE_ASSERT(queue);

	HLIB_TREE_RemoveAll(queue->tree);
	queue->length = 0;
}

void *		HLIB_PRIORITYQUEUE_Front (HxPriorityQueue_t *queue)
{
	void 		*iter;
	HxQueue_t	*leanQ;

	QUEUE_ASSERT(queue);

	iter = HLIB_TREE_Iterator(queue->tree);
	if (iter == NULL)
		return NULL;

	leanQ = (HxQueue_t *)HLIB_TREE_IteratorValue(iter);
	if (leanQ == NULL)
		return NULL;

	return HLIB_QUEUE_Front(leanQ);
}

void *		HLIB_PRIORITYQUEUE_Back (HxPriorityQueue_t *queue)
{
	void 		*iter;
	HxQueue_t	*leanQ;

	QUEUE_ASSERT(queue);

	leanQ = NULL;
	for ( iter = HLIB_TREE_Iterator(queue->tree)
		; iter
		; iter = HLIB_TREE_NextIterator(iter) )
	{
		leanQ = (HxQueue_t *)HLIB_TREE_IteratorValue(iter);
	}

	if (leanQ == NULL)
		return NULL;

	return HLIB_QUEUE_Back(leanQ);
}

void *		HLIB_PRIORITYQUEUE_Pop (HxPriorityQueue_t *queue)
{
	void *element;

	if (HLIB_PRIORITYQUEUE_PopEx(queue, &element))
		return element;
	return NULL;
}

HBOOL		HLIB_PRIORITYQUEUE_PopEx (HxPriorityQueue_t *queue, void **element)
{
	void 		*iter;
	HxQueue_t	*linearQ;
	HBOOL		success;
	HUINT32		length;

	QUEUE_ASSERT(queue);

	iter = HLIB_TREE_Iterator(queue->tree);
	if (iter == NULL)
		return FALSE;

	linearQ = (HxQueue_t *)HLIB_TREE_IteratorValue(iter);
	if (linearQ == NULL)
		return FALSE;

	length = HLIB_QUEUE_Length(linearQ);
	HxLOG_Assert(length > 0);
	if (length > 1)
	{
		success = HLIB_QUEUE_PopEx(linearQ, element);
	}
	else
	{
		HLIB_TREE_Steal(queue->tree, (const void *)linearQ);
		success = HLIB_QUEUE_PopEx(linearQ, element);
		priorityqueue_FreeLinearQ(queue, linearQ);
	}

	if (success)
		queue->length--;
	return success;
}

HBOOL		HLIB_PRIORITYQUEUE_Push (HxPriorityQueue_t *queue, void *element)
{
	HxQueue_t	*linearQ;
	HBOOL		success = FALSE;

	QUEUE_ASSERT(queue);

	(void) HLIB_QUEUE_Push(queue->key_q, element);
	linearQ = (HxQueue_t *)HLIB_TREE_Lookup(queue->tree, (void *)queue->key_q);
	HLIB_QUEUE_Pop(queue->key_q);

	if (linearQ)
	{
		success = HLIB_QUEUE_Push(linearQ, element);
	}
	else
	{
		linearQ = priorityqueue_AllocLinearQ(queue);
		if (linearQ)
		{
			success = HLIB_QUEUE_Push(linearQ, element);
			if (!success)
			{
				priorityqueue_FreeLinearQ(queue, linearQ);
				return FALSE;
			}
			HLIB_TREE_Insert(queue->tree, (void *)linearQ, (void *)linearQ);
		}
	}
	if (success)
		queue->length++;
	return success;
}


