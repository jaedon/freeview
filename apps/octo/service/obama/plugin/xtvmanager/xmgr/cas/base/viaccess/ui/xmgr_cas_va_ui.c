/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

//#include <ray_popupui_coordinate.h>

#include "../local_include/_xmgr_cas_va_adapt.h"
#include "../local_include/_xmgr_cas_va_util.h"
#include "../local_include/_xmgr_cas_va.h"
#include "../local_include/_xmgr_cas_va_ui.h"

#include <svc_cas.h>
#include <mgr_live.h>
#include <mgr_svclist.h>

#include "va_util.h"
#include <xmgr_cas_res_str.h>
//#include <app_manager_cas.h>// TODO: hjlee3, just for build

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
#ifdef CONFIG_DEBUG
HUINT32	g_CasVaui_debug_level = DBG_ASSERT; // | DBG_TRACE; /* DBG_TRACE, DBG_PRINT, DBG_ASSERT 만 사용! */
#endif


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#ifdef CONFIG_DEBUG
#define ApCasVaUiPrint(level, message)		(((level) & g_CasVaui_debug_level) ? ((VK_Print("[%d][%s] ", __LINE__, __FUNCTION__)), (VK_Print message)) : 0)
#define ApCasVaUiAssert(c) 						((c) ? 0 : (VK_Print("\nxmgr_cas_NaUI assertion on %s: line%d\n\n", __FILE__, __LINE__)))
#define ApCasVaUiData(level, message)			(((level) & g_CasVaui_debug_level) ? ( (VK_Print message)) : 0)
#else
#define ApCasVaUiPrint(level, message)
#define ApCasVaUiAssert(c)
#define ApCasVaUiData(level, message)
#endif


#define VA_CAMLOCK_SCREEN_SAVER

#define BLACK_BACKGROUND_WINDOW_ID	eDxCAS_GROUPID_VA

#define FINGER_PRINT_TIMER_ID		eDxCAS_GROUPID_VA
#define FINGER_PRINT_TIMER_TIME		(5*1000)

#define FINGERPRINT_WINDOW_ID	1
#define FINGERPRINT_TEXT_ID		(FINGERPRINT_WINDOW_ID + 0x01)
#define FINGERPRINT_LENGTH		32
#define FINGERPRINT_WIDTH		210
#define FINGERPRINT_HEIGHT		55

#if defined(CONFIG_MW_CAS_VIACCESS_MULTI_OPMSG)
#define OPMSG_ICON_TIMER_ID		(eDxCAS_GROUPID_VA+1)
#define OPMSG_ICON_TIMER_TIME		(5*60*1000)	// 5분

#define	OPMSG_ICON_APP_W				91
#define	OPMSG_ICON_APP_H				92
#define	OPMSG_ICON_APP_X				1000//1119
#define	OPMSG_ICON_APP_Y				70//46

#define	OPMSG_ICON_ID 						10
#endif

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct {
	HUINT8 szFingerPrintStr[FINGERPRINT_LENGTH];
	HINT32 nDisplayed[32];
	HUINT32 ulTimerId;
} VA_FingerPrint_Context;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
// hjlee3 STATIC AP_DialogInstance_t	s_stVaMsgBoxInst;		/**< Pin Change Suceess Notice Dialog Instans. */
STATIC eCAS_VA_UI_STATE		s_eUiState;
STATIC HUINT8				s_szSystemMessage[TEXT_SIZE512];
STATIC VA_FingerPrint_Context	s_stVaFingerPrintContext;
STATIC HUINT8				s_szOpMsg[TEXT_SIZE512];

/*******************************************************************/
/********************      Local Functions      ********************/
/*******************************************************************/

#define _____LOCAL_FUNCTION______________________________________________________

