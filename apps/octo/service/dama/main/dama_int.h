/**************************************************************
 *	@file		dama_int.h
 *	HumaxDataManager internal header for other applications.
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/03/19
 *	@author			humax
 *	@brief			DaMa internal header
 **************************************************************/
#ifndef	__DAMA_INT_H__
#define	__DAMA_INT_H__

#include	<dapi.h>

#define DAMA_CHECK_STORAGE_STATUS

#if defined(CONFIG_DEBUG) && defined(CONFIG_DAMA_MULTITHREAD)
#define DAMA_DEBUG_OBSERVER
#endif

#define DAMA_Malloc			HLIB_STD_MemAlloc
#define DAMA_Calloc			HLIB_STD_MemCalloc
#define	DAMA_Realloc		HLIB_STD_MemRealloc
#define	DAMA_StrDup			HLIB_STD_StrDup
#define	DAMA_MemDup			HLIB_STD_MemDup
#define	DAMA_MemSet			memset
#define	DAMA_MemCpy			memcpy
#define DAMA_Free			HLIB_STD_MemFree
#define	DAMA_Malloc_CB		HLIB_STD_MemAlloc_CB
#define	DAMA_Free_CB		HLIB_STD_MemFree_CB

/**************************************************************
	internal structures and variables
 **************************************************************/
//#define		DAMA_SIGNAL_UDS					"/tmp/.dama_signal.tmp"
#define		DAMA_MODULE_DEFAULT				"DAMADEF"
#define		DAMA_OBJECTNAME_LEN				64
#define		DAMA_DATA_STREAM				"/tmp/.dama.stream"

#define		DAMA_HEAD_SIZE					6
#define		DAMA_HEAD						"@dHaC@"
#define		DAMA_TAIL_SIZE					6
#define		DAMA_TAIL						"@dTaC@"

#define		DAMA_OBJECT_NAME_SIZE			4
#define		DAMA_KEY_SIZE					4
#define		DAMA_COUNT_SIZE					4
#define		DAMA_DATA_SIZE_SIZE				4
#define		DAMA_REQUEST_ID_SIZE			4

#define		DAMA_REQUEST_ID_NONE			"none"

#define		DAMA_DATA_START					"ds"
#define		DAMA_DATA_START_SIZE			2
#define		DAMA_DATA_FINISH				"df"
#define		DAMA_DATA_FINISH_SIZE			2
#define		DAMA_REQUEST_START				"dr"
#define		DAMA_REQUEST_START_SIZE			2

#define		DAMA_REQMODE_INDEX				"idx "
#define		DAMA_REQMODE_QUERY				"qry "
#define		DAMA_REQMODE_SIZE				4

#define		DAMA_RECEIVE_WAITING_TIME		10000
#define		DAMA_TABLE_NOTIFIER				-1
#define		DAMA_TABLE_RESET_UID			-1

#define		DAMA_BATCH_THRESHOLD			(1024 * 1024)
#define		DAMA_BATCH_THRESHOLD_COUNT		40

#define		DAMA_CONFIG_MULTITHREAD
#define		DAMA_CONFIG_Q_THRESHOLD			(1024 * 1024 * 20)	// 5M
#define		DAMA_CONFIG_PRIORITY			100
#define		DAMA_CONFIG_SYNC_DONE_PRIORITY	2000
#define		DAMA_CONFIG_VACUUM				500	// 500 page(maybe 500k)
#if defined(CONFIG_DAMA_PAGE_SIZE)
#define		DAMA_CONFIG_PAGE_SIZE			8192
#endif

#if defined(DAMA_CHECK_STORAGE_STATUS)
#define		DAMA_CONFIG_STORAGE_PRIORITY	3000
#endif

#ifdef		CONFIG_USES_NFSROOT
//#define		DAMA_LOGGING	1	// 0: dapi, 1:dama
#if	defined(DAMA_LOGGING) && DAMA_LOGGING == 0
//#define		DAPI_CMD_LOGGING
#endif
#endif

/*	DATA
		DH
			requestId(4)
			objectNameLen(4)
			"objectName" without \0
			"ds"
				objectKey(4)
				objectDataSize(4)
				objectKey(4)
				objectDataSize(4)
				objectData...
				....
			"df"
			objectNameLen(2)
			"objectName" without \0
			"ds"
				....
			"df"
		DT

	REQUEST
		DH
			requestId(4)
			objectNameLen(4)
			"objectName" without \0
			"dr"
				reqMode(4)		--> DAMA_REQMODE_QUERY/DAMA_REQMODE_INDEX

				DAMA_REQMODE_INDEX
					objectKey(4)
					objectCount(4)

				DAMA_REQMODE_QUERY
					queryLen(4)
					queryData()
			"df"
		DT
*/

