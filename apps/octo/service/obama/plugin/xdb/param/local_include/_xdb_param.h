/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _xdb_param.h
	@brief	  default setting local header

	Description:  \n
	Module: DB/DEFAULT \n
	Remarks : 										\n

	Copyright (c) 2012 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__XDB_PARAM_INT_H__
#define	__XDB_PARAM_INT_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <_db_main.h>
#include <_db_param_default.h>

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
extern HERROR					 xdb_param_KeepingItemCreateList(const DbParam_Item_e *astKeyList, const HINT32 nListNum, dbParam_KeepingItem_t **pastItem, HINT32 *pnItemNum);
extern HERROR					 xdb_param_KeepingItemFree(dbParam_KeepingItem_t *astItem, HINT32 nItemNum);

#if defined(CONFIG_PROD_HMS1000S)
extern HINT32					 xdb_default_GetSystemConfigItemNum_Hms1000s(void);
extern dbParam_DefaultItem_t	*xdb_default_GetSystemConfigItem_Hms1000s(HUINT32 ulItemIdx);

#elif defined(CONFIG_PROD_IR4000HD)
extern HINT32					 xdb_default_GetSystemConfigItemNum_Ir4000hd(void);
extern dbParam_DefaultItem_t	*xdb_default_GetSystemConfigItem_Ir4000hd(HUINT32 ulItemIdx);
extern HERROR					 xdb_param_GetKeepingConfigItem_Ir4000hd(dbParam_KeepingItem_t **pastItem, HINT32 *pnItemNum);
extern HERROR					 xdb_param_FreeKeepingConfigItem_Ir4000hd(dbParam_KeepingItem_t *astItem, HINT32 nItemNum);
extern HBOOL					 xdb_param_FindKeepingConfigItem_Ir4000hd(DbParam_Item_e eItem);

#elif defined(CONFIG_PROD_ICORDPRO)
extern HINT32					 xdb_default_GetSystemConfigItemNum_Icordpro(void);
extern dbParam_DefaultItem_t	*xdb_default_GetSystemConfigItem_Icordpro(HUINT32 ulItemIdx);
extern HERROR					 xdb_param_GetKeepingConfigItem_Icordpro(dbParam_DefaultItem_t **pastItem, HINT32 *pnItemNum);
extern HERROR					 xdb_param_FreeKeepingConfigItem_Icordpro(dbParam_DefaultItem_t *astItem, HINT32 nItemNum);
extern HBOOL					 xdb_param_FindKeepingConfigItem_Icordpro(DbParam_Item_e eItem);

#elif defined(CONFIG_PROD_HDNEO)
extern HINT32					 xdb_default_GetSystemConfigItemNum_Hdneo(void);
extern dbParam_DefaultItem_t	*xdb_default_GetSystemConfigItem_Hdneo(HUINT32 ulItemIdx);
extern HERROR					 xdb_param_GetKeepingConfigItem_Hdneo(dbParam_DefaultItem_t **pastItem, HINT32 *pnItemNum);
extern HERROR					 xdb_param_FreeKeepingConfigItem_Hdneo(dbParam_DefaultItem_t *astItem, HINT32 nItemNum);
extern HBOOL					 xdb_param_FindKeepingConfigItem_Hdneo(DbParam_Item_e eItem);

#elif defined(CONFIG_PROD_HMS1000T)
extern HINT32					 xdb_default_GetSystemConfigItemNum_Hms1000t(void);
extern dbParam_DefaultItem_t	*xdb_default_GetSystemConfigItem_Hms1000t(HUINT32 ulItemIdx);
#elif defined(CONFIG_PROD_HD9000I)
extern HINT32 					 xdb_default_GetSystemConfigItemNum_HD9000i(void);
extern dbParam_DefaultItem_t	*xdb_default_GetSystemConfigItem_HD9000i(HUINT32 ulItemIdx);
#elif defined(CONFIG_PROD_FVP4000T)
extern HINT32					 xdb_default_GetSystemConfigItemNum_Fvp4000t(void);
extern dbParam_DefaultItem_t	*xdb_default_GetSystemConfigItem_Fvp4000t(HUINT32 ulItemIdx);
#elif defined(CONFIG_PROD_HMX4KBBC)
extern HINT32					 xdb_default_GetSystemConfigItemNum_Hmx4kbbc(void);
extern dbParam_DefaultItem_t	*xdb_default_GetSystemConfigItem_Hmx4kbbc(HUINT32 ulItemIdx);
#elif defined(CONFIG_PROD_DIGIMAXT2)
extern HINT32					 xdb_default_GetSystemConfigItemNum_Digimaxt2(void);
extern dbParam_DefaultItem_t	*xdb_default_GetSystemConfigItem_Digimaxt2(HUINT32 ulItemIdx);

