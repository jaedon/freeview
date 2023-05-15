/**
	@file     ap_cas_cx_pincodecx5_ui.c
	@brief    file_name & simple comment.

	Description: for CONAX Check Maturity rating Pin code UI \n
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

#include <mgr_action.h>
#include <mgr_pg.h>
#include <xmgr_cas_res_str.h>
#include <cx_common.h>
#include <conax.h>

#include "../local_include/_xmgr_cas_cx_adapt.h"
#include "../local_include/_xmgr_cas_cx_util.h"
#include "../local_include/_xmgr_cas_cx.h"


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

STATIC HUINT8	s_szPopup[256];


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/






/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef struct
{
	// Maturiy Rating Info
	HUINT8			ucCurMRLevet;
	HUINT8			ucProgMRLevel;
} CasCx_PinDialogMRinfo_t;


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC AP_PinDialogInstance_t 		s_CheckPinDialogInst;

STATIC XmgrCas_CxPinCheckStepInDlg_e	s_eCheckPinCheckStep;	// Pin code Input Step
STATIC XmgrCas_CxPinCheckEnterCaInDlg_e	s_eCheckPinEnterCA;		// Pin code Mode
STATIC BUS_Callback_t 				s_checkpin_parentApp;	// Pin code callback Func

STATIC CxUiMsgInfo_t				s_stCasCx_UiInfo;


/*******************************************************************/
/********************      	Static Prototype	********************/
/*******************************************************************/
STATIC AP_PinDialogInstance_t*	xmgr_cas_CxGetCheckPinDialogInstance(void);
STATIC HINT32		xmgr_cas_CxCheckpindlg_CheckCurrNumber(AP_PinDialogInstance_t *pstInstance, HINT32 number);
STATIC HERROR		xmgr_cas_CxGeChecktPopUpMsg_MRChange(HUINT8	*pszMsg, XmgrCas_CxPinCheckStepInDlg_e ePinCheckStep, HUINT32 ulSize);

STATIC BUS_Result_t	xmgr_cas_CxCheckgwmCheckPincode(AP_PinDialogInstance_t *pstInstance);
STATIC void			xmgr_cas_CxCheckpindlg_NotifyPinAuthorized(AP_PinDialogInstance_t *pstInstance);
STATIC void			xmgr_cas_CxCheckpindlg_NotifyPinDenied(AP_PinDialogInstance_t *pstInstance);

STATIC BUS_Result_t	xmgr_cas_CxCheckPinDialog_Proc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t	xmgr_cas_CxCheckgwmCreatePincode(AP_PinDialogInstance_t* pstInstance, XmgrCas_CxPinCheckStepInDlg_e ePinCheckStep);

