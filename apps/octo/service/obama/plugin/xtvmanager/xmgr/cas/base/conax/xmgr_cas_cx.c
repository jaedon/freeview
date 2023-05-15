/*******************************************************************
	File Description
*******************************************************************/
/*++
    @file        xmgr_cas_cx.c
    @brief       tvmanager plugin for CONAX

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2011 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
--*/
/*******************************************************************/
#define ________XMGR_CAS_CX_Private_Include___________________________________
/*******************************************************************/
#include <octo_common.h>
#include <bus.h>

#include <mgr_live.h>
#include <mgr_cas.h>
#include <mgr_schedule.h>
#include <mgr_emmupdate.h>

#include <conax.h>
#include <conax_ctrl.h>

#include "local_include/_xmgr_cas_cx.h"
#include "../../local_include/_xmgr_cas.h"

/*******************************************************************/
#define ________XMGR_CAS_CX_Golbal_Value______________________________________
/*******************************************************************/
/*******************************************************************/
#define ________XMGR_CAS_CX_Private_Define____________________________________
/*******************************************************************/
/*
typedef enum
{
	eCxCasUi_FnMenu = 0,
	eCxCasUi_FnSystemMessage,

	eCxCasUi_FnMax
} XmgrCas_CxCasUiFn_e;
*/
/*
typedef enum
{
	eCiCasAct_FnCamTune = 0,
	eCiCasAct_FnCamUpgrade,
//#if defined(CONFIG_MW_CI_PLUS_V_1_3)
	eCiCasAct_FnOperatorProfile,
//#endif

	eCiCasAct_FnMax
} XmgrCas_CiCasActFn_e;
*/
/*
typedef enum
{
	eCiCasUi_MmiNone,
	eCiCasUi_MmiForMenu,
	eCiCasUi_MmiForZap,

	eCiCasUi_MmiTypeMax
} XmgrCas_CiCasUiMmiType_e;
*/

/*
typedef struct
{
	BUS_Callback_t			pfnActCallback[eCiCasAct_FnMax];
} XmgrCas_CiCasAct_t;
*/

typedef struct
{
	HUINT32				ulSessionHandle;	// APPKIT에서 관리되고 있는 Handle
	BUS_Callback_t		pfnUiCallback;
} XmgrCas_CxUiSession_t;

typedef struct
{
	HBOOL					bStandby;
	Handle_t				hAction;
	HUINT32					ulSlotNum;
	XmgrCas_CxUiSession_t	stUiSession[eCxCasUiType_Max];
} XmgrCas_CxContext_t;


/*******************************************************************/
#define ________XMGR_CAS_CX_Private_Static_Value______________________________
/*******************************************************************/
STATIC XmgrCas_CxContext_t			s_stCxContext;

/*******************************************************************/
#define ________XMGR_CAS_CX_Global_Value______________________________________
/*******************************************************************/
/*******************************************************************/
#define ________XMGR_CAS_CX_Private_Static_Prototype__________________________
/*******************************************************************/
/*******************************************************************/
#define ________XMGR_CAS_CX_Private_Static_Prototype_Body_____________________
/*******************************************************************/
/*******************************************************************/
#define ________XMGR_CAS_CX_Public_Functions_Body_____________________________
/*******************************************************************/
extern HERROR	OM_CAS_MMI_RegisterCasUiProc(DxCAS_GroupId_e eCasGrpId, BUS_Callback_t fnCasUiProc);

#define _____LOCAL_UTIL_FUNCTIONS______________________________________________________
STATIC XmgrCas_CxContext_t *_xmgr_cas_CxGetContext(void)
{
	return &s_stCxContext;
}

STATIC BUS_Result_t	_xmgr_cas_CxMsgBusCreate(XmgrCas_CxContext_t *pContext)
{
	HxSTD_MemSet(pContext, 0x00, sizeof(XmgrCas_CxContext_t));

	pContext->stUiSession[eCxCasUiType_CasMenu].pfnUiCallback = xproc_cas_CxUiMenu;
	pContext->stUiSession[eCxCasUiType_SystemMessage].pfnUiCallback = xproc_cas_CxUiPopUp;
	pContext->stUiSession[eCxCasUiType_MailIcon].pfnUiCallback = NULL;
	pContext->stUiSession[eCxCasUiType_MailMessage].pfnUiCallback = NULL;
	pContext->stUiSession[eCxCasUiType_FingerPrint].pfnUiCallback = NULL;
	pContext->stUiSession[eCxCasUiType_PinDialog].pfnUiCallback = NULL;
	pContext->stUiSession[eCxCasUiType_PpvDialog].pfnUiCallback = NULL;
	pContext->stUiSession[eCxCasUiType_PurchaseList].pfnUiCallback = NULL;
	pContext->stUiSession[eCxCasUiType_CheckMessage].pfnUiCallback = NULL;

	if(OM_CAS_MMI_RegisterCasUiProc(eDxCAS_GROUPID_CX, xproc_cas_CxMain) != ERR_OK)
	{
		HxLOG_Error("OM_CAS_MMI_RegisterCasUiProc() error\n");
	}

	return MESSAGE_BREAK;
}



