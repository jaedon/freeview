/**************************************************************
 *	@file		dama.c
 *	Humax Data Manager for OCTO 2.0
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/03/01
 *	@author			humax
 *	@brief			HumaxDataManager for OCTO 2.0 Project
 **************************************************************/
#include <unistd.h>
#include <signal.h>


#include <vkernel.h>

#include <hlib.h>

#include <dlib.h>
#include "dama_int.h"

#include "dama_db.h"
#define DAMA_DB_NEW
#define	CACHE_USE_HASHTABLE

#if defined(CONFIG_DEBUG) && 0
#define DAMA_MONITOR_DATA(db,name,data,size)	\
	do {\
		if (name == NULL || HLIB_TREE_Lookup(db->pTableMonitor,name)){\
			HUINT32 _i_,_j_;	\
			HUINT8  *_c_ = (HUINT8 *)data;	\
			HCHAR	_ascii_[17];\
			Hx_Print("[01;33m [%s:%d:%s] [NUM:%d,SEQ:%d] DATA(0x%X, size:%u) [00;39;49m \n", __FUNCTION__, __LINE__, name, db->nDataMonitorNo, db->nDataMonitorSeq, (int)data, size);\
			fprintf(db->fpDataMonitor, "\n\n[%s:%d:%s] [NUM:%d] DATA(0x%X, size:%u)\n", __FUNCTION__, __LINE__, name, db->nDataMonitorNo, (int)data, size);\
			for (_i_ = 0, _j_ = 0 ; _i_ < size ; _i_++, _c_++) {\
				if (_i_ != 0 && _i_ % 16 == 0) {\
					_ascii_[_j_] = '\0';\
					_j_ = 0;\
					fprintf(db->fpDataMonitor, "%s\n", _ascii_);\
				}\
				_ascii_[_j_++] = isalnum(*_c_) ? *_c_ : '.';\
				fprintf(db->fpDataMonitor, "%02X ", *_c_);\
			}\
			if (_j_) {\
				_ascii_[_j_] = '\0';\
				fprintf(db->fpDataMonitor, "%s\n", _ascii_);\
			}\
			db->nDataMonitorNo++;\
		}\
	} while (0)
#endif

typedef struct
{
	DAMA_DB_t		*db;
	HBOOL			 useCache;
} damaDbDelUserData_t;

typedef struct
{
	DAMA_DB_t		*db;
	HCHAR			*szQuery;
	HBOOL			 bFound;
} damaDbCheckQuery_t;


static HBOOL s_bIsChangedData = FALSE;
static HBOOL dama_db_SetIsChangedData(HBOOL bIsChangedData)
{
	s_bIsChangedData = bIsChangedData;
	return s_bIsChangedData;
}

static HBOOL dama_db_GetIsChangedData(void)
{
	return s_bIsChangedData;
}

static	HBOOL	dama_db_syncTable(DAMA_DB_t *db, DAMA_Table_t *table);
static	DERROR	dama_db_makeNewDb (DAMA_DB_t *db, const HCHAR *objName, const  const HCHAR *fileName, DAMA_Table_t	*table);
static	DERROR	dama_db_loadFileDB (DAMA_DB_t *db, const HCHAR *objName, const  const HCHAR *fileName, DAMA_Table_t	*table);
static	DERROR	dama_db_createTable(DAMA_DB_t *db, const HCHAR *objName, HBOOL bBootTime);
static	HBOOL	dama_db_deleteFiles(const HCHAR *objName, const HCHAR *tags[], void *userdata);
static	DERROR	dama_db_deleteTable(DAMA_DB_t *db, const HCHAR *name, HBOOL useCache);

#define	DamaLOG(...) dama_CmdDebug (__FUNCTION__, __VA_ARGS__)

static void	dama_CmdDebug (const HCHAR *function, const HCHAR *format, ...)
{
	HCHAR	buf[512];
	HCHAR	*msg = NULL, *dbuf = NULL;
	va_list	ap, cp;
	HINT32	n;
	static	FILE	*cmdlogfp = NULL;



	if(cmdlogfp == NULL)
	{
		cmdlogfp  = fopen("dama.sql.comm", "w");
	}

	dbuf = NULL;


	if(cmdlogfp)
	{
		va_start(ap, format);
		va_copy(cp, ap);
		n = HxSTD_PrintToStrV(buf, 512, format, ap);
		if (n >= 512 || n < 0)
		{
			dbuf = HLIB_STD_MemAlloc(4096);
			if (dbuf == NULL)
			{
				va_end(cp);
				va_end(ap);
				return;
			}
			msg = dbuf;
			HxSTD_PrintToStrV(dbuf, 4096, format, cp);
		}
		else
		{
			msg  = buf;
		}
		va_end(cp);
		va_end(ap);

		if(function) fwrite(function, HxSTD_StrLen(function), 1, cmdlogfp);
		if(msg) fwrite(msg, HxSTD_StrLen(msg), 1, cmdlogfp);
	}

	HxLOG_Critical("SQLCMD [%s] %s\n", function, msg);

	if (dbuf)
		HLIB_STD_MemFree(dbuf);
	return;
}

static DAMA_Result_t*	dama_db_queryData(DAMA_DB_t *db, const HCHAR *name, const HCHAR *query, HBOOL bPreload);
static HUINT32	dama_db_deleteLinkByUid (DAMA_DB_t *db, const HCHAR *name, HUID	uid, HBOOL useCache);

static HBOOL	dama_db_eachDaMaObject (const HCHAR *objectName, const HCHAR *tags[], void *userdata)
{
	DAMA_ObjIter_t *iter = (DAMA_ObjIter_t *)userdata;
	HINT32	i;

	for (i = 0 ; tags[i] ; i++)
	{
		if (strcmp(tags[i], "DaMa") == 0)
		{
			iter->func(objectName, tags, iter->userdata);
			break;
		}
	}
	return FALSE;
}

static void	dama_db_forEachObject (HBOOL (*func)(const HCHAR *objectName, const HCHAR *tags[], void *userdata), void *userdata)
{
	DAMA_ObjIter_t	iter;

	iter.func = func;
	iter.userdata = userdata;

	DLIB_ForEachObject(dama_db_eachDaMaObject, &iter);
}

static DERROR	dama_db_deleteLink(const HCHAR *pszObjectName, HUID uid, const HCHAR *link)
{
	return DLIB_WriteData(pszObjectName, uid, link, NULL, 0);
}

static HBOOL	dama_db_isIndexable(const HCHAR *pszObjectName, HINT32 nIndex)
{
#if !defined(CONFIG_TEMP_DLIB_NO_AUTOMATIC_TRASHING)
{
	const DxField_t *field;

	field = DLIB_GetField(pszObjectName, nIndex, NULL, 0);
	if (field == NULL)
		return FALSE;

	return field->indexable;
}
#else
{
	HBOOL				 bIndexable = FALSE;
	const DxField_t		*pstField = DLIB_CreateGetFieldData (pszObjectName, nIndex, NULL, 0);

	if (NULL != pstField)
	{
		bIndexable = pstField->indexable;
		DLIB_DestroyGetFieldData(pstField);
	}

	return bIndexable;
}
#endif
}

static const HCHAR*	dama_db_getFieldTypeString(const HCHAR *pszObjectName, HINT32 nIndex)
{
#if !defined(CONFIG_TEMP_DLIB_NO_AUTOMATIC_TRASHING)
{
	const DxField_t *field;

	field = DLIB_GetField(pszObjectName, nIndex, NULL, 0);
	if (field == NULL)
		return "text";

	switch (field->type)
	{
	case eDxTYPE_STRING:
		return "text";
	case eDxTYPE_INTEGER:
		return "integer(4)";
	case eDxTYPE_BINARY:
		return "BLOB";
	default:
		break;
	}
	return "text";
}
#else
{
	const HCHAR			*pszTypeString = NULL;
	const DxField_t		*pstField = NULL;

	pstField = DLIB_CreateGetFieldData(pszObjectName, nIndex, NULL, 0);
	if (NULL == pstField)
	{
		pszTypeString = "text";
		goto END_FUNC;
	}

	switch (pstField->type)
	{
	case eDxTYPE_STRING:
		pszTypeString = "text";
		break;

	case eDxTYPE_INTEGER:
		pszTypeString = "integer(4)";
		break;

	case eDxTYPE_BINARY:
		pszTypeString = "BLOB";
		break;

	default:
		break;
	}

END_FUNC:
	if (NULL != pstField)
	{
		DLIB_DestroyGetFieldData(pstField);
	}

	if (HxSTD_StrEmpty(pszTypeString))
	{
		pszTypeString = "text";
	}

	return pszTypeString;
}
#endif
}

static const HCHAR*	dama_db_getFieldName(DAMA_DB_t *db, const HCHAR *pszObjectName, HINT32 nIndex, HCHAR *pszBuffer)
{
#if !defined(CONFIG_TEMP_DLIB_NO_AUTOMATIC_TRASHING)
{
	const DxField_t *field;

	field = DLIB_GetField(pszObjectName, nIndex, NULL, 0);
	if (field == NULL)
		return NULL;
	return field->name;
}
#else
{
	HUINT32				 ulLength;
	const HCHAR			*pszRetString = NULL;
	const DxField_t		*pstField = NULL;

	if (NULL == pszBuffer)
	{
		goto END_FUNC;
	}

	pstField = DLIB_CreateGetFieldData(pszObjectName, nIndex, NULL, 0);
	if (NULL == pstField)
	{
		goto END_FUNC;
	}

	ulLength = HxSTD_StrLen(pstField->name);
	if (ulLength >= DxLIB_MAX_NAME)
	{
		ulLength = DxLIB_MAX_NAME - 1;
	}

	HxSTD_MemCopy(pszBuffer, pstField->name, ulLength);
	pszBuffer[ulLength] = '\0';

	pszRetString = (const HCHAR *)pszBuffer;

END_FUNC:
	if (NULL != pstField)
	{
		DLIB_DestroyGetFieldData(pstField);
	}

	return pszRetString;
}
#endif
}

static	DAMA_Table_t*	dama_db_getTable(DAMA_DB_t *db, const HCHAR *objName)
{
	if ((db == NULL) || (NULL == objName) || ('\0' == *objName))
		return NULL;
	return (DAMA_Table_t*)HLIB_HASH_Lookup(db->pTableMap, (void*)objName);
}

static DAMA_Result_t *	dama_db_getTempResult (DAMA_DB_t *db)
{
	DAMA_Result_t *result;

	result = &(db->stTempResult.aResult[db->stTempResult.nIndex]);
	db->stTempResult.nIndex = (db->stTempResult.nIndex + 1) % DAMA_DB_RESULT_MAX;

	if (result->pList == NULL)
	{
		result->pList = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))HLIB_STD_MemFree_CB, NULL);
		if (result->pList == NULL)
			return NULL;
	}
	HLIB_VECTOR_RemoveAll(result->pList);
	result->nIndex = 0;
	return result;
}


static const HCHAR *	dama_db_makeSQL (DAMA_DB_t *db, const HCHAR *fmt, ...)
{
	va_list	ap;

	HLIB_RWSTREAM_Rewind(db->nSqlBuf);

	va_start(ap, fmt);
	HLIB_RWSTREAM_VPrint(db->nSqlBuf, fmt, ap);
	va_end(ap);

	return HLIB_RWSTREAM_GetBuf(db->nSqlBuf);
}

static const HCHAR *	dama_db_makeSQLForTableCreation(DAMA_DB_t *db, const HCHAR *tableName, HBOOL overwrite, HBOOL index)
{
	HINT32	i, n;
	const HCHAR *field;
	HCHAR			 bufferForField[DxLIB_MAX_NAME];

	n = DLIB_GetFieldCount(tableName);
	HLIB_RWSTREAM_Rewind(db->nSqlBuf);
	if (overwrite)
	{
		for (i = 0; i < n; i++)
		{
			if (dama_db_isIndexable(tableName, i))
			{
				field = dama_db_getFieldName(db, tableName, i, bufferForField);
				HLIB_RWSTREAM_Print(db->nSqlBuf, "DROP INDEX IF EXISTS %s_%s;", tableName, field);
			}
		}
		HLIB_RWSTREAM_Print (db->nSqlBuf, "DROP TABLE IF EXISTS %s;", tableName);
	}

	HLIB_RWSTREAM_Print(db->nSqlBuf, "CREATE TABLE IF NOT EXISTS %s (", tableName);
	HLIB_RWSTREAM_Print(db->nSqlBuf, "[uid] integer(%d) PRIMARY KEY UNIQUE", sizeof(HUID));

	//	Get Keyword list from module so.
	for (i = 0; i < n; i++)
	{
		HLIB_RWSTREAM_Print(db->nSqlBuf, ", [%s] %s", dama_db_getFieldName(db, tableName, i, bufferForField), dama_db_getFieldTypeString(tableName, i));
	}

	HLIB_RWSTREAM_Print(db->nSqlBuf, ", [link] TEXT");
	HLIB_RWSTREAM_Print(db->nSqlBuf, ", [linkdata] BLOB");
	HLIB_RWSTREAM_Print(db->nSqlBuf, ");");

	if (index)
	{
		for (i = 0; i < n; i++)
		{
			if (dama_db_isIndexable(tableName, i))
			{
				field = dama_db_getFieldName(db, tableName, i, bufferForField);
				HLIB_RWSTREAM_Print(db->nSqlBuf, "CREATE INDEX IF NOT EXISTS %s_%s ON %s(%s ASC);", tableName, field, tableName, field);
			}
		}
	}

	return HLIB_RWSTREAM_GetBuf(db->nSqlBuf);
}


#ifdef CONFIG_DEBUG
static int dama_db_sqlite_exec(sqlite3* db, const char * sql, int (*callback)(void*,int,char**,char**), void * arg)
{
	HCHAR	*errmsg = NULL;
	int		ret = -1;

	ret = sqlite3_exec(db, sql, callback, arg, &errmsg);
	if(errmsg)
	{
		HxLOG_Error("sqlite3_exe error sql[%s] errmsg[%s]\n", sql, errmsg);
		sqlite3_free(errmsg);
	}
	return ret;
}
#else
#define dama_db_sqlite_exec(db, sql, callback, arg) sqlite3_exec(db, sql, callback, arg, NULL)
#endif


static	HCHAR*	dama_db_makeSQLForInsertion(DAMA_DB_t *db, const HCHAR *name, HUID uid)
{
	HINT32	i, nFieldNum;
	HUINT32	 ulRetStrLen = 0;
	HCHAR	*pszSQLInsert;
	HCHAR	*pszReturn;

	nFieldNum = DLIB_GetFieldCount(name);

	HLIB_RWSTREAM_Rewind(db->nSqlBuf);
	HLIB_RWSTREAM_Print(db->nSqlBuf, "INSERT INTO %s ('uid'", name);
	for (i = 0; i < nFieldNum; i++)
	{
		HCHAR			 bufferForField[DxLIB_MAX_NAME];

		HLIB_RWSTREAM_Print(db->nSqlBuf, ", '%s'", dama_db_getFieldName(db, name, i, bufferForField));
	}
	HLIB_RWSTREAM_Print(db->nSqlBuf, ", link");
	HLIB_RWSTREAM_Print(db->nSqlBuf, ", linkdata) VALUES(%"HUID_FMT);

	for (i = 0; i < nFieldNum; i++)
	{
		HLIB_RWSTREAM_Print(db->nSqlBuf, ", ?");
	}
	HLIB_RWSTREAM_Print(db->nSqlBuf, ", ?, ?);");

	pszSQLInsert = HLIB_RWSTREAM_GetBuf(db->nSqlBuf);
	if ((NULL == pszSQLInsert) || ('\0' == pszSQLInsert[0]))
	{
		HxLOG_Error("HLIB_RWSTREAM_GetBuf(db->nSqlBuf) err:\n");
		return NULL;
	}

	ulRetStrLen = HxSTD_StrLen(pszSQLInsert) + 16;
	pszReturn = HLIB_STD_MemAlloc(ulRetStrLen);
	if (NULL == pszReturn)
	{
		HxLOG_Error("HLIB_STD_MemAlloc err:\n");
		return NULL;
	}

	HxSTD_snprintf(pszReturn, ulRetStrLen - 1, pszSQLInsert, uid);
	pszReturn[ulRetStrLen - 1] = '\0';

	HxLOG_Debug("[%s]\n", pszReturn);

	return pszReturn;
}

