
#ifndef	__OTL_HJM_H__
#define	__OTL_HJM_H__


#include <hlib.h>
#include <dlib.h>

#ifdef  __cplusplus
extern "C" {
#endif


typedef void 	(* OtlHjmPostProcessor_t) (DxRecListData_t *);
typedef HBOOL	(* OtlHjmCompare_t) (const DxRecListData_t *, void *key);

/**
 * RecordingList 생성 후, 나머지 정보를 체워줄 수 있는 callback 등록
 */
void				OTL_HJM_SetPostProcessor (OtlHjmPostProcessor_t postProcessor);

/**
 * path의 hjm을 읽어 Recording 인스턴스를 리턴한다.
 * (내부 컨텍스트에 추가하지 않음, 그냥 로딩으로 끝)
 */
DxRecListData_t *	OTL_HJM_Load (const HCHAR *path);

/**
 * 'path' 내의 hjm을 스캔 후, 완료 되면 async로 'scanDone'을 호출한다.
 * scanDone()의
 * 첫 번째 인자로는, userdata가 들어가고
 * 두 번째 인자로 개수,
 * 세 번째 인자로는 HxVector_t(DxRecListData_t *)가 들어온다.
 */
void				OTL_HJM_Scan (const HCHAR *path, HxIoctlFunc_t scanDone, void *userdata);

/**
 * OTL HJM은 url이 primary key이다. (contentId와 1:1로 매핑됨)
 * contentId를 url로 변경해 준다.
 * 만약, buf와 n을 넣지 않으면 alloc된 url이 리턴된다.
 */
HCHAR *				OTL_HJM_GetURL (HUINT32 contentId, HCHAR *buf, HUINT32 n);

/**
 * URL로 레코딩 얻기
 */
DxRecListData_t *	OTL_HJM_GetRecording (const HCHAR *url);

/**
 * 레코딩 리스트를 얻는다.
 */
HxVector_t *		OTL_HJM_GetRecordingList (OtlHjmCompare_t filter, void *key);

HUINT32				OTL_HJM_AddRecording (const DxRecListData_t *rec, HBOOL withHJM);
HUINT32				OTL_HJM_UpdateRecording (const DxRecListData_t *rec, HBOOL withHJM);

/**
 * 레코딩을 삭제 한다. (withHJM: HJM 파일도 삭제 한다.)
 */
HUINT32				OTL_HJM_RemoveRecording (const HCHAR *url, HBOOL withHJM);

/**
 * 레코딩 리스트를 삭제 한다. (withHJM: HJM 파일도 삭제 한다.)
 */
HUINT32				OTL_HJM_RemoveRecordingList (OtlHjmCompare_t filter, void *key, HBOOL withHJM);


#ifdef  __cplusplus
}
#endif

#endif //__OTL_HJM_H__

