/**
	@file     ap_cas_va_pincode_ui.c
	@brief    file_name & simple comment.

	Description: for CONAX Check Pin code UI \n
	Module: Application(Layer), UI(module), CAS(module), CONAX(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2009/07/30		modified					jhlee\n

	Copyright (c) 2009 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <osd_gfx.h>
//#include <menuui_coor.h>
//#include <popupui_coor.h>

//#include <ray_popupui_coordinate.h>
#include "va_ui_adapt.h"

#include <mgr_pg.h>
#include "../local_include/_xmgr_cas_va_util.h"
#include "../local_include/_xmgr_cas_va_ui.h"
#include "_xmgr_cas_va_pincode_ui_coordinate.h"
#include <xmgr_cas_res_str.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
#if defined(CONFIG_DEBUG)
HUINT32	g_CasVa_PinUi_debug_level = DBG_ASSERT | DBG_TRACE; /* DBG_TRACE, DBG_PRINT, DBG_ASSERT 만 사용! */
#endif


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#if defined(CONFIG_DEBUG)
#define ApCasVaPinUiPrint(level, message)	(((level) & g_CasVa_PinUi_debug_level) ? ((VK_Print("[AP:CAS_VA:PIN][%d][%s] ", __LINE__, __FUNCTION__)), (VK_Print message)) : 0)
#define ApCasVaPinUiError(level, message)	(((level) & g_CasVa_PinUi_debug_level) ? ((VK_Print("[AP:CAS_VA:PIN][%d][%s] ERROR, ", __LINE__, __FUNCTION__)), (VK_Print message)) : 0)
#else
#define	ApCasVaPinUiPrint(level, message)
#define	ApCasVaPinUiError(level, message)
#endif


/*******************************************************************/
/********************      typedef         *************************/
/*******************************************************************/



/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HINT32		s_CasVa_sc_PinDialogInst;
STATIC HINT32		s_stModifyPinInst;

STATIC BUS_Callback_t				s_parentApp;
STATIC eCasVA_PinChangeStep_InDlg	s_CasVaPinChangeStep_InDlg;
STATIC eCasVA_PinChangeError		s_CasVaPinChangeError;
STATIC HUINT8						s_szCasVaInputedOldPinCodeStr[5];

STATIC HUINT32			s_stNoticeDialogInst;		/**< Pin Change Suceess Notice Dialog Instans. */

STATIC HUINT8						s_AcceptPin[5];
STATIC HUINT8						s_szPincodeMessage[TEXT_SIZE128];
/*******************************************************************/
/********************      	Static Prototype	********************/
/*******************************************************************/

// UI (body msg) manage
STATIC HBOOL			local_cas_va_Pindlg_CheckChgPinAreNotMatch(HUINT32 *pstInstance);
STATIC HINT32			local_cas_va_Pindlg_CheckCurrNumber(HUINT32 *pstInstance, eCasVA_PinChangeStep_InDlg ePinChangeStep, HINT32 number);
STATIC BUS_Result_t		local_cas_va_Pindlg_CheckPincodeStep(HUINT32 *pstInstance, eCasVA_PinChangeStep_InDlg ePinChangeStep);
STATIC void				local_cas_va_Pindlg_CheckPincodeProcess(HUINT32 *pstInstance);
STATIC HUINT8*			local_cas_va_Pindlg_GetCurPincodeDlgMsg(void);
STATIC HUINT8*			local_cas_va_Pindlg_GetNewPincodeDlgMsg(void);
STATIC HUINT8*			local_cas_va_Pindlg_GetVerifyNewPincodeDlgMsg(void);
STATIC void				local_cas_va_PinDlg_DrawUpdate(HUINT32 *pstInstance, HBOOL bEnabled);

// notice dlg manage
STATIC void				local_cas_va_Pindlg_ShowModifyResult(eCasVA_PinChangeError eChgPinErr);

