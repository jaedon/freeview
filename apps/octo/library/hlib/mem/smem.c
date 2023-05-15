//	Make new memory pool on each thread
//	We don't need semaphore for accessing memory pool

/* *************************************************************************************************** */
#define ___HEADER________________________________________________________________________________________
/* *************************************************************************************************** */
#include <hlib.h>

#include <stdio.h>

#include <vkernel.h>

/* *************************************************************************************************** */
#define ___GLOBAL_HEADER_________________________________________________________________________________
/* *************************************************************************************************** */



/* *************************************************************************************************** */
#define ___DEFINE________________________________________________________________________________________
/* *************************************************************************************************** */
#define		OS_MEM_Alloc			malloc
#define		OS_MEM_Free				free
#define		OS_MEM_Memset			VK_MEM_Memset
#define		OS_MEM_Memcpy			VK_MEM_Memcpy

#define		DEFAULT_SEGMENT_SIZE			0x40000		//	default 256KB
#define		DEFAULT_LARGEST_SEGMENT_SIZE	0x100000	//	default 1MB
#define		ACCEPTED_BLOCK_NUM			5
#define		ACCEPTED_ALL_BLOCK			0xFFFFFFFF

static	unsigned int		__accepted_blocksize[ACCEPTED_BLOCK_NUM] =
{
	0x00400,					//	1KB
	0x01000,					//	4KB
	0x04000,					//	16KB
	0x20000,					//	128KB
	ACCEPTED_ALL_BLOCK,			//	All memory
};

#define	MIN_SIZE	32

#ifdef	SMEM_DEBUG
	#define	ALIGNED_BYTE	32
#else
	#define	ALIGNED_BYTE	16
#endif
/* *************************************************************************************************** */
#define ___TYPEDEF_______________________________________________________________________________________
/* *************************************************************************************************** */

typedef union	__header_u		_header_u;
typedef struct	__mempool_t		_mempool_t;
typedef	struct	__memitem_t		_memitem_t;

union __header_u
{
#ifdef	SMEM_DEBUG
	struct
	{
		_header_u		*ptr;
		long			size;
		void			*memitem;
		void			*mempool;

		char			*function;
		int				line;
		int				magicnumber;
		int				reserved;
	} s;
	char	align[32];
#else
	struct
	{
		_header_u		*ptr;
		long			size;
		void			*memitem;
		void			*mempool;
	} s;
	char	align[16];
#endif
};

typedef	struct
{
	void	*buffer;

	_header_u	base;
	_header_u	*free;
} _membuffer_t;

struct __memitem_t
{
	unsigned int	acceptmask;				//	under XXX bytes
	unsigned int	fullsize;
	_membuffer_t	membuffer;

	_mempool_t		*mempool;
	_memitem_t		*next;
};

struct	__mempool_t
{
	unsigned int	threadid;

	_memitem_t		*prevUse;				//	cache for mem pool

	_memitem_t		*memitem;
	_mempool_t		*next;
};

/* *************************************************************************************************** */
#define ___STATIC_VARIABLES______________________________________________________________________________
/* *************************************************************************************************** */
static	unsigned int	s_ulSMemSemId = 0;
static	_mempool_t		*s_memitemlist = NULL;

/* *************************************************************************************************** */
#define ___Private_FUNCTION______________________________________________________________________________
/* *************************************************************************************************** */
static	void		_free_mem(void *ptr);

static	void		_print_memusage(_mempool_t *pool)
{
	int		i = 0;
	_memitem_t		*memitem;
	_membuffer_t	*buf;

	HxLOG_Print("thread id : [0x%08X]\n", pool->threadid);
	memitem = pool->memitem;
	while (memitem)
	{
		_header_u	*p, *freep;
		int			n, avail, maxblock;

		buf = &memitem->membuffer;
		HxLOG_Print("\tpool [%d] - accepted [%d] allocated [%dKB]\n", i, memitem->acceptmask, memitem->fullsize / 1024);

		freep = buf->free;

		p = freep;
		n = avail = maxblock = 0;

		do
		{
			n++;
			avail += p->s.size;
			if (maxblock < p->s.size)
				maxblock = p->s.size;
			p = p->s.ptr;
		} while (p != freep);

		avail = avail * sizeof(_header_u) / 1024;
		maxblock = maxblock * sizeof(_header_u) / 1024;

		HxLOG_Print("\t== total %d block free, available = %dKB, largest = %dKB\n", n, avail, maxblock);

		memitem = memitem->next;
	}
}

static	_mempool_t*		_find_memory_pool(unsigned int threadno)
{
	_mempool_t		*list = s_memitemlist;

	while (list)
	{
		if (list->threadid == threadno)
			return list;

		list = list->next;
	}

	return NULL;
}

