#ifndef __APK_CAS_CI_H__
#define __APK_CAS_CI_H__


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

typedef	void	(*APK_CAS_CI_Notifier_t)(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);

typedef void (*APK_CAS_CI_AppMmiNotifier_t) (
											CI_AMMI_MSG_TYPE enMsgType,
											void*			pvUserData, // CI_AMMI_CONTENT 가 가게 된다.
											HUINT32 ulSize);



HERROR		APK_CAS_CI_Init(void);
void			APK_CAS_CI_RegisterNotifier(APK_CAS_CI_Notifier_t fnNotifier);
void			APK_CAS_CI_UnRegisterNotifier(APK_CAS_CI_Notifier_t fnNotifier);
HERROR		APK_CAS_CI_IsSupportedCI(HBOOL *pbSupported);
HERROR		APK_CAS_CI_IsSupportedCIPlus(HBOOL *pbSupported);
HERROR		APK_CAS_CI_GetCIPlusVersionName(HCHAR *pszVesionName);
HERROR		APK_CAS_CI_GetSlotCount(HUINT32 *pulSlotCount);
HERROR		APK_CAS_CI_GetCamNumberBySlot(HUINT32 ulSlot, HUINT32 *pulCamNumber);
HERROR		APK_CAS_CI_GetCamStatusByIndex(HUINT32 ulSlot, DxCASCI_CamStatus_e *peCamStatus);
HERROR		APK_CAS_CI_GetCamNameByIndex(HUINT32 ulSlot, HCHAR *pszCamName);

void	 		APK_CAS_CI_RegisterAppMmiCallback (APK_CAS_CI_AppMmiNotifier_t pfnCallback);
void			APK_CAS_CI_UnRegisterAppMmiCallback(APK_CAS_CI_AppMmiNotifier_t pfnCallback);
HERROR 		APK_CAS_CI_RequestFileorData (HUINT16 usSessionId, CI_AMMI_REQUEST_TYPE enReqType, HUINT8 *pucReqName, HULONG ulReqNameLen);
HERROR 		APK_CAS_CI_RequestAppAbort (HUINT16 usSessionId, CI_AMMI_ABORT_REQUEST_CODE enAbortReqCode);


#endif // __APK_CAS_CI_H__
