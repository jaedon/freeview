/**
	@file     xmgr_live_lock_base.c
	@brief    file_name & simple comment.

	Description: Applicatuion for searching Services\n
	Module: Application(Layer), Menu(module) \n
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


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <db_param.h>
#include <db_svc.h>

#include <svc_ch.h>
#include <svc_si.h>
#include <svc_av.h>
#include <svc_cas.h>
#include <svc_sbtl.h>
#include <svc_ttx.h>

#include <bus.h>

#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_pg.h>
#include <mgr_live.h>
#include <xmgr_zapping.h>
#include <mgr_rscmap.h>
#include <mgr_cas.h>

#include <mgr_pg.h>
#include <mgr_output.h>
#include <mgr_3rdparty.h>

// Local/Member Header Files
#include <mgr_live.h>
#include <xmgr_live.h>
#include "../local_include/_xmgr_live.h"
#include "../local_include/_xmgr_live_lock.h"

// 아래 CI 관련 include는 coding convention에 어긋남. 정리 되면 수정 필요
#if defined(CONFIG_MW_CI_PLUS_V_1_3)
#include <ci_ctrl.h>
#include <ci_plus.h>
#endif

#define ___LOCAL_DEFINITIONS___

#define ___LOCAL_DATATYPES___

#define ___LOCAL_VARIABLES___
STATIC xMGR_LiveLock_Context_t			 s_astXmgrLiveLock_Context[eActionId_MAX];

#define ___LOCAL_FUNCTIONS___
STATIC HINT8* _mgr_LiveLock_base_GetUiStateStr(MgrLiveLock_UiState_e eUiState);
STATIC HINT8* _mgr_LiveLock_base_GetAppStateStr(xMGR_LiveLock_AppState_e eAppState);
STATIC void _mgr_LiveLock_base_ShowProgram(xMGR_LiveLock_Context_t *pstContext);

#define _____CONTEXT_FUNCTIONS_____
INLINE STATIC xMGR_LiveLock_Context_t *_mgr_LiveLock_base_getContext (HUINT32 ulActionId)
{
	if (((ulActionId >= eActionId_VIEW_FIRST) && (ulActionId <= eActionId_VIEW_LAST)) ||
		((ulActionId >= eActionId_BG_FIRST) && (ulActionId <= eActionId_BG_LAST)))
	{
		return s_astXmgrLiveLock_Context + ulActionId;
	}

	return NULL;
}

STATIC HERROR _mgr_LiveLock_base_initContext (void)
{
	HUINT32					 ulActionId;

	HxSTD_MemSet (s_astXmgrLiveLock_Context, 0, sizeof(xMGR_LiveLock_Context_t) * eActionId_MAX);

	for (ulActionId = 0; ulActionId < eActionId_MAX; ulActionId++)
	{
		xMGR_LiveLock_Context_t			*pstContext = s_astXmgrLiveLock_Context + ulActionId;

		pstContext->ulActionId = ulActionId;
		pstContext->eUiState = eLockUi_NONE;
	}

	return ERR_OK;
}

STATIC HERROR _mgr_LiveLock_base_resetUiState(HUINT32 ulActionId, HBOOL bSvcStart)
{
	HERROR							hError = ERR_FAIL;
	xMGR_LiveLock_Context_t			*pstContext = NULL;

	if (ulActionId < eActionId_MAX)
	{
		pstContext = &s_astXmgrLiveLock_Context[ulActionId];

		if (bSvcStart != FALSE)
		{
			// Start
			pstContext->eUiState = eLockUi_OK;
		}
		else
		{
			// Stop
			pstContext->eUiState = eLockUi_NONE;
		}

		hError = ERR_OK;
	}

	return hError;
}

INLINE STATIC HERROR _mgr_LiveLock_base_ResetCommonContextFlags (xMGR_LiveLock_Context_t *pstContext, Handle_t hNewSvc, MgrLive_TuningMode_t tuningMode)
{
	if((tuningMode & ePINCODE_PASSED) != 0)
	{
		/* PIN code들을 pass로 강제 하라고 하네... */
		pstContext->bLockPinPassed = TRUE;
		pstContext->bRatingPinPassed = TRUE;
		pstContext->bSCPinPassed = TRUE;
	}
	else if((tuningMode & eKEEP_PINPASS) == 0)
	{
		/* PIN code pass flag를 유지하지 않을 경우에만 update */
		if(hNewSvc == pstContext->hSvc)
		{
			/* Service가 바뀌지 않는 경우에는 zap info에 백업되어 있는 lock pin passed flag를 반영 */
			HxLOG_Print ("[AP:ZAP(%d)] \t=> Same service handle. Apply last lock pin passed flag.\n",pstContext->ulActionId);
			pstContext->bLockPinPassed = MGR_LIVE_SvcLockIsPassed(pstContext->ulActionId, hNewSvc);
		}
		else
		{
			pstContext->bLockPinPassed = FALSE;
			MGR_LIVE_SvcLockResetPassed(pstContext->ulActionId);
		}

		pstContext->bRatingPinPassed = FALSE;
		pstContext->bSCPinPassed = FALSE;
	}
	else
	{
		/* 현재 상태 유지 */
	}

	return ERR_OK;
}

BUS_Result_t _mgr_LiveLock_base_SendBusMessage(BUS_Callback_t appCallback, HINT32 ulMsg, Handle_t hAction, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3)
{
#if defined(CONFIG_SUPPORT_FCC)
	HUINT32		ulActionId = SVC_PIPE_GetActionId(hAction);

	if (ulActionId != MGR_ACTION_GetMainActionId())
	{
		// main action(main live)인 경우에만 msg를 날리자
		return ERR_BUS_NO_ERROR;
	}
#endif

	return BUS_SendMessage(appCallback, ulMsg, hAction, lParam1, lParam2, lParam3);
}

BUS_Result_t _mgr_LiveLock_base_PostBusMessage(BUS_Callback_t appCallback, HINT32 ulMsg, Handle_t hAction, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3)
{
#if defined(CONFIG_SUPPORT_FCC)
	HUINT32 	ulActionId = SVC_PIPE_GetActionId(hAction);

	if (ulActionId != MGR_ACTION_GetMainActionId())
	{
		// main action(main live)인 경우에만 msg를 날리자
		return ERR_BUS_NO_ERROR;
	}
#endif

	return BUS_PostMessage(appCallback, ulMsg, hAction, lParam1, lParam2, lParam3);
}

#define _____LOCAL_FUNCTIONS_____
STATIC HERROR _mgr_LiveLock_base_AVHide(xMGR_LiveLock_Context_t *pstContext, HUINT32 ulClientMask, HBOOL bHideFlag)
{
	HERROR		hErr = ERR_FAIL;

	if( NULL == pstContext )
		return hErr;

	if( eMgrLiveMode_LiveStreaming == pstContext->eLiveMode )
	{
		/**
		 *	author : hsseo
		 *	note : Action VIEW의 AV에 관한 내용을 금지함
		**/
	}
	else
	{
#if defined(CONFIG_SUPPORT_FCC)
		// main action id인 경우에만 video hide control
		if (pstContext->ulActionId == MGR_ACTION_GetMainActionId())
#endif
		{
			hErr = SVC_AV_SetAudioHide(pstContext->ulActionId, ulClientMask,	bHideFlag);
			if (hErr)
			{
				HxLOG_Error("SVC_AV_SetAudioHide ret :0x%X\n", hErr);
			}

			hErr = SVC_AV_SetVideoHide(pstContext->ulActionId, ulClientMask, bHideFlag);
			if (hErr)
			{
				HxLOG_Error("SVC_AV_SetVideoHide ret :0x%X\n", hErr);
			}
		}
	}


	return hErr;
}


#define _____TVSERVICE_FUNCTIONS_____
STATIC HERROR _mgr_LiveLock_base_StartPid(xMGR_LiveLock_Context_t *pstContext)
{
	HBOOL			bHideFlag;

#if defined(CONFIG_APCORE_PC_SVC_MARK)
	if(DbSvc_GetParentalControlFlag(&pstContext->stSvcInfo) == TRUE)
	{
		bHideFlag = TRUE;
	}
	else
	{
		bHideFlag = FALSE;
	}
#else
	bHideFlag = TRUE;
#endif

	_mgr_LiveLock_base_AVHide(pstContext, eAvMuteFlag_PcLock, bHideFlag);
	_mgr_LiveLock_base_ShowProgram(pstContext);

	return ERR_OK;
}

STATIC HERROR _mgr_LiveLock_base_CheckPinStateChanged(MgrLiveLock_UiState_e eOldState, MgrLiveLock_UiState_e eNewState, HBOOL *pbChanged, HBOOL *pbBlocked)
{
	if((pbChanged == NULL) || (pbBlocked == NULL))
	{
		return ERR_FAIL;
	}

	switch(eOldState)
	{
		case eLockUi_RATING_PIN:
			switch(eNewState)
			{
				case eLockUi_RATING_PIN:
					*pbBlocked = TRUE;
					*pbChanged = FALSE;
					break;
				case eLockUi_RATING_PIN_DENIED:
					*pbBlocked = TRUE;
					*pbChanged = TRUE;
					break;
				case eLockUi_NO_EIT:
					*pbBlocked = TRUE;
					*pbChanged = TRUE;
					break;
				default:
					*pbBlocked = FALSE;
					*pbChanged = TRUE;
					break;
			}
			break;

		case eLockUi_RATING_PIN_DENIED:
			switch(eNewState)
			{
				case eLockUi_RATING_PIN:
					*pbBlocked = TRUE;
					*pbChanged = TRUE;
					break;
				case eLockUi_RATING_PIN_DENIED:
					*pbBlocked = TRUE;
					*pbChanged = FALSE;
					break;
				case eLockUi_NO_EIT:
					*pbBlocked = TRUE;
					*pbChanged = TRUE;
					break;
				default:
					*pbBlocked = FALSE;
					*pbChanged = TRUE;
					break;
			}
			break;

		case eLockUi_NO_EIT:
			switch(eNewState)
			{
				case eLockUi_RATING_PIN:
					*pbBlocked = TRUE;
					*pbChanged = TRUE;
					break;
				case eLockUi_RATING_PIN_DENIED:
					*pbBlocked = TRUE;
					*pbChanged = TRUE;
					break;
				case eLockUi_NO_EIT:
					*pbBlocked = TRUE;
					*pbChanged = FALSE;
					break;
				default:
					*pbBlocked = FALSE;
					*pbChanged = TRUE;
					break;
			}
			break;


		default:
			switch(eNewState)
			{
				case eLockUi_RATING_PIN:
				case eLockUi_RATING_PIN_DENIED:
				case eLockUi_NO_EIT:
				case eLockUi_SC_PIN:
					*pbBlocked = TRUE;
					*pbChanged = TRUE;
					break;
				default:
					*pbBlocked = FALSE;
					*pbChanged = FALSE;
					break;
			}
			break;
	}
	return ERR_OK;
}

STATIC void _mgr_LiveLock_base_SendPinCtrlChanged(xMGR_LiveLock_Context_t *pstContext, HBOOL bBlocked)
{
	Handle_t		hAction;
#if defined(CONFIG_OP_SES)
	HBOOL		nBlockedByCas = FALSE;
#else
	HINT32			systemId = 0;
#endif

	if(pstContext == NULL)
	{
		return;
	}

	hAction = SVC_PIPE_GetActionHandle(pstContext->ulActionId);

#if defined(CONFIG_OP_SES)
		if (pstContext->eUiState == eLockUi_SC_PIN || pstContext->eUiState == eLockUi_SC_PIN_DENIED)
		{
			nBlockedByCas = TRUE;
		}
	BUS_SendMessage(NULL, eMEVT_LIVE_NOTIFY_EVENT_RATING_CTRL_CHANGED, hAction, bBlocked, pstContext->eRatingValue, nBlockedByCas);
#else
	_mgr_LiveLock_base_SendBusMessage(NULL, eMEVT_LIVE_NOTIFY_EVENT_RATING_CTRL_CHANGED, hAction, bBlocked, pstContext->eRatingValue, systemId);
#endif

	return;
}

