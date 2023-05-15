/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <mgr_live.h>
#include <mgr_cas.h>

#include <xmgr_cas.h>
#include <xmgr_cas_res_str.h>

#include <namgr_main.h>

#include "../../local_include/_xmgr_cas_na_util.h"
#include "../../local_include/_xmgr_cas_na_adapt.h"
#include "../../local_include/_xmgr_cas_na.h"


#if 0 // 사용하지 않는 파일임..지워야함.
/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/



typedef struct
{
	HINT8					messageType;
	HUINT8					cas_na_entitle_message[512];//TEXT_SIZE512];
//	AP_DialogInstance_t		hApsUiDlgInstance;
//	GWM_Callback_t			hApsUiCallBack;
} sNaUI_SES_Context_t;


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC sNaUI_SES_Context_t		s_stSesUIContext;

/*******************************************************************/
/********************      Local Functions      ********************/
/*******************************************************************/
#define _____XMGR_CAS_NA_SES_UI_LOCAL_FUNCTION______________________________________________________

STATIC sNaUI_SES_Context_t*		xmgr_cas_NaSES_GetContext(void)
{
	return &s_stSesUIContext;
}

#if 0
STATIC BUS_Result_t	xmgr_cas_NaSES_SystemMessageProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	switch(message)
	{
	case eMEVT_BUS_CREATE :
		break;

	case eMEVT_BUS_DESTROY :
		break;

	default:
		break;
	}

	//return AP_StatusMessage_ProtoProc(message, hAction, p1, p2, p3);
}
#endif

STATIC BUS_Result_t	xmgr_cas_NaSES_ConfirmMessageProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t		eBusRes = ERR_BUS_NO_ERROR;
//	sNaUI_SES_Context_t	*pstContext = xmgr_cas_NaSES_GetContext();

	switch (message)
	{
	case eMEVT_BUS_CREATE :
		HxLOG_Print("MSG_GWM_CREATE [%x]\n", message);
		break;

	case eOEVT_CAS_CASUI_APP_USERINPUTDATA :
		//AP_KeyConv_UiCommon(&p1);
		switch (p1)
		{

		default:
			break;
		}
		break;
#if 0
	case MSG_DLG_CLICKED:
		eClickButton = (DLG_ClickedButton_t)p1;
		switch(eClickButton)
		{
		case eDLG_CLICKED_BTN_MIDDLE :
			xmgr_cas_NaUpdateCasMessage_Aps(eCAS_NA_UI_APS_UPDATE_OK);
			BUS_MGR_Destroy(NULL);
			break;

		default:
			break;
		}

		return MESSAGE_BREAK;
#endif
	case eMEVT_BUS_DESTROY :
		HxLOG_Print("eMEVT_BUS_DESTROY [%x]\n", message);
		break;
	}

	if(eBusRes != ERR_BUS_NO_ERROR)
	{
		return eBusRes;
	}

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
#define _____XMGR_CAS_NA_SES_UI_PUBLIC_FUNCTION______________________________________________________

BUS_Result_t	xproc_cas_NaSes(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	sNaUI_SES_Context_t* pContext = xmgr_cas_NaSES_GetContext();

	switch (message)
	{
	case eMEVT_BUS_CREATE :
		BUS_MGR_SetMgrName(BUS_MGR_GetThis(), "xproc_cas_NaSes");
		HxSTD_memset(pContext, 0x00, sizeof(sNaUI_SES_Context_t));
		//pContext->hApsUiCallBack = NULL;

		// to get message type and string from module of ap_core
		xmgr_cas_NaGetMessageTypeAndString_Aps(&pContext->messageType, &(pContext->cas_na_entitle_message[0]));
		HxLOG_Print("messageType[%x]\n", pContext->messageType);

		if (pContext->messageType == NA_CAS_APS_MSG_T_BLOCK)
		{
			p1 = (HINT32)&(pContext->cas_na_entitle_message[0]);
//			pContext->hApsUiCallBack = xmgr_cas_NaAPS_SystemMessageProc;
		}
		else if (pContext->messageType == NA_CAS_APS_MSG_T_DIALOG)
		{
#if 0
			AP_Dialog_SetDialog(&pContext->hApsUiDlgInstance, BUS_MGR_GetAppCallback(BUS_MGR_GetThis()),
								DLG_TYPE_NO_EXIT | DLG_OK, NULL, (HUINT8*)&(pContext->cas_na_entitle_message[0]));
#endif
//			pContext->hApsUiCallBack = xmgr_cas_NaAPS_ConfirmMessageProc;
		}
		break;

	case eMEVT_XMGR_CAS_NA_APS_MESSAGE_UPDATED :
		HxLOG_Print("eMEVT_XMGR_CAS_NA_APS_MESSAGE_UPDATED received!\n");

		BUS_MGR_Destroy(NULL);
		BUS_MGR_Create("xproc_cas_NaSes", APP_CAS_PRIORITY,
						xproc_cas_NaSes, hAction, 0, 0, 0);
		break;
#if 0
	case MSG_GWM_QUIT_UI :
		return MESSAGE_PASS;
#endif
	case eMEVT_BUS_DESTROY :
		break;

	default :
		break;
	}
#if 0
	if (pContext->hApsUiCallBack != NULL)
	{
		return pContext->hApsUiCallBack(message, hAction, p1, p2, p3);
	}
#endif

	BUS_MGR_Destroy(NULL);

	return ERR_FAIL;
}
#endif
