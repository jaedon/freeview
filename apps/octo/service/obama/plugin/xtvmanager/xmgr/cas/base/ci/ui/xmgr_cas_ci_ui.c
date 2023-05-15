/**************************************************************
 *	@file		xmgr_cas_ci_ui.c
 *	common interface subcas system message banner module
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2011/12/04
 *	@author			humax
 *	@brief
 **************************************************************/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <svc_cas.h>


#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_live.h>
#include <mgr_cas.h>
#include <xmgr_cas_res_str.h>
#include "../local_include/_xmgr_cas_ci.h"

//#include <ap_ciplus_upgrade.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#if defined(CONFIG_DEBUG)
#define	LOG					HxLOG_Print
#define INFO_LOG			HxLOG_Info
#define	ERR_LOG				HxLOG_Error
#define MSG_LOG				HxLOG_Message
#define	FuncENTER			HxLOG_Info("(+)Enter!!\n");
#define	FuncLEAVE(X)		HxLOG_Info("(-)Leave -- (0x%X)\n",X);
#define	DELAY_LOG(X)		OCTO_NULL_PRINT
#define PAUSE				OCTO_NULL_PRINT
#else /* defined(CONFIG_DEBUG) */
#define	LOG					OCTO_NULL_PRINT
#define INFO_LOG			OCTO_NULL_PRINT
#define	ERR_LOG				OCTO_NULL_PRINT
#define MSG_LOG				OCTO_NULL_PRINT
#define	FuncENTER			OCTO_NULL_PRINT
#define	FuncLEAVE(X)		OCTO_NULL_PRINT
#define	DELAY_LOG(X)		OCTO_NULL_PRINT
#define PAUSE				OCTO_NULL_PRINT
#endif /* defined(CONFIG_DEBUG) */

//#define CI_UI_MSG_TIMEOUT_10SEC		(DLG_TIMEOUT_10SEC)
//#define CI_UI_MSG_TIMER_ID			(900)

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eCI_UI_SESSION_STATUS_Stopped,
	eCI_UI_SESSION_STATUS_StartReady,
	eCI_UI_SESSION_STATUS_Started,

	eCI_UI_SESSION_STATUS_Max
} XmgrCas_CiUiSessionStatus_e;

typedef struct
{
	DxCAS_MmiSessionType_e	eSessionType;
	HUINT32					ulSessionStatus;
	HUINT32					ulUiHandle;
	HUINT32					ulSlot;
	Handle_t				hSession;
} XmgrCas_CiUiAlarmContext_t;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
STATIC XmgrCas_CiUiAlarmContext_t	s_astCasCiAlramContext[XMGR_CAS_CI_SLOT_MAX];

/*******************************************************************/
/********************      Local Functions      ********************/
/*******************************************************************/
#define ____LOCAL_FUNCTION_________________________________________________________________________________________
INLINE XmgrCas_CiUiAlarmContext_t *xmgr_cas_CiUiGetAlramContext(HUINT32 ulSlotNumber)
{
	#if defined(CONFIG_DEBUG)
	HxLOG_Assert(ulSlotNumber < XMGR_CAS_CI_SLOT_MAX);
	#endif /* defined(CONFIG_DEBUG) */

	return &(s_astCasCiAlramContext[ulSlotNumber]);
}

STATIC HCHAR *xmgr_cas_CiUiEncodeString(HCHAR *pszString)
{
	HUINT32		Len = 0;
	HCHAR		*buf = NULL;

	Len = HLIB_STD_URLStrLen(pszString) + 1;
    buf = HLIB_STD_MemCalloc(Len+1);
    HLIB_STD_URLEncode(buf,pszString,Len);

    return buf;
}

STATIC HERROR	xmgr_cas_CiUiPostMessageSessionEventToOM(XmgrCas_CiUiAlarmContext_t *pstContext, HUINT32 ulSlotNumber, HINT32 eEvent)
{
	HERROR								hError = ERR_OK;
	BUS_Result_t						hResult = ERR_BUS_NO_ERROR;
	OxMgrCasUiEvtSession_t				*pstEvent = NULL;

	pstEvent = (OxMgrCasUiEvtSession_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiEvtSession_t));

	pstEvent->eSessionType = pstContext->eSessionType;
	pstEvent->ulUiHandle = pstContext->ulUiHandle;
	pstEvent->hSession = pstContext->hSession;

	hResult = BUS_PostMessageToOM(NULL, eEvent, HANDLE_NULL, eDxCAS_GROUPID_CAM, ulSlotNumber, (HINT32)pstEvent);
	if(hResult != ERR_BUS_NO_ERROR)
	{
		hError = ERR_FAIL;
	}

	return hError;
}

