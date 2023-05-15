
#include "../../_dlib.h"

const HCHAR *	DLIB_SVC_HUMAX_GetDBFullName (HINT32 dupIndex);
HUINT32			DLIB_SVC_HUMAX_GetSyncTime (void);
HINT32			DLIB_SVC_HUMAX_GetFieldCount (void);
DxField_t *		DLIB_SVC_HUMAX_GetField (HINT32 index, const void *src, HUINT32 srcSize);
HERROR			DLIB_SVC_HUMAX_MakeType (HUID uid, const void *src, HUINT32 srcSize, void **type);
void			DLIB_SVC_HUMAX_FreeType (void *type);