static	HCHAR*	dama_db_makeSQLForSelection(DAMA_DB_t *db, const HCHAR *name, HUID uid)
{
	HUINT32	 ulRetStrLen = 0;
	HCHAR	*pszReturn;
	HCHAR	*pszSQLFind;

	HLIB_RWSTREAM_Rewind(db->nSqlBuf);
	HLIB_RWSTREAM_Print(db->nSqlBuf, "SELECT link, linkdata FROM %s WHERE uid=%"HUID_FMT";", name);
	pszSQLFind = HLIB_RWSTREAM_GetBuf(db->nSqlBuf);
	if ((NULL == pszSQLFind) || ('\0' == pszSQLFind[0]))
	{
		HxLOG_Error("HLIB_RWSTREAM_GetBuf(db->nSqlBuf) err:\n");
		return NULL;
	}

	ulRetStrLen = HxSTD_StrLen(pszSQLFind) + 16;
	pszReturn = HLIB_STD_MemAlloc(ulRetStrLen);
	if (NULL == pszReturn)
	{
		HxLOG_Error("HLIB_STD_MemAlloc err:\n");
		return NULL;
	}

	HxSTD_snprintf(pszReturn, ulRetStrLen - 1, pszSQLFind, uid);
	pszReturn[ulRetStrLen - 1] = '\0';

	HxLOG_Debug("[%s]\n", pszReturn);

	return pszReturn;
}

static	HCHAR*	dama_db_makeSQLForUpdate(DAMA_DB_t *db, const HCHAR *name, HUID uid)
{
	HINT32	i, nFieldNum;
	HUINT32	 ulRetStrLen = 0;
	HCHAR	*pszReturn;
	HCHAR	*pszSQLUpdate;

	nFieldNum = DLIB_GetFieldCount(name);

	HLIB_RWSTREAM_Rewind(db->nSqlBuf);
	HLIB_RWSTREAM_Print(db->nSqlBuf, "UPDATE %s SET ", name);
	for (i = 0; i < nFieldNum; i++)
	{
		HCHAR			 bufferForField[DxLIB_MAX_NAME];

		HLIB_RWSTREAM_Print(db->nSqlBuf, "%s=?, ", dama_db_getFieldName(db, name, i, bufferForField));
	}
	HLIB_RWSTREAM_Print(db->nSqlBuf, " link=?, linkdata=? WHERE uid=%"HUID_FMT";");

	pszSQLUpdate = HLIB_RWSTREAM_GetBuf(db->nSqlBuf);
	if ((NULL == pszSQLUpdate) || ('\0' == pszSQLUpdate[0]))
	{
		HxLOG_Error("HLIB_RWSTREAM_GetBuf(db->nSqlBuf) err:\n");
		return NULL;
	}

	ulRetStrLen = HxSTD_StrLen(pszSQLUpdate) + 16;
	pszReturn = HLIB_STD_MemAlloc(ulRetStrLen);
	if (NULL == pszReturn)
	{
		HxLOG_Error("HLIB_STD_MemAlloc err:\n");
		return NULL;
	}

	HxSTD_snprintf(pszReturn, ulRetStrLen - 1, pszSQLUpdate, uid);
	pszReturn[ulRetStrLen - 1] = '\0';

	HxLOG_Debug("pszSQLUpdate : [%s]\n", pszReturn);
	return pszReturn;
}

static HBOOL	dama_db_copyFile(const HCHAR *src, const HCHAR *dst)
{
	HCHAR	buf[HxPATH_MAX], *dir;

	if (!HLIB_FILE_Exist(src))
	{
		HxLOG_Warning("There is no file(%s)\n", src);
		return FALSE;
	}

	dir = HLIB_PATH_DirName(dst, buf, HxPATH_MAX);
	if(dir)
	{
		if(HLIB_FILE_Exist(dir) == FALSE)
		{
			HLIB_DIR_Make(dir);
		}
#if defined(CONFIG_DROP_ROOT_PRIVILEGES)
		if (HLIB_EXTRA_ChownWithParam(dir, UID_dama, UID_settop) != ERR_OK)
		{
			HxLOG_Error("Fail to chown path = %s \n", dir);
		}

		if (HLIB_EXTRA_Chmod(dir, 0770) != ERR_OK)
		{
			HxLOG_Error("Fail to chmod path = %s \n", dir);
		}
#endif
	}

	if (HLIB_FILE_Exist(dst))
	{
		HLIB_FILE_Delete(dst);
	}

	if (HLIB_FILE_Copy(src, dst) != ERR_OK)
	{
		HxLOG_Warning("error to making backup file! (%s : %s)\n", src, dst);
		return FALSE;
	}
	return TRUE;
}

static DERROR	dama_db_getDbPath (DAMA_DB_t *db, const HCHAR *objectName, HINT32 dbIndex, HCHAR *szPath)
{
	if (NULL == szPath)
	{
		return DERR_FAIL;
	}

	//	return tmp dir
	if (dbIndex == -1)
	{
		HxSTD_snprintf(szPath, MAX_DB_PATH_LEN + MAX_DB_FILE_LEN - 1, "%s/%s.db", db->szTempPath, objectName);
	}
	else
	{
		//	Using Default DB.
		if (DLIB_GetDBPath(objectName, 0) == NULL)
		{
			if (dbIndex == 0)
				HxSTD_snprintf(szPath, MAX_DB_PATH_LEN + MAX_DB_FILE_LEN - 1, "%s/%s.db", db->szPath, objectName);
			else if (dbIndex == 1)
				HxSTD_snprintf(szPath, MAX_DB_PATH_LEN + MAX_DB_FILE_LEN - 1, "%s/%s.db", db->szBackupPath, objectName);
			else
				return DERR_FAIL;
		}
		else
		{
			HCHAR			*szDlibPath;

			szDlibPath = (HCHAR*)DLIB_GetDBPath(objectName, dbIndex);
			if (NULL == szDlibPath)
			{
				HxLOG_Error("DLIB_GetDBPath err:objName[%s], dbIndex[%d]\n", objectName, dbIndex);
				return DERR_FAIL;
			}

			HxSTD_StrNCpy(szPath, szDlibPath, MAX_DB_PATH_LEN + MAX_DB_FILE_LEN - 1);
		}
	}

	szPath[MAX_DB_PATH_LEN + MAX_DB_FILE_LEN - 1] = '\0';
	return ERR_OK;
}

static HBOOL	dama_db_backupTable (DAMA_DB_t *db, DAMA_Table_t *table)
{
	HINT32		i = 1;
	HINT32		c = 0;
	HCHAR		*szOrgPath = NULL;
	HCHAR		*szDstPath = NULL;
	DERROR		 eErr;

	szOrgPath = (HCHAR *)HLIB_STD_MemAlloc(MAX_DB_PATH_LEN + MAX_DB_FILE_LEN);
	szDstPath = (HCHAR *)HLIB_STD_MemAlloc(MAX_DB_PATH_LEN + MAX_DB_FILE_LEN);
	if ((NULL == szOrgPath) || (NULL == szDstPath))
	{
		HxLOG_Error("memory allocation failed:\n");
		goto END_FUNC;
	}


	eErr = dama_db_getDbPath(db, table->szObjName, 0, szOrgPath);
	if (DERR_OK != eErr)
	{
		HxLOG_Error("dama_db_getDbPath err: objectName[%s], dbIndex[%d]\n", table->szObjName, 0);
		goto END_FUNC;
	}

	while (dama_db_getDbPath(db, table->szObjName, i++, szDstPath) == DERR_OK)
	{
		HxLOG_Debug("Start BACKUP : [%s] --> [%s]\n", szOrgPath, szDstPath);
		if (HxSTD_StrCmp(szDstPath, szOrgPath) == 0)
		{
			HxLOG_Critical("BACKUP FILE is SAME [%s]\n", szDstPath);
			continue;
		}

		if (dama_db_copyFile(szOrgPath, szDstPath))
		{
			HxLOG_Message("[%s] Backup success: org : %s des : %s\n", table->szObjName, szOrgPath, szDstPath);
			c++;
		}
		else
		{
			DAMA_ERROR("[%s] BACKUP FAILED: copy %s to %s", table->szObjName, szOrgPath, szDstPath);
		}
	}

END_FUNC:
	if (NULL != szOrgPath)					{ HLIB_STD_MemFree(szOrgPath); }
	if (NULL != szDstPath)					{ HLIB_STD_MemFree(szDstPath); }

	return c > 0 ? TRUE : FALSE;
}


static	HBOOL	dama_db_hasChanged(DAMA_DB_t *db)
{
	HxHASH_TableIter_t	iter;
	DAMA_Table_t		*table;
	if (db->pTableMap == NULL)
		return FALSE;

	HLIB_HASH_IterInit(&iter, db->pTableMap);
	while (HLIB_HASH_IterNext(&iter, NULL, (void **)&table))
	{
		if (table->bUpdated)
			return TRUE;
	}
	return FALSE;
}

static	void	dama_db_syncDatabase(DAMA_DB_t *db)
{
	HxHASH_TableIter_t	iter;
	DAMA_Table_t		*table;
	const HCHAR 		*path;

	if (db->pTableMap == NULL)
		return;

	HLIB_HASH_IterInit(&iter, db->pTableMap);
	while (HLIB_HASH_IterNext(&iter, NULL, (void **)&table))
	{
		if (!table->bUpdated)
			continue;
#if defined(CONFIG_DAMA_BACKGROUND_SYNC)
		if (table->bSyncInOtherThread)
		{
			if (table->bInSyncJob)
			{
				continue;
			}

			if ((0 != table->ulLastSyncTick) && ((HLIB_STD_GetSystemTick() - table->ulLastSyncTick) < (30 * 1000)))
			{
				continue;
			}
		}
#endif
#if defined(CONFIG_DAMA_BACKGROUND_LOAD)
		if ((TRUE == table->bLoadInOtherThread) && (TRUE != table->bLoadDone))
		{
			continue;
		}
#endif

		path = DLIB_GetDBPath(table->szObjName, 0);
		if (path)
		{
			if(HLIB_STD_StrCaseCmp(path, ":memory:") == 0)
			{
				table->bManualUpdate = FALSE;
				table->bUpdated = FALSE;
				continue;
			}

			if(!HLIB_DIR_IsMountedPath(path))
			{
				continue;
			}
		}

		if (!table->bManualUpdate)
		{
			if (table->ulSyncCycle == DAMA_MANUAL_SYNC)
				continue;
			if (table->ulLastSyncTime + table->ulSyncCycle > HLIB_STD_GetSystemTick())
			{
				HxLOG_Print("Sync Table Name[%s] Time[%d] [%d]\n",table->szObjName ,table->ulLastSyncTime + table->ulSyncCycle, HLIB_STD_GetSystemTick());
				continue;
			}
		}

		HxLOG_Info("%s:%s(%s):"HxANSI_COLOR_CYAN("synced!!")"\n"
			, HLIB_DATETIME_UnixTimeToString(HLIB_STD_GetSystemTime(), NULL, 0), __FUNCTION__, table->szObjName);

#if defined(CONFIG_DAMA_BACKGROUND_SYNC)
		if (table->bSyncInOtherThread)
		{
			HCHAR		*szTemp = NULL;
			HCHAR		*szNewTemp = NULL;
			HERROR		 hErr;

			szTemp = (HCHAR *)HLIB_STD_MemAlloc(MAX_DB_PATH_LEN + MAX_DB_FILE_LEN);
			szNewTemp = (HCHAR *)HLIB_STD_MemAlloc(MAX_DB_PATH_LEN + MAX_DB_FILE_LEN);
			if ((NULL != szTemp) && (NULL != szNewTemp))
			{
				hErr = dama_db_getDbPath(db, table->szObjName, -1, szTemp);
				if (DERR_OK == hErr)
				{
					HxSTD_PrintToStrN(szNewTemp, MAX_DB_PATH_LEN + MAX_DB_FILE_LEN - 1, "%s.tmp", szTemp);
					szNewTemp[MAX_DB_PATH_LEN + MAX_DB_FILE_LEN - 1] = '\0';

					if (HLIB_FILE_Exist((const HUINT8 *)szNewTemp))
					{
						HLIB_FILE_Delete((const HUINT8 *)szNewTemp);
					}

					hErr = HLIB_FILE_Copy((const HUINT8 *)szTemp, (const HUINT8 *)szNewTemp);
					if (ERR_OK == hErr)
					{
						// Thread Safe¸¦ À§ÇØ SyncData¸¸Àº ¿©±â¼­ ÇÑ´Ù.
						(void) DLIB_SyncData(table->szObjName);

						DAMA_TakeSyncJob();
						hErr = DAMA_SendSyncCmdToSyncThread(eDAMA_ASYNCTHREAD_MSGTYPE_SyncTable, (void *)db, table->szObjName, szTemp, szNewTemp);
						if (ERR_OK == hErr)
						{
							table->bInSyncJob = TRUE;
						}
						else
						{
							DAMA_ReleaseSyncJob();
						}
					}
				}
			}

			if (NULL != szTemp)					{ HLIB_STD_MemFree(szTemp); }
			if (NULL != szNewTemp)				{ HLIB_STD_MemFree(szNewTemp); }
		}

		if (!table->bInSyncJob)
#endif
		{
			(void) DLIB_SyncData(table->szObjName);
			(void) dama_db_backupTable(db, table);
			(void) dama_db_syncTable(db, table);
		}

		table->bManualUpdate = FALSE;
		table->bUpdated = FALSE;

	}
}

static	DERROR	dama_db_removeItem(DAMA_DB_t *db, const HCHAR *tableName, HUID uid, HBOOL useCache)
{
	const HCHAR		*sql;
	DAMA_Table_t	*table;

	table = dama_db_getTable(db, tableName);
	if (table)
	{
#if defined(CONFIG_DAMA_BACKGROUND_LOAD)
		if ((TRUE == table->bLoadInOtherThread) && (TRUE != table->bLoadDone))
		{
			HxLOG_Error("Not Initialized yet... Do not Reset!!! name(%s)\n", tableName);
			return DERR_OK;
		}
#endif

		dama_db_deleteLinkByUid(db, tableName, uid, useCache);

		sql = dama_db_makeSQL(db,
				"DELETE FROM %s WHERE uid="HUID_FMT
				, tableName
				, uid
			);
		if (dama_db_sqlite_exec(db->pMainDB, sql, NULL, 0) != SQLITE_OK)
		{
			DAMA_ERROR("[%s] RemoveItem FAILED", tableName);
			return DERR_FAIL;
		}

		s_bIsChangedData = TRUE;
		table->bUpdated = TRUE;
		//	TODO: 64bit´Â ¾îÄÉ Áö¿ø??
#ifdef CACHE_USE_HASHTABLE
		if (table->bUseInternalCache && table->pstCacheData && useCache)
			HLIB_HASH_Remove(table->pstCacheData, (void*)(HUINT32)uid);
#endif
	}

	DAMA_TRACE("[%s] RemoveItem(%lld) SUCCESS", tableName, uid);
	return DERR_OK;
}


static HCHAR *	dama_db_getReloadPath (DAMA_DB_t *db, const HCHAR *objName, HCHAR *reloadPath, HUINT32 n)
{
	HINT32	 i;
	HCHAR	*szPath = NULL;
	HCHAR	*szResult = NULL;
	HERROR	 hErr;

	szPath = (HCHAR *)HLIB_STD_MemAlloc(MAX_DB_PATH_LEN + MAX_DB_FILE_LEN);
	if (NULL == szPath)
	{
		HxLOG_Error("Memory allocation failed:\n");
		goto END_FUNC;
	}

	for (i = 0; dama_db_getDbPath(db, objName, i, szPath) == DERR_OK; i++)
	{
		if (HLIB_FILE_Exist(szPath))
		{
			goto END_FUNC;
		}
	}

	hErr = dama_db_getDbPath(db, objName, 0, szPath);
	if ((DERR_OK == hErr) && !HLIB_DIR_IsMountedPath(szPath))
	{
		HLIB_STD_StrNCpySafe(reloadPath, szPath, n);
		szResult = reloadPath;
		goto END_FUNC;
	}

END_FUNC:
	if (NULL != szPath)					{ HLIB_STD_MemFree(szPath); }
	return szResult;
}

