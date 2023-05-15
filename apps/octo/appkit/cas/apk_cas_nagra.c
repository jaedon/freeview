/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk_cas_nagra.c
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

#include <hlib.h>
#include <oapi_cas_nagra.h>
#include <apk_cas_nagra.h>

#define ___INTERNAL_STRUCTURES___

#define ___INTERNAL_VARIABLES___
STATIC APK_CAS_NAGRA_Notifier_t s_pfAppNotifier;
STATIC HUINT8 s_nAccessState = 0;

#if defined(CONFIG_SUPPORT_NOVA_APP)
STATIC APK_CAS_NAGRA_NovaTransportSessionInfoNotifier_t	s_pfApkNovaTransportSessionInfoNotifier;
STATIC APK_CAS_NAGRA_NovaLicenseNotifier_t				s_pfApkNovaLicenseNotifier;
STATIC APK_CAS_NAGRA_NovaKeyHandlerNotifier_t				s_pfApkNovaKeyHandlerNotifier;
STATIC APK_CAS_NAGRA_NovaPlatformUsageRulesNotifier_t		s_pfApkNovaPlatformUsageRulesNotifier;
STATIC APK_CAS_NAGRA_NovaServiceUsageRulesNotifier_t		s_pfApkNovaServiceUsageRulesNotifierr;
#endif

#define ___PRIVATE_FUNCTION___
#define _____NOTIFIER_FUNCTION_____
STATIC void apk_cas_nagra_onStatusChanged(void)
{
	HERROR					hErr = ERR_OK;
	OxCasNagraStatus_t		stStatus;
	HBOOL					bRet = FALSE;

	hErr = OAPI_CAS_NAGRA_GetStatus(&stStatus);
	if(hErr == ERR_OK)
	{
		bRet = stStatus.bSCRemoved;		// 1 : Nagra Smartcard is removed.
		HxLOG_Error("[apk_cas_nagra_onStatusChanged] bRet = %d\n", bRet);
	}
	else
	{
		bRet = FALSE;					// 0 : Nagra Smartcard is inserted.
		HxLOG_Error("[apk_cas_nagra_onStatusChanged] bRet = %d\n", bRet);
	}

	if(s_pfAppNotifier != NULL)
	{
		APK_EVENT_EMIT_ARGS	stApkEvtArgs = {0,};

		stApkEvtArgs.pfnFreeFunc = NULL;
		stApkEvtArgs.ulArgCount = 2;
		stApkEvtArgs.apArgV[0] = (void *)s_pfAppNotifier;
		stApkEvtArgs.apArgV[1] = (void *)bRet;

		APK_EVENT_Emit(&stApkEvtArgs);
	}
}

#if defined(CONFIG_SUPPORT_NOVA_APP)
STATIC void apk_cas_nagra_NovatTransportSessionInfoNotifier(HUINT32 hAction, HUINT32 ulTransportSessionInfo, HINT32 usOnId, HINT32 usTsId, HINT32 usSvcId, HUINT16 usAudioPid, HUINT16 usVideoPid)
{
	HxLOG_Trace();

	if(s_pfApkNovaTransportSessionInfoNotifier != NULL)
	{
		APK_EVENT_EMIT_ARGS	stApkEvtArgs = {0,};

		HxLOG_Debug("*\t usOnId = 0x%08x, usTsId = 0x%08x, usSvcId = 0x%08x, usAudioPid = 0x%x, usVideoPid = 0x%x\n", usOnId, usTsId, usSvcId, usAudioPid, usVideoPid);

		stApkEvtArgs.pfnFreeFunc = NULL;
		stApkEvtArgs.ulArgCount = 8;
		stApkEvtArgs.apArgV[0] = (void *)s_pfApkNovaTransportSessionInfoNotifier;
		stApkEvtArgs.apArgV[1] = (void *)hAction;
		stApkEvtArgs.apArgV[2] = (void *)ulTransportSessionInfo;
		stApkEvtArgs.apArgV[3] = (void *)usOnId;
		stApkEvtArgs.apArgV[4] = (void *)usTsId;
		stApkEvtArgs.apArgV[5] = (void *)usSvcId;
		stApkEvtArgs.apArgV[6] = (void *)usAudioPid;
		stApkEvtArgs.apArgV[7] = (void *)usVideoPid;

		APK_EVENT_Emit(&stApkEvtArgs);
	}
	else
	{
		HxLOG_Error("[apk_cas_nagra_NovatTransportSessionInfoNotifier] is fail \n");
	}
}

