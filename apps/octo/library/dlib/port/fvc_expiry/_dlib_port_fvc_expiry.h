
#include "../../_dlib.h"


void			DLIB_FVC_EXPIRY_Init(void);
const HCHAR *	DLIB_FVC_EXPIRY_GetDBFullName (HINT32 dupIndex);
HINT32			DLIB_FVC_EXPIRY_GetFieldCount (void);
DxField_t *		DLIB_FVC_EXPIRY_GetField (HINT32 index, const void *src, HUINT32 srcSize);
HERROR			DLIB_FVC_EXPIRY_Encode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_FVC_EXPIRY_Decode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_FVC_EXPIRY_Compress (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_FVC_EXPIRY_Extract (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_FVC_EXPIRY_MakeType (HUID uid, const void *src, HUINT32 srcSize, void **type);
void			DLIB_FVC_EXPIRY_FreeType (void *type);


