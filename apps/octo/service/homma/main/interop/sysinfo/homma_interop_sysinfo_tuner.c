/**************************************************************
*	@file	homma_interop.c
*	http://www.humaxdigital.com
*	@author			humax
*	@brief			HOMMA / interoperation supporting instead
*					of using APK
**************************************************************/

/*
* ⓒ 2011-2012 Humax Co., Ltd.
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
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/

/* ------------------------------------------------------------- //
 * Author : tjjang
 * Description : This is for replacing apk methods
 *  - HOMMA_INTEROP prefix is used for external methods
 *  - s_HOMMA_INTEROP prefix is used to represent static methods
 */

#include <homma_interop.h>

/*
 * methods
 *  - static & external
 */


/* ############################################################################
                  _          __
                 (_)        / _|
 ___  _   _  ___  _  _ __  | |_  ___
/ __|| | | |/ __|| || '_ \ |  _|/ _ \
\__ \| |_| |\__ \| || | | || | | (_) |
|___/ \__, ||___/|_||_| |_||_|  \___/
       __/ |
      |___/
 xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx  sysinfo tuner & stb  xxxxxxxx
############################################################################ */



// unwilling to use apps/octo/appkit/
#ifdef HOMMA_USE_APPKIT_DIRECTLY

HERROR HOMMA_INTEROP_SYSINFO_TUNER_Init(void)
{
	return APK_SYSINFO_TUNER_Init();
}

HERROR HOMMA_INTEROP_SYSINFO_TUNER_GetSignalLevel(HINT32 nIndex, HINT32 *pnSignalLevel)
{
	return APK_SYSINFO_TUNER_GetSignalLevel(nIndex, pnSignalLevel);
}

HERROR HOMMA_INTEROP_SYSINFO_TUNER_GetSignalQuality(HINT32 nIndex, HINT32 *pnSignalQuality)
{
	return APK_SYSINFO_TUNER_GetSignalQuality(nIndex, pnSignalQuality);
}

HERROR HOMMA_INTEROP_SYSINFO_TUNER_IsLocked(HINT32 nIndex, HBOOL *pbLocked)
{
	return APK_SYSINFO_TUNER_IsLocked(nIndex, pbLocked);
}

#else // #ifdef HOMMA_USE_APPKIT_DIRECTLY else

//APK_SYSINFO_TUNER_Init();
HERROR HOMMA_INTEROP_SYSINFO_TUNER_Init(void)
{
	return OAPI_SYSINFO_TUNER_Init(NULL);
}

//APK_SYSINFO_TUNER_GetSignalLevel(nIndex, &nSignalLevel);
HERROR HOMMA_INTEROP_SYSINFO_TUNER_GetSignalLevel(HINT32 nIndex, HINT32 *pnSignalLevel)
{
	HERROR  err;
	OxSysInfoTuner_SignalInfo_t stSignalInfo;

	err = OAPI_SYSINFO_TUNER_GetSignalInfo(nIndex, &stSignalInfo);
	*pnSignalLevel = stSignalInfo.nSignalLevel;

	return err;
}

//APK_SYSINFO_TUNER_GetSignalQuality(nIndex, &nSignalQuality);
HERROR HOMMA_INTEROP_SYSINFO_TUNER_GetSignalQuality(HINT32 nIndex, HINT32 *pnSignalQuality)
{
	HERROR  err;
	OxSysInfoTuner_SignalInfo_t stSignalInfo;

	err = OAPI_SYSINFO_TUNER_GetSignalInfo(nIndex, &stSignalInfo);
	*pnSignalQuality = stSignalInfo.nSignalQuality;

	return err;
}

//APK_SYSINFO_TUNER_IsLocked(nIndex, pbLocked);
HERROR HOMMA_INTEROP_SYSINFO_TUNER_IsLocked(HINT32 nIndex, HBOOL *pbLocked)
{
	return OAPI_SYSINFO_TUNER_IsLocked(nIndex, pbLocked);
}

#endif // #ifdef HOMMA_USE_APPKIT_DIRECTLY endif

