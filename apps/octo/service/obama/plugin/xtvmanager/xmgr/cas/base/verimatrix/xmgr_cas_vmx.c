/**
	@file     xmgr_cas_vmx.c
	@brief    file_name & simple comment.

	Description: ... \n
	Module: ... \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/**
  * Character encoding.
  *
  * MS949
  *
  * This source file that uses MS949 encoding.
  * MS949 encoding 을 사용하는 source file 입니다.
  * MS949 encodingを使用して source fileです。
  * Quelldatei, die MS949-Codierung verwendet.
  *
**/

#include <octo_common.h>
#include <mgr_action.h>
#include <mgr_live.h>

#include <svc_pipe.h>
#include <svc_si.h>
#include <svc_cas.h>

#include <mgr_cas.h>
#include <xmgr_cas.h>
#include <xmgr_cas_res_str.h>

#include "local_include/_xmgr_cas_vmx.h"
#include "local_include/_xmgr_cas_vmx_adapt.h"
#if defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)
#include "_xsvc_cas_vmx_iptv_main.h"
#include "_xsvc_cas_vmx_iptvh_ui.h"
#endif

/*******************************************************************/
/********************      Definitions    **************************/
/*******************************************************************/
typedef struct
{
	HUINT32				ulSessionHandle;	// APPKIT에서 관리되고 있는 Handle
	BUS_Callback_t		pfnUiCallback;
} XmgrCas_VmxUiSession_t;

typedef struct
{
	Handle_t				hAction;
	XmgrCas_VmxUiSession_t	stUiSession[eVmxCasUiType_Max];
} XmgrCas_VmxContext_t;

/* local macros using in proc msg  */
#define VMX_CAS_PARAM_WARNING_REMOVE(arg1, arg2, arg3, arg4, arg5, arg6)	\
			{													\
				(void)arg1;										\
				(void)arg2;										\
				(void)arg3;										\
				(void)arg4;				 						\
				(void)arg5;				 						\
				(void)arg6;				 						\
			}



/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC XmgrCas_VmxContext_t			s_stVmxCasContext;


/*******************************************************************/
/********************      External Functions      ********************/
/*******************************************************************/
extern HERROR	OM_CAS_MMI_RegisterCasUiProc(DxCAS_GroupId_e eCasGrpId, BUS_Callback_t fnCasUiProc);
#if defined(CONFIG_DEBUG)
extern void xsvc_cas_VmxIptvCmdInit(HxCMD_Handle_t hCmdHandle);
#endif

/*******************************************************************/
/********************      Local Functions      ********************/
/*******************************************************************/
#if defined (CONFIG_DEBUG)
HUINT8 *xmgr_cas_VmxGetMessageName(HINT32 message);
#endif
STATIC BUS_Result_t _xmgr_cas_VmxMsgGwmCreate(XmgrCas_VmxContext_t *pContext);

#define _____XMGR_CAS_VMX_LOCAL_FUNCTION______________________________________________________

STATIC XmgrCas_VmxContext_t *_xmgr_cas_VmxGetContext(void)
{
	return &s_stVmxCasContext;
}

