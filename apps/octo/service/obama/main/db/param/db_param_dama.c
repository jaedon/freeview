/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  db_param_dama.c
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

#define	SYNC_WAITING_TIMEOUT	10000

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Local Variables      ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Local Functions      ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

#define	___DAMA__________________

STATIC void db_param_DamaDapiGetter(const HCHAR *pszName, HUID hUid, const void *pvData, HUINT32 nSize, void *pvUser)
{
	HxTREE_t		*pstDataTree = NULL;
	DxDBSetupItem_t	*pstItemInfo;
	DAxIterator_t	 stIter;

	if (pvUser == NULL || pvData == NULL || nSize == 0)
	{
		HxLOG_Error("Invalid parameter\n");
		return;
	}

	pstDataTree = db_param_CacheGetTreeFromHandle((Handle_t)pvUser);
	if (NULL == pstDataTree)
	{
		HxLOG_Error("Invalid parameter\n");
		return;
	}

	DAPI_InitIterator(&stIter, pvData, nSize, 0);
	while (DAPI_NextIterator(&stIter))
	{
		pstItemInfo = (DxDBSetupItem_t*)stIter.data;
		if (stIter.uid != (HUID)pstItemInfo->uid)
		{
			HxLOG_Critical("DB Enumeration Field is not SAME with previous one!!!\n");
			HxLOG_Critical("Please add your own enumeration value to right position!\n");
			HxLOG_Assert(0);
		}
#if 0
		switch(pstItemInfo->eItemType)
		{
		case eDxTYPE_INTEGER:
			HLIB_CMD_Printf(HxANSI_COLOR_GREEN("\t%08X INT: %d\n"), (int)stIter.uid, pstItemInfo->nValue);
			break;

		case eDxTYPE_BINARY:
			HLIB_CMD_Printf(HxANSI_COLOR_GREEN("\t%08X BIN\n"), (int)stIter.uid);
			HLIB_LOG_Dump(pstItemInfo->pucData, pstItemInfo->nSize, 0, FALSE);
			break;

		case eDxTYPE_STRING:
			HLIB_CMD_Printf(HxANSI_COLOR_GREEN("\t%08X STR: %s\n"), (int)stIter.uid, pstItemInfo->pucData);
			break;
		default:
			break;
		}
#endif
		HLIB_TREE_Replace(pstDataTree, (void*)pstItemInfo->uid, HLIB_STD_MemDup(stIter.data, sizeof(DxDBSetupItem_t) + pstItemInfo->nSize));
	}
}

STATIC void db_param_DamaDapiNotify(const HCHAR *pszName, HUID hUid, const void *pvData, HUINT32 nSize, void *pvUser)
{
#if 0
	HxTREE_t	*pstDataTree = NULL;

	pstDataTree = db_param_CacheGetTreeFromHandle((Handle_t)pvUser);
	if (NULL == pstDataTree)
	{
		HxLOG_Error("Invalid parameter\n");
		return;
	}
#endif
	//	TODO: Data is changed by other process
	HxLOG_Warning("Data is changed by other process ---> [%s]("HUID_FMT")\n", pszName, hUid);
}

STATIC HERROR db_param_DamaSystemSetValue(const DxDBSETUP_Item_e eItem, HINT32 nValue)
{
	DxDBSetupItem_t		*pstInfo;

	dbParam_ENTER_CRITICAL();

	pstInfo = db_param_CacheSetItemValue(dbParam_GetCacheHandle(), eItem, nValue);
	if (pstInfo)
	{
		DAPI_Set(DxDBSETUP_SYSTEM, eItem, pstInfo, sizeof(DxDBSetupItem_t) + pstInfo->nSize);
        DAPI_Sync(DxDBSETUP_SYSTEM);
	}
	dbParam_LEAVE_CRITICAL();

	return pstInfo ? ERR_OK : ERR_FAIL;
}