#if 1 //defined(TARA_SES_DEMO) // TARA & SES Demo onlystatic HUINT8 TempLicenseBuf[1024];
static HUINT8 TempLicenseBuf[1024];
static HUINT8 TempLiceseSize;

HERROR apk_cas_nagra_GetTempLicenseBuf(HUINT8 * pBuf, HUINT32 ulSize)
{
	if(pBuf == NULL)
	{
		return ERR_FAIL;
	}

	HxSTD_memcpy(pBuf, &TempLicenseBuf[0], ulSize);
	return ERR_OK;	
}

void apk_cas_nagra_SetTempLicenseBuf(HUINT8 * pBuf, HUINT32 ulSize)
{
	if(pBuf == NULL)
	{
		return;
	}
	#if 0
	{
		int i;
		HxLOG_Print ("################# 1. test tt ulSize(%d)\n", ulSize);
		for(i=0;i<ulSize;i++)
		{
			if(i%16==0)
				HxLOG_Print ("\n");
			HxLOG_Print ("%02x ", pBuf[i]);
		}
		HxLOG_Print ("\n");
	}
	#endif
	HxSTD_memset(&TempLicenseBuf[0], 0, sizeof(TempLicenseBuf));

	HxSTD_memcpy(&TempLicenseBuf[0], pBuf, ulSize);
	#if 0
	{
		int i;
		HxLOG_Print ("################# 2. test tt ulSize(%d)\n", ulSize);
		for(i=0;i<ulSize;i++)
		{
			if(i%16==0)
				HxLOG_Print ("\n");
			HxLOG_Print ("%02x ", TempLicenseBuf[i]);
		}
		HxLOG_Print ("\n");
	}
	#endif
}

#endif

STATIC void apk_cas_nagra_NovaLicenseNotifier(HUINT32 ulTransportSessionId, HUINT32 ulRecordHandle,  HUINT8 *pucLicense, HUINT32 ulSize, HBOOL bRequestLicenseAgain)
{
	HxLOG_Trace();

	if(s_pfApkNovaLicenseNotifier != NULL)
	{
		APK_EVENT_EMIT_ARGS	stApkEvtArgs = {0,};

		HxLOG_Debug("[%s:%d]  ulTransportSessionId(%d), ulRecordHandle(0x%08x), ulSize(%d), bRequestLicenseAgain(%d), License{0x%x, 0x%x, 0x%x, ...}\n"
								, __FUNCTION__
								, __HxLINE__
								, ulTransportSessionId
								, ulRecordHandle
								, ulSize
								, bRequestLicenseAgain
								, pucLicense[0], pucLicense[1], pucLicense[2]);

		stApkEvtArgs.pfnFreeFunc = NULL;
		stApkEvtArgs.ulArgCount = 6;
		stApkEvtArgs.apArgV[0] = (void *)s_pfApkNovaLicenseNotifier;
		stApkEvtArgs.apArgV[1] = (void *)ulTransportSessionId;
		stApkEvtArgs.apArgV[2] = (void *)ulRecordHandle;
		stApkEvtArgs.apArgV[3] = (void *)pucLicense;
		stApkEvtArgs.apArgV[4] = (void *)ulSize;
		stApkEvtArgs.apArgV[5] = (void *)bRequestLicenseAgain;

#if 1 //defined(TARA_SES_DEMO) // TARA & SES Demo onlystatic HUINT8 TempLicenseBuf[1024];
		apk_cas_nagra_SetTempLicenseBuf(pucLicense, ulSize);
#endif

		APK_EVENT_Emit(&stApkEvtArgs);
	}
	else
	{
		HxLOG_Error("[apk_cas_nagra_NovaLicenseNotifier] is fail \n");
	}
}

