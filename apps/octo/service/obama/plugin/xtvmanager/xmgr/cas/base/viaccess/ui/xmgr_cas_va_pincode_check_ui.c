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
//#include <gwm.h>
//#include <osd_gfx.h>

//#include <ray_popupui_coordinate.h>
//#include <menuui_coor.h>
//#include <popupui_coor.h>
#include <mgr_pg.h>
#include <mgr_action.h>

#include "va_ui_adapt.h"

#include "../local_include/_xmgr_cas_va_util.h"
#include "../local_include/_xmgr_cas_va_ui.h"
#include "_xmgr_cas_va_pincode_ui_coordinate.h"
#include <xmgr_cas_res_str.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
#if defined(CONFIG_DEBUG)
HUINT32	g_CasVa_PinCheck_debug_level = DBG_ASSERT;
#endif


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#if defined(CONFIG_DEBUG)
#define	ApCasVaPinCheckPrint(level, message)			(((level) & g_CasVa_PinCheck_debug_level) ? ((VK_Print("==[AP:CAS_VA_CHECKPIN_UI]==")), (VK_Print message)) : 0)
#define	ApCasVaPinCheckAssert(c) 					((c) ? 0 : (VK_Print("\==[CAS_VA_CHECKPIN_UI]== assertion on %s: line%d\n\n", __FILE__, __LINE__)))
#define	ApCasVaPinCheckData(level, message)			(((level) & g_CasVa_PinCheck_debug_level) ? ( (VK_Print message)) : 0)
#else
#define	ApCasVaPinCheckPrint(level, message)
#define	ApCasVaPinCheckAssert(c)
#define	ApCasVaPinCheckData(level, message)
#endif


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

STATIC HUINT32						s_CheckPinDlgInstance; // hjlee3, just for build
STATIC eCasVa_PinCheckStep_InDlg	s_eCheckVaPincodeCheckStep;
STATIC eCasVa_PinCheckEnterCA_InDlg	s_eCheckPincodeEnterCA;
STATIC BUS_Callback_t 				s_checkpin_parentApp;	// Pin code callback Func
STATIC VaUiMsgInfo_t				s_stCasVa_UiInfo;
STATIC HUINT8	s_szPinDialogMessage[TEXT_SIZE512];
STATIC HUINT32	s_ulAcsId;
STATIC HUINT32	s_ulStreamHandle;

/*******************************************************************/
/********************      	Static Prototype	********************/
/*******************************************************************/

STATIC HUINT32*	local_cas_va_GetCheckPinDialogInstance(void); // hjlee3, just for build
STATIC HINT32					local_cas_va_Checkpindlg_CheckCurrNumber (HUINT32 *pstInstance, HINT32 number); // hjlee3, just for build
STATIC BUS_Result_t				local_cas_va_ShowCheckPinCodeDlg(HUINT32* pstInstance, eCasVa_PinCheckStep_InDlg ePinCheckStep); // hjlee3, just for build
STATIC HERROR					local_cas_va_PinDlgUnlockConsultation(HUINT8 *pszMsg, HUINT8 ucRemainAttemps, eCasVa_PinCheckStep_InDlg ePinCheckStep);
STATIC HERROR					local_cas_va_PinDlgChangeMaturityRate(HUINT8 *pszMsg, HUINT8 ucRemainAttemps, eCasVa_PinCheckStep_InDlg ePinCheckStep);
STATIC BUS_Result_t				local_cas_va_CheckPinCodeSmartcard(HUINT32 *pstInstance); // hjlee3, just for build
STATIC BUS_Result_t				local_cas_va_CheckPinCodeProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC void						local_cas_va_Checkpindlg_Init(HUINT32 *pstInstance, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3); //  hjlee3, just for build
STATIC void						local_cas_va_Checkpindlg_SetDlgInfo(HINT32 p1, HINT32 p2, HINT32 p3);
STATIC HERROR					local_cas_va_SetScreenSaver(HINT32 message, HINT32 p1);

