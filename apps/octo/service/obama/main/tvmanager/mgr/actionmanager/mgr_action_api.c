/**
	@file     ap_actionmanager.c
	@brief    Action application���� creation/destroy �� �����ϰ� ���ҽ��� �Ҵ��ϴ� ���� application.

	Description: WatchTV, Recording, TSR, search �� action application���� �ó������� ���ҽ� ��� ��Ȳ�� ����\n
				���� ��Ű�� ���ֱ⵵ �Ѵ�. Action application�� ���ҽ��� �Ҵ� �޴� API�� �����Ѵ�.
	Module: AP/AM			 	\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <dlib.h>
#include <rlib.h>
#include <papi.h>
#include <octo_common.h>
#include <db_svc.h>
#include <db_param.h>
#include <svc_pipe.h>
//#include <mwc_util.h>
#include <svc_pipe.h>
#include <svc_sbtl.h>
#include <svc_ttx.h>
#include <svc_sys.h>
#include <svc_cas.h>
#include <svc_av.h>
#include <svc_epg.h>
#include <svc_si.h>
#include <svc_fs.h>
#if defined(CONFIG_MW_MM_PVR)
#include <svc_rec.h>
#include <svc_pb.h>
#include <svc_meta.h>
#endif
#include <otl.h>

#include <bus.h>
#include <mgr_common.h>
#include <mgr_action.h>
#if defined(CONFIG_MW_MM_MEDIA)		/* temporary */
#include <mgr_media.h>
#endif
#include <mgr_live.h>
#include <pmgr_action.h>

#include "local_include/_mgr_action.h"

/******** local header files *******/
//#include "int_actionmgr.h"
/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	MgrAction_Type_e	actionType;
	BUS_Callback_t		actionProc;
	HUINT32				actionPriority;

	HBOOL				bNextAction;
	MgrAction_Type_e	nextActionType;
	MgrAction_Type_e	prevActionType;

	HINT32				nSetupCnt;
} MgrAction_Context_t;

typedef struct
{
	HUINT32				ulActionId;

} MgrAction_ViewInfo_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
STATIC MgrAction_Context_t		 s_astActionContext[NUM_MAX_ACTION];
STATIC HUINT32					 s_aulActionPriority[NUM_MAX_ACTION];		// Action Setup �� ����ߴٰ� Take �� ����ϱ� ���� Priority ��.

STATIC MgrAction_ViewInfo_t		 s_astActionViewInfo[NUM_DISPLAY];		// index ������ ViewID (0 ~ NUM_DISPLAY - 1)
STATIC HINT32					 s_nLastActionSetupCnt		= 0;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
#if defined(CONFIG_3RD_MHEG)
STATIC	MgrAction_MhegStatus_e	s_MhegStatus = eAmMhegStatus_None;
#endif
/*******************************************************************/
/***************** Static Function Prototypes  *********************/
/*******************************************************************/

#if defined(CONFIG_MW_MM_PVR)
STATIC HERROR		mgr_action_CheckPossibleRecord (Handle_t hSvc, MgrRec_RecordType_e eRecordType, HUINT32 ulPvrIdx);
#endif

STATIC HERROR 		mgr_action_InitViewInfo(void);


// Debug��
STATIC HERROR		mgr_action_printResourceAllocation (HUINT32 ulActionId);

/*******************************************************************/
/***************** Static Function  *********************/
/*******************************************************************/
STATIC HERROR mgr_action_DestoryBackGroundAction(HUINT32 ulActionId)
{
	if ((ulActionId < eActionId_BG_FIRST) || (ulActionId > eActionId_BG_LAST))
	{
		return ERR_FAIL;
	}

	if (s_astActionContext[ulActionId].actionProc != NULL)
	{
		HxLOG_Print("[AP:AM(%d)] Destroy BG action[%s]\n",ulActionId,MGR_ACTION_GetTypeStr(s_astActionContext[ulActionId].actionType));
		BUS_MGR_DestroyWithReason(s_astActionContext[ulActionId].actionProc, eDESTROY_BUS_ACTION, 0);
	}

	return ERR_OK;
}

STATIC HERROR mgr_action_setActionSetupCount (HUINT32 ulActionId)
{
	if (ulActionId >= eActionId_MAX)
	{
		return ERR_FAIL;
	}

	s_nLastActionSetupCnt++;
	s_astActionContext[ulActionId].nSetupCnt = s_nLastActionSetupCnt;

	return ERR_OK;
}

STATIC HERROR mgr_action_resetActionSetupCount (HUINT32 ulActionId)
{
	if (ulActionId >= eActionId_MAX)
	{
		return ERR_FAIL;
	}

	s_astActionContext[ulActionId].nSetupCnt = -1;
	return ERR_OK;
}

/*******************************************************************/
/********************      API Functions   *************************/
/*******************************************************************/
void MGR_ACTION_Init(void)
{
	HINT32			i;

	(void)mgr_action_RegisterSiTimeoutCallback();

	/* Action context �ʱ�ȭ */
	for(i = 0; i < NUM_MAX_ACTION; i++)
	{
		s_astActionContext[i].actionType = eActionType_NONE;
		/* GWM_Callback_t�� NULL�� broadcasting�̳� self or this�� �ǹ̰� �����Ƿ� �࿩�� NULL�� �ʱ�ȭ ���� ����.
			�޼����� broadcasting �ǰų� API�� �θ��� proc�� destroy��. */
		s_astActionContext[i].actionProc = (BUS_Callback_t)-1;
		s_astActionContext[i].actionPriority = 0;
		s_aulActionPriority[i] = 0;
	}

	mgr_action_InitViewInfo();
}

MgrAction_State_e MGR_ACTION_GetState(void)
{
	return mgr_action_GetState();
}

void MGR_ACTION_SetMainActionId(HUINT32 ulActionId)
{
	HxLOG_Info("[AP:AM(%d)] MGR_ACTION_SetMainActionId(%d)\n",ulActionId,ulActionId);
//	g_ulMainViewId = ulActionId; /* Master action ID�� �ٲٰ�... */
	mgr_action_SetMainViewId(ulActionId);

#if defined(CONFIG_MW_SUBTITLE)
	SVC_SBTL_SetMasterAction(ulActionId);
#endif

	BUS_SendMessage(NULL, eMEVT_ACTION_NOTIFY_MAIN_CHANGED, 0, 0, 0, 0); /* Action�鿡�� �˸���. */
}

HUINT32 MGR_ACTION_GetMainActionId(void)
{
	return mgr_action_GetMainViewId();
}

HUINT32 MGR_ACTION_GetSubActionId(void)
{
	if(eActionId_VIEW_0 == mgr_action_GetMainViewId())
	{
		return eActionId_VIEW_1;
	}
	else
	{
		return eActionId_VIEW_0;
	}
}

