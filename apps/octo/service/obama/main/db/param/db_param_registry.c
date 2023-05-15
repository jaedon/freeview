/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  db_param_registry.c
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
#include "./local_include/_db_param_default.h"
#include "_db_param.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

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

#define	___REGISTRY_INTEFACE__________________

/*	Registry Functions
 *
 */
HERROR db_param_RegSetConfigItem(const dbParam_ItemConfig_t *pstConfig)
{
	HERROR		hErr;
	const HCHAR	*pszKey;

	if (NULL == pstConfig)
	{
		return ERR_FAIL;
	}

	if (NULL == pstConfig->utParam.pszKey)
	{
		return ERR_FAIL;
	}

	pszKey = (const HCHAR *)pstConfig->utParam.pszKey;

	hErr = ERR_FAIL;

	// Cache 에 넣을 필요 없음.
	// HAPI Registry 내에 구현되어 있음.
	switch (pstConfig->eType)
	{
	case eDB_STORAGE_TYPE_REG_INT:
		HxLOG_Print("[%s (%04d)] [%s] : Value[%d]\n", __FUNCTION__, __LINE__, pszKey, pstConfig->utParam.utRegInt.nValue);
		hErr = HAPI_SetRegistryInt(pszKey, pstConfig->utParam.utRegInt.nValue);
		break;

	case eDB_STORAGE_TYPE_REG_STR:
		if (NULL != pstConfig->utParam.utRegStr.pszValue)
		{
			HxLOG_Print("[%s (%04d)] [%s] : Value[%d]\n", __FUNCTION__, __LINE__, pszKey, pstConfig->utParam.utRegStr.pszValue);
			hErr = HAPI_SetRegistryStr(pszKey, pstConfig->utParam.utRegStr.pszValue);
		}
		break;

	case eDB_STORAGE_TYPE_REG_ENUM:
		if (NULL != pstConfig->utParam.utRegEnum.pszEnumType)
		{
			HxLOG_Print("[%s (%04d)] [%s][%s] : Value[%d]\n", __FUNCTION__, __LINE__, pszKey, pstConfig->utParam.utRegEnum.pszEnumType, pstConfig->utParam.utRegEnum.eEnumValue);
			hErr = HAPI_SetRegistryEnum(pszKey, pstConfig->utParam.utRegEnum.pszEnumType, pstConfig->utParam.utRegEnum.eEnumValue);
		}
		break;

	default:
		hErr = ERR_FAIL;
		break;
	}

//	HxLOG_Print("[%s (%04d)] hErr : [%d]\n", __FUNCTION__, __LINE__, hErr);

	return hErr;
}