STATIC void _mgr_LiveLock_base_SetUiState(xMGR_LiveLock_Context_t *pstContext, MgrLiveLock_UiState_e eUiState)
{
	HUINT32					ulActionId = pstContext->ulActionId;
	Handle_t				hAction = SVC_PIPE_GetActionHandle (ulActionId);

	HxLOG_Info ("[AP:Lock(%d)] _mgr_LiveLock_base_SetUiState(%s -> %s)\n", ulActionId,_mgr_LiveLock_base_GetUiStateStr(pstContext->eUiState), _mgr_LiveLock_base_GetUiStateStr(eUiState));
	if(eUiState != pstContext->eUiState)
	{
		HBOOL bChanged = FALSE, bBlocked = FALSE;

		_mgr_LiveLock_base_CheckPinStateChanged(pstContext->eUiState, eUiState, &bChanged, &bBlocked);

		/* UI state 바꾸고... */
		pstContext->eUiState = eUiState;

		/* ui 찌꺼기를 남기지 않기 위해 pin code dialog를 띄울 때는 먼저, destroy시킬 때는 나중에 event를 발생시켜야 한다. */
		if(bChanged == TRUE && bBlocked == TRUE)
		{
			_mgr_LiveLock_base_SendPinCtrlChanged(pstContext, bBlocked);
		}

		/* 새 UI로 그린다. */
		// param에다 uiState를 안 넣어줘도 알아서 가져간다
		HxLOG_Debug("send ui state (%s)  \n", _mgr_LiveLock_base_GetUiStateStr(eUiState));
		_mgr_LiveLock_base_SendBusMessage(NULL, eMEVT_LIVE_NOTIFY_LOCK_STATE_CHANGED, hAction, 0, 0, 0);

		if(bChanged == TRUE && bBlocked == FALSE)
		{
			_mgr_LiveLock_base_SendPinCtrlChanged(pstContext, bBlocked);
		}
	}
	else
	{
		/* 같은 state다. 그냥 있으면 되겠네... */
		HxLOG_Error("[AP:Lock(%d)] \t=> Same UI state.\n",ulActionId);
	}
}

STATIC void _mgr_LiveLock_base_SetAppState (xMGR_LiveLock_Context_t *pstContext, xMGR_LiveLock_AppState_e eAppState)
{
	HUINT32		ulActionId = pstContext->ulActionId;

/* 1. 현재 state에서 벗어나면서 처리할 것들 ... AV stop/hide/show 처리 */
	if (eAppState == pstContext->eAppState)
	{
		/*lock -> lock 이더라도 appState set이 필요함. lock&nosignal -> lock&signal의 경우를 위해.*/
		if((pstContext->eAppState != eLiveLockState_LOCK_PIN) || (pstContext->bNoSignal != TRUE))
		{
			HxLOG_Print("[AP:Lock(%d)] \t=> Same WTV state(%s). Do nothing.\n",ulActionId, _mgr_LiveLock_base_GetAppStateStr(pstContext->eAppState));
			return;
		}
	}
	/*svc lock상태에서 pin입력이 아닌 svclock자체를 해제할 때 av 나오지 않는 문제 수정*/
	else if((pstContext->eAppState == eLiveLockState_LOCK_PIN) && (eAppState == eLiveLockState_AV))
	{
		_mgr_LiveLock_base_SendBusMessage(NULL, eMEVT_LIVE_NOTIFY_SVC_UNLOCKED, SVC_PIPE_GetActionHandle (pstContext->ulActionId), 0, 0, 0);
	}

/* 2. 새 state 등록. context는 변경해 놔야 아래 루틴들이 작업시 반영함. */
	HxLOG_Print("[AP:Lock(%d)] \t=> State transition <%s> => <%s>\n",ulActionId,
									_mgr_LiveLock_base_GetAppStateStr(pstContext->eAppState),
									_mgr_LiveLock_base_GetAppStateStr(eAppState));

	pstContext->eAppState = eAppState;

/* 3. 새로운 state로 진입하면서 처리할 것들 ... */
	switch(eAppState)
	{
		case eLiveLockState_LOCK_PIN :
			HxLOG_Debug("eLiveLockState_LOCK_PIN \n");

			_mgr_LiveLock_base_AVHide(pstContext, eAvMuteFlag_UserLock,  TRUE );
		#if defined(CONFIG_MW_CI_PLUS_V_1_3)
			// TODO: CiPlus Content여서 CAM에 의한 PC를 수행하여 A/V를 막을 때 현재는 A/V만 막고, status 관리하는 쪽을 타지 않고 있음
			// TODO: 인증은 통과 할 수 있도록 해 놓았지만, system message 등을 출력하기 위해 기존 시나리오에 녹아 들어야 함.
			// TODO: 담당자에게 문의하여 xmgr_live_lock_ciplus.c 등으로 빠져야 하는지... 혹은 시나리오 등을 문의하고 작업 필요함.
			if(pstContext->bIsCiPlusContent == TRUE)
			{
				HxLOG_Info("Content is CI+ PC content... Asking PIN code by Prism UI is not required...\n");
			}
			else
			{
				_mgr_LiveLock_base_SetUiState (pstContext, eLockUi_LOCK_PIN);

				/*no signal->signal 로 전환시 view proc에서 mw service들이 start된 후 svc locked를 받아 필요한 svc만 stop해야 한다.
				send로 보내면 ch locked event를 받기전에 state가 svc locked가 되어버러 mw svc들을 start하지 못한다.*/
				if(pstContext->bNoSignal == TRUE)
				{
					HxLOG_Print("\n\n post eMEVT_LIVE_NOTIFY_SVC_LOCKED \n");
					_mgr_LiveLock_base_PostBusMessage(NULL, eMEVT_LIVE_NOTIFY_SVC_LOCKED, SVC_PIPE_GetActionHandle (pstContext->ulActionId), 0, 0, 0);
				}
				else
				{
					HxLOG_Print("\n\n send eMEVT_LIVE_NOTIFY_SVC_LOCKED \n");
					_mgr_LiveLock_base_SendBusMessage(NULL, eMEVT_LIVE_NOTIFY_SVC_LOCKED, SVC_PIPE_GetActionHandle (pstContext->ulActionId), 0, 0, 0);
				}
			}
		#else
			_mgr_LiveLock_base_SetUiState (pstContext, eLockUi_LOCK_PIN);

			/*no signal->signal 로 전환시 view proc에서 mw service들이 start된 후 svc locked를 받아 필요한 svc만 stop해야 한다.
			send로 보내면 ch locked event를 받기전에 state가 svc locked가 되어버러 mw svc들을 start하지 못한다.*/
			if(pstContext->bNoSignal == TRUE)
			{
				HxLOG_Print("\n\n post MSG_LIVELOCK_SVC_LOCKED \n");
				_mgr_LiveLock_base_PostBusMessage(NULL, eMEVT_LIVE_NOTIFY_SVC_LOCKED, SVC_PIPE_GetActionHandle (pstContext->ulActionId), 0, 0, 0);
			}
			else
			{
				HxLOG_Print("\n\n send MSG_LIVELOCK_SVC_LOCKED \n");
				_mgr_LiveLock_base_SendBusMessage(NULL, eMEVT_LIVE_NOTIFY_SVC_LOCKED, SVC_PIPE_GetActionHandle (pstContext->ulActionId), 0, 0, 0);
			}
		#endif
			break;

/*------------------- PIN NOT Running States Above ------------------*/

/*------------------- PIN Running States Below ----------------------*/
		case eLiveLockState_WAIT_RATING :
			HxLOG_Debug("eLiveLockState_WAIT_RATING \n");
			_mgr_LiveLock_base_SetUiState(pstContext, eLockUi_NONE);
			break;

		case eLiveLockState_RATING_PIN :
			HxLOG_Debug("eLiveLockState_RATING_PIN \n");
			/* 1. AV hide - (rating pin state는 av stop하지 않고 hide/show로 처리) */
			_mgr_LiveLock_base_AVHide(pstContext, eAvMuteFlag_PcLock,  TRUE );
			{

#if defined(CONFIG_MW_CAS_IRDETO) && (CONFIG_MW_CAS_NUM_EMBEDDED <= 1)
				DxRatingAge_e	eRating;
				HERROR			hErr;

				//eRating = MWC_PARAM_GetMaturityRating();
				hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_MATURITYRATING, (HUINT32 *)&eRating, 0);
				if (ERR_OK != hErr)
				{
					HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_MATURITYRATING) Error!!!\n");
				}

				HxLOG_Info("reg eRating: %d, pstContext->bEitValid: %d \n", eRating, pstContext->bEitValid);

				if(pstContext->bEitValid == FALSE && (eRating != eDxRATING_VIEW_ALL))
				{
					_mgr_LiveLock_base_SetUiState(pstContext, eLockUi_NO_EIT);
				}
				else
#endif
				{
					_mgr_LiveLock_base_SetUiState(pstContext, eLockUi_RATING_PIN);
				}
			}
			break;

		case eLiveLockState_SC_PIN :
			HxLOG_Debug("eLiveLockState_SC_PIN \n");
			_mgr_LiveLock_base_AVHide(pstContext, (eAvMuteFlag_PcLock),  TRUE);
			_mgr_LiveLock_base_SetUiState(pstContext, eLockUi_SC_PIN);
			break;

		case eLiveLockState_AV :
			HxLOG_Debug("eLiveLockState_AV \n");
			_mgr_LiveLock_base_AVHide(pstContext, (eAvMuteFlag_UserLock | eAvMuteFlag_PcLock),  FALSE);
			_mgr_LiveLock_base_SetUiState(pstContext, eLockUi_OK);

			/* Application들에게 AV 시작을 알림. 이 action이 main action이라면 teletext, data방송등이 시작될 것임. */
			_mgr_LiveLock_base_SendBusMessage(NULL, eMEVT_OUTPUT_AV_STARTED, SVC_PIPE_GetActionHandle (pstContext->ulActionId), pstContext->hSvc, 0, 0);
			break;

		default :
			/* 위 case 말고 다른 state 나 WTV state가 아닌 값은 이 API를 쓰면 안된다. */
			HxLOG_Critical("\n\n");
			break;
	}

	return;
}

STATIC void _mgr_LiveLock_base_CheckServiceLock(xMGR_LiveLock_Context_t *pstContext)
{
	HUINT32				ulActionId = pstContext->ulActionId;
	DbSvc_Info_t*			pstMasterSvcInfo = NULL;

	HxLOG_Info("[AP:Lock(%d)] _mgr_LiveLock_base_CheckServiceLock()\n",ulActionId);
	HxLOG_Error("[AP:Lock(%d)] \tbLocked:%d, bLockPinPassed:%d\n",ulActionId, DbSvc_GetLocked(&pstContext->stSvcInfo), pstContext->bLockPinPassed);

	pstMasterSvcInfo = &(pstContext->stSvcInfo);

	if( (DbSvc_GetLocked(pstMasterSvcInfo) == TRUE) && (pstContext->bLockPinPassed != TRUE) )
	{
		_mgr_LiveLock_base_SetAppState(pstContext, eLiveLockState_LOCK_PIN);
	}
	else if(pstContext->eAppState < eLiveLockState_PID_START)
	{
		/* PID start 전이면 PID start 로 진행 */
		_mgr_LiveLock_base_StartPid(pstContext);
	}
	else
	{
		/* PID start 이후이고 lock 반영안되면 아무 동작하지 않는다. */
	}

	return;
}

STATIC void _mgr_LiveLock_base_SetPvrPinPassedEventId(xMGR_LiveLock_Context_t *pstContext)
{
#if defined(CONFIG_MW_MM_PVR)
	HUINT32			ulActionId = pstContext->ulActionId;
	HUINT8			ucPRate;
	HBOOL			bRatingPassed;

	MGR_LIVE_PincodeIsEventIdRatingPassed(pstContext->hSvc, pstContext->ulEventId, &ucPRate, &bRatingPassed);
	if(bRatingPassed == FALSE)
	{
		HxLOG_Error("[AP:Lock(%d)] Register event ID to PVR PIN code history.\n",ulActionId);
		MGR_LIVE_PincodeSetRatingPassedEventId(pstContext->hSvc, pstContext->ulEventId, (HUINT8)pstContext->eRatingValue);
	}
#endif
}

void _mgr_LiveLock_base_RegisterRatingPassedEvent(xMGR_LiveLock_Context_t *pstContext)
{
	HxLOG_Error("Saving Event ID [0x%x] to service info\n", pstContext->ulEventId);
	DbSvc_SetRatingPassedEvent(&pstContext->stSvcInfo, pstContext->ulEventId);
	DB_SVC_UpdateServiceInfoEx(pstContext->hSvc, eSvcField_06_ulRatingPassedEventId, eSvcField_Ext_All, &(pstContext->stSvcInfo));
}

