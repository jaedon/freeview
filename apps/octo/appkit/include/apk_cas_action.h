#ifndef __APK_CAS_ACTION_H__
#define __APK_CAS_ACTION_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
//#include <oapi_cas_action.h>


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef	void	(*APK_CAS_ACTION_Notifier_t)(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
HERROR		APK_CAS_ACTION_Init(void);
void			APK_CAS_ACTION_RegisterNotifier(APK_CAS_ACTION_Notifier_t fnNotifer);
void			APK_CAS_ACTION_UnRegisterNotifier(APK_CAS_ACTION_Notifier_t fnNotifer);
HERROR 		APK_CAS_ACTION_GetActionType(HUINT8 *pucActionType);
HERROR		APK_CAS_ACTION_GetSlotId(HUINT8 *pucSlotId);
HERROR 		APK_CAS_ACTION_GetCasId(HUINT8 *pucCasId);
HERROR 		APK_CAS_ACTION_GetData(HUINT8 *pucData);





#endif // __APK_CAS_ACTION_H__
