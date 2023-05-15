
#include "../../_dlib.h"

const HCHAR *	DLIB_SVC_TVTV_GetDBFullName (HINT32 dupIndex);
HUINT32			DLIB_SVC_TVTV_GetSyncTime (void);
HINT32			DLIB_SVC_TVTV_GetFieldCount (void);
DxField_t *		DLIB_SVC_TVTV_GetField (HINT32 index, const void *src, HUINT32 srcSize);
HERROR			DLIB_SVC_TVTV_MakeType (HUID uid, const void *src, HUINT32 srcSize, void **type);
void				DLIB_SVC_TVTV_FreeType (void *type);


