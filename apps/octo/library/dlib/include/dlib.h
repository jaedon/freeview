/* $Header: $ */

#ifndef DLIB_H__
#define DLIB_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hlib.h"

#define DLIB_Malloc			HLIB_STD_MemAlloc
#define DLIB_Calloc			HLIB_STD_MemCalloc
#define	DLIB_Realloc		HLIB_STD_MemRealloc
#define	DLIB_StrDup			HLIB_STD_StrDup
#define	DLIB_MemDup			HLIB_STD_MemDup
#define DLIB_Free			HLIB_STD_MemFree
#define	DLIB_Malloc_CB		HLIB_STD_MemAlloc_CB
#define	DLIB_Free_CB		HLIB_STD_MemFree_CB

typedef enum
{
	eDxTYPE_INTEGER,
	eDxTYPE_BINARY,
	eDxTYPE_STRING,
	eDxTYPE_JSON,
	eDxTYPE_OBJECT,
	eDxTYPE_MAX
} DxType_e;

#define	DxLIB_MAX_NAME		128

typedef struct
{
	DxType_e		type;
	HCHAR			name[DxLIB_MAX_NAME];
	HBOOL			indexable;
	union {
		HINT32			integer;
		HCHAR *			string;
		struct {
			void *		data;
			HUINT32		size;
		} binary;
	} u;
} DxField_t;

typedef	struct
{
	HCHAR	*enumName;
	HINT32	enumValue;
} DxEnumItem_t;

typedef	struct
{
	HCHAR			*enumType;
	DxEnumItem_t	pstEnumItems[];
} DxEnumStringTable_t;

typedef void (* DxFreeFunc_t) (void *);

#include <dlib_config.h>
#include <dlib_enum_string.h>

//	몇몇 Type는 dependency가 존재함. --> 수정 필요.
#include <dlib_base_types.h>
#include <dlib_antinfo.h>
#include <dlib_tuneparam.h>
#include <dlib_bouquet.h>
#include <dlib_group.h>
#include <dlib_network.h>
#include <dlib_provider.h>
#include <dlib_service.h>
#include <dlib_transponder.h>
#include <dlib_epg.h>
#include <dlib_schedule.h>
#include <dlib_dbsetup.h>
#include <dlib_copyrightcontrol.h>
#include <dlib_recording.h>
#include <dlib_channel_logo.h>
#include <dlib_fileinfo.h>
#include <dlib_storage.h>
#include <dlib_hotplug.h>
#include <dlib_grouplist.h>
#include <dlib_cas.h>
#include <dlib_swupdate.h>
#include <dlib_dsmcc.h>
#include <dlib_builder.h>
#include <dlib_download.h>


//	dlib_xxx.h로 옮겨 오기 전까지 사용.
#include <oapi_types.h>

/**
 * 초기화
 *
 */
HERROR			DLIB_Init (void);

/**
 //  Get Full Database Name from ObjectName
 //  dupIndex : 0 : for baseblock, 1 : for backup block, 2 : 2nd Backup block
 //  if dupIndex == 0,
 // 	 return NULL : object is controlled by DAMA
 //  else
 // 	 return NULL : object doesn't need n'th backup block
 */
const HCHAR *	DLIB_GetDBPath (const HCHAR *typeName, HINT32 dupIndex);

/**
 * DB Table의 File Write Sync Time.
 *
 * @param	typeName	type name
 * @return	sync cycle in millisec
 */
HUINT32			DLIB_GetSyncTime (const HCHAR *typeName);

/**
 * DB Table의 아이템 개수 임계치
 *
 * @param	typeName	type name
 * @return	threshold
 */
HUINT32			DLIB_GetThreshold (const HCHAR *typeName);

/**
 * DB Table의 아이템 개수 임계치
 *
 * @param	typeName	type name
 * @return	thresholdSize
 */
HUINT64			DLIB_GetThresholdSize (const HCHAR *typeName);

/**
 * Push notification cycle
 * 만약 0을 리턴하면, 1초 이내에 데이터 변화가 없을 때까지 기다렸다가 Push가 올라옴
 *
 * @param	typeName	type name
 * @return	cycle in millisec
 */
HUINT32			DLIB_GetPushCycle (const HCHAR *typeName);

/**
 * DB Table의 Field 개수 리턴
 */
HINT32			DLIB_GetFieldCount (const HCHAR *typeName);

/**
 * get field for DB
 *
 * src 가 NULL 또는 srcSize == 0이면, DxField에서 value를 채워주지 않는다.
 */
#if !defined(CONFIG_TEMP_DLIB_NO_AUTOMATIC_TRASHING)
const DxField_t	*	DLIB_GetField (const HCHAR *typeName, HINT32 index, const void *src, HUINT32 srcSize);
#else
const DxField_t	*	DLIB_CreateGetFieldData (const HCHAR *typeName, HINT32 index, const void *src, HUINT32 srcSize);
HERROR				DLIB_DestroyGetFieldData (const DxField_t *field);
#endif


/**
 * 파일 저장 용도로 데이터 인코딩
 *
 * @param	typeName	type name
 * @param	uid			unique id
 * @param	src			인코딩 할 데이터
 * @param	srcSize		인코딩 할 데이터 사이즈
 * @param	dst			인코딩 된 데이터 (const 로 free하면 안되요)
 * @param	dstSize		인코딩 된 데이터 사이즈
 * @return	ERR_OK / ...
 */