static	HCHAR*	dama_db_getLink(DAMA_DB_t *db, const HCHAR *name, HUID uid, const void *src, HUINT32 srcSize)
{
	const HCHAR	*linkInfo;
	HCHAR		*szNameBuf = NULL;

	linkInfo = DLIB_GetDataPath(name);
	if (linkInfo == NULL)
		return NULL;				//	using BLOB field.

	szNameBuf = HLIB_STD_MemAlloc(256);
	if (NULL == szNameBuf)
	{
		HxLOG_Error("Memory allocation failed:\n");
		return NULL;
	}

	if (DLIB_GetLinkName(name, uid, src, srcSize, szNameBuf, 256) != TRUE)
	{
		HxSTD_snprintf(szNameBuf, 255, "%s/"HUID_FMT".file", linkInfo, uid);
	}

	szNameBuf[255] = '\0';
	return szNameBuf;
}

static	DERROR	dama_db_updateLink(DAMA_DB_t *db, const HCHAR *name, HUID uid, const HCHAR *pszLink, HINT32 size, const void *pvData)
{
	return DLIB_WriteData(name, uid, pszLink, pvData, (HUINT32)size);
}

static	DERROR	dama_db_readLink(DAMA_DB_t *db, const HCHAR *name, HUID uid, const HCHAR *pszLink, HINT32 *pnSize, void **ppvData)
{
	return DLIB_ReadData(name, uid, pszLink, ppvData, (HUINT32 *)pnSize);
}


static HUINT32	dama_db_deleteLinkByUid (DAMA_DB_t *db, const HCHAR *name, HUID	uid, HBOOL useCache)
{
	sqlite3_stmt	*stmt;
	const HCHAR		*sql;
	HUINT32			count = 0;

	sql = dama_db_makeSQL(db, "SELECT link FROM %s where uid="HUID_FMT, name, uid);

	HxLOG_Debug(HxANSI_COLOR_YELLOW("%s")"\n", sql);

	if (sqlite3_prepare_v2(db->pMainDB, sql, HxSTD_StrLen(sql), &stmt, NULL) != SQLITE_OK)
	{
		DAMA_ERROR("[%s] prepare failed: %s", name, sql);
		return 0;
	}

	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		HUID	uid = (HUID)sqlite3_column_int64(stmt, 0);
		const HCHAR *link = (const HCHAR *)sqlite3_column_text(stmt, 1);
		HxLOG_Debug("\tuid: "HUID_FMT" link[%s]\n", uid, link);

		if (useCache)
		{
			dama_db_deleteLink(name, uid, link);
		}

		count++;
	}
	sqlite3_finalize(stmt);

	return count;
}


static HUINT32	dama_db_deleteLinkByQuery (DAMA_DB_t *db, const HCHAR *name, const HCHAR *query)
{
	sqlite3_stmt	*stmt;
	const HCHAR		*sql;
	HCHAR			*szStripQuery;
	HUINT32			count = 0;

	szStripQuery = HLIB_STD_StrLStrip((HCHAR *)query, NULL);

	sql = dama_db_makeSQL(db, "SELECT uid, link %s", szStripQuery + 6/*strlen("DELETE")*/);

	HxLOG_Debug(HxANSI_COLOR_YELLOW("%s")"\n", sql);

	if (sqlite3_prepare_v2(db->pMainDB, sql, HxSTD_StrLen(sql), &stmt, NULL) != SQLITE_OK)
	{
		DAMA_ERROR("[%s] prepare failed: %s", name, sql);
		return 0;
	}

	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		HUID	uid = (HUID)sqlite3_column_int64(stmt, 0);
		const HCHAR *link = (const HCHAR *)sqlite3_column_text(stmt, 1);
		HxLOG_Debug("\tuid: "HUID_FMT" link[%s]\n", uid, link);
		dama_db_deleteLink(name, uid, link);
		count++;
	}
	sqlite3_finalize(stmt);

	return count;
}


static HCHAR *	dama_db_isDeleteLinkQuery (DAMA_DB_t *db, const HCHAR *queryIn)
{
	HCHAR	*query;
	HCHAR	*save;
	HCHAR	*token;
	HCHAR	*result = NULL;
	HCHAR	*dupQuery = NULL;

	query = HLIB_STD_StrLStrip((HCHAR *)queryIn, NULL);
	if (*query != 'd' && *query != 'D')
	{
		goto END_FUNC;
	}
	if (!HLIB_STD_StrCaseStartWith(query, "DELETE"))
	{
		goto END_FUNC;
	}

	dupQuery = HLIB_STD_StrDup(query);
	if (NULL == dupQuery)
	{
		goto END_FUNC;
	}

	token = HxSTD_strtok_r(dupQuery, " \t\n", &save);
	token = HxSTD_strtok_r(NULL , " \t\n", &save);
	if (token == NULL || HLIB_STD_StrCaseCmp(token, "FROM") != 0)
	{
		goto END_FUNC;
	}

	token = HxSTD_strtok_r(NULL , " \t\n", &save);
	if (token == NULL || DLIB_GetDataPath(token) == NULL)
	{
		goto END_FUNC;
	}

	result = HLIB_STD_StrDup(token);

END_FUNC:
	if (NULL != dupQuery)			{ HLIB_STD_MemFree(dupQuery); }

	return result;
}

static void dama_db_cbFreeDlibField (void *data)
{
	if (data)
	{
		DLIB_DestroyGetFieldData((const DxField_t *)data);
	}
}

static	DERROR	dama_db_addData(DAMA_DB_t *db, const HCHAR *name, HUID uid, HINT32 size, const void *data, HBOOL useCache)
{
	HINT32	rst, i;
	HINT32	nFieldNum, nParamNo;
	HCHAR	*pszLinkInfo = NULL;
	HCHAR	*pszSQLFind = NULL;
	HCHAR	*pszSQL = NULL;
	HINT32	nEncodedSize;
	void	*pvEncodedData = NULL;
	HBOOL	bUpdateFlag = FALSE;
	DAMA_Table_t	*pstTable;
	DERROR			 eRet = DERR_FAIL;

	sqlite3_stmt	*pstStmt;

#if defined(CONFIG_TEMP_DLIB_NO_AUTOMATIC_TRASHING)
	HINT32			 nCreatedEncSize = 0;
	void			*pvCreatedEncData = NULL;
	HxList_t		*pstFieldList = NULL;
#endif

	HxLOG_Debug("name(%s) uid("HUID_FMT") size(%d)\n", name, uid, size);

	pstTable = dama_db_getTable(db, name);
	if (NULL == pstTable)
	{
		HxLOG_Error("dama_db_getTable NULL: name(%s)\n", name);
		goto END_FUNC;
	}

#if defined(CONFIG_DAMA_BACKGROUND_LOAD)
	if ((TRUE == pstTable->bLoadInOtherThread) && (TRUE != pstTable->bLoadDone))
	{
		HxLOG_Error("Not Initialized yet... Do not Set!!! name(%s)\n", name);
		eRet = DERR_OK;
		goto END_FUNC;
	}
#endif

#ifdef	CACHE_USE_HASHTABLE
	if (useCache)
	{
		if (pstTable->bUseInternalCache && pstTable->pstCacheData)
		{
			dama_cache_t	*pstCache = (dama_cache_t*)HLIB_HASH_Lookup(pstTable->pstCacheData, (void*)(HUINT32)uid);

			if (pstCache && pstCache->size == size)
			{
				if (HxSTD_MemCmp(pstCache->data, data, size) == 0)
				{
					HxLOG_Debug("Same data is coming..\n");

					eRet = DERR_OK;
					goto END_FUNC;
				}
				else
				{
					HxLOG_Debug("Same size But defferent Data\n");
				}
				bUpdateFlag = TRUE;
				HxSTD_MemCopy(pstCache->data, data, size);
			}
			else
			{
				if (pstCache)
					HLIB_HASH_Remove(pstTable->pstCacheData, (void*)(HUINT32)uid);
				pstCache = HLIB_STD_MemAlloc(size + sizeof(dama_cache_t));
				pstCache->size = size;
				HxSTD_MemCopy(pstCache->data, data, size);
				HLIB_HASH_Insert(pstTable->pstCacheData, (void*)(HUINT32)uid, (void*)pstCache);

				//	no data in cache. --> new data
				bUpdateFlag = FALSE;
			}
		}
	}
#endif

	//	Make Link/LinkData first
#if !defined(CONFIG_TEMP_DLIB_NO_AUTOMATIC_TRASHING)
	if ((pstTable && !(pstTable->bUseCodec))
		|| DLIB_Encode(name, uid, data, size, (const void **)&pvEncodedData, (HUINT32 *)&nEncodedSize) != DERR_OK)
	{
		HxLOG_Debug("Encoding Error...\n");
		nEncodedSize = size;
		pvEncodedData = (void*)data;
	}
#else
	if (!pstTable || pstTable->bUseCodec)
	{
		if (DLIB_CreateEncodedData(name, uid, data, size, (const void **)&pvCreatedEncData, (HUINT32 *)&nCreatedEncSize) == DERR_OK)
		{
			pvEncodedData = pvCreatedEncData;
			nEncodedSize = nCreatedEncSize;
		}
	}

	if (NULL == pvEncodedData)
	{
		nEncodedSize = size;
		pvEncodedData = (void*)data;
	}
#endif

	HxLOG_Debug("Encoded (%d)\n", nEncodedSize);

	pszLinkInfo = dama_db_getLink(db, name, uid, data, size);
	//	Finish making link/linkdata

	pszSQLFind   = dama_db_makeSQLForSelection(db, name, uid);
	if (NULL == pszSQLFind)
	{
		DAMA_ERROR("dama_db_makeSQLForSelection err:NULL");
		goto END_FUNC;
	}

#ifdef	CACHE_USE_HASHTABLE
	if (pstTable && pstTable->bUseInternalCache && pstTable->pstCacheData && useCache)
	{
		//	Cache Compare, bUpdateFlag is already set
	} else
#endif
	{
		rst = sqlite3_prepare_v2(db->pMainDB, pszSQLFind, HxSTD_StrLen(pszSQLFind), &pstStmt, NULL);
		if (rst != SQLITE_OK)
		{
			DAMA_ERROR("[%s] prepare failed: %s", name, pszSQLFind);
			goto END_FUNC;
		}

		rst = sqlite3_step(pstStmt);
		if (rst == SQLITE_ROW)
		{
			//	Check Link/LinkData
			if (pszLinkInfo == NULL)
			{
				HINT32	column_count, blob_size;
				void	*blob_data;

				//	Compare BLOB block with pvEncodedData
				column_count = sqlite3_column_count(pstStmt);
				blob_size = sqlite3_column_bytes(pstStmt, column_count - 1);
				blob_data = (void*)sqlite3_column_blob (pstStmt, column_count - 1);

				if (blob_size == nEncodedSize)
				{
					if (HxSTD_MemCmp(blob_data, pvEncodedData, nEncodedSize) == 0)
					{
						HxLOG_Debug("Same data is coming..\n");
						sqlite3_finalize(pstStmt);

						eRet = DERR_OK;
						goto END_FUNC;
					}
				}
				bUpdateFlag = TRUE;
			}
			else
			{
				//	TODO: Load Data Block from VFS and Comparing with Given Data.
				bUpdateFlag = TRUE;
			}
		}
		else
		{
			bUpdateFlag = FALSE;
		}
		sqlite3_finalize(pstStmt);
	}
	if (bUpdateFlag)
		pszSQL = dama_db_makeSQLForUpdate(db, name, uid);
	else
		pszSQL = dama_db_makeSQLForInsertion(db, name, uid);

	if (NULL == pszSQL)
	{
		DAMA_ERROR("dama_db_makeSQLForUpdate err:NULL");
		goto END_FUNC;
	}

	rst = sqlite3_prepare_v2(db->pMainDB, pszSQL, HxSTD_StrLen(pszSQL), &pstStmt, NULL);
	if (rst != SQLITE_OK)
	{
		DAMA_ERROR("[%s] prepare failed: %s", name, pszSQL);
		goto END_FUNC;
	}

#ifdef DAMA_MONITOR_DATA
	DAMA_MONITOR_DATA(db, name, data, size);
#endif
	nFieldNum = DLIB_GetFieldCount(name);

	nParamNo = 1;
	for (i = 0; i < nFieldNum; i++)
	{
		const DxField_t	*field;

#if !defined(CONFIG_TEMP_DLIB_NO_AUTOMATIC_TRASHING)
		field = DLIB_GetField(name, i, data, size);
#else
		field = DLIB_CreateGetFieldData(name, i, data, size);
#endif
		if (field == NULL)
		{
			nParamNo++;
			continue;
		}

#if defined(CONFIG_TEMP_DLIB_NO_AUTOMATIC_TRASHING)
 		pstFieldList = HLIB_LIST_Append(pstFieldList, (void *)field);
#endif
 		switch (field->type)
		{
		case eDxTYPE_STRING:
			if (field->u.string)
				sqlite3_bind_text(pstStmt, nParamNo, field->u.string, HxSTD_StrLen(field->u.string), SQLITE_STATIC);
			nParamNo++;
			break;

		case eDxTYPE_INTEGER:
			sqlite3_bind_int(pstStmt, nParamNo++, field->u.integer);
			break;

		case eDxTYPE_BINARY:
			if (field->u.binary.data && field->u.binary.size)
				sqlite3_bind_blob(pstStmt, nParamNo, field->u.binary.data, field->u.binary.size, SQLITE_STATIC);
			nParamNo++;
			break;

		default:
			break;
		}
	}

	if (pszLinkInfo == NULL)
	{
		HxLOG_Debug("Write BLOB block (%d)\n", nEncodedSize);
		sqlite3_bind_text(pstStmt, nParamNo++, NULL, 0, SQLITE_STATIC);
		sqlite3_bind_blob(pstStmt, nParamNo++, pvEncodedData, nEncodedSize, SQLITE_STATIC);
	}
	else
	{
		HxLOG_Debug("Write Link Data (%d)\n", pszLinkInfo);
		sqlite3_bind_text(pstStmt, nParamNo++, pszLinkInfo, HxSTD_StrLen(pszLinkInfo), SQLITE_STATIC);
		sqlite3_bind_blob(pstStmt, nParamNo++, NULL, 0, SQLITE_STATIC);
	}

	rst=sqlite3_step(pstStmt);
	if(rst != SQLITE_OK)
	{
		HxLOG_Debug("sqlite3_step return : (%d)\n",rst);
	}

	rst=sqlite3_finalize(pstStmt);
	if(rst != SQLITE_OK)
	{
		HxLOG_Debug("sqlite3_finalize return : (%d)\n",rst);
	}

	if (pszLinkInfo)
		dama_db_updateLink(db, name, uid, pszLinkInfo, nEncodedSize, pvEncodedData);

	eRet = DERR_DATACHANGED;

END_FUNC:
	if (NULL != pszSQLFind)				{ HLIB_STD_MemFree(pszSQLFind); }
	if (NULL != pszSQL)					{ HLIB_STD_MemFree(pszSQL); }
	if (NULL != pszLinkInfo)			{ HLIB_STD_MemFree(pszLinkInfo); }
#if defined(CONFIG_TEMP_DLIB_NO_AUTOMATIC_TRASHING)
	if (NULL != pvCreatedEncData)
	{
		DLIB_DestroyEncodedData((const void *)pvCreatedEncData);
	}

	if (NULL != pstFieldList)
	{
		pstFieldList = HLIB_LIST_RemoveAllFunc(pstFieldList, dama_db_cbFreeDlibField);
	}
#endif

	return eRet;
}

