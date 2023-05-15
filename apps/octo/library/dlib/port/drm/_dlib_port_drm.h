#if 0
#include "../../_dlib.h"

void			DLIB_DRM_Init (void);
HERROR			DLIB_DRM_Encode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_DRM_Decode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_DRM_MakeType (HUID uid, const void *src, HUINT32 srcSize, void **type);
void			DLIB_DRM_FreeType (void *type);
void			DLIB_DRM_Print (FILE *fp, const void *t);
#endif

