
#include "../../_dlib.h"

void				DLIB_EPG_TVTV_Init(void);
const HCHAR *	DLIB_EPG_TVTV_GetDBFullName (HINT32 dupIndex);
HUINT32			DLIB_EPG_TVTV_GetSyncTime (void);
HINT32			DLIB_EPG_TVTV_GetFieldCount (void);
DxField_t *		DLIB_EPG_TVTV_GetField (HINT32 index, const void *src, HUINT32 srcSize);
HERROR			DLIB_EPG_TVTV_Encode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_EPG_TVTV_Decode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_EPG_TVTV_Compress (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_EPG_TVTV_Extract (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_EPG_TVTV_MakeType (HUID uid, const void *src, HUINT32 srcSize, void **type);
void				DLIB_EPG_TVTV_FreeType (void *type);
const HCHAR*	DLIB_EPG_TVTV_GetRepositoryPath(void);
HERROR			DLIB_EPG_TVTV_ReadData(HUID uid, const HCHAR *pszLink, void **ppvData, HUINT32 *pnSize);
HERROR			DLIB_EPG_TVTV_WriteData(HUID uid, const HCHAR *pszLink, const void *pvData, HUINT32 size);
HERROR			DLIB_EPG_TVTV_LinkName(HUID uid, const void *pvData, HUINT32 size, void *name, HUINT32 nameSize);
HERROR			DLIB_EPG_TVTV_SyncData(void);
void 			DLIB_EPG_TVTV_Print_Event(FILE *fp, const void *t);




