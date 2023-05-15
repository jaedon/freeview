/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk_cas_irdeto.c
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

#include <hlib.h>
#include <oapi_cas_irdeto.h>
#include <apk_cas_irdeto.h>

#define ___INTERNAL_STRUCTURES___

#define ___INTERNAL_VARIABLES___
STATIC APK_CAS_IRDETO_Notifier_t s_pfAppNotifier;

#define ___PRIVATE_FUNCTION___
#define _____NOTIFIER_FUNCTION_____
STATIC void apk_cas_irdeto_onStatusChanged(void)
{
#if 0 // not yet used.
	if(s_pfAppNotifier != NULL)
	{
		APK_EVENT_EMIT_ARGS	stApkEvtArgs = {0,};

		stApkEvtArgs.pfnFreeFunc = NULL;
		stApkEvtArgs.ulArgCount = 0;

		APK_EVENT_Emit(&stApkEvtArgs);

		// TODO: Queue에 넣는 방식으로 고쳐야 함.
		(s_pfAppNotifier)();
	}
#endif
}

#define ___MEMBER_FUNCTION___

#define ___API_FUNCTION___
HERROR APK_CAS_IRDETO_Init(void)
{
	OAPI_CAS_IRDETO_Init(apk_cas_irdeto_onStatusChanged);

	return ERR_OK;
}