STATIC HERROR xmgr_cas_CiUiPostBannerMessage(XmgrCas_CiUiAlarmContext_t *pstContext) //HUINT32 ulSlot, DxCAS_MmiSessionType_e eSessionType, HUINT8 *pszMessage)
{
	HERROR					hErr = ERR_OK;
	BUS_Result_t			hBusResult = ERR_BUS_NO_ERROR;
	OxMgrCasUiEvtBanner_t	*pstEvent = NULL;
	HINT32					slStream;
	HCHAR					*pzProperties = NULL;

	pstEvent = (OxMgrCasUiEvtBanner_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiEvtBanner_t));

	pstEvent->stSessionInfo.eSessionType 	= pstContext->eSessionType;
	pstEvent->stSessionInfo.hSession		= pstContext->hSession;
	pstEvent->stSessionInfo.ulUiHandle		= pstContext->ulUiHandle;

	pstEvent->stBanner.ulTimeoutSecond = 10; /* 10 sec */

	slStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
	HLIB_RWSTREAM_Print(slStream, "[{\"id\":\"LOC_CI_INITCIMODULE_ID\"}]");

	pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);

	MWC_UTIL_DvbStrncpy(pstEvent->stBanner.stString.szString, pzProperties, MGR_CAS_STR_LENGTH_LONG-1);
	pstEvent->stBanner.stString.szString[MGR_CAS_STR_LENGTH_LONG-1] = '\0';
	pstEvent->stBanner.stString.ulStringLength = SvcCas_UtilStrLen(pstEvent->stBanner.stString.szString);
	HLIB_STD_MemFree(pzProperties);

	hBusResult = BUS_PostMessageToOM(NULL, eMEVT_CASUI_TYPE_BANNER_DATA, HANDLE_NULL, (HINT32)eDxCAS_GROUPID_CAM, (HINT32)pstContext->ulSlot, (HINT32)pstEvent);
	if(hBusResult != ERR_BUS_NO_ERROR)
	{
		hErr = ERR_FAIL;
	}

	return hErr;
}

STATIC HERROR xmgr_cas_CiUiPostBannerHideMessage(XmgrCas_CiUiAlarmContext_t *pstContext) //, HUINT32 ulSlot, DxCAS_MmiSessionType_e eSessionType)
{
	HERROR					hErr = ERR_OK;
	BUS_Result_t			hBusResult = ERR_BUS_NO_ERROR;
	OxMgrCasUiEvtBanner_t	*pstEvent = NULL;

	pstEvent = (OxMgrCasUiEvtBanner_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiEvtBanner_t));

	pstEvent->stSessionInfo.eSessionType = pstContext->eSessionType;
	pstEvent->stSessionInfo.hSession = pstContext->hSession;
	pstEvent->stSessionInfo.ulUiHandle = pstContext->ulUiHandle;
	pstEvent->stBanner.stString.ulStringLength = 0;

	/* Display할 String length를 0으로 할 경우 Banner 를 hide시킨다. */
	pstEvent->stBanner.stString.ulStringLength = 0;

	hBusResult = BUS_PostMessageToOM(NULL, eMEVT_CASUI_TYPE_BANNER_DATA, HANDLE_NULL, (HINT32)eDxCAS_GROUPID_CAM, (HINT32)pstContext->ulSlot, (HINT32)pstEvent);
	if(hBusResult != ERR_BUS_NO_ERROR)
	{
		hErr = ERR_FAIL;
	}

	return hErr;
}

STATIC BUS_Result_t xmgr_cas_CiUiEvtChangeState(XmgrCas_CiUiAlarmContext_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32						ulSlot = 0;
	XmgrCas_CiCamStatus_e		eCamStatus = eCiCas_CamRemoved;

	ulSlot = (HUINT32)p1;
	eCamStatus = (XmgrCas_CiCamStatus_e)p2;

	LOG("[%s:%d] SlotNumber %d, CAMSTATUS %d, SESSIONSTATUS %d\n", __FUNCTION__, __LINE__,
		ulSlot, eCamStatus, pstContext->ulSessionStatus);

	if(pstContext->ulSessionStatus != eCI_UI_SESSION_STATUS_Started)
	{
		HxLOG_Critical("Session Status is not Started!!! Check it!!\n");
	}

	switch(eCamStatus)
	{
		case eCiCas_CamRemoved:
			xmgr_cas_CiUiPostBannerHideMessage(pstContext);
			break;

		case eCiCas_CamInserted:
			xmgr_cas_CiUiPostBannerMessage(pstContext);
			break;

		case eCiCas_CamInitialized:
			xmgr_cas_CiUiPostBannerHideMessage(pstContext);
			break;

		default:
			break;
	}

	return ERR_BUS_MESSAGE_PASS;
}

#define _____LOCAL_EVENT_PROC______________________________________________________
STATIC BUS_Result_t xmgr_cas_CiUiMsgCreate(XmgrCas_CiUiAlarmContext_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32						ulSlot = (HUINT32)p1;
	DxCAS_MmiSessionType_e		eSessionType = (DxCAS_MmiSessionType_e)p2;

	HxSTD_MemSet((void *)pstContext, 0x00, sizeof(XmgrCas_CiUiAlarmContext_t));

	pstContext->eSessionType = eSessionType;
	pstContext->ulSessionStatus = eCI_UI_SESSION_STATUS_Stopped;
	pstContext->ulUiHandle = (HUINT32)xproc_cas_CiUiSysMsg;
	pstContext->ulSlot = ulSlot;
	pstContext->hSession = HANDLE_NULL;

	LOG("SlotNumber %d, SESSIONTYPE %d\n", ulSlot, eSessionType);

	return ERR_BUS_MESSAGE_PASS;
}
STATIC BUS_Result_t xmgr_cas_CiUiMsgOpenSession(XmgrCas_CiUiAlarmContext_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	OxMgrCasSessionEvent_t		*pstSession = (OxMgrCasSessionEvent_t *)p3;

	pstContext->ulSessionStatus = eCI_UI_SESSION_STATUS_Started;
	pstContext->hSession = pstSession->hSession;

	xmgr_cas_CiUiPostMessageSessionEventToOM(pstContext, pstSession->ulCasSlotNum, eMEVT_CASUI_SESSION_OPENED);

	return MESSAGE_BREAK;
}

