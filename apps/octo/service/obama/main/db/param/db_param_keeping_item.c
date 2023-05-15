/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  db_param_keeping_item.c
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
typedef enum
{
	eDB_PARAM_NOTI_REG		= 0x00000001,
	eDB_PARAM_NOTI_DAMA		= 0x00000002
} dbParam_NotiType_e, dbParam_NotiType_b;

typedef struct
{
	HBOOL					 bKeeping;
	dbParam_KeepingItem_t	*astItem;
	HINT32					 nItemNum;
	dbParam_NotiType_b		 bwNotiFlag;
	HBOOL					 bRegRemoved;
	HBOOL					 bDamaRemoved;
	HBOOL					 bFinished;
} dbParam_KeepingItemConfig_t;

/*******************************************************************/
/********************      Local Variables      ********************/
/*******************************************************************/

STATIC dbParam_KeepingItemConfig_t	s_stKeepingItemInfo;

/*******************************************************************/
/********************      Local Functions      ********************/
/*******************************************************************/
STATIC HERROR db_param_KeepingItemWrite(dbParam_KeepingItem_t *astItem, HINT32 nItemNum);
STATIC HERROR db_param_KeepingItemReset(dbParam_KeepingItemConfig_t *pstInfo);

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

/*	Common Functions
 */
#define	___DEBUG_FUNCTIONS___________________
/***************************************************************************************
 ***************************************************************************************/

#define	___GLOBAL_FUNCTIONS___________________
/***************************************************************************************
 ***************************************************************************************/

STATIC dbParam_KeepingItemConfig_t *db_param_KeepingItemGetContext(void)
{
	return &s_stKeepingItemInfo;
//	return (dbParam_KeepingItemConfig_t *)db_param_GetUserData();
}

STATIC void db_param_KeepingItemPrintConfigItem(DbParam_Item_e eItem, const dbParam_ItemConfig_t *pstConfig)
{
#if defined(CONFIG_DEBUG)
	DxDBSETUP_Item_e	 eKey;
	HCHAR				*pszKey;

	if (NULL == pstConfig)
	{
		return;
	}

	eKey = pstConfig->utParam.eKey;
	pszKey = pstConfig->utParam.pszKey;
	switch(pstConfig->eType)
	{
	case eDB_STORAGE_TYPE_REG_INT:
		HxLOG_Print("[ParamKey : %4d, Reg Key : \"%-10s\"] INT [%d] \n", eItem, pszKey, pstConfig->utParam.utDamaInt.nValue);
		break;
	case eDB_STORAGE_TYPE_REG_STR:
		HxLOG_Print("[ParamKey : %4d, Reg Key : \"%-10s\"] STR [%s] \n", eItem, pszKey, pstConfig->utParam.utDamaStr.pszString);
		break;
	case eDB_STORAGE_TYPE_REG_ENUM:
		HxLOG_Print("[ParamKey : %4d, Reg Key : \"%-10s\"] ENUM [\"%s\", %d] \n", eItem, pszKey, pstConfig->utParam.utDamaEnum.pszEnumType, pstConfig->utParam.utDamaEnum.eEnumValue);
		break;
	case eDB_STORAGE_TYPE_DAMA_INT:
		HxLOG_Print("[ParamKey : %4d, Dama Key : %12d] INT [%d] \n", eItem, eKey, pstConfig->utParam.utDamaInt.nValue);
		break;
	case eDB_STORAGE_TYPE_DAMA_STR:
		HxLOG_Print("[ParamKey : %4d, Dama Key : %12d] STR [%s] \n", eItem, eKey, pstConfig->utParam.utDamaStr.pszString);
		break;
	case eDB_STORAGE_TYPE_DAMA_ENUM:
		HxLOG_Print("[ParamKey : %4d, Dama Key : %12d] ENUM [\"%s\", %d] \n", eItem, eKey, pstConfig->utParam.utDamaEnum.pszEnumType, pstConfig->utParam.utDamaEnum.eEnumValue);
		break;
	case eDB_STORAGE_TYPE_DAMA_BIN:
		HxLOG_Print("[ParamKey : %4d, Dama Key : %12d] BIN [%p, Len:%d] \n", eItem, eKey, pstConfig->utParam.utDamaBin.pvData, (HINT32)pstConfig->utParam.utDamaBin.ulBufLen);
		if (pstConfig->utParam.utDamaBin.pvData && pstConfig->utParam.utDamaBin.ulBufLen)
		{
			HLIB_LOG_Dump(pstConfig->utParam.utDamaBin.pvData, pstConfig->utParam.utDamaBin.ulBufLen, 0, FALSE);
		}
		break;
	default:
		HxLOG_Warning("Invalid Type [%d] ParamKey [%4d] \n", pstConfig->eType, eItem);
		break;
	}
#endif
}