STATIC HERROR local_cas_vaui_SendBannerHideMessage(DxCAS_GroupId_e eCasGroup, HUINT32 ulUiHandle)
{
	HERROR hErr;
	OxMgrCasUiEvtBanner_t	stEvent;

	HxSTD_memset(&stEvent, 0, sizeof(OxMgrCasUiEvtBanner_t));
//	stEvent.ulUiHandle = (HUINT32)ulUiHandle; // hjlee3, just for build
	/* Display할 String length를 0으로 할 경우 Banner 를 hide시킨다. */
	stEvent.stBanner.stString.ulStringLength = 0;

//	hErr = MGR_CAS_SendUiEvent (eCasGroup, 0, EVT_CASUI_BANNER, &stEvent); // hjlee3, just for build

	if(hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
	}

	return hErr;
}
STATIC HERROR local_cas_vaui_SendBannerMessage(DxCAS_GroupId_e eCasGroup, HUINT32 ulUiHandle, HUINT8 *pszMessage)
{
	HERROR hErr;
	OxMgrCasUiEvtBanner_t	stEvent;

	HxSTD_memset(&stEvent, 0, sizeof(OxMgrCasUiEvtBanner_t));
//	stEvent.ulUiHandle = (HUINT32)ulUiHandle; // hjlee2, just for build
	MWC_UTIL_DvbStrncpy(stEvent.stBanner.stString.szString, pszMessage, MGR_CAS_STR_LENGTH_LONG-1);
	stEvent.stBanner.stString.szString[MGR_CAS_STR_LENGTH_LONG-1] = '\0';
	stEvent.stBanner.stString.ulStringLength = SvcCas_UtilStrLen(stEvent.stBanner.stString.szString);
// hjlee3, just for build
//	hErr = MGR_CAS_SendUiEvent (eCasGroup, 0, EVT_CASUI_BANNER, &stEvent); // hjlee3, just for build

	if(hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
	}

	return hErr;
}
STATIC BUS_Result_t local_cas_vaui_MsgEvtCasOk(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR hErr = ERR_OK;
	if(local_cas_vaui_SendBannerHideMessage(eDxCAS_GROUPID_VA, (HUINT32)xmgr_cas_VaNotificationProc) != ERR_OK)
	{
		HxLOG_Critical("\n\n");
	}
	return hErr;
}
STATIC BUS_Result_t local_cas_cxui_MsgEvtCasFail(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR						hError = ERR_OK;
	HUINT8	szCasVa_Message[TEXT_SIZE512];

	HxLOG_Print("eMEVT_CAS_FAIL! \n");

#if 0 // not use for octo2.0
	if(xmgr_cas_CxGetErrorData(p1, szCasCx_Message) != ERR_OK)
	{
		return ERR_FAIL;
	}
#endif
	/* Session이 Start 되어 Application과 통신할 준비가 됨. */
	//if(pstContext->bSessionStarted == TRUE)
	{
		if(local_cas_vaui_SendBannerMessage(eDxCAS_GROUPID_VA, (HUINT32)xmgr_cas_VaNotificationProc, szCasVa_Message) != ERR_OK)
		{
			HxLOG_Critical("\n\n");
		}
	}

	return ERR_BUS_MESSAGE_PASS;
}
STATIC BUS_Result_t local_cas_vaui_MsgEvtCasCheck(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t eBusErr = ERR_BUS_NO_ERROR;

	return eBusErr;
}


STATIC DxCasType_e local_ap_cas_va_GetSvcCasType(Handle_t hAction)
{
	HERROR		hError = ERR_OK;
	DbSvc_Info_t 	svcInfo;
	HUINT32		ulActionId = SVC_PIPE_GetActionId(hAction);

	DbSvc_SetCasType(&svcInfo, eDxCAS_TYPE_All);

	hError = MGR_LIVE_GetSvcInfo (ulActionId, eLiveViewPlaySvc_MASTER, &svcInfo);
	if (hError != ERR_OK)
	{
		ApCasVaUiPrint(DBG_TRACE, ("OxMGR_WTV_GetSvcInfo() Error(%d) \n", hError));
	}

	return DbSvc_GetCasType(&svcInfo);
}


STATIC BUS_Result_t local_ap_cas_va_ShowMessageBoxProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{

	switch(message)
	{
		#if 0 // not use for octo2.0
		case MSG_GWM_KEYDOWN:
			AP_KeyConv_UiCommon(&p1);

			switch (p1)
			{
				case KEY_EXIT:
				case KEY_OK:
				case KEY_MENU:
					BUS_MGR_Destroy(0);
					BUS_MGR_Destroy(xmgr_cas_VaOpMessage_Proc);

/*
					if(p1 == KEY_OK)
					{
						switch(s_eUiState)
						{
							case eCAS_VA_UI_NewOpMessage:
								BUS_MGR_Create("xmgr_cas_VaStatusListProc", APP_DIALOGBOX_PRIORITY, (GWM_Callback_t)xmgr_cas_VaStatusListProc, 0, 0, 0, eCasVAMenuItem_Operator_Msg);
								BUS_MGR_Destroy(xmgr_cas_VaOpMessage_Proc);
								break;
						}
					}
*/
					return MESSAGE_BREAK;
				default:
					break;
			}
		break;
		#endif


	}
	//return AP_Dialog_ProtoProc(&s_stVaMsgBoxInst, message, hAction, p1, p2, p3);

}


STATIC HERROR local_ap_cas_va_ShowMessageBox(HUINT8 *pszMsg)
{

	BUS_Result_t hResult = ERR_BUS_NO_ERROR;

#if 0 // not use for octo2.0
	AP_Dialog_SetDialog(&s_stVaMsgBoxInst,
						BUS_MGR_GetAppCallback(BUS_MGR_GetThis()),
						DLG_OK | DLG_TIMEOUT_NOTIMEOUT /*| DLG_TYPE_TITLE */| DLG_TYPE_MESSAGE,
						/*AP_CasResStr_GetStrRsc(LOC_CAS_VA_TITLE_VIACCESS_ID)*/NULL, pszMsg);
#endif

	hResult = BUS_MGR_Create("local_ap_cas_va_ShowMessageBoxProc", APP_DIALOGBOX_PRIORITY, (BUS_Callback_t)local_ap_cas_va_ShowMessageBoxProc, 0, 0, 0, 0);

	return (hResult == ERR_BUS_NO_ERROR ? ERR_OK : ERR_FAIL);

}