typedef enum
{
	eDaTAG_OBJECT_NAME		= 0x20,
	eDaTAG_DATA_BINARY,
	eDaTAG_MAX
} DAMA_TAG_e;

typedef enum
{
	/**
	 * -->
	 * DAMA_Header_t
	 * data
	 */
	eDAMA_CMD_SET,
	/**
	 * -->
	 * DAMA_Header_t
	 * query(strlen(query) 만큼만, NULL이 없음)
	 */
	eDAMA_CMD_REQUEST,

	/**
	 * <-- (request)
	 * DAMA_Header_t
	 * loop {
	 *     HUID
	 *     size
	 * 	   data {
	 * 		   tablename_length(include '\0'): 4byte
	 * 		   tablename: Nbyte
	 * 		   data
	 * 	   }
	 * }
	 *
	 * <-- (factory reset)
	 * DAMA_Header_t
	 */
	eDAMA_CMD_RESPONSE,

	/**
	 * -->
	 * DAMA_Header_t {
	 * if (name == NULL)
	 *     table_notifier
	 * }
	 */
	eDAMA_CMD_ADD_NOTIFIER,

	/**
	 * -->
	 * DAMA_Header_t
	 */
	eDAMA_CMD_REMOVE_NOTIFIER,

	/**
	 * -->
	 * DAMA_Header_t
	 */
	eDAMA_CMD_ALLOW_NOTIFY_MYSELF,

	/**
	 * <-- (notification)
	 * DAMA_Header_t
	 * loop {
	 * 	   HUID
	 * }
	 */
	eDAMA_CMD_NOTIFICATION,

	/**
	 * -->
	 * DAMA_Header_t {
	 * if (name == NULL)
	 *     sync all
	 * }
	 */
	eDAMA_CMD_SYNC,

	/**
	 * -->
	 * DAMA_Header_t {
	 * if (name == NULL)
	 *     restore all
	 * }
	 */
	eDAMA_CMD_RESTORE,

	/**
	 * -->
	 * DAMA_Header_t {
	 * if (name == NULL)
	 *     table_notifier
	 * }
	 */
	eDAMA_CMD_BATCH,

	/**
	 * -->
	 * DAMA_Header_t
	 * loop (SET loop){
	 *     DAMA_Header_t
	 *     data
	 * }
	 */
	eDAMA_CMD_FACTORY_RESET,
	/**
	 * <--
	 * DAMA_Header_t
	 */
	eDAMA_CMD_SERVER_SYNC,
	/**
	 * <--
	 * DAMA_Header_t
	 */

	eDAMA_CMD_EXIT,

	/**
	 * <--
	 * DAMA_Header_t
	 * DAMA_Status_e
	 * argv HINT32[]
	 */
	eDAMA_CMD_MONITOR,

#if defined(CONFIG_FUNC_USE_DAPI_DIRECT_QUERY)
	/**
	 * -->
	 * DAMA_Header_t
	 *
	 */

	eDAMA_CMD_DATACHANGED,
#endif

#if defined(CONFIG_DAMA_BACKGROUND_SYNC)
	eDAMA_CMD_SYNCFINISHED,
#endif

#if defined(DAMA_CHECK_STORAGE_STATUS)
	/**
	 * <--
	 * DAMA_Header_t
	 * DxStorage_Info_t
	 */
	eDAMA_CMD_STORAGE_INSERTED,

	/**
	 * <--
	 * DAMA_Header_t
	 * DxStorage_Info_t
	 */
	eDAMA_CMD_STORAGE_REMOVED,

	/**
	 * <--
	 * DAMA_Header_t
	 * HUINT32 (StringLength)
	 * HINT32[]
	 */
	eDAMA_CMD_STORAGE_UNMOUNTED,
#endif

#if defined(CONFIG_DAMA_MULTITHREAD)
	/**
	 * <--
	 * DAMA_Header_t
	 */
	eDAMA_CMD_NOTIFY_TO_OBSERVERS,


#endif

	eDAMA_CMD_MAX
} DAMA_Cmd_e;


typedef enum
{
	eDAMA_NONE,
	eDAMA_READY,
	eDAMA_LIMIT,
	eDAMA_BUSY,
	eDAMA_UNKNOWN
} DAMA_Status_e;

typedef struct
{
	DAMA_Cmd_e	cmd;
	HUINT32		requestId;
	HCHAR		name[DAMA_OBJECTNAME_LEN];
	HUID		uid;
	HUINT32		size;
	HUINT32		allowNotifyMyself;
} DAMA_Header_t;


