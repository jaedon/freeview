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

/******************************************************************************/
/**
 * @file	  		nx_core_cas_na.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/


/******************************************************************************/
#define _________Private_Include______________________________________________
/******************************************************************************/
#include <nx_common.h>
#include <nx_port_cas.h>
#include <nx_core_cas.h>
#include <nx_core_cas_na.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static HERROR	nx_cas_na_OpenBannerSession(void);
static HERROR	nx_cas_na_OpenExtentionSession(void);
static void		nx_cas_na_notifyCasStateChangeFunc(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);

static HERROR	nx_cas_na_ConvertMwCASErrorMsgStrIdToApCASErrorType_Base(HCHAR *pszItemIdStrId, NX_CasNaMessage_Type *eCasMessageType);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/
static HERROR	nx_cas_na_OpenBannerSession(void)
{
	HERROR		hError = ERR_FAIL;
	HUINT32		ulSlot = eNxCasSlot_0;

	hError = NX_CAS_OpenSession(eNxCasNAHandleKey_Banner, eDxCAS_GROUPID_NA, ulSlot, eDxCAS_SESSIONTYPE_BANNER, (HUINT8*)NX_CAS_NA_BANNER_PARAM_DATA);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_CAS_OpenSession() - eNxCasNAHandleKey_Banner! Fail!! \n");
	}

	return	hError;
}

static HERROR	nx_cas_na_OpenExtentionSession(void)
{
	HERROR		hError = ERR_FAIL;
	HUINT32		ulSlot = eNxCasSlot_0;

	hError = NX_CAS_OpenSession(eNxCasNAHandleKey_Extension, eDxCAS_GROUPID_NA, ulSlot, eDxCAS_SESSIONTYPE_EXTENSION, (HUINT8*)NX_CAS_NA_EXTENSION_PARAM_DATA);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_CAS_OpenSession() - eNxCasNAHandleKey_Banner! Fail!! \n");
	}

	return	hError;
}

static void		nx_cas_na_notifyCasStateChangeFunc(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	ONDK_GWM_RECV_PostMsgToGWM(MSG_GWM_APK_MESSAGE, GWM_APK_CAS_SCSTATE_EVENT, (HINT32)ulParam1, (HINT32)ulParam2, (HINT32)ulParam3);
}