STATIC HBOOL _mgr_LiveLock_base_IsRatingOk(xMGR_LiveLock_Context_t *pstContext)
{
	HUINT32			ulActionId = pstContext->ulActionId;
	HUINT32			ulParentalSetup = MGR_PG_GetRatingFromSetup();

	if(MGR_PG_IsRatingOk(ulActionId, pstContext->hSvc, DbSvc_GetDeliType(pstContext->stSvcInfo), ulParentalSetup, pstContext->eRatingValue) == TRUE)
	{
		/* OK to watch */
		return TRUE;
	}
	else
	{
#if defined(CONFIG_APCORE_EVENT_BASED_RATING)
		if( (DbSvc_GetRatingPassedEvent(&pstContext->stSvcInfo) != (HUINT32)-1) && /* PIN code pass된 event ID가 등록되어 있고... */
			(pstContext->ulEventId == DbSvc_GetRatingPassedEvent(&pstContext->stSvcInfo)) /* 현재의 event ID가 여전히 등록된 event ID를 유지하고 있는 경우. */
		)
		{ /* OK to watch */
			HxLOG_Error("[AP:Lock(%d)] OK to watch for the same event ID as the previous PIN passed one.\n",ulActionId);
			return TRUE;
		}
		else
#endif
		{
#if defined(CONFIG_MW_MM_PVR)
			/* 주의 : 이 위치에서 확인해야 함. 미리 passed flag들 set 하고 들어오면 안됨. */

			HUINT8	ucPRate;
			HBOOL	bRatingPassed;

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
			if (pstContext->bSCPinRemoved == FALSE)
			{
				return FALSE;
			}
#endif
			// pvr(tsr playback, chase playback) 에서 해당 event 를 해제한 적인 있는지 확인한다.
			MGR_LIVE_PincodeIsEventIdRatingPassed(pstContext->hSvc, pstContext->ulEventId, &ucPRate, &bRatingPassed);
			if(bRatingPassed == TRUE)
			{
				HxLOG_Error("[AP:Lock(%d)] OK to watch by PVR PIN code history.\n",ulActionId);
				pstContext->bRatingPinPassed = TRUE;
				pstContext->bSCPinPassed = TRUE;

				/* Passed flag들 없이 여기 처음 들어와서 PVR history에 의해 풀린 것이니 service info에도 기록해야 함. */
				_mgr_LiveLock_base_RegisterRatingPassedEvent(pstContext);

				return TRUE;
			}
#endif
			return FALSE;
		}
	}
}

#if defined(CONFIG_OP_SES) && defined(CONFIG_MW_CAS_NAGRA)
STATIC HBOOL _mgr_LiveLock_base_IsHDplusService(HUINT32 ulActionId)
{
	HERROR			hErr = ERR_OK;
	HUINT8			bSame = FALSE;

	HUINT32 		ulPMTCaIdCnt = 0;
	HUINT16 		ausPMTCaId[64];
	HUINT32 		ulCnt = 0;

	VK_memset(ausPMTCaId, 0, sizeof(ausPMTCaId));
	hErr = SVC_CAS_GetCaSystemIdByActionId(ulActionId, &ulPMTCaIdCnt, ausPMTCaId);
	if(hErr != ERR_OK)
	{
		HxLOG_Debug("_mgr_LiveLock_base_IsHDplusService() - not found CA System ID from PMT \n");
		return FALSE;
	}

	for(ulCnt = 0; ulCnt < ulPMTCaIdCnt ; ulCnt++)
	{
		hErr = MGR_CAS_IsSameCasIdInSc(0, ausPMTCaId[ulCnt], &bSame);
		if(hErr == ERR_OK && bSame == TRUE)
		{
			HxLOG_Debug("HD PLUS SERVICE !!! \n");
			return TRUE;
		}
	}
	HxLOG_Debug("NOT HD PLUS SERVICE !!! \n");
	return FALSE;
}
#endif

STATIC void _mgr_LiveLock_base_ShowProgram(xMGR_LiveLock_Context_t *pstContext)
{
	HUINT32				ulActionId = pstContext->ulActionId;
	HUINT32				ulParentalSetup = 0;
	xMGR_LiveLock_AppState_e	eAppState;
#if defined(CONFIG_APCORE_PC_SVC_MARK)
	HBOOL				bPCService = DbSvc_GetParentalControlFlag(&pstContext->stSvcInfo);
#endif

	HxLOG_Info("[AP:Lock(%d)] _mgr_LiveLock_base_ShowProgram()\n",ulActionId);

	if( (pstContext->bSCPinCodeValid == TRUE) && (pstContext->bSCPinPassed == TRUE) )
	{ /* Smart card rating PIN 통과 되어 있음. OK to watch */
		eAppState = eLiveLockState_AV;
		_mgr_LiveLock_base_SetPvrPinPassedEventId(pstContext);
	}
	else if (pstContext->bRatingPinPassed == TRUE)
	{
		/* APS HD+ 가 아닌 서비스에서 STB rating PIN 통과 되어 있음. OK to watch */
		eAppState = eLiveLockState_AV;
		_mgr_LiveLock_base_SetPvrPinPassedEventId(pstContext);
	}
	else
	{
		/* rating PIN 통과되지 않았거나, APS HD+에서는 SC PIN 통과되지 않음. Rating check해야 함. */
		MgrPg_Info_t stParentalRatingContext;

		stParentalRatingContext.bCasOk = pstContext->bCasOk;
		stParentalRatingContext.eCasType = DbSvc_GetCasType(&(pstContext->stSvcInfo));
		stParentalRatingContext.bRatingValid = pstContext->bRatingValid;
		ulParentalSetup = MGR_PG_GetRatingFromSetup();
		if( (ulParentalSetup == eDxRATING_VIEW_ALL)			/* Setup이 view all 이거나... */
#if defined(CONFIG_APCORE_PC_SVC_MARK)
			|| (bPCService == FALSE)						/* Rating 이 걸린적이 없는 서비스이면 */
#endif
		)
		{ /* No need to check parental control. OK to watch */
			eAppState = eLiveLockState_AV;
		}
#if 0
		else if( (pstContext->bCasOk == TRUE) && (pstContext->nCasSystemId == eDxCAS_GROUPID_CAM) )
		{ /* CI로 descramble 하는 경우는 rating check 안한다.
			CI는 MMI를 쓰므로 무조건 OK를 준다.
			MMI로 rating을 처리할 것이다. Rating 처리하지 않는 CAM도 있으나 어쩔 수 없다. */
			eAppState = eLiveLockState_AV;
		}
#endif
		else if (MGR_PG_IsWaitRating(&stParentalRatingContext) == TRUE)
		{
			eAppState = eLiveLockState_WAIT_RATING;
		}
		else if(_mgr_LiveLock_base_IsRatingOk(pstContext) == TRUE)
		{ /* OK to watch */
			eAppState = eLiveLockState_AV;
		}
		else
		{ /* Should create PIN code */
			HxLOG_Error("[AP:Lock(%d)] First event ID or NOT registared event ID. Block AV.\n",ulActionId);

#if defined(CONFIG_OP_SES) && defined(CONFIG_MW_CAS_NAGRA)
			if(_mgr_LiveLock_base_IsHDplusService(ulActionId) == TRUE)
#else
			if(DbSvc_GetOpType(&(pstContext->stSvcInfo)) == eDxOPERATOR_Ses)
#endif
			{ /* APS HD+ 채널에서... */
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
				HxLOG_Print("pstContext->bSCPinCodeValid(%d)\n", pstContext->bSCPinCodeValid);

				if(pstContext->bSCPinCodeValid != TRUE)
				{
					pstContext->bSCPinCodeValid = SVC_CAS_CheckSmartCardValidity(eDxCAS_GROUPID_NA);
					HxLOG_Print("pstContext->bSCPinCodeValid(%d)\n", pstContext->bSCPinCodeValid);
				}
#endif
				if(pstContext->bSCPinCodeValid == TRUE)
				{ /* SC-Pincode가 valid하면 모든 채널에 적용 */
					eAppState = eLiveLockState_SC_PIN;		// eWtvState_SC_PIN 와 유사한 게 필요한건가....
				}
				else
				{ /* SC-Pincode가 valid하지 않으면 APS HD+ 채널은 CAS FAIL로 처리하기 위하여 그대로 유지. */
					eAppState = pstContext->eAppState;
				}
			}
			else
			{
				/* STB-Pincode로 변경 */
				eAppState = eLiveLockState_RATING_PIN;
			}
		}
	}

	HxLOG_Print("eAppState(%d)\n", eAppState);

#if 0
	HxLOG_Error ("[AP:Lock(%d)] Rating Evaluation --------------------------\n \
\t\t\t eSvcOpType[%d]\n \
\t\t\t bRatingPinPassed[%d]\n  \
\t\t\t ulParentalSetup[%d]\n \
\t\t\t eCasType[%d]\n \
\t\t\t bCasOk[%d]\n \
\t\t\t bRatingValid[%d]\n \
\t\t\t Rating value[0x%x]\n",		ulActionId,
									pstContext->stSvcInfo.eSvcOpType,
									pstContext->bRatingPinPassed,
									ulParentalSetup,
									pstContext->stSvcInfo.eCasType,
									pstContext->bCasOk,
									pstContext->bRatingValid,
									pstContext->eRatingValue
									);
#endif

	/* 새로 결정된 state로 이동 */
	_mgr_LiveLock_base_SetAppState(pstContext, eAppState);
}

STATIC void _mgr_LiveLock_base_RegisterParentalControlService(xMGR_LiveLock_Context_t *pstContext)
{
#if defined(CONFIG_APCORE_PC_SVC_MARK)
	if (DbSvc_GetParentalControlFlag(&pstContext->stSvcInfo) == FALSE)
	{ /* 현재 PC mark가 FALSE 인데, PC가 발견됨. 현재 정보와 DB도 update해야 함. */

		if( (pstContext->eRatingValue != eDxRATING_VIEW_ALL))
		{ /* Rating 발견되어도 view all 넘들은 그냥 넘어간다. */
			HxLOG_Print("[AP:ZAP(%d)] \t=> Marking this service as PARENTAL CONTROL service.\n",pstContext->ulActionId);
			DbSvc_SetParentalControlFlag(&pstContext->stSvcInfo, TRUE);
			DB_SVC_UpdateServiceInfoEx(pstContext->hSvc, eSvcField_05_eUserFlag1, eSvcField_Ext_All, &(pstContext->stSvcInfo));
		}
	}
#endif

	return;
}

#if defined(CONFIG_MW_CI_PLUS_V_1_3)
// Return 이 ERR_FAIL인 경우 해당 content는 CI+CAM을 타지 않는다는 의미
STATIC HERROR _mgr_LiveLock_base_GetCiPlusSlotIdOfContent(xMGR_LiveLock_Context_t *pstContext, HUINT16 *pusSlotId)
{
	SvcCas_CtrlParam_t					stParam;
	CI_GetSlotIdByActionIdInParam_t		stSlotIdInParam;
	CI_GetSlotIdByActionIdOutParam_t	stSlotIdOutParam;
	HERROR								hErr;

	stSlotIdInParam.ulActionId = pstContext->ulActionId;
	stParam.pvIn = (void *)&stSlotIdInParam;
	stParam.pvOut = (void *)&stSlotIdOutParam;
	stParam.ulControlType = eCACTRL_CI_GetSlotIdByActionId;

	hErr = SVC_CAS_CtrlGet(eDxCAS_GROUPID_CAM, eCACTRL_CI_GetSlotIdByActionId, &stParam);
	if (hErr == ERR_FAIL)
	{
		HxLOG_Info("It is not the service which is using CAM...\n");
		// 혹시라도 현재 삽입되어 있는 CAM중 하나라도 pin capability가 2이거나 4 일 때 여기서 처리 되어야 하는지 파악하고 처리해야 한다면 코드 추가.
		return ERR_FAIL;
	}

	*pusSlotId = stSlotIdOutParam.usSlotId;

	return ERR_OK;
}

