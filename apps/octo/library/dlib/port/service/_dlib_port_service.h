
#include "../../_dlib.h"


void			DLIB_SERVICE_Init(void);
HINT32			DLIB_SERVICE_GetFieldCount (void);
DxField_t *		DLIB_SERVICE_GetField (HINT32 index, const void *src, HUINT32 srcSize);
HERROR			DLIB_SERVICE_Encode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_SERVICE_Decode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_SERVICE_Compress (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_SERVICE_Extract (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_SERVICE_MakeType (HUID uid, const void *src, HUINT32 srcSize, void **type);
void			DLIB_SERVICE_FreeType (void *type);
void			DLIB_SERVICE_Print (FILE *fp, const DxService_t *t);