STATIC HERROR local_ap_cas_va_CheckMessageProcess(HINT32 p1, HINT32 p2, HINT32 p3)
{
	VA_UI_EVENT *pEvent = (VA_UI_EVENT *)p2;
	VA_UI_OP_MESSAGE *pOpMsg;
	HUINT8				szConvertOpMsg[VA_UI_OP_MSG_SIZE+1] = {0,};

	if(pEvent == NULL)
	{
		return ERR_FAIL;
	}

	switch(pEvent->eEventType)
	{
		case VA_UI_DESCRAMBLING_KO_MATURITY_RATING:
			s_eUiState = eCAS_VA_UI_AgreeMaturityRate;
			break;

		case VA_UI_NEW_OPERATOR_MESSAGE:
			s_eUiState = eCAS_VA_UI_NewOpMessage;

			pOpMsg = VA_UI_Adapt_GetLastOpMessage();
#if 0
			sprintf(s_szOpMsg, "%s\n%s", AP_CasResStr_GetStrRsc(LOC_CAS_VA_MSG_OPMESSAGE_ID), pOpMsg->szMsg);
#else
			if(pOpMsg != NULL)
				AP_CAS_VA_GetConvertOpMessage(pOpMsg, szConvertOpMsg, NULL);

			// 모든 String 관리는 Web에서 처리하는 것으로 약속됨에 따라 AP_CasResStr_GetStrRsc() 함수를 더이상 사용하지 말아야 함.
			// 따라서 LOC ID를 Web으로 전송 후 Web에서 그에 맞는 string을 찾아 출력하도록 함.
			todo
			MWC_UTIL_DvbUtf8Sprintf(s_szOpMsg, "%s\n%s", AP_CasResStr_GetStrRsc(STR_OPERATOR_MESSAGE_ID), szConvertOpMsg);
#endif
			local_ap_cas_va_ShowMessageBox(s_szOpMsg);
//			local_ap_cas_va_ShowMessageBox(AP_CasResStr_GetStrRsc(LOC_CAS_VA_MSG_OPMESSAGE_ID));
			break;

		default:
			s_eUiState = eCAS_VA_UI_None;
			break;
	}

	return ERR_OK;
}

#if 0 // not use for octo2.0
STATIC BUS_Result_t		local_ap_cas_va_FillMsgBackground(OSD_Screen_t *screen, OSD_Area_t *area, OSD_Rect_t *rect, GWM_GuiObject_t object, void *arg, HINT32 param1)
{
	HINT32			x1, y1, x2, y2;
	OSD_Color_t		ulColor, ulAlpha;
	OSD_Area_t		stArea, stOsdRect;

	/* warning remove */
	NOT_USED_PARAM(object);
	NOT_USED_PARAM(arg);
	NOT_USED_PARAM(param1);

	/* absolute coordinate */
	x1 = OSD_SCREEN_OFFSET_X;
	y1 = OSD_SCREEN_OFFSET_Y;
	x2 = x1 + OSD_SCREEN_WIDTH;
	y2 = y1 + OSD_SCREEN_HEIGHT;

	ulAlpha = 0xff;
	AP_UTILOSD_GetBGAlpha(0, (HINT32 *)&ulAlpha);
	ulColor = (ulAlpha << 24) | (COL(C_W_BG01) & 0x00ffffff);

	OSD_GFX_DrawRectBlendFill(screen, x1, y1, x2, y2, ulColor);

	return ERR_BUS_NO_ERROR;
}


STATIC HERROR local_ap_cas_va_DrawMsgBackground()
{
	OSD_Rect_t	stRect = {0, };

	GWM_APP_SetAppArea(OSD_SCREEN_OFFSET_X, OSD_SCREEN_OFFSET_Y, OSD_SCREEN_WIDTH, OSD_SCREEN_HEIGHT);

	// set app area
	OSD_GFX_SetRect (&stRect, OSD_SCREEN_OFFSET_X, OSD_SCREEN_OFFSET_Y, OSD_SCREEN_WIDTH, OSD_SCREEN_HEIGHT);
	GWM_Frame_Create(BLACK_BACKGROUND_WINDOW_ID, &stRect, NULL);
	GWM_APP_SetObjectDrawMethod(BLACK_BACKGROUND_WINDOW_ID, local_ap_cas_va_FillMsgBackground);
}


