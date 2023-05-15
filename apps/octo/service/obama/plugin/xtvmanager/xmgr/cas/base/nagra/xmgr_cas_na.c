#include <octo_common.h>
#include <mgr_action.h>
#include <mgr_live.h>

#include <svc_pipe.h>
#include <svc_si.h>
#include <svc_cas.h>

#include <mgr_cas.h>
#include <xmgr_cas.h>
#include <xmgr_cas_res_str.h>
#include <xmgr_module_search.h>

#include <namgr_main.h>

#include "local_include/_xmgr_cas_na_util.h"
#include "local_include/_xmgr_cas_na_adapt.h"
#include "local_include/_xmgr_cas_na.h"


/*******************************************************************/
/********************      Definitions    **************************/
/*******************************************************************/
typedef struct
{
	HUINT32				ulSessionHandle;	// APPKIT���� �����ǰ� �ִ� Handle
	BUS_Callback_t		pfnUiCallback;
} XmgrCas_NaUiSession_t;

typedef struct
{
	Handle_t				hAction;
	HUINT32					ulSlotNum;
	XmgrCas_NaAlamType_e	eAlarmType;		// �ڵ����� �� �� ���� ����.
	XmgrCas_NaUiSession_t	stUiSession[eNaCasUiType_Max];
} XmgrCas_NaContext_t;

/* local macros using in proc msg  */
#define NA_CAS_PARAM_WARNING_REMOVE(arg1, arg2, arg3, arg4, arg5, arg6)	\
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
STATIC XmgrCas_NaContext_t			s_stNaCasContext;
#if defined (CONFIG_OP_SES)
STATIC HBOOL						s_bSCInserted = FALSE;
#endif

extern HERROR	OM_CAS_MMI_RegisterCasUiProc(DxCAS_GroupId_e eCasGrpId, BUS_Callback_t fnCasUiProc);

/*******************************************************************/
/********************      Local Functions      ********************/
/*******************************************************************/
#define _____XMGR_CAS_NA_LOCAL_FUNCTION______________________________________________________

STATIC XmgrCas_NaContext_t *_xmgr_cas_NaGetContext(void)
{
	return &s_stNaCasContext;
}

STATIC HBOOL _xmgr_cas_NaIsActiveCasSystemMessage(void)
{
	BUS_Callback_t	pfnUiCallback;

	pfnUiCallback = s_stNaCasContext.stUiSession[eNaCasUiType_SystemMessage].pfnUiCallback;

	return BUS_MGR_Get(pfnUiCallback) ? TRUE : FALSE;
}

STATIC HERROR _xmgr_cas_NaCheckAction(Handle_t hAction)
{
	if (SVC_PIPE_GetActionId(hAction) != MGR_ACTION_GetMainActionId())
		return ERR_FAIL;

/*	channel change�� �̷������ TSR session�� stop�� �Ǹ鼭 live CAS EVT�� �߻��� �Ѵ�.
	�̹� MGR�� ActionHandle�� ������ �� �����ε�, SVC�� ������ ActionHandle�� ������ �߻��� ��Ų��.
	���� ���� ����� CH change event ������ live CAS EVT�� �޾Ƽ� ó���ϸ� ��������(message ����̶� ����� ���� ����),
	�ϴ��� last version�� check �ϴ� ������� ������. ���߿� �ٸ� model���� �׽�Ʈ �Ŀ� ������ �ʿ��� ������ ���� */
#if defined(CONFIG_PROD_HD9000I) || defined(CONFIG_PROD_HGS1000S)
	if(SVC_PIPE_IsActionHandleLatest(hAction) != ERR_OK)
		return ERR_FAIL;
#endif

	return ERR_OK;
}

#if 0
STATIC HUINT8 _xmgr_cas_NaCheckCasId(HINT32 ulCasId, DxCAS_GroupId_e eGroupId)
{
	if (ulCasId == eGroupId)
		return TRUE;
	else
		return FALSE;
}
#endif

STATIC XmgrCas_NaUiType_e _xmgr_cas_Na_GetNaUiSessionType(XmgrCas_NaContext_t *pContext, DxCAS_MmiSessionType_e eUiSession, HUINT32 ulUiHandle)
{
	XmgrCas_NaUiType_e eNaUiSession = eNaCasUiType_Max;
	XmgrCas_NaUiType_e eIndex;

	switch(eUiSession)
	{
		case eDxCAS_SESSIONTYPE_BANNER:
			eNaUiSession = eNaCasUiType_SystemMessage;
			break;

		case eDxCAS_SESSIONTYPE_MENU:
			eNaUiSession = eNaCasUiType_CasMenu;
			break;

		case eDxCAS_SESSIONTYPE_EXTENSION:
			for(eIndex = 0 ; eIndex < eNaCasUiType_Max ; eIndex++)
			{
				if(pContext->stUiSession[eIndex].pfnUiCallback == (BUS_Callback_t)ulUiHandle)
				{
					eNaUiSession = eIndex;
					break;
				}
			}
			break;

		default:
			break;
	}

	return eNaUiSession;
}

