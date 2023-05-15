/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <mgr_live.h>
#include <mgr_cas.h>
#include <mgr_pg.h>
#include <xmgr_cas_res_str.h>

#include <namgr_main.h>
#include <isosvc.h>

#include "../local_include/_xmgr_cas_na_util.h"
#include "../local_include/_xmgr_cas_na.h"


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/



/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
#define		NA_UI_PINCODE_TIMER_ID			500
#define		NA_UI_PINCODE_TIMEOUT			2000 //3초 간격으로 timer~

#define		DLG_PIN_TIMEOUT_MASK			0x000000FF


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

typedef enum
{
	eCAS_NA_PINCODE_QUERY = 0,
	eCAS_NA_PINCODE_CHANGE,
} eCAS_NA_PINCODE;

typedef struct
{
	eCAS_NA_PINCODE			eStateOfCASPincode;
	HUINT8					*cas_na_pin_change_title;
	HUINT8					cas_na_pincode_message[512/*TEXT_SIZE512*/];
} NA_CAS_UI_PIN_Context;

typedef struct
{
	Handle_t		hAction;
	Handle_t		hSession;
	HUINT32			ulSlotNumber;
	HINT32			ulMessageBackup;
	HINT32			ulCasMsgIdBackup;
} sNaUi_PinDialog_Context_t;


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC Na_PinDialogInstance_t 	s_cas_na_sc_PinDialogInst;
STATIC NA_CAS_UI_PIN_Context	s_cas_na_pin_context;
STATIC HUINT8					s_bCheckSCPincode;

static sNaUi_PinDialog_Context_t s_stCasNaPinDialogContext;

/*******************************************************************/
/********************      Local Functions      ********************/
/*******************************************************************/
#define _____XMGR_CAS_NA_PINCODE_UI_LOCAL_FUNCTION______________________________________________________

HUINT8 *xmgr_cas_Napin_GetOpPinMessage(HUINT8 a_nRetryCnt);

#if defined(CONFIG_OP_SKYD)
extern HERROR AP_SKYD_GetPinMessageLocId(HUINT8 a_nRetryCnt, HINT32 *a_plLocId);
#endif

#if defined (CONFIG_OP_KDG)
extern HERROR AP_KDG_GetPinMessageLocId(HUINT8 a_nRetryCnt, HINT32 *a_plLocId);
#endif

/*******************************************************************/
/*********** Smartcard Check 및 Smartcard Change        ************/
/*********** Pin-Code에서 공통으로 사용하는 함수들      ************/
/*******************************************************************/
#define _____COMMON_UI_API______________________________________________________
STATIC Na_PinDialogInstance_t*		xmgr_cas_NaGetPinDialogInstance(void)
{
	return &s_cas_na_sc_PinDialogInst;
}

STATIC NA_CAS_UI_PIN_Context*		xmgr_cas_NaGetPinDialogContext(void)
{
	return &s_cas_na_pin_context;
}

#if 0
STATIC void				xmgr_cas_Napindlg_NotifyPinAuthorized(Na_PinDialogInstance_t *instance)
{
	BUS_MGR_Destroy(0);
	BUS_SendMessage(instance->parentAppCallback, eMEVT_PG_PIN_CHECK_OK, (int)NULL, 0, (int)instance->ulPinDlgWhoseId, (int)instance->thisAppCallback);
}
#endif

STATIC void				xmgr_cas_Napindlg_NotifyPinDenied(Na_PinDialogInstance_t *instance)
{
	/* 없애고나서 메시지 보내야 함. 순서 지킬 것... */
	BUS_MGR_Destroy(0);
	BUS_SendMessage(instance->parentAppCallback, eMEVT_PG_PIN_CHECK_DENIED, (int)NULL, 0, (int)instance->ulPinDlgWhoseId, (int)instance->thisAppCallback);
}



/*******************************************************************/
/*********** SC Change  Pin-Code에서 사용하는 함수들    ************/
/*******************************************************************/
#define _____USE_SC_PIN_CHANGE_UI_API______________________________________________________
#if 0
STATIC HINT32		xmgr_cas_Napin_change_BackspacePosition(Na_PinDialogInstance_t *instance)
{
	HINT32	nObjId = 0;

	if((instance->nPinInputPos > 0) && (instance->nPinInputPos < 5-1))
	{
		/* backspace */
		instance->nPinInputPos--;

		if(instance->bConfirmFlag == FALSE)
		{
			/* set curr by inputed value */
			instance->szNewPinCodeStr[instance->nPinInputPos] = ('-');
			nObjId = ALERT_CHANGEPIN_BTN_ID;
		}
		else
		{
			/* set curr by inputed value */
			instance->szVerifyPinCodeStr[instance->nPinInputPos] = ('-');
			nObjId = ALERT_CHANGEPIN_VERIFYBTN_ID;
		}

		/* invalidate Curr */
		GWM_APP_InvalidateObject(nObjId + instance->nPinInputPos + 1);

		/* set focus next */
		GWM_Obj_SetFocus(nObjId + instance->nPinInputPos);
	}
	else if(instance->nPinInputPos == 0)
	{
		if(instance->bConfirmFlag == TRUE)
		{
			instance->bConfirmFlag = FALSE; /* veryfy flag setting */
			instance->nPinInputPos = 5 - 2;

			instance->szNewPinCodeStr[instance->nPinInputPos] = ('-');
			GWM_Obj_SetFocus(ALERT_CHANGEPIN_BTN_ID + instance->nPinInputPos);
		}
	}
	else
	{
		/* nothing to do */
	}

	return instance->nPinInputPos;
}