/*******************************************************************/
/********************      	Static Function 	********************/
/*******************************************************************/
#define _________________UI__________________________________

#if 0
STATIC HUINT32*	local_cas_va_GetCheckPinDialogInstance(void)
{
	return &s_CheckPinDlgInstance;
}
#endif

STATIC HINT32	local_cas_va_Checkpindlg_CheckCurrNumber(HUINT32 *pstInstance, HINT32 number)
{
#if 0 // not use for octo2.0
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
#endif
}

STATIC HERROR	local_cas_va_PinDlgUnlockConsultation(HUINT8 *pszMsg, HUINT8 ucRemainAttemps, eCasVa_PinCheckStep_InDlg ePinCheckStep)
{
	HERROR hError = ERR_OK;
	HUINT8 *pszStr = NULL;

	//pszStr = GetTextBuffer(TEXT_SIZE64);
	HUINT8	szStr[TEXT_SIZE64];
	pszStr = szStr;

	switch(ePinCheckStep)
	{
		case eVA_PinCheckStep_InDlg_InputPincode:
			// 모든 String 관리는 Web에서 처리하는 것으로 약속됨에 따라 AP_CasResStr_GetStrRsc() 함수를 더이상 사용하지 말아야 함.
			// 따라서 LOC ID를 Web으로 전송 후 Web에서 그에 맞는 string을 찾아 출력하도록 함.
			todo
			snprintf(pszStr, TEXT_SIZE64, AP_CasResStr_GetStrRsc(STR_MESG_1129_ID), ucRemainAttemps);
			MWC_UTIL_DvbUtf8Sprintf(pszMsg, "%s\n%s\n%s.", AP_CasResStr_GetStrRsc(STR_MESG_1101_ID), AP_CasResStr_GetStrRsc(STR_MESG_1102_ID), pszStr);
			break;

		case eVA_PinCheckStep_InDlg_WrongPincode:
			snprintf(pszStr, TEXT_SIZE64, AP_CasResStr_GetStrRsc(STR_MESG_1129_ID), ucRemainAttemps);
			MWC_UTIL_DvbUtf8Sprintf(pszMsg, "%s\n%s\n%s\n%s.", AP_CasResStr_GetStrRsc(STR_WRONG_PIN_CODE_ID), AP_CasResStr_GetStrRsc(STR_MESG_1101_ID), AP_CasResStr_GetStrRsc(STR_MESG_1102_ID), pszStr);
			break;

		case eVA_PinCheckStep_InDlg_NoPincode:
			snprintf(pszStr, TEXT_SIZE64, AP_CasResStr_GetStrRsc(STR_MESG_1129_ID), ucRemainAttemps);
			MWC_UTIL_DvbUtf8Sprintf(pszMsg, "%s\n%s\n%s\n%s.", AP_CasResStr_GetStrRsc(STR_MESG_1140_ID), AP_CasResStr_GetStrRsc(STR_MESG_1101_ID), AP_CasResStr_GetStrRsc(STR_MESG_1102_ID), pszStr);
			break;

		default:
			ApCasVaPinCheckPrint(DBG_ASSERT, ("Unknown Pincode Type (%d)\n", ePinCheckStep));
			hError = ERR_FAIL;
			break;
	}

	return hError;
}

