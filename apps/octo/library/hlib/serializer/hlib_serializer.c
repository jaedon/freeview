


#if 1
#include <_hlib_log.h>
#include <_hlib_mem.h>
#include <_hlib_stdlib.h>
#include <_hlib_rwstream.h>
#include <_hlib_stack.h>
#include <_hlib_queue.h>
#include <_hlib_serializer.h>

//#define	SERIALIZER_MEM_CATEGORY		1

#define	SERIALIZER_ASSERT			HxLOG_Assert

#define	SERIALIZER_MALLOC		HLIB_MEM_Malloc
#define	SERIALIZER_CALLOC		HLIB_STD_MemCalloc
#define	SERIALIZER_FREE			HLIB_MEM_Free
#define	SERIALIZER_ERROR		HxLOG_Error

#else
#include "_hlib_rwstream.h"
#include "_hlib_stack.h"
#include "_hlib_queue.h"
#include "_hlib_serializer.h"
#include <assert.h>
#endif

#ifndef	SERIALIZER_MALLOC
#define	SERIALIZER_MALLOC		HLIB_MEM_Malloc
#endif

#ifndef	SERIALIZER_CALLOC
#define	SERIALIZER_CALLOC(size)	HLIB_MEM_Calloc(size)//calloc(size, sizeof(char))
#endif

#ifndef	SERIALIZER_FREE
#define	SERIALIZER_FREE			HLIB_MEM_Free
#endif

#ifndef	SERIALIZER_ASSERT
#define	SERIALIZER_ASSERT		assert
#endif

#ifndef	SERIALIZER_ERROR
#define	SERIALIZER_ERROR		printf
#endif


