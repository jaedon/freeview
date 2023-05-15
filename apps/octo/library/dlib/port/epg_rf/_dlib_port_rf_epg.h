
#include "../../_dlib.h"

void			DLIB_EPG_RF_Init (void);

const HCHAR *	DLIB_EPG_PF_GetDBFullName (HINT32 dupIndex);
const HCHAR *	DLIB_EPG_SC_GetDBFullName (HINT32 dupIndex);
HUINT32			DLIB_EPG_PF_GetPushCycle (void);
HUINT32			DLIB_EPG_SC_GetPushCycle (void);
HUINT32			DLIB_EPG_PF_GetThreshold (void);
HUINT32			DLIB_EPG_SC_GetThreshold (void);
HINT32			DLIB_EPG_RF_GetFieldCount (void);

DxField_t *		DLIB_EPG_RF_GetField (HINT32 index, const void *src, HUINT32 srcSize);
HERROR			DLIB_EPG_RF_Encode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_EPG_RF_Decode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_EPG_RF_Compress (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_EPG_RF_Extract (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_EPG_RF_MakeType (HUID uid, const void *src, HUINT32 srcSize, void **type);
void			DLIB_EPG_RF_FreeType (void *type);
void			DLIB_EPG_RF_Print (FILE *fp, const void *t);


