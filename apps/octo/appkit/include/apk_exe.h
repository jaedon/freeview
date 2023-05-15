#ifndef __APK_EXE_H__
#define __APK_EXE_H__
//////////////////////////////////////////////////////////////////////  BML
typedef void	(*APK_EXE_BATCH_Notifier_t)(DxBatchType_e eType, DxBatchProc_b eProc, HERROR eERR);
typedef void	(*APK_EXE_FACTORYDEFAULT_Notifier_t)(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);
typedef void	(*APK_EXE_BML_Notifier_t)(void *userdata, HCHAR	* pBMLName, HCHAR * pBMLParam);
typedef void	(*APK_EXE_RFVOD_Notifier_t)(HCHAR *appName);
typedef void	(*APK_EXE_MHEG_Notifier_t)(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);
typedef void	(*APK_EXE_ImExportDBNotifier_t)(HUINT32 type /* 1:import, 2: export*/, HUINT32 result /* 0:failed  1: successed*/);
typedef void	(*APK_EXE_ColdBootNotifier_t)(void);
typedef void	(*APK_EXE_CasActionIrEmmForcedOtaNotifier_t)(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);
typedef void	(*APK_EXE_LcnUpdateNotifier_t)(DxLcnUpdateInform_t *pstLcnInform);
typedef void	(*APK_EXE_TrdConflictNotifier_t)(DxTrdConflictData_t *pstTrdConflictInfo);
typedef void	(*APK_EXE_NcdInfoNotifier_t)(HCHAR *pNcdInfo);

///////////////////////////////////////////////////////////////////////////////////

// batch
HERROR APK_EXE_BATCH_Start(DxBatchType_e eType);
HERROR APK_EXE_BATCH_Install(HUINT32 ulCount,/*DxBatchType_e eType, DxBatchProc_b eProc,*/ ...);
HERROR APK_EXE_BATCH_RegisterNotifier(APK_EXE_BATCH_Notifier_t	fnNotifier);

/*
ulCount : DxBatchType_e[1] + DxBatchProc_b[n]
EXAMPLE :
	APK_EXE_BATCH_Install(3,eDxBAT_TYPE_STANDBY, eDxBAT_PROC_OTACHECK);
	APK_EXE_BATCH_Install(6,eDxBAT_TYPE_STANDBY, eDxBAT_PROC_OTACHECK);
*/

HERROR APK_EXE_BATCH_Stop(void);

// FACTORY DEFAULT
HERROR APK_EXE_FACTORYDEFAULT_RegisterNotifier(APK_EXE_FACTORYDEFAULT_Notifier_t	fnNotifier);


HERROR APK_EXE_FTP_Start(void);
HERROR APK_EXE_FTP_Stop(void);
HERROR APK_EXE_FTP_GetStatus(HBOOL *pbFtpRun);
HERROR APK_EXE_FTP_Init(void);

HERROR APK_EXE_StartFactoryDefault(DxFactoryDefault_type_e eFactoryDefaultMode);

HERROR APK_EXE_ExportDB(APK_EXE_ImExportDBNotifier_t fnNotifier, HBOOL bSync);
HERROR APK_EXE_ImportDB(APK_EXE_ImExportDBNotifier_t fnNotifier, HBOOL bSync);

HERROR APK_EXE_StartLcnUpdate(APK_EXE_LcnUpdateNotifier_t fnNotifier, HUINT32 ulSearchMode);
HERROR APK_EXE_StopLcnUpdate(APK_EXE_LcnUpdateNotifier_t fnNotifier);
HERROR APK_EXE_SaveLcnUpdate(APK_EXE_LcnUpdateNotifier_t fnNotifier, HUINT32 ulSearchMode, HUINT32 ulIndex);
HERROR APK_EXE_GetLcnUpdateInfo(APK_EXE_LcnUpdateNotifier_t fnNotifier);
HERROR APK_EXE_RemoveLcnUpdateInfo(HBOOL bSyncMode);

HERROR APK_EXE_MHEG_Start(HUINT32 nWndId, HUINT32 nStillWndId);
HERROR APK_EXE_MHEG_SetKey(HUINT32 key);
HERROR APK_EXE_MHEG_DeepLink(const HCHAR *pszURL, HUINT32 prevLcn);
HERROR APK_EXE_MHEG_ControlOP(HBOOL bControl);
HERROR APK_EXE_MHEG_RegisterNotifier(APK_EXE_MHEG_Notifier_t fnNotifier);

HERROR	APK_EXE_MHEGIB_RequestFile(HUINT32 ulSessionId, HINT32 nRequestNameLen, const HCHAR *pszRequestName);
HERROR	APK_EXE_MHEGIB_RequestData(HUINT32 ulSessionId, HINT32 nRequestNameLen, const HCHAR *pszRequestName);
HERROR	APK_EXE_MHEGIB_AbortByUser(HUINT32 ulSessionId);
HERROR	APK_EXE_MHEGIB_AbortBySystem(HUINT32 ulSessionId);
HERROR	APK_EXE_StartSatRecord(void);

HERROR APK_EXE_TVTVEPG_Initdirectory(void);
HERROR APK_EXE_TVTVEPG_Setting(HUINT32 ulEnable);
HERROR APK_EXE_Load_BaseEPG(void);
HERROR APK_EXE_Load_TVTVEPG(void);
HERROR APK_EXE_GetTrdConflictInfo(APK_EXE_TrdConflictNotifier_t fnNotifier,HUINT32 depth);
HERROR APK_EXE_SetTrdConflictInfo(HUINT32 depth, HUINT32 index);
HERROR APK_EXE_SetNcdNotifyData(HUINT32 ulASONotifyFlag);
HERROR APK_EXE_GetNcdInfo(APK_EXE_NcdInfoNotifier_t fnNotifier,HUINT32 param);

HERROR 	APK_EXE_Init(void);
#endif // __APK_EXE_H__