HERROR xmgr_cas_CxInitialize(void)
{
	return ERR_OK;
}


HERROR xmgr_cas_CxDefaultSetting(void)
{
	CXAPI_NVM_ClearConaxUserData();
	return ERR_OK;
}

/*
STATIC BUS_Result_t	_xmgr_cas_CxMsgOpenSession(XmgrCas_CxContext_t *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	OxMgrCasSessionEvent_t	*pstSession = (OxMgrCasSessionEvent_t *)p3;
	XmgrCas_CxUI_e			eCxUiSession;

	if(pstSession->hSession == HANDLE_NULL)
	{
		HxLOG_Error("Invalid Session Handle\n");
		return ERR_BUS_MESSAGE_BREAK;
	}

	eCxUiSession = _xmgr_cas_Na_GetNaUiSessionType(pContext, pstSession->eSessionType, pstSession->ulUiHandle);
	if(eNaUiSession == eNaCasUiType_Max)
	{
		if(BUS_MGR_Get((BUS_Callback_t)pstSession->ulUiHandle) != NULL)
		{
			return BUS_SendMessage((BUS_Callback_t)pstSession->ulUiHandle, message, hAction, p1, p2, p3);
		}

		HxLOG_Error("Invalid NAGRA UI Session\n");
		return ERR_BUS_MESSAGE_PASS;
	}

	if (BUS_MGR_Get(pContext->stUiSession[eNaUiSession].pfnUiCallback) == NULL)
	{
		// 아래를 check 하는 이유는 EXTENSION TYPE의 경우 OBAMA에서 REUQEST 하기 때문에 PROC이 무조건 존재해야 한다.
		// 빠르게 동작되어야 하는 UI의 경우는 발생할 수 있다.
		if(pstSession->eSessionType == eDxCAS_SESSIONTYPE_EXTENSION)
			HxLOG_Warning("Invalid sequence for CAS UI\n");

		HxLOG_Print("Create UI proc (%d)\n", eNaUiSession);
		BUS_MGR_Create(NULL, APP_CAS_PRIORITY, pContext->stUiSession[eNaUiSession].pfnUiCallback, hAction, p1, p2, p3);
	}

	BUS_SendMessage(pContext->stUiSession[eNaUiSession].pfnUiCallback, message, hAction, p1, p2, p3);

	pContext->stUiSession[eNaUiSession].ulSessionHandle = pstSession->hSession;

	NA_CAS_PARAM_WARNING_REMOVE(pContext, message, hAction, p1, p2, p3);
	return ERR_BUS_MESSAGE_BREAK;
}
*/

STATIC XmgrCas_CxUiType_e _xmgr_cas_CxGetUiSessionType(XmgrCas_CxContext_t *pContext, DxCAS_MmiSessionType_e eUiSession, HUINT32 ulUiHandle)
{
	XmgrCas_CxUiType_e eCxUiType = eCxCasUiType_Max;
	XmgrCas_CxUiType_e eIndex;

	switch(eUiSession)
	{
		case eDxCAS_SESSIONTYPE_BANNER:
			eCxUiType = eCxCasUiType_SystemMessage;
			break;

		case eDxCAS_SESSIONTYPE_MENU:
			eCxUiType = eCxCasUiType_CasMenu;
			break;

		case eDxCAS_SESSIONTYPE_EXTENSION:
			for(eIndex = 0 ; eIndex < eCxCasUiType_Max ; eIndex++)
			{
				if(pContext->stUiSession[eIndex].pfnUiCallback == (BUS_Callback_t)ulUiHandle)
				{
					eCxUiType = eIndex;
					break;
				}
			}
			break;

		default:
			break;
	}

	return eCxUiType;
}