STATIC HERROR	local_cas_va_PinDlgChangeMaturityRate(HUINT8 *pszMsg, HUINT8 ucRemainAttemps, eCasVa_PinCheckStep_InDlg ePinCheckStep)
{
	HERROR		hError = ERR_OK;
	HUINT8		*pszStr = NULL;

	//pszStr = GetTextBuffer(TEXT_SIZE64);
	HUINT8	szStr[TEXT_SIZE64];
	pszStr = szStr;


	switch(ePinCheckStep)
	{
		case eVA_PinCheckStep_InDlg_InputPincode:
			// 모든 String 관리는 Web에서 처리하는 것으로 약속됨에 따라 AP_CasResStr_GetStrRsc() 함수를 더이상 사용하지 말아야 함.
			// 따라서 LOC ID를 Web으로 전송 후 Web에서 그에 맞는 string을 찾아 출력하도록 함.
			todo
			snprintf(pszStr, TEXT_SIZE64, AP_CasResStr_GetStrRsc(STR_MESG_1129_ID), ucRemainAttemps);
			MWC_UTIL_DvbUtf8Sprintf(pszMsg, "%s\n%s.", AP_CasResStr_GetStrRsc(STR_MESG_269_ID), pszStr);
			break;

		case eVA_PinCheckStep_InDlg_WrongPincode:
			snprintf(pszStr, TEXT_SIZE64, AP_CasResStr_GetStrRsc(STR_MESG_1129_ID), ucRemainAttemps);
			MWC_UTIL_DvbUtf8Sprintf(pszMsg, "%s\n%s\n%s.", AP_CasResStr_GetStrRsc(STR_WRONG_PIN_CODE_ID), AP_CasResStr_GetStrRsc(STR_MESG_269_ID), pszStr);
			break;

		case eVA_PinCheckStep_InDlg_NoPincode:
			snprintf(pszStr, TEXT_SIZE64, AP_CasResStr_GetStrRsc(STR_MESG_1129_ID), ucRemainAttemps);
			MWC_UTIL_DvbUtf8Sprintf(pszMsg, "%s\n%s\n%s.", AP_CasResStr_GetStrRsc(STR_MESG_1140_ID), AP_CasResStr_GetStrRsc(STR_MESG_269_ID), pszStr);
			break;

		default:
			ApCasVaPinCheckPrint(DBG_ASSERT, ("Unknown Pincode Type (%d)\n", ePinCheckStep));
			hError = ERR_FAIL;
			break;
	}

	return	hError;
}

