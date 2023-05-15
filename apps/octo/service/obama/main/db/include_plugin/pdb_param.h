/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  db_param.h
	@brief	  default setting local header

	Description:  \n
	Module: DB/DEFAULT \n
	Remarks : 										\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__PDB_PARAM_H__
#define	__PDB_PARAM_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <dlib.h>

#include <octo_common.h>

#include <db_param.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
HINT32					 pdb_param_GetDefaultSystemConfigItemNum (void);
dbParam_DefaultItem_t	*pdb_param_GetDefaultSystemConfigItem (HUINT32 ulItemIdx);
HERROR					 pdb_param_GetKeepingConfigItem(dbParam_KeepingItem_t **pastItem, HINT32 *pnItemNum);
HERROR					 pdb_param_FreeKeepingConfigItem(dbParam_KeepingItem_t *astItem, HINT32 nItemNum);
HBOOL					 pdb_param_FindKeepingConfigItem(DbParam_Item_e eItem);
//HBOOL					 pdb_param_NeedsMigration(void);
HERROR					 pdb_param_MigrateItems(void);


#endif /* !__PDB_PARAM_H__ */
