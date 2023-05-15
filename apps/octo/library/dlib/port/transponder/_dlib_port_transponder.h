
#include "../../_dlib.h"


void			DLIB_TRANSPONDER_Init(void);
HINT32			DLIB_TRANSPONDER_GetFieldCount (void);
DxField_t *		DLIB_TRANSPONDER_GetField (HINT32 index, const void *src, HUINT32 srcSize);
HERROR			DLIB_TRANSPONDER_Encode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_TRANSPONDER_Decode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_TRANSPONDER_Compress (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_TRANSPONDER_Extract (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_TRANSPONDER_MakeType (HUID uid, const void *src, HUINT32 srcSize, void **type);
void			DLIB_TRANSPONDER_FreeType (void *type);
void			DLIB_TRANSPONDER_Print (FILE *fp, const DxTransponder_t *t);


