

#ifndef	HxSERIALIZER_H__
#define HxSERIALIZER_H__

#include "htype.h"
#include "_hlib_types.h"

#ifdef	__cplusplus
extern	"C" {
#endif

#ifdef	CONFIG_DEBUG
#define	HxSERIALIZER_DEBUG
#endif

typedef enum
{
	HxOBJECT_TYPE_BIN	= 'b',
	HxOBJECT_TYPE_STR	= 's',
	HxOBJECT_TYPE_INT	= 'i',
	HxOBJECT_TYPE_INT64	= 'l',
	HxOBJECT_TYPE_ARR	= 'a'
} HxObjectTag_e;

typedef struct hxobject HxObject_t;
struct hxobject
{
	HxObjectTag_e	type;

	union
	{
		HCHAR		*str;
		HINT32		num;
		HINT64		num64;

		struct
		{
			HUINT32	size;
			void 	*data;
		} bin;

		struct
		{
			HUINT32		len;
			HxObject_t	**values;
		} array;
	} u;
	void *garbage;
};
#define	HxOBJECT_STR(p)			(((HxObject_t *)(p))->u.str)
#define	HxOBJECT_INT(p)			(((HxObject_t *)(p))->u.num)
#define	HxOBJECT_INT64(p)			(((HxObject_t *)(p))->u.num64)
#define	HxOBJECT_BIN_DATA(p)	(((HxObject_t *)(p))->u.bin.data)
#define	HxOBJECT_BIN_SIZE(p)	(((HxObject_t *)(p))->u.bin.size)
#define	HxOBJECT_ARR_VAL(p,i)	(((HxObject_t *)(p))->u.array.values[i])
#define	HxOBJECT_ARR_LEN(p)		(((HxObject_t *)(p))->u.array.len)

#define	HxOBJECT_IS_STR(p)		(((HxObject_t *)(p))->type == HxOBJECT_TYPE_STR)
#define	HxOBJECT_IS_INT(p)		(((HxObject_t *)(p))->type == HxOBJECT_TYPE_INT)
#define	HxOBJECT_IS_BIN(p)		(((HxObject_t *)(p))->type == HxOBJECT_TYPE_BIN)
#define	HxOBJECT_IS_INT64(p)		(((HxObject_t *)(p))->type == HxOBJECT_TYPE_INT64)


/**
 * Serializing 과정을 거친 데이터를 unpack 한다
 */
HxObject_t *	HLIB_SERIALIZER_Unpack (const void *data, HUINT32 size);
void			HLIB_SERIALIZER_FreeObject (HxObject_t *object);

/**
 * HxObject_t의 alloc/free가 빈번할 것으로 보여, 해당 부분은 caching함
 * (cache를 사용치 않으려면 위의 Unpack,FreeObject pair를 사용하면 된다.)
 */
void *			HLIB_SERIALIZER_OpenUnpackCache (void);
void			HLIB_SERIALIZER_CloseUnpackCache (void *cache_handle);

/**
 * Serializing 과정을 거친 데이터를 unpack 한다
 *
 * @param	cache 	캐시
 * @param	data	serialized data
 * @param	size	size of data
 * @param	cloneData	data를 unpack과정에서 HxObject_t에 clonning한다. 아니면, 그냥 포인터를 복사
 */
HxObject_t *	HLIB_SERIALIZER_UnpackWithCache (void *cache, const void *data, HUINT32 size, HBOOL cloneData);
void			HLIB_SERIALIZER_FreeObjectWithCache (void *cache, HxObject_t *object);

void			HLIB_SERIALIZER_DebugObject (const HxObject_t *obj, FILE *fp);


/**
 * open Serializer
 *
 * @return	descriptor of Serializer
 */
HINT32			HLIB_SERIALIZER_Open (void);

/**
 * open Serializer with custom malloc & free
 *
 * @param	memalloc	malloc
 * @param	memfree		free
 * @return	descriptor of Serializer
 */
HINT32			HLIB_SERIALIZER_OpenEx (HxAllocFunc_t alloc, HxFreeFunc_t freee);

/**
 * close Serializer
 *
 * @param	srzr	descriptor of Serializer
 */
void			HLIB_SERIALIZER_Close (HINT32 srzr);

/**
 * clear Serializer
 *
 * @param	srzr	descriptor of Serializer
 */
void			HLIB_SERIALIZER_Clear (HINT32 srzr);

/**
 * check Serializer error
 *
 * @param	srzr	descriptor of Serializer
 * @return	if (error) return error text
 */
const HCHAR *	HLIB_SERIALIZER_Error (HINT32 srzr);

void *			HLIB_SERIALIZER_Pack (HINT32 srzr, HUINT32 *size);

HERROR	HLIB_SERIALIZER_MakeSerializedData(HINT32 nSrzr, const HCHAR *pszSignature, ...);

#ifndef	HxSERIALIZER_DEBUG
/**
 * write data to Serializer
 *
 * @param	srzr	descriptor of Serializer
 * @param	tag		see below table
 * @param	...		see below table
 *  +---------------------+--------------------------+
 *  | type | description  | param (separated with ,) |
 *  +------+--------------+--------------------------+
 *  |   [  | array-begin  | void                     |
 *  |   ]  | array-end    | void                     |
 *  |   s  | string       | const HCHAR * string     |
 *  |   i  | integer      | HINT32 integer           |
 *  |   l  | integer      | HINT64 integer           |
 *  |   b  | binary       | void *data, HUINT32 size |
 *  +------+--------------+--------------------------+
 * @return	>= 0: success, < 0: error
 */
HINT32	HLIB_SERIALIZER_Write (HINT32 srzr, HCHAR tag, ...);
HINT32	HLIB_SERIALIZER_Batch (HINT32 srzr, const HCHAR *tags, ...);
#else
HINT32	HLIB_SERIALIZER_DebugWrite (const HCHAR *file, HINT32 line, HINT32 srzr, HCHAR tag, ...);
HINT32	HLIB_SERIALIZER_DebugBatch (const HCHAR *file, HINT32 line, HINT32 srzr, const HCHAR *tags, ...);
#define	HLIB_SERIALIZER_Write(srzr,t,...)		HLIB_SERIALIZER_DebugWrite(__FILE__,__LINE__,srzr,t,##__VA_ARGS__)
#define	HLIB_SERIALIZER_Batch(srzr,t,...)		HLIB_SERIALIZER_DebugBatch(__FILE__,__LINE__,srzr,t,##__VA_ARGS__)
#endif

#define	HxSERIALIZER_Array(srzr)				HLIB_SERIALIZER_Write(srzr, '[')
#define	HxSERIALIZER_ArrayEnd(srzr)				HLIB_SERIALIZER_Write(srzr, ']')
#define	HxSERIALIZER_String(srzr, str)			HLIB_SERIALIZER_Write(srzr, 's', str)
#define	HxSERIALIZER_Integer(srzr, num)			HLIB_SERIALIZER_Write(srzr, 'i', num)
#define	HxSERIALIZER_LongInteger(srzr, num)			HLIB_SERIALIZER_Write(srzr, 'l', num)
#define	HxSERIALIZER_Binary(srzr, data, size)	HLIB_SERIALIZER_Write(srzr, 'b', data, size)

#ifdef	__cplusplus
}
#endif

#endif /* HxSERIALIZER_H__ */