static DAMA_Result_t*	dama_db_queryData(DAMA_DB_t *db, const HCHAR *name, const HCHAR *query, HBOOL bPreload)
{
	HINT32	rst, size;
	HCHAR	*pszSQLRule = NULL;
	void	*pvEncodedData, *pvRealData;
	HINT32	nEncodedSize, nRealSize;
	int		i, n;
	void 	*garbage[4] = {NULL};
	int		gbglen = 0;
	struct column_index {
		int	uid;
		int	link;
		int	linkdata;
		int	tablename;
	} cindex = { -1, -1, -1, -1};
	const char *cname;
	const char *tablename;

	sqlite3_stmt	*pstStmt;
	DAMA_Result_t	*result = NULL;
	DAMA_Table_t	*table;
	HCHAR			*extName = NULL;

	if(HxSTD_StrEmpty(query) == TRUE)
	{
		HxLOG_Error("Query is null...\n");
		goto END_FUNC;
	}

#if defined(CONFIG_DAMA_BACKGROUND_LOAD)
	table = dama_db_getTable(db, name);
	if ((table) && (TRUE == table->bLoadInOtherThread) && (TRUE != table->bLoadDone))
	{
		HxLOG_Error("Not Initialized yet... Do not Query!!! name(%s)\n", name);
		goto END_FUNC;
	}

	table = NULL;
#endif

	if ((extName = dama_db_isDeleteLinkQuery(db, query)) != NULL)
	{
		HBOOL			 bDelLinkQuery = TRUE;

#if defined(CONFIG_DAMA_BACKGROUND_LOAD)
		table = dama_db_getTable(db, extName);
		if ((table) && (TRUE == table->bLoadInOtherThread) && (TRUE != table->bLoadDone))
		{
			HxLOG_Error("Load not finished!!! please don't query yet!!!: name(%s)\n", extName);
			bDelLinkQuery = FALSE;
		}
#endif

		if (bDelLinkQuery)
		{
			if (dama_db_deleteLinkByQuery(db, extName, query) > 0)
			{
				table = dama_db_getTable(db, extName);
				if (table)
				{
					s_bIsChangedData = TRUE;
					table->bUpdated = TRUE;
				}
			}
		}
	}

	pszSQLRule = HLIB_STD_StrDup(query);
	if (NULL == pszSQLRule)
	{
		goto END_FUNC;
	}

	rst = sqlite3_prepare_v2(db->pMainDB, pszSQLRule, HxSTD_StrLen(pszSQLRule), &pstStmt, NULL);
	if (rst != SQLITE_OK)
	{
		DAMA_ERROR("[UNKNOWN] prepare failed: %s, rst: %d, db->pMainDB: 0x%x", pszSQLRule, rst, db->pMainDB);
		goto END_FUNC;
	}

	n = sqlite3_column_count(pstStmt);
	for (i = 0 ; i < n ; i++)
	{
		cname = sqlite3_column_name(pstStmt, i);
		if (cname == NULL)
			continue;
		if (cindex.uid < 0 && strcmp(cname, "uid") == 0)
			cindex.uid = i;
		else if (cindex.link < 0 && strcmp(cname, "link") == 0)
			cindex.link = i;
		else if (cindex.linkdata < 0 && strcmp(cname, "linkdata") == 0)
			cindex.linkdata = i;
		else if (cindex.linkdata < 0 && strcmp(cname, "tablename") == 0)
			cindex.tablename = i;
	}

	if (!bPreload)
		result = dama_db_getTempResult(db);

	while (sqlite3_step(pstStmt) == SQLITE_ROW)
	{
		HUID	uid;

		if (cindex.uid < 0)
			uid = 0;
		else
			uid = (HUID)sqlite3_column_int64(pstStmt, cindex.uid);

		tablename = NULL;
		nEncodedSize  = 0;
		pvEncodedData = NULL;
#if defined(CONFIG_TEMP_DLIB_NO_AUTOMATIC_TRASHING)
		nRealSize = 0;
		pvRealData = NULL;
#endif

		if (cindex.link >= 0)
		{
			const HCHAR *link = (const HCHAR *)sqlite3_column_text(pstStmt, cindex.link);

			tablename = sqlite3_column_table_name(pstStmt, cindex.link);
			if (!HxSTD_StrEmpty(link))
			{
				if (tablename == NULL && cindex.tablename >= 0)
				{
					tablename = (const char*)sqlite3_column_text(pstStmt, cindex.tablename);
				}
				if (dama_db_readLink(db, tablename, uid, link, &nEncodedSize, &pvEncodedData) != DERR_OK)
				{
					DAMA_ERROR("[%s] Failed to open VFS: %s", tablename, link);
					continue;
				}
				garbage[gbglen++] = pvEncodedData;
			}
		}

		if (cindex.linkdata >= 0)
		{
			tablename = sqlite3_column_table_name(pstStmt, cindex.linkdata);
			size = sqlite3_column_bytes(pstStmt, cindex.linkdata);
			if (size > 0)
			{
				nEncodedSize  = size;
				pvEncodedData = (void *)sqlite3_column_blob(pstStmt, cindex.linkdata);
			}
		}

		if (tablename == NULL && cindex.tablename >= 0)
		{
			tablename = (const char*)sqlite3_column_text(pstStmt, cindex.tablename);
		}

		if (pvEncodedData && nEncodedSize && tablename)
		{
			DAMA_Item_t	*item;
			HUINT32 	tnameLen;
			dama_cache_t	*pstCache = NULL;

//			HxLOG_Debug("Encoded UID ["HUID_FMT"] Size [%d], linkdata:%d, link:%d\n", uid, nEncodedSize, cindex.linkdata, cindex.link);
			table = dama_db_getTable(db, tablename);

#ifdef	CACHE_USE_HASHTABLE
			if (table && table->bUseInternalCache && table->pstCacheData)
				pstCache = (dama_cache_t*)HLIB_HASH_Lookup(table->pstCacheData, (void*)(HUINT32)uid);
#endif

			if (pstCache)
			{
				pvEncodedData = pstCache->data;
				nEncodedSize = pstCache->size;
			}
			else
			{
				if (table && table->bUseCodec
#if !defined(CONFIG_TEMP_DLIB_NO_AUTOMATIC_TRASHING)
					&& DLIB_Decode(tablename, uid, pvEncodedData, nEncodedSize, (const void **)&pvRealData, (HUINT32*)&nRealSize) == DERR_OK)
#else
					&& DLIB_CreateDecodedData(tablename, uid, pvEncodedData, nEncodedSize, (const void **)&pvRealData, (HUINT32*)&nRealSize) == DERR_OK)
#endif
				{
					pvEncodedData = pvRealData;
					nEncodedSize  = nRealSize;
				}
				else
				{
					HxLOG_Debug("Decoding Error .. \n");
				}

#ifdef	CACHE_USE_HASHTABLE
				if (table && table->bUseInternalCache && table->pstCacheData)
				{
					pstCache = (dama_cache_t*)HLIB_STD_MemAlloc(nEncodedSize + sizeof(dama_cache_t));
					pstCache->size = nEncodedSize;
					HxSTD_memcpy(pstCache->data, pvEncodedData, nEncodedSize);
					HLIB_HASH_Insert(table->pstCacheData, (void*)(HUINT32)uid, (void*)pstCache);
				}
#endif
			}

			if (bPreload == FALSE)
			{
				tnameLen = HxSTD_StrLen(tablename) + 1; // include '\0'
				item = (DAMA_Item_t *)HLIB_STD_MemAlloc(sizeof(DAMA_Item_t) + sizeof(HUINT32) + tnameLen + nEncodedSize );
				if (item)
				{
					// DAMA_Item_t + (len + tablename + pvEncodedData)
					HUINT8 *ext_data;

					ext_data = (HUINT8*)(item + 1); //DAMA_Item_t ¸¸Å­ ÁÖ¼Ò Á¡ÇÁÈÄ 1byte ¾¿Á¢±Ù À§ÇØ HUINT8* ·Îº¯È¯
					item->uid  = uid;
					item->size = sizeof(HUINT32) + tnameLen + nEncodedSize;
					item->tablename = (HCHAR*)(ext_data + sizeof(HUINT32));
					HxSTD_memcpy(ext_data, &tnameLen, sizeof(HUINT32));
					HxSTD_memcpy(ext_data + sizeof(HUINT32), tablename, tnameLen);
					HxSTD_memcpy(ext_data + sizeof(HUINT32) + tnameLen, pvEncodedData, nEncodedSize);
					HLIB_VECTOR_Add(result->pList, item);
				}
			}

#if defined(CONFIG_TEMP_DLIB_NO_AUTOMATIC_TRASHING)
			if (NULL != pvRealData)
			{
				DLIB_DestroyDecodedData(tablename, (const void *)pvRealData);
				pvRealData = NULL;
			}
#endif
		}
		while (gbglen)
			DLIB_MemFree(garbage[--gbglen]);
	}
	sqlite3_finalize(pstStmt);

END_FUNC:
	if (NULL != pszSQLRule)				{ HLIB_STD_MemFree(pszSQLRule); }
	if (NULL != extName)				{ HLIB_STD_MemFree(extName); }
	return result;
}


static HUINT32	dama_db_countItem (DAMA_DB_t *db, const HCHAR *name)
{
	const HCHAR		*sql;
	const HCHAR		*err;
	sqlite3_stmt	*stmt;
	HUINT32			count = 0;

	sql = dama_db_makeSQL(db, "SELECT COUNT(*) FROM %s", name);
	if (sqlite3_prepare_v2(db->pMainDB, sql, HxSTD_StrLen(sql), &stmt, (const char**)&err) != SQLITE_OK)
	{
		DAMA_ERROR("[%s] prepare failed: %s", name, err);
		return 0;
	}

	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		count = sqlite3_column_int(stmt, 0);
		break;
	}
	sqlite3_finalize(stmt);

	return count;
}

static void		dama_db_setManualUpdateFlag(DAMA_DB_t *db, const HCHAR *objName, HBOOL flag)
{
	DAMA_Table_t *table;

	table = dama_db_getTable(db, objName);
	if (table)
	{
		table->bManualUpdate = flag;
		//DAMA_TRACE("[%s] Table will be updated cuz manual update requested!", objName);
	}
}

static HBOOL	dama_db_vacuumFile(DAMA_DB_t *db, DAMA_Table_t *table)
{
	HCHAR		*szFileName = NULL;
	sqlite3 	*fileDb;
	HINT32		ret;
	const HCHAR 	*sql;
	HERROR			 hErr;
	HBOOL			 bRetValue = FALSE;

	DAMA_TRACE("[%s] dama_db_vacuumFile", table->szObjName);

	if (table == NULL)
	{
		goto END_FUNC;
	}

	szFileName = (HCHAR *)HLIB_STD_MemAlloc(MAX_DB_PATH_LEN + MAX_DB_FILE_LEN);
	if (NULL == szFileName)
	{
		HxLOG_Error("memory allocation failed:\n");
		goto END_FUNC;
	}

	hErr = dama_db_getDbPath(db, table->szObjName, -1, szFileName);
	if (DERR_OK != hErr)
	{
		HxLOG_Error("dama_db_getDbPath err: objName[%s], dbIndex[%d]\n", table->szObjName, -1);
		goto END_FUNC;
	}

	sql = dama_db_makeSQL(db, "DETACH %s;", table->szObjName);
	if(dama_db_sqlite_exec(db->pMainDB, sql, NULL, 0) != SQLITE_OK)
	{
		goto END_FUNC;
	}

	if(sqlite3_open(szFileName, &fileDb) != SQLITE_OK)
	{
		goto END_FUNC;
	}

	if(dama_db_sqlite_exec(fileDb, "VACUUM;", 0, 0) != SQLITE_OK)
	{
		goto END_FUNC;
	}

	ret = sqlite3_close(fileDb);
	sql = dama_db_makeSQL(db, "ATTACH DATABASE '%s' AS %s; ", szFileName, table->szObjName);
	ret = dama_db_sqlite_exec(db->pMainDB, sql, NULL, 0);
	if (ret != SQLITE_OK)
	{
		HxLOG_Critical("[%s] Load file DB: Failed to attach file DB with sql \"%s\"", table->szObjName, sql);

		sql = dama_db_makeSQL(db, "DETACH %s; ATTACH DATABASE '%s' AS %s;", table->szObjName, szFileName, table->szObjName);
		if (dama_db_sqlite_exec(db->pMainDB, sql, NULL, 0) != SQLITE_OK)
		{
			HxLOG_Critical("[%s] Load file DB: Failed to re-attach file DB with sql \"%s\"", table->szObjName, sql);
			goto END_FUNC;
		}
	}

	bRetValue = TRUE;
END_FUNC:
	if (NULL != szFileName)					{ HLIB_STD_MemFree(szFileName); }
	return bRetValue;
}

static HBOOL	dama_db_syncTable(DAMA_DB_t *db, DAMA_Table_t *table)
{
	HCHAR		*szSrcPath = NULL;
	HCHAR		*szDstPath = NULL;
	HERROR		 hErr;
	HBOOL		 bRetValue = FALSE;
#ifdef CONFIG_DEBUG
	HUINT32		tick = HLIB_STD_GetSystemTick();
#endif

	HxLOG_Message("%s(%s)\n", __FUNCTION__, table->szObjName);

	szSrcPath = (HCHAR *)HLIB_STD_MemAlloc(MAX_DB_PATH_LEN + MAX_DB_FILE_LEN);
	szDstPath = (HCHAR *)HLIB_STD_MemAlloc(MAX_DB_PATH_LEN + MAX_DB_FILE_LEN);
	if ((NULL == szSrcPath) || (NULL == szDstPath))
	{
		HxLOG_Error("memory allocation failed:\n");
		goto END_FUNC;
	}

	hErr  = dama_db_getDbPath(db, table->szObjName, 0, szDstPath);		// /var/lib/humaxtv/dlogger
	hErr |= dama_db_getDbPath(db, table->szObjName, -1, szSrcPath);	// /tmp/
	if (DERR_OK != hErr)
	{
		HxLOG_Error("dama_db_getDbPath err: objName[%s]\n", table->szObjName);
		goto END_FUNC;
	}

#if defined(CONFIG_FUNC_USE_DAPI_DIRECT_QUERY)
	if(!HLIB_FILE_Exist(szSrcPath))
	{
		hErr = dama_db_deleteTable(db, table->szObjName, TRUE);
	}
	else
#endif
	{
		// ramfs -> flash
		dama_db_vacuumFile(db, table);
		HxLOG_Message("delete db %s\n", szDstPath);

		hErr = dama_db_copyFile(szSrcPath, szDstPath) ? ERR_OK : ERR_FAIL;
	}

#ifdef CONFIG_DEBUG
	if(HLIB_STD_GetSystemTick() - tick > 15000)
	{
		HxLOG_Warning("vaccum copy %s Tick[%d]\n", table->szObjName, HLIB_STD_GetSystemTick() - tick);
	}
#endif
	if (ERR_OK != hErr)
	{
		DAMA_ERROR("[%s] SYNC FAILED: Failed to copy(%s->%s)", table->szObjName, szSrcPath, szDstPath);
		goto END_FUNC;
	}

	DAMA_TRACE("[%s] SYNC SUCCESS src[%s] des[%s]\n", table->szObjName, szSrcPath, szDstPath);

	bRetValue = TRUE;
END_FUNC:
	if (NULL != szSrcPath)					{ HLIB_STD_MemFree(szSrcPath); }
	if (NULL != szDstPath)					{ HLIB_STD_MemFree(szDstPath); }

	return bRetValue;
}