STATIC HINT32		xmgr_cas_Napin_change_CheckNewPinNumber(Na_PinDialogInstance_t *instance, HINT32 number)
{
	/* new pin code */
	if(instance->nPinInputPos < 5-1)
	{
		instance->szNewPinCodeStr[instance->nPinInputPos] = ('0' + number);
		instance->nPinInputPos++;

		GWM_Obj_SetFocus(ALERT_CHANGEPIN_BTN_ID + instance->nPinInputPos);
	}

	return instance->nPinInputPos;
}


STATIC HINT32		xmgr_cas_Napin_change_CheckConfirmPinNumber(Na_PinDialogInstance_t *instance, HINT32 number)
{
	if(instance->nPinInputPos < 5-1)
	{
		instance->szVerifyPinCodeStr[instance->nPinInputPos] = ('0' + number);
		instance->nPinInputPos++;
		GWM_Obj_SetFocus(ALERT_CHANGEPIN_VERIFYBTN_ID + instance->nPinInputPos);
	}

	return instance->nPinInputPos;
}

STATIC void			xmgr_cas_Napin_change_ProcessChangePinReset(Na_PinDialogInstance_t *instance)
{
	HUINT8			i,j;

	/* verify OK 된 후 flag는 초기화 */
	instance->bConfirmFlag = FALSE;

	HxSTD_memset(instance->szNewPinCodeStr, '-', sizeof(HUINT8) * (5-1));
	instance->szNewPinCodeStr[5-1] = '\0';
	HxSTD_memset(instance->szVerifyPinCodeStr, '-', sizeof(HUINT8) * (5-1));
	instance->szVerifyPinCodeStr[5-1] = '\0';

	instance->nPinInputPos = 0;

	for (i = 0; i < 5-1; i++)	// new button 초기 화
	{
		GWM_Button_ChangeTitle(ALERT_CHANGEPIN_BTN_ID + i, instance->szNewPinCodeStr + i);
	}
	for (j = 0; j < 5-1; j++)	// verify button 초기 화
	{
		GWM_Button_ChangeTitle(ALERT_CHANGEPIN_VERIFYBTN_ID + j, instance->szVerifyPinCodeStr + j);
	}

	GWM_Obj_SetFocus(ALERT_CHANGEPIN_BTN_ID);
}
#endif

STATIC HBOOL			xmgr_cas_Napin_change_IsChangePinOk(Na_PinDialogInstance_t *instance)
{
	HERROR		hError;

	if (MWC_UTIL_DvbStrncmp(instance->szVerifyPinCodeStr, instance->szNewPinCodeStr, 5-1) == 0)
	{
		hError = xmgr_cas_NaChangeSmartCardPin(instance->szPinCodeStr, instance->szNewPinCodeStr);
		if (hError != ERR_OK)
		{
			return FALSE;
		}

		HxLOG_Print(">>>>>>>> New PIN CODE is Okay(%s)(%s)\n", instance->szNewPinCodeStr, instance->szVerifyPinCodeStr);
		return TRUE;
	}
	else
	{
		HxLOG_Print(">>>>>>>> New PIN CODE is NOT Okay\n");
		return FALSE;
	}
}


/*******************************************************************/
/*********** SC Check  Pin-Code에서 사용하는 함수들     ************/
/*******************************************************************/
#define _____USE_SC_PIN_CHECK_UI_API______________________________________________________
STATIC HUINT8			*xmgr_cas_Napin_check_GetBlockRemainTime(HUINT32 ulRemainTime)
{
	HUINT32 curTime = 0;
	HINT32 ulDiffTime = 0;
	HINT32 nLocId;

	if (VK_CLOCK_GetTime((unsigned long*)&curTime) == ERR_OK)
	{
		ulDiffTime = (HINT32)(((ulRemainTime-1) / 60) + 1);
	}

//	nLocId = STR_MESG_238_ID; //LOC_CAS_NA_ENTERPINCODE_INCORRECT3
	// 모든 String 관리는 Web에서 처리하는 것으로 약속됨에 따라 AP_CasResStr_GetStrRsc() 함수를 더이상 사용하지 말아야 함.
	// 따라서 LOC ID를 Web으로 전송 후 Web에서 그에 맞는 string을 찾아 출력하도록 함.
	snprintf(s_cas_na_pin_context.cas_na_pincode_message, 512, "[{\"id\":\"LOC_CAS_NA_ENTERPINCODE_INCORRECT3\",\"in\":[%d]}]", ulDiffTime);


#if defined(CONFIG_OP_SKYD) || defined (CONFIG_OP_KDG)
	{
		HUINT8	szIncorrect3Str[256];
		//nLocId = STR_MESG_827_ID;//LOC_CAS_NA_SKYD_ENTERPINCODE_INCORRECT3
		// 모든 String 관리는 Web에서 처리하는 것으로 약속됨에 따라 AP_CasResStr_GetStrRsc() 함수를 더이상 사용하지 말아야 함.
		// 따라서 LOC ID를 Web으로 전송 후 Web에서 그에 맞는 string을 찾아 출력하도록 함.
		snprintf(s_cas_na_pin_context.cas_na_pincode_message, 512,
			"[{\"id\":\"LOC_CAS_NA_SKYD_ENTERPINCODE_TITLE_ID\"},{\"id\":\"LOC_CAS_NA_SKYD_ENTERPINCODE_INCORRECT3\",\"in\":[%d]}]", ulDiffTime);
		//sprintf(szIncorrect3Str, AP_CasResStr_GetStrRsc(nLocId), ulDiffTime);
		//sprintf(s_cas_na_pin_context.cas_na_pincode_message, "%s\n%s"
		//	, AP_CasResStr_GetStrRsc(STR_PARENTAL_CONTROL_02_ID/*LOC_CAS_NA_SKYD_ENTERPINCODE_TITLE_ID*/)
		//	, szIncorrect3Str);
	}
#endif

	return s_cas_na_pin_context.cas_na_pincode_message;
}