STATIC HERROR	local_cas_va_PinDlgCheckMaturityRate(HUINT8 *pszMsg, HUINT8 ucMaturityRate, HUINT8 ucRemainAttemps, eCasVa_PinCheckStep_InDlg ePinCheckStep)
{
	HERROR	hError = ERR_OK;
	HUINT8	*pszStr1 = NULL, *pszStr2 = NULL;
	HUINT8	szStr1[256];
	HUINT8	szStr2[256];
#if defined(CONFIG_OP_NORDIG_BOXER)
	HUINT32	ulBoxerModelId = xmgr_cas_VaAdaptGetBoxerModel_ID();
	HUINT32	ulBoxerErrCode_MR = 14;	// refer eVA_UI_DESCRAMBLING_KO_MATURITY_RATING_EVENT, xmgr_cas_VaAdaptMessageFunction()
#endif

	//pszStr1 = GetTextBuffer(256);
	//pszStr2 = GetTextBuffer(256);
	pszStr1 = szStr1;
	pszStr2 = szStr2;

	VK_memset32(pszStr1, 0, 256);
	VK_memset32(pszStr2, 0, 256);

	switch(ePinCheckStep)
	{
		case eVA_PinCheckStep_InDlg_InputPincode:
#if defined(CONFIG_OP_NORDIG_BOXER)
			// 모든 String 관리는 Web에서 처리하는 것으로 약속됨에 따라 AP_CasResStr_GetStrRsc() 함수를 더이상 사용하지 말아야 함.
			// 따라서 LOC ID를 Web으로 전송 후 Web에서 그에 맞는 string을 찾아 출력하도록 함.
			todo
			MWC_UTIL_DvbUtf8Sprintf(pszStr1, AP_CasResStr_GetStrRsc(STR_MESG_2017_ID), ucMaturityRate);
			MWC_UTIL_DvbUtf8Sprintf(pszStr2, AP_CasResStr_GetStrRsc(STR_MESG_1129_ID), ucRemainAttemps);
			MWC_UTIL_DvbUtf8Sprintf(pszMsg, "%s\n%s.\nid : %03d %03d", pszStr1, pszStr2, ulBoxerModelId, ulBoxerErrCode_MR);
#else
			snprintf(pszStr1, 256, AP_CasResStr_GetStrRsc(STR_MESG_1146_ID), ucMaturityRate);
			snprintf(pszStr2, 256, AP_CasResStr_GetStrRsc(STR_MESG_1129_ID), ucRemainAttemps);
			MWC_UTIL_DvbUtf8Sprintf(pszMsg, "%s\n%s\n%s", pszStr1, AP_CasResStr_GetStrRsc(STR_MESG_1147_ID), pszStr2);
#endif
			break;

		case eVA_PinCheckStep_InDlg_WrongPincode:
#if defined(CONFIG_OP_NORDIG_BOXER)
			MWC_UTIL_DvbUtf8Sprintf(pszStr1, AP_CasResStr_GetStrRsc(STR_MESG_2017_ID), ucMaturityRate);
			MWC_UTIL_DvbUtf8Sprintf(pszStr2, AP_CasResStr_GetStrRsc(STR_MESG_1129_ID), ucRemainAttemps);
			MWC_UTIL_DvbUtf8Sprintf(pszMsg, "%s\n%s\n%s.\nid : %03d %03d", AP_CasResStr_GetStrRsc(STR_WRONG_PIN_CODE_ID), pszStr1, pszStr2, ulBoxerModelId, ulBoxerErrCode_MR);
#else
			snprintf(pszStr1, 256, AP_CasResStr_GetStrRsc(STR_MESG_1146_ID), ucMaturityRate);
			snprintf(pszStr2, 256, AP_CasResStr_GetStrRsc(STR_MESG_1129_ID), ucRemainAttemps);
			MWC_UTIL_DvbUtf8Sprintf(pszMsg, "%s\n%s\n%s\n%s", AP_CasResStr_GetStrRsc(STR_WRONG_PIN_CODE_ID), pszStr1, AP_CasResStr_GetStrRsc(STR_MESG_1147_ID), pszStr2);
#endif
			break;

		case eVA_PinCheckStep_InDlg_NoPincode:
#if defined(CONFIG_OP_NORDIG_BOXER)
			MWC_UTIL_DvbUtf8Sprintf(pszStr1, AP_CasResStr_GetStrRsc(STR_MESG_2017_ID), ucMaturityRate);
			MWC_UTIL_DvbUtf8Sprintf(pszStr2, AP_CasResStr_GetStrRsc(STR_MESG_1129_ID), ucRemainAttemps);
			MWC_UTIL_DvbUtf8Sprintf(pszMsg, "%s\n%s\n%s.\nid : %03d %03d", AP_CasResStr_GetStrRsc(STR_MESG_1140_ID), pszStr1, pszStr2, ulBoxerModelId, ulBoxerErrCode_MR);
#else
			snprintf(pszStr1, 256, AP_CasResStr_GetStrRsc(STR_MESG_1146_ID), ucMaturityRate);
			snprintf(pszStr2, 256, AP_CasResStr_GetStrRsc(STR_MESG_1129_ID), ucRemainAttemps);
			MWC_UTIL_DvbUtf8Sprintf(pszMsg, "%s\n%s\n%s\n%s", AP_CasResStr_GetStrRsc(STR_MESG_1140_ID), pszStr1, AP_CasResStr_GetStrRsc(STR_MESG_1147_ID), pszStr2);
#endif
			break;

		default:
			hError = ERR_FAIL;
			break;
	}

	return hError;
}

#define _________________Message_____________________________


