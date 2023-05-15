/*******************************************************************
	File Description
*******************************************************************/
/*++
    @file        cx_ui.c
    @brief       UI

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2011 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
--*/
/*******************************************************************/
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
/*******************************************************************/
#define ________CX_UI_Private_Include_________________________________________
/*******************************************************************/
#include <octo_common.h>
#include <isosvc.h>
#include <_svc_cas_sub_api.h>
#include <conax.h>
#include <cxmgr_action.h>


/*******************************************************************/
#define ________CX_UI_Golbal_Value____________________________________________
/*******************************************************************/
/*******************************************************************/
#define ________CX_UI_Private_Define__________________________________________
/*******************************************************************/
#define MAX_CX_EVENT_BUFFER		15
/*******************************************************************/
#define ________CX_UI_Private_Static_Value____________________________________
/*******************************************************************/
static CxUiMsgInfo_t	s_stCxUiMsgInfo[MAX_CX_EVENT_BUFFER];
static HUINT8			s_ucCxUiMsgBufIdx;

#if defined(CONFIG_MW_CAS_CONAX_TEXT)
static CxUiMsgInfo_t	s_stCxUiMsgUTInfo;		// User Text Info
#endif

#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
static CxUiMsgInfo_t	s_stCxUiMsgFPInfo[MAX_CX_EVENT_BUFFER];		// Finger print info
static HUINT8			s_ucCxUiMsgFPIdx;
#endif
/*******************************************************************/
#define ________CX_UI_Global_Value____________________________________________
/*******************************************************************/
/*******************************************************************/
#define ________CX_UI_Private_Static_Prototype________________________________
/*******************************************************************/
STATIC void cxDalUi_Init(void);
STATIC HERROR cxDalUi_UpdateDescrambleStatus(Handle_t hStbHandle, HBOOL bDescrambled);
STATIC HERROR cxDalUi_Display(Handle_t hStbHandle, CxUiMsgType_t eMsg, HUINT32 ulParam);
STATIC HERROR cxDalUi_MakeDisplayMsg(CxUiMsgInfo_t* pstMsgInfo, CxUiMsgType_t eMsg, HUINT32 ulParam);

/*******************************************************************/
#define ________CX_UI_Private_Static_Prototype_Body___________________________
/*******************************************************************/
STATIC void cxDalUi_Init(void)
{
	ENTER_FUNCTION;

	s_ucCxUiMsgBufIdx = 0;

	HxSTD_memset(&s_stCxUiMsgInfo, 0x00, sizeof(CxUiMsgInfo_t) * MAX_CX_EVENT_BUFFER);
#if defined(CONFIG_MW_CAS_CONAX_TEXT)
	HxSTD_memset(&s_stCxUiMsgUTInfo, 0x00, sizeof(CxUiMsgInfo_t));
#endif
#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
	s_ucCxUiMsgFPIdx = 0;
	HxSTD_memset(&s_stCxUiMsgFPInfo, 0x00, sizeof(CxUiMsgInfo_t) * MAX_CX_EVENT_BUFFER);
#endif

	LEAVE_FUNCTION;
}

STATIC HERROR cxDalUi_UpdateDescrambleStatus(Handle_t hStbHandle, HBOOL bDescrambled)
{
	SvcCas_MsgType_e	eMsgType;
	SvcCas_ShowType_e	eShowType;

	ENTER_FUNCTION;

	if (bDescrambled)
	{
		eMsgType = eCAS_MSG_TYPE_Ok;
		eShowType = eCasShowType_Both;
	}
	else
	{
		eMsgType = eCAS_MSG_TYPE_Fail;
		eShowType = eCasShowType_None;
	}

	if (bDescrambled)
	{
		// 왜 이게 호출되면 화면에서 cas message가 사라질까??
		// if (bDescrambled) 조건 추가하면서 사라지는 문제 해결함. 20110420
		// prevent #232332
		if (svc_cas_MgrSubUpdateCasMessage(CXACT_GetActionHandle(hStbHandle), eMsgType, eDxCAS_GROUPID_CX, (void*)NULL, eShowType) != ERR_OK)
		{
			LEAVE_FUNCTION;
			return ERR_FAIL;
		}
	}

	LEAVE_FUNCTION;
	return ERR_OK;
}