STATIC void db_param_KeepingItemPrintList(dbParam_KeepingItem_t *astList, HINT32 nItemNum, HBOOL bReadFromDb)
{
#if defined(CONFIG_DEBUG)
	if (astList && nItemNum)
	{
		HINT32 					 i;
		DbParam_Item_e			 eItem;
		dbParam_CfgItemInfo_t	*pstInfo;
		HERROR					 hErr;
		dbParam_ItemConfig_t	 stConfig;
		HUINT32					 ulValue, ulLength;

		HxLOG_Print("[%s (%04d)] [Print Keeping Item List From %s][%d] (+) >>>\n", __FUNCTION__, __LINE__, bReadFromDb ? "DB" : "astList", nItemNum);

		for (i=0; i<nItemNum; i++)
		{
			eItem = astList[i].eItem;
			pstInfo = &astList[i].stInfo;

			ulValue = ulLength = 0;
			(void)db_param_GetCfgItemValue((const dbParam_CfgItemInfo_t *)pstInfo, &ulValue, &ulLength);

//			HxLOG_Error("[DbParam_Item_e : %4d, Value : %4d(0x%08x), Length : %d]\n", eItem, ulValue, ulValue, ulLength);
			hErr = db_param_TableConfigItem(&stConfig, TRUE, eItem, ulValue, ulLength);
			if (ERR_OK == hErr)
			{
				if (bReadFromDb)
				{
					db_param_ReadAndPrintItemConfigValue(eItem, (const dbParam_ItemConfig_t *)&stConfig);
				}
				else
				{
					db_param_KeepingItemPrintConfigItem(eItem, (const dbParam_ItemConfig_t *)&stConfig);
				}
			}
			else
			{
				CHECK;
			}
		}

		HxLOG_Print("[%s (%04d)] [Print Keeping Item List From %s] (-) >>>\n", __FUNCTION__, __LINE__, bReadFromDb ? "DB" : "astList", nItemNum);
	}
#endif
}

STATIC HERROR db_param_KeepingItemWrite(dbParam_KeepingItem_t *astItem, HINT32 nItemNum)
{
	HERROR							 hErr, hRes;
	dbParam_ItemConfig_t			 stConfig;
	const dbParam_KeepingItem_t		*pstKeepingItem;
	HINT32							 i;
	HUINT32							 ulValue, ulLength;

	if (NULL == astItem)
	{
		HxLOG_Error("NULL == astItem\n");
		return ERR_FAIL;
	}

	HxLOG_Debug("nItemNum [%d]\n", nItemNum);

	db_param_KeepingItemPrintList(astItem, nItemNum, FALSE);

	hRes = ERR_OK;

	// Restore Dama and Registry
	for (i = 0; i < nItemNum; i++)
	{
		pstKeepingItem = (const dbParam_KeepingItem_t *)&astItem[i];

		ulValue = ulLength = 0;
		(void)db_param_GetCfgItemValue((const dbParam_CfgItemInfo_t *)&pstKeepingItem->stInfo, &ulValue, &ulLength);

//		HxLOG_Print("[%s (%04d)] [DbParam_Item_e : %4d, Value : %4d(%p), Length : %d]\n", __FUNCTION__, __LINE__, pstKeepingItem->eItem, ulValue, ulValue, ulLength);
		hErr = db_param_TableConfigItem(&stConfig, TRUE, pstKeepingItem->eItem, ulValue, ulLength);
		if (ERR_OK == hErr)
		{
			const dbParam_ItemConfig_t	*pstConfigItem = (const dbParam_ItemConfig_t *)&stConfig;

			if (dbParam_IsDama(stConfig.eType))
			{
				// Set To Registry
				hErr = db_param_DamaSetConfigItem(pstConfigItem);
			}
			else if (dbParam_IsRegistry(stConfig.eType))
			{
				// Set To Registry
				hErr = db_param_RegSetConfigItem(pstConfigItem);
			}
			else
			{
				HxLOG_Warning("Invalid Type [%d]\n", stConfig.eType);
			}

			if (ERR_OK != hErr)
			{
				HxLOG_Warning("Write Error [%d]\n", hErr);
				hRes = ERR_FAIL;
			}
		}
		else
		{
			HxLOG_Error("db_param_TableConfigItem() Error [%d]\n", hErr);
			hRes = ERR_FAIL;
		}
	}

	DAPI_Sync(DxDBSETUP_SYSTEM);

	db_param_KeepingItemPrintList(astItem, nItemNum, TRUE);

	return hRes;
}

