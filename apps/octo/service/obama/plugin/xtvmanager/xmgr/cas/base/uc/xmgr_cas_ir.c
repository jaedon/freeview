#if !defined(CONFIG_MW_CAS_IRDETO_UCAS)
#include <s3_cadrv.h>
#endif


#include <octo_common.h>

#include <db_param.h>

#include <svc_pipe.h>
#include <svc_cas.h>
#include <xsvc_si.h>

#include <mgr_playback.h>
#include <mgr_cas.h>

#include "local_include/_xmgr_cas_ir.h"
#include "local_include/_xmgr_cas_ir_adapt.h"
#include "local_include/_xmgr_cas_ir_util.h"
#include "local_include/_xmgr_cas_ir_ui.h"
#include "../../local_include/_xmgr_cas.h"

#include <ir_api.h>
#include <ir_evt.h>
#include <ir_msg.h>
#include <mgr_live.h>
#include <mgr_swup.h>
#include <mgr_action.h>
#include <ir_ctrl.h>
#include <mgr_appmgr.h>

//#include <_svc_cas_mgr_control.h>
//#include <_svc_cas_mgr_main.h>

#include <ctype.h>	/* for toupper(...) */
#if !defined(CONFIG_MW_CAS_IRDETO_UCAS)
#include "ir_fta_block.h"
#endif
#include <xmgr_swup.h>

#include <mgr_common.h>

#if defined(CONFIG_OTA_IRDETO)
#include <svc_si.h>
#endif

/*******************************************************************/
/********************      Definitions         **************************/
/*******************************************************************/

#define OTA_PROCESS_NORMAL	(0)
#define OTA_PROCESS_FORCED	(100)

typedef enum
{
	eIrCasUi_CasMenu		= 0x00000001,
	eIrCasUi_ErrorMessage 	= 0x00000002,
	eIrCasUi_MailIcon		= 0x00000004,
	eIrCasUi_MailMessage	= 0x00000008,
	eIrCasUi_IppvDialog		= 0x00000010,
	eIrCasUi_DecoderPvr		= 0x00000020,
} IR_CAS_UI_e;

typedef enum
{
	eIrCasUi_FnErrorMessage = 0,
	eIrCasUi_FnMenu,
	eIrCasUi_FnMailIcon,
	eIrCasUi_FnMailMessage,

	eIrCasUi_FnMax
} IR_CAS_UI_FN_e;


#define IR_CAS_UI_EXTENDED_SESSION_MAX	eIrCasUi_FnMax

typedef struct
{
	DxCAS_MmiSessionType_e			eUiSession;
	IR_CAS_UI_FN_e					eFunction;
	IR_CAS_UI_e						eUiState;
} IR_CAS_UI_Extended_Session_t;

typedef struct
{
	IR_CAS_UI_e						eActiveUiStatus;
	IR_CAS_UI_Extended_Session_t	*pstCasUiCustomSession;
	BUS_Callback_t					pfnUiCallback[eIrCasUi_FnMax];
} IR_CAS_UI_Context;

/* local macros using in proc msg  */
#define IR_CAS_PARAM_WARNING_REMOVE(arg1, arg2, arg3, arg4, arg5, arg6)	\
			{													\
				(void)arg1;										\
				(void)arg2;										\
				(void)arg3;										\
				(void)arg4;				 						\
				(void)arg5;				 						\
				(void)arg6;				 						\
			}


/*******************************************************************/
/********************      Static Varaibles      ********************/
/*******************************************************************/


STATIC IR_CAS_UI_Context				s_stIrCasUiContext;
STATIC IR_CAS_UI_Extended_Session_t		s_stIrCasUiExtendedSession[IR_CAS_UI_EXTENDED_SESSION_MAX] =
{
	{eDxCAS_SESSIONTYPE_BANNER,		eIrCasUi_FnErrorMessage,		eIrCasUi_ErrorMessage},
	{eDxCAS_SESSIONTYPE_MENU, 		eIrCasUi_FnMenu, 				eIrCasUi_CasMenu},
	{eDxCAS_SESSIONTYPE_EXTENSION,	eIrCasUi_FnMailIcon,			eIrCasUi_MailIcon},
	{eDxCAS_SESSIONTYPE_EXTENSION,	eIrCasUi_FnMailMessage, 		eIrCasUi_MailMessage},
};

/*******************************************************************/
/********************      Local Functions      ********************/
/*******************************************************************/
#define _____XMGR_CAS_IR_LOCAL_FUNCTION______________________________________________________

STATIC IR_CAS_UI_Context *xmgr_cas_ir_GetContextUI(void)
{
	return &s_stIrCasUiContext;
}

STATIC HUINT8 xmgr_cas_ir_CheckCasId(HINT32 ulCasId)
{
	if (ulCasId == eDxCAS_GROUPID_IR)
		return TRUE;
	else
		return FALSE;
}

STATIC HERROR xmgr_cas_ir_FindCasUiFnIndex(IR_CAS_UI_Context *pContext, DxCAS_MmiSessionType_e eUiSession, HUINT32 ulUiHandle, IR_CAS_UI_FN_e *peFnIndex, IR_CAS_UI_e *peUiState)
{
	HUINT32 i;

	HxLOG_Debug("ulUiHandle: 0x%x, eUiSession: %d \n", ulUiHandle, eUiSession);
	for(i = 0; i < IR_CAS_UI_EXTENDED_SESSION_MAX; i++)
	{
		HxLOG_Debug("eUiSession: %d, pfnUiCallback: 0x%x \n", pContext->pstCasUiCustomSession[i].eUiSession, pContext->pfnUiCallback[pContext->pstCasUiCustomSession[i].eFunction]);
		if( (pContext->pstCasUiCustomSession[i].eUiSession == eUiSession)
			&& (pContext->pfnUiCallback[pContext->pstCasUiCustomSession[i].eFunction] == ulUiHandle))
		{
			*peFnIndex = pContext->pstCasUiCustomSession[i].eFunction;
			*peUiState =  pContext->pstCasUiCustomSession[i].eUiState;
			HxLOG_Print("Finded Session eFunction[%d], eUiState[%d]\n", *peFnIndex, *peUiState);
			return ERR_OK;
		}
	}

	HxLOG_Critical("\n\n");
	return ERR_FAIL;
}

STATIC HERROR xmgr_cas_ir_CheckCasUiExtendedSessionStart(IR_CAS_UI_Context *pContext, HINT32 message, Handle_t hAction, OxMgrCasSessionEvent_t *pstSessionEvent)
{
	HERROR							hErr;
	IR_CAS_UI_FN_e					eFunction;
	IR_CAS_UI_e						eUiState;
	DxCAS_MmiSessionType_e			eSessionType;
	HUINT32 							ulCasSlotNum;
	DxCAS_GroupId_e					eCasMenuGroupId;
	Handle_t							hSession;
	HUINT32							ulUiHandle;

	ulCasSlotNum = pstSessionEvent->ulCasSlotNum;
	eSessionType= pstSessionEvent->eSessionType;
	eCasMenuGroupId = pstSessionEvent->eCasMenuGroupId;
	hSession = pstSessionEvent->hSession;
	ulUiHandle = pstSessionEvent->ulUiHandle;

	HxLOG_Debug("ulCasSlotNum: %d, eSessionType: %d, eCasMenuGroupId: 0x%x, hSession: 0x%x \n", ulCasSlotNum, eSessionType, eCasMenuGroupId, hSession);
	hErr = xmgr_cas_ir_FindCasUiFnIndex(pContext, eSessionType, ulUiHandle, &eFunction, &eUiState);

	if(hErr == ERR_FAIL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}


	if (BUS_MGR_Get(pContext->pfnUiCallback[eFunction]) == NULL)
	{
		BUS_MGR_Create("pContext->pfnUiCallback[eFunction]", APP_CAS_PRIORITY,
					pContext->pfnUiCallback[eFunction], hAction, ulCasSlotNum, eSessionType, eCasMenuGroupId);
	}

	pContext->eActiveUiStatus |= eUiState;

	BUS_PostData(pContext->pfnUiCallback[eFunction], message, hAction, 0, 0, pstSessionEvent, sizeof(OxMgrCasSessionEvent_t));

//	IR_CAS_PARAM_WARNING_REMOVE(pContext, message, hAction, p1, p2, p3);

	return ERR_OK;
}

