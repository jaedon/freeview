/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <osd_gfx.h>
#include <osd_font.h>

//#include <ap_util_osd.h>
#include <mgr_live.h>
#include <mgr_cas.h>

#include <app_manager_cas.h>

#include "../local_include/_xmgr_cas_cx_adapt.h"
#include "../local_include/_xmgr_cas_cx_util.h"
#include "../local_include/_xmgr_cas_cx.h"
#include "../../../local_include/_xmgr_cas.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

STATIC HUINT8	s_szMsg[256];

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#define	CX_UI_MAILBOX_MSG_DISPLAY_TIME			5000 // mili-second

#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)

#define USE_DIRECT_VK_TIMER		// FP Test 시, Duration이 10ms~100ms 이상 오차 발생하여, Direct로 VK Timer 호출함 In LABWISE

#define CX_UI_FP_FPPROC_CREATE					0
#define CX_UI_FP_FPPROC_TIMER					1


//#define	CX_FINGERPRINT_DRAW_FIX				// used Fix Window Size

#define	CX_UI_FP_STARTTIMER_ID					1
#define	CX_UI_FP_DURATIONTIMER_ID				2

#define	CX_UI_FP_TEXT_XPOS_OFFSET_PILLARBOX		92	// PILLABOX mode의 경우 OFFSET

#define	CX_UI_FP_TEXT_XPOS_OFFSET				30
#define	CX_UI_FP_TEXT_YPOS_OFFSET				12
#define	CX_UI_FP_TEXT_XPOS_OFFSET_EPG			60
#define	CX_UI_FP_TEXT_YPOS_OFFSET_EPG			40

#define	CX_UI_FP_TEXT_MARGIN					20
#define	CX_UI_FP_TEXT_GAP						10
#endif	// End of defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)


#if defined(CONFIG_MW_CAS_CONAX_TEXT)
//#define	CX_USER_TEXT_DRAW_FIX				// USER Text Draw X(0) 에서 With 만큼 그린다.
//#define	CX_USER_TEXT_DRAW_CENTER			// USER Text Draw X(Center) 에서 With 만큼 그린다.
#define	CX_USER_TEXT_DRAW_HALFOSD				// USER Text Draw X(0) 에서 With(1280)만큼 그린다.

#define	CX_UI_USER_TEXT_X						0
#define	CX_UI_USER_TEXT_Y						70

#define	CX_UI_USER_TEXT_W						1280
#define	CX_UI_USER_TEXT_H						170
#define	CX_UI_USER_TEXT_MARGIN					95

#define	CX_UI_UT_STARTTIMER_ID					3
#define	CX_UI_UT_DURATIONTIMER_ID				4
#define	CX_UI_UT_INTERVALTIMER_ID				5


#define	CX_UI_MAILICON_APP_W					91
#define	CX_UI_MAILICON_APP_H					92
#define	CX_UI_MAILICON_APP_X					1000
#define	CX_UI_MAILICON_APP_Y					70

#define	CX_UI_MAILICON_ID 						10
#define CX_UI_UT_INTERVAL_OFFSET				1		// Interval은 repeat-1 의 횟수
#define CX_UT_OFFSET							CX_UI_UT_INTERVAL_OFFSET

#endif	// End of defined(CONFIG_MW_CAS_CONAX_TEXT)

//#define	CX_UI_INFO_TIMEOUT_10SEC				DLG_TIMEOUT_10SEC


#if defined(CONFIG_PROD_HDPVR5000T)
	#define APPLY_CONAX_SC_MMI_TIMEOUT
#endif

#if defined(APPLY_CONAX_SC_MMI_TIMEOUT)			// conax s/c mmi timeout 적용... 일단 define으로 묶음...
	#define	CX_UI_MMI_TIMER_ID						0x0132DCB8
	#define	CX_UI_MMI_TIMEOUT						5000
#endif

#define FONT_SIZE_DEAULT_OFFSET					2


#define XmgrCasCxUiAssert(c) 						((c) ? 0 : (HxLOG_Print("\n==[xmgr_cas_CxUI]== assertion on %s: line%d\n\n", __FILE__, __LINE__)))
/*******************************************************************/
/********************      Type define		    ********************/
/*******************************************************************/
typedef struct
{
	AP_DialogInstance_t		InfoDlgInstance;
	BUS_Callback_t			InfoCallBack;
	HINT32					lTimeout;
} XmgrCas_CxUiInfoContext_t;


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
//STATIC HUINT8	s_szCasCx_Message[TEXT_SIZE512];

STATIC AP_PinDialogInstance_t	s_CheckPinCodeInstance;
STATIC BUS_Callback_t			s_CheckCallBack;

STATIC XmgrCas_CxUiInfoContext_t	s_CasCx_InfoContext;

#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
STATIC CxUiMsgInfo_t			s_stCxUiFPInfo; 		// for Finger Print
#endif	// End of defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)

#if defined(CONFIG_MW_CAS_CONAX_TEXT)
STATIC CxUserText_t				s_stCasCx_UserText;
STATIC CxUiMsgInfo_t			s_stCxUiUTInfo; 		// for Finger Print
#endif