#if 0
STATIC HINT32			xmgr_cas_Napin_check_CheckCurrNumber(Na_PinDialogInstance_t *instance, HINT32 number)
{
	if (instance->nPinInputPos < 5-1)
	{
		/* set curr by inputed value */
		instance->szPinCodeStr[instance->nPinInputPos] = ('0' + number);

		/* invalidate Curr */
		GWM_APP_InvalidateObject(ALERT_PIN_CODE_ID + instance->nPinInputPos);

		/* go to next */
		instance->nPinInputPos++;

		/* set focus next */
		GWM_Obj_SetFocus(ALERT_PIN_CODE_ID + instance->nPinInputPos);
	}

	return instance->nPinInputPos;
}
#endif

#if 0
STATIC BUS_Result_t		xmgr_cas_Napin_check_CreatePincode(Na_PinDialogInstance_t* instance, HUINT8 bTimer)
{
	HUINT32 			ulRemainTime = 0;
	eCAS_PIN_STATE		nAccessState;
	HUINT8				nRetryCount;
	HUINT8				*pszPopup = NULL;
	HUINT32 			ulDlgAttr = instance->ulDlgAttributes;
//	BUS_Callback_t		parentAppCallback = instance->parentAppCallback;

	nAccessState = xmgr_cas_NaGetPincodeAccessibleState(&ulRemainTime, &nRetryCount);
	if (nAccessState == eCAS_PIN_STATE_OK)
	{
//		ulDlgAttr &= (~DLG_TYPE_BLKPINCODE);
		BUS_KillTimer(NA_UI_PINCODE_TIMER_ID);

		pszPopup = xmgr_cas_Napin_GetOpPinMessage(nRetryCount);
		if ( pszPopup == NULL )
		{
			switch (nRetryCount)
			{
			case 1 :
				pszPopup = (HUINT8*)AP_CasResStr_GetStrRsc(STR_MESG_228_ID/*LOC_CAS_NA_ENTERPINCODE_INCORRECT1*/);
				break;

			case 2 :
				pszPopup = (HUINT8*)AP_CasResStr_GetStrRsc(STR_MESG_196_ID/*LOC_CAS_NA_ENTERPINCODE_INCORRECT2*/);
				break;

			default :
				pszPopup = (HUINT8*)AP_CasResStr_GetStrRsc(STR_MESG_171_ID/*LOC_CAS_NA_ENTERPINCODE*/);
				break;
			}
		}
	}
	else if (nAccessState == eCAS_PIN_STATE_NORIGHT)
	{
//		ulDlgAttr |= DLG_TYPE_BLKPINCODE;
		pszPopup = xmgr_cas_Napin_check_GetBlockRemainTime(ulRemainTime);

		if (bTimer)
		{
			BUS_SetTimer(NA_UI_PINCODE_TIMER_ID, NA_UI_PINCODE_TIMEOUT);
		}
	}
	else
	{
		HxLOG_Print("nAccessState[%x] don't create pin-code dialog \n", nAccessState);
		xmgr_cas_Napindlg_NotifyPinDenied(instance);
		return ERR_BUS_NO_ERROR;
	}

	//AP_PinDialog_SetDialog(instance, parentAppCallback, ulDlgAttr, NULL, pszPopup);
	//AP_PinDialog_UpdateDlg(instance);

	//1 To do 여기에서 Appication으로 전달할 Data를 만들자.

	return ERR_BUS_NO_ERROR;
}
#endif

#if 0
STATIC BUS_Result_t		xmgr_cas_Napin_check_CheckPincode(Na_PinDialogInstance_t *instance)
{
	HERROR				hError;

	hError = xmgr_cas_NaCheckSmartCardPin(instance->szPinCodeStr);
	if (hError != ERR_OK)
	{
		xmgr_cas_Napin_check_CreatePincode(instance, TRUE);
	}
	else
	{
		BUS_MGR_Destroy(NULL);
		xmgr_cas_Napindlg_NotifyPinAuthorized(instance);
	}

	return MESSAGE_BREAK;
}
#endif