STATIC HERROR _mgr_LiveLock_base_GetCiPlusPinCapaInfo(xMGR_LiveLock_Context_t *pstContext, HUINT16 usSlotId, CI_PIN_CAPA_INFO *pstPinCapaInfo)
{
	SvcCas_CtrlParam_t					stParam;
	CI_GetPinCapabilityInParam_t		stPinCapaInParam;
	CI_GetPinCapabilityOutParam_t		stPinCapaOutParam;
	HERROR								hErr;

	stPinCapaInParam.usSlotId = usSlotId;
	stParam.pvIn = (void *)&stPinCapaInParam;
	stParam.pvOut = (void *)&stPinCapaOutParam;
	stParam.ulControlType = eCACTRL_CI_GetPinCapa;

	HxLOG_Info("Using CAM Slot ID: [%02x]\n", usSlotId);

	hErr = SVC_CAS_CtrlGet(eDxCAS_GROUPID_CAM, eCACTRL_CI_GetPinCapa, &stParam);
	if (hErr == ERR_FAIL)
	{
		HxLOG_Error("ERROR: SVC_CAS_CtrlGet => eCACTRL_CI_GetPinCapa...\n");
		return ERR_FAIL;
	}

	HxSTD_memcpy(pstPinCapaInfo, &stPinCapaOutParam.stPinCapaInfo, sizeof(CI_PIN_CAPA_INFO));

	return ERR_OK;
}

STATIC HBOOL _mgr_LiveLock_base_IsCiPlusPcContent(xMGR_LiveLock_Context_t *pstContext)
{
	HUINT16		usSlotId;
	HERROR		hErr;
	CI_PIN_CAPA_INFO stPinCapaInfo;

	hErr = _mgr_LiveLock_base_GetCiPlusSlotIdOfContent(pstContext, &usSlotId);
	if (hErr == ERR_FAIL)
	{
		return FALSE;
	}

	hErr = _mgr_LiveLock_base_GetCiPlusPinCapaInfo(pstContext, usSlotId, &stPinCapaInfo);
	if (hErr == ERR_FAIL)
	{
		return FALSE;
	}

	if (stPinCapaInfo.enPinCapa > CI_PIN_CAPABILITY_NONE)
	{
		// PIN code 관리를 CAM이 하기 때문에 Box의 EIT에 따른 Parental Control 시나리오가 수행 되면 안됨.
		return TRUE;
	}

	return FALSE;
}

// CI+에 의하여 PC가 관리 되면 STB의 PC 시나리오는 타지 않는다 & 타면 안된다. (CI spec에 의해...)
STATIC void _mgr_LiveLock_base_CheckCasPcLock(xMGR_LiveLock_Context_t *pstContext, DxCopyrightControl_CiPlusParentalCtrlInfo_t *pstCiPlusPcInfo)
{
	HBOOL bIsCiPlusPcContent = FALSE;

	bIsCiPlusPcContent = _mgr_LiveLock_base_IsCiPlusPcContent(pstContext);

	if(bIsCiPlusPcContent == TRUE)
	{
		CI_PIN_CAPA_INFO 	stPinCapaInfo;
		HUINT16				usSlotId;
		HERROR				hErr;

		hErr = _mgr_LiveLock_base_GetCiPlusSlotIdOfContent(pstContext, &usSlotId);
		hErr = _mgr_LiveLock_base_GetCiPlusPinCapaInfo(pstContext, usSlotId, &stPinCapaInfo);

		pstContext->bIsCiPlusContent = TRUE;

		if(stPinCapaInfo.ucRating < pstCiPlusPcInfo->ucAgeLimit)
		{
			// CAM에 setting된 rating 보다 현재 content의 rating이 더 높다면 AV를 막아야 함.
			_mgr_LiveLock_base_SetAppState(pstContext, eLiveLockState_LOCK_PIN);
		}
		else
		{
			_mgr_LiveLock_base_SetAppState(pstContext, eLiveLockState_AV);
		}

		pstContext->bIsCiPlusContent = FALSE;
	}

	return;
}

#endif


#define _____CONTEXT_FUNC_____
xMGR_LiveLock_Context_t *XMGR_LiveLock_BASE_GetContext (HUINT32 ulActionId)
{
	return _mgr_LiveLock_base_getContext (ulActionId);
}

HERROR XMGR_LiveLock_BASE_InitContext (void)
{
	return _mgr_LiveLock_base_initContext();
}

HERROR XMGR_LiveLock_BASE_ResetUiState(HUINT32 ulActionId, HBOOL bSvcStart)
{
	return _mgr_LiveLock_base_resetUiState(ulActionId, bSvcStart);
}

#define _____PROTOTYPE_METHODS_____
HERROR XMGR_LiveLock_BASE_StartSession (xMGR_LiveLock_Context_t *pstContext, Handle_t hAction, MgrLive_START_t *pstStart)
{
	HUINT32			 ulActionId = SVC_PIPE_GetActionId (hAction);
	HERROR			 hErr = ERR_FAIL;

	if (pstContext->ulActionId != ulActionId)
	{
		HxLOG_Error ("Action Handle not available\n");
		return ERR_FAIL;
	}

	_mgr_LiveLock_base_SetAppState(pstContext, eLiveLockState_IDLE);
	_mgr_LiveLock_base_SetUiState(pstContext, eLockUi_NONE);
	_mgr_LiveLock_base_ResetCommonContextFlags (pstContext, pstStart->hService, eNO_IPLATE);

	pstContext->hSvc = pstStart->hService;
	pstContext->hMasterSvc = pstStart->hMasterSvc;
	pstContext->eLiveMode = pstStart->eLiveMode;
	pstContext->bEitValid = FALSE;
	pstContext->bRatingValid = FALSE;
	pstContext->bRatingPassByLockPass = FALSE;
	pstContext->bCasOk = FALSE;
	pstContext->bNoSignal = FALSE;
#if defined(CONFIG_MW_SI_PMT_RATING)
	pstContext->bPmtValid = FALSE;
	pstContext->bPmtRatingValid = FALSE;
#endif
	HxSTD_memcpy (&(pstContext->stStartArg), pstStart, sizeof(MgrLive_START_t));

	hErr = DB_SVC_GetServiceInfo(pstContext->hSvc, &(pstContext->stSvcInfo));
	if (hErr != ERR_OK)
	{
		HxLOG_Error("DB_SVC_GetServiceInfo ret :0x%X\n",hErr);
	}

	hErr = MGR_RSC_GetResourceInfo(pstContext->ulActionId, &pstContext->stRscInfo);
	if (hErr)
	{
		HxLOG_Error("MGR_RSC_GetResourceInfo ret :0x%X\n",hErr);
	}

#if defined(CONFIG_SUPPORT_FCC)
	{
		//user lock이나 Rating이 ViewALL이 아닐경우에만  Video Hide
		HUINT32				ulParentalSetup = 0;
		ulParentalSetup = MGR_PG_GetRatingFromSetup();

		if (DbSvc_GetLocked(&pstContext->stSvcInfo) == TRUE || (ulParentalSetup != eDxRATING_VIEW_ALL))
		{
			// Cas 없는 모델에서는 Svc_Av.c 에서 AV Start 되기 전에 Lock 풀려서 채널 전환시에 이전 채널이 잠시 보이는 문제가 있어서 수정함.
			_mgr_LiveLock_base_AVHide(pstContext, (eAvMuteFlag_UserLock | eAvMuteFlag_PcLock|eAvMuteFlag_View),  TRUE );
		}
	}
#else
		// Cas 없는 모델에서는 Svc_Av.c 에서 AV Start 되기 전에 Lock 풀려서 채널 전환시에 이전 채널이 잠시 보이는 문제가 있어서 수정함.
		_mgr_LiveLock_base_AVHide(pstContext, (eAvMuteFlag_UserLock | eAvMuteFlag_PcLock |eAvMuteFlag_View),	TRUE );
#endif

	_mgr_LiveLock_base_ShowProgram(pstContext);

#if defined(CONFIG_SUPPORT_FCC)
	// user lock이 설정되어 있지 않다면 여기서 fcc flag로 show해준다.
	// user lock true라면 tune state msg를 받은 다음 처리될 것임..
	if (DbSvc_GetLocked(&pstContext->stSvcInfo) == FALSE)
	{
		hErr = SVC_AV_SetAudioHide(ulActionId, eAvMuteFlag_Fcc,  FALSE);
		if (hErr)
		{
			HxLOG_Error("SVC_AV_SetAudioHide ret :0x%X\n", hErr);
		}

		hErr = SVC_AV_SetVideoHide(ulActionId, eAvMuteFlag_Fcc, FALSE);
		if (hErr)
		{
			HxLOG_Error("SVC_AV_SetVideoHide ret :0x%X\n", hErr);
		}
	}
#endif

	return ERR_OK;
}

HERROR XMGR_LiveLock_BASE_StopSession (xMGR_LiveLock_Context_t *pstContext)
{
	return ERR_OK;
}


#define _____PROTOTYPE_MSG_METHODS_____
BUS_Result_t XMGR_LiveLock_BASE_MsgBusCreate (xMGR_LiveLock_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	MgrLive_START_t			*pstStart = (MgrLive_START_t *)p1;

	if (NULL == pstContext)
	{
		HxLOG_Error ("Context NULL!!!\n");
		return MESSAGE_PASS;
	}

	pstContext->ulActionId = SVC_PIPE_GetActionId (hAction);
	pstContext->hSvc = pstStart->hService;
	pstContext->eLiveMode = pstStart->eLiveMode;
	pstContext->eUiState = eLockUi_NONE;

#if defined(CONFIG_MW_CAS_NAGRA)
	pstContext->bSCPinCodeValid = SVC_CAS_CheckSmartCardValidity(eDxCAS_GROUPID_NA);
#else
	pstContext->bSCPinCodeValid = FALSE;
#endif

	HxSTD_memcpy (&(pstContext->stStartArg), pstStart, sizeof(MgrLive_START_t));

	return MESSAGE_PASS;
}

BUS_Result_t XMGR_LiveLock_BASE_MsgBusDestroy (xMGR_LiveLock_Context_t *pstContext, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (NULL == pstContext)
	{
		HxLOG_Error ("Context NULL!!!\n");
		return MESSAGE_PASS;
	}

	return MESSAGE_PASS;
}

BUS_Result_t XMGR_LiveLock_BASE_MsgLiveStartSvc (xMGR_LiveLock_Context_t *pstContext, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	MgrLive_START_t	*pstStart = (MgrLive_START_t *)p1;
	HERROR				 hErr = ERR_FAIL;

	hErr = XMGR_LiveLock_BASE_StartSession (pstContext, hHandle, pstStart);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("XMGR_LiveLock_BASE_StartSession hErr:\n");
	}

	return MESSAGE_PASS;
}

BUS_Result_t XMGR_LiveLock_BASE_MsgLiveStopSvc (xMGR_LiveLock_Context_t *pstContext, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32			 ulActionId = SVC_PIPE_GetActionId (hHandle);

	if (pstContext->ulActionId != ulActionId)
	{
		HxLOG_Error ("Action Handle not available\n");
		return MESSAGE_PASS;
	}

	// Stop Service
	_mgr_LiveLock_base_AVHide(pstContext,(eAvMuteFlag_UserLock | eAvMuteFlag_PcLock),  FALSE);

	return MESSAGE_PASS;
}

