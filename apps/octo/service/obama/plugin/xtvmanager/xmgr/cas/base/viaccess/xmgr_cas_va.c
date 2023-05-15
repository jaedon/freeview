/*
 * (c) 2011-2012 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/
#include <octo_common.h>
#include <mgr_live.h>

#include <svc_pipe.h>
#include <svc_si.h>
#include <svc_cas.h>

#include <mgr_pg.h>
#include <mgr_cas.h>

#include <va_ui.h>
#include <va_main.h>

#include "vamgr_main.h"
#include "va_util.h"


#include "./local_include/_xmgr_cas_va_adapt.h"
#include "./local_include/_xmgr_cas_va_util.h"
#include "./local_include/_xmgr_cas_va.h"
#include "./local_include/_xmgr_cas_va_ui.h"
#include "./local_include/_xmgr_cas_va_sc.h"


//#include <app_manager_cas.h>// TODO: hjlee3, just for build
/*******************************************************************/
/********************      Definitions    **************************/
/*******************************************************************/





/*******************************************************************/
/********************      Typedef      ****************************/
/*******************************************************************/
typedef enum
{
	eVaCasUi_SessionSystemMessage = 0, //   TODO: hjlee3, just for build
	eVaCasUi_SessionCheckMessage,
	eVaCasUi_SessionMailIcon,
	eVaCasUi_SessionMailMessage,
	eVaCasUi_SessionFingerPrint,
	eVaCasUi_SessionPinDialog,
	eVaCasUi_SessionPpvDialog,
	eVaCasUi_SessionPurchaseList,
	eVaCasUi_SessionCamlockScreen,
	eVaCasUi_SessionMax
}VA_CAS_UI_CUSTOM_SESSION_e;

#define VA_CAS_UI_CUSTOM_SESSION_MAX	(eVaCasUi_SessionMax - eVaCasUi_SessionSystemMessage)

typedef struct
{
	VA_CAS_UI_CUSTOM_SESSION_e			eUiSession;
	VA_CAS_UI_FN_e						eFunction;
	VA_CAS_UI_e							eUiState;
} VA_CAS_UI_Custom_Session_t;

typedef struct
{
	CAS_VA_SC_State_t	stScState;
	VA_CAS_UI_e			eUiState;
	VA_CAS_UI_Custom_Session_t	*pstCasUiCustomSession;
	BUS_Callback_t		pfnUiCallback[eVaCasUi_FnMax];
} VA_CAS_UI_Context;

#if 0
typedef struct
{
	BUS_Callback_t		pfnNotificationProc;
	BUS_Callback_t		pfnCheckMessageProc;
	BUS_Callback_t		pfnCamlockScreenProc;
	BUS_Callback_t		pfnOpMessageProc;
	CAS_VA_SC_State_t	stScState;
	eCAS_VA_UI_STATE	eUiState;
} VA_CAS_UI_Context;
#endif

/* local macros using in proc msg  */
#define VA_CAS_PARAM_WARNING_REMOVE(arg1, arg2, arg3, arg4, arg5, arg6)	\
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

STATIC VA_CAS_UI_Context		s_stVaCasUiContext;
STATIC VA_CAS_UI_Custom_Session_t		s_stVaCasUiCustomSession[VA_CAS_UI_CUSTOM_SESSION_MAX] =
{
	{eVaCasUi_SessionSystemMessage,		eVaCasUi_FnSystemMessage,		eVaCasUi_SystemMessage},
	{eVaCasUi_SessionCheckMessage,		eVaCasUi_FnCheckMessage, 		eVaCasUi_CheckMessage},
	{eVaCasUi_SessionMailIcon,			eVaCasUi_FnMailIcon,			eVaCasUi_MailIcon},
	{eVaCasUi_SessionMailMessage,		eVaCasUi_FnMailMessage, 		eVaCasUi_MailMessage},
	{eVaCasUi_SessionFingerPrint,		eVaCasUi_FnFingerPrint, 		eVaCasUi_FingerPrint},
	{eVaCasUi_SessionPinDialog,			eVaCasUi_FnPinDialog, 			eVaCasUi_FnPinDialog},
	{eVaCasUi_SessionPpvDialog,			eVaCasUi_FnPpvDialog, 			eVaCasUi_PpvDialog},
	{eVaCasUi_SessionPurchaseList,		eVaCasUi_FnPurchaseList, 		eVaCasUi_PurchaseList},
	{eVaCasUi_SessionCamlockScreen,		eVaCasUi_FnCamlockScreen, 		eVaCasUi_CamlockScreen},
};

/*******************************************************************/
/********************      Local Functions      ********************/
/*******************************************************************/

#if defined(CONFIG_DEBUG)
STATIC void xmgr_cas_VaPrintCasEventName(HINT32 message, HINT32 CasMsg);
STATIC HUINT8 *xmgr_cas_VaGetMessageName(HINT32 message);
#endif

STATIC HINT32 xmgr_cas_VaGetCamlockLevel(void);
STATIC HUINT8 xmgr_cas_VaGetCamlockState(HBOOL bCheckSecurityRestriction);

#define _____LOCAL_FUNCTION______________________________________________________

STATIC VA_CAS_UI_Context *xmgr_cas_VaGetContextUI(void)
{
	return &s_stVaCasUiContext;
}
STATIC VA_CAS_UI_Custom_Session_t	*xmgr_cas_VaGetCustomSession(void)
{
	return	&s_stVaCasUiCustomSession[0];
}
STATIC HERROR xmgr_cas_VaCheckActionID(HUINT32 ulActionID)
{
	HERROR bCheck = ERR_OK;

	if (ulActionID != MGR_ACTION_GetMainActionId())
	{
		HxLOG_Info("[received, but [%x] isn't main actiod id!\n", ulActionID);
		bCheck = ERR_FAIL;
	}

	return bCheck;
}

STATIC HUINT8 xmgr_cas_VaCheckCasId(HINT32 ulCasId, DxCAS_GroupId_e eGroupId)
{
	if (ulCasId == eGroupId)
		return TRUE;
	else
		return FALSE;
}