STATIC BUS_Result_t		_xmgr_cas_CxMsgOpenSession(XmgrCas_CxContext_t *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	OxMgrCasSessionEvent_t	*pstSession = (OxMgrCasSessionEvent_t *)p3;
	XmgrCas_CxUiType_e		eCxUiType;

	if(pstSession->hSession == HANDLE_NULL)
	{
		HxLOG_Error("Invalid Session Handle\n");
		return ERR_BUS_MESSAGE_BREAK;
	}

	eCxUiType = _xmgr_cas_CxGetUiSessionType(pContext, pstSession->eSessionType, pstSession->ulUiHandle);
	if(eCxUiType == eCxCasUiType_Max)
	{
		if(BUS_MGR_Get((BUS_Callback_t)pstSession->ulUiHandle) != NULL)
		{
			return BUS_SendMessage((BUS_Callback_t)pstSession->ulUiHandle, message, hAction, p1, p2, p3);
		}

		HxLOG_Error("Invalid CONAX UI Session\n");
		return ERR_BUS_MESSAGE_PASS;
	}

	if (BUS_MGR_Get(pContext->stUiSession[eCxUiType].pfnUiCallback) == NULL)
	{
		// 아래를 check 하는 이유는 EXTENSION TYPE의 경우 OBAMA에서 REUQEST 하기 때문에 PROC이 무조건 존재해야 한다.
		// 빠르게 동작되어야 하는 UI의 경우는 발생할 수 있다.
		if(pstSession->eSessionType == eDxCAS_SESSIONTYPE_EXTENSION)
			HxLOG_Warning("Invalid sequence for CAS UI\n");

		HxLOG_Print("Create UI proc (%d)\n", eCxUiType);
		BUS_MGR_Create(NULL, APP_CAS_PRIORITY, pContext->stUiSession[eCxUiType].pfnUiCallback, hAction, p1, p2, p3);
	}

	BUS_SendMessage(pContext->stUiSession[eCxUiType].pfnUiCallback, message, hAction, p1, p2, p3);

	pContext->stUiSession[eCxUiType].ulSessionHandle = pstSession->hSession;

	return ERR_BUS_MESSAGE_BREAK;
}

STATIC BUS_Result_t		_xmgr_cas_CxMsgCloseSession(XmgrCas_CxContext_t *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	OxMgrCasSessionEvent_t	*pstSession = (OxMgrCasSessionEvent_t *)p3;
	XmgrCas_CxUiType_e		eCxUiType;

	if(pstSession->hSession == HANDLE_NULL)
	{
		HxLOG_Error("Invalid Session Handle\n");
		return ERR_BUS_MESSAGE_BREAK;
	}

	eCxUiType = _xmgr_cas_CxGetUiSessionType(pContext, pstSession->eSessionType, pstSession->ulUiHandle);
	if(eCxUiType == eCxCasUiType_Max)
	{
		if(BUS_MGR_Get((BUS_Callback_t)pstSession->ulUiHandle) != NULL)
		{
			return BUS_SendMessage((BUS_Callback_t)pstSession->ulUiHandle, message, hAction, p1, p2, p3);
		}

		HxLOG_Error("Invalid CONAX UI Session\n");
		return ERR_BUS_MESSAGE_PASS;
	}

	if (BUS_MGR_Get(pContext->stUiSession[eCxUiType].pfnUiCallback) == NULL)
	{
		// 아래를 check 하는 이유는 EXTENSION TYPE의 경우 OBAMA에서 REUQEST 하기 때문에 PROC이 무조건 존재해야 한다.
		// 빠르게 동작되어야 하는 UI의 경우는 발생할 수 있다.
		if(pstSession->eSessionType == eDxCAS_SESSIONTYPE_EXTENSION)
			HxLOG_Warning("Invalid sequence for CAS UI\n");

		HxLOG_Print("Create UI proc (%d)\n", eCxUiType);
		BUS_MGR_Create(NULL, APP_CAS_PRIORITY, pContext->stUiSession[eCxUiType].pfnUiCallback, hAction, p1, p2, p3);
	}

	BUS_SendMessage(pContext->stUiSession[eCxUiType].pfnUiCallback, message, hAction, p1, p2, p3);

	pContext->stUiSession[eCxUiType].ulSessionHandle = pstSession->hSession;

	return ERR_BUS_MESSAGE_BREAK;
}