STATIC HERROR db_param_KeepingItemReset(dbParam_KeepingItemConfig_t *pstInfo)
{
	HERROR		hErr = ERR_FAIL;

	if (NULL == pstInfo)
		return ERR_FAIL;

	if (pstInfo)
	{
		hErr = pdb_param_FreeKeepingConfigItem(pstInfo->astItem, pstInfo->nItemNum);
		HxSTD_MemSet(pstInfo, 0x00, sizeof(dbParam_KeepingItemConfig_t));
	}

	return hErr;
}

HBOOL db_param_FindKeepingConfigItem(const DxDBSETUP_Item_e eDxItem)
{
	DbParam_Item_e eItem = db_param_TableGetParamItemByDxItem((const DxDBSETUP_Item_e)eDxItem);

	return pdb_param_FindKeepingConfigItem(eItem);
}

HERROR db_param_KeepingItemLoadDefault(void)
{
	HERROR						 hErr;
	HINT32						 nItemNum;
	HBOOL						 bKeepingItem;
	dbParam_KeepingItem_t		*astItem = NULL;
	dbParam_KeepingItemConfig_t	*pstInfo = db_param_KeepingItemGetContext();

	if (NULL == pstInfo)
		return ERR_FAIL;

	bKeepingItem = FALSE;

	// 1. Get(Clone) Keeping Items
	HxLOG_Debug("1.1. Get Keeping Items !!!!!!!!!\n");
	astItem = NULL;
	nItemNum = 0;
	hErr = pdb_param_GetKeepingConfigItem(&astItem, &nItemNum);
	if (ERR_OK == hErr && NULL != astItem && nItemNum > 0)
	{
		bKeepingItem = TRUE;
	}
	HxLOG_Debug("bKeepingItem = [%d]\n", bKeepingItem);

	HxLOG_Debug("1.2 Set Keeping Items [NumItem:%d]\n", nItemNum);
	pstInfo->bKeeping = bKeepingItem;
	pstInfo->astItem = astItem;
	pstInfo->nItemNum = nItemNum;

	db_param_KeepingItemPrintList(astItem, nItemNum, TRUE);

	return (bKeepingItem) ? ERR_OK : ERR_FAIL;
}

STATIC HERROR db_param_KeepingItemOnRemoveCompleted(dbParam_KeepingItemConfig_t *pstInfo)
{
	HERROR	hErr = ERR_OK;

	if (NULL == pstInfo)
	{
		HxLOG_Error("NULL == pstInfo\n");
		return ERR_FAIL;
	}

	// 1. Write Keeping Item
	HxLOG_Debug("1. Write Keeping Item\n");
	if (pstInfo->bKeeping)
	{
		hErr = db_param_KeepingItemWrite(pstInfo->astItem, pstInfo->nItemNum);
		if (ERR_OK == hErr)
		{
			DAPI_Sync(DxDBSETUP_SYSTEM);
		}
	}

	// 2. Reset Item List
	HxLOG_Debug("2. Reset Keeping Item List\n");
	if (pstInfo->astItem)
	{
		hErr |= db_param_KeepingItemReset(pstInfo);
	}

	HxLOG_Print("[%s (%04d)] 3. return [0x%x]\n", __FUNCTION__, __LINE__, hErr);

	return hErr;
}

