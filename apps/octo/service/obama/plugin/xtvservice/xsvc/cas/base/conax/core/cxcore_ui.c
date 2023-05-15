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
/**
	@file     cxcore_ui.c
	@brief    cxcore_ui.c (CONAX MAIN / UI)

	Description: ui 관련 \n
	Module: MW/CAS/ CONAX \n
	Remarks :\n

	Copyright (c) 2009 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <tapi.h>
#include <_svc_cas_mgr_svc_port.h>
#include "../local_include/cxcore_main.h"
#include "../local_include/cxcore_os.h"
#include "../local_include/cxcore_ui.h"
#include "../local_include/cxcore_sc.h"
#include "../local_include/cxcore_nvm.h"
#include "../local_include/cxcore_session.h"
#include "../local_include/cxcore_stream.h"
#include "../local_include/cxcore_ecm.h"
#include <cxmgr_action.h>
#if defined(CONFIG_MW_CAS_CONAX_CPM)
#include <pal_sys.h>
#endif

#include "../local_include/cx_ui.h"

/*******************************************************************/
/********************      Global Variables 	********************/
/*******************************************************************/
#if defined(CONFIG_DEBUG)
#if defined(WIN32)
#define	USE_CONAX_EMULATOR
#endif
#endif

/*******************************************************************/
/********************      Defines  	   *************************/
/*******************************************************************/
#if defined(CONFIG_MW_CAS_CONAX_TEXT)
#define	CX_USER_TEXT_CHECK_TIME		5000		// core init 5초 후에 확인..
#define	CX_USER_TEXT_RETRY_TIME		2000		// 2초간격 16번으로 했을 경우, Fail case 발생
#define	CX_USER_TEXT_RETRY_COUNT	48

#define	CX_USER_TEXT_RETRY_TRICK_COUNT	10
#define	CX_USER_TEXT_RETRY_TRICK_TIME	1000
#endif

#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
#define	CX_FINGERPRINT_CHECK_TIME		5000		// core init 5초 후에 확인..
#define	CX_FINGERPRINT_RETRY_TIME		2000
#define	CX_FINGERPRINT_RETRY_TRICK_COUNT	30
#define	CX_FINGERPRINT_RETRY_TRICK_TIME		1000
#endif

#define GET_SVCHANDLE_FROM_STBHANDLE(x)	(x & 0x00ffffff)

/*******************************************************************/
/********************      Functions	   *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef  	   *************************/
/*******************************************************************/
typedef enum
{
	eCxUi_None = 0x00,
	eCxUi_Pincode = 0x01,
	eCxUi_Menu = 0x02,
}eCxUi_UiDisaplyed;

/*******************************************************************/
/********************      Static Variables 	********************/
/*******************************************************************/

STATIC CxUiMsgType_t			s_astCxUiLastMsg[eCxUi_GroupMax];

STATIC HULONG					s_ulCxUiSemId;

#if defined(CONFIG_MW_CAS_CONAX_TEXT)
STATIC HULONG					s_ulCxUserTextTimerId				= 0;
STATIC HULONG					s_ulCxUserTextRetryTimerId			= 0;
STATIC CxUserText_t				s_stUserText;
STATIC CxUserText_t* 			s_pstUserTextList;
STATIC HINT32					s_ulCxUserTextRetryCount			= 0;
STATIC HINT32					s_ulCxUserTextRetryTrickCount		= 0;
STATIC HULONG					s_ulCxUserTextRetryTrickTimerId		= 0;
#endif

#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
STATIC HULONG					s_ulCxFingerprintTimerId			= 0;
STATIC HULONG					s_ulCxFingerprintRetryTimerId		= 0;
STATIC CxFingerprint_t			s_stFingerprint;
STATIC HINT32					s_ulCxFingerprintRetryTrickCount	= 0;
STATIC HULONG					s_ulCxFingerprintRetryTrickTimerId	= 0;
#endif

// UI message의 parameter를 보관하는 buffer
STATIC CxMaturityRatingValue_t	s_stCxUiMrValue;
STATIC CxOrderInfo_t			s_stCxUiOrder;
STATIC CxAcceptViewingInfo_t	s_stCxUiAcceptViewing;
STATIC CxTokenDebitInfo_t		s_stCxUiTokenDebit;
STATIC eCxUi_UiDisaplyed		s_eCxUiDisplayed;

/*******************************************************************/
/********************      Functions	   *************************/
/*******************************************************************/

static HERROR CXUTIL_GetTime(HUINT32* pulTime)
{
	if (VK_CLOCK_GetTime((unsigned long*)pulTime) == VK_OK)
	{
		return ERR_OK;
	}

	return ERR_FAIL;
}

#define INIT_________________________________________________

void local_cxui_TimerHandler(unsigned long ulTimerId, void* pArg)
{
#if defined(CONFIG_MW_CAS_CONAX_TEXT)

	HUINT32	param	= 0;

	if (ulTimerId == s_ulCxUserTextTimerId)
	{
		s_ulCxUserTextTimerId = 0;
		CXCORE_SendMsg(eCxMsg_Ui_CheckUserTextAtBoot, HANDLE_NULL, (HUINT32)0, (HUINT32)0);
	}
	else if (ulTimerId == s_ulCxUserTextRetryTimerId)
	{
		if (pArg == NULL)
		{
			return;
		}

		CX_memcpy(&param, pArg, sizeof(param));

		s_ulCxUserTextRetryTimerId = 0;
		CXCORE_SendMsg(eCxMsg_Ui_RetryUserText, HANDLE_NULL, (HUINT32)param, (HUINT32)0);
	}
	else if (ulTimerId == s_ulCxUserTextRetryTrickTimerId)
	{
		s_ulCxUserTextRetryTrickTimerId = 0;
		CXCORE_SendMsg(eCxMsg_Ui_CheckUserTextAtStandby, HANDLE_NULL, (HUINT32)0, (HUINT32)0);
	}

#endif

#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
	if (ulTimerId == s_ulCxFingerprintTimerId)
	{
		s_ulCxFingerprintTimerId = 0;
		CXCORE_SendMsg(eCxMsg_Ui_CheckFingerprintAtBoot, HANDLE_NULL, (HUINT32)0, (HUINT32)0);
	}
	else if (ulTimerId == s_ulCxFingerprintRetryTimerId)
	{
		s_ulCxFingerprintRetryTimerId = 0;
		CXCORE_SendMsg(eCxMsg_Ui_RetryFingerprint, HANDLE_NULL, (HUINT32)0, (HUINT32)0);
	}
	else if (ulTimerId == s_ulCxFingerprintRetryTrickTimerId)
	{
		s_ulCxFingerprintRetryTrickTimerId = 0;
		CXCORE_SendMsg(eCxMsg_Ui_CheckFingerprintAtStandby, HANDLE_NULL, (HUINT32)0, (HUINT32)0);
	}

#endif
}

HERROR CXUI_InitCore(void)
{
	int		i;
	HINT32	nResult			= 0;

	ENTER_FUNCTION;

	CXUI_Init();

#if defined(CONFIG_MW_CAS_CONAX_TEXT)
	s_ulCxUserTextTimerId = 0;
	s_ulCxUserTextRetryTimerId = 0;
	s_ulCxUserTextRetryTrickTimerId = 0;
	s_pstUserTextList = NULL;
#endif

#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
	s_ulCxFingerprintTimerId = 0;
	s_ulCxFingerprintRetryTimerId = 0;
	s_ulCxFingerprintRetryTrickTimerId = 0;
#endif

	s_eCxUiDisplayed = eCxUi_None;

	nResult = CX_SEM_Create("CxCoreUi", &s_ulCxUiSemId);
	if (nResult != ERR_OK)
	{
		HxLOG_Error("Failure in Semaphore Creation \r\n");
		return ERR_FAIL;
	}

	for (i = 0; i < eCxUi_GroupMax; i++)
	{
		s_astCxUiLastMsg[i] = eCxUi_GroupAll_None;
	}
#if 0
	s_eCxUiLevel = eCxUi_GroupBasic;
	s_eCxUiMsg = eCxUi_GroupAll_None;
	s_ulCxUiSeqNum = 0;
#endif

#if defined(CONFIG_MW_CAS_CONAX_TEXT)
	CX_TIMER_EventAfter(CX_USER_TEXT_CHECK_TIME, local_cxui_TimerHandler, NULL, 0, &s_ulCxUserTextTimerId);
#endif

	LEAVE_FUNCTION;

	return ERR_OK;
}

#define UI__________________________________________________

/*
* 한번에 한가지만 표시해라
* 새것이 헌것을 update한다
* 내용이 바뀌면 바로 update되야 한다
* descrambling/return channel/smarcard mmi 메시지는 채널 체인지 할때 없어져야 한다. exit 필요없이 ch+/- 바로 먹어야 한다.
* Conax라는 말은 menu외에는 쓰지마라
* 모든 메시지는 timeout 이 있어야 한다. 단, order_info, accept_viewing_info, token_debit_info, unlocking maturity rating은 예외
* priority
	smartcard > menu > return channel > maturity rating > PPV > messaging > basic UI

+ smartcard 메시지 - 1줄 메시지
	: no card / incorrect / incorrect conax / comm error / ready

- menu 메시지
	: list 등등 여러가지 형태

- return channel - 1줄 메시지
	: 	connecting, please wait...
		busy, please try again
		order accepted
		order failed / error code: 'pi-statue'

+ maturity rating - 여러줄 + ca pin
	: maturity lock
	program rating: PG
	card level: A
	enter CA PIN to unlock: ****

+ PPV - 여러줄 + ca pin

- Messaging - 여러줄

+ Basic
	: ok / no access / no access blackout / no access network
*/



// app는 ui를 닫고나면 이걸 호출해줘야 한다.
// user가 close시키든, menu 진입, guide화면 진입 혹은 channel change 등에 의해서 ui가 닫혀도
// 이걸 불러줘야 다음 ui 동작이 제대로 이루어질것이다.
HERROR CXAPI_UiClosed(CxUiMsgType_t eMsg)
//HERROR CXUI_UiClosed(CxUiMsgType_t eMsg, void *pParam)
{
	HUINT32	nLevel;

	CX_SEM_Get(s_ulCxUiSemId);
	ENTER_FUNCTION;

	// basic이나 s/c는 user가 바꾸는 것이 아니다.
	// ppv/mr/menu 는 user가 닫으면 close되는 것이다.

	nLevel = CXUI_GET_MSG_LEVEL(eMsg);

	switch (nLevel)
	{
	case eCxUi_GroupMessage:
	case eCxUi_GroupPpv:
	case eCxUi_GroupMr:
	case eCxUi_GroupRc:
	case eCxUi_GroupMenu:
		s_astCxUiLastMsg[nLevel] = eCxUi_GroupAll_None;
		break;

	default:
		break;
	}

	CX_SEM_Release(s_ulCxUiSemId);

	LEAVE_FUNCTION;
	return ERR_OK;
}

