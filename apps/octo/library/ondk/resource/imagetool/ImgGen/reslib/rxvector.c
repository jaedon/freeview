/*
 * rxvector.c
 *
 *  Created on: 2011. 1. 12.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define	RxLOG_DOMAIN	"rxvector"

#include	<rxstdlib.h>
#include 	"rxvector.h"

#define	OS_malloc	RxSTD_Alloc
#define	OS_realloc	RxSTD_Realloc
#define	OS_free		RxSTD_Free
#define	OS_memset	memset
#define	OS_memcpy	memcpy

#define	OS_memmove	memmove
#define	OS_assert
#ifdef WIN32
#define	OS_snprintf	_snprintf
#else
#define	OS_snprintf	snprintf
#endif
#define	HVEC_DEFAULT_LEN	10


#define	____RxVECTOR_PRIVATE_FUNCTIONS______________________________________________

static HINT32	_rxvector_DefaultCmp (const void *arg1, const void *arg2)
{
	return *(HINT32 *)arg1 - *(HINT32 *)arg2;
}


static HBOOL	_rxvector_IsFull  (RxVector_t *self)
{
	HINT32 used = self->count * sizeof(void *);

	if (used >= self->size)
	{
		return TRUE;
	}
	return FALSE;
}

static HERROR	_rxvector_Realloc (RxVector_t *self, HUINT32 size)
{
	void *	mem;

/*
	mem  = OS_realloc((void *)self->rxvector, size);
	if (mem)
	{
		self->size = size;
		return ERR_OK;
	}
*/
	mem  = OS_malloc(size);
	if (mem == NULL)
	{
		OS_assert(mem != NULL);
		return ERR_FAIL;
	}

	if (self->count > 0)
	{
		OS_memcpy(mem, self->rxvector, sizeof(void *) * self->count);
	}
	OS_free(self->rxvector);

	self->rxvector = (void **)mem;
	self->size   = size;

	return ERR_OK;
}


static HERROR	_rxvector_Insert (RxVector_t *self, HINT32 n, void *data)
{
	HERROR	err;
	HINT32	size, retry = 5;
	void *	item;

	if (n < 0 || self->count < n)
		return ERR_FAIL;

	while (retry-- && _rxvector_IsFull(self))
	{
		err = _rxvector_Realloc(self, self->size * 2);
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
		OS_memmove(self->rxvector + n + 1, self->rxvector + n, sizeof(void *) * size);
	}
	self->rxvector[n] = item;
#else
	for (size = self->count ; size > n ; size--)
	{
		self->rxvector[size] = self->rxvector[size - 1];
	}
	self->rxvector[size] = item;
#endif
	self->count++;

	return ERR_OK;
}

/////////////////////////////////////////////////////////////////////////////

#define	____RxVECTOR_CompareAPIs____________________________________________________

HINT32	RxVector_CompareHINT8	(const void *arg1, const void *arg2)
{
	return (HINT32)(*(HINT8 *)arg1) - (HINT32)(*(HINT8 *)arg2);
}

HINT32	RxVector_CompareHINT16	(const void *arg1, const void *arg2)
{
	return (HINT32)(*(HINT16 *)arg1) - (HINT32)(*(HINT16 *)arg2);
}

HINT32	RxVector_CompareHINT32	(const void *arg1, const void *arg2)
{
	return *(HINT32 *)arg1 - *(HINT32 *)arg2;
}

HINT32	RxVector_CompareHUINT8	(const void *arg1, const void *arg2)
{
	return (HINT32)(*(HUINT8 *)arg1) - (HINT32)(*(HUINT8 *)arg2);
}

HINT32	RxVector_CompareHUINT16	(const void *arg1, const void *arg2)
{
	return (HINT32)(*(HUINT16 *)arg1) - (HINT32)(*(HUINT16 *)arg2);
}

HINT32	RxVector_CompareHUINT32	(const void *arg1, const void *arg2)
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

HINT32	RxVector_CompareSzASCII	(const void *arg1, const void *arg2)
{
	return (HINT32)strcmp(*(char **)arg1, *(char **)arg2);
}

#define	____RxVECTOR_APIs____________________________________________________


RxVector_t *	RxVector_New (RxVector_t *self, HINT32 (* cmp)(const void *, const void *))
{
	return RxVector_NewEx(self, HVEC_DEFAULT_LEN, NULL, NULL, cmp);
}


RxVector_t * RxVector_NewEx (RxVector_t *self, HINT32 length, void * (*mk)(void *), void * (*rm)(void *), HINT32 (* cmp)(const void *, const void *))
{
	RxVector_t *thiz = self;

	if (thiz == NULL)
	{
		thiz = (RxVector_t *) OS_malloc (sizeof(RxVector_t));
		if (thiz == NULL)
		{
			return NULL;
		}
		thiz->allocSelf = thiz;
	}
	else
		thiz->allocSelf = NULL;

	thiz->size = length * sizeof(void *);
	thiz->count  = 0;
	thiz->mkItem = mk;
	thiz->rmItem = rm;
	thiz->cmpItem = cmp;
	if (thiz->cmpItem == NULL)
		thiz->cmpItem = _rxvector_DefaultCmp;

	thiz->rxvector = (void **) OS_malloc (thiz->size);
	if (thiz->rxvector == NULL)
	{
		if (thiz->allocSelf)
			OS_free(thiz);
		return NULL;
	}
	return thiz;
}


void		RxVector_Delete (RxVector_t *self)
{
	OS_assert(self != NULL);

	(void) RxVector_RemoveAll (self);

	if (self->rxvector)
	{
		OS_free(self->rxvector);
		self->rxvector = NULL;
	}

	if (self->allocSelf)
		OS_free(self->allocSelf);
}

