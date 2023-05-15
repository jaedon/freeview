/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  db_param.c
	@brief

	Description:  \n
	Module: db/param \n
	Remarks : 										\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <dapi.h>
#include <hapi.h>

#include <db_param.h>
#include "../local_include/_db_main.h"
#include "_db_param.h"
#include <pdb_param.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HUINT32						 ulDBSemId;
	Handle_t 					 hCache;
	HxCBUF_t 					 hDbParamCBuf;
	HUINT32						 ulDbParamCBufSem;
	void						*pvUserData;
	void						(* pfnFreeCb)(void *pvUserData);
} dbParam_Context_t;

/*******************************************************************/
/********************      Local Variables      ********************/
/*******************************************************************/

STATIC dbParam_Context_t	s_stDbParamContext;

/*******************************************************************/
/********************      Local Functions      ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

/*	Common Functions
 */

STATIC void db_param_SetCbufHandle(HxCBUF_t hCBuf)
{
	s_stDbParamContext.hDbParamCBuf = hCBuf;
}

HxCBUF_t db_param_GetCbufHandle(void)
{
	return s_stDbParamContext.hDbParamCBuf;
}

STATIC void			db_param_InitBufSem (void)
{
	HxSEMT_Create(&s_stDbParamContext.ulDbParamCBufSem,"dbparambuf",HxSEMT_FIFO);
}

void			db_param_GetCbufSem (void)
{
	HxSEMT_Get(s_stDbParamContext.ulDbParamCBufSem);
}

void			db_param_ReleaseCbufSem (void)
{
	HxSEMT_Release(s_stDbParamContext.ulDbParamCBufSem);
}

HERROR db_param_GetCfgItemValue(const dbParam_CfgItemInfo_t *pstItemInfo, HUINT32 *pulData, HUINT32 *pulLength)
{
	HERROR		hErr = ERR_OK;

	if (NULL == pstItemInfo)
		return ERR_FAIL;

	if (NULL == pulData)
		return ERR_FAIL;

	if (pstItemInfo->nDataSize)
	{
		*pulData = (HUINT32)pstItemInfo->pucData;
		if (pulLength)
		{
			*pulLength = pstItemInfo->nDataSize;
		}
	}
	else if (HxSTD_StrLen(pstItemInfo->szItemText) > 0)
	{
		*pulData = (HUINT32)pstItemInfo->szItemText;
		if (pulLength)
		{
			*pulLength = HxSTD_StrLen(pstItemInfo->szItemText);
		}
	}
	else
	{
		*pulData = (HUINT32)pstItemInfo->nItemValue;
		if (pulLength)
		{
			*pulLength = sizeof(HINT32);
		}
	}

	return hErr;
}

#define	___DEBUG_FUNCTIONS___________________
/***************************************************************************************
 ***************************************************************************************/
#if defined(CONFIG_DEBUG)
STATIC void db_param_PrintDamaValue(DbParam_Item_e eItem, const dbParam_ItemConfig_t *pstConfig)
{
	HERROR						hErr;
	const DxDBSETUP_Item_e		eKey = pstConfig->utParam.eKey;

	if (eKey == 0)
	{
		HxLOG_Error("Invalid Key [0] \n");
		return;
	}

	switch (pstConfig->eType)
	{
	case eDB_STORAGE_TYPE_DAMA_STR:
		{
			HCHAR szValue[64];

			hErr = db_param_DamaGetStr(eKey, szValue, 64);
			if (ERR_OK == hErr)
				HxLOG_Debug("[ParamKey : %4d, Key : %12d] STR  : [%s] \n", eItem, eKey, szValue);
		}
		break;
	case eDB_STORAGE_TYPE_DAMA_ENUM:
		{
			HINT32	eEnum;
			hErr = db_param_DamaGetEnum(eKey, pstConfig->utParam.utDamaEnum.pszEnumType, (HUINT32 *)&eEnum);
			if (ERR_OK == hErr)
				HxLOG_Debug("[ParamKey : %4d, Key : %12d] ENUM : [%0d(0x%08x)] \n", eItem, eKey, pstConfig->utParam.utDamaEnum.pszEnumType, eEnum, eEnum);
		}
		break;
	case eDB_STORAGE_TYPE_DAMA_INT:
		{
			HINT32 nValue;
			hErr = db_param_DamaGetInt(eKey, (HUINT32 *)&nValue);
			if (ERR_OK == hErr)
				HxLOG_Debug("[ParamKey : %4d, Key : %12d] INT  : [%0d(0x%08x)] \n", eItem, eKey, nValue, nValue);
		}
		break;
	case eDB_STORAGE_TYPE_DAMA_BIN:
		{
			if (pstConfig->utParam.utDamaBin.ulBufLen)
			{
				void *pvData = DB_MEM_Alloc(pstConfig->utParam.utDamaBin.ulBufLen);
				if (pvData)
				{
					hErr = db_param_DamaGetBin(eKey, pvData, pstConfig->utParam.utDamaBin.ulBufLen);
					if (ERR_OK == hErr)
					{
						HxLOG_Debug("[ParamKey : %4d, Key : %12d] BIN  : [Addr:%p, Length:%d] \n", eItem, eKey, pvData, pstConfig->utParam.utDamaBin.ulBufLen);
						HLIB_LOG_Dump(pvData, pstConfig->utParam.utDamaBin.ulBufLen, 0, FALSE);
					}
					DB_MEM_Free(pvData);
				}
			}
		}
		break;
	default:
		break;
	}
}

