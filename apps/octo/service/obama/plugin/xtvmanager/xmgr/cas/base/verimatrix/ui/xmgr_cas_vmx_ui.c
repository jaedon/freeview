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
#include "_svc_cas_mgr_main.h"
#include "_svc_cas_mgr_instance.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define 	VMX_UI_TIMER_ID				900					// TODO_VMX : 이것은 필요시 사용해야함.
#define		TEXT_SIZE512				512

typedef enum
{
	ePopup_Message,
	ePopup_Popup_Max
} VmxPopupType_t;

typedef struct
{
	Handle_t				hSession;
	HINT32				nTimeout;
	VmxPopupType_t 	eType;
	VmxIptv_PopupItem_t *pstPopupItem;
} sVmxUi_Popup_Context_t;

typedef struct
{
	Handle_t				hSession;
	HINT32				nTimeout;
	VmxIptv_Fingerprinting_t	*pstFingerPrinting;
} sVmxUi_FingerPrinting_Context_t;

typedef struct
{
	Handle_t				hSession;
	HINT32				nTimeout;
	HUINT8				*pucMsg;
} sVmxUi_ErrorMsg_Context_t;

STATIC sVmxUi_Popup_Context_t	s_stCasVmxPopupContext;
STATIC sVmxUi_FingerPrinting_Context_t	s_stCasVmxFingerPrintingContext;
STATIC sVmxUi_ErrorMsg_Context_t	s_stCasVmxErrorMsgContext;

#define ______________FINGER_PRINT______________________________________________

STATIC sVmxUi_FingerPrinting_Context_t*	_xmgr_cas_VmxUi_FingerPrinting_GetContext(void)
{
	return &s_stCasVmxFingerPrintingContext;
}

STATIC void _xmgr_cas_VmxUi_FingerPrinting_Free(VmxIptv_Fingerprinting_t *pstFingerPrinting)
{
	HUINT16 i;
	if(pstFingerPrinting != NULL)
	{
		for(i=0;i<VMX_FINGERPRINTING_LOCALDATA_MAX_NUM;i++)
		{
			if(pstFingerPrinting->stLocalData[i].pData != NULL)
			{
				OxCAS_Free(pstFingerPrinting->stLocalData[i].pData);
				pstFingerPrinting->stLocalData[i].pData = NULL;
			}
		}

		//OxCAS_Free(pstFingerPrinting);
	}
}

STATIC HERROR _xmgr_cas_VmxUi_FingerPrinting_SendMessage(sVmxUi_FingerPrinting_Context_t *pstContext)
{
#if 0 // TODO_VMX FingerPrinting를 ui와 논의후 작업 진행해야함.
	OxMgrCasUiEvtInput_t *pstEvent = NULL;
	HUINT32		slStream = 0;
	HUINT8		*pszMsg = NULL;
	HCHAR		*pszEncodedStr = NULL, *pzProperties = NULL;

	if(pstContext == NULL)
	{
		HxLOG_Error("\r\n");
		return ERR_FAIL;
	}

	if(pstContext->hSession == HANDLE_NULL)
	{
		HxLOG_Error("ulSessionHandle (0x%x)\n", pstContext->hSession);
		return ERR_FAIL;
	}

	#if 1
	// TODO_VMX : FingerPrinting 관련해서 UI와 논의후 작업을 진행하여야한다.
	#endif

_ERROR:
	if(pstEvent)
		HLIB_STD_MemFree(pstEvent);

	if(pszEncodedStr != NULL)
		HLIB_STD_MemFree(pszEncodedStr);

	if(slStream != 0)
		HLIB_RWSTREAM_Close(slStream);
#endif
	return ERR_FAIL;
}

