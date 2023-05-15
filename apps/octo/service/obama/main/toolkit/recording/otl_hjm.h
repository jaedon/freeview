
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
 * RecordingList ���� ��, ������ ������ ü���� �� �ִ� callback ���
 */
void				OTL_HJM_SetPostProcessor (OtlHjmPostProcessor_t postProcessor);

/**
 * path�� hjm�� �о� Recording �ν��Ͻ��� �����Ѵ�.
 * (���� ���ؽ�Ʈ�� �߰����� ����, �׳� �ε����� ��)
 */
DxRecListData_t *	OTL_HJM_Load (const HCHAR *path);

/**
 * 'path' ���� hjm�� ��ĵ ��, �Ϸ� �Ǹ� async�� 'scanDone'�� ȣ���Ѵ�.
 * scanDone()��
 * ù ��° ���ڷδ�, userdata�� ����
 * �� ��° ���ڷ� ����,
 * �� ��° ���ڷδ� HxVector_t(DxRecListData_t *)�� ���´�.
 */
void				OTL_HJM_Scan (const HCHAR *path, HxIoctlFunc_t scanDone, void *userdata);

/**
 * OTL HJM�� url�� primary key�̴�. (contentId�� 1:1�� ���ε�)
 * contentId�� url�� ������ �ش�.
 * ����, buf�� n�� ���� ������ alloc�� url�� ���ϵȴ�.
 */
HCHAR *				OTL_HJM_GetURL (HUINT32 contentId, HCHAR *buf, HUINT32 n);

/**
 * URL�� ���ڵ� ���
 */
DxRecListData_t *	OTL_HJM_GetRecording (const HCHAR *url);

/**
 * ���ڵ� ����Ʈ�� ��´�.
 */
HxVector_t *		OTL_HJM_GetRecordingList (OtlHjmCompare_t filter, void *key);

HUINT32				OTL_HJM_AddRecording (const DxRecListData_t *rec, HBOOL withHJM);
HUINT32				OTL_HJM_UpdateRecording (const DxRecListData_t *rec, HBOOL withHJM);

/**
 * ���ڵ��� ���� �Ѵ�. (withHJM: HJM ���ϵ� ���� �Ѵ�.)
 */
HUINT32				OTL_HJM_RemoveRecording (const HCHAR *url, HBOOL withHJM);

/**
 * ���ڵ� ����Ʈ�� ���� �Ѵ�. (withHJM: HJM ���ϵ� ���� �Ѵ�.)
 */
HUINT32				OTL_HJM_RemoveRecordingList (OtlHjmCompare_t filter, void *key, HBOOL withHJM);


#ifdef  __cplusplus
}
#endif

#endif //__OTL_HJM_H__