STATIC void db_param_PrintRegValue(DbParam_Item_e eItem, const dbParam_ItemConfig_t *pstConfig)
{
	HERROR	hErr;

	if (NULL == pstConfig->utParam.pszKey)
	{
		HxLOG_Error("Invalid Key [NULL] \n");
		return;
	}

	if (pstConfig->eType == eDB_STORAGE_TYPE_REG_STR)
	{
		HCHAR szValue[64];
		hErr = HAPI_GetRegistryStr(pstConfig->utParam.pszKey, szValue, 64);
		if (ERR_OK == hErr)
			HxLOG_Debug("[ParamKey : %4d, Key : \"%-10s\"] STR  : [%s] \n", eItem, pstConfig->utParam.pszKey, szValue);
	}
	else if (pstConfig->eType == eDB_STORAGE_TYPE_REG_ENUM)
	{
		HINT32 nValue;
		hErr = HAPI_GetRegistryEnum(pstConfig->utParam.pszKey, pstConfig->utParam.utRegEnum.pszEnumType, &nValue);
		if (ERR_OK == hErr)
			HxLOG_Debug("[ParamKey : %4d, Key : \"%-10s\"] ENUM : [%0d(0x%08x)] \n", eItem, pstConfig->utParam.pszKey, nValue, nValue);
	}
	else if (pstConfig->eType == eDB_STORAGE_TYPE_REG_INT)
	{
		HINT32 nValue;
		hErr = HAPI_GetRegistryInt(pstConfig->utParam.pszKey, &nValue);
		if (ERR_OK == hErr)
			HxLOG_Debug("[ParamKey : %4d, Key : \"%-10s\"] INT  : [%0d(0x%08x)] \n", eItem, pstConfig->utParam.pszKey, nValue, nValue);
	}
	else
	{
		HxLOG_Warning("Invalid eType : [%d], ParamKey [%4d] \n", pstConfig->eType, eItem);
	}
}

void db_param_ReadAndPrintItemConfigValue(DbParam_Item_e eItem, const dbParam_ItemConfig_t *pstConfig)
{
	if (dbParam_IsDama(pstConfig->eType))
	{
		db_param_PrintDamaValue(eItem, pstConfig);
	}
	else
	{
		db_param_PrintRegValue(eItem, pstConfig);
	}
}
#endif


#define	___GLOBAL_FUNCTIONS___________________
/***************************************************************************************
 ***************************************************************************************/

HUINT32 db_param_GetSemId(void)
{
	return s_stDbParamContext.ulDBSemId;
}

Handle_t db_param_GetCacheHandle(void)
{
	return s_stDbParamContext.hCache;
}

void *db_param_GetUserData(void)
{
	return s_stDbParamContext.pvUserData;
}

void db_param_SetUserData(void *pvUserData, void (* pfnFreeCb)(void *))
{
	s_stDbParamContext.pvUserData = pvUserData;
	s_stDbParamContext.pfnFreeCb = pfnFreeCb;
}