STATIC BUS_Result_t		local_ap_cas_va_fingerprint_DrawBackground(OSD_Screen_t *screen, OSD_Area_t *area, OSD_Rect_t *rect, GWM_GuiObject_t object, void *arg, HINT32 param1)
{
	HINT32			x1, y1, x2, y2;
	OSD_Color_t		ulColor, ulAlpha;
	OSD_Area_t		stArea, stOsdRect;

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
	AP_UTILOSD_GetBGAlpha(0, (HINT32 *)&ulAlpha);
	ulColor = (ulAlpha << 24) | (COL(C_W_BG01) & 0x00ffffff);

	OSD_GFX_DrawRectBlendFill(screen, x1, y1, x2, y2, ulColor);

	return ERR_BUS_NO_ERROR;
}
#endif

STATIC HERROR local_ap_cas_va_FingerPrintProcess()
{
	#if 0 // not use for octo2.0
#if 0
	// 720 x 576 (PAL), 720 x 480 (NTSC)
	HINT32 nX[32] = { 20, 145, 280, 415,  										/* 4 */
				20, 145, 280, 415,  										/* 4 */
				20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,					/* 12 */
				545, 545, 545, 545, 545, 545, 545, 545, 545, 545, 545, 545 };		/* 12 */
	HINT32 nY[32] = { 10, 10, 10, 10, 											/* 4 */
				520, 520, 520, 520, 										/* 4 */
				40, 80, 120, 160, 200, 240, 280, 320, 360, 400, 440, 480,			/* 12 */
				40, 80, 120, 160, 200, 240, 280, 320, 360, 400, 440, 480};		/* 12 */
#else
	// 1280 x 720
	HINT32 nX[32] = { 40, 290, 560, 830,												/* 4 */
		40, 290, 560, 830,  															/* 4 */
		40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,									/* 12 */
		1028, 1028, 1028, 1028, 1028, 1028, 1028, 1028, 1028, 1028, 1028, 1028};		/* 12 */
	HINT32 nY[32] = { 30, 30, 30, 30, 													/* 4 */
		625, 625, 625, 625, 															/* 4 */
		50, 100, 150, 200, 250, 300, 350, 400, 450, 500, 550, 600,						/* 12 */
		50, 100, 150, 200, 250, 300, 350, 400, 450, 500, 550, 600};						/* 12 */
#endif

	HINT32		bSelected, i;
	HUINT32		ulIdx;
	OSD_Area_t	stRect;

	// generate random number (쉽게 ^^)
	ulIdx = VK_TIMER_GetSystemTick();
	ulIdx = ulIdx % 32;

	// find a position which is not displayed yet
	bSelected = FALSE;
	i = ulIdx;
	do
	{
		i = (i + 7) % 32;
		if (s_stVaFingerPrintContext.nDisplayed[i] == 0)
		{
			bSelected = TRUE;
			s_stVaFingerPrintContext.nDisplayed[i] = TRUE;
			break;
		}
	} while (i != ulIdx);

	if (bSelected)
	{
		ulIdx = i;
	}
	else
	{
		// fingerprint is displayed at all predifined positions.
		VK_memset32(s_stVaFingerPrintContext.nDisplayed, 0, 32);
		s_stVaFingerPrintContext.nDisplayed[ulIdx] = TRUE;
	}

	// set app area
	GWM_APP_SetAppArea(0, 0, OSD_SCREEN_WIDTH, OSD_SCREEN_HEIGHT);
	OSD_GFX_SetRect (&stRect, nX[ulIdx], nY[ulIdx], FINGERPRINT_WIDTH, FINGERPRINT_HEIGHT);

	GWM_Frame_Create(FINGERPRINT_WINDOW_ID, &stRect, NULL);
	GWM_APP_SetObjectDrawMethod(FINGERPRINT_WINDOW_ID, local_ap_cas_va_fingerprint_DrawBackground);

	GWM_Text_Create (FINGERPRINT_TEXT_ID, &stRect, s_stVaFingerPrintContext.szFingerPrintStr);
	GWM_Text_SetAlign (FINGERPRINT_TEXT_ID, TEXT_ALIGN_CENTER | TEXT_ALIGN_MIDDLE);
	GWM_Text_SetSize (FINGERPRINT_TEXT_ID, FONT_SIZE_L);
	GWM_Text_ChangeColor (FINGERPRINT_TEXT_ID, COL (C_T_Enable), NORMAL_TEXTBACK_COLOR);
#endif
	return ERR_OK;
}


STATIC BUS_Result_t local_ap_cas_va_fingerprint_proc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
#if 0 // not use for octo2.0
	switch(message)
	{
		case eMEVT_BUS_CREATE:
			local_ap_cas_va_FingerPrintProcess();
			break;
	}
#endif
}


