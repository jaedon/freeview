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
/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <vamgr_main.h>
#ifdef DxMAX_TS_NUM
#undef DxMAX_TS_NUM
#endif

#include "./local_include/_xmgr_cas_va_adapt.h"
#include "./local_include/_xmgr_cas_va_util.h"
#include "./local_include/_xmgr_cas_va_ui.h"
#include "./local_include/_xmgr_cas_va.h"
#include <xmgr_cas_res_str.h>

/*******************************************************************/
/********************      Definitions         **************************/
/*******************************************************************/





/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

STATIC HINT32	s_nLocId;

#if defined(CONFIG_OP_NORDIG_BOXER)
STATIC HUINT32	s_ulBoxerErrCode;
#endif

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/



/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

#if defined(CONFIG_OP_NORDIG_BOXER)
typedef	enum
{
	eBoxer_ErrCode_No_Error = 0,
	eBoxer_ErrCode_01_SC_NOT_FOUND,
	eBoxer_ErrCode_02_SC_FAIL,
	eBoxer_ErrCode_03_SC_EXTRACT,
	eBoxer_ErrCode_04_NO_RIGHT,
	eBoxer_ErrCode_05_NOT_APPLICABLE,
	eBoxer_ErrCode_06_SC_EXTRACT_PAIRED_STB,
	eBoxer_ErrCode_07_SC_NOT_ACCEPTED,
	eBoxer_ErrCode_08_CW_PROT_MISMATCH,
	eBoxer_ErrCode_09_SC_INVALID,
	eBoxer_ErrCode_10_GEOGRAPHICAL_BLACKOUT,
	eBoxer_ErrCode_11_MISSING_KEY,
	eBoxer_ErrCode_12_CAMLOCK_ACTIVATED_NO_CARD,
	eBoxer_ErrCode_13_CAMLOCK_ACTIVATED_KO,
	eBoxer_ErrCode_14_DESC_KO_MR,
	eBoxer_ErrCode_15_PAIRING_MISMATCH,
	eBoxer_ErrCode_16_SC_SNSUFFICIENT_MEM,
	eBoxer_ErrCode_19_DESCRABLING_KO_FORBIDDEN = 19,
	eBoxer_ErrCode_20_LOCKED_PINCODE_STATUS	= 20,
	eBoxer_ErrCode_Max	// not use !!
}VA_BOXER_ErrCode_e;
#endif

#ifdef CONFIG_DEBUG
extern HUINT8 *VA_UI_Debug_GetEventName(VA_UI_EVENT_TYPE eEventType);
extern HUINT8 *VA_UI_Debug_GetCamlockLevel(HINT32 nCamlockCtrlLevel);
extern HUINT8 *VA_UI_Debug_GetCamlockState(HINT32 nCamlock);
extern HUINT8 *VA_UI_Debug_GetScState(HINT32 nScState);
#endif

/*******************************************************************/
/********************      Local Functions      ********************/
/*******************************************************************/

#define _____AP_CAS_VA_ADAPT_LOCAL_FUNCTION______________________________________________________

#if defined(CONFIG_DEBUG)

//STATIC HUINT8 *local_cas_va_get_ui_state_name(eCAS_VA_UI_STATE eUiState)
STATIC HUINT8 *xmgr_cas_VaGetUiStateName(eCAS_VA_UI_STATE eUiState)
{
	switch(eUiState)
	{
		case eCAS_VA_UI_None:				return "VA_UI_None";
		case eCAS_VA_UI_Error:				return "VA_UI_Error";
		case eCAS_VA_UI_AgreeMaturityRate:	return "VA_UI_AgreeMaturityRate";
		case eCAS_VA_UI_NewOpMessage:		return "VA_UI_NewOpMessage";
		case eCAS_VA_UI_PreView:			return "VA_UI_PreView";
		case eCAS_VA_UI_CamlockOk:			return "VA_UI_CamlockOk";
		default:							return "Unknown";
	}
}

#endif