static DERROR	dama_db_makeNewDb (DAMA_DB_t *db, const HCHAR *objName, const  const HCHAR *fileName, DAMA_Table_t	*table)
{
	sqlite3		*fileDb;
	int			ret;
	const HCHAR *sql;
	HCHAR	buf[HxPATH_MAX], *dir;

	if (!HLIB_DIR_IsMountedPath(fileName) || HLIB_FILE_Exist(fileName))
	{
		HxLOG_Warning("There is DB file or Not mounted[%s].\n", fileName);
		return DERR_FAIL;
	}

	dir = HLIB_PATH_DirName(fileName, buf, HxPATH_MAX);
	if(dir)
	{
		if(HLIB_FILE_Exist(dir) == FALSE)
		{
			HLIB_DIR_Make(dir);
		}
#if defined(CONFIG_DROP_ROOT_PRIVILEGES)
		if (HLIB_EXTRA_ChownWithParam(dir, UID_dama, UID_settop) != ERR_OK)
		{
			HxLOG_Error("Fail to chown path = %s \n", dir);
		}

		if (HLIB_EXTRA_Chmod(dir, 0770) != ERR_OK)
		{
			HxLOG_Error("	Fail to chmod path = %s \n", dir);
		}
#endif
	}

	ret = sqlite3_open(fileName, &fileDb);
	if (ret != SQLITE_OK)
	{
		DAMA_ERROR("[%s] Load file DB: Can't create file DB", objName);
		return DERR_FAIL;
	}
#ifdef	DAMA_CONFIG_PAGE_SIZE
	{
		const HCHAR *query;

		query = dama_db_makeSQL(db, "PRAGMA page_size = %d;", DAMA_CONFIG_PAGE_SIZE);

		HxLOG_Debug("[%s] dama_db_makeSQL : %s\n", __FUNCTION__, query);

		if (dama_db_sqlite_exec(fileDb, query, NULL, NULL) != SQLITE_OK)
		{
			DAMA_ERROR("PRAGMA page_size(%s) error\n", query);
			return DERR_FAIL;
		}
	}
#endif
	sql = dama_db_makeSQLForTableCreation(db, objName, TRUE, TRUE);

	ret = dama_db_sqlite_exec(fileDb, sql, NULL, 0);

	sqlite3_close(fileDb);
	if (ret != SQLITE_OK)
	{
		return DERR_FAIL;
	}
	return DERR_OK;
}
static HUINT32	dama_db_addNewColumn(DAMA_DB_t *db, const HCHAR *tableName)
{
	const HCHAR *sql;
	HINT32		ret = 0;
	HINT32		n = 0, i=0;
	n = DLIB_GetFieldCount(tableName);

	//	Get Keyword list from module so.
	for (i = 0; i < n; i++)
	{
		HCHAR			*errmsg = NULL;
		HCHAR			 bufferForField[DxLIB_MAX_NAME];

		sql = dama_db_makeSQL(db, "Alter Table %s ADD COLUMN [%s] %s; ", tableName, dama_db_getFieldName(db, tableName, i, bufferForField), dama_db_getFieldTypeString(tableName, i));
		ret |= sqlite3_exec(db->pMainDB, sql, NULL, 0, &errmsg);
		if(errmsg)
		{
			HxLOG_Print("sqlite3_exe error sql[%s] errmsg[%s]\n", sql, errmsg);
			sqlite3_free(errmsg);
		}
	}

	return ret;
}

static HINT32	dama_db_countKeys (DAMA_DB_t *db, const HCHAR *name, const HCHAR *key)
{
	const HCHAR		*sql;
	const HCHAR		*err;
	sqlite3_stmt	*stmt;
	HINT32			count = -1;

	sql = dama_db_makeSQL(db, "SELECT COUNT(%s) FROM %s", key, name);
	if (sqlite3_prepare_v2(db->pMainDB, sql, HxSTD_StrLen(sql), &stmt, (const char**)&err) != SQLITE_OK)
	{
		DAMA_ERROR("[%s] prepare failed: %s", name, err);
		return -1;
	}

	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		count = sqlite3_column_int(stmt, 0);
		break;
	}
	sqlite3_finalize(stmt);

	return count;
}

static DERROR	dama_db_loadFileDB (DAMA_DB_t *db, const HCHAR *objName, const  const HCHAR *fileName, DAMA_Table_t	*table)
{
	int			ret;
	const HCHAR *sql;
	HCHAR		*fdb = NULL;
#ifdef	CONFIG_DEBUG
	HUINT32		tick = HLIB_STD_GetSystemTick();
#endif
	DERROR		eRet = DERR_FAIL;

	if (!HLIB_FILE_Exist(fileName))
	{
		HxLOG_Warning("There is no DB file [%s].\n", fileName);
		goto END_FUNC;
	}

#ifdef	CONFIG_DEBUG
{
	const HCHAR *path = DLIB_GetDBPath(objName, 0);
	if (path && strcmp(path, ":memory:") != 0 && !HLIB_DIR_IsMountedPath(fileName))
	{
		HxLOG_Warning("%s(%s): path(%s) does NOT mounted!!\n", __FUNCTION__, objName, fileName);
		goto END_FUNC;
	}
}
#endif

	fdb = (HCHAR *)HLIB_STD_MemAlloc(DxLIB_MAX_NAME + 32);
	if (NULL == fdb)
	{
		HxLOG_Warning("memory allocation failed:\n");
		goto END_FUNC;
	}

	HxSTD_PrintToStrN(fdb, DxLIB_MAX_NAME + 31, "%s", objName);
	fdb[DxLIB_MAX_NAME + 31] = '\0';

	sql = dama_db_makeSQL(db, "ATTACH DATABASE '%s' AS %s; ", fileName, fdb);

	ret = dama_db_sqlite_exec(db->pMainDB, sql, NULL, 0);
	if (ret != SQLITE_OK)
	{
		HxLOG_Warning("[%s] Load file DB: Failed to attach file DB with sql \"%s\", error", objName, sql);

		sql = dama_db_makeSQL(db, "DETACH %s; ATTACH DATABASE '%s' AS %s;", fdb, fileName, fdb);
		if (dama_db_sqlite_exec(db->pMainDB, sql, NULL, 0) != SQLITE_OK)
		{
			HxLOG_Warning("[%s] Load file DB: Failed to re-attach file DB with sql \"%s\", error", objName, sql);
			goto END_FUNC;
		}
	}

	{
		HINT32			 nUidCnt, nLinkCnt, nLinkDataCnt;

		nUidCnt = dama_db_countKeys(db, objName, "uid");
		nLinkCnt = dama_db_countKeys(db, objName, "link");
		nLinkDataCnt = dama_db_countKeys(db, objName, "linkdata");

		if ((nUidCnt < 0) || (nLinkCnt < 0) || (nLinkDataCnt < 0) ||
			(nUidCnt != (nLinkCnt + nLinkDataCnt)))
		{
			HxLOG_Warning("\033[01;31m!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\033[22;39m\n");
			HxLOG_Warning("\033[01;31m!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\033[22;39m\n");
			HxLOG_Warning("\n");
			HxLOG_Warning("\033[01;31m   Warning !!!\033[22;39m\n");
			HxLOG_Warning("\033[01;31m     DB [%s] seems to be BROKEN !!!\033[22;39m\n", objName);
			HxLOG_Warning("\033[01;31m     DB [%s] seems to be BROKEN !!!\033[22;39m\n", objName);
			HxLOG_Warning("\033[01;31m     DB [%s] seems to be BROKEN !!!\033[22;39m\n", objName);
			HxLOG_Warning("\033[01;31m         - UID Cnt      (%d)\n\033[22;39m", nUidCnt);
			HxLOG_Warning("\033[01;31m         - LINK Cnt     (%d)\n\033[22;39m", nLinkCnt);
			HxLOG_Warning("\033[01;31m         - LINKDATA Cnt (%d)\n\033[22;39m", nLinkDataCnt);
			HxLOG_Warning("\n");
			HxLOG_Warning("\033[01;31m!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\033[22;39m\n");
			HxLOG_Warning("\033[01;31m!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\033[22;39m\n");
		}
	}

	dama_db_addNewColumn(db, objName);

#ifdef	CONFIG_DEBUG
	if(HLIB_STD_GetSystemTick() - tick > 3000) HxLOG_Warning("Insert Tick[%d]\n", HLIB_STD_GetSystemTick() - tick);
#endif
	DAMA_TRACE("[%s] Load file DB: Success", fileName);
	eRet = DERR_OK;

END_FUNC:
	if (NULL != fdb)						{ HLIB_STD_MemFree(fdb); }
	return eRet;
}

static	DERROR	dama_db_createTable(DAMA_DB_t *db, const HCHAR *objName, HBOOL bBootTime)
{
	HINT32			rst;
	HCHAR			*szFullName = NULL;
	HCHAR			*szPath = NULL;
	HCHAR			*szTempPath = NULL;
	DAMA_Table_t	*table = NULL;
	const HCHAR		*sql;
    const HUINT64   CHECK_FILE_SIZE  = 256;  //byte..
    DERROR			eErr, eResult = DERR_FAIL;

	table = (DAMA_Table_t*)HLIB_STD_MemCalloc(sizeof(DAMA_Table_t));
	if (table == NULL)
	{
		HxLOG_Critical("%s() Out of momory!\n", __FUNCTION__);
		goto END_FUNC;
	}

	szFullName = (HCHAR *)HLIB_STD_MemAlloc(MAX_DB_PATH_LEN + MAX_DB_FILE_LEN);
	szPath = (HCHAR *)HLIB_STD_MemAlloc(MAX_DB_PATH_LEN + MAX_DB_FILE_LEN);
	szTempPath = (HCHAR *)HLIB_STD_MemAlloc(MAX_DB_PATH_LEN + MAX_DB_FILE_LEN);
	if ((NULL == szFullName) || (NULL == szPath) || (NULL == szTempPath))
	{
		HxLOG_Error("memory allocation failed:\n");
		goto END_FUNC;
	}

	HxSTD_StrNCpy(table->szObjName, objName, DxLIB_MAX_NAME-1);
	table->ulSyncCycle = (DLIB_GetSyncTime(objName)*1000);
	table->ulLastSyncTime  = HLIB_STD_GetSystemTick();

	eErr = dama_db_getDbPath(db, objName, 0, szFullName);
	if (DERR_OK != eErr)
	{
		HxLOG_Error("dama_db_getDbPath err: objName[%s] dbIndex[%d]\n", objName, 0);
		goto END_FUNC;
	}

	//	Make table in memory DB with given parameters
	if (HLIB_STD_StrCaseCmp(szFullName, ":memory:") != 0)
	{
		HINT32	backupIndex;
		HUINT64      size = 0;
#if defined(CONFIG_DAMA_BACKGROUND_LOAD)
		HBOOL			 bLoadInOtherThread;
#endif

		eErr = dama_db_getDbPath(db, objName, -1, szTempPath);
		if (DERR_OK != eErr)
		{
			HxLOG_Error("dama_db_getDbPath err: objName[%s] dbIndex[%d]\n", objName, -1);
			goto END_FUNC;
		}

#if defined(CONFIG_DAMA_BACKGROUND_LOAD)
		bLoadInOtherThread = DLIB_HasTag(objName, "SLOWINIT") ? TRUE : FALSE;
		if ((TRUE == bLoadInOtherThread) && (TRUE == bBootTime))
		{
			HxLOG_Print("Table [%s] init in the other thread later...\n", objName);
			table->bLoadInOtherThread = TRUE;
		}
		else
#endif
		{
			HxSTD_StrNCpy(szPath, szFullName,MAX_DB_PATH_LEN + MAX_DB_FILE_LEN - 1);

			for ( eErr = DERR_OK, backupIndex = 1
				; DERR_OK == eErr
				; eErr = dama_db_getDbPath(db, objName, backupIndex++, szPath))
			{
                if (HLIB_FILE_SizeFromPath(szPath,&size ) == ERR_OK && size > CHECK_FILE_SIZE)
                {
                    dama_db_copyFile(szPath, szTempPath);
        			if (dama_db_loadFileDB(db, objName, szTempPath, table) == DERR_OK)
        			{
        				dama_db_copyFile(szTempPath, szFullName);
        				break;
        			}
    			}
			}

			if (!HLIB_FILE_Exist(szTempPath) && HLIB_DIR_IsMountedPath(szFullName))
			// All File Deleted. DB is loaded. but tmpPath is not exist. have to make new Default DB.
			{
				HxLOG_Warning("All DB Deleted[%s].\n", szTempPath);
				dama_db_makeNewDb (db, objName, szTempPath, table);
				if(dama_db_loadFileDB(db, objName, szTempPath, table) != DERR_OK)
				{
					HxLOG_Warning("Make New DB But Loading Failed[%s].\n", szTempPath);
				}
				else
				{
					dama_db_copyFile(szTempPath, szFullName);
					HxLOG_Print("Make New DB. Loading Succeed[%s].\n", szTempPath);
				}
			}

			if (DERR_OK != eErr)
			{
				HxLOG_Warning("%s() Failed to load file DB(%s)\n", __FUNCTION__, objName);
				if (dama_db_getReloadPath(db, objName, table->szReloadPath, MAX_DB_PATH_LEN))
				{
					db->nReloadable++;
					DAMA_TRACE("[%s] Created but file DB(%s) will be loaded soon ", objName, table->szReloadPath);
				}
				else
				{
					DAMA_ERROR("[%s] Failed to load file DB", objName);
				}
			}
			else
			{
				DAMA_TRACE("[%s] created with FILE:%s", objName, szPath);
			}
		}
	}
	else
	{
		if ((sql = dama_db_makeSQLForTableCreation(db, objName, TRUE, TRUE)) == NULL)
		{
			HxLOG_Critical("SQL Creation FAIL!!! Abort!\n");
			HxLOG_Assert(0);
		}

		HxLOG_Debug("SQL [%s]\n", sql);
		rst = dama_db_sqlite_exec(db->pMainDB, sql, NULL, 0);
		if (rst != SQLITE_OK)
		{
			HxLOG_Critical("Fail to create database\n");
			goto END_FUNC;
		}

		DAMA_TRACE("[%s] created on memory", objName);
	}

	table->bUseCodec = DLIB_HasTag(objName, "BLOB") ? FALSE : TRUE;
	table->bUseInternalCache = DLIB_HasTag(objName, "CACHE") ? TRUE : FALSE;
#if defined(CONFIG_DAMA_BACKGROUND_SYNC)
	table->bSyncInOtherThread = DLIB_HasTag(objName, "ASYNC") ? TRUE : FALSE;
#endif


	HLIB_HASH_Insert(db->pTableMap, HLIB_STD_StrDup(objName), (void*)table);

	if (table->bUseInternalCache)
	{
		//	TODO: 64BitÁö¿øÀ» ¾îÄÉ ÇÏÁö???
		table->pstCacheData = HLIB_HASH_NewFull(HLIB_HASH_GetIntKey, HLIB_HASH_IsIntEqual, NULL, HLIB_STD_MemFree_CB);

		//	TODO: add Preload when internal cache is enabled.
		//dama_db_queryData(db, objName, "SELECT * from DxService_t", TRUE);
	}

	table = NULL;
	eResult = DERR_OK;

END_FUNC:
	if (NULL != table)					{ HLIB_STD_MemFree(table); }
	if (NULL != szFullName)				{ HLIB_STD_MemFree(szFullName); }
	if (NULL != szPath)					{ HLIB_STD_MemFree(szPath); }
	if (NULL != szTempPath)				{ HLIB_STD_MemFree(szTempPath); }

	return eResult;
}

static HBOOL	dama_db_deleteFiles(const HCHAR *objName, const HCHAR *tags[], void *userdata)
{
	DAMA_DB_t	*db;
	HBOOL		 useCache;

	HINT32		index;
	DAMA_Table_t 	*table;
	damaDbDelUserData_t	*delArgs;
	const HCHAR 	*sql;

	HxLOG_Warning("[%s] dama_db_deleteFiles", objName);

	delArgs = (damaDbDelUserData_t *)userdata;
	db = delArgs->db;
	useCache = delArgs->useCache;

	table = dama_db_getTable(db, objName);
#if defined(CONFIG_DAMA_BACKGROUND_LOAD)
	if ((table) && (TRUE == table->bLoadInOtherThread) && (TRUE != table->bLoadDone))
	{
		HxLOG_Error("Not Initialized yet... Do not Reset!!! name(%s)\n", objName);
		return DERR_OK;
	}
#endif

	if (useCache)
	{
		dama_db_deleteLink(objName, DAMA_TABLE_RESET_UID, NULL);
	}

	if (table)
	{
		if (table->bUseInternalCache && table->pstCacheData && useCache)
			HLIB_HASH_RemoveAll(table->pstCacheData);

		sql = dama_db_makeSQL(db, "DETACH %s;", objName);
		if(dama_db_sqlite_exec(db->pMainDB, sql, NULL, 0) != SQLITE_OK)
		{
			HxLOG_Error("DETACH FAIL [%s]\n", objName);
		}
		table->bManualUpdate = TRUE;
		table->bUpdated = TRUE;
	}

	if (useCache)
	{
		HCHAR				*szPath;

		szPath = (HCHAR *)HLIB_STD_MemAlloc(MAX_DB_PATH_LEN + MAX_DB_FILE_LEN);
		if (NULL != szPath)
		{
			for (index = -1 ; dama_db_getDbPath(db, objName, index, szPath) == DERR_OK; index++)
			{
				if (HLIB_STD_StrCaseCmp(szPath, ":memory:") == 0)
					break;

				HxLOG_Warning("[%s] file(%s) removed\n", objName, szPath);
				HxLOG_Warning("delete db %s\n", szPath);
				HLIB_FILE_Delete(szPath);
			}

			HLIB_HASH_Remove(db->pTableMap , objName);
			dama_db_createTable(db, objName, FALSE);

			HLIB_STD_MemFree(szPath);
		}
	}
	else
	{
		const HCHAR 	*sql;
		HINT32			rst;

		if ((sql = dama_db_makeSQLForTableCreation(db, objName, TRUE, TRUE)) == NULL)
		{
			HxLOG_Critical("SQL Creation FAIL!!! Abort!\n");
			HxLOG_Assert(0);
		}

		HxLOG_Debug("SQL [%s]\n", sql);
		rst = dama_db_sqlite_exec(db->pMainDB, sql, NULL, 0);
		if (rst != SQLITE_OK)
		{
			HxLOG_Critical("Fail to create database\n");
			HLIB_STD_MemFree(table);
			return DERR_FAIL;
		}
		DAMA_TRACE("[%s] created on memory", objName);
	}

	return FALSE;
}