STATIC BUS_Result_t _xmgr_cas_VmxUi_FingerPrinting_MsgCreate(sVmxUi_FingerPrinting_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if(pstContext == NULL)
	{
		HxLOG_Error("\r\n");
		return ERR_BUS_MESSAGE_PASS;
	}

	HxSTD_memset(pstContext, 0x00, sizeof(sVmxUi_FingerPrinting_Context_t));

	pstContext->hSession = HANDLE_NULL;

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t _xmgr_cas_VmxUi_FingerPrinting_MsgDestroy(sVmxUi_FingerPrinting_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t _xmgr_cas_VmxUi_FingerPrinting_MsgEvtInputNotify(sVmxUi_FingerPrinting_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if(pstContext == NULL)
	{
		HxLOG_Error("\r\n");
		return ERR_BUS_MESSAGE_PASS;
	}

	#if 0 // TODO_VMX // 필요시 진행.
	DxCAS_MmiSessionType_e eSessionType = (DxCAS_MmiSessionType_e)p2;
	OxMgrCasUiUserInputNotify_t *pstUserInputNotify = (OxMgrCasUiUserInputNotify_t *)p3;

	HxLOG_Debug("\n");

	if(pstContext == NULL)
	{
		HxLOG_Error("[%s:%d] pstContext is NULL\n", __FUNCTION__, __LINE__);
		return ERR_BUS_MESSAGE_PASS;
	}
	#endif

	return ERR_BUS_MESSAGE_PASS;
}

STATIC BUS_Result_t _xmgr_cas_VmxUi_FingerPrinting_ServiceChanged(sVmxUi_FingerPrinting_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if(pstContext == NULL)
	{
		HxLOG_Error("\r\n");
		return ERR_BUS_MESSAGE_PASS;
	}

	if(pstContext->hSession != HANDLE_NULL)
	{
		xmgr_cas_VmxUtil_PostMessageSessionEventToOM(eDxCAS_SESSIONTYPE_EXTENSION, xproc_cas_VmxUiFingerPrinting, pstContext->hSession, 0, eMEVT_CASUI_REQ_CLOSE_SESSION);
	}

	_xmgr_cas_VmxUi_FingerPrinting_Free(pstContext->pstFingerPrinting);

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t _xmgr_cas_VmxUi_FingerPrinting_MsgEvtCasCmd(sVmxUi_FingerPrinting_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HBOOL		bDisplay = FALSE;
	VmxIptv_DisplayUi_t			*pstDisplayCmd = NULL;

	pstDisplayCmd = (VmxIptv_DisplayUi_t *)p2;

	if(pstContext == NULL || pstDisplayCmd == NULL)
	{
		HxLOG_Error("\r\n");
		return ERR_BUS_MESSAGE_PASS;
	}

	_xmgr_cas_VmxUi_FingerPrinting_Free(pstContext->pstFingerPrinting);

	pstContext->pstFingerPrinting = &pstDisplayCmd->item.stFingrPrinting;

	if(pstDisplayCmd->item.stFingrPrinting.usFormatLenght > 0 && pstDisplayCmd->item.stFingrPrinting.usLocalDataLength > 0)
	{
		bDisplay = TRUE;
	}

	HxLOG_Print("hSession(%d), usFormatLenght(%d), usLocalDataLength(%d), bDisplay(%d)\n",
		pstContext->hSession,
		pstDisplayCmd->item.stFingrPrinting.usFormatLenght,
		pstDisplayCmd->item.stFingrPrinting.usLocalDataLength,
		bDisplay);

	if(bDisplay == TRUE)
	{
		if(pstContext->hSession != HANDLE_NULL)
		{
			_xmgr_cas_VmxUi_FingerPrinting_SendMessage(pstContext);
		}
		else
		{
			xmgr_cas_VmxUtil_PostMessageSessionEventToOM(eDxCAS_SESSIONTYPE_EXTENSION, xproc_cas_VmxUiFingerPrinting, HANDLE_NULL, 0, eMEVT_CASUI_REQ_OPEN_SESSION);
		}
	}
	else
	{
		if(pstContext->hSession != HANDLE_NULL)
		{
			xmgr_cas_VmxUtil_PostMessageSessionEventToOM(eDxCAS_SESSIONTYPE_EXTENSION, xproc_cas_VmxUiFingerPrinting, pstContext->hSession, 0, eMEVT_CASUI_REQ_CLOSE_SESSION);
		}
	}

	return ERR_BUS_NO_ERROR;
}


STATIC BUS_Result_t _xmgr_cas_VmxUi_FingerPrinting_MsgOpenSession(sVmxUi_FingerPrinting_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	OxMgrCasSessionEvent_t	*pstSession = (OxMgrCasSessionEvent_t *)p3;

	if(pstContext == NULL || pstSession == NULL)
	{
		HxLOG_Error("\r\n");
		return ERR_BUS_MESSAGE_PASS;
	}

	pstContext->hSession = pstSession->hSession;

	xmgr_cas_VmxUtil_PostMessageSessionEventToOM(eDxCAS_SESSIONTYPE_EXTENSION, xproc_cas_VmxUiFingerPrinting, pstSession->hSession, pstSession->ulCasSlotNum, eMEVT_CASUI_SESSION_OPENED);

	if(_xmgr_cas_VmxUi_FingerPrinting_SendMessage(pstContext) != ERR_OK)
	{
		HxLOG_Error("\r\n");
	}

	return ERR_BUS_MESSAGE_PASS;
}

STATIC BUS_Result_t _xmgr_cas_VmxUi_FingerPrinting_MsgCloseSession(sVmxUi_FingerPrinting_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return ERR_BUS_MESSAGE_PASS;
	}

	_xmgr_cas_VmxUi_FingerPrinting_Free(pstContext->pstFingerPrinting);

	pstContext->hSession = HANDLE_NULL;

	return BUS_MGR_Destroy(NULL);
}

BUS_Result_t	xproc_cas_VmxUiFingerPrinting (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t eBusRes = ERR_BUS_NO_ERROR;
	sVmxUi_FingerPrinting_Context_t	*pstContext = _xmgr_cas_VmxUi_FingerPrinting_GetContext();

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
			eBusRes = _xmgr_cas_VmxUi_FingerPrinting_MsgCreate(pstContext, message, hAction, p1, p2, p3);
			break;

		case eMEVT_CAS_CMD:
			eBusRes = _xmgr_cas_VmxUi_FingerPrinting_MsgEvtCasCmd(pstContext, message, hAction, p1, p2, p3);
			break;

		case eOEVT_CAS_CASUI_APP_OPEN_SESSION:
			eBusRes = _xmgr_cas_VmxUi_FingerPrinting_MsgOpenSession(pstContext, message, hAction, p1, p2, p3);
			break;

		case eOEVT_CAS_CASUI_APP_CLOSE_SESSION:
			eBusRes = _xmgr_cas_VmxUi_FingerPrinting_MsgCloseSession(pstContext, message, hAction, p1, p2, p3);
			break;

		case eOEVT_CAS_CASUI_APP_USERINPUTDATA :
			eBusRes = _xmgr_cas_VmxUi_FingerPrinting_MsgEvtInputNotify(pstContext, message, hAction, p1, p2, p3);
			break;

		case eMEVT_LIVE_NOTIFY_SVC_CHANGED:
			eBusRes = _xmgr_cas_VmxUi_FingerPrinting_ServiceChanged(pstContext, message, hAction, p1, p2, p3);
			break;

		case eMEVT_BUS_DESTROY :
			eBusRes = _xmgr_cas_VmxUi_FingerPrinting_MsgDestroy(pstContext, message, hAction, p1, p2, p3);
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



#define ______________POPUP______________________________________________

STATIC sVmxUi_Popup_Context_t*	_xmgr_cas_VmxUi_Popup_GetContext(void)
{
	return &s_stCasVmxPopupContext;
}

STATIC void _xmgr_cas_VmxUi_Popup_FreePopupItem(VmxIptv_PopupItem_t *pstPopupItem)
{
	if(pstPopupItem != NULL)
	{
		if(pstPopupItem->ulPeriod)
			BUS_KillTimer(VMX_UI_TIMER_ID);

		if(pstPopupItem->pucMsg != NULL)
			OxCAS_Free(pstPopupItem->pucMsg);

		//OxCAS_Free(pstPopupItem);
	}
}

STATIC HERROR _xmgr_cas_VmxUi_Popup_SendPopupMessage(sVmxUi_Popup_Context_t *pstContext)
{
	OxMgrCasUiEvtInput_t *pstEvent = NULL;
	HUINT32		slStream = 0;
	HUINT8		*pszPopupMsg = NULL;
	HCHAR		*pszEncodedStr = NULL, *pzProperties = NULL;

	if(pstContext == NULL)
	{
		HxLOG_Error("\r\n");
		return ERR_FAIL;
	}

	if(pstContext->hSession == HANDLE_NULL)
	{
		HxLOG_Error("ulSessionHandle (0x%x)\n", pstContext->hSession);
		return ERR_FAIL;
	}

	pstEvent = (OxMgrCasUiEvtInput_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiEvtInput_t));
	if(pstEvent == NULL)
	{
		HxLOG_Error("HLIB_STD_MemCalloc() error\n");
		goto _ERROR;
	}

	VK_memset(pstEvent, 0, sizeof(OxMgrCasUiEvtInput_t));

	slStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
	if(slStream == 0)
	{
		HxLOG_Error("slStream is 0\n");
		goto _ERROR;
	}

	HLIB_RWSTREAM_Print(slStream, "[");

	switch(pstContext->eType)
	{
		case ePopup_Message:
			xmgr_cas_VmxUtil_ConvertLanguageCode(pstContext->pstPopupItem->pucMsg);

			MWC_UTIL_ConvSiStringToUtf8(NULL, MWC_UTIL_DvbStrlen(pstContext->pstPopupItem->pucMsg), pstContext->pstPopupItem->pucMsg, &pszPopupMsg);
			if(pszPopupMsg == NULL)
			{
				HxLOG_Error("pszPopupMsg is NULL\n");
				goto _ERROR;
			}

			pszEncodedStr = xmgr_cas_VmxUtil_EncodeString(pszPopupMsg);
			if(pszEncodedStr != NULL)
			{
				HLIB_RWSTREAM_Print(slStream, "\"%s\"", pszEncodedStr);
				HLIB_STD_MemFree(pszEncodedStr);
				pszEncodedStr = NULL;
			}

			pstEvent->stInputData.utInputData.stBtn.eMgrCasBtnType = eMgrCasBtn_1st;
			MWC_UTIL_DvbStrncpy(pstEvent->stInputData.utInputData.stBtn.aszButtonName[0], "[{\"id\":\"LOC_CLOSE_ID\"}]", MGR_CAS_STR_LENGTH_SHORT-1);
			break;

		default:
			HxLOG_Error("Unknown popup type (%d)\n", pstContext->eType);
			goto _ERROR;
	}

	HLIB_RWSTREAM_Print(slStream, "]");

	pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);

	pstEvent->stSessionInfo.eSessionType = eDxCAS_SESSIONTYPE_EXTENSION;
	pstEvent->stSessionInfo.hSession = pstContext->hSession;
	pstEvent->stSessionInfo.ulUiHandle = (HUINT32)xproc_cas_VmxUiPopup;

	pstEvent->stHeader.ulAttr = eMgrCasUiAttr_Subtitle;
	pstEvent->stInputData.eInputType = eMgrCasInputType_Button;

	MWC_UTIL_DvbStrncpy(pstEvent->stHeader.szSubTitle, pzProperties, MGR_CAS_STR_LENGTH_LONG - 1);

	HLIB_STD_MemFree(pzProperties);

	if(BUS_PostMessageToOM(NULL, eMEVT_CASUI_TYPE_INPUT_DATA, HANDLE_NULL, eDxCAS_GROUPID_VX, 0, (HINT32)pstEvent) != ERR_BUS_NO_ERROR)
	{
		HxLOG_Error("\n");
	}

	return ERR_OK;

_ERROR:
	if(pstEvent)
		HLIB_STD_MemFree(pstEvent);

	if(pszEncodedStr != NULL)
		HLIB_STD_MemFree(pszEncodedStr);

	if(slStream != 0)
		HLIB_RWSTREAM_Close(slStream);

	return ERR_FAIL;
}


STATIC BUS_Result_t _xmgr_cas_VmxUi_Popup_MsgCreate(sVmxUi_Popup_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if(pstContext == NULL)
	{
		HxLOG_Error("\r\n");
		return ERR_BUS_MESSAGE_PASS;
	}

	HxSTD_memset(pstContext, 0x00, sizeof(sVmxUi_Popup_Context_t));

	pstContext->hSession = HANDLE_NULL;

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t _xmgr_cas_VmxUi_Popup_MsgDestroy(sVmxUi_Popup_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t _xmgr_cas_VmxUi_Popup_MsgEvtInputNotify(sVmxUi_Popup_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if(pstContext == NULL)
	{
		HxLOG_Error("\r\n");
		return ERR_BUS_MESSAGE_PASS;
	}

	#if 0 // TODO_VMX // 필요시 진행.
	DxCAS_MmiSessionType_e eSessionType = (DxCAS_MmiSessionType_e)p2;
	OxMgrCasUiUserInputNotify_t *pstUserInputNotify = (OxMgrCasUiUserInputNotify_t *)p3;

	HxLOG_Debug("\n");

	if(pstContext == NULL)
	{
		HxLOG_Error("[%s:%d] pstContext is NULL\n", __FUNCTION__, __LINE__);
		return ERR_BUS_MESSAGE_PASS;
	}
	#endif

	return ERR_BUS_MESSAGE_PASS;
}

STATIC BUS_Result_t _xmgr_cas_VmxUi_Popup_ServiceChanged(sVmxUi_Popup_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if(pstContext == NULL)
	{
		HxLOG_Error("\r\n");
		return ERR_BUS_MESSAGE_PASS;
	}

	if(pstContext->hSession != HANDLE_NULL)
	{
		xmgr_cas_VmxUtil_PostMessageSessionEventToOM(eDxCAS_SESSIONTYPE_EXTENSION, xproc_cas_VmxUiPopup, pstContext->hSession, 0, eMEVT_CASUI_REQ_CLOSE_SESSION);
	}

	_xmgr_cas_VmxUi_Popup_FreePopupItem(pstContext->pstPopupItem);

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t _xmgr_cas_VmxUi_Popup_MsgEvtCasCmd(sVmxUi_Popup_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HBOOL		bDisplay = FALSE;
	VmxIptv_DisplayUi_t			*pstDisplayCmd = NULL;

	pstDisplayCmd = (VmxIptv_DisplayUi_t *)p2;

	if(pstContext == NULL || pstDisplayCmd == NULL)
	{
		HxLOG_Error("\r\n");
		return ERR_BUS_MESSAGE_PASS;
	}

	_xmgr_cas_VmxUi_Popup_FreePopupItem(pstContext->pstPopupItem);

	pstContext->pstPopupItem = &pstDisplayCmd->item.stPopupItem;

	// TODO_VMX : 현재는 다른 내용이 없으므로 ePopup_Message로 고정한다.
	if(pstDisplayCmd->eType == eVmxIptv_Display_OsdMessage)
	{
		pstContext->eType = ePopup_Message;
	}
	else
	{
		pstContext->eType = ePopup_Message;
	}

	if(pstDisplayCmd->item.stPopupItem.usLenght > 0 && pstDisplayCmd->item.stPopupItem.pucMsg != NULL)
	{
		bDisplay = TRUE;
	}

	HxLOG_Print("hSession(%d), DisplayType(%d), length(%d), bDisplay(%d)\n", pstContext->hSession, pstContext->eType, pstDisplayCmd->item.stPopupItem.usLenght, bDisplay);

	if(bDisplay == TRUE)
	{
		if(pstContext->hSession != HANDLE_NULL)
		{
			_xmgr_cas_VmxUi_Popup_SendPopupMessage(pstContext);
		}
		else
		{
			xmgr_cas_VmxUtil_PostMessageSessionEventToOM(eDxCAS_SESSIONTYPE_EXTENSION, xproc_cas_VmxUiPopup, HANDLE_NULL, 0, eMEVT_CASUI_REQ_OPEN_SESSION);
		}
	}
	else
	{
		if(pstContext->hSession != HANDLE_NULL)
		{
			xmgr_cas_VmxUtil_PostMessageSessionEventToOM(eDxCAS_SESSIONTYPE_EXTENSION, xproc_cas_VmxUiPopup, pstContext->hSession, 0, eMEVT_CASUI_REQ_CLOSE_SESSION);
		}
	}

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t _xmgr_cas_VmxUi_Popup_MsgEvtCasFail(sVmxUi_Popup_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HBOOL		bDisplay = FALSE;
	VmxIptv_DisplayUi_t			*pstDisplayCmd = NULL;

	pstDisplayCmd = (VmxIptv_DisplayUi_t *)p2;

	if(pstContext == NULL || pstDisplayCmd == NULL)
	{
		HxLOG_Error("\r\n");
		return ERR_BUS_MESSAGE_PASS;
	}

	_xmgr_cas_VmxUi_Popup_FreePopupItem(pstContext->pstPopupItem);

	pstContext->pstPopupItem = &pstDisplayCmd->item.stPopupItem;

	// TODO_VMX : 현재는 다른 내용이 없으므로 ePopup_Message로 고정한다.
	if(pstDisplayCmd->eType == eVmxIptv_Display_OsdMessage)
	{
		pstContext->eType = ePopup_Message;
	}
	else
	{
		pstContext->eType = ePopup_Message;
	}

	if(pstDisplayCmd->item.stPopupItem.usLenght > 0 && pstDisplayCmd->item.stPopupItem.pucMsg != NULL)
	{
		bDisplay = TRUE;
	}

	HxLOG_Print("hSession(%d), DisplayType(%d), length(%d), bDisplay(%d)\n", pstContext->hSession, pstContext->eType, pstDisplayCmd->item.stPopupItem.usLenght, bDisplay);

	if(bDisplay == TRUE)
	{
		if(pstContext->hSession != HANDLE_NULL)
		{
			_xmgr_cas_VmxUi_Popup_SendPopupMessage(pstContext);
		}
		else
		{
			xmgr_cas_VmxUtil_PostMessageSessionEventToOM(eDxCAS_SESSIONTYPE_EXTENSION, xproc_cas_VmxUiPopup, HANDLE_NULL, 0, eMEVT_CASUI_REQ_OPEN_SESSION);
		}
	}
	else
	{
		if(pstContext->hSession != HANDLE_NULL)
		{
			xmgr_cas_VmxUtil_PostMessageSessionEventToOM(eDxCAS_SESSIONTYPE_EXTENSION, xproc_cas_VmxUiPopup, pstContext->hSession, 0, eMEVT_CASUI_REQ_CLOSE_SESSION);
		}
	}

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t _xmgr_cas_VmxUi_Popup_MsgOpenSession(sVmxUi_Popup_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	OxMgrCasSessionEvent_t	*pstSession = (OxMgrCasSessionEvent_t *)p3;

	if(pstContext == NULL || pstSession == NULL)
	{
		HxLOG_Error("\r\n");
		return ERR_BUS_MESSAGE_PASS;
	}

	pstContext->hSession = pstSession->hSession;

	xmgr_cas_VmxUtil_PostMessageSessionEventToOM(eDxCAS_SESSIONTYPE_EXTENSION, xproc_cas_VmxUiPopup, pstSession->hSession, pstSession->ulCasSlotNum, eMEVT_CASUI_SESSION_OPENED);

	if(_xmgr_cas_VmxUi_Popup_SendPopupMessage(pstContext) != ERR_OK)
	{
		HxLOG_Error("\r\n");
	}

	return ERR_BUS_MESSAGE_PASS;
}

STATIC BUS_Result_t _xmgr_cas_VmxUi_Popup_MsgCloseSession(sVmxUi_Popup_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return ERR_BUS_MESSAGE_PASS;
	}

	_xmgr_cas_VmxUi_Popup_FreePopupItem(pstContext->pstPopupItem);

	pstContext->hSession = HANDLE_NULL;

	return BUS_MGR_Destroy(NULL);
}


BUS_Result_t	xproc_cas_VmxUiPopup (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t eBusRes = ERR_BUS_NO_ERROR;
	sVmxUi_Popup_Context_t	*pstContext = _xmgr_cas_VmxUi_Popup_GetContext();

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
			eBusRes = _xmgr_cas_VmxUi_Popup_MsgCreate(pstContext, message, hAction, p1, p2, p3);
			break;

		case eMEVT_CAS_CMD:
			eBusRes = _xmgr_cas_VmxUi_Popup_MsgEvtCasCmd(pstContext, message, hAction, p1, p2, p3);
			break;

		case eOEVT_CAS_CASUI_APP_OPEN_SESSION:
			eBusRes = _xmgr_cas_VmxUi_Popup_MsgOpenSession(pstContext, message, hAction, p1, p2, p3);
			break;

		case eOEVT_CAS_CASUI_APP_CLOSE_SESSION:
			eBusRes = _xmgr_cas_VmxUi_Popup_MsgCloseSession(pstContext, message, hAction, p1, p2, p3);
			break;

		case eOEVT_CAS_CASUI_APP_USERINPUTDATA :
			eBusRes = _xmgr_cas_VmxUi_Popup_MsgEvtInputNotify(pstContext, message, hAction, p1, p2, p3);
			break;

		case eMEVT_LIVE_NOTIFY_SVC_CHANGED:
			eBusRes = _xmgr_cas_VmxUi_Popup_ServiceChanged(pstContext, message, hAction, p1, p2, p3);
			break;

		case eMEVT_BUS_DESTROY :
			eBusRes = _xmgr_cas_VmxUi_Popup_MsgDestroy(pstContext, message, hAction, p1, p2, p3);
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

#define ______________BANNER______________________________________________

STATIC sVmxUi_ErrorMsg_Context_t*	_xmgr_cas_VmxUi_ErrorMsg_GetContext(void)
{
	return &s_stCasVmxErrorMsgContext;
}

STATIC void _xmgr_cas_VmxUi_ErrorMsg_Free(sVmxUi_ErrorMsg_Context_t *pContext)
{
	if(pContext != NULL)
	{
		if(pContext->pucMsg !=NULL)
		{
			OxCAS_Free(pContext->pucMsg);
			pContext->pucMsg= NULL;
		}
	}
}

STATIC BUS_Result_t _xmgr_cas_VmxUi_ErroMsg_MsgCreate(sVmxUi_ErrorMsg_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxSTD_memset(pstContext, 0, sizeof(sVmxUi_ErrorMsg_Context_t));

	pstContext->hSession = HANDLE_NULL;

	return ERR_BUS_MESSAGE_BREAK;
}

STATIC HERROR _xmgr_cas_VmxUi_SendBannerMessage(sVmxUi_ErrorMsg_Context_t *pstContext)
{
	HINT32	slStream = 0;
	HCHAR	*pzProperties = NULL;
	OxMgrCasUiEvtBanner_t *pstEvent = NULL;

	if(pstContext->hSession == HANDLE_NULL)
	{
		HxLOG_Error("pstContext->hSession is HANDLE_NULL\n");
		goto _ERROR;
	}

	if(pstContext->pucMsg == NULL)
	{
		HxLOG_Error("pstContext->pucMsg is NULL\n");
		goto _ERROR;
	}

	pstEvent = (OxMgrCasUiEvtBanner_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiEvtBanner_t));
	if(pstEvent == NULL)
	{
		HxLOG_Error("HLIB_STD_MemCalloc() error\n");
		goto _ERROR;
	}

	slStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
	if(slStream == 0)
	{
		HxLOG_Error("slStream is 0\n");
		goto _ERROR;
	}


	HLIB_RWSTREAM_Print(slStream, "[");
	HLIB_RWSTREAM_Print(slStream, "{\"id\":\"%s\"}", pstContext->pucMsg);
	HLIB_RWSTREAM_Print(slStream, "]");

	pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);

	pstEvent->stSessionInfo.eSessionType	= eDxCAS_SESSIONTYPE_BANNER;
	pstEvent->stSessionInfo.hSession		= pstContext->hSession;
	pstEvent->stSessionInfo.ulUiHandle		= (HUINT32)xproc_cas_VmxUiErrorMsg;

	MWC_UTIL_DvbStrncpy(pstEvent->stBanner.stString.szString, pzProperties, MGR_CAS_STR_LENGTH_LONG-1);
	pstEvent->stBanner.stString.szString[MGR_CAS_STR_LENGTH_LONG-1] = '\0';
	pstEvent->stBanner.stString.ulStringLength = SvcCas_UtilStrLen(pstEvent->stBanner.stString.szString);

	HxLOG_Debug("System Message Str = %s\n", pstEvent->stBanner.stString.szString);

	HLIB_STD_MemFree(pzProperties);

	if(BUS_PostMessageToOM(NULL, eMEVT_CASUI_TYPE_BANNER_DATA, HANDLE_NULL, (HINT32)eDxCAS_GROUPID_VX, 0, (HINT32)pstEvent) != ERR_BUS_NO_ERROR)
	{
		HxLOG_Error("BUS_PostMessageToOM() error\n");

		if(pstEvent)
			HLIB_STD_MemFree(pstEvent);
		return ERR_FAIL;
	}

	return ERR_OK;

_ERROR:
	if(slStream != 0)
		HLIB_RWSTREAM_Close(slStream);

	if(pstEvent != NULL)
		HLIB_STD_MemFree(pstEvent);

	return ERR_FAIL;
}

STATIC BUS_Result_t _xmgr_cas_VmxUi_ErrorMsg_MsgEvtCasFail(sVmxUi_ErrorMsg_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	VmxIptv_DisplayUi_t			*pstDisplayUi = NULL;
	VmxIptv_ErrorMessage_e 		eErrMsgType = eVmxIptv_ErrMsg_Max;

	HUINT8			*pErrCodeText;

	pstDisplayUi = (VmxIptv_DisplayUi_t *)p2;
	if(pstDisplayUi == NULL || pstDisplayUi->eType != eVmxIptv_Display_ErrorMessage)
	{
		return ERR_BUS_MESSAGE_BREAK;
	}
	eErrMsgType = pstDisplayUi->item.stErrorMsg.eErrMsg;
	pErrCodeText = OxAP_Malloc (TEXT_SIZE512);

	if (pErrCodeText == NULL)
	{
		return ERR_BUS_MESSAGE_BREAK;
	}
	else
	{
		HxSTD_memset(pErrCodeText, 0x00, TEXT_SIZE512);
	}

	// TODO_VMX : Error  Message 에 따라서 ui와 연계시켜 동작 시켜야함.
	switch(eErrMsgType)
	{
		case eVmxIptv_ErrMsg_DecodeInit:
			MWC_UTIL_DvbStrcpy(pErrCodeText, (HUINT8 *)"eVmxIptv_ErrMsg_DecodeInit");
			break;
		case eVmxIptv_ErrMsg_DecodeOk:
			MWC_UTIL_DvbStrcpy(pErrCodeText, (HUINT8 *)"eVmxIptv_ErrMsg_DecodeOk");
			break;
		case eVmxIptv_ErrMsg_DecodeFailurePinRequired:
			MWC_UTIL_DvbStrcpy(pErrCodeText, (HUINT8 *)"eVmxIptv_ErrMsg_DecodeFailurePinRequired");
			break;
		case eVmxIptv_ErrMsg_DecodeFailureKeyNotAvailable:
			MWC_UTIL_DvbStrcpy(pErrCodeText, (HUINT8 *)"eVmxIptv_ErrMsg_DecodeFailureKeyNotAvailable");
			break;
		case eVmxIptv_ErrMsg_DecodeFailureBlackout:
			MWC_UTIL_DvbStrcpy(pErrCodeText, (HUINT8 *)"eVmxIptv_ErrMsg_DecodeFailureBlackout");
			break;
		case eVmxIptv_ErrMsg_DecodeFailureOther:
			MWC_UTIL_DvbStrcpy(pErrCodeText, (HUINT8 *)"eVmxIptv_ErrMsg_DecodeFailureOther");
			break;
		case eVmxIptv_ErrMsg_DecodeFailureTA:
			MWC_UTIL_DvbStrcpy(pErrCodeText, (HUINT8 *)"eVmxIptv_ErrMsg_DecodeFailureTA");
			break;
		default:
			break;
	}

	pstContext->pucMsg = pErrCodeText;

	HxLOG_Print("Verimatrix System message [%s]\n", pstContext->pucMsg);

	if(pstContext->hSession != HANDLE_NULL)
	{
		HxLOG_Print("\n");
		if(_xmgr_cas_VmxUi_SendBannerMessage(pstContext) != ERR_OK)
		{
			HxLOG_Error("\n");
		}
	}
	// 항상 열려있으므로 여기는 들어오지 않을거라 예상됨
	else
	{
		HxLOG_Print("\n");
		xmgr_cas_VmxUtil_PostMessageSessionEventToOM(eDxCAS_SESSIONTYPE_BANNER, xproc_cas_VmxUiErrorMsg, HANDLE_NULL, 0, eMEVT_CASUI_REQ_OPEN_SESSION);
	}

	return ERR_BUS_MESSAGE_BREAK;
}

STATIC HERROR _xmgr_cas_VmxUi_SendBannerHideMessage(sVmxUi_ErrorMsg_Context_t *pstContext)
{
	OxMgrCasUiEvtBanner_t	*pstEvent;

	HxLOG_Debug("\n");

	if(pstContext->hSession == HANDLE_NULL)
	{
		// 앞전에서 session이 다 열려야 한다.
		HxLOG_Error("SessionHandle (0x%x)\n", pstContext->hSession);
		return ERR_FAIL;
	}

	pstEvent = (OxMgrCasUiEvtBanner_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiEvtBanner_t));
	if(pstEvent == NULL)
	{
		HxLOG_Error("HLIB_STD_MemCalloc() error\n");
		return ERR_FAIL;
	}

	pstEvent->stSessionInfo.eSessionType	= eDxCAS_SESSIONTYPE_BANNER;
	pstEvent->stSessionInfo.hSession		= pstContext->hSession;
	pstEvent->stSessionInfo.ulUiHandle		= (HUINT32)xproc_cas_VmxUiErrorMsg;

	/* Display할 String length를 0으로 할 경우 Banner 를 hide시킨다. */
	pstEvent->stBanner.stString.ulStringLength = 0;

	if(BUS_PostMessageToOM(NULL, eMEVT_CASUI_TYPE_BANNER_DATA, HANDLE_NULL, (HINT32)eDxCAS_GROUPID_VX, (HINT32)0, (HINT32)pstEvent) != ERR_BUS_NO_ERROR)
	{
		HxLOG_Error("BUS_PostMessage() error\n");
		if(pstEvent)
			HLIB_STD_MemFree(pstEvent);
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC BUS_Result_t _xmgr_cas_VmxUi_ErrorMsg_MsgOpenSession(sVmxUi_ErrorMsg_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	OxMgrCasSessionEvent_t	*pstSession = (OxMgrCasSessionEvent_t *)p3;

	pstContext->hSession = pstSession->hSession;

	xmgr_cas_VmxUtil_PostMessageSessionEventToOM(eDxCAS_SESSIONTYPE_BANNER, xproc_cas_VmxUiErrorMsg, pstSession->hSession, pstSession->ulCasSlotNum, eMEVT_CASUI_SESSION_OPENED);

	return ERR_BUS_MESSAGE_PASS;
}

STATIC BUS_Result_t _xmgr_cas_VmxUi_ErrorMsg_MsgCloseSession(sVmxUi_ErrorMsg_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	//1 TODO: System message session은 한번 열리면 절대로 닫히지 않는 Session으로 정의하였다.

	pstContext->hSession = HANDLE_NULL;

	// TODO: free할 것 하고...

	return BUS_MGR_Destroy(NULL);
}

BUS_Result_t	xproc_cas_VmxUiErrorMsg(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t eBusRes = ERR_BUS_NO_ERROR;
	sVmxUi_ErrorMsg_Context_t *pstContext = _xmgr_cas_VmxUi_ErrorMsg_GetContext();

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
			eBusRes = _xmgr_cas_VmxUi_ErroMsg_MsgCreate(pstContext, message, hAction, p1, p2, p3);
			break;

		case eMEVT_CAS_FAIL :
			eBusRes = _xmgr_cas_VmxUi_ErrorMsg_MsgEvtCasFail(pstContext, message, hAction, p1, p2, p3);
			break;

		case eMEVT_LIVE_NOTIFY_SVC_CHANGED:
		case eMEVT_CAS_OK:
			_xmgr_cas_VmxUi_SendBannerHideMessage(pstContext);
			break;

		case eMEVT_LIVE_NOTIFY_VIEW_STATE_CHANGED:
			//eBusRes = _xmgr_cas_NaUi_MsgLiveViewState(pstContext, message, hAction, p1, p2, p3);
			break;

		case eOEVT_CAS_CASUI_APP_OPEN_SESSION:
			eBusRes = _xmgr_cas_VmxUi_ErrorMsg_MsgOpenSession(pstContext, message, hAction, p1, p2, p3);
			break;

		case eOEVT_CAS_CASUI_APP_CLOSE_SESSION:
			eBusRes = _xmgr_cas_VmxUi_ErrorMsg_MsgCloseSession(pstContext, message, hAction, p1, p2, p3);
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