BUS_Result_t XMGR_LiveLock_BASE_EvtSiEit (xMGR_LiveLock_Context_t *pstContext, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR			hErr = ERR_FAIL;
	HUINT32			ulActionId = SVC_PIPE_GetActionId (hHandle);
	SvcSi_Event_t	stEventInfo;
	HUINT32			ulEventId = 0;
	HUINT32			ulRatingValue;
	DbSvc_Info_t			stSvcInfo;
	DbSvc_TripleId_t 		stTripleId;

	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/* 1. Event ID를 알아본다. */
	HxSTD_memset(&stSvcInfo, 0x00, sizeof(DbSvc_Info_t));
	HxSTD_memset(&stEventInfo, 0x00, sizeof(SvcSi_Event_t));
	hErr = DB_SVC_GetServiceInfo (pstContext->hSvc, &stSvcInfo);
	if (hErr == ERR_OK)
	{
		stTripleId.usOnId = DbSvc_GetOnId(&stSvcInfo);
		stTripleId.usTsId = DbSvc_GetTsId(&stSvcInfo);
		stTripleId.usSvcId = DbSvc_GetSvcId(&stSvcInfo);

		hErr = SVC_SI_GetEvtInfoFromPfEvent(ulActionId, stTripleId, TRUE, TRUE, &stEventInfo, NULL);
	}

	if (hErr == ERR_OK)
	{	/* Event ID를 등록한다. */
		ulEventId = (HUINT32)stEventInfo.nEvtId;
		pstContext->bEitValid = TRUE;
	}
	else
	{
		/* Event ID가 없으면 -1로 초기화. */
		HxLOG_Print("ActionId :(%d)  No event ID in EIT\n", ulActionId);
		ulEventId = (HUINT32)-1;
		pstContext->bEitValid = FALSE;
	}

	/* event ID가 달라졌으면 rating PIN passed flag reset. */
	if(ulEventId != pstContext->ulEventId)
	{
		HxLOG_Print("ActionId:(%d) Event ID changed from [0x%x] to [0x%x].\n", ulActionId, pstContext->ulEventId, ulEventId);

#if defined(CONFIG_APCORE_EVENT_BASED_RATING)
		pstContext->bRatingPinPassed = FALSE;
		pstContext->bSCPinPassed = FALSE; /* APS 사양 AS5101 8.5.1.4 */
#endif
		pstContext->ulEventId = ulEventId; /* Event ID 갱신 */
	}

#if defined(CONFIG_MW_SI_PMT_RATING)
	if(pstContext->bPmtRatingValid == TRUE)
	{
		/* PMT rating 사양에서는 EIT의 rating이 PMT에 우선 순위가 밀린다. PMT rating이 valid하면 EIT 처리하지 않음. */
		HxLOG_Error("[AP:Lock(%d)] \t=> There is PMT rating value. Skip EIT rating check\n",ulActionId);
	}
	else
#endif
	{
		if ((pstContext->bEitValid == TRUE) && (stEventInfo.ucParentRating > 0))
		{
			/* Valid 한 rating value를 받았으니 context에 update함 ...  */
			HxLOG_Print("[AP:Lock(%d)] \t=> Found EIT rating value (%d).\n",ulActionId,stEventInfo.ucParentRating);
			pstContext->bRatingValid = TRUE;
			pstContext->eRatingValue = stEventInfo.ucParentRating;
			_mgr_LiveLock_base_RegisterParentalControlService(pstContext);
		}
		else
		{
			/* Rating 정보 없음. 그냥 무제한으로 간주. */
			HxLOG_Print("[AP:Lock(%d)] \t=> No rating value. \n",ulActionId);
			ulRatingValue = MGR_PG_GetRatingForNoEit(DbSvc_GetSvcType(&pstContext->stSvcInfo), pstContext->bEitValid);

			pstContext->bRatingValid = TRUE;
			pstContext->eRatingValue = ulRatingValue;
		}
	}

#if 0//defined(CONFIG_MW_CAS_IRDETO) // TODO: shko
		{

			HUINT8			aucNationality[4];

			//	Rating이 설정되면 해당 EIT Nationality도 같이 보관..
			if (MW_SI_GetCountryCodeFromPfEvent(pstContext->ulActionId, pstContext->hSvc, TRUE, aucNationality) == ERR_OK)
			{
				HxSTD_StrNCpy(pstContext->aucEitNationality, aucNationality, 4);
			}
			else
			{
				HxSTD_StrNCpy(pstContext->aucEitNationality, "", 4);
			}

			AP_CAS_IR_SetCountryCodeFromEIT(pstContext->aucEitNationality);
		}
#endif

	/* 3. Lock pass에 의한 rating pass 처리 */
	if(pstContext->bRatingPassByLockPass == TRUE)
	{
		/* Lock pass에 의하여 rating도 pass하라는 flag가 있음. */
		HxLOG_Error("[AP:Lock(%d)] \t=> Rating PIN is set passed by lock PIN pass.\n",ulActionId);
		pstContext->bRatingPinPassed = TRUE;
		pstContext->bRatingPassByLockPass = FALSE; /* 한번 쓴것은 꼭 버려야 한다. 안 그러면 매번 통과한다. */
	}

#if defined(CONFIG_MW_CI_PLUS_V_1_3)
#if defined(CONFIG_OP_SES)
	{
		HUINT16		usSlotId = -1;

		hErr = _mgr_LiveLock_base_GetCiPlusSlotIdOfContent(pstContext, &usSlotId);
		if (hErr == ERR_OK)
		{
			pstContext->bRatingPinPassed = TRUE;
		}
	}
#else
	{
		HBOOL bIsCiPlusPcContent = FALSE;

		bIsCiPlusPcContent = _mgr_LiveLock_base_IsCiPlusPcContent(pstContext);
		if(bIsCiPlusPcContent == TRUE)
		{
			pstContext->bRatingPinPassed = TRUE;
		}
	}
#endif
#endif

	/* 4. Check rating */
	if(pstContext->eAppState > eLiveLockState_PID_START)
	{
		_mgr_LiveLock_base_ShowProgram(pstContext);
	}
	else
	{
		/* 다른 state에서는 그냥 기록만 해 둔다. */
	}

	if (SVC_SI_FreeEventInfo(&stEventInfo) != ERR_OK)
	{
		HxLOG_Error("SVC_SI_FreeEventInfo() failed..! \n");
	}

	NOT_USED_PROC_ARGUMENT (hHandle, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}

BUS_Result_t XMGR_LiveLock_BASE_EvtSiEitTimeout (xMGR_LiveLock_Context_t *pstContext, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return XMGR_LiveLock_BASE_EvtSiEit(pstContext, hHandle, p1,  p2, p3);
}

BUS_Result_t XMGR_LiveLock_BASE_EvtSiPmt (xMGR_LiveLock_Context_t *pstContext, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
#if defined(CONFIG_MW_SI_PMT_RATING)
	HERROR		hErr = ERR_FAIL;
	HUINT32		eRatingValue;
#endif
	HUINT32		ulActionId = SVC_PIPE_GetActionId (hHandle);

	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#if defined(CONFIG_MW_SI_PMT_RATING)
	if(pstContext->bPmtValid != TRUE)
	{
		/* 첫 번째 PMT들어오는 경우 처리... */
		pstContext->bPmtValid = TRUE;
	}

	hErr = OxXSVC_SI_JP_GetParentRateFromPmt(ulActionId, pstContext->hSvc, &eRatingValue);
	if(hErr == ERR_OK)
	{
		/* PMT에서 PC 정보 들어옴. EIT보다 PMT PC가 우선하므로 무조건 PC value update 함. */
		HxLOG_Error("[AP:Lock(%d)] \t=> Found PMT rating value (0x%x).\n",ulActionId,eRatingValue);
		pstContext->bPmtRatingValid = TRUE;
		pstContext->bRatingValid = TRUE;
		pstContext->eRatingValue = eRatingValue;

		_mgr_LiveLock_base_RegisterParentalControlService(pstContext);
	}

#if defined(CONFIG_MW_SI_PMT_RATING)
		if(pstContext->eAppState > eLiveLockState_PID_START)
		{ /* PID 시작 이후에서는 rating check 한번 돌림. */
			_mgr_LiveLock_base_ShowProgram(pstContext);
		}
#endif
#elif defined(CONFIG_MW_CAS_NAGRA_FEATURE_PARENTAL_CONTROL)
	/*	NAGRA CAS의 PARENTAL CONTROL을 사용할 경우 RATING 정보를 기다리지 않도록 한다.
		(NAGRA CAS SYSTEM에 의한 RATING CONTROL만을 사용)
		추후, EIT 등의 RATING 정보와 함께 사용해야 하는 MODEL의 경우는 시나리오를 고려하자. */
	if(pstContext->stSvcInfo.eCasType & eDxCAS_TYPE_FTA)
	{
		pstContext->bRatingValid = TRUE;
	}
#endif
	//--------------------------------------------------------
 	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);
	return ERR_BUS_NO_ERROR;

}

#if defined(CONFIG_MW_CAS)
BUS_Result_t XMGR_LiveLock_BASE_EvtCasOk(xMGR_LiveLock_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	pstContext->bCasOk = TRUE;
#if defined(CONFIG_MW_CAS_NAGRA_FEATURE_PARENTAL_CONTROL)
	/*	NAGRA CAS의 PARENTAL CONTROL을 사용할 경우 RATING 정보를 기다리지 않도록 한다.
		(NAGRA CAS SYSTEM에 의한 RATING CONTROL만을 사용)
		추후, EIT 등의 RATING 정보와 함께 사용해야 하는 MODEL의 경우는 시나리오를 고려하자. */
	if( (DxCAS_GroupId_e)p1 == eDxCAS_GROUPID_NA)
	{
		pstContext->bRatingValid = TRUE;
	}
#endif

	if(pstContext->eAppState > eLiveLockState_PID_START)
	{
		_mgr_LiveLock_base_ShowProgram(pstContext);
	}

	//--------------------------------------------------------
 	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);

	return ERR_BUS_NO_ERROR;
}

BUS_Result_t XMGR_LiveLock_BASE_EvtCasFail(xMGR_LiveLock_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	pstContext->bCasOk = FALSE;
#if defined(CONFIG_MW_CAS_NAGRA_FEATURE_PARENTAL_CONTROL)
	/*	NAGRA CAS의 PARENTAL CONTROL을 사용할 경우 RATING 정보를 기다리지 않도록 한다.
		(NAGRA CAS SYSTEM에 의한 RATING CONTROL만을 사용)
		추후, EIT 등의 RATING 정보와 함께 사용해야 하는 MODEL의 경우는 시나리오를 고려하자. */
	if( (DxCAS_GroupId_e)p1 == eDxCAS_GROUPID_NA)
	{
		pstContext->bRatingValid = TRUE;
	}
#elif defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)
	if( (DxCAS_GroupId_e)p1 == eDxCAS_GROUPID_VX)
	{
#if defined(CONFIG_OP_TDC)
		// 아직 OP에서 EIT rating이나 cas rating을 사용할 계획이 없다고 하니 막아둔다
		pstContext->bRatingValid = TRUE;
#else
		// 아직은 이쪽을 타는 케이스는 없음
		HUINT32 ulRating = 0;

		if(MGR_CAS_GetRatingFromCasMsg((void *)p2, &ulRating) == ERR_OK)
		{
			pstContext->bRatingValid = TRUE;
			pstContext->eRatingValue = ulRating;
		}
		else
		{
			pstContext->bRatingValid = TRUE;
			pstContext->eRatingValue = eDxRATING_VIEW_ALL; // TODO_VMX : 일단 보여주자.
		}
#endif
	}
#endif

	if(pstContext->eAppState > eLiveLockState_PID_START)
	{
		_mgr_LiveLock_base_ShowProgram(pstContext);
	}

	//--------------------------------------------------------
 	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);

	return ERR_BUS_NO_ERROR;

}

BUS_Result_t XMGR_LiveLock_BASE_EvtCasScRemoved(xMGR_LiveLock_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	if(pstContext->bSCPinCodeValid == TRUE)
	{ /* SC-PIN-Code valid 였는데... */
		if(SVC_CAS_CheckSmartCardValidity(eDxCAS_GROUPID_NA) != TRUE)
		{ /* SC-PIN-Code 검증 기능을 사용할 수 없게 되었다. */
			HxLOG_Error("[AP:Lock(%d)] SC-PIN-Code is no longer valid !\n", ulActionId);

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
			pstContext->bSCPinRemoved = FALSE;
#endif
			pstContext->bSCPinCodeValid = FALSE;
			pstContext->bSCPinPassed = FALSE; /* SC PIN Pass flag를 reset하고... */
			DbSvc_SetRatingPassedEvent(&pstContext->stSvcInfo, -1); /* passed event ID 기록도 삭제한다. */

			if(pstContext->eAppState > eLiveLockState_PID_START)
			{ /* 현재 SC-PIN-Code로 돌고 있을 것이므로 다시 STB-PIN-Code로 바꾼다. */
				HxLOG_Error("Check if SC-PIN-Code should be deactivated.\n");
				_mgr_LiveLock_base_ShowProgram(pstContext);
			}
		}
	}

	//--------------------------------------------------------
	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}

