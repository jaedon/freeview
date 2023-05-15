
#ifndef	__OAPI_META_RECORD_H__
#define	__OAPI_META_RECORD_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <hlib.h>
#include <dlib.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

// To OBAMA
#define RPC_OAPI_META_RECORD_Update			"oapi.meta.record.Update"
#define RPC_OAPI_META_RECORD_Rename			"oapi.meta.record.Rename"
#define RPC_OAPI_META_RECORD_Remove			"oapi.meta.record.Remove"
#define RPC_OAPI_META_RECORD_Move				"oapi.meta.record.Move"
#define RPC_OAPI_META_RECORD_Copy				"oapi.meta.record.Copy"
#define RPC_OAPI_META_RECORD_CancelFileOperation		"oapi.meta.record.CancelFileOperation"
#define RPC_OAPI_META_RECORD_Lock				"oapi.meta.record.Lock"
#define RPC_OAPI_META_RECORD_AddBookmark		"oapi.meta.record.AddBookmark"
#define RPC_OAPI_META_RECORD_RemoveBookmark		"oapi.meta.record.RemoveBookmark"
#define RPC_OAPI_META_RECORD_RemoveAll			"oapi.meta.record.RemoveAll"
#define RPC_OAPI_META_RECORD_SetListener			"oapi.meta.record.SetListener"

// From OBAMA
#define RPC_OBAMA_META_RECORD_OnUpdate		"obama.meta.record.OnUpdate"
#define RPC_OBAMA_META_RECORD_OnScanDone	"obama.meta.record.OnScanDone"

#ifdef  __cplusplus
extern "C" {
#endif

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef enum
{
	eOxMETA_RECORD_ADDED,
	eOxMETA_RECORD_UPDATED,
	eOxMETA_RECORD_REMOVED,
	eOxMETA_RECORD_SCANDONE,

	eOxMETA_RECORD_LAST
} OxMetaRecord_Event_e;

/**
 * Recording Listener
 *
 * @param	evt		@see OxMetaRecord_Event_e
 * @param	count	변화량
 * @param	cid		contents id (if 0, DO NOT USE)
 * @param	rec		recording
 * @param	userdata
 */
typedef void (* OxMetaRecord_Notifier_t) (
						OxMetaRecord_Event_e evt
						, HINT32 count
						, HUINT32 cid
						, const DxRecListData_t *rec
						, void *userdata
					);


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/

/**
 * Update Recording
 * - recording을 업뎃
 *
 * @param	rec		recording
 */
void	OAPI_META_RECORD_Update (const DxRecListData_t *rec);

/**
 * rename
 *
 * @param	cid		contents id
 * @param	name	변경할 이름
 */
void	OAPI_META_RECORD_Rename (HUINT32 cid, const HCHAR *name);

/**
 * 레코딩 삭제
 *
 * @param	cid		contents id
 * @param	ret		(async) return callback
 * @param	userdata
 */
HINT32 OAPI_META_RECORD_Remove (HUINT32 cid, HxReturnFunc_t ret, void *userdata);

/**
 * 레코딩 이동
 *
 * @param	cid		contents id
 * @param	path	복사할 경로
 * @param	ret		(async) return callback
 * @param	userdata
 */
HINT32	OAPI_META_RECORD_Move (HUINT32 cid, const HCHAR *path, HxReturnFunc_t ret, void *userdata);

/**
 * 레코딩 복사
 *
 * @param	cid		contents id
 * @param	path	복사할 경로
 * @param	bDecrypt  decryption copy flag
 * @param	ret		(async) return callback
 * @param	userdata
 */
HINT32	OAPI_META_RECORD_Copy (HUINT32 cid, const HCHAR *path, HxReturnFunc_t ret, void *userdata);

/**
 * 레코딩 복사/이동 취소
 *
 * @param	cid		contents id
 */
void	OAPI_META_RECORD_CancelFileOperation (HUINT32 cid);

/**
 * Lock/Unlock
 *
 * @param	cid		contents id
 * @param	lockOrNot
 */
void	OAPI_META_RECORD_Lock (HUINT32 cid, HBOOL lockOrNot);

/**
 * Bookmark 추가
 *
 * @param	cid		contents id
 * @param	pos		position
 * @param	name	name
 */
void	OAPI_META_RECORD_AddBookmark (HUINT32 cid, HUINT32 pos, const HCHAR *name);

/**
 * Bookmark 삭제
 *
 * @param	cid		contents id
 * @param	pos		position
 * @param	name	name
 */
void	OAPI_META_RECORD_RemoveBookmark (HUINT32 cid, HUINT32 pos, const HCHAR *name);

/**
 * 레코딩 모두 삭제
 */
void	OAPI_META_RECORD_RemoveAll (void);

/**
 * Listener 등록/해제
 *
 * @param	listener
 * @param	userdata
 */
void	OAPI_META_RECORD_SetListener(OxMetaRecord_Notifier_t listener, void *userdata);



#ifdef  __cplusplus
}
#endif

#endif /* __OAPI_META_RECORD_H__ */

