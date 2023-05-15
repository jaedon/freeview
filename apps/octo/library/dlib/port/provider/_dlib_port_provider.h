
#include "../../_dlib.h"


void			DLIB_PROVIDER_Init(void);
HINT32			DLIB_PROVIDER_GetFieldCount (void);
DxField_t *		DLIB_PROVIDER_GetField (HINT32 index, const void *src, HUINT32 srcSize);
HERROR			DLIB_PROVIDER_Encode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_PROVIDER_Decode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_PROVIDER_Compress (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_PROVIDER_Extract (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_PROVIDER_MakeType (HUID uid, const void *src, HUINT32 srcSize, void **type);
void			DLIB_PROVIDER_FreeType (void *type);
void			DLIB_PROVIDER_Print (FILE *fp, const DxProvider_t *t);