STATIC HERROR cxDalUi_Display(Handle_t hStbHandle, CxUiMsgType_t eMsg, HUINT32 ulParam)
{
	HERROR				errResult		= ERR_FAIL;
	SvcCas_MsgType_e	eMsgType		= eCAS_MSG_TYPE_Ok;
	SvcCas_ShowType_e	eShowType		= eCasShowType_Both;
	CxUiMsgInfo_t* 		pstMsgInfo		= NULL;
#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
	CxUiMsgInfo_t* 		pstFPInfo		= NULL;
#endif
	HBOOL				bShowAV			= FALSE;


	ENTER_FUNCTION;
	/*
		HUINT32 msg[5];
		if (sCxAxiQid)
		{
			msg[0] = (HUINT32)hStbHandle;
			msg[1] = (HUINT32)eLevel;
			msg[2] = (HUINT32)eMsg;
			msg[3] = (HUINT32)ulParam;
			msg[4] = (HUINT32)ulUiSeqNum;
			VK_MSG_Send(sCxAxiQid, &msg, 5*4);
		}
	*/
	//HxSTD_memset(&s_stCxUiMsgInfo, 0x00, sizeof(CxUiMsgInfo_t));
	if (s_ucCxUiMsgBufIdx >= MAX_CX_EVENT_BUFFER)
	{
		s_ucCxUiMsgBufIdx = 0;
	}

	pstMsgInfo = &s_stCxUiMsgInfo[s_ucCxUiMsgBufIdx++];

	switch (eMsg)
	{
		// eCxUi_GroupBasic - Descramble MMI - ACCESS_STATUS
	case eCxUi_GroupBasic_NoAccess:
	case eCxUi_GroupBasic_NoAccessBlackOut:
	case eCxUi_GroupBasic_NoAccessNetwork:
#if defined(CONFIG_MW_CAS_CONAX_CPM)
		pstMsgInfo->cpm_nErrCode = ulParam;
#endif
		pstMsgInfo->eUiMsgType = eMsg;
		eMsgType = eCAS_MSG_TYPE_Fail;
		eShowType = eCasShowType_None;
		break;

		// eCxUi_GroupMr - Descramble MMI - ACCESS_STATUS (Maturity Rating)
	case eCxUi_GroupMr_Ask:
		cxDalUi_MakeDisplayMsg(pstMsgInfo, eMsg, ulParam);
		eMsgType = eCAS_MSG_TYPE_Check;
		eShowType = eCasShowType_None;
		break;

	case eCxUi_GroupMr_WrongPin:
	case eCxUi_GroupMr_PinLocked:
		// 이것도 보내야 하나? 음.. 확인해 보자.
		// TODO: TBC
		return	ERR_OK;

		// eCxUi_GroupPpv - Descramble MMI - ACCESS_STATUS (Order Info)
	case eCxUi_GroupPpv_OrderInfo:
		cxDalUi_MakeDisplayMsg(pstMsgInfo, eMsg, ulParam);
		eMsgType = eCAS_MSG_TYPE_Check;
		eShowType = eCasShowType_None;
		break;
	case eCxUi_GroupPpv_OrderConnect:
	case eCxUi_GroupPpv_OrderFailed:
	case eCxUi_GroupPpv_OrderAccepted:
	case eCxUi_GroupPpv_OrderNotAccepted:
	case eCxUi_GroupPpv_OrderClosed:
		// 이것도 보내야 하나? 음.. 확인해 보자.
		// TODO: TBC
		return	ERR_OK;

		// eCxUi_GroupPpv - Descramble MMI - ACCESS_STATUS (Accept Viewing Info)
	case eCxUi_GroupPpv_AcceptViewing:
		cxDalUi_MakeDisplayMsg(pstMsgInfo, eMsg, ulParam);
		// PPV preview를 처리하기 위하여 eShowType을 eCasShowType_None에서 eCasShowType_Both로 변경. PT 이슈.
		eMsgType = eCAS_MSG_TYPE_Check;
		eShowType = eCasShowType_Both;
		break;

		// eCxUi_GroupPpv - Descramble MMI - ACCESS_STATUS (Tokens Debit Info)
	case eCxUi_GroupPpv_TokenPpv:
		cxDalUi_MakeDisplayMsg(pstMsgInfo, eMsg, ulParam);
		eMsgType = eCAS_MSG_TYPE_Check;
		eShowType = eCasShowType_None;
		break;

#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
		// eCxUi_GroupOther - Finger Print
	case eCxUi_GroupOther_Fingerprint:
		if (s_ucCxUiMsgFPIdx >= MAX_CX_EVENT_BUFFER)
		{
			s_ucCxUiMsgFPIdx = 0;
		}
		pstFPInfo = &s_stCxUiMsgFPInfo[s_ucCxUiMsgFPIdx++];
		cxDalUi_MakeDisplayMsg(pstFPInfo, eMsg, ulParam);
		eMsgType = eCAS_MSG_TYPE_Command;
		eShowType = eCasShowType_Both;
		break;
#endif

#if defined(CONFIG_MW_CAS_CONAX_TEXT)
		// eCxUi_GroupMessage - User text
	case eCxUi_GroupMessage_Display:
		cxDalUi_MakeDisplayMsg(pstMsgInfo, eMsg, ulParam);
		eMsgType = eCAS_MSG_TYPE_Command;
		eShowType = eCasShowType_Both;
		break;
#endif

		// eCxUi_GroupSc - Smart Card Driver MMI
	case eCxUi_GroupSc_NoCard:
	case eCxUi_GroupSc_Inserted:
	case eCxUi_GroupSc_Incorrect:
	case eCxUi_GroupSc_IncorrectConax:
	case eCxUi_GroupSc_CommFail:
		//s_stCxUiMsgInfo.eUiMsgType = eMsg;
		pstMsgInfo->eUiMsgType = eMsg;
		eMsgType = eCAS_MSG_TYPE_Fail;
		eShowType = eCasShowType_None;
		bShowAV = (HBOOL)ulParam;
		if (eMsg == eCxUi_GroupSc_Incorrect && bShowAV == TRUE)
		{
			eShowType = eCasShowType_Both;
		}
		break;

	case eCxUi_GroupAll_None:
		//s_stCxUiMsgInfo.eUiMsgType = eMsg;
		pstMsgInfo->eUiMsgType = eMsg;
		eMsgType = eCAS_MSG_TYPE_Ok;
		eShowType = eCasShowType_Both;
		break;

	default:
		HxLOG_Critical("\n\n");
		eMsgType = eCAS_MSG_TYPE_None;
		return	ERR_FAIL;
	}

	HxLOG_Print("========== Disp MSG ============== \n");
	switch (eMsg)
	{
#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
	case eCxUi_GroupOther_Fingerprint:
		//HxLOG_Print(" eUiMsgType :  %s(0x%x) \n", CXUI_DEBUG_GetMsgTypeStr(pstFPInfo->eUiMsgType), pstFPInfo->eUiMsgType);
		errResult = svc_cas_MgrSubUpdateCasMessage(CXACT_GetActionHandle(hStbHandle), eMsgType, eDxCAS_GROUPID_CX, (void*)pstFPInfo, eShowType);
		break;
#endif
#if defined(CONFIG_MW_CAS_CONAX_TEXT)
	case eCxUi_GroupMessage_Display:
		//HxLOG_Print(" eUiMsgType :  %s(0x%x) \n", CXUI_DEBUG_GetMsgTypeStr(s_stCxUiMsgUTInfo.eUiMsgType), s_stCxUiMsgUTInfo.eUiMsgType);
		errResult = svc_cas_MgrSubUpdateCasMessage(CXACT_GetActionHandle(hStbHandle), eMsgType, eDxCAS_GROUPID_CX, (void*)&s_stCxUiMsgUTInfo, eShowType);
		break;
#endif
	default:
#if 0
			HxLOG_Error(" eUiMsgType :  %s(0x%x) \n", CXUI_DEBUG_GetMsgTypeStr(s_stCxUiMsgInfo.eUiMsgType), s_stCxUiMsgInfo.eUiMsgType );
			errResult = svc_cas_MgrSubUpdateCasMessage(CXACT_GetActionHandle(hStbHandle), eMsgType, eDxCAS_GROUPID_CX, (void*)&s_stCxUiMsgInfo, eShowType);
#else
			//HxLOG_Print(" eUiMsgType :  %s(0x%x) \n", CXUI_DEBUG_GetMsgTypeStr(pstMsgInfo->eUiMsgType), pstMsgInfo->eUiMsgType );
			errResult = svc_cas_MgrSubUpdateCasMessage(CXACT_GetActionHandle(hStbHandle), eMsgType, eDxCAS_GROUPID_CX, (void*)pstMsgInfo, eShowType);
#endif
		break;
	}
	HxLOG_Print(" eMsgType :  %d\n", eMsgType);
	HxLOG_Print(" eShowType : %d \n", eShowType);
	HxLOG_Print("================================\n");

	if (errResult != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		LEAVE_FUNCTION;
		return	ERR_FAIL;
	}

	LEAVE_FUNCTION;
	return	ERR_OK;
}