#if 0
STATIC HINT32			local_pindlg_BackspacePosition(Na_PinDialogInstance_t *instance)
{
	if((instance->nPinInputPos > 0) && (instance->nPinInputPos < 5-1))
	{
		/* backspace */
		instance->nPinInputPos--;

		/* set curr by inputed value */
		instance->szPinCodeStr[instance->nPinInputPos] = ('-');

		/* invalidate Curr */
		GWM_APP_InvalidateObject(ALERT_PIN_CODE_ID + instance->nPinInputPos + 1);

		/* set focus next */
		GWM_Obj_SetFocus(ALERT_PIN_CODE_ID + instance->nPinInputPos);
	}

	return instance->nPinInputPos;
}
#endif

#if 0
STATIC BUS_Result_t		xmgr_cas_Napin_check_Dialog_Proc(HINT32 message, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t			eGwmResult = ERR_BUS_NO_ERROR;
	Na_PinDialogInstance_t	*instance = NULL;

	instance = xmgr_cas_NaGetPinDialogInstance();

	switch (message)
	{
	case eMEVT_BUS_CREATE :
		eGwmResult = xmgr_cas_Napin_check_CreatePincode(instance, TRUE);
		if(instance->ulDlgAttributes & DLG_PIN_TIMEOUT_MASK)
		{
			//BUS_SetTimer(DLG_EXP_TIMER_ID, DLG_PIN_TIMEOUT);
		}
		break;

	case eMEVT_BUS_TIMER:
		if (p1 == NA_UI_PINCODE_TIMER_ID)
		{
			xmgr_cas_Napin_check_CreatePincode(instance, FALSE);
			return MESSAGE_BREAK;
		}
		/*
		else if(p1 == DLG_EXP_TIMER_ID)
		{
			xmgr_cas_Napindlg_NotifyPinDenied(instance);
		}
		*/
		break;
#if 0
	case MSG_OAPI_CASUI_INPUT_NOTIFY:

		//BUS_SendMessage(NULL, MSG_APP_CHLIST_RESET_OSDTIMER, (int)NULL, 0, 0, 0);
		//BUS_ResetTimer(DLG_EXP_TIMER_ID);

		switch (p1)
		{
			case KEY_GUIDE:
			case KEY_MENU:
				if( (instance->ulDlgAttributes & DLG_TYPE_NO_EXIT) == 0 )
				{
					xmgr_cas_Napindlg_NotifyPinDenied(instance);
				}
				return MESSAGE_PASS;

			/* keys to pin quit */
			case KEY_BACK:
				if( (instance->ulDlgAttributes & DLG_TYPE_NO_EXIT) == 0 )
				{
					xmgr_cas_Napindlg_NotifyPinDenied(instance);
				}
				return MESSAGE_BREAK;

			/* keys to pin quit */
			case KEY_EXIT:
				if( (instance->ulDlgAttributes & DLG_TYPE_NO_EXIT) == 0 )
				{
					xmgr_cas_Napindlg_NotifyPinDenied(instance);
				}
				return MESSAGE_PASS;

			/* keys to ignore */
			case KEY_ARROWRIGHT:
			case KEY_ARROWUP:
			case KEY_ARROWDOWN:
			case KEY_RED:
			case KEY_BLUE:
			case KEY_GREEN:
			case KEY_YELLOW:
				return MESSAGE_BREAK;

			/* go back */
			case KEY_ARROWLEFT:
				local_pindlg_BackspacePosition(instance);
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
				if (instance->ulDlgAttributes & DLG_TYPE_BLKPINCODE)
				{
					return MESSAGE_BREAK;
				}

				if (xmgr_cas_Napin_check_CheckCurrNumber(instance, (p1 - KEY_0)) >= 5-1)
				{
					GWM_DirectRedrawAll();

					xmgr_cas_Napin_check_CheckPincode(instance);
				}
				return MESSAGE_BREAK;

			default:
				break;
		}
#endif
		break;

#if 0
	case MSG_APP_UPDATE_SMARTCARD_INFO :
		if (p1 == eDxCAS_GROUPID_NA)
		{
			if (xmgr_cas_NaPincodeCheckSCRemoved())
			{
				xmgr_cas_Napindlg_NotifyPinDenied(instance);
			}
		}
		break;
#endif

	case eMEVT_BUS_DESTROY:
		break;

	}

	if( (eGwmResult == MESSAGE_BREAK) || (eGwmResult == MESSAGE_PASS) )
		return eGwmResult;

	return BUS_ProcessMessageDefault(message, handle, p1, p2, p3);
}
#endif

