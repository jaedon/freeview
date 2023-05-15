/**************************************************************
 *	@file		papi.h
 *	HumaxDataManager global header for other applications.
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/03/19
 *	@author			humax
 *	@brief			pama global header
 **************************************************************/
#ifndef	__PAPI_H__
#define	__PAPI_H__

#ifdef __cplusplus
extern "C" {
#endif
#include <htype.h>
#include <hlib.h>


#define	PxDEFAULT_WAIT		2000

typedef	enum
{
	ePAMA_STATUS_Undefined = 0,
	ePAMA_STATUS_Operation,
	ePAMA_STATUS_Reboot,				//	just for request
	ePAMA_STATUS_Shutdown,				//	just for request
	ePAMA_STATUS_RebootDone,			//	replay to obama		it will be deleted after moved di reboot function
	ePAMA_STATUS_Sync,
	ePAMA_STATUS_ShutdownDone,			//	replay to obama		it will be deleted after moved di shutdown function
	ePAMA_STATUS_Unknown
} PxSTATUS_e;

typedef enum
{
	ePAMA_POWERSTATE_IDLE,
	ePAMA_POWERSTATE_LIGHT_SLEEP,		// s2 mode
	ePAMA_POWERSTATE_DEEP_SLEEP,		// s3 mode
	ePAMA_POWERSTATE_MAX
} PxSLEEP_MODE_e;

typedef enum
{
	ePAMA_BOOTMODE_WARM,
	ePAMA_BOOTMODE_COLD,				// s2 에서는 warm 만..
	ePAMA_BOOTMODE_MAX
} PxBOOT_MODE_e;


extern HERROR	PAPI_WaitForReboot(HUINT32 ulWaitTime);
extern HERROR	PAPI_WaitForShutDown(HUINT32 ulWaitTime);
extern HERROR 	PAPI_WaitForSync(HUINT32 ulWaitTime);
extern HERROR 	PAPI_SyncDone(void);


extern HERROR	PAPI_RequestShutDown(void);
extern HERROR	PAPI_RequestOperation(void);
extern HERROR	PAPI_RequestReboot(void);

extern HERROR	PAPI_GetLastStandby(HINT32 *pStandby);

extern HERROR	PAPI_SetPowerSavingOnStandby(HBOOL bEnable);
extern HERROR	PAPI_GetPowerSavingOnStandby(HBOOL *pEnable);

extern HERROR	PAPI_SetSleepModeOnStandby(PxSLEEP_MODE_e eSleepMode);
extern HERROR	PAPI_GetSleepModeOnStandby(PxSLEEP_MODE_e *pSleepMode);

extern HERROR	PAPI_SetBootModeOnStandby(PxBOOT_MODE_e	eBootMode);
extern HERROR	PAPI_GetBootModeOnStandby(PxBOOT_MODE_e *pBootMode);

extern HERROR	PAPI_SetWifiPowerOnStandby(HBOOL bOnOff);
extern HERROR	PAPI_GetWifiPowerOnStandby(HBOOL *pOnOff);

extern HERROR	PAPI_GetMainAppReady(HBOOL *pDone);

#ifdef __cplusplus
}
#endif

#endif
