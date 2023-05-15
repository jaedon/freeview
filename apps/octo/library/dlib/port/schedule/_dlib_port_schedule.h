
#include "../../_dlib.h"


void			DLIB_SCHEDULE_Init(void);
HINT32			DLIB_SCHEDULE_GetFieldCount (void);
DxField_t *		DLIB_SCHEDULE_GetField (HINT32 index, const void *src, HUINT32 srcSize);
HERROR			DLIB_SCHEDULE_Encode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_SCHEDULE_Decode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_SCHEDULE_Compress (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_SCHEDULE_Extract (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_SCHEDULE_MakeType (HUID uid, const void *src, HUINT32 srcSize, void **type);
void			DLIB_SCHEDULE_FreeType (void *type);


