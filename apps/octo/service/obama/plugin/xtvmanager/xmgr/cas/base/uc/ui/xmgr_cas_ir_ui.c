/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <db_svc.h>

#include <isosvc.h>

#include <svc_pipe.h>
#include <svc_cas.h>
#include <ir_api.h>
#include <ir_msg.h>
#include <ir_evt.h>
#include <ir_ctrl.h>
//#include <_svc_cas_mgr_control.h>

#include <mgr_common.h>
#include <mgr_live.h>
#include <mgr_cas.h>
#include <xmgr_cas_res_str.h>

#include "../local_include/_xmgr_cas_ir.h"
#include "../local_include/_xmgr_cas_ir_adapt.h"
#include "../local_include/_xmgr_cas_ir_util.h"
#include "../local_include/_xmgr_cas_ir_ui.h"


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#if defined(CONFIG_DEBUG)

#define	LOG					HxLOG_Print
#define 	INFO_LOG			HxLOG_Info
#define	ERR_LOG			HxLOG_Error
#define 	MSG_LOG			HxLOG_Message
#define	FuncENTER			HxLOG_Info("(+)Enter!!\n");
#define	FuncLEAVE(X)			HxLOG_Info("(-)Leave -- (0x%X)\n",X);
#define	DELAY_LOG(X)		OCTO_NULL_PRINT
#define 	PAUSE				OCTO_NULL_PRINT

#else /* defined(CONFIG_DEBUG) */

#define	LOG					OCTO_NULL_PRINT
#define 	INFO_LOG			OCTO_NULL_PRINT
#define	ERR_LOG			OCTO_NULL_PRINT
#define 	MSG_LOG			OCTO_NULL_PRINT
#define	FuncENTER			OCTO_NULL_PRINT
#define	FuncLEAVE(X)			OCTO_NULL_PRINT
#define	DELAY_LOG(X)		OCTO_NULL_PRINT
#define 	PAUSE				OCTO_NULL_PRINT

#endif /* defined(CONFIG_DEBUG) */




#define IR_TEXT_SIZE256				256

// Safe Area
#define	OSD_SCREEN_SAFE_X			96

#define	FP_FRAME_W					300
#define	FP_FRAME_H					70
#define	FP_FRAME_X					40
#define	FP_FRAME_Y					300

#define	NORMAL_FP_TIMEOUT					(7) /* based on DVT automation phase, 3.9 FINGERPRINTING.MPG, Note/Requirement */

#define	MAX_MAIL_MESSAGE_LENGTH			1024 // Irdeto 최대 512 + 각종 header들.

// flexflash
#define	IRUC_FLEXFLASH_TIMEOUT_ID			4
#define	IRUC_FLEXFLASH_TIMEOUT		5000 /* uc lib write 하는 시간  sec */



/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/




typedef struct
{
	HUINT32		ulLastErrCode;
	OxMgrCasUiEvtSession_t	stSessionInfo;
	HUINT32					ulSessionStatus;
	HUINT32					ulSlot;
	HUINT8					szCasIrMessage[IR_TEXT_SIZE256];
} IR_UI_SysMsg_Context_t;


typedef struct
{
	OxMgrCasUiEvtSession_t	stSessionInfo;
	HUINT32					ulSessionStatus;
	HUINT32					ulSlot;
	HUINT8					iconType;
} IR_UI_MailIcon_Context_t;


typedef struct
{
	OxMgrCasUiEvtSession_t	stSessionInfo;
	HUINT32					ulSessionStatus;
	HUINT32					ulSlot;
	ATTRIBUTED_DISPLAY		Atrb_Display;
	IREVT_CLASSe				irEvtClass;
	HUINT8					mailIdx;
	HUINT8					szCas_ir_mail_message_text[MAX_ATTRIBUTED_TEXT_LENGTH];
	HCHAR					strReceivedDate[MGR_CAS_MAIL_RECEIVED_DATE_LENGTH];
	HCHAR					strReceivedTime[MGR_CAS_MAIL_RECEIVED_TIME_LENGTH];
	HUINT8					bAttributeMsg;
	HUINT8					iconType;

} IR_UI_MailMessage_Context_t;


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC IR_UI_SysMsg_Context_t			s_stSysMsgContext;
STATIC IR_UI_MailIcon_Context_t		s_cas_ir_mail_icon;
STATIC IR_UI_MailMessage_Context_t		s_cas_ir_mail_message;


/*******************************************************************/
/********************      Local Functions      ********************/
/*******************************************************************/

STATIC HERROR xmgr_cas_ir_ui_PostHideMessage(HUINT32 ulSlot, IR_UI_SysMsg_Context_t *pstContext);
STATIC HERROR	xmgr_cas_ir_ui_PostMessageSessionEventToOM(OxMgrCasUiEvtSession_t *pstSessionInfo, HUINT32 ulSlotNumber, HINT32 eEvent);
STATIC HCHAR *xmgr_cas_ir_ui_EncodeString(HCHAR *string);

