/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk_output_thermal_protection.c
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

#include <oapi.h>
#include <apk.h>
#include <papi.h>

#define ___INTERNAL_STRUCTURES___

#define ___INTERNAL_VARIABLES___
//static HINT32	s_nApplPriority = 0;
STATIC APLK_OUTPUT_THERMAL_Notifier_t	s_pfAppNotifier;

#define ___PRIVATE_FUNCTION___
#define _____NOTIFIER_FUNCTION_____
STATIC void apk_output_thermal_onStatusChanged(void)
{
	HBOOL					bRet = FALSE;

	HxLOG_Error("%s()\n", __FUNCTION__);

	if(s_pfAppNotifier != NULL)
	{
		APK_EVENT_EMIT_ARGS	stApkEvtArgs = {0,};
		HxLOG_Error("%s()\n", __FUNCTION__);


		stApkEvtArgs.pfnFreeFunc = NULL;
		stApkEvtArgs.ulArgCount = 2;
		stApkEvtArgs.apArgV[0] = (void *)s_pfAppNotifier;
		stApkEvtArgs.apArgV[1] = (void *)bRet;
		APK_EVENT_Emit(&stApkEvtArgs);
	}
}

#define ___MEMBER_FUNCTION___

#define ___API_FUNCTION___

HERROR APK_OUTPUT_THERMAL_Init(void)
{
	HxLOG_Error("%s()\n", __FUNCTION__);

	OAPI_OUTPUT_THERMAL_Init(apk_output_thermal_onStatusChanged);

	return ERR_OK;
}

HERROR APK_OUTPUT_THERMAL_RegisterNotifier(APLK_OUTPUT_THERMAL_Notifier_t fnNotifier)
{
	HxLOG_Error("%s()\n", __FUNCTION__);

	if(NULL == fnNotifier)
	{
		HxLOG_Error ("[%s:%d] fnNotifier NULL \n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}
	else
	{
		HxLOG_Error("%s() ==> \n", __FUNCTION__);
		s_pfAppNotifier = fnNotifier;
	}

	return ERR_OK;
}

HERROR APK_OUTPUT_THERMAL_UnregisterNotifier(void)
{
	HxLOG_Error("%s()\n", __FUNCTION__);

	s_pfAppNotifier = NULL;

	return ERR_OK;
}

HERROR	APK_OUTPUT_THERMAL_Shutdown(void)
{
	HxLOG_Error("%s()\n", __FUNCTION__);
	//APK_POWER_RequestReboot() 와 동일
	return PAPI_RequestReboot();
}


/* end of file */