HERROR		RxVector_Add (RxVector_t *self, void *data)
{
	OS_assert(self != NULL);

	return _rxvector_Insert(self, self->count, data);
}


HERROR		RxVector_Insert (RxVector_t *self, HINT32 n, void *data)
{
	OS_assert(self != NULL);

	return _rxvector_Insert(self, n, data);
}

HERROR		RxVector_InsertAfter (RxVector_t *self, const void *sibling, void *data)
{
	HINT32	index;

	OS_assert(self != NULL);

	index = RxVector_IndexOf(self, sibling);
	if (index < 0)
		return ERR_FAIL;

	return _rxvector_Insert (self, index + 1, data);
}

HERROR		RxVector_InsertBefore (RxVector_t *self, const void *sibling, void *data)
{
	HINT32	index;

	OS_assert(self != NULL);

	index = RxVector_IndexOf(self, sibling);
	if (index < 0)
		return ERR_FAIL;

	return _rxvector_Insert (self, index, data);

}


HERROR		RxVector_Remove (RxVector_t *self, HINT32 n)
{
	void *	item;

	OS_assert(self != NULL);

	if (n < 0 || self->count <= n)
		return ERR_FAIL;

	item = RxVector_Detach(self, n);

	if (self->rmItem)
		self->rmItem(item);

	return ERR_OK;
}


HERROR		RxVector_RemoveAll (RxVector_t *self)
{
	HINT32	count;
	HERROR	err = ERR_OK;

	OS_assert(self != NULL);

	count = self->count;
	while (count--)
	{
		err |= RxVector_Remove(self, count);
	}
	return err;
}


void *		RxVector_Detach (RxVector_t *self, HINT32 n)
{
	HINT32	size;
	void *	item;

	OS_assert(self != NULL);

	if (n < 0 || self->count <= n)
		return NULL;

	self->count--;
	item = self->rxvector[n];

#if defined(OS_memmove)
	size = self->count - n;
	if (size > 0)
		OS_memmove(self->rxvector + n, self->rxvector + n + 1, sizeof(void *) * size);
#else
	for (size = n ; size < self->count ; size++)
	{
		self->rxvector[size] = self->rxvector[size + 1];
	}
#endif
	return item;
}


HINT32		RxVector_Length	(RxVector_t *self)
{
	OS_assert(self != NULL);
	return self->count;
}

HINT32		RxVector_MemLength	(RxVector_t *self)
{
	OS_assert(self != NULL);
	return self->size / sizeof(void *);
}

HERROR		RxVector_Reverse	(RxVector_t *self)
{
	HINT32 min;
	HINT32 max;
	void * temp;

	OS_assert(self != NULL);

	min = 0;
	max = self->count - 1;

	while (min < max)
	{
		temp = self->rxvector[min];
		self->rxvector[min] = self->rxvector[max];
		self->rxvector[max] = temp;

		min++;
		max--;
	}
	return ERR_OK;
}


HERROR		RxVector_Sort (RxVector_t *self, HINT32 (* cmp)(const void *, const void *))
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

	qsort(self->rxvector, self->count, sizeof(void *), cmpItem);

	return ERR_OK;
}


void *		RxVector_First	(RxVector_t *self)
{
	return RxVector_ItemAt(self, 0);
}

void *		RxVector_Last	(RxVector_t *self)
{
	return RxVector_ItemAt(self, self->count - 1);
}

void *		RxVector_ItemAt	(RxVector_t *self, HINT32 n)
{
	OS_assert(self != NULL);

	if (n < 0 || self->count <= n)
		return NULL;

	return self->rxvector[n];
}

void *		RxVector_Replace (RxVector_t *self, HINT32 n, void *data)
{
	void *old;

	OS_assert(self != NULL);

	if (n < 0 || self->count <= n)
		return NULL;

	old = self->rxvector[n];
	self->rxvector[n] = data;
	return old;
}

HINT32		RxVector_IndexOf	(RxVector_t *self, const void *data)
{
	HINT32	i;

	OS_assert(self != NULL);
	OS_assert(self->cmpItem != NULL);

	for (i = 0 ; i < self->count ; i++)
	{
		if (self->cmpItem(&data, self->rxvector + i) == 0)
			return i;
	}
	return -1;
}


HINT32		RxVector_BSearch (RxVector_t *self, const void *key, HINT32 (* cmp)(const void *, const void *))
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

	ret = bsearch(&key, self->rxvector, self->count, sizeof(void *), cmpItem);
	if (ret == NULL)
		return -1;
	return (HINT32)(((HUINT32)ret - (HUINT32)self->rxvector) / sizeof(void *));
}


HINT32		RxVector_ForEach	(RxVector_t *self, HBOOL (* func)(void *data, void *userdata), void *userdata)
{
	HINT32	i;

	OS_assert(self != NULL);
	OS_assert(func != NULL);

	for (i = 0 ; i < self->count ; i++)
	{
		if (func(self->rxvector[i], userdata) == TRUE)
			break;
	}
	return i;
}

HUINT8 *	RxVector_ToString (RxVector_t *self, HUINT8 *str, HUINT32 n)
{
	OS_assert(self != NULL);
	OS_assert(str != NULL && n > 0);

	OS_snprintf((char *)str, n,
			"HVector::size(%dB) max(%d), count(%d), allocSelf(%s)",
			self->size, self->size / sizeof(void *), self->count, self->allocSelf ? "TRUE" : "FALSE"
		);
	return str;
}

