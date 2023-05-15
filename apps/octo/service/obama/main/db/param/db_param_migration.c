/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  db_param_migration.c
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

#include <oapi_scenario.h>

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
#if defined(CONFIG_DEBUG)
void db_param_PrintLang(void)
{
	HINT32	nValue;
	HCHAR	szValue[64], *pszKey;

	pszKey = OAPI_SCENARIO_OPERATORTYPE;
	HAPI_GetRegistryEnum(pszKey, DxNAMEOF(DxOperator_e), &nValue);
	HxLOG_Print("[%s] [%20s] : [%d]\n", __FUNCTION__, OAPI_SCENARIO_OPERATORTYPE, nValue);

	pszKey = OAPI_SCENARIO_MAINLANGUAGE;
	HAPI_GetRegistryStr(pszKey, szValue, 64);
	HxLOG_Print("[%s] [%20s] : [%s]\n", __FUNCTION__, pszKey, szValue);

	HAPI_GetRegistryStr(OAPI_SCENARIO_AUDIOLANGUAGE0, szValue, 64);
	HxLOG_Print("[%s] [%20s] : [%s]\n", __FUNCTION__, pszKey, szValue);

	HAPI_GetRegistryStr(OAPI_SCENARIO_AUDIOLANGUAGE1, szValue, 64);
	HxLOG_Print("[%s] [%20s] : [%s]\n", __FUNCTION__, pszKey, szValue);

	HAPI_GetRegistryStr(OAPI_SCENARIO_SUBTITLELANGUAGE0, szValue, 64);
	HxLOG_Print("[%s] [%20s] : [%s]\n", __FUNCTION__, pszKey, szValue);

	HAPI_GetRegistryStr(OAPI_SCENARIO_SUBTITLELANGUAGE1, szValue, 64);
	HxLOG_Print("[%s] [%20s] : [%s]\n", __FUNCTION__, pszKey, szValue);
}
#endif

#define	___MIGRATION___________________

STATIC HBOOL db_param_MigrateIsDxItemMigrated(void)
{
	HERROR	hErr;
	HINT32	nValue;

	hErr = HAPI_GetRegistryInt(dbParam_ITEM_MIGRATION_FLAG, &nValue);
	if (ERR_OK == hErr)
	{
//		if (nValue == dbParam_VERSION)
			return TRUE;
	}

	return FALSE;
}

STATIC HERROR db_param_MigrateWriteFlag(HINT32 nVersion)
{
	return HAPI_SetRegistryInt(dbParam_ITEM_MIGRATION_FLAG, nVersion);
}

HERROR db_param_MigrateInt(const DxDBSETUP_Item_e eDxItem, const HCHAR *pszRegKey)
{
	HERROR	 hErr;
	HINT32	 nValue = 0;

	if (TRUE == db_param_FindKeepingConfigItem(eDxItem))
	{
		HxLOG_Warning("Skip Keeping Item [%d]\n", eDxItem);
		return ERR_OK;
	}

	hErr = db_param_DamaGetInt(eDxItem, &nValue);
	if (ERR_OK != hErr)
	{
		nValue = 0;
		HxLOG_Print("[%s (%04d)] db_param_DamaGetInt() Return Error!\n", __FUNCTION__, __LINE__);
		dbParam_ENTER_CRITICAL();
		db_param_RegGetDefaultValue(pszRegKey, (HUINT32 *)&nValue);
		dbParam_LEAVE_CRITICAL();
		HxLOG_Print("[%s (%04d)] Call db_param_RegGetDefaultValue(%s) : nValue [%d]\n", __FUNCTION__, __LINE__, pszRegKey, nValue);
	}

	hErr = HAPI_SetRegistryInt(pszRegKey, nValue);
	if (ERR_OK == hErr)
	{
//		DAPI_Reset(DxDBSETUP_SYSTEM, eDxItem);
	}

	return hErr;
}

HERROR db_param_MigrateEnum(const DxDBSETUP_Item_e eDxItem, const HCHAR *pszRegKey, const HCHAR *pszEnumType)
{
	HERROR	 hErr;
	HINT32	 nValue = 0;

	if (TRUE == db_param_FindKeepingConfigItem(eDxItem))
	{
		HxLOG_Warning("Skip Keeping Item [%d]\n", eDxItem);
		return ERR_OK;
	}

	hErr = db_param_DamaGetInt(eDxItem, &nValue);
	if (ERR_OK != hErr)
	{
		nValue = 0;
		HxLOG_Print("[%s (%04d)] db_param_DamaGetInt() Return Error!\n", __FUNCTION__, __LINE__);
		dbParam_ENTER_CRITICAL();
		db_param_RegGetDefaultValue(pszRegKey, (HUINT32 *)&nValue);
		dbParam_LEAVE_CRITICAL();
		HxLOG_Print("[%s (%04d)] Call db_param_RegGetDefaultValue(%s) : nValue [%d]\n", __FUNCTION__, __LINE__, pszRegKey, nValue);
	}

	hErr = HAPI_SetRegistryEnum(pszRegKey, pszEnumType, nValue);
	if (ERR_OK == hErr)
	{
//		DAPI_Reset(DxDBSETUP_SYSTEM, eDxItem);
	}

	return hErr;
}