STATIC HERROR local_ap_cas_va_SetScreenSaver(HINT32 message)
{
	HERROR hError = ERR_OK;
#if 0 // not use for octo2.0
/*
#undef SCREEN_SAVER_TIMEOUT
#define SCREEN_SAVER_TIMEOUT	15*1000
*/
	switch(message)
	{
	case eMEVT_BUS_CREATE :
		if(BUS_MGR_GetThis() == BUS_MGR_GetUITop())
		{
			GWM_SendMessage(NULL, MSG_SCREEN_SAVER_TIMER_STARTED, 0, (HINT32)BUS_MGR_GetAppCallback(BUS_MGR_GetThis()), 0, 0);

			if(BUS_SetTimer(SCREEN_SAVER_TIMER_ID, SCREEN_SAVER_TIMEOUT) == ERR_LINK_CONFLICT)
				GWM_ResetTimer(SCREEN_SAVER_TIMER_ID);
		}
		break;

	case MSG_GWM_STACK_CHANGED:
		if(BUS_MGR_GetThis() == BUS_MGR_GetUITop())
		{
			GWM_SendMessage(NULL, MSG_SCREEN_SAVER_TIMER_STARTED, 0, (HINT32)BUS_MGR_GetAppCallback(BUS_MGR_GetThis()), 0, 0);

			if(BUS_SetTimer(SCREEN_SAVER_TIMER_ID, SCREEN_SAVER_TIMEOUT) == ERR_LINK_CONFLICT)
				GWM_ResetTimer(SCREEN_SAVER_TIMER_ID);
		}
		break;

	case eMEVT_BUS_TIMER:
		GWM_SendMessage(NULL, MSG_SCREEN_SAVER_START, 0, (HINT32)BUS_MGR_GetAppCallback(BUS_MGR_GetThis()), FALSE, 0);
		BUS_KillTimer(SCREEN_SAVER_TIMER_ID);
		break;

	case MSG_GWM_DESTROY :
		GWM_SendMessage(NULL, MSG_SCREEN_SAVER_STOP, 0, (HINT32)BUS_MGR_GetAppCallback(BUS_MGR_GetThis()), FALSE, 0);
		BUS_KillTimer(SCREEN_SAVER_TIMER_ID);
		break;

	default:
		hError = ERR_FAIL;
		break;
	}
#endif
	return hError;
}

#if defined(CONFIG_MW_CAS_VIACCESS_MULTI_OPMSG)
BUS_Result_t		local_ap_cas_va_OpMsg_DrawBannerIcon(OSD_Screen_t *screen, OSD_Area_t *area,
										OSD_Rect_t *rect, GWM_GuiObject_t object, void *arg, int param1)
{
	OSD_Rect_t			ImgRect = {0, };
	OSD_StockImage_t	MailIcon = { 0, };

	NOT_USED_PARAM(object);
	NOT_USED_PARAM(arg);
	NOT_USED_PARAM(param1);

	if(AP_StockImg_GetImage(e470_1_46_Mail_Messages_01, &MailIcon) == ERR_OK)
	{
		OSD_GFX_SetRect(&ImgRect, (area->x1 + rect->x), (area->y1 + rect->y), MailIcon.width, MailIcon.height);
		OSD_GFX_FB_Blend(MailIcon.hBuffer, MailIcon.width, MailIcon.height, screen, &ImgRect);
	}

	return ERR_OK;
}
#endif

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

#define _____PUBLIC_FUNCTION______________________________________________________

#if defined(CONFIG_MW_CAS_VIACCESS_MULTI_OPMSG)
BUS_Result_t	xmgr_cas_VaOpMessageIconProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	switch(message)
	{
		case eMEVT_BUS_CREATE :
		{
			OSD_Rect_t		rect;

			/* application 영역을 정한다. */
			GWM_APP_SetAppArea(OPMSG_ICON_APP_X, OPMSG_ICON_APP_Y, OPMSG_ICON_APP_W, OPMSG_ICON_APP_H);

			/* 절대 hide 되지 않는다. */
			GWM_APP_SetUIHideFlag(GWM_UI_Hide_Never);

			/* icon을 그리지만 뒷 어플들을 dim시키면 안되므로 hidden UI로 뜬다. */
			GWM_APP_SetAttribute(GWM_HiddenUI_App);

			OSD_GFX_SetRect(&rect, 0, 0, OPMSG_ICON_APP_W, OPMSG_ICON_APP_H);
			GWM_Rect_Create(OPMSG_ICON_ID, &rect, 0x00);
			GWM_APP_SetObjectDrawMethod(OPMSG_ICON_ID, local_ap_cas_va_OpMsg_DrawBannerIcon);

			// 5분 timeout을 두는데, irdeto는 timeout이 없는데??
			BUS_KillTimer(OPMSG_ICON_TIMER_ID);
			BUS_SetTimer(OPMSG_ICON_TIMER_ID, OPMSG_ICON_TIMER_TIME);
			break;
		}

		case eMEVT_BUS_TIMER:
			if(p1 == OPMSG_ICON_TIMER_ID)
			{
				BUS_KillTimer(OPMSG_ICON_TIMER_ID);
				BUS_MGR_Destroy(NULL);
			}
			break;

		case MSG_GWM_KEYDOWN:
			AP_KeyConv_UiCommon(&p1);

			switch (p1)
			{
			case KEY_ADD_BOOKMARK:
				// bookmark 키를 누르면 실제 메시지를 표시해준다.
				BUS_MGR_Create("xmgr_cas_VaOpMsgTextProc", APP_CAS_PRIORITY,
										xmgr_cas_VaOpMsgTextProc, hAction, p1, p2, p3);

				BUS_MGR_Destroy(0);

			}
			break;

		case MSG_GWM_DESTROY :
			BUS_KillTimer(OPMSG_ICON_TIMER_ID);
			return MESSAGE_BREAK;

		default:
			break;
	}

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}
#endif