HBOOL MGR_ACTION_IsViewAction(HUINT32 ulActionId)
{
	if ((ulActionId >= eActionId_VIEW_FIRST) && (ulActionId <= eActionId_VIEW_LAST))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

HBOOL MGR_ACTION_IsBackGroundAction(HUINT32 ulActionId)
{
	if ((ulActionId >= eActionId_BG_FIRST) && (ulActionId <= eActionId_BG_LAST))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

HERROR MGR_ACTION_DestoryBackGroundAction(HUINT32 ulActionId)
{
	return mgr_action_DestoryBackGroundAction(ulActionId);
}

MgrAction_Type_e MGR_ACTION_GetType(HUINT32 ulActionId)
{
	if (ulActionId < NUM_MAX_ACTION)
	{
		return s_astActionContext[ulActionId].actionType;
	}

	return eActionType_NONE;
}

MgrAction_Priority_e MGR_ACTION_GetPriority (HUINT32 ulActionId)
{
	if (ulActionId < NUM_MAX_ACTION)
	{
		if (eActionType_NONE != s_astActionContext[ulActionId].actionType)
		{
			return (MgrAction_Priority_e)s_astActionContext[ulActionId].actionPriority;
		}
	}

	return eAmPriority_NONE;
}

MgrAction_Priority_e MGR_ACTION_GetPrePriority (HUINT32 ulActionId)
{
	if (ulActionId < NUM_MAX_ACTION)
	{
		return (MgrAction_Priority_e)s_aulActionPriority[ulActionId];
	}

	return eAmPriority_NONE;
}

BUS_Callback_t MGR_ACTION_GetActionProc (HUINT32 ulActionId)
{
	if (ulActionId < NUM_MAX_ACTION)
	{
		return s_astActionContext[ulActionId].actionProc;
	}

	return NULL;
}

HERROR MGR_ACTION_GetActionSetupCount (HUINT32 ulActionId, HUINT32 *pulCount)
{
	if ((NULL == pulCount) || (ulActionId >= NUM_MAX_ACTION))
	{
		return ERR_FAIL;
	}

	if ((eActionType_NONE == s_astActionContext[ulActionId].actionType) || (s_astActionContext[ulActionId].nSetupCnt <= 0))
	{
		return ERR_FAIL;
	}

	*pulCount = (HUINT32)s_astActionContext[ulActionId].nSetupCnt;
	return ERR_OK;
}


// Delayed Recording �� eRecType_TSR_REC ->  eRecType_REC ���� �����Ѵ�.
HERROR MGR_ACTION_ChangeType(HUINT32 ulActionId, MgrAction_Type_e actionSrcType, MgrAction_Type_e actionTgtType)
{
	HBOOL				bAllowedActType;

	if (ulActionId >= NUM_MAX_ACTION)				{ return ERR_FAIL; }
	if (s_astActionContext[ulActionId].actionType != actionSrcType)		{ return ERR_FAIL; }

	// ToDo: ���⼭ ���Ǵ� Action Type ���� �ֵ��� �Ѵ�.
	bAllowedActType = FALSE;

#if defined(CONFIG_MW_MM_DELAYED_REC)
	// Delayed Recording�� ��� : Source TSR -> Target Rec
	if (actionSrcType == eRecType_TSR_REC && actionTgtType == eRecType_REC)
	{
		bAllowedActType = TRUE;
	}
#endif

	// PVR Chase PB -> Normal PB
	if (actionSrcType == eViewType_CHASE_PB && actionTgtType == eViewType_PB)
	{
		bAllowedActType = TRUE;
	}

#if defined(CONFIG_SUPPORT_FCC)
	// FCC Live -> Main Live
	if (actionSrcType == eViewType_FCC && actionTgtType == eViewType_WTV)
	{
		bAllowedActType = TRUE;
	}

	// Main Live -> Fcc Live
	if (actionSrcType == eViewType_WTV && actionTgtType == eViewType_FCC)
	{
		bAllowedActType = TRUE;
	}
#endif


	// End of act type check

	if (bAllowedActType == TRUE)
	{
		s_astActionContext[ulActionId].actionType = actionTgtType;

		BUS_SendMessage (NULL, eMEVT_ACTION_NOTIFY_TYPE_CHANGED, SVC_PIPE_GetActionHandle (ulActionId), actionTgtType, actionSrcType, 0);
		return ERR_OK;
	}

	return ERR_FAIL;
}

HERROR MGR_ACTION_ChangePriority (HUINT32 ulActionId, HUINT32 ulActionPriority)
{

	if (ulActionId >= NUM_MAX_ACTION)				{ return ERR_FAIL; }
	if (s_astActionContext[ulActionId].actionType == eActionType_NONE)		{ return ERR_FAIL; }

	if (s_astActionContext[ulActionId].actionPriority != ulActionPriority)
	{
		HUINT32			 ulOldActPriority = s_astActionContext[ulActionId].actionPriority;

		s_astActionContext[ulActionId].actionPriority = ulActionPriority;
		s_aulActionPriority[ulActionId] = ulActionPriority;

		BUS_SendMessage (NULL, eMEVT_ACTION_NOTIFY_PRIORITY_CHANGED, SVC_PIPE_GetActionHandle (ulActionId), ulActionPriority, ulOldActPriority, s_astActionContext[ulActionId].actionType);
	}

	return ERR_OK;
}

HINT8 *MGR_ACTION_GetTypeStr(MgrAction_Type_e actionType)
{
	switch(actionType)
	{
		ENUM_TO_STR(eActionType_NONE);
		/* View action types */
		ENUM_TO_STR(eViewType_WTV);
		ENUM_TO_STR(eViewType_PIP);
		ENUM_TO_STR(eViewType_FCC);
		ENUM_TO_STR(eViewType_ATV);
		ENUM_TO_STR(eViewType_IP);
		ENUM_TO_STR(eViewType_PB);
		ENUM_TO_STR(eViewType_TSR_PB);
		ENUM_TO_STR(eViewType_CHASE_PB);
		ENUM_TO_STR(eViewType_TV_INPUT);
		ENUM_TO_STR(eViewType_MEDIA);
		ENUM_TO_STR(eViewType_MOVIE_PB);
		ENUM_TO_STR(eViewType_RFVOD);
		ENUM_TO_STR(eActionType_CAS_TUNE);
		/* Recording action types */
		ENUM_TO_STR(eRecType_REC);
		ENUM_TO_STR(eRecType_TSR_REC);
		ENUM_TO_STR(eRecType_EXT_REC);
		/* Background action type s*/
		ENUM_TO_STR(eActionType_SEARCH);
		ENUM_TO_STR(eBgType_TVTV);
		ENUM_TO_STR(eActionType_HOMECHANNEL);
		ENUM_TO_STR(eActionType_EIT_PF_TUNE);
		ENUM_TO_STR(eActionType_BG_TUNE);
		ENUM_TO_STR(eActionType_TRANSCODING_LIVE);
		ENUM_TO_STR(eActionType_TRANSCODING_PLAYBACK);
		ENUM_TO_STR(eActionType_SATIP);
		ENUM_TO_STR(eActionType_NTLS);

		default:	break;
	}

	HxLOG_Print("\tERROR! Unknown action type (%d)\n", actionType);
	return "UNKNOWN Action Type";
}

HERROR MGR_ACTION_GetEmptyBgActionId (HUINT32 *pulActionId)
{
	HUINT32			 ulActionId;

	if (NULL == pulActionId)					{ return ERR_FAIL; }

	*pulActionId = HANDLE_NULL;

	for (ulActionId = eActionId_BG_FIRST; ulActionId <= eActionId_BG_LAST; ulActionId++)
	{
		if (eActionType_NONE == s_astActionContext[ulActionId].actionType)
		{
			*pulActionId = ulActionId;
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

HERROR MGR_ACTION_GetEmptyRecActionId (HUINT32 *pulActionId)
{
	HUINT32			 ulActionId;

	if (NULL == pulActionId)					{ return ERR_FAIL; }

	*pulActionId = HANDLE_NULL;

	for (ulActionId = eActionId_REC_FIRST; ulActionId <=  eActionId_REC_LAST; ulActionId++)
	{
		if (eActionType_NONE == s_astActionContext[ulActionId].actionType)
		{
			*pulActionId = ulActionId;
			return ERR_OK;
		}

	}

	return ERR_FAIL;
}

HERROR MGR_ACTION_GetMasterSvcHandle(HUINT32 ulActionId, Handle_t *phSvc)
{
	HERROR err = ERR_FAIL;

	if(ulActionId >= NUM_VIEW_ACTION)
	{
		return err;
	}

	HxLOG_Print("[%s:%d] actionType(%s)\r\n", __FUNCTION__, __LINE__, MGR_ACTION_GetTypeStr(s_astActionContext[ulActionId].actionType));

	switch(s_astActionContext[ulActionId].actionType)
	{
		case eViewType_WTV:
		case eViewType_FCC:
		err = MGR_LIVE_GetServiceHandle (ulActionId, eLiveViewPlaySvc_MASTER, phSvc);
		break;

		case eViewType_PIP:
		err = MGR_LIVE_GetServiceHandle (ulActionId, eLiveViewPlaySvc_MASTER, phSvc);
		break;

#if defined(CONFIG_MW_MM_PVR)
		case eViewType_TSR_PB:
		case eViewType_CHASE_PB:
			{
				MgrPb_Info_t stInfo;

				HxSTD_memset(&stInfo, 0, sizeof(MgrPb_Info_t));
				err = MGR_PLAYBACK_GetPlaybackInfo(ulActionId, &stInfo);
				if(err == ERR_OK && phSvc)
				{
					*phSvc = stInfo.hMasterSvc;
				}
			}
			break;
#endif
		default:
			HxLOG_Error("actionType(%s)\r\n", MGR_ACTION_GetTypeStr(s_astActionContext[ulActionId].actionType));
			break;
	}

	return err;
}

HERROR MGR_ACTION_GetActualSvcHandle(HUINT32 ulActionId, Handle_t *phSvc)
{
	HERROR err = ERR_FAIL;

	if(ulActionId >= NUM_VIEW_ACTION)
	{
		return err;
	}

	switch(s_astActionContext[ulActionId].actionType)
	{
		case eViewType_WTV:
			err = MGR_LIVE_GetServiceHandle (ulActionId, eLiveViewPlaySvc_ACTUAL, phSvc);
			break;

#if defined(CONFIG_MW_MM_PVR)
		case eViewType_TSR_PB:
		case eViewType_CHASE_PB:
			{
				MgrPb_Info_t stInfo;

				HxSTD_memset(&stInfo, 0, sizeof(MgrPb_Info_t));
				err = MGR_PLAYBACK_GetPlaybackInfo(ulActionId, &stInfo);
				if(err == ERR_OK && phSvc)
				{
					*phSvc = stInfo.hSvc;
				}
			}
			break;
#endif
		default:
			break;
	}

	return err;
}

HERROR MGR_ACTION_GetSvcListGroup(HUINT32 ulActionId, ChListUiGroup_t *peSvcListGroup)
{
	HERROR err = ERR_FAIL;

	if(ulActionId >= NUM_VIEW_ACTION)
	{
		return err;
	}

	switch(s_astActionContext[ulActionId].actionType)
	{
		case eViewType_WTV:
		case eViewType_ATV:
			*peSvcListGroup = eChListUiGroup_TV;
			break;

#if defined(CONFIG_MW_MM_PVR)
		case eViewType_PB:
		case eViewType_TSR_PB:
		case eViewType_CHASE_PB:
			{
				MgrPb_Info_t stInfo;

				HxSTD_memset(&stInfo, 0, sizeof(MgrPb_Info_t));
				err = MGR_PLAYBACK_GetPlaybackInfo(ulActionId, &stInfo);
				if(err == ERR_OK && peSvcListGroup)
				{
					*peSvcListGroup = stInfo.eSvcListGroup;
				}
			}
			break;
#endif
		default:
			break;
	}

	return err;
}

HINT8 *MGR_ACTION_GetStateStr(MgrAction_State_e state)
{
	switch(state)
	{
		ENUM_TO_STR(eAmState_IDLE);
		ENUM_TO_STR(eAmState_NORMAL);
		ENUM_TO_STR(eAmState_ACTIVE_STANDBY);
	}

	return "UNKNOWN";
}

int MGR_ACTION_PrintContext(void *arg)
{
	HUINT32		i;

	HxLOG_Print("\nAction Manager State [%s]\n", MGR_ACTION_GetStateStr(mgr_action_GetState()));
	HxLOG_Print("Main View ID : %d\n", mgr_action_GetMainViewId());

	for(i = 0; i < NUM_MAX_ACTION; i++)
	{
		HxLOG_Print("========================================================\n");
		HxLOG_Print("\tAction Context :%d\n", i);
		HxLOG_Print("\t\tAction Type : %s\n", MGR_ACTION_GetTypeStr(s_astActionContext[i].actionType));
		HxLOG_Print("\t\tAction Proc : 0x%x\n", (HINT32)s_astActionContext[i].actionProc);
		HxLOG_Print("========================================================\n");
	}

	return 0;
}

void MGR_ACTION_StartBatchProc(DxBatchType_e eBatchType)
{
	HxLOG_Print("Message  MGR_ACTION_StartBatchProc..\n");
	BUS_SendMessage(NULL, eMEVT_BUS_START_BATCH, 0, eBatchType, 0, 0);
}


void MGR_ACTION_RequestShutdown(void)
{
	HINT32	i;

	for(i = 0; i < NUM_MAX_ACTION; i++)
	{ /* Set all the next context to NONE to prevent post-processing of action release. */
		s_astActionContext[i].bNextAction = TRUE;
		s_astActionContext[i].nextActionType = eActionType_NONE;
	}
	PAPI_RequestShutDown();
}

/*
	���� operation���� ������ ��û �ϴ� ����� rsv �ܿ� Ư���� ���� ���� ����.
	sama �и� �Ŀ��� ������� ���� ������ ����.
*/
void MGR_ACTION_RequestOperation(WakeUpReason_t wakeUpReason, WakeUpSecondReason_t wakeUpSecondReason)
{
	SVC_SYS_SetWakeUpReason(wakeUpReason);				// it will be move to pama.
	SVC_SYS_SetWakeUpSecondReason(wakeUpSecondReason);   // it will be move to pama.
	PAPI_RequestOperation();
}

void MGR_ACTION_RebootSystem (void)
{
	PAPI_RequestReboot();
}

HERROR MGR_ACTION_WaitShutDown(HUINT32 mili)
{
	return PAPI_WaitForShutDown(mili);
}

HERROR MGR_ACTION_WaitReboot(HUINT32 mili)
{
	return PAPI_WaitForReboot(mili);
}

HERROR MGR_ACTION_Take(HUINT32 ulActionId, MgrAction_Type_e actionType, BUS_Callback_t actionProc)
{
	HUINT32			 actionPriority;

	if(ulActionId >= NUM_MAX_ACTION)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}
	HxLOG_Info("[AP:AM(%d)] MGR_ACTION_Take(%d, %s)\n",ulActionId,ulActionId,MGR_ACTION_GetTypeStr(actionType));
	HxLOG_Print("[AP:AM(%d)] \t=> Change Action Type [%s] => [%s]\n",ulActionId,MGR_ACTION_GetTypeStr(s_astActionContext[ulActionId].actionType),MGR_ACTION_GetTypeStr(actionType));

	actionPriority = s_aulActionPriority[ulActionId];
//	s_aulActionPriority[ulActionId] = 0;

	if(s_astActionContext[ulActionId].actionType == eActionType_NONE)
	{ /* ��� �ִ� context�� action ���� ��� */
		s_astActionContext[ulActionId].actionType = actionType;
		s_astActionContext[ulActionId].actionProc = actionProc;
		s_astActionContext[ulActionId].actionPriority = actionPriority;
		s_astActionContext[ulActionId].bNextAction = FALSE; /* Next action intercepting�� ���Ͽ� clear�Ѵ�. */

		/* �ش� action type�� ����� application���� ��ó�ϵ��� ��ü application�� �뺸. */
		BUS_SendMessage(NULL, eMEVT_ACTION_NOTIFY_TAKEN, SVC_PIPE_GetActionHandle(ulActionId), actionType, 0, 0);

		return ERR_OK;
	}
	else if(s_astActionContext[ulActionId].actionType == actionType)
	{ /* ���� action type�̸� proc�� �ٽ� ��� */
		s_astActionContext[ulActionId].actionProc = actionProc;
		s_astActionContext[ulActionId].actionPriority = actionPriority;
		s_astActionContext[ulActionId].bNextAction = FALSE; /* Next action intercepting�� ���Ͽ� clear�Ѵ�. */

		return ERR_OK;
	}
	else
	{ /* ��� ���� ������ ���� ���� ����. ���� ���� ����ä�� ��Ȳ... ����� ��� �ȵǰ� ����. */

		HxLOG_Print("[AP:AM(%d)] ****** Action Type [%s] is still remained *****\n",ulActionId,MGR_ACTION_GetTypeStr(actionType));
		return ERR_FAIL;
	}
}


HERROR MGR_ACTION_Release(HUINT32 ulActionId, MgrAction_Type_e actionType)
{
	HUINT32				previousActionPriority;

	HxLOG_Info("[AP:AM(%d)] MGR_ACTION_Release (%s)\n",ulActionId,MGR_ACTION_GetTypeStr(actionType));
	if(ulActionId >= NUM_MAX_ACTION)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if(s_astActionContext[ulActionId].actionType == actionType)
	{
		/* ��ϵ� action�� ������ action ���� ����. ��ϵ� action�� �ٸ��� ���� ����. ���� ���� ������ ���� ���� ������... */
		// ���� Action ���� ����.
		s_astActionContext[ulActionId].prevActionType = s_astActionContext[ulActionId].actionType;

		s_astActionContext[ulActionId].actionType = eActionType_NONE;
		/* GWM_Callback_t�� NULL�� broadcasting�̳� self or this�� �ǹ̰� �����Ƿ� �࿩�� NULL�� �ʱ�ȭ ���� ����.
			�޼����� broadcasting �ǰų� API�� �θ��� proc�� destroy��. */
		s_astActionContext[ulActionId].actionProc = (BUS_Callback_t)-1;
		previousActionPriority = s_astActionContext[ulActionId].actionPriority;
		s_astActionContext[ulActionId].actionPriority = 0;

		s_aulActionPriority[ulActionId] = 0;

		(void)mgr_action_resetActionSetupCount(ulActionId);

		/* ���ҽ� ���� */
		SVC_PIPE_ClearAction (ulActionId);

		/* �ش� action type�� ����� application���� ��ó�ϵ��� ��ü application�� �뺸. */
		BUS_SendMessage(NULL, eMEVT_ACTION_NOTIFY_RELEASED, SVC_PIPE_GetActionHandle(ulActionId), actionType, previousActionPriority, 0);

		/* OCTO 2������ Action Manager�� �ļ� ó���� ������ �ʴ´�. */
		#if 0
		/* �ļ� ó���� ���� ��츦 ó���ϱ� ���Ͽ� default ó�� ȣ�� */
		mgr_action_ActionReleasePostProcess(ulActionId, actionType);
		#endif
		return ERR_OK;
	}

	HxLOG_Error ("***** Action Type [%d:%s] is NOT mine *****\n", ulActionId, MGR_ACTION_GetTypeStr(actionType));
	return ERR_FAIL;
}


HERROR MGR_ACTION_ReleaseAll (void)
{
	HUINT32			 ulActionId;
	HERROR			 hErr;

	for (ulActionId = 0; ulActionId < eActionId_MAX; ulActionId++)
	{
		if (eActionType_NONE != s_astActionContext[ulActionId].actionType)
		{
			hErr = MGR_ACTION_Release (ulActionId, s_astActionContext[ulActionId].actionType);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("MGR_ACTION_Release err: ActionId(%d)\n", ulActionId);
			}
		}
	}

	return ERR_OK;
}

HERROR MGR_ACTION_CountByType (MgrAction_Type_e eActType, HINT32 *pnActNum)
{
	HUINT32 ulActionId = 0;
	HINT32	nCnt = 0;

	if (pnActNum == NULL)
	{
		return ERR_FAIL;
	}

	for (ulActionId = 0; ulActionId < NUM_MAX_ACTION; ulActionId++)
	{
		if (s_astActionContext[ulActionId].actionType == eActType)
		{
			nCnt ++;
		}
	}

	*pnActNum = nCnt;
	return (nCnt > 0) ? ERR_OK : ERR_FAIL;
}

HERROR MGR_ACTION_GetActionIdByActTypeAndIndex (MgrAction_Type_e eActType, HINT32 nIdx, ActionId_t *peActionId)
{
	HUINT32 ulActionId = 0;
	HINT32	nCnt = 0;

	if (peActionId == NULL)
	{
		return ERR_FAIL;
	}

	for (ulActionId = 0; ulActionId < NUM_MAX_ACTION; ulActionId++)
	{
		if (s_astActionContext[ulActionId].actionType == eActType)
		{
			if (nCnt == nIdx)
			{
				*peActionId = ulActionId;
				return ERR_OK;
			}

			nCnt++;
		}
	}

	return ERR_FAIL;
}
#if defined(CONFIG_3RD_MHEG)
void	MGR_ACTION_SetMhegStatus(MgrAction_MhegStatus_e eStatus)
{
	s_MhegStatus = eStatus;
}

MgrAction_MhegStatus_e	MGR_ACTION_GetMhegStatus(void)
{
	return s_MhegStatus;
}
#endif

/* Action setup API's :
	���ҽ��� �Ҵ�ޱ⸦ ���ϴ� action�� ������ action�� release��Ű�� �����ϱ� ���� �غ� �ϰ� �ϴ� API.
	�� API�� ERR_OK�� �����ϸ� ������ action�� ������ǰ� ���ο� action�� �����ϱ� ���� ���ҽ� �Ҵ��� ���
	�̷���� �����̴�. Action�� �׳� ���ϴ� ������ �ϸ� �ȴ�. ERR_FAIL�� �����ϸ� �ó������� �㰡���� �ʴ� �����̰ų�
	���ҽ� �浹�� ���Ͽ� �������� ���ϴ� �����̴�. */
#define _________Action_Setup_API_________
STATIC HERROR mgr_action_SetRfInputAndTunerInfoToRscReqHandle (HBOOL bSharable, HBOOL bRscIdDynamic, DxTuneParam_t *pstTuningInfo, Handle_t hRscReq)
{
	HUINT32					 ulRscId;
	RxTrialArg_t			 unTrialArg;
	HERROR					 hErr;

	unTrialArg.eArgType = eRxARGTYPE_TUNEPARAM;
	unTrialArg.stTuneParam.stTuneParam = *pstTuningInfo;

	// RF-Input: Only available in Satellite
	if (eDxDELIVERY_TYPE_SAT == pstTuningInfo->eDeliType)
	{
		ulRscId = (TRUE == bRscIdDynamic) ? RxRSCID_DYNAMIC : pstTuningInfo->sat.ulRfInputId;

		hErr = SVC_PIPE_AddRequiredResourceToRequestHandle (hRscReq, eRxRSCTYPE_RFINPUT, ulRscId, bSharable, eRxUSAGETYPE_NONE);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("\n\nSVC_PIPE_AddRequiredResourceToRequestHandle err: eRxRSCTYPE_RFINPUT\n");
		}
	}

	// Tuner:
	if (TRUE == bRscIdDynamic)
	{
		ulRscId 	= RxRSCID_DYNAMIC;
	}
	else
	{
		switch (pstTuningInfo->eDeliType)
		{
		case eDxDELIVERY_TYPE_SAT:
			ulRscId		= pstTuningInfo->sat.ulTunerGroupId;
			break;

		case eDxDELIVERY_TYPE_CAB:
			ulRscId		= pstTuningInfo->cab.ulTunerGroupId;
			break;

		case eDxDELIVERY_TYPE_TER:
			ulRscId		= pstTuningInfo->ter.ulTunerGroupId;
			break;

		default:
			ulRscId		= RxRSCID_DYNAMIC;
			break;
		}
	}

	hErr = SVC_PIPE_AddRequiredResourceToRequestHandle (hRscReq, eRxRSCTYPE_TUNER_GROUP, ulRscId, bSharable, eRxUSAGETYPE_NONE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("\n\nSVC_PIPE_AddRequiredResourceToRequestHandle err: eRxRSCTYPE_RFINPUT\n");
	}

	hErr = SVC_PIPE_AddArgumentToRequestHandle (hRscReq, &unTrialArg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("\n\nSVC_PIPE_AddArgumentToRequestHandle err:\n");
	}

	return ERR_OK;
}

STATIC HERROR mgr_action_SetKillableActionToRscReqHandleByActionPriority (HUINT32 ulActionId, HUINT32 ulActionPriority, Handle_t hRscReq)
{
	HUINT32					 ulActionCnt;
	HERROR					 hErr;
	HBOOL					 bAddRemovable = FALSE;
	HINT32					 ulLatestWtvSetupCnt = 0;


	for (ulActionCnt = 0; ulActionCnt < eActionId_MAX; ulActionCnt++)
	{
		if(eViewType_WTV == s_astActionContext[ulActionCnt].actionType)
		{
			ulLatestWtvSetupCnt = s_astActionContext[ulActionCnt].nSetupCnt;
			break;
		}
	}

	for (ulActionCnt = 0; ulActionCnt < eActionId_MAX; ulActionCnt++)
	{
		bAddRemovable = FALSE;

		if (ulActionId != ulActionCnt)
		{
			if ((eActionType_NONE != s_astActionContext[ulActionCnt].actionType) && (ulActionPriority > s_astActionContext[ulActionCnt].actionPriority))
			{
				bAddRemovable = TRUE;
			}
#if defined(CONFIG_SUPPORT_FCC)
			else if(eViewType_FCC == s_astActionContext[ulActionCnt].actionType)
			{
				// ���� eViewType_WTV ���� ������ ������ eViewType_FCC��
				// ������ actionPriority�� ���� ���ο� Action�� Resource�� �纸 �� �� �ִ�.
				// FCC�� �ϳ��� ��� ���� �� ��� Next, Prev ������ �;� Next FCC �� ���� �ȴ�.
				if((s_astActionContext[ulActionCnt].nSetupCnt < ulLatestWtvSetupCnt))
				{
					bAddRemovable = TRUE;
				}
			}
#endif

			if(bAddRemovable == TRUE)
			{
				hErr = SVC_PIPE_AddRemovableActionIdToRequestHandle (hRscReq, ulActionCnt, s_astActionContext[ulActionCnt].actionPriority);
				if (ERR_OK != hErr)
				{
					HxLOG_Error ("SVC_PIPE_AddRemovableActionIdToRequestHandle err: ActionId(%d)\n", ulActionCnt);
				}
			}
		}
	}

	return ERR_OK;
}

STATIC HERROR mgr_action_KillActionsByTrialResult (RxRscTrialResult_t *pstTrialResult)
{
	HUINT32					 ulCnt;

	// �׿��� �Ѵٰ� ������ Action�� ������ �׿��� �����ؾ� �Ѵ�.
	if ((pstTrialResult->ulNumActionToBeKilled > 0) && (NULL != pstTrialResult->pulActionsToBeKilled))
	{
		for (ulCnt = 0; ulCnt < pstTrialResult->ulNumActionToBeKilled; ulCnt++)
		{
			HUINT32			 ulActionId = pstTrialResult->pulActionsToBeKilled[ulCnt];

			if (NULL != s_astActionContext[ulActionId].actionProc)
			{
				//HxLOG_Print ("Proc(%s) with ActionId(%d) shall be destroyed\n", BUS_MGR_GetMgrName(s_astActionContext[ulActionId].actionProc), ulActionId);
				HxLOG_Print ("Proc with ActionId(%d) shall be destroyed\n", ulActionId);
				BUS_MGR_DestroyWithReason(s_astActionContext[ulActionId].actionProc, eDESTROY_BUS_ACTION, 0);
			}
		}
	}

	return ERR_OK;
}

STATIC HERROR mgr_action_SetupDvbLive (HINT32 ulActionId, Handle_t hSvc, MgrAction_Priority_e ePriority, MgrAction_Type_e eActionType)
{
	Handle_t				 hRscReq = (Handle_t)NULL;
	RxUsageType_e			 eAvDecUsage;
	DbSvc_TsInfo_t			 stTsInfo;
	DxTuneParam_t			 stChTuneInfo;
	RxRscTrialResult_t		*pstTrialResult;
	HERROR					 hErr, hResult = ERR_FAIL;

	BUS_SendMessage (NULL, eMEVT_ACTION_SETUP_REQUESTED, SVC_PIPE_GetActionHandle (ulActionId), eActionType, hSvc, 0);

	/* 1. ���� ���� Ȯ�� */
	if (ePriority < MGR_ACTION_GetPriority (ulActionId))
	{
		HxLOG_Error ("Action not allowed for the priority: (previous=0x%x, current=0x%x)\n", MGR_ACTION_GetPriority (ulActionId), ePriority);
		goto END_FUNC;
	}

	hErr = DB_SVC_GetTsInfoByServiceHandle (hSvc, &stTsInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("DB_SVC_GetTsInfoByServiceHandle error\n");
		goto END_FUNC;
	}

	(void)OTL_CONV_DbTuningInfo2DxTuneParam((const DbSvc_TsInfo_t *)&stTsInfo, &stChTuneInfo);

	switch(eActionType)
	{
		case eViewType_WTV:		eAvDecUsage = eRxUSAGETYPE_AVDEC_MAIN;		break;
		case eViewType_PIP:		eAvDecUsage = eRxUSAGETYPE_AVDEC_SUB;		break;
		case eViewType_FCC:		eAvDecUsage = eRxUSAGETYPE_AVDEC_MAIN;		break;

		default:
			HxLOG_Error ("Not support eActionType(%d) ActionId(%d), Priority(0x%x)\n", eActionType, ulActionId, ePriority);
			goto END_FUNC;
	}

	hRscReq = SVC_PIPE_MakeResourceRequestHandle (ulActionId, ePriority);
	if ((Handle_t)NULL == hRscReq)
	{
		HxLOG_Error ("SVC_PIPE_MakeResourceRequestHandle err: ActionId(%d), Priority(0x%x)\n", ulActionId, ePriority);
		goto END_FUNC;
	}

	(void)mgr_action_SetRfInputAndTunerInfoToRscReqHandle (TRUE, TRUE, &stChTuneInfo, hRscReq);
	(void)SVC_PIPE_AddRequiredResourceToRequestHandle (hRscReq, eRxRSCTYPE_DEMUX, RxRSCID_DYNAMIC, TRUE, eRxUSAGETYPE_DEMUX_LIVEREC);
	(void)SVC_PIPE_AddRequiredResourceToRequestHandle (hRscReq, eRxRSCTYPE_AV_DEC, RxRSCID_DYNAMIC, FALSE, eAvDecUsage);

	(void)mgr_action_SetKillableActionToRscReqHandleByActionPriority (ulActionId, ePriority, hRscReq);

	hErr = SVC_PIPE_TryResourceRequest (hRscReq);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_PIPE_TryResourceRequest\n");
		goto END_FUNC;
	}

	pstTrialResult = SVC_PIPE_GetRequestTrialResult (hRscReq);
	if (NULL != pstTrialResult)
	{
		mgr_action_KillActionsByTrialResult (pstTrialResult);
	}

	// ���� Action ID�� ���� Live �ƴ� Action�� �־ �̰��� ���ش�.
	if ((eActionType_NONE != s_astActionContext[ulActionId].actionType) &&
		(eActionType != s_astActionContext[ulActionId].actionType))
	{
		if (NULL != s_astActionContext[ulActionId].actionProc)
		{
			BUS_MGR_DestroyWithReason(s_astActionContext[ulActionId].actionProc, eDESTROY_BUS_ACTION, 0);
		}
	}

	hErr = SVC_PIPE_SetResourcesByTrialResults (hRscReq);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_PIPE_SetResourcesByTrialResults err:\n");
		goto END_FUNC;
	}

	s_aulActionPriority[ulActionId] = ePriority;
	mgr_action_printResourceAllocation (ulActionId);

	// Action�� ���� �� ������ ���:
	(void)mgr_action_setActionSetupCount(ulActionId);

	hResult = ERR_OK;
	HxLOG_Debug("send eMEVT_ACTION_SETUP_SUCCEEDED, ulActionId: %d, actionHandle: %d, eActionType: %d, hSvc: %d\n", ulActionId, SVC_PIPE_GetActionHandle (ulActionId), eActionType, hSvc);
	BUS_SendMessage(NULL, eMEVT_ACTION_SETUP_SUCCEEDED, SVC_PIPE_GetActionHandle (ulActionId), eActionType, hSvc, 0);

END_FUNC:
	if ((Handle_t)NULL != hRscReq)
	{
		SVC_PIPE_DestroyResourceRequestHandle (hRscReq);
	}

	return hResult;
}