STATIC BUS_Result_t	xmgr_cas_Napin_change_Dialog_Proc(HINT32 message, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t			eGwmResult = ERR_BUS_NO_ERROR;
	Na_PinDialogInstance_t	*instance = NULL;

	instance = xmgr_cas_NaGetPinDialogInstance();

	switch (message)
	{
	case eMEVT_BUS_CREATE:
		p3 = (HINT32)instance->parentAppCallback;
		break;
#if 0
	case MSG_OAPI_CASUI_INPUT_NOTIFY:

		/* set dialog timer */
		//BUS_ResetTimer(DLG_EXP_TIMER_ID);
		BUS_SendMessage(instance->parentAppCallback, MSG_DLG_INPUTED_KEYDOWN, (int)NULL, p1, 0, (HINT32)BUS_MGR_GetMgrCallback(BUS_MGR_GetThis()));

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
#endif
		break;

#if 0
	case MSG_APP_UPDATE_SMARTCARD_INFO :
		if (p1 == eDxCAS_GROUPID_NA)
		{
			if (xmgr_cas_NaPincodeCheckSCRemoved())
			{
				xmgr_cas_Napindlg_NotifyPinDenied(instance);
			}
		}
		break;
#endif

	default :
		break;
	}


	if( (eGwmResult == MESSAGE_BREAK) || (eGwmResult == MESSAGE_PASS) )
		return eGwmResult;

	return BUS_ProcessMessageDefault(message, handle, p1, p2, p3);
}

STATIC HERROR		xmgr_cas_Napin_code_ValidCheck(void)
{
	HUINT32 			ulRemainTime = 0;
	HUINT8				nRetryCount;
	eCAS_PIN_STATE		nAccessState;

	nAccessState = xmgr_cas_NaGetPincodeAccessibleState(&ulRemainTime, &nRetryCount);
	if ((nAccessState == eCAS_PIN_STATE_NOACCESS) || (nAccessState == eCAS_PIN_STATE_NOTIME))
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC BUS_Result_t	xmgr_cas_Napin_code_Initialize(HUINT32 ulDlgAttributes, BUS_Callback_t	parentAppCallback)
{
	Na_PinDialogInstance_t	*instance = NULL;

	if (xmgr_cas_NaPincodeCheckSCRemoved())
	{
		HxLOG_Error("Smartcard not inserted!\n");
		return ERR_BUS_INITIAL_FAILED;
	}

	instance = xmgr_cas_NaGetPinDialogInstance();
	HxSTD_memset (instance, 0, sizeof(Na_PinDialogInstance_t));

	if (xmgr_cas_NaPincodeInit() != ERR_OK)
	{
		HxLOG_Print("xmgr_cas_NaPincodeInit fail \n");
	}
#if 0
	if ((ulDlgAttributes & DLG_TYPE_TITLE) != 0)
	{
		ulDlgAttributes &= (~DLG_TYPE_TITLE);
		ulDlgAttributes |= DLG_TYPE_MESSAGE;
	}

	if ((ulDlgAttributes & (DLG_TYPE_TITLE | DLG_TYPE_MESSAGE)) == 0)
	{
		ulDlgAttributes |= DLG_TYPE_MESSAGE;
	}
#endif
	instance->ulDlgAttributes = ulDlgAttributes;
	instance->parentAppCallback = parentAppCallback;

	return ERR_BUS_NO_ERROR;
}

HUINT8 *xmgr_cas_Napin_GetOpPinMessage(HUINT8 a_nRetryCnt)
{
#if defined(CONFIG_OP_SKYD) || defined(CONFIG_OP_KDG)
	HERROR hErr = ERR_FAIL;
	HINT32 nLocId;
#endif
	HUINT8 *pszErrMsg = (HUINT8 *)NULL;

#if defined(CONFIG_OP_SKYD)
	hErr = AP_SKYD_GetPinMessageLocId(a_nRetryCnt, &nLocId);
	if (hErr == ERR_OK)
	{
		// 모든 String 관리는 Web에서 처리하는 것으로 약속됨에 따라 AP_CasResStr_GetStrRsc() 함수를 더이상 사용하지 말아야 함.
		// 따라서 LOC ID를 Web으로 전송 후 Web에서 그에 맞는 string을 찾아 출력하도록 함.

		// TODO: 아래 nLocId 값에 따라 실제 LOC_XXXX_ID 을 넣어주는 작업 필요
		snprintf(s_cas_na_pin_context.cas_na_pincode_message, 512,
			"[{\"id\":\"LOC_CAS_NA_SKYD_ENTERPINCODE_TITLE_ID\"},{\"id\":\"LOC_XXXX_ID\"}]");
		//sprintf(s_cas_na_pin_context.cas_na_pincode_message, "%s\n%s"
		//	, AP_CasResStr_GetStrRsc(STR_PARENTAL_CONTROL_02_ID/*LOC_CAS_NA_SKYD_ENTERPINCODE_TITLE_ID*/)
		//	, AP_CasResStr_GetStrRsc(nLocId));
		pszErrMsg = (HUINT8 *)s_cas_na_pin_context.cas_na_pincode_message;
	}
	else
	{
		pszErrMsg = (HUINT8 *)NULL;
	}
#endif	// #if defined(CONFIG_OP_SKYD)

#if defined(CONFIG_OP_KDG)
	hErr = AP_KDG_GetPinMessageLocId(a_nRetryCnt, &nLocId);
	if (hErr == ERR_OK)
	{
		// 모든 String 관리는 Web에서 처리하는 것으로 약속됨에 따라 AP_CasResStr_GetStrRsc() 함수를 더이상 사용하지 말아야 함.
		// 따라서 LOC ID를 Web으로 전송 후 Web에서 그에 맞는 string을 찾아 출력하도록 함.

		// TODO: 아래 nLocId 값에 따라 실제 LOC_XXXX_ID 을 넣어주는 작업 필요
		snprintf(s_cas_na_pin_context.cas_na_pincode_message, 512,
			"[{\"id\":\"LOC_CAS_NA_SKYD_ENTERPINCODE_TITLE_ID\"},{\"id\":\"LOC_XXXX_ID\"}]");
		//sprintf(s_cas_na_pin_context.cas_na_pincode_message, "%s\n%s"
		//	, AP_CasResStr_GetStrRsc(STR_PARENTAL_CONTROL_02_ID/*LOC_CAS_NA_SKYD_ENTERPINCODE_TITLE_ID*/)
		//	, AP_CasResStr_GetStrRsc(nLocId));
		pszErrMsg = (HUINT8 *)s_cas_na_pin_context.cas_na_pincode_message;
	}
	else
	{
		pszErrMsg = (HUINT8 *)NULL;
	}
 #endif	// #if defined(CONFIG_OP_KDG)


	return pszErrMsg;
}

#if defined(CONFIG_MW_CAS_NAGRA_FEATURE_PARENTAL_CONTROL)
STATIC sNaUi_PinDialog_Context_t* _xmgr_cas_NaUi_PinDialog_GetContext(void)
{
	return &s_stCasNaPinDialogContext;
}

STATIC HERROR _xmgr_cas_NaUi_PinDialog_SendPinMessage(sNaUi_PinDialog_Context_t *pstContext)
{
	OxMgrCasUiEvtInput_t	*pstEvent = NULL;
	BUS_Result_t			hBusResult = ERR_BUS_NO_ERROR;
	OxMgrCasUiInputHeader_t	*pstHeader = NULL;
	OxMgrCasUiInput_t		*pstInputData = NULL;

	HxLOG_Print("[%s:%d] called...\n", __FUNCTION__, __HxLINE__);

	if(pstContext->hSession == HANDLE_NULL)
	{
		HxLOG_Error("pstContext->hSession is HANDLE_NULL\n");
		return ERR_FAIL;
	}

	pstEvent = (OxMgrCasUiEvtInput_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiEvtInput_t));
	if(pstEvent == NULL)
	{
		return ERR_FAIL;
	}

	pstEvent->stSessionInfo.eSessionType	= eDxCAS_SESSIONTYPE_EXTENSION;
	pstEvent->stSessionInfo.hSession		= pstContext->hSession;
	pstEvent->stSessionInfo.ulUiHandle		= (HUINT32)xproc_cas_NaUiPinDialog;

	pstHeader = &pstEvent->stHeader;
	pstInputData = &pstEvent->stInputData;

	pstInputData->eInputType = eMgrCasInputType_Number;
	pstInputData->utInputData.stNumber.ulAttr = eMgrCasUiAttr_HiddenInput ;
	pstInputData->utInputData.stNumber.ulNumberLength = 4;
	HxSTD_MemSet(pstInputData->utInputData.stNumber.pszNumber, 0x00, sizeof(MGR_CASINPUT_NUMBER_MAX_LENGTH));

	hBusResult = BUS_PostMessageToOM(NULL, eMEVT_CASUI_TYPE_INPUT_DATA, HANDLE_NULL, (HINT32)eDxCAS_GROUPID_NA, (HINT32)0, (HINT32)pstEvent);
	if(hBusResult != ERR_BUS_NO_ERROR)
	{
		HxLOG_Error("BUS_PostMessageToOM() ERROR!!\n");
		goto _ERROR;
	}

	return ERR_OK;

_ERROR:
	if(pstEvent != NULL)
		HLIB_STD_MemFree(pstEvent);

	return ERR_FAIL;
}