BUS_Result_t xmgr_cas_VaFingerPrintProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
#if 0 // not use for octo2.0
	switch(message)
	{
		case eMEVT_BUS_CREATE:
			BUS_MGR_Create("local_ap_cas_va_fingerprint_proc", APP_CAS_HIGH_PRIORITY, local_ap_cas_va_fingerprint_proc, hAction, 0, 0, 0);
			BUS_SetTimer(FINGER_PRINT_TIMER_ID, FINGER_PRINT_TIMER_TIME);
			break;

		case eMEVT_BUS_TIMER:
			if(p1 == FINGER_PRINT_TIMER_ID)
			{
				BUS_MGR_Destroy(local_ap_cas_va_fingerprint_proc);
				BUS_MGR_Create("local_ap_cas_va_fingerprint_proc", APP_CAS_HIGH_PRIORITY, local_ap_cas_va_fingerprint_proc, hAction, 0, 0, 0);
			}
			break;

		case MSG_GWM_DESTROY:
			BUS_MGR_Destroy(local_ap_cas_va_fingerprint_proc);
			BUS_KillTimer(FINGER_PRINT_TIMER_ID);
			break;
	}
#endif
}


BUS_Result_t xmgr_cas_VaOpMessageProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	switch(message)
	{
	case eMEVT_BUS_CREATE :
		local_ap_cas_va_CheckMessageProcess(p1, p2, p3);
		break;

	default:
		break;
	}
}

BUS_Result_t xmgr_cas_VaCamlockScreen_Proc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
#if 0 // not use for octo2.0
	switch(message)
	{
		case eMEVT_BUS_CREATE :
			if(AP_CAS_VA_Adapt_GetErrorMessage(s_szSystemMessage) != ERR_OK)
			{
				BUS_MGR_Destroy(0);
				return MESSAGE_BREAK;
			}
			local_ap_cas_va_DrawMsgBackground();

#ifdef VA_CAMLOCK_SCREEN_SAVER
			local_ap_cas_va_SetScreenSaver(message);
#endif

			// 만약 menu가 떠있다면 닫는다.
			if(BUS_MGR_Get(xmgr_cas_VaSmartCardMenuProc) != NULL)
				BUS_MGR_Destroy(xmgr_cas_VaSmartCardMenuProc);

			if(BUS_MGR_Get(xmgr_cas_VaStatusListProc) != NULL)
				BUS_MGR_Destroy(xmgr_cas_VaStatusListProc);

			if(BUS_MGR_Get(xmgr_cas_VaMaturityRatingProc) != NULL)
				BUS_MGR_Destroy(xmgr_cas_VaMaturityRatingProc);

			// camlock level low이면 appl 생성시 현재 ch을 검사해서 fta면 appl hide 시킨다
			// no db상태에서도 ui hide 시킨다
			if ((SVC_PIPE_GetActionId(hAction) == eActionId_VIEW_0)
				&& (xmgr_cas_VaGetCamlockLevel() == VA_CAMLOCK_CONTROL_LOW))
			{
				if ( (local_ap_cas_va_GetSvcCasType(hAction) == eDxCAS_TYPE_FTA)
					|| (MGR_SVCLIST_GetSvcTypeForListCount(MGR_SVCLIST_GetSvcTypeForListCount()) == 0))
				{
					GWM_APP_HideUI();
				}
			}
			break;

		case MSG_GWM_STACK_CHANGED:
#ifdef VA_CAMLOCK_SCREEN_SAVER
			local_ap_cas_va_SetScreenSaver(message);
#endif
			break;

		case eMEVT_BUS_TIMER:
#ifdef VA_CAMLOCK_SCREEN_SAVER
			if (p1 == SCREEN_SAVER_TIMER_ID)
			{
				if ((SVC_PIPE_GetActionId(hAction) == eActionId_VIEW_0)
					&& (xmgr_cas_VaGetCamlockLevel() == VA_CAMLOCK_CONTROL_LOW))
				{
					if (local_ap_cas_va_GetSvcCasType(hAction) != eDxCAS_TYPE_FTA)
					{
						local_ap_cas_va_SetScreenSaver(message);
					}
				}
				else
				{
					// org
					local_ap_cas_va_SetScreenSaver(message);
				}

				return MESSAGE_BREAK;
			}
#endif
			break;

		case eSEVT_CAS_OK :
		case eSEVT_CAS_FAIL:
			if ((SVC_PIPE_GetActionId(hAction) == eActionId_VIEW_0)
				&& (xmgr_cas_VaGetCamlockLevel() == VA_CAMLOCK_CONTROL_LOW))
			{
				// camlock level low에서는 fta에서 banner가 없어야 한다
				// zapping시에 hide시키고, zapping 완료후 cas 관련 msg 올라오면 cas service인지 확인해서 다시 보여준다
				if (local_ap_cas_va_GetSvcCasType(hAction) == eDxCAS_TYPE_FTA)
				{
					// FTA - zapping시에 이미 appl hide되었으므로 unmute만 시켜준다
					VA_OutputMute(FALSE, VA_MUTE_CAMLOCK);
				}
				else
				{
					// scrambled ch
					// camlock low,active 상태로 factory default를 하면 부팅시 install wizard와 함께 camlock proc이 생성된다
					// 이 경우에 language를 변경한 상태가 반영되지 않아 default language로 message가 나오게 되는데,
					// 이를 방지하고자 show하는 경우에는 msg를 다시 읽어오도록 한다
					AP_CAS_VA_Adapt_GetErrorMessage(s_szSystemMessage);
					GWM_APP_ShowUI();
				}
			}
			break;

		case MSG_APP_LANGUAGE_CHANGED:
			if(AP_CAS_VA_Adapt_GetErrorMessage(s_szSystemMessage) != ERR_OK)
			{
				BUS_MGR_Destroy(0);
				return MESSAGE_BREAK;
			}
			break;

		// 외부에서 아래 메시지 수신시 스스로 종료시켜 버림... 처리하지 않고 넘김
		case MSG_GWM_QUIT_UI:
			return MESSAGE_PASS;

		case MSG_GWM_DESTROY :
#ifdef VA_CAMLOCK_SCREEN_SAVER
			local_ap_cas_va_SetScreenSaver(message);
#endif
			break;

		default:
			break;
	}

	ApCasVaUiPrint(DBG_TRACE, ("[%s]\n", s_szSystemMessage));