HERROR MGR_ACTION_SetupLive (HINT32 ulActionId, Handle_t hSvc, MgrAction_Priority_e ePriority, MgrAction_Type_e eActionType)
{
	DbSvc_TsInfo_t			stTsInfo;
	HERROR				hErr = ERR_FAIL;

	if ((eActionType != eViewType_WTV) && (eActionType != eViewType_PIP) && (eActionType != eViewType_FCC))
	{
		HxLOG_Error ("invalid action type - ulActionId(%d), eActionType(%d) \n", ulActionId, eActionType);
		return ERR_FAIL;
	}

	/* Service handle validity check */
	hErr = DB_SVC_CheckValidServiceHandle (hSvc);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("ulActionId(%d), hSvc(0x%08x), DB_SVC_CheckValidServiceHandle err:\n", ulActionId, hSvc);
		return ERR_FAIL;
	}

	hErr = DB_SVC_GetTsInfoByServiceHandle (hSvc, &stTsInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("ulActionId(%d), hSvc(0x%08x), DB_SVC_GetTsInfoByServiceHandle err:\n", ulActionId, hSvc);
		return ERR_FAIL;
	}

	switch (stTsInfo.eDeliType)
	{
	case eDxDELIVERY_TYPE_SAT:
	case eDxDELIVERY_TYPE_TER:
	case eDxDELIVERY_TYPE_CAB:
		return mgr_action_SetupDvbLive (ulActionId, hSvc, ePriority, eActionType);
		break;

	default:
		HxLOG_Error ("ulActionId(%d), hSvc(0x%08x), Invalid delivery type err:\n", ulActionId, hSvc);
		break;
	}

	return ERR_FAIL;
}

STATIC HERROR mgr_action_SetupLiveStreamingViaLiveTv (HINT32 ulActionId, Handle_t hSvc, MgrAction_Priority_e ePriority, MgrAction_Type_e eActionType)
{
	Handle_t				 hRscReq = (Handle_t)NULL;
	DbSvc_TsInfo_t			 stTsInfo;
	DxTuneParam_t			 stChTuneInfo;
	RxRscTrialResult_t		*pstTrialResult;
	HERROR					 hErr, hResult = ERR_FAIL;

	/* Service handle validity check */
	hErr = DB_SVC_CheckValidServiceHandle (hSvc);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("ulActionId(%d), hSvc(0x%08x), DB_SVC_CheckValidServiceHandle err:\n", ulActionId, hSvc);
		goto END_FUNC;
	}

	hErr = DB_SVC_GetTsInfoByServiceHandle (hSvc, &stTsInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("ulActionId(%d), hSvc(0x%08x), DB_SVC_GetTsInfoByServiceHandle err:\n", ulActionId, hSvc);
		goto END_FUNC;
	}

	BUS_SendMessage (NULL, eMEVT_ACTION_SETUP_REQUESTED, SVC_PIPE_GetActionHandle (ulActionId), eActionType, hSvc, 0);

	/* 1. ���� ���� Ȯ�� */
	if (ePriority < MGR_ACTION_GetPriority (ulActionId))
	{
		HxLOG_Error ("Action not allowed for the priority: (previous=0x%x, current=0x%x)\n", MGR_ACTION_GetPriority (ulActionId), ePriority);
		goto END_FUNC;
	}

	OTL_CONV_DbTuningInfo2DxTuneParam((const DbSvc_TsInfo_t *)&stTsInfo, &stChTuneInfo);

	hRscReq = SVC_PIPE_MakeResourceRequestHandle (ulActionId, ePriority);
	if ((Handle_t)NULL == hRscReq)
	{
		HxLOG_Error ("SVC_PIPE_MakeResourceRequestHandle err: ActionId(%d), Priority(0x%x)\n", ulActionId, ePriority);
		goto END_FUNC;
	}

	(void)mgr_action_SetRfInputAndTunerInfoToRscReqHandle (TRUE, TRUE, &stChTuneInfo, hRscReq);
	(void)SVC_PIPE_AddRequiredResourceToRequestHandle (hRscReq, eRxRSCTYPE_DEMUX, RxRSCID_DYNAMIC, FALSE, eRxUSAGETYPE_DEMUX_LIVEREC);
	(void)SVC_PIPE_AddRequiredResourceToRequestHandle (hRscReq, eRxRSCTYPE_TRANSCODER, RxRSCID_DYNAMIC, FALSE, eRxUSAGETYPE_NONE);
	(void)SVC_PIPE_AddRequiredResourceToRequestHandle (hRscReq, eRxRSCTYPE_HDD_REC, RxRSCID_DYNAMIC, FALSE, eRxUSAGETYPE_RECHDD_TRANS);

	(void)mgr_action_SetKillableActionToRscReqHandleByActionPriority (ulActionId, ePriority, hRscReq);

	hErr = SVC_PIPE_TryResourceRequest (hRscReq);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_PIPE_TryResourceRequest\n");
		goto END_FUNC;
	}

	pstTrialResult = SVC_PIPE_GetRequestTrialResult (hRscReq);
	if (NULL != pstTrialResult)
	{
		// �׿��� �Ѵٰ� ������ Action�� ������ �׿��� �����ؾ� �Ѵ�.
		mgr_action_KillActionsByTrialResult (pstTrialResult);
	}

	// ���� Action ID�� ���� Live �ƴ� Action�� �־ �̰��� ���ش�.
	if ((eActionType_NONE != s_astActionContext[ulActionId].actionType) &&
		(eActionType != s_astActionContext[ulActionId].actionType))
	{
		if (NULL != s_astActionContext[ulActionId].actionProc)
		{
			BUS_MGR_DestroyWithReason(s_astActionContext[ulActionId].actionProc, eDESTROY_BUS_ACTION, 0);
		}
	}

	hErr = SVC_PIPE_SetResourcesByTrialResults (hRscReq);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_PIPE_SetResourcesByTrialResults err:\n");
		goto END_FUNC;
	}

	s_aulActionPriority[ulActionId] = ePriority;
	mgr_action_printResourceAllocation (ulActionId);

	// Action�� ���� �� ������ ���:
	(void)mgr_action_setActionSetupCount(ulActionId);

	hResult = ERR_OK;
	HxLOG_Debug("send eMEVT_ACTION_SETUP_SUCCEEDED, ulActionId: %d, actionHandle: %d, eActionType: %d, hSvc: %d\n", ulActionId, SVC_PIPE_GetActionHandle (ulActionId), eActionType, hSvc);
	BUS_SendMessage(NULL, eMEVT_ACTION_SETUP_SUCCEEDED, SVC_PIPE_GetActionHandle (ulActionId), eActionType, hSvc, 0);

END_FUNC:
	if ((Handle_t)NULL != hRscReq)
	{
		SVC_PIPE_DestroyResourceRequestHandle (hRscReq);
	}

	return hResult;
}

STATIC HERROR mgr_action_SetupLiveStreamingViaPvrPb (HINT32 ulActionId, MgrAction_Priority_e ePriority, MgrAction_Type_e eActionType)
{
	Handle_t				 hRscReq = (Handle_t)NULL;
	RxRscTrialResult_t		*pstTrialResult;
	HERROR					 hErr, hResult = ERR_FAIL;

	BUS_SendMessage (NULL, eMEVT_ACTION_SETUP_REQUESTED, SVC_PIPE_GetActionHandle (ulActionId), eActionType, 0, 0);

	/* 1. ���� ���� Ȯ�� */
	if (ePriority < MGR_ACTION_GetPriority (ulActionId))
	{
		HxLOG_Error ("Action not allowed for the priority: (previous=0x%x, current=0x%x)\n", MGR_ACTION_GetPriority (ulActionId), ePriority);
		goto END_FUNC;
	}

	hRscReq = SVC_PIPE_MakeResourceRequestHandle (ulActionId, ePriority);
	if ((Handle_t)NULL == hRscReq)
	{
		HxLOG_Error ("SVC_PIPE_MakeResourceRequestHandle err: ActionId(%d), Priority(0x%x)\n", ulActionId, ePriority);
		goto END_FUNC;
	}

	(void)SVC_PIPE_AddRequiredResourceToRequestHandle (hRscReq, eRxRSCTYPE_DEMUX, RxRSCID_DYNAMIC, FALSE, eRxUSAGETYPE_DEMUX_PLAYBACK);
	(void)SVC_PIPE_AddRequiredResourceToRequestHandle (hRscReq, eRxRSCTYPE_PVR_PLAYER, RxRSCID_DYNAMIC, FALSE, eRxUSAGETYPE_NONE);
	(void)SVC_PIPE_AddRequiredResourceToRequestHandle (hRscReq, eRxRSCTYPE_TRANSCODER, RxRSCID_DYNAMIC, FALSE, eRxUSAGETYPE_NONE);
	(void)SVC_PIPE_AddRequiredResourceToRequestHandle (hRscReq, eRxRSCTYPE_HDD_REC, RxRSCID_DYNAMIC, FALSE, eRxUSAGETYPE_RECHDD_TRANS);

	(void)mgr_action_SetKillableActionToRscReqHandleByActionPriority (ulActionId, ePriority, hRscReq);

	hErr = SVC_PIPE_TryResourceRequest (hRscReq);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_PIPE_TryResourceRequest\n");
		goto END_FUNC;
	}

	pstTrialResult = SVC_PIPE_GetRequestTrialResult (hRscReq);
	if (NULL != pstTrialResult)
	{
		// �׿��� �Ѵٰ� ������ Action�� ������ �׿��� �����ؾ� �Ѵ�.
		mgr_action_KillActionsByTrialResult (pstTrialResult);
	}

	// ���� Action ID�� ���� Live �ƴ� Action�� �־ �̰��� ���ش�.
	if ((eActionType_NONE != s_astActionContext[ulActionId].actionType) &&
		(eActionType != s_astActionContext[ulActionId].actionType))
	{
		if (NULL != s_astActionContext[ulActionId].actionProc)
		{
			BUS_MGR_DestroyWithReason(s_astActionContext[ulActionId].actionProc, eDESTROY_BUS_ACTION, 0);
		}
	}

	hErr = SVC_PIPE_SetResourcesByTrialResults (hRscReq);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_PIPE_SetResourcesByTrialResults err:\n");
		goto END_FUNC;
	}

	s_aulActionPriority[ulActionId] = ePriority;
	mgr_action_printResourceAllocation (ulActionId);

	// Action�� ���� �� ������ ���:
	(void)mgr_action_setActionSetupCount(ulActionId);

	hResult = ERR_OK;
	HxLOG_Debug("send eMEVT_ACTION_SETUP_SUCCEEDED, ulActionId: %d, actionHandle: %d, eActionType: %d\n", ulActionId, SVC_PIPE_GetActionHandle (ulActionId), eActionType);
	BUS_SendMessage(NULL, eMEVT_ACTION_SETUP_SUCCEEDED, SVC_PIPE_GetActionHandle (ulActionId), eActionType, 0, 0);

END_FUNC:
	if ((Handle_t)NULL != hRscReq)
	{
		SVC_PIPE_DestroyResourceRequestHandle (hRscReq);
	}

	return hResult;
}

HERROR MGR_ACTION_SetupLiveStream (HINT32 ulActionId, Handle_t hSvc, MgrAction_Priority_e ePriority, MgrAction_LiveStreamMode_e eLsMode)
{
//	DbSvc_TsInfo_t			 stTsInfo;
	HERROR					 hErr;

	switch (eLsMode)
	{
	case eLiveStreamMode_ViaLiveView:
		hErr = mgr_action_SetupLiveStreamingViaLiveTv (ulActionId, hSvc, ePriority, eActionType_TRANSCODING_LIVE);
		break;

	case eLiveStreamMode_ViaPvrPb:
		hErr = mgr_action_SetupLiveStreamingViaPvrPb (ulActionId, ePriority, eActionType_TRANSCODING_PLAYBACK);
		break;

	default:
		return ERR_FAIL;
	}

	return hErr;
}

HERROR MGR_ACTION_SetupSearch (HUINT32 ulActionId, DxTuneParam_t *pstTuningInfo, MgrAction_Priority_e ePriority)
{
	Handle_t				 hRscReq = (Handle_t)NULL;
	RxRscTrialResult_t		*pstTrialResult;
	HERROR					 hErr, hResult = ERR_FAIL;

	if (NULL == pstTuningInfo)
	{
		HxLOG_Critical ("Tuning Info NULL:\n");
		goto END_FUNC;
	}

	if (ePriority < MGR_ACTION_GetPriority (ulActionId))
	{
		HxLOG_Error ("Action not allowed for the priority: (previous=0x%x, current=0x%x)\n", MGR_ACTION_GetPriority (ulActionId), ePriority);
		goto END_FUNC;
	}

	BUS_SendMessage (NULL, eMEVT_ACTION_SETUP_REQUESTED, SVC_PIPE_GetActionHandle (ulActionId), eActionType_SEARCH, HANDLE_NULL, 0);

	hRscReq = SVC_PIPE_MakeResourceRequestHandle (ulActionId, ePriority);
	if ((Handle_t)NULL == hRscReq)
	{
		HxLOG_Error ("SVC_PIPE_MakeResourceRequestHandle err: ActionId(%d), Priority(0x%x)\n", ulActionId, ePriority);
		goto END_FUNC;
	}

	(void)mgr_action_SetRfInputAndTunerInfoToRscReqHandle (FALSE, FALSE, pstTuningInfo, hRscReq);
	(void)SVC_PIPE_AddRequiredResourceToRequestHandle (hRscReq, eRxRSCTYPE_DEMUX, RxRSCID_DYNAMIC, FALSE, eRxUSAGETYPE_DEMUX_LIVEREC);

	(void)mgr_action_SetKillableActionToRscReqHandleByActionPriority (ulActionId, ePriority, hRscReq);

	hErr = SVC_PIPE_TryResourceRequest (hRscReq);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_PIPE_TryResourceRequest\n");
		goto END_FUNC;
	}

	pstTrialResult = SVC_PIPE_GetRequestTrialResult (hRscReq);
	if (NULL != pstTrialResult)
	{
		// �׿��� �Ѵٰ� ������ Action�� ������ �׿��� �����ؾ� �Ѵ�.
		mgr_action_KillActionsByTrialResult (pstTrialResult);
	}

	// ���� Action ID�� ���� Action�� �־ �̰��� ���ش�.
	if ((eActionType_NONE != s_astActionContext[ulActionId].actionType) && (NULL != s_astActionContext[ulActionId].actionProc))
	{
		BUS_MGR_DestroyWithReason(s_astActionContext[ulActionId].actionProc, eDESTROY_BUS_ACTION, 0);
	}

	hErr = SVC_PIPE_SetResourcesByTrialResults (hRscReq);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_PIPE_SetResourcesByTrialResults err:\n");
		goto END_FUNC;
	}

	s_aulActionPriority[ulActionId] = ePriority;
	mgr_action_printResourceAllocation (ulActionId);

	// Action�� ���� �� ������ ���:
	(void)mgr_action_setActionSetupCount(ulActionId);

	hResult = ERR_OK;
	HxLOG_Debug("send eMEVT_ACTION_SETUP_SUCCEEDED, ulActionId: %d, actionHandle: %d, eActionType: eActionType_SEARCH\n", ulActionId, SVC_PIPE_GetActionHandle (ulActionId));
	BUS_SendMessage(NULL, eMEVT_ACTION_SETUP_SUCCEEDED, SVC_PIPE_GetActionHandle (ulActionId), eActionType_SEARCH, 0, 0);