static	DERROR	dama_db_deleteTable(DAMA_DB_t *db, const HCHAR *name, HBOOL useCache)
{
	damaDbDelUserData_t			 userData;

	userData.db = db;
	userData.useCache = useCache;

	if (name == NULL)
	{
		HxLOG_Warning("dama_db_deleteTable ALL\n");
		dama_db_forEachObject(dama_db_deleteFiles, (void *)&userData);
	}
	else
	{
		HxLOG_Print("dama_db_deleteTable [%s]\n", name);
		dama_db_deleteFiles(name, NULL, (void*)&userData);
	}
	return DERR_OK;
}

static HBOOL	dama_db_dump (DAMA_DB_t *db, const HCHAR *name)
{
	HBOOL			success = FALSE;
	int				ret;
	sqlite3			*dest;
	sqlite3_backup	*backup;

	if (HxSTD_StrEmpty(name))
		name = "dama.db";

	ret = sqlite3_open(name, &dest);
	if (ret != SQLITE_OK)
	{
		HxLOG_Error("%s(%s) Cannot open dump db file\n", __FUNCTION__, name);
		return FALSE;
	}

	backup = sqlite3_backup_init(dest, "main", db->pMainDB, "main");
	if (backup)
	{
		(void)sqlite3_backup_step(backup, -1);
		(void)sqlite3_backup_finish(backup);
		HxLOG_Info("%s(%s) Dump done!\n", __FUNCTION__, name);
		success = TRUE;
	}
	sqlite3_close(dest);
	return success;
}

static HBOOL	dama_db_createEachTable (const HCHAR *name, const HCHAR *tags[], void *userdata)
{
	DAMA_DB_t *db = (DAMA_DB_t *)userdata;

	if (!HxSTD_StrEmpty(name))
	{
		(void)dama_db_createTable(db, name, TRUE);
	}
	return FALSE;
}

static	DERROR	dama_db_openDatabase(DAMA_DB_t *db, HBOOL bOpenDbFiles)
{
	HINT32	rc;
	if (db->pMainDB)
	{
		HxLOG_Critical("DAMA-DB is already opened!!\n");
		return DERR_FAIL;
	}
#if 0//defined(CONFIG_FUNC_USE_DAPI_DIRECT_QUERY)
	rc = sqlite3_open(DAMA_MAIN_DB, &db->pMainDB);
#else
	rc = sqlite3_open(":memory:", &db->pMainDB);
#endif
	if (rc != SQLITE_OK)
	{
		HxLOG_Critical("DAMA_DB : Fail to create database named [%s]\n", ":memory:");
		return DERR_FAIL;
	}
#ifdef	DAMA_CONFIG_VACUUM
	// INCREMENTAL VACUUM
	if(dama_db_sqlite_exec(db->pMainDB, "PRAGMA auto_vacuum = 2;", NULL, NULL) != SQLITE_OK) return DERR_FAIL;
#endif

	if(dama_db_sqlite_exec(db->pMainDB, "PRAGMA cache_size = 6000;", NULL, NULL) != SQLITE_OK) return DERR_FAIL;
	//if(dama_db_sqlite_exec(db->pMainDB, "PRAGMA synchronous=OFF", NULL, NULL) != SQLITE_OK) return DERR_FAIL;
	if(dama_db_sqlite_exec(db->pMainDB, "PRAGMA count_changes=OFF", NULL, NULL) != SQLITE_OK) return DERR_FAIL;
	//if(dama_db_sqlite_exec(db->pMainDB, "PRAGMA journal_mode=OFF", NULL, NULL) != SQLITE_OK) return DERR_FAIL;

	//sqlite3_limit(db->pMainDB,SQLITE_LIMIT_ATTACHED,20);

	if (TRUE == bOpenDbFiles)
	{
		dama_db_forEachObject(dama_db_createEachTable, db);
	}

	return DERR_OK;
}

static	DERROR	dama_db_closeDatabase(DAMA_DB_t *db)
{
	if (db->pMainDB)
		sqlite3_close(db->pMainDB);
	db->pMainDB = NULL;

	return DERR_OK;
}

#if defined(CONFIG_DAMA_BACKGROUND_LOAD)
// dama_db_createTable ¸¦ ±âÃÊ·Î ¸¸µç LoadÇÔ¼ö
static HBOOL dama_db_loadLeftTables (const HCHAR *objName, const HCHAR *tags[], void *userdata)
{
	HCHAR			*szFullName = NULL;
	HCHAR			*szPath = NULL;
	HCHAR			*szTempPath = NULL;
	DAMA_DB_t		*db;
	DAMA_Table_t	*table;

	db = (DAMA_DB_t *)userdata;

	table = dama_db_getTable(db, objName);
	if ((table) && (TRUE == table->bLoadInOtherThread) && (TRUE != table->bLoadDone))
	{
		HINT32			 backupIndex;
		DERROR			 eErr;

		szFullName = (HCHAR *)HLIB_STD_MemAlloc(MAX_DB_PATH_LEN + MAX_DB_FILE_LEN);
		szPath = (HCHAR *)HLIB_STD_MemAlloc(MAX_DB_PATH_LEN + MAX_DB_FILE_LEN);
		szTempPath = (HCHAR *)HLIB_STD_MemAlloc(MAX_DB_PATH_LEN + MAX_DB_FILE_LEN);
		if ((NULL == szFullName) || (NULL == szPath) || (NULL == szTempPath))
		{
			HxLOG_Error("memory allocation failed:\n");
			goto END_FUNC;
		}

		eErr  = dama_db_getDbPath(db, objName, 0, szFullName);
		eErr |= dama_db_getDbPath(db, objName, -1, szTempPath);
		if (DERR_OK != eErr)
		{
			HxLOG_Error("dama_db_getDbPath err: objName[%s]\n", objName);
			goto END_FUNC;
		}

		//	Make table in memory DB with given parameters
		HxSTD_StrNCpy(szPath, szFullName, MAX_DB_PATH_LEN + MAX_DB_FILE_LEN - 1);
		szPath[MAX_DB_PATH_LEN + MAX_DB_FILE_LEN - 1] = '\0';

		for (backupIndex = 1, eErr = DERR_OK
			; DERR_OK == eErr
			; eErr = dama_db_getDbPath(db, objName, backupIndex++, szPath))
		{
			if (HLIB_FILE_Exist(szPath))
			{
				dama_db_copyFile(szPath, szTempPath);
			}

			if (dama_db_loadFileDB(db, objName, szTempPath, table) == DERR_OK)
			{
				dama_db_copyFile(szTempPath, szFullName);
				break;
			}
		}

		if (!HLIB_FILE_Exist(szTempPath) && HLIB_DIR_IsMountedPath(szFullName))
		// All File Deleted. DB is loaded. but tmpPath is not exist. have to make new Default DB.
		{
			HxLOG_Warning("All DB Deleted[%s].\n", szTempPath);
			dama_db_makeNewDb (db, objName, szTempPath, table);
			if(dama_db_loadFileDB(db, objName, szTempPath, table) != DERR_OK)
			{
				HxLOG_Warning("Make New DB But Loading Failed[%s].\n", szTempPath);
			}
			else
			{
				dama_db_copyFile(szTempPath, szFullName);
				HxLOG_Print("Make New DB. Loading Succeed[%s].\n", szTempPath);
			}
		}

		if (DERR_OK != eErr)
		{
			HxLOG_Warning("%s() Failed to load file DB(%s)\n", __FUNCTION__, objName);
			if (dama_db_getReloadPath(db, objName, table->szReloadPath, MAX_DB_PATH_LEN))
			{
				db->nReloadable++;
				DAMA_TRACE("[%s] Created but file DB(%s) will be loaded soon ", objName, table->szReloadPath);
			}
			else
			{
				DAMA_ERROR("[%s] Failed to load file DB", objName);
			}
		}
		else
		{
			DAMA_TRACE("[%s] created with FILE:%s", objName, szPath);
		}

		table->bLoadDone = TRUE;
	}

END_FUNC:
	if (NULL != szFullName)					{ HLIB_STD_MemFree(szFullName); }
	if (NULL != szPath)						{ HLIB_STD_MemFree(szPath); }
	if (NULL != szTempPath)					{ HLIB_STD_MemFree(szTempPath); }

	return FALSE;
}

static HBOOL dama_db_checkQueryUsingUnloadedDb (const HCHAR *objName, const HCHAR *tags[], void *userdata)
{
	damaDbCheckQuery_t	*pstCheckQuery;
	DAMA_Table_t 		*table;

	pstCheckQuery = (damaDbCheckQuery_t *)userdata;

	table = dama_db_getTable(pstCheckQuery->db, objName);
	if ((table) && (TRUE == table->bLoadInOtherThread) && (TRUE != table->bLoadDone))
	{
		HCHAR			 szWordToFind[32];

		HxSTD_PrintToStrN(szWordToFind, 32, "FROM %s", objName);

		if (HLIB_STD_StrCaseStr(pstCheckQuery->szQuery, szWordToFind) != NULL)
		{
			pstCheckQuery->bFound = TRUE;
			return TRUE;
		}
	}

	return FALSE;
}
#endif

DAMA_DB_t *		DAMA_DB_Open (const HCHAR *path, const HCHAR *backupPath, const HCHAR *tmpPath, HBOOL bCreateSqlite, HBOOL bOpenDbFiles)
{
	DAMA_DB_t *db;

	HxLOG_Trace();

	db = (DAMA_DB_t *)HLIB_STD_MemCalloc(sizeof(DAMA_DB_t));
	if (db)
	{
		HxSTD_StrNCpy(db->szPath, path, MAX_DB_PATH_LEN - 1);
		HxSTD_StrNCpy(db->szBackupPath, backupPath, MAX_DB_PATH_LEN - 1);
		HxSTD_StrNCpy(db->szTempPath, tmpPath, MAX_DB_PATH_LEN - 1);
		if (access(db->szPath, R_OK | W_OK) != 0)
		{
			HLIB_DIR_Make(db->szPath);
		}
		if (access(db->szBackupPath, R_OK | W_OK) != 0)
		{
			HLIB_DIR_Make(db->szBackupPath);
		}
		db->nSqlBuf   = HLIB_RWSTREAM_Open(NULL, 8192);
		db->pTableMap = HLIB_HASH_New((_HxHashFunc)HLIB_HASH_GetStringKey, (_HxEqualFunc)HLIB_HASH_GetStringKey);

#ifdef	DAMA_MONITOR_DATA
		DAMA_DEBUG(
			HCHAR 	buf[128];
			HINT32	n = 0;
			HxDIR_t	*dir = HLIB_DIR_Open("/");
			if (dir) {
				do
				{
					HUINT8 *name = HLIB_DIR_GetName(dir);
					if (HLIB_STD_StrCaseStartWith(name, "dama.monitor"))
						n++;
				} while (HLIB_DIR_Next(dir) == ERR_OK);
				HLIB_DIR_Close(dir);
			}
			HxSTD_PrintToStrN(buf, 128, "dama.monitor.%02d.log", n);
			db->nDataMonitorSeq = n;
			db->fpDataMonitor = fopen(buf, "w");
		);
#endif
		DAMA_DEBUG(
			db->pTableMonitor = HLIB_TREE_NewFull((HxCompareDataFunc)HLIB_STD_StrCaseCmp, NULL, NULL, HLIB_STD_MemFree_CB);
		);

		if (TRUE == bCreateSqlite)
		{
			dama_db_openDatabase(db, bOpenDbFiles);
		}
	}
	return db;
}


void			DAMA_DB_Close (DAMA_DB_t *db)
{
	HxLOG_Trace();
	HxLOG_Assert(db);

	dama_db_closeDatabase(db);

	HLIB_RWSTREAM_Close(db->nSqlBuf);
	HLIB_HASH_Destroy(db->pTableMap);

#ifdef	DAMA_MONITOR_DATA
	HLIB_TREE_Destroy(db->pTableMonitor);
	fclose(db->fpDataMonitor);
#endif
	HLIB_STD_MemFree(db);
}

HBOOL	DAMA_DB_LoadCache(DAMA_DB_t *db)
{
	HCHAR				*key;
	HxHASH_TableIter_t	iter;
	DAMA_Table_t		*tableInfo;

	HLIB_HASH_IterInit(&iter, db->pTableMap);
	while (HLIB_HASH_IterNext(&iter, (void**)&key, (void **)&tableInfo))
	{
		if(tableInfo->bUseInternalCache)
		{
			HCHAR pCacheQuery[256]={0,};
			HxSTD_snprintf(pCacheQuery, 255, "SELECT * FROM %s", tableInfo->szObjName);
			dama_db_queryData(db, tableInfo->szObjName, pCacheQuery, TRUE);
		}
	}
	HxLOG_Message("DAMA_DB_LoadCache Cache data Loaded\n");
	return TRUE;
}




HBOOL			DAMA_DB_HasChange (DAMA_DB_t *db)
{
	HxLOG_Assert(db);

	return dama_db_hasChanged(db);
}

HBOOL DAMA_DB_SetChangedDataFlag(HBOOL bIsChangedData)
{
	return dama_db_SetIsChangedData(bIsChangedData);
}



HBOOL DAMA_DB_GetChangedDataDataFlag(void)
{
	return dama_db_GetIsChangedData();

}


DERROR			DAMA_DB_Sync (DAMA_DB_t *db)
{
	HxLOG_Trace();
	HxLOG_Assert(db);

	dama_db_syncDatabase(db);
	return DERR_OK;
}

DERROR			DAMA_DB_ForcedSync(DAMA_DB_t *db, const HCHAR *name)
{
	HxLOG_Trace();

	if (name == NULL)
	{
		HCHAR				*key;
		HxHASH_TableIter_t	iter;
		DAMA_Table_t		*tableInfo;

		HLIB_HASH_IterInit(&iter, db->pTableMap);
		while (HLIB_HASH_IterNext(&iter, (void**)&key, (void **)&tableInfo))
		{
			tableInfo->bManualUpdate = TRUE;
		}
		DAMA_TRACE("All table will be synced");
	} else
	{
		dama_db_setManualUpdateFlag(db, name, TRUE);
	}
	dama_db_syncDatabase(db);

#if !defined(CONFIG_DAMA_BACKGROUND_SYNC)
	HLIB_STD_TaskSleep(1);
#endif

	return DERR_OK;
}

DERROR			DAMA_DB_Restore (DAMA_DB_t *db, const HCHAR *name)
{
	HCHAR			*szPath = NULL;
	DERROR			 eErr, eResult = DERR_FAIL;

	HxLOG_Trace();
	HxLOG_Assert(name);

	szPath = (HCHAR *)HLIB_STD_MemAlloc(MAX_DB_PATH_LEN + MAX_DB_FILE_LEN);
	if (NULL == szPath)
	{
		HxLOG_Error("memory allocation failed:\n");
		goto END_FUNC;
	}

	eErr = dama_db_getDbPath(db, name, 0, szPath);
	if (DERR_OK != eErr)
	{
		HxLOG_Error("dama_db_getDbPath err: objName[%s] dbIndex[%d]\n", name, 0);
		goto END_FUNC;
	}

	if (HLIB_STD_StrCaseCmp(szPath, ":memory:") != 0)
	{
#if defined(CONFIG_DAMA_BACKGROUND_LOAD)
		DAMA_Table_t	*table = dama_db_getTable(db, name);

		if ((table) && (TRUE == table->bLoadInOtherThread) && (TRUE != table->bLoadDone))
		{
			HxLOG_Error("Not Initialized yet... Do not Restore!!! name(%s)\n", name);
			eResult = DERR_OK;
			goto END_FUNC;
		}
#endif

		dama_db_deleteTable(db, name, TRUE);
		dama_db_createTable(db, name, FALSE);
	}

	eResult = DERR_OK;

END_FUNC:
	if (NULL != szPath)					{ HLIB_STD_MemFree(szPath); }
	return eResult;
}

