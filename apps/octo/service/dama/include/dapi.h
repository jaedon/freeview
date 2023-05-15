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
 * Multi Thread ���� dapi�� ��� �ϴ� ��� ���� DAPI_Init() �� ���� �̸� context�� �����ؾ�
 * thread safe �ϰ� ���� �� �� �ִ�.
 */
HERROR	DAPI_Init(void);

/**
 * Batch begin/end ���̿� DAPI_Set�� ȣ���ϸ� �ѹ��� ��Ҵٰ� Set�� ��
 * ���󻡶󻡶�~
 */
DERROR	DAPI_BatchBegin (const HCHAR *name);
DERROR	DAPI_BatchEnd (const HCHAR *name);

/**
 * data�� dama�� ����
 */
DERROR	DAPI_Set (const HCHAR *name, HUID uid, const void *data, HUINT32 size);

/**
 * ���� Set�� �Ǿ� �ϴ� ��찡 �ƴϸ�, �� API�� ����ؼ� ���� ����ȭ�� �� �� �ִ�.
 * - ���������� Batch ���� ó���� �ϱ� ������ ����
 */
DERROR	DAPI_LazySet (const HCHAR *name, HUID uid, const void *data, HUINT32 size, HUINT32 term);

/**
 * name �� uid�� match�Ǵ� �����͸� dama�κ��� get
 * - getter�� async�� ȣ�� ��
 */
HUINT32	DAPI_Get (const HCHAR *name, HUID uid, DAPI_Getter getter, void *userdata);

/**
 * query�� match�Ǵ� �����͸� dama�� ���� get
 */
HUINT32	DAPI_Query (const HCHAR *query, DAPI_Getter getter, void *userdata);

/**
 * name �� match �Ǵ� ��� ���̺��� �����͸� dama�κ��� get
 */
HUINT32	DAPI_GetAll (const HCHAR *name, DAPI_Getter getter, void *userdata);

//	get data list from uid list.
HUINT32	DAPI_GetUIDInfos(const HCHAR *name, HUID *pullUids, HINT32 nNumber, DAPI_Getter getter, void *pvUserData);

/**
 * name�� uid�� match�Ǵ� �������� ������Ʈ�� �ִ� ���, notifier�� ������ ���
 * - notifier�� async�� ȣ���
 */
HUINT32	DAPI_AddNotifier (const HCHAR *name, HUID uid, DAPI_Getter notifier, void *userdata);

/**
 * name�� match�Ǵ� �������� ������Ʈ�� �ִ� ���, notifier�� ������ ���
 * - notifier�� async�� ȣ���
 */
HUINT32	DAPI_AddTableNotifier(const HCHAR *name, DAPI_Getter notifier, void *userdata);

/**
 * bAllow �� TRUE �̸�, �ڱ� �ڽ��� DB Update�� �ϴ� ��쵵 Notify �ǵ��� ����.
 */
HBOOL	DAPI_AllowTableNotifyMyself(const HCHAR *name, HBOOL bAllow);

/**
 * notifier ��� ����
 */
void	DAPI_RemoveNotifier (HUINT32 id);

/**
 * Async�� ȣ��Ǵ� DAPI_Get�� DAPI_Query�� getter�� ��ٸ�
 * - task sleep�̱� ������ ����
 * - getter�� notifier �ȿ��� ȣ���ϸ� ������ timeout ��
 */
DERROR	DAPI_Wait (HUINT32 timeout, HUINT32 requestId);
DERROR	DAPI_WaitRequests (HUINT32 timeout, const HUINT32 *ids, HUINT32 n);
DERROR	DAPI_WaitRequestsV (HUINT32 timeout, HUINT32 n, ...);

/**
 * ������ DAPI_Set�� ���� Server DB update�� ��ٸ���.
 * ���� DB File Save�� �ǹ��ϴ� ���� �ƴϰ� MemDB update�� �ǹ��Ѵ�.
 */
DERROR	DAPI_WaitServerSync(void);

/**
 * name �� match�Ǵ� DB table�� storage�� sync
 */
DERROR	DAPI_Sync (const HCHAR *name);

/**
 * ����� DAPI_Sync�� ������, term (millisec) ���� name�� ���� �ƹ��� API call�� ���� ���
 * ����ȭ�� �õ��Ѵ�.
 * - �����ϰ� ��û�� ���� ��� ���� ������ �� �� �ִ�.
 */
DERROR	DAPI_LazySync (const HCHAR *name, HUINT32 term);

/**
 * forced update lazy set.
 */
DERROR	DAPI_LazyUpdate(const HCHAR *name);

/**
 * name �� match�Ǵ� DB table�� storage�κ��� load
 */
DERROR	DAPI_Restore (const HCHAR *name);

/**
 * name�� uid�� match�Ǵ� �����͸� ����
 */
DERROR	DAPI_Reset (const HCHAR *name, HUID uid);

/**
 * name�� match�Ǵ� ���̺��� ����
 */
DERROR	DAPI_ResetTable (const HCHAR *name);

/**
 * ��� DB Table�� ���� & Sync
 * �� �Լ��� ����ȭ�� ������
 */
DERROR	DAPI_ResetAll(void);

void	DAPI_AddStatusMonitor (const HCHAR *name, DAPI_Monitor monitor, void *userdata);
void	DAPI_RemoveStatusMonitor (const HCHAR *name, DAPI_Monitor monitor);

/**
 * DAPI_Get(), DAPI_Query()�� getter�� ������ ������ iterator
 * �Ʒ� �ʵ�� ���� mutation�ϸ� OTL
 */
typedef struct
{
	HUID			uid;		///< uid
	const HCHAR		*name;		///< table name
	const HUINT8	*data;		///< data
	HUINT32			size;		///< data size

	HINT32			iteration;	///< �� �� ° iteration?
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