STATIC HERROR _xmgr_cas_NaUi_PinDialog_CheckPinCode(sNaUi_PinDialog_Context_t *pstContext, OxMgrCasUiUserInputNotify_t *pstInputNotify)
{
	SvcCas_CtrlParam_t				stParam;
	NA_SetParentalControlInParam_t	stInParam;
	OxMgrCasUiNotifyNumber_t		*pstNumber;
	HCHAR 							szPinCode[16] = {0,};
	HUINT8							ucPinLength;

	if(DB_PARAM_GetItem(eDB_PARAM_ITEM_USER_PINCODE, (HUINT32 *)szPinCode, 16) != ERR_OK)
	{
		HLIB_STD_StrNCpySafe(szPinCode, "0000", 16);
	}

	pstNumber = &pstInputNotify->utData.stNumber;

	ucPinLength = (pstNumber->ulNumberLength > 16 ? 16 : pstNumber->ulNumberLength);
	if(HxSTD_StrNCmp(szPinCode, pstNumber->pszNumber, ucPinLength) != 0)
	{
		return _xmgr_cas_NaUi_PinDialog_SendPinMessage(pstContext);
	}

	stInParam.xTsId			= pstContext->hAction;
	stInParam.bUserPinPass	= TRUE;

	stParam.pvIn			= &stInParam;
	stParam.ulControlType	= eNaCtrl_SetParentalControl;

	if(SVC_CAS_CtrlSet(eDxCAS_GROUPID_NA, stParam.ulControlType, &stParam) != ERR_OK)
	{
		HxLOG_Error("SVC_CAS_CtrlSet(eNaCtrl_SetParentalControl) error\n");
		return ERR_FAIL;
	}

	xmgr_cas_NaUtil_PostMessageSessionEventToOM(eDxCAS_SESSIONTYPE_EXTENSION, xproc_cas_NaUiPinDialog, pstContext->hSession, pstContext->ulSlotNumber, eMEVT_CASUI_REQ_CLOSE_SESSION);

	return ERR_OK;
}

