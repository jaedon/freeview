
#include "../../_dlib.h"

typedef struct
{
	HCHAR			*pszProcName;
	HCHAR			*pszKey;
	HxObjectTag_e	eTag;
	union
	{
		HINT32	integer;
		HCHAR	*pszString;
	} u;
} dlib_udbmodule_item_t;

void			DLIB_UDBMODULE_Init(void);
HERROR			DLIB_UDBMODULE_Encode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_UDBMODULE_Decode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);