STATIC void			xmgr_cas_CxCheckpindlg_Init(AP_PinDialogInstance_t *pstInstance, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC void			xmgr_cas_CxCheckpindlg_SetDlgInfo(HINT32 p1, HINT32 p2, HINT32 p3);


/*******************************************************************/
/********************      	Static Function 	********************/
/*******************************************************************/
#define _________________UI__________________________________

STATIC AP_PinDialogInstance_t*	xmgr_cas_CxGetCheckPinDialogInstance(void)
{
	return &s_CheckPinDialogInst;
}

#if 0 // not use for octo2.0
STATIC HINT32	xmgr_cas_CxCheckpindlgCheckCurrNumber(AP_PinDialogInstance_t *pstInstance, HINT32 number)
{
	if (pstInstance->nPinInputPos < 5-1)
	{
		/* set curr by inputed value */
		pstInstance->szPinCodeStr[pstInstance->nPinInputPos] = (HUINT8)('0' + number);

		/* invalidate Curr */
		GWM_APP_InvalidateObject(ALERT_PIN_CODE_ID + pstInstance->nPinInputPos);

		/* go to next */
		pstInstance->nPinInputPos++;

		/* set focus next */
		GWM_Obj_SetFocus(ALERT_PIN_CODE_ID + pstInstance->nPinInputPos);
	}

	return	pstInstance->nPinInputPos;
}
#endif

STATIC HERROR	xmgr_cas_CxGeChecktPopUpMsg_MRChange(HUINT8	*pszMsg, XmgrCas_CxPinCheckStepInDlg_e ePinCheckStep, HUINT32 ulSize)
{
	HERROR		hError = ERR_OK;
	HUINT8		*pszPopup = NULL;

	if (ePinCheckStep == eCX_PinCheckStep_InDlg_InputPincode)
	{
		pszPopup = (HUINT8*)AP_CasResStr_GetStrRsc(STR_ENTER_CA_PIN_ID);
		MWC_UTIL_DvbStrncpy(pszMsg, pszPopup, ulSize);
	}
	else if (ePinCheckStep == eCX_PinCheckStep_InDlg_WrongPincode)
	{
		pszPopup = (HUINT8*)AP_CasResStr_GetStrRsc(STR_MESG_398_ID);
		MWC_UTIL_DvbStrncpy(pszMsg, pszPopup, ulSize);
	}
	else
	{
		HxLOG_Print("\t MRChange - ePinCheckStep[%x] don't create pin-code dialog \n", ePinCheckStep);
		hError = ERR_FAIL;
	}

	return	hError;
}


#define _________________Message_____________________________

STATIC void	xmgr_cas_CxCheckpindlg_NotifyPinAuthorized(AP_PinDialogInstance_t *pstInstance)
{
	// Save CAS Pin-code
	xmgr_cas_CxSetAcceptCAPincode(pstInstance->szPinCodeStr);

	// Set PPV Mgr Element
	xmgr_cas_CxSetPpvMgrAction(s_stCasCx_UiInfo.eUiMsgType, pstInstance->szPinCodeStr);

#if 0 // not use for octo2.0
	BUS_MGR_Destroy(0);
#endif
	BUS_SendMessage(pstInstance->parentAppCallback, eMEVT_PG_PIN_CHECK_OK, (int)NULL, 0, (int)pstInstance->ulPinDlgWhoseId, (int)pstInstance->thisAppCallback);
}


STATIC void	xmgr_cas_CxCheckpindlg_NotifyPinDenied(AP_PinDialogInstance_t *pstInstance)
{
	/* 없애고나서 메시지 보내야 함. 순서 지킬 것... */
#if 0 // not use for octo2.0
	BUS_MGR_Destroy(0);
#endif
	BUS_SendMessage(pstInstance->parentAppCallback, eMEVT_PG_PIN_CHECK_DENIED, (int)NULL, 0, (int)pstInstance->ulPinDlgWhoseId, (int)pstInstance->thisAppCallback);
}

HERROR xmgr_cas_CxTEST_SetPINCode()
{
	AP_PinDialogInstance_t *pstInstance;
	pstInstance = xmgr_cas_CxGetCheckPinDialogInstance();
	HxSTD_snprintf(pstInstance->szPinCodeStr, sizeof(pstInstance->szPinCodeStr), (HUINT8*)"1234");

	s_eCheckPinEnterCA = eCX_PinCheckEnter_TokenDebit;
	s_stCasCx_UiInfo.eUiMsgType = eCxUi_GroupPpv_TokenPpv;
	xmgr_cas_CxCheckpindlg_NotifyPinAuthorized(pstInstance);
	//xmgr_cas_CxCheckgwmCheckPincode(pstInstance);
}
STATIC BUS_Result_t	xmgr_cas_CxCheckgwmCheckPincode(AP_PinDialogInstance_t *pstInstance)
{
	HERROR		hError = ERR_FAIL;
	HUINT32		ulActionId;
	Handle_t	hMasterSvcHandle = HANDLE_NULL;

	ulActionId = MGR_ACTION_GetMainActionId();
	hError = MGR_ACTION_GetMasterSvcHandle(ulActionId, &hMasterSvcHandle);
	if (hError != ERR_OK)
	{
		hMasterSvcHandle = HANDLE_NULL;
		HxLOG_Critical("\n\n");
	}

	if (s_eCheckPinEnterCA == eCX_PinCheckEnter_MR_CHG)
	{
		hError = xmgr_cas_CxChangeMaturityRating(CX_SC_SLOT_0, s_stCasCx_UiInfo.mr_ucSmartcard, pstInstance->szPinCodeStr);
	}
	else
	{
		hError = xmgr_cas_CxCheckSmartCardPin(s_stCasCx_UiInfo.eUiMsgType, hMasterSvcHandle, pstInstance->szPinCodeStr);
	}

	if (hError != ERR_OK)
	{
		s_eCheckPinCheckStep = eCX_PinCheckStep_InDlg_WrongPincode;
		xmgr_cas_CxCheckgwmCreatePincode(pstInstance , s_eCheckPinCheckStep);
	}
	else
	{
#if 0 // not use for octo2.0
		BUS_MGR_Destroy(NULL);
#endif
		xmgr_cas_CxCheckpindlg_NotifyPinAuthorized(pstInstance);
	}

	return	MESSAGE_BREAK;
}


#define	________________Init_Func____________________________

STATIC BUS_Result_t	xmgr_cas_CxCheckgwmCreatePincode(AP_PinDialogInstance_t* pstInstance, XmgrCas_CxPinCheckStepInDlg_e ePinCheckStep)
{
	HERROR		hError = ERR_OK;
	HUINT32 	ulDlgAttr;
	HUINT8		*pszPopupTitle = NULL;


	HxSTD_memset(s_szPopup, 0, 256);
	HxSTD_memset (pstInstance, 0, sizeof(AP_PinDialogInstance_t));

	ulDlgAttr = (/* DLG_TYPE_TITLE| */DLG_TYPE_MESSAGE|DLG_TIMEOUT_NOTIMEOUT);
	pszPopupTitle = (HUINT8*)AP_CasResStr_GetStrRsc(STR_CHANGE_MATURITY_RATING_ID);

	switch (s_eCheckPinEnterCA)
	{
		case eCX_PinCheckEnter_AcceptView:
		case eCX_PinCheckEnter_MR:
		case eCX_PinCheckEnter_TokenDebit:
			hError = xmgr_cas_CxGetCheckData((HINT32)&s_stCasCx_UiInfo, s_szPopup, (HINT32)ePinCheckStep, 256);
			break;

		case eCX_PinCheckEnter_MR_CHG:
			hError = xmgr_cas_CxGeChecktPopUpMsg_MRChange(s_szPopup, ePinCheckStep, 256);
			break;

		default:
			HxLOG_Critical("\n\n");
			hError = ERR_FAIL;
			break;
	}

	if (hError != ERR_OK)
	{
#if 0 // not use for octo2.0
		BUS_MGR_Destroy(NULL);
#endif
		return	ERR_BUS_NO_ERROR;
	}

	HxLOG_Print("\t Dialog title [%s]\n", s_szPopup);
#if 0 // not use for octo2.0
	AP_PinDialog_SetDialog(pstInstance, s_checkpin_parentApp, ulDlgAttr, NULL, s_szPopup);
	AP_PinDialog_UpdateDlg(pstInstance);
#endif
	return	ERR_BUS_NO_ERROR;
}


STATIC BUS_Result_t	xmgr_cas_CxCheckPinDialog_Proc(HINT32 message, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t			busResult = ERR_BUS_NO_ERROR;
	AP_PinDialogInstance_t	*pstInstance = NULL;

	pstInstance = xmgr_cas_CxGetCheckPinDialogInstance();

	switch (message)
	{
		case eMEVT_BUS_CREATE :
			s_eCheckPinCheckStep = (XmgrCas_CxPinCheckStepInDlg_e)p1;
			xmgr_cas_CxCheckpindlg_SetDlgInfo(p1, p2, p3);
			busResult = xmgr_cas_CxCheckgwmCreatePincode(pstInstance, s_eCheckPinCheckStep);
			break;

		case eMEVT_BUS_TIMER:
			break;
		// TODO: Blocking
		#if 0 // not use for octo2.0
		case MSG_GWM_KEYUP:
			AP_KeyConv_UiCommon(&p1);
			break;

		case MSG_GWM_KEYDOWN:
			AP_KeyConv_UiCommon(&p1);
			switch (p1)
			{
				/* keys to pin quit */
				case KEY_BACK:
				case KEY_EXIT:
					BUS_MGR_Destroy(NULL);
					return MESSAGE_BREAK;

				/* keys to ignore */
				case KEY_ARROWRIGHT:
				case KEY_ARROWUP:
				case KEY_ARROWDOWN:
				case KEY_RED:
				case KEY_BLUE:
				case KEY_GREEN:
				case KEY_YELLOW:
					return MESSAGE_BREAK;

#if 0 /* CPP가 지우지 말란다... */
				/* go back */
				case KEY_ARROWLEFT:
					local_pindlg_BackspacePosition(pstInstance);
					return MESSAGE_BREAK;
#else
				case KEY_ARROWLEFT:
					return MESSAGE_BREAK;
#endif
				case KEY_CH_MINUS:
				case KEY_CH_PLUS:
					BUS_MGR_Destroy(NULL);
					break;

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
						return MESSAGE_BREAK;
					}

					if (xmgr_cas_CxCheckpindlgCheckCurrNumber(pstInstance, (p1 - KEY_0)) >= 5-1)
					{
						GWM_DirectRedrawAll();

						xmgr_cas_CxCheckgwmCheckPincode(pstInstance);
					}
					return MESSAGE_BREAK;

				default:
					break;
			}
			break;
		#endif
		case eMEVT_BUS_DESTROY:

			break;

	}

	if ((busResult == MESSAGE_BREAK) || (busResult == MESSAGE_PASS))
	{
		return	busResult;
	}

	return	BUS_ProcessMessageDefault(message, handle, p1, p2, p3);
}


