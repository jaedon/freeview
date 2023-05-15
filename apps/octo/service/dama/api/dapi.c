/**************************************************************
 *	@file		dama.c
 *	HumaxDataManager for OCTO 2.0
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/03/01
 *	@author			humax
 *	@brief			HumaxDataManager for OCTO 2.0 Project
 **************************************************************/
#include	<unistd.h>
#include	<vkernel.h>
#include	<hlib.h>

#include	<dama_int.h>

#if defined(CONFIG_FUNC_USE_DAPI_DIRECT_QUERY)
#include	<dlib.h>
#include	<dama_db.h>
#endif

#ifdef	CONFIG_DEBUG
#define	DAPI_DEBUG_EXPR(expr)	do {expr;}while(0)
#else
#define	DAPI_DEBUG_EXPR(expr)
#endif

#define	DAPI_LAZY_JOB_MAX		5
#define	DAPI_CIRCULARBUF_SIZE	20480

#define DAPI_SEND_FAIL_MAXIMUM				16
#define DAPI_SEND_ASYNC_QUEUE_DEFAULT		128

// #define DAPI_MONITOR_SOCKET_SENDING
// #define DAPI_MONITOR_SOCKET_SENDING_PROCESS			"obama"

typedef struct dapi	dapi_t;
typedef HERROR (*dapi_lazyfunc_t) (dapi_t *, const HCHAR *, ...);

typedef struct dapi_request
{
	HUINT32			id;
	HCHAR			name[DAMA_OBJECTNAME_LEN];
	DAPI_Getter		callback;
	void *			userdata;
	struct dapi_request *next;

#ifdef	CONFIG_DEBUG
	HCHAR			query[DAMA_OBJECTNAME_LEN];
	HUINT32			tick;
#endif
} dapi_request_t;

typedef struct
{
	HUID		uid;
	HINT32		size;
	HUINT8		data[4];
} dapi_list_t;

typedef struct dapi_batch
{
	HCHAR			name[DAMA_OBJECTNAME_LEN];
	HINT32			buffer;

	HINT32			refcount;

	HINT32			size;
	HxList_t		*list;

	struct dapi_batch *next;
} dapi_batch_t;

typedef struct dapi_lazyjob
{
	HCHAR			name[DAMA_OBJECTNAME_LEN];
	HUINT32			timestamp;
	HUINT32			term;
	dapi_lazyfunc_t	func;
} dapi_lazyjob_t;

typedef struct dapi_monitor
{
	HCHAR			name[DAMA_OBJECTNAME_LEN];
	DAPI_Monitor	callback;
	void			*userdata;
} dapi_monitor_t;

#if defined(CONFIG_FUNC_USE_DAPI_DIRECT_QUERY)
#define	DAPI_BUF_THRESHOLD			(1024 * 1024 * 2)
#define	DAPI_TEMP_BUF				3
#define	DAPI_TEMP_STREAM			3
#endif

typedef enum
{
	eReCnctState_None				= 0,

	eReCnctState_Start,
	eReCnctState_DisconnectOld,
	eReCnctState_ConnectNew,
	eReCnctState_Done,

	eReCnctState_EndOfCase
} dapi_reconnect_State_e;

struct dapi
{
	HINT32			socket;

	dapi_request_t	*request;
	dapi_request_t	*observer;
	dapi_request_t	*freed;
	dapi_batch_t	*batch;

	HxHASH_t	*lazyjob;

	HxTREE_t		*monitor;

	HINT32			send_stream;

	HUINT32			semaphore;
	HxCBUF_t			buf;
	HUINT16			idseq;
	HUINT16			pid16;
#if	defined(DAMA_LOGGING) && DAMA_LOGGING == 0
	FILE			*logfp;
#if defined(DAPI_CMD_LOGGING)
	FILE			*cmdlogfp;
#endif
#endif

	HINT32			snd_timeout;
	HINT32			rcv_timeout;
	HINT32			snd_bufsize;
	HINT32			rcv_bufsize;
	HINT32			num_socket_failed;
	HUINT32			socket_fail_tick;
	dapi_reconnect_State_e	 recnct_state;
	unsigned long			 send_task_qid;

#if defined(CONFIG_FUNC_USE_DAPI_DIRECT_QUERY)
	DAMA_DB_t		*db;
	struct {
		void *	buf[DAPI_TEMP_BUF];
		HUINT32 size[DAPI_TEMP_BUF];
		HINT32	index;
	} tempbuf;
	struct {
		HINT32	rwstream[DAPI_TEMP_STREAM];
		HINT32	index;
	} tempstream;

#endif

};

typedef	struct
{
	HINT32			 nSocket;
	void			*pvData;
	HUINT32			 ulSize;
	HUINT32			 ulReqId;
} dapi_SendMsg_t;

STATIC HINT32		 s_nDapiBlockedSocket;

static dapi_t *	dapi_get_instance (void);
static void		dapi_release_instance(dapi_t * dapi);
static void	dapi_receive_data (dapi_t *dapi, HINT32 socket_handle, HUINT32 request_id, HUINT32 size, const void *data, HBOOL bDapiDirect);
static HERROR	dapi_send (dapi_t *dapi, const void *data, HUINT32 n, HUINT32 reqid);

#ifdef DAPI_CMD_LOGGING
static void	dapi_CmdDebug (const HCHAR *function, const HCHAR *format, ...)
{
	HCHAR	buf[512]="";
	HCHAR	*msg = NULL, *dbuf = NULL;
	va_list	ap, cp;
	HINT32	n = 0;
	FILE	*fp = NULL;

	dapi_t * dapi = dapi_get_instance();

	if (dapi == NULL)
		return;

	fp = dapi->cmdlogfp;

	if(fp)
	{
		va_start(ap, format);
		va_copy(cp, ap);
		n = HxSTD_PrintToStrV(buf, 512, format, ap);
		if (n >= 512 || n < 0)
		{
			dbuf = DAMA_Calloc(4096);
			if (dbuf != NULL)
			{
				msg = dbuf;
				HxSTD_PrintToStrV(dbuf, 4096, format, cp);
			}
		}
		else
		{
			msg  = buf;
		}
		va_end(cp);
		va_end(ap);
		if(msg) HLIB_FILE_Printf(fp,"%s %s %s", HLIB_DATETIME_UnixTimeToString(HLIB_STD_GetSystemTime(), NULL, 0), function, msg);
		HLIB_FILE_Flush(fp);
	}

	if(msg) HxLOG_Message("<%s:%s>", function, msg);

	if (dbuf)
		DAMA_Free(dbuf);
	dapi_release_instance(dapi);
	return;
}
#endif


void		dapi_remove_compile_warning (void)
{
#ifdef	CONFIG_DEBUG
	(void)dama_Debug;
#ifdef	DAMA_LOGGING
	(void)dama_Logging;
#ifdef DAPI_CMD_LOGGING
	(void)dapi_CmdDebug;
#endif
#endif
#endif
}

#if !defined(DAPI_CMD_LOGGING)
#define	DapiLOG(...)	HxLOG_Debug(__VA_ARGS__)//HxLOG_Trace()
#else
#define	DapiLOG(...) dapi_CmdDebug (__FUNCTION__, __VA_ARGS__)
#endif


#if defined(CONFIG_FUNC_USE_DAPI_DIRECT_QUERY)

#define	CACHE_USE_HASHTABLE

#ifdef CONFIG_DEBUG
static int dapi_db_sqlite_exec(sqlite3* db, const char * sql, int (*callback)(void*,int,char**,char**), void * arg)
{
	HCHAR	*errmsg = NULL;
	int		ret = sqlite3_exec(db, sql, callback, arg, &errmsg);
	if(errmsg)
	{
		HxLOG_Error("sqlite3_exe error sql[%s] errmsg[%s]\n", sql, errmsg);
		sqlite3_free(errmsg);
	}
	return ret;
}
#else
#define dapi_db_sqlite_exec(db, sql, callback, arg) sqlite3_exec(db, sql, callback, arg, NULL)
#endif


static void *		dapi_get_tempbuf (dapi_t *dapi, HUINT32 size)
{
	HINT32	index;

	index = dapi->tempbuf.index;
	if (dapi->tempbuf.size[index] < size || dapi->tempbuf.size[index] > DAPI_BUF_THRESHOLD)
	{
		void *buf = HLIB_STD_MemAlloc(size);
		if (buf == NULL)
			return NULL;
		if (dapi->tempbuf.buf[index])
			HLIB_STD_MemFree(dapi->tempbuf.buf[index]);
		dapi->tempbuf.buf[index]  = buf;
		dapi->tempbuf.size[index] = size;
	}

	dapi->tempbuf.index = (index + 1) % DAPI_TEMP_BUF;
	return dapi->tempbuf.buf[index];
}
static HINT32		dapi_get_tempstream (dapi_t *dapi)
{
	HINT32	stream;

	if (dapi->tempstream.rwstream[dapi->tempstream.index] == 0)
	{
		dapi->tempstream.rwstream[dapi->tempstream.index] = HLIB_RWSTREAM_Open(NULL, 1024);
	}
	stream = dapi->tempstream.rwstream[dapi->tempstream.index];
	if (stream)
	{
		if (HLIB_RWSTREAM_GetBufSize(stream) > DAPI_BUF_THRESHOLD)
		{
			HLIB_RWSTREAM_Close(stream);
			dapi->tempstream.rwstream[dapi->tempstream.index] = HLIB_RWSTREAM_Open(NULL, 1024);
			stream = dapi->tempstream.rwstream[dapi->tempstream.index];
		}
		else
		{
			HLIB_RWSTREAM_Rewind(stream);
		}
	}
	dapi->tempstream.index = (dapi->tempstream.index + 1) % DAPI_TEMP_STREAM;
	return stream;
}

static	DAMA_Table_t*	dapi_db_getTable(DAMA_DB_t *db, const HCHAR *objName)
{
	return (DAMA_Table_t*)HLIB_HASH_Lookup(db->pTableMap, (void*)objName);
}

static	HCHAR*	dapi_db_getTempBuf(int size)
{
	static HxCBUF_t s_buf;

	if (s_buf == NULL)
	{
		s_buf = HLIB_CBUF_New(0x10000);
		HxLOG_Assert(s_buf);
	}
	return HLIB_CBUF_GetBuffer(s_buf, (HINT32)size);
}

static const HCHAR *	dapi_db_getDbPath(DAMA_DB_t *db, const HCHAR *objectName, HINT32 dbIndex)
{
	HCHAR	*fullname;
	HCHAR	*tempname;

	tempname = dapi_db_getTempBuf(MAX_DB_PATH_LEN + MAX_DB_FILE_LEN);
	//	return tmp dir
	if (dbIndex == -1)
	{
		HxSTD_MemSet(tempname, 0, MAX_DB_PATH_LEN + MAX_DB_FILE_LEN);
		HxSTD_snprintf(tempname, MAX_DB_PATH_LEN + MAX_DB_FILE_LEN - 1, "%s/%s.db", db->szTempPath, objectName);

		return tempname;
	}

	//	Using Default DB.
	if (DLIB_GetDBPath(objectName, 0) == NULL)
	{
		HxSTD_MemSet(tempname, 0, MAX_DB_PATH_LEN + MAX_DB_FILE_LEN);
		if (dbIndex == 0)
			HxSTD_snprintf(tempname, MAX_DB_PATH_LEN + MAX_DB_FILE_LEN - 1, "%s/%s.db", db->szPath, objectName);
		else if (dbIndex == 1)
			HxSTD_snprintf(tempname, MAX_DB_PATH_LEN + MAX_DB_FILE_LEN - 1, "%s/%s.db", db->szBackupPath, objectName);
		else
			return NULL;

		fullname = tempname;
	}
	else
	{
		fullname = (HCHAR*)DLIB_GetDBPath(objectName, dbIndex);
	}
	return fullname;
}

static const HCHAR *	dapi_db_makeSQL (DAMA_DB_t *db, const HCHAR *fmt, ...)
{
	va_list	ap;

	HLIB_RWSTREAM_Rewind(db->nSqlBuf);

	va_start(ap, fmt);
	HLIB_RWSTREAM_VPrint(db->nSqlBuf, fmt, ap);
	va_end(ap);

	return HLIB_RWSTREAM_GetBuf(db->nSqlBuf);
}

static DERROR	dapi_db_loadFileDB (DAMA_DB_t *db, const HCHAR *objName, const  const HCHAR *fileName)
{
	int			ret;
	const HCHAR *sql;
	HCHAR		*fdb;
#ifdef	CONFIG_DEBUG
	HUINT32		tick = HLIB_STD_GetSystemTick();
#endif

	if (!HLIB_FILE_Exist(fileName))
	{
		HxLOG_Warning("There is no DB file [%s].\n", fileName);
		return DERR_FAIL;
	}

#ifdef	CONFIG_DEBUG
{
	const HCHAR *path = DLIB_GetDBPath(objName, 0);
	if (path && strcmp(path, ":memory:") != 0 && !HLIB_DIR_IsMountedPath(fileName))
	{
		HxLOG_Warning("%s(%s): path(%s) does NOT mounted!!\n", __FUNCTION__, objName, fileName);
		return DERR_FAIL;
	}
}
#endif

	fdb = dapi_db_getTempBuf(DxLIB_MAX_NAME + 32);
	HxSTD_PrintToStrN(fdb, DxLIB_MAX_NAME + 32, "%s", objName);
	sql = dapi_db_makeSQL(db, "ATTACH DATABASE '%s' AS %s; ", fileName, fdb);

	ret = dapi_db_sqlite_exec(db->pMainDB, sql, NULL, 0);
	if (ret != SQLITE_OK)
	{
		HxLOG_Warning("[%s] Load file DB: Failed to attach file DB with sql \"%s\", error", objName, sql);

		sql = dapi_db_makeSQL(db, "DETACH %s; ATTACH DATABASE '%s' AS %s;", fdb, fileName, fdb);
		if (dapi_db_sqlite_exec(db->pMainDB, sql, NULL, 0) != SQLITE_OK)
		{
			HxLOG_Warning("[%s] Load file DB: Failed to re-attach file DB with sql \"%s\", error", objName, sql);
			return DERR_FAIL;
		}
	}

	//dama_db_addNewColumn(db, objName);

#ifdef	CONFIG_DEBUG
	if(HLIB_STD_GetSystemTick() - tick > 3000) HxLOG_Warning("Insert Tick[%d]\n", HLIB_STD_GetSystemTick() - tick);
#endif
	DAMA_TRACE("[%s] Load file DB: Success", fileName);
	return DERR_OK;
}






static	HCHAR*	dapi_db_makeSQLWithQuery(DAMA_DB_t *db, const HCHAR *query)
{
	return HLIB_STD_StrDupEx(query, 0x7FFFFFFF, (void *(*)(HUINT32))dapi_db_getTempBuf);
}
static	DERROR	dapi_db_readLink(DAMA_DB_t *db, const HCHAR *name, HUID uid, const HCHAR *pszLink, HINT32 *pnSize, void **ppvData)
{
	return DLIB_ReadData(name, uid, pszLink, ppvData, (HUINT32 *)pnSize);
}

static const HCHAR *	dapi_db_isDeleteLinkQuery(DAMA_DB_t *db, const HCHAR *queryIn)
{
	HCHAR	*query;
	HCHAR	*save;
	HCHAR	*token;

	query = HLIB_STD_StrLStrip((HCHAR *)queryIn, NULL);
	if (*query != 'd' && *query != 'D')
		return NULL;
	if (!HLIB_STD_StrCaseStartWith(query, "DELETE"))
		return NULL;

	query = HLIB_STD_StrDupEx(query, 0x7FFFFFFF, (void *(*)(HUINT32))dapi_db_getTempBuf);
	token = HxSTD_strtok_r(query, " \t\n", &save);
	token = HxSTD_strtok_r(NULL , " \t\n", &save);
	if (token == NULL || HLIB_STD_StrCaseCmp(token, "FROM") != 0)
		return NULL;

	token = HxSTD_strtok_r(NULL , " \t\n", &save);
	if (token == NULL || DLIB_GetDataPath(token) == NULL)
		return NULL;

	return token;
}

static DERROR	dapi_db_deleteLink(const HCHAR *pszObjectName, HUID uid, const HCHAR *link)
{
	return DLIB_WriteData(pszObjectName, uid, link, NULL, 0);
}

static HUINT32	dapi_db_deleteLinkByQuery (DAMA_DB_t *db, const HCHAR *name, const HCHAR *query)
{
	sqlite3_stmt	*stmt;
	const HCHAR		*sql;
	HUINT32			count = 0;

	sql = HLIB_STD_StrDupEx(HLIB_STD_StrLStrip((HCHAR *)query, NULL), 0x7FFFFFFF, (void *(*)(HUINT32))dapi_db_getTempBuf);
	sql = dapi_db_makeSQL(db, "SELECT uid, link %s", sql + 6/*strlen("DELETE")*/);

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
		dapi_db_deleteLink(name, uid, link);
		count++;
	}
	sqlite3_finalize(stmt);

	return count;
}