// Msg handling
STATIC void				local_cas_va_Pindlg_NotifyPinAuthorized(HUINT32 *pstInstance);
STATIC void				local_cas_va_Pindlg_NotifyPinDenied(HUINT32 *pstInstance);
STATIC BUS_Result_t 	local_cas_va_Pindlg_MsgGwmCreate(HUINT32 *pstInstance, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t		local_cas_va_Pindlg_MsgGwmKeyUp(HUINT32 *pstInstance, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t		local_cas_va_Pindlg_MsgGwmKeyDown(HUINT32 *pstInstance, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t		local_cas_va_Pindlg_Proc(HINT32 message, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t		local_cas_va_Pindlg_CreatePincode(HUINT32* pstInstance, eCasVA_PinChangeStep_InDlg ePinChangeStep);
STATIC BUS_Result_t		local_cas_va_Pindlg_Create_PincodeDlg(HUINT32* pstInstance, HUINT8 *pszTitle, HUINT8 *pszMsg);
STATIC BUS_Result_t		local_cas_va_Pindlg_MsgDlgClicked(HUINT32 *pstInstance, HINT32 p1, HINT32 p2, HINT32 p3);


STATIC void				local_cas_va_SetAcceptCAPincode (HUINT8 *szPin);
STATIC void				local_cas_va_GetAcceptCAPincode (HUINT8 *szPin);

/*******************************************************************/
/********************      	Static Function 	********************/
/*******************************************************************/

#define _________________UI__________________________________

STATIC HUINT32*	local_cas_va_Pindlg_GetInstance(void)
{
	//return &s_CasVa_sc_PinDialogInst;
//	return &s_stModifyPinInst; // just for build
}

STATIC HBOOL	local_cas_va_Pindlg_CheckChgPinAreNotMatch(HUINT32 *pstInstance)
{
#if 0
	if (MWC_UTIL_DvbStrncmp(pstInstance->szVerifyPinCodeStr, pstInstance->szNewPinCodeStr, 5-1))
	{
		return	TRUE;	// Not matched
	}
#endif
	return	FALSE;		// Madched
}


STATIC BUS_Result_t	local_cas_va_Pindlg_CheckPincodeStep(HUINT32 *pstInstance, eCasVA_PinChangeStep_InDlg ePinChangeStep)
{
	if (ePinChangeStep == eVA_PinChangeStep_InDlg_InputCurPincode)
	{
//		VK_memcpy32(s_szCasVaInputedOldPinCodeStr, pstInstance->szPinCodeStr, 5); //hjlee3, just compile

		// 2. Enter old PIN
		BUS_MGR_Destroy(NULL);
		local_cas_va_Pindlg_NotifyPinAuthorized(pstInstance);
	}
	else if (ePinChangeStep == eVA_PinChangeStep_InDlg_InputNewPincode)
	{
		// 3. Enter new PIN
		BUS_MGR_Destroy(NULL);
		local_cas_va_Pindlg_NotifyPinAuthorized(pstInstance);
	}
	else if (ePinChangeStep == eVA_PinChangeStep_InDlg_VerifyNewPincode)
	{
		// 4. Comfirm new PIN
//		BUS_MGR_Destroy(NULL);
		local_cas_va_Pindlg_CheckPincodeProcess(pstInstance);
	}
	else
	{
		ApCasVaPinUiError(DBG_ASSERT, ("Unknown step (%d)\n", ePinChangeStep));
	}

	return	MESSAGE_BREAK;
}


STATIC void	local_cas_va_Pindlg_CheckPincodeProcess(HUINT32 *pstInstance)
{
	HERROR		hError = ERR_FAIL;
	HINT32		nResult;

	ApCasVaPinUiPrint(DBG_PRINT|DBG_TRACE, ("Old Pincode : %s / New Pincode : \n", s_szCasVaInputedOldPinCodeStr));

	// 5. New Pin comparision
	if (local_cas_va_Pindlg_CheckChgPinAreNotMatch(pstInstance))	// Not match
	{
		s_CasVaPinChangeStep_InDlg = eVA_PinChangeStep_InDlg_InputCurPincode;
		s_CasVaPinChangeError = eVA_PinChangeError_ConfirmationAreNotMatch;

		// 6a. Show Error dialogue
		local_cas_va_Pindlg_ShowModifyResult(s_CasVaPinChangeError);
	}
	else	// Match
	{
		// 6b. Change Pin code
		// 7. Chage PIN OK?
//		AP_CAS_VA_ChangePinCode(s_szCasVaInputedOldPinCodeStr, pstInstance->szNewPinCodeStr, &nResult); // hjlee3, just for compile
		ApCasVaPinUiPrint(DBG_TRACE, ("result (%d)\n", nResult));

		switch(nResult)
		{
			case ERR_OK:
// hjlee3, just for compile				local_cas_va_SetAcceptCAPincode (pstInstance->szNewPinCodeStr);
				VK_memset32(s_szCasVaInputedOldPinCodeStr, 0, 5);

				s_CasVaPinChangeStep_InDlg = eVA_PinChangeStep_InDlg_InputCurPincode;
				s_CasVaPinChangeError = eVA_PinChangeError_Normal;
				break;

			case VA_ERR_UI_WRONG_PIN_CODE:
				s_CasVaPinChangeStep_InDlg = eVA_PinChangeStep_InDlg_InputCurPincode;
				s_CasVaPinChangeError = eVA_PinChangeError_InputCodeAreNotMatch;
				break;

			case VA_ERR_UI_LOCKED_PIN_CODE:
				s_CasVaPinChangeStep_InDlg = eVA_PinChangeStep_InDlg_InputCurPincode;
				s_CasVaPinChangeError = eVA_PinChangeError_ChangeFailed;
				break;

			case VA_ERR_UI_MAX_CHANGES_PIN_CODE:
				break;

			case VA_ERR_UI_FORBIDDEN_PIN_CODE:
				break;

			case VA_ERR_UI_NO_PIN_CODE:
				s_CasVaPinChangeStep_InDlg = eVA_PinChangeStep_InDlg_InputCurPincode;
				s_CasVaPinChangeError = eVA_PinChangeError_NoPincode;
				break;

			case VA_ERR_UI_SMARTCARD_INSUFFICIENT_MEMORY:
				break;

			case VA_ERR_UI_WRONG_SLOT:
				break;

			case VA_ERR_UI_SMARTCARD_ERROR:
				break;

			default:
				s_CasVaPinChangeStep_InDlg = eVA_PinChangeStep_InDlg_InputCurPincode;
				s_CasVaPinChangeError = eVA_PinChangeError_Normal;

				ApCasVaPinUiError(DBG_ASSERT, ("Unknown result (%d)\n", nResult));
				return;
			break;
		}

		local_cas_va_Pindlg_ShowModifyResult(s_CasVaPinChangeError);
	}

}


void	local_cas_va_SetAcceptCAPincode (HUINT8 *szPin)
{
	memcpy (s_AcceptPin, szPin, (sizeof (HUINT8)) * 5);
	s_AcceptPin[5-1] = '\0';
}

void	local_cas_va_GetAcceptCAPincode (HUINT8 *szPin)
{
	memcpy (szPin, s_AcceptPin, (sizeof (HUINT8)) * 5);
	szPin[5-1] = '\0';
}


STATIC HINT32	local_cas_va_Pindlg_CheckCurrNumber(HUINT32 *pstInstance, eCasVA_PinChangeStep_InDlg ePinChangeStep, HINT32 number)
{
#if 0 // not use for octo2.0
	if (pstInstance->nPinInputPos < 5-1)
	{
		HUINT8 *pszTargetStr = pstInstance->szPinCodeStr;

		if (ePinChangeStep == eVA_PinChangeStep_InDlg_InputNewPincode)
		{
			pszTargetStr = pstInstance->szNewPinCodeStr;
		}
		else if (ePinChangeStep == eVA_PinChangeStep_InDlg_VerifyNewPincode)
		{
			pszTargetStr = pstInstance->szVerifyPinCodeStr;
		}

		/* set curr by inputed value */
		pstInstance->szPinCodeStr[pstInstance->nPinInputPos] = (HUINT8)('0' + number);
		pszTargetStr[pstInstance->nPinInputPos] = (HUINT8)('0' + number);

		/* invalidate Curr */
		GWM_APP_InvalidateObject(ALERT_PIN_CODE_ID + pstInstance->nPinInputPos);

		/* go to next */
		pstInstance->nPinInputPos++;

		/* set focus next */
		GWM_Obj_SetFocus(ALERT_PIN_CODE_ID + pstInstance->nPinInputPos);
	}

	return	pstInstance->nPinInputPos;
#endif
}


STATIC HUINT8*	local_cas_va_Pindlg_GetCurPincodeDlgMsg()
{
	HUINT8		*pszMsg = NULL;

	if (s_CasVaPinChangeError == eVA_PinChangeError_InputCodeAreNotMatch)
	{
		// 모든 String 관리는 Web에서 처리하는 것으로 약속됨에 따라 AP_CasResStr_GetStrRsc() 함수를 더이상 사용하지 말아야 함.
		// 따라서 LOC ID를 Web으로 전송 후 Web에서 그에 맞는 string을 찾아 출력하도록 함.
		todo
		pszMsg = (HUINT8*)AP_CasResStr_GetStrRsc(STR_WRONG_PIN_CODE_ID);
	}
	else
	{
		pszMsg = (HUINT8*)AP_CasResStr_GetStrRsc(STR_MESG_1126_ID);
	}

	return	pszMsg;
}


STATIC HUINT8*	local_cas_va_Pindlg_GetNewPincodeDlgMsg()
{
	HUINT8 *pszMsg = NULL;

	if (s_CasVaPinChangeError == eVA_PinChangeError_InputCodeAreNotMatch)
	{
		// 모든 String 관리는 Web에서 처리하는 것으로 약속됨에 따라 AP_CasResStr_GetStrRsc() 함수를 더이상 사용하지 말아야 함.
		// 따라서 LOC ID를 Web으로 전송 후 Web에서 그에 맞는 string을 찾아 출력하도록 함.
		todo
		pszMsg = (HUINT8*)AP_CasResStr_GetStrRsc(STR_MESG_1131_ID);
		s_CasVaPinChangeError = eVA_PinChangeError_Normal;
	}
	else
	{
		pszMsg = (HUINT8*)AP_CasResStr_GetStrRsc(STR_MESG_1127_ID);
	}

	return	pszMsg;
}


STATIC HUINT8*	local_cas_va_Pindlg_GetVerifyNewPincodeDlgMsg()
{
	// 모든 String 관리는 Web에서 처리하는 것으로 약속됨에 따라 AP_CasResStr_GetStrRsc() 함수를 더이상 사용하지 말아야 함.
	// 따라서 LOC ID를 Web으로 전송 후 Web에서 그에 맞는 string을 찾아 출력하도록 함.
	todo
	return (HUINT8*)AP_CasResStr_GetStrRsc(STR_MESG_1128_ID);
}


#define	________________Custom_Drawing_Func_________________
#if 0 // not use for octo2.0
STATIC void	local_cas_va_PinDlg_DrawUpdate(HUINT32 *pstInstance, HBOOL bEnabled)
{
	OSD_Rect_t		rc, rcTitleArea, rcMsgArea, rcMsgArea1, rcCodeBtnArea;
	HINT32			nFontSize = FONT_SIZE_S;
	HINT8			i = 0;
	HUINT8			ucPinAttemptCount = 2;
	HUINT8			szPinRetryNum[128];
	HUINT8			szPinEnterStep[128];

	// frame, title
	GWM_Obj_Destroy(ALERT_PIN_FRAME_ID);

	OSD_GFX_SetRect(&rcTitleArea, CASVA_PIN_FRAME_X, CASVA_PIN_FRAME_Y, CASVA_PIN_FRAME_W, CASVA_PIN_FRAME_H);
	OSD_GFX_SetRect(&rc, 0, 0, rcTitleArea.w, rcTitleArea.h);
	GWM_APP_SetAppArea(rcTitleArea.x, rcTitleArea.y, rcTitleArea.w,rcTitleArea.h);
	GWM_Frame_Create(ALERT_PIN_FRAME_ID, &rc, (char*)pstInstance->pszDlgTitle);
	GWM_APP_SetObjectDrawMethod(ALERT_PIN_FRAME_ID, AP_LNF_WindowTD299Frame_Draw);

	// msg
	GWM_Obj_Destroy(ALERT_PIN_DESC_ID);

	OSD_GFX_SetRect(&rcMsgArea, CASVA_PIN_MSG_X, CASVA_PIN_MSG_Y, CASVA_PIN_MSG_W, CASVA_PIN_MSG_H);
	GWM_Text_Create(ALERT_PIN_DESC_ID, &rcMsgArea, pstInstance->pszDlgMsg);


	GWM_Text_SetSize(ALERT_PIN_DESC_ID, nFontSize);
	GWM_Text_SetAlign(ALERT_PIN_DESC_ID, TEXT_ALIGN_CENTER);
	GWM_Text_ChangeColor(ALERT_PIN_DESC_ID, COL(C_T_Dialog_MSG), 0);
	GWM_APP_SetObjectDrawMethod(ALERT_PIN_DESC_ID, AP_LNF_PlainMultilineText_Draw);

	// code btn
	for (i = 0; i < 5-1; i++)
	{
		GWM_Obj_Destroy(ALERT_PIN_CODE_ID + i);

		OSD_GFX_SetRect(&rcCodeBtnArea, CASVA_PIN_CODE_BTN_X + i * (ALERT_PIN_MSG_CODERC_W + ALERT_PIN_MSG_CODE_GAP),
			CASVA_PIN_CODE_BTN_Y, ALERT_PIN_MSG_CODERC_W, ALERT_PIN_MSG_CODERC_H);
		GWM_Button_Create(ALERT_PIN_CODE_ID + i, &rcCodeBtnArea, (char*)pstInstance->szPinCodeStr + i);
		GWM_APP_SetObjectDrawMethod(ALERT_PIN_CODE_ID + i, AP_LNF_PinCodeButton_Draw);
	}

	if (bEnabled)
	{
		GWM_Obj_SetFocus(ALERT_PIN_CODE_ID);
	}

	/* invalidate dialog area */
	GWM_APP_InvalidateObject(ALERT_PIN_FRAME_ID);
}
#endif

#define	________________Launch_Popups_______________________

STATIC HERROR	local_cas_va_Pindlg_NoticeDialogProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
#if 0 // not use for octo2.0
	return AP_Dialog_ProtoProc(&s_stNoticeDialogInst, message, hAction, p1, p2, p3);
#endif
}

STATIC void	  local_cas_va_Pindlg_ShowModifyResult(eCasVA_PinChangeError eChgPinErr)
{
#if 0 // not use for octo2.0
	HINT32 nLocId;

	switch(eChgPinErr)
	{
	case eVA_PinChangeError_Normal:
		nLocId = LOC_CAS_VA_MSG_CHANGE_PIN_SUCCESS_ID;
		break;

	case eVA_PinChangeError_InputCodeAreNotMatch:
		nLocId = LOC_CAS_VA_ERR_WRONG_OLD_PIN_ID;
		break;

	case eVA_PinChangeError_ConfirmationAreNotMatch:
		nLocId = LOC_CAS_VA_ERR_MISMATCH_NEW_PIN_ID;
		break;

	case eVA_PinChangeError_ChangeFailed:
		nLocId = LOC_CAS_VA_ERR_CHANGE_PIN_ID;
		break;

	case eVA_PinChangeError_ScLocked:
		nLocId = LOC_CAS_VA_ERR_SC_LOCKED_ID;
		break;

	case eVA_PinChangeError_NoPincode:
		nLocId = LOC_CAS_VA_MSG_SC_NO_PINCODE_IN_CARD_ID;
		eChgPinErr = eVA_PinChangeError_Normal;
		break;

	default:
		ApCasVaPinUiError(DBG_ASSERT, ("Unknown err (%d)\n", eChgPinErr));
		return;
	}

#if defined(CONFIG_OP_NORDIG_BOXER)
	if (eChgPinErr == eVA_PinChangeError_ScLocked)
	{
		HUINT8	*pszStr1 = NULL;

		pszStr1 = GetTextBuffer(TEXT_SIZE512);
		VK_memset32(pszStr1, 0, TEXT_SIZE512);

		if (xmgr_cas_VaAdaptGetBoxerScLockMsg(pszStr1) != ERR_OK)
		{	// error case
			ApCasVaPinUiError(DBG_ASSERT, ("xmgr_cas_VaAdaptGetBoxerScLockMsg() Fail \n"));
		}
		else
		{
			AP_Dialog_SetDialog(&s_stNoticeDialogInst,
						BUS_MGR_GetAppCallback(BUS_MGR_GetThis()),
						(DLG_OK | DLG_TIMEOUT_NOTIMEOUT /*| DLG_TYPE_TITLE */| DLG_TYPE_MESSAGE),
						/*AP_CasResStr_GetStrRsc(LOC_CHLIST_PAYCH_VIACCESS_ELEMENT_ID)*/NULL, pszStr1);
		}
	}
	else
#endif
	{
		AP_Dialog_SetDialog(&s_stNoticeDialogInst,
						BUS_MGR_GetAppCallback(BUS_MGR_GetThis()),
						(DLG_OK | DLG_TIMEOUT_NOTIMEOUT /*| DLG_TYPE_TITLE */| DLG_TYPE_MESSAGE),
						/*AP_CasResStr_GetStrRsc(LOC_CHLIST_PAYCH_VIACCESS_ELEMENT_ID)*/NULL, AP_CasResStr_GetStrRsc(nLocId));
	}

	BUS_MGR_Create("local_cas_va_Pindlg_NoticeDialogProc", APP_DIALOGBOX_PRIORITY, (BUS_Callback_t)local_cas_va_Pindlg_NoticeDialogProc, 0,0,0,0);
#endif
}


#define _________________Message_____________________________

STATIC void	local_cas_va_Pindlg_NotifyPinAuthorized(HUINT32 *pstInstance)
{
	BUS_MGR_Destroy(0);

	s_CasVaPinChangeStep_InDlg = eVA_PinChangeStep_InDlg_InputCurPincode;
	s_CasVaPinChangeError = eVA_PinChangeError_Normal;
// hjlee3, just for compile  BUS_SendMessage(pstInstance->parentAppCallback, eMEVT_PG_PIN_CHECK_OK, (int)NULL, 0, (int)pstInstance->ulPinDlgWhoseId, (int)pstInstance->thisAppCallback);
}


STATIC void	local_cas_va_Pindlg_NotifyPinDenied(HUINT32 *pstInstance)
{
	eCasVA_PinChangeError	eCasVaPinChangeError;

	BUS_MGR_Destroy(0);

	eCasVaPinChangeError = s_CasVaPinChangeError;
	s_CasVaPinChangeStep_InDlg = eVA_PinChangeStep_InDlg_InputCurPincode;
	s_CasVaPinChangeError = eVA_PinChangeError_Normal;

// hjlee3, just for compile  	BUS_SendMessage(pstInstance->parentAppCallback, eMEVT_PG_PIN_CHECK_DENIED, (int)NULL, eCasVaPinChangeError, (int)pstInstance->ulPinDlgWhoseId, (int)pstInstance->thisAppCallback);
}


STATIC BUS_Result_t	local_cas_va_Pindlg_MsgGwmCreate(HUINT32 *pstInstance, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t			eGwmResult = ERR_BUS_NO_ERROR;

	AP_CAS_VA_PARAM_WARRING_REMOVE(pstInstance, p1, p2, p3);

	s_CasVaPinChangeStep_InDlg = (eCasVA_PinChangeStep_InDlg)p1;

	eGwmResult = local_cas_va_Pindlg_CreatePincode(pstInstance, s_CasVaPinChangeStep_InDlg);

	return	eGwmResult;
}


STATIC BUS_Result_t	local_cas_va_Pindlg_MsgGwmKeyUp(HUINT32 *pstInstance, HINT32 p1, HINT32 p2, HINT32 p3)
{
#if 0 // not use for octo2.0
	AP_CAS_VA_PARAM_WARRING_REMOVE(pstInstance, p1, p2, p3);

	AP_KeyConv_UiCommon(&p1);
#endif
	return	ERR_BUS_NO_ERROR;
}


STATIC BUS_Result_t	local_cas_va_Pindlg_MsgDlgClicked(HUINT32 *pstInstance, HINT32 p1, HINT32 p2, HINT32 p3)
{
	AP_CAS_VA_PARAM_WARRING_REMOVE(pstInstance, p1, p2, p3);

//	if (p1 == eDLG_CLICKED_BTN_MIDDLE) //hjlee3, just for compile
	{
		//AP_CAS_VA_HideMessageBox();
		BUS_MGR_Destroy((BUS_Callback_t)local_cas_va_Pindlg_NoticeDialogProc);

		BUS_MGR_Destroy(NULL);

		if (s_CasVaPinChangeError == eVA_PinChangeError_Normal)
		{
			local_cas_va_Pindlg_NotifyPinAuthorized(pstInstance);
		}
		else
		{
			local_cas_va_Pindlg_NotifyPinDenied(pstInstance);
		}

	}
	return	ERR_BUS_NO_ERROR;
}


STATIC BUS_Result_t	local_cas_va_Pindlg_MsgGwmKeyDown(HUINT32 *pstInstance, HINT32 p1, HINT32 p2, HINT32 p3)
{
#if 0 // not use for octo2.0
	AP_CAS_VA_PARAM_WARRING_REMOVE(pstInstance, p1, p2, p3);

	AP_KeyConv_UiCommon(&p1);

	BUS_SendMessage(NULL, MSG_APP_CHLIST_RESET_OSDTIMER, (int)NULL, 0, 0, 0);
	GWM_ResetTimer(DLG_EXP_TIMER_ID);

	switch (p1)
	{
		/* keys to pin quit */
		case KEY_BACK:
		case KEY_EXIT:
			BUS_MGR_Destroy(NULL);
			return	MESSAGE_BREAK;

		/* keys to ignore */
		case KEY_ARROWRIGHT:
		case KEY_ARROWUP:
		case KEY_ARROWDOWN:
		case KEY_RED:
		case KEY_BLUE:
		case KEY_GREEN:
		case KEY_YELLOW:
			return	MESSAGE_BREAK;

#if 0 /* CPP가 지우지 말란다... */
			/* go back */
		case KEY_ARROWLEFT:
			local_pindlg_BackspacePosition(pstInstance);
			return MESSAGE_BREAK;
#else
		case KEY_ARROWLEFT:
			return	MESSAGE_BREAK;
#endif

		case KEY_0:
		case KEY_1:
		case KEY_2:
		case KEY_3:
		case KEY_4:
		case KEY_5:
		case KEY_6:
		case KEY_7:
		case KEY_8:
		case KEY_9:
			if (pstInstance->ulDlgAttributes & DLG_TYPE_BLKPINCODE)
			{
				return	MESSAGE_BREAK;
			}

			if (local_cas_va_Pindlg_CheckCurrNumber(pstInstance, s_CasVaPinChangeStep_InDlg, (p1 - KEY_0)) >= 5-1)
			{
				GWM_DirectRedrawAll();
				local_cas_va_Pindlg_CheckPincodeStep(pstInstance, s_CasVaPinChangeStep_InDlg);
			}
			return	MESSAGE_BREAK;
	}
#endif
	return	ERR_BUS_NO_ERROR;
}


#define	________________Init_Func____________________________


STATIC BUS_Result_t	local_cas_va_Pindlg_Proc(HINT32 message, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t			gwmResult = ERR_BUS_NO_ERROR;
	HUINT32	*pstInstance = NULL;

	pstInstance = local_cas_va_Pindlg_GetInstance();
#if 0 // not use for octo2.0
	switch (message)
	{
		case MSG_GWM_CREATE:
			gwmResult = local_cas_va_Pindlg_MsgGwmCreate(pstInstance, p1, p2, p3);
			break;

		case MSG_GWM_TIMER:
			break;

		case MSG_GWM_KEYUP:
			gwmResult = local_cas_va_Pindlg_MsgGwmKeyUp(pstInstance, p1, p2, p3);
			break;

		case MSG_GWM_KEYDOWN:
			gwmResult = local_cas_va_Pindlg_MsgGwmKeyDown(pstInstance, p1, p2, p3);
			break;

		case MSG_DLG_CLICKED:
			gwmResult = local_cas_va_Pindlg_MsgDlgClicked(pstInstance, p1, p2, p3);
			return MESSAGE_BREAK;

		case MSG_GWM_DESTROY:
			BUS_MGR_Destroy(local_cas_va_Pindlg_NoticeDialogProc);
			break;
	}	// end switch

	if ((gwmResult == MESSAGE_BREAK) || (gwmResult == MESSAGE_PASS))
	{
		return	gwmResult;
	}
#endif
	return	BUS_ProcessMessageDefault(message, handle, p1, p2, p3);
}


STATIC BUS_Result_t	local_cas_va_Pindlg_CreatePincode(HUINT32* pstInstance, eCasVA_PinChangeStep_InDlg ePinChangeStep)
{
	HUINT8		szInputedNewPinCodeStr[5];
	HUINT8		*pszTitle = NULL;
	HUINT8		*pszPinCodeMsg = NULL;
	HUINT8		pszAttemptMsg[TEXT_SIZE64];
	HUINT8		ucRemainCnt, ucMaxCnt;
	HINT32		nResult;
#if 0 // not use for octo2.0

	pszTitle = (HUINT8*)AP_CasResStr_GetStrRsc(LOC_CAS_VA_TITLE_VIACCESS_ID);

	if(ePinChangeStep != eVA_PinChangeStep_InDlg_MAX)
	{
		AP_CAS_VA_GetPinCodeAttemps(&ucRemainCnt, &ucMaxCnt, &nResult);

		if(nResult != ERR_OK)
		{
			BUS_MGR_Destroy(NULL);
			return ERR_FAIL;
		}

		if(ucRemainCnt <= 0)
		{
			BUS_MGR_Destroy(NULL);
			BUS_SendMessage(pstInstance->parentAppCallback, eMEVT_PG_PIN_CHECK_DENIED, (int)NULL, (HINT32)eVA_PinChangeError_ScLocked, (int)pstInstance->ulPinDlgWhoseId, (int)pstInstance->thisAppCallback);
			return ERR_FAIL;
		}

		switch (ePinChangeStep)
		{
		case eVA_PinChangeStep_InDlg_InputCurPincode:
			pszPinCodeMsg = local_cas_va_Pindlg_GetCurPincodeDlgMsg();
			break;

		case eVA_PinChangeStep_InDlg_InputNewPincode:
			pszPinCodeMsg = local_cas_va_Pindlg_GetNewPincodeDlgMsg();
			break;

		case eVA_PinChangeStep_InDlg_VerifyNewPincode:
			VK_memcpy32(szInputedNewPinCodeStr, pstInstance->szNewPinCodeStr, 5);
			pszPinCodeMsg = local_cas_va_Pindlg_GetVerifyNewPincodeDlgMsg();
			break;

		default:
			ApCasVaPinUiError(DBG_ASSERT, ("ePinChangeStep[%x] don't create pin-code dialog \n", ePinChangeStep));
			BUS_MGR_Destroy(NULL);
			return	ERR_FAIL;
		}

		if(ucRemainCnt == 0xff && ucMaxCnt == 0xff)
		{
			MWC_UTIL_DvbUtf8Sprintf (s_szPincodeMessage, "%s", pszPinCodeMsg);
		}
		else
		{
			snprintf (pszAttemptMsg, TEXT_SIZE64, AP_CasResStr_GetStrRsc(LOC_CAS_VA_MSG_ATTEMPT_LEFT_ID), ucRemainCnt);
			MWC_UTIL_DvbUtf8Sprintf (s_szPincodeMessage, "%s\n%s.", pszPinCodeMsg, pszAttemptMsg);
		}

		local_cas_va_Pindlg_Create_PincodeDlg(pstInstance, pszTitle, s_szPincodeMessage);
	}

	// szNewPinCodeStr를 사용하기 위해
	if (ePinChangeStep == eVA_PinChangeStep_InDlg_VerifyNewPincode)
	{
		VK_memcpy32(pstInstance->szNewPinCodeStr, szInputedNewPinCodeStr, 5);
	}
#endif
	return	ERR_OK;
}


STATIC BUS_Result_t	local_cas_va_Pindlg_Create_PincodeDlg(HUINT32* pstInstance, HUINT8 *pszTitle, HUINT8 *pszMsg)
{
	HUINT32 	ulDlgAttr = 0;
	HBOOL		bFocusEnabled = FALSE;
#if 0 // not use for octo2.0

	ulDlgAttr = /*DLG_TYPE_TITLE | */DLG_TYPE_MESSAGE | DLG_TIMEOUT_NOTIMEOUT ;

	AP_PinDialog_SetDialog(pstInstance, s_parentApp, ulDlgAttr, pszTitle, pszMsg);
	AP_PinDialog_UpdateDlg(pstInstance);
#endif
	return	ERR_BUS_NO_ERROR;
}


#define	________________Global_Function_______________________

/*******************************************************************/
/********************      	Global Function 	********************/
/*******************************************************************/
/* TODO : 이거 필요 없나 ?? 확인 하자 hjlee3
BUS_Result_t	AP_CAS_VA_PinDialog_Proc(HUINT32 *pstInstance, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	s_parentApp = pstInstance->parentAppCallback;

	return	local_cas_va_Pindlg_Proc(message, hAction, p1, p2, p3);
}
*/
void AP_CAS_VA_PinDialogDrawUpdate(HUINT32 *pstInstance)
{
	//local_cas_va_PinDlg_DrawUpdate(pstInstance, FALSE);
}

// End of file
