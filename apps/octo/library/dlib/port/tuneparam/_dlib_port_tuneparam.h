
#include "../../_dlib.h"


void			DLIB_TUNEPARAM_Init(void);
HINT32			DLIB_TUNEPARAM_GetFieldCount (void);
DxField_t *		DLIB_TUNEPARAM_GetField (HINT32 index, const void *src, HUINT32 srcSize);
HERROR			DLIB_TUNEPARAM_Encode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_TUNEPARAM_Decode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_TUNEPARAM_Compress (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_TUNEPARAM_Extract (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_TUNEPARAM_MakeType (HUID uid, const void *src, HUINT32 srcSize, void **type);
void			DLIB_TUNEPARAM_FreeType (void *type);


