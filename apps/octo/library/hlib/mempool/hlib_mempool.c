

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <stdarg.h>

#include <_hlib_log.h>
#include <_hlib_stdlib.h>
#include <_hlib_mem.h>

#undef	HxLOG_DOMAIN
#define	HxLOG_DOMAIN	"MEMPOOL"

#include "_hlib_mempool.h"

// TODO: temporary...
#define	MEMPOOL_ASSERT		HxLOG_Assert


#ifndef	MEMPOOL_MALLOC
#define	MEMPOOL_MALLOC		HLIB_MEM_Malloc
#endif

#ifndef	MEMPOOL_FREE
#define	MEMPOOL_FREE		HLIB_MEM_Free
#endif

#ifndef	MEMPOOL_ASSERT
#define	MEMPOOL_ASSERT		assert
#endif


#define	_____mempool_type______________________________________________________________

struct mempool
{
	void			*buf;		///< memory buffer
	size_t			bytes;		///< buffer size in bytes

	size_t			count;		///< block counts
	size_t			block;		///< block size
	size_t			allocated;	///< allocated

	unsigned char	*pool;		///< pool start address
	size_t			offset;		///< current pool offset

	void			*freelist;	///< free list

	void *			(*_malloc)(size_t);	///< malloc
	void			(*_free) (void *);	///< free

	void			*garbage;
};

#define	MEMPOOL_MINIMUMSIZE(block)	(sizeof(struct mempool) + (block) + sizeof(void *))

#define	_____mempool_private______________________________________________________________


static int		__mempool_is_valid_address (struct mempool *self, const void *address)
{
	if ((size_t)address < (size_t)self->buf || (size_t)self->buf + self->bytes < (size_t)address)
		return 0;
	return 1;
}

static void *	__mempool_allocblock (struct mempool *self)
{
	unsigned char *block;
	unsigned int  *item;

	MEMPOOL_ASSERT(self);

	if (self->count <= self->allocated)
		return NULL;	// memory underflow

	if (self->freelist)
	{
		item  = (unsigned int *)self->freelist;
		self->freelist = (void *)*item;
		block = (unsigned char *)(++item);
	}
	else
	{
		item  = (unsigned int *)((unsigned char *)self->pool + self->offset);
		self->offset  += sizeof(void *) + self->block;
		block = (unsigned char *)(++item);
	}
	self->allocated++;

	return block;
}


static void *	__mempool_freeblock	(struct mempool *self, void *data)
{
	unsigned int  *block;

	MEMPOOL_ASSERT(self);

	// check invalid address
	if (!__mempool_is_valid_address(self, data))
		return data;

	if (self->allocated == 0)
		return data;

	block    = (unsigned int *)data;
	*--block = (unsigned int)self->freelist;
	self->freelist = (void *)block;

	self->allocated--;

	return NULL;
}


#define	_____mempool_public______________________________________________________________


struct mempool *	mempool_newex (void *buf, size_t bytes, size_t blocksize, void *(*_malloc)(size_t), void (*_free)(void *))
{
	struct mempool	*self;
	void			*garbage = NULL;

	MEMPOOL_ASSERT(blocksize);
	MEMPOOL_ASSERT((_malloc && _free) || (_malloc == NULL && _free == NULL));

	if (bytes < MEMPOOL_MINIMUMSIZE(blocksize))	// invalid argument
		return NULL;

	if (buf == NULL)
	{
		if (_malloc)
			buf = _malloc(bytes);
		else
			buf = MEMPOOL_MALLOC(bytes);
		if (buf == NULL)
			return NULL;
		garbage = buf;
	}

	self = (struct mempool *)buf;
	memset(self, 0, sizeof(struct mempool));

	self->buf    = buf;
	self->bytes  = bytes;
	self->count  = (bytes - sizeof(struct mempool)) / (blocksize + sizeof(void *));
	self->block  = blocksize;
	self->pool   = (unsigned char *)buf + sizeof(struct mempool);
	self->garbage = garbage;
	self->_malloc = _malloc;
	self->_free   = _free;

	return self;
}


struct mempool *	mempool_new (void *buf, size_t bytes, size_t blocksize)
{
	return mempool_newex(buf, bytes, blocksize, NULL, NULL);
}


void	mempool_delete (struct mempool *self)
{
	if (self && self->garbage)
	{
		if (self->_free)
			self->_free(self->garbage);
		else
			MEMPOOL_FREE(self->garbage);
	}
}


void	mempool_clear (struct mempool *self)
{
	MEMPOOL_ASSERT(self);

	(void)mempool_new(self->buf, self->bytes, self->block);
}


void *	mempool_alloc (struct mempool *self)
{
	void *block;

	MEMPOOL_ASSERT(self);

	block = __mempool_allocblock(self);
	if (block == NULL && self->_malloc)
	{
		block = self->_malloc(self->block);
	}
	return block;
}


void *	mempool_calloc (struct mempool *self)
{
	void *block;

	block = mempool_alloc(self);
	if (block)
	{
		memset(block, 0, self->block);
	}
	return block;
}


void *	mempool_free	(struct mempool *self, void *data)
{
	void *block;

	MEMPOOL_ASSERT(self);

	block = __mempool_freeblock(self, data);
	if (block && self->_free)
	{
		self->_free(block);
		block = NULL;
	}
	return block;
}

int		mempool_is_allocated (struct mempool *self, const void *addr)
{
	MEMPOOL_ASSERT(self);

	return __mempool_is_valid_address(self, addr);
}


#define	_____HxMEMPOOL_PORT______________________________________________________________

HUINT32	HLIB_MEMPOOL_ComputeBytes (HUINT32 blocksize, HUINT32 blockcount)
{
	return sizeof(struct mempool) + (blocksize * blockcount);
}


HINT32	HLIB_MEMPOOL_New (void *buf, HUINT32 bytes, HUINT32 blocksize)
{
	HxLOG_Trace();

	return (HINT32)mempool_new(buf, (size_t)bytes, (size_t)blocksize);
}


HINT32	HLIB_MEMPOOL_NewEx (void *buf, HUINT32 bytes, HUINT32 blocksize, void * (*_malloc)(HUINT32), void (*_free)(void *))
{
	HxLOG_Trace();

	return (HINT32)mempool_newex(buf, (size_t)bytes, (size_t)blocksize, _malloc, _free);
}

void	HLIB_MEMPOOL_Delete (HINT32 md)
{
	HxLOG_Trace();

	mempool_delete((struct mempool *)md);
}

void	HLIB_MEMPOOL_Clear (HINT32 md)
{
	HxLOG_Trace();

	mempool_clear((struct mempool *)md);
}

void *	HLIB_MEMPOOL_Alloc (HINT32 md)
{
	HxLOG_Trace();

	return mempool_alloc((struct mempool *)md);
}

void *	HLIB_MEMPOOL_Calloc (HINT32 md)
{
	HxLOG_Trace();

	return mempool_calloc((struct mempool *)md);
}

void *	HLIB_MEMPOOL_Free  (HINT32 md, void *data)
{
	HxLOG_Trace();

	return mempool_free((struct mempool *)md, data);
}

HBOOL	HLIB_MEMPOOL_IsAllocated (HINT32 md, const void *address)
{
	HxLOG_Trace();

	return mempool_is_allocated((struct mempool *)md, address) ? TRUE : FALSE;
}