STATIC BUS_Result_t		_xmgr_cas_Na_MsgOpenSession(XmgrCas_NaContext_t *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	OxMgrCasSessionEvent_t	*pstSession = (OxMgrCasSessionEvent_t *)p3;
	XmgrCas_NaUiType_e		eNaUiSession;

	if(pstSession->hSession == HANDLE_NULL)
	{
		HxLOG_Error("Invalid Session Handle\n");
		return ERR_BUS_MESSAGE_BREAK;
	}

	eNaUiSession = _xmgr_cas_Na_GetNaUiSessionType(pContext, pstSession->eSessionType, pstSession->ulUiHandle);
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
		// �Ʒ��� check �ϴ� ������ EXTENSION TYPE�� ��� OBAMA���� REUQEST �ϱ� ������ PROC�� ������ �����ؾ� �Ѵ�.
		// ������ ���۵Ǿ�� �ϴ� UI�� ���� �߻��� �� �ִ�.
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

STATIC BUS_Result_t		_xmgr_cas_Na_MsgCloseSession(XmgrCas_NaContext_t *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	OxMgrCasSessionEvent_t	*pstSession = (OxMgrCasSessionEvent_t *)p3;
	XmgrCas_NaUiType_e		eNaUiSession;

	eNaUiSession = _xmgr_cas_Na_GetNaUiSessionType(pContext, pstSession->eSessionType, pstSession->ulUiHandle);
	if(eNaUiSession == eNaCasUiType_Max)
	{
		if(BUS_MGR_Get((BUS_Callback_t)pstSession->ulUiHandle) != NULL)
		{
			return BUS_SendMessage((BUS_Callback_t)pstSession->ulUiHandle, message, hAction, p1, p2, p3);
		}

		HxLOG_Error("Invalid NAGRA UI Session\n");
		return ERR_BUS_MESSAGE_PASS;
	}

	if (BUS_MGR_Get(pContext->stUiSession[eNaUiSession].pfnUiCallback) != NULL)
	{
		// �ڵ尡 �̸� �Ǿ� �ִ� ���¶� �۾��� �� �ξ��µ�... �� Proc�� �ִµ� �ٽ� Proc�� �����ҷ��� ����?
		#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
		if((DxCAS_MmiSessionType_e)p2 == eDxCAS_SESSIONTYPE_MENU)
		{
		   BUS_Result_t hBusResult = BUS_MGR_Create("xproc_cas_NaUiSysInfo", APP_CAS_PRIORITY, pContext->stUiSession[eNaCasUiType_CasMenu].pfnUiCallback, hAction, p1, p2, p3);
		   if(hBusResult != ERR_BUS_NO_ERROR)
		   {
				HxLOG_Info("bus result %d\n", hBusResult);
		   }
		}
		#endif
		BUS_SendMessage(pContext->stUiSession[eNaUiSession].pfnUiCallback, message, hAction, p1, p2, p3);
	}

	pContext->stUiSession[eNaUiSession].ulSessionHandle = HANDLE_NULL;

	NA_CAS_PARAM_WARNING_REMOVE(pContext, message, hAction, p1, p2, p3);
	return ERR_BUS_MESSAGE_BREAK;
}

STATIC BUS_Result_t _xmgr_cas_Na_MsgUserInputData(XmgrCas_NaContext_t *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	DxCAS_MmiSessionType_e		eSessionType = (DxCAS_MmiSessionType_e)p2;
	OxMgrCasUiUserInputNotify_t *pstUserInputNotify = (OxMgrCasUiUserInputNotify_t *)p3;
	XmgrCas_NaUiType_e			eNaUiSession;

	eNaUiSession = _xmgr_cas_Na_GetNaUiSessionType(pContext, eSessionType, pstUserInputNotify->ulUiHandle);
	if(eNaUiSession == eNaCasUiType_Max)
	{
		if(BUS_MGR_Get((BUS_Callback_t)pstUserInputNotify->ulUiHandle) != NULL)
		{
			return BUS_SendMessage((BUS_Callback_t)pstUserInputNotify->ulUiHandle, message, hAction, p1, p2, p3);
		}

		HxLOG_Error("Invalid NAGRA UI Session\n");
		return ERR_BUS_MESSAGE_PASS;
	}

	if (BUS_MGR_Get(pContext->stUiSession[eNaUiSession].pfnUiCallback) != NULL)
	{
		BUS_SendMessage(pContext->stUiSession[eNaUiSession].pfnUiCallback, message, hAction, p1, p2, p3);
	}

	NA_CAS_PARAM_WARNING_REMOVE(pContext, message, hAction, p1, p2, p3);
	return ERR_BUS_MESSAGE_BREAK;
}

STATIC HERROR _xmgr_cas_NaCommandMSGProcess(XmgrCas_NaContext_t *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrCas_NaUiPopupCmd_e		popup_cmd;
	XmgrCas_NaUiPopupMode_e		popup_mode;
	XmgrCas_NaUiCmdType_e		eCmdType;
	HUINT8 						*pszMsg = NULL;
	NaCmdData_t					*pstNaCmd;
	XmgrCas_NaUiType_e		eNaUiSession;

	if((pstNaCmd = (NaCmdData_t *)p2) == NULL)
	{
		HxLOG_Error("pstNaCmd is NULL\n");
		return ERR_FAIL;
	}

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES) // BDC ADD
	if(xmgr_cas_NaCommandProcessing((void*)p2, &eCmdType) != ERR_OK)
	{
		HxLOG_Error("xmgr_cas_NaCommandProcessing error\n");
		return ERR_FAIL;
	}

	switch(eCmdType)
	{
		case eCAS_NA_BDCCMD:
			eNaUiSession = eNaCasUiType_Bdc;
			break;
		case eCAS_NA_POPUPCMD:
			eNaUiSession = eNaCasUiType_CaPopup;
			break;
		case eCAS_NA_IRDCMD:
		default:
			HxLOG_Error("Unknown CmdType (0x%x)\n", pstNaCmd->eCmdType);
			return ERR_FAIL;
	}

	#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	if(eCmdType == eCAS_NA_IRDCMD)
	{
		HxLOG_Warning("eCmdType is eCAS_NA_IRDCMD -- return\n");
		return ERR_FAIL;
	}
	#endif

	if(xmgr_cas_NaGetPopupData(&popup_cmd, &popup_mode, &pszMsg) != ERR_OK)
	{
		HxLOG_Error("xmgr_cas_NaGetPopupData is error\n");
		return ERR_FAIL;
	}

	#if 0 // ���������� Ȯ���� ����..
	// delete system message
	if (BUS_MGR_Get(pContext->pfnUiCallback[eNaCasUi_FnSystemMessage]) != NULL)
	{
		/* CAS OK�� ������ System message�� ��������� ����. */
		BUS_SendMessage(pContext->pfnUiCallback[eNaCasUi_FnSystemMessage], eMEVT_CAS_OK, hAction, p1, p2, p3);
		/* System message�� ��� Sesstion�� stop���� ���� length �� 0�� banner data�� ������ UI�� ������. */
		//_xmgr_cas_NaCasUiRequestSessionStop(pContext, eNaCasUi_SessionSystemMessage);
	}
	#endif

	p1 = (HINT32)popup_mode;
	p3 = (HINT32)popup_cmd;

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	if(eCmdType == eCAS_NA_BDCCMD)
	{
		p2 = (HINT32)pszMsg;
	}
	else
	{
		HxLOG_Error(">>> bypass p2(0x%x) \n", p2);
	}
#endif

	HxLOG_Error("eNaUiSession(%d), p1(%d), p3(%d), Msg = %s\n", eNaUiSession, p1, p3, p2);
#else
	switch(pstNaCmd->eCmdType)
	{
		case eCmd_PopUp:
			eNaUiSession = eNaCasUiType_CaPopup;
			break;

		case eCmd_Bdc:
			eNaUiSession = eNaCasUiType_Bdc;
			break;

		case eCmd_IrdCmd:
			return xmgr_cas_NaCommandProcessing((void*)p2, &eCmdType);

		default:
			HxLOG_Error("Unknown CmdType (0x%x)\n", pstNaCmd->eCmdType);
			return ERR_FAIL;
	}

#endif

	if(BUS_MGR_Get(pContext->stUiSession[eNaUiSession].pfnUiCallback) == NULL)
		BUS_MGR_Create(NULL, APP_CAS_PRIORITY, pContext->stUiSession[eNaUiSession].pfnUiCallback, hAction, p1, p2, p3);

	BUS_SendMessage(pContext->stUiSession[eNaUiSession].pfnUiCallback, message, hAction, p1, p2, p3);

	return ERR_OK;
}

