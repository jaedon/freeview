/*
 * hvector.c
 *
 *  Created on: 2011. 1. 12.
 */


#include <vkernel.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define	HxLOG_DOMAIN	"hxvector"
#include <_hlib_log.h>
#include <_hlib_stdlib.h>
#include <_hlib_mem.h>

#include "_hlib_vector.h"

#define	OS_malloc	HLIB_MEM_Malloc
#define	OS_realloc	HLIB_MEM_Realloc
#define	OS_free		HLIB_MEM_Free
#define	OS_memset	VK_memset
#define	OS_memcpy	VK_memcpy

#define	OS_memmove	memmove
#define	OS_assert	HxLOG_Assert
#define	OS_snprintf	snprintf
#define	HVEC_DEFAULT_LEN	10


#define	____HxVECTOR_PRIVATE_FUNCTIONS______________________________________________

static HINT32	_vector_DefaultCmp (const void *arg1, const void *arg2)
{
	return *(HINT32 *)arg1 - *(HINT32 *)arg2;
}


static HBOOL	_vector_IsFull  (HxVector_t *self)
{
	HINT32 used = self->count * sizeof(void *);

	if (used >= self->size)
	{
		return TRUE;
	}
	return FALSE;
}

static HERROR	_vector_Realloc (HxVector_t *self, HUINT32 size)
{
	void *	mem;

/*
	mem  = OS_realloc((void *)self->vector, size);
	if (mem)
	{
		self->size = size;
		return ERR_OK;
	}
*/
#if defined(CONFIG_DEBUG)
	mem = (void **) DS_MEM_Malloc(size, self, self->debugText, self->debugLine);
#else
	mem  = OS_malloc(size);
#endif
	if (mem == NULL)
	{
		OS_assert(mem != NULL);
		return ERR_FAIL;
	}

	if (self->count > 0)
	{
		OS_memcpy(mem, self->vector, sizeof(void *) * self->count);
	}
#if defined(CONFIG_DEBUG)
	DS_MEM_Free(self->vector,self,self->debugText,self->debugLine);
#else
	OS_free(self->vector);
#endif

	self->vector = (void **)mem;
	self->size   = size;

	return ERR_OK;
}


static HERROR	_vector_Insert (HxVector_t *self, HINT32 n, void *data)
{
	HERROR	err;
	HINT32	size, retry = 5;
	void *	item;

	if (n < 0 || self->count < n)
		return ERR_FAIL;

	while (retry-- && _vector_IsFull(self))
	{
		err = _vector_Realloc(self, self->size * 2);
		if (err != ERR_OK)
			return err;
	}
	if (retry < 0)
	{
		return ERR_FAIL;
	}

	if (self->mkItem)
		item = self->mkItem(data);
	else
		item = data;

#if defined(OS_memmove)
	size = self->count - n;
	if (size > 0)
	{
		OS_memmove(self->vector + n + 1, self->vector + n, sizeof(void *) * size);
	}
	self->vector[n] = item;
#else
	for (size = self->count ; size > n ; size--)
	{
		self->vector[size] = self->vector[size - 1];
	}
	self->vector[size] = item;
#endif
	self->count++;

	return ERR_OK;
}

/////////////////////////////////////////////////////////////////////////////

#define	____HxVECTOR_CompareAPIs____________________________________________________

HINT32	HLIB_VECTOR_CompareHINT8	(const void *arg1, const void *arg2)
{
	return (HINT32)(*(HINT8 *)arg1) - (HINT32)(*(HINT8 *)arg2);
}

HINT32	HLIB_VECTOR_CompareHINT16	(const void *arg1, const void *arg2)
{
	return (HINT32)(*(HINT16 *)arg1) - (HINT32)(*(HINT16 *)arg2);
}

HINT32	HLIB_VECTOR_CompareHINT32	(const void *arg1, const void *arg2)
{
	return *(HINT32 *)arg1 - *(HINT32 *)arg2;
}

HINT32	HLIB_VECTOR_CompareHUINT8	(const void *arg1, const void *arg2)
{
	return (HINT32)(*(HUINT8 *)arg1) - (HINT32)(*(HUINT8 *)arg2);
}

HINT32	HLIB_VECTOR_CompareHUINT16	(const void *arg1, const void *arg2)
{
	return (HINT32)(*(HUINT16 *)arg1) - (HINT32)(*(HUINT16 *)arg2);
}

HINT32	HLIB_VECTOR_CompareHUINT32	(const void *arg1, const void *arg2)
{
	HUINT32	lhs	= *(HUINT32 *)arg1;
	HUINT32	rhs	= *(HUINT32 *)arg2;

	if (lhs == rhs)
	{
		return 0;
	}
	else if (lhs < rhs)
	{
		return -1;
	}
	else
	{
		return 1;
	}
}

