
#include "../../_dlib.h"

void			DLIB_EPG_HUMAX_Init(void);
const HCHAR *	DLIB_EPG_HUMAX_GetDBFullName (HINT32 dupIndex);
HUINT32			DLIB_EPG_HUMAX_GetSyncTime (void);
HINT32			DLIB_EPG_HUMAX_GetFieldCount (void);
DxField_t *		DLIB_EPG_HUMAX_GetField (HINT32 index, const void *src, HUINT32 srcSize);
HERROR			DLIB_EPG_HUMAX_Encode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_EPG_HUMAX_Decode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_EPG_HUMAX_Compress (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_EPG_HUMAX_Extract (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);
HERROR			DLIB_EPG_HUMAX_MakeType (HUID uid, const void *src, HUINT32 srcSize, void **type);
void			DLIB_EPG_HUMAX_FreeType (void *type);
const HCHAR*	DLIB_EPG_HUMAX_GetRepositoryPath(void);
HUINT32			DLIB_EPG_HUMAX_GetThreshold (void);
HUINT64			DLIB_EPG_HUMAX_GetThresholdSize (void);
HERROR			DLIB_EPG_HUMAX_ReadData(HUID uid, const HCHAR *pszLink, void **ppvData, HUINT32 *pnSize);
HERROR			DLIB_EPG_HUMAX_WriteData(HUID uid, const HCHAR *pszLink, const void *pvData, HUINT32 size);
HERROR			DLIB_EPG_HUMAX_LinkName(HUID uid, const void *pvData, HUINT32 size, void *name, HUINT32 nameSize);
HERROR			DLIB_EPG_HUMAX_SyncData(void);
HERROR			DLIB_EPG_HUMAX_OnStorageAdded (DxStorage_Info_t *pstStorage);
HERROR			DLIB_EPG_HUMAX_OnStorageRemoved (DxStorage_Info_t *pstStorage);
void 			DLIB_EPG_HUMAX_Print_Event(FILE *fp, const void *t);