static	_mempool_t*		_create_memory_pool(unsigned int threadno)
{
	_mempool_t		*newPool;

	newPool = OS_MEM_Alloc(sizeof(_mempool_t));
	OS_MEM_Memset(newPool, 0, sizeof(_mempool_t));

	newPool->threadid = threadno;
	if (s_memitemlist == NULL)
		s_memitemlist = newPool;
	else
	{
		_mempool_t	*list = s_memitemlist;

		while (list->next)
			list = list->next;

		list->next = newPool;
	}

#if	0
	printf("memory pool list ...\n");
	{
		_mempool_t	*_list = s_memitemlist;
		while (_list)
		{
			printf("\t threadid [%X]\n", _list->threadid);
			_list = _list->next;
		}
	}
#endif

	return newPool;
}

static	_memitem_t*		_create_memitem(_mempool_t *pool, int size)
{
	int		i, allocSize;

	_memitem_t		*newItem;

	HxLOG_Print("create mem item : size [%d]\n", size);

	newItem = OS_MEM_Alloc(sizeof(_memitem_t));

	for (i = 0; i < ACCEPTED_BLOCK_NUM; i++)
	{
		if (__accepted_blocksize[i] > size)
			break;
	}

	newItem->acceptmask = __accepted_blocksize[i == ACCEPTED_BLOCK_NUM ? --i : i];
	newItem->mempool = pool;
	newItem->next = NULL;

	HxLOG_Print("create mem item : accepted mask : [%d]\n", newItem->acceptmask);
	if (newItem->acceptmask == ACCEPTED_ALL_BLOCK)
	{
		if (size < DEFAULT_LARGEST_SEGMENT_SIZE)
		{
			allocSize = DEFAULT_LARGEST_SEGMENT_SIZE;
			newItem->acceptmask = DEFAULT_LARGEST_SEGMENT_SIZE;
		}
		else
		{
			allocSize = size + sizeof(_header_u) * 2;
			newItem->acceptmask = allocSize;
		}
	} else
		allocSize = DEFAULT_SEGMENT_SIZE;

	HxLOG_Print("new alloc size : [%d]\n", allocSize);

	if (pool->memitem == NULL)
		pool->memitem = newItem;
	else
	{
		_memitem_t	*pItem;

		pItem = pool->memitem;
		while (pItem->next)
			pItem = pItem->next;

		pItem->next = newItem;
	}
	pool->prevUse = newItem;

	newItem->membuffer.buffer = OS_MEM_Alloc(allocSize);
	newItem->fullsize = allocSize;
	{
		_header_u	*bp;

		newItem->membuffer.base.s.ptr = newItem->membuffer.free = &newItem->membuffer.base;
		newItem->membuffer.base.s.size = 0;

		bp = (_header_u*)newItem->membuffer.buffer;
		bp->s.size = (allocSize - sizeof(_header_u)) / sizeof(_header_u);
		bp->s.memitem = newItem;
		bp->s.mempool = pool;
#ifdef	SMEM_DEBUG
		bp->s.magicnumber = 0x12349876;
		bp->s.function = "Create Pool";
		bp->s.line = __LINE__;
#endif

		_free_mem((void*)(bp + 1));
	}

	//	TODO: Sort Memory Item with BlockSize.
	_print_memusage(pool);

	return newItem;
}

static	void*			_alloc_mem_item(_memitem_t *memitem, int size)
{
	int		nunits;

	_membuffer_t	*membuffer;
	_header_u		*p, *prevp, *freep;

	//printf("alloc_mem_item : memitem size : [%d]\n", memitem->fullsize);

	membuffer = &memitem->membuffer;
	freep = membuffer->free;
	nunits = (size + sizeof(_header_u) - 1) / sizeof(_header_u) + 1;
	prevp = freep;

	for (p = prevp->s.ptr; ; prevp = p, p = p->s.ptr)
	{
		if (p->s.size >= nunits)
		{
			if ((p->s.size - nunits) * sizeof(_header_u) < MIN_SIZE)
			{
				if (p->s.ptr == p)
				{
					return NULL;
				}
				prevp->s.ptr = p->s.ptr;
			}
			else
			{
				p->s.size -= nunits;
				p += p->s.size;
				p->s.size = nunits;
			}
			freep = prevp;

			membuffer->free = freep;
			p->s.memitem = memitem;
			p->s.mempool = memitem->mempool;
#ifdef	SMEM_DEBUG
			p->s.magicnumber = 0x12349876;
			p->s.function = "??";
			p->s.line = 0;
#endif

			return (void *)(p+1);
		}

		if (p == freep)
		{
			return NULL;
		}
	}

	return NULL;
}

