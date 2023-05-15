
#include "../../_dlib.h"


void			DLIB_ANTINFO_Init(void);
HINT32			DLIB_ANTINFO_GetFieldCount (void);
DxField_t *		DLIB_ANTINFO_GetField (HINT32 index, const void *src, HUINT32 srcSize);
HERROR			DLIB_ANTINFO_Encode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_ANTINFO_Decode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_ANTINFO_Compress (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_ANTINFO_Extract (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_ANTINFO_MakeType (HUID uid, const void *src, HUINT32 srcSize, void **type);
void			DLIB_ANTINFO_FreeType (void *type);
void			DLIB_ANTINFO_Print (FILE *fp, const void *v);