//	DB_PARAM 包府林眉绰 Obama, 历厘林眉绰 DAMA or HAMA
HERROR db_param_Init (void)
{
	HERROR	hErr;

	HxSTD_MemSet(&s_stDbParamContext, 0x00, sizeof(dbParam_Context_t));

	// 1. Create Buffer
	HxLOG_Debug("1. Create Buffer\n");
	db_param_SetCbufHandle(HLIB_CBUF_New(4096));
	db_param_InitBufSem();

	// 2. Create Semaphore
	HxLOG_Debug("2. Create Semaphore\n");
	if (VK_SEM_Create((unsigned long*)&s_stDbParamContext.ulDBSemId, "dbsem", VK_SUSPENDTYPE_FIFO) != VK_OK)
	{
		HxLOG_Error("Error : Can't not create Semaphore!\n");
		return ERR_FAIL;
	}

	// 3. Create Cache (HxTree)
	HxLOG_Debug("3. Create Cache (HxTree)\n");
	s_stDbParamContext.hCache = db_param_CacheInit();
	if (HANDLE_NULL == s_stDbParamContext.hCache)
	{
		HxLOG_Error("Error : Can't not create hCache!\n");
		return ERR_FAIL;
	}

	// 4. Init Dama Connection : Load Data set Notifier
	HxLOG_Debug("4. Init Dama Connection : Load Data set Notifier\n");
	hErr = db_param_DamaInit(s_stDbParamContext.hCache);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("Error : db_param_DamaInit() Fail!\n");
		return ERR_FAIL;
	}

	// 5. Migrate Data
	HxLOG_Debug("5. Migrate Data\n");
	hErr = db_param_MigrateItems();
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("Error : db_param_MigrateItems() Fail!\n");
	}

	// 6. Init Keeping Item
	HxLOG_Debug("6. Init Keeping Item\n");
	hErr = db_param_KeepingItemInit();
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("Error : db_param_KeepingItemInit() Fail!\n");
	}

	return hErr;
}

HERROR DB_PARAM_SetItem(DbParam_Item_e eItem, HUINT32 ulData, HUINT32 ulParam)
{
	HERROR					hErr = ERR_FAIL;
	dbParam_ItemConfig_t	stItem;

	HxSTD_MemSet(&stItem, 0, sizeof(dbParam_ItemConfig_t));

	hErr = db_param_TableConfigItem(&stItem, TRUE, eItem, ulData, ulParam);
	if (ERR_OK == hErr)
	{
		hErr = ERR_FAIL;
		switch (stItem.eType)
		{
		case eDB_STORAGE_TYPE_REG_INT:
			hErr = db_param_RegSetInt(stItem.utParam.pszKey,	stItem.utParam.utRegInt.nValue, 					stItem.nMsg);
			break;
		case eDB_STORAGE_TYPE_REG_STR:
			hErr = db_param_RegSetStr(stItem.utParam.pszKey,	(const HCHAR *)stItem.utParam.utRegStr.pszValue,	stItem.nMsg);
			break;
		case eDB_STORAGE_TYPE_REG_ENUM:
			hErr = db_param_RegSetEnum(stItem.utParam.pszKey,	(const HCHAR *)stItem.utParam.utRegEnum.pszEnumType,
																stItem.utParam.utRegEnum.eEnumValue,				stItem.nMsg);
			break;
		case eDB_STORAGE_TYPE_DAMA_INT:
			hErr = db_param_DamaSetInt(stItem.utParam.eKey,		stItem.utParam.utDamaInt.nValue,					stItem.nMsg);
			break;
		case eDB_STORAGE_TYPE_DAMA_STR:
			hErr = db_param_DamaSetStr(stItem.utParam.eKey, 	(const HCHAR *)stItem.utParam.utDamaStr.pszString,
																stItem.utParam.utDamaStr.ulBufLen,					stItem.nMsg);
			break;
		case eDB_STORAGE_TYPE_DAMA_ENUM:
			hErr = db_param_DamaSetEnum(stItem.utParam.eKey,	(const HCHAR *)stItem.utParam.utDamaEnum.pszEnumType,
																stItem.utParam.utDamaEnum.eEnumValue,				stItem.nMsg);
			break;
		case eDB_STORAGE_TYPE_DAMA_BIN:
			if ((0 < stItem.utParam.utDamaBin.ulBufLen) && (NULL != stItem.utParam.utDamaBin.pvData))
			{
				hErr = db_param_DamaSetBin(stItem.utParam.eKey,		stItem.utParam.utDamaBin.pvData,
																	stItem.utParam.utDamaBin.ulBufLen,
																	stItem.utParam.utDamaBin.bCompare,				stItem.nMsg);
			}
			else
			{
				HxLOG_Error("eDB_STORAGE_TYPE_DAMA_BIN : Invalid Binary Size 0 \n");
			}
			break;
		default:
			hErr = ERR_FAIL;
			break;
		}
	}

	return hErr;
}