HINT32	HLIB_VECTOR_CompareSzASCII	(const void *arg1, const void *arg2)
{
	return (HINT32)strcmp(*(char **)arg1, *(char **)arg2);
}

#define	____HxVECTOR_APIs____________________________________________________


HxVector_t *	_HLIB_VECTOR_New (HxVector_t *self, HINT32 (* cmp)(const void *, const void *), const HCHAR * file, HUINT32 line)
{
	return _HLIB_VECTOR_NewEx(self, HVEC_DEFAULT_LEN, NULL, NULL, cmp, file, line);
}


HxVector_t * _HLIB_VECTOR_NewEx (HxVector_t *self, HINT32 length, void * (*mk)(void *), void * (*rm)(void *), HINT32 (* cmp)(const void *, const void *), const HCHAR * file, HUINT32 line)
{
	HxVector_t *thiz = self;

	if (thiz == NULL)
	{
#if defined(CONFIG_DEBUG)
		thiz = (HxVector_t *) DS_MEM_Malloc(sizeof(HxVector_t), NULL, file, line);
#else
		thiz = (HxVector_t *) OS_malloc (sizeof(HxVector_t));
#endif
		if (thiz == NULL)
		{
			return NULL;
		}
		thiz->allocSelf = thiz;
	}
	else
		thiz->allocSelf = NULL;

#if defined(CONFIG_DEBUG)
		thiz->debugText = (HCHAR*)file;
		thiz->debugLine = line;
#endif

	if (length == 0)
		length = HVEC_DEFAULT_LEN;
	thiz->size = length * sizeof(void *);
	thiz->count  = 0;
	thiz->mkItem = mk;
	thiz->rmItem = rm;
	thiz->cmpItem = cmp;
	if (thiz->cmpItem == NULL)
		thiz->cmpItem = _vector_DefaultCmp;

#if defined(CONFIG_DEBUG)
	thiz->vector = (void **) DS_MEM_Malloc(thiz->size, thiz, file, line);
#else
	thiz->vector = (void **) OS_malloc (thiz->size);
#endif
	if (thiz->vector == NULL)
	{
		if (thiz->allocSelf)
		{
#if defined(CONFIG_DEBUG)
			DS_MEM_Free(thiz,thiz,thiz->debugText,thiz->debugLine);
#else
			OS_free(thiz);
#endif
		}
		return NULL;
	}
	return thiz;
}

HxVector_t	*_HLIB_VECTOR_NewEasy (HINT32 length, HxFreeFunc_t rm, HxCompareFunc_t cmp, const HCHAR * file, HUINT32 line)
{
	return _HLIB_VECTOR_NewEx(NULL, length, NULL, (void *(*)(void *))rm, cmp, file, line);
}

void		HLIB_VECTOR_Delete (HxVector_t *self)
{
	OS_assert(self != NULL);

	(void) HLIB_VECTOR_RemoveAll (self);

	if (self->vector)
	{
#if defined(CONFIG_DEBUG)
		DS_MEM_Free(self->vector,self,self->debugText,self->debugLine);
#else
		OS_free(self->vector);
#endif
		self->vector = NULL;
	}

	if (self->allocSelf)
	{
#if defined(CONFIG_DEBUG)
		DS_MEM_Free(self->allocSelf,self,self->debugText,self->debugLine);
#else
		OS_free(self->allocSelf);
#endif
	}
}

HERROR		HLIB_VECTOR_Add (HxVector_t *self, void *data)
{
	OS_assert(self != NULL);

	return _vector_Insert(self, self->count, data);
}


HERROR		HLIB_VECTOR_Append (HxVector_t *self, HxVector_t *other, void *(*each)(void *))
{
	void *	item;

	OS_assert(self != NULL);
	OS_assert(other != NULL);

	HxVECTOR_EACH(other, void *, item, {
		if (each)
			HLIB_VECTOR_Add(self, each(item));
		else
			HLIB_VECTOR_Add(self, item);
	});
	return ERR_OK;
}


HERROR		HLIB_VECTOR_Insert (HxVector_t *self, HINT32 n, void *data)
{
	OS_assert(self != NULL);

	return _vector_Insert(self, n, data);
}

HERROR		HLIB_VECTOR_InsertAfter (HxVector_t *self, const void *sibling, void *data)
{
	HINT32	index;

	OS_assert(self != NULL);

	index = HLIB_VECTOR_IndexOf(self, sibling);
	if (index < 0)
		return ERR_FAIL;

	return _vector_Insert (self, index + 1, data);
}