STATIC HERROR xmgr_cas_ir_CheckCasUiExtendedSessionStop(IR_CAS_UI_Context *pContext, HINT32 message, Handle_t hAction, OxMgrCasSessionEvent_t *pstSessionEvent)
{
	HERROR						hErr;
	IR_CAS_UI_FN_e				eFunction;
	IR_CAS_UI_e					eUiState;

	DxCAS_MmiSessionType_e			eSessionType;
	HUINT32 							ulCasSlotNum;
	DxCAS_GroupId_e					eCasMenuGroupId;
	Handle_t							hSession;
	HUINT32							ulUiHandle;

	ulCasSlotNum = pstSessionEvent->ulCasSlotNum;
	eSessionType= pstSessionEvent->eSessionType;
	eCasMenuGroupId = pstSessionEvent->eCasMenuGroupId;
	hSession = pstSessionEvent->hSession;
	ulUiHandle = pstSessionEvent->ulUiHandle;

	hErr = xmgr_cas_ir_FindCasUiFnIndex(pContext, eSessionType, ulUiHandle, &eFunction, &eUiState);

	if(hErr == ERR_FAIL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if (BUS_MGR_Get(pContext->pfnUiCallback[eFunction]) != NULL)
	{

		BUS_PostData(pContext->pfnUiCallback[eFunction], message, hAction, 0, 0, pstSessionEvent, sizeof(OxMgrCasSessionEvent_t));

		pContext->eActiveUiStatus &= ~eUiState;
		HxLOG_Print("OK! Destroy by OAPI Session End. eFunction[0x%x][]\n", eFunction);
	}


	return ERR_OK;
}

STATIC BUS_Result_t		xmgr_cas_ir_MsgCasUiSessionStart(IR_CAS_UI_Context *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	OxMgrCasSessionEvent_t 			stSessionEvent;
	DxCAS_MmiSessionType_e			eSessionType;
	HUINT32 						ulCasSlotNum;
	DxCAS_GroupId_e					eCasMenuGroupId;
	Handle_t						hSession;
	HUINT32							ulUiHadle;

	HxSTD_memcpy(&stSessionEvent, (void*)p3, sizeof(OxMgrCasSessionEvent_t));
	ulCasSlotNum = stSessionEvent.ulCasSlotNum;
	eSessionType= stSessionEvent.eSessionType;
	eCasMenuGroupId = stSessionEvent.eCasMenuGroupId;
	hSession = stSessionEvent.hSession;
	ulUiHadle = stSessionEvent.ulUiHandle;


	if(eSessionType == eDxCAS_SESSIONTYPE_MENU)
	{
		if (BUS_MGR_Get(pContext->pfnUiCallback[eIrCasUi_FnMenu]) == NULL)
		{
			BUS_MGR_Create(NULL, APP_CAS_PRIORITY,
							pContext->pfnUiCallback[eIrCasUi_FnMenu], hAction, ulCasSlotNum, eSessionType, eCasMenuGroupId);

			pContext->eActiveUiStatus |= eIrCasUi_CasMenu;

			BUS_PostData(pContext->pfnUiCallback[eIrCasUi_FnMenu], message, hAction, 0, 0, &stSessionEvent, sizeof(OxMgrCasSessionEvent_t));

			HxLOG_Print("OK! Created by OAPI [0x%x][eMgrCasUiSession_Menu]\n", message);
		}
		else
		{
			BUS_PostData(pContext->pfnUiCallback[eIrCasUi_FnMenu], message, hAction, 0, 0, &stSessionEvent, sizeof(OxMgrCasSessionEvent_t));
		}
	}
	else if(eSessionType == eDxCAS_SESSIONTYPE_BANNER)
	{
		if (BUS_MGR_Get(pContext->pfnUiCallback[eIrCasUi_FnErrorMessage]) == NULL)
		{
			BUS_MGR_Create(NULL, APP_CAS_PRIORITY,
							pContext->pfnUiCallback[eIrCasUi_FnErrorMessage], hAction, ulCasSlotNum, eSessionType, eCasMenuGroupId);

			pContext->eActiveUiStatus |= eIrCasUi_ErrorMessage;

			BUS_PostData(pContext->pfnUiCallback[eIrCasUi_FnErrorMessage], message, hAction, 0, 0, &stSessionEvent, sizeof(OxMgrCasSessionEvent_t));

			HxLOG_Print("OK! Created by OAPI [0x%x][eMgrCasUiSession_Menu]\n", message);
		}
		else
		{
			BUS_PostData(pContext->pfnUiCallback[eIrCasUi_FnErrorMessage], message, hAction, 0, 0, &stSessionEvent, sizeof(OxMgrCasSessionEvent_t));
		}
	}
	else
	{
		if(xmgr_cas_ir_CheckCasUiExtendedSessionStart(pContext, message, hAction, &stSessionEvent) != ERR_OK)
		{
			HxLOG_Critical("\n\n");
		}
	}

	IR_CAS_PARAM_WARNING_REMOVE(pContext, message, hAction, p1, p2, p3);

	return ERR_BUS_MESSAGE_BREAK;

}

STATIC BUS_Result_t		xmgr_cas_ir_MsgCasUiSessionStop(IR_CAS_UI_Context *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	OxMgrCasSessionEvent_t 				stSessionEvent;
	DxCAS_MmiSessionType_e			eSessionType;
	HUINT32 							ulCasSlotNum;
	Handle_t							hSession;
	HUINT32							ulUiHadle;

	HxSTD_memcpy(&stSessionEvent, (void*)p3, sizeof(OxMgrCasSessionEvent_t));
	ulCasSlotNum = stSessionEvent.ulCasSlotNum;
	eSessionType= stSessionEvent.eSessionType;
	hSession = stSessionEvent.hSession;
	ulUiHadle = stSessionEvent.ulUiHandle;


	if(eSessionType == eDxCAS_SESSIONTYPE_MENU)
	{
		if (BUS_MGR_Get(pContext->pfnUiCallback[eIrCasUi_FnMenu]) != NULL)
		{
			BUS_PostData(pContext->pfnUiCallback[eIrCasUi_FnMenu], message, hAction, 0, 0, &stSessionEvent, sizeof(OxMgrCasSessionEvent_t));

			pContext->eActiveUiStatus &= ~eIrCasUi_CasMenu;
			HxLOG_Print("OK! Destroy by OAPI Session End [0x%x][eMgrCasUiSession_Menu]\n", message);
		}
	}
	else if(eSessionType == eDxCAS_SESSIONTYPE_BANNER)
	{
		if (BUS_MGR_Get(pContext->pfnUiCallback[eIrCasUi_FnErrorMessage]) != NULL)
		{
			BUS_PostData(pContext->pfnUiCallback[eIrCasUi_FnErrorMessage], message, hAction, 0, 0, &stSessionEvent, sizeof(OxMgrCasSessionEvent_t));

			pContext->eActiveUiStatus &= ~eIrCasUi_ErrorMessage;
			HxLOG_Print("OK! Destroy by OAPI Session End [0x%x][eMgrCasUiSession_Menu]\n", message);
		}
	}
	else
	{
		if(xmgr_cas_ir_CheckCasUiExtendedSessionStop(pContext, message, hAction, &stSessionEvent) != ERR_OK)
		{
			HxLOG_Critical("\n\n");
		}
	}

	IR_CAS_PARAM_WARNING_REMOVE(pContext, message, hAction, p1, p2, p3);

	return ERR_BUS_MESSAGE_BREAK;
}

STATIC BUS_Result_t		xmgr_cas_ir_MsgCasUiUserInputData(IR_CAS_UI_Context *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, const void *pvData)
{
	BUS_Result_t		eResult = ERR_BUS_INITIAL_FAILED;
	OxMgrCasUiUserInputNotify_t	*pstData = NULL;

	pstData = HLIB_STD_MemCalloc(sizeof(OxMgrCasUiUserInputNotify_t));
	if(pstData == NULL)
	{
		HxLOG_Error("pDATA is NULL \n");
		goto END_FUNC;
	}

	HLIB_STD_MemCpySafe(pstData, sizeof(OxMgrCasUiUserInputNotify_t), (const void *)pvData, sizeof(OxMgrCasUiUserInputNotify_t));

	if(p2 == eDxCAS_SESSIONTYPE_MENU)
	{
		if (BUS_MGR_Get(pContext->pfnUiCallback[eIrCasUi_FnMenu]) != NULL)
		{
			eResult = BUS_PostData(pContext->pfnUiCallback[eIrCasUi_FnMenu], message, hAction, p1, p2, pstData, sizeof(OxMgrCasUiUserInputNotify_t));
		}
	}

END_FUNC:

	if(pstData != NULL)
	{
		HLIB_STD_MemFree(pstData);
		pstData = NULL;
	}

	return ERR_BUS_MESSAGE_BREAK;
}

STATIC HINT32	xmgr_cas_ir_StrCaseCmp (const HUINT8 *szS1, const HUINT8 *szS2)
{
	HINT32	ch1;
	HINT32	ch2;

	do
	{
		ch1	= toupper(*szS1++);
		ch2	= toupper(*szS2++);
	} while (ch1 == ch2 && ch1 != 0);

	return ch1 - ch2;
}

 // TODO: shko 아래 msg들 중 일단 특정 config나 모델에서 사용하는 것들은 추후에 하고.
STATIC BUS_Result_t	xmgr_cas_ir_CommandMSGProcess(IR_CAS_UI_Context *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR				nErrorReturn = ERR_OK;
	IREVT_Data			*pEvtData;
	MAILBOX_TABLE		MailInfo;
	HUINT8				szPincode[5];
	BUS_Callback_t		pfnUiProc;
	HUINT32 			ulProperty;
	IR_GetMailBoxOutParam_t 	stMailBox;
	SvcCas_CtrlParam_t		stIrCtrlParam;
	IR_GetMailBoxOutParam_t stMailBoxParam;


	if (!xmgr_cas_ir_CheckCasId(p1))
	{
		HxLOG_Print("xmgr_cas_ir_CheckCasId error[%x]\n", p1);
		return ERR_FAIL;
	}

	pEvtData = (IREVT_Data*)p2;

	HxLOG_Print("xmgr_cas_ir_CommandMSGProcess --- p1[0x%x], p2[0x%x], p3[0x%x] \n", p1, p2, p3);

	switch (pEvtData->eIrEvtClass)
	{
		case	IREVT_CLASS_DECODER_MSG_FORCED_DISPLAY:
			stIrCtrlParam.ulControlType = eCACTRL_IR_GetMailBox;
			stIrCtrlParam.pvOut 	= &stMailBoxParam;
			stMailBoxParam.ucMailIndex = pEvtData->ExtData.ucMail_Index;
			stMailBoxParam.pstMailBox	= &MailInfo;

			SVC_CAS_CtrlGet(eDxCAS_GROUPID_IR, eCACTRL_IR_GetMailBox, &stIrCtrlParam);


			stMailBox.pstMailBox			= &MailInfo;
			stMailBox.ucMailIndex			= pEvtData->ExtData.ucMail_Index;

			HxLOG_Print("IREVT_CLASS_DECODER_MSG_FORCED_DISPLAY --- AttrFlag[%d], MsgType[%d], Flash[%d] \n", MailInfo.bAttributeMsg, MailInfo.Atrb_Display.ucMsg_Type, MailInfo.Atrb_Display.bFlashing);

			ulProperty = APP_DIALOGBOX_PRIORITY;


			if(MailInfo.bAttributeMsg == TRUE)	// Attributed Display
			{

				HxLOG_Print("Attributed Display Proc \n");

				if (MailInfo.Atrb_Display.ucMsg_Type == ATTRIBUTED_MESSAGE_TYPE_FINGERPRINT
				|| MailInfo.Atrb_Display.ucMsg_Type == ATTRIBUTED_MESSAGE_TYPE_FINGERPRINT_COVERT_OVERT_OPTION
				)
				{
					ulProperty = APP_CAS_HIGH_PRIORITY;
				}
				xmgr_cas_IrCreateUI("Attributed Message", ulProperty, eCasIr_UI_MailMessage, (HINT32)hAction, pEvtData->eIrEvtClass, (HINT32)pEvtData->ExtData.ucMail_Index, eDxCAS_SESSIONTYPE_EXTENSION);

				BUS_SendMessage(xmgr_cas_IrGetUiProc(eCasIr_UI_MailMessage), eMEVT_BUS_IRDETO_UPDATE_UI, hAction, pEvtData->eIrEvtClass, (HINT32)pEvtData->ExtData.ucMail_Index, eDxCAS_SESSIONTYPE_EXTENSION);

			}
			else
			{
				HxLOG_Print("Forced Mail Message Text Proc \n");
				xmgr_cas_IrCreateUI("Mail Message", ulProperty, eCasIr_UI_MailMessage, (HINT32)hAction, pEvtData->eIrEvtClass, (HINT32)pEvtData->ExtData.ucMail_Index, eDxCAS_SESSIONTYPE_EXTENSION);

				BUS_SendMessage(xmgr_cas_IrGetUiProc(eCasIr_UI_MailMessage), eMEVT_BUS_IRDETO_UPDATE_UI, (HINT32)hAction, pEvtData->eIrEvtClass, (HINT32)pEvtData->ExtData.ucMail_Index, eDxCAS_SESSIONTYPE_EXTENSION);
			}
			break;

		case	IREVT_CLASS_DECODER_MSG_NORMAL_MAIL:
			stIrCtrlParam.ulControlType = eCACTRL_IR_GetMailBox;
			stIrCtrlParam.pvOut 	= &stMailBoxParam;
			stMailBoxParam.ucMailIndex = pEvtData->ExtData.ucMail_Index;
			stMailBoxParam.pstMailBox	= &MailInfo;

			SVC_CAS_CtrlGet(eDxCAS_GROUPID_IR, eCACTRL_IR_GetMailBox, &stIrCtrlParam);

			stMailBox.pstMailBox			= &MailInfo;
			stMailBox.ucMailIndex			= pEvtData->ExtData.ucMail_Index;

			HxLOG_Print("IREVT_CLASS_DECODER_MSG_NORMAL_MAIL --- AttrFlag[%d], MsgType[%d], Flash[%d] \n", MailInfo.bAttributeMsg, MailInfo.Atrb_Display.ucMsg_Type, MailInfo.Atrb_Display.bFlashing);


			if((MailInfo.bAttributeMsg == TRUE) && (MailInfo.msg_class == EPG_MESSAGE_CLASS_FORCED_DISPLAY))	// Attributed Display
			{

				HxLOG_Print("Attributed Display Proc \n");

				if (MailInfo.Atrb_Display.ucMsg_Type == ATTRIBUTED_MESSAGE_TYPE_FINGERPRINT
				|| MailInfo.Atrb_Display.ucMsg_Type == ATTRIBUTED_MESSAGE_TYPE_FINGERPRINT_COVERT_OVERT_OPTION
				)
				{
					ulProperty = APP_CAS_HIGH_PRIORITY;
				}
				else
				{
					ulProperty = APP_CAS_PRIORITY;
				}
				xmgr_cas_IrCreateUI("Attributed Message", ulProperty, eCasIr_UI_MailMessage, (HINT32)hAction, pEvtData->eIrEvtClass, (HINT32)pEvtData->ExtData.ucMail_Index, eDxCAS_SESSIONTYPE_EXTENSION);

				BUS_SendMessage(xmgr_cas_IrGetUiProc(eCasIr_UI_MailMessage), eMEVT_BUS_IRDETO_UPDATE_UI, (HINT32)hAction, pEvtData->eIrEvtClass, (HINT32)pEvtData->ExtData.ucMail_Index, eDxCAS_SESSIONTYPE_EXTENSION);
			}
			else
			{
				HxLOG_Print("Normal Mail Message Icon Proc \n");
				xmgr_cas_IrCreateUI("Mail Icon", APP_CAS_HIGH_PRIORITY, eCasIr_UI_MailIcon, (HINT32)hAction, MailInfo.ucType, 0, eDxCAS_SESSIONTYPE_EXTENSION);

				BUS_SendMessage(xmgr_cas_IrGetUiProc(eCasIr_UI_MailIcon), eMEVT_BUS_IRDETO_UPDATE_UI, (HINT32)hAction, MailInfo.ucType, 0, eDxCAS_SESSIONTYPE_EXTENSION);
			}
			break;

		case	IREVT_CLASS_DECODER_MSG_ENHANCED_TEXT:
			HxLOG_Print("Attributed Display Proc \n");
			xmgr_cas_IrCreateUI("Attributed Message", APP_CAS_PRIORITY, eCasIr_UI_MailMessage, (HINT32)hAction, pEvtData->eIrEvtClass, (HINT32)pEvtData->ExtData.pucExt, eDxCAS_SESSIONTYPE_EXTENSION);

			BUS_SendMessage(xmgr_cas_IrGetUiProc(eCasIr_UI_MailMessage), eMEVT_BUS_IRDETO_UPDATE_UI, (HINT32)hAction, pEvtData->eIrEvtClass, (HINT32)pEvtData->ExtData.pucExt, eDxCAS_SESSIONTYPE_EXTENSION);
			break;


		case IREVT_CLASS_DECODER_MSG_PVR:
			HxLOG_Print("Attributed Display Proc -- IREVT_CLASS_DECODER_MSG_PVR - ulErrIndex(%d)\n", pEvtData->ExtData.ulErrIndex);
#if 0 // TODO: shko 일단 보류.

			eActionType = MGR_ACTION_GetType(SVC_PIPE_GetActionId(hAction));
			if (eActionType == eRecType_TSR_REC)
			{
				HxLOG_Print("TSR Recording - Ignore NO MSK Message!!!\n");
				break;
			}
			//	PVR Message, CAS_HIGH_PRIORITY
			xmgr_cas_IrCreateUI("IR Decoder PVR", APP_CAS_HIGH_PRIORITY, eCasIr_UI_DecoderPVR, (HINT32)hAction, pEvtData->eIrEvtClass, pEvtData->ExtData.ulErrIndex, eDxCAS_SESSIONTYPE_EXTENSION);

			BUS_SendMessage(xmgr_cas_IrGetUiProc(eCasIr_UI_DecoderPVR), eMEVT_BUS_IRDETO_UPDATE_UI, (HINT32)hAction, pEvtData->eIrEvtClass, pEvtData->ExtData.ulErrIndex, eDxCAS_SESSIONTYPE_EXTENSION);
#endif

			break;


		case	IREVT_CLASS_DECODER_MSG_ECM_FINGERPRINT:
		case	IREVT_CLASS_DECODER_MSG_EMM_FINGERPRINT:
			if(pEvtData->eIrEvtClass==IREVT_CLASS_DECODER_MSG_ECM_FINGERPRINT)
			{
				HxLOG_Print("[IREVT_CLASS_DECODER_MSG_ECM_FINGERPRINT]\n");
			}
			else
			{
				HxLOG_Print("[IREVT_CLASS_DECODER_MSG_EMM_FINGERPRINT]\n");
			}
									// fp
			pfnUiProc = xmgr_cas_IrGetUiProc(eCasIr_UI_MailMessage);
			if (pfnUiProc == NULL)
			{
				HxLOG_Print("pfnUiProc == NULL , break! \n");
				break;
			}

			//if(BUS_MGR_Get(pfnUiProc) != NULL && (xmgr_cas_IrMailMessageGetCurrentEvtClass()==IREVT_CLASS_DECODER_MSG_ECM_FINGERPRINT))
			{
				//BUS_MGR_Destroy(XPROC_CAS_IR_FP);
				/* PVR의 경우 Live/TSR 이 ECM을 각가 걸어서 두 개의 FP가 올라온다. 그래서 하나만 출력 하도록 수정 */
				// TODO: shko 고려해야할 사항
				//현재 web ui에서 session close를 내려주지 않는 상황.
				//그래서 7초 ecm fp가 두번 올라오면 (live, tsr 등) 두번 모두 web ui로 fp를 던질 것임.
				//이를 방지하기 위해서 어떻게 할까?
				//1. xmgr_cas_ir_ui 쪽에 7초 timeout을 둔다.
				//2. web ui에서 session close를 내려준다. 그럼 xmgr_cas_ir_ui 쪽의 proc이 사라질 테니~
				//	(fp뿐만 아니라 mail이나 banner 등도)
				// TODO: 만약 이 부분에서 mail message proc을 안 만드는 방향으로 web ap과 협의하게 되면, attr FP의 경우 여기서 해당 index의 mail 삭제해줘야 함.
				//HxLOG_Print("BUS_MGR_Get(pfnUiProc) != NULL && (xmgr_cas_IrMailMessageGetCurrentEvtClass()==IREVT_CLASS_DECODER_MSG_ECM_FINGERPRINT)\n");
				//BUS_PostMessage (pfnUiProc, eMEVT_BUS_TIMER, HANDLE_NULL, eCAS_IR_CA_EVT_TIME_RESET, 0, 0);
			}
//			else
			{
				HxLOG_Print("create eCasIr_UI_MailMessage proc for fingerprint\n");
				xmgr_cas_IrCreateUI("Finger Print", APP_CAS_HIGH_PRIORITY, eCasIr_UI_MailMessage, hAction, pEvtData->eIrEvtClass, (HINT32)pEvtData->ExtData.aucHashedCode, eDxCAS_SESSIONTYPE_EXTENSION);
			}
			BUS_SendMessage(xmgr_cas_IrGetUiProc(eCasIr_UI_MailMessage), eMEVT_BUS_IRDETO_UPDATE_UI, hAction, pEvtData->eIrEvtClass, (HINT32)pEvtData->ExtData.aucHashedCode, eDxCAS_SESSIONTYPE_EXTENSION);
			break;

		case	IREVT_CLASS_DECODER_MSG_CHANGE_PINCODE:
			HxLOG_Print("Change Pin Code (0x%x) \n", pEvtData->ExtData.ulPinCodeBCDType);
			if (pEvtData->ExtData.ulPinCodeBCDType >= 0x999A)
			{
				HxLOG_Print(">>>>>>>> New PIN CODE is Range Over\n");
				return	ERR_FAIL;
			}

			szPincode[5 - 1] = '\0';

			snprintf((char*)szPincode, 5, "%04X", pEvtData->ExtData.ulPinCodeBCDType);

			DB_PARAM_SetItem(eDB_PARAM_ITEM_USER_PINCODE, (HUINT32)(szPincode), 0);
			DB_PARAM_Sync();
			break;

#if	defined(CONFIG_OTA_IRDETO)
		case	IREVT_CLASS_DECODER_MSG_DOWNLOAD_ALLOWED_ENABLE:
			HxLOG_Print("SW Download Enable \n");
			XMGR_SWUP_SetControlByte_Irdeto(0x02);
			BUS_SendMessage(NULL, eMEVT_CAS_NOTIFICATION, hAction, eDxCAS_GROUPID_IR, 0, 0);
			break;
		case	IREVT_CLASS_DECODER_MSG_DOWNLOAD_ALLOWED_DISABLE:
			HxLOG_Print("SW Download Disable \n");
			XMGR_SWUP_SetControlByte_Irdeto(0);
			BUS_SendMessage(NULL, eMEVT_CAS_NOTIFICATION, hAction, eDxCAS_GROUPID_IR, 0, 0);
			break;
		case	IREVT_CLASS_DECODER_MSG_FORCED_DOWNLOAD:
			{
				HUINT8		ucMenuDisable, ucEmmDisable;

				XMGR_SWUP_CheckDisableIrdetoOta_Irdeto(&ucMenuDisable, &ucEmmDisable);
				if(ucEmmDisable == FALSE)
				{
					HxLOG_Print("Forced Download !!! \n");
#if 1
					XMGR_SWUP_SetForcedOtaFlag_Irdeto(1);
					BUS_PostMessageToOM(NULL, eMEVT_CAS_ACTION_UPDATE_START, HANDLE_NULL,
															(HUINT32)eDxCAS_GROUPID_IR,
															(HINT32)0,
															(HINT32)NULL);
#else	// apk_cas_action 을 통하여 event 가 가고, web ap에서 처리한다. web ap 구현 후 삭제.
					MGR_SWUP_SetSpec(eOTA_SPEC_IRDETO);
					MGR_SWUP_SetDefaultTp();
					MGR_SWUP_DetectCreate(0, HANDLE_NULL);
#endif

				}
			}
			break;
#endif

		case IREVT_CLASS_SC_NATIONALITY_CHANGED_BY_EMM :
			{
				HERROR						hErr;
				HUINT8						aucNationalCode[4];
				HBOOL						bSaveNationality;
				IR_API_SC_INFO				stIrApiScInfo;
				IR_GetSCInfoOutParam_t			stSCInfo;
				SvcCas_CtrlParam_t				stOutParam;

//				xmgr_cas_IrScInfo(0, &stIrApiScInfo);

				stOutParam.ulControlType	= eCACTRL_IR_SC_Info;
				stOutParam.pvOut			= &stSCInfo;
				stSCInfo.ucScSlotId 		= 0;//ucSlotId;
				stSCInfo.pstIrApiScInfo 	= &stIrApiScInfo;

				SVC_CAS_CtrlGet(eDxCAS_GROUPID_IR, eCACTRL_IR_SC_Info, &stOutParam);

				HxLOG_Print(">>>>>>>> New Nationality (%s)\n", stIrApiScInfo.szNationality);

				bSaveNationality = FALSE;
				if (SvcCas_UtilStrLen(stIrApiScInfo.szNationality) > 0)
				{
					xmgr_cas_IrSetNationalityAvailable(TRUE);
					// Read Nationality Code from DB
					hErr = DB_PARAM_GetItem(eDxSETUP_SYSTEM_IR_NATIONAL_CODE, (HUINT32 *)aucNationalCode, IR_SC_INFO_NATIONALITY_LENGTH);
					HxLOG_Print(">> Old (%s) 0x%x 0x%x 0x%x 0x%x => New Nationality (%s) 0x%x 0x%x 0x%x 0x%x\n", aucNationalCode, aucNationalCode[0], aucNationalCode[1], aucNationalCode[2], aucNationalCode[3], stIrApiScInfo.szNationality, *(stIrApiScInfo.szNationality+0), *(stIrApiScInfo.szNationality+1), *(stIrApiScInfo.szNationality+2), *(stIrApiScInfo.szNationality+3));
					if (hErr == ERR_OK)
					{
						if(xmgr_cas_ir_StrCaseCmp(aucNationalCode, stIrApiScInfo.szNationality) != 0)
						{
							bSaveNationality = TRUE;
						}
					}
					else
					{
						bSaveNationality = TRUE;
					}
				}
				else
				{
					xmgr_cas_IrSetNationalityAvailable(FALSE);
				}

				HxLOG_Print(">> bSaveNationality (%d)\n", bSaveNationality);
				if (bSaveNationality)
				{
					// Write Nationality Code to DB
					hErr = DB_PARAM_SetItem(eDxSETUP_SYSTEM_IR_NATIONAL_CODE, (HUINT32)stIrApiScInfo.szNationality, 0);
					if (hErr == ERR_OK)
					{
						DB_PARAM_Sync();
						HxLOG_Print("\t=> DB_PARAM_SetItem(MENUCFG_IR_NATIONAL_CODE, %s)\n", stIrApiScInfo.szNationality);
					}
					else
					{
						HxLOG_Print("\t=> DB_PARAM_SetItem(MENUCFG_IR_NATIONAL_CODE) Fail! (err:%x)\n", hErr);
					}
				}

				BUS_PostMessage(NULL, eMEVT_CAS_SYSINFO_UPDATED, (Handle_t)NULL, eDxCAS_GROUPID_IR, 0, 0);
			}
			break;

		case IREVT_CLASS_DECODER_MSG_FORCE_TUNE:
			{
				//Service Handle, Network ID, TS ID comes in	(IREVT_CLASS_DECODER_MSG_FORCE_TUNE)
				HxLOG_Print("\t=>IREVT_CLASS_DECODER_MSG_FORCE_TUNE\n");
				BUS_PostMessage(NULL, eSEVT_CAS_DECODE_FORCE_TUNE, (Handle_t)NULL,
								pEvtData->ExtData.stForceTuneParam.usNetworkId,
								pEvtData->ExtData.stForceTuneParam.usTsId,
								pEvtData->ExtData.stForceTuneParam.usSvcId);
			}
			break;

		case IREVT_CLASS_DECODER_MSG_RESET_IRD:
			HxLOG_Print("\t=>IREVT_CLASS_DECODER_MSG_RESET_IRD\n");
#if defined(CONFIG_MW_MM_PVR)
			if(MGR_ACTION_GetRecordCount() > 0) /* 녹화 중에는 이 message를 무시 */
			{
				break;
			}
#endif
			MGR_ACTION_RebootSystem();
			//BUS_PostMessage(NULL, eSEVT_CAS_DECODE_RESET_IRD, (Handle_t)NULL, 0, 0, 0);
			break;

		// TODO: shko CONFIG_OP_DIGITURK 일 경우만 처리해주면 될 듯.
		case IREVT_CLASS_DECODER_MSG_RESET_CH_ORDER:
			//Service Handle, Network ID, TS ID comes in	(IREVT_CLASS_DECODER_MSG_FORCE_TUNE)
			HxLOG_Print("\t=>IREVT_CLASS_DECODER_MSG_RESET_CH_ORDER\n");
			BUS_PostMessage(NULL, eSEVT_CAS_DECODE_RESET_CH_ORDER, (Handle_t)NULL, pEvtData->ExtData.stForceTuneParam.usBouquetId, 0, 0);
			break;

		// TODO: shko CONFIG_OP_DIGITURK 일 경우만 처리해주면 될 듯.
		case IREVT_CLASS_DECODER_MSG_RESCAN:
			HxLOG_Print("\t=>IREVT_CLASS_DECODER_MSG_RESCAN\n");
			BUS_PostMessage(NULL, eSEVT_CAS_DECODE_RESCAN, (Handle_t)NULL, 0, 0, 0);
			break;

#if	defined(CONFIG_MW_CAS_IRDETO_RECOVERY_RESCAN_HOME)
		case IREVT_CLASS_DECODER_MSG_RESCAN_FROM_HOME:
			{
				HERROR hErr;
				IR_SC_HOME_CHANNEL_INFO stHomingChInfo;

				if(HIR_OK != xsvc_cas_IrGetHomeChannelInfo(&stHomingChInfo))
				{
					HxLOG_Critical("\n\n");
				}
				else
				{
					hErr = xmgr_cas_IrUpdateHomingChannelInfo((void *)&stHomingChInfo);
					if(hErr != ERR_OK)
					{
						HxLOG_Critical("\n\n");
					}
				}
			}
			HxLOG_Print("\t=>IREVT_CLASS_DECODER_MSG_RESCAN_FROM_HOME\n");
			BUS_PostMessage(NULL, eSEVT_CAS_DECODE_RESCAN_FROM_HOME, (Handle_t)NULL, 0, 0, 0);
			break;
#endif

		// TODO: shko CONFIG_OP_DIGITURK 일 경우만 처리해주면 될 듯.
		case IREVT_CLASS_DECODER_MSG_RECONNECT:
			HxLOG_Print("\t=>IREVT_CLASS_DECODER_MSG_RECONNECT\n");
			BUS_PostMessage(NULL, eSEVT_CAS_DECODE_RECONNECT, (Handle_t)NULL, 0, 0, 0);
			break;

#if defined(IRPLAT_FN_DDEMM_USER_PAYLOAD_FOR_DIGITURK)
		case IREVT_CLASS_DECODER_MSG_CA_VIRTUAL_BILL:
			HxLOG_Print("\t=>IREVT_CLASS_DECODER_MSG_CA_VIRTUAL_BILL len[%d]\n", pEvtData->ExtData.stVirtualBillData.usLength);

			if(pEvtData->ExtData.stVirtualBillData.pucBillData)
			{
				OxCAS_Free(pEvtData->ExtData.stVirtualBillData.pucBillData);
			}
			/* Open TV applcation을 위한 것으로 현재 지원하지 않아되 됨. */
			break;
		case IREVT_CLASS_DECODER_MSG_CA_DM_PARAMETERS:
			HxLOG_Print("\t=>IREVT_CLASS_DECODER_MSG_CA_DM_PARAMETERS\n");
			//pEvtData->ExtData.stDMParameters;
			/* Data Mining Information을 보낼 정보가 포함되어 있음. Phase 2에서 지원 예정 */
			break;
		case IREVT_CLASS_DECODER_MSG_CA_FEATURE_BITMAP:
			HxLOG_Print("\t=>IREVT_CLASS_DECODER_MSG_CA_FEATURE_BITMAP\n");
			/*
				1. Digiturk 기본 시나리오는 Smartcard가 꽂혀져 있지 않은 상태에서 FTA service의 recording을 제한 한다.
				2. ulFeatureBitmap이 0x01일 경우에 Smartcard 없이 recording을 가능하게 한다.
				3. 이 DDEMM을 받기 전까지는 FTA service recoding을 Smartcard 삽입 상태에 따라 제한한다.
				4. ulFeatureBitmap 0x02는 Internel HDD PVR에서 Externel HDD로 contents copy 기능 제한을 위해 사용, 우리는 고려 하지 않는다.
			*/
			nErrorReturn = AP_SPEC_SetDigiturkCaFeatureBitmap(pEvtData->ExtData.ulCaFeatureBitmap);
			if(nErrorReturn != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");
			break;
		case IREVT_CLASS_DECODER_MSG_CA_IPPV_IP_AND_PHONE_NO:
			HxLOG_Print("\t=>IREVT_CLASS_DECODER_MSG_CA_IPPV_IP_AND_PHONE_NO\n");
			//pEvtData->ExtData.stIppvCallbackInfo;
			nErrorReturn = AP_SPEC_SetDigiturkIPPVCallbackInfo(pEvtData->ExtData.stIppvCallbackInfo.ulIpAddress, pEvtData->ExtData.stIppvCallbackInfo.usPort);
			if(nErrorReturn != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");
			break;
		case IREVT_CLASS_DECODER_MSG_CA_IPPV_DEBIT_LIMIT_THRESHOLD:
			HxLOG_Print("\t=>IREVT_CLASS_DECODER_MSG_CA_IPPV_DEBIT_LIMIT_THRESHOLD\n");
			//pEvtData->ExtData.ucThreshold;
			/* CAS API를 통해 지원 해야 함. 단, IPPV를 완료해야 하는 Phase2에서 기능 추가필요 */
			break;
		case IREVT_CLASS_DECODER_MSG_CA_IPPV_SCHEDULE:
			HxLOG_Print("\t=>IREVT_CLASS_DECODER_MSG_CA_IPPV_SCHEDULE\n");
			//pEvtData->ExtData.stIppvSchedule;
			/* IPPV 관련 정보를 report하는 schedule을 설정하는 것. 단, IPPV를 완료해야 하는 Phase2에서 기능 추가필요 */
			break;
		case IREVT_CLASS_DECODER_MSG_CA_REPORT_STATUS:
			HxLOG_Print("\t=>IREVT_CLASS_DECODER_MSG_CA_REPORT_STATUS\n");
			//pEvtData->ExtData.stDecorderReportStatus;
			/* Set의 Status를 reporting하는 동작. Data format은 PVR_Decoder_Status_Specification_1.1.pdf 참고. Phase 2에서 지원 추가 필요 */
			break;
		case IREVT_CLASS_DECODER_MSG_CA_SET_DOWNLOAD_TYPE:
			HxLOG_Print("\t=>IREVT_CLASS_DECODER_MSG_CA_SET_DOWNLOAD_TYPE[%d]\n", pEvtData->ExtData.stDownloadType.ucApplication);
			/*
				1. stDownloadType.ucApplication이 0x01(Test Mode)인 경우 BAT내 IRDETO download descriptor의 control byte가 3인 경우에 대해 처리 하도록 한다.
				2. 0인 경우에는 Normal하게 처리 하면 된다.
				3. stDownloadType.ucSystem은 고려하지 않는다.
			*/
			AP_OTA_IRDETO_SetDownloadType(pEvtData->ExtData.stDownloadType.ucApplication);
			break;

		case IREVT_CLASS_DECODER_MSG_EPG_DM_SETUP:
			HxLOG_Print("\t=>IREVT_CLASS_DECODER_MSG_CA_SET_DOWNLOAD_TYPE\n");
			//pEvtData->ExtData.stDMStatus;
			/* Data mining report에 관련된 것으로 현재 지원하지 않으며 Phase 2에서 지원 예정 */
			break;
		case IREVT_CLASS_DECODER_MSG_EPG_FORCED_STANDBY:
		{
			MgrAction_State_e		 eAmState;

			HxLOG_Print("\t=>IREVT_CLASS_DECODER_MSG_EPG_FORCED_STANDBY\n");

			eAmState = MGR_ACTION_GetState();
			if (eAmState != eAmState_ACTIVE_STANDBY)		// Active Standby Action Manager
			{
				// Standby 중에 Record Stop이 올라오면 Standby 로 간다.
				MGR_ACTION_RequestShutdown();
			}
		}
			break;
		case IREVT_CLASS_DECODER_MSG_EPG_DISPLAY_CARD_NUMBER:
			HxLOG_Print("\t=>IREVT_CLASS_DECODER_MSG_EPG_DISPLAY_CARD_NUMBER\n");
#if 0
			BUS_SendMessage(NULL, MSG_APPC2U_DDEMM_UI_DISPLAY_POPUP, HANDLE_NULL, (HINT32)eCasIr_DDEMM_UI_DisplayCardNo, (HINT32)&pEvtData->ExtData.stDisplayCardNo, 0);
#endif
			break;
		case IREVT_CLASS_DECODER_MSG_EPG_REMOTE_TAG:
			HxLOG_Print("\t=>IREVT_CLASS_DECODER_MSG_EPG_REMOTE_TAG\n");
#if 0
			BUS_SendMessage(NULL, MSG_APPC2U_DDEMM_UI_DISPLAY_POPUP, HANDLE_NULL, (HINT32)eCasIr_DDEMM_UI_RemoteTagging, (HINT32)&pEvtData->ExtData.stRemoteTagging, 0);
#endif
			break;
		case IREVT_CLASS_DECODER_MSG_EPG_REMOTE_RECORD:
			HxLOG_Print("\t=>IREVT_CLASS_DECODER_MSG_EPG_REMOTE_RECORD\n");
#if 0
			BUS_SendMessage(NULL, MSG_APPC2U_DDEMM_UI_DISPLAY_POPUP, HANDLE_NULL, (HINT32)eCasIr_DDEMM_UI_RemoteRecord, (HINT32)&pEvtData->ExtData.stRemoteRecording, 0);
#endif
			break;
		case IREVT_CLASS_DECODER_MSG_EPG_DEL_PVOD_EVENT:
			HxLOG_Print("\t=>IREVT_CLASS_DECODER_MSG_EPG_DEL_PVOD_EVENT\n");
			//pEvtData->ExtData.stDelPushVodEvent;
			/* Push VOD를 위한 것으로 현재 지원하지 않아되 됨. */
			break;
		case IREVT_CLASS_DECODER_MSG_EPG_CLEAR_3PA_DATA:
			HxLOG_Print("\t=>IREVT_CLASS_DECODER_MSG_EPG_CLEAR_3PA_DATA\n");
			//pEvtData->ExtData.stClear3PaData;
			/* Open TV applcation을 위한 것으로 현재 지원하지 않아되 됨. */
			break;
		case IREVT_CLASS_DECODER_MSG_EPG_COLOUR_DISPLAY:
			HxLOG_Print("\t=>IREVT_CLASS_DECODER_MSG_EPG_COLOUR_DISPLAY\n");
#if 0
			BUS_SendMessage(NULL, MSG_APPC2U_DDEMM_UI_DISPLAY_POPUP, HANDLE_NULL, (HINT32)eCasIr_DDEMM_UI_ColourDisplay, (HINT32)&pEvtData->ExtData.stColourDisplayData, 0);
#endif
			break;
		case IREVT_CLASS_DECODER_MSG_EPG_CS_PHONE_NO:
			HxLOG_Print("\t=>IREVT_CLASS_DECODER_MSG_EPG_CS_PHONE_NO\n");
			nErrorReturn = AP_SPEC_SetDigiturkCSPhoneNo(pEvtData->ExtData.stCSPhoneNo.pucPhoneNo);
			if(nErrorReturn != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");
			break;
		case IREVT_CLASS_DECODER_MSG_EPG_PPV_PHONE_NO:
			HxLOG_Print("\t=>IREVT_CLASS_DECODER_MSG_EPG_PPV_PHONE_NO\n");
			nErrorReturn = AP_SPEC_SetDigiturkPPVPhoneNo(pEvtData->ExtData.stPpvPhoneNo.pucPhoneNo);
			if(nErrorReturn != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");
			break;
		case IREVT_CLASS_DECODER_MSG_EPG_UPDATE_HDD:
			HxLOG_Print("\t=>IREVT_CLASS_DECODER_MSG_EPG_UPDATE_HDD\n");
			/*
				1. Internel HDD PVR model을 위한 것으로 현재 지원하지 않아되 됨.
				2. HDD고장 등으로 Internel HDD를 교체 한 후 이 Data를 받아야 사용 가능하도록 하는 기능임.
			*/
			break;
		case IREVT_CLASS_DECODER_MSG_EPG_FORMAT_HDD:
			HxLOG_Print("\t=>IREVT_CLASS_DECODER_MSG_EPG_FORMAT_HDD\n");
			/* 문의후 동작 추가 여부 결정 예정 */
			break;
		case IREVT_CLASS_DECODER_MSG_EPG_EPG_FEATURES:
			HxLOG_Print("\t=>IREVT_CLASS_DECODER_MSG_EPG_EPG_FEATURES\n");
			//pEvtData->ExtData.ulEpgFeatureBitmap;
			/* 문의한 결과를 받고 추가 여부 결정 예정 */
			break;
		case IREVT_CLASS_DECODER_MSG_EPG_USER_FOLDER_SETTINGS:
			HxLOG_Print("\t=>IREVT_CLASS_DECODER_MSG_EPG_USER_FOLDER_SETTINGS\n");
			//pEvtData->ExtData.usUserFolderSize;
			/* Open TV applcation을 위한 것으로 현재 지원하지 않아되 됨. */
			break;

		case IREVT_CLASS_DECODER_MSG_EPG_SET_OTHER_MODE:
		{
			HUINT32 ulSerialNo, ulForceSerialNo;
			HUINT8	pszCSSN1[9], pszCSSN2[9], pszCSSN3[9];

			HxLOG_Print("\t=>IREVT_CLASS_DECODER_MSG_EPG_SET_OTHER_MODE\n");

			if(SVC_SYS_GetChipId(&ulSerialNo) != ERR_OK)
			{
				HxLOG_Critical("\n\n");
				ulSerialNo = 0;
			}
			sprintf((char*)pszCSSN1, "%08X", ulSerialNo);

			HxSTD_memcpy(pszCSSN2, pEvtData->ExtData.stSetOtherMode.pucChipID, 8);
			pszCSSN2[8] = '\0';

			ulForceSerialNo = 0xFFFFFFFF;
			sprintf((char*)pszCSSN3, "%08X", ulForceSerialNo);

			HxLOG_Print("\t=>CSSN\n");
			HxLOG_Print("\t=>System 0x%s\n", pszCSSN1);
			HxLOG_Print("\t=>DDEMM	0x%s\n", pszCSSN2);
			HxLOG_Print("\t=>Forced 0x%s\n", pszCSSN3);

#if !defined(WIN32)
			if((xmgr_cas_ir_StrCaseCmp(pszCSSN1, pszCSSN2) == 0)
				|| (xmgr_cas_ir_StrCaseCmp(pszCSSN2, pszCSSN3) == 0))
#endif
			{
				nErrorReturn = AP_SPEC_ChangeOthermodeFlag(pEvtData->ExtData.stSetOtherMode.ucEnableOtherMode);
				if(nErrorReturn != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");
			}
		}
			break;
		case IREVT_CLASS_DECODER_MSG_EPG_EASY_SMS_DISPLAY:
			HxLOG_Print("\t=>IREVT_CLASS_DECODER_MSG_EPG_EASY_SMS_DISPLAY\n");
#if 0
			BUS_SendMessage(NULL, MSG_APPC2U_DDEMM_UI_DISPLAY_POPUP, HANDLE_NULL, (HINT32)eCasIr_DDEMM_UI_EasySMS, (HINT32)&pEvtData->ExtData.stEasySmsDisplayData, 0);
#endif
			break;
#if 0
		case IREVT_CLASS_DECODER_MSG_EPG_REMOTE_TAG_DS265:
			HxLOG_Print("\t=>IREVT_CLASS_DECODER_MSG_EPG_REMOTE_TAG_DS265\n");
			/* 문의한 결과를 받고 추가 여부 결정 예정 */
			break;
#endif
		case IREVT_CLASS_DECODER_MSG_EPG_DIGIAKTIF:
			HxLOG_Print("\t=>IREVT_CLASS_DECODER_MSG_EPG_DIGIAKTIF\n");
			//pEvtData->ExtData.stDigiaktifTagging;
			/* 문의한 결과를 받고 추가 여부 결정 예정 */
			break;
		case IREVT_CLASS_DECODER_MSG_EPG_SET_FTA_LIMIT:
		{
			HERROR	hErr;
			HUINT32 ulSerialNo, ulForceSerialNo;
			HUINT8	pszCSSN1[9], pszCSSN2[9], pszCSSN3[9];

			HxLOG_Print("\t=>IREVT_CLASS_DECODER_MSG_EPG_SET_FTA_LIMIT\n");

			if(SVC_SYS_GetChipId(&ulSerialNo) != ERR_OK)
			{
				HxLOG_Critical("\n\n");
				ulSerialNo = 0;
			}
			sprintf((char*)pszCSSN1, "%08X", ulSerialNo);

			HxSTD_memcpy(pszCSSN2, pEvtData->ExtData.stFtaLimit.pucChipID, 8);
			pszCSSN2[8] = '\0';

			ulForceSerialNo = 0xFFFFFFFF;
			sprintf((char*)pszCSSN3, "%08X", ulForceSerialNo);

			HxLOG_Print("\t=>CSSN\n");
			HxLOG_Print("\t=>System 0x%s\n", pszCSSN1);
			HxLOG_Print("\t=>DDEMM	0x%s\n", pszCSSN2);
			HxLOG_Print("\t=>Forced 0x%s\n", pszCSSN3);

#if !defined(WIN32)
			if((xmgr_cas_ir_StrCaseCmp(pszCSSN1, pszCSSN2) == 0)
				|| (xmgr_cas_ir_StrCaseCmp(pszCSSN2, pszCSSN3) == 0))
#endif
			{
				hErr = AP_FTALIMIT_SetFtaLimitData(pEvtData->ExtData.stFtaLimit.ucEnableFtaLimit, pEvtData->ExtData.stFtaLimit.ucDayLimit);
				if(hErr != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");
			}
		}
			break;
#endif

#if defined(CONFIG_OP_UPC)
		case IREVT_CLASS_DECODER_MSG_EPG_CS_PHONE_NO:
			ApCasIrPrint(DBG_PRINT, ("\t=>IREVT_CLASS_DECODER_MSG_EPG_CS_PHONE_NO\n"));
			nErrorReturn = AP_SPEC_SetUpcCSPhoneNo(pEvtData->ExtData.stCSPhoneNo.eCountryId, pEvtData->ExtData.stCSPhoneNo.pucPhoneNo);
			ApCasIrAssert(nErrorReturn == ERR_OK);
			break;

		case IREVT_CLASS_DECODER_MSG_USAGE_ID:
		{
			ApCasIrPrint(DBG_PRINT, ("\t=>IREVT_CLASS_DECODER_MSG_USAGE_ID (%x)\n", pEvtData->ExtData.ucUsageId));
			MW_SYS_SetUsageId(pEvtData->ExtData.ucUsageId);
			break;
		}
#endif

#if defined(CONFIG_MW_CAS_IRDETO_SOL)
		case IREVT_CLASS_SOL_STATUS_CHANGED:
			HxLOG_Print("xproc_cas_IrMain eMEVT_CAS_CMD! IREVT_CLASS_SOL_STATUS_CHANGED !\n");
			//BUS_PostMessage(OxMGR_LIVE_GetLiveViewProc(SVC_PIPE_GetActionId(hAction)), MSG_APP_NOTIFY_IR_SOL_CHECK, (Handle_t) NULL, 0, 0, 0);
			BUS_PostMessage(NULL, MSG_APP_NOTIFY_IR_SOL_CHECK, SVC_PIPE_GetActionId(hAction), 0, 0, 0);
			return MESSAGE_BREAK;
#endif

#if defined(CONFIG_MW_CAS_IRDETO_SCELL)
		 // TODO: shko CONFIG_MW_CAS_IRDETO_FTA_BLOCK 일 경우 체크해주도록 구현해야함.
		case IREVT_CLASS_FTA_BLOCK_STATUS_CHANGED:
			HxLOG_Print("xproc_cas_IrMain eMEVT_CAS_CMD! IREVT_CLASS_FTA_BLOCK_STATUS_CHANGED !\n");
			//BUS_PostMessage(OxMGR_LIVE_GetLiveViewProc(SVC_PIPE_GetActionId(hAction)), eSEVT_CAS_IR_FTA_BLOCK_CHECK, (Handle_t) NULL, 0, 0, 0);
			BUS_PostMessage(NULL, eSEVT_CAS_IR_FTA_BLOCK_CHECK, SVC_PIPE_GetActionId(hAction), 0, 0, 0);
			return MESSAGE_BREAK;
#endif

#if defined(CONFIG_MW_CAS_IRDETO_HOMINGCHANNEL)
		case IREVT_CLASS_HOMING_CH_UPDATE:
			HxLOG_Print("xproc_cas_IrMain eMEVT_CAS_CMD! IREVT_CLASS_HOMING_CH_UPDATE !\n");
			{
				HERROR				herr;
				DbSvc_TuningInfo_t	stTuneInfo;

				herr = MGR_TPLIST_SatGetHomeTuneInfo(eSatType_ALL, &stTuneInfo, TRUE);

				if(herr != ERR_OK)
				{
					break;
				}

				MGR_SEARCHUTIL_SetUserSatTpInfo(&stTuneInfo);

				//1 To Do : 아래 기능 완성을 위해 Action을 추가해야 한다. EVT_CAS_DECODE_RESET_CH_ORDER을 사용하면 되나?
				//BUS_PostMessage( NULL, MSG_APP_NOTIFY_IR_HOMING_CH_UPDATE, (Handle_t) NULL, 0, 0, 0);
				}
			return MESSAGE_BREAK;
#endif
#if defined(CONFIG_MW_CAS_IRDETO_MATURITY_RATING)
		case IREVT_CLASS_MR_PIN_STAUS_UPDATE:
			HxLOG_Print("\t=>IREVT_CLASS_MR_PIN_STAUS_UPDATE\n");
			{
				HINT32		nError = 0;

				if(pEvtData->ExtData.ulErrIndex == IR_MR_PIN_NOTIFY_E138)
				{
					nError = AXI_STATUSCODE_IDX_E138_4;
				}
				else if(pEvtData->ExtData.ulErrIndex == IR_MR_PIN_NOTIFY_E137)
				{
					nError = AXI_STATUSCODE_IDX_E137_4;
				}
				else if(pEvtData->ExtData.ulErrIndex == IR_MR_PIN_NOTIFY_E136)
				{
					nError = AXI_STATUSCODE_IDX_E136_4;
				}

				XMGR_CAS_IR_MR_Notify(0, nError);
			}
			return MESSAGE_BREAK;
#endif
#if defined(CONFIG_MW_CAS_IRDETO_PREVIEW)
		case IREVT_CLASS_ECM_MONITOR_PREVIEW_TIME:
			HxLOG_Print("IREVT_CLASS_ECM_MONITOR_PREVIEW_TIME, Create System message Proc \n");
			xmgr_cas_IrCreateUI("System Message", APP_CAS_PRIORITY, eCasIr_UI_ErrorMessage, (HINT32)hAction, pEvtData->eIrEvtClass, pEvtData->ExtData.ulErrIndex, eDxCAS_SESSIONTYPE_BANNER);
			BUS_SendMessage(xmgr_cas_IrGetUiProc(eCasIr_UI_ErrorMessage), eMEVT_BUS_IRDETO_UPDATE_UI, hAction, pEvtData->eIrEvtClass, pEvtData->ExtData.ulErrIndex, eDxCAS_SESSIONTYPE_BANNER);
			break;
#endif

#if defined(CONFIG_MW_CAS_IRDETO_UCAS)
		case IREVT_CLASS_FLEXIFLASH_STATUS_UPDATE:
		/* TODO : set timer */
			xmgr_cas_IrCreateUI("IRUC FlexFlash ", APP_CAS_PRIORITY, eCasIr_FlexFlash, (HINT32)hAction, pEvtData->eIrEvtClass, pEvtData->ExtData.usFlexiflash, eDxCAS_SESSIONTYPE_MENU);
			BUS_SendMessage(xmgr_cas_IrGetUiProc(eCasIr_FlexFlash), eMEVT_BUS_IRDETO_UPDATE_UI, hAction, pEvtData->eIrEvtClass, pEvtData->ExtData.usFlexiflash, eDxCAS_SESSIONTYPE_MENU);
			break;
#endif

		default:
			break;

	}


	/* Menu item Update - Mail menu Item Update */
	switch (pEvtData->eIrEvtClass)
	{
		case	IREVT_CLASS_DECODER_MSG_FORCED_DISPLAY:
		case	IREVT_CLASS_DECODER_MSG_NORMAL_MAIL:
		case	IREVT_CLASS_DECODER_MSG_ENHANCED_TEXT:

			pfnUiProc = xmgr_cas_IrGetUiProc(eCasIr_UI_Menu);
			if (pfnUiProc == NULL)
			{
				HxLOG_Print("pfnUiProc == NULL , break! \n");
				break;
			}

			if(BUS_MGR_Get(pfnUiProc) != NULL)
			{
				BUS_SendMessage(xmgr_cas_IrGetUiProc(eCasIr_UI_Menu), eMEVT_BUS_IRDETO_UPDATE_UI, (Handle_t) NULL, 0, 0, 0);
			}

			break;
		default:
			break;
	}

	return nErrorReturn;
}

STATIC BUS_Result_t		xmgr_cas_ir_CheckMSGProcess(IR_CAS_UI_Context *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR				nErrorReturn = ERR_OK;
	// 아래 메시지들 모두 CMD로 변경 됨

	// TODO: 이 함수는 삭제되도 됨...

	if (!xmgr_cas_ir_CheckCasId(p1))
	{
		HxLOG_Print("xmgr_cas_ir_CheckCasId error[%x]\n", p1);
		return ERR_FAIL;
	}


	HxLOG_Print("xmgr_cas_ir_CheckMSGProcess --- p1[0x%x], p2[0x%x], p3[0x%x] \n", p1, p2, p3);

	return nErrorReturn;
}

//	15초로 변경함. 시간이늘어 남...
#define	IGNORE_I07_MESSAGE_ON_BOOTING			15000//10000
STATIC BUS_Result_t xmgr_cas_ir_AlarmMSGProcess(IR_CAS_UI_Context *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR			nErrorReturn = ERR_OK;
	HUINT8			alarm_type;
	HUINT32				ulActionId = SVC_PIPE_GetActionId(hAction);
	MgrAction_Type_e		eActionType = eActionType_NONE;

	HxLOG_Info("p1: 0x%x, p2: %d, p3: %d \n", p1, p2, p3);

	eActionType = MGR_ACTION_GetType(ulActionId);
	if(eActionType == eRecType_REC || eActionType == eRecType_EXT_REC || eActionType == eRecType_TSR_REC)
	{
		HxLOG_Print("ignore message because eActionType == %d (recording action..)\n", eActionType);
		return ERR_FAIL;
	}

	if (!xmgr_cas_ir_CheckCasId(p1))
	{
		HxLOG_Print("xmgr_cas_ir_CheckCasId error[%x]\n", p1);
		return ERR_FAIL;
	}

	nErrorReturn = xmgr_cas_ir_AlarmDataProcessing((void*)p2, &alarm_type);
	if (nErrorReturn != ERR_OK)
	{
		HxLOG_Print("xmgr_cas_ir_AlarmDataProcessing error[%x] \n", nErrorReturn);
		return ERR_BUS_MESSAGE_BREAK;
	}

#if defined(CONFIG_MW_CAS_IRDETO_MATURITY_RATING)
	if(p2 == AXI_STATUSCODE_IDX_E137_4 || p2 == AXI_STATUSCODE_IDX_E136_4 || p2 == AXI_STATUSCODE_IDX_E138_4)
	{
		HxLOG_Print("MR error[%x] \n", p2);
		XMGR_CAS_IR_MR_Notify(0, p2);
		return ERR_BUS_MESSAGE_BREAK;
	}
#endif

	if (p2 == IRUC_STATUS_MSG_NONE)
	{
		//BUS_MGR_Destroy(NULL);
		return ERR_BUS_MESSAGE_BREAK;
	}

	if (p2 == IRUC_STATUS_MSG_I007_4) //AXI_STATUSCODE_IDX_I07_4
	{
		if (VK_TIMER_GetSystemTick() < IGNORE_I07_MESSAGE_ON_BOOTING)
		{
			HxLOG_Print("Ignore Checking S/C Message\n");
			//BUS_MGR_Destroy(NULL);
			return ERR_BUS_MESSAGE_BREAK;
		}
	}

	switch (alarm_type)
	{
		case eCAS_IR_ALARM_BANNER :
		case eCAS_IR_ALARM_PAIRING :
			xmgr_cas_IrCreateUI("System Message", APP_CAS_PRIORITY, eCasIr_UI_ErrorMessage, (HINT32)hAction, p1, p2, eDxCAS_SESSIONTYPE_BANNER);
			nErrorReturn = BUS_SendMessage(xmgr_cas_IrGetUiProc(eCasIr_UI_ErrorMessage), eMEVT_BUS_IRDETO_UPDATE_UI, hAction, p1, p2, eDxCAS_SESSIONTYPE_BANNER);
			break;

		default :
			break;
	}

	return nErrorReturn;
}

STATIC BUS_Result_t xmgr_cas_ir_HideErrorMessage(IR_CAS_UI_Context *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Callback_t		pfnUiProc;

	NOT_USED_PARAM(message);
	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	pfnUiProc = xmgr_cas_IrGetUiProc(eCasIr_UI_ErrorMessage);
	if (pfnUiProc == NULL)
	{
		HxLOG_Print("pfnUiProc == NULL , break! \n");
		return ERR_BUS_NO_OBJECT;
	}

	if(BUS_MGR_Get(pfnUiProc) != NULL)
	{
		BUS_SendMessage(xmgr_cas_IrGetUiProc(eCasIr_UI_ErrorMessage), eMEVT_BUS_IRDETO_HIDE_UI, hAction, 0, 0, 0);
	}

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t	xmgr_cas_ir_MsgLiveViewState(IR_CAS_UI_Context *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					ulActionId = SVC_PIPE_GetActionId(hAction);
	HERROR					hErr;
	MgrLiveView_UiState_e		 eViewState;

//	if ((ulActionId == pstContext->ulActionId) && (TRUE == pstContext->bActionTaken))
 	{

		hErr  = MGR_LIVE_GetState (ulActionId, eLiveProc_View, (HUINT32 *)&eViewState);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("MGR_LIVE_GetState Error\n");
			return MESSAGE_PASS;
		}

		HxLOG_Info("eViewState:%d \n", eViewState);

		if(eViewState != eLiveViewUi_CAS)
		{
			xmgr_cas_ir_HideErrorMessage(pstContext, message, hAction, p1, p2, p3);
		}

		if(eViewState == eLiveViewUi_EMPTY_SVC)
		{
			xmgr_cas_ir_AlarmMSGProcess(pstContext, message, hAction, eDxCAS_GROUPID_IR, IRUC_STATUS_MSG_E037_32, 0);
		}
		else if(eViewState == eLiveViewUi_NO_SIGNAL)
		{
			xmgr_cas_ir_AlarmMSGProcess(pstContext, message, hAction, eDxCAS_GROUPID_IR, IRUC_STATUS_MSG_E048_32, 0);
		}
		else if(eViewState == eLiveViewUi_AV_UNDER_RUN)
		{
			xmgr_cas_ir_AlarmMSGProcess(pstContext, message, hAction, eDxCAS_GROUPID_IR, IRUC_STATUS_MSG_E037_32, 0);
		}
		else if(eViewState == eLiveViewUi_NOT_RUNNING)
		{
			xmgr_cas_ir_AlarmMSGProcess(pstContext, message, hAction, eDxCAS_GROUPID_IR, IRUC_STATUS_MSG_E038_32, 0);
		}
		else if(eViewState == eLiveViewUi_NO_SIGNAL_TIMEOUT)
		{
			xmgr_cas_ir_AlarmMSGProcess(pstContext, message, hAction, eDxCAS_GROUPID_IR, IRUC_STATUS_MSG_E052_32, 0);
		}

 	}

	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t	xmgr_cas_ir_MsgPbStateChanged(IR_CAS_UI_Context *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
//	HUINT32					ulActionId	=	SVC_PIPE_GetActionId(hAction);
	BUS_Result_t			eResult		=	ERR_BUS_NO_ERROR;
	MgrPb_State_e		ePbState	=	(MgrPb_State_e)p1;

	/*
	 *	author : hsseo
	 *	note : handover Event of PlayBack Underrun
	 */
 	{
		HxLOG_Info("Playback State Changed [%d]\n", ePbState);

		if (eMgrPb_AV_UNDER_RUN== ePbState)
		{
			eResult = xmgr_cas_ir_AlarmMSGProcess(pstContext, message, hAction, eDxCAS_GROUPID_IR, IRUC_STATUS_MSG_E037_32, 0);
		}

 	}

	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

#if !defined(CONFIG_OTA_IRDETO_LOADER_V5)
STATIC BUS_Result_t	xmgr_cas_ir_GetStbDefaultData(IR_CAS_UI_Context *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR								hErr = ERR_FAIL;
	HUINT32								ulSwupFlag = 0;
	SvcSi_SwupFlag_t 					stSwupCheckParam;
	SvcSi_SwupCheckSpec_t				stSwupCheckSpec;
	XsvcSi_SwupCheckParam_Irdeto_t		stIrdetoParam;

	SvcCas_CtrlParam_t					stIrCtrlParam;
	IR_LOADER_GetManuCodeOutParam_t 	stManCodeParam;
	IR_LOADER_GetHwCodeOutParam_t		stHwcodeParm;
	IR_LOADER_GetLoadSqeNumOutParam_t	stLoadSqeNum;
	IR_LOADER_GetStatusOutParam_t		stStatus;

	stIrCtrlParam.pvOut 	= &stManCodeParam;
	stIrCtrlParam.ulControlType = eCACTRL_IR_LOADER_GetManuCode;
	SVC_CAS_CtrlGet(eDxCAS_GROUPID_IR, eCACTRL_IR_LOADER_GetManuCode, &stIrCtrlParam);

	stIrCtrlParam.pvOut 	= &stHwcodeParm;
	stIrCtrlParam.ulControlType = eCACTRL_IR_LOADER_GetHWCode;
	SVC_CAS_CtrlGet(eDxCAS_GROUPID_IR, eCACTRL_IR_LOADER_GetHWCode, &stIrCtrlParam);

	stIrCtrlParam.pvOut 	= &stLoadSqeNum;
	stIrCtrlParam.ulControlType = eCACTRL_IR_LOADER_GetLoadSeqNum;
	SVC_CAS_CtrlGet(eDxCAS_GROUPID_IR, eCACTRL_IR_LOADER_GetLoadSeqNum, &stIrCtrlParam);

	stIrCtrlParam.pvOut 	= &stStatus;
	stIrCtrlParam.ulControlType = eCACTRL_IR_LOADER_GetStatusInfo;
	SVC_CAS_CtrlGet(eDxCAS_GROUPID_IR, eCACTRL_IR_LOADER_GetStatusInfo, &stIrCtrlParam);

	HxSTD_MemSet(&stIrdetoParam, 0, sizeof(XsvcSi_SwupCheckParam_Irdeto_t));
	stIrdetoParam.ucHardwareCode = (HUINT8)stHwcodeParm.usHwCode;
	stIrdetoParam.ucLoadSequenceNumber = (HUINT8)stLoadSqeNum.usLoadSeqNum;
	stIrdetoParam.ucManufacturerCode = (HUINT8)stManCodeParam.usManCode;
	stIrdetoParam.ucVariantHi = (HUINT8)((stStatus.stIRStatus.u16Variant >> 8) & 0x00FF);
	stIrdetoParam.ucVariantLo = (HUINT8)(stStatus.stIRStatus.u16Variant & 0x00FF);

	stSwupCheckParam.eSiTableType = eSIxTABLETYPE_ALL;
	stSwupCheckParam.ulSize = sizeof(XsvcSi_SwupCheckParam_Irdeto_t);
	stSwupCheckParam.pvData = &stIrdetoParam;

	stSwupCheckSpec.eActionKind = eSVCSI_ACT_KIND_SwupCheck;
	stSwupCheckSpec.ulActionSpec = eSVCSI_ACT_SPEC_Irdeto;
	hErr = SVC_SI_SetSwupCheckParam(&stSwupCheckSpec, &stSwupCheckParam);
	if(hErr == ERR_OK)
	{
		hErr = SVC_SI_GetSwupFlag (hAction, &ulSwupFlag);
		if (hErr != ERR_OK)	HxLOG_Error("error in SVC_SI_GetSwupFlag");
		HxLOG_Debug("ulSwupFlag: %d \n", ulSwupFlag);
		XMGR_SWUP_SetControlByte_Irdeto((HUINT8)ulSwupFlag);
		BUS_SendMessage(NULL, eMEVT_CAS_NOTIFICATION, hAction, eDxCAS_GROUPID_IR, 0, 0);
	}
	else
	{
		HxLOG_Error("SVC_SI_SetSwupCheckParam() error! \n");
	}

	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_PASS;
}
#else
STATIC BUS_Result_t	xmgr_cas_ir_GetStbDefaultData(IR_CAS_UI_Context *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR								hErr = ERR_FAIL;
	HUINT32								ulSwupFlag = 0;
	SvcSi_SwupFlag_t 					stSwupCheckParam;
	SvcSi_SwupCheckSpec_t				stSwupCheckSpec;
	XsvcSi_SwupCheckParam_Irdeto_t		stIrdetoParam;

	SvcCas_CtrlParam_t					stIrCtrlParam;
	IR_LOADER_GetManuCodeOutParam_t 	stManCodeParam;
	IR_LOADER_GetHwCodeOutParam_t		stHwcodeParm;
	IR_LOADER_GetLoadSqeNumOutParam_t	stDownloadSqeNum;
	IR_LOADER_GetStatusOutParam_t		stStatus;

	stIrCtrlParam.pvOut 	= &stManCodeParam;
	stIrCtrlParam.ulControlType = eCACTRL_IR_LOADER_GetManuCode;
	SVC_CAS_CtrlGet(eDxCAS_GROUPID_IR, eCACTRL_IR_LOADER_GetManuCode, &stIrCtrlParam);

	stIrCtrlParam.pvOut 	= &stHwcodeParm;
	stIrCtrlParam.ulControlType = eCACTRL_IR_LOADER_GetHWCode;
	SVC_CAS_CtrlGet(eDxCAS_GROUPID_IR, eCACTRL_IR_LOADER_GetHWCode, &stIrCtrlParam);

	stIrCtrlParam.pvOut 	= &stDownloadSqeNum;
	stIrCtrlParam.ulControlType = eCACTRL_IR_LOADER_GetLoadSeqNum;
	SVC_CAS_CtrlGet(eDxCAS_GROUPID_IR, eCACTRL_IR_LOADER_GetLoadSeqNum, &stIrCtrlParam);

	stIrCtrlParam.pvOut 	= &stStatus;
	stIrCtrlParam.ulControlType = eCACTRL_IR_LOADER_GetStatusInfo;
	SVC_CAS_CtrlGet(eDxCAS_GROUPID_IR, eCACTRL_IR_LOADER_GetStatusInfo, &stIrCtrlParam);

	HxSTD_MemSet(&stIrdetoParam, 0, sizeof(XsvcSi_SwupCheckParam_Irdeto_t));
	stIrdetoParam.usManufacturerId = stManCodeParam.usManCode;
	stIrdetoParam.usHardwareVersion = stHwcodeParm.usHwCode;
	stIrdetoParam.usDownloadSequenceNumber = stDownloadSqeNum.usLoadSeqNum;
	stIrdetoParam.usVariant = stStatus.stIRStatus.u16Variant;
	stIrdetoParam.usSubVariant= stStatus.stIRStatus.u16SubVariant;

	stSwupCheckParam.eSiTableType = eSIxTABLETYPE_ALL;
	stSwupCheckParam.ulSize = sizeof(XsvcSi_SwupCheckParam_Irdeto_t);
	stSwupCheckParam.pvData = &stIrdetoParam;

	stSwupCheckSpec.eActionKind = eSVCSI_ACT_KIND_SwupCheck;
	stSwupCheckSpec.ulActionSpec = eSVCSI_ACT_SPEC_Irdeto;
	hErr = SVC_SI_SetSwupCheckParam(&stSwupCheckSpec, &stSwupCheckParam);
	if(hErr == ERR_OK)
	{
		hErr = SVC_SI_GetSwupFlag (hAction, &ulSwupFlag);
		if (hErr != ERR_OK)	HxLOG_Error("error in SVC_SI_GetSwupFlag");
		HxLOG_Debug("ulSwupFlag: %d \n", ulSwupFlag);
		XMGR_SWUP_SetControlByte_Irdeto((HUINT8)ulSwupFlag);
		BUS_SendMessage(NULL, eMEVT_CAS_NOTIFICATION, hAction, eDxCAS_GROUPID_IR, 0, 0);
	}
	else
	{
		HxLOG_Error("SVC_SI_SetSwupCheckParam() error! \n");
	}

	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_PASS;
}
#endif

STATIC BUS_Result_t	xmgr_cas_ir_MsgLiveLockState(IR_CAS_UI_Context *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					ulActionId = SVC_PIPE_GetActionId(hAction);
	HERROR					hErr;
	MgrLiveLock_UiState_e	eLockState;

//	if ((ulActionId == pstContext->ulActionId) && (TRUE == pstContext->bActionTaken))
 	{
		hErr = MGR_LIVE_GetState (ulActionId, eLiveProc_Lock, (HUINT32 *)&eLockState);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("MGR_LIVE_GetState Error\n");
			return MESSAGE_PASS;
		}

		HxLOG_Info("eLockState:%d \n", eLockState);

		// TODO: shko E42  PC 관련 처리
 	}

	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC HERROR xmgr_cas_ir_ScRemovedMSGProcess(IR_CAS_UI_Context *pContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxLOG_Print("eMEVT_CAS_SC_REMOVED! \n");
	HxLOG_Print("xmgr_cas_ir_ScRemovedMSGProcess\n");

	if (!xmgr_cas_ir_CheckCasId(p1))
	{
		HxLOG_Print("xmgr_cas_ir_CheckCasId error[%x]\n", p1);
		return ERR_FAIL;
	}

	xmgr_cas_IrSetNationalityAvailable(FALSE);

	NOT_USED_PARAM(hAction);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return	ERR_OK;
}

STATIC HBOOL	xmgr_cas_ir_ProcessSmartCardReply(IR_CAS_UI_Context *pContext, HUINT32 msg, HUINT8 *pData)
{
/* Comment : hjlee3 TODO 어케 처리 해야 하나 확인 해봐야 한다.  */
#if (0)
	switch(msg)
	{
		case	MSG_SMARTCARD_USER_DATA_REPLY:
			xmgr_cas_IrScReadUserDataReply(pData);
			break;

		case	MSG_PIN_CODE_REPLY:
			xmgr_cas_IrScPinCodeReply(pData);
			break;

#if defined(CONFIG_OP_DIGITURK)
#if defined(CONFIG_MW_CAS_IRDETO_IPPV_CB)
		case MSG_REMOTE_MESSAGE_REPLY:
			// not yet implemented...
			break;
#endif

		case MSG_SMARTCARD_TMS_DATA_REPLY:
			// not yet implemented...
			break;


		case	MSG_PRODUCT_CHECK_REPLY:
			// not yet implemented...
			break;
#endif
	}
#endif
	if(pData)
		OxCAS_Free(pData);

	return TRUE;
}
extern HERROR	OM_CAS_MMI_RegisterCasUiProc(DxCAS_GroupId_e eCasGrpId, BUS_Callback_t fnCasUiProc);

STATIC HERROR xmgr_cas_ir_MsgGwmCreate(IR_CAS_UI_Context *pContext)
{
	pContext->pfnUiCallback[eIrCasUi_FnErrorMessage]	= xmgr_cas_IrGetUiProc(eCasIr_UI_ErrorMessage);
	pContext->pfnUiCallback[eIrCasUi_FnMenu] = xmgr_cas_IrGetUiProc(eCasIr_UI_Menu);
	pContext->pfnUiCallback[eIrCasUi_FnMailIcon]= xmgr_cas_IrGetUiProc(eCasIr_UI_MailIcon);
	pContext->pfnUiCallback[eIrCasUi_FnMailMessage] = xmgr_cas_IrGetUiProc(eCasIr_UI_MailMessage);

	pContext->pstCasUiCustomSession = &s_stIrCasUiExtendedSession[0];

	if(OM_CAS_MMI_RegisterCasUiProc(eDxCAS_GROUPID_IR, xproc_cas_IrMain) != ERR_OK)
	{
		HxLOG_Critical("\n\n");
	}

	return ERR_OK;
}

#if defined(CONFIG_DEBUG)
STATIC HUINT8 *xmgr_cas_ir_GetMessageName(HINT32 message)
{
	switch(message)
	{
		ENUM_TO_STR(eMEVT_CAS_OK);
		ENUM_TO_STR(eMEVT_CAS_FAIL);
		ENUM_TO_STR(eMEVT_CAS_CMD);
		ENUM_TO_STR(eMEVT_CAS_CHECK);

		ENUM_TO_STR(eMEVT_CAS_PPV_REQUEST);
		ENUM_TO_STR(eMEVT_CAS_PPV_REPLY);
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
		ENUM_TO_STR(eMEVT_CAS_PINCODE_REPLY);
		ENUM_TO_STR(eMEVT_CAS_UDATAREAD_REPLY);
		ENUM_TO_STR(eSEVT_CH_NO_SIGNAL);
		ENUM_TO_STR(eSEVT_CH_MOVING_MOTOR);
		ENUM_TO_STR(eMEVT_LIVE_NOTIFY_SVC_CHANGED);
		ENUM_TO_STR(eMEVT_CAS_CLOSE);

		default:
			return NULL;
	}
}
#endif

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
#define _____XMGR_CAS_IR_PUBLIC_FUNCTION______________________________________________________

BUS_Result_t 	xproc_cas_IrMain(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t		eBusResult = ERR_BUS_NO_ERROR;
	HUINT32				ulActionId = SVC_PIPE_GetActionId(hAction);
	IR_CAS_UI_Context 	*pContext = xmgr_cas_ir_GetContextUI();
	IREVT_Data			*pEvtData;

#ifdef CONFIG_DEBUG
	{
		HUINT8 *pMsg = xmgr_cas_ir_GetMessageName(message);

		if(pMsg != NULL)
		{
			HxLOG_Info("[%s], ActId(%x), p1(%x), p2(%x), p3(%x)\n", pMsg, ulActionId, p1, p2, p3);
		}
	}
#endif
	switch(message)
	{
		case eMEVT_BUS_CREATE:
			HxLOG_Print("xproc_cas_IrMain - Create \n");
			BUS_MGR_SetMgrName(BUS_MGR_GetThis(), "xproc_cas_IrMain");
			xmgr_cas_ir_MsgGwmCreate(pContext);
			break;

		case eMEVT_BUS_READY_SHUTDOWN:
			HxLOG_Info("receive eMEVT_BUS_READY_SHUTDOWN \n");
			return MESSAGE_PASS;

		case eMEVT_CAS_SC_REMOVED:
			HxLOG_Info("receive eMEVT_CAS_SC_REMOVED \n");
			xmgr_cas_ir_ScRemovedMSGProcess(pContext, hAction, p1, p2, p3);
	#if defined(CONFIG_MW_CAS_IRDETO_PPV)
			XMGR_CAS_IR_PPV_SC_Removed((HUINT8) p1);
	#endif
			break;

		// ewlee 20100914
#if defined(CONFIG_MW_CAS_IRDETO_PPV)
		case eMEVT_CAS_SC_INSERTED:
			HxLOG_Info("receive eMEVT_CAS_SC_INSERTED \n");
			XMGR_CAS_IR_PPV_SC_Inserted((HUINT8) p1);
			break;
#endif

		case eMEVT_CAS_FAIL :
			HxLOG_Info("receive eMEVT_CAS_FAIL \n");
			pEvtData = (IREVT_Data*)p2;
			eBusResult = xmgr_cas_ir_AlarmMSGProcess(pContext, message, hAction, p1, pEvtData->ulAxiCodeIdx, p3);
			break;

		case eMEVT_CAS_CMD :
			HxLOG_Info("receive eMEVT_CAS_CMD \n");
			eBusResult = xmgr_cas_ir_CommandMSGProcess(pContext, message, hAction, p1, p2, 0);
			break;

		case eMEVT_CAS_CHECK :
			HxLOG_Info("receive eMEVT_CAS_CHECK \n");
			pEvtData = (IREVT_Data*)p2;
			eBusResult = xmgr_cas_ir_CheckMSGProcess(pContext, message, hAction, pEvtData->eIrEvtClass, p2, 0);
			break;

		// Receving eMEVT_CAS_OK message, all irdeto cas windows have to close(except bdc window)
		case eMEVT_CAS_OK :
			HxLOG_Info("receive eMEVT_CAS_OK \n");
			if(pContext != NULL)
			{
				if (!xmgr_cas_ir_CheckCasId(p1))
					break;

				if (ulActionId != MGR_ACTION_GetMainActionId())
				{
					break;
				}

				xmgr_cas_ir_HideErrorMessage(pContext, message, hAction, p1, p2, 0);
			}
			break;

#if defined (CONFIG_MW_CAS_IRDETO_PPV)
		// Receives PPV (i or L) request from other AP Proc
		case eMEVT_CAS_PPV_REQUEST:
			HxLOG_Info("receive eMEVT_CAS_PPV_REQUEST \n");
			if(p1 == eDxCAS_GROUPID_IR)
			{
				SvcCas_SmartcardReplyData_t *pReply = (SvcCas_SmartcardReplyData_t*)p2;
				xmgr_cas_IrProcessPPVQuery(pReply->ulMsg, pReply->pvData);
			}
			return MESSAGE_BREAK;

		// Receives PPV (i or L) replies from IR MW. ie Softcell
		case eMEVT_CAS_PPV_REPLY:
			HxLOG_Info("receive eMEVT_CAS_PPV_REPLY \n");
			if(p1 == eDxCAS_GROUPID_IR)
			{
				SvcCas_SmartcardReplyData_t *pReply = (SvcCas_SmartcardReplyData_t*)p2;
				xmgr_cas_IrProcessPPVReply(pReply->ulMsg, pReply->pvData);
				if(pReply)
					OxCAS_Free(pReply);
			}
			return MESSAGE_BREAK;
#endif

		case eMEVT_CAS_PINCODE_REPLY:
		case eMEVT_CAS_UDATAREAD_REPLY:
			HxLOG_Info("receive eMEVT_CAS_PINCODE_REPLY or  eMEVT_CAS_UDATAREAD_REPLY\n");
			if(p1 == eDxCAS_GROUPID_IR)
			{
				SvcCas_SmartcardReplyData_t *pReply = (SvcCas_SmartcardReplyData_t*)p2;
				if(pReply == NULL)
					return MESSAGE_BREAK;
				xmgr_cas_ir_ProcessSmartCardReply(pContext, pReply->ulMsg, pReply->pvData);
				if(pReply)
				{
					OxCAS_Free(pReply);
				}
			}
			return MESSAGE_BREAK;

		// Something wrong the signal, all irdeto cas windows have to close!
		case eSEVT_CH_MOVING_MOTOR :
			HxLOG_Info("receive eSEVT_CH_MOVING_MOTOR\n");
			xmgr_cas_ir_HideErrorMessage(pContext, message, hAction, p1, p2, p3);
			break;

		case eMEVT_LIVE_NOTIFY_SVC_CHANGED :
			HxLOG_Info("receive eMEVT_LIVE_NOTIFY_SVC_CHANGED\n");
			xmgr_cas_ir_HideErrorMessage(pContext, message, hAction, p1, p2, p3);
			break;

		case eMEVT_CAS_CLOSE :
			HxLOG_Info("receive eMEVT_CAS_CLOSE do nothing!!!\n");
#if 0
			if (ulActionId != MGR_ACTION_GetMainActionId())
			{
				return MESSAGE_PASS;
			}
			xmgr_cas_ir_HideErrorMessage(pContext, message, hAction, p1, p2, p3);
#endif
			break;

		case eMEVT_BUS_DESTROY :
			HxLOG_Info("receive eMEVT_BUS_DESTROY\n");
			break;

		case eOEVT_CAS_CASUI_APP_OPEN_SESSION:
			HxLOG_Info("receive eOEVT_CAS_CASUI_APP_OPEN_SESSION \n");

			if(pContext != NULL)
			{
				eBusResult = xmgr_cas_ir_MsgCasUiSessionStart(pContext, message, hAction, p1, p2, p3);
			}
			break;

		case eOEVT_CAS_CASUI_APP_CLOSE_SESSION:
			HxLOG_Info("receive eOEVT_CAS_CASUI_APP_CLOSE_SESSION \n");
			if(pContext != NULL)
			{
				eBusResult = xmgr_cas_ir_MsgCasUiSessionStop(pContext, message, hAction, p1, p2, p3);
			}
			break;
		case eOEVT_CAS_CASUI_APP_USERINPUTDATA:
			HxLOG_Info("receive eOEVT_CAS_CASUI_APP_USERINPUTDATA \n");
			if(pContext != NULL)
			{
				eBusResult = xmgr_cas_ir_MsgCasUiUserInputData(pContext, message, hAction, p1, p2, (const void *)p3);
			}
			else
			{
				if(NULL != (void *)p3)
				{
					//OTL_BUSBUFFER_Release((void *)p3);
				}
			}
			break;

		case eMEVT_LIVE_NOTIFY_LOCK_STATE_CHANGED:
			HxLOG_Info("receive eMEVT_LIVE_NOTIFY_LOCK_STATE_CHANGED \n");
			if(pContext != NULL)
			{
				eBusResult = xmgr_cas_ir_MsgLiveLockState(pContext, message, hAction, p1, p2, p3);
			}
			break;

		case eMEVT_LIVE_NOTIFY_VIEW_STATE_CHANGED:
			HxLOG_Info("receive eMEVT_LIVE_NOTIFY_VIEW_STATE_CHANGED \n");
			if(pContext != NULL)
			{
				if (ulActionId != MGR_ACTION_GetMainActionId())
				{
					return MESSAGE_PASS;
				}

				eBusResult = xmgr_cas_ir_MsgLiveViewState(pContext, message, hAction, p1, p2, p3);
			}
			break;

		case eMEVT_PVR_PLAYBACK_STATE_CHANGED:
			HxLOG_Info("receive eMEVT_PVR_PLAYBACK_STATE_CHANGED \n");
			if(pContext != NULL)
			{
				if (ulActionId != MGR_ACTION_GetMainActionId())
				{
					return MESSAGE_PASS;
				}

				eBusResult = xmgr_cas_ir_MsgPbStateChanged(pContext, message, hAction, p1, p2, p3);
			}
			break;

#if defined(CONFIG_OTA_IRDETO)

#if !defined(CONFIG_OTA_IRDETO_LOADER_V5)
		case eSEVT_SI_BAT :
			HxLOG_Info("receive eSEVT_SI_BAT \n");
			eBusResult = xmgr_cas_ir_GetStbDefaultData(pContext, message, hAction, p1, p2, p3);
			break;

		case eSEVT_SI_BAT_TIMEOUT :
			HxLOG_Info("receive eSEVT_SI_BAT_TIMEOUT \n");
			XMGR_SWUP_SetControlByte_Irdeto(0);
			BUS_SendMessage(NULL, eMEVT_CAS_NOTIFICATION, hAction, eDxCAS_GROUPID_IR, 0, 0);
			break;
#else
		case eSEVT_SI_BAT :
		case eSEVT_SI_NIT :
			HxLOG_Info("receive eSEVT_SI_BAT or eSEVT_SI_NIT \n");
			eBusResult = xmgr_cas_ir_GetStbDefaultData(pContext, message, hAction, p1, p2, p3);
			break;

		case eSEVT_SI_BAT_TIMEOUT :
		case eSEVT_SI_NIT_TIMEOUT :
			HxLOG_Info("receive eSEVT_SI_BAT_TIMEOUT or eSEVT_SI_NIT_TIMEOUT \n");
			//XMGR_SWUP_SetControlByte_Irdeto(0);
			//BUS_SendMessage(NULL, eMEVT_CAS_NOTIFICATION, hAction, eDxCAS_GROUPID_IR, 0, 0);
			break;
#endif

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



