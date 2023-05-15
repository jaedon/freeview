

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <stdarg.h>

#include "_hlib_queue.h"
#include "_hlib_mem.h"

//#define	QUEUE_MALLOC	HLIB_MEM_Malloc
//#define	QUEUE_CALLOC(s)	calloc(sizeof(char), s)
//#define	QUEUE_FREE		free

#define	QUEUE_MALLOC	HLIB_MEM_Malloc
#define	QUEUE_CALLOC	HLIB_MEM_Calloc
#define	QUEUE_FREE		HLIB_MEM_Free

#define	QUEUE_ASSERT	assert
#define	QUEUE_DEFAULT_LENGTH	32

struct hxqueue
{
#if defined(CONFIG_DEBUG)
	HCHAR				* debugText;
	HUINT32 			debugLine;
#endif
	HxQueue_RemoveCallback_t	removeNotify;
	HBOOL 		dynamic;
	HINT32		front;
	HINT32		rear;
	HUINT32		count;
	HUINT32		length;
	void 		**array;
};

#define	QUEUE_INDEX(q,index)	((index) % (q)->length)
#define	QUEUE_ADVANCE(q,field)	QUEUE_INDEX(q, (field) + 1)

static
HBOOL		queue_pop (HxQueue_t *q, void **element)
{
	if (q->count == 0)
		return FALSE;

	*element = q->array[q->front];
	q->front = QUEUE_ADVANCE(q, q->front);
	q->count--;
	return TRUE;
}

static
HBOOL		queue_push (HxQueue_t *q, void *element)
{
	if (q->count == q->length)
		return FALSE;

	q->rear  = QUEUE_ADVANCE(q, q->rear);
	q->array[q->rear] = element;
	q->count++;
	return TRUE;
}

static
HBOOL		queue_grow (HxQueue_t *q, HUINT32 length)
{
	void **array;

#if defined(CONFIG_DEBUG)
	array = (void **)DS_MEM_Calloc(length * sizeof(void *), q, q->debugText, q->debugLine);
#else
	array = (void **)QUEUE_CALLOC(length * sizeof(void *));
#endif

	if (array == NULL)
		return FALSE;

	if (q->array)
	{
		HUINT32	i;

		for (i = 0 ; i < q->count ; i++)
		{
			array[i] = q->array[(i + q->front) % q->length];
		}
#if defined(CONFIG_DEBUG)
		DS_MEM_Free(q->array,q,q->debugText,q->debugLine);
#else
		QUEUE_FREE(q->array);
#endif
	}
	q->length = length;
	q->array  = array;
	q->front  = 0;
	q->rear   = (HINT32)q->count - 1;

	return TRUE;
}



HxQueue_t *	_HLIB_QUEUE_New (HxQueue_RemoveCallback_t removeNotify, const char* file, const int line)
{
	return _HLIB_QUEUE_NewEx(QUEUE_DEFAULT_LENGTH, TRUE, removeNotify, file, line);
}

HxQueue_t *	_HLIB_QUEUE_NewEx (HUINT32 length, HBOOL dynamic, HxQueue_RemoveCallback_t removeNotify, const char* file, const int line)
{
	HxQueue_t *q;
#if defined(CONFIG_DEBUG)
	q = (HxQueue_t *)DS_MEM_Calloc(sizeof(HxQueue_t), NULL, file, line);
	q->debugText = (HCHAR*)file;
	q->debugLine = (HUINT32)line;
#else
	q = (HxQueue_t *)QUEUE_CALLOC(sizeof(HxQueue_t));
#endif
	if (q == NULL)
		return NULL;

	if (length == 0)
		length = QUEUE_DEFAULT_LENGTH;

	if (!queue_grow(q, length))
	{
#if defined(CONFIG_DEBUG)
		DS_MEM_Free(q,q,q->debugText,q->debugLine);
#else
		QUEUE_FREE(q);
#endif
		return NULL;
	}
	q->dynamic = dynamic;
	q->removeNotify = removeNotify;
	return q;
}


void		HLIB_QUEUE_Delete (HxQueue_t *q)
{
	if (q)
	{
		HLIB_QUEUE_Clear(q);
#if defined(CONFIG_DEBUG)
		DS_MEM_Free(q->array,q,q->debugText,q->debugLine);
		DS_MEM_Free(q,q,q->debugText,q->debugLine);
#else
		QUEUE_FREE(q->array);
		QUEUE_FREE(q);
#endif
	}
}

HUINT32		HLIB_QUEUE_Length (HxQueue_t *q)
{
	return q->count;
}

void		HLIB_QUEUE_Clear (HxQueue_t *q)
{
	void *elem;

	while (queue_pop(q, &elem))
	{
		if (q->removeNotify)
			q->removeNotify(elem);
	}
}

void *		HLIB_QUEUE_Front (HxQueue_t *q)
{
	if (q->count > 0)
		return q->array[q->front];
	return NULL;
}

void *		HLIB_QUEUE_Back (HxQueue_t *q)
{
	if (q->count > 0)
		return q->array[q->rear];
	return NULL;
}

void *		HLIB_QUEUE_Pop (HxQueue_t *q)
{
	void *elem;

	if (queue_pop(q, &elem))
		return elem;
	return NULL;
}

HBOOL		HLIB_QUEUE_PopEx (HxQueue_t *q, void **element)
{
	return queue_pop(q, element);
}

HBOOL		HLIB_QUEUE_Push (HxQueue_t *q, void *element)
{
	if (!queue_push(q, element))
	{
		if (!q->dynamic)
			return FALSE;

		if (!queue_grow(q, q->length * 2))
			return FALSE;
		return queue_push(q, element);
	}
	return TRUE;
}

