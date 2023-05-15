/**
	@file     ap_cas_cx_pincode_ui.c
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


#include <mgr_pg.h>
#include "../local_include/_xmgr_cas_cx_util.h"
#include "../local_include/_xmgr_cas_cx_adapt.h"
#include <xmgr_cas_res_str.h>


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

STATIC HUINT8		s_szMsg[TEXT_SIZE512];


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/



// timer
#define		SKYD_CAS_NA_UI_PINCODE_TIMER_ID			500
#define		SKYD_CAS_NA_UI_PINCODE_TIMEOUT			2000

/*******************************************************************/
/********************      typedef         *************************/
/*******************************************************************/



/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC AP_PinDialogInstance_t			s_stPinDialogInst;
STATIC BUS_Callback_t					s_procParent;
STATIC XmgrCas_CxPinChangeStepInDlg_e	s_eCasCxPinChangeStepInDlg;
STATIC XmgrCas_CxPinChangeError_e		s_eCasCxPinChangeError;
STATIC HUINT8							s_szCasCxInputedOldPinCodeStr[5];
STATIC AP_DialogInstance_t				s_stNoticeDialogInst;		/**< Pin Change Suceess Notice Dialog Instans. */


/*******************************************************************/
/********************      	Static Prototype	********************/
/*******************************************************************/

// UI (body msg) manage
STATIC AP_PinDialogInstance_t*	xmgr_cas_CxPinGetInstance(void);
STATIC HBOOL			xmgr_cas_CxPinCheckChgPinAreNotMatch(AP_PinDialogInstance_t *pstInstance);
STATIC HINT32			xmgr_cas_CxPinCheckCurrNumber(AP_PinDialogInstance_t *pstInstance, XmgrCas_CxPinChangeStepInDlg_e ePinChangeStep, HINT32 number);
STATIC BUS_Result_t		xmgr_cas_CxPinCheckPincodeStep(AP_PinDialogInstance_t *pstInstance, XmgrCas_CxPinChangeStepInDlg_e ePinChangeStep);
STATIC void				xmgr_cas_CxPinCheckPincodeProcess(AP_PinDialogInstance_t *pstInstance);

STATIC HUINT8*			xmgr_cas_CxPinGetCurPincodeDlgMsg(void);
STATIC HUINT8*			xmgr_cas_CxPinGetNewPincodeDlgMsg(void);
STATIC HUINT8*			xmgr_cas_CxPinGetVerifyNewPincodeDlgMsg(void);

STATIC void				xmgr_cas_CxPinDlgDrawUpdate(AP_PinDialogInstance_t *pstInstance, HBOOL bEnabled);

// notice dlg manage
STATIC HERROR			xmgr_cas_CxPinNoticeDialogProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC void				xmgr_cas_CxPinCreateNoticeDialog(XmgrCas_CxPinChangeError_e eChgPinErr);
STATIC HERROR			xmgr_cas_CxPinDestroyNoticeDialog(void);

// Msg handling
STATIC void				xmgr_cas_CxPinNotifyPinAuthorized(AP_PinDialogInstance_t *pstInstance);
STATIC void				xmgr_cas_CxPinNotifyPinDenied(AP_PinDialogInstance_t *pstInstance);

