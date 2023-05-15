/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <mgr_live.h>
#include <mgr_cas.h>
#include <mgr_action.h>

#include <xmgr_cas.h>
#include <xmgr_cas_res_str.h>


#include "../local_include/_xmgr_cas_vmx.h"
#include "../local_include/_xmgr_cas_vmx_util.h"

#if defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)
#include "_xsvc_cas_vmx_iptvh_ui.h"
#endif
#include "_xsvc_cas_vmx_ctrl.h"
#include "_svc_cas_mgr_main.h"
#include "_svc_cas_mgr_instance.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/


typedef struct
{
	Handle_t		hAction;
	Handle_t		hSession;
} sVmxUi_PinDialog_Context_t;

STATIC sVmxUi_PinDialog_Context_t	s_stCasVmxPinDialogContext;


STATIC sVmxUi_PinDialog_Context_t*	_xmgr_cas_VmxUi_PinDialog_GetContext(void)
{
	return &s_stCasVmxPinDialogContext;
}

STATIC HERROR _xmgr_cas_VmxUi_PinDialog_SendPinMessage(sVmxUi_PinDialog_Context_t *pstContext)
{
	OxMgrCasUiEvtInput_t	*pstEvent = NULL;
	BUS_Result_t			hBusResult = ERR_BUS_NO_ERROR;
	OxMgrCasUiInputHeader_t	*pstHeader = NULL;
	OxMgrCasUiInput_t		*pstInputData = NULL;

	HxLOG_Print("[Enter]\n");

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
	pstEvent->stSessionInfo.ulUiHandle		= (HUINT32)xproc_cas_VmxUiPinDialog;

	pstHeader = &pstEvent->stHeader;
	pstInputData = &pstEvent->stInputData;

	pstInputData->eInputType = eMgrCasInputType_Number;
	pstInputData->utInputData.stNumber.ulAttr = eMgrCasUiAttr_HiddenInput ;
	pstInputData->utInputData.stNumber.ulNumberLength = 4;
	HxSTD_MemSet(pstInputData->utInputData.stNumber.pszNumber, 0x00, sizeof(MGR_CASINPUT_NUMBER_MAX_LENGTH));

	hBusResult = BUS_PostMessageToOM(NULL, eMEVT_CASUI_TYPE_INPUT_DATA, HANDLE_NULL, (HINT32)eDxCAS_GROUPID_VMX, (HINT32)0, (HINT32)pstEvent);
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

STATIC HERROR _xmgr_cas_VmxUi_PinDialog_CheckPinCode(sVmxUi_PinDialog_Context_t *pstContext, OxMgrCasUiUserInputNotify_t *pstInputNotify)
{
//	SvcCas_CtrlParam_t				stParam;
//	VMXIPTV_SetOverrideRatingLevelInParam_t	stInParam;
	OxMgrCasUiNotifyNumber_t		*pstNumber;
	HCHAR 							szPinCode[16] = {0,};
	HUINT8							ucPinLength;

	HxLOG_Print("[Enter]\n");

	if(DB_PARAM_GetItem(eDB_PARAM_ITEM_USER_PINCODE, (HUINT32 *)szPinCode, 16) != ERR_OK)
	{
		HLIB_STD_StrNCpySafe(szPinCode, "0000", 16);
	}

	pstNumber = &pstInputNotify->utData.stNumber;

	HxLOG_Print("UserInput Pin(%s), STBPin(%s)\n", pstNumber->pszNumber, szPinCode);

	ucPinLength = (pstNumber->ulNumberLength > 16 ? 16 : pstNumber->ulNumberLength);
	if(HxSTD_StrNCmp(szPinCode, pstNumber->pszNumber, ucPinLength) != 0)
	{
		return _xmgr_cas_VmxUi_PinDialog_SendPinMessage(pstContext);
	}

	#if 0
	stInParam.hAction		= pstContext->hAction;
	stInParam.bUserPinPass	= TRUE;

	stParam.pvIn			= &stInParam;
	stParam.ulControlType	= eVmxIptvCtrl_SetOverrideRatingLevel;

	if(SVC_CAS_CtrlSet(eDxCAS_GROUPID_VMX, stParam.ulControlType, &stParam) != ERR_OK)
	{
		HxLOG_Error("SVC_CAS_CtrlSet(eVmxIptvCtrl_SetOverrideRatingLevel) error\n");
		return ERR_FAIL;
	}
	#endif
	xmgr_cas_Vmx_SetOverrideRatingLevel(pstContext->hAction, 0);

	xmgr_cas_VmxUtil_PostMessageSessionEventToOM(eDxCAS_SESSIONTYPE_EXTENSION, xproc_cas_VmxUiPinDialog, pstContext->hSession, 0, eMEVT_CASUI_REQ_CLOSE_SESSION);

	return ERR_OK;
}

STATIC BUS_Result_t _xmgr_cas_VmxUi_PinDialog_MsgCreate(sVmxUi_PinDialog_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxLOG_Print("[Enter]\n");	
	if(pstContext == NULL)
	{
		HxLOG_Error("\r\n");
		return ERR_BUS_MESSAGE_PASS;	
	}

	HxSTD_memset(pstContext, 0x00, sizeof(sVmxUi_PinDialog_Context_t));

	pstContext->hSession = HANDLE_NULL;

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t _xmgr_cas_VmxUi_PinDialog_MsgDestroy(sVmxUi_PinDialog_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t _xmgr_cas_VmxUi_PinDialog_MsgEvtInputNotify(sVmxUi_PinDialog_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32							ulSlotNumber = 0;
	DxCAS_MmiSessionType_e		eSessionType;
	OxMgrCasUiUserInputNotify_t		*pstUserInputNotify = NULL;

	HxLOG_Print("[Enter]\n");

	if(pstContext == NULL)
	{
		HxLOG_Error("\r\n");
		return ERR_BUS_MESSAGE_PASS;	
	}	

	ulSlotNumber = (HUINT32)p1;
	eSessionType = (DxCAS_MmiSessionType_e)p2;
	pstUserInputNotify = (OxMgrCasUiUserInputNotify_t *)p3;

	switch(pstUserInputNotify->eInputType)
	{
		case eMgrCasInputType_Number:
			_xmgr_cas_VmxUi_PinDialog_CheckPinCode(pstContext, pstUserInputNotify);
			break;

		default:
			break;
	}

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t _xmgr_cas_VmxUi_PinDialog_ServiceChanged(sVmxUi_PinDialog_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxLOG_Print("[Enter]\n");	
	if(pstContext == NULL)
	{
		HxLOG_Error("\r\n");
		return ERR_BUS_MESSAGE_PASS;	
	}

	if(pstContext->hSession != HANDLE_NULL)
	{
		xmgr_cas_VmxUtil_PostMessageSessionEventToOM(eDxCAS_SESSIONTYPE_EXTENSION, xproc_cas_VmxUiPopup, pstContext->hSession, 0, eMEVT_CASUI_REQ_CLOSE_SESSION);
	}
	HxLOG_Print("[Exit]\n");
	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t _xmgr_cas_VmxUi_PinDialog_MsgEvtCasFail(sVmxUi_PinDialog_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxLOG_Print("[Enter]\n");	
	pstContext->hAction = hAction;

	if(pstContext->hSession != HANDLE_NULL)
		_xmgr_cas_VmxUi_PinDialog_SendPinMessage(pstContext);
	else
		xmgr_cas_VmxUtil_PostMessageSessionEventToOM(eDxCAS_SESSIONTYPE_EXTENSION, xproc_cas_VmxUiPinDialog, HANDLE_NULL, 0, eMEVT_CASUI_REQ_OPEN_SESSION);

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t _xmgr_cas_VmxUi_PinDialog_MsgOpenSession(sVmxUi_PinDialog_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	OxMgrCasSessionEvent_t	*pstSession = (OxMgrCasSessionEvent_t *)p3;
	HxLOG_Print("[Enter]\n");
	if(pstContext == NULL || pstSession == NULL)
	{
		HxLOG_Error("\r\n");
		return ERR_BUS_MESSAGE_PASS;	
	}

	pstContext->hSession = pstSession->hSession;

	xmgr_cas_VmxUtil_PostMessageSessionEventToOM(eDxCAS_SESSIONTYPE_EXTENSION, xproc_cas_VmxUiPinDialog, pstSession->hSession, 0, eMEVT_CASUI_SESSION_OPENED);

	if(_xmgr_cas_VmxUi_PinDialog_SendPinMessage(pstContext) != ERR_OK)
	{
		HxLOG_Error("\r\n");
	}

	return ERR_BUS_MESSAGE_PASS;
}

STATIC BUS_Result_t _xmgr_cas_VmxUi_PinDialog_MsgCloseSession(sVmxUi_PinDialog_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxLOG_Print("[Enter]\n");
	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return ERR_BUS_MESSAGE_PASS;
	}
	
	pstContext->hSession = HANDLE_NULL;

	return BUS_MGR_Destroy(NULL);
}


BUS_Result_t	xproc_cas_VmxUiPinDialog (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t eBusRes = ERR_BUS_NO_ERROR;
	sVmxUi_PinDialog_Context_t	*pstContext = _xmgr_cas_VmxUi_PinDialog_GetContext();

#if defined(CONFIG_DEBUG)
	HUINT8 *pszMsg = xmgr_cas_VmxGetMessageName(message);
	if(pszMsg != NULL)
	{
		HxLOG_Print("message(%s), hAction(%x), p1(%x), p2(%x), p3(%x)\n", pszMsg, hAction, p1, p2, p3);
	}
#endif

	switch(message)
	{
		case eMEVT_BUS_CREATE :
			BUS_MGR_SetMgrName(BUS_MGR_GetThis(), "xproc_cas_VmxUiPinDialog");
			eBusRes = _xmgr_cas_VmxUi_PinDialog_MsgCreate(pstContext, message, hAction, p1, p2, p3);			
			break;

		case eMEVT_CAS_FAIL:
			eBusRes = _xmgr_cas_VmxUi_PinDialog_MsgEvtCasFail(pstContext, message, hAction, p1, p2, p3);
			break;

		case eOEVT_CAS_CASUI_APP_OPEN_SESSION:
			eBusRes = _xmgr_cas_VmxUi_PinDialog_MsgOpenSession(pstContext, message, hAction, p1, p2, p3);
			break;

		case eOEVT_CAS_CASUI_APP_CLOSE_SESSION:
			eBusRes = _xmgr_cas_VmxUi_PinDialog_MsgCloseSession(pstContext, message, hAction, p1, p2, p3);
			break;

		case eOEVT_CAS_CASUI_APP_USERINPUTDATA :
			eBusRes = _xmgr_cas_VmxUi_PinDialog_MsgEvtInputNotify(pstContext, message, hAction, p1, p2, p3);
			break;

		case eMEVT_LIVE_NOTIFY_SVC_CHANGED:
			eBusRes = _xmgr_cas_VmxUi_PinDialog_ServiceChanged(pstContext, message, hAction, p1, p2, p3);
			break;

		case eMEVT_BUS_DESTROY :
			eBusRes = _xmgr_cas_VmxUi_PinDialog_MsgDestroy(pstContext, message, hAction, p1, p2, p3);			
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