// app 에 전달되는 pParam은 나중에 CXUI_UiClosed()호출할때 다시 줘야 제대로 free한다.
// return 값으로는 실제 display여부를 알지 못해요.. 그렇게 하면 좋을 거 같긴한데.. 필요하려나 ??
HERROR local_cxui_Display(Handle_t hSession, CxUiPriority_t eLevel, CxUiMsgType_t eMsg, HUINT32 ulParam)
{
	//	int				i;
	//	HBOOL			bHigherMsgExist;
	Handle_t		hStbHandle;
	//	CxUiPriority_t		eDisplayLevel		= eLevel;
	CxUiMsgType_t	eDisplayMsg		= eMsg;
	HUINT32			ulDisplayParam	= ulParam;
	HBOOL			bDisplay		= FALSE;

	CX_SEM_Get(s_ulCxUiSemId);

	ENTER_FUNCTION;

	HxLOG_Print("\t-->UI<%x, %s>\n", eLevel, CXUI_DEBUG_GetMsgTypeStr(eMsg));

	// core 외부로 보낼때는 hStbHandle을 사용한다.
	if (hSession != HANDLE_NULL)
	{
		hStbHandle = CXSESSION_GetStbHandle(hSession);
	}
	else
	{
		// broadcasting 혹은 main view에만 보내는 거같은 개념..
		// <user text>나 s/c 넣어라.. 같이 main view에서만 표시해야 한다거나,,,  뭐 app쪽에서 알아서 판단 하시길.
		hStbHandle = HANDLE_NULL;
	}

	// other group은 별개로 취급한다.
	if (eLevel == eCxUi_GroupOther)
	{
		// 일단 display한다. (실제 display여부는 app 등에서 결정)
		bDisplay = TRUE;
	}
	/*
		else if (eLevel == eCxUi_GroupMenu)
		{
			// 별도로 처리한다
			// s/c가 있으면 s/c를 보내고, 아니면 menu 메시지를 보낸다.
			// 해당 group은 새로운 메시지로 update한다.
			s_astCxUiLastMsg[eLevel] = eMsg;
			// display를 할 것이다.
			bDisplay = TRUE;
			if (s_astCxUiLastMsg[eCxUi_GroupSc] != eCxUi_GroupAll_None)
			{
				//그런데, menu 대신  sc 메시지를 display한다.
				eDisplayLevel		= eCxUi_GroupSc;
				eDisplayMsg		= s_astCxUiLastMsg[eCxUi_GroupSc];
				ulDisplayParam	= 0;
			}
		}
	*/
	else if (eLevel < eCxUi_GroupMax)
	{
#if 1
		// priority에 대한 판단은 ap에 맡기고 항상 올린다
		bDisplay = TRUE;
		s_astCxUiLastMsg[eLevel] = eMsg;
#else
		// priority가 높은 메시지가 있는지 확인한다.
		bHigherMsgExist = FALSE;

		for (i = eLevel + 1; i < eCxUi_GroupMax; i++)
		{
			if (s_astCxUiLastMsg[i] != eCxUi_GroupAll_None)
			{
				bHigherMsgExist = TRUE;
				break;
			}
		}
		//DDI_UART_Print("bHigherMsgExist=%d\n", bHigherMsgExist);
		if (bHigherMsgExist)
		{
			// priority가 높은 메시지가 표시되고 있으므로 현재 메시지는 표시할 수 없다.
			// 기록은 해둔다.
			s_astCxUiLastMsg[eLevel] = eMsg;
		}
		else
		{
			//일단 같은 메시지도 다 올려보내고 app쪽에서 걸러내도록 하자. 화면 표시 상태를 모두 알수 없으니 여기서는 처리가 곤란

			// 해당 group은 새로운 메시지로 update한다.
			s_astCxUiLastMsg[eLevel] = eMsg;

			// display를 할 것이다.
			bDisplay = TRUE;
		}
#endif
	}
	else
	{
		// unknown eLevel
		CX_SEM_Release(s_ulCxUiSemId);
		return ERR_FAIL;
	}

	// sem_release는 어느위치가 더 적당한가 ??
	//	CX_SEM_Release(s_ulCxUiSemId);

	//## display한다.
	if (bDisplay)
	{
		CXUI_Display(hStbHandle, eDisplayMsg, ulDisplayParam);
	}

	CX_SEM_Release(s_ulCxUiSemId);
	LEAVE_FUNCTION;
	return ERR_OK;
}

#define CaPin__________________________________________________

HERROR local_cxui_EnterCaPin(Handle_t hSession, HINT32 nScSlot, CxPinCheckType_t ePinUse, HUINT8* pucPin)
{
	HERROR	nErr;
	ENTER_FUNCTION;

	nErr = CXSC_EnterCaPin(hSession, nScSlot, ePinUse, pucPin);

	LEAVE_FUNCTION;

	return nErr;
}

#if defined(CONFIG_MW_MM_PVR)
static HERROR local_cxui_EnterCaPinToSameSession(Handle_t hSession, HUINT8* pucPin, CxPinCheckType_t ePinCheckType)
{
	Handle_t	ahSession[CX_MAX_NUM_OF_SESSION];
	HUINT32		ulCount, i;
	HERROR		err;
	HINT32		nScSlot;
	Handle_t	hLiveStbHandle, hTsrStbHandle, hSvc;

	hLiveStbHandle = hTsrStbHandle = HANDLE_NULL;

	nScSlot = CXSESSION_GetScSlot(hSession);

	if (nScSlot == CX_SC_NONE)
	{
		//이렇게 될 경우가 있을까?
		return ERR_FAIL;
	}

	hSvc = CXACT_GetSvcHandleFromStbHandle(CXSESSION_GetStbHandle(hSession));
	ulCount = CXSESSION_GetAllSessionByScSlot(nScSlot, ahSession);

	HxLOG_Error("SC SLOT <%d> / Session Number <%d>\n", nScSlot, ulCount);

	for (i = 0 ; i < ulCount ; i++)
	{
		// 이미 한번 보낸 session이기에 건너뛰고...
		if (hSession == ahSession[i])
		{
			continue;
		}

#if 0
		hLiveStbHandle = CXSESSION_GetStbHandle(hSession);
		hTsrStbHandle = CXSESSION_GetStbHandle(ahSession[i]);

		// pin 관련 채널이 아닌데 단순히 session이 선택되어 있다고 해서 다 보내면 될까??
		if ((CXSESSION_IsSelected(ahSession[i]) == TRUE) && (CXSESSION_IsTsrSession(ahSession[i]) == TRUE) && (GET_SVCHANDLE_FROM_STBHANDLE(hLiveStbHandle) == GET_SVCHANDLE_FROM_STBHANDLE(hTsrStbHandle)))
		{
			err = local_cxui_EnterCaPin(ahSession[i], nScSlot, ePinCheckType, pucPin);
			HxLOG_Error("Session Index[%d], err[%d]\n", i, err);
		}
#else
		if(CXSESSION_IsSelected(ahSession[i]) != TRUE)
			continue;

		if(CXACT_GetSvcHandleFromStbHandle(CXSESSION_GetStbHandle(ahSession[i])) != hSvc)
			continue;

		err = local_cxui_EnterCaPin(ahSession[i], nScSlot, ePinCheckType, pucPin);
#endif
	}

	(void)(err);
	(void)(hLiveStbHandle);
	(void)(hTsrStbHandle);

	return ERR_OK;
}
#endif

#if defined(USE_CONAX_EMULATOR)
// do not
#else
// return value : ERR_OK /ERR_FAIL / CX_ERR_UI_WRONG_PIN /CX_ERR_UI_PIN_LOCKED
HERROR CXAPI_UiEnterCaPin(HINT32 nScSlot, HUINT8* pucPin)
{
	HERROR	nErr;
	ENTER_FUNCTION;


	nErr = local_cxui_EnterCaPin(HANDLE_NULL, nScSlot, eCxPinCheck_General, pucPin);

	LEAVE_FUNCTION;

	return nErr;
}
#endif	// End of defined(USE_CONAX_EMULATOR)

#define Session__________________________________________________

/*
session 관련 메시지
	- 채널 변경: s/c, menu를 제외한 모든 메시지 clear
	- free-to-air (요건 해야되나 말아야 되나 ?) scrambled -> free 로 변경된 경우 no access를 지워주어야지요..
*/
HERROR CXUI_SessionStart(Handle_t hSession)
{
	ENTER_FUNCTION;
	// SC group 의 UI가 있다면 다시 뿌려준다.
	if (s_astCxUiLastMsg[eCxUi_GroupSc] != eCxUi_GroupAll_None)
	{
		local_cxui_Display(hSession, eCxUi_GroupSc, s_astCxUiLastMsg[eCxUi_GroupSc], 0);
	}
	LEAVE_FUNCTION;

	return ERR_OK;
}

HERROR CXUI_SessionStop(Handle_t hSession)
{
	ENTER_FUNCTION;
	// basic group (no access 등)의 UI가 있다면 제거
	if (s_astCxUiLastMsg[eCxUi_GroupBasic] != eCxUi_GroupAll_None)
	{
		local_cxui_Display(hSession, eCxUi_GroupBasic, eCxUi_GroupAll_None, 0);
	}

	// ppv group 의 UI가 있다면 제거
	if (s_astCxUiLastMsg[eCxUi_GroupPpv] != eCxUi_GroupAll_None)
	{
		local_cxui_Display(hSession, eCxUi_GroupPpv, eCxUi_GroupAll_None, 0);
	}

	// MR group 의 UI가 있다면 제거
	if (s_astCxUiLastMsg[eCxUi_GroupMr] != eCxUi_GroupAll_None)
	{
		local_cxui_Display(hSession, eCxUi_GroupMr, eCxUi_GroupAll_None, 0);
	}

	// message, RC, Menu, SC는 일단 제외함 - channel이 변경되도 나와야 하는거 아닌가 싶어서..

	LEAVE_FUNCTION;
	return ERR_OK;
}