#if defined(CONFIG_MW_CI_PLUS_V_1_3)
BUS_Result_t XMGR_LiveLock_BASE_EvtCasDrmSignalled(xMGR_LiveLock_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	DxCopyrightControl_t	stDrmInfo;

	if(pstContext == NULL || p3 == NULL)
	{
		return ERR_BUS_NO_ERROR;
	}

	HLIB_STD_MemCpySafe(&stDrmInfo, sizeof(DxCopyrightControl_t), (void *)p3, sizeof(DxCopyrightControl_t));

	HxLOG_Info("(+)Enter!!\n");

	if(stDrmInfo.stCasCcInfo.eCasCcType == eDxCopyrightControl_CasCcType_CiPlus)
	{
		switch(stDrmInfo.stCasCcInfo.info.stCiPlusDrmInfo.eCiPlusDrmType)
		{
			case eDxCopyrightControl_CiPlusDrmType_ParentalCtrl:
				_mgr_LiveLock_base_CheckCasPcLock(pstContext, &stDrmInfo.stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusParentalCtrlInfo);
				break;
			default:
				HxLOG_Info("CI+ DRM Type[%02x] is not handled at xmgr_live_lock_base module...\n", stDrmInfo.stCasCcInfo.info.stCiPlusDrmInfo.eCiPlusDrmType);
				break;
		}
	}
	else
	{
		HxLOG_Info("Current DRM Message is not for CI+...\n");
	}

	HxLOG_Info("(-)Leave!!\n");
	//--------------------------------------------------------
	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}

BUS_Result_t XMGR_LiveLock_BASE_EvtCasDrmAvBlank(xMGR_LiveLock_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	DxCAS_GroupId_e		eCasGroupId = (DxCAS_GroupId_e)p1;
	HBOOL				bIsBlank = (HBOOL)p2;

	HxLOG_Info("(+)Enter!!\n");
	if(eCasGroupId == eDxCAS_GROUPID_CAM)
	{
		if (bIsBlank == FALSE)
		{
			_mgr_LiveLock_base_SetAppState(pstContext, eLiveLockState_AV);
		}
		else
		{
			// show 되고있는 A/V를 막아줘야 하는 상황인데, 현재 이쪽으로 루틴을 탈 일은 없음.
			HxLOG_Warning("Blank AV...\n");
			//_mgr_LiveLock_base_SetAppState(pstContext, eLiveLockState_LOCK_PIN);
		}
	}
	else
	{
		HxLOG_Error("ERROR: CAS Group ID should be CAM... check it...\n");
	}

	HxLOG_Info("(-) Leave!!\n");
	//--------------------------------------------------------
	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}

#endif
#endif

BUS_Result_t XMGR_LiveLock_BASE_MsgWtvSetRatingPassedFlag(xMGR_LiveLock_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	pstContext->bRatingPinPassed = p1;

	_mgr_LiveLock_base_ShowProgram(pstContext);

	//--------------------------------------------------------
 	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}

BUS_Result_t XMGR_LiveLock_BASE_MsgAppPinOk(xMGR_LiveLock_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{

	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	if(pstContext->eAppState == eLiveLockState_LOCK_PIN)
	{
		pstContext->bLockPinPassed = TRUE;
		MGR_LIVE_SvcLockSetPassed(pstContext->ulActionId, pstContext->hSvc);

		if(pstContext->bRatingValid == TRUE)
		{
			/* Lock check 중 rating value가 들어왔으면 rating PIN까지도 통과한 것으로 처리해야 함. */
			HxLOG_Error ("[AP:Lock(%d)] \t=> Rating value already arrived. Regard rating PIN passed, too.\n",0);
			pstContext->bRatingPinPassed = TRUE; /* 바로 pass 처리함... */
		}
		else
		{
			/* Rating value 결정 이전에 lock pass하면 바로 다음 event는 rating pass로 하기 위하여 flag setting. */
			pstContext->bRatingPassByLockPass = TRUE;
		}

		_mgr_LiveLock_base_SendBusMessage(NULL, eMEVT_LIVE_NOTIFY_SVC_UNLOCKED, SVC_PIPE_GetActionHandle (pstContext->ulActionId), 0, 0, 0);

		/* Service lock이 풀렸으니 PID 시작한다. */
		_mgr_LiveLock_base_StartPid(pstContext);
	}
	else if(pstContext->eAppState == eLiveLockState_RATING_PIN)
	{
		pstContext->bRatingPinPassed = TRUE;

		_mgr_LiveLock_base_RegisterRatingPassedEvent(pstContext);
		_mgr_LiveLock_base_ShowProgram(pstContext);
	}
	else if(pstContext->eAppState == eLiveLockState_SC_PIN)
	{
		pstContext->bSCPinPassed = TRUE;
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
		pstContext->bSCPinRemoved = TRUE;
#endif
		_mgr_LiveLock_base_RegisterRatingPassedEvent(pstContext);
		_mgr_LiveLock_base_ShowProgram(pstContext);
	}
	else
	{
		HxLOG_Error("[AP:Lock(%d)] \t=> Unexpected message in this state(%s). Ignore this message ...\n", ulActionId,
											_mgr_LiveLock_base_GetAppStateStr(pstContext->eAppState));
		return MESSAGE_BREAK;
	}

	//--------------------------------------------------------
 	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}

BUS_Result_t XMGR_LiveLock_BASE_MsgAppPinDenied(xMGR_LiveLock_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/* PIN deny는 다른 변경 없이 UI 만 없어진 것이므로 UI만 다시 생성해준다. */
	if(pstContext->eAppState == eLiveLockState_LOCK_PIN)
	{
		_mgr_LiveLock_base_SetUiState(pstContext, eLockUi_LOCK_PIN_DENIED);
	}
	else if(pstContext->eAppState == eLiveLockState_RATING_PIN)
	{
		_mgr_LiveLock_base_SetUiState(pstContext, eLockUi_RATING_PIN_DENIED);
	}
	else if(pstContext->eAppState == eLiveLockState_SC_PIN)
	{
		_mgr_LiveLock_base_SetUiState(pstContext, eLockUi_SC_PIN_DENIED);

	}

	//--------------------------------------------------------
 	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}

BUS_Result_t XMGR_LiveLock_BASE_MsgAppCheckRating(xMGR_LiveLock_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	pstContext->bRatingPinPassed = FALSE; /* User에 의해 rating이 바뀐 상황이므로 먼저 넣은 PIN passed flag는 reset 한다. */

	if(pstContext->eAppState > eLiveLockState_PID_START)
	{
		/* PID running state에서는 rating을 다시 체크해 봐야 함. */
		_mgr_LiveLock_base_ShowProgram(pstContext);
	}

	//--------------------------------------------------------
 	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);

	return ERR_BUS_NO_ERROR;
}

BUS_Result_t XMGR_LiveLock_BASE_MsgAppCheckLock(xMGR_LiveLock_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return ERR_BUS_NO_ERROR;
}

BUS_Result_t XMGR_LiveLock_BASE_EvtChLocked(xMGR_LiveLock_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	pstContext->bNoSignal = FALSE;
	_mgr_LiveLock_base_CheckServiceLock(pstContext);

	//--------------------------------------------------------
 	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}

BUS_Result_t XMGR_LiveLock_BASE_EvtNoSignal(xMGR_LiveLock_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);

	/*nosignal->signal로 갈 때 state는 여전히 svc locked 상태여서 appState를 set못하게 되고, view proc에게 svc locked를 알려줄 수 없는 문제 수정*/
	pstContext->bNoSignal = TRUE;

	/*signal->nosignal->signal 동작에서 eMEVT_LIVE_NOTIFY_EVENT_RATING_CTRL_CHANGED 이벤트를 발생시키기 위한 조치
	해당 event가 발생되야 ui에서 pincode destroy/create동작이 수행된다.*/
	pstContext->bCasOk = FALSE;
	if(pstContext->eAppState > eLiveLockState_PID_START)
	{
		_mgr_LiveLock_base_ShowProgram(pstContext);
	}

	//--------------------------------------------------------
 	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}

BUS_Result_t XMGR_LiveLock_BASE_MsgLiveNotifyServiceChanged(xMGR_LiveLock_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	Handle_t				hAction = SVC_PIPE_GetActionHandle (ulActionId);

	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);

	_mgr_LiveLock_base_PostBusMessage(NULL, eMEVT_LIVE_NOTIFY_LOCK_STATE_CHANGED, hAction, 0, 0, 0);

 	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}

BUS_Result_t XMGR_LiveLock_BASE_MsgLiveSetMainviewAction(xMGR_LiveLock_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 					ulOldMainActionId, ulNewMainActionId;
	MgrLive_LiveMode_e	 	eNewLiveMode = eMgrLiveMode_MainLive;

	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);

	if ((pstContext->ulActionId < eActionId_VIEW_FIRST)
		|| (pstContext->ulActionId > eActionId_VIEW_LAST))
	{
		HxLOG_Error("[AP:LiveLock(%d)] invalid context action id(%d)\n",pstContext->ulActionId, pstContext->ulActionId);
		return ERR_BUS_NO_ERROR;
	}

	ulNewMainActionId = p1;
	ulOldMainActionId = p2;

	HxLOG_Debug("[AP:LiveLock(%d)] Context :: LiveMode(%s) actionId(%d) \n", pstContext->ulActionId, (pstContext->eLiveMode == eMgrLiveMode_MainLive) ? "MAIN" : "PIP", pstContext->ulActionId);
	HxLOG_Debug("[AP:LiveLock(%d)] oldMainActionId(%d) newMainActionId(%d) \n",pstContext->ulActionId, ulOldMainActionId, ulNewMainActionId);

	if ((pstContext->eLiveMode == eMgrLiveMode_MainLive)
		&& (pstContext->ulActionId == ulOldMainActionId))
	{
		// Main -> PIP
		HxLOG_Debug("[AP:LiveLock(%d)] MainView -> Pip \n", pstContext->ulActionId);

		eNewLiveMode = eMgrLiveMode_PipLive;
	}
	else if ((pstContext->eLiveMode == eMgrLiveMode_PipLive)
			&& (pstContext->ulActionId == ulNewMainActionId))
	{
		// PIP -> Main
		HxLOG_Debug("[AP:LiveLock(%d)] Pip -> MainView \n", pstContext->ulActionId);

		eNewLiveMode = eMgrLiveMode_MainLive;
	}
	else
	{
		HxLOG_Critical("\n\n");
		return ERR_BUS_NO_ERROR;
	}

	HxLOG_Debug("[AP:LiveLock(%d)] LiveMode trasition (%d -> %d) \n",pstContext->ulActionId, pstContext->eLiveMode, eNewLiveMode);

	// update view context
	pstContext->eLiveMode = eNewLiveMode;

 	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}

BUS_Result_t XMGR_LiveLock_BASE_MsgLiveSetFccMainviewAction(xMGR_LiveLock_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
 	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}

BUS_Result_t XMGR_LiveLock_BASE_MsgLiveNotifyFccTuneStatus(xMGR_LiveLock_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 	ulTuneMsg = (HUINT32)p1;
	HERROR		hErr = ERR_FAIL;

	switch (ulTuneMsg)
	{
		case eSEVT_CH_LOCKED:
			XMGR_LiveLock_BASE_EvtChLocked(pstContext, ulActionId, p1, p2, p3);
			break;

		case eSEVT_CH_NO_SIGNAL:
			XMGR_LiveLock_BASE_EvtNoSignal(pstContext, ulActionId, p1, p2, p3);
			break;

		default:
			// do nothing
			HxLOG_Error("Invalid tuner state - (%d) \n", ulTuneMsg);
			break;
	}

	hErr = SVC_AV_SetAudioHide(ulActionId, eAvMuteFlag_Fcc,  FALSE);
	if (hErr)
	{
		HxLOG_Error("SVC_AV_SetAudioHide ret :0x%X\n", hErr);
	}

	hErr = SVC_AV_SetVideoHide(ulActionId, eAvMuteFlag_Fcc, FALSE);
	if (hErr)
	{
		HxLOG_Error("SVC_AV_SetVideoHide ret :0x%X\n", hErr);
	}


	return MESSAGE_PASS;
}