STATIC BUS_Result_t xmgr_cas_CiUiMsgCloseSession(XmgrCas_CiUiAlarmContext_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	DxCAS_MmiSessionType_e		eSessionType = eDxCAS_SESSIONTYPE_NONE;

	pstContext->ulSessionStatus = eCI_UI_SESSION_STATUS_Stopped;

	xmgr_cas_CiUiPostBannerHideMessage(pstContext);

	return MESSAGE_BREAK;
}


#define _____XMGR_CAS_CI_UI_PUBLIC_FUNCTION______________________________________________________
/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
BUS_Result_t	xproc_cas_CiUiSysMsg(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 ulMaxSlotNumber = 0, i = 0, ulSlot = 0;
	BUS_Result_t eBusRes = ERR_BUS_NO_ERROR;
	XmgrCas_CiUiAlarmContext_t *pstContext = NULL;
	OxMgrCasSessionEvent_t *pstSessionEvent = NULL;

	switch(message)
	{
		case eMEVT_BUS_CREATE :
			HxLOG_Info("eMEVT_BUS_CREATE...\n");
			BUS_MGR_SetMgrName(BUS_MGR_GetThis(), (HINT8 *)__FUNCTION__);

			SVC_CAS_GetNumOfCISlot(&ulMaxSlotNumber);
			for(i = 0;i < ulMaxSlotNumber;i++)
			{
				pstContext = xmgr_cas_CiUiGetAlramContext(i);
				eBusRes = xmgr_cas_CiUiMsgCreate(pstContext, message, hAction, (HINT32)i, p2, p3);
			}
			break;

		case eMEVT_CAS_FAIL:
		case eMEVT_CAS_CHECK:
		case eMEVT_LIVE_NOTIFY_SVC_CHANGED:
		case eMEVT_CAS_OK:
			LOG("[%s:%d] MSG_CAS_ EVENT TRIGGER(0x%x)\n", __FUNCTION__, __LINE__, message);
			break;

		case eMEVT_CAS_CAM_REMOVED:
		case eMEVT_CAS_CAM_INSERTED:
		case eMEVT_CAS_CAM_INITIALIZED:
			HxLOG_Info("CAM Status Changed...\n");
			ulSlot = (HUINT32)p1;
			pstContext = xmgr_cas_CiUiGetAlramContext(ulSlot);
			if(pstContext != NULL)
			{
				eBusRes = xmgr_cas_CiUiEvtChangeState(pstContext, message, hAction, p1, p2, p3);
			}
			break;

		case eOEVT_CAS_CASUI_APP_OPEN_SESSION:
			HxLOG_Info("eOEVT_CAS_CASUI_APP_OPEN_SESSION...\n");
			pstSessionEvent = (OxMgrCasSessionEvent_t *)p3;
			HxLOG_Info("Slot Num: [%d]...\n", pstSessionEvent->ulCasSlotNum);
			pstContext = xmgr_cas_CiUiGetAlramContext(pstSessionEvent->ulCasSlotNum);
			if(pstContext != NULL)
			{
				eBusRes = xmgr_cas_CiUiMsgOpenSession(pstContext, message, hAction, p1, p2, p3);
			}
			break;

		case eOEVT_CAS_CASUI_APP_CLOSE_SESSION:
			HxLOG_Info("eOEVT_CAS_CASUI_APP_CLOSE_SESSION...\n");
			pstSessionEvent = (OxMgrCasSessionEvent_t *)p3;
			pstContext = xmgr_cas_CiUiGetAlramContext(pstSessionEvent->ulCasSlotNum);
			if(pstContext != NULL)
			{
				eBusRes = xmgr_cas_CiUiMsgCloseSession(pstContext, message, hAction, p1, p2, p3);
			}
			break;

		case eOEVT_CAS_CASUI_APP_USERINPUTDATA:
			LOG("[%s:%d] MSG_APPMGR_CASUI_ EVENT TRIGGER(0x%x)\n", __FUNCTION__, __LINE__, message);
			eBusRes = ERR_BUS_MESSAGE_PASS;
			break;

		case eMEVT_BUS_DESTROY :
			break;

		case eMEVT_BUS_READY_SHUTDOWN:
			return ERR_BUS_MESSAGE_PASS;

	default:
		break;
	}

	if(eBusRes != ERR_BUS_NO_ERROR)
	{
		eBusRes = BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
	}

	return eBusRes;
}

/* EOF */