/*******************************************************************/
/********************      	Static Prototype	********************/
/*******************************************************************/
STATIC BUS_Result_t	xproc_cas_CxMsg (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t	xproc_cas_CxCheck (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t	xproc_cas_CxInfo (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t	xproc_cas_CxCmdFp (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t	xproc_cas_CxCmdUt (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

// Check Msg
STATIC void			xmgr_cas_CxCheckMsgInit(HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t	xproc_cas_CxCheckMsg(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

// Info Msg
STATIC XmgrCas_CxUiInfoContext_t*	xmgr_cas_CxGetInfoContext(void);
STATIC BUS_Result_t				xproc_cas_CxInfoMsgResponse(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t				xproc_CxInfoMsgDialog(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);


STATIC HINT16		xmgr_cas_CxGetNumberofLinesFromWidth(HUINT8 *message, HINT32 nfontsize, HINT32 nwidth);
#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
// Finger Print
STATIC HERROR		xmgr_cas_CxCommandFingerPrintProcess(Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

#if 0 // not use for octo2.0
STATIC HERROR		xmgr_cas_CxFingerPrintMessageFixSize(HINT32 lMsg);
STATIC HERROR		xmgr_cas_CxFingerPrintMessageVariableSize(HINT32 lMsg);
STATIC BUS_Result_t	xmgr_cas_CxFingerPrintDrawFrame(OSD_Screen_t *screen, OSD_Area_t *area, OSD_Rect_t *rect, GWM_GuiObject_t object, void *arg, HINT32 param1);
#endif
#endif	// End of defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)

#if defined(CONFIG_MW_CAS_CONAX_TEXT)
// User Text
STATIC HERROR		xmgr_cas_CxCommandUserTextProcess(Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
#if 0 // not use for octo2.0
STATIC HERROR		xmgr_cas_CxUserText_Message(void);
STATIC HERROR		xmgr_cas_CxUserText_MessageByHalfOsd(void);
STATIC BUS_Result_t	xmgr_cas_CxUserText_DrawFrame(OSD_Screen_t *screen, OSD_Area_t *area, OSD_Rect_t *rect, GWM_GuiObject_t object, void *arg, HINT32 param1);
#endif
STATIC HERROR		xmgr_cas_CxUserTextSetMessage(HINT32 lMsg);
STATIC void			xmgr_cas_CxUserTextSetTimer(void);
STATIC BUS_Result_t	xmgr_cas_CxUserTextPause(void);
STATIC BUS_Result_t	xmgr_cas_CxUserTextResume(void);
#if 0 // not use for octo2.0
#if defined(CONFIG_MW_CAS_CONAX_MAIL)
STATIC BUS_Result_t	xmgr_cas_CxMailDrawBannerIcon(OSD_Screen_t *screen, OSD_Area_t *area,
										OSD_Rect_t *rect, GWM_GuiObject_t object, void *arg, int param1);
#endif	// End of #if defined(CONFIG_MW_CAS_CONAX_MAIL)
#endif

STATIC HERROR xmgr_cas_CxUiSendBannerMessage(DxCAS_GroupId_e eCasGroup, HUINT32 ulUiHandle, HUINT8 *pszMessage);

#endif	// End of defined(CONFIG_MW_CAS_CONAX_TEXT)

/*******************************************************************/
/********************      Local Functions      ********************/
/*******************************************************************/

#define ________CHECk_MSG_____________________________________

STATIC void	xmgr_cas_CxCheckMsgInit(HINT32 p1, HINT32 p2, HINT32 p3)
{
	CxUiMsgInfo_t	*stCxUiInfo = NULL;

	HxLOG_Print("[Enter]\r\n");

	XMGR_CAS_CX_PARAM_UNUSED(p2);
	XMGR_CAS_CX_PARAM_UNUSED(p3);

	stCxUiInfo = (CxUiMsgInfo_t*)p1;

	s_CheckPinCodeInstance.parentAppCallback = xproc_cas_CxMain;

	switch (stCxUiInfo->eUiMsgType)
	{
		case	eCxUi_GroupMr_Ask:
			s_CheckPinCodeInstance.ulDlgAttributes = (XmgrCas_CxPinCheckEnterCaInDlg_e)eCX_PinCheckEnter_MR;
			break;

		case	eCxUi_GroupPpv_AcceptViewing:
			s_CheckPinCodeInstance.ulDlgAttributes = (XmgrCas_CxPinCheckEnterCaInDlg_e)eCX_PinCheckEnter_AcceptView;
			break;

		case	eCxUi_GroupPpv_TokenPpv:
			s_CheckPinCodeInstance.ulDlgAttributes = (XmgrCas_CxPinCheckEnterCaInDlg_e)eCX_PinCheckEnter_TokenDebit;
			break;

		default:
			HxLOG_Critical("\n\n");
			break;
	}

	HxLOG_Print("[Exit]\r\n");
}


STATIC BUS_Result_t	xproc_cas_CxCheckMsg(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrCas_CxPinCheckStepInDlg_e	ePinCheckStep;

	switch(message)
	{
		case eMEVT_BUS_CREATE :
			HxLOG_Print("[Enter]\r\n");
			ePinCheckStep = eCX_PinCheckStep_InDlg_InputPincode;
			xmgr_cas_CxCheckMsgInit(p1, p2, p3);
			return	xproc_cas_CxCheckPinDialog(&s_CheckPinCodeInstance, message, hAction, (HINT32)ePinCheckStep, p1, p3);

		case eMEVT_BUS_DESTROY :
			// TODO: Destory 확인해야함...
			//BUS_SendMessage(NULL, MSG_APP_NOTIFY_CONAX_MSGUI_DESTROYED, (Handle_t)NULL, 0, 0, 0);
			break;

		default:
			break;
	}

	return	xproc_cas_CxCheckPinDialog(&s_CheckPinCodeInstance, message, hAction, p1, p2, p3);
}


/**
	@brief     xproc_cas_CxCheck
	Check & Info Msg with Pin-code . 		\n
	for example, 							\n
	@brief	p1 - structure of CxUiMsgInfo_t	\n
			p2 - XmgrCas_CxUiPopupMode_e		\n
			p3 - 0
	@return .
  */
STATIC BUS_Result_t	xproc_cas_CxCheck(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32	lUiMode = eCAS_CX_POPUP_MODE_NONE;

	switch(message)
	{
		case eMEVT_BUS_CREATE :
			HxLOG_Print("[Enter]\r\n");
			lUiMode = p2;
			s_CheckCallBack = NULL;

			if (lUiMode & eCAS_CX_POPUP_MODE_CHECK)
			{
				s_CheckCallBack = xproc_cas_CxCheckMsg;
			}
			else if (lUiMode & eCAS_CX_POPUP_MODE_INFO)
			{
				s_CheckCallBack = xproc_cas_CxInfo;
			}
			else
			{
				HxLOG_Critical("\n\n");
			}
			break;

		case eMEVT_BUS_DESTROY :
			break;

		default:
			break;
	}

	if (s_CheckCallBack != NULL)
	{
		return s_CheckCallBack(message, hAction, p1, p2, p3);
	}

	//BUS_MGR_Destroy(0);

	return	ERR_BUS_INITIAL_FAILED;

}


#define ________INFO_MSG_____________________________________

STATIC XmgrCas_CxUiInfoContext_t*	xmgr_cas_CxGetInfoContext(void)
{
	return &s_CasCx_InfoContext;
}


STATIC BUS_Result_t	xproc_cas_CxInfoMsgResponse(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrCas_CxUiInfoContext_t	*stInfoContext = xmgr_cas_CxGetInfoContext();

	switch (message)
	{
		case eMEVT_BUS_CREATE :
			HxLOG_Print("[Enter]\r\n");
			// TODO: 타이머 셋팅
			#if 0 // not use for octo2.0
			if (stInfoContext->lTimeout > 0)
			{
				BUS_SetTimer(RESPONSE_MSG_EXP_TIMER_ID, stInfoContext->lTimeout * CAS_CX_SECONDS);
			}
			#endif
			break;

		case eMEVT_BUS_TIMER:
			// TODO: 타이머 셋팅
			#if 0 // not use for octo2.0
			if ((p1 == RESPONSE_MSG_EXP_TIMER_ID) && (stInfoContext->lTimeout > 0))
			{
				BUS_MGR_Destroy(NULL);
			}
			#endif
			return	MESSAGE_BREAK;
		// TODO: Blocking
		#if 0 // not use for octo2.0
		case MSG_GWM_KEYDOWN :
			if (stInfoContext->lTimeout > 0)	// timeout mode
			{
				return	BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
			}
			else						// always mode
			{
				AP_KeyConv_UiCommon(&p1);
				switch (p1)
				{
					case	KEY_CH_PLUS:
					case	KEY_CH_MINUS:
						return MESSAGE_PASS;

					case	KEY_EXIT:
					case	KEY_OK:
						BUS_MGR_Destroy(NULL);
						break;
					default:
						break;
				}
				return	MESSAGE_BREAK;
			}
			break;
		#endif
		case eMEVT_BUS_DESTROY:
			// TODO: Destory 확인해야함...
			//BUS_SendMessage(NULL, MSG_APP_NOTIFY_CONAX_MSGUI_DESTROYED, (Handle_t)NULL, 0, 0, 0);
			break;

		default :
			break;
	}

	//return	AP_ResponseMessage_ProtoProc(message, hAction, p1, p2, p3);
	return ERR_OK;
}


STATIC BUS_Result_t	xproc_CxInfoMsgDialog(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrCas_CxUiInfoContext_t	*stInfoContext = xmgr_cas_CxGetInfoContext();

	switch (message)
	{
		case eMEVT_BUS_CREATE :
			HxLOG_Print("[Enter]\r\n");
			break;

		case eMEVT_BUS_TIMER:
			// TODO: Destory 확인해야함... 어디에 사용하지..... 타이머로 인해 사용될지 확인해야함.
			#if 0 // not use for octo2.0
			if (p1 == DLG_EXP_TIMER_ID)
			{
				BUS_MGR_Destroy(NULL);

				BUS_SendMessage(stInfoContext->InfoDlgInstance.thisAppCallback, MSG_DLG_TIMEOUT, (int)NULL, 0, 0, 0);
			}
			#endif
			return	MESSAGE_BREAK;
		// TODO: Blocking
		#if 0 // not use for octo2.0
		case MSG_GWM_KEYDOWN :
			AP_KeyConv_UiCommon(&p1);
			switch (p1)
			{
				case	KEY_EXIT:
					BUS_MGR_Destroy(NULL);
					break;
				default:
					break;
			}
			break;
		case MSG_DLG_CLICKED:
			{
				DLG_ClickedButton_t 	eClickButton;
				eClickButton = (DLG_ClickedButton_t)p1;
				switch (eClickButton)
				{
					case eDLG_CLICKED_BTN_MIDDLE :
						//BUS_MGR_Destroy(NULL);
						break;

					default:
						break;
				}
			}
			return	MESSAGE_BREAK;

		case MSG_DLG_TIMEOUT :
			//BUS_MGR_Destroy(NULL);
			return	MESSAGE_BREAK;
#endif

		case eMEVT_BUS_DESTROY:
			// TODO: Destory 확인해야함...
			//BUS_SendMessage(NULL, MSG_APP_NOTIFY_CONAX_MSGUI_DESTROYED, (Handle_t)NULL, 0, 0, 0);
			break;
	}

	//return	AP_Dialog_ProtoProc(&stInfoContext->InfoDlgInstance, message, hAction,p1, p2, p3);
	return ERR_OK;
}

STATIC HERROR xmgr_cas_CxUiSendBannerHideMessage(DxCAS_GroupId_e eCasGroup, HUINT32 ulUiHandle)
{
	HERROR hErr;
	OxMgrCasUiEvtBanner_t	stEvent;

	HxSTD_memset(&stEvent, 0, sizeof(OxMgrCasUiEvtBanner_t));
	stEvent.ulUiHandle = (HUINT32)ulUiHandle;
	/* Display할 String length를 0으로 할 경우 Banner 를 hide시킨다. */
	stEvent.stBanner.stString.ulStringLength = 0;

	hErr = MGR_APPMGR_MODULE_SendUiEvent (eCasGroup, 0, EVT_CASUI_BANNER, &stEvent);

	if(hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
	}

	return hErr;
}
STATIC HERROR xmgr_cas_CxUiSendBannerMessage(DxCAS_GroupId_e eCasGroup, HUINT32 ulUiHandle, HUINT8 *pszMessage)
{
	HERROR hErr;
	OxMgrCasUiEvtBanner_t	stEvent;

	HxSTD_memset(&stEvent, 0, sizeof(OxMgrCasUiEvtBanner_t));
	stEvent.ulUiHandle = (HUINT32)ulUiHandle;
	MWC_UTIL_DvbStrncpy(stEvent.stBanner.stString.szString, pszMessage, MGR_CAS_STR_LENGTH_LONG-1);
	stEvent.stBanner.stString.szString[MGR_CAS_STR_LENGTH_LONG-1] = '\0';
	stEvent.stBanner.stString.ulStringLength = SvcCas_UtilStrLen(stEvent.stBanner.stString.szString);

	hErr = MGR_APPMGR_MODULE_SendUiEvent (eCasGroup, 0, EVT_CASUI_BANNER, &stEvent);

	if(hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
	}

	return hErr;
}
STATIC BUS_Result_t xmgr_cas_CxUiMsgEvtCasOk(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR hErr = ERR_OK;
	if(xmgr_cas_CxUiSendBannerHideMessage(eDxCAS_GROUPID_CX, (HUINT32)xproc_cas_CxMsg) != ERR_OK)
	{
		HxLOG_Critical("\n\n");
	}
	return hErr;
}
STATIC BUS_Result_t xmgr_cas_CxUiMsgEvtCasFail(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR						hError = ERR_OK;
	HUINT8	szCasCx_Message[TEXT_SIZE512];

	HxLOG_Print("eMEVT_CAS_FAIL! \n");

	if(xmgr_cas_CxGetErrorData(p1, szCasCx_Message) != ERR_OK)
	{
		return ERR_FAIL;
	}
	/* Session이 Start 되어 Application과 통신할 준비가 됨. */
	//if(pstContext->bSessionStarted == TRUE)
	{
		if(xmgr_cas_CxUiSendBannerMessage(eDxCAS_GROUPID_CX, (HUINT32)xproc_cas_CxMsg, szCasCx_Message) != ERR_OK)
		{
			HxLOG_Critical("\n\n");
		}
	}

	return ERR_BUS_MESSAGE_PASS;
}
STATIC BUS_Result_t xmgr_cas_CxUiMsgEvtCasCheck(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t eBusErr = ERR_BUS_NO_ERROR;

	return eBusErr;
}


/**
	@brief     xproc_cas_CxMsg
	System Msg 출력 - Smart Card Status . 	\n
	for example, 							\n
	@brief	p1 - structure of CxUiMsgInfo_t	\n
			p2 - XmgrCas_CxUiPopupMode_e		\n
			p3 - 0
	@return .
  */
STATIC BUS_Result_t	xproc_cas_CxMsg(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxLOG_Print("[Enter]\r\n");
#if defined(APPLY_CONAX_SC_MMI_TIMEOUT)
	CxUiMsgInfo_t	*pstCxUiInfo;
#endif
	HERROR		eBusRes = ERR_OK;
	switch(message)
	{
		case eMEVT_BUS_CREATE :
			BUS_MGR_SetMgrName(BUS_MGR_GetThis(), "xproc_cas_CxMsg");

#if defined(APPLY_CONAX_SC_MMI_TIMEOUT)
			// PT에서 timeout 없어서 fail난 항목들 work-around code 3가지 경우에 대해서만 적용한다. -> "Incorrect card", "Incorrect Conax card", "Card problem - check card"
			pstCxUiInfo = (CxUiMsgInfo_t*)p1;

			if(pstCxUiInfo->eUiMsgType == eCxUi_GroupSc_Incorrect ||
				pstCxUiInfo->eUiMsgType == eCxUi_GroupSc_IncorrectConax || pstCxUiInfo->eUiMsgType == eCxUi_GroupSc_CommFail)
			{
				BUS_SetTimer (CX_UI_MMI_TIMER_ID, CX_UI_MMI_TIMEOUT);
			}
#endif
			break;
		case eMEVT_LIVE_NOTIFY_SVC_CHANGED:
		case eMEVT_CAS_OK:
			eBusRes = xmgr_cas_CxUiMsgEvtCasOk(message, hAction, p1, p2, p3);
			break;

		case eMEVT_CAS_FAIL:
			eBusRes = xmgr_cas_CxUiMsgEvtCasFail(message, hAction, p1, p2, p3);
			break;

		case eMEVT_CAS_CHECK:
			eBusRes = xmgr_cas_CxUiMsgEvtCasCheck(message, hAction, p1, p2, p3);
			break;

#if defined(APPLY_CONAX_SC_MMI_TIMEOUT)
		case eMEVT_BUS_TIMER:
			if (p1 == CX_UI_MMI_TIMER_ID)
			{
				BUS_KillTimer (CX_UI_MMI_TIMER_ID);
				//BUS_MGR_Destroy(NULL);
				return MESSAGE_BREAK;
			}
			break;
#endif
#if 0 // not use for octo2.0
		case MSG_BUS_QUIT_UI:
			//usb가 인식되면 MSG_BUS_QUIT_UI메세지를 뿌림. 여기서 막지 않으면, BUS_ProcessMessageDefault함수에서 본 proc을 destory함.
			return MESSAGE_PASS;
#endif

		case eMEVT_BUS_DESTROY :
			// TODO: Destory 확인해야함...
			//BUS_SendMessage(NULL, MSG_APP_NOTIFY_CONAX_MSGUI_DESTROYED, (Handle_t)NULL, 0, 0, 0);
			break;

		default:
			break;
	}

	//return AP_StatusMessage_ProtoProc(message, hAction, (HINT32)s_szCasCx_Message, p2, p3);
	return ERR_OK;
}


/**
	@brief     xproc_cas_CxInfo
	Check & Info Msg 		 				\n
	for example, 							\n
	@brief	p1 - structure of CxUiMsgInfo_t	\n
			p2 - XmgrCas_CxUiPopupMode_e		\n
			p3 - 0
	@return .
 */
STATIC BUS_Result_t	xproc_cas_CxInfo(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR		hError = ERR_FAIL;
	HINT32		lUiMode = eCAS_CX_POPUP_MODE_NONE;
	XmgrCas_CxUiInfoContext_t	*stInfoContext = xmgr_cas_CxGetInfoContext();

	switch (message)
	{
		case eMEVT_BUS_CREATE :
			{
				HxLOG_Print("[Enter]\r\n");

				HxSTD_memset(s_szMsg, 0, 256);

				HxSTD_memset(stInfoContext, 0x00, sizeof(XmgrCas_CxUiInfoContext_t));
				stInfoContext->InfoCallBack = NULL;
				lUiMode = p2;

				if ((lUiMode & eCAS_CX_POPUP_MODE_INFO) && (lUiMode & eCAS_CX_POPUP_MODE_TIMEOUT))
				{
					hError = xmgr_cas_CxGetInfoData(p1, s_szMsg, 256);
					if (hError == ERR_OK)
					{
						p1 = (HINT32)s_szMsg;
						p2 = (HINT32)TRUE;
						stInfoContext->lTimeout = 0x0000000A;
						stInfoContext->InfoCallBack = xproc_cas_CxInfoMsgResponse;
					}
				}
				else if ((lUiMode & eCAS_CX_POPUP_MODE_INFO) && (lUiMode & eCAS_CX_POPUP_MODE_USEREXIT))
				{
					hError = xmgr_cas_CxGetInfoData(p1, s_szMsg, 256);
					if (hError == ERR_OK)
					{
						#if 0 // not use for octo2.0
						AP_Dialog_SetDialog(&stInfoContext->InfoDlgInstance, BUS_MGR_GetAppCallback(BUS_MGR_GetThis()),
											DLG_TYPE_NO_EXIT | DLG_OK, NULL, (HUINT8*)s_szMsg);
						#endif
						stInfoContext->InfoCallBack = xproc_CxInfoMsgDialog;
					}
				}
				else if (lUiMode & eCAS_CX_POPUP_MODE_INFO)
				{
					hError = xmgr_cas_CxGetInfoData(p1, s_szMsg, 256);
					if (hError == ERR_OK)
					{
						p1 = (HINT32)s_szMsg;
						p2 = (HINT32)TRUE;
						stInfoContext->InfoCallBack = xproc_cas_CxInfoMsgResponse;
					}
				}
				else
				{
					hError = ERR_FAIL;
					HxLOG_Critical("\n\n");
				}

				if (hError == ERR_FAIL)
				{
					//BUS_MGR_Destroy(NULL);
					HxLOG_Critical("\n\n");
				}
			}
			break;

		case eMEVT_BUS_DESTROY :
			break;

		default :
			break;
	}

	if (stInfoContext->InfoCallBack != NULL)
	{
		return stInfoContext->InfoCallBack(message, hAction, p1, p2, p3);
	}

	return	ERR_BUS_INITIAL_FAILED;
}

#if 0 // not use for octo2.0
STATIC HINT16	xmgr_cas_CxGetNumberofLinesFromWidth(HUINT8 *message, HINT32 nfontsize, HINT32 nwidth)
{
	HINT32		nNumOfMsgTextLine = 0;
	HINT32		nTotal = 0;
	HINT32		nStrLen = 0;
	HINT32		nNumOfChForOneLine = 0;

	OSD_Font_t	*pFont = NULL;
	eHxCP_CodePage_e	encoding = eHCodepage_MAX;

	pFont = OSD_FONT_Get(eFont_SystemNormal);
	OSD_FONT_SetSize(pFont,nfontsize);
	nTotal = MWC_UTIL_GetLocaleCodeLen (&(message[nTotal]));
	do
	{
		nNumOfChForOneLine = 0;
		nStrLen = SvcCas_UtilStrLen(&(message[nTotal]));
		encoding= MWC_UTIL_GetEncodingByLocaleCode((const HUINT8 *)message);

		OSD_FONT_CountChByWidth_SingleByte(
			pFont, nwidth, encoding,
			nStrLen, &(message[nTotal]), &nNumOfChForOneLine);

		if(nNumOfChForOneLine != 0)
		{
			nNumOfMsgTextLine++;
		}
		nTotal += (nNumOfChForOneLine - 1);

	} while(nNumOfChForOneLine != 0);

	OSD_FONT_Release(pFont);

	return (HINT16)nNumOfMsgTextLine;
}
#endif


#define ________CMD_MSG_____________________________________

STATIC BUS_Result_t	xproc_cas_CxCmdFp(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR		hError= ERR_FAIL;
	HINT32		lCmd_type = eCAS_CX_POPUP_MODE_NONE;

#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
	STATIC HBOOL	s_bInit = FALSE;
#endif

	switch(message)
	{
		case eMEVT_BUS_CREATE :
			HxLOG_Print("[Enter]\r\n");
			lCmd_type = p1;
			if (lCmd_type & eCAS_CX_POPUP_MODE_FINGERPRINT)
			{
#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
				if(s_bInit == FALSE)
				{
					HxSTD_memset(&s_stCxUiFPInfo, 0x00, sizeof(CxUiMsgInfo_t));
					s_bInit = TRUE;
				}

				hError = xmgr_cas_CxCommandFingerPrintProcess(hAction, p2, 0 , CX_UI_FP_FPPROC_CREATE);
				if (hError != ERR_OK)
				{
					HxLOG_Critical("\n\n");
					return	ERR_FAIL;
				}
#endif
			}
			else
			{
				HxLOG_Critical("\n\n");
				return	ERR_FAIL;
			}
			break;

		case eMEVT_BUS_TIMER:
#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
			if (p1 == CX_UI_FP_STARTTIMER_ID)
			{
				HxLOG_Print("eMEVT_BUS_TIMER : CX_UI_FP_STARTTIMER_ID\r\n");
				BUS_KillTimer(CX_UI_FP_STARTTIMER_ID);
				xmgr_cas_CxCommandFingerPrintProcess(hAction, (HINT32)&s_stCxUiFPInfo, p2, CX_UI_FP_FPPROC_TIMER);
				return	MESSAGE_BREAK;
			}
#endif	// End of defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
			break;

		case eMEVT_BUS_DESTROY :
#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
			BUS_KillTimer(CX_UI_FP_STARTTIMER_ID);
			//BUS_MGR_Destroy(xproc_cas_CxFingerPrint);
#endif
			break;

		default:
			break;
	}

	return	BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}


STATIC BUS_Result_t	xproc_cas_CxCmdUt(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR		hError= ERR_FAIL;
	HINT32		lCmd_type = eCAS_CX_POPUP_MODE_NONE;

	switch(message)
	{
		case eMEVT_BUS_CREATE :
			HxLOG_Print("[Enter]\r\n");
			lCmd_type = p1;
			if (lCmd_type & eCAS_CX_POPUP_MODE_USERTEXT)
			{
#if defined(CONFIG_MW_CAS_CONAX_TEXT)
				HxSTD_memset(&s_stCxUiUTInfo, 0x00, sizeof(CxUiMsgInfo_t));
				hError = xmgr_cas_CxCommandUserTextProcess(hAction, p2, 0, 0);
				if (hError != ERR_OK)
				{
					HxLOG_Critical("\n\n");
					return	ERR_FAIL;
				}
#endif
			}
			else
			{
				HxLOG_Critical("\n\n");
				return	ERR_FAIL;
			}
			break;

		case eMEVT_BUS_TIMER:
#if defined(CONFIG_MW_CAS_CONAX_TEXT)
			if (p1 == CX_UI_UT_STARTTIMER_ID)
			{
				HxLOG_Print("eMEVT_BUS_TIMER : CX_UI_UT_STARTTIMER_ID\r\n");
				BUS_KillTimer(CX_UI_UT_STARTTIMER_ID);
				xmgr_cas_CxCommandUserTextProcess(hAction, (HINT32)&s_stCxUiUTInfo, CX_UI_UT_STARTTIMER_ID, p3);
				return	MESSAGE_BREAK;
			}
#endif	// End of CAS_CX_UI_UTTIMER_ID
			break;

		case eMEVT_BUS_DESTROY :
#if defined(CONFIG_MW_CAS_CONAX_TEXT)
			BUS_KillTimer(CX_UI_UT_STARTTIMER_ID);
			//BUS_MGR_Destroy(xproc_cas_CxUserText);
#endif
			break;

		default:
			break;
	}

	return	BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}


#define ________FINGER_PRINT________________________________
#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)

#if DEBUG	// Duration 디버깅 용도
unsigned long	Check_Start_FPTime;
unsigned long	Check_Finish_FPTime;
STATIC void xmgr_cas_CxSetStartTimeStamp()
{
	Check_Start_FPTime = VK_TIMER_GetSystemTick();
}
STATIC void xmgr_cas_CxSetFinishTimeStamp()
{
	Check_Finish_FPTime = VK_TIMER_GetSystemTick();
}

STATIC void xmgr_cas_CxPrintTimeStamp()
{
	HUINT32			ulCurrentTime = 0;
	if (CXUTIL_GetTime(&ulCurrentTime) != ERR_OK)
	{
		return	ERR_FAIL;
	}

	HxLOG_Error("[FP] Duration ( %ld), Start(0x%x/%d), Finish(0x%x/%d), Current(0x%x/%d)\n",
								(Check_Finish_FPTime-Check_Start_FPTime),
								Check_Start_FPTime,Check_Start_FPTime,
								Check_Finish_FPTime,Check_Finish_FPTime,
								ulCurrentTime,ulCurrentTime));
	Check_Start_FPTime = 0;
	Check_Finish_FPTime = 0;
}
#endif


STATIC HERROR	xmgr_cas_CxCommandFingerPrintProcess(Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HBOOL		bIsTimePrint;
	HERROR		hError = ERR_OK;
	HUINT16		usDuration = 0;
	HULONG		ulTime = 0;
	CxUiMsgInfo_t	*stCxUiInfo = NULL;

	XMGR_CAS_CX_PARAM_UNUSED(p2);

	hError = xmgr_cas_CxOnFingerPrint(p1, &bIsTimePrint, &ulTime, &usDuration);
	if (hError == ERR_OK)
	{
		/* MSG_CREATE 인 경우에만 parameter에서 넘어온 structure를 STATIC variable에 저장 */
		if (p3 == CX_UI_FP_FPPROC_CREATE)
		{
			stCxUiInfo = (CxUiMsgInfo_t*)p1;
			#if 0 // not use for octo2.0
			if(BUS_MGR_Get(xproc_cas_CxFingerPrint) != NULL)
			{
				if( (s_stCxUiFPInfo.fp_bFPFromProtectedData == TRUE) &&
					(stCxUiInfo->fp_bFPFromProtectedData == FALSE) )
				{
					return ERR_FAIL;
				}
			}
			#endif

			s_stCxUiFPInfo.eUiMsgType = stCxUiInfo->eUiMsgType;
			s_stCxUiFPInfo.fp_ulTime = stCxUiInfo->fp_ulTime;
			HxSTD_memcpy(s_stCxUiFPInfo.fp_ucCentTime, stCxUiInfo->fp_ucCentTime, sizeof(stCxUiInfo->fp_ucCentTime));
			s_stCxUiFPInfo.fp_usDuration = stCxUiInfo->fp_usDuration;
			s_stCxUiFPInfo.fp_usPosX = stCxUiInfo->fp_usPosX;
			s_stCxUiFPInfo.fp_usPosY = stCxUiInfo->fp_usPosY;
			s_stCxUiFPInfo.fp_usHeight = stCxUiInfo->fp_usHeight;
			MWC_UTIL_DvbStrcpy(s_stCxUiFPInfo.fp_szText, stCxUiInfo->fp_szText);
			s_stCxUiFPInfo.fp_ucDisplayed = stCxUiInfo->fp_ucDisplayed;
			s_stCxUiFPInfo.fp_bFPFromProtectedData = stCxUiInfo->fp_bFPFromProtectedData;
		}

		if (bIsTimePrint == TRUE)	// 미래 시간
		{
			HxLOG_Print("Set fingerPint Timer : CAS_CX_UI_FPTIMER_ID!! Time(%d)\n", ulTime);
			BUS_KillTimer(CX_UI_FP_STARTTIMER_ID);
			BUS_SetTimer(CX_UI_FP_STARTTIMER_ID, ulTime);
		}
		else	// 현재 or 즉시
		{
			//s_stCxUiFPInfo.fp_usDuration = usDuration;	// 일정 시간이 흐른 후의 usDuration.
			#if 0 // not use for octo2.0
			BUS_MGR_Destroy(xproc_cas_CxFingerPrint);
			BUS_MGR_Create("xproc_cas_CxFingerPrint", APP_CAS_HIGH_PRIORITY,
							xproc_cas_CxFingerPrint, hAction, (HINT32)&s_stCxUiFPInfo, 0, 0);
			#endif
		}
	}
	else
	{
		HxLOG_Print("On fingerPint Error! \n");
		return	ERR_FAIL;
	}

	return	ERR_OK;
}

#if 0 // not use for octo2.0
STATIC HERROR	xmgr_cas_CxFingerPrintMessageFixSize(HINT32 lMsg)
{
	OSD_Rect_t	rc;
	HINT32		lNumOfMsgTextLine = 0;
	HINT32		lBannerWidth = 0, lBannerHeight = 0;
	HINT32		lFrameX = 0, lFrameY = 0;
	HINT32		lTextY = 0;
	HINT32 		lFontSize = FONT_SIZE_S;
	CxUiMsgInfo_t	*stCxUiInfo = NULL;

	stCxUiInfo = (CxUiMsgInfo_t*)lMsg;
	if (stCxUiInfo == NULL)
	{
		HxLOG_Critical("\n\n");
		return	ERR_FAIL;
	}

	lNumOfMsgTextLine = xmgr_cas_CxGetNumberofLinesFromWidth(stCxUiInfo->fp_szText, lFontSize, ALERT_MSG_DESC_W2);

	lBannerWidth = ALERT_MSG_FRAME_W2;
	lBannerHeight = ALERT_MSG_FRAME_H2;
	lBannerHeight += (ALERT_MSG_DESC_Hx * (lNumOfMsgTextLine-2));

#if 0 // not use for octo2.0
	//nFrameX = (OSD_SCREEN_WIDTH - nBannerWidth) / 2;
	//nFrameY = (OSD_SCREEN_HEIGHT - nBannerHeight) / 2;
#else	// Draw Position
	lFrameX = stCxUiInfo->fp_usPosX;
	if ((lFrameX + lBannerWidth) > OSD_SCREEN_WIDTH)
	{
		lFrameX = (OSD_SCREEN_WIDTH - lBannerWidth );
		HxLOG_Critical("\n\n");
	}
	lFrameX = stCxUiInfo->fp_usPosX;
	lFrameY = stCxUiInfo->fp_usPosY;
#endif

	GWM_APP_SetAppArea(lFrameX, lFrameY, lBannerWidth, lBannerHeight);

	/* custom draw frame */
	OSD_GFX_SetRect(&rc, 0, 0, lBannerWidth, lBannerHeight);
	GWM_Frame_Create(ALERT_MSG_FRAME_ID, &rc, (char *)NULL);
	GWM_APP_SetObjectDrawMethod(ALERT_MSG_FRAME_ID, AP_LNF_MessageFrame_Draw);

	/* custom draw title */
	lTextY = ((lBannerHeight - (ALERT_MSG_DESC_Hx * lNumOfMsgTextLine)) / 2) + 2;
	OSD_GFX_SetRect(&rc, ALERT_MSG_DESC_X2, lTextY, ALERT_MSG_DESC_W2, lBannerHeight);

	/* dialog message */
	GWM_Text_Create(ALERT_MSG_DESC_ID, &rc, stCxUiInfo->fp_szText);
	GWM_Text_SetSize(ALERT_MSG_DESC_ID, lFontSize);
	GWM_Text_ChangeColor(ALERT_MSG_DESC_ID, COL(C_T_Enable), 0);
	GWM_APP_SetObjectDrawMethod(ALERT_MSG_DESC_ID, AP_LNF_PlainMultilineText_Draw);

	GWM_APP_InvalidateRect(lFrameX, lFrameY, lBannerWidth, lBannerHeight);

	if (stCxUiInfo->fp_usDuration)
	{
		BUS_SetTimer(CX_UI_FP_DURATIONTIMER_ID, stCxUiInfo->fp_usDuration * 10);
	}

	return	ERR_OK;
}


STATIC HERROR	xmgr_cas_CxFingerPrintMessageVariableSize(HINT32 lMsg)
{
	OSD_Rect_t	rc;
	HINT32		lNumOfMsgTextLine = 0;
	HINT32		lBannerWidth = 0, lBannerHeight = 0;
	HINT32		lFrameX = 0, lFrameY = 0;
	HINT32		lStrWith = 0;
	HINT32 		lFontSize = FONT_SIZE_DEAULT_OFFSET;
	HUINT32		ulStrLen = 0;
	CxUiMsgInfo_t	*stCxUiInfo = NULL;
	OSD_Font_t		*pFont = NULL;
	HUINT32		nResult = 0;
	HUINT32		ulValue = 0;
	HUINT32		nDescender = 0;

	HxLOG_Print("[Enter]\r\n");

	stCxUiInfo = (CxUiMsgInfo_t*)lMsg;
	if (stCxUiInfo == NULL)
	{
		HxLOG_Critical("\n\n");
		return	ERR_FAIL;
	}
	lFontSize += (HINT32)stCxUiInfo->fp_usHeight;
	lNumOfMsgTextLine = xmgr_cas_CxGetNumberofLinesFromWidth(stCxUiInfo->fp_szText, lFontSize, ALERT_MSG_DESC_W2);

	HxLOG_Print("[FP] origin FP size(%d / %d), PosX(%d), PosY(%d), Duration(0x%x/%d/%d)\n",
					stCxUiInfo->fp_usHeight, lFontSize,stCxUiInfo->fp_usPosX,stCxUiInfo->fp_usPosY,
					stCxUiInfo->fp_usDuration,stCxUiInfo->fp_usDuration,(stCxUiInfo->fp_usDuration * 10)));

	pFont = OSD_FONT_Get(eFont_SystemNormal);

	OSD_FONT_GetDescender(pFont, &nDescender);

 	//CONAX PT:Conax에서 오로지 숫자 char의 높이만 맞추라 하여 descender 등을 빼줌
  	//정확히 하기 위해서는 line gap 등도 고려해 줘야 함.
	lFontSize = lFontSize + nDescender*2;
	OSD_FONT_SetSize(pFont, lFontSize);


	GWM_Text_SetSize(ALERT_MSG_DESC_ID, lFontSize);
	ulStrLen = SvcCas_UtilStrLen(stCxUiInfo->fp_szText);
	if (ulStrLen == 0)
	{
		HxLOG_Critical("\n\n");
		return	ERR_FAIL;
	}

	//lStrWith = OSD_FONT_GetStringPixelWidth(pFont, eHxCP_UNICODE_DVB_UTF8, stCxUiInfo->fp_szText, ulStrLen);
	lStrWith = OSD_GFX_GetStringWidth(pFont, stCxUiInfo->fp_szText);
	lStrWith +=CX_UI_FP_TEXT_MARGIN;
	if (pFont != NULL)
	{
		OSD_FONT_Release (pFont);
	}

	lBannerWidth = lStrWith;
	lBannerHeight = CX_UI_FP_TEXT_MARGIN;
	lBannerHeight += (ALERT_MSG_DESC_Hx * lNumOfMsgTextLine);


	 //CONAX PT: 만약 fp의 좌표와 size를 잘못 보내서, fp가 잘릴 상황이라도 상관없이 그냥 뿌려야 한다.
/*
	if ((lFrameX + lBannerWidth) > OSD_SCREEN_WIDTH)
	{
		lFrameX = (OSD_SCREEN_WIDTH - lBannerWidth );
		HxLOG_Critical("\n\n");
	}
*/

	nResult = DB_PARAM_GetItem(MENUCFG_ITEMNAME_ASPECT_RATIO_ADJUST, &ulValue, 0);
//	 //CONAX PT:frame buffer에 의해 잘리는 사항 고려 필요 없다고 함.

	if((nResult == ERR_OK) && (ulValue == eArAdjust_STB_PillarBox))
	{
		lFrameX = (stCxUiInfo->fp_usPosX /*+ CX_UI_FP_TEXT_XPOS_OFFSET*/ + CX_UI_FP_TEXT_XPOS_OFFSET_PILLARBOX);
	}
	else
	{
		lFrameX = (stCxUiInfo->fp_usPosX + 5/*+ CX_UI_FP_TEXT_XPOS_OFFSET*/);
	}

//	lFrameX = stCxUiInfo->fp_usPosX;
	 //CONAX PT:Conax에서 FP 좌표는 720*576 에 맞추어서 계산하는 것이 맞다고 함.
	lFrameX = (OSD_SCREEN_WIDTH *  lFrameX) / 720;
	lFrameY = (OSD_SCREEN_HEIGHT * stCxUiInfo->fp_usPosY) / 576;
	 //정확히는 font에서 숫자 위에 여백으로 비는 부분 공간을 빼주어야 함.
	lFrameY -= nDescender*2;

#if 0 // not use for octo2.0
	// EPG Screen 에서, EPG PIP 로 FP 이동해서 Display
	if(BUS_MGR_Get(AP_EPGMAIN_Proc) == NULL)
	{
		// Do nothing....
	}
	else
	{
		#if 0 // not use for octo2.0
		HxLOG_Print("Before Converted position X(%d), Y(%d)\n",lFrameX,lFrameY);

		lFrameX = ((lFrameX/3.8) + CX_UI_FP_TEXT_XPOS_OFFSET_EPG);
		lFrameY = ((lFrameY/3.5) + CX_UI_FP_TEXT_YPOS_OFFSET_EPG);

		HxLOG_Print("After Converted position X(%d), Y(%d)\n",lFrameX,lFrameY);
		#endif
	}
#endif
#if 0 // not use for octo2.0
	GWM_APP_SetAppArea(lFrameX, lFrameY, lStrWith, lBannerHeight);
#endif
	/*
		CONAX PT에서 배경 없이 흰색으로 출력 하라고 함. - jhlee
	*/
#if 0 // not use for octo2.0
	/* custom draw frame */
	OSD_GFX_SetRect(&rc, 0, 0, lBannerWidth, lBannerHeight);
	GWM_Frame_Create(ALERT_MSG_FRAME_ID, &rc, (char *)NULL);
	GWM_APP_SetObjectDrawMethod(ALERT_MSG_FRAME_ID, xmgr_cas_CxFingerPrintDrawFrame);
#endif

	/* custom draw Text */
	OSD_GFX_SetRect(&rc, CX_UI_FP_TEXT_GAP, CX_UI_FP_TEXT_GAP, lBannerWidth - CX_UI_FP_TEXT_MARGIN, lBannerHeight);

	/* dialog message */
	GWM_Text_Create(ALERT_MSG_DESC_ID, &rc, stCxUiInfo->fp_szText);
	GWM_Text_SetSize(ALERT_MSG_DESC_ID, lFontSize);
	GWM_Text_ChangeColor(ALERT_MSG_DESC_ID, COL(C_T_Editbox), 0);
	GWM_APP_SetObjectDrawMethod(ALERT_MSG_DESC_ID, AP_LNF_PlainMultilineText_Draw);

	GWM_APP_InvalidateRect(lFrameX, lFrameY, lBannerWidth, lBannerHeight);

/* timer 등록 위치 변경
	if (stCxUiInfo->fp_usDuration)
	{
		BUS_SetTimer(CX_UI_FP_DURATIONTIMER_ID, stCxUiInfo->fp_usDuration * 10);
	}
*/

	HxLOG_Print("[Exit]\r\n");

	return	ERR_OK;
}


STATIC BUS_Result_t	xmgr_cas_CxFingerPrintDrawFrame(OSD_Screen_t *screen, OSD_Area_t *area, OSD_Rect_t *rect, GWM_GuiObject_t object, void *arg, HINT32 param1)
{
	HINT32			x1, y1, x2, y2;
	OSD_Color_t		ulColor, ulAlpha;

	/* warning remove */
	NOT_USED_PARAM(object);
	NOT_USED_PARAM(arg);
	NOT_USED_PARAM(param1);

	/* absolute coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;
	ulAlpha = 0xff;
	AP_UTILOSD_GetBGAlpha(eAP_AlphaType_A, (HINT32 *)&ulAlpha);
	ulColor = (ulAlpha << 24) | (COL(C_W_BG01) & 0x00ffffff);

	OSD_GFX_DrawRectFill(screen, x1, y1, x2, y2, ulColor);

	return ERR_BUS_NO_ERROR;
}
#endif

HERROR local_cxsc_cbFingerPrintDestroy(HUINT32 ulTimerId, void *pArg)
{
	// TODO: 타이머 미사용으로 필요없을듯...
	#if 0 // not use for octo2.0
	if(ulTimerId != s_ulFPVKTimerId)
	{
		return ERR_FAIL;
	}
	s_ulFPVKTimerId = 0;

	BUS_MGR_Destroy(xproc_cas_CxFingerPrint);

	// GWM Task Delay(?) 현상이 발생해서, Dration을 지키지 못한다.
	//   -> (Msg 처리하는데 가끔 우선순위가 밀리는듯...)
	// 아래의 코드를 넣음으로서, 문제 해결하고자 함
	// Fingerprint는 Duration이 중요함.
	GWM_DirectRedrawAll();

	#endif
	return ERR_OK;
}

HERROR xmgr_cas_CxSetFingerprint_Timer(HINT32 lMsg)
{
	CxUiMsgInfo_t	*stCxUiInfo = NULL;

	stCxUiInfo = (CxUiMsgInfo_t*)lMsg;
	if (stCxUiInfo == NULL)
	{
		HxLOG_Critical("\n\n");
		return	ERR_FAIL;
	}

	#if DEBUG
		xmgr_cas_CxSetStartTimeStamp();
	#endif

	// TODO: 타이머 미사용으로 필요없을듯...
	#if 0 // not use for octo2.0
	if (stCxUiInfo->fp_usDuration > 0)
	{
		#ifdef USE_DIRECT_VK_TIMER
			VK_TIMER_EventAfter(stCxUiInfo->fp_usDuration * 10, local_cxsc_cbFingerPrintDestroy, NULL, NULL, &s_ulFPVKTimerId);
		#else
			BUS_SetTimer(CX_UI_FP_DURATIONTIMER_ID, stCxUiInfo->fp_usDuration * 10);
		#endif
	}
	#endif
	return ERR_OK;
}
/**
	@brief     xproc_cas_CxFingerPrint
	Finger print Msg 		 				\n
	for example, 							\n
	@brief	p1 - structure of CxUiMsgInfo_t	\n
			p2 - 0							\n
			p3 - 0
	@return .
 */
BUS_Result_t	xproc_cas_CxFingerPrint(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR			hError = ERR_FAIL;

	switch (message)
	{
		case eMEVT_BUS_CREATE:
			HxLOG_Print("[Enter]\r\n");
#if 0 // not use for octo2.0
#if defined(CX_FINGERPRINT_DRAW_FIX)
			hError = xmgr_cas_CxFingerPrintMessageFixSize(p1);
#else
			hError = xmgr_cas_CxFingerPrintMessageVariableSize(p1);
#endif
#endif
			if (hError != ERR_OK)
			{
				HxLOG_Critical("\n\n");
				//BUS_MGR_Destroy(NULL);
			}
			// Kill timer
			BUS_KillTimer(CX_UI_FP_DURATIONTIMER_ID);

			xmgr_cas_CxSetFingerprint_Timer(p1);
			break;
		// TODO: Blocking
		#if 0 // not use for octo2.0
		case MSG_GWM_KEYDOWN:
			switch (p1)
			{
				case	KEY_CH_PLUS:
				case	KEY_CH_MINUS:
					BUS_MGR_Destroy(NULL);
					return MESSAGE_PASS;

				case KEY_EXIT:
					BUS_MGR_Destroy(NULL);
					return	MESSAGE_BREAK;
				/* This MSG alert usually doesn't care any kind of key inputs, */
				/* would be killed by other external app. */
				default:
					break;
			}
			break;
		#endif
		case eMEVT_BUS_TIMER:
			if (p1 == CX_UI_FP_DURATIONTIMER_ID)
			{
				HxLOG_Print("eMEVT_BUS_TIMER : CX_UI_FP_DURATIONTIMER_ID\r\n");
				BUS_KillTimer(CX_UI_FP_DURATIONTIMER_ID);
				//BUS_MGR_Destroy(NULL);

				return	MESSAGE_BREAK;
			}
			break;

		// TODO: Destory 확인해야함...
		#if 0 // not use for octo2.0
		case MSG_APP_CONAX_FINGERPRINT_DESTROY:
			BUS_MGR_Destroy(NULL);
			return MESSAGE_BREAK;
		#endif

		case eMEVT_BUS_DESTROY:
			#if DEBUG
				xmgr_cas_CxSetFinishTimeStamp();
				xmgr_cas_CxPrintTimeStamp();
			#endif

			//xmgr_cas_CxSetFingerPrintDisplayed();

			BUS_KillTimer(CX_UI_FP_DURATIONTIMER_ID);
			//BUS_MGR_Destroy(xproc_cas_CxCmdFp);

			// TODO: Destory 확인해야함...
			//BUS_SendMessage(NULL, MSG_APP_NOTIFY_CONAX_MSGUI_DESTROYED, (Handle_t)NULL, 0, 0, 0);
			break;
	}

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}
#endif

#define ________USER_TEXT________________________________
#if defined(CONFIG_MW_CAS_CONAX_TEXT)

STATIC HERROR	xmgr_cas_CxCommandUserTextProcess(Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HBOOL		bUserTextTime = FALSE;
	HERROR		hError = ERR_OK;
	HULONG		ulTime = 0;
	CxUiMsgInfo_t	*stCxUiInfo = NULL;
	CxUiMsgInfo_t	stCxTmpUiInfo;

	XMGR_CAS_CX_PARAM_UNUSED(p3);

	if (p2 == CX_UI_UT_STARTTIMER_ID)
	{
		HxLOG_Print("User Text process by CAS_CX_UI_UTTIMER_ID!!\n");
		HxSTD_memset(&stCxTmpUiInfo, 0x00, sizeof(CxUiMsgInfo_t));
	}
	else
	{
		HxLOG_Print("User Text process by Direct!!\n");
		HxSTD_memcpy(&stCxTmpUiInfo, &s_stCxUiUTInfo, sizeof(CxUiMsgInfo_t));
	}

	hError = xmgr_cas_CxOnUserText(p1, &stCxTmpUiInfo, &bUserTextTime, &ulTime);
	if (hError == ERR_OK)
	{
		stCxUiInfo = (CxUiMsgInfo_t*)p1;

#if defined(CONFIG_MW_CAS_CONAX_MAIL)
		// User Text Mail Type Icon
		if (stCxUiInfo->ut_usDuration == 0)
		{
			#if 0 // not use for octo2.0
			if(BUS_MGR_Get(xproc_cas_CxMailMessage) != NULL)
			{
				BUS_SendMessage(xproc_cas_CxMailMessage, XMGR_CAS_UPDATE_MENU, (Handle_t) NULL,	0, stCxUiInfo->ut_ucSeqNo, eCxUi_GroupMessage_Display);
			}

			if(BUS_MGR_Get(xproc_cas_CxMailMsgText) != NULL)
			{
				BUS_SendMessage(xproc_cas_CxMailMsgText, XMGR_CAS_UPDATE_MENU, (Handle_t) NULL,	0, stCxUiInfo->ut_ucSeqNo, eCxUi_GroupMessage_Display);
			}
			#endif

			//return	ERR_OK;
		}
#endif
		// User Text Immediate Message
		// 처음 받은 User Text의 경우만 setting. Timer로 들어오는 경우는 무시.
		if (p2 != CX_UI_UT_STARTTIMER_ID)
		{
			s_stCxUiUTInfo.eUiMsgType = stCxUiInfo->eUiMsgType;
			s_stCxUiUTInfo.ut_ucDisplayed = stCxUiInfo->ut_ucDisplayed;
			s_stCxUiUTInfo.ut_usDuration = stCxUiInfo->ut_usDuration;		// 이 값이 0이면 메시지를 표시하지는 않고 왔다고만 알려준다 (mailbox message). 단위 second.
			s_stCxUiUTInfo.ut_ucSeqNo = stCxUiInfo->ut_ucSeqNo;
			s_stCxUiUTInfo.ut_ucRepeat = stCxUiInfo->ut_ucRepeat;			// 없으면 1회만 출력
			s_stCxUiUTInfo.ut_usInterval = stCxUiInfo->ut_usInterval;		// 메시지를 표시하는 간격 (10초단위로 들어오나 *10하여 second단위로 저장하고 있음)
			MWC_UTIL_DvbStrcpy(s_stCxUiUTInfo.ut_szText, stCxUiInfo->ut_szText);
			HxSTD_memcpy(&s_stCxUiUTInfo.ut_stStartDataTime, &stCxUiInfo->ut_stStartDataTime, sizeof(HxDATETIME_t));

		}
		if (bUserTextTime == TRUE)
		{
			HxLOG_Print("Set Uset Text Timer : CX_UI_UT_STARTTIMER_ID (%d)!!\n", ulTime);
			BUS_KillTimer(CX_UI_UT_STARTTIMER_ID);
			BUS_SetTimer(CX_UI_UT_STARTTIMER_ID, ulTime);
		}
		else
		{
			// Mail Message Notify
			if (stCxUiInfo->ut_usDuration == 0)
			{
				/* Mail List 진입시 Normal Mail Received Icon 은 삭제한다. */
#if defined(CONFIG_MW_CAS_CONAX_MAIL)
#if 0 // not use for octo2.0
				if(BUS_MGR_Get(xproc_cas_CxMainIcon) == NULL)
				{
					BUS_MGR_Create("xproc_cas_CxMainIcon", APP_CAS_HIGH_PRIORITY,
								xproc_cas_CxMainIcon, hAction,   0, 0, 0);
				}
#endif
#endif
			}
			else
			{
				// Immediate Message Process
#if defined(CONFIG_MW_CAS_CONAX_TEXT)
#if 0 // not use for octo2.0
				//BUS_MGR_Destroy(xproc_cas_CxUserText);
				BUS_MGR_Create("xproc_cas_CxUserText", APP_CAS_PRIORITY,
							xproc_cas_CxUserText, hAction, (HINT32)&s_stCxUiUTInfo, 0, 0);
#endif
#endif
			}
		}
	}
	else
	{
		HxLOG_Print("On User Text Error!! \n");
		return	ERR_FAIL;
	}

	return	ERR_OK;
}


 HERROR		xmgr_cas_CxUserTextSetMessage(HINT32 lMsg)
{
	HUINT8			*szStr;
	CxUiMsgInfo_t	*stCxUiInfo = NULL;

	HxSTD_memset(&s_stCasCx_UserText, 0x00, sizeof(CxUserText_t));

	stCxUiInfo = (CxUiMsgInfo_t*)lMsg;
	if (stCxUiInfo == NULL)
	{
		HxLOG_Critical("\n\n");
		return	ERR_FAIL;
	}

	s_stCasCx_UserText.ucDisplayed = stCxUiInfo->ut_ucDisplayed;
	s_stCasCx_UserText.usDuration = stCxUiInfo->ut_usDuration;		// 이 값이 0이면 메시지를 표시하지는 않고 왔다고만 알려준다 (mailbox message). 단위 second.
	s_stCasCx_UserText.ucSeqNo = stCxUiInfo->ut_ucSeqNo;
	s_stCasCx_UserText.ucRepeat = stCxUiInfo->ut_ucRepeat;			// 없으면 1회만 출력
	s_stCasCx_UserText.usInterval = stCxUiInfo->ut_usInterval;		// 메시지를 표시하는 간격 (10초단위로 들어오나 *10하여 second단위로 저장하고 있음)

	HxSTD_memcpy(&s_stCasCx_UserText.stStartDataTime, &stCxUiInfo->ut_stStartDataTime, sizeof(HxDATETIME_t));

	MWC_UTIL_DvbStrcpy(s_stCasCx_UserText.szText, stCxUiInfo->ut_szText);

	return	ERR_OK;
}


STATIC void	xmgr_cas_CxUserTextSetTimer(void)
{
	HULONG		ulInterval = 0;
	HULONG		ulDuration = 0;
	if (s_stCasCx_UserText.usDuration)
	{
		ulDuration = (s_stCasCx_UserText.usDuration * CAS_CX_SECONDS);

		if (s_stCasCx_UserText.usInterval)
		{
			// Interval 정의
			// TEXT Start Time 과 다음 Start Time 의 간격을 Interval 이라고 한다.
			// Ex) Interval(20) + Duration(10) -> Display(10)가 끝난 뒤 부터, 10초간 Interval을 가지도록 함
			// 결국 Duration과 상과없은 각자 독립적으로 동작 한다.
			ulInterval = (s_stCasCx_UserText.usInterval* CAS_CX_SECONDS);

			BUS_SetTimer(CX_UI_UT_INTERVALTIMER_ID, ulInterval);
			HxLOG_Print("Set Time(%d) : CX_UI_UT_INTERVALTIMER_ID\r\n", ulInterval);
		}
		HxLOG_Print("Set Time(%d) : CX_UI_UT_DURATIONTIMER_ID\r\n", ulDuration);
		BUS_SetTimer(CX_UI_UT_DURATIONTIMER_ID, ulDuration);
	}
	else
	{
		HxLOG_Print("Set Time(%d) : CX_UI_UT_INTERVALTIMER_ID - CX_UI_MAILBOX_MSG_DISPLAY_TIME\r\n", CX_UI_MAILBOX_MSG_DISPLAY_TIME);
		BUS_SetTimer(CX_UI_UT_INTERVALTIMER_ID, CX_UI_MAILBOX_MSG_DISPLAY_TIME);
	}
}

#if 0 // not use for octo2.0
STATIC HERROR	xmgr_cas_CxUserText_Message(void)
{
	OSD_Rect_t	rc;
	HINT32		lNumOfMsgTextLine = 0;
	HINT32		lBannerWidth = 0, lBannerHeight = 0;
	HINT32		lFrameX = 0, lFrameY = 0;
	HINT32		lTextY = 0;
	HINT32 		lFontSize = FONT_SIZE_S;

	lNumOfMsgTextLine = xmgr_cas_CxGetNumberofLinesFromWidth(s_stCasCx_UserText.szText, lFontSize, ALERT_MSG_DESC_W2);

	lBannerWidth = ALERT_MSG_FRAME_W2;
	lBannerHeight = ALERT_MSG_FRAME_H2;
	lBannerHeight += (ALERT_MSG_DESC_Hx * (lNumOfMsgTextLine-2));

#if defined(CX_USER_TEXT_DRAW_FIX)
	lFrameX = CX_UI_USER_TEXT_X;
	lFrameY = CX_UI_USER_TEXT_Y;
#elif defined(CX_USER_TEXT_DRAW_CENTER)
	lFrameX = (OSD_SCREEN_WIDTH - lBannerWidth) / 2;
	lFrameY = CX_UI_USER_TEXT_Y;
#else
	lFrameX = (OSD_SCREEN_WIDTH - lBannerWidth) / 2;
	lFrameY = (OSD_SCREEN_HEIGHT - lBannerHeight) / 2;
#endif

	GWM_APP_SetAppArea(lFrameX, lFrameY, lBannerWidth, lBannerHeight);

	/* custom draw frame */
	OSD_GFX_SetRect(&rc, 0, 0, lBannerWidth, lBannerHeight);
	GWM_Frame_Create(ALERT_MSG_FRAME_ID, &rc, (char *)NULL);
	GWM_APP_SetObjectDrawMethod(ALERT_MSG_FRAME_ID, AP_LNF_MessageFrame_Draw);

	/* custom draw title */
	lTextY = ((lBannerHeight - (ALERT_MSG_DESC_Hx * lNumOfMsgTextLine)) / 2) + 2;
	OSD_GFX_SetRect(&rc, ALERT_MSG_DESC_X2, lTextY, ALERT_MSG_DESC_W2, lBannerHeight);

	/* dialog message */
	GWM_Text_Create(ALERT_MSG_DESC_ID, &rc, s_stCasCx_UserText.szText);
	GWM_Text_SetSize(ALERT_MSG_DESC_ID, lFontSize);
	GWM_Text_ChangeColor(ALERT_MSG_DESC_ID, COL(C_T_Enable), 0);
	GWM_APP_SetObjectDrawMethod(ALERT_MSG_DESC_ID, AP_LNF_PlainMultilineText_Draw);

	GWM_APP_InvalidateRect(lFrameX, lFrameY, lBannerWidth, lBannerHeight);

	return	ERR_OK;
}


STATIC HERROR	xmgr_cas_CxUserText_MessageByHalfOsd(void)
{
	OSD_Rect_t	rc;
	HINT32		lBannerWidth = 0, lBannerHeight = 0;
	HINT32		lFrameX = 0, lFrameY = 0;
	HINT32		lTextW = 0;

	lFrameX = CX_UI_USER_TEXT_X;
	lFrameY = CX_UI_USER_TEXT_Y;
	lBannerWidth = CX_UI_USER_TEXT_W;
	lBannerHeight = CX_UI_USER_TEXT_H;

	GWM_APP_SetAppArea(lFrameX, lFrameY, lBannerWidth, lBannerHeight);

	/* custom draw frame */
	OSD_GFX_SetRect(&rc, 0, 0, lBannerWidth, lBannerHeight);
	GWM_Frame_Create(ALERT_MSG_FRAME_ID, &rc, (char *)NULL);
	GWM_APP_SetObjectDrawMethod(ALERT_MSG_FRAME_ID, xmgr_cas_CxUserText_DrawFrame);

	/* custom draw title */
	lTextW = CX_UI_USER_TEXT_W - (CX_UI_USER_TEXT_MARGIN * 2);
	OSD_GFX_SetRect(&rc, CX_UI_USER_TEXT_MARGIN, ALERT_MSG_DESC_Hx, lTextW, lBannerHeight);

	/* dialog message */
	GWM_Text_Create(ALERT_MSG_DESC_ID, &rc, s_stCasCx_UserText.szText);
	GWM_Text_SetSize(ALERT_MSG_DESC_ID, FONT_SIZE_S);
	GWM_Text_ChangeColor(ALERT_MSG_DESC_ID, COL(C_T_Enable), 0);
	GWM_Text_SetAlign (ALERT_MSG_DESC_ID, TEXT_ALIGN_CENTER);

	GWM_APP_SetObjectDrawMethod(ALERT_MSG_DESC_ID, AP_LNF_PlainMultilineText_Draw);

	GWM_APP_InvalidateRect(lFrameX, lFrameY, lBannerWidth, lBannerHeight + 2);

	return	ERR_OK;
}


STATIC BUS_Result_t	xmgr_cas_CxUserText_DrawFrame(OSD_Screen_t *screen, OSD_Area_t *area, OSD_Rect_t *rect, GWM_GuiObject_t object, void *arg, HINT32 param1)
{
	HINT32			x1, y1, x2, y2;
	OSD_Color_t		ulColor, ulAlpha;

	/* warning remove */
	NOT_USED_PARAM(object);
	NOT_USED_PARAM(arg);
	NOT_USED_PARAM(param1);

	/* absolute coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w - 1;
	y2 = area->y1 + rect->y + rect->h - 1;
	ulAlpha = 0xff;
	AP_UTILOSD_GetBGAlpha(eAP_AlphaType_B, (HINT32 *)&ulAlpha);
	ulColor = (ulAlpha << 24) | (COL(C_W_BG01) & 0x00ffffff);

	OSD_GFX_DrawRectFill(screen, x1, y1, x2, y2, ulColor);

	return ERR_BUS_NO_ERROR;
}
#endif
STATIC HERROR	xmgr_cas_CxUT_ResetUTDBOnDestroy(void)
{
	if(s_stCasCx_UserText.ucRepeat > 1)
	{
		// 반복 수행 UsetText의 경우.
		// wrap around 되어 과거시간이 다시 시작 되어도,
		// 같은 내용의 User Text의 경우는 그리지 말아라 - LABATUS

		// do nothing...
	}
	else
	{
		// Mostelecom OP 요구사항
		// 한번 display 한것은 다시 display 하지 마라.
		// 여기서 삭제하지 않으면, 다시 그리지 않는다.
		//xmgr_cas_CxRemoveUserText(s_stCasCx_UserText.ucSeqNo);
	}

	HxSTD_memset(&s_stCasCx_UserText, 0x00, sizeof(CxUserText_t));
	return ERR_OK;
}
STATIC HERROR	xmgr_cas_CxUT_IncreaseDisplayCount(void)
{
	s_stCasCx_UserText.ucDisplayed++;
	return ERR_OK;
}
STATIC HUINT8	xmgr_cas_CxUT_GetDisplayCount(void)
{
	return s_stCasCx_UserText.ucDisplayed;
}
BUS_Result_t	xmgr_cas_CxUserTextPause(void)
{
	BUS_KillTimer(CX_UI_UT_INTERVALTIMER_ID);
	BUS_KillTimer(CX_UI_UT_DURATIONTIMER_ID);

	// TODO: UI Hide 해야하는 위치
	//GWM_APP_HideUI();

	return ERR_BUS_NO_ERROR;
}
BUS_Result_t	xmgr_cas_CxUserTextResume(void)
{
	// Resume과 동시에, 대기중인 Msg Display 하도록 수정.

	// TODO: UI Show 해야하는 위치
	//GWM_APP_ShowUI();
	xmgr_cas_CxUserTextSetTimer();

	// Temporary structure update
	xmgr_cas_CxUT_IncreaseDisplayCount();
	xmgr_cas_CxSetUserTextDisplayed(s_stCasCx_UserText.ucSeqNo,eCxUTDisplayCntOnce);

/*
	// Resume 할때는, 기존 Interval만큼을 더 기다렸다가, display한다.
	xmgr_cas_CxUserTextSetTimer();
	BUS_KillTimer(CX_UI_UT_DURATIONTIMER_ID);
*/
	return ERR_BUS_NO_ERROR;
}

/**
	@brief     xproc_cas_CxUserText
	Uset Text Msg	 		 				\n
	for example, 							\n
	@brief	p1 - structure of CxUiMsgInfo_t	\n
			p2 - 0							\n
			p3 - 0
	@return .
 */
BUS_Result_t	xproc_cas_CxUserText(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR			hError = ERR_FAIL;

	switch (message)
	{
		case eMEVT_BUS_CREATE:
			HxLOG_Print("[Enter]\r\n");

			// Set User Text Datas
			hError = xmgr_cas_CxUserTextSetMessage(p1);
			if (hError != ERR_OK)
			{
				//BUS_MGR_Destroy(NULL);
			}

			// Draw User Text
#if 0 // not use for octo2.0
#if defined(CX_USER_TEXT_DRAW_HALFOSD)
			xmgr_cas_CxUserText_MessageByHalfOsd();
#else
			xmgr_cas_CxUserText_Message();
#endif
#endif
			// Kill timer
			BUS_KillTimer(CX_UI_UT_DURATIONTIMER_ID);
			BUS_KillTimer(CX_UI_UT_INTERVALTIMER_ID);

			// Set Timer
			xmgr_cas_CxUserTextSetTimer();
			break;
		// TODO: Blocking
		#if 0 // not use for octo2.0
		case MSG_GWM_KEYDOWN:
			switch (p1)
			{
				case	KEY_CH_PLUS:
				case	KEY_CH_MINUS:
					BUS_MGR_Destroy(NULL);
					return MESSAGE_PASS;

				case KEY_EXIT:
					BUS_MGR_Destroy(NULL);
					return	MESSAGE_BREAK;

				case KEY_OK:
					BUS_MGR_Destroy(NULL);
					break;
				/* This MSG alert usually doesn't care any kind of key inputs, */
				/* would be killed by other external app. */
				default:
					break;
			}
			break;
		#endif
		case eMEVT_BUS_TIMER:
			if (p1 == CX_UI_UT_DURATIONTIMER_ID)
			{
				HxLOG_Print("eMEVT_BUS_TIMER : CX_UI_UT_DURATIONTIMER_ID\r\n");
				BUS_KillTimer(CX_UI_UT_DURATIONTIMER_ID);

				// Temporary structure update
				xmgr_cas_CxUT_IncreaseDisplayCount();

				// Set Displayed count
				xmgr_cas_CxSetUserTextDisplayed(s_stCasCx_UserText.ucSeqNo,eCxUTDisplayCntOnce);

				if ((s_stCasCx_UserText.usInterval == 0) && (s_stCasCx_UserText.ucRepeat == 1))
				{
					HxLOG_Critical("\n\n");
					//HxSTD_memset(&s_stCasCx_UserText, 0x00, sizeof(CxUserText_t));
					//BUS_MGR_Destroy(NULL);
				}
				else
				{
					// Hide UI when end of duration time
					// TODO: UI Hide 해야하는 위치
					//GWM_APP_HideUI();
				}

				return	MESSAGE_BREAK;
			}
			else if(p1 == CX_UI_UT_INTERVALTIMER_ID)
			{
				HxLOG_Print("eMEVT_BUS_TIMER : CX_UI_UT_INTERVALTIMER_ID\r\n");
				HxLOG_Print("User Text Displayed(%d),Repeat(%d) \r\n", xmgr_cas_CxUT_GetDisplayCount(),s_stCasCx_UserText.ucRepeat);
				BUS_KillTimer(CX_UI_UT_INTERVALTIMER_ID);

				if (xmgr_cas_CxUT_GetDisplayCount() < s_stCasCx_UserText.ucRepeat)
				{
					// TODO: UI Show 해야하는 위치
					//GWM_APP_ShowUI();
					xmgr_cas_CxUserTextSetTimer();
				}
				else
				{
					//BUS_MGR_Destroy(NULL);
				}

				return	MESSAGE_BREAK;
			}
			break;

		case eMEVT_BUS_DESTROY:
			BUS_KillTimer(CX_UI_UT_DURATIONTIMER_ID);
			BUS_KillTimer(CX_UI_UT_INTERVALTIMER_ID);
			xmgr_cas_CxUT_ResetUTDBOnDestroy();
			//BUS_MGR_Destroy(xproc_cas_CxCmdUt);
			// TODO: Destory 확인해야함...
			//BUS_SendMessage(NULL, MSG_APP_NOTIFY_CONAX_MSGUI_DESTROYED, (Handle_t)NULL, 0, 0, 0);
			break;

		case eMEVT_BUS_CONAX_USERTEXT_PAUSE:
			xmgr_cas_CxUserTextPause();
			return	MESSAGE_BREAK;
			break;

		case MSG_BUS_CONAX_USERTEXT_RESUME:
			xmgr_cas_CxUserTextResume();
			return	MESSAGE_BREAK;
			break;

	}

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}


#if defined(CONFIG_MW_CAS_CONAX_MAIL)
#if 0 // not use for octo2.0
BUS_Result_t		xmgr_cas_CxMailDrawBannerIcon(OSD_Screen_t *screen, OSD_Area_t *area,
										OSD_Rect_t *rect, GWM_GuiObject_t object, void *arg, int param1)
{
	OSD_Rect_t			ImgRect = {0, };
	OSD_StockImage_t	MailIcon = { 0, };
	int	nStockImgId;

	NOT_USED_PARAM(object);
	NOT_USED_PARAM(arg);
	NOT_USED_PARAM(param1);

	nStockImgId = e178_3_25_Icon_Mail;

	if(AP_StockImg_GetImage(nStockImgId, &MailIcon) == ERR_OK)
	{
		OSD_GFX_SetRect(&ImgRect, (area->x1 + rect->x), (area->y1 + rect->y), MailIcon.width, MailIcon.height);
		OSD_GFX_FB_Blend(MailIcon.hBuffer, MailIcon.width, MailIcon.height, screen, &ImgRect);
	}

	return ERR_OK;

}
#endif
BUS_Result_t	xproc_cas_CxMainIcon(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	OSD_Rect_t		rect;

	HxLOG_Print("xproc_cas_CxMainIcon() +\n");

	switch(message)
	{
		case eMEVT_BUS_CREATE :
			#if 0 // not use for octo2.0
			/* application 영역을 정한다. */
			GWM_APP_SetAppArea(CX_UI_MAILICON_APP_X, CX_UI_MAILICON_APP_Y, CX_UI_MAILICON_APP_W, CX_UI_MAILICON_APP_H);

			/* 절대 hide 되지 않는다. */
			GWM_APP_SetUIHideFlag(GWM_UI_Hide_Only_Self);

			/* icon을 그리지만 뒷 어플들을 dim시키면 안되므로 hidden UI로 뜬다. */
			GWM_APP_SetAttribute(GWM_HiddenUI_App);

			OSD_GFX_SetRect(&rect, 0, 0, CX_UI_MAILICON_APP_W, CX_UI_MAILICON_APP_H);
			GWM_Rect_Create(CX_UI_MAILICON_ID, &rect, 0x00);
			GWM_APP_SetObjectDrawMethod(CX_UI_MAILICON_ID, xmgr_cas_CxMailDrawBannerIcon);
			#endif
			break;

		case eMEVT_BUS_DESTROY :
			return MESSAGE_BREAK;

		default:
			break;
	}

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}
#endif	// End of defined(CONFIG_MW_CAS_CONAX_MAIL)

#endif


BUS_Callback_t	xmgr_cas_CxGetUiProc(XmgrCas_CxUiType_e eUiType)
{
	BUS_Callback_t	pfnProc;

	switch (eUiType)
	{
	case eCxCasUiType_CasMenu:
		pfnProc = xproc_cas_CxSmartCardMenu;
		break;

	case eCxCasUiType_SystemMessage:
		pfnProc = xproc_cas_CxMsg;
		break;

	case eCxCasUiType_CheckMessage:
		pfnProc = xproc_cas_CxCheck;
		break;

	case eCxCasUiType_MailIcon:
		pfnProc = xproc_cas_CxMainIcon;
		break;

	case eCxCasUiType_MailMessage:
		pfnProc = xproc_cas_CxCmdUt;
		break;

	case eCxCasUiType_FingerPrint:
		pfnProc = xproc_cas_CxCmdFp;
		break;

	case eCxCasUiType_PinDialog:
		pfnProc = xproc_cas_CxPinDialog;
		break;

	case eCxCasUiType_PpvDialog:
		//pfnProc = XPROC_CAS_IR_DECODER_PVR;
		pfnProc = NULL;
		break;

	case eCxCasUiType_PurchaseList:
		//pfnProc = xproc_cas_IrMailMessage;
		pfnProc = NULL;
		break;

	default:
		return NULL;
	}
	return pfnProc;
}



// End of file

