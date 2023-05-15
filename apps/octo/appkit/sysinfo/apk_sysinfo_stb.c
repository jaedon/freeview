/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk_sysinfo_stb.c
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
* MS949 encoding ?» »ç¿ëÇÏ´Â source file ?Ô´Ï´Ù.
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
STATIC APK_SYSINFO_STB_Notifier_t	s_fnNotifier;

#define ___PRIVATE_FUNCTION___
#define _____NOTIFIER_FUNCTION_____
STATIC void	apk_sysinfo_stb_onStatusChanged(void)
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
HERROR	APK_SYSINFO_STB_Init(void)
{
	return OAPI_SYSINFO_STB_Init(apk_sysinfo_stb_onStatusChanged);
}

HERROR	APK_SYSINFO_STB_DeInit(void)
{
	return OAPI_SYSINFO_STB_DeInit();
}

HERROR	APK_SYSINFO_STB_Initialized(void)
{
	return OAPI_SYSINFO_Initialized();
}

HERROR	APK_SYSINFO_STB_RegisterListener(APK_SYSINFO_STB_Notifier_t fnNotifier)
{
	if (NULL == fnNotifier)
	{
		HxLOG_Error("[%s:%d] fnNotifier is NULL \n", __FUNCTION__,__LINE__);
		return ERR_FAIL;
	}
	else
	{
		s_fnNotifier = fnNotifier;
	}

	return ERR_OK;
}

HERROR	APK_SYSINFO_STB_UnregisterListener(APK_SYSINFO_STB_Notifier_t fnNotifier)
{
	fnNotifier = NULL;

	return ERR_OK;
}

HERROR	APK_SYSINFO_STB_GetModelName(HCHAR *pszModelName, HINT32 nMax)
{
	return OAPI_SYSINFO_STB_GetModelName(pszModelName, nMax);
}

HERROR	APK_SYSINFO_STB_GetUpdatedList(HCHAR *pszUpdatedList, HINT32 nMax)
{
	return OAPI_SYSINFO_STB_GetUpdatedList(pszUpdatedList, nMax);
}

HERROR	APK_SYSINFO_STB_GetSoftwareVersion(HCHAR *pszSoftwareVersion, HINT32 nMax)
{
	return OAPI_SYSINFO_STB_GetSoftwareVersion(pszSoftwareVersion, nMax);
}

HERROR	APK_SYSINFO_STB_GetPackageVersion(HCHAR *pszPackageVersion, HINT32 nMax)
{
	return OAPI_SYSINFO_STB_GetPackageVersion(pszPackageVersion, nMax);
}

HERROR	APK_SYSINFO_STB_GetLoaderName(HCHAR *pszLoaderName, HINT32 nMax)
{
	return OAPI_SYSINFO_STB_GetLoaderName(pszLoaderName, nMax);
}

HERROR	APK_SYSINFO_STB_GetLoaderVersion(HCHAR *pszLoaderVersion, HINT32 nMax)
{
	return OAPI_SYSINFO_STB_GetLoaderVersion(pszLoaderVersion, nMax);
}

HERROR	APK_SYSINFO_STB_GetUpgraderVersion(HCHAR *pszVersion, HINT32 nBufSize)
{
	return OAPI_SYSINFO_STB_GetUpgraderVersion(pszVersion, nBufSize);
}

HERROR	APK_SYSINFO_STB_GetManufacturerName(HCHAR *pszManufacturerName, HINT32 nMax)
{
	return OAPI_SYSINFO_STB_GetManufacturerName(pszManufacturerName, nMax);
}

HERROR	APK_SYSINFO_STB_GetSystemId(HCHAR *pszSystemId, HINT32 nMax)
{
	return OAPI_SYSINFO_STB_GetSystemId(pszSystemId, nMax);
}

HERROR	APK_SYSINFO_STB_GetDeviceId(HCHAR *pszDeviceId, HINT32 nMax)
{
	return OAPI_SYSINFO_STB_GetDeviceId(pszDeviceId, nMax);
}

HERROR	APK_SYSINFO_STB_GetChipId(HCHAR *pszChipId, HINT32 nMax)
{
	return OAPI_SYSINFO_STB_GetChipId(pszChipId, nMax);
}