#elif defined(CONFIG_PROD_HDFOXIRCONNECT)
extern HINT32					 xdb_default_GetSystemConfigItemNum_HdFoxIrConnect(void);
extern dbParam_DefaultItem_t	*xdb_default_GetSystemConfigItem_HdFoxIrConnect(HUINT32 ulItemIdx);
extern HERROR					 xdb_param_GetKeepingConfigItem_HdFoxIrConnect(dbParam_KeepingItem_t **pastItem, HINT32 *pnItemNum);
extern HERROR					 xdb_param_FreeKeepingConfigItem_HdFoxIrConnect(dbParam_KeepingItem_t *astItem, HINT32 nItemNum);
extern HBOOL					 xdb_param_FindKeepingConfigItem_HdFoxIrConnect(DbParam_Item_e eItem);

#elif defined(CONFIG_PROD_CDIG2000C)
extern HINT32 					xdb_default_GetSystemConfigItemNum_CDIG2000C(void);
extern dbParam_DefaultItem_t	*xdb_default_GetSystemConfigItem_CDIG2000C(HUINT32 ulItemIdx);

#elif defined(CONFIG_PROD_ROCKYPOM) || defined(CONFIG_PROD_ROCKYWPOM)
extern HINT32 					xdb_default_GetSystemConfigItemNum_ROCKYPOM(void);
extern dbParam_DefaultItem_t	*xdb_default_GetSystemConfigItem_ROCKYPOM(HUINT32 ulItemIdx);

#elif defined(CONFIG_PROD_HGS1000S)
extern HINT32					 xdb_default_GetSystemConfigItemNum_Hgs1000s(void);
extern dbParam_DefaultItem_t	*xdb_default_GetSystemConfigItem_Hgs1000s(HUINT32 ulItemIdx);
extern HERROR					 xdb_param_GetKeepingConfigItem_Hgs1000s(dbParam_DefaultItem_t **pastItem, HINT32 *pnItemNum);
extern HERROR					 xdb_param_FreeKeepingConfigItem_Hgs1000s(dbParam_DefaultItem_t *astItem, HINT32 nItemNum);
extern HBOOL					 xdb_param_FindKeepingConfigItem_Hgs1000s(DbParam_Item_e eItem);

#elif defined(CONFIG_PROD_HA7100S)
extern HINT32					 xdb_default_GetSystemConfigItemNum_Ha7100s(void);
extern dbParam_DefaultItem_t	*xdb_default_GetSystemConfigItem_Ha7100s(HUINT32 ulItemIdx);

#elif defined(CONFIG_PROD_YSR2000)
extern HINT32 					xdb_default_GetSystemConfigItemNum_YSR2000(void);
extern dbParam_DefaultItem_t	*xdb_default_GetSystemConfigItem_YSR2000(HUINT32 ulItemIdx);

#elif defined(CONFIG_PROD_YS1000)
extern HINT32 					xdb_default_GetSystemConfigItemNum_YS1000(void);
extern dbParam_DefaultItem_t	*xdb_default_GetSystemConfigItem_YS1000(HUINT32 ulItemIdx);

#elif defined(CONFIG_PROD_M1)
extern HINT32					 xdb_default_GetSystemConfigItemNum_M1(void);
extern dbParam_DefaultItem_t	*xdb_default_GetSystemConfigItem_M1(HUINT32 ulItemIdx);

#endif



HERROR xdb_param_MigrateItems_Base(void);
#endif /* !__XDB_PARAM_INT_H__ */
