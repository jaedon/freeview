
#include "../../_dlib.h"


void			DLIB_RECORDING_Init(void);
const HCHAR *	DLIB_RECORDING_GetDBFullName (HINT32 dupIndex);
HINT32			DLIB_RECORDING_GetFieldCount (void);
DxField_t *		DLIB_RECORDING_GetField (HINT32 index, const void *src, HUINT32 srcSize);
void*			DLIB_RECORDING_CloneType(void *org, HINT32 nSize);
HERROR			DLIB_RECORDING_Encode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_RECORDING_Decode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_RECORDING_Compress (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_RECORDING_Extract (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_RECORDING_MakeType (HUID uid, const void *src, HUINT32 srcSize, void **type);
void			DLIB_RECORDING_FreeType (void *type);
void			DLIB_RECORDING_Print (FILE *fp, const DxRecListData_t *t);


