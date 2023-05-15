/**************************************************************
 *	@file		dapi.h
 *	HumaxDataManager global header for other applications.
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/03/19
 *	@author			humax
 *	@brief			DaMa global header
 **************************************************************/
#ifndef	__DAPI_H__
#define	__DAPI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <hlib.h>

/**************************************************************
	DaMa definition
 **************************************************************/
typedef enum
{
	DERR_OK,
	DERR_FAIL,
	DERR_OOM,
	DERR_TIMEOUT,
	DERR_DATACHANGED
} DERROR;

typedef enum
{
	eDAPI_NONE,
	eDAPI_READY,
	eDAPI_LIMIT,
	eDAPI_BUSY,
	eDAPI_UNKNOWN
} DAxSTATUS_e;//DAPI_Status_e;

typedef enum
{
	eDAPI_OPTION_SndBufSize,
	eDAPI_OPTION_RcvBufSize,
	eDAPI_OPTION_SndTimeout,
	eDAPI_OPTION_RcvTimeout,
	eDAPI_OPTION_AsyncSend,

	eDAPI_OPTION_EndOfCase
} DAxOption_e;

#define DAxFACOTRY_RESET_SLEEP_TIME 500

typedef void	(* DAPI_Getter ) (const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata);
typedef void	(* DAPI_Monitor) (const HCHAR *name, DAxSTATUS_e status, void *userdata, ...);

/**
 * Multi Thread 에서 dapi를 사용 하는 경우 에는 DAPI_Init() 을 통해 미리 context를 생성해야
 * thread safe 하게 동작 할 수 있다.
 */
HERROR	DAPI_Init(void);

/**
 * Batch begin/end 사이에 DAPI_Set을 호출하면 한번에 모았다가 Set을 함
 * 빨라빨라빨라~
 */
DERROR	DAPI_BatchBegin (const HCHAR *name);
DERROR	DAPI_BatchEnd (const HCHAR *name);

/**
 * data를 dama에 전송
 */
DERROR	DAPI_Set (const HCHAR *name, HUID uid, const void *data, HUINT32 size);

/**
 * 당장 Set이 되야 하는 경우가 아니면, 이 API를 사용해서 성능 최적화를 할 수 있다.
 * - 내부적으로 Batch 연산 처리를 하기 때문에 고성능
 */
DERROR	DAPI_LazySet (const HCHAR *name, HUID uid, const void *data, HUINT32 size, HUINT32 term);

/**
 * name 과 uid에 match되는 데이터를 dama로부터 get
 * - getter는 async로 호출 됨
 */
HUINT32	DAPI_Get (const HCHAR *name, HUID uid, DAPI_Getter getter, void *userdata);

/**
 * query에 match되는 데이터를 dama로 부터 get
 */
HUINT32	DAPI_Query (const HCHAR *query, DAPI_Getter getter, void *userdata);

/**
 * name 에 match 되는 모든 테이블의 데이터를 dama로부터 get
 */
HUINT32	DAPI_GetAll (const HCHAR *name, DAPI_Getter getter, void *userdata);

//	get data list from uid list.
HUINT32	DAPI_GetUIDInfos(const HCHAR *name, HUID *pullUids, HINT32 nNumber, DAPI_Getter getter, void *pvUserData);

/**
 * name과 uid에 match되는 데이터의 업데이트가 있는 경우, notifier가 오도록 등록
 * - notifier는 async로 호출됨
 */
HUINT32	DAPI_AddNotifier (const HCHAR *name, HUID uid, DAPI_Getter notifier, void *userdata);

/**
 * name에 match되는 데이터의 업데이트가 있는 경우, notifier가 오도록 등록
 * - notifier는 async로 호출됨
 */
HUINT32	DAPI_AddTableNotifier(const HCHAR *name, DAPI_Getter notifier, void *userdata);

/**
 * bAllow 가 TRUE 이면, 자기 자신이 DB Update를 하는 경우도 Notify 되도록 설정.
 */
HBOOL	DAPI_AllowTableNotifyMyself(const HCHAR *name, HBOOL bAllow);

/**
 * notifier 등록 해제
 */
void	DAPI_RemoveNotifier (HUINT32 id);

/**
 * Async로 호출되는 DAPI_Get과 DAPI_Query의 getter를 기다림
 * - task sleep이기 때문에 비추
 * - getter나 notifier 안에서 호출하면 무조건 timeout 남
 */
DERROR	DAPI_Wait (HUINT32 timeout, HUINT32 requestId);
DERROR	DAPI_WaitRequests (HUINT32 timeout, const HUINT32 *ids, HUINT32 n);
DERROR	DAPI_WaitRequestsV (HUINT32 timeout, HUINT32 n, ...);

/**
 * 마지막 DAPI_Set에 대한 Server DB update를 기다린다.
 * 실제 DB File Save를 의미하는 것이 아니고 MemDB update를 의미한다.
 */
DERROR	DAPI_WaitServerSync(void);

/**
 * name 에 match되는 DB table을 storage에 sync
 */
DERROR	DAPI_Sync (const HCHAR *name);

/**
 * 기능은 DAPI_Sync와 같지만, term (millisec) 동안 name에 대한 아무런 API call이 없는 경우
 * 동기화를 시도한다.
 * - 자잘하게 요청이 많은 경우 성능 유지를 할 수 있다.
 */
DERROR	DAPI_LazySync (const HCHAR *name, HUINT32 term);

/**
 * forced update lazy set.
 */
DERROR	DAPI_LazyUpdate(const HCHAR *name);

/**
 * name 에 match되는 DB table을 storage로부터 load
 */
DERROR	DAPI_Restore (const HCHAR *name);

/**
 * name과 uid에 match되는 데이터를 삭제
 */
DERROR	DAPI_Reset (const HCHAR *name, HUID uid);

/**
 * name에 match되는 테이블을 삭제
 */
DERROR	DAPI_ResetTable (const HCHAR *name);

/**
 * 모든 DB Table을 삭제 & Sync
 * 이 함수는 동기화를 보장함
 */
DERROR	DAPI_ResetAll(void);

void	DAPI_AddStatusMonitor (const HCHAR *name, DAPI_Monitor monitor, void *userdata);
void	DAPI_RemoveStatusMonitor (const HCHAR *name, DAPI_Monitor monitor);

/**
 * DAPI_Get(), DAPI_Query()의 getter로 들어오는 데이터 iterator
 * 아래 필드들 직접 mutation하면 OTL
 */
typedef struct
{
	HUID			uid;		///< uid
	const HCHAR		*name;		///< table name
	const HUINT8	*data;		///< data
	HUINT32			size;		///< data size

	HINT32			iteration;	///< 몇 번 째 iteration?
	const HUINT8	*next;		///< private
	HUINT32			remain;		///< private
	HUINT32			checksize;	///< private
} DAxIterator_t;
void	DAPI_InitIterator (DAxIterator_t *iter, const void *data, HUINT32 size, HUINT32 checksize);
HBOOL	DAPI_NextIterator (DAxIterator_t *iter);

HERROR	DAPI_Option (DAxOption_e eOption, HUINT32 ulValue, void *pvValue);


#if defined(CONFIG_FUNC_USE_DAPI_DIRECT_QUERY)
void DAPI_PrintMemUsed(void);
#endif

#ifdef __cplusplus
}
#endif

#endif