static	void*			_alloc_mem(_mempool_t* pool, int size)
{
	void	*p = NULL;

	_memitem_t		*memitem, *memitem_;

	if (size < 0)
		return NULL;

	memitem = pool->prevUse;
	if (memitem == NULL)
		memitem = _create_memitem(pool, size);

	if (memitem->acceptmask > size)
		p = _alloc_mem_item(memitem, size);

	//printf("p : [%X] memitem accepted : [%d]\n", p, memitem->acceptmask);

	if (p == NULL)
	{
		memitem_ = pool->memitem;
		while (memitem_)
		{
			if (memitem_ != memitem)
			{
				if (memitem_->acceptmask > size)
				{
					p = _alloc_mem_item(memitem_, size);
					if (p)
					{
						HxLOG_Print("in loop p : [%X] memitem accepted : [%d]\n", (unsigned int)p, memitem->acceptmask);
						pool->prevUse = memitem_;
						break;
					}
				}
			}

			memitem_ = memitem_->next;
		}
	}

	if (p == NULL)
	{
		HxLOG_Print("NEW MEM ITEM POOL NEED!!!\n");
		memitem = _create_memitem(pool, size + size / 2);
		p = _alloc_mem_item(memitem, size);
		pool->prevUse = memitem;
	}

	//printf("alloc mem in (0x%08x) : size[%d] ptr : [0x%08x]\n", pool->threadid, size, p);

	return p;
}

static	void		_free_mem(void *ptr)
{
	_memitem_t		*memitem;
	_mempool_t		*mempool;
	_membuffer_t	*membuffer;

	_header_u		*bp, *p, *freep;

	bp = (_header_u *)ptr - 1;

	memitem = (_memitem_t*)bp->s.memitem;
	mempool = (_mempool_t*)bp->s.mempool;

	//printf("free mem in (0x%08x) : ptr [0x%08x]\n", mempool->threadid, ptr);
#ifdef	SMEM_DEBUG
	if (bp->s.magicnumber != 0x12349876)
	{
		HxLOG_Print("\n");
		HxLOG_Print("==========================================================\n");
		HxLOG_Print(" ERROR : ptr is allocated by common function..\n");
		free(ptr);
		HxLOG_Print("==========================================================\n");
		HxLOG_Print("\n");
		return;
	}
#endif
	if (mempool->threadid != pthread_self())
	{
		HxLOG_Print("\n");
		HxLOG_Print("==========================================================\n");
		HxLOG_Print(" ERROR : free called by other thread id!!!!\n");
#ifdef	SMEM_DEBUG
		HxLOG_Print(" Alloc in [%s] (%d)\n", bp->s.function, bp->s.line);
#endif
		HxLOG_Print("==========================================================\n");
		HxLOG_Print("\n");
	}

	membuffer = &memitem->membuffer;
	freep = membuffer->free;

	for (p = freep; ; p = p->s.ptr)
	{
		if (p <= bp && p+p->s.size > bp)
		{
			HxLOG_Print("error : already freed memory...\n");
			_print_memusage(mempool);
			return;
		}

		if (bp > p && bp < p->s.ptr) break;

		if (p >= p->s.ptr && bp > p) break;
	}

	if (bp + bp->s.size == p->s.ptr) {
		bp->s.size += p->s.ptr->s.size;
		bp->s.ptr = p->s.ptr->s.ptr;
	}
	else
		bp->s.ptr = p->s.ptr;

	if (p + p->s.size == bp) {
		p->s.size += bp->s.size;
		p->s.ptr = bp->s.ptr;
	}
	else
		p->s.ptr = bp;

	membuffer->free = p;

	return;
}

#ifdef	SMEM_DEBUG
static	void	_add_dbginfo(void *ptr, char *function, int line)
{
	_header_u		*bp;

	bp = (_header_u *)ptr - 1;
	bp->s.function = function;
	bp->s.line = line;
	bp->s.magicnumber = 0x12349876;
}
#endif

static	void*	smem_alloc(int size)
{
	unsigned int	threadno;
	_mempool_t		*foundpool;

	threadno = (unsigned int)pthread_self();

	foundpool = _find_memory_pool(threadno);
	if (foundpool == NULL)
	{
		VK_SEM_Get(s_ulSMemSemId);
		foundpool = _find_memory_pool(threadno);
		if (foundpool == NULL)
			foundpool = _create_memory_pool(threadno);
		VK_SEM_Release(s_ulSMemSemId);
	}

	return _alloc_mem(foundpool, size);
}