HERROR db_param_MigrateStr(const DxDBSETUP_Item_e eDxItem, const HCHAR *pszRegKey)
{
	HERROR	 hErr;
	HCHAR	*pszValue = NULL;

	db_param_GetCbufSem();
	pszValue = HLIB_CBUF_GetBuffer(db_param_GetCbufHandle(), 256);
	db_param_ReleaseCbufSem();

	if (TRUE == db_param_FindKeepingConfigItem(eDxItem))
	{
		HxLOG_Warning("Skip Keeping Item [%d]\n", eDxItem);
		return ERR_OK;
	}

	hErr = db_param_DamaGetStr(eDxItem, pszValue, 256);
	if (ERR_OK != hErr)
	{
		HxLOG_Print("[%s (%04d)] db_param_DamaGetStr() Return Error!\n", __FUNCTION__, __LINE__);
		dbParam_ENTER_CRITICAL();
		db_param_RegGetDefaultValue(pszRegKey, (HUINT32 *)pszValue);
		dbParam_LEAVE_CRITICAL();
		HxLOG_Print("[%s (%04d)] Call db_param_RegGetDefaultValue(%s) : pszValue [%s]\n", __FUNCTION__, __LINE__, pszRegKey, pszValue);
	}

	hErr = HAPI_SetRegistryStr(pszRegKey, pszValue);
	if (ERR_OK == hErr)
	{
//		DAPI_Reset(DxDBSETUP_SYSTEM, eDxItem);
	}

	return hErr;
}

HERROR db_param_MigrateLang(const DxDBSETUP_Item_e eDxItem, const HCHAR *pszRegKey)
{
	HERROR	 hErr;
	HINT32	 nValue = 0;
	HCHAR	 szValue[4] = { 0, };

	if (TRUE == db_param_FindKeepingConfigItem(eDxItem))
	{
		HxLOG_Warning("Skip Keeping Item [%d]\n", eDxItem);
		return ERR_OK;
	}

	hErr = db_param_DamaGetInt(eDxItem, &nValue);
	if (ERR_OK != hErr || nValue == 0 ) //eLangID_NONE
	{
		nValue = 0;
		HxLOG_Print("[%s (%04d)] db_param_DamaGetInt() Return Error!\n", __FUNCTION__, __LINE__);
		dbParam_ENTER_CRITICAL();
		db_param_RegGetDefaultValue(pszRegKey, (HUINT32 *)szValue);
		dbParam_LEAVE_CRITICAL();
		HxLOG_Print("[%s (%04d)] Call db_param_RegGetDefaultValue(%s) : szValue [%s]\n", __FUNCTION__, __LINE__, pszRegKey, szValue);
	}
	else
	{
		HxLOG_Print("[%s (%04d)] db_param_DamaSetInt() nValue [%d]\n", __FUNCTION__, __LINE__, nValue);
		HxSTD_StrNCpy(szValue, HLIB_LANG_IdTo639(nValue), 4);
		HxLOG_Print("[%s (%04d)] szValue : [%s]\n", __FUNCTION__, __LINE__, szValue);
	}

	hErr = HAPI_SetRegistryStr(pszRegKey, szValue);
	if (ERR_OK == hErr)
	{
//		DAPI_Reset(DxDBSETUP_SYSTEM, eDxItem);
	}

	return hErr;
}

/*	Init and Default Functions
 *
 */

HERROR	db_param_MigrateItems(void)
{
#if 0
	if (pdb_param_NeedsMigration() == FALSE)
	{
		return ERR_OK;
	}
#endif

	if (db_param_MigrateIsDxItemMigrated() == FALSE)
	{
		HxLOG_Warning("registry module now ready -- run compatibility module!\n");

		pdb_param_MigrateItems();

		// Write Migrated Flag to Registry
		(void)db_param_MigrateWriteFlag(dbParam_VERSION);

		HxLOG_Warning("finish to run module compatibility");
	}

#if defined(CONFIG_DEBUG)
	db_param_PrintLang();
#endif

	return ERR_OK;
}

/* end of file */
