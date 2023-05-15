
#include "../../_dlib.h"


void			DLIB_FILEINFO_Init(void);
const HCHAR *	DLIB_FILEINFO_GetDBFullName (HINT32 dupIndex);
HINT32			DLIB_FILEINFO_GetFieldCount (void);
DxField_t *		DLIB_FILEINFO_GetField (HINT32 index, const void *src, HUINT32 srcSize);
HERROR			DLIB_FILEINFO_Encode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_FILEINFO_Decode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_FILEINFO_Serialize (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_FILEINFO_Deserialize (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_FILEINFO_Compress (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_FILEINFO_Extract (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
void			DLIB_FILEINFO_Print (FILE *fp, const void *t);