STATIC HERROR db_param_DamaSystemSetText(const DxDBSETUP_Item_e eItem, HCHAR *pszText)
{
	DxDBSetupItem_t		*pstInfo;

	dbParam_ENTER_CRITICAL();

	pstInfo = db_param_CacheSetItemText(dbParam_GetCacheHandle(), eItem, pszText);
	if (pstInfo)
	{
		DAPI_Set(DxDBSETUP_SYSTEM, eItem, pstInfo, sizeof(DxDBSetupItem_t) + pstInfo->nSize);
        DAPI_Sync(DxDBSETUP_SYSTEM);
	}
	dbParam_LEAVE_CRITICAL();

	return pstInfo ? ERR_OK : ERR_FAIL;
}

STATIC HERROR db_param_DamaSystemSetBinary(const DxDBSETUP_Item_e eItem, HINT32 nSize, void *pvData)
{
	DxDBSetupItem_t		*pstInfo;

	dbParam_ENTER_CRITICAL();

	pstInfo = db_param_CacheSetItemBinary(dbParam_GetCacheHandle(), eItem, nSize, pvData);
	if (pstInfo)
	{
		DAPI_Set(DxDBSETUP_SYSTEM, eItem, pstInfo, sizeof(DxDBSetupItem_t) + pstInfo->nSize);
        DAPI_Sync(DxDBSETUP_SYSTEM);
	}
	dbParam_LEAVE_CRITICAL();

	return pstInfo ? ERR_OK : ERR_FAIL;
}

STATIC HERROR db_param_DamaSystemGetValue(const DxDBSETUP_Item_e eItem, HINT32 *pnValue)
{
	DxDBSetupItem_t		*pstInfo;

	dbParam_ENTER_CRITICAL();

	pstInfo = db_param_CacheGetItemValue(dbParam_GetCacheHandle(), eItem, pnValue);

	dbParam_LEAVE_CRITICAL();

	return pstInfo ? ERR_OK : ERR_FAIL;
}

STATIC HERROR db_param_DamaSystemGetText (const DxDBSETUP_Item_e eItem, HINT32 nLimit, HCHAR *pszText)
{
	DxDBSetupItem_t		*pstInfo;

	dbParam_ENTER_CRITICAL();

	pstInfo = db_param_CacheGetItemText(dbParam_GetCacheHandle(), eItem, nLimit, pszText);

	dbParam_LEAVE_CRITICAL();

	return pstInfo ? ERR_OK : ERR_FAIL;
}

STATIC HERROR db_param_DamaSystemGetBinary(const DxDBSETUP_Item_e eItem, HINT32 nLimit, HINT32 *pnDataSize, void *pvData)
{
	DxDBSetupItem_t		*pstInfo;

	dbParam_ENTER_CRITICAL();

	pstInfo = db_param_CacheGetItemBinary(dbParam_GetCacheHandle(), eItem, nLimit, pnDataSize, pvData);

	dbParam_LEAVE_CRITICAL();

	return pstInfo ? ERR_OK : ERR_FAIL;
}

STATIC HERROR db_param_DamaSystemSetEnum(const DxDBSETUP_Item_e eItem, const char *pszEnumType, HINT32 eEnumValue)
{
	const char	*pszEnumStr;

	pszEnumStr = DLIB_ConvertEnum2Str(pszEnumType, eEnumValue);
	if (NULL == pszEnumStr)
		return ERR_FAIL;

	return db_param_DamaSystemSetText(eItem, (HCHAR *)pszEnumStr);
}


#define	___DAMA_INTEFACE__________________
/***************************************************************************************
 ***************************************************************************************/
