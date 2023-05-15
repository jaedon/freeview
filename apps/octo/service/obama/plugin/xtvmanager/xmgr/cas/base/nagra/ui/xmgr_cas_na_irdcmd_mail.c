/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <mgr_live.h>
#include <mgr_cas.h>

#include <xmgr_mgr.h>
#include <xmgr_cas_res_str.h>

#include <namgr_main.h>

#include "../local_include/_xmgr_cas_na_util.h"
#include "../local_include/_xmgr_cas_na_adapt.h"
#include "../local_include/_xmgr_cas_na.h"


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

typedef struct
{
	Handle_t	hSession;
	HUINT16		usMailId;
	HUINT16		usDeletedMailId;
}sNaUi_MailMessage_Context_t;

#define XMGR_CAS_NA_MAIL_PARAM_WARRING_REMOVE(arg1, arg2, arg3, arg4)				\
			HxLOG_Info("\n");			\
			{															\
			(void)arg1;													\
			(void)arg2;													\
			(void)arg3;													\
			(void)arg4;					 								\
			}
#define XMGR_CAS_NA_MAIL_POINTER_INVALID_CHECK(pArg, returnValue)					\
			HxLOG_Info("\n");					\
			if(pArg == NULL)													\
			{																	\
			HxLOG_Critical("\n\n");												\
			return returnValue;													\
			}
#define XMGR_CAS_NA_MAIL_HERROR_INVALID_CHECK(pArg, returnValue)					\
			HxLOG_Info("\n");			\
			if(pArg != ERR_BUS_NO_ERROR)									\
			{															\
			HxLOG_Critical("\n\n");												\
			return returnValue;											\
			}

#define	MAILICON_NA_IRD_APP_W				84
#define	MAILICON_NA_IRD_APP_H				78
#define	MAILICON_NA_IRD_APP_X				(1000-100)
#define	MAILICON_NA_IRD_APP_Y				70

#define	MAILICON_NA_IRD_ID					1

/*******************************************************************/
/********************      Local Variables     *********************/
/*******************************************************************/
static sNaUi_MailMessage_Context_t	s_stMailMessageContext;


/*******************************************************************/
/********************      Local Functions      ********************/
/*******************************************************************/
#define _____XMGR_CAS_NA_UI_LOCAL_FUNCTION______________________________________________________
#if 0
BUS_Result_t		xmgr_cas_Nanormal_mail_DrawBannerIcon(OSD_Screen_t *screen, OSD_Area_t *area,
										OSD_Rect_t *rect, BUS_GuiObject_t object, void *arg, int param1)
{
	OSD_Rect_t		ImgRect = {0, };
	OSD_StockImage_t	MailIcon = { 0, };
	HINT32			nImg = eImg_NULL;

	NOT_USED_PARAM(object);
	NOT_USED_PARAM(arg);
	NOT_USED_PARAM(param1);

#if defined(CONFIG_OP_RCS)
	nImg = e178_6_31_Icon_Mail;
#endif

	if(AP_StockImg_GetImage(nImg, &MailIcon) == ERR_OK)
	{
		OSD_GFX_SetRect(&ImgRect, (area->x1 + rect->x), (area->y1 + rect->y), MailIcon.width, MailIcon.height);
		OSD_GFX_FB_Blend(MailIcon.hBuffer, MailIcon.width, MailIcon.height, screen, &ImgRect);
	}

	return ERR_OK;

}
#endif

static sNaUi_MailMessage_Context_t* _xmgr_cas_NaUi_MailMessage_GetContext(void)
{
	return &s_stMailMessageContext;
}