/* *************************************************************************************************** */
#define ___PUBLIC_FUNCTION_______________________________________________________________________________
/* *************************************************************************************************** */
void	HxSMEM_Init(void)
{
	//	Make semaphore for managing memory pool
	HxLOG_Print("HxSMEM_Init....\n");
	if (s_ulSMemSemId == 0)
		VK_SEM_Create((unsigned long *)&s_ulSMemSemId, "SMEMSem", VK_SUSPEND_FIFO);
}



void	*HxSMEM_Alloc(int size)
{
	void	*p;

	p = smem_alloc(size);
	if (p == NULL)
		HxLOG_Print("\n\n alloc fail!!!\n\n");

	return p;
}

void	HxSMEM_Free(void *ptr)
{
	//printf("free mem : ptr [%x]\n", ptr);
	_free_mem(ptr);
}

void	*HxSMEM_Calloc(int size)
{
	void	*p = smem_alloc(size);

	if (p)
		HxSMEM_MemSet(p, 0, size);

	return p;
}

void	*HxSMEM_Realloc(void *ptr, int size)
{
	void	*p = smem_alloc(size);

	if (ptr)
	{
		HxSMEM_MemCpy(p, ptr, size);
		_free_mem(ptr);
	}

	return p;
}

void	HxSMEM_MemSet(void *dest, unsigned char c, int count)
{
	register unsigned long		i;
	unsigned char	*p8;
	unsigned long	*p32;
	unsigned long	val32;
	unsigned long	count32;

	p8 = (unsigned char*)dest;
	if (((int)p8) & 3)
	{
		for (i = 0; i < count; i++)
		{
			if ( (((int)p8) & 3) == 0)
				break;
			*p8++ = c;
		}
		count -= i;
	}

	p32 = (unsigned long*)p8;
	count32 = count >> 2;
	val32 = ((c) | (c << 8) | (c << 16) | (c << 24));
	i = count32;
	while (i >= 4)
	{
		*p32++ = val32;
		*p32++ = val32;
		*p32++ = val32;
		*p32++ = val32;
		i -= 4;
	}
	while (i >= 1)
	{
		*p32++ = val32;
		i--;
	}

	count -= (count32 << 2);
	p8 = (unsigned char*)p32;
	for (i = 0; i < count; i++)
		*p8++ = c;
}

#define UNALIGNED32(X, Y) (((long)X & (sizeof (long) - 1)) | ((long)Y & (sizeof (long) - 1)))
#define BIGBLOCKSIZE    (sizeof (long) << 2)
#define LITTLEBLOCKSIZE (sizeof (long))
#define TOO_SMALL(LEN)  ((LEN) < BIGBLOCKSIZE)

void *HxSMEM_MemCpy(void *dst0, void *src0, unsigned long len0)
{
	char *dst = dst0;
	const	char *src = src0;
	long *aligned_dst;
	const	long *aligned_src;
	unsigned int   len =  len0;

	if (!TOO_SMALL(len) && !UNALIGNED32 (src, dst))
	{
		aligned_dst = (long*)dst;
		aligned_src = (const long*)src;

		/* Copy 4X long words at a time if possible.  */
		while (len >= BIGBLOCKSIZE)
		{
			*aligned_dst++ = *aligned_src++;
			*aligned_dst++ = *aligned_src++;
			*aligned_dst++ = *aligned_src++;
			*aligned_dst++ = *aligned_src++;
			len -= BIGBLOCKSIZE;
		}

		/* Copy one long word at a time if possible.  */
		while (len >= LITTLEBLOCKSIZE)
		{
			*aligned_dst++ = *aligned_src++;
			len -= LITTLEBLOCKSIZE;
		}

		/* Pick up any residual with a byte copier.  */
		dst = (char*)aligned_dst;
		src = (const char*)aligned_src;
	}

	while (len--)
		*dst++ = *src++;

	return dst0;
}

#ifdef	SMEM_DEBUG
void*	HxSMEM_DBG_Alloc(int size, char *function, int line)
{
	void	*p;

	p = smem_alloc(size);

	_add_dbginfo(p, function, line);

	return p;
}

void*	HxSMEM_DBG_Realloc(void *ptr, int size, char *function, int line)
{
	void	*p = smem_alloc(size);

	if (ptr)
	{
		OS_MEM_Memcpy(p, ptr, size);
		_free_mem(ptr);
	}

	_add_dbginfo(p, function, line);

	return p;
}

void*	HxSMEM_DBG_Calloc(int size, char *function, int line)
{
	void	*p = smem_alloc(size);

	if (p)
		OS_MEM_Memset(p, 0, size);

	_add_dbginfo(p, function, line);

	return p;
}
#endif