STATIC BUS_Result_t		_xmgr_cas_NaMsgGwmCreate(XmgrCas_NaContext_t *pContext)
{
	HxSTD_memset(pContext, 0, sizeof(XmgrCas_NaContext_t));

#if	defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)
	pContext->stUiSession[eNaCasUiType_SystemMessage].pfnUiCallback	= xproc_cas_NaBannerSmartCardError_Mediaset;
#elif	defined(CONFIG_MW_CAS_NAGRA_OP_TIVU)
	pContext->stUiSession[eNaCasUiType_SystemMessage].pfnUiCallback	= xproc_cas_NaBannerSmartCardError_Tivu;
#else
	pContext->stUiSession[eNaCasUiType_SystemMessage].pfnUiCallback	= xproc_cas_NaUiSysMsg;
#endif

	pContext->stUiSession[eNaCasUiType_CasMenu].pfnUiCallback		= xproc_cas_NaUiSysInfo;

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_MAIL)
	pContext->stUiSession[eNaCasUiType_MailIcon].pfnUiCallback		= xproc_cas_NaUiMailIcon;
	pContext->stUiSession[eNaCasUiType_MailMessage].pfnUiCallback	= xproc_cas_NaUiMailMessage;
#endif

	pContext->stUiSession[eNaCasUiType_FingerPrint].pfnUiCallback	= NULL;
	pContext->stUiSession[eNaCasUiType_PinDialog].pfnUiCallback		= NULL;

	pContext->stUiSession[eNaCasUiType_PpvDialog].pfnUiCallback		= NULL;
	pContext->stUiSession[eNaCasUiType_PurchaseList].pfnUiCallback	= NULL;

#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
	pContext->stUiSession[eNaCasUiType_CaPopup].pfnUiCallback		= xproc_cas_NaUiPopup;
#endif
	pContext->stUiSession[eNaCasUiType_Bdc].pfnUiCallback			= xproc_cas_NaUiBdc;

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	pContext->stUiSession[eNaCasUiType_EntitlementMessage].pfnUiCallback			= xproc_cas_NaUiEntitlementMessage;
#endif

	if(OM_CAS_MMI_RegisterCasUiProc(eDxCAS_GROUPID_NA, xproc_cas_NaMain) != ERR_OK)
	{
		HxLOG_Error("OM_CAS_MMI_RegisterCasUiProc() error\n");
	}

	return ERR_BUS_NO_ERROR;
}

#if defined(CONFIG_OP_SES)
STATIC BUS_Result_t		_xmgr_cas_Na_MsgEvtCamRemoved(XmgrCas_NaContext_t *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Callback_t			pfnUiCallback;

	pfnUiCallback = pContext->stUiSession[eNaCasUiType_SystemMessage].pfnUiCallback;

	if (BUS_MGR_Get(pfnUiCallback) == NULL)
		BUS_MGR_Create(NULL, APP_CAS_PRIORITY, pfnUiCallback, hAction, p1, p2, p3);

	BUS_SendMessage(pfnUiCallback, message, hAction, p1, p2, p3);

	return ERR_BUS_MESSAGE_PASS;
}
#endif

#if defined (CONFIG_OP_SES)
STATIC BUS_Result_t		_xmgr_cas_Na_MsgEvtCasSCInserted(XmgrCas_NaContext_t *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if(s_bSCInserted == FALSE)
	{
		XMGR_SEARCH_LCNUPDATE_SimulcastPreferredList(message);
	}

	s_bSCInserted = TRUE;

	return ERR_BUS_NO_ERROR;
}
#endif