//	return AP_StatusMessage_ProtoProc(message, hAction, (HINT32)s_szSystemMessage, p2, p3);
#endif
}


BUS_Result_t xmgr_cas_VaCheckMessageProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	switch(message)
	{
		case eMEVT_BUS_CREATE:
			local_ap_cas_va_CheckMessageProcess(p1, p2, p3);
			break;

		default:
			break;
	}

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}


BUS_Result_t xmgr_cas_VaNotificationProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	switch(message)
	{
	case eMEVT_BUS_CREATE :
		if(xmgr_cas_VaAdaptGetErrorMessage(s_szSystemMessage) != ERR_OK)
		{
			BUS_MGR_Destroy(0);
			return MESSAGE_BREAK;
		}

		local_ap_cas_va_SetScreenSaver(message);

        /* smartcard menu 내에 있을 때 smartcard 가 제거되면 smartcard menu 를 닫도록 한다 :

                UI_SMARTCARD_EXTRACTED_EVENT 가 eSEVT_CAS_CMD 로 올려지는 경우엔 (즉, camlock activate 상태) AP_CAS_VA_CamlockScreen_Proc() 에서 이게 처리가 되지만
                UI_SMARTCARD_EXTRACTED_EVENT 가 eSEVT_CAS_FAIL 로 올려지는 경우엔 (즉, camlock deactivate 상태) 여기에서만 처리가 가능하다.

                단, 이 수정도 FTA 채널인 경우엔 처리가 되지 않는다는 한계가 있다... 메시지 처리 부분은 전반적으로 뜯어 고치고 싶다! */

        CAS_VA_SC_State_t       stScState;
        AP_CAS_VA_GetSmartcardState(&stScState);

        if(stScState.nState == VA_UI_SMARTCARD_EXTRACTED)
        {
            // 만약 menu가 떠있다면 닫는다.
            if(BUS_MGR_Get(xmgr_cas_VaSmartCardMenuProc) != NULL)
                    BUS_MGR_Destroy(xmgr_cas_VaSmartCardMenuProc);

            if(BUS_MGR_Get(xmgr_cas_VaStatusListProc) != NULL)
                    BUS_MGR_Destroy(xmgr_cas_VaStatusListProc);

            if(BUS_MGR_Get(xmgr_cas_VaMaturityRatingProc) != NULL)
                    BUS_MGR_Destroy(xmgr_cas_VaMaturityRatingProc);
        }

		break;
#if 0 // not use for octo2.0

	case MSG_GWM_STACK_CHANGED:
		local_ap_cas_va_SetScreenSaver(message);
		break;

	case eMEVT_BUS_TIMER:
		if(p1 == SCREEN_SAVER_TIMER_ID)
		{
			local_ap_cas_va_SetScreenSaver(message);
			return MESSAGE_BREAK;
		}
		break;

	case MSG_APP_LANGUAGE_CHANGED :
		if(AP_CAS_VA_Adapt_GetErrorMessage(s_szSystemMessage) != ERR_OK)
		{
			BUS_MGR_Destroy(0);
			return MESSAGE_BREAK;
		}
		break;

	// 외부에서 아래 메시지 수신시 스스로 종료시켜 버림... 처리하지 않고 넘김
	case MSG_GWM_QUIT_UI:
		return MESSAGE_PASS;

#endif
	case eMEVT_BUS_DESTROY :
		local_ap_cas_va_SetScreenSaver(message);
		break;

	default:
		break;
	}

	ApCasVaUiPrint(DBG_TRACE, ("[%s]\n", s_szSystemMessage));