STATIC BUS_Result_t xmgr_cas_VaMsgGwmDeleteDialog(VA_CAS_UI_Context *pContext)
{
	if(BUS_MGR_Get(pContext->pfnUiCallback[eVaCasUi_FnSystemMessage]) != NULL)
		BUS_MGR_Destroy(pContext->pfnUiCallback[eVaCasUi_FnSystemMessage]);

	if(BUS_MGR_Get(pContext->pfnUiCallback[eVaCasUi_FnCheckMessage]) != NULL)
		BUS_MGR_Destroy(pContext->pfnUiCallback[eVaCasUi_FnCheckMessage]);

	if(BUS_MGR_Get(pContext->pfnUiCallback[eVaCasUi_FnMailMessage]) != NULL)
		BUS_MGR_Destroy(pContext->pfnUiCallback[eVaCasUi_FnMailMessage]);

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t xmgr_cas_VaMsgGwmDeleteMainProc(VA_CAS_UI_Context *pContext)
{
	if ((BUS_MGR_Get(pContext->pfnUiCallback[eVaCasUi_FnSystemMessage]) == NULL) &&
		(BUS_MGR_Get(pContext->pfnUiCallback[eVaCasUi_FnCheckMessage]) == NULL) &&
		(BUS_MGR_Get(pContext->pfnUiCallback[eVaCasUi_FnMailMessage]) == NULL))
	{
		BUS_MGR_Destroy(NULL);
	}

	return ERR_BUS_NO_ERROR;
}

STATIC HERROR xmgr_cas_VaFailMessageProcess(VA_CAS_UI_Context *pContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR	hError = ERR_OK;
	HUINT8 lockValue;
	HINT32 lockLevel;

	HxLOG_Info("xmgr_cas_VaAdaptMessageProcess\n");
	hError = xmgr_cas_VaAdaptMessageProcess((void*)p2, (void*)&pContext->stScState, (void*)&pContext->eUiState);

	if(hError == ERR_FAIL)
	{
		HxLOG_Error("AP_CAS_VA_Adapt_MessageProcess() fail or nothing to do\n");
		return hError;
	}

	if(pContext->eUiState == eCAS_VA_UI_None)
	{
		HxLOG_Info("Ui State : eCAS_VA_UI_None\n");
		return hError;
	}

	if(pContext->eUiState == eCAS_VA_UI_CamlockOk)
	{
		HxLOG_Info("Ui State : eCAS_VA_UI_CamlockOk\n");
		BUS_MGR_Destroy(pContext->pfnUiCallback[eVaCasUi_FnCamlockScreen]);
		BUS_MGR_Destroy(pContext->pfnUiCallback[eVaCasUi_FnSystemMessage]);
		VA_OutputMute(FALSE, VA_MUTE_CAMLOCK);
		return hError;
	}

#if 1 //To comile for Octo2
	AP_CAS_VA_DestroyPreview();
#endif
	BUS_MGR_Destroy(pContext->pfnUiCallback[eVaCasUi_FnSystemMessage]);

	BUS_MGR_Create("pContext->pfnUiCallback[eVaCasUi_FnSystemMessage]", APP_CAS_PRIORITY, pContext->pfnUiCallback[eVaCasUi_FnSystemMessage], hAction, 0, 0, 0);

#if 1 // 불필요. 여기 오기 전에 local_ap_cas_va_MsgGwmCreateDialog() 에서 xmgr_cas_VaGetCamlockState(TRUE) == 0 인 경우만 허용되므로 여기서는 사실상 마지막 else 루틴만 처리되게 되어 있음. #if 0 으로 막아도 될 듯...
	lockValue = xmgr_cas_VaGetCamlockState(TRUE);
	if(lockValue & AP_CAS_VA_CAMLOCK_CTRL_ON) // security restriction 상황과 관계 없이 camlock 이 걸려 있으면...
	{
		lockLevel = xmgr_cas_VaGetCamlockLevel();
		if (lockLevel == VA_CAMLOCK_CONTROL_HIGH)
		{
			// high level camlock feature 를 지원하는 경우, black screen + not compliant 배너 표시 + 채널 전환 안됨 + FTA 시청 불가 + 메뉴 진입 안됨 (VA-5SD, VHDR-3000S)
			BUS_MGR_Destroy(pContext->pfnUiCallback[eVaCasUi_FnCamlockScreen]);
			BUS_MGR_Create("pContext->pfnUiCallback[eVaCasUi_FnCamlockScreen]", APP_CAS_HIGH_PRIORITY, pContext->pfnUiCallback[eVaCasUi_FnCamlockScreen], hAction, 0, 0, 0);
			VA_OutputMute(TRUE, VA_MUTE_CAMLOCK);
		}
		else if (lockLevel == VA_CAMLOCK_CONTROL_MEDIUM)
		{
			// medium level camlock feature 를 지원하는 경우, black screen + not compliant 배너 표시 + 채널 전환 가능 (단, 스크램블 채널 상관 없이 배너는 계속 표시) + FTA 시청 불가(?) + 배너 닫고 메뉴 진입 가능 (현재로선 해당 모델 없음)
			BUS_MGR_Destroy(pContext->pfnUiCallback[eVaCasUi_FnCamlockScreen]);
			BUS_MGR_Create("pContext->pfnUiCallback[eVaCasUi_FnCamlockScreen]", APP_CAS_PRIORITY, pContext->pfnUiCallback[eVaCasUi_FnCamlockScreen], hAction, 0, 0, 0);
			VA_OutputMute(TRUE, VA_MUTE_CAMLOCK);
		}
		else
		{
			// low level camlock feature 를 지원하는 경우, black screen + not compliant 배너 표시 + 채널 전환 가능 (단, 스크램블 채널인 경우에 한해 배너는 계속 표시) + FTA 시청 가능 + 배너 닫고 메뉴 진입 가능 (BXR-HD)
			DbSvc_Info_t 		svcInfo;

			// cam lock proc에서 fta인지 check하여 fta라면 create하고 스스로 hide
			BUS_MGR_Destroy(pContext->pfnUiCallback[eVaCasUi_FnCamlockScreen]);
			BUS_MGR_Create("pContext->pfnUiCallback[eVaCasUi_FnCamlockScreen]", APP_CAS_PRIORITY, pContext->pfnUiCallback[eVaCasUi_FnCamlockScreen], hAction, 0, 0, 0);

			// scrambled ch이면 mute, fta라면 unmute
			hError = MGR_LIVE_GetSvcInfo (MGR_ACTION_GetMainActionId(), eLiveViewPlaySvc_MASTER, &svcInfo);
			if (DbSvc_GetCasType(&svcInfo) != eDxCAS_TYPE_FTA)
			{
				VA_OutputMute(TRUE, VA_MUTE_CAMLOCK);
			}
			else
			{
				VA_OutputMute(FALSE, VA_MUTE_CAMLOCK);
			}
		}
	}
	else if(lockValue == AP_CAS_VA_SECURITY_CTRL_ON) // camlock 없이 security restriction 만 걸려 있으면...
	{
		BUS_MGR_Destroy(pContext->pfnUiCallback[eVaCasUi_FnCamlockScreen]);
		BUS_MGR_Create("pContext->pfnUiCallback[eVaCasUi_FnCamlockScreen]", APP_CAS_PRIORITY, pContext->pfnUiCallback[eVaCasUi_FnCamlockScreen], hAction, 0, 0, 0);
		VA_OutputMute(TRUE, VA_MUTE_CAMLOCK);
	}
	else
#endif
	{
		BUS_MGR_Destroy(pContext->pfnUiCallback[eVaCasUi_FnCamlockScreen]);
		VA_OutputMute(FALSE, VA_MUTE_CAMLOCK);
	}

	return hError;
}

STATIC HERROR xmgr_cas_VaCommandMessageProcess(VA_CAS_UI_Context *pContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR hError = ERR_OK;
	VA_UI_EVENT *pEvent = (VA_UI_EVENT *)p2;
	HUINT8 lockValue;
	HINT32 lockLevel;

	if(pEvent == NULL)
	{
		HxLOG_Error("pEvent NULL\n");
		return ERR_FAIL;
	}

	switch(pEvent->eEventType)
	{
		case VA_UI_NEW_OPERATOR_MESSAGE:
#if defined(CONFIG_MW_CAS_VIACCESS_MULTI_OPMSG)
			VA_UI_Adapt_GetNewOpMessage(pEvent->ulScSlot);
#else
			VA_UI_Adapt_GetOpMessage(pEvent->ulScSlot);
#endif
			BUS_MGR_Create("pContext->pfnUiCallback[eVaCasUi_FnMailMessage]", APP_CAS_PRIORITY,
										pContext->pfnUiCallback[eVaCasUi_FnMailMessage], hAction, p1, p2, p3);
			break;

#ifdef CONFIG_MW_CAS_VIACCESS_FINGERPRINT
		case VA_UI_SHOW_FINGERPRINT:
#if 1 //To comile for Octo2
			AP_CAS_VA_ShowFingerPrint(hAction, p2);
#endif
			break;

		case VA_UI_HIDE_FINGERPRINT:
#if 1 //To comile for Octo2
			AP_CAS_VA_HideFingerPrint();
#endif
			break;
#endif

#if 0 // 불필요. 여기 오기 전에 local_ap_cas_va_MsgGwmCreateDialog() 에서 AP_CAS_VA_GetSmartcardState() 를 호출함으로써 이미 셋팅됨.
		case VA_UI_START_SECURITY_RESTRICTION:
		case VA_UI_STOP_SECURITY_RESTRICTION:
			if (pEvent->eEventType == VA_UI_START_SECURITY_RESTRICTION)
				s_stVaCasUiContext.stScState.nSecurityRestriction = TRUE;
			else
				s_stVaCasUiContext.stScState.nSecurityRestriction = FALSE;
#endif

		default:
			// Camlock, priority s/c message 등 처리

			HxLOG_Info("xmgr_cas_VaAdaptMessageProcess\n");
			hError = xmgr_cas_VaAdaptMessageProcess((void*)p2, (void*)&pContext->stScState, (void*)&pContext->eUiState);

			if(hError == ERR_FAIL)
			{
				HxLOG_Error("AP_CAS_VA_Adapt_MessageProcess() Fail\n");
				return hError;
			}

			lockValue = xmgr_cas_VaGetCamlockState(TRUE);
			if(lockValue & AP_CAS_VA_CAMLOCK_CTRL_ON) // security restriction 상황과 관계 없이 camlock 이 걸려 있으면...
			{
				lockLevel = xmgr_cas_VaGetCamlockLevel();
				if (lockLevel == VA_CAMLOCK_CONTROL_HIGH)
				{
					// high level camlock feature 를 지원하는 경우, black screen + not compliant 배너 표시 + 채널 전환 안됨 + FTA 시청 불가 + 메뉴 진입 안됨 (VA-5SD, VHDR-3000S)
					BUS_MGR_Destroy(pContext->pfnUiCallback[eVaCasUi_FnCamlockScreen]);
					BUS_MGR_Create("pContext->pfnUiCallback[eVaCasUi_FnCamlockScreen]", APP_CAS_HIGH_PRIORITY, pContext->pfnUiCallback[eVaCasUi_FnCamlockScreen], hAction, 0, 0, 0);
					VA_OutputMute(TRUE, VA_MUTE_CAMLOCK);
				}
				else if (lockLevel == VA_CAMLOCK_CONTROL_MEDIUM)
				{
					// medium level camlock feature 를 지원하는 경우, black screen + not compliant 배너 표시 + 채널 전환 가능 (단, 스크램블 채널 상관 없이 배너는 계속 표시) + FTA 시청 불가(?) + 배너 닫고 메뉴 진입 가능 (현재로선 해당 모델 없음)
					BUS_MGR_Destroy(pContext->pfnUiCallback[eVaCasUi_FnCamlockScreen]);
					BUS_MGR_Create("pContext->pfnUiCallback[eVaCasUi_FnCamlockScreen]", APP_CAS_PRIORITY, pContext->pfnUiCallback[eVaCasUi_FnCamlockScreen], hAction, 0, 0, 0);
					VA_OutputMute(TRUE, VA_MUTE_CAMLOCK);
				}
				else
				{
					// low level camlock feature 를 지원하는 경우, black screen + not compliant 배너 표시 + 채널 전환 가능 (단, 스크램블 채널인 경우에 한해 배너는 계속 표시) + FTA 시청 가능 + 배너 닫고 메뉴 진입 가능 (BXR-HD)
					DbSvc_Info_t 		svcInfo;

					// cam lock proc에서 fta인지 check하여 fta라면 create하고 스스로 hide
					BUS_MGR_Destroy(pContext->pfnUiCallback[eVaCasUi_FnCamlockScreen]);
					BUS_MGR_Create("pContext->pfnUiCallback[eVaCasUi_FnCamlockScreen]", APP_CAS_PRIORITY, pContext->pfnUiCallback[eVaCasUi_FnCamlockScreen], hAction, 0, 0, 0);

					hError = MGR_LIVE_GetSvcInfo (MGR_ACTION_GetMainActionId(), eLiveViewPlaySvc_MASTER, &svcInfo);
					if (DbSvc_GetCasType(&svcInfo) != eDxCAS_TYPE_FTA)
					{
						// camlock low, activate 상태에서 card extrace하면 5초간 still video 나오다가 video under-run 발생하면서 video mute 처리되는 문제 수정
						VA_OutputMute(TRUE, VA_MUTE_CAMLOCK);
					}
					else
					{
						// FTA에서는 unmute 해준다
						VA_OutputMute(FALSE, VA_MUTE_CAMLOCK);
					}
				}
			}
			else if(lockValue == AP_CAS_VA_SECURITY_CTRL_ON) // camlock 없이 security restriction 만 걸려 있으면...
			{
				BUS_MGR_Destroy(pContext->pfnUiCallback[eVaCasUi_FnCamlockScreen]);
				BUS_MGR_Create("pContext->pfnUiCallback[eVaCasUi_FnCamlockScreen]", APP_CAS_PRIORITY, pContext->pfnUiCallback[eVaCasUi_FnCamlockScreen], hAction, 0, 0, 0);
				VA_OutputMute(TRUE, VA_MUTE_CAMLOCK);
			}
			else
			{
				BUS_MGR_Destroy(pContext->pfnUiCallback[eVaCasUi_FnCamlockScreen]);
				VA_OutputMute(FALSE, VA_MUTE_CAMLOCK);
			}

			break;
	}

	return hError;
}

STATIC HERROR xmgr_cas_VaCheckMessageProcess(VA_CAS_UI_Context *pContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR hError = ERR_OK;
	VA_UI_EVENT *pEvent = (VA_UI_EVENT *)p2;

	if(pEvent == NULL)
	{
		HxLOG_Error("pEvent NULL\n");
		return ERR_FAIL;
	}

	// error 창은 닫아놓자..
	BUS_MGR_Destroy(pContext->pfnUiCallback[eVaCasUi_FnSystemMessage]);

	switch(pEvent->eEventType)
	{
		case VA_UI_DESCRAMBLING_KO_MATURITY_RATING:
#if 1 //To comile for Octo2
			AP_CAS_VA_AgreeMaturityRate(pEvent->ulAcsId, pEvent->ulStbStreamHandle);
#endif
			break;

		case VA_UI_DESCRAMBLING_OK_PREVIEW:
#if 1 //To comile for Octo2
			AP_CAS_VA_Preview(pEvent->ulAcsId, pEvent->ulStbStreamHandle);
#endif
			break;

		case VA_UI_DESCRAMBLING_OK_RIGHT_OK:
#if 1 //To comile for Octo2
			AP_CAS_VA_DestroyPreview();
#endif
			break;

		default:
			hError = ERR_FAIL;
			break;
	}

	return hError;
}

#if defined(CONFIG_PROD_VHDR3000S) || defined(CONFIG_PROD_VAHD3100S)
#define VA_BBX_INIT_CHECK // bbx update 확인을 위해서..
#endif

#ifdef VA_BBX_INIT_CHECK
STATIC HBOOL s_bIsFirstMsg = TRUE;
extern BUS_Result_t AP_NoDbDialog_Proc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
#endif

STATIC HERROR xmgr_cas_VaFindCasUiFnIndex(VA_CAS_UI_Context *pContext, VA_CAS_UI_CUSTOM_SESSION_e eUiSession, VA_CAS_UI_FN_e *peFnIndex, VA_CAS_UI_e *peUiState)
{
	HUINT32 i;

	for(i = 0; i < VA_CAS_UI_CUSTOM_SESSION_MAX; i++)
	{
		if(pContext->pstCasUiCustomSession[i].eUiSession == eUiSession)
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

STATIC HERROR xmgr_cas_VaRequestCasUiCustomSessionStart(VA_CAS_UI_Context *pContext,
																			VA_CAS_UI_CUSTOM_SESSION_e eUiSession,
																			HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t					hRes = ERR_BUS_NO_ERROR;
	HERROR							hErr;
	VA_CAS_UI_FN_e					eFunction;
	VA_CAS_UI_e						eUiState;
// hjlee3, just for compile	OxMgrCasUiEvtReqSessionStart_t	stEvent;
	hErr = xmgr_cas_VaFindCasUiFnIndex(pContext, eUiSession, &eFunction, &eUiState);

	if(hErr == ERR_FAIL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if (BUS_MGR_Get(pContext->pfnUiCallback[eFunction]) == NULL)
	{
		/* UI Proc이 create되지 않은 상태에서는 UI Proc Create후 Application으로 Session Start를 request 한다. */
		BUS_MGR_Create(NULL, APP_CAS_PRIORITY,
						pContext->pfnUiCallback[eFunction], hAction, p1, p2, p3);

// hjlee3, just for compile		stEvent.ulSessionType = eUiSession;
// hjlee3, just for compile		if(MGR_CAS_SendUiEvent (eDxCAS_GROUPID_VA, 0, eMEVT_CASUI_REQ_OPEN_SESSION, &stEvent) != ERR_OK)
		{
			HxLOG_Critical("\n\n");
		}

		HxLOG_Print("OK!! Created by Octo Message [0x%x]\n", message);
	}

	hRes = BUS_SendMessage(pContext->pfnUiCallback[eFunction], message, hAction, p1, p2, p3);

	VA_CAS_PARAM_WARNING_REMOVE(pContext, message, hAction, p1, p2, p3);
	return hRes;

}
STATIC BUS_Result_t xmgr_cas_VaMsgEvtCasCmd(VA_CAS_UI_Context *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t	hRes = ERR_BUS_NO_ERROR;
	HUINT32		ulActionId = SVC_PIPE_GetActionId(hAction);

	if (xmgr_cas_VaCheckActionID(ulActionId) != ERR_OK)
	{
		HxLOG_Error("fail, p1(%x), p2(%x)\n",p1, p2);
		return ERR_BUS_MESSAGE_PASS;
	}


#if 1	//1 To do 준비되면 열자.
	if(xmgr_cas_VaCommandMessageProcess(pContext, hAction, p1, p2, p3) != ERR_OK)
	{
		hRes = ERR_BUS_MESSAGE_BREAK;
	}
#endif

	VA_CAS_PARAM_WARNING_REMOVE(pContext, message, hAction, p1, p2, p3);
	return hRes;
}
STATIC HERROR		xmgr_cas_VaMsgEvtCasFailOrCheck(VA_CAS_UI_Context *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
// hjlee3, just for compile	OxMgrCasUiEvtReqSessionStart_t stEvent;
	BUS_Result_t	hRes = ERR_BUS_NO_ERROR;
	HUINT32		ulActionId = SVC_PIPE_GetActionId(hAction);


	HxLOG_Print("message[0x%x]\n", message);

	if (xmgr_cas_VaCheckActionID(ulActionId) != ERR_OK)
	{
		HxLOG_Error("fail, p1(%x), p2(%x)\n",p1, p2);
		return ERR_BUS_MESSAGE_PASS;
	}

	hRes = xmgr_cas_VaRequestCasUiCustomSessionStart(pContext, eVaCasUi_SessionSystemMessage, message, hAction, p1, p2, p3);

	VA_CAS_PARAM_WARNING_REMOVE(pContext, message, hAction, p1, p2, p3);

	return hRes;
}
STATIC HERROR xmgr_cas_VaCheckCasUiCustomSessionStart(VA_CAS_UI_Context *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR							hErr;
	VA_CAS_UI_CUSTOM_SESSION_e	eUiSession = (VA_CAS_UI_CUSTOM_SESSION_e)p1;
	VA_CAS_UI_FN_e					eFunction;
	VA_CAS_UI_e						eUiState;
// hjlee3, just for compile	OxMgrCasUiEvtSessionStarted_t		stEvent;

	hErr = xmgr_cas_VaFindCasUiFnIndex(pContext, eUiSession, &eFunction, &eUiState);
	if(hErr == ERR_FAIL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if (BUS_MGR_Get(pContext->pfnUiCallback[eFunction]) == NULL)
	{

		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	/* Session Start를 받으면 Started를 UI handle(UI Proc function pointer value)과 같이 Application으로 보내준다. */
// hjlee3, just for compile	stEvent.ulSessionType = eUiSession;
// hjlee3, just for compile	stEvent.ulUiHandle = (HUINT32)pContext->pfnUiCallback[eFunction];
// hjlee3, just for compile	if(MGR_CAS_SendUiEvent (eDxCAS_GROUPID_VA, 0, eMEVT_CASUI_SESSION_OPENED, &stEvent) != ERR_OK)
	{
		HxLOG_Critical("\n\n");
	}

	pContext->eUiState |= eUiState;

	BUS_SendMessage(pContext->pfnUiCallback[eFunction], message, hAction, p1, p2, p3);

	VA_CAS_PARAM_WARNING_REMOVE(pContext, message, hAction, p1, p2, p3);

	return ERR_OK;
}
STATIC BUS_Result_t		xmgr_cas_VaMsgCasUiSessionStart(VA_CAS_UI_Context *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 ulUiSession = (HUINT32)p1;
/*
	Todo: eMgrCasUiSession_Menu  정의해라. hjlee3
		  MGR_CAS_SendUiEvent
*/
//	if(ulUiSession == eMgrCasUiSession_Menu)
	if(1)  // hjlee3, just for compile
	{
// hjlee3, just for compile		OxMgrCasUiEvtSessionStarted_t		stEvent;

		/* Session Start를 받으면 Started를 UI handle(UI Proc function pointer value)과 같이 Application으로 보내준다. */
// hjlee3, just for compile		stEvent.ulSessionType = p1;
// hjlee3, just for compile		stEvent.ulUiHandle = (HUINT32)pContext->pfnUiCallback[eVaCasUi_FnCasMenu];

// hjlee3, just for compile		if(MGR_CAS_SendUiEvent (eDxCAS_GROUPID_VA, 0, eMEVT_CASUI_SESSION_OPENED, &stEvent) != ERR_OK)
		{
			HxLOG_Critical("\n\n");
		}

		if (BUS_MGR_Get(pContext->pfnUiCallback[eVaCasUi_FnCasMenu]) == NULL)
		{
			BUS_MGR_Create("pContext->pfnUiCallback[eCxCasUi_FnCasMenu]", APP_CAS_PRIORITY,
							pContext->pfnUiCallback[eVaCasUi_FnCasMenu], hAction, p1, p2, p3);

			pContext->eUiState |= eVaCasUi_FnCasMenu;
			HxLOG_Print("OK! Created by OAPI [0x%x][eMgrCasUiSession_Menu]\n", message);
		}
		else
		{
			BUS_SendMessage(pContext->pfnUiCallback[eVaCasUi_FnCasMenu], message, hAction, p1, p2, p3);
		}
	}
	else
	{
		if(xmgr_cas_VaCheckCasUiCustomSessionStart(pContext, message, hAction, p1, p2, p3) != ERR_OK)
		{
			HxLOG_Critical("\n\n");
		}
	}

	VA_CAS_PARAM_WARNING_REMOVE(pContext, message, hAction, p1, p2, p3);
	return ERR_BUS_MESSAGE_BREAK;
}


STATIC HERROR xmgr_cas_VaCheckCasUiCustomSessionStop(VA_CAS_UI_Context *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR							hErr;
	VA_CAS_UI_CUSTOM_SESSION_e	eUiSession = (VA_CAS_UI_CUSTOM_SESSION_e)p1;
	VA_CAS_UI_FN_e					eFunction;
	VA_CAS_UI_e						eUiState;

	hErr = xmgr_cas_VaFindCasUiFnIndex(pContext, eUiSession, &eFunction, &eUiState);

	if(hErr == ERR_FAIL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if (BUS_MGR_Get(pContext->pfnUiCallback[eFunction]) != NULL)
	{
		BUS_MGR_Destroy(pContext->pfnUiCallback[eFunction]);

		pContext->eUiState &= ~eUiState;
		HxLOG_Print("OK! Destroy by OAPI Session End. eFunction[0x%x][]\n", eFunction);
	}

	VA_CAS_PARAM_WARNING_REMOVE(pContext, message, hAction, p1, p2, p3);

	return ERR_OK;
}

STATIC BUS_Result_t		xmgr_cas_VaMsgCasUiSessionStop(VA_CAS_UI_Context *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32	ulUiSession = (HUINT32)p1;

//	if(ulUiSession == eMgrCasUiSession_Menu) hjlee3, just for compile
	if(1)
	{
		if (BUS_MGR_Get(pContext->pfnUiCallback[eVaCasUi_FnCasMenu]) != NULL)
		{
			BUS_MGR_Destroy(pContext->pfnUiCallback[eVaCasUi_FnCasMenu]);

			pContext->eUiState &= ~eVaCasUi_FnCasMenu;
			HxLOG_Print("OK! Destroy by OAPI Session End [0x%x][eMgrCasUiSession_Menu]\n", message);
		}
	}
	else
	{
		if(xmgr_cas_VaCheckCasUiCustomSessionStop(pContext, message, hAction, p1, p2, p3) != ERR_OK)
		{
			HxLOG_Critical("\n\n");
		}
	}

	VA_CAS_PARAM_WARNING_REMOVE(pContext, message, hAction, p1, p2, p3);
	return ERR_BUS_MESSAGE_BREAK;
}

STATIC BUS_Result_t xmgr_cas_VaMsgCasUiInputNotify(VA_CAS_UI_Context *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{

	BUS_Callback_t	pfnUiCallback = (BUS_Callback_t)p1;

	HxLOG_Print("[%s][%04d](Session : 0x%x), (p1 : 0x%x), (p3 : 0x%x)\n", __FUNCTION__,__LINE__,p1, p2, p3);

	if (BUS_MGR_Get(pfnUiCallback) != NULL)
	{
		BUS_SendMessage(pfnUiCallback, message, hAction, p1, p2, p3);
	}

	return ERR_BUS_MESSAGE_BREAK;
}


STATIC HERROR xmgr_cas_VaMsgGwmCreateDialog(VA_CAS_UI_Context *pContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR	hError = ERR_OK;
	AP_CAS_VA_GetSmartcardState(&pContext->stScState);

	HxLOG_Info("EVENT(%s) Process State : Security(%d) CamLockCtrlLevel(%s) CamLock(%s) ScState(%s)\n",
		xmgr_cas_VaGetMessageName(message),
		pContext->stScState.nSecurity,
		VA_UI_Debug_GetCamlockLevel(pContext->stScState.nCamLockCtrlLevel), VA_UI_Debug_GetCamlockState(pContext->stScState.nCamlock),
		VA_UI_Debug_GetScState(pContext->stScState.nState));
#ifdef VA_BBX_INIT_CHECK
	if (s_bIsFirstMsg)
	{
		s_bIsFirstMsg = FALSE;

		if (VA_UTIL_GetBbxInitDisplay() == FALSE)
		{
#if 0 // not use for octo2.0
			// no channel dialog가 표시된 경우만 hidden menu를 표시한다.. 아닌경우는 이미 채널이 있는 거고, 공장이 아니므로 표시할 필요가 없다.
			if(BUS_MGR_Get(AP_NoDbDialog_Proc) != NULL)
			{
				AP_HiddenSettings_CreateFusingInfo();
			}
#endif
			VA_UTIL_SetBbxInitDisplay();
		}
	}
#endif

	switch (message)
	{
		case eMEVT_CAS_CMD :
			hError = xmgr_cas_VaCommandMessageProcess(pContext, hAction, p1, p2, p3);
			break;

		case eMEVT_CAS_FAIL:
			if (message == eMEVT_CAS_FAIL)
			{
				// security restriction or camlock 상태이면 fail 처리 안함..
				if (xmgr_cas_VaGetCamlockState(TRUE) == 0) // if (xmgr_cas_VaGetCamlockState(TRUE) != ERR_OK)
				{
					hError = xmgr_cas_VaFailMessageProcess(pContext, hAction, p1, p2, p3);
				}
				else if ((xmgr_cas_VaGetCamlockState(TRUE) != 0)
					&& (xmgr_cas_VaGetCamlockLevel() == VA_CAMLOCK_CONTROL_LOW))
				{
					// cam lock 상태이고, level low이면 hide된 camlock appl을 show한다
					if(BUS_MGR_Get(pContext->pfnUiCallback[eVaCasUi_FnCamlockScreen]) != NULL)
					{
						#if 0 //To comile for Octo2
						GWM_APP_SetUIShow(pContext->pfnUiCallback[eVaCasUi_FnCamlockScreen]);
						#endif
					}
				}
			}
			else if (message == eMEVT_CAS_CHECK)
			{
				hError = xmgr_cas_VaCheckMessageProcess(pContext, hAction, p1, p2, p3);
			}
			hError = xmgr_cas_VaMsgEvtCasFailOrCheck(pContext, message, hAction, p2, 0, 0);
			break;


		default :
			hError = ERR_FAIL;
			break;
	}

	return hError;
}

STATIC HERROR xmgr_cas_VaMsgGwmCreate(VA_CAS_UI_Context *pContext)
{


	HxSTD_memset(pContext, 0, sizeof(VA_CAS_UI_Context));
	pContext->pfnUiCallback[eVaCasUi_FnCasMenu] = xmgr_cas_VaCbGetUiProc(eVaCasUi_FnCasMenu);
	pContext->pfnUiCallback[eVaCasUi_FnSystemMessage] = xmgr_cas_VaCbGetUiProc(eVaCasUi_FnSystemMessage);
	pContext->pfnUiCallback[eVaCasUi_FnCheckMessage] = xmgr_cas_VaCbGetUiProc(eVaCasUi_FnCheckMessage);
	pContext->pfnUiCallback[eVaCasUi_FnCamlockScreen] = xmgr_cas_VaCbGetUiProc(eVaCasUi_FnCamlockScreen);
#if defined(CONFIG_MW_CAS_VIACCESS_MULTI_OPMSG)
	pContext->pfnUiCallback[eVaCasUi_FnMailIcon] = xmgr_cas_VaCbGetUiProc(eVaCasUi_FnMailIcon);
#else
	pContext->pfnUiCallback[eVaCasUi_FnMailMessage] = xmgr_cas_VaCbGetUiProc(eVaCasUi_FnMailMessage);
#endif

	pContext->pstCasUiCustomSession = xmgr_cas_VaGetCustomSession();

#if 0
 	TODO: hjlee3 MGR_CAS_RegisterCasUiProc 관련 해서 확인 할 것.
	if(MGR_CAS_RegisterCasUiProc(eDxCAS_GROUPID_VA, 0/*nSlotId*/, xporc_cas_vaMain) != ERR_OK)
	{
		HxLOG_Critical("\n\n");
	}
#endif

#if 0 //To comile for Octo2
	pContext->pfnNotificationProc	= xmgr_cas_VaNotification_Proc;
	pContext->pfnCheckMessageProc	= xmgr_cas_VaCheckMessage_Proc;
	pContext->pfnCamlockScreenProc	= xmgr_cas_VaCamlockScreen_Proc;
#if defined(CONFIG_MW_CAS_VIACCESS_MULTI_OPMSG)
	pContext->pfnOpMessageProc	=xmgr_cas_VaOpMessageIcon_Proc;	// 메시지를 바로 표시하지 않고 icon을 먼저 표시한다. AP_CAS_VA_OpMessage_Proc;
#else
	pContext->pfnOpMessageProc	= xmgr_cas_VaOpMessage_Proc;
#endif
#endif
	return ERR_OK;
}


#if defined(CONFIG_DEBUG)

STATIC void xmgr_cas_VaPrintCasEventName(HINT32 message, HINT32 CasMsg)
{
	VA_UI_EVENT *pEvent = (VA_UI_EVENT *)CasMsg;

	if (pEvent != NULL)
	{
		switch (message)
		{
			case eMEVT_CAS_FAIL :
				HxLOG_Info("eMEVT_CAS_FAIL [%s / %x]\n", VA_UI_Debug_GetEventName(pEvent->eEventType), pEvent->eEventType);
				break;
			case eMEVT_CAS_CMD :
				HxLOG_Info("eMEVT_CAS_CMD [%s / %x]\n", VA_UI_Debug_GetEventName(pEvent->eEventType), pEvent->eEventType);
				break;
			case eMEVT_CAS_CHECK :
				HxLOG_Info("eMEVT_CAS_CHECK [%s / %x]\n", VA_UI_Debug_GetEventName(pEvent->eEventType), pEvent->eEventType);
				break;
			default:
				return;
		}
	}
	else
	{
		HxLOG_Error("pEvent is NULL !!\n");
	}

	return;
}

STATIC HUINT8 *xmgr_cas_VaGetMessageName(HINT32 message)
{
	switch(message)
	{
		ENUM_TO_STR(eMEVT_BUS_CREATE);
		ENUM_TO_STR(eMEVT_BUS_DESTROY);
		ENUM_TO_STR(eMEVT_CAS_CLOSE);
		ENUM_TO_STR(eMEVT_CAS_OK);
		ENUM_TO_STR(eMEVT_CAS_FAIL);
		ENUM_TO_STR(eMEVT_CAS_CMD);
		ENUM_TO_STR(eMEVT_CAS_CHECK);
		ENUM_TO_STR(eMEVT_CAS_SC_REMOVED);
		ENUM_TO_STR(eMEVT_CAS_SC_INSERTED);
		ENUM_TO_STR(eMEVT_PG_PIN_CHECK_OK);
		ENUM_TO_STR(eMEVT_PG_PIN_CHECK_DENIED);
		//ENUM_TO_STR(MSG_GWM_KEYDOWN);
		default:
			return NULL;
	}
}

#endif

// TODO: VA_GetCamLockControlLevel() 와 함께 통일시키는 것이 좋겠음.
// HERROR xmgr_cas_VaGetCamlockState(HBOOL bCheckSecurityRestriction) // if return ERR_FAIL : camlock 처리 필요 없음. if return ERR_OK : camlock 처리 필요.
STATIC HUINT8 xmgr_cas_VaGetCamlockState(HBOOL bCheckSecurityRestriction)
{
	HUINT8 result = 0x00; // no control for both of security restriction and camlock

	if(VA_MAIN_TaskStarted() == FALSE)
		return 0x00; // return ERR_FAIL;

	#if 0 // 이 함수가 불리는 곳에서 이미 호출되므로 중복됨.
	AP_CAS_VA_GetSmartcardState(&s_stVaCasUiContext.stScState);
	#endif

	// security restriction도 camlock과 비슷한 방법으로 처리한다.
	if (bCheckSecurityRestriction && s_stVaCasUiContext.stScState.nSecurity)
		result |= AP_CAS_VA_SECURITY_CTRL_ON; // return ERR_OK;

//	if (VA_GetCamLockControlLevel())
	if (s_stVaCasUiContext.stScState.nCamLockCtrlLevel) // 사실상 VA_CAMLOCK_CONTROL_NO (0) 은 현재 코드 구현상 편의를 위해 추가한 값이지 실제 VA 모델에서 쓰일 수 없으므로 이 조건 체크도 필요 없을 듯...
	{
		// camlock이 deactivate 된 상태이으로 camlock 처리 필요 없음.
		if(s_stVaCasUiContext.stScState.nCamlock == VA_UI_CAMLOCK_NOT_ACTIVATED)
		{
			/* nothing to do */ // return ERR_FAIL;
		}
		else
		{
			// 카드가 준비되었고 camlock ok 가 떨어졌으므로 camlock 처리 필요 없음. 나머지 경우는 camlock 처리 필요.
			if((s_stVaCasUiContext.stScState.nState == VA_UI_SMARTCARD_READY) &&
				(s_stVaCasUiContext.stScState.nCamlock == VA_UI_CAMLOCK_ACTIVATED_OK))
			{
				/* nothing to do */ // return ERR_FAIL;
			}
			else
			{
				result |= AP_CAS_VA_CAMLOCK_CTRL_ON; // return ERR_OK;
			}
		}
	}
	else
	{
		/* nothing to do */ // return ERR_FAIL;
	}

	HxLOG_Info("CheckSecurity(%d) Security(%d) CamLockCtrlLevel(%s) CamLock(%s) ScState(%s) -> %02x\n",
		bCheckSecurityRestriction, s_stVaCasUiContext.stScState.nSecurity,
		VA_UI_Debug_GetCamlockLevel(s_stVaCasUiContext.stScState.nCamLockCtrlLevel), VA_UI_Debug_GetCamlockState(s_stVaCasUiContext.stScState.nCamlock),
		VA_UI_Debug_GetScState(s_stVaCasUiContext.stScState.nState), result);

	return result;
}

STATIC HINT32 xmgr_cas_VaGetCamlockLevel(void)
{
	return s_stVaCasUiContext.stScState.nCamLockCtrlLevel;
}


STATIC BUS_Result_t xmgr_cas_VaCheckRcuEventProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
#if 0 // not use for octo2.0
	switch(message)
	{
		case MSG_GWM_KEYDOWN:
		case MSG_GWM_KEYUP:
			AP_KeyConv_UiCommon(&p1);

			if(p1 != KEY_STANDBY)
			{
				HxLOG_Info("%s\n", PAL_KEY_GetKeyString(p1));

				// high level camlock feature 를 지원하지 않으면 이 조건 체크에서 MESSAGE_BREAK 될 일은 없다. 하지만...
				// security restriction 을 camlock 과 동일하게 AP_CAS_VA_CamlockScreen_Proc() 에서 핸들링 하도록 해놓았으므로
				// high level camlock feature 를 지원하지 않아도 security restriction 에서의 키 처리를 위해서 xmgr_cas_VaCheckRcuEventProc()이 필요함.
				AP_CAS_VA_GetSmartcardState(&s_stVaCasUiContext.stScState);
				if ((xmgr_cas_VaGetCamlockLevel() == VA_CAMLOCK_CONTROL_HIGH) && xmgr_cas_VaGetCamlockState(FALSE)) // if (xmgr_cas_VaGetCamlockState(FALSE) == ERR_OK)
					return MESSAGE_BREAK;
			}

			break;

		default:
			break;
	}
#endif
	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

#define ______PUBLIC_FUNCTION______________________________________________________


HERROR xmgr_cas_VaStart(void)
{
	// VA task를 시작시킨다... (ACS Library 동작)
	VAMGR_Task_Init();

	return ERR_OK;
}

HERROR xmgr_cas_VaInitialize(void)
{
	VA_CAS_UI_Context 	*pContext = xmgr_cas_VaGetContextUI();

	pContext->eUiState = eCAS_VA_UI_None;

#if 0 // 논리적으로 이게 맞지만 기존에 아래와 같이 0 으로 밀어버리네...
	pContext->stScState.nState = VA_UI_SC_EXTRACTED;
	pContext->stScState.nCamlock = VA_UI_CAMLOCK_ACTIVATED_NO_CARD; // or VA_UI_CAMLOCK_NOT_ACTIVATED ? see VA_UI_Adapt_Init()
	pContext->stScState.nSecurityRestriction = FALSE;
#else
	VK_memset32((void*)&pContext->stScState, 0, sizeof(CAS_VA_SC_State_t));
#endif

	return ERR_OK;
}

BUS_Result_t 	xporc_cas_VaMain(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t		eBusResult = ERR_BUS_NO_ERROR;
	HUINT32				ulActionId = SVC_PIPE_GetActionId(hAction);
	VA_CAS_UI_Context	*pContext = xmgr_cas_VaGetContextUI();
//	VA_UI_EVENT			*pEvent = (VA_UI_EVENT *)p2;

#ifdef CONFIG_DEBUG

	{
		HUINT8 *pMsg = xmgr_cas_VaGetMessageName(message);

		if(pMsg != NULL)
		{
			if(xmgr_cas_VaCheckActionID(ulActionId) == ERR_OK)
			{
				HxLOG_Info("%s, ActId(%x), p1(%x), p2(%x), p3(%x)\n", pMsg, ulActionId, p1, p2, p3);
			}
		}
	}
#endif
	switch(message)
	{
		case eMEVT_BUS_CREATE :
//			security restriction 을 camlock 과 동일하게 AP_CAS_VA_CamlockScreen_Proc() 에서 핸들링 하도록 해놓았으므로
//			camlock feature 를 지원하지 않아도 security restriction 에서의 키 처리를 위해서 xmgr_cas_VaCheckRcuEventProc()은 항상 create 되어야 하기 때문에 아래와 같이 수정함.
//			if (VA_GetCamLockControlLevel())
//			{
#if 0 // not use for octo2.0
				if(BUS_MGR_Get(xmgr_cas_VaCheckRcuEventProc) == NULL)
				{
					BUS_MGR_Create("xmgr_cas_VaCheckRcuEventProc", APP_CAS_HIGH_PRIORITY, (BUS_Callback_t)xmgr_cas_VaCheckRcuEventProc, 0, 0, 0, 0);
				}
#endif
//			}

			xmgr_cas_VaMsgGwmCreate(pContext);
			break;

		case eMEVT_CAS_FAIL :
		case eMEVT_CAS_CMD :
		case eMEVT_CAS_CHECK :
			if (xmgr_cas_VaCheckActionID(ulActionId) != ERR_OK)
			{
				HxLOG_Info("%s, but [0x%x] isn't main actiod id\n", xmgr_cas_VaGetMessageName(message), ulActionId);
				return MESSAGE_PASS;
			}

			if(p1 != eDxCAS_GROUPID_VA)
			{
				HxLOG_Info("%s, but p1[%x] isn't eDxCAS_GROUPID_VA\n", xmgr_cas_VaGetMessageName(message), p1);
				return MESSAGE_PASS;
			}

			if(p2 == 0)
			{
				HxLOG_Info("%s, but p2[%x] is ZERO\n", xmgr_cas_VaGetMessageName(message), p2);
				return MESSAGE_PASS;
			}

#ifdef CONFIG_DEBUG
			xmgr_cas_VaPrintCasEventName(message, p2);
#endif
			xmgr_cas_VaMsgGwmCreateDialog(pContext, message, hAction, p1, p2, p3);
			break;

#if 0// ysmoon_20130812
		case MSG_OAPI_CASUI_SESSION_START:
			eBusResult = xmgr_cas_VaMsgCasUiSessionStart(pContext, message, hAction, p1, p2, p3);
			break;

		case MSG_OAPI_CASUI_SESSION_STOP:
			eBusResult = xmgr_cas_VaMsgCasUiSessionStop(pContext, message, hAction, p1, p2, p3);
			break;

		case MSG_OAPI_CASUI_INPUT_NOTIFY:
			eBusResult = xmgr_cas_VaMsgCasUiInputNotify(pContext, message, hAction, p1, p2, p3);
			break;
#endif
		case eMEVT_PG_PIN_CHECK_OK:
		case eMEVT_PG_PIN_CHECK_DENIED:
			if (xmgr_cas_VaCheckActionID(ulActionId) != ERR_OK)
				return MESSAGE_PASS;

#if 0 // not use for octo2.0
			AP_CAS_VA_AgreeMaturityRateDestory();

			if(message == eMEVT_PG_PIN_CHECK_OK)
			{
				AP_CAS_VA_HideMessageBox();
			}

			if(message == eMEVT_PG_PIN_CHECK_DENIED)
			{
				if(p1 == eVA_PinChangeError_ScLocked)
				{
#if defined(CONFIG_OP_NORDIG_BOXER)
					xmgr_cas_VaAdaptShowBoxerScLockMsg();
#else
					xmgr_cas_VaShowMessageBoxByLoc(LOC_CAS_VA_ERR_SC_LOCKED_ID);
#endif
				}
				else if(p1 == eVA_PinChangeError_NoPincode)
				{
					xmgr_cas_VaShowMessageBoxByLoc(LOC_CAS_VA_MSG_SC_NO_PINCODE_IN_CARD_ID);
				}
				else
					;
			}
#endif
			break;

		case eMEVT_CAS_OK :
		case eMEVT_CAS_CLOSE :
			if (xmgr_cas_VaCheckActionID(ulActionId) != ERR_OK)
				return MESSAGE_PASS;

#if 0 //To comile for Octo2
			AP_CAS_VA_DestroyPreview();
#endif
			xmgr_cas_VaMsgGwmDeleteDialog(pContext);
			xmgr_cas_VaMsgGwmDeleteMainProc(pContext);

			break;

		case eMEVT_CAS_SC_INSERTED:
		case eMEVT_CAS_SC_REMOVED:
			break;

		case eMEVT_BUS_DESTROY :
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

