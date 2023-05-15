/*
 * hxstack.c
 *
 *  Created on: 2011. 1. 25.
 */

#define	HxLOG_DOMAIN	"hxstack"
#include <_hlib_log.h>
#include <_hlib_stack.h>


#define	OS_assert	HxLOG_Assert
#define	OS_snprintf	snprintf


#define	____HxSTACK_APIs____________________________________________________

void *	HLIB_STACK_Pop(HxStack_t *self)
{
	HINT32	length;
	void *	top;

	length = HLIB_VECTOR_Length((HxVector_t *)self);
	if (length <= 0)
	{
		return NULL;
	}
	length--;

	top = HLIB_VECTOR_Detach((HxVector_t *) self, length);
	return top;
}

HUINT8 *	HLIB_STACK_ToString (HxStack_t *self, HUINT8 *str, HUINT32 n)
{
	OS_assert(self != NULL);
	OS_assert(str != NULL && n > 0);

	OS_snprintf((char *)str, n,
			"HxStack::size(%dB) max(%d), count(%d), allocSelf(%s)",
			self->size, self->size / sizeof(void *), self->count, self->allocSelf ? "TRUE" : "FALSE"
		);
	return str;
}


