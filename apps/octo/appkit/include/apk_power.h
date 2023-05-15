#ifndef __APK_POWER_H__
#define __APK_POWER_H__

typedef	void	(*APK_POWER_Notifier_t)(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);

typedef enum
{
	eAPK_POWER_STANDBYTYPE_ASITWAS,
	eAPK_POWER_STANDBYTYPE_NORMAL,
	eAPK_POWER_STANDBYTYPE_POWERSAVING,
	eAPK_POWER_STANDBYTYPE_MAX
} APK_POWER_BoxStandbyType_e;


//PxSLEEP_MODE_e
typedef enum
{
	eAPK_POWER_POWERSTATE_IDLE,
	eAPK_POWER_POWERSTATE_LIGHT_SLEEP,		// s2 mode
	eAPK_POWER_POWERSTATE_DEEP_SLEEP,		// s3 mode
	eAPK_POWER_POWERSTATE_MAX
} APK_POWER_SLEEP_MODE_e;

//PxBOOT_MODE_e
typedef enum
{
	eAPK_POWER_BOOTMODE_WARM,
	eAPK_POWER_BOOTMODE_COLD,				// s2 에서는 warm 만..
	eAPK_POWER_BOOTMODE_MAX
} APK_POWER_BOOT_MODE_e;

//same at PxSTATUS_e in papi.h
typedef	enum
{
	eAPK_POWER_STATUS_Undefined = 0,
	eAPK_POWER_STATUS_Operation,
	eAPK_POWER_STATUS_Reboot,
	eAPK_POWER_STATUS_Shutdown,
	eAPK_POWER_STATUS_RebootDone,
	eAPK_POWER_STATUS_Sync,
	eAPK_POWER_STATUS_ShutdownDone,
	eAPK_POWER_STATUS_Unknown
} APK_POWER_STATUS_e;

typedef enum
{
	eAPK_POWER_BOXMODE_UNDEFINED,
	eAPK_POWER_BOXMODE_STANDBY,
	eAPK_POWER_BOXMODE_OPERATION,
	eAPK_POWER_BOXMODE_REBOOT,
	eAPK_POWER_BOXMODE_MAX
} APK_POWER_BOXMODE_e;


HERROR	APK_POWER_GetWakeupReason(DxWakeUpReason_e *peWakeupReason);
HERROR	APK_POWER_GetWakeupSecondReason(DxWakeUp2rdReason_e *peWakeupSecondReason);
HERROR	APK_POWER_SetStandbyPowerMode(APK_POWER_BoxStandbyType_e ePowerMode);
HERROR	APK_POWER_GetStandbyPowerMode(APK_POWER_BoxStandbyType_e *pePowerMode);
HBOOL	APK_POWER_IsStandbyMode(void);
HBOOL	APK_POWER_GoingToReboot(void);
HBOOL	APK_POWER_GoingToShutdown(void);
HBOOL	APK_POWER_GoingToOperate(void);
HERROR	APK_POWER_GetLastStandby(HBOOL *pLastStandby);
HERROR	APK_POWER_RequestShutDown(void);
HERROR	APK_POWER_RequestReboot(void);
HERROR	APK_POWER_RequestOperation(void);
HERROR	APK_POWER_WaitShutDown(HUINT32 ulWaitTime);
HERROR	APK_POWER_SetSleepModeOnStandby(APK_POWER_SLEEP_MODE_e eSleepMode);
HERROR	APK_POWER_GetSleepModeOnStandby(APK_POWER_SLEEP_MODE_e *pSleepMode);
HERROR	APK_POWER_SetBootModeOnStandby(APK_POWER_BOOT_MODE_e	eBootMode);
HERROR	APK_POWER_GetBootModeOnStandby(APK_POWER_BOOT_MODE_e *pBootMode);
HERROR	APK_POWER_SetWifiPowerOnStandby(HBOOL bOnOff);
HERROR	APK_POWER_GetWifiPowerOnStandby(HBOOL *pOnOff);
HERROR	APK_POWER_GetMainAppReady(HBOOL *pDone);
void	APK_POWER_RegisterNotifier(APK_POWER_Notifier_t pfnObjectHandler);

HERROR	APK_POWER_Init(void);


#endif // __APK_POWER_H__

