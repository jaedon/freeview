/*
 * (c) 2011-2012 Humax Co., Ltd.
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
#include <octo_common.h>
//#include <ap_resource.h> // hjlee3
#include <svc_sys.h>

#include "va_ui.h"
#include "va_ui_adapt.h"

#include "./local_include/_xmgr_cas_va.h"
#include "./local_include/_xmgr_cas_va_util.h"
#include "./local_include/_xmgr_cas_va_sc.h"


#define SMARTCARD_SLOT_NUM		0

//extern HUINT8 *VA_UI_Adapt_GetHwVersionStr();


HERROR xmgr_cas_VaGetOpList(VA_MENU_OP_LIST *pOp)
{
	if(pOp == NULL)
	{
		return ERR_OK;
	}

	pOp->uiOpNum = 0;
	pOp->pOpList = VA_UI_Adapt_GetSoidList(SMARTCARD_SLOT_NUM, &pOp->uiOpNum);

	return ERR_OK;
}

HERROR xmgr_cas_VaGetOpClass(VA_UI_OPERATOR_LIST *pOp, VA_MENU_CLASS_CONSULTATION *pClass, HINT32 *pResult)
{
	if(pOp == NULL || pClass == NULL || pResult == NULL)
	{
		return ERR_FAIL;
	}

	pClass->uiNum = 0;
	pClass->pList =	VA_UI_Adapt_GetClassList(SMARTCARD_SLOT_NUM, pOp->ulSoid, &pClass->uiNum, pResult);

	return ERR_OK;
}

HERROR xmgr_cas_VaGetOpTheme(VA_UI_OPERATOR_LIST *pOp, VA_MENU_THEME_CONSULTATION *pTheme, HINT32 *pResult)
{
	if(pOp == NULL || pTheme == NULL || pResult == NULL)
	{
		return ERR_FAIL;
	}

	pTheme->uiNum = 0;
	pTheme->pList =	VA_UI_Adapt_GetThemeLevelList(SMARTCARD_SLOT_NUM, pOp->ulSoid, &pTheme->uiNum, pResult);

	return ERR_OK;
}

HERROR xmgr_cas_VaGetOpPrebook(VA_UI_OPERATOR_LIST *pOp, VA_MENU_PREBOOK_CONSULTATION *pPrebook, HINT32 *pResult)
{
	if(pOp == NULL || pPrebook == NULL || pResult == NULL)
	{
		return ERR_FAIL;
	}

	pPrebook->uiNum = 0;
	pPrebook->pList = VA_UI_Adapt_GetPrebookedList(SMARTCARD_SLOT_NUM, pOp->ulSoid, &pPrebook->uiNum, pResult);

	return ERR_OK;
}

HERROR xmgr_cas_VaGetOpLastMessage(VA_UI_OP_MESSAGE **ppOpMsg)
{
	*ppOpMsg = VA_UI_Adapt_GetLastOpMessage(SMARTCARD_SLOT_NUM);

	return ERR_OK;
}

HERROR xmgr_cas_VaGetSmartcardInfo(VA_MENU_SMARTCARD_INFO *pInfo)
{
	if(pInfo == NULL)
	{
		return ERR_FAIL;
	}

	pInfo->uiPairMode = VA_UI_Adapt_GetPairingMode();

	if(pInfo->uiPairMode == VA_UI_AUTOPAIRING_MODE)
	{
		pInfo->uiPairNum = VA_UI_Adapt_GetPairingSC();
		pInfo->pUaList = VA_UI_Adapt_GetPairingUaList(&pInfo->uiUaNum);
	}

	return ERR_OK;
}

HERROR xmgr_cas_VaGetStbInfo(VA_MENU_STB_INFO *pInfo)
{
	HUINT32	ulVersion = 0;

	if(pInfo == NULL)
	{
		return ERR_FAIL;
	}

	pInfo->uiManufId = VA_UI_Adapt_GetManufacturerId();
	pInfo->uiModelId = VA_UI_Adapt_GetModelId();
	pInfo->pszTerminalSn = VA_UI_Adapt_GetStbSerialStr();
	pInfo->pszHwVer = VA_UI_Adapt_GetHwVersionStr();// 주의 할 것!! 하드웨어 버전이 바뀔 경우 내부 함수의 매크로 값을 수정할 것!!

	SVC_SYS_GetSystemVersion(eVERSION_TYPE_LOADER, &ulVersion);
	pInfo->pszBlVer = GetTextBuffer(TEXT_SIZE128);
	SVC_SYS_MakeSystemVerString(eVERSION_TYPE_LOADER, ulVersion, pInfo->pszBlVer);		// 뒤에 5자리만 필요함.

	//Software Version
	SVC_SYS_GetSystemVersion(eVERSION_TYPE_APPL, &ulVersion);
	pInfo->pszSwVer = GetTextBuffer(TEXT_SIZE128);
	SVC_SYS_MakeSystemVerString(eVERSION_TYPE_APPL, ulVersion, pInfo->pszSwVer);

	pInfo->pszStbId = VA_UI_Adapt_GetStbId();
#ifdef CONFIG_MW_CAS_VIACCESS_PVR
	//swlee3 : device name 하드 코딩 된 부분 추후 수정되어야 함.
	if(SVC_SYS_GetHddSerialNumber("dev/sda1", &pInfo->pszHddSn) == ERR_FAIL)
	{
		pInfo->pszHddSn = "N/A";
	}
#endif
	pInfo->pszUA = VA_UI_Adapt_GetUA(SMARTCARD_SLOT_NUM);
#ifdef CONFIG_MW_CAS_VIACCESS_PURPLE_BOX
	pInfo->pszTerminalId;
#endif
	pInfo->pszAcsVer = VA_UI_Adapt_GetVersionName();
	pInfo->pszCamlock = VA_UI_Adapt_GetCamlockStr();

	return ERR_OK;
}

HERROR xmgr_cas_VaGetPinCodeAttemps(HUINT8 *pucRemain, HUINT8 *pucMax, HINT32 *pResult)
{
	if(pucRemain == NULL || pucMax == NULL || pResult == NULL)
	{
		return ERR_FAIL;
	}

	*pucRemain = 0;

	*pResult = VA_UI_Adapt_GetPinCodeAttempts(SMARTCARD_SLOT_NUM, pucRemain, pucMax);

	return ERR_OK;
}

HERROR xmgr_cas_VaChangePinCode(HUINT8 *pszOld, HUINT8 *pszNew, HINT32 *pResult)
{
	if(pszOld == NULL || pszNew == NULL || pResult == NULL)
	{
		return ERR_FAIL;
	}

	*pResult = VA_UI_Adapt_ModifyPinCode(SMARTCARD_SLOT_NUM, pszOld, pszNew);

	return ERR_OK;
}

HERROR xmgr_cas_VaGetMaturityRate(HUINT8 *pucCurrentMR, HUINT8 *pucMaxMR, HINT32 *pResult)
{
	if(pucCurrentMR == NULL || pucMaxMR == NULL || pResult == NULL)
	{
		return ERR_FAIL;
	}

	*pResult = VA_UI_Adapt_GetMaturityRating(SMARTCARD_SLOT_NUM, pucCurrentMR, pucMaxMR);

	return ERR_OK;
}

HERROR xmgr_cas_VaChangeMaturityRate(HUINT8 *pszPin, HUINT8 ucMaturityRate, HINT32 *pResult)
{
	if(pszPin == NULL || pResult == NULL)
	{
		return ERR_FAIL;
	}

	*pResult = VA_UI_Adapt_ModifyMaturityRating(SMARTCARD_SLOT_NUM, pszPin, ucMaturityRate);

	return (*pResult == VA_ERR_OK ? ERR_OK : ERR_FAIL);
}

HERROR xmgr_cas_VaGetMaturityRateValue(HUINT32 ulAcsId, HUINT32 ulStbStreamHandle, HUINT8 *pucMaturityRatingValue, HINT32 *pResult)
{
	if(pucMaturityRatingValue == NULL || pResult == NULL)
	{
		return ERR_FAIL;
	}

	*pResult = VA_UI_Adapt_GetMaturityRatingValue(ulAcsId, ulStbStreamHandle, pucMaturityRatingValue);

	return (*pResult == VA_ERR_OK ? ERR_OK : ERR_FAIL);
}

HERROR xmgr_cas_VaSetMaturityRateValue(HUINT32 ulAcsId, HUINT8 *pszPin, HINT32 *pResult)
{
	if(pszPin == NULL || pResult == NULL)
	{
		return ERR_FAIL;
	}

	*pResult= VA_UI_Adapt_AcceptMaturityRating(ulAcsId, SMARTCARD_SLOT_NUM, pszPin);

	return (*pResult == VA_ERR_OK ? ERR_OK : ERR_FAIL);
}

HERROR xmgr_cas_VaUnlockConsultation(HUINT8 *pszPin, HINT32 *pResult)
{
	if(pszPin == NULL || pResult == NULL)
	{
		return ERR_FAIL;
	}

	*pResult = VA_UI_Adapt_UnlockConsultation(SMARTCARD_SLOT_NUM, pszPin);

	return (*pResult == VA_ERR_OK ? ERR_OK : ERR_FAIL);
}

HERROR xmgr_cas_VaGetPreViewInfo(HUINT32 ulAcsId, HUINT32 ulStbStreamHandle, HUINT8 *pucUsedCwNumber, HUINT8 *pucMaxCwNumber, HINT32 *pResult)
{
	if(pucUsedCwNumber == NULL || pucMaxCwNumber== NULL || pResult == NULL)
	{
		return ERR_FAIL;
	}

	*pResult = VA_UI_Adapt_GetPreviewInfo(ulAcsId, ulStbStreamHandle, pucUsedCwNumber, pucMaxCwNumber);

	return (*pResult == VA_ERR_OK ? ERR_OK : ERR_FAIL);
}

HERROR xmgr_cas_VaGetSmartcardState(CAS_VA_SC_State_t *pScState)
{
	if(pScState == NULL)
	{
		return ERR_FAIL;
	}

	pScState->nSecurity = VA_UI_Adapt_GetSecurityState(SMARTCARD_SLOT_NUM);
	pScState->nCamLockCtrlLevel = VA_UI_Adapt_GetCamlockLevel(SMARTCARD_SLOT_NUM);
	pScState->nCamlock = VA_UI_Adapt_GetCamlockState(SMARTCARD_SLOT_NUM);
	pScState->nState = VA_UI_Adapt_GetSmartcardState(SMARTCARD_SLOT_NUM);

	return ERR_OK;
}

#ifdef CONFIG_MW_CAS_VIACCESS_PVR
HERROR xmgr_cas_VaGetPlaybackInfo(VA_UI_PLAYBACK_INFO *pstInfo)
{
	if(pstInfo == NULL)
	{
		return ERR_FAIL;
	}

	if(VA_UI_Adapt_GetPlaybackInfoNow(pstInfo) != VA_ERR_OK)
	{
		VK_memset32(pstInfo, 0, sizeof(VA_UI_PLAYBACK_INFO));
	}

	return ERR_OK;
}
#endif