#if 0	// compile warning
STATIC HERROR local_cas_va_adapt_get_locid_process(VA_UI_EVENT *pEvent, CAS_VA_SC_State_t *pScState)
{
	if(pScState->nState != VA_UI_SMARTCARD_READY)
	{
		if(	(pEvent->eEventType == VA_UI_SMARTCARD_EXTRACTED) ||
			(pEvent->eEventType == VA_UI_SMARTCARD_FAILED) ||
			(pEvent->eEventType == VA_UI_SMARTCARD_NOT_ACCEPTED) ||
			(pEvent->eEventType == VA_UI_SMARTCARD_INVALIDATED) )
		{
			return ERR_OK;
		}

		if(pEvent->eEventType == VA_UI_CAMLOCK_ACTIVATED_NO_CARD)
		{
			return ERR_OK;
		}
	}

	if(pScState->nState == VA_UI_SMARTCARD_READY)
	{
		if(pEvent->eEventType == VA_UI_CAMLOCK_ACTIVATED_KO)
		{
			return ERR_OK;
		}

		if(pScState->nCamlock == VA_UI_CAMLOCK_ACTIVATED_OK)
		{
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}
#endif


#if defined(CONFIG_OP_NORDIG_BOXER)

STATIC HERROR xmgr_cas_VaAdaptGetEsErrorMessage(HINT32 *pnLocId, VA_UI_EVENT_TYPE eEventType, VA_ES_TYPE eDscrErrEsType)
{
	switch(eEventType)
	{
//## 이걸 처리할 일이 있을까??
	case VA_UI_DESCRAMBLING_OK_RIGHT_OK:
		if(eDscrErrEsType == VA_ES_ALL)
			*pnLocId = LOC_CAS_VA_BOXER_ERR_DESC_NO_RIGHT_VIDEO_AUDIO_ID;
		else if(eDscrErrEsType == VA_ES_VIDEO)
			*pnLocId = LOC_CAS_VA_ERR_DESC_NO_RIGHT_VIDEO_ID;
		else if(eDscrErrEsType == VA_ES_AUDIO)
			*pnLocId = LOC_CAS_VA_ERR_DESC_NO_RIGHT_AUDIO_ID;
		else
			;
		break;

	case VA_UI_DESCRAMBLING_KO_NO_RIGHT:
		if(eDscrErrEsType == VA_ES_ALL)
			*pnLocId = LOC_CAS_VA_BOXER_ERR_DESC_NO_RIGHT_VIDEO_AUDIO_ID;
		else if(eDscrErrEsType == VA_ES_VIDEO)
			*pnLocId = LOC_CAS_VA_ERR_DESC_NO_RIGHT_VIDEO_ID;
		else if(eDscrErrEsType == VA_ES_AUDIO)
			*pnLocId = LOC_CAS_VA_ERR_DESC_NO_RIGHT_AUDIO_ID;
		else
			;
		break;

	case VA_UI_DESCRAMBLING_KO_GEOGRAPHICAL_BLACKOUT:
		if(eDscrErrEsType == VA_ES_ALL)
			*pnLocId = LOC_CAS_VA_BOXER_ERR_DESC_GEOGRAPHICAL_BLACKOUT_VIDEO_AUDIO_ID;
		else if(eDscrErrEsType == VA_ES_VIDEO)
			*pnLocId = LOC_CAS_VA_ERR_DESC_GEOGRAPHICAL_BLACKOUT_VIDEO_ID;
		else if(eDscrErrEsType == VA_ES_AUDIO)
			*pnLocId = LOC_CAS_VA_ERR_DESC_GEOGRAPHICAL_BLACKOUT_AUDIO_ID;
		else
			;
		break;

	case VA_UI_DESCRAMBLING_KO_MISSING_KEY:
		if(eDscrErrEsType == VA_ES_ALL)
			*pnLocId = LOC_CAS_VA_BOXER_ERR_DESC_MISSING_KEY_VIDEO_AUDIO_ID;
		else if(eDscrErrEsType == VA_ES_VIDEO)
			*pnLocId = LOC_CAS_VA_ERR_DESC_MISSING_KEY_VIDEO_ID;
		else if(eDscrErrEsType == VA_ES_AUDIO)
			*pnLocId = LOC_CAS_VA_ERR_DESC_MISSING_KEY_AUDIO_ID;
		else
			;
		break;

	case VA_UI_DESCRAMBLING_KO_FORBIDDEN_MODE:
		if(eDscrErrEsType == VA_ES_ALL)
			*pnLocId = LOC_CAS_VA_BOXER_ERR_DESCRAMBLING_KO_FORBIDDEN_ID;
		else if(eDscrErrEsType == VA_ES_VIDEO)
			*pnLocId = LOC_CAS_VA_ERR_DESC_FORBIDDEN_MODE_VIDEO_ID;
		else if(eDscrErrEsType == VA_ES_AUDIO)
			*pnLocId = LOC_CAS_VA_ERR_DESC_FORBIDDEN_MODE_AUDIO_ID;
		else
			;
		break;

	default:
		break;
	}
	return ERR_OK;
}

STATIC HERROR xmgr_cas_VaAdaptMessageFunction(void *pData, void *pSc, void *pUi)
{
	HINT32 nLocId = -1;
	VA_UI_EVENT *pEvent = (VA_UI_EVENT *)pData;
	eCAS_VA_UI_STATE *pUiState = (eCAS_VA_UI_STATE *)pUi;
	CAS_VA_SC_State_t *pScState = (CAS_VA_SC_State_t *)pSc;
	VA_UI_EVENT_TYPE eEventType;
	HUINT32		ulBoxerErrCode;

	if(pEvent == NULL)
	{
		HxLOG_Error("pEvent NULL\n");
		return ERR_FAIL;
	}

	if(pUiState == NULL)
	{
		HxLOG_Error("pUiState NULL\n");
		return ERR_FAIL;
	}

	if(pScState == NULL)
	{
		HxLOG_Error("pScState NULL\n");
		return ERR_FAIL;
	}

	ApCasVaAdaptPrint(DBG_TRACE, ("%s : Security(%d) CamLockCtrlLevel(%s) Camlock(%s) ScState(%s) PrevUiState(%s)\n",
		(HUINT8 *)VA_UI_Debug_GetEventName(pEvent->eEventType),
		pScState->nSecurity,
		(HUINT8 *)VA_UI_Debug_GetCamlockLevel(pScState->nCamLockCtrlLevel), (HUINT8 *)VA_UI_Debug_GetCamlockState(pScState->nCamlock),
		(HUINT8 *)VA_UI_Debug_GetScState(pScState->nState), xmgr_cas_VaGetUiStateName(*pUiState)));

//##없어도 될듯..
/*
	if(local_cas_va_adapt_get_locid_process(pEvent, pScState) == ERR_FAIL)
	{
		return ERR_OK;
	}
*/

	*pUiState = eCAS_VA_UI_Error;
	ulBoxerErrCode = eBoxer_ErrCode_No_Error;

	// security restriction이 걸리면 다른 거 처리 안함.
	if (pScState->nSecurity)
	{
		eEventType = VA_UI_START_SECURITY_RESTRICTION;
	}
	else
	{
		eEventType = pEvent->eEventType;
	}

	switch (eEventType)
	{
		// UC01 simply notify (SC0103: without message)
		case VA_UI_DESCRAMBLING_OK:
		case VA_UI_FREE_TO_AIR:
		case VA_UI_STOP_SECURITY_RESTRICTION:
		case VA_UI_CAMLOCK_NOT_ACTIVATED:
		case VA_UI_CAMLOCK_DISABLED:
		case VA_UI_DESCRAMBLING_KO:
		case VA_UI_SMARTCARD_READY:
		case VA_UI_SMARTCARD_INSERTED:
			*pUiState = eCAS_VA_UI_None;
			break;

		case VA_UI_CAMLOCK_ACTIVATED_OK:
			*pUiState = eCAS_VA_UI_CamlockOk;
			break;

		// UC01 simply notify (SC0101 or SC0102: without/with video)
		case VA_UI_DESCRAMBLING_OK_RIGHT_OK:
			xmgr_cas_VaAdaptGetEsErrorMessage(&nLocId, pEvent->eEventType, pEvent->eDscrErrEsType);
			break;

		case VA_UI_DESCRAMBLING_KO_NO_RIGHT:
			xmgr_cas_VaAdaptGetEsErrorMessage(&nLocId, pEvent->eEventType, pEvent->eDscrErrEsType);
			ulBoxerErrCode = eBoxer_ErrCode_04_NO_RIGHT;
			break;

		case VA_UI_DESCRAMBLING_KO_GEOGRAPHICAL_BLACKOUT:
			xmgr_cas_VaAdaptGetEsErrorMessage(&nLocId, pEvent->eEventType, pEvent->eDscrErrEsType);
			ulBoxerErrCode = eBoxer_ErrCode_10_GEOGRAPHICAL_BLACKOUT;
			break;

		case VA_UI_DESCRAMBLING_KO_MISSING_KEY:
			xmgr_cas_VaAdaptGetEsErrorMessage(&nLocId, pEvent->eEventType, pEvent->eDscrErrEsType);
			ulBoxerErrCode = eBoxer_ErrCode_11_MISSING_KEY;
			break;

		case VA_UI_DESCRAMBLING_KO_FORBIDDEN_MODE:
			xmgr_cas_VaAdaptGetEsErrorMessage(&nLocId, pEvent->eEventType, pEvent->eDscrErrEsType);
			ulBoxerErrCode = eBoxer_ErrCode_19_DESCRABLING_KO_FORBIDDEN;
			break;

		// event for schip only
		case VA_UI_DESCRAMBLING_KO_CW_PROTECTION_MISMATCH:
			nLocId = LOC_CAS_VA_BOXER_ERR_DESC_CW_PROTECTION_MISMATCH_ID;
			ulBoxerErrCode = eBoxer_ErrCode_08_CW_PROT_MISMATCH;
			break;

		// UC01 simply notify (SC0101: without video)
		case VA_UI_VIACCESS_NOT_APPLICABLE:
			nLocId = LOC_CAS_VA_BOXER_ERR_VIACCESS_NOT_APPLICABLE_ID;
			ulBoxerErrCode = eBoxer_ErrCode_05_NOT_APPLICABLE;
			break;

		case VA_UI_VIACCESS_SMARTCARD_NOT_FOUND:
			#if 0 // org
			nLocId = LOC_CAS_VA_BOXER_MSG_SC_VIACCESS_SMARTCARD_NOT_FOUND_ID;
			#else
			// VA_UI_VIACCESS_SMARTCARD_NOT_FOUND_EVENT 이후에 VA_UI_SMARTCARD_READY_EVENT 가 발생하는 경우,
			// pScState->nState 값은 여기 처리 직전에 읽어서 갱신이 되나 VA_UI_SMARTCARD_READY_EVENT 는 이 다음에 올라오므로
			// 불필요하게 "Please insert the smartcard to access the program." 메시지가 잠깐 보이게 된다.
			// 아래 코드는 이를 수정한 것임.
			if (pScState->nState == VA_UI_SMARTCARD_INSERTED || pScState->nState == VA_UI_SMARTCARD_READY)
			{
				*pUiState = eCAS_VA_UI_None;
			}
			else
			{
				nLocId = LOC_CAS_VA_BOXER_MSG_SC_VIACCESS_SMARTCARD_NOT_FOUND_ID;
				ulBoxerErrCode = eBoxer_ErrCode_01_SC_NOT_FOUND;
			}
			#endif
			break;

		case VA_UI_SMARTCARD_EXTRACTED:
			{
				HINT32 lPairingMode = VA_UI_Adapt_GetPairingMode();

				if ((lPairingMode != VA_UI_ERR)
					&& (lPairingMode  != VA_UI_NO_MODE))
				{
					// pairing set
					nLocId = LOC_CAS_VA_BOXER_MSG_SC_EXTRACTED_PAIRED_STB_ID;
					ulBoxerErrCode = eBoxer_ErrCode_06_SC_EXTRACT_PAIRED_STB;
				}
				else
				{
					// not pairing set
					nLocId = LOC_CAS_VA_BOXER_MSG_SC_EXTRACTED_ID;
					ulBoxerErrCode = eBoxer_ErrCode_03_SC_EXTRACT;
				}
			}
			break;

		case VA_UI_SMARTCARD_NEEDED:
			nLocId = LOC_CAS_VA_MSG_SC_NEEDED_ID;
			break;

		case VA_UI_SMARTCARD_FAILED:
			nLocId = LOC_CAS_VA_BOXER_MSG_SC_FAILED_ID;
			ulBoxerErrCode = eBoxer_ErrCode_02_SC_FAIL;

#if defined(CONFIG_OP_NORDIG)
			if (BUS_MGR_Get(AP_System_Proc) != NULL)
			{
				// Message 중복 되는 경우 막음
				AP_CAS_VA_HideMessageBox();
			}
#endif

			break;

		case VA_UI_SMARTCARD_NOT_ACCEPTED:
			nLocId = LOC_CAS_VA_BOXER_MSG_SC_NOT_ACCEPTED_ID;
			ulBoxerErrCode = eBoxer_ErrCode_07_SC_NOT_ACCEPTED;
			break;

		case VA_UI_SMARTCARD_INSUFFICIENT_MEMORY:
			nLocId = LOC_CAS_VA_BOXER_MSG_SC_INSUFFICIENT_MEMORY_ID;
			ulBoxerErrCode = eBoxer_ErrCode_16_SC_SNSUFFICIENT_MEM;
			break;

		case VA_UI_SMARTCARD_INVALIDATED:
			nLocId = LOC_CAS_VA_BOXER_MSG_SC_INVALIDATED_ID;
			ulBoxerErrCode = eBoxer_ErrCode_09_SC_INVALID;
			break;

		case VA_UI_CAMLOCK_ACTIVATED_NO_CARD:
			nLocId = LOC_CAS_VA_BOXER_CAM_ACTIVE_NO_CARD_ID;
			ulBoxerErrCode = eBoxer_ErrCode_12_CAMLOCK_ACTIVATED_NO_CARD;
			break;

		// event for schip only
		case VA_UI_PAIRING_MISMATCH:
			nLocId = LOC_CAS_VA_BOXER_ERR_PAIRING_MISMATCH_ID;
			ulBoxerErrCode = eBoxer_ErrCode_15_PAIRING_MISMATCH;
			break;

		// UC02 operator message
		case VA_UI_NEW_OPERATOR_MESSAGE:
			*pUiState = eCAS_VA_UI_NewOpMessage;
			break;

		// UC03 camlock
		case VA_UI_CAMLOCK_ACTIVATED_KO:
//## camlock ko인경우, smartcard 상태를 확인하여 메시지를 선택한다.
			switch(pScState->nState)
			{
			case VA_UI_SC_EXTRACTED:
				nLocId = LOC_CAS_VA_BOXER_CAM_ACTIVE_NO_CARD_ID;
				ulBoxerErrCode = eBoxer_ErrCode_12_CAMLOCK_ACTIVATED_NO_CARD;
				break;

			case VA_UI_SC_FAILED:
				nLocId = LOC_CAS_VA_BOXER_MSG_SC_FAILED_ID;
				ulBoxerErrCode = eBoxer_ErrCode_02_SC_FAIL;
				break;

			case VA_UI_SC_NOT_ACCEPTED:
				nLocId = LOC_CAS_VA_BOXER_MSG_SC_NOT_ACCEPTED_ID;
				ulBoxerErrCode = eBoxer_ErrCode_07_SC_NOT_ACCEPTED;
				break;

			case VA_UI_SC_INVALIDATED:
				nLocId = LOC_CAS_VA_BOXER_MSG_SC_INVALIDATED_ID;
				ulBoxerErrCode = eBoxer_ErrCode_09_SC_INVALID;
				break;

			case VA_UI_SC_INSERTED:
			case VA_UI_SC_READY:
			default:
				nLocId = LOC_CAS_VA_BOXER_ERR_CAMLOCK_ID;
				ulBoxerErrCode = eBoxer_ErrCode_13_CAMLOCK_ACTIVATED_KO;
				break;
			}
			break;

		// UC04 security restriction
		case VA_UI_START_SECURITY_RESTRICTION:
			nLocId = LOC_CAS_VA_ERR_SECURITY_RESTRICTION_ID;
			break;

		// UC05 maturity rating agreement
		case VA_UI_DESCRAMBLING_KO_MATURITY_RATING:
			nLocId = LOC_CAS_VA_BOXER_MSG_MR_INIT1_ID;
			ulBoxerErrCode = eBoxer_ErrCode_14_DESC_KO_MR;
			break;

		// UC06 preview
		case VA_UI_DESCRAMBLING_OK_PREVIEW:
			nLocId = LOC_CAS_VA_MSG_PREVIEW_ID;
			break;

		case VA_UI_NEW_OPERATOR_DATA:
			break;

#ifdef CONFIG_MW_CAS_VIACCESS_IPPV
		// event for ippv only
		case VA_UI_DESCRAMBLING_KO_IPPV_OFFER:							// UC07
		case VA_UI_DESCRAMBLING_KO_INSUFFICIENT_CREDIT:				// UC01- SC0101
		case VA_UI_DESCRAMBLING_OK_PREVIEW_AND_IPPV_OFFER:			// UC07
		case VA_UI_DESCRAMBLING_KO_IPPV_OFFER_EXPIRED:					// UC01- SC0101
		case VA_UI_DESCRAMBLING_KO_MATURITY_RATING_AND_IPPV_OFFER:	// UC07
		case VA_UI_OVERDRAFT_REACHED:									// UC01- SC0101
		case VA_UI_THRESHOLD_REACHED:									// UC01- SC0101
			HxLOG_Info("event (%d) is for IPPV only: not implemented\n", pEvent->eEventType);
			break;
#endif

		// event for pvr only
		case VA_UI_DESCRAMBLING_KO_RIGHT_EXPIRED:						// UC01- SC0101
		case VA_UI_DESCRAMBLING_KO_RIGHT_CONSUMED:					// UC01- SC0101
		case VA_UI_RECORD_RIGHT_KO:
		case VA_UI_TIMESHIFT_RIGHT_KO:
			HxLOG_Info("event (%d) is for PVR only: not implemented\n", pEvent->eEventType);
			break;

#ifdef CONFIG_MW_CAS_VIACCESS_PURPLE_BOX
		// event for purple box or IP box only (must not be implemented)
		case VA_UI_VIACCESS_TERMINAL_NOT_ACTIVATED:
		case VA_UI_ORM_CONNECTION_OK:
		case VA_UI_ORM_CONNECTION_KO:
		case VA_UI_CW_SERVER_CONNECTION_OK:
		case VA_UI_CW_SERVER_CONNECTION_KO:
		case VA_UI_TERMINAL_NOT_ACTIVATED:								// UC19
		case VA_UI_TERMINAL_READY:
		case VA_UI_TERMINAL_INSUFFICIENT_MEMORY:
		case VA_UI_TERMINAL_CONFIGURATION_ERROR:
		case VA_UI_TERMINAL_INVALIDATED:
		case VA_UI_TERMINAL_PERSONALIZATION_OK:
		case VA_UI_TERMINAL_PERSONALIZATION_KO:
			HxLOG_Info("event (%d) is for Purple box only: not implemented\n", pEvent->eEventType);
			break;
#endif

		default:
			HxLOG_Error("event (%x) is unknown\n", pEvent->eEventType);
			return ERR_FAIL;
	}

	s_nLocId = nLocId;
	s_ulBoxerErrCode = ulBoxerErrCode;

	return ERR_OK;
}
#else	// #if defined(CONFIG_OP_NORDIG_BOXER)

STATIC HERROR xmgr_cas_VaAdaptGetEsErrorMessage(HINT32 *pnLocId, VA_UI_EVENT_TYPE eEventType, VA_ES_TYPE eDscrErrEsType)
{
	#if 0  // hjlee3, just to build
	switch(eEventType)
	{
//## 이걸 처리할 일이 있을까??
	case VA_UI_DESCRAMBLING_OK_RIGHT_OK:
		if(eDscrErrEsType == VA_ES_ALL)
			*pnLocId = LOC_CAS_VA_ERR_DESC_NO_RIGHT_VIDEO_AUDIO_ID;
		else if(eDscrErrEsType == VA_ES_VIDEO)
			*pnLocId = LOC_CAS_VA_ERR_DESC_NO_RIGHT_VIDEO_ID;
		else if(eDscrErrEsType == VA_ES_AUDIO)
			*pnLocId = LOC_CAS_VA_ERR_DESC_NO_RIGHT_AUDIO_ID;
		else
			;
		break;

	case VA_UI_DESCRAMBLING_KO_NO_RIGHT:
		if(eDscrErrEsType == VA_ES_ALL)
			*pnLocId = LOC_CAS_VA_ERR_DESC_NO_RIGHT_VIDEO_AUDIO_ID;
		else if(eDscrErrEsType == VA_ES_VIDEO)
			*pnLocId = LOC_CAS_VA_ERR_DESC_NO_RIGHT_VIDEO_ID;
		else if(eDscrErrEsType == VA_ES_AUDIO)
			*pnLocId = LOC_CAS_VA_ERR_DESC_NO_RIGHT_AUDIO_ID;
		else
			;
		break;

	case VA_UI_DESCRAMBLING_KO_GEOGRAPHICAL_BLACKOUT:
		if(eDscrErrEsType == VA_ES_ALL)
			*pnLocId = LOC_CAS_VA_ERR_DESC_GEOGRAPHICAL_BLACKOUT_VIDEO_AUDIO_ID;
		else if(eDscrErrEsType == VA_ES_VIDEO)
			*pnLocId = LOC_CAS_VA_ERR_DESC_GEOGRAPHICAL_BLACKOUT_VIDEO_ID;
		else if(eDscrErrEsType == VA_ES_AUDIO)
			*pnLocId = LOC_CAS_VA_ERR_DESC_GEOGRAPHICAL_BLACKOUT_AUDIO_ID;
		else
			;
		break;

	case VA_UI_DESCRAMBLING_KO_MISSING_KEY:
		if(eDscrErrEsType == VA_ES_ALL)
			*pnLocId = LOC_CAS_VA_ERR_DESC_MISSING_KEY_VIDEO_AUDIO_ID;
		else if(eDscrErrEsType == VA_ES_VIDEO)
			*pnLocId = LOC_CAS_VA_ERR_DESC_MISSING_KEY_VIDEO_ID;
		else if(eDscrErrEsType == VA_ES_AUDIO)
			*pnLocId = LOC_CAS_VA_ERR_DESC_MISSING_KEY_AUDIO_ID;
		else
			;
		break;

	case VA_UI_DESCRAMBLING_KO_FORBIDDEN_MODE:
		if(eDscrErrEsType == VA_ES_ALL)
			*pnLocId = LOC_CAS_VA_ERR_DESC_FORBIDDEN_MODE_VIDEO_AUDIO_ID;
		else if(eDscrErrEsType == VA_ES_VIDEO)
			*pnLocId = LOC_CAS_VA_ERR_DESC_FORBIDDEN_MODE_VIDEO_ID;
		else if(eDscrErrEsType == VA_ES_AUDIO)
			*pnLocId = LOC_CAS_VA_ERR_DESC_FORBIDDEN_MODE_AUDIO_ID;
		else
			;
		break;
	default:
		break;
	}
	#endif
	return ERR_OK;
}

STATIC HERROR xmgr_cas_VaAdaptMessageFunction(void *pData, void *pSc, void *pUi)
{
	HINT32 nLocId = -1;
	VA_UI_EVENT *pEvent = (VA_UI_EVENT *)pData;
	eCAS_VA_UI_STATE *pUiState = (eCAS_VA_UI_STATE *)pUi;
	CAS_VA_SC_State_t *pScState = (CAS_VA_SC_State_t *)pSc;
	VA_UI_EVENT_TYPE eEventType;

	if(pEvent == NULL)
	{
		HxLOG_Error("pEvent NULL\n");
		return ERR_FAIL;
	}

	if(pUiState == NULL)
	{
		HxLOG_Error("pUiState NULL\n");
		return ERR_FAIL;
	}

	if(pScState == NULL)
	{
		HxLOG_Error("pScState NULL\n");
		return ERR_FAIL;
	}

	HxLOG_Info("%s : Security(%d) CamLockCtrlLevel(%s) Camlock(%s) ScState(%s) PrevUiState(%s)\n",
		VA_UI_Debug_GetEventName(pEvent->eEventType),
		pScState->nSecurity,
		VA_UI_Debug_GetCamlockLevel(pScState->nCamLockCtrlLevel), VA_UI_Debug_GetCamlockState(pScState->nCamlock),
		VA_UI_Debug_GetScState(pScState->nState), xmgr_cas_VaGetUiStateName(*pUiState));

//##없어도 될듯..
/*
	if(local_cas_va_adapt_get_locid_process(pEvent, pScState) == ERR_FAIL)
	{
		return ERR_OK;
	}
*/

	*pUiState = eCAS_VA_UI_Error;

	// security restriction이 걸리면 다른 거 처리 안함.
	if (pScState->nSecurity)
	{
		eEventType = VA_UI_START_SECURITY_RESTRICTION;
	}
	else
	{
		eEventType = pEvent->eEventType;
	}

	switch (eEventType)
	{
		// UC01 simply notify (SC0103: without message)
		case VA_UI_DESCRAMBLING_OK:
		case VA_UI_FREE_TO_AIR:
		case VA_UI_STOP_SECURITY_RESTRICTION:
		case VA_UI_CAMLOCK_NOT_ACTIVATED:
		case VA_UI_CAMLOCK_DISABLED:
		case VA_UI_DESCRAMBLING_KO:
		case VA_UI_SMARTCARD_READY:
		case VA_UI_SMARTCARD_INSERTED:
			*pUiState = eCAS_VA_UI_None;
			break;

		case VA_UI_CAMLOCK_ACTIVATED_OK:
			*pUiState = eCAS_VA_UI_CamlockOk;
			break;

		// UC01 simply notify (SC0101 or SC0102: without/with video)
		case VA_UI_DESCRAMBLING_OK_RIGHT_OK:
			xmgr_cas_VaAdaptGetEsErrorMessage(&nLocId, pEvent->eEventType, pEvent->eDscrErrEsType);
			break;

		case VA_UI_DESCRAMBLING_KO_NO_RIGHT:
			xmgr_cas_VaAdaptGetEsErrorMessage(&nLocId, pEvent->eEventType, pEvent->eDscrErrEsType);
			break;

		case VA_UI_DESCRAMBLING_KO_GEOGRAPHICAL_BLACKOUT:
			xmgr_cas_VaAdaptGetEsErrorMessage(&nLocId, pEvent->eEventType, pEvent->eDscrErrEsType);
			break;

		case VA_UI_DESCRAMBLING_KO_MISSING_KEY:
			xmgr_cas_VaAdaptGetEsErrorMessage(&nLocId, pEvent->eEventType, pEvent->eDscrErrEsType);
			break;

		case VA_UI_DESCRAMBLING_KO_FORBIDDEN_MODE:
			xmgr_cas_VaAdaptGetEsErrorMessage(&nLocId, pEvent->eEventType, pEvent->eDscrErrEsType);
			break;

#if 1 //To comile for Octo2
		// event for schip only
		case VA_UI_DESCRAMBLING_KO_CW_PROTECTION_MISMATCH:
// hjlee3, just for build nLocId = LOC_CAS_VA_ERR_DESC_CW_PROTECTION_MISMATCH_ID;
			break;

		// UC01 simply notify (SC0101: without video)
		case VA_UI_VIACCESS_NOT_APPLICABLE:
// hjlee3, just for build			nLocId = LOC_CAS_VA_ERR_VIACCESS_NOT_APPLICABLE_ID;
			break;

		case VA_UI_VIACCESS_SMARTCARD_NOT_FOUND:
			#if 0 // org
			nLocId = LOC_CAS_VA_MSG_SC_VIACCESS_SMARTCARD_NOT_FOUND_ID;
			#else
			// VA_UI_VIACCESS_SMARTCARD_NOT_FOUND_EVENT 이후에 VA_UI_SMARTCARD_READY_EVENT 가 발생하는 경우,
			// pScState->nState 값은 여기 처리 직전에 읽어서 갱신이 되나 VA_UI_SMARTCARD_READY_EVENT 는 이 다음에 올라오므로
			// 불필요하게 "Please insert the smartcard to access the program." 메시지가 잠깐 보이게 된다.
			// 아래 코드는 이를 수정한 것임.
			if (pScState->nState == VA_UI_SMARTCARD_INSERTED || pScState->nState == VA_UI_SMARTCARD_READY)
			{
				*pUiState = eCAS_VA_UI_None;
			}
			else
			{
// hjlee3, just for build 				nLocId = LOC_CAS_VA_MSG_SC_VIACCESS_SMARTCARD_NOT_FOUND_ID;
			}
			#endif
			break;

		case VA_UI_SMARTCARD_EXTRACTED:
// hjlee3, just for build 			nLocId = LOC_CAS_VA_MSG_SC_EXTRACTED_ID;
			break;

		case VA_UI_SMARTCARD_NEEDED:
// hjlee3, just for build 			nLocId = LOC_CAS_VA_MSG_SC_NEEDED_ID;
			break;

		case VA_UI_SMARTCARD_FAILED:
// hjlee3, just for build 			nLocId = LOC_CAS_VA_MSG_SC_FAILED_ID;
			break;

		case VA_UI_SMARTCARD_NOT_ACCEPTED:
// hjlee3, just for build 			nLocId = LOC_CAS_VA_MSG_SC_NOT_ACCEPTED_ID;
			break;

		case VA_UI_SMARTCARD_INSUFFICIENT_MEMORY:
// hjlee3, just for build 			nLocId = LOC_CAS_VA_MSG_SC_INSUFFICIENT_MEMORY_ID;
			break;

		case VA_UI_SMARTCARD_INVALIDATED:
// hjlee3, just for build 			nLocId = LOC_CAS_VA_MSG_SC_INVALIDATED_ID;
			break;

		case VA_UI_CAMLOCK_ACTIVATED_NO_CARD:
// hjlee3, just for build 			nLocId = LOC_CAS_VA_MSG_SC_EXTRACTED_ID;
			break;

		// event for schip only
		case VA_UI_PAIRING_MISMATCH:
			break;

		// UC02 operator message
		case VA_UI_NEW_OPERATOR_MESSAGE:
			*pUiState = eCAS_VA_UI_NewOpMessage;
			break;

		// UC03 camlock
		case VA_UI_CAMLOCK_ACTIVATED_KO:
//## camlock ko인경우, smartcard 상태를 확인하여 메시지를 선택한다.
			switch(pScState->nState)
			{
			case VA_UI_SMARTCARD_EXTRACTED:
// hjlee3, just for build 				nLocId = LOC_CAS_VA_MSG_SC_EXTRACTED_ID;
				break;

			case VA_UI_SMARTCARD_FAILED:
// hjlee3, just for build 				nLocId = LOC_CAS_VA_MSG_SC_FAILED_ID;
				break;

			case VA_UI_SMARTCARD_NOT_ACCEPTED:
// hjlee3, just for build 				nLocId = LOC_CAS_VA_MSG_SC_NOT_ACCEPTED_ID;
				break;

			case VA_UI_SMARTCARD_INVALIDATED:
// hjlee3, just for build 				nLocId = LOC_CAS_VA_MSG_SC_INVALIDATED_ID;
				break;

			case VA_UI_SMARTCARD_INSERTED:
			case VA_UI_SMARTCARD_READY:
			default:
// hjlee3, just for build 				nLocId = LOC_CAS_VA_ERR_CAMLOCK_ID;
				break;
			}
			break;

		// UC04 security restriction
		case VA_UI_START_SECURITY_RESTRICTION:
// hjlee3, just for build 			nLocId = LOC_CAS_VA_ERR_SECURITY_RESTRICTION_ID;
			break;

		// UC05 maturity rating agreement
		case VA_UI_DESCRAMBLING_KO_MATURITY_RATING:
// hjlee3, just for build 			nLocId = LOC_CAS_VA_MSG_MR_INIT1_ID;
			break;

		// UC06 preview
		case VA_UI_DESCRAMBLING_OK_PREVIEW:
// hjlee3, just for build 			nLocId = LOC_CAS_VA_MSG_PREVIEW_ID;
			break;

		case VA_UI_NEW_OPERATOR_DATA:
			break;

#ifdef CONFIG_MW_CAS_VIACCESS_IPPV
		// event for ippv only
		case VA_UI_DESCRAMBLING_KO_IPPV_OFFER:							// UC07
		case VA_UI_DESCRAMBLING_KO_INSUFFICIENT_CREDIT:				// UC01- SC0101
		case VA_UI_DESCRAMBLING_OK_PREVIEW_AND_IPPV_OFFER:			// UC07
		case VA_UI_DESCRAMBLING_KO_IPPV_OFFER_EXPIRED:					// UC01- SC0101
		case VA_UI_DESCRAMBLING_KO_MATURITY_RATING_AND_IPPV_OFFER:	// UC07
		case VA_UI_OVERDRAFT_REACHED:									// UC01- SC0101
		case VA_UI_THRESHOLD_REACHED:									// UC01- SC0101
			HxLOG_Info("event (%d) is for IPPV only: not implemented\n", pEvent->eEventType);
			break;
#endif

		// event for pvr only
		case VA_UI_DESCRAMBLING_KO_RIGHT_EXPIRED:						// UC01- SC0101
#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
// hjlee3, just for build 			nLocId = LOC_CAS_VA_ERR_RIGHT_EXPIRED_VIDEO_AUDIO_ID;
#else
			HxLOG_Info("[VA_UI_Task] event (%d) is for PVR only: not implemented\n", pEvent->eEventType);
#endif
			break;

		case VA_UI_DESCRAMBLING_KO_RIGHT_CONSUMED:					// UC01- SC0101
#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
// hjlee3, just for build 			nLocId = LOC_CAS_VA_ERR_RIGHT_CONSUMED_VIDEO_AUDIO_ID;
#else
			HxLOG_Info("[VA_UI_Task] event (%d) is for PVR only: not implemented\n", pEvent->eEventType);
#endif
			break;

		case VA_UI_RECORD_RIGHT_KO:
		case VA_UI_TIMESHIFT_RIGHT_KO:
			HxLOG_Info("event (%d) is for PVR only: not implemented\n", pEvent->eEventType);
			break;

#ifdef CONFIG_MW_CAS_VIACCESS_PURPLE_BOX
		// event for purple box or IP box only (must not be implemented)
		case VA_UI_VIACCESS_TERMINAL_NOT_ACTIVATED:
		case VA_UI_ORM_CONNECTION_OK:
		case VA_UI_ORM_CONNECTION_KO:
		case VA_UI_CW_SERVER_CONNECTION_OK:
		case VA_UI_CW_SERVER_CONNECTION_KO:
		case VA_UI_TERMINAL_NOT_ACTIVATED:								// UC19
		case VA_UI_TERMINAL_READY:
		case VA_UI_TERMINAL_INSUFFICIENT_MEMORY:
		case VA_UI_TERMINAL_CONFIGURATION_ERROR:
		case VA_UI_TERMINAL_INVALIDATED:
		case VA_UI_TERMINAL_PERSONALIZATION_OK:
		case VA_UI_TERMINAL_PERSONALIZATION_KO:
			HxLOG_Info("event (%d) is for Purple box only: not implemented\n", pEvent->eEventType);
			break;
#endif
#endif
		default:
			HxLOG_Error("event (%x) is unknown\n", pEvent->eEventType);
			return ERR_FAIL;
	}

	s_nLocId = nLocId;

	return ERR_OK;
}

#endif	// #if defined(CONFIG_OP_NORDIG_BOXER)


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

#define _____AP_CAS_VA_ADAPT_PUBLIC_FUNCTION______________________________________________________

HERROR xmgr_cas_VaAdaptMessageProcess(void *pData, void *pScState, void *pUiState)
{
	return xmgr_cas_VaAdaptMessageFunction(pData, pScState ,pUiState);
}

#if defined(CONFIG_OP_NORDIG_BOXER)
HUINT32 xmgr_cas_VaAdaptGetBoxerModel_ID(void)
{
#if defined(CONFIG_PROD_BXRHD)
	return BOXER_MODELID_BXR_HD;	// fixed by boxer
#elif defined(CONFIG_PROD_BXRHDPLUS)
	return BOXER_MODELID_BXR_HD_PLUS;	// fixed by boxer
#elif defined(CONFIG_PROD_BXRHD2)
	return BOXER_MODELID_BXR_HD2;	// fixed by boxer
#elif defined(CONFIG_PROD_BXRHDPLUS2)
	return BOXER_MODELID_BXR_HD_PLUS2;	// need to be fixed by boxer
#else
	// 반드시 boxer에서 할당된 model id가 존재해야 한다
	#error
#endif

}

// 정리 필요..
HERROR xmgr_cas_VaAdaptGetBoxerScLockMsg(HUINT8 *pszLockMsg_512)
{
	HUINT32 ulModelId = xmgr_cas_VaAdaptGetBoxerModel_ID();

	if (pszLockMsg_512 == NULL)
	{
		HxLOG_Error("pszLockMsg_256 NULL\n");
		return ERR_FAIL;
	}

	MWC_UTIL_DvbUtf8Sprintf(pszLockMsg_512, "%s\nid : %03d %03d", GetStrRsc(LOC_CAS_VA_BOXER_ERR_MAX_CHANGE_PIN_ID), ulModelId, eBoxer_ErrCode_20_LOCKED_PINCODE_STATUS);

	return ERR_OK;
}

// 정리 필요..
HERROR xmgr_cas_VaAdaptShowBoxerScLockMsg(void)
{
	HUINT8	*pszStr1 = NULL;

#if 0 //To comile for Octo2
	pszStr1 = GetTextBuffer(TEXT_SIZE512);
	VK_memset32(pszStr1, 0, TEXT_SIZE512);

	if (xmgr_cas_VaAdaptGetBoxerScLockMsg(pszStr1) != ERR_OK)
	{
		HxLOG_Error("xmgr_cas_VaAdaptGetBoxerScLockMsg() Fail \n");
		return ERR_FAIL;
	}

	xmgr_cas_VaShowMessageBox(pszStr1);
#endif
	return ERR_OK;
}
#endif

HERROR xmgr_cas_VaAdaptGetErrorMessage(HUINT8 *pszMsg)
{
#if 0 //To comile for Octo2 // hjlee3, just to build
	if(s_nLocId != -1)
	{
#if defined(CONFIG_OP_NORDIG_BOXER)
		if (s_ulBoxerErrCode != 0)
		{
			MWC_UTIL_DvbUtf8Sprintf (pszMsg, (HUINT8*)"%s\n\nid:%03d %03d", GetStrRsc(s_nLocId), xmgr_cas_VaAdaptGetBoxerModel_ID(), s_ulBoxerErrCode);
		}
		else
		{
			MWC_UTIL_DvbStrcpy(pszMsg, GetStrRsc(s_nLocId));
		}
#else
		MWC_UTIL_DvbStrcpy(pszMsg, GetStrRsc(s_nLocId));
#endif
	}
#endif
	return ERR_OK;
}