STATIC HERROR cxDalUi_MakeDisplayMsg(CxUiMsgInfo_t* pstMsgInfo, CxUiMsgType_t eMsg, HUINT32 ulParam)
{
	switch (eMsg)
	{
	case eCxUi_GroupMr_Ask:
		{
			CxMaturityRatingValue_t* 										stCxUiMrValue;

			stCxUiMrValue = (CxMaturityRatingValue_t *)ulParam;

			pstMsgInfo->eUiMsgType = eMsg;
			pstMsgInfo->mr_ucProgram = stCxUiMrValue->ucProgram;
			pstMsgInfo->mr_ucSmartcard = stCxUiMrValue->ucSmartcard;
		}
		break;
	case eCxUi_GroupPpv_OrderInfo:
		{
			CxOrderInfo_t* 	stCxUiOrder;

			stCxUiOrder = (CxOrderInfo_t *)ulParam;
			pstMsgInfo->eUiMsgType = eMsg;
			pstMsgInfo->or_ulProdRef = stCxUiOrder->ulProdRef;
			pstMsgInfo->or_usPriceUnit = stCxUiOrder->stPrice.usPriceUnit;
			pstMsgInfo->or_ucPriceDecimal = stCxUiOrder->stPrice.ucPriceDecimal;
			MWC_UTIL_DvbStrcpy(pstMsgInfo->or_szLabel, stCxUiOrder->szLabel);
			MWC_UTIL_DvbStrcpy(pstMsgInfo->or_aucCurrency, stCxUiOrder->stPrice.aucCurrency);
		}
		break;

	case eCxUi_GroupPpv_AcceptViewing:
		{
			CxAcceptViewingInfo_t* 	stCxUiAcceptView;

			stCxUiAcceptView = (CxAcceptViewingInfo_t *)ulParam;
			pstMsgInfo->eUiMsgType = eMsg;
			pstMsgInfo->av_ulProdRef = stCxUiAcceptView->ulProdRef;
			pstMsgInfo->av_usMinuteLeft = stCxUiAcceptView->usMinuteLeft;
			MWC_UTIL_DvbStrcpy(pstMsgInfo->av_szLabel, stCxUiAcceptView->szLabel);
		}
		break;

	case eCxUi_GroupPpv_TokenPpv:
		{
			CxTokenDebitInfo_t* stCxUiTokesDebit;

			stCxUiTokesDebit = (CxTokenDebitInfo_t *)ulParam;

			pstMsgInfo->eUiMsgType = eMsg;
			pstMsgInfo->td_usPurseRef = stCxUiTokesDebit->usPurseRef;
			pstMsgInfo->td_ulEventTag = stCxUiTokesDebit->ulEventTag;		// 24 bits
			pstMsgInfo->td_ucInsufficientTokens = stCxUiTokesDebit->ucInsufficientTokens;
			pstMsgInfo->td_ucTokenMode = stCxUiTokesDebit->ucTokenMode;
			pstMsgInfo->td_ulEventCost = stCxUiTokesDebit->ulEventCost;	// 24 bits
			MWC_UTIL_DvbStrcpy(pstMsgInfo->td_szLabel, stCxUiTokesDebit->szLabel);
		}
		break;

#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
	case eCxUi_GroupOther_Fingerprint:
		{
			CxFingerprint_t* 								pstCxUiFingerPrint;

			pstCxUiFingerPrint = (CxFingerprint_t *)ulParam;

			if (pstCxUiFingerPrint != NULL)
			{
				pstMsgInfo->eUiMsgType = eMsg;
				pstMsgInfo->fp_ulTime = pstCxUiFingerPrint->ulTime;
				HxSTD_memcpy(pstMsgInfo->fp_ucCentTime, pstCxUiFingerPrint->ucCentTime, sizeof(pstCxUiFingerPrint->ucCentTime));
				pstMsgInfo->fp_usDuration = pstCxUiFingerPrint->usDuration;
				pstMsgInfo->fp_usPosX = pstCxUiFingerPrint->usPosX;
				pstMsgInfo->fp_usPosY = pstCxUiFingerPrint->usPosY;
				pstMsgInfo->fp_usHeight = pstCxUiFingerPrint->usHeight;
				MWC_UTIL_DvbStrcpy(pstMsgInfo->fp_szText, pstCxUiFingerPrint->szText);
				pstMsgInfo->fp_ucDisplayed = pstCxUiFingerPrint->ucDisplayed;
				pstMsgInfo->fp_bFPFromProtectedData = pstCxUiFingerPrint->bFPFromProtectedData;
				// prevent #232357
				OxCAS_Free(pstCxUiFingerPrint);
			}
		}
		break;
#endif

#if defined(CONFIG_MW_CAS_CONAX_TEXT)
	case eCxUi_GroupMessage_Display:
		{
			CxUserText_t* 	pstCxUiUserText;

			pstCxUiUserText = (CxUserText_t *)ulParam;

			if (pstCxUiUserText != NULL)
			{
				s_stCxUiMsgUTInfo.eUiMsgType = eMsg;
				s_stCxUiMsgUTInfo.ut_ucDisplayed = pstCxUiUserText->ucDisplayed;
				s_stCxUiMsgUTInfo.ut_usDuration = pstCxUiUserText->usDuration;		// 이 값이 0이면 메시지를 표시하지는 않고 왔다고만 알려준다 (mailbox message). 단위 second.
				s_stCxUiMsgUTInfo.ut_ucSeqNo = pstCxUiUserText->ucSeqNo;
				s_stCxUiMsgUTInfo.ut_ucRepeat = pstCxUiUserText->ucRepeat;			// 없으면 1회만 출력
				s_stCxUiMsgUTInfo.ut_usInterval = pstCxUiUserText->usInterval;		// 메시지를 표시하는 간격 (10초단위로 들어오나 *10하여 second단위로 저장하고 있음)
				//s_stCxUiMsgUTInfo.ut_stUnixTime = pstCxUiUserText->stUnixTime;
				s_stCxUiMsgUTInfo.ut_stStartDataTime.stDate.usYear = pstCxUiUserText->stStartDataTime.stDate.usYear;
				s_stCxUiMsgUTInfo.ut_stStartDataTime.stDate.ucMonth = pstCxUiUserText->stStartDataTime.stDate.ucMonth;
				s_stCxUiMsgUTInfo.ut_stStartDataTime.stDate.ucDay = pstCxUiUserText->stStartDataTime.stDate.ucDay;
				s_stCxUiMsgUTInfo.ut_stStartDataTime.stTime.ucHour = pstCxUiUserText->stStartDataTime.stTime.ucHour;
				s_stCxUiMsgUTInfo.ut_stStartDataTime.stTime.ucMinute = pstCxUiUserText->stStartDataTime.stTime.ucMinute;
				s_stCxUiMsgUTInfo.ut_stStartDataTime.stTime.ucSecond = pstCxUiUserText->stStartDataTime.stTime.ucSecond;
				s_stCxUiMsgUTInfo.ut_stStartDataTime.stTime.usMillisecond = pstCxUiUserText->stStartDataTime.stTime.usMillisecond;
				HxSTD_memcpy(s_stCxUiMsgUTInfo.ut_szText, pstCxUiUserText->szText, CX_MAX_USER_TEXT_LEN);
				s_stCxUiMsgUTInfo.ut_ulImmediateStartTime = pstCxUiUserText->ulImmediateStartTime;

				// prevent #232351
				OxCAS_Free(pstCxUiUserText);
			}
		}
		break;
#endif

	default:
		HxLOG_Critical("\n\n");
		break;
	}

	return	ERR_OK;
}
/*******************************************************************/
#define ________CX_UI_Public_Functions_Body___________________________________
/*******************************************************************/
void CXUI_Init(void)
{
	cxDalUi_Init();
}

