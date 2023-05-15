/* $Header: $ */

#ifndef _DLIB_H
#define _DLIB_H

#ifdef __cplusplus
extern "C" {
#endif

#include "hlib.h"
#include "dlib.h"

#define	DxLIB_PRINT_INT(field)		fprintf(fp, "%30s: %d\n", #field, field)
#define	DxLIB_PRINT_HEX(field)		fprintf(fp, "%30s: 0x%X\n", #field, field)
#define DxLIB_PRINT_STR(field)		fprintf(fp, "%30s: %s\n", #field, field)
#define DxLIB_PRINT_BOOL(field)		fprintf(fp, "%30s: %s\n", #field, field ? "true" : "false")
#define DxLIB_PRINT_ENUM(field)		fprintf(fp, "%30s: enum %d\n", #field, field)
#define DxLIB_PRINT_INT_ARRAY(field, n)		\
	do {\
		HINT32	__i;\
		fprintf(fp, "%30s: [", #field);\
		for (__i = 0 ; __i < n ; __i++)\
			fprintf(fp, "%c%d", __i == 0 ? ' ' : ',', field[__i]);\
		fprintf(fp, "]\n");\
	} while (0)

#define DxLIB_PRINT_HEX_ARRAY(field, n)		\
	do {\
		HINT32	__i;\
		fprintf(fp, "%30s: [", #field);\
		for (__i = 0 ; __i < n ; __i++)\
			fprintf(fp, "%c%X", __i == 0 ? ' ' : ',', field[__i]);\
		fprintf(fp, "]\n");\
	} while (0)

#define DxLIB_PRINT_BYTE_DUMP(field, size) 	\
	do {\
		const HUINT8 *b = (const HUINT8 *)&(field);\
		HUINT32	s = size;\
		fprintf(fp, "%30s: ", #field);\
		while (s--) fprintf(fp, "%02X", *b++);\
		fprintf(fp, "\n");\
	} while (0)


typedef struct
{
	HCHAR			name[DxLIB_MAX_NAME];
	HCHAR			tags[DxLIB_MAX_NAME];

	void			(* init)(void);
	const HCHAR *	(* getDBFullName) (HINT32 dupIndex);
	HUINT32			(* getSyncTime) (void);
	HUINT32			(* getPushCycle) (void);
	HUINT32			(* getThreshold) (void);
	HUINT64			(* getThresholdSize) (void);
	HINT32			(* getFieldCount) (void);
	DxField_t *		(* getField) (HINT32 index, const void *src, HUINT32 srcSize);
	HERROR			(* encode) (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);			// DB 저장용
	HERROR			(* decode) (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);			// DB 용
	HERROR			(* serialize) (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);		// 통신 용 serialize
	HERROR			(* deserialize) (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);		// 통신 용 deserialize
	void *			(* cloneType) (void *type, HINT32 size);
	HERROR			(* makeType) (HUID uid, const void *src, HUINT32 srcSize, void **type);
	void			(* freeType) (void *type);
	const HCHAR *	(* getDataPath) (void);
	HERROR			(* readData) (HUID uid, const HCHAR *pszLink, void **ppvData, HUINT32 *size);
	HERROR			(* writeData) (HUID uid, const HCHAR *pszLink, const void *pvData, HUINT32 size);
	HERROR			(* getLinkName) (HUID uid, const void *pvData, HUINT32 size, void *name, HUINT32 nameSize);
	HERROR			(* syncData) (void);
	const HCHAR *	(* encodeJsonBin) (HxJSONBIN_t enc, const void *pstData, HINT32 nSize);
	const void  *	(* decodeJsonBin) (void *pvData, HxJSON_t json, HINT32 nSize);
	HERROR			(* onStorageAdded) (DxStorage_Info_t *pstStorage);
	HERROR			(* onStorageRemoved) (DxStorage_Info_t *pstStorage);
	void			(* print) (FILE *fp, const void *t);
} DxObject_t;

typedef	HERROR	(*DxDECFunc)(const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize);

HERROR		dlib_RegisterObject   (const DxObject_t *object);
HERROR		dlib_RegisterEnumTable(const DxEnumStringTable_t *pstEnumTable);
HERROR		dlib_RegisterEnumTable_deprecated(const DxEnumStringTable_t *pstEnumTable);
const HCHAR*	dlib_ConvertEnum2Str(const HCHAR *enumType, HINT32 enumValue);
HINT32		dlib_ConvertStr2Enum(const HCHAR *enumType, const HCHAR *enumName);
HERROR		dlib_RegisterDECModule(const HCHAR *decType, DxDECFunc encoder, DxDECFunc decoder);
DxDECFunc	dlib_GetDECModule(const HCHAR *decType, HBOOL bEncoder);


#ifdef __cplusplus
}
#endif

#endif // _DLIB_H