STATIC BUS_Result_t	local_cas_va_CheckPinCodeSmartcard(HUINT32 *pstInstance)
{
	HERROR		hError = ERR_FAIL;
	HUINT32		ulActionId;
	Handle_t	hMasterSvcHandle = HANDLE_NULL;
	HINT32		message, nResult;

	ulActionId = MGR_ACTION_GetMainActionId();
	hError = MGR_ACTION_GetMasterSvcHandle(ulActionId, &hMasterSvcHandle);
	if (hError != ERR_OK)
	{
		hMasterSvcHandle = HANDLE_NULL;
		ApCasVaPinCheckAssert(0);
	}

	switch(s_eCheckPincodeEnterCA)
	{
		case eVA_PinCheckEnter_UnlockConsultation:
// hjlee3, just for compile			hError = AP_CAS_VA_UnlockConsultation(pstInstance->szPinCodeStr, &nResult);
			break;

		case eVA_PinCheckEnter_AgreeMaturityRate:
// hjlee3, just for compile			hError = AP_CAS_VA_SetMaturityRateValue(s_ulAcsId, pstInstance->szPinCodeStr, &nResult);
			break;

		case eVA_PinCheckEnter_ModifyMaturityRate:
// hjlee3, just for compile			hError = AP_CAS_VA_ChangeMaturityRate(pstInstance->szPinCodeStr, s_stCasVa_UiInfo.mr_ucSmartcard, &nResult);
			break;

		default:
			break;
	}

	if(hError == ERR_OK)
	{
// hjlee3, just for compile		BUS_SendMessage(pstInstance->parentAppCallback, eMEVT_PG_PIN_CHECK_OK, (int)NULL, 0, (int)pstInstance->ulPinDlgWhoseId, (int)pstInstance->thisAppCallback);
	}
	else
	{
		switch(nResult)
		{
			case VA_ERR_UI_WRONG_PIN_CODE:
				s_eCheckVaPincodeCheckStep = eVA_PinCheckStep_InDlg_WrongPincode;
				break;

			case VA_ERR_UI_NO_PIN_CODE:
				s_eCheckVaPincodeCheckStep = eVA_PinCheckStep_InDlg_NoPincode;
				break;

			default:
				s_eCheckVaPincodeCheckStep = eVA_PinCheckStep_InDlg_WrongPincode;
				break;
		}

		local_cas_va_ShowCheckPinCodeDlg(pstInstance, s_eCheckVaPincodeCheckStep);
	}

	return	MESSAGE_BREAK;
}


#define	________________Init_Func____________________________

STATIC BUS_Result_t	local_cas_va_ShowCheckPinCodeDlg(HUINT32* pstInstance, eCasVa_PinCheckStep_InDlg ePinCheckStep)
{
	HERROR		hError = ERR_OK;
	HUINT32 	ulDlgAttr;
	HUINT8		*pszTitle = NULL, *pszMsg = NULL;
	HUINT8		ucRemainCnt, ucMaxCnt, ucMaturityRate;
	HINT32		nResult;

	if(ePinCheckStep == eVA_PinCheckStep_InDlg_NoPincode)
	{
		BUS_MGR_Destroy(NULL);
// hjlee3, just for compile		BUS_SendMessage(pstInstance->parentAppCallback, eMEVT_PG_PIN_CHECK_DENIED, (int)NULL, (HINT32)eVA_PinChangeError_NoPincode, (int)pstInstance->ulPinDlgWhoseId, (int)pstInstance->thisAppCallback);
		return	ERR_BUS_NO_ERROR;
	}

	HxSTD_memset (pstInstance, 0, sizeof(HUINT32));

	AP_CAS_VA_GetPinCodeAttemps(&ucRemainCnt, &ucMaxCnt, &nResult);

	if(nResult == ERR_OK && ucRemainCnt == 0)
	{
		ApCasVaPinCheckData(DBG_ASSERT, ("fail to attemps count, error (%d), attemps (%d)\n", nResult, ucRemainCnt));

		BUS_MGR_Destroy(NULL);
// hjlee3, just for compile		BUS_SendMessage(pstInstance->parentAppCallback, eMEVT_PG_PIN_CHECK_DENIED, (int)NULL, (HINT32)eVA_PinChangeError_ScLocked, (int)pstInstance->ulPinDlgWhoseId, (int)pstInstance->thisAppCallback);
		return	ERR_BUS_NO_ERROR;
	}

	if(hError == ERR_OK)
	{
		pszMsg = s_szPinDialogMessage;

		VK_memset32(pszMsg, 0, TEXT_SIZE512);

		switch (s_eCheckPincodeEnterCA)
		{
			case eVA_PinCheckEnter_UnlockConsultation:
				hError = local_cas_va_PinDlgUnlockConsultation(pszMsg, ucRemainCnt, ePinCheckStep);
				break;

			case eVA_PinCheckEnter_AgreeMaturityRate:
				AP_CAS_VA_GetMaturityRateValue(s_ulAcsId, s_ulStreamHandle, &ucMaturityRate, &nResult);
				hError = local_cas_va_PinDlgCheckMaturityRate(pszMsg, ucMaturityRate, ucRemainCnt, ePinCheckStep);
				break;

			case eVA_PinCheckEnter_ModifyMaturityRate:
				hError = local_cas_va_PinDlgChangeMaturityRate(pszMsg, ucRemainCnt, ePinCheckStep);
				break;

			default:
				hError = ERR_FAIL;
				break;
		}
	}

	if (hError != ERR_OK)
	{
		BUS_MGR_Destroy(NULL);
		return	ERR_BUS_NO_ERROR;
	}
#if 0 // not use for octo2.0
	ulDlgAttr = DLG_TYPE_MESSAGE | DLG_TIMEOUT_NOTIMEOUT;

	AP_PinDialog_SetDialog(pstInstance, s_checkpin_parentApp, ulDlgAttr, NULL, pszMsg);
	AP_PinDialog_UpdateDlg(pstInstance);
#endif
	return	ERR_BUS_NO_ERROR;
}