HERROR db_param_DamaSetConfigItem(const dbParam_ItemConfig_t *pstConfig)
{
	HERROR				hErr;
	DxDBSETUP_Item_e	eKey;

	if (NULL == pstConfig)
	{
		return ERR_FAIL;
	}

	if (0 == pstConfig->utParam.eKey)
	{
		return ERR_FAIL;
	}

	eKey = pstConfig->utParam.eKey;
	hErr = ERR_FAIL;
	switch(pstConfig->eType)
	{
	case eDB_STORAGE_TYPE_DAMA_INT:
		hErr = db_param_DamaSystemSetValue((const DxDBSETUP_Item_e)eKey, pstConfig->utParam.utDamaInt.nValue);
		break;
	case eDB_STORAGE_TYPE_DAMA_STR:
		hErr = db_param_DamaSystemSetText((const DxDBSETUP_Item_e)eKey, pstConfig->utParam.utDamaStr.pszString);
		break;
	case eDB_STORAGE_TYPE_DAMA_ENUM:
		hErr = db_param_DamaSystemSetEnum((const DxDBSETUP_Item_e)eKey, pstConfig->utParam.utDamaEnum.pszEnumType, pstConfig->utParam.utDamaEnum.eEnumValue);
		break;
	case eDB_STORAGE_TYPE_DAMA_BIN:
		if ((0 < pstConfig->utParam.utDamaBin.ulBufLen) && (NULL != pstConfig->utParam.utDamaBin.pvData))
		{
			hErr = db_param_DamaSystemSetBinary((const DxDBSETUP_Item_e)eKey, (HINT32)pstConfig->utParam.utDamaBin.ulBufLen, pstConfig->utParam.utDamaBin.pvData);
		}
		break;
	default:
		break;
	}

	return hErr;
}

HERROR db_param_DamaSetInt(const DxDBSETUP_Item_e eItem, HINT32 nValue, HINT32 nMsg)
{
	HERROR	hErr;
	HINT32	nTempValue;

	if (db_param_DamaSystemGetValue(eItem, &nTempValue) == ERR_OK)
	{
		if (nTempValue == nValue)
			return ERR_FAIL;
	}

	hErr = db_param_DamaSystemSetValue(eItem, nValue);
	if (ERR_OK != hErr)
		return ERR_FAIL;

	if(nMsg != eDB_PARAM_ITEM_UNKNOWN)
	{
		dbParam_PostMsgToMgr(eSEVT_DB_PARAM_UPDATED, HANDLE_NULL, nMsg, 0, 0);
	}

	return ERR_OK;
}

HERROR db_param_DamaGetInt(const DxDBSETUP_Item_e eItem, HUINT32 *pulData)
{
	HERROR		hErr;
	HINT32		nValue;

	nValue = 0;
	hErr = db_param_DamaSystemGetValue(eItem, (HINT32*)&nValue);
	*pulData = (HUINT32)nValue;

	return hErr;
}

HERROR db_param_DamaSetEnum(const DxDBSETUP_Item_e eItem, const char *pszEnumType, HINT32 eEnumValue, HINT32 nMsg)
{
	HERROR		 hErr;
	const char	*pszEnumStr;
	HCHAR		 szCurrentEnumStr[64];

	pszEnumStr = DLIB_ConvertEnum2Str(pszEnumType, eEnumValue);
	if (NULL == pszEnumStr)
		return ERR_FAIL;

	if (db_param_DamaSystemGetText(eItem, 32, szCurrentEnumStr) == ERR_OK)
	{
		if (HxSTD_StrCmp(pszEnumStr, szCurrentEnumStr) == 0)
			return ERR_OK;
	}

	hErr = db_param_DamaSystemSetText(eItem, (HCHAR *)pszEnumStr);

	if(nMsg != eDB_PARAM_ITEM_UNKNOWN)
	{
		dbParam_PostMsgToMgr(eSEVT_DB_PARAM_UPDATED, HANDLE_NULL, nMsg, 0, 0);
	}

	return hErr;
}

HERROR db_param_DamaGetEnum(const DxDBSETUP_Item_e eItem, const HCHAR *pszEnumType, HUINT32 *pulData)
{
	HERROR			hErr;
	static HCHAR	s_szEnumStr[64];

	*pulData = 0;
	hErr = db_param_DamaSystemGetText(eItem, 32, s_szEnumStr);
	if (ERR_OK == hErr)
	{
		*pulData = (HUINT32)DLIB_ConvertStr2Enum(pszEnumType, s_szEnumStr);
	}

	return hErr;
}

