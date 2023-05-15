/*
 * hxstack.c
 *
 *  Created on: 2011. 1. 25.
 */

#define	HxLOG_DOMAIN	"hxringqueue"
#include <_hlib_log.h>
#include <hlib.h>


#define	OS_assert	HxLOG_Assert
#define	OS_snprintf	snprintf


#define	____HxSTACK_APIs____________________________________________________
HERROR	HLIB_RINGQUEUE_Put(HxRingQueue_t *self, void *data)
{
	HINT32 nMax = 0;
	HINT32 nLength = 0;
	HERROR hErr = ERR_OK;

	nMax = HLIB_VECTOR_MemLength(self);
	HxLOG_Assert(nMax);

	nLength = HLIB_VECTOR_Length(self);

	if(nMax > nLength)
	{
		return HLIB_VECTOR_Add(self, data);
	}

	hErr = HLIB_VECTOR_Remove(self, 0);
	if(hErr != ERR_OK)
	{
		return hErr;
	}

	return HLIB_VECTOR_Add(self, data);
}

void *	HLIB_RINGQUEUE_Get(HxRingQueue_t *self)
{
	return HLIB_VECTOR_Detach((HxVector_t *) self, 0);
}

HUINT8 *	HLIB_RINGQUEUE_ToString (HxRingQueue_t *self, HUINT8 *str, HUINT32 n)
{
	OS_assert(self != NULL);
	OS_assert(str != NULL && n > 0);

	OS_snprintf((char *)str, n,
			"HxRingQueue::size(%dB) max(%d), count(%d), allocSelf(%s)",
			self->size, self->size / sizeof(void *), self->count, self->allocSelf ? "TRUE" : "FALSE"
		);
	return str;
}