static DAMA_Result_t *	dapi_db_getTempResult (DAMA_DB_t *db)
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

static DAMA_Result_t*	dapi_db_queryData(DAMA_DB_t *db, const HCHAR *name, const HCHAR *query, HBOOL bPreload)
{
	HINT32	rst, size;
	HCHAR	*pszSQLRule;
	void	*pvEncodedData, *pvRealData;
	HINT32	nEncodedSize, nRealSize;
	int		i, n;
	void 	*garbage[4];
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
	const HCHAR		*extName;
#if 1  // TODO:
	if ((extName = dapi_db_isDeleteLinkQuery(db, query)) != NULL)
	{
		if (dapi_db_deleteLinkByQuery(db, extName, query) > 0)
		{
			table = dapi_db_getTable(db, extName);
			if (table)
			{
//				s_bIsChangedData = TRUE;
				table->bUpdated = TRUE;
			}
		}
	}
#endif

	HxLOG_Debug("pMainDB: 0x%x \n", db->pMainDB);
	pszSQLRule = dapi_db_makeSQLWithQuery(db, query);

	HxLOG_Debug("pszSQLRule: %s \n", pszSQLRule);


	sqlite3_busy_timeout(db->pMainDB, 5000);

	rst = sqlite3_prepare_v2(db->pMainDB, pszSQLRule, HxSTD_StrLen(pszSQLRule), &pstStmt, NULL);
	if (rst != SQLITE_OK)
	{
		DAMA_ERROR("[UNKNOWN] prepare failed: %s, rst: %d, db->pMainDB: 0x%x", pszSQLRule, rst, db->pMainDB);
		return NULL;
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
		result = dapi_db_getTempResult(db);


	while (sqlite3_step(pstStmt) == SQLITE_ROW)
	{
		HUID	uid;

		HxLOG_Debug("cindex.uid: %d \n", cindex.uid);
		if (cindex.uid < 0)
			uid = 0;
		else
			uid = (HUID)sqlite3_column_int64(pstStmt, cindex.uid);

		HxLOG_Debug("uid: %d \n", uid);
		tablename = NULL;
		nEncodedSize  = 0;
		pvEncodedData = NULL;
#if defined(CONFIG_TEMP_DLIB_NO_AUTOMATIC_TRASHING)
		nRealSize = 0;
		pvRealData = NULL;
#endif

		HxLOG_Debug("cindex.link: %d \n", cindex.link);
		if (cindex.link >= 0)
		{
			const HCHAR *link = (const HCHAR *)sqlite3_column_text(pstStmt, cindex.link);

			tablename = sqlite3_column_table_name(pstStmt, cindex.link);
			if(tablename != NULL)
			{
				HxLOG_Debug("tablename: %s \n", tablename);
			}
			else
			{
				HxLOG_Debug("tablename is NULL \n");
			}
			if (!HxSTD_StrEmpty(link))
			{
				if (tablename == NULL && cindex.tablename >= 0)
				{
					tablename = sqlite3_column_text(pstStmt, cindex.tablename);
				}
				if (dapi_db_readLink(db, tablename, uid, link, &nEncodedSize, &pvEncodedData) != DERR_OK)
				{
					DAMA_ERROR("[%s] Failed to open VFS: %s", tablename, link);
					continue;
				}
				garbage[gbglen++] = pvEncodedData;
			}
		}

		HxLOG_Debug("cindex.linkdata: %d \n", cindex.linkdata);
		if (cindex.linkdata >= 0)
		{
			if(tablename != NULL)
			{
				HxLOG_Debug("tablename: %s \n", tablename);
			}
			else
			{
				HxLOG_Debug("tablename is NULL \n");
			}
			tablename = sqlite3_column_table_name(pstStmt, cindex.linkdata);
			if(tablename != NULL)
			{
				HxLOG_Debug("tablename: %s \n", tablename);
			}
			else
			{
				HxLOG_Debug("tablename is NULL \n");
			}
			size = sqlite3_column_bytes(pstStmt, cindex.linkdata);
			if (size > 0)
			{
				nEncodedSize  = size;
				pvEncodedData = (void *)sqlite3_column_blob(pstStmt, cindex.linkdata);
			}
		}

		HxLOG_Debug("cindex.tablename: %d \n", cindex.tablename);
		if (tablename == NULL && cindex.tablename >= 0)
		{
			tablename = sqlite3_column_text(pstStmt, cindex.tablename);
		}

		if (pvEncodedData && nEncodedSize && tablename)
		{
			DAMA_Item_t	*item;
			HUINT32 	tnameLen;
			dama_cache_t	*pstCache = NULL;

			HxLOG_Debug("Encoded UID ["HUID_FMT"] Size [%d], linkdata:%d, link:%d\n", uid, nEncodedSize, cindex.linkdata, cindex.link);
			HxLOG_Debug("tablename: %s \n", tablename);
			table = dapi_db_getTable(db, tablename);
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
				} else HxLOG_Debug("Decoding Error .. \n");

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
				item = (DAMA_Item_t *)HLIB_STD_MemAlloc(sizeof(DAMA_Item_t) + nEncodedSize + tnameLen);
				if (item)
				{
					item->uid  = uid;
					item->size = nEncodedSize + tnameLen + sizeof(HUINT32);
					item->tablename = item->data + sizeof(HUINT32);
					HxSTD_memcpy(item->data, &tnameLen, sizeof(HUINT32));
					HxSTD_memcpy(item->data + sizeof(HUINT32), tablename, tnameLen);
					HxSTD_memcpy(item->data + sizeof(HUINT32) + tnameLen, pvEncodedData, nEncodedSize);

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

	HxLOG_Debug("%s end \n", __FUNCTION__);
	return result;

}


DAMA_Result_t *	dapi_db_query (DAMA_DB_t *db, const HCHAR *name, const HCHAR *query)
{
	DAMA_Result_t	*result;

	HxLOG_Trace();
	HxLOG_Debug("%s(%s, %s)\n", __FUNCTION__, name, query);

	result = dapi_db_queryData(db, name, query, FALSE);

	return result;
}

static	DERROR	dapi_db_openDatabase(DAMA_DB_t *db)
{
	HINT32	rc;
	if (db->pMainDB)
	{
		HxLOG_Critical("DAMA-DB is already opened!!\n");
		return DERR_FAIL;
	}

	rc = sqlite3_open(":memory:", &db->pMainDB);
//	rc = sqlite3_open(DAMA_MAIN_DB, &db->pMainDB);
	if (rc != SQLITE_OK)
	{
		HxLOG_Critical("DAMA_DB : Fail to create database named [%s]\n", ":memory:");
		return DERR_FAIL;
	}
#ifdef	DAMA_CONFIG_VACUUM
	// INCREMENTAL VACUUM
	if(dapi_db_sqlite_exec(db->pMainDB, "PRAGMA auto_vacuum = 2;", NULL, NULL) != SQLITE_OK) return DERR_FAIL;
#endif

	if(dapi_db_sqlite_exec(db->pMainDB, "PRAGMA cache_size = 6000;", NULL, NULL) != SQLITE_OK) return DERR_FAIL;
	//if(dapi_db_sqlite_exec(db->pMainDB, "PRAGMA synchronous=OFF", NULL, NULL) != SQLITE_OK) return DERR_FAIL;
	if(dapi_db_sqlite_exec(db->pMainDB, "PRAGMA count_changes=OFF", NULL, NULL) != SQLITE_OK) return DERR_FAIL;
	//if(dapi_db_sqlite_exec(db->pMainDB, "PRAGMA journal_mode=OFF", NULL, NULL) != SQLITE_OK) return DERR_FAIL;

	//sqlite3_limit(db->pMainDB,SQLITE_LIMIT_ATTACHED,20);


	return DERR_OK;
}

static DAMA_DB_t *		dapi_db_open (const HCHAR *path, const HCHAR *backupPath, const HCHAR *tmpPath)
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
			HLIB_DIR_Make(db->szPath);
		if (access(db->szBackupPath, R_OK | W_OK) != 0)
			HLIB_DIR_Make(db->szBackupPath);

		db->nSqlBuf   = HLIB_RWSTREAM_Open(NULL, 8192);
		db->pTableMap = HLIB_HASH_New((_HxHashFunc)HLIB_HASH_GetStringKey, (_HxEqualFunc)HLIB_HASH_GetStringKey);

		DAMA_DEBUG(
			db->pTableMonitor = HLIB_TREE_NewFull((HxCompareDataFunc)HLIB_STD_StrCaseCmp, NULL, NULL, HLIB_STD_MemFree_CB);
		);
		dapi_db_openDatabase(db);
	}
	return db;
}

static const DAMA_Item_t *	dapi_db_nextItem (DAMA_Result_t *result)
{
	HxLOG_Assert(result);

	return (const DAMA_Item_t *)HLIB_VECTOR_ItemAt(result->pList, result->nIndex++);
}

static const HCHAR*	dapi_db_getFieldName(const HCHAR *pszObjectName, HINT32 nIndex)
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
	HUINT32 			 ulLength;
	const HCHAR 		*pszRetString = NULL;
	const DxField_t 	*pstField = NULL;

	pstField = DLIB_CreateGetFieldData(pszObjectName, nIndex, NULL, 0);
	if (NULL == pstField)
	{
		goto END_FUNC;
	}

	ulLength = HxSTD_StrLen(pstField->name);

	pszRetString = (const HCHAR *)DLIB_Malloc(ulLength + 1);
	if (NULL == pszRetString)
	{
		goto END_FUNC;
	}

	HxSTD_MemCopy(pszRetString, pstField->name, ulLength);
	pszRetString[ulLength] = '\0';

END_FUNC:
	if (NULL != pstField)
	{
		DLIB_DestroyGetFieldData(pstField);
	}

	return pszRetString;
}
#endif
}

static	HCHAR*	dapi_db_makeSQLForInsertion(DAMA_DB_t *db, const HCHAR *name, HUID uid)
{
	HINT32	i, nFieldNum;
	HCHAR	*pszSQLInsert;
	HCHAR	*pszReturn;

	nFieldNum = DLIB_GetFieldCount(name);

	HLIB_RWSTREAM_Rewind(db->nSqlBuf);
	HLIB_RWSTREAM_Print(db->nSqlBuf, "INSERT INTO %s ('uid'", name);
	for (i = 0; i < nFieldNum; i++)
	{
		HLIB_RWSTREAM_Print(db->nSqlBuf, ", '%s'", name);
	}
	HLIB_RWSTREAM_Print(db->nSqlBuf, ", link");
	HLIB_RWSTREAM_Print(db->nSqlBuf, ", linkdata) VALUES(%"HUID_FMT);

	for (i = 0; i < nFieldNum; i++)
	{
		HLIB_RWSTREAM_Print(db->nSqlBuf, ", ?");
	}
	HLIB_RWSTREAM_Print(db->nSqlBuf, ", ?, ?);");

	pszSQLInsert = HLIB_RWSTREAM_GetBuf(db->nSqlBuf);

	pszReturn = dapi_db_getTempBuf(1024);
	HxSTD_snprintf(pszReturn, 1023, pszSQLInsert, uid);

	HxLOG_Debug("[%s]\n", pszReturn);

	return pszReturn;
}

static	HCHAR*	dapi_db_makeSQLForSelection(DAMA_DB_t *db, const HCHAR *name, HUID uid)
{
	HCHAR	*pszReturn;
	HCHAR	*pszSQLFind;

	HLIB_RWSTREAM_Rewind(db->nSqlBuf);
	HLIB_RWSTREAM_Print(db->nSqlBuf, "SELECT link, linkdata FROM %s WHERE uid=%"HUID_FMT";", name);
	pszSQLFind = HLIB_RWSTREAM_GetBuf(db->nSqlBuf);
	pszReturn  = dapi_db_getTempBuf(1024);
	HxSTD_snprintf(pszReturn, 1023, pszSQLFind, uid);

	HxLOG_Debug("[%s]\n", pszReturn);

	return pszReturn;
}

static	HCHAR*	dapi_db_makeSQLForUpdate(DAMA_DB_t *db, const HCHAR *name, HUID uid)
{
	HINT32	i, nFieldNum;
	HCHAR	*pszReturn;
	HCHAR	*pszSQLUpdate;

	nFieldNum = DLIB_GetFieldCount(name);

	HLIB_RWSTREAM_Rewind(db->nSqlBuf);
	HLIB_RWSTREAM_Print(db->nSqlBuf, "UPDATE %s SET ", name);
	for (i = 0; i < nFieldNum; i++)
	{
#if !defined(CONFIG_TEMP_DLIB_NO_AUTOMATIC_TRASHING)
		HLIB_RWSTREAM_Print(db->nSqlBuf, "%s=?, ", dapi_db_getFieldName(name, i));
#else
		const HCHAR			*field;

		field = dapi_db_getFieldName(name, i);
		if (NULL != field)
		{
			HLIB_RWSTREAM_Print(db->nSqlBuf, "%s=?, ", field);
			DLIB_Free(field);
		}
#endif
	}
	HLIB_RWSTREAM_Print(db->nSqlBuf, " link=?, linkdata=? WHERE uid=%"HUID_FMT";");

	pszSQLUpdate = HLIB_RWSTREAM_GetBuf(db->nSqlBuf);
	pszReturn    = dapi_db_getTempBuf(1024);
	HxSTD_snprintf(pszReturn, 1023, pszSQLUpdate, uid);

	HxLOG_Debug("pszSQLUpdate : [%s]\n", pszReturn);
	return pszReturn;
}


static	HCHAR*	dapi_db_getLink(DAMA_DB_t *db, const HCHAR *name, HUID uid, const void *src, HUINT32 srcSize)
{
	const HCHAR	*linkInfo;
	HCHAR	*buf = NULL;
	HCHAR	nameBuf[128]="";

	linkInfo = DLIB_GetDataPath(name);
	if (linkInfo == NULL)
		return NULL;				//	using BLOB field.

	if(DLIB_GetLinkName(name, uid,src,srcSize,nameBuf,128) == TRUE)
	{
		buf = dapi_db_getTempBuf(256);
		HxSTD_snprintf(buf, 255, "%s", nameBuf);
	}
	else
	{
		HxSTD_snprintf(buf, 255, "%s/"HUID_FMT".file", linkInfo, uid);
	}
	return buf;
}

static	DERROR	dapi_db_updateLink(DAMA_DB_t *db, const HCHAR *name, HUID uid, const HCHAR *pszLink, HINT32 size, const void *pvData)
{
	return DLIB_WriteData(name, uid, pszLink, pvData, (HUINT32)size);
}