//	return AP_StatusMessage_ProtoProc(message, hAction, (HINT32)s_szSystemMessage, p2, p3);

}


#ifdef CONFIG_MW_CAS_VIACCESS_FINGERPRINT
HERROR xmgr_cas_VaShowFingerPrint(HUINT32 hAction, HINT32 p2)
{
	VA_UI_EVENT *pEvent = (VA_UI_EVENT *)p2;

	if (SvcCas_UtilStrLen((HUINT8 *)pEvent->ulScSlot) > FINGERPRINT_LENGTH)
	{
		HxSTD_memcpy(s_stVaFingerPrintContext.szFingerPrintStr, ((HUINT8 *)pEvent->ulScSlot), FINGERPRINT_LENGTH);
		s_stVaFingerPrintContext.szFingerPrintStr[FINGERPRINT_LENGTH - 1] = '\0';
	}
	else
	{
		MWC_UTIL_DvbStrcpy(s_stVaFingerPrintContext.szFingerPrintStr, ((const HUINT8 *)pEvent->ulScSlot));
	}

	BUS_MGR_Create("AP_CAS_VA_FingerPrint_Proc", APP_CAS_HIGH_PRIORITY, xmgr_cas_VaFingerPrintProc, hAction, 0, 0, 0);
	return ERR_OK;
}


HERROR xmgr_cas_VaHideFingerPrint()
{
	BUS_MGR_Destroy(xmgr_cas_VaFingerPrintProc);

	return ERR_OK;
}
#endif


HERROR xmgr_cas_VaHideMessageBox()
{
	BUS_MGR_Destroy(local_ap_cas_va_ShowMessageBoxProc);
	BUS_MGR_Destroy(xmgr_cas_VaOpMessageProc);

	return ERR_OK;
}


HERROR xmgr_cas_VaShowMessageBox(HUINT8 *pszMsg)
{
	s_eUiState = eCAS_VA_UI_None;

	return local_ap_cas_va_ShowMessageBox(pszMsg);
}


HERROR xmgr_cas_VaShowMessageBoxByLoc(HINT32 nLocId)
{
	VK_memset32(s_szSystemMessage, 0, TEXT_SIZE512);

	// 모든 String 관리는 Web에서 처리하는 것으로 약속됨에 따라 AP_CasResStr_GetStrRsc() 함수를 더이상 사용하지 말아야 함.
	// 따라서 LOC ID를 Web으로 전송 후 Web에서 그에 맞는 string을 찾아 출력하도록 함.
	todo
	MWC_UTIL_DvbStrcpy(s_szSystemMessage, AP_CasResStr_GetStrRsc(nLocId));

	ApCasVaUiPrint(DBG_TRACE, ("[%s]\n", s_szSystemMessage));

	return xmgr_cas_VaShowMessageBox(s_szSystemMessage);
}


HERROR xmgr_cas_VaSetScreenSaver(HINT32 message)
{
	return local_ap_cas_va_SetScreenSaver(message);
}

BUS_Callback_t	xmgr_cas_VaCbGetUiProc(VA_CAS_UI_FN_e eUiType)
{
	BUS_Callback_t	pfnProc;

	switch (eUiType)
	{
	case eVaCasUi_FnCasMenu:
		pfnProc = xmgr_cas_VaSmartCardMenuProc;
		break;

	case eVaCasUi_FnSystemMessage:
		pfnProc = xmgr_cas_VaNotificationProc;
		break;

	case eVaCasUi_FnCheckMessage:
		pfnProc = xmgr_cas_VaCheckMessageProc;
		break;
#if defined(CONFIG_MW_CAS_VIACCESS_MULTI_OPMSG)
	case eVaCasUi_FnMailIcon:
		pfnProc = xmgr_cas_VaOpMessageIconProc;
		break;
#else
	case eVaCasUi_FnMailMessage:
		pfnProc = xmgr_cas_VaOpMessageProc;
		break;
#endif
	case eVaCasUi_FnFingerPrint:
		pfnProc = NULL;
		break;

	case eVaCasUi_FnPinDialog:
		pfnProc = NULL;
		break;

	case eVaCasUi_FnPpvDialog:
		pfnProc = NULL;
		break;

	case eVaCasUi_FnPurchaseList:
		pfnProc = NULL;
		break;

	default:
		return NULL;
	}
	return pfnProc;
}