HERROR	APK_SYSINFO_STB_GetObamaVersion(HCHAR *pszObamaVersion, HINT32 nMax)
{
	return OAPI_SYSINFO_STB_GetObamaVersion(pszObamaVersion, nMax);
}

HERROR	APK_SYSINFO_STB_GetMicomVersion(HCHAR *pszMicomVersion, HINT32 nMax)
{
	return OAPI_SYSINFO_STB_GetMicomVersion(pszMicomVersion, nMax);
}

HERROR	APK_SYSINFO_STB_GetUpdateDate(HCHAR *pszUpdateDate, HINT32 nMax)
{
	return OAPI_SYSINFO_STB_GetUpdateDate(pszUpdateDate, nMax);
}

HERROR	APK_SYSINFO_STB_GetSerialNumber(HCHAR *pszSerialNo, HINT32 nMax)
{
	return OAPI_SYSINFO_STB_GetSerialNo(pszSerialNo, nMax);
}

HERROR	APK_SYSINFO_STB_GetCustomCode(HUINT32 *pulCustomCode)
{
	OAPI_SYSINFO_STB_Refresh(TRUE);
	return OAPI_SYSINFO_STB_GetCustomCode(pulCustomCode);
}

HERROR	APK_SYSINFO_STB_GetColdBootDone(HBOOL *pbCheckColdBootDone, APK_SYSINFO_STB_Notifier_t fnNotifier)
{
	if (pbCheckColdBootDone != NULL)
	{
		OAPI_SYSINFO_STB_Refresh(TRUE);
		OAPI_SYSINFO_STB_GetColdBootDone(pbCheckColdBootDone);
	}

	if (NULL == fnNotifier)
	{
		HxLOG_Error("[%s:%d] fnNotifier is NULL \n", __FUNCTION__,__LINE__);
		return ERR_FAIL;
	}
	else
	{
		s_fnNotifier = fnNotifier;
	}

	return ERR_OK;
}

HERROR		APK_SYSINFO_STB_SetFirstBootOnProduction(HBOOL bFirstBootOnProduction)
{
	HxLOG_Debug("APK_SYSINFO_STB_SetFirstBootOnProduction: %d \n", bFirstBootOnProduction);
	return OAPI_SYSINFO_STB_SetFirstBootOnProduction(bFirstBootOnProduction);
}

HERROR		APK_SYSINFO_STB_GetFirstBootOnProduction(HBOOL *pbFirstBootOnProduction)
{
	HERROR	hErr = ERR_FAIL;
	HBOOL	bFirstBootOnProduction = FALSE;

	if (pbFirstBootOnProduction == NULL)
		return hErr;

	hErr = OAPI_SYSINFO_STB_GetFirstBootOnProduction(&bFirstBootOnProduction);
	*pbFirstBootOnProduction = bFirstBootOnProduction;

	HxLOG_Debug("APK_SYSINFO_STB_GetFirstBootOnProduction: %d \n", *pbFirstBootOnProduction);

	return hErr;
}

HERROR		APK_SYSINFO_STB_GetLaunchAppMode(HUINT32 *pulLaunchAppMode)
{
	HERROR	hErr = ERR_FAIL;
	HUINT32 ulLaunchAppMode = 0;

	if (pulLaunchAppMode == NULL)
		return hErr;

	hErr = OAPI_SYSINFO_STB_GetLaunchAppMode(&ulLaunchAppMode);
	*pulLaunchAppMode = ulLaunchAppMode;

	HxLOG_Debug("APK_SYSINFO_STB_GetLaunchAppMode: %d \n", *pulLaunchAppMode);

	return hErr;
}

HERROR		APK_SYSINFO_STB_SetAntennaPower(HBOOL bAntPower)
{
	HxLOG_Debug("APK_SYSINFO_STB_SetAntennaPower: %d \n", bAntPower);
	return OAPI_SYSINFO_STB_SetAntennaPower(bAntPower);
}

HERROR		APK_SYSINFO_STB_GetAntennaPower(HBOOL *pbAntPower)
{
	HERROR	hErr = ERR_FAIL;
	HBOOL	bAntennaPower = FALSE;

	if (pbAntPower == NULL)
		return hErr;

  	hErr = OAPI_SYSINFO_STB_GetAntennaPower(&bAntennaPower);
	*pbAntPower = bAntennaPower;

	HxLOG_Debug("APK_SYSINFO_STB_GetAntennaPower: %d \n", *pbAntPower);

	return hErr;
}