HERROR db_param_DamaSetStr(const DxDBSETUP_Item_e eItem, const HCHAR *pszString, HUINT32 ulBufLen, HINT32 nMsg)
{
	HERROR		hErr;
	HCHAR		szCurrentStr[64];

	if (db_param_DamaSystemGetText(eItem, ulBufLen, szCurrentStr) == ERR_OK)
	{
		if (HxSTD_StrCmp(pszString, szCurrentStr) == 0)
			return ERR_FAIL;
	}

	hErr = db_param_DamaSystemSetText(eItem, (HCHAR *)pszString);
	if (ERR_OK != hErr)
		return ERR_FAIL;

	if(nMsg != eDB_PARAM_ITEM_UNKNOWN)
	{
		dbParam_PostMsgToMgr(eSEVT_DB_PARAM_UPDATED, HANDLE_NULL, nMsg, 0, 0);
	}

	return ERR_OK;
}

HERROR db_param_DamaGetStr(const DxDBSETUP_Item_e eItem, HCHAR *pszString, HUINT32 ulBufLen)
{
	HERROR		hErr;

	hErr = db_param_DamaSystemGetText(eItem, ulBufLen, pszString);

	return hErr;
}

HERROR db_param_DamaSetBin(const DxDBSETUP_Item_e eItem, void *pvData, HUINT32 ulSize, HBOOL bComp, HINT32 nMsg)
{
	HERROR			hErr;
	HINT32			nDataSize;

	if (NULL == pvData)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if (0 == ulSize)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if(bComp)
	{
		HCHAR		*pucBuf = NULL;
		db_param_GetCbufSem();
		pucBuf = HLIB_CBUF_GetBuffer(db_param_GetCbufHandle(), ulSize);
		db_param_ReleaseCbufSem();
	    if (db_param_DamaSystemGetBinary(eItem, ulSize, &nDataSize, (void *)pucBuf) == ERR_OK)
	    {
			if (ulSize > 0)
			{
			    if (HxSTD_MemCmp((void*)pucBuf, pvData, ulSize) == 0)
				    return ERR_FAIL;
			}
	    }
	}

	hErr = db_param_DamaSystemSetBinary(eItem, ulSize, pvData);
	if(hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if(nMsg != eDB_PARAM_ITEM_UNKNOWN)
	{
		dbParam_PostMsgToMgr(eSEVT_DB_PARAM_UPDATED, HANDLE_NULL, (HINT32)nMsg, 0, 0);
	}

	return ERR_OK;
}

HERROR db_param_DamaGetBin(const DxDBSETUP_Item_e eItem, void *pvData, HUINT32 ulSize)
{
	HERROR			hErr;
	HINT32			nDataSize = 0;

	hErr = db_param_DamaSystemGetBinary(eItem, ulSize, &nDataSize, pvData);
	if(hErr != ERR_OK) 	 	{	HxLOG_Critical("\n\n");	}
	if (nDataSize <= 0) 	{	HxLOG_Critical("\n\n");	}

	return hErr;
}

HERROR db_param_DamaInit(Handle_t hCache)
{
	HUINT32	nId;

	nId = DAPI_GetAll(DxDBSETUP_SYSTEM, db_param_DamaDapiGetter, (void *)hCache);
//	DAPI_AddTableNotifier(DxDBSETUP_SYSTEM, db_param_DamaDapiNotify, (void *)hCache);
	DAPI_Wait(SYNC_WAITING_TIMEOUT, nId);

	return ERR_OK;
}

HERROR db_param_DamaRemoveAll(void)
{
	HERROR	hErr;

	hErr = DAPI_ResetTable (DxDBSETUP_SYSTEM);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("DAPI_ResetTable (DxDBSETUP_SYSTEM) Fail [%d]\n", hErr);
	}

	(void)DAPI_Sync (DxDBSETUP_SYSTEM);

	return hErr;
}

/* end of file */