static HERROR _xmgr_cas_NaUi_MailMessage_SendMailMessage(sNaUi_MailMessage_Context_t *pstContext)
{
	OxMgrCasUiEvtMail_t	*pstMailEvent = NULL;
	OxMgrCasUiMail_t	*pstMailMessage;
	HxDATETIME_t		dateTime;
	HUINT8				*pszUtf8Text = NULL, *pszEncodedStr = NULL, *pszProperties = NULL;
	HINT32				slStream = 0;
	HUINT32				ulStreamSize;

	NA_MAIL_MSG			*pstMail;
	SvcCas_CtrlParam_t	stParam;
	NA_IRDCMD_MAIL_GetMailDataInParam_t		stInParam;
	NA_IRDCMD_MAIL_GetMailDataOutParam_t	stOutParam;

	stInParam.nMailId = pstContext->usMailId;
	stOutParam.ppMailData = &pstMail;

	stParam.ulControlType = eNaCtrl_IRDCMD_MAIL_GetMailData;
	stParam.pvIn = (void *)&stInParam;
	stParam.pvOut = (void *)&stOutParam;

	if(SVC_CAS_CtrlGet(eDxCAS_GROUPID_NA, stParam.ulControlType, &stParam) != ERR_OK)
		return ERR_FAIL;

	pstMailEvent = (OxMgrCasUiEvtMail_t *)OxCAS_Calloc(sizeof(OxMgrCasUiEvtMail_t));
	if(pstMailEvent == NULL)
		goto _ERROR;

	pstMailEvent->stSessionInfo.eSessionType = eDxCAS_SESSIONTYPE_EXTENSION;
	pstMailEvent->stSessionInfo.hSession = pstContext->hSession;
	pstMailEvent->stSessionInfo.ulUiHandle = (HUINT32)xproc_cas_NaUiMailMessage;

	pstMailMessage = &pstMailEvent->stMail;

	pstMailMessage->stHeader.ulAttr = eMgrCasUiAttr_Title | eMgrCasUiAttr_Bottom;
	HxSTD_StrNCpy(pstMailMessage->stHeader.szTitle, "[{\"id\":\"LOC_MAIL_MESSAGES_ID\"}]", MGR_CAS_STR_LENGTH_LONG - 1);
	HxSTD_StrNCpy(pstMailMessage->stHeader.szBottom, "[{\"id\":\"LOC_EXIT_TO_QUIT_ID\"}]", MGR_CAS_STR_LENGTH_LONG - 1);

	HLIB_DATETIME_ConvertUnixTimeToDateTime(pstMail->nSystime, &dateTime);

	HxSTD_snprintf(pstMailMessage->stHeader.strReceivedDate, MGR_CAS_MAIL_RECEIVED_DATE_LENGTH, "[\"%02d/%02d\"]", dateTime.stDate.ucMonth, dateTime.stDate.ucDay);
	HxSTD_snprintf(pstMailMessage->stHeader.strReceivedTime, MGR_CAS_MAIL_RECEIVED_TIME_LENGTH, "[\"%02d:%02d\"]", dateTime.stTime.ucHour, dateTime.stTime.ucMinute);

	xmgr_cas_NaUtil_ConvertLanguageCode(pstMail->szText);

	MWC_UTIL_ConvSiStringToUtf8(NULL, MWC_UTIL_DvbStrlen(pstMail->szText), pstMail->szText, &pszUtf8Text);
	if(pszUtf8Text == NULL)
		goto _ERROR;

	slStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
	if(slStream == 0)
		goto _ERROR;

	pszEncodedStr = xmgr_cas_NaUtil_EncodeString(pszUtf8Text);
	if(pszEncodedStr != NULL)
	{
		HLIB_RWSTREAM_Print(slStream, "[\"%s\"]", pszEncodedStr);
		HLIB_STD_MemFree(pszEncodedStr);
		pszEncodedStr = NULL;
	}

	pszProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);
	ulStreamSize = HxSTD_StrLen(pszProperties);

	pstMailMessage->ulTextLength = HxSTD_StrLen(pszUtf8Text);
	pstMailMessage->pszText = (HCHAR *)HLIB_STD_MemCalloc(ulStreamSize+1);
	if(pstMailMessage->pszText != NULL)
		MWC_UTIL_DvbStrncpy(pstMailMessage->pszText, pszProperties, ulStreamSize);

	HLIB_STD_MemFree(pszProperties);
	OxSI_Free(pszUtf8Text);

	if(BUS_PostMessageToOM(NULL, eMEVT_CASUI_TYPE_MAIL_DATA, HANDLE_NULL, (HINT32)eDxCAS_GROUPID_NA, 0, (HINT32)pstMailEvent) != ERR_BUS_NO_ERROR)
		HxLOG_Error("BUS_PostMessageToOM() error\n");

	return ERR_OK;

