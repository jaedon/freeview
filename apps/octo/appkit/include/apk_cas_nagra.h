#ifndef __APK_CAS_NAGRA_H__
#define __APK_CAS_NAGRA_H__


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/


typedef void (*APK_CAS_NAGRA_Notifier_t) (HBOOL *pbRet);

#if defined(CONFIG_SUPPORT_NOVA_APP)
typedef void (*APK_CAS_NAGRA_NovaTransportSessionInfoNotifier_t) (HUINT32 hAction, HUINT32 ulTransportSessionId, HINT32 usOnId, HINT32 usTsId, HINT32 usSvcId, HUINT16 usAudioPid, HUINT16 usVideoPid);
typedef void (*APK_CAS_NAGRA_NovaLicenseNotifier_t) (HUINT32 ulTransportSessionId, HUINT32 ulRecordHandle, HUINT8 *pucLicense, HUINT32 ulSize, HBOOL bRequestLicenseAgain);
typedef void (*APK_CAS_NAGRA_NovaKeyHandlerNotifier_t) (HUINT8 *pucData);
typedef void (*APK_CAS_NAGRA_NovaPlatformUsageRulesNotifier_t) (HUINT8 *pucUsageRules, HUINT32 ulSize);
typedef void (*APK_CAS_NAGRA_NovaServiceUsageRulesNotifier_t) (HUINT32 ulTransportSessionId, HUINT8 *pucUsageRules, HUINT32 ulSize);
#endif

HERROR 		APK_CAS_NAGRA_Init (void);
HERROR 		APK_CAS_NAGRA_RegisterNotifier(APK_CAS_NAGRA_Notifier_t fnNotifier);
HERROR 		APK_CAS_NAGRA_UnregisterNotifier(void);
#if defined(CONFIG_SUPPORT_NOVA_APP)
HERROR 		APK_CAS_NAGRA_NOVA_Init (void);
HERROR			APK_CAS_NAGRA_UnregisterNovaTransportSessionInfoNotifier(void);
HERROR 		APK_CAS_NAGRA_RegisterNovaTransportSessionInfoNotifier(APK_CAS_NAGRA_NovaTransportSessionInfoNotifier_t fnNotifier);
HERROR 		APK_CAS_NAGRA_UnregisterNovaLicenseNotifier(void);
HERROR 		APK_CAS_NAGRA_RegisterNovaLicenseNotifier(APK_CAS_NAGRA_NovaLicenseNotifier_t fnNotifier);
HERROR 		APK_CAS_NAGRA_RegisterNovaPlatformUsageRulesNotifier(APK_CAS_NAGRA_NovaPlatformUsageRulesNotifier_t fnNotifier);
HERROR 		APK_CAS_NAGRA_UnregisterNovaPlatformUsageRulesNotifier(void);
HERROR 		APK_CAS_NAGRA_RegisterNovaServiceUsageRulesNotifier(APK_CAS_NAGRA_NovaServiceUsageRulesNotifier_t fnNotifier);
HERROR 		APK_CAS_NAGRA_UnregisterNovaServiceUsageRulesNotifier(void);
HERROR 		APK_CAS_NAGRA_RegisterNovaKeyHandlerNotifier(APK_CAS_NAGRA_NovaKeyHandlerNotifier_t fnNotifier);
HERROR 		APK_CAS_NAGRA_UnregisterNovaKeyHandlerNotifier(void);
#endif

HERROR		APK_CAS_NAGRA_IsCWE(HBOOL *pbCWE);
HERROR		APK_CAS_NAGRA_IsJTAG(HBOOL *pbJTAG);
HERROR		APK_CAS_NAGRA_IsSBOOT(HBOOL *pbSBOOT);
HERROR		APK_CAS_NAGRA_IsOTP(HBOOL *pbOTP);
HERROR		APK_CAS_NAGRA_GetNUID(HCHAR *pszNUID);
HERROR		APK_CAS_NAGRA_GetCASN(HCHAR *pszCASN);
HERROR	    APK_CAS_NAGRA_GetFusingInfo(HUINT8 *pucSCS, HUINT8 *pucJTAG, HUINT8 *pucOTP, HUINT8 *pucCWE);
HBOOL		APK_CAS_NAGRA_IsSCInserted(void);
HBOOL		APK_CAS_NAGRA_PINCODE_Verify(const HCHAR *pszPinStr);
HBOOL		APK_CAS_NAGRA_GetAccessibleState(HUINT8 *pnAccessState, HUINT32 *pulRemainTime, HUINT8 *pnRetryCount);
HBOOL		APK_CAS_NAGRA_VerifyPincodeGetAccessibleState(HUINT8 *pnAccessState, HUINT32 *pulRemainTime, HUINT8 *pnRetryCount, const HCHAR *pszPinStr);
HBOOL		APK_CAS_NAGRA_IsAccessibleState(HUINT8 *pnAccessState);
HBOOL		APK_CAS_NAGRA_PINCODE_Set(const HCHAR *pszOldPinStr, const HCHAR *pszNewPinStr);
#if defined (CONFIG_OP_SES)
HBOOL		APK_CAS_NAGRA_UnlockAV(void);
#endif

#if defined(CONFIG_SUPPORT_NOVA_APP)
HERROR APK_CAS_NAGRA_PRM_GetCaInfo(HUINT8 *pucData);
HERROR APK_CAS_NAGRA_PRM_NovaDrmSetParameters(HUINT32 ulTransportSessionId
									, HUINT8 *pOldLicense, HUINT32 ulOldLicenseSize
									, HUINT32 ulCollectionId
									, HUINT32 ulDuration
									, HUINT8 *pUsageRules, HUINT32 ulUsageRulesLength);
HERROR APK_CAS_NAGRA_PRM_NovaDrmRelease(HUINT32 ulTransportSessionId, HUINT32 ulRecordHandle);
#if 1 //defined(TARA_SES_DEMO) // TARA & SES Demo onlystatic HUINT8 TempLicenseBuf[1024];
HERROR apk_cas_nagra_GetTempLicenseBuf(HUINT8 * pBuf, HUINT32 ulSize);
#endif
#endif
#endif // __APK_CAS_NAGRA_H__