BUS_Result_t XMGR_LiveLock_BASE_MsgBusGoStandby(xMGR_LiveLock_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);

	switch (pstContext->eLiveMode)
	{
	case eMgrLiveMode_LiveStreaming:
		HxLOG_Info ("The Live module works for the live streaming: It shall not be killed by 'GoStandby!'\n");
		break;

	case eMgrLiveMode_MainLive:
	case eMgrLiveMode_PipLive:
		BUS_MGR_Destroy (NULL);
		break;

	default:
		HxLOG_Error ("Invalid LiveMode(%d)... Destroy the procedure anyway.\n", pstContext->eLiveMode);
		BUS_MGR_Destroy (NULL);
		break;
	}

	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);
	return MESSAGE_PASS;
}

BUS_Result_t XMGR_LiveLock_BASE_EvtDbCheckSvcDb(xMGR_LiveLock_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	DbSvc_Info_t	 stSvcInfo;
	HERROR			 hErr;

	hErr = DB_SVC_GetServiceInfo(pstContext->hSvc, &stSvcInfo);
	if (ERR_OK == hErr)
	{
		if ((DbSvc_GetCasType(&(pstContext->stSvcInfo)) != DbSvc_GetCasType(&stSvcInfo)) ||
			(DbSvc_GetOpType(&(pstContext->stSvcInfo)) != DbSvc_GetOpType(&stSvcInfo)) ||
			(DbSvc_GetLocked(&(pstContext->stSvcInfo)) != DbSvc_GetLocked(&stSvcInfo)) ||
			(DbSvc_GetParentalControlFlag(&(pstContext->stSvcInfo)) != DbSvc_GetParentalControlFlag(&stSvcInfo)))
		{
			HxSTD_MemCopy(&(pstContext->stSvcInfo), &stSvcInfo, sizeof(DbSvc_Info_t));

			if(pstContext->eAppState > eLiveLockState_PID_START)
			{
				_mgr_LiveLock_base_ShowProgram(pstContext);
			}
		}
	}

	return MESSAGE_PASS;
}


#define _____PROTOTYPE_PROCEDURE_____

BUS_Result_t XMGR_LiveLock_BASE_ProtoProc (xMGR_LiveLock_Context_t *pstContext, HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t	eResult = ERR_BUS_NO_ERROR;
	HUINT32		ulActionId = SVC_PIPE_GetActionId (hHandle);

	if (NULL == pstContext)
	{
		HxLOG_Error ("Context NULL !!!!\n");
		return MESSAGE_PASS;
	}

	switch (message)
	{
		case eMEVT_BUS_CREATE:
			HxLOG_Print ("ActionId(%d) eMEVT_BUS_CREATE (0x%08x:%d,%d,%d)\n", pstContext->ulActionId, hHandle, p1, p2, p3);
			eResult = XMGR_LiveLock_BASE_MsgBusCreate (pstContext, hHandle, p1, p2, p3);
			break;

		case eMEVT_BUS_DESTROY:
			HxLOG_Print ("ActionId(%d) eMEVT_BUS_DESTROY (0x%08x:%d,%d,%d)\n", pstContext->ulActionId, hHandle, p1, p2, p3);
			eResult = XMGR_LiveLock_BASE_MsgBusDestroy (pstContext, hHandle, p1, p2, p3);
			break;

		case eMEVT_LIVE_START_SVC:
			HxLOG_Print ("ActionId(%d) eMEVT_LIVE_START_SVC (0x%08x:%d,%d,%d)\n", pstContext->ulActionId, hHandle, p1, p2, p3);
			eResult = XMGR_LiveLock_BASE_MsgLiveStartSvc (pstContext, hHandle, p1, p2, p3);
			break;

		case eMEVT_LIVE_STOP_SVC:
			HxLOG_Print ("ActionId(%d) eMEVT_LIVE_STOP_SVC (0x%08x:%d,%d,%d)\n", pstContext->ulActionId, hHandle, p1, p2, p3);
			eResult = XMGR_LiveLock_BASE_MsgLiveStopSvc (pstContext, hHandle, p1, p2, p3);
			break;

		case eSEVT_SI_PMT :
			LIVE_CHECK_ACTION_ID();
			HxLOG_Print ("ActionId(%d) eSEVT_SI_PMT (0x%08x:%d,%d,%d)\n", pstContext->ulActionId, hHandle, p1, p2, p3);
#if defined(CONFIG_MW_SI_PMT_RATING)
			eResult = XMGR_LiveLock_BASE_EvtSiPmt (pstContext, hHandle, p1, p2, p3);
#endif
			break;

		case eSEVT_SI_EIT :
			LIVE_CHECK_ACTION_ID();
			HxLOG_Print ("ActionId(%d) eSEVT_SI_EIT (0x%08x:%d,%d,%d)\n", pstContext->ulActionId, hHandle, p1, p2, p3);
			eResult = XMGR_LiveLock_BASE_EvtSiEit (pstContext, hHandle, p1, p2, p3);
			break;

		case eSEVT_SI_EIT_TIMEOUT :
			LIVE_CHECK_ACTION_ID();
			HxLOG_Print ("ActionId(%d) eSEVT_SI_EIT_TIMEOUT (0x%08x:%d,%d,%d)\n", pstContext->ulActionId, hHandle, p1, p2, p3);
			eResult = XMGR_LiveLock_BASE_EvtSiEitTimeout (pstContext, hHandle, p1, p2, p3);
			break;

		/**********************************************
			Rating PIN code passed flag change
		***********************************************/
		case eMEVT_LIVE_SET_RATING_PASSED_FLAG:
			LIVE_CHECK_ACTION_ID();
			HxLOG_Print ("ActionId(%d) eMEVT_LIVE_SET_RATING_PASSED_FLAG (0x%08x:%d,%d,%d)\n", pstContext->ulActionId, hHandle, p1, p2, p3);
			eResult = XMGR_LiveLock_BASE_MsgWtvSetRatingPassedFlag(pstContext, ulActionId, p1, p2, p3);
			break;

		/**********************************************
			PIN code OK
		***********************************************/
		case eMEVT_PG_PIN_CHECK_OK :
			LIVE_CHECK_ACTION_ID();
			HxLOG_Print ("ActionId(%d) MSG_APP_PIN_OK (0x%08x:%d,%d,%d)\n", pstContext->ulActionId, hHandle, p1, p2, p3);
			eResult = XMGR_LiveLock_BASE_MsgAppPinOk(pstContext, ulActionId, p1, p2, p3);
			break;

		/**********************************************
			PIN code NG
		***********************************************/
		case eMEVT_PG_PIN_CHECK_DENIED :
			LIVE_CHECK_ACTION_ID();
			HxLOG_Print ("ActionId(%d) MSG_APP_PIN_DENIED (0x%08x:%d,%d,%d)\n", pstContext->ulActionId, hHandle, p1, p2, p3);
			eResult = XMGR_LiveLock_BASE_MsgAppPinDenied(pstContext, ulActionId, p1, p2, p3);
			break;

		/**********************************************************************
			설정등 application 상에서 rating 관련 정보가 변경되었음. 한번 체크해 봐야 함.
		***********************************************************************/
		case eOEVT_PG_CHECK_RATING :
			HxLOG_Print ("ActionId(%d) MSG_MGR_CHECK_RATING (0x%08x:%d,%d,%d)\n", pstContext->ulActionId, hHandle, p1, p2, p3);
			eResult = XMGR_LiveLock_BASE_MsgAppCheckRating(pstContext, ulActionId, p1, p2, p3);
			break;

		case eOEVT_PG_CHECK_LOCK:
			HxLOG_Print ("ActionId(%d) MSG_MGR_CHECK_LOCK (0x%08x:%d,%d,%d)\n", pstContext->ulActionId, hHandle, p1, p2, p3);
			eResult = XMGR_LiveLock_BASE_MsgAppCheckLock(pstContext, ulActionId, p1, p2, p3);
			break;

		case eSEVT_CH_LOCKED :
			LIVE_CHECK_ACTION_ID();
			HxLOG_Print ("ActionId(%d) eSEVT_CH_LOCKED (0x%08x:%d,%d,%d)\n", pstContext->ulActionId, hHandle, p1, p2, p3);
			eResult = XMGR_LiveLock_BASE_EvtChLocked(pstContext, ulActionId, p1, p2, p3);
			break;

		case eSEVT_CH_NO_SIGNAL:
			LIVE_CHECK_ACTION_ID();
			HxLOG_Print ("ActionId(%d) eSEVT_CH_NO_SIGNAL (0x%08x:%d,%d,%d)\n", pstContext->ulActionId, hHandle, p1, p2, p3);
			eResult = XMGR_LiveLock_BASE_EvtNoSignal(pstContext, ulActionId, p1, p2, p3);
			break;

#if defined(CONFIG_MW_CAS)
		/*pstContext->bCasOk 가 설정되지 않으면 rating이 걸려도 계속 wating상태로 남아있게 된다.*/
		case eSEVT_CAS_OK:
			LIVE_CHECK_ACTION_ID();
			HxLOG_Print ("ActionId(%d) eSEVT_CAS_OK (0x%08x:%d,%d,%d)\n", pstContext->ulActionId, hHandle, p1, p2, p3);
			eResult = XMGR_LiveLock_BASE_EvtCasOk(pstContext, ulActionId, p1, p2, p3);
			break;

		case eSEVT_CAS_FAIL:
			LIVE_CHECK_ACTION_ID();
			HxLOG_Print ("ActionId(%d) eSEVT_CAS_FAIL (0x%08x:%d,%d,%d)\n", pstContext->ulActionId, hHandle, p1, p2, p3);
			eResult = XMGR_LiveLock_BASE_EvtCasFail(pstContext, ulActionId, p1, p2, p3);
			break;

		case eSEVT_CAS_SC_REMOVED:
			HxLOG_Print ("ActionId(%d) eSEVT_CAS_SC_REMOVED (0x%08x:%d,%d,%d)\n", pstContext->ulActionId, hHandle, p1, p2, p3);
			eResult = XMGR_LiveLock_BASE_EvtCasScRemoved(pstContext, ulActionId, p1, p2, p3);
			break;

	#if defined(CONFIG_MW_CI_PLUS_V_1_3)
		case eSEVT_CAS_DRM_SIGNALLED:
			LIVE_CHECK_ACTION_ID();
			HxLOG_Print ("ActionId(%d) EVT_CAS_DRM_SIGNALLED (0x%08x:%d,%d,%d)\n", pstContext->ulActionId, hHandle, p1, p2, p3);
			eResult = XMGR_LiveLock_BASE_EvtCasDrmSignalled(pstContext, ulActionId, p1, p2, p3);
			break;
		case eSEVT_CAS_DRM_AV_BLANK:
			LIVE_CHECK_ACTION_ID();
			HxLOG_Print ("ActionId(%d) EVT_CAS_DRM_AV_BLANK (0x%08x:%d,%d,%d)\n", pstContext->ulActionId, hHandle, p1, p2, p3);
			eResult = XMGR_LiveLock_BASE_EvtCasDrmAvBlank(pstContext, ulActionId, p1, p2, p3);
			break;
	#endif
#endif

		case eMEVT_LIVE_NOTIFY_SVC_CHANGED:
			LIVE_CHECK_ACTION_ID();
			HxLOG_Print ("ActionId(%d) eMEVT_LIVE_NOTIFY_SVC_CHANGED (0x%08x:%d,%d,%d)\n", pstContext->ulActionId, hHandle, p1, p2, p3);
			eResult = XMGR_LiveLock_BASE_MsgLiveNotifyServiceChanged(pstContext, ulActionId, p1, p2, p3);
			break;

		/**
		  *	main view가 변경됨 (PIP swap)
		**/
		case eMEVT_LIVE_SET_MAINVIEW_ACTION:
			HxLOG_Print ("ActionId(%d) eMEVT_LIVE_SET_MAINVIEW_ACTION (0x%08x:%d,%d,%d)\n", pstContext->ulActionId, hHandle, p1, p2, p3);
			eResult = XMGR_LiveLock_BASE_MsgLiveSetMainviewAction(pstContext, ulActionId, p1, p2, p3);
			break;

		/**
		  *	main view가 변경됨 (FCC svc -> Main svc)
		**/
		case eMEVT_LIVE_SET_FCC_MAINVIEW_ACTION:
#if defined(CONFIG_SUPPORT_FCC)
			HxLOG_Print ("ActionId(%d) eMEVT_LIVE_SET_FCC_MAINVIEW_ACTION (0x%08x:%d,%d,%d)\n", pstContext->ulActionId, hHandle, p1, p2, p3);
			eResult = XMGR_LiveLock_BASE_MsgLiveSetFccMainviewAction(pstContext, ulActionId, p1, p2, p3);
#endif
			break;

		case eMEVT_LIVE_NOTIFY_FCC_TUNE_STATUS:
#if defined(CONFIG_SUPPORT_FCC)
			HxLOG_Print ("ActionId(%d) eMEVT_LIVE_NOTIFY_FCC_TUNE_STATUS (0x%08x:%d,%d,%d)\n", pstContext->ulActionId, hHandle, p1, p2, p3);
			eResult = XMGR_LiveLock_BASE_MsgLiveNotifyFccTuneStatus(pstContext, ulActionId, p1, p2, p3);
#endif
			break;


		/**********************************************************
		  ***	STANDBY
		***********************************************************/
		case eMEVT_BUS_READY_SHUTDOWN:
			HxLOG_Print ("ActionId(%d) eMEVT_BUS_READY_SHUTDOWN (0x%08x:%d,%d,%d)\n", pstContext->ulActionId, hHandle, p1, p2, p3);
			eResult = XMGR_LiveLock_BASE_MsgBusGoStandby(pstContext, ulActionId, p1, p2, p3);
			break;

		case eSEVT_DB_CHECK_SVC_DB:
			HxLOG_Print ("ActionId(%d) eSEVT_DB_CHECK_SVC_DB (0x%08x:%d,%d,%d)\n", pstContext->ulActionId, hHandle, p1, p2, p3);
			eResult = XMGR_LiveLock_BASE_EvtDbCheckSvcDb(pstContext, ulActionId, p1, p2, p3);
			break;

		default:
			eResult = ERR_BUS_NO_ERROR;
			break;
	}

	return eResult;
}

