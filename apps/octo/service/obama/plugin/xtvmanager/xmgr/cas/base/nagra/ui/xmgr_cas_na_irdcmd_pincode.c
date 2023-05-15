/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <mgr_live.h>
#include <mgr_cas.h>

#include <xmgr_cas_res_str.h>
#include <na_mgr_cak_ext.h>

#include <namgr_main.h>

#include "../local_include/_xmgr_cas_na.h"
#include "../local_include/_xmgr_cas_na_util.h"
#include "../local_include/_xmgr_cas_na_adapt.h"

#if 0
typedef struct
{
	/* message Dialog를 사용하기 전 꼭 채워주여야 하는 값들. */
	HUINT32			ulPinDlgWhoseId;							// 동일한 app 에서 pincode 창을 2개 용도로 보여줘야 할 경우에 사용, ex> medialist 에서 파일 재생시 권한 체크 용도, 파일에 대한 Lock 변경 용도
	HUINT32			ulDlgAttributes;
	HUINT8			*pszDlgTitle;
	HUINT8			*pszDlgMsg;
	BUS_Callback_t	parentAppCallback;

	/* 자동으로 지정됨. 만질 필요없음. */
	BUS_Callback_t	thisAppCallback;
	HBOOL			bConfirmFlag;								/* verify drawing flag and re-confirm*/
	HINT32			nPinInputPos;
	HUINT8			szPinCodeStr[5];
	HUINT8			szNewPinCodeStr[5];			/* new pin code */
	HUINT8			szVerifyPinCodeStr[5];		/* verify pin code */
	HUINT8			szSavedPinCode[5];		/* verify pin code */
	HUINT8			*pszNewPincodeUIStr;
	HUINT8			*pszVerifyPincodeUIStr;
	HUINT32			ulActionId;
	HUINT32			ulAppPriority;
} Na_PinDialogInstance_t;
#endif
/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
//STATIC Na_PinDialogInstance_t	s_na_stChangePinCodeInst;		/**< Change Pin Code Instance. */
static Handle_t	s_hSession = HANDLE_NULL;


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Local Functions      ********************/
/*******************************************************************/
#if 0
STATIC Na_PinDialogInstance_t*		xmgr_cas_NaGetPinChangeDialogInstance(void)
{
	return &s_na_stChangePinCodeInst;
}
#endif

static BUS_Result_t _xmgr_cas_NaCmd_ResetPinCode(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);


#if 0
#define	______________PRPTOTYPE_______________
STATIC BUS_Result_t		xmgr_cas_Nacmd_ChangePinCodeProc(HINT32 message, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);

#define _____XMGR_CAS_NA_UI_LOCAL_FUNCTION______________________________________________________

BUS_Result_t	xmgr_cas_Nacmd_ChangePinCodeProc(HINT32 message, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t			eGwmResult = ERR_BUS_NO_ERROR;
	Na_PinDialogInstance_t	*instance = NULL;

	instance = xmgr_cas_NaGetPinChangeDialogInstance();

	switch (message)
	{
	case eMEVT_BUS_CREATE:
		//GetStrRsc(LOC_PARENCON_CHANGE_PASSWORD_ITEM_ID),0,(HINT32)BUS_MGR_GetAppCallback(BUS_MGR_GetThis()
		//1 To do 여기에서 Appication으로 전달할 Data를 만들자.
		break;
#if 0
	case MSG_OAPI_CASUI_INPUT_NOTIFY:
		/* set dialog timer */

		BUS_ResetTimer(DLG_EXP_TIMER_ID);

		switch (p1)
		{
			case KEY_GUIDE:
			case KEY_MENU:
				xmgr_cas_Napindlg_NotifyPinDenied(instance);
				return MESSAGE_PASS;

			/* keys to pin quit */
			case KEY_OK:
			case KEY_EXIT:
			case KEY_BACK:
				xmgr_cas_Napindlg_NotifyPinDenied(instance);
				return MESSAGE_BREAK;

			/* keys to ignore */
			case KEY_ARROWLEFT:
				xmgr_cas_Napin_change_BackspacePosition(instance);
				return MESSAGE_BREAK;


			case KEY_ARROWRIGHT:
			case KEY_ARROWUP:
			case KEY_ARROWDOWN:
				return MESSAGE_BREAK;

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
				/* new pin first */
				if(instance->bConfirmFlag == FALSE)
				{
					if(xmgr_cas_Napin_change_CheckNewPinNumber(instance, (p1 - KEY_0)) >= 5 - 1)
					{
						/* 자동으로 verify pin code로 전환 */
						instance->bConfirmFlag = TRUE; /* veryfy flag setting */
						instance->nPinInputPos = 0;
						GWM_Obj_SetFocus(ALERT_CHANGEPIN_VERIFYBTN_ID + instance->nPinInputPos);
					}
				}
				else
				{
					if(xmgr_cas_Napin_change_CheckConfirmPinNumber(instance, (p1 - KEY_0)) >= 5 - 1)
					{
						GWM_DirectRedrawAll();
						if (xmgr_cas_Napin_change_IsChangePinOk(instance) == TRUE)
						{
							/* pin code is RIGHT */
							xmgr_cas_Napindlg_NotifyPinAuthorized(instance);
						}
						else
						{
							/* pin code is WRONG */
							xmgr_cas_Napin_change_ProcessChangePinReset(instance);
							GWM_APP_InvalidateRect(ALERT_CHANGEPIN_FRAME_X, ALERT_CHANGEPIN_FRAME_Y, ALERT_CHANGEPIN_FRAME_W, ALERT_CHANGEPIN_FRAME_H);
						}
					}
				}
				return MESSAGE_BREAK;

			default:
				break;
		}
		break;
#endif

#if 0
	case MSG_APP_UPDATE_SMARTCARD_INFO :
		if (p1 == eDxCAS_GROUPID_NA)
		{
			if (xmgr_cas_NaPincodeCheckSCRemoved())
			{
				//xmgr_cas_Napindlg_NotifyPinDenied(instance);
			}
		}
		break;
#endif
	default :
		break;
	}


	if( (eGwmResult == MESSAGE_BREAK) || (eGwmResult == MESSAGE_PASS) )
		return eGwmResult;

	return BUS_ProcessMessageDefault (message,  handle, p1, p2, p3);

}
#endif

