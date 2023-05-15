
#include "../../_dlib.h"


void			DLIB_CHANNEL_LOGO_Init(void);
const HCHAR *	DLIB_CHANNEL_LOGO_GetDBFullName (HINT32 dupIndex);
HINT32			DLIB_CHANNEL_LOGO_GetFieldCount (void);
DxField_t *		DLIB_CHANNEL_LOGO_GetField (HINT32 index, const void *src, HUINT32 srcSize);
HERROR			DLIB_CHANNEL_LOGO_Encode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_CHANNEL_LOGO_Decode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_CHANNEL_LOGO_Compress (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_CHANNEL_LOGO_Extract (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_CHANNEL_LOGO_MakeType (HUID uid, const void *src, HUINT32 srcSize, void **type);
void			DLIB_CHANNEL_LOGO_FreeType (void *type);