STATIC BUS_Result_t _xmgr_cas_CxMsgInputNotify(XmgrCas_CxContext_t *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Callback_t				pfnUiCallback = NULL;
	DxCAS_MmiSessionType_e		eSessionType = eDxCAS_SESSIONTYPE_NONE;
	OxMgrCasUiUserInputNotify_t	*pstUserInputNotify = NULL;

	HxLOG_Trace();

	eSessionType = (DxCAS_MmiSessionType_e)p2;
	pstUserInputNotify = (OxMgrCasUiUserInputNotify_t *)p3;

	HxLOG_Info("UiHandle:[0x%08x]...\n", pstUserInputNotify->ulUiHandle);
	pfnUiCallback = (BUS_Callback_t)pstUserInputNotify->ulUiHandle;
	if(pfnUiCallback != NULL)
	{
		BUS_SendMessage(pfnUiCallback, message, hAction, p1, p2, p3);
	}

	(void)(eSessionType);

	return ERR_BUS_MESSAGE_BREAK;
}

STATIC BUS_Result_t		_xmgr_cas_CxMsgEvtCasFailOrCheck(XmgrCas_CxContext_t *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t			hRes = ERR_BUS_NO_ERROR;
	BUS_Callback_t			pfnUiCallback;
//	XmgrCas_CxUiType_e		eCxUiSession;

	pContext->hAction = hAction;

	pfnUiCallback = pContext->stUiSession[eCxCasUiType_SystemMessage].pfnUiCallback;

	if (BUS_MGR_Get(pfnUiCallback) == NULL)
		BUS_MGR_Create(NULL, APP_CAS_PRIORITY, pfnUiCallback, hAction, p1, p2, p3);

	return BUS_SendMessage(pfnUiCallback, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t		_xmgr_cas_CxMsgEvtSmartCard(XmgrCas_CxContext_t *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t			hRes = ERR_BUS_NO_ERROR;
	BUS_Callback_t			pfnUiCallback;
//	XmgrCas_CxUiType_e		eCxUiSession;

	pContext->hAction = hAction;

	pfnUiCallback = pContext->stUiSession[eCxCasUiType_CasMenu].pfnUiCallback;
	if (BUS_MGR_Get(pfnUiCallback) == NULL)
		BUS_MGR_Create(NULL, APP_CAS_PRIORITY, pfnUiCallback, hAction, p1, p2, p3);

	hRes = BUS_SendMessage(pfnUiCallback, message, hAction, p1, p2, p3);
	if(hRes != ERR_BUS_MESSAGE_PASS)
	{
		HxLOG_Error("BUS_SendMessage Error!! [%d]...\n", hRes);
		return hRes;
	}

	pfnUiCallback = pContext->stUiSession[eCxCasUiType_SystemMessage].pfnUiCallback;
	if (BUS_MGR_Get(pfnUiCallback) == NULL)
		BUS_MGR_Create(NULL, APP_CAS_PRIORITY, pfnUiCallback, hAction, p1, p2, p3);

	hRes = BUS_SendMessage(pfnUiCallback, message, hAction, p1, p2, p3);
	if(hRes != ERR_BUS_MESSAGE_PASS)
	{
		HxLOG_Error("BUS_SendMessage Error!! [%d]...\n", hRes);
		return hRes;
	}

	return ERR_BUS_MESSAGE_PASS;
}

STATIC BUS_Result_t _xmgr_cas_CxMsgNotifyServiceChanged(XmgrCas_CxContext_t *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
#if defined(CONFIG_SUPPORT_FCC)
	SvcCas_CtrlParam_t stParam;
	CX_SetMainActionInParam_t stInParam;

	stInParam.hMainAction = hAction;

	stParam.ulControlType = eCACTRL_CX_SetMainAction;
	stParam.pvIn = &stInParam;
	stParam.pvOut = NULL;

	if(SVC_CAS_CtrlSet(eDxCAS_GROUPID_CX, stParam.ulControlType, &stParam) != ERR_OK)
	{
		HxLOG_Error("SVC_CAS_CtrlSet(eCACTRL_CX_SetMainAction) error\n");
	}
#endif
	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t _xmgr_cas_CxMsgReadyShutdown(XmgrCas_CxContext_t *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	pContext->bStandby = TRUE;

	CXAPI_ReadyShutdown(xmgr_cas_CxGetPowerSavingStandby());

	return MESSAGE_PASS;
}

STATIC BUS_Result_t _xmgr_cas_CxMsgGoOperation(XmgrCas_CxContext_t *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	/*
		active standby와 passive standby or S3 일때가 WakeUp Reason이 각각 다르네...
		외부 요인 때문에 인증에 영향이 있으면 안되니 자체적으로 관리하는 것으로 해야겠다...
	*/
#if 0
	WakeUpReason_t eWakeUpReason;

	SVC_SYS_GetWakeUpReason(&eWakeUpReason);

	switch(eWakeUpReason)
	{
		case eWAKEUP_BY_KEY:
		case eWAKEUP_BY_TIMER:
			CXAPI_WakeUpStandby(xmgr_cas_CxGetPowerSavingStandby());
			break;

		default:
			HxLOG_Info("eWakeUpReason (%d)\n", eWakeUpReason);
			break;
	}
#else
	if(pContext->bStandby == TRUE)
	{
		CXAPI_WakeUpStandby(xmgr_cas_CxGetPowerSavingStandby());
		pContext->bStandby = FALSE;
	}

	CXAPI_GoOperation();
#endif
	return ERR_BUS_NO_ERROR;
}

#define _____LOCAL_EVENT_PROC_FUNCTIONS______________________________________________________
/*
STATIC BUS_Result_t xproc_cas_CxMsgMgr(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32				ulSlotNumber = 0;
	BUS_Result_t		eBusResult = ERR_BUS_NO_ERROR;
	DxCAS_MmiSessionType_e	eSessionType = eDxCAS_SESSIONTYPE_NONE;
	XmgrCas_CiCasMgrContext_t 	*pContext = (XmgrCas_CiCasMgrContext_t *)NULL;
	BUS_Callback_t		pfnCallback;
	HERROR				hErr = ERR_OK;
	OxMgrCasSessionEvent_t *pstSessionEvent = NULL;

	switch(message)
	{
		case eMEVT_CAS_ACTION_PREPARE_TUNE_FAIL:
			ulSlotNumber = (HUINT32)p1;
			pContext = xmgr_cas_CiGetCiCasMgrContext(ulSlotNumber);
			hErr = xmgr_cas_CiGetActProcByMSG(pContext, message, &pfnCallback);
			BUS_SendMessage(pfnCallback, message, hAction, ulSlotNumber, 0, 0);
			break;
		case eOEVT_CAS_CASUI_APP_OPEN_SESSION:
			HxLOG_Info("eOEVT_CAS_CASUI_APP_OPEN_SESSION...\n");
			pstSessionEvent = (OxMgrCasSessionEvent_t *)p3;
			pContext = xmgr_cas_CiGetCiCasMgrContext(pstSessionEvent->ulCasSlotNum);
			if(pContext != NULL)
			{
				eBusResult = xmgr_cas_CiMsgOpenSession(pContext, message, hAction, pstSessionEvent);
			}
			break;

		case eOEVT_CAS_CASUI_APP_CLOSE_SESSION:
			HxLOG_Info("eOEVT_CAS_CASUI_APP_CLOSE_SESSION...\n");
			pstSessionEvent = (OxMgrCasSessionEvent_t *)p3;
			pContext = xmgr_cas_CiGetCiCasMgrContext(pstSessionEvent->ulCasSlotNum);
			if(pContext != NULL)
			{
				eBusResult = xmgr_cas_CiMsgCloseSession(pContext, message, hAction, pstSessionEvent);
			}
			break;

		case eOEVT_CAS_CASUI_APP_USERINPUTDATA:
			HxLOG_Info("eOEVT_CAS_CASUI_APP_USERINPUTDATA...\n");
			ulSlotNumber = (HUINT32)p1;
			pContext = xmgr_cas_CiGetCiCasMgrContext(ulSlotNumber);
			if(pContext != NULL)
			{
				eBusResult = xmgr_cas_CiMsgCasUiInputNotify(pContext, message, hAction, p1, p2, p3);
			}
			break;

		default:
			break;
	}

	return eBusResult;
}
*/
BUS_Result_t xproc_cas_CxMain(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
//	HUINT32					ulActionId = SVC_PIPE_GetActionId(hAction);
	XmgrCas_CxContext_t		*pContext = _xmgr_cas_CxGetContext();
//	OxMgrCasSessionEvent_t *pstSessionEvent = NULL;
	BUS_Result_t			eBusResult = ERR_BUS_NO_ERROR;

	switch(message)
	{
		case eMEVT_BUS_CREATE :
			eBusResult = _xmgr_cas_CxMsgBusCreate(pContext);
			/*
			hError = OM_CAS_MMI_RegisterCasUiProc(eDxCAS_GROUPID_CX, (BUS_Callback_t)xproc_cas_CxMain);
			if(hError != ERR_OK)
			{
				HxLOG_Error("\n\n");
			}
			*/
 			break;
		case eMEVT_CAS_SC_INSERTED:
			HxLOG_Info("eMEVT_CAS_SC_INSERTED...\n");
			eBusResult = _xmgr_cas_CxMsgEvtSmartCard(pContext, message, hAction, p1, p2, p3);
			break;
		case eMEVT_CAS_SC_REMOVED:
			HxLOG_Info("eMEVT_CAS_SC_REMOVED...\n");
			eBusResult = _xmgr_cas_CxMsgEvtSmartCard(pContext, message, hAction, p1, p2, p3);
			break;
		case eMEVT_CAS_CMD :
			HxLOG_Info("eMEVT_CAS_CMD...\n");
//			eBusResult = _xmgr_cas_Na_MsgEvtCasCmd(pContext, message, hAction, p1, p2, p3);
//			break;
		case eMEVT_CAS_FAIL :
		case eMEVT_CAS_CHECK :
			_xmgr_cas_CxMsgEvtCasFailOrCheck(pContext, message, hAction, p1, p2, p3);
			break;
		// Receving eMEVT_CAS_OK message, all nagra cas windows have to close(except bdc window)
		case eMEVT_CAS_OK :
			_xmgr_cas_CxMsgEvtCasFailOrCheck(pContext, message, hAction, p1, p2, p3);
			break;
		// this message comes from watch tv(destory_ui function)
		case eMEVT_CAS_CLOSE :
//			if (_xmgr_cas_NaCheckAction(hAction) != ERR_OK)
//				return ERR_BUS_MESSAGE_PASS;
			break;
//		case eMEVT_LIVE_NOTIFY_SVC_CHANGED:
//			eBusResult = _xmgr_cas_Na_MsgSvcChanged(pContext, message, hAction, p1, p2, p3);
//			break;
		case eMEVT_BUS_DESTROY :
			break;

		case eOEVT_CAS_CASUI_APP_OPEN_SESSION:
			eBusResult = _xmgr_cas_CxMsgOpenSession(pContext, message, hAction, p1, p2, p3);
			break;

		case eOEVT_CAS_CASUI_APP_CLOSE_SESSION:
			eBusResult = _xmgr_cas_CxMsgCloseSession(pContext, message, hAction, p1, p2, p3);
			break;

		case eOEVT_CAS_CASUI_APP_USERINPUTDATA:
			HxLOG_Info("eOEVT_CAS_CASUI_APP_USERINPUTDATA...\n");
			eBusResult = _xmgr_cas_CxMsgInputNotify(pContext, message, hAction, p1, p2, p3);
			break;

		case eMEVT_LIVE_NOTIFY_SVC_CHANGED:
			eBusResult = _xmgr_cas_CxMsgNotifyServiceChanged(pContext, message, hAction, p1, p2, p3);
			break;

		case eMEVT_BUS_READY_SHUTDOWN:
			eBusResult = _xmgr_cas_CxMsgReadyShutdown(pContext, message, hAction, p1, p2, p3);
#if defined(CONFIG_MW_CAS_CONAX_PRETEST)
			if (xmgr_cas_CxGetPowerSavingStandby() == FALSE)
			{
				MGR_EMMUpdate_Start();
			}
#endif
			break;
		case eMEVT_BUS_GO_OPERATION:
			eBusResult = _xmgr_cas_CxMsgGoOperation(pContext, message, hAction, p1, p2, p3);
#if defined(CONFIG_MW_CAS_CONAX_PRETEST)
			MGR_EMMUpdate_Stop();
#endif
			break;
#if defined(CONFIG_MW_CAS_CONAX_PRETEST)
		case eMEVT_PVR_NOTIFY_RECORD_STARTED:
			if (MGR_ACTION_GetState() == eAmState_ACTIVE_STANDBY)
			{
				MGR_EMMUpdate_Stop();
			}
			break;

		case eMEVT_PVR_NOTIFY_RECORD_STOPPED:
			if (MGR_ACTION_GetState() == eAmState_ACTIVE_STANDBY)
			{
				MGR_EMMUpdate_Start();
			}
			break;
#endif
		default:
			break;
	}

	if(eBusResult != ERR_BUS_NO_ERROR)
	{
		return eBusResult;
	}

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}

void xmgr_cas_CxCmdInit(HxCMD_Handle_t hCmdHandle)
{
//	xsvc_cas_CxCmdInit(hCmdHandle);
	return;
}