HERROR		HLIB_VECTOR_InsertBefore (HxVector_t *self, const void *sibling, void *data)
{
	HINT32	index;

	OS_assert(self != NULL);

	index = HLIB_VECTOR_IndexOf(self, sibling);
	if (index < 0)
		return ERR_FAIL;

	return _vector_Insert (self, index, data);

}


HERROR		HLIB_VECTOR_Remove (HxVector_t *self, HINT32 n)
{
	void *	item;

	OS_assert(self != NULL);

	if (n < 0 || self->count <= n)
		return ERR_FAIL;

	item = HLIB_VECTOR_Detach(self, n);

	if (self->rmItem)
		self->rmItem(item);

	return ERR_OK;
}


HERROR		HLIB_VECTOR_RemoveAll (HxVector_t *self)
{
	HINT32	count;
	HERROR	err = ERR_OK;

	OS_assert(self != NULL);

	count = self->count;
	while (count--)
	{
		err |= HLIB_VECTOR_Remove(self, count);
	}
	return err;
}


void *		HLIB_VECTOR_Detach (HxVector_t *self, HINT32 n)
{
	HINT32	size;
	void *	item;

	OS_assert(self != NULL);

	if (n < 0 || self->count <= n)
		return NULL;

	self->count--;
	item = self->vector[n];

#if defined(OS_memmove)
	size = self->count - n;
	if (size > 0)
		OS_memmove(self->vector + n, self->vector + n + 1, sizeof(void *) * size);
#else
	for (size = n ; size < self->count ; size++)
	{
		self->vector[size] = self->vector[size + 1];
	}
#endif
	return item;
}


HINT32		HLIB_VECTOR_Length	(HxVector_t *self)
{
	OS_assert(self != NULL);
	return self->count;
}

HINT32		HLIB_VECTOR_MemLength	(HxVector_t *self)
{
	OS_assert(self != NULL);
	return self->size / sizeof(void *);
}

HERROR		HLIB_VECTOR_Reverse	(HxVector_t *self)
{
	HINT32 min;
	HINT32 max;
	void * temp;

	OS_assert(self != NULL);

	min = 0;
	max = self->count - 1;

	while (min < max)
	{
		temp = self->vector[min];
		self->vector[min] = self->vector[max];
		self->vector[max] = temp;

		min++;
		max--;
	}
	return ERR_OK;
}


HERROR		HLIB_VECTOR_Sort (HxVector_t *self, HINT32 (* cmp)(const void *, const void *))
{
	HINT32 (* cmpItem)(const void *, const void *);

	OS_assert(self != NULL);

	if (self->count <= 1)
		return ERR_OK;

	if (cmp == NULL)
	{
		cmpItem = self->cmpItem;
	}
	else
	{
		cmpItem = cmp;
	}

	qsort(self->vector, self->count, sizeof(void *), cmpItem);

	return ERR_OK;
}

HERROR		HLIB_VECTOR_SortEx (HxVector_t *self, HINT32 (* cmp)(const void *, const void *, void *), void *userdata)
{
	OS_assert(self && cmp);

	if (self->count <= 1)
		return ERR_OK;

	HLIB_STD_QuickSort(
			  (void *)self->vector
			, (HUINT32)self->count
			, (HUINT32)sizeof(void *)
			, cmp
			, userdata
		);
	return ERR_OK;
}


void *		HLIB_VECTOR_First	(HxVector_t *self)
{
	return HLIB_VECTOR_ItemAt(self, 0);
}

void *		HLIB_VECTOR_Last	(HxVector_t *self)
{
	return HLIB_VECTOR_ItemAt(self, self->count - 1);
}

void *		HLIB_VECTOR_ItemAt	(HxVector_t *self, HINT32 n)
{
	OS_assert(self != NULL);

	if (n < 0 || self->count <= n)
		return NULL;

	return self->vector[n];
}


void *		HLIB_VECTOR_Find (HxVector_t *self, HBOOL (* filter)(void *data, void *userdata), void *userdata)
{
	HINT32	i;

	i = HLIB_VECTOR_ForEach(self, filter, userdata);
	if (i < 0)
		return NULL;
	return HLIB_VECTOR_ItemAt(self, i);
}


