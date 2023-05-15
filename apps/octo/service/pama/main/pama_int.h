/**************************************************************
 *	@file		pama_int.h
 *	HumaxDataManager internal header for other applications.
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/12/26
 *	@author			humax
 *	@brief			pama internal header
 **************************************************************/
#ifndef	__PAMA_INT_H__
#define	__PAMA_INT_H__

#include	<papi.h>
#include	<../secure/include/se_papi.h>

/**************************************************************
	internal structures and variables
 **************************************************************/
//#define		PAMA_SIGNAL_UDP					"/tmp/.pama_signal.tmp"

#define		PAMA_WAITTIME_NORMAL			5000
#define		PAMA_WAITTIME_SHORT				3000
#define		PAMA_WAITTIME_LONG				30000

#define		PAMA_PROCESS_NAME_LEN			32

#define		PxDB_LAST_STANDBY				"LastStandby"
#define		PxDB_POWERSAVING_ON_STANDBY		"PowerSavingOnStandby"
#define		PxDB_SLEEPMODE_ON_STANDBY		"SleepModeOnStandby"
#define		PxDB_BOOTMODE_ON_STANDBY		"BootModeOnStandby"
#define		PxDB_WIFIPOWER_ON_STANDBY		"WifiPowerOnStandby"
#define		PxDB_MAINAPP_READY			"MainAppReady"

typedef enum
{
	ePAMA_CMD_RequestShutDown,
	ePAMA_CMD_RequestOperation,
	ePAMA_CMD_RequestReboot,
	ePAMA_CMD_WaitShutDown,
	ePAMA_CMD_WaitReboot,
	ePAMA_CMD_WaitSync,
	ePAMA_CMD_SyncDone,
	ePAMA_CMD_SetPowerSavingOnStandby,
	ePAMA_CMD_SetSleepModeOnStandby,
	ePAMA_CMD_SetBootModeOnStandby,
	ePAMA_CMD_GetWifiPowerOnStandby,

	ePAMA_CMD_Undefined,
} PamaSignal_e;

typedef struct
{
	PamaSignal_e	ePamaSignal;
	HUINT32			ulParam1;
	HCHAR			szProcessName[PAMA_PROCESS_NAME_LEN];
} PamaSignal_t;


#endif
