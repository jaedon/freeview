
#include "../../_dlib.h"


void			DLIB_GROUP_Init(void);
HINT32			DLIB_GROUP_GetFieldCount (void);
DxField_t *		DLIB_GROUP_GetField (HINT32 index, const void *src, HUINT32 srcSize);
HERROR			DLIB_GROUP_Encode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_GROUP_Decode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_GROUP_Compress (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_GROUP_Extract (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_GROUP_MakeType (HUID uid, const void *src, HUINT32 srcSize, void **type);
void			DLIB_GROUP_FreeType (void *type);
void			DLIB_GROUP_Print (FILE *fp, const DxGroup_t *t);