STATIC void apk_cas_nagra_NovaKeyHandlerNotifier(HUINT8 *pucData)
{
	HxLOG_Trace();

	if(s_pfApkNovaKeyHandlerNotifier != NULL)
	{
		APK_EVENT_EMIT_ARGS	stApkEvtArgs = {0,};

		HxLOG_Debug("[%s:%d]  data = {0x%02x 0x%02x 0x%02x ...}\n"
								, __FUNCTION__
								, __HxLINE__
								, pucData[0], pucData[1], pucData[2]);

		stApkEvtArgs.pfnFreeFunc = NULL;
		stApkEvtArgs.ulArgCount = 2;
		stApkEvtArgs.apArgV[0] = (void *)s_pfApkNovaKeyHandlerNotifier;
		stApkEvtArgs.apArgV[1] = (void *)pucData;

		APK_EVENT_Emit(&stApkEvtArgs);
	}
	else
	{
		HxLOG_Error("[apk_cas_nagra_NovaKeyHandlerNotifier] is fail \n");
	}
}

STATIC void apk_cas_nagra_NovaPlatformUsageRulesNotifier(HUINT8 *pucUsageRules, HUINT32 ulSize)
{
	HxLOG_Trace();
	if(s_pfApkNovaPlatformUsageRulesNotifier != NULL)
	{
		APK_EVENT_EMIT_ARGS	stApkEvtArgs = {0,};

		stApkEvtArgs.pfnFreeFunc = NULL;
		stApkEvtArgs.ulArgCount = 3;
		stApkEvtArgs.apArgV[0] = (void *)s_pfApkNovaPlatformUsageRulesNotifier;
		stApkEvtArgs.apArgV[1] = (void *)pucUsageRules;
		stApkEvtArgs.apArgV[2] = (void *)ulSize;

		APK_EVENT_Emit(&stApkEvtArgs);
	}
	else
	{
		HxLOG_Error("[apk_cas_nagra_NovaPlatformUsageRulesNotifier] is fail \n");
	}
}

STATIC void apk_cas_nagra_NovaServiceUsageRulesNotifier(HUINT32 ulTransportSessionId, HUINT8 *pucUsageRules, HUINT32 ulSize)
{
	HxLOG_Trace();

	if(s_pfApkNovaServiceUsageRulesNotifierr != NULL)
	{
		APK_EVENT_EMIT_ARGS	stApkEvtArgs = {0,};

		stApkEvtArgs.pfnFreeFunc = NULL;
		stApkEvtArgs.ulArgCount = 4;
		stApkEvtArgs.apArgV[0] = (void *)s_pfApkNovaServiceUsageRulesNotifierr;
		stApkEvtArgs.apArgV[1] = (void *)ulTransportSessionId;
		stApkEvtArgs.apArgV[2] = (void *)pucUsageRules;
		stApkEvtArgs.apArgV[3] = (void *)ulSize;

		APK_EVENT_Emit(&stApkEvtArgs);
	}
	else
	{
		HxLOG_Error("[apk_cas_nagra_NovaServiceUsageRulesNotifier] is fail \n");
	}
}
#endif

#define ___MEMBER_FUNCTION___

#define ___API_FUNCTION___
HERROR APK_CAS_NAGRA_Init(void)
{
	OAPI_CAS_NAGRA_Init(apk_cas_nagra_onStatusChanged);
	return ERR_OK;
}

#if defined(CONFIG_SUPPORT_NOVA_APP)
HERROR APK_CAS_NAGRA_NOVA_Init(void)
{
	if(OAPI_CAS_NAGRA_NOVA_Init(apk_cas_nagra_NovatTransportSessionInfoNotifier
								, apk_cas_nagra_NovaLicenseNotifier
								, apk_cas_nagra_NovaKeyHandlerNotifier
								, apk_cas_nagra_NovaPlatformUsageRulesNotifier
								, apk_cas_nagra_NovaServiceUsageRulesNotifier) != ERR_OK)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}