END_FUNC:
	if ((Handle_t)NULL != hRscReq)
	{
		SVC_PIPE_DestroyResourceRequestHandle (hRscReq);
	}

	return hResult;
}

HERROR MGR_ACTION_SetupMedia (HUINT32 ulActionId, MgrAction_Type_e eActionType, MgrAction_Priority_e ePriority)
{
	Handle_t				 hRscReq = (Handle_t)NULL;
	RxRscTrialResult_t		*pstTrialResult;
	HERROR					 hErr, hResult = ERR_FAIL;

	if ((eActionType != eViewType_MEDIA) && (eActionType != eViewType_IP))
	{
		HxLOG_Error ("Invalid Action type[%s] for this action Id\n", MGR_ACTION_GetTypeStr(eActionType));
		goto END_FUNC;
	}

	if (TRUE != MGR_ACTION_IsViewAction (ulActionId))
	{
		HxLOG_Error ("Invalid Action Id [%d] for this action Id\n", ulActionId);
		goto END_FUNC;
	}

	BUS_SendMessage (NULL, eMEVT_ACTION_SETUP_REQUESTED, SVC_PIPE_GetActionHandle (ulActionId), eActionType, HANDLE_NULL, 0);

	if (ePriority < MGR_ACTION_GetPriority (ulActionId))
	{
		HxLOG_Error ("Action not allowed for the priority: (previous=0x%x, current=0x%x)\n", MGR_ACTION_GetPriority (ulActionId), ePriority);
		goto END_FUNC;
	}

	if (eActionType == MGR_ACTION_GetType(ulActionId))
	{
		HxLOG_Error ("Already ActionID(%d) Taken by actionType(%s). Use the action as is.\n", ulActionId, MGR_ACTION_GetTypeStr(eActionType));

		s_aulActionPriority[ulActionId] = ePriority;
		goto END_FUNC;
	}

	hRscReq = SVC_PIPE_MakeResourceRequestHandle (ulActionId, ePriority);
	if ((Handle_t)NULL == hRscReq)
	{
		HxLOG_Error ("SVC_PIPE_MakeResourceRequestHandle err: ActionId(%d), Priority(0x%x)\n", ulActionId, ePriority);
		goto END_FUNC;
	}

	(void)SVC_PIPE_AddRequiredResourceToRequestHandle (hRscReq, eRxRSCTYPE_DEMUX, RxRSCID_DYNAMIC, FALSE, eRxUSAGETYPE_DEMUX_PLAYBACK);
	(void)SVC_PIPE_AddRequiredResourceToRequestHandle (hRscReq, eRxRSCTYPE_AV_DEC, RxRSCID_DYNAMIC, FALSE, eRxUSAGETYPE_AVDEC_MAIN);

	(void)mgr_action_SetKillableActionToRscReqHandleByActionPriority (ulActionId, ePriority, hRscReq);

	hErr = SVC_PIPE_TryResourceRequest (hRscReq);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_PIPE_TryResourceRequest\n");
		goto END_FUNC;
	}

	pstTrialResult = SVC_PIPE_GetRequestTrialResult (hRscReq);
	if (NULL != pstTrialResult)
	{
		mgr_action_KillActionsByTrialResult (pstTrialResult);
	}

	// �� ActionID�� �ɸ� ���� ���δ�.
	if (NULL != s_astActionContext[ulActionId].actionProc)
	{
		BUS_MGR_DestroyWithReason(s_astActionContext[ulActionId].actionProc, eDESTROY_BUS_ACTION, 0);
	}

	hErr = SVC_PIPE_SetResourcesByTrialResults (hRscReq);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_PIPE_SetResourcesByTrialResults err:\n");
		goto END_FUNC;
	}

	s_aulActionPriority[ulActionId] = ePriority;
	mgr_action_printResourceAllocation (ulActionId);

	// Action�� ���� �� ������ ���:
	(void)mgr_action_setActionSetupCount(ulActionId);

	hResult = ERR_OK;
	HxLOG_Debug("send eMEVT_ACTION_SETUP_SUCCEEDED, ulActionId: %d, actionHandle: %d, eActionType: %d\n", ulActionId, SVC_PIPE_GetActionHandle (ulActionId), eActionType);
	BUS_SendMessage (NULL, eMEVT_ACTION_SETUP_SUCCEEDED, SVC_PIPE_GetActionHandle (ulActionId), eActionType, HANDLE_NULL, 0);

END_FUNC:
	if ((Handle_t)NULL != hRscReq)
	{
		SVC_PIPE_DestroyResourceRequestHandle (hRscReq);
	}

	return hResult;
}

//#ifdef CONFIG_MW_CI_PLUS
HERROR MGR_ACTION_SetupBgTune (HUINT32 ulActionId, DxTuneParam_t *pstTuningInfo, MgrAction_Priority_e ePriority)
{
	Handle_t				 hRscReq = (Handle_t)NULL;
	RxRscTrialResult_t		*pstTrialResult;
	HERROR					 hErr, hResult = ERR_FAIL;

	if (NULL == pstTuningInfo)
	{
		HxLOG_Critical ("Tuning Info NULL:\n");
		return ERR_FAIL;
	}

	BUS_SendMessage (NULL, eMEVT_ACTION_SETUP_REQUESTED, SVC_PIPE_GetActionHandle (ulActionId), eActionType_BG_TUNE, HANDLE_NULL, 0);

	if (ePriority < MGR_ACTION_GetPriority (ulActionId))
	{
		HxLOG_Error ("Action not allowed for the priority: (previous=0x%x, current=0x%x)\n", MGR_ACTION_GetPriority (ulActionId), ePriority);
		return ERR_FAIL;
	}

	hRscReq = SVC_PIPE_MakeResourceRequestHandle (ulActionId, ePriority);
	if ((Handle_t)NULL == hRscReq)
	{
		HxLOG_Error ("SVC_PIPE_MakeResourceRequestHandle err: ActionId(%d), Priority(0x%x)\n", ulActionId, ePriority);
		goto END_FUNC;
	}

	(void)mgr_action_SetRfInputAndTunerInfoToRscReqHandle (TRUE, FALSE, pstTuningInfo, hRscReq);
	(void)SVC_PIPE_AddRequiredResourceToRequestHandle (hRscReq, eRxRSCTYPE_DEMUX, RxRSCID_DYNAMIC, TRUE, eRxUSAGETYPE_DEMUX_LIVEREC);

	(void)mgr_action_SetKillableActionToRscReqHandleByActionPriority (ulActionId, ePriority, hRscReq);

	hErr = SVC_PIPE_TryResourceRequest (hRscReq);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_PIPE_TryResourceRequest() failed..! \n");
		goto END_FUNC;
	}

	pstTrialResult = SVC_PIPE_GetRequestTrialResult (hRscReq);
	if (NULL != pstTrialResult)
	{
		mgr_action_KillActionsByTrialResult (pstTrialResult);
	}

	// ���� Action ID�� ���� Action�� �־ �̰��� ���ش�.
	if ((eActionType_NONE != s_astActionContext[ulActionId].actionType) && (NULL != s_astActionContext[ulActionId].actionProc))
	{
		BUS_MGR_DestroyWithReason(s_astActionContext[ulActionId].actionProc, eDESTROY_BUS_ACTION, 0);
	}

	hErr = SVC_PIPE_SetResourcesByTrialResults (hRscReq);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_PIPE_SetResourcesByTrialResults err:\n");
		goto END_FUNC;
	}

	s_aulActionPriority[ulActionId] = ePriority;
	mgr_action_printResourceAllocation (ulActionId);

	// Action�� ���� �� ������ ���:
	(void)mgr_action_setActionSetupCount(ulActionId);

	hResult = ERR_OK;
	HxLOG_Debug("send eMEVT_ACTION_SETUP_SUCCEEDED, ulActionId: %d, actionHandle: %d, eActionType: eActionType_BG_TUNE\n", ulActionId, SVC_PIPE_GetActionHandle (ulActionId));
	BUS_SendMessage (NULL, eMEVT_ACTION_SETUP_SUCCEEDED, SVC_PIPE_GetActionHandle (ulActionId), eActionType_BG_TUNE, HANDLE_NULL, 0);

END_FUNC:
	if ((Handle_t)NULL != hRscReq)
	{
		SVC_PIPE_DestroyResourceRequestHandle (hRscReq);
	}

	return hResult;
}

HERROR MGR_ACTION_SetupMediaProve (HUINT32 ulActionId, MgrAction_Priority_e ePriority)
{
	Handle_t				 hRscReq = (Handle_t)NULL;
	RxRscTrialResult_t		*pstTrialResult;
	HERROR					 hErr, hResult = ERR_FAIL;

	if (ePriority < MGR_ACTION_GetPriority (ulActionId))
	{
		HxLOG_Error ("Action not allowed for the priority: (previous=0x%x, current=0x%x)\n", MGR_ACTION_GetPriority (ulActionId), ePriority);
		return ERR_FAIL;
	}

	hRscReq = SVC_PIPE_MakeResourceRequestHandle (ulActionId, ePriority);
	if ((Handle_t)NULL == hRscReq)
	{
		HxLOG_Error ("SVC_PIPE_MakeResourceRequestHandle err: ActionId(%d), Priority(0x%x)\n", ulActionId, ePriority);
		goto END_FUNC;
	}

	(void)SVC_PIPE_AddRequiredResourceToRequestHandle (hRscReq, eRxRSCTYPE_DEMUX, RxRSCID_DYNAMIC, FALSE, eRxUSAGETYPE_DEMUX_PLAYBACK);

	(void)mgr_action_SetKillableActionToRscReqHandleByActionPriority (ulActionId, ePriority, hRscReq);

	hErr = SVC_PIPE_TryResourceRequest (hRscReq);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_PIPE_TryResourceRequest\n");
		goto END_FUNC;
	}

	pstTrialResult = SVC_PIPE_GetRequestTrialResult (hRscReq);
	if (NULL != pstTrialResult)
	{
		// �׿��� �Ѵٰ� ������ Action�� ������ �׿��� �����ؾ� �Ѵ�.
		mgr_action_KillActionsByTrialResult (pstTrialResult);
	}

	// ���� Action ID�� ���� Live �ƴ� Action�� �־ �̰��� ���ش�.
	if (eActionType_NONE != s_astActionContext[ulActionId].actionType)
	{
		if (NULL != s_astActionContext[ulActionId].actionProc)
		{
			BUS_MGR_DestroyWithReason(s_astActionContext[ulActionId].actionProc, eDESTROY_BUS_ACTION, 0);
		}
	}

	hErr = SVC_PIPE_SetResourcesByTrialResults (hRscReq);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_PIPE_SetResourcesByTrialResults err:\n");
		goto END_FUNC;
	}

	s_aulActionPriority[ulActionId] = ePriority;
	mgr_action_printResourceAllocation (ulActionId);

	// Action�� ���� �� ������ ���:
	(void)mgr_action_setActionSetupCount(ulActionId);

	hResult = ERR_OK;

END_FUNC:
	if ((Handle_t)NULL != hRscReq)
	{
		SVC_PIPE_DestroyResourceRequestHandle (hRscReq);
	}

	return hResult;
}

//#ifdef CONFIG_MW_CI_PLUS
HERROR MGR_ACTION_SetupCasTune (HUINT32 ulActionId, Handle_t hSvc, MgrAction_Priority_e ePriority)
{
	Handle_t				 hRscReq = (Handle_t)NULL;
	DbSvc_TsInfo_t			 stTsInfo;
	DxTuneParam_t			 stChTuneInfo;
	RxRscTrialResult_t		*pstTrialResult;
	HERROR					 hErr, hResult = ERR_FAIL;

	BUS_SendMessage (NULL, eMEVT_ACTION_SETUP_REQUESTED, SVC_PIPE_GetActionHandle (ulActionId), eActionType_CAS_TUNE, hSvc, 0);

	hErr = DB_SVC_GetTsInfoByServiceHandle (hSvc, &stTsInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("DB_SVC_GetTsInfoByServiceHandle error\n");
		return ERR_FAIL;
	}

	if (ePriority < MGR_ACTION_GetPriority (ulActionId))
	{
		HxLOG_Error ("Action not allowed for the priority: (previous=0x%x, current=0x%x)\n", MGR_ACTION_GetPriority (ulActionId), ePriority);
		return ERR_FAIL;
	}

	if ((TRUE == MGR_ACTION_IsViewAction (ulActionId)) && (eActionType_CAS_TUNE == MGR_ACTION_GetType (ulActionId)))
	{
		HxLOG_Error ("Already ActionID(%d) Taken by actionType(%s). Use the action as is.\n", ulActionId, MGR_ACTION_GetTypeStr(eActionType_CAS_TUNE));

		s_aulActionPriority[ulActionId] = ePriority;
		return ERR_OK;
	}

	OTL_CONV_DbTuningInfo2DxTuneParam((const DbSvc_TsInfo_t *)&stTsInfo, &stChTuneInfo);

	hRscReq = SVC_PIPE_MakeResourceRequestHandle (ulActionId, ePriority);
	if ((Handle_t)NULL == hRscReq)
	{
		HxLOG_Error ("SVC_PIPE_MakeResourceRequestHandle err: ActionId(%d), Priority(0x%x)\n", ulActionId, ePriority);
		goto END_FUNC;
	}

	(void)mgr_action_SetRfInputAndTunerInfoToRscReqHandle (TRUE, TRUE, &stChTuneInfo, hRscReq);
	(void)SVC_PIPE_AddRequiredResourceToRequestHandle (hRscReq, eRxRSCTYPE_DEMUX, RxRSCID_DYNAMIC, FALSE, eRxUSAGETYPE_DEMUX_LIVEREC);

	(void)mgr_action_SetKillableActionToRscReqHandleByActionPriority (ulActionId, ePriority, hRscReq);

	hErr = SVC_PIPE_TryResourceRequest (hRscReq);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_PIPE_TryResourceRequest\n");
		goto END_FUNC;
	}

	pstTrialResult = SVC_PIPE_GetRequestTrialResult (hRscReq);
	if (NULL != pstTrialResult)
	{
		// �׿��� �Ѵٰ� ������ Action�� ������ �׿��� �����ؾ� �Ѵ�.
		mgr_action_KillActionsByTrialResult (pstTrialResult);
	}

	// ���� Action ID�� ���� Action�� �־ �̰��� ���ش�.
	if ((eActionType_NONE != s_astActionContext[ulActionId].actionType) && (NULL != s_astActionContext[ulActionId].actionProc))
	{
		BUS_MGR_DestroyWithReason(s_astActionContext[ulActionId].actionProc, eDESTROY_BUS_ACTION, 0);
	}

	hErr = SVC_PIPE_SetResourcesByTrialResults (hRscReq);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_PIPE_SetResourcesByTrialResults err:\n");
		goto END_FUNC;
	}

	s_aulActionPriority[ulActionId] = ePriority;
	mgr_action_printResourceAllocation (ulActionId);

	// Action�� ���� �� ������ ���:
	(void)mgr_action_setActionSetupCount(ulActionId);

	hResult = ERR_OK;
	HxLOG_Debug("send eMEVT_ACTION_SETUP_SUCCEEDED, ulActionId: %d, actionHandle: %d, eActionType: eActionType_CAS_TUNE, hSvc: %d\n", ulActionId, SVC_PIPE_GetActionHandle (ulActionId), hSvc);
	BUS_SendMessage (NULL, eMEVT_ACTION_SETUP_SUCCEEDED, SVC_PIPE_GetActionHandle (ulActionId), eActionType_CAS_TUNE, hSvc, 0);

END_FUNC:
	if ((Handle_t)NULL != hRscReq)
	{
		SVC_PIPE_DestroyResourceRequestHandle (hRscReq);
	}

	return hResult;
}
//#endif

