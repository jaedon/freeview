/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk_power.c
	@brief

	Description:  									\n
	Module: APPKITv2			\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/
/*
 * (c) 2011-2013 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/
#include <vkernel.h>
#include <oapi.h>
#include <apk.h>
#include <apk_power.h>
#include <hapi.h>
#include <dapi.h>
#include <papi.h>

#include	"_apk_int.h"


#define ApkPowerTRACE()	do{printf("[%s]", HLIB_STD_GetCurProcessName());HxLOG_Trace();}while(0)

typedef struct
{
	DxWakeUpReason_e		eWakeupReason;
	DxWakeUp2rdReason_e		eWakeup2rdReason;
	APK_POWER_BOXMODE_e		eCurPowerMode;
	APK_POWER_BOXMODE_e		eLastRequestMode;
	APK_POWER_Notifier_t		cbApkPowerNotifier;
} apkPowerContext_t;

static apkPowerContext_t 	*apkPowerCtx = NULL;
static HxCnvtEnum32_t		 s_astApkPower_PowerStateCnvtTable[] = {
								{ ePAMA_POWERSTATE_IDLE,			eAPK_POWER_POWERSTATE_IDLE },
								{ ePAMA_POWERSTATE_LIGHT_SLEEP,		eAPK_POWER_POWERSTATE_LIGHT_SLEEP },
								{ ePAMA_POWERSTATE_DEEP_SLEEP,		eAPK_POWER_POWERSTATE_DEEP_SLEEP }
							};

static HxCnvtEnum32_t		 s_astApkPower_BootModeCnvtTable[] = {
								{ ePAMA_BOOTMODE_WARM,				eAPK_POWER_BOOTMODE_WARM },
								{ ePAMA_BOOTMODE_COLD,				eAPK_POWER_BOOTMODE_COLD }
							};

static apkPowerContext_t * apk_power_getCtx(void)
{
	if(!apkPowerCtx)
	{
		apkPowerCtx = (apkPowerContext_t *)APK_Calloc(sizeof(apkPowerContext_t));
		apkPowerCtx->eWakeupReason = eDxWAKEUP_NONE;
		apkPowerCtx->eWakeup2rdReason = eDxWAKEUP_2RD_UNDECIED;
		apkPowerCtx->eCurPowerMode = eAPK_POWER_BOXMODE_UNDEFINED;
		apkPowerCtx->cbApkPowerNotifier = NULL;
	}

	return apkPowerCtx;
}

#define ___PRIVATE_FUNCTION___

static HERROR	apk_power_convertPowerStatePama2Appkit (PxSLEEP_MODE_e ePxSleepMode, APK_POWER_SLEEP_MODE_e *peApkSleepMode)
{
	HUINT32				 ulSrcEnum = (HUINT32)ePxSleepMode;
	HUINT32				 ulDstEnum = 0;
	HUINT32				 ulTableSize = 0;
	HxCnvtEnum32_t		*pstCnvtTable = NULL;

	if (NULL == peApkSleepMode)
	{
		return ERR_FAIL;
	}

	ulTableSize = sizeof(s_astApkPower_PowerStateCnvtTable) / sizeof(HxCnvtEnum32_t);
	pstCnvtTable = s_astApkPower_PowerStateCnvtTable;

	if (HLIB_CONVERT_ConvertAtoB(pstCnvtTable, ulTableSize, ulSrcEnum, &ulDstEnum) != ERR_OK)
	{
		HxLOG_Error("PowerState (%d) Convert Failed:\n", ePxSleepMode);
		return ERR_FAIL;
	}

	*peApkSleepMode = (APK_POWER_SLEEP_MODE_e)ulDstEnum;
	return ERR_OK;
}

static HERROR	apk_power_convertPowerStateAppkit2Pama (APK_POWER_SLEEP_MODE_e eApkSleepMode, PxSLEEP_MODE_e *pePxSleepMode)
{
	HUINT32				 ulSrcEnum = (HUINT32)eApkSleepMode;
	HUINT32				 ulDstEnum = 0;
	HUINT32				 ulTableSize = 0;
	HxCnvtEnum32_t		*pstCnvtTable = NULL;

	if (NULL == pePxSleepMode)
	{
		return ERR_FAIL;
	}

	ulTableSize = sizeof(s_astApkPower_PowerStateCnvtTable) / sizeof(HxCnvtEnum32_t);
	pstCnvtTable = s_astApkPower_PowerStateCnvtTable;

	if (HLIB_CONVERT_ConvertBtoA(pstCnvtTable, ulTableSize, ulSrcEnum, &ulDstEnum) != ERR_OK)
	{
		HxLOG_Error("PowerState (%d) Convert Failed:\n", ulSrcEnum);
		return ERR_FAIL;
	}

	*pePxSleepMode = (PxSLEEP_MODE_e)ulDstEnum;
	return ERR_OK;
}

static HERROR	apk_power_convertBootModePama2Appkit (PxBOOT_MODE_e ePxBootMode, APK_POWER_BOOT_MODE_e *peApkBootMode)
{
	HUINT32				 ulSrcEnum = (HUINT32)ePxBootMode;
	HUINT32				 ulDstEnum = 0;
	HUINT32				 ulTableSize = 0;
	HxCnvtEnum32_t		*pstCnvtTable = NULL;

	if (NULL == peApkBootMode)
	{
		return ERR_FAIL;
	}

	ulTableSize = sizeof(s_astApkPower_BootModeCnvtTable) / sizeof(HxCnvtEnum32_t);
	pstCnvtTable = s_astApkPower_BootModeCnvtTable;

	if (HLIB_CONVERT_ConvertAtoB(pstCnvtTable, ulTableSize, ulSrcEnum, &ulDstEnum) != ERR_OK)
	{
		HxLOG_Error("PowerState (%d) Convert Failed:\n", ePxBootMode);
		return ERR_FAIL;
	}

	*peApkBootMode = (APK_POWER_BOOT_MODE_e)ulDstEnum;
	return ERR_OK;
}

static HERROR	apk_power_convertBootModeAppkit2Pama (APK_POWER_BOOT_MODE_e eApkBootMode, PxBOOT_MODE_e *pePxBootMode)
{
	HUINT32				 ulSrcEnum = (HUINT32)eApkBootMode;
	HUINT32				 ulDstEnum = 0;
	HUINT32				 ulTableSize = 0;
	HxCnvtEnum32_t		*pstCnvtTable = NULL;

	if (NULL == pePxBootMode)
	{
		return ERR_FAIL;
	}

	ulTableSize = sizeof(s_astApkPower_BootModeCnvtTable) / sizeof(HxCnvtEnum32_t);
	pstCnvtTable = s_astApkPower_BootModeCnvtTable;

	if (HLIB_CONVERT_ConvertBtoA(pstCnvtTable, ulTableSize, ulSrcEnum, &ulDstEnum) != ERR_OK)
	{
		HxLOG_Error("PowerState (%d) Convert Failed:\n", ulSrcEnum);
		return ERR_FAIL;
	}

	*pePxBootMode = (PxBOOT_MODE_e)ulDstEnum;
	return ERR_OK;
}

static HERROR	apk_power_onBootupStateChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HSystemBootupStatus_t	* pstBootupStatus = NULL;
	apkPowerContext_t 		* pwrCtx = apk_power_getCtx();

	pstBootupStatus = HxOBJECT_BIN_DATA(apArgv[0]);

	//printf("[%s][%s] BootUpReason[%d]\n", HLIB_STD_GetCurProcessName(), __FUNCTION__, pstBootupStatus->eBootUpReason);

	if(pwrCtx && pstBootupStatus)
	{
		switch(pstBootupStatus->eBootUpReason)
		{
			case eHBootUpReason_POWER:
				pwrCtx->eWakeupReason = eDxWAKEUP_ACPOWER;
				break;
			case eHBootUpReason_TIMER:
				pwrCtx->eWakeupReason = eDxWAKEUP_TIMER;
				break;
			case eHBootUpReason_USER:
				pwrCtx->eWakeupReason = eDxWAKEUP_KEY;
				break;
			case eHBootUpReason_UNKNOWN:
				pwrCtx->eWakeupReason = eDxWAKEUP_NONE;
				break;
			default :
				pwrCtx->eWakeupReason = eDxWAKEUP_NONE;
				break;
		}
	}

	return ERR_OK;
}

static HERROR	apk_power_onSystemStateChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HERROR		hErr = ERR_OK;
	PxSTATUS_e	ePapiStatus = ePAMA_STATUS_Undefined;
	APK_POWER_BOXMODE_e ePowerMode = eAPK_POWER_BOXMODE_UNDEFINED;
	apkPowerContext_t		* pwrCtx = apk_power_getCtx();

	HxLOG_Assert(nArgc == 1);

	ePapiStatus = HxOBJECT_INT(apArgv[0]);
	HxLOG_Print("[%s][%s] ePapiStatus[%d]\n",HLIB_STD_GetCurProcessName(), __FUNCTION__, ePapiStatus);
	ePowerMode = pwrCtx->eCurPowerMode;

	switch(ePapiStatus)
	{
		case ePAMA_STATUS_Operation :
			pwrCtx->eLastRequestMode = pwrCtx->eCurPowerMode = eAPK_POWER_BOXMODE_OPERATION;
			break;
		case ePAMA_STATUS_Shutdown :
#if defined(CONFIG_OP_TDC_NETWORK_LED)
			hErr = APK_OUTPUT_PANEL_SetRgbLedOff(eDxLED_ID_RGB_NET);
			if(hErr != ERR_OK)
			{
				HxLOG_Debug("APK_OUTPUT_PANEL_SetRgbLedOff is Failed !!! \n");
			}
#endif
			pwrCtx->eLastRequestMode = pwrCtx->eCurPowerMode = eAPK_POWER_BOXMODE_STANDBY;
			break;
		case ePAMA_STATUS_Reboot :
		case ePAMA_STATUS_RebootDone :
			pwrCtx->eLastRequestMode = eAPK_POWER_BOXMODE_REBOOT;
			break;
		case ePAMA_STATUS_ShutdownDone :
		default :
			break;
	}

	//if(ePapiStatus == ePAMA_STATUS_Operation || ePapiStatus == ePAMA_STATUS_Shutdown)
	{
		//if(pwrCtx->eCurPowerMode != ePowerMode)
		{
			HxLOG_Print("[%s][%s] SendPower Notifier[%d]\n",HLIB_STD_GetCurProcessName(), __FUNCTION__, pwrCtx->eCurPowerMode);

			if(pwrCtx->cbApkPowerNotifier)
			{
				APK_EVENT_EMIT_ARGS	stApkEvtArgs = {0,};

				HxLOG_Print("[%s][%s] Real SendPower Notifier[%d]\n",HLIB_STD_GetCurProcessName(), __FUNCTION__, pwrCtx->eCurPowerMode);

				stApkEvtArgs.pfnFreeFunc = NULL;
				stApkEvtArgs.ulArgCount = 3;
				stApkEvtArgs.apArgV[0] = (void *)pwrCtx->cbApkPowerNotifier;
				stApkEvtArgs.apArgV[1] = (void *)pwrCtx->eCurPowerMode;
				stApkEvtArgs.apArgV[2] = (void *)pwrCtx->eLastRequestMode;
				APK_EVENT_Emit(&stApkEvtArgs);
			}
		}
	}

	return hErr;
}


HERROR	APK_POWER_GetWakeupReason(DxWakeUpReason_e *peWakeupReason)
{
	apkPowerContext_t		* pwrCtx = apk_power_getCtx();
	*peWakeupReason = pwrCtx->eWakeupReason;
	return ERR_OK;
}

HERROR	APK_POWER_GetWakeupSecondReason(DxWakeUp2rdReason_e *peWakeupSecondReason)
{
	apkPowerContext_t		* pwrCtx = apk_power_getCtx();
	*peWakeupSecondReason = pwrCtx->eWakeup2rdReason;
	return ERR_OK;
}



HERROR	APK_POWER_SetStandbyPowerMode(APK_POWER_BoxStandbyType_e ePowerMode)
{
	if (ePowerMode == eAPK_POWER_STANDBYTYPE_NORMAL)
		PAPI_SetPowerSavingOnStandby(FALSE);
	else if(ePowerMode == eAPK_POWER_STANDBYTYPE_POWERSAVING)
		PAPI_SetPowerSavingOnStandby(TRUE);

	return ERR_OK;
}

HERROR	APK_POWER_GetStandbyPowerMode(APK_POWER_BoxStandbyType_e *pePowerMode)
{
	HBOOL nValue = FALSE;

	if(PAPI_GetPowerSavingOnStandby(&nValue) != ERR_FAIL)
	{
		if(nValue == FALSE)
			*pePowerMode = eAPK_POWER_STANDBYTYPE_NORMAL;
		else
			*pePowerMode = eAPK_POWER_STANDBYTYPE_POWERSAVING;
	}
	else
	{
		*pePowerMode = eAPK_POWER_STANDBYTYPE_NORMAL;
	}
	return ERR_OK;
}

HBOOL	APK_POWER_IsStandbyMode(void)
{
	apkPowerContext_t		* pwrCtx = apk_power_getCtx();
	if(pwrCtx->eCurPowerMode == eAPK_POWER_BOXMODE_STANDBY)
		return TRUE;
	else
		return FALSE;
}

HBOOL APK_POWER_GoingToReboot(void)
{
	apkPowerContext_t		* pwrCtx = apk_power_getCtx();
	if(pwrCtx->eLastRequestMode == eAPK_POWER_BOXMODE_REBOOT)
		return TRUE;
	else
		return FALSE;
}

HBOOL APK_POWER_GoingToShutdown(void)
{
	apkPowerContext_t		* pwrCtx = apk_power_getCtx();
	if(pwrCtx->eLastRequestMode == eAPK_POWER_BOXMODE_STANDBY)
		return TRUE;
	else
		return FALSE;
}

HBOOL APK_POWER_GoingToOperate(void)
{
	apkPowerContext_t		* pwrCtx = apk_power_getCtx();
	if(pwrCtx->eLastRequestMode == eAPK_POWER_BOXMODE_OPERATION)
		return TRUE;
	else
		return FALSE;
}

HERROR	APK_POWER_GetLastStandby(HBOOL *pLastStandby)
{
	ApkPowerTRACE();
	return PAPI_GetLastStandby(pLastStandby);
}

HERROR	APK_POWER_RequestShutDown(void)
{
	ApkPowerTRACE();
	return PAPI_RequestShutDown();
}

HERROR	APK_POWER_RequestReboot(void)
{
	ApkPowerTRACE();
	return PAPI_RequestReboot();
}

HERROR	APK_POWER_RequestOperation(void)
{
	ApkPowerTRACE();
	return PAPI_RequestOperation();
}

HERROR	APK_POWER_WaitShutDown(HUINT32 ulWaitTime)
{
	ApkPowerTRACE();
	return PAPI_WaitForShutDown(ulWaitTime);
}

HERROR	APK_POWER_SetSleepModeOnStandby(APK_POWER_SLEEP_MODE_e eSleepMode)
{
	PxSLEEP_MODE_e	 ePxSleepMode = ePAMA_POWERSTATE_IDLE;

	ApkPowerTRACE();

	// APPKIT -> PAPI
	if (apk_power_convertPowerStateAppkit2Pama(eSleepMode, &ePxSleepMode) != ERR_OK)
	{
		HxLOG_Error("APK_POWER_SLEEP_MODE_e %d not converted:\n", eSleepMode);
		return ERR_FAIL;
	}

	return PAPI_SetSleepModeOnStandby(ePxSleepMode);
}

HERROR	APK_POWER_GetSleepModeOnStandby(APK_POWER_SLEEP_MODE_e *pSleepMode)
{
	PxSLEEP_MODE_e	 ePxSleepMode = ePAMA_POWERSTATE_IDLE;

	ApkPowerTRACE();

	if (PAPI_GetSleepModeOnStandby(&ePxSleepMode) != ERR_OK)
	{
		HxLOG_Error("PAPI_GetSleepModeOnStandby err:\n");
		return ERR_FAIL;
	}

	// PAPI -> APPKIT
	return apk_power_convertPowerStatePama2Appkit(ePxSleepMode, pSleepMode);
}

HERROR	APK_POWER_SetBootModeOnStandby(APK_POWER_BOOT_MODE_e	eBootMode)
{
	PxBOOT_MODE_e	 ePxBootMode = ePAMA_BOOTMODE_COLD;

	ApkPowerTRACE();

	// APPKIT -> PAPI
	if (apk_power_convertBootModeAppkit2Pama(eBootMode, &ePxBootMode) != ERR_OK)
	{
		HxLOG_Error("APK_POWER_BOOT_MODE_e %d not converted:\n", eBootMode);
		return ERR_FAIL;
	}

	return PAPI_SetBootModeOnStandby(ePxBootMode);
}

HERROR	APK_POWER_GetBootModeOnStandby(APK_POWER_BOOT_MODE_e *pBootMode)
{
	PxBOOT_MODE_e	 ePxBootMode = ePAMA_BOOTMODE_COLD;

	ApkPowerTRACE();

	if (PAPI_GetBootModeOnStandby(&ePxBootMode) != ERR_OK)
	{
		HxLOG_Error("PAPI_GetBootModeOnStandby err:\n");
		return ERR_FAIL;
	}

	// PAPI -> APPKIT
	return apk_power_convertBootModePama2Appkit(ePxBootMode, pBootMode);
}

HERROR	APK_POWER_SetWifiPowerOnStandby(HBOOL bOnOff)
{
	ApkPowerTRACE();
	return PAPI_SetWifiPowerOnStandby(bOnOff);
}
HERROR	APK_POWER_GetWifiPowerOnStandby(HBOOL *pOnOff)
{
	ApkPowerTRACE();
	return PAPI_GetWifiPowerOnStandby(pOnOff);
}

HERROR	APK_POWER_GetMainAppReady(HBOOL *pDone)
{
	ApkPowerTRACE();
	return PAPI_GetMainAppReady(pDone);
}


void	APK_POWER_RegisterNotifier(APK_POWER_Notifier_t pfnObjectHandler)
{
	apkPowerContext_t * pwrCtx = apk_power_getCtx();
	ApkPowerTRACE();

	if(pwrCtx)
	{
		APK_POWER_BoxStandbyType_e ePowerMode;
		APK_POWER_GetStandbyPowerMode(&ePowerMode);		// opl에서 noti를 받을 경우 db change가 이루어지게 함. 다른 곳으로 옮겨야 하는데.

		pwrCtx->cbApkPowerNotifier = pfnObjectHandler;

		if(pwrCtx->eCurPowerMode != eAPK_POWER_BOXMODE_UNDEFINED)
		{
			APK_EVENT_EMIT_ARGS	stApkEvtArgs = {0,};

			HxLOG_Print("[%s][%s] Already Changed PowerStatus Send Noti to Corsair[%d]\n",HLIB_STD_GetCurProcessName(), __FUNCTION__, pwrCtx->eCurPowerMode);

			stApkEvtArgs.pfnFreeFunc = NULL;
			stApkEvtArgs.ulArgCount = 3;
			stApkEvtArgs.apArgV[0] = (void *)pwrCtx->cbApkPowerNotifier;
			stApkEvtArgs.apArgV[1] = (void *)pwrCtx->eCurPowerMode;
			stApkEvtArgs.apArgV[2] = (void *)pwrCtx->eLastRequestMode;
			APK_EVENT_Emit(&stApkEvtArgs);
		}
	}
}

HERROR	APK_POWER_Init(void)
{
	apk_power_getCtx();
	HxLOG_Print("[%s][%s][%s]\n",HLIB_STD_GetCurProcessName(), __FUNCTION__, HLIB_STD_GetCurProcessName());

	HAPI_ConnectSignal("signal:onBootupStateChanged",	NULL, apk_power_onBootupStateChanged);
	HAPI_ConnectSignal("signal:onSystemStateChanged",	NULL, apk_power_onSystemStateChanged);

	return ERR_OK;
}




/* end of file */