static HERROR _xmgr_cas_NaCmd_SendResetPinCodeMessage(void)
{
	OxMgrCasUiEvtInput_t *pstEvent;

	if(s_hSession == HANDLE_NULL)
	{
		return ERR_FAIL;
	}

	pstEvent = (OxMgrCasUiEvtInput_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiEvtInput_t));
	if(pstEvent == NULL)
	{
		HxLOG_Error("HLIB_STD_MemCalloc() error\n");
		return ERR_FAIL;
	}

	pstEvent->stHeader.ulAttr = eMgrCasUiAttr_Subtitle;
	MWC_UTIL_DvbStrncpy(pstEvent->stHeader.szSubTitle, "[{\"id\":\"LOC_CAS_NA_RESET_PIN_CODE_ID\"}]", MGR_CAS_STR_LENGTH_LONG-1);

	pstEvent->stInputData.eInputType = eMgrCasInputType_Button;
	pstEvent->stInputData.utInputData.stBtn.eMgrCasBtnType = eMgrCasBtn_1st;
	MWC_UTIL_DvbStrncpy(pstEvent->stInputData.utInputData.stBtn.aszButtonName[0], "[{\"id\":\"LOC_CLOSE_ID\"}]", MGR_CAS_STR_LENGTH_SHORT-1);

	pstEvent->stSessionInfo.eSessionType = eDxCAS_SESSIONTYPE_EXTENSION;
	pstEvent->stSessionInfo.hSession = s_hSession;
	pstEvent->stSessionInfo.ulUiHandle = (HUINT32)_xmgr_cas_NaCmd_ResetPinCode;

	BUS_PostMessageToOM(NULL, eMEVT_CASUI_TYPE_INPUT_DATA, HANDLE_NULL, eDxCAS_GROUPID_NA, 0, (HINT32)pstEvent);

	return ERR_OK;
}


static BUS_Result_t _xmgr_cas_NaCmd_ResetPinCode_Create(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	s_hSession = HANDLE_NULL;

	return ERR_BUS_NO_ERROR;
}

static BUS_Result_t _xmgr_cas_NaCmd_ResetPinCode_OpenSession(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	OxMgrCasSessionEvent_t *pstSession = (OxMgrCasSessionEvent_t *)p3;

	s_hSession = pstSession->hSession;

	xmgr_cas_NaUtil_PostMessageSessionEventToOM(eDxCAS_SESSIONTYPE_EXTENSION, _xmgr_cas_NaCmd_ResetPinCode, s_hSession, 0, eMEVT_CASUI_SESSION_OPENED);

	_xmgr_cas_NaCmd_SendResetPinCodeMessage();

	return ERR_BUS_NO_ERROR;
}