#if defined(CONFIG_MW_MM_PVR)
HERROR MGR_ACTION_SetupGetEitPfInfo (HUINT32 ulActionId, Handle_t hSvc, MgrAction_Priority_e ePriority)
{
	Handle_t				 hRscReq = (Handle_t)NULL;
	DbSvc_TsInfo_t			 stTsInfo;
	DxTuneParam_t			 stChTuneInfo;
	RxRscTrialResult_t		*pstTrialResult;
	HERROR					 hErr, hResult = ERR_FAIL;

	BUS_SendMessage (NULL, eMEVT_ACTION_SETUP_REQUESTED, SVC_PIPE_GetActionHandle (ulActionId), eActionType_EIT_PF_TUNE, hSvc, 0);

	if (ePriority < MGR_ACTION_GetPriority (ulActionId))
	{
		HxLOG_Error ("Action not allowed for the priority: (previous=0x%x, current=0x%x)\n", MGR_ACTION_GetPriority (ulActionId), ePriority);
		return ERR_FAIL;
	}

	hErr = DB_SVC_GetTsInfoByServiceHandle (hSvc, &stTsInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("DB_SVC_GetTsInfoByServiceHandle error\n");
		return ERR_FAIL;
	}

	(void)OTL_CONV_DbTuningInfo2DxTuneParam((const DbSvc_TsInfo_t *)&stTsInfo, &stChTuneInfo);

	hRscReq = SVC_PIPE_MakeResourceRequestHandle (ulActionId, ePriority);
	if ((Handle_t)NULL == hRscReq)
	{
		HxLOG_Error ("SVC_PIPE_MakeResourceRequestHandle err: ActionId(%d), Priority(0x%x)\n", ulActionId, ePriority);
		goto END_FUNC;
	}

	(void)mgr_action_SetRfInputAndTunerInfoToRscReqHandle (TRUE, TRUE, &stChTuneInfo, hRscReq);
	(void)SVC_PIPE_AddRequiredResourceToRequestHandle (hRscReq, eRxRSCTYPE_DEMUX, RxRSCID_DYNAMIC, FALSE, eRxUSAGETYPE_DEMUX_LIVEREC);

	(void)mgr_action_SetKillableActionToRscReqHandleByActionPriority (ulActionId, ePriority, hRscReq);

	hErr = SVC_PIPE_TryResourceRequest (hRscReq);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_PIPE_TryResourceRequest\n");
		goto END_FUNC;
	}

	pstTrialResult = SVC_PIPE_GetRequestTrialResult (hRscReq);
	if (NULL != pstTrialResult)
	{
		// �׿��� �Ѵٰ� ������ Action�� ������ �׿��� �����ؾ� �Ѵ�.
		mgr_action_KillActionsByTrialResult (pstTrialResult);
	}

	// ���� Action ID�� ���� Action�� �־ �̰��� ���ش�.
	if ((eActionType_NONE != s_astActionContext[ulActionId].actionType) && (NULL != s_astActionContext[ulActionId].actionProc))
	{
		BUS_MGR_DestroyWithReason(s_astActionContext[ulActionId].actionProc, eDESTROY_BUS_ACTION, 0);
	}

	hErr = SVC_PIPE_SetResourcesByTrialResults (hRscReq);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_PIPE_SetResourcesByTrialResults err:\n");
		goto END_FUNC;
	}

	s_aulActionPriority[ulActionId] = ePriority;
	mgr_action_printResourceAllocation (ulActionId);

	// Action�� ���� �� ������ ���:
	(void)mgr_action_setActionSetupCount(ulActionId);

	hResult = ERR_OK;
	HxLOG_Debug("send eMEVT_ACTION_SETUP_SUCCEEDED, ulActionId: %d, actionHandle: %d, eActionType: eActionType_EIT_PF_TUNE, hSvc: %d\n", ulActionId, SVC_PIPE_GetActionHandle (ulActionId), hSvc);
	BUS_SendMessage (NULL, eMEVT_ACTION_SETUP_SUCCEEDED, SVC_PIPE_GetActionHandle (ulActionId), eActionType_EIT_PF_TUNE, hSvc, 0);

END_FUNC:
	if ((Handle_t)NULL != hRscReq)
	{
		SVC_PIPE_DestroyResourceRequestHandle (hRscReq);
	}

	return hResult;
}

STATIC HERROR mgr_action_SetupTranscodeRecord (HUINT32 ulActionId, HUINT32 ulPvrIdx, MgrAction_Priority_e ePriority)
{
	Handle_t				 hRscReq = (Handle_t)NULL;
	RxRscTrialResult_t		*pstTrialResult;
	HERROR					 hErr, hResult = eMgrRecFailed_UNKNOWN;

	/* Tuning Conflict �� ������ ������ Conflict üũ */
	hErr = mgr_action_CheckPossibleRecord (HANDLE_NULL, eMgrRecType_TRREC, ulPvrIdx);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("mgr_action_CheckPossibleRecord err\n");
		return hErr;
	}

	hRscReq = SVC_PIPE_MakeResourceRequestHandle (ulActionId, ePriority);
	if ((Handle_t)NULL == hRscReq)
	{
		HxLOG_Error ("SVC_PIPE_MakeResourceRequestHandle err: ActionId(%d), Priority(0x%x)\n", ulActionId, ePriority);
		goto END_FUNC;
	}

	(void)SVC_PIPE_AddRequiredResourceToRequestHandle (hRscReq, eRxRSCTYPE_DEMUX, RxRSCID_DYNAMIC, FALSE, eRxUSAGETYPE_DEMUX_PLAYBACK);
	(void)SVC_PIPE_AddRequiredResourceToRequestHandle (hRscReq, eRxRSCTYPE_HDD_REC, RxRSCID_DYNAMIC, FALSE, eRxUSAGETYPE_RECHDD_REC);

	(void)mgr_action_SetKillableActionToRscReqHandleByActionPriority (ulActionId, ePriority, hRscReq);

	hErr = SVC_PIPE_TryResourceRequest (hRscReq);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_PIPE_TryResourceRequest\n");
		goto END_FUNC;
	}

	pstTrialResult = SVC_PIPE_GetRequestTrialResult (hRscReq);
	if (NULL != pstTrialResult)
	{
		// �׿��� �Ѵٰ� ������ Action�� ������ �׿��� �����ؾ� �Ѵ�.
		mgr_action_KillActionsByTrialResult (pstTrialResult);
	}

	// �ڽ��� Proc�� �׿���...
	if (NULL != s_astActionContext[ulActionId].actionProc)
	{
		BUS_MGR_DestroyWithReason(s_astActionContext[ulActionId].actionProc, eDESTROY_BUS_ACTION, 0);
	}

	hErr = SVC_PIPE_SetResourcesByTrialResults (hRscReq);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_PIPE_SetResourcesByTrialResults err:\n");
		goto END_FUNC;
	}

	s_aulActionPriority[ulActionId] = ePriority;
	mgr_action_printResourceAllocation (ulActionId);

	// Action�� ���� �� ������ ���:
	(void)mgr_action_setActionSetupCount(ulActionId);

	hResult = eMgrRecFailed_NOFAIL;
	HxLOG_Debug("send eMEVT_ACTION_SETUP_SUCCEEDED, ulActionId: %d, actionHandle: %d, eActionType: eMgrRecType_TRREC\n", ulActionId, SVC_PIPE_GetActionHandle (ulActionId));
	BUS_SendMessage(NULL, eMEVT_ACTION_SETUP_SUCCEEDED, SVC_PIPE_GetActionHandle (ulActionId), eRecType_REC, HANDLE_NULL, eMgrRecType_TRREC);

END_FUNC:
	if ((Handle_t)NULL != hRscReq)
	{
		SVC_PIPE_DestroyResourceRequestHandle (hRscReq);
	}

	return hResult;
}

STATIC HERROR mgr_action_SetupScr2DescRecord (HUINT32 ulActionId, HUINT32 ulPvrIdx, MgrAction_Priority_e ePriority)
{
	Handle_t				 hRscReq = (Handle_t)NULL;
	RxRscTrialResult_t		*pstTrialResult;
	HERROR					 hErr, hResult = eMgrRecFailed_UNKNOWN;

	/* Tuning Conflict �� ������ ������ Conflict üũ */
	hErr = mgr_action_CheckPossibleRecord (HANDLE_NULL, eMgrRecType_SCR2DESC, ulPvrIdx);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("mgr_action_CheckPossibleRecord err\n");
		goto END_FUNC;
	}

	hRscReq = SVC_PIPE_MakeResourceRequestHandle (ulActionId, ePriority);
	if ((Handle_t)NULL == hRscReq)
	{
		HxLOG_Error ("SVC_PIPE_MakeResourceRequestHandle err: ActionId(%d), Priority(0x%x)\n", ulActionId, ePriority);
		goto END_FUNC;
	}

#if 0	// SCR2DESC ���۽� DEMUXID�� PB_DEMUXID �����.
	(void)SVC_PIPE_AddRequiredResourceToRequestHandle (hRscReq, eRxRSCTYPE_DEMUX, RxRSCID_DYNAMIC, FALSE, eRxUSAGETYPE_DEMUX_LIVEREC);
#endif
	(void)SVC_PIPE_AddRequiredResourceToRequestHandle (hRscReq, eRxRSCTYPE_HDD_REC, RxRSCID_DYNAMIC, FALSE, eRxUSAGETYPE_RECHDD_REC);

	(void)mgr_action_SetKillableActionToRscReqHandleByActionPriority (ulActionId, ePriority, hRscReq);

	hErr = SVC_PIPE_TryResourceRequest (hRscReq);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_PIPE_TryResourceRequest\n");
		goto END_FUNC;
	}

	pstTrialResult = SVC_PIPE_GetRequestTrialResult (hRscReq);
	if (NULL != pstTrialResult)
	{
		// �׿��� �Ѵٰ� ������ Action�� ������ �׿��� �����ؾ� �Ѵ�.
		mgr_action_KillActionsByTrialResult (pstTrialResult);
	}

	// �ڽ��� Proc�� �׿���...
	if (NULL != s_astActionContext[ulActionId].actionProc)
	{
		BUS_MGR_DestroyWithReason(s_astActionContext[ulActionId].actionProc, eDESTROY_BUS_ACTION, 0);
	}

	hErr = SVC_PIPE_SetResourcesByTrialResults (hRscReq);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_PIPE_SetResourcesByTrialResults err:\n");
		goto END_FUNC;
	}

	s_aulActionPriority[ulActionId] = ePriority;
	mgr_action_printResourceAllocation (ulActionId);

	// Action�� ���� �� ������ ���:
	(void)mgr_action_setActionSetupCount(ulActionId);

	hResult = eMgrRecFailed_NOFAIL;
	HxLOG_Debug("send eMEVT_ACTION_SETUP_SUCCEEDED, ulActionId: %d, actionHandle: %d, eMgrRecType_SCR2DESC\n", ulActionId, SVC_PIPE_GetActionHandle (ulActionId));
	BUS_SendMessage(NULL, eMEVT_ACTION_SETUP_SUCCEEDED, SVC_PIPE_GetActionHandle (ulActionId), eRecType_REC, HANDLE_NULL, eMgrRecType_SCR2DESC);

END_FUNC:
	if ((Handle_t)NULL != hRscReq)
	{
		SVC_PIPE_DestroyResourceRequestHandle (hRscReq);
	}

	return hResult;
}

STATIC HERROR mgr_action_SetupDelayedRecord (HUINT32 ulActionId, HUINT32 ulPvrIdx, MgrAction_Priority_e ePriority)
{
	/* Tuning Conflict �� ������ ������ Conflict üũ */
	s_astActionContext[ulActionId].actionType = eActionType_NONE;
	return ERR_OK;
}

STATIC HERROR mgr_action_SetupRecord (HUINT32 ulActionId, MgrAction_Type_e eActionType, Handle_t hSvc, MgrRec_RecordType_e eRecordType, HUINT32 ulPvrIdx, MgrAction_Priority_e ePriority)
{
	Handle_t				 hRscReq = (Handle_t)NULL;
	DbSvc_TsInfo_t			 stTsInfo;
	DxTuneParam_t 		 stChTuneInfo;
	RxRscTrialResult_t		*pstTrialResult;
	HERROR					 hErr, hResult = ERR_FAIL;

	hErr = DB_SVC_GetTsInfoByServiceHandle (hSvc, &stTsInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("DB_SVC_GetTsInfoByServiceHandle err\n");
		hResult = eMgrRecFailed_NONSERVICE;
		goto END_FUNC;
	}

	OTL_CONV_DbTuningInfo2DxTuneParam((const DbSvc_TsInfo_t *)&stTsInfo, &stChTuneInfo);

	hRscReq = SVC_PIPE_MakeResourceRequestHandle (ulActionId, ePriority);
	if ((Handle_t)NULL == hRscReq)
	{
		HxLOG_Error ("SVC_PIPE_MakeResourceRequestHandle err: ActionId(%d), Priority(0x%x)\n", ulActionId, ePriority);
		hResult = eMgrRecFailed_CONFLICT_TP;
		goto END_FUNC;
	}

	(void)mgr_action_SetRfInputAndTunerInfoToRscReqHandle (TRUE, TRUE, &stChTuneInfo, hRscReq);
	(void)SVC_PIPE_AddRequiredResourceToRequestHandle (hRscReq, eRxRSCTYPE_DEMUX, RxRSCID_DYNAMIC, FALSE, eRxUSAGETYPE_DEMUX_LIVEREC);
	(void)SVC_PIPE_AddRequiredResourceToRequestHandle (hRscReq, eRxRSCTYPE_HDD_REC, RxRSCID_DYNAMIC, FALSE, eRxUSAGETYPE_RECHDD_REC);

	(void)mgr_action_SetKillableActionToRscReqHandleByActionPriority (ulActionId, ePriority, hRscReq);

	hErr = SVC_PIPE_TryResourceRequest (hRscReq);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_PIPE_TryResourceRequest\n");
		hResult = eMgrRecFailed_CONFLICT_TP;
		goto END_FUNC;
	}

	pstTrialResult = SVC_PIPE_GetRequestTrialResult (hRscReq);
	if (NULL != pstTrialResult)
	{
		mgr_action_KillActionsByTrialResult (pstTrialResult);
	}

	// �� ActionID�� �ɸ� ���� ���δ�.
	if (NULL != s_astActionContext[ulActionId].actionProc)
	{
		BUS_MGR_DestroyWithReason(s_astActionContext[ulActionId].actionProc, eDESTROY_BUS_ACTION, 0);
	}

	hErr = SVC_PIPE_SetResourcesByTrialResults (hRscReq);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_PIPE_SetResourcesByTrialResults err:\n");
		hResult = eMgrRecFailed_UNKNOWN;
		goto END_FUNC;
	}

	s_aulActionPriority[ulActionId] = ePriority;
	mgr_action_printResourceAllocation (ulActionId);

	// Action�� ���� �� ������ ���:
	(void)mgr_action_setActionSetupCount(ulActionId);

	hResult = eMgrRecFailed_NOFAIL;
	HxLOG_Debug("send eMEVT_ACTION_SETUP_SUCCEEDED, ulActionId: %d, actionHandle: %d, eActionType: %d, hSvc: %d\n", ulActionId, SVC_PIPE_GetActionHandle (ulActionId), eActionType, hSvc);
	BUS_SendMessage(NULL, eMEVT_ACTION_SETUP_SUCCEEDED, SVC_PIPE_GetActionHandle (ulActionId), eActionType, hSvc, 0);

END_FUNC:
	if ((Handle_t)NULL != hRscReq)
	{
		SVC_PIPE_DestroyResourceRequestHandle (hRscReq);
	}

	return hResult;
}

// TSR_REC, Recording �� ���� resource �� üũ �� resouce �� �Ҵ��Ѵ�.
HERROR MGR_ACTION_SetupRecord (HUINT32 ulActionId, MgrAction_Type_e eActionType, Handle_t	hSvc, MgrRec_RecordType_e eRecordType, HUINT32 ulPvrIdx, MgrAction_Priority_e ePriority)
{
	if ((eActionType != eRecType_REC) && (eActionType != eRecType_TSR_REC) && (eActionType != eRecType_EXT_REC))
	{
		HxLOG_Error ("Invalid Action type[%s] for this action Id\n", MGR_ACTION_GetTypeStr(eActionType));
		return eMgrRecFailed_UNKNOWN;
	}

	if ((ulActionId < eActionId_REC_FIRST) || (ulActionId > eActionId_REC_LAST))
	{
		HxLOG_Error ("Invalid Action type[%s] Id[%d] for this action Id\n", MGR_ACTION_GetTypeStr(eActionType), ulActionId);
		return eMgrRecFailed_UNKNOWN;
	}

	BUS_SendMessage (NULL, eMEVT_ACTION_SETUP_REQUESTED, SVC_PIPE_GetActionHandle (ulActionId), eActionType, hSvc, 0);

	if (ePriority < MGR_ACTION_GetPriority (ulActionId))
	{
		HxLOG_Error ("Action not allowed for the priority: (previous=0x%x, current=0x%x)\n", s_astActionContext[ulActionId].actionPriority, ePriority);
		return eMgrRecFailed_UNKNOWN;
	}

	switch (eRecordType)
	{
	case eMgrRecType_SCR2DESC:
		return mgr_action_SetupScr2DescRecord (ulActionId, ulPvrIdx, ePriority);

	case eMgrRecType_TRREC:
		return mgr_action_SetupTranscodeRecord(ulActionId, ulPvrIdx, ePriority);

	case eMgrRecType_DELAYED:
		if (eRecType_TSR_REC == s_astActionContext[ulActionId].actionType)
		{
			return mgr_action_SetupDelayedRecord (ulActionId, ulPvrIdx, ePriority);
		}
		else if (eActionType_NONE != s_astActionContext[ulActionId].actionType)
		{
			HxLOG_Error ("Delayed Record shall be started in the TSR action (%d)\n", ulActionId);
			return eMgrRecFailed_UNKNOWN;
		}

		break;

	default:
		break;
	}

	return mgr_action_SetupRecord (ulActionId, eActionType, hSvc, eRecordType, ulPvrIdx, ePriority);
}