HERROR APK_CAS_IRDETO_RegisterNotifier(APK_CAS_IRDETO_Notifier_t fnNotifier)
{
	if(NULL == fnNotifier)
	{
		HxLOG_Error ("[%s:%d] fnNotifier NULL \n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}
	else
	{
		s_pfAppNotifier = fnNotifier;
	}

	return ERR_OK;
}

HERROR APK_CAS_IRDETO_UnregisterNotifier(void)
{
	s_pfAppNotifier = NULL;

	return ERR_OK;
}

HERROR		APK_CAS_IRDETO_SetVirtualSecureCwMode(HBOOL bVirtualSecureCW)
{
	return OAPI_CAS_IRDETO_SetVirtualSecureCwMode(bVirtualSecureCW);
}

HERROR		APK_CAS_IRDETO_GetVirturlSecureCwMode(HBOOL *pbVirtualSecureCwMode)
{
	HERROR					hErr;
	OxCasIrdetoStatus_t			stStatus;

	hErr = OAPI_CAS_IRDETO_GetStatus(&stStatus);
	if(hErr == ERR_OK)
	{
		*pbVirtualSecureCwMode = stStatus.bVirtualSecureCW;
	}
	else
	{
		*pbVirtualSecureCwMode = FALSE;
	}

	return hErr;
}

HERROR		APK_CAS_IRDETO_GetIrdetoClientVersionName(HCHAR *pszVesionName)
{
	HERROR					hErr;
	OxCasIrdetoStatus_t			stStatus;

	hErr = OAPI_CAS_IRDETO_GetStatus(&stStatus);
	if(hErr == ERR_OK)
	{
		HLIB_STD_StrUtf8NCpy(pszVesionName, stStatus.szClientVer, OAPI_CAS_IRDETO_CLIENT_VERSION_LENG);
	}
	else
	{

	}

	return hErr;
}

HERROR		APK_CAS_IRDETO_IsCWE(HBOOL *pbCWE)
{
	HERROR					hErr;
	OxCasIrdetoStatus_t			stStatus;

	hErr = OAPI_CAS_IRDETO_GetStatus(&stStatus);
	if(hErr == ERR_OK)
	{
		*pbCWE = stStatus.bCWE;
	}
	else
	{
		*pbCWE = FALSE;
	}

	return hErr;
}

HERROR		APK_CAS_IRDETO_IsJTAG(HBOOL *pbJTAG)
{
	HERROR					hErr;
	OxCasIrdetoStatus_t			stStatus;

	hErr = OAPI_CAS_IRDETO_GetStatus(&stStatus);
	if(hErr == ERR_OK)
	{
		*pbJTAG = stStatus.bJTAG;
	}
	else
	{
		*pbJTAG = FALSE;
	}

	return hErr;
}

HERROR		APK_CAS_IRDETO_IsSBOOT(HBOOL *pbSBOOT)
{
	HERROR					hErr;
	OxCasIrdetoStatus_t			stStatus;

	hErr = OAPI_CAS_IRDETO_GetStatus(&stStatus);
	if(hErr == ERR_OK)
	{
		*pbSBOOT = stStatus.bSBOOT;
	}
	else
	{
		*pbSBOOT = FALSE;
	}

	return hErr;
}

HERROR		APK_CAS_IRDETO_IsOTP(HBOOL *pbOTP)
{
	HERROR					hErr;
	OxCasIrdetoStatus_t			stStatus;

	hErr = OAPI_CAS_IRDETO_GetStatus(&stStatus);
	if(hErr == ERR_OK)
	{
		*pbOTP = stStatus.bOTP;
	}
	else
	{
		*pbOTP = FALSE;
	}

	return hErr;
}

HERROR		APK_CAS_IRDETO_GetIrdetoManCode(HCHAR *pszMancode)
{
	HERROR					hErr;
	OxCasIrdetoStatus_t			stStatus;

	hErr = OAPI_CAS_IRDETO_GetStatus(&stStatus);
	if(hErr == ERR_OK)
	{
		HLIB_STD_StrUtf8NCpy(pszMancode, (const HCHAR*)stStatus.szMancode, OAPI_CAS_IRDETO_SYSTEM_INFO_COMPONENT_LENG);
	}
	else
	{

	}

	return hErr;
}

HERROR		APK_CAS_IRDETO_GetIrdetoHwCode(HCHAR *pszHWCode)
{
	HERROR					hErr;
	OxCasIrdetoStatus_t			stStatus;

	hErr = OAPI_CAS_IRDETO_GetStatus(&stStatus);
	if(hErr == ERR_OK)
	{
		HLIB_STD_StrUtf8NCpy(pszHWCode, (const HCHAR*)stStatus.szHWCode, OAPI_CAS_IRDETO_SYSTEM_INFO_COMPONENT_LENG);
	}
	else
	{

	}

	return hErr;
}

HERROR		APK_CAS_IRDETO_GetIrdetoVariant(HCHAR *pszVariant)
{
	HERROR					hErr;
	OxCasIrdetoStatus_t			stStatus;

	hErr = OAPI_CAS_IRDETO_GetStatus(&stStatus);
	if(hErr == ERR_OK)
	{
		HLIB_STD_StrUtf8NCpy(pszVariant, (const HCHAR*)stStatus.szVariant, OAPI_CAS_IRDETO_SYSTEM_INFO_COMPONENT_LENG);
	}
	else
	{

	}

	return hErr;
}

HERROR		APK_CAS_IRDETO_GetIrdetoSysId(HCHAR *pszSysId)
{
	HERROR					hErr;
	OxCasIrdetoStatus_t			stStatus;

	hErr = OAPI_CAS_IRDETO_GetStatus(&stStatus);
	if(hErr == ERR_OK)
	{
		HLIB_STD_StrUtf8NCpy(pszSysId, (const HCHAR*)stStatus.szSysId, OAPI_CAS_IRDETO_SYSTEM_INFO_COMPONENT_LENG);
	}
	else
	{

	}

	return hErr;
}

HERROR		APK_CAS_IRDETO_GetIrdetoKeyVer(HCHAR *pszKeyVer)
{
	HERROR					hErr;
	OxCasIrdetoStatus_t			stStatus;

	hErr = OAPI_CAS_IRDETO_GetStatus(&stStatus);
	if(hErr == ERR_OK)
	{
		HLIB_STD_StrUtf8NCpy(pszKeyVer, (const HCHAR*)stStatus.szKeyVer, OAPI_CAS_IRDETO_SYSTEM_INFO_COMPONENT_LENG);
	}
	else
	{

	}

	return hErr;
}

HERROR		APK_CAS_IRDETO_GetIrdetoSigVer(HCHAR *pszSigVer)
{
	HERROR					hErr;
	OxCasIrdetoStatus_t			stStatus;

	hErr = OAPI_CAS_IRDETO_GetStatus(&stStatus);
	if(hErr == ERR_OK)
	{
		HLIB_STD_StrUtf8NCpy(pszSigVer, (const HCHAR*)stStatus.szSigVer, OAPI_CAS_IRDETO_SYSTEM_INFO_COMPONENT_LENG);
	}
	else
	{

	}

	return hErr;
}

HERROR		APK_CAS_IRDETO_GetControlByte(HUINT8 *pControlByte)
{
	HERROR					hErr;
	OxCasIrdetoStatus_t			stStatus;

	hErr = OAPI_CAS_IRDETO_GetStatus(&stStatus);
	if(hErr == ERR_OK)
	{
		*pControlByte = stStatus.ucControlByte;
	}
	else
	{
		*pControlByte = 0;
	}

	return hErr;
}

HERROR		APK_CAS_IRDETO_GetPrivateData(HUINT8 *pPrivateData)
{
	HERROR					hErr = ERR_FAIL;
#if defined(CONFIG_MW_CAS_IRDETO_UCAS)
	OxCasIrdetoStatus_t		stStatus;
#endif

#if defined(CONFIG_MW_CAS_IRDETO_UCAS)
	hErr = OAPI_CAS_IRDETO_GetStatus(&stStatus);
	if(hErr == ERR_OK)
	{
		HLIB_STD_StrUtf8NCpy(pPrivateData, stStatus.szPrivateData, OAPI_CAS_IRDETO_CLIENT_VERSION_LENG);
		HxLOG_Debug("[%s:%d] szPrivateData :[%s]\n", __FUNCTION__, __LINE__, stStatus.szPrivateData);
	}
	else
	{
		HxLOG_Error ("[%s:%d] OAPI_CAS_IRDETO_GetStatus Error!! \n", __FUNCTION__, __LINE__);
	}
#endif

	return hErr;
}




/* end of file */
