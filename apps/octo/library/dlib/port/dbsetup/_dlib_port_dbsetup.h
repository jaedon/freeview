
#include "../../_dlib.h"


void			DLIB_DBSETUP_Init(void);
HINT32			DLIB_DBSETUP_GetFieldCount (void);
DxField_t *		DLIB_DBSETUP_GetField (HINT32 index, const void *src, HUINT32 srcSize);
HERROR			DLIB_DBSETUP_Encode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_DBSETUP_Decode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_DBSETUP_Compress (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_DBSETUP_Extract (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_DBSETUP_MakeType (HUID uid, const void *src, HUINT32 srcSize, void **type);
void			DLIB_DBSETUP_FreeType (void *type);