HERROR		APK_SYSINFO_STB_SetServiceUpdateFlag(HBOOL bOnOff)
{
	HxLOG_Print(HxANSI_COLOR_YELLOW("APK_SYSINFO_STB_SetServiceUpdateFlag: %d \n"), bOnOff);
	return OAPI_SYSINFO_STB_SetServiceUpdateFlag(bOnOff);
}

HERROR		APK_SYSINFO_STB_GetServiceUpdateFlag(HBOOL *pbOnOff)
{
	HERROR	hErr = ERR_FAIL;
	HBOOL	bSvcUpdateFlag = FALSE;

	if (pbOnOff == NULL)
		return hErr;

  	hErr = OAPI_SYSINFO_STB_GetServiceUpdateFlag(&bSvcUpdateFlag);
	*pbOnOff = bSvcUpdateFlag;

	HxLOG_Debug("APK_SYSINFO_STB_GetServiceUpdateFlag: %d \n", *pbOnOff);

	return hErr;
}

HERROR		APK_SYSINFO_STB_GetNandBadBlocks(HUINT32 *pulCntLdr, HUINT32 *pulCntUBI, HUINT32 *pulCntUBIFS)
{
	HERROR	hErr = ERR_FAIL;

	if (pulCntLdr == NULL || pulCntUBI == NULL || pulCntUBIFS == NULL)
		return hErr;

  	hErr = OAPI_SYSINFO_STB_GetNandBadBlocks(pulCntLdr, pulCntUBI, pulCntUBIFS);

	HxLOG_Debug("APK_SYSINFO_STB_GetNandBadBlocks: %d %d %d \n", *pulCntLdr, *pulCntUBI, *pulCntUBIFS);

	return hErr;
}

HERROR		APK_SYSINFO_STB_GetFusingInfo(HUINT8 *pucSCS, HUINT8 *pucJTAG, HUINT8 *pucOTP, HUINT8 *pucCWE, HUINT32* pulMarketID)
{
	HERROR	hErr = ERR_FAIL;

	if (pucSCS == NULL || pucJTAG == NULL || pucOTP == NULL || pucCWE == NULL || pulMarketID == NULL)
		return hErr;

  	hErr = OAPI_SYSINFO_STB_GetFusingInfo(pucSCS, pucJTAG, pucOTP, pucCWE, pulMarketID);

	HxLOG_Debug("APK_SYSINFO_STB_GetFusingInfo: SCS:%d JTAG:%d OTP:%d CWE:%d, MarketID:0x%x\n", *pucSCS, *pucJTAG, *pucOTP, *pucCWE, *pulMarketID);

	return hErr;
}

HERROR		APK_SYSINFO_STB_GetFusingInfoExt(HUINT8 *pucSCS, HUINT8 *pucJTAG, HUINT8 *pucOTP, HUINT8 *pucCWE, HUINT32* pulOTPField1, HUINT32* pulOTPField2)
{
	HERROR	hErr = ERR_FAIL;

	if (pucSCS == NULL || pucJTAG == NULL || pucOTP == NULL || pucCWE == NULL || pulOTPField1 == NULL || pulOTPField2 == NULL)
		return hErr;

  	hErr = OAPI_SYSINFO_STB_GetFusingInfoExt(pucSCS, pucJTAG, pucOTP, pucCWE, pulOTPField1, pulOTPField2);

	HxLOG_Debug("APK_SYSINFO_STB_GetFusingInfoExt: SCS:%d JTAG:%d OTP:%d CWE:%d OTPField1:0x%08x, OTPField2:0x%08x\n", *pucSCS, *pucJTAG, *pucOTP, *pucCWE, *pulOTPField1, *pulOTPField2);

	return hErr;
}


#if defined(CONFIG_NETFLIX_APP)
// for Netflix
HERROR	APK_SYSINFO_STB_SetNetflixESN(HCHAR *pszESN)
{
    return OAPI_SYSINFO_STB_SetNetflixESN(pszESN);
}

// for Netflix
HERROR	APK_SYSINFO_STB_GetNetflixESN(HCHAR *pszESN, HINT32 nMax)
{
    return OAPI_SYSINFO_STB_GetNetflixESN(pszESN, nMax);
}
#endif

/* end of file */

