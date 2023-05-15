
#include "../../_dlib.h"

void			DLIB_BOUQUET_Init(void);
HINT32			DLIB_BOUQUET_GetFieldCount (void);
DxField_t *		DLIB_BOUQUET_GetField (HINT32 index, const void *src, HUINT32 srcSize);
HERROR			DLIB_BOUQUET_Encode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_BOUQUET_Decode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_BOUQUET_Compress (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_BOUQUET_Extract (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_BOUQUET_MakeType (HUID uid, const void *src, HUINT32 srcSize, void **type);
void			DLIB_BOUQUET_FreeType (void *type);
void			DLIB_BOUQUET_Print (FILE *fp, const DxBouquet_t *t);