// Tsr Playback, Chase Playback, Normal Playback
HERROR MGR_ACTION_SetupPlayback (HUINT32 ulActionId, MgrAction_Type_e eActionType, MgrPb_PlayType_e ePlaybackType, MgrAction_Priority_e ePriority)
{
	Handle_t				 hRscReq = (Handle_t)NULL;
	RxRscTrialResult_t		*pstTrialResult;
	HERROR					 hErr, hResult = ERR_FAIL;

	if ((eActionType != eViewType_PB) && (eActionType != eViewType_TSR_PB) && (eActionType != eViewType_CHASE_PB))
	{
		HxLOG_Error ("Invalid Action type[%s] for this action Id\n", MGR_ACTION_GetTypeStr(eActionType));
		goto END_FUNC;
	}

	if (TRUE != MGR_ACTION_IsViewAction (ulActionId) && TRUE != MGR_ACTION_IsBackGroundAction (ulActionId))
	{
		HxLOG_Error ("Invalid Action Id [%d] for this action Id\n", ulActionId);
		goto END_FUNC;
	}

	BUS_SendMessage (NULL, eMEVT_ACTION_SETUP_REQUESTED, SVC_PIPE_GetActionHandle (ulActionId), eActionType, HANDLE_NULL, 0);

	if (ePriority < MGR_ACTION_GetPriority (ulActionId))
	{
		HxLOG_Error ("Action not allowed for the priority: (previous=0x%x, current=0x%x)\n", MGR_ACTION_GetPriority (ulActionId), ePriority);
		goto END_FUNC;
	}

	hRscReq = SVC_PIPE_MakeResourceRequestHandle (ulActionId, ePriority);
	if ((Handle_t)NULL == hRscReq)
	{
		HxLOG_Error ("SVC_PIPE_MakeResourceRequestHandle err: ActionId(%d), Priority(0x%x)\n", ulActionId, ePriority);
		goto END_FUNC;
	}

	(void)SVC_PIPE_AddRequiredResourceToRequestHandle (hRscReq, eRxRSCTYPE_DEMUX, RxRSCID_DYNAMIC, FALSE, eRxUSAGETYPE_DEMUX_PLAYBACK);
	(void)SVC_PIPE_AddRequiredResourceToRequestHandle (hRscReq, eRxRSCTYPE_PVR_PLAYER, RxRSCID_DYNAMIC, FALSE, eRxUSAGETYPE_NONE);
	(void)SVC_PIPE_AddRequiredResourceToRequestHandle (hRscReq, eRxRSCTYPE_AV_DEC, RxRSCID_DYNAMIC, FALSE, eRxUSAGETYPE_AVDEC_MAIN);

	(void)mgr_action_SetKillableActionToRscReqHandleByActionPriority (ulActionId, ePriority, hRscReq);

	hErr = SVC_PIPE_TryResourceRequest (hRscReq);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_PIPE_TryResourceRequest\n");
		goto END_FUNC;
	}

	pstTrialResult = SVC_PIPE_GetRequestTrialResult (hRscReq);
	if (NULL != pstTrialResult)
	{
		// �׿��� �Ѵٰ� ������ Action�� ������ �׿��� �����ؾ� �Ѵ�.
		mgr_action_KillActionsByTrialResult (pstTrialResult);
	}

	// �� ActionID�� �ɸ� ���� ���δ�.
	if (NULL != s_astActionContext[ulActionId].actionProc)
	{
		BUS_MGR_DestroyWithReason(s_astActionContext[ulActionId].actionProc, eDESTROY_BUS_ACTION, 0);
	}

	hErr = SVC_PIPE_SetResourcesByTrialResults (hRscReq);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_PIPE_SetResourcesByTrialResults err:\n");
		goto END_FUNC;
	}

	s_aulActionPriority[ulActionId] = ePriority;
	mgr_action_printResourceAllocation (ulActionId);

	// Action�� ���� �� ������ ���:
	(void)mgr_action_setActionSetupCount(ulActionId);

	hResult = ERR_OK;
	HxLOG_Debug("send eMEVT_ACTION_SETUP_SUCCEEDED, ulActionId: %d, actionHandle: %d, eActionType: %d\n", ulActionId, SVC_PIPE_GetActionHandle (ulActionId), eActionType);
	BUS_SendMessage(NULL, eMEVT_ACTION_SETUP_SUCCEEDED, SVC_PIPE_GetActionHandle (ulActionId), eActionType, HANDLE_NULL, 0);

END_FUNC:
	if ((Handle_t)NULL != hRscReq)
	{
		SVC_PIPE_DestroyResourceRequestHandle (hRscReq);
	}

	return hResult;
}

// TSR_REC, Instant_REC, Schedule_REC
HERROR mgr_action_CheckPossibleRecord(Handle_t	hSvc, MgrRec_RecordType_e eRecordType, HUINT32 ulPvrIdx)
{
	HBOOL			bFullStorage = TRUE;
	MgrAction_Type_e	eActType;
	HUINT32 		ulContentNum = 0;
	HERROR			err;
	HUINT32 		ulRecCnt = MGR_ACTION_GetRecordCount();
	HBOOL			bTsrEnable = MGR_PVR_GetTsrPlaybackEnable();
	HCHAR			szPath[FS_MAX_FILE_NAME_LEN];

	HxLOG_Info("[AP:AM(%d)] [mgr_action_CheckPossibleRecord] hSvc==%x, eRecordType=%d\n", MGR_ACTION_GetMainActionId(), hSvc, (HINT32)eRecordType);
	if (SVC_FS_GetPvrHddStatus(ulPvrIdx) == eDevStatus_OK)
	{
		SvcFs_DevIdx_e		 eDevIdx = eSVC_FS_DEV_NULL;

		err = SVC_FS_GetPvrDevIdPartIdx (ulPvrIdx, &eDevIdx, NULL);
		if (err == ERR_OK)
		{
			HxLOG_Info("check eDevIdx (%d)\n", eDevIdx);

			if (SVC_FS_IsDevicePvrPossible (eDevIdx) == TRUE)
			{
			HxLOG_Info("device possible for pvr\n");
				bFullStorage = FALSE;
			}
			else
			{
				HxLOG_Error("storage Full ! with SVC_FS_IsDevicePvrPossible() \n");
			}
		}
		else
		{
			HxLOG_Error("can't get the eDevIdx !!! with SVC_FS_GetPvrDevIdPartIdx() \n");
		}
	}
	else
	{
		HxLOG_Error("can't get the HDD !!! with SVC_FS_GetPvrHddStatus(%d) \n", ulPvrIdx);
		return eMgrRecFailed_NOHDD;
	}

	// Search ���� ���
	eActType = MGR_ACTION_GetType(MGR_ACTION_GetMainActionId());
	if(eActType == eActionType_SEARCH)
	{
		return eMgrRecFailed_SEARCHING;
	}

	// TODO: CI Plus Firmware Update. ���.

	// check file cout limit
	if(SVC_FS_GetPvrPath (ulPvrIdx, eSVC_FS_UsageType_Record, szPath) != ERR_OK)
	{
		HxLOG_Error("SVC_FS_GetPvrPath Error\n");
		return eMgrRecFailed_UNKNOWN;
	}

	if(MWC_UTIL_DvbStrcmp(MGR_MEDIA_GetVideoLoadedPath(), szPath) != 0)
	{
		err = SVC_META_GetContentsNumber(szPath, &ulContentNum);
		if(err != ERR_OK)
		{
			ulContentNum = 0;
		}
	}
	else
	{
		err = SVC_META_CountContentsIdList(&ulContentNum);
	}

	// Check Conflict
	switch(eRecordType)
	{
		case eMgrRecType_TSR:
			// TSR_REC �� Disable �̸� error return ��.
			if(bTsrEnable == FALSE)
			{
				HxLOG_Print("[mgr_action_CheckPossibleRecord] TSR_Rec Failed!!! TSR_DISABLE\n");
				return eMgrRecFailed_UNKNOWN;
			}
			break;

		case eMgrRecType_INSTANT:
		case eMgrRecType_SCR2DESC:
		case eMgrRecType_TRREC:
		case eMgrRecType_DELAYED:
		case eMgrRecType_FULLTSREC:
			if(bFullStorage)
			{
				HxLOG_Print("[mgr_action_CheckPossibleRecord] eMgrRecType_INSTANT/eMgrRecType_SCR2DESC/eMgrRecType_DELAYED Failed!!! FULL Storage \n");
				return eMgrRecFailed_HDDFULL;
			}
			if(ulRecCnt >= NUM_REC_ACTION)
			{
				HxLOG_Print("[mgr_action_CheckPossibleRecord] eMgrRecType_INSTANT/eMgrRecType_SCR2DESC/eMgrRecType_DELAYED Failed!!! NUM_REC_ACTION\n");
				return eMgrRecFailed_MAXNUM;
			}
			break;

		case eMgrRecType_TBR:
		case eMgrRecType_EBR:
		case eMgrRecType_SBR:
			if(bFullStorage)
			{
				HxLOG_Print("[mgr_action_CheckPossibleRecord] eMgrRecType_TBR/eMgrRecType_EBR/eMgrRecType_SBR Failed!!! FULL Storage \n");
				return eMgrRecFailed_HDDFULL;
			}
			if(ulRecCnt >= NUM_REC_ACTION)
			{
				HxLOG_Print("[mgr_action_CheckPossibleRecord] eMgrRecType_TBR/eMgrRecType_EBR/eMgrRecType_SBR Failed!!! NUM_REC_ACTION\n");
				return eMgrRecFailed_MAXNUM;
			}
			break;

		default:
			break;
	}

	return ERR_OK;
}

HUINT32 MGR_ACTION_GetAvailableRecordActionId(void)
{
	HUINT32 ulActionId;

	for(ulActionId = eActionId_REC_FIRST; ulActionId <= eActionId_REC_LAST; ulActionId++)
	{
		if(s_astActionContext[ulActionId].actionType == eActionType_NONE)
		{
			return ulActionId;
		}
	}

	return 0xffffffff;	// error
}

HUINT32 MGR_ACTION_GetRecordCount(void)
{
	HUINT32 ulActionId, ulRecCnt=0;

	for(ulActionId = eActionId_REC_FIRST; ulActionId <= eActionId_REC_LAST; ulActionId++)
	{
		if(s_astActionContext[ulActionId].actionType == eRecType_REC)
		{
			ulRecCnt++;
		}
	}

	return ulRecCnt;
}

HUINT32 MGR_ACTION_GetSATIPCount(void)
{
	HUINT32 ulActionId, ulSATIPCnt=0;

	for(ulActionId = eActionId_BG_FIRST; ulActionId <= eActionId_BG_LAST; ulActionId++)
	{
		if(s_astActionContext[ulActionId].actionType == eActionType_SATIP)
		{
			ulSATIPCnt++;
		}
	}

	return ulSATIPCnt;
}

HUINT32 MGR_ACTION_GetTranscoderCount(void)
{
	HUINT32 ulActionId, ulTrCnt=0;

	for(ulActionId = eActionId_BG_FIRST; ulActionId <= eActionId_BG_LAST; ulActionId++)
	{
		if ((s_astActionContext[ulActionId].actionType == eActionType_TRANSCODING_LIVE) ||
			(s_astActionContext[ulActionId].actionType == eActionType_TRANSCODING_PLAYBACK))
		{
			ulTrCnt++;
		}
	}

	return ulTrCnt;
}


// recording ���� �� index�� ���߾� �ش��ϴ� rec actionId�� ��ȯ.
HERROR	MGR_ACTION_GetRecordingActionIdByIdx(HINT32 nIdx, ActionId_t *pActionId)
{
	HUINT32 ulActionId = 0;
	HINT32	nCnt = 0;

	if(pActionId == NULL)
	{
		return ERR_FAIL;
	}

	for(ulActionId = eActionId_REC_FIRST; ulActionId <= eActionId_REC_LAST; ulActionId++)
	{
		if(s_astActionContext[ulActionId].actionType == eRecType_REC)
		{
			if(nCnt == nIdx)
			{
				*pActionId = ulActionId;

				return ERR_OK;
			}
			nCnt++;
		}
	}

	return ERR_FAIL;
}

HBOOL MGR_ACTION_IsRecordingActionId(HUINT32 ulActionId)
{
	if(ulActionId < eActionId_REC_FIRST || ulActionId > eActionId_REC_LAST)
	{
		return FALSE;
	}

	if(s_astActionContext[ulActionId].actionType == eRecType_REC)
	{
		return TRUE;
	}

	return FALSE;
}

#endif


#if defined(CONFIG_MW_MM_MEDIA_XVID)
HERROR MGR_ACTION_SetupMovie (HUINT32 ulActionId, MgrAction_Priority_e ePriority)
{
	Handle_t				 hRscReq = (Handle_t)NULL;
	RxRscTrialResult_t		*pstTrialResult;
	HERROR					 hErr, hResult = ERR_FAIL;

	if (TRUE != MGR_ACTION_IsViewAction (ulActionId))
	{
		HxLOG_Error ("Invalid Action Id [%d] for this action Id\n", ulActionId);
		goto END_FUNC;
	}

	BUS_SendMessage (NULL, eMEVT_ACTION_SETUP_REQUESTED, SVC_PIPE_GetActionHandle (ulActionId), eViewType_MOVIE_PB, HANDLE_NULL, 0);

	if (ePriority < MGR_ACTION_GetPriority (ulActionId))
	{
		HxLOG_Error ("Action not allowed for the priority: (previous=0x%x, current=0x%x)\n", MGR_ACTION_GetPriority (ulActionId), ePriority);
		goto END_FUNC;
	}

	if (eViewType_MOVIE_PB == MGR_ACTION_GetType(ulActionId))
	{
		HxLOG_Error ("Already ActionID(%d) Taken by actionType(%s). Use the action as is.\n", ulActionId, MGR_ACTION_GetTypeStr(eViewType_MOVIE_PB));

		s_aulActionPriority[ulActionId] = ePriority;
		hResult = ERR_OK;
		goto END_FUNC;
	}

	hRscReq = SVC_PIPE_MakeResourceRequestHandle (ulActionId, ePriority);
	if ((Handle_t)NULL == hRscReq)
	{
		HxLOG_Error ("SVC_PIPE_MakeResourceRequestHandle err: ActionId(%d), Priority(0x%x)\n", ulActionId, ePriority);
		goto END_FUNC;
	}

	(void)SVC_PIPE_AddRequiredResourceToRequestHandle (hRscReq, eRxRSCTYPE_DEMUX, RxRSCID_DYNAMIC, FALSE, eRxUSAGETYPE_DEMUX_PLAYBACK);
	(void)SVC_PIPE_AddRequiredResourceToRequestHandle (hRscReq, eRxRSCTYPE_AV_DEC, RxRSCID_DYNAMIC, FALSE, eRxUSAGETYPE_AVDEC_MAIN);

	(void)mgr_action_SetKillableActionToRscReqHandleByActionPriority (ulActionId, ePriority, hRscReq);

	hErr = SVC_PIPE_TryResourceRequest (hRscReq);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_PIPE_TryResourceRequest\n");
		goto END_FUNC;
	}

	pstTrialResult = SVC_PIPE_GetRequestTrialResult (hRscReq);
	if (NULL != pstTrialResult)
	{
		// �׿��� �Ѵٰ� ������ Action�� ������ �׿��� �����ؾ� �Ѵ�.
		mgr_action_KillActionsByTrialResult (pstTrialResult);
	}

	// �� ActionID�� �ɸ� ���� ���δ�.
	if (NULL != s_astActionContext[ulActionId].actionProc)
	{
		BUS_MGR_DestroyWithReason(s_astActionContext[ulActionId].actionProc, eDESTROY_BUS_ACTION, 0);
	}

	hErr = SVC_PIPE_SetResourcesByTrialResults (hRscReq);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_PIPE_SetResourcesByTrialResults err:\n");
		goto END_FUNC;
	}

	s_aulActionPriority[ulActionId] = ePriority;
	mgr_action_printResourceAllocation (ulActionId);

	// Action�� ���� �� ������ ���:
	(void)mgr_action_setActionSetupCount(ulActionId);

	hResult = ERR_OK;
	HxLOG_Debug("send eMEVT_ACTION_SETUP_SUCCEEDED, ulActionId: %d, actionHandle: %d, eViewType_MOVIE_PB\n", ulActionId, SVC_PIPE_GetActionHandle (ulActionId));
	BUS_SendMessage(NULL, eMEVT_ACTION_SETUP_SUCCEEDED, SVC_PIPE_GetActionHandle (ulActionId), eViewType_MOVIE_PB, HANDLE_NULL, 0);

END_FUNC:
	if ((Handle_t)NULL != hRscReq)
	{
		SVC_PIPE_DestroyResourceRequestHandle (hRscReq);
	}

	return hResult;
}
#endif