static HERROR	nx_cas_na_ConvertMwCASErrorMsgStrIdToApCASErrorType_Base(HCHAR *pszItemIdStrId, NX_CasNaMessage_Type *eCasMessageType)
{
	HINT32		lStrLen = 0;


	if (pszItemIdStrId == NULL)
	{
		NX_APP_Error("Error!!! (pszItemIdStrId == NULL) \n");
		*eCasMessageType = eNxCasNaMessage_Type_None;
		return	ERR_FAIL;
	}

	lStrLen = HxSTD_StrLen(pszItemIdStrId);
	if (lStrLen < 0)
	{
		NX_APP_Error("Error!!! (lStrLen < 0) \n");
		*eCasMessageType = eNxCasNaMessage_Type_None;
		return	ERR_FAIL;
	}

	/*
		Caution!!!  It must match string in xmgr_cas_na_ui.c file
	*/
	NX_APP_Debug("[Info] pszItemIdStrId:(%s), lStrLen:(%d)\n", pszItemIdStrId, lStrLen);

	if (0 == HxSTD_StrNCmp((char*)"LOC_CAS_NA_MSG_SMARTCARD_REMOVED_ID", (char*)pszItemIdStrId, lStrLen))
	{
		*eCasMessageType = eNxCasNaMessage_SMARTCARD_REMOVED;
	}
	else if (0 == HxSTD_StrNCmp((char*)"LOC_CAS_NA_MSG_SMARTCARD_COM_ERROR_ID", (char*)pszItemIdStrId, lStrLen))
	{
		*eCasMessageType = eNxCasNaMessage_SMARTCARD_COM_ERROR;
	}
	else if (0 == HxSTD_StrNCmp((char*)"LOC_CAS_NA_MSG_SMARTCARD_BLACKLISTED_ID", (char*)pszItemIdStrId, lStrLen))
	{
		*eCasMessageType = eNxCasNaMessage_SMARTCARD_BLACKLISTED;
	}
	else if (0 == HxSTD_StrNCmp((char*)"LOC_CAS_NA_MSG_SMARTCARD_NEVER_PAIRED_ID", (char*)pszItemIdStrId, lStrLen))
	{
		*eCasMessageType = eNxCasNaMessage_SMARTCARD_NEVER_PAIRED;
	}
	else if (0 == HxSTD_StrNCmp((char*)"LOC_CAS_NA_MSG_SMARTCARD_NOT_PAIRED_ID", (char*)pszItemIdStrId, lStrLen))
	{
		*eCasMessageType = eNxCasNaMessage_SMARTCARD_NOT_PAIRED;
	}
	else if (0 == HxSTD_StrNCmp((char*)"LOC_CAS_NA_MSG_SMARTCARD_MUTE_ID", (char*)pszItemIdStrId, lStrLen))
	{
		*eCasMessageType = eNxCasNaMessage_SMARTCARD_MUTE;
	}
	else if (0 == HxSTD_StrNCmp((char*)"LOC_CAS_NA_MSG_SMARTCARD_INVALID_ID", (char*)pszItemIdStrId, lStrLen))
	{
		*eCasMessageType = eNxCasNaMessage_SMARTCARD_INVALID;
	}
	else if (0 == HxSTD_StrNCmp((char*)"LOC_CAS_NA_MSG_SMARTCARD_SUSPENDED_ID", (char*)pszItemIdStrId, lStrLen))
	{
		*eCasMessageType = eNxCasNaMessage_SMARTCARD_SUSPENDED;
	}
	else if (0 == HxSTD_StrNCmp((char*)"LOC_CAS_NA_MSG_SMARTCARD_EXPIRED_ID", (char*)pszItemIdStrId, lStrLen))
	{
		*eCasMessageType = eNxCasNaMessage_SMARTCARD_EXPIRED;
	}
	else if (0 == HxSTD_StrNCmp((char*)"LOC_CAS_NA_MSG_SMARTCARD_NOT_CERTIFIED_ID", (char*)pszItemIdStrId, lStrLen))
	{
		*eCasMessageType = eNxCasNaMessage_SMARTCARD_NOT_CERTIFIED;
	}
	else if (0 == HxSTD_StrNCmp((char*)"LOC_CAS_NA_MSG_ACCESS_DENIED_ID", (char*)pszItemIdStrId, lStrLen))
	{
		*eCasMessageType = eNxCasNaMessage_ACCESS_DENIED;
	}
	else if (0 == HxSTD_StrNCmp((char*)"LOC_CAS_NA_MSG_ACCESS_NO_VALID_SMARTCARD_ID", (char*)pszItemIdStrId, lStrLen))
	{
		*eCasMessageType = eNxCasNaMessage_ACCESS_NO_VALID_SMARTCARD;
	}
	else if (0 == HxSTD_StrNCmp((char*)"LOC_CAS_NA_MSG_ACCESS_BLACKOUT_ID", (char*)pszItemIdStrId, lStrLen))
	{
		*eCasMessageType = eNxCasNaMessage_ACCESS_BLACKOUT;
	}
	else if (0 == HxSTD_StrNCmp((char*)"LOC_CAS_NA_MSG_ACCESS_DENIED_NO_VALID_CREDIT_ID", (char*)pszItemIdStrId, lStrLen))
	{
		*eCasMessageType = eNxCasNaMessage_ACCESS_DENIED_NO_VALID_CREDIT;
	}
	else if (0 == HxSTD_StrNCmp((char*)"LOC_CAS_NA_MSG_ACCESS_DENIED_COPY_PROTECTED_ID", (char*)pszItemIdStrId, lStrLen))
	{
		*eCasMessageType = eNxCasNaMessage_ACCESS_DENIED_COPY_PROTECTED;
	}
	else if (0 == HxSTD_StrNCmp((char*)"LOC_CAS_NA_MSG_ACCESS_DENIED_DIALOG_REQUIRED_ID", (char*)pszItemIdStrId, lStrLen))
	{
		*eCasMessageType = eNxCasNaMessage_ACCESS_DENIED_DIALOG_REQUIRED;
	}
	else if (0 == HxSTD_StrNCmp((char*)"LOC_CAS_NA_MSG_ACCESS_DENIED_PAIRING_REQUIRED_ID", (char*)pszItemIdStrId, lStrLen))
	{
		*eCasMessageType = eNxCasNaMessage_ACCESS_DENIED_PAIRING_REQUIRED;
	}
	else if (0 == HxSTD_StrNCmp((char*)"LOC_CAS_NA_MSG_ACCESS_DENIED_CS_PAIRING_REQUIRED_ID", (char*)pszItemIdStrId, lStrLen))
	{
		*eCasMessageType = eNxCasNaMessage_ACCESS_DENIED_CS_PAIRING_REQUIRED;
	}
	else
	{
		NX_APP_Error("Error!! Unknow Error Message - pszItemIdStrId :(%s)!!", pszItemIdStrId);
		*eCasMessageType = eNxCasNaMessage_Type_None;
	}

	return	ERR_OK;
}



