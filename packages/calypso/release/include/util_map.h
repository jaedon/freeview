#ifndef __UTIL_MAP_H__
#define __UTIL_MAP_H__

#ifdef  __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include "cso_def.h"

typedef enum {
	ELEM_TYPE_UNKNOWN,
	ELEM_TYPE_BOOL,
	ELEM_TYPE_INT32,
	ELEM_TYPE_UINT32,
	ELEM_TYPE_INT64,
	ELEM_TYPE_UINT64,
	ELEM_TYPE_DOUBLE,
	ELEM_TYPE_STRING,
	ELEM_TYPE_DATA	
} UTIL_MAP_ELEM_TYPE_t;

typedef struct util_map_elem_data
{
	int32_t nLen;
	int8_t *data;
} UTIL_MAP_ELEM_DATA_t;

typedef struct util_map_elem
{
	int8_t *pKey;
	int8_t *pBackwardCompKey;
	int8_t *pHelpMessage;
	UTIL_MAP_ELEM_TYPE_t eType;

	union {
		bool_t				elem_bool;
		int32_t 			elem_int32;
		uint32_t 			elem_uint32;
		int64_t 			elem_int64;
		uint64_t 			elem_uint64;
		double				elem_double;
		UTIL_MAP_ELEM_DATA_t elem_data;
	};
	
} UTIL_MAP_ELEM_t;

/* Forward declare */
typedef struct UTIL_MAP_t UTIL_MAP_t;

/* Set Interfaces */
RETURN_CODE UTIL_MAP_SetValueBool(UTIL_MAP_t ** ppMap, int8_t *pKey, bool_t bValue);
RETURN_CODE UTIL_MAP_SetValueInt32(UTIL_MAP_t ** ppMap, int8_t *pKey, int32_t nValue);
RETURN_CODE UTIL_MAP_SetValueUint32(UTIL_MAP_t ** ppMap, int8_t *pKey, uint32_t nValue);
RETURN_CODE UTIL_MAP_SetValueInt64(UTIL_MAP_t ** ppMap, int8_t *pKey, int64_t llValue);
RETURN_CODE UTIL_MAP_SetValueUint64(UTIL_MAP_t ** ppMap, int8_t *pKey, uint64_t llValue);
RETURN_CODE UTIL_MAP_SetValueDouble(UTIL_MAP_t ** ppMap, int8_t *pKey, double lfValue);
RETURN_CODE UTIL_MAP_SetValueString(UTIL_MAP_t ** ppMap, int8_t *pKey, int8_t *pValue);
RETURN_CODE UTIL_MAP_SetValueData(UTIL_MAP_t ** ppMap, int8_t *pKey, int8_t *pValue, int32_t nSize);
RETURN_CODE UTIL_MAP_AddBackwardCompatibleKey(UTIL_MAP_t * pMap, int8_t *pKey, int8_t *pAddCompKey);
RETURN_CODE UTIL_MAP_AddHelpMessage(UTIL_MAP_t * pMap, int8_t *pKey, int8_t *pString);

/* Get Interfaces */
UTIL_MAP_ELEM_t *UTIL_MAP_GetValue(UTIL_MAP_t *pMap, int8_t *pKey);
UTIL_MAP_ELEM_t *UTIL_MAP_GetNthValue(UTIL_MAP_t *pMap, int32_t nIdx);
int32_t			UTIL_MAP_GetElemCount(UTIL_MAP_t *pMap);

/* Utility */
RETURN_CODE UTIL_MAP_Serialize(UTIL_MAP_t *pMap, void **pRetData, int32_t *pRetDataSize);
UTIL_MAP_t 	*UTIL_MAP_Deserialize(void *pSerializedMapData, int32_t nSize);
UTIL_MAP_t	*UTIL_MAP_Duplicate(UTIL_MAP_t*);

/* Cleanup */
void		UTIL_MAP_Free(UTIL_MAP_t **ppMap);

/* Debugging */
void		UTIL_MAP_PrintElements(UTIL_MAP_t*);
void 		UTIL_MAP_PrintDataToHexTable(void *pSerializedData, int32_t nSize, int32_t nPrintSize);



#ifdef  __cplusplus
}
#endif

#endif /* __CSO_DEBUG_H__ */
