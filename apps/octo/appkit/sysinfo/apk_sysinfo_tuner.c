/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk_sysinfo_tuner.c
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
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
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

#include <oapi.h>

#include <apk.h>

#define ___INTERNAL_STRUCTURES___

#define ___INTERNAL_VARIABLES___
STATIC APK_SYSINFO_TUNER_Notifier_t	s_fnNotifier;

#define ___PRIVATE_FUNCTION___
#define _____NOTIFIER_FUNCTION_____
STATIC void	apk_sysinfo_tuner_onStatusChanged(void)
{
	if (s_fnNotifier)
	{
		APK_EVENT_EMIT_ARGS	stApkEvtArgs = {0,};

		stApkEvtArgs.pfnFreeFunc = NULL;
		stApkEvtArgs.ulArgCount = 1;
		stApkEvtArgs.apArgV[0] = (void *)s_fnNotifier;

		APK_EVENT_Emit(&stApkEvtArgs);
	}
}

#define ___MEMBER_FUNCTION___

#define ___API_FUNCTION___
HERROR	APK_SYSINFO_TUNER_Init(void)
{
	return OAPI_SYSINFO_TUNER_Init(apk_sysinfo_tuner_onStatusChanged);
}

HERROR	APK_SYSINFO_TUNER_DeInit(void)
{
	return OAPI_SYSINFO_TUNER_DeInit();
}

HERROR	APK_SYSINFO_TUNER_RegisterListener(APK_SYSINFO_TUNER_Notifier_t fnNotifier)
{
	s_fnNotifier = fnNotifier;

	return ERR_OK;
}

HERROR	APK_SYSINFO_TUNER_UnregisterListener(APK_SYSINFO_TUNER_Notifier_t fnNotifier)
{
	s_fnNotifier = NULL;

	return ERR_OK;
}

HERROR	APK_SYSINFO_TUNER_GetDeviceNum(HUINT32 *pulDevNum)
{
	return OAPI_SYSINFO_TUNER_GetDeviceNum(pulDevNum);
}

HERROR	APK_SYSINFO_TUNER_GetDevType(HINT32 nIndex, DxDeliveryType_e *peDeliType)
{
	return OAPI_SYSINFO_TUNER_GetDevType(nIndex, peDeliType);
}

HERROR	APK_SYSINFO_TUNER_IsLocked(HINT32 nIndex, HBOOL *pbLocked)
{
	return OAPI_SYSINFO_TUNER_IsLocked(nIndex, pbLocked);
}

HERROR	APK_SYSINFO_TUNER_GetSignalLevel(HINT32 nIndex, HINT32 *pnSignalLevel)
{
	HERROR	err;
	OxSysInfoTuner_SignalInfo_t	stSignalInfo;

	err = OAPI_SYSINFO_TUNER_GetSignalInfo(nIndex, &stSignalInfo);
	*pnSignalLevel = stSignalInfo.nSignalLevel;

	return err;
}

HERROR	APK_SYSINFO_TUNER_GetSignalQuality(HINT32 nIndex, HINT32 *pnSignalQuality)
{
	HERROR	err;
	OxSysInfoTuner_SignalInfo_t	stSignalInfo;

	err = OAPI_SYSINFO_TUNER_GetSignalInfo(nIndex, &stSignalInfo);
	*pnSignalQuality = stSignalInfo.nSignalQuality;

	return err;
}

HERROR	APK_SYSINFO_TUNER_GetAgc(HINT32 nIndex, HINT32 *pnAgc)
{
	HERROR	err;
	OxSysInfoTuner_SignalInfo_t	stSignalInfo;

	err = OAPI_SYSINFO_TUNER_GetSignalInfo(nIndex, &stSignalInfo);
	*pnAgc = stSignalInfo.nAgc;

	return err;
}

HERROR	APK_SYSINFO_TUNER_GetBer(HINT32 nIndex, HFLOAT32 *pfBer)
{
	HERROR	err;
	OxSysInfoTuner_SignalInfo_t	stSignalInfo;

	err = OAPI_SYSINFO_TUNER_GetSignalInfo(nIndex, &stSignalInfo);
	*pfBer = stSignalInfo.fBer;

	return err;
}

HERROR	APK_SYSINFO_TUNER_GetSnr(HINT32 nIndex, HFLOAT32 *pfSnr)
{
	HERROR	err;
	OxSysInfoTuner_SignalInfo_t	stSignalInfo;

	err = OAPI_SYSINFO_TUNER_GetSignalInfo(nIndex, &stSignalInfo);
	*pfSnr = stSignalInfo.fSnr;

	return err;
}

HERROR	APK_SYSINFO_TUNER_GetInputPower(HINT32 nIndex, HFLOAT32 *pfInputPower)
{
	HERROR	err;
	OxSysInfoTuner_SignalInfo_t	stSignalInfo;

	err = OAPI_SYSINFO_TUNER_GetSignalInfo(nIndex, &stSignalInfo);
	*pfInputPower = stSignalInfo.fInputPower;

	return err;
}

HERROR	APK_SYSINFO_TUNER_GetTuneParam(HINT32 nIndex, DxTuneParam_t *pstTuneParam)
{
	return OAPI_SYSINFO_TUNER_GetTuneParam(nIndex, pstTuneParam);
}

HERROR	APK_SYSINFO_TUNER_GetTunerIdByTripleId(HINT32 nOnId, HINT32 nTsId, HINT32 nSvcId, HINT32 *pnTunerId)
{
	return OAPI_SYSINFO_TUNER_GetTunerIdByTripleId(nOnId, nTsId, nSvcId, pnTunerId);
}

/* end of file */