#if !defined(CONFIG_TEMP_DLIB_NO_AUTOMATIC_TRASHING)
HERROR			DLIB_Encode (const HCHAR *typeName, HUID uid, const void *src, HUINT32 srcSize, const void **dst, HUINT32 *dstSize);
#else
HERROR			DLIB_CreateEncodedData (const HCHAR *typeName, HUID uid, const void *src, HUINT32 srcSize, const void **dst, HUINT32 *dstSize);
HERROR			DLIB_DestroyEncodedData (const void *dst);
#endif

/**
 * 파일 저장 용도로 인코딩된 데이터 디코딩
 *
 * @param	typeName	type name
 * @param	uid			unique id
 * @param	src			인코딩 된 데이터
 * @param	srcSize		인코딩 된 데이터 사이즈
 * @param	dst			디코딩 된 데이터 (const 로 free하면 안되요)
 * @param	dstSize		디코딩 된 데이터 사이즈
 * @return	ERR_OK / ...
 */
#if !defined(CONFIG_TEMP_DLIB_NO_AUTOMATIC_TRASHING)
HERROR			DLIB_Decode (const HCHAR *typeName, HUID uid, const void *src, HUINT32 srcSize, const void **dst, HUINT32 *dstSize);
#else
HERROR			DLIB_CreateDecodedData (const HCHAR *typeName, HUID uid, const void *src, HUINT32 srcSize, const void **dst, HUINT32 *dstSize);
HERROR			DLIB_DestroyDecodedData (const HCHAR *typeName, const void *dst);
#endif

HERROR			DLIB_Serialize (const HCHAR *typeName, HUID uid, const void *src, HUINT32 srcSize, const void **dst, HUINT32 *dstSize);

HERROR			DLIB_Deserialize (const HCHAR *typeName, HUID uid, const void *src, HUINT32 srcSize, const void **dst, HUINT32 *dstSize);


HBOOL			DLIB_HasTag (const HCHAR *typeName, const HCHAR *tag);

/**
 * get data path for DB
 *
 */
const HCHAR*	DLIB_GetDataPath(const HCHAR *typeName);

/**
 * data를 따로 저장하는 경우에 대한 API
 */
 HERROR			DLIB_GetLinkName(const HCHAR *typeName, HUID uid, const void *pvData, HUINT32 ulSize, void *pvName, HUINT32 ulNameSize);
HERROR			DLIB_WriteData(const HCHAR *typeName, HUID uid, const HCHAR *pszLink, const void *pvData, HUINT32 ulSize);
HERROR			DLIB_ReadData(const HCHAR *typeName, HUID uid, const HCHAR *pszLink, void **ppvData, HUINT32 *pulSize);

HERROR			DLIB_SyncData(const HCHAR *typeName);

/**
 * 만약 src 데이터를 기준으로 타입을 생성해야 하는 경우 (ex. binary -> DxEvent_t)
 *
 */
void *			DLIB_CloneType (const HCHAR *typeName, void *src, HINT32 srcSize);
void *			DLIB_NewType (const HCHAR *typeName, HUID uid, const void *src, HUINT32 srcSize);
void			DLIB_FreeType (const HCHAR *typeName, void *type);
DxFreeFunc_t	DLIB_GetFreeFunc (const HCHAR *typeName);

/**
 * 등록된 ObjectMAP의 ObjectName을 순차적으로 돌려준다.
 *
 * @param	func	callback
 *				- objectName	type name
 *				- tags  		tag가 등록되어 있는 경우 tag array(NULL terminated array)
 *				- userdata
 * @param	userdata	func callback으로 pass 됨
 */
void			DLIB_ForEachObject(HBOOL (*func)(const HCHAR *objectName, const HCHAR *tags[], void *userdata), void *userdata);

/**
 * 타입 디버깅
 */
HINT32			DLIB_Print (const HCHAR *typeName, FILE *fp, const void *data);


// TODO: DO NOT USE FOLLOWING FUNCTIONS
/**
 * 주어진 Data에 대해 BASE64형식의 JSON 을 돌려 주거나 구조화 한다.
 */
const HCHAR*	DLIB_EncodeJSONBin(const HCHAR *typeName, HxJSONBIN_t enc, const void *pstData, HINT32 nSize);
const void*		DLIB_DecodeJSONBin(const HCHAR *typeName, void *type, HxJSON_t json, HINT32 nBinSize);


/**
 * HDD/USB 등의 Storage Mount/Insertion 등에 맞춰 동작하기 위한 함수
 *
 * @param	typeName	type name
 * @param	pstStorage	Inserted device : DxStorage_Info_t
 * @return	ERR_OK / ...
 */
HERROR			DLIB_OnStorageAdded (const HCHAR *typeName, DxStorage_Info_t *pstStorage);

/**
 * HDD/USB 등의 Storage Unmount/Remove 등에 맞춰 동작하기 위한 함수
 *
 * @param	typeName	type name
 * @param	pstStorage	removed device : DxStorage_Info_t
 * @return	ERR_OK / ...
 */
HERROR			DLIB_OnStorageRemoved (const HCHAR *typeName, DxStorage_Info_t *pstStorage);


/*
	DLIB Converting ENUM2STR or STR2ENUM
*/
const HCHAR*	DLIB_ConvertEnum2Str(const HCHAR *enumType, HINT32 enumValue);
      HINT32	DLIB_ConvertStr2Enum(const HCHAR *enumType, const HCHAR *enumName);

void		DLIB_MemFree(void *p);


#ifdef __cplusplus
}
#endif

#endif // DLIB_H__