#ifdef	DAMA_LOGGING
typedef struct
{
	HCHAR		cmd[4];
	HUINT32		time;
	HUINT32		requestId;
	HUINT32		size;
} DAMA_Log_t;
#endif

#if defined(CONFIG_DAMA_BACKGROUND_SYNC) || defined(CONFIG_DAMA_BACKGROUND_LOAD)
typedef enum
{
	eDAMA_ASYNCTHREAD_MSGTYPE_None,

	eDAMA_ASYNCTHREAD_MSGTYPE_SyncTable,
	eDAMA_ASYNCTHREAD_MSGTYPE_LoadTable,

	eDAMA_ASYNCTHREAD_MSGTYPE_EndOfCase
} DAMA_AsyncMsgType_e;

typedef struct
{
	DAMA_AsyncMsgType_e		 eType;

	void		*pvDamaDb;		// DAMA_DB_t
	HCHAR		 szObjName[64];
	HCHAR		 szOrgFileName[64];
	HCHAR		 szTmpFileName[64];
} DAMA_SyncMsg_t;
#endif

typedef struct observer
{
	DAMA_Header_t	header;
	HINT32			updated;	// rwstream
	HUINT32			requestId;
	HINT32			clientHandle;
	HUINT32			pushCycle;

#if defined(DAMA_DEBUG_OBSERVER)
	HCHAR			szObserverProcess[64];
#endif

	struct {
		HUINT32		first;
		HUINT32		last;
	} tickStamp;
	struct observer	*next;
} DAMA_Observer_t;


/*****************************************************************************
	internal API description
 *****************************************************************************/

#ifdef	CONFIG_DEBUG
#define DAMA_DEBUG(expr)	do {expr;}while(0)
#else
#define DAMA_DEBUG(expr)
#endif


#define	DAMA_TRACE(...)	dama_Debug(__FUNCTION__, __LINE__, HxLOG_TRACE, __VA_ARGS__)
#define	DAMA_ERROR(...)	dama_Debug(__FUNCTION__, __LINE__, HxLOG_ERROR, __VA_ARGS__)

static void	dama_Debug (const HCHAR *function, HINT32 line, HxLOG_Level_e lv, const HCHAR *format, ...)
{
	HCHAR	buf[512];
	HCHAR	*msg, *dbuf;
	va_list	ap, cp;
	HINT32	n;

	dbuf = NULL;
	msg  = buf;

	va_start(ap, format);
	va_copy(cp, ap);
	n = HxSTD_PrintToStrV(buf, 512, format, ap);
	if (n >= 512 || n < 0)
	{
		dbuf = DAMA_Malloc(4096);
		if (dbuf == NULL)
		{
			va_end(cp);
			va_end(ap);
			return;
		}
		msg = dbuf;
		HxSTD_PrintToStrV(dbuf, 4096, format, cp);
	}
	va_end(cp);
	va_end(ap);

	if (lv & (HxLOG_TRACE | HxLOG_ERROR))
	{
		HLIB_CMD_CaptureLog("<%s:%d:%s> %s", function, line, (lv & HxLOG_ERROR) ? "ERROR" : "TRACE",msg);
	}

	switch (lv)
	{
	case HxLOG_TRACE:
		HxLOG_Message("<%s:%d> %s\n", function, line, msg);
		break;

	case HxLOG_ERROR:
		HxLOG_Error("<%s:%d> %s\n", function, line, msg);
		break;
	default:
		break;
	}

	if (dbuf)
		DAMA_Free(dbuf);
}

#ifdef	DAMA_LOGGING
static void		dama_Logging (FILE *fp, HCHAR cmd, const void *data, HUINT32 n, HUINT32 reqid)
{
	DAMA_Log_t	log;

	if (fp == NULL)
		return;

	memset(&log, 0, sizeof(DAMA_Log_t));
	log.cmd[0] = cmd;
	log.time   = HLIB_STD_GetSystemTime();
	log.requestId = reqid;
	log.size   = n;

	fwrite(&log, sizeof(DAMA_Log_t), 1, fp);
	fwrite(data, 1, n, fp);
}
#endif

#if defined(CONFIG_DAMA_BACKGROUND_SYNC)
extern HERROR	DAMA_SendSyncCmdToSyncThread (DAMA_AsyncMsgType_e eType, void *pvDamaDb, HCHAR *szObjName, HCHAR *szOrgFileName, HCHAR *szTmpFileName);
extern void		DAMA_TakeSyncJob (void);
extern void		DAMA_ReleaseSyncJob (void);
#endif

#endif