_ERROR:
	if(pstMailEvent != NULL)	OxCAS_Free(pstMailEvent);
	if(pszUtf8Text != NULL)		OxSI_Free(pszUtf8Text);
	if(slStream != 0)			HLIB_RWSTREAM_Close(slStream);

	return ERR_FAIL;
}

static BUS_Result_t _xmgr_cas_NaUi_MailMessage_MsgCreate(sNaUi_MailMessage_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxSTD_memset(pstContext, 0, sizeof(sNaUi_MailMessage_Context_t));

	pstContext->hSession = HANDLE_NULL;

	return ERR_BUS_NO_ERROR;
}

static BUS_Result_t _xmgr_cas_NaUi_MailMessage_EvtCasCmd(sNaUi_MailMessage_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 ulSlotNumber = 0;

	pstContext->usMailId = (HUINT16)p1;
	pstContext->usDeletedMailId = (HUINT16)p2;

	if(pstContext->hSession != HANDLE_NULL)
		_xmgr_cas_NaUi_MailMessage_SendMailMessage(pstContext);
	else
		xmgr_cas_NaUtil_PostMessageSessionEventToOM(eDxCAS_SESSIONTYPE_EXTENSION, xproc_cas_NaUiMailMessage, HANDLE_NULL, ulSlotNumber, eMEVT_CASUI_REQ_OPEN_SESSION);

	return ERR_BUS_NO_ERROR;
}