static BUS_Result_t _xmgr_cas_NaCmd_ResetPinCode_CloseSession(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return BUS_MGR_Destroy(_xmgr_cas_NaCmd_ResetPinCode);
}

static BUS_Result_t _xmgr_cas_NaCmd_ResetPinCode_UserInputData(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgr_cas_NaUtil_PostMessageSessionEventToOM(eDxCAS_SESSIONTYPE_EXTENSION, _xmgr_cas_NaCmd_ResetPinCode, s_hSession, 0, eMEVT_CASUI_REQ_CLOSE_SESSION);

	return ERR_BUS_NO_ERROR;
}

static BUS_Result_t _xmgr_cas_NaCmd_ResetPinCode_SvcChanged(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgr_cas_NaUtil_PostMessageSessionEventToOM(eDxCAS_SESSIONTYPE_EXTENSION, _xmgr_cas_NaCmd_ResetPinCode, s_hSession, 0, eMEVT_CASUI_REQ_CLOSE_SESSION);

	return ERR_BUS_NO_ERROR;
}

static BUS_Result_t _xmgr_cas_NaCmd_ResetPinCode(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t eRes = ERR_BUS_NO_ERROR;

#if defined(CONFIG_DEBUG)
	HUINT8 *pszMsg = xmgr_cas_NaGetMessageName(message);
	if(pszMsg != NULL)
	{
		HxLOG_Print("message(%s), ActId(%x), p1(%x), p2(%x), p3(%x)\n", pszMsg, hAction, p1, p2, p3);
	}
#endif

	switch(message)
	{
		case eMEVT_BUS_CREATE :
			eRes = _xmgr_cas_NaCmd_ResetPinCode_Create(message, hAction, p1, p2, p3);
			break;

		case eOEVT_CAS_CASUI_APP_OPEN_SESSION:
			eRes = _xmgr_cas_NaCmd_ResetPinCode_OpenSession(message, hAction, p1, p2, p3);
			break;

		case eOEVT_CAS_CASUI_APP_CLOSE_SESSION:
			eRes = _xmgr_cas_NaCmd_ResetPinCode_CloseSession(message, hAction, p1, p2, p3);
			break;

		case eOEVT_CAS_CASUI_APP_USERINPUTDATA:
			eRes = _xmgr_cas_NaCmd_ResetPinCode_UserInputData(message, hAction, p1, p2, p3);
			break;

		case eMEVT_LIVE_NOTIFY_SVC_CHANGED:
			eRes = _xmgr_cas_NaCmd_ResetPinCode_SvcChanged(message, hAction, p1, p2, p3);
			break;

		default:
			break;
	}

	if(eRes != ERR_BUS_NO_ERROR)
	{
		return eRes;
	}

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}


#define _____XMGR_CAS_NA_IRDCMD_PIN_CODE_FUNCTION______________________________________________________

void xmgr_cas_NaPIN_CODE_Display(void)
{
#if 0
	HxLOG_Print("Reset Pin Code!!!...\n");
	s_na_stChangePinCodeInst.parentAppCallback = BUS_MGR_GetMgrCallback(BUS_MGR_GetThis ());

	//BUS_MGR_Create(  "AP_ChangePinCodeProc",APP_DIALOGBOX_PRIORITY,xmgr_cas_Nacmd_ChangePinCodeProc,HANDLE_NULL,(HINT32)GetStrRsc(LOC_PARENCON_CHANGE_PASSWORD_ITEM_ID),0,(HINT32)BUS_MGR_GetAppCallback(BUS_MGR_GetThis()));
	BUS_MGR_Create(  "AP_ChangePinCodeProc",APP_DIALOGBOX_PRIORITY,xmgr_cas_Nacmd_ChangePinCodeProc,HANDLE_NULL, 0, 0, 0);
#endif
}

void xmgr_cas_NaCmd_DisplayResetPinCodeMessage(void)
{
	if(BUS_MGR_Get(_xmgr_cas_NaCmd_ResetPinCode) == NULL)
		BUS_MGR_Create(NULL, APP_CAS_PRIORITY, _xmgr_cas_NaCmd_ResetPinCode, HANDLE_NULL, 0, 0, 0);

	if(s_hSession == HANDLE_NULL)
		xmgr_cas_NaUtil_PostMessageSessionEventToOM(eDxCAS_SESSIONTYPE_EXTENSION, _xmgr_cas_NaCmd_ResetPinCode, HANDLE_NULL, 0, eMEVT_CASUI_REQ_OPEN_SESSION);
	else
		_xmgr_cas_NaCmd_SendResetPinCodeMessage();
}

/* end of file */