HERROR DB_PARAM_GetItem(DbParam_Item_e eItem, HUINT32 *pulData, HUINT32 ulParam)
{
	HERROR					hErr = ERR_FAIL;
	dbParam_ItemConfig_t	stConfig;

	if (NULL == pulData)
	{
		return ERR_FAIL;
	}

	hErr = db_param_TableConfigItem(&stConfig, FALSE, eItem, 0, ulParam);
	if (ERR_OK == hErr)
	{
		hErr = ERR_FAIL;
		switch (stConfig.eType)
		{
		case eDB_STORAGE_TYPE_REG_INT:
			hErr = db_param_RegGetInt(stConfig.utParam.pszKey, pulData);
			break;
		case eDB_STORAGE_TYPE_REG_STR:
			switch(eItem)
			{
			case eDB_PARAM_ITEM_LANG_MAIN:
			case eDB_PARAM_ITEM_LANG_EVENT:
			case eDB_PARAM_ITEM_LANG_AUDIO0:
			case eDB_PARAM_ITEM_LANG_AUDIO1:
			case eDB_PARAM_ITEM_LANG_AUDIO2:
			case eDB_PARAM_ITEM_LANG_AUDIO3:
			case eDB_PARAM_ITEM_LANG_AUDIO4:
			case eDB_PARAM_ITEM_LANG_AUDIO5:
			case eDB_PARAM_ITEM_LANG_AUDIO6:
			case eDB_PARAM_ITEM_LANG_AUDIO7:
			case eDB_PARAM_ITEM_LANG_AUDIO_UNDEFINED:
			case eDB_PARAM_ITEM_LANG_AUDIO_ORIGINAL:
			case eDB_PARAM_ITEM_LANG_AUDIO_DEFAULT:
			case eDB_PARAM_ITEM_LANG_SUBTITLE0:
			case eDB_PARAM_ITEM_LANG_SUBTITLE1:
			case eDB_PARAM_ITEM_LANG_SUBTITLE2:
			case eDB_PARAM_ITEM_LANG_SUBTITLE3:
			case eDB_PARAM_ITEM_LANG_SUBTITLE4:
			case eDB_PARAM_ITEM_LANG_SUBTITLE5:
			case eDB_PARAM_ITEM_LANG_SUBTITLE6:
			case eDB_PARAM_ITEM_LANG_SUBTITLE7:
			case eDB_PARAM_ITEM_LANG_SUBTITLE_UNDEFINED:
			case eDB_PARAM_ITEM_LANG_SUBTITLE_ORIGINAL:
			case eDB_PARAM_ITEM_LANG_SUBTITLE_DEFAULT:
				{
					HCHAR	*pszLanguage = NULL;
					db_param_GetCbufSem();
					pszLanguage = HLIB_CBUF_GetBuffer(db_param_GetCbufHandle(), 32);
					db_param_ReleaseCbufSem();
					hErr = db_param_RegGetLang(stConfig.utParam.pszKey, (HCHAR *)pszLanguage, 32);
					if (ERR_OK == hErr)
					{
						*pulData = (HUINT32)HLIB_LANG_639ToId(pszLanguage);
					}
				}
				break;
			default:
				hErr = db_param_RegGetStr(stConfig.utParam.pszKey,	(HCHAR *)pulData, stConfig.utParam.utRegStr.ulBufLen);
				break;
			}
			break;
		case eDB_STORAGE_TYPE_REG_ENUM:
			hErr = db_param_RegGetEnum(stConfig.utParam.pszKey,	(const HCHAR *)stConfig.utParam.utRegEnum.pszEnumType, pulData);
			break;
		case eDB_STORAGE_TYPE_DAMA_INT:
			hErr = db_param_DamaGetInt(stConfig.utParam.eKey,	pulData);
			break;
		case eDB_STORAGE_TYPE_DAMA_STR:
			hErr = db_param_DamaGetStr(stConfig.utParam.eKey,	(HCHAR *)pulData, stConfig.utParam.utDamaStr.ulBufLen);
			break;
		case eDB_STORAGE_TYPE_DAMA_ENUM:
			hErr = db_param_DamaGetEnum(stConfig.utParam.eKey,	(const HCHAR *)stConfig.utParam.utDamaEnum.pszEnumType, pulData);
			break;
		case eDB_STORAGE_TYPE_DAMA_BIN:
			hErr = db_param_DamaGetBin(stConfig.utParam.eKey,	(void *)pulData, stConfig.utParam.utDamaBin.ulBufLen);
			break;
		default:
			hErr = ERR_FAIL;
			break;
		}
	}

	return hErr;
}