static BUS_Result_t _xmgr_cas_NaUi_MailMessage_MsgOpenSession(sNaUi_MailMessage_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{

	OxMgrCasSessionEvent_t	*pstSession = (OxMgrCasSessionEvent_t *)p3;

	pstContext->hSession = pstSession->hSession;

	xmgr_cas_NaUtil_PostMessageSessionEventToOM(eDxCAS_SESSIONTYPE_EXTENSION, xproc_cas_NaUiMailMessage, pstSession->hSession, pstSession->ulCasSlotNum, eMEVT_CASUI_SESSION_OPENED);

	if(_xmgr_cas_NaUi_MailMessage_SendMailMessage(pstContext) != ERR_OK)
		HxLOG_Error("_xmgr_cas_NaUi_MailMessage_SendMailMessage() error\n");

	return ERR_BUS_MESSAGE_PASS;
}

static BUS_Result_t _xmgr_cas_NaUi_MailMessage_MsgCloseSession(sNaUi_MailMessage_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	// TODO: 정리할 내용 추가.
	return BUS_MGR_Destroy(NULL);
}

/*******************************************************************/
/********************      Public Functions      ********************/
/*******************************************************************/

#define _____XMGR_CAS_NA_MAIL_MESSAGE_FUNCTION______________________________________________________
BUS_Result_t	xproc_cas_NaUiMailMessage(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	/* 우선은 IRD-MAIL 이벤트가 발생했을 때 Mail을 표시하도록만 작업함. */

	sNaUi_MailMessage_Context_t *pstContext = _xmgr_cas_NaUi_MailMessage_GetContext();
	BUS_Result_t  eBusResult;
#if defined(CONFIG_DEBUG)
	HUINT8 *pszMsg = xmgr_cas_NaGetMessageName(message);
	if(pszMsg != NULL)
		HxLOG_Print("message(%s), ActId(%x), p1(%x), p2(%x), p3(%x)\n", pszMsg, hAction, p1, p2, p3);
#endif

	switch(message)
	{
		case eMEVT_BUS_CREATE :
			eBusResult = _xmgr_cas_NaUi_MailMessage_MsgCreate(pstContext, message, hAction, p1, p2, p3);
			break;

		case eMEVT_CAS_CMD:
			eBusResult = _xmgr_cas_NaUi_MailMessage_EvtCasCmd(pstContext, message, hAction, p1, p2, p3);
			break;

		case eOEVT_CAS_CASUI_APP_OPEN_SESSION:
			eBusResult = _xmgr_cas_NaUi_MailMessage_MsgOpenSession(pstContext, message, hAction, p1, p2, p3);
			break;

		case eOEVT_CAS_CASUI_APP_CLOSE_SESSION:
			eBusResult = _xmgr_cas_NaUi_MailMessage_MsgCloseSession(pstContext, message, hAction, p1, p2, p3);
			break;
	}

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}

#define _____XMGR_CAS_NA_NORMAL_MAIL_ICON_FUNCTION______________________________________________________
BUS_Result_t	xproc_cas_NaUiMailIcon(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxLOG_Print("xproc_cas_NaUiMailIcon() +\n");

	switch(message)
	{
		case eMEVT_BUS_CREATE :
			BUS_MGR_SetMgrName(BUS_MGR_GetThis(), "xproc_cas_NaUiMailIcon");
		{
			//OSD_Rect_t		rect;

			/* application 영역을 정한다. */
			//BUS_APP_SetAppArea(MAILICON_NA_IRD_APP_X, MAILICON_NA_IRD_APP_Y, MAILICON_NA_IRD_APP_W, MAILICON_NA_IRD_APP_H);

			/* 절대 hide 되지 않는다. */
			//BUS_APP_SetUIHideFlag(BUS_UI_Hide_Never);

			/* icon을 그리지만 뒷 어플들을 dim시키면 안되므로 hidden UI로 뜬다. */
			//BUS_APP_SetAttribute(BUS_HiddenUI_App);

			//OSD_GFX_SetRect(&rect, 0, 0, MAILICON_NA_IRD_APP_W, MAILICON_NA_IRD_APP_H);
			//BUS_Rect_Create(MAILICON_NA_IRD_ID, &rect, 0x00);
			//BUS_APP_SetObjectDrawMethod(MAILICON_NA_IRD_ID, xmgr_cas_Nanormal_mail_DrawBannerIcon);

			//1 To do 여기에서 Appication으로 전달할 Data를 만들자.
			break;
		}

		case eMEVT_BUS_DESTROY :
			//1 To do 여기에서 Appication으로 전달할 Data를 만들자.
			return MESSAGE_BREAK;

		default:
			break;
	}

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}

void XMGR_CAS_NA_MAIL_ICON_Display(void)
{
	if(BUS_MGR_Get(xproc_cas_NaUiMailIcon) != NULL)
	{
		BUS_MGR_Destroy(xproc_cas_NaUiMailIcon);
	}
	//BUS_MGR_Create("xproc_cas_NaUiMailIcon", APP_CAS_HIGH_PRIORITY, xproc_cas_NaMailIcon, (HINT32)0, 0, 0, 0);
}

void XMGR_CAS_NA_MAIL_ICON_Close(void)
{
	if(BUS_MGR_Get(xproc_cas_NaUiMailIcon) != NULL)
	{
		BUS_MGR_Destroy(xproc_cas_NaUiMailIcon);
	}
}

void XMGR_CAS_NA_MAIL_ICON_Check(void)
{
	if(xmgr_cas_NaIrdCmdMailCheckUnreadMessage() == ERR_OK)
	{
		XMGR_CAS_NA_MAIL_ICON_Display();
	}
	else
	{
		XMGR_CAS_NA_MAIL_ICON_Close();
	}
}


/* end of file */
