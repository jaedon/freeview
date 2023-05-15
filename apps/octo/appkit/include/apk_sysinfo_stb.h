#ifndef __APK_SYSINFO_STB_H__
#define __APK_SYSINFO_STB_H__

typedef void (*APK_SYSINFO_STB_Notifier_t) (void);

HERROR	APK_SYSINFO_STB_Init(void);
HERROR	APK_SYSINFO_STB_DeInit(void);
HERROR	APK_SYSINFO_STB_Initialized(void);
HERROR	APK_SYSINFO_STB_RegisterListener(APK_SYSINFO_STB_Notifier_t fnNotifier);
HERROR	APK_SYSINFO_STB_UnregisterListener(APK_SYSINFO_STB_Notifier_t fnNotifier);
HERROR	APK_SYSINFO_STB_GetModelName(HCHAR *pszModelName, HINT32 nMax);
HERROR	APK_SYSINFO_STB_GetUpdatedList(HCHAR *pszUpdatedList, HINT32 nMax);
HERROR	APK_SYSINFO_STB_GetSoftwareVersion(HCHAR *pszSoftwareVersion, HINT32 nMax);
HERROR	APK_SYSINFO_STB_GetPackageVersion(HCHAR *pszPackageVersion, HINT32 nMax);
HERROR	APK_SYSINFO_STB_GetLoaderName(HCHAR *pszLoaderName, HINT32 nMax);
HERROR	APK_SYSINFO_STB_GetLoaderVersion(HCHAR *pszLoaderVersion, HINT32 nMax);
HERROR	APK_SYSINFO_STB_GetUpgraderVersion(HCHAR *pszVersion, HINT32 nBufSize);
HERROR	APK_SYSINFO_STB_GetManufacturerName(HCHAR *pszManufacturerName, HINT32 nMax);
HERROR	APK_SYSINFO_STB_GetSystemId(HCHAR *pszSystemId, HINT32 nMax);
HERROR	APK_SYSINFO_STB_GetDeviceId(HCHAR *pszDeviceId, HINT32 nMax);
HERROR	APK_SYSINFO_STB_GetChipId(HCHAR *pszChipId, HINT32 nMax);
HERROR	APK_SYSINFO_STB_GetObamaVersion(HCHAR *pszObamaVersion, HINT32 nMax);
HERROR	APK_SYSINFO_STB_GetMicomVersion(HCHAR *pszMicomVersion, HINT32 nMax);
HERROR	APK_SYSINFO_STB_GetUpdateDate(HCHAR *pszUpdateDate, HINT32 nMax);
HERROR	APK_SYSINFO_STB_GetSerialNumber(HCHAR *pszSerialNo, HINT32 nMax);
HERROR	APK_SYSINFO_STB_GetCustomCode(HUINT32 *pulCustomCode);
HERROR	APK_SYSINFO_STB_GetColdBootDone(HBOOL *pbCheckColdBootDone, APK_SYSINFO_STB_Notifier_t fnNotifier);
HERROR	APK_SYSINFO_STB_SetFirstBootOnProduction(HBOOL bFirstBootOnProduction);
HERROR	APK_SYSINFO_STB_GetFirstBootOnProduction(HBOOL *pbFirstBootOnProduction);
HERROR	APK_SYSINFO_STB_GetLaunchAppMode(HUINT32 *pulLaunchAppMode);
HERROR 	APK_SYSINFO_STB_SetAntennaPower(HBOOL bAntPower);
HERROR 	APK_SYSINFO_STB_GetAntennaPower(HBOOL *pbAntPower);
HERROR 	APK_SYSINFO_STB_SetServiceUpdateFlag(HBOOL bOnOff);
HERROR	APK_SYSINFO_STB_GetServiceUpdateFlag(HBOOL *pbOnOff);
HERROR	APK_SYSINFO_STB_GetNandBadBlocks(HUINT32 *pulCntLdr, HUINT32 *pulCntUBI, HUINT32 *pulCntUBIFS);
HERROR	APK_SYSINFO_STB_GetFusingInfo(HUINT8 *pucSCS, HUINT8 *pucJTAG, HUINT8 *pucOTP, HUINT8 *pucCWE, HUINT32* pulMarketID);
HERROR	APK_SYSINFO_STB_GetFusingInfoExt(HUINT8 *pucSCS, HUINT8 *pucJTAG, HUINT8 *pucOTP, HUINT8 *pucCWE, HUINT32* pulOTPField1, HUINT32* pulOTPField2);

#if defined(CONFIG_NETFLIX_APP)
// for Netflix
HERROR	APK_SYSINFO_STB_SetNetflixESN(HCHAR *pszESN);
HERROR	APK_SYSINFO_STB_GetNetflixESN(HCHAR *pszESN, HINT32 nMax);
#endif

#endif // __APK_SYSINFO_STB_H__


