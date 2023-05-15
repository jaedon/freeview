

#ifndef	__DB_PARAM_MIGRATION_INT_H__
#define	__DB_PARAM_MIGRATION_INT_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <dlib.h>

#include <octo_common.h>

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

HERROR	db_param_MigrateInt(const DxDBSETUP_Item_e eDxItem, const HCHAR *pszRegKey);
HERROR	db_param_MigrateEnum(const DxDBSETUP_Item_e eDxItem, const HCHAR *pszRegKey, const HCHAR *pszEnumType);
HERROR	db_param_MigrateStr(const DxDBSETUP_Item_e eDxItem, const HCHAR *pszRegKey);
HERROR	db_param_MigrateLang(const DxDBSETUP_Item_e eDxItem, const HCHAR *pszRegKey);

#endif /* __DB_PARAM_MIGRATION_INT_H__ */