STATIC BUS_Result_t _xmgr_cas_NaUi_PinDialog_MsgCreate(sNaUi_PinDialog_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxLOG_Print("[%s:%d] eMEVT_BUS_CREATE! \n", __FUNCTION__, __HxLINE__);

	HxSTD_memset(pstContext, 0, sizeof(sNaUi_PinDialog_Context_t));

	pstContext->hSession = HANDLE_NULL;

	return ERR_BUS_MESSAGE_BREAK;
}

STATIC BUS_Result_t _xmgr_cas_NaUi_PinDialog_MsgEvtCasFail(sNaUi_PinDialog_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	pstContext->hAction = hAction;

	if(pstContext->hSession != HANDLE_NULL)
		_xmgr_cas_NaUi_PinDialog_SendPinMessage(pstContext);
	else
		xmgr_cas_NaUtil_PostMessageSessionEventToOM(eDxCAS_SESSIONTYPE_EXTENSION, xproc_cas_NaUiPinDialog, HANDLE_NULL, pstContext->ulSlotNumber, eMEVT_CASUI_REQ_OPEN_SESSION);

	return ERR_BUS_MESSAGE_BREAK;
}

STATIC BUS_Result_t _xmgr_cas_NaUi_PinDialog_MsgEvtCasOk(sNaUi_PinDialog_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if(pstContext->hSession != HANDLE_NULL)
		xmgr_cas_NaUtil_PostMessageSessionEventToOM(eDxCAS_SESSIONTYPE_EXTENSION, xproc_cas_NaUiPinDialog, pstContext->hSession, pstContext->ulSlotNumber, eMEVT_CASUI_REQ_CLOSE_SESSION);

	return ERR_BUS_MESSAGE_BREAK;
}