HERROR	DB_PARAM_GetDefaultItem (DbParam_Item_e eItem, HUINT32 *pulData)
{
	HERROR					 hErr;
	dbParam_DefaultItem_t	*pstItem;
	HUINT32					 ulValue;

	if (NULL == pulData)
		return ERR_FAIL;

	pstItem = db_param_DefaultValueGetItemByParamKey((const DbParam_Item_e)eItem);
	if (NULL == pstItem)
	{
		return ERR_FAIL;
	}

	ulValue = 0;
	hErr = db_param_GetCfgItemValue((const dbParam_CfgItemInfo_t *)&pstItem->stInfo, &ulValue, NULL);
	if (ERR_OK != hErr)
		return ERR_FAIL;

	// In Case Of String or Binary ulValue should be static Pointer.(Harcoded Data)
	// No Need To make clone;
	*pulData = ulValue;

	return ERR_OK;
}

HERROR	DB_PARAM_GetItemType(DbParam_Item_e eItem, DbParam_Type_e *peDataType)
{
	if (NULL == peDataType)
	{
		return ERR_FAIL;
	}

	return db_param_TableGetDataType(eItem, peDataType);
}

void   DB_PARAM_Close(void)
{
	DAPI_Sync(DxDBSETUP_SYSTEM);
}

HERROR DB_PARAM_LoadDefault(HBOOL bKeepFactoryValue)
{
	HERROR	hErr = ERR_OK;

	(void)bKeepFactoryValue;

	// 1. Load Default Value
	HxLOG_Debug("1. Load Default Value !!!!!!!!!\n");
	hErr = db_param_KeepingItemLoadDefault();
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("db_param_KeepingItemLoadDefault() Fail [%d]\n", hErr);
	}

	// 2. Remove DB Setup Table from DAMA
	HxLOG_Debug("2. Remove DAMA Setup Table !!!!!!!!!\n");
	hErr = db_param_DamaRemoveAll();
	if (ERR_OK != hErr)
	{
		HxLOG_Critical("db_param_DamaRemoveAll Fail [%d]\n", hErr);
		HxLOG_Assert(0);
	}
	else
	{
		hErr = db_param_KeepingItemNotifyDamaRemoved();
	}

	// 3. Remove Registry Obama's Item
	HxLOG_Debug("3. Remove Registry Obama's Item !!!!!!!!!\n");
	hErr |= db_param_RegRemoveAll();	// Remove Registry
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("db_param_RegRemoveAll() Fail [%d]\n", hErr);
	}

	return hErr;
}

HERROR DB_PARAM_Sync(void)
{
//	DAPI_Sync(DxDBSETUP_SYSTEM);

//	pdb_svc_DamaAdaptorSyncAntennaDB();

	return ERR_OK;
}

HERROR DB_PARAM_RemoveCache(void)
{
	HERROR	hErr;

	hErr = db_param_CacheRemoveAll(db_param_GetCacheHandle());
	if (ERR_OK != hErr)
	{
		HxLOG_Error("db_param_CacheRemoveAll Fail [%d]\n", hErr);
	}

	return hErr;
}



/* end of file */