HERROR CXUI_UpdateDescrambleStatus(Handle_t hStbHandle, HBOOL bDescrambled)
{
	return cxDalUi_UpdateDescrambleStatus(hStbHandle, bDescrambled);
}

HERROR CXUI_Display(Handle_t hStbHandle, CxUiMsgType_t eMsg, HUINT32 ulParam)
{
	return cxDalUi_Display(hStbHandle, eMsg, ulParam);
}

HERROR CXUI_MakeDisplayMsg(CxUiMsgInfo_t* pstMsgInfo, CxUiMsgType_t eMsg, HUINT32 ulParam)
{
	return cxDalUi_MakeDisplayMsg(pstMsgInfo, eMsg, ulParam);
}

/*******************************************************************/
#define ________CX_UI_DEBUG_Functions_Body___________________________________
/*******************************************************************/
#if defined(CONFIG_DEBUG)
HERROR CXUI_Display_CMD_Test(Handle_t hAction, CxUiMsgType_t eMsg, HUINT32 ulParam)
{
	HERROR				errResult	= ERR_FAIL;
	SvcCas_MsgType_e	eMsgType	= eCAS_MSG_TYPE_Ok;
	SvcCas_ShowType_e	eShowType	= eCasShowType_Both;

	HxSTD_memset(&s_stCxUiMsgInfo, 0x00, sizeof(CxUiMsgInfo_t));

	switch (eMsg)
	{
		// eCxUi_GroupBasic - Descramble MMI - ACCESS_STATUS
		case eCxUi_GroupBasic_NoAccess:
#if defined(CONFIG_MW_CAS_CONAX_CPM)
		s_stCxUiMsgInfo->cpm_nErrCode = ulParam;
#endif
	case eCxUi_GroupBasic_NoAccessBlackOut:
	case eCxUi_GroupBasic_NoAccessNetwork:
		s_stCxUiMsgInfo->eUiMsgType = eMsg;
		eMsgType = eCAS_MSG_TYPE_Fail;
		eShowType = eCasShowType_None;
		break;

		// eCxUi_GroupMr - Descramble MMI - ACCESS_STATUS (Maturity Rating)
	case eCxUi_GroupMr_Ask:
		cxDalUi_MakeDisplayMsg((CxUiMsgInfo_t*)&s_stCxUiMsgInfo[0], eMsg, ulParam);
		eMsgType = eCAS_MSG_TYPE_Check;
		eShowType = eCasShowType_None;
		break;

		// eCxUi_GroupPpv - Descramble MMI - ACCESS_STATUS (Order Info)
	case eCxUi_GroupPpv_OrderInfo:
		cxDalUi_MakeDisplayMsg((CxUiMsgInfo_t*)&s_stCxUiMsgInfo[0], eMsg, ulParam);
		eMsgType = eCAS_MSG_TYPE_Check;
		eShowType = eCasShowType_None;
		break;

		// eCxUi_GroupPpv - Descramble MMI - ACCESS_STATUS (Accept Viewing Info)
	case eCxUi_GroupPpv_AcceptViewing:
		cxDalUi_MakeDisplayMsg((CxUiMsgInfo_t*)&s_stCxUiMsgInfo[0], eMsg, ulParam);
		eMsgType = eCAS_MSG_TYPE_Check;
		eShowType = eCasShowType_None;
		break;

		// eCxUi_GroupPpv - Descramble MMI - ACCESS_STATUS (Tokens Debit Info)
	case eCxUi_GroupPpv_TokenPpv:
		cxDalUi_MakeDisplayMsg((CxUiMsgInfo_t*)&s_stCxUiMsgInfo[0], eMsg, ulParam);
		eMsgType = eCAS_MSG_TYPE_Check;
		eShowType = eCasShowType_None;
		break;

#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
		// eCxUi_GroupOther - Finger Print
	case eCxUi_GroupOther_Fingerprint:
		cxDalUi_MakeDisplayMsg((CxUiMsgInfo_t*)&s_stCxUiMsgInfo[0], eMsg, ulParam);
		eMsgType = eCAS_MSG_TYPE_Command;
		eShowType = eCasShowType_Both;
		break;
#endif

#if defined(CONFIG_MW_CAS_CONAX_TEXT)
		// eCxUi_GroupMessage - user text
	case eCxUi_GroupMessage_Display:
		cxDalUi_MakeDisplayMsg((CxUiMsgInfo_t*)&s_stCxUiMsgInfo[0], eMsg, ulParam);
		eMsgType = eCAS_MSG_TYPE_Command;
		eShowType = eCasShowType_Both;
		break;
#endif

		// eCxUi_GroupSc - Smart Card Driver MMI
	case eCxUi_GroupSc_NoCard:
	case eCxUi_GroupSc_Inserted:
	case eCxUi_GroupSc_Incorrect:
	case eCxUi_GroupSc_IncorrectConax:
	case eCxUi_GroupSc_CommFail:
		s_stCxUiMsgInfo->eUiMsgType = eMsg;
		eMsgType = eCAS_MSG_TYPE_Fail;
		eShowType = eCasShowType_None;
		break;

	case eCxUi_GroupAll_None:
		s_stCxUiMsgInfo->eUiMsgType = eMsg;
		eMsgType = eCAS_MSG_TYPE_Ok;
		eShowType = eCasShowType_Both;
		break;
	default:
		eMsgType = eCAS_MSG_TYPE_None;
		break;
	}

	switch (eMsg)
	{
#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
	case eCxUi_GroupOther_Fingerprint:
		errResult = svc_cas_MgrSubUpdateCasMessage(hAction, eMsgType, eDxCAS_GROUPID_CX, (void*)&s_stCxUiMsgFPInfo, eShowType);
		break;
#endif
#if defined(CONFIG_MW_CAS_CONAX_TEXT)
	case eCxUi_GroupMessage_Display:
		errResult = svc_cas_MgrSubUpdateCasMessage(hAction, eMsgType, eDxCAS_GROUPID_CX, (void*)&s_stCxUiMsgUTInfo, eShowType);
		break;
#endif
	default:
		errResult = svc_cas_MgrSubUpdateCasMessage(hAction, eMsgType, eDxCAS_GROUPID_CX, (void*)&s_stCxUiMsgInfo, eShowType);
		break;
	}

	if (errResult != ERR_OK)
	{
		HxLOG_Critical("\n\n");
	}

	return ERR_OK;
}
#endif