#if defined(CONFIG_MW_SI_HOME_CHANNEL)
HERROR MGR_ACTION_SetupHomeChannel (HUINT32 ulActionId, Handle_t hSvc, MgrAction_Priority_e ePriority)
{
	Handle_t				 hRscReq = (Handle_t)NULL;
	DbSvc_TsInfo_t			 stTsInfo;
	DxTuneParam_t			 stChTuneInfo;
	RxRscTrialResult_t		*pstTrialResult;
	HERROR					 hErr, hResult = ERR_FAIL;

	if (ePriority < MGR_ACTION_GetPriority (ulActionId))
	{
		HxLOG_Error ("Action not allowed for the priority: (previous=0x%x, current=0x%x)\n", MGR_ACTION_GetPriority (ulActionId), ePriority);
		goto END_FUNC;
	}

	hErr = DB_SVC_GetTsInfoByServiceHandle (hSvc, &stTsInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("DB_SVC_GetTsInfoByServiceHandle error\n");
		goto END_FUNC;
	}

	BUS_SendMessage (NULL, eMEVT_ACTION_SETUP_REQUESTED, SVC_PIPE_GetActionHandle (ulActionId), eActionType_HOMECHANNEL, hSvc, 0);

	if ((TRUE == MGR_ACTION_IsViewAction (ulActionId)) && (eActionType_HOMECHANNEL == MGR_ACTION_GetType (ulActionId)))
	{
		HxLOG_Error ("Already ActionID(%d) Taken by actionType(%s). Use the action as is.\n", ulActionId, MGR_ACTION_GetTypeStr(eActionType_HOMECHANNEL));

		s_aulActionPriority[ulActionId] = ePriority;
		hResult = ERR_OK;
		goto END_FUNC;
	}

	(void)OTL_CONV_DbTuningInfo2DxTuneParam((const DbSvc_TsInfo_t *)&stTsInfo, &stChTuneInfo);

	hRscReq = SVC_PIPE_MakeResourceRequestHandle (ulActionId, ePriority);
	if ((Handle_t)NULL == hRscReq)
	{
		HxLOG_Error ("SVC_PIPE_MakeResourceRequestHandle err: ActionId(%d), Priority(0x%x)\n", ulActionId, ePriority);
		goto END_FUNC;
	}

	(void)mgr_action_SetRfInputAndTunerInfoToRscReqHandle (TRUE, TRUE, &stChTuneInfo, hRscReq);
	(void)SVC_PIPE_AddRequiredResourceToRequestHandle (hRscReq, eRxRSCTYPE_DEMUX, RxRSCID_DYNAMIC, FALSE, eRxUSAGETYPE_DEMUX_LIVEREC);

	(void)mgr_action_SetKillableActionToRscReqHandleByActionPriority (ulActionId, ePriority, hRscReq);

	hErr = SVC_PIPE_TryResourceRequest (hRscReq);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_PIPE_TryResourceRequest\n");
		goto END_FUNC;
	}

	pstTrialResult = SVC_PIPE_GetRequestTrialResult (hRscReq);
	if (NULL != pstTrialResult)
	{
		// �׿��� �Ѵٰ� ������ Action�� ������ �׿��� �����ؾ� �Ѵ�.
		mgr_action_KillActionsByTrialResult (pstTrialResult);
	}

	// ���� Action ID�� ���� Action�� �־ �̰��� ���ش�.
	if ((eActionType_NONE != s_astActionContext[ulActionId].actionType) && (NULL != s_astActionContext[ulActionId].actionProc))
	{
		BUS_MGR_DestroyWithReason(s_astActionContext[ulActionId].actionProc, eDESTROY_BUS_ACTION, 0);
	}

	hErr = SVC_PIPE_SetResourcesByTrialResults (hRscReq);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_PIPE_SetResourcesByTrialResults err:\n");
		goto END_FUNC;
	}

	s_aulActionPriority[ulActionId] = ePriority;
	mgr_action_printResourceAllocation (ulActionId);

	// Action�� ���� �� ������ ���:
	(void)mgr_action_setActionSetupCount(ulActionId);

	hResult = ERR_OK;
	HxLOG_Debug("send eMEVT_ACTION_SETUP_SUCCEEDED, ulActionId: %d, actionHandle: %d, eActionType_HOMECHANNEL, hSvc: %d\n", ulActionId, SVC_PIPE_GetActionHandle (ulActionId), hSvc);
	BUS_SendMessage(NULL, eMEVT_ACTION_SETUP_SUCCEEDED, SVC_PIPE_GetActionHandle (ulActionId), eActionType_HOMECHANNEL, hSvc, 0);

END_FUNC:
	if ((Handle_t)NULL != hRscReq)
	{
		SVC_PIPE_DestroyResourceRequestHandle (hRscReq);
	}

	return hResult;
}
#endif


#if defined(CONFIG_MW_EPG_TVTV)
HERROR MGR_ACTION_SetupTvTv (HUINT32 ulActionId, Handle_t hSvc, DxTuneParam_t *pTuningInfo, MgrAction_Priority_e ePriority)
{
	Handle_t				 hRscReq = (Handle_t)NULL;
	DbSvc_TsInfo_t			 stTsInfo;
	DxTuneParam_t			 stChTuneInfo;
	RxRscTrialResult_t		*pstTrialResult;
	HERROR					 hErr, hResult = ERR_FAIL;

	BUS_SendMessage (NULL, eMEVT_ACTION_SETUP_REQUESTED, SVC_PIPE_GetActionHandle (ulActionId), eBgType_TVTV, hSvc, 0);

	if (TRUE != MGR_ACTION_IsBackGroundAction(ulActionId))
	{
		HxLOG_Error ("TVTVEPG Shall be executed only in the action\n");

		s_aulActionPriority[ulActionId] = ePriority;
		hResult = ERR_OK;
		goto END_FUNC;
	}

	if ((TRUE == MGR_ACTION_IsViewAction (ulActionId)) && (eActionType_HOMECHANNEL == MGR_ACTION_GetType (ulActionId)))
	{
		HxLOG_Error ("Already ActionID(%d) Taken by actionType(%s). Use the action as is.\n", ulActionId, MGR_ACTION_GetTypeStr(eActionType_CAS_TUNE));

		s_aulActionPriority[ulActionId] = ePriority;
		hResult = ERR_OK;
		goto END_FUNC;
	}

	if (HANDLE_NULL != hSvc)
	{
		hErr = DB_SVC_GetTsInfoByServiceHandle (hSvc, &stTsInfo);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("DB_SVC_GetTsInfoByServiceHandle error\n");
			goto END_FUNC;
		}

		OTL_CONV_DbTuningInfo2DxTuneParam((const DbSvc_TsInfo_t *)&stTsInfo, &stChTuneInfo);
	}
	else if (NULL != pTuningInfo)
	{
		HxSTD_memcpy (&stChTuneInfo, pTuningInfo, sizeof(DxTuneParam_t));
	}
	else
	{
		HxLOG_Error ("hSvc=HANDLE_NULL, pTuningInfo=NULL\n");
		goto END_FUNC;
	}

	if (ePriority < MGR_ACTION_GetPriority (ulActionId))
	{
		HxLOG_Error ("Action not allowed for the priority: (previous=0x%x, current=0x%x)\n", MGR_ACTION_GetPriority (ulActionId), ePriority);
		goto END_FUNC;
	}

	hRscReq = SVC_PIPE_MakeResourceRequestHandle (ulActionId, ePriority);
	if ((Handle_t)NULL == hRscReq)
	{
		HxLOG_Error ("SVC_PIPE_MakeResourceRequestHandle err: ActionId(%d), Priority(0x%x)\n", ulActionId, ePriority);
		goto END_FUNC;
	}

	(void)mgr_action_SetRfInputAndTunerInfoToRscReqHandle (TRUE, TRUE, &stChTuneInfo, hRscReq);
	(void)SVC_PIPE_AddRequiredResourceToRequestHandle (hRscReq, eRxRSCTYPE_DEMUX, RxRSCID_DYNAMIC, FALSE, eRxUSAGETYPE_DEMUX_LIVEREC);

	(void)mgr_action_SetKillableActionToRscReqHandleByActionPriority (ulActionId, ePriority, hRscReq);

	hErr = SVC_PIPE_TryResourceRequest (hRscReq);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_PIPE_TryResourceRequest\n");
		goto END_FUNC;
	}

	pstTrialResult = SVC_PIPE_GetRequestTrialResult (hRscReq);
	if (NULL != pstTrialResult)
	{
		// �׿��� �Ѵٰ� ������ Action�� ������ �׿��� �����ؾ� �Ѵ�.
		mgr_action_KillActionsByTrialResult (pstTrialResult);
	}

	// ���� Action ID�� ���� Action�� �־ �̰��� ���ش�.
	if ((eActionType_NONE != s_astActionContext[ulActionId].actionType) && (NULL != s_astActionContext[ulActionId].actionProc))
	{
		BUS_MGR_DestroyWithReason(s_astActionContext[ulActionId].actionProc, eDESTROY_BUS_ACTION, 0);
	}

	hErr = SVC_PIPE_SetResourcesByTrialResults (hRscReq);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_PIPE_SetResourcesByTrialResults err:\n");
		goto END_FUNC;
	}

	// Action�� ���� �� ������ ���:
	(void)mgr_action_setActionSetupCount(ulActionId);

	s_aulActionPriority[ulActionId] = ePriority;
	mgr_action_printResourceAllocation (ulActionId);

	hResult = ERR_OK;
	HxLOG_Debug("send eMEVT_ACTION_SETUP_SUCCEEDED, ulActionId: %d, actionHandle: %d, eBgType_TVTV, hSvc: %d\n", ulActionId, SVC_PIPE_GetActionHandle (ulActionId), hSvc);
	BUS_SendMessage(NULL, eMEVT_ACTION_SETUP_SUCCEEDED, SVC_PIPE_GetActionHandle (ulActionId), eBgType_TVTV, hSvc, 0);

END_FUNC:
	if ((Handle_t)NULL != hRscReq)
	{
		SVC_PIPE_DestroyResourceRequestHandle (hRscReq);
	}

	return hResult;
}
#endif

HERROR MGR_ACTION_SetupRFVOD(HUINT32 ulActionId, DxTuneParam_t *pstTuningInfo, MgrAction_Type_e eActionType, MgrAction_Priority_e ePriority)
{
	Handle_t				 hRscReq = (Handle_t)NULL;
	RxUsageType_e			 eAvDecUsage = eRxUSAGETYPE_AVDEC_MAIN;
	RxRscTrialResult_t		*pstTrialResult;
	HERROR					 hErr, hResult = ERR_FAIL;

	HxLOG_Info("MGR_ACTION_SetupRFVOD(actionId:%d)\n", ulActionId);

	if (TRUE != MGR_ACTION_IsViewAction (ulActionId))
	{
		HxLOG_Error ("Invalid Action Id [%d] for this action Id\n", ulActionId);
		goto END_FUNC;
	}

	BUS_SendMessage (NULL, eMEVT_ACTION_SETUP_REQUESTED, SVC_PIPE_GetActionHandle (ulActionId), eActionType, HANDLE_NULL, 0);

	if (ePriority < MGR_ACTION_GetPriority (ulActionId))
	{
		HxLOG_Error ("Action not allowed for the priority: (previous=0x%x, current=0x%x)\n", MGR_ACTION_GetPriority (ulActionId), ePriority);
		goto END_FUNC;
	}

	if (eActionType == MGR_ACTION_GetType(ulActionId))
	{
		HxLOG_Error ("Already ActionID(%d) Taken by actionType(%s). Use the action as is.\n", ulActionId, MGR_ACTION_GetTypeStr(eActionType));

		s_aulActionPriority[ulActionId] = ePriority;
		hResult = ERR_OK;
		goto END_FUNC;
	}

	eAvDecUsage = (eActionType != eViewType_PIP) ? eRxUSAGETYPE_AVDEC_MAIN : eRxUSAGETYPE_AVDEC_SUB;

	hRscReq = SVC_PIPE_MakeResourceRequestHandle (ulActionId, ePriority);
	if ((Handle_t)NULL == hRscReq)
	{
		HxLOG_Error ("SVC_PIPE_MakeResourceRequestHandle err: ActionId(%d), Priority(0x%x)\n", ulActionId, ePriority);
		goto END_FUNC;
	}

	(void)mgr_action_SetRfInputAndTunerInfoToRscReqHandle (TRUE, TRUE, pstTuningInfo, hRscReq);
	(void)SVC_PIPE_AddRequiredResourceToRequestHandle (hRscReq, eRxRSCTYPE_DEMUX, RxRSCID_DYNAMIC, FALSE, eRxUSAGETYPE_DEMUX_LIVEREC);
	(void)SVC_PIPE_AddRequiredResourceToRequestHandle (hRscReq, eRxRSCTYPE_AV_DEC, RxRSCID_DYNAMIC, FALSE, eAvDecUsage);

	(void)mgr_action_SetKillableActionToRscReqHandleByActionPriority (ulActionId, ePriority, hRscReq);

	hErr = SVC_PIPE_TryResourceRequest (hRscReq);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_PIPE_TryResourceRequest\n");
		goto END_FUNC;
	}

	pstTrialResult = SVC_PIPE_GetRequestTrialResult (hRscReq);
	if (NULL != pstTrialResult)
	{
		// �׿��� �Ѵٰ� ������ Action�� ������ �׿��� �����ؾ� �Ѵ�.
		mgr_action_KillActionsByTrialResult (pstTrialResult);
	}

	// �� ActionID�� �ɸ� ���� ���δ�.
	if (NULL != s_astActionContext[ulActionId].actionProc)
	{
		BUS_MGR_DestroyWithReason(s_astActionContext[ulActionId].actionProc, eDESTROY_BUS_ACTION, 0);
	}

	hErr = SVC_PIPE_SetResourcesByTrialResults (hRscReq);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_PIPE_SetResourcesByTrialResults err:\n");
		goto END_FUNC;
	}

	s_aulActionPriority[ulActionId] = ePriority;
	mgr_action_printResourceAllocation (ulActionId);

	// Action�� ���� �� ������ ���:
	(void)mgr_action_setActionSetupCount(ulActionId);

	hResult = ERR_OK;
	HxLOG_Debug("send eMEVT_ACTION_SETUP_SUCCEEDED, ulActionId: %d, actionHandle: %d, eActionType: %d\n", ulActionId, SVC_PIPE_GetActionHandle (ulActionId), eActionType);
	BUS_SendMessage(NULL, eMEVT_ACTION_SETUP_SUCCEEDED, SVC_PIPE_GetActionHandle (ulActionId), eActionType, HANDLE_NULL, 0);

END_FUNC:
	if ((Handle_t)NULL != hRscReq)
	{
		SVC_PIPE_DestroyResourceRequestHandle (hRscReq);
	}

	return hResult;
}


HERROR MGR_ACTION_SetupSatIp (HUINT32 ulActionId, DxTuneParam_t *pstTuningInfo, MgrAction_Priority_e ePriority, HBOOL bForce, HBOOL bTranscode)
{
	Handle_t				 hRscReq = (Handle_t)NULL;
	RxRscTrialResult_t		*pstTrialResult;
	HERROR					 hErr, hResult = ERR_FAIL;
	MgrAction_Priority_e	eSimulationPriority;

	if (NULL == pstTuningInfo)
	{
		HxLOG_Critical ("Tuning Info NULL:\n");
		return ERR_FAIL;
	}

	BUS_SendMessage (NULL, eMEVT_ACTION_SETUP_REQUESTED, SVC_PIPE_GetActionHandle (ulActionId), eActionType_SATIP, HANDLE_NULL, 0);

	if (ePriority < MGR_ACTION_GetPriority (ulActionId))
	{
		HxLOG_Error ("Action not allowed for the priority: (previous=0x%x, current=0x%x)\n", MGR_ACTION_GetPriority (ulActionId), ePriority);
		return ERR_FAIL;
	}

	if (bForce == TRUE)
	{
		eSimulationPriority = ePriority|ACTION_PRIORITY_LOWER_MASK; // aa ffff
	}
	else
	{
		eSimulationPriority = ePriority&ACTION_PRIORITY_UPPER_MASK; // aa 0000
	}

	hRscReq = SVC_PIPE_MakeResourceRequestHandle (ulActionId, eSimulationPriority);
	if ((Handle_t)NULL == hRscReq)
	{
		HxLOG_Error ("SVC_PIPE_MakeResourceRequestHandle err: ActionId(%d), Priority(0x%x)\n", ulActionId, ePriority);
		goto END_FUNC;
	}

	(void)mgr_action_SetRfInputAndTunerInfoToRscReqHandle (TRUE, TRUE, pstTuningInfo, hRscReq);
	(void)SVC_PIPE_AddRequiredResourceToRequestHandle (hRscReq, eRxRSCTYPE_DEMUX, RxRSCID_DYNAMIC, FALSE, eRxUSAGETYPE_DEMUX_LIVEREC);
	(void)SVC_PIPE_AddRequiredResourceToRequestHandle (hRscReq, eRxRSCTYPE_HDD_REC, RxRSCID_DYNAMIC, FALSE, eRxUSAGETYPE_RECHDD_SATIP);

	if(bTranscode == TRUE)
	{
		(void)SVC_PIPE_AddRequiredResourceToRequestHandle (hRscReq, eRxRSCTYPE_TRANSCODER, RxRSCID_DYNAMIC, FALSE, eRxUSAGETYPE_NONE);
	}

	//HxLOG_Warning(HxANSI_COLOR_YELLOW("ulActionId = %d, ePriority = %d\n"), ulActionId, ePriority);

	(void)mgr_action_SetKillableActionToRscReqHandleByActionPriority (ulActionId, eSimulationPriority, hRscReq);

	hErr = SVC_PIPE_TryResourceRequest (hRscReq);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_PIPE_TryResourceRequest\n");
		goto END_FUNC;
	}

	pstTrialResult = SVC_PIPE_GetRequestTrialResult (hRscReq);
	if (NULL != pstTrialResult)
	{
		mgr_action_KillActionsByTrialResult (pstTrialResult);
	}

	// ���� Action ID�� ���� Action�� �־ �̰��� ���ش�.
	if ((eActionType_NONE != s_astActionContext[ulActionId].actionType) && (NULL != s_astActionContext[ulActionId].actionProc))
	{
		BUS_MGR_DestroyWithReason(s_astActionContext[ulActionId].actionProc, eDESTROY_BUS_ACTION, 0);
	}

	hErr = SVC_PIPE_SetResourcesByTrialResults (hRscReq);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_PIPE_SetResourcesByTrialResults err:\n");
		goto END_FUNC;
	}

	s_aulActionPriority[ulActionId] = ePriority;
	mgr_action_printResourceAllocation (ulActionId);

	// Action�� ���� �� ������ ���:
	(void)mgr_action_setActionSetupCount(ulActionId);

	hResult = ERR_OK;
	HxLOG_Debug("send eMEVT_ACTION_SETUP_SUCCEEDED, ulActionId: %d, actionHandle: %d, eActionType: eActionType_SATIP\n", ulActionId, SVC_PIPE_GetActionHandle (ulActionId));
	BUS_SendMessage (NULL, eMEVT_ACTION_SETUP_SUCCEEDED, SVC_PIPE_GetActionHandle (ulActionId), eActionType_SATIP, HANDLE_NULL, 0);

