/**
	@file     db_default_base7356.c
	@brief    icordhdplus project default data

	Description: default db 생성을 위한 default value 모음
	Module: DB/default \n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <htype.h>
#include <vkernel.h>
#include <octo_common.h>


#include <db_common.h>
#include <db_param.h>
#include <db_svc.h>

#include <_db_main.h>
#include <_db_param_default.h>
#include <pdb_param.h>

#include "./local_include/_xdb_param.h"


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/**************    local function prototypes    ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

HINT32 pdb_param_GetDefaultSystemConfigItemNum(void)
{
#if defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE)
	return xdb_default_GetSystemConfigItemNum_Hms1000s();
#elif defined(CONFIG_PROD_IR4000HD)
	return xdb_default_GetSystemConfigItemNum_Ir4000hd();
#elif defined(CONFIG_PROD_ICORDPRO)
	return xdb_default_GetSystemConfigItemNum_Icordpro();
#elif defined(CONFIG_PROD_HDNEO)
	return xdb_default_GetSystemConfigItemNum_Hdneo();
#elif defined(CONFIG_PROD_HD9000I)
	return xdb_default_GetSystemConfigItemNum_HD9000i();
#elif defined(CONFIG_PROD_HMS1000T)
	return xdb_default_GetSystemConfigItemNum_Hms1000t();
#elif defined(CONFIG_PROD_IHDR6100C)
	return xdb_default_GetSystemConfigItemNum_Ihdr6100c();
#elif defined(CONFIG_PROD_FVP4000T)
	return xdb_default_GetSystemConfigItemNum_Fvp4000t();
#elif defined(CONFIG_PROD_HMX4KBBC)
	return xdb_default_GetSystemConfigItemNum_Hmx4kbbc();
#elif defined(CONFIG_PROD_DIGIMAXT2)
	return xdb_default_GetSystemConfigItemNum_Digimaxt2();
#elif defined(CONFIG_PROD_HDFOXIRCONNECT)
	return xdb_default_GetSystemConfigItemNum_HdFoxIrConnect();
#elif defined(CONFIG_PROD_YSR2000)
	return xdb_default_GetSystemConfigItemNum_YSR2000();
#elif defined(CONFIG_PROD_YS1000)
	return xdb_default_GetSystemConfigItemNum_YS1000();
#elif defined(CONFIG_PROD_CDIG2000C)
	return xdb_default_GetSystemConfigItemNum_CDIG2000C();
#elif defined(CONFIG_PROD_ROCKYPOM) || defined(CONFIG_PROD_ROCKYWPOM)
	return xdb_default_GetSystemConfigItemNum_ROCKYPOM();
#elif defined(CONFIG_PROD_HGS1000S)
	return xdb_default_GetSystemConfigItemNum_Hgs1000s();
#elif defined(CONFIG_PROD_HA7100S)
	return xdb_default_GetSystemConfigItemNum_Ha7100s();
#elif defined(CONFIG_PROD_M1)
	return xdb_default_GetSystemConfigItemNum_M1();
#else
	#error ("xdb_default_GetSystemConfigItemNum_XXX for a Product is not activated!\n")
#endif
}

dbParam_DefaultItem_t	*pdb_param_GetDefaultSystemConfigItem(HUINT32 ulItemIdx)
{
#if defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE)
	return xdb_default_GetSystemConfigItem_Hms1000s(ulItemIdx);
#elif defined(CONFIG_PROD_IR4000HD)
	return xdb_default_GetSystemConfigItem_Ir4000hd(ulItemIdx);
#elif defined(CONFIG_PROD_ICORDPRO)
	return xdb_default_GetSystemConfigItem_Icordpro(ulItemIdx);
#elif defined(CONFIG_PROD_HDNEO)
	return xdb_default_GetSystemConfigItem_Hdneo(ulItemIdx);
#elif defined(CONFIG_PROD_HD9000I)	// TODO:compile
	return xdb_default_GetSystemConfigItem_HD9000i(ulItemIdx);
#elif defined(CONFIG_PROD_HMS1000T)
	return xdb_default_GetSystemConfigItem_Hms1000t(ulItemIdx);
#elif defined(CONFIG_PROD_IHDR6100C)
	return xdb_default_GetSystemConfigItem_Ihdr6100c(ulItemIdx);
#elif defined(CONFIG_PROD_FVP4000T)
	return xdb_default_GetSystemConfigItem_Fvp4000t(ulItemIdx);
#elif defined(CONFIG_PROD_HMX4KBBC)
	return xdb_default_GetSystemConfigItem_Hmx4kbbc(ulItemIdx);
#elif defined(CONFIG_PROD_DIGIMAXT2)
	return xdb_default_GetSystemConfigItem_Digimaxt2(ulItemIdx);
#elif defined(CONFIG_PROD_HDFOXIRCONNECT)
	return xdb_default_GetSystemConfigItem_HdFoxIrConnect(ulItemIdx);
#elif defined(CONFIG_PROD_YSR2000)
	return xdb_default_GetSystemConfigItem_YSR2000(ulItemIdx);
#elif defined(CONFIG_PROD_YS1000)
	return xdb_default_GetSystemConfigItem_YS1000(ulItemIdx);
#elif defined(CONFIG_PROD_CDIG2000C)
	return xdb_default_GetSystemConfigItem_CDIG2000C(ulItemIdx);
#elif defined(CONFIG_PROD_ROCKYPOM) || defined(CONFIG_PROD_ROCKYWPOM)
	return xdb_default_GetSystemConfigItem_ROCKYPOM(ulItemIdx);
#elif defined(CONFIG_PROD_HGS1000S)
	return xdb_default_GetSystemConfigItem_Hgs1000s(ulItemIdx);
#elif defined(CONFIG_PROD_HA7100S)
	return xdb_default_GetSystemConfigItem_Ha7100s(ulItemIdx);
#elif defined(CONFIG_PROD_M1)
	return xdb_default_GetSystemConfigItem_M1(ulItemIdx);
#else
	#error ("xdb_default_GetSystemConfigItem_XXX for a Product is not activated!\n")
#endif
}

HERROR pdb_param_GetKeepingConfigItem(dbParam_KeepingItem_t **pastItem, HINT32 *pnItemNum)
{
	HERROR					 hErr = ERR_FAIL;

	if (NULL == pastItem)
		return ERR_FAIL;

	if (NULL == pnItemNum)
		return ERR_FAIL;

	*pastItem = NULL;
	*pnItemNum = 0;

#if defined(CONFIG_PROD_ICORDPRO)
	hErr = xdb_param_GetKeepingConfigItem_Icordpro(pastItem, pnItemNum);
#elif defined(CONFIG_PROD_HDNEO)
	hErr = xdb_param_GetKeepingConfigItem_Hdneo(pastItem, pnItemNum);
#elif defined(CONFIG_PROD_HGS1000S)
	hErr = xdb_param_GetKeepingConfigItem_Hgs1000s(pastItem, pnItemNum);
#elif defined(CONFIG_PROD_IR4000HD)
	hErr = xdb_param_GetKeepingConfigItem_Ir4000hd(pastItem, pnItemNum);
#elif defined(CONFIG_PROD_HDFOXIRCONNECT)
	hErr = xdb_param_GetKeepingConfigItem_HdFoxIrConnect(pastItem, pnItemNum);
#endif

	return hErr;
}

HERROR pdb_param_FreeKeepingConfigItem(dbParam_KeepingItem_t *astItem, HINT32 nItemNum)
{
	HERROR					 hErr = ERR_FAIL;

	if (NULL == astItem)
		return ERR_FAIL;

#if defined(CONFIG_PROD_ICORDPRO)
	hErr = xdb_param_FreeKeepingConfigItem_Icordpro(astItem, nItemNum);
#elif defined(CONFIG_PROD_HDNEO)
	hErr = xdb_param_FreeKeepingConfigItem_Hdneo(astItem, nItemNum);
#elif defined(CONFIG_PROD_HGS1000S)
	hErr = xdb_param_FreeKeepingConfigItem_Hgs1000s(astItem, nItemNum);
#elif defined(CONFIG_PROD_IR4000HD)
	hErr = xdb_param_FreeKeepingConfigItem_Ir4000hd(astItem, nItemNum);
#elif defined(CONFIG_PROD_HDFOXIRCONNECT)
	hErr = xdb_param_FreeKeepingConfigItem_HdFoxIrConnect(astItem, nItemNum);
#endif

	return hErr;
}

HBOOL pdb_param_FindKeepingConfigItem(DbParam_Item_e eItem)
{
	HBOOL	 bRet = FALSE;

	if (0 == eItem || eDB_PARAM_ITEM_MAX <= eItem)
		return FALSE;

#if defined(CONFIG_PROD_ICORDPRO)
	bRet = xdb_param_FindKeepingConfigItem_Icordpro(eItem);
#elif defined(CONFIG_PROD_HDNEO)
	bRet = xdb_param_FindKeepingConfigItem_Hdneo(eItem);
#elif defined(CONFIG_PROD_HGS1000S)
	bRet = xdb_param_FindKeepingConfigItem_Hgs1000s(eItem);
#elif defined(CONFIG_PROD_IR4000HD)
	bRet = xdb_param_FindKeepingConfigItem_Ir4000hd(eItem);
#elif defined(CONFIG_PROD_HDFOXIRCONNECT)
	bRet = xdb_param_FindKeepingConfigItem_HdFoxIrConnect(eItem);
#endif

	return bRet;
}

#if 0
HBOOL pdb_param_NeedsMigration(void)
{
#if defined(CONFIG_PROD_BASE7356)
	return TRUE;
#elif defined(CONFIG_PROD_HDR1000S)
	return TRUE;
#elif defined(CONFIG_PROD_HMS1000S)
	return TRUE;
#elif defined(CONFIG_PROD_IR4000HD)
	return FALSE;
#elif defined(CONFIG_PROD_ICORDPRO)
	return FALSE;
#elif defined(CONFIG_PROD_HD9000I)
	return FALSE;
#elif defined(CONFIG_PROD_HMS1000T)
	return FALSE;
#elif defined(CONFIG_PROD_IHDR6100C)
	return FALSE;
#elif defined(CONFIG_PROD_YSR2000)
	return FALSE;
#elif defined(CONFIG_PROD_YS1000)
	return FALSE;
#else
	#error ("Need Migration Flag for a Product is not activated!\n")
#endif
}
#endif

HERROR pdb_param_MigrateItems(void)
{
	return xdb_param_MigrateItems_Base();
}

/*********************** End of File ******************************/

