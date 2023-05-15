/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  db_param_default.c
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

/*******************************************************************/
/********************      Local Variables      ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Local Functions      ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

/*	Common Functions
 */

#define	___DEFAULT_VALUE__________________

dbParam_DefaultItem_t *db_param_DefaultValueGetItemByParamKey(const DbParam_Item_e eItem)
{
	HINT32					 i, nItemNum;
	dbParam_DefaultItem_t	*pstItem;

	nItemNum = pdb_param_GetDefaultSystemConfigItemNum();
	for(i=0; i<nItemNum; i++)
	{
		pstItem = pdb_param_GetDefaultSystemConfigItem(i);
		if (pstItem)
		{
			if (eItem == pstItem->eItem)
			{
				return pstItem;
			}
		}
	}

	return NULL;
}

dbParam_DefaultItem_t *db_param_DefaultValueGetItemByDxKey(const DxDBSETUP_Item_e eItem)
{
	HINT32					 i, nItemNum;
	dbParam_DefaultItem_t	*pstItem;

	nItemNum = pdb_param_GetDefaultSystemConfigItemNum();
	for(i=0; i<nItemNum; i++)
	{
		pstItem = pdb_param_GetDefaultSystemConfigItem(i);
		if (pstItem)
		{
			if (db_param_TableGetParamItemByDxItem(eItem) == pstItem->eItem)
			{
				return pstItem;
			}
		}
	}

	return NULL;
}

dbParam_DefaultItem_t *db_param_DefaultValueGetItemByRegKey(const HCHAR *pszKey)
{
	HINT32					 i, nItemNum;
	dbParam_DefaultItem_t	*pstItem;

	nItemNum = pdb_param_GetDefaultSystemConfigItemNum();
	for(i=0; i<nItemNum; i++)
	{
		pstItem = pdb_param_GetDefaultSystemConfigItem(i);
		if (pstItem)
		{
			if (db_param_TableGetParamItemByRegKey((const HCHAR *)pszKey) == pstItem->eItem)
			{
				return pstItem;
			}
		}
	}

	return NULL;
}

/* end of file */
