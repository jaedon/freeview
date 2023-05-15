

#ifndef	__DB_PARAM_INT_H__
#define	__DB_PARAM_INT_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include "_db_main.h"
#include "local_include/_db_param_default.h"
#include <pdb_param.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

#define dbParam_SET_FLAG(bwNotiFlag, eValue)		bwNotiFlag |= (eValue)
#define dbParam_RESET_FLAG(bwNotiFlag, eValue)		bwNotiFlag &= ~(eValue)
#define dbParam_CHECK_FLAG(bwNotiFlag, eValue)		(HBOOL)((bwNotiFlag & (eValue)) ? TRUE : FALSE)

#define	dbParam_VERSION								0x00010000

#define	dbParam_ITEM_MIGRATION_FLAG					"param_init"

#define	dbParam_2ND_INT_HDD_UUID_MAX				37	//SVC_FS_DEV_UUID_BUFSIZE	: stb_params.h∑Œ ¿Ãµø???

#define dbParam_PostMsgToMgr(a, b, c, d, e)			db_main_NotifyUpdate(a, b, (HUINT32)c, (HUINT32)d, (HUINT32)e)

#define	dbParam_IsDama(eType)																		\
	((eType == eDB_STORAGE_TYPE_DAMA_INT || eType == eDB_STORAGE_TYPE_DAMA_STR ||					\
		eType == eDB_STORAGE_TYPE_DAMA_ENUM || eType == eDB_STORAGE_TYPE_DAMA_BIN) ? TRUE : FALSE)

#define	dbParam_IsRegistry(eType)																		\
		((eType == eDB_STORAGE_TYPE_REG_INT || eType == eDB_STORAGE_TYPE_REG_STR ||					\
			eType == eDB_STORAGE_TYPE_REG_ENUM) ? TRUE : FALSE)

#define	dbParam_ENTER_CRITICAL()		VK_SEM_Get(db_param_GetSemId())
#define	dbParam_LEAVE_CRITICAL()		VK_SEM_Release(db_param_GetSemId())
#define	dbParam_GetCacheHandle()		db_param_GetCacheHandle()

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eDB_STORAGE_TYPE_REG_INT,
	eDB_STORAGE_TYPE_REG_STR,
	eDB_STORAGE_TYPE_REG_ENUM,
	eDB_STORAGE_TYPE_DAMA_INT,
	eDB_STORAGE_TYPE_DAMA_STR,
	eDB_STORAGE_TYPE_DAMA_ENUM,
	eDB_STORAGE_TYPE_DAMA_BIN,
	eDB_STORAGE_TYPE_MAX,
} dbStorageType_e;

typedef struct
{
	dbStorageType_e 	 eType;
	HINT32				 nMsg;			// Message To Notify
	HUINT32				 ulRetValue;
	union
	{
		HCHAR					*pszKey;
		DxDBSETUP_Item_e		 eKey;
		/* eDB_STORAGE_TYPE_REG_ENUM */
		struct
		{
			HCHAR				*pszKey;
			HCHAR				*pszEnumType;
			HINT32				 eEnumValue;
		} utRegEnum;
		/* eDB_STORAGE_TYPE_REG_INT */
		struct
		{
			HCHAR				*pszKey;
			HINT32				 nValue;
		} utRegInt;
		struct
		{
			HCHAR				*pszKey;
			HCHAR				*pszValue;
			HUINT32				 ulBufLen;
		} utRegStr;
		struct
		{
			DxDBSETUP_Item_e	eKey;
			HINT32				nValue;
		} utDamaInt;
		struct
		{
			DxDBSETUP_Item_e	 eKey;
			HCHAR				*pszEnumType;
			HINT32				 eEnumValue;
		} utDamaEnum;
		struct
		{
			DxDBSETUP_Item_e	 eKey;
			HCHAR				*pszString;
			HUINT32				 ulBufLen;
		} utDamaStr;
		struct
		{
			DxDBSETUP_Item_e	 eKey;
			void				*pvData;
			HUINT32				 ulBufLen;
			HBOOL				 bCompare;
		} utDamaBin;
	} utParam;
} dbParam_ItemConfig_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
extern HUINT32			 db_param_GetSemId(void);
extern Handle_t			 db_param_GetCacheHandle(void);
extern HxCBUF_t			 db_param_GetCbufHandle(void);
extern HERROR			 db_param_GetCfgItemValue(const dbParam_CfgItemInfo_t *pItemInfo, HUINT32 *pulData, HUINT32 *pulLength);
extern void				 db_param_GetCbufSem (void);
extern void				 db_param_ReleaseCbufSem (void);

/*	User Data
 */
extern void				*db_param_GetUserData(void);
extern void				 db_param_SetUserData(void *pvUserData, void (* pfnFreeCb)(void *));

/*	Table
 */
extern DbParam_Item_e	 db_param_TableGetParamItemByDxItem(const DxDBSETUP_Item_e eDxItem);
extern DbParam_Item_e	 db_param_TableGetParamItemByRegKey(const HCHAR *pszKey);
extern HERROR			 db_param_TableGetDataType(DbParam_Item_e eItem, DbParam_Type_e *peDataType);
extern HERROR			 db_param_TableConfigItem(dbParam_ItemConfig_t *pstConfig, HBOOL bSet, DbParam_Item_e eItem, HUINT32 ulData, HUINT32 ulParam);