#define Smartcard__________________________________________________
/*
Smartcard 관련 메시지
	> No card inserted
	> Incorrect Conax card 				-- CASS_VER랑 HOST_VER가 다름
	> Incorrect card 					-- CASS_VER가 안 왔음
	> Card problem - check card		-- comm. fail
*/
HERROR CXUI_ScStateChanged(HINT32 nScSlot, CxSc_State_t eScState, HBOOL bShowAV)
{
	HUINT32			ulCount;
	Handle_t		ahSession[CX_MAX_NUM_OF_SESSION];
	CxUiMsgType_t	eMsgType;
	int				i;

	ENTER_FUNCTION;

	switch (eScState)
	{
	case eCxSc_StateNone:
		//no card inserted
		eMsgType = eCxUi_GroupSc_NoCard;
		break;

	case eCxSc_StateInserted:
		// s/c detected
		eMsgType = eCxUi_GroupSc_Inserted;
		break;

	case eCxSc_StateReady:
		//sc 관련 메시지 삭제
		eMsgType = eCxUi_GroupAll_None;
		break;

	case eCxSc_StateDifferentVer:
#if !defined(CONFIG_OP_TELENOR)
		//incorrect conax card
		eMsgType = eCxUi_GroupSc_IncorrectConax;
		break;
#endif

	case eCxSc_StateInvalid:
		//incorrect card
		eMsgType = eCxUi_GroupSc_Incorrect;
		break;

	case eCxSc_StateCommError:
		//card problem - check card
		eMsgType = eCxUi_GroupSc_CommFail;
		break;

	default:
		//do nothing
		HxLOG_Error("Wrong param\n");
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	// 모든 세션을 얻어옴
	ulCount = CXSESSION_GetAllSessionByScSlot(CX_SC_NONE, ahSession);

	if (ulCount == 0)
	{
		local_cxui_Display(HANDLE_NULL, eCxUi_GroupSc, eMsgType, bShowAV);
	}
	else
	{
		for (i = 0; i < ulCount; i++)
		{
			local_cxui_Display(ahSession[i], eCxUi_GroupSc, eMsgType, bShowAV);
		}
	}

	LEAVE_FUNCTION;
	return ERR_OK;
}

#if defined(USE_CONAX_EMULATOR)

#else
#define Menu__________________________________________________

// Smart Card State
HERROR CXAPI_UiGetSCState(HINT32 lScSlot)
{
	return	CXSC_GetSCState(lScSlot);
}
/*
CAS Menu
	Conax Conditional Access / Conax CA
		Subscription Status
		Event Status
		Tokens Status
		Change CA PIN			-- enter old&new pin/confirm/ PIN confirmation does not match / Incorrect old PIN / Change CA PIN failed
		Maturity Rating			-- guilde line 문서 30 page 참조 (select new level / enter pin)
		Modem Ordering
		Message
		About Conax CA			-- HOST_VER / s/w ver / smartcard number / num of session / country indicator / ca_sys_id (guideline 32 page)
*/

// subscription/event/credit/purse/debit 나눌까... 아니면 그냥 둘까...??
// return되는 status info는 s/c가 빠지거나 다음 status info 조회를 하기전까지는 유효하며, 따로 free할 필요 없음
CxStatusList_t* CXAPI_UiGetStatusInfo(HINT32 nScSlot, CxStatusSelectType_t eType, HUINT32 ulRef)
{
	CxStatusList_t* pstList;

	ENTER_FUNCTION;

	pstList = CXSC_GetCaStatus(nScSlot, eType, ulRef);

	LEAVE_FUNCTION;
	return pstList;
}


HERROR CXAPI_UiChangePin(HINT32 nScSlot, HUINT8* pucOldPin, HUINT8* pucNewPin, HUINT8* pucNewPin2)
{
	HERROR	nErr;

	ENTER_FUNCTION;

	if (pucOldPin == NULL || pucNewPin == NULL || pucNewPin2 == NULL)
	{
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	//	 if (CX_memcmp(pucNewPin, pucNewPin2, 4))
	if (*(HUINT32 *)pucNewPin != *(HUINT32 *)pucNewPin2)
	{
		// new pin != new pin2
		LEAVE_FUNCTION;
		return CX_ERR_UI_DIFFERENT_NEW_PIN;
	}

	nErr = CXSC_ChangeCaPin(nScSlot, pucOldPin, pucNewPin);

	if (nErr != ERR_OK)
	{
		if (nErr == CX_ERR_SC_PIN_LOCKED)
		{
			LEAVE_FUNCTION;
			return CX_ERR_UI_PIN_LOCKED;
		}
		else if (nErr == CX_ERR_SC_WRONG_PIN)
		{
			LEAVE_FUNCTION;
			return CX_ERR_UI_WRONG_PIN;
		}

		/*only display this MMI in other case*/
		LEAVE_FUNCTION;
		return CX_ERR_UI_PIN_CHANGE_FAIL;
	}

	LEAVE_FUNCTION;
	// 성공~
	return ERR_OK;
}

HERROR CXAPI_UiGetMaturityRatingLevel(HINT32 nScSlot, HUINT8* pucMaturityRating)
{
	HINT32	nRet;

	ENTER_FUNCTION;

	nRet = CXSC_GetMaturityRatingLevel(nScSlot);
	if (nRet < 0)
	{
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	*pucMaturityRating = (HUINT8)nRet;

	LEAVE_FUNCTION;
	return ERR_OK;
}

HERROR CXAPI_UiChangeMaturityRating(HINT32 nScSlot, HUINT8 ucMaturityRating, HUINT8* pucPin)
{
	HERROR	nErr;

	ENTER_FUNCTION;

	if (pucPin == NULL)
	{
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	nErr = CXSC_ChangeMaturityRating(nScSlot, ucMaturityRating, pucPin);

	if (nErr != ERR_OK)
	{
		if (nErr == CX_ERR_SC_PIN_LOCKED)
		{
			LEAVE_FUNCTION;
			return CX_ERR_UI_PIN_LOCKED;
		}

		LEAVE_FUNCTION;
		return CX_ERR_UI_WRONG_PIN;
	}
	// 성공~
	LEAVE_FUNCTION;
	return ERR_OK;
}

// 아래 함수 호출할 때, *pulNumMsg에다가 pstUserText array의 size를 넘겨주어야 함
// return 할 때는 *pulNumMsg에 실제 copy된 user_text의 개수가 넘어감
HERROR CXAPI_UiGetUserText(HUINT32* pulNumMsg, CxUserTextList_t* pstList)
{
#if defined(CONFIG_MW_CAS_CONAX_TEXT)
	register int	i;
	HUINT32			ulSlotNum;
	HERROR			hErr;
	UNIXTIME		utCurrTime, utDBStartTime;
	HUINT32			ulCount			= 0;

	ENTER_FUNCTION;

	*pulNumMsg = 0;

	if (pstList == NULL)
	{
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	ulSlotNum = CXNVM_GetUserTextSlotNum();

	if (s_pstUserTextList == NULL)
	{
		s_pstUserTextList = CX_MEM_Alloc(ulSlotNum * sizeof(CxUserText_t));

		if (s_pstUserTextList == NULL)
		{
			LEAVE_FUNCTION;
			return ERR_FAIL;
		}
	}

	for (i = 0; i < ulSlotNum; i++)
	{
		CX_memset(&s_pstUserTextList[i], 0, sizeof(CxUserText_t));

		if (CXNVM_ReadUserText(i, &s_pstUserTextList[i]) != ERR_OK)
		{
			// user text는 slot이 random하게 들어올 수 있다.
			// 그런데, 중간에 빠지는 slot을 읽었을 때 fail처리해서 return하면
			// ConaxUserText.dat가 없는 상태에서 msgScheduledMailBox.trp만 재생한 경우 처리가 안 됨
			HxLOG_Error("%d slot's user text read failed !!!\r\n", i);
		}

		s_pstUserTextList[i].szText[CX_MAX_USER_TEXT_LEN] = '\0';

		hErr = VK_CLOCK_GetTime((HULONG *)&utCurrTime);

		/* 미래 시작 메시지는 메뉴에 전달하지 않는다. */
		if (hErr == ERR_OK)
		{
			// mailbox list에 message가 보이지 않는 문제. currentTime(offset x), dateTime(offset o) 가 제대로 비교되지 않음.
			HLIB_DATETIME_ConvertDateTimeToUnixTime(&(s_pstUserTextList[i].stStartDataTime), &utDBStartTime);
			if (utCurrTime < utDBStartTime)
			{
				CX_memset(&s_pstUserTextList[i], 0, sizeof(CxUserText_t));
			}
			else
			{
				//don't copy to list, don't display until the start time.
			}

			//Immediate message
			if (s_pstUserTextList[i].usDuration != 0)
			{
				// Don't count 'Message's...
			}
			//실제 UserText가 들어있는 slot 만 사용하기 위한 count :: slot count 는 따로 관리하고 있지 않음
			else if (strlen(s_pstUserTextList[i].szText) > 0)
			{
				ulCount++;
			}
		}
	}

	pstList->pstUserText = s_pstUserTextList;
	pstList->ulCount = ulCount;
	*pulNumMsg = ulSlotNum;

	if (pstList->pstUserText == NULL)
	{
		CX_MEM_Free(s_pstUserTextList);
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	LEAVE_FUNCTION;

	return ERR_OK;
#else
	HxLOG_Error("[NOT SUPPORT]\r\n");
	return ERR_FAIL;
#endif
}

HERROR CXAPI_UiGetVersionInfo(HINT32 nScSlot, CxVersionInfo_t* pstInfo)
{
	ENTER_FUNCTION;

	if (pstInfo == NULL)
	{
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	pstInfo->nHostVer = CXSC_GetHostVersion();

	// 만약 카드가 없다면 아래 값들은 (-1)이 들어갈꺼다...
	pstInfo->nSessionCnt = CXSC_GetSessionCount(nScSlot);
	pstInfo->nCountryInd = CXSC_GetCountryIndicator(nScSlot);
	pstInfo->nCaSysId = CXSC_GetCaSysId(nScSlot);

	pstInfo->ulCardNumber = CXSC_GetCardNumber(nScSlot);
	pstInfo->pszSwVer = CX_CORE_VERSION_STR;

	//## chipset pairing 모델 진행시 추가 구현..
#if defined(CONFIG_MW_CAS_CONAX_CPM)
	(void)svc_cas_DevPortSysGetChipId(&pstInfo->ulChipId);
#else
	pstInfo->ulChipId = 0;
#endif

	LEAVE_FUNCTION;
	return ERR_OK;
}


HERROR CXAPI_UiGetCountryIndicator(HINT32 nScSlot, HINT32* nCountryIdx)
{
	HINT32	nIndex;

	ENTER_FUNCTION;

	nIndex = CXSC_GetCountryIndicator(nScSlot);
	*nCountryIdx = nIndex;

	LEAVE_FUNCTION;

	return ERR_OK;
}


#endif	// End of defined(USE_CONAX_EMULATOR)

#define UserText_Fingerprinting__________________________________________________

/*
text & fingerprint
*/

//## 언제 이걸 호출할 건가?? - stb 시작하고 나서 한번만 부르면 될 거 같은데요 ??
// Booting / Standby -> Wakeup 시에 호출 된다.
HERROR CXUI_CheckUserTextAvailable()
{
#if defined(CONFIG_MW_CAS_CONAX_TEXT)
	register int	i;
	HUINT32			ulCnt;
	CxUserText_t	stText;
	HERROR			hErr			= ERR_OK;

	ENTER_FUNCTION;

	ulCnt = CXNVM_GetUserTextSlotNum();

	for (i = 0; i < ulCnt; i++)
	{
		if (CXNVM_ReadUserText(i, &stText) != ERR_OK)
		{
			HxLOG_Error("[Read Fail] Don't read UserText\r\n");
			LEAVE_FUNCTION;
			return ERR_FAIL;
		}

		if (stText.ucRepeat > stText.ucDisplayed)
		{
			hErr = CXUI_CheckScheduledMsgAvailable(&stText);
			if (hErr != ERR_OK)
			{
				continue;
			}

			HxLOG_Error("[In Booting or Wakeup] repeat(%d), displayed(%d)\n", stText.ucRepeat, stText.ucDisplayed);
#if defined(CONFIG_DEBUG)
			HxLOG_Error("[In Booting or Wakeup] repeat(%d), displayed(%d)\n", stText.ucRepeat, stText.ucDisplayed);
#endif
			CXUI_UserTextReceived(&stText, eCxUTFlagFromFlash);
			break;
		}
	}

	LEAVE_FUNCTION;
	return ERR_OK;
#else
	HxLOG_Error("[NOT SUPPORT]\r\n");

	return ERR_FAIL;
#endif
}
#if defined(CONFIG_MW_CAS_CONAX_TEXT)
void CXUI_RetryUserText(HUINT32 ucFlagFrom)
{
	ENTER_FUNCTION;

	CXUI_UserTextReceived(&s_stUserText, (CxUTFlagFrom_t)ucFlagFrom);

	LEAVE_FUNCTION;
}
#endif
HULONG	ulCheck_Recv_UTTime;
HULONG	ulCheck_Update_SITime;
// Immediate & Repeat Message 을 위한 value
// Standby 이슈를 해결하기 위함

// UserText를 받자마자 Display 함과 동시에 set
STATIC void local_cas_cx_SetRecvUTTimeStamp(void)
{
	ulCheck_Recv_UTTime = VK_TIMER_GetSystemTick();
}

// Time Source가 SI 로 update 되면 바로 set
STATIC void local_cas_cx_SetUpdateSITimeStamp(void)
{
	ulCheck_Update_SITime = VK_TIMER_GetSystemTick();
}
// 반복 & Immediate Message처리
// Standby -> Wake up 시에, Start time이 없어서 발생하는 문제를 해결하기 위해
// ulImmediateStartTime 값을 사용
HERROR local_cxui_SetRepeatImmediateMsgTime(CxUserText_t* pstText, UNIXTIME* unixtime)
{
	HxDATETIME_t	stCurTime;
	HUINT32			ulCurrentTime		= 0,ulCurrentUnixTime = 0;
	HULONG			ulOffsetUpdateTime	= 0;

	if (pstText->ulImmediateStartTime > 0)
	{
		// (최초 time 유지)덮어쓰지 않기 위해 '0' or '음수'인 경우에만 Write 시도.
		// Flash에서 읽은 데이터는 처리하지 않음
		return ERR_OK;
	}
	// Repeat Immediate Message 를 위한 Current Time setting :: Standby -> Wake Up 시나리오를 위해 필요함
	CX_memset(&stCurTime, 0, sizeof(HxDATETIME_t));

	// Get Current Time
	if (CXUTIL_GetTime(&ulCurrentTime) != ERR_OK)
	{
		HxLOG_Error("Can't Get Current Time !!!!");
		return	ERR_FAIL;
	}

	HLIB_DATETIME_ConvertUnixTimeToDateTimeWithOutOffset(ulCurrentTime, &stCurTime);
	HLIB_DATETIME_ConvertDateTimeToUnixTime(&stCurTime, &ulCurrentUnixTime);

	ulOffsetUpdateTime = (ulCheck_Update_SITime - ulCheck_Recv_UTTime) / 1000;
	ulCurrentUnixTime = (ulCurrentUnixTime - (HUINT32)ulOffsetUpdateTime);
	CXNVM_OverWriteUserText(pstText->ucSeqNo, ulCurrentUnixTime);

	*unixtime = ulCurrentUnixTime;

	return ERR_OK;
}
/*
  User Text가 TS에서 전달된 것인지,
  Booting하고 Flash에서 읽어온 것인지
  구분하여 Current Time 비교를 달리한다.
*/
HERROR local_cxui_ClassifyIdentityTSOrFlash(CxUserText_t* pstText, CxUTFlagFrom_t ucFlagFrom)
{
	HUINT32			ulCurrentTime		= 0;
	HxDATETIME_t	stCurTime;
	HxDATETIME_t	stUTTime;
	UNIXTIME		stUnixTime			= 0;	/*UnixTime이 0 이면 Instant Message*/
	TxSOURCE_e		eClockSetMode		= 0;
	HERROR			nErr;
	UNIXTIME		ulCurrentUnixTime	= 0, ulFPUnixTime = 0;

	// time = 0 이면 바로 표시할 수도 있고, time > 0이면 해당 시간에 표시해야 하고..
	// duration = 0이면 mail icon을 보여주는 정도이어야 하고 (혹은 메시지 도착했음..정도)
	// repeat > 1이면 반복해서 보여줘야 하고...

	// #미래시간이 아닌 (1970 && 0 ) 인 경우는 Instant 메시지이므로, DateTime을 다시 reset해줌
	// # 이유는 # local_cxui_TimerHandler() 에서 atboot() 조건에도 해당 API()를 호출하면서,
	//   DB의 Data를 긁어와버린다. 결과로... DateTime이 변경되면서, 꼬이게 된다.
	// # UnixTime 과 DateTime을 Converting 하여 예외처리함 In PT

	CX_memset(&stUTTime, 0, sizeof(HxDATETIME_t));
	CX_memset(&stCurTime, 0, sizeof(HxDATETIME_t));

	HLIB_DATETIME_ConvertDateTimeToUnixTimeWithOutOffset(&(pstText->stStartDataTime), &stUnixTime);

	if (stUnixTime == 0)
	{
		CX_memset(&pstText->stStartDataTime, 0, sizeof(HxDATETIME_t));
	}

	nErr = TAPI_GetTimeSource(&eClockSetMode);
	if (nErr != ERR_OK)
	{
		HxLOG_Error(" TAPI_GetTimeSource() Fail !!\n");
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	// Get Current Time
	if (CXUTIL_GetTime(&ulCurrentTime) != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return	ERR_FAIL;
	}

	HLIB_DATETIME_ConvertUnixTimeToDateTimeWithOutOffset(ulCurrentTime, &stCurTime);
	HLIB_DATETIME_ConvertDateTimeToUnixTime(&stCurTime, &ulCurrentUnixTime);

	HLIB_DATETIME_ConvertDateTimeToUnixTimeWithOutOffset(&pstText->stStartDataTime, &ulFPUnixTime);


	HxLOG_Error("<<ulCurrentUnixTime=%ld, (%d/%02d/%02d) (%2d:%02d:%02d)>>\n", ulCurrentUnixTime, stCurTime.stDate.usYear, stCurTime.stDate.ucMonth, stCurTime.stDate.ucDay, stCurTime.stTime.ucHour, stCurTime.stTime.ucMinute, stCurTime.stTime.ucSecond);
	HxLOG_Error("<<ulFPUnixTime=%ld, (%d/%02d/%02d) (%2d:%02d:%02d)>>\n", ulFPUnixTime, pstText->stStartDataTime.stDate.usYear, pstText->stStartDataTime.stDate.ucMonth, pstText->stStartDataTime.stDate.ucDay, pstText->stStartDataTime.stTime.ucHour, pstText->stStartDataTime.stTime.ucMinute, pstText->stStartDataTime.stTime.ucSecond);

	HxLOG_Error("local_cxui_ClassifyIdentityTSOrFlash(eClockSetMode(%d), stUnixTime(%d), ulCurrentUnixTime(%d), pstText->usDuration(%d) )\n", eClockSetMode, stUnixTime, ulCurrentUnixTime, pstText->usDuration);

#if 0	// USER_TEXT STBY rollback
	if (ucFlagFrom == eCxUTFlagFromFlash)
	{
		// Immediate message 인 경우 SI time 받기 위한 timer 필요 없음
		// 동일 함수 내 if(stUnixTime==0) 조건에서 immediate message인 경우 stStartDataTime을 초기화 함
		if (pstText->stStartDataTime.stDate.usYear == 0 && pstText->stStartDataTime.stDate.ucMonth == 0 && pstText->stStartDataTime.stDate.ucDay == 0)
		{
			return ERR_FAIL;
		}
		// PI_STARTTIME이 존재하고 반복이 남은 경우 SI time 받기 위한 timer 필요 없음. locas_cas_cx_AdjustUTIntervalTimeOnBoot() 함수 내에 처리 부분과 맞물려 동작함.
		else if ((pstText->ucDisplayed > 0) && (pstText->ucDisplayed < pstText->ucRepeat))
		{
			return ERR_FAIL;
		}
		else
		{
			// need timer.....
			return ERR_OK;
		}
	}
#endif
	// From Flash case
	if ((ucFlagFrom == eCxUTFlagFromFlash)) //&&(stCurTime.stDate.usYear < 2000) // SI time을 기다리면, "< 2000" 조건 필요 없다.
	{
		if (eClockSetMode != TxSOURCE_SET_BY_SI)
		{
			/*	// Immediate 인 경우는 바로 뿌리고 싶다면, 사용할 코드이지만... Video 없는 상태(Balck Screen)에서도 Display 되버린다...
			if(stUnixTime==0)
			{
				// no timer.....
				return ERR_FAIL;
			}
			*/

			// need timer.....
			return ERR_OK;
		}
	}
	else if ((ucFlagFrom == eCxUTFlagFromTS) &&					// Stream으로부터 전달되는 User Text만 처리.
			 (eClockSetMode != TxSOURCE_SET_BY_SI) &&				// SI를 통해 Clock이 set되기를 기다린다.
			 (stUnixTime != 0/* && pstText->usDuration != 0 */))	// Immediate && StartTime 값 없으면, 현재 시간 보정 기다리지 않는다.
	// CXHD-5100C 2011-08 1st Conax PT. 바로 위의 조건문이 왜 필요하지... 위의 조건으로 mailbox indication이 나오지 않는다. 그래서 주석으로 막아 놓음.
	{
		// need timer.....
		return ERR_OK;
	}

	return ERR_FAIL;
}

#if defined(CONFIG_MW_CAS_CONAX_TEXT)
STATIC HERROR local_cxui_CheckStartTimeIsValid(CxUserText_t* pstText)
{
	HINT32		i;
	HUINT8		ucRepeat			= 0;
	HUINT32		ulIntervalSec		= 0, ulTmpStartTime	= 0;
	HBOOL		bStartTimeIsValid	= FALSE;
	HERROR		hErr				= ERR_OK;
	UNIXTIME	ulCurTime			= 0, ulStartTime = 0;

	if (pstText == NULL)
	{
		HxLOG_Print("There is no Text Data.\r\n");
		return ERR_FAIL;
	}

	hErr = HLIB_DATETIME_ConvertDateTimeToUnixTimeWithOutOffset(&pstText->stStartDataTime, &ulStartTime);
	if (hErr != ERR_OK)
	{
		HxLOG_Print("Fail to convert DateTime to UnixTime.\r\n");
		return ERR_FAIL;
	}

	hErr = VK_CLOCK_GetTime((HULONG *)&ulCurTime);
	if (hErr != ERR_OK)
	{
		ulCurTime = 0;
	}

	if (ulCurTime > ulStartTime)
	{
		ucRepeat = pstText->ucRepeat;
		ulIntervalSec = (HUINT32)pstText->usInterval * 10;
		ulTmpStartTime = ulStartTime;

		if (ucRepeat > 1)
		{
			for (i = 1; i < ucRepeat; i++)
			{
				// 반복해서 보여 줄 때, 앞으로 보여 줄 시간대가 하나라도 유효하면 유효한 시간부터 저장해 display 한다.
				ulTmpStartTime += ulIntervalSec;
				if (ulCurTime <= ulTmpStartTime)
				{
					bStartTimeIsValid = TRUE;
					// 유효한 시간으로 StartTime 변경.
					HLIB_DATETIME_ConvertUnixTimeToDateTime(ulTmpStartTime, &pstText->stStartDataTime);
					// Repeat 횟수도 변경.
					pstText->ucRepeat = ucRepeat - (HUINT8)i;
					break;
				}
			}
		}

		if (bStartTimeIsValid != TRUE)
		{
			// 지나간 scheduled msg는 display 하지 않는다. ERR_FAIL!
			HxLOG_Print("All scheduled time are past >>> Do not display!\r\n");
			return ERR_FAIL;
		}
	}

	return ERR_OK;
}

HERROR CXUI_CheckScheduledMsgAvailable(CxUserText_t* pstText)
{
	HERROR	hErr	= ERR_OK;

	if (pstText == NULL)
	{
		HxLOG_Print("There is no Text Data.\r\n");
		return ERR_FAIL;
	}

	// Scheduled message의 경우 UserText를 display 할 시간 이 모두 과거라면 display 하지 않으므로 check.
	hErr = local_cxui_CheckStartTimeIsValid(pstText);
	if (hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}
#endif


// 여기서는 data 전달만 하고, 실제 표시는 CXAPI_UiDisplayUserText()를 통해서 해야해요..
// ==> 그냥 여기서 표시하는걸루 바꾸었어요. CXAPI_UiDisplayUserText()는 삭제합니다.
HERROR CXUI_UserTextReceived(CxUserText_t* pstText, CxUTFlagFrom_t ucFlagFrom)
{
#if defined(CONFIG_MW_CAS_CONAX_TEXT)
	CxUserText_t* 	pstCopy;
	HUINT32			ulNeedTimer		= 0;

	ENTER_FUNCTION;
	HxLOG_Print(" User Text is transported from (%s)!!\n", ucFlagFrom == 1 ? "TS" : "FLASH");

	ulNeedTimer = local_cxui_ClassifyIdentityTSOrFlash(pstText, ucFlagFrom);

	HxLOG_Print(" CXUI_UserTextReceived ulNeedTimer[%s]\n", ulNeedTimer == ERR_OK ? "Yes" : "No");

	if (ulNeedTimer == ERR_OK)
	{
		HxLOG_Print("run timer for adjust time source(%d)\n", s_ulCxUserTextRetryCount);

		if (s_ulCxUserTextRetryCount > CX_USER_TEXT_RETRY_COUNT)
		{
			HxLOG_Print(" Clcok Time isn't set by SI - cnt(%d)\n", s_ulCxUserTextRetryCount);
			return ERR_FAIL;
		}

		CX_memcpy(&s_stUserText, pstText, sizeof(CxUserText_t));

		if (s_ulCxUserTextRetryTimerId == 0)
		{
			s_ulCxUserTextRetryCount++;
			HxLOG_Print("Current time may be wrong. try again after %d sec.\r\n", CX_USER_TEXT_RETRY_TIME);

			CX_TIMER_EventAfter(CX_USER_TEXT_RETRY_TIME, local_cxui_TimerHandler, (void*)&ucFlagFrom, (int)sizeof(HUINT32), &s_ulCxUserTextRetryTimerId);
		}
	}
	else
	{
		s_ulCxUserTextRetryCount = 0;
		CX_memcpy(&s_stUserText, pstText, sizeof(CxUserText_t));

		pstCopy = (CxUserText_t *)CX_MEM_Alloc(sizeof(CxUserText_t));
		if (pstCopy == NULL)
		{
			return ERR_FAIL;
		}
		CX_memcpy(pstCopy, pstText, sizeof(CxUserText_t));
#if defined(CONFIG_DEBUG)
		CXUI_DEBUG_PrintData(eCxUi_GroupMessage_Display, (HUINT32)pstCopy);
#endif

		local_cxui_Display(HANDLE_NULL, eCxUi_GroupMessage, eCxUi_GroupMessage_Display, (HUINT32)pstCopy);
	}

	LEAVE_FUNCTION;
	return ERR_OK;
#else
	HxLOG_Print("[NOT SUPPORT]\r\n");

	return ERR_FAIL;
#endif
}

// 2011/7/20. 인증 종료 하면, 삭제 예정.
//local_cxui_ClassifyIdentityTSOrFlash() 추가하면서 backup 해 놓은 소스
#if 0
HERROR CXUI_UserTextReceived(CxUserText_t *pstText,CxUTFlagFrom_t ucFlagFrom)
{
#if defined(CONFIG_MW_CAS_CONAX_TEXT)
	CxUserText_t		 *pstCopy;
	HUINT32			ulCurrentTime = 0;
	HxDATETIME_t		stCurTime;
	UNIXTIME		stUnixTime=0;	/*UnixTime이 0 이면 Instant Message*/
	VK_CLOCK_SET_MODE		eClockSetMode = 0;
	HERROR					nErr;

	ENTER_FUNCTION;
	HxLOG_Print(" User Text is transported from (%s)!!\n",ucFlagFrom==1?"TS":"FLASH");

	//##일단 data만 전달하고 표시해야될 시점은 app쪽에서 계산해서 다시 요청한다.

	// time = 0 이면 바로 표시할 수도 있고, time > 0이면 해당 시간에 표시해야 하고..
	// duration = 0이면 mail icon을 보여주는 정도이어야 하고 (혹은 메시지 도착했음..정도)
	// repeat > 1이면 반복해서 보여줘야 하고...

	// #미래시간이 아닌 (1970 && 0 ) 인 경우는 Instant 메시지이므로, DateTime을 다시 reset해줌
	// # 이유는 # local_cxui_TimerHandler() 에서 atboot() 조건에도 해당 API()를 호출하면서,
	//   DB의 Data를 긁어와버린다. 결과로... DateTime이 변경되면서, 꼬이게 된다.
	// # UnixTime 과 DateTime을 Converting 하여 예외처리함 In PT
	CXMWC_UTIL_ConvertDateTimeToUnixTime(&(pstText->stStartDataTime),&stUnixTime);
	if(stUnixTime==0)
	{
		CX_memset(&pstText->stStartDataTime,0,sizeof(HxDATETIME_t));
	}

	nErr = VK_CLOCK_GetTimeSource(&eClockSetMode);
	if ( nErr != ERR_OK)
	{
		HxLOG_Print(" VK_CLOCK_GetTimeSource () Fail !!\n");
		return ERR_FAIL;
	}
	// SI 를 통한 시간설정이 완료 되기를 기다림
	// Immediate && StartTime 값 없으면, 현재 시간 보정 기다리지 않는다.
	//if (eClockSetMode != VK_CLOCK_SETBY_SI)
	if ((eClockSetMode != VK_CLOCK_SETBY_SI) && (stUnixTime != 0 && pstText->usDuration != 0))
	{
		if(s_ulUserTextRetryCount > CX_USER_TEXT_RETRY_COUNT)
		{
			HxLOG_Print(" Clcok Time isn't set by SI - cnt(%d)\n",s_ulUserTextRetryCount);
			return ERR_FAIL;
		}

		CX_memcpy(&s_stUserText, pstText, sizeof(CxUserText_t));

		if (s_ulCxUserTextRetryTimerId == 0)
		{
			s_ulUserTextRetryCount++;
			HxLOG_Print("Current time may be wrong. try again after %d sec.\r\n", CX_USER_TEXT_RETRY_TIME);
			CX_TIMER_EventAfter(CX_USER_TEXT_RETRY_TIME, local_cxui_TimerHandler, (void*)&ucFlagFrom, (int)sizeof(HUINT32),&s_ulCxUserTextRetryTimerId);
		}
	}
	else
	{
		s_ulUserTextRetryCount = 0;

		pstCopy = (CxUserText_t *)CX_MEM_Alloc(sizeof(CxUserText_t));
		if (pstCopy == NULL)
		{
			return ERR_FAIL;
		}

		CX_memcpy(pstCopy, pstText, sizeof(CxUserText_t));
#if defined(CONFIG_DEBUG)
		CXUI_DEBUG_PrintData(eCxUi_GroupMessage_Display, (HUINT32)pstCopy);
#endif
		local_cxui_Display(HANDLE_NULL, eCxUi_GroupMessage, eCxUi_GroupMessage_Display, (HUINT32)pstCopy);
	}

	LEAVE_FUNCTION;
	return ERR_OK;
#else
	HxLOG_Print("[NOT SUPPORT]\r\n");

	return ERR_FAIL;
#endif
}
#endif

#if defined(USE_CONAX_EMULATOR)
// do not
#else
HERROR CXAPI_UiUserTextDisplayed(HUINT8 ucSeqNo, CxUTDisplayCount_t ulOpt)
{
#if defined(CONFIG_MW_CAS_CONAX_TEXT)
	HERROR	nErr;

	ENTER_FUNCTION;

	nErr = CXNVM_UserTextDisplayed(ucSeqNo, ulOpt);

	LEAVE_FUNCTION;

	return nErr;
#else
	HxLOG_Print("[NOT SUPPORT]\r\n");

	return ERR_FAIL;
#endif
}
HERROR CXAPI_NVMRemoveUserText(HUINT8 ucSeqNo)
{
#if defined(CONFIG_MW_CAS_CONAX_TEXT)
	HERROR	nErr;

	ENTER_FUNCTION;

	nErr = CXNVM_RemoveUserText(ucSeqNo);

	LEAVE_FUNCTION;

	return nErr;
#else
	HxLOG_Print("[NOT SUPPORT]\r\n");

	return ERR_FAIL;
#endif
}
#endif

HERROR CXAPI_UiCxMenuDisplayed(HBOOL bDisplayed)
{
	if(bDisplayed)
		s_eCxUiDisplayed |= eCxUi_Menu;
	else
		s_eCxUiDisplayed &= (~eCxUi_Menu);

	return ERR_OK;
}

HERROR CXAPI_UiPinMmiDisplayed(HBOOL bDisplayed)
{
	if(bDisplayed)
		s_eCxUiDisplayed |= eCxUi_Pincode;
	else
		s_eCxUiDisplayed &= (~eCxUi_Pincode);

	return ERR_OK;
}

HBOOL CXUI_GetUiDisplayedState(HINT32 nScSlot)
{
	if(nScSlot != CX_SC_SLOT_0)
		return FALSE;

	return s_eCxUiDisplayed ? TRUE : FALSE;
}

#if 0
// userText를 표시할 수 있는지 허가를 얻는 의미임..
// 표시할 수 있다면 displayui가 갈 것임
HERROR CXAPI_UiDisplayUserText(CxUserText_t *pstText)
{
#if defined(CONFIG_MW_CAS_CONAX_TEXT)
	HERROR nErr;

	ENTER_FUNCTION;

	nErr = local_cxui_Display(HANDLE_NULL, eCxUi_GroupMessage, eCxUi_GroupMessage_Display, (HUINT32)pstText);

	LEAVE_FUNCTION;

	return nErr;
#else
	HxLOG_Print("[NOT SUPPORT]\r\n");

	return ERR_FAIL;
#endif
}
#endif

//## 언제 이걸 호출할 건가?? - stb 시작하고 나서 한번만 부르면 될 거 같은데요 ??
HERROR CXUI_CheckFingerprintAvailable()
{
#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
	CxFingerprint_t	stFingerprint;

	ENTER_FUNCTION;
	CX_memset(&stFingerprint, 0x00, sizeof(CxFingerprint_t));

	if (CXNVM_ReadFingerprint(&stFingerprint) == ERR_OK)
	{
		if (stFingerprint.ucDisplayed == FALSE)
		{
			HxLOG_Print("[In Booting or Wakeup] FP (%s)\n", stFingerprint.szText);
			CXUI_FingerprintReceived(&stFingerprint, HANDLE_NULL);
		}
	}

	LEAVE_FUNCTION;
	return ERR_OK;
#else
	HxLOG_Print("[NOT SUPPORT]\r\n");

	return ERR_FAIL;
#endif
}

#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
void CXUI_RetryFingerprint()
{
	ENTER_FUNCTION;

	CXUI_FingerprintReceived(&s_stFingerprint, HANDLE_NULL);

	LEAVE_FUNCTION;
}
#endif

HERROR CXUI_FingerprintReceived(CxFingerprint_t* pstFingerprint, Handle_t hSession)
{
#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
	CxFingerprint_t* 	pstCopy;
//	HUINT32				ulCurrentTime	= 0;
//	HxDATETIME_t		stCurTime;
//	VK_CLOCK_SET_MODE	eClockSetMode	= 0;
//	HERROR				nErr;

	ENTER_FUNCTION;

	//## 던져 주면 app쪽에서 알아서 표시

	// time = 0xffffff 이면 바로 표시할 수도 있고, 아니면 해당 시간에 표시 (단 24시간 동안만 유효하다는 것에 주의)
	//## 다른 메시지들과는 별개로 처리되어야 하는데 어떤 식으로 해야할까 ???
	// pstFingerprint는 caller쪽에서 free해버릴 것이라 copy해서 사용.
#if 0
	// Get Current Time
	if (CXUTIL_GetTime(&ulCurrentTime) != ERR_OK)
	{
		HxLOG_Error("");
		return	ERR_FAIL;
	}

	nErr = VK_CLOCK_GetTimeSource(&eClockSetMode);
	if (nErr != ERR_OK)
	{
		HxLOG_Print(" VK_CLOCK_GetTimeSource () Fail !!\n");
		return ERR_FAIL;
	}

	HLIB_DATETIME_ConvertUnixTimeToDateTime(ulCurrentTime, &stCurTime);

	HxLOG_Print("<<FINGER:ulCurrentTime=%d, (%d/%02d/%02d), (%d/%d/%d)>>\n", ulCurrentTime, stCurTime.stDate.usYear, stCurTime.stDate.ucMonth, stCurTime.stDate.ucDay, stCurTime.stTime.ucHour, stCurTime.stTime.ucMinute, stCurTime.stTime.ucSecond);

	if ((stCurTime.stDate.usYear < 2000) /*&& (pstFingerprint->ulTime != CX_FINGER_PRINT_IMMEDIATELY_TIME)*/)
	{
		CX_memcpy(&s_stFingerprint, pstFingerprint, sizeof(CxFingerprint_t));

		if (s_ulCxFingerprintRetryTimerId == 0)
		{
			HxLOG_Print("Current time may be wrong. try again after %d sec.\r\n", CX_FINGERPRINT_RETRY_TIME);
			CX_TIMER_EventAfter(CX_FINGERPRINT_RETRY_TIME, local_cxui_TimerHandler, NULL, 0, &s_ulCxFingerprintRetryTimerId);
		}
	}
	else
#endif
	{
		pstCopy = (CxFingerprint_t *)CX_MEM_Alloc(sizeof(CxFingerprint_t));
		if (pstCopy == NULL)
		{
			return ERR_FAIL;
		}

		CX_memcpy(pstCopy, pstFingerprint, sizeof(CxFingerprint_t));
#if defined(CONFIG_DEBUG)
		CXUI_DEBUG_PrintData(eCxUi_GroupOther_Fingerprint, (HUINT32)pstCopy);
#endif
		local_cxui_Display(hSession, eCxUi_GroupOther, eCxUi_GroupOther_Fingerprint, (HUINT32)pstCopy);
	}

	LEAVE_FUNCTION;
	return ERR_OK;
#else
	HxLOG_Print("[NOT SUPPORT]\r\n");

	return ERR_FAIL;
#endif
}

#if defined(CONFIG_MW_MM_PVR)
HERROR CXUI_DisplayMaturityRate(Handle_t hSession, CxMaturityRatingValue_t *pstMaturityRate, HBOOL bScPinPass)
{
	Handle_t hMainSession = HANDLE_NULL;
	HBOOL bCaPinPass = FALSE;

	if(bScPinPass == TRUE)
	{
		return local_cxui_Display(hSession, eCxUi_GroupMr, eCxUi_GroupAll_None, 0);
	}

	if (s_astCxUiLastMsg[eCxUi_GroupSc] != eCxUi_GroupAll_None)
	{
		return local_cxui_Display(hSession, eCxUi_GroupSc, s_astCxUiLastMsg[eCxUi_GroupSc], 0);
	}

	s_stCxUiMrValue.ucProgram = pstMaturityRate->ucProgram;
	s_stCxUiMrValue.ucSmartcard = CXSC_GetMaturityRatingLevel(CX_SC_SLOT_0);

	if(s_stCxUiMrValue.ucProgram <= s_stCxUiMrValue.ucSmartcard)
	{
		return local_cxui_Display(hSession, eCxUi_GroupMr, eCxUi_GroupAll_None, 0);
	}

	switch(CXSESSION_GetActionType(hSession))
	{
		case eCAS_ActionType_Playback:
		case eCAS_ActionType_Tsr_Play:
			bCaPinPass = CXSESSION_GetCaPinPass(hSession);
			break;

		default:
			return ERR_OK;
	}

	if(bCaPinPass)
	{
		return local_cxui_Display(hSession, eCxUi_GroupMr, eCxUi_GroupAll_None, 0);
	}

	return local_cxui_Display(hSession, eCxUi_GroupMr, eCxUi_GroupMr_Ask, (HUINT32)&s_stCxUiMrValue);
}
#endif

#if defined(USE_CONAX_EMULATOR)
// do not
HERROR CXUI_Trick_FingerprintReceived(void)
{
	return ERR_OK;
}

HERROR CXUI_Trick_UserTextReceived(void)
{
	return ERR_OK;
}
#else
HERROR CXAPI_UiFingerprintDisplayed(void)
{
#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
	HERROR	nErr;

	ENTER_FUNCTION;

	nErr = CXNVM_FingerprintDisplayed();

	LEAVE_FUNCTION;

	return nErr;
#else
	HxLOG_Print("[NOT SUPPORT]\r\n"));

	return ERR_FAIL;
#endif
}

HERROR CXUI_Trick_FingerprintReceived(void)
{
#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
	if (CXSESSION_GetSessionCnt() == 0)
	{
		if (s_ulCxFingerprintRetryTrickCount > CX_FINGERPRINT_RETRY_TRICK_COUNT)
		{
			HxLOG_Print("[ERROR] Session is not allocated.\r\n");
			return ERR_FAIL;
		}

		if (s_ulCxFingerprintRetryTrickTimerId == 0)
		{
			s_ulCxFingerprintRetryTrickCount++;
			HxLOG_Print("Current time may be wrong. try again after %d ms.\r\n", CX_FINGERPRINT_RETRY_TRICK_TIME);
			CX_TIMER_EventAfter(CX_FINGERPRINT_RETRY_TRICK_TIME, local_cxui_TimerHandler, NULL, 0, &s_ulCxFingerprintRetryTrickTimerId);
		}
	}
	else
	{
		s_ulCxFingerprintRetryTrickCount = 0;
		CXUI_CheckFingerprintAvailable();
	}
#endif
	return ERR_OK;
}

/*
	Active Standby 일 경우 동작
	Session count 를 확인하고 Flash read동작.
*/
HERROR CXUI_Trick_UserTextReceived(void)
{
#if defined(CONFIG_MW_CAS_CONAX_TEXT)
	if (CXSESSION_GetSessionCnt() == 0)
	{
		if (s_ulCxUserTextRetryTrickCount > CX_USER_TEXT_RETRY_TRICK_COUNT)
		{
			HxLOG_Print("[ERROR] Session is not allocated.\r\n");
			return ERR_FAIL;
		}

		if (s_ulCxUserTextRetryTrickTimerId == 0)
		{
			s_ulCxUserTextRetryTrickCount++;
			HxLOG_Print("Current time may be wrong. try again after %d ms.\r\n", CX_USER_TEXT_RETRY_TRICK_TIME);
			CX_TIMER_EventAfter(CX_USER_TEXT_RETRY_TRICK_TIME, local_cxui_TimerHandler, NULL, 0, &s_ulCxUserTextRetryTrickTimerId);
		}
	}
	else
	{
		s_ulCxUserTextRetryTrickCount = 0;
		CXUI_CheckUserTextAvailable();
	}
#endif
	return ERR_OK;
}
#endif

#define Basic_Descrambling__________________________________________________

HERROR CXUI_DisplayAccessStatus(Handle_t hEcm, CxAccessStatus_t* pAccess, HUINT32 ulParam)
{
	register int	i;
	Handle_t		ahSession[CX_MAX_NUM_OF_SESSION];
	HUINT32			ulCount;
	HINT32			nRet;
	HINT32			nErrCode	= 0;
	CxUiMsgType_t	eUiMsgType;

	HxLOG_Print("[Enter][%02X %02X]\r\n", pAccess->ucStatus1, pAccess->ucStatus2);

	// 어떤 session에 관한 것인지는 hEcm으로 추적해야..
	ulCount = CXSTREAM_GetAllSessionByEcm(hEcm, ahSession);

	if (pAccess->ucStatus1 & 0x40)
	{
		// bAccessGranted, teaser나 insufficient token이 같이 올 수 있음
		// AccessGranted면서도 ulParam(cpm code)가 있을 경우는 ERROR 이다.
		eUiMsgType = pAccess->ucExt3 ? eCxUi_GroupBasic_NoAccess : eCxUi_GroupAll_None;
		for (i = 0; i < ulCount; i++)
		{
			local_cxui_Display(ahSession[i], eCxUi_GroupBasic, eUiMsgType, pAccess->ucExt3);
		}
	}
	else if (pAccess->ucStatus1 & 0x80)
	{
		// bNotSufficientInfo : do nothing
	}
	else if (pAccess->ucStatus1 & 0x20)
	{
		//## bOrderAccessTicket : ORDER_INFO가 올 것이다.. 그걸 처리해줍시다. ppv 사세요~
	}
	else if (pAccess->ucStatus1 & 0x8)
	{
		// bInsufficientTokens : 토큰이 모자랍니다. 아마 TOKEN_DEBIT_INFO가 올 것이어요. 거기서 처리해주죠?
		// CXUI_ProcessTokenDebitInfo() 참고
	}
	else if (pAccess->ucStatus1 & 0x4)
	{
		// bGeographicalBlackout : no access (blackout) 표시
		for (i = 0; i < ulCount; i++)
		{
			local_cxui_Display(ahSession[i], eCxUi_GroupBasic, eCxUi_GroupBasic_NoAccessBlackOut, 0);
		}
	}
	else if (pAccess->ucStatus1 & 0x2)
	{
		// bMaturityRating : mr 창 표시, pin 입력 받아서 ca_pin_in (0x1) 호출
		// cur_mat_level = pAccess->ucExt1;
		for (i = 0; i < ulCount; i++)
		{
			nRet = CXSC_GetMaturityRatingLevel(CXSESSION_GetScSlot(ahSession[i]));
			if (nRet == CX_ERR_INVALID_PARAMETER)
			{
				return ERR_FAIL;
			}

			s_stCxUiMrValue.ucProgram = pAccess->ucExt1;
			s_stCxUiMrValue.ucSmartcard = (HUINT8)nRet;
			local_cxui_Display(ahSession[i], eCxUi_GroupMr, eCxUi_GroupMr_Ask, (HUINT32)&s_stCxUiMrValue);

			CXSESSION_SetCaPinPass(ahSession[i], FALSE);
		}
	}
	else if (pAccess->ucStatus1 & 0x1)
	{
		// bAcceptViewing : ACCEPT_VIEWING_INFO 가 올거 같아요. 그거보고 user한테 볼건지 물어봅시다
		//CXUI_ProcessAcceptViewingInfo() 참고
	}

	if (pAccess->ucStatus2 & 0x80)
	{
		// bTeaser : preview 같은건데 특별히 해줄건 없어요..
	}
	else if (pAccess->ucStatus2 & 0x40)
	{
		//## bPpvExpired : ppv expired되서 못보여줘요..
	}
	else if (pAccess->ucStatus2 & 0x20)
	{
		//## bNoAccessToNetwork : no access (network) 표시
		for (i = 0; i < ulCount; i++)
		{
			local_cxui_Display(ahSession[i], eCxUi_GroupBasic, eCxUi_GroupBasic_NoAccessNetwork, 0);
		}
	}

	if (pAccess->ucStatus1 == 0 && pAccess->ucStatus2 == 0)
	{
		if(pAccess->ucExt4 == 0)
		{
#if defined(CONFIG_MW_CAS_CONAX_CPM)
			nErrCode = CXSC_CheckErrorByAccessDenied();
#endif
			for (i = 0; i < ulCount; i++)
			{
				local_cxui_Display(ahSession[i], eCxUi_GroupBasic, eCxUi_GroupBasic_NoAccess, nErrCode);
			}
		}
	}

	LEAVE_FUNCTION;

	return ERR_OK;
}

#if defined(CONFIG_MW_CAS_CONAX_CPM)
HERROR CXUI_DisplayPairingError(Handle_t hSession, HINT32 nErrCode)
{
	register int	i;
	Handle_t		ahSession[CX_MAX_NUM_OF_SESSION];
	HUINT32			ulCount;
	HINT32			nSlot;

	ENTER_FUNCTION;


	nSlot = CXSESSION_GetScSlot(hSession);
	ulCount = CXSESSION_GetAllSessionByScSlot(nSlot, ahSession);

	for (i = 0; i < ulCount; i++)
	{
		local_cxui_Display(ahSession[i], eCxUi_GroupBasic, eCxUi_GroupBasic_NoAccess, nErrCode);
	}

	//CXUI_UpdateDescrambleStatus(CXSESSION_GetStbHandle(hSession), FALSE);

	LEAVE_FUNCTION;

	return ERR_OK;
}
#endif

#define MaturityRating__________________________________________________
#if defined(USE_CONAX_EMULATOR)
// do not
#else
// app는 ui를 닫고 이걸 호출하면, 여기서 적절한 error msg 등을 다시 보내줄거다.
// hStbHandle은 정확히 줘야 한다.. pin이 맞으면 ECM을 다시 받기 위해서 바로 filter를 restart할 것이기 때문에..
HERROR CXAPI_UiAcceptMaturityRatingOverride(Handle_t hStbHandle, HUINT8* pucPin)
//HERROR CXUI_AcceptMaturityRatingOverride(Handle_t hSession, HUINT8 *pucPin)
{
	HERROR		nErr;
	Handle_t	hSession;

	ENTER_FUNCTION;

	if (pucPin == NULL)
	{
		return ERR_FAIL;
	}

	hSession = CXSESSION_GetHandleByStbHandle(hStbHandle);

	nErr = local_cxui_EnterCaPin(hSession, CXSESSION_GetScSlot(hSession), eCxPinCheck_MatRatOverride, pucPin);

	if (nErr == ERR_FAIL)
	{
		local_cxui_Display(hSession, eCxUi_GroupMr, eCxUi_GroupMr_WrongPin, 0);
		return ERR_FAIL;
	}

	if (nErr == CX_ERR_SC_WRONG_PIN)
	{
		// wrong pin
		local_cxui_Display(hSession, eCxUi_GroupMr, eCxUi_GroupMr_WrongPin, 0);
		return ERR_FAIL;
	}
	else if (nErr == CX_ERR_SC_PIN_LOCKED)
	{
		// 너무 많이 틀렸어..
		local_cxui_Display(hSession, eCxUi_GroupMr, eCxUi_GroupMr_PinLocked, 0);
		return ERR_FAIL;
	}

	// 성공~
	local_cxui_Display(hSession, eCxUi_GroupMr, eCxUi_GroupAll_None, 0);

#if defined(CONFIG_MW_MM_PVR)
	local_cxui_EnterCaPinToSameSession(hSession, pucPin, eCxPinCheck_MatRatOverride);
#endif

	LEAVE_FUNCTION;
	return ERR_OK;
}
#endif	// End of defined(USE_CONAX_EMULATOR)



#define PPV__________________________________________________

HERROR CXUI_DisplayOrdering(Handle_t hSession)
{
	//## 여기서 ordering 과정을 표시한다.
	// s/c에서 ORDER_REQ_APDU를 받으면 즉시 connection 시도하고.. 성공하면 nothing, 실패하면 order failed
	// order를 보내는데, 성공하면 nothing, 실패하면 order failed
	// order 접수되었다는 응답을 받으면, order accepted, 실패하면 order failed
	//	local_cxui_Display(hSession, eCxUi_GroupPpv, eCxUi_GroupPpv_OrderAccepted, NULL);
	ENTER_FUNCTION;

	LEAVE_FUNCTION;
	return ERR_OK;
}

// app는 ui를 닫고 이걸 호출하면, 여기서 적절한 error msg 등을 다시 보내줄거다.
// return path가 없으면 이걸 부를 일은 없을거다
HERROR CXAPI_UiStartOrderingPPV(Handle_t hStbHandle, HUINT8* pucPin, HUINT32 ulProdId)
{
	HERROR		nErr;
	Handle_t	hSession;

	ENTER_FUNCTION;

	if (pucPin == NULL)
	{
		return ERR_FAIL;
	}

	hSession = CXSESSION_GetHandleByStbHandle(hStbHandle);

	nErr = CXSC_RequestOrderApdu(hSession, pucPin, ulProdId);

	if (nErr == CX_ERR_SC_WRONG_PIN)
	{
		// wrong pin
		local_cxui_Display(hSession, eCxUi_GroupPpv, eCxUi_GroupMr_WrongPin, 0);
		return ERR_FAIL;
	}
	else if (nErr == CX_ERR_SC_PIN_LOCKED)
	{
		// 너무 많이 틀렸어..
		local_cxui_Display(hSession, eCxUi_GroupPpv, eCxUi_GroupMr_PinLocked, 0);
		return ERR_FAIL;
	}

	//## 성공이라면 다른
	local_cxui_Display(hSession, eCxUi_GroupPpv, eCxUi_GroupAll_None, 0);

	LEAVE_FUNCTION;
	return ERR_OK;
}


#if defined(USE_CONAX_EMULATOR)
// do not
#else
// app는 ui를 닫고 이걸 호출하면, 여기서 적절한 error msg 등을 다시 보내줄거다.
// hStbHandle은 정확히 줘야 한다.. pin이 맞으면 ECM을 다시 받기 위해서 바로 filter를 restart할 것이기 때문에..
HERROR CXAPI_UiStartViewingPPV(Handle_t hStbHandle, HUINT8* pucPin)
//HERROR CXUI_StartViewing(Handle_t hSession, HUINT8 *pucPin)
{
	HERROR		nErr;
	Handle_t	hSession;

	ENTER_FUNCTION;

	if (pucPin == NULL)
	{
		return ERR_FAIL;
	}

	hSession = CXSESSION_GetHandleByStbHandle(hStbHandle);

	nErr = local_cxui_EnterCaPin(hSession, CXSESSION_GetScSlot(hSession), eCxPinCheck_StartViewing, pucPin);

	if (nErr == CX_ERR_SC_WRONG_PIN)
	{
		// wrong pin
		local_cxui_Display(hSession, eCxUi_GroupPpv, eCxUi_GroupMr_WrongPin, 0);
		return ERR_FAIL;
	}
	else if (nErr == CX_ERR_SC_PIN_LOCKED)
	{
		// 너무 많이 틀렸어..
		local_cxui_Display(hSession, eCxUi_GroupPpv, eCxUi_GroupMr_PinLocked, 0);
		return ERR_FAIL;
	}

	// 성공~
	local_cxui_Display(hSession, eCxUi_GroupPpv, eCxUi_GroupAll_None, 0);

#if defined(CONFIG_MW_MM_PVR)
	local_cxui_EnterCaPinToSameSession(hSession, pucPin, eCxPinCheck_StartViewing);
#endif

	LEAVE_FUNCTION;

	//## ecm 다시 보내야 하나? (아예 local_cxui_EnterCaPin()에서 처리해버릴까?)
	return ERR_OK;
}
#endif	// End of defined(USE_CONAX_EMULATOR)


#if defined(USE_CONAX_EMULATOR)
// do not
#else
// app는 ui를 닫고 이걸 호출하면, 여기서 적절한 error msg 등을 다시 보내줄거다.
// hStbHandle은 정확히 줘야 한다.. pin이 맞으면 ECM을 다시 받기 위해서 바로 filter를 restart할 것이기 때문에..
HERROR CXAPI_UiStartViewingTokenPPV(Handle_t hStbHandle, HUINT8* pucPin)
//HERROR CXUI_DebitToken(Handle_t hSession, HUINT8 *pucPin)
{
	HERROR		nErr;
	Handle_t	hSession;

	ENTER_FUNCTION;

	if (pucPin == NULL)
	{
		return ERR_FAIL;
	}

	hSession = CXSESSION_GetHandleByStbHandle(hStbHandle);

	nErr = local_cxui_EnterCaPin(hSession, CXSESSION_GetScSlot(hSession), eCxPinCheck_DebitToken, pucPin);

	//DDI_UART_Print("(EnterCaPin:%x,%c%c%c%c=%x)\n", hSession, pucPin[0], pucPin[1], pucPin[2], pucPin[3], nErr);

	if (nErr == CX_ERR_SC_WRONG_PIN)
	{
		// wrong pin
		local_cxui_Display(hSession, eCxUi_GroupPpv, eCxUi_GroupMr_WrongPin, 0);
		return ERR_FAIL;
	}
	else if (nErr == CX_ERR_SC_PIN_LOCKED)
	{
		// 너무 많이 틀렸어..
		local_cxui_Display(hSession, eCxUi_GroupPpv, eCxUi_GroupMr_PinLocked, 0);
		return ERR_FAIL;
	}

	// 성공~
	local_cxui_Display(hSession, eCxUi_GroupPpv, eCxUi_GroupAll_None, 0);

#if defined(CONFIG_MW_MM_PVR)
	local_cxui_EnterCaPinToSameSession(hSession, pucPin, eCxPinCheck_DebitToken);
#endif

	LEAVE_FUNCTION;

	//## ecm 다시 보내야 하나? (아예 local_cxui_EnterCaPin()에서 처리해버릴까?)
	return ERR_OK;
}
#endif	// End of defined(USE_CONAX_EMULATOR)

/*
PPV
	> Ordering / 방송 - 가격/ enter pin .. (page 34)					-- ORDER_INFO가 오면 즉시 표시해야 함 (modem이 없다면 pin 묻지 말자)
	> Start viewing / 방송 - 남은 시간 / enter pin.. (page35)			-- ACCEPT_VIEWING_INFO가 오면 표시
*/

HERROR CXUI_ProcessOrderInfo(Handle_t hEcm, CxOrderInfo_t* pInfo)
{
	register int	i;
	Handle_t		ahSession[CX_MAX_NUM_OF_SESSION];
	HUINT32			ulCount;

	ENTER_FUNCTION;

	CX_memcpy(&s_stCxUiOrder, pInfo, sizeof(CxOrderInfo_t));

	// 어떤 session에 관한 것인지는 hEcm으로 추적해야..
	ulCount = CXSTREAM_GetAllSessionByEcm(hEcm, ahSession);

	// 화면에 보여주세요. - return path가 없으면 그냥 표시하고 말고, 있으면 ca pin 입력 받아서 order process를 진행해야 한다.
	for (i = 0; i < ulCount; i++)
	{
		local_cxui_Display(ahSession[i], eCxUi_GroupPpv, eCxUi_GroupPpv_OrderInfo, (HUINT32)&s_stCxUiOrder);
	}

	LEAVE_FUNCTION;

	return ERR_OK;
}

HERROR CXUI_ProcessAcceptViewingInfo(Handle_t hEcm, CxAcceptViewingInfo_t* pInfo)
{
	register int	i;
	Handle_t		ahSession[CX_MAX_NUM_OF_SESSION];
	HUINT32			ulCount;

	ENTER_FUNCTION;

	CX_memcpy(&s_stCxUiAcceptViewing, pInfo, sizeof(CxAcceptViewingInfo_t));

	// 어떤 session에 관한 것인지는 hEcm으로 추적해야..
	ulCount = CXSTREAM_GetAllSessionByEcm(hEcm, ahSession);

	// 화면에 보여주세요. - ca pin 입력 받아서 ca_pin_in (0x2) 호출
	for (i = 0; i < ulCount; i++)
	{
		local_cxui_Display(ahSession[i], eCxUi_GroupPpv, eCxUi_GroupPpv_AcceptViewing, (HUINT32)&s_stCxUiAcceptViewing);
	}

	LEAVE_FUNCTION;

	return ERR_OK;
}

HERROR CXUI_ProcessTokenDebitInfo(Handle_t hEcm, CxTokenDebitInfo_t* pInfo)
{
	register int	i;
	Handle_t		ahSession[CX_MAX_NUM_OF_SESSION];
	HUINT32			ulCount;

	ENTER_FUNCTION;

	CX_memcpy(&s_stCxUiTokenDebit, pInfo, sizeof(CxTokenDebitInfo_t));

	// 어떤 session에 관한 것인지는 hEcm으로 추적해야..
	ulCount = CXSTREAM_GetAllSessionByEcm(hEcm, ahSession);

	// 화면에 보여주세요. - token이 없으면 그냥 없다고 표시. token 있으면  ca pin 입력 받아서 ca_pin_in (0x3) 호출

	for (i = 0; i < ulCount; i++)
	{
		local_cxui_Display(ahSession[i], eCxUi_GroupPpv, eCxUi_GroupPpv_TokenPpv, (HUINT32)&s_stCxUiTokenDebit);
	}

	LEAVE_FUNCTION;

	return ERR_OK;
}

CxStatusList_t* CXAPI_UiGetPurseBalance(Handle_t hStbHandle, HUINT32 ulRef)
{
	CxStatusList_t* pstList;
	Handle_t		hSession;

	ENTER_FUNCTION;

	hSession = CXSESSION_GetHandleByStbHandle(hStbHandle);
	pstList = CXSC_GetCaStatus(CXSESSION_GetScSlot(hSession), eCxCaStatus_Purse, ulRef);

	//DDI_UART_Print("CXAPI_GetPurseBalance(%x,%x,%x)\n", hStbHandle, hSession, pstList);

	LEAVE_FUNCTION;
	return pstList;
}
void CXUI_SetRecvUTTimeStamp(void)
{
	local_cas_cx_SetRecvUTTimeStamp();
}
void CXUI_SetUpdateSITimeStamp(void)
{
	local_cas_cx_SetUpdateSITimeStamp();
}

HERROR CXUI_SetRepeatImmediateMsgTime(CxUserText_t* pstText, UNIXTIME* pulUnixtime)
{
	HERROR		hErr;
	UNIXTIME	ulUnixtime	= 0;

	hErr = local_cxui_SetRepeatImmediateMsgTime(pstText, &ulUnixtime);

	*pulUnixtime = ulUnixtime;
	return hErr;
}

HERROR CXAPI_SetScPinForTokenPPV(Handle_t hStbHandle, HUINT8* pucPin, CxUiMsgType_t* eReturnMsg)
{
	HERROR		nErr;
	Handle_t	hSession;

	ENTER_FUNCTION;

	if (pucPin == NULL)
	{
		return ERR_FAIL;
	}

	hSession = CXSESSION_GetHandleByStbHandle(hStbHandle);

	nErr = local_cxui_EnterCaPin(hSession, CXSESSION_GetScSlot(hSession), eCxPinCheck_DebitToken, pucPin);

	if (nErr == CX_ERR_SC_WRONG_PIN)
	{
		// wrong pin
		*eReturnMsg = eCxUi_GroupMr_WrongPin;
		// local_cxui_Display(hSession, eCxUi_GroupPpv, eCxUi_GroupMr_WrongPin, 0);
		return ERR_FAIL;
	}
	else if (nErr == CX_ERR_SC_PIN_LOCKED)
	{
		// 너무 많이 틀렸어..
		*eReturnMsg = eCxUi_GroupMr_PinLocked;
		// local_cxui_Display(hSession, eCxUi_GroupPpv, eCxUi_GroupMr_PinLocked, 0);
		return ERR_FAIL;
	}

	// 성공~
	*eReturnMsg = eCxUi_GroupAll_None;
	// local_cxui_Display(hSession, eCxUi_GroupPpv, eCxUi_GroupAll_None, 0);

#if defined(CONFIG_MW_MM_PVR)
	local_cxui_EnterCaPinToSameSession(hSession, pucPin, eCxPinCheck_StartViewing);
#endif

	LEAVE_FUNCTION;

	//## ecm 다시 보내야 하나? (아예 local_cxui_EnterCaPin()에서 처리해버릴까?)
	return ERR_OK;
}

HERROR CXAPI_SetScPin(Handle_t hAction, HUINT8* pucPin, CxUiMsgType_t eMsgType, CxUiMsgType_t* eReturnMsg)
{
	HERROR		nErr;
	Handle_t	hStbHandle, hSession;
	HINT32		nScSlot;
	CxPinCheckType_t ePinUse;
	SvcCas_ActionType_e eActionType;
#if defined(CONFIG_MW_MM_PVR)
	CxMaturityRatingValue_t stMaturityRate;
#endif

	ENTER_FUNCTION;

	if (pucPin == NULL || eReturnMsg == NULL)
	{
		return ERR_FAIL;
	}

	eActionType = CXACT_GetActionTypeFromActionHandle(hAction);

	switch(eMsgType)
	{
		case eCxUi_GroupPpv_TokenPpv:
			ePinUse = eCxPinCheck_DebitToken;
			break;

		case eCxUi_GroupPpv_AcceptViewing:
			ePinUse = eCxPinCheck_StartViewing;
			break;

		case eCxUi_GroupMr_Ask:
			switch(eActionType)
			{
#if defined(CONFIG_MW_MM_PVR)
				case eCAS_ActionType_Playback:
				case eCAS_ActionType_Tsr_Play:
					ePinUse = eCxPinCheck_General;
					break;
#endif
				default:
					ePinUse = eCxPinCheck_MatRatOverride;
					break;
			}
			break;

		case eCxUi_GroupOther_CachedPin:
			ePinUse = eCxPinCheck_General;
			break;

		default:
			ePinUse = eCxPinCheck_General;
			break;
	}

	if(ePinUse == eCxPinCheck_General)
	{
		// If pin is used for general propose, Action is not used... ex)cached pin progress...
		hSession = HANDLE_NULL;
		nScSlot = 0;
	}
	else
	{
		if(hAction == HANDLE_NULL)
		{
			hSession = CXSESSION_GetMainHandle();
		}
		else
		{
			hStbHandle = CXACT_GetStbHandleFromActionHandle(hAction);
			if(hStbHandle == HANDLE_NULL)
			{
				HxLOG_Error("hStbHandle HANDLE_NULL\n");
				return ERR_FAIL;
			}

			hSession = CXSESSION_GetHandleByStbHandle(hStbHandle);
		}

		if(hSession == HANDLE_NULL)
		{
			HxLOG_Error("hSession HANDLE_NULL\n");
			return ERR_FAIL;
		}

		nScSlot = CXSESSION_GetScSlot(hSession);
	}

	nErr = local_cxui_EnterCaPin(hSession, nScSlot, ePinUse, pucPin);

	if (nErr == CX_ERR_SC_WRONG_PIN || nErr == ERR_FAIL)
	{
		// wrong pin
		*eReturnMsg = eCxUi_GroupMr_WrongPin;
		nErr = ERR_FAIL;
	}
	else if (nErr == CX_ERR_SC_PIN_LOCKED)
	{
		// 너무 많이 틀렸어..
		*eReturnMsg = eCxUi_GroupMr_PinLocked;
		nErr = ERR_FAIL;
	}
	else
	{
		*eReturnMsg = eCxUi_GroupAll_None;
		nErr = ERR_OK;
	}

	switch(eActionType)
	{
#if defined(CONFIG_MW_MM_PVR)
		case eCAS_ActionType_Playback:
		case eCAS_ActionType_Tsr_Play:
			hStbHandle = CXACT_GetStbHandleFromActionHandle(hAction);
			if(hStbHandle == HANDLE_NULL)
			{
				HxLOG_Error("hStbHandle HANDLE_NULL\n");
				return ERR_FAIL;
			}

			hSession = CXSESSION_GetHandleByStbHandle(hStbHandle);
			if(hSession == HANDLE_NULL)
			{
				HxLOG_Error("hSession HANDLE_NULL\n");
				return ERR_FAIL;
			}

			if(nErr == ERR_OK)
			{
				CXSESSION_SetCaPinPass(hSession, TRUE);
				CXPVR_SetMaturityRate(hAction, &stMaturityRate);
				CXUI_DisplayMaturityRate(hSession, &stMaturityRate, TRUE);
			}
			break;
#endif
		default:
			switch(eMsgType)
			{
				case eCxUi_GroupPpv_TokenPpv:
				case eCxUi_GroupPpv_AcceptViewing:
				case eCxUi_GroupMr_Ask:
					if(nErr == ERR_OK)
					{
						CXSESSION_SetCaPinPass(hSession, TRUE);
					}
					break;

				default:
					break;
			}
			break;
	}

	LEAVE_FUNCTION;

	//## ecm 다시 보내야 하나? (아예 local_cxui_EnterCaPin()에서 처리해버릴까?)
	return nErr;
}


#define ReturnChannel__________________________________________________

/*
Return channel
	> Connecting, please wait...
	> Busy, please try again
	> Order accepted
	> Order failed / Error code: xx
*/

#define Debugging__________________________________________________

/*
for debugging
*/
HUINT8* CXUI_DEBUG_GetMsgTypeStr(CxUiMsgType_t eMsg)
{
	HUINT8* pszName;

#if defined(CONFIG_DEBUG)
	switch (eMsg)
	{
	case eCxUi_GroupAll_None:
		pszName = "eCxUi_GroupAll_None";break;
	case eCxUi_GroupBasic_NoAccess:
		pszName = "eCxUi_GroupBasic_NoAccess";break;
	case eCxUi_GroupBasic_NoAccessBlackOut:
		pszName = "eCxUi_GroupBasic_NoAccessBlackOut";break;
	case eCxUi_GroupBasic_NoAccessNetwork:
		pszName = "eCxUi_GroupBasic_NoAccessNetwork";break;
	case eCxUi_GroupBasic_NoAccessPullEmm:
		pszName = "eCxUi_GroupBasic_NoAccessPullEmm";break;
	case eCxUi_GroupMessage_Display:
		pszName = "eCxUi_GroupMessage_Display";break;
	case eCxUi_GroupPpv_OrderInfo:
		pszName = "eCxUi_GroupPpv_OrderInfo";break;
	case eCxUi_GroupPpv_OrderConnect:
		pszName = "eCxUi_GroupPpv_OrderConnect";break;
	case eCxUi_GroupPpv_OrderFailed:
		pszName = "eCxUi_GroupPpv_OrderFailed";break;
	case eCxUi_GroupPpv_OrderAccepted:
		pszName = "eCxUi_GroupPpv_OrderAccepted";break;
	case eCxUi_GroupPpv_OrderNotAccepted:
		pszName = "eCxUi_GroupPpv_OrderNotAccepted";break;
	case eCxUi_GroupPpv_OrderClosed:
		pszName = "eCxUi_GroupPpv_OrderClosed";break;
	case eCxUi_GroupPpv_AcceptViewing:
		pszName = "eCxUi_GroupPpv_AcceptViewing";break;
	case eCxUi_GroupPpv_TokenPpv:
		pszName = "eCxUi_GroupPpv_TokenPpv";break;
	case eCxUi_GroupMr_Ask:
		pszName = "eCxUi_GroupMr_Ask";break;
	case eCxUi_GroupMr_WrongPin:
		pszName = "eCxUi_GroupMr_WrongPin";break;
	case eCxUi_GroupMr_PinLocked:
		pszName = "eCxUi_GroupMr_PinLocked";break;
	case eCxUi_GroupMenu_Subscription:
		pszName = "eCxUi_GroupMenu_Subscription";break;
	case eCxUi_GroupMenu_Event:
		pszName = "eCxUi_GroupMenu_Event";break;
	case eCxUi_GroupMenu_Purse:
		pszName = "eCxUi_GroupMenu_Purse";break;
	case eCxUi_GroupMenu_Debit:
		pszName = "eCxUi_GroupMenu_Debit";break;
	case eCxUi_GroupMenu_Credit:
		pszName = "eCxUi_GroupMenu_Credit";break;
	case eCxUi_GroupMenu_ChangePin:
		pszName = "eCxUi_GroupMenu_ChangePin";break;
	case eCxUi_GroupMenu_CheckNewPin:
		pszName = "eCxUi_GroupMenu_CheckNewPin";break;
	case eCxUi_GroupMenu_WrongPin:
		pszName = "eCxUi_GroupMenu_WrongPin";break;
	case eCxUi_GroupMenu_PinLocked:
		pszName = "eCxUi_GroupMenu_PinLocked";break;
	case eCxUi_GroupMenu_Mr:
		pszName = "eCxUi_GroupMenu_Mr";break;
	case eCxUi_GroupMenu_Modem:
		pszName = "eCxUi_GroupMenu_Modem";break;
	case eCxUi_GroupMenu_Message:
		pszName = "eCxUi_GroupMenu_Message";break;
	case eCxUi_GroupMenu_Info:
		pszName = "eCxUi_GroupMenu_Info";break;
	case eCxUi_GroupSc_NoCard:
		pszName = "eCxUi_GroupSc_NoCard";break;
	case eCxUi_GroupSc_Inserted:
		pszName = "eCxUi_GroupSc_Inserted";break;
	case eCxUi_GroupSc_Incorrect:
		pszName = "eCxUi_GroupSc_Incorrect";break;
	case eCxUi_GroupSc_IncorrectConax:
		pszName = "eCxUi_GroupSc_IncorrectConax";break;
	case eCxUi_GroupSc_CommFail:
		pszName = "eCxUi_GroupSc_CommFail";break;
	case eCxUi_GroupOther_Fingerprint:
		pszName = "eCxUi_GroupOther_Fingerprint";break;
		//	case eCxUi_GroupOther_MessageReceived:			pszName = "eCxUi_GroupOther_MessageReceived";break;
	default:
		pszName = "UNKNOWN";break;
	}

#else
	pszName = "UNKNOWN";
#endif
	return pszName;
}


#if defined(CONFIG_DEBUG)
void CXUI_DEBUG_PrintData(CxUiMsgType_t eMsg, HUINT32 ulParam)
{
	HxLOG_Print("============== Debug Print ====================\n");
	HxLOG_Print("\t-->UI<0x%x, %s>\n", eMsg, CXUI_DEBUG_GetMsgTypeStr(eMsg));
	switch (eMsg)
	{
#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
	case eCxUi_GroupOther_Fingerprint:
		{
			CxFingerprint_t* 								stCxUiFingerPrint;

			stCxUiFingerPrint = (CxFingerprint_t *)ulParam;

			HxLOG_Print("\t-->Position<x:%d, y:%d>\n", stCxUiFingerPrint->usPosX, stCxUiFingerPrint->usPosY);
			HxLOG_Print("\t-->Height <%d>\n", stCxUiFingerPrint->usHeight);
			HxLOG_Print("\t-->Time <0x%x>\n", stCxUiFingerPrint->ulTime);
			HxLOG_Print("\t-->Duration<%d>\n", stCxUiFingerPrint->usDuration);
			HxLOG_Print("\t-->Msg<%s>\n", stCxUiFingerPrint->szText);
		}
		break;
#endif

#if defined(CONFIG_MW_CAS_CONAX_TEXT)
	case eCxUi_GroupMessage_Display:
		{
			CxUserText_t* 	stCxUiUserText;

			stCxUiUserText = (CxUserText_t *)ulParam;

			HxLOG_Print("\t-->SqeNo<%d>\n", stCxUiUserText->ucSeqNo);
			HxLOG_Print("\t-->Repeat<%d>\n", stCxUiUserText->ucRepeat);
			HxLOG_Print("\t-->Interval<%d>\n", stCxUiUserText->usInterval);
			HxLOG_Print("\t-->Duration<%d>\n", stCxUiUserText->usDuration);
			HxLOG_Print("\t-->Start Year<%d> Month<%d> Day<%d> Hour<%d> Min<%d> Sec<%d>\n", stCxUiUserText->stStartDataTime.stDate.usYear, stCxUiUserText->stStartDataTime.stDate.ucMonth, stCxUiUserText->stStartDataTime.stDate.ucDay, stCxUiUserText->stStartDataTime.stTime.ucHour, stCxUiUserText->stStartDataTime.stTime.ucMinute, stCxUiUserText->stStartDataTime.stTime.ucSecond);
			HxLOG_Print("\t-->Text<%s>\n", stCxUiUserText->szText);
		}
		break;
#endif

	default:
		break;
	}

	HxLOG_Print("==================================\n");
}

#endif

#if 0
HERROR CXUI_UpdateUIForPlayback(Handle_t hAction, HBOOL bCardDisable, CxUiMsgInfo_t* pstCxUiMsgInfo)
{
	if (pstCxUiMsgInfo == NULL)
	{
		return ERR_FAIL;
	}

	if (bCardDisable == TRUE)
	{
		pstCxUiMsgInfo->eUiMsgType = eCxUi_GroupSc_NoCard;
		svc_cas_SvcPortUtilPostMsgToAp(eSEVT_CAS_FAIL, hAction, eDxCAS_GROUPID_CX, (HINT32)pstCxUiMsgInfo, eCasShowType_None);
	}
	else
	{
		svc_cas_SvcPortUtilPostMsgToAp(eSEVT_CAS_OK, hAction, eDxCAS_GROUPID_CX, (HINT32)pstCxUiMsgInfo, eCasShowType_Both);
	}
	return ERR_OK;
}
#endif

/*********************** End of File ******************************/