END_FUNC:
	if ((Handle_t)NULL != hRscReq)
	{
		SVC_PIPE_DestroyResourceRequestHandle (hRscReq);
	}

	return hResult;
}

HERROR MGR_ACTION_SetupNtls (HUINT32 ulActionId, Handle_t hSvc, MgrAction_Priority_e ePriority)
{
	Handle_t				 hRscReq = (Handle_t)NULL;
	DbSvc_TsInfo_t			 stTsInfo;
	DxTuneParam_t			 stChTuneInfo;
	RxRscTrialResult_t		*pstTrialResult;
	HERROR					 hErr, hResult = ERR_FAIL;

	/* Service handle validity check */
	hErr = DB_SVC_CheckValidServiceHandle (hSvc);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("ulActionId(%d), hSvc(0x%08x), DB_SVC_CheckValidServiceHandle err:\n", ulActionId, hSvc);
		goto END_FUNC;
	}

	hErr = DB_SVC_GetTsInfoByServiceHandle (hSvc, &stTsInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("ulActionId(%d), hSvc(0x%08x), DB_SVC_GetTsInfoByServiceHandle err:\n", ulActionId, hSvc);
		goto END_FUNC;
	}

	BUS_SendMessage (NULL, eMEVT_ACTION_SETUP_REQUESTED, SVC_PIPE_GetActionHandle (ulActionId), eActionType_NTLS, hSvc, 0);


	if (ePriority < MGR_ACTION_GetPriority (ulActionId))
	{
		HxLOG_Error ("Action not allowed for the priority: (previous=0x%x, current=0x%x)\n", MGR_ACTION_GetPriority (ulActionId), ePriority);
		return ERR_FAIL;
	}

	OTL_CONV_DbTuningInfo2DxTuneParam((const DbSvc_TsInfo_t *)&stTsInfo, &stChTuneInfo);

	hRscReq = SVC_PIPE_MakeResourceRequestHandle (ulActionId, ePriority);
	if ((Handle_t)NULL == hRscReq)
	{
		HxLOG_Error ("SVC_PIPE_MakeResourceRequestHandle err: ActionId(%d), Priority(0x%x)\n", ulActionId, ePriority);
		goto END_FUNC;
	}

	(void)mgr_action_SetRfInputAndTunerInfoToRscReqHandle (TRUE, TRUE, &stChTuneInfo, hRscReq);
	(void)SVC_PIPE_AddRequiredResourceToRequestHandle (hRscReq, eRxRSCTYPE_DEMUX, RxRSCID_DYNAMIC, FALSE, eRxUSAGETYPE_DEMUX_LIVEREC);
	// TODO: SATIP/NTLS ������ ���� �ӽ� ����, �ٽ� eRxUSAGETYPE_RECHDD_SATIP ���� �����ؾ���. 2014-02-06
	// --> device id �ٽ� �������� ���� (2014-04-21)
#if 1
	(void)SVC_PIPE_AddRequiredResourceToRequestHandle (hRscReq, eRxRSCTYPE_HDD_REC, RxRSCID_DYNAMIC, FALSE, eRxUSAGETYPE_RECHDD_SATIP);
#else
	(void)SVC_PIPE_AddRequiredResourceToRequestHandle (hRscReq, eRxRSCTYPE_HDD_REC, RxRSCID_DYNAMIC, FALSE, eRxUSAGETYPE_RECHDD_REC);
#endif

	(void)mgr_action_SetKillableActionToRscReqHandleByActionPriority (ulActionId, ePriority, hRscReq);

	hErr = SVC_PIPE_TryResourceRequest (hRscReq);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_PIPE_TryResourceRequest\n");
		goto END_FUNC;
	}

	pstTrialResult = SVC_PIPE_GetRequestTrialResult (hRscReq);
	if (NULL != pstTrialResult)
	{
		mgr_action_KillActionsByTrialResult (pstTrialResult);
	}

	// ���� Action ID�� ���� Action�� �־ �̰��� ���ش�.
	if ((eActionType_NONE != s_astActionContext[ulActionId].actionType) && (NULL != s_astActionContext[ulActionId].actionProc))
	{
		BUS_MGR_DestroyWithReason(s_astActionContext[ulActionId].actionProc, eDESTROY_BUS_ACTION, 0);
	}

	hErr = SVC_PIPE_SetResourcesByTrialResults (hRscReq);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_PIPE_SetResourcesByTrialResults err:\n");
		goto END_FUNC;
	}

	s_aulActionPriority[ulActionId] = ePriority;
	mgr_action_printResourceAllocation (ulActionId);

	// Action�� ���� �� ������ ���:
	(void)mgr_action_setActionSetupCount(ulActionId);

	hResult = ERR_OK;
	HxLOG_Debug("send eMEVT_ACTION_SETUP_SUCCEEDED, ulActionId: %d, actionHandle: %d, eActionType: eActionType_NTLS, hSvc: %d\n", ulActionId, SVC_PIPE_GetActionHandle (ulActionId), hSvc);
	BUS_SendMessage (NULL, eMEVT_ACTION_SETUP_SUCCEEDED, SVC_PIPE_GetActionHandle (ulActionId), eActionType_NTLS, hSvc, 0);

END_FUNC:
	if ((Handle_t)NULL != hRscReq)
	{
		SVC_PIPE_DestroyResourceRequestHandle (hRscReq);
	}

	return hResult;
}

/* No actual effect. Just simulate */
#define _________Action_Trial_API_________
HERROR MGR_ACTION_TrySatIp (HUINT32 ulActionId, DxTuneParam_t *pstTuningInfo, MgrAction_Priority_e ePriority, HBOOL bForce, HBOOL bTranscode)
{
	Handle_t				hRscReq = (Handle_t)NULL;
	HERROR				hErr, hResult = ERR_FAIL;
	MgrAction_Priority_e	eSimulationPriority;

	if (NULL == pstTuningInfo)
	{
		HxLOG_Critical ("Tuning Info NULL:\n");
		return ERR_FAIL;
	}

	if (ePriority < MGR_ACTION_GetPriority (ulActionId))
	{
		HxLOG_Error ("Action not allowed for the priority: (previous=0x%x, current=0x%x)\n", MGR_ACTION_GetPriority (ulActionId), ePriority);
		return ERR_FAIL;
	}

	if (bForce == TRUE)
	{
		eSimulationPriority = ePriority|ACTION_PRIORITY_LOWER_MASK; // aa ffff
	}
	else
	{
		eSimulationPriority = ePriority&ACTION_PRIORITY_UPPER_MASK; // aa 0000
	}

	hRscReq = SVC_PIPE_MakeResourceRequestHandle (ulActionId, eSimulationPriority);
	if ((Handle_t)NULL == hRscReq)
	{
		HxLOG_Error ("SVC_PIPE_MakeResourceRequestHandle err: ActionId(%d), Priority(0x%x)\n", ulActionId, ePriority);
		goto END_FUNC;
	}

	(void)mgr_action_SetRfInputAndTunerInfoToRscReqHandle (TRUE, TRUE, pstTuningInfo, hRscReq);
	(void)SVC_PIPE_AddRequiredResourceToRequestHandle (hRscReq, eRxRSCTYPE_DEMUX, RxRSCID_DYNAMIC, FALSE, eRxUSAGETYPE_DEMUX_LIVEREC);
	(void)SVC_PIPE_AddRequiredResourceToRequestHandle (hRscReq, eRxRSCTYPE_HDD_REC, RxRSCID_DYNAMIC, FALSE, eRxUSAGETYPE_RECHDD_SATIP);
	//HxLOG_Warning(HxANSI_COLOR_YELLOW("ulActionId = %d, ePriority = %d\n"), ulActionId, ePriority);
	if (bTranscode == TRUE)
	{
		(void)SVC_PIPE_AddRequiredResourceToRequestHandle (hRscReq, eRxRSCTYPE_TRANSCODER, RxRSCID_DYNAMIC, FALSE, eRxUSAGETYPE_NONE);
	}

	(void)mgr_action_SetKillableActionToRscReqHandleByActionPriority (ulActionId, eSimulationPriority, hRscReq);

	hErr = SVC_PIPE_TryResourceRequest (hRscReq);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_PIPE_TryResourceRequest failed. \n");
		goto END_FUNC;
	}

	hResult = ERR_OK;

END_FUNC:
	if ((Handle_t)NULL != hRscReq)
	{
		SVC_PIPE_DestroyResourceRequestHandle (hRscReq);
	}

	return hResult;
}


#define ____FAST_CH_CHANGE_FUNCTION____
STATIC MgrAction_ViewInfo_t *mgr_action_getViewInfo (HUINT32 ulViewId)
{
	if (ulViewId < NUM_DISPLAY)
	{
		return &s_astActionViewInfo[ulViewId];
	}

	return NULL;
}

STATIC HERROR mgr_action_InitViewInfo (void)
{
	HUINT32 	i;

	HxSTD_MemSet(s_astActionViewInfo, 0, sizeof(MgrAction_ViewInfo_t) * NUM_DISPLAY);

	for (i = 0; i < NUM_DISPLAY; i++)
	{
		s_astActionViewInfo[i].ulActionId = i;
	}

	return ERR_OK;
}

HERROR MGR_ACTION_ResetViewInfo (void)
{
	return mgr_action_InitViewInfo();
}

HERROR MGR_ACTION_GetActionIdByViewId (HUINT32 ulViewId, HUINT32 *pulActionId)
{
	MgrAction_ViewInfo_t	*pstViewInfo = mgr_action_getViewInfo(ulViewId);

	if ((pstViewInfo == NULL) || (pulActionId == NULL))
	{
		HxLOG_Error("Invalid param. viewId(%d) pulActionId(%p) !! \n", ulViewId, pulActionId);
		return ERR_FAIL;
	}

	*pulActionId = pstViewInfo->ulActionId;
	return ERR_OK;
}

HERROR MGR_ACTION_GetViewIdByActionId (HUINT32 ulActionId, HUINT32 *pulViewid)
{
	HUINT32 	ulViewId;

	if ((ulActionId > eActionId_VIEW_LAST) || (pulViewid == NULL))
	{
		HxLOG_Error("Invalid param. actId(%d) !! \n", ulActionId);
		return ERR_FAIL;
	}

	for (ulViewId = 0; ulViewId < NUM_DISPLAY; ulViewId++)
	{
		MgrAction_ViewInfo_t	*pstViewInfo = mgr_action_getViewInfo(ulViewId);

		if ((NULL != pstViewInfo) && (pstViewInfo->ulActionId == ulActionId))
		{
			*pulViewid = ulViewId;
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

// ViewID�� Ư�� ActionID�� �Ҵ��� �ִ� ��� : FCC�� ���
HERROR MGR_ACTION_OverwriteViewIdTable (HUINT32 ulViewId, HUINT32 ulActionId)
{
	MgrAction_ViewInfo_t	*pstViewInfo = mgr_action_getViewInfo(ulViewId);

	if ((NULL == pstViewInfo) || (ulActionId < eActionId_VIEW_FIRST) || (ulActionId > eActionId_VIEW_LAST))
	{
		HxLOG_Error("Invalid param!! ulViewId(%d) actId(%d) !! \n", ulViewId, ulActionId);
		return ERR_FAIL;
	}

	pstViewInfo->ulActionId = ulActionId;
	return ERR_OK;
}

// ViewID 2������ Swap�ϴ� ��� : PIP� ���
HERROR MGR_ACTION_UpdateViewIdTable(HUINT32 ulViewId, HUINT32 ulActionId)
{
	HUINT32 	i;
	MgrAction_ViewInfo_t	*pstNewViewInfo = mgr_action_getViewInfo(ulViewId);
	MgrAction_ViewInfo_t	*pstOldViewInfo = NULL;
	MgrAction_ViewInfo_t	 stTmpViewInfo;

	if ((NULL == pstNewViewInfo) || (ulActionId > eActionId_VIEW_LAST))
	{
		HxLOG_Error("Invalid param!! ulViewId(%d) actId(%d) !! \n", ulViewId, ulActionId);
		return ERR_FAIL;
	}

	if (pstNewViewInfo->ulActionId == ulActionId)
	{
		// �̹� �ݿ��Ǿ� �ִ�..
		return ERR_OK;
	}

	//HxLOG_Print("Trace param - vid(%d) aid(%d) ", ulViewId, ulActionId);
	//HxLOG_Print("BEFORE :: (%d, %d, %d, %d) ", ulViewIdTable[0], ulViewIdTable[1], ulViewIdTable[2], ulViewIdTable[3]);

	// ���� action id�� ��� view id�� �Ҵ�Ǿ� �ִ��� ã��
	for (i = 0; i < NUM_VIEW_ACTION; i++)
	{
		if (i == ulViewId)					{ continue; }

		pstOldViewInfo = mgr_action_getViewInfo(i);
		if ((NULL != pstOldViewInfo) && (pstOldViewInfo->ulActionId == ulActionId))
		{
			break;
		}
	}

	// �� ã������ error
	if ((i >= NUM_VIEW_ACTION) || (NULL == pstOldViewInfo))
	{
		HxLOG_Error("not found source view id index..! ulViewId(%d) actId(%d)\n", ulViewId, ulActionId);
		return ERR_FAIL;
	}

	// ã������ swap~
	HxSTD_MemCopy(&stTmpViewInfo, pstOldViewInfo, sizeof(MgrAction_ViewInfo_t));
	HxSTD_MemCopy(pstOldViewInfo, pstNewViewInfo, sizeof(MgrAction_ViewInfo_t));
	HxSTD_MemCopy(pstNewViewInfo, &stTmpViewInfo, sizeof(MgrAction_ViewInfo_t));

	return ERR_OK;
}

#define ____DEBUG_FUNCTION____

const HINT8 *MGR_ACTION_MakeStrZapCheckResType(MgrAction_ZapCheckResult_e type)
{
	switch (type)
	{
		ENUM_TO_STR(eZAP_OK);
		ENUM_TO_STR(eZAP_ACTION_CHANGED);
		ENUM_TO_STR(eZAP_REC_CONFLICT);
		ENUM_TO_STR(eZAP_TUNER_CONFLICT);
		ENUM_TO_STR(eZAP_NOT_ALLOWED);
		ENUM_TO_STR(eZAP_ASSERT);
	}
	return ("Unknown");
}

STATIC HERROR mgr_action_printResourceAllocation (HUINT32 ulActionId)
{
#if 0
	HUINT32					 ulRscId;
	HERROR					 hErr;

	HLIB_CMD_Printf ("\n\n\n\n\n");
	HLIB_CMD_Printf ("============= Action (%d) Resources : =============\n", ulActionId);

	hErr = SVC_PIPE_GetResourceId (ulActionId, eRxRSCTYPE_RFINPUT, &ulRscId);
	if (ERR_OK == hErr)
	{
		HLIB_CMD_Printf ("	- RF-Input		: %d\n", ulRscId);
	}

	hErr = SVC_PIPE_GetResourceId (ulActionId, eRxRSCTYPE_TUNER_GROUP, &ulRscId);
	if (ERR_OK == hErr)
	{
		HLIB_CMD_Printf ("	- Tuner 		: %d\n", ulRscId);
	}

	hErr = SVC_PIPE_GetResourceId (ulActionId, eRxRSCTYPE_DEMUX, &ulRscId);
	if (ERR_OK == hErr)
	{
		HLIB_CMD_Printf ("	- Demux 		: %d\n", ulRscId);
	}

	hErr = SVC_PIPE_GetResourceId (ulActionId, eRxRSCTYPE_AV_DEC, &ulRscId);
	if (ERR_OK == hErr)
	{
		HLIB_CMD_Printf ("	- A/V Decoder   : %d\n", ulRscId);
	}

	hErr = SVC_PIPE_GetResourceId (ulActionId, eRxRSCTYPE_HDD_REC, &ulRscId);
	if (ERR_OK == hErr)
	{
		HLIB_CMD_Printf ("	- HDD-REC		: %d\n", ulRscId);
	}

	HLIB_CMD_Printf ("===================================================\n");
	HLIB_CMD_Printf ("\n\n\n\n\n");
#endif

	return ERR_OK;
}

#if defined(CONFIG_DEBUG)
static int mgr_action_PrintAction(void)
{

	HUINT32 i =0;
	HUINT32	mainActionId = 0;

	mainActionId = MGR_ACTION_GetMainActionId();
	HLIB_CMD_Printf("Main action Id: %d \n", mainActionId);

	for(i = 0; i < NUM_MAX_ACTION; i++)
	{
		HLIB_CMD_Printf("\n");
		if(s_astActionContext[i].actionType != eActionType_NONE)
		{
			HLIB_CMD_Printf("ActionId : %d ProcName : %16s[%x] ActionType : %16s priority : 0x%08x\n"
				, i, BUS_MGR_GetMgrName(BUS_MGR_Get(s_astActionContext[i].actionProc)), s_astActionContext[i].actionProc, MGR_ACTION_GetTypeStr(s_astActionContext[i].actionType), s_astActionContext[i].actionPriority, s_astActionContext[i]);
			HLIB_CMD_Printf("\tNextAction : %5s prevAction : %16s nextAction : %16s\n"
				,s_astActionContext[i].bNextAction?"TRUE":"FALSE", MGR_ACTION_GetTypeStr(s_astActionContext[i].prevActionType), MGR_ACTION_GetTypeStr(s_astActionContext[i].nextActionType));

		}
	}
	return 0;
}
#endif


void CMD_Register_mgr_ActionPrint(void)
{
#define	hCmdHandle		"mgr_action"

#ifdef CONFIG_DEBUG
	HLIB_CMD_RegisterWord(hCmdHandle,(void*)mgr_action_PrintAction,
			"print_action",	/* keyword */
			"action informations", /*	 help  */
			"print_action");	/*	usage  */
#endif
}


/*********************** End of File ******************************/