#if 0
STATIC void db_param_KeepingItemOnDamaChanged (const HCHAR *pszName, HUID hUid, const void *pvData, HUINT32 ulSize, void *pvUserData)
{
	HINT32						 i;
	HINT32						 nCount;
	const HUID 					*ahUidList;
	HUID						 hEvtUid;
	HBOOL						 bTableRemoved;
	dbParam_KeepingItemConfig_t	*pstInfo = (dbParam_KeepingItemConfig_t *)pvUserData;

	HxLOG_Debug("%s(%s, uid:"HUID_FMT", data:0x%X, size:%u, userdata:0x%X\n", __FUNCTION__, pszName, hUid, pvData, ulSize, pvUserData);

	if (ulSize == 0)
	{
		HxLOG_Error("ulSize == 0\n");
		return;
	}

	if (HxSTD_StrCmp(pszName, DxDBSETUP_SYSTEM) != 0)
	{
		HxLOG_Error("Invalid Table Name [%s]\n", pszName);
		return;
	}

	bTableRemoved = FALSE;
	ahUidList = (const HUID *)pvData;
	nCount = (HINT32)(ulSize / sizeof(HUID));
	for (i = 0; i < nCount; i++, ahUidList++)
	{
		hEvtUid = *ahUidList;
		if (hEvtUid == -1)
		{
			bTableRemoved = TRUE;
			HxLOG_Debug("Notify Table(%s) Removed!\n", pszName);
			break;
		}
	}

	HxLOG_Debug("bTableRemoved [%d]\n", bTableRemoved);
	if (TRUE == bTableRemoved)
	{
		//pstInfo->bwNotiFlag |= eDB_PARAM_NOTI_DAMA;
		dbParam_SET_FLAG(pstInfo->bwNotiFlag, eDB_PARAM_NOTI_DAMA);

		HxLOG_Debug("[0x%08x, %d, %d]\n", pstInfo->bwNotiFlag, pstInfo->bDamaRemoved, pstInfo->bRegRemoved);

		dbParam_PostMsgToMgr(eSEVT_DB_PARAM_CHECK_ALL_REMOVED, HANDLE_NULL, (HINT32)pvUserData, eDB_PARAM_NOTI_DAMA, 0);
		HxLOG_Debug("PostMsg : eSEVT_DB_PARAM_CHECK_ALL_REMOVED \n");
	}
}
#endif

HERROR db_param_KeepingItemNotifyDamaRemoved(void)
{
	dbParam_KeepingItemConfig_t *pstInfo = db_param_KeepingItemGetContext();

	//pstInfo->bwNotiFlag |= eDB_PARAM_NOTI_DAMA;
	dbParam_SET_FLAG(pstInfo->bwNotiFlag, eDB_PARAM_NOTI_DAMA);

	HxLOG_Debug("[0x%08x, %d, %d]\n", pstInfo->bwNotiFlag, pstInfo->bDamaRemoved, pstInfo->bRegRemoved);

	dbParam_PostMsgToMgr(eSEVT_DB_PARAM_CHECK_ALL_REMOVED, HANDLE_NULL, (HINT32)pstInfo, eDB_PARAM_NOTI_DAMA, 0);
	HxLOG_Debug("PostMsg : eSEVT_DB_PARAM_CHECK_ALL_REMOVED \n");

	return ERR_OK;
}

STATIC HERROR db_param_KeepingItemRegistryOnChanged(void *pvUserData, HBOOL bTableRemoved)
{
	dbParam_KeepingItemConfig_t	*pstInfo = (dbParam_KeepingItemConfig_t *)pvUserData;

	if (NULL == pstInfo)
	{
		HxLOG_Error("NULL == pstInfo\n");
		return ERR_FAIL;
	}

	HxLOG_Error("bTableRemoved [%d]\n", bTableRemoved);
	if (TRUE == bTableRemoved)
	{
		//pstInfo->bwNotiFlag |= eDB_PARAM_NOTI_REG;
		dbParam_SET_FLAG(pstInfo->bwNotiFlag, eDB_PARAM_NOTI_REG);

		HxLOG_Debug("[0x%08x, %d, %d]\n", pstInfo->bwNotiFlag, pstInfo->bDamaRemoved, pstInfo->bRegRemoved);

		dbParam_PostMsgToMgr(eSEVT_DB_PARAM_CHECK_ALL_REMOVED, HANDLE_NULL, (HINT32)pvUserData, eDB_PARAM_NOTI_REG, 0);
		HxLOG_Debug("PostMsg : eSEVT_DB_PARAM_CHECK_ALL_REMOVED \n");
	}

	return ERR_OK;
}

STATIC HERROR db_param_KeepingItemRegisterNotifier(dbParam_KeepingItemConfig_t *pstInfo)
{
	HERROR						 hErr;

#if 0
	HUINT32						 ulRequestId;

	ulRequestId = DAPI_AddTableNotifier(DxDBSETUP_SYSTEM, db_param_KeepingItemOnDamaChanged, (void *)pstInfo);
	HxLOG_Critical("[ulRequestId : %d]\n", ulRequestId);
	if (0 == ulRequestId)
	{
		HxLOG_Critical("DAPI_AddTableNotifier() Fail! [ulRequestId : %d]\n", ulRequestId);
		return ERR_FAIL;
	}
#endif

	hErr = HAPI_SetRegistryListener((void *)pstInfo, db_param_KeepingItemRegistryOnChanged);
	if (ERR_OK != hErr)
	{
		HxLOG_Critical("HAPI_SetRegistryListener() Fail! [0x%x]\n", hErr);
	}

	return hErr;
}