STATIC BUS_Result_t _xmgr_cas_NaUi_PinDialog_MsgUserInputData(sNaUi_PinDialog_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32							ulSlotNumber = 0;
	DxCAS_MmiSessionType_e			eSessionType;
	OxMgrCasUiUserInputNotify_t		*pstUserInputNotify = NULL;

	SvcCas_CtrlParam_t				stParam;
	NA_SetParentalControlInParam_t	stInParam;

	ulSlotNumber = (HUINT32)p1;
	eSessionType = (DxCAS_MmiSessionType_e)p2;
	pstUserInputNotify = (OxMgrCasUiUserInputNotify_t *)p3;

	switch(pstUserInputNotify->eInputType)
	{
		case eMgrCasInputType_Number:
			_xmgr_cas_NaUi_PinDialog_CheckPinCode(pstContext, pstUserInputNotify);
			break;

		default:
			break;
	}

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t _xmgr_cas_NaUi_PinDialog_MsgOpenSession(sNaUi_PinDialog_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	OxMgrCasSessionEvent_t *pstSession = (OxMgrCasSessionEvent_t *)p3;

	pstContext->hSession = pstSession->hSession;

	xmgr_cas_NaUtil_PostMessageSessionEventToOM(eDxCAS_SESSIONTYPE_EXTENSION, xproc_cas_NaUiPinDialog, pstSession->hSession, pstSession->ulCasSlotNum, eMEVT_CASUI_SESSION_OPENED);
	_xmgr_cas_NaUi_PinDialog_SendPinMessage(pstContext);

	return ERR_BUS_MESSAGE_PASS;
}

STATIC BUS_Result_t _xmgr_cas_NaUi_PinDialog_MsgCloseSession(sNaUi_PinDialog_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return BUS_MGR_Destroy(NULL);
}

STATIC BUS_Result_t _xmgr_cas_NaUi_PinDialog_MsgEvtSvcChanged(sNaUi_PinDialog_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if(pstContext->hSession != HANDLE_NULL)
		xmgr_cas_NaUtil_PostMessageSessionEventToOM(eDxCAS_SESSIONTYPE_EXTENSION, xproc_cas_NaUiPinDialog, pstContext->hSession, pstContext->ulSlotNumber, eMEVT_CASUI_REQ_CLOSE_SESSION);

	return ERR_BUS_MESSAGE_PASS;
}
#endif
/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
#define _____XMGR_CAS_NA_PINCODE_UI_PUBLIC_FUNCTION______________________________________________________
#if 0
BUS_Result_t	xmgr_cas_NaPinDialog_ProtoProc(Na_PinDialogInstance_t *pInstance, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	switch (message)
	{
	case eMEVT_BUS_CREATE :
		s_bCheckSCPincode = TRUE;

		if (xmgr_cas_Napin_code_Initialize(pInstance->ulDlgAttributes, pInstance->parentAppCallback) != ERR_OK)
		{
			BUS_MGR_Destroy(NULL);
			return MESSAGE_BREAK;
		}

		if (xmgr_cas_Napin_code_ValidCheck() != ERR_OK)
		{
			HxLOG_Error("xmgr_cas_Napin_code_ValidCheck!\n");
			s_bCheckSCPincode = FALSE;
		}

		break;

	default :
		break;
	}

	if (s_bCheckSCPincode)
	{
		return xmgr_cas_Napin_check_Dialog_Proc(message, hAction, p1, p2, p3);
	}
	else
	{
		return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
	}
}

BUS_Result_t	xmgr_cas_NaChangePinDialog_ProtoProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NA_CAS_UI_PIN_Context *pContext = xmgr_cas_NaGetPinDialogContext();

	switch(message)
	{
		case eMEVT_BUS_CREATE :
			HxSTD_memset(pContext, 0, sizeof(NA_CAS_UI_PIN_Context));

			pContext->eStateOfCASPincode = eCAS_NA_PINCODE_QUERY;
			pContext->cas_na_pin_change_title = (HUINT8*)p1;

			if (xmgr_cas_Napin_code_Initialize(p2, xmgr_cas_NaChangePinDialog_ProtoProc) != ERR_OK)
			{
				BUS_MGR_Destroy(NULL);
				return MESSAGE_BREAK;
			}

			BUS_MGR_Create("xmgr_cas_NaPincodeDialog_Proc", APP_DIALOGBOX_PRIORITY,
							(BUS_Callback_t)xmgr_cas_Napin_check_Dialog_Proc, HANDLE_NULL, 0, 0, 0);
			break;

		case eMEVT_PG_PIN_CHECK_OK :
			if (pContext->eStateOfCASPincode == eCAS_NA_PINCODE_QUERY)
			{
				pContext->eStateOfCASPincode = eCAS_NA_PINCODE_CHANGE;

				BUS_MGR_Create("xmgr_cas_NaChangePincodeDialog_Proc", APP_DIALOGBOX_PRIORITY,
								(BUS_Callback_t)xmgr_cas_Napin_change_Dialog_Proc, HANDLE_NULL,
								(HINT32)pContext->cas_na_pin_change_title, 0, 0);
			}
			else
			{
				BUS_MGR_Destroy(NULL);
			}
			break;

		case eMEVT_PG_PIN_CHECK_DENIED :
			BUS_MGR_Destroy(NULL);
			break;
	}

	return ERR_BUS_NO_ERROR;
}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_FEATURE_PARENTAL_CONTROL)
BUS_Result_t	xproc_cas_NaUiPinDialog(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t eBusRes = ERR_BUS_NO_ERROR;
	sNaUi_PinDialog_Context_t *pstContext = _xmgr_cas_NaUi_PinDialog_GetContext();

#if defined(CONFIG_DEBUG)
	HUINT8 *pszMsg = xmgr_cas_NaGetMessageName(message);
	if(pszMsg != NULL)
	{
		HxLOG_Print("[%s:%d] message(%s), hAction(%x), p1(%x), p2(%x), p3(%x)\n", __FUNCTION__, __HxLINE__, pszMsg, hAction, p1, p2, p3);
	}
#endif

	switch(message)
	{
		case eMEVT_BUS_CREATE :
			BUS_MGR_SetMgrName(BUS_MGR_GetThis(), "xproc_cas_NaUiPinDialog");
			eBusRes = _xmgr_cas_NaUi_PinDialog_MsgCreate(pstContext, message, hAction, p1, p2, p3);
			break;

		case eMEVT_CAS_FAIL:
			eBusRes = _xmgr_cas_NaUi_PinDialog_MsgEvtCasFail(pstContext, message, hAction, p1, p2, p3);
			break;
#if 0
		case eMEVT_CAS_CHECK:
			eBusRes = _xmgr_cas_NaUi_MsgEvtCasCheck(pstContext, message, hAction, p1, p2, p3);
			break;
#endif
		case eMEVT_LIVE_NOTIFY_SVC_CHANGED:
			eBusRes = _xmgr_cas_NaUi_PinDialog_MsgEvtSvcChanged(pstContext, message, hAction, p1, p2, p3);
			break;

		case eMEVT_CAS_OK:
			eBusRes = _xmgr_cas_NaUi_PinDialog_MsgEvtCasOk(pstContext, message, hAction, p1, p2, p3);
			break;

		case eOEVT_CAS_CASUI_APP_OPEN_SESSION:
			eBusRes = _xmgr_cas_NaUi_PinDialog_MsgOpenSession(pstContext, message, hAction, p1, p2, p3);
			break;

		case eOEVT_CAS_CASUI_APP_CLOSE_SESSION:
			eBusRes = _xmgr_cas_NaUi_PinDialog_MsgCloseSession(pstContext, message, hAction, p1, p2, p3);
			break;

		case eOEVT_CAS_CASUI_APP_USERINPUTDATA:
			eBusRes = _xmgr_cas_NaUi_PinDialog_MsgUserInputData(pstContext, message, hAction, p1, p2, p3);
			break;

		case eMEVT_BUS_DESTROY :
			break;

		default:
			break;
	}

	if(eBusRes != ERR_BUS_NO_ERROR)
	{
		return eBusRes;
	}

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}
#endif

