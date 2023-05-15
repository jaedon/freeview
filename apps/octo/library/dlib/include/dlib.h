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

//	��� Type�� dependency�� ������. --> ���� �ʿ�.
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


//	dlib_xxx.h�� �Ű� ���� ������ ���.
#include <oapi_types.h>

/**
 * �ʱ�ȭ
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
 * DB Table�� File Write Sync Time.
 *
 * @param	typeName	type name
 * @return	sync cycle in millisec
 */
HUINT32			DLIB_GetSyncTime (const HCHAR *typeName);

/**
 * DB Table�� ������ ���� �Ӱ�ġ
 *
 * @param	typeName	type name
 * @return	threshold
 */
HUINT32			DLIB_GetThreshold (const HCHAR *typeName);

/**
 * DB Table�� ������ ���� �Ӱ�ġ
 *
 * @param	typeName	type name
 * @return	thresholdSize
 */
HUINT64			DLIB_GetThresholdSize (const HCHAR *typeName);

/**
 * Push notification cycle
 * ���� 0�� �����ϸ�, 1�� �̳��� ������ ��ȭ�� ���� ������ ��ٷȴٰ� Push�� �ö��
 *
 * @param	typeName	type name
 * @return	cycle in millisec
 */
HUINT32			DLIB_GetPushCycle (const HCHAR *typeName);

/**
 * DB Table�� Field ���� ����
 */
HINT32			DLIB_GetFieldCount (const HCHAR *typeName);

/**
 * get field for DB
 *
 * src �� NULL �Ǵ� srcSize == 0�̸�, DxField���� value�� ä������ �ʴ´�.
 */
#if !defined(CONFIG_TEMP_DLIB_NO_AUTOMATIC_TRASHING)
const DxField_t	*	DLIB_GetField (const HCHAR *typeName, HINT32 index, const void *src, HUINT32 srcSize);
#else
const DxField_t	*	DLIB_CreateGetFieldData (const HCHAR *typeName, HINT32 index, const void *src, HUINT32 srcSize);
HERROR				DLIB_DestroyGetFieldData (const DxField_t *field);
#endif


/**
 * ���� ���� �뵵�� ������ ���ڵ�
 *
 * @param	typeName	type name
 * @param	uid			unique id
 * @param	src			���ڵ� �� ������
 * @param	srcSize		���ڵ� �� ������ ������
 * @param	dst			���ڵ� �� ������ (const �� free�ϸ� �ȵǿ�)
 * @param	dstSize		���ڵ� �� ������ ������
 * @return	ERR_OK / ...
 */
#if !defined(CONFIG_TEMP_DLIB_NO_AUTOMATIC_TRASHING)
HERROR			DLIB_Encode (const HCHAR *typeName, HUID uid, const void *src, HUINT32 srcSize, const void **dst, HUINT32 *dstSize);
#else
HERROR			DLIB_CreateEncodedData (const HCHAR *typeName, HUID uid, const void *src, HUINT32 srcSize, const void **dst, HUINT32 *dstSize);
HERROR			DLIB_DestroyEncodedData (const void *dst);
#endif

/**
 * ���� ���� �뵵�� ���ڵ��� ������ ���ڵ�
 *
 * @param	typeName	type name
 * @param	uid			unique id
 * @param	src			���ڵ� �� ������
 * @param	srcSize		���ڵ� �� ������ ������
 * @param	dst			���ڵ� �� ������ (const �� free�ϸ� �ȵǿ�)
 * @param	dstSize		���ڵ� �� ������ ������
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
 * data�� ���� �����ϴ� ��쿡 ���� API
 */
 HERROR			DLIB_GetLinkName(const HCHAR *typeName, HUID uid, const void *pvData, HUINT32 ulSize, void *pvName, HUINT32 ulNameSize);
HERROR			DLIB_WriteData(const HCHAR *typeName, HUID uid, const HCHAR *pszLink, const void *pvData, HUINT32 ulSize);
HERROR			DLIB_ReadData(const HCHAR *typeName, HUID uid, const HCHAR *pszLink, void **ppvData, HUINT32 *pulSize);

HERROR			DLIB_SyncData(const HCHAR *typeName);

/**
 * ���� src �����͸� �������� Ÿ���� �����ؾ� �ϴ� ��� (ex. binary -> DxEvent_t)
 *
 */
void *			DLIB_CloneType (const HCHAR *typeName, void *src, HINT32 srcSize);
void *			DLIB_NewType (const HCHAR *typeName, HUID uid, const void *src, HUINT32 srcSize);
void			DLIB_FreeType (const HCHAR *typeName, void *type);
DxFreeFunc_t	DLIB_GetFreeFunc (const HCHAR *typeName);

/**
 * ��ϵ� ObjectMAP�� ObjectName�� ���������� �����ش�.
 *
 * @param	func	callback
 *				- objectName	type name
 *				- tags  		tag�� ��ϵǾ� �ִ� ��� tag array(NULL terminated array)
 *				- userdata
 * @param	userdata	func callback���� pass ��
 */
void			DLIB_ForEachObject(HBOOL (*func)(const HCHAR *objectName, const HCHAR *tags[], void *userdata), void *userdata);

/**
 * Ÿ�� �����
 */
HINT32			DLIB_Print (const HCHAR *typeName, FILE *fp, const void *data);


// TODO: DO NOT USE FOLLOWING FUNCTIONS
/**
 * �־��� Data�� ���� BASE64������ JSON �� ���� �ְų� ����ȭ �Ѵ�.
 */
const HCHAR*	DLIB_EncodeJSONBin(const HCHAR *typeName, HxJSONBIN_t enc, const void *pstData, HINT32 nSize);
const void*		DLIB_DecodeJSONBin(const HCHAR *typeName, void *type, HxJSON_t json, HINT32 nBinSize);


/**
 * HDD/USB ���� Storage Mount/Insertion � ���� �����ϱ� ���� �Լ�
 *
 * @param	typeName	type name
 * @param	pstStorage	Inserted device : DxStorage_Info_t
 * @return	ERR_OK / ...
 */
HERROR			DLIB_OnStorageAdded (const HCHAR *typeName, DxStorage_Info_t *pstStorage);

/**
 * HDD/USB ���� Storage Unmount/Remove � ���� �����ϱ� ���� �Լ�
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