STATIC BUS_Result_t	local_cas_va_CheckPinCodeProc(HINT32 message, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t			gwmResult = ERR_BUS_NO_ERROR;
	HUINT32	*pstInstance = NULL;

	pstInstance = local_cas_va_GetCheckPinDialogInstance();
#if 0 // not use for octo2.0
	switch (message)
	{
		case eMEVT_BUS_CREATE :
			local_cas_va_Checkpindlg_SetDlgInfo(p1, p2, p3);
			gwmResult = local_cas_va_ShowCheckPinCodeDlg(pstInstance, s_eCheckVaPincodeCheckStep);
			local_cas_va_SetScreenSaver(message, p1);
			break;

		case MSG_GWM_STACK_CHANGED:
			local_cas_va_SetScreenSaver(message, p1);
			break;

		case MSG_GWM_TIMER:
			if(local_cas_va_SetScreenSaver(message, p1) == ERR_OK)
				return MESSAGE_BREAK;
			break;

		case MSG_GWM_KEYUP:
			AP_KeyConv_UiCommon(&p1);
			break;

		case eSEVT_CH_NO_SIGNAL:
			BUS_MGR_Destroy(NULL);
			break;

		case MSG_GWM_KEYDOWN:
#if defined(CONFIG_OP_NTVPLUS)
			AP_KeyConv_MenuRoot(&p1);
#else
			AP_KeyConv_UiCommon(&p1);
#endif
			switch (p1)
			{
				/* keys to pin quit */
				case KEY_BACK:
// 아래 코드가 있을 경우,  MR이 다시 들어오면 메시지가 뜨지를 않아요..
//					if(s_eCheckPincodeEnterCA == eVA_PinCheckEnter_AgreeMaturityRate)
//					{
//						BUS_SendMessage(pstInstance->parentAppCallback, MSG_GWM_QUIT_UI, (int)NULL, 0, (int)pstInstance->ulPinDlgWhoseId, (int)pstInstance->thisAppCallback);
//					}
					BUS_MGR_Destroy(NULL);
					return MESSAGE_BREAK;

				case KEY_MENU:
					if(s_eCheckPincodeEnterCA == eVA_PinCheckEnter_AgreeMaturityRate)
					{
//						BUS_SendMessage(pstInstance->parentAppCallback, MSG_GWM_QUIT_UI, (int)NULL, 0, (int)pstInstance->ulPinDlgWhoseId, (int)pstInstance->thisAppCallback);
						BUS_MGR_Destroy(NULL);
						BUS_MGR_Create("AP_Portal_Proc", APP_DEFAULT_PRIORITY, AP_Portal_Proc, NULL, 0, 0, 0);
						return MESSAGE_BREAK;
					}

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
					/* pstInstance->szPinCodeStr[5] is pincode value user input */
					if (pstInstance->ulDlgAttributes & DLG_TYPE_BLKPINCODE)
					{
						return MESSAGE_BREAK;
					}

					if (local_cas_va_Checkpindlg_CheckCurrNumber(pstInstance, (p1 - KEY_0)) >= 5-1)
					{
						GWM_DirectRedrawAll();
						local_cas_va_CheckPinCodeSmartcard(pstInstance);
					}

					return MESSAGE_BREAK;

				default:
					break;
			}
			break;

		case MSG_APP_LANGUAGE_CHANGED :
			gwmResult = local_cas_va_ShowCheckPinCodeDlg(pstInstance, s_eCheckVaPincodeCheckStep);
			break;

		case MSG_GWM_DESTROY:
			local_cas_va_SetScreenSaver(message, p1);
			break;

	}

	if ((gwmResult == MESSAGE_BREAK) || (gwmResult == MESSAGE_PASS))
	{
		return	gwmResult;
	}
#endif
	return	BUS_ProcessMessageDefault(message, handle, p1, p2, p3);
}


