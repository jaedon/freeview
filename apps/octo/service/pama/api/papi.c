/**************************************************************
 *	@file		papi.c
 *	HumaxDataManager for OCTO 2.0
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/03/01
 *	@author			humax
 *	@brief			HumaxDataManager for OCTO 2.0 Project
 **************************************************************/
#include <hlib.h>
#include <hapi.h>
#include <pama_int.h>


 /**************************************************************
	 internal structures and variables
  **************************************************************/
static HBOOL			 _isInitialized = FALSE;
static HINT32			_clientSocketInfo = 0;
static HINT32			_RpcHandle = 0;

#define CHECK_MODULE_INIT()		 do{if (_isInitialized == FALSE){ if(initializeCore() == ERR_FAIL) return ERR_FAIL;}}while(0)

static HERROR	initializeCore(void)
{
	_clientSocketInfo	= HLIB_EXTRA_OpenClient("pama");//HLIB_SOCKET_OpenClient(HxSOCKET_UDS, PAMA_SIGNAL_UDP, 0);
	if(_clientSocketInfo)
		_RpcHandle	= HLIB_RPC_Open(_clientSocketInfo, HLIB_STD_GetPID());
	if (_clientSocketInfo == 0 || _RpcHandle == 0)		return ERR_FAIL;


	HxLOG_Warning("** (%s) is connected to PAMA\n", HLIB_STD_GetCurProcessName());

	_isInitialized = TRUE;
	return ERR_OK;
}

static HERROR  PxRPC_Method(PamaSignal_e eSignal, HUINT32 ulTime)
{
	HINT32 ret = 0;
	CHECK_MODULE_INIT();
	ret = HLIB_RPC_Call(_RpcHandle, "pama.setCommand", "iis", eSignal, ulTime, HLIB_STD_GetCurProcessName());

	if(ret == -1)
		return ERR_FAIL;
	return ERR_OK;
}

HERROR  PAPI_WaitForReboot(HUINT32 ulWaitTime)
{
	HxLOG_Trace();
	if (ulWaitTime < PAMA_WAITTIME_SHORT)
		ulWaitTime = PAMA_WAITTIME_SHORT;

	return PxRPC_Method(ePAMA_CMD_WaitReboot, ulWaitTime);
}


HERROR  PAPI_WaitForShutDown(HUINT32 ulWaitTime)
{
	HxLOG_Trace();
	if (ulWaitTime < PAMA_WAITTIME_SHORT)
		ulWaitTime = PAMA_WAITTIME_SHORT;
	else if(ulWaitTime > PAMA_WAITTIME_LONG)
		ulWaitTime = PAMA_WAITTIME_LONG;

	return PxRPC_Method(ePAMA_CMD_WaitShutDown, ulWaitTime);

}

HERROR  PAPI_WaitForSync(HUINT32 ulWaitTime)
{
	HxLOG_Trace();
	if (ulWaitTime < PAMA_WAITTIME_SHORT)
		ulWaitTime = PAMA_WAITTIME_SHORT;
	else if(ulWaitTime > PAMA_WAITTIME_LONG)
		ulWaitTime = PAMA_WAITTIME_LONG;

	return PxRPC_Method(ePAMA_CMD_WaitSync, ulWaitTime);
}

HERROR  PAPI_SyncDone(void)
{
	HxLOG_Trace();
	return PxRPC_Method(ePAMA_CMD_SyncDone, 0);
}

HERROR	PAPI_RequestShutDown(void)
{
	HxLOG_Trace();
	return PxRPC_Method(ePAMA_CMD_RequestShutDown, 0);
}

HERROR	PAPI_RequestOperation(void)
{
	HxLOG_Trace();
	return PxRPC_Method(ePAMA_CMD_RequestOperation, 0);
}

HERROR	PAPI_RequestReboot(void)
{
	HxLOG_Trace();
	return PxRPC_Method(ePAMA_CMD_RequestReboot, 0);
}

HERROR	PAPI_SetPowerSavingOnStandby(HBOOL bEnable)
{
	HxLOG_Print(HxANSI_COLOR_YELLOW("[SET] Power Saving on Standby : %s\n"), TRUE == bEnable ? "On":"Off");
	HxLOG_Trace();
	return PxRPC_Method(ePAMA_CMD_SetPowerSavingOnStandby, bEnable);
}
HERROR	PAPI_GetPowerSavingOnStandby(HBOOL *pEnable)
{
	return HAPI_GetRegistryInt(PxDB_POWERSAVING_ON_STANDBY, pEnable);
}

HERROR	PAPI_GetLastStandby(HINT32 *pStandby)
{
	return HAPI_GetRegistryInt(PxDB_LAST_STANDBY,pStandby);
}

HERROR	PAPI_SetSleepModeOnStandby(PxSLEEP_MODE_e eSleepMode)
{
	HCHAR	*szSleepMode = NULL;
	switch(eSleepMode)
	{
	case ePAMA_POWERSTATE_IDLE:				szSleepMode = "Idle";	break;
	case ePAMA_POWERSTATE_LIGHT_SLEEP:		szSleepMode = "S2";		break;
	case ePAMA_POWERSTATE_DEEP_SLEEP:		szSleepMode = "S3";		break;
	default:								szSleepMode = "??";		break;
	}
	HxLOG_Print(HxANSI_COLOR_YELLOW("[SET] Sleep Mode : %s\n"), szSleepMode);

	HxLOG_Trace();
	return PxRPC_Method(ePAMA_CMD_SetSleepModeOnStandby, eSleepMode);
}
HERROR	PAPI_GetSleepModeOnStandby(PxSLEEP_MODE_e *pSleepMode)
{
	return HAPI_GetRegistryInt(PxDB_SLEEPMODE_ON_STANDBY,pSleepMode);
}

HERROR	PAPI_SetBootModeOnStandby(PxBOOT_MODE_e	eBootMode)
{
	HxLOG_Print(HxANSI_COLOR_YELLOW("[SET] Boot Mode : %s\n")
		, (ePAMA_BOOTMODE_WARM == eBootMode) ? "Warm" :
				((ePAMA_BOOTMODE_COLD == eBootMode) ? "Cold" : "??"));

	HxLOG_Trace();
	return PxRPC_Method(ePAMA_CMD_SetBootModeOnStandby, eBootMode);
}

HERROR	PAPI_GetBootModeOnStandby(PxBOOT_MODE_e *pBootMode)
{
	return HAPI_GetRegistryInt(PxDB_BOOTMODE_ON_STANDBY,pBootMode);
}

HERROR	PAPI_SetWifiPowerOnStandby(HBOOL bOnOff)
{
	HxLOG_Print(HxANSI_COLOR_YELLOW("[SET] Wifi : %s\n"), TRUE == bOnOff ? "On":"Off");

	HxLOG_Trace();
	return PxRPC_Method(ePAMA_CMD_GetWifiPowerOnStandby, bOnOff);
}

HERROR	PAPI_GetWifiPowerOnStandby(HBOOL *pOnOff)
{
	return HAPI_GetRegistryInt(PxDB_WIFIPOWER_ON_STANDBY,pOnOff);
}

HERROR	PAPI_GetMainAppReady(HBOOL *pDone)
{
	return HAPI_GetRegistryInt(PxDB_MAINAPP_READY,pDone);
}