#define _____PROCEDURES_____
STATIC BUS_Result_t  _mgr_LiveLock_base_procView0 (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xMGR_LiveLock_Context_t			*pstContext = _mgr_LiveLock_base_getContext (eActionId_VIEW_FIRST + 0);

	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
	}

	return XMGR_LiveLock_BASE_ProtoProc (pstContext, message, hHandle, p1, p2, p3);
}

STATIC BUS_Result_t  _mgr_LiveLock_base_procView1 (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xMGR_LiveLock_Context_t			*pstContext = _mgr_LiveLock_base_getContext (eActionId_VIEW_FIRST + 1);

	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
	}

	return XMGR_LiveLock_BASE_ProtoProc (pstContext, message, hHandle, p1, p2, p3);
}

#if defined(CONFIG_SUPPORT_FCC)
STATIC BUS_Result_t  _mgr_LiveLock_base_procView2 (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xMGR_LiveLock_Context_t			*pstContext = _mgr_LiveLock_base_getContext (eActionId_VIEW_FIRST + 2);

	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
	}

	return XMGR_LiveLock_BASE_ProtoProc (pstContext, message, hHandle, p1, p2, p3);
}

STATIC BUS_Result_t  _mgr_LiveLock_base_procView3 (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xMGR_LiveLock_Context_t			*pstContext = _mgr_LiveLock_base_getContext (eActionId_VIEW_FIRST + 3);

	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
	}

	return XMGR_LiveLock_BASE_ProtoProc (pstContext, message, hHandle, p1, p2, p3);
}
#endif


STATIC BUS_Result_t  _mgr_LiveLock_base_procBg0 (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xMGR_LiveLock_Context_t			*pstContext = _mgr_LiveLock_base_getContext (eActionId_BG_FIRST + 0);

	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
	}

	return XMGR_LiveLock_BASE_ProtoProc (pstContext, message, hHandle, p1, p2, p3);
}

STATIC BUS_Result_t  _mgr_LiveLock_base_procBg1 (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xMGR_LiveLock_Context_t			*pstContext = _mgr_LiveLock_base_getContext (eActionId_BG_FIRST + 1);

	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
	}

	return XMGR_LiveLock_BASE_ProtoProc (pstContext, message, hHandle, p1, p2, p3);
}

STATIC BUS_Result_t  _mgr_LiveLock_base_procBg2 (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xMGR_LiveLock_Context_t			*pstContext = _mgr_LiveLock_base_getContext (eActionId_BG_FIRST + 2);

	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
	}

	return XMGR_LiveLock_BASE_ProtoProc (pstContext, message, hHandle, p1, p2, p3);
}

STATIC BUS_Result_t  _mgr_LiveLock_base_procBg3 (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xMGR_LiveLock_Context_t			*pstContext = _mgr_LiveLock_base_getContext (eActionId_BG_FIRST + 3);

	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
	}

	return XMGR_LiveLock_BASE_ProtoProc (pstContext, message, hHandle, p1, p2, p3);
}

BUS_Callback_t XMGR_LiveLock_BASE_GetProcByActionId (HUINT32 ulActionId)
{
	xMGR_LiveLock_Context_t		*pstContext = _mgr_LiveLock_base_getContext (ulActionId);
	BUS_Callback_t			 pfLiveProc = NULL;

	if (NULL != pstContext)
	{
		switch (ulActionId)
		{
		case (eActionId_VIEW_FIRST + 0):
			pfLiveProc = _mgr_LiveLock_base_procView0;
			break;

		case (eActionId_VIEW_FIRST + 1):
			pfLiveProc = _mgr_LiveLock_base_procView1;
			break;

#if defined(CONFIG_SUPPORT_FCC)
		case (eActionId_VIEW_FIRST + 2):
			pfLiveProc = _mgr_LiveLock_base_procView2;
			break;

		case (eActionId_VIEW_FIRST + 3):
			pfLiveProc = _mgr_LiveLock_base_procView3;
			break;
#endif

		case (eActionId_BG_FIRST + 0):
			pfLiveProc = _mgr_LiveLock_base_procBg0;
			break;

		case (eActionId_BG_FIRST + 1):
			pfLiveProc = _mgr_LiveLock_base_procBg1;
			break;

		case (eActionId_BG_FIRST + 2):
			pfLiveProc = _mgr_LiveLock_base_procBg2;
			break;

		case (eActionId_BG_FIRST + 3):
			pfLiveProc = _mgr_LiveLock_base_procBg3;
			break;

		default:
			break;
		}
	}

	return pfLiveProc;

}

HERROR XMGR_LiveLock_BASE_Init (void)
{
	_mgr_LiveLock_base_initContext();

	return ERR_OK;
}

HERROR XMGR_LiveLock_BASE_GetState(HUINT32 ulActionId, MgrLiveLock_UiState_e *peLockState)
{
	xMGR_LiveLock_Context_t		*pstContext = NULL;

	if ((ulActionId >= eActionId_MAX) || (peLockState == NULL))
	{
		HxLOG_Debug("[AP:LiveLock(%d)] invalid parameter \n");
		return ERR_FAIL;
	}

	pstContext = _mgr_LiveLock_base_getContext (ulActionId);
	*peLockState = pstContext->eUiState;

	return ERR_OK;
}


HERROR XMGR_LiveLock_BASE_GetRating(HUINT32 ulActionId, HBOOL *pbRatingValid, DxRatingAge_e *peRatingValue)
{
	xMGR_LiveLock_Context_t		*pstContext = NULL;

	if ((ulActionId >= eActionId_MAX) || (pbRatingValid == NULL) || (peRatingValue == NULL))
	{
		HxLOG_Debug("[AP:LiveLock(%d)] invalid parameter \n");
		return ERR_FAIL;
	}

	pstContext = _mgr_LiveLock_base_getContext (ulActionId);
	*pbRatingValid = pstContext->bRatingValid;
	*peRatingValue = pstContext->eRatingValue;

	return ERR_OK;
}

#define ______DEBUG_FUNC_____________
STATIC HINT8* _mgr_LiveLock_base_GetUiStateStr(MgrLiveLock_UiState_e eUiState)
{
	switch (eUiState)
	{
		ENUM_TO_STR(eLockUi_OK);
		ENUM_TO_STR(eLockUi_NONE);
		ENUM_TO_STR(eLockUi_LOCK_PIN);
		ENUM_TO_STR(eLockUi_LOCK_PIN_DENIED);
		ENUM_TO_STR(eLockUi_RATING_PIN);
		ENUM_TO_STR(eLockUi_RATING_PIN_DENIED);
		ENUM_TO_STR(eLockUi_SC_PIN);
		ENUM_TO_STR(eLockUi_SC_PIN_DENIED);
		ENUM_TO_STR(eLockUi_NO_EIT);
		default:		break;
	}

	HxLOG_Error("\neUiState(%d)\n", eUiState);
	return "Unknown_eUiState";
}

STATIC HINT8* _mgr_LiveLock_base_GetAppStateStr(xMGR_LiveLock_AppState_e eAppState)
{
	switch(eAppState)
	{
		ENUM_TO_STR(eLiveLockState_IDLE);
		ENUM_TO_STR(eLiveLockState_LOCK_PIN);
		ENUM_TO_STR(eLiveLockState_PID_START);
		ENUM_TO_STR(eLiveLockState_WAIT_RATING);
		ENUM_TO_STR(eLiveLockState_RATING_PIN);
		ENUM_TO_STR(eLiveLockState_SC_PIN);
		ENUM_TO_STR(eLiveLockState_AV);
		default:		break;
	}

	HxLOG_Error("\neAppState(%d)\n", eAppState);
	return "Unknown_eAppState";
}

void XMGR_LiveLock_BASE_DumpContext(xMGR_LiveLock_Context_t *pstContext)
{
#if defined(CONFIG_DEBUG)

	if (pstContext == NULL)
		return;

	HLIB_CMD_Printf("\n\t======================================================\n");
	HLIB_CMD_Printf("\tLive Lock Module Context[%d]\n\n", pstContext->ulActionId);
	HLIB_CMD_Printf("\teLiveMode[%d]\n", pstContext->eLiveMode);
	HLIB_CMD_Printf("\teAppState(%s) eUiState(%s)\n", _mgr_LiveLock_base_GetAppStateStr(pstContext->eAppState), _mgr_LiveLock_base_GetUiStateStr(pstContext->eUiState));
	HLIB_CMD_Printf("\thSvc(0x%X) hMasterSvc(0x%X)\n", pstContext->hSvc, pstContext->hMasterSvc);
	HLIB_CMD_Printf("\tbEitValid(%d) ulEventId(0x%X)\n", pstContext->bEitValid, pstContext->ulEventId);
	HLIB_CMD_Printf("\tbRatingPassByLockPass(%d) \n", pstContext->bRatingPassByLockPass);
	HLIB_CMD_Printf("\tbRatingValid(%d) eRatingValue(%d)\n", pstContext->bRatingValid, pstContext->eRatingValue);
	HLIB_CMD_Printf("\tbRatingPinPassed(%d) bLockPinPassed(%d)\n", pstContext->bRatingPinPassed, pstContext->bLockPinPassed);
	HLIB_CMD_Printf("\tbCasOk(%d) bGetCasEvt(%d) bCasStateChanged(%d)\n", pstContext->bCasOk, pstContext->bGetCasEvt, pstContext->bCasStateChanged);
	HLIB_CMD_Printf("\tbEitValid(%d) ulEventId(0x%X)\n", pstContext->bEitValid, pstContext->ulEventId);
	HLIB_CMD_Printf("\tbNoSignal[%d]\n", pstContext->bNoSignal);
	HLIB_CMD_Printf("\tbSCPinCodeValid(%d) bSCPinPassed(%d)\n", pstContext->bSCPinCodeValid, pstContext->bSCPinPassed);

#if defined(CONFIG_MW_SI_PMT_RATING)
	HLIB_CMD_Printf("\tbPmtValid(%d) bPmtRatingValid(%d)\n", pstContext->bPmtValid, pstContext->bPmtRatingValid);
#endif

#endif

	return;
}

void XMGR_LiveLock_BASE_PrintContext(void)
{
#if defined(CONFIG_DEBUG)
	HUINT32						ulActionId;
	xMGR_LiveLock_Context_t		*pstContext = NULL;

	for (ulActionId=eActionId_VIEW_FIRST; ulActionId <= eActionId_VIEW_LAST; ulActionId++)
	{
		pstContext = _mgr_LiveLock_base_getContext (ulActionId);
		if (pstContext == NULL)
		{
			continue;
		}

		XMGR_LiveLock_BASE_DumpContext(pstContext);
	}
#endif

	return;
}

/* End of file */