/*	Default Value
 */
extern dbParam_DefaultItem_t	*db_param_DefaultValueGetItemByParamKey(const DbParam_Item_e eItem);
extern dbParam_DefaultItem_t	*db_param_DefaultValueGetItemByDxKey(const DxDBSETUP_Item_e eItem);
extern dbParam_DefaultItem_t	*db_param_DefaultValueGetItemByRegKey(const HCHAR *pszKey);

/*	Cache
 */
extern Handle_t			 db_param_CacheInit(void);
extern HxTREE_t			*db_param_CacheGetTreeFromHandle(Handle_t hCache);
extern HERROR			 db_param_CacheRemoveAll(Handle_t hCache);

extern DxDBSetupItem_t	*db_param_CacheSetItemValue(Handle_t hCache, DxDBSETUP_Item_e eItem, HINT32 nValue);
extern DxDBSetupItem_t	*db_param_CacheGetItemValue(Handle_t hCache, DxDBSETUP_Item_e eItem, HINT32 *pnValue);

extern DxDBSetupItem_t	*db_param_CacheSetItemText(Handle_t hCache, DxDBSETUP_Item_e eItem, HCHAR *pszText);
extern DxDBSetupItem_t	*db_param_CacheGetItemText(Handle_t hCache, DxDBSETUP_Item_e eItem, HINT32 nLimit, HCHAR *pszText);

extern DxDBSetupItem_t	*db_param_CacheSetItemBinary(Handle_t hCache, DxDBSETUP_Item_e eItem, HINT32 size, void *pvData);
extern DxDBSetupItem_t	*db_param_CacheGetItemBinary(Handle_t hCache, DxDBSETUP_Item_e eItem, HINT32 nLimit, HINT32 *nDataSize, void *pvData);

/*	Dama
 */
extern HERROR db_param_DamaInit (Handle_t hCache);

extern HERROR db_param_DamaRemoveAll (void);

extern HERROR db_param_DamaSetConfigItem(const dbParam_ItemConfig_t *pstConfig);

extern HERROR db_param_DamaSetInt(const DxDBSETUP_Item_e eItem, HINT32 nValue, HINT32 nMsg);
extern HERROR db_param_DamaGetInt(const DxDBSETUP_Item_e eItem, HUINT32 *pulData);

extern HERROR db_param_DamaSetEnum(const DxDBSETUP_Item_e eItem, const char *pszEnumType, HINT32 eEnumValue, HINT32 nMsg);
extern HERROR db_param_DamaGetEnum(const DxDBSETUP_Item_e eItem, const HCHAR *pszEnumType, HUINT32 *pulData);

extern HERROR db_param_DamaSetStr(const DxDBSETUP_Item_e eItem, const HCHAR *pszString, HUINT32 ulBufLen, HINT32 nMsg);
extern HERROR db_param_DamaGetStr(const DxDBSETUP_Item_e eItem, HCHAR *pszString, HUINT32 ulBufLen);

extern HERROR db_param_DamaSetBin(const DxDBSETUP_Item_e eItem, void *pvData, HUINT32 ulSize, HBOOL bComp ,HINT32 nMsg);
extern HERROR db_param_DamaGetBin(const DxDBSETUP_Item_e eItem, void *pvData, HUINT32 ulSize);

/*	Registry
 */

extern HERROR db_param_RegSetConfigItem(const dbParam_ItemConfig_t *pstConfig);

extern HERROR db_param_RegGetDefaultValue(const HCHAR *pszKey, HUINT32 *pulData);

extern HERROR db_param_RegSetInt(const HCHAR *pszKey, HINT32 nValue, HINT32 nMsg);
extern HERROR db_param_RegGetInt(const HCHAR *pszKey, HUINT32 *pulData);

extern HERROR db_param_RegSetEnum(const HCHAR *pszKey, const HCHAR *pszEnumType, HINT32 nValue, HINT32 nMsg);
extern HERROR db_param_RegGetEnum(const HCHAR *pszKey, const HCHAR *pszEnumType, HUINT32 *pulData);

extern HERROR db_param_RegSetStr(const HCHAR *pszKey, const HCHAR *pszValue, HINT32 nMsg);
extern HERROR db_param_RegGetStr(const HCHAR *pszKey, HCHAR *pszString, HUINT32 ulSrcBufLen);
extern HERROR db_param_RegGetLang(const HCHAR *pszKey, HCHAR *pszString, HUINT32 ulSrcBufLen);

extern HERROR db_param_RegRemoveAll(void);

/*	Migration
 */
extern HERROR db_param_MigrateItems(void);

#if defined(CONFIG_DEBUG)
extern void db_param_ReadAndPrintItemConfigValue(DbParam_Item_e eItem, const dbParam_ItemConfig_t *pstConfig);
#endif


/*	Keeping Item
 */
extern HERROR db_param_KeepingItemInit(void);
extern HERROR db_param_KeepingItemNotifyDamaRemoved(void);
extern HERROR db_param_KeepingItemLoadDefault(void);
extern HBOOL db_param_FindKeepingConfigItem(const DxDBSETUP_Item_e eDxItem);



#endif /* __DB_PARAM_INT_H__ */