#define	DEPACK_THROW(__cond,__err,__expr)	\
		if(__cond){\
			SERIALIZER_ERROR("<%s:%d> exception: %s:%s\n",__FUNCTION__,__LINE__,#__cond,__err);\
			__expr;\
		}else{}

typedef struct
{
	HINT32	tag;
	HUINT32	size;
} packet_t;

typedef struct
{
	HBOOL			cloneData;
	HxAllocExFunc_t	alloc;
	HxFreeExFunc_t	freee;
	void			*udata;
} unpack_context_t;


typedef struct
{
	HxQueue_t	*queue;
} unpack_cache_t;


static void *	serializer_malloc (HUINT32 size, void *userdata)
{
	(void)userdata;
	return SERIALIZER_MALLOC(size);
}

static void *	serializer_calloc (HUINT32 size, void *userdata)
{
	(void)userdata;
	return SERIALIZER_CALLOC(size);
}


static void		serializer_free (void *data, void *userdata)
{
	(void)userdata;
	SERIALIZER_FREE(data);
}

static void *	serializer_memdup (const void *org, HUINT32 size)
{
	void *dup = SERIALIZER_MALLOC(size);
	if (dup)
	{
		memcpy(dup, org, size);
	}
	return dup;
}

static void *	unpack_cached_malloc (HUINT32 size, unpack_cache_t *cache)
{
	void *mem;

	mem = HLIB_QUEUE_Pop(cache->queue);
	if (mem == NULL)
	{
		mem = HLIB_MEM_Malloc(size);
	}
	return mem;
}

static void		unpack_cached_free (void *data, unpack_cache_t *cache)
{
	if (data)
		HLIB_QUEUE_Push(cache->queue, data);
}

static HCHAR *	unpack_dump (HCHAR *str, HUINT32 len, const HUINT8 *data, HINT32 n)
{
	HUINT32	i = 0;

	while (n--)
	{
		if (i == len)
			break;
		if (isalnum(*data))
			str[i] = (HCHAR)*data;
		else
			str[i] = '.';

		data++;
		i++;
	}
	if (i == len)
		str[i - 1] = '\0';
	else
		str[i] = '\0';
	return str;
}

static void			unpack_debug_object (const HxObject_t *obj, HUINT32 depth, FILE *fp)
{
	HCHAR	indent[64];
	HCHAR	bin[36];
	HUINT32	i;

	if (obj == NULL)
		return;

	for (i = 0 ; i < depth ; i++)
		indent[i] = '\t';
	indent[i] = '\0';

	switch (obj->type)
	{
	case HxOBJECT_TYPE_BIN:
		fprintf(fp, "%sBIN(size:%u, data:%s) garbage:0x%X\n", indent, HxOBJECT_BIN_SIZE(obj)
					, unpack_dump(bin, 36, HxOBJECT_BIN_DATA(obj), HxOBJECT_BIN_SIZE(obj)), (unsigned int)obj->garbage);
		break;
	case HxOBJECT_TYPE_INT:
		fprintf(fp, "%sINT(%d) garbage:0x%X\n", indent, HxOBJECT_INT(obj), (unsigned int)obj->garbage);
		break;
	case HxOBJECT_TYPE_INT64:
		fprintf(fp, "%sLONGINT(%lld) garbage:0x%X\n", indent, HxOBJECT_INT64(obj), (unsigned int)obj->garbage);
		break;
	case HxOBJECT_TYPE_STR:
		fprintf(fp, "%sSTR(%s) garbage:0x%X\n", indent, HxOBJECT_STR(obj), (unsigned int)obj->garbage);
		break;
	case HxOBJECT_TYPE_ARR:
		fprintf(fp, "%sARR(%d) garbage:0x%X [\n", indent, HxOBJECT_ARR_LEN(obj), (unsigned int)obj->garbage);
		for (i = 0 ; i < HxOBJECT_ARR_LEN(obj) ; i++)
		{
			unpack_debug_object(HxOBJECT_ARR_VAL(obj, i), depth + 1, fp);
		}
		fprintf(fp, "%s]\n", indent);
		break;
	default:
		break;
	}
}

static HxObject_t *	unpack_alloc_object (unpack_context_t *ctx, HINT32 type)
{
	HxObject_t	*pkt;

	pkt = (HxObject_t *)ctx->alloc(sizeof(HxObject_t), ctx->udata);
	if (pkt)
	{
		pkt->garbage = NULL;
		pkt->type = type == '[' ? HxOBJECT_TYPE_ARR : type;
	}
	return pkt;
}

static void			unpack_free_object (HxObject_t *obj, HxFreeExFunc_t freeee, void *userdata)
{
	if (obj == NULL)
		return;

	if (obj->type == HxOBJECT_TYPE_ARR)
	{
		HUINT32	i;

		for (i = 0 ; i < HxOBJECT_ARR_LEN(obj) ; i++)
		{
			unpack_free_object(HxOBJECT_ARR_VAL(obj, i), freeee, userdata);
		}
	}
	if (obj->garbage)
		serializer_free(obj->garbage, NULL);
	freeee(obj, userdata);
}

static HUINT32		unpack_array_len (unpack_context_t *ctx, const HUINT8 *stream, HUINT32 size)
{
	const packet_t *packet;
	HUINT32	length;
	HUINT32	offset;

	length = offset = 0;
	packet = (const packet_t *)stream;
	while (packet->tag != ']')
	{
		offset += packet->size + sizeof(packet_t);
		DEPACK_THROW(offset > size, "invalid stream: invalid array size", return 0);
		length++;
		packet   = (const packet_t *)(stream + offset);
	}
	DEPACK_THROW(size != offset + sizeof(packet_t), "invalid stream: ] tag size unmatched", return 0);
	(void)ctx;
	return length;
}

static HxObject_t *	unpack (unpack_context_t *ctx, const HUINT8 *stream, HUINT32 size)
{
	HxObject_t    *obj;
	HxObject_t    *val;
	packet_t      *pkt;
	packet_t      *iter;
	const HUINT8  *data;
	HUINT32       i;
	HUINT32       offset;

	if (size == 0)
		return NULL;

	pkt = (packet_t *)stream;

	DEPACK_THROW(pkt->size == 0, "invalid stream", return NULL);
	DEPACK_THROW(size < pkt->size + sizeof(packet_t), "invalid stream: invalid size", return NULL);
	DEPACK_THROW((obj = unpack_alloc_object(ctx, pkt->tag)) == NULL, "out of memory", return NULL);

	switch (pkt->tag)
	{
	case '[':
		obj->u.array.len = unpack_array_len(ctx, (const HUINT8 *)(pkt + 1), pkt->size);
		DEPACK_THROW(obj->u.array.len == 0, "invalid stream: empty array", break);

		obj->u.array.values = (HxObject_t **)serializer_calloc(sizeof(HxObject_t *) * obj->u.array.len, NULL);
		DEPACK_THROW(obj->u.array.values == NULL, "out of memory", break);
		obj->garbage = obj->u.array.values;

		offset = i = 0;
		data   = (const HUINT8 *)(pkt + 1);
		iter   = (packet_t *)(pkt + 1);
		while (iter->tag != ']')
		{
			DEPACK_THROW((val = unpack(ctx, (const HUINT8 *)iter, sizeof(packet_t) + iter->size)) == NULL, "unknown error", goto FUNC_ERROR);
			obj->u.array.values[i++] = val;
			iter = (packet_t *)((HUINT8 *)iter + (iter->size + sizeof(packet_t)));
		}
		return obj;

	case 'i':
		DEPACK_THROW(pkt->size != sizeof(HINT32), "invalid stream: integer size != 4", break);
		memcpy(&(obj->u.num), pkt + 1, pkt->size);
		return obj;

	case 'l':
		DEPACK_THROW(pkt->size != sizeof(HINT64), "invalid stream: integer size != 8", break);
		memcpy(&(obj->u.num64), pkt + 1, pkt->size);
		return obj;

	case 's':
		if (ctx->cloneData)
		{
			obj->u.str = (HCHAR *)serializer_memdup(pkt + 1, pkt->size);
			DEPACK_THROW(obj->u.str == NULL, "out of memory", break);
			obj->garbage = obj->u.str;
		}
		else
		{
			obj->u.str = (HCHAR *)(pkt + 1);
		}
		return obj;

	case 'b':
		if (ctx->cloneData)
		{
			obj->u.bin.size = pkt->size;
			obj->u.bin.data = serializer_memdup(pkt + 1, pkt->size);
			DEPACK_THROW(obj->u.bin.data == NULL, "out of memory", break);
			obj->garbage = obj->u.bin.data;
		}
		else
		{
			obj->u.bin.size = pkt->size;
			obj->u.bin.data = (void *)(pkt + 1);
		}
		return obj;

	default:
		DEPACK_THROW(strchr("[isb", pkt->tag) == NULL, "unknown tag", break);
		break;
	}
FUNC_ERROR:
	if (obj)
	{
		unpack_free_object(obj, ctx->freee, ctx->udata);
	}
	return NULL;
}

void *			HLIB_SERIALIZER_OpenUnpackCache (void)
{
	unpack_cache_t	*cache;

	cache = (unpack_cache_t *)HLIB_MEM_Calloc(sizeof(unpack_cache_t));
	if (cache)
	{
		cache->queue = HLIB_QUEUE_New((HxQueue_RemoveCallback_t)HLIB_MEM_Free_CB);
		if (cache->queue == NULL)
		{
			HLIB_SERIALIZER_CloseUnpackCache(cache);
			return NULL;
		}
	}
	return (void *)cache;
}

void			HLIB_SERIALIZER_CloseUnpackCache (void *cache_handle)
{
	unpack_cache_t	*cache;

	cache = (unpack_cache_t *)cache_handle;
	if (cache)
	{
		if (cache->queue)
			HLIB_QUEUE_Delete(cache->queue);
		HLIB_MEM_Free(cache);
	}
}

HxObject_t *	HLIB_SERIALIZER_Unpack (const void *data, HUINT32 size)
{
	return HLIB_SERIALIZER_UnpackWithCache (NULL, data, size, TRUE);
}

void			HLIB_SERIALIZER_FreeObject (HxObject_t *obj)
{
	HLIB_SERIALIZER_FreeObjectWithCache(NULL, obj);
}

HxObject_t *	HLIB_SERIALIZER_UnpackWithCache (void *cache_handle, const void *data, HUINT32 size, HBOOL cloneData)
{
	unpack_context_t	ctx;
	unpack_cache_t		*cache;

	cache = (unpack_cache_t *)cache_handle;

	memset(&ctx, 0, sizeof(unpack_context_t));

	if (cache == NULL)
	{
		ctx.alloc = serializer_malloc;
		ctx.freee = serializer_free;
		ctx.udata = NULL;
	}
	else
	{
		ctx.alloc = (HxAllocExFunc_t)unpack_cached_malloc;
		ctx.freee = (HxFreeExFunc_t )unpack_cached_free;
		ctx.udata = (void *)cache;
	}
	ctx.cloneData = cloneData;

	return unpack(&ctx, data, size);
}


void			HLIB_SERIALIZER_FreeObjectWithCache (void *cache_handle, HxObject_t *obj)
{
	unpack_cache_t	*cache;

	cache = (unpack_cache_t *)cache_handle;
	if (cache)
		unpack_free_object(obj, (HxFreeExFunc_t)unpack_cached_free, cache);
	else
		unpack_free_object(obj, serializer_free, NULL);
}


void			HLIB_SERIALIZER_DebugObject (const HxObject_t *obj, FILE *fp)
{
	SERIALIZER_ASSERT(obj);

	unpack_debug_object(obj, 0, fp ? fp : stdout);
}


typedef struct
{
	HINT32			rwstream;
	HxStack_t		*stack;
	HxAllocFunc_t	alloc;
	HxFreeFunc_t	freee;
	const HCHAR		*error;
} pack_context_t;

static HINT32	pack_write (pack_context_t *ctx, HCHAR tag, va_list ap, const HCHAR *file, HINT32 line)
{
	packet_t	packet;
	HUINT32		written;

	packet.tag = tag;
	switch (tag)
	{
	case '[':
	{
		HxStack_Push(ctx->stack, (void *)HLIB_RWSTREAM_GetOffset(ctx->rwstream));
		packet.size = 0;
		return (HINT32)HLIB_RWSTREAM_Write(ctx->rwstream, &packet, sizeof(packet_t));
	}

	case ']':
	{
		HINT32	offset;
		HINT32	last;

		if (HxStack_Length(ctx->stack) == 0)
		{
			ctx->error = "syntax error: array does not opened";
			break;
		}
		offset = (HINT32)HLIB_STACK_Pop(ctx->stack);
		last   = HLIB_RWSTREAM_GetOffset(ctx->rwstream);
		if (offset == last)
		{
			ctx->error = "syntax error: empty array";
			break;
		}
		HLIB_RWSTREAM_Seek(ctx->rwstream, offset);
		packet.tag  = '[';
		packet.size = last - offset;
		HLIB_RWSTREAM_Write(ctx->rwstream, &packet, sizeof(packet_t));
		HLIB_RWSTREAM_Seek(ctx->rwstream, last);
		packet.tag  = ']';
		packet.size = 0;
		return (HINT32)HLIB_RWSTREAM_Write(ctx->rwstream, &packet, sizeof(packet_t));
	}
	case 'i':
	{
		HINT32	num = va_arg(ap, HINT32);

		packet.size = sizeof(HINT32);
		written   = HLIB_RWSTREAM_Write(ctx->rwstream, &packet, sizeof(packet_t));
		written  += HLIB_RWSTREAM_Write(ctx->rwstream, &num, packet.size);
		return (HINT32)written;
	}
	case 'l':
	{
		HINT64	num64 = va_arg(ap, HINT64);
		packet.size = sizeof(HINT64);
		written   = HLIB_RWSTREAM_Write(ctx->rwstream, &packet, sizeof(packet_t));
		written  += HLIB_RWSTREAM_Write(ctx->rwstream, &num64, packet.size);
		return (HINT32)written;
	}

	case 's':
	{
		const HCHAR *str = va_arg(ap, const HCHAR *);

		if (str == NULL)
		{
			ctx->error = "invalid parameter: NULL string";
			break;
		}
		packet.size = strlen(str) + 1;
		written   = HLIB_RWSTREAM_Write(ctx->rwstream, &packet, sizeof(packet_t));
		written  += HLIB_RWSTREAM_Write(ctx->rwstream, (void *)str, packet.size);
		return (HINT32)written;
	}

	case 'b':
	{
		void   *data = va_arg(ap, void *);
		HUINT32 size = va_arg(ap, HUINT32);

		if (data == NULL || size == 0)
		{
			ctx->error = "invalid parameter: NULL binary";
			break;
		}
		packet.size = size;
		written   = HLIB_RWSTREAM_Write(ctx->rwstream, &packet, sizeof(packet_t));
		written  += HLIB_RWSTREAM_Write(ctx->rwstream, data, packet.size);
		return (HINT32)written;
	}

	default:
		ctx->error = "invalid parameter: unknown tag";
		break;
	}
	SERIALIZER_ERROR("<%s:%d> \'%c\' exception:%s from[%s:%d]\n", __FUNCTION__, __LINE__, tag, ctx->error, file, line);
	return -1;
}

HINT32			HLIB_SERIALIZER_Open (void)
{
	return HLIB_SERIALIZER_OpenEx((HxAllocFunc_t)serializer_malloc, (HxFreeFunc_t)serializer_free);
}

HINT32			HLIB_SERIALIZER_OpenEx (HxAllocFunc_t alloc, HxFreeFunc_t freee)
{
	pack_context_t	*ctx;

	SERIALIZER_ASSERT(alloc && freee);

	ctx = (pack_context_t *)alloc(sizeof(pack_context_t));
	if (ctx)
	{
		memset(ctx, 0, sizeof(pack_context_t));

		ctx->alloc = alloc;
		ctx->freee = freee;

		ctx->rwstream = HLIB_RWSTREAM_Open(NULL, 0);
		if (!ctx->rwstream)
		{
			HLIB_SERIALIZER_Close((HINT32)ctx);
			return 0;
		}
		ctx->stack = HxStack_New(NULL, NULL);
		if (ctx->stack == NULL)
		{
			HLIB_SERIALIZER_Close((HINT32)ctx);
			return 0;
		}
	}
	return (HINT32)ctx;
}

void			HLIB_SERIALIZER_Close (HINT32 sezr)
{
	pack_context_t	*ctx;

	SERIALIZER_ASSERT(sezr);

	ctx = (pack_context_t *)sezr;

	if (ctx->rwstream)
		HLIB_RWSTREAM_Close(ctx->rwstream);
	if (ctx->stack)
		HxStack_Delete(ctx->stack);
	ctx->freee(ctx);
}

void			HLIB_SERIALIZER_Clear (HINT32 sezr)
{
	pack_context_t	*ctx;

	SERIALIZER_ASSERT(sezr);

	ctx = (pack_context_t *)sezr;

	HLIB_RWSTREAM_Truncate(ctx->rwstream);
	HLIB_VECTOR_RemoveAll((HxVector_t *)ctx->stack);
	ctx->error = NULL;
}

const HCHAR *	HLIB_SERIALIZER_Error (HINT32 sezr)
{
	pack_context_t	*ctx;

	SERIALIZER_ASSERT(sezr);

	ctx = (pack_context_t *)sezr;
	if (ctx->error)
		return ctx->error;

	if (HxStack_Length(ctx->stack) > 0)
	{
		return "syntax error: array does not closed";
	}
	return NULL;
}

void *			HLIB_SERIALIZER_Pack (HINT32 sezr, HUINT32 *size)
{
	pack_context_t	*ctx;

	if (HLIB_SERIALIZER_Error(sezr))
		return NULL;

	SERIALIZER_ASSERT(sezr);

	ctx = (pack_context_t *)sezr;
	if (size)
		*size = HLIB_RWSTREAM_GetWritten(ctx->rwstream);
	return HLIB_RWSTREAM_GetBuf(ctx->rwstream);
}

HERROR	HLIB_SERIALIZER_MakeSerializedData(HINT32 nSrzr, const HCHAR *pszSignature, ...)
{
	const HCHAR	*pszText;
	HINT64		nNumber;
	HUINT32		ulSize;
	void 		*pvData;
	va_list 	ap;

	va_start(ap, pszSignature);

	if (pszSignature)
	{
		HLIB_SERIALIZER_Clear(nSrzr);
		HxSERIALIZER_Array(nSrzr);

		while (*pszSignature != '\0')
		{
			switch (*pszSignature++)
			{
				case 's':
					pszText = va_arg(ap, const HCHAR *);
					if (pszText == NULL)
						HxSERIALIZER_String(nSrzr, "");
					else
						HxSERIALIZER_String(nSrzr, pszText);
					break;
				case 'i':
					nNumber = va_arg(ap, HINT32);
					HxSERIALIZER_Integer(nSrzr, nNumber);
					break;
				case 'l':
					nNumber = va_arg(ap, HINT64);
					HxSERIALIZER_LongInteger(nSrzr, nNumber);
					break;
				case 'b':
					pvData = va_arg(ap, void *);
					ulSize = va_arg(ap, HUINT32);
					HxSERIALIZER_Binary(nSrzr, pvData, ulSize);
					break;
				default:
					break;
			}
		}

		HxSERIALIZER_ArrayEnd(nSrzr);
	}


	va_end(ap);

	return ERR_OK;
}

#ifndef	HxSERIALIZER_DEBUG

HINT32		HLIB_SERIALIZER_Write (HINT32 sezr, HCHAR tag, ...)
{
	pack_context_t	*ctx;
	va_list			ap;
	HINT32			ret;

	SERIALIZER_ASSERT(sezr);

	ctx = (pack_context_t *)sezr;
	if (ctx->error)
		return -1;

	va_start(ap, tag);
	ret = pack_write(ctx, tag, ap, "", -1);
	va_end(ap);

	return ret;
}

HINT32		HLIB_SERIALIZER_Batch (HINT32 srzr, const HCHAR *tags, ...)
{
	va_list		ap;
	HINT32		ret;
	const HCHAR	*s;
	HINT32		d;
	void		*b;
	HUINT32		size;
	HINT32		sum = 0;

	SERIALIZER_ASSERT(srzr && tags);

	va_start(ap, tags);
	while (*tags != '\0')
	{
		switch (*tags)
		{
		case '[':
		case ']':
			ret = HLIB_SERIALIZER_Write(srzr, *tags);
			break;

		case 's':
			s   = va_arg(ap, const HCHAR *);
			ret = HLIB_SERIALIZER_Write(srzr, *tags, s);
			break;

		case 'd':
			d   = va_arg(ap, HINT32);
			ret = HLIB_SERIALIZER_Write(srzr, *tags, d);
			break;

		case 'b':
			b   = va_arg(ap, void *);
			size= va_arg(ap, HUINT32);
			ret = HLIB_SERIALIZER_Write(srzr, *tags, b, size);
			break;

		default:
			va_end(ap);
			return -1;
		}
		if (ret < 0)
		{
			va_end(ap);
			return -1;
		}
		sum += ret;
		tags++;
	}
	va_end(ap);
	return sum;
}

#else

HINT32			HLIB_SERIALIZER_DebugWrite (const HCHAR *file, HINT32 line, HINT32 sezr, HCHAR tag, ...)
{
	pack_context_t	*ctx;
	va_list			ap;
	HINT32			ret;

	SERIALIZER_ASSERT(sezr);

	ctx = (pack_context_t *)sezr;
	if (ctx->error)
		return -1;

	va_start(ap, tag);
	ret = pack_write(ctx, tag, ap, file, line);
	va_end(ap);

	//serializer_dump(tag, HLIB_RWSTREAM_GetBuf(ctx->rwstream), HLIB_RWSTREAM_GetWritten(ctx->rwstream));

	return ret;
}

HINT32	HLIB_SERIALIZER_DebugBatch (const HCHAR *file, HINT32 line, HINT32 srzr, const HCHAR *tags, ...)
{
	va_list		ap;
	HINT32		ret;
	const HCHAR	*s;
	HINT64		i;
	void		*b;
	HUINT32		size;
	HINT32		sum = 0;

	SERIALIZER_ASSERT(srzr && tags);

	va_start(ap, tags);
	while (*tags != '\0')
	{
		switch (*tags)
		{
		case '[':
		case ']':
			ret = HLIB_SERIALIZER_DebugWrite(file, line, srzr, *tags);
			break;

		case 's':
			s   = va_arg(ap, const HCHAR *);
			ret = HLIB_SERIALIZER_DebugWrite(file, line, srzr, *tags, s);
			break;

		case 'i':
			i   = va_arg(ap, HINT32);
			ret = HLIB_SERIALIZER_DebugWrite(file, line, srzr, *tags, i);
			break;

		case 'l':
			i	= va_arg(ap, HINT64);
			ret = HLIB_SERIALIZER_DebugWrite(file, line, srzr, *tags, i);
			break;

		case 'b':
			b   = va_arg(ap, void *);
			size= va_arg(ap, HUINT32);
			ret = HLIB_SERIALIZER_DebugWrite(file, line, srzr, *tags, b, size);
			break;

		default:
			va_end(ap);
			return -1;
		}
		if (ret < 0)
		{
			va_end(ap);
			return -1;
		}
		sum += ret;
		tags++;
	}
	va_end(ap);
	return sum;
}
#endif