STATIC void	xmgr_cas_CxCheckpindlg_Init(AP_PinDialogInstance_t *pstInstance, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XMGR_CAS_CX_PARAM_WARRING_REMOVE(hAction, p1, p2, p3);

	if (message == eMEVT_BUS_CREATE)
	{
		HxSTD_memset(&	s_stCasCx_UiInfo, 0, sizeof(CxUiMsgInfo_t));

		s_checkpin_parentApp = (BUS_Callback_t)pstInstance->parentAppCallback;
		s_eCheckPinEnterCA = (XmgrCas_CxPinCheckEnterCaInDlg_e)pstInstance->ulDlgAttributes;
	}
}


STATIC void	xmgr_cas_CxCheckpindlg_SetDlgInfo(HINT32 p1, HINT32 p2, HINT32 p3)
{
	CxUiMsgInfo_t	*stCxUiInfo = NULL;

	XMGR_CAS_CX_PARAM_UNUSED(p1);
	XMGR_CAS_CX_PARAM_UNUSED(p3);

	/*
		p1 : Step Info
		p2 : Data
	*/
	switch (s_eCheckPinEnterCA)
	{
		case	eCX_PinCheckEnter_MR_CHG:
			s_stCasCx_UiInfo.mr_ucSmartcard = (HUINT8)p2;
			break;

		case	eCX_PinCheckEnter_MR:
		case	eCX_PinCheckEnter_AcceptView:
		case	eCX_PinCheckEnter_TokenDebit:
			stCxUiInfo = (CxUiMsgInfo_t*)p2;
			if (stCxUiInfo != NULL)
			{
				memcpy(&s_stCasCx_UiInfo, stCxUiInfo, sizeof(CxUiMsgInfo_t));
			}
			break;

		default:
			HxLOG_Critical("\n\n");
			break;
	}
}


#define	__________Gobal_Function______________________________

/*******************************************************************/
/********************      	Global Function 	********************/
/*******************************************************************/

/**
	@brief     XPROC_CAS_CX_CheckPinDialog
	Pin-code Check UI. 		\n
	for example, 							\n
	@brief	p1 - XmgrCas_CxPinCheckStepInDlg_e	\n
			p2 - structure of CxUiMsgInfo_t	\n
			p3 - 0
	@return .
  */
BUS_Result_t	xproc_cas_CxCheckPinDialog(AP_PinDialogInstance_t *pstInstance, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgr_cas_CxCheckpindlg_Init(pstInstance, message, hAction, p1, p2, p3);

	return	xmgr_cas_CxCheckPinDialog_Proc(message, hAction, p1, p2, p3);
}

// End of file

