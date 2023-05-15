#ifndef __APK_CAS_IRDETO_H__
#define __APK_CAS_IRDETO_H__


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/


typedef void (*APK_CAS_IRDETO_Notifier_t) (void);

HERROR 		APK_CAS_IRDETO_Init (void);
HERROR 		APK_CAS_IRDETO_RegisterNotifier(APK_CAS_IRDETO_Notifier_t fnNotifier);
HERROR 		APK_CAS_IRDETO_UnregisterNotifier(void);

HERROR		APK_CAS_IRDETO_SetVirtualSecureCwMode(HBOOL bVirtualSecureCwMode);
HERROR		APK_CAS_IRDETO_GetVirturlSecureCwMode(HBOOL *pbCwMode);
HERROR		APK_CAS_IRDETO_GetIrdetoClientVersionName(HCHAR *pszVesionName);
HERROR		APK_CAS_IRDETO_IsCWE(HBOOL *pbCWE);
HERROR		APK_CAS_IRDETO_IsJTAG(HBOOL *pbJTAG);
HERROR		APK_CAS_IRDETO_IsSBOOT(HBOOL *pbSBOOT);
HERROR		APK_CAS_IRDETO_IsOTP(HBOOL *pbOTP);
HERROR		APK_CAS_IRDETO_GetIrdetoManCode(HCHAR *pszMancode);
HERROR		APK_CAS_IRDETO_GetIrdetoHwCode(HCHAR *pszHWCode);
HERROR		APK_CAS_IRDETO_GetIrdetoVariant(HCHAR *pszVariant);
HERROR		APK_CAS_IRDETO_GetIrdetoSysId(HCHAR *pszSysId);
HERROR		APK_CAS_IRDETO_GetIrdetoKeyVer(HCHAR *pszKeyVer);
HERROR		APK_CAS_IRDETO_GetIrdetoSigVer(HCHAR *pszSigVer);
HERROR		APK_CAS_IRDETO_GetControlByte(HUINT8 *pControlByte);
HERROR		APK_CAS_IRDETO_GetPrivateData(HUINT8 *pPrivateData);

#endif // __APK_CAS_IRDETO_H__