STATIC HERROR _xmgr_cas_VmxCheckAction(Handle_t hAction)
{
	if (SVC_PIPE_GetActionId(hAction) != MGR_ACTION_GetMainActionId())
	{
		HxLOG_Error("\r hAction(0x%08x)\n", hAction);
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC XmgrCas_VmxUiType_e _xmgr_cas_Vmx_GetVmxUiSessionType(XmgrCas_VmxContext_t *pContext, DxCAS_MmiSessionType_e eUiSession, HUINT32 ulUiHandle)
{
	XmgrCas_VmxUiType_e eVmxUiSession = eVmxCasUiType_Max;
	XmgrCas_VmxUiType_e eIndex;

	switch(eUiSession)
	{
		case eDxCAS_SESSIONTYPE_BANNER:
			//eVmxUiSession = eVmxCasUiType_FingerPrinting; // test only : 어떤것으로 display해야할지 정해지지 않음
			break;

		case eDxCAS_SESSIONTYPE_MENU:
			//eVmxUiSession =
			break;

		case eDxCAS_SESSIONTYPE_EXTENSION:
			for(eIndex = 0 ; eIndex < eVmxCasUiType_Max ; eIndex++)
			{
				if(pContext->stUiSession[eIndex].pfnUiCallback == (BUS_Callback_t)ulUiHandle)
				{
					eVmxUiSession = eIndex;
					break;
				}
			}
			break;

		default:
			break;
	}

	return eVmxUiSession;
}

STATIC BUS_Result_t		_xmgr_cas_VmxMsgGwmCreate(XmgrCas_VmxContext_t *pContext)
{
	HxSTD_memset(pContext, 0, sizeof(XmgrCas_VmxContext_t));

	pContext->stUiSession[eVmxCasUiType_FingerPrinting].pfnUiCallback	= xproc_cas_VmxUiFingerPrinting;
	pContext->stUiSession[eVmxCasUiType_Popup].pfnUiCallback			= xproc_cas_VmxUiPopup;
	pContext->stUiSession[eVmxCasUiType_SystemMessage].pfnUiCallback	= xproc_cas_VmxUiErrorMsg;
//	pContext->stUiSession[eVmxCasUiType_Pincode].pfnUiCallback	= xproc_cas_VmxUiPinDialog;

	if(OM_CAS_MMI_RegisterCasUiProc(eDxCAS_GROUPID_VX, xproc_cas_VmxMain) != ERR_OK)
	{
		HxLOG_Error("OM_CAS_MMI_RegisterCasUiProc() error\n");
	}

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t		_xmgr_cas_Vmx_MsgEvtCasCmd(XmgrCas_VmxContext_t *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t				hRes = ERR_BUS_NO_ERROR;
	XmgrCas_VmxUiType_e			eVmxUiSession = eVmxCasUiType_Max;
	VmxIptv_DisplayUi_t			*pstDisplayUi = NULL;
	VmxIptv_DisplayType_e		eDisplayType = eVmxIptv_Display_Max;

	if (_xmgr_cas_VmxCheckAction(hAction) != ERR_OK)
	{
		HxLOG_Error("_xmgr_cas_VmxCheckAction() fail, p1(%x), p2(%x)\n", p1, p2);
		return ERR_BUS_MESSAGE_PASS;
	}

	pstDisplayUi = (VmxIptv_DisplayUi_t *)p2;

	if(pstDisplayUi == NULL)
	{
		HxLOG_Error("pstDisplayUi() is NULL\n");
		return ERR_BUS_MESSAGE_PASS;
	}

	eDisplayType = pstDisplayUi->eType;

	// TODO_VMX :  현재는 임의로 모두 eVmxCasUiType_Popup으로 넣어지만 실제 stream으로 test시 변경해주어야한다.
	switch(eDisplayType)
	{
		case eVmxIptv_Display_FingerPrinting:
			eVmxUiSession = eVmxCasUiType_FingerPrinting;
			break;
		case eVmxIptv_Display_OsdMessage:
			eVmxUiSession = eVmxCasUiType_Popup;
			break;
		case eVmxIptv_Display_ErrorMessage:
			eVmxUiSession = eVmxCasUiType_Popup;
			break;
		default:
			HxLOG_Error("Unknown eDisplayType (0x%x)\n", eDisplayType);
			return ERR_BUS_NO_ERROR;
	}

	if(BUS_MGR_Get(pContext->stUiSession[eVmxUiSession].pfnUiCallback) == NULL)
	{
		BUS_MGR_Create(NULL, APP_CAS_PRIORITY, pContext->stUiSession[eVmxUiSession].pfnUiCallback, hAction, p1, p2, p3);
	}

	BUS_SendMessage(pContext->stUiSession[eVmxUiSession].pfnUiCallback, message, hAction, p1, p2, p3);

	VMX_CAS_PARAM_WARNING_REMOVE(pContext, message, hAction, p1, p2, p3);
	return hRes;
}

STATIC BUS_Result_t		_xmgr_cas_Vmx_MsgEvtCasFailOrCheck(XmgrCas_VmxContext_t *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t	hRes = ERR_BUS_NO_ERROR;
	XmgrCas_VmxUiType_e		eVmxUiSession;
	VmxIptv_DisplayUi_t			*pstDisplayUi = NULL;
	VmxIptv_DisplayType_e		eDisplayType = eVmxIptv_Display_Max;

	if (_xmgr_cas_VmxCheckAction(hAction) != ERR_OK)
	{
		HxLOG_Error("_xmgr_cas_VmxCheckAction() fail, p1(%x), p2(%x)\n", p1, p2);
		return ERR_BUS_MESSAGE_PASS;
	}

	pstDisplayUi = (VmxIptv_DisplayUi_t *)p2;

	if(pstDisplayUi == NULL)
	{
		HxLOG_Error("pstDisplayUi() is NULL\n");
		return ERR_BUS_MESSAGE_PASS;
	}

	eDisplayType = pstDisplayUi->eType;

	// TODO_VMX :  현재는 임의로 모두 eVmxCasUiType_Popup으로 넣어지만 실제 stream으로 test시 변경해주어야한다.
	switch(eDisplayType)
	{
 		case eVmxIptv_Display_ErrorMessage:
			eVmxUiSession = eVmxCasUiType_SystemMessage;
			break;
		case eVmxIptv_Display_PinDialog:
//			eVmxUiSession = eVmxCasUiType_Pincode;
		default:
			HxLOG_Error("Unknown eDisplayType (0x%x)\n", eDisplayType);
			return ERR_BUS_MESSAGE_PASS;
			break;
	}

	if(BUS_MGR_Get(pContext->stUiSession[eVmxUiSession].pfnUiCallback) == NULL)
	{
		BUS_MGR_Create(NULL, APP_CAS_PRIORITY, pContext->stUiSession[eVmxUiSession].pfnUiCallback, hAction, p1, p2, p3);
	}

	BUS_SendMessage(pContext->stUiSession[eVmxUiSession].pfnUiCallback, message, hAction, p1, p2, p3);

	VMX_CAS_PARAM_WARNING_REMOVE(pContext, message, hAction, p1, p2, p3);
	return hRes;
}

STATIC BUS_Result_t		_xmgr_cas_Vmx_MsgOpenSession(XmgrCas_VmxContext_t *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	OxMgrCasSessionEvent_t	*pstSession = (OxMgrCasSessionEvent_t *)p3;
	XmgrCas_VmxUiType_e		eVmxUiSession;

	if(pstSession->hSession == HANDLE_NULL)
	{
		HxLOG_Error("Invalid Session Handle\n");
		return ERR_BUS_MESSAGE_BREAK;
	}

	eVmxUiSession = _xmgr_cas_Vmx_GetVmxUiSessionType(pContext, pstSession->eSessionType, pstSession->ulUiHandle);
	if(eVmxUiSession == eVmxCasUiType_Max)
	{
		if(BUS_MGR_Get((BUS_Callback_t)pstSession->ulUiHandle) != NULL)
		{
			return BUS_SendMessage((BUS_Callback_t)pstSession->ulUiHandle, message, hAction, p1, p2, p3);
		}

		HxLOG_Error("Invalid VERIMATRIX UI Session\n");
		return ERR_BUS_MESSAGE_PASS;
	}

	if (BUS_MGR_Get(pContext->stUiSession[eVmxUiSession].pfnUiCallback) == NULL)
	{
		// 아래를 check 하는 이유는 EXTENSION TYPE의 경우 OBAMA에서 REUQEST 하기 때문에 PROC이 무조건 존재해야 한다.
		// 빠르게 동작되어야 하는 UI의 경우는 발생할 수 있다.
		if(pstSession->eSessionType == eDxCAS_SESSIONTYPE_EXTENSION)
		{
			HxLOG_Warning("Invalid sequence for CAS UI\n");
		}

		HxLOG_Print("Create UI proc (%d)\n", eVmxUiSession);
		BUS_MGR_Create(NULL, APP_CAS_PRIORITY, pContext->stUiSession[eVmxUiSession].pfnUiCallback, hAction, p1, p2, p3);
	}

	BUS_SendMessage(pContext->stUiSession[eVmxUiSession].pfnUiCallback, message, hAction, p1, p2, p3);

	pContext->stUiSession[eVmxUiSession].ulSessionHandle = pstSession->hSession;

	VMX_CAS_PARAM_WARNING_REMOVE(pContext, message, hAction, p1, p2, p3);
	return ERR_BUS_MESSAGE_BREAK;
}

STATIC BUS_Result_t		_xmgr_cas_Vmx_MsgCloseSession(XmgrCas_VmxContext_t *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	OxMgrCasSessionEvent_t	*pstSession = (OxMgrCasSessionEvent_t *)p3;
	XmgrCas_VmxUiType_e		eVmxUiSession;

	eVmxUiSession = _xmgr_cas_Vmx_GetVmxUiSessionType(pContext, pstSession->eSessionType, pstSession->ulUiHandle);
	if(eVmxUiSession == eVmxCasUiType_Max)
	{
		if(BUS_MGR_Get((BUS_Callback_t)pstSession->ulUiHandle) != NULL)
		{
			return BUS_SendMessage((BUS_Callback_t)pstSession->ulUiHandle, message, hAction, p1, p2, p3);
		}

		HxLOG_Error("Invalid VERIMATRIX UI Session\n");
		return ERR_BUS_MESSAGE_PASS;
	}

	if (BUS_MGR_Get(pContext->stUiSession[eVmxUiSession].pfnUiCallback) != NULL)
	{
		// 코드가 이리 되어 있는 상태라 작업은 해 두었는데... 왜 Proc이 있는데 다시 Proc을 생성할려고 하지?
		BUS_SendMessage(pContext->stUiSession[eVmxUiSession].pfnUiCallback, message, hAction, p1, p2, p3);
	}

	pContext->stUiSession[eVmxUiSession].ulSessionHandle = HANDLE_NULL;

	VMX_CAS_PARAM_WARNING_REMOVE(pContext, message, hAction, p1, p2, p3);
	return ERR_BUS_MESSAGE_BREAK;
}

STATIC BUS_Result_t _xmgr_cas_Vmx_MsgUserInputData(XmgrCas_VmxContext_t *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	DxCAS_MmiSessionType_e		eSessionType = (DxCAS_MmiSessionType_e)p2;
	OxMgrCasUiUserInputNotify_t *pstUserInputNotify = (OxMgrCasUiUserInputNotify_t *)p3;
	XmgrCas_VmxUiType_e			eVmxUiSession;

	eVmxUiSession = _xmgr_cas_Vmx_GetVmxUiSessionType(pContext, eSessionType, pstUserInputNotify->ulUiHandle);
	if(eVmxUiSession == eVmxCasUiType_Max)
	{
		if(BUS_MGR_Get((BUS_Callback_t)pstUserInputNotify->ulUiHandle) != NULL)
		{
			return BUS_SendMessage((BUS_Callback_t)pstUserInputNotify->ulUiHandle, message, hAction, p1, p2, p3);
		}

		HxLOG_Error("Invalid VERIMATRIX UI Session\n");
		return ERR_BUS_MESSAGE_PASS;
	}

	if (BUS_MGR_Get(pContext->stUiSession[eVmxUiSession].pfnUiCallback) != NULL)
	{
		BUS_SendMessage(pContext->stUiSession[eVmxUiSession].pfnUiCallback, message, hAction, p1, p2, p3);
	}

	VMX_CAS_PARAM_WARNING_REMOVE(pContext, message, hAction, p1, p2, p3);
	return ERR_BUS_MESSAGE_BREAK;
}

STATIC BUS_Result_t _xmgr_cas_Vmx_MsgSvcChanged(XmgrCas_VmxContext_t *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t	eBusResult = ERR_BUS_NO_ERROR;


	if (BUS_MGR_Get(pContext->stUiSession[eVmxCasUiType_FingerPrinting].pfnUiCallback) != NULL)
	{
		BUS_SendMessage(pContext->stUiSession[eVmxCasUiType_FingerPrinting].pfnUiCallback, message, hAction, p1, p2, p3);
	}

	if (BUS_MGR_Get(pContext->stUiSession[eVmxCasUiType_Popup].pfnUiCallback) != NULL)
	{
		BUS_SendMessage(pContext->stUiSession[eVmxCasUiType_Popup].pfnUiCallback, message, hAction, p1, p2, p3);
	}

	return eBusResult;
}

HERROR xmgr_cas_VmxDefaultSetting(void)
{
	return xsvc_vmx_main_DefaultSetting();
}

BUS_Result_t	xproc_cas_VmxMain (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t			eBusResult = ERR_BUS_NO_ERROR;
//	HUINT32					ulActionId = SVC_PIPE_GetActionId(hAction);
	XmgrCas_VmxContext_t		*pContext = _xmgr_cas_VmxGetContext();

#if defined(CONFIG_DEBUG)
	HUINT8 *pszMsg = xmgr_cas_VmxGetMessageName(message);
	if(pszMsg != NULL)
	{
		HxLOG_Print("message(%s), ActId(%x), p1(%x), p2(%x), p3(%x)\n", pszMsg, hAction, p1, p2, p3);
	}
#endif

	switch(message)
	{
		case eMEVT_BUS_CREATE :
			eBusResult = _xmgr_cas_VmxMsgGwmCreate(pContext);
 			break;

		case eMEVT_CAS_CMD :
			eBusResult = _xmgr_cas_Vmx_MsgEvtCasCmd(pContext, message, hAction, p1, p2, p3);
			break;

		case eMEVT_CAS_FAIL :
		case eMEVT_CAS_CHECK:
			eBusResult = _xmgr_cas_Vmx_MsgEvtCasFailOrCheck(pContext, message, hAction, p1, p2, p3);
			break;

		case eMEVT_LIVE_NOTIFY_SVC_CHANGED:
			eBusResult = _xmgr_cas_Vmx_MsgSvcChanged(pContext, message, hAction, p1, p2, p3);
			break;

		case eMEVT_BUS_DESTROY :
			break;

		case eOEVT_CAS_CASUI_APP_OPEN_SESSION:
			eBusResult = _xmgr_cas_Vmx_MsgOpenSession(pContext, message, hAction, p1, p2, p3);
			break;

		case eOEVT_CAS_CASUI_APP_CLOSE_SESSION:
			eBusResult = _xmgr_cas_Vmx_MsgCloseSession(pContext, message, hAction, p1, p2, p3);
			break;

		case eOEVT_CAS_CASUI_APP_USERINPUTDATA:
			eBusResult = _xmgr_cas_Vmx_MsgUserInputData(pContext, message, hAction, p1, p2, p3);
			break;
		case eOEVT_PG_CHECK_RATING:

			break;
		case eMEVT_PG_PIN_CHECK_OK:

			break;

#if defined(CONFIG_APCORE_ALWAYS_ACTIVE_STANDBY)
		case eMEVT_BUS_READY_SHUTDOWN:
			return MESSAGE_PASS;
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

#if defined(CONFIG_DEBUG)
void xmgr_cas_VmxIptvCmd(HxCMD_Handle_t hCmdHandle)
{
	xsvc_cas_VmxIptvCmdInit(hCmdHandle);
}
#endif

/********************************************************************
* For Debugging
********************************************************************/
#if defined (CONFIG_DEBUG)
HUINT8 *xmgr_cas_VmxGetMessageName(HINT32 message)
{
	switch(message)
	{
		ENUM_TO_STR(eOEVT_CAS_CASUI_APP_OPEN_SESSION);
		ENUM_TO_STR(eOEVT_CAS_CASUI_APP_CLOSE_SESSION);
		ENUM_TO_STR(eOEVT_CAS_CASUI_APP_USERINPUTDATA);
		ENUM_TO_STR(eMEVT_LIVE_NOTIFY_SVC_CHANGED);
		ENUM_TO_STR(eMEVT_LIVE_NOTIFY_VIEW_STATE_CHANGED);
		ENUM_TO_STR(eMEVT_CAS_CMD);
		ENUM_TO_STR(eMEVT_CAS_FAIL);
		ENUM_TO_STR(eOEVT_PG_CHECK_RATING);
		ENUM_TO_STR(eMEVT_PG_PIN_CHECK_OK);
		ENUM_TO_STR(eMEVT_BUS_READY_SHUTDOWN);
		ENUM_TO_STR(eMEVT_BUS_GO_OPERATION);
		ENUM_TO_STR(eMEVT_ACTION_FINISHED_BATCH_ACTION_ITEMS);

		ENUM_TO_STR(eMEVT_CAS_CAM_REMOVED);		/* cam이 제거되는경우 화면에 error msg를 dispplay하기위해 여기에 cam remove를 만듬 */
		default:
			return NULL;
	}
}
#endif