HERROR db_param_RegGetDefaultValue(const HCHAR *pszKey, HUINT32 *pulData)
{
	HERROR					 hErr;
	dbParam_ItemConfig_t	 stConfig;
	dbParam_DefaultItem_t	*pstDefItem;
	HUINT32					 ulData = 0;

	pstDefItem = db_param_DefaultValueGetItemByRegKey(pszKey);
	if (NULL == pstDefItem)
	{
		return ERR_FAIL;
	}

	(void)db_param_GetCfgItemValue((const dbParam_CfgItemInfo_t *)&pstDefItem->stInfo, &ulData, NULL);
	HxLOG_Print("[%s;%d] db_param_GetCfgItemValue() return [%d]\n", __FUNCTION__, __LINE__, ulData);

	HxSTD_MemSet(&stConfig, 0, sizeof(dbParam_ItemConfig_t));
	hErr = db_param_TableConfigItem(&stConfig, TRUE, pstDefItem->eItem, ulData, 0);
	if (ERR_OK == hErr)
	{
		HxLOG_Print("[%s;%d] stConfig.eType [%d]\n", __FUNCTION__, __LINE__, stConfig.eType);
		if (dbParam_IsRegistry(stConfig.eType))
		{
			// Set To Registry
			hErr = db_param_RegSetConfigItem((const dbParam_ItemConfig_t *)&stConfig);
			*pulData = ulData;
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

HERROR db_param_RegSetInt(const HCHAR *pszKey, HINT32 nValue, HINT32 nMsg)
{
	HERROR	hErr;
	HINT32	nTemp;

	if (HAPI_GetRegistryInt(pszKey, &nTemp) == ERR_OK)
	{
		if (nTemp == nValue)
			return ERR_OK;
	}

	hErr = HAPI_SetRegistryInt(pszKey, nValue);
	if (ERR_OK != hErr)
	{
		return ERR_FAIL;
	}

	if (nMsg != eDB_PARAM_ITEM_UNKNOWN)
	{
		dbParam_PostMsgToMgr(eSEVT_DB_PARAM_UPDATED, HANDLE_NULL, nMsg, 0, 0);
	}

	return ERR_OK;
}

HERROR db_param_RegGetInt(const HCHAR *pszKey, HUINT32 *pulData)
{
	HERROR	hErr;
	HINT32	nValue = 0;

	nValue = 0;
	hErr = HAPI_GetRegistryInt(pszKey, &nValue);
	if (ERR_OK != hErr)
	{
		dbParam_ENTER_CRITICAL();
		hErr = db_param_RegGetDefaultValue(pszKey, (HUINT32 *)&nValue);
		dbParam_LEAVE_CRITICAL();
	}

	if (ERR_OK == hErr)
	{
		*pulData = (HUINT32)nValue;
	}

	return hErr;
}

HERROR db_param_RegSetEnum(const HCHAR *pszKey, const HCHAR *pszEnumType, HINT32 nValue, HINT32 nMsg)
{
	HERROR	hErr;
	HINT32	nTemp;

	if (HAPI_GetRegistryEnum(pszKey, pszEnumType, &nTemp) == ERR_OK)
	{
		if (nTemp == nValue)
			return ERR_FAIL;
	}

	hErr = HAPI_SetRegistryEnum(pszKey, pszEnumType, nValue);
	if (ERR_OK != hErr)
	{
		return ERR_FAIL;
	}

	if (nMsg != eDB_PARAM_ITEM_UNKNOWN)
	{
		dbParam_PostMsgToMgr(eSEVT_DB_PARAM_UPDATED, HANDLE_NULL, nMsg, 0, 0);
	}

	return ERR_OK;
}

HERROR db_param_RegGetEnum(const HCHAR *pszKey, const HCHAR *pszEnumType, HUINT32 *pulData)
{
	HERROR	hErr;
	HINT32	nValue;

	nValue = 0;
	hErr = HAPI_GetRegistryEnum(pszKey, pszEnumType, &nValue);
	if (ERR_OK != hErr)
	{
		dbParam_ENTER_CRITICAL();
		hErr = db_param_RegGetDefaultValue(pszKey, (HUINT32 *)&nValue);
		dbParam_LEAVE_CRITICAL();
	}

	if (ERR_OK == hErr)
	{
		*pulData = (HUINT32)nValue;
	}

	return hErr;
}

HERROR db_param_RegSetStr(const HCHAR *pszKey, const HCHAR *pszValue, HINT32 nMsg)
{
	HERROR	 hErr;
	HCHAR	*pszTempValue = NULL;

	db_param_GetCbufSem();
	pszTempValue = HLIB_CBUF_GetBuffer(db_param_GetCbufHandle(), 256);
	db_param_ReleaseCbufSem();

	if (HAPI_GetRegistryStr(pszKey, pszTempValue, 256) == ERR_OK)
	{
		if (pszTempValue != NULL && pszValue != NULL && HxSTD_StrCmp(pszTempValue, pszValue) == 0)
			return ERR_FAIL;
	}

	hErr = HAPI_SetRegistryStr(pszKey, (HCHAR*)pszValue);
	if (ERR_OK != hErr)
	{
		return ERR_FAIL;
	}

	if (nMsg != eDB_PARAM_ITEM_UNKNOWN)
	{
		dbParam_PostMsgToMgr(eSEVT_DB_PARAM_UPDATED, HANDLE_NULL, nMsg, 0, 0);
	}

	return ERR_OK;
}

HERROR db_param_RegGetStr(const HCHAR *pszKey, HCHAR *pszString, HUINT32 ulSrcBufLen)
{
	HERROR	 hErr;
	HCHAR	*pszTemp, *pszSource;
	HUINT32	 nStrLen, ulData;

	dbParam_ENTER_CRITICAL();

	pszSource = NULL;
	db_param_GetCbufSem();
	pszTemp = HLIB_CBUF_GetBuffer(db_param_GetCbufHandle(), 256);
	db_param_ReleaseCbufSem();
	hErr = HAPI_GetRegistryStr(pszKey, pszTemp, 256);
	if (ERR_OK != hErr)
	{
		ulData = 0;
		hErr = db_param_RegGetDefaultValue(pszKey, &ulData);
		if (ERR_OK != hErr || 0 == ulData)
		{
			hErr = ERR_FAIL;
			pszSource = NULL;
			ulSrcBufLen = 0;
		}
		else
		{
			pszSource = (HCHAR *)ulData;
			nStrLen = HxSTD_StrLen((HCHAR *)ulData);
			ulSrcBufLen = (ulSrcBufLen <= nStrLen) ? ulSrcBufLen : nStrLen;
			hErr = ERR_OK;
		}
	}
	else
	{
		pszSource = pszTemp;
	}

	if (pszSource != NULL && ulSrcBufLen > 0)
	{
		HLIB_STD_StrNCpySafe(pszString, pszSource, ulSrcBufLen);
	}

	dbParam_LEAVE_CRITICAL();
	return hErr;
}

HERROR db_param_RegGetLang(const HCHAR *pszKey, HCHAR *pszString, HUINT32 ulSrcBufLen)
{
	HERROR	 hErr;
	HUINT32	 ulData = 0;

	if(pszString == NULL || ulSrcBufLen == 0)
		return ERR_FAIL;

	dbParam_ENTER_CRITICAL();

	hErr = HAPI_GetRegistryStr(pszKey, pszString, ulSrcBufLen);
	if (ERR_OK != hErr)
	{
		hErr = db_param_RegGetDefaultValue(pszKey, &ulData);
		if (ERR_OK != hErr || 0 == ulData)
		{
			hErr = ERR_FAIL;
		}
		else
		{
			HxSTD_StrNCpy(pszString, HLIB_LANG_IdTo639(ulData), ulSrcBufLen);
		}
	}

	dbParam_LEAVE_CRITICAL();
	return hErr;
}

HERROR db_param_RegRemoveAll(void)
{
	HAPI_RemoveRegistry();

	return ERR_OK;
}


/* end of file */