/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

HERROR	NX_CAS_NAGRA_Init(void)
{
	nx_cas_na_OpenBannerSession();
	//nx_cas_na_OpenExtentionSession();

	NX_CAS_NAGRA_RegisterNotifier((APK_CAS_NAGRA_Notifier_t)nx_cas_na_notifyCasStateChangeFunc);

	return	ERR_OK;
}

HERROR	NX_CAS_NAGRA_RegisterNotifier(APK_CAS_NAGRA_Notifier_t fnNotifier)
{
	return	NX_PORT_CAS_NAGRA_RegisterNotifier(fnNotifier);
}

HERROR	NX_CAS_NAGRA_UnregisterNotifier(void)
{
	NX_PORT_CAS_NAGRA_UnregisterNotifier();
	return	ERR_OK;
}

HERROR	NX_CAS_NAGRA_IsCWE(HBOOL *pbCWE)
{
	return	NX_PORT_CAS_NAGRA_IsCWE(pbCWE);
}

HERROR	NX_CAS_NAGRA_IsJTAG(HBOOL *pbJTAG)
{
	return	NX_PORT_CAS_NAGRA_IsJTAG(pbJTAG);
}

HERROR	NX_CAS_NAGRA_IsSBOOT(HBOOL *pbSBOOT)
{
	return	NX_PORT_CAS_NAGRA_IsSBOOT(pbSBOOT);
}

HERROR	NX_CAS_NAGRA_IsOTP(HBOOL *pbOTP)
{
	return	NX_PORT_CAS_NAGRA_IsOTP(pbOTP);
}

HERROR	NX_CAS_NAGRA_GetNUID(HUINT8 *pszNUID)
{
	return	NX_PORT_CAS_NAGRA_GetNUID(pszNUID);
}

HERROR	NX_CAS_NAGRA_GetCASN(HUINT8 *pszCASN)
{
	return	NX_PORT_CAS_NAGRA_GetCASN(pszCASN);
}

HERROR	NX_CAS_NAGRA_GetFusingInfo(HUINT8 *pucSCS, HUINT8 *pucJTAG, HUINT8 *pucOTP, HUINT8 *pucCWE)
{
	return	NX_PORT_CAS_NAGRA_GetFusingInfo(pucSCS, pucJTAG, pucOTP, pucCWE);
}

HBOOL	NX_CAS_NAGRA_IsSCInserted(void)
{
	return	NX_PORT_CAS_NAGRA_IsSCInserted();
}

HBOOL	NX_CAS_NAGRA_PINCODE_Verify(HUINT8 *pszPinStr)
{
	return	NX_PORT_CAS_NAGRA_PINCODE_Verify(pszPinStr);
}

HBOOL	NX_CAS_NAGRA_GetAccessibleState(HUINT8 *pnAccessState, HUINT32 *pulRemainTime, HUINT8 *pnRetryCount)
{
	return	NX_PORT_CAS_NAGRA_GetAccessibleState(pnAccessState, pulRemainTime, pnRetryCount);
}

HBOOL	NX_CAS_NAGRA_VerifyPincodeGetAccessibleState(HUINT8 *pnAccessState, HUINT32 *pulRemainTime, HUINT8 *pnRetryCount, HUINT8 *pszPinStr)
{
	return	NX_PORT_CAS_NAGRA_VerifyPincodeGetAccessibleState(pnAccessState, pulRemainTime, pnRetryCount, pszPinStr);
}

//APK_CAS_NAGRA_GetAccessibleState(), APK_CAS_NAGRA_VerifyPincodeGetAccessibleState() 함수가 먼저 불린 후에, 이 함수 콜해야 한다.
HBOOL	NX_CAS_NAGRA_IsAccessibleState(HUINT8 *pnAccessState)
{
	return	NX_PORT_CAS_NAGRA_IsAccessibleState(pnAccessState);
}

HBOOL	NX_CAS_NAGRA_PINCODE_Set(HUINT8 *pszOldPinStr, HUINT8 *pszNewPinStr)
{
	return	NX_PORT_CAS_NAGRA_PINCODE_Set(pszOldPinStr, pszNewPinStr);
}

HERROR	NX_CAS_NAGRA_GetErrorMessage(HCHAR *pszItemIdStrId, NX_CasNaMessage_Type *eCasMessageType)
{
	return	nx_cas_na_ConvertMwCASErrorMsgStrIdToApCASErrorType_Base(pszItemIdStrId, eCasMessageType);
}



/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/