DERROR			DAMA_DB_CreateTable (DAMA_DB_t *db, const HCHAR *name)
{
	DAMA_Table_t	*table;
	DERROR			err;

	//HxLOG_Trace();
	err = DERR_FAIL;
	if (!HxSTD_StrEmpty(name))
	{
		table = dama_db_getTable(db, name);
		if (table == NULL)
		{
			return dama_db_createTable(db, name, FALSE);
		}
	}
	return err;
}

DERROR			DAMA_DB_ReloadTable (DAMA_DB_t *db, HCHAR *szObjName)
{
	const HCHAR 	*objName;
	DAMA_Table_t	*table;
	HxHASH_TableIter_t	iter;
	HCHAR				*szTmpPath = NULL;
	DERROR				 eErr, eResult = DERR_FAIL;

	if (db->nReloadable == 0)
	{
		goto END_FUNC;
	}

	szTmpPath = (HCHAR *)HLIB_STD_MemAlloc(MAX_DB_PATH_LEN + MAX_DB_FILE_LEN);
	if (NULL == szTmpPath)
	{
		HxLOG_Error("memory allocation failed:\n");
		goto END_FUNC;
	}

	HLIB_HASH_IterInit(&iter, db->pTableMap);
	while (HLIB_HASH_IterNext(&iter, (void **)&objName, (void **)&table))
	{
		if (!HxSTD_StrEmpty(table->szReloadPath) && HLIB_DIR_IsMountedPath(table->szReloadPath))
		{
			eErr = dama_db_getDbPath(db, objName, -1, szTmpPath);
			if (DERR_OK == eErr)
			{
				dama_db_makeNewDb (db, objName, table->szReloadPath, table);
				HxLOG_Warning("delete db %s\n", szTmpPath);
				dama_db_copyFile(table->szReloadPath, szTmpPath);
				eErr = dama_db_loadFileDB(db, objName, szTmpPath, table);
				HxLOG_Info("%s(%s) reload(%s) %s\n", __FUNCTION__, objName, table->szReloadPath, eErr ? "FAIL" : "SUCCESS");

				DAMA_TRACE("[%s] Table reloaded", objName);
				table->szReloadPath[0] = '\0';

				if (NULL != szObjName)
				{
					HLIB_STD_StrNCpySafe(szObjName, objName, DxLIB_MAX_NAME);
				}

				db->nReloadable--;

				eResult = DERR_OK;
				goto END_FUNC;
			}


		}
	}

END_FUNC:
	if (NULL != szTmpPath)					{ HLIB_STD_MemFree(szTmpPath); }

	return eResult;
}

HUINT32			DAMA_DB_Vacuum (DAMA_DB_t *db)
{
#ifdef	DAMA_CONFIG_VACUUM
	const HCHAR	*query;
	HUINT32	used;

	HxLOG_Trace();

	query = dama_db_makeSQL(db, "PRAGMA incremental_vacuum(%d);", DAMA_CONFIG_VACUUM);
	if (dama_db_sqlite_exec(db->pMainDB, query, NULL, NULL) != SQLITE_OK)
	{
		DAMA_ERROR("VACUUM(%s) error\n", query);
	}
	used  = (HUINT32)sqlite3_memory_used();
	DAMA_TRACE(HxANSI_COLOR_YELLOW("VACUUM done: memory(%u bytes)\n"), used);
	return used;
#else
	(void)db;
	return 0;
#endif
}

HUINT32			DAMA_DB_Count (DAMA_DB_t *db, const HCHAR *name)
{
	DAMA_Table_t	*table;

	HxLOG_Trace();

	table = dama_db_getTable(db, name);
	if (table == NULL)
		return 0;

	return dama_db_countItem(db, name);
}

void			DAMA_DB_CheckThreshold (DAMA_DB_t *db
					, void (* callback)(const HCHAR *name, HUINT32 count, void *userdata), void *userdata)
{
	HxHASH_TableIter_t	iter;
	DAMA_Table_t		*table;
	HUINT32				threshold_count;
	HUINT64				threshold_size;
	HUINT32				count;
	HUINT64				size = 0;
	HCHAR				filepath[128] = {0,};
	HxFILE_t			file;

	HxLOG_Trace();

	HLIB_HASH_IterInit(&iter, db->pTableMap);
	while (HLIB_HASH_IterNext(&iter, NULL, (void **)&table))
	{
		threshold_count = DLIB_GetThreshold(table->szObjName);
		if (threshold_count == 0)
		{
			continue;
		}

		threshold_size = DLIB_GetThresholdSize(table->szObjName);

		count = dama_db_countItem(db, table->szObjName);

		if(threshold_size > 0)
		{
			HxSTD_StrNCpy(filepath, "/tmp/", 5);
			HxSTD_StrNCat(filepath, table->szObjName, HxSTD_StrLen(table->szObjName));
			HxSTD_StrNCat(filepath, ".db", 3);
			file = HLIB_FILE_Open(filepath,"r");
			if(file != NULL)
			{
				HLIB_FILE_Size(file, &size);
				HLIB_FILE_Close(file);
			}

			HxSTD_MemSet(filepath, 0, 128);
		}

		if (count >= threshold_count)
		{
			callback(table->szObjName, count, userdata);
	}

		if ((threshold_size > 0) && (size >= threshold_size))
		{
			callback(table->szObjName, 0, userdata);
		}
	}
}

#if defined(CONFIG_FUNC_USE_DAPI_DIRECT_QUERY)
DERROR			DAMA_DB_Datachanged (DAMA_DB_t *db, const HCHAR *name)
{
	DAMA_Table_t	*pstTableInfo;

	HxLOG_Trace();
	HxLOG_Assert(name);

	pstTableInfo = dama_db_getTable(db, name);
	if (pstTableInfo)
	{
		s_bIsChangedData = TRUE;
		pstTableInfo->bUpdated = TRUE;
		pstTableInfo->ulLastSyncTime	= HLIB_STD_GetSystemTick();
	}

	return DERR_DATACHANGED;
}
#endif


DERROR			DAMA_DB_Set (DAMA_DB_t *db, const HCHAR *name, HUID uid, HINT32 size, const void *data, HBOOL bCache)
{
	DERROR	err;
#ifdef CONFIG_DEBUG
	STATIC	HUINT32	PerFormance_szie = 0;
	STATIC	HUINT32	Stamp_szie = 0;

	STATIC HUINT32 tick     =  0 ;
	STATIC HUINT32 stamp_count=0, count     =  0 ;

	PerFormance_szie +=size;
	count++;

	if(tick + 10000 < HLIB_STD_GetSystemTick())
	{
		tick = HLIB_STD_GetSystemTick();
		HxLOG_Message("DAMA_DB_Set ===========> Size[%d] count[%d]\n",PerFormance_szie-Stamp_szie, count - stamp_count);
		Stamp_szie = PerFormance_szie;
		stamp_count = count;
	}
#endif

	HxLOG_Trace();
	HxLOG_Assert(name);
	err = dama_db_addData(db, name, uid, size, data, bCache);

	if (err == DERR_DATACHANGED)
	{
		DAMA_Table_t * pstTableInfo = dama_db_getTable(db, name);

		if(pstTableInfo)
		{
			s_bIsChangedData = TRUE;
			pstTableInfo->bUpdated = TRUE;
			pstTableInfo->ulLastSyncTime	= HLIB_STD_GetSystemTick();
		}
	}

	return err;
}

DAMA_Result_t *	DAMA_DB_Query (DAMA_DB_t *db, const HCHAR *name, const HCHAR *query)
{
	DAMA_Result_t	*result;

	HxLOG_Trace();
	HxLOG_Debug("%s(%s, %s)\n", __FUNCTION__, name, query);

#if defined(CONFIG_DAMA_BACKGROUND_LOAD)
{
	damaDbCheckQuery_t		 stArg;

	HxSTD_MemSet(&stArg, 0, sizeof(damaDbCheckQuery_t));
	stArg.bFound	= FALSE;
	stArg.szQuery	= (HCHAR *)query;
	stArg.db		= db;

	dama_db_forEachObject(dama_db_checkQueryUsingUnloadedDb, (void *)&stArg);
	if (TRUE == stArg.bFound)
	{
		HxLOG_Error("Not Initialized yet... Do not Query!!! name(%s)\n", name);
		return NULL;
	}
}
#endif

	result = dama_db_queryData(db, name, query, FALSE);
#ifdef DAMA_MONITOR_DATA
	if (result)
	{
		DAMA_Item_t *item;
		HxVECTOR_EACH(result->pList, DAMA_Item_t *, item, {
			DAMA_MONITOR_DATA(db, item->tablename, item->data, item->size);
		});
	}
#endif
	return result;
}

void			DAMA_DB_Remove (DAMA_DB_t *db, const HCHAR *name, HUID uid, HBOOL useCache)
{
	HxLOG_Trace();
	HxLOG_Debug("%s(%s, "HUID_FMT")\n", __FUNCTION__, name, uid);
	if (name == NULL)
	{
		//	remove all.
		dama_db_deleteTable(db, NULL, useCache);
		DAMA_TRACE("Remove all!!!");
		return;
	}

	if (uid == DAMA_TABLE_RESET_UID)
	{
		dama_db_deleteTable(db, name, useCache);
		DAMA_TRACE("[%s] Remove table", name);
	}
	else
	{
		dama_db_removeItem(db, name, uid, useCache);
	}
}


const DAMA_Item_t *	DAMA_DB_NextItem (DAMA_Result_t *result)
{
	HxLOG_Assert(result);

	return (const DAMA_Item_t *)HLIB_VECTOR_ItemAt(result->pList, result->nIndex++);
}

HBOOL			DAMA_DB_AutoCommit(DAMA_DB_t *db, HBOOL enable)
{
	HINT32 rc=SQLITE_OK;

	HxLOG_Trace();

	if (enable)
		rc=dama_db_sqlite_exec(db->pMainDB, "COMMIT", NULL, NULL);
	else
		rc=dama_db_sqlite_exec(db->pMainDB, "BEGIN", NULL, NULL);

	if(rc != SQLITE_OK)
	{
		HxLOG_Error("dama_db_sqlite_exec err:(%d)\n",rc);
		return FALSE;
	}

	return sqlite3_get_autocommit(db->pMainDB) ? TRUE : FALSE;
}

HBOOL		DAMA_DB_Dump (DAMA_DB_t *db, const HCHAR *dest)
{
	return dama_db_dump(db, dest);
}

#if defined(CONFIG_DAMA_BACKGROUND_SYNC)
typedef struct
{
	DAMA_DB_t	*db;
	void		*data;
	HUINT32		 size;
} DamaQueueItem_t;

static HBOOL	dama_db_queueData (const HCHAR *objName, const HCHAR *tags[], void *userdata)
{
	DamaQueueItem_t			*pstItem = (DamaQueueItem_t *)userdata;
	DAMA_Table_t			*pstTable;

	if ((NULL != objName) && (NULL != pstItem) && (NULL != pstItem->data) && (0 != pstItem->size))
	{
		pstTable = dama_db_getTable(pstItem->db, objName);
		if (NULL != pstTable)
		{
			if (pstTable->bSyncInOtherThread && pstTable->bInSyncJob)
			{
				void		*pvDupData;

				pvDupData = (void *)HLIB_STD_MemAlloc(pstItem->size);
				if (NULL != pvDupData)
				{
					HxSTD_MemCopy(pvDupData, pstItem->data, pstItem->size);
					pstTable->pstQueueSetData = HLIB_LIST_Append(pstTable->pstQueueSetData, pvDupData);
				}
			}
		}
	}

	return FALSE;
}

static HERROR dama_db_syncTempFile (DAMA_DB_t *db, DAMA_Table_t *table, const HCHAR *tempfile)
{
	sqlite3 	*fileDb;
	HCHAR		*szFlashPath = NULL;

	// 1. Vacuum the temporary file:
	if (sqlite3_open(tempfile, &fileDb) != SQLITE_OK)
	{
		return ERR_FAIL;
	}

	if (dama_db_sqlite_exec(fileDb, "VACUUM;", 0, 0) != SQLITE_OK)
	{
		(void)sqlite3_close(fileDb);
		return ERR_FAIL;
	}

	if (sqlite3_close(fileDb) != SQLITE_OK)
	{
		return ERR_FAIL;
	}

	// 2. Copy the file to the FLASH
	szFlashPath = (HCHAR *)HLIB_STD_MemAlloc(MAX_DB_PATH_LEN + MAX_DB_FILE_LEN);
	if (NULL != szFlashPath)
	{
		if (dama_db_getDbPath(db, table->szObjName, 0, szFlashPath) == DERR_OK)
		{
			(void)dama_db_copyFile(tempfile, (const HCHAR *)szFlashPath);
		}

		HLIB_STD_MemFree(szFlashPath);
	}

	return ERR_OK;
}

static HERROR dama_db_replaceDbSyncFile (DAMA_DB_t *db, DAMA_Table_t *table)
{
	const HCHAR 	*szSqlQuery;
	HCHAR 			*szRamFileName = NULL;
	HCHAR			*szSyncFileName = NULL;
	DERROR			 eErr;
	HERROR			 hResult = ERR_FAIL;

	szRamFileName = (HCHAR *)HLIB_STD_MemAlloc(MAX_DB_PATH_LEN + MAX_DB_FILE_LEN);
	szSyncFileName = (HCHAR *)HLIB_STD_MemAlloc(MAX_DB_PATH_LEN + MAX_DB_FILE_LEN);
	if ((NULL == szRamFileName) || (NULL == szSyncFileName))
	{
		HxLOG_Error("memory allocation failed:\n");
		goto END_FUNC;
	}

	eErr = dama_db_getDbPath(db, table->szObjName, -1, szRamFileName);
	if (DERR_OK != eErr)
	{
		HxLOG_Error("dama_db_getDbPath err: objName[%s], dbIndex[%d]\n", table->szObjName, -1);
		goto END_FUNC;
	}

	HxSTD_PrintToStrN(szSyncFileName, MAX_DB_PATH_LEN + MAX_DB_FILE_LEN - 1, "%s.tmp", szRamFileName);
	szSyncFileName[MAX_DB_PATH_LEN + MAX_DB_FILE_LEN - 1] = '\0';

	// Detach the DB
	szSqlQuery = dama_db_makeSQL(db, "DETACH %s;", table->szObjName);
	if(dama_db_sqlite_exec(db->pMainDB, szSqlQuery, NULL, 0) != SQLITE_OK)
	{
		HxLOG_Error("sqlite exec failed: [%s]\n", szSqlQuery);
		goto END_FUNC;
	}

	// Remove the old ram file and replace the vacuumed one.
	if (HLIB_FILE_Exist(szRamFileName))
	{
		HLIB_FILE_Delete(szRamFileName);
	}

	HLIB_FILE_Move(szSyncFileName, szRamFileName);

	// Attach again:
	szSqlQuery = dama_db_makeSQL(db, "ATTACH DATABASE '%s' AS %s; ", szRamFileName, table->szObjName);
	if(dama_db_sqlite_exec(db->pMainDB, szSqlQuery, NULL, 0) != SQLITE_OK)
	{
		HxLOG_Critical("[%s] Load file DB: Failed to attach file DB with sql \"%s\"", table->szObjName, szSqlQuery);

		szSqlQuery = dama_db_makeSQL(db, "DETACH %s; ATTACH DATABASE '%s' AS %s;", table->szObjName, szRamFileName, table->szObjName);
		if (dama_db_sqlite_exec(db->pMainDB, szSqlQuery, NULL, 0) != SQLITE_OK)
		{
			HxLOG_Critical("[%s] Load file DB: Failed to re-attach file DB with sql \"%s\"", table->szObjName, szRamFileName);
			goto END_FUNC;
		}
	}

	hResult = ERR_OK;
END_FUNC:
	if (NULL != szRamFileName)					{ HLIB_STD_MemFree(szRamFileName); }
	if (NULL != szSyncFileName)					{ HLIB_STD_MemFree(szSyncFileName); }
	return hResult;
}