HERROR db_param_KeepingItemInit(void)
{
	HERROR	hErr;

	HxSTD_MemSet(&s_stKeepingItemInfo, 0, sizeof(dbParam_KeepingItemConfig_t));
	db_param_SetUserData(&s_stKeepingItemInfo, NULL);

	// Register Notify Callback (Registry and Dama)
	HxLOG_Debug("Register Notify Callback\n");
	hErr = db_param_KeepingItemRegisterNotifier(&s_stKeepingItemInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("Error : db_param_KeepingItemRegisterNotifier() Fail!\n");
	}

	return hErr;
}

HBOOL DB_PARAM_KeepingItemFinished(void)
{
	dbParam_KeepingItemConfig_t	*pstInfo = db_param_KeepingItemGetContext();

	if (NULL == pstInfo)
		return FALSE;

	HxLOG_Debug("bKeeping [%d], bFinished [%d]\n", pstInfo->bKeeping, pstInfo->bFinished);
	if (pstInfo->bKeeping)
	{
		return pstInfo->bFinished;
	}
	return TRUE;
}

HBOOL DB_PARAM_KeepingItemAllTableRemoved(void *pvKeepingItemContext, HUINT32 ulType)
{
	dbParam_KeepingItemConfig_t	*pstInfo = (dbParam_KeepingItemConfig_t *)pvKeepingItemContext;
	HBOOL						 bRet = FALSE;
	dbParam_NotiType_e			 eType = (dbParam_NotiType_e)ulType;

	if (NULL == pvKeepingItemContext)
	{
		HxLOG_Error("NULL == pvKeepingItemContext\n");
		return FALSE;
	}

	HxLOG_Debug("eType [0x%x], bwNotiFlag [0x%08x]\n", eType, pstInfo->bwNotiFlag);
//	HxLOG_Debug("(&s_stKeepingItemInfo == pstInfo) : [%s]\n", (&s_stKeepingItemInfo == pstInfo) ? "True" : "False");

//	if (pstInfo->bwNotiFlag & eDB_PARAM_NOTI_REG)
	if (dbParam_CHECK_FLAG(pstInfo->bwNotiFlag, eDB_PARAM_NOTI_REG))
	{
		//pstInfo->bwNotiFlag &= ~(eDB_PARAM_NOTI_REG);
		dbParam_RESET_FLAG(pstInfo->bwNotiFlag, eDB_PARAM_NOTI_REG);
		pstInfo->bRegRemoved = TRUE;
		HxLOG_Debug("Registry : Remove Done [%s]\n", (pstInfo->bRegRemoved) ? "True" : "False");
	}

//	if (pstInfo->bwNotiFlag & eDB_PARAM_NOTI_DAMA)
	if (dbParam_CHECK_FLAG(pstInfo->bwNotiFlag, eDB_PARAM_NOTI_DAMA))
	{
		//pstInfo->bwNotiFlag &= ~(eDB_PARAM_NOTI_DAMA);
		dbParam_RESET_FLAG(pstInfo->bwNotiFlag, eDB_PARAM_NOTI_DAMA);
		pstInfo->bDamaRemoved = TRUE;
		HxLOG_Debug("DAMA System Table : Remove Done [%s]\n", (pstInfo->bRegRemoved) ? "True" : "False");
	}

	HxLOG_Debug("DAMA, Registry Removed? : [DAMA:%s, REG:%s]\n", (pstInfo->bDamaRemoved) ? "True" : "False", (pstInfo->bRegRemoved) ? "True" : "False");
	HxLOG_Debug("bKeeping [%d]\n", pstInfo->bKeeping);

	bRet = FALSE;
	if (TRUE == pstInfo->bRegRemoved && TRUE == pstInfo->bDamaRemoved)
	{
		bRet = TRUE;
	}

	HxLOG_Debug("bRet [%d]\n", bRet);

	return bRet;
}

HERROR DB_PARAM_KeepingItemWrite(void *pvKeepingItemContext)
{
	dbParam_KeepingItemConfig_t	*pstInfo = (dbParam_KeepingItemConfig_t *)pvKeepingItemContext;

	if (NULL == pstInfo)
	{
		return ERR_FAIL;
	}

	(void)db_param_KeepingItemOnRemoveCompleted(pstInfo);
	pstInfo->bFinished = TRUE;
	pstInfo->bKeeping = FALSE;

	return ERR_OK;
}

/* end of file */
