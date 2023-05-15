/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  hxsqlite.h
	@brief	  sqlite3 wrapper

	Description:   \n
	Module: hlib/sqlite \n
	Remarks :  \n

	Copyright (c) 2011 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__HXSQLITE_H__
#define	__HXSQLITE_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#define HX_SQLITE_IN_MEMORY_DB_NAME		":memory:"

/*******************************************************************/
/********************      Type Definition      ********************/
/*******************************************************************/
typedef void *	SqliteHandle_t;
typedef void *	PrepareHandle_t;

typedef enum
{
	eColType_Integer,
	eColType_Text,
	eColType_Blob,
	eColType_Unknown
} HxSqliteColumnType_e;

typedef enum
{
	eStepResult_MoreRow,
	eStepResult_Done,
	eStepResult_Unknown
} HxSqliteStepResult_e;

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

extern HERROR					HxSqlite_Open(
									const HCHAR 	*szFileName	/* Database filename (UTF-8) */,
									SqliteHandle_t	*hSqlite	/* OUT: SQLite db handle */
								);
extern HERROR					HxSqlite_Close(SqliteHandle_t hSqlite);
extern HERROR					HxSqlite_Exec(
									SqliteHandle_t 	hSqlite,	/* An open database */
									const HCHAR		*szSql		/* SQL to be excuted */
								);
extern HERROR					HxSqlite_Prepare(
									SqliteHandle_t	hSqlite,	/* An open database */
									const HCHAR		*szSql,		/* SQL statement, UTF-8 encoded */
									PrepareHandle_t	*phPrepare	/* OUT: Prepare handle */
								);
extern HxSqliteStepResult_e		HxSqlite_Step(SqliteHandle_t hSqlite,PrepareHandle_t hPrepare);
extern HERROR					HxSqlite_Finalize(SqliteHandle_t hSqlite, PrepareHandle_t hPrepare);
extern HERROR					HxSqlite_BindBlob(PrepareHandle_t hPrepare, HINT32 nIndex, HINT32 nDataSize, const void* pvData);
extern HERROR					HxSqlite_BindInt(PrepareHandle_t hPrepare, HINT32 nIndex, HINT32 nValue);
extern HERROR					HxSqlite_BindText(PrepareHandle_t hPrepare, HINT32 nIndex, const HCHAR *szText);
extern HINT32					HxSqlite_GetColumnInt(PrepareHandle_t hPrepare, HINT32 nColIndex);
extern const HCHAR*				HxSqlite_GetColumnText(PrepareHandle_t hPrepare, HINT32 nColIndex);
extern const void*				HxSqlite_GetColumnBlob(PrepareHandle_t hPrepare, HINT32 nColIndex);
extern HINT32					HxSqlite_GetColumnBytes(PrepareHandle_t hPrepare, HINT32 nColIndex);
extern const HCHAR*				HxSqlite_GetColumnName(PrepareHandle_t hPrepare, HINT32 nColIndex);
extern HxSqliteColumnType_e		HxSqlite_GetColumnType(PrepareHandle_t hPrepare, HINT32 nColIndex);
extern HINT32					HxSqlite_GetColumnCount(PrepareHandle_t hPrepare);

#endif /* !__HXSQLITE_H__ */