STATIC IR_UI_MailIcon_Context_t 	*xmgr_cas_ir_ui_MailIcon_GetLocalContents(void);
STATIC BUS_Result_t	xmgr_cas_ir_ui_MailIcon_MsgGwmCreate (IR_UI_MailIcon_Context_t *pContents , HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t 	xmgr_cas_ir_ui_MailIcon_MsgGwmDestroy(IR_UI_MailIcon_Context_t *pContents);
STATIC BUS_Result_t xmgr_cas_ir_ui_MailIcon_MsgIrdetoUpdateUi(IR_UI_MailIcon_Context_t *pContents, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC HERROR xmgr_cas_ir_ui_MailIcon_PostMessage(IR_UI_MailIcon_Context_t *pstContext);
STATIC BUS_Result_t xmgr_cas_ir_ui_MailIcon_EvtChangeState(IR_UI_MailIcon_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t xmgr_cas_ir_ui_MailIcon_MsgReqSessionStart(IR_UI_MailIcon_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t xmgr_cas_ir_ui_MailIcon_MsgReqSessionStop(IR_UI_MailIcon_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t xmgr_cas_ir_ui_MailIcon_MsgSessionStopped(IR_UI_MailIcon_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t xmgr_cas_ir_ui_MailIcon_MsgSessionStarted(IR_UI_MailIcon_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

STATIC IR_UI_MailMessage_Context_t 	*xmgr_cas_ir_ui_MailMessage_GetLocalContents(void);
STATIC HERROR xmgr_cas_ir_ui_MailMessage_MsgIrdetoUpdateUi(IR_UI_MailMessage_Context_t *pContents, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t	xmgr_cas_ir_ui_MailMessage_MsgGwmCreate (IR_UI_MailMessage_Context_t *pContents , HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t 	xmgr_cas_ir_ui_MailMessage_MsgGwmDestroy(IR_UI_MailMessage_Context_t *pContents);
STATIC HERROR xmgr_cas_ir_ui_MailMessage_PostMessage(IR_UI_MailMessage_Context_t *pstContext);
STATIC BUS_Result_t xmgr_cas_ir_ui_MailMessage_EvtChangeState(IR_UI_MailMessage_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t xmgr_cas_ir_ui_MailMessage_MsgReqSessionStart(IR_UI_MailMessage_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t xmgr_cas_ir_ui_MailMessage_MsgReqSessionStop(IR_UI_MailMessage_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t xmgr_cas_ir_ui_MailMessage_MsgSessionStopped(IR_UI_MailMessage_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t xmgr_cas_ir_ui_MailMessage_MsgSessionStarted(IR_UI_MailMessage_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

STATIC IR_UI_SysMsg_Context_t *xmgr_cas_ir_ui_GetSysMsgContext(void);
STATIC HERROR xmgr_cas_ir_ui_SysMsg_PostBannerMessage(IR_UI_SysMsg_Context_t *pstContext);
STATIC BUS_Result_t xmgr_cas_ir_ui_SysMsg_MsgCreate(IR_UI_SysMsg_Context_t *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t xmgr_cas_ir_ui_SysMsg_EvtChangeState(IR_UI_SysMsg_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t xmgr_cas_ir_ui_SysMsg_MsgIrdetoUpdateUi(IR_UI_SysMsg_Context_t *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t xmgr_cas_ir_ui_SysMsg_MsgReqSessionStart(IR_UI_SysMsg_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t xmgr_cas_ir_ui_SysMsg_MsgReqSessionStop(IR_UI_SysMsg_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t xmgr_cas_ir_ui_SysMsg_MsgSessionStopped(IR_UI_SysMsg_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t xmgr_cas_ir_ui_SysMsg_MsgSessionStarted(IR_UI_SysMsg_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);




#define _____XMGR_CAS_IR_UI_LOCAL_FUNCTION______________________________________________________


STATIC HERROR xmgr_cas_ir_ui_PostHideMessage(HUINT32 ulSlot, IR_UI_SysMsg_Context_t *pstContext)
{
	HERROR					hErr = ERR_OK;
	BUS_Result_t			hBusResult = ERR_BUS_NO_ERROR;

#if 1
	OxMgrCasUiEvtBanner_t	*pstEvent = NULL;

	pstEvent = (OxMgrCasUiEvtBanner_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiEvtBanner_t));


	pstEvent->stSessionInfo.eSessionType = pstContext->stSessionInfo.eSessionType;
	pstEvent->stSessionInfo.hSession = pstContext->stSessionInfo.hSession;
	pstEvent->stSessionInfo.ulUiHandle = pstContext->stSessionInfo.ulUiHandle;


	/* Display할 String length를 0으로 할 경우 Banner 를 hide시킨다. */
	pstEvent->stBanner.stString.ulStringLength = 0;

	hBusResult = BUS_PostMessageToOM(NULL, eMEVT_CASUI_TYPE_BANNER_DATA, HANDLE_NULL, (HINT32)eDxCAS_GROUPID_IR, (HINT32)ulSlot, (HINT32)pstEvent);
	if(hBusResult != ERR_BUS_NO_ERROR)
	{
		hErr = ERR_FAIL;
	}
#else 	// TODO: shko 혹시 session 을 닫아야 하나...
	hBusResult = xmgr_cas_ir_ui_PostMessageSessionEventToOMToOM(eSessionType, ulUiHandle, ulSlot, eMEVT_CASUI_REQ_CLOSE_SESSION);
	if(hBusResult != ERR_BUS_NO_ERROR)
	{
		hErr = ERR_FAIL;
	}
#endif

	return hErr;
}


STATIC HERROR	xmgr_cas_ir_ui_PostMessageSessionEventToOM(OxMgrCasUiEvtSession_t *pstSessionInfo, HUINT32 ulSlotNumber, HINT32 eEvent)
{
	HERROR								hError = ERR_OK;
	BUS_Result_t						hResult = ERR_BUS_NO_ERROR;
	OxMgrCasUiEvtSession_t				*pstEvent = NULL;

	HxLOG_Trace();

	pstEvent = (OxMgrCasUiEvtSession_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiEvtSession_t));

	HxLOG_Info("eSessionType: %d \n", pstSessionInfo->eSessionType);
	pstEvent->eSessionType = pstSessionInfo->eSessionType;
	pstEvent->ulUiHandle = pstSessionInfo->ulUiHandle;
	pstEvent->hSession = pstSessionInfo->hSession;
	HxLOG_Debug("hSession: 0x%x, ulUiHandle: 0x%x \n", pstEvent->hSession, pstEvent->ulUiHandle);

	hResult = BUS_PostMessage(NULL, eEvent, HANDLE_NULL, eDxCAS_GROUPID_IR, ulSlotNumber, (HINT32)pstEvent);
	if(hResult != ERR_BUS_NO_ERROR)
	{
		hError = ERR_FAIL;
	}

	return hError;
}


STATIC HCHAR *xmgr_cas_ir_ui_EncodeString(HCHAR *string)
{
	HUINT32		Len = 0;
	HCHAR		*buf = NULL;

	Len = HLIB_STD_URLStrLen(string) + 1;
	buf = HLIB_STD_MemCalloc(Len+1);
	HLIB_STD_URLEncode(buf,string,Len);

	return buf;
}



#define _____Local_MailIcon____________________________________________________




STATIC IR_UI_MailIcon_Context_t 	*xmgr_cas_ir_ui_MailIcon_GetLocalContents(void)
{
	return &s_cas_ir_mail_icon;
}

STATIC BUS_Result_t	xmgr_cas_ir_ui_MailIcon_MsgGwmCreate (IR_UI_MailIcon_Context_t *pContents , HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					ulSlot = 0;

	HxLOG_Info("[xmgr_cas_ir_ui_MailIcon_MsgGwmCreate] iocn type:%d, p2:%d, eSessionType: %d \n", p1, p2, p3);

	HxSTD_MemSet(pContents, 0x00, sizeof(IR_UI_MailIcon_Context_t));

	pContents->ulSessionStatus = eIR_UI_SESSION_STATUS_Stopped;
	pContents->stSessionInfo.ulUiHandle = (HUINT32)xproc_cas_IrMailIcon;
	pContents->ulSlot = ulSlot;
	pContents->iconType = p1;
	return	ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t 	xmgr_cas_ir_ui_MailIcon_MsgGwmDestroy(IR_UI_MailIcon_Context_t *pContents)
{
	// TODO: shko 혹시 session 을 닫아야 하나...

	return	ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t xmgr_cas_ir_ui_MailIcon_MsgIrdetoUpdateUi(IR_UI_MailIcon_Context_t *pContents, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{


	pContents->iconType = p1;
	pContents->ulSlot = 0;
	pContents->stSessionInfo.eSessionType = p3;

	HxLOG_Print("\t Mail Icon type = 0x%x  (0x00: mailbox, 0x10: announcement, 0x20: club number\n", pContents->iconType);

	return ERR_BUS_NO_ERROR;
}

STATIC HERROR xmgr_cas_ir_ui_MailIcon_PostMessage(IR_UI_MailIcon_Context_t *pstContext)
{
	HERROR					hErr = ERR_OK;
	BUS_Result_t				hBusResult = ERR_BUS_NO_ERROR;
	OxMgrCasUiEvtMail_t			*pstMailEvent = NULL;
	HUINT32 					ulSlot = pstContext->ulSlot;
	DxCAS_MmiSessionType_e 		eSessionType = pstContext->stSessionInfo.eSessionType;

	pstMailEvent = (OxMgrCasUiEvtMail_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiEvtMail_t));
	//pstMailEvent = xmgr_cas_ir_ui_MailIcon_GetEventData();

	if(pstMailEvent == (OxMgrCasUiEvtMail_t *)NULL)
	{
		HxLOG_Error("\n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}
	pstMailEvent->stSessionInfo.eSessionType = eSessionType;
	pstMailEvent->stSessionInfo.ulUiHandle = (HUINT32)pstContext->stSessionInfo.ulUiHandle;
	pstMailEvent->stSessionInfo.hSession = (HUINT32)pstContext->stSessionInfo.hSession;

	pstMailEvent->stMail.stHeader.bUseIcon = TRUE;
	pstMailEvent->stMail.stHeader.ulMailIconType = pstContext->iconType;

	pstMailEvent->stMail.ulTextLength = 0;

	hBusResult = BUS_PostMessageToOM(NULL, eMEVT_CASUI_TYPE_MAIL_DATA, HANDLE_NULL, (HINT32)eDxCAS_GROUPID_IR, (HINT32)ulSlot, (HINT32)pstMailEvent);
	if(hBusResult != ERR_BUS_NO_ERROR)
	{
		hErr = ERR_FAIL;
		goto END_FUNC;
	}


END_FUNC:
	if(hErr == ERR_FAIL)
	{
		if(pstMailEvent != NULL)
		{
			if(pstMailEvent->stMail.pszText != NULL)
			{
				HLIB_STD_MemFree(pstMailEvent->stMail.pszText);
			}
			HLIB_STD_MemFree(pstMailEvent);
		}
	}

	return hErr;
}


STATIC BUS_Result_t xmgr_cas_ir_ui_MailIcon_EvtChangeState(IR_UI_MailIcon_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{

	HxLOG_Info("p1: 0x%x, p2: %d, p3: %d, pstContext->ulSessionStatus: %d, pstContext->eSessionType: %d\n",
		p1, p2, p3, pstContext->ulSessionStatus, pstContext->stSessionInfo.eSessionType);

	switch(pstContext->ulSessionStatus)
	{
		case eIR_UI_SESSION_STATUS_Stopped:
			pstContext->ulSessionStatus = eIR_UI_SESSION_STATUS_Started;
			xmgr_cas_ir_ui_PostMessageSessionEventToOM(&(pstContext->stSessionInfo), pstContext->ulSlot, eMEVT_CASUI_REQ_OPEN_SESSION);//~~~~~~~
		case eIR_UI_SESSION_STATUS_Started:
			xmgr_cas_ir_ui_MailIcon_PostMessage(pstContext);
			break;

		default:
			break;
	}

	return ERR_BUS_MESSAGE_PASS;
}


 // TODO: shko fp랑, sys msg랑 똑같은 함수인데..합칠까.?
STATIC BUS_Result_t xmgr_cas_ir_ui_MailIcon_MsgReqSessionStart(IR_UI_MailIcon_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	OxMgrCasSessionEvent_t 				stSessionEvent;
	DxCAS_MmiSessionType_e			eSessionType;
	HUINT32 							ulSlotNumber;
	Handle_t							hSession;

	HxSTD_memcpy(&stSessionEvent, (void*)p3, sizeof(OxMgrCasSessionEvent_t));
	ulSlotNumber = stSessionEvent.ulCasSlotNum;
	eSessionType= stSessionEvent.eSessionType;
	hSession = stSessionEvent.hSession;


	pstContext->stSessionInfo.hSession = hSession;

	LOG("[%s:%d] SlotNumber %d, SESSIONTYPE %d, SESSIONSTATUS %d\n", __FUNCTION__, __LINE__,
		ulSlotNumber, eSessionType, pstContext->ulSessionStatus);

	switch(pstContext->ulSessionStatus)
	{
		case eIR_UI_SESSION_STATUS_Stopped:
			pstContext->ulSessionStatus = eIR_UI_SESSION_STATUS_Started;
			/* continue */
		case eIR_UI_SESSION_STATUS_Started:
			xmgr_cas_ir_ui_PostMessageSessionEventToOM(&(pstContext->stSessionInfo), ulSlotNumber, eMEVT_CASUI_SESSION_OPENED);
			xmgr_cas_ir_ui_MailIcon_PostMessage(pstContext);
			break;
		default:
			break;
	}

	return MESSAGE_BREAK;
}

STATIC BUS_Result_t xmgr_cas_ir_ui_MailIcon_MsgReqSessionStop(IR_UI_MailIcon_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	OxMgrCasSessionEvent_t 				stSessionEvent;
	DxCAS_MmiSessionType_e			eSessionType;
	HUINT32 							ulSlotNumber;
	Handle_t							hSession;

	HxSTD_memcpy(&stSessionEvent, (void*)p3, sizeof(OxMgrCasSessionEvent_t));
	ulSlotNumber = stSessionEvent.ulCasSlotNum;
	eSessionType= stSessionEvent.eSessionType;
	hSession = stSessionEvent.hSession;

	LOG("[%s:%d] SlotNumber %d, SESSIONSTATUS %d\n", __FUNCTION__, __LINE__,
		ulSlotNumber, pstContext->ulSessionStatus);

	switch(pstContext->ulSessionStatus)
	{
		case eIR_UI_SESSION_STATUS_Started:
			pstContext->ulSessionStatus = eIR_UI_SESSION_STATUS_Stopped;
			/* continue */
		case eIR_UI_SESSION_STATUS_Stopped:
			HxLOG_Print("\n\n\n\n xmgr_cas_ir_ui_MailIcon_MsgReqSessionStop \n\n\n\n");
			xmgr_cas_ir_ui_PostHideMessage(ulSlotNumber, pstContext);
			HxLOG_Print("\n\n\n\n xmgr_cas_ir_ui_MailIcon_MsgReqSessionStop \n\n\n\n");
			break;

		default:
			break;
	}

	return MESSAGE_BREAK;
}

STATIC BUS_Result_t xmgr_cas_ir_ui_MailIcon_MsgSessionStopped(IR_UI_MailIcon_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					ulSlotNumber = 0;
	DxCAS_MmiSessionType_e		eSessionType = eDxCAS_SESSIONTYPE_NONE;

	ulSlotNumber = (HUINT32)p1;
	eSessionType = (DxCAS_MmiSessionType_e)p2;

	LOG("[%s:%d] SlotNumber %d, SESSIONTYPE %d, SESSIONSTATUS %d\n", __FUNCTION__, __LINE__,
		ulSlotNumber, eSessionType, pstContext->ulSessionStatus);

	switch(pstContext->ulSessionStatus)
	{
		case eIR_UI_SESSION_STATUS_Started:
			pstContext->ulSessionStatus = eIR_UI_SESSION_STATUS_Stopped;
			/* continue */
		case eIR_UI_SESSION_STATUS_Stopped:
			break;

		default:
			break;
	}

	return MESSAGE_BREAK;
}

STATIC BUS_Result_t xmgr_cas_ir_ui_MailIcon_MsgSessionStarted(IR_UI_MailIcon_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	OxMgrCasSessionEvent_t 				stSessionEvent;
	DxCAS_MmiSessionType_e			eSessionType;
	HUINT32 							ulSlotNumber;
	Handle_t							hSession;

	HxSTD_memcpy(&stSessionEvent, (void*)p3, sizeof(OxMgrCasSessionEvent_t));
	ulSlotNumber = stSessionEvent.ulCasSlotNum;
	eSessionType= stSessionEvent.eSessionType;
	hSession = stSessionEvent.hSession;

	LOG("[%s:%d] SlotNumber %d, SESSIONTYPE %d, SESSIONSTATUS %d\n", __FUNCTION__, __LINE__,
		ulSlotNumber, eSessionType, pstContext->ulSessionStatus);

	switch(pstContext->ulSessionStatus)
	{
		case eIR_UI_SESSION_STATUS_Stopped:
			pstContext->ulSessionStatus = eIR_UI_SESSION_STATUS_Started;
			xmgr_cas_ir_ui_PostMessageSessionEventToOM(&(pstContext->stSessionInfo), ulSlotNumber, eMEVT_CASUI_SESSION_OPENED);
			/* continue */
		case eIR_UI_SESSION_STATUS_Started:
			xmgr_cas_ir_ui_MailIcon_PostMessage(pstContext);
			break;

		default:
			break;
	}

	return MESSAGE_BREAK;
}


#define _____Local_MailMessage____________________________________________________


STATIC IR_UI_MailMessage_Context_t 	*xmgr_cas_ir_ui_MailMessage_GetLocalContents(void)
{
	return &s_cas_ir_mail_message;
}

STATIC HERROR xmgr_cas_ir_ui_MailMessage_MsgIrdetoUpdateUi(IR_UI_MailMessage_Context_t *pContents, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	MAILBOX_TABLE		MailInfo;
	SvcCas_CtrlParam_t		stIrCtrlParam;
	IR_GetMailBoxOutParam_t	stMailBoxParam;
	HUINT32					ulSlot = 0;
	HERROR					hErr = ERR_OK;



	pContents->ulSlot = ulSlot;
	pContents->irEvtClass = p1;
	pContents->stSessionInfo.eSessionType = p3;


	HxLOG_Print("xmgr_cas_ir_ui_MailMessage_MsgIrdetoUpdateUi() + mailIndex (0x%x) + pContents->ulSessionStatus (%d)\n", pContents->mailIdx, pContents->ulSessionStatus);

	HxSTD_MemSet(pContents->szCas_ir_mail_message_text, 0x00, MAX_ATTRIBUTED_TEXT_LENGTH);
	pContents->szCas_ir_mail_message_text[0]='\0';
	HxSTD_MemSet(&(pContents->Atrb_Display), 0x00, sizeof(ATTRIBUTED_DISPLAY));

	HxSTD_MemSet(pContents->strReceivedDate, 0x00, MGR_CAS_MAIL_RECEIVED_DATE_LENGTH);
	pContents->strReceivedDate[0]='\0';

	HxSTD_MemSet(pContents->strReceivedTime, 0x00, MGR_CAS_MAIL_RECEIVED_TIME_LENGTH);
	pContents->strReceivedTime[0]='\0';


	switch(pContents->irEvtClass)
	{
		case IREVT_CLASS_DECODER_MSG_FORCED_DISPLAY:
		case IREVT_CLASS_DECODER_MSG_NORMAL_MAIL:
		case IREVT_CLASS_DECODER_MSG_ENHANCED_TEXT:
			//xsvc_cas_IrGetMailBox(pContents->mailIdx, &MailInfo);

			pContents->mailIdx = p2;


			stIrCtrlParam.ulControlType = eCACTRL_IR_GetMailBox;
			stIrCtrlParam.pvOut 	= &stMailBoxParam;
			stMailBoxParam.ucMailIndex = pContents->mailIdx;
			stMailBoxParam.pstMailBox	= &MailInfo;

			SVC_CAS_CtrlGet(eDxCAS_GROUPID_IR, eCACTRL_IR_GetMailBox, &stIrCtrlParam);

			HxSTD_memcpy(&(pContents->Atrb_Display), &MailInfo.Atrb_Display, sizeof(ATTRIBUTED_DISPLAY));
			HxSTD_memcpy(pContents->szCas_ir_mail_message_text, MailInfo.text, MAX_ATTRIBUTED_TEXT_LENGTH);
			pContents->bAttributeMsg = MailInfo.bAttributeMsg;

			HxLOG_Print("\tMsgType = %d\n", pContents->Atrb_Display.ucMsg_Type);
			HxLOG_Print("\tDuration = %d\n", pContents->Atrb_Display.unDuration);
			HxLOG_Print("\tDisplayMethod = %d\n", pContents->Atrb_Display.ucDisplay_Method);
			HxLOG_Print("\tFlashing = %d\n", pContents->Atrb_Display.bFlashing);
			HxLOG_Print("\tNormal = %d\n", pContents->Atrb_Display.bNormal);
			HxLOG_Print("\tCoverage = %d\n", pContents->Atrb_Display.ucCoverage);
			HxLOG_Print("\tbAttributeMsg = %d\n", pContents->bAttributeMsg);

			// Attributed Finger print가 Mail에 같이 저장 된다. 그래서 출력할때 삭제 해야 한다. 그래야
			// 다음 Attributed Finger print가 출력 된다.
			if (pContents->Atrb_Display.ucMsg_Type == ATTRIBUTED_MESSAGE_TYPE_FINGERPRINT
				|| pContents->Atrb_Display.ucMsg_Type == ATTRIBUTED_MESSAGE_TYPE_FINGERPRINT_COVERT_OVERT_OPTION
			)
			{

				SvcCas_CtrlParam_t		stIrCtrlParam;
				IR_GetCheckMailOutParam_t	stCheckMailParam;
				HUINT8					mailIdx;
				SvcCas_CtrlParam_t			stInParam;
				IR_DeleteMailInParam_t		stDeleteMail;

				HxLOG_Print("\t Delete Finger Print MSG!!, stMailBoxParam.ucMailIndex: %d, ucFingerprint_Type = %d\n", stMailBoxParam.ucMailIndex, pContents->Atrb_Display.ucFingerprint_Type);

				//xsvc_cas_IrDeleteMail(pContents->mailIdx);

				mailIdx = stMailBoxParam.ucMailIndex;

				stIrCtrlParam.ulControlType = eCACTRL_IR_CheckValidOfMail;
				stIrCtrlParam.pvOut 	= &stCheckMailParam;
				stCheckMailParam.ucMailIndex = mailIdx;
				SVC_CAS_CtrlGet(eDxCAS_GROUPID_IR, eCACTRL_IR_CheckValidOfMail, &stIrCtrlParam);
				HxLOG_Info("stCheckMailParam.bCheckMail: %d \n", stCheckMailParam.bCheckMail);
				if (stCheckMailParam.bCheckMail == TRUE)
				{
					stInParam.ulControlType		= eCACTRL_IR_DeleteMail;
					stInParam.pvIn				= &stDeleteMail;
					stDeleteMail.ucMailIndex = mailIdx;

					SVC_CAS_CtrlCall(eDxCAS_GROUPID_IR, eCACTRL_IR_DeleteMail, &stInParam);

				}


			}

			snprintf(pContents->strReceivedDate, MGR_CAS_MAIL_RECEIVED_DATE_LENGTH, "[\"%02d/%02d\"]", MailInfo.add_date.ucDay, MailInfo.add_date.ucMonth);
			snprintf(pContents->strReceivedTime, MGR_CAS_MAIL_RECEIVED_TIME_LENGTH, "[\"%02d:%02d\"]", MailInfo.add_time.ucHour, MailInfo.add_time.ucMinute);

			pContents->iconType = MailInfo.ucType;

			break;

		case	IREVT_CLASS_DECODER_MSG_ECM_FINGERPRINT:
		case	IREVT_CLASS_DECODER_MSG_EMM_FINGERPRINT:
			HxLOG_Info("ECM/EMM FP \n");
			HxSTD_memcpy(pContents->szCas_ir_mail_message_text, (HUINT8*)p2, IR_SC_INFO_ID_LENGTH);
			break;

		default:
			break;
	}

// "Finger print option ADM (0x03)" 의 경우 ir_evt.c 에서 CONFIG_MW_CAS_IRDETO_SUPPORT_COVERT_FINGERPRINT 유무에 따라 xmgr_cas_ir_ui로 event를 줄지/안줄지 결정
// 이 부분에서 이 부분을 막는 의미는 일반 "Finger print ADM (0x02)" 인데 type이 covert로 오면 overt로 display 해 달라는 irdeto 요청에 의함.
#if 0//!defined(CONFIG_MW_CAS_IRDETO_SUPPORT_COVERT_FINGERPRINT)
	if(pContents->irEvtClass  == IREVT_CLASS_DECODER_MSG_FORCED_DISPLAY
		|| pContents->irEvtClass  == IREVT_CLASS_DECODER_MSG_NORMAL_MAIL
		|| pContents->irEvtClass  == IREVT_CLASS_DECODER_MSG_ENHANCED_TEXT)
	{
		if (pContents->Atrb_Display.ucMsg_Type == ATTRIBUTED_MESSAGE_TYPE_FINGERPRINT_COVERT_OVERT_OPTION)
		{
			HxLOG_Print("Do not support  ATTRIBUTED_MESSAGE_TYPE_FINGERPRINT_COVERT_OVERT_OPTION!!\n");
			hErr = ERR_FAIL;
		}

		if(pContents->Atrb_Display.ucFingerprint_Type == BIT_FP_TYPE_COVERT)
		{
			HxLOG_Print("Do not support  BIT_FP_TYPE_COVERT!!\n");
			hErr = ERR_FAIL;
		}
	}
#endif


	HxLOG_Print("\tText = %s\n", pContents->szCas_ir_mail_message_text);

	return hErr;
}

STATIC BUS_Result_t	xmgr_cas_ir_ui_MailMessage_MsgGwmCreate (IR_UI_MailMessage_Context_t *pContents , HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					ulSlot = 0;

	HxLOG_Info("[xmgr_cas_ir_ui_MailMessage_MsgGwmCreate] irEvtClass:%d, mailIdx:%d, eSessionType: %d \n", p1, p2, p3);

	HxSTD_MemSet(pContents, 0x00, sizeof(IR_UI_MailMessage_Context_t));

	pContents->ulSessionStatus = eIR_UI_SESSION_STATUS_Stopped;
	pContents->stSessionInfo.ulUiHandle = (HUINT32)xproc_cas_IrMailMessage;
	pContents->ulSlot = ulSlot;
	pContents->irEvtClass = p1;
	pContents->mailIdx = p2;
	pContents->stSessionInfo.eSessionType = p3;

	return	ERR_BUS_NO_ERROR;
}


STATIC BUS_Result_t 	xmgr_cas_ir_ui_MailMessage_MsgGwmDestroy(IR_UI_MailMessage_Context_t *pContents)
{
	// TODO: shko 혹시 session 을 닫아야 하나...
	switch (pContents->Atrb_Display.ucMsg_Type)
	{
		case ATTRIBUTED_MESSAGE_TYPE_FINGERPRINT:
			break;

		case ATTRIBUTED_MESSAGE_TYPE_FORCEDTEXT:
			if ( pContents->Atrb_Display.bNormal == BIT_DISP_METHOD_NORMAL )	// Normal (with Title bar)
			{
			}
			break;
		default:
			break;
	}

	return	ERR_BUS_NO_ERROR;
}

STATIC HERROR xmgr_cas_ir_ui_MailMessage_PostMessage(IR_UI_MailMessage_Context_t *pstContext)
{
	HERROR					hErr = ERR_OK;
	BUS_Result_t				hBusResult = ERR_BUS_NO_ERROR;
	OxMgrCasUiEvtMail_t			*pstMailEvent = NULL;
	HINT32					slStream;
	HCHAR					*pzEncodedStr = NULL;
	HCHAR					*pzProperties = NULL;
	HUINT32 					ulSlot = pstContext->ulSlot;
	DxCAS_MmiSessionType_e 		eSessionType = pstContext->stSessionInfo.eSessionType;
	HUINT32					ulStreamSize = 0;
	HUINT8					*pszUtf8Text = NULL;
	HUINT32					ulStringLength = 0;
#if !defined(USE_WEB_UI_COORDINATE)
	HUINT32 					ulSystemTick = 0;
	HUINT32					ulSystemTick_y = 0;
#endif

	pstMailEvent = (OxMgrCasUiEvtMail_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiEvtMail_t));
	//pstMailEvent = xmgr_cas_ir_ui_MailMessage_GetEventData();

	if(pstMailEvent == (OxMgrCasUiEvtMail_t *)NULL)
	{
		HxLOG_Error("\n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	pstMailEvent->stSessionInfo.eSessionType = eSessionType;
	pstMailEvent->stSessionInfo.ulUiHandle = (HUINT32)pstContext->stSessionInfo.ulUiHandle;
	pstMailEvent->stSessionInfo.hSession = (HUINT32)pstContext->stSessionInfo.hSession;

	 // TODO: 필요하다면 다음 추가된 항목들 처리
	#if 0
	HBOOL				bRead;

	HUINT32 				ulBgColor;
	HUINT32 				ulFgColor;
	HUINT32 				ulProgress;

	/** < String */
	HCHAR					szTitle[MGR_CAS_STR_LENGTH_LONG];		/** < 타이틀 영역에 표시될 스트링 */
	HCHAR					szSubTitle[MGR_CAS_STR_LENGTH_LONG]; 	/** < 서브타이틀 영역에 표시될 스트링 */
	HCHAR					szBottom[MGR_CAS_STR_LENGTH_LONG];		/** < 하단 영역에 표시된 스트링 */

	#endif

	pstMailEvent->stMail.stHeader.nMailIdx = pstContext->mailIdx;
	pstMailEvent->stMail.stHeader.nMailType = pstContext->Atrb_Display.ucMsg_Type;

	pstMailEvent->stMail.stHeader.ulAttr = 0;

	HxSTD_memcpy(pstMailEvent->stMail.stHeader.strReceivedDate, pstContext->strReceivedDate, MGR_CAS_MAIL_RECEIVED_DATE_LENGTH);
	HxSTD_memcpy(pstMailEvent->stMail.stHeader.strReceivedTime, pstContext->strReceivedTime, MGR_CAS_MAIL_RECEIVED_TIME_LENGTH);

#if defined(USE_WEB_UI_COORDINATE)
	pstMailEvent->stMail.stHeader.ulCoverage = pstContext->Atrb_Display.ucCoverage;
#endif
	pstMailEvent->stMail.stHeader.bIsAttributed = pstContext->bAttributeMsg;
	switch(pstContext->irEvtClass)
	{
		case IREVT_CLASS_DECODER_MSG_FORCED_DISPLAY:
		case IREVT_CLASS_DECODER_MSG_NORMAL_MAIL:
		case IREVT_CLASS_DECODER_MSG_ENHANCED_TEXT:

			if(pstContext->Atrb_Display.bFlashing)
			{
				pstMailEvent->stMail.stHeader.ulAttr |= eMgrCasUiAttr_Flashing;
			}

			if(pstContext->Atrb_Display.bNormal)
			{
				pstMailEvent->stMail.stHeader.ulAttr |= eMgrCasUiAttr_Title;
				// 모든 String 관리는 Web에서 처리하는 것으로 약속됨에 따라 AP_CasResStr_GetStrRsc() 함수를 더이상 사용하지 말아야 함.
				// 따라서 LOC ID를 Web으로 전송 후 Web에서 그에 맞는 string을 찾아 출력하도록 함.
				snprintf(pstMailEvent->stMail.stHeader.szTitle , MGR_CAS_STR_LENGTH_LONG, "[{\"id\":\"LOC_MAIL_MESSAGES_ID\"}]");
			}

			if (pstContext->Atrb_Display.ucMsg_Type == ATTRIBUTED_MESSAGE_TYPE_FINGERPRINT
			|| pstContext->Atrb_Display.ucMsg_Type == ATTRIBUTED_MESSAGE_TYPE_FINGERPRINT_COVERT_OVERT_OPTION
				)
			{
				pstMailEvent->stMail.stHeader.bIsFp = TRUE;
				pstMailEvent->stMail.stHeader.ulFpType = pstContext->Atrb_Display.ucFingerprint_Type;

				pstMailEvent->stMail.stHeader.ulAttr |= eMgrCasUiAttr_RandomPosition;
				pstMailEvent->stMail.stHeader.ulAttr |= eMgrCasUiAttr_SetSize;


				if(pstContext->Atrb_Display.ucFingerprint_Type == BIT_FP_TYPE_COVERT)
				{
#if !defined(USE_WEB_UI_COORDINATE)
					pstMailEvent->stMail.stHeader.ulHeight = HLIB_MATH_Sqrt(CONFIG_MWC_OSD_VERTICAL*CONFIG_MWC_OSD_VERTICAL * pstContext->Atrb_Display.ucCoverage / 100);
					pstMailEvent->stMail.stHeader.ulWidth = pstMailEvent->stMail.stHeader.ulHeight * pstContext->Atrb_Display.ucCoverage / 100;
					pstMailEvent->stMail.stHeader.ulPosX = (CONFIG_MWC_OSD_HORIZONTAL - pstMailEvent->stMail.stHeader.ulWidth) / 2;
					pstMailEvent->stMail.stHeader.ulPosY = (CONFIG_MWC_OSD_VERTICAL - pstMailEvent->stMail.stHeader.ulHeight) / 2;
#endif
				}
				else
				{

#if !defined(USE_WEB_UI_COORDINATE)
					HINT32		nBannerWidth = 0, nBannerHeight = 0, nBannerWidthTmp=0;
					HINT32		nFrameX = 0, nFrameY = 0;
					HINT32		nTextX = 0, nTextY = 0, lTextShiftX = 0, lTextShiftY = 0;
					HUINT32		ulFontSize = 0;

					nBannerWidth = (CONFIG_MWC_OSD_HORIZONTAL* pstContext->Atrb_Display.ucCoverage) / 100;
					nBannerHeight = (CONFIG_MWC_OSD_VERTICAL*pstContext->Atrb_Display.ucCoverage) / 100;
					nFrameX = (CONFIG_MWC_OSD_HORIZONTAL - nBannerWidth) / 2;

					if (nFrameX < OSD_SCREEN_SAFE_X)
					{
						lTextShiftX = OSD_SCREEN_SAFE_X;
						lTextShiftY = 0;
					}
					else
					{
						lTextShiftX = lTextShiftY = 0;
					}

					nTextX += lTextShiftX;
					nTextY += lTextShiftY;

					 // TODO: 크기에 맞게 font를 그려야 하니..
					//nBannerHeight = xmgr_cas_ir_ui_attr_CalcFpTextHeight(pContents->s_cas_ir_mail_message_text, nBannerWidth - (nTextX*2), &ulFontSize);
					nBannerHeight= nBannerHeight + (nTextY*2);
					nFrameY = (CONFIG_MWC_OSD_VERTICAL - nBannerHeight) / 2;

					pstMailEvent->stMail.stHeader.ulHeight = nBannerHeight;
					pstMailEvent->stMail.stHeader.ulWidth = nBannerWidth;
					pstMailEvent->stMail.stHeader.ulPosX = nFrameX;
					pstMailEvent->stMail.stHeader.ulPosY = nFrameY;
#endif

				}




			}
			else
			{
				pstMailEvent->stMail.stHeader.ulAttr |= eMgrCasUiAttr_SetPosition;
				pstMailEvent->stMail.stHeader.ulAttr |= eMgrCasUiAttr_Multiline;
#if !defined(USE_WEB_UI_COORDINATE)
				pstMailEvent->stMail.stHeader.ulHeight = (CONFIG_MWC_OSD_VERTICAL*pstContext->Atrb_Display.ucCoverage) / 100;
				pstMailEvent->stMail.stHeader.ulWidth = (CONFIG_MWC_OSD_HORIZONTAL* pstContext->Atrb_Display.ucCoverage) / 100;
				pstMailEvent->stMail.stHeader.ulPosX = (CONFIG_MWC_OSD_HORIZONTAL - pstMailEvent->stMail.stHeader.ulWidth) / 2;
				pstMailEvent->stMail.stHeader.ulPosY = (CONFIG_MWC_OSD_VERTICAL - pstMailEvent->stMail.stHeader.ulHeight) / 2;
#endif

			}

			pstMailEvent->stMail.stHeader.ulTimeoutSecond = pstContext->Atrb_Display.unDuration;

			pstMailEvent->stMail.stHeader.ulMailIconType = pstContext->iconType;

			break;

		case	IREVT_CLASS_DECODER_MSG_ECM_FINGERPRINT:
		case	IREVT_CLASS_DECODER_MSG_EMM_FINGERPRINT:
			pstMailEvent->stMail.stHeader.bIsFp = TRUE;
			pstMailEvent->stMail.stHeader.ulFpType = BIT_FP_TYPE_OVERT;

			pstMailEvent->stMail.stHeader.ulTimeoutSecond = NORMAL_FP_TIMEOUT;

			pstMailEvent->stMail.stHeader.ulAttr |= eMgrCasUiAttr_RandomPosition;
			pstMailEvent->stMail.stHeader.ulAttr |= eMgrCasUiAttr_SetSize;
#if !defined(USE_WEB_UI_COORDINATE)
			//fp
			pstMailEvent->stMail.stHeader.ulWidth = FP_FRAME_W;
			pstMailEvent->stMail.stHeader.ulHeight = FP_FRAME_H;

			ulSystemTick = VK_TIMER_GetSystemTick();
			ulSystemTick = ulSystemTick % 900;
			ulSystemTick_y = VK_TIMER_GetSystemTick();
			ulSystemTick_y = ulSystemTick % 600;

			pstMailEvent->stMail.stHeader.ulPosX = FP_FRAME_X + ulSystemTick;
			pstMailEvent->stMail.stHeader.ulPosY = FP_FRAME_Y + ulSystemTick_y;			//nFrameY = FP_FRAME_Y;
#endif
			break;

		default:
			break;
	}


	MWC_UTIL_ConvSiStringToUtf8(NULL, SvcCas_UtilStrLen(pstContext->szCas_ir_mail_message_text), pstContext->szCas_ir_mail_message_text, &pszUtf8Text);
	if(pszUtf8Text == NULL)
	{
		HxLOG_Warning("\n");
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	ulStringLength = SvcCas_UtilStrLen(pszUtf8Text);


	slStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
	pzEncodedStr = xmgr_cas_ir_ui_EncodeString(pszUtf8Text);
	if(pzEncodedStr != NULL)
	{
		HLIB_RWSTREAM_Print(slStream, "[\"%s\"]", pzEncodedStr);
		HLIB_STD_MemFree(pzEncodedStr);
		pzEncodedStr = NULL;
	}

	pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);
	ulStreamSize = HxSTD_StrLen(pzProperties);

	pstMailEvent->stMail.pszText = (HCHAR *)HLIB_STD_MemCalloc(ulStreamSize+1);
	MWC_UTIL_DvbStrncpy(pstMailEvent->stMail.pszText, pzProperties, ulStreamSize);
	pstMailEvent->stMail.ulTextLength = ulStringLength;
	HLIB_STD_MemFree(pzProperties);


	hBusResult = BUS_PostMessageToOM(NULL, eMEVT_CASUI_TYPE_MAIL_DATA, HANDLE_NULL, (HINT32)eDxCAS_GROUPID_IR, (HINT32)ulSlot, (HINT32)pstMailEvent);
	if(hBusResult != ERR_BUS_NO_ERROR)
	{
		hErr = ERR_FAIL;
		goto END_FUNC;
	}


END_FUNC:
	if(hErr == ERR_FAIL)
	{
		if(pstMailEvent != NULL)
		{
			if(pstMailEvent->stMail.pszText != NULL)
			{
				HLIB_STD_MemFree(pstMailEvent->stMail.pszText);
			}
			HLIB_STD_MemFree(pstMailEvent);
		}
	}

	return hErr;
}


STATIC BUS_Result_t xmgr_cas_ir_ui_MailMessage_EvtChangeState(IR_UI_MailMessage_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{

	HxLOG_Info("p1: 0x%x, p2: %d, p3: %d, pstContext->ulSessionStatus: %d, pstContext->eSessionType: %d\n",
		p1, p2, p3, pstContext->ulSessionStatus, pstContext->stSessionInfo.eSessionType);

	switch(pstContext->ulSessionStatus)
	{
		case eIR_UI_SESSION_STATUS_Stopped:
			pstContext->ulSessionStatus = eIR_UI_SESSION_STATUS_Started;
			/* continue */
			xmgr_cas_ir_ui_PostMessageSessionEventToOM(&(pstContext->stSessionInfo), pstContext->ulSlot, eMEVT_CASUI_REQ_OPEN_SESSION);//~~~~~~~
		case eIR_UI_SESSION_STATUS_Started:
			xmgr_cas_ir_ui_MailMessage_PostMessage(pstContext);
			break;

		default:
			break;
	}

	return ERR_BUS_MESSAGE_PASS;
}


STATIC BUS_Result_t xmgr_cas_ir_ui_MailMessage_MsgReqSessionStart(IR_UI_MailMessage_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	OxMgrCasSessionEvent_t 				stSessionEvent;
	DxCAS_MmiSessionType_e			eSessionType;
	HUINT32 							ulSlotNumber;
	Handle_t							hSession;

	HxSTD_memcpy(&stSessionEvent, (void*)p3, sizeof(OxMgrCasSessionEvent_t));
	ulSlotNumber = stSessionEvent.ulCasSlotNum;
	eSessionType= stSessionEvent.eSessionType;
	hSession = stSessionEvent.hSession;


	LOG("[%s:%d] SlotNumber %d, SESSIONTYPE %d, SESSIONSTATUS %d\n", __FUNCTION__, __LINE__,
		ulSlotNumber, eSessionType, pstContext->ulSessionStatus);
	pstContext->stSessionInfo.hSession = hSession;

	switch(pstContext->ulSessionStatus)
	{
		case eIR_UI_SESSION_STATUS_Stopped:
			pstContext->ulSessionStatus = eIR_UI_SESSION_STATUS_Started;
			/* continue */
		case eIR_UI_SESSION_STATUS_Started:
			xmgr_cas_ir_ui_PostMessageSessionEventToOM(&(pstContext->stSessionInfo), ulSlotNumber, eMEVT_CASUI_SESSION_OPENED);
			xmgr_cas_ir_ui_MailMessage_PostMessage(pstContext);
			break;

		default:
			break;
	}

	return MESSAGE_BREAK;
}

STATIC BUS_Result_t xmgr_cas_ir_ui_MailMessage_MsgReqSessionStop(IR_UI_MailMessage_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	OxMgrCasSessionEvent_t				stSessionEvent;
	DxCAS_MmiSessionType_e			eSessionType;
	HUINT32 							ulSlotNumber;
	Handle_t							hSession;

	HxSTD_memcpy(&stSessionEvent, (void*)p3, sizeof(OxMgrCasSessionEvent_t));
	ulSlotNumber = stSessionEvent.ulCasSlotNum;
	eSessionType= stSessionEvent.eSessionType;
	hSession = stSessionEvent.hSession;


	LOG("[%s:%d] SlotNumber %d, SESSIONSTATUS %d\n", __FUNCTION__, __LINE__,
		ulSlotNumber, pstContext->ulSessionStatus);

	switch(pstContext->ulSessionStatus)
	{
		case eIR_UI_SESSION_STATUS_Started:
			pstContext->ulSessionStatus = eIR_UI_SESSION_STATUS_Stopped;
			/* continue */
		case eIR_UI_SESSION_STATUS_Stopped:
			xmgr_cas_ir_ui_PostHideMessage(ulSlotNumber, pstContext);
			break;

		default:
			break;
	}

	return MESSAGE_BREAK;
}

STATIC BUS_Result_t xmgr_cas_ir_ui_MailMessage_MsgSessionStopped(IR_UI_MailMessage_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					ulSlotNumber = 0;
	DxCAS_MmiSessionType_e		eSessionType = eDxCAS_SESSIONTYPE_NONE;

	ulSlotNumber = (HUINT32)p1;
	eSessionType = (DxCAS_MmiSessionType_e)p2;

	LOG("[%s:%d] SlotNumber %d, SESSIONTYPE %d, SESSIONSTATUS %d\n", __FUNCTION__, __LINE__,
		ulSlotNumber, eSessionType, pstContext->ulSessionStatus);

	switch(pstContext->ulSessionStatus)
	{
		case eIR_UI_SESSION_STATUS_Started:
			pstContext->ulSessionStatus = eIR_UI_SESSION_STATUS_Stopped;
			/* continue */
		case eIR_UI_SESSION_STATUS_Stopped:
			break;

		default:
			break;
	}

	return MESSAGE_BREAK;
}

STATIC BUS_Result_t xmgr_cas_ir_ui_MailMessage_MsgSessionStarted(IR_UI_MailMessage_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	OxMgrCasSessionEvent_t 				stSessionEvent;
	DxCAS_MmiSessionType_e			eSessionType;
	HUINT32 							ulSlotNumber;
	Handle_t							hSession;

	HxSTD_memcpy(&stSessionEvent, (void*)p3, sizeof(OxMgrCasSessionEvent_t));
	ulSlotNumber = stSessionEvent.ulCasSlotNum;
	eSessionType= stSessionEvent.eSessionType;
	hSession = stSessionEvent.hSession;

	LOG("[%s:%d] SlotNumber %d, SESSIONTYPE %d, SESSIONSTATUS %d\n", __FUNCTION__, __LINE__,
		ulSlotNumber, eSessionType, pstContext->ulSessionStatus);

	switch(pstContext->ulSessionStatus)
	{
		case eIR_UI_SESSION_STATUS_Stopped:
			pstContext->ulSessionStatus = eIR_UI_SESSION_STATUS_Started;
			xmgr_cas_ir_ui_PostMessageSessionEventToOM(&(pstContext->stSessionInfo), ulSlotNumber, eMEVT_CASUI_SESSION_OPENED);
			/* continue */
		case eIR_UI_SESSION_STATUS_Started:
			xmgr_cas_ir_ui_MailMessage_PostMessage(pstContext);
			break;

		default:
			break;
	}

	return MESSAGE_BREAK;
}




#define _____Local_SysMsg____________________________________________________


STATIC IR_UI_SysMsg_Context_t *xmgr_cas_ir_ui_GetSysMsgContext(void)
{
	return &s_stSysMsgContext;
}



STATIC HERROR xmgr_cas_ir_ui_SysMsg_PostBannerMessage(IR_UI_SysMsg_Context_t *pstContext)
{
	HERROR					hErr = ERR_OK;
	BUS_Result_t			hBusResult = ERR_BUS_NO_ERROR;
	OxMgrCasUiEvtBanner_t	*pstEvent = NULL;
	HINT32					slStream;
	HCHAR					*pzEncodedStr = NULL;
	HCHAR					*pzProperties = NULL;
	HUINT32 					ulSlot = pstContext->ulSlot;
	DxCAS_MmiSessionType_e 		eSessionType = pstContext->stSessionInfo.eSessionType;

	HxLOG_Info("pstContext->szCasIrMessage: %s \n", pstContext->szCasIrMessage);

	pstEvent = (OxMgrCasUiEvtBanner_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiEvtBanner_t));
	//pstEvent = xmgr_cas_ir_ui_SysMessage_GetEventData();
	if(pstEvent == NULL)
	{
		hErr = ERR_FAIL;
		goto END_FUNC;
	}

	HxLOG_Info("eSessionType: %d \n", eSessionType);
	pstEvent->stSessionInfo.eSessionType = eSessionType;
	pstEvent->stSessionInfo.ulUiHandle = (HUINT32)pstContext->stSessionInfo.ulUiHandle;
	pstEvent->stSessionInfo.hSession = (HUINT32)pstContext->stSessionInfo.hSession;

	pstEvent->stBanner.ulTimeoutSecond = 0;

	slStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
	pzEncodedStr = xmgr_cas_ir_ui_EncodeString(pstContext->szCasIrMessage);
	if(pzEncodedStr != NULL)
	{
		HLIB_RWSTREAM_Print(slStream, "[\"%s\"]", pzEncodedStr);
		HLIB_STD_MemFree(pzEncodedStr);
		pzEncodedStr = NULL;
	}

	pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);

	MWC_UTIL_DvbStrncpy(pstEvent->stBanner.stString.szString, pzProperties, MGR_CAS_STR_LENGTH_LONG-1);
	pstEvent->stBanner.stString.szString[MGR_CAS_STR_LENGTH_LONG-1] = '\0';
	pstEvent->stBanner.stString.ulStringLength = SvcCas_UtilStrLen(pstEvent->stBanner.stString.szString);
	HxLOG_Info("pstEvent->stBanner.stString.szString: %s \n", pstEvent->stBanner.stString.szString);

	HLIB_STD_MemFree(pzProperties);

	hBusResult = BUS_PostMessageToOM(NULL, eMEVT_CASUI_TYPE_BANNER_DATA, HANDLE_NULL, (HINT32)eDxCAS_GROUPID_IR, (HINT32)ulSlot, (HINT32)pstEvent);
	if(hBusResult != ERR_BUS_NO_ERROR)
	{
		hErr = ERR_FAIL;
		goto END_FUNC;
	}


END_FUNC:

	// 여기서 pstEvent를  free 하면 안됨. om_cas_mmi_GetEventData 에서 하게 된다.

	return hErr;
}



STATIC BUS_Result_t xmgr_cas_ir_ui_SysMsg_MsgCreate(IR_UI_SysMsg_Context_t *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{

	HUINT32				ulSlot = 0;//(HUINT32)p1;
	DxCAS_MmiSessionType_e		eSessionType = (DxCAS_MmiSessionType_e)p2;

	HxLOG_Print("eMEVT_BUS_CREATE p1(0x%x), p2(%d), p3(0x%x)\n", p1,p2,p3);

	HxSTD_MemSet((void *)pContext, 0x00, sizeof(IR_UI_SysMsg_Context_t));

	pContext->stSessionInfo.eSessionType = eSessionType;
	pContext->ulSessionStatus = eIR_UI_SESSION_STATUS_Stopped;
	pContext->stSessionInfo.ulUiHandle = (HUINT32)xproc_cas_IrSysMsg;
	pContext->ulSlot = ulSlot;

	HxLOG_Info("ulSlot: %d, SESSIONTYPE %d\n", pContext->ulSlot, eSessionType);

	return ERR_BUS_MESSAGE_PASS;

}


STATIC BUS_Result_t xmgr_cas_ir_ui_SysMsg_EvtChangeState(IR_UI_SysMsg_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxLOG_Info("p1: 0x%x, p2: %d, p3: %d, pstContext->ulSessionStatus: %d\n",
		p1, p2, p3, pstContext->ulSessionStatus);

	switch(pstContext->ulSessionStatus)
	{
		case eIR_UI_SESSION_STATUS_Stopped:
			pstContext->ulSessionStatus = eIR_UI_SESSION_STATUS_Started;
			/* continue */
		case eIR_UI_SESSION_STATUS_Started:
			xmgr_cas_ir_ui_SysMsg_PostBannerMessage(pstContext);
			break;

		default:
			break;
	}

	return ERR_BUS_MESSAGE_PASS;
}


STATIC BUS_Result_t xmgr_cas_ir_ui_SysMsg_MsgIrdetoUpdateUi(IR_UI_SysMsg_Context_t *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t			eBusResult = ERR_BUS_NO_ERROR;

	pContext->ulLastErrCode = p2;

#if defined(CONFIG_MW_CAS_IRDETO_PREVIEW)
	if(p1 == IREVT_CLASS_ECM_MONITOR_PREVIEW_TIME)
	{
		xmgr_cas_ir_GetPreviewTimeText(p2, pContext->szCasIrMessage);
	}
	else
#endif
	{
		xmgr_cas_ir_GetErrorText(p2, pContext->szCasIrMessage);
		xmgr_cas_iruc_ExtendErrorText(p2, pContext->szCasIrMessage);
	}

	HxLOG_Print("[%s]\n", pContext->szCasIrMessage);
	return eBusResult;
}


STATIC BUS_Result_t xmgr_cas_ir_ui_SysMsg_MsgReqSessionStart(IR_UI_SysMsg_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	OxMgrCasSessionEvent_t 				stSessionEvent;
	DxCAS_MmiSessionType_e			eSessionType;
	HUINT32 							ulSlotNumber;
	Handle_t							hSession;

	HxSTD_memcpy(&stSessionEvent, (void*)p3, sizeof(OxMgrCasSessionEvent_t));
	ulSlotNumber = stSessionEvent.ulCasSlotNum;
	eSessionType= stSessionEvent.eSessionType;
	hSession = stSessionEvent.hSession;



	LOG("[%s:%d] SlotNumber %d, SESSIONTYPE %d, SESSIONSTATUS %d\n", __FUNCTION__, __LINE__,
		ulSlotNumber, eSessionType, pstContext->ulSessionStatus);
	HxLOG_Debug("sessionType: %d, hSession: 0x%x \n", pstContext->stSessionInfo.eSessionType, hSession);
	pstContext->stSessionInfo.hSession = hSession;

	switch(pstContext->ulSessionStatus)
	{
		// TODO: 정리.
		case eIR_UI_SESSION_STATUS_Stopped:
			pstContext->ulSessionStatus = eIR_UI_SESSION_STATUS_Started;
			/* continue */
		case eIR_UI_SESSION_STATUS_Started:
			pstContext->stSessionInfo.eSessionType = eSessionType;
			xmgr_cas_ir_ui_PostMessageSessionEventToOM(&(pstContext->stSessionInfo), ulSlotNumber, eMEVT_CASUI_SESSION_OPENED);
			xmgr_cas_ir_ui_SysMsg_PostBannerMessage(pstContext);
			break;

		default:
			break;
	}

	return MESSAGE_BREAK;
}

STATIC BUS_Result_t xmgr_cas_ir_ui_SysMsg_MsgReqSessionStop(IR_UI_SysMsg_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	OxMgrCasSessionEvent_t 				stSessionEvent;
	DxCAS_MmiSessionType_e			eSessionType;
	HUINT32 							ulSlotNumber;
	Handle_t							hSession;

	HxSTD_memcpy(&stSessionEvent,(void*) p3, sizeof(OxMgrCasSessionEvent_t));
	ulSlotNumber = stSessionEvent.ulCasSlotNum;
	eSessionType= stSessionEvent.eSessionType;
	hSession = stSessionEvent.hSession;


	LOG("[%s:%d] SlotNumber %d, pstContext->eSessionType: %d, SESSIONSTATUS %d\n", __FUNCTION__, __LINE__,
		ulSlotNumber, pstContext->stSessionInfo.eSessionType, pstContext->ulSessionStatus);

	switch(pstContext->ulSessionStatus)
	{
		case eIR_UI_SESSION_STATUS_Started:
			pstContext->ulSessionStatus = eIR_UI_SESSION_STATUS_Stopped;
			/* continue */
		case eIR_UI_SESSION_STATUS_Stopped:
			xmgr_cas_ir_ui_PostHideMessage(ulSlotNumber, pstContext);
			break;
		default:
			break;
	}

	return MESSAGE_BREAK;
}

STATIC BUS_Result_t xmgr_cas_ir_ui_SysMsg_MsgSessionStopped(IR_UI_SysMsg_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					ulSlotNumber = 0;
	DxCAS_MmiSessionType_e		eSessionType = eDxCAS_SESSIONTYPE_NONE;

	ulSlotNumber = (HUINT32)p1;
	eSessionType = (DxCAS_MmiSessionType_e)p2;

	LOG("[%s:%d] SlotNumber %d, SESSIONTYPE %d, SESSIONSTATUS %d\n", __FUNCTION__, __LINE__,
		ulSlotNumber, eSessionType, pstContext->ulSessionStatus);

	switch(pstContext->ulSessionStatus)
	{
		case eIR_UI_SESSION_STATUS_Started:
			pstContext->ulSessionStatus = eIR_UI_SESSION_STATUS_Stopped;
			/* continue */
		case eIR_UI_SESSION_STATUS_Stopped:
			break;

		default:
			break;
	}

	return MESSAGE_BREAK;
}

STATIC BUS_Result_t xmgr_cas_ir_ui_SysMsg_MsgSessionStarted(IR_UI_SysMsg_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					ulSlotNumber = 0;
	DxCAS_MmiSessionType_e		eSessionType = eDxCAS_SESSIONTYPE_NONE;

	ulSlotNumber = (HUINT32)p1;
	eSessionType = (DxCAS_MmiSessionType_e)p2;

	LOG("[%s:%d] SlotNumber %d, SESSIONTYPE %d, SESSIONSTATUS %d\n", __FUNCTION__, __LINE__,
		ulSlotNumber, eSessionType, pstContext->ulSessionStatus);

	switch(pstContext->ulSessionStatus)
	{
		case eIR_UI_SESSION_STATUS_Stopped:
			pstContext->ulSessionStatus = eIR_UI_SESSION_STATUS_Started;
			xmgr_cas_ir_ui_PostMessageSessionEventToOM(&(pstContext->stSessionInfo), ulSlotNumber, eMEVT_CASUI_SESSION_OPENED);
			/* continue */
		case eIR_UI_SESSION_STATUS_Started:
			xmgr_cas_ir_ui_SysMsg_PostBannerMessage(pstContext);
			break;

		default:
			break;
	}

	return MESSAGE_BREAK;
}


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
#define _____XMGR_CAS_IR_UI_PUBLIC_FUNCTION______________________________________________________

BUS_Result_t	xproc_cas_IrSysMsg(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t			eBusResult = ERR_BUS_NO_ERROR;
	IR_UI_SysMsg_Context_t	*pContext = xmgr_cas_ir_ui_GetSysMsgContext();

	switch(message)
	{

		case eMEVT_BUS_CREATE :
			HxLOG_Info("receive eMEVT_BUS_CREATE \n");
			BUS_MGR_SetMgrName(BUS_MGR_GetThis(), "xproc_cas_IrSysMsg");
			eBusResult = xmgr_cas_ir_ui_SysMsg_MsgCreate(pContext, message, hAction, p1, p2, p3);
			break;

		case eMEVT_BUS_READY_SHUTDOWN:
			HxLOG_Info("receive eMEVT_BUS_READY_SHUTDOWN \n");
			return MESSAGE_PASS;

		case eMEVT_BUS_DESTROY:
			HxLOG_Info("receive eMEVT_BUS_DESTROY \n");
			xmgr_cas_ir_ui_PostMessageSessionEventToOM(&(pContext->stSessionInfo), 0, eMEVT_CASUI_REQ_CLOSE_SESSION);
			break;


		case eMEVT_BUS_IRDETO_UPDATE_UI:
			HxLOG_Info("receive eMEVT_BUS_IRDETO_UPDATE_UI \n");
			eBusResult = xmgr_cas_ir_ui_SysMsg_MsgIrdetoUpdateUi(pContext, message, hAction, p1, p2, p3);
			eBusResult = xmgr_cas_ir_ui_SysMsg_EvtChangeState(pContext, message, hAction, p1, p2, p3);
			break;

		case eMEVT_BUS_IRDETO_HIDE_UI:
			HxLOG_Info("receive eMEVT_BUS_IRDETO_HIDE_UI \n");
			pContext->ulLastErrCode = IRUC_STATUS_MSG_NONE;
			eBusResult = xmgr_cas_ir_ui_PostHideMessage(pContext->ulSlot, pContext);
			break;

		case eOEVT_CAS_CASUI_APP_OPEN_SESSION:
			HxLOG_Info("receive eOEVT_CAS_CASUI_APP_OPEN_SESSION \n");
			if(pContext != NULL)
			{
				eBusResult = xmgr_cas_ir_ui_SysMsg_MsgReqSessionStart(pContext, message, hAction, p1, p2, p3);
			}
			break;

		case eOEVT_CAS_CASUI_APP_CLOSE_SESSION:
			HxLOG_Info("receive eOEVT_CAS_CASUI_APP_CLOSE_SESSION \n");
			if(pContext != NULL)
			{
				eBusResult = xmgr_cas_ir_ui_SysMsg_MsgReqSessionStop(pContext, message, hAction, p1, p2, p3);
				BUS_MGR_Destroy (0);
			}
			break;
		case eOEVT_CAS_CASUI_APP_USERINPUTDATA:
			HxLOG_Info("receive eOEVT_CAS_CASUI_APP_USERINPUTDATA \n");
			eBusResult = ERR_BUS_MESSAGE_PASS;
			break;

		default:
			break;
	}

	if(eBusResult != ERR_BUS_NO_ERROR)
	{
		return eBusResult;
	}

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}




BUS_Result_t	xproc_cas_IrMailIcon(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t					eBusResult = ERR_BUS_NO_ERROR;
	IR_UI_MailIcon_Context_t			*pContext = xmgr_cas_ir_ui_MailIcon_GetLocalContents();

	switch(message)
	{
		case eMEVT_BUS_CREATE:
			BUS_MGR_SetMgrName(BUS_MGR_GetThis(), "xproc_cas_IrMailIcon");
			HxLOG_Print("xproc_cas_IrMailIcon(), p1(icon type):0x%x, p3(session type): %d\n", p1, p3);
			xmgr_cas_ir_ui_MailIcon_MsgGwmCreate(pContext, message, hAction, p1, p2, p3);
			break;

		case eMEVT_BUS_DESTROY :
			HxLOG_Info("receive eMEVT_BUS_DESTROY \n");
			xmgr_cas_ir_ui_MailIcon_MsgGwmDestroy(pContext);
			xmgr_cas_ir_ui_PostMessageSessionEventToOM(&(pContext->stSessionInfo), 0, eMEVT_CASUI_REQ_CLOSE_SESSION);
			break;

		case eMEVT_BUS_IRDETO_UPDATE_UI :
			HxLOG_Info("receive eMEVT_BUS_IRDETO_UPDATE_UI \n");
			eBusResult = xmgr_cas_ir_ui_MailIcon_MsgIrdetoUpdateUi(pContext, message, hAction, p1, p2, p3);
			xmgr_cas_ir_ui_MailIcon_EvtChangeState(pContext, message, hAction, p1, p2, p3);
			break;

		case eOEVT_CAS_CASUI_APP_OPEN_SESSION:
			HxLOG_Info("receive eOEVT_CAS_CASUI_APP_OPEN_SESSION \n");
			if(pContext != NULL)
			{
				eBusResult = xmgr_cas_ir_ui_MailIcon_MsgReqSessionStart(pContext, message, hAction, p1, p2, p3);
			}
			break;

		case eOEVT_CAS_CASUI_APP_CLOSE_SESSION:
			HxLOG_Info("receive eOEVT_CAS_CASUI_APP_CLOSE_SESSION \n");
			if(pContext != NULL)
			{
				eBusResult = xmgr_cas_ir_ui_MailIcon_MsgReqSessionStop(pContext, message, hAction, p1, p2, p3);
				BUS_MGR_Destroy (0);
			}
			break;
		case eOEVT_CAS_CASUI_APP_USERINPUTDATA:
			HxLOG_Info("receive eOEVT_CAS_CASUI_APP_USERINPUTDATA \n");
			eBusResult = ERR_BUS_MESSAGE_PASS;
			break;

		default:
			break;
	}

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}



BUS_Result_t	xproc_cas_IrMailMessage(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t					eBusResult = ERR_BUS_NO_ERROR;
	IR_UI_MailMessage_Context_t		*pContext = xmgr_cas_ir_ui_MailMessage_GetLocalContents();
	HERROR						hErr = ERR_FAIL;

	switch(message)
	{
		case eMEVT_BUS_CREATE:
			BUS_MGR_SetMgrName(BUS_MGR_GetThis(), "xproc_cas_IrMailMessage");
			HxLOG_Print("xproc_cas_IrMailMessage() + mailIndex (0x%x)\n", p2);
			xmgr_cas_ir_ui_MailMessage_MsgGwmCreate(pContext, message, hAction, p1, p2, p3);
			break;

		case eMEVT_BUS_DESTROY :
			HxLOG_Info("receive eMEVT_BUS_DESTROY \n");
			xmgr_cas_ir_ui_MailMessage_MsgGwmDestroy(pContext);
			xmgr_cas_ir_ui_PostMessageSessionEventToOM(&(pContext->stSessionInfo), 0, eMEVT_CASUI_REQ_CLOSE_SESSION);
			break;

		case eMEVT_BUS_IRDETO_UPDATE_UI :
			HxLOG_Info("receive eMEVT_BUS_IRDETO_UPDATE_UI \n");
			hErr = xmgr_cas_ir_ui_MailMessage_MsgIrdetoUpdateUi(pContext, message, hAction, p1, p2, p3);
			if(hErr == ERR_OK)
			{
				xmgr_cas_ir_ui_MailMessage_EvtChangeState(pContext, message, hAction, p1, p2, p3);
			}
			break;

		case eOEVT_CAS_CASUI_APP_OPEN_SESSION:
			HxLOG_Info("receive eOEVT_CAS_CASUI_APP_OPEN_SESSION \n");
			if(pContext != NULL)
			{
				eBusResult = xmgr_cas_ir_ui_MailMessage_MsgReqSessionStart(pContext, message, hAction, p1, p2, p3);
			}
			break;

		case eOEVT_CAS_CASUI_APP_CLOSE_SESSION:
			HxLOG_Info("receive eOEVT_CAS_CASUI_APP_CLOSE_SESSION \n");
			if(pContext != NULL)
			{
				eBusResult = xmgr_cas_ir_ui_MailMessage_MsgReqSessionStop(pContext, message, hAction, p1, p2, p3);
				BUS_MGR_Destroy(0);
			}
			break;
		case eOEVT_CAS_CASUI_APP_USERINPUTDATA:
			HxLOG_Info("receive eOEVT_CAS_CASUI_APP_USERINPUTDATA \n");
			eBusResult = ERR_BUS_MESSAGE_PASS;
			break;

		default:
			break;
	}

	if(eBusResult != ERR_BUS_NO_ERROR)
	{
		return eBusResult;
	}

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);


}


BUS_Result_t	xproc_cas_IrFlexFlash(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t					eBusResult = ERR_BUS_NO_ERROR;
	HUINT16 						ret;
	static HUINT16 				usflexflash=0;

	switch(message)
	{
		case eMEVT_BUS_CREATE:
			BUS_MGR_SetMgrName(BUS_MGR_GetThis(), "xproc_cas_IrFlexFlash");
			HxLOG_Print("xproc_cas_IrFlexFlash() + mailIndex (0x%x)\n", p2);
			usflexflash = (HUINT16)p2;
			BUS_SetTimer(IRUC_FLEXFLASH_TIMEOUT_ID, IRUC_FLEXFLASH_TIMEOUT);
			break;

		case eMEVT_BUS_TIMER:
			if(p1 == IRUC_FLEXFLASH_TIMEOUT_ID)
			{
				if(usflexflash ==1)
				{
					ret = VK_SYSTEM_Command("rm /opt/share/IrUC_PSFile* ");
					if(ret == 0) HxLOG_Print("IrUC_PSFile* delete OK \n");
					else  HxLOG_Print("IrUC_PSFile* delete Fail \n");

					ret = VK_SYSTEM_Command("cp /var/lib/humaxtv/cas/irUC/* /opt/share/ ");
					if(ret == 0) HxLOG_Print("corefile copy to  RootFs is OK \n ");
					else HxLOG_Print("corefile copy to  RootFs is Fail \n");

					ret = VK_SYSTEM_Command("chmod 777 /opt/share/IrUC_PSFile* ");

					if(ret == 0) HxLOG_Print("Corefile Permission change OK \n ");
					else HxLOG_Print("Corefile Permission change FAIL \n");
					usflexflash = 0;
				}
				BUS_KillTimer(IRUC_FLEXFLASH_TIMEOUT_ID);
				BUS_MGR_Destroy(0);
			}

			break;

		case eMEVT_BUS_DESTROY :
			HxLOG_Info("receive eMEVT_BUS_DESTROY \n");
			return MESSAGE_BREAK;

		default:
			break;
	}

	if(eBusResult != ERR_BUS_NO_ERROR)
	{
		return eBusResult;
	}

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}



#define	____XMGR_CAS_IR_CORE_UI_IMPLEMENTS_____



/**
 * CAS UI 별로 ulProperty 및 parameter를 변경해서 사용할 경우가 있으므로,
 * 아래 함수를 통해 UI를 Create한다.
 */
BUS_Result_t	xmgr_cas_IrCreateUI	(HUINT8 *szName, HUINT32 ulProperty, XmgrCas_IrUiType_e eUiType, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Callback_t	pfnProc;

	pfnProc	= xmgr_cas_IrGetUiProc(eUiType);
	if (pfnProc == NULL)
	{
		return ERR_BUS_INITIAL_FAILED;
	}

	if (BUS_MGR_Get(pfnProc) != NULL)
	{
		HxLOG_Print("BUS_MGR_Get(pfnProc) != NULL \n");
		return ERR_BUS_NO_ERROR;
//		BUS_MGR_Destroy(pfnProc);
	}

	HxLOG_Print("create pfonProc \n");

	return BUS_MGR_Create(szName, ulProperty, pfnProc, hAction, p1, p2, p3);
}

BUS_Callback_t	xmgr_cas_IrGetUiProc 	(XmgrCas_IrUiType_e eUiType)
{
	BUS_Callback_t	pfnProc;

	switch (eUiType)
	{
		case eCasIr_UI_ErrorMessage:
			pfnProc = xproc_cas_IrSysMsg;
			break;

		case eCasIr_UI_Menu:
			pfnProc = xproc_cas_IrUC;
			break;

		case eCasIr_UI_MailIcon:
			pfnProc = xproc_cas_IrMailIcon;
			break;

		case eCasIr_UI_MailMessage:
			pfnProc = xproc_cas_IrMailMessage;
			break;

		case eCasIr_FlexFlash:
			pfnProc = xproc_cas_IrFlexFlash;
			break;

		default:
			return NULL;
	}
	return pfnProc;
}

IREVT_CLASSe	xmgr_cas_IrMailMessageGetCurrentEvtClass	(void)
{
	IR_UI_MailMessage_Context_t		*pContext = xmgr_cas_ir_ui_MailMessage_GetLocalContents();

	return pContext->irEvtClass;
}