STATIC BUS_Result_t		_xmgr_cas_Na_MsgEvtCasCmd(XmgrCas_NaContext_t *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t	hRes = ERR_BUS_NO_ERROR;

	if (_xmgr_cas_NaCheckAction(hAction) != ERR_OK)
	{
		HxLOG_Error("_xmgr_cas_NaCheckAction() fail, p1(%x), p2(%x)\n", p1, p2);
		return ERR_BUS_MESSAGE_PASS;
	}

#if defined(CONFIG_MW_CAS_NAGRA_OP_TIVU)
	if (!xmgr_cas_NaCheckTivuService())
	{
		return ERR_BUS_MESSAGE_PASS;
	}
#endif

	if(_xmgr_cas_NaCommandMSGProcess(pContext, message, hAction, p1, p2, p3) != ERR_OK)
	{
		hRes = ERR_BUS_MESSAGE_BREAK;
	}

	NA_CAS_PARAM_WARNING_REMOVE(pContext, message, hAction, p1, p2, p3);
	return hRes;
}

STATIC BUS_Result_t		_xmgr_cas_Na_MsgEvtCasFailOrCheck(XmgrCas_NaContext_t *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
#if defined(CONFIG_OP_CAS_NAGRA_CAKTEST_PPT)
	BUS_Result_t			hRes = ERR_BUS_NO_ERROR;
#endif
	BUS_Callback_t			pfnUiCallback;
	XmgrCas_NaUiType_e	eNaUiSession;

#if 0		// not used
	HCHAR					*pszAppName;
	OxMgrCasUiEvtSession_t	stEvent;
#endif

	if (_xmgr_cas_NaCheckAction(hAction) != ERR_OK)
	{
		HxLOG_Error("_xmgr_cas_NaCheckAction() fail, p1(%x), p2(%x)\n", p1, p2);
		return ERR_BUS_MESSAGE_PASS;
	}

#if defined(CONFIG_MW_CAS_NAGRA_OP_TIVU)
	if (!xmgr_cas_NaCheckTivuService())
	{
		return ERR_BUS_MESSAGE_PASS;
	}
#endif

	pContext->hAction = hAction;

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	// cas check�� �ƴҰ�쿡�� alarmData�� Ȯ���ϵ��� �Ѵ�. alarm�� ���� kill �Ǵ� ����߻�.
	// TODO : ses���� Fail�� check�� �����ؼ� �ٽ� ��������.
	if(message != eMEVT_CAS_CHECK)
#endif
	xmgr_cas_NaAlarmDataProcessing((void*)p2, &pContext->eAlarmType);

#if defined(CONFIG_OP_CAS_NAGRA_CAKTEST_PPT)
	if (pContext->eAlarmType == eCAS_NA_CA_ACCESS_DENIED_DIALOG_REQUIRED)
	{
		STATIC HUINT8	szMsg[128];

		hRes = _xmgr_cas_NaRequestCasUiCustomSessionStart(pContext, eNaCasUiType_CaPopup, message, hAction, (HINT32)eCAS_NA_POPUP_MODE_PPT, (HINT32)szMsg, p3);

		return ERR_BUS_MESSAGE_BREAK;
	}
#endif

	switch(pContext->eAlarmType)
	{
	#if defined(CONFIG_MW_CAS_NAGRA_FEATURE_PARENTAL_CONTROL)
		case eCAS_NA_CA_ACCESS_DENIED_PARENTAL_CONTROL:
			return ERR_BUS_MESSAGE_BREAK;
	#endif

		default:
			eNaUiSession = eNaCasUiType_SystemMessage;
			break;
	}

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	if(message == eMEVT_CAS_CHECK)
	{
		eNaUiSession = eNaCasUiType_EntitlementMessage;
	}
	// SES�� SystemMsg�� EntitlementMsg�� ���� �и��ؼ� ���� ��Ŵ���� cas fail�� EntitlementMsg�� display�Ȱ�쿡�� close or hide�� ���־���Ѵ�.
	else if(message == eMEVT_CAS_FAIL)
	{
		if(BUS_MGR_Get(pContext->stUiSession[eNaCasUiType_EntitlementMessage].pfnUiCallback) != NULL)
		{
			BUS_SendMessage(pContext->stUiSession[eNaCasUiType_EntitlementMessage].pfnUiCallback, message, hAction, p1, p2, p3);
		}
	}
#endif

	pfnUiCallback = pContext->stUiSession[eNaUiSession].pfnUiCallback;

	if (BUS_MGR_Get(pfnUiCallback) == NULL)
		BUS_MGR_Create(NULL, APP_CAS_PRIORITY, pfnUiCallback, hAction, p1, p2, p3);

	return BUS_SendMessage(pfnUiCallback, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t _xmgr_cas_Na_MsgEvtCasOk(XmgrCas_NaContext_t *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (_xmgr_cas_NaCheckAction(hAction) != ERR_OK)
	{
		HxLOG_Error("_xmgr_cas_NaCheckAction() fail, p1(%x), p2(%x)\n", p1, p2);
		return ERR_BUS_MESSAGE_PASS;
	}

	if (BUS_MGR_Get(pContext->stUiSession[eNaCasUiType_SystemMessage].pfnUiCallback) != NULL)
		BUS_SendMessage(pContext->stUiSession[eNaCasUiType_SystemMessage].pfnUiCallback, message, hAction, p1, p2, p3);

	if (BUS_MGR_Get(pContext->stUiSession[eNaCasUiType_PinDialog].pfnUiCallback) != NULL)
		BUS_SendMessage(pContext->stUiSession[eNaCasUiType_PinDialog].pfnUiCallback, message, hAction, p1, p2, p3);

#if defined(CONFIG_OP_CAS_NAGRA_CAKTEST_PPT)
	if (BUS_MGR_Get(pContext->stUiSession[eNaCasUiType_CaPopup].pfnUiCallback) != NULL)
		BUS_SendMessage(pContext->stUiSession[eNaCasUiType_CaPopup].pfnUiCallback, message, hAction, p1, p2, p3);
#endif

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t _xmgr_cas_Na_MsgSvcChanged(XmgrCas_NaContext_t *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t	eBusResult = ERR_BUS_NO_ERROR;
#if 0		// not used
	HUINT32			ulActionId = SVC_PIPE_GetActionId(hAction);
#endif

	pContext->eAlarmType = 0;

/*	broadcast message�̱⿡ �� proc���� handling �����ϴ�.
	�ٸ� �𵨿��� �׽�Ʈ �Ŀ� �� �̻������ �����ص� ������ ��...*/
#if !defined(CONFIG_PROD_HD9000I)
	if (BUS_MGR_Get(pContext->stUiSession[eNaCasUiType_SystemMessage].pfnUiCallback) != NULL)
		BUS_SendMessage(pContext->stUiSession[eNaCasUiType_SystemMessage].pfnUiCallback, message, hAction, p1, p2, p3);

	if (BUS_MGR_Get(pContext->stUiSession[eNaCasUiType_PinDialog].pfnUiCallback) != NULL)
		BUS_SendMessage(pContext->stUiSession[eNaCasUiType_PinDialog].pfnUiCallback, message, hAction, p1, p2, p3);

	if (BUS_MGR_Get(pContext->stUiSession[eNaCasUiType_CaPopup].pfnUiCallback) != NULL)
		BUS_SendMessage(pContext->stUiSession[eNaCasUiType_CaPopup].pfnUiCallback, message, hAction, p1, p2, p3);
#endif

	return eBusResult;
}

#if 0
STATIC BUS_Result_t		xmgr_cas_NaMsgCasUiClose(XmgrCas_NaContext_t *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 ulUiHandle = (HUINT32)p1;
	HUINT32 ulUiStatus = 0x00000001;
	HUINT32 i;

	for(i = 0; i < eNaCasUi_FnMax; i ++)
	{
		if(ulUiHandle == (HUINT32)pContext->pfnUiCallback[i])
		{
			if (BUS_MGR_Get(pContext->pfnUiCallback[i]) != NULL)
			{
				BUS_MGR_Destroy(pContext->pfnUiCallback[i]);

				pContext->eActiveUiStatus &= ~(ulUiStatus << i);
				HxLOG_Print("OK! Destroy by OAPI Ui Close [0x%x] UI handle[0x%x]\n", message, ulUiHandle);
			}
			break;
		}
	}

	NA_CAS_PARAM_WARNING_REMOVE(pContext, message, hAction, p1, p2, p3);
	return ERR_BUS_MESSAGE_BREAK;
}
#endif

#if 0 //For test
STATIC HERROR _xmgr_cas_NaNotifyScRemovedEvent(void)
{
	HUINT8					*pszScRemoved;
	OxMgrCasUiEvtBanner_t	stEvent;

	pszScRemoved = xmgr_cas_NaGetMessage(eCAS_NA_CA_SMARTCARD_REMOVED);

	HxLOG_Print("Message : %s\n", pszScRemoved);
	pszScRemoved = "[NA] SC Removed!";

	HxSTD_memset(&stEvent, 0, sizeof(OxMgrCasUiEvtBanner_t));
	//stEvent.ulUiHandle = (HUINT32)xproc_cas_NaUiSysMsg;

	MWC_UTIL_DvbStrncpy(stEvent.stBanner.stString.szString, pszScRemoved, MGR_CAS_STR_LENGTH_LONG-1);
	stEvent.stBanner.stString.szString[MGR_CAS_STR_LENGTH_LONG-1] = '\0';
	stEvent.stBanner.stString.ulStringLength = SvcCas_UtilStrLen(stEvent.stBanner.stString.szString);

//	HLIB_LOG_Dump (&stEvent, sizeof(OxMgrCasUiEvtBanner_t), 0, 0);

#if 0 //1 To do �߰� ���� �ʿ�, compile�� ���� �ӽ÷� ���� 20130903
	_xmgr_cas_NaPostMessageSessionEventToOM (stEvent.eSessionType, stEvent.ulUiHandle, 0, eMEVT_CASUI_TYPE_BANNER_DATA);
#endif

	return ERR_OK;
}

STATIC HERROR _xmgr_cas_NaNotifyScRemovedEvent2(void)
{
	HUINT8					*pszMessage = "[NA] Long Long Variable Length Message\nThis format should be described in data object of PayloadDataAndList type\n";
	OxMgrCasUiEvtTextbox_t	stEvent;
	OxMgrCasUiTextbox_t		*pstTextbox;

	HxSTD_memset(&stEvent, 0, sizeof(OxMgrCasUiEvtTextbox_t));
	//stEvent.ulUiHandle = (HUINT32)xproc_cas_NaUiSysMsg;

	pstTextbox = &stEvent.stTextbox;

	pstTextbox->ulAttr = 0;
	pstTextbox->pszString = pszMessage;
	pstTextbox->ulStringLength = VK_strlen(pszMessage);

	HxLOG_Print("Message : %s\n", pstTextbox->pszString);
	HxLOG_Print("Textbox Message : %s\n", pstTextbox->pszString);
	HxLOG_Print("Textbox Message Length : %d\n", pstTextbox->ulStringLength);
	HxLOG_Print("Textbox Attr : %d\n", pstTextbox->ulAttr);

//	HLIB_LOG_Dump (&stEvent, sizeof(OxMgrCasUiEvtBanner_t), 0, 0);

#if 0 //1 To do �߰� ���� �ʿ�, compile�� ���� �ӽ÷� ���� 20130903
	_xmgr_cas_NaPostMessageSessionEventToOM (stEvent.eSessionType, stEvent.ulUiHandle, 0, eMEVT_CASUI_TYPE_TEXTBOX_DATA);
#endif

	return ERR_OK;
}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_FEATURE_PARENTAL_CONTROL)
STATIC BUS_Result_t _xmgr_cas_Na_EvtPgCheckRating(XmgrCas_NaContext_t *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if(xmgr_cas_Na_SetParentalControlLevel() != ERR_OK)
		HxLOG_Error("xmgr_cas_Na_SetParentalControlLevel() error.\n");

	return ERR_BUS_MESSAGE_PASS;
}

STATIC BUS_Result_t _xmgr_cas_Na_EvtPgPinCheckOk(XmgrCas_NaContext_t *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if(xmgr_cas_Na_SetParentalControl(pContext->hAction) != ERR_OK)
		HxLOG_Error("xmgr_cas_Na_SetParentalControl() error.\n");

	return ERR_BUS_MESSAGE_PASS;
}
#endif


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
#define _____XMGR_CAS_NA_PUBLIC_FUNCTION______________________________________________________
BUS_Result_t 	xproc_cas_NaMain(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t			eBusResult = ERR_BUS_NO_ERROR;
	//HUINT32					ulActionId = SVC_PIPE_GetActionId(hAction);
	XmgrCas_NaContext_t		*pContext = _xmgr_cas_NaGetContext();

#if defined(CONFIG_DEBUG)
	HUINT8 *pszMsg = xmgr_cas_NaGetMessageName(message);
	if(pszMsg != NULL)
	{
		HxLOG_Print("message(%s), ActId(%x), p1(%x), p2(%x), p3(%x)\n", pszMsg, hAction, p1, p2, p3);
	}
#endif

	switch(message)
	{
		case eMEVT_BUS_CREATE :
			eBusResult = _xmgr_cas_NaMsgGwmCreate(pContext);
#if defined(CONFIG_MW_CAS_NAGRA_PRM)
			BUS_MGR_Create("xproc_cas_NaPrmMain", APP_CAS_PRIORITY, xproc_cas_NaPrmMain, hAction, p1, p2, p3);
#endif
#if defined(CONFIG_SUPPORT_NOVA_APP) && defined(CONFIG_MW_CAS_NAGRA_PRM_VOD_PVR)
			BUS_MGR_Create("xproc_cas_NaPrmNovaMain", APP_CAS_PRIORITY, xproc_cas_NaPrmNovaMain, hAction, p1, p2, p3);
#endif
 			break;

		case eMEVT_CAS_UPDATE_KEYINFO:
#if defined(CONFIG_MW_CAS_NAGRA_PRM)
			if (BUS_MGR_Get(xproc_cas_NaPrmMain) != NULL)
			{
				BUS_SendMessage(xproc_cas_NaPrmMain, message, hAction, p1, p2, p3);
			}
#endif
#if defined(CONFIG_SUPPORT_NOVA_APP) && defined(CONFIG_MW_CAS_NAGRA_PRM_VOD_PVR)
			if (BUS_MGR_Get(xproc_cas_NaPrmNovaMain) != NULL)
			{
				BUS_SendMessage(xproc_cas_NaPrmNovaMain, message, hAction, p1, p2, p3);
			}
#endif
			break;
		case eMEVT_CAS_PRM_PVR_UPDATE_KEYINFO:
#if defined(CONFIG_SUPPORT_NOVA_APP) && defined(CONFIG_MW_CAS_NAGRA_PRM_VOD_PVR)
			if (BUS_MGR_Get(xproc_cas_NaPrmNovaMain) != NULL)
			{
				BUS_SendMessage(xproc_cas_NaPrmNovaMain, message, hAction, p1, p2, p3);
			}
#endif
			break;
#if 1 //For Test
		case eMEVT_CAS_SC_INSERTED:
			#if defined (CONFIG_OP_SES)
			eBusResult = _xmgr_cas_Na_MsgEvtCasSCInserted(pContext, message, hAction, p1, p2, p3);
			#endif
			break;
		case eMEVT_CAS_SC_REMOVED:
			#if defined (CONFIG_OP_SES)
			s_bSCInserted = FALSE;
			#endif
			//_xmgr_cas_NaNotifyScRemovedEvent();
			//_xmgr_cas_NaNotifyScRemovedEvent2();
			break;
#endif
#if defined(CONFIG_OP_SES)
		/* standby������ �� �־�� �ϹǷ� �׳� PASS
		    stanby mode������ �ٷ� live�� ��ȯ�� �ش� proc�� distory�Ǵ� ���������� live ��ȯ�� msg ó�� ���� �߻�.
		*/
		case eMEVT_BUS_READY_SHUTDOWN:
			return MESSAGE_PASS;

		case eMEVT_CAS_CAM_REMOVED:
			eBusResult = _xmgr_cas_Na_MsgEvtCamRemoved(pContext, message, hAction, p1, p2, p3);
			break;
#endif

		case eMEVT_CAS_CMD :
			eBusResult = _xmgr_cas_Na_MsgEvtCasCmd(pContext, message, hAction, p1, p2, p3);
			break;

		case eMEVT_CAS_FAIL :
		case eMEVT_CAS_CHECK :
			eBusResult = _xmgr_cas_Na_MsgEvtCasFailOrCheck(pContext, message, hAction, p1, p2, p3);
			break;

		// Receving eMEVT_CAS_OK message, all nagra cas windows have to close(except bdc window)
		case eMEVT_CAS_OK :
			eBusResult = _xmgr_cas_Na_MsgEvtCasOk(pContext, message, hAction, p1, p2, p3);
			break;

		// this message comes from watch tv(destory_ui function)
		case eMEVT_CAS_CLOSE :
			if (_xmgr_cas_NaCheckAction(hAction) != ERR_OK)
				return ERR_BUS_MESSAGE_PASS;

#if 0	//1 Ȯ�� �ʿ�. ���� ���� message�� Application���� ó���Ǿ�� �ҵ� �ѵ�...
			if (BUS_MGR_Get(pContext->pfnUiCallback[eNaCasUi_FnSystemMessage]) != NULL)
			{
				eBusResult = BUS_SendMessage(pContext->pfnUiCallback[eNaCasUi_FnSystemMessage], message, hAction, p1, p2, p3);
			}
#endif
			break;

		case eMEVT_LIVE_NOTIFY_SVC_CHANGED:
			eBusResult = _xmgr_cas_Na_MsgSvcChanged(pContext, message, hAction, p1, p2, p3);
			break;

		case eMEVT_XMGR_CAS_NA_CHANGE_PPV_OPTYPE :
			if (p1 == eDxCAS_GROUPID_NA)
			{
#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)
				if (p2 == eNAGRA_OP_TYPE_PANGEA)
				{
					pContext->pfnUiCallback[eNaCasUi_FnSystemMessage]	= xproc_cas_NaBannerSmartCardError_Pangea;
				}
#if defined(CONFIG_MW_CAS_NAGRA_OP_TIVU)
				else if (p2 == eNAGRA_OP_TYPE_TIVU)
				{
					pContext->pfnUiCallback[eNaCasUi_FnSystemMessage]	= xproc_cas_NaBannerSmartCardError_Tivu;
				}
#endif
				else
				{
					pContext->pfnUiCallback[eNaCasUi_FnSystemMessage]	= xproc_cas_NaBannerSmartCardError_Mediaset;
				}
#endif
			}
			break;

//		case MSG_GWM_QUIT_UI :
//			return ERR_BUS_MESSAGE_PASS;

#if 0
		case eMEVT_LIVE_NOTIFY_SVC_CHANGED :
			if (_xmgr_cas_NaCheckAction(hAction) != ERR_OK)
				return ERR_BUS_MESSAGE_PASS;

			xmgr_cas_NaMsgGwmDeleteDialog(pContext);
			xmgr_cas_NaMsgGwmDeleteMainProc(pContext);
			break;

		case eMEVT_PVR_NOTIFY_PLAYBACK_STARTED :
			if ((eViewType_PB == MGR_ACTION_GetType(ulActionId)) ||
				(eViewType_MOVIE_PB == MGR_ACTION_GetType(ulActionId)) )
			{
				xmgr_cas_NaMsgGwmDeleteDialog(pContext);
				xmgr_cas_NaMsgGwmDeleteMainProc(pContext);
			}
			break;
#endif
		case eMEVT_BUS_DESTROY :
			break;

		case eOEVT_CAS_CASUI_APP_OPEN_SESSION:
			eBusResult = _xmgr_cas_Na_MsgOpenSession(pContext, message, hAction, p1, p2, p3);
			break;

		case eOEVT_CAS_CASUI_APP_CLOSE_SESSION:
			eBusResult = _xmgr_cas_Na_MsgCloseSession(pContext, message, hAction, p1, p2, p3);
			break;

		case eOEVT_CAS_CASUI_APP_USERINPUTDATA:
			eBusResult = _xmgr_cas_Na_MsgUserInputData(pContext, message, hAction, p1, p2, p3);
			break;

#if defined(CONFIG_MW_CAS_NAGRA_FEATURE_PARENTAL_CONTROL)
		case eOEVT_PG_CHECK_RATING:
			eBusResult = _xmgr_cas_Na_EvtPgCheckRating(pContext, message, hAction, p1, p2, p3);
			break;

		case eMEVT_PG_PIN_CHECK_OK:
			eBusResult = _xmgr_cas_Na_EvtPgPinCheckOk(pContext, message, hAction, p1, p2, p3);
			break;
#endif

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

#if defined(CONFIG_MW_CAS_NAGRA_DALTEST) && defined(CONFIG_MW_CAS_NAGRA_OP_SES) // BDC_TEST
void xmgr_cas_NaCasUiRequestSessionStopFromUi(void)
{
}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
HERROR	xmgr_cas_NaRequestCasUiBdcSessionClose(void)
{
#if 0
	XmgrCas_NaContext_t 	*pContext = _xmgr_cas_NaGetContext();
	return ERR_FAIL;
#else
	return ERR_OK;
#endif
}
#endif

HERROR			xmgr_cas_NaInitialize(void)
{
	// to register callback function
	xmgr_cas_NaRegisterSystemInfoCallback();
	xmgr_cas_NaRegisterSmartcardInfoCallback();
#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)
	xmgr_cas_NaRegisterCardCreditCallback_Mediaset();
	xmgr_cas_NaRegisterTsCheckNotifyCallback_Mediaset();
	xmgr_cas_NaRegisterEventInfoNotifyCallback_Mediaset();
	xmgr_cas_NaRegisterPurchaseHistoryNotifyCallback_Mediaset();
	xmgr_cas_NaRegisterRechargeListNotifyCallback_Mediaset();
	xmgr_cas_NaRegisterOppvPurchaseListNotifyCallback_Mediaset();
	xmgr_cas_NaRegisterRechargeAlarmNotifyCallback_Mediaset();
	xmgr_cas_NaRegisterPackageListNotifyCallback_Mediaset();
	xmgr_cas_NaRegisterRequestPurchaseEventNotifyCallback_Mediaset();
	xmgr_cas_NaRegisterRequestPurchasePackageNotifyCallback_Mediaset();
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
// ���� �ʿ� by mslee
	xmgr_cas_NaCheckIrdCommand_Aps();
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_RCS)
#if	defined(CONFIG_MW_CAS_NAGRA_FEATURE_PPV)
	xmgr_cas_NaPpvInitialize();
	xmgr_cas_NaPpvRegisterCreditCallback();
	xmgr_cas_NaPpvRegisterPpvListCallback();
	xmgr_cas_NaPpvRegisterRechargeNotifyCallback();
	xmgr_cas_NaPpvRegisterRequestPurchaseEventNotifyCallback();
	xmgr_cas_NaPpvRegisterRequestEventNotifyCallback();
//	xmgr_cas_NaPpvRegisterPurchaseHistoryNotifyCallback(); 	// not used
#endif
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_MAIL)
	xmgr_cas_NaIrdCmdMailLoadMailMessages();
#endif

	return ERR_OK;
}


HERROR			xmgr_cas_NaDefaultSetting(void)
{
	// to initalize ird sequence data(eeprom)
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD)
	xmgr_cas_NaUtil_FactoryDefault();
#endif
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	xmgr_cas_NaDefaultSetting_Aps();
#endif

	return ERR_OK;
}

HBOOL xmgr_cas_NaIsActiveCasSystemMessage(void)
{
	return _xmgr_cas_NaIsActiveCasSystemMessage();
}

HERROR			xmgr_cas_NaTotTdtUpdated(void)
{
	HxLOG_Print("*** TOT/TDT Updated ***\n");

#if 0//defined(CONFIG_MW_CAS_NAGRA_OP_SES)
// ���� �ʿ� by mslee
	xmgr_cas_NaUpdateCasMessage_Aps(eCAS_NA_UI_APS_UPDATE_PRV);

	if (BUS_MGR_Get(xproc_cas_NaManagement_Aps) == NULL)
	{
		BUS_MGR_Create("xproc_cas_NaManagement_Aps", 2/* very low */,
						xproc_cas_NaManagement_Aps, (HINT32)NULL, 0, 0, 0);
	}
#endif
#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)
	CA_NA_CakReceivedTDT();

	if (BUS_MGR_Get(xproc_cas_NaManagement_Mediaset) == NULL)
	{
		BUS_MGR_Create("xproc_cas_NaManagement_Mediaset", 2/* very low */,
						xproc_cas_NaManagement_Mediaset, (HINT32)NULL, 0, 0, 0);
	}
#endif
#if defined(CONFIG_MW_CAS_NAGRA_OP_RCS)
	if (BUS_MGR_Get(xproc_cas_NaManagement_Rcs) == NULL)
	{
		BUS_MGR_Create("xproc_cas_NaManagement_Rcs", 2/* very low */,
						xproc_cas_NaManagement_Rcs, (HINT32)NULL, 0, 0, 0);
	}
#endif
#if defined(CONFIG_MW_CAS_NAGRA_OP_SPAIN_TDT)
	xmgr_cas_NaTdtEMMUpdate();
#endif

	return ERR_OK;
}

BUS_Callback_t xmgr_cas_Na_GetUiProc(XmgrCas_NaUiType_e eNaUiType)
{
	XmgrCas_NaContext_t *pstContext = _xmgr_cas_NaGetContext();

	if(eNaUiType >= eNaCasUiType_Max)
		return NULL;

	return pstContext->stUiSession[eNaUiType].pfnUiCallback;
}

XmgrCas_NaAlamType_e xmgr_cas_Na_GetAlarmType(void)
{
	return s_stNaCasContext.eAlarmType;
}

/********************************************************************
* For Debugging
********************************************************************/
#if defined (CONFIG_DEBUG)
HUINT8 *xmgr_cas_NaGetMessageName(HINT32 message)
{
	switch(message)
	{
		ENUM_TO_STR(eMEVT_CAS_OK);
		ENUM_TO_STR(eMEVT_CAS_FAIL);
		ENUM_TO_STR(eMEVT_CAS_CMD);
		ENUM_TO_STR(eMEVT_CAS_CHECK);

		ENUM_TO_STR(eMEVT_CAS_PPV_REQUEST);
		ENUM_TO_STR(eMEVT_CAS_PINCODE_REQUEST);
		ENUM_TO_STR(eMEVT_CAS_UDATAREAD_REQUEST);

		ENUM_TO_STR(eMEVT_CAS_SC_REMOVED);
		ENUM_TO_STR(eMEVT_CAS_SC_INSERTED);

		ENUM_TO_STR(eMEVT_CAS_DRM_SIGNALLED);
		ENUM_TO_STR(eMEVT_CAS_DRM_UNKNOWN);
		ENUM_TO_STR(eMEVT_CAS_DRM_NONE);

		ENUM_TO_STR(eMEVT_CAS_DMV_SIGNALLED);
		ENUM_TO_STR(eMEVT_CAS_DMV_UNKNOWN);
		ENUM_TO_STR(eMEVT_CAS_DMV_NONE);

		ENUM_TO_STR(eOEVT_CAS_CASUI_APP_OPEN_SESSION);
		ENUM_TO_STR(eOEVT_CAS_CASUI_APP_CLOSE_SESSION);
		ENUM_TO_STR(eOEVT_CAS_CASUI_APP_USERINPUTDATA);
		ENUM_TO_STR(eMEVT_LIVE_NOTIFY_SVC_CHANGED);
		ENUM_TO_STR(eMEVT_LIVE_NOTIFY_VIEW_STATE_CHANGED);

		ENUM_TO_STR(eMEVT_BUS_READY_SHUTDOWN);
		ENUM_TO_STR(eMEVT_BUS_GO_OPERATION);
		ENUM_TO_STR(eMEVT_ACTION_FINISHED_BATCH_ACTION_ITEMS);
#if defined(CONFIG_OP_SES)
		ENUM_TO_STR(eMEVT_CAS_CAM_REMOVED);		/* cam�� ���ŵǴ°�� ȭ�鿡 error msg�� dispplay�ϱ����� ���⿡ cam remove�� ���� */
#endif
		default:
			return NULL;
	}
}

void xmgr_cas_NaCmdInit(HxCMD_Handle_t hCmdHandle)
{
#if defined (CONFIG_MW_CAS_NAGRA_PRM)
	extern void xmgr_cas_NaPrmCmdInit(HxCMD_Handle_t hCmdHandle);

	xmgr_cas_NaPrmCmdInit(hCmdHandle);
#endif

}
#endif



