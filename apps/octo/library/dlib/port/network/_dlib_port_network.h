
#include "../../_dlib.h"


void			DLIB_NETWORK_Init(void);
HINT32			DLIB_NETWORK_GetFieldCount (void);
DxField_t *		DLIB_NETWORK_GetField (HINT32 index, const void *src, HUINT32 srcSize);
HERROR			DLIB_NETWORK_Encode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_NETWORK_Decode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_NETWORK_Compress (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_NETWORK_Extract (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_NETWORK_MakeType (HUID uid, const void *src, HUINT32 srcSize, void **type);
void			DLIB_NETWORK_FreeType (void *type);
void			DLIB_NETWORK_Print (FILE *fp, const DxNetwork_t *t);