HINT32		HLIB_VECTOR_RemoveIf (HxVector_t *self, HBOOL (* filter)(void *data, void *userdata), void *userdata)
{
	void	**remain;
	void	*item;
	HINT32	i, c;
	HINT32	removed;

	OS_assert(self != NULL);
	OS_assert(filter != NULL);

	if (self->count == 0)
		return 0;

#if defined(CONFIG_DEBUG)
	remain = (void **) DS_MEM_Malloc(sizeof(void *) * self->count, self, self->debugText, self->debugLine);
#else
	remain = (void **)OS_malloc(sizeof(void *) * self->count);
#endif
	if (remain == NULL)
		return 0;

	for (i = 0, c = 0, removed = 0 ; i < self->count ; i++)
	{
		item = self->vector[i];

		if (filter(item, userdata))
		{
			if (self->rmItem)
				self->rmItem(item);
			removed++;
		}
		else
		{
			remain[c++] = item;
		}
	}

	self->count = c;
	if (c > 0)
		memcpy(self->vector, remain, sizeof(void *) * self->count);

#if defined(CONFIG_DEBUG)
	DS_MEM_Free(remain,self,self->debugText,self->debugLine);
#else
	OS_free(remain);
#endif

	return removed;
}


void *		HLIB_VECTOR_Replace (HxVector_t *self, HINT32 n, void *data)
{
	void *old;

	OS_assert(self != NULL);

	if (n < 0 || self->count <= n)
		return NULL;

	old = self->vector[n];
	self->vector[n] = data;
	return old;
}

HINT32		HLIB_VECTOR_IndexOf	(HxVector_t *self, const void *data)
{
	HINT32	i;

	OS_assert(self != NULL);
	OS_assert(self->cmpItem != NULL);

	for (i = 0 ; i < self->count ; i++)
	{
		if (self->cmpItem(&data, self->vector + i) == 0)
			return i;
	}
	return -1;
}


HINT32		HLIB_VECTOR_BSearch (HxVector_t *self, const void *key, HINT32 (* cmp)(const void *, const void *))
{
	void 	* ret;
	HINT32 (* cmpItem)(const void *, const void *);

	OS_assert(self && key);

	if (self->count < 1)
		return -1;

	if (cmp == NULL)
	{
		cmpItem = self->cmpItem;
	}
	else
	{
		cmpItem = cmp;
	}

	ret = bsearch(&key, self->vector, self->count, sizeof(void *), cmpItem);
	if (ret == NULL)
		return -1;
	return (HINT32)(((HUINT32)ret - (HUINT32)self->vector) / sizeof(void *));
}


HINT32		HLIB_VECTOR_ForEach	(HxVector_t *self, HBOOL (* func)(void *data, void *userdata), void *userdata)
{
	HINT32	i;

	OS_assert(self != NULL);
	OS_assert(func != NULL);

	for (i = 0 ; i < self->count ; i++)
	{
		if (func(self->vector[i], userdata) == TRUE)
			break;
	}
	return i;
}


HxVector_t *HLIB_VECTOR_Clone (HxVector_t *self)
{
	HxVector_t	*clone;
	HINT32		i;

	OS_assert(self != NULL);

	clone = HLIB_VECTOR_NewEx(NULL
				, self->count == 0 ? HVEC_DEFAULT_LEN : self->count
				, self->mkItem
				, self->rmItem
				, self->cmpItem
			);
	if (clone == NULL)
		return NULL;

	for (i = 0 ; i < self->count ; i++)
	{
		(void) HLIB_VECTOR_Add(clone, HLIB_VECTOR_ItemAt(self, i));
	}
	return clone;
}

HxVector_t	*HLIB_VECTOR_CloneEx (HxVector_t *self, void *(* cloneItem)(void *))
{
	HxVector_t	*clone;
	HINT32		i;

	if (cloneItem == NULL)
		return HLIB_VECTOR_Clone(self);

	OS_assert(self != NULL);

	clone = HLIB_VECTOR_NewEx(NULL
				, self->count == 0 ? HVEC_DEFAULT_LEN : self->count
				, self->mkItem
				, self->rmItem
				, self->cmpItem
			);
	if (clone == NULL)
		return NULL;

	for (i = 0 ; i < self->count ; i++)
	{
		(void) HLIB_VECTOR_Add(clone, cloneItem(HLIB_VECTOR_ItemAt(self, i)));
	}
	return clone;
}


HUINT8 *	HLIB_VECTOR_ToString (HxVector_t *self, HUINT8 *str, HUINT32 n)
{
	OS_assert(self != NULL);
	OS_assert(str != NULL && n > 0);

	OS_snprintf((char *)str, n,
			"HVector::size(%dB) max(%d), count(%d), allocSelf(%s)",
			self->size, self->size / sizeof(void *), self->count, self->allocSelf ? "TRUE" : "FALSE"
		);
	return str;
}