STATIC BUS_Result_t 	xmgr_cas_CxPinMgsCreate(AP_PinDialogInstance_t *pstInstance, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t 	xmgr_cas_CxPinMgsTimer(AP_PinDialogInstance_t *pstInstance, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t 	xmgr_cas_CxPinMgsKeyUp(AP_PinDialogInstance_t *pstInstance, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t 	xmgr_cas_CxPinMgsKeyDown(AP_PinDialogInstance_t *pstInstance, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t 	xmgr_cas_CxPinMsgDlgCliecked(AP_PinDialogInstance_t *pstInstance, HINT32 p1, HINT32 p2, HINT32 p3);

STATIC BUS_Result_t		xproc_cas_CxPinDialog(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t		xmgr_cas_CxPinCreatePincode(AP_PinDialogInstance_t* pstInstance, XmgrCas_CxPinChangeStepInDlg_e ePinChangeStep);
STATIC BUS_Result_t		xmgr_cas_CxPinCreatePincodeDlg(AP_PinDialogInstance_t* pstInstance, HUINT8 *pszTitle, HUINT8 *pszMsg);


/*******************************************************************/
/********************      	Static Function 	********************/
/*******************************************************************/

#define _________________UI__________________________________

STATIC AP_PinDialogInstance_t*	xmgr_cas_CxPinGetInstance(void)
{
	return &s_stPinDialogInst;
}


STATIC HBOOL	xmgr_cas_CxPinCheckChgPinAreNotMatch(AP_PinDialogInstance_t *pstInstance)
{
	if (MWC_UTIL_DvbStrncmp(pstInstance->szVerifyPinCodeStr, pstInstance->szNewPinCodeStr, 5-1))
	{
		return	TRUE;	// Not matched
	}

	return	FALSE;		// Madched
}


STATIC BUS_Result_t	xmgr_cas_CxPinCheckPincodeStep(AP_PinDialogInstance_t *pstInstance, XmgrCas_CxPinChangeStepInDlg_e ePinChangeStep)
{

	if (ePinChangeStep == eCX_PinChangeStep_InDlg_InputCurPincode)
	{
		VK_memcpy32(s_szCasCxInputedOldPinCodeStr, pstInstance->szPinCodeStr, 5);

		// 2. Enter old PIN
		#if 0 // not use for octo2.0
		BUS_MGR_Destroy(NULL);
		#endif
		xmgr_cas_CxPinNotifyPinAuthorized(pstInstance);
	}
	else if (ePinChangeStep == eCX_PinChangeStep_InDlg_InputNewPincode)
	{
		// 3. Enter new PIN
		#if 0 // not use for octo2.0
		BUS_MGR_Destroy(NULL);
		#endif
		xmgr_cas_CxPinNotifyPinAuthorized(pstInstance);
	}
	else if (ePinChangeStep == eCX_PinChangeStep_InDlg_VerifyNewPincode)
	{
		// 4. Comfirm new PIN
		xmgr_cas_CxPinCheckPincodeProcess(pstInstance);
	}
	else
	{
		HxLOG_Critical("\n\n");
	}

	return	MESSAGE_BREAK;
}


STATIC void	xmgr_cas_CxPinCheckPincodeProcess(AP_PinDialogInstance_t *pstInstance)
{
	HERROR		hError = ERR_FAIL;

	// 5. New Pin comparision
	if (xmgr_cas_CxPinCheckChgPinAreNotMatch(pstInstance))	// Not match
	{
		s_eCasCxPinChangeStepInDlg = eCX_PinChangeStep_InDlg_InputCurPincode;
		s_eCasCxPinChangeError = eCX_PinChangeError_ConfirmationAreNotMatch;

		// 6a. Show Error dialogue
		xmgr_cas_CxPinCreateNoticeDialog(s_eCasCxPinChangeError);
	}
	else	// Match
	{
		// 6b. Change Pin code
		// 7. Chage PIN OK?
		hError = xmgr_cas_CxChangeSmartCardPin(s_szCasCxInputedOldPinCodeStr, pstInstance->szNewPinCodeStr);
		if (hError == ERR_OK)
		{
			// 9c. CA PIN has changed
			HxLOG_Print("==[New PIN CODE]== Pin-code is Okay(old - %s)(new - %s)\n",
				s_szCasCxInputedOldPinCodeStr, pstInstance->szVerifyPinCodeStr);

			xmgr_cas_CxSetAcceptCAPincode(pstInstance->szNewPinCodeStr);
			VK_memset32(s_szCasCxInputedOldPinCodeStr, 0, 5);

			s_eCasCxPinChangeStepInDlg = eCX_PinChangeStep_InDlg_InputCurPincode;
			s_eCasCxPinChangeError = eCX_PinChangeError_Normal;
		}
		else if (hError == CX_ERR_UI_WRONG_PIN)
		{
			// 9a. Incorrect old PIN
			HxLOG_Print("==[New PIN CODE]== Pin-code is CX_ERR_UI_WRONG_PIN \n");

			s_eCasCxPinChangeStepInDlg = eCX_PinChangeStep_InDlg_InputCurPincode;
			s_eCasCxPinChangeError = eCX_PinChangeError_InputCodeAreNotMatch;
		}
		else if (hError == CX_ERR_UI_DIFFERENT_NEW_PIN)
		{
			// 6a. PIN confirm ation does not match
			HxLOG_Print("==[New PIN CODE]== Pin-code is CX_ERR_UI_DIFFERENT_NEW_PIN \n");

			s_eCasCxPinChangeStepInDlg = eCX_PinChangeStep_InDlg_InputCurPincode;
			s_eCasCxPinChangeError = eCX_PinChangeError_ConfirmationAreNotMatch;
		}
		else if (hError == CX_ERR_UI_PIN_LOCKED)
		{
			HxLOG_Print("==[New PIN CODE]== Pin-code is CX_ERR_UI_PIN_LOCKED \n");

			s_eCasCxPinChangeStepInDlg = eCX_PinChangeStep_InDlg_InputCurPincode;
			s_eCasCxPinChangeError = eCX_PinChangeError_ChangeFailed;
		}
		else if (hError == CX_ERR_UI_PIN_CHANGE_FAIL)
		{
			// 9b. pin change fail
			HxLOG_Print("==[New PIN CODE]== Pin-code is CX_ERR_UI_PIN_CHANGE_FAIL \n");

			s_eCasCxPinChangeStepInDlg = eCX_PinChangeStep_InDlg_InputCurPincode;
			s_eCasCxPinChangeError = eCX_PinChangeError_ChangeFailed;
		}

		else
		{
			// UnKnow Msg
			s_eCasCxPinChangeStepInDlg = eCX_PinChangeStep_InDlg_InputCurPincode;
			s_eCasCxPinChangeError = eCX_PinChangeError_Normal;

			HxLOG_Critical("\n\n");
			return;
		}

		xmgr_cas_CxPinCreateNoticeDialog(s_eCasCxPinChangeError);
	}

}



STATIC HINT32	xmgr_cas_CxPinCheckCurrNumber(AP_PinDialogInstance_t *pstInstance, XmgrCas_CxPinChangeStepInDlg_e ePinChangeStep, HINT32 number)
{
	if (pstInstance->nPinInputPos < 5-1)
	{
		HUINT8 *pszTargetStr = pstInstance->szPinCodeStr;

		if (ePinChangeStep == eCX_PinChangeStep_InDlg_InputNewPincode)
		{
			pszTargetStr = pstInstance->szNewPinCodeStr;
		}
		else if (ePinChangeStep == eCX_PinChangeStep_InDlg_VerifyNewPincode)
		{
			pszTargetStr = pstInstance->szVerifyPinCodeStr;
		}

		/* set curr by inputed value */
		pstInstance->szPinCodeStr[pstInstance->nPinInputPos] = (HUINT8)('0' + number);
		pszTargetStr[pstInstance->nPinInputPos] = (HUINT8)('0' + number);

		#if 0 // not use for octo2.0
		/* invalidate Curr */
		GWM_APP_InvalidateObject(ALERT_PIN_CODE_ID + pstInstance->nPinInputPos);

		/* go to next */
		pstInstance->nPinInputPos++;

		/* set focus next */
		GWM_Obj_SetFocus(ALERT_PIN_CODE_ID + pstInstance->nPinInputPos);
		#endif
	}

	return	pstInstance->nPinInputPos;
}


STATIC HUINT8*	xmgr_cas_CxPinGetCurPincodeDlgMsg()
{
	HUINT8		*pszMsg = NULL;

	if (s_eCasCxPinChangeError == eCX_PinChangeError_InputCodeAreNotMatch)
	{
		pszMsg = (HUINT8*)AP_CasResStr_GetStrRsc(STR_MESG_398_ID);
	}
	else
	{
		pszMsg = (HUINT8*)AP_CasResStr_GetStrRsc(STR_ENTER_CA_PIN_ID);
	}

	return	pszMsg;
}


STATIC HUINT8*	xmgr_cas_CxPinGetNewPincodeDlgMsg()
{
	HUINT8 *pszMsg = NULL;

	if (s_eCasCxPinChangeError == eCX_PinChangeError_InputCodeAreNotMatch)
	{
		pszMsg = (HUINT8*)AP_CasResStr_GetStrRsc(STR_MESG_409_ID);
		s_eCasCxPinChangeError = eCX_PinChangeError_Normal;
	}
	else
	{
		pszMsg = (HUINT8*)AP_CasResStr_GetStrRsc(STR_MESG_357_ID);
	}

	return	pszMsg;
}


STATIC HUINT8*	xmgr_cas_CxPinGetVerifyNewPincodeDlgMsg()
{
	return (HUINT8*)AP_CasResStr_GetStrRsc(STR_MESG_354_ID);
}


#define	________________Custom_Drawing_Func_________________
// TODO: To Remove
#if 0 // not use for octo2.0
STATIC void	xmgr_cas_CxPinDlgDrawUpdate(AP_PinDialogInstance_t *pstInstance, HBOOL bEnabled)
{
	OSD_Rect_t		rc, rcTitleArea, rcMsgArea, rcCodeBtnArea;
	HINT32			nFontSize = FONT_SIZE_S;
	HINT8			i = 0;

	// frame, title
	GWM_Obj_Destroy(ALERT_PIN_FRAME_ID);

	OSD_GFX_SetRect(&rcTitleArea, CASCX_PIN_FRAME_X, CASCX_PIN_FRAME_Y, CASCX_PIN_FRAME_W, CASCX_PIN_FRAME_H);
	OSD_GFX_SetRect(&rc, 0, 0, rcTitleArea.w, rcTitleArea.h);
	GWM_APP_SetAppArea(rcTitleArea.x, rcTitleArea.y, rcTitleArea.w,rcTitleArea.h);
	GWM_Frame_Create(ALERT_PIN_FRAME_ID, &rc, (char*)pstInstance->pszDlgTitle);
	GWM_APP_SetObjectDrawMethod(ALERT_PIN_FRAME_ID, AP_LNF_WindowTD299Frame_Draw);

	// msg
	GWM_Obj_Destroy(ALERT_PIN_DESC_ID);

	OSD_GFX_SetRect(&rcMsgArea, CASCX_PIN_MSG_X, CASCX_PIN_MSG_Y, CASCX_PIN_MSG_W, CASCX_PIN_MSG_H);
	GWM_Text_Create(ALERT_PIN_DESC_ID, &rcMsgArea, pstInstance->pszDlgMsg);
	GWM_Text_SetSize(ALERT_PIN_DESC_ID, nFontSize);
	GWM_Text_SetAlign(ALERT_PIN_DESC_ID, TEXT_ALIGN_CENTER);
	GWM_Text_ChangeColor(ALERT_PIN_DESC_ID, COL(C_T_Dialog_MSG), 0);
	GWM_APP_SetObjectDrawMethod(ALERT_PIN_DESC_ID, AP_LNF_PlainMultilineText_Draw);

	// code btn
	for (i = 0; i < 5-1; i++)
	{
		GWM_Obj_Destroy(ALERT_PIN_CODE_ID + i);

		OSD_GFX_SetRect(&rcCodeBtnArea, CASCX_PIN_CODE_BTN_X + i * (ALERT_PIN_MSG_CODERC_W + ALERT_PIN_MSG_CODE_GAP),
			CASCX_PIN_CODE_BTN_Y, ALERT_PIN_MSG_CODERC_W, ALERT_PIN_MSG_CODERC_H);
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

STATIC HERROR	xmgr_cas_CxPinNoticeDialogProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	// TODO: 뭘로 대체하지? app_alert.c 에 정의된 내용이다.....
	#if 0 // not use for octo2.0
	return AP_Dialog_ProtoProc(&s_stNoticeDialogInst, message, hAction, p1, p2, p3);
	#endif

}


STATIC void	  xmgr_cas_CxPinCreateNoticeDialog(XmgrCas_CxPinChangeError_e eChgPinErr)
{
	HxSTD_memset(s_szMsg, 0, TEXT_SIZE512);

	if (eChgPinErr == eCX_PinChangeError_Normal)
	{
		MWC_UTIL_DvbStrcpy(s_szMsg, AP_CasResStr_GetStrRsc(STR_MESG_371_ID));
	}
	else if (eChgPinErr == eCX_PinChangeError_InputCodeAreNotMatch)
	{
		MWC_UTIL_DvbStrcpy(s_szMsg, AP_CasResStr_GetStrRsc(STR_INCORRECT_OLD_PIN_ID));
	}
	else if (eChgPinErr == eCX_PinChangeError_ConfirmationAreNotMatch)
	{
		MWC_UTIL_DvbStrcpy(s_szMsg, AP_CasResStr_GetStrRsc(STR_MESG_383_ID));
	}
	else if (eChgPinErr == eCX_PinChangeError_ChangeFailed)
	{
		MWC_UTIL_DvbStrcpy(s_szMsg, AP_CasResStr_GetStrRsc(STR_MESG_351_ID));
	}
	else
	{
		HxLOG_Critical("\n\n");
		return;
	}

	// TODO: Set Dialog
	#if 0 // not use for octo2.0
	AP_Dialog_SetDialog(&s_stNoticeDialogInst,
						BUS_MGR_GetAppCallback(BUS_MGR_GetThis()),
						(DLG_OK | DLG_TIMEOUT_NOTIMEOUT | DLG_TYPE_TITLE|DLG_TYPE_MESSAGE),
						(HUINT8 *)AP_CasResStr_GetStrRsc(LOC_CAS_CX_CHANGE_CA_PIN_TITLE_ID), s_szMsg);

	BUS_MGR_Create("xmgr_cas_CxPinNoticeDialogProc", APP_DIALOGBOX_PRIORITY, (BUS_Callback_t)xmgr_cas_CxPinNoticeDialogProc, 0,0,0,0);
	#endif
}


STATIC HERROR	xmgr_cas_CxPinDestroyNoticeDialog(void)
{
	#if 0 // not use for octo2.0
	return BUS_MGR_Destroy((BUS_Callback_t)xmgr_cas_CxPinNoticeDialogProc);
	#endif
}


#define _________________Message_____________________________

STATIC void	xmgr_cas_CxPinNotifyPinAuthorized(AP_PinDialogInstance_t *pstInstance)
{
	#if 0 // not use for octo2.0
	BUS_MGR_Destroy(0);
	#endif

	s_eCasCxPinChangeStepInDlg = eCX_PinChangeStep_InDlg_InputCurPincode;
	s_eCasCxPinChangeError = eCX_PinChangeError_Normal;
	BUS_SendMessage(pstInstance->parentAppCallback, eMEVT_PG_PIN_CHECK_OK, (int)NULL, 0, (int)pstInstance->ulPinDlgWhoseId, (int)pstInstance->thisAppCallback);
}


STATIC void	xmgr_cas_CxPinNotifyPinDenied(AP_PinDialogInstance_t *pstInstance)
{
	XmgrCas_CxPinChangeError_e	eCasCxPinChangeError;
#if 0 // not use for octo2.0
	BUS_MGR_Destroy(0);
#endif
	eCasCxPinChangeError = s_eCasCxPinChangeError;
	s_eCasCxPinChangeStepInDlg = eCX_PinChangeStep_InDlg_InputCurPincode;
	s_eCasCxPinChangeError = eCX_PinChangeError_Normal;
	BUS_SendMessage(pstInstance->parentAppCallback, eMEVT_PG_PIN_CHECK_DENIED, (int)NULL, eCasCxPinChangeError, (int)pstInstance->ulPinDlgWhoseId, (int)pstInstance->thisAppCallback);
}


STATIC BUS_Result_t	xmgr_cas_CxPinMgsCreate(AP_PinDialogInstance_t *pstInstance, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t			eGwmResult = ERR_BUS_NO_ERROR;

	XMGR_CAS_CX_PARAM_WARRING_REMOVE(pstInstance, p1, p2, p3);

	s_eCasCxPinChangeStepInDlg = (XmgrCas_CxPinChangeStepInDlg_e)p1;

	eGwmResult = xmgr_cas_CxPinCreatePincode(pstInstance, s_eCasCxPinChangeStepInDlg);

	return	eGwmResult;
}

// TODO: Blocking
#if 0 // not use for octo2.0
STATIC BUS_Result_t	xmgr_cas_CxPinMgsKeyUp(AP_PinDialogInstance_t *pstInstance, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XMGR_CAS_CX_PARAM_WARRING_REMOVE(pstInstance, p1, p2, p3);

	AP_KeyConv_UiCommon(&p1);

	return	ERR_BUS_NO_ERROR;
}
#endif

STATIC BUS_Result_t	xmgr_cas_CxPinMsgDlgCliecked(AP_PinDialogInstance_t *pstInstance, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XMGR_CAS_CX_PARAM_WARRING_REMOVE(pstInstance, p1, p2, p3);

	if (p1 == eDLG_CLICKED_BTN_MIDDLE)
	{
		xmgr_cas_CxPinDestroyNoticeDialog();
		#if 0 // not use for octo2.0
		BUS_MGR_Destroy(NULL);
		#endif
		if (s_eCasCxPinChangeError == eCX_PinChangeError_Normal)
		{
			xmgr_cas_CxPinNotifyPinAuthorized(pstInstance);
		}
		else
		{
			xmgr_cas_CxPinNotifyPinDenied(pstInstance);
		}

	}
	return	ERR_BUS_NO_ERROR;
}

// TODO: Blocking
#if 0 // not use for octo2.0
STATIC BUS_Result_t	xmgr_cas_CxPinMgsKeyDown(AP_PinDialogInstance_t *pstInstance, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XMGR_CAS_CX_PARAM_WARRING_REMOVE(pstInstance, p1, p2, p3);

	AP_KeyConv_UiCommon(&p1);

	BUS_SendMessage(NULL, MSG_APP_CHLIST_RESET_OSDTIMER, (int)NULL, 0, 0, 0);
	BUS_ResetTimer(DLG_EXP_TIMER_ID);

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

			if (xmgr_cas_CxPinCheckCurrNumber(pstInstance, s_eCasCxPinChangeStepInDlg, (p1 - KEY_0)) >= 5-1)
			{
				GWM_DirectRedrawAll();

				xmgr_cas_CxPinCheckPincodeStep(pstInstance, s_eCasCxPinChangeStepInDlg);
			}
			return	MESSAGE_BREAK;
	}

	return	ERR_BUS_NO_ERROR;
}
#endif

#define	________________Init_Func____________________________

STATIC BUS_Result_t	xproc_cas_CxPinDialog(HINT32 message, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t			busResult = ERR_BUS_NO_ERROR;
	AP_PinDialogInstance_t	*pstInstance = NULL;

	pstInstance = xmgr_cas_CxPinGetInstance();

	switch (message)
	{
		case eMEVT_BUS_CREATE:
			busResult = xmgr_cas_CxPinMgsCreate(pstInstance, p1, p2, p3);
			break;

		case eMEVT_BUS_TIMER:
			break;
// TODO: Blocking
#if 0 // not use for octo2.0
		case MSG_BUS_KEYUP:
			busResult = xmgr_cas_CxPinMgsKeyUp(pstInstance, p1, p2, p3);
			break;

		case MSG_BUS_KEYDOWN:
			busResult = xmgr_cas_CxPinMgsKeyDown(pstInstance, p1, p2, p3);
			break;
#endif

		case MSG_DLG_CLICKED:
			busResult = xmgr_cas_CxPinMsgDlgCliecked(pstInstance, p1, p2, p3);
			return MESSAGE_BREAK;

		case eMEVT_BUS_DESTROY:
			break;
	}	// end switch

	if ((busResult == MESSAGE_BREAK) || (busResult == MESSAGE_PASS))
	{
		return	busResult;
	}

	return	BUS_ProcessMessageDefault(message, handle, p1, p2, p3);
}


STATIC BUS_Result_t	xmgr_cas_CxPinCreatePincode(AP_PinDialogInstance_t* pstInstance, XmgrCas_CxPinChangeStepInDlg_e ePinChangeStep)
{
	HUINT8		szInputedNewPinCodeStr[5];
	HUINT8		*pszTitle = NULL;
	HUINT8		*pszMsg = NULL;

	pszTitle = (HUINT8*)AP_CasResStr_GetStrRsc(STR_CHANGE_CA_PIN_ID);

	switch (ePinChangeStep)
	{
		case eCX_PinChangeStep_InDlg_InputCurPincode:
			pszMsg = xmgr_cas_CxPinGetCurPincodeDlgMsg();
			break;

		case eCX_PinChangeStep_InDlg_InputNewPincode:
			pszMsg = xmgr_cas_CxPinGetNewPincodeDlgMsg();
			break;

		case eCX_PinChangeStep_InDlg_VerifyNewPincode:
			VK_memcpy32(szInputedNewPinCodeStr, pstInstance->szNewPinCodeStr, 5);
			pszMsg = xmgr_cas_CxPinGetVerifyNewPincodeDlgMsg();
			break;

		default:
			HxLOG_Print("[xmgr_cas_CxPinCreatePincode] ePinChangeStep[%x] don't create pin-code dialog \n", ePinChangeStep);
			#if 0 // not use for octo2.0
			BUS_MGR_Destroy(NULL);
			#endif
			return	ERR_FAIL;
		}

	if (pszMsg == NULL)
	{
		HxLOG_Print("[xmgr_cas_CxPinCreatePincode] can't get msg dialog \n");
		#if 0 // not use for octo2.0
		BUS_MGR_Destroy(NULL);
		#endif
		return	ERR_FAIL;
	}

#if 0 // not use for octo2.0
	xmgr_cas_CxPinCreatePincodeDlg(pstInstance, pszTitle, pszMsg);
#endif
	// szNewPinCodeStr를 사용하기 위해
	if (ePinChangeStep == eCX_PinChangeStep_InDlg_VerifyNewPincode)
	{
		VK_memcpy32(pstInstance->szNewPinCodeStr, szInputedNewPinCodeStr, 5);
	}

	return	ERR_FAIL;
}

// TODO: To Remove
#if 0 // not use for octo2.0
STATIC BUS_Result_t	xmgr_cas_CxPinCreatePincodeDlg(AP_PinDialogInstance_t* pstInstance, HUINT8 *pszTitle, HUINT8 *pszMsg)
{
	HUINT32 	ulDlgAttr = 0;
	HBOOL		bFocusEnabled = FALSE;


	ulDlgAttr = DLG_TYPE_TITLE | DLG_TYPE_MESSAGE | DLG_TIMEOUT_NOTIMEOUT ;

	AP_PinDialog_SetDialog(pstInstance, s_procParent, ulDlgAttr, pszTitle, pszMsg);
	AP_PinDialog_UpdateDlg(pstInstance);

	xmgr_cas_CxPinDlgDrawUpdate(pstInstance, bFocusEnabled);

	return	ERR_BUS_NO_ERROR;
}
#endif

#define	________________Global_Function_______________________

/*******************************************************************/
/********************      	Global Function 	********************/
/*******************************************************************/

BUS_Result_t	xproc_cas_CxPinDialog(AP_PinDialogInstance_t *pstInstance, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	s_procParent = pstInstance->parentAppCallback;

	return	xproc_cas_CxPinDialog(message, hAction, p1, p2, p3);
}

// End of file