STATIC void	local_cas_va_Checkpindlg_Init(HUINT32 *pstInstance, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	AP_CAS_VA_PARAM_WARRING_REMOVE(hAction, p1, p2, p3);

	if (message == eMEVT_BUS_CREATE)
	{
		HxSTD_memset(&	s_stCasVa_UiInfo, 0, sizeof(VaUiMsgInfo_t));
// hjlee3, just for compile		s_checkpin_parentApp = (BUS_Callback_t)pstInstance->parentAppCallback;
// hjlee3, just for compile		s_eCheckPincodeEnterCA = (eCasVa_PinCheckEnterCA_InDlg)pstInstance->ulDlgAttributes;
	}
}


STATIC void	local_cas_va_Checkpindlg_SetDlgInfo(HINT32 p1, HINT32 p2, HINT32 p3)
{
	s_eCheckVaPincodeCheckStep = (eCasVa_PinCheckStep_InDlg)p1;

	switch (s_eCheckPincodeEnterCA)
	{
		case eVA_PinCheckEnter_ModifyMaturityRate:
			s_stCasVa_UiInfo.mr_ucSmartcard = (HUINT8)p2;
			break;

		case eVA_PinCheckEnter_AgreeMaturityRate:
			s_ulAcsId = (HUINT32)p2;
			s_ulStreamHandle = (HUINT32)p3;
			break;

		default:
			break;
	}
}


STATIC HERROR local_cas_va_SetScreenSaver(HINT32 message, HINT32 p1)
{
	if(s_eCheckPincodeEnterCA != eVA_PinCheckEnter_AgreeMaturityRate)
	{
		return ERR_FAIL;
	}

	if(message == eMEVT_BUS_TIMER && p1 != SCREEN_SAVER_TIMER_ID)
	{
		return ERR_FAIL;
	}

	AP_CAS_VA_SetScreenSaver(message);

	return ERR_OK;
}


#define	__________Gobal_Function______________________________

/*******************************************************************/
/********************      	Global Function 	********************/
/*******************************************************************/

/**
	@brief     AP_CAS_VA_CheckPinDialog_Proc
	Pin-code Check UI. 		\n
	for example, 							\n
	@brief	p1 - eCasVa_PinCheckStep_InDlg	\n
			p2 - structure of CxUiMsgInfo_t	\n
			p3 - 0
	@return .
  */
#if 0 // hjlee3
BUS_Result_t	AP_CAS_VA_CheckPinDialog_Proc(HUINT32 *pstInstance, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	local_cas_va_Checkpindlg_Init (pstInstance, message, hAction, p1, p2, p3);

	return	local_cas_va_CheckPinCodeProc(message, hAction, p1, p2, p3);
}
#endif
// End of file
