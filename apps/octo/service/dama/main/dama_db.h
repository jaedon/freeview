/**************************************************************
 *	@file		dama_db.h
 *	HumaxDataManager internal header for other applications.
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/03/19
 *	@author			humax
 *	@brief			DaMa internal header
 **************************************************************/
#ifndef	__DAMA_DB_H__
#define	__DAMA_DB_H__

#include	<dapi.h>


#include	<sqlite3.h>

#define DAMA_DIR_TEMP_PATH			"/tmp"
#define DAMA_EXT_JSON				".json"

#define	DAMA_DIR_DB					CONFIG_PRODUCT_DATA_DIR "/dlogger"
#define	DAMA_DIR_BACKUPDB			CONFIG_PRODUCT_BACKUPDATA_DIR "/dlogger"

#define	DAMA_FILE_JSON_DB			DAMA_DIR_DB DAMA_EXT_JSON
#define	DAMA_FILE_JSON_BACKUPDB		DAMA_DIR_BACKUPDB DAMA_EXT_JSON

#if 0
#define	DAMA_MAIN_DB					DAMA_DIR_TEMP_PATH "/main.db"
#endif


typedef struct dama_db				DAMA_DB_t;
typedef struct dama_result			DAMA_Result_t;
typedef	struct dama_table			DAMA_Table_t;

typedef struct
{
	HUID		uid;
	HCHAR		*tablename;
	HUINT32		size;
} DAMA_Item_t;


#define	DAMA_DB_RESULT_MAX		5
#define	MAX_DB_PATH_LEN			256
#define	MAX_DB_FILE_LEN			256
#define	DAMA_MANUAL_SYNC		0

struct dama_table
{
	HBOOL			bUpdated;					///< Is Updated?
	HBOOL			bUseCodec;
	HBOOL			bUseInternalCache;			///< decoder cache.
	HCHAR			szObjName[DxLIB_MAX_NAME];	///< Object Name
	HUINT32			ulSyncCycle;				///< Sync cycle (in mili seconds)
	HUINT32			ulLastSyncTime;				///< Last sync time (in mili seconds)
	HBOOL			bManualUpdate;				///< Manual update flag
	HCHAR			szReloadPath[MAX_DB_PATH_LEN];
	HBOOL			bIndexed;
	HxHASH_t	*pstCacheData;

	// Sync Thread 를 사용하기 위한 내용물
	HBOOL			bSyncInOtherThread;
	HBOOL			bInSyncJob;
	HxList_t		*pstQueueSetData;
	HUINT32			 ulLastSyncTick;

	// Load Thread를 사용하기 위한 내용
	HBOOL			 bLoadInOtherThread;
	HBOOL			 bLoadDone;
};

struct dama_result
{
	HxVector_t		*pList;
	HINT32			nIndex;
};

struct dama_db
{
	HCHAR			szPath[MAX_DB_PATH_LEN];
	HCHAR			szBackupPath[MAX_DB_PATH_LEN];
	HCHAR			szTempPath[MAX_DB_PATH_LEN];

	HINT32			nSqlBuf;				///< RWSTREAM for SQL Query
	sqlite3			*pMainDB;				///< Main DB on memory
	HxHASH_t	*pTableMap;				///< table (struct dama_table) map
	HINT32			nReloadable;
	struct {
		DAMA_Result_t	aResult[DAMA_DB_RESULT_MAX];
		HINT32			nIndex;
	} stTempResult;

#ifdef	CONFIG_DEBUG
	HxTREE_t		*pTableMonitor;
	FILE			*fpDataMonitor;
	HINT32			nDataMonitorNo;
	HINT32			nDataMonitorSeq;
#endif
};

typedef struct
{
	HINT32		size;
	HUINT8		data[4];
} dama_cache_t;

typedef struct
{
	HBOOL (*func)(const HCHAR *objectName, const HCHAR *tags[], void *userdata);
	void   *userdata;
} DAMA_ObjIter_t;


DAMA_DB_t *		DAMA_DB_Open (const HCHAR *path, const HCHAR *backupPath, const HCHAR *tmpPath, HBOOL bCreateSqlite, HBOOL bOpenDbFiles);
void			DAMA_DB_Close (DAMA_DB_t *db);
HBOOL			DAMA_DB_LoadCache(DAMA_DB_t *db);

HBOOL			DAMA_DB_HasChange (DAMA_DB_t *db);
DERROR			DAMA_DB_Sync (DAMA_DB_t *db);
DERROR			DAMA_DB_ForcedSync(DAMA_DB_t *db, const HCHAR *name);
DERROR			DAMA_DB_Restore (DAMA_DB_t *db, const HCHAR *name);
DERROR			DAMA_DB_CreateTable (DAMA_DB_t *db, const HCHAR *name);
DERROR			DAMA_DB_ReloadTable (DAMA_DB_t *db, HCHAR *szObjName);
HUINT32			DAMA_DB_Vacuum (DAMA_DB_t *db);
HUINT32			DAMA_DB_Count (DAMA_DB_t *db, const HCHAR *name);
void			DAMA_DB_CheckThreshold (DAMA_DB_t *db, void (* callback)(const HCHAR *name, HUINT32 count, void *userdata), void *userdata);
DERROR			DAMA_DB_Set (DAMA_DB_t *db, const HCHAR *name, HUID uid, HINT32 size, const void *data, HBOOL bCache);
#if defined(CONFIG_FUNC_USE_DAPI_DIRECT_QUERY)
DERROR			DAMA_DB_Datachanged (DAMA_DB_t *db, const HCHAR *name);
#endif
DAMA_Result_t *	DAMA_DB_Query (DAMA_DB_t *db, const HCHAR *name, const HCHAR *query);
void			DAMA_DB_Remove (DAMA_DB_t *db, const HCHAR *name, HUID uid, HBOOL useCache);

HBOOL DAMA_DB_SetChangedDataFlag(HBOOL bIsChangedData);
HBOOL DAMA_DB_GetChangedDataDataFlag(void);

const DAMA_Item_t*	DAMA_DB_NextItem (DAMA_Result_t *result);

HBOOL			DAMA_DB_AutoCommit(DAMA_DB_t *db, HBOOL enable);
HBOOL			DAMA_DB_Dump (DAMA_DB_t *db, const HCHAR *dest);
#if defined(CONFIG_DAMA_BACKGROUND_SYNC)
HERROR			DAMA_DB_QueueDataDuringSync (DAMA_DB_t *db, const HCHAR *name, const void *data, HUINT32 size);
HERROR			DAMA_DB_SyncTempFile (DAMA_DB_t *db, const HCHAR *name, const HCHAR *tempfile);
HERROR			DAMA_DB_FinalizeSync (DAMA_DB_t *db, const HCHAR *name);
#endif
#if defined(CONFIG_DAMA_BACKGROUND_LOAD)
HERROR			DAMA_DB_LoadDbTableAsync (DAMA_DB_t *db);
#endif
#if defined(DAMA_CHECK_STORAGE_STATUS)
HERROR			DAMA_DB_ExecuteForStorageChange (DAMA_DB_t *db, DAMA_Cmd_e eCmd, const void *pvData);
#endif
void			DAMA_DB_ForEachObject (HBOOL (*func)(const HCHAR *objectName, const HCHAR *tags[], void *userdata), void *userdata);
DAMA_Table_t *	DAMA_DB_GetTable (DAMA_DB_t *db, const HCHAR *objName);

#ifdef	CONFIG_DEBUG
HINT32			DAMA_DB_Command(DAMA_DB_t *db, const HCHAR *cmd, void *data);
#endif

#endif	//__DAMA_DB_H__