static	DERROR	dapi_db_addData(DAMA_DB_t *db, const HCHAR *name, HUID uid, HINT32 size, const void *data)
{

	HINT32	rst, i;
	HINT32	nFieldNum, nParamNo;
	HCHAR	*pszSQLInsert;
	HCHAR	*pszLinkInfo;
	HCHAR	*pszSQLFind, *pszSQLUpdate;
	HCHAR	*pszSQL;
	HINT32	nEncodedSize = 0;
	void	*pvEncodedData = NULL;
	HBOOL	bUpdateFlag;
	DAMA_Table_t	*pstTable;

	sqlite3_stmt	*pstStmt;

#if defined(CONFIG_TEMP_DLIB_NO_AUTOMATIC_TRASHING)
	HINT32			 nCreatedEncSize = 0;
	void			*pvCreatedEncData = NULL;
#endif
	DERROR			 eRetValue = DERR_FAIL;

	HxLOG_Debug("name(%s) uid("HUID_FMT") size(%d)\n", name, uid, size);

	pstTable = dapi_db_getTable(db, name);

#ifdef	CACHE_USE_HASHTABLE
	if (pstTable && pstTable->bUseInternalCache && pstTable->pstCacheData)
	{
		dama_cache_t	*pstCache = (dama_cache_t*)HLIB_HASH_Lookup(pstTable->pstCacheData, (void*)(HUINT32)uid);

		if (pstCache && pstCache->size == size)
		{
			if (HxSTD_MemCmp(pstCache->data, data, size) == 0)
			{
				HxLOG_Debug("Same data is coming..\n");

				eRetValue = DERR_OK;
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

	pszLinkInfo = dapi_db_getLink(db, name, uid, data, size);
	//	Finish making link/linkdata

	nFieldNum = DLIB_GetFieldCount(name);

	pszSQLInsert = dapi_db_makeSQLForInsertion(db, name, uid);
	pszSQLFind   = dapi_db_makeSQLForSelection(db, name, uid);
	pszSQLUpdate = dapi_db_makeSQLForUpdate(db, name, uid);

#ifdef	CACHE_USE_HASHTABLE
	if (pstTable && pstTable->bUseInternalCache && pstTable->pstCacheData)
	{
		//	Cache Compare, bUpdateFlag is already set
	} else
#endif
	{
		rst = sqlite3_prepare_v2(db->pMainDB, pszSQLFind, HxSTD_StrLen(pszSQLFind), &pstStmt, NULL);
		if (rst != SQLITE_OK)
		{
			DAMA_ERROR("[%s] prepare failed: %s", name, pszSQLFind);
			eRetValue = DERR_FAIL;
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

						eRetValue = DERR_OK;
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
		pszSQL = pszSQLUpdate;
	else
		pszSQL = pszSQLInsert;

	rst = sqlite3_prepare_v2(db->pMainDB, pszSQL, HxSTD_StrLen(pszSQL), &pstStmt, NULL);
	if (rst != SQLITE_OK)
	{
		DAMA_ERROR("[%s] prepare failed: %s", name, pszSQL);
		eRetValue = DERR_FAIL;
		goto END_FUNC;
	}


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

#if defined(CONFIG_TEMP_DLIB_NO_AUTOMATIC_TRASHING)
		DLIB_DestroyGetFieldData(field);
#endif
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

	sqlite3_step(pstStmt);
	sqlite3_finalize(pstStmt);

	if (pszLinkInfo)
		dapi_db_updateLink(db, name, uid, pszLinkInfo, nEncodedSize, pvEncodedData);

	eRetValue = DERR_DATACHANGED;

END_FUNC:
#if defined(CONFIG_TEMP_DLIB_NO_AUTOMATIC_TRASHING)
	if (NULL != pvCreatedEncData)
	{
		DLIB_DestroyEncodedData((const void *)pvCreatedEncData);
	}
#endif
	return eRetValue;
}


DERROR			dapi_db_set (DAMA_DB_t *db, const HCHAR *name, HUID uid, HINT32 size, const void *data)
{
	DERROR	err;
	DAMA_Table_t	*pstTableInfo;

	HxLOG_Trace();
	HxLOG_Assert(name);

	err = dapi_db_addData(db, name, uid, size, data);
	pstTableInfo = dapi_db_getTable(db, name);
	if (pstTableInfo && err == DERR_DATACHANGED)
	{
		pstTableInfo->bUpdated = TRUE;
		pstTableInfo->ulLastSyncTime	= HLIB_STD_GetSystemTick();
	}

	return err;
}

static DERROR	dapi_command_set (DAMA_DB_t *db, const DAMA_Header_t *header, const void *data)
{
	DERROR			err;

	HxLOG_Trace();


	err = dapi_db_set(db, header->name, header->uid, header->size, data);
	switch (err)
	{
		case DERR_OK:
			break;
		case DERR_FAIL:
			break;
		case DERR_DATACHANGED:
			//dama_add_to_notification_q(dama, header->name, header->uid, client_handle);
			break;
		default:
			break;
	}

	return err;
}

static HBOOL	dapi_db_eachDaMaObject (const HCHAR *objectName, const HCHAR *tags[], void *userdata)
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

static void	dapi_db_forEachObject (HBOOL (*func)(const HCHAR *objectName, const HCHAR *tags[], void *userdata), void *userdata)
{
	DAMA_ObjIter_t	iter;

	iter.func = func;
	iter.userdata = userdata;

	DLIB_ForEachObject(dapi_db_eachDaMaObject, &iter);
}

static HBOOL	dapi_db_isIndexable(const HCHAR *pszObjectName, HINT32 nIndex)
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

static const HCHAR*	dapi_db_getFieldTypeString(const HCHAR *pszObjectName, HINT32 nIndex)
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

static const HCHAR *	dapi_db_makeSQLForTableCreation(DAMA_DB_t *db, const HCHAR *name, HBOOL overwrite, HBOOL index)
{
	HINT32	i, n;
	const HCHAR *field;

	n = DLIB_GetFieldCount(name);
	HLIB_RWSTREAM_Rewind(db->nSqlBuf);
	if (overwrite)
	{
		for (i = 0; i < n; i++)
		{
			if (dapi_db_isIndexable(name, i))
			{
#if !defined(CONFIG_TEMP_DLIB_NO_AUTOMATIC_TRASHING)
				field = dapi_db_getFieldName(name, i);
				HLIB_RWSTREAM_Print(db->nSqlBuf, "DROP INDEX IF EXISTS %s_%s;", name, field);
#else
				field = dapi_db_getFieldName(name, i);
				if (NULL != field)
				{
					HLIB_RWSTREAM_Print(db->nSqlBuf, "DROP INDEX IF EXISTS %s_%s;", name, field);
					DLIB_Free(field);
				}
#endif
			}
		}
		HLIB_RWSTREAM_Print (db->nSqlBuf, "DROP TABLE IF EXISTS %s;", name);
	}

	HLIB_RWSTREAM_Print(db->nSqlBuf, "CREATE TABLE IF NOT EXISTS %s (", name);
	HLIB_RWSTREAM_Print(db->nSqlBuf, "[uid] integer(%d) PRIMARY KEY UNIQUE", sizeof(HUID));

	//	Get Keyword list from module so.
	for (i = 0; i < n; i++)
	{
#if !defined(CONFIG_TEMP_DLIB_NO_AUTOMATIC_TRASHING)
		HLIB_RWSTREAM_Print(db->nSqlBuf, ", [%s] %s", dapi_db_getFieldName(name, i), dapi_db_getFieldTypeString(name, i));
#else
		field = dapi_db_getFieldName(name, i);
		if (NULL != field)
		{
			HLIB_RWSTREAM_Print(db->nSqlBuf, ", [%s] %s", field, dapi_db_getFieldTypeString(name, i));
			DLIB_Free(field);
		}
#endif
	}

	HLIB_RWSTREAM_Print(db->nSqlBuf, ", [link] TEXT");
	HLIB_RWSTREAM_Print(db->nSqlBuf, ", [linkdata] BLOB");
	HLIB_RWSTREAM_Print(db->nSqlBuf, ");");
#if 1
	if (index)
	{
		for (i = 0; i < n; i++)
		{
			if (dapi_db_isIndexable(name, i))
			{
#if !defined(CONFIG_TEMP_DLIB_NO_AUTOMATIC_TRASHING)
				field = dapi_db_getFieldName(name, i);
				HLIB_RWSTREAM_Print(db->nSqlBuf, "CREATE INDEX IF NOT EXISTS %s_%s ON %s(%s ASC);", name, field, name, field);
#else
				field = dapi_db_getFieldName(name, i);
				if (NULL != field)
				{
					HLIB_RWSTREAM_Print(db->nSqlBuf, "CREATE INDEX IF NOT EXISTS %s_%s ON %s(%s ASC);", name, field, name, field);
					DLIB_Free(field);
				}
#endif
			}
		}
	}
#endif
	return HLIB_RWSTREAM_GetBuf(db->nSqlBuf);
}

static	DERROR	dapi_db_attachTableInner(DAMA_DB_t *db, const HCHAR *objName)
{
	HCHAR			*fullName;
	DAMA_Table_t	*table;
#if 0
	const HCHAR		*sql;
	HINT32			rst;
#endif

	//	Make table in memory DB with given parameters
	fullName = (HCHAR*)dapi_db_getDbPath(db, objName, 0);
	if (HLIB_STD_StrCaseCmp(fullName, ":memory:") != 0)
	{

		const HCHAR *tmpPath =	dapi_db_getDbPath(db, objName, -1); 	// /tmp/

		if(HLIB_FILE_Exist(tmpPath))
		{
			;
		}
		else
		{
			HxLOG_Debug("not yet exists \n");
			return DERR_FAIL;
		}

		if (dapi_db_loadFileDB(db, objName, tmpPath) != DERR_OK)
		{
			HxLOG_Debug("cannot load \n");
			return DERR_FAIL;
		}


	}
	else
	{
		// TODO: memory db 를 각자 다른 process에서..각기 다른 handle로...가능 ??
		return DERR_FAIL;
#if 0
		if ((sql = dapi_db_makeSQLForTableCreation(db, objName, FALSE, TRUE)) == NULL)
		{
			HxLOG_Critical("SQL Creation FAIL!!! Abort!\n");
			HxLOG_Assert(0);
		}

		HxLOG_Debug("SQL [%s]\n", sql);
		rst = dapi_db_sqlite_exec(db->pMainDB, sql, NULL, 0);
		if (rst != SQLITE_OK)
		{
			HxLOG_Critical("Fail to create database\n");
			return DERR_FAIL;
		}
		DAMA_TRACE("[%s] created on memory", objName);
#endif
	}

	table = (DAMA_Table_t*)HLIB_STD_MemCalloc(sizeof(DAMA_Table_t));
	if (table == NULL)
	{
		HxLOG_Critical("%s() Out of momory!\n", __FUNCTION__);
		return DERR_FAIL;
	}

	HxSTD_StrNCpy(table->szObjName, objName, DxLIB_MAX_NAME-1);
	table->ulSyncCycle = (DLIB_GetSyncTime(objName)*1000);
	table->ulLastSyncTime  = HLIB_STD_GetSystemTick();


	table->bUseCodec = DLIB_HasTag(objName, "BLOB") ? FALSE : TRUE;
	table->bUseInternalCache = DLIB_HasTag(objName, "CACHE") ? TRUE : FALSE;


	HLIB_HASH_Insert(db->pTableMap, HLIB_STD_StrDup(objName), (void*)table);

	if (table->bUseInternalCache)
	{
		//	TODO: 64Bit지원을 어케 하지???
		table->pstCacheData = HLIB_HASH_NewFull(HLIB_HASH_GetIntKey, HLIB_HASH_IsIntEqual, NULL, HLIB_STD_MemFree_CB);

	}

	return DERR_OK;
}


static DERROR	dapi_db_makeNewDb (DAMA_DB_t *db, const HCHAR *objName, const  const HCHAR *fileName)
{
	sqlite3		*fileDb;
	int			ret;
	const HCHAR *sql;
	const HCHAR	*tabName;
	HCHAR	buf[HxPATH_MAX], *dir;

	HxLOG_Debug("fileName: %s, objName: %s \n", fileName, objName);

	if (!HLIB_DIR_IsMountedPath(fileName) || HLIB_FILE_Exist(fileName))
	{
		HxLOG_Warning("There is DB file or Not mounted[%s].\n", fileName);
		return DERR_FAIL;
	}

	dir = HLIB_PATH_DirName(fileName, buf, HxPATH_MAX);
	if (dir && !HLIB_FILE_Exist(dir))
	{
		HxLOG_Debug("make dir %s \n", dir);
		HLIB_DIR_Make(dir);
	}

	ret = sqlite3_open(fileName, &fileDb);
	if (ret != SQLITE_OK)
	{
		DAMA_ERROR("[%s] Load file DB: Can't create file DB", objName);
		return DERR_FAIL;
	}

	HxLOG_Debug("tabName: %s \n", objName);

	sql = dapi_db_makeSQLForTableCreation(db, objName, TRUE, TRUE);
	HxLOG_Debug("sql: %s \n", sql);

	ret = dapi_db_sqlite_exec(fileDb, sql, NULL, 0);

	sqlite3_close(fileDb);
	if (ret != SQLITE_OK)
	{
		HxLOG_Debug("cannot create db! ret: %d \n", ret);
		return DERR_FAIL;
	}
	return DERR_OK;
}

static	DERROR	dapi_db_createTable(DAMA_DB_t *db, const HCHAR *objName)
{
//	HINT32			rst;
	HCHAR			*fullName;
	DAMA_Table_t	*table;
//	const HCHAR		*sql;

	HxLOG_Trace();

	table = (DAMA_Table_t*)HLIB_STD_MemCalloc(sizeof(DAMA_Table_t));
	if (table == NULL)
	{
		HxLOG_Critical("%s() Out of momory!\n", __FUNCTION__);
		return DERR_FAIL;
	}

	HxSTD_StrNCpy(table->szObjName, objName, DxLIB_MAX_NAME-1);
	table->ulSyncCycle = (DLIB_GetSyncTime(objName)*1000);
	table->ulLastSyncTime  = HLIB_STD_GetSystemTick();

	//	Make table in memory DB with given parameters
	fullName = (HCHAR*)dapi_db_getDbPath(db, objName, 0);
	HxLOG_Debug("fullName: %s \n", fullName);
	if (HLIB_STD_StrCaseCmp(fullName, ":memory:") != 0)
	{
//		HINT32	backupIndex;
//		const HCHAR	*path;
		const HCHAR *tmpPath =	dapi_db_getDbPath(db, objName, -1); 	// /tmp/


		HxLOG_Debug("tmpPath: %s \n", tmpPath);
		if (!HLIB_FILE_Exist(tmpPath) && HLIB_DIR_IsMountedPath(fullName))
		// All File Deleted. DB is loaded. but tmpPath is not exist. have to make new Default DB.
		{
			HxLOG_Warning("All DB Deleted[%s].\n", tmpPath);
			dapi_db_makeNewDb (db, objName, tmpPath);
		}

		if(dapi_db_loadFileDB(db, objName, tmpPath) != DERR_OK)
		{
			HxLOG_Warning("Make New DB But Loading Failed[%s].\n", tmpPath);
		}


	}


	table->bUseCodec = DLIB_HasTag(objName, "BLOB") ? FALSE : TRUE;
	table->bUseInternalCache = DLIB_HasTag(objName, "CACHE") ? TRUE : FALSE;

	HLIB_HASH_Insert(db->pTableMap, HLIB_STD_StrDup(objName), (void*)table);

	if (table->bUseInternalCache)
	{
		//	TODO: 64Bit지원을 어케 하지???
		table->pstCacheData = HLIB_HASH_NewFull(HLIB_HASH_GetIntKey, HLIB_HASH_IsIntEqual, NULL, HLIB_STD_MemFree_CB);

		//	TODO: add Preload when internal cache is enabled.
		//dama_db_queryData(db, objName, "SELECT * from DxService_t", TRUE);
	}

	return DERR_OK;
}

static DERROR			dapi_db_attachTable (DAMA_DB_t *db, const HCHAR *name, HUID uid)
{
	DAMA_Table_t	*table;
	DERROR			err;

	HxLOG_Trace();
	err = DERR_FAIL;
	if (!HxSTD_StrEmpty(name))
	{
		table = dapi_db_getTable(db, name);
		if (table == NULL)
		{
//			return dapi_db_createTable(db, name);
			return dapi_db_attachTableInner(db, name);
		}
		else
		{
			err = DERR_OK;
		}
	}
	return err;
}

static HBOOL	dapi_db_deleteFiles(const HCHAR *objName, const HCHAR *tags[], void *userdata)
{
	DAMA_DB_t	*db;
	DAMA_Table_t 	*table;

	HxLOG_Warning("[%s] dapi_db_deleteFiles", objName);

	db = (DAMA_DB_t *)userdata;
#if 0 // TODO:
	dapi_db_deleteLink(objName, DAMA_TABLE_RESET_UID, NULL);
#endif
	table = dapi_db_getTable(db, objName);
	if (table)
	{
		const HCHAR		*sql;

		if (table->bUseInternalCache && table->pstCacheData)
			HLIB_HASH_RemoveAll(table->pstCacheData);

		sql = dapi_db_makeSQL(db, "DETACH %s;", objName);
		if(dapi_db_sqlite_exec(db->pMainDB, sql, NULL, 0) != SQLITE_OK)
		{
			HxLOG_Error("DETACH FAIL [%s]\n", objName);
		}
		table->bManualUpdate = TRUE;
		table->bUpdated = TRUE;
	}


//	for (index = -1 ; (path = dapi_db_getDbPath(db, objName, index)) != NULL ; index++)
	{
		const HCHAR *tmpPath =	dapi_db_getDbPath(db, objName, -1); 	// /tmp/

		HxLOG_Warning("[%s] file(%s) removed\n", objName, tmpPath);
		HxLOG_Warning("delete db %s\n", tmpPath);
		HLIB_FILE_Delete(tmpPath);
	}

	HLIB_HASH_Remove(db->pTableMap , objName);
	dapi_db_createTable(db, objName);

	return FALSE;
}

static	DERROR	dapi_db_deleteTable(DAMA_DB_t *db, const HCHAR *name)
{
	if (name == NULL)
	{
		HxLOG_Warning("dapi_db_deleteTable ALL\n");
		dapi_db_forEachObject(dapi_db_deleteFiles, (void *)db);
	}
	else
	{
		HxLOG_Print("dapi_db_deleteTable [%s]\n", name);
		dapi_db_deleteFiles(name, NULL, (void*)db);
	}
	return DERR_OK;
}

static	DERROR	dapi_db_removeItem(DAMA_DB_t *db, const HCHAR *name, HUID uid)
{
	const HCHAR		*sql;
	DAMA_Table_t	*table;

#if 0// TODO:
	dama_db_deleteLinkByUid(db, name, uid);
#endif

	sql = dapi_db_makeSQL(db,
			"DELETE FROM %s WHERE uid="HUID_FMT
			, name
			, uid
		);
	if (dapi_db_sqlite_exec(db->pMainDB, sql, NULL, 0) != SQLITE_OK)
	{
		DAMA_ERROR("[%s] RemoveItem FAILED", name);
		return DERR_FAIL;
	}
	table = dapi_db_getTable(db, name);
	if (table)
	{
		table->bUpdated = TRUE;
		//	TODO: 64bit는 어케 지원??
#ifdef CACHE_USE_HASHTABLE
		if (table && table->bUseInternalCache && table->pstCacheData)
			HLIB_HASH_Remove(table->pstCacheData, (void*)(HUINT32)uid);
#endif
	}

	DAMA_TRACE("[%s] RemoveItem(%lld) SUCCESS", name, uid);
	return DERR_OK;
}

static void			dapi_db_remove (DAMA_DB_t *db, const HCHAR *name, HUID uid)
{
	HxLOG_Trace();
	HxLOG_Debug("%s(%s, "HUID_FMT")\n", __FUNCTION__, name, uid);
	if (name == NULL)
	{
		//	remove all.
		dapi_db_deleteTable(db, NULL);
		DAMA_TRACE("Remove all!!!");
		return;
	}

	if (uid == DAMA_TABLE_RESET_UID)
	{
		dapi_db_deleteTable(db, name);
		DAMA_TRACE("[%s] Remove table", name);
	}
	else
	{
		dapi_db_removeItem(db, name, uid);
	}
}

static DERROR	dapi_command_reset (DAMA_DB_t *db, const DAMA_Header_t *header)
{
	HxLOG_Trace();

	if (header->name[0] == '\0')
	{
		HxLOG_Debug("%s(%s) reset all\n", __FUNCTION__, header->name);
		dapi_db_remove(db, NULL, 0);
		return DERR_DATACHANGED;
	}
	dapi_db_remove(db, header->name, header->uid);
	//dama_add_to_notification_q(dama, header->name, header->uid, client_handle);

	return DERR_DATACHANGED;

}

static DERROR	dapi_sendDataChangedEventToDama (dapi_t *dapi, const HCHAR *name, const void *uidData, HUINT32 nUidCnt)
{
	DAMA_Header_t	header;

	header.cmd   = eDAMA_CMD_DATACHANGED;
	header.requestId = 0;
	if (name)
		HLIB_STD_StrNCpySafe(header.name, name, DAMA_OBJECTNAME_LEN);
	else
		HxSTD_StrNCpy(header.name, "", DAMA_OBJECTNAME_LEN);
	header.uid  = 0;
	header.size = (nUidCnt * sizeof(HUID));


	HLIB_RWSTREAM_Rewind(dapi->send_stream);
	HLIB_RWSTREAM_Write(dapi->send_stream, &header, sizeof(DAMA_Header_t));

	if (header.size > 0 && uidData)
	{
		HLIB_RWSTREAM_Write(dapi->send_stream, (void *)uidData, header.size);
	}


	return dapi_send(dapi, HLIB_RWSTREAM_GetBuf(dapi->send_stream), (HINT32)HLIB_RWSTREAM_GetOffset(dapi->send_stream), header.requestId);
}

HBOOL			dapi_db_autoCommit(DAMA_DB_t *db, HBOOL enable)
{
	HxLOG_Trace();

	HxLOG_Debug("enable: %d \n", enable);
	if (enable)
		dapi_db_sqlite_exec(db->pMainDB, "COMMIT", NULL, NULL);
	else
		dapi_db_sqlite_exec(db->pMainDB, "BEGIN", NULL, NULL);

	return sqlite3_get_autocommit(db->pMainDB) ? TRUE : FALSE;
}

void DAPI_PrintMemUsed(void)
{
	HxLOG_Warning("sqlite3 mem used: %d \n", sqlite3_memory_used());
	HxLOG_Warning("sqlite3 mem highwater: %d \n", sqlite3_memory_highwater(0));
}
#endif


static void	dapi_dummy_getter (const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata)
{
	(void)name;
	(void)uid;
	(void)data;
	(void)size;
	HxLOG_Message("%s() \'%s\' done!\n", __FUNCTION__, (char *)userdata);
}

static HINT32	dapi_compare_monitor (const dapi_monitor_t *lhs, const dapi_monitor_t *rhs)
{
	HINT32	diff;

	diff = HxSTD_StrCmp(lhs->name, rhs->name);
	if (diff == 0)
		diff = (HINT32)lhs->callback - (HINT32)rhs->callback;
	return diff;
}

static HBOOL	dapi_verify_query (dapi_t *dapi, const HCHAR *query)
{
	HCHAR  *buf;
	HCHAR  *token, *save;
	HUINT32 len;
	HBOOL   uid  = FALSE;
	HBOOL   data = FALSE;
	HBOOL   name = FALSE;
#define	VERIFY_DELIM	", \t\n\r"
	while (isspace(*query))
		query++;

	if (!HLIB_STD_StrCaseStartWith(query, "SELECT"))
		return TRUE;// check only 'select' query

	len = strlen(query);
	if (len >= DAPI_CIRCULARBUF_SIZE)
		return TRUE;

	buf = HLIB_CBUF_GetBuffer(dapi->buf, (HINT32)len + 1);
	DAMA_MemCpy(buf, query, len);
	buf[len] = '\0';

	(void)  HxSTD_strtok_r(buf , VERIFY_DELIM, &save);
	token = HxSTD_strtok_r(NULL, VERIFY_DELIM, &save);
	while (token && HLIB_STD_StrCaseCmp(token, "FROM") != 0)
	{
		if (strcmp(token, "*") == 0 || HLIB_STD_StrCaseCmp(token, "ALL") == 0)
		{
			uid  = TRUE;
			data = TRUE;
		}
		else if (strcmp(token, "uid") == 0)
			uid  = TRUE;
		else if (strcmp(token, "link") == 0 || strcmp(token, "linkdata") == 0)
			data = TRUE;
		else if (strcmp(token, "tablename") == 0)
			name = TRUE;

		token = HxSTD_strtok_r(NULL, VERIFY_DELIM, &save);
	}

	if (!uid || !data)
	{
		HxLOG_Warning(
			"<%s:%d> VERIFY FAIL!! \"%s\"\n"
			"\t- The DAPI \'SELECT\' expression need result-column 'uid', 'link' or 'linkdata'.\n"
			"\t- here's an example\n"
			"\t-     SELECT * FROM DxService_t\n"
			"\t-     SELECT uid, linkdata FROM DxService_t\n"
			"\n\n"
			, __FUNCTION__, __LINE__
			, query
		);
		return FALSE;
	}

	if (token)
	{
		token = HxSTD_strtok_r(NULL, VERIFY_DELIM, &save);
		while (token)
		{
			if (HLIB_STD_StrCaseCmp(token, "UNION") == 0)
			{
				if (!name)
				{
					HxLOG_Warning(
						"<%s:%d> VERIFY FAIL!!! \"%s\"\n"
						"\t- sqlite3 doesn't support function \'sqlite3_column_table_name()\' with \'UNION\'.\n"
						"\t- here's an example, how to use sql with \'UNION\'\n"
						"\t-     SELECT 'eDxEPG_TYPE_PF' AS tablename, * FROM eDxEPG_TYPE_PF\n"
						"\t      UNION\n"
						"\t      SELECT 'eDxEPG_TYPE_SC' AS tablename, * FROM eDxEPG_TYPE_SC\n"
						"\n\n"
						, __FUNCTION__, __LINE__
						, query
					);
					return FALSE;
				}
			}
			token = HxSTD_strtok_r(NULL, VERIFY_DELIM, &save);
		}
	}
	return TRUE;
}

static HCHAR *	dapi_make_sql (dapi_t *dapi, const HCHAR *fmt, ...)
{
	va_list	ap;
	HCHAR	*query;

	query = HLIB_CBUF_GetBuffer(dapi->buf, 1024);
	va_start(ap, fmt);
	HxSTD_PrintToStrV(query, 1024, fmt, ap);
	va_end(ap);

	return query;
}

static HUINT32			dapi_next_request_id (dapi_t *dapi)
{
	return (HUINT32)((dapi->pid16 << 16) | (++dapi->idseq));
}


static dapi_request_t *	dapi_alloc_request (dapi_t *dapi, DAPI_Getter callback, void *userdata, const HCHAR *name, const HCHAR *query)
{
	dapi_request_t *req;

	if (dapi->freed)
	{
		req = dapi->freed;
		dapi->freed = req->next;
		DAMA_MemSet(req, 0, sizeof(dapi_request_t));
	}
	else req = (dapi_request_t *)DAMA_Calloc(sizeof(dapi_request_t));

	if (req)
	{
		req->callback = callback;
		req->userdata = userdata;
		if (name)
			HLIB_STD_StrNCpySafe(req->name, name, DAMA_OBJECTNAME_LEN);
		DAPI_DEBUG_EXPR(
			if (query)
				HLIB_STD_StrNCpySafe(req->query, query, DAMA_OBJECTNAME_LEN);
			req->tick = HLIB_STD_GetSystemTick();
		);
	}
	return req;
}

static void		dapi_free_request (dapi_t *dapi, dapi_request_t *req)
{
	if (req)
	{
		req->next = dapi->freed;
		dapi->freed = req;
	}
}

static dapi_request_t *	dapi_find_request (dapi_request_t **head, HUINT32 req_id)
{
	dapi_request_t *req;
	req  = *head;
	while (req)
	{
		if (req->id == req_id)
		{
			return req;
		}
		req  = req->next;
	}
	return NULL;
}

static dapi_request_t *	dapi_find_request_name (dapi_request_t **head, const HCHAR *name)
{
	dapi_request_t *req;
	req  = *head;
	while (req)
	{
		if (strcmp(req->name, name) == 0)
		{
			return req;
		}
		req  = req->next;
	}
	return NULL;
}

static dapi_request_t *	dapi_pop_request (dapi_request_t **head, HUINT32 req_id)
{
	dapi_request_t *req, *prev;

	DAPI_DEBUG_EXPR(
		HUINT32	tick = HLIB_STD_GetSystemTick();
		for (req = *head ; req; req = req->next)
		{
			if (tick - req->tick > 3000)
				HxLOG_Warning("\'%s:%s\' Not response.. during %u tick\n", req->name, req->query, tick - req->tick);
		}
	);

	prev = NULL;
	req  = *head;
	while (req)
	{
		if (req->id == req_id)
		{
			if (prev)
				prev->next = req->next;
			else
				*head = req->next;
			req->next = NULL;
			return req;
		}
		prev = req;
		req  = req->next;
	}
	return NULL;
}


static dapi_batch_t *	dapi_batch_find (dapi_t *dapi, const HCHAR *name, HBOOL pop)
{
	dapi_batch_t *batch, *prev;

	if (HxSTD_StrEmpty(name))
		return NULL;

	prev  = NULL;
	batch = dapi->batch;
	while (batch)
	{
		if (strcmp(batch->name, name) == 0)
		{
			if (pop)
			{
				if (prev == NULL)
					dapi->batch = batch->next;
				else
					prev->next = batch->next;
				batch->next = NULL;
			}
			return batch;
		}
		prev  = batch;
		batch = batch->next;
	}
	return NULL;
}

static DERROR	dapi_batch_begin (dapi_t *dapi, const HCHAR *name)
{
	dapi_batch_t	*batch;
	DAMA_Header_t	header;

	if ((batch = dapi_batch_find(dapi, name, FALSE)) != NULL)
	{
		batch->refcount++;
		HxLOG_Debug("%s(%s) is already begun.(ref:%d)\n", __FUNCTION__, name, batch->refcount);
		return DERR_OK;
	}

	batch = (dapi_batch_t *)DAMA_Calloc(sizeof(dapi_batch_t));
	if (batch == NULL)
	{
		HxLOG_Warning("%s(%s) Out of memory!\n", __FUNCTION__, name);
		return DERR_OOM;
	}
	batch->refcount++;

	HLIB_STD_StrNCpySafe(batch->name, name, DAMA_OBJECTNAME_LEN);
	batch->buffer = HLIB_RWSTREAM_OpenEx(NULL, 0, DAMA_Malloc_CB, DAMA_Free_CB);
	if (!batch->buffer)
	{
		DAMA_Free(batch);
		HxLOG_Warning("%s(%s) Out of memory!\n", __FUNCTION__, name);
		return DERR_OOM;
	}
	batch->list = NULL;

	DAMA_MemSet(&header, 0, sizeof(DAMA_Header_t));
	header.cmd = eDAMA_CMD_BATCH;
	HLIB_STD_StrNCpySafe(header.name, name, DAMA_OBJECTNAME_LEN);
	HLIB_RWSTREAM_Write(batch->buffer, &header, sizeof(DAMA_Header_t));

	batch->next = dapi->batch;
	dapi->batch = batch;

	return DERR_OK;
}

#if defined(DAPI_MONITOR_SOCKET_SENDING)
STATIC HUINT32		 s_ulDapi_MaxTick_Tick;
STATIC HUINT32		 s_ulDapi_MaxTick_Size;
STATIC HUINT32		 s_ulDapi_MaxSize_Tick;
STATIC HUINT32		 s_ulDapi_MaxSize_Size;
#endif

static HERROR	dapi_send (dapi_t *dapi, const void *data, HUINT32 n, HUINT32 reqid)
{
#if defined(CONFIG_DEBUG) || defined(DAPI_MONITOR_SOCKET_SENDING)
	HUINT32		 ulTick, ulTick1;
#endif
	HERROR		 hErr;

	if (eReCnctState_None != dapi->recnct_state)
	{
		HxLOG_Error("dapi_send exceeded the fail time...\n");
		return ERR_FAIL;
	}


#if	defined(DAMA_LOGGING) && DAMA_LOGGING == 0
	if (dapi->logfp)
	{
		dama_Logging(dapi->logfp, '>', data, n, reqid);
	}
#endif

#if defined(CONFIG_DEBUG) || defined(DAPI_MONITOR_SOCKET_SENDING)
	ulTick1 = HLIB_STD_GetSystemTick();
#endif
	if (0 == dapi->send_task_qid)
	{
		hErr = HLIB_SOCKET_Write(dapi->socket, data, n, reqid);
	}
	else
	{
		hErr = ERR_FAIL;
		if ((NULL != data) && (n > 0))
		{
			void				*pvDupData = HLIB_STD_MemDup(data, n);

			if (NULL != pvDupData)
			{
				dapi_SendMsg_t		 stMsg;

				stMsg.nSocket = dapi->socket;
				stMsg.pvData = pvDupData;
				stMsg.ulSize = n;
				stMsg.ulReqId = reqid;

				hErr = VK_MSG_SendTimeout(dapi->send_task_qid, (const void *)&stMsg, sizeof(dapi_SendMsg_t), 0);
				if (VK_OK != hErr)
				{
					HLIB_STD_MemFree(pvDupData);
					hErr = ERR_FAIL;
				}
			}
		}
	}


#if defined(CONFIG_DEBUG) || defined(DAPI_MONITOR_SOCKET_SENDING)
	ulTick = HLIB_STD_GetSystemTick() - ulTick1;
#endif

#if defined(DAPI_MONITOR_SOCKET_SENDING)
	if (s_ulDapi_MaxTick_Tick < ulTick)
	{
		s_ulDapi_MaxTick_Tick = ulTick;
		s_ulDapi_MaxTick_Size = n;
	}

	if (s_ulDapi_MaxSize_Size < n)
	{
		s_ulDapi_MaxSize_Tick = ulTick;
		s_ulDapi_MaxSize_Size = n;
	}
#endif

	if (ERR_OK != hErr)
	{
#if defined(CONFIG_DEBUG)
		HxLOG_Critical("HLIB_SOCKET_Write failed with Tick(%d), Size(%d)\n", ulTick, n);
#endif
		dapi->num_socket_failed++;

		if (dapi->num_socket_failed >= DAPI_SEND_FAIL_MAXIMUM)
		{
			s_nDapiBlockedSocket = dapi->socket;
			dapi->recnct_state = eReCnctState_Start;
			dapi->socket_fail_tick = HLIB_STD_GetSystemTick();
		}
	}
	else
	{
		if ((dapi->num_socket_failed > 0) && (dapi->recnct_state == eReCnctState_None))
		{
			dapi->num_socket_failed--;
		}
	}

	return hErr;
}

static void		dapi_recv (dapi_t *dapi, const void *data, HUINT32 n, HUINT32 reqid)
{
#if	defined(DAMA_LOGGING) && DAMA_LOGGING == 0
	dapi_t *release = NULL;

	if (dapi == NULL)
		release = dapi = dapi_get_instance();

	if (dapi->logfp)
	{
		dama_Logging(dapi->logfp, '<', data, n, reqid);
	}

	if (release)
		dapi_release_instance(release);
#else
	(void)dapi;
	(void)data;
	(void)n;
	(void)reqid;
#endif
}

static DERROR	dapi_batch_flush (dapi_t *dapi, dapi_batch_t *batch)
{
	DAMA_Header_t	header;
	HxList_t		*list;
	dapi_list_t		*info;
	DERROR			err;

	HxLOG_Trace();

	list = batch->list;

	if(list)
	{

#if defined(CONFIG_FUNC_USE_DAPI_DIRECT_QUERY)
		info = (dapi_list_t*)HLIB_LIST_Data(list);
		DAMA_MemCpy(&header, info->data, sizeof(DAMA_Header_t));

		err = dapi_db_attachTable(dapi->db, header.name, header.uid);
		if(err == DERR_OK)
		{
			DAMA_Header_t	headerForDirectQuery;
			HINT32			bufferForDirectQuery;
			HINT32			bufferForUids;
			HUINT32 		nChangedCnt = 0;

			bufferForDirectQuery = HLIB_RWSTREAM_OpenEx(NULL, 0, DAMA_Malloc_CB, DAMA_Free_CB);
			if (!bufferForDirectQuery)
			{
				HxLOG_Warning("%s(%s) Out of memory!\n", __FUNCTION__, header.name);
				return DERR_OOM;
			}

			bufferForUids = HLIB_RWSTREAM_OpenEx(NULL, 0, DAMA_Malloc_CB, DAMA_Free_CB);
			if (!bufferForUids)
			{
				HxLOG_Warning("%s(%s) Out of memory!\n", __FUNCTION__, header.name);
				HLIB_RWSTREAM_Close(bufferForDirectQuery);
				return DERR_OOM;
			}

			dapi_db_autoCommit(dapi->db, FALSE);

			while(list)
			{
				info = (dapi_list_t*)HLIB_LIST_Data(list);

				HLIB_RWSTREAM_Rewind(bufferForDirectQuery);

				if(info)
				{
					DAMA_MemCpy(&headerForDirectQuery, info->data, sizeof(DAMA_Header_t));
					HLIB_RWSTREAM_Write(bufferForDirectQuery, info->data + sizeof(DAMA_Header_t), info->size - sizeof(DAMA_Header_t));


					if (headerForDirectQuery.size > 0)
					{
						err = dapi_command_set(dapi->db, &headerForDirectQuery, HLIB_RWSTREAM_GetBuf(bufferForDirectQuery));
					}
					else
					{
						err = dapi_command_reset(dapi->db, &headerForDirectQuery);
					}


					if(err == DERR_DATACHANGED)
					{
						HLIB_RWSTREAM_Write(bufferForUids, &(headerForDirectQuery.uid), sizeof(HUID));
						nChangedCnt++;
					}
				}


				list = list->next;

			}

			dapi_db_autoCommit(dapi->db, TRUE);

			if(nChangedCnt > 0)
			{
				HxLOG_Debug("nChangedCnt: %d \n", nChangedCnt);
				(void)dapi_sendDataChangedEventToDama(dapi,  header.name, HLIB_RWSTREAM_GetBuf(bufferForUids), nChangedCnt);
			}


			batch->list = HLIB_LIST_RemoveAllFunc(batch->list, DAMA_Free_CB);
			HLIB_RWSTREAM_Close(bufferForDirectQuery);
			HLIB_RWSTREAM_Close(bufferForUids);

			HxLOG_Debug("%s end \n", __FUNCTION__);
			return DERR_OK;


		}
		else
#endif
		{


			while (list)
			{
				info = (dapi_list_t*)HLIB_LIST_Data(list);
				if(info)
				{
					HLIB_RWSTREAM_Write(batch->buffer, info->data, info->size);
				}
				list = list->next;
			}
			batch->list = HLIB_LIST_RemoveAllFunc(batch->list, DAMA_Free_CB);
			batch->size = 0;
			if (HLIB_RWSTREAM_GetOffset(batch->buffer) == sizeof(DAMA_Header_t))
			{
				HxLOG_Debug("%s(%s) no data!\n", __FUNCTION__, batch->name);
				return DERR_FAIL;
			}

			HLIB_RWSTREAM_Rewind(batch->buffer);
			if (HLIB_RWSTREAM_Read(batch->buffer, &header, sizeof(DAMA_Header_t)) == 0)
			{
				HxLOG_Debug("%s(%s) no header data!\n", __FUNCTION__, batch->name);
				return DERR_FAIL;
			}

			HLIB_RWSTREAM_Rewind(batch->buffer);
			header.size = HLIB_RWSTREAM_GetWritten(batch->buffer) - sizeof(DAMA_Header_t);
			HLIB_RWSTREAM_Write(batch->buffer, &header, sizeof(DAMA_Header_t));

			err = dapi_send(dapi, HLIB_RWSTREAM_GetBuf(batch->buffer), (HINT32)HLIB_RWSTREAM_GetWritten(batch->buffer), 0);
			HLIB_RWSTREAM_Truncate(batch->buffer);
			header.size = 0;
			HLIB_RWSTREAM_Write(batch->buffer, &header, sizeof(DAMA_Header_t));
			return err;
		}
	}
	else
	{
		HxLOG_Debug("%s(%s) no data!\n", __FUNCTION__, batch->name);
		return DERR_FAIL;
	}
}

static DERROR	dapi_batch_end (dapi_t *dapi, const HCHAR *name)
{
	dapi_batch_t	*batch;
	DERROR			err;

	batch = dapi_batch_find(dapi, name, TRUE);
	if (batch == NULL)
	{
		HxLOG_Error("%s(%s) cannot find batch!\n", __FUNCTION__, name);
		return DERR_FAIL;
	}
	batch->refcount--;

	err = dapi_batch_flush(dapi, batch);

	if (batch->refcount > 0)
	{
		HxLOG_Debug("%s(%s) still batch-ing...\n", __FUNCTION__, name);
		batch->next = dapi->batch;		// reconnect
		dapi->batch = batch;
		return DERR_OK;
	}


	HLIB_RWSTREAM_Close(batch->buffer);
	HLIB_LIST_RemoveAllFunc(batch->list, DAMA_Free_CB);
	DAMA_Free(batch);

	return (DERROR)err;
}

static HBOOL	dapi_batch_compareUid(void *a, void *b)
{
	HUID		uid = *(HUID*)a;
	dapi_list_t	*pInfo = (dapi_list_t*)b;

	if (uid == pInfo->uid)
		return TRUE;

	return FALSE;
}



static DERROR	dapi_set_data (dapi_t *dapi, const HCHAR *name, HUID uid, const void *data, HUINT32 size)
{
	DAMA_Header_t	header;
	dapi_batch_t	*batch;
#if defined(CONFIG_FUNC_USE_DAPI_DIRECT_QUERY)
	DERROR			err;
	void				*uidData;
#endif

	header.cmd   = eDAMA_CMD_SET;
	header.requestId = 0;
	if (name != NULL)
		HLIB_STD_StrNCpySafe(header.name, name, DAMA_OBJECTNAME_LEN);
	else
		HxSTD_StrNCpy(header.name, "", DAMA_OBJECTNAME_LEN);
	header.uid  = uid;
	header.size = size;

	HLIB_RWSTREAM_Rewind(dapi->send_stream);
	HLIB_RWSTREAM_Write(dapi->send_stream, &header, sizeof(DAMA_Header_t));
	if (size > 0 && data)
	{
		HLIB_RWSTREAM_Write(dapi->send_stream, (void *)data, size);
	}

	if ((batch = dapi_batch_find(dapi, name, FALSE)) != NULL)
	{
		HxList_t		*list;
		dapi_list_t	*info;

		list = HLIB_LIST_FindEx(batch->list, (void*)&uid, dapi_batch_compareUid);
		info = HLIB_LIST_Data(list);
		if (list && info && info->size == HLIB_RWSTREAM_GetOffset(dapi->send_stream))
		{
			DAMA_MemCpy(info->data, HLIB_RWSTREAM_GetBuf(dapi->send_stream), info->size);
		} else
		{
			if (info == NULL)
				batch->size++;

			batch->list = HLIB_LIST_RemoveIf(batch->list, (void*)&uid, dapi_batch_compareUid, DAMA_Free_CB);
			info = (dapi_list_t*)DAMA_Malloc(sizeof(dapi_list_t) + HLIB_RWSTREAM_GetOffset(dapi->send_stream));
			info->uid = uid;
			info->size = HLIB_RWSTREAM_GetOffset(dapi->send_stream);
			DAMA_MemCpy(info->data, HLIB_RWSTREAM_GetBuf(dapi->send_stream), info->size);

			batch->list = HLIB_LIST_Append(batch->list, (void*)info);
		}

		if (batch->size > DAMA_BATCH_THRESHOLD_COUNT)
			dapi_batch_flush(dapi, batch);
#if	0
		HLIB_RWSTREAM_Write(batch->buffer, HLIB_RWSTREAM_GetBuf(dapi->send_stream), HLIB_RWSTREAM_GetOffset(dapi->send_stream));
		if (HLIB_RWSTREAM_GetWritten(batch->buffer) > DAMA_BATCH_THRESHOLD)
		{
			dapi_batch_flush(dapi, batch);
		}
#endif
		return DERR_OK;
	}

#if defined(CONFIG_FUNC_USE_DAPI_DIRECT_QUERY)

	err = dapi_db_attachTable(dapi->db, header.name, header.uid);
	if(err != DERR_OK)
	{
		HxLOG_Warning("%s(%s) Attach Fail!\n", __FUNCTION__, header.name);

		return dapi_send(dapi, HLIB_RWSTREAM_GetBuf(dapi->send_stream), (HINT32)HLIB_RWSTREAM_GetOffset(dapi->send_stream), header.requestId);
	}
	else
	{

		if (header.size > 0)
		{
			err = dapi_command_set(dapi->db, &header, data);
		}
		else
		{
			err = dapi_command_reset(dapi->db, &header);
		}

		HxLOG_Debug("err: %d \n", err);
		if(err == DERR_DATACHANGED)
		{
			uidData = (void*)DAMA_Malloc(sizeof(HUID));
			DAMA_MemCpy(uidData, &uid, sizeof(HUID));
			(void)dapi_sendDataChangedEventToDama(dapi, name, uidData, 1);
			DAMA_Free(uidData);

			err = DERR_OK; // 기존과 맞춤...
		}

		return err;
	}
#else
	return dapi_send(dapi, HLIB_RWSTREAM_GetBuf(dapi->send_stream), (HINT32)HLIB_RWSTREAM_GetOffset(dapi->send_stream), header.requestId);
#endif

}

static HUINT32	dapi_get_data (dapi_t *dapi, const HCHAR *name, const HCHAR *query
								, DAPI_Getter getter, void *userdata)
{
#if defined(CONFIG_FUNC_USE_DAPI_DIRECT_QUERY)
	DAMA_Header_t	header;
	HUID		uid;
	DAMA_Result_t	*result;
	HUINT8		*buf;
	HINT32		temp_sd;
	HINT32		res_sd;
	const DAMA_Item_t	*item;
	DAMA_Header_t	resheader;
	dapi_request_t	*req;
	DERROR		dErr = DERR_FAIL;

	if(query == NULL)
		return 0; /* return request id zero */

	// 아래 함수를 체크해서 ERR를 리턴할 수 있지만, sql을 완벽하게 파싱한게 아니므로, 경고만 한다.
	(void) dapi_verify_query(dapi, query);

	DAMA_MemSet(&header, 0, sizeof(DAMA_Header_t));
	header.cmd   = eDAMA_CMD_REQUEST;
	header.requestId = dapi_next_request_id(dapi);

	if (getter)
	{
		req = dapi_alloc_request(dapi, getter, userdata, name, query);
		if (req)
		{
			req->id   = header.requestId;
			req->next = dapi->request;
			dapi->request = req;
		}
	}


	if (query)
	{
		header.size = HxSTD_StrLen(query);
	}
	if (name)
	{
		HLIB_STD_StrNCpySafe(header.name, name, DAMA_OBJECTNAME_LEN);
	}


	buf = dapi_get_tempbuf(dapi, header.size + 1);
	memcpy(buf, query, header.size);
	buf[header.size] = '\0';
	uid = 0;


	dErr = dapi_db_attachTable(dapi->db, header.name, uid);
	if(dErr != DERR_OK)
	{
		HxLOG_Warning("%s(%s) Attach Fail!\n", __FUNCTION__, name);
		goto DAMA_QUERY;

	}

	result = dapi_db_query(dapi->db, name, buf);
	if (result == NULL)
	{
		HxLOG_Warning("%s(%s) Empty repository!\n", __FUNCTION__, name);
		HLIB_DBG_Trace("DaMaQuery", {}, 0, buf);

		goto DAMA_QUERY;
	}
	else
	{
		memcpy(&resheader, &header, sizeof(DAMA_Header_t));
		resheader.cmd  = eDAMA_CMD_RESPONSE;
		resheader.size = 0;

		temp_sd = dapi_get_tempstream(dapi);
		HLIB_DBG_Trace("DapiQuery", {
			res_sd	= dapi_get_tempstream(dapi);
			while ((item = dapi_db_nextItem(result)) != NULL)
			{
				HLIB_RWSTREAM_Write(res_sd, (void *)&(item->uid) , sizeof(HUID));
				HLIB_RWSTREAM_Write(res_sd, (void *)&(item->size), sizeof(HUINT32));
				HLIB_RWSTREAM_Write(res_sd, (void *)item->data, item->size);
			}

			resheader.size = HLIB_RWSTREAM_GetOffset(res_sd);
			HLIB_RWSTREAM_Write(temp_sd, &resheader, sizeof(DAMA_Header_t));
			if (resheader.size > 0)
				HLIB_RWSTREAM_Write(temp_sd, HLIB_RWSTREAM_GetBuf(res_sd), resheader.size);


			dapi_receive_data (dapi, 0, resheader.requestId, HLIB_RWSTREAM_GetOffset(temp_sd), HLIB_RWSTREAM_GetBuf(temp_sd), TRUE);

		}, HLIB_RWSTREAM_GetOffset(temp_sd), buf);
	}


	goto END_FUNC;

DAMA_QUERY:
	HLIB_RWSTREAM_Rewind(dapi->send_stream);
	HLIB_RWSTREAM_Write(dapi->send_stream, &header, sizeof(DAMA_Header_t));

	if (query && header.size)
		HLIB_RWSTREAM_Write(dapi->send_stream, (void *)query, header.size);

	// socket write
	dapi_send(dapi, HLIB_RWSTREAM_GetBuf(dapi->send_stream), (HINT32)HLIB_RWSTREAM_GetOffset(dapi->send_stream), header.requestId);



END_FUNC:

	return header.requestId;


#else
	DAMA_Header_t	header;
	dapi_request_t	*req;

	if(query == NULL)
		return 0; /* return request id zero */

	// 아래 함수를 체크해서 ERR를 리턴할 수 있지만, sql을 완벽하게 파싱한게 아니므로, 경고만 한다.
	(void) dapi_verify_query(dapi, query);

	DAMA_MemSet(&header, 0, sizeof(DAMA_Header_t));
	header.cmd   = eDAMA_CMD_REQUEST;
	header.requestId = dapi_next_request_id(dapi);

	if (getter)
	{
		req = dapi_alloc_request(dapi, getter, userdata, name, query);
		if (req)
		{
			req->id   = header.requestId;
			req->next = dapi->request;
			dapi->request = req;
		}
	}

	if (query)
		header.size = HxSTD_StrLen(query);
	if (name)
		HLIB_STD_StrNCpySafe(header.name, name, DAMA_OBJECTNAME_LEN);
	HLIB_RWSTREAM_Rewind(dapi->send_stream);
	HLIB_RWSTREAM_Write(dapi->send_stream, &header, sizeof(DAMA_Header_t));

	if (query && header.size)
		HLIB_RWSTREAM_Write(dapi->send_stream, (void *)query, header.size);

	// socket write
	dapi_send(dapi, HLIB_RWSTREAM_GetBuf(dapi->send_stream), (HINT32)HLIB_RWSTREAM_GetOffset(dapi->send_stream), header.requestId);
	return header.requestId;
#endif
}

static HUINT32	dapi_add_notifier (dapi_t *dapi, const HCHAR *name, HUID uid, DAPI_Getter notifier, void *userdata)
{
	DAMA_Header_t	header;
	dapi_request_t	*req;

#if defined(DAMA_DEBUG_OBSERVER)
	HCHAR			 szProcessName[64];

	DAMA_MemSet(szProcessName, 0, 64);
	HxSTD_StrNCpy(szProcessName, HLIB_STD_GetCurProcessName(), 64);
	szProcessName[63] = '\0';
#endif

	DAMA_MemSet(&header, 0, sizeof(DAMA_Header_t));
	header.cmd = eDAMA_CMD_ADD_NOTIFIER;
	header.requestId = dapi_next_request_id(dapi);
	header.uid = uid;
	header.allowNotifyMyself = FALSE;
#if defined(DAMA_DEBUG_OBSERVER)
	header.size = 64;
#endif

	req = dapi_alloc_request(dapi, notifier, userdata, name, NULL);
	if (req == NULL)
	{
		HxLOG_Error("%s() out of memory!\n", __FUNCTION__);
		return 0;
	}
	req->id   = header.requestId;
	req->next = dapi->observer;
	dapi->observer = req;

	HLIB_STD_StrNCpySafe(header.name, name, DAMA_OBJECTNAME_LEN);
	HLIB_RWSTREAM_Rewind(dapi->send_stream);
	HLIB_RWSTREAM_Write(dapi->send_stream, &header, sizeof(DAMA_Header_t));
#if defined(DAMA_DEBUG_OBSERVER)
	HLIB_RWSTREAM_Write(dapi->send_stream, szProcessName, 64);
#endif
	// socket write
	dapi_send(dapi, HLIB_RWSTREAM_GetBuf(dapi->send_stream), (HINT32)HLIB_RWSTREAM_GetOffset(dapi->send_stream), header.requestId);
	return header.requestId;
}

static HUINT32	dapi_allow_notifyMyself (dapi_t *dapi, const HCHAR *name, HBOOL bAllow)
{
	DAMA_Header_t	header;
	dapi_request_t	*req;
	HUINT32			ulNotifierId = 0;

	req = dapi_find_request_name(&dapi->observer, name);
	if (req == NULL)
	{
		HxLOG_Warning("%s() There is no notifier!(id:0x%X)\n", __FUNCTION__, ulNotifierId);
		return 0;
	}

	ulNotifierId = req->id;

	DAMA_MemSet(&header, 0, sizeof(DAMA_Header_t));
	header.cmd = eDAMA_CMD_ALLOW_NOTIFY_MYSELF;
	header.requestId = ulNotifierId;
	header.allowNotifyMyself = bAllow;
	header.uid = DAMA_TABLE_NOTIFIER;
	HLIB_STD_StrNCpySafe(header.name, name, DAMA_OBJECTNAME_LEN);

	HLIB_RWSTREAM_Rewind(dapi->send_stream);
	HLIB_RWSTREAM_Write(dapi->send_stream, &header, sizeof(DAMA_Header_t));

	// socket write
	dapi_send(dapi, HLIB_RWSTREAM_GetBuf(dapi->send_stream), (HINT32)HLIB_RWSTREAM_GetOffset(dapi->send_stream), header.requestId);

	return ulNotifierId;
}

static void		dapi_remove_notifier (dapi_t *dapi, HUINT32 id)
{
	DAMA_Header_t	header;
	dapi_request_t	*req;

	req = dapi_pop_request(&dapi->observer, id);
	if (req == NULL)
	{
		HxLOG_Warning("%s() There is no notifier!(id:0x%X)\n", __FUNCTION__, id);
		return;
	}

	DAMA_MemSet(&header, 0, sizeof(DAMA_Header_t));
	header.cmd	 = eDAMA_CMD_REMOVE_NOTIFIER;
	header.requestId = id;
	HLIB_STD_StrNCpySafe(header.name, req->name, DAMA_OBJECTNAME_LEN);

	HLIB_RWSTREAM_Rewind(dapi->send_stream);
	HLIB_RWSTREAM_Write(dapi->send_stream, &header, sizeof(DAMA_Header_t));

	dapi_free_request(dapi, req);

	// socket write
	dapi_send(dapi, HLIB_RWSTREAM_GetBuf(dapi->send_stream), (HINT32)HLIB_RWSTREAM_GetOffset(dapi->send_stream), header.requestId);
}

static DERROR	dapi_sync (dapi_t *dapi, const HCHAR *name)
{
	DAMA_Header_t	header;
	dapi_batch_t	*batch = NULL;

	header.cmd   = eDAMA_CMD_SYNC;
	header.requestId = 0;
	if (name)
		HLIB_STD_StrNCpySafe(header.name, name, DAMA_OBJECTNAME_LEN);
	else
		HxSTD_StrNCpy(header.name, "", DAMA_OBJECTNAME_LEN);
	header.uid  = 0;
	header.size = 0;

	HLIB_RWSTREAM_Rewind(dapi->send_stream);
	HLIB_RWSTREAM_Write(dapi->send_stream, &header, sizeof(DAMA_Header_t));

	batch = dapi_batch_find(dapi, name, FALSE);
	if (batch)
	{
		dapi_batch_flush(dapi, batch);
	}

	return dapi_send(dapi, HLIB_RWSTREAM_GetBuf(dapi->send_stream), (HINT32)HLIB_RWSTREAM_GetOffset(dapi->send_stream), header.requestId);
}

static HCHAR *	dapi_make_lazy_job_key (dapi_t *dapi, const HCHAR *name, dapi_lazyfunc_t func)
{
	HCHAR	*buf;

	buf = HLIB_CBUF_GetBuffer(dapi->buf, 256);
	HxSTD_PrintToStrN(buf, 256, "%s:func(0x%08X)", name ? name : "*", (int)func);

	return buf;
}

static HBOOL	dapi_has_lazy_job (dapi_t *dapi, const HCHAR *name, dapi_lazyfunc_t func)
{
	HCHAR	*key;

	key = dapi_make_lazy_job_key(dapi, name, func);
	if (HLIB_HASH_Lookup(dapi->lazyjob, (void *)key) == NULL)
		return FALSE;
	return TRUE;
}

static DERROR	dapi_set_lazy_job (dapi_t *dapi, const HCHAR *name, HUINT32 term, dapi_lazyfunc_t jobfunc)
{
	HCHAR			*key;
	dapi_lazyjob_t	*job;

	key = dapi_make_lazy_job_key(dapi, name, jobfunc);
	job = (dapi_lazyjob_t *)HLIB_HASH_Lookup(dapi->lazyjob, (void *)key);
	if (job == NULL)
	{
		job = (dapi_lazyjob_t *)DAMA_Calloc(sizeof(dapi_lazyjob_t));
		if (job == NULL)
			return ERR_FAIL;
		HLIB_HASH_Insert(dapi->lazyjob, (void *)DAMA_StrDup(key), job);
	}

	if (name == NULL)
		name = "*";

	HLIB_STD_StrNCpySafe(job->name, name, DAMA_OBJECTNAME_LEN);
	job->timestamp = HLIB_STD_GetSystemTick();
	job->term      = term;
	job->func      = jobfunc;

	return DERR_OK;
}

static void		dapi_update_lazy_job (dapi_t *dapi, const HCHAR *name)
{
	dapi_lazyjob_t		*job;
	HxHASH_TableIter_t	iter;

	if (name == NULL)
		name = "*";

	HLIB_HASH_IterInit(&iter, dapi->lazyjob);
	while (HLIB_HASH_IterNext(&iter, NULL, (void **)&job))
	{
		if (strcmp(job->name, name) == 0)
		{
			job->timestamp = HLIB_STD_GetSystemTick();
		}
	}
}

static HINT32	dapi_do_lazy_job (dapi_t *dapi, HBOOL bNow)
{
	HUINT32			time;
	dapi_lazyjob_t	*job;
	HINT32			run;
	HxHASH_TableIter_t	iter;

	time = HLIB_STD_GetSystemTick();
	run  = 0;

	HLIB_HASH_IterInit(&iter, dapi->lazyjob);
	while (HLIB_HASH_IterNext(&iter, NULL, (void **)&job))
	{
		if ((time < job->timestamp + job->term) && !bNow)
		{
			HxLOG_Debug(HxANSI_COLOR_YELLOW("%s(time:%u, job->name:%s, job->timestamp:%u, job->term:%u)\n"),
					__FUNCTION__, time, job->name, job->timestamp, job->term);
			continue;
		}
		HxLOG_Debug (HxANSI_COLOR_GREEN("%s(time:%u, job->name:%s, job->timestamp:%u, job->term:%u)\n"),
					__FUNCTION__, time, job->name, job->timestamp, job->term);

		if (job->func)
			job->func(dapi, job->name[0] == '*' ? NULL : job->name);
		run++;
		HLIB_HASH_IterRemove(&iter);
	}
	return run;
}


static DERROR	dapi_restore (dapi_t *dapi, const HCHAR *name)
{
	DAMA_Header_t	header;

	header.cmd	 = eDAMA_CMD_RESTORE;
	header.requestId = 0;
	if (name)
		HLIB_STD_StrNCpySafe(header.name, name, DAMA_OBJECTNAME_LEN);
	else
		HxSTD_StrNCpy(header.name, "", DAMA_OBJECTNAME_LEN);
	header.uid	= 0;
	header.size = 0;

	HLIB_RWSTREAM_Rewind(dapi->send_stream);
	HLIB_RWSTREAM_Write(dapi->send_stream, &header, sizeof(DAMA_Header_t));

	return dapi_send(dapi, HLIB_RWSTREAM_GetBuf(dapi->send_stream), (HINT32)HLIB_RWSTREAM_GetOffset(dapi->send_stream), header.requestId);
}


static HUINT32	dapi_factory_reset (dapi_t *dapi)
{
	DAMA_Header_t	header;
	dapi_request_t	*req;

	DAMA_MemSet(&header, 0, sizeof(DAMA_Header_t));
	header.cmd	= eDAMA_CMD_FACTORY_RESET;
	header.requestId = dapi_next_request_id(dapi);
	HLIB_STD_StrNCpySafe(header.name, "", DAMA_OBJECTNAME_LEN);

	DAMA_TRACE("DAPI: request factory reset!\n");

	req = dapi_alloc_request(dapi, dapi_dummy_getter, "factory reset done!", NULL, NULL);
	if (req)
	{
		req->id   = header.requestId;
		req->next = dapi->request;
		dapi->request = req;
	}

	(void)dapi_send(dapi, &header, sizeof(DAMA_Header_t), header.requestId);
	return header.requestId;
}


static HUINT32	dapi_server_sync (dapi_t *dapi)
{
	DAMA_Header_t	header;
	dapi_request_t	*req;

	DAMA_MemSet(&header, 0, sizeof(DAMA_Header_t));
	header.cmd	= eDAMA_CMD_SERVER_SYNC;
	header.requestId = dapi_next_request_id(dapi);
	HLIB_STD_StrNCpySafe(header.name, "", DAMA_OBJECTNAME_LEN);

	DAMA_TRACE("DAPI: request Server Sync!\n");

	req = dapi_alloc_request(dapi, dapi_dummy_getter, "Server sync is done!", NULL, NULL);
	if (req)
	{
		req->id   = header.requestId;
		req->next = dapi->request;
		dapi->request = req;
	}

	(void)dapi_send(dapi, &header, sizeof(DAMA_Header_t), header.requestId);
	return header.requestId;
}

static HBOOL	dapi_has_running_job (dapi_t *dapi, HUINT32 request_id)
{
	dapi_request_t	*req;

	if (request_id == 0)
		return dapi->request ? TRUE : FALSE;

	for (req = dapi->request ; req ; req = req->next)
	{
		if (req->id == request_id)
			return TRUE;
	}
	return FALSE;
}

static void dapi_connect(dapi_t *dapi)
{
	dapi->socket  = HLIB_EXTRA_OpenClient("dama");//HLIB_SOCKET_OpenClient(HxSOCKET_UDS/*_RAW*/, DAMA_SIGNAL_UDS, 0);
	if (dapi->socket)
	{
		if (dapi->snd_timeout >= 0)
		{
			HLIB_SOCKET_SetSendTimeout(dapi->socket, (HUINT32)dapi->snd_timeout);
		}

		if (dapi->rcv_timeout >= 0)
		{
			HLIB_SOCKET_SetReceiveTimeout(dapi->socket, (HUINT32)dapi->rcv_timeout);
		}

		if (dapi->snd_bufsize > 0)
		{
			HLIB_SOCKET_SetSendBufferSize(dapi->socket, (HUINT32)dapi->snd_bufsize);
		}

		if (dapi->rcv_bufsize > 0)
		{
			HLIB_SOCKET_SetReceiveBufferSize(dapi->socket, (HUINT32)dapi->rcv_bufsize);
		}
	}
}
static void dapi_disconnect(dapi_t *dapi)
{
	HLIB_SOCKET_Close(dapi->socket);
	dapi->socket = 0;
}

static void dapi_receive_state (dapi_t *dapi, HxSocket_Status_e status, HINT32 socket_handle)
{
 	HxLOG_Debug("%s(status:%d, handle:0x%X)\n", __FUNCTION__, status, socket_handle);

 	if (status == HxSOCKET_DISCONNECT_CLIENT)
 	{
		HxLOG_Warning("dapi disconnected!!\n");
		dapi_disconnect(dapi);
 	}
}

static void	dapi_on_cmd_response (dapi_t *dapi, const DAMA_Header_t *header, const HUINT8 *data, HUINT32 request_id, HBOOL bDapiDirect)
{
	dapi_request_t	*req;
	DAPI_Getter 	callback = NULL;
	void *			userdata = NULL;

	if(bDapiDirect == FALSE)
		HxSEMT_Get(dapi->semaphore);


	req = dapi_find_request(&dapi->request, request_id);
	if (req)
	{
		DAPI_DEBUG_EXPR(HxLOG_Debug("%s() callback[%s:%s] called!!\n", __FUNCTION__, req->name, req->query));
		callback = req->callback;
		userdata = req->userdata;
	}

	if(bDapiDirect == FALSE)
		HxSEMT_Release(dapi->semaphore);


	if (callback)
		callback(header->name, header->uid, data, header->size, userdata);

	if (req)
	{
		if(bDapiDirect == FALSE)
			HxSEMT_Get(dapi->semaphore);

		dapi_free_request(dapi, dapi_pop_request(&dapi->request, request_id));

		if(bDapiDirect == FALSE)
			HxSEMT_Release(dapi->semaphore);

	}
}

static void	dapi_on_cmd_notification (dapi_t *dapi, const DAMA_Header_t *header, const HUINT8 *data, HUINT32 request_id)
{
	DAPI_Getter 	callback = NULL;
	void *			userdata = NULL;
	dapi_request_t	*observer;

	HxSEMT_Get(dapi->semaphore);
	for (observer = dapi->observer ; observer ; observer = observer->next)
	{
		if (observer->id == request_id)
		{
			DAPI_DEBUG_EXPR(HxLOG_Debug("%s() notifier[%s:%s] called!!\n", __FUNCTION__, observer->name, observer->query));
			callback = observer->callback;
			userdata = observer->userdata;
			break;
		}
	}
	HxSEMT_Release(dapi->semaphore);

	if (callback)
	{
		callback(header->name, header->uid, data, header->size, userdata);
	}
}

static void	dapi_on_cmd_exit (dapi_t *dapi)
{
	DAMA_TRACE("DAPI exit requested!!\n");

	HxSEMT_Get(dapi->semaphore);
	HLIB_SOCKET_Close(dapi->socket);
	dapi->socket = 0;
	HxSEMT_Release(dapi->semaphore);
}

static void	dapi_on_cmd_monitor (dapi_t *dapi, const DAMA_Header_t *header, const HUINT8 *data)
{
	void			*iter;
	DAMA_Status_e	status;
	DAxSTATUS_e		apistat;
	dapi_monitor_t	*mon;
	HINT32			n;
	HINT32			argv[10];

	if (data == NULL)
		return;

	DAMA_MemCpy(&status, data, sizeof(DAMA_Status_e));
	switch (status)
	{
	case eDAMA_LIMIT: apistat = eDAPI_LIMIT; break;
	case eDAMA_READY: apistat = eDAPI_READY; break;
	case eDAMA_BUSY:  apistat = eDAPI_BUSY;  break;
	default:
		return;
	}
	DAMA_TRACE("DAPI status monitor: %d\n", status);

	DAMA_MemSet(argv, 0, sizeof(HINT32) * 10);
	n = (header->size - sizeof(DAMA_Status_e)) / sizeof(HINT32);
	if (n > 10)
		n = 10;
	if (n > 0)
		DAMA_MemCpy(argv, data + sizeof(DAMA_Status_e), sizeof(HINT32) * n);

	HxSEMT_Get(dapi->semaphore);

	for (iter = HLIB_TREE_Iterator(dapi->monitor) ; iter ; iter = HLIB_TREE_NextIterator(iter))
	{
		mon = (dapi_monitor_t *)HLIB_TREE_IteratorValue(iter);
		if (HxSTD_StrCmp(mon->name, header->name) == 0)
		{
			dapi_monitor_t	temp;

			DAMA_MemCpy(&temp, mon, sizeof(dapi_monitor_t));
			HxSEMT_Release(dapi->semaphore);

			HxLOG_Debug(HxANSI_COLOR_YELLOW("%s(stat:%d, name:%s, argv[0]: %d)\n"), __FUNCTION__, apistat, header->name, argv[0]);
			if (n == 0)
				temp.callback(header->name, apistat, temp.userdata);
			else
				temp.callback(header->name, apistat, temp.userdata
					, argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7], argv[8], argv[9]
				);
			HxSEMT_Get(dapi->semaphore);
		}
	}
	HxSEMT_Release(dapi->semaphore);
}

static void	dapi_receive_data (dapi_t *dapi, HINT32 socket_handle, HUINT32 request_id, HUINT32 size, const void *data, HBOOL bDapiDirect)
{
	DAMA_Header_t	header;

	HxLOG_Trace();
	HxLOG_Debug("%s(socket:0x%X, request_id:0x%X, size:%u, data:0x%X)\n", __FUNCTION__, socket_handle, request_id, size, data);
	if (size < sizeof(DAMA_Header_t))
	{
		HxLOG_Critical("%s() check \'size\':%u\n", __FUNCTION__, size);
		return;
	}

	DAMA_MemCpy(&header, data, sizeof(DAMA_Header_t));
	if (header.size + sizeof(DAMA_Header_t) != size)
	{
		HxLOG_Critical("%s() invalid header!\n", __FUNCTION__);
		return;
	}
	dapi_recv(NULL, data, size, request_id);

	if (header.size == 0)
		data = NULL;
	else
		data = (const HUINT8 *)data + sizeof(DAMA_Header_t);

	if (header.cmd == eDAMA_CMD_RESPONSE)
	{
		dapi_on_cmd_response(dapi, &header, (const HUINT8 *)data, request_id, bDapiDirect);
	}
	else if (header.cmd == eDAMA_CMD_NOTIFICATION)
	{
		dapi_on_cmd_notification(dapi, &header, (const HUINT8 *)data, request_id);
	}
	else if (header.cmd == eDAMA_CMD_EXIT)
	{
		dapi_on_cmd_exit(dapi);
	}
	else if (header.cmd == eDAMA_CMD_MONITOR)
	{
		dapi_on_cmd_monitor(dapi, &header, (const HUINT8 *)data);
	}
}

static void 	dapi_on_packet_received (void *userData, HINT32 socketHandle, HUINT32 requestId, HINT32 size, void *pvData)
{
	dapi_receive_data((dapi_t *)userData, socketHandle, requestId, (HUINT32)size, pvData, FALSE);
}

static void 	dapi_on_status_changed (void *userData, HxSocket_Status_e eStatus, HINT32 nClientHandle)
{
	dapi_t	*dapi = (dapi_t *)userData;

	HxSEMT_Get(dapi->semaphore);
	dapi_receive_state(dapi, eStatus, nClientHandle);
	HxSEMT_Release(dapi->semaphore);
}

static const HCHAR *dapi_getStringOfReconnectState (dapi_reconnect_State_e recnct_state)
{
	switch (recnct_state)
	{
	case eReCnctState_None:
		return "NONE";
	case eReCnctState_Start:
		return "START";
	case eReCnctState_DisconnectOld:
		return "DISCONNECT";
	case eReCnctState_ConnectNew:
		return "CONNECT";
	default:
		break;
	}

	return "UNKNOWN";
}

static void		dapi_reconnectSocket (dapi_t *dapi)
{
	HxLOG_Critical("Socket Problem occurred... now try to reconnect\n");

	// Start -> Disconnect:
	dapi_disconnect(dapi);

	HxSEMT_Release(dapi->semaphore);
	VK_TASK_Sleep(1000);

	// Disconnect -> Connect
	while (1)
	{
		HxSEMT_Get(dapi->semaphore);

		dapi_connect(dapi);
		if (dapi->socket)
		{
			break;
		}

		HxSEMT_Release(dapi->semaphore);
		VK_TASK_Sleep(1000);
	}

	dapi->recnct_state = eReCnctState_None;
	dapi->num_socket_failed = 0;
	s_nDapiBlockedSocket = 0;
}

static void 	dapi_socket_listener (void *arg)
{
	dapi_t	*dapi = (dapi_t *)arg;
	HUINT32	synctick = 0;
	HUINT32	currtick;
	HUINT32	nTimeout = 100;
	HUINT32	nSleepTime;
	HINT32	nRet = 0;

	if (dapi == NULL)
	{
		HxLOG_Critical("!!!!! ERROR !!!!! \n");
		return;
	}

	HxLOG_Trace();
	for (;;)
	{
		nSleepTime = 0;

		if(dapi->socket)
		{
			nRet = HLIB_SOCKET_ListenTimeout(dapi->socket, nTimeout, dapi_on_packet_received, dapi_on_status_changed, (void *)dapi);
			if (nRet < 0)
			{
				if (eReCnctState_None == dapi->recnct_state)
				{
					HLIB_STD_TaskSleep(100);
					continue;
				}
			}

			HxSEMT_Get(dapi->semaphore);

			if (eReCnctState_None == dapi->recnct_state)
			{
				currtick = HLIB_STD_GetSystemTick();
				if (currtick - synctick > 500)
				{
					HINT32	synced = dapi_do_lazy_job(dapi, FALSE);
					if (synced)
					{
						HxLOG_Info("%s() lazy synced(%d)\n", __FUNCTION__, synced);
					}

					synctick = currtick;
				}

				nTimeout = (HLIB_HASH_Size(dapi->lazyjob) > 0) ? 100 : 500;
			}
			else
			{
				dapi_reconnectSocket(dapi);
				nTimeout = 500;
				nSleepTime = 50;
			}

			HxSEMT_Release(dapi->semaphore);
		}
		else
		{
			HxSEMT_Get(dapi->semaphore);
			dapi_connect(dapi);
			HxSEMT_Release(dapi->semaphore);
			nSleepTime = 100;
		}

		if (nSleepTime > 0)
		{
			HLIB_STD_TaskSleep(nSleepTime);
		}
	}
}


#if defined(DAPI_MONITOR_SOCKET_SENDING)
static void 	dapi_sndsize_checker (void *arg)
{
	while (1)
	{
		HxLOG_Critical("Max Tick (%dms : %dbytes), Max Size (%dms : %dbytes)\n", s_ulDapi_MaxTick_Tick, s_ulDapi_MaxTick_Size,
																				s_ulDapi_MaxSize_Tick, s_ulDapi_MaxSize_Size);
		VK_TASK_Sleep(10 * 1000);
	}
}
#endif

#if 1
static void 	dapi_async_sender (void *arg)
{
	unsigned long	 ulQueueId = (unsigned long)arg;
	dapi_SendMsg_t	 stMsg;
	HERROR			 hErr;
	dapi_t		*dapi;

	while (1)
	{
		hErr = VK_MSG_Receive(ulQueueId, &stMsg, sizeof(dapi_SendMsg_t));
		if (VK_OK == hErr)
		{
			dapi = dapi_get_instance();
			if (dapi && dapi->socket && (s_nDapiBlockedSocket != dapi->socket))
			{
				hErr = HLIB_SOCKET_Write(dapi->socket, stMsg.pvData, stMsg.ulSize, stMsg.ulReqId);
				if (ERR_OK != hErr)
				{
					dapi->num_socket_failed++;
					if (dapi->num_socket_failed >= DAPI_SEND_FAIL_MAXIMUM)
					{
						s_nDapiBlockedSocket = dapi->socket;
						dapi->recnct_state = eReCnctState_Start;
						dapi->socket_fail_tick = HLIB_STD_GetSystemTick();
					}
				}
				else
				{
					if ((dapi->num_socket_failed > 0) && (dapi->recnct_state == eReCnctState_None))
					{
						dapi->num_socket_failed--;
					}
				}
			}

			if (NULL != stMsg.pvData)
			{
				HLIB_STD_MemFree(stMsg.pvData);
			}
			dapi_release_instance(dapi);
		}
	}
}
#else
static void 	dapi_async_sender (void *arg)
{
	unsigned long	 ulQueueId = (unsigned long)arg;
	dapi_SendMsg_t	 stMsg;
#if defined(CONFIG_DEBUG)
	HUINT32			 ulTick;
#endif
	HERROR			 hErr;

	while (1)
	{
		hErr = VK_MSG_Receive(ulQueueId, &stMsg, sizeof(dapi_SendMsg_t));
		if (VK_OK == hErr)
		{
			if (stMsg.nSocket != s_nDapiBlockedSocket)
			{
#if defined(CONFIG_DEBUG)
				ulTick = HLIB_STD_GetSystemTick();
#endif
				hErr = HLIB_SOCKET_Write(stMsg.nSocket, stMsg.pvData, stMsg.ulSize, stMsg.ulReqId);
#if defined(CONFIG_DEBUG)
				ulTick = HLIB_STD_GetSystemTick() - ulTick;
#endif
				if (ERR_OK != hErr)
				{
					dapi_t		*dapi = dapi_get_instance();

					if (dapi)
					{
#if defined(CONFIG_DEBUG)
						HxLOG_Critical("HLIB_SOCKET_Write failed with Tick(%d), Size(%d)\n", ulTick, stMsg.ulSize);
#endif
						dapi->num_socket_failed++;

						if (dapi->num_socket_failed >= DAPI_SEND_FAIL_MAXIMUM)
						{
							s_nDapiBlockedSocket = dapi->socket;
							dapi->recnct_state = eReCnctState_Start;
							dapi->socket_fail_tick = HLIB_STD_GetSystemTick();
						}

						dapi_release_instance(dapi);
					}
				}
			}

			if (NULL != stMsg.pvData)
			{
				HLIB_STD_MemFree(stMsg.pvData);
			}
		}
	}
}
#endif
static dapi_t *	dapi_get_instance (void)
{
	static dapi_t	*s_dapi = NULL;

	if (s_dapi == NULL)
	{
		unsigned long	task_id;

		HxLOG_Trace();
		HxLOG_Debug("%s() dapi initialize.\n", __FUNCTION__);

		s_dapi = (dapi_t *)DAMA_Calloc(sizeof(dapi_t));

		// initialize
		HxSEMT_Create(&s_dapi->semaphore, "dapi_sem", 0);
		HxSEMT_Get(s_dapi->semaphore);

		s_dapi->snd_bufsize = -1;
		s_dapi->rcv_bufsize = -1;
		s_dapi->snd_timeout = -1;
		s_dapi->rcv_timeout = -1;

		dapi_connect(s_dapi);
		s_dapi->send_stream = HLIB_RWSTREAM_OpenEx(NULL, 4096, DAMA_Malloc_CB, DAMA_Free_CB);
		s_dapi->pid16   = (HUINT16)getpid(); // 일단 pid로 ..
		s_dapi->buf     = HLIB_CBUF_New(DAPI_CIRCULARBUF_SIZE);
		s_dapi->monitor = HLIB_TREE_NewFull((HxCompareDataFunc)dapi_compare_monitor, NULL, NULL, (HxDestroyNotify)DAMA_Free_CB);
		s_dapi->lazyjob = HLIB_HASH_NewFull(HLIB_HASH_GetStringKey, HLIB_HASH_IsStringEqual, (_HxDestroyNotify)DAMA_Free_CB, (_HxDestroyNotify)DAMA_Free_CB);
		HxLOG_Warning("** (%s) is connected to DAMA\n", HLIB_STD_GetCurProcessName());
#if	defined(DAMA_LOGGING) && DAMA_LOGGING == 0
		{
			HCHAR	name[64];
			HCHAR	nameBack[64];
			HxSTD_PrintToStrN(name, 64, "dapi.%s.comm", HLIB_STD_GetCurProcessName());
			s_dapi->logfp  = fopen(name, "wb");
#if defined(DAPI_CMD_LOGGING)
			HxSTD_PrintToStrN(name, 64, "dapicmd.%s.comm", HLIB_STD_GetCurProcessName());
			HxSTD_PrintToStrN(nameBack, 64, "dapicmd.%s.bak", HLIB_STD_GetCurProcessName());
			HLIB_FILE_Copy(name, nameBack);
			s_dapi->cmdlogfp  = fopen(name, "w");
#endif
		}
#endif

#if defined(CONFIG_FUNC_USE_DAPI_DIRECT_QUERY)
		s_dapi->db = dapi_db_open(DAMA_DIR_DB, DAMA_DIR_BACKUPDB, DAMA_DIR_TEMP_PATH);
		HxLOG_Debug("s_dapi->db->pMainDB: 0x%x", s_dapi->db->pMainDB);

#endif
		VK_TASK_Create(dapi_socket_listener, 70, SIZE_256K, "dapi_listener", (void*)s_dapi, &task_id, 0);
		VK_TASK_Start(task_id);

#if defined(DAPI_MONITOR_SOCKET_SENDING)
		{
			if (HxSTD_StrCmp(HLIB_STD_GetCurProcessName(), DAPI_MONITOR_SOCKET_SENDING_PROCESS) == 0)
			{
				VK_TASK_Create(dapi_sndsize_checker, 69, SIZE_32K, "dapi_sndsize", (void*)s_dapi, &task_id, 0);
				VK_TASK_Start(task_id);
			}
		}
#endif
	}
	else
	{
		HxSEMT_Get(s_dapi->semaphore);
	}

	if (s_dapi->socket == 0)
	{
		HxSEMT_Release(s_dapi->semaphore);
		return NULL;
	}
	return s_dapi;
}

static void		dapi_release_instance (dapi_t *dapi)
{
	if (dapi)
	{
		HxSEMT_Release(dapi->semaphore);
	}
}


static DERROR	dapi_wait (HUINT32 timeout, HUINT32 req_id)
{
	dapi_t	*dapi;
	HUINT32	tick;

	HxLOG_Info("%s(timeout:%u, requestId: 0x%X)\n", __FUNCTION__, timeout, req_id);
	DapiLOG("timeout[%d]\n", timeout);

	tick = HLIB_STD_GetSystemTick();
	for (;;)
	{
		dapi = dapi_get_instance();
		if (dapi)
		{
			if (!dapi_has_running_job(dapi, req_id))
			{
				dapi_release_instance(dapi);

				HxLOG_Info("%s(requestId: 0x%X) <-- succecss\n", __FUNCTION__, timeout, req_id);
				DAPI_DEBUG_EXPR(
					timeout = HLIB_STD_GetSystemTick() - tick;
					if (timeout < 1000)
						HxLOG_Debug("%s(%u) done!!\n", __FUNCTION__, timeout);
					else if (timeout < 3000)
						HxLOG_Warning("%s(%u) done!!\n", __FUNCTION__, timeout);
					else
						HxLOG_Critical("%s(%u) done!!\n", __FUNCTION__, timeout);
				);
				return DERR_OK;
			}
			dapi_release_instance(dapi);
		} else
			return DERR_FAIL;

		if (HLIB_STD_GetSystemTick() - tick > timeout)
			break;

		HLIB_STD_TaskSleep(10);
	}
	HxLOG_Warning("%s(timeout:%u, requestId: 0x%X) timeout!!\n", __FUNCTION__, timeout, req_id);

	if (req_id)
	{
		dapi = dapi_get_instance();
		if (dapi)
		{
			dapi_free_request(dapi, dapi_pop_request(&dapi->request, req_id));
			dapi_release_instance(dapi);
		}
	}
	return DERR_TIMEOUT;


}

HERROR	DAPI_Init(void)
{
	dapi_release_instance(dapi_get_instance());
	return ERR_OK;
}


DERROR	DAPI_BatchBegin (const HCHAR *name)
{
	dapi_t	*dapi;
	DERROR	err;
#if defined(DAPI_CMD_LOGGING)
	if(HxSTD_StrCmp("eDxEPG_TYPE_PF", name) != 0
	&& HxSTD_StrCmp("eDxEPG_TYPE_SC", name) != 0)
		DapiLOG("Name[%s]\n", name);
#endif

	dapi = dapi_get_instance();
	if (dapi)
	{
		err  = dapi_batch_begin(dapi, name);
		dapi_release_instance(dapi);
	}
	else err = DERR_FAIL;

	return err;
}

DERROR	DAPI_BatchEnd (const HCHAR *name)
{
	dapi_t	*dapi;
	DERROR	err;
#if defined(DAPI_CMD_LOGGING)
	if(HxSTD_StrCmp("eDxEPG_TYPE_PF", name) != 0
	&& HxSTD_StrCmp("eDxEPG_TYPE_SC", name) != 0)
		DapiLOG("Name[%s]\n", name);
#endif

	dapi = dapi_get_instance();
	if (dapi)
	{
		err  = dapi_batch_end(dapi, name);
		dapi_release_instance(dapi);
	}
	else err = DERR_FAIL;

	return err;
}

DERROR	DAPI_Set (
				const HCHAR *name
				, HUID uid
				, const void *data
				, HUINT32 size
			)
{
	dapi_t	*dapi;
	DERROR	err;

#if defined(DAPI_CMD_LOGGING)
	if(HxSTD_StrCmp("eDxEPG_TYPE_PF", name) != 0
	&& HxSTD_StrCmp("eDxEPG_TYPE_SC", name) != 0)
	DapiLOG("Name[%s] uid[%lld] size[%d]\n", name, uid, size);
#endif

	dapi = dapi_get_instance();
	if (dapi)
	{
		dapi_update_lazy_job(dapi, name);
		err  = dapi_set_data(dapi, name, uid, data, size);
		dapi_release_instance(dapi);
	}
	else err = DERR_FAIL;

	return err;
}


HUINT32	DAPI_Get (
				const HCHAR *name
				, HUID uid
				, DAPI_Getter getter
				, void *userdata
			)
{
	HxLOG_Trace();
	HxLOG_Assert(name);

	if (uid)
	{
		dapi_t	*dapi;
		HUINT32	rid;

		rid  = 0;
		dapi = dapi_get_instance();
		if (dapi)
		{
			dapi_update_lazy_job(dapi, name);
			rid = dapi_get_data(
					  dapi
					, name
					, dapi_make_sql(dapi, "SELECT * FROM %s WHERE uid="HUID_FMT, name, uid)
					, getter
					, userdata
				);
			dapi_release_instance(dapi);
		}
		return rid;
	}
	return DAPI_GetAll(name, getter, userdata);
}

HUINT32	DAPI_GetUIDInfos(const HCHAR *name, HUID *pullUids, HINT32 nNumber, DAPI_Getter getter, void *pvUserData)
{
	HxLOG_Trace();
	HxLOG_Assert(name);

	if (pullUids && nNumber)
	{
		dapi_t	*dapi;
		HUINT32	rid;

		rid = 0;
		dapi = dapi_get_instance();
		if (dapi)
		{
			HINT32	nStream, i;

			dapi_update_lazy_job(dapi, name);

			nStream = HLIB_RWSTREAM_Open(NULL, 4096);
			HLIB_RWSTREAM_Print(nStream, "SELECT * FROM %s WHERE uid IN (", name);
			for (i = 0; i < nNumber; i++)
			{
				if (i)
					HLIB_RWSTREAM_Print(nStream, ","HUID_FMT, pullUids[i]);
				else
					HLIB_RWSTREAM_Print(nStream, HUID_FMT, pullUids[i]);
			}
			HLIB_RWSTREAM_Print(nStream, ")");
			rid = dapi_get_data(
					dapi,
					name,
					HLIB_RWSTREAM_GetBuf(nStream),
					getter,
					pvUserData
				);
			HLIB_RWSTREAM_Close(nStream);
			dapi_release_instance(dapi);
		}

		return rid;
	}

	return DAPI_GetAll(name, getter, pvUserData);
}

HUINT32	DAPI_Query (
				  const HCHAR *query
				, DAPI_Getter getter
				, void *userdata
			)
{

	dapi_t	*dapi;
	HUINT32	rid;
#if defined(DAPI_CMD_LOGGING)
	if(HxSTD_StrStr(query,"eDxEPG_TYPE_PF") == NULL
	&& HxSTD_StrStr(query,"eDxEPG_TYPE_SC") == NULL)
	DapiLOG("query[%s]\n", query);
#endif
	HxLOG_Debug("%s(%s)\n", __FUNCTION__, query);

	dapi = dapi_get_instance();
	if (dapi)
	{
		rid  = dapi_get_data(dapi, NULL, query, getter, userdata);
		dapi_release_instance(dapi);
	}
	else rid = 0;

	return rid;
}

HUINT32	DAPI_GetAll (const HCHAR *name, DAPI_Getter getter, void *userdata)
{
	dapi_t	*dapi;
	HUINT32	rid;

	HxLOG_Trace();
	HxLOG_Assert(name);

	dapi = dapi_get_instance();
	if (dapi)
	{
		dapi_update_lazy_job(dapi, name);
		rid = dapi_get_data(
				  dapi
				, name
				, dapi_make_sql(dapi, "SELECT * FROM %s", name)
				, getter
				, userdata
			);
		dapi_release_instance(dapi);
	}
	else rid = 0;

	return rid;
}

HUINT32	DAPI_AddNotifier (const HCHAR *name, HUID uid, DAPI_Getter notifier, void *userdata)
{
	dapi_t	*dapi;
	HUINT32	id;

	HxLOG_Trace();

	dapi = dapi_get_instance();
	if (dapi)
	{
		dapi_update_lazy_job(dapi, name);
		id = dapi_add_notifier(dapi, name, uid, notifier, userdata);
		dapi_release_instance(dapi);
	}
	else id = 0;

	return id;
}

HUINT32	DAPI_AddTableNotifier(const HCHAR *name, DAPI_Getter notifier, void *userdata)
{
	return DAPI_AddNotifier(name, DAMA_TABLE_NOTIFIER, notifier, userdata);
}

void	DAPI_RemoveNotifier (HUINT32 id)
{
	dapi_t	*dapi;

	HxLOG_Trace();

	dapi = dapi_get_instance();
	if (dapi)
	{
		dapi_remove_notifier(dapi, id);
		dapi_release_instance(dapi);
	}
}

HBOOL	DAPI_AllowTableNotifyMyself(const HCHAR *name, HBOOL bAllow)
{
	dapi_t		*dapi;
	HBOOL		 bResult = TRUE;

	HxLOG_Trace();

	dapi = dapi_get_instance();
	if (dapi)
	{
		dapi_update_lazy_job(dapi, name);
		dapi_allow_notifyMyself(dapi, name, bAllow);
		dapi_release_instance(dapi);
	}
	else
	{
		bResult = FALSE;
	}

	return bResult;
}

DERROR	DAPI_Wait (HUINT32 timeout, HUINT32 requestId)
{
	HxLOG_Trace();
	return dapi_wait(timeout, requestId);
}

DERROR	DAPI_WaitRequests (HUINT32 timeout, const HUINT32 *ids, HUINT32 n)
{
	HUINT32	requestId;
	DERROR	err;
	HUINT32	prev, curr;
	HUINT32	i;

	HxLOG_Trace();

	if (ids == NULL || n == 0)
		return DAPI_Wait(timeout, 0);

	prev = HLIB_STD_GetSystemTick();
	for (i = 0 ; i < n ; i++)
	{
		requestId = ids[i];
		if (requestId != 0)
		{
			err = DAPI_Wait(timeout, requestId);
			if (err == DERR_TIMEOUT)
				return err;
		}
		curr = HLIB_STD_GetSystemTick();
		if (curr - prev >= timeout)
			return DERR_TIMEOUT;

		timeout -= (curr - prev);
		prev     = curr;
	}
	return DERR_OK;
}

DERROR	DAPI_WaitRequestsV (HUINT32 timeout, HUINT32 n, ...)
{
	va_list		ap;
	HUINT32		ids[32];
	HUINT32		i;

	HxLOG_Trace();

	if (n == 0)
		return DAPI_Wait(timeout, 0);

	va_start(ap, n);
	for (i = 0 ; i < n && i < 32 ; i++)
	{
		ids[i] = va_arg(ap, HUINT32);
	}
	va_end(ap);

	return DAPI_WaitRequests(timeout, ids, i);
}

DERROR	DAPI_Sync (const HCHAR *name)
{
#if 0//defined(CONFIG_FUNC_USE_DAPI_DIRECT_QUERY)
	return DERR_OK;
#else
	DERROR	err;
	dapi_t	*dapi;

	DapiLOG("Name[%s] \n", name);

	dapi = dapi_get_instance();
	if (dapi)
	{
		err  = dapi_sync(dapi, name);
		dapi_release_instance(dapi);
	} else
		err = DERR_FAIL;

	return err;
#endif
}

DERROR	DAPI_LazySet (
				const HCHAR *name
				, HUID uid
				, const void *data
				, HUINT32 size
				, HUINT32 term
			)
{
	dapi_t	*dapi;
	DERROR	err;

	DapiLOG("Name[%s] uid[%lld] size[%d]\n", name, uid, size);

	dapi = dapi_get_instance();
	if (dapi)
	{
		if (!dapi_has_lazy_job(dapi, name, (dapi_lazyfunc_t)dapi_batch_end))
		{
			dapi_batch_begin(dapi, name);
			dapi_set_lazy_job(dapi, name, term, (dapi_lazyfunc_t)dapi_batch_end);
		}
		else dapi_update_lazy_job(dapi, name);

		err  = dapi_set_data(dapi, name, uid, data, size);
		dapi_release_instance(dapi);
	}
	else err = DERR_FAIL;

	return err;
}

DERROR	DAPI_LazySync (const HCHAR *name, HUINT32 term)
{
	DERROR	err;
	dapi_t	*dapi;

	DapiLOG("Name[%s] Term[%d]\n", name, term);

	if (term == 0)
		return DAPI_Sync(name);

	HxLOG_Trace();

	dapi = dapi_get_instance();
	if (dapi)
	{
		err  = dapi_set_lazy_job(dapi, name, term, (dapi_lazyfunc_t)dapi_sync);
		dapi_release_instance(dapi);
	} else
		err = DERR_FAIL;
	return err;
}

DERROR	DAPI_LazyUpdate(const HCHAR *name)
{
	dapi_t	*dapi;

	DapiLOG("Name[%s]\n", name);

	dapi = dapi_get_instance();
	if (dapi)
	{
		dapi_update_lazy_job(dapi, name);
		dapi_do_lazy_job(dapi, TRUE);
		dapi_release_instance(dapi);
	}

	return DERR_FAIL;
}

DERROR	DAPI_Restore (const HCHAR *name)
{
	DERROR	err;
	dapi_t	*dapi;

	HxLOG_Trace();
	HxLOG_Assert(name);	// name must be specified

	dapi = dapi_get_instance();
	if (dapi)
	{
		dapi_update_lazy_job(dapi, name);
		err  = dapi_restore(dapi, name);
		dapi_release_instance(dapi);
	} else
		err = DERR_FAIL;

	return err;
}

DERROR	DAPI_Reset (const HCHAR *name, HUID uid)
{
	DapiLOG("Name[%s] uid[%lld]\n", name, uid);
	return DAPI_Set(name, uid, NULL, 0);
}

DERROR	DAPI_ResetTable (const HCHAR *name)
{
	DapiLOG("Name[%s]\n", name);
	return DAPI_Reset(name, DAMA_TABLE_RESET_UID);
}

DERROR	DAPI_ResetAll(void)
{
	DERROR	err;
	dapi_t	*dapi;
	HUINT32	req_id = 0;
	DapiLOG("DAPI_ResetAll", NULL);

	dapi = dapi_get_instance();
	if (dapi)
	{
		req_id = dapi_factory_reset(dapi);
		dapi_release_instance(dapi);
	}

	if (req_id)
	{
		DAMA_TRACE("DAPI: factory reset...\n");
		err = dapi_wait(60000, req_id);
		DAMA_TRACE("DAPI: factory reset done!\n");
	}
	else
	{
		DAMA_ERROR("DAPI: factory reset error!\n");
		err = ERR_FAIL;
	}
	return err;
}

DERROR	DAPI_WaitServerSync(void)
{
	DERROR	err;
	dapi_t	*dapi;
	HUINT32	req_id = 0;

	dapi = dapi_get_instance();
	if (dapi)
	{
		req_id = dapi_server_sync(dapi);
		dapi_release_instance(dapi);
	}

	if (req_id)
	{
		DAMA_TRACE("DAPI: server sync...\n");
		err = dapi_wait(60000, req_id);
		DAMA_TRACE("DAPI: server sync done!\n");
	} else
	{
		DAMA_ERROR("DAPI: server sync error!\n");
		err = ERR_FAIL;
	}

	return err;
}

void	DAPI_AddStatusMonitor (const HCHAR *name, DAPI_Monitor monitor, void *userdata)
{
	dapi_t	*dapi;

	HxLOG_Assert(name && monitor);

	dapi = dapi_get_instance();
	if (dapi)
	{
		dapi_monitor_t	*mon;

		if ((mon = (dapi_monitor_t *)DAMA_Calloc(sizeof(dapi_monitor_t))) != NULL)
		{
			HLIB_STD_StrNCpySafe(mon->name, name, DAMA_OBJECTNAME_LEN);
			mon->callback = monitor;
			mon->userdata = userdata;

			HLIB_TREE_Replace(dapi->monitor, (void *)mon, (void *)mon);
		}
		dapi_release_instance(dapi);
	}
}

void	DAPI_RemoveStatusMonitor (const HCHAR *name, DAPI_Monitor monitor)
{
	dapi_t	*dapi;

	HxLOG_Assert(name && monitor);

	dapi = dapi_get_instance();
	if (dapi)
	{
		dapi_monitor_t	mon;
		HLIB_STD_StrNCpySafe(mon.name, name, DAMA_OBJECTNAME_LEN);
		mon.callback = monitor;

		HLIB_TREE_Remove(dapi->monitor, (void *)&mon);
		dapi_release_instance(dapi);
	}
}

void	DAPI_InitIterator (DAxIterator_t *iter, const void *data, HUINT32 size, HUINT32 checksize)
{
	iter->next = (const HUINT8 *)data;
	iter->remain = size;
	iter->iteration = 0;
	iter->checksize = checksize;
}


HBOOL	DAPI_NextIterator (DAxIterator_t *iter)
{
	HUID	uid;
	HUINT32	size;
	HUINT32	nlen;

	if (iter->remain < 8)
	{
		HxLOG_Debug("\t>>> iter->remain == %d\n", iter->remain);
		return FALSE;
	}

	// read uid
	DAMA_MemCpy(&uid , iter->next, sizeof(HUID));
	iter->remain -= sizeof(HUID);
	iter->next   += sizeof(HUID);

	// read item size
	DAMA_MemCpy(&size, iter->next, 4);
	iter->remain -= 4;
	iter->next   += 4;
	if (size > iter->remain)
	{
		HxLOG_Debug("\t>>> (size:%u > iter->remain:%u)\n", size, iter->remain);
		return FALSE;
	}

	// read type name size
	DAMA_MemCpy(&nlen, iter->next, 4);
	if (nlen > iter->remain)
	{
		HxLOG_Debug("\t>>> (nlen:%u > iter->remain:%u)\n", nlen, iter->remain);
		return FALSE;
	}

	iter->uid  = uid;
	iter->name = (const HCHAR *)iter->next + 4;
	iter->data = iter->next + 4 + nlen;
	iter->size = size - nlen - 4;
	if (iter->checksize != 0 && iter->checksize != iter->size)
	{
		HxLOG_Critical("\t>>> invalid size (check:%u != %u)\n", iter->checksize, iter->size);
		return FALSE;
	}

	iter->remain -= size;
	iter->next   += size;
	iter->iteration++;

	return TRUE;
}

HERROR DAPI_Option (DAxOption_e eOption, HUINT32 ulValue, void *pvValue)
{
	dapi_t	*dapi;
	HERROR	 hErr = ERR_FAIL;

	dapi = dapi_get_instance();
	if (dapi)
	{
		switch (eOption)
		{
		case eDAPI_OPTION_SndBufSize:
			if (dapi->socket)
			{
				hErr = HLIB_SOCKET_SetSendBufferSize(dapi->socket, ulValue);
			}

			dapi->snd_bufsize = ulValue;
			break;

		case eDAPI_OPTION_RcvBufSize:
			if (dapi->socket)
			{
				hErr = HLIB_SOCKET_SetReceiveBufferSize(dapi->socket, ulValue);
			}

			dapi->rcv_bufsize = ulValue;
			break;

		case eDAPI_OPTION_SndTimeout:
			if (dapi->socket)
			{
				hErr = HLIB_SOCKET_SetSendTimeout(dapi->socket, ulValue);
			}

			dapi->snd_timeout = ulValue;
			break;

		case eDAPI_OPTION_RcvTimeout:
			if (dapi->socket)
			{
				hErr = HLIB_SOCKET_SetReceiveTimeout(dapi->socket, ulValue);
			}

			dapi->rcv_timeout = ulValue;
			break;

		case eDAPI_OPTION_AsyncSend:
			if (0 == dapi->send_task_qid)
			{
				unsigned long		 ulTaskId = 0;
				unsigned long		 ulQueueId = 0;
				unsigned long		 ulQueueSize = (ulValue > 0) ? (unsigned long)ulValue : DAPI_SEND_ASYNC_QUEUE_DEFAULT;

				hErr = VK_MSG_Create(ulQueueSize, sizeof(dapi_SendMsg_t), "dapi_asyncSend", &ulQueueId, VK_SUSPENDTYPE_FIFO);
				if ((ERR_OK == hErr) && (0 != ulQueueId))
				{
					hErr = VK_TASK_Create(dapi_async_sender, 70, SIZE_64K, "dapi_asyncSender", (void *)ulQueueId, &ulTaskId, 0);
					if ((ERR_OK == hErr) && (0 != ulTaskId))
					{
						hErr = VK_TASK_Start(ulTaskId);
						if (ERR_OK == hErr)
						{
							dapi->send_task_qid = ulQueueId;
						}
					}
				}

				if (0 == dapi->send_task_qid)
				{
					if (0 != ulTaskId)
					{
						VK_TASK_Stop(ulTaskId);
						VK_TASK_Destroy(ulTaskId);
					}

					if (0 != ulQueueId)
					{
						VK_MSG_Destroy(ulQueueId);
					}
				}
			}
			else
			{
				HxLOG_Error("Already Asynchronous Sending Activated:\n");
			}

			break;

		default:
			(void)pvValue;
			break;
		}

		dapi_release_instance(dapi);
	}

	return hErr;
}