static HERROR dama_db_executeSetFunctionDuringSync (DAMA_DB_t *db, DAMA_Table_t *table)
{
	while (table->pstQueueSetData)
	{
		HUINT8			*pucSetData = (HUINT8 *)HLIB_LIST_Data(table->pstQueueSetData);

		if (NULL != pucSetData)
		{
			DAMA_Header_t	*pstHeader = (DAMA_Header_t *)pucSetData;

 			if (pstHeader->size > 0)
			{
				DAMA_DB_Set(db, table->szObjName, pstHeader->uid, pstHeader->size, pucSetData + sizeof(DAMA_Header_t), FALSE);
			}
			else
			{
				DAMA_DB_Remove(db, table->szObjName, pstHeader->uid, FALSE);
			}

			table->pstQueueSetData = HLIB_LIST_Remove(table->pstQueueSetData, (const void *)pucSetData);
			HLIB_STD_MemFree(pucSetData);
		}
	}

	return ERR_OK;
}

HERROR		DAMA_DB_QueueDataDuringSync (DAMA_DB_t *db, const HCHAR *name, const void *data, HUINT32 size)
{
	DamaQueueItem_t		 stItem;

	stItem.db		= db;
	stItem.data		= (void *)data;
	stItem.size	= size;

	if (name == NULL)
	{
		dama_db_forEachObject(dama_db_queueData, (void *)&stItem);
	}
	else
	{
		dama_db_queueData(name, NULL, (void *)&stItem);
	}

	return DERR_OK;
}

HERROR		DAMA_DB_SyncTempFile (DAMA_DB_t *db, const HCHAR *name, const HCHAR *tempfile)
{
	DAMA_Table_t	*table = dama_db_getTable(db, name);

	(void)dama_db_backupTable(db, table);
	VK_TASK_Sleep(30);
	(void)dama_db_syncTempFile(db, table, tempfile);

	return ERR_OK;
}

HERROR		DAMA_DB_FinalizeSync (DAMA_DB_t *db, const HCHAR *name)
{
	DAMA_Table_t	*table = dama_db_getTable(db, name);
	HERROR			 hErr;

	if ((NULL != table) && (table->bSyncInOtherThread) && (table->bInSyncJob))
	{
		hErr = dama_db_replaceDbSyncFile(db, table);
		if (ERR_OK == hErr)
		{
			(void)dama_db_executeSetFunctionDuringSync(db, table);
		}

		table->bInSyncJob = FALSE;
		table->ulLastSyncTick = HLIB_STD_GetSystemTick();
	}

	return ERR_OK;
}

#endif

#if defined(CONFIG_DAMA_BACKGROUND_LOAD)
HERROR DAMA_DB_LoadDbTableAsync (DAMA_DB_t *db)
{
	dama_db_forEachObject(dama_db_loadLeftTables, db);
	return ERR_OK;
}
#endif

#if defined(DAMA_CHECK_STORAGE_STATUS)
STATIC HxList_t		*s_pstDamaDbStorages = NULL;

STATIC DxStorage_Info_t *dama_db_findStorage (HCHAR *szMountPath)
{
	if ((NULL != szMountPath) && ('\0' != *szMountPath))
	{
		HxList_t			*pstList;

		for (pstList = s_pstDamaDbStorages; NULL != pstList; pstList = HLIB_LIST_Next(pstList))
		{
			DxStorage_Info_t	*pstStorage = (DxStorage_Info_t *)HLIB_LIST_Data(pstList);

			if ((NULL != pstStorage) && (HxSTD_StrCmp(szMountPath, pstStorage->szMountPath) == 0))
			{
				return pstStorage;
			}
		}
	}

	return NULL;
}

STATIC HERROR dama_db_appendStorage (DxStorage_Info_t *pstStorage)
{
	DxStorage_Info_t	*pstDstStorage = NULL;

	if (NULL == pstStorage)					{ return ERR_FAIL; }

	pstDstStorage = dama_db_findStorage(pstStorage->szMountPath);
	if (NULL != pstDstStorage)
	{
		HxSTD_MemCopy(pstDstStorage, pstStorage, sizeof(DxStorage_Info_t));
	}
	else
	{
		pstDstStorage = (DxStorage_Info_t *)HLIB_STD_MemAlloc(sizeof(DxStorage_Info_t));
		if (NULL == pstDstStorage)				{ return ERR_FAIL; }

		HxSTD_MemCopy(pstDstStorage, pstStorage, sizeof(DxStorage_Info_t));
		s_pstDamaDbStorages = HLIB_LIST_Append(s_pstDamaDbStorages, (void *)pstDstStorage);
	}

	return ERR_OK;
}

STATIC HERROR dama_db_removeStorage (HCHAR *szMountPath)
{
	DxStorage_Info_t	*pstStorage = dama_db_findStorage(szMountPath);

	if (NULL == pstStorage)					{ return ERR_FAIL; }

	s_pstDamaDbStorages = HLIB_LIST_Remove(s_pstDamaDbStorages, (const void *)pstStorage);
	HLIB_STD_MemFree(pstStorage);

	return ERR_OK;
}

STATIC HERROR dama_db_executeStorageInsertion (DAMA_DB_t *db, DxStorage_Info_t *pstStorage)
{
	const HCHAR 		*objName;
	DAMA_Table_t		*table;
	HxHASH_TableIter_t	 iter;

	HLIB_HASH_IterInit(&iter, db->pTableMap);
	while (HLIB_HASH_IterNext(&iter, (void **)&objName, (void **)&table))
	{
		(void)DLIB_OnStorageAdded(objName, pstStorage);
	}

	(void)dama_db_appendStorage(pstStorage);
	return ERR_FAIL;
}

STATIC HERROR dama_db_executeStorageDeletion (DAMA_DB_t *db, DxStorage_Info_t *pstStorage)
{
	const HCHAR 		*objName;
	DAMA_Table_t		*table;
	HxHASH_TableIter_t	 iter;

	HLIB_HASH_IterInit(&iter, db->pTableMap);
	while (HLIB_HASH_IterNext(&iter, (void **)&objName, (void **)&table))
	{
		(void)DLIB_OnStorageRemoved(objName, pstStorage);
	}

	(void)dama_db_removeStorage(pstStorage->szMountPath);
	return ERR_FAIL;
}

HERROR DAMA_DB_ExecuteForStorageChange (DAMA_DB_t *db, DAMA_Cmd_e eCmd, const void *pvData)
{
	HCHAR					*szPath = NULL;
	DxStorage_Info_t		*pstStorage;
	HERROR					 hErr;

	if ( (db == NULL) || (pvData == NULL) )
		return ERR_FAIL;

	switch (eCmd)
	{
	case eDAMA_CMD_STORAGE_INSERTED:
		pstStorage = (DxStorage_Info_t *)pvData;
		if (dama_db_findStorage(pstStorage->szMountPath) == NULL)
		{
			hErr = dama_db_executeStorageInsertion(db, pstStorage);
			if (ERR_OK != hErr)
			{
				HxLOG_Error("dama_db_executeStorageInsertion failed: Storage DevIdx(%d), MountPath(%s)\n", pstStorage->ulDeviceId, pstStorage->szMountPath);
			}
		}
		break;

	case eDAMA_CMD_STORAGE_REMOVED:
		pstStorage = (DxStorage_Info_t *)pvData;
		pstStorage = dama_db_findStorage(pstStorage->szMountPath);
		if (NULL != pstStorage)
		{
			hErr = dama_db_executeStorageDeletion(db, pstStorage);
			if (ERR_OK != hErr)
			{
				HxLOG_Error("dama_db_executeStorageDeletion failed: MountPath(%s)\n", pstStorage->szMountPath);
			}
		}
		break;

	case eDAMA_CMD_STORAGE_UNMOUNTED:
			szPath = ((HCHAR *)pvData + sizeof(HUINT32));
			pstStorage = dama_db_findStorage(szPath);
			if (NULL != pstStorage)
			{
				hErr = dama_db_executeStorageDeletion(db, pstStorage);
				if (ERR_OK != hErr)
				{
					HxLOG_Error("dama_db_executeStorageDeletion failed: MountPath(%s)\n", pstStorage->szMountPath);
				}
			}
		break;

	default:
		break;
	}

	return ERR_OK;

}
#endif

void DAMA_DB_ForEachObject (HBOOL (*func)(const HCHAR *objectName, const HCHAR *tags[], void *userdata), void *userdata)
{
	dama_db_forEachObject(func, userdata);
}

DAMA_Table_t *DAMA_DB_GetTable (DAMA_DB_t *db, const HCHAR *objName)
{
	return dama_db_getTable(db, objName);
}

#ifdef	CONFIG_DEBUG

static HINT32	dama_db_DebugQuery (DAMA_DB_t *db, const HCHAR *pszSQL)
{
	HCHAR			*pszSQLResult;
	HINT32			rst, colCount, col, ctype;
	sqlite3_stmt	*pstStmt;
	HUINT32			tick;

	if (HxSTD_StrEmpty(pszSQL))
	{
		HLIB_CMD_Printf("pszSQL is empty...\n");
		return 1;
	}
	tick = HLIB_STD_GetSystemTick();
	rst = sqlite3_prepare_v2(db->pMainDB, pszSQL, HxSTD_StrLen(pszSQL), &pstStmt, (const char**)&pszSQLResult);
	if (rst != SQLITE_OK)
	{
		HLIB_CMD_Printf("SQLITE prepare FAILED... [SQL:%s][%s]\n", pszSQL, pszSQLResult);
		return 1;
	}

	colCount = sqlite3_column_count(pstStmt);
	HLIB_CMD_Printf("\n");
	for (col = 0; col < colCount; col++)
	{
		if (sqlite3_column_type(pstStmt, col) == SQLITE_INTEGER)
			HLIB_CMD_Printf("[%8s] ", sqlite3_column_name(pstStmt, col));
		else
			HLIB_CMD_Printf("[%16s] ", sqlite3_column_name(pstStmt, col));
	}

	while ((rst = sqlite3_step(pstStmt)) == SQLITE_ROW)
	{
		for (col = 0; col < colCount; col++)
		{
			ctype = sqlite3_column_type(pstStmt, col);
			switch (ctype)
			{
				case SQLITE_INTEGER:
					HLIB_CMD_Printf("%8d", sqlite3_column_int(pstStmt, col));
					break;
				case SQLITE_FLOAT:
					HLIB_CMD_Printf("%8f", sqlite3_column_double(pstStmt, col));
					break;
				case SQLITE_BLOB:
					HLIB_CMD_Printf("%16s", "[BLOB]");
					break;
				case SQLITE_NULL:
					HLIB_CMD_Printf("%16s", "[NULL]");
					break;
				case SQLITE_TEXT:
					HLIB_CMD_Printf("[%16s]", sqlite3_column_text(pstStmt, col));
					break;
			}
		}
		HLIB_CMD_Printf("\n");
	}
	HLIB_CMD_Printf("\nret:%d\ntick:%u\n", rst, HLIB_STD_GetSystemTick() - tick);

	sqlite3_finalize(pstStmt);


	return 0;
}


static HBOOL 	dama_db_PrintSchema(const HCHAR *name, const HCHAR *tags[], void *userdata)
{
	DAMA_DB_t	*db;
	FILE		*fp;
	HINT32		i, n;
	HCHAR		*szPath = NULL;
	const DxField_t	*field;

	db = (DAMA_DB_t *)((void **)userdata)[0];
	fp = (     FILE *)((void **)userdata)[1];

	szPath = (HCHAR *)HLIB_STD_MemAlloc(MAX_DB_PATH_LEN + MAX_DB_FILE_LEN);
	if (NULL == szPath)
	{
		HxLOG_Error("memory allocation failed:\n");
		goto END_FUNC;
	}

	fprintf(fp, "\nTABLE: \"%s\"\n", name);
	for (i = 0 ; dama_db_getDbPath(db, name, i, szPath) == DERR_OK && strcmp(szPath, ":memory:") != 0 ; i++)
		fprintf(fp, "\tpath[%d]:%s\n", i, szPath);
	fprintf(fp,
		"+------------------------------+---------------+----------+\n"
		"|            Field             |     Type      | Indexable|\n"
		"+------------------------------+---------------+----------+\n"
		"| %28s | %13s |     %s    |\n"
		"| %28s | %13s |     %s    |\n"
		"| %28s | %13s |     %s    |\n"
		, "uid", "INTEGER(8)", "Y"
		, "link", "TEXT", "N"
		, "linkdata", "BLOB", "N"
	);

	n = DLIB_GetFieldCount(name);
	for (i = 0 ; i < n ; i++)
	{
#if !defined(CONFIG_TEMP_DLIB_NO_AUTOMATIC_TRASHING)
		field = DLIB_GetField(name, i, NULL, 0);

		fprintf(fp, "| %28s | %13s |     %s    |\n"
			, field->name
			, field->type == eDxTYPE_INTEGER ? "INTEGER(4)"
			: field->type == eDxTYPE_BINARY  ? "BLOB" : "TEXT"
			, field->indexable ? "Y" : "N"
		);
#else
		field = DLIB_CreateGetFieldData(name, i, NULL, 0);
		if (NULL != field)
		{
			fprintf(fp, "| %28s | %13s |	 %s    |\n"
				, field->name
				, field->type == eDxTYPE_INTEGER ? "INTEGER(4)"
				: field->type == eDxTYPE_BINARY  ? "BLOB" : "TEXT"
				, field->indexable ? "Y" : "N"
			);

			DLIB_DestroyGetFieldData(field);
		}
#endif
	}
	fprintf(fp,
		"+------------------------------+---------------+----------+\n"
	);

END_FUNC:
	if (NULL != szPath)					{ HLIB_STD_MemFree(szPath); }
	return FALSE;
}


static HINT32	dama_db_DebugPrintDBSchema (DAMA_DB_t *db, const HCHAR *output)
{
	void *argv[2];
	FILE *fp;

	if (HxSTD_StrEmpty(output))
		fp = stdout;
	else
		fp = fopen(output, "w");

	argv[0] = (void *)db;
	argv[1] = fp;

	dama_db_forEachObject (dama_db_PrintSchema, (void *)argv);

	if (!HxSTD_StrEmpty(output))
		fclose(fp);
	return 0;
}

STATIC int dama_db_cmd_callback(void *NotUsed, int argc, char **argv, char **azColName)
{
	HINT32 	i;

	NotUsed=0;
	for(i=0; i<argc; i++)
	{
		HLIB_CMD_Printf("%20s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
  	HLIB_CMD_Printf("\n");

	return 0;
}

STATIC HINT32 dama_db_DebugSqliteExec(DAMA_DB_t *db, const HCHAR *pszSQL)
{
	HCHAR			*pszSQLResult;
	HINT32			rst;
	HUINT32			tick;

	if (HxSTD_StrEmpty(pszSQL))
	{
		HLIB_CMD_Printf("pszSQL is empty...\n");
		return 1;
	}

	HLIB_CMD_Printf("pszSQL Query: [[ %s ]] \n", pszSQL);

	tick = HLIB_STD_GetSystemTick();

	rst = sqlite3_exec(db->pMainDB, pszSQL, dama_db_cmd_callback, (void *)NULL, (char**)&pszSQLResult);
	if (rst != SQLITE_OK)
	{
		HLIB_CMD_Printf("SQLITE prepare FAILED... [SQL:%s][%s]\n", pszSQL, pszSQLResult);
		return 1;
	}

	HLIB_CMD_Printf("\nret:%d\ntick:%u\n", rst, HLIB_STD_GetSystemTick() - tick);

	return 0;
}

HINT32		DAMA_DB_Command(DAMA_DB_t *db, const HCHAR *cmd, void *data)
{
	HxLOG_Assert(db);

	if (HLIB_STD_StrCaseCmp(cmd, "SQL") == 0)
	{
		return dama_db_DebugQuery(db, data);
	}
	else if (HLIB_STD_StrCaseCmp(cmd, "SCHEMA") == 0)
	{
		return dama_db_DebugPrintDBSchema(db, data);
	}
	else if (HLIB_STD_StrCaseCmp(cmd, "SQLITE") == 0)
	{
		return dama_db_DebugSqliteExec(db, data);
	}
	if (HLIB_STD_StrCaseCmp(cmd, "MEM") == 0)
	{
		HLIB_CMD_Printf("DAMA DB Used : [%lld] bytes\n", sqlite3_memory_used());
		return HxCMD_OK;
	}


	return 0;
}
#endif


