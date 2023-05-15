/*
 * hcollection.c
 *
 *  Created on: 2011. 2. 28.
 */


#include <vkernel.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define	HxLOG_DOMAIN	"HxCollection"
#include <_hlib_log.h>

#include "_hlib_collection.h"

#define	OS_malloc	HLIB_MEM_Malloc
#define	OS_free		HLIB_MEM_Free
#define	OS_assert	HxLOG_Assert
#define	OS_snprintf	snprintf


#define	____HxCOLLECTION_PRIVATE_FUNCTIONS______________________________________________

#define	____HxCOLLECTION_APIs____________________________________________________

HxCOLLECTION_t	*HLIB_COLLECTION_NewEx  (HxCOLLECTION_t *self, HINT32 length, void * (*mk)(void *), void * (*rm)(void *), HINT32 (* cmp)(const void *, const void *))
{
	HxCOLLECTION_t	*thiz;

	if (self == NULL)
	{
		self = (HxCOLLECTION_t *) OS_malloc (sizeof(HxCOLLECTION_t));
		if (self == NULL)
		{
			return NULL;
		}
		self->allocSelf = self;
	}
	else
		self->allocSelf = NULL;

	thiz = (HxCOLLECTION_t *)HLIB_VECTOR_NewEx((HxVector_t *)self, length, mk, rm, cmp);
	if (thiz == NULL)
	{
		if (self->allocSelf)
			OS_free(self->allocSelf);
		return NULL;
	}
	thiz->defIndex = 0;

	return thiz;
}

void		HLIB_COLLECTION_Delete (HxCOLLECTION_t *self)
{
	OS_assert(self);

	HLIB_VECTOR_Delete((HxVector_t *)self);

	if (self->allocSelf)
		OS_free(self->allocSelf);
}

HINT32		HLIB_COLLECTION_SetDefault (HxCOLLECTION_t *self, HINT32 index)
{
	OS_assert(self);

	if (index < 0)
		return -1;

	if (HLIB_COLLECTION_Length(self) <= index)
		return -1;

	self->defIndex	= index;

	return index;
}

void *		HLIB_COLLECTION_GetItem	(HxCOLLECTION_t *self, HINT32 n)
{
	void *item;

	OS_assert(self);

	item = HLIB_VECTOR_ItemAt((HxVector_t *)self, n);
	if (item == NULL)
	{
		item = HLIB_VECTOR_ItemAt((HxVector_t *)self, self->defIndex);
	}
	return item;
}

HUINT8 *	HLIB_COLLECTION_ToString (HxCOLLECTION_t *self, HUINT8 *str, HUINT32 n)
{
	HxVector_t	*super = (HxVector_t *)self;

	OS_assert(self != NULL);
	OS_assert(str != NULL && n > 0);

	OS_snprintf((char *)str, n,
			"HxCollection::size(%dB) max(%d), count(%d), allocSelf(%s), defaultIdx(%d)",
			super->size, super->size / sizeof(void *), super->count, super->allocSelf ? "TRUE" : "FALSE", self->defIndex
		);
	return str;
}