#endif

HERROR APK_CAS_NAGRA_RegisterNotifier(APK_CAS_NAGRA_Notifier_t fnNotifier)
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

HERROR APK_CAS_NAGRA_UnregisterNotifier(void)
{
	s_pfAppNotifier = NULL;

	return ERR_OK;
}

HERROR		APK_CAS_NAGRA_IsCWE(HBOOL *pbCWE)
{
	HERROR					hErr;
	OxCasNagraStatus_t			stStatus;

	hErr = OAPI_CAS_NAGRA_GetStatus(&stStatus);
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

HERROR		APK_CAS_NAGRA_IsJTAG(HBOOL *pbJTAG)
{
	HERROR					hErr;
	OxCasNagraStatus_t			stStatus;

	hErr = OAPI_CAS_NAGRA_GetStatus(&stStatus);
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

HERROR		APK_CAS_NAGRA_IsSBOOT(HBOOL *pbSBOOT)
{
	HERROR					hErr;
	OxCasNagraStatus_t			stStatus;

	hErr = OAPI_CAS_NAGRA_GetStatus(&stStatus);
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

HERROR		APK_CAS_NAGRA_IsOTP(HBOOL *pbOTP)
{
	HERROR					hErr;
	OxCasNagraStatus_t			stStatus;

	hErr = OAPI_CAS_NAGRA_GetStatus(&stStatus);
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

HERROR		APK_CAS_NAGRA_GetNUID(HCHAR *pszNUID)
{
	HERROR					hErr;
	OxCasNagraStatus_t			stStatus;

	hErr = OAPI_CAS_NAGRA_GetStatus(&stStatus);
	if(hErr == ERR_OK)
	{
		HLIB_STD_StrUtf8NCpy(pszNUID, (const HCHAR*)stStatus.szNUID, OAPI_CAS_NAGRA_SYSTEM_INFO_COMPONENT_LENG);
	}
	else
	{
		HxLOG_Error ("[%s:%d] Error on OAPI_CAS_NAGRA_GetStatus \n", __FUNCTION__, __LINE__);
	}

	return hErr;
}

HERROR		APK_CAS_NAGRA_GetCASN(HCHAR *pszCASN)
{
	HERROR					hErr;
	OxCasNagraStatus_t			stStatus;

	hErr = OAPI_CAS_NAGRA_GetStatus(&stStatus);
	if(hErr == ERR_OK)
	{
		HLIB_STD_StrUtf8NCpy(pszCASN, (const HCHAR*)stStatus.szCASN, OAPI_CAS_NAGRA_SYSTEM_INFO_COMPONENT_LENG);
	}
	else
	{
		HxLOG_Error ("[%s:%d] Error on OAPI_CAS_NAGRA_GetStatus \n", __FUNCTION__, __LINE__);
	}

	return hErr;
}
HERROR	    APK_CAS_NAGRA_GetFusingInfo(HUINT8 *pucSCS, HUINT8 *pucJTAG, HUINT8 *pucOTP, HUINT8 *pucCWE)
{
	HERROR	hErr = ERR_FAIL;
    OxCasNagraStatus_t			stStatus;

	if (pucSCS == NULL || pucJTAG == NULL || pucOTP == NULL || pucCWE == NULL)
		return hErr;

  	hErr = OAPI_CAS_NAGRA_GetStatus(&stStatus);

    if(hErr == ERR_OK){
        *pucSCS = stStatus.bSBOOT;
    	*pucJTAG = stStatus.bJTAG;
    	*pucOTP = stStatus.bOTP;
    	*pucCWE = stStatus.bCWE;
    }
    else
    {
        HxLOG_Error ("[%s:%d] Error on OAPI_CAS_NAGRA_GetStatus \n", __FUNCTION__, __LINE__);
    }

    HxLOG_Debug("[%s, %d]SCS:%d JTAG:%d OTP:%d CWE:%d\n", __FUNCTION__, __LINE__,*pucSCS, *pucJTAG, *pucOTP, *pucCWE);

	return hErr;
}

HBOOL	APK_CAS_NAGRA_IsSCInserted(void)
{
	HERROR				hErr = ERR_FAIL;
	HBOOL				SCInserted = FALSE;

	hErr = OAPI_CAS_NAGRA_IsSCInserted(&SCInserted);
	if(hErr == ERR_OK)
	{
		HxLOG_Debug("[%s:%d] SCInserted = %d \n", __FUNCTION__, __HxLINE__, SCInserted);
		return SCInserted;
	}

	HxLOG_Debug("[%s:%d] pbSCInserted = %d \n", __FUNCTION__, __HxLINE__, SCInserted);

	return FALSE;
}

HBOOL	APK_CAS_NAGRA_PINCODE_Verify(const HCHAR *pszPinStr)
{
	HERROR				hErr = ERR_FAIL;
	HBOOL				bVerified = FALSE;

	hErr = OAPI_CAS_NAGRA_PINCODE_Verify(pszPinStr, &bVerified);
	if ( hErr == ERR_OK)
	{
		HxLOG_Debug("[%s:%d] bVerified = %d \n", __FUNCTION__, __HxLINE__, bVerified);
		return bVerified;
	}

	HxLOG_Debug("%s:%d] bVerified = %d \n", __FUNCTION__, __HxLINE__, bVerified);

	return FALSE;
}

HBOOL	APK_CAS_NAGRA_GetAccessibleState(HUINT8 *pnAccessState, HUINT32 *pulRemainTime, HUINT8 *pnRetryCount)
{
	HERROR					hErr = ERR_FAIL;
	// pnAccessState 의 data type은 eCAS_PIN_STATE

	hErr = OAPI_CAS_NAGRA_GetAccessibleState(pnAccessState, pulRemainTime, pnRetryCount);
	if(hErr == ERR_OK)
	{
		HxLOG_Error("[%s:%d] hErr = %d, *pnAccessState = %d, *pnRetryCount = %d, *pulRemainTime = %d \n", __FUNCTION__, __HxLINE__,
						hErr, *pnAccessState, *pnRetryCount, *pulRemainTime);
		s_nAccessState = *pnAccessState;
		return TRUE;
	}

	s_nAccessState = *pnAccessState;
	HxLOG_Error("[%s:%d] OAPI_CAS_NAGRA_GetAccessibleState() fail ---------\n", __FUNCTION__, __HxLINE__);

	return FALSE;
}

HBOOL	APK_CAS_NAGRA_VerifyPincodeGetAccessibleState(HUINT8 *pnAccessState, HUINT32 *pulRemainTime, HUINT8 *pnRetryCount, const HCHAR *pszPinStr)
{
	HERROR					hErr = ERR_FAIL;
	// pnAccessState 의 data type은 eCAS_PIN_STATE

	hErr = OAPI_CAS_NAGRA_VerifyPincodeGetAccessibleState(pnAccessState, pulRemainTime, pnRetryCount, pszPinStr);
	if(hErr == ERR_OK)
	{
		HxLOG_Error("[%s:%d] *pnAccessState = %d, *pnRetryCount = %d, *pulRemainTime = %d \n", __FUNCTION__, __HxLINE__,
						*pnAccessState, *pnRetryCount, *pulRemainTime);
		s_nAccessState = *pnAccessState;
		return TRUE;
	}

	s_nAccessState = *pnAccessState;
	HxLOG_Error("[%s:%d] OAPI_CAS_NAGRA_GetPincodeAccessibleState() fail\n", __FUNCTION__, __HxLINE__);

	return FALSE;
}

//APK_CAS_NAGRA_GetAccessibleState(), APK_CAS_NAGRA_VerifyPincodeGetAccessibleState() 함수가 먼저 불린 후에, 이 함수 콜해야 한다.
HBOOL	APK_CAS_NAGRA_IsAccessibleState(HUINT8 *pnAccessState)
{
	if (s_nAccessState == ERR_OK) // eCAS_PIN_STATE_OK
	{
		HxLOG_Error("[%s:%d] It is Accessible!!!\n", __FUNCTION__, __HxLINE__);
		*pnAccessState = s_nAccessState;
		return TRUE;
	}
	else
	{
		HxLOG_Error("[%s:%d] It is NOT Accessible!!! (s_nAccessState=%d)\n", __FUNCTION__, __HxLINE__, s_nAccessState);
		*pnAccessState = s_nAccessState;
		return FALSE;
	}
}

HBOOL	APK_CAS_NAGRA_PINCODE_Set(const HCHAR *pszOldPinStr, const HCHAR *pszNewPinStr)
{
	HERROR					hErr = ERR_FAIL;

	hErr = OAPI_CAS_NAGRA_PINCODE_Set(pszOldPinStr, pszNewPinStr);
	if (hErr == ERR_OK)
	{
		HxLOG_Debug("[%s:%d] OAPI_CAS_NAGRA_PINCODE_Set() success\n", __FUNCTION__, __HxLINE__);
		return TRUE;
	}

	HxLOG_Error("[%s:%d] OAPI_CAS_NAGRA_PINCODE_Set() fail\n", __FUNCTION__, __HxLINE__);

	return FALSE;
}

#if defined (CONFIG_OP_SES)
HBOOL	APK_CAS_NAGRA_UnlockAV()
{
	HERROR				hErr = ERR_FAIL;

	hErr = OAPI_CAS_NAGRA_UnlockAV();
	if (hErr == ERR_OK)
	{
		HxLOG_Error("[%s:%d] success \n", __FUNCTION__, __HxLINE__);
		return TRUE;
	}

	HxLOG_Error("[%s:%d] fail \n", __FUNCTION__, __HxLINE__);

	return FALSE;
}
#endif

#if defined(CONFIG_SUPPORT_NOVA_APP)
HERROR APK_CAS_NAGRA_RegisterNovaTransportSessionInfoNotifier(APK_CAS_NAGRA_NovaTransportSessionInfoNotifier_t fnNotifier)
{
	if(NULL == fnNotifier)
	{
		HxLOG_Error ("[%s:%d] fnNotifier NULL \n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}
	else
	{
		HxLOG_Debug("[%s:%d] s_pfApkNovaTransportSessionInfoNotifier - OK  (0x%08x)\n", __FUNCTION__, __HxLINE__, fnNotifier);
		s_pfApkNovaTransportSessionInfoNotifier = fnNotifier;
	}

	return ERR_OK;
}

HERROR APK_CAS_NAGRA_UnregisterNovaTransportSessionInfoNotifier(void)
{
	HxLOG_Trace();
	s_pfApkNovaTransportSessionInfoNotifier = NULL;

	return ERR_OK;
}

HERROR APK_CAS_NAGRA_RegisterNovaLicenseNotifier(APK_CAS_NAGRA_NovaLicenseNotifier_t fnNotifier)
{
	if(NULL == fnNotifier)
	{
		HxLOG_Error ("[%s:%d] fnNotifier NULL \n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}
	else
	{
		HxLOG_Debug("[%s:%d] s_pfApkNovaLicenseNotifier - OK  (0x%08x)\n", __FUNCTION__, __HxLINE__, fnNotifier);
		s_pfApkNovaLicenseNotifier = fnNotifier;
	}

	return ERR_OK;
}

HERROR APK_CAS_NAGRA_UnregisterNovaLicenseNotifier(void)
{
	HxLOG_Trace();
	s_pfApkNovaLicenseNotifier = NULL;

	return ERR_OK;
}

HERROR APK_CAS_NAGRA_RegisterNovaKeyHandlerNotifier(APK_CAS_NAGRA_NovaKeyHandlerNotifier_t fnNotifier)
{
	if(NULL == fnNotifier)
	{
		HxLOG_Error ("[%s:%d] fnNotifier NULL \n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}
	else
	{
		HxLOG_Debug("[%s:%d] s_pfApkNovaKeyHandlerNotifier - OK  (0x%08x)\n", __FUNCTION__, __HxLINE__, fnNotifier);
		s_pfApkNovaKeyHandlerNotifier = fnNotifier;
	}

	return ERR_OK;
}

HERROR APK_CAS_NAGRA_UnregisterNovaKeyHandlerNotifier(void)
{
	HxLOG_Trace();
	s_pfApkNovaKeyHandlerNotifier = NULL;

	return ERR_OK;
}

HERROR APK_CAS_NAGRA_RegisterNovaPlatformUsageRulesNotifier(APK_CAS_NAGRA_NovaPlatformUsageRulesNotifier_t fnNotifier)
{
	if(NULL == fnNotifier)
	{
		HxLOG_Error ("[%s:%d] fnNotifier NULL \n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}
	else
	{
		HxLOG_Debug("[%s:%d] s_pfApkNovaPlatformUsageRulesNotifier - OK\n", __FUNCTION__, __HxLINE__);
		s_pfApkNovaPlatformUsageRulesNotifier = fnNotifier;
	}

	return ERR_OK;
}

HERROR APK_CAS_NAGRA_UnregisterNovaPlatformUsageRulesNotifier(void)
{
	HxLOG_Trace();
	s_pfApkNovaPlatformUsageRulesNotifier = NULL;

	return ERR_OK;
}

HERROR APK_CAS_NAGRA_RegisterNovaServiceUsageRulesNotifier(APK_CAS_NAGRA_NovaServiceUsageRulesNotifier_t fnNotifier)
{
	if(NULL == fnNotifier)
	{
		HxLOG_Error ("[%s:%d] fnNotifier NULL \n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}
	else
	{
		HxLOG_Debug("[%s:%d] s_pfApkNovaServiceUsageRulesNotifierr - OK\n", __FUNCTION__, __HxLINE__);
		s_pfApkNovaServiceUsageRulesNotifierr = fnNotifier;
	}

	return ERR_OK;
}

HERROR APK_CAS_NAGRA_UnregisterNovaServiceUsageRulesNotifier(void)
{
	HxLOG_Trace();
	s_pfApkNovaServiceUsageRulesNotifierr = NULL;

	return ERR_OK;
}

HERROR APK_CAS_NAGRA_PRM_GetCaInfo(HUINT8 *pucData)
{
	HERROR				hErr = ERR_FAIL;

	if(pucData == NULL)
	{
		return ERR_FAIL;
	}

	hErr =OAPI_CAS_NAGRA_PRM_GetCaInfo(pucData);
	if (hErr == ERR_FAIL)
	{
		HxLOG_Error("[%s:%d] fail \n", __FUNCTION__, __HxLINE__);
	}

	return hErr;
}

HERROR APK_CAS_NAGRA_PRM_NovaDrmRelease(HUINT32 ulTransportSessionId, HUINT32 ulRecordHandle)
{
	HERROR				hErr = ERR_FAIL;

	hErr = OAPI_CAS_NAGRA_PRM_NovaDrmRelease(ulTransportSessionId, ulRecordHandle);

	if (hErr == ERR_FAIL)
	{
		HxLOG_Error("[%s:%d] fail\n", __FUNCTION__, __HxLINE__);
	}

	return hErr;
}

HERROR APK_CAS_NAGRA_PRM_NovaDrmSetParameters(HUINT32 ulTransportSessionId
									, HUINT8 *pOldLicense, HUINT32 ulOldLicenseSize
									, HUINT32 ulCollectionId
									, HUINT32 ulDuration
									, HUINT8 *pUsageRules, HUINT32 ulUsageRulesLength)
{
	HERROR				hErr = ERR_FAIL;

	hErr = OAPI_CAS_NAGRA_PRM_NovaDrmSetParameters(ulTransportSessionId, pOldLicense, ulOldLicenseSize, ulCollectionId, ulDuration, pUsageRules, ulUsageRulesLength);

	if (hErr == ERR_FAIL)
	{
		HxLOG_Error("[%s:%d] fail\n", __FUNCTION__, __HxLINE__);
	}

	return hErr;
}
#endif
/* end of file */
