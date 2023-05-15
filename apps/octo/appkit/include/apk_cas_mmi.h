#ifndef __APK_CAS_MMI_H__
#define __APK_CAS_MMI_H__


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <oapi_cas_mmi.h>


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/



/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef void (*APK_CAS_MMI_Notifier_t) (HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/

void		APK_CAS_MMI_Init(void);
void		APK_CAS_MMI_RemoveEvtInfo(DxCAS_MMI_Event_t *pstEvent);
void		APK_CAS_MMI_GetEvtInfo(DxCAS_MMI_Event_t **ppstEvent);
void		APK_CAS_MMI_RemoveUiEvent(DxCAS_MmiSessionType_e eSessionType, DxCAS_MMI_Event_t *pstEvent);
void		APK_CAS_MMI_GetUiEvent(DxCAS_MmiSessionType_e eSession, Handle_t hSession, DxCAS_MMI_Event_t **ppstEvent);
void		APK_CAS_MMI_SendUiResponse(DxCAS_GroupId_e eModule, HUINT32 nSlot, DxCAS_MmiSessionType_e eSession, Handle_t hSession, const HCHAR *pzInputData);
void		APK_CAS_MMI_CloseSession(DxCAS_GroupId_e	eModule, HUINT32 nSlot, DxCAS_MmiSessionType_e eSession, Handle_t hSession);
Handle_t	APK_CAS_MMI_OpenSession(DxCAS_GroupId_e	eModule, HUINT32 nSlot, DxCAS_MmiSessionType_e eSession, const char *pcParamData);
void		APK_CAS_MMI_UnRegisterSessionNotifier(DxCAS_MmiSessionType_e eSession);
void		APK_CAS_MMI_RegisterSessionNotifier(DxCAS_MmiSessionType_e eSession, APK_CAS_MMI_Notifier_t fnNotifier);
void		APK_CAS_MMI_UnRegisterEventNotifier(HUINT32 objId);


#endif // __APK_CAS_MMI_H__
